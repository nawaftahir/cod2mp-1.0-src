#include "common_types.h"
#include "imports.h"
#include "bytematch.h"
#include "headers/PC/cgame_mp/cg_local.h"
#include <string.h>
extern weaponInfo_t cg_weaponsArray[];
extern scr_const_t scr_const;

extern dvar_t *Dvar_RegisterFloat(const char *dvarName, float value, float min, float max, unsigned short flags);
extern int BG_GetFirstAvailableOffhand(const playerState_t *ps, int offhandClass);
extern int BG_GetFirstEquippedOffhand(const playerState_t *ps, int offhandClass);
extern float CG_FadeHudMenu(const dvar_t *fadeDvar, int displayStartTime, int duration);
extern void UI_DrawText(const char *text, int maxChars, FontHandle font, float x, float y, int horzAlign, int vertAlign, float scale, const vec_t *color, int style);
extern const char *UI_SafeTranslateString(const char *str);
extern int CG_PlayEntitySoundAlias(int entNum, snd_alias_list_t *aliasList);
extern int CG_PlaySoundAlias(int entNum, const float *origin, snd_alias_list_t *aliasList);
extern qboolean CG_DObjGetWorldTagPos(const centity_t *cent, struct DObj_s *dobj, unsigned int tag, float *origin);
extern int CG_DObjGetViewModelTagPos(struct DObj_s *dobj, unsigned int tag, float *origin);
extern void BG_EvaluateTrajectory( const trajectory_t *tr, int atTime, vec3_t result );
extern WeaponDef * BG_GetWeaponDef(int iWeapon);
extern int BG_GetNumWeapons(void);
extern int BG_ClipForWeapon(int weaponIndex);
extern struct DObj_s * Com_GetClientDObj(int handle, int localClientNum);
extern void CG_MenuShowNotify(int type);
extern void UI_DrawHandlePic(float x, float y, float w, float h, int horzAlign, int vertAlign, const float *color, MaterialHandle material);
extern float floorf(float x);
extern float cosf(float x);
extern char *va( const char *format, ... );
extern void Com_Printf(const char *fmt, ...);

extern const dvar_t *hud_fade_offhand;

/* offhandStrings: migrated from the ILP32 data blob to typed C (re-lays-out per
 * target). Blob extent was 12 B of pointers + 20 B inter-symbol padding (next
 * symbol hintStrings distinct; indexed [weaponType] in 0..2). [0] is the empty
 * literal (literals32.c); the other two are in common.c. (x64 port Stage 2.) */
static const char *offhandStrings[3] = {
    "",
    "WEAPON_FRAGGRENADE",
    "WEAPON_SMOKEGRENADE",
};
static const dvar_t *hud_flash_time_offhand;
static const dvar_t *hud_flash_period_offhand;

COD2_ASSERT_FIELD(struct dvar_s, current,      0x08);
COD2_ASSERT_FIELD(WeaponDef,     offhandClass, 0x84);

void CG_OffhandRegisterDvars(void);
void CG_DrawOffHandName(rectDef_s *rect, struct Font_s *font, float scale, vec_t *color, int textStyle, int weaponType);
void CG_PrepOffHand(entityState_t *ent, int event, int eventParam);
void CG_UseOffHand(centity_t *cent, int event, int eventParam);
void CG_SetEquippedOffHand(int offHandIndex);
void CG_SwitchOffHandCmd(void);
void CG_DrawOffHandIcon(rectDef_s *rect, float scale, vec_t *color, MaterialHandle material, int weaponType);
void CG_DrawOffHandHighlight(rectDef_s *rect, float scale, vec_t *color, MaterialHandle material, int weaponType);
void CG_DrawOffHandAmmo(rectDef_s *rect, struct Font_s *font, float scale, vec_t *color, int textStyle, int weaponType);

void CG_UseOffHand(centity_t *cent, int event, int eventParam);

void CG_OffhandRegisterDvars(void)
{
    hud_flash_time_offhand = Dvar_RegisterFloat("hud_flash_time_offhand", 2.0f, 0.0f, 30.0f, 0x1001);
    hud_flash_period_offhand = Dvar_RegisterFloat("hud_flash_period_offhand", 0.5f, 0.0f, 30.0f, 0x1001);
}

static __forceinline int GetBestOffhandBody(int offhandClass)
{
    int newOffhand;

    newOffhand = BG_GetFirstAvailableOffhand(&cg->predictedPlayerState, offhandClass);
    if (!newOffhand)
        return BG_GetFirstEquippedOffhand(&cg->predictedPlayerState, offhandClass);
    return newOffhand;
}

// retail 0x4d1cc0, called from one site only
static __declspec(noinline) int GetBestOffhand(int offhandClass)
{
    return GetBestOffhandBody(offhandClass);
}

