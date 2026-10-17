#include "common_types.h"
#include "imports.h"
#include "bytematch.h"
#include "headers/PC/cgame_mp/cg_local.h"

#include <string.h>
#include <stdlib.h>
extern const dvar_t *cg_showmiss;
extern const dvar_t *cg_teamChatsOnly;
extern displayContextDef_t cgDC;

extern const dvar_t *cg_chatHeight;
extern const dvar_t *cg_chatTime;
extern const dvar_t *cg_thirdPerson;

extern float floorf(float x);

extern const char *CL_GetConfigString(int index);
extern const char *Info_ValueForKey( const char *s, const char *key );
extern void Dvar_SetStringByName(const char *dvarName, const char *value);
extern void Dvar_SetFromStringByName(const char *dvarName, const char *value);
extern const char *GetBspExtension(void);
extern int Com_sprintf(char *dest, size_t size, const char *format, ...);
extern void Com_Printf(const char *fmt, ...);
extern void Com_Error(errorParm_t code, const char *fmt, ...);
extern const char *CG_Argv(int arg);
extern int Cmd_Argc(void);
extern void Cbuf_AddText(const char *text);
extern char *va( const char *format, ... );
extern void I_strncpyz(char *dest, const char *src, int destsize);
extern int I_stricmp(const char *s1, const char *s2);
extern qboolean CL_Popup(const char *menu);
extern void CL_ClosePopup(const char *menuName);
extern char *Com_Parse( const char *( *data_p ) );
extern WeaponDef *BG_GetWeaponDef(int weaponIndex);
extern void CL_SwitchFog(int fogIndex, int serverTime, int transitionTime);
extern void CL_SetFog(int fogIndex, float start, float halfDist, int r, int g, int b, float density);
extern void SND_DeactivateChannelVolumes(int prio, int fadeTime);
extern void SND_SetChannelVolumes(int prio, const float *volumes, int fadeTime);
extern void SND_DeactivateEnvironmentEffects(int prio, int fadeTime);
extern void SND_SetEnvironmentEffects(int priority, const char *roomstring, float drylevel, float wetlevel, int fademsec);
extern void SND_PlayMusicAlias(const snd_alias_t *pAlias, snd_alias_system_t system);
extern void SND_StopMusic(int fadetime);
extern void SND_FadeAllSounds(float volume, int fadetime);
extern int Load_ScriptMenu(const char *name, int flags);
extern MaterialHandle CL_RegisterMaterialNoMip(const char *name, int imageTrack);
extern MaterialHandle CL_RegisterMaterial(const char *name, int flags);
extern struct XModel *CL_RegisterModel(const char *name);
extern EffectTemplate *FX_RegisterEffect(const char *fileName);
extern void CG_InitLocalEntities(void);
extern void CG_InitMarkPolys(void);
extern void FX_FreeActive(void);
extern void SND_StopSounds(snd_stopsounds_arg_t mode);
extern void CG_StartAmbient(void);
extern void Dvar_SetBool(dvar_t *dvar, bool value);
extern void CL_SetADS(int value);
extern void CL_CloseAllMenus(void);
extern void CG_ScoresUp_f(void);
extern void CL_SyncTimes(void);
extern int CL_GetServerCommand(int seq);
extern int stricmp(const char *s1, const char *s2);
extern const char *SEH_LocalizeTextMessage(const char *msg, const char *context, msgLocErrType_t errType);
extern void CG_GameMessage(const char *msg);
extern void CG_BoldGameMessage(const char *msg);
extern void CG_TranslateHudElemMessage(const char *message, const char *messageType, char *hudElemString);
extern Bool CG_PlaySoundOnFirstClient(void);
extern snd_alias_t *CL_PickSoundAlias(const char *aliasname);
extern int CG_PlayClientSoundAliasByName(const char *aliasname);
extern int CL_GetLocalClientActiveCount(void);
extern menuDef_t *Menus_FindByName(displayContextDef_t *dc, const char *name);
extern void Window_AddDynamicFlags(Window *w, const int newFlags);
extern void CG_SetEquippedOffHand(int offHandIndex);
extern void CG_SelectWeaponIndex(int weaponIndex);
extern void CL_ResetPlayerMuting(int clientIndex);
extern void CL_CloseInGameMenu(void);
extern void CG_SetupWeaponDef(void);
extern void CG_RegisterItems(void);
extern void CG_NorthDirectionChanged(void);
extern qboolean CG_LoadShellShockDvars(const char *name);
extern void CG_SetShellShockParmsFromDvars(shellshock_parms_t *parms);
extern const char *UI_GetMapDisplayNameFromPartialLoadNameMatch(const char *pszMap, int *mapLoadNameLen);

