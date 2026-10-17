#include "common_types.h"
#include "imports.h"
#include "headers/PC/cgame_mp/cg_local.h"
#include <stdlib.h>
#include <string.h>

extern int CL_GetKeyCatchers(void);
extern Bool CL_GetDisplayHUDWithKeycatchUI(void);
extern float CL_NormalizedTextScale(FontHandle font, float fontScale);
extern void CalcScreenPlacement(float *x, float *y, float *w, float *h, int horzAlign, int vertAlign);
extern void CL_DrawTextPhysical(const char *text, int maxChars, FontHandle font, float x, float y, float scaleX, float scaleY, const float *color, int style);
extern void CL_DrawStretchPicPhysical(float x, float y, float w, float h, float s0, float t0, float s1, float t1, const float *color, Material *material);
extern void CG_DrawRotatedPicPhysical(float x, float y, float w, float h, float angle, const float *color, Material *material);
extern void CL_DrawSprite(Material *material, const byte *rgbaColor, const vec_t *pos,
                          float radius, float minScreenRadius, int renderFxFlags);
extern FontHandle UI_GetFontHandle(int scriptIndex, float fontScale);
extern int UI_TextWidth(const char *text, int maxChars, FontHandle font, float fontScale);
extern int UI_TextHeight(FontHandle font, float fontScale);
extern void CG_SafeTranslateHudElemString(int label, char *dst);
extern qboolean CG_ServerMaterialName(int index, char *materialName, int maxLen);
extern MaterialHandle CL_RegisterMaterial(const char *name, int imageTrack);
extern MaterialHandle CL_RegisterMaterialNoMip(const char *name, int imageTrack);
extern const char *CL_GetConfigString(int index);
extern void I_strncpyz(char *dest, const char *src, int destsize);
extern void I_strncat(char *dest, int maxlen, const char *src);
extern char *va( const char *format, ... );
extern float AngleNormalize360( float angle );

Bool CG_AreHudElemsHidden(void)
{
    return !cg->drawHud || ((CL_GetKeyCatchers() & 8) && !CL_GetDisplayHUDWithKeycatchUI()) ? 1 : 0;
}

float HudElemStringWidth(const char *string, const cg_hudelem_t *cghe)
{
    return (float)UI_TextWidth(string, 0, cghe->font, cghe->fontScale);
}

int GetHudElemTime(const hudelem_t *elem)
{
    int time;

    switch (elem->type)
    {
    case HE_TYPE_TIMER_DOWN:
        time = elem->time - cg->time + 999;
        break;
    case HE_TYPE_TENTHS_TIMER_DOWN:
        time = elem->time - cg->time + 99;
        break;
    case HE_TYPE_CLOCK_DOWN:
        time = elem->time - cg->time;
        break;
    case HE_TYPE_TIMER_UP:
    case HE_TYPE_TENTHS_TIMER_UP:
    case HE_TYPE_CLOCK_UP:
        time = cg->time - elem->time;
        break;
    default:
        return 0;
    }
    if (time < 0)
        time = 0;
    return time;
}

const char *HudElemTimerString(const hudelem_t *elem)
{
    int time;
    int hours;
    int minutes;
    int seconds;

    time = GetHudElemTime(elem) / 1000;
    hours = time / 3600;
    time -= hours * 3600;
    minutes = time / 60;
    seconds = time - minutes * 60;
    if (hours)
        return va("%i:%02i:%02i", hours, minutes, seconds);
    else
        return va("%i:%02i", minutes, seconds);
}

const char *HudElemTenthsTimerString(const hudelem_t *elem)
{
    int time;
    int hours;
    int minutes;
    int seconds;
    int tenths;

    time = GetHudElemTime(elem) / 100;
    hours = time / 36000;
    time -= hours * 36000;
    minutes = time / 600;
    time -= minutes * 600;
    seconds = time / 10;
    tenths = time - seconds * 10;
    if (hours)
        return va("%i:%02i:%02i.%i", hours, minutes, seconds, tenths);
    else
        return va("%i:%02i.%i", minutes, seconds, tenths);
}

