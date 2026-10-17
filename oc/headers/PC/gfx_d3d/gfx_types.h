#ifndef CLEAN_PC_GFX_D3D_GFX_TYPES_H
#define CLEAN_PC_GFX_D3D_GFX_TYPES_H

#include "../../cod2_fwd.h"
#include "../../cod2_defs.h"

typedef struct BuiltInMaterialTable BuiltInMaterialTable;
typedef struct DSurface_s DSurface_s;
typedef struct DiskGfxVertex DiskGfxVertex;
typedef struct DiskTriangleSoup DiskTriangleSoup;
typedef struct DxCapsCheckBits DxCapsCheckBits;
typedef struct DxCapsCheckInteger DxCapsCheckInteger;
typedef struct DxStencilDecode DxStencilDecode;
typedef struct DxTextureOpDecode DxTextureOpDecode;
typedef struct DxTextureStageEnums DxTextureStageEnums;
typedef struct FxMemMgr_Emitter FxMemMgr_Emitter;
typedef struct GfxAabbTree GfxAabbTree;
typedef struct GfxCachedShaderText GfxCachedShaderText;
typedef struct GfxCell GfxCell;
typedef struct GfxCmdCall GfxCmdCall;
typedef struct GfxCmdClearScreen GfxCmdClearScreen;
typedef struct GfxCmdDrawSunPostEffects GfxCmdDrawSunPostEffects;
typedef struct GfxCmdDrawSurfs GfxCmdDrawSurfs;
typedef struct GfxCmdSetClipPlanes GfxCmdSetClipPlanes;
typedef struct GfxCmdSetLightProperties GfxCmdSetLightProperties;
typedef struct GfxCmdSetMaterialColor GfxCmdSetMaterialColor;
typedef struct GfxCmdSetRenderTarget GfxCmdSetRenderTarget;
typedef struct GfxCmdStretchRaw GfxCmdStretchRaw;
typedef struct GfxCmdStretchPic GfxCmdStretchPic;
typedef struct GfxCullGroup GfxCullGroup;
typedef struct GfxDrawGroupCommands GfxDrawGroupCommands;
typedef struct GfxImageFileHeader GfxImageFileHeader;
typedef struct GfxLightGridColors GfxLightGridColors;
typedef struct GfxLightGridPoint GfxLightGridPoint;
typedef struct GfxLightDef GfxLightDef;
typedef struct GfxModelRigidSurface GfxModelRigidSurface;
typedef struct GfxModelSkinnedSurface GfxModelSkinnedSurface;
typedef struct GfxOccluder GfxOccluder;
typedef struct GfxOccluderEdge GfxOccluderEdge;
typedef struct GfxPortal GfxPortal;
typedef struct GfxRenderCommandExecState GfxRenderCommandExecState;
typedef struct GfxScene GfxScene;
typedef struct DpvsScene DpvsScene;
typedef struct GfxStateOverride GfxStateOverride;
typedef struct GfxStaticModelCachedSurface GfxStaticModelCachedSurface;
typedef struct GfxStaticModelDynamic GfxStaticModelDynamic;
typedef struct GfxStaticModelInstance GfxStaticModelInstance;
typedef struct GfxStaticSurface GfxStaticSurface;
typedef struct GfxSurface GfxSurface;
typedef struct GfxCmdSetViewport GfxCmdSetViewport;
typedef struct ImageList ImageList;
typedef struct GfxCmdDrawText GfxCmdDrawText;
typedef struct MaterialObj MaterialObj;
typedef struct MtlStateMapBitGroup MtlStateMapBitGroup;
typedef struct MtlStateMapBitName MtlStateMapBitName;
typedef struct MtlTextureFunctionDx7 MtlTextureFunctionDx7;
typedef struct OutdoorGlob OutdoorGlob;
typedef struct PassOptionDx7 PassOptionDx7;
typedef struct Cursor Cursor;
typedef struct PointLightPartition PointLightPartition;
typedef struct ShadowCookieGlob ShadowCookieGlob;
typedef struct SkinRigidXModelCmd SkinRigidXModelCmd;
typedef struct SkinStaticModelCachedCmd SkinStaticModelCachedCmd;
typedef struct SkinXModelCmd SkinXModelCmd;
typedef struct WaveletDecode WaveletDecode;
typedef struct WaveletHuffmanDecode WaveletHuffmanDecode;
typedef struct GfxCmdSetShadowCookie GfxCmdSetShadowCookie;
typedef struct DxState DxState;
typedef struct WaterGlob WaterGlob;
typedef struct lightGlob_type lightGlob_type;
typedef struct mnode_t mnode_t;
typedef struct r_global_permanent_t r_global_permanent_t;
typedef struct r_globals_load_t r_globals_load_t;
typedef struct r_lightmapGroup_t r_lightmapGroup_t;
typedef struct GfxBspLoad GfxBspLoad;
typedef struct GfxDebugFrameGlob GfxDebugFrameGlob;
typedef struct srfTriangles_t srfTriangles_t;
typedef struct static_model_cache_t static_model_cache_t;
typedef struct stream_dest_info_t stream_dest_info_t;
typedef struct stream_source_info_t stream_source_info_t;
typedef struct DxGlobals DxGlobals;
typedef struct r_globals_t r_globals_t;