#ifdef __cplusplus
extern "C" {
#endif
extern LegacyHacks *legacyHacks;
#ifdef __cplusplus
}
#endif

#define CGUI_PTR ((char *)legacyHacks)

void CG_ParseServerinfo(void);
void CG_ParseCodinfo(void);
static void __attribute_regparm__(2) CG_SetClientDvarFromServer(const char *dvarname, const char *value);
static void CG_OpenScriptMenu(void);
void CG_CheckOpenWaitingScriptMenu(void);
void CG_CloseScriptMenu(void);
void CG_MenuShowNotify(int menuToShow);
static void __attribute_regparm__(2) CG_SetClientDvarFromServer(const char *dvarname, const char *value);
void CG_ParseFog(void);
static void CG_DeactivateChannelVolCmd(void);
static void CG_SetChannelVolCmd(void);
static void CG_DeactivateReverbCmd(void);
void CG_SetConfigValues(void);
void CG_MapRestart(qboolean savepersist);
void CG_ServerCommand(void);
void CG_ExecuteNewServerCommands(int latestSequence);

void CG_ParseServerinfo(void);
void CG_ParseCodinfo(void);
static void __attribute_regparm__(1) CG_AddToTeamChat(const char *str);
static void CG_OpenScriptMenu(void);
void CG_CheckOpenWaitingScriptMenu(void);
void CG_CloseScriptMenu(void);
void CG_MenuShowNotify(int menuToShow);
static void __attribute_regparm__(2) CG_SetClientDvarFromServer(const char *dvarname, const char *value);
void CG_ParseFog(void);
static void CG_DeactivateChannelVolCmd(void);
static void CG_SetChannelVolCmd(void);
static void CG_DeactivateReverbCmd(void);
void CG_SetConfigValues(void);
void CG_MapRestart(qboolean savepersist);
void CG_ServerCommand(void);
void CG_ExecuteNewServerCommands(int latestSequence);

#define I_IsColorString(p) ((p) && *(p) == '^' && *((p) + 1) && *((p) + 1) != '^' && *((p) + 1) >= '0' && *((p) + 1) <= '9')

static qboolean CG_DvarCurrentBool(void *importPtr)
{
    const dvar_t *dvar = importPtr ? *(const dvar_t **)importPtr : NULL;
    return dvar && dvar->current.enabled;
}

static void CG_ParseScores(void)
{
    int i;
    int icon;
    int team;

    cg->numScores = atoi(CG_Argv(1));
    if (cg->numScores > 64) {
        cg->numScores = 64;
    }

    memset(cg->teamScores, 0, sizeof(cg->teamScores));
    cg->teamScores[1] = atoi(CG_Argv(2));
    cg->teamScores[2] = atoi(CG_Argv(3));

    memset(cg->scores, 0, sizeof(cg->scores));
    memset(cg->teamPings, 0, sizeof(cg->teamPings));
    memset(cg->teamPlayers, 0, sizeof(cg->teamPlayers));

    for (i = 0; i < cg->numScores; i++) {
        cg->scores[i].client = atoi(CG_Argv(i * 5 + 4));
        cg->scores[i].score = atoi(CG_Argv(i * 5 + 5));
        cg->scores[i].ping = atoi(CG_Argv(i * 5 + 6));
        cg->scores[i].deaths = atoi(CG_Argv(i * 5 + 7));
        icon = atoi(CG_Argv(i * 5 + 8));
        if (icon > 0 && icon <= 8) {
            cg->scores[i].hStatusIcon = CL_RegisterMaterialNoMip(CL_GetConfigString(icon + 0x16), 7);
        }

        if (cg->scores[i].client < 0 || cg->scores[i].client >= 64) {
            cg->scores[i].client = 0;
        }

        cg->bgs.clientinfo[cg->scores[i].client].score = cg->scores[i].score;
        if (cg->bgs.clientinfo[cg->scores[i].client].infoValid) {
            team = cg->bgs.clientinfo[cg->scores[i].client].team;
        } else {
            team = 0;
        }
        cg->scores[i].team = team;

        cg->teamPlayers[cg->scores[i].team]++;
        cg->teamPings[cg->scores[i].team] += cg->scores[i].ping;
    }

    for (i = 0; i < 4; i++) {
        if (cg->teamPlayers[i] >= 1 && cg->teamPings[i] >= 1) {
            cg->teamPings[i] /= cg->teamPlayers[i];
        } else {
            cg->teamPings[i] = 0;
        }
    }
}

