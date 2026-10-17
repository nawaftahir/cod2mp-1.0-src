#include "../qcommon/qcommon.h"
#include "com_files.h"

const dvar_t *com_playerProfile;

const dvar_t *ui_playerProfileAlreadyChosen;

extern const dvar_t *com_playerProfile;
extern const dvar_t *ui_playerProfileAlreadyChosen;

void Dvar_ChangeResetValue(dvar_t *dvar, DvarValue value);
void Dvar_ResetDvars(int filter, DvarSetSource setSource);
bool Dvar_AnyLatchedValues();
void Com_CheckSetRecommended();
qboolean Sys_RemoveDirTree(const char *dir);

int Com_BuildPlayerProfilePath(char *path, int pathSize, const char *format, ...);

static bool Com_PlayerProfileExists(const char *profileName)
{
	char **fileList;
	int fileCount;
	int i;
	bool exists;

	if ( !profileName[0] )
		return false;

	exists = false;
	fileList = FS_ListFiles("players", "/", FS_LIST_ALL, &fileCount, 3);

	for ( i = 0; i < fileCount; i++ )
	{
		if ( !I_stricmp(fileList[i], profileName) )
		{
			exists = true;
			break;
		}
	}

	FS_FreeFileList(fileList, 3);
	return exists;
}

static void Com_SetPlayerProfile(const char *profileName)
{
	char configFile[64];

	Dvar_SetString((dvar_t *)com_playerProfile, profileName);
	Com_BuildPlayerProfilePath(configFile, sizeof(configFile), "config_mp.cfg");
	Com_ExecStartupConfigs(configFile);
}

void Com_ChangePlayerProfile(const char *profileName)
{
	char name[64];

	if ( !I_stricmp(profileName, com_playerProfile->current.string) )
		return;

	I_strncpyz(name, profileName, sizeof(name));

	if ( !Com_PlayerProfileExists(name) )
		return;

	FS_WriteFile("players/active.txt", name, strlen(name));
	Cbuf_ExecuteText(EXEC_NOW, "disconnect");
	Dvar_ResetDvars(-1, DVAR_SOURCE_EXTERNAL);
	Com_SetPlayerProfile(name);
	Com_CheckSetRecommended();

	if ( Dvar_AnyLatchedValues() )
		Cbuf_AddText("snd_restart\n");
}

static bool Com_LoadActivePlayerProfile()
{
	void *buffer;
	const char *text;
	const char *profileName;

	if ( FS_ReadFile("players/active.txt", &buffer) < 0 )
		return false;

	text = (const char *)buffer;
	profileName = Com_Parse(&text);
	FS_FreeFile(buffer);

	if ( !Com_PlayerProfileExists(profileName) )
		return false;

	Com_SetPlayerProfile(profileName);
	return true;
}

void Com_InitPlayerProfiles()
{
	DvarValue resetValue;

	ui_playerProfileAlreadyChosen = Dvar_RegisterInt("ui_playerProfileAlreadyChosen", 0, 0, 1, 0x1200);
	resetValue.integer = 1;
	Dvar_ChangeResetValue((dvar_t *)ui_playerProfileAlreadyChosen, resetValue);
	com_playerProfile = Dvar_RegisterString("com_playerProfile", "", 0x1040);

	if ( !Com_LoadActivePlayerProfile() )
		Com_ExecStartupConfigs(NULL);
}

int Com_HasPlayerProfile()
{
	return com_playerProfile->current.string[0] != 0;
}

static int Com_BuildPlayerProfilePathInternal(char *path, int pathSize, const char *playerName, const char *format, va_list argptr)
{
	int prefixLen;
	int len;

	prefixLen = Com_sprintf(path, pathSize, "players/%s/", playerName);
	if ( prefixLen < 0 || prefixLen >= pathSize )
		return pathSize;

	len = _vsnprintf(path + prefixLen, pathSize - prefixLen, format, argptr);
	if ( len < 0 || len + prefixLen >= pathSize )
	{
		path[pathSize - 1] = 0;
		return pathSize;
	}

	return len + prefixLen;
}

int Com_BuildPlayerProfilePathForPlayer(char *path, int pathSize, const char *playerName, const char *format, ...)
{
	va_list argptr;

	va_start(argptr, format);
	return Com_BuildPlayerProfilePathInternal(path, pathSize, playerName, format, argptr);
}

int Com_BuildPlayerProfilePath(char *path, int pathSize, const char *format, ...)
{
	va_list argptr;

	if ( !com_playerProfile->current.string[0] )
		Com_Error(ERR_FATAL, "Tried to use a player profile before it was set.  This is probably a menu bug.\n");

	va_start(argptr, format);
	return Com_BuildPlayerProfilePathInternal(path, pathSize, com_playerProfile->current.string, format, argptr);
}

bool Com_NewPlayerProfile(const char *profileName)
{
	char profilePath[64];
	char osPath[256];

	if ( Com_PlayerProfileExists(profileName) )
	{
		Com_Printf("Profile '%s' already exists\n", profileName);
		return false;
	}

	Com_BuildPlayerProfilePathForPlayer(profilePath, sizeof(profilePath), profileName, "");
	FS_BuildOSPath(fs_basepath->current.string, fs_gamedir, profilePath, osPath);

	if ( FS_CreatePath(osPath) )
	{
		Com_Printf("Unable to create new profile path: %s\n", osPath);
		return false;
	}

	return true;
}

bool Com_DeletePlayerProfile(const char *profileName)
{
	char profilePath[64];
	char osPath[256];

	if ( !Com_PlayerProfileExists(profileName) )
		return false;

	Com_BuildPlayerProfilePathForPlayer(profilePath, sizeof(profilePath), profileName, "");
	FS_BuildOSPath(fs_basepath->current.string, fs_gamedir, profilePath, osPath);

	if ( !Sys_RemoveDirTree(osPath) )
		return false;

	if ( !I_stricmp(profileName, com_playerProfile->current.string) )
		Dvar_SetString((dvar_t *)com_playerProfile, "");

	return true;
}

