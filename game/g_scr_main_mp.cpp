#include "../qcommon/qcommon.h"
#include "g_shared.h"
#include "../script/script_public.h"
#include "../stringed/stringed_public.h"

// Original name unknown; unreferenced.
static const float unusedConstant = 0.2f;


scr_data_t g_scr_data;

/*
===============
Scr_ParseGameTypeList
===============
*/
char *strlwr( char *s );

/*
===============
GScr_print3d
===============
*/
extern void G_AddDebugString( const vec3_t xyz, const vec4_t color, float scale, const char *pszText );

/*
===============
SetObjectiveIcon
===============
*/
#define MAX_ICON_NAME_LEN 64

#define Scr_PlayFX_Error( msg, fxId ) \
	do { \
		char fxName[MAX_STRING_CHARS]; \
		if ( fxId ) \
			SV_GetConfigstring( (fxId) + CS_EFFECT_NAMES, fxName, sizeof( fxName ) ); \
		else \
			strcpy( fxName, "not successfully loaded" ); \
		Scr_Error( va( msg " (effect = %s)\n", fxName ) ); \
	} while ( 0 )

/*
===============
Scr_BulletTracePassed
===============
*/

void ScrCmd_GetAttachTagName( scr_entref_t entref );
void ClearObjective( objective_t *obj );
void GScr_SetArchive();
void (*Scr_GetFunction( const char **pName, int *type ))( );
void (*BuiltIn_GetMethod( const char **pName, int *type ))( scr_entref_t );
void (*Scr_GetMethod( const char **pName, int *type ))( scr_entref_t );
qboolean Scr_IsValidGameType( const char *pszGameType );
void Scr_PlayerConnect( gentity_t *self );

unsigned int GScr_AllocString( const char *s );
void Scr_LoadLevel();
int GScr_LoadScriptAndLabel( const char *filename, const char *name, qboolean bEnforceExists );
void GScr_LoadGameTypeScript();
void GScr_LoadLevelScript();
void GScr_PostLoadScripts();
void GScr_LoadScripts();
void GScr_FreeScripts();
gentity_t* GetEntity( scr_entref_t entref );
gentity_t* GetPlayerEntity( scr_entref_t entref );
void ScrCmd_GetClanId( scr_entref_t entref );
void ScrCmd_GetClanName( scr_entref_t entref );
void ScrCmd_GetClanMotto( scr_entref_t entref );
void ScrCmd_GetClanDescription( scr_entref_t entref );
void ScrCmd_GetClanURL( scr_entref_t entref );
void print();
void println();
void Scr_LocalizationError( int iParm, const char *pszErrorMessage );
void Scr_ValidateLocalizedStringRef( int parmIndex, const char *token, int tokenLen );
void Scr_ConstructMessageString( int firstParmIndex, int lastParmIndex, const char *errorContext, char *string, int stringLimit );
void Scr_MakeGameMessage( int iClientNum, const char *pszCmd );
void iprintln();
void iprintlnbold();
void GScr_print3d();
void GScr_line();
void assertCmd();
void assertexCmd();
void assertmsgCmd();
void GScr_IsDefined();
void GScr_IsString();
void GScr_IsAlive();
void GScr_GetDvar();
void GScr_GetDvarInt();
void GScr_GetDvarFloat();
void GScr_SetDvar();
void GScr_GetTime();
void Scr_GetEntByNum();
void Scr_GetWeaponModel();
void GScr_GetAmmoCount( scr_entref_t entref );
void GScr_GetAnimLength();
void GScr_AnimHasNotetrack();
void GScr_GetBrushModelCenter();
void GScr_Spawn();
void GScr_SpawnTurret();
void GScr_PrecacheTurret();
void ScrCmd_attach( scr_entref_t entref );
void ScrCmd_detach( scr_entref_t entref );
void ScrCmd_detachAll( scr_entref_t entref );
void ScrCmd_GetAttachSize( scr_entref_t entref );
void ScrCmd_GetAttachModelName( scr_entref_t entref );
void ScrCmd_GetAttachTagName( scr_entref_t entref );
void ScrCmd_GetAttachIgnoreCollision( scr_entref_t entref );
void ScrCmd_LinkTo( scr_entref_t entref );
void ScrCmd_Unlink( scr_entref_t entref );
void ScrCmd_EnableLinkTo( scr_entref_t entref );
void ScrCmd_GetOrigin( scr_entref_t entref );
void ScrCmd_GetEye( scr_entref_t entref );
void ScrCmd_UseBy( scr_entref_t entref );
void ScrCmd_IsTouching( scr_entref_t entref );
void ScrCmd_SoundExists();
void ScrCmd_PlaySound( scr_entref_t entref );
void ScrCmd_PlaySoundAsMaster(scr_entref_t entref);
void ScrCmd_PlayLoopSound( scr_entref_t entref );
void ScrCmd_StopLoopSound( scr_entref_t entref );
void ScrCmd_PlayRumble( scr_entref_t entref );
void ScrCmd_PlayLoopRumble( scr_entref_t entref );
void ScrCmd_StopRumble( scr_entref_t entref );
void ScrCmd_Delete( scr_entref_t entref );
void ScrCmd_SetModel( scr_entref_t entref );
void ScrCmd_GetNormalHealth( scr_entref_t entref );
void ScrCmd_SetNormalHealth( scr_entref_t entref );
void ScrCmd_Show( scr_entref_t entref );
void ScrCmd_Hide( scr_entref_t entref );
void ScrCmd_ShowToPlayer( scr_entref_t entref );
void ScrCmd_SetContents( scr_entref_t entref );
void Scr_SetStableMissile( scr_entref_t entref );
void GScr_SetCursorHint( scr_entref_t entref );
int G_GetHintStringIndex( int *piIndex, const char *pszString );
void GScr_SetHintString( scr_entref_t entref );
void GScr_GetEntityNumber( scr_entref_t entref );
void GScr_EnableGrenadeTouchDamage( scr_entref_t entref );
void GScr_DisableGrenadeTouchDamage( scr_entref_t entref );
void GScr_EnableGrenadeBounce( scr_entref_t entref );
void GScr_DisableGrenadeBounce( scr_entref_t entref );
void GScr_EnableAimAssist( scr_entref_t entref );
void GScr_DisableAimAssist( scr_entref_t entref );
void G_InitObjectives();
qboolean ObjectiveStateIndexFromString( int *piStateIndex, unsigned int stateString );
void ClearObjective_OnEntity( objective_t *obj );
void ClearObjective( objective_t *obj );
void Scr_Objective_Add();
void Scr_Objective_Delete();
void Scr_Objective_State();
void Scr_Objective_Icon();
void Scr_Objective_Position();
void Scr_Objective_OnEntity();
void Scr_Objective_Current();
void GScr_Objective_Team();
void GScr_LogPrint();
void GScr_WorldEntNumber();
void GScr_Obituary();
void GScr_positionWouldTelefrag();
void GScr_getStartTime();
void GScr_PrecacheMenu();
int GScr_GetScriptMenuIndex( const char *pszMenu );
void GScr_PrecacheStatusIcon();
int GScr_GetStatusIconIndex( const char *pszIcon );
void GScr_PrecacheHeadIcon();
int GScr_GetHeadIconIndex( const char *pszIcon );
void Scr_BulletTrace();
void Scr_BulletTracePassed();
void Scr_SightTracePassed();
void Scr_PhysicsTrace();
void Scr_RandomInt();
void Scr_RandomFloat();
void Scr_RandomIntRange();
void Scr_RandomFloatRange();
void GScr_sin();
void GScr_cos();
void GScr_tan();
void GScr_asin();
void GScr_acos();
void GScr_atan();
void GScr_CastInt();
void Scr_Distance();
void Scr_DistanceSquared();
void Scr_Length();
void Scr_LengthSquared();
void Scr_Closer();
void Scr_VectorDot();
void Scr_VectorNormalize();
void Scr_VectorToAngles();
void Scr_AnglesToUp();
void Scr_AnglesToRight();
void Scr_AnglesToForward();
void Scr_IsSubStr();
void Scr_GetSubStr();
void Scr_ToLower();
void Scr_StrTok();
void Scr_MusicPlay();
void Scr_MusicStop();
void Scr_SoundFade();
void Scr_PrecacheModel();
void Scr_PrecacheShellShock();
void Scr_PrecacheItem();
void Scr_PrecacheShader();
void Scr_PrecacheString();
void Scr_PrecacheRumble();
void Scr_AmbientPlay();
void Scr_AmbientStop();
void Scr_GrenadeExplosionEffect();
void GScr_RadiusDamage();
void GScr_SetPlayerIgnoreRadiusDamage();
void GScr_GetMoveDelta();
void GScr_GetAngleDelta();
void GScr_GetNorthYaw();
void Scr_LoadFX();
void Scr_PlayFX();
void Scr_PlayFXOnTag();
void Scr_PlayLoopedFX();
void Scr_SetFog( const char *cmd, float start, float density, float heightDensity, float r, float g, float b, float fTime );
void Scr_SetLinearFog();
void Scr_SetExponentialFog();
void GScr_IsPlayer();
void GScr_IsPlayerNumber();
void GScr_SetWinningPlayer();
void GScr_SetWinningTeam();
void GScr_Announcement();
void GScr_ClientAnnouncement();
void GScr_GetTeamScore();
void GScr_SetTeamScore();
void GScr_SetClientNameMode();
void GScr_UpdateClientNames();
void GScr_GetTeamPlayersAlive();
void GScr_GetNumParts();
void GScr_GetPartName();
void GScr_Earthquake();
void GScr_ShellShock( scr_entref_t entref );
void GScr_StopShellShock( scr_entref_t entref );
void GScr_ViewKick( scr_entref_t entref );
void GScr_LocalToWorldCoords( scr_entref_t entref );
void GScr_SetRightArc( scr_entref_t entref );
void GScr_SetLeftArc( scr_entref_t entref );
void GScr_SetTopArc( scr_entref_t entref );
void GScr_SetBottomArc( scr_entref_t entref );
void GScr_PlaceSpawnPoint( scr_entref_t entref );
void GScr_UpdateScores( scr_entref_t entref );
void GScr_MapRestart();
void GScr_LoadMap();
void GScr_ExitLevel();
void GScr_AddTestClient();
void GScr_MakeDvarServerInfo();
void GScr_AllClientsPrint();
void GScr_MapExists();
void GScr_IsValidGameType();
void GScr_SetVoteString();
void GScr_SetVoteTime();
void GScr_SetVoteYesCount();
void GScr_SetVoteNoCount();
void GScr_KickPlayer();
void GScr_BanPlayer();
void GScr_ClientPrint();
void GScr_OpenFile();
void GScr_CloseFile();
void GScr_FPrintln();
void GScr_FReadLn();
void GScr_FGetArg();
void GScr_SetArchive();
void Scr_PlayRumbleOnPos();
void Scr_PlayLoopRumbleOnPos();
void Scr_StopAllRumbles();
void Scr_IsSplitscreen();
void GScr_MatchEnd();
void GScr_SetPlayerTeamRank();
void GScr_SendXboxLiveRanks();
void GScr_SetTeamForTrigger( scr_entref_t entref );
void GScr_ClientClaimTrigger( scr_entref_t entref );
void GScr_ClientReleaseTrigger( scr_entref_t entref );
void GScr_ReleaseClaimedTrigger( scr_entref_t entref );
void (*Scr_GetMethod( const char **pName, int *type ))( scr_entref_t );
void (*Scr_GetMethod( const char **pName, int *type ))( scr_entref_t );
void (*Scr_GetMethod( const char **pName, int *type ))( scr_entref_t );
void Scr_SetOrigin( gentity_t *ent, int offset );
void Scr_SetAngles( gentity_t *ent, int offset );
void Scr_SetHealth( gentity_t *ent, int offset );
void GScr_AddVector( const vec3_t vVec );
void GScr_AddEntity( gentity_t *pEnt );
char* Scr_GetGameTypeNameForScript( const char *pszGameTypeScript );
qboolean Scr_IsValidGameType( const char *pszGameType );
void Scr_LoadGameType();
void Scr_PlayerConnect( gentity_t *self );
void Scr_PlayerDisconnect( gentity_t *self );
void Scr_VoteCalled( gentity_t *self, const char *command, const char *param1, const char *param2 );
void Scr_PlayerVote( gentity_t *self, const char *option );
int GScr_Stub();

void CL_AddDebugLine( const float *start, const float *end, const float *color, int depthTest, int duration, int fromServer );

scr_function_t functions[] =
{
	{ "print", print, qtrue, },
	{ "println", println, qtrue, },
	{ "iprintln", iprintln, qfalse, },
	{ "iprintlnbold", iprintlnbold, qfalse, },
	{ "print3d", GScr_print3d, qtrue, },
	{ "line", GScr_line, qtrue, },
	{ "getent", Scr_GetEnt, qfalse, },
	{ "getentarray", Scr_GetEntArray, qfalse, },
	{ "spawn", GScr_Spawn, qfalse, },
	{ "spawnturret", GScr_SpawnTurret, qfalse, },
	{ "precacheturret", GScr_PrecacheTurret, qfalse, },
	{ "spawnstruct", Scr_AddStruct, qfalse, },
	{ "assert", assertCmd, qtrue, },
	{ "assertex", assertexCmd, qtrue, },
	{ "assertmsg", assertmsgCmd, qtrue, },
	{ "isdefined", GScr_IsDefined, qfalse, },
	{ "isstring", GScr_IsString, qfalse, },
	{ "isalive", GScr_IsAlive, qfalse, },
	{ "getcvar", GScr_GetDvar, qfalse, },
	{ "getcvarint", GScr_GetDvarInt, qfalse, },
	{ "getcvarfloat", GScr_GetDvarFloat, qfalse, },
	{ "setcvar", GScr_SetDvar, qfalse, },
	{ "gettime", GScr_GetTime, qfalse, },
	{ "getentbynum", Scr_GetEntByNum, qtrue, },
	{ "getweaponmodel", Scr_GetWeaponModel, qfalse, },
	{ "getanimlength", GScr_GetAnimLength, qfalse, },
	{ "animhasnotetrack", GScr_AnimHasNotetrack, qfalse, },
	{ "getbrushmodelcenter", GScr_GetBrushModelCenter, qfalse, },
	{ "objective_add", Scr_Objective_Add, qfalse, },
	{ "objective_delete", Scr_Objective_Delete, qfalse, },
	{ "objective_state", Scr_Objective_State, qfalse, },
	{ "objective_icon", Scr_Objective_Icon, qfalse, },
	{ "objective_position", Scr_Objective_Position, qfalse, },
	{ "objective_onentity", Scr_Objective_OnEntity, qfalse, },
	{ "objective_current", Scr_Objective_Current, qfalse, },
	{ "bullettrace", Scr_BulletTrace, qfalse, },
	{ "bullettracepassed", Scr_BulletTracePassed, qfalse, },
	{ "sighttracepassed", Scr_SightTracePassed, qfalse, },
	{ "physicstrace", Scr_PhysicsTrace, qfalse, },
	{ "getmovedelta", GScr_GetMoveDelta, qfalse, },
	{ "getangledelta", GScr_GetAngleDelta, qfalse, },
	{ "getnorthyaw", GScr_GetNorthYaw, qfalse, },
	{ "randomint", Scr_RandomInt, qfalse, },
	{ "randomfloat", Scr_RandomFloat, qfalse, },
	{ "randomintrange", Scr_RandomIntRange, qfalse, },
	{ "randomfloatrange", Scr_RandomFloatRange, qfalse, },
	{ "sin", GScr_sin, qfalse, },
	{ "cos", GScr_cos, qfalse, },
	{ "tan", GScr_tan, qfalse, },
	{ "asin", GScr_asin, qfalse, },
	{ "acos", GScr_acos, qfalse, },
	{ "atan", GScr_atan, qfalse, },
	{ "int", GScr_CastInt, qfalse, },
	{ "distance", Scr_Distance, qfalse, },
	{ "distancesquared", Scr_DistanceSquared, qfalse, },
	{ "length", Scr_Length, qfalse, },
	{ "lengthsquared", Scr_LengthSquared, qfalse, },
	{ "closer", Scr_Closer, qfalse, },
	{ "vectordot", Scr_VectorDot, qfalse, },
	{ "vectornormalize", Scr_VectorNormalize, qfalse, },
	{ "vectortoangles", Scr_VectorToAngles, qfalse, },
	{ "anglestoup", Scr_AnglesToUp, qfalse, },
	{ "anglestoright", Scr_AnglesToRight, qfalse, },
	{ "anglestoforward", Scr_AnglesToForward, qfalse, },
	{ "issubstr", Scr_IsSubStr, qfalse, },
	{ "getsubstr", Scr_GetSubStr, qfalse, },
	{ "tolower", Scr_ToLower, qfalse, },
	{ "strtok", Scr_StrTok, qfalse, },
	{ "musicplay", Scr_MusicPlay, qfalse, },
	{ "musicstop", Scr_MusicStop, qfalse, },
	{ "soundfade", Scr_SoundFade, qfalse, },
	{ "ambientplay", Scr_AmbientPlay, qfalse, },
	{ "ambientstop", Scr_AmbientStop, qfalse, },
	{ "precachemodel", Scr_PrecacheModel, qfalse, },
	{ "precacheshellshock", Scr_PrecacheShellShock, qfalse, },
	{ "precacheitem", Scr_PrecacheItem, qfalse, },
	{ "precacheshader", Scr_PrecacheShader, qfalse, },
	{ "precachestring", Scr_PrecacheString, qfalse, },
	{ "precacherumble", Scr_PrecacheRumble, qfalse, },
	{ "loadfx", Scr_LoadFX, qfalse, },
	{ "playfx", Scr_PlayFX, qfalse, },
	{ "playfxontag", Scr_PlayFXOnTag, qfalse, },
	{ "playloopedfx", Scr_PlayLoopedFX, qfalse, },
	{ "setcullfog", Scr_SetLinearFog, qfalse, },
	{ "setexpfog", Scr_SetExponentialFog, qfalse, },
	{ "grenadeexplosioneffect", Scr_GrenadeExplosionEffect, qfalse, },
	{ "radiusdamage", GScr_RadiusDamage, qfalse, },
	{ "setplayerignoreradiusdamage", GScr_SetPlayerIgnoreRadiusDamage, qfalse, },
	{ "getnumparts", GScr_GetNumParts, qfalse, },
	{ "getpartname", GScr_GetPartName, qfalse, },
	{ "earthquake", GScr_Earthquake, qfalse, },
	{ "newhudelem", GScr_NewHudElem, qfalse, },
	{ "newclienthudelem", GScr_NewClientHudElem, qfalse, },
	{ "newteamhudelem", GScr_NewTeamHudElem, qfalse, },
	{ "resettimeout", Scr_ResetTimeout, qfalse, },
	{ "isplayer", GScr_IsPlayer, qfalse, },
	{ "isplayernumber", GScr_IsPlayerNumber, qfalse, },
	{ "setwinningplayer", GScr_SetWinningPlayer, qfalse, },
	{ "setwinningteam", GScr_SetWinningTeam, qfalse, },
	{ "announcement", GScr_Announcement, qfalse, },
	{ "clientannouncement", GScr_ClientAnnouncement, qfalse, },
	{ "getteamscore", GScr_GetTeamScore, qfalse, },
	{ "setteamscore", GScr_SetTeamScore, qfalse, },
	{ "setclientnamemode", GScr_SetClientNameMode, qfalse, },
	{ "updateclientnames", GScr_UpdateClientNames, qfalse, },
	{ "getteamplayersalive", GScr_GetTeamPlayersAlive, qfalse, },
	{ "objective_team", GScr_Objective_Team, qfalse, },
	{ "logprint", GScr_LogPrint, qfalse, },
	{ "worldentnumber", GScr_WorldEntNumber, qfalse, },
	{ "obituary", GScr_Obituary, qfalse, },
	{ "positionwouldtelefrag", GScr_positionWouldTelefrag, qfalse, },
	{ "getstarttime", GScr_getStartTime, qfalse, },
	{ "precachemenu", GScr_PrecacheMenu, qfalse, },
	{ "precachestatusicon", GScr_PrecacheStatusIcon, qfalse, },
	{ "precacheheadicon", GScr_PrecacheHeadIcon, qfalse, },
	{ "map_restart", GScr_MapRestart, qfalse, },
	{ "exitlevel", GScr_ExitLevel, qfalse, },
	{ "addtestclient", GScr_AddTestClient, qfalse, },
	{ "makecvarserverinfo", GScr_MakeDvarServerInfo, qfalse, },
	{ "setarchive", GScr_SetArchive, qfalse, },
	{ "allclientsprint", GScr_AllClientsPrint, qfalse, },
	{ "clientprint", GScr_ClientPrint, qfalse, },
	{ "mapexists", GScr_MapExists, qfalse, },
	{ "isvalidgametype", GScr_IsValidGameType, qfalse, },
	{ "matchend", GScr_MatchEnd, qfalse, },
	{ "setplayerteamrank", GScr_SetPlayerTeamRank, qfalse, },
	{ "sendranks", GScr_SendXboxLiveRanks, qfalse, },
	{ "setvotestring", GScr_SetVoteString, qfalse, },
	{ "setvotetime", GScr_SetVoteTime, qfalse, },
	{ "setvoteyescount", GScr_SetVoteYesCount, qfalse, },
	{ "setvotenocount", GScr_SetVoteNoCount, qfalse, },
	{ "openfile", GScr_OpenFile, qfalse, },
	{ "closefile", GScr_CloseFile, qfalse, },
	{ "fprintln", GScr_FPrintln, qfalse, },
	{ "freadln", GScr_FReadLn, qfalse, },
	{ "fgetarg", GScr_FGetArg, qfalse, },
	{ "kick", GScr_KickPlayer, qfalse, },
	{ "ban", GScr_BanPlayer, qfalse, },
	{ "map", GScr_LoadMap, qfalse, },
	{ "playrumbleonpos", Scr_PlayRumbleOnPos, qfalse, },
	{ "playlooprumbleonpos", Scr_PlayLoopRumbleOnPos, qfalse, },
	{ "stopallrumbles", Scr_StopAllRumbles, qfalse, },
	{ "soundexists", ScrCmd_SoundExists, qfalse, },
	{ "issplitscreen", Scr_IsSplitscreen, qfalse, },
};
static scr_method_t methods[] =
{
	{ "attach", ScrCmd_attach, qfalse, },
	{ "detach", ScrCmd_detach, qfalse, },
	{ "detachall", ScrCmd_detachAll, qfalse, },
	{ "getattachsize", ScrCmd_GetAttachSize, qfalse, },
	{ "getattachmodelname", ScrCmd_GetAttachModelName, qfalse, },
	{ "getattachtagname", ScrCmd_GetAttachTagName, qfalse, },
	{ "getattachignorecollision", ScrCmd_GetAttachIgnoreCollision, qfalse, },
	{ "getammocount", GScr_GetAmmoCount, qfalse, },
	{ "getclanid", ScrCmd_GetClanId, qfalse, },
	{ "getclanname", ScrCmd_GetClanName, qfalse, },
	{ "getclandescription", ScrCmd_GetClanDescription, qfalse, },
	{ "getclanmotto", ScrCmd_GetClanMotto, qfalse, },
	{ "getclanurl", ScrCmd_GetClanURL, qfalse, },
	{ "linkto", ScrCmd_LinkTo, qfalse, },
	{ "unlink", ScrCmd_Unlink, qfalse, },
	{ "enablelinkto", ScrCmd_EnableLinkTo, qfalse, },
	{ "getorigin", ScrCmd_GetOrigin, qfalse, },
	{ "geteye", ScrCmd_GetEye, qfalse, },
	{ "useby", ScrCmd_UseBy, qfalse, },
	{ "setstablemissile", Scr_SetStableMissile, qfalse, },
	{ "istouching", ScrCmd_IsTouching, qfalse, },
	{ "playsound", ScrCmd_PlaySound, qfalse, },
	{ "playsoundasmaster", ScrCmd_PlaySoundAsMaster, qfalse, },
	{ "playloopsound", ScrCmd_PlayLoopSound, qfalse, },
	{ "stoploopsound", ScrCmd_StopLoopSound, qfalse, },
	{ "playrumble", ScrCmd_PlayRumble, qfalse, },
	{ "playlooprumble", ScrCmd_PlayLoopRumble, qfalse, },
	{ "stoprumble", ScrCmd_StopRumble, qfalse, },
	{ "delete", ScrCmd_Delete, qfalse, },
	{ "setmodel", ScrCmd_SetModel, qfalse, },
	{ "getnormalhealth", ScrCmd_GetNormalHealth, qfalse, },
	{ "setnormalhealth", ScrCmd_SetNormalHealth, qfalse, },
	{ "show", ScrCmd_Show, qfalse, },
	{ "hide", ScrCmd_Hide, qfalse, },
	{ "showtoplayer", ScrCmd_ShowToPlayer, qfalse, },
	{ "setcontents", ScrCmd_SetContents, qfalse, },
	{ "setcursorhint", GScr_SetCursorHint, qfalse, },
	{ "sethintstring", GScr_SetHintString, qfalse, },
	{ "shellshock", GScr_ShellShock, qfalse, },
	{ "stopshellshock", GScr_StopShellShock, qfalse, },
	{ "viewkick", GScr_ViewKick, qfalse, },
	{ "localtoworldcoords", GScr_LocalToWorldCoords, qfalse, },
	{ "setrightarc", GScr_SetRightArc, qfalse, },
	{ "setleftarc", GScr_SetLeftArc, qfalse, },
	{ "settoparc", GScr_SetTopArc, qfalse, },
	{ "setbottomarc", GScr_SetBottomArc, qfalse, },
	{ "getentitynumber", GScr_GetEntityNumber, qfalse, },
	{ "enablegrenadetouchdamage", GScr_EnableGrenadeTouchDamage, qfalse, },
	{ "disablegrenadetouchdamage", GScr_DisableGrenadeTouchDamage, qfalse, },
	{ "enablegrenadebounce", GScr_EnableGrenadeBounce, qfalse, },
	{ "disablegrenadebounce", GScr_DisableGrenadeBounce, qfalse, },
	{ "enableaimassist", GScr_EnableAimAssist, qfalse, },
	{ "disableaimassist", GScr_DisableAimAssist, qfalse, },
	{ "placespawnpoint", GScr_PlaceSpawnPoint, qfalse, },
	{ "updatescores", GScr_UpdateScores, qfalse, },
	{ "setteamfortrigger", GScr_SetTeamForTrigger, qfalse, },
	{ "clientclaimtrigger", GScr_ClientClaimTrigger, qfalse, },
	{ "clientreleasetrigger", GScr_ClientReleaseTrigger, qfalse, },
	{ "releaseclaimedtrigger", GScr_ReleaseClaimedTrigger, qfalse, },
};