float HudElemMaterialSpecifiedSize(int sizeVirtual, const cg_hudelem_t *cghe)
{
    if (sizeVirtual)
        return (float)sizeVirtual;
    return cghe->fontHeight;
}

float HudElemMaterialWidth(const hudelem_t *elem, const cg_hudelem_t *cghe)
{
    float width;
    float fromWidth;
    float lerp;
    int deltaTime;

    width = HudElemMaterialSpecifiedSize(elem->width, cghe);
    if (elem->scaleTime > 0)
    {
        deltaTime = cg->time - elem->scaleStartTime;
        if (deltaTime < elem->scaleTime)
        {
            fromWidth = HudElemMaterialSpecifiedSize(elem->fromWidth, cghe);
            lerp = (float)deltaTime * (1.0f / (float)elem->scaleTime);
            if (lerp < 0.0f)
                lerp = 0.0f;
            else if (lerp > 1.0f)
                lerp = 1.0f;
            width = (width - fromWidth) * lerp + fromWidth;
        }
    }
    return width;
}

float HudElemMaterialHeight(const hudelem_t *elem, const cg_hudelem_t *cghe)
{
    float height;
    float fromHeight;
    float lerp;
    int deltaTime;

    height = HudElemMaterialSpecifiedSize(elem->height, cghe);
    if (elem->scaleTime > 0)
    {
        deltaTime = cg->time - elem->scaleStartTime;
        if (deltaTime < elem->scaleTime)
        {
            fromHeight = HudElemMaterialSpecifiedSize(elem->fromHeight, cghe);
            lerp = (float)deltaTime * (1.0f / (float)elem->scaleTime);
            if (lerp < 0.0f)
                lerp = 0.0f;
            else if (lerp > 1.0f)
                lerp = 1.0f;
            height = (height - fromHeight) * lerp + fromHeight;
        }
    }
    return height;
}

float HudElemWidth(const hudelem_t *elem, const cg_hudelem_t *cghe)
{
    switch (elem->type)
    {
    case HE_TYPE_TEXT:
    case HE_TYPE_VALUE:
    case HE_TYPE_PLAYERNAME:
    case HE_TYPE_MAPNAME:
    case HE_TYPE_GAMETYPE:
    case HE_TYPE_TIMER_DOWN:
    case HE_TYPE_TIMER_UP:
    case HE_TYPE_TENTHS_TIMER_DOWN:
    case HE_TYPE_TENTHS_TIMER_UP:
        return cghe->textWidth + cghe->labelWidth;
    case HE_TYPE_MATERIAL:
    case HE_TYPE_CLOCK_DOWN:
    case HE_TYPE_CLOCK_UP:
        return HudElemMaterialWidth(elem, cghe) + cghe->labelWidth;
    case HE_TYPE_WAYPOINT:
        return (float)elem->width;
    default:
        return 0.0f;
    }
}

float HudElemHeight(const hudelem_t *elem, const cg_hudelem_t *cghe)
{
    float height;

    switch (elem->type)
    {
    case HE_TYPE_TEXT:
    case HE_TYPE_VALUE:
    case HE_TYPE_PLAYERNAME:
    case HE_TYPE_MAPNAME:
    case HE_TYPE_GAMETYPE:
    case HE_TYPE_TIMER_DOWN:
    case HE_TYPE_TIMER_UP:
    case HE_TYPE_TENTHS_TIMER_DOWN:
    case HE_TYPE_TENTHS_TIMER_UP:
        height = cghe->fontHeight;
        break;
    case HE_TYPE_CLOCK_DOWN:
    case HE_TYPE_CLOCK_UP:
        height = HudElemMaterialHeight(elem, cghe);
        break;
    case HE_TYPE_MATERIAL:
    case HE_TYPE_WAYPOINT:
        height = (float)elem->height;
        break;
    default:
        return 0.0f;
    }
    if (cghe->hudElemLabel && height < cghe->fontHeight)
        return cghe->fontHeight;
    return height;
}

