#ifndef CLEAN_PC_UNIVERSAL_COM_MATH_H
#define CLEAN_PC_UNIVERSAL_COM_MATH_H

#include "../../cod2_fwd.h"

enum errorParm_t {
    ERR_FATAL = 0,
    ERR_DROP = 1,
    ERR_SERVERDISCONNECT = 2,
    ERR_DISCONNECT = 3,
    ERR_SCRIPT = 4,
    ERR_SCRIPT_DROP = 5,
    ERR_LOCALIZATION = 6
};
typedef struct FxHelper FxHelper;

typedef struct GfxGammaRamp GfxGammaRamp;
typedef struct XAnimTree_s XAnimTree_s;
typedef struct dmodel_t dmodel_t;
typedef struct dnode_t dnode_t;

typedef struct moveclip_t moveclip_t;
typedef struct refexport_t refexport_t;
typedef struct refimport_t refimport_t;

typedef struct sightclip_t sightclip_t;

typedef struct sightpointtrace_t sightpointtrace_t;
typedef struct snd_overlay_info_t snd_overlay_info_t;
typedef struct snd_save_2D_sample_t snd_save_2D_sample_t;
typedef struct snd_save_3D_sample_t snd_save_3D_sample_t;
typedef struct snd_save_stream_t snd_save_stream_t;
typedef struct traceWork_t traceWork_t;
typedef struct viewState_t viewState_t;

typedef struct weaponParms weaponParms;
typedef enum errorParm_t errorParm_t;

typedef struct DObj_s DObj;
typedef struct DObjTrace_s DObjTrace;

typedef struct XAnim_s XAnim;

typedef struct weaponInfo_s weaponInfo_t;
#ifndef _WIN32
typedef int jmp_buf[39];
#endif

#include "../../../../EffectsCore/FxHelper.h"

struct GfxGammaRamp {
    short unsigned int entries[256];
};

struct XAnimTree_s {
    XAnim *anims;
    short unsigned int entnum;
    Bool bAbs;
    Bool bUseGoalWeight;
    short unsigned int infoArray[1];
};

struct dmodel_t {
    float mins[3];
    float maxs[3];
    int firstTriangle;
    int numTriangles;
    int firstSurface;
    int numSurfaces;
    int firstBrush;
    int numBrushes;
};

struct dnode_t {
    int planeNum;
    int children[2];
    int mins[3];
    int maxs[3];
};

struct moveclip_t {
    vec3_t mins;
    vec3_t maxs;
    vec3_t outerSize;
    TraceExtents extents;
    int passEntityNum;
    int passOwnerNum;
    int contentmask;
};

