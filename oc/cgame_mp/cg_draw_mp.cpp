extern void Com_Printf(const char *fmt, ...);
#include "common_types.h"
#include "imports.h"
#include "bytematch.h"
#include "headers/PC/cgame_mp/cg_local.h"

struct lagometer_t lagometer;
extern weaponInfo_t cg_weaponsArray[];
extern displayContextDef_t cgDC;

extern clientStatic_t cls;
extern const dvar_t *cg_hudDamageIconHeight;
extern const dvar_t *cg_hudDamageIconInScope;
extern const dvar_t *cg_hudDamageIconOffset;
extern const dvar_t *cg_hudDamageIconWidth;
extern const dvar_t *cg_hudGrenadeIconHeight;
extern const dvar_t *cg_hudGrenadeIconInScope;
extern const dvar_t *cg_hudGrenadeIconMaxHeight;
extern const dvar_t *cg_hudGrenadeIconMaxRange;
extern const dvar_t *cg_hudGrenadeIconOffset;
extern const dvar_t *cg_hudGrenadeIconWidth;
extern const dvar_t *cg_hudGrenadePointerHeight;
extern const dvar_t *cg_hudGrenadePointerPivot;
extern const dvar_t *cg_hudGrenadePointerPulseFreq;
extern const dvar_t *cg_hudGrenadePointerPulseMax;
extern const dvar_t *cg_hudGrenadePointerPulseMin;
extern const dvar_t *cg_hudGrenadePointerWidth;
extern const dvar_t *cg_centerPrintY;
extern void CalcScreenX(float *x, int align);
extern void CalcScreenY(float *y, int align);
extern const dvar_t *cg_centertime;
extern const dvar_t *cg_chatHeight;
extern const dvar_t *cg_chatTime;
extern const dvar_t *cg_crosshairAlpha;
extern const dvar_t *cg_crosshairAlphaMin;
extern const dvar_t *cg_crosshairDynamic;
extern const dvar_t *cg_crosshairEnemyColor;
extern const dvar_t *cg_draw2D;
extern const dvar_t *cg_drawCrosshair;
extern const dvar_t *cg_drawCrosshairNames;
extern const dvar_t *cg_drawCrosshairNamesPosX;
extern const dvar_t *cg_drawCrosshairNamesPosY;
extern const dvar_t *cg_drawFPS;
extern const dvar_t *cg_drawGameMessages;
extern const dvar_t *cg_drawGun;
extern const dvar_t *cg_drawLagometer;
extern const dvar_t *cg_drawMaterial;
extern const dvar_t *cg_drawScriptUsage;
extern const dvar_t *cg_drawSnapshot;
extern const dvar_t *cg_drawSoundOverlay;
extern const dvar_t *cg_drawTurretCrosshair;
extern const dvar_t *cg_hudChatPosition;
extern const dvar_t *cg_hudCompassSize;
extern const dvar_t *cg_hudSayPosition;
extern const dvar_t *cg_minicon;
extern const dvar_t *cg_nopredict;
extern const dvar_t *cg_paused;
extern const dvar_t *cg_subtitleCharHeight;
extern const dvar_t *cg_subtitlePosX;
extern const dvar_t *cg_subtitlePosY;
extern const dvar_t *cg_subtitles;
extern const dvar_t *cg_synchronousClients;

extern const dvar_t *hud_fade_ammodisplay;
extern const dvar_t *hud_fade_compass;
extern const dvar_t *hud_fade_healthbar;
extern const dvar_t *hud_fade_offhand;
extern const dvar_t *hud_fade_stance;
extern const dvar_t *hud_health_startpulse_injured;

extern Bool CG_AreHudElemsHidden(void);
extern Bool CG_AreHudMenusHidden(void);
extern void CG_Draw2dHudElems(qboolean foreground);
extern int CG_DrawBigDevString(float x, float y, const char *s, float alpha, int align);
extern qboolean CG_DrawScoreboard(void);
extern void CL_TrackStatistics(trStatistics_t *pStats);
extern void Con_DrawMiniConsole(int xPos, int yPos, float alpha);
extern void Con_DrawNotify(int xPos, int yPos, float alpha, msgwnd_mode_t mode);
extern void Con_DrawSay(int y);
extern void Con_DrawSubtitles(int xPos, int yPos, int charHeight, float alpha, msgwnd_mode_t mode);
extern void Menu_PaintAll(displayContextDef_t *dc);
extern float floorf(float x);
extern int stricmp(const char *s1, const char *s2);

extern struct lagometer_t lagometer;
static int previous;
static struct trStatistics_t rendererStats;
static int fps_previousTimes[32];
static int fps_index;
static vec4_t colorWhiteFaded;
static vec4_t colorGreenFaded;
static vec4_t colorRedFaded;

extern const vec_t colorWhite[4];
extern void UI_DrawHandlePic(float x, float y, float w, float h, int horzAlign, int vertAlign, const vec_t *color, MaterialHandle material);
extern void CG_DrawStringExt(float x, float y, const char *string, const vec_t *setColor, qboolean forceColor, qboolean shadow, float charHeight, qboolean adjust);
extern void CL_LookupColor(unsigned char c, vec_t *color);
extern char *va( const char *format, ... );
extern unsigned int Scr_GetNumScriptVars(void);
extern unsigned int Scr_GetNumScriptThreads(void);
extern int Scr_GetStringUsage(void);
extern int Sys_Milliseconds(void);
extern int CL_GetCurrentCmdNumber(void);
extern int CL_GetKeyCatchers(void);
extern qboolean CL_GetUserCmd(int cmdNumber, usercmd_t *ucmd);
extern Bool CL_IsRenderingSplitScreen(void);
extern void CL_SetUserCmdAimValues(vec_t *kickAngles);
extern void CL_SetUserCmdValue(int weapon, int offHandIndex, float sensitivity);
extern void CL_RenderScene(const refdef_t *fd);
extern qboolean CG_DrawShellShockSavedScreenBlend(const shellshock_parms_t *parms, int start, int duration);
extern void CG_TileClear(void);
extern const char *UI_SafeTranslateString(const char *ref);
extern const char *UI_ReplaceConversionString(const char *sourceString, const char *replaceString);
extern const char *SEH_LocalizeTextMessage(const char *msg, const char *context, msgLocErrType_t errType);
extern FontHandle UI_GetFontHandle(int fontEnum, float scale);
extern int UI_TextWidth(const char *text, int maxChars, FontHandle font, float scale);
extern int UI_TextHeight(FontHandle font, float fontScale);
extern void UI_DrawText(const char *text, int maxChars, FontHandle font, float x, float y, int horzAlign, int vertAlign, float scale, const vec_t *color, int style);
extern MaterialHandle CL_RegisterMaterial(const char *name, int flags);
extern void CG_TranslateHudElemMessage(const char *message, const char *messageType, char *hudElemString);
extern void CG_Trace(trace_t *result, const vec_t *start, const vec_t *mins, const vec_t *maxs, const vec_t *end, int skipNumber, int mask);
extern int BG_GetViewmodelWeaponIndex(const playerState_t *ps);
extern WeaponDef * BG_GetWeaponDef(int iWeapon);
extern int BG_GetNumWeapons(void);
extern void CL_DrawStretchPic(float x, float y, float w, float h, int horzAlign, int vertAlign, float s1, float t1, float s2, float t2, const vec_t *color, MaterialHandle material);
extern void CL_DrawStretchPicPhysical(float x, float y, float w, float h, float s1, float t1, float s2, float t2, const vec_t *color, MaterialHandle material);
extern void CalcScreenPlacement(float *x, float *y, float *w, float *h, int horzAlign, int vertAlign);
extern void CG_DrawRotatedPic(float x, float y, float width, float height, int horzAlign, int vertAlign, float angle, const vec_t *color, MaterialHandle material);
extern qboolean CG_ScoreboardDisplayed(void);
extern void Con_DrawBoldMessages(int xPos, int yPos, float alpha, msgwnd_mode_t mode);
extern float *CG_FadeColor(int startMsec, int totalMsec, int fadeMsec);
extern void Controls_GetConfig(void);
extern int Key_GetCatcher(void);
extern const dvar_t *cg_descriptiveText;
extern int GetKeyBindingLocalizedString(const char *command, char *keys);
extern void I_strncpyz(char *dest, const char *src, int destsize);
extern char *I_CleanStr(char *string);
extern unsigned int SEH_ReadCharFromString(const char **ppsText, qboolean *pbIsTrailingPunctuation);
extern void CG_PlayerSprites(centity_t *cent);
extern qboolean CL_PickMaterial(const vec_t *org, const vec_t *dir, char *pszName, char *pszSurfaceFlags, char *pszContents, int iMaxChars);
extern int CG_DrawSmallDevStringColor(float x, float y, const char *s, const vec_t *color, int align);
extern int CG_DrawBigDevStringColor(float x, float y, const char *s, const vec_t *color, int align);
extern int __mh_execute_header;
extern int SND_GetSoundOverlay(snd_overlay_type_t type, snd_overlay_info_t *info, int maxcount, int *cpu);
extern const char* Dvar_GetString(const char *dvarName);
extern int Dvar_GetInt(const char *dvarName);
extern bool Dvar_GetBool(const char *dvarName);
extern void AngleVectors( const vec3_t angles, vec3_t forward, vec3_t right, vec3_t up );
extern float Vec3Distance(const vec3_t v1, const vec3_t v2);
extern float crandom(void);
extern double sin(double);
extern double tan(double);
extern float GetRealWidthFromVirtualWidth(float width);
extern float GetRealHeightFromVirtualHeight(float height);
extern float GetVirtualWidthFromRealWidth(float width);
extern float GetVirtualHeightFromRealHeight(float height);
extern void BG_GetSpreadForWeapon(const playerState_t *ps, int weaponIndex, float *minSpread, float *maxSpread);
extern float AlignHudElemX(int alignOrg, float x, float width);
extern float AlignHudElemY(int alignOrg, float y, float height);
extern void UI_FillRectPhysical(float x, float y, float width, float height, const vec_t *color);
extern void StatMon_GetStatsArray(const statmonitor_t **array, int *count);
extern void CL_CloseAllMenus(void);
extern void CG_DrawRotatedQuadPic(float x, float y, vec2_t *verts, float angle, const vec_t *color, MaterialHandle material);
extern void CL_DrawQuadPic(vec2_t *verts, const vec_t *color, MaterialHandle material);
extern float vectoyaw( const vec3_t vec );
extern float AngleNormalize360( float angle );
extern float sinf(float);
extern float cosf(float);
extern float fabsf(float);

extern const vec_t colorYellow[4];
extern const vec_t colorRed[4];
extern const vec_t colorGreen[4];
extern const vec_t colorBlue[4];

extern void Menus_CloseByName(displayContextDef_t *dc, const char *p);
extern menuDef_t *Menus_FindByName(displayContextDef_t *dc, const char *name);
extern void Window_AddDynamicFlags(Window *w, const int newFlags);
/* Declared int here, but the real function returns void and takes a Window*. A caller
   does `return Window_RemoveDynamicFlags(..)`, so correcting the signature would change
   that caller's return value -- needs care. */
extern void Window_RemoveDynamicFlags(Window *w, const int newFlags);
extern void CG_MenuShowNotify(int menuToShow);
extern float CG_CalcPlayerHealth(void);
extern Bool CG_CheckPlayerForLowAmmo(void);
extern Bool CG_CheckPlayerForLowClip(void);
extern int CL_GetLocalClientActiveCount(void);