float AlignHudElemX(int alignOrg, float x, float width)
{
    switch ((alignOrg >> 2) & 3)
    {
    case 0:
        return x;
    case 1:
        return x - width * 0.5f;
    case 2:
        return x - width;
    default:
        return x;
    }
}

float AlignHudElemY(int alignOrg, float y, float height)
{
    switch (alignOrg & 3)
    {
    case 0:
        return y;
    case 1:
        return y - height * 0.5f;
    case 2:
        return y - height;
    default:
        return y;
    }
}

void GetHudElemOrg(int alignOrg, int alignScreen, float x, float y, float width, float height, float *orgX, float *orgY)
{
    float w;
    float h;

    *orgX = AlignHudElemX(alignOrg, x, width);
    *orgY = AlignHudElemY(alignOrg, y, height);
    w = 0.0f;
    h = 0.0f;
    CalcScreenPlacement(orgX, orgY, &w, &h, (alignScreen >> 3) & 7, alignScreen & 7);
}

float HudElemMovementFrac(const hudelem_t *elem)
{
    int time;

    if (elem->moveTime <= 0)
        return 1.0f;
    time = cg->time - elem->moveStartTime;
    if (time <= 0)
        return 0.0f;
    if (time >= elem->moveTime)
        return 1.0f;
    return (float)time / (float)elem->moveTime;
}

void SetHudElemPos(const hudelem_t *elem, cg_hudelem_t *cghe)
{
    float from[2];
    float to[2];
    float lerp;

    cghe->z = elem->z;
    lerp = HudElemMovementFrac(elem);
    if (lerp == 1.0f)
    {
        GetHudElemOrg(elem->alignOrg, elem->alignScreen, elem->x, elem->y, cghe->width, cghe->height, &cghe->x, &cghe->y);
        cghe->x = (float)I_fround(cghe->x);
        cghe->y = (float)I_fround(cghe->y);
    }
    else
    {
        GetHudElemOrg(elem->fromAlignOrg, elem->fromAlignScreen, elem->fromX, elem->fromY, cghe->width, cghe->height, &from[0], &from[1]);
        GetHudElemOrg(elem->alignOrg, elem->alignScreen, elem->x, elem->y, cghe->width, cghe->height, &to[0], &to[1]);
        cghe->x = (to[0] - from[0]) * lerp + from[0];
        cghe->y = (to[1] - from[1]) * lerp + from[1];
    }
}

float OffsetHudElemY(const hudelem_t *elem, const cg_hudelem_t *cghe, float offsetY)
{
    float from;
    float to;
    float lerp;

    lerp = HudElemMovementFrac(elem);
    if (lerp == 1.0f)
        return AlignHudElemY(elem->alignOrg, cghe->y, offsetY);
    from = AlignHudElemY(elem->fromAlignOrg, cghe->y, offsetY);
    to = AlignHudElemY(elem->alignOrg, cghe->y, offsetY);
    return (from - to) * lerp + from;
}

void HudElemScaleSize(int alignScreen, float *w, float *h)
{
    float x;
    float y;

    x = 0.0f;
    y = 0.0f;
    CalcScreenPlacement(&x, &y, w, h, (alignScreen >> 3) & 7, alignScreen & 7);
}

