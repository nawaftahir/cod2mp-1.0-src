#include <sys/stat.h>
#include "../qcommon/qcommon.h"
#include "../qcommon/cmd.h"
#include "com_files.h"
#include "dvar.h"
#include "../stringed/stringed_public.h"

#undef I_strlen
int I_strlen(const char *s);

int com_fileAccessed = 0;
static int fs_loadStack;
char fs_gamedir[MAX_OSPATH];

dvar_t *fs_debug;
dvar_t *fs_homepath;
dvar_t *fs_basepath;
static dvar_t *fs_basegame;
static dvar_t *fs_useOldAssets;
dvar_t *fs_cdpath;
dvar_t *fs_copyfiles;
static dvar_t *fs_gameDirVar;
dvar_t *fs_restrict;
dvar_t *fs_ignoreLocalized;

searchpath_t *fs_searchpaths = NULL;
static int fs_packFiles;
int fs_fakeChkSum;
int fs_checksumFeed;
fileHandleData_t fsh[MAX_FILE_HANDLES];

int fs_numServerIwds = 0;
int fs_serverIwds[MAX_IWDFILES];
const char *fs_serverIwdNames[MAX_IWDFILES];
int fs_numServerReferencedIwds;
int fs_serverReferencedIwds[MAX_IWDFILES];
const char *fs_serverReferencedIwdNames[MAX_IWDFILES];

static char lastValidBase[MAX_OSPATH];
static char lastValidGame[MAX_OSPATH];

#define MAX_FOUND_FILES 0x1000

void FS_Remove( const char *osPath );

qboolean FS_SV_FileExists( const char *file );

void FS_BuildOSPath(const char *base, const char *game, const char *qpath, char* ospath);
int FS_FOpenFileRead(const char *filename, fileHandle_t *file, qboolean uniqueFILE);
char** FS_ListFiles(const char* path, const char* extension, FsListBehavior behavior, int* numfiles, int allocType);
char **FS_ListFilteredFilesOfDirType( const char *path, const char *extension, const char *filter, FsListBehavior behavior, int *numfiles, int dirTypes, int flags );

int FS_LanguageHasAssets( int language );
void FS_BuildOSPath(const char *base, const char *game, const char *qpath, char* ospath);
void FS_Remove( const char *osPath );
qboolean FS_FileExists( const char *file );
void FS_Rename( const char *from, const char *to );
qboolean FS_FileCompare( const char *filename1, const char *filename2 );
char *FS_ShiftedStrStr( const char *string, const char *substring, int shift );
int FS_FOpenFileRead(const char *filename, fileHandle_t *file, qboolean uniqueFILE);
const char *FS_ShortOSFilePath( const char *qpath );
qboolean FS_SetWritable( const char *filename, int writable );
int FS_SeekInternal( int f, int offset, int origin );
int FS_GetOSPath( const char *qpath, char *ospath );
fileHandle_t FS_OpenFileOverwrite( const char *qpath );
char **FS_ListFilteredFiles(searchpath_t *searchPath, const char *path, const char *extension, const char *filter, FsListBehavior behavior, int *numfiles, int allocType);
char **FS_ListFilesOfDirType( const char *path, const char *extension, FsListBehavior behavior, int *numfiles, int dirTypes, int flags );
bool FS_IsBaseGameDirOfType( const char *dir, int dirTypes );
char **FS_ListFilteredFilesOfDirType( const char *path, const char *extension, const char *filter, FsListBehavior behavior, int *numfiles, int dirTypes, int flags );
int FS_ConditionalRestart( int checksumFeed );
int FS_FTell( int f );
const char *GetBspExtension();

enum snd_stopsounds_arg_t
{
	SND_STOP_STREAMED = 8
};
void SND_StopSounds( snd_stopsounds_arg_t which );
void SEH_Init_StringEd();
void SEH_Shutdown_StringEd();
void SEH_UpdateLanguageInfo();

void Com_ReadCDKey( void );

/*
==============
FS_Remove
==============
*/
void FS_Remove( const char *osPath )
{
	remove(osPath);
}

qboolean FS_Initialized()
{
	return fs_searchpaths != NULL;
}

void FS_CheckFileSystemStarted()
{
	assert(fs_searchpaths);
}

static int FS_IwdIsPure(const iwd_t *iwd)
{
	int i;

	if ( fs_numServerIwds )
	{
		for ( i = 0; i < fs_numServerIwds; i++ )
		{
			if ( iwd->checksum == fs_serverIwds[i] )
				return 1;
		}

		return 0;
	}

	return 1;
}

int FS_LoadStack()
{
	return fs_loadStack;
}

qboolean FS_UseSearchPath(const searchpath_t *pSearch)
{
	if ( pSearch->localized && fs_ignoreLocalized->current.boolean )
	{
		return qfalse;
	}

	return qtrue;
}

// retail 0x420150, unreferenced; name unknown (Kisak FS_UseSearchPath body)
qboolean FS_UseSearchPathLanguage_420150(const searchpath_t *pSearch)
{
	if ( pSearch->localized && fs_ignoreLocalized->current.boolean )
	{
		return qfalse;
	}

	if ( pSearch->localized && pSearch->language != SEH_GetCurrentLanguage() )
	{
		return qfalse;
	}

	return qtrue;
}

/*
==============
FS_LanguageHasAssets
==============
*/
int FS_LanguageHasAssets( int language )
{
	searchpath_t *search;

	for ( search = fs_searchpaths; search; search = search->next )
	{
		if ( search->localized && search->language == language )
			return true;
	}

	return false;
}

long FS_HashFileName( const char *fname, int hashSize )
{
	int i;
	long hash;
	int letter;

	hash = 0;
	i = 0;
	while ( fname[i] != '\0' )
	{
		letter = tolower( fname[i] );
		if ( letter == '.' )
		{
			break;
		}
		if ( letter == '\\' )
		{
			letter = '/';
		}
		if ( letter == PATH_SEP )
		{
			letter = '/';
		}
		hash += letter * ( i + 119 );
		i++;
	}
	hash = ( hash ^ ( hash >> 10 ) ^ ( hash >> 20 ) );
	hash &= ( hashSize - 1 );
	return hash;
}

fileHandle_t FS_HandleForFile(FsThread thread)
{
	int i;
	int first;
	int count;

	if ( thread )
	{
		first = 51;
		count = 13;
	}
	else
	{
		first = 1;
		count = 50;
	}

	for (i = 0; i < count; ++i)
	{
		if (fsh[first + i].handleFiles.file.o == NULL)
		{
			return first + i;
		}
	}

	for (i = 1; i < MAX_FILE_HANDLES; ++i)
	{
		Com_Printf("FILE %2i: '%s'\n", i, fsh[i].name);
	}

	Com_Error(ERR_DROP, "\x15" "FS_HandleForFile: none free");
	return -1;
}

FILE *FS_FileForHandle( fileHandle_t f )
{
	return fsh[f].handleFiles.file.o;
}

int FS_filelength( fileHandle_t f )
{
	int pos;
	int end;
	FILE*   h;
	unz_s *zfile;

	FS_CheckFileSystemStarted();

	if (fsh[f].zipFile)
	{
		zfile = (unz_s*)fsh[f].handleFiles.file.z;
		return zfile->cur_file_info.uncompressed_size;
	}

	h = FS_FileForHandle( f );

	pos = ftell( h );
	FS_FileSeek( h, 0, SEEK_END );
	end = ftell( h );
	FS_FileSeek( h, pos, SEEK_SET );

	return end;
}

static void FS_ReplaceSeparators(char *path)
{
	char *src;
	char *dst;
	bool wasSep = false;

	src = path;
	dst = path;

	while ( *src )
	{
		if ( *src == '/' || *src == '\\' )
		{
			if ( !wasSep )
			{
				wasSep = true;
				*dst++ = PATH_SEP;
			}
		}
		else
		{
			wasSep = false;
			*dst = *src;
			dst++;
		}
		++src;
	}
	*dst = 0;
}

static void FS_BuildOSPath_Internal(const char *base, const char *game, const char *qpath, char *ospath, FsThread thread)
{
	unsigned int lenBase;
	unsigned int lenGame;
	unsigned int lenQpath;

	if ( !game || !game[0] )
		game = fs_gamedir;

	lenBase = I_strlen(base);
	lenGame = I_strlen(game);
	lenQpath = I_strlen(qpath);

	if ((int)(lenBase + lenGame + 1 + lenQpath + 1) >= MAX_OSPATH)
	{
		if (thread)
		{
			*ospath = 0;
			return;
		}

		Com_Error(ERR_FATAL, "\x15" "FS_BuildOSPath: os path length exceeded\n");
	}

	memcpy(ospath, base, lenBase);
	ospath[lenBase] = '/';

	memcpy(&ospath[lenBase + 1], game, lenGame);
	ospath[lenBase + 1 + lenGame] = '/';

	memcpy(ospath + lenBase + lenGame + 2, qpath, lenQpath + 1);
	FS_ReplaceSeparators(ospath);
}

void FS_BuildOSPath(const char *base, const char *game, const char *qpath, char* ospath)
{
	FS_BuildOSPath_Internal(base, game, qpath, ospath, FS_THREAD_MAIN);
}

int FS_CreatePath(char *OSPath)
{
	char	*ofs;

	// make absolutely sure that it can't back up the path
	// FIXME: is c: allowed???
	if ( strstr( OSPath, ".." ) || strstr( OSPath, "::" ) )
	{
		Com_Printf( "WARNING: refusing to create relative path \"%s\"\n", OSPath );
		return qtrue;
	}

	for (ofs = OSPath+1; *ofs ; ofs++)
	{
		if (*ofs == PATH_SEP)
		{
			// create the directory
			*ofs = 0;
			Sys_Mkdir (OSPath);
			*ofs = PATH_SEP;
		}
	}

	return qfalse;
}

