#include "../qcommon/qcommon.h"
#include "../qcommon/cmd.h"
#include "com_files.h"
#include "dvar.h"
#include "../stringed/stringed_public.h"

// result buffers in storage order; the unreferenced blocks keep that layout
static char fs_unreferenced0[0x180];
static char fs_loadedIwdChecksums[BIG_INFO_STRING];
static char fs_loadedIwdNames[BIG_INFO_STRING];
static char fs_loadedIwdPureChecksums[BIG_INFO_STRING];
static char fs_referencedIwdChecksums[BIG_INFO_STRING];
static char fs_referencedIwdPureChecksums[BIG_INFO_STRING];
static char fs_unreferenced1[BIG_INFO_STRING];
static char fs_referencedIwdNames[BIG_INFO_STRING];
static char fs_shiftStrBuf[MAX_STRING_CHARS];
static char fs_mapBaseName[MAX_QPATH];

static char** Sys_ConcatenateFileLists( char **list0, char **list1, char **list2 );

static char** Sys_ConcatenateFileLists( char **list0, char **list1, char **list2 );
int FS_GetModList( char *listbuf, int bufsize );
void FS_SetRestrictions( void );

enum snd_stopsounds_arg_t
{
	SND_STOP_STREAMED = 8
};
void SND_StopSounds( snd_stopsounds_arg_t which );

qboolean FS_SV_FileExists( const char *file )
{
	FILE *f;
	char testpath[MAX_OSPATH];

	FS_BuildOSPath( fs_homepath->current.string, file, "", testpath );
	testpath[strlen( testpath ) - 1] = 0;

	f = FS_FileOpen( testpath, "rb" );

	if ( f )
	{
		FS_FileClose( f );
		return qtrue;
	}

	return qfalse;
}

int FS_SV_FOpenFileWrite( const char *filename )
{
	char ospath[MAX_OSPATH];
	fileHandle_t f;

	FS_CheckFileSystemStarted();

	FS_BuildOSPath( fs_homepath->current.string, filename, "", ospath );
	ospath[strlen( ospath ) - 1] = '\0';

	f = FS_HandleForFile(FS_THREAD_MAIN);
	fsh[f].zipFile = qfalse;

	if ( fs_debug->current.integer )
	{
		Com_Printf( "FS_SV_FOpenFileWrite: %s\n", ospath );
	}

	if ( FS_CreatePath( ospath ) )
	{
		return 0;
	}

	Com_DPrintf( "writing to: %s\n", ospath );
	fsh[f].handleFiles.file.o = FS_FileOpen( ospath, "wb" );

	Q_strncpyz( fsh[f].name, filename, sizeof( fsh[f].name ) );

	fsh[f].handleSync = qfalse;

	if ( !fsh[f].handleFiles.file.o )
	{
		f = 0;
	}

	return f;
}