/*
===============
GScr_AllocString
===============
*/

/*
===============
ScrCmd_PlayRumble
===============
*/

/*
===============
ScrCmd_PlayLoopRumble
===============
*/

/*
===============
ScrCmd_StopRumble
===============
*/

/*
===============
GScr_asin
===============
*/

/*
===============
GScr_acos
===============
*/

/*
===============
Scr_ToLower
===============
*/

/*
===============
Scr_PrecacheRumble
===============
*/

/*
===============
GScr_KickPlayer
===============
*/

/*
===============
GScr_BanPlayer
===============
*/

/*
===============
Scr_PlayRumbleOnPos
===============
*/

/*
===============
Scr_PlayLoopRumbleOnPos
===============
*/

/*
===============
Scr_StopAllRumbles
===============
*/

/*
===============
GScr_MatchEnd
===============
*/

/*
===============
GScr_SetPlayerTeamRank
===============
*/

/*
===============
GScr_SendXboxLiveRanks
===============
*/

/*
===============
Scr_IsValidGameType
===============
*/

/*
===============
Scr_PlayerConnect
===============
*/

/*
===============
Scr_PlayerDisconnect
===============
*/


/*
===============
Scr_LoadLevel
===============
*/
void Scr_LoadLevel()
{
	unsigned short handle;

	if ( !g_scr_data.levelscript )
	{
		return;
	}

	handle = Scr_ExecThread( g_scr_data.levelscript, 0 );
	Scr_FreeThread( handle );
}

/*
===============
GScr_LoadScriptAndLabel
===============
*/
int GScr_LoadScriptAndLabel( const char *filename, const char *name, qboolean bEnforceExists )
{
	if ( !Scr_LoadScript(filename) && bEnforceExists )
	{
		Com_Error(ERR_DROP, "\x15" "Could not find script '%s'", filename);
	}

	int handle = Scr_GetFunctionHandle(filename, name);

	if ( !handle && bEnforceExists )
	{
		Com_Error(ERR_DROP, "\x15" "Could not find label '%s' in script '%s'", name, filename);
	}

	return handle;
}

/*
===============
GScr_LoadGameTypeScript
===============
*/
void GScr_LoadGameTypeScript()
{
	char filename[MAX_QPATH];

	Com_sprintf(filename, sizeof(filename), "maps/mp/gametypes/%s", g_gametype->current.string);

	g_scr_data.gametype.main = GScr_LoadScriptAndLabel(filename, "main", qtrue);
	g_scr_data.gametype.startupgametype = GScr_LoadScriptAndLabel("maps/mp/gametypes/_callbacksetup", "CodeCallback_StartGameType", qtrue);
	g_scr_data.gametype.playerconnect = GScr_LoadScriptAndLabel("maps/mp/gametypes/_callbacksetup", "CodeCallback_PlayerConnect", qtrue);
	g_scr_data.gametype.playerdisconnect = GScr_LoadScriptAndLabel("maps/mp/gametypes/_callbacksetup", "CodeCallback_PlayerDisconnect", qtrue);
	g_scr_data.gametype.playerdamage = GScr_LoadScriptAndLabel("maps/mp/gametypes/_callbacksetup", "CodeCallback_PlayerDamage", qtrue);
	g_scr_data.gametype.playerkilled = GScr_LoadScriptAndLabel("maps/mp/gametypes/_callbacksetup", "CodeCallback_PlayerKilled", qtrue);

}

/*
===============
GScr_LoadLevelScript
===============
*/
void GScr_LoadLevelScript()
{
	char filename[MAX_QPATH];

	dvar_t *mapname = Dvar_RegisterString("mapname", "", DVAR_SERVERINFO | DVAR_ROM | DVAR_CHANGEABLE_RESET);
	Com_sprintf(filename, sizeof(filename), "maps/mp/%s", mapname->current.string);
	g_scr_data.levelscript = GScr_LoadScriptAndLabel(filename, "main", qfalse);
}

/*
===============
GScr_PostLoadScripts
===============
*/
void GScr_PostLoadScripts()
{
	for ( int classnum = 0; classnum < CLASS_NUM_COUNT; classnum++ )
	{
		Scr_SetClassMap(classnum);
	}

	GScr_AddFieldsForEntity();
	GScr_AddFieldsForHudElems();
	GScr_AddFieldsForRadiant();
}

/*
===============
GScr_LoadScripts
===============
*/
void GScr_LoadScripts()
{
	Scr_BeginLoadScripts();

	g_scr_data.deletestruct = GScr_LoadScriptAndLabel("codescripts/delete", "main", qtrue);
	g_scr_data.initstructs = GScr_LoadScriptAndLabel("codescripts/struct", "initstructs", qtrue);
	g_scr_data.createstruct = GScr_LoadScriptAndLabel("codescripts/struct", "createstruct", qtrue);

	GScr_LoadGameTypeScript();
	GScr_LoadLevelScript();

	Scr_PostCompileScripts();

	GScr_PostLoadScripts();
	Scr_EndLoadScripts();
}

/*
===============
GScr_FreeScripts
===============
*/
void GScr_FreeScripts()
{
	for ( int classnum = 0; classnum < CLASS_NUM_COUNT; classnum++ )
	{
		Scr_RemoveClassMap(classnum);
	}
}

/*
===============
GetEntity
===============
*/
gentity_t* GetEntity( scr_entref_t entref )
{
	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		assert(entref.entnum < MAX_GENTITIES);
		return &g_entities[entref.entnum];
	}

	Scr_ObjectError("not an entity");
	return NULL;
}

/*
===============
GetPlayerEntity
===============
*/
gentity_t* GetPlayerEntity( scr_entref_t entref )
{
	gentity_t *ent;

	ent = GetEntity(entref);

	if ( !ent->client )
	{
		Scr_Error(va("only valid on players; called on entity %i at %.0f %.0f %.0f classname %s targetname %s\n",
		             entref.entnum,
		             ent->r.currentOrigin[0],
		             ent->r.currentOrigin[1],
		             ent->r.currentOrigin[2],
		             SL_ConvertToString(ent->classname),
		             ent->targetname ? SL_ConvertToString(ent->targetname) : "<undefined>"));
	}

	return ent;
}

/*
===============
ScrCmd_GetClanId
===============
*/
void ScrCmd_GetClanId( scr_entref_t entref )
{
	Scr_AddString("0");
}

/*
===============
ScrCmd_GetClanName
===============
*/
void ScrCmd_GetClanName( scr_entref_t entref )
{
	Scr_AddString("");
}

/*
===============
ScrCmd_GetClanMotto
===============
*/
void ScrCmd_GetClanMotto( scr_entref_t entref )
{
	Scr_AddString("");
}

/*
===============
ScrCmd_GetClanDescription
===============
*/
void ScrCmd_GetClanDescription( scr_entref_t entref )
{
	Scr_AddString("");
}

/*
===============
ScrCmd_GetClanURL
===============
*/
void ScrCmd_GetClanURL( scr_entref_t entref )
{
	Scr_AddString("");
}

/*
===============
print
===============
*/
void print()
{
	int numParams;
	int i;

	if ( g_no_script_spam->current.boolean )
	{
		return;
	}

	numParams = Scr_GetNumParam();

	for ( i = 0; i < numParams; i++ )
	{
		Com_Printf("%s", Scr_GetDebugString(i));
	}
}

/*
===============
println
===============
*/
void println()
{
	if ( g_no_script_spam->current.boolean )
	{
		return;
	}

	print();
	Com_Printf("\n");
}

/*
===============
Scr_LocalizationError
===============
*/
void Scr_LocalizationError( int iParm, const char *pszErrorMessage )
{
	Com_Error(ERR_LOCALIZATION, pszErrorMessage);
}

/*
===============
Scr_ValidateLocalizedStringRef
===============
*/
void Scr_ValidateLocalizedStringRef( int parmIndex, const char *token, int tokenLen )
{
	assert(token);
	assert(tokenLen >= 0);

	if ( tokenLen <= 1 )
	{
		return;
	}

	for ( int charIter = 0; charIter < tokenLen; charIter++ )
	{
		if ( !isalnum(token[charIter]) && token[charIter] != '_' )
		{
			Scr_ParamError(parmIndex, va("Illegal localized string reference: %s must contain only alpha-numeric characters and underscores", token));
		}
	}
}

/*
===============
Scr_ConstructMessageString
===============
*/
void Scr_ConstructMessageString( int firstParmIndex, int lastParmIndex, const char *errorContext, char *string, int stringLimit )
{
	int charIndex;
	int i;
	int stringLen;
	int tokenLen;
	int type;
	const char *token;
	bool wasString;
	bool localized;
	gentity_t *ent;

	wasString = true;
	stringLen = 0;

	for ( i = firstParmIndex; i <= lastParmIndex; i++ )
	{
		type = Scr_GetType(i);
		localized = false;

		if ( type == VAR_ISTRING )
		{
			token = Scr_GetIString(i);
			tokenLen = strlen(token);

			Scr_ValidateLocalizedStringRef(i, token, tokenLen);

			if ( stringLen + tokenLen + 1 >= stringLimit )
			{
				Scr_ParamError(i, va("%s is too long. Max length is %i\n", errorContext, stringLimit));
			}

			if ( stringLen )
			{
				string[stringLen] = 20;
				stringLen++;
			}

			wasString = true;
		}
		else if ( type == VAR_POINTER && Scr_GetPointerType(i) == VAR_ENTITY )
		{
			ent = Scr_GetEntity(i);

			if ( !ent->client )
			{
				Scr_ParamError(i, "Entity is not a player");
			}

			token = va("%s^7", ent->client->sess.cs.name);
			tokenLen = strlen(token);

			if ( stringLen + tokenLen + 1 >= stringLimit )
			{
				Scr_ParamError(i, va("%s is too long. Max length is %i\n", errorContext, stringLimit));
			}

			if ( wasString )
			{
				string[stringLen] = 21;
				stringLen++;
			}

			wasString = false;
		}
		else
		{
			token = Scr_GetString(i);
			tokenLen = strlen(token);

			for ( charIndex = 0; charIndex < tokenLen; charIndex++ )
			{
				if ( token[charIndex] == 20 || token[charIndex] == 21 || token[charIndex] == 22 )
				{
					Scr_ParamError(i, va("bad escape character (%i) present in string", token[charIndex]));
				}

				if ( isalpha(token[charIndex]) )
				{
					if ( loc_warningsAsErrors->current.boolean )
					{
						Scr_LocalizationError(i, va("non-localized %s strings are not allowed to have letters in them: \"%s\"", errorContext, token));
					}
					else
					{
						Com_Printf("^3WARNING: Non-localized %s string is not allowed to have letters in it. Must be changed over to a localized" " string: \"%s\"\n", errorContext, token);
					}

					break;
				}
			}

			if ( stringLen + tokenLen + 1 >= stringLimit )
			{
				Scr_ParamError(i, va("%s is too long. Max length is %i\n", errorContext, stringLimit));
			}

			if ( wasString )
			{
				string[stringLen] = 21;
				stringLen++;
			}

			wasString = false;
		}

		for ( charIndex = 0; charIndex < tokenLen; charIndex++ )
		{
			if ( token[charIndex] == 20 || token[charIndex] == 21 || token[charIndex] == 22 )
			{
				string[stringLen] = '.';
			}
			else
			{
				string[stringLen] = token[charIndex];
			}

			stringLen++;
		}
	}

	string[stringLen] = 0;
}

/*
===============
Scr_MakeGameMessage
===============
*/
void Scr_MakeGameMessage( int iClientNum, const char *pszCmd )
{
	char string[MAX_STRING_CHARS];

	Scr_ConstructMessageString(0, Scr_GetNumParam() - 1, "Game Message", string, sizeof(string));
	SV_GameSendServerCommand(iClientNum, SV_CMD_CAN_IGNORE, va("%s \"%s\"", pszCmd, string));
}

/*
===============
iprintln
===============
*/
void iprintln()
{
	Scr_MakeGameMessage(-1, va("%c", 102));
}

/*
===============
iprintlnbold
===============
*/
void iprintlnbold()
{
	Scr_MakeGameMessage(-1, va("%c", 103));
}

void GScr_print3d()
{
	vec3_t origin;
	const char *text;
	vec4_t color;
	vec3_t vec;
	float scale;

	scale = 1.0f;
	color[0] = 1.0f;
	color[1] = 1.0f;
	color[2] = 1.0f;
	color[3] = 1.0f;

	switch ( Scr_GetNumParam() )
	{
	default:
		scale = Scr_GetFloat( 4 );
	case 4:
		color[3] = Scr_GetFloat( 3 );
	case 3:
		Scr_GetVector( 2, vec );
		VectorCopy( vec, color );
	case 2:
		text = Scr_GetString( 1 );
		Scr_GetVector( 0, origin );
		G_AddDebugString( origin, color, scale, text );
	}
}

/*
===============
GScr_line
===============
*/
void GScr_line()
{
	vec3_t start;
	vec3_t end;
	vec4_t color;
	vec3_t vec;
	int depthTest;

	depthTest = 0;
	color[0] = 1.0f;
	color[1] = 1.0f;
	color[2] = 1.0f;
	color[3] = 1.0f;

	switch ( Scr_GetNumParam() )
	{
	default:
		depthTest = Scr_GetInt( 4 );
	case 4:
		color[3] = Scr_GetFloat( 3 );
	case 3:
		Scr_GetVector( 2, vec );
		VectorCopy( vec, color );
	case 2:
		Scr_GetVector( 1, end );
		Scr_GetVector( 0, start );
		CL_AddDebugLine( start, end, color, depthTest, 0, qtrue );
	}
}

/*
===============
assertCmd
===============
*/
void assertCmd()
{
	if ( Scr_GetInt(0) )
	{
		return;
	}

	Scr_Error("assert fail");
}

/*
===============
assertexCmd
===============
*/
void assertexCmd()
{
	if ( Scr_GetInt(0) )
	{
		return;
	}

	Scr_Error(va("assert fail: %s", Scr_GetString(1)));
}

/*
===============
assertmsgCmd
===============
*/
void assertmsgCmd()
{
	Scr_Error(va("assert fail: %s", Scr_GetString(0)));
}

/*
===============
GScr_IsDefined
===============
*/
void GScr_IsDefined()
{
	int type = Scr_GetType(0);

	if ( type == VAR_POINTER )
	{
		type = Scr_GetPointerType(0);
		assert( type >= FIRST_OBJECT );
		Scr_AddInt( type < VAR_DEAD_THREAD && type != VAR_DEAD_ENTITY );
		return;
	}

	assert(type < FIRST_OBJECT);
	Scr_AddInt( type != VAR_UNDEFINED );
}

/*
===============
GScr_IsString
===============
*/
void GScr_IsString()
{
	Scr_AddInt( Scr_GetType(0) == VAR_STRING );
}

/*
===============
GScr_IsAlive
===============
*/
void GScr_IsAlive()
{
	gentity_t *ent;

	if ( Scr_GetType(0) != VAR_POINTER )
	{
		Scr_AddInt(false);
	}
	else if ( Scr_GetPointerType(0) != VAR_ENTITY )
	{
		Scr_AddInt(false);
	}
	else
	{
		ent = Scr_GetEntity(0);

		if ( ent->health > 0 )
		{
			Scr_AddInt(true);
		}
		else
		{
			Scr_AddInt(false);
		}
	}
}