void FS_CopyFile(char *fromOSPath, char *toOSPath)
{
	FILE *f;
	size_t len;
	char *buf;

	f = FS_FileOpen(fromOSPath, "rb");

	if (!f)
	{
		return;
	}

	FS_FileSeek(f, 0, SEEK_END);
	len = ftell(f);
	FS_FileSeek(f, 0, SEEK_SET);
	buf = (char *)malloc(len);

	if (FS_FileRead(buf, 1, len, f) != len)
	{
		Com_Error(ERR_FATAL, "\x15" "Short read in FS_CopyFile()\n");
	}

	FS_FileClose(f);

	if (FS_CreatePath(toOSPath))
	{
		free(buf);
		return;
	}

	f = FS_FileOpen(toOSPath, "wb");

	if (!f)
	{
		free(buf);
		return;
	}

	if (FS_FileWrite(buf, 1, len, f) != len)
	{
		Com_Error(ERR_FATAL, "\x15" "Short write in FS_CopyFile()\n");
	}

	FS_FileClose(f);
	free(buf);
}

/*
==============
FS_FileExists
==============
*/
qboolean FS_FileExists( const char *file )
{
	FILE *f;
	char testpath[MAX_OSPATH];

	FS_BuildOSPath(fs_homepath->current.string, fs_gamedir, file, testpath);
	f = FS_FileOpen(testpath, "rb");

	if ( f )
	{
		FS_FileClose(f);
		return qtrue;
	}

	return qfalse;
}

/*
==============
FS_Rename
==============
*/
void FS_Rename( const char *from, const char *to )
{
	char from_ospath[MAX_OSPATH];
	char to_ospath[MAX_OSPATH];

	FS_CheckFileSystemStarted();
	FS_BuildOSPath(fs_homepath->current.string, fs_gamedir, from, from_ospath);
	FS_BuildOSPath(fs_homepath->current.string, fs_gamedir, to, to_ospath);

	if ( fs_debug->current.integer )
		Com_Printf("FS_Rename: %s --> %s\n", from_ospath, to_ospath);

	if ( rename(from_ospath, to_ospath) )
	{
		FS_Remove(to_ospath);
		if ( rename(from_ospath, to_ospath) )
		{
			FS_CopyFile(from_ospath, to_ospath);
			FS_Remove(from_ospath);
		}
	}
}

void FS_FCloseFile( fileHandle_t h )
{
	FILE *f;

	FS_CheckFileSystemStarted();

	if ( fsh[h].streamed )
	{
		Sys_EndStreamedFile( h );
	}

	if ( fsh[h].zipFile )
	{
		unzCloseCurrentFile( fsh[h].handleFiles.file.z );
		if ( fsh[h].handleFiles.unique )
		{
			unzClose( fsh[h].handleFiles.file.z );
		}
		Com_Memset( &fsh[h], 0, sizeof( fsh[h] ) );
		return;
	}

	// we didn't find it as a pak, so close it as a unique file
	if ( h )
	{
		f = FS_FileForHandle(h);
		FS_FileClose( f );
	}

	Com_Memset( &fsh[h], 0, sizeof( fsh[h] ) );
}

static fileHandle_t FS_GetHandleAndOpenFile(const char *filename, const char *ospath, const char *modes, FsThread thread)
{
	fileHandle_t f;
	FILE* fp;

	fp = FS_FileOpen(ospath, modes);

	if (!fp)
	{
		return 0;
	}

	f = FS_HandleForFile(thread);

	fsh[f].zipFile = qfalse;
	fsh[f].handleFiles.file.o = fp;
	I_strncpyz(fsh[f].name, filename, sizeof(fsh[f].name));
	fsh[f].handleSync = 0;

	return f;
}

fileHandle_t FS_FOpenFileWrite(const char *filename)
{
	char ospath[MAX_OSPATH];

	FS_CheckFileSystemStarted();
	FS_BuildOSPath(fs_homepath->current.string, fs_gamedir, filename, ospath);

	if (fs_debug->current.integer)
	{
		Com_Printf("FS_FOpenFileWrite: %s\n", ospath);
	}

	if (FS_CreatePath(ospath))
	{
		return 0;
	}

	return FS_GetHandleAndOpenFile(filename, ospath, "wb", FS_THREAD_MAIN);
}

fileHandle_t FS_FOpenTextFileWrite(const char* filename)
{
	char ospath[MAX_OSPATH];
	FILE* f;
	fileHandle_t h;

	h = 0;
	FS_CheckFileSystemStarted();

	h = FS_HandleForFile(FS_THREAD_MAIN);
	fsh[h].zipFile = qfalse;

	FS_BuildOSPath(fs_homepath->current.string, fs_gamedir, filename, ospath);

	if (fs_debug->current.integer)
	{
		Com_Printf("FS_FOpenFileWrite: %s\n", ospath);
	}

	if (FS_CreatePath(ospath))
	{
		return 0;
	}

	f = FS_FileOpen(ospath, "wt");
	fsh[h].handleFiles.file.o = f;
	I_strncpyz(fsh[h].name, filename, sizeof(fsh[h].name));
	fsh[h].handleSync = qfalse;

	if (!fsh[h].handleFiles.file.o)
	{
		h = 0;
	}

	return h;
}

fileHandle_t FS_FOpenFileAppend(const char* filename)
{
	char ospath[MAX_OSPATH];
	FILE* f;
	fileHandle_t h;

	h = 0;
	FS_CheckFileSystemStarted();

	h = FS_HandleForFile(FS_THREAD_MAIN);
	fsh[h].zipFile = qfalse;
	I_strncpyz(fsh[h].name, filename, sizeof(fsh[h].name));

	FS_BuildOSPath(fs_homepath->current.string, fs_gamedir, filename, ospath);

	if (fs_debug->current.integer)
	{
		Com_Printf("FS_FOpenFileAppend: %s\n", ospath);
	}

	if (FS_CreatePath(ospath))
	{
		return 0;
	}

	f = FS_FileOpen(ospath, "at");
	fsh[h].handleFiles.file.o = f;
	fsh[h].handleSync = qfalse;

	if (!fsh[h].handleFiles.file.o)
	{
		h = 0;
	}

	return h;
}

int FS_FilenameCompare(const char *s1, const char *s2)
{
	int c1, c2;

	do
	{
		c1 = *s1++;
		c2 = *s2++;

		if (I_islower(c1))
		{
			c1 -= ('a' - 'A');
		}
		if (I_islower(c2))
		{
			c2 -= ('a' - 'A');
		}

		if (c1 == '\\' || c1 == ':')
		{
			c1 = '/';
		}
		if (c2 == '\\' || c2 == ':')
		{
			c2 = '/';
		}

		if (c1 != c2)
		{
			return -1;      // strings not equal
		}
	}
	while (c1);

	return 0;       // strings are equal
}

/*
==============
FS_FileCompare
==============
*/
qboolean FS_FileCompare( const char *filename1, const char *filename2 )
{
	FILE *f1;
	FILE *f2;
	int len1;
	int len2;
	int pos;
	byte *buf1;
	byte *buf2;
	byte *p1;
	byte *p2;

	f1 = FS_FileOpen(filename1, "rb");

	if ( !f1 )
	{
		Com_Error(ERR_FATAL, "\x15" "FS_FileCompare: %s does not exist\n", filename1);
	}

	f2 = FS_FileOpen(filename2, "rb");

	if ( !f2 )
	{
		FS_FileClose(f1);
		return qfalse;
	}

	pos = ftell(f1);
	FS_FileSeek(f1, 0, SEEK_END);
	len1 = ftell(f1);
	FS_FileSeek(f1, pos, SEEK_SET);

	pos = ftell(f2);
	FS_FileSeek(f2, 0, SEEK_END);
	len2 = ftell(f2);
	FS_FileSeek(f2, pos, SEEK_SET);

	if ( len1 != len2 )
	{
		FS_FileClose(f1);
		FS_FileClose(f2);
		return qfalse;
	}

	buf1 = (byte *)Z_Malloc(len1);

	if ( FS_FileRead(buf1, 1, len1, f1) != len1 )
	{
		Com_Error(ERR_FATAL, "\x15" "Short read in FS_FileCompare()\n");
	}

	FS_FileClose(f1);

	buf2 = (byte *)Z_Malloc(len2);

	if ( FS_FileRead(buf2, 1, len2, f2) != len2 )
	{
		Com_Error(ERR_FATAL, "\x15" "Short read in FS_FileCompare()\n");
	}

	FS_FileClose(f2);

	p1 = buf1;
	p2 = buf2;

	for ( pos = 0; pos < len1; pos++, p1++, p2++ )
	{
		if ( *p1 != *p2 )
		{
			free(buf1);
			free(buf2);
			return qfalse;
		}
	}

	free(buf1);
	free(buf2);

	return qtrue;
}

/*
==============
FS_ShiftedStrStr
==============
*/
char *FS_ShiftedStrStr( const char *string, const char *substring, int shift )
{
	char buf[256];
	int i;

	for ( i = 0; substring[i]; i++ )
		buf[i] = substring[i] + shift;

	buf[i] = 0;
	return strstr(string, buf);
}

static qboolean FS_PureIgnoreFiles(const char *extension)
{
	if (extension[0] == '.')
		++extension;

	if (!stricmp(extension, "cfg"))
		return qtrue;

	if (!I_stricmp(extension, "menu"))
		return qtrue;

	if (!I_stricmp(extension, ".dm_NETWORK_PROTOCOL_VERSION"))
		return qtrue;

	return qfalse;
}

static bool FS_IsBackupSubStr(const char *filenameSubStr)
{
	if ( filenameSubStr[0] == '.' && filenameSubStr[1] == '.' )
		return 1;

	if ( filenameSubStr[0] == ':' && filenameSubStr[1] == ':' )
		return 1;

	return 0;
}