int FS_SV_FOpenFileRead( const char *filename, fileHandle_t *fp )
{
	char ospath[MAX_OSPATH];
	fileHandle_t f = 0;

	FS_CheckFileSystemStarted();

	f = FS_HandleForFile(FS_THREAD_MAIN);
	fsh[f].zipFile = qfalse;

	Q_strncpyz( fsh[f].name, filename, sizeof( fsh[f].name ) );

	// search homepath
	FS_BuildOSPath( fs_homepath->current.string, filename, "", ospath );
	// remove trailing slash
	ospath[strlen( ospath ) - 1] = '\0';

	if ( fs_debug->current.integer )
	{
		Com_Printf( "FS_SV_FOpenFileRead (fs_homepath): %s\n", ospath );
	}

	fsh[f].handleFiles.file.o = FS_FileOpen( ospath, "rb" );
	fsh[f].handleSync = qfalse;

	if ( !fsh[f].handleFiles.file.o )
	{
		// NOTE TTimo on non *nix systems, fs_homepath == fs_basepath, might want to avoid
		if ( Q_stricmp( fs_homepath->current.string,fs_basepath->current.string ) )
		{
			// search basepath
			FS_BuildOSPath( fs_basepath->current.string, filename, "", ospath );
			ospath[strlen( ospath ) - 1] = '\0';

			if ( fs_debug->current.integer )
			{
				Com_Printf( "FS_SV_FOpenFileRead (fs_basepath): %s\n", ospath );
			}

			fsh[f].handleFiles.file.o = FS_FileOpen( ospath, "rb" );
			fsh[f].handleSync = qfalse;

			if ( !fsh[f].handleFiles.file.o )
			{
				f = 0;
			}
		}
	}

	if ( !fsh[f].handleFiles.file.o )
	{
		// search cd path
		FS_BuildOSPath( fs_cdpath->current.string, filename, "", ospath );
		ospath[strlen( ospath ) - 1] = '\0';

		if ( fs_debug->current.integer )
		{
			Com_Printf( "FS_SV_FOpenFileRead (fs_cdpath) : %s\n", ospath );
		}

		fsh[f].handleFiles.file.o = FS_FileOpen( ospath, "rb" );
		fsh[f].handleSync = qfalse;

		if ( !fsh[f].handleFiles.file.o )
		{
			f = 0;
		}
	}

	*fp = f;

	if ( f )
	{
		return FS_filelength( f );
	}

	return 0;
}

void FS_SV_Rename( const char *from, const char *to )
{
	char from_ospath[MAX_OSPATH];
	char to_ospath[MAX_OSPATH];

	FS_CheckFileSystemStarted();

	FS_BuildOSPath( fs_homepath->current.string, from, "", from_ospath );
	FS_BuildOSPath( fs_homepath->current.string, to, "", to_ospath );

	from_ospath[strlen( from_ospath ) - 1] = '\0';
	to_ospath[strlen( to_ospath ) - 1] = '\0';

	if ( fs_debug->current.integer )
	{
		Com_Printf( "FS_SV_Rename: %s --> %s\n", from_ospath, to_ospath );
	}

	if ( rename( from_ospath, to_ospath ) )
	{
		// Failed, try copying it and deleting the original
		FS_CopyFile( from_ospath, to_ospath );
		FS_Remove( from_ospath );
	}
}

char *FS_ShiftStr( const char *string, int shift )
{
	int i, l;

	l = strlen(string);
	for ( i = 0; i < l; i++ )
		fs_shiftStrBuf[i] = string[i] + shift;
	fs_shiftStrBuf[i] = '\0';

	return fs_shiftStrBuf;
}

// Original name unknown.
qboolean FS_FileMatchesDisk( const char *qpath, const char *ospath )
{
	void *fsData;
	int fsLen;
	FILE *f;
	int diskLen;
	byte *diskData;
	int i;
	qboolean different;

	fsLen = FS_ReadFile( qpath, &fsData );
	if ( fsLen == -1 )
		return 0;

	f = fopen( ospath, "rb" );
	different = qtrue;
	if ( f )
	{
		fseek( f, 0, SEEK_END );
		diskLen = ftell( f );
		fseek( f, 0, SEEK_SET );

		if ( diskLen > 0 )
		{
			diskData = (byte *)Z_Malloc( diskLen );
			fread( diskData, 1, diskLen, f );

			if ( diskLen == fsLen )
			{
				for ( i = 0; i < diskLen; i++ )
				{
					if ( diskData[i] != ((byte *)fsData)[i] )
						break;
				}
				if ( i == diskLen )
					different = qfalse;
			}
			free( diskData );
		}
		fclose( f );
	}

	FS_FreeFile( fsData );
	return !different;
}

int FS_Read2( void *buffer, int len, fileHandle_t f )
{
	FS_CheckFileSystemStarted();

	if ( !f )
		return 0;

	if ( fsh[f].streamed )
	{
		int r;

		fsh[f].streamed = qfalse;
		r = Sys_StreamedRead(buffer, len, 1, f);
		fsh[f].streamed = qtrue;
		return r;
	}

	return FS_Read(buffer, len, f);
}

