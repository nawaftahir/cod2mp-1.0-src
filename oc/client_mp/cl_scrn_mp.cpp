#include "common_types.h"
#include "imports.h"
#include "bytematch.h"
#include <stdlib.h>
#include <string.h>
#include <strings.h>
Bool updateScreenCalled = 0;
extern const vec4_t colorBlack;
extern refexport_t re;
extern clientActive_t clients[1];
extern clientConnection_t clientConnections[1];
#define clc clientConnections[0]
extern clientStatic_t cls;
#define stricmp strcasecmp

extern qboolean scr_initialized;
extern int com_errorEntered;
extern int g_cginit_loading;
extern Bool updateScreenCalled;


#define re_ptr_195eca8 ((refexport_t *)((void *)&re))
#define cls_ptr_195ecac (((byte *)&::cls))
#define ptr_195eea4 ((byte *)((void *)&com_errorEntered))
#define ptr_195f58c ((byte *)((void *)&colorBlack))
#define ptr_195f5e0 ((byte *)imp_net_showprofile)

extern dvar_t *net_showprofile;

typedef void (*re_void_func)(void);
typedef void (*re_int_func)(int);
typedef void (*re_int2_func)(int, int);
typedef void (*re_int4_func)(int, int, int, int);
typedef void (*re_floatp_func)(float *);
/* the font parameter is a FontHandle (pointer); the decomp typed it as int,
 * which truncates the 8-byte handle on x64 -> R_TextHeight etc. deref garbage. */
typedef float (*re_font_height_func)(FontHandle, float);
typedef int (*re_font_iheight_func)(FontHandle);
typedef void (*re_draw_string_func)(const char *, int, FontHandle, float, float, float, float, const float *, int);
typedef void (*re_draw_console_func)(const short int *, int, int, float, float, float, float, const float *, int);
typedef void (*re_write_cubemap_func)(const char *, int, float, float);

/* The offsets are x86 byte offsets into refexport_t's function-pointer table
 * (field index * 4). On x64 the pointers are 8 bytes, so the same field lives at
 * field index * 8 == double the x86 byte offset. refexport_t is a pure vtable
 * (all function pointers) so the doubling is exact. */
#if defined(COD2_X64)
#define RE_FUNC(re, offset, type) ((type)(*(void **)((byte *)(re) + (offset) * 2)))
#else
#define RE_FUNC(re, offset, type) ((type)(*(void **)((byte *)(re) + (offset))))
#endif

extern void Com_Printf(const char *fmt, ...);
extern void Com_Error(errorParm_t code, const char *fmt, ...);
extern int Cmd_Argc(void);
extern const char *Cmd_Argv( int arg );
extern char *va( const char *format, ... );
extern qboolean CG_DrawActiveFrame(int serverTime, DemoType demoType, CubemapShot cubemapShot, int cubemapSize, qboolean renderScreen);
extern float UI_GetBlurRadius(void);
extern void UI_Refresh(void);
extern int UI_IsFullscreen(void);
extern uiMenuCommand_t UI_GetActiveMenu(void);
extern void UI_DrawConnectScreen(void);
extern qboolean UI_SetActiveMenu(int menu);
extern void UI_UpdateTime(int time);
extern void CL_ClearScene(void);
extern void CL_ResetSkeletonCache(int flags);
extern void CL_SendCmdInternal(void);
extern void CL_SetCGameTime(void);
extern Bool CL_AnyLocalClientChallenging(void);
extern void CL_DrawLogo(void);
extern void CL_LookupColor(unsigned char c, float *outColor);
extern void Con_DrawConsole(void);
extern void Con_CheckResize(void);
extern void Con_DrawSolidConsole(void);
extern void SND_StopSounds(snd_stopsounds_arg_t flags);
extern void SCR_DrawCinematic(void);
extern Bool Sys_IsMainThread(void);
extern void Sys_LoadingKeepAlive(void);
extern void Net_DisplayProfile(void);
extern int FS_FTell(int fh);
extern void CalcScreenPlacement(float *x, float *y, float *w, float *h, int horzAlign, int vertAlign);
extern void CalcScreenPlacementInl(float *x, float *y, float *w, float *h, int horzAlign, int vertAlign);