void CG_ParseServerinfo(void)
{
    const char *info;
    const char *mapname;

    info = CL_GetConfigString(0);
    strncpy(cgs->szHostName, Info_ValueForKey(info, "sv_hostname"), 0x100);
    strncpy(cgs->gametype, Info_ValueForKey(info, "g_gametype"), 0x20);
    if (!cgs->localServer)
        Dvar_SetStringByName("g_gametype", cgs->gametype);
    cgs->maxclients = atoi(Info_ValueForKey(info, "sv_maxclients"));
    mapname = Info_ValueForKey(info, "mapname");
    Com_sprintf(cgs->mapname, 0x40, "maps/mp/%s.%s", mapname, GetBspExtension());
}

void CG_ParseCodinfo(void)
{
    int i;
    const char *key;

    if (cgs->localServer != 0) {
        return;
    }

    for (i = 0x8e; i < 0xee; i++) {
        key = CL_GetConfigString(i);
        if (*key == '\0') {
            break;
        }
        Dvar_SetFromStringByName(key, CL_GetConfigString(i + 0x60));
    }
}

void CG_ParseFog(void)
{
    const char *info;
    const char *token;
    float halfDist, val3;
    float start;
    int r, g, b;
    int transitionTime;

    info = CL_GetConfigString(12);

    token = Com_Parse(&info);
    start = (float)atof(token);

    token = Com_Parse(&info);
    if (token == NULL || *token == '\0') {

        CL_SwitchFog(0, cg->time, (int)start);
        return;
    }

    halfDist = (float)atof(token);

    val3 = (float)atof(Com_Parse(&info));
    r = I_fround((float)atof(Com_Parse(&info)) * 255.0f);
    g = I_fround((float)atof(Com_Parse(&info)) * 255.0f);
    b = I_fround((float)atof(Com_Parse(&info)) * 255.0f);

    token = Com_Parse(&info);
    transitionTime = atoi(token);

    CL_SetFog(1, start, halfDist, r, g, b, val3);

    CL_SwitchFog(1, cg->time, transitionTime);
}

// retail 0x4ca770, unreferenced; Kisak CG_PrecacheScriptMenu (SUPPORTED)
void CG_PrecacheScriptMenu(int iConfigNum)
{
    const char *configString;

    configString = CL_GetConfigString(iConfigNum);
    if (!*configString)
        return;
    if (!Load_ScriptMenu(configString, 7))
        Com_Error(ERR_DROP, (const char *)"\x15"
                                          "Could not load script menu file '%s'\n",
                  configString);
}

// retail 0x4ca7c0, index in eax
static void CG_RegisterConfigMaterial(int index)
{
    const char *str;

    str = CL_GetConfigString(index);
    if (*str) {
        CL_RegisterMaterialNoMip(str, 7);
    }
}

void CG_SetConfigValues(void)
{
    int i;
    const char *str;

    cgs->teamScores[0] = atoi(CL_GetConfigString(5));
    cgs->teamScores[1] = atoi(CL_GetConfigString(6));
    cgs->levelStartTime = atoi(CL_GetConfigString(13));

    CL_SwitchFog(0, cg->time, 0);

    for (i = 0x4de; i < 0x4fe; i++) {
        str = CL_GetConfigString(i);
        if (*str == '\0') {
            continue;
        }
        if (!Load_ScriptMenu(str, 7)) {
            Com_Error(ERR_DROP, (const char *)"\x15"
                                              "Could not load script menu file '%s'\n",
                      str);
        }
    }

    for (i = 0x17; i < 0x1f; i++) {
        CL_RegisterMaterialNoMip(CL_GetConfigString(i), 7);
    }

    for (i = 0x1f; i < 0x2e; i++) {
        CL_RegisterMaterial(CL_GetConfigString(i), 7);
    }

    for (i = 0x61f; i < 0x69e; i++) {
        str = CL_GetConfigString(i);
        if (*str == '\0') {
            continue;
        }
        CL_RegisterMaterialNoMip(str, 7);
    }
}

static void CG_SetVoteStringFromConfigString(const char *configString)
{
    char converted[256];
    int mapLoadNameLen;
    const char *displayName;
    int mapNameIndex;
    int inPos;
    int outPos;

    outPos = 0;
    for (inPos = 0; inPos < 256; ++inPos) {
        displayName = UI_GetMapDisplayNameFromPartialLoadNameMatch(configString + inPos, &mapLoadNameLen);
        if (displayName) {
            for (mapNameIndex = 0; inPos < 256 && displayName[mapNameIndex]; ++mapNameIndex)
                converted[outPos++] = displayName[mapNameIndex];
            inPos += mapLoadNameLen;
        }
        converted[outPos++] = configString[inPos];
        if (!configString[inPos])
            break;
    }

    converted[255] = 0;
    I_strncpyz(cgs->voteString, SEH_LocalizeTextMessage(converted, "vote string", (msgLocErrType_t)0), sizeof(cgs->voteString));
}