void ConsolidateHudElemText(cg_hudelem_t *cghe, char *hudElemString)
{
    int len;
    int labelIndex;
    int textIndex;

    len = 0;
    for (labelIndex = 0; len < 255 && cghe->hudElemLabel[labelIndex]; labelIndex++)
    {
        if (cghe->hudElemLabel[labelIndex] == "&&"[0]
            && cghe->hudElemLabel[labelIndex + 1] == "&&"[1]
            && cghe->hudElemLabel[labelIndex + 2] == '1')
        {
            labelIndex += 3;
            break;
        }
        hudElemString[len++] = cghe->hudElemLabel[labelIndex];
    }
    for (textIndex = 0; len < 255 && cghe->hudElemText[textIndex]; textIndex++)
        hudElemString[len++] = cghe->hudElemText[textIndex];
    while (len < 255 && cghe->hudElemLabel[labelIndex])
        hudElemString[len++] = cghe->hudElemLabel[labelIndex++];
    hudElemString[len] = 0;
    memcpy(cghe->hudElemText, hudElemString, sizeof(cghe->hudElemText));
    cghe->textWidth = HudElemStringWidth(cghe->hudElemText, cghe);
    cghe->hudElemLabel[0] = 0;
    cghe->labelWidth = 0.0f;
}

void GetHudElemInfo(const hudelem_t *elem, cg_hudelem_t *cghe, char *hudElemString)
{
    int clientNum;
    int deltaTime;
    float lerp;

    switch (elem->font)
    {
    case 0:
        cghe->fontScale = elem->fontScale * 0.25f;
        cghe->font = UI_GetFontHandle(0, cghe->fontScale);
        cghe->fontHeight = (float)UI_TextHeight(cghe->font, cghe->fontScale);
        break;
    case 1:
        cghe->fontScale = elem->fontScale * 0.5f;
        cghe->font = UI_GetFontHandle(4, cghe->fontScale);
        cghe->fontHeight = (float)UI_TextHeight(cghe->font, cghe->fontScale);
        break;
    case 2:
        cghe->fontScale = elem->fontScale * 0.33333334f;
        cghe->font = UI_GetFontHandle(5, cghe->fontScale);
        cghe->fontHeight = (float)UI_TextHeight(cghe->font, cghe->fontScale);
        break;
    }
    cghe->hudElemLabel[0] = 0;
    if (elem->label)
        CG_SafeTranslateHudElemString(elem->label, cghe->hudElemLabel);
    cghe->hudElemText[0] = 0;
    switch (elem->type)
    {
    case HE_TYPE_PLAYERNAME:
        clientNum = I_fround(elem->value);
        if (clientNum >= 0 && clientNum < 64)
            I_strncpyz(cghe->hudElemText, cg->bgs.clientinfo[clientNum].name, 256);
        break;
    case HE_TYPE_MAPNAME:
        I_strncpyz(cghe->hudElemText, CL_GetConfigString(0x13), 256);
        break;
    case HE_TYPE_GAMETYPE:
        I_strncpyz(cghe->hudElemText, CL_GetConfigString(0x14), 256);
        break;
    case HE_TYPE_TEXT:
        if (elem->text)
            CG_SafeTranslateHudElemString(elem->text, cghe->hudElemText);
        break;
    case HE_TYPE_TIMER_DOWN:
    case HE_TYPE_TIMER_UP:
        I_strncpyz(cghe->hudElemText, HudElemTimerString(elem), 256);
        break;
    case HE_TYPE_TENTHS_TIMER_DOWN:
    case HE_TYPE_TENTHS_TIMER_UP:
        I_strncpyz(cghe->hudElemText, HudElemTenthsTimerString(elem), 256);
        break;
    case HE_TYPE_VALUE:
        I_strncpyz(cghe->hudElemText, va("%g", elem->value), 256);
        break;
    }
    if (cghe->hudElemLabel[0] && cghe->hudElemText[0])
        ConsolidateHudElemText(cghe, hudElemString);
    if (cghe->hudElemLabel[0])
        cghe->labelWidth = HudElemStringWidth(cghe->hudElemLabel, cghe);
    else
        cghe->labelWidth = 0.0f;
    if (cghe->hudElemText[0])
        cghe->textWidth = HudElemStringWidth(cghe->hudElemText, cghe);
    else
        cghe->textWidth = 0.0f;
    cghe->width = HudElemWidth(elem, cghe);
    cghe->height = HudElemHeight(elem, cghe);
    SetHudElemPos(elem, cghe);
    deltaTime = cg->time - elem->fadeStartTime;
    if (elem->fadeTime > 0 && deltaTime < elem->fadeTime)
    {
        if (deltaTime < 0)
            deltaTime = 0;
        lerp = (float)deltaTime / (float)elem->fadeTime;
        cghe->color[0] = ((elem->color.r - elem->fromColor.r) * lerp + elem->fromColor.r) * (1.0f / 255.0f);
        cghe->color[1] = ((elem->color.g - elem->fromColor.g) * lerp + elem->fromColor.g) * (1.0f / 255.0f);
        cghe->color[2] = ((elem->color.b - elem->fromColor.b) * lerp + elem->fromColor.b) * (1.0f / 255.0f);
        cghe->color[3] = ((elem->color.a - elem->fromColor.a) * lerp + elem->fromColor.a) * (1.0f / 255.0f);
    }
    else
    {
        cghe->color[0] = elem->color.r * (1.0f / 255.0f);
        cghe->color[1] = elem->color.g * (1.0f / 255.0f);
        cghe->color[2] = elem->color.b * (1.0f / 255.0f);
        cghe->color[3] = elem->color.a * (1.0f / 255.0f);
    }
}