/*
===============
GScr_GetDvar
===============
*/
void GScr_GetDvar()
{
	const char *v2;
	const char *s;

	v2 = Scr_GetString(0);
	s = Dvar_GetVariantString(v2);
	Scr_AddString(s);
}

/*
===============
GScr_GetDvarInt
===============
*/
void GScr_GetDvarInt()
{
	const char *name;
	const char *value;

	name = Scr_GetString(0);
	value = Dvar_GetVariantString(name);
	Scr_AddInt(atoi(value));
}

/*
===============
GScr_GetDvarFloat
===============
*/
void GScr_GetDvarFloat()
{
	Scr_AddFloat( atof( Dvar_GetVariantString( Scr_GetString(0) ) ) );
}

/*
===============
GScr_SetDvar
===============
*/
void GScr_SetDvar()
{
	int i;
	int len;
	int type;
	char *p;
	char string[MAX_STRING_CHARS];
	char outString[MAX_STRING_CHARS];
	const char *dvarName;
	const char *dvarValue;
	dvar_t *dvar;
	int flags;

	dvarName = Scr_GetString(0);
	type = Scr_GetType(1);

	if ( type == VAR_ISTRING )
	{
		Scr_ConstructMessageString(1, Scr_GetNumParam() - 1, "Dvar Value", string, sizeof(string));
		dvarValue = string;
	}
	else
	{
		dvarValue = Scr_GetString(1);
	}

	// no bounds check against outString's size
	len = strlen(dvarValue);
	p = outString;
	memset(outString, 0, sizeof(outString));

	for ( i = 0; i < 0x2000 && dvarValue[i]; i++, p++ )
	{
		*p = I_CleanChar(dvarValue[i]);

		if ( *p == '"' )
		{
			*p = '\'';
		}
	}

	if ( !Dvar_IsValidName(dvarName) )
	{
		Scr_Error(va("Dvar %s has an invalid dvar name", dvarName));
		return;
	}

	flags = (Scr_GetNumParam() >= 3 && Scr_GetInt(2));

	Dvar_SetFromStringByName(dvarName, dvarValue);

	if ( flags )
	{
		// noupdate flag example: setcvar("sv_hostname", "localhost", 1);
		dvar = Dvar_FindVar(dvarName);
		Dvar_AddFlags(dvar, DVAR_SERVERINFO_NOUPDATE);
	}
}

/*
===============
GScr_GetTime
===============
*/
void GScr_GetTime()
{
	Scr_AddInt(level.time);
}

/*
===============
Scr_GetEntByNum
===============
*/
void Scr_GetEntByNum()
{
	unsigned int v1;

	v1 = Scr_GetInt(0);

	if ( v1 >= MAX_GENTITIES )
	{
		return;
	}

	gentity_t *ent = &g_entities[v1];

	if ( !ent->r.inuse )
	{
		return;
	}

	Scr_AddEntity(ent);
}

/*
===============
Scr_GetWeaponModel
===============
*/
void Scr_GetWeaponModel()
{
	const char *pszWeaponName = Scr_GetString(0);
	int iWeaponIndex = G_GetWeaponIndexForName(pszWeaponName);

	if ( !iWeaponIndex )
	{
		if ( pszWeaponName[0] )
		{
			if ( I_stricmp(pszWeaponName, "none") )
			{
				Com_Printf(va("unknown weapon '%s' in getWeaponModel\n", pszWeaponName));
			}
		}

		Scr_AddString("");
		return;
	}

	Scr_AddString(BG_GetWeaponDef(iWeaponIndex)->worldModel);
}

/*
===============
GScr_GetAmmoCount
===============
*/
void GScr_GetAmmoCount( scr_entref_t entref )
{
	const char *name;
	int weaponIndex;
	gentity_t *ent;

	ent = GetPlayerEntity(entref);
	assert(ent);
	assert(ent->client);

	name = Scr_GetString(0);
	weaponIndex = G_GetWeaponIndexForName(name);

	if ( !weaponIndex )
	{
		Scr_AddInt(0);
		return;
	}

	Scr_AddInt(BG_WeaponAmmo(&ent->client->ps, weaponIndex));
}

/*
===============
GScr_GetAnimLength
===============
*/
void GScr_GetAnimLength()
{
	scr_anim_s anim;
	XAnim *anims;

	anim = Scr_GetAnim(0, NULL);
	anims = Scr_GetAnims(anim.tree);

	if ( !XAnimIsPrimitive(anims, anim.index) )
	{
		Scr_ParamError(0, "non-primitive animation has no concept of length");
	}

	Scr_AddFloat( XAnimGetLength(anims, anim.index) );
}

/*
===============
GScr_AnimHasNotetrack
===============
*/
void GScr_AnimHasNotetrack()
{
	scr_anim_s anim;
	unsigned short notetrack;

	anim = Scr_GetAnim(0, NULL);
	notetrack = Scr_GetConstString(1);

	Scr_AddBool( XAnimNotetrackExists( Scr_GetAnims(anim.tree), anim.index, notetrack ) );
}

/*
===============
GScr_GetBrushModelCenter
===============
*/
void GScr_GetBrushModelCenter()
{
	vec3_t vCenter;

	gentity_t *pEnt = Scr_GetEntity(0);

	VectorAdd(pEnt->r.absmin, pEnt->r.absmax, vCenter);
	VectorScale(vCenter, 0.5, vCenter);

	Scr_AddVector(vCenter);
}

/*
===============
GScr_Spawn
===============
*/
void GScr_Spawn()
{
	unsigned short classname;
	vec3_t origin;
	gentity_t *ent;
	int iSpawnFlags;

	classname = Scr_GetConstString(0);
	Scr_GetVector(1, origin);

	if ( Scr_GetNumParam() > 2 )
	{
		iSpawnFlags = Scr_GetInt(2);
	}
	else
	{
		iSpawnFlags = 0;
	}

	ent = G_Spawn();

	Scr_SetString(&ent->classname, classname);
	VectorCopy(origin, ent->r.currentOrigin);
	ent->spawnflags = iSpawnFlags;

	if ( G_CallSpawnEntity(ent) )
	{
		Scr_AddEntity(ent);
	}
	else
	{
		Scr_Error(va("unable to spawn \"%s\" entity", SL_ConvertToString(classname)));
	}
}

/*
===============
GScr_SpawnTurret
===============
*/
void GScr_SpawnTurret()
{
	unsigned short classname;
	vec3_t origin;
	gentity_t *ent;
	const char *weaponName;

	classname = Scr_GetConstString(0);
	Scr_GetVector(1, origin);
	weaponName = Scr_GetString(2);

	ent = G_Spawn();

	Scr_SetString(&ent->classname, classname);
	VectorCopy(origin, ent->r.currentOrigin);
	G_SpawnTurret(ent, weaponName);

	Scr_AddEntity(ent);
}

/*
===============
GScr_PrecacheTurret
===============
*/
void GScr_PrecacheTurret()
{
	const char *name;

	if ( !level.initializing )
	{
		Scr_Error("precacheTurret must be called before any wait statements in the level script\n");
	}

	name = Scr_GetString(0);
	G_GetWeaponIndexForName(name);
}

/*
===============
ScrCmd_attach
===============
*/
void ScrCmd_attach( scr_entref_t entref )
{
	gentity_t *ent = GetEntity(entref);
	const char *modelName = Scr_GetString(0);
	unsigned int tag = Scr_GetNumParam() >= 2 ? Scr_GetConstLowercaseString(1) : scr_const.emptystring;
	qboolean ignoreCollision = Scr_GetNumParam() >= 3 ? (qboolean)Scr_GetInt(2) : qfalse;

	if ( G_EntDetach(ent, modelName, tag) )
	{
		Scr_Error(va("model '%s' already attached to tag '%s'", modelName, SL_ConvertToString(tag)));
	}

	if ( !G_EntAttach(ent, modelName, tag, ignoreCollision) )
	{
		Scr_Error("maximum attached models exceeded");
	}
}

/*
===============
ScrCmd_detach
===============
*/
void ScrCmd_detach( scr_entref_t entref )
{
	gentity_t *ent;
	const char *modelName;
	unsigned int tag;

	ent = GetEntity(entref);
	modelName = Scr_GetString(0);
	tag = Scr_GetNumParam() >= 2 ? Scr_GetConstLowercaseString(1) : scr_const.emptystring;

	if ( G_EntDetach(ent, modelName, tag) )
	{
		return;
	}

	Com_Printf("Current attachments:\n");

	for ( int i = 0; i < MAX_MODEL_ATTACHMENTS + 1; i++ )
	{
		if ( !ent->attachModelNames[i] )
		{
			continue;
		}

		if ( !ent->attachTagNames[i] )
		{
			continue;
		}

		Com_Printf("model: '%s', tag: '%s'\n", G_ModelName(ent->attachModelNames[i]), SL_ConvertToString(ent->attachTagNames[i]));
	}

	Scr_Error(va("failed to detach model '%s' from tag '%s'", modelName, SL_ConvertToString(tag)));
}

/*
===============
ScrCmd_detachAll
===============
*/
void ScrCmd_detachAll( scr_entref_t entref )
{
	gentity_t *ent;

	ent = GetEntity(entref);
	G_EntDetachAll(ent);
}

/*
===============
ScrCmd_GetAttachSize
===============
*/
void ScrCmd_GetAttachSize( scr_entref_t entref )
{
	int i;
	gentity_t *ent = GetEntity(entref);

	for ( i = 0; i < MAX_MODEL_ATTACHMENTS + 1; i++ )
	{
		if ( !ent->attachModelNames[i] )
		{
			break;
		}
	}

	Scr_AddInt(i);
}

/*
===============
ScrCmd_GetAttachTagName
===============
*/
void ScrCmd_GetAttachModelName( scr_entref_t entref )
{
	unsigned int i;
	gentity_t *ent;

	ent = GetEntity(entref);
	i = Scr_GetInt(0);

	if ( i >= MAX_MODEL_ATTACHMENTS + 1 || !ent->attachModelNames[i] )
	{
		Scr_ParamError(0, "bad index");
	}

	Scr_AddString(G_ModelName(ent->attachModelNames[i]));
}

/*
===============
ScrCmd_GetAttachTagName
===============
*/
void ScrCmd_GetAttachTagName( scr_entref_t entref )
{
	unsigned int i;
	gentity_t *ent;

	ent = GetEntity(entref);
	i = Scr_GetInt(0);

	if ( i >= MAX_MODEL_ATTACHMENTS + 1 || !ent->attachModelNames[i] )
	{
		Scr_ParamError(0, "bad index");
	}

	assert(ent->attachTagNames[i]);
	Scr_AddConstString(ent->attachTagNames[i]);
}

/*
===============
ScrCmd_GetAttachIgnoreCollision
===============
*/
void ScrCmd_GetAttachIgnoreCollision( scr_entref_t entref )
{
	unsigned int i;
	gentity_t *ent;

	ent = GetEntity(entref);
	i = Scr_GetInt(0);

	if ( i >= MAX_MODEL_ATTACHMENTS + 1 || !ent->attachModelNames[i] )
	{
		Scr_ParamError(0, "bad index");
	}

	Scr_AddBool( ( ent->attachIgnoreCollision & ( 1 << i ) ) != 0 );
}

/*
===============
ScrCmd_LinkTo
===============
*/
void ScrCmd_LinkTo( scr_entref_t entref )
{
	gentity_t *ent;
	gentity_t *parent;
	unsigned int tagName;
	int numParam;
	vec3_t originOffset;
	vec3_t anglesOffset;

	ent = GetEntity(entref);

	if ( Scr_GetType(0) != VAR_POINTER || Scr_GetPointerType(0) != VAR_ENTITY )
	{
		Scr_ParamError(0, "not an entity");
	}

	if ( !(ent->flags & FL_SUPPORTS_LINKTO) )
	{
		Scr_ObjectError(va("entity (classname: '%s') does not currently support linkTo", SL_ConvertToString(ent->classname)));
	}

	parent = Scr_GetEntity(0);
	numParam = Scr_GetNumParam();
	tagName = 0;

	if ( numParam >= 2 )
	{
		tagName = Scr_GetConstLowercaseString(1);

		if ( !SL_ConvertToString(tagName)[0] )
		{
			tagName = 0;
		}
	}

	if ( numParam <= 2 )
	{
		if ( G_EntLinkTo(ent, parent, tagName) )
		{
			return;
		}
	}
	else
	{
		Scr_GetVector(2, originOffset);
		Scr_GetVector(3, anglesOffset);

		if ( G_EntLinkToWithOffset(ent, parent, tagName, originOffset, anglesOffset) )
		{
			return;
		}
	}

	if ( !SV_DObjExists(parent) )
	{
		if ( !parent->model )
		{
			Scr_Error("failed to link entity since parent has no model");
		}

		Scr_Error(va("failed to link entity since parent model '%s' is invalid", G_ModelName(parent->model)));
	}

	assert(parent->model);

	if ( tagName )
	{
		if ( SV_DObjGetBoneIndex(parent, tagName) < 0 )
		{
			SV_DObjDumpInfo(parent);
			Scr_Error(va("failed to link entity since tag '%s' does not exist in parent model '%s'", SL_ConvertToString(tagName), G_ModelName(parent->model)));
		}
	}

	Scr_Error("failed to link entity due to link cycle");
}

/*
===============
ScrCmd_Unlink
===============
*/
void ScrCmd_Unlink( scr_entref_t entref )
{
	gentity_t *ent = GetEntity(entref);

	G_EntUnlink(ent);
}

/*
===============
ScrCmd_EnableLinkTo
===============
*/
void ScrCmd_EnableLinkTo( scr_entref_t entref )
{
	gentity_t *ent = GetEntity(entref);

	if ( ent->flags & FL_SUPPORTS_LINKTO )
	{
		Scr_ObjectError("entity already has linkTo enabled");
	}

	if ( ent->s.eType != ET_GENERAL || ent->physicsObject )
	{
		Scr_ObjectError(va("entity (classname: '%s') does not currently support enableLinkTo", SL_ConvertToString(ent->classname)));
	}

	assert(!ent->client);
	ent->flags |= FL_SUPPORTS_LINKTO;
}

/*
===============
ScrCmd_GetOrigin
===============
*/
void ScrCmd_GetOrigin( scr_entref_t entref )
{
	gentity_t *ent;
	vec3_t origin;

	ent = GetEntity(entref);

	VectorCopy(ent->r.currentOrigin, origin);
	Scr_AddVector(origin);
}

/*
===============
ScrCmd_GetEye
===============
*/
void ScrCmd_GetEye( scr_entref_t entref )
{
	gentity_t *ent;
	vec3_t origin;

	ent = GetEntity(entref);
	VectorCopy(ent->r.currentOrigin, origin);
	origin[2] = origin[2] + 40.0f;
	Scr_AddVector(origin);
}

/*
===============
ScrCmd_UseBy
===============
*/
void ScrCmd_UseBy( scr_entref_t entref )
{
	gentity_t *pEnt;
	gentity_t *pOther;
	void (*use)(gentity_t *, gentity_t *, gentity_t *);

	pEnt = GetEntity(entref);
	pOther = Scr_GetEntity(0);

	Scr_AddEntity(pOther);
	Scr_Notify(pEnt, scr_const.trigger, 1);

	use = entityHandlers[pEnt->handler].use;

	if ( use )
	{
		use(pEnt, pOther, pOther);
	}
}

/*
===============
ScrCmd_IsTouching
===============
*/
void ScrCmd_IsTouching( scr_entref_t entref )
{
	gentity_t *pEnt;
	gentity_t *pOther;
	gentity_t *pTemp;
	vec3_t vMins;
	vec3_t vMaxs;
	int bTouching;

	bTouching = 0;
	pEnt = GetEntity(entref);

	if ( pEnt->r.bmodel || pEnt->r.svFlags & ( SVF_CYLINDER | SVF_DISK ) )
	{
		pTemp = pEnt;
		pEnt = Scr_GetEntity(0);

		if ( pEnt->r.bmodel || pEnt->r.svFlags & ( SVF_CYLINDER | SVF_DISK ) )
		{
			Scr_Error("istouching cannot be called on 2 brush/cylinder entities");
		}

		pOther = pTemp;
	}
	else
	{
		pOther = Scr_GetEntity(0);
	}

	assert(pEnt);
	assert(pEnt->r.maxs[0] >= pEnt->r.mins[0]);
	assert(pEnt->r.maxs[1] >= pEnt->r.mins[1]);
	assert(pEnt->r.maxs[2] >= pEnt->r.mins[2]);

	VectorAdd(pEnt->r.currentOrigin, pEnt->r.mins, vMins);
	VectorAdd(pEnt->r.currentOrigin, pEnt->r.maxs, vMaxs);

	ExpandBoundsToWidth(vMins, vMaxs);
	bTouching = SV_EntityContact(vMins, vMaxs, pOther);
	Scr_AddInt( bTouching );
}

/*
===============
ScrCmd_SoundExists
===============
*/
void ScrCmd_SoundExists()
{
	const char *name;

	name = Scr_GetString(0);
	Scr_AddBool(Com_FindSoundAlias(name) != 0);
}

/*
===============
ScrCmd_PlaySound
===============
*/
void ScrCmd_PlaySound( scr_entref_t entref )
{
	gentity_t *ent;
	const char *aliasName;
	unsigned char soundIndex;
	gentity_t *tempEnt;

	ent = GetEntity(entref);
	aliasName = Scr_GetString(0);
	soundIndex = G_SoundAliasIndex(aliasName);

	tempEnt = G_TempEntity(ent->r.currentOrigin, EV_SOUND_ALIAS);

	tempEnt->r.svFlags |= SVF_BROADCAST;
	tempEnt->s.eventParm = soundIndex;
}

/*
===============
ScrCmd_PlaySoundAsMaster
===============
*/
void ScrCmd_PlaySoundAsMaster(scr_entref_t entref)
{
	gentity_t *ent;
	const char *aliasName;
	unsigned char soundIndex;
	gentity_t *tempEnt;

	ent = GetEntity(entref);
	aliasName = Scr_GetString(0);
	soundIndex = G_SoundAliasIndex(aliasName);

	tempEnt = G_TempEntity(ent->r.currentOrigin, EV_SOUND_ALIAS_AS_MASTER);

	tempEnt->r.svFlags |= SVF_BROADCAST;
	tempEnt->s.eventParm = soundIndex;
}

/*
===============
ScrCmd_PlayLoopSound
===============
*/
void ScrCmd_PlayLoopSound( scr_entref_t entref )
{
	gentity_t *pEnt;
	const char *string;
	unsigned char soundIndex;

	pEnt = GetEntity(entref);
	string = Scr_GetString(0);
	soundIndex = G_SoundAliasIndex(string);

	pEnt->r.broadcastTime = -1;
	pEnt->s.loopSound = soundIndex;
}

/*
===============
ScrCmd_StopLoopSound
===============
*/
void ScrCmd_StopLoopSound( scr_entref_t entref )
{
	gentity_t *ent = GetEntity(entref);

	ent->r.broadcastTime = level.time + 300;
	ent->s.loopSound = 0;
}

/*
===============
ScrCmd_Delete
===============
*/
void ScrCmd_Delete( scr_entref_t entref )
{
	gentity_t *pEnt = GetEntity(entref);

	if ( pEnt->client )
	{
		Scr_Error("Cannot delete a client entity");
	}

	if ( level.currentEntityThink == pEnt->s.number )
	{
		Scr_Error("Cannot delete entity during its think");
	}

	Scr_Notify(pEnt, scr_const.death, 0);
	G_FreeEntity(pEnt);
}

/*
===============
ScrCmd_SetModel
===============
*/
void ScrCmd_SetModel( scr_entref_t entref )
{
	gentity_t *pEnt;
	const char *name;

	pEnt = GetEntity(entref);
	name = Scr_GetString(0);

	G_SetModel(pEnt, name);
	G_DObjUpdate(pEnt);
	SV_LinkEntity(pEnt);
}

/*
===============
ScrCmd_GetNormalHealth
===============
*/
void ScrCmd_GetNormalHealth( scr_entref_t entref )
{
	gentity_t *ent = GetEntity(entref);

	if ( ent->client )
	{
		if ( ent->health )
		{
			Scr_AddFloat( (float)ent->health / (float)ent->client->sess.maxHealth );
		}
		else
		{
			Scr_AddFloat(0);
		}

		return;
	}

	Scr_AddFloat(ent->health);
}

