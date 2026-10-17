#include "cod2_feature_config.h"
#include "common_types.h"
#include "imports.h"
#include "headers/PC/cgame_mp/cg_local.h"
#include <stdlib.h>

extern dvar_t *cg_viewsize;

extern int CG_CrosshairPlayer(void);
extern void Cmd_ArgvBuffer(int arg, char *buf, int bufSize);
extern char *va( const char *format, ... );
extern void Cbuf_AddText(const char *text);
extern void Dvar_SetInt(dvar_t *dvar, int value);
extern void Com_Printf(const char *fmt, ...);
extern int CG_ScoreboardDisplayed(void);
extern int Cmd_Argc(void);
extern void CG_SetShellShockParmsFromDvars(shellshock_parms_t *parms);
extern int CG_LoadShellShockDvars(const char *name);
extern qboolean CG_SaveShellShockDvars(const char *name);
extern float floorf(float x);
extern void Cmd_ArgsBuffer(char *buf, int bufSize);
extern int Com_sprintf(char *dest, size_t size, const char *format, ...);
extern void CL_AddReliableCommand(const char *cmd);
extern const char *CG_Argv(int arg);
extern int I_stricmp(const char *s1, const char *s2);
extern void CL_AddCgameCommand(const char *cmdName);
extern qboolean CL_Popup(const char *menu);
extern const char *UI_SafeTranslateString(const char *key);

void CG_TargetCommand_f(void);
static void CG_SizeUp_f(void);
static void CG_SizeDown_f(void);
static void CG_Viewpos_f(void);
void CG_ScoresUp_f(void);
void CG_ScoresDown_f(void);
void CG_PrevWeapon_f(void);
void CG_NextWeapon_f(void);
void CG_WeaponSlot_f(void);
void CG_FxSetTestPosition(void);
void CG_FxTest(void);
void CG_FxRestart(void);
static void CG_ShellShock_f(void);
static void CG_ShellShock_Load_f(void);
static void CG_ShellShock_Save_f(void);
static void CG_TellTarget_f(void);
static void CG_QuickMessage_f(void);
static void CG_VoiceChat_f(void);
static void CG_TeamVoiceChat_f(void);
qboolean CG_ConsoleCommand(void);
qboolean CG_IsConsoleCommandName(const char *cmd);
void CG_InitConsoleCommands(void);

// order and entries from retail 0x55ddb8; "mr" is forwarded to the server
static const consoleCommand_t commandsList[] = {
    { "viewpos", CG_Viewpos_f },
    { "+scores", CG_ScoresDown_f },
    { "-scores", CG_ScoresUp_f },
    { "sizeup", CG_SizeUp_f },
    { "sizedown", CG_SizeDown_f },
    { "weapnext", CG_NextWeapon_f },
    { "weapprev", CG_PrevWeapon_f },
    { "weaponslot", CG_WeaponSlot_f },
    { "tcmd", CG_TargetCommand_f },
    { "fxSetTestPosition", CG_FxSetTestPosition },
    { "fxTest", CG_FxTest },
    { "fxRestart", CG_FxRestart },
    { "cg_shellshock", CG_ShellShock_f },
    { "cg_shellshock_load", CG_ShellShock_Load_f },
    { "cg_shellshock_save", CG_ShellShock_Save_f },
    { "tell_target", CG_TellTarget_f },
    { "mp_QuickMessage", CG_QuickMessage_f },
    { "VoiceChat", CG_VoiceChat_f },
    { "VoiceTeamChat", CG_TeamVoiceChat_f },
    { "mr", NULL },
    { NULL, NULL }
};

void CG_TargetCommand_f(void)
{
    unsigned int targetNum;
    char test[4];

    targetNum = CG_CrosshairPlayer();
    if (targetNum == 0)
        return;

    Cmd_ArgvBuffer(1, test, 4);
    Cbuf_AddText(va("gc %i %i", targetNum, atoi(test)));
}

static void CG_SizeUp_f(void)
{
    dvar_t *dvar = cg_viewsize;
    int val = dvar->current.integer;

    Dvar_SetInt(dvar, val + 10);
}

static void CG_SizeDown_f(void)
{
    dvar_t *dvar = cg_viewsize;
    int val = dvar->current.integer;

    Dvar_SetInt(dvar, val - 10);
}

static void CG_Viewpos_f(void)
{

    Com_Printf("(%i %i %i) : %i\n",
               (int)cg->refdef.vieworg[0],
               (int)cg->refdef.vieworg[1],
               (int)cg->refdef.vieworg[2],
               (int)cg->refdefViewAngles[1]);
}

void CG_ScoresUp_f(void)
{

    if (!CG_ScoreboardDisplayed())
        return;

    cg->showScores = 0;
    cg->scoreFadeTime = cg->time;
}

void CG_ScoresDown_f(void)
{
    if (cg->scoresRequestTime + 2000 < cg->time) {
        cg->scoresRequestTime = cg->time;
        CL_AddReliableCommand("score");

        if (!CG_ScoreboardDisplayed()) {
            cg->numScores = 0;
            cg->scoresTop = 0;
            cg->showScores = 1;
        }
        return;
    }

    cg->showScores = 1;
}