void SCR_DrawSmallStringExt(int x, int y, const char *string, const vec_t *setColor);
void SCR_DrawConsoleString(int x, int y, const short int *string, int maxChars, const vec_t *setColor);
void SCR_Init(void);
DemoType CL_GetDemoType(void);
float CL_GetMenuBlurRadius(void);
static void CL_CubemapShotUsage(void);
void CL_CubemapShot_f(void);
static void SCR_UpdateFrame(void);
void SCR_UpdateScreenInternal(void);
void SCR_UpdateScreen(void);

// retail 0x412ff0, unreferenced; Q3 SCR_DrawPic (SUPPORTED)
void SCR_DrawPic(float x, float y, float width, float height, int horzAlign, int vertAlign, MaterialHandle material)
{
    CalcScreenPlacement(&x, &y, &width, &height, horzAlign, vertAlign);
    re.DrawStretchPic(x, y, width, height, 0.0f, 0.0f, 1.0f, 1.0f, NULL, material);
}

// retail 0x413040, unreferenced; Q3 SCR_DrawSmallChar (SUPPORTED)
void SCR_DrawSmallChar(int x, int y, int ch, const vec_t *setColor)
{
    char string[2];
    int fontHeight;
    float fy;

    string[0] = ch;
    string[1] = 0;
    fontHeight = re.TextHeight(cls.consoleFont);
    fy = (float)y + (float)fontHeight;
    re.DrawText(string, 1, cls.consoleFont, (float)x, fy, 1.0f, 1.0f, setColor, 0);
}

void SCR_DrawSmallStringExt(int x, int y, const char *string, const vec_t *setColor)
{
    int fontHeight;
    float fy;

    fontHeight = re.TextHeight(cls.consoleFont);
    fy = (float)y + (float)fontHeight;
    re.DrawText(string, 0x7fffffff, cls.consoleFont, (float)x, fy, 1.0f, 1.0f, setColor, 0);
}

void SCR_DrawConsoleString(int x, int y, const short int *string, int maxChars, const vec_t *setColor)
{
    int fontHeight;
    float fy;

    fontHeight = re.TextHeight(cls.consoleFont);
    fy = (float)y + (float)fontHeight;
    re.DrawConsoleText(string, maxChars, cls.consoleFont, (float)x, fy, 1.0f, 1.0f, setColor, 0);
}

// retail 0x413170
void SCR_DrawDemoRecording(void)
{
    char string[1024];
    float color[4];
    float x;
    float y;
    float w;
    float h;
    float scale;

    if (!clc.demorecording)
        return;

    sprintf(string, "RECORDING %s: %ik", clc.demoName, FS_FTell(clc.demofile) / 1024);
    CL_LookupColor('7', color);
    scale = re.NormalizedTextScale(cls.consoleFont, 0.33333334f);
    x = 5.0f;
    y = 479.0f;
    w = scale;
    h = w;
    CalcScreenPlacementInl(&x, &y, &w, &h, 1, 1);
    re.DrawText(string, 0x7fffffff, cls.consoleFont, x, y, w, h, color, 0);
}

void SCR_Init(void)
{
    scr_initialized = 1;
}

// retail 0x4132a0; Kisak cl_scrn_mp
DemoType CL_GetDemoType(void)
{
    return (DemoType)(clc.demoplaying != 0);
}

// retail 0x4132b0
qboolean CL_CGameRendering(qboolean renderScreen)
{
    DemoType demoType;

    demoType = CL_GetDemoType();
    return CG_DrawActiveFrame(clients[0].serverTime, demoType, CUBEMAPSHOT_NONE, 0, renderScreen);
}

