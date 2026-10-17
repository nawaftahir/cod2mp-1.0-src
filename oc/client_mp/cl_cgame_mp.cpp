#include "common_types.h"
#include "imports.h"

#    include <mmintrin.h>
#include <string.h>
extern const dvar_t *cl_freezeDemo;
extern dvar_t *cl_paused;
extern dvar_t *com_expectedHunkUsage;
extern float com_timescaleValue;
extern dvar_t *loc_warnings;
extern dvar_t *loc_warningsAsErrors;
extern dvar_t *sv_paused;
extern clientConnection_t clientConnections[1];
#define clc clientConnections[0]
extern clientStatic_t cls;
extern const dvar_t *cl_activeAction;
/* dvar globals */
extern const dvar_t *cl_showServerCommands;
extern const dvar_t *cl_showTimeDelta;
extern const dvar_t *com_statmon;
extern dvar_t *com_sv_running;

extern char *getenv(const char *name);

extern refexport_t re;

extern void Com_Error(errorParm_t code, const char *fmt, ...);
extern void Com_Printf(const char *fmt, ...);
extern void Com_DPrintf(const char *fmt, ...);
extern int Com_sprintf(char *dest, size_t size, const char *format, ...);
extern char *Com_Parse( const char *( *data_p ) );
extern void Com_LoadBsp(const char *name);
extern void Com_UnloadBsp(void);
extern void Com_TouchMemory(void);
extern void Com_InitDObj(void);
extern void Com_SetWeaponInfoMemory(int mode);
extern void Com_FreeWeaponInfoMemory(int mode);
extern void Com_UnloadSoundAliases(snd_alias_system_t system);
extern void Com_LoadSoundAliases(const char *loadspec, const char *zone, snd_alias_system_t flag);
extern snd_alias_t *Com_PickSoundAlias( const char *aliasName );
extern const char *Cmd_Argv( int arg );
extern int Cmd_Argc(void);
extern void Cmd_TokenizeString(const char *text);
extern void Cmd_TokenizeStringWithLimit(const char *text_in, int max_tokens);
extern void Cmd_AddCommand(const char *cmd, void (*func)(void));
extern int I_stricmp(const char *s1, const char *s2);
extern int I_strnicmp(const char *s1, const char *s2, int n);
extern void I_strncat(char *dest, int size, const char *src);
extern void I_strncpyz(char *dest, const char *src, int maxLen);
extern void CM_LoadMap(const char *name, int *checksum);
extern void CM_LinkWorld(void);
extern void CG_Init(int serverMessageNum, int serverCommandSequence, int clientNum);
extern void CG_Shutdown(void);
extern qboolean CG_ConsoleCommand(void);

extern void CG_DrawStringExt(float x, float y, const char *string, const vec_t *setColor, qboolean forceColor, qboolean shadow, float charHeight, qboolean adjust);
extern void CG_ArchiveState(MemoryFile *memFile);
extern void CL_SystemInfoChanged(void);
extern void CL_StartHunkUsers(void);
extern void CL_ReadDemoMessage(void);
extern int CL_GetSkelTimeStamp(int localClientNum);
extern char *CL_AllocSkelMemory(int localClientNum, unsigned int size);
extern void CL_ConsolePrint(print_msg_type_t type, const char *txt, int duration, int linewidth);
extern int DObjSkelExists(struct DObj_s *obj, int timeStamp);
extern int DObjSkelIsBoneUpToDate(DObj_s *obj, int boneIndex);
extern qboolean DObjSkelAreBonesUpToDate(const struct DObj_s *obj, int *partBits);
extern int DObjGetAllocSkelSize(const struct DObj_s *obj);
extern void DObjCreateSkel(struct DObj_s *obj, struct DSkel_t *skel, int timeStamp);
extern void FX_AdjustCamera(const refdef_t *refdef, float zfar);
extern int FS_FOpenFileByMode(const char *filename, int *handle, fsMode_t mode);
extern fileHandle_t FS_FOpenFileWrite(const char *filename);
extern int FS_Read(void *buffer, int len, int handle);
extern int FS_Write(const void *buffer, int len, int handle);
extern void FS_FCloseFile(fileHandle_t handle);
extern void FS_Printf(int handle, const char *fmt, ...);
extern int Hunk_Used(void);
extern "C" void *Z_MallocInternal( int size );
extern "C" void Z_FreeInternal( void *ptr );
extern const char *Info_ValueForKey( const char *s, const char *key );
extern const char *GetBspExtension(void);
extern int Sys_Milliseconds(void);
extern void Con_Close(void);
extern void Con_ClearNotify(void);
extern void Con_ClearSubtitles(void);
extern const char *SEH_SafeTranslateString(const char *text);
extern const char *SEH_StringEd_GetString(const char *text);
extern const char *UI_ReplaceConversionString(const char *base, const char *replacement);
extern qboolean UI_SetActiveMenu(int menu);
extern uiMenuCommand_t UI_GetActiveMenu(void);
extern qboolean UI_IsFullscreen(void);
extern void UI_CloseFocusedMenu(void);
extern void UI_KeyEvent(int key, int down);
extern void SCR_UpdateScreenInternal(void);
extern void R_AddCmdDrawStretchPic(float x, float y, float w, float h, float s0, float t0, float s1, float t1, const vec_t *color, MaterialHandle material);
extern void R_AddCmdDrawQuadPic(vec2_t *verts, const vec_t *color, MaterialHandle material);
extern void Cbuf_AddText(const char *text);
extern void Dvar_SetString(dvar_t *dvar, const char *value);
extern void Dvar_SetInt(dvar_t *dvar, int value);
extern int Dvar_GetInt(const char *name);
extern void Dvar_GetUnpackedColorByName(const char *name, float *color);
extern void CalcScreenPlacement(float *x, float *y, float *w, float *h, int horzAlign, int vertAlign);
extern void SetScreenScaling(float scaleX, float scaleY, float x, float y, float w, float h);
extern void StatMon_Warning(int type, int threshold, const char *msg);
extern char *va( const char *format, ... );
extern unsigned char ColorIndex(unsigned char c);
extern int atoi(const char *str);
extern char *strcat(char *dest, const char *src);

static int warnCount;
static int warnCount_00f13084;
static char bigConfigString[8192];
extern const vec4_t g_color_table[];
extern clientActive_t clients[1];

#define RE (&re)
#define CLS ((&::cls))
#define CL_LOCAL ((clientActive_t *)((void *)clients))
#define CLUI_STATE (&::clc)