typedef struct MaterialConstantDefObj MaterialConstantDefObj;
typedef struct MaterialShader MaterialShader;
typedef struct MaterialStateMap MaterialStateMap;
typedef struct MaterialStateMapRuleSet MaterialStateMapRuleSet;
typedef struct MaterialStreamRouting MaterialStreamRouting;
typedef struct MaterialTechnique MaterialTechnique;
typedef struct MaterialTechniqueSet MaterialTechniqueSet;
typedef struct MaterialVertexDeclaration MaterialVertexDeclaration;
typedef struct MaterialWaterDef MaterialWaterDef;

enum D3DXINCLUDE_TYPE {
    D3DXINC_LOCAL = 0,
    D3DXINC_SYSTEM = 1,
    D3DXINC_FORCE_DWORD = 2147483647
};

enum DpvsClipChildren {
    DPVS_DONT_CLIP_CHILDREN = 0,
    DPVS_CLIP_CHILDREN = 1
};

enum FogColorSrcEnum {
    FOG_COLOR_SRC_UNSPECIFIED = 0,
    FOG_COLOR_SRC_STANDARD = 1,
    FOG_COLOR_SRC_BLACK = 2
};

enum GfxDrawGroupGlueBehavior {
    R_DRAWGROUP_GLUE_ISSUE = 0,
    R_DRAWGROUP_GLUE_SKIP = 1
};

enum GfxDrawGroupType {
    R_DRAWGROUP_DEPTH_PREPASS = 0,
    R_DRAWGROUP_SHADOW_COOKIE = 1,
    R_DRAWGROUP_WORLD_SUN = 2,
    R_DRAWGROUP_WORLD_REMAINDER = 3,
    R_DRAWGROUP_POST_AA = 4,
    R_DRAWGROUP_COUNT = 5
};

enum GfxLockType {
    GFX_LOCK_BEGIN_FRAME = 0,
    GFX_LOCK_RESUME_FRAME = 1
};

enum GfxScreenshotType {
    R_SCREENSHOT_JPG = 0,
    R_SCREENSHOT_TGA = 1
};

enum MaterialShaderType {
    MTL_VERTEX_SHADER = 0,
    MTL_PIXEL_SHADER = 1
};

enum MtlParseSuccess {
    MTL_PARSE_SUCCESS = 0,
    MTL_PARSE_NO_MATCH = 1,
    MTL_PARSE_ERROR = 2
};

enum RenderTargetUsage {
    RENDERTARGET_USAGE_RENDER = 0,
    RENDERTARGET_USAGE_TEXTURE = 1,
    RENDERTARGET_USAGE_READBACK = 2
};