/*
=======================
Sys_ConcatenateFileLists

mkv: Naive implementation. Concatenates three lists into a
	 new list, and frees the old lists from the heap.
bk001129 - from cvs1.17 (mkv)

FIXME TTimo those two should move to common.c next to Sys_ListFiles
=======================
 */
static unsigned int Sys_CountFileList( char **list )
{
	int i = 0;

	if ( list )
	{
		while ( *list )
		{
			list++;
			i++;
		}
	}
	return i;
}

static char** Sys_ConcatenateFileLists( char **list0, char **list1, char **list2 )
{
	int totalLength = 0;
	char** cat = NULL, **dst, **src;

	totalLength += Sys_CountFileList( list0 );
	totalLength += Sys_CountFileList( list1 );
	totalLength += Sys_CountFileList( list2 );

	/* Create new list. */
	dst = cat = (char **)Z_Malloc( ( totalLength + 1 ) * sizeof( char* ) );

	/* Copy over lists. */
	if ( list0 )
	{
		for ( src = list0; *src; src++, dst++ )
			*dst = *src;
	}
	if ( list1 )
	{
		for ( src = list1; *src; src++, dst++ )
			*dst = *src;
	}
	if ( list2 )
	{
		for ( src = list2; *src; src++, dst++ )
			*dst = *src;
	}

	// Terminate the list
	*dst = NULL;

	// Free our old lists.
	// NOTE: not freeing their content, it's been merged in dst and still being used
	if ( list0 )
	{
		Z_Free( list0 );
	}
	if ( list1 )
	{
		Z_Free( list1 );
	}
	if ( list2 )
	{
		Z_Free( list2 );
	}

	return cat;
}