void CL_FirstSnapshot(void);
void CL_GetScreenDimensions(int *width, int *height, float *aspect);
qboolean CL_GetUserCmd(int cmdNumber, usercmd_t *ucmd);
int CL_GetCurrentCmdNumber(void);
void CL_GetCurrentSnapshotNumber(int *snapshotNumber, int *serverTime);
qboolean CL_GetSnapshot(int snapshotNumber, snapshot_t *snapshot);
void CL_SetUserCmdValue(int userCmdValue, int holdableValue, float sensitivityScale);
void CL_SetUserCmdInShellshock(qboolean shocked);
void CL_AddCgameCommand(const char *cmdName);
void CL_ConfigstringModified(void);
qboolean CL_GetServerCommand(int serverCommandNumber);
void CL_SetExpectedHunkUsage(const char *mapname);
void CL_CM_LoadMap(const char *mapname);
void CL_ShutdownCGame(void);
qboolean CL_DObjCreateSkelForBone(struct DObj_s *obj, int boneIndex, int localClientNum);
qboolean CL_DObjCreateSkelForBones(const struct DObj_s *obj, int *partBits, int localClientNum);
void CL_SubtitlePrint(const char *pszText, int iDuration, int iLineWidth);
const char *CL_GetConfigString(int index);
snd_alias_t *CL_PickSoundAlias(const char *aliasname);
struct XModel *CL_RegisterModel(const char *name);
struct GfxBrushModel *CL_RegisterInlineModel(int modelIndex);
void CL_FinishLoadingModels(void);
MaterialHandle CL_RegisterMaterial(const char *name, int imageTrack);
MaterialHandle CL_RegisterMaterialNoMip(const char *name, int imageTrack);
MaterialHandle CL_Material_Duplicate(MaterialHandle mtlCopy, const char *name);
void CL_DuplicateFont(FontHandle fontCopy, const char *name);
void CL_ClearScene(void);
void CL_DefaultVertexFrames(int vertCount, GfxWorldVertex *verts);
void CL_AddPolyToScene(MaterialHandle mtlHandle, unsigned short lmapIndex, unsigned short vertCount, const GfxWorldVertex *verts);
void CL_AddLightToScene(const vec_t *org, float radius, float r, float g, float b);
void CL_SetFog(int fogvar, float start, float end, int r, int g, int b, float density);
void CL_SwitchFog(int fogvar, int startTime, int transitionTime);
void CL_RenderScene(const refdef_t *fd);
int CL_BeginDelayedDrawing(void);
void CL_EndDelayedDrawing(int marker);
void CL_IssueDelayedDrawing(int marker);
void CL_SetViewport(int x, int y, int width, int height);
void CL_SaveScreen(void);
void CL_BlendSavedScreen(int fadeMsec);
void CL_DrawStretchPicPhysical(float x, float y, float w, float h, float s1, float t1, float s2, float t2, const vec_t *color, MaterialHandle material);
void CL_DrawQuadPic(vec2_t *verts, const vec_t *color, MaterialHandle material);
void CL_DrawSprite(MaterialHandle material, const byte *rgbaColor, const vec_t *pos, float radius, float minScreenRadius, int renderFxFlags);
void CL_ModelBounds(struct GfxBrushModel *model, vec_t *mins, vec_t *maxs);
int CL_MarkFragments(const vec3_t *points, const vec_t *origin, const vec3_t *axis, float radius, int maxPoints, GfxWorldVertex *verts, int maxFragments, GfxMarkFragment *fragmentBuffer, MaterialHandle markMaterial);
void CL_TrackStatistics(trStatistics_t *pStats);
qboolean CL_PickMaterial(const vec_t *org, const vec_t *dir, char *pszName, char *pszSurfaceFlags, char *pszContents, int iMaxChars);
void CL_LoadSoundAliases(const char *loadspec);
qboolean CL_Popup(const char *menu);
void CL_ClosePopup(const char *menu);
void CL_CloseInGameMenu(void);
void CL_CloseAllMenus(void);
void CL_SetWeaponInfoMemory(void);
void CL_FreeWeaponInfoMemory(void);
void CL_FX_AdjustCamera(refdef_t *refdef);
void CL_CapTurnRate(float maxPitchSpeed, float maxYawSpeed);
void LoadWorld(const char *mapname);
void CL_UpdateLevelHunkUsage(void);
void CL_StartLoading(const char *mapname, const char *gametype);
void CL_InitCGame(void);
qboolean CL_GameCommand(void);
void CL_AdjustTimeDelta(void);
void CL_SetADS(int ads);
void CL_DrawString(int x, int y, const char *pszString, qboolean bShadow, int iCharHeight);
void CL_ArchiveClientState(MemoryFile *memFile);
void CL_SetLodOrigin(const refdef_t *fd);
void CL_LookupColor(unsigned char c, vec_t *color);
void CL_SetUserCmdAimValues(vec_t *kickAngles);
void CL_UpdateColor(void);
void CL_SetFullScreenViewport(void);
void CL_DrawStretchPic(float x, float y, float w, float h, int horzAlign, int vertAlign, float s1, float t1, float s2, float t2, const vec_t *color, MaterialHandle material);
void CL_SyncTimes(void);
void CL_SetCGameTime(void);

void CL_GetScreenDimensions(int *width, int *height, float *aspect);
qboolean CL_GetUserCmd(int cmdNumber, usercmd_t *ucmd);
int CL_GetCurrentCmdNumber(void);
void CL_GetCurrentSnapshotNumber(int *snapshotNumber, int *serverTime);
qboolean CL_GetSnapshot(int snapshotNumber, snapshot_t *snapshot);
void CL_SetUserCmdValue(int userCmdValue, int holdableValue, float sensitivityScale);
void CL_SetUserCmdInShellshock(qboolean shocked);
void CL_AddCgameCommand(const char *cmdName);
void CL_ConfigstringModified(void);
qboolean CL_GetServerCommand(int serverCommandNumber);
void CL_SetExpectedHunkUsage(const char *mapname);
void CL_CM_LoadMap(const char *mapname);
void CL_ShutdownCGame(void);
qboolean CL_DObjCreateSkelForBone(struct DObj_s *obj, int boneIndex, int localClientNum);
qboolean CL_DObjCreateSkelForBones(const struct DObj_s *obj, int *partBits, int localClientNum);
void CL_SubtitlePrint(const char *pszText, int iDuration, int iLineWidth);
const char *CL_GetConfigString(int index);
snd_alias_t *CL_PickSoundAlias(const char *aliasname);
struct XModel *CL_RegisterModel(const char *name);
struct GfxBrushModel *CL_RegisterInlineModel(int modelIndex);
void CL_FinishLoadingModels(void);
MaterialHandle CL_RegisterMaterial(const char *name, int imageTrack);
MaterialHandle CL_RegisterMaterialNoMip(const char *name, int imageTrack);
MaterialHandle CL_Material_Duplicate(MaterialHandle mtlCopy, const char *name);
void CL_DuplicateFont(FontHandle fontCopy, const char *name);
void CL_ClearScene(void);
void CL_DefaultVertexFrames(int vertCount, GfxWorldVertex *verts);
void CL_AddPolyToScene(MaterialHandle mtlHandle, unsigned short lmapIndex, unsigned short vertCount, const GfxWorldVertex *verts);
void CL_AddLightToScene(const vec_t *org, float radius, float r, float g, float b);
void CL_SetFog(int fogvar, float start, float end, int r, int g, int b, float density);
void CL_SwitchFog(int fogvar, int startTime, int transitionTime);
void CL_RenderScene(const refdef_t *fd);
int CL_BeginDelayedDrawing(void);
void CL_EndDelayedDrawing(int marker);
void CL_IssueDelayedDrawing(int marker);
void CL_SetViewport(int x, int y, int width, int height);
void CL_SaveScreen(void);
void CL_BlendSavedScreen(int fadeMsec);
void CL_DrawStretchPicPhysical(float x, float y, float w, float h, float s1, float t1, float s2, float t2, const vec_t *color, MaterialHandle material);
void CL_DrawQuadPic(vec2_t *verts, const vec_t *color, MaterialHandle material);
void CL_DrawSprite(MaterialHandle material, const byte *rgbaColor, const vec_t *pos, float radius, float minScreenRadius, int renderFxFlags);
void CL_ModelBounds(struct GfxBrushModel *model, vec_t *mins, vec_t *maxs);
int CL_MarkFragments(const vec3_t *points, const vec_t *origin, const vec3_t *axis, float radius, int maxPoints, GfxWorldVertex *verts, int maxFragments, GfxMarkFragment *fragmentBuffer, MaterialHandle markMaterial);
void CL_TrackStatistics(trStatistics_t *pStats);
qboolean CL_PickMaterial(const vec_t *org, const vec_t *dir, char *pszName, char *pszSurfaceFlags, char *pszContents, int iMaxChars);
void CL_LoadSoundAliases(const char *loadspec);
qboolean CL_Popup(const char *menu);
void CL_ClosePopup(const char *menu);
void CL_CloseInGameMenu(void);
void CL_CloseAllMenus(void);
void CL_SetWeaponInfoMemory(void);
void CL_FreeWeaponInfoMemory(void);
void CL_FX_AdjustCamera(refdef_t *refdef);
void CL_CapTurnRate(float maxPitchSpeed, float maxYawSpeed);
void LoadWorld(const char *mapname);
void CL_UpdateLevelHunkUsage(void);
void CL_StartLoading(const char *mapname, const char *gametype);
void CL_InitCGame(void);
qboolean CL_GameCommand(void);
void CL_AdjustTimeDelta(void);
void CL_SetADS(int ads);
void CL_DrawString(int x, int y, const char *pszString, qboolean bShadow, int iCharHeight);
void CL_ArchiveClientState(MemoryFile *memFile);
void CL_SetLodOrigin(const refdef_t *fd);
void CL_LookupColor(unsigned char c, vec_t *color);
void CL_SetUserCmdAimValues(vec_t *kickAngles);
void CL_UpdateColor(void);
void CL_SetFullScreenViewport(void);
void CL_DrawStretchPic(float x, float y, float w, float h, int horzAlign, int vertAlign, float s1, float t1, float s2, float t2, const vec_t *color, MaterialHandle material);
void CL_SyncTimes(void);
void CL_SetCGameTime(void);