/*
===============
ScrCmd_SetNormalHealth
===============
*/
void ScrCmd_SetNormalHealth( scr_entref_t entref )
{
	gentity_t *ent;
	float normalHealth;
	int newHealth;

	ent = GetEntity(entref);
	normalHealth = Scr_GetFloat(0);

	if ( normalHealth > 1 )
	{
		normalHealth = 1;
	}

	if ( ent->client )
	{
		newHealth = I_fround(ent->client->sess.maxHealth * normalHealth);
		SV_GameSendServerCommand(ent - g_entities, SV_CMD_CAN_IGNORE, va("%c \"%i\"", 73, 0));
	}
	else if ( ent->maxHealth )
	{
		newHealth = ent->maxHealth * normalHealth;
	}
	else
	{
		newHealth = normalHealth;
	}

	if ( newHealth <= 0 )
	{
		Com_Printf("ERROR: Cannot setnormalhealth to 0 or below.\n");
		return;
	}

	ent->health = newHealth;
}

/*
===============
ScrCmd_Show
===============
*/
void ScrCmd_Show( scr_entref_t entref )
{
	gentity_t *pEnt = GetEntity(entref);

	pEnt->flags &= ~FL_NODRAW;

	pEnt->r.clientMask[0] = 0;
	pEnt->r.clientMask[1] = 0;
}

/*
===============
ScrCmd_Hide
===============
*/
void ScrCmd_Hide( scr_entref_t entref )
{
	gentity_t *pEnt = GetEntity(entref);

	pEnt->flags |= FL_NODRAW;

	pEnt->r.clientMask[0] = -1;
	pEnt->r.clientMask[1] = -1;
}

/*
===============
ScrCmd_ShowToPlayer
===============
*/
void ScrCmd_ShowToPlayer( scr_entref_t entref )
{
	gentity_t *pEnt = GetEntity(entref);
	gentity_t *clientEnt = Scr_GetEntity(0);

	if ( clientEnt->s.number >= MAX_CLIENTS )
	{
		Scr_Error("showToClient error: param must be a client entity\n");
		return;
	}

	pEnt->flags &= ~FL_NODRAW;
	pEnt->r.clientMask[clientEnt->s.number >> 5] &= ~(1 << (clientEnt->s.number & 0x1F));
}

/*
===============
ScrCmd_SetContents
===============
*/
void ScrCmd_SetContents( scr_entref_t entref )
{
	gentity_t *ent = GetEntity(entref);
	int contents = Scr_GetInt(0);

	Scr_AddInt(ent->r.contents);
	ent->r.contents = contents;

	SV_LinkEntity(ent);
}

/*
===============
Scr_SetStableMissile
===============
*/
void Scr_SetStableMissile( scr_entref_t entref )
{
	gentity_t *ent = GetEntity(entref);
	int stableMissile = Scr_GetInt(0);

	if ( ent->s.eType != ET_PLAYER )
	{
		Scr_Error("Type should be a player");
	}

	if ( stableMissile )
		ent->flags |= FL_STABLE_MISSILES;
	else
		ent->flags &= ~FL_STABLE_MISSILES;
}

/*
===============
GScr_SetCursorHint
===============
*/
void GScr_SetCursorHint( scr_entref_t entref )
{
	int i;
	gentity_t *pEnt;
	const char *pszHint;

	pEnt = GetEntity(entref);
	assert(pEnt->s.eType != ET_MISSILE);
	pszHint = Scr_GetString(0);

	if ( pEnt->classname == scr_const.trigger_use || pEnt->classname == scr_const.trigger_use_touch )
	{
		if ( !I_stricmp(pszHint, "HINT_INHERIT") )
		{
			pEnt->s.hintType = -1;
			return;
		}
	}

	// the bound exceeds the table; the NULL entry ends the scan
	for ( i = 1; i < 133 && hintStrings[i]; i++ )
	{
		if ( !I_stricmp(pszHint, hintStrings[i]) )
		{
			pEnt->s.hintType = i;
			return;
		}
	}

	Com_Printf("List of valid hint type strings\n");

	if ( pEnt->classname == scr_const.trigger_use || pEnt->classname == scr_const.trigger_use_touch )
	{
		Com_Printf("HINT_INHERIT (for trigger_use or trigger_use_touch entities only)\n");
	}

	for ( i = 1; i < 133 && hintStrings[i]; i++ )
	{
		Com_Printf("%s\n", hintStrings[i]);
	}

	Scr_Error(va("%s is not a valid hint type. See above for list of valid hint types\n", pszHint));
}

/*
===============
G_GetHintStringIndex
===============
*/
int G_GetHintStringIndex( int *piIndex, const char *pszString )
{
	int iConfigNum;
	char szConfigString[MAX_STRING_CHARS];

	for ( iConfigNum = 0; iConfigNum < MAX_HINTSTRINGS; iConfigNum++ )
	{
		SV_GetConfigstring(iConfigNum + CS_HINTSTRINGS, szConfigString, sizeof(szConfigString));

		if ( !szConfigString[0] )
		{
			SV_SetConfigstring(iConfigNum + CS_HINTSTRINGS, pszString);
			*piIndex = iConfigNum;
			return 1;
		}

		if ( !strcmp(pszString, szConfigString) )
		{
			*piIndex = iConfigNum;
			return 1;
		}
	}

	*piIndex = -1;
	return 0;
}

/*
===============
GScr_SetHintString
===============
*/
void GScr_SetHintString( scr_entref_t entref )
{
	int i;
	int iType;
	gentity_t *pEnt;
	char szHint[MAX_STRING_CHARS];

	pEnt = GetEntity(entref);

	if ( pEnt->classname != scr_const.trigger_use && pEnt->classname != scr_const.trigger_use_touch )
	{
		Scr_Error("The setHintString command only works on trigger_use or trigger_use_touch entities.\n");
	}

	iType = Scr_GetType(0);

	if ( iType == VAR_STRING && !I_stricmp(Scr_GetString(0), "") )
	{
		pEnt->s.hintString = (byte)-1;
		return;
	}

	Scr_ConstructMessageString(0, Scr_GetNumParam() - 1, "Hint String", szHint, sizeof(szHint));

	if ( !G_GetHintStringIndex(&i, szHint) )
	{
		Scr_Error(va("Too many different hintstring values. Max allowed is %i different strings", MAX_HINTSTRINGS));
	}

	pEnt->s.hintString = (byte)i;
}

/*
===============
GScr_GetEntityNumber
===============
*/
void GScr_GetEntityNumber( scr_entref_t entref )
{
	gentity_t *ent = GetEntity(entref);

	Scr_AddInt( ent->s.number );
}

/*
===============
GScr_EnableGrenadeTouchDamage
===============
*/
void GScr_EnableGrenadeTouchDamage( scr_entref_t entref )
{
	gentity_t *ent = GetEntity(entref);

	if ( ent->classname != scr_const.trigger_damage )
	{
		Scr_Error("Currently on supported on damage triggers");
	}

	ent->flags |= FL_GRENADE_TOUCH_DAMAGE;
}

/*
===============
GScr_DisableGrenadeTouchDamage
===============
*/
void GScr_DisableGrenadeTouchDamage( scr_entref_t entref )
{
	gentity_t *ent = GetEntity(entref);

	if ( ent->classname != scr_const.trigger_damage )
	{
		Scr_Error("Currently on supported on damage triggers");
	}

	ent->flags &= ~FL_GRENADE_TOUCH_DAMAGE;
}

/*
===============
GScr_EnableGrenadeBounce
===============
*/
void GScr_EnableGrenadeBounce( scr_entref_t entref )
{
	gentity_t *ent;

	ent = GetEntity(entref);
	ent->flags &= ~FL_GRENADE_BOUNCE;
}

/*
===============
GScr_DisableGrenadeBounce
===============
*/
void GScr_DisableGrenadeBounce( scr_entref_t entref )
{
	gentity_t *ent;

	ent = GetEntity(entref);
	ent->flags |= FL_GRENADE_BOUNCE;
}

/*
===============
GScr_EnableAimAssist
===============
*/
void GScr_EnableAimAssist( scr_entref_t entref )
{
	gentity_t *ent = GetEntity(entref);

	if ( !ent->r.bmodel )
	{
		Scr_Error("Currently only supported on entities with brush models");
	}

	ent->s.eFlags |= EF_AIMASSIST;
}

/*
===============
GScr_DisableAimAssist
===============
*/
void GScr_DisableAimAssist( scr_entref_t entref )
{
	gentity_t *ent = GetEntity(entref);

	if ( !ent->r.bmodel )
	{
		Scr_Error("Currently only supported on entities with brush models");
	}

	ent->s.eFlags &= ~EF_AIMASSIST;
}

/*
===============
G_InitObjectives
===============
*/
void G_InitObjectives()
{
	for ( int i = 0; i < MAX_OBJECTIVES; i++ )
	{
		ClearObjective(&level.objectives[i]);
	}
}

/*
===============
ObjectiveStateIndexFromString
===============
*/
qboolean ObjectiveStateIndexFromString( int *piStateIndex, unsigned int stateString )
{
	if ( stateString == scr_const.empty )
	{
		*piStateIndex = OBJST_EMPTY;
	}
	else if ( stateString == scr_const.invisible )
	{
		*piStateIndex = OBJST_INVISIBLE;
	}
	else if ( stateString == scr_const.current )
	{
		*piStateIndex = OBJST_CURRENT;
	}
	else
	{
		*piStateIndex = OBJST_EMPTY;
		return qfalse;
	}

	return qtrue;
}

/*
===============
ClearObjective_OnEntity
===============
*/
void ClearObjective_OnEntity( objective_t *obj )
{
	gentity_t *pEnt;

	if ( obj->entNum != ENTITYNUM_NONE )
	{
		pEnt = &g_entities[obj->entNum];

		if ( pEnt->r.inuse )
		{
			pEnt->r.svFlags &= ~SVF_OBJECTIVE;
		}

		obj->entNum = ENTITYNUM_NONE;
	}
}

/*
===============
ClearObjective
===============
*/
void ClearObjective( objective_t *obj )
{
	obj->state = OBJST_EMPTY;

	VectorClear(obj->origin);

	obj->entNum = ENTITYNUM_NONE;
	obj->teamNum = TEAM_FREE;
	obj->icon = 0;
}

void SetObjectiveIcon( objective_t *obj, int paramNum )
{
	int i;
	const char *shaderName = Scr_GetString(paramNum);

	for ( i = 0; shaderName[i]; i++ )
	{
		if ( shaderName[i] <= 31 || shaderName[i] >= 127 )
		{
			Scr_ParamError(3, va("Illegal character '%c'(ascii %i) in objective icon name: %s\n", shaderName[i], (unsigned char)shaderName[i], shaderName));
		}
	}

	if ( i >= MAX_ICON_NAME_LEN )
	{
		Scr_ParamError(3, va("Objective icon name is too long (> %i): %s\n", MAX_ICON_NAME_LEN - 1, shaderName));
	}

	obj->icon = G_ShaderIndex(shaderName);
}

/*
===============
Scr_Objective_Add
===============
*/
void Scr_Objective_Add()
{
	int numParam = Scr_GetNumParam();

	if ( numParam < 2 )
	{
		Scr_Error("objective_add needs at least the first two parameters out of its parameter list of: index state [string] [position]\n");
	}

	int objNum = Scr_GetInt(0);

	if ( objNum < 0 || objNum >= MAX_OBJECTIVES )
	{
		Scr_ParamError(0, va("index %i is an illegal objective index. Valid indexes are 0 to %i\n", objNum, MAX_OBJECTIVES - 1));
	}

	objective_t *obj = &level.objectives[objNum];
	ClearObjective_OnEntity(obj);

	unsigned short stateName = Scr_GetConstString(1);
	int state;

	if ( !ObjectiveStateIndexFromString(&state, stateName) )
	{
		Scr_ParamError(1, va("Illegal objective state \"%s\". Valid states are \"empty\", \"invisible\", \"current\"\n", SL_ConvertToString(stateName)));
	}

	obj->state = state;

	if ( numParam >= 3 )
	{
		Scr_GetVector(2, obj->origin);
		SnapVector(obj->origin);

		obj->entNum = ENTITYNUM_NONE;

		if ( numParam >= 4 )
		{
			SetObjectiveIcon(obj, 3);
		}
	}

	obj->teamNum = TEAM_NONE;
}

/*
===============
Scr_Objective_Delete
===============
*/
void Scr_Objective_Delete()
{
	int objNum = Scr_GetInt(0);

	if ( objNum < 0 || objNum >= MAX_OBJECTIVES )
	{
		Scr_ParamError(0, va("index %i is an illegal objective index. Valid indexes are 0 to %i\n", objNum, MAX_OBJECTIVES - 1));
	}

	ClearObjective_OnEntity(&level.objectives[objNum]);
	ClearObjective(&level.objectives[objNum]);
}

/*
===============
Scr_Objective_State
===============
*/
void Scr_Objective_State()
{
	int objNum;
	objective_t *obj;
	int state;
	unsigned short stateName;

	objNum = Scr_GetInt(0);

	if ( objNum < 0 || objNum >= MAX_OBJECTIVES )
	{
		Scr_ParamError(0, va("index %i is an illegal objective index. Valid indexes are 0 to %i\n", objNum, MAX_OBJECTIVES - 1));
	}

	obj = &level.objectives[objNum];
	stateName = Scr_GetConstString(1);

	if ( !ObjectiveStateIndexFromString(&state, stateName) )
	{
		Scr_ParamError(1, va("Illegal objective state \"%s\". Valid states are \"empty\", \"invisible\", \"current\"\n", Scr_GetString(1)));
	}

	obj->state = state;

	if ( state == OBJST_EMPTY || state == OBJST_INVISIBLE )
	{
		ClearObjective_OnEntity(obj);
	}
}

/*
===============
Scr_Objective_Icon
===============
*/
void Scr_Objective_Icon()
{
	int objNum = Scr_GetInt(0);

	if ( objNum < 0 || objNum >= MAX_OBJECTIVES )
	{
		Scr_ParamError(0, va("index %i is an illegal objective index. Valid indexes are 0 to %i\n", objNum, MAX_OBJECTIVES - 1));
	}

	SetObjectiveIcon(&level.objectives[objNum], 1);
}

/*
===============
Scr_Objective_Position
===============
*/
void Scr_Objective_Position()
{
	int objNum;
	objective_t *obj;

	objNum = Scr_GetInt(0);

	if ( objNum < 0 || objNum >= MAX_OBJECTIVES )
	{
		Scr_ParamError(0, va("index %i is an illegal objective index. Valid indexes are 0 to %i\n", objNum, MAX_OBJECTIVES - 1));
	}

	obj = &level.objectives[objNum];

	ClearObjective_OnEntity(obj);

	Scr_GetVector(1, obj->origin);
	SnapVector(obj->origin);
}

/*
===============
Scr_Objective_OnEntity
===============
*/
void Scr_Objective_OnEntity()
{
	int objNum;
	objective_t *obj;
	gentity_t *ent;

	objNum = Scr_GetInt(0);

	if ( objNum < 0 || objNum >= MAX_OBJECTIVES )
	{
		Scr_ParamError(0, va("index %i is an illegal objective index. Valid indexes are 0 to %i\n", objNum, MAX_OBJECTIVES - 1));
	}

	obj = &level.objectives[objNum];

	ClearObjective_OnEntity(obj);

	ent = Scr_GetEntity(1);
	ent->r.svFlags |= SVF_OBJECTIVE;

	obj->entNum = ent->s.number;
}

/*
===============
GScr_SetArchive
===============
*/
void Scr_Objective_Current()
{
	int i;
	int numParam;
	int objNum;
	objective_t *obj;
	qboolean makeCurrent[MAX_OBJECTIVES];

	numParam = Scr_GetNumParam();
	memset(makeCurrent, 0, sizeof(makeCurrent));

	for ( i = 0; i < numParam; i++ )
	{
		objNum = Scr_GetInt(i);

		if ( objNum < 0 || objNum >= MAX_OBJECTIVES )
		{
			Scr_ParamError(i, va("index %i is an illegal objective index. Valid indexes are 0 to %i\n", objNum, MAX_OBJECTIVES - 1));
		}

		makeCurrent[objNum] = qtrue;
	}

	for ( objNum = 0; objNum < MAX_OBJECTIVES; objNum++ )
	{
		obj = &level.objectives[objNum];

		if ( makeCurrent[objNum] )
		{
			obj->state = OBJST_CURRENT;
			continue;
		}

		if ( obj->state == OBJST_CURRENT )
		{
			obj->state = OBJST_ACTIVE;
		}
	}
}

/*
===============
GScr_Objective_Team
===============
*/
void GScr_Objective_Team()
{
	int objNum;
	objective_t *obj;
	unsigned short team;

	objNum = Scr_GetInt(0);

	if ( objNum < 0 || objNum >= MAX_OBJECTIVES )
	{
		Scr_ParamError(0, va("index %i is an illegal objective index. Valid indexes are 0 to %i\n", objNum, MAX_OBJECTIVES - 1));
	}

	obj = &level.objectives[objNum];
	team = Scr_GetConstString(1);

	if ( team == scr_const.allies )
	{
		obj->teamNum = TEAM_ALLIES;
	}
	else if ( team == scr_const.axis )
	{
		obj->teamNum = TEAM_AXIS;
	}
	else if ( team == scr_const.none )
	{
		obj->teamNum = TEAM_NONE;
	}
	else
	{
		Scr_ParamError(1, va("Illegal team string '%s'. Must be allies, axis, or none.", SL_ConvertToString(team)));
	}
}

/*
===============
GScr_LogPrint
===============
*/
void GScr_LogPrint()
{
	int i;
	int iNumParms;
	int iStringLen;
	int l;
	char string[MAX_STRING_CHARS];
	const char *pszToken;

	string[0] = 0;
	iStringLen = 0;

	iNumParms = Scr_GetNumParam();

	for ( i = 0; i < iNumParms; i++ )
	{
		pszToken = Scr_GetString(i);
		l = strlen(pszToken);

		if ( iStringLen + l < MAX_STRING_CHARS )
		{
			I_strncat(string, sizeof(string), pszToken);
			iStringLen += l;
		}
		else
		{
			break;
		}
	}

	G_LogPrintf(string);
}

/*
===============
GScr_WorldEntNumber
===============
*/
void GScr_WorldEntNumber()
{
	Scr_AddInt(ENTITYNUM_WORLD);
}

/*
===============
GScr_Obituary
===============
*/
void GScr_Obituary()
{
	gentity_t *pOtherEnt;
	const char *pszWeapon;
	const char *pszMeansOfDeath;
	int iWeaponNum;
	int iMODNum;
	gentity_t *tempEnt;

	pszWeapon = Scr_GetString(2);
	iWeaponNum = G_GetWeaponIndexForName(pszWeapon);
	pszMeansOfDeath = Scr_GetString(3);
	iMODNum = G_IndexForMeansOfDeath(pszMeansOfDeath);
	pOtherEnt = Scr_GetEntity(0);

	tempEnt = G_TempEntity(vec3_origin, EV_OBITUARY);
	tempEnt->s.otherEntityNum = pOtherEnt->s.number;

	if ( Scr_GetType(1) == VAR_POINTER && Scr_GetPointerType(1) == VAR_ENTITY )
	{
		pOtherEnt = Scr_GetEntity(1);
		tempEnt->s.attackerEntityNum = pOtherEnt->s.number;
	}
	else
	{
		tempEnt->s.attackerEntityNum = ENTITYNUM_WORLD;
	}

	tempEnt->r.svFlags = SVF_BROADCAST;

	if ( iMODNum == MOD_MELEE
	        || iMODNum == MOD_HEAD_SHOT
	        || iMODNum == MOD_SUICIDE
	        || iMODNum == MOD_FALLING
	        || iMODNum == MOD_CRUSH )
	{
		tempEnt->s.eventParm = iMODNum | 0x80;
	}
	else
	{
		tempEnt->s.eventParm = iWeaponNum;
	}
}

/*
===============
GScr_positionWouldTelefrag
===============
*/
void GScr_positionWouldTelefrag()
{
	int i, num;
	int entityList[MAX_GENTITIES];
	gentity_t *e;
	vec3_t mins;
	vec3_t maxs;
	vec3_t vPos;

	Scr_GetVector(0, vPos);

	VectorAdd(vPos, playerMins, mins);
	VectorAdd(vPos, playerMaxs, maxs);

	num = CM_AreaEntities(mins, maxs, entityList, MAX_GENTITIES, CONTENTS_BODY);

	for ( i = 0; i < num; i++ )
	{
		e = &g_entities[entityList[i]];

		if ( e->client )
		{
			if ( e->client->ps.pm_type < PM_DEAD )
			{
				Scr_AddInt(true);
				return;
			}
		}
	}

	Scr_AddInt(false);
}

/*
===============
GScr_getStartTime
===============
*/
void GScr_getStartTime()
{
	Scr_AddInt(level.startTime);
}

