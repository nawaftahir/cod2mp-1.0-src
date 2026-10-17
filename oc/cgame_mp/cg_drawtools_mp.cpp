#include "common_types.h"
#include "imports.h"
#include "bytematch.h"
#include "headers/PC/cgame_mp/cg_local.h"

#include <string.h>

extern float fabsf(float x);
extern float sinf(float x);
extern float cosf(float x);
extern double atof(const char *s);

void CG_DrawStringExt(float x, float y, const char *string, const vec_t *setColor, qboolean forceColor, qboolean shadow, float charHeight, qboolean adjust);
int CG_DrawDevString(float x, float y, const char *s, const vec_t *color, int align, FontHandle font, int style);
int CG_DrawBigDevString(float x, float y, const char *s, float alpha, int align);
int CG_DrawBigDevStringColor(float x, float y, const char *s, const vec_t *color, int align);
int CG_DrawSmallDevStringColor(float x, float y, const char *s, const vec_t *color, int align);
void CG_TileClear(void);
float *CG_FadeColor(int startMsec, int totalMsec, int fadeMsec);
void CG_NorthDirectionChanged(void);
void CG_UpdateCompassOrientation(void);
void CG_UpdateCompPointerOrientation(void);
void CG_DrawRotatedPicPhysical(float x, float y, float width, float height, float angle, const vec_t *color, MaterialHandle material);
void CG_DrawRotatedPic(float x, float y, float width, float height, int horzAlign, int vertAlign, float angle, const vec_t *color, MaterialHandle material);
void CG_DrawRotatedQuadPic(float x, float y, vec2_t *verts, float angle, const vec_t *color, MaterialHandle material);

static vec4_t color;
extern const dvar_t *cg_hudCompassSpringyPointers;
extern const vec4_t colorWhite;
extern FontHandle UI_GetFontHandle(int type, float scale);
extern void UI_DrawText(const char *text, int maxChars, FontHandle font, float x, float y, int horzAlign, int vertAlign, float scale, const vec_t *color, int style);
extern int CL_TextWidth(const char *text, int maxChars, FontHandle font);
extern int CL_TextHeight(FontHandle font);
extern void CL_DrawText(const char *text, int maxChars, FontHandle font, float x, float y, int horzAlign, int vertAlign, float xScale, float yScale, const vec_t *color, int style);
extern const char *CL_GetConfigString(int index);
extern float AngleNormalize360( float angle );
extern float AngleNormalize180( float angle );
extern float AngleSubtract( float a1, float a2 );
extern void CL_SetFullScreenViewport(void);
extern void CL_SetViewport(int x, int y, int width, int height);
extern void CalcScreenPlacement(float *x, float *y, float *w, float *h, int horzAlign, int vertAlign);
extern void UI_FillRect(float x, float y, float width, float height, int horzAlign, int vertAlign, const vec_t *color);
extern void CL_DrawQuadPic(vec2_t *verts, const vec_t *color, MaterialHandle material);
void CG_DrawStringExt(float x, float y, const char *string, const vec_t *setColor, qboolean forceColor, qboolean shadow, float charHeight, qboolean adjust);
int CG_DrawDevString(float x, float y, const char *s, const vec_t *color, int align, FontHandle font, int style);
int CG_DrawBigDevStringColor(float x, float y, const char *s, const vec_t *color, int align);
int CG_DrawSmallDevStringColor(float x, float y, const char *s, const vec_t *color, int align);
float *CG_FadeColor(int startMsec, int totalMsec, int fadeMsec);
void CG_NorthDirectionChanged(void);
void CG_UpdateCompassOrientation(void);
void CG_UpdateCompPointerOrientation(void);
void CG_TileClear(void);
int CG_DrawBigDevString(float x, float y, const char *s, float alpha, int align);
void CG_DrawRotatedPicPhysical(float x, float y, float width, float height, float angle, const vec_t *color, MaterialHandle material);
void CG_DrawRotatedPic(float x, float y, float width, float height, int horzAlign, int vertAlign, float angle, const vec_t *color, MaterialHandle material);
void CG_DrawRotatedQuadPic(float x, float y, vec2_t *verts, float angle, const vec_t *color, MaterialHandle material);

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

