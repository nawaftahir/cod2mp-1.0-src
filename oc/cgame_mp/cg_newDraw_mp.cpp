#include "common_types.h"
#include "imports.h"
#include "bytematch.h"
#include "headers/PC/cgame_mp/cg_local.h"
#include <math.h>
#include <string.h>
#include <stdio.h>
extern int I_stricmp(const char *s0, const char *s1);
extern weaponInfo_t cg_weaponsArray[];

extern const dvar_t *cg_drawHealth;
extern const dvar_t *cg_hudCompassSize;
extern const dvar_t *cg_hudProneY;
extern const dvar_t *cg_hudStanceFlash;
extern const dvar_t *cg_hudStanceHintPrints;

extern const dvar_t *cg_cursorHints;
extern const dvar_t *cg_drawBreathHint;
extern const dvar_t *cg_drawMantleHint;
extern const dvar_t *cg_hintFadeTime;
extern dvar_t *loc_warnings;
extern dvar_t *loc_warningsAsErrors;

extern const dvar_t *hud_fade_ammodisplay;
extern const dvar_t *hud_fade_healthbar;
extern const dvar_t *hud_fade_compass;
extern const dvar_t *hud_fade_stance;
extern const dvar_t *hud_fade_offhand;
extern const dvar_t *hud_health_startpulse_injured;
extern const dvar_t *hud_health_startpulse_critical;
extern const dvar_t *hud_health_pulserate_injured;
extern const dvar_t *hud_health_pulserate_critical;
extern const dvar_t *hud_deathQuoteFadeTime;
static vec4_t color;
static char szErrorString[1024];
static vec4_t color_00302d80;   /* duplicate decl removed (C++ redefinition -> C2086) */
static const dvar_t *hud_fadeout_speed;
static const dvar_t *hud_enable;
static const dvar_t *hud_healthOverlay_regenPauseTime;
static const dvar_t *hud_healthOverlay_pulseStart;
static const dvar_t *hud_healthOverlay_phaseOne_pulseDuration;
static const dvar_t *hud_healthOverlay_phaseTwo_toAlphaMultiplier;
static const dvar_t *hud_healthOverlay_phaseTwo_pulseDuration;
static const dvar_t *hud_healthOverlay_phaseThree_toAlphaMultiplier;
static const dvar_t *hud_healthOverlay_phaseThree_pulseDuration;
static const dvar_t *hud_healthOverlay_phaseEnd_toAlpha;
static const dvar_t *hud_healthOverlay_phaseEnd_pulseDuration;

typedef void (*MemoryFileArchiveProc)(MemoryFile *memFile, int size, void *data);

extern char *va( const char *format, ... );
extern int CL_GetKeyCatchers(void);
extern Bool CL_GetDisplayHUDWithKeycatchUI(void);
extern const char *CL_GetConfigString(int index);
extern void CL_DrawStretchPic(float x, float y, float w, float h, int horzAlign, int vertAlign, float s1, float t1, float s2, float t2, const vec_t *color, MaterialHandle material);
dvar_t *Dvar_RegisterBool(const char *dvarName, bool value, unsigned short flags);
extern dvar_t *Dvar_RegisterFloat(const char *dvarName, float value, float min, float max, unsigned short flags);
extern dvar_t *Dvar_RegisterInt(const char *dvarName, int value, int min, int max, unsigned short flags);
extern dvar_t* Dvar_FindVar(const char* dvarName);
extern void Controls_GetConfig(void);
extern int BG_GetNumWeapons(void);
extern int GetKeyBindingLocalizedString(const char *command, char *keys);
extern int BG_GetViewmodelWeaponIndex(const playerState_t *ps);
extern WeaponDef * BG_GetWeaponDef(int iWeapon);
extern int BG_AmmoForWeapon(int weapon);
extern int BG_GetTotalAmmoReserve(const playerState_t *ps, int weaponIndex);
extern int BG_GetAmmoTypeMax(int iAmmoIndex);
extern qboolean BG_WeaponIsClipOnly(int weapon);
extern int BG_ClipForWeapon(int weapon);
extern int BG_GetAmmoClipSize(int iClipIndex);
extern bool BG_DoesWeaponNeedSlot(int weapon);
extern int BG_GetEmptySlotForWeapon(playerState_t *pPS, int iWeaponIndex);
extern float sinf(float x);
extern const char *SEH_LocalizeTextMessage(const char *msg, const char *context, msgLocErrType_t errType);
extern const char *UI_ReplaceConversionString(const char *sourceString, const char *replaceString);
extern const char *UI_SafeTranslateString(const char *ref);
extern void I_strncpyz(char *dest, const char *src, int destsize);
extern int Com_sprintf(char *dest, size_t size, const char *format, ...);
extern int UI_TextWidth(const char *text, int maxChars, FontHandle font, float fontScale);
extern int UI_TextHeight(FontHandle font, float fontScale);
extern void UI_DrawText(const char *text, int maxChars, FontHandle font, float x, float y, int horzAlign, int vertAlign, float scale, const vec_t *color, int style);
extern void UI_DrawHandlePic(float x, float y, float w, float h, int horzAlign, int vertAlign, const vec_t *color, MaterialHandle hMaterial);
extern const char *SEH_StringEd_GetString(const char *pszReference);
extern void Com_Error(errorParm_t code, const char *fmt, ...);
extern void Com_Printf(const char *fmt, ...);
extern void I_strncat(char *dest, int maxlen, const char *src);
extern qboolean CG_ScoreboardDisplayed(void);
extern void CG_ScrollScoreboardUp(void);
extern void CG_ScrollScoreboardDown(void);
extern float *CG_FadeColor(int startMsec, int totalMsec, int fadeMsec);
extern qboolean GetCommandHasBinding(const char *command);
extern void Dvar_GetUnpackedColor(const dvar_t *dvar, vec_t *expandedColor);
extern void CG_UpdateCompassOrientation(void);
extern void CG_UpdateCompPointerOrientation(void);
extern void CG_DrawRotatedPic(float x, float y, float width, float height, int horzAlign, int vertAlign, float angle, const vec_t *color, MaterialHandle material);
extern void CG_DrawCompassFriendlies(rectDef_t *rect, MaterialHandle material, vec_t *color);
extern void CG_DrawOffHandName(rectDef_s *rect, struct Font_s *font, float scale, vec_t *color, int textStyle, int weaponType);
extern void CG_DrawOffHandIcon(rectDef_s *rect, float scale, vec_t *color, MaterialHandle material, int weaponType);
extern void CG_DrawOffHandHighlight(rectDef_s *rect, float scale, vec_t *color, MaterialHandle material, int weaponType);
extern void CG_DrawOffHandAmmo(rectDef_s *rect, struct Font_s *font, float scale, vec_t *color, int textStyle, int weaponType);
extern void CG_DrawTeamBackground(float x, float y, float w, float h, float alpha, int team);

enum {
    CG_PLAYER_AMMO_VALUE = 5,
    CG_PLAYER_AMMO_BACKDROP = 6,
    CG_PLAYER_STANCE = 20,
    CG_BLUE_SCORE = 27,
    CG_RED_SCORE = 28,
    CG_PLAYER_LOCATION = 33,
    CG_TEAM_COLOR = 34,
    CG_GAME_TYPE = 39,
    CG_KILLER = 50,
    CG_SPECTATORS = 60,
    CG_1STPLACE = 67,
    CG_2NDPLACE = 68,
    CG_PLAYER_AMMOCLIP_VALUE = 70,
    CG_HOLD_BREATH_HINT = 71,
    CG_CURSORHINT = 72,
    CG_PLAYER_BAR_HEALTH = 79,
    CG_MANTLE_HINT = 80,
    CG_PLAYER_WEAPON_NAME = 81,
    CG_PLAYER_WEAPON_NAME_BACK = 82,
    CG_PLAYER_WEAPON_MODE_ICON = 83,
    CG_PLAYER_COMPASS = 84,
    CG_PLAYER_COMPASS_BACK = 85,
    CG_PLAYER_COMPASS_POINTERS = 86,
    CG_PLAYER_COMPASS_FRIENDS = 89,
    CG_DRAW_SHADER = 94,
    CG_PLAYER_BAR_HEALTH_BACK = 95,
    CG_OFFHAND_WEAPON_ICON_FRAG = 100,
    CG_OFFHAND_WEAPON_ICON_SMOKE = 101,
    CG_OFFHAND_WEAPON_AMMO_FRAG = 102,
    CG_OFFHAND_WEAPON_AMMO_SMOKE = 103,
    CG_OFFHAND_WEAPON_NAME_FRAG = 104,
    CG_OFFHAND_WEAPON_NAME_SMOKE = 105,
    CG_OFFHAND_WEAPON_SELECT_FRAG = 106,
    CG_OFFHAND_WEAPON_SELECT_SMOKE = 107,
    CG_PLAYER_LOW_HEALTH_OVERLAY = 109
};