/*
===============
GScr_PrecacheMenu
===============
*/
void GScr_PrecacheMenu()
{
	int iConfigNum;
	const char *pszNewMenu;
	char szConfigString[MAX_STRING_CHARS];

	pszNewMenu = Scr_GetString(0);

	for ( iConfigNum = 0; iConfigNum < MAX_SCRIPT_MENUS; iConfigNum++ )
	{
		SV_GetConfigstring(iConfigNum + CS_SCRIPT_MENUS, szConfigString, sizeof(szConfigString));

		if ( !I_stricmp(szConfigString, pszNewMenu) )
		{
			Com_DPrintf("Script tried to precache the menu '%s' more than once\n", pszNewMenu);
			return;
		}
	}

	for ( iConfigNum = 0; iConfigNum < MAX_SCRIPT_MENUS; iConfigNum++ )
	{
		SV_GetConfigstring(iConfigNum + CS_SCRIPT_MENUS, szConfigString, sizeof(szConfigString));

		if ( !szConfigString[0] )
		{
			break;
		}
	}

	if ( iConfigNum == MAX_SCRIPT_MENUS )
	{
		Scr_Error(va("Too many menus precached. Max allowed menus is %i", MAX_SCRIPT_MENUS));
	}

	SV_SetConfigstring(iConfigNum + CS_SCRIPT_MENUS, pszNewMenu);
}

/*
===============
GScr_GetScriptMenuIndex
===============
*/
int GScr_GetScriptMenuIndex( const char *pszMenu )
{
	int iConfigNum;
	char szConfigString[MAX_STRING_CHARS];

	for ( iConfigNum = 0; iConfigNum < MAX_SCRIPT_MENUS; iConfigNum++ )
	{
		SV_GetConfigstring(iConfigNum + CS_SCRIPT_MENUS, szConfigString, sizeof(szConfigString));

		if ( !I_stricmp(szConfigString, pszMenu) )
		{
			return iConfigNum;
		}
	}

	Scr_Error(va("Menu '%s' was not precached\n", pszMenu));
	return 0;
}

/*
===============
GScr_PrecacheStatusIcon
===============
*/
void GScr_PrecacheStatusIcon()
{
	int iConfigNum;
	const char *pszNewIcon;
	char szConfigString[MAX_STRING_CHARS];

	pszNewIcon = Scr_GetString(0);

	for ( iConfigNum = 0; iConfigNum < MAX_STATUS_ICONS; iConfigNum++ )
	{
		SV_GetConfigstring(iConfigNum + CS_STATUS_ICONS, szConfigString, sizeof(szConfigString));

		if ( !I_stricmp(szConfigString, pszNewIcon) )
		{
			Com_DPrintf("Script tried to precache the player status icon '%s' more than once\n", pszNewIcon);
			return;
		}
	}

	for ( iConfigNum = 0; iConfigNum < MAX_STATUS_ICONS; iConfigNum++ )
	{
		SV_GetConfigstring(iConfigNum + CS_STATUS_ICONS, szConfigString, sizeof(szConfigString));

		if ( !szConfigString[0] )
		{
			break;
		}
	}

	if ( iConfigNum == MAX_STATUS_ICONS )
	{
		Scr_Error(va("Too many player status icons precached. Max allowed is %i", MAX_STATUS_ICONS));
	}

	SV_SetConfigstring(iConfigNum + CS_STATUS_ICONS, pszNewIcon);
}

/*
===============
GScr_GetStatusIconIndex
===============
*/
int GScr_GetStatusIconIndex( const char *pszIcon )
{
	int iConfigNum;
	char szConfigString[MAX_STRING_CHARS];

	if ( !*pszIcon )
	{
		return 0;
	}

	for ( iConfigNum = 0; iConfigNum < MAX_STATUS_ICONS; iConfigNum++ )
	{
		SV_GetConfigstring(iConfigNum + CS_STATUS_ICONS, szConfigString, sizeof(szConfigString));

		if ( !I_stricmp(szConfigString, pszIcon) )
		{
			return iConfigNum + 1;
		}
	}

	Scr_Error(va("Status icon '%s' was not precached\n", pszIcon));
	return 0;
}

/*
===============
GScr_PrecacheHeadIcon
===============
*/
void GScr_PrecacheHeadIcon()
{
	int iConfigNum;
	const char *pszNewIcon;
	char szConfigString[MAX_STRING_CHARS];

	pszNewIcon = Scr_GetString(0);

	for ( iConfigNum = 0; iConfigNum < MAX_HEADICONS; iConfigNum++ )
	{
		SV_GetConfigstring(iConfigNum + CS_HEAD_ICONS, szConfigString, sizeof(szConfigString));

		if ( !I_stricmp(szConfigString, pszNewIcon) )
		{
			Com_DPrintf("Script tried to precache the player head icon '%s' more than once\n", pszNewIcon);
			return;
		}
	}

	for ( iConfigNum = 0; iConfigNum < MAX_HEADICONS; iConfigNum++ )
	{
		SV_GetConfigstring(iConfigNum + CS_HEAD_ICONS, szConfigString, sizeof(szConfigString));

		if ( !szConfigString[0] )
		{
			break;
		}
	}

	if ( iConfigNum == MAX_HEADICONS )
	{
		Scr_Error(va("Too many player head icons precached. Max allowed is %i", MAX_HEADICONS));
	}

	SV_SetConfigstring(iConfigNum + CS_HEAD_ICONS, pszNewIcon);
}

/*
===============
GScr_GetHeadIconIndex
===============
*/
int GScr_GetHeadIconIndex( const char *pszIcon )
{
	int iConfigNum;
	char szConfigString[MAX_STRING_CHARS];

	if ( !*pszIcon )
	{
		return 0;
	}

	for ( iConfigNum = 0; iConfigNum < MAX_HEADICONS; iConfigNum++ )
	{
		SV_GetConfigstring(iConfigNum + CS_HEAD_ICONS, szConfigString, sizeof(szConfigString));

		if ( !I_stricmp(szConfigString, pszIcon) )
		{
			return iConfigNum + 1;
		}
	}

	Scr_Error(va("Head icon '%s' was not precached\n", pszIcon));
	return 0;
}

/*
===============
Scr_BulletTrace
===============
*/
void Scr_BulletTrace()
{
	vec3_t vStart;
	vec3_t vEnd;
	int iHitSky;
	gentity_t *pIgnoreEnt = NULL;
	int iIgnoreEntNum = ENTITYNUM_NONE;
	int iClipMask = MASK_SHOT;
	trace_t trace;
	int surfType;
	vec3_t endpos;
	vec3_t vNorm;

	Scr_GetVector(0, vStart);
	Scr_GetVector(1, vEnd);

	iHitSky = Scr_GetInt(2);
	if ( !iHitSky )
	{
		iClipMask &= ~CONTENTS_BODY;
	}

	if ( Scr_GetType(3) == VAR_POINTER && Scr_GetPointerType(3) == VAR_ENTITY )
	{
		pIgnoreEnt = Scr_GetEntity(3);
		iIgnoreEntNum = pIgnoreEnt->s.number;
	}

	G_LocationalTrace(&trace, vStart, vEnd, iIgnoreEntNum, iClipMask, NULL);

	Scr_MakeArray();
	Scr_AddFloat(trace.fraction);
	Scr_AddArrayStringIndexed(scr_const.fraction);

	Vec3Lerp(vStart, vEnd, trace.fraction, endpos);

	Scr_AddVector(endpos);
	Scr_AddArrayStringIndexed(scr_const.position);

	if ( trace.entityNum == ENTITYNUM_NONE || trace.entityNum == ENTITYNUM_WORLD )
		Scr_AddUndefined();
	else
		Scr_AddEntity(&g_entities[trace.entityNum]);

	Scr_AddArrayStringIndexed(scr_const.entity);

	if ( trace.fraction < 1.0f )
	{
		Scr_AddVector(trace.normal);
		Scr_AddArrayStringIndexed(scr_const.normal);
		surfType = (unsigned char)(( trace.surfaceFlags & 0x1f00000 ) >> 20);
		Scr_AddString(Com_SurfaceTypeToName(surfType));
		Scr_AddArrayStringIndexed(scr_const.surfacetype);
		return;
	}

	VectorSubtract(vEnd, vStart, vNorm);
	Vec3Normalize(vNorm);

	Scr_AddVector(vNorm);
	Scr_AddArrayStringIndexed(scr_const.normal);
	Scr_AddConstString(scr_const.none);
	Scr_AddArrayStringIndexed(scr_const.surfacetype);
}

void Scr_BulletTracePassed()
{
	vec3_t vStart;
	vec3_t vEnd;
	int iHitSky;
	gentity_t *ent;
	int iIgnoreEntNum;
	int iClipMask;

	ent = NULL;
	iIgnoreEntNum = ENTITYNUM_NONE;
	iClipMask = MASK_SHOT;

	Scr_GetVector(0, vStart);
	Scr_GetVector(1, vEnd);

	iHitSky = Scr_GetInt(2);

	if ( !iHitSky )
	{
		iClipMask &= ~CONTENTS_BODY;
	}

	if ( Scr_GetType(3) == VAR_POINTER && Scr_GetPointerType(3) == VAR_ENTITY )
	{
		ent = Scr_GetEntity(3);
		iIgnoreEntNum = ent->s.number;
	}

	Scr_AddBool(G_LocationalTracePassed(vStart, vEnd, iIgnoreEntNum, iClipMask));
}

/*
===============
Scr_SightTracePassed
===============
*/
void Scr_SightTracePassed()
{
	vec3_t vStart;
	vec3_t vEnd;
	int bHitCharacters;
	gentity_t *pIgnoreEnt;
	int iIgnoreEntNum;
	int iClipMask;
	int hitNum;

	pIgnoreEnt = 0;
	iIgnoreEntNum = ENTITYNUM_NONE;
	iClipMask = CONTENTS_SOLID | CONTENTS_FOILAGE | CONTENTS_SKY | CONTENTS_AI_NOSIGHT | CONTENTS_UNKNOWN | CONTENTS_BODY;

	Scr_GetVector(0, vStart);
	Scr_GetVector(1, vEnd);

	bHitCharacters = Scr_GetInt(2);

	if ( !bHitCharacters )
	{
		iClipMask &= ~CONTENTS_BODY;
	}

	if ( Scr_GetType(3) == VAR_POINTER && Scr_GetPointerType(3) == VAR_ENTITY )
	{
		pIgnoreEnt = Scr_GetEntity(3);
		iIgnoreEntNum = pIgnoreEnt->s.number;
	}

	G_SightTrace(&hitNum, vStart, vEnd, iIgnoreEntNum, iClipMask);

	if ( !hitNum )
	{
		if ( SV_FX_GetVisibility(vStart, vEnd) < 0.2f )
		{
			hitNum = 1;
		}
	}

	Scr_AddBool(hitNum == 0);
}

/*
===============
Scr_PhysicsTrace
===============
*/
void Scr_PhysicsTrace()
{
	vec3_t start;
	vec3_t end;
	vec3_t endpos;
	trace_t trace;

	Scr_GetVector(0, start);
	Scr_GetVector(1, end);

	G_TraceCapsule(&trace, start, vec3_origin, vec3_origin, end, ENTITYNUM_NONE, CONTENTS_SOLID | CONTENTS_GLASS | CONTENTS_MONSTERCLIP | CONTENTS_UNKNOWN);

	Vec3Lerp(start, end, trace.fraction, endpos);
	Scr_AddVector(endpos);
}

/*
===============
Scr_RandomInt
===============
*/
void Scr_RandomInt()
{
	int iMax = Scr_GetInt(0);

	if ( iMax <= 0 )
	{
		Com_Printf("RandomInt parm: %d  ", iMax);
		Scr_Error("RandomInt parm must be positive integer.\n");
	}
	else
	{
		Scr_AddInt( irand(0, iMax) );
	}
}

/*
===============
Scr_RandomFloat
===============
*/
void Scr_RandomFloat()
{
	float max;

	max = Scr_GetFloat(0);
	Scr_AddFloat(flrand(0, max));
}

/*
===============
Scr_RandomIntRange
===============
*/
void Scr_RandomIntRange()
{
	int iMin = Scr_GetInt(0);
	int iMax = Scr_GetInt(1);

	if ( iMax <= iMin )
	{
		Com_Printf("RandomIntRange parms: %d %d ", iMin, iMax);
		Scr_Error("RandomIntRange range must be positive integer.\n");
	}

	Scr_AddInt( irand(iMin, iMax) );
}

/*
===============
Scr_RandomFloatRange
===============
*/
void Scr_RandomFloatRange()
{
	float fMin = Scr_GetFloat(0);
	float fMax = Scr_GetFloat(1);

	if ( fMax <= fMin )
	{
		Com_Printf("Scr_RandomFloatRange parms: %d %d ", fMin, fMax);
		Scr_Error("Scr_RandomFloatRange range must be positive float.\n");
	}

	Scr_AddFloat( flrand(fMin, fMax) );
}

/*
===============
GScr_sin
===============
*/
void GScr_sin()
{
	Scr_AddFloat( sin(Scr_GetFloat(0) * RADINDEG) );
}

/*
===============
GScr_cos
===============
*/
void GScr_cos()
{
	Scr_AddFloat( cos(Scr_GetFloat(0) * RADINDEG) );
}

/*
===============
GScr_tan
===============
*/
void GScr_tan()
{
	float sinT;
	float cosT;

	FastSinCos(Scr_GetFloat(0) * RADINDEG, &sinT, &cosT);

	if ( cosT == 0 )
	{
		Scr_Error("divide by 0");
	}

	Scr_AddFloat( sinT / cosT );
}

/*
===============
GScr_atan
===============
*/
void GScr_atan()
{
	int v3;

	*(float *)&v3 = atan(Scr_GetFloat(0));
	*(float *)&v3 = *(float *)&v3 * DEGINRAD;
	Scr_AddFloat(*(float *)&v3);
}

/*
===============
GScr_CastInt
===============
*/
void GScr_CastInt()
{
	switch ( Scr_GetType(0) )
	{
	case VAR_INTEGER:
		Scr_AddInt( Scr_GetInt(0) );
		return;
	case VAR_FLOAT:
		Scr_AddInt( (int)Scr_GetFloat(0) );
		return;
	case VAR_STRING:
		Scr_AddInt( atoi(Scr_GetString(0)) );
		return;
	}

	Scr_ParamError(0, va("cannot cast %s to int", Scr_GetTypeName(0)));
}

/*
===============
Scr_Distance
===============
*/
void Scr_Distance()
{
	vec3_t a;
	vec3_t b;

	Scr_GetVector(0, a);
	Scr_GetVector(1, b);

	Scr_AddFloat(Vec3Distance(a, b));
}

/*
===============
Scr_DistanceSquared
===============
*/
void Scr_DistanceSquared()
{
	vec3_t a;
	vec3_t b;

	Scr_GetVector(0, a);
	Scr_GetVector(1, b);

	Scr_AddFloat(Vec3DistanceSq(a, b));
}

/*
===============
Scr_Length
===============
*/
void Scr_Length()
{
	vec3_t v;

	Scr_GetVector(0, v);
	Scr_AddFloat(VectorLength(v));
}

/*
===============
Scr_LengthSquared
===============
*/
void Scr_LengthSquared()
{
	vec3_t v;

	Scr_GetVector(0, v);
	Scr_AddFloat(VectorLengthSquared(v));
}

/*
===============
Scr_Closer
===============
*/
void Scr_Closer()
{
	vec3_t vRef;
	vec3_t vA;
	vec3_t vB;
	float fDistA;
	float fDistB;

	Scr_GetVector(0, vRef);
	Scr_GetVector(1, vA);
	Scr_GetVector(2, vB);

	fDistA = Vec3DistanceSq(vA, vRef);
	fDistB = Vec3DistanceSq(vB, vRef);

	Scr_AddInt(fDistA < fDistB);
}

/*
===============
Scr_VectorDot
===============
*/
void Scr_VectorDot()
{
	vec3_t a;
	vec3_t b;

	Scr_GetVector(0, a);
	Scr_GetVector(1, b);

	Scr_AddFloat(DotProduct(a, b));
}

/*
===============
Scr_VectorNormalize
===============
*/
void Scr_VectorNormalize()
{
	vec3_t a;
	vec3_t b;

	Scr_GetVector(0, a);

	VectorCopy(a, b);
	Vec3Normalize(b);

	Scr_AddVector(b);
}

/*
===============
Scr_VectorToAngles
===============
*/
void Scr_VectorToAngles()
{
	vec3_t vec;
	vec3_t angles;

	Scr_GetVector(0, vec);
	vectoangles(vec, angles);
	Scr_AddVector(angles);
}

/*
===============
Scr_AnglesToUp
===============
*/
void Scr_AnglesToUp()
{
	vec3_t angles;
	vec3_t up;

	Scr_GetVector(0, angles);

	AngleVectors(angles, NULL, NULL, up);
	Scr_AddVector(up);
}

/*
===============
Scr_AnglesToRight
===============
*/
void Scr_AnglesToRight()
{
	vec3_t angles;
	vec3_t right;

	Scr_GetVector(0, angles);

	AngleVectors(angles, NULL, right, NULL);
	Scr_AddVector(right);
}

/*
===============
Scr_AnglesToForward
===============
*/
void Scr_AnglesToForward()
{
	vec3_t angles;
	vec3_t forward;

	Scr_GetVector(0, angles);

	AngleVectors(angles, forward, NULL, NULL);
	Scr_AddVector(forward);
}

/*
===============
Scr_IsSubStr
===============
*/
void Scr_IsSubStr()
{
	Scr_AddBool( strstr( Scr_GetString(0), Scr_GetString(1) ) != NULL );
}

/*
===============
Scr_GetSubStr
===============
*/
void Scr_GetSubStr()
{
	char tempString[MAX_STRING_CHARS];
	const char *s;
	int startParam;
	int end;
	int start;
	int i;
	char c;

	s = Scr_GetString(0);
	startParam = Scr_GetInt(1);
	end = Scr_GetNumParam() >= 3 ? Scr_GetInt(2) : INT_MAX;
	start = startParam;

	for ( i = 0; start < end; start++, i++ )
	{
		if ( i >= (int)sizeof(tempString) )
		{
			Scr_Error("string too long");
		}

		c = s[start];

		if ( !c )
		{
			break;
		}

		tempString[i] = c;
	}

	tempString[i] = 0;
	Scr_AddString(tempString);
}

/*
===============
Scr_StrTok
===============
*/
void Scr_StrTok()
{
	char tempString[MAX_STRING_CHARS];
	const char *s;
	const char *delim;
	int delimLen;
	int source;
	int dest;
	char c;
	int i;

	s = Scr_GetString(0);
	delim = Scr_GetString(1);
	delimLen = strlen(delim);
	dest = 0;

	Scr_MakeArray();

	for ( source = 0; ; source++ )
	{
		c = s[source];

		if ( !c )
		{
			break;
		}

		for ( i = 0; i < delimLen; i++ )
		{
			if ( c != delim[i] )
			{
				continue;
			}

			if ( dest )
			{
				tempString[dest] = 0;

				Scr_AddString(tempString);
				Scr_AddArray();

				dest = 0;
			}

			goto nextChar;
		}

		tempString[dest] = c;
		dest++;

		if ( dest >= (int)sizeof(tempString) )
		{
			Scr_Error("string too long");
		}
nextChar:
		;
	}

	if ( dest )
	{
		tempString[dest] = 0;

		Scr_AddString(tempString);
		Scr_AddArray();

		dest = 0;
	}
}

/*
===============
Scr_MusicPlay
===============
*/
void Scr_MusicPlay()
{
	SV_GameSendServerCommand(-1, SV_CMD_RELIABLE, va("%c %s", 111, Scr_GetString(0)));
}

/*
===============
Scr_MusicStop
===============
*/
void Scr_MusicStop()
{
	int iFadeTime;

	switch ( Scr_GetNumParam() )
	{
	case 1:
		iFadeTime = I_fround(Scr_GetFloat(0) * 1000.0f);
		break;

	case 0:
		iFadeTime = 0;
		break;

	default:
		Scr_Error(va("USAGE: musicStop([fadetime]);\n"));
		return;
	}

	if ( iFadeTime < 0 )
	{
		Scr_Error(va("musicStop: fade time must be >= 0\n"));
	}

	SV_GameSendServerCommand(-1, SV_CMD_RELIABLE, va("%c %i", 112, iFadeTime));
}

/*
===============
Scr_SoundFade
===============
*/
void Scr_SoundFade()
{
	float fTargetVol;
	int iFadeTime;

	fTargetVol = Scr_GetFloat(0);

	if ( Scr_GetNumParam() > 1 )
	{
		iFadeTime = (int)(Scr_GetFloat(1) * 1000.0f);
	}
	else
	{
		iFadeTime = 0;
	}

	SV_GameSendServerCommand(-1, SV_CMD_RELIABLE, va("%c %f %i\n", 113, fTargetVol, iFadeTime));
}

/*
===============
Scr_PrecacheModel
===============
*/
void Scr_PrecacheModel()
{
	if ( !level.initializing )
	{
		Scr_Error("precacheModel must be called before any wait statements in the gametype or level script\n");
	}

	G_ModelIndex(Scr_GetString(0));
}

/*
===============
Scr_PrecacheShellShock
===============
*/
void Scr_PrecacheShellShock()
{
	if ( !level.initializing )
	{
		Scr_Error("precacheShellShock must be called before any wait statements in the gametype or level script\n");
	}

	G_ShellShockIndex(Scr_GetString(0));
}