// RtCW bar flags
#define BAR_LEFT 0x0001
#define BAR_CENTER 0x0002
#define BAR_VERT 0x0004
#define BAR_BG 0x0010
#define BAR_BGSPACING_X0Y5 0x0020
#define BAR_BGSPACING_X0Y0 0x0040
#define BAR_LERP_COLOR 0x0100
#define BAR_BORDERSIZE 1










static __forceinline int CG_FloatIsNegative(const float x)
{
    return *(const int *)&x < 0;
}

// retail 0x4bbcd0, unreferenced; Kisak name
void Vec4Lerp(const float *from, const float *to, float frac, float *result)
{
    result[0] = (to[0] - from[0]) * frac + from[0];
    result[1] = (to[1] - from[1]) * frac + from[1];
    result[2] = (to[2] - from[2]) * frac + from[2];
    result[3] = (to[3] - from[3]) * frac + from[3];
}

// retail 0x4bbd10, unreferenced (RtCW shape, no hud alpha)
void CG_FilledBar(float x, float y, float w, float h, const float *startColor, const float *endColor, const float *bgColor, float frac, int flags)
{
    vec4_t scolor;
    vec4_t backgroundcolor = {1, 1, 1, 0.25f}, colorAtPos;
    int indent = BAR_BORDERSIZE;

    scolor[0] = startColor[0];
    scolor[1] = startColor[1];
    scolor[2] = startColor[2];
    scolor[3] = startColor[3];

    if ((flags & BAR_BG) && bgColor) {
        backgroundcolor[0] = bgColor[0];
        backgroundcolor[1] = bgColor[1];
        backgroundcolor[2] = bgColor[2];
        backgroundcolor[3] = bgColor[3];
    }

    if (flags & BAR_LERP_COLOR) {
        colorAtPos[0] = (endColor[0] - scolor[0]) * frac + scolor[0];
        colorAtPos[1] = (endColor[1] - scolor[1]) * frac + scolor[1];
        colorAtPos[2] = (endColor[2] - scolor[2]) * frac + scolor[2];
        colorAtPos[3] = (endColor[3] - scolor[3]) * frac + scolor[3];
    }

    if (flags & BAR_BG) {
        UI_FillRect(x, y, w, h, 0, 0, backgroundcolor);

        if (flags & BAR_BGSPACING_X0Y0) {
        } else if (flags & BAR_BGSPACING_X0Y5) {
            indent *= 3;
            y += indent;
            h -= (2 * indent);
        } else {
            x += indent;
            y += indent;
            w -= (2 * indent);
            h -= (2 * indent);
        }
    }

    if (flags & BAR_VERT) {
        if (flags & BAR_LEFT)
            y += (h * (1 - frac));
        else if (flags & BAR_CENTER)
            y += (h * (1 - frac) / 2);

        if (flags & BAR_LERP_COLOR)
            UI_FillRect(x, y, w, h * frac, 0, 0, colorAtPos);
        else
            UI_FillRect(x, y, w, h * frac, 0, 0, scolor);
    } else {
        if (flags & BAR_LEFT)
            x += (w * (1 - frac));
        else if (flags & BAR_CENTER)
            x += (w * (1 - frac) / 2);

        if (flags & BAR_LERP_COLOR)
            UI_FillRect(x, y, w * frac, h, 0, 0, colorAtPos);
        else
            UI_FillRect(x, y, w * frac, h, 0, 0, scolor);
    }
}

// retail 0x4bbf80, unreferenced
void CG_HorizontalPercentBar(float x, float y, float width, float height, float percent)
{
    vec4_t bgcolor = {0.5f, 0.5f, 0.5f, 0.3f},
           color = {1.0f, 1.0f, 1.0f, 0.3f};
    CG_FilledBar(x, y, width, height, color, NULL, bgcolor, percent, BAR_BG);
}

void CG_DrawRotatedPic(float x, float y, float width, float height, int horzAlign, int vertAlign, float angle, const vec_t *color, MaterialHandle material)
{
    CalcScreenPlacement(&x, &y, &width, &height, horzAlign, vertAlign);
    CG_DrawRotatedPicPhysical(x, y, width, height, angle, color, material);
}