void CG_DrawTeamBackground(float x, float y, float w, float h, float alpha, int team);
static void CG_DrawScriptUsage(void);
void CG_CalculateFPS(void);
void CG_AddLagometerFrameInfo(void);
void CG_AddLagometerSnapshotInfo(snapshot_t *snap);
static void CG_DrawDisconnect(void);
void CG_PriorityCenterPrint(const char *str, float charWidth, int priority);
bool CG_GetWeapReticleZoom(float *pfZoom);
void CG_DrawFrameOverlay(float innerLeft, float innerRight, float innerTop, float innerBottom, const vec_t *color, MaterialHandle material);
static void CG_DrawCrosshairNames(void);
void CG_CheckTimedMenus(void);
static void CG_DrawSoundOverlay(void);
static void CG_DrawMaterial(void);
void CG_ShakeCamera(void);
static qboolean CG_DrawFollow(void);
void CG_DrawPlayerSprites(void);
void CG_DrawActive(void);
static void CG_DrawChatMessages(void);
static void __attribute_regparm__(2) CG_CalcCrosshairPosition(float *x, float *y);
static float CG_DrawFPS(float y);
void CG_DrawBoldGameMessages(void);
void CG_DrawTurretCrossHair(void);
static void CG_DrawSpectatorMessage(void);
void CG_StartShakeCamera(float scale, int duration, const vec_t *src, float radius);
static float CG_DrawWeapReticle(void);
void CG_DrawCrosshair(void);
void CG_Draw2D(void);

bool CG_GetWeapReticleZoom(float *pfZoom);
void CG_DrawTurretCrossHair(void);
static inline int CG_RoundFPS(float value);
static inline void FastSinCos(const float value, float *pSin, float *pCos);

extern int Com_sprintf(char *dest, size_t size, const char *format, ...);

extern const vec3_t vec3_origin;

extern int I_stricmp(const char *s1, const char *s2);



extern void CalcScreenPlacementInl(float *x, float *y, float *w, float *h, int horzAlign, int vertAlign);
/* qboolean (int), not Bool: callers (cg_weapons.c, cg_predict_mp.c) declare it
 * `extern qboolean` and read full eax; a 1-byte Bool return leaves eax's upper
 * bytes dirty under MSVC -> garbage non-zero -> !CG_GetWeapReticleZoom() == 0 ->
 * drawGun never set -> the viewmodel was hidden even when spawned. */


// Original name unknown.
int CG_DrawFieldWidth(int width, int value, int charWidth)
{
    char num[16], *ptr;
    int l;
    int totalWidth;

    totalWidth = 0;
    if (width < 1)
        return 0;

    if (width > 5)
        width = 5;

    switch (width)
    {
    case 1:
        value = value > 9 ? 9 : value;
        value = value < 0 ? 0 : value;
        break;
    case 2:
        value = value > 99 ? 99 : value;
        value = value < -9 ? -9 : value;
        break;
    case 3:
        value = value > 999 ? 999 : value;
        value = value < -99 ? -99 : value;
        break;
    case 4:
        value = value > 9999 ? 9999 : value;
        value = value < -999 ? -999 : value;
        break;
    }

    Com_sprintf(num, sizeof(num), "%i", value);
    l = strlen(num);
    if (l > width)
        l = width;

    ptr = num;
    while (*ptr && l)
    {
        totalWidth += charWidth;
        ptr++;
        l--;
    }

    return totalWidth;
}

void CG_DrawTeamBackground(float x, float y, float w, float h, float alpha, int team)
{
    vec4_t color;

    color[3] = alpha;
    if (team == 1) {
        color[0] = 1.0f;
        color[1] = 0.0f;
        color[2] = 0.0f;
    } else if (team == 2) {
        color[0] = 0.0f;
        color[1] = 0.0f;
        color[2] = 1.0f;
    } else {
        return;
    }

    UI_DrawHandlePic(x, y, w, h, 0, 0, color, cgs->media.teamStatusBar);
}

void CG_DrawScriptUsage(void)
{
    CG_DrawStringExt(480.0f, 80.0f, va("num vars:    %d", Scr_GetNumScriptVars()), colorWhite, 1, 1, 16.0f, 1);
    CG_DrawStringExt(480.0f, 96.0f, va("num threads: %d", Scr_GetNumScriptThreads()), colorWhite, 1, 1, 16.0f, 1);
    CG_DrawStringExt(480.0f, 112.0f, va("string usage: %d", Scr_GetStringUsage()), colorWhite, 1, 1, 16.0f, 1);
}

static float CG_DrawSnapshot(float y)
{
    return y + (float)CG_DrawBigDevStringColor(620.0f, y,
        va("time:%i snap:%i cmd:%i", cg->nextSnap->serverTime, cg->latestSnapshotNum, cgs->serverCommandSequence),
        colorWhite, 6);
}

void CG_CalculateFPS(void)
{
    int now;
    int index;

    int delta;

    now = Sys_Milliseconds();
    delta = now - previous;
    previous = now;
    index = fps_index;
    fps_previousTimes[index % 32] = delta;
    fps_index = index + 1;
}

static float CG_DrawFPS(float y)
{
    const char *s;
    int total;
    int fpsMin;
    int fpsMax;
    int minTime = 0x7fffffff;
    int maxTime = 0;
    float variance;
    float average;
    const float *color;
    int i;

    if (fps_index >= 32) {
        total = 0;

        for (i = 0; i < 32; ++i) {
            total += fps_previousTimes[i];

            if (minTime > fps_previousTimes[i])
                minTime = fps_previousTimes[i];

            if (maxTime < fps_previousTimes[i])
                maxTime = fps_previousTimes[i];
        }

        average = (float)total * (1.0f / 32.0f);

        variance = 0.0f;

        for (i = 0; i < 32; ++i)
            variance += fabsf((float)fps_previousTimes[i] - average);

        variance *= 1.0f / 32.0f;

        if (!total)
            total = 1;

        if (minTime <= 0)
            minTime = 1;

        fpsMin = CG_RoundFPS(1000.0f / (float)maxTime);
        fpsMax = CG_RoundFPS(1000.0f / (float)minTime);

        if (cg_drawFPS->current.integer > 2)
            s = va("%1.2fmspf(%i-%i)", (float)total * (1.0f / 32.0f), minTime, maxTime);
        else {
            int fpsAvg = CG_RoundFPS(32000.0f / (float)total);
            int fpsVar = CG_RoundFPS(variance);

            s = va("%ifps(%i-%i,%i)", fpsAvg, fpsMin, fpsMax, fpsVar);
        }

        y += (float)CG_DrawBigDevStringColor(620.0f, y, s, colorWhiteFaded, 6);

        if (cg_drawFPS->current.integer > 1) {
            if (!stricmp(Dvar_GetString("gfx_driver"), "ogl"))
                return y + (float)CG_DrawSmallDevStringColor(620.0f, y, "use d3d for stats", colorWhiteFaded, 6);

            CL_TrackStatistics(&rendererStats);
            y += (float)CG_DrawSmallDevStringColor(620.0f, y, "scene", colorGreenFaded, 6);

            CG_DrawSmallDevStringColor(620.0f, y, va("view tris          "), colorWhiteFaded, 6);
            y += (float)CG_DrawSmallDevStringColor(620.0f, y, va("%i", rendererStats.c_viewIndexes / 3), colorWhiteFaded, 6);

            if (rendererStats.c_shadowIndexes) {
                CG_DrawSmallDevStringColor(620.0f, y, va("shadow tris          "), colorWhiteFaded, 6);
                y += (float)CG_DrawSmallDevStringColor(620.0f, y, va("%i", rendererStats.c_shadowIndexes / 3), colorWhiteFaded, 6);
            }

            CG_DrawSmallDevStringColor(620.0f, y, va("raw geo tris          "), colorWhiteFaded, 6);
            y += (float)CG_DrawSmallDevStringColor(620.0f, y, va("%i", rendererStats.c_indexes / 3), colorWhiteFaded, 6);

            CG_DrawSmallDevStringColor(620.0f, y, va("raw fx tris          "), colorWhiteFaded, 6);
            y += (float)CG_DrawSmallDevStringColor(620.0f, y, va("%i", rendererStats.c_fxIndexes / 3), colorWhiteFaded, 6);

            CG_DrawSmallDevStringColor(620.0f, y, va("prim          "), colorWhiteFaded, 6);
            y += (float)CG_DrawSmallDevStringColor(620.0f, y, va("%i", rendererStats.c_batches), colorWhiteFaded, 6);

            y += (float)CG_DrawSmallDevStringColor(620.0f, y, "level", colorGreenFaded, 6);

            CG_DrawSmallDevStringColor(620.0f, y, va("tex      "), colorWhiteFaded, 6);
            y += (float)CG_DrawSmallDevStringColor(
                620.0f, y, va("%d", rendererStats.c_imageUsage.total / (1024 * 1024)), colorWhiteFaded, 6);

            if (rendererStats.c_imageUsage.minspec <= 48 * 1024 * 1024) {
                color = colorWhiteFaded;
                s = va("min pc tex      ");
            } else {
                color = colorRedFaded;
                s = va("min pc tex (%g)      ", 48.0);
            }

            CG_DrawSmallDevStringColor(620.0f, y, s, color, 6);
            y += (float)CG_DrawSmallDevStringColor(
                620.0f, y, va("%d", rendererStats.c_imageUsage.minspec / (1024 * 1024)), color, 6);

            CG_DrawSmallDevStringColor(620.0f, y, va("lightmap      "), colorWhiteFaded, 6);
            y += (float)CG_DrawSmallDevStringColor(
                620.0f, y, va("%d", rendererStats.c_imageUsage.lightmap / (1024 * 1024)), colorWhiteFaded, 6);
        }
    }

    return y;
}

static void CG_DrawUpperRight(void)
{
    float y;

    CL_TrackStatistics(0);
    y = 0.0f;
    if (cg_drawSnapshot->current.enabled)
        y = CG_DrawSnapshot(y);
    if (cg_drawFPS->current.integer)
        y = CG_DrawFPS(y);
}

void CG_DrawChatMessages(void)
{
    FontHandle font;
    const char *msg;
    vec4_t color;
    float alphaPercent;
    float y;
    float w;
    int chatHeight;
    int hudChatX;
    int hudChatY;
    int i;

    chatHeight = cg_chatHeight->current.integer;
    if (!chatHeight)
        return;

    hudChatX = (int)cg_hudChatPosition->current.vector[0];
    hudChatY = (int)cg_hudChatPosition->current.vector[1];
    if (cgs->teamLastChatPos == cgs->teamChatPos)
        return;

    if (cg->time - cgs->teamChatMsgTimes[cgs->teamLastChatPos % chatHeight] > cg_chatTime->current.integer)
        cgs->teamLastChatPos++;

    font = UI_GetFontHandle(0, 0.20833333f);
    for (i = cgs->teamChatPos - 1; i >= cgs->teamLastChatPos; i--) {
        alphaPercent = (float)cg_chatTime->current.integer - (float)(cg->time - cgs->teamChatMsgTimes[i % chatHeight]);
        if (alphaPercent > 200.0f) {
            alphaPercent = 1.0f;
        } else {
            alphaPercent *= 0.005f;
            if (alphaPercent <= 0.0f)
                continue;
        }

        msg = cgs->teamChatMsgs[i % chatHeight];
        if (msg && msg[0] == '^' && msg[1] && msg[1] != '^' && msg[1] >= '0' && msg[1] <= '9') {
            CL_LookupColor((unsigned char)msg[1], color);
        } else {
            color[0] = 1.0f;
            color[1] = 1.0f;
            color[2] = 1.0f;
        }
        color[0] *= 0.25f;
        color[1] *= 0.25f;
        color[2] *= 0.25f;
        color[3] = alphaPercent * 0.6f;
        y = (float)(hudChatY + (i - cgs->teamChatPos) * 10);
        w = (float)(UI_TextWidth(msg, 0, font, 0.20833333f) + 24);
        UI_DrawHandlePic(0.0f, y, w, 10.0f, 1, 1, color,
                         cgs->media.teamStatusBar);

        color[0] = 1.0f;
        color[1] = 1.0f;
        color[2] = 1.0f;
        color[3] = alphaPercent;
        UI_DrawText(msg, 0x7fffffff, font, (float)hudChatX, (float)(hudChatY + 9 + (i - cgs->teamChatPos) * 10),
                    1, 1, 0.20833333f, color, 3);
    }
}