struct BuiltInMaterialTable {
    const char *name;
    _ValueType *material;
};

struct DSurface_s {
    short int modelIndex;
    short int subMatIndex;
};

struct DiskGfxVertex {
    vec3_t xyz;
    vec3_t normal;
    byte color[4];
    vec2_t texCoord;
    vec2_t lmapCoord;
    vec3_t tangent;
    vec3_t binormal;
};

struct DiskTriangleSoup {
    short unsigned int materialIndex;
    short unsigned int lightmapIndex;
    int firstVertex;
    short unsigned int vertexCount;
    short unsigned int indexCount;
    int firstIndex;
};

struct DxCapsCheckBits {
    int offset;
    DWORD setBits;
    DWORD clearBits;
    DxCapsResponse response;
    const char *msg;
};

struct DxCapsCheckInteger {
    int offset;
    int min;
    int max;
    DxCapsResponse response;
    const char *msg;
};

struct DxStencilDecode {
    int shift;
    D3DRENDERSTATETYPE renderState;
};

struct DxTextureOpDecode {
    DWORD enumerant;
    int usedArgs;
};

struct DxTextureStageEnums {
    D3DTEXTURESTAGESTATETYPE op;
    D3DTEXTURESTAGESTATETYPE arg[3];
};

struct FxMemMgr_Emitter {
    int _placeholder;
};

struct GfxAabbTree {
    vec3_t mins;
    vec3_t maxs;
    int surfaceCount;
    int startSurfIndex;
    int staticModelCount;
    int *staticModels;
    int childCount;
    struct GfxAabbTree *children;
};

struct GfxCachedShaderText {
    const char *name;
    const char *text;
    int textSize;
};

struct GfxCell {
    struct GfxCellWritable writable;
    vec3_t mins;
    vec3_t maxs;
    GfxAabbTree *aabbTree;
    int portalCount;
    GfxPortal *portals;
    int cullGroupCount;
    int *cullGroups;
    int occluderCount;
    GfxOccluder * *occluders;
    GfxSceneModelCellRef *modelRefs;
};

struct GfxCmdCall {
    struct GfxCmdHeader header;
    const void *subCmd;
};

struct GfxCmdClearScreen {
    GfxCmdHeader header;
    byte whichToClear;
    byte stencil;
    float depth;
    vec4_t color;
};

struct GfxCmdDrawSunPostEffects {
    GfxCmdHeader header;
    int viewIndex;
};

struct GfxCmdDrawSurfs {
    GfxCmdHeader header;
    GfxDrawSurfOrder order;
    GfxDrawSurf *drawSurfs;
    int drawSurfCount;
    MaterialTechniqueType techType;
};

struct GfxCmdSetClipPlanes {
    GfxCmdHeader header;
    int clipPlaneCount;
    vec4_t clipPlanes[1];
};

struct GfxCmdSetLightProperties {
    GfxCmdHeader header;
    int lightIndex;
    vec4_t position;
    vec4_t ambient;
    vec4_t color;
    vec4_t specular;
    GfxLightDef *lightDef;
};

struct GfxCmdSetMaterialColor {
    GfxCmdHeader header;
    vec4_t color;
};

struct GfxCmdSetRenderTarget {
    GfxCmdHeader header;
    GfxRenderTargetId renderTargetId;
};

struct GfxCmdStretchRaw {
    GfxCmdHeader header;
    int x;
    int y;
    int w;
    int h;
    int cols;
    int rows;
    const byte *data;
    int client;
    qboolean dirty;
};

struct GfxCmdStretchPic {
    GfxCmdHeader header;
    const Material *material;
    float x;
    float y;
    float w;
    float h;
    float s0;
    float t0;
    float s1;
    float t1;
    GfxColor color;
};

struct GfxCullGroup {
    vec3_t mins;
    vec3_t maxs;
    int surfaceCount;
    int startSurfIndex;
};

struct GfxDrawGroupCommands {
    Bool isIssuingGlue;
    int begin;
    int perView[4];
    int end;
};