static void CG_ConfigStringModified(void)
{
    const char *str;
    int num;

    num = atoi(CG_Argv(1));
    str = CL_GetConfigString(num);

    if (num == 7) {
        CG_SetupWeaponDef();
    } else if (num == 8) {
        CG_RegisterItems();
    } else if (num == 3) {
        CG_StartAmbient();
    } else if (num == 0) {
        CG_ParseServerinfo();
    } else if (num >= 0x8e && num < 0x14e) {
        CG_ParseCodinfo();
    } else if (num == 5) {
        cgs->teamScores[0] = atoi(str);
    } else if (num == 6) {
        cgs->teamScores[1] = atoi(str);
    } else if (num == 13) {
        cgs->levelStartTime = atoi(str);
    } else if (num == 15) {
        cgs->voteTime = atoi(str);
    } else if (num == 17) {
        cgs->voteYes = atoi(str);
    } else if (num == 18) {
        cgs->voteNo = atoi(str);
    } else if (num == 16) {
        CG_SetVoteStringFromConfigString(str);
    } else if (num == 12) {
        CG_ParseFog();
    } else if (num >= 0x14e && num < 0x24e) {
        cgs->gameModels[num - 0x14e] = CL_RegisterModel(str);
    } else if (num >= 0x34e && num < 0x38e) {
        cgs->fxs[num - 0x34e] = FX_RegisterEffect(str);
    } else if (num >= 0x48e && num < 0x49e) {
        if (*str && CG_LoadShellShockDvars(str)) {
            CG_SetShellShockParmsFromDvars(&cgs->shellshockParms[num - 0x48e]);
        }
    } else if (num >= 0x17 && num < 0x1f) {
        CL_RegisterMaterialNoMip(str, 7);
    } else if (num >= 0x1f && num < 0x2e) {
        CL_RegisterMaterial(str, 7);
    } else if (num >= 0x61e && num < 0x69e) {
        CG_RegisterConfigMaterial(num);
    } else if (num == 11) {
        CG_NorthDirectionChanged();
    }
}

static void __attribute_regparm__(1) CG_AddToTeamChat(const char *str)
{
    int len;
    char *p, *ls;
    int lastcolor;
    int chatHeight;

    chatHeight = cg_chatHeight->current.integer;
    if (!chatHeight || cg_chatTime->current.integer <= 0) {
        cgs->teamChatPos = cgs->teamLastChatPos = 0;
        return;
    }

    len = 0;

    p = cgs->teamChatMsgs[cgs->teamChatPos % chatHeight];
    *p = 0;

    lastcolor = '7';

    ls = NULL;
    while (*str) {
        if (len > 0x59) {
            if (ls) {
                str -= (p - ls);
                str++;
                p -= (p - ls);
            }
            *p = 0;

            cgs->teamChatMsgTimes[cgs->teamChatPos % chatHeight] = cg->time;

            cgs->teamChatPos++;
            p = cgs->teamChatMsgs[cgs->teamChatPos % chatHeight];
            *p++ = '^';
            *p++ = lastcolor;
            len = 0;
            ls = NULL;
        }

        if (I_IsColorString(str)) {
            *p++ = *str++;
            lastcolor = *str;
            *p++ = *str++;
            continue;
        }
        if (*str == ' ') {
            ls = p;
        }
        *p++ = *str++;
        len++;
    }
    *p = 0;

    cgs->teamChatMsgTimes[cgs->teamChatPos % chatHeight] = cg->time;
    cgs->teamChatPos++;

    if (cgs->teamChatPos - cgs->teamLastChatPos > chatHeight)
        cgs->teamLastChatPos = cgs->teamChatPos - chatHeight;
}

void CG_MapRestart(qboolean savepersist)
{
    char *cgui;

    if (cg_showmiss->current.integer) {
        Com_Printf((const char *)"CG_MapRestart\n");
    }

    cg->centerPrintTime = 0;
    cg->cursorHintFade = 0;
    cg->lastHealthLerpDelay = 1;

    CG_InitLocalEntities();
    CG_InitMarkPolys();
    FX_FreeActive();

    cgs->voteTime = 0;

    cg->mapRestart = 1;

    SND_StopSounds((snd_stopsounds_arg_t)0);
    CG_StartAmbient();

    cg->latchVictorySound = 0;
    cg->v_dmg_time = 0;

    memset(cg->viewDamage, 0, sizeof(cg->viewDamage));

    memset(cg->fWeapSelectFrac, 0, sizeof(cg->fWeapSelectFrac));
    cg->iWeapSelectLastDrawTime = 0;

    Dvar_SetBool((dvar_t *)cg_thirdPerson, false);

    cgui = CGUI_PTR;
    *(int *)(cgui + 8) = 0;

    CL_SetADS(0);

    if (!savepersist) {
        cgui = CGUI_PTR;
        ((LegacyHacks *)cgui)->ui_scriptMenuAllowResponse  = 0;

        CG_CloseScriptMenu();
        CG_CloseScriptMenu();

        cgui = CGUI_PTR;
        ((LegacyHacks *)cgui)->ui_scriptMenuAllowResponse  = 1;

        CL_CloseAllMenus();
    }

    CG_ScoresUp_f();
    *(unsigned char *)cg->objectiveText = 0;

    CL_SyncTimes();
}