/*
================
FS_GetModList

Returns a list of mod directory names
A mod directory is a peer to baseq3 with a pk3 in it
The directories are searched in base path, cd path and home path
================
*/
int FS_GetModList( char *listbuf, int bufsize )
{
	int nMods, i, j, nTotal, nLen, nPaks, nPotential, nDescLen;
	char **pFiles = NULL;
	char **pPaks = NULL;
	char *name;
	char path[MAX_OSPATH];
	char descPath[MAX_OSPATH];
	fileHandle_t descHandle;

	int dummy;
	char **pFiles0 = NULL;
	char **pFiles1 = NULL;
	char **pFiles2 = NULL;
	qboolean bDrop = qfalse;

	*listbuf = 0;
	nMods = nPotential = nTotal = 0;

	pFiles0 = Sys_ListFiles( fs_homepath->current.string, NULL, NULL, &dummy, qtrue );
	pFiles1 = Sys_ListFiles( fs_basepath->current.string, NULL, NULL, &dummy, qtrue );

	// DHM - Nerve :: Don't add blank paths (root)
	if ( fs_cdpath->current.string && strlen( fs_cdpath->current.string ) != 0 )
	{
		pFiles2 = Sys_ListFiles( fs_cdpath->current.string, NULL, NULL, &dummy, qtrue );
	}

	// we searched for mods in the three paths
	// it is likely that we have duplicate names now, which we will cleanup below
	pFiles = Sys_ConcatenateFileLists( pFiles0, pFiles1, pFiles2 );
	nPotential = Sys_CountFileList( pFiles );

	for ( i = 0 ; i < nPotential ; i++ )
	{
		name = pFiles[i];
		// NOTE: cleaner would involve more changes
		// ignore duplicate mod directories
		if ( i != 0 )
		{
			bDrop = qfalse;
			for ( j = 0; j < i; j++ )
			{
				if ( Q_stricmp( pFiles[j],name ) == 0 )
				{
					// this one can be dropped
					bDrop = qtrue;
					break;
				}
			}
		}

		// we drop "baseq3" "." and ".."
		if ( bDrop )
		{
			continue;
		}

		if ( I_strnicmp( name, ".", 1 ) )
		{
			// now we need to find some .pk3 files to validate the mod
			// NOTE TTimo: (actually I'm not sure why .. what if it's a mod under developement with no .pk3?)
			// we didn't keep the information when we merged the directory names, as to what OS Path it was found under
			//   so it could be in base path, cd path or home path
			//   we will try each three of them here (yes, it's a bit messy)
			// NOTE Arnout: what about dropping the current loaded mod as well?
			FS_BuildOSPath( fs_basepath->current.string, name, "", path );
			nPaks = 0;
			pPaks = Sys_ListFiles( path, "iwd", NULL, &nPaks, qfalse );
			Sys_FreeFileList( pPaks ); // we only use Sys_ListFiles to check wether .pk3 files are present

			/* Try on cd path */
			if ( nPaks <= 0 )
			{
				FS_BuildOSPath( fs_cdpath->current.string, name, "", path );
				nPaks = 0;
				pPaks = Sys_ListFiles( path, "iwd", NULL, &nPaks, qfalse );
				Sys_FreeFileList( pPaks );
			}

			/* try on home path */
			if ( nPaks <= 0 )
			{
				FS_BuildOSPath( fs_homepath->current.string, name, "", path );
				nPaks = 0;
				pPaks = Sys_ListFiles( path, "iwd", NULL, &nPaks, qfalse );
				Sys_FreeFileList( pPaks );
			}

			if ( nPaks > 0 )
			{
				nLen = strlen( name ) + 1;
				// nLen is the length of the mod path
				// we need to see if there is a description available
				strcpy( descPath, name );
				I_strncat( descPath, sizeof( descPath ), "/description.txt" );
				nDescLen = FS_SV_FOpenFileRead( descPath, &descHandle );
				if ( nDescLen > 0 && descHandle )
				{
					FILE *file;
					file = FS_FileForHandle( descHandle );
					Com_Memset( descPath, 0, sizeof( descPath ) );
					nDescLen = FS_FileRead( descPath, 1, 48, file );
					if ( nDescLen >= 0 )
					{
						descPath[nDescLen] = '\0';
					}
					FS_FCloseFile( descHandle );
				}
				else if ( !I_stricmp(name, BASEGAME) )
				{
					strcpy(descPath, "Call of Duty 2 Multiplayer");
				}
				else
				{
					strcpy(descPath, name);
				}
				nDescLen = strlen( descPath ) + 1;

				if ( nTotal + nLen + 1 + nDescLen + 1 < bufsize )
				{
					strcpy( listbuf, name );
					listbuf += nLen;
					strcpy( listbuf, descPath );
					listbuf += nDescLen;
					nTotal += nLen + nDescLen;
					nMods++;
				}
				else
				{
					break;
				}
			}
		}
	}
	Sys_FreeFileList( pFiles );

	return nMods;
}

static void FS_Dir_f( void )
{
	const char    *path;
	const char    *extension;
	char   		  **dirnames;
	int ndirs;
	int i;

	if ( Cmd_Argc() < 2 || Cmd_Argc() > 3 )
	{
		Com_Printf( "usage: dir <directory> [extension]\n" );
		return;
	}

	if ( Cmd_Argc() == 2 )
	{
		path = Cmd_Argv( 1 );
		extension = "";
	}
	else
	{
		path = Cmd_Argv( 1 );
		extension = Cmd_Argv( 2 );
	}

	Com_Printf( "Directory of %s %s\n", path, extension );
	Com_Printf( "---------------\n" );

	dirnames = FS_ListFiles( path, extension, FS_LIST_PURE_ONLY, &ndirs, 10 );

	for ( i = 0; i < ndirs; i++ )
	{
		Com_Printf( "%s\n", dirnames[i] );
	}

	FS_FreeFileList( dirnames, 10 );
}

