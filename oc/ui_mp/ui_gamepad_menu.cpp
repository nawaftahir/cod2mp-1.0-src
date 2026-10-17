#include "common_types.h"
#include "imports.h"
#include "cod2_feature_config.h"

#if COD2_FEATURE_GAMEPAD

extern uiInfo_t *uiInfo;
extern int UI_LoadMenus(const char *name, int imageTrack);
extern void UI_AddMenuList(uiInfo_t *info, int menuList);
extern qboolean Menus_OpenByName(uiInfo_t *info, const char *name);
extern void Cmd_AddCommand(const char *cmd, void (*func)(void));
extern void Cbuf_AddText(const char *text);
extern void Com_Printf(const char *fmt, ...);
extern dvar_t *Dvar_RegisterString(const char *dvarName, const char *value, unsigned short flags);

static int s_gamepadMenuCmdsRegistered;
static int s_gamepadMenuLoaded;
static const dvar_t *cl_gamepadButtonConfig;

static void UI_Gamepad_OpenMenu_f(void)
{
    Com_Printf("Gamepad config (cvar-only):\n"
               "  cl_gamepadEnable 0|1\n"
               "  cl_gamepadLookSensitivity <0.1..50>\n"
               "  cl_gamepadDeadzone <0..0.9>\n"
               "  cl_gamepadInvertY 0|1\n"
               "  cl_gamepadButtonConfig default|southpaw|legacy|tactical\n"
               "  gamepad_defaults   - reset to shipped defaults\n"
               "  gamepad_restart    - re-apply the button-layout preset\n");
}

static void UI_Gamepad_Defaults_f(void)
{
    Cbuf_AddText("seta cl_gamepadEnable 1\n");
    Cbuf_AddText("seta cl_gamepadLookSensitivity 8\n");
    Cbuf_AddText("seta cl_gamepadDeadzone 0.2\n");
    Cbuf_AddText("seta cl_gamepadInvertY 0\n");
    Cbuf_AddText("seta cl_gamepadButtonConfig default\n");
    Com_Printf("Controller settings reset to defaults.\n");
}

static void UI_Gamepad_Restart_f(void)
{
    extern void CL_Gamepad_ApplyButtonConfig(const char *preset);
    const char *preset = cl_gamepadButtonConfig
                             ? cl_gamepadButtonConfig->current.string
                             : "default";
    CL_Gamepad_ApplyButtonConfig(preset);
}

void UI_Gamepad_RegisterMenus(uiInfo_t *info)
{
    (void)info;

    if (!cl_gamepadButtonConfig)
        cl_gamepadButtonConfig = Dvar_RegisterString("cl_gamepadButtonConfig",
                                                     "default", 0x1 );

    if (!s_gamepadMenuCmdsRegistered) {
        Cmd_AddCommand("controls_gamepad", UI_Gamepad_OpenMenu_f);
        Cmd_AddCommand("gamepad_defaults", UI_Gamepad_Defaults_f);
        Cmd_AddCommand("gamepad_restart", UI_Gamepad_Restart_f);
        s_gamepadMenuCmdsRegistered = 1;
    }
}

#else

typedef int ui_gamepad_menu_translation_unit_not_empty;

#endif