extern void *s_cmdList;

int g_cginit_loading = 0;

static const dvar_t *CL_DvarFromImport(void *importPtr)
{
    return importPtr ? *(const dvar_t **)importPtr : NULL;
}

const char *CL_GetConfigString(int index)
{
    return clients[0].gameState.stringData + clients[0].gameState.stringOffsets[index];
}

static qboolean CL_DvarCurrentBool(void *importPtr)
{
    const dvar_t *dvar = CL_DvarFromImport(importPtr);
    return dvar && dvar->current.enabled;
}

static int CL_DvarCurrentInt(void *importPtr)
{
    const dvar_t *dvar = CL_DvarFromImport(importPtr);
    return dvar ? dvar->current.integer : 0;
}

static const char *CL_DvarCurrentString(void *importPtr)
{
    const dvar_t *dvar = CL_DvarFromImport(importPtr);
    return dvar ? dvar->current.string : NULL;
}

static float CL_ComTimescaleValue(void)
{
    return ((float)com_timescaleValue);
}

// retail 0x401240, unreferenced; Q3 CL_GetGameState (SUPPORTED)
void CL_GetGameState(gameState_t *gs)
{
    *gs = clients->gameState;
}

void CL_GetScreenDimensions(int *width, int *height, float *aspect)
{
    clientStatic_t *cls = CLS;
    *width = cls->vidConfig.width;
    *height = cls->vidConfig.height;
    *aspect = cls->vidConfig.aspectRatioWindow;
}

qboolean CL_GetUserCmd(int cmdNumber, usercmd_t *ucmd)
{
    clientActive_t *cl = CL_LOCAL;
    int currentCmd = cl->cmdNumber;

    if (cmdNumber > currentCmd) {
        Com_Error(ERR_DROP, "CL_GetUserCmd: %i >= %i", cmdNumber, currentCmd);
        currentCmd = cl->cmdNumber;
    }

    if (cmdNumber <= currentCmd - 128) {
        return 0;
    }

    int idx = cmdNumber & 0x7f;
    *ucmd = cl->cmds[idx];
    return 1;
}

int CL_GetCurrentCmdNumber(void)
{
    clientActive_t *cl = CL_LOCAL;
    return cl->cmdNumber;
}

void CL_GetCurrentSnapshotNumber(int *snapshotNumber, int *serverTime)
{
    clientActive_t *cl = CL_LOCAL;
    *snapshotNumber = cl->snap.messageNum;
    *serverTime = cl->snap.serverTime;
}

qboolean CL_GetSnapshot(int snapshotNumber, snapshot_t *snapshot)
{
    clientActive_t *cl = CL_LOCAL;
    int count;
    int i;

    if (snapshotNumber > cl->snap.messageNum) {
        Com_Error(ERR_DROP, "CL_GetSnapshot: snapshotNumber > cl->snapshot.messageNum");
    }

    if (cl->snap.messageNum - snapshotNumber >= 0x20) {
        return 0;
    }

    int idx = snapshotNumber & 0x1f;
    clSnapshot_t *clSnap = &cl->snapshots[idx];

    if (clSnap->valid == 0) {
        return 0;
    }

    if (cl->parseEntitiesNum - clSnap->parseEntitiesNum >= 0x800) {
        return 0;
    }
    if (cl->parseClientsNum - clSnap->parseClientsNum >= 0x800) {
        return 0;
    }

    snapshot->snapFlags = clSnap->snapFlags;
    snapshot->serverCommandSequence = clSnap->serverCommandNum;
    snapshot->ping = clSnap->ping;
    snapshot->serverTime = clSnap->serverTime;

    memcpy(&snapshot->ps, &clSnap->ps, sizeof(playerState_t));

    count = clSnap->numEntities;
    if (count > 256) {
        if ((com_statmon)->current.enabled) {
            StatMon_Warning(4, 3000, "gfx/2d/warning@snapshotents.jpg");
        } else {
            Com_DPrintf("CL_GetSnapshot: truncated %i entities to %i\n", count, 256);
        }
        count = 256;
    }

    snapshot->numEntities = count;

    for (i = 0; i < count; i++) {
        int entIdx = (i + clSnap->parseEntitiesNum) & 0x7ff;
        memcpy(&snapshot->entities[i], &cl->parseEntities[entIdx], sizeof(entityState_t));
    }

    int clientCount = clSnap->numClients;
    if (clientCount > 64) {
        clientCount = 64;
    }
    snapshot->numClients = clientCount;

    for (i = 0; i < clientCount; i++) {
        int cliIdx = (i + clSnap->parseClientsNum) & 0x7ff;
        memcpy(&snapshot->clients[i], &cl->parseClients[cliIdx], sizeof(clientState_t));
    }

    return 1;
}