struct refexport_t {
    void (*Shutdown)(void);
    void (*BeginRegistration)(void *vidConfigOut);
    struct XModel * (*RegisterModel)(const char *name);
    GfxBrushModel * (*RegisterInlineModel)(int modelIndex);
    MaterialHandle (*RegisterMaterial)(const char *name, int baseImageFlags, int imageTrack);
    MaterialHandle (*RegisterRawImage)(const char *name, int baseImageFlags, int imageTrack);
    Bool (*Material_IsDefault)(MaterialHandle handle);
    void (*LoadWorld)(const char *name, int *checksum);
    void (*GetWorldBounds)(vec_t *min, vec_t *max);
    void (*FinishLoadingModels)(void);
    void (*SetIgnorePrecacheErrors)(qboolean ignore);
    qboolean (*GetIgnorePrecacheErrors)(void);
    int (*GetMinSpecImageMemory)(void);
    const char * (*GetMaterialName)(MaterialHandle handle);
    int (*GetMaterialSubimageCount)(MaterialHandle handle);
    Bool (*IsMaterialRefractive)(MaterialHandle handle);
    float (*GetFarPlaneDist)(void);
    void (*EndRegistration)(void);
    void (*ClearScene)(void);
    void (*SkinGfxEntityDelayed)(struct GfxEntity *ent);
    struct GfxEntity * (*AddRefEntityToScene)(const struct GfxEntity *refEnt, union GfxModel sceneModel, const struct centity_s *cent);
    void (*UpdateXModelBoundsDelayed)(struct GfxEntity *ent);
    void (*UpdateEffectsBolt)(void);
    void (*UpdateEffectsNonBolt)(void);
    void (*DefaultVertexFrames)(int vertCount, GfxWorldVertex *verts);
    void (*AddPolyToScene)(MaterialHandle materialHandle, unsigned short lmapIndex, unsigned short vertCount, const GfxWorldVertex *verts);
    void (*AddLightToScene)(const vec_t *org, float radius, float r, float g, float b);
    void (*InterpretSunLightParseParams)();
    void (*ResetSunLightParseParams)(void);
    void (*SetCullDist)(float dist);
    void (*SetFog)(int fogvar, float start, float end, int r, int g, int b, float density);
    void (*SwitchFog)(int fogvar, int startTime, int transitionTime);
    void (*ArchiveFogState)(void *memFile);
    void (*ClearFogs)(void);
    void (*SetSunLightOverride)(const vec_t *sunColor);
    void (*ResetSunLightOverride)(void);
    void (*RenderScene)(const refdef_t *refdef);
    int (*BeginDelayedDrawing)(void);
    void (*EndDelayedDrawing)(int marker);
    void (*IssueDelayedDrawing)(int marker);
    void (*ClearFlares)(void);
    void (*SetMaterialColor)(const vec_t *color);
    void (*DrawStretchPic)(float x, float y, float w, float h, float s0, float t0, float s1, float t1, const vec_t *color, MaterialHandle material);
    void (*DrawStretchPicRotate)(float x, float y, float w, float h, float s0, float t0, float s1, float t1, float angle, const vec_t *color, MaterialHandle material);
    void (*DrawStretchRaw)();
    void (*DrawQuadPic)(void *verts, const vec_t *color, MaterialHandle material);
    void (*DrawSprite)(MaterialHandle material, const unsigned char *rgbaColor, const vec_t *pos, float radius, float minScreenRadius, int renderFxFlags);
    void (*BeginFrame)(void);
    void (*EndFrame)(void);
    void (*BeginDebugFrame)(void);
    void (*EndDebugFrame)(void);
    void (*EndView)(int viewIndex);
    void (*DoneRenderingViews)(void);
    void (*SaveScreen)(void);
    void (*BlendSavedScreen)(int fadeMsec);
    void (*ClearScreen)(int whichToClear, const vec_t *color, float depth, int stencil);
    void (*SetViewport)(int x, int y, int width, int height);
    int (*MarkFragments)(void *points, const vec_t *origin, void *axis, float radius, int maxPoints, GfxWorldVertex *verts, int maxFragments, void *fragmentBuffer, MaterialHandle markMaterial);
    void (*ModelBounds)(GfxBrushModel *bmodel, vec_t *mins, vec_t *maxs);
    void (*TrackStatistics)(void *stats);
    qboolean (*PickMaterial)(const vec_t *org, const vec_t *dir, char *name, char *surfaceFlags, char *contents, int charLimit);
    FontHandle (*RegisterFont)(const char *fontName, int imageTrack);
    void (*ResetImageAllocations)(void);
    void (*FreeImageAllocations)(void);
    void (*BeginCubemapShot)(int pixelWidthHeight, int pixelBorder);
    void (*EndCubemapShot)(int shotIndex);
    void (*SaveCubemapShot)(const char *filename, int shotIndex, float n0, float n1);
    void (*LightingFromCubemapShots)(const vec_t *baseColor);
    void (*LocateDebugStrings)(void *strings, int stringCount, int maxStringCount);
    void (*LocateDebugLines)(void *lines, int lineCount, int maxLineCount);
    void (*AddPlume)(const vec_t *origin, int score, const vec_t *color, int duration);
    void (*ShutdownDebug)(void);
    void (*UpdateColor)(const vec_t *color_allies, const vec_t *color_axis);
    float (*NormalizedTextScale)(FontHandle font, float scale);
    int (*TextWidth)(const char *text, int maxChars, FontHandle font);
    int (*TextHeight)(FontHandle font);
    void (*DrawText)(const char *text, int maxChars, FontHandle font, float x, float y, float xScale, float yScale, const vec_t *color, int style);
    void (*DrawTextInSpace)(const char *text, FontHandle font, const vec_t *org, const vec_t *xPixelStep, const vec_t *yPixelStep, const vec_t *color);
    int (*ConsoleTextWidth)(const short int *string, int maxChars, FontHandle font);
    void (*DrawConsoleText)(const short int *string, int maxChars, FontHandle font, float x, float y, float xScale, float yScale, const vec_t *color, int style);
    void (*DrawTextWithCursor)(const char *text, int maxChars, FontHandle font, float x, float y, float xScale, float yScale, const vec_t *color, int style, int cursorPos, char cursor);
    void (*DObjGetSurfMaterials)(struct DObj_s *obj, int lod, MaterialHandle *matHandleArray);
    void (*DObjReplaceMaterial)(struct DObj_s *obj, int lod, int surfaceIndex, MaterialHandle material);
    const char * (*ParseSunLight)(void *params, const char *text);
    MaterialHandle (*Material_Duplicate)(MaterialHandle mtlCopy, const char *name);
    void (*DuplicateFont)(FontHandle fontCopy, const char *name);
    void *unknown86_95[10];
    struct XSurface_s * (*XModelReadSurface)(struct XModel *model, int *partBits, const unsigned char **pos, void *(*Alloc)(int));
    struct trXSkin_t * (*LoadXSkins)(struct XModel *model);
    void *unknown98_102[5];
    void (*SyncRenderThread)(void);
    void (*AbortRenderCommands)(void);
    void (*SyncGpu)(void);
    void *unknown106;
    void (*SetLodOrigin)(const refdef_t *refdef);
};