void CG_DrawStringExt(float x, float y, const char *string, const vec_t *setColor, qboolean forceColor, qboolean shadow, float charHeight, qboolean adjust)
{
    float baselineY = y + charHeight * 0.8f;
    float fontScale = charHeight / 48.0f;

    if (!setColor) {
        setColor = colorWhite;
    }

    FontHandle font = UI_GetFontHandle(5, fontScale);
    int style = shadow ? 3 : 0;

    UI_DrawText(string, 0x7fffffff, font, x, baselineY, 1, 1, fontScale, setColor, style);
}

// Original name unknown.
byte CG_IsFullViewport(void)
{
    return cgs->viewX == cg->refdef.x
        && cgs->viewY == cg->refdef.y
        && cgs->viewWidth == cg->refdef.width
        && cgs->viewHeight == cg->refdef.height;
}

void CG_TileClear(void)
{

    if (CG_IsFullViewport()) {
        return;
    }

    CL_SetFullScreenViewport();
    CL_SetViewport(
        cg->refdef.x,
        cg->refdef.y,
        cg->refdef.width,
        cg->refdef.height);
}

float *CG_FadeColor(int startMsec, int totalMsec, int fadeMsec)
{
    if (!startMsec) {
        return NULL;
    }

    int t = cg->time - startMsec;

    if (t >= totalMsec) {
        return NULL;
    }

    int remaining = totalMsec - t;

    if (remaining < fadeMsec) {
        color[3] = (float)remaining / (float)fadeMsec;
    } else {
        color[3] = 1.0f;
    }

    color[2] = 1.0f;
    color[1] = 1.0f;
    color[0] = 1.0f;
    return color;
}

// retail 0x4bc5c0, unreferenced; Q3 shape without armor (SUPPORTED)
__forceinline void CG_GetColorForHealth(int health, vec4_t hcolor)
{
    if (health <= 0) {
        hcolor[0] = 0.0f;
        hcolor[1] = 0.0f;
        hcolor[2] = 0.0f;
        hcolor[3] = 1.0f;
        return;
    }

    hcolor[0] = 1.0f;
    hcolor[3] = 1.0f;
    if (health >= 100)
        hcolor[2] = 1.0f;
    else if (health < 66)
        hcolor[2] = 0.0f;
    else
        hcolor[2] = (float)(health - 66) / 33.0f;

    if (health > 60)
        hcolor[1] = 1.0f;
    else if (health < 30)
        hcolor[1] = 0.0f;
    else
        hcolor[1] = (float)(health - 30) / 30.0f;
}

// retail 0x4bc640, unreferenced
void CG_ColorForHealth(vec4_t hcolor)
{
    CG_GetColorForHealth(cg->nextSnap->ps.stats[0], hcolor);
}

void CG_NorthDirectionChanged(void)
{
    const char *configStr = CL_GetConfigString(11);
    cg->compassNorthYaw = (float)atof(configStr);
}

