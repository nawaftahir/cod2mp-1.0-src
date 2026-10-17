#include "common_types.h"
#include "imports.h"

extern ScreenPlacementGlob spGlob;
#define SP (&spGlob)

extern clientStatic_t cls;

float GetRealWidthFromVirtualWidth(float width);
float GetRealHeightFromVirtualHeight(float height);
float GetVirtualWidthFromRealWidth(float width);
float GetVirtualHeightFromRealHeight(float height);
void CalcScreenX(float *x, int horzAlign);
void CalcScreenY(float *y, int vertAlign);
void CalcSplitScreenTextOffset(FontHandle font, float *y);
void SetScreenScaling(float safeAreaRatioHorizontal, float safeAreaRatioVertical, float viewportX, float viewportY, float viewportWidth, float viewportHeight);
void CalcScreenPlacement(float *x, float *y, float *w, float *h, int horzAlign, int vertAlign);
void CalcScreenPlacementInl(float *x, float *y, float *w, float *h, int horzAlign, int vertAlign);

// retail 0x449a60, unreferenced; name unknown
float ScrPlace_ViewableWidth_449a60(void)
{
    return SP->virtualViewableMax[0] - SP->virtualViewableMin[0];
}

// retail 0x449a70, unreferenced; name unknown
float ScrPlace_ViewableHeight_449a70(void)
{
    return SP->virtualViewableMax[1] - SP->virtualViewableMin[1];
}

// retail 0x449a80, unreferenced; name unknown
float ScrPlace_RealViewableWidth_449a80(void)
{
    return SP->realViewableMax[0] - SP->realViewableMin[0];
}

// retail 0x449a90, unreferenced; name unknown
float ScrPlace_RealViewableHeight_449a90(void)
{
    return SP->realViewableMax[1] - SP->realViewableMin[1];
}

// retail 0x449aa0; name from KisakCOD (SUPPORTED)
static void ScrPlace_CalcSafeAreaOffsets(float safeAreaRatioHorz, float safeAreaRatioVert,
                                         float viewportX, float viewportY, float viewportWidth, float viewportHeight,
                                         float horzAspectScale, float *realViewableMin, float *realViewableMax,
                                         float *virtualViewableMin, float *virtualViewableMax)
{
    float displayWidth = (float)cls.vidConfig.width;
    float displayHeight;
    float unsafeSize[2];
    float safeMax[2];
    float safeLeft, safeTop, clampedRight, clampedBottom;
    unsafeSize[0] = (1.0f - safeAreaRatioHorz) * displayWidth * 0.5f;
    displayHeight = (float)cls.vidConfig.height;
    unsafeSize[1] = (1.0f - safeAreaRatioVert) * displayHeight * 0.5f;
    safeMax[0] = displayWidth - unsafeSize[0];
    safeMax[1] = displayHeight - unsafeSize[1];
    safeLeft = (viewportX - unsafeSize[0] >= 0) ? viewportX : unsafeSize[0];
    safeTop = (viewportY - unsafeSize[1] >= 0) ? viewportY : unsafeSize[1];
    clampedRight = (safeMax[0] - (viewportX + viewportWidth) >= 0) ? (viewportX + viewportWidth) : safeMax[0];
    clampedBottom = (safeMax[1] - (viewportY + viewportHeight) >= 0) ? (viewportY + viewportHeight) : safeMax[1];
    realViewableMin[0] = safeLeft - viewportX;
    realViewableMin[1] = safeTop - viewportY;
    realViewableMax[0] = clampedRight - viewportX;
    realViewableMax[1] = clampedBottom - viewportY;
    virtualViewableMin[0] = 640.0f / viewportWidth * realViewableMin[0] * horzAspectScale;
    virtualViewableMin[1] = 480.0f / viewportHeight * realViewableMin[1];
    virtualViewableMax[0] = 640.0f / viewportWidth * realViewableMax[0] * horzAspectScale;
    virtualViewableMax[1] = 480.0f / viewportHeight * realViewableMax[1];
}
void SetScreenScaling(float safeAreaRatioHorizontal, float safeAreaRatioVertical,
                      float viewportX, float viewportY, float viewportWidth, float viewportHeight)
{
    float adjustedRealWidth;
    SP->realViewportSize[0] = viewportWidth;
    SP->realViewportSize[1] = viewportHeight;
    adjustedRealWidth = viewportHeight / cls.vidConfig.aspectRatioPixel * (4.0f / 3.0f);
    ScrPlace_CalcSafeAreaOffsets(safeAreaRatioHorizontal, safeAreaRatioVertical,
                                 viewportX, viewportY, viewportWidth, viewportHeight,
                                 viewportWidth / adjustedRealWidth,
                                 SP->realViewableMin, SP->realViewableMax, SP->virtualViewableMin, SP->virtualViewableMax);
    SP->scaleVirtualToReal[0] = adjustedRealWidth / 640.0f;
    SP->scaleVirtualToReal[1] = viewportHeight / 480.0f;
    SP->scaleVirtualToFull[0] = viewportWidth / 640.0f;
    SP->scaleVirtualToFull[1] = viewportHeight / 480.0f;
    SP->scaleRealToVirtual[0] = 640.0f / adjustedRealWidth;
    SP->scaleRealToVirtual[1] = 480.0f / viewportHeight;
    SP->subScreenLeft = (viewportWidth - adjustedRealWidth) * 0.5f;
    SP->virtualScreenOffsetX = 640.0f - SP->scaleRealToVirtual[0] * adjustedRealWidth;
}
// retail 0x449cd0, unreferenced; name unknown
void SetScreenScalingInt(float safeAreaRatioHorizontal, float safeAreaRatioVertical,
                         int viewportX, int viewportY, int viewportWidth, int viewportHeight)
{
    SetScreenScaling(safeAreaRatioHorizontal, safeAreaRatioVertical,
                     (float)viewportX, (float)viewportY, (float)viewportWidth, (float)viewportHeight);
}