void CG_AddLagometerFrameInfo(void)
{
    lagometer.frameSamples[lagometer.frameCount & 0x7f] = cg->time - cg->latestSnapshotTime;
    lagometer.frameCount++;
}

void CG_AddLagometerSnapshotInfo(snapshot_t *snap)
{
    if (!snap) {
        lagometer.snapshotSamples[lagometer.snapshotCount & 0x7f] = -1;
        lagometer.snapshotCount++;
        return;
    }
    lagometer.snapshotSamples[lagometer.snapshotCount & 0x7f] = snap->ping;
    lagometer.snapshotFlags[lagometer.snapshotCount & 0x7f] = snap->snapFlags;
    lagometer.snapshotCount++;
}

void CG_DrawDisconnect(void)
{
    usercmd_t cmd;
    snapshot_t *snap;
    const char *text;
    FontHandle font;
    int textWidth;
    float x;
    vec4_t color = { 1.0f, 1.0f, 1.0f, 1.0f };

    CL_GetUserCmd(CL_GetCurrentCmdNumber() - 127, &cmd);
    snap = cg->nextSnap;
#if COD2_TRACE
    if (getenv("COD2_CNXDIAG")) {
        static int c;
        if ((c++ & 0x3f) == 0)
            Com_Printf("[cnxdiag] cmd.serverTime=%d snap->ps.commandTime=%d cg->time=%d cmdNum=%d\n",
                       cmd.serverTime, snap->ps.commandTime, cg->time, CL_GetCurrentCmdNumber());
    }
#endif
    if (cmd.serverTime <= snap->ps.commandTime || cmd.serverTime > cg->time) {
        return;
    }

    text = UI_SafeTranslateString("CGAME_CONNECTIONINTERUPTED");
    font = UI_GetFontHandle(0, 0.5f);
    textWidth = UI_TextWidth(text, 0, font, 0.5f);
    textWidth = (int)(float)textWidth;
    x = (float)((640 - textWidth) / 2);
    UI_DrawText(text, 0x7fffffff, font, x, 100.0f, 0, 0, 0.5f, color, 3);
    if (!(cg->time & 0x200)) {
        UI_DrawHandlePic(296.0f, 416.0f, 48.0f, 48.0f, 0, 0, 0, CL_RegisterMaterial("net_disconnect", 7));
    }
}

// retail inlines CL_DrawStretchPic here (placement folded) and calls the Physical
static __forceinline void CL_DrawStretchPicInl(float x, float y, float w, float h, int horzAlign, int vertAlign, float s1, float t1, float s2, float t2, const vec_t *color, MaterialHandle material)
{
    CalcScreenPlacementInl(&x, &y, &w, &h, horzAlign, vertAlign);
    CL_DrawStretchPicPhysical(x, y, w, h, s1, t1, s2, t2, color, material);
}

static void CG_DrawLagometer(void)
{
    int a;
    int i;
    float v;
    float ax;
    float ay;
    float aw;
    float ah;
    float mid;
    float range;
    float vscale;
    const float *color;

    if (cg_drawLagometer->current.enabled && !cgs->localServer)
    {
        UI_DrawHandlePic(-55.0f, -140.0f, 48.0f, 48.0f, 3, 3, 0, cgs->media.lagometerMaterial);

        ax = -55.0f;
        ay = -140.0f;
        aw = 48.0f;
        ah = 48.0f;

        range = ah / 3.0f;
        mid = ay + range;

        // draw the frame interpolate / extrapolate graph
        for (a = 0; a < aw; a++)
        {
            i = (lagometer.frameCount - a - 1) & 127;
            v = (float)lagometer.frameSamples[i];
            vscale = range / 300.0f;
            v *= vscale;
            if (v > 0.0f)
            {
                if (v > range)
                    v = range;
                CL_DrawStretchPicInl(ax + aw - a, mid - v, 1.0f, v, 3, 3, 0.0f, 0.0f, 0.0f, 0.0f, colorYellow, cgs->media.whiteMaterial);
            }
            else if (v < 0.0f)
            {
                v = -v;
                if (v > range)
                    v = range;
                CL_DrawStretchPicInl(ax + aw - a, mid, 1.0f, v, 3, 3, 0.0f, 0.0f, 0.0f, 0.0f, colorBlue, cgs->media.whiteMaterial);
            }
        }

        // draw the snapshot latency / drop graph
        range = ah / 2.0f;

        for (a = 0; a < aw; a++)
        {
            i = (lagometer.snapshotCount - a - 1) & 127;
            v = (float)lagometer.snapshotSamples[i];
            if (v > 0.0f)
            {
                if (lagometer.snapshotFlags[i] & 1)
                    color = colorYellow;
                else
                    color = colorGreen;
                vscale = range / 900.0f;
                v = v * vscale;
                if (v > range)
                    v = range;
                CL_DrawStretchPicInl(ax + aw - a, ay + ah - v, 1.0f, v, 3, 3, 0.0f, 0.0f, 0.0f, 0.0f, color, cgs->media.whiteMaterial);
            }
            else if (v < 0.0f)
            {
                CL_DrawStretchPicInl(ax + aw - a, ay + ah - range, 1.0f, range, 3, 3, 0.0f, 0.0f, 0.0f, 0.0f, colorRed, cgs->media.whiteMaterial);
            }
        }

        if (cg_nopredict->current.enabled || cg_synchronousClients->current.enabled)
            CG_DrawBigDevString(ax, ay, "snc", 1.0f, 9);
    }

    CG_DrawDisconnect();
}

void CG_PriorityCenterPrint(const char *str, float charWidth, int priority)
{
    char hudElemString[256];
    const char *s;
    qboolean needNewline = 0;
    int count;
    unsigned int letter;

    if (cgArray[0].centerPrintTime && priority < cgArray[0].centerPrintPriority) {
        return;
    }

    CG_TranslateHudElemMessage(str, "Center Print", hudElemString);
    I_strncpyz(cgArray[0].centerPrint, hudElemString, 0x100);
    cgArray[0].centerPrintPriority = priority;

    s = cgArray[0].centerPrint;
    count = 0;
    while (*s) {
        letter = SEH_ReadCharFromString(&s, 0);
        if (letter == '\n') {
            needNewline = 0;
            count = 0;
            continue;
        }

        ++count;
        if (count >= 0x4b) {
            needNewline = 1;
        }

        if (needNewline && letter == ' ') {
            ((char *)s)[-1] = '\n';
            needNewline = 0;
            count = 0;
        }
    }

    cgArray[0].centerPrintTime = cgArray[0].time + 0x7d0;
    cgArray[0].centerPrintCharWidth = (int)charWidth;
    cgArray[0].centerPrintLines = 1;

    s = cgArray[0].centerPrint;
    while (*s) {
        letter = SEH_ReadCharFromString(&s, 0);
        if (letter == '\n') {
            ++cgArray[0].centerPrintLines;
            continue;
        }

        if (letter == '\\' && *s == 'n') {
            ++cgArray[0].centerPrintLines;
            ++s;
        }
    }
}

// retail 0x4c20c0, unreferenced; Q3 CG_CenterPrint (SUPPORTED)
void CG_CenterPrint(const char *str, float charWidth)
{
    CG_PriorityCenterPrint(str, charWidth, 0);
}

static void CG_DrawCenterString(void)
{
    char *start;
    int l;
    int h;
    float x;
    float lineHeight;
    float y;
    float scale;
    float *color;
    FontHandle font;
    char linebuffer[1024];

    if (!cg->centerPrintTime)
        return;

    color = CG_FadeColor(cg->centerPrintTime, (int)(cg_centertime->current.value * 1000.0f), 100);
    if (!color)
    {
        cg->centerPrintTime = 0;
        cg->centerPrintPriority = 0;
        return;
    }

    start = cg->centerPrint;

    scale = cg->centerPrintCharWidth / 32.0f;
    h = UI_TextHeight(UI_GetFontHandle(0, scale), scale);

    ;
    y = cg_centerPrintY->current.value - cg->centerPrintLines * (h * 1.2f);

    while (1)
    {
        for (l = 0; l < 75; l++)
        {
            if (!start[l] || start[l] == '\n')
                break;
            linebuffer[l] = start[l];
        }
        linebuffer[l] = 0;

        font = UI_GetFontHandle(0, scale);
        x = UI_TextWidth(linebuffer, 0, font, scale); x = x * (-0.5);
        UI_DrawText(linebuffer, 0x7fffffff, font, x, y, 7, 3, scale, color, 3);

        y += (h * 1.2f);

        while (*start && *start != '\n')
            start++;
        if (!*start)
            break;
        start++;
    }
}

// inlined com_math Vec3Dot
static __forceinline float Vec3DotCH(const vec_t *a, const vec_t *b)
{
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}
static void __attribute_regparm__(2) CG_CalcCrosshairPosition(float *x, float *y)
{
    vec3_t gunAng;
    vec3_t gunDir;
    float forward;

    gunAng[0] = cg->gunPitch;
    gunAng[2] = cg->refdefViewAngles[2];
    gunAng[1] = cg->gunYaw;
    AngleVectors(gunAng, gunDir, NULL, NULL);

    forward = Vec3DotCH(cg->refdef.viewaxis[0], gunDir);
    if (forward <= 0.0f || cg->refdef.fov_x <= 0.0f || cg->refdef.fov_y <= 0.0f) {
        *x = 0.0f;
        *y = 0.0f;
        return;
    }

    *x = Vec3DotCH(cg->refdef.viewaxis[1], gunDir) / (tan(cg->refdef.fov_x * 0.0087266462f) * forward) * -320.0f;
    *y = Vec3DotCH(cg->refdef.viewaxis[2], gunDir) / (tan(cg->refdef.fov_y * 0.0087266462f) * forward) * -240.0f;
}