/*
===============
Scr_PrecacheItem
===============
*/
void Scr_PrecacheItem()
{
	if ( !level.initializing )
	{
		Scr_Error("precacheItem must be called before any wait statements in the gametype or level script\n");
	}

	const char *pszItemName = Scr_GetString(0);
	gitem_t *item = G_FindItem(pszItemName);

	if ( !item )
	{
		Scr_ParamError(0, va("unknown item '%s'", pszItemName));
	}

	int iItemIndex = item - bg_itemlist;

	RegisterItem(iItemIndex, qtrue);
}

/*
===============
Scr_PrecacheShader
===============
*/
void Scr_PrecacheShader()
{
	if ( !level.initializing )
	{
		Scr_Error("precacheShader must be called before any wait statements in the gametype or level script\n");
	}

	const char *shaderName = Scr_GetString(0);

	if ( !shaderName[0] )
	{
		Scr_ParamError(0, "Shader name string is empty");
	}

	G_ShaderIndex(shaderName);
}

/*
===============
Scr_PrecacheString
===============
*/
void Scr_PrecacheString()
{
	if ( !level.initializing )
	{
		Scr_Error("precacheString must be called before any wait statements in the gametype or level script\n");
	}

	const char *string = Scr_GetIString(0);

	if ( string[0] )
	{
		G_LocalizedStringIndex(string);
	}
}

/*
===============
Scr_AmbientPlay
===============
*/
void Scr_AmbientPlay()
{
	int iFadeTime;
	const char *name;

	name = "";
	iFadeTime = 0;

	switch ( Scr_GetNumParam() )
	{
	case 2:
		iFadeTime = I_fround(Scr_GetFloat(1) * 1000.0f);
		// falls through
	case 1:
		name = Scr_GetString(0);
		break;

	default:
		Scr_Error(va("USAGE: ambientPlay(alias_name, <fadetime>);\n"));
		return;
	}

	if ( !name[0] )
	{
		Scr_Error(va("ambientPlay: alias name cannot be the empty string... use stop or fade version\n"));
	}

	if ( iFadeTime < 0 )
	{
		Scr_Error(va("ambientPlay: fade time must be >= 0\n"));
	}

	SV_SetConfigstring(CS_AMBIENT, va("n\\%s\\t\\%i", name, level.time + iFadeTime));
}

/*
===============
Scr_AmbientStop
===============
*/
void Scr_AmbientStop()
{
	int iFadeTime;

	switch ( Scr_GetNumParam() )
	{
	case 1:
		iFadeTime = I_fround(Scr_GetFloat(0) * 1000.0f);
		break;

	case 0:
		iFadeTime = 0;
		break;

	default:
		Scr_Error(va("USAGE: ambientStop(<fadetime>);\n"));
		return;
	}

	if ( iFadeTime < 0 )
	{
		Scr_Error(va("ambientStop: fade time must be >= 0\n"));
	}

	SV_SetConfigstring(CS_AMBIENT, va("t\\%i", level.time + iFadeTime));
}

/*
===============
Scr_GrenadeExplosionEffect
===============
*/
void Scr_GrenadeExplosionEffect()
{
	vec3_t vOrg;
	vec3_t vDir;
	vec3_t vPos;
	vec3_t vEnd;
	gentity_t *pEnt;
	trace_t trace;

	Scr_GetVector(0, vOrg);

	VectorCopy(vOrg, vPos);
	vPos[2] += 1;

	pEnt = G_TempEntity(vPos, EV_GRENADE_EXPLODE);

	VectorSet(vDir, 0, 0, 1);
	pEnt->s.eventParm = DirToByte(vDir);
	VectorCopy(vPos, vEnd);
	vEnd[2] -= 17;

	G_TraceCapsule(&trace, vPos, vec3_origin, vec3_origin, vEnd, ENTITYNUM_NONE, MASK_SOLID);
	pEnt->s.surfType = (unsigned char)( ( trace.surfaceFlags & 0x1f00000 ) >> 20 );
}

/*
===============
GScr_RadiusDamage
===============
*/
void GScr_RadiusDamage()
{
	vec3_t origin;

	Scr_GetVector(0, origin);

	float range = Scr_GetFloat(1);
	float max_damage = Scr_GetFloat(2);
	float min_damage = Scr_GetFloat(3);

	level.bPlayerIgnoreRadiusDamage = level.bPlayerIgnoreRadiusDamageLatched;
	G_RadiusDamage(origin, NULL, &g_entities[ENTITYNUM_WORLD], max_damage, min_damage, range, NULL, MOD_EXPLOSIVE);
	level.bPlayerIgnoreRadiusDamage = 0;
}

/*
===============
GScr_SetPlayerIgnoreRadiusDamage
===============
*/
void GScr_SetPlayerIgnoreRadiusDamage()
{
	level.bPlayerIgnoreRadiusDamageLatched = Scr_GetInt(0);
}

/*
===============
GScr_GetMoveDelta
===============
*/
void GScr_GetMoveDelta()
{
	scr_anim_s anim;
	vec2_t rot;
	vec3_t trans;

	float startTime = 0;
	float endTime = 1;

	// cases fall through: 3 params sets end and start time, 2 only start time
	switch ( Scr_GetNumParam() )
	{
	default:
		endTime = Scr_GetFloat(2);

		if ( endTime < 0 || endTime > 1 )
		{
			Scr_ParamError(2, "end time must be between 0 and 1");
		}

	case 2:
		startTime = Scr_GetFloat(1);

		if ( startTime < 0 || startTime > 1 )
		{
			Scr_ParamError(1, "start time must be between 0 and 1");
		}

	case 1:
		break;
	}

	anim = Scr_GetAnim(0, NULL);
	XAnimGetRelDelta(Scr_GetAnims(anim.tree), anim.index, rot, trans, startTime, endTime);

	Scr_AddVector(trans);
}

/*
===============
GScr_GetAngleDelta
===============
*/
void GScr_GetAngleDelta()
{
	scr_anim_s anim;
	vec2_t rot;
	vec3_t trans;

	float startTime = 0;
	float endTime = 1;

	// cases fall through: 3 params sets end and start time, 2 only start time
	switch ( Scr_GetNumParam() )
	{
	default:
		endTime = Scr_GetFloat(2);

		if ( endTime < 0 || endTime > 1 )
		{
			Scr_ParamError(2, "end time must be between 0 and 1");
		}

	case 2:
		startTime = Scr_GetFloat(1);

		if ( startTime < 0 || startTime > 1 )
		{
			Scr_ParamError(1, "start time must be between 0 and 1");
		}

	case 1:
		break;
	}

	anim = Scr_GetAnim(0, NULL);
	XAnimGetRelDelta(Scr_GetAnims(anim.tree), anim.index, rot, trans, startTime, endTime);

	Scr_AddFloat(RotationToYaw(rot));
}

/*
===============
GScr_GetNorthYaw
===============
*/
void GScr_GetNorthYaw()
{
	char northYawString[32];

	SV_GetConfigstring(CS_NORTHYAW, northYawString, sizeof(northYawString));
	Scr_AddFloat(atof(northYawString));
}

/*
===============
Scr_LoadFX
===============
*/
void Scr_LoadFX()
{
	int id;
	const char *filename = Scr_GetString(0);

	id = G_EffectIndex(filename);

	if ( !id && !level.initializing )
	{
		Scr_Error("loadFx must be called before any wait statements in the gametype or level script, or on an already loaded effect\n");
	}

	Scr_AddInt(id);
}

/*
===============
Scr_PlayFX_Error
===============
*/
void Scr_FxParamError( const char *errorString, int fxId )
{
	char fxName[MAX_STRING_CHARS];

	if ( fxId )
		SV_GetConfigstring(fxId + CS_EFFECT_NAMES, fxName, sizeof(fxName));
	else
		strcpy(fxName, "not successfully loaded");

	Scr_Error(va("%s (effect = %s)\n", errorString, fxName));
}

/*
===============
Scr_PlayFX
===============
*/
void Scr_PlayFX()
{
	gentity_t *ent;
	int fxId;
	int numParams;
	float vecLength;
	vec3_t pos;
	vec3_t axis[3];

	numParams = Scr_GetNumParam();

	if ( numParams < 2 || numParams > 4 )
	{
		Scr_Error("Incorrect number of parameters");
	}

	fxId = Scr_GetInt(0);
	Scr_GetVector(1, pos);

	ent = G_TempEntity(pos, EV_PLAY_FX);
	ent->s.eventParm = (unsigned char)fxId;

	if ( numParams == 2 )
	{
		ent->s.apos.trBase[0] = -90;
		return;
	}

	Scr_GetVector(2, axis[0]);
	vecLength = VectorLength(axis[0]);

	if ( vecLength == 0 )
	{
		Scr_FxParamError("playFx called with (0 0 0) forward direction", fxId);
	}

	VectorScale(axis[0], 1.0f / vecLength, axis[0]);

	if ( numParams == 3 )
	{
		vectoangles(axis[0], ent->s.apos.trBase);
		return;
	}

	Scr_GetVector(3, axis[2]);
	vecLength = VectorLength(axis[2]);

	if ( vecLength == 0 )
	{
		Scr_FxParamError("playFx called with (0 0 0) up direction", fxId);
	}

	VectorScale(axis[2], 1.0f / vecLength, axis[2]);
	Vec3Cross(axis[2], axis[0], axis[1]);

	vecLength = VectorLength(axis[1]);

	if ( vecLength < 0.001f )
	{
		Scr_FxParamError("playFx called an up direction 0 or 180 degrees from forward", fxId);
	}
	else if ( vecLength < 0.999f )
	{
		VectorScale(axis[1], 1.0f / vecLength, axis[1]);
		Vec3Cross(axis[0], axis[1], axis[2]);
	}

	AxisToAngles(axis, ent->s.apos.trBase);
}

/*
===============
Scr_PlayFXOnTag
===============
*/
void Scr_PlayFXOnTag()
{
	int fxId;
	int csIndex;
	gentity_t *ent;
	unsigned int tag;

	if ( Scr_GetNumParam() != 3 )
	{
		Scr_Error("USAGE: playFxOnTag <effect id from loadFx> <entity> <tag name>");
	}

	fxId = Scr_GetInt(0);

	if ( fxId <= 0 || fxId >= MAX_EFFECT_NAMES )
	{
		Scr_ParamError(0, va("effect id %i is invalid\n", fxId));
	}

	ent = Scr_GetEntity(1);

	if ( !ent->model )
	{
		Scr_ParamError(1, "cannot play fx on entity with no model");
	}

	tag = Scr_GetConstLowercaseString(2);

	if ( strchr(SL_ConvertToString(tag), '"') )
	{
		Scr_ParamError(2, "cannot use \" characters in tag names\n");
	}

	if ( SV_DObjGetBoneIndex(ent, tag) < 0 )
	{
		SV_DObjDumpInfo(ent);
		Scr_ParamError(2, va("tag '%s' does not exist on entity with model '%s'", tag, G_ModelName(ent->model)));
	}

	csIndex = G_FindConfigstringIndex(va("%02d%s", fxId, SL_ConvertToString(tag)), CS_EFFECT_TAGS, MAX_EFFECT_TAGS, qtrue, NULL);
	assert(csIndex > 0 && csIndex < MAX_EFFECT_TAGS);

	G_AddEvent(ent, EV_PLAY_FX_ON_TAG, csIndex);
}

/*
===============
Scr_PlayLoopedFX
===============
*/
void Scr_PlayLoopedFX()
{
	gentity_t *ent;
	int fxId;
	vec3_t pos;
	vec3_t vForward;
	vec3_t vUp;
	float cullDist;
	int repeat;

	if ( Scr_GetNumParam() < 3 || Scr_GetNumParam() > 6 )
	{
		Scr_Error(
		    "USAGE: playLoopedFx <effect id from loadFx> <repeat delay> <vector position of effect> <optional cull distance (0 "
		    "= never cull)> <optional forward vector> <optional up vector>");
	}

	VectorClear(vForward);
	VectorClear(vUp);

	cullDist = 0;
	fxId = Scr_GetInt(0);

	// optional parms
	switch (Scr_GetNumParam())
	{
	case 6:
		Scr_GetVector(5, vUp);
		if ( Vec3Normalize(vUp) == 0 )
		{
			Scr_PlayFX_Error("playLoopedFx called with (0 0 0) up direction", fxId);
		}
		// fall through

	case 5:
		Scr_GetVector(4, vForward);
		if ( Vec3Normalize(vForward) == 0 )
		{
			Scr_PlayFX_Error("playLoopedFx called with (0 0 0) forward direction", fxId);
		}
		// fall through

	case 4:
		cullDist = Scr_GetFloat(3);
		break;
	}

	Scr_GetVector(2, pos);
	repeat = I_fround(Scr_GetFloat(1) * 1000);

	if ( repeat <= 0 )
	{
		Scr_Error(va("playLoopedFx called with %f s repeat (should be >= 0.0005s)\n", repeat));
	}

	ent = G_Spawn();

	ent->s.eType = ET_LOOP_FX;
	ent->r.svFlags |= SVF_BROADCAST;
	ent->s.hintString = (byte)fxId;
	assert(ent->s.eventParm2 == (byte)fxId);

	G_SetOrigin(ent, pos);
	VectorCopy(vForward, ent->s.origin2);

	ent->s.eventParm = DirToByte(vUp);
	ent->s.cullDist = cullDist;
	ent->s.period = repeat;

	SV_LinkEntity(ent);
	Scr_AddEntity(ent);
}

/*
===============
Scr_SetFog
===============
*/
void Scr_SetFog( const char *cmd, float start, float density, float heightDensity, float r, float g, float b, float fTime )
{

if ( start < 0 )
	{
		Scr_Error(va("%s: near distance must be >= 0", cmd));
	}

	if ( density <= start )
	{
		Scr_Error(va("%s: near distance must be less than far distance", cmd));
	}

	if ( r < 0 || r > 1 || g < 0 || g > 1 || b < 0 || b > 1 )
	{
		Scr_Error(va("%s: red/green/blue color components must be in the range [0, 1]", cmd));
	}

	if ( fTime < 0 )
	{
		Scr_Error(va("%s: transition time must be >= 0 seconds", cmd));
	}

	G_setfog(va("%g %g %g %g %g %g %.0f", start, density, heightDensity, r, g, b, fTime * 1000));
}

/*
===============
Scr_SetLinearFog
===============
*/
void Scr_SetLinearFog()
{
	if ( Scr_GetNumParam() != 6 )
	{
		Scr_Error("USAGE: setCullFog(near distance, far distance, red, green, blue, transition time);\n");
	}

	float nearDist = Scr_GetFloat(0);
	float farDist = Scr_GetFloat(1);
	float red = Scr_GetFloat(2);
	float green = Scr_GetFloat(3);
	float blue = Scr_GetFloat(4);
	float fTime = Scr_GetFloat(5);

	Scr_SetFog("setCullFog", nearDist, farDist, 1.0, red, green, blue, fTime);
}

/*
===============
Scr_SetExponentialFog
===============
*/
void Scr_SetExponentialFog()
{
	if ( Scr_GetNumParam() != 5 )
	{
		Scr_Error(
		    "USAGE: setExpFog(density, red, green, blue, transition time);\n"
		    "Density must be greater than 0 and less than 1, and typically less than .001.  For example, .0002 means the fog ge"
		    "ts .02%% more dense for every 1 unit of distance (about 1%% thicker every 50 units of distance)\n");
	}

	float density = Scr_GetFloat(0);
	float red = Scr_GetFloat(1);
	float green = Scr_GetFloat(2);
	float blue = Scr_GetFloat(3);
	float fTime = Scr_GetFloat(4);

	if ( density <= 0 || density >= 1 )
	{
		Scr_Error("setExpFog: distance must be greater than 0 and less than 1");
	}

	Scr_SetFog("setExpFog", 0, 1.0, density, red, green, blue, fTime);
}

/*
===============
GScr_IsPlayer
===============
*/
void GScr_IsPlayer()
{
	gentity_t *v1;

	if ( Scr_GetType(0) == VAR_POINTER && Scr_GetPointerType(0) == VAR_ENTITY && (v1 = Scr_GetEntity(0))->client != NULL )
	{
		Scr_AddInt(1);
	}
	else
	{
		Scr_AddInt(0);
	}
}

/*
===============
GScr_IsPlayerNumber
===============
*/
void GScr_IsPlayerNumber()
{
	int v1;

	v1 = Scr_GetInt(0);

	if ( v1 < 0 || v1 >= 64 )
	{
		Scr_AddInt(0);
	}
	else
	{
		Scr_AddInt(1);
	}
}

/*
===============
GScr_SetWinningPlayer
===============
*/
void GScr_SetWinningPlayer()
{
	int iWinner;
	const char *pszWinner;
	char buffer[MAX_INFO_STRING];
	gentity_t *ent;

	ent = Scr_GetEntity(0);
	iWinner = ent->s.number + 1;
	SV_GetConfigstring(CS_MULTI_MAPWINNER, buffer, sizeof(buffer));
	pszWinner = va("%i", iWinner);

	if ( !I_stricmp(Info_ValueForKey(buffer, "winner"), pszWinner) )
	{
		return;
	}

	Info_SetValueForKey(buffer, "winner", pszWinner);
	SV_SetConfigstring(CS_MULTI_MAPWINNER, buffer);
}

/*
===============
GScr_SetWinningTeam
===============
*/
void GScr_SetWinningTeam()
{
	unsigned short team;
	int iWinner;
	const char *pszWinner;
	char buffer[MAX_INFO_STRING];

	team = Scr_GetConstString(0);

	if ( team == scr_const.allies )
	{
		iWinner = -TEAM_ALLIES;
	}
	else if ( team == scr_const.axis )
	{
		iWinner = -TEAM_AXIS;
	}
	else if ( team == scr_const.none )
	{
		iWinner = TEAM_NONE;
	}
	else
	{
		Scr_ParamError(0, va("Illegal team string '%s'. Must be allies, axis, or none.", SL_ConvertToString(team)));
		return;
	}

	SV_GetConfigstring(CS_MULTI_MAPWINNER, buffer, sizeof(buffer));

	pszWinner = va("%i", iWinner);

	if ( !I_stricmp(Info_ValueForKey(buffer, "winner"), pszWinner) )
	{
		return;
	}

	Info_SetValueForKey(buffer, "winner", pszWinner);
	SV_SetConfigstring(CS_MULTI_MAPWINNER, buffer);
}

/*
===============
GScr_Announcement
===============
*/
void GScr_Announcement()
{
	char string[MAX_STRING_CHARS];

	Scr_ConstructMessageString(0, Scr_GetNumParam() - 1, "Announcement", string, sizeof(string));
	SV_GameSendServerCommand(-1, SV_CMD_CAN_IGNORE, va("%c \"%s\" 2", 99, string));
}

/*
===============
GScr_ClientAnnouncement
===============
*/
void GScr_ClientAnnouncement()
{
	gentity_t *ent;
	char string[MAX_STRING_CHARS];

	ent = Scr_GetEntity(0);

	Scr_ConstructMessageString(1, Scr_GetNumParam() - 1, "Announcement", string, sizeof(string));
	SV_GameSendServerCommand(ent->s.number, SV_CMD_CAN_IGNORE, va("%c \"%s\" 2", 99, string));
}

/*
===============
GScr_GetTeamScore
===============
*/
void GScr_GetTeamScore()
{
	unsigned short team = Scr_GetConstString(0);

	if ( team != scr_const.allies && team != scr_const.axis )
	{
		Scr_Error(va("Illegal team string '%s'. Must be allies, or axis.", SL_ConvertToString(team)));
	}

	int score;

	if ( team == scr_const.allies )
	{
		score = level.teamScores[TEAM_ALLIES];
	}
	else
	{
		score = level.teamScores[TEAM_AXIS];
	}

	Scr_AddInt(score);
}

/*
===============
GScr_SetTeamScore
===============
*/
void GScr_SetTeamScore()
{
	unsigned short team = Scr_GetConstString(0);

	if ( team != scr_const.allies && team != scr_const.axis )
	{
		Scr_Error(va("Illegal team string '%s'. Must be allies, or axis.", SL_ConvertToString(team)));
	}

	int score = Scr_GetInt(1);

	if ( team == scr_const.allies )
	{
		level.teamScores[TEAM_ALLIES] = score;
		SV_GameSendServerCommand(-1, SV_CMD_CAN_IGNORE, va("%c %i", 72, level.teamScores[TEAM_ALLIES]));
	}
	else
	{
		level.teamScores[TEAM_AXIS] = score;
		SV_GameSendServerCommand(-1, SV_CMD_CAN_IGNORE, va("%c %i", 71, level.teamScores[TEAM_AXIS]));
	}

	level.bUpdateScoresForIntermission = qtrue;
}