static void CG_OpenScriptMenu(void)
{
    int menuIndex;
    const char *pszMenu;
    Bool noMouseControl;

    noMouseControl = 1;
    menuIndex = atoi(CG_Argv(1));

    if (menuIndex >= 0 && menuIndex < 32) {
        pszMenu = CL_GetConfigString(menuIndex + 0x4de);
        if (!*pszMenu) {
            Com_Printf("Server tried to open a non-loaded script menu index: %i\n", menuIndex);
            Cbuf_AddText(va("cmd mr %i bad\n", menuIndex));
            return;
        }

        if (Cmd_Argc() <= 2 || !CG_Argv(2) || !*CG_Argv(2))
            noMouseControl = 0;

        I_strncpyz(legacyHacks->ui_newScriptMenu, pszMenu, sizeof(legacyHacks->ui_newScriptMenu));
        legacyHacks->ui_newScriptMenuIndex = menuIndex;

        if (!CL_Popup(noMouseControl ? "UIMENU_SCRIPT_POPUP_NO_MOUSE" : "UIMENU_SCRIPT_POPUP")) {
            legacyHacks->ui_newScriptMenu[0] = 0;
            legacyHacks->ui_newScriptMenuIndex = -1;
            if (legacyHacks->ui_waitingScriptMenu[0]) {
                if (!I_stricmp(pszMenu, legacyHacks->ui_waitingScriptMenu))
                    return;
                Cbuf_AddText(va("cmd mr %i noop\n", legacyHacks->ui_waitingScriptMenuIndex));
            }
            I_strncpyz(legacyHacks->ui_waitingScriptMenu, pszMenu, sizeof(legacyHacks->ui_waitingScriptMenu));
            legacyHacks->ui_waitingScriptMenuIndex = menuIndex;
            legacyHacks->ui_waitingScriptMenuNoMouse = noMouseControl;
        }
        return;
    }

    Com_Printf("Server tried to open a bad script menu index: %i\n", menuIndex);
    Cbuf_AddText(va("cmd mr %i bad\n", menuIndex));
}

void CG_CheckOpenWaitingScriptMenu(void)
{
    char *cgui;
    int result;
#if COD2_TRACE
    static int traceCount;
#endif

    cgui = CGUI_PTR;
    if ((*(unsigned char *)&((LegacyHacks *)cgui)->ui_waitingScriptMenu[0]) == '\0') {
        return;
    }

    strcpy(((char *)cgui + offsetof(LegacyHacks, ui_newScriptMenu[0])), ((char *)cgui + offsetof(LegacyHacks, ui_waitingScriptMenu[0])));
    cgui = CGUI_PTR;
    ((LegacyHacks *)cgui)->ui_newScriptMenuIndex = ((LegacyHacks *)cgui)->ui_waitingScriptMenuIndex;

    cgui = CGUI_PTR;
    if (((LegacyHacks *)cgui)->ui_waitingScriptMenuNoMouse) {
        result = CL_Popup((const char *)"UIMENU_SCRIPT_POPUP_NO_MOUSE");
    } else {
        result = CL_Popup((const char *)"UIMENU_SCRIPT_POPUP");
    }
#if COD2_TRACE
    if (traceCount++ < 32) {
        if (getenv("MTRACE"))
            Com_Printf("[menu-trace] waiting popup menu='%s' index=%d noMouse=%u result=%d\n",
                       CGUI_PTR + 0x3e8,
                       *(int *)(CGUI_PTR + 0x4e8),
                       *(unsigned char *)(CGUI_PTR + 0x4ec), result);
    }
#endif

    if (result) {

        *(CGUI_PTR + 0x3e8) = '\0';
        *(int *)(CGUI_PTR + 0x4e8) = -1;
        *(unsigned char *)(CGUI_PTR + 0x4ec) = 0;
    } else {

        *(CGUI_PTR + 0x2e4) = '\0';
        *(int *)(CGUI_PTR + 0x3e4) = -1;
    }
}