bool CG_GetWeapReticleZoom(float *pfZoom)
{
    WeaponDef *weaponDef;
    float zoom;

    weaponDef = (WeaponDef *)BG_GetWeaponDef(BG_GetViewmodelWeaponIndex(&cg->predictedPlayerState));
    zoom = cg->predictedPlayerState.fWeaponPosFrac;
    *pfZoom = 0.0f;

    if (!weaponDef->szOverlayMaterial[0] && !weaponDef->overlayReticle)
    {
        return 0;
    }

    if (zoom == 0.0f) {
        return 0;
    }

    if (cg->playerEntity.bPositionToADS) {
        *pfZoom = zoom - (1.0f - weaponDef->fAdsZoomInFrac);
        if (*pfZoom > 0.0f) {
            *pfZoom /= weaponDef->fAdsZoomInFrac;
        }
    } else {
        *pfZoom = zoom - (1.0f - weaponDef->fAdsZoomOutFrac);
        if (*pfZoom > 0.0f) {
            *pfZoom /= weaponDef->fAdsZoomOutFrac;
        }
    }

    if (*pfZoom <= 0.01f) {
        return 0;
    }

    if (*pfZoom > 1.0f) {
        *pfZoom = 1.0f;
    }

    return 1;
}

void CG_DrawFrameOverlay(float innerLeft, float innerRight, float innerTop, float innerBottom, const vec_t *color, MaterialHandle material)
{
    float screenWidth;
    float screenHeight;

    screenWidth = (float)cls.vidConfig.width ;
    screenHeight = (float)cls.vidConfig.height ;

    if (innerLeft > 0.0f) {
        CL_DrawStretchPicPhysical(0.0f, 0.0f, innerLeft, screenHeight, 0.0f, 0.0f, 0.0f, 1.0f, color, material);
    }

    if (innerRight < screenWidth) {
        CL_DrawStretchPicPhysical(innerRight, 0.0f, screenWidth - innerRight, screenHeight, 0.0f, 0.0f, 0.0f, 1.0f, color, material);
    }

    if (innerTop > 0.0f) {
        CL_DrawStretchPicPhysical(innerLeft, 0.0f, innerRight - innerLeft, innerTop, 0.0f, 0.0f, 1.0f, 0.0f, color, material);
    }

    if (innerBottom < screenHeight) {
        CL_DrawStretchPicPhysical(innerLeft, innerBottom, innerRight - innerLeft, screenHeight - innerBottom, 0.0f, 0.0f, 1.0f, 0.0f, color, material);
    }
}

static void CG_DrawAdsOverlay(const WeaponDef *weapDef, int weapIndex, const float *color, const float *crosshairPos)
{
    MaterialHandle material;
    vec2_t drawPos;
    vec2_t drawSize;

    material = cg_weaponsArray[weapIndex].hADSOverlay;
    if (!material)
        return;

    drawSize[0] = weapDef->overlayWidth;
    drawSize[1] = weapDef->overlayHeight;
    if (!(drawSize[0] > 320.0f) && !(drawSize[1] > 240.0f)) {
        drawPos[0] = crosshairPos[0] - drawSize[0];
        drawPos[1] = crosshairPos[1] - drawSize[1];
        CL_DrawStretchPic(drawPos[0], drawPos[1], drawSize[0], drawSize[1], 2, 2,
                          0.0f, 0.0f, 1.0f, 1.0f, color, material);

        drawPos[0] = crosshairPos[0];
        drawPos[1] = crosshairPos[1] - drawSize[1];
        CL_DrawStretchPic(drawPos[0], drawPos[1], drawSize[0], drawSize[1], 2, 2,
                          1.0f, 0.0f, 0.0f, 1.0f, color, material);

        drawPos[0] = crosshairPos[0] - drawSize[0];
        drawPos[1] = crosshairPos[1];
        CL_DrawStretchPic(drawPos[0], drawPos[1], drawSize[0], drawSize[1], 2, 2,
                          0.0f, 1.0f, 1.0f, 0.0f, color, material);

        drawPos[0] = crosshairPos[0];
        drawPos[1] = crosshairPos[1];
        CL_DrawStretchPic(drawPos[0], drawPos[1], drawSize[0], drawSize[1], 2, 2,
                          1.0f, 1.0f, 0.0f, 0.0f, color, material);

        drawPos[0] = crosshairPos[0] - drawSize[0];
        drawPos[1] = crosshairPos[1] - drawSize[1];
        CalcScreenPlacement(&drawPos[0], &drawPos[1], &drawSize[0], &drawSize[1], 2, 2);
        CG_DrawFrameOverlay(drawPos[0], drawPos[0] + drawSize[0] * 2.0f,
                            drawPos[1], drawPos[1] + drawSize[1] * 2.0f, color, material);
    } else {
        drawPos[0] = (crosshairPos[0] - drawSize[0] * 0.5f);
        drawPos[1] = crosshairPos[1] - drawSize[1] * 0.5f;
        CL_DrawStretchPic(drawPos[0], drawPos[1], drawSize[0], drawSize[1], 2, 2,
                          0.0f, 0.0f, 1.0f, 1.0f, color, material);

        CalcScreenPlacement(&drawPos[0], &drawPos[1], &drawSize[0], &drawSize[1], 2, 2);
        CG_DrawFrameOverlay(drawPos[0], drawPos[0] + drawSize[0],
                            drawPos[1], drawPos[1] + drawSize[1], color, material);
    }
}

float CG_DrawWeapReticle(void)
{
    int weapIndex;
    WeaponDef *weapDef;
    float zoomFrac;
    vec4_t color;
    vec2_t crosshairPos;

    if (!CG_GetWeapReticleZoom(&zoomFrac))
        return 1.0f;

    weapIndex = BG_GetViewmodelWeaponIndex(&cg->predictedPlayerState);
    weapDef = (WeaponDef *)BG_GetWeaponDef(weapIndex);
    color[0] = 1.0f;
    color[1] = 1.0f;
    color[2] = 1.0f;
    color[3] = zoomFrac;
    CG_CalcCrosshairPosition(&crosshairPos[0], &crosshairPos[1]);
    CG_DrawAdsOverlay(weapDef, weapIndex, color, crosshairPos);
    return 1.0f - zoomFrac;
}

static void CG_CalcCrosshairColor(float alpha, float *color)
{
    if (cg->predictedPlayerState.pm_flags & 0x100000) {
        color[0] = 0.25f;
        color[1] = 1.0f;
        color[2] = 0.25f;
    } else if ((cg->predictedPlayerState.pm_flags & 0x200000) && cg_crosshairEnemyColor->current.enabled) {
        color[0] = 1.0f;
        color[1] = 0.25f;
        color[2] = 0.25f;
    } else {
        color[0] = 1.0f;
        color[1] = 1.0f;
        color[2] = 1.0f;
    }
    color[3] = alpha * cg_crosshairAlpha->current.value;
}

void CG_DrawTurretCrossHair(void)
{
    int weapIndex;
    MaterialHandle material;
    WeaponDef *weapDef;
    vec4_t color;
    float size;
    float x;
    float y;
    float w;
    float h;

    if (!cg_drawTurretCrosshair->current.enabled || cg_paused->current.integer)
        return;

    weapIndex = cg_entities[cg->predictedPlayerState.viewlocked_entNum].nextState.weapon;
    if (!weapIndex)
        return;
    material = cg_weaponsArray[weapIndex].hReticleCenter;
    if (!material)
        return;
    if (cg_crosshairAlpha->current.value < 0.01f)
        return;

    CG_CalcCrosshairColor(1.0f, color);
    weapDef = (WeaponDef *)BG_GetWeaponDef(weapIndex);
    size = (float)weapDef->iReticleCenterSize;
    w = size;
    h = size;
    x = size * -0.5f;
    y = x;
    CalcScreenPlacement(&x, &y, &w, &h, 2, 2);
    CL_DrawStretchPicPhysical(x, y, w, h, 0.0f, 0.0f, 1.0f, 1.0f, color, material);
}

// retail 0x4c2c40, unreferenced; name unknown
Bool CG_CrosshairDisabled_4c2c40(void)
{
    return !cg_drawCrosshair->current.enabled;
}

static bool AllowedToDrawCrosshair(void)
{
    if (cg_paused->current.integer)
        return 0;
    if (!cg_drawCrosshair->current.enabled)
        return 0;
    if (cg->predictedPlayerState.weaponstate == 10 || cg->predictedPlayerState.weaponstate == 11
        || cg->predictedPlayerState.weaponstate == 5 || cg->predictedPlayerState.weaponstate == 1
        || cg->predictedPlayerState.weaponstate == 2)
        return 0;
    return 1;
}

static void CG_DrawAdsAimIndicator(int weapIndex, const WeaponDef *weapDef, const float *color, float centerX, float centerY, float transScale)
{
    MaterialHandle material;
    float w;

    if (!cg_drawGun->current.enabled && !(transScale >= 1.0f)) {
        material = cg_weaponsArray[weapIndex].hReticleCenter;
        if (material) {
            w = 1.5f - transScale; w = w * ((float)weapDef->iReticleCenterSize);
            CL_DrawStretchPic(centerX - w * 0.5f, centerY - w * 0.5f, w, w, 2, 2, 0.0f, 0.0f, 1.0f, 1.0f, color, material);
        }
    }
}

static void CG_TransitionToAds(const WeaponDef *weapDef, float posLerp, float *transScale, float *transShift)
{
    float f;

    if (cg->playerEntity.bPositionToADS) {
        f = posLerp - (1.0f - weapDef->fAdsCrosshairInFrac);
        if (!(f > 0.0f))
            return;
        f = f / weapDef->fAdsCrosshairInFrac;
    } else {
        f = posLerp - (1.0f - weapDef->fAdsCrosshairOutFrac);
        if (!(f > 0.0f))
            return;
        f = f / weapDef->fAdsCrosshairOutFrac;
    }

    if (f > 0.0f) {
        *transScale = 1.0f - f * 0.5f;
        float pitchScale = 480.0f / cg->refdef.fov_y;
        *transShift = pitchScale * weapDef->fAdsAimPitch * f;
    }
}

static void CG_DrawReticleCenter(const WeaponDef *weapDef, int weapIndex, const float *color, float centerX, float centerY, float transScale)
{
    MaterialHandle material;
    float drawSize;

    material = cg_weaponsArray[weapIndex].hReticleCenter;
    if (material) {
        drawSize = (float)weapDef->iReticleCenterSize;
        if (weapDef->weapType == WEAPTYPE_GRENADE && cg->predictedPlayerState.grenadeTimeLeft && weapDef->bCookOffHold)
            drawSize = (float)(cg->predictedPlayerState.grenadeTimeLeft % 1000) * 0.01f + drawSize;
        drawSize = drawSize * transScale;
        CL_DrawStretchPic(centerX - drawSize * 0.5f, centerY - drawSize * 0.5f, drawSize, drawSize, 2, 2,
                          0.0f, 0.0f, 1.0f, 1.0f, color, material);
    }
}

static __declspec(noinline) void CG_CalcReticleSpread(const WeaponDef *weapDef, int weapIndex, const float *drawSize, float transScale, float *spread)
{
    float minSpread;
    float maxSpread;
    float f;
    float scale;

    BG_GetSpreadForWeapon(&cg->predictedPlayerState, weapIndex, &minSpread, &maxSpread);
    f = ((maxSpread - minSpread) * (cg->predictedPlayerState.aimSpreadScale * (1.0f / 255.0f)) + minSpread);
    f = f * transScale;
    scale = tan(f * 0.017453292f) / tan(0.5f * (cg->refdef.fov_y * 0.017453292f)) * 240.0f;
    if (scale < (float)weapDef->iReticleMinOfs)
        scale = (float)weapDef->iReticleMinOfs;
    spread[0] = scale - weapDef->fHipReticleSidePos * drawSize[0];
    spread[1] = scale - weapDef->fHipReticleSidePos * drawSize[1];
}