// retail 0x4132e0, unreferenced; same body as Con_DrawConsole, name unknown
void SCR_DrawConsole_4132e0(void)
{
    Con_CheckResize();
    if (clients[0].keyCatchers & 1)
        Con_DrawSolidConsole();
}

// retail 0x413310
void SCR_DrawScreenField(void)
{
    if (!cls.uiStarted) {
        re.ClearScreen(1, colorBlack, 0.0f, 0);
        return;
    }

    if (clc.state != CA_ACTIVE && clc.state != CA_CINEMATIC)
        re.ClearScreen(1, colorBlack, 0.0f, 0);

    UI_UpdateTime(cls.realtime);

    if (!UI_IsFullscreen()) {
        switch (clc.state) {
        case CA_CINEMATIC:
            SCR_DrawCinematic();
            break;
        case CA_LOGO:
            CL_DrawLogo();
            if (clc.state != CA_LOGO)
                return;
            break;
        case CA_DISCONNECTED:
            SND_StopSounds((snd_stopsounds_arg_t)0);
            if (!Sys_IsMainThread()) {
                if (!CL_AnyLocalClientChallenging())
                    break;
            } else {
                UI_SetActiveMenu(1);
                break;
            }
        case CA_CONNECTING:
        case CA_CHALLENGING:
        case CA_CONNECTED:
        case CA_LOADING:
        case CA_PRIMED:
            UI_Refresh();
            UI_DrawConnectScreen();
            break;
        case CA_ACTIVE:
            if (!CL_CGameRendering(1))
                CL_SendCmdInternal();
            SCR_DrawDemoRecording();
            break;
        default:
            Com_Error(ERR_FATAL, "\x15SCR_DrawScreenField: bad clc->state");
            break;
        }
    } else {
        switch (clc.state) {
        case CA_DISCONNECTED:
            break;
        case CA_CINEMATIC:
            break;
        case CA_LOGO:
            break;
        case CA_CONNECTING:
        case CA_CHALLENGING:
        case CA_CONNECTED:
        case CA_LOADING:
        case CA_PRIMED:
            break;
        case CA_ACTIVE:
            if (!CL_CGameRendering(0))
                CL_SendCmdInternal();
            break;
        default:
            Com_Error(ERR_FATAL, "\x15SCR_DrawScreenField: bad clc->state");
            break;
        }
    }

    if ((clients[0].keyCatchers & 8) && clc.state != CA_CINEMATIC)
        UI_Refresh();

    if (net_showprofile->current.integer)
        Net_DisplayProfile();

    re.EndView(0);
}

float CL_GetMenuBlurRadius(void)
{
    if ((clients->keyCatchers & 8) && cls.uiStarted && clc.state != 1)
        return UI_GetBlurRadius();

    return 0.0f;
}

static void SCR_UpdateFrame(void)
{
    re.BeginFrame();
    CL_ClearScene();
    CL_ResetSkeletonCache(0);
    SCR_DrawScreenField();
    re.DoneRenderingViews();
    Con_DrawConsole();
    re.EndFrame();
    Sys_IsMainThread();
}

void SCR_UpdateScreenInternal(void)
{
    if (updateScreenCalled)
        return;

    if (::clc.state == 6)
        Sys_LoadingKeepAlive();

    if (scr_initialized && !com_errorEntered) {
        updateScreenCalled = 1;
        SCR_UpdateFrame();
        updateScreenCalled = 0;
    }
}

void SCR_UpdateScreen(void)
{
    if (updateScreenCalled)
        return;

    if (::clc.state == 6)
        Sys_LoadingKeepAlive();

    if (scr_initialized && !com_errorEntered) {
        updateScreenCalled = 1;
        SCR_UpdateFrame();
        updateScreenCalled = 0;
    }
}