struct GfxImageFileHeader {
    char tag[3];
    byte version;
    byte format;
    byte flags;
    short int dimensions[3];
    int fileSizeForPicmip[4];
};

struct GfxLightGridColors {
    byte rgb[3][8];
};

struct GfxLightGridPoint {
    unsigned int xyzHighBits;
    unsigned char xyzLowBitsAndSunVisible;
    unsigned char needsTrace;
    short unsigned int colorsIndex;
};

struct GfxLightDef {
    GfxLightType type;
    struct GfxLightImage cookie;
    GfxLightImage attenuation;
    const char *name;
};

struct GfxModelRigidSurface {
    GfxModelSurface surf;
    vec3_t boneAxis[4];
};

struct GfxModelSkinnedSurface {
    struct GfxModelSurface surf;
    int skinnedCachedOffset;
    union { void *variant; GfxVertexDx7 *dx7; GfxVertex *dx9; } skinnedVert;
};

struct GfxOccluder {
    int planeCount;
    DpvsPlane *planes;
    int edgeCount;
    GfxOccluderEdge *edges;
    int vertexCount;
    vec3_t *vertices;
    int ignoreStackLevel;
    int viewPlaneCount;
    DpvsPlane *viewPlanes;
};

struct GfxOccluderEdge {
    DpvsPlane * plane[2];
    vec3_t * vertex[2];
};

struct GfxPortal {
    struct GfxPortalWritable writable;
    struct DpvsPlane plane;
    GfxCell *cell;
    vec3_t *vertices;
    byte vertexCount;
    byte hullPointCount;
    vec2_t *hullPoints;
    vec3_t hullAxis[2];
};

struct GfxRenderCommandExecState {
    const void *cmd;
    int stackPos;
    const void * retCmd[2];
};

struct GfxScene {
    int viewCount;
    struct GfxSceneDef def;
    int dlightCount;
    GfxLight dlights[32];
    Bool dlightCulled[32];
    int drawSurfCount;
    GfxDrawSurf *drawSurfs;
    int polyCount;
    GfxSceneEntity sceneEnts[2046];
    int sceneEntMaterialCount;
    const Material * sceneMaterials[4096];
};

struct DpvsScene {
    GfxSceneModelCellRef modelRefs[4096];
    int modelRefCount;
};

struct GfxStateOverride {
    int stateBitsMaskClear[2];
    int stateBitsMaskSet[2];
};

struct GfxStaticModelCachedSurface {
    GfxModelSurface surf;
    GfxStaticModelSurfaceCached *surface;
    GfxEntity *ent;
};

struct GfxStaticModelDynamic {
    int viewCount;
    GfxStaticSurface *staticSurfs;
};

struct GfxStaticModelInstance {
    float cullDist;
    vec3_t origin;
    struct XModel *model;
    vec3_t mins;
    vec3_t maxs;
    vec3_t axis[3];
    float scale;
    vec3_t baseLightingCoords;
};

struct GfxStaticSurface {
    GfxStaticModelSurfaceCached * cachedLods[4];
};

struct GfxSurface {
    const Material *material;
    short unsigned int lightmapIndex;
    short unsigned int sortGroup;
    union { surfaceType_t *data; srfTriangles_t *tris; };
};

struct GfxCmdSetViewport {
    GfxCmdHeader header;
    GfxViewport viewport;
};

struct ImageList {
    int count;
    GfxImage * image[2048];
};

struct GfxCmdDrawText {
    GfxCmdHeader header;
    float x;
    float y;
    FontHandle font;
    float xScale;
    float yScale;
    GfxColor color;
    int style;
    int cursorPos;
    char cursor;
    int maxChars;
    char text[3];
};

struct MaterialObj {
    MaterialInfoObj info;
    int stateBits[2];
    short unsigned int textureCount;
    short unsigned int constantCount;
    MaterialTechniqueSet *techniqueSet;
    MaterialTextureDef *textures;
    MaterialConstantDefObj *constants;
};