static __declspec(noinline) void CG_CalcReticleColor(const float *baseColor, float alpha, float *reticleColor)
{
    reticleColor[0] = baseColor[0];
    reticleColor[1] = baseColor[1];
    reticleColor[2] = baseColor[2];
    reticleColor[3] = (1.0f - cg->predictedPlayerState.aimSpreadScale * (1.0f / 255.0f)) * cg_crosshairAlpha->current.value * alpha;
    if (reticleColor[3] < cg_crosshairAlphaMin->current.value)
        reticleColor[3] = cg_crosshairAlphaMin->current.value;
}

static __declspec(noinline) void CG_CalcReticleImageOffset(const float *drawSize, float *imageTexelOffset)
{
    vec2_t realSize;
    vec2_t screenSize;

    realSize[0] = GetRealWidthFromVirtualWidth(drawSize[0]);
    realSize[1] = GetRealHeightFromVirtualHeight(drawSize[1]);
    screenSize[0] = (float)cgs->viewWidth;
    realSize[0] = realSize[0] / screenSize[0];
    screenSize[1] = (float)cgs->viewHeight;
    realSize[1] = realSize[1] / screenSize[1];
    realSize[0] = realSize[0] * 0.125f;
    realSize[1] = realSize[1] * 0.125f;
    realSize[0] = realSize[0] * screenSize[0];
    realSize[1] = realSize[1] * screenSize[1];
    imageTexelOffset[0] = GetVirtualWidthFromRealWidth(realSize[0]);
    imageTexelOffset[1] = GetVirtualHeightFromRealHeight(realSize[1]);
}

static void CG_DrawReticleSides(const WeaponDef *weapDef, int weapIndex, const float *baseColor, float centerX, float centerY, float transScale)
{
    MaterialHandle material;
    vec4_t reticleColor;
    vec2_t drawSize;
    vec2_t drawPos;
    vec2_t spread;
    vec2_t imageTexelOffset;
    vec2_t imageSize;
    float reticleAlpha;

    material = cg_weaponsArray[weapIndex].hReticleSide;
    if (!material)
        return;

    reticleAlpha = CG_DrawWeapReticle();
    CG_CalcReticleColor(baseColor, reticleAlpha, reticleColor);
    drawSize[0] = (float)weapDef->iReticleSideSize * transScale;
    drawSize[1] = drawSize[0];
    imageSize[0] = drawSize[0];
    imageSize[1] = drawSize[0];
    CG_CalcReticleSpread(weapDef, weapIndex, drawSize, transScale, spread);
    CG_CalcReticleImageOffset(imageSize, imageTexelOffset);

    drawPos[0] = centerX - drawSize[0] * 0.5f;
    drawPos[1] = centerY - drawSize[1] - spread[1] - imageTexelOffset[1];
    CG_DrawRotatedPic(drawPos[0], drawPos[1], drawSize[0], drawSize[1], 2, 2, 0.0f, reticleColor, material);

    drawPos[0] = centerX + spread[0];
    drawPos[1] = centerY - drawSize[1] * 0.5f;
    CG_DrawRotatedPic(drawPos[0], drawPos[1], drawSize[0], drawSize[1], 2, 2, 90.0f, reticleColor, material);

    drawPos[0] = centerX - drawSize[0] * 0.5f - imageTexelOffset[0];
    drawPos[1] = centerY + spread[1];
    CG_DrawRotatedPic(drawPos[0], drawPos[1], drawSize[0], drawSize[1], 2, 2, 180.0f, reticleColor, material);

    drawPos[0] = centerX - drawSize[0] - spread[0] - imageTexelOffset[0];
    drawPos[1] = centerY - drawSize[1] * 0.5f - imageTexelOffset[1];
    CG_DrawRotatedPic(drawPos[0], drawPos[1], drawSize[0], drawSize[1], 2, 2, 270.0f, reticleColor, material);
}

void CG_DrawCrosshair(void)
{
    playerState_t *ps = &cg->predictedPlayerState;
    float posLerp;
    bool drawHud;
    float reticleAlpha;
    float centerX;
    float centerY;
    int weapIndex;
    WeaponDef *weapDef;
    vec4_t color;
    float transScale;
    float transShift;

    if (cg->renderingThirdPerson)
        return;

    drawHud = !CG_AreHudMenusHidden();
    posLerp = ps->fWeaponPosFrac;
    transScale = 1.0f;
    transShift = 0.0f;
    if (ps->eFlags & 0x300) {
        if (drawHud && ps->viewlocked_entNum != 0x3ff)
            CG_DrawTurretCrossHair();
        return;
    }

    weapIndex = BG_GetViewmodelWeaponIndex(ps);
    if (!weapIndex)
        return;

    weapDef = (WeaponDef *)BG_GetWeaponDef(weapIndex);
    reticleAlpha = CG_DrawWeapReticle();
    if (!drawHud)
        return;

    CG_CalcCrosshairColor(reticleAlpha, color);
    if (color[3] < 0.01f)
        return;
    if ((posLerp != 1.0f || !cg_drawGun->current.enabled) && AllowedToDrawCrosshair()) {
        CG_CalcCrosshairPosition(&centerX, &centerY);
        if (posLerp != 0.0f) {
            CG_TransitionToAds(weapDef, posLerp, &transScale, &transShift);
            CG_DrawAdsAimIndicator(weapIndex, weapDef, color, centerX, centerY, transScale);
        }
        if (posLerp != 1.0f || cg_drawGun->current.enabled) {
            if (!cg_crosshairDynamic->current.enabled) {
                centerX = 0.0f;
                centerY = transShift;
            }
            CG_DrawReticleCenter(weapDef, weapIndex, color, centerX, centerY, transScale);
            CG_DrawReticleSides(weapDef, weapIndex, color, centerX, centerY, transScale);
        }
    }
}

static void CG_ScanForCrosshairEntity(void)
{
    trace_t trace;
    vec3_t start;
    vec3_t end;

    start[0] = cg->refdef.vieworg[0];
    start[1] = cg->refdef.vieworg[1];
    start[2] = cg->refdef.vieworg[2];
    end[0] = cg->refdef.vieworg[0] + cg->refdef.viewaxis[0][0] * 8192.0f;
    end[1] = cg->refdef.vieworg[1] + cg->refdef.viewaxis[0][1] * 8192.0f;
    end[2] = cg->refdef.vieworg[2] + cg->refdef.viewaxis[0][2] * 8192.0f;
    CG_Trace(&trace, start, vec3_origin, vec3_origin, end, cg->nextSnap->ps.clientNum, 0x2000001);
    if (trace.entityNum < 64) {
        cg->crosshairClientNum = trace.entityNum;
        cg->crosshairClientTime = cg->time;
    }
}

void CG_DrawCrosshairNames(void)
{
    float *color;
    char *name;
    vec4_t textColor;
    float x;
    float y;
    int clientNum;

    if (!cg_drawCrosshairNames->current.enabled)
        return;
    if (cg->renderingThirdPerson)
        return;

    CG_ScanForCrosshairEntity();
    color = CG_FadeColor(cg->crosshairClientTime, 150, 100);
    if (!color)
        return;
    if (cg->crosshairClientNum > 64)
        return;

    clientNum = cg->nextSnap->ps.clientNum;
    if (!cg->bgs.clientinfo[clientNum].infoValid)
        return;
    if (!cg->bgs.clientinfo[clientNum].team)
        return;
    if (!cg->bgs.clientinfo[cg->crosshairClientNum].infoValid)
        return;
    if (cg->bgs.clientinfo[clientNum].team != 3
        && cg->bgs.clientinfo[cg->crosshairClientNum].team != cg->bgs.clientinfo[clientNum].team)
        return;

    name = (char *)va("%s", cg->bgs.clientinfo[cg->crosshairClientNum].name);
    if (!name)
        return;
    if (!name[0])
        return;
    I_CleanStr(name);

    x = (float)cg_drawCrosshairNamesPosX->current.integer;
    y = (float)(cg_drawCrosshairNamesPosY->current.integer + 16);
    if (cg->crosshairClientNum == cg->identifyClientNum) {
        textColor[1] = (float)cg->identifyClientHealth * 0.01f;
        if (textColor[1] > 1.0f)
            textColor[1] = 1.0f;
        else if (textColor[1] < 0.0f)
            textColor[1] = 0.0f;
        textColor[2] = 0.0f;
        if (textColor[1] > 0.5) {
            textColor[0] = (1.0f - textColor[1]) + (1.0f - textColor[1]);
            textColor[1] = 1.0f;
        } else {
            textColor[1] = textColor[1] + textColor[1];
            textColor[0] = 1.0f;
        }
    } else {
        textColor[0] = 1.0f;
        textColor[2] = 1.0f;
        textColor[1] = 1.0f;
    }
    textColor[3] = color[3] * 0.6f;
    UI_DrawText(name, 0x7fffffff, UI_GetFontHandle(0, 1.0f / 3.0f), x, y, 0, 0, 1.0f / 3.0f, textColor, 3);
}

static void CG_DrawFlashDamage(void)
{
    float redFlash;
    vec4_t col;

    if (!cg->nextSnap)
        return;

    if (cg->v_dmg_time > cg->time)
    {
        redFlash = fabsf((float)(cg->v_dmg_time - cg->time) * cg->v_dmg_pitch * 0.002f);
        if (redFlash > 5.0f)
            redFlash = 5.0f;

        col[0] = 0.2f;
        col[1] = 0.0f;
        col[2] = 0.0f;
        col[3] = 0.7f * (redFlash / 5.0f);

        UI_FillRectPhysical(-10.0f, -10.0f, (float)cls.vidConfig.width + 10.0f, (float)cls.vidConfig.height + 10.0f, col);
    }
}

static void CG_DrawDamageDirectionIndicators(void)
{
    float centerY;
    float centerX;
    float halfWidth;
    float height;
    float radius;
    float xy[4][2];
    float color[4];
    int slot;
    int maxTime;
    float angle;
    int t;
    float alpha;

    if (!cg->nextSnap)
        return;

    if (CG_GetWeapReticleZoom(&angle))
    {
        if (!cg_hudDamageIconInScope->current.enabled)
            return;
        CG_CalcCrosshairPosition(&centerX, &centerY);
        centerX = centerX + 320.0f;
        centerY = centerY + 240.0f;
    }
    else
    {
        centerX = 320.0f;
        centerY = 240.0f;
    }

    halfWidth = cg_hudDamageIconWidth->current.value * 0.5f;
    height = cg_hudDamageIconHeight->current.value;
    ;
    xy[0][0] = halfWidth;
    xy[0][1] = height + cg_hudDamageIconOffset->current.value;
    xy[1][0] = -halfWidth;
    xy[1][1] = xy[0][1];
    xy[2][0] = xy[1][0];
    xy[2][1] = cg_hudDamageIconOffset->current.value;
    xy[3][0] = halfWidth;
    xy[3][1] = cg_hudDamageIconOffset->current.value;
    color[0] = 1.0f;
    color[1] = 1.0f;
    color[2] = 1.0f;
    color[3] = 1.0f;
    CalcScreenX(&centerX, 0);
    CalcScreenY(&centerY, 0);

    for (slot = 0; slot < 8; slot++)
    {
        maxTime = cg->viewDamage[slot].duration;
        t = cg->time - cg->viewDamage[slot].time;
        if (t > 0 && t < maxTime)
        {
            angle = vectoyaw(cg->refdef.viewaxis[0]) - cg->viewDamage[slot].yaw;
            alpha = 2.0f - ((float)t + (float)t) / maxTime;
            color[3] = alpha - 1.0f >= 0.0f ? 1.0f : alpha;
            CG_DrawRotatedQuadPic(centerX, centerY, xy, angle, color, cgs->media.damageMaterial);
        }
    }
}