/*
===============
GScr_SetClientNameMode
===============
*/
void GScr_SetClientNameMode()
{
	unsigned short mode = Scr_GetConstString(0);

	if ( mode == scr_const.auto_change )
	{
		level.manualNameChange = qfalse;
	}
	else if ( mode == scr_const.manual_change )
	{
		level.manualNameChange = qtrue;
	}
	else
	{
		Scr_Error("Unknown mode");
	}
}

/*
===============
GScr_UpdateClientNames
===============
*/
void GScr_UpdateClientNames()
{
	gclient_t *cl;
	char oldname[MAX_NAME_LENGTH];
	int i;

	if ( !level.manualNameChange )
	{
		Scr_Error("Only works in [manual_change] mode");
	}

	for ( i = 0, cl = level.clients; i < level.maxclients ; i++, cl++ )
	{
		if ( cl->sess.connected != CON_CONNECTED )
		{
			continue;
		}

		if ( !strcmp(cl->sess.cs.name, cl->sess.newnetname) )
		{
			continue;
		}

		I_strncpyz(oldname, cl->sess.cs.name, sizeof(oldname));
		I_strncpyz(cl->sess.cs.name, cl->sess.newnetname, sizeof(cl->sess.cs.name));

		ClientUserinfoChanged(i);
	}
}

/*
===============
GScr_GetTeamPlayersAlive
===============
*/
void GScr_GetTeamPlayersAlive()
{
	unsigned short team;
	int iLivePlayers;
	int iTeamNum;
	int i;
	gentity_t *pEnt;

	team = Scr_GetConstString(0);

	if ( team != scr_const.allies && team != scr_const.axis )
	{
		Scr_Error(va("Illegal team string '%s'. Must be allies, or axis.", SL_ConvertToString(team)));
	}

	if ( team == scr_const.allies )
		iTeamNum = TEAM_ALLIES;
	else
		iTeamNum = TEAM_AXIS;

	iLivePlayers = 0;

	for ( i = 0; i < g_maxclients->current.integer; i++ )
	{
		pEnt = &g_entities[i];

		if ( pEnt->r.inuse && pEnt->client->sess.cs.team == iTeamNum && pEnt->health > 0 )
		{
			iLivePlayers++;
		}
	}

	Scr_AddInt(iLivePlayers);
}

/*
===============
GScr_GetNumParts
===============
*/
void GScr_GetNumParts()
{
	XModel *model = SV_XModelGet(Scr_GetString(0));

	Scr_AddInt(XModelNumBones(model));
}

/*
===============
GScr_GetPartName
===============
*/
void GScr_GetPartName()
{
	XModel *model;
	unsigned int index;
	unsigned int numbones;
	unsigned short name;

	model = SV_XModelGet(Scr_GetString(0));
	index = Scr_GetInt(1);

	numbones = XModelNumBones(model);

	if ( index >= numbones )
	{
		Scr_ParamError(1, va("index out of range (0 - %d)", numbones - 1));
	}

	name = XModelBoneNames(model)[index];

	if ( !name )
	{
		Scr_ParamError(0, "bad model");
	}

	Scr_AddConstString(name);
}

/*
===============
GScr_Earthquake
===============
*/
void GScr_Earthquake()
{
	gentity_t *tent;
	float scale;
	int duration;
	vec3_t source;
	float radius;

	scale = Scr_GetFloat(0);
	duration = I_fround(Scr_GetFloat(1) * 1000);
	Scr_GetVector(2, source);
	radius = Scr_GetFloat(3);

	if ( scale <= 0 )
	{
		Scr_ParamError(0, "Scale must be greater than 0");
	}

	if ( duration <= 0 )
	{
		Scr_ParamError(1, "duration must be greater than 0");
	}

	if ( radius <= 0 )
	{
		Scr_ParamError(3, "Radius must be greater than 0");
	}

	tent = G_TempEntity(source, EV_EARTHQUAKE);

	tent->s.scale2 = scale;
	tent->s.time = duration;
	tent->s.radius = radius;
}

/*
===============
GScr_ShellShock
===============
*/
void GScr_ShellShock( scr_entref_t entref )
{
	gentity_t *ent;
	int id;
	int duration;
	const char *shock;
	char s[MAX_STRING_CHARS];

	ent = GetPlayerEntity(entref);

	if ( Scr_GetNumParam() != 2 )
	{
		Scr_Error("USAGE: <player> shellshock(<shellshockname>, <duration>)\n");
	}

	shock = Scr_GetString(0);

	for ( id = 1; id < MAX_SHELLSHOCKS; id++ )
	{
		SV_GetConfigstring(id + CS_SHELLSHOCKS, s, sizeof(s));

		if ( !strcasecmp(s, shock) )
		{
			duration = I_fround(Scr_GetFloat(1) * 1000.0f);

			if ( duration < 0 || duration > 60000 )
			{
				Scr_ParamError(1, va("duration %g should be >= 0 and <= 60", duration * 0.001f));
			}

			ent->client->ps.shellshockIndex = id;
			ent->client->ps.shellshockTime = level.time;
			ent->client->ps.shellshockDuration = duration;

			if ( ent->health > 0 )
			{
				bgs = &level_bgs;
				BG_AnimScriptEvent(&ent->client->ps, ANIM_ET_SHELLSHOCK, qfalse, qtrue);
			}

			return;
		}
	}

	Scr_Error(va("shellshock '%s' was not precached\n", shock));
}

/*
===============
GScr_StopShellShock
===============
*/
void GScr_StopShellShock( scr_entref_t entref )
{
	gentity_t *ent = GetPlayerEntity(entref);

	if ( Scr_GetNumParam() )
	{
		Scr_Error("USAGE: <player> stopshellshock()\n");
	}

	ent->client->ps.shellshockIndex = 0;
	ent->client->ps.shellshockTime = 0;
	ent->client->ps.shellshockDuration = 0;
}

/*
===============
GetPlayerEntity
===============
*/
void GScr_ViewKick( scr_entref_t entref )
{
	gentity_t *ent;
	vec3_t origin;

	ent = GetPlayerEntity(entref);

	if ( Scr_GetNumParam() != 2 )
	{
		Scr_Error("USAGE: <player> viewkick <force 0-127> <source position>\n");
	}

	ent->client->damage_blood = (Scr_GetInt(0) * ent->maxHealth + 50) / 100;

	if ( ent->client->damage_blood < 0 )
	{
		Scr_Error(va("viewkick: damage %g < 0\n", Scr_GetFloat(0)));
	}

	Scr_GetVector(1, origin);
	VectorSubtract(ent->client->ps.origin, origin, ent->client->damage_from);
}

/*
===============
GScr_LocalToWorldCoords
===============
*/
void GScr_LocalToWorldCoords( scr_entref_t entref )
{
	gentity_t *ent;
	vec3_t vLocal;
	vec3_t vWorld;
	float axis[3][3];

	ent = GetEntity(entref);
	Scr_GetVector(0, vLocal);

	AnglesToAxis(ent->r.currentAngles, axis);
	MatrixTransformVector(vLocal, axis, vWorld);
	VectorAdd(vWorld, ent->r.currentOrigin, vWorld);

	Scr_AddVector(vWorld);
}

/*
===============
GScr_SetRightArc
===============
*/
void GScr_SetRightArc( scr_entref_t entref )
{
	gentity_t *ent = GetEntity(entref);
	turretInfo_s *pTurretInfo = ent->pTurretInfo;

	if ( !pTurretInfo )
	{
		Scr_Error("entity is not a turret");
	}

	pTurretInfo->arcmin[1] = -Scr_GetFloat(0);

	if ( pTurretInfo->arcmin[1] > 0 )
	{
		pTurretInfo->arcmin[1] = 0;
	}
}

/*
===============
GScr_SetLeftArc
===============
*/
void GScr_SetLeftArc( scr_entref_t entref )
{
	gentity_t *ent = GetEntity(entref);
	turretInfo_s *pTurretInfo = ent->pTurretInfo;

	if ( !pTurretInfo )
	{
		Scr_Error("entity is not a turret");
	}

	pTurretInfo->arcmax[1] = Scr_GetFloat(0);

	if ( pTurretInfo->arcmax[1] < 0 )
	{
		pTurretInfo->arcmax[1] = 0;
	}
}

/*
===============
GScr_SetTopArc
===============
*/
void GScr_SetTopArc( scr_entref_t entref )
{
	gentity_t *ent = GetEntity(entref);
	turretInfo_s *pTurretInfo = ent->pTurretInfo;

	if ( !pTurretInfo )
	{
		Scr_Error("entity is not a turret");
	}

	pTurretInfo->arcmin[0] = -Scr_GetFloat(0);

	if ( pTurretInfo->arcmin[0] > 0 )
	{
		pTurretInfo->arcmin[0] = 0;
	}
}

/*
===============
GScr_SetBottomArc
===============
*/
void GScr_SetBottomArc( scr_entref_t entref )
{
	gentity_t *ent = GetEntity(entref);
	turretInfo_s *pTurretInfo = ent->pTurretInfo;

	if ( !pTurretInfo )
	{
		Scr_Error("entity is not a turret");
	}

	pTurretInfo->arcmax[0] = Scr_GetFloat(0);

	if ( pTurretInfo->arcmax[0] < 0 )
	{
		pTurretInfo->arcmax[0] = 0;
	}
}

/*
===============
GScr_PlaceSpawnPoint
===============
*/
void GScr_PlaceSpawnPoint( scr_entref_t entref )
{
	gentity_t *ent;
	trace_t trace;
	vec3_t vStart;
	vec3_t vEnd;

	ent = GetEntity(entref);

	VectorCopy(ent->r.currentOrigin, vStart);
	VectorCopy(ent->r.currentOrigin, vEnd);
	vEnd[2] += 128;

	G_TraceCapsule(&trace, vStart, playerMins, playerMaxs, vEnd, ent->s.number, MASK_PLAYERSOLID);

	Vec3Lerp(vStart, vEnd, trace.fraction, vStart);
	VectorCopy(vStart, vEnd);
	vEnd[2] -= 262144;

	G_TraceCapsule(&trace, vStart, playerMins, playerMaxs, vEnd, ent->s.number, MASK_PLAYERSOLID);

	ent->s.groundEntityNum = trace.entityNum;
	Vec3Lerp(vStart, vEnd, trace.fraction, vStart);

	G_TraceCapsule(&trace, vStart, playerMins, playerMaxs, vStart, ent->s.number, MASK_PLAYERSOLID);

	if ( trace.allsolid )
	{
		Com_Printf("WARNING: Spawn point entity %i is in solid at (%i, %i, %i)\n",
		           ent->s.number, (int)ent->r.currentOrigin[0], (int)ent->r.currentOrigin[1], (int)ent->r.currentOrigin[2]);
	}

	G_SetOrigin(ent, vStart);
}

/*
===============
GScr_UpdateScores
===============
*/
void GScr_UpdateScores( scr_entref_t entref )
{
	gentity_t *ent = GetEntity(entref);

	SV_GameSendServerCommand(ent - g_entities, SV_CMD_CAN_IGNORE, va("%c %i", 72, level.teamScores[TEAM_ALLIES]));
	SV_GameSendServerCommand(ent - g_entities, SV_CMD_CAN_IGNORE, va("%c %i", 71, level.teamScores[TEAM_AXIS]));
}

/*
===============
GScr_MapRestart
===============
*/
void GScr_MapRestart()
{
	if ( level.finished )
	{
		Scr_Error(level.finished == LF_MAP_RESTART ? "map_restart already called" : "exitlevel already called");
	}

	level.finished = LF_MAP_RESTART;
	level.savePersist = qfalse;

	if ( Scr_GetNumParam() )
	{
		level.savePersist = Scr_GetInt(0);
	}

	Cbuf_ExecuteText(EXEC_APPEND, "fast_restart\n");
}

/*
===============
GScr_LoadMap
===============
*/
void GScr_LoadMap()
{
	if ( Scr_GetNumParam() < 1 )
	{
		return;
	}

	const char *mapname = Scr_GetString(0);

	if ( !SV_MapExists(mapname) )
	{
		return;
	}

	if ( level.finished )
	{
		Scr_Error(level.finished == LF_MAP_CHANGE ? "map already called" : "exitlevel already called");
	}

	level.finished = LF_MAP_CHANGE;
	level.savePersist = qfalse;

	if ( Scr_GetNumParam() > 1 )
	{
		level.savePersist = Scr_GetInt(1);
	}

	Cbuf_ExecuteText(EXEC_APPEND, va("map %s\n", mapname));
}

/*
===============
GScr_ExitLevel
===============
*/
void GScr_ExitLevel()
{
	if ( level.finished )
	{
		Scr_Error(level.finished == LF_MAP_RESTART ? "map_restart already called" : "exitlevel already called");
	}

	level.finished = LF_EXITLEVEL;
	level.savePersist = qfalse;

	if ( Scr_GetNumParam() )
	{
		level.savePersist = Scr_GetInt(0);
	}

	SV_MatchEnd();
	ExitLevel();
}

/*
===============
GScr_AddTestClient
===============
*/
void GScr_AddTestClient()
{
	gentity_t *ent;

	ent = SV_AddTestClient();

	if ( ent )
	{
		Scr_AddEntity(ent);
	}
}

/*
===============
GScr_MakeDvarServerInfo
===============
*/
void GScr_MakeDvarServerInfo()
{
	unsigned int i;
	int len;
	int type;
	char *p;
	char string[MAX_STRING_CHARS];
	char outString[MAX_STRING_CHARS];
	const char *dvarName;
	const char *dvarValue;
	dvar_t *dvar;

	dvarName = Scr_GetString(0);
	dvar = Dvar_FindVar(dvarName);

	if ( dvar )
	{
		Dvar_AddFlags(dvar, DVAR_CODINFO);
		return;
	}

	type = Scr_GetType(1);

	if ( type == VAR_ISTRING )
	{
		Scr_ConstructMessageString(1, Scr_GetNumParam() - 1, "Dvar Value", string, sizeof(string));
		dvarValue = string;
	}
	else
	{
		dvarValue = Scr_GetString(1);
	}

	// outString is sanitized but the raw dvarValue is registered
	len = strlen(dvarValue);
	p = outString;

	for ( i = 0; i < 0x3ff && dvarValue[i]; i++, p++ )
	{
		*p = I_CleanChar(dvarValue[i]);

		if ( *p == '"' )
		{
			*p = '\'';
		}
	}

	*p = 0;

	Dvar_RegisterString(dvarName, dvarValue, DVAR_CODINFO | DVAR_EXTERNAL);
}

/*
===============
GScr_AllClientsPrint
===============
*/
void GScr_AllClientsPrint()
{
	const char *string;

	if ( Scr_GetNumParam() <= 0 )
	{
		return;
	}

	string = Scr_GetString(0);
	SV_GameSendServerCommand(-1, SV_CMD_CAN_IGNORE, va("%c \"%s\"", 101, string));
}

/*
===============
GScr_MapExists
===============
*/
void GScr_MapExists()
{
	const char *s;

	if ( Scr_GetNumParam() <= 0 )
	{
		return;
	}

	s = Scr_GetString(0);

	if ( SV_MapExists(s) )
	{
		Scr_AddInt(1);
	}
	else
	{
		Scr_AddInt(0);
	}
}

/*
===============
GScr_IsValidGameType
===============
*/
void GScr_IsValidGameType()
{
	const char *v1;

	if ( Scr_GetNumParam() <= 0 )
	{
		return;
	}

	v1 = Scr_GetString(0);

	if ( Scr_IsValidGameType(v1) )
	{
		Scr_AddInt(1);
	}
	else
	{
		Scr_AddInt(0);
	}
}

/*
===============
GScr_SetVoteString
===============
*/
void GScr_SetVoteString()
{
	if ( Scr_GetNumParam() <= 0 )
	{
		return;
	}

	const char *str = Scr_GetString(0);

	SV_SetConfigstring(CS_VOTE_STRING, str);
	SV_SetConfigstring(CS_VOTE_TIME,   va("%i", level.voteTime));
	SV_SetConfigstring(CS_VOTE_YES,    va("%i", level.voteYes));
	SV_SetConfigstring(CS_VOTE_NO,     va("%i", level.voteNo));
}

/*
===============
GScr_SetVoteTime
===============
*/
void GScr_SetVoteTime()
{
	if ( Scr_GetNumParam() <= 0 )
	{
		return;
	}

	int count = Scr_GetInt(0);

	SV_SetConfigstring(CS_VOTE_TIME, va("%i", count));
	SV_SetConfigstring(CS_VOTE_YES,  va("%i", level.voteYes));
	SV_SetConfigstring(CS_VOTE_NO,   va("%i", level.voteNo));
}

/*
===============
GScr_SetVoteYesCount
===============
*/
void GScr_SetVoteYesCount()
{
	if ( Scr_GetNumParam() <= 0 )
	{
		return;
	}

	int count = Scr_GetInt(0);

	SV_SetConfigstring(CS_VOTE_YES, va("%i", count));
	SV_SetConfigstring(CS_VOTE_NO,  va("%i", level.voteNo));
}

/*
===============
GScr_SetVoteNoCount
===============
*/
void GScr_SetVoteNoCount()
{
	if ( Scr_GetNumParam() <= 0 )
	{
		return;
	}

	int count = Scr_GetInt(0);

	SV_SetConfigstring(CS_VOTE_NO, va("%i", count));
}

/*
===============
GScr_ClientPrint
===============
*/
void GScr_ClientPrint()
{
	gentity_t *v3;
	const char *v2;

	if ( Scr_GetNumParam() <= 0 )
	{
		return;
	}

	v3 = Scr_GetEntity(0);
	v2 = Scr_GetString(1);
	SV_GameSendServerCommand(v3 - g_entities, SV_CMD_CAN_IGNORE, va("%c \"%s\"", 101, v2));
}

/*
===============
GScr_OpenFile
===============
*/
void GScr_OpenFile()
{
	const char *filename;
	const char *mode;
	fileHandle_t *f;
	fileHandle_t tempFile;
	int openResult;
	int filenum;

	f = NULL;

	if ( Scr_GetNumParam() <= 1 )
	{
		return;
	}

	filename = Scr_GetString(0);
	mode = Scr_GetString(1);

	for ( filenum = 0; filenum < MAX_SCRIPT_IO_FILE_HANDLES; filenum++ )
	{
		if ( level.openScriptIOFileHandles[filenum] < 0 )
		{
			f = &level.openScriptIOFileHandles[filenum];
			break;
		}
	}

	if ( !f )
	{
		Com_Printf("OpenFile failed.  %i files already open\n", MAX_SCRIPT_IO_FILE_HANDLES);
		Scr_AddInt(-1);
		return;
	}

	if ( !strcmp(mode, "read") )
	{
		int filesize = FS_FOpenFileByMode(va("%s/%s", "scriptdata", filename), &tempFile, FS_READ);

		if ( filesize < 0 )
		{
			Scr_AddInt(-1);
			return;
		}

		{
			level.openScriptIOFileBuffers[filenum] = (char *)Z_Malloc(filesize + 1);

			FS_Read(level.openScriptIOFileBuffers[filenum], filesize, tempFile);
			FS_FCloseFile(tempFile);

			level.openScriptIOFileBuffers[filenum][filesize] = 0;

			Com_BeginParseSession(filename);
			Com_SetCSV(qtrue);

			level.currentScriptIOLineMark[filenum].lines = 0;
			Scr_AddInt(filenum);
			return;
		}
	}

	if ( !strcmp(mode, "write") )
	{
		openResult = FS_FOpenFileByMode(va("%s/%s", "scriptdata", filename), f, FS_WRITE);

		if ( openResult < 0 )
		{
			Scr_AddInt(-1);
		}
		else
		{
			Scr_AddInt(filenum);
		}

		return;
	}

	if ( !strcmp(mode, "append") )
	{
		openResult = FS_FOpenFileByMode(va("%s/%s", "scriptdata", filename), f, FS_APPEND);

		if ( openResult < 0 )
		{
			Scr_AddInt(-1);
		}
		else
		{
			Scr_AddInt(filenum);
		}

		return;
	}

	Com_Printf("Valid openfile modes are 'write', 'read', and 'append'\n");
	Scr_AddInt(-1);
}

/*
===============
GScr_CloseFile
===============
*/
void GScr_CloseFile()
{
	if ( Scr_GetNumParam() <= 0 )
	{
		return;
	}

	int filenum = Scr_GetInt(0);

	if ( filenum < 0 || filenum > MAX_SCRIPT_IO_FILE_HANDLES )
	{
		Com_Printf("CloseFile failed, invalid file number %i\n", filenum);
		Scr_AddInt(-1);
		return;
	}

	assert(((level.openScriptIOFileHandles[filenum] != 0) && (level.openScriptIOFileBuffers[filenum] != NULL)));

	if ( level.openScriptIOFileHandles[filenum] > 0 )
	{
		FS_FCloseFile(level.openScriptIOFileHandles[filenum]);
		level.openScriptIOFileHandles[filenum] = -1;
	}
	else if ( level.openScriptIOFileBuffers[filenum] )
	{
		Com_EndParseSession();

		Z_Free(level.openScriptIOFileBuffers[filenum]);
		level.openScriptIOFileBuffers[filenum] = NULL;
	}
	else
	{
		Com_Printf("CloseFile failed, file number %i was not open\n", filenum);
		Scr_AddInt(-1);
		return;
	}

	Scr_AddInt(1);
}