void CL_SetUserCmdValue(int userCmdValue, int holdableValue, float sensitivityScale)
{
    clientActive_t *cl = CL_LOCAL;
    cl->cgameUserCmdValue = userCmdValue;
    cl->cgameUserHoldableValue = holdableValue;
    cl->cgameSensitivity = sensitivityScale;
}

void CL_SetUserCmdInShellshock(qboolean shocked)
{
    clientActive_t *cl = CL_LOCAL;
    cl->cgameInShellshock = shocked;
}

// retail 0x4014e0, unreferenced; name unknown
qboolean CL_GetUserCmdInShellshock_4014e0(void)
{
    clientActive_t *cl = CL_LOCAL;
    return cl->cgameInShellshock;
}

void CL_SetUserCmdAimValues(vec_t *kickAngles)
{
    clientActive_t *cl = CL_LOCAL;
    vec_t *dst = cl->cgameKickAngles;
    dst[0] = kickAngles[0];
    dst[1] = kickAngles[1];
    dst[2] = kickAngles[2];
}

void CL_AddCgameCommand(const char *cmdName)
{
    Cmd_AddCommand(cmdName, 0);
}

void CL_ConfigstringModified(void)
{
    const char *old, *s;
    int i, index;
    const char *dup;
    gameState_t oldGs;
    int len;

    index = atoi(Cmd_Argv(1));
    if (index < 0 || index >= 0x800)
        Com_Error(ERR_DROP, "\x15" "configstring > MAX_CONFIGSTRINGS");
    s = Cmd_Argv(2);

    old = clients[0].gameState.stringData + clients[0].gameState.stringOffsets[index];
    if (!strcmp(old, s))
        return;

    oldGs = clients[0].gameState;
    memset(&clients[0].gameState, 0, sizeof(clients[0].gameState));
    clients[0].gameState.dataCount = 1;

    for (i = 0; i < 0x800; i++) {
        if (i == index)
            dup = s;
        else
            dup = oldGs.stringData + oldGs.stringOffsets[i];
        if (!dup[0])
            continue;
        len = strlen(dup);
        if (len + 1 + clients[0].gameState.dataCount > 16000)
            Com_Error(ERR_DROP, "\x15MAX_GAMESTATE_CHARS exceeded");
        clients[0].gameState.stringOffsets[i] = clients[0].gameState.dataCount;
        memcpy(clients[0].gameState.stringData + clients[0].gameState.dataCount, dup, len + 1);
        clients[0].gameState.dataCount += len + 1;
    }

    if (index == 1)
        CL_SystemInfoChanged();
}

qboolean CL_GetServerCommand(int serverCommandNumber)
{
    clientConnection_t *clui = CLUI_STATE;
    char *s;
    const char *cmd;
    int argc;

    if (serverCommandNumber <= clui->serverCommandSequence - 128) {
        if (clui->demoplaying) {
            return 0;
        }
        Com_Printf("CL_GetServerCommand: a]command was lost\n");
        Com_Printf("serverCommandNumber: %d\n", serverCommandNumber & 0x7f);
        {
            int j;
            for (j = 0; j < 128; j++) {
                clientConnection_t *clui2 = CLUI_STATE;
                Com_Printf("cmd %5d: %s\n", j, clui2->serverCommands[j & 0x7f]);
            }
        }
        Com_Error(ERR_DROP, "CL_GetServerCommand: too many missed");
    }

    clui = CLUI_STATE;
    if (serverCommandNumber > clui->serverCommandSequence) {
        Com_Error(ERR_DROP, "CL_GetServerCommand: requested beyond last");
    }

    s = clui->serverCommands[serverCommandNumber & 0x7f];
    clui->lastExecutedServerCommand = serverCommandNumber;

    if ((cl_showServerCommands)->current.enabled) {
        Com_DPrintf("serverCommand: %i : %s\n", serverCommandNumber, s);
    }

restart:
    Cmd_TokenizeString(s);
    cmd = Cmd_Argv(0);
    argc = Cmd_Argc();

    switch (cmd[0]) {
    case 'w':
        if (argc >= 2) {
            const char *reason = SEH_SafeTranslateString(Cmd_Argv(1));
            const char *base = SEH_SafeTranslateString("EXE_SERVERDISCONNECTREASON");
            const char *msg = UI_ReplaceConversionString(base, reason);
            Com_Error(ERR_SERVERDISCONNECT, msg);
        } else {
            Com_Error(ERR_SERVERDISCONNECT, "EXE_DISCONNECTED");
        }

    case 'x':
        Cmd_TokenizeStringWithLimit(s, 3);
        {
            const char *arg2 = Cmd_Argv(2);
            const char *arg1 = Cmd_Argv(1);
            Com_sprintf(bigConfigString, 0x2000, "%c %s %s", 'd', arg1, arg2);
        }
        return 0;
    case 'y':
        Cmd_TokenizeStringWithLimit(s, 3);
        {
            const char *arg2 = Cmd_Argv(2);
            if (strlen(bigConfigString) + strlen(arg2) >= 0x2000) {
                Com_Error(ERR_DROP, "BCS overflow");
            }
            strcat(bigConfigString, arg2);
        }
        return 0;
    case 'z':
        Cmd_TokenizeStringWithLimit(s, 3);
        {
            const char *arg2 = Cmd_Argv(2);
            if (strlen(bigConfigString) + strlen(arg2) + 1 >= 0x2000) {
                Com_Error(ERR_DROP, "BCS overflow");
            }
            strcat(bigConfigString, arg2);
        }
        s = bigConfigString;
        goto restart;
    case 'd':
        Cmd_TokenizeStringWithLimit(s, 3);
        CL_ConfigstringModified();
        Cmd_TokenizeStringWithLimit(s, 3);
        return 1;
    case 'B':
    case 'n':
        Con_ClearNotify();
        Con_ClearSubtitles();
        memset(CL_LOCAL->cmds, 0, sizeof(CL_LOCAL->cmds));
        RE->ClearFlares();
        return 1;
    default:
        return 1;
    }
}

void CL_SetExpectedHunkUsage(const char *mapname)
{
    int handle;
    int len;
    char *buf;
    const char *buftrav;
    const char *token;

    len = FS_FOpenFileByMode("hunkusage.dat", &handle, FS_READ);
    if (len < 0) {
        goto set_default;
    }

    buf = (char *)Z_MallocInternal(len + 1);
    memset(buf, 0, len + 1);
    FS_Read(buf, len, handle);
    FS_FCloseFile(handle);

    buftrav = buf;
    while (1) {
        token = Com_Parse(&buftrav);
        if (!token || *token == '\0')
            break;

        if (I_stricmp(token, mapname) != 0)
            continue;

        token = Com_Parse(&buftrav);
        if (!token || *token == '\0')
            continue;

        Dvar_SetInt( (dvar_t *)com_expectedHunkUsage, atoi(token));
        Z_FreeInternal(buf);
        return;
    }

    Z_FreeInternal(buf);

set_default:
    Dvar_SetInt( (dvar_t *)com_expectedHunkUsage, 0);
}

void CL_CM_LoadMap(const char *mapname)
{
    int checksum;

    Com_LoadBsp(mapname);
    CM_LoadMap(mapname, &checksum);

    if (!(com_sv_running)->current.enabled) {
        CM_LinkWorld();
    }
}

