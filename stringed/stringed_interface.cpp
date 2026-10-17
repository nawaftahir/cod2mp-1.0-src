#include "stringed_alloc.h"
#include "../qcommon/qcommon.h"

static int giFilesFound;

void SE_FreeFileDataAfterLoad( unsigned char *psLoadedFile )
{
	FS_FreeFile(psLoadedFile);
}

unsigned char *SE_LoadFileData( const char *psFileName )
{
	unsigned char *pbReturn;
	int iTotalBytesLoaded;

	iTotalBytesLoaded = FS_ReadFile(psFileName, (void **)&pbReturn);
	return iTotalBytesLoaded <= 0 ? NULL : pbReturn;
}

static void SE_R_ListFiles( const char *psExtension, const char *psDir, se_string &strResults )
{
	char **sysFiles;
	char **dirFiles;
	int numSysFiles;
	int i;
	int numdirs;

	dirFiles = FS_ListFiles(psDir, "/", FS_LIST_PURE_ONLY, &numdirs, 10);
	for ( i = 0; i < numdirs; i++ )
	{
		// skip blanks, plus ".", ".." etc
		if ( dirFiles[i][0] && dirFiles[i][0] != '.' )
		{
			char sDirName[MAX_QPATH];
			sprintf(sDirName, "%s/%s", psDir, dirFiles[i]);
			SE_R_ListFiles(psExtension, sDirName, strResults);
		}
	}
	sysFiles = FS_ListFiles(psDir, psExtension, FS_LIST_PURE_ONLY, &numSysFiles, 10);
	for ( i = 0; i < numSysFiles; i++ )
	{
		char sFilename[MAX_QPATH];
		sprintf(sFilename, "%s/%s", psDir, sysFiles[i]);
		strResults += sFilename;
		strResults += ';';
		giFilesFound++;
	}
	FS_FreeFileList(sysFiles, 10);
	FS_FreeFileList(dirFiles, 10);
}

int SE_BuildFileList( const char *psStartDir, se_string &strResults )
{
	giFilesFound = 0;
	strResults = "";
	SE_R_ListFiles("str", psStartDir, strResults);
	return giFilesFound;
}