static void FS_NewDir_f( void )
{
	const char	*filter;
	char	**dirnames;
	int		ndirs;
	int		i;

	if ( Cmd_Argc() < 2 )
	{
		Com_Printf( "usage: fdir <filter>\n" );
		Com_Printf( "example: fdir *q3dm*.bsp\n");
		return;
	}

	filter = Cmd_Argv( 1 );

	Com_Printf( "---------------\n" );

	dirnames = FS_ListFilteredFiles( fs_searchpaths, "", "", filter, FS_LIST_PURE_ONLY, &ndirs, 10 );

	FS_SortFileList(dirnames, ndirs);

	for ( i = 0; i < ndirs; i++ )
	{
		FS_ConvertPathInl(dirnames[i]);
		Com_Printf( "%s\n", dirnames[i] );
	}

	Com_Printf( "%d files listed\n", ndirs );
	FS_FreeFileList( dirnames, 10 );
}

static void FS_TouchFile_f( void )
{
	if ( Cmd_Argc() != 2 )
	{
		Com_Printf("Usage: touchFile <file>\n");
	}
	else
	{
		FS_TouchFile(Cmd_Argv(1));
	}
}

qboolean FS_iwIwd(char *iwd, const char *base)
{
	int i;
	char szFile[MAX_QPATH];
	char *pszLoc;

	for ( i = 0; i < NUM_IW_IWDS; ++i )
	{
		if ( !FS_FilenameCompare(iwd, va("%s/iw_%02d", base, i)) )
			return qtrue;
	}

	pszLoc = strstr(iwd, "localized_");

	if ( pszLoc )
	{
		strcpy(szFile, iwd);
		szFile[pszLoc - iwd + 10] = 0;

		if ( !FS_FilenameCompare(szFile, va("%s/localized_", base)) )
		{
			strcpy(szFile, pszLoc + 10);
			I_strlwrInl(szFile);

			for ( i = 0; i < NUM_IW_IWDS; ++i )
			{
				if ( strstr(szFile, va("_iw%02d", i)) )
					return qtrue;
			}
		}
	}

	return qfalse;
}

qboolean FS_CompareIwds( char *neededIwds, int len, qboolean dlstring )
{
	searchpath_t *sp;
	qboolean haveIwd;
	int i;
	char st[MAX_OSPATH];

	if ( !fs_numServerReferencedIwds )
	{
		return qfalse;
	}

	*neededIwds = 0;

	for ( i = 0; i < fs_numServerReferencedIwds; ++i )
	{
		haveIwd = qfalse;

		if ( FS_iwIwd((char *)fs_serverReferencedIwdNames[i], BASEGAME) )
			continue;

		for ( sp = fs_searchpaths; sp; sp = sp->next )
		{
			if ( sp->iwd && sp->iwd->checksum == fs_serverReferencedIwds[i] )
			{
				haveIwd = qtrue;
				break;
			}
		}

		if ( !haveIwd && fs_serverReferencedIwdNames[i] && *fs_serverReferencedIwdNames[i] )
		{
			if ( dlstring )
			{
				// Remote name
				I_strncat( neededIwds, len, "@" );
				I_strncat( neededIwds, len, fs_serverReferencedIwdNames[i] );
				I_strncat( neededIwds, len, ".iwd" );

				// Local name
				I_strncat( neededIwds, len, "@" );

				// Do we have one with the same name?
				if ( FS_SV_FileExists( va( "%s.iwd", fs_serverReferencedIwdNames[i] ) ) )
				{
					// Make sure the server cannot make us write to non-iwd files
					Com_sprintf( st, sizeof( st ), "%s.%08x.iwd", fs_serverReferencedIwdNames[i], fs_serverReferencedIwds[i] );
					I_strncat( neededIwds, len, st );
				}
				else
				{
					I_strncat( neededIwds, len, fs_serverReferencedIwdNames[i] );
					I_strncat( neededIwds, len, ".iwd" );
				}
			}
			else
			{
				I_strncat( neededIwds, len, fs_serverReferencedIwdNames[i] );
				I_strncat( neededIwds, len, ".iwd" );

				if ( FS_SV_FileExists( va( "%s.iwd", fs_serverReferencedIwdNames[i] ) ) )
				{
					I_strncat( neededIwds, len, " (local file exists with wrong checksum)" );
				}

				I_strncat( neededIwds, len, "\n" );
			}
		}
	}

	if ( *neededIwds )
	{
		Com_Printf( "Need iwds: %s\n", neededIwds );
		return qtrue;
	}

	return qfalse;
}