void CG_CloseScriptMenu(void)
{
    LegacyHacks *hacks;

    CL_ClosePopup((const char *)"UIMENU_SCRIPT_POPUP");
    CL_ClosePopup((const char *)"UIMENU_SCRIPT_POPUP_NO_MOUSE");

    hacks = legacyHacks;
    hacks->ui_scriptMenu[0] = '\0';
    hacks->ui_scriptMenuIndex = -1;
    hacks->ui_newScriptMenu[0] = '\0';
    hacks->ui_newScriptMenuIndex = -1;
    hacks->ui_waitingScriptMenu[0] = '\0';
    hacks->ui_waitingScriptMenuIndex = -1;
    hacks->ui_waitingScriptMenuNoMouse = 0;
}

void CG_MenuShowNotify(int menuToShow)
{
    void *menu;

    if ((unsigned int)menuToShow > 5) {
        return;
    }

    switch (menuToShow) {
    case 0:
        if (cg->healthFadeTime >= cg->time) {
            return;
        }
        cg->healthFadeTime = cg->time;
        if (CL_GetLocalClientActiveCount() == 1) {
            menu = Menus_FindByName((&cgDC),(const char *)"Health");
        } else {
            menu = Menus_FindByName((&cgDC),(const char *)"Health_mp");
        }
        break;
    case 1:
        CG_MenuShowNotify(4);
        if (cg->ammoFadeTime >= cg->time) {
            return;
        }
        cg->ammoFadeTime = cg->time;
        if (CL_GetLocalClientActiveCount() == 1) {
            menu = Menus_FindByName((&cgDC),(const char *)"weaponinfo");
        } else {
            menu = Menus_FindByName((&cgDC),(const char *)"weaponinfo_mp");
        }
        break;
    case 2:
        if (cg->compassFadeTime >= cg->time) {
            return;
        }
        cg->compassFadeTime = cg->time;
        if (CL_GetLocalClientActiveCount() == 1) {
            menu = Menus_FindByName((&cgDC),(const char *)"Compass");
        } else {
            menu = Menus_FindByName((&cgDC),(const char *)"Compass_mp");
        }
        break;
    case 3:
        if (cg->stanceFadeTime >= cg->time) {
            return;
        }
        cg->stanceFadeTime = cg->time;
        if (CL_GetLocalClientActiveCount() == 1) {
            menu = Menus_FindByName((&cgDC),(const char *)"stance");
        } else {
            menu = Menus_FindByName((&cgDC),(const char *)"stance_mp");
        }
        break;
    case 4:
        if (cg->offhandFadeTime >= cg->time) {
            return;
        }
        cg->offhandFadeTime = cg->time;
        if (CL_GetLocalClientActiveCount() == 1) {
            menu = Menus_FindByName((&cgDC),(const char *)"offhandinfo");
        } else {
            menu = Menus_FindByName((&cgDC),(const char *)"offhandinfo_mp");
        }
        break;
    case 5:
        if (cg->scoreFadeTime >= cg->time) {
            return;
        }
        cg->scoreFadeTime = cg->time;
        menu = Menus_FindByName((&cgDC),(const char *)"objectiveinfo");
        break;
    default:
        return;
    }

    if (menu != NULL) {
        Window_AddDynamicFlags((Window *)menu, 4);
    }
}

// retail 0x4cb1a0, unreferenced
void CG_HudMenuShowAllTimed(void)
{
    menuDef_t *menu;

    if (cg->healthFadeTime < cg->time) {
        cg->healthFadeTime = cg->time;
        menu = Menus_FindByName(&cgDC, "Health");
        if (menu)
            Window_AddDynamicFlags((Window *)menu, 4);
    }
    if (cg->ammoFadeTime < cg->time) {
        cg->ammoFadeTime = cg->time;
        menu = Menus_FindByName(&cgDC, "weaponinfo");
        if (menu)
            Window_AddDynamicFlags((Window *)menu, 4);
    }
    if (cg->compassFadeTime < cg->time) {
        cg->compassFadeTime = cg->time;
        menu = Menus_FindByName(&cgDC, "Compass");
        if (menu)
            Window_AddDynamicFlags((Window *)menu, 4);
    }
    if (cg->stanceFadeTime < cg->time) {
        cg->stanceFadeTime = cg->time;
        menu = Menus_FindByName(&cgDC, "stance");
        if (menu)
            Window_AddDynamicFlags((Window *)menu, 4);
    }
    if (cg->offhandFadeTime < cg->time) {
        cg->offhandFadeTime = cg->time;
        menu = Menus_FindByName(&cgDC, "offhandinfo");
        if (menu)
            Window_AddDynamicFlags((Window *)menu, 4);
    }
    if (cg->scoreFadeTime < cg->time) {
        cg->scoreFadeTime = cg->time;
        menu = Menus_FindByName(&cgDC, "objectiveinfo");
        if (menu)
            Window_AddDynamicFlags((Window *)menu, 4);
    }
}