void CG_AntiBurnInHUD_RegisterDvars(void);
Bool CG_AreHudMenusHidden(void);
float CG_CalcPlayerHealth(void);
void CG_ResetLowHealthOverlay(void);
qboolean CG_ServerMaterialName(int index, char *materialName, int maxLen);
void CG_ApplySplitScreenCompassScale(float *x, float *y, float *w, float *h);
static const char *CG_GetUseString(void);
static void CG_DrawCursorhint(const rectDef_t *rect, struct Font_s *font, float fontscale, int textStyle);
void CG_DrawMantleHint(const rectDef_t *rect, struct Font_s *font, float fontscale, int textStyle);
const char *CG_GetTranslatedLocationString(int iLocation);
static void __attribute_regparm__(3) CG_DrawScore(int team, const rectDef_t *rect, struct Font_s *font, float scale, vec_t *color, MaterialHandle material, int textStyle);
const char *CG_GetKillerText(void);
const char *CG_GameTypeString(void);
int CG_KeyInterceptEvent(int key, qboolean down);
static void CG_PulseLowHealthOverlay(float healthRatio);
void CG_ArchiveState(MemoryFile *memFile);
void CG_DrawHoldBreathHint(const rectDef_t *rect, struct Font_s *font, float fontscale, int textStyle);
float CG_FadeHudMenu(const dvar_t *fadeDvar, int displayStartTime, int duration);
Bool CG_CheckPlayerForLowAmmo(void);
Bool CG_CheckPlayerForLowClip(void);
void CG_DrawPlayerCompassBack(const rectDef_t *rect, MaterialHandle material, vec_t *color);
static void CG_DrawPlayerStance(const rectDef_t *rect, vec_t *color, struct Font_s *font, float scale, int textStyle);
static void CG_DrawPlayerAmmoValue(const rectDef_t *rect, struct Font_s *font, float scale, vec_t *color, int textStyle);
extern void CG_DrawPlayerCompassPointers(const rectDef_t *rect, vec_t *color);
void CG_OwnerDraw(float x, float y, float w, float h, int horzAlign, int vertAlign, float text_x, float text_y, int ownerDraw, int ownerDrawFlags, int align, float special, struct Font_s *font, float scale, vec_t *color, MaterialHandle material, int textStyle);

COD2_ASSERT_FIELD(struct dvar_s, current,        0x08);
COD2_ASSERT_FIELD(WeaponDef,     weapClass,      0x7c);
COD2_ASSERT_FIELD(WeaponDef,     weapSlot,       0x80);
COD2_ASSERT_FIELD(WeaponDef,     overlayReticle, 0x278);
COD2_ASSERT_FIELD(WeaponDef,     bWideListIcon,  0x344);
COD2_ASSERT_FIELD(playerState_t, fWeaponPosFrac, 0xdc);

// retail 0x4bd0c0, unreferenced; Q3 CG_TeamColor (constants PINNED)
float *CG_TeamColor(int team)
{
    static vec4_t red = { 1.0f, 0.2f, 0.2f, 1.0f };
    static vec4_t blue = { 0.2f, 0.2f, 1.0f, 1.0f };
    static vec4_t other = { 1.0f, 1.0f, 1.0f, 1.0f };
    static vec4_t spectator = { 0.7f, 0.7f, 0.7f, 1.0f };

    switch (team) {
    case 1:
        return red;
    case 2:
        return blue;
    case 3:
        return spectator;
    default:
        return other;
    }
}

inline float I_fclamp(const float val, const float min, const float max);

inline float I_fsel(const float x, const float y, const float z)
{
    return x >= 0.0f ? y : z;
}

inline float I_fclamp(const float val, const float min, const float max)
{
    return I_fsel(min - val, min, I_fsel(val - max, max, val));
}

static qboolean CG_PlayerOwnsWeapon(const int *weapons, int weaponIndex)
{
    return (weapons[weaponIndex >> 5] & (1 << (weaponIndex & 31))) != 0;
}

// retail 0x4bd110/0x4bd120, unreferenced; MemFile_ArchiveData from CoD4 (SUPPORTED), int form name unknown
void MemFile_ArchiveData(MemoryFile *memFile, int bytes, void *data)
{
    ((MemoryFileArchiveProc)memFile->archiveProc)(memFile, bytes, data);
}

void MemFile_ArchiveInt_4bd120(MemoryFile *memFile, int *data)
{
    ((MemoryFileArchiveProc)memFile->archiveProc)(memFile, 4, data);
}

void CG_AntiBurnInHUD_RegisterDvars(void)
{
    hud_fadeout_speed = Dvar_RegisterFloat("hud_fadeout_speed", 0.1f, 0.0f, 1.0f, 0x1001);
    hud_enable = Dvar_RegisterBool("hud_enable", 1, 0x1001);

    hud_fade_ammodisplay = Dvar_RegisterFloat("hud_fade_ammodisplay", 0.0f, 0.0f, 30.0f, 0x1001);
    hud_fade_healthbar = Dvar_RegisterFloat("hud_fade_healthbar", 2.0f, 0.0f, 30.0f, 0x1001);
    hud_fade_compass = Dvar_RegisterFloat("hud_fade_compass", 0.0f, 0.0f, 30.0f, 0x1001);
    hud_fade_stance = Dvar_RegisterFloat("hud_fade_stance", 1.7f, 0.0f, 30.0f, 0x1001);
    hud_fade_offhand = Dvar_RegisterFloat("hud_fade_offhand", 0.0f, 0.0f, 30.0f, 0x1001);

    hud_health_startpulse_injured = Dvar_RegisterFloat("hud_health_startpulse_injured", 1.0f, 0.0f, 1.1f, 0x1001);
    hud_health_startpulse_critical = Dvar_RegisterFloat("hud_health_startpulse_critical", 0.33f, 0.0f, 1.1f, 0x1001);
    hud_health_pulserate_injured = Dvar_RegisterFloat("hud_health_pulserate_injured", 1.0f, 0.1f, 3.0f, 0x1001);
    hud_health_pulserate_critical = Dvar_RegisterFloat("hud_health_pulserate_critical", 0.5f, 0.1f, 3.0f, 0x1001);

    hud_deathQuoteFadeTime = Dvar_RegisterInt("hud_deathQuoteFadeTime", 1000, 0, 100000, 0x1001);

    hud_healthOverlay_regenPauseTime = Dvar_RegisterInt("hud_healthOverlay_regenPauseTime", 5000, 0, 10000, 0x1080);
    hud_healthOverlay_pulseStart = Dvar_RegisterFloat("hud_healthOverlay_pulseStart", 0.35f, 0.0f, 1.0f, 0x1080);
    hud_healthOverlay_phaseOne_pulseDuration = Dvar_RegisterInt("hud_healthOverlay_phaseOne_pulseDuration", 150, 0, 1000, 0x1080);
    hud_healthOverlay_phaseTwo_toAlphaMultiplier = Dvar_RegisterFloat("hud_healthOverlay_phaseTwo_toAlphaMultiplier", 0.7f, 0.0f, 1.0f, 0x1080);
    hud_healthOverlay_phaseTwo_pulseDuration = Dvar_RegisterInt("hud_healthOverlay_phaseTwo_pulseDuration", 320, 0, 1000, 0x1080);
    hud_healthOverlay_phaseThree_toAlphaMultiplier = Dvar_RegisterFloat("hud_healthOverlay_phaseThree_toAlphaMultiplier", 0.6f, 0.0f, 1.0f, 0x1080);
    hud_healthOverlay_phaseThree_pulseDuration = Dvar_RegisterInt("hud_healthOverlay_phaseThree_pulseDuration", 400, 0, 1000, 0x1080);
    hud_healthOverlay_phaseEnd_toAlpha = Dvar_RegisterFloat("hud_healthOverlay_phaseEnd_toAlpha", 0.0f, 0.0f, 1.0f, 0x1080);
    hud_healthOverlay_phaseEnd_pulseDuration = Dvar_RegisterInt("hud_healthOverlay_phaseEnd_pulseDuration", 700, 0, 1000, 0x1080);
}

Bool CG_AreHudMenusHidden(void)
{
    return ((CL_GetKeyCatchers() & 8) && !CL_GetDisplayHUDWithKeycatchUI()) || !cg->drawHud ? 1 : 0;
}

float CG_FadeHudMenu(const dvar_t *fadeDvar, int displayStartTime, int duration)
{
    float *fadeColor;

    if (CG_AreHudMenusHidden() || !hud_enable->current.enabled) {
        return 0.0f;
    }

    if (fadeDvar->current.value == 0.0f) {
        return 1.0f;
    }

    fadeColor = CG_FadeColor(displayStartTime, duration, 700);
    if (!fadeColor) {
        return 0.0f;
    }

    return fadeColor[3];
}

static int CG_GetDrawWeaponIndex(void)
{
    playerState_t *ps;
    int weaponIndex;

    ps = &cg->predictedPlayerState;
    weaponIndex = cg->weaponSelect;
    if (weaponIndex >= 0 && weaponIndex < BG_GetNumWeapons() && CG_PlayerOwnsWeapon(ps->weapons, weaponIndex)) {
        return weaponIndex;
    }
    return ps->weapon;
}

Bool CG_CheckPlayerForLowAmmo(void)
{
    playerState_t *ps;
    int weaponIndex;
    int ammoIndex;
    int currentAmmo;
    int maxAmmo;

    ps = &cg->predictedPlayerState;
    weaponIndex = cg->weaponSelect;

    if (weaponIndex < 0 || weaponIndex >= BG_GetNumWeapons() || !(ps->weapons[weaponIndex >> 5] & (1 << (weaponIndex & 0x1f)))) {
        weaponIndex = ps->weapon;
    }

    if (!weaponIndex) {
        return 0;
    }

    ammoIndex = BG_AmmoForWeapon(weaponIndex);
    currentAmmo = BG_GetTotalAmmoReserve(ps, weaponIndex);
    if (currentAmmo > 999) {
        currentAmmo = 999;
    }

    maxAmmo = BG_GetAmmoTypeMax(ammoIndex);
    if (maxAmmo > 999) {
        maxAmmo = 999;
    } else if (maxAmmo < 0) {
        return 0;
    }

    if ((float)currentAmmo <= (float)maxAmmo * 0.2f) {
        return 1;
    }
    return 0;
}

Bool CG_CheckPlayerForLowClip(void)
{
    playerState_t *ps;
    int weaponIndex;
    int clipIndex;
    int currentClip;
    int clipSize;

    ps = &cg->predictedPlayerState;
    weaponIndex = cg->weaponSelect;

    if (weaponIndex < 0 || weaponIndex >= BG_GetNumWeapons() || !(ps->weapons[weaponIndex >> 5] & (1 << (weaponIndex & 0x1f)))) {
        weaponIndex = ps->weapon;
    }

    if (!weaponIndex || BG_WeaponIsClipOnly(weaponIndex)) {
        return 0;
    }

    clipIndex = BG_ClipForWeapon(weaponIndex);
    currentClip = ps->ammoclip[clipIndex];
    if (currentClip < 0) {
        return 0;
    }
    if (currentClip > 999) {
        currentClip = 999;
    }

    clipSize = BG_GetAmmoClipSize(clipIndex);
    if (clipSize > 999) {
        clipSize = 999;
    } else if (clipSize <= 0) {
        return 0;
    }

    if ((float)currentClip <= 0.330000013f * (float)clipSize) {
        return 1;
    }
    return 0;
}