void FS_RemoveCommands()
{
	Cmd_RemoveCommand("path");
	Cmd_RemoveCommand("dir");
	Cmd_RemoveCommand("fdir");
	Cmd_RemoveCommand("touchFile");
}

void FS_AddCommands()
{
	Cmd_AddCommand("path", FS_Path_f);
	Cmd_AddCommand("fullpath", FS_FullPath_f);
	Cmd_AddCommand("dir", FS_Dir_f);
	Cmd_AddCommand("fdir", FS_NewDir_f);
	Cmd_AddCommand("touchFile", FS_TouchFile_f);
}

/*
===================
FS_SetRestrictions

Looks for product keys and restricts media add on ability
if the full version is not found
===================
*/
void FS_SetRestrictions( void )
{
	searchpath_t	*path;

#ifndef PRE_RELEASE_DEMO
	// if fs_restrict is set, don't even look for the id file,
	// which allows the demo release to be tested even if
	// the full game is present
	if ( !fs_restrict->current.boolean )
	{
		// look for the full game id

		// NO RESTRICTIONS IN RELEASE GAME
		return;
	}
#endif

	Dvar_SetBool( fs_restrict, true );

	Com_Printf( "\nRunning in restricted demo mode.\n\n" );

	// restart the filesystem with just the demo directory
	FS_Shutdown( 0 );
	FS_Startup( DEMOGAME );

	// make sure that the pak file has the header checksum we expect
	for ( path = fs_searchpaths ; path ; path = path->next )
	{
		// a tiny attempt to keep the checksum from being scannable from the exe
		if ( !FS_UseSearchPath(path) )
			continue;

		if ( path->localized && path->language != SEH_GetCurrentLanguage() )
			continue;

		if ( path->iwd && (path->iwd->checksum ^ 0x2261994) != -1277981599 )
		{
			Com_Error( ERR_FATAL, "Corrupted iw0.iwd: %u", path->iwd->checksum );
		}
	}
}

const char *FS_LoadedIwdChecksums()
{
	searchpath_t *search;

	fs_loadedIwdChecksums[0] = 0;

	for ( search = fs_searchpaths; search; search = search->next )
	{
		// is the element a iwd file?
		if ( !search->iwd )
			continue;

		if ( search->localized )
			continue;

		I_strncat( fs_loadedIwdChecksums, sizeof( fs_loadedIwdChecksums ), va( "%i ", search->iwd->checksum ) );
	}

	return fs_loadedIwdChecksums;
}

const char *FS_LoadedIwdNames()
{
	searchpath_t *search;

	fs_loadedIwdNames[0] = 0;

	for ( search = fs_searchpaths; search; search = search->next )
	{
		// is the element a iwd file?
		if ( !search->iwd )
			continue;

		if ( search->localized )
			continue;

		if ( *fs_loadedIwdNames )
			I_strncat( fs_loadedIwdNames, sizeof( fs_loadedIwdNames ), " " );

		I_strncat( fs_loadedIwdNames, sizeof( fs_loadedIwdNames ), search->iwd->iwdBasename );
	}

	return fs_loadedIwdNames;
}

const char *FS_LoadedIwdPureChecksums()
{
	searchpath_t *search;

	fs_loadedIwdPureChecksums[0] = 0;

	for ( search = fs_searchpaths; search; search = search->next )
	{
		// is the element a iwd file?
		if ( !search->iwd )
			continue;

		if ( search->localized )
			continue;

		I_strncat( fs_loadedIwdPureChecksums, sizeof( fs_loadedIwdPureChecksums ), va( "%i ", search->iwd->pure_checksum ) );
	}

	return fs_loadedIwdPureChecksums;
}

