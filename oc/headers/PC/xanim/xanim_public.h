#ifndef CLEAN_PC_XANIM_XANIM_PUBLIC_H
#define CLEAN_PC_XANIM_XANIM_PUBLIC_H

#include "../../cod2_fwd.h"

typedef struct XModelConfig XModelConfig;
typedef struct DSurface_s DSurface;
typedef void * (*Alloc_t)(int);   /* real allocator(size) sig; matches common_types.h */

typedef struct TestLod TestLod;
typedef struct XAnimCalcAnimInfo XAnimCalcAnimInfo;
typedef struct XAnimDeltaPart XAnimDeltaPart;
typedef struct XAnimDeltaPartQuat XAnimDeltaPartQuat;
typedef struct XAnimDeltaPartTrans XAnimDeltaPartTrans;
typedef struct XAnimNotifyInfo XAnimNotifyInfo;
typedef struct XAnimPartQuat XAnimPartQuat;
typedef struct XAnimPartTrans XAnimPartTrans;
typedef struct XAnimSimpleRotPos XAnimSimpleRotPos;
typedef struct XAnimInfo XAnimInfo;
typedef struct XAnimToXModel XAnimToXModel;
typedef struct XModelCollSurf_s XModelCollSurf_s;
typedef struct XModelCollTri_s XModelCollTri_s;
typedef struct XModelDefault XModelDefault;

struct XModelConfigEntry {
    char filename[1024];
    float dist;
};

struct XModelConfig {
    XModelConfigEntry entries[4];
    float mins[3];
    float maxs[3];
    int collLod;
    unsigned char flags;
};

struct TestLod {
    Bool enabled;
    float dist;
};

struct XAnimCalcAnimInfo {
    DObjAnimMat rotTransArray[512];
    int animPartBits[4];
    int ignorePartBits[4];
};

struct XAnimDeltaPart {
    XAnimDeltaPartTrans *trans;
    XAnimDeltaPartQuat *quat;
};

struct XAnimDeltaPartQuat {
    short unsigned int size;
    union XAnimDeltaPartQuatData u;
};

struct XAnimDeltaPartTrans {
    short unsigned int size;
    union XAnimDeltaPartTransData u;
};

struct XAnimNotifyInfo {
    ScriptString name;
    float time;
};

struct XAnimPartQuat {
    short unsigned int size;
    union XAnimPartQuatData u;
};

struct XAnimPartTrans {
    short unsigned int size;
    union XAnimPartTransData u;
};

struct XAnimSimpleRotPos {
    vec2_t rot;
    float posWeight;
    vec3_t pos;
};

struct XAnimInfo {
    short unsigned int notifyChild;
    short int notifyIndex;
    short unsigned int notifyName;
    short unsigned int notifyType;
    short unsigned int prev;
    short unsigned int next;
    struct XAnimState s;
};

struct XAnimToXModel {
    int partBits[4];
    unsigned char boneIndex[128];
};

struct XModelCollSurf_s {
    XModelCollTri *collTris;
    int numCollTris;
    vec3_t mins;
    vec3_t maxs;
    int boneIdx;
    int contents;
    int surfFlags;
};

struct XModelCollTri_s {
    vec4_t plane;
    vec4_t svec;
    vec4_t tvec;
};

struct XModelDefault {
    scr_string_t boneNames[1];
    XBoneHierarchy hierarchy;
    XModelParts modelParts;
    XModelSurfs modelSurf;
    XBoneInfo boneInfo;
    unsigned char partClassification[1];
    short unsigned int surfNames[1];
};
#endif