void CL_ShutdownCGame(void)
{
    clientActive_t *cl;

    Com_UnloadSoundAliases(SASYS_CGAME);
    cl = CL_LOCAL;
    if (cl->cgameInitCalled) {
        CG_Shutdown();
        cl->cgameInitCalled = 0;
        cl->cgameInitialized = 0;
    }
}

qboolean CL_DObjCreateSkelForBone(struct DObj_s *obj, int boneIndex, int localClientNum)
{
    int timeStamp;
    void *mem;

    timeStamp = CL_GetSkelTimeStamp(localClientNum);

    if (DObjSkelExists((DObj_s *)obj, timeStamp)) {
        return DObjSkelIsBoneUpToDate(obj, boneIndex);
    }

    mem = CL_AllocSkelMemory(localClientNum, DObjGetAllocSkelSize(obj));
    if (mem) {
        DObjCreateSkel((DObj_s *)obj, (DSkel_t *)mem, timeStamp);
        return 0;
    }

    if (warnCount != timeStamp) {
        warnCount = timeStamp;
        Com_Printf("^3WARNING: CL_SKEL_MEMORY_SIZE exceeded - not calculating skeleton\n");
    }
    return 1;
}

qboolean CL_DObjCreateSkelForBones(const struct DObj_s *obj, int *partBits, int localClientNum)
{
    int timeStamp;
    void *mem;

    timeStamp = CL_GetSkelTimeStamp(localClientNum);

    if (DObjSkelExists((DObj_s *)obj, timeStamp)) {
        return DObjSkelAreBonesUpToDate(obj, partBits);
    }

    mem = CL_AllocSkelMemory(localClientNum, DObjGetAllocSkelSize(obj));
    if (mem) {
        DObjCreateSkel((DObj_s *)obj, (DSkel_t *)mem, timeStamp);
        return 0;
    }

    if (warnCount_00f13084 != timeStamp) {
        warnCount_00f13084 = timeStamp;
        Com_Printf("^3WARNING: CL_SKEL_MEMORY_SIZE exceeded - not calculating skeleton\n");
    }
    return 1;
}

void CL_SubtitlePrint(const char *pszText, int iDuration, int iLineWidth)
{
    const char *translated = SEH_StringEd_GetString(pszText);

    if (!translated) {
        if (loc_warnings->current.enabled) {
            if (loc_warningsAsErrors->current.enabled)
                Com_Error(ERR_LOCALIZATION, "Could not translate subtitle text: \"%s\"", pszText);
            else
                Com_Printf("^3WARNING: Could not translate subtitle text: \"%s\"\n", pszText);
            translated = va("^1UNLOCALIZED(^7%s^1)^7", pszText);
        } else {
            translated = pszText;
        }
    }

    CL_ConsolePrint((print_msg_type_t)3, translated, iDuration, iLineWidth);
}

snd_alias_t *CL_PickSoundAlias(const char *aliasname)
{
    return Com_PickSoundAlias(aliasname);
}

extern void DObjCreate(struct DObjModel_s *dobjModels, unsigned int numModels, struct XAnimTree_s *tree, struct DObj_s *obj, unsigned int entnum);

// retail 0x402040, unreferenced; name unknown
void CL_DObjCreate_402040(struct DObjModel_s *dobjModels, unsigned short numModels, struct XAnimTree_s *tree, struct DObj_s *obj)
{
    DObjCreate(dobjModels, numModels, tree, obj, 0);
}

struct XModel *CL_RegisterModel(const char *name)
{
    return RE->RegisterModel(name);
}

struct GfxBrushModel *CL_RegisterInlineModel(int modelIndex)
{
    return RE->RegisterInlineModel(modelIndex);
}

void CL_FinishLoadingModels(void)
{
    RE->FinishLoadingModels();
}

// retail 0x402090, unreferenced; name unknown
void CL_SetIgnorePrecacheErrors_402090(qboolean ignore)
{
    RE->SetIgnorePrecacheErrors(ignore);
}

MaterialHandle CL_RegisterMaterial(const char *name, int imageTrack)
{
    return RE->RegisterMaterial(name, 3, imageTrack);
}

MaterialHandle CL_RegisterMaterialNoMip(const char *name, int imageTrack)
{
    return RE->RegisterMaterial(name, 0x30, imageTrack);
}

MaterialHandle CL_Material_Duplicate(MaterialHandle mtlCopy, const char *name)
{
    return RE->Material_Duplicate(mtlCopy, name);
}

void CL_DuplicateFont(FontHandle fontCopy, const char *name)
{
    RE->DuplicateFont(fontCopy, name);
}

void CL_ClearScene(void)
{
    RE->ClearScene();
}

void R_SkinGfxEntityDelayed(GfxEntity *ent)
{
    RE->SkinGfxEntityDelayed(ent);
}

GfxEntity *R_AddRefEntityToScene(const GfxEntity *refEnt, GfxModel sceneModel, const centity_t *cent)
{
    return RE->AddRefEntityToScene(refEnt, sceneModel, cent);
}

void CL_DefaultVertexFrames(int vertCount, GfxWorldVertex *verts)
{
    RE->DefaultVertexFrames(vertCount, verts);
}

void CL_AddPolyToScene(MaterialHandle mtlHandle, unsigned short lmapIndex, unsigned short vertCount, const GfxWorldVertex *verts)
{
    RE->AddPolyToScene(mtlHandle, lmapIndex, vertCount, verts);
}

void CL_AddLightToScene(const vec_t *org, float radius, float r, float g, float b)
{
    RE->AddLightToScene(org, radius, r, g, b);
}

void CL_SetFog(int fogvar, float start, float end, int r, int g, int b, float density)
{
    RE->SetFog(fogvar, start, end, r, g, b, density);
}

void CL_SwitchFog(int fogvar, int startTime, int transitionTime)
{
    RE->SwitchFog(fogvar, startTime, transitionTime);
}

void CL_RenderScene(const refdef_t *fd)
{
    RE->RenderScene(fd);
}

int CL_BeginDelayedDrawing(void)
{
    return RE->BeginDelayedDrawing();
}

void CL_EndDelayedDrawing(int marker)
{
    RE->EndDelayedDrawing(marker);
}

void CL_IssueDelayedDrawing(int marker)
{
    RE->IssueDelayedDrawing(marker);
}

void CL_SetViewport(int x, int y, int width, int height)
{
    RE->SetViewport(x, y, width, height);
}

void CL_SaveScreen(void)
{
    RE->SaveScreen();
}

void CL_BlendSavedScreen(int fadeMsec)
{
    RE->BlendSavedScreen(fadeMsec);
}

void CL_SetFullScreenViewport(void)
{
    clientStatic_t *cls = CLS;

    RE->SetViewport(0, 0, cls->vidConfig.width, cls->vidConfig.height);
    SetScreenScaling(1.0f, 1.0f, 0, 0, cls->vidConfig.width, cls->vidConfig.height);
}

void CL_DrawStretchPicPhysical(float x, float y, float w, float h, float s1, float t1, float s2, float t2, const vec_t *color, MaterialHandle material)
{
#if defined(COD2_X64) && !defined(COD2_GFX_DLL)
    R_AddCmdDrawStretchPic(x, y, w, h, s1, t1, s2, t2, color, material);
#else
    RE->DrawStretchPic(x, y, w, h, s1, t1, s2, t2, color, material);
#endif
}