const char *FS_ReferencedIwdChecksums()
{
	searchpath_t *search;

	fs_referencedIwdChecksums[0] = 0;

	for ( search = fs_searchpaths ; search ; search = search->next )
	{
		// is the element a iwd file?
		if ( !search->iwd )
			continue;

		// is the element a iwd file and has it been referenced based on flag?
		if ( search->iwd->referenced || I_strnicmp( search->iwd->iwdGamename, BASEGAME, strlen( BASEGAME ) ) )
			I_strncat( fs_referencedIwdChecksums, sizeof( fs_referencedIwdChecksums ), va( "%i ", search->iwd->checksum ) );
	}

	return fs_referencedIwdChecksums;
}

const char *FS_ReferencedIwdNames()
{
	searchpath_t *search;

	fs_referencedIwdNames[0] = 0;

	// we want to return ALL iwd's from the fs_game path
	// and referenced one's from base
	for ( search = fs_searchpaths ; search ; search = search->next )
	{
		// is the element a iwd file?
		if ( !search->iwd )
			continue;

		if ( search->iwd->referenced || I_strnicmp( search->iwd->iwdGamename, BASEGAME, strlen( BASEGAME ) ) )
		{
			if ( *fs_referencedIwdNames )
				I_strncat( fs_referencedIwdNames, sizeof( fs_referencedIwdNames ), " " );
			I_strncat( fs_referencedIwdNames, sizeof( fs_referencedIwdNames ), search->iwd->iwdGamename );
			I_strncat( fs_referencedIwdNames, sizeof( fs_referencedIwdNames ), "/" );
			I_strncat( fs_referencedIwdNames, sizeof( fs_referencedIwdNames ), search->iwd->iwdBasename );
		}
	}

	return fs_referencedIwdNames;
}

const char *FS_ReferencedIwdPureChecksums()
{
	searchpath_t *search;
	int numIwds;
	int checksum;

	fs_referencedIwdPureChecksums[0] = 0;

	checksum = fs_checksumFeed;
	numIwds = 0;

	// add a delimiter between must haves and general refs
	fs_referencedIwdPureChecksums[strlen( fs_referencedIwdPureChecksums ) + 1] = '\0';
	fs_referencedIwdPureChecksums[strlen( fs_referencedIwdPureChecksums ) + 2] = '\0';
	fs_referencedIwdPureChecksums[strlen( fs_referencedIwdPureChecksums )] = '@';
	fs_referencedIwdPureChecksums[strlen( fs_referencedIwdPureChecksums )] = ' ';

	for ( search = fs_searchpaths; search; search = search->next )
	{
		// is the element a iwd file?
		if ( !search->iwd )
			continue;

		if ( search->localized )
			continue;

		if ( search->iwd->referenced )
		{
			I_strncat( fs_referencedIwdPureChecksums, sizeof( fs_referencedIwdPureChecksums ), va( "%i ", search->iwd->pure_checksum ) );
			checksum ^= search->iwd->pure_checksum;
			++numIwds;
		}
	}

	if ( fs_fakeChkSum )
	{
		I_strncat( fs_referencedIwdPureChecksums, sizeof( fs_referencedIwdPureChecksums ), va( "%i ", fs_fakeChkSum ) );
	}

	checksum ^= numIwds;
	I_strncat( fs_referencedIwdPureChecksums, sizeof( fs_referencedIwdPureChecksums ), va( "%i ", checksum ) );

	return fs_referencedIwdPureChecksums;
}