static bool FS_IsPathSepChar(char c)
{
	return c == '/' || c == '\\';
}

// size is unused
static bool FS_SanitizeFilename(const char *filename, char *sanitizedName, int size)
{
	int srcIndex;
	int dstIndex;

	for ( srcIndex = 0; FS_IsPathSepChar(filename[srcIndex]); ++srcIndex );
	dstIndex = 0;

	while ( filename[srcIndex] )
	{
		if ( FS_IsBackupSubStr(&filename[srcIndex]) )
			return false;

		if ( filename[srcIndex] != '.' || filename[srcIndex + 1] && !FS_IsPathSepChar(filename[srcIndex + 1]) )
		{
			if ( !FS_IsPathSepChar(filename[srcIndex]) )
			{
				sanitizedName[dstIndex] = filename[srcIndex];
			}
			else
			{
				sanitizedName[dstIndex] = '/';

				while ( FS_IsPathSepChar(filename[srcIndex + 1]) )
					++srcIndex;
			}

			++dstIndex;
		}

		++srcIndex;
	}

	sanitizedName[dstIndex] = 0;
	return true;
}

static bool FS_FilesAreLoadedGlobally(const char *filename)
{
	const char *extensions[8];
	int extensionNum;
	int filenameLen;

	extensions[0] = ".hlsl";
	extensions[1] = ".txt";
	extensions[2] = ".cfg";
	extensions[3] = ".levelshots";
	extensions[4] = ".menu";
	extensions[5] = ".arena";
	extensions[6] = ".str";
	extensions[7] = "";

	filenameLen = I_strlen(filename);

	for ( extensionNum = 0; *extensions[extensionNum]; ++extensionNum )
	{
		if ( !Q_stricmp(filename + filenameLen - strlen(extensions[extensionNum]), extensions[extensionNum]) )
			return true;
	}

	return false;
}

static int FS_FOpenFileRead_Internal(const char *filename, int *file, qboolean uniqueFILE, FsThread thread)
{
	searchpath_t *i;
	char src[MAX_OSPATH - 1];
	char netpath[MAX_OSPATH];
	iwd_t *iwd;
	fileInIwd_t *iwdFile;
	directory_t *dir;
	int hash;
	void *dest;
	FILE *stream;
	const char *extension;
	iwd_t *impureIwd;
	bool wasSkipped;
	char copypath[MAX_OSPATH - 1];

	wasSkipped = false;
	hash = 0;

	FS_CheckFileSystemStarted();

	if (!FS_SanitizeFilename(filename, src, MAX_OSPATH))
	{
		if (file)
		{
			*file = 0;
		}

		return -1;
	}

	if (!file)
	{
		// just wants to see if file is there
		for (i = fs_searchpaths; i; i = i->next)
		{
			if (!FS_UseSearchPath(i) || (i->localized && i->language != SEH_GetCurrentLanguage()))
			{
				continue;
			}

			// look through all the iwd file elements
			if (i->iwd)
			{
				hash = FS_HashFileName(src, i->iwd->hashSize);
			}

			if (i->iwd && i->iwd->hashTable[hash])
			{
				iwd = i->iwd;
				iwdFile = iwd->hashTable[hash];

				// case and separator insensitive comparisons
				while (1)
				{
					if (!FS_FilenameCompare(iwdFile->name, src))
					{
						// found it!
						return 1;
					}

					iwdFile = iwdFile->next;

					if (!iwdFile)
					{
						goto nextSearchPath1;
					}
				}
			}

			if (i->dir)
			{
				dir = i->dir;

				FS_BuildOSPath_Internal(dir->path, dir->gamedir, src, netpath, thread);
				stream = FS_FileOpen(netpath, "rb");

				if (!stream)
				{
					continue;
				}

				FS_FileClose(stream);
				return 1;
			}

		nextSearchPath1:
			;
		}
		return -1;
	}

	//
	// search through the path, one element at a time
	//

	*file = FS_HandleForFile(thread);
	fsh[*file].handleFiles.unique = uniqueFILE;

	impureIwd = NULL;

	for (i = fs_searchpaths; i; i = i->next)
	{
		if (!FS_UseSearchPath(i) || (i->localized && i->language != SEH_GetCurrentLanguage()))
		{
			continue;
		}

		// look through all the iwd file elements
		iwd = i->iwd;
		if (iwd)
		{
			hash = FS_HashFileName(src, iwd->hashSize);
		}

		if (iwd && iwd->hashTable[hash])
		{
			iwdFile = iwd->hashTable[hash];

			// case and separator insensitive comparisons
			do
			{
				if (!FS_FilenameCompare(iwdFile->name, src))
				{
					// found it!
					if (!i->localized && !FS_IwdIsPure(iwd))
					{
						impureIwd = iwd;
						goto nextSearchPath2;
					}

					if (!iwd->referenced && !FS_FilesAreLoadedGlobally(src))
					{
						iwd->referenced = 1;
					}

					if (uniqueFILE)
					{
						// open a new file on the pakfile
						fsh[*file].handleFiles.file.z = unzReOpen(iwd->iwdFilename, iwd->handle);

						if (!fsh[*file].handleFiles.file.z)
						{
							if (thread)
							{
								FS_FCloseFile(*file);
								*file = 0;
								return -1;
							}

							Com_Error(ERR_FATAL, "\x15" "Couldn't reopen %s", iwd->iwdFilename);
						}
					}
					else
					{
						fsh[*file].handleFiles.file.z = iwd->handle;
					}

					I_strncpyz(fsh[*file].name, src, MAX_OSPATH);
					fsh[*file].zipFile = (qboolean)iwd;

					dest = fsh[*file].handleFiles.file.o;
					stream = *(FILE **)dest;

					// set the file position in the zip file (also sets the current file info)
					unzSetCurrentFileInfoPosition(iwd->handle, iwdFile->pos);

					// copy the file info into the unzip structure (retail has no dest==src guard)
					Com_Memcpy(dest, iwd->handle, sizeof(unz_s));

					// we copy this back into the structure
					*(FILE **)dest = stream;

					// open the file in the zip
					unzOpenCurrentFile(fsh[*file].handleFiles.file.z);
					fsh[*file].zipFilePos = iwdFile->pos;

					if (fs_debug->current.integer && !thread)
					{
						Com_Printf(
						    "FS_FOpenFileRead: %s (found in '%s')\n",
						    src,
						    iwd->iwdFilename);
					}

					return ((unz_s *)dest)->cur_file_info.uncompressed_size;
				}

				iwdFile = iwdFile->next;
			}
			while (iwdFile);
		}
		else
		{
			if (i->dir)
			{
				extension = Com_GetExtensionSubString(src);

				// check a file in the directory tree
				if (!((!fs_restrict->current.boolean && !fs_numServerIwds) || i->localized || FS_PureIgnoreFiles(extension)))
				{
					if (!wasSkipped)
					{
						dir = i->dir;

						FS_BuildOSPath_Internal(dir->path, dir->gamedir, src, netpath, thread);
						stream = FS_FileOpen(netpath, "rb");

						if (stream)
						{
							wasSkipped = true;
							FS_FileClose(stream);
						}
					}
				}
				else
				{
					dir = i->dir;

					FS_BuildOSPath_Internal(dir->path, dir->gamedir, src, netpath, thread);
					fsh[*file].handleFiles.file.o = FS_FileOpen(netpath, "rb");

					if (!fsh[*file].handleFiles.file.o)
					{
						continue;
					}

					if (!i->localized && !FS_PureIgnoreFiles(extension))
					{
						fs_fakeChkSum = rand() + 1;
					}

					I_strncpyz(fsh[*file].name, src, MAX_OSPATH);
					fsh[*file].zipFile = qfalse;

					if (fs_debug->current.integer && !thread)
					{
						Com_Printf(
						    "FS_FOpenFileRead: %s (found in '%s/%s')\n",
						    src, dir->path, dir->gamedir);
					}

					// if we are getting it from the cdpath, optionally copy it
					//  to the basepath
					if (fs_copyfiles->current.boolean && !I_stricmp(dir->path, fs_cdpath->current.string))
					{
						FS_BuildOSPath_Internal(fs_basepath->current.string, dir->gamedir, src, copypath, thread);
						FS_CopyFile(netpath, copypath);
					}

					return FS_filelength(*file);
				}
			}
		}

	nextSearchPath2:
		;
	}

	if (fs_debug->current.integer && !thread)
	{
		Com_Printf("Can't find %s\n", filename);
	}

	*file = 0;

	if (impureIwd)
	{
		Com_Error(ERR_DROP, va("EXE_UNPURECLIENTDETECTED\x15\n%s", impureIwd->iwdFilename));
	}

	if (wasSkipped)
	{
		return -2;
	}
	else
	{
		return -1;
	}
}

int FS_FOpenFileReadStream(const char *filename, fileHandle_t *file, qboolean uniqueFILE)
{
	int ret;

	ret = FS_FOpenFileRead_Internal(filename, file, uniqueFILE, FS_THREAD_STREAM);
	return ret;
}

int FS_FOpenFileRead(const char *filename, fileHandle_t *file, qboolean uniqueFILE)
{
	int ret;

	com_fileAccessed = 1;
	ret = FS_FOpenFileRead_Internal(filename, file, uniqueFILE, FS_THREAD_MAIN);
	return ret;
}

int FS_TouchFile( const char *filename )
{
	fileHandle_t file;

	FS_FOpenFileRead(filename, &file, 0);

	if ( !file )
		return 0;

	FS_FCloseFile(file);
	return 1;
}