static void CG_DrawPlayerAmmoBackdrop(const rectDef_t *rect, MaterialHandle material, vec_t *color)
{
    vec_t drawColor[4];

    if (cg->predictedPlayerState.weapon)
    {
        drawColor[3] = CG_FadeHudMenu(hud_fade_ammodisplay, cg->ammoFadeTime,
                                          I_fround(hud_fade_ammodisplay->current.value * 1000.0f));
        if (drawColor[3] != 0.0f)
        {
            if (CG_CheckPlayerForLowAmmo())
            {
                drawColor[0] = 0.89f;
                drawColor[1] = 0.18f;
                drawColor[2] = 0.01f;
            }
            else
            {
                drawColor[0] = color[0];
                drawColor[1] = color[1];
                drawColor[2] = color[2];
            }
            UI_DrawHandlePic(rect->x, rect->y, rect->w, rect->h, rect->horzAlign, rect->vertAlign, drawColor, material);
        }
    }
}

static void CG_DrawPlayerAmmoValue(const rectDef_t *rect, struct Font_s *font, float scale, vec_t *color, int textStyle)
{
    vec_t ammoColor[4];
    int ammoVal;
    bool drawAmmo;
    char clipString[256];
    char ammoString[256];
    Bool lowAmmo;
    Bool lowClip;
    bool drawClip;
    vec_t flashColor[4];
    int weapIndex;
    float x;
    int clipVal;
    centity_t *cent;

    if (!cg->predictedPlayerState.weapon)
        return;

    color[3] = CG_FadeHudMenu(hud_fade_ammodisplay, cg->ammoFadeTime,
                                  I_fround(hud_fade_ammodisplay->current.value * 1000.0f));
    if (color[3] == 0.0f)
        return;

    cent = &cg_entities[cg->nextSnap->ps.clientNum];
    if ((cg->nextSnap->ps.pm_flags & 0x800000) && cg->weaponSelect >= 0 && cg->weaponSelect < BG_GetNumWeapons())
        weapIndex = cg->weaponSelect;
    else
        weapIndex = cent->nextState.weapon;
    if (!weapIndex)
        return;

    drawAmmo = 1;
    drawClip = 1;
    lowAmmo = 0;
    lowClip = 0;
    ammoVal = BG_GetTotalAmmoReserve(&cg->predictedPlayerState, weapIndex);
    if (BG_WeaponIsClipOnly(weapIndex))
        clipVal = -1;
    else
        clipVal = cg->predictedPlayerState.ammoclip[BG_ClipForWeapon(weapIndex)];
    if (clipVal < 0)
        drawClip = 0;
    if (clipVal > 999)
        clipVal = 999;
    if (ammoVal < 0)
        drawAmmo = 0;
    if (ammoVal > 999)
        ammoVal = 999;

    if (drawClip)
    {
        sprintf(clipString, "%2i", clipVal);
        lowClip = CG_CheckPlayerForLowClip();
    }
    if (drawAmmo)
    {
        sprintf(ammoString, "%3i", ammoVal);
        lowAmmo = CG_CheckPlayerForLowAmmo();
    }

    if (lowClip)
    {
        if (cg->lastClipFlashTime > cg->time || cg->lastClipFlashTime + 800 < cg->time)
            cg->lastClipFlashTime = cg->time;
        flashColor[0] = 0.89f;
        flashColor[1] = 0.18f;
        flashColor[2] = 0.01f;
        flashColor[3] = (float)(cg->lastClipFlashTime + 800 - cg->time); flashColor[3] = flashColor[3] * 0.00125f;
        if (color[3] < flashColor[3])
            flashColor[3] = color[3];
    }

    if (lowAmmo)
    {
        ammoColor[0] = 0.89f;
        ammoColor[1] = 0.18f;
        ammoColor[2] = 0.01f;
    }
    else
    {
        ammoColor[0] = color[0];
        ammoColor[1] = color[1];
        ammoColor[2] = color[2];
    }
    ammoColor[3] = color[3];

    if (drawClip)
    {
        if (drawAmmo)
        {
            UI_DrawText(clipString, 0x7fffffff, font, rect->x, rect->y, rect->horzAlign, rect->vertAlign, scale, color, textStyle);
            if (lowClip)
                UI_DrawText(clipString, 0x7fffffff, font, rect->x, rect->y, rect->horzAlign, rect->vertAlign, scale, flashColor, textStyle);
            x = rect->x + rect->w - (float)UI_TextWidth(ammoString, 0, font, scale);
            UI_DrawText(ammoString, 0x7fffffff, font, x, rect->y, rect->horzAlign, rect->vertAlign, scale, ammoColor, textStyle);
            x = (rect->w - (float)UI_TextWidth("|", 0, font, scale)) * 0.5f + rect->x - 5.0f;
            UI_DrawText("|", 0x7fffffff, font, x, rect->y, rect->horzAlign, rect->vertAlign, scale, ammoColor, textStyle);
        }
        else
        {
            x = (rect->w - (float)UI_TextWidth(clipString, 0, font, scale)) * 0.5f + rect->x;
            UI_DrawText(clipString, 0x7fffffff, font, x, rect->y, rect->horzAlign, rect->vertAlign, scale, color, textStyle);
            if (lowClip)
                UI_DrawText(clipString, 0x7fffffff, font, x, rect->y, rect->horzAlign, rect->vertAlign, scale, flashColor, textStyle);
        }
    }
    else if (drawAmmo)
    {
        x = (rect->w - (float)UI_TextWidth(ammoString, 0, font, scale)) * 0.5f + rect->x;
        UI_DrawText(ammoString, 0x7fffffff, font, x, rect->y, rect->horzAlign, rect->vertAlign, scale, ammoColor, textStyle);
    }
}

static void CG_DrawPlayerWeaponName(const rectDef_t *rect, struct Font_s *font, float scale, vec_t *color, int textStyle)
{
    weaponInfo_t *weapInfo;
    float *fadeColor;
    const char *string;
    vec_t drawColor[4];
    int weapIndex;
    WeaponDef *weapDef;
    float x;

    fadeColor = CG_FadeColor(cg->weaponSelectTime, 1800, 700);
    if (fadeColor)
    {
        drawColor[3] = fadeColor[3];
        drawColor[0] = color[0];
        drawColor[1] = color[1];
        drawColor[2] = color[2];
        weapIndex = CG_GetDrawWeaponIndex();
        if (weapIndex)
        {
            weapInfo = &cg_weaponsArray[weapIndex];
            weapDef = BG_GetWeaponDef(weapIndex);
            if (*weapDef->szModeName)
                string = va("%s / %s", weapInfo->pszTranslatedDisplayName, weapInfo->pszTranslatedModename);
            else
                string = va("%s", weapInfo->pszTranslatedDisplayName);
            x = rect->x + rect->w - (float)UI_TextWidth(string, 0, font, scale) - 28.0f;
            UI_DrawText(string, 0x7fffffff, font, x, rect->y, rect->horzAlign, rect->vertAlign, scale, drawColor, textStyle);
        }
    }
}

static void CG_DrawPlayerWeaponNameBack(const rectDef_t *rect, struct Font_s *font, float scale, vec_t *color, MaterialHandle material)
{
    weaponInfo_t *weapInfo;
    const char *string;
    vec_t drawColor[4];
    int weapIndex;
    WeaponDef *weapDef;
    float x;
    float w;

    drawColor[3] = CG_FadeHudMenu(hud_fade_ammodisplay, cg->weaponSelectTime, 1800);
    if (drawColor[3] != 0.0f)
    {
        drawColor[0] = color[0];
        drawColor[1] = color[1];
        drawColor[2] = color[2];
        weapIndex = CG_GetDrawWeaponIndex();
        if (weapIndex)
        {
            weapInfo = &cg_weaponsArray[weapIndex];
            weapDef = BG_GetWeaponDef(weapIndex);
            if (*weapDef->szModeName)
                string = va("%s / %s", weapInfo->pszTranslatedDisplayName, weapInfo->pszTranslatedModename);
            else
                string = va("%s", weapInfo->pszTranslatedDisplayName);
            w = (float)UI_TextWidth(string, 0, font, scale) + 36.0f;
            x = rect->x + rect->w - w;
            UI_DrawHandlePic(x, rect->y, w, rect->h, rect->horzAlign, rect->vertAlign, drawColor, material);
        }
    }
}

static void CG_DrawPlayerWeaponModeIcon(const rectDef_t *rect, vec_t *color)
{
    int weapIndex;
    weaponInfo_t *weapInfo;

    color[3] = CG_FadeHudMenu(hud_fade_ammodisplay, cg->weaponSelectTime, 1800);
    if (color[3] != 0.0f)
    {
        weapIndex = CG_GetDrawWeaponIndex();
        if (weapIndex)
        {
            weapInfo = &cg_weaponsArray[weapIndex];
            if (weapInfo->hModeIcon)
                UI_DrawHandlePic(rect->x, rect->y, rect->w, rect->h, rect->horzAlign, rect->vertAlign, color, weapInfo->hModeIcon);
        }
    }
}