void FS_PureServerSetLoadedIwds(const char *paksums, const char *paknames)
{
	int j, k;
	int numPakSums;
	int numPakNames;
	int lpakSums[MAX_IWDFILES];
	char *lpakNames[MAX_IWDFILES];

	Cmd_TokenizeString(paksums);

	numPakSums = Cmd_Argc();

	if ( numPakSums > MAX_IWDFILES )
	{
		numPakSums = MAX_IWDFILES;
	}

	for ( j = 0; j < numPakSums; ++j )
	{
		lpakSums[j] = atoi( Cmd_Argv( j ) );
	}

	Cmd_TokenizeString(paknames);

	numPakNames = Cmd_Argc();

	if ( numPakNames > MAX_IWDFILES )
	{
		numPakNames = MAX_IWDFILES;
	}

	for ( j = 0; j < numPakNames; ++j )
	{
		lpakNames[j] = CopyString( Cmd_Argv( j ) );
	}

	if ( numPakSums != numPakNames )
	{
		Com_Error(ERR_DROP, "iwd sum/name mismatch");
	}

	if ( numPakSums == fs_numServerIwds )
	{
		for ( j = 0; j < numPakSums; ++j )
		{
			for ( k = 0; k < fs_numServerIwds; ++k )
			{
				if ( lpakSums[j] == fs_serverIwds[k] && !Q_stricmp(lpakNames[j], fs_serverIwdNames[k]) )
					goto foundIwd;
			}

			goto reload;

foundIwd:
			;
		}

		for ( j = 0; j < numPakNames; ++j )
		{
			Z_Free(lpakNames[j]);
		}

		return;
	}

reload:
	SND_StopSounds(SND_STOP_STREAMED);
	FS_ShutdownServerIwdNames();
	fs_numServerIwds = numPakSums;

	if ( fs_numServerIwds )
	{
		Com_DPrintf("Connected to a pure server.\n");
		Com_Memcpy(fs_serverIwds, lpakSums, sizeof(char*) * fs_numServerIwds);
		Com_Memcpy(fs_serverIwdNames, lpakNames, sizeof(char*) * fs_numServerIwds);
		fs_fakeChkSum = 0;
	}
}

void FS_PureServerSetReferencedIwds( const char *iwdSums, const char *iwdNames )
{
	int i;
	int numIwdSums;
	int numIwdNames;

	Cmd_TokenizeString(iwdSums);
	numIwdSums = Cmd_Argc();

	if ( numIwdSums > MAX_IWDFILES )
	{
		numIwdSums = MAX_IWDFILES;
	}

	FS_ShutdownServerReferencedIwds();

	for ( i = 0; i < numIwdSums; i++ )
	{
		fs_serverReferencedIwds[i] = atoi( Cmd_Argv( i ) );
	}

	if ( iwdNames && *iwdNames )
	{
		Cmd_TokenizeString(iwdNames);
		numIwdNames = Cmd_Argc();

		if ( numIwdNames > MAX_IWDFILES )
		{
			numIwdNames = MAX_IWDFILES;
		}

		if ( numIwdSums != numIwdNames )
		{
			Com_Error(ERR_DROP, "iwd sum/name mismatch");
		}

		for ( i = 0; i < numIwdNames; i++ )
		{
			fs_serverReferencedIwdNames[i] = CopyString( Cmd_Argv( i ) );
		}
	}
	else if ( numIwdSums )
	{
		Com_Error(ERR_DROP, "iwd sum/name mismatch");
	}

	fs_numServerReferencedIwds = numIwdSums;
}

char *FS_GetMapBaseName(const char *mapname)
{
	int len;
	int c;

	if ( !I_strnicmp(mapname, "maps/mp/", 8) )
	{
		mapname += 8;
	}

	len = strlen(mapname);

	if ( !strcasecmp(&mapname[len - 3], "bsp") )
	{
		len -= 7;
	}

	memcpy(fs_mapBaseName, mapname, len);
	fs_mapBaseName[len] = 0;

	for ( c = 0; c < len; ++c )
	{
		if ( fs_mapBaseName[c] == '%' )
		{
			fs_mapBaseName[c] = '_';
		}
	}

	return fs_mapBaseName;
}


// Unreferenced storage; original declarations unknown (sized from the layout).
static int unusedStorage;