/*
==============
FS_ShortOSFilePath
==============
*/
const char *FS_ShortOSFilePath( const char *qpath )
{
	searchpath_t *search;
	char netpath[MAX_OSPATH];
	directory_t *dir;
	FILE *f;

	for ( search = fs_searchpaths; search; search = search->next )
	{
		if ( !FS_UseSearchPath(search) || (search->localized && search->language != SEH_GetCurrentLanguage()) )
			continue;
		if ( search->dir )
		{
			dir = search->dir;
			FS_BuildOSPath(dir->path, dir->gamedir, qpath, netpath);
			f = FS_FileOpen(netpath, "rb");
			if ( !f )
				continue;
			FS_FileClose(f);
			return va("%s/%s", dir->gamedir, qpath);
		}
	}

	return NULL;
}

static int FS_Delete(const char *filename)
{
	char ospath[MAX_OSPATH];

	FS_CheckFileSystemStarted();

	if ( !*filename )
		return 0;

	FS_BuildOSPath(fs_homepath->current.string, fs_gamedir, filename, ospath);

	if ( remove(ospath) != -1 )
		return 1;

	return 0;
}

/*
==============
FS_SetWritable
==============
*/
// retail 0x421570: win32 attribute form, nonzero arg sets READONLY
qboolean FS_SetWritable( const char *filename, int writable )
{
	char ospath[MAX_OSPATH];
	DWORD attrib;
	DWORD newAttrib;

	FS_BuildOSPath(fs_homepath->current.string, fs_gamedir, filename, ospath);
	attrib = GetFileAttributesA(ospath);
	newAttrib = !writable ? attrib & ~FILE_ATTRIBUTE_READONLY : attrib | FILE_ATTRIBUTE_READONLY;
	if ( newAttrib == attrib )
		return 0;
	SetFileAttributesA(ospath, newAttrib);
	return 1;
}

int FS_Read(void *buffer, int len, fileHandle_t h)
{
	size_t block;
	size_t remaining;
	int read;
	char *buf;
	int tries;
	FILE *f;

	FS_CheckFileSystemStarted();

	if (!h)
	{
		return 0;
	}

	if (fsh[h].zipFile)
	{
		read = unzReadCurrentFile(fsh[h].handleFiles.file.z, buffer, len);
		return read;
	}

	f = FS_FileForHandle(h);
	buf = (char *)buffer;
	remaining = len;
	tries = 0;

	while (remaining)
	{
		block = remaining;
		read = FS_FileRead(buf, 1, block, f);

		if (!read)
		{
			if (!tries)
			{
				tries = 1;
			}
			else
			{
				return len - remaining;
			}
		}

		if (read == -1)
		{
			if (h >= 51 && h < 64)
			{
				return -1;
			}

			Com_Error(ERR_FATAL, "\x15" "FS_Read: -1 bytes read");
		}

		remaining -= read;
		buf += read;
	}

	return len;
}

int FS_Write(const void *buffer, int len, fileHandle_t h)
{
	size_t block;
	size_t remaining;
	int written;
	char *buf;
	int tries;
	FILE *f;

	FS_CheckFileSystemStarted();

	if (!h)
	{
		return 0;
	}

	f = FS_FileForHandle(h);
	buf = (char *)buffer;
	remaining = len;
	tries = 0;

	while (remaining)
	{
		block = remaining;
		written = FS_FileWrite(buf, 1, block, f);

		if (!written)
		{
			if (tries)
			{
				return 0;
			}

			tries = 1;
		}

		if (written == -1)
		{
			return 0;
		}

		remaining -= written;
		buf += written;
	}

	if (fsh[h].handleSync)
	{
		fflush(f);
	}

	return len;
}

void FS_Printf( fileHandle_t h, const char *fmt, ... )
{
	va_list argptr;
	char msg[MAXPRINTMSG];

	va_start( argptr,fmt );
	vsprintf( msg, fmt, argptr );
	va_end( argptr );

	FS_Write( msg, I_strlen( msg ), h );
}

/*
==============
FS_SeekInternal
==============
*/
int FS_SeekInternal( int f, int offset, int origin )
{
	int _origin;
	int r;
	int iZipPos;
	int iZipOffset;
	FILE *file;

	FS_CheckFileSystemStarted();

	if ( fsh[f].streamed )
	{
		fsh[f].streamed = qfalse;
		Sys_StreamSeek(f, offset, origin);
		fsh[f].streamed = qtrue;
	}

	if ( fsh[f].zipFile )
	{
		if ( offset == 0 && origin == FS_SEEK_SET )
		{
			unzSetCurrentFileInfoPosition(fsh[f].handleFiles.file.z, fsh[f].zipFilePos);
			return unzOpenCurrentFile(fsh[f].handleFiles.file.z);
		}

		if ( offset == 0 && origin == FS_SEEK_CUR )
		{
			return 0;
		}

		iZipPos = unztell(fsh[f].handleFiles.file.z);

		if ( origin == FS_SEEK_CUR )
		{
			if ( offset < 0 )
			{
				unzSetCurrentFileInfoPosition(fsh[f].handleFiles.file.z, fsh[f].zipFilePos);
				unzOpenCurrentFile(fsh[f].handleFiles.file.z);
				iZipOffset = iZipPos + offset;
			}
			else
			{
				iZipOffset = offset;
			}
		}
		else if ( origin == FS_SEEK_END )
		{
			if ( FS_filelength(f) + offset < iZipPos )
			{
				unzSetCurrentFileInfoPosition(fsh[f].handleFiles.file.z, fsh[f].zipFilePos);
				unzOpenCurrentFile(fsh[f].handleFiles.file.z);
				iZipOffset = FS_filelength(f) + offset;
			}
			else
			{
				iZipOffset = FS_filelength(f) + offset - iZipPos;
			}
		}
		else if ( origin == FS_SEEK_SET )
		{
			if ( offset < iZipPos )
			{
				unzSetCurrentFileInfoPosition(fsh[f].handleFiles.file.z, fsh[f].zipFilePos);
				unzOpenCurrentFile(fsh[f].handleFiles.file.z);
				iZipOffset = offset;
			}
			else
			{
				iZipOffset = offset - iZipPos;
			}
		}
		else
		{
			return -1;
		}

		r = unzReadCurrentFile(fsh[f].handleFiles.file.z, NULL, iZipOffset);

		if ( !r )
		{
			return -1;
		}

		return 0;
	}

	file = FS_FileForHandle(f);

	switch ( origin )
	{
	case FS_SEEK_CUR:
		_origin = SEEK_CUR;
		break;
	case FS_SEEK_END:
		_origin = SEEK_END;
		break;
	case FS_SEEK_SET:
		_origin = SEEK_SET;
		break;
	default:
		return 0;
	}

	return FS_FileSeek(file, offset, _origin);
}

__forceinline int FS_ReadFileInl(const char* qpath, void** buffer)
{
	fileHandle_t h;
	char* buf;
	int len;

	FS_CheckFileSystemStarted();

	if (!qpath || !qpath[0])
	{
		Com_Error(ERR_FATAL, "\x15" "FS_ReadFile with empty name\n");
	}

	buf = NULL;

	// look for it in the filesystem or iwd files
	len = FS_FOpenFileRead(qpath, &h, 0);

	if (h == 0)
	{
		if (buffer)
		{
			*buffer = NULL;
		}
		return -1;
	}

	if (!buffer)
	{
		FS_FCloseFile(h);
		return len;
	}

	++fs_loadStack;
	buf = (char *)Hunk_AllocateTempMemory(len + 1);
	*buffer = buf;

	FS_Read(buf, len, h);

	// guarantee that it will have a trailing 0 for string operations
	buf[len] = 0;

	FS_FCloseFile(h);
	return len;
}

int FS_ReadFile(const char* qpath, void** buffer)
{
	return FS_ReadFileInl(qpath, buffer);
}

void FS_ResetFiles()
{
	fs_loadStack = 0;
}

__forceinline void FS_FreeFileInl(void* buffer)
{
	FS_CheckFileSystemStarted();
	--fs_loadStack;
	Hunk_FreeTempMemory(buffer);
}

__declspec(noinline) void FS_FreeFile(void* buffer)
{
	FS_FreeFileInl(buffer);
}

int FS_WriteFile(const char* filename, const void* buffer, int size)
{
	fileHandle_t f;
	int actualSize;

	FS_CheckFileSystemStarted();

	f = FS_FOpenFileWrite(filename);

	if (!f)
	{
		Com_Printf("Failed to open %s\n", filename);
		return 0;
	}

	actualSize = FS_Write(buffer, size, f);
	FS_FCloseFile(f);

	if (actualSize == size)
	{
		return 1;
	}

	FS_Delete(filename);
	return 0;
}

/*
==============
FS_GetOSPath
==============
*/
int FS_GetOSPath( const char *qpath, char *ospath )
{
	char sanitizedName[MAX_OSPATH];
	searchpath_t *search;
	directory_t *dir;
	FILE *f;

	if ( !FS_SanitizeFilename(qpath, sanitizedName, sizeof(sanitizedName)) )
		return -1;

	for ( search = fs_searchpaths; search; search = search->next )
	{
		if ( !FS_UseSearchPath(search) || (search->localized && search->language != SEH_GetCurrentLanguage()) )
			continue;
		if ( search->iwd )
			continue;
		dir = search->dir;
		FS_BuildOSPath_Internal(dir->path, dir->gamedir, sanitizedName, ospath, FS_THREAD_MAIN);
		f = FS_FileOpen(ospath, "rb");
		if ( !f )
			continue;
		FS_FileClose(f);
		return 0;
	}

	return -1;
}