// Slot order pinned from the 1.0 client CL_InitRef stores (169 slots).
struct refimport_t {
    void (*Printf)(int print_level, const char *fmt, ...);
    void (*Error)(errorParm_t code, const char *fmt, ...);
    int (*Milliseconds)();
    void * (*Hunk_AllocInternal)(int size);
    void * (*Hunk_AllocateTempMemoryInternal)(int size);
    void * (*Z_MallocInternal)(int size);
    void (*Z_FreeInternal)(void *ptr);
    void * (*Hunk_AllocAlignInternal)(int size, int align);
    void * (*Z_VirtualReserveInternal)(int size);
    void (*Z_VirtualCommitInternal)(void *ptr, int size);
    void (*Z_VirtualDecommitInternal)(void *ptr, int size);
    void (*Z_VirtualFreeInternal)(void *ptr);
    void (*Hunk_FreeTempMemory)(void *buf);
    void (*Hunk_ClearTempMemory)();
    int (*Hunk_HideTempMemory)();
    void (*Hunk_ShowTempMemory)(void *mark);
    void * (*Hunk_AllocateTempMemoryHighInternal)(int size);
    void (*Hunk_ClearTempMemoryHigh)();
    void (*HiAlloc)();
    void (*HiSetOffset)();
    void (*HiOffToPtr)();
    void (*Sys_DirectXFatalError)();
    void (*Sys_ShowSplashWindow)();
    void (*Sys_HideSplashWindow)();
    void (*Sys_LoadingKeepAlive)();
    const dvar_t * (*Dvar_RegisterBool)(const char *dvarName, int value, int flags);
    const dvar_t * (*Dvar_RegisterInt)(const char *dvarName, int value, int min, int max, int flags);
    const dvar_t * (*Dvar_RegisterFloat)(const char *dvarName, float value, float min, float max, int flags);
    const dvar_t * (*Dvar_RegisterString)(const char *dvarName, const char *value, int flags);
    const dvar_t * (*Dvar_RegisterEnum)(const char *dvarName, const char **valueList, int defaultIndex, int flags);
    const dvar_t * (*Dvar_RegisterColor)(const char *dvarName, float r, float g, float b, float a, int flags);
    const dvar_t * (*Dvar_RegisterVec2)(const char *dvarName, float x, float y, float min, float max, int flags);
    const dvar_t * (*Dvar_RegisterVec3)(const char *dvarName, float x, float y, float z, float min, float max, int flags);
    const dvar_t * (*Dvar_RegisterVec4)(const char *dvarName, float x, float y, float z, float w, float min, float max, int flags);
    void (*Dvar_UnregisterSystem)(int sysFlag);
    void (*Dvar_ChangeResetValue)(const dvar_t *dvar, DvarValue value);
    int (*Dvar_IsAtDefaultValue)(const dvar_t *dvar);
    void (*Dvar_ClearModified)(const dvar_t *dvar);
    void (*Dvar_SetModified)(const dvar_t *dvar);
    void (*Dvar_UpdateEnumDomain)(const dvar_t *dvar, const char **stringTable);
    void (*Dvar_SetBool)(const dvar_t *dvar, int value);
    void (*Dvar_SetInt)(const dvar_t *dvar, int value);
    void (*Dvar_SetFloat)(const dvar_t *dvar, float value);
    void (*Dvar_SetString)(const dvar_t *dvar, const char *value);
    void (*Dvar_SetColor)(const dvar_t *dvar, float r, float g, float b, float a);
    void (*Dvar_SetVec2)(const dvar_t *dvar, float x, float y);
    void (*Dvar_SetVec3)(const dvar_t *dvar, float x, float y, float z);
    void (*Dvar_SetVec4)(const dvar_t *dvar, float x, float y, float z, float w);
    void (*Dvar_SetFromString)(const dvar_t *dvar, const char *string);
    void (*Dvar_SetBoolByName)(const char *name, int value);
    void (*Dvar_SetIntByName)(const char *name, int value);
    void (*Dvar_SetFloatByName)(const char *name, float value);
    void (*Dvar_SetStringByName)(const char *dvarName, const char *value);
    void (*Dvar_SetColorByName)(const char *dvarName, int r, int g, int b, int a);
    void (*Dvar_SetVec2ByName)(const char *dvarName, float x, float y);
    void (*Dvar_SetVec3ByName)(const char *dvarName, float x, float y, float z);
    void (*Dvar_SetVec4ByName)(const char *dvarName, float x, float y, float z, float w);
    void (*Dvar_SetFromStringByName)(const char *dvarName, const char *value);
    int (*Dvar_GetBool)(const char *dvarName);
    int (*Dvar_GetInt)(const char *dvarName);
    float (*Dvar_GetFloat)(const char *name);
    const char * (*Dvar_GetString)(const char *dvarName);
    const char * (*Dvar_GetVariantString)(const char *dvarName);
    const char * (*Dvar_EnumToString)(dvar_t *dvar);
    void (*Dvar_Reset)(void *dvar, int source);
    void (*Cmd_AddCommand)(const char *cmdName, xcommand_t function);
    void (*Cmd_RemoveCommand)(const char *cmdName);
    int (*Cmd_Argc)();
    char * (*Cmd_Argv)(int arg);
    void (*Cbuf_ExecuteText)(int exec_when, const char *text);
    int (*Com_SaveDvarsToBuffer)(const char **dvar_names, int count, char *buf, int bufsize);
    int (*Com_LoadDvarsFromBuffer)(const char **dvar_names, int count, const char *buf, const char *path);
    void * (*Com_GetBsp)(int *fileSize, unsigned int *checksum);
    unsigned int (*SEH_ReadCharFromString)(const char **ppsText, int *pbIsTrailingPunctuation);
    void (*CL_UpdateDebugData)();
    void (*CL_FlushDebugData)(int fromServer);
    void (*StatMon_Warning)(int type, int duration, const char *materialName);
    int (*FS_ReadFile)(const char *qpath, void **buffer);
    void (*FS_FreeFile)(void *buffer);
    int (*FS_FOpenFileRead)(const char *filename, fileHandle_t *file, int uniqueFILE);
    char ** (*FS_ListFiles)(const char *path, const char *extension, int behavior, int *numfiles, int allocTrackType);
    void (*FS_FreeFileList)(char **list, int allocTrackType);
    int (*FS_FileExists)(const char *path);
    int (*FS_WriteFile)(const char *path, const void *buffer, int size);
    int (*FS_FOpenFileByMode)(const char *qpath, fileHandle_t *f, fsMode_t mode);
    void (*FS_FCloseFile)(fileHandle_t h);
    int (*FS_Read)(void *buffer, int len, fileHandle_t h);
    int (*FS_Write)(const void *buf, int len, int fh);
    void (*CM_SaveLump)(int lumpnum, byte *newLump, int size, int *checksum);
    void (*CM_BoxTrace)(trace_t *results, const vec_t *start, const vec_t *end, const vec_t *mins, const vec_t *maxs, clipHandle_t model, int brushmask);
    int (*CM_BoxSightTrace)(int oldHitNum, const vec_t *start, const vec_t *end, const vec_t *mins, const vec_t *maxs, clipHandle_t model, int brushmask);
    int (*CM_RayTriangleIntersect)();
    struct XModel * (*XModelPrecache)(const char *name, Alloc_t Alloc, Alloc_t AllocColl);
    int (*XModelGetSurfaces)(const XModel *model, struct XSurface_s ***surfaces, int lod, int **partBits);
    int (*XModelBad)(const XModel *model);
    void (*Hunk_OverrideDataForFile)(int type, const char *name, void *data);
    int (*XModelGetNumLods)(const XModel *model);
    void (*XModelSetTestLods)(int lodLevel, float dist);
    int (*XModelGetLodForDist)(const XModel *model, float dist);
    float (*XModelGetLodOutDist)(const XModel *model);
    const char * (*XModelGetSurfaceName)(const XModel *model, int subMatIndex, int lod);
    const char * (*XModelGetName)(const XModel *model);
    unsigned char (*XModelGetFlags)(const XModel *model);
    int (*XModelNumBones)(const XModel *model);
    const trXSkin_t * (*XModelGetSkins)(const XModel *model);
    int (*XModelGetMemUsage)(const XModel *model);
    const char * (*XModelGetLodName)(const XModel *model, int lod);
    void * (*XModelGetBasePose)(const XModel *model);
    void * (*XModelGetBasePoseBone)(const XModel *model, int skelMatBoneOffset);
    int (*DObjBad)(const DObj *obj);
    void (*DB_EnumXAssets)(int type, void (*func)(union XAssetHeader, void *), void *data, int overrides);
    void *(*Sys_GetValue)(int valueIndex);
    int (*Sys_IsMainThread)();
    void (*Sys_ReturnFalse)();
    void (*DObjGetBounds)(const DObj *obj, vec_t *mins, vec_t *maxs);
    void * (*DObjGetSurface)(const DObj *obj, int modelIndex, int subMatIndex, int lod);
    int (*DObjGetNumModels)(const DObj *obj);
    int (*DObjGetNumSurfaces)(const DObj *obj, char *lods);
    int (*DObjGetSurfaceList)(const DObj *obj, void *surfaces, int *partBits, char *lods);
    int * (*DObjGetPartBits)();
    void * (*DObjGetRotTransArray)(const DObj *obj);
    int (*DObjSkelAreBonesUpToDate)(const DObj *obj, int *partBits);
    int (*DObjGetMatOffset)(const DObj *obj, int modelIndex);
    struct XModel * (*DObjGetModel)(const DObj *obj, int modelIndex);
    const char * (*DObjGetSurfaceName)(DObj *obj, int modelIndex, int subMatIndex, int lod);
    void (*DObjCreate)(DObjModel_s *dobjModels, unsigned int numModels, XAnimTree_s *tree, char *buf, unsigned int entnum);
    int (*DObjGetAllocSkelSize)(const DObj *obj);
    void (*DObjCreateSkel)(const DObj *obj, char *buf, int timeStamp);
    void (*DObjCalcAnim)();
    void (*DObjCalcSkel)();
    int (*DObjNumBones)(const DObj *obj);
    void (*DObjGetBoneInfo)(const DObj *obj, void *boneInfo);
    int (*DObjGetLodForDist)(const DObj *obj, int modelIndex, float dist);
    float (*DObjGetMaxLodOutDist)(const DObj *obj);
    void (*DObjCompleteHierarchyBits)(const DObj *obj, int *partBits);
    void (*DObjSetModel)(DObj *obj, const XModel *model);
    void (*CIN_UploadCinematic)(int handle);
    int (*CIN_PlayCinematic)(const char *arg, int x, int y, int w, int h, int systemBits);
    e_status (*CIN_RunCinematic)(int handle);
    void (*CG_DObjCalcPose)();
    void (*CG_CullIn)();
    void (*CG_UsedDObjCalcPose)();
    void (*CG_AddMarks)();
    void (*GetRealWidthFromVirtualWidth)();
    void (*GetRealHeightFromVirtualHeight)();
    void (*GetVirtualWidthFromRealWidth)();
    void (*GetVirtualHeightFromRealHeight)();
    void * (*CM_GetPlaneNum)(int planeNum);
    void (*slot148)();
    void (*slot149)();
    void (*slot150)();
    void (*slot151)();
    void (*slot152)();
    void (*slot153)();
    void (*slot154)();
    void (*slot155)();
    void (*slot156)();
    void (*slot157)();
    void (*slot158)();
    void (*slot159)();
    void (*slot160)();
    void (*slot161)();
    void (*slot162)();
    void (*slot163)();
    void (*slot164)();
    void (*FX_UpdateNonBolt)();
    void (*FX_UpdateBolt)();
    void (*FX_UpdateAndDraw)();
    const char * (*CL_GetHudMsgIconMaterialName)(int index);
};