static void CG_DrawStanceIcon(const rectDef_t *rect, vec_t *drawColor, float x, float y, float fadeAlpha)
{
    MaterialHandle icon;
    float width;
    float height;
    float flashAlpha;

    width = rect->w;
    height = rect->h;

    if (cg->lastStance & 1) {
        icon = cgs->media.stanceMaterials[2];
    } else if (cg->lastStance & 2) {
        icon = cgs->media.stanceMaterials[1];
    } else {
        icon = cgs->media.stanceMaterials[0];
    }

    UI_DrawHandlePic(
        x, y, width, height,
        rect->horzAlign, rect->vertAlign,
        drawColor, icon);

    if (cg->lastStanceChangeTime + 1000 > cg->time) {
        Dvar_GetUnpackedColor(cg_hudStanceFlash, drawColor);

        flashAlpha = (float)(cg->lastStanceChangeTime - cg->time + 1000) / 1000.0f;
        drawColor[3] = flashAlpha * 0.8f;
        if (fadeAlpha < drawColor[3]) {
            drawColor[3] = fadeAlpha;
        }

        UI_DrawHandlePic(
            x, y, width, height,
            rect->horzAlign, rect->vertAlign,
            drawColor, cgs->media.stanceMaterials[3]);
    }
}

static void CG_DrawPlayerStance(const rectDef_t *rect, vec_t *color, struct Font_s *font, float scale, int textStyle)
{
    float x;
    float y;
    float fadeAlpha;
    vec4_t drawColor;
    const char *proneStr;
    int duration;

    duration = I_fround(1000.0f * hud_fade_stance->current.value);
    fadeAlpha = CG_FadeHudMenu(hud_fade_stance, cg->stanceFadeTime, duration);

    if (fadeAlpha == 0.0f) {
        return;
    }

    if (!cg_hudStanceHintPrints->current.enabled) {
        cg->lastStanceChangeTime = 0;
    } else if (cg->lastStance != (cg->predictedPlayerState.pm_flags & 3)) {
        cg->lastStanceChangeTime = cg->time;
    }

    cg->lastStance = cg->predictedPlayerState.pm_flags & 3;

    x = rect->x + (cg_hudCompassSize->current.value - 1.0f) * cgs->compassWidth * 0.7f;
    y = rect->y;

    drawColor[0] = color[0];
    drawColor[1] = color[1];
    drawColor[2] = color[2];

    if ((cg->predictedPlayerState.pm_flags & 0x10000) && cg->proneBlockedEndTime < cg->time) {
        cg->proneBlockedEndTime = cg->time + 1500;
    }

    if (cg->proneBlockedEndTime > cg->time) {
        int width;
        float halfWidth;
        float pulseDegrees;

        proneStr = UI_SafeTranslateString("CGAME_PRONE_BLOCKED");
        width = UI_TextWidth(proneStr, 0, font, scale);
        halfWidth = (float)width * 0.5f;
        pulseDegrees = 540.0f * ((float)(cg->proneBlockedEndTime - cg->time) / 1500.0f);
        drawColor[3] = (float)fabs(sin(pulseDegrees * 0.017453292f));

        UI_DrawText(
            proneStr,
            0x7fffffff,
            font,
            -halfWidth,
            cg_hudProneY->current.value,
            7,
            3,
            scale,
            drawColor,
            textStyle);
    }

    if (cg_hudStanceHintPrints->current.enabled && cg->lastStanceChangeTime + 3000 > cg->time) {
        int i;
        int commandIndex;
        int numHintLines;
        float height;
        float hintY;
        const char *string;
        const char *binding;
        const char *hintLineCmds[3];
        char keyBinding[256];
        const char *standCmds[3][6] = {
            { NULL },
            { "gocrouch", "togglecrouch", "lowerstance", "+movedown", NULL, NULL },
            { "goprone", "+prone", NULL }
        };
        const char *duckCmds[3][6] = {
            { "+gostand", "raisestance", "+moveup", NULL },
            { NULL },
            { "goprone", "lowerstance", "toggleprone", "+prone", NULL, NULL }
        };
        const char *proneCmds[3][6] = {
            { "+gostand", "toggleprone", NULL },
            { "gocrouch", "togglecrouch", "raisestance", "+movedown", "+moveup", NULL },
            { NULL }
        };
        const char *hintTypeStrings[3] = {
            "PLATFORM_STANCEHINT_STAND",
            "PLATFORM_STANCEHINT_CROUCH",
            "PLATFORM_STANCEHINT_PRONE"
        };

        Controls_GetConfig();

        if (cg->lastStanceChangeTime + 2000 > cg->time) {
            drawColor[3] = 1.0f;
        } else {
            drawColor[3] = (float)(cg->lastStanceChangeTime - cg->time + 3000) / 1000.0f;
        }

        height = (float)UI_TextHeight(font, scale);
        numHintLines = 0;

        for (i = 0; i < 3; ++i) {
            hintLineCmds[i] = NULL;

            commandIndex = 0;
            if (cg->lastStance & 1) {
                binding = proneCmds[i][commandIndex];
            } else if (cg->lastStance & 2) {
                binding = duckCmds[i][commandIndex];
            } else {
                binding = standCmds[i][commandIndex];
            }
            while (commandIndex < 6 && binding) {
                if (GetCommandHasBinding(binding)) {
                    hintLineCmds[i] = binding;
                    ++numHintLines;
                    break;
                }
                ++commandIndex;
                if (cg->lastStance & 1) {
                    binding = proneCmds[i][commandIndex];
                } else if (cg->lastStance & 2) {
                    binding = duckCmds[i][commandIndex];
                } else {
                    binding = standCmds[i][commandIndex];
                }
            }
        }

        hintY = rect->y + rect->h * 0.5f - 1.5f;

        switch (numHintLines) {
        case 1:
            hintY += height * 0.5f;
            break;
        case 3:
            hintY -= height * 0.5f + 1.5f;
            break;
        }

        if (fadeAlpha < drawColor[3]) {
            drawColor[3] = fadeAlpha;
        }

        for (i = 0; i < 3; ++i) {
            if (!hintLineCmds[i]) {
                continue;
            }

            GetKeyBindingLocalizedString(hintLineCmds[i], keyBinding);
            string = UI_ReplaceConversionString(UI_SafeTranslateString(hintTypeStrings[i]), keyBinding);

            UI_DrawText(
                string,
                0x7fffffff,
                font,
                x + rect->w,
                hintY,
                rect->horzAlign,
                rect->vertAlign,
                scale,
                drawColor,
                textStyle);

            hintY += height + 1.5f;
        }
    }

    drawColor[3] = fadeAlpha;
    CG_DrawStanceIcon(rect, drawColor, x, y, fadeAlpha);
}

float CG_CalcPlayerHealth(void)
{
    playerState_t *ps;
    float healthRatio;

    ps = &cgArray[0].nextSnap->ps;
    if (ps->stats[0] == 0 || ps->stats[2] == 0 || ps->pm_type == 6) {
        return 0.0f;
    }

    healthRatio = (float)ps->stats[0] / (float)ps->stats[2];
    if (healthRatio < 0.0f) {
        return 0.0f;
    }
    if (healthRatio > 1.0f) {
        return 1.0f;
    }
    return healthRatio;
}

void CG_DrawPlayerBarHealth(const rectDef_t *rect, MaterialHandle material, vec_t *color)
{
    float health;
    float frac;
    float scale;
    playerState_t *ps;
    float x;
    float y;
    float w;
    float h;

    if (!cg_drawHealth->current.enabled)
        return;

    health = CG_CalcPlayerHealth();
    color[3] = CG_FadeHudMenu(hud_fade_healthbar, cg->healthFadeTime, I_fround(hud_fade_healthbar->current.value * 1000.0f));
    if (color[3] == 0.0f)
        return;

    ps = &cg->nextSnap->ps;
    frac = I_fclamp(health, 0.0f, 1.0f);

    if (health > 0.0f)
    {
        x = rect->x;
        y = rect->y;
        w = health * rect->w;
        h = rect->h;
        if (frac > 0.5f)
        {
            scale = 1.0f - frac;
            color[0] = scale * color[0] + scale * color[0];
            color[2] = scale * color[2] + scale * color[2];
        }
        else
        {
            color[1] = (frac + 0.2f) * color[1] + 0.3f;
        }
        CL_DrawStretchPic(x, y, w, h, rect->horzAlign, rect->vertAlign, 0.0f, 0.0f, health, 1.0f, color, material);
    }

    if (cg->lastHealthClient != ps->clientNum)
    {
        cg->lastHealthClient = ps->clientNum;
        cg->lastHealth = health;
        cg->lastHealthLerpDelay = 1;
    }
    else if (health < cg->lastHealth)
    {
        if (cg->lastHealthLerpDelay)
        {
            cg->lastHealthLerpDelay -= cg->frametime;
            if (cg->lastHealthLerpDelay < 0)
                cg->lastHealthLerpDelay = 0;
        }
        else
        {
            cg->lastHealth = cg->lastHealth - (float)cg->frametime * 0.0012f;
            if (cg->lastHealth <= health)
            {
                cg->lastHealth = health;
                cg->lastHealthLerpDelay = 1;
            }
        }
    }
    else
    {
        cg->lastHealth = health;
        cg->lastHealthLerpDelay = 1;
    }

    if (cg->lastHealth > health)
    {
        x = (health * rect->w) + rect->x;
        y = rect->y;
        w = (cg->lastHealth - health) * rect->w;
        h = rect->h;
        color[0] = 1.0f;
        color[1] = 0.0f;
        color[2] = 0.0f;
        CL_DrawStretchPic(x, y, w, h, rect->horzAlign, rect->vertAlign, health, 0.0f, cg->lastHealth, 1.0f, color, material);
    }
}