/*
==============
FS_OpenFileOverwrite

Opens an existing loose file for writing, clearing its read-only attribute.
==============
*/
fileHandle_t FS_OpenFileOverwrite( const char *qpath )
{
	char ospath[MAX_OSPATH];
	DWORD attrib;

	if ( FS_GetOSPath(qpath, ospath) < 0 )
	{
		Com_Error(ERR_DROP, "FS_FOpenFileOverWrite: Failed to open %s for writing.  It either does not exist or is in a iwd file.", qpath);
		return 0;
	}

	if ( fs_debug->current.integer )
	{
		Com_Printf("FS_FOpenFileOverWrite: %s\n", ospath);
	}

	attrib = GetFileAttributesA(ospath);
	if ( (attrib & ~FILE_ATTRIBUTE_READONLY) != attrib )
	{
		SetFileAttributesA(ospath, attrib & ~FILE_ATTRIBUTE_READONLY);
	}

	return FS_GetHandleAndOpenFile(qpath, ospath, "wb", FS_THREAD_MAIN);
}

static iwd_t *FS_LoadZipFile( char *zipfile, const char *basename )
{
	fileInIwd_t    *buildBuffer;
	iwd_t          *iwd;
	unzFile uf;
	int err;
	unz_global_info gi;
	char filename_inzip[MAX_ZPATH];
	unz_file_info file_info;
	int i, len;
	long hash;
	int fs_numHeaderLongs;
	intptr_t        *fs_headerLongs;
	char            *namePtr;

	fs_numHeaderLongs = 0;

	uf = unzOpen( zipfile );
	err = unzGetGlobalInfo( uf,&gi );

	if ( err != UNZ_OK )
	{
		return NULL;
	}

	fs_packFiles += gi.number_entry;

	len = 0;
	unzGoToFirstFile( uf );
	for ( i = 0; i < gi.number_entry; i++ )
	{
		err = unzGetCurrentFileInfo( uf, &file_info, filename_inzip, sizeof( filename_inzip ), NULL, 0, NULL, 0 );
		if ( err != UNZ_OK )
		{
			break;
		}
		len += I_strlen( filename_inzip ) + 1;
		unzGoToNextFile( uf );
	}

	buildBuffer = (fileInIwd_t *)Z_Malloc( ( gi.number_entry * sizeof( fileInIwd_t ) ) + len );
	namePtr = ( (char *) buildBuffer ) + gi.number_entry * sizeof( fileInIwd_t );
	fs_headerLongs = (intptr_t *)Z_Malloc( gi.number_entry * sizeof( intptr_t ) );

	// get the hash table size from the number of files in the zip
	// because lots of custom iwd files have less than 32 or 64 files
	for ( i = 1; i <= MAX_FILEHASH_SIZE; i <<= 1 )
	{
		if ( i > gi.number_entry )
		{
			break;
		}
	}

	iwd = (iwd_t *)Z_Malloc( sizeof( iwd_t ) + i * sizeof( fileInIwd_t * ) );
	iwd->hashSize = i;
	iwd->hashTable = ( fileInIwd_t ** )( ( (char *) iwd ) + sizeof( iwd_t ) );
	for ( i = 0; i < iwd->hashSize; i++ )
	{
		iwd->hashTable[i] = NULL;
	}

	Q_strncpyz( iwd->iwdFilename, zipfile, sizeof( iwd->iwdFilename ) );
	Q_strncpyz( iwd->iwdBasename, basename, sizeof( iwd->iwdBasename ) );

	// strip .iwd if needed
	if ( strlen( iwd->iwdBasename ) > 4 && !Q_stricmp( iwd->iwdBasename + strlen( iwd->iwdBasename ) - 4, ".iwd" ) )
	{
		iwd->iwdBasename[strlen( iwd->iwdBasename ) - 4] = 0;
	}

	iwd->handle = uf;
	iwd->numFiles = gi.number_entry;
	unzGoToFirstFile( uf );

	for ( i = 0; i < gi.number_entry; i++ )
	{
		err = unzGetCurrentFileInfo( uf, &file_info, filename_inzip, sizeof( filename_inzip ), NULL, 0, NULL, 0 );
		if ( err != UNZ_OK )
		{
			break;
		}
		if ( file_info.uncompressed_size > 0 )
		{
			fs_headerLongs[fs_numHeaderLongs++] = LittleLong( file_info.crc );
		}
		I_strlwrInl( filename_inzip );
		hash = FS_HashFileName( filename_inzip, iwd->hashSize );
		buildBuffer[i].name = namePtr;
		strcpy( buildBuffer[i].name, filename_inzip );
		namePtr += strlen( filename_inzip ) + 1;
		// store the file position in the zip
		unzGetCurrentFileInfoPosition( uf, &buildBuffer[i].pos );
		//
		buildBuffer[i].next = iwd->hashTable[hash];
		iwd->hashTable[hash] = &buildBuffer[i];
		unzGoToNextFile( uf );
	}

	iwd->checksum = Com_BlockChecksum( fs_headerLongs, sizeof( intptr_t ) * fs_numHeaderLongs );
	iwd->pure_checksum = Com_BlockChecksumKey( fs_headerLongs, sizeof( intptr_t ) * fs_numHeaderLongs, LittleLong( fs_checksumFeed ) );
	iwd->checksum = LittleLong( iwd->checksum );
	iwd->pure_checksum = LittleLong( iwd->pure_checksum );

	Z_Free( fs_headerLongs );

	iwd->buildBuffer = buildBuffer;
	return iwd;
}

static int FS_ReturnPath( const char *zname, char *zpath, int *depth )
{
	int len, at, newdep;

	newdep = 0;
	zpath[0] = 0;
	len = 0;
	at = 0;

	while ( zname[at] != 0 )
	{
		if ( zname[at] == '/' || zname[at] == '\\' )
		{
			len = at;
			newdep++;
		}
		at++;
	}
	strcpy( zpath, zname );
	zpath[len] = 0;
	if ( len + 1 == at )
		--newdep;
	*depth = newdep;

	return len;
}

// allocType is unused
static int FS_AddFileToList( char *name, char *list[MAX_FOUND_FILES], int nfiles, int allocType )
{
	int i;

	if ( nfiles == MAX_FOUND_FILES - 1 )
	{
		return nfiles;
	}
	for ( i = 0 ; i < nfiles ; i++ )
	{
		if ( !Q_stricmp( name, list[i] ) )
		{
			return nfiles;      // allready in list
		}
	}
	list[nfiles] = CopyString( name );
	nfiles++;

	return nfiles;
}

/*
===============
FS_ListFilteredFiles

Returns a uniqued list of files that match the given criteria
from all search paths
===============
*/
char **FS_ListFilteredFiles(searchpath_t *searchPath, const char *path, const char *extension, const char *filter, FsListBehavior behavior, int *numfiles, int allocType)
{
	int nfiles;
	char            **listCopy;
	char            *list[MAX_FOUND_FILES];
	searchpath_t    *search;
	int i;
	int pathLength;
	int extensionLength;
	int length, pathDepth, temp;
	iwd_t           *iwd;
	fileInIwd_t     *buildBuffer;
	char zpath[MAX_ZPATH];
	char sanitizedPath[MAX_OSPATH];
	bool isDirSearch;

	FS_CheckFileSystemStarted();

	if ( !path )
	{
		*numfiles = 0;
		return NULL;
	}

	if ( !extension )
	{
		extension = "";
	}

	if ( !FS_SanitizeFilename(path, sanitizedPath, sizeof(sanitizedPath)) )
	{
		*numfiles = 0;
		return NULL;
	}

	isDirSearch = I_stricmp(extension, "/") == 0;
	pathLength = I_strlen( sanitizedPath );
	if ( pathLength && (sanitizedPath[pathLength - 1] == '\\' || sanitizedPath[pathLength - 1] == '/' ) )
	{
		pathLength--;
	}
	extensionLength = I_strlen( extension );
	nfiles = 0;
	FS_ReturnPath( sanitizedPath, zpath, &pathDepth );
	if ( sanitizedPath[0] )
		++pathDepth;

	//
	// search through the path, one element at a time, adding to list
	//
	for ( search = searchPath ; search ; search = search->next )
	{
		if ( !FS_UseSearchPath(search) )
		{
			continue;
		}

		if ( search->localized && search->language != SEH_GetCurrentLanguage() )
		{
			continue;
		}

		// is the element a pak file?
		if ( search->iwd )
		{
			//ZOID:  If we are pure, don't search for files on paks that
			// aren't on the pure list
			if ( search->localized || FS_IwdIsPure(search->iwd) )
			{
				// look through all the pak file elements
				iwd = search->iwd;
				buildBuffer = iwd->buildBuffer;
				for ( i = 0; i < iwd->numFiles; i++ )
				{
					char    *name;
					int zpathLen, depth;
					char szTrimmedName[MAX_QPATH];

					// check for directory match
					name = buildBuffer[i].name;
					//
					if ( filter )
					{
						// case insensitive
						if ( !Com_FilterPath( filter, name, qfalse ) )
						{
							continue;
						}
						// unique the match
						nfiles = FS_AddFileToList( name, list, nfiles, allocType );
					}
					else
					{
						zpathLen = FS_ReturnPath( name, zpath, &depth );

						if ( depth != pathDepth || pathLength > zpathLen || (pathLength > 0 && name[pathLength] != '/') || I_strnicmp(name, sanitizedPath, pathLength) )
						{
							continue;
						}

						// check for extension match
						if ( isDirSearch )
						{
							if ( name[strlen( name ) - 1] != '/' )
							{
								continue;
							}
						}
						else if ( extensionLength )
						{
							// unique the match
							length = I_strlen( name );
							if ( length <= extensionLength )
							{
								continue;
							}
							if ( name[length - extensionLength - 1] != '.' )
							{
								continue;
							}
							if ( I_stricmp( name + length - extensionLength, extension ) )
							{
								continue;
							}
						}

						temp = pathLength;
						if ( pathLength )
						{
							temp++;     // include the '/'
						}

						if ( isDirSearch )
						{
							strcpy(szTrimmedName, &name[temp]);
							szTrimmedName[strlen(szTrimmedName) - 1] = '\0';
							nfiles = FS_AddFileToList(szTrimmedName, list, nfiles, allocType);
						}
						else
						{
							nfiles = FS_AddFileToList( name + temp, list, nfiles, allocType );
						}
					}
				}
			}
		}
		else if ( search->dir && (!fs_restrict->current.boolean && !fs_numServerIwds || behavior) )     // scan for files in the filesystem
		{
			int numSysFiles;
			char    **sysFiles;
			char    *name;
			char netpath[MAX_OSPATH];

			FS_BuildOSPath(search->dir->path, search->dir->gamedir, sanitizedPath, netpath);
			sysFiles = Sys_ListFiles( netpath, extension, filter, &numSysFiles, isDirSearch );
			for ( i = 0 ; i < numSysFiles ; i++ )
			{
				// unique the match
				name = sysFiles[i];
				nfiles = FS_AddFileToList( name, list, nfiles, allocType );
			}
			Sys_FreeFileList( sysFiles );
		}
	}

	// return a copy of the list
	*numfiles = nfiles;

	if ( !nfiles )
	{
		return NULL;
	}

	listCopy = (char **)Z_Malloc( ( nfiles + 1 ) * sizeof( *listCopy ) );
	for ( i = 0 ; i < nfiles ; i++ )
	{
		listCopy[i] = list[i];
	}
	listCopy[i] = NULL;

	return listCopy;
}