static void CG_RemoveChatEscapeChar(char *text)
{
    int i;
    int l;

    l = 0;
    for (i = 0; text[i]; i++) {
        if (text[i] == 0x19) {
            continue;
        }
        text[l++] = text[i];
    }
    text[l] = 0;
}

// retail 0x4cb300, unreferenced; name unknown
void CG_SetTeamScore_4cb300(int team, int score)
{
    cgs->teamScores[team] = score;
}

static void CG_ReverbCmd(void)
{
    int prio;
    float dryLevel;
    float wetLevel;
    float fadeSeconds;
    const char *roomstring;
    float fval;
    int fadeTime;

    if (Cmd_Argc() != 6) {
        Com_Printf("ERROR: CG_ReverbCmd called with %i args (should be 6)\n", Cmd_Argc());
        return;
    }

    prio = atoi(CG_Argv(1));
    dryLevel = (float)atof(CG_Argv(3));
    wetLevel = (float)atof(CG_Argv(4));
    fadeSeconds = (float)atof(CG_Argv(5));
    roomstring = CG_Argv(2);
    fval = (float)I_fround(fadeSeconds * 1000.0f);
    fadeTime = (int)((0.0f - fval >= 0.0f) ? 0.0f : fval);

    SND_SetEnvironmentEffects(prio, roomstring, dryLevel, wetLevel, fadeTime);
}

static void CG_DeactivateReverbCmd(void)
{
    int prio;
    int fadeTime;
    float fval;

    if (Cmd_Argc() != 3) {
        Com_Printf((const char *)"ERROR: CG_DeactivateReverbCmd called with %i args (should be 3)\n", Cmd_Argc());
        return;
    }

    prio = atoi(CG_Argv(1));
    fval = (float)I_fround((float)atof(CG_Argv(2)) * 1000.0f);
    fadeTime = (int)((0.0f - fval >= 0.0f) ? 0.0f : fval);

    SND_DeactivateEnvironmentEffects(prio, fadeTime);
}

static void CG_SetChannelVolCmd(void)
{
    int prio;
    int shockIndex;
    int fadeTime;
    float fval;

    if (Cmd_Argc() != 4) {
        Com_Printf((const char *)"ERROR: CG_SetChannelVolCmd called with %i args (should be 4)\n", Cmd_Argc());
        return;
    }

    prio = atoi(CG_Argv(1));
    shockIndex = atoi(CG_Argv(2));
    fval = (float)I_fround((float)atof(CG_Argv(3)) * 1000.0f);
    fadeTime = (int)((0.0f - fval >= 0.0f) ? 0.0f : fval);

    SND_SetChannelVolumes(prio, cgs->shellshockParms[shockIndex].sound.channelvolume, fadeTime);
}

static void CG_DeactivateChannelVolCmd(void)
{
    int prio;
    int fadeTime;
    float fval;

    if (Cmd_Argc() != 3) {
        Com_Printf((const char *)"ERROR: CG_DeactivateChannelVolCmd called with %i args (should be 3)\n", Cmd_Argc());
        return;
    }

    prio = atoi(CG_Argv(1));
    fval = (float)I_fround((float)atof(CG_Argv(2)) * 1000.0f);
    fadeTime = (int)((0.0f - fval >= 0.0f) ? 0.0f : fval);

    SND_DeactivateChannelVolumes(prio, fadeTime);
}

static void CG_LocalSoundCmd(void)
{
    int soundIndex;

    if (Cmd_Argc() != 2) {
        Com_Printf("ERROR: CG_LocalSound called with %i args (should be 2)\n", Cmd_Argc());
        return;
    }

    soundIndex = atoi(CG_Argv(1));
    if (soundIndex > 0 && soundIndex <= 0x100) {
        CG_PlayClientSoundAliasByName(CL_GetConfigString(soundIndex + 0x24e));
        return;
    }

    Com_Printf("ERROR: CG_LocalSound called with index %i (should be in range[1,%i])\n", soundIndex, 0x100);
}

// retail 0x4cb820, unreferenced; name unknown
void CG_SetObjectiveText_4cb820(const char *text)
{
    I_strncpyz(cg->objectiveText, text, sizeof(cg->objectiveText));
}

// retail 0x4cb840, unreferenced; name unknown
void CG_SetDrawHud_4cb840(int drawHud)
{
    cg->drawHud = drawHud;
}

// retail 0x4cb850, unreferenced; name unknown
void CG_SetScriptMainMenu_4cb850(const char *text)
{
    I_strncpyz(cg->scriptMainMenu, text, sizeof(cg->scriptMainMenu));
}