void CG_DrawPlayerBarHealthBack(const rectDef_t *rect, MaterialHandle material, vec_t *color)
{
    int flashTime;
    float health;
    float fadeAlpha;
    float x;
    float y;
    float w;
    float h;

    if (!cg_drawHealth->current.enabled)
        return;

    fadeAlpha = CG_FadeHudMenu(hud_fade_healthbar, cg->healthFadeTime, I_fround(hud_fade_healthbar->current.value * 1000.0f));
    if (fadeAlpha == 0.0f)
        return;

    color[3] = fadeAlpha;
    x = rect->x;
    y = rect->y;
    w = rect->w;
    h = rect->h;
    CL_DrawStretchPic(x, y, w, h, rect->horzAlign, rect->vertAlign, 0.0f, 0.0f, 1.0f, 1.0f, color, material);

    health = CG_CalcPlayerHealth();
    if (health == 0.0f)
        return;

    if (health < hud_health_startpulse_critical->current.value)
        flashTime = I_fround(hud_health_pulserate_critical->current.value * 1000.0f);
    else if (health < hud_health_startpulse_injured->current.value)
        flashTime = I_fround(hud_health_pulserate_injured->current.value * 1000.0f);
    else
        return;

    if (flashTime)
    {
        if (cg->lastHealthPulseTime > cg->time || flashTime + cg->lastHealthPulseTime < cg->time)
            cg->lastHealthPulseTime = cg->time;
        color[0] = 0.89f;
        color[1] = 0.18f;
        color[2] = 0.01f;
        color[3] = (float)(cg->lastHealthPulseTime - cg->time + flashTime) / (float)flashTime;
        if (fadeAlpha < color[3])
            color[3] = fadeAlpha;
        CL_DrawStretchPic(x, y, w, h, rect->horzAlign, rect->vertAlign, 0.0f, 0.0f, 1.0f, 1.0f, color, material);
    }
}

float CG_FadeLowHealthOverlay(void)
{
    int elapsed;

    elapsed = cg->time - cg->healthOverlayPulseTime;
    if (elapsed < 0)
        elapsed = 0;

    if (cg->healthOverlayPulseDuration <= 0 || elapsed >= cg->healthOverlayPulseDuration)
        return cg->healthOverlayToAlpha;

    return ((float)elapsed / (float)cg->healthOverlayPulseDuration) * (cg->healthOverlayToAlpha - cg->healthOverlayFromAlpha)
        + cg->healthOverlayFromAlpha;
}

void CG_PulseLowHealthOverlay(float healthRatio)
{
    float pulseMags[4] = { 1.0f, 0.8f, 0.6f, 0.3f };

    if (healthRatio < cg->healthOverlayOldHealth && healthRatio < hud_healthOverlay_pulseStart->current.value) {
        cg->healthOverlayLastHitTime = cg->time;
        cg->healthOverlayPulseIndex = 0;
    }
    cg->healthOverlayOldHealth = healthRatio;
    if (cg->healthOverlayPulseDuration + cg->healthOverlayPulseTime <= cg->time
        && (healthRatio < hud_healthOverlay_pulseStart->current.value || cg->healthOverlayHurt)) {
        if (!cg->healthOverlayHurt)
            cg->healthOverlayHurt = 1;
        cg->healthOverlayPulseTime = cg->time;
        cg->healthOverlayFromAlpha = cg->healthOverlayToAlpha;
        if ((unsigned int)cg->healthOverlayPulseIndex < 4) {
            switch (cg->healthOverlayPulsePhase) {
            case 0:
                cg->healthOverlayToAlpha = I_fclamp(pulseMags[cg->healthOverlayPulseIndex], 0.0f, 1.0f);
                cg->healthOverlayPulseDuration = hud_healthOverlay_phaseOne_pulseDuration->current.integer;
                cg->healthOverlayPulsePhase = 1;
                break;
            case 1:
                cg->healthOverlayToAlpha = I_fclamp(pulseMags[cg->healthOverlayPulseIndex] * hud_healthOverlay_phaseTwo_toAlphaMultiplier->current.value, 0.0f, 1.0f);
                cg->healthOverlayPulseDuration = hud_healthOverlay_phaseTwo_pulseDuration->current.integer;
                cg->healthOverlayPulsePhase = 2;
                break;
            case 2:
                cg->healthOverlayToAlpha = I_fclamp(pulseMags[cg->healthOverlayPulseIndex] * hud_healthOverlay_phaseThree_toAlphaMultiplier->current.value, 0.0f, 1.0f);
                cg->healthOverlayPulseDuration = hud_healthOverlay_phaseThree_pulseDuration->current.integer;
                cg->healthOverlayPulsePhase = 0;
                if (cg->time >= hud_healthOverlay_regenPauseTime->current.integer + cg->healthOverlayLastHitTime
                    - 3 * (hud_healthOverlay_phaseThree_pulseDuration->current.integer
                        + hud_healthOverlay_phaseTwo_pulseDuration->current.integer
                        + hud_healthOverlay_phaseOne_pulseDuration->current.integer))
                    ++cg->healthOverlayPulseIndex;
                break;
            }
        } else {
            cg->healthOverlayHurt = 0;
            cg->healthOverlayToAlpha = hud_healthOverlay_phaseEnd_toAlpha->current.value;
            cg->healthOverlayPulseDuration = hud_healthOverlay_phaseEnd_pulseDuration->current.integer;
            cg->healthOverlayPulsePhase = 0;
        }
    }
}

void CG_ResetLowHealthOverlay(void)
{
    cg->healthOverlayHurt = 0;
    cg->healthOverlayToAlpha = hud_healthOverlay_phaseEnd_toAlpha->current.value;
    cg->healthOverlayPulseDuration = 0;
    cg->healthOverlayPulsePhase = 0;
    cg->healthOverlayPulseIndex = 0;
    cg->healthOverlayOldHealth = 1.0f;
}

void CG_DrawPlayerLowHealthOverlay(const rectDef_t *rect, MaterialHandle material, vec_t *color)
{
    float healthRatio;

    healthRatio = CG_CalcPlayerHealth();
    if (healthRatio == 0.0f)
        return;

    CG_PulseLowHealthOverlay(healthRatio);
    color[3] = CG_FadeLowHealthOverlay();
    if (color[3] == 0.0f)
        return;

    CL_DrawStretchPic(rect->x, rect->y, rect->w, rect->h, rect->horzAlign, rect->vertAlign, 0.0f, 0.0f, 1.0f, 1.0f, color, material);
}

qboolean CG_ServerMaterialName(int index, char *materialName, int maxLen)
{
    const char *configString;

    if (index <= 0 || index >= 128) {
        return 0;
    }

    configString = CL_GetConfigString(index + 0x61e);
    if (configString[0] == '\0') {
        return 0;
    }

    if (strlen(configString) >= (unsigned int)maxLen) {
        return 0;
    }

    strcpy(materialName, configString);
    return 1;
}

extern MaterialHandle CL_RegisterMaterialNoMip(const char *name, int imageTrack);

// Unreferenced; original name unknown.
MaterialHandle CG_ObjectiveIconMaterial(int icon, int index)
{
    char materialName[64];

    if (icon && CG_ServerMaterialName(icon, materialName, 64))
        return CL_RegisterMaterialNoMip(materialName, 7);

    return cgs->media.objectiveMaterials[index];
}

// retail 0x4bf0c0, unreferenced; name unknown
void CG_CompassCalcDimensions_4bf0c0(const rectDef_t *rect, float *x, float *y, float *w, float *h)
{
    *x = rect->x;
    *y = rect->y - (cg_hudCompassSize->current.value - 1.0f) * rect->h;
    *w = rect->w * cg_hudCompassSize->current.value;
    *h = rect->h * cg_hudCompassSize->current.value;
}

void CG_DrawPlayerCompass(const rectDef_t *rect, MaterialHandle material, vec_t *color)
{
    int duration;
    int displayStartTime;
    float alpha;
    float x;
    float y;
    float w;
    float h;

    duration = I_fround(1000.0f * hud_fade_compass->current.value);
    displayStartTime = cg->compassFadeTime;
    alpha = CG_FadeHudMenu(hud_fade_compass, displayStartTime, duration);
    color[3] = alpha;
    if (alpha == 0.0f) {
        return;
    }

    x = rect->x;
    y = rect->y - (cg_hudCompassSize->current.value - 1.0f) * rect->h;
    w = rect->w * cg_hudCompassSize->current.value;
    h = rect->h * cg_hudCompassSize->current.value;
    CG_UpdateCompassOrientation();
    CG_DrawRotatedPic(x, y, w, h, rect->horzAlign, rect->vertAlign, cg->compassYaw, color, material);
}

void CG_ApplySplitScreenCompassScale(float *x, float *y, float *w, float *h)
{

}

void CG_DrawPlayerCompassBack(const rectDef_t *rect, MaterialHandle material, vec_t *color)
{
    int duration;
    int displayStartTime;
    float alpha;

    duration = I_fround((1000.0f * hud_fade_compass->current.value));
    displayStartTime = cg->compassFadeTime;
    alpha = CG_FadeHudMenu(hud_fade_compass, displayStartTime, duration);
    color[3] = alpha;
    if (alpha == 0.0f) {
        return;
    }

    CL_DrawStretchPic(
        rect->x,
        rect->y - rect->h * (cg_hudCompassSize->current.value - 1.0f),
        rect->w * cg_hudCompassSize->current.value,
        rect->h * cg_hudCompassSize->current.value,
        rect->horzAlign,
        rect->vertAlign,
        0.0f,
        0.0f,
        1.0f,
        1.0f,
        color,
        material);
}

// retail 0x4bf300 (16.0f PINNED), inlined in CG_DrawPlayerCompassPointers; name unknown
void CompassIconSize_4bf300(float *w, float *h)
{
    *w = cg_hudCompassSize->current.value * 16.0f;
    *h = cg_hudCompassSize->current.value * 16.0f;
}

static void CG_UpdateCursorHints(void)
{
    if (!cg->renderingThirdPerson)
    {
        if (cg->nextSnap->ps.cursorHint)
        {
            cg->cursorHintTime = cg->time;
            cg->cursorHintFade = cg_hintFadeTime->current.integer;
            cg->cursorHintIcon = cg->nextSnap->ps.cursorHint;
            cg->cursorHintString = cg->nextSnap->ps.cursorHintString;
        }
    }
}