char** FS_ListFiles(const char* path, const char* extension, FsListBehavior behavior, int* numfiles, int allocType)
{
	return FS_ListFilteredFiles(fs_searchpaths, path, extension, 0, behavior, numfiles, allocType);
}

/*
==============
FS_ListFilesOfDirType
==============
*/
char **FS_ListFilesOfDirType( const char *path, const char *extension, FsListBehavior behavior, int *numfiles, int dirTypes, int flags )
{
	return FS_ListFilteredFilesOfDirType(path, extension, NULL, behavior, numfiles, dirTypes, flags);
}

/*
==============
FS_IsBaseGameDirOfType
==============
*/
bool FS_IsBaseGameDirOfType( const char *dir, int dirTypes )
{
	if ( dirTypes == 0x3f )
		return true;
	if ( (dirTypes & 1) && !I_strncmp(dir, "main", 4) )
		return true;
	if ( (dirTypes & 2) && !I_strncmp(dir, "dev", 3) )
		return true;
	if ( (dirTypes & 4) && !I_strncmp(dir, "temp", 4) )
		return true;
	if ( (dirTypes & 8) && !I_strncmp(dir, "raw", 3) )
		return true;
	if ( (dirTypes & 0x10) && !I_strncmp(dir, "raw_shared", 10) )
		return true;
	if ( (dirTypes & 0x20) && !I_strncmp(dir, "devraw", 6) )
		return true;
	return false;
}

/*
==============
FS_ListFilteredFilesOfDirType
==============
*/
char **FS_ListFilteredFilesOfDirType( const char *path, const char *extension, const char *filter, FsListBehavior behavior, int *numfiles, int dirTypes, int flags )
{
	char **fileList;
	searchpath_t *sp;
	searchpath_t *filteredHead;
	searchpath_t *filteredTail;
	const char *gameName;

	filteredHead = NULL;
	filteredTail = NULL;

	for ( sp = fs_searchpaths; sp; sp = sp->next )
	{
		if ( sp->dir )
			gameName = sp->dir->gamedir;
		else if ( sp->iwd )
			gameName = sp->iwd->iwdGamename;
		else
			gameName = NULL;

		if ( !FS_IsBaseGameDirOfType(gameName, dirTypes) )
			continue;

		if ( filteredHead )
		{
			filteredTail->next = (searchpath_t *)Z_Malloc(sizeof(searchpath_t));
			filteredTail = filteredTail->next;
		}
		else
		{
			filteredHead = (searchpath_t *)Z_Malloc(sizeof(searchpath_t));
			filteredTail = filteredHead;
		}

		filteredTail->next = NULL;
		filteredTail->dir = sp->dir;
		filteredTail->language = sp->language;
		filteredTail->localized = sp->localized;
		filteredTail->iwd = sp->iwd;
	}

	fileList = FS_ListFilteredFiles(filteredHead, path, extension, filter, behavior, numfiles, flags);

	while ( filteredHead )
	{
		filteredTail = filteredHead->next;
		Z_Free(filteredHead);
		filteredHead = filteredTail;
	}

	return fileList;
}

// second argument is the same memory-tag id threaded through FS_ListFiles; unused here
void FS_FreeFileList( char **list, int allocType )
{
	int i;

	FS_CheckFileSystemStarted();

	if ( !list )
	{
		return;
	}

	for ( i = 0 ; list[i] ; i++ )
	{
		Z_Free( list[i] );
	}

	Z_Free( list );
}

int FS_GetFileList(const char *path, const char *extension, FsListBehavior behavior, char *listbuf, int bufsize)
{
	int fileCount;
	int i;
	int nTotal;
	int nLen;
	char** fileNames;

	*listbuf = 0;
	fileCount = 0;
	nTotal = 0;
	if (!I_stricmp(path, "$modlist"))
	{
		return FS_GetModList(listbuf, bufsize);
	}

	fileNames = FS_ListFiles(path, extension, behavior, &fileCount, 3);
	for (i = 0; i < fileCount; ++i)
	{
		nLen = I_strlen(fileNames[i]) + 1;
		if (nTotal + nLen + 1 < bufsize)
		{
			strcpy(listbuf, fileNames[i]);
			listbuf += nLen;
			nTotal += nLen;
		}
		else
		{
			fileCount = i;
			break;
		}
	}

	FS_FreeFileList(fileNames, 3);
	return fileCount;
}

__forceinline void FS_ConvertPathInl(char *s)
{
	while (*s)
	{
		if (*s == '\\' || *s == ':')
		{
			*s = '/';
		}
		s++;
	}
}

__declspec(noinline) void FS_ConvertPath(char *s)
{
	FS_ConvertPathInl(s);
}

static int FS_PathCmp(const char* s1, const char* s2)
{
	int c1;
	int c2;

	do
	{
		c1 = *s1++;
		c2 = *s2++;

		if (I_islower(c1))
		{
			c1 -= ('a' - 'A');
		}
		if (I_islower(c2))
		{
			c2 -= ('a' - 'A');
		}
		if (c1 == '\\' || c1 == ':')
		{
			c1 = '/';
		}
		if (c2 == '\\' || c2 == ':')
		{
			c2 = '/';
		}

		if (c1 < c2)
		{
			return -1;      // strings not equal
		}
		if (c1 > c2)
		{
			return 1;
		}
	}
	while (c1);

	return 0;       // strings are equal
}

void FS_SortFileList( char **filelist, int numfiles )
{
	int i, j, k, numsortedfiles;
	char **sortedlist;

	sortedlist = (char **)Z_Malloc( 4 * numfiles + 4 );
	sortedlist[0] = NULL;
	numsortedfiles = 0;
	for ( i = 0; i < numfiles; i++ )
	{
		for ( j = 0; j < numsortedfiles; j++ )
		{
			if ( FS_PathCmp( filelist[i], sortedlist[j] ) < 0 )
			{
				break;
			}
		}
		for ( k = numsortedfiles; k > j; k-- )
		{
			sortedlist[k] = sortedlist[k - 1];
		}
		sortedlist[j] = filelist[i];
		numsortedfiles++;
	}
	Com_Memcpy( filelist, sortedlist, numfiles * sizeof( *filelist ) );
	Z_Free( sortedlist );
}

static void FS_DisplayPath(int bLanguageCull)
{
	searchpath_t* s;
	int i;

	Com_Printf("Current language: %s\n", SEH_GetLanguageName(SEH_GetCurrentLanguage()));

	if (fs_ignoreLocalized->current.boolean)
	{
		Com_Printf("    localized assets are being ignored\n");
	}

	Com_Printf("Current search path:\n");

	for (s = fs_searchpaths; s; s = s->next)
	{
		if (bLanguageCull && !FS_UseSearchPath(s))
		{
			continue;
		}

		if (bLanguageCull && s->localized && s->language != SEH_GetCurrentLanguage())
		{
			continue;
		}

		if (s->iwd)
		{
			Com_Printf("%s (%i files)\n", s->iwd, s->iwd->numFiles);

			if (s->localized)
			{
				Com_Printf("    localized assets iwd file for %s\n", SEH_GetLanguageName(s->language));
			}

			if (fs_numServerIwds)
			{
				if (!FS_IwdIsPure(s->iwd))
				{
					Com_Printf("    not on the pure list\n");
				}
				else
				{
					Com_Printf("    on the pure list\n");
				}
			}
		}
		else
		{
			Com_Printf("%s/%s\n", s->dir, s->dir->gamedir);

			if (s->localized)
			{
				Com_Printf("    localized assets game folder for %s\n", SEH_GetLanguageName(s->language));
			}
		}
	}

	Com_Printf("\nFile Handles:\n");

	for (i = 1; i < MAX_FILE_HANDLES; ++i)
	{
		if (fsh[i].handleFiles.file.o)
		{
			Com_Printf("handle %i: %s\n", i, fsh[i].name);
		}
	}
}

void FS_FullPath_f(void)
{
	FS_DisplayPath(0);
}

void FS_Path_f(void)
{
	FS_DisplayPath(1);
}

static const char* IwdFileLanguage(const char *instr)
{
	signed int i;
	static char Array64[128];
	static qboolean flip;

	flip ^= 1u;
	if ( strlen(instr) < 10 )
	{
		Array64[64 * flip] = 0;
	}
	else
	{
		i = 10;
		memset(&Array64[64 * flip], 0, 64);
		for(; i < 64 && instr[i] != '\0' && isalpha(instr[i]) != '\0'; i++)
		{
			Array64[(64 * flip) + i - 10] = instr[i];
		}
	}

	return &Array64[64 * flip];
}