static void CG_ShellShock_f(void)
{
    char arg[256];
    int argc;
    int msec;

    argc = Cmd_Argc();

    switch (argc) {
    case 2:
        break;
    case 3:
        Cmd_ArgvBuffer(2, arg, 256);
        if (!CG_LoadShellShockDvars(arg))
            return;
        break;
    default:
        Com_Printf("USAGE: cg_shellshock <duration> <filename?>\n");
        return;
    }

    Cmd_ArgvBuffer(1, arg, 256);
    msec = I_fround((float)atof(arg) * 1000.0f);

    CG_SetShellShockParmsFromDvars( (shellshock_parms_t *)((byte *)cgs->shellshockParms));

    cg->testShock.time = cg->time;
    cg->testShock.duration = msec;
}

static void CG_ShellShock_Load_f(void)
{
    char name[64];

    if (Cmd_Argc() != 2) {
        Com_Printf("USAGE: cg_shellshock_load <name>\n");
        return;
    }

    Cmd_ArgvBuffer(1, name, 64);
    CG_LoadShellShockDvars(name);
}

static void CG_ShellShock_Save_f(void)
{
    char name[64];

    if (Cmd_Argc() != 2) {
        Com_Printf("USAGE: cg_shellshock_save <name>\n");
        return;
    }

    Cmd_ArgvBuffer(1, name, 64);
    CG_SaveShellShockDvars(name);
}

static void CG_TellTarget_f(void)
{
    unsigned int clientNum;
    char command[128];
    char message[128];

    clientNum = CG_CrosshairPlayer();
    if (clientNum == -1)
        return;

    Cmd_ArgsBuffer(message, 128);
    Com_sprintf(command, 128, "tell %i \"%s\"", clientNum, message);
    CL_AddReliableCommand(command);
}

static void CG_QuickMessage_f(void)
{
    snapshot_t *nextSnap = cg->nextSnap;

    if (nextSnap == NULL)
        return;
    if (!(nextSnap->ps.pm_flags & 0x800000))
        return;

    CL_Popup("UIMENU_WM_QUICKMESSAGE");
}

static void CG_VoiceChat_f(void)
{
    char chatCmd[64];
    snapshot_t *nextSnap;

    if (Cmd_Argc() != 2)
        return;

    nextSnap = cgArray[0].nextSnap;

    if (nextSnap != NULL && nextSnap->ps.pm_type != 5 && !(nextSnap->ps.pm_flags & 0x800000)) {
        Com_Printf("%s\n", UI_SafeTranslateString("CGAME_NOSPECTATORVOICECHAT"));
        return;
    }

    Cmd_ArgvBuffer(1, chatCmd, 64);
    Cbuf_AddText(va("cmd vsay %s\n", chatCmd));
}

static void CG_TeamVoiceChat_f(void)
{
    char chatCmd[64];
    snapshot_t *nextSnap;

    if (Cmd_Argc() != 2)
        return;

    nextSnap = cgArray[0].nextSnap;

    if (nextSnap != NULL && nextSnap->ps.pm_type != 5 && !(nextSnap->ps.pm_flags & 0x800000)) {
        Com_Printf("%s\n", UI_SafeTranslateString("CGAME_NOSPECTATORVOICECHAT"));
        return;
    }

    Cmd_ArgvBuffer(1, chatCmd, 64);
    Cbuf_AddText(va("cmd vsay_team %s\n", chatCmd));
}

qboolean CG_IsConsoleCommandName(const char *cmd)
{
    int i;

    if (!cmd)
        return 0;

    for (i = 0; commandsList[i].cmd; i++) {
        if (!I_stricmp(cmd, commandsList[i].cmd))
            return 1;
    }

    return 0;
}

qboolean CG_ConsoleCommand(void)
{
    const char *cmd;
    int i;

    if (cg->nextSnap == NULL)
        return 0;

    cmd = CG_Argv(0);
    for (i = 0; commandsList[i].cmd; i++) {
        if (!I_stricmp(cmd, commandsList[i].cmd)) {
            if (commandsList[i].function)
                commandsList[i].function();
            return 1;
        }
    }

    return 0;
}

void CG_InitConsoleCommands(void)
{
    int i;

    for (i = 0; commandsList[i].cmd; i++)
        CL_AddCgameCommand(commandsList[i].cmd);

    CL_AddCgameCommand("kill");
    CL_AddCgameCommand("give");
    CL_AddCgameCommand("take");
    CL_AddCgameCommand("god");
    CL_AddCgameCommand("demigod");
    CL_AddCgameCommand("notarget");
    CL_AddCgameCommand("noclip");
    CL_AddCgameCommand("ufo");
    CL_AddCgameCommand("levelshot");
    CL_AddCgameCommand("setviewpos");
    CL_AddCgameCommand("jumptonode");
    CL_AddCgameCommand("stats");
    CL_AddCgameCommand("say");
    CL_AddCgameCommand("say_team");
    CL_AddCgameCommand("tell");
    CL_AddCgameCommand("team");
    CL_AddCgameCommand("follow");
    CL_AddCgameCommand("callvote");
    CL_AddCgameCommand("vote");
    CL_AddCgameCommand("follownext");
    CL_AddCgameCommand("followprev");
    CL_AddCgameCommand("printentities");
    CL_AddCgameCommand("muteplayer");
    CL_AddCgameCommand("unmuteplayer");

#if COD2_FEATURE_RUMBLE

    {
        extern void CG_InitRumble(void);
        CG_InitRumble();
    }
#endif
}