static const char *CG_GetWeaponUseString(void)
{
    WeaponDef *weapDef;
    int weaponIndex;
    const char *displayString;
    char binding[0x100];

    weapDef = (WeaponDef *)BG_GetWeaponDef(cg->cursorHintIcon - 4);
    weaponIndex = cg->cursorHintIcon - 4;
    GetKeyBindingLocalizedString("+activate", binding);

    if (BG_DoesWeaponNeedSlot(weaponIndex) && !BG_GetEmptySlotForWeapon(&cg->predictedPlayerState, weaponIndex))
    {
        if (((WeaponDef *)BG_GetWeaponDef(cg->predictedPlayerState.weapon))->weapSlot == weapDef->weapSlot)
        {
            if (cg->predictedPlayerState.weapon == weaponIndex)
                return NULL;
        }
        else if ((signed char)cg->predictedPlayerState.weaponslots[weapDef->weapSlot] == weaponIndex)
        {
            return NULL;
        }
        displayString = UI_SafeTranslateString("PLATFORM_SWAPWEAPONS");
    }
    else
    {
        displayString = UI_SafeTranslateString("PLATFORM_PICKUPNEWWEAPON");
    }

    return UI_ReplaceConversionString(displayString, binding);
}

const char *CG_GetUseString(void)
{
    const char *hintString;
    char binding[0x100];

    hintString = CL_GetConfigString(cgArray[0].cursorHintString + 0x4fe);
    if (!hintString || !*hintString) {
        return 0;
    }

    if (!GetKeyBindingLocalizedString("+activate", binding)) {
        I_strncpyz(binding, UI_SafeTranslateString("KEY_USE"), sizeof(binding));
    }

    return UI_ReplaceConversionString(SEH_LocalizeTextMessage(hintString, "Hint String", (msgLocErrType_t)0), binding);
}

static void CG_DrawCursorhint(const rectDef_t *rect, struct Font_s *font, float fontscale, int textStyle)
{
    MaterialHandle hintIcon;
    float *fadeColor;
    float scale;
    float halfscale;
    float widthScale;
    float widthOfs;
    const char *text;
    float textWidth;
    float textHeight;
    float x;
    float y;
    char binding[0x100];

    if (!cg_cursorHints->current.integer)
        return;

    CG_UpdateCursorHints();

    hintIcon = cgs->media.hintMaterials[cg->cursorHintIcon];
    if (!hintIcon)
        return;

    fadeColor = CG_FadeColor(cg->cursorHintTime, cg->cursorHintFade, 100);
    if (!fadeColor)
    {
        cg->cursorHintIcon = 0;
        return;
    }

    Controls_GetConfig();

    widthScale = 1.0f;
    widthOfs = 0.0f;

    if (cg_cursorHints->current.integer == 3)
        fadeColor[3] *= (sinf((float)cg->time / 150.0f) + 1.0f) * 0.5f;

    if (cg_cursorHints->current.integer >= 3)
    {
        halfscale = 0.0f;
        scale = 0.0f;
    }
    else
    {
        if (cg_cursorHints->current.integer == 2)
            scale = (float)(cg->cursorHintTime % 1000) / 100.0f;
        else
            scale = 10.0f * ((sinf((float)cg->time / 150.0f) + 1.0f) * 0.5f);
        halfscale = scale * 0.5f;
    }

    if (cg->cursorHintIcon >= 5 && cg->cursorHintIcon <= 132)
    {
        if (((WeaponDef *)BG_GetWeaponDef(cg->cursorHintIcon - 4))->bWideListIcon)
        {
            widthScale = 2.0f;
            widthOfs = rect->w * -0.5f;
        }

        if (((WeaponDef *)BG_GetWeaponDef(cg->cursorHintIcon - 4))->weapClass == 7)
        {
            if (cg->cursorHintString < 0)
                goto draw_icon;
            text = CG_GetUseString();
        }
        else
        {
            text = CG_GetWeaponUseString();
        }
    }
    else if (cg->cursorHintString >= 0)
    {
        text = CG_GetUseString();
    }
    else if (cg->cursorHintIcon == 3)
    {
        GetKeyBindingLocalizedString("+activate", binding);
        text = UI_ReplaceConversionString(UI_SafeTranslateString("PLATFORM_PICKUPHEALTH"), binding);
    }
    else
    {
        goto draw_icon;
    }

    if (text && *text)
    {
        textWidth = (float)UI_TextWidth(text, 0, font, fontscale);
        textHeight = (float)UI_TextHeight(font, fontscale);
        x = (widthScale * rect->w + textWidth + scale) * -0.5f;
        y = rect->y - rect->h * 0.5f;
        UI_DrawText(text, 0x7fffffff, font, x, textHeight * 0.5f + rect->y,
                    rect->horzAlign, rect->vertAlign, fontscale, fadeColor, textStyle);
        UI_DrawHandlePic(x + textWidth, y, widthScale * rect->w + scale, rect->h + scale, rect->horzAlign,
                         rect->vertAlign, fadeColor, hintIcon);
        return;
    }

draw_icon:
    UI_DrawHandlePic(rect->x - (halfscale + rect->w + widthOfs) * 0.5f, rect->y - halfscale, widthScale * rect->w + scale,
                     rect->h + scale, rect->horzAlign, rect->vertAlign, fadeColor, hintIcon);
}

void CG_DrawHoldBreathHint(const rectDef_t *rect, struct Font_s *font, float fontscale, int textStyle)
{
    playerState_t *ps;
    WeaponDef *weaponDef;
    char binding[256];
    const char *text;
    int textWidth;
    float x;

    if (!cg_drawBreathHint->current.enabled) {
        return;
    }

    ps = &cg->predictedPlayerState;
    if (ps->pm_flags & 0x4000) {
        return;
    }

    weaponDef = (WeaponDef *)BG_GetWeaponDef(BG_GetViewmodelWeaponIndex(ps));
    if (!weaponDef->overlayReticle || weaponDef->weapClass == 9) {
        return;
    }

    if (ps->fWeaponPosFrac != 1.0f) {
        return;
    }

    Controls_GetConfig();
    if (!GetKeyBindingLocalizedString("+holdbreath", binding)) {
        if (!GetKeyBindingLocalizedString("+melee_breath", binding)) {
            GetKeyBindingLocalizedString("+breath_binoculars", binding);
        }
    }

    text = UI_ReplaceConversionString(UI_SafeTranslateString("PLATFORM_HOLD_BREATH"), binding);
    textWidth = UI_TextWidth(text, 0, font, fontscale);
    x = rect->x - (float)I_fround(textWidth * 0.5f);

    UI_DrawText(
        text,
        0x7fffffff,
        font,
        x,
        rect->y,
        rect->horzAlign,
        rect->vertAlign,
        fontscale,
        color,
        textStyle);
}

void CG_DrawMantleHint(const rectDef_t *rect, struct Font_s *font, float fontscale, int textStyle)
{
    char binding[0x100];
    const char *text;
    float length;
    float height;
    float x;

    if (!cg_drawMantleHint->current.enabled) {
        return;
    }

    if (!(*(byte *)&cg->predictedPlayerState.mantleState.flags & 8)) {
        return;
    }

    Controls_GetConfig();
    if (!GetKeyBindingLocalizedString("+gostand", binding)) {
        GetKeyBindingLocalizedString("+moveup", binding);
    }

    text = UI_ReplaceConversionString(UI_SafeTranslateString("PLATFORM_MANTLE"), binding);
    length = (float)UI_TextWidth(text, 0, font, fontscale);
    height = (float)UI_TextHeight(font, fontscale);
    x = (length + rect->w) * -0.5f + rect->x;

    UI_DrawText(
        text,
        0x7fffffff,
        font,
        x,
        height * 0.5f + rect->y,
        rect->horzAlign,
        rect->vertAlign,
        fontscale,
        color,
        textStyle);

    UI_DrawHandlePic(
        x + length,
        -(rect->h * 0.5f) + rect->y,
        rect->w,
        rect->h,
        rect->horzAlign,
        rect->vertAlign,
        color,
        cgs->media.mantleHint);
}

const char *CG_GetTranslatedLocationString(int iLocation)
{
    const char *p;
    const char *localized;

    p = CL_GetConfigString(iLocation + 0x2e);
    if (!p || !*p) {
        p = "CGAME_UNKNOWN";
    }

    localized = SEH_StringEd_GetString(p);
    if (localized) {
        return localized;
    }

    if (loc_warnings->current.enabled) {
        if (loc_warningsAsErrors->current.enabled) {
            Com_Error(ERR_LOCALIZATION, "Could not translate map location string \"%s\"", p);
        } else {
            Com_Printf("^3WARNING: Could not translate map location string \"%s\"\n", p);
        }

        strcpy(szErrorString, "^1UNLOCALIZED(^7");
        I_strncat(szErrorString, sizeof(szErrorString), p);
        I_strncat(szErrorString, sizeof(szErrorString), "^1)^7");
        return szErrorString;
    }

    I_strncpyz(szErrorString, p, sizeof(szErrorString));
    return szErrorString;
}

static void CG_DrawPlayerLocation(const rectDef_t *rect, struct Font_s *font, float scale, vec_t *color, int textStyle)
{
    clientInfo_t *ci;
    const char *s;

    ci = &cg->bgs.clientinfo[cg->nextSnap->ps.clientNum];
    if (ci->infoValid)
    {
        s = CG_GetTranslatedLocationString(ci->location);
        UI_DrawText(s, 0x7fffffff, font, rect->x, rect->y + rect->h, rect->horzAlign, rect->vertAlign, scale, color, textStyle);
    }
}

static void __attribute_regparm__(3)
    CG_DrawScore(int team, const rectDef_t *rect, struct Font_s *font, float scale, vec_t *color, MaterialHandle material, int textStyle)
{
    char scoreText[16];
    int textWidth;
    (void)material;
    if (cgs->teamScores[team] == -9999) {
        Com_sprintf(scoreText, sizeof(scoreText), "-");
    } else {
        Com_sprintf(scoreText, sizeof(scoreText), "%i", cgs->teamScores[team]);
    }
    textWidth = UI_TextWidth(scoreText, 0, font, scale);
    UI_DrawText(
        scoreText,
        0x7fffffff,
        font,
        rect->x + rect->w - (float)textWidth,
        rect->y + rect->h,
        rect->horzAlign,
        rect->vertAlign,
        scale,
        color,
        textStyle);
}