struct MtlStateMapBitGroup {
    const char *name;
    const MtlStateMapBitName *bitNames;
    int stateBitsMask[2];
};

struct MtlStateMapBitName {
    const char *name;
    int bits;
};

struct MtlTextureFunctionDx7 {
    const char *name;
    int enumerant;
    int argCount;
    MtlTextureFunctionValidDx7 valid;
};

struct OutdoorGlob {
    vec3_t bbox[2];
    vec3_t scale;
    vec3_t invScale;
    vec3_t add;
    byte *pic;
};

struct PassOptionDx7 {
    const char *name;
    size_t valueOffset;
};

struct Cursor {
    Bits16 data;
    Bits16 mask;
    struct Point hotSpot;
};

struct PointLightPartition {
    const GfxLight *light;
    int firstDrawSurf;
    int drawSurfCount;
};

struct ShadowCookieGlob {
    float weightCap;
    int lastTime;
};

struct SkinRigidXModelCmd {
    surfaceType_t *surfs;
    int surfCount;
    GfxEntity *e;
    DObjAnimMat mat;
};

struct SkinStaticModelCachedCmd {
    GfxStaticModelSurfaceCached *cached;
    int smodelIndex;
    const Material *material;
};

struct SkinXModelCmd {
    surfaceType_t *surfs;
    GfxEntity *e;
    const DObjAnimMat *mat;
    int surfacePartBits[4];
    byte surfCount;
    byte boneCount;
    byte matOffset[64];
};

struct WaveletDecode {
    short unsigned int value;
    short unsigned int bit;
    const byte *data;
    int width;
    int height;
    int channels;
    int bpp;
    int mipLevel;
    Bool dataInitialized;
};

struct WaveletHuffmanDecode {
    short int value;
    short int bits;
};

struct GfxCmdSetShadowCookie {
    GfxCmdHeader header;
    D3DMATRIX lookupMatrix;
    float fade;
};

struct DxState {
    vec4_t vertexShaderConsts[256];
    vec4_t pixelShaderConsts[256];
    int refStateBits[2];
    int activeStateBits[2];
    int stencilRefValue;
    int refColorStageBits[8];
    int refAlphaStageBits[8];
    int activeColorStageBits[8];
    int activeAlphaStageBits[8];
    Bool gridLighting;
    byte genTexCoords[2];
    GfxRenderTargetId renderTargetId;
    int renderTargetWidth;
    int renderTargetHeight;
    GfxViewportBehavior viewportBehavior;
    IDirect3DSurface9 *renderTargetSurface;
    IDirect3DSurface9 *depthStencilSurface;
    D3DVIEWPORT9 viewport;
    Bool viewportIsNull;
    IDirect3DIndexBuffer9 *indexBuffer;
    struct { IDirect3DVertexBuffer9 *vb; int offset; int stride; } streams[1];
    volatile IDirect3DIndexBuffer9 *indexBufferDeselecting;
    volatile IDirect3DVertexBuffer9 *vertexBufferDeselecting;
    byte samplerState[16];
    const GfxImage * samplerImage[16];
    DWORD textureColorArg;
    IDirect3DPixelShader9 *pixelShader;
    IDirect3DVertexShader9 *vertexShader;
    IDirect3DVertexDeclaration9 *vertexDecl;
    DWORD fvf;
    byte alphaRef;
    D3DCOLOR textureFactor;
    D3DCOLOR ambientColor;
    int clipPlaneCount;
    struct { union GfxColor color; D3DFOGMODE mode; float start; float end; float density; } fog;
};

struct WaterGlob {
    complex_t H[16384];
    byte pixels[65536];
};

struct lightGlob_type {
    int defCount;
    GfxLightDef * defs[64];
};

struct mnode_t {
    int contents;
    int parent;
    int cellIndex;
    union mnode_info_t u;
};