/*
===============
GScr_FPrintln
===============
*/
void GScr_FPrintln()
{
	if ( Scr_GetNumParam() <= 1 )
	{
		Com_Printf("fprintln requires at least 2 parameters (file, output)\n");
		Scr_AddInt(-1);
		return;
	}

	int filenum = Scr_GetInt(0);

	if ( filenum < 0 || filenum > MAX_SCRIPT_IO_FILE_HANDLES )
	{
		Com_Printf("FPrintln failed, invalid file number %i\n", filenum);
		Scr_AddInt(-1);
		return;
	}

	if ( level.openScriptIOFileHandles[filenum] < 0 )
	{
		Com_Printf("FPrintln failed, file number %i was not open for writing\n", filenum);
		Scr_AddInt(-1);
		return;
	}

	for ( int arg = 1; arg < Scr_GetNumParam(); arg++ )
	{
		const char *s = Scr_GetString(arg);

		FS_Write(s, strlen(s), level.openScriptIOFileHandles[filenum]);
		FS_Write(",", 1, level.openScriptIOFileHandles[filenum]);
	}

	Scr_AddInt(Scr_GetNumParam() - 1);
}

/*
===============
GScr_FReadLn
===============
*/
void GScr_FReadLn()
{
	if ( Scr_GetNumParam() <= 0 )
	{
		Com_Printf("freadln requires a parameter - the file to read from\n");
		Scr_AddInt(-1);
		return;
	}

	int filenum;
	int eof;
	const char *buf;
	const char *token;

	filenum = Scr_GetInt(0);

	if ( filenum < 0 || filenum > MAX_SCRIPT_IO_FILE_HANDLES )
	{
		Com_Printf("freadln failed, invalid file number %i\n", filenum);
		Scr_AddInt(-1);
		return;
	}

	if ( !level.openScriptIOFileBuffers[filenum] )
	{
		Com_Printf("freadln failed, file number %i was not open for reading\n", filenum);
		Scr_AddInt(-1);
		return;
	}

	buf = level.openScriptIOFileBuffers[filenum];

	if ( !level.currentScriptIOLineMark[filenum].lines )
	{
		Com_ParseSetMark(&buf, &level.currentScriptIOLineMark[filenum]);
		Scr_AddInt(Com_GetArgCountOnLine(&buf));
		return;
	}

	Com_ParseReturnToMark(&buf, &level.currentScriptIOLineMark[filenum]);
	Com_SkipRestOfLine(&buf);
	Com_ParseSetMark(&buf, &level.currentScriptIOLineMark[filenum]);

	token = Com_Parse(&buf);
	if ( !*token )
	{
		eof = 1;
	}
	else
	{
		eof = 0;
	}

	Com_ParseReturnToMark(&buf, &level.currentScriptIOLineMark[filenum]);

	if ( eof )
	{
		Scr_AddInt(-1);
		return;
	}

	Scr_AddInt(Com_GetArgCountOnLine(&buf));
}

/*
===============
GScr_FGetArg
===============
*/
void GScr_FGetArg()
{
	if ( Scr_GetNumParam() <= 1 )
	{
		Com_Printf("freadline requires at least 2 parameters (file, string)\n");
		Scr_AddString("");
		return;
	}

	int filenum;
	int arg;
	int i;
	const char *token;
	const char *buf;

	filenum = Scr_GetInt(0);
	arg = Scr_GetInt(1);

	if ( filenum < 0 || filenum > MAX_SCRIPT_IO_FILE_HANDLES )
	{
		Com_Printf("freadline failed, invalid file number %i\n", filenum);
		Scr_AddString("");
		return;
	}

	if ( arg < 0 )
	{
		Com_Printf("freadline failed, invalid argument number %i\n", arg);
		Scr_AddString("");
		return;
	}

	if ( !level.openScriptIOFileBuffers[filenum] )
	{
		Com_Printf("freadline failed, file number %i was not open for reading\n", filenum);
		Scr_AddString("");
		return;
	}

	token = NULL;
	buf = level.openScriptIOFileBuffers[filenum];

	Com_ParseReturnToMark(&buf, &level.currentScriptIOLineMark[filenum]);

	for ( i = 0; i <= arg; i++ )
	{
		token = Com_ParseOnLine(&buf);

		if ( !token[0] )
		{
			Com_Printf("freadline failed, there aren't %i arguments on this line, there are only %i arguments\n", arg + 1, i);
			Scr_AddString("");
			return;
		}
	}

	Scr_AddString(token);
}

/*
===============
GScr_SetArchive
===============
*/
void GScr_SetArchive()
{
	SV_EnableArchivedSnapshot(Scr_GetInt(0));
}

/*
===============
Scr_IsSplitscreen
===============
*/
void Scr_IsSplitscreen()
{
	Scr_AddInt(0);
}

/*
===============
ScrCmd_Unlink
===============
*/
void GScr_SetTeamForTrigger( scr_entref_t entref )
{
	gentity_t *ent = GetEntity(entref);

	if ( ent->classname != scr_const.trigger_use && ent->classname != scr_const.trigger_use_touch )
	{
		Scr_Error(va("setteamfortrigger: trigger entity must be of type %s or %s", SL_ConvertToString(scr_const.trigger_use), SL_ConvertToString(scr_const.trigger_use_touch)));
	}

	unsigned short team = Scr_GetConstString(0);

	if ( team == scr_const.allies )
	{
		ent->team = TEAM_ALLIES;
	}
	else if ( team == scr_const.axis )
	{
		ent->team = TEAM_AXIS;
	}
	else if ( team == scr_const.none )
	{
		ent->team = TEAM_NONE;
	}
	else
	{
		Scr_Error(va("setteamfortrigger: invalid team used must be %s, %s or %s", SL_ConvertToString(scr_const.allies), SL_ConvertToString(scr_const.axis), SL_ConvertToString(scr_const.none)));
	}
}

/*
===============
GScr_ClientClaimTrigger
===============
*/
void GScr_ClientClaimTrigger( scr_entref_t entref )
{
	gentity_t *clientEnt = GetEntity(entref);

	if ( !clientEnt->client )
	{
		Scr_Error("clientclaimtrigger: claimer must be a client.");
	}

	gentity_t *triggerEnt = Scr_GetEntity(0);

	if ( triggerEnt->classname != scr_const.trigger_use && triggerEnt->classname != scr_const.trigger_use_touch )
	{
		Scr_Error(va("clientclaimtrigger: trigger entity must be of type %s or %s", SL_ConvertToString(scr_const.trigger_use), SL_ConvertToString(scr_const.trigger_use_touch)));
	}

	if ( triggerEnt->trigger.singleUserEntIndex == ENTITYNUM_NONE || triggerEnt->trigger.singleUserEntIndex == clientEnt->client->ps.clientNum )
	{
		triggerEnt->trigger.singleUserEntIndex = clientEnt->client->ps.clientNum;
	}
}

/*
===============
GScr_ClientReleaseTrigger
===============
*/
void GScr_ClientReleaseTrigger( scr_entref_t entref )
{
	gentity_t *clientEnt = GetEntity(entref);

	if ( !clientEnt->client )
	{
		Scr_Error("clientreleasetrigger: releaser must be a client.");
	}

	gentity_t *triggerEnt = Scr_GetEntity(0);

	if ( triggerEnt->classname != scr_const.trigger_use && triggerEnt->classname != scr_const.trigger_use_touch )
	{
		Scr_Error(va("clientreleasetrigger: trigger entity must be of type %s or %s", SL_ConvertToString(scr_const.trigger_use), SL_ConvertToString(scr_const.trigger_use_touch)));
	}

	if ( triggerEnt->trigger.singleUserEntIndex != clientEnt->client->ps.clientNum )
	{
		return;
	}

	triggerEnt->trigger.singleUserEntIndex = ENTITYNUM_NONE;
}

/*
===============
GScr_ReleaseClaimedTrigger
===============
*/
void GScr_ReleaseClaimedTrigger( scr_entref_t entref )
{
	gentity_t *triggerEnt = GetEntity(entref);

	if ( triggerEnt->classname != scr_const.trigger_use && triggerEnt->classname != scr_const.trigger_use_touch )
	{
		Scr_Error(va("releaseclaimedtrigger: trigger entity must be of type %s or %s", SL_ConvertToString(scr_const.trigger_use), SL_ConvertToString(scr_const.trigger_use_touch)));
	}

	triggerEnt->trigger.singleUserEntIndex = ENTITYNUM_NONE;
}

/*
===============
Scr_GetFunction
===============
*/
void (*Scr_GetFunction( const char **pName, int *type ))( )
{
	unsigned int i;
	const char *name;

	name = *pName;

	for ( i = 0; i < 144; i++ )
	{
		if ( !strcmp(name, functions[i].name) )
		{
			*pName = functions[i].name;
			*type = functions[i].developer;

			return functions[i].call;
		}
	}

	return NULL;
}

/*
===============
BuiltIn_GetMethod
===============
*/
void (*BuiltIn_GetMethod( const char **pName, int *type ))( scr_entref_t )
{
	int i;
	const char *name;

	name = *pName;

	for ( i = 0; i < ARRAY_COUNT(methods); i++ )
	{
		if ( !strcmp(name, methods[i].name) )
		{
			*pName = methods[i].name;
			*type = methods[i].developer;

			return methods[i].call;
		}
	}

	return NULL;
}

/*
===============
Scr_GetMethod
===============
*/
void (*Scr_GetMethod( const char **pName, int *type ))( scr_entref_t )
{
	void (*meth)(scr_entref_t);

	*type = BUILTIN_ANY;

	meth = Player_GetMethod(pName);
	if ( meth )
		return meth;

	meth = ScriptEnt_GetMethod(pName);
	if ( meth )
		return meth;

	meth = HudElem_GetMethod(pName);
	if ( meth )
		return meth;

	return BuiltIn_GetMethod(pName, type);
}

/*
===============
Scr_SetOrigin
===============
*/
void Scr_SetOrigin( gentity_t *ent, int offset )
{
	vec3_t org;

	Scr_GetVector(0, org);
	G_SetOrigin(ent, org);

	if ( ent->r.linked )
	{
		SV_LinkEntity(ent);
	}
}

/*
===============
Scr_SetAngles
===============
*/
void Scr_SetAngles( gentity_t *ent, int offset )
{
	vec3_t angles;

	Scr_GetVector(0, angles);
	G_SetAngle(ent, angles);
}

/*
===============
Scr_SetHealth
===============
*/
void Scr_SetHealth( gentity_t *ent, int offset )
{
	int health = Scr_GetInt(0);

	if ( ent->client )
	{
		ent->health = health;
		ent->client->ps.stats[STAT_HEALTH] = health;
	}
	else
	{
		ent->maxHealth = health;
		ent->health = health;
	}
}

/*
===============
GScr_AddVector
===============
*/
void GScr_AddVector( const vec3_t vVec )
{
	if ( vVec )
	{
		Scr_AddVector(vVec);
	}
	else
	{
		Scr_AddUndefined();
	}
}

/*
===============
GScr_AddEntity
===============
*/
void GScr_AddEntity( gentity_t *pEnt )
{
	if ( pEnt )
	{
		Scr_AddEntity(pEnt);
	}
	else
	{
		Scr_AddUndefined();
	}
}

void Scr_ParseGameTypeList()
{
	char *src;
	int i;
	int len;
	int n;
	int c;
	const char *text;
	char *token;
	gameTypeScript_t *entry;
	const char *pszFile;
	char buffer[1024];
	char listbuf[4096];
	fileHandle_t f;
	int l;

	n = 0;

	memset(g_scr_data.gametype.list, 0, sizeof(g_scr_data.gametype.list));
	c = FS_GetFileList("maps/mp/gametypes", "gsc", FS_LIST_PURE_ONLY, listbuf, sizeof(listbuf));

	src = listbuf;

	for ( i = 0; i < c; i++ )
	{
		entry = &g_scr_data.gametype.list[n];

		l = strlen(src);

		if ( *src == '_' )
		{
			src += l + 1;
			continue;
		}

		if ( !I_stricmp(&src[l - 4], ".gsc") )
		{
			src[l - 4] = 0;
		}

		if ( n == MAX_GAMETYPE_SCRIPTS )
		{
			Com_Printf("Too many game type scripts found! Only loading the first %i\n", MAX_GAMETYPE_SCRIPTS - 1);
			break;
		}

		I_strncpyz(entry->pszScript, src, sizeof(entry->pszScript));
		strlwr(entry->pszScript);

		pszFile = va("maps/mp/gametypes/%s.txt", src);
		len = FS_FOpenFileByMode(pszFile, &f, FS_READ);

		if ( len <= 0 || len >= (int)sizeof(buffer) )
		{
			if ( len <= 0 )
			{
				Com_Printf("WARNING: Could not load GameType description file %s for gametype %s\n", va("maps/mp/gametypes/%s.txt", src), src);
			}
			else
			{
				Com_Printf("WARNING: GameType description file %s is too big to load.\n", va("maps/mp/gametypes/%s.txt", src));
			}

			I_strncpyz(entry->pszName, entry->pszScript, sizeof(entry->pszScript));
			entry->bTeamBased = qfalse;
		}
		else
		{
			FS_Read(buffer, len, f);
			text = buffer;
			token = Com_Parse(&text);
			I_strncpyz(entry->pszName, token, sizeof(entry->pszName));
			token = Com_Parse(&text);
			if ( token && !I_stricmp(token, "team") )
			{
				entry->bTeamBased = qtrue;
			}
			else
			{
				entry->bTeamBased = qfalse;
			}
		}

		n++;

		if ( len > 0 )
		{
			FS_FCloseFile(f);
		}

		src += l + 1;
	}

	g_scr_data.gametype.iNumGameTypes = n;
}

/*
===============
Scr_GetGameTypeNameForScript
===============
*/
char* Scr_GetGameTypeNameForScript( const char *pszGameTypeScript )
{
	for ( int i = 0; i < g_scr_data.gametype.iNumGameTypes; i++ )
	{
		if ( !I_stricmp(g_scr_data.gametype.list[i].pszScript, pszGameTypeScript) )
		{
			return g_scr_data.gametype.list[i].pszName;
		}
	}

	return NULL;
}

/*
===============
Scr_LoadGameType
===============
*/
void Scr_LoadGameType()
{
	unsigned short handle;

	handle = Scr_ExecThread( g_scr_data.gametype.main, 0 );
	Scr_FreeThread( handle );
}

/*
===============
Scr_PlayerConnect
===============
*/
void Scr_StartupGameType()
{
	unsigned short handle;

	handle = Scr_ExecThread( g_scr_data.gametype.startupgametype, 0 );
	Scr_FreeThread( handle );
}

/*
===============
Scr_PlayerDamage
===============
*/
void Scr_PlayerDamage( gentity_t *self, gentity_t *inflictor, gentity_t *attacker,
                       int damage, int dflags, int meansOfDeath, int iWeapon,
                       const vec3_t vPoint, const vec3_t vDir, int hitLoc, int timeOffset )
{
	unsigned short threadId;
	Scr_AddInt(timeOffset);
	Scr_AddConstString(G_GetHitLocationString(hitLoc));
	GScr_AddVector(vDir);
	GScr_AddVector(vPoint);
	Scr_AddString(BG_GetWeaponDef(iWeapon)->szInternalName);
	if ( meansOfDeath < 0 || (unsigned int)meansOfDeath >= MOD_NUM )
		Scr_AddString("badMOD");
	else
		Scr_AddString(modNames[meansOfDeath]);
	Scr_AddInt(dflags);
	Scr_AddInt(damage);
	GScr_AddEntity(attacker);
	GScr_AddEntity(inflictor);
	threadId = Scr_ExecEntThread( self, g_scr_data.gametype.playerdamage, 10 );
	Scr_FreeThread( threadId );
}
/*
===============
Scr_PlayerKilled
===============
*/
void Scr_PlayerKilled( gentity_t *self, gentity_t *inflictor, gentity_t *attacker,
                       int damage, int meansOfDeath, int iWeapon,
                       const vec3_t vDir, int hitLoc, int psTimeOffset, int deathAnimDuration )
{
	unsigned short threadId;
	Scr_AddInt(deathAnimDuration);
	Scr_AddInt(psTimeOffset);
	Scr_AddConstString(G_GetHitLocationString(hitLoc));
	GScr_AddVector(vDir);
	Scr_AddString(BG_GetWeaponDef(iWeapon)->szInternalName);
	if ( meansOfDeath < 0 || (unsigned int)meansOfDeath >= MOD_NUM )
		Scr_AddString("badMOD");
	else
		Scr_AddString(modNames[meansOfDeath]);
	Scr_AddInt(damage);
	GScr_AddEntity(attacker);
	GScr_AddEntity(inflictor);
	threadId = Scr_ExecEntThread( self, g_scr_data.gametype.playerkilled, 9 );
	Scr_FreeThread( threadId );
}

/*
===============
Scr_VoteCalled
===============
*/
void Scr_VoteCalled( gentity_t *self, const char *command, const char *param1, const char *param2 )
{
	Scr_AddString(param2);
	Scr_AddString(param1);
	Scr_AddString(command);

	Scr_Notify(self, scr_const.call_vote, 3);
}

/*
===============
Scr_PlayerVote
===============
*/
void Scr_PlayerVote( gentity_t *self, const char *option )
{
	Scr_AddString(option);
	Scr_Notify(self, scr_const.vote, 1);
}

unsigned int GScr_AllocString( const char *s )
{
	return Scr_AllocString(s, 1);
}
void ScrCmd_PlayRumble( scr_entref_t entref )
{
	;
}
void ScrCmd_PlayLoopRumble( scr_entref_t entref )
{
	;
}
void ScrCmd_StopRumble( scr_entref_t entref )
{
	;
}
void GScr_asin()
{
	float v4;
	int v3;

	v4 = Scr_GetFloat(0);

	if ( v4 < -1.0f || v4 > 1.0f )
	{
		Scr_Error(va("%g out of range", v4));
	}

	*(float *)&v3 = asin(v4);
	*(float *)&v3 = *(float *)&v3 * DEGINRAD;
	Scr_AddFloat(*(float *)&v3);
}
void GScr_acos()
{
	float v4;

	v4 = Scr_GetFloat(0);

	if ( v4 < -1.0f || v4 > 1.0f )
	{
		Scr_Error(va("%g out of range", v4));
	}

	v4 = acos(v4);
	v4 = v4 * DEGINRAD;
	Scr_AddFloat(v4);
}
void Scr_ToLower()
{
	char tempString[MAX_STRING_CHARS];
	const char *s;
	int i;
	char c;

	s = Scr_GetString(0);

	for ( i = 0; i < MAX_STRING_CHARS; i++, s++ )
	{
		c = tolower(*s);
		tempString[i] = c;

		if ( !c )
		{
			Scr_AddString(tempString);
			return;
		}
	}

	Scr_Error("string too long");
}
void Scr_PrecacheRumble()
{
	;
}
void GScr_KickPlayer()
{
	int v1;

	if ( Scr_GetNumParam() <= 0 )
	{
		return;
	}

	v1 = Scr_GetInt(0);
	Cbuf_ExecuteText(EXEC_APPEND, va("tempBanClient %i\n", v1));
}
void GScr_BanPlayer()
{
	int v1;

	if ( Scr_GetNumParam() <= 0 )
	{
		return;
	}

	v1 = Scr_GetInt(0);
	Cbuf_ExecuteText(EXEC_APPEND, va("banClient %i\n", v1));
}
void Scr_PlayRumbleOnPos()
{
	;
}
void Scr_PlayLoopRumbleOnPos()
{
	;
}
void Scr_StopAllRumbles()
{
	;
}
void GScr_MatchEnd()
{
	;
}
void GScr_SetPlayerTeamRank()
{
	;
}
void GScr_SendXboxLiveRanks()
{
	;
}
qboolean Scr_IsValidGameType( const char *pszGameType )
{
	int valid;

	if ( Scr_GetGameTypeNameForScript(pszGameType) )
	{
		valid = 1;
	}
	else
	{
		valid = 0;
	}

	return valid;
}
void Scr_PlayerConnect( gentity_t *self )
{
	unsigned short threadId;

	threadId = Scr_ExecEntThread( self, g_scr_data.gametype.playerconnect, 0 );
	Scr_FreeThread( threadId );
}
void Scr_PlayerDisconnect( gentity_t *self )
{
	unsigned short threadId;

	threadId = Scr_ExecEntThread( self, g_scr_data.gametype.playerdisconnect, 0 );
	Scr_FreeThread( threadId );
}
/*
===============
GScr_Stub
===============
*/
// Unreferenced; the original name did not survive.
int GScr_Stub()
{
	return 0;
}