static void __attribute_regparm__(2) CG_SetClientDvarFromServer(const char *dvarname, const char *value)
{
    if (stricmp(dvarname, (const char *)"cg_objectiveText") == 0) {
        I_strncpyz(cg->objectiveText, value, sizeof(cg->objectiveText));
    } else if (stricmp(dvarname, (const char *)"cg_drawHud") == 0) {
        int hud = atoi(value);
        cg->drawHud = hud;
    } else if (stricmp(dvarname, (const char *)"g_scriptMainMenu") == 0) {
        I_strncpyz(cg->scriptMainMenu, value, sizeof(cg->scriptMainMenu));
    } else {
        Dvar_SetFromStringByName(dvarname, value);
    }
}

void CG_ServerCommand(void)
{
    const char *cmd;
    int value;
    int argc;
    int i;
    char text[150];
    char message[256];

    cmd = CG_Argv(0);
    switch (cmd[0]) {
    case 'a':
        CG_SelectWeaponIndex(atoi(CG_Argv(1)));
        return;
    case 'b':
        CG_ParseScores();
        return;
    case 'c':
        CG_TranslateHudElemMessage(CG_Argv(1), "announcement message", message);
        CG_BoldGameMessage(message);
        return;
    case 'd':
        CG_ConfigStringModified();
        return;
    case 'e':
    case 'f':
        CG_TranslateHudElemMessage(CG_Argv(1), "game message", message);
        CG_GameMessage(message);
        return;
    case 'g':
        CG_TranslateHudElemMessage(CG_Argv(1), "bold game message", message);
        CG_BoldGameMessage(message);
        return;
    case 'h':
        if (cg_teamChatsOnly->current.enabled) {
            return;
        }
        I_strncpyz(text, SEH_LocalizeTextMessage(CG_Argv(1), "chat message", (msgLocErrType_t)0), sizeof(text));
        CG_RemoveChatEscapeChar(text);
        CG_AddToTeamChat(text);
        Com_Printf("%s\n", text);
        return;
    case 'i':
        I_strncpyz(text, SEH_LocalizeTextMessage(CG_Argv(1), "team chat message", (msgLocErrType_t)0), sizeof(text));
        CG_RemoveChatEscapeChar(text);
        CG_AddToTeamChat(text);
        Com_Printf("%s\n", text);
        return;
    case 'n':
        CG_MapRestart(1);
        return;
    case 'o':
        SND_PlayMusicAlias(CL_PickSoundAlias(CG_Argv(1)), SASYS_CGAME);
        return;
    case 'p':
        SND_StopMusic(atoi(CG_Argv(1)));
        return;
    case 'q':
        SND_FadeAllSounds((float)atof(CG_Argv(1)), atoi(CG_Argv(2)));
        return;
    case 'r':
        CG_ReverbCmd();
        return;
    case 'D':
        CG_DeactivateReverbCmd();
        return;
    case 'E':
        CG_SetChannelVolCmd();
        return;
    case 'F':
        CG_DeactivateChannelVolCmd();
        return;
    case 's':
        CG_LocalSoundCmd();
        return;
    case 't':
        CG_OpenScriptMenu();
        return;
    case 'u':
        CG_CloseScriptMenu();
        return;
    case 'K':
        CL_CloseInGameMenu();
        return;
    case 'v':
        I_strncpyz(text, CG_Argv(1), sizeof(text));
        CG_SetClientDvarFromServer(text, CG_Argv(2));
        return;
    case 'B':
        CG_MapRestart(0);
        return;
    case 'C':
        value = atoi(CG_Argv(1));
        if (!value || BG_GetWeaponDef(value)->offhandClass) {
            CG_SetEquippedOffHand(value);
        }
        return;
    case 'J':
        CL_ResetPlayerMuting(atoi(CG_Argv(1)));
        return;
    case 'G':
        cgs->teamScores[0] = atoi(CG_Argv(1));
        return;
    case 'H':
        cgs->teamScores[1] = atoi(CG_Argv(1));
        return;
    case 'I':
        CG_MenuShowNotify(atoi(CG_Argv(1)));
        return;
    case 0:
        return;
    default:
        Com_Printf("Unknown client game command: %s\n", CG_Argv(0));
        argc = Cmd_Argc();
        if (argc > 1) {
            Com_Printf("Arguments(%i):", argc - 1);
            for (i = 1; i < argc; ++i) {
                Com_Printf(" %s", CG_Argv(i));
            }
            Com_Printf("\n");
        }
        return;
    }
}

void CG_ExecuteNewServerCommands(int latestSequence)
{

    while (cgs->serverCommandSequence < latestSequence) {
        cgs->serverCommandSequence += 1;
        if (CL_GetServerCommand(cgs->serverCommandSequence)) {
            CG_ServerCommand();
        }
    }
}