static void CG_DrawTeamColor(const rectDef_t *rect, vec_t *color)
{
    if (cg->bgs.clientinfo[cg->nextSnap->ps.clientNum].infoValid)
        CG_DrawTeamBackground(rect->x, rect->y, rect->w, rect->h, color[3], cg->bgs.clientinfo[cg->nextSnap->ps.clientNum].team);
}

const char *CG_GetKillerText(void)
{
    const char *s = "";

    if (cg->killerName[0])
        s = va("Fragged by %s", cg->killerName);
    return s;
}

const char *CG_GameTypeString(void)
{
    return cgs->gametype;
}

static void CG_DrawKiller(const rectDef_t *rect, struct Font_s *font, float scale, vec_t *color, int textStyle)
{
    const char *s;
    float x;

    if (cg->killerName[0])
    {
        s = va("Fragged by %s", cg->killerName);
        x = (rect->w - (float)UI_TextWidth(s, 0, font, scale)) * 0.5f + rect->x;
        UI_DrawText(s, 0x7fffffff, font, x, rect->y + rect->h, rect->horzAlign, rect->vertAlign, scale, color, textStyle);
    }
}

static void CG_DrawPlaceScore(int team, const rectDef_t *rect, struct Font_s *font, float scale, vec_t *color, int textStyle)
{
    if (cgs->teamScores[team] != -9999)
        UI_DrawText(va("%2i", cgs->teamScores[team]), 0x7fffffff, font, rect->x, rect->y,
                    rect->horzAlign, rect->vertAlign, scale, color, textStyle);
}

static void CG_DrawGameType(const rectDef_t *rect, struct Font_s *font, float scale, vec_t *color, int textStyle)
{
    UI_DrawText(CG_GameTypeString(), 0x7fffffff, font, rect->x, rect->y + rect->h,
                rect->horzAlign, rect->vertAlign, scale, color, textStyle);
}

static void CG_DrawHudPic(const rectDef_t *rect, MaterialHandle material, const vec_t *color)
{
    CL_DrawStretchPic(rect->x, rect->y, rect->w, rect->h, rect->horzAlign, rect->vertAlign, 0.0f, 0.0f, 1.0f, 1.0f, color, material);
}

void CG_OwnerDraw(float x, float y, float w, float h, int horzAlign, int vertAlign, float text_x, float text_y, int ownerDraw, int ownerDrawFlags, int align, float special, struct Font_s *font, float scale, vec_t *color, MaterialHandle material, int textStyle)
{
    rectDef_t rect;

    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
    rect.horzAlign = horzAlign;
    rect.vertAlign = vertAlign;

    switch (ownerDraw)
    {
    case CG_PLAYER_AMMO_BACKDROP:
        CG_DrawPlayerAmmoBackdrop(&rect, material, color);
        break;
    case CG_PLAYER_AMMO_VALUE:
        CG_DrawPlayerAmmoValue(&rect, font, scale, color, textStyle);
        break;
    case CG_HOLD_BREATH_HINT:
        CG_DrawHoldBreathHint(&rect, font, scale, textStyle);
        break;
    case CG_MANTLE_HINT:
        CG_DrawMantleHint(&rect, font, scale, textStyle);
        break;
    case CG_CURSORHINT:
        CG_DrawCursorhint(&rect, font, scale, textStyle);
        break;
    case CG_PLAYER_AMMOCLIP_VALUE:
        CG_DrawPlayerAmmoValue(&rect, font, scale, color, textStyle);
        break;
    case CG_PLAYER_STANCE:
        CG_DrawPlayerStance(&rect, color, font, scale, textStyle);
        break;
    case CG_RED_SCORE:
        CG_DrawScore(0, &rect, font, scale, color, material, textStyle);
        break;
    case CG_BLUE_SCORE:
        CG_DrawScore(1, &rect, font, scale, color, material, textStyle);
        break;
    case CG_PLAYER_LOCATION:
        CG_DrawPlayerLocation(&rect, font, scale, color, textStyle);
        break;
    case CG_TEAM_COLOR:
        CG_DrawTeamColor(&rect, color);
        break;
    case CG_GAME_TYPE:
        CG_DrawGameType(&rect, font, scale, color, textStyle);
        break;
    case CG_KILLER:
        CG_DrawKiller(&rect, font, scale, color, textStyle);
        break;
    case CG_PLAYER_WEAPON_NAME:
        CG_DrawPlayerWeaponName(&rect, font, scale, color, textStyle);
        break;
    case CG_PLAYER_WEAPON_NAME_BACK:
        CG_DrawPlayerWeaponNameBack(&rect, font, scale, color, material);
        break;
    // retail jump table has 76 and 77 as their own empty case
    case 76:
    case 77:
        return;
    case CG_PLAYER_WEAPON_MODE_ICON:
        CG_DrawPlayerWeaponModeIcon(&rect, color);
        break;
    case CG_PLAYER_COMPASS:
        CG_DrawPlayerCompass(&rect, material, color);
        break;
    case CG_PLAYER_COMPASS_BACK:
        CG_DrawPlayerCompassBack(&rect, material, color);
        break;
    case CG_PLAYER_COMPASS_POINTERS:
        CG_DrawPlayerCompassPointers(&rect, color);
        break;
    case CG_PLAYER_BAR_HEALTH:
        CG_DrawPlayerBarHealth(&rect, material, color);
        break;
    case CG_PLAYER_BAR_HEALTH_BACK:
        CG_DrawPlayerBarHealthBack(&rect, material, color);
        break;
    case CG_PLAYER_LOW_HEALTH_OVERLAY:
        CG_DrawPlayerLowHealthOverlay(&rect, material, color);
        break;
    case CG_PLAYER_COMPASS_FRIENDS:
        CG_DrawCompassFriendlies(&rect, material, color);
        break;
    case CG_1STPLACE:
        CG_DrawPlaceScore(0, &rect, font, scale, color, textStyle);
        break;
    case CG_2NDPLACE:
        CG_DrawPlaceScore(1, &rect, font, scale, color, textStyle);
        break;
    case CG_DRAW_SHADER:
        CG_DrawHudPic(&rect, material, color);
        break;
    case CG_OFFHAND_WEAPON_ICON_FRAG:
        CG_DrawOffHandIcon(&rect, scale, color, material, 1);
        break;
    case CG_OFFHAND_WEAPON_AMMO_FRAG:
        CG_DrawOffHandAmmo(&rect, font, scale, color, textStyle, 1);
        break;
    case CG_OFFHAND_WEAPON_NAME_FRAG:
        CG_DrawOffHandName(&rect, font, scale, color, textStyle, 1);
        break;
    case CG_OFFHAND_WEAPON_SELECT_FRAG:
        CG_DrawOffHandHighlight(&rect, scale, color, material, 1);
        break;
    case CG_OFFHAND_WEAPON_ICON_SMOKE:
        CG_DrawOffHandIcon(&rect, scale, color, material, 2);
        break;
    case CG_OFFHAND_WEAPON_AMMO_SMOKE:
        CG_DrawOffHandAmmo(&rect, font, scale, color, textStyle, 2);
        break;
    case CG_OFFHAND_WEAPON_NAME_SMOKE:
        CG_DrawOffHandName(&rect, font, scale, color, textStyle, 2);
        break;
    case CG_OFFHAND_WEAPON_SELECT_SMOKE:
        CG_DrawOffHandHighlight(&rect, scale, color, material, 2);
        break;
    }
}

int CG_KeyInterceptEvent(int key, qboolean down)
{
    if (!down || !CG_ScoreboardDisplayed()) {
        return 0;
    }

    if (key == 0xce || key == 0xa4 || key == 0xb8) {
        CG_ScrollScoreboardUp();
        return 1;
    }

    if (key == 0xcd || key == 0xa3 || key == 0xbe) {
        CG_ScrollScoreboardDown();
        return 1;
    }

    return 0;
}

// retail 0x4c08a0, unreferenced; Q3 CG_ClientNumFromName (SUPPORTED)
int CG_ClientNumFromName(const char *p)
{
    int i;

    for (i = 0; i < cgs->maxclients; i++) {
        if (cg->bgs.clientinfo[i].infoValid && !I_stricmp(cg->bgs.clientinfo[i].name, p))
            return i;
    }
    return -1;
}

extern displayContextDef_t cgDC;
extern qboolean Menus_OpenByName(displayContextDef_t *dc, const char *p);

// retail 0x4c0900, unreferenced; Q3 CG_ShowResponseHead (SUPPORTED)
void CG_ShowResponseHead(void)
{
    Menus_OpenByName(&cgDC, "voiceMenu");
    cg->voiceTime = cg->time;
}

// retail 0x4c0940, unreferenced; Q3 CG_GetTeamColor (SUPPORTED)
void CG_GetTeamColor(vec4_t *color)
{
    if (cg->bgs.clientinfo[cg->nextSnap->ps.clientNum].infoValid)
    {
        if (cg->bgs.clientinfo[cg->nextSnap->ps.clientNum].team == 1)
        {
            (*color)[0] = 1.0f;
            (*color)[1] = 0.0f;
            (*color)[2] = 0.0f;
            (*color)[3] = 0.25f;
            return;
        }
        if (cg->bgs.clientinfo[cg->nextSnap->ps.clientNum].team == 2)
        {
            (*color)[0] = (*color)[1] = 0.0f;
            (*color)[2] = 1.0f;
            (*color)[3] = 0.25f;
            return;
        }
    }
    (*color)[0] = 0.0f;
    (*color)[1] = 0.17f;
    (*color)[2] = 0.0f;
    (*color)[3] = 0.25f;
}