void CL_DrawQuadPic(vec2_t *verts, const vec_t *color, MaterialHandle material)
{
#if defined(COD2_X64) && !defined(COD2_GFX_DLL)
    R_AddCmdDrawQuadPic(verts, color, material);
#else
    RE->DrawQuadPic(verts, color, material);
#endif
}

void CL_DrawStretchPic(float x, float y, float w, float h, int horzAlign, int vertAlign, float s1, float t1, float s2, float t2, const vec_t *color, MaterialHandle material)
{
    CalcScreenPlacement(&x, &y, &w, &h, horzAlign, vertAlign);
#if defined(COD2_X64) && !defined(COD2_GFX_DLL)
    R_AddCmdDrawStretchPic(x, y, w, h, s1, t1, s2, t2, color, material);
#else
    RE->DrawStretchPic(x, y, w, h, s1, t1, s2, t2, color, material);
#endif
}

#if !defined(COD2_X64) || defined(COD2_GFX_DLL)
// retail 0x4022d0, unreferenced; name unknown
void CL_DrawStretchPicRotate_4022d0(float x, float y, float w, float h, float s1, float t1, float s2, float t2, float angle, const vec_t *color, MaterialHandle material)
{
    RE->DrawStretchPicRotate(x, y, w, h, s1, t1, s2, t2, angle, color, material);
}

// retail 0x402310, unreferenced; name unknown
void CL_GetWorldBounds_402310(vec_t *mins, vec_t *maxs)
{
    RE->GetWorldBounds(mins, maxs);
}
#endif

void CL_SyncTimes(void)
{
    clientConnection_t *clui = CLUI_STATE;
    if (clui->state == 8)
        CL_FirstSnapshot();
}

void CL_DrawSprite(MaterialHandle material, const byte *rgbaColor, const vec_t *pos, float radius, float minScreenRadius, int renderFxFlags)
{
    RE->DrawSprite(material, rgbaColor, pos, radius, minScreenRadius, renderFxFlags);
}

void CL_ModelBounds(struct GfxBrushModel *model, vec_t *mins, vec_t *maxs)
{
    RE->ModelBounds(model, mins, maxs);
}

int CL_MarkFragments(const vec3_t *points, const vec_t *origin, const vec3_t *axis, float radius, int maxPoints, GfxWorldVertex *verts, int maxFragments, GfxMarkFragment *fragmentBuffer, MaterialHandle markMaterial)
{
    return RE->MarkFragments((void *)points, origin, (void *)axis, radius, maxPoints, verts, maxFragments, fragmentBuffer, markMaterial);
}

void CL_TrackStatistics(trStatistics_t *pStats)
{
    RE->TrackStatistics(pStats);
}

qboolean CL_PickMaterial(const vec_t *org, const vec_t *dir, char *pszName, char *pszSurfaceFlags, char *pszContents, int iMaxChars)
{
    return RE->PickMaterial(org, dir, pszName, pszSurfaceFlags, pszContents, iMaxChars);
}

void CL_LoadSoundAliases(const char *loadspec)
{
    Com_LoadSoundAliases(loadspec, "all_mp", (snd_alias_system_t)(1));
}

qboolean CL_Popup(const char *menu)
{
    if (clc.state != CA_ACTIVE || clc.demoplaying || UI_IsFullscreen())
        return 0;

    if (!I_stricmp(menu, "UIMENU_WM_QUICKMESSAGE")) {
        UI_SetActiveMenu(UIMENU_WM_QUICKMESSAGE);
    } else if (!I_stricmp(menu, "UIMENU_WM_AUTOUPDATE")) {
        UI_SetActiveMenu(UIMENU_WM_AUTOUPDATE);
    } else if (!I_strnicmp(menu, "UIMENU_SCRIPT_POPUP", 19)) {
        if (!I_stricmp(menu, "UIMENU_SCRIPT_POPUP_NO_MOUSE"))
            return UI_SetActiveMenu(UIMENU_SCRIPT_POPUP_NO_MOUSE);
        return UI_SetActiveMenu(UIMENU_SCRIPT_POPUP);
    }
    return 1;
}

void CL_ClosePopup(const char *menu)
{
    if (UI_IsFullscreen())
        return;

    // retail tests SCRIPT_POPUP for both names
    if ((!I_stricmp(menu, "UIMENU_SCRIPT_POPUP_NO_MOUSE") && UI_GetActiveMenu() == UIMENU_SCRIPT_POPUP)
        || (!I_stricmp(menu, "UIMENU_SCRIPT_POPUP") && UI_GetActiveMenu() == UIMENU_SCRIPT_POPUP))
        UI_CloseFocusedMenu();
}

void CL_CloseInGameMenu(void)
{
    if (UI_IsFullscreen())
        return;
    if (UI_GetActiveMenu() != 2)
        return;
    UI_CloseFocusedMenu();
}

void CL_CloseAllMenus(void)
{
    if (!UI_IsFullscreen()) {
        UI_KeyEvent(0x1b, 1);
        UI_KeyEvent(0x1b, 1);
        UI_KeyEvent(0x1b, 1);
    }
}

void CL_SetWeaponInfoMemory(void)
{
    Com_SetWeaponInfoMemory(2);
}

void CL_FreeWeaponInfoMemory(void)
{
    Com_FreeWeaponInfoMemory(2);
}

void CL_FX_AdjustCamera(refdef_t *refdef)
{
    float zfar = RE->GetFarPlaneDist();
    FX_AdjustCamera(refdef, zfar);
}

void CL_CapTurnRate(float maxPitchSpeed, float maxYawSpeed)
{
    clientActive_t *cl = CL_LOCAL;
    cl->cgameMaxPitchSpeed = maxPitchSpeed;
    cl->cgameMaxYawSpeed = maxYawSpeed;
}

// retail 0x4025e0, unreferenced; name unknown
void CL_SetViewAngles_4025e0(const vec_t *angles)
{
    clientActive_t *cl = CL_LOCAL;
    vec_t *dst = cl->viewangles;
    dst[0] = angles[0];
    dst[1] = angles[1];
    dst[2] = angles[2];
}

void LoadWorld(const char *mapname)
{
    RE->LoadWorld(mapname, 0);
    Com_UnloadBsp();
}