static void CL_CubemapShotUsage(void)
{
    Com_Printf("Syntax: cubemapShot size basefilename [water r0 g0 b0 r90 g90 b90 | fresnel n0 n1]\n");
    Com_Printf("size must be a power of 2 >= 4 and <= 1024\n");
    Com_Printf("screenshots will be written to 'env/basefilename_*.tga'\n");
    Com_Printf("basefilename must not exceed %i chars\n", 40);
    Com_Printf("If 'water' is specified, a diffuse water color cubemap is generated using local lighting.\n");
    Com_Printf("The water has the given colors at the given angles, and blends between them in the middle.\n");
    Com_Printf("If 'fresnel' is specified, the alpha channel of the cubemap contains the reflection factor.\n");
    Com_Printf("n0 and n1 are the index of refraction of the 'air' and 'water' surfaces, respectively.\n");
    Com_Printf("The index of refraction must always be 1 or greater.\n");
    Com_Printf("This is always calculated, and defaults to air-water interface (n0 = 1, n1 = 1.333).\n");
}

void CL_CubemapShot_f(void)
{
    static const char *szShotName[] = { "_up", "_dn", "_lf", "_rt", "_ft", "_bk" };
    char szBaseName[64];
    float rgb[3];
    float n0, n1;
    int size;
    int side;
    byte isLightingShot;

    if (!clients[0].cgameInitialized) {
        Com_Printf("must be in a map to use this command\n");
        return;
    }
    if (Cmd_Argc() < 3 || strlen(Cmd_Argv(2)) > 40) {
        CL_CubemapShotUsage();
        return;
    }
    strcpy(szBaseName, Cmd_Argv(2));
    size = atoi(Cmd_Argv(1));
    if (size < 4 || size > 1024 || (size & (size - 1))) {
        CL_CubemapShotUsage();
        return;
    }

    isLightingShot = 0;
    rgb[0] = 0.0f;
    rgb[1] = 0.0f;
    rgb[2] = 0.0f;
    n0 = 1.0f;
    n1 = 1.333f;
    if (Cmd_Argc() == 7) {
        if (stricmp(Cmd_Argv(3), "lighting")) {
            CL_CubemapShotUsage();
            return;
        }
        rgb[0] = (float)atof(Cmd_Argv(4));
        rgb[1] = (float)atof(Cmd_Argv(5));
        rgb[2] = (float)atof(Cmd_Argv(6));
        isLightingShot = 1;
    } else if (Cmd_Argc() == 6) {
        if (stricmp(Cmd_Argv(3), "fresnel")) {
            CL_CubemapShotUsage();
            return;
        }
        n0 = (float)atof(Cmd_Argv(4));
        n1 = (float)atof(Cmd_Argv(5));
        if (n0 < 1.0f || n1 < 1.0f) {
            CL_CubemapShotUsage();
            return;
        }
    } else if (Cmd_Argc() != 3) {
        CL_CubemapShotUsage();
        return;
    }

    CL_ResetSkeletonCache(0);
    for (side = 1; side < 7; side++) {
        RE_FUNC(re_ptr_195eca8, 0xec, re_int2_func)(size, 1);
        RE_FUNC(re_ptr_195eca8, 0xa8, re_void_func)();
        CL_ClearScene();
        CG_DrawActiveFrame(clients[0].serverTime, CL_GetDemoType(), (CubemapShot)side, size, 0);
        RE_FUNC(re_ptr_195eca8, 0xac, re_void_func)();
        RE_FUNC(re_ptr_195eca8, 0xf0, re_int_func)(side);
    }
    if (isLightingShot)
        RE_FUNC(re_ptr_195eca8, 0xf8, re_floatp_func)(rgb);
    for (side = 1; side < 7; side++)
        RE_FUNC(re_ptr_195eca8, 0xf4, re_write_cubemap_func)(va("env/%s%s.tga", szBaseName, szShotName[side - 1]), side, n0, n1);
}