static void CG_DrawGrenadePointer(float centerX, float centerY, const float *grenadeOffset, const float *color)
{
    float width;
    float height;
    float radius;
    float pivot[2];
    float yaw;
    float sinYaw;
    float cosYaw;
    float pointerPos[2];
    vec2_t verts[4];

    width = GetRealWidthFromVirtualWidth(cg_hudGrenadePointerWidth->current.value);
    height = GetRealHeightFromVirtualHeight(cg_hudGrenadePointerHeight->current.value);
    radius = GetRealHeightFromVirtualHeight(cg_hudGrenadeIconOffset->current.value);
    pivot[0] = cg_hudGrenadePointerPivot->current.vector[0];
    pivot[1] = cg_hudGrenadePointerPivot->current.vector[1];
    pivot[0] = GetRealWidthFromVirtualWidth(pivot[0]);
    pivot[1] = GetRealHeightFromVirtualHeight(pivot[1]);

    yaw = AngleNormalize360(vectoyaw(grenadeOffset) - cg->predictedPlayerState.viewangles[1]);
    FastSinCos(yaw * 0.017453292f, &sinYaw, &cosYaw);

    pointerPos[0] = centerX - sinYaw * radius;
    pointerPos[1] = centerY - cosYaw * radius;

    verts[0][0] = verts[3][0] = -pivot[0];
    verts[1][0] = verts[2][0] = width - pivot[0];
    verts[0][1] = verts[1][1] = -pivot[1];
    verts[2][1] = verts[3][1] = height - pivot[1];

    CG_DrawRotatedQuadPic(pointerPos[0], pointerPos[1], verts, -yaw, color, cgs->media.grenadePointer);
}

static void CG_DrawGrenadeIcon(float centerX, float centerY, const float *grenadeOffset, const float *color)
{
    float width;
    float height;
    float radius;
    float yaw;
    float sinYaw;
    float cosYaw;
    vec2_t verts[4];

    width = GetRealWidthFromVirtualWidth(cg_hudGrenadeIconWidth->current.value);
    height = GetRealHeightFromVirtualHeight(cg_hudGrenadeIconHeight->current.value);
    radius = GetRealHeightFromVirtualHeight(cg_hudGrenadeIconOffset->current.value);

    yaw = AngleNormalize360(vectoyaw(grenadeOffset) - cg->predictedPlayerState.viewangles[1]);
    FastSinCos(yaw * 0.017453292f, &sinYaw, &cosYaw);

    verts[0][0] = verts[3][0] = centerX - width * 0.5f - sinYaw * radius;
    verts[1][0] = verts[2][0] = centerX + width * 0.5f - sinYaw * radius;
    verts[0][1] = verts[1][1] = centerY - height * 0.5f - cosYaw * radius;
    verts[2][1] = verts[3][1] = centerY + height * 0.5f - cosYaw * radius;

    CL_DrawQuadPic(verts, color, cgs->media.grenadeIcon);
}

// Original name unknown.
static bool CG_ShouldDrawGrenadeIndicator(const entityState_t *es)
{
    vec3_t offset;
    float maxRange;

    if (es->eType != 4)
        return false;
    if (es->pos.trDelta[0] * es->pos.trDelta[0] + es->pos.trDelta[1] * es->pos.trDelta[1] + es->pos.trDelta[2] * es->pos.trDelta[2] > 1.0f)
        return false;
    if (BG_GetWeaponDef(es->weapon)->projExplosion)
        return false;

    offset[0] = es->pos.trBase[0] - cg->predictedPlayerState.origin[0];
    offset[1] = es->pos.trBase[1] - cg->predictedPlayerState.origin[1];
    offset[2] = es->pos.trBase[2] - cg->predictedPlayerState.origin[2];
    if (offset[2] < 0.0f)
    {
        if (offset[2] < -cg_hudGrenadeIconMaxHeight->current.value)
            return false;
    }
    else if (offset[2] > cg->predictedPlayerState.viewHeightCurrent + cg_hudGrenadeIconMaxHeight->current.value)
    {
        return false;
    }

    maxRange = cg_hudGrenadeIconMaxRange->current.value;
    if (offset[2] * offset[2] + (offset[0] * offset[0] + offset[1] * offset[1]) > maxRange * maxRange)
        return false;
    return true;
}

static void CG_DrawGrenadeIndicators(void)
{
    float centerX;
    float centerY;
    float zoom;
    int i;
    const entityState_t *es;
    vec3_t grenadeOffset;
    vec4_t color;
    float amplitude;
    float clamped;

    if (!cg->nextSnap)
        return;
    if (cg->predictedPlayerState.pm_type == 4)
        return;
    if (cg->predictedPlayerState.pm_flags & 0x400000)
        return;

    if (CG_GetWeapReticleZoom(&zoom))
    {
        if (!cg_hudGrenadeIconInScope->current.enabled)
            return;
        CG_CalcCrosshairPosition(&centerX, &centerY);
    }
    else
    {
        centerX = 0.0f;
        centerY = 0.0f;
    }
    CalcScreenX(&centerX, 2);
    CalcScreenY(&centerY, 2);

    for (i = 0; i < cg->nextSnap->numEntities; i++)
    {
        es = &cg->nextSnap->entities[i];
        if (!CG_ShouldDrawGrenadeIndicator(es))
            continue;

        grenadeOffset[0] = es->pos.trBase[0] - cg->predictedPlayerState.origin[0];
        grenadeOffset[1] = es->pos.trBase[1] - cg->predictedPlayerState.origin[1];
        grenadeOffset[2] = es->pos.trBase[2] - cg->predictedPlayerState.origin[2];

        color[0] = 1.0f;
        color[1] = 1.0f;
        color[2] = 1.0f;
        color[3] = 1.0f;

        amplitude = (cg_hudGrenadePointerPulseMax->current.value - cg_hudGrenadePointerPulseMin->current.value) * 0.5f;
        color[3] = (float)sin((float)cg->time * cg_hudGrenadePointerPulseFreq->current.value * 0.0062831853f) * amplitude
            + (cg_hudGrenadePointerPulseMin->current.value + amplitude);
        clamped = color[3] - 1.0f >= 0.0f ? 1.0f : color[3];
        color[3] = 0.0f - color[3] >= 0.0f ? 0.0f : clamped;

        CG_DrawGrenadePointer(centerX, centerY, grenadeOffset, color);
        CG_DrawGrenadeIcon(centerX, centerY, grenadeOffset, color);
    }
}

int CG_CheckPlayerMovement(usercmd_t oldCmd, usercmd_t newCmd)
{
    if (memcmp(oldCmd.angles, newCmd.angles, sizeof(oldCmd.angles)))
        return 1;
    return newCmd.forwardmove || newCmd.rightmove;
}

int CG_CheckPlayerStanceChange(int newButtons, int changedButtons)
{
    if (changedButtons & 0x2300)
    {
        CG_MenuShowNotify(3);
        return 1;
    }
    if (newButtons & 0x2300)
        CG_MenuShowNotify(3);
    return 0;
}

int CG_CheckPlayerTryReload(int buttons)
{
    return (buttons & 0x30)
        && !(cg->predictedPlayerState.pm_flags & 4)
        && !(cg->snap->ps.cursorHint & 2)
        && !(cg->predictedPlayerState.eFlags & 0x300);
}

int CG_CheckPlayerFireNonTurret(int buttons)
{
    return (buttons & 1) && !(cg->predictedPlayerState.eFlags & 0x300);
}

int CG_CheckPlayerWeaponUsage(int buttons)
{
    if (!CG_CheckPlayerFireNonTurret(buttons) && !CG_CheckPlayerTryReload(buttons))
        return 0;
    CG_MenuShowNotify(1);
    return 1;
}

int CG_CheckPlayerOffHandUsage(int buttons)
{
    if (buttons & 0x30000)
    {
        CG_MenuShowNotify(4);
        return 1;
    }
    return 0;
}

int CG_CheckPlayerMiscInput(int buttons)
{
    return buttons & 0xffffd4ff;
}

void CG_CheckForPlayerInput(void)
{
    usercmd_t oldCmd;
    usercmd_t newCmd;
    int newCmdIndex;
    int changedButtons;
    int newInput;

    newCmdIndex = CL_GetCurrentCmdNumber();
    if (newCmdIndex > 1)
    {
        CL_GetUserCmd(newCmdIndex - 1, &oldCmd);
        CL_GetUserCmd(newCmdIndex, &newCmd);
        changedButtons = oldCmd.buttons ^ newCmd.buttons;
        newInput = CG_CheckPlayerMovement(oldCmd, newCmd);
        if (CG_CheckPlayerWeaponUsage(newCmd.buttons))
            newInput = 1;
        if (CG_CheckPlayerOffHandUsage(newCmd.buttons))
            newInput = 1;
        if (CG_CheckPlayerStanceChange(newCmd.buttons, changedButtons))
            newInput = 1;
        if (newInput || CG_CheckPlayerMiscInput(changedButtons))
            CG_MenuShowNotify(2);
    }
}

void CG_CheckHudHealthDisplay(void)
{
    menuDef_t *menu;

    if (CG_CalcPlayerHealth() < hud_health_startpulse_injured->current.value)
    {
        CG_MenuShowNotify(0);
    }
    else
    {
        if (hud_fade_healthbar->current.value != 0.0f
            && cg->healthFadeTime
            && (float)(cg->time - cg->healthFadeTime) > hud_fade_healthbar->current.value * 1000.0f)
        {
            menu = Menus_FindByName(&cgDC, "Health");
            if (menu)
                Window_RemoveDynamicFlags((Window *)menu, 4);
            cg->healthFadeTime = 0;
        }
    }
}

void CG_CheckHudAmmoDisplay(void)
{
    menuDef_t *menu;

    if (CG_CheckPlayerForLowAmmo() || CG_CheckPlayerForLowClip())
        CG_MenuShowNotify(1);
    if (hud_fade_ammodisplay->current.value != 0.0f
        && cg->ammoFadeTime
        && (float)(cg->time - cg->ammoFadeTime) > hud_fade_ammodisplay->current.value * 1000.0f)
    {
        menu = Menus_FindByName(&cgDC, "weaponinfo");
        if (menu)
            Window_RemoveDynamicFlags((Window *)menu, 4);
        cg->ammoFadeTime = 0;
    }
}

void CG_CheckHudCompassDisplay(void)
{
    menuDef_t *menu;

    if (hud_fade_compass->current.value != 0.0f
        && cg->compassFadeTime
        && (float)(cg->time - cg->compassFadeTime) > hud_fade_compass->current.value * 1000.0f)
    {
        menu = Menus_FindByName(&cgDC, "Compass");
        if (menu)
            Window_RemoveDynamicFlags((Window *)menu, 4);
        cg->compassFadeTime = 0;
    }
}

void CG_CheckHudStanceDisplay(void)
{
    menuDef_t *menu;

    if ((cg->nextSnap->ps.eFlags & 8) && (cg->nextSnap->ps.eFlags & 0x100)
        || (cg->nextSnap->ps.eFlags & 4) && (cg->nextSnap->ps.eFlags & 0x200))
    {
        CG_MenuShowNotify(3);
    }
    if (hud_fade_stance->current.value != 0.0f
        && cg->stanceFadeTime
        && (float)(cg->time - cg->stanceFadeTime) > hud_fade_stance->current.value * 1000.0f)
    {
        menu = Menus_FindByName(&cgDC, "stance");
        if (menu)
            Window_RemoveDynamicFlags((Window *)menu, 4);
        cg->stanceFadeTime = 0;
    }
}