void CL_UpdateLevelHunkUsage(void)
{
    int handle;
    int len;
    int memusage;
    char *buf;
    char *outbuf;
    const char *buftrav;
    const char *token;
    char outstr[256];
    int buflen;

    memusage = Hunk_Used();

    len = FS_FOpenFileByMode("hunkusage.dat", &handle, FS_READ);
    if (len < 0)
        goto write_new;

    buflen = len + 1;
    buf = (char *)Z_MallocInternal(buflen);
    memset(buf, 0, buflen);
    outbuf = (char *)Z_MallocInternal(buflen);
    memset(outbuf, 0, buflen);

    FS_Read(buf, len, handle);
    FS_FCloseFile(handle);

    buftrav = buf;
    outbuf[0] = '\0';

    while (1) {
        token = Com_Parse(&buftrav);
        if (!token || *token == '\0')
            break;

        {
            clientActive_t *cl = CL_LOCAL;
            if (I_stricmp(token, cl->mapname) == 0) {
                token = Com_Parse(&buftrav);
                if (!token || *token == '\0')
                    continue;
                if (atoi(token) == memusage) {
                    Z_FreeInternal(buf);
                    Z_FreeInternal(outbuf);
                    return;
                }
                continue;
            }
        }

        I_strncat(outbuf, buflen, token);
        I_strncat(outbuf, buflen, " ");

        token = Com_Parse(&buftrav);
        if (!token || *token == '\0') {
            Com_Error(ERR_DROP, "CL_UpdateLevelHunkUsage: bad hunkusage.dat");
            continue;
        }

        I_strncat(outbuf, buflen, token);
        I_strncat(outbuf, buflen, "\n");
    }

    handle = FS_FOpenFileWrite("hunkusage.dat");
    if (!handle) {
        Com_Error(ERR_DROP, va("EXE_ERR_CANT_CREATE\x15%s", "hunkusage.dat"));
    }

    {
        int outLen = strlen(outbuf);
        if (FS_Write(outbuf, outLen, handle) != outLen) {
            Com_Error(ERR_DROP, va("EXE_ERR_CANT_WRITE\x15%s", "hunkusage.dat"));
        }
    }
    FS_FCloseFile(handle);

    Z_FreeInternal(buf);
    Z_FreeInternal(outbuf);

write_new:

    len = FS_FOpenFileByMode("hunkusage.dat", &handle, FS_APPEND);
    if (!handle) {
        Com_Error(ERR_DROP, "CL_UpdateLevelHunkUsage: cannot open for append");
    }

    {
        clientActive_t *cl = CL_LOCAL;
        Com_sprintf(outstr, 256, "%s %i\n", cl->mapname, memusage);
    }
    FS_Write(outstr, strlen(outstr), handle);
    FS_FCloseFile(handle);

    len = FS_FOpenFileByMode("hunkusage.dat", &handle, FS_READ);
    if (len >= 0) {
        FS_FCloseFile(handle);
    }
}

void CL_StartLoading(const char *mapname, const char *gametype)
{
    char *ptr = (char *)::legacyHacks;
    if (*(int *)(ptr + 4) == 0)
        return;
    CL_StartHunkUsers();
    SCR_UpdateScreenInternal();
}

void CL_InitCGame(void)
{
    int t1;
    char mapname[64];
    clientActive_t *cl;

    t1 = Sys_Milliseconds();
    Con_Close();

    cl = CL_LOCAL;
    {
        int offset = cl->gameState.stringOffsets[0];
        const char *systemInfo = cl->gameState.stringData + offset;
        I_strncpyz(mapname, Info_ValueForKey(systemInfo, "mapname"), 64);
    }

    {
        const char *ext = GetBspExtension();
        Com_sprintf(cl->mapname, 64, "maps/mp/%s.%s", mapname, ext);
    }

    if (!(com_sv_running)->current.enabled) {
        Com_InitDObj();
        CL_SetExpectedHunkUsage(cl->mapname);
    }

    {
        clientConnection_t *clui = CLUI_STATE;
        clui->state = (connstate_t)(6);
        cl->cgameInitCalled = 1;
        CG_Init(clui->clientNum, clui->lastExecutedServerCommand, clui->serverMessageSequence);
        cl->cgameInitialized = 1;
        clui->state = (connstate_t)(7);
    }

    {
        int t2 = Sys_Milliseconds();
        double elapsed = (double)(t2 - t1) / 1000.0;
        Com_Printf("CL_InitCGame: %5.2f seconds\n", elapsed);
    }

    RE->EndRegistration();
    Com_TouchMemory();
    Con_ClearNotify();
    Con_ClearSubtitles();
    CL_UpdateLevelHunkUsage();
}

qboolean CL_GameCommand(void)
{
    clientActive_t *cl = CL_LOCAL;
    if (!cl->cgameInitialized)
        return 0;
    return CG_ConsoleCommand();
}

void CL_AdjustTimeDelta(void)
{
    clientActive_t *cl;
    clientConnection_t *clui;
    int serverTime, frameTime, newDelta, serverTimeDelta, deltaDiff;

    cl = CL_LOCAL;
    cl->newSnapshots = 0;

    clui = CLUI_STATE;
    if (clui->demoplaying)
        return;

    serverTime = cl->snap.serverTime;
    frameTime = serverTime - cl->oldSnapServerTime;

    {
        clientStatic_t *cls = CLS;
        newDelta = serverTime - cls->realtime - frameTime - 5;
    }
    serverTimeDelta = cl->serverTimeDelta;
    deltaDiff = newDelta - serverTimeDelta;

    if (deltaDiff > 0) {
        if ((unsigned int)frameTime <= 500) {
            deltaDiff -= frameTime;
            if (deltaDiff < 0)
                goto smooth;
        }
    } else {
        deltaDiff = -deltaDiff;
    }

    if (deltaDiff > 500) {
        cl->serverTimeDelta = newDelta;
        cl = CL_LOCAL;
        cl->oldServerTime = cl->snap.serverTime;
        cl->serverTime = cl->snap.serverTime;
        if ((cl_showTimeDelta)->current.enabled) {
            Com_Printf("cl_showTimeDelta: reset\n");
        }
        goto debug_print;
    }

    if (deltaDiff > 100) {

        if ((cl_showTimeDelta)->current.enabled) {
            Com_Printf("cl_showTimeDelta: average\n");
        }
        cl = CL_LOCAL;
        cl->serverTimeDelta = (newDelta + cl->serverTimeDelta) >> 1;
        goto debug_print;
    }

smooth: {
    float timescale = CL_ComTimescaleValue();
    if (timescale != 1.0f)
        goto debug_print;
}

    if (cl->extrapolatedSnapshot) {
        cl->extrapolatedSnapshot = 0;
        cl = CL_LOCAL;
        cl->serverTimeDelta -= 2;
    } else if (newDelta > serverTimeDelta) {
        cl->serverTimeDelta = serverTimeDelta + 1;
    } else if (newDelta < serverTimeDelta) {
        cl->serverTimeDelta = serverTimeDelta - 1;
    }

debug_print:
    if ((cl_showTimeDelta)->current.enabled) {
        cl = CL_LOCAL;
        {
            clientStatic_t *cls = CLS;
            Com_Printf("client time: %i, server time: %i\n",
                       cls->realtime + cl->serverTimeDelta,
                       cl->snap.serverTime);
        }
        Com_Printf("snapshot delta: %i, time delta: %i\n",
                   newDelta, cl->serverTimeDelta);
    }
}

void CL_SetADS(int ads)
{
    clientActive_t *cl = CL_LOCAL;
    cl->usingAds = (unsigned char)ads;
}

void CL_DrawString(int x, int y, const char *pszString, qboolean bShadow, int iCharHeight)
{
    clientActive_t *cl = CL_LOCAL;
    float fy;

    if (!cl->cgameInitialized)
        return;
    fy = (float)y;
    CG_DrawStringExt((float)x, fy, pszString, 0, 0, bShadow, (float)iCharHeight, 1);
}