// retail 0x4d1ce0
static Bool IsOffHandDisplayVisible(void)
{
    return cg->predictedPlayerState.pm_type < 6;
}

void CG_DrawOffHandIcon(rectDef_s *rect, float scale, vec_t *color, MaterialHandle material, int weaponType)
{
    float drawColor[4];
    int weapon;
    MaterialHandle iconMaterial;

    if (!IsOffHandDisplayVisible())
        return;
    if (!GetBestOffhandBody(weaponType))
        return;

    drawColor[3] = CG_FadeHudMenu(hud_fade_offhand, cg->offhandFadeTime, I_fround(hud_fade_offhand->current.value * 1000.0f));
    if (drawColor[3] == 0.0f)
        return;

    *(int *)&drawColor[0] = *(int *)&color[0];
    *(int *)&drawColor[1] = *(int *)&color[1];
    *(int *)&drawColor[2] = *(int *)&color[2];

    weapon = cg->equippedOffHand;
    if (!weapon || BG_GetWeaponDef(weapon)->offhandClass != weaponType) {
        weapon = GetBestOffhand(weaponType);
        if (!weapon)
            return;
    }

    iconMaterial = ((cg_weaponsArray))[weapon].hHudIcon;

    UI_DrawHandlePic(rect->x, rect->y, rect->w,
                     rect->h, rect->horzAlign, rect->vertAlign,
                     drawColor, iconMaterial);
}

// retail 0x4d1df0
static void OffHandFlash(const float *baseColor, float *outColor)
{
    float flashTime;

    outColor[0] = baseColor[0];
    outColor[1] = baseColor[1];
    outColor[2] = baseColor[2];
    outColor[3] = baseColor[3];
    flashTime = (float)(cg->time - cg->offhandFlashTime) * 0.001f;
    if (flashTime < hud_flash_time_offhand->current.value)
        outColor[3] = baseColor[3] * ((cosf(flashTime / hud_flash_period_offhand->current.value * 6.2831855f) + 1.0f) * 0.5f);
}

// retail 0x4d1e60
static int CalcOffHandAmmo(int weaponType)
{
    int ammoCount;
    int weapCount;
    int i;

    ammoCount = 0;
    weapCount = BG_GetNumWeapons();
    for (i = 1; i <= weapCount; i++) {
        if ((cg->predictedPlayerState.weapons[i >> 5] & (1 << (i & 0x1f)))
            && ((WeaponDef *)BG_GetWeaponDef(i))->offhandClass == weaponType)
            ammoCount += cg->predictedPlayerState.ammoclip[BG_ClipForWeapon(i)];
    }
    return ammoCount;
}

void CG_DrawOffHandHighlight(rectDef_s *rect, float scale, vec_t *color, MaterialHandle material, int weaponType)
{
    float drawColor[4];
    float flashColor[4];

    if (!IsOffHandDisplayVisible())
        return;
    if (!GetBestOffhandBody(weaponType))
        return;
    if (!cg->equippedOffHand)
        return;

    drawColor[3] = CG_FadeHudMenu(hud_fade_offhand, cg->offhandFadeTime, I_fround(hud_fade_offhand->current.value * 1000.0f));
    if (drawColor[3] == 0.0f)
        return;
    if (weaponType != ((WeaponDef *)BG_GetWeaponDef(cg->equippedOffHand))->offhandClass)
        return;

    if (!CalcOffHandAmmo(weaponType)) {
        drawColor[0] = 0.89f;
        drawColor[1] = 0.18f;
        drawColor[2] = 0.01f;
    } else {
        *(int *)&drawColor[0] = *(int *)&color[0];
        *(int *)&drawColor[1] = *(int *)&color[1];
        *(int *)&drawColor[2] = *(int *)&color[2];
    }
    OffHandFlash(drawColor, flashColor);
    UI_DrawHandlePic(rect->x, rect->y, rect->w,
                     rect->h, rect->horzAlign, rect->vertAlign,
                     flashColor, material);
}

void CG_DrawOffHandAmmo(rectDef_s *rect, struct Font_s *font, float scale, vec_t *color, int textStyle, int weaponType)
{
    float drawColor[4];
    int ammoCount;
    char *text;

    if (!IsOffHandDisplayVisible())
        return;
    if (!GetBestOffhandBody(weaponType))
        return;

    drawColor[3] = CG_FadeHudMenu(hud_fade_offhand, cg->offhandFadeTime, I_fround(hud_fade_offhand->current.value * 1000.0f));
    if (drawColor[3] == 0.0f)
        return;

    ammoCount = CalcOffHandAmmo(weaponType);
    text = va("%i", ammoCount);
    if (!ammoCount) {
        drawColor[0] = 0.89f;
        drawColor[1] = 0.18f;
        drawColor[2] = 0.01f;
    } else {
        *(int *)&drawColor[0] = *(int *)&color[0];
        *(int *)&drawColor[1] = *(int *)&color[1];
        *(int *)&drawColor[2] = *(int *)&color[2];
    }

    UI_DrawText(text, 0x7fffffff, font,
                rect->x, rect->y,
                rect->horzAlign, rect->vertAlign,
                scale, drawColor, textStyle);
}