void CG_CheckHudOffHandDisplay(void)
{
    menuDef_t *menu;

    if (hud_fade_offhand->current.value != 0.0f
        && cg->offhandFadeTime
        && (float)(cg->time - cg->offhandFadeTime) > hud_fade_offhand->current.value * 1000.0f)
    {
        menu = Menus_FindByName(&cgDC, "offhandinfo");
        if (menu)
            Window_RemoveDynamicFlags((Window *)menu, 4);
        cg->offhandFadeTime = 0;
    }
}

void CG_CheckHudObjectiveDisplay(void)
{
    menuDef_t *menu;

    if (CG_ScoreboardDisplayed())
    {
        if (cg->time - cg->scoreFadeTime > 100)
        {
            menu = Menus_FindByName(&cgDC, "objectiveinfo");
            if (menu)
                Window_RemoveDynamicFlags((Window *)menu, 4);
        }
    }
}

void CG_CheckTimedMenus(void)
{
    if (cg->voiceTime && cg->time - cg->voiceTime > 2500)
    {
        Menus_CloseByName(&cgDC, "voiceMenu");
        cg->voiceTime = 0;
    }
    CG_CheckForPlayerInput();
    CG_CheckHudHealthDisplay();
    CG_CheckHudAmmoDisplay();
    CG_CheckHudCompassDisplay();
    CG_CheckHudStanceDisplay();
    CG_CheckHudOffHandDisplay();
    CG_CheckHudObjectiveDisplay();
}

static void CG_ScreenFade(void)
{
    int msec;
    int i;
    float t;
    float invt;
    vec4_t color;

    if (cg->fadeRate == 0.0f)
        return;

    msec = cg->fadeTime - cg->time;
    if (msec <= 0)
    {
        cg->fadeColor1[0] = cg->fadeColor2[0];
        cg->fadeColor1[3] = cg->fadeColor2[3];
        cg->fadeColor1[1] = cg->fadeColor2[1];
        cg->fadeColor1[2] = cg->fadeColor2[2];

        if (cg->fadeColor2[3] == 0.0f)
        {
            cg->fadeRate = 0.0f;
            return;
        }

        UI_FillRectPhysical(0.0f, 0.0f, (float)cls.vidConfig.width, (float)cls.vidConfig.height, cg->fadeColor1);
    }
    else
    {
        t = (float)msec * cg->fadeRate;
        invt = 1.0f - t;

        for (i = 0; i < 4; i++)
            color[i] = cg->fadeColor2[i] * invt + cg->fadeColor1[i] * t;

        if (color[3] != 0.0f)
            UI_FillRectPhysical(0.0f, 0.0f, (float)cls.vidConfig.width, (float)cls.vidConfig.height, color);
    }
}

static void CG_DrawSoundOverlay(void)
{
    snd_overlay_info_t info[64];
    const char *provider;
    const char *line;
    int cpu;
    int count;
    int bits;
    int khz;
    int channels;
    int i;
    float y;

    count = SND_GetSoundOverlay(
        (snd_overlay_type_t)cg_drawSoundOverlay->current.integer,
        info,
        (int)(sizeof(info) / sizeof(info[0])),
        &cpu);
    if (count <= 0)
        return;

    provider = Dvar_GetString("mss_3d_provider");
    bits = Dvar_GetInt("snd_bits");
    khz = Dvar_GetInt("snd_khz");
    channels = Dvar_GetBool("snd_stereo") ? 2 : 1;

    CG_DrawStringExt(
        2.0f,
        82.0f,
        va("CPU: ^3%%%i ^73D provider: ^3%s ^7bits: ^3%i ^7kHz: ^3%i ^7chan: ^3%i", cpu, provider, bits, khz, channels),
        colorWhite,
        0,
        1,
        10.0f,
        0);

    y = 92.0f;
    for (i = 0; i < count; ++i) {
        if (!info[i].pszSampleName) {
            line = va("%2i", i);
        } else {
            line = va(
                "%2i %-50s vol^3%04.2f ^7rvol^3%04.2f ^7dist^3%5i ^7pit^3%04.2f",
                i,
                info[i].pszSampleName,
                info[i].fBaseVolume,
                info[i].fCurVolume,
                info[i].dist,
                info[i].fPitch);
        }

        CG_DrawStringExt(2.0f, y, line, colorWhite, 0, 1, 10.0f, 0);
        y += 10.0f;
    }
}

static void CG_DrawGameMessages(void)
{
    int y;
    float alpha;
    float *color;

    y = I_fround(cgs->compassY - (cg_hudCompassSize->current.value - 1.0f) * cgs->compassHeight + 12.0f);

    if (cg->showScores)
    {
        color = CG_FadeColor(cg->scoreFadeTime, 100, 100);
        if (!color)
            return;
        alpha = color[3];
    }
    else
    {
        alpha = 1.0f;
    }

    Con_DrawNotify(6, y, alpha, MWM_BOTTOMUP);
}

void CG_DrawBoldGameMessages(void)
{
    float x;
    float y;
    float alpha;
    float *color;

    x = 0.0f;
    y = -60.0f;

    if (cg->showScores)
    {
        color = CG_FadeColor(cg->scoreFadeTime, 100, 100);
        if (!color)
            return;
        alpha = color[3];
    }
    else
    {
        alpha = 1.0f;
    }

    Con_DrawBoldMessages(I_fround(x), I_fround(y), alpha, MWM_BOTTOMUP_CENTERED);
}

static void CG_DrawMiniConsole(void)
{
    if (cg_minicon->current.enabled)
        Con_DrawMiniConsole(2, 4, 1.0f);
}

static void CG_DrawSubtitles(void)
{
    if (cg_subtitles->current.enabled)
        Con_DrawSubtitles(cg_subtitlePosX->current.integer, cg_subtitlePosY->current.integer,
                          cg_subtitleCharHeight->current.integer, 1.0f, MWM_BOTTOMUP);
}

static void CG_DrawSay(void)
{
    Con_DrawSay((int)cg_hudSayPosition->current.vector[1] + 24);
}

static void CG_DrawPerformanceWarnings(void)
{
    int now;
    const void *statsArray;
    int statsCount;
    int i;
    float x;
    float y;

    now = Sys_Milliseconds();
    StatMon_GetStatsArray((const statmonitor_t **)&statsArray, &statsCount);

    x = 2.0f;
    y = 200.0f;
    for (i = 0; i < statsCount; i++)
    {
        const int *entry = (const int *)((const char *)statsArray + i * 8);
        if (entry[0] >= now)
            UI_DrawHandlePic(x, y, 32.0f, 32.0f, 1, 0, 0, (MaterialHandle)entry[1]);
        x += 34.0f;
        if (x + 32.0f > 68.0f)
        {
            x = 2.0f;
            y += 34.0f;
        }
    }
}

void CG_DrawMaterial(void)
{
    char szName[64];
    char szSurfaceFlags[0x1000];
    char szContents[0x1000];
    float y;

    if (!CL_PickMaterial((const vec_t *)cg->refdef.vieworg, (const vec_t *)cg->refdef.viewaxis, szName, szSurfaceFlags, szContents, 0x1000)) {
        return;
    }

    y = 240.0f;
    y += (float)CG_DrawSmallDevStringColor(8.0f, y, szName, colorWhite, 5);
    y += (float)CG_DrawSmallDevStringColor(8.0f, y, szSurfaceFlags, colorWhite, 5);
    CG_DrawSmallDevStringColor(8.0f, y, szContents, colorWhite, 5);
}

static void CG_DrawDebugOverlays(void)
{
    if (cg_drawSoundOverlay->current.integer)
        CG_DrawSoundOverlay();
    else if (cg_drawScriptUsage->current.enabled)
        CG_DrawScriptUsage();
    else if (cg_drawMaterial->current.enabled)
        CG_DrawMaterial();
}

static qboolean CG_UpdateCameraShakeStrength(cameraShake_t *shake)
{
    float elapsed;
    float distanceScale;
    float rumbleScale;

    int elapsedMs = cg->time - shake->time;
    if (elapsedMs < 0) {
        return 0;
    }
    elapsed = (float)elapsedMs;
    if (elapsed >= shake->length) {
        return 0;
    }

    distanceScale = 1.0f - Vec3Distance(shake->src, cg->refdef.vieworg) / shake->radius;
    rumbleScale = (1.0f - elapsed / shake->length) * shake->scale;
    if (rumbleScale <= 0.0f) {
        return 0;
    }

    shake->size = distanceScale >= 0.0f ? distanceScale * rumbleScale : distanceScale / rumbleScale;
    shake->rumbleScale = rumbleScale;
    return 1;
}

void CG_StartShakeCamera(float scale, int duration, const vec_t *src, float radius)
{
    cameraShake_t temp;
    cameraShake_t *shake;
    int bestIndex;
    float bestSize;
    int i;

    temp.scale = scale;
    temp.length = (float)duration;
    temp.time = cg->time;
    temp.src[0] = src[0];
    temp.src[1] = src[1];
    temp.src[2] = src[2];
    temp.radius = radius;
    CG_UpdateCameraShakeStrength(&temp);

    for (i = 0; i < 4; ++i) {
        shake = &cg->cameraShake[i];
        if (shake->time > cg->time)
            goto store;
        if ((float)shake->time + shake->length <= (float)cg->time)
            goto store;
    }

    bestIndex = i;
    bestSize = temp.size;
    for (i = 0; i < 4; ++i) {
        if (cg->cameraShake[i].size < bestSize) {
            bestIndex = i;
            bestSize = cg->cameraShake[i].size;
        }
    }
    if (bestIndex == 4)
        return;
    i = bestIndex;

store:
    shake = &cg->cameraShake[i];
    *shake = temp;
}

void CG_ShakeCamera(void)
{
    cameraShake_t *shake;
    float sx;
    float bestSize;
    float bestRumble;
    float angle;
    int i;

    sx = (float)cg->time / 600.0f;
    bestRumble = 0.0f;
    bestSize = 0.0f;

    for (i = 0; i < 4; ++i) {
        shake = &cg->cameraShake[i];
        if (!CG_UpdateCameraShakeStrength(shake)) {
            continue;
        }

        if (shake->size > bestSize) {
            bestSize = shake->size;
            bestRumble = shake->rumbleScale;
        }
    }

    if (cg->rumbleScale > bestSize) {
        bestSize = cg->rumbleScale;
        bestRumble = cg->rumbleScale;
    }

    if (bestSize <= 0.0f) {
        cg->cameraShakePhase = crandom() * 3.14159265358979323846f;
        return;
    }

    if (bestSize > 1.0f) {
        bestSize = 1.0f;
    }
    angle = (float)sin(sx * 25.132741228718345f + cg->cameraShakePhase) * bestSize * bestRumble * 18.0f;
    cg->refdefViewAngles[0] += angle;
    angle = (float)sin(sx * 47.12388980384689f + cg->cameraShakePhase) * bestSize * bestRumble * 16.0f;
    cg->refdefViewAngles[1] += angle;
    angle = (float)sin(sx * 37.69911184307752f + cg->cameraShakePhase) * bestSize * bestRumble * 10.0f;
    cg->refdefViewAngles[2] += angle;
}