void DrawHudElemString(const char *text, const hudelem_t *elem, cg_hudelem_t *cghe)
{
    float y;
    float scaleY;
    float scaleX;
    float dx;
    float dy;

    y = OffsetHudElemY(elem, cghe, -(cghe->height - cghe->fontHeight));
    scaleX = CL_NormalizedTextScale(cghe->font, cghe->fontScale);
    scaleY = scaleX;
    HudElemScaleSize(elem->alignScreen, &scaleX, &scaleY);
    dx = 0.0f;
    dy = cghe->fontHeight;
    HudElemScaleSize(elem->alignScreen, &dx, &dy);
    CL_DrawTextPhysical(text, 0x7fffffff, cghe->font, cghe->x, y + dy, scaleX, scaleY, cghe->color, 3);
}

void DrawHudElemClock(const hudelem_t *elem, cg_hudelem_t *cghe)
{
    char materialName[64];
    Material *faceMaterial;
    Material *needleMaterial;
    int time;
    float angle;
    float width;
    float height;
    float y;

    if (!CG_ServerMaterialName(elem->materialIndex, materialName, 58))
        return;
    faceMaterial = CL_RegisterMaterialNoMip(materialName, 7);
    I_strncat(materialName, 64, "needle");
    needleMaterial = CL_RegisterMaterialNoMip(materialName, 7);
    time = GetHudElemTime(elem);
    if (elem->duration)
        angle = AngleNormalize360((float)time * 360.0f / (float)elem->duration);
    else
        angle = AngleNormalize360((float)time * 0.006f);
    width = HudElemMaterialWidth(elem, cghe);
    height = HudElemMaterialHeight(elem, cghe);
    y = OffsetHudElemY(elem, cghe, -(cghe->height - height));
    HudElemScaleSize(elem->alignScreen, &width, &height);
    CL_DrawStretchPicPhysical(cghe->x, y, width, height, 0.0f, 0.0f, 1.0f, 1.0f, cghe->color, faceMaterial);
    CG_DrawRotatedPicPhysical(cghe->x, y, width, height, angle, cghe->color, needleMaterial);
}

void DrawHudElemMaterial(const hudelem_t *elem, cg_hudelem_t *cghe)
{
    char materialName[64];
    Material *material;
    float width;
    float height;
    float y;

    if (!CG_ServerMaterialName(elem->materialIndex, materialName, 64))
        return;
    material = CL_RegisterMaterial(materialName, 7);
    width = HudElemMaterialWidth(elem, cghe);
    height = HudElemMaterialHeight(elem, cghe);
    y = OffsetHudElemY(elem, cghe, -(cghe->height - height));
    HudElemScaleSize(elem->alignScreen, &width, &height);
    CL_DrawStretchPicPhysical(cghe->x, y, width, height, 0.0f, 0.0f, 1.0f, 1.0f, cghe->color, material);
}