void CG_UpdateCompassOrientation(void)
{
    float fTargetYaw = AngleNormalize360(cg->refdefViewAngles[1] - cg->compassNorthYaw);

    int lastTime = cg->compassLastTime;
    int currentTime = cg->time;

    if (lastTime > currentTime || (float)(currentTime - lastTime) > 500.0f) {
        cg->compassLastTime = currentTime;
        cg->compassYaw = fTargetYaw;
        cg->compassSpeed = 0.0f;
        return;
    }

    int iTotalTime = currentTime - lastTime;
    cg->compassLastTime = currentTime;
    float fDiff = AngleSubtract(cg->compassYaw, fTargetYaw);

    while (iTotalTime > 0) {
        int step;
        if (iTotalTime > 5) {
            step = 5;
            iTotalTime -= 5;
        } else {
            step = iTotalTime;
            iTotalTime = 0;
        }
        float fTimeStep = (float)step * 0.001f;

        if (fabsf(fDiff) < 0.25f) {
            if (fabsf(cg->compassSpeed) < 1.0f) {
                cg->compassYaw = fTargetYaw;
                cg->compassSpeed = 0.0f;
                return;
            }
        }

        fDiff = AngleNormalize180(fDiff + fTimeStep * cg->compassSpeed);

        if (fDiff > 0.0f) {
            cg->compassSpeed -= fTimeStep * 1000.0f;
        } else if (fDiff < 0.0f) {
            cg->compassSpeed += fTimeStep * 1000.0f;
        }

        cg->compassSpeed = cg->compassSpeed - fTimeStep * cg->compassSpeed * 2.0f;

        if (cg->compassSpeed > 0.0f) {
            if (fDiff > 0.0f) {
                cg->compassSpeed = cg->compassSpeed - fTimeStep * cg->compassSpeed * 3.5f;
            }
            cg->compassSpeed = cg->compassSpeed - fTimeStep;
            if (cg->compassSpeed < 0.0f) {
                cg->compassSpeed = 0.0f;
            }
        } else {
            if (fDiff < 0.0f) {
                cg->compassSpeed = cg->compassSpeed - fTimeStep * cg->compassSpeed * 3.5f;
            }
            cg->compassSpeed = fTimeStep + cg->compassSpeed;
            if (cg->compassSpeed > 0.0f) {
                cg->compassSpeed = 0.0f;
            }
        }

        if (cg->compassSpeed > 30000.0f) {
            cg->compassSpeed = 30000.0f;
        } else if (cg->compassSpeed < -30000.0f) {
            cg->compassSpeed = -30000.0f;
        }
    }

    cg->compassYaw = AngleNormalize360(fDiff + fTargetYaw);
}

void CG_UpdateCompPointerOrientation(void)
{
    if (cg_hudCompassSpringyPointers->current.enabled == 0) {
        cg->compPointerYaw = cg->refdefViewAngles[1];
        return;
    }

    int lastTime = cg->compassLastTime;
    int currentTime = cg->time;

    if (lastTime == currentTime) {
        return;
    }

    if (lastTime > currentTime || (float)(currentTime - lastTime) > 500.0f) {
        cg->compassLastTime = currentTime;
        cg->compPointerYaw = cg->refdefViewAngles[1];
        cg->compPointerSpeed = 0.0f;
        return;
    }

    int iTotalTime = currentTime - lastTime;
    cg->compassLastTime = currentTime;
    float fDiff = AngleSubtract(cg->compPointerYaw, cg->refdefViewAngles[1]);

    if (fabsf(fDiff) > 10.0f) {
        fDiff = CG_FloatIsNegative(fDiff) ? -10.0f : 10.0f;
    }

    while (iTotalTime > 0) {
        int step;
        if (iTotalTime > 5) {
            step = 5;
            iTotalTime -= 5;
        } else {
            step = iTotalTime;
            iTotalTime = 0;
        }
        float fTimeStep = (float)step * 0.001f;

        if (fabsf(fDiff) < 0.5f) {
            if (fabsf(cg->compPointerSpeed) < 2.0f) {
                cg->compPointerYaw = cg->refdefViewAngles[1];
                cg->compPointerSpeed = 0.0f;
                return;
            }
        }

        fDiff = AngleNormalize180(fDiff + fTimeStep * cg->compPointerSpeed);

        if (fDiff > 0.0f) {
            cg->compPointerSpeed -= fTimeStep * 1500.0f;
        } else if (fDiff < 0.0f) {
            cg->compPointerSpeed += fTimeStep * 1500.0f;
        }

        cg->compPointerSpeed = cg->compPointerSpeed - fTimeStep * cg->compPointerSpeed * 3.0f;

        if (cg->compPointerSpeed > 0.0f) {
            if (fDiff > 0.0f) {
                cg->compPointerSpeed = cg->compPointerSpeed - fTimeStep * cg->compPointerSpeed * 5.0f;
            }
            cg->compPointerSpeed = cg->compPointerSpeed - (fTimeStep + fTimeStep);
            if (cg->compPointerSpeed < 0.0f) {
                cg->compPointerSpeed = 0.0f;
            }
        } else {
            if (fDiff < 0.0f) {
                cg->compPointerSpeed = cg->compPointerSpeed - fTimeStep * cg->compPointerSpeed * 5.0f;
            }
            cg->compPointerSpeed = fTimeStep + fTimeStep + cg->compPointerSpeed;
            if (cg->compPointerSpeed > 0.0f) {
                cg->compPointerSpeed = 0.0f;
            }
        }

        if (cg->compPointerSpeed > 2000.0f) {
            cg->compPointerSpeed = 2000.0f;
        } else if (cg->compPointerSpeed < -2000.0f) {
            cg->compPointerSpeed = -2000.0f;
        }
    }

    cg->compPointerYaw = AngleNormalize360(fDiff + cg->refdefViewAngles[1]);
}

