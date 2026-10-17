#undef chmod
#include <direct.h>
#include <io.h>
#include "../qcommon/qcommon.h"

#define MAX_FOUND_FILES 0x1000

int I_stricmpwild( const char *wild, const char *s );

void Sys_Mkdir( const char *path )
{
	_mkdir( path );
}

qboolean Sys_RemoveDirTree( const char *dir )
{
	char path[MAX_OSPATH];
	struct _finddata_t fdata;
	intptr_t handle;
	bool hasSlash;
	bool failed;
	int len;

	len = strlen( dir );
	hasSlash = dir[len - 1] == '\\' || dir[len - 1] == '/';
	// the retail call passes fdata.name as a spare argument
	Com_sprintf( path, sizeof( path ), hasSlash ? "%s*" : "%s\\*", dir, fdata.name );

	handle = _findfirst( path, &fdata );
	if ( handle != -1 )
	{
		failed = false;
		do
		{
			if ( fdata.name[0] == '.' && ( !fdata.name[1] || ( fdata.name[1] == '.' && !fdata.name[2] ) ) )
				continue;

			Com_sprintf( path, sizeof( path ), hasSlash ? "%s%s" : "%s\\%s", dir, fdata.name );

			if ( fdata.attrib & _A_SUBDIR )
				failed = !Sys_RemoveDirTree( path );
			else
				failed = remove( path ) == -1;

			if ( failed )
				break;
		}
		while ( _findnext( handle, &fdata ) != -1 );

		_findclose( handle );

		if ( failed )
			return qfalse;
	}

	return _rmdir( dir ) != -1;
}

char *Sys_Cwd( void )
{
	static char cwd[MAX_OSPATH];

	_getcwd( cwd, sizeof( cwd ) - 1 );
	cwd[MAX_OSPATH - 1] = 0;

	return cwd;
}

const char *Sys_DefaultCDPath( void )
{
	return "";
}

const char *Sys_DefaultHomePath( void )
{
	return NULL;
}

char *Sys_DefaultInstallPath( void )
{
	return Sys_Cwd();
}

static void Sys_ListFilteredFiles( const char *basedir, const char *subdirs, const char *filter, char **list, int *numfiles )
{
	char search[MAX_OSPATH];
	char filename[MAX_OSPATH];
	int findhandle;
	struct _finddata_t findinfo;

	if ( *numfiles >= MAX_FOUND_FILES - 1 )
		return;

	if ( strlen( subdirs ) )
		Com_sprintf( search, sizeof( search ), "%s\\%s\\*", basedir, subdirs );
	else
		Com_sprintf( search, sizeof( search ), "%s\\*", basedir );

	findhandle = _findfirst( search, &findinfo );
	if ( findhandle == -1 )
		return;

	do
	{
		if ( findinfo.attrib & _A_SUBDIR )
		{
			if ( !I_stricmp( findinfo.name, "." ) || !I_stricmp( findinfo.name, ".." ) || !I_stricmp( findinfo.name, "CVS" ) )
				continue;
		}

		if ( *numfiles >= MAX_FOUND_FILES - 1 )
			break;

		if ( subdirs )
			Com_sprintf( filename, sizeof( filename ), "%s\\%s", subdirs, findinfo.name );
		else
			Com_sprintf( filename, sizeof( filename ), "%s", findinfo.name );

		if ( !Com_FilterPath( filter, filename, qfalse ) )
			continue;

		list[*numfiles] = CopyString( filename );
		(*numfiles)++;
	}
	while ( _findnext( findhandle, &findinfo ) != -1 );

	_findclose( findhandle );
}

qboolean HasFileExtension( const char *name, const char *extension )
{
	char search[MAX_OSPATH];

	Com_sprintf( search, sizeof( search ), "*.%s", extension );
	return I_stricmpwild( search, name ) == 0;
}