static signed int iwdsort(const void *cmp1_arg, const void *cmp2_arg)
{
	const char *cmp1;
	const char *cmp2;
	const char *lang1;
	const char *lang2;

	cmp1 = *(const char**)cmp1_arg;
	cmp2 = *(const char**)cmp2_arg;

	if ( !I_strncmp(cmp1, "          ", 10) )
	{
		if ( !I_strncmp(cmp2, "          ", 10) )
		{
			lang1 = IwdFileLanguage(cmp1);
			lang2 = IwdFileLanguage(cmp2);

			if ( !I_stricmp(lang1, "english") )
			{
				if ( I_stricmp(lang2, "english") )
				{
					return -1;
				}
			}
			else
			{
				if ( !I_stricmp(lang2, "english") )
				{
					return 1;
				}
			}
		}
	}

	return FS_PathCmp(cmp1, cmp2);
}

static void FS_AddSearchPath(searchpath_t* search)
{
	searchpath_t** pSearch;

	pSearch = &fs_searchpaths;
	if (search->localized)
	{
		while (*pSearch && !(*pSearch)->localized)
		{
			pSearch = (searchpath_t**)*pSearch;
		}
	}
	search->next = *pSearch;
	*pSearch = search;
}

static qboolean bLanguagesListed;

void FS_AddIwdFilesForGameDirectory(const char *path, const char *dir)
{
	int i;
	int j;
	int langindex;
	int numfiles;
	qboolean islocalized;
	char iwdfile[MAX_OSPATH];
	char *sorted[MAX_IWDFILES];
	char **iwdfiles;
	const char *language;
	searchpath_t *search;
	iwd_t *iwd;

	FS_BuildOSPath(path, dir, "", iwdfile);
	iwdfile[strlen(iwdfile) - 1] = 0;
	iwdfiles = Sys_ListFiles(iwdfile, "iwd", 0, &numfiles, 0);

	if ( numfiles > MAX_IWDFILES )
	{
		Com_Printf("WARNING: Exceeded max number of iwd files in %s/%s (%1/%1)\n", path, dir, numfiles, MAX_IWDFILES);
		numfiles = MAX_IWDFILES;
	}

	for ( i = 0; i < numfiles; i++ )
	{
		sorted[i] = iwdfiles[i];

		if ( !I_strncmp(sorted[i], "localized_", 10) )
		{
			memcpy(sorted[i], "          ", 10);
		}
	}

	qsort(sorted, numfiles, sizeof(intptr_t), iwdsort);

	for ( i = 0; i < numfiles; i++ )
	{
		if ( !I_strncmp(sorted[i], "          ", 10) )
		{
			memcpy(sorted[i], "localized_", 10);
			islocalized = qtrue;
			language = IwdFileLanguage(sorted[i]);

			if ( !language[0] )
			{
				Com_Printf("WARNING: Localized assets iwd file %s/%s/%s has invalid name (no language specified). Proper naming convention is: localized_[language]_iwd#.iwd\n", path, dir, sorted[i]);
				continue;
			}

			if ( !SEH_GetLanguageIndexForName(language, &langindex) )
			{
				Com_Printf("WARNING: Localized assets iwd file %s/%s/%s has invalid name (bad language name specified). Proper naming convention is: localized_[language]_iwd#.iwd\n", path, dir, sorted[i]);

				if ( !bLanguagesListed )
				{
					Com_Printf("Supported languages are:\n");

					for ( j = 0; j < MAX_LANGUAGES; j++ )
						Com_Printf("    %s\n", SEH_GetLanguageName(j));

					bLanguagesListed = qtrue;
				}
				continue;
			}
		}
		else
		{
			islocalized = qfalse;
			langindex = 0;
		}

		FS_BuildOSPath(path, dir, sorted[i], iwdfile);
		iwd = FS_LoadZipFile( iwdfile, sorted[i]);

		if ( !iwd )
		{
			continue;
		}

		strcpy(iwd->iwdGamename, dir);

		search = (searchpath_t *)S_Malloc(sizeof(searchpath_t));
		search->iwd = iwd;
		search->localized = islocalized;
		search->language = langindex;
		FS_AddSearchPath(search);
	}

	Sys_FreeFileList(iwdfiles);
}

static void FS_AddGameDirectoryInternal(const char *path, const char *dir, int bLanguageDirectory, int iLanguage)
{
	char ospath[MAX_OSPATH];
	const char* pszLanguage;
	char szGameFolder[MAX_QPATH];
	searchpath_t* search;
	searchpath_t* i;

	if (bLanguageDirectory)
	{
		pszLanguage = SEH_GetLanguageName(iLanguage);
		Com_sprintf(szGameFolder, sizeof(szGameFolder), "%s/%s", dir, pszLanguage);
	}
	else
	{
		I_strncpyz(szGameFolder, dir, sizeof(szGameFolder));
	}

	for (i = fs_searchpaths; i; i = i->next)
	{
		if (i->dir && !I_stricmp(i->dir->path, path) && !I_stricmp(i->dir->gamedir, szGameFolder))
		{
			if (i->localized != bLanguageDirectory)
			{
				Com_Printf(
				    "WARNING: game folder %s/%s added as both localized & non-localized. Using folder as %s\n",
				    path, szGameFolder, i->localized ? "localized" : "non-localized");
			}

			if (i->localized)
			{
				if (i->language != iLanguage)
				{
					Com_Printf(
					    "WARNING: game folder %s/%s re-added as localized folder with different language\n",
					    path, szGameFolder);
				}
			}
			return;
		}
	}

	if (bLanguageDirectory)
	{
		// find all iwd files in this directory
		FS_BuildOSPath(path, szGameFolder, "", ospath);
		ospath[strlen(ospath) - 1] = 0; // strip the trailing slash
		if (!Sys_DirectoryHasContents(ospath))
		{
			return;
		}
	}
	else
	{
		I_strncpyz(fs_gamedir, szGameFolder, sizeof(fs_gamedir));
	}

	//
	// add the directory to the search path
	//
	search = (searchpath_t*)Z_Malloc(sizeof(searchpath_t));
	search->dir = (directory_t*)Z_Malloc(sizeof(*search->dir));

	I_strncpyz(search->dir->path, path, sizeof(search->dir->path));
	I_strncpyz(search->dir->gamedir, szGameFolder, sizeof(search->dir->gamedir));

	search->localized = bLanguageDirectory;
	search->language = iLanguage;

	FS_AddSearchPath(search);
	FS_AddIwdFilesForGameDirectory(path, szGameFolder);
}

void FS_AddGameDirectory(const char *path, const char *dir)
{
	int language;

	for ( language = MAX_LANGUAGES - 1; language >= 0; language-- )
		FS_AddGameDirectoryInternal(path, dir, qtrue, language);

	FS_AddGameDirectoryInternal(path, dir, qfalse, 0);
}

static void FS_ShutdownSearchPaths(searchpath_t *p)
{
	searchpath_t* next;

	while (p)
	{
		next = p->next;
		if (p->iwd)
		{
			unzClose(p->iwd->handle);
			Z_Free(p->iwd->buildBuffer);
			Z_Free(p->iwd);
		}
		if (p->dir)
		{
			Z_Free(p->dir);
		}
		Z_Free(p);
		p = next;
	}
}

void FS_ShutdownServerIwdNames()
{
	int i;

	for (i = 0; i < fs_numServerIwds; ++i)
	{
		if ( fs_serverIwdNames[i] )
			FreeString((char *)fs_serverIwdNames[i]);

		fs_serverIwdNames[i] = NULL;
	}

	fs_numServerIwds = 0;
}

void FS_ShutdownServerReferencedIwds()
{
	int i;

	for (i = 0; i < fs_numServerReferencedIwds; ++i)
	{
		if ( fs_serverReferencedIwdNames[i] )
			FreeString((char *)fs_serverReferencedIwdNames[i]);

		fs_serverReferencedIwdNames[i] = NULL;
	}

	fs_numServerReferencedIwds = 0;
}

// closemfp is unused
void FS_Shutdown( int closemfp )
{
	int i;

	SND_StopSounds(SND_STOP_STREAMED);
	SEH_Shutdown_StringEd();

	for (i = 1; i < MAX_FILE_HANDLES; ++i)
	{
		if (fsh[i].fileSize)
		{
			FS_FCloseFile(i);
		}
	}

	FS_ShutdownSearchPaths(fs_searchpaths);
	fs_searchpaths = NULL;
	Cmd_RemoveCommand("path");
	Cmd_RemoveCommand("fullpath");
	Cmd_RemoveCommand("dir");
	Cmd_RemoveCommand("fdir");
	Cmd_RemoveCommand("touchFile");
}

static bool FS_RegisterDvars()
{
	const char *homePath;

	if ( !fs_debug )
	{
		fs_debug = Dvar_RegisterInt("fs_debug", 0, 0, 2, DVAR_CHANGEABLE_RESET);
		fs_copyfiles = Dvar_RegisterBool("fs_copyfiles", false, DVAR_INIT | DVAR_CHANGEABLE_RESET);
		fs_cdpath = Dvar_RegisterString("fs_cdpath", Sys_DefaultCDPath(), DVAR_INIT | DVAR_CHANGEABLE_RESET);
		fs_basepath = Dvar_RegisterString("fs_basepath", Sys_DefaultInstallPath(), DVAR_INIT | DVAR_CHANGEABLE_RESET);
		fs_basegame = Dvar_RegisterString("fs_basegame", "", DVAR_INIT | DVAR_CHANGEABLE_RESET);
		fs_useOldAssets = Dvar_RegisterBool("fs_useOldAssets", false, DVAR_CHANGEABLE_RESET);

		homePath = Sys_DefaultHomePath();

		if (!homePath || !homePath[0])
		homePath = fs_basepath->current.string;

		fs_homepath = Dvar_RegisterString("fs_homepath", homePath, DVAR_INIT | DVAR_CHANGEABLE_RESET);
		fs_gameDirVar = Dvar_RegisterString("fs_game", "", DVAR_SERVERINFO | DVAR_SYSTEMINFO | DVAR_INIT | DVAR_CHANGEABLE_RESET);
		fs_restrict = Dvar_RegisterBool("fs_restrict", false, DVAR_INIT | DVAR_CHANGEABLE_RESET);
		fs_ignoreLocalized = Dvar_RegisterBool("fs_ignoreLocalized", false, DVAR_LATCH | DVAR_CHEAT | DVAR_CHANGEABLE_RESET);

		return true;
	}

	return false;
}