void CG_ArchiveState(MemoryFile *memFile)
{

    ((MemoryFileArchiveProc)memFile->archiveProc)(memFile, 4, (byte *)&cg->healthFadeTime);
    ((MemoryFileArchiveProc)memFile->archiveProc)(memFile, 4, (byte *)&cg->ammoFadeTime);
    ((MemoryFileArchiveProc)memFile->archiveProc)(memFile, 4, (byte *)&cg->stanceFadeTime);
    ((MemoryFileArchiveProc)memFile->archiveProc)(memFile, 4, (byte *)&cg->compassFadeTime);
    ((MemoryFileArchiveProc)memFile->archiveProc)(memFile, 4, (byte *)&cg->offhandFadeTime);
    ((MemoryFileArchiveProc)memFile->archiveProc)(memFile, 4, (byte *)&cg->drawHud);
    ((MemoryFileArchiveProc)memFile->archiveProc)(memFile, 0x400, cg->objectiveText);
    ((MemoryFileArchiveProc)memFile->archiveProc)(memFile, 0x100, cg->scriptMainMenu);
}


const unsigned char __ZZ17CG_DrawMantleHintPK9rectDef_sP6Font_sfiE5color[16] = { 0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x80, 0x3f };

const unsigned char __ZZ21CG_DrawHoldBreathHintPK9rectDef_sP6Font_sfiE5color[896] = {
    0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x80, 0x3f,
    0xff, 0xff, 0xff, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xb6, 0xad, 0x18, 0x00, 0xe0, 0xad, 0x18, 0x00, 0xab, 0xad, 0x18, 0x00, 0xab, 0xad, 0x18, 0x00,
    0xab, 0xad, 0x18, 0x00, 0xab, 0xad, 0x18, 0x00, 0xab, 0xad, 0x18, 0x00, 0xab, 0xad, 0x18, 0x00,
    0xab, 0xad, 0x18, 0x00, 0xab, 0xad, 0x18, 0x00, 0xab, 0xad, 0x18, 0x00, 0xab, 0xad, 0x18, 0x00,
    0xab, 0xad, 0x18, 0x00, 0xab, 0xad, 0x18, 0x00, 0xab, 0xad, 0x18, 0x00, 0xda, 0xae, 0x18, 0x00,
    0xab, 0xad, 0x18, 0x00, 0xab, 0xad, 0x18, 0x00, 0xab, 0xad, 0x18, 0x00, 0xab, 0xad, 0x18, 0x00,
    0xab, 0xad, 0x18, 0x00, 0xab, 0xad, 0x18, 0x00, 0xf8, 0xbf, 0x18, 0x00, 0x4d, 0xbf, 0x18, 0x00,
    0xab, 0xad, 0x18, 0x00, 0xab, 0xad, 0x18, 0x00, 0xab, 0xad, 0x18, 0x00, 0xab, 0xad, 0x18, 0x00,
    0x78, 0xbf, 0x18, 0x00, 0xe6, 0xbe, 0x18, 0x00, 0xab, 0xad, 0x18, 0x00, 0xab, 0xad, 0x18, 0x00,
    0xab, 0xad, 0x18, 0x00, 0xab, 0xad, 0x18, 0x00, 0x5c, 0xc1, 0x18, 0x00, 0xab, 0xad, 0x18, 0x00,
    0xab, 0xad, 0x18, 0x00, 0xab, 0xad, 0x18, 0x00, 0xab, 0xad, 0x18, 0x00, 0xab, 0xad, 0x18, 0x00,
    0xab, 0xad, 0x18, 0x00, 0xab, 0xad, 0x18, 0x00, 0xab, 0xad, 0x18, 0x00, 0xab, 0xad, 0x18, 0x00,
    0xab, 0xad, 0x18, 0x00, 0x26, 0xc0, 0x18, 0x00, 0xab, 0xad, 0x18, 0x00, 0xab, 0xad, 0x18, 0x00,
    0xab, 0xad, 0x18, 0x00, 0xab, 0xad, 0x18, 0x00, 0xab, 0xad, 0x18, 0x00, 0xab, 0xad, 0x18, 0x00,
    0xab, 0xad, 0x18, 0x00, 0xab, 0xad, 0x18, 0x00, 0xab, 0xad, 0x18, 0x00, 0xab, 0xad, 0x18, 0x00,
    0xab, 0xad, 0x18, 0x00, 0xab, 0xad, 0x18, 0x00, 0xab, 0xad, 0x18, 0x00, 0xab, 0xad, 0x18, 0x00,
    0xab, 0xad, 0x18, 0x00, 0xab, 0xad, 0x18, 0x00, 0xe9, 0xc0, 0x18, 0x00, 0xe9, 0xba, 0x18, 0x00,
    0xab, 0xad, 0x18, 0x00, 0x73, 0xbb, 0x18, 0x00, 0x83, 0xbb, 0x18, 0x00, 0xac, 0xbb, 0x18, 0x00,
    0xab, 0xad, 0x18, 0x00, 0xab, 0xad, 0x18, 0x00, 0xab, 0xad, 0x18, 0x00, 0xab, 0xad, 0x18, 0x00,
    0xab, 0xad, 0x18, 0x00, 0xab, 0xad, 0x18, 0x00, 0xe2, 0xaa, 0x18, 0x00, 0xc4, 0xbb, 0x18, 0x00,
    0xed, 0xbb, 0x18, 0x00, 0x5f, 0xbd, 0x18, 0x00, 0x0a, 0xaf, 0x18, 0x00, 0xe8, 0xaf, 0x18, 0x00,
    0x4d, 0xb1, 0x18, 0x00, 0x6b, 0xb1, 0x18, 0x00, 0xab, 0xad, 0x18, 0x00, 0xab, 0xad, 0x18, 0x00,
    0x8c, 0xb5, 0x18, 0x00, 0xab, 0xad, 0x18, 0x00, 0xab, 0xad, 0x18, 0x00, 0xab, 0xad, 0x18, 0x00,
    0xab, 0xad, 0x18, 0x00, 0xaa, 0xb5, 0x18, 0x00, 0xff, 0xb5, 0x18, 0x00, 0xab, 0xad, 0x18, 0x00,
    0xab, 0xad, 0x18, 0x00, 0xab, 0xad, 0x18, 0x00, 0xab, 0xad, 0x18, 0x00, 0xce, 0xb8, 0x18, 0x00,
    0xff, 0xb8, 0x18, 0x00, 0x17, 0xb9, 0x18, 0x00, 0x4f, 0xb9, 0x18, 0x00, 0x59, 0xb9, 0x18, 0x00,
    0x91, 0xb9, 0x18, 0x00, 0x9b, 0xb9, 0x18, 0x00, 0xcc, 0xb9, 0x18, 0x00, 0xab, 0xad, 0x18, 0x00,
    0xd6, 0xb9, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x0b, 0xc8, 0x18, 0x00, 0x2d, 0xc8, 0x18, 0x00, 0xa1, 0xc8, 0x18, 0x00, 0x87, 0xc8, 0x18, 0x00,
    0x75, 0xc8, 0x18, 0x00, 0x09, 0xc8, 0x18, 0x00, 0xf1, 0xc7, 0x18, 0x00, 0x47, 0xc8, 0x18, 0x00,
    0xef, 0xc8, 0x18, 0x00, 0x01, 0xc9, 0x18, 0x00, 0x75, 0xc9, 0x18, 0x00, 0x5b, 0xc9, 0x18, 0x00,
    0x49, 0xc9, 0x18, 0x00, 0xed, 0xc8, 0x18, 0x00, 0xdd, 0xc8, 0x18, 0x00, 0x1b, 0xc9, 0x18, 0x00,
    0xd8, 0xcc, 0x18, 0x00, 0xda, 0xcd, 0x18, 0x00, 0xb9, 0xcd, 0x18, 0x00, 0x8c, 0xcd, 0x18, 0x00,
    0x67, 0xcd, 0x18, 0x00, 0x66, 0xcc, 0x18, 0x00, 0x3a, 0xcd, 0x18, 0x00, 0x2a, 0xcc, 0x18, 0x00,
    0xb3, 0xcc, 0x18, 0x00, 0x0d, 0xcd, 0x18, 0x00, 0x7e, 0xce, 0x18, 0x00, 0x51, 0xce, 0x18, 0x00,
    0x2c, 0xce, 0x18, 0x00, 0xae, 0xcc, 0x18, 0x00, 0x07, 0xce, 0x18, 0x00, 0x72, 0xcc, 0x18, 0x00,
    0xff, 0xff, 0xff, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xc7, 0xe3, 0x19, 0x00, 0xdc, 0xe3, 0x19, 0x00, 0xb5, 0xe3, 0x19, 0x00, 0xf1, 0xe3, 0x19, 0x00,
    0x13, 0xe4, 0x19, 0x00, 0xb5, 0xe3, 0x19, 0x00, 0xb5, 0xe3, 0x19, 0x00, 0xb5, 0xe3, 0x19, 0x00,
    0xab, 0xe3, 0x19, 0x00, 0xb7, 0xe7, 0x19, 0x00, 0xcc, 0xe7, 0x19, 0x00, 0x7e, 0xe7, 0x19, 0x00,
    0xeb, 0xe7, 0x19, 0x00, 0x1a, 0xe8, 0x19, 0x00, 0x7e, 0xe7, 0x19, 0x00, 0x7e, 0xe7, 0x19, 0x00,
    0x7e, 0xe7, 0x19, 0x00, 0x6f, 0xe8, 0x19, 0x00, 0xda, 0xe8, 0x19, 0x00, 0x94, 0xe9, 0x19, 0x00,
    0x81, 0xe9, 0x19, 0x00, 0x5e, 0xe9, 0x19, 0x00, 0x4b, 0xe9, 0x19, 0x00, 0x2a, 0xe9, 0x19, 0x00,
    0x09, 0xe9, 0x19, 0x00, 0xed, 0xe8, 0x19, 0x00, 0xba, 0xe8, 0x19, 0x00, 0x8a, 0xf0, 0x19, 0x00,
    0x11, 0xf1, 0x19, 0x00, 0x84, 0xf0, 0x19, 0x00, 0xee, 0xf0, 0x19, 0x00, 0xc2, 0xf0, 0x19, 0x00,
    0x4a, 0xf0, 0x19, 0x00, 0xa1, 0xf0, 0x19, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xff, 0xff, 0xff, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xff, 0xff, 0xff, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