void CG_DrawRotatedPicPhysical(float x, float y, float width, float height, float angle, const vec_t *color, MaterialHandle material)
{
    static const float sign[4][2] = { { -1.0f, -1.0f }, { 1.0f, -1.0f }, { 1.0f, 1.0f }, { -1.0f, 1.0f } };
    float c;
    float s;
    float halfW;
    float halfH;
    float center[2];
    float scale[2][2];
    vec2_t verts[4];
    int i;

    FastSinCos((angle * 0.017453292f), &s, &c);
    halfW = width * 0.5f;
    halfH = height * 0.5f;
    center[0] = x + halfW;
    center[1] = halfH + y;
    scale[0][0] = c * halfW;
    scale[0][1] = s * halfW;
    scale[1][0] = s * halfH;
    scale[1][1] = c * halfH;
    for (i = 0; i < 4; i++) {
        verts[i][0] = center[0] + scale[0][0] * sign[i][0] - scale[0][1] * sign[i][1];
        verts[i][1] = scale[1][0] * sign[i][0] + scale[1][1] * sign[i][1] + center[1];
    }

    CL_DrawQuadPic(verts, color, material);
}
void CG_DrawRotatedQuadPic(float x, float y, vec2_t *verts, float angle, const vec_t *color, MaterialHandle material)
{
    float c;
    float s;
    vec2_t xy[4];
    int i;

    FastSinCos((angle * 0.017453292f), &s, &c);
    for (i = 0; i < 4; i++) {
        xy[i][0] = c * verts[i][0] + x - s * verts[i][1];
        xy[i][1] = s * verts[i][0] + c * verts[i][1] + y;
    }

    CL_DrawQuadPic(xy, color, material);
}
int CG_DrawDevString(float x, float y, const char *s, const vec_t *color, int align, FontHandle font, int style)
{
    int height;
    float fStep;
    int textWidth;

    if ((align & 3) != 1)
        CL_TextWidth(s, 0, font);

    height = CL_TextHeight(font);
    fStep = (float)height;
    if ((align & 0xc) != 8) {
        if ((align & 0xc) == 0xc)
            fStep *= 0.5f;
        y += fStep;
    }

    textWidth = CL_TextWidth(s, 0, font);
    CL_DrawText(s, 0x7fffffff, font, -(float)textWidth, y, 3, 1, 1.0f, 1.0f, color, style);
    return height;
}
int CG_DrawBigDevString(float x, float y, const char *s, float alpha, int align)
{
    vec4_t c;
    c[0] = 1.0f;
    c[1] = 1.0f;
    c[2] = 1.0f;
    c[3] = alpha;

    return CG_DrawDevString(x, y, s, c, align, cgs->media.bigDevFont, 0);
}
int CG_DrawBigDevStringColor(float x, float y, const char *s, const vec_t *color, int align)
{
    return CG_DrawDevString(x, y, s, color, align, cgs->media.bigDevFont, 0);
}
int CG_DrawSmallDevString(float x, float y, const char *s, float alpha, int align)
{
    vec4_t c;
    c[0] = 1.0f;
    c[1] = 1.0f;
    c[2] = 1.0f;
    c[3] = alpha;

    return CG_DrawDevString(x, y, s, c, align, cgs->media.smallDevFont, 0);
}
int CG_DrawSmallDevStringColor(float x, float y, const char *s, const vec_t *color, int align)
{
    return CG_DrawDevString(x, y, s, color, align, cgs->media.smallDevFont, 0);
}