void FS_Startup(const char *gameName)
{
	Com_Printf("----- FS_Startup -----\n");
	FS_RegisterDvars();

	if ( fs_useOldAssets->current.boolean )
	{
		if ( fs_basepath->current.string[0] )
			FS_AddGameDirectory(fs_basepath->current.string, "tempcod");
		if ( fs_homepath->current.string[0] )
			FS_AddGameDirectory(fs_homepath->current.string, "tempcod");
	}

	if ( fs_basepath->current.string[0] )
	{
		FS_AddGameDirectory(fs_basepath->current.string, "devraw_shared");
		FS_AddGameDirectory(fs_basepath->current.string, "devraw");
		FS_AddGameDirectory(fs_basepath->current.string, "raw_shared");
		FS_AddGameDirectory(fs_basepath->current.string, "raw");
	}

	if ( fs_homepath->current.string[0] )
	{
		FS_AddGameDirectory(fs_homepath->current.string, "devraw_shared");
		FS_AddGameDirectory(fs_homepath->current.string, "devraw");
		FS_AddGameDirectory(fs_homepath->current.string, "raw_shared");
		FS_AddGameDirectory(fs_homepath->current.string, "raw");
	}

	if ( fs_cdpath->current.string[0] )
	{
		FS_AddGameDirectory(fs_cdpath->current.string, "devraw_shared");
		FS_AddGameDirectory(fs_cdpath->current.string, "devraw");
		FS_AddGameDirectory(fs_cdpath->current.string, "raw_shared");
		FS_AddGameDirectory(fs_cdpath->current.string, "raw");
		FS_AddGameDirectory(fs_cdpath->current.string, gameName);
	}

	if ( fs_basepath->current.string[0] )
		FS_AddGameDirectory(fs_basepath->current.string, gameName);
	if ( fs_basepath->current.string[0] && I_stricmp(fs_homepath->current.string, fs_basepath->current.string) )
		FS_AddGameDirectory(fs_homepath->current.string, gameName);

	if ( fs_basegame->current.string[0] && !I_stricmp(gameName, BASEGAME) && I_stricmp(fs_basegame->current.string, gameName) )
	{
		if ( fs_cdpath->current.string[0] )
			FS_AddGameDirectory(fs_cdpath->current.string, fs_basegame->current.string);
		if ( fs_basepath->current.string[0] )
			FS_AddGameDirectory(fs_basepath->current.string, fs_basegame->current.string);
		if ( fs_homepath->current.string[0] && I_stricmp(fs_homepath->current.string, fs_basepath->current.string) )
			FS_AddGameDirectory(fs_homepath->current.string, fs_basegame->current.string);
	}

	if ( fs_gameDirVar->current.string[0] && !I_stricmp(gameName, BASEGAME) && I_stricmp(fs_gameDirVar->current.string, gameName) )
	{
		if ( fs_cdpath->current.string[0] )
			FS_AddGameDirectory(fs_cdpath->current.string, fs_gameDirVar->current.string);
		if ( fs_basepath->current.string[0] )
			FS_AddGameDirectory(fs_basepath->current.string, fs_gameDirVar->current.string);
		if ( fs_homepath->current.string[0] && I_stricmp(fs_homepath->current.string, fs_basepath->current.string) )
			FS_AddGameDirectory(fs_homepath->current.string, fs_gameDirVar->current.string);
	}

	Com_ReadCDKey();
	FS_AddCommands();
	FS_DisplayPath(1);
	Dvar_ClearModified(fs_gameDirVar);
	Com_Printf("----------------------\n");
	Com_Printf("%d files in iwd files\n", fs_packFiles);
}

void FS_ClearIwdReferences()
{
	searchpath_t *search;

	for ( search = fs_searchpaths; search; search = search->next )
	{
		// is the element a iwd file and has it been referenced?
		if ( search->iwd )
		{
			search->iwd->referenced = 0;
		}
	}
}

void FS_InitFilesystem()
{
	Com_StartupVariable("fs_cdpath");
	Com_StartupVariable("fs_basepath");
	Com_StartupVariable("fs_homepath");
	Com_StartupVariable("fs_game");
	Com_StartupVariable("fs_copyfiles");
	Com_StartupVariable("fs_restrict");
	Com_StartupVariable("loc_language");
	SEH_InitLanguage();
	FS_Startup(BASEGAME);
	SEH_Init_StringEd();
	SEH_UpdateLanguageInfo();
	FS_SetRestrictions();

	// if we can't find default.cfg, assume that the paths are
	// busted and error out now, rather than getting an unreadable
	// graphics screen when the font fails to load
	if ( FS_ReadFile( DEFAULT_CONFIG, NULL ) <= 0 )
	{
		// TTimo - added some verbosity, 'couldn't load default.cfg' confuses the hell out of users
		Com_Error( ERR_FATAL, "Couldn't load %s.  Make sure Call of Duty is run from the correct folder.", DEFAULT_CONFIG );
	}

	I_strncpyz(lastValidBase, fs_basepath->current.string, sizeof(lastValidBase));
	I_strncpyz(lastValidGame, fs_gameDirVar->current.string, sizeof(lastValidGame));
}

void FS_Restart(int checksumFeed)
{
	FS_Shutdown( 0 );
	fs_checksumFeed = checksumFeed;
	FS_ClearIwdReferences();
	FS_Startup(BASEGAME);
	SEH_Init_StringEd();
	SEH_UpdateLanguageInfo();
	FS_SetRestrictions();

	// if we can't find default.cfg, assume that the paths are
	// busted and error out now, rather than getting an unreadable
	// graphics screen when the font fails to load
	if ( FS_ReadFile( DEFAULT_CONFIG, NULL ) <= 0 )
	{
		// this might happen when connecting to a pure server not using BASEGAME/pak0.pk3
		// (for instance a TA demo server)
		if ( lastValidBase[0] )
		{
			FS_PureServerSetLoadedIwds( "", "" );
			Dvar_SetString( fs_basepath, lastValidBase );
			Dvar_SetString( fs_gameDirVar, lastValidGame );
			lastValidBase[0] = '\0';
			lastValidGame[0] = '\0';
			Dvar_SetBool( fs_restrict, 0 );
			FS_Restart( checksumFeed );
			Com_Error( ERR_DROP, "Invalid game folder\n" );
		}
		// TTimo - added some verbosity, 'couldn't load default.cfg' confuses the hell out of users
		Com_Error( ERR_FATAL, "Couldn't load %s.  Make sure Call of Duty is run from the correct folder.", DEFAULT_CONFIG );
	}

	// bk010116 - new check before safeMode
	if ( I_stricmp( fs_gameDirVar->current.string, lastValidGame ) )
	{
		// skip the wolfconfig.cfg if "safe" is on the command line
		if ( !Com_SafeMode() )
		{
			Cbuf_AddText( va( "exec %s\n", CONFIG_NAME ) );
		}
	}

	I_strncpyz(lastValidBase, fs_basepath->current.string, sizeof(lastValidBase));
	I_strncpyz(lastValidGame, fs_gameDirVar->current.string, sizeof(lastValidGame));
}

/*
==============
FS_ConditionalRestart
==============
*/
int FS_ConditionalRestart( int checksumFeed )
{
	if ( com_sv_running->current.boolean )
		return 0;

	if ( fs_gameDirVar->modified )
	{
		FS_Restart(checksumFeed);
		return 1;
	}

	if ( checksumFeed != fs_checksumFeed )
	{
		FS_Restart(checksumFeed);
		return 1;
	}

	return 0;
}

int FS_FOpenFileByMode( const char *qpath, fileHandle_t *f, fsMode_t mode )
{
	// sentinel for an unhandled mode
	int r = 6969;
	qboolean sync;

	sync = qfalse;

	switch ( mode )
	{
	case FS_READ:
		r = FS_FOpenFileRead( qpath, f, qtrue );
		break;
	case FS_WRITE:
		*f = FS_FOpenFileWrite( qpath );
		r = 0;
		if ( *f == 0 )
		{
			r = -1;
		}
		break;
	case FS_APPEND_SYNC:
		sync = qtrue;
	case FS_APPEND:
		*f = FS_FOpenFileAppend( qpath );
		r = 0;
		if ( *f == 0 )
		{
			r = -1;
		}
		break;
	default:
		Com_Error( ERR_FATAL, "\x15" "FSH_FOpenFile: bad mode" );
		break;
	}

	if ( !f )
	{
		return r;
	}

	if ( *f )
	{
		fsh[*f].fileSize = r;
		fsh[*f].streamed = qfalse;
	}

	fsh[*f].handleSync = sync;

	return r;
}

/*
==============
FS_FTell
==============
*/
int FS_FTell( int f )
{
	int pos;

	if ( fsh[f].zipFile )
		pos = unztell(fsh[f].handleFiles.file.z);
	else
		pos = ftell(FS_FileForHandle(f));

	return pos;
}

void FS_Flush(fileHandle_t f)
{
	fflush(FS_FileForHandle(f));
}

/*
==============
GetBspExtension
==============
*/
const char *GetBspExtension()
{
	const char *string = Dvar_GetString("gfx_driver");

	if ( *string )
		return va("%sbsp", string);

	return va("d3dbsp");
}

