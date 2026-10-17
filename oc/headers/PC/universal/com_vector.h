#ifndef CLEAN_PC_UNIVERSAL_COM_VECTOR_H
#define CLEAN_PC_UNIVERSAL_COM_VECTOR_H

#include "../../cod2_fwd.h"

enum GfxPrimStatsTarget {
    GFX_PRIM_STATS_WORLD = 0,
    GFX_PRIM_STATS_SMODELCACHED = 1,
    GFX_PRIM_STATS_SMODELRIGID = 2,
    GFX_PRIM_STATS_XMODELCACHED = 3,
    GFX_PRIM_STATS_XMODELRIGID = 4,
    GFX_PRIM_STATS_XMODELSKINNED = 5,
    GFX_PRIM_STATS_BMODEL = 6,
    GFX_PRIM_STATS_FX = 7,
    GFX_PRIM_STATS_HUD = 8,
    GFX_PRIM_STATS_DEBUG = 9,
    GFX_PRIM_STATS_CODE = 10,
    GFX_PRIM_STATS_COUNT = 11
};

enum hitLocation_t {
    HITLOC_NONE = 0,
    HITLOC_HELMET = 1,
    HITLOC_HEAD = 2,
    HITLOC_NECK = 3,
    HITLOC_TORSO_UPR = 4,
    HITLOC_TORSO_LWR = 5,
    HITLOC_R_ARM_UPR = 6,
    HITLOC_L_ARM_UPR = 7,
    HITLOC_R_ARM_LWR = 8,
    HITLOC_L_ARM_LWR = 9,
    HITLOC_R_HAND = 10,
    HITLOC_L_HAND = 11,
    HITLOC_R_LEG_UPR = 12,
    HITLOC_L_LEG_UPR = 13,
    HITLOC_R_LEG_LWR = 14,
    HITLOC_L_LEG_LWR = 15,
    HITLOC_R_FOOT = 16,
    HITLOC_L_FOOT = 17,
    HITLOC_GUN = 18,
    HITLOC_NUM = 19
};

typedef struct GfxCmdDrawQuadPic GfxCmdDrawQuadPic;

typedef struct GfxCmdDrawTextInSpace GfxCmdDrawTextInSpace;

typedef struct GfxCmdStretchPicRotate GfxCmdStretchPicRotate;

typedef struct GfxDrawPrimArgs GfxDrawPrimArgs;

typedef struct GfxMarkFragment GfxMarkFragment;

typedef struct MantleResults MantleResults;
typedef struct SkinBuffers SkinBuffers;
typedef struct itemInfo_t itemInfo_t;

typedef struct locTraceWork_t locTraceWork_t;

typedef struct pml_t pml_t;

typedef struct pmove_t pmove_t;
typedef struct pointtrace_t pointtrace_t;

typedef struct weaponState_t weaponState_t;

typedef enum GfxPrimStatsTarget GfxPrimStatsTarget;

typedef enum hitLocation_t hitLocation_t;
typedef struct DObjModel_s DObjModel;
typedef struct XAnimNotify_s XAnimNotify;

typedef struct dvar_s dvar_t;
typedef struct leafList_s leafList_t;
typedef struct localEntity_s localEntity_t;
typedef int clipHandle_t;

typedef short unsigned int scr_thread_t;

struct GfxCmdDrawQuadPic {
    GfxCmdHeader header;
    const Material *material;
    vec2_t verts[4];
    GfxColor color;
};

struct GfxCmdStretchPicRotate {
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
    float rotation;
};

struct GfxDrawPrimArgs {
    int firstVertexFromBase;
    int vertexCount;
    int primCount;
    union { struct { int baseVertex; int baseIndex; } buf; struct { int vertexStride; } tess; } u;
};

struct MantleResults {
    vec3_t dir;
    vec3_t startPos;
    vec3_t ledgePos;
    vec3_t endPos;
    int flags;
    int duration;
};

struct GfxCmdDrawTextInSpace {
    GfxCmdHeader header;
    vec3_t org;
    FontHandle font;
    vec3_t xPixelStep;
    vec3_t yPixelStep;
    GfxColor color;
    char text[4];
};

struct GfxMarkFragment {
    MaterialHandle markMaterial;
    short unsigned int lmapIndex;
    byte pointCount;
    int firstPoint;
};

struct SkinBuffers {
    DObjSkelMat matArrayBuf[128];
    union { GfxSModelCachedVertexDx7 dx7[512]; GfxSModelCachedVertex dx9[512]; } verts;
};

struct itemInfo_t {
    qboolean registered;
    struct XModel * models[2];
    MaterialHandle icons[4];
    snd_alias_list_t *pickupSound;
    snd_alias_list_t *ammoPickupSound;
};

struct locTraceWork_t {
    int contents;
    TraceExtents extents;
};

struct pointtrace_t {
    TraceExtents extents;
    int passEntityNum;
    int passOwnerNum;
    int contentmask;
    qboolean bLocational;
    unsigned char *priorityMap;
};

struct pml_t {
    vec3_t forward;
    vec3_t right;
    vec3_t up;
    float frametime;
    int msec;
    qboolean walking;
    qboolean groundPlane;
    qboolean almostGroundPlane;
    trace_t groundTrace;
    float impactSpeed;
    vec3_t previous_origin;
    vec3_t previous_velocity;
    int previous_waterlevel;
};

struct pmove_t {
    playerState_t *ps;
    usercmd_t cmd;
    usercmd_t oldcmd;
    int tracemask;
    int numtouch;
    int touchents[32];
    vec3_t mins;
    vec3_t maxs;
    float xyspeed;
    qboolean proneChange;
    byte handler;
    Bool mantleStarted;
    vec3_t mantleEndPos;
    int mantleDuration;
};

struct weaponState_t {
    playerState_t *ps;
    float xyspeed;
    float frametime;
    vec3_t vLastMoveAng;
    float fLastIdleFactor;
    int time;
    int damageTime;
    float v_dmg_pitch;
    float v_dmg_roll;
    vec3_t vGunOffset;
    vec3_t vGunSpeed;
    vec3_t swayAngles;
    int *weapIdleTime;
};
#endif
