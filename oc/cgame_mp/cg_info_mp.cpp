#include "common_types.h"
#include "imports.h"
#include "headers/PC/cgame_mp/cg_local.h"

#include <ctype.h>
extern const vec4_t colorWhite;
extern Bool g_waitingForServer;

extern void I_strncpyz(char *dest, const char *src, int destsize);
extern char *va( const char *format, ... );
extern void Com_Printf(const char *msg, ...);
extern void SCR_UpdateScreen(void);
extern void CG_CloseScriptMenu(void);
extern qboolean UI_IsFullscreen(void);
extern void UI_KeyEvent(int key, int down);
extern void SND_StopSounds(snd_stopsounds_arg_t flags);
extern const char *CL_GetConfigString(int index);
extern const char *Info_ValueForKey( const char *s, const char *key );
extern const char* Dvar_GetString(const char *dvarName);
extern int Dvar_GetInt(const char *dvarName);
extern void UI_DrawMapLevelshot(void);
extern int Sys_Milliseconds(void);
extern FontHandle UI_GetFontHandle(int fontEnum, float scale);
extern int UI_TextWidth(const char *text, int maxChars, struct Font_s *font, float scale);
extern const char *UI_SafeTranslateString(const char *ref);
extern void UI_DrawText(const char *text, int maxChars, FontHandle font, float x, float y, int horzAlign, int vertAlign, float scale, const vec_t *color, int style);
extern void UI_DrawLoadBar(float x, float y, float w, float h, int horzAlign, int vertAlign, const vec_t *color, MaterialHandle material);
extern void UI_FillRect(float x, float y, float width, float height, int horzAlign, int vertAlign, const vec_t *color);

extern LegacyHacks *legacyHacks;



void CG_LoadingString(const char *s);
void CG_DrawInformation(qboolean serverLoading);

void CG_LoadingString(const char *s)
{

    I_strncpyz(cg->infoScreenText, s, sizeof(cg->infoScreenText));

    if (s && *s)
        Com_Printf(va("LOADING... %s\n", s));

    SCR_UpdateScreen();
}

void CG_DrawInformation(qboolean serverLoading)
{
    char string[1024];
    const char *info;
    const char *mapname;
    const char *text;
    const char *dots;
    FontHandle font;
    char *s;
    float width;
    const char *sa;
    float x;

    if (!serverLoading)
    {
        legacyHacks->cl_serverloadmap[0] = 0;
        legacyHacks->cl_serverloadgametype[0] = 0;
        legacyHacks->cl_serverloadwaiting = 0;
    }
    else if (!legacyHacks->cl_serverloadwaiting)
    {
        legacyHacks->ui_scriptMenuAllowResponse = 0;
        CG_CloseScriptMenu();
        CG_CloseScriptMenu();
        legacyHacks->ui_scriptMenuAllowResponse = 1;
        if (!UI_IsFullscreen())
        {
            UI_KeyEvent(0x1b, 1);
            UI_KeyEvent(0x1b, 1);
            UI_KeyEvent(0x1b, 1);
        }
        SND_StopSounds(SND_STOP_ALL);
        legacyHacks->cl_serverloadwaiting = 1;
    }

    info = CL_GetConfigString(0);
    if (serverLoading)
    {
        mapname = legacyHacks->cl_serverloadmap;
    }
    else
    {
        mapname = Info_ValueForKey(info, "mapname");
        if (!mapname[0])
            mapname = Dvar_GetString("mapname");
    }
    I_strncpyz(string, mapname, sizeof(string));
    for (s = string; *s; s++)
        *s = tolower(*s);

    UI_DrawMapLevelshot();

    if (Dvar_GetInt("com_expectedhunkusage") > 0)
        return;
    if (!serverLoading || !g_waitingForServer)
        return;

    font = UI_GetFontHandle(0, 0.5f);
    switch ((Sys_Milliseconds() / 750) & 3)
    {
    case 1:
        dots = ".";
        break;
    case 2:
        dots = "..";
        break;
    case 3:
        dots = "...";
        break;
    default:
        dots = "";
        break;
    }

    text = UI_SafeTranslateString("CGAME_WAITINGFORSERVERLOAD");
    width = (float)UI_TextWidth(text, 0, (struct Font_s *)font, 0.5f);
    sa = va("%s%s", text, dots);
    x = (640.0f - width) * 0.5f;
    UI_DrawText(sa, 0x7fffffff, font, x, 439.0f, 0, 0, 0.5f, colorWhite, 3);
}

