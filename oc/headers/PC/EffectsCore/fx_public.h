#ifndef CLEAN_PC_EFFECTSCORE_FX_PUBLIC_H
#define CLEAN_PC_EFFECTSCORE_FX_PUBLIC_H

#include "../../cod2_fwd.h"

typedef struct Cloud Cloud;
typedef struct Cylinder Cylinder;
typedef struct EffectPrimitive EffectPrimitive;
typedef struct Emitter Emitter;
typedef struct Flash Flash;
typedef struct FxBoltFrame FxBoltFrame;
typedef struct Light Light;
typedef struct Line Line;
typedef struct OrientedParticle OrientedParticle;
typedef struct Tail Tail;

typedef struct EffectCluster EffectCluster;
typedef struct FxCurve FxCurve;
typedef struct FxFlagEntry FxFlagEntry;
typedef struct BackCompatibleParameters BackCompatibleParameters;
typedef struct FxScheduler FxScheduler;
typedef struct GPObject GPObject;
typedef struct GPValue GPValue;
typedef struct GenericParser2 GenericParser2;
typedef struct ScheduledEffect ScheduledEffect;
typedef struct SortedEffect SortedEffect;
typedef union TMediaElement TMediaElement;
typedef struct PrimitiveTemplate PrimitiveTemplate;
typedef struct TextPool TextPool;

struct Cylinder {
    int _placeholder;
};

struct Emitter {

    char _base[0x24c];

    vec3_t emitPos;
    vec3_t initialVel;
    vec3_t velocityDelta;
    int emitLastTime;
    float emitStep;
    float spawnSize;
    char _gap2[8];
    float spawnDensity;
    char _gap3[4];
    float spawnStep;
    void *emitFx;
    float spawnVariance;
    char _tail[4];

};

struct Flash {
    int _placeholder;
};

struct EffectPrimitive {
    const EffectTemplate *fx;
    const PrimitiveTemplate *primTemp;
    FxBoltFramePtr boltFrame;
};

struct FxBoltFrame {
    int refCount;
    union {
        int cachedServerTime;
        int mTime;
    };
    orientation_t orientation;
    struct FxBoltFrame *next;
    struct FxBoltInfo mBolt;

};

struct Line {

    char _base[0x1C4];
    vec3_t endpoint;
};

struct Light {
    struct Effect base;
    float colorBlendFactor;
    float sizeBlendFactor;
    FxChannelInstance colorChannelInstance;
    FxChannelInstance colorRandChannelInstance;
    FxChannelInstance sizeChannelInstance;
    FxChannelInstance sizeRandChannelInstance;

};

struct OrientedParticle {

    char _base[0x1C4];
    vec3_t normal;
};

struct Cloud {

    struct Particle base;
    vec3_t randomDirection;
    char _gap[4];
    Bool useLengthFlag;
    char _pad[3];
    float randomLengthBlend;
    FxChannelInstance heightChannel;
    FxChannelInstance heightRandChannel;

};

struct Tail {

    struct Particle base;
    float endpoint[3];
    float tailLength;
    float lengthBlendFactor;
    FxChannelInstance lengthChannelInstance;
    FxChannelInstance lengthRandChannelInstance;

};

COD2_ASSERT_FIELD(struct Emitter, _base, 0x000);
COD2_ASSERT_FIELD(struct Emitter, emitPos, 0x24c);
COD2_ASSERT_FIELD(struct Emitter, initialVel, 0x258);
COD2_ASSERT_FIELD(struct Emitter, velocityDelta, 0x264);
COD2_ASSERT_FIELD(struct Emitter, spawnSize, 0x278);
COD2_ASSERT_FIELD(struct Emitter, _gap2, 0x27c);
COD2_ASSERT_FIELD(struct Emitter, spawnDensity, 0x284);
COD2_ASSERT_FIELD(struct Emitter, _gap3, 0x288);
COD2_ASSERT_FIELD(struct Emitter, spawnStep, 0x28c);
COD2_ASSERT_FIELD(struct Emitter, spawnVariance, 0x294);
COD2_ASSERT_FIELD(struct Emitter, _tail, 0x298);
COD2_ASSERT_FIELD(struct FxBoltFrame, refCount, 0x00);
COD2_ASSERT_FIELD(struct FxBoltFrame, orientation, 0x08);
COD2_ASSERT_FIELD(struct Line, _base, 0x000);
COD2_ASSERT_FIELD(struct Line, endpoint, 0x1C4);
COD2_ASSERT_FIELD(struct Light, base, 0x00);
COD2_ASSERT_FIELD(struct Light, colorBlendFactor, 0xc4);
COD2_ASSERT_FIELD(struct Light, sizeBlendFactor, 0xc8);
COD2_ASSERT_FIELD(struct Light, colorChannelInstance, 0xcc);
COD2_ASSERT_FIELD(struct Light, colorRandChannelInstance, 0xd8);
COD2_ASSERT_FIELD(struct Light, sizeChannelInstance, 0xe4);
COD2_ASSERT_FIELD(struct Light, sizeRandChannelInstance, 0xf0);
COD2_ASSERT_FIELD(struct OrientedParticle, _base, 0x000);
COD2_ASSERT_FIELD(struct OrientedParticle, normal, 0x1C4);
COD2_ASSERT_FIELD(struct Cloud, base, 0x000);
COD2_ASSERT_FIELD(struct Cloud, _gap, 0x258);
COD2_ASSERT_FIELD(struct Cloud, _pad, 0x25d);
COD2_ASSERT_FIELD(struct Tail, base, 0x000);
COD2_ASSERT_FIELD(struct Tail, endpoint, 0x24c);
COD2_ASSERT_FIELD(struct Tail, lengthChannelInstance, 0x260);
COD2_ASSERT_FIELD(struct Tail, lengthRandChannelInstance, 0x26c);