float GetRealWidthFromVirtualWidth(float width)
{
    return SP->scaleVirtualToReal[0] * width;
}

float GetRealHeightFromVirtualHeight(float height)
{
    return SP->scaleVirtualToReal[1] * height;
}

float GetVirtualWidthFromRealWidth(float width)
{
    return SP->scaleRealToVirtual[0] * width;
}

float GetVirtualHeightFromRealHeight(float height)
{
    return SP->scaleRealToVirtual[1] * height;
}

void CalcScreenX(float *x, int horzAlign)
{
    float scaled;

    switch (horzAlign) {
    case 1:
        scaled = SP->scaleVirtualToReal[0] * *x;
        *x = scaled + SP->realViewableMin[0];
        break;
    case 2:
        scaled = SP->scaleVirtualToReal[0] * *x;
        *x = scaled + SP->realViewportSize[0] * 0.5f;
        break;
    case 7:
        scaled = SP->scaleVirtualToReal[0] * *x;
        *x = scaled + (SP->realViewableMax[0] + SP->realViewableMin[0]) * 0.5f;
        break;
    case 3:
        scaled = SP->scaleVirtualToReal[0] * *x;
        *x = scaled + SP->realViewableMax[0];
        break;
    case 4:
        *x = SP->scaleVirtualToFull[0] * *x;
        break;
    case 6:
        scaled = SP->scaleRealToVirtual[0] * *x;
        *x = scaled + SP->virtualScreenOffsetX;
        break;
    case 0:
        scaled = SP->scaleVirtualToReal[0] * *x;
        *x = scaled + SP->virtualScreenOffsetX + SP->subScreenLeft;
        break;
    case 5:
        break;
    default:
        scaled = SP->scaleVirtualToReal[0] * *x;
        *x = scaled + SP->virtualScreenOffsetX + SP->subScreenLeft;
        break;
    }
}

void CalcScreenY(float *y, int vertAlign)
{
    float scaled;

    switch (vertAlign) {
    case 1:
        scaled = SP->scaleVirtualToReal[1] * *y;
        *y = scaled + SP->realViewableMin[1];
        break;
    case 2:
        scaled = SP->scaleVirtualToReal[1] * *y;
        *y = scaled + SP->realViewportSize[1] * 0.5f;
        break;
    case 7:
        scaled = SP->scaleVirtualToReal[1] * *y;
        *y = scaled + (SP->realViewableMax[1] + SP->realViewableMin[1]) * 0.5f;
        break;
    case 3:
        scaled = SP->scaleVirtualToReal[1] * *y;
        *y = scaled + SP->realViewableMax[1];
        break;
    case 4:
        *y = SP->scaleVirtualToFull[1] * *y;
        break;
    case 6:
        *y = SP->scaleRealToVirtual[1] * *y;
        break;
    case 0:
        *y = SP->scaleVirtualToReal[1] * *y;
        break;
    case 5:
        break;
    default:
        *y = SP->scaleVirtualToReal[1] * *y;
        break;
    }
}

