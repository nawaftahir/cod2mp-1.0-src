#ifndef CLEAN_PC_GFX_D3D_RB_BACKEND_H
#define CLEAN_PC_GFX_D3D_RB_BACKEND_H

#include "../../cod2_fwd.h"

typedef struct GfxCmdDrawLines GfxCmdDrawLines;
typedef struct GfxCmdDrawPoints GfxCmdDrawPoints;
typedef struct GfxCmdDrawSprite GfxCmdDrawSprite;
typedef struct GfxCmdDrawTriangles GfxCmdDrawTriangles;
typedef struct GfxCmdSetStencilRefValue GfxCmdSetStencilRefValue;
typedef struct GfxCmdStencilPlanes GfxCmdStencilPlanes;
typedef struct SunFlareDynamic SunFlareDynamic;
typedef struct materialCommands_t materialCommands_t;
typedef struct r_backEndGlobals_t r_backEndGlobals_t;

struct GfxCmdDrawTriangles {
    GfxCmdHeader header;
    const Material *material;
    MaterialTechniqueType techType;
    short int indexCount;
    short int vertexCount;
};

struct GfxCmdSetStencilRefValue {
    GfxCmdHeader header;
    int refValue;
};

struct GfxCmdStencilPlanes {
    GfxCmdHeader header;
    vec_t nearDist;
    int planeCount;
    vec_t planeDists[1];
};

struct GfxCmdDrawSprite {
    GfxCmdHeader header;
    Material *material;
    GfxColor rgbaColor;
    vec3_t pos;
    float radius;
    float minScreenRadius;
    int renderFxFlags;
};

struct SunFlareDynamic {
    float flareIntensity;
    float currentBlind;
    float currentGlare;
    int lastTime;
    Bool error;
    float cumulVisibility;
    float lastVisibility;
    float lastDot;
    int hitNum;
    IDirect3DQuery9 * sunQuery[2];
    Bool sunQueryIssued[2];
};

enum GfxOptimizedVertexSource {
    OPT_VERTS_NONE = 0,
    OPT_VERTS_WORLD = 1,
    OPT_VERTS_SMC = 2,
    OPT_VERTS_COUNT = 3
};

enum MaterialVertexDeclType {
    VERTDECL_GENERIC = 0,
    VERTDECL_WORLD = 1,
    VERTDECL_POS_TEX = 2,
    VERTDECL_STATICMODELCACHE = 3,
    VERTDECL_COUNT = 4
};

struct GfxBackEndLightSettings {
    vec4_t position;
    vec4_t ambient;
    vec4_t color;
    vec4_t specular;
    GfxLightDef *def;
};

struct GfxDebugPlume {
    vec3_t origin;
    vec4_t color;
    int score;
    int startTime;
    int duration;
};

struct GfxDebugPoly {
    vec4_t color;
    int firstVert;
    int vertCount;
};

struct GfxEntityLighting {
    int viewCount;
    float sunVisibility;
    vec4_t colorForDir[6];
};

struct GfxPointVertex {
    vec3_t xyz;
    byte color[4];
};

struct GfxVertex {
    vec4_t xyzw;
    vec3_t normal;
    GfxColor color;
    vec2_t texCoord;
    vec3_t binormal;
    vec3_t tangent;
};

struct GfxVertexDx7 {
    vec3_t xyz;
    vec3_t normal;
    GfxColor color;
    vec2_t texCoord;
};

struct GfxWorldVertexDx7 {
    vec3_t xyz;
    GfxColor color;
    vec2_t texCoord;
    vec2_t lmapCoord;
};

struct Glyph {
    short unsigned int letter;
    signed char x0;
    signed char y0;
    byte dx;
    byte pixelWidth;
    byte pixelHeight;
    float s0;
    float t0;
    float s1;
    float t1;
};

struct Font_s {
    const char *name;
    int pixelHeight;
    int glyphCount;
    MaterialHandle material;
    Glyph *glyphs;
};

struct trDebugLine_t {
    vec3_t start;
    vec3_t end;
    vec4_t color;
    qboolean depthTest;
};

struct trDebugString_t {
    vec3_t xyz;
    vec4_t color;
    float scale;
    char text[96];
};

struct GfxCmdDrawLines {
    GfxCmdHeader header;
    short int lineCount;
    short int width;
    GfxPointVertex verts[2];
};

struct GfxCmdDrawPoints {
    GfxCmdHeader header;
    short int pointCount;
    short int size;
    GfxPointVertex verts[1];
};

struct materialCommands_t {
    union { union { GfxVertex generic[5450]; GfxWorldVertex world[5450]; } dx9; union { GfxVertexDx7 generic[5450]; GfxWorldVertexDx7 world[5450]; } dx7; } verts;
    byte reserved[8];
    r_index_t *indices;
    r_index_t *optimizedIndices;
    GfxOptimizedVertexSource optimizedVertexSource;
    const Material *material;
    MaterialTechniqueType techType;
    int lmapIndex;
    float materialTime;
    MaterialVertexDeclType declType;
    int indexCount;
    int vertexCount;
    int firstVertex;
    int lastVertex;
    int optimizedIndexCount;
    int optimizedVertexCount;
    int firstOptimizedVertex;
};

struct r_backEndGlobals_t {
    vec4_t codeConsts[59];
    int frameCount;
    int viewCount;
    GfxSceneDef sceneDef;
    const GfxViewParms *viewParms;
    GfxLodParms lodParms;
    GfxViewport sceneViewport;
    D3DMATRIX viewProjectionMatrix;
    int tileIndex;
    int tileCount;
    int width;
    int height;
    GfxEntity *currentEntity;
    GfxEntityLighting *currentEntityLighting;
    GfxEntity worldEntity;
    Bool viewportIsDirty;
    Bool projection2D;
    GfxSamplerState defaultSamplerState;
    GfxColor color_axis;
    GfxColor color_allies;
    GfxColor fogColor;
    int glowIndexFirst;
    int glowCount;
    GfxImage * glowImage[2];
    float texScrollAmountDx7;
    GfxCodeMatrices codeMatrixStack[3];
    int codeMatrixStackLevel;
    GfxRenderTargetId resolvedPostSunTarget;
    GfxRenderTargetId resolvedSceneTarget;
    GfxImage *currentFeedbackImage;
    GfxBackEndLightSettings light[2];
    GfxEntityLighting entityLighting[2046];
    D3DMATRIX shadowLookupMatrix;
    FontHandle debugFont;
};
#endif