void CG_DrawOffHandName(rectDef_s *rect, struct Font_s *font, float scale, vec_t *color, int textStyle, int weaponType)
{
    const dvar_t *hud;
    float drawColor[4];
    const char *text;
    int weapon;
    int time;

    if (cg->predictedPlayerState.pm_type >= 6)
        return;

    weapon = BG_GetFirstAvailableOffhand( (const playerState_t *)((void *)&cg->predictedPlayerState), weaponType);
    if (weapon == 0) {
        weapon = BG_GetFirstEquippedOffhand( (const playerState_t *)((void *)&cg->predictedPlayerState), weaponType);
        if (weapon == 0)
            return;
    }

    hud = hud_fade_offhand;

    time = I_fround(hud->current.value * 1000.0f);
    drawColor[3] = CG_FadeHudMenu( (const dvar_t *)((void *)hud), cg->offhandFadeTime, time);

    if (drawColor[3] == 0.0f)
        return;

    *(int *)&drawColor[0] = *(int *)&color[0];
    *(int *)&drawColor[1] = *(int *)&color[1];
    *(int *)&drawColor[2] = *(int *)&color[2];

    text = UI_SafeTranslateString(offhandStrings[weaponType]);

    UI_DrawText(text, 0x7fffffff, font,
                rect->x, rect->y,
                rect->horzAlign, rect->vertAlign,
                scale, drawColor, textStyle);
}

void CG_PrepOffHand(entityState_t *ent, int event, int eventParam)
{
    weaponInfo_s *wi;

    wi = &((cg_weaponsArray))[eventParam];

    /* pullbackSound is a snd_alias_list_t*; pass the whole pointer (see the note
     * in CG_UseOffHand -- the `(int)(intptr_t)` truncation crashes on x64). */
    if (wi->pullbackSound != NULL) {
        CG_PlayEntitySoundAlias(ent->number, wi->pullbackSound);
    }
}

void CG_UseOffHand(centity_t *cent, int event, int eventParam)
{
    weaponInfo_s *wi;
    int clientNum;
    float origin[3];
    struct DObj_s *dobj;

    wi = &((cg_weaponsArray))[eventParam];

    /* flashSound is a snd_alias_list_t* -- test the whole pointer (the decomp's
     * `(int)(intptr_t)` truncated it to 32 bits, which both mis-detects NULL and,
     * below, passed a truncated/garbage list pointer to CG_PlaySoundAlias -> crash
     * in Com_PickSoundAliasFromList reading list->head on x64). */
    if (wi->flashSound == NULL)
        return;

    clientNum = cent->nextState.number;

    if (clientNum == cg->nextSnap->ps.clientNum) {

        struct DObj_s *viewModel = (struct DObj_s *)wi->viewModelDObj;
        if (viewModel != NULL) {
            if (CG_DObjGetViewModelTagPos(viewModel, scr_const.tag_flash, origin)) {
                goto play_sound;
            }
        }
    } else {

        dobj = Com_GetClientDObj(clientNum, cent->localClientNum);
        if (dobj != NULL) {
            if (CG_DObjGetWorldTagPos(cent, dobj, scr_const.tag_flash, origin)) {
                goto play_sound;
            }
        }
    }

    BG_EvaluateTrajectory(&cent->nextState.pos, cg->time, origin);

play_sound:
    CG_PlaySoundAlias(cent->nextState.number, origin, wi->flashSound);
}

void CG_SetEquippedOffHand(int offHandIndex)
{
    cg->equippedOffHand = offHandIndex;
    CG_MenuShowNotify(4);
}

void CG_SwitchOffHandCmd(void)
{
    int currentWeapon;
    void *weapDef;
    int newWeapon;

    currentWeapon = cg->equippedOffHand;

    if (currentWeapon == 0)
        return;

    weapDef = BG_GetWeaponDef(currentWeapon);
    newWeapon = BG_GetFirstAvailableOffhand( (const playerState_t *)((void *)&cg->predictedPlayerState), ((WeaponDef *)weapDef)->offhandClass);

    if (newWeapon == 0)
        return;

    CG_SetEquippedOffHand(newWeapon);
}