void CalcSplitScreenTextOffset(FontHandle font, float *y)
{

}

__forceinline void CalcScreenPlacementInl(float *x, float *y, float *w, float *h, int horzAlign, int vertAlign)
{
    float scaled;

    switch (horzAlign) {
    case 1:
        scaled = SP->scaleVirtualToReal[0] * *x;
        *x = scaled + SP->realViewableMin[0];
        *w = SP->scaleVirtualToReal[0] * *w;
        break;
    case 2:
        scaled = SP->scaleVirtualToReal[0] * *x;
        *x = scaled + SP->realViewportSize[0] * 0.5f;
        *w = SP->scaleVirtualToReal[0] * *w;
        break;
    case 7:
        scaled = SP->scaleVirtualToReal[0] * *x;
        *x = scaled + (SP->realViewableMax[0] + SP->realViewableMin[0]) * 0.5f;
        *w = SP->scaleVirtualToReal[0] * *w;
        break;
    case 3:
        scaled = SP->scaleVirtualToReal[0] * *x;
        *x = scaled + SP->realViewableMax[0];
        *w = SP->scaleVirtualToReal[0] * *w;
        break;
    case 4:
        *x = SP->scaleVirtualToFull[0] * *x;
        *w = SP->scaleVirtualToFull[0] * *w;
        break;
    case 6:
        scaled = SP->scaleRealToVirtual[0] * *x;
        *x = scaled + SP->virtualScreenOffsetX;
        *w = SP->scaleRealToVirtual[0] * *w;
        break;
    case 0:
        scaled = SP->scaleVirtualToReal[0] * *x;
        *x = scaled + SP->virtualScreenOffsetX + SP->subScreenLeft;
        *w = SP->scaleVirtualToReal[0] * *w;
        break;
    case 5:
        break;
    default:
        scaled = SP->scaleVirtualToReal[0] * *x;
        *x = scaled + SP->virtualScreenOffsetX + SP->subScreenLeft;
        *w = SP->scaleVirtualToReal[0] * *w;
        break;
    }

    switch (vertAlign) {
    case 1:
        scaled = SP->scaleVirtualToReal[1] * *y;
        *y = scaled + SP->realViewableMin[1];
        *h = SP->scaleVirtualToReal[1] * *h;
        break;
    case 2:
        scaled = SP->scaleVirtualToReal[1] * *y;
        *y = scaled + SP->realViewportSize[1] * 0.5f;
        *h = SP->scaleVirtualToReal[1] * *h;
        break;
    case 7:
        scaled = SP->scaleVirtualToReal[1] * *y;
        *y = scaled + (SP->realViewableMax[1] + SP->realViewableMin[1]) * 0.5f;
        *h = SP->scaleVirtualToReal[1] * *h;
        break;
    case 3:
        scaled = SP->scaleVirtualToReal[1] * *y;
        *y = scaled + SP->realViewableMax[1];
        *h = SP->scaleVirtualToReal[1] * *h;
        break;
    case 4:
        *y = SP->scaleVirtualToFull[1] * *y;
        *h = SP->scaleVirtualToFull[1] * *h;
        break;
    case 6:
        *y = SP->scaleRealToVirtual[1] * *y;
        *h = SP->scaleRealToVirtual[1] * *h;
        break;
    case 0:
        *y = SP->scaleVirtualToReal[1] * *y;
        *h = SP->scaleVirtualToReal[1] * *h;
        break;
    case 5:
        break;
    default:
        *y = SP->scaleVirtualToReal[1] * *y;
        *h = SP->scaleVirtualToReal[1] * *h;
        break;
    }
}

__declspec(noinline) void CalcScreenPlacement(float *x, float *y, float *w, float *h, int horzAlign, int vertAlign)
{
    CalcScreenPlacementInl(x, y, w, h, horzAlign, vertAlign);
}