char **Sys_ListFiles( const char *directory, const char *extension, const char *filter, int *numfiles, int wantsubs )
{
	char search[MAX_OSPATH];
	int nfiles;
	char **listCopy;
	char **list;
	struct _finddata_t findinfo;
	int findhandle;
	int flag;
	int i;
	LargeLocal list_large_local( MAX_FOUND_FILES * sizeof( char * ) );

	list = (char **)list_large_local.GetBuf();

	if ( filter )
	{
		nfiles = 0;
		Sys_ListFilteredFiles( directory, "", filter, list, &nfiles );

		list[nfiles] = 0;
		*numfiles = nfiles;

		if ( !nfiles )
			return NULL;

		listCopy = (char **)Z_Malloc( ( nfiles + 1 ) * sizeof( *listCopy ) );
		for ( i = 0; i < nfiles; i++ )
			listCopy[i] = list[i];
		listCopy[i] = NULL;

		return listCopy;
	}

	if ( !extension )
		extension = "";

	// passing a slash as extension will find directories
	if ( extension[0] == '/' && extension[1] == 0 )
	{
		extension = "";
		flag = 0;
	}
	else
	{
		flag = _A_SUBDIR;
	}

	if ( *extension )
		Com_sprintf( search, sizeof( search ), "%s\\*.%s", directory, extension );
	else
		Com_sprintf( search, sizeof( search ), "%s\\*", directory );

	nfiles = 0;

	findhandle = _findfirst( search, &findinfo );
	if ( findhandle == -1 )
	{
		*numfiles = 0;
		return NULL;
	}

	do
	{
		if ( ( !wantsubs && flag ^ ( findinfo.attrib & _A_SUBDIR ) ) || ( wantsubs && findinfo.attrib & _A_SUBDIR ) )
		{
			if ( findinfo.attrib & _A_SUBDIR )
			{
				if ( !I_stricmp( findinfo.name, "." ) || !I_stricmp( findinfo.name, ".." ) || !I_stricmp( findinfo.name, "CVS" ) )
					continue;
			}

			if ( *extension && !HasFileExtension( findinfo.name, extension ) )
				continue;

			list[nfiles] = CopyString( findinfo.name );
			nfiles++;

			if ( nfiles == MAX_FOUND_FILES - 1 )
				break;
		}
	}
	while ( _findnext( findhandle, &findinfo ) != -1 );

	list[nfiles] = 0;
	_findclose( findhandle );
	*numfiles = nfiles;

	if ( !nfiles )
		return NULL;

	listCopy = (char **)Z_Malloc( ( nfiles + 1 ) * sizeof( *listCopy ) );
	for ( i = 0; i < nfiles; i++ )
		listCopy[i] = list[i];
	listCopy[i] = NULL;

	return listCopy;
}

void Sys_FreeFileList( char **list )
{
	int i;

	if ( !list )
		return;

	for ( i = 0; list[i]; i++ )
		FreeString( list[i] );

	FreeString( list );
}

// Returns true if the directory holds anything besides ".", ".." and "CVS".
// The find handle is never closed.
qboolean Sys_DirectoryHasContents( const char *dir )
{
	char search[MAX_OSPATH];
	int findhandle;
	struct _finddata_t findinfo;

	Com_sprintf( search, sizeof( search ), "%s\\*", dir );

	findhandle = _findfirst( search, &findinfo );
	if ( findhandle == -1 )
		return qfalse;

	do
	{
		if ( !( findinfo.attrib & _A_SUBDIR )
			|| I_stricmp( findinfo.name, "." ) && I_stricmp( findinfo.name, ".." ) && I_stricmp( findinfo.name, "CVS" ) )
			return qtrue;
	}
	while ( _findnext( findhandle, &findinfo ) != -1 );

	return qfalse;
}

// Streaming is not threaded on this platform: plain file reads.
void Sys_InitStreamThread( void )
{
}

void Sys_ShutdownStreamThread( void )
{
}

void Sys_BeginStreamedFile( fileHandle_t f, int readAhead )
{
}

void Sys_EndStreamedFile( fileHandle_t f )
{
}

int Sys_StreamedRead( void *buffer, int size, int count, fileHandle_t f )
{
	return FS_Read( buffer, size * count, f );
}

void Sys_StreamSeek( fileHandle_t f, int offset, int origin )
{
	FS_SeekInternal( f, offset, origin );
}