void DrawHudElemSprite(bool constantSize, Material *material, const float *origin, float height, float alpha)
{
    float radius;
    int renderFxFlags;
    byte color[4];

    if (constantSize)
    {
        radius = height * 0.0043f;
        renderFxFlags = 0x3000;
    }
    else
    {
        renderFxFlags = 0;
        radius = height;
    }
    color[3] = (byte)I_fround(alpha * 255.0f);
    color[0] = 0xff;
    color[1] = 0xff;
    color[2] = 0xff;
    CL_DrawSprite(material, color, origin, radius, 0.0f, renderFxFlags);
}

void DrawHudElemWaypoint(const hudelem_t *elem, cg_hudelem_t *cghe)
{
    char materialName[64];
    Material *material;
    float height;
    vec3_t origin;
    float alpha;

    if (!elem->color.a)
        return;
    if (!CG_ServerMaterialName(elem->materialIndex, materialName, 64))
        return;
    material = CL_RegisterMaterial(materialName, 7);
    height = HudElemMaterialHeight(elem, cghe);
    if (height == 0.0f)
        return;
    origin[0] = elem->x;
    origin[1] = elem->y;
    origin[2] = elem->z;
    alpha = elem->color.a * (1.0f / 255.0f);
    DrawHudElemSprite(elem->value > 0.0f, material, origin, height, alpha);
}

void DrawSingleHudElem2d(const hudelem_t *elem)
{
    char hudElemString[256];
    cg_hudelem_t cghe;

    if (elem->type == HE_TYPE_WAYPOINT)
        return;
    GetHudElemInfo(elem, &cghe, hudElemString);
    if (cghe.color[3] == 0.0f)
        return;
    if (cghe.hudElemLabel[0])
    {
        DrawHudElemString(cghe.hudElemLabel, elem, &cghe);
        cghe.x += cghe.labelWidth;
    }
    switch (elem->type)
    {
    case HE_TYPE_TEXT:
    case HE_TYPE_VALUE:
    case HE_TYPE_PLAYERNAME:
    case HE_TYPE_MAPNAME:
    case HE_TYPE_GAMETYPE:
    case HE_TYPE_TIMER_DOWN:
    case HE_TYPE_TIMER_UP:
    case HE_TYPE_TENTHS_TIMER_DOWN:
    case HE_TYPE_TENTHS_TIMER_UP:
        if (cghe.hudElemText[0])
            DrawHudElemString(cghe.hudElemText, elem, &cghe);
        break;
    case HE_TYPE_CLOCK_DOWN:
    case HE_TYPE_CLOCK_UP:
        DrawHudElemClock(elem, &cghe);
        break;
    case HE_TYPE_MATERIAL:
        DrawHudElemMaterial(elem, &cghe);
        break;
    }
}

void DrawSingleHudElem3d(const hudelem_t *elem)
{
    char hudElemString[256];
    cg_hudelem_t cghe;

    if (elem->type != HE_TYPE_WAYPOINT)
        return;
    GetHudElemInfo(elem, &cghe, hudElemString);
    if (cghe.hudElemLabel[0])
    {
        DrawHudElemString(cghe.hudElemLabel, elem, &cghe);
        cghe.x += cghe.labelWidth;
    }
    if (elem->type == HE_TYPE_WAYPOINT)
        DrawHudElemWaypoint(elem, &cghe);
}

int compare_hudelems(const inflate_huft *pe0, const inflate_huft *pe1)
{
    const hudelem_t *e0 = *(const hudelem_t **)pe0;
    const hudelem_t *e1 = *(const hudelem_t **)pe1;
    float delta = e0->sort - e1->sort;
    if (delta < 0.0f) {
        return -1;
    }
    if (delta > 0.0f) {
        return 1;
    }
    return 0;
}