static void CG_DrawSpectator(void)
{
    const char *s;
    FontHandle font;
    float x;

    s = UI_SafeTranslateString("CGAME_SPECTATOR");
    font = UI_GetFontHandle(4, 0.5f);
    x = (float)-UI_TextWidth(s, 0, font, 0.5f) * 0.5f;
    UI_DrawText(s, 0x7fffffff, font, x, -27.0f, 7, 3, 0.5f, colorWhite, 3);
}

static void CG_DrawVote(void)
{
    char szVoteYes[256];
    char szVoteNo[256];
    char binding[256];
    FontHandle font;
    int sec;
    const char *s;

    if (!cgs->voteTime)
        return;

    font = UI_GetFontHandle(0, 0.20833333f);

    if (GetKeyBindingLocalizedString("vote yes", binding))
        I_strncpyz(szVoteYes, binding, 256);
    else
        I_strncpyz(szVoteYes, "vote yes", 256);

    if (GetKeyBindingLocalizedString("vote no", binding))
        I_strncpyz(szVoteNo, binding, 256);
    else
        I_strncpyz(szVoteNo, "vote no", 256);

    sec = (cgs->voteTime - cg->time) / 1000;
    if (sec < 0)
        sec = 0;

    if (cg->nextSnap->ps.eFlags & 0x100000)
    {
        s = va("%s(%i):%s", UI_SafeTranslateString("CGAME_VOTE"), sec, cgs->voteString);
        UI_DrawText(s, 0x7fffffff, font, 8.0f, 200.0f, 0, 0, 0.20833333f, colorYellow, 3);
        s = va("%s:%i, %s:%i", UI_SafeTranslateString("CGAME_YES"), cgs->voteYes,
               UI_SafeTranslateString("CGAME_NO"), cgs->voteNo);
        UI_DrawText(s, 0x7fffffff, font, 8.0f, 210.0f, 0, 0, 0.20833333f, colorYellow, 3);
    }
    else
    {
        s = va("%s(%i):%s", UI_SafeTranslateString("CGAME_VOTE"), sec, cgs->voteString);
        UI_DrawText(s, 0x7fffffff, font, 8.0f, 200.0f, 0, 0, 0.20833333f, colorYellow, 3);
        s = va("%s(%s):%i, %s(%s):%i", UI_SafeTranslateString("CGAME_YES"), szVoteYes, cgs->voteYes,
               UI_SafeTranslateString("CGAME_NO"), szVoteNo, cgs->voteNo);
        UI_DrawText(s, 0x7fffffff, font, 8.0f, 210.0f, 0, 0, 0.20833333f, colorYellow, 3);
    }
}

static void CG_DrawIntermission(void)
{
    CL_CloseAllMenus();
    cg->showScores = 1;
    cg->scoreFadeTime = cg->time;
    CG_DrawScoreboard();
}

static const char *CG_GetBoundSpectatorCommand(const char *command, const char *altCommand)
{
    char binding[256];

    if (GetKeyBindingLocalizedString(command, binding))
        return command;
    if (GetKeyBindingLocalizedString(altCommand, binding))
        return altCommand;
    return command;
}

static void CG_DrawSpectatorMessage(void)
{
    const char *commands[5];
    const char *messages[5];
    char binding[256];
    const char *str;
    FontHandle font;
    float lineHeight;
    float y;
    int lineNum;
    int i;

    if (!cg_descriptiveText->current.enabled || (Key_GetCatcher() & 8)
        || !(cg->nextSnap->ps.pm_flags & 0x3000000))
        return;

    Controls_GetConfig();
    font = UI_GetFontHandle(0, 0.20833333f);
    lineNum = 0;
    lineHeight = (float)UI_TextHeight(font, 0.20833333f);
    y = 436.0f - (lineHeight + lineHeight);

    if (cg->nextSnap->ps.pm_flags & 0x1000000) {
        commands[lineNum] = "+attack";
        messages[lineNum++] = "PLATFORM_FOLLOWNEXTPLAYER";
        commands[lineNum] = CG_GetBoundSpectatorCommand("toggleads", "+speed");
        messages[lineNum++] = "PLATFORM_FOLLOWPREVIOUSPLAYER";
    }
    if (cg->nextSnap->ps.pm_flags & 0x2000000) {
        commands[lineNum] = CG_GetBoundSpectatorCommand("+melee", "+melee_breath");
        messages[lineNum++] = "PLATFORM_FOLLOWSTOP";
    }

    for (i = 0; i < lineNum; i++) {
        if (!GetKeyBindingLocalizedString(commands[i], binding))
            I_strncpyz(binding, UI_SafeTranslateString("KEY_UNBOUND"), 256);
        str = UI_ReplaceConversionString(UI_SafeTranslateString(messages[i]), binding);
        UI_DrawText(str, 0x7fffffff, font, 240.0f, y, 0, 0, 0.20833333f, colorWhite, 3);
        y = y + lineHeight;
    }
}

qboolean CG_DrawFollow(void)
{
    const char *clientName;
    const char *followingString;
    FontHandle font;
    float scale;
    float x;
    int textWidth;

    if (!(cg->nextSnap->ps.pm_flags & 0x400000)) {
        return 0;
    }

    if (cg->inKillCam) {
        return 0;
    }

    if (cg->bgs.clientinfo[cg->nextSnap->ps.clientNum].infoValid)
        clientName = cg->bgs.clientinfo[cg->nextSnap->ps.clientNum].name;
    else
        clientName = "?";

    followingString = SEH_LocalizeTextMessage("CGAME_FOLLOWING\x15", "spectator follow string", (msgLocErrType_t)0);
    scale = CL_IsRenderingSplitScreen() ? 0.5f : (1.0f / 3.0f);
    font = UI_GetFontHandle(0, 1.0f / 3.0f);

    textWidth = UI_TextWidth(followingString, 0, font, scale);
    x = (float)(-textWidth) * 0.5f;
    UI_DrawText(followingString, 0x7fffffff, font, x, 40.0f, 7, 1, scale, colorWhite, 3);

    textWidth = UI_TextWidth(clientName, 0, font, scale);
    x = (float)(-textWidth) * 0.5f;
    UI_DrawText(clientName, 0x7fffffff, font, x, 65.0f, 7, 1, scale, colorWhite, 3);

    return 1;
}

void CG_DrawPlayerSprites(void)
{
    int entityIndex;
    centity_t *cent;

    for (entityIndex = 0; entityIndex < cg->nextSnap->numEntities; ++entityIndex) {
        cent = &cg_entities[cg->nextSnap->entities[entityIndex].number];
        if (cent->nextState.eType == 1) {
            CG_PlayerSprites(cent);
        }
    }
}

void CG_Draw2D(void)
{
    bool drawHudMenus;
    bool drawHudElems;
    qboolean drewScoreboard;

    if (cg->cubemapShot)
        return;

    if (!cg_draw2D->current.enabled)
        return;

    CG_ScreenFade();

    drawHudMenus = !CG_AreHudMenusHidden();
    drawHudElems = !CG_AreHudElemsHidden();

    if (cg->nextSnap->ps.pm_type == 5)
    {
        CG_DrawIntermission();
        return;
    }

    if (cg->nextSnap->ps.pm_type == 4)
    {
        if (drawHudMenus)
        {
            CG_DrawSpectator();
            CG_DrawCrosshairNames();
            CG_DrawChatMessages();
        }
        if (drawHudElems)
        {
            CG_Draw2dHudElems(0);
            CG_Draw2dHudElems(1);
        }
    }
    else
    {
        if (cg->nextSnap->ps.pm_type < 6)
            CG_DrawCrosshair();
        if (drawHudMenus)
        {
            if (cg->nextSnap->ps.pm_type < 6)
                CG_DrawCrosshairNames();
            CG_DrawChatMessages();
        }
        CG_CheckTimedMenus();
        if (drawHudElems)
            CG_Draw2dHudElems(0);
        if (drawHudMenus)
            Menu_PaintAll(&cgDC);
        CG_Draw2dHudElems(1);
    }

    drewScoreboard = CG_DrawScoreboard();
    if (!drawHudMenus)
        return;

    CG_DrawFlashDamage();
    CG_DrawDamageDirectionIndicators();
    CG_DrawGrenadeIndicators();
    if (!drewScoreboard)
    {
        CG_DrawSpectatorMessage();
        CG_DrawFollow();
    }
    CG_DrawVote();
    CG_DrawLagometer();
    CG_DrawPerformanceWarnings();
    CG_DrawDebugOverlays();
    CG_DrawUpperRight();
    if (!drewScoreboard)
    {
        CG_DrawCenterString();
        if (cg_drawGameMessages->current.enabled)
            CG_DrawGameMessages();
        CG_DrawBoldGameMessages();
        CG_DrawMiniConsole();
        CG_DrawSubtitles();
        CG_DrawSay();
    }
    return;
}

// retail 0x4c5bd0, unreferenced; name unknown
void CG_DrawShellShockBlend_4c5bd0(void)
{
    CG_DrawShellShockSavedScreenBlend(cg->shellshock.parms, cg->shellshock.startTime, cg->shellshock.duration);
}

void CG_DrawActive(void)
{
    float sensitivity;
    float angles[3];
    vec_t *ka;
    vec_t *oa;

    cg->refdef.time = cg->time;
    cg->refdef.viewIndex = 0;

    sensitivity = cg->zoomSensitivity;
    if (cg->shellshock.sensitivity != 0.0f)
        sensitivity = cg->shellshock.sensitivity * cg->zoomSensitivity;

    ka = cg->kickAngles;
    oa = cg->offsetAngles;
    angles[0] = ka[0] + oa[0];
    angles[1] = ka[1] + oa[1];
    angles[2] = ka[2] + oa[2];

    CL_SetUserCmdAimValues(angles);

    CL_SetUserCmdValue(cg->weaponSelect, cg->equippedOffHand, sensitivity);

    CL_RenderScene(&cg->refdef);

    if (!CL_IsRenderingSplitScreen()) {
        /* Pass parms/startTime/duration with their real types. The decomp emitted
         * `*(float*)&field` reinterpretations here; on x86 they harmlessly launder
         * the same 4 bytes, but on x64 (Win64 ABI) a float arg is passed in an xmm
         * register while the pointer/int params are read from rcx/rdx/r8 -- so all
         * three args arrived as garbage (and the parms pointer was truncated to 32
         * bits), making the code see a bogus active shellshock and set
         * hasSavedScreen=1 every frame, which suppressed the viewmodel. */
        CG_DrawShellShockSavedScreenBlend(
            cg->shellshock.parms,
            cg->shellshock.startTime,
            cg->shellshock.duration);
    }

    CG_TileClear();
}

static inline int CG_RoundFPS(float value)
{
#if defined(_MSC_VER) && defined(_M_IX86)
    int result;
    /* Break exact half-way ties upward before x87's round-to-nearest. */
    const double roundEpsilon = 1.0 / 1073741824.0;

    __asm fld value
    __asm fadd roundEpsilon
    __asm fistp result

    return result;
#else
    return I_fround(value);
#endif
}
static inline void FastSinCos(const float value, float *pSin, float *pCos)
{
    __asm
    {
        push eax
        push ecx
        fld value
        fsincos
        mov ecx, pCos
        mov eax, pSin
        fstp dword ptr [ecx]
        fstp dword ptr [eax]
        pop ecx
        pop eax
    }
}