struct r_global_permanent_t {
    qboolean materialLoaded;
    int materialCount;
    Material * sortedMaterials[1024];
    GfxImage *whiteImage;
    GfxImage *blackImage;
    GfxImage *identityNormalMapImage;
    GfxImage *specularityImage;
    GfxImage *lightmapWeightsImage;
    GfxImage * lightGridWeightsImage[2];
    GfxImage *outdoorImage;
    GfxLightDef *dlightDef;
    Material *defaultMaterial;
    Material *rawMaterial;
    Material *stencilPlaneMaterial;
    Material *whiteMaterial;
    Material *additiveMaterial;
    Material *pointMaterial;
    Material *lineMaterial;
    Material *clearAlphaStencilMaterial;
    Material *shadowClearMaterial;
    Material *shadowCookieOverlayMaterial;
    Material *shadowCookieBlurMaterial;
    Material *glareBlindMaterial;
    Material *normalFillMaterial;
    Material *alphaBlendFillMaterial;
    Material *alphaTestPassFillMaterial;
    Material *alphaTestFailFillMaterial;
    Material *additiveFillMaterial;
    Material *depthOnlyFillMaterial;
    Material *stencilOneSideKeepFillMaterial;
    Material *stencilOneSideRejectFillMaterial;
    Material *stencilTwoSideKeepFillMaterial;
    Material *stencilTwoSideRejectFillMaterial;
    Material *phongBumpFillMaterial;
    Material *phongBumpSpecFillMaterial;
    Material *colorChannelMixerMaterial;
    Material *frameColorDebugMaterial;
    Material *frameAlphaDebugMaterial;
    GfxImage *rawImage;
    GfxWorld *world;
    GfxImage *sunHalfAngleImage;
    GfxImage *waterColorImage;
    Material *feedbackReplaceMaterial;
    Material *feedbackBlendMaterial;
    Material * symmetricFilterMaterial[8];
    Material *shellShockMaterial;
    Material *glowSetupMaterial;
    Material *glowApplySkyBleedMaterial;
    Material *glowApplyBloomMaterial;
    int savedScreenTime;
    MaterialTextureDef rawTexdef;
};

struct r_globals_load_t {
    GfxOccluder * *occluderIndices;
    int *cullGroupIndices;
    GfxOccluder *occluders;
    vec3_t *portalVerts;
    GfxAabbTree *aabbTrees;
    int aabbTreeCount;
};

struct r_lightmapGroup_t {
    int wideCount;
    int highCount;
};

struct GfxBspLoad {
    const dheader_t *header;
    const byte *fileBase;
    int fileSize;
    const dmaterial_t *materials;
    int materialCount;
    r_lightmapMerge_t lmapMergeInfo[32];
};

struct GfxDebugFrameGlob {
    GfxCmdArray *restoreCmdList;
    GfxBackEndData *restoreFrontEndDataOut;
    Bool restoreSkinnedCache;
    GfxBackEndData frontEndDataOut;
};

struct srfTriangles_t {
    surfaceType_t surfaceType;
    vec3_t bounds[2];
    int firstVertex;
    short unsigned int vertexCount;
    short unsigned int indexCount;
    int (*indices)[4];
};

struct static_model_cache_t {
    static_model_tree_t trees[128];
    static_model_node_list_t freelist[5];
    static_model_tree_list_t usedlist;
    struct { int allocatedVerts; int usedVerts; } stats;
};

struct stream_dest_info_t {
    BYTE Usage;
    BYTE UsageIndex;
};

struct stream_source_info_t {
    BYTE Stream;
    BYTE Offset;
    BYTE Type;
};