void CL_FirstSnapshot(void)
{
    clientActive_t *cl = CL_LOCAL;
    clientConnection_t *clui = CLUI_STATE;

    if (cl->snap.snapFlags & 2)
        return;

    clui->state = (connstate_t)(8);
    int serverTime = cl->snap.serverTime;
    clientStatic_t *cls = CLS;
    cl->serverTimeDelta = serverTime - cls->realtime;
    cl->oldServerTime = serverTime;
    clui->timeDemoBaseTime = serverTime;

    {
        const char *autorecStr = (cl_activeAction)->current.string;
        if (*autorecStr) {
            Cbuf_AddText(autorecStr);
            Cbuf_AddText("\n");
            Dvar_SetString((dvar_t *)cl_activeAction, "");
        }
    }

}

// retail 0x402e70
char *CL_TimeDemoLogBaseName(void)
{
    char *mapname;
    char *base;
    char *ext;
    char *name;

    ext = NULL;
    base = CL_LOCAL->mapname;
    for (mapname = CL_LOCAL->mapname; *mapname; mapname++) {
        if (*mapname == '/' || *mapname == '\\') {
            base = mapname + 1;
            ext = NULL;
        } else if (*mapname == '.') {
            ext = mapname;
        }
    }
    if (!ext)
        return base;

    name = va("%s", base);
    name[ext - base] = 0;
    return name;
}

// retail 0x402ed0
void CL_UpdateTimeDemo(void)
{
    int now;

    if (!clc.timeDemoLog)
        clc.timeDemoLog = FS_FOpenFileWrite(va("demos/timedemo_%s_mode_%i.csv", CL_TimeDemoLogBaseName(), Dvar_GetInt("r_mode")));

    now = Sys_Milliseconds();
    if (!clc.timeDemoStart)
        clc.timeDemoStart = now;
    else if (clc.timeDemoLog)
        FS_Printf(clc.timeDemoLog, "%i,%i\n", clc.timeDemoFrames, now - clc.timeDemoPrev);

    clc.timeDemoFrames++;
    clc.timeDemoPrev = now;
    CL_LOCAL->serverTime = clc.timeDemoBaseTime + clc.timeDemoFrames * 50;
}

void CL_SetCGameTime(void)
{
    clientActive_t *cl = CL_LOCAL;

    if (clc.state != CA_ACTIVE) {
        if (clc.state != CA_PRIMED)
            return;
        if (clc.demoplaying) {
            if (!clc.firstDemoFrameSkipped) {
                clc.firstDemoFrameSkipped = 1;
                return;
            }
            CL_ReadDemoMessage();
        }
        if (cl->newSnapshots) {
            cl->newSnapshots = 0;
            CL_FirstSnapshot();
        }
        if (clc.state != CA_ACTIVE)
            return;
    }

    if (!cl->snap.valid)
        Com_Error(ERR_DROP, "CL_SetCGameTime: no snapshot");

    if (sv_paused->current.integer && cl_paused->current.integer && com_sv_running->current.enabled)
        return;

    if (cl->snap.serverTime < cl->oldFrameServerTime) {
        if (!I_stricmp(cls.servername, "localhost"))
            CL_FirstSnapshot();
        else
            Com_Error(ERR_DROP, "cl.snap.serverTime < cl.oldFrameServerTime");
    }
    cl->oldFrameServerTime = cl->snap.serverTime;

    if (!clc.demoplaying || !cl_freezeDemo->current.enabled) {
        cl->serverTime = cls.realtime + cl->serverTimeDelta;
        if (cl->serverTime < cl->oldServerTime)
            cl->serverTime = cl->oldServerTime;
        cl->oldServerTime = cl->serverTime;
        if (cls.realtime + cl->serverTimeDelta >= cl->snap.serverTime - 5) {
            cl->extrapolatedSnapshot = 1;
            if (cl_showTimeDelta->current.enabled)
                Com_Printf("Extrapolating snapshot!\n");
        }
    }

    if (cl->newSnapshots)
        CL_AdjustTimeDelta();

    if (!clc.demoplaying)
        return;

    if (clc.isTimeDemo)
        CL_UpdateTimeDemo();

    while (cl->serverTime >= cl->snap.serverTime) {
        CL_ReadDemoMessage();
        if (clc.state != CA_ACTIVE)
            return;
    }
}

void CL_ArchiveClientState(MemoryFile *memFile)
{
    CG_ArchiveState(memFile);
    RE->ArchiveFogState(memFile);
}

void CL_SetLodOrigin(const refdef_t *fd)
{
    RE->SetLodOrigin(fd);
}

void R_UpdateXModelBoundsDelayed(GfxEntity *ent)
{
    RE->UpdateXModelBoundsDelayed(ent);
}

void R_UpdateEffectsBolt(void)
{
    RE->UpdateEffectsBolt();
}

void R_UpdateEffectsNonBolt(void)
{
    RE->UpdateEffectsNonBolt();
}

void CL_LookupColor(unsigned char c, vec_t *color)
{
    unsigned int index;
    const float *src;

    index = ColorIndex(c);
    if (index < 8) {
        src = g_color_table[index];
        color[0] = src[0];
        color[1] = src[1];
        color[2] = src[2];
        color[3] = src[3];
        return;
    }

    switch (c) {
    case '8':
        color[0] = CL_LOCAL->color_allies[0];
        color[1] = CL_LOCAL->color_allies[1];
        color[2] = CL_LOCAL->color_allies[2];
        color[3] = CL_LOCAL->color_allies[3];
        break;
    case '9':
        color[0] = CL_LOCAL->color_axis[0];
        color[1] = CL_LOCAL->color_axis[1];
        color[2] = CL_LOCAL->color_axis[2];
        color[3] = CL_LOCAL->color_axis[3];
        break;
    default:
        color[0] = 1.0f;
        color[1] = 1.0f;
        color[2] = 1.0f;
        color[3] = 1.0f;
        break;
    }
}

// retail 0x4032c0; name unknown
void CL_GetTeamColor_4032c0(const char *dvarName, float *color)
{
    Dvar_GetUnpackedColorByName(dvarName, color);
    color[3] = 1.0f;
}

void CL_UpdateColor(void)
{
    clientActive_t *cl = CL_LOCAL;
    float *alliesColor = (float *)cl->color_allies;
    float *axisColor;

    CL_GetTeamColor_4032c0("g_TeamColor_Allies", alliesColor);

    axisColor = (float *)cl->color_axis;
    CL_GetTeamColor_4032c0("g_TeamColor_Axis", axisColor);

    RE->UpdateColor(alliesColor, axisColor);
}


static const vec4_t g_color_table[8] = {
    { 0.0f, 0.0f, 0.0f, 1.0f },
    { 1.0f, 0.0f, 0.0f, 1.0f },
    { 0.0f, 1.0f, 0.0f, 1.0f },
    { 1.0f, 1.0f, 0.0f, 1.0f },
    { 0.0f, 0.0f, 1.0f, 1.0f },
    { 0.0f, 1.0f, 1.0f, 1.0f },
    { 1.0f, 0.0f, 1.0f, 1.0f },
    { 1.0f, 1.0f, 1.0f, 1.0f }
};