struct sightclip_t {
    vec3_t mins;
    vec3_t maxs;
    vec3_t outerSize;
    vec3_t start;
    vec3_t end;
    int passEntityNum[2];
    int contentmask;
};

struct sightpointtrace_t {
    vec3_t start;
    vec3_t end;
    int passEntityNum[2];
    int contentmask;
    qboolean locational;
};

struct snd_overlay_info_t {
    const char *pszSampleName;
    float fBaseVolume;
    float fCurVolume;
    int dist;
    float fPitch;
};

struct snd_save_2D_sample_t {
    float fraction;
    float pitch;
    float volume;
    float pan;
};

struct snd_save_3D_sample_t {
    float fraction;
    float pitch;
    float volume;
    vec3_t org;
};

struct snd_save_stream_t {
    float fraction;
    int rate;
    float basevolume;
    float volume;
    float pan;
    vec3_t org;
};

struct traceWork_t {
    struct TraceExtents extents;
    vec3_t delta;
    vec_t deltaLen;
    vec_t deltaLenSq;
    vec3_t midpoint;
    vec3_t halfDelta;
    vec3_t halfDeltaAbs;
    vec3_t size;
    vec3_t bounds[2];
    int contents;
    qboolean isPoint;
    qboolean axialCullOnly;
    float radius;
    float offsetZ;
    vec3_t radiusOffset;
    struct TraceThreadInfo threadInfo;
};