struct DxGlobals {
    HINSTANCE hinst;
    IDirect3D9 *d3d9;
    void (*device)();
    UINT adapterIndex;
    D3DFORMAT backBufferFormat;
    int displayModeCount;
    D3DDISPLAYMODE displayModes[256];
    const char * resolutionNameTable[257];
    const char * refreshRateNameTable[257];
    char modeText[5120];
    int gpuSync;
    D3DMULTISAMPLE_TYPE multiSampleType;
    DWORD multiSampleQuality;
    int sunSpriteSamples;
    GfxRenderTarget renderTargets[13];
    IDirect3DSurface9 *singleSampleDepthStencilSurface;
    short unsigned int cubemapShotRes;
    short unsigned int cubemapShotPixelBorder;
    Bool deviceLost;
    Bool inScene;
    short unsigned int feedbackWidth;
    short unsigned int feedbackHeight;
    int targetWindowIndex;
    int windowCount;
    GfxWindowTarget windows[1];
    IDirect3DQuery9 *flushGpuQuery;
    int gpuSyncDelay;
    int gpuSyncAlreadyWaited;
    Bool flushGpuQueryIssued;
    int anisotropy;
    int maxAnisotropy;
    int maxClipPlanes;
    Bool stencilTwoSided;
    Bool stencilWrap;
    Bool slopeScaleDepthBias;
    Bool canMipCubemaps;
    Bool hasBlendOp;
    Bool hasSeparateAlphaBlend;
    Bool hasTransparencyMsaa;
    r_ib_state_t dynamicIndexBufferPool[1];
    r_ib_state_t *dynamicIndexBuffer;
    r_vb_state_t skinnedCacheVbPool[2];
    r_vb_state_t dynamicVertexBufferPool[1];
    r_vb_state_t *dynamicVertexBuffer;
    IDirect3DVertexBuffer9 *particleCloudVertexBuffer;
    IDirect3DIndexBuffer9 *particleCloudIndexBuffer;
    byte *skinnedCacheLockAddr;
    IDirect3DVertexBuffer9 *smodelCacheVb;
    r_index_t *smodelCacheIndices;
    int dynamicBufferFrame;
    byte *tempSkinBuf;
    int tempSkinPos;
};

struct r_globals_t {
    Bool registered;
    Bool forbidDelayLoadImages;
    Bool ignorePrecacheErrors;
    vec3_t viewOrg;
    vec3_t viewDir;
    int frameCountInternal;
    int markCount;
    int totalImageMemory;
    Material * materialHashTable[1024];
    int sceneWaterMapSetupsCount;
    water_t sceneWaterMapSetups[16];
    GfxFog fogSettings[5];
    int fogIndex;
    float sinTable[1024];
    complex_t fftTrigTable[256];
    int fftBitswap[256];
    struct DObj_s *modelDObj;
    char modelDObjBuf[100];
    trStatistics_t *stats;
    struct GfxLodParms lodParms;
    const GfxViewParms *debugViewParms;
    GfxStaticModelDynamic *smodelDyncs;
    GfxSurfaceDynamic *surfaces;
    GfxCullGroupDynamic *cullGroups;
};

struct MaterialConstantDefObj {
    const char *name;
    vec4_t literal;
};

struct MaterialShader {
    const char *name;
    void (*program)();
    short unsigned int programLen;
    byte shaderType;
    byte shaderVersion;
    union MaterialShaderPtr u;
};

struct MaterialStateMap {
    const char *name;
    const MaterialStateMapRuleSet * ruleSet[11];
};

struct MaterialStateMapRuleSet {
    int ruleCount;
    MaterialStateMapRule rules[1];
};

struct MaterialStreamRouting {
    MaterialStreamSource source;
    MaterialStreamDest dest;
};

struct MaterialTechnique {
    const char *name;
    short unsigned int flags;
    short unsigned int passCount;
    union MaterialPassArray passArray;
};

struct MaterialTechniqueSet {
    const char *name;
    MaterialTechnique * techniques[34];
};

struct MaterialVertexDeclaration {
    MaterialStreamRouting *data;
    int streamCount;
    void (*decl[4])();
};

struct MaterialWaterDef {
    int textureWidth;
    float horizontalWorldLength;
    float verticalWorldLength;
    float amplitude;
    float windSpeed;
    vec2_t windDirection;
    water_t *map;
};
#endif