enum FxChannelId {
    FXCHAN_COLOR = 0,
    FXCHAN_COLOR_RAND = 1,
    FXCHAN_ALPHA = 2,
    FXCHAN_ALPHA_RAND = 3,
    FXCHAN_SIZE = 4,
    FXCHAN_SIZE_RAND = 5,
    FXCHAN_SIZE2 = 6,
    FXCHAN_SIZE2_RAND = 7,
    FXCHAN_LENGTH = 8,
    FXCHAN_LENGTH_RAND = 9,
    FXCHAN_ROTATION_DELTA = 10,
    FXCHAN_ROTATION_DELTA_RAND = 11,
    FXCHAN_VELOCITY_X = 12,
    FXCHAN_VELOCITY_Y = 13,
    FXCHAN_VELOCITY_Z = 14,
    FXCHAN_VELOCITY_X_RAND = 15,
    FXCHAN_VELOCITY_Y_RAND = 16,
    FXCHAN_VELOCITY_Z_RAND = 17,
    FXCHAN_VELOCITY2_X = 18,
    FXCHAN_VELOCITY2_Y = 19,
    FXCHAN_VELOCITY2_Z = 20,
    FXCHAN_VELOCITY2_X_RAND = 21,
    FXCHAN_VELOCITY2_Y_RAND = 22,
    FXCHAN_VELOCITY2_Z_RAND = 23,
    FXCHAN_COUNT = 24
};

struct EffectCluster {
    vec3_t origin;
    int refCount;
};

struct FxCurve {
    int dimensionCount;
    int keyCount;
    float keys[1];
};

struct FxFlagEntry {
    const char *flag;
    unsigned int masks[2];
};

struct BackCompatibleParameters {
    FxChannelBackwardCompatible fxChannels[24];
};

struct FxScheduler {
    int mSeed;
    ScheduledEffect *mScheduledHead;
    int mScheduledCount;
};

struct GPObject {
    const char *name;
    struct GPObject *next;
    struct GPObject *sortedNext;
    struct GPObject *sortedPrev;
};

struct GPValue {
    const char *name;
    struct GPObject *next;
    struct GPObject *sortedNext;
    struct GPObject *sortedPrev;
    struct GPValue *valueList;
};

struct GenericParser2 {
    struct GPGroup group;
    struct TextPool *textPoolList;
    byte writeable;
};

struct ScheduledEffect {
    const EffectTemplate *mFx;
    int mPrimIndex;
    int mStartTime;
    FxBoltInfo mBolt;
    vec3_t mOrigin;
    vec3_t mAxis[3];
    int mSeed;
    int mIndexInBatch;
    int mScheduledNext;
};

struct SortedEffect {
    Effect *effect;
    float distSq;
};

union TMediaElement {
    struct XModel *model;
    struct Material *material;
    union EffectTemplateLoadPtr u;
    void *data;
};

struct PrimitiveTemplate {
    char mName[32];
    char mMaterialImpact[32];
    PrimType mType;
    int mParentPrimIndex;
    struct FxRange mSpawnDelay;
    FxRange mSpawnCount;
    FxRange mLife;
    FxRange mSpawnRange;
    struct MediaHandles mMediaHandles;
    MediaHandles mImpactFxHandles;
    MediaHandles mDeathFxHandles;
    MediaHandles mEmitterFxHandles;
    MediaHandles mPlayFxHandles;
    int mAttributeFlags;
    int mSpawnFlags;
    int mGroupFlags;
    Bool mNonUniformScale;
    Bool useLength;
    vec3_t mMin;
    vec3_t mMax;
    FxRange mOrigin1X;
    FxRange mOrigin1Y;
    FxRange mOrigin1Z;
    FxRange mOrigin2X;
    FxRange mOrigin2Y;
    FxRange mOrigin2Z;
    FxRange mRadius;
    FxRange mHeight;
    FxRange mWindModifier;
    FxChannel mFxChannels[24];
    FxRange mRotation;
    FxRange mAngle1;
    FxRange mAngle2;
    FxRange mAngle3;
    FxRange mAngle1Delta;
    FxRange mAngle2Delta;
    FxRange mAngle3Delta;
    FxRange mGravity;
    FxRange mDensity;
    FxRange mVariance;
    FxRange mTexCoordS;
    FxRange mTexCoordT;
    FxRange mElasticity;
    StartFrameMode mSequenceStartFrameMode;
    int mSequenceFixedFrameValue;
    PlayRateMode mSequencePlayRateMode;
    float mSequenceFixedFpsValue;
    LoopMode mSequenceLoopMode;
    int mSequenceLoopTimes;
    float spawnFrustumCullRadius;
};

struct TextPool {
    char *data;
    struct TextPool *next;
    int capacity;
    int used;
};
#endif