struct viewState_t {
    playerState_t *ps;
    int damageTime;
    int time;
    float v_dmg_pitch;
    float v_dmg_roll;
    float xyspeed;
    float frametime;
    float fLastIdleFactor;
    int *weapIdleTime;
};

struct weaponParms {
    vec3_t forward;
    vec3_t right;
    vec3_t up;
    vec3_t muzzleTrace;
    vec3_t gunForward;
    WeaponDef *weapDef;
};
inline int I_fround(const float x)
{
    // fistp rounds to nearest; the bias sends exact halves up
    const double bias = 1.0 / (1 << 30);
    int result;

    __asm fld x
    __asm fadd bias
    __asm fistp result

    return result;
}
static inline int FastCeil(const float x)
{
    // fistp rounds to nearest; 0.5 - 2^-30 sends non-integers up and keeps
    // exact integers in place
    const double bias = 0.5 - 1.0 / (1 << 30);
    int result;

    __asm fld x
    __asm fadd bias
    __asm fistp result

    return result;
}
// Original name unknown.
static inline int FastFloor(const float x)
{
    // fistp rounds to nearest; 0.5 - 2^-30 sends non-integers down and keeps
    // exact integers in place
    const double bias = 0.5 - 1.0 / (1 << 30);
    int result;

    __asm fld x
    __asm fsub bias
    __asm fistp result

    return result;
}
#endif