int GetSortedHudElems(hudelem_t **elems)
{
    playerState_t *ps;
    int elemCount;
    int i;

    ps = &cg->nextSnap->ps;
    elemCount = 0;
    for (i = 0; i < 31 && ps->hud.archival[i].type; i++)
        elems[elemCount++] = &ps->hud.archival[i];
    for (i = 0; i < 31 && ps->hud.current[i].type; i++)
        elems[elemCount++] = &ps->hud.current[i];
    qsort(elems, elemCount, sizeof(elems[0]), (int (*)(const void *, const void *))compare_hudelems);
    return elemCount;
}

void CG_Draw2dHudElems(qboolean foreground)
{
    hudelem_t *elems[62];
    int elemCount;
    int i;

    elemCount = GetSortedHudElems(elems);
    if (elemCount)
    {
        for (i = 0; i < elemCount; i++)
        {
            if (elems[i]->foreground == foreground)
                DrawSingleHudElem2d(elems[i]);
        }
    }
}

void CG_Draw3dHudElems(void)
{
    hudelem_t *elems[62];
    int elemCount;
    int i;

    if (CG_AreHudElemsHidden())
        return;
    elemCount = GetSortedHudElems(elems);
    if (elemCount)
    {
        for (i = 0; i < elemCount; i++)
            DrawSingleHudElem3d(elems[i]);
    }
}


const unsigned char __ZZ25CG_DrawRotatedPicPhysicalfffffPKfP8MaterialE4sign[480] = {
    0x00, 0x00, 0x80, 0xbf, 0x00, 0x00, 0x80, 0xbf, 0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x80, 0xbf,
    0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x80, 0xbf, 0x00, 0x00, 0x80, 0x3f,
    0x07, 0x0f, 0x1a, 0x00, 0x07, 0x0f, 0x1a, 0x00, 0x07, 0x0f, 0x1a, 0x00, 0x40, 0x0e, 0x1a, 0x00,
    0x40, 0x0e, 0x1a, 0x00, 0x40, 0x0e, 0x1a, 0x00, 0x91, 0x0e, 0x1a, 0x00, 0x40, 0x0e, 0x1a, 0x00,
    0xde, 0x0f, 0x1a, 0x00, 0x40, 0x0e, 0x1a, 0x00, 0xbc, 0x0f, 0x1a, 0x00, 0x9a, 0x0f, 0x1a, 0x00,
    0x78, 0x0f, 0x1a, 0x00, 0x56, 0x0f, 0x1a, 0x00, 0x34, 0x0f, 0x1a, 0x00, 0x40, 0x0e, 0x1a, 0x00,
    0x40, 0x0e, 0x1a, 0x00, 0x07, 0x0f, 0x1a, 0x00, 0x40, 0x0e, 0x1a, 0x00, 0x40, 0x0e, 0x1a, 0x00,
    0x40, 0x0e, 0x1a, 0x00, 0x40, 0x0e, 0x1a, 0x00, 0x40, 0x0e, 0x1a, 0x00, 0x40, 0x0e, 0x1a, 0x00,
    0x40, 0x0e, 0x1a, 0x00, 0x40, 0x0e, 0x1a, 0x00, 0x40, 0x0e, 0x1a, 0x00, 0x40, 0x0e, 0x1a, 0x00,
    0x40, 0x0e, 0x1a, 0x00, 0x40, 0x0e, 0x1a, 0x00, 0x40, 0x0e, 0x1a, 0x00, 0x40, 0x0e, 0x1a, 0x00,
    0x40, 0x0e, 0x1a, 0x00, 0x40, 0x0e, 0x1a, 0x00, 0x40, 0x0e, 0x1a, 0x00, 0x40, 0x0e, 0x1a, 0x00,
    0x40, 0x0e, 0x1a, 0x00, 0x40, 0x0e, 0x1a, 0x00, 0x40, 0x0e, 0x1a, 0x00, 0x9b, 0x0e, 0x1a, 0x00,
    0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xff, 0xff, 0xff, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xff, 0xff, 0xff, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
