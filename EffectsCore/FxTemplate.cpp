#include "../qcommon/qcommon.h"

struct FxCurve;
struct FxEffectDef;

struct GPObject
{
	const char *GetName() { return mName; }
	GPObject *GetNext() { return mNext; }

	const char *mName;
	GPObject *mNext;
	GPObject *mSortedNext;
	GPObject *mSortedPrev;
};

struct GPValue : GPObject
{
	GPValue *GetNext() { return (GPValue *)mNext; }
	bool IsList();
	const char *GetTopValue();
	GPObject *GetList() { return mList; }

	GPObject *mList;
};

struct GPGroup : GPObject
{
	GPValue *GetPairs() { return mPairs; }
	GPGroup *GetSubGroups() { return mSubGroups; }
	GPGroup *GetNext() { return (GPGroup *)mNext; }

	GPValue *mPairs;
	GPValue *mPairsSorted;
	GPValue *mPairsLast;
	GPGroup *mSubGroups;
	GPGroup *mSubGroupsSorted;
	GPGroup *mSubGroupsLast;
	GPGroup *mParent;
	bool mClean;
};

struct FxRange
{
	float min;
	float max;
};

struct FxChannel
{
	const FxCurve *curve;
	FxRange scaleRange;
};

// Pre-curve channel description kept for old effect files.
struct FxChannelBackwardCompatible
{
	FxRange start[3];
	FxRange end[3];
	FxRange parm;
	int flags;
	// Set once any key of the channel was parsed; original name unknown.
	bool used;
};

struct BackCompatibleParameters
{
	FxChannelBackwardCompatible fxChannels[24];
};

enum FxChannelId
{
	FX_CHANNEL_COLOR,
	FX_CHANNEL_COLOR_RAND,
	FX_CHANNEL_ALPHA,
	FX_CHANNEL_ALPHA_RAND,
	FX_CHANNEL_SIZE,
	FX_CHANNEL_SIZE_RAND,
	FX_CHANNEL_SIZE2,
	FX_CHANNEL_SIZE2_RAND,
	FX_CHANNEL_LENGTH,
	FX_CHANNEL_LENGTH_RAND,
	FX_CHANNEL_ROTATION_DELTA,
	FX_CHANNEL_ROTATION_DELTA_RAND,
	FX_CHANNEL_VELOCITY_X,
	FX_CHANNEL_VELOCITY_Y,
	FX_CHANNEL_VELOCITY_Z,
	FX_CHANNEL_VELOCITY_X_RAND,
	FX_CHANNEL_VELOCITY_Y_RAND,
	FX_CHANNEL_VELOCITY_Z_RAND,
	FX_CHANNEL_VELOCITY2_X,
	FX_CHANNEL_VELOCITY2_Y,
	FX_CHANNEL_VELOCITY2_Z,
	FX_CHANNEL_VELOCITY2_X_RAND,
	FX_CHANNEL_VELOCITY2_Y_RAND,
	FX_CHANNEL_VELOCITY2_Z_RAND,
	FX_CHANNEL_COUNT
};

enum
{
	FX_DEPTH_HACK                = 0x1,
	FX_RELATIVE                  = 0x2,
	FX_SET_SHADER_TIME           = 0x4,
	FX_USE_MODEL                 = 0x10,
	FX_USE_PHYSICS               = 0x20,
	FX_USE_BBOX                  = 0x40,
	FX_USE_ALPHA                 = 0x80,
	FX_EMIT_FX                   = 0x100,
	FX_DEATH_FX                  = 0x200,
	FX_IMPACT_KILLS              = 0x400,
	FX_IMPACT_FX                 = 0x800,
	FX_BLOCKS_SIGHT              = 0x1000,
	FX_RAND_COLORS               = 0x2000,
	FX_RAND_ALPHA                = 0x4000,
	FX_RAND_SIZE                 = 0x8000,
	FX_RAND_SIZE2                = 0x10000,
	FX_RAND_LENGTH               = 0x20000,
	FX_RAND_ROTATION_DELTA       = 0x40000,
	FX_RAND_VELOCITY             = 0x80000,
	FX_RAND_VELOCITY2            = 0x100000,
	FX_ABSOLUTE_VEL              = 0x200000,
	FX_ABSOLUTE_VEL2             = 0x400000,
	FX_AFFECTED_BY_WIND          = 0x800000,
	FX_DISABLE_FAR_PLANE_CULLING = 0x2000000
};

enum
{
	FX_ORG_ON_SPHERE        = 0x1,
	FX_AXIS_FROM_SPHERE     = 0x2,
	FX_ORG_ON_CYLINDER      = 0x4,
	FX_ORG2_FROM_TRACE      = 0x8,
	FX_TRACE_IMPACT_FX      = 0x10,
	FX_ORG2_IS_OFFSET       = 0x20,
	FX_CHEAP_ORG_CALC       = 0x40,
	FX_CHEAP_ORG2_CALC      = 0x80,
	FX_RAND_ROT_AROUND_FWD  = 0x100,
	FX_EVEN_DISTRIBUTION    = 0x200,
	FX_FRUSTUM_CULL         = 0x400
};

enum
{
	FX_LINEAR    = 0x1,
	FX_RAND      = 0x2,
	FX_NONLINEAR = 0x4,
	FX_WAVE      = 0x8,
	FX_CLAMP     = 0xC
};

enum PrimType
{
	PRIM_NONE,
	PRIM_PARTICLE,
	PRIM_LINE,
	PRIM_TAIL,
	PRIM_CYLINDER,
	PRIM_EMITTER,
	PRIM_DECAL,
	PRIM_ORIENTED_PARTICLE,
	PRIM_FXRUNNER,
	PRIM_LIGHT,
	PRIM_CAMERA_SHAKE,
	PRIM_FLASH,
	PRIM_CLOUD
};

enum StartFrameMode
{
	START_FRAME_FIRST,
	START_FRAME_RANDOM,
	START_FRAME_FIXED,
	START_FRAME_MODE_COUNT
};

enum PlayRateMode
{
	PLAY_RATE_LIFETIME,
	PLAY_RATE_FIXED_FPS,
	PLAY_RATE_MODE_COUNT
};

enum LoopMode
{
	LOOP_NONE,
	LOOP_TIMES,
	LOOP_MODE_COUNT
};

union TMediaElement
{
	XModel *model;
	Material *material;
	FxEffectDef *effect;
	void *data;
};

struct TMediaList
{
	TMediaElement *elements;
	unsigned short size;
	unsigned short maxSize;
};

struct MediaHandles
{
	void Shutdown();
	void AddHandle( TMediaElement item );
	void AddEffect( FxEffectDef *fx );

	TMediaList mMediaList;
};

struct FxFlagEntry
{
	const char *flag;
	unsigned int masks[2];
};

struct PrimitiveTemplate
{
	void Init();
	void Shutdown();
	void InitBackCompatibleParameters( BackCompatibleParameters *bcp );
	void MigrateBackCompatibleParameters( BackCompatibleParameters *bcp );
	bool ParseFloat( const char *val, float *min, float *max );
	bool ParseVector( const char *val, vec3_t min, vec3_t max );
	bool ParseGroupFlags( const char *val, int *flags );
	bool ParseMin( const char *val );
	bool ParseMax( const char *val );
	bool ParseLife( const char *val );
	bool ParseSpawnRange( const char *val );
	bool ParseDelay( const char *val );
	bool ParseCount( const char *val );
	bool ParseElasticity( const char *val );
	bool ParseOrigin1( const char *val );
	bool ParseOrigin2( const char *val );
	bool ParseRadius( const char *val );
	bool ParseHeight( const char *val );
	bool ParseWindModifier( const char *val );
	bool ParseRotation( const char *val );
	float GetBackCompatibleRange( float min, float max );
	void CreateBackCompatibleRotationDeltaCurve( float initialValue, float keyScale, float lifetime, int channelId, float graphScale );
	void CreateBackCompatibleVelocityCurve( float initialValue, float keyScale, int channelId, float graphScale );
	void CreateBackCompatibleAccelerationCurve( float initialValue, float keyScale, float lifetime, int channelId, float graphScale );
	bool ParseRotationDelta( const char *val );
	bool ParseAngle( const char *val );
	bool ParseAngleDelta( const char *val );
	bool ParseVelocity( const char *val );
	bool ParseFlag( const char *flag, const FxFlagEntry *flagEntries, int flagEntryCount );
	bool ParseFlags( const char *line, const FxFlagEntry *flagEntries, int flagEntryCount );
	bool ParseAttributeFlags( const char *val );
	bool ParseSpawnFlags( const char *val );
	bool ParseAcceleration( const char *val );
	bool ParseGravity( const char *val );
	bool ParseDensity( const char *val );
	bool ParseVariance( const char *val );
	bool ParseSequenceStartFrameMode( const char *val );
	bool ParseSequenceFixedFrameValue( const char *val );
	bool ParseSequencePlayRateMode( const char *val );
	bool ParseSequenceFixedFpsValue( const char *val );
	bool ParseSequenceLoopMode( const char *val );
	bool ParseSequenceLoopTimes( const char *val );
	bool ParseSpawnFrustumCullRadius( const char *val );
	bool ParseBackCompatibleStart3( BackCompatibleParameters *bcp, const char *val, FxChannelId channelId );
	bool ParseBackCompatibleEnd3( BackCompatibleParameters *bcp, const char *val, FxChannelId channelId );
	bool ParseBackCompatibleStart( BackCompatibleParameters *bcp, const char *val, FxChannelId channelId );
	bool ParseBackCompatibleEnd( BackCompatibleParameters *bcp, const char *val, FxChannelId channelId );
	bool ParseBackCompatibleParm( BackCompatibleParameters *bcp, const char *val, FxChannelId channelId );
	bool ParseBackCompatibleFlags( BackCompatibleParameters *bcp, const char *val, FxChannelId channelId );
	bool ParseMaterials( GPValue *grp );
	bool ParseModels( GPValue *grp );
	bool ParseImpactFxStrings( GPValue *grp );
	bool ParseDeathFxStrings( GPValue *grp );
	bool ParseEmitterFxStrings( GPValue *grp );
	bool ParsePlayFxStrings( GPValue *grp );
	void ParseChannelRgbCurve( GPValue *pairs, FxChannelId channel );
	void ParseChannelCurve( GPValue *pairs, FxChannelId channel );
	void ParseChannelScale( const char *val, FxChannelId channel );
	bool ParseChannel( BackCompatibleParameters *bcp, GPGroup *grp, FxChannelId channelId );
	bool ParseMaterialImpact( const char *val );
	bool ParsePrimitiveInternal( BackCompatibleParameters *bcp, GPGroup *grp );
	bool ParsePrimitive( GPGroup *grp );

	char mName[32];
	char mMaterialImpact[32];
	PrimType mType;
	int mParentPrimIndex;
	FxRange mSpawnDelay;
	FxRange mSpawnCount;
	FxRange mLife;
	FxRange mSpawnRange;
	MediaHandles mMediaHandles;
	MediaHandles mImpactFxHandles;
	MediaHandles mDeathFxHandles;
	MediaHandles mEmitterFxHandles;
	MediaHandles mPlayFxHandles;
	int mAttributeFlags;
	int mSpawnFlags;
	int mGroupFlags;
	bool mNonUniformScale;
	bool useLength;
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
	FxChannel mFxChannels[FX_CHANNEL_COUNT];
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

const FxCurve *FxCurve_AllocAndCreateWithKeys( const float *keyArray, int dimensionCount, int keyCount );
void FxChannel_CreateDefault( FxChannel *createe, int dimensions, float value1, float value2 );
void FxChannel_CreateViaMigration( const FxChannelBackwardCompatible *source, int dimensions, float lifetime, bool forceUnitScale, FxChannel *target );
FxEffectDef *FX_RegisterEffect( const char *name );
XModel *FX_XModelPrecache( const char *name );
void FX_Print( const char *fmt, ... );
Material *FX_RegisterMaterial( const char *name );
XModel *FX_ModelRegister( const char *name );
void FxRange_SetRange( FxRange *range, float min, float max );
float __stdcall FxRange_GetValPct( const FxRange *range, float pct );

extern bool g_rendererExists;

void FX_Print( const char *fmt, ... );
Material *FX_RegisterMaterial( const char *name );
XModel *FX_ModelRegister( const char *name );
void FxRange_SetRange( FxRange *range, float min, float max );
float __stdcall FxRange_GetValPct( const FxRange *range, float pct );

extern const FxFlagEntry fxAttributeFlags[24];
static const FxFlagEntry fxAttributeFlags[24] =
{
	{ "depthHack", { FX_DEPTH_HACK, 0 } },
	{ "setShaderTime", { FX_SET_SHADER_TIME, 0 } },
	{ "useModel", { FX_USE_MODEL, 0 } },
	{ "useBBox", { FX_USE_BBOX, 0 } },
	{ "usePhysics", { FX_USE_PHYSICS, 0 } },
	{ "impactKills", { FX_IMPACT_KILLS, 0 } },
	{ "impactFx", { FX_IMPACT_FX, 0 } },
	{ "deathFx", { FX_DEATH_FX, 0 } },
	{ "useAlpha", { FX_USE_ALPHA, 0 } },
	{ "useRandomColors", { FX_RAND_COLORS, 0 } },
	{ "useRandomAlpha", { FX_RAND_ALPHA, 0 } },
	{ "useRandomSize", { FX_RAND_SIZE, 0 } },
	{ "useRandomSize2", { FX_RAND_SIZE2, 0 } },
	{ "useRandomLength", { FX_RAND_LENGTH, 0 } },
	{ "useRandomRotationDelta", { FX_RAND_ROTATION_DELTA, 0 } },
	{ "useRandomVelocity", { FX_RAND_VELOCITY, 0 } },
	{ "useRandomVelocity2", { FX_RAND_VELOCITY2, 0 } },
	{ "absoluteVel", { FX_ABSOLUTE_VEL, 0 } },
	{ "absoluteVel2", { FX_ABSOLUTE_VEL2, 0 } },
	{ "affectedByWind", { FX_AFFECTED_BY_WIND, 0 } },
	{ "emitFx", { FX_EMIT_FX, 0 } },
	{ "relative", { FX_RELATIVE, 0 } },
	{ "blocksSight", { FX_BLOCKS_SIGHT, 0 } },
	{ "disableFarPlaneCulling", { FX_DISABLE_FAR_PLANE_CULLING, 0 } }
};
extern const FxFlagEntry fxSpawnFlags[13];
static const FxFlagEntry fxSpawnFlags[13] =
{
	{ "org2fromTrace", { 0, FX_ORG2_FROM_TRACE } },
	{ "traceImpactFx", { 0, FX_TRACE_IMPACT_FX } },
	{ "org2isOffset", { 0, FX_ORG2_IS_OFFSET } },
	{ "cheapOrgCalc", { 0, FX_CHEAP_ORG_CALC } },
	{ "cheapOrg2Calc", { 0, FX_CHEAP_ORG2_CALC } },
	{ "orgOnSphere", { 0, FX_ORG_ON_SPHERE } },
	{ "orgOnCylinder", { 0, FX_ORG_ON_CYLINDER } },
	{ "axisFromSphere", { 0, FX_AXIS_FROM_SPHERE } },
	{ "randrotaroundfwd", { 0, FX_RAND_ROT_AROUND_FWD } },
	{ "evenDistribution", { 0, FX_EVEN_DISTRIBUTION } },
	{ "frustumCull", { 0, FX_FRUSTUM_CULL } },
	{ "absoluteVel", { FX_ABSOLUTE_VEL, 0 } },
	{ "absoluteAccel", { FX_ABSOLUTE_VEL2, 0 } }
};

// Renderer exports; only the slot used here (client re 0x60f080).
struct refexport_t
{
	void *unused0[4];
	Material *(*RegisterMaterial)( const char *name, int baseImageFlags, int imageTrack );
};
extern refexport_t re;

void PrimitiveTemplate::Init()
{
	FxRange_SetRange(&mLife, 1.0f, 1.0f);
	FxRange_SetRange(&mSpawnCount, 1.0f, 1.0f);
	FxRange_SetRange(&mRadius, 1.0f, 1.0f);
	FxRange_SetRange(&mHeight, 1.0f, 1.0f);
	FxChannel_CreateDefault(&mFxChannels[FX_CHANNEL_COLOR], 3, 1.0f, 0.0f);
	FxChannel_CreateDefault(&mFxChannels[FX_CHANNEL_COLOR_RAND], 3, 1.0f, 0.0f);
	FxChannel_CreateDefault(&mFxChannels[FX_CHANNEL_ALPHA], 1, 1.0f, 0.0f);
	FxChannel_CreateDefault(&mFxChannels[FX_CHANNEL_ALPHA_RAND], 1, 1.0f, 0.0f);
	FxChannel_CreateDefault(&mFxChannels[FX_CHANNEL_SIZE], 1, 1.0f, 0.0f);
	FxChannel_CreateDefault(&mFxChannels[FX_CHANNEL_SIZE_RAND], 1, 1.0f, 0.0f);
	FxChannel_CreateDefault(&mFxChannels[FX_CHANNEL_SIZE2], 1, 1.0f, 0.0f);
	FxChannel_CreateDefault(&mFxChannels[FX_CHANNEL_SIZE2_RAND], 1, 1.0f, 0.0f);
	FxChannel_CreateDefault(&mFxChannels[FX_CHANNEL_LENGTH], 1, 1.0f, 0.0f);
	FxChannel_CreateDefault(&mFxChannels[FX_CHANNEL_LENGTH_RAND], 1, 1.0f, 0.0f);
	FxChannel_CreateDefault(&mFxChannels[FX_CHANNEL_ROTATION_DELTA], 1, 0.0f, 0.0f);
	FxChannel_CreateDefault(&mFxChannels[FX_CHANNEL_ROTATION_DELTA_RAND], 1, 0.0f, 0.0f);
	FxChannel_CreateDefault(&mFxChannels[FX_CHANNEL_VELOCITY_X], 1, 0.0f, 0.0f);
	FxChannel_CreateDefault(&mFxChannels[FX_CHANNEL_VELOCITY_Y], 1, 0.0f, 0.0f);
	FxChannel_CreateDefault(&mFxChannels[FX_CHANNEL_VELOCITY_Z], 1, 0.0f, 0.0f);
	FxChannel_CreateDefault(&mFxChannels[FX_CHANNEL_VELOCITY_X_RAND], 1, 0.0f, 0.0f);
	FxChannel_CreateDefault(&mFxChannels[FX_CHANNEL_VELOCITY_Y_RAND], 1, 0.0f, 0.0f);
	FxChannel_CreateDefault(&mFxChannels[FX_CHANNEL_VELOCITY_Z_RAND], 1, 0.0f, 0.0f);
	FxChannel_CreateDefault(&mFxChannels[FX_CHANNEL_VELOCITY2_X], 1, 0.0f, 0.0f);
	FxChannel_CreateDefault(&mFxChannels[FX_CHANNEL_VELOCITY2_Y], 1, 0.0f, 0.0f);
	FxChannel_CreateDefault(&mFxChannels[FX_CHANNEL_VELOCITY2_Z], 1, 0.0f, 0.0f);
	FxChannel_CreateDefault(&mFxChannels[FX_CHANNEL_VELOCITY2_X_RAND], 1, 0.0f, 0.0f);
	FxChannel_CreateDefault(&mFxChannels[FX_CHANNEL_VELOCITY2_Y_RAND], 1, 0.0f, 0.0f);
	FxChannel_CreateDefault(&mFxChannels[FX_CHANNEL_VELOCITY2_Z_RAND], 1, 0.0f, 0.0f);
	FxRange_SetRange(&mTexCoordS, 1.0f, 1.0f);
	FxRange_SetRange(&mTexCoordT, 1.0f, 1.0f);
	FxRange_SetRange(&mVariance, 1.0f, 1.0f);
	FxRange_SetRange(&mDensity, 10.0f, 10.0f);
	mSequenceStartFrameMode = START_FRAME_FIRST;
	mSequenceFixedFrameValue = 1;
	mSequencePlayRateMode = PLAY_RATE_LIFETIME;
	mSequenceFixedFpsValue = 1.0f;
	mSequenceLoopMode = LOOP_NONE;
	mSequenceLoopTimes = 1;
	spawnFrustumCullRadius = 0.0f;
}

void FxRange_SetRange( FxRange *range, float min, float max )
{
	range->min = min;
	range->max = max;
}

// Original name unknown.
template<class T> inline void Swap( T &a, T &b )
{
	T temp;

	temp = a;
	a = b;
	b = temp;
}

void PrimitiveTemplate::Shutdown()
{
	mMediaHandles.Shutdown();
	mImpactFxHandles.Shutdown();
	mDeathFxHandles.Shutdown();
	mEmitterFxHandles.Shutdown();
	mPlayFxHandles.Shutdown();
}

// Original name unknown.
void PrimitiveTemplate::InitBackCompatibleParameters( BackCompatibleParameters *bcp )
{
	int channelId;
	int dim;

	memset(bcp, 0, sizeof(*bcp));
	for ( channelId = 0; channelId != FX_CHANNEL_COUNT; channelId++ )
	{
		for ( dim = 0; dim != 3; dim++ )
		{
			FxRange_SetRange(&bcp->fxChannels[channelId].start[dim], 1.0f, 1.0f);
			FxRange_SetRange(&bcp->fxChannels[channelId].end[dim], 1.0f, 1.0f);
			FxRange_SetRange(&bcp->fxChannels[channelId].parm, 1.0f, 1.0f);
		}
	}
}

// Original name unknown.
void PrimitiveTemplate::MigrateBackCompatibleParameters( BackCompatibleParameters *bcp )
{
	bool forceUnitScale;
	int dimensions;
	int channelId;

	for ( channelId = 0; channelId != FX_CHANNEL_COUNT; channelId++ )
	{
		if ( bcp->fxChannels[channelId].used )
		{
			forceUnitScale = false;
			dimensions = 1;
			if ( channelId == FX_CHANNEL_ALPHA )
				forceUnitScale = true;
			if ( channelId == FX_CHANNEL_COLOR )
				dimensions = 3;
			FxChannel_CreateViaMigration(&bcp->fxChannels[channelId], dimensions, FxRange_GetValPct(&mLife, 0.5f), forceUnitScale, &mFxChannels[channelId]);
		}
	}
}

bool PrimitiveTemplate::ParseFloat( const char *val, float *min, float *max )
{
	int v;

	if ( !min || !max )
		return false;
	v = sscanf(val, "%f %f", min, max);
	if ( !v )
		return false;
	if ( v == 1 )
		*max = *min;
	return true;
}

bool PrimitiveTemplate::ParseVector( const char *val, vec3_t min, vec3_t max )
{
	int v;

	if ( !min || !max )
		return false;
	v = sscanf(val, "%f %f %f   %f %f %f", &min[0], &min[1], &min[2], &max[0], &max[1], &max[2]);
	if ( v < 3 || v == 4 || v == 5 )
		return false;
	if ( v == 3 )
		VectorCopy(min, max);
	return true;
}

bool PrimitiveTemplate::ParseGroupFlags( const char *val, int *flags )
{
	char flag[][32] = { "\0", "\0", "\0", "0" };
	bool ok;
	int v;
	int i;

	ok = true;
	v = sscanf(val, "%s %s %s %s", flag[0], flag[1], flag[2], flag[3]);
	*flags = 0;
	for ( i = 0; i < 4; i++ )
	{
		if ( i + 1 > v )
			return true;
		if ( !stricmp(flag[i], "linear") )
			*flags |= FX_LINEAR;
		else if ( !stricmp(flag[i], "nonlinear") )
			*flags |= FX_NONLINEAR;
		else if ( !stricmp(flag[i], "wave") )
			*flags |= FX_WAVE;
		else if ( !stricmp(flag[i], "random") )
			*flags |= FX_RAND;
		else if ( !stricmp(flag[i], "clamp") )
			*flags |= FX_CLAMP;
		else
			ok = false;
	}
	return ok;
}

bool PrimitiveTemplate::ParseMin( const char *val )
{
	vec3_t min;

	if ( ParseVector(val, min, min) == true )
	{
		VectorCopy(min, mMin);
		mAttributeFlags |= FX_USE_BBOX | FX_USE_PHYSICS;
		return true;
	}
	return false;
}

bool PrimitiveTemplate::ParseMax( const char *val )
{
	vec3_t max;

	if ( ParseVector(val, max, max) == true )
	{
		VectorCopy(max, mMax);
		mAttributeFlags |= FX_USE_BBOX | FX_USE_PHYSICS;
		return true;
	}
	return false;
}

bool PrimitiveTemplate::ParseLife( const char *val )
{
	float min;
	float max;

	if ( ParseFloat(val, &min, &max) )
	{
		FxRange_SetRange(&mLife, min, max);
		return true;
	}
	return false;
}

bool PrimitiveTemplate::ParseSpawnRange( const char *val )
{
	float min;
	float max;

	if ( ParseFloat(val, &min, &max) )
	{
		if ( min > max )
			return false;
		FxRange_SetRange(&mSpawnRange, min, max);
		return true;
	}
	return false;
}

bool PrimitiveTemplate::ParseDelay( const char *val )
{
	float min;
	float max;

	if ( ParseFloat(val, &min, &max) )
	{
		FxRange_SetRange(&mSpawnDelay, min, max);
		return true;
	}
	return false;
}

bool PrimitiveTemplate::ParseCount( const char *val )
{
	float min;
	float max;

	if ( ParseFloat(val, &min, &max) )
	{
		FxRange_SetRange(&mSpawnCount, min, max);
		return true;
	}
	return false;
}

bool PrimitiveTemplate::ParseElasticity( const char *val )
{
	float min;
	float max;

	if ( ParseFloat(val, &min, &max) )
	{
		if ( min > max )
			return false;
		if ( min < 0.0f )
			return false;
		if ( min > 1.0f )
			return false;
		if ( max < 0.0f )
			return false;
		if ( max > 1.0f )
			return false;
		FxRange_SetRange(&mElasticity, min, max);
		mAttributeFlags |= FX_USE_PHYSICS;
		return true;
	}
	return false;
}

bool PrimitiveTemplate::ParseOrigin1( const char *val )
{
	vec3_t min;
	vec3_t max;

	if ( ParseVector(val, min, max) == true )
	{
		FxRange_SetRange(&mOrigin1X, min[0], max[0]);
		FxRange_SetRange(&mOrigin1Y, min[1], max[1]);
		FxRange_SetRange(&mOrigin1Z, min[2], max[2]);
		return true;
	}
	return false;
}

bool PrimitiveTemplate::ParseOrigin2( const char *val )
{
	vec3_t min;
	vec3_t max;

	if ( ParseVector(val, min, max) == true )
	{
		FxRange_SetRange(&mOrigin2X, min[0], max[0]);
		FxRange_SetRange(&mOrigin2Y, min[1], max[1]);
		FxRange_SetRange(&mOrigin2Z, min[2], max[2]);
		return true;
	}
	return false;
}

bool PrimitiveTemplate::ParseRadius( const char *val )
{
	float min;
	float max;

	if ( ParseFloat(val, &min, &max) )
	{
		FxRange_SetRange(&mRadius, min, max);
		return true;
	}
	return false;
}

bool PrimitiveTemplate::ParseHeight( const char *val )
{
	float min;
	float max;

	if ( ParseFloat(val, &min, &max) )
	{
		FxRange_SetRange(&mHeight, min, max);
		return true;
	}
	return false;
}

bool PrimitiveTemplate::ParseWindModifier( const char *val )
{
	float min;
	float max;

	if ( ParseFloat(val, &min, &max) )
	{
		FxRange_SetRange(&mWindModifier, min, max);
		return true;
	}
	return false;
}

bool PrimitiveTemplate::ParseRotation( const char *val )
{
	float min;
	float max;

	if ( ParseFloat(val, &min, &max) )
	{
		FxRange_SetRange(&mRotation, min, max);
		return true;
	}
	return false;
}

// Original name unknown.
float PrimitiveTemplate::GetBackCompatibleRange( float min, float max )
{
	float range;

	if ( max > fabs(min) )
		range = max;
	else
		range = fabs(min);
	return range;
}

void PrimitiveTemplate::CreateBackCompatibleRotationDeltaCurve( float initialValue, float keyScale, float lifetime, int channelId, float graphScale )
{
	const int keyCount = 20;
	float keys[keyCount][2];
	int keyIndex;
	float frac;
	float step;

	step = lifetime / 20.0f;
	for ( keyIndex = 0; keyIndex < keyCount; keyIndex++ )
	{
		keys[keyIndex][0] = keyIndex / 19.0f;
		if ( !keyIndex )
		{
			if ( keyScale == 0.0f )
				keys[keyIndex][1] = initialValue;
			else
				keys[keyIndex][1] = initialValue / keyScale;
		}
		else
		{
			frac = step * 0.00065f;
			frac = I_fclamp(frac, 0.0f, 1.0f);
			keys[keyIndex][1] = keys[keyIndex - 1][1] * (1.0f - frac);
		}
	}
	mFxChannels[channelId].curve = FxCurve_AllocAndCreateWithKeys(keys[0], 1, keyCount);
	FxRange_SetRange(&mFxChannels[channelId].scaleRange, graphScale, graphScale);
}

// Original name unknown.
void PrimitiveTemplate::CreateBackCompatibleVelocityCurve( float initialValue, float keyScale, int channelId, float graphScale )
{
	const int keyCount = 2;
	float keys[keyCount][2];

	keys[0][0] = 0.0f;
	if ( keyScale == 0.0f )
		keys[0][1] = 0.0f;
	else
		keys[0][1] = initialValue / keyScale;
	keys[1][0] = 1.0f;
	keys[1][1] = keys[0][1];
	mFxChannels[channelId].curve = FxCurve_AllocAndCreateWithKeys(keys[0], 1, keyCount);
	FxRange_SetRange(&mFxChannels[channelId].scaleRange, graphScale, graphScale);
}

// Original name unknown.
void PrimitiveTemplate::CreateBackCompatibleAccelerationCurve( float initialValue, float keyScale, float lifetime, int channelId, float graphScale )
{
	const int keyCount = 2;
	float keys[keyCount][2];

	keys[0][0] = 0.0f;
	keys[0][1] = 0.0f;
	keys[1][0] = 1.0f;
	keys[1][1] = initialValue * lifetime * 0.001f;
	if ( keyScale == 0.0f )
		keys[1][1] = 0.0f;
	else
		keys[1][1] = keys[1][1] / keyScale;
	mFxChannels[channelId].curve = FxCurve_AllocAndCreateWithKeys(keys[0], 1, keyCount);
	FxRange_SetRange(&mFxChannels[channelId].scaleRange, graphScale, graphScale);
}

bool PrimitiveTemplate::ParseRotationDelta( const char *val )
{
	float min;
	float max;
	float range;

	if ( ParseFloat(val, &min, &max) )
	{
		if ( min > max )
			Swap(min, max);
		range = GetBackCompatibleRange(min, max);
		range = range * 2.0f;
		CreateBackCompatibleRotationDeltaCurve(min, range, mLife.min, FX_CHANNEL_ROTATION_DELTA, range);
		if ( min != max )
		{
			mAttributeFlags |= FX_RAND_ROTATION_DELTA;
			CreateBackCompatibleRotationDeltaCurve(max, range, mLife.max, FX_CHANNEL_ROTATION_DELTA_RAND, 1.0f);
		}
		return true;
	}
	return false;
}

bool PrimitiveTemplate::ParseAngle( const char *val )
{
	vec3_t min;
	vec3_t max;

	if ( ParseVector(val, min, max) == true )
	{
		FxRange_SetRange(&mAngle1, min[0], max[0]);
		FxRange_SetRange(&mAngle2, min[1], max[1]);
		FxRange_SetRange(&mAngle3, min[2], max[2]);
		return true;
	}
	return false;
}

bool PrimitiveTemplate::ParseAngleDelta( const char *val )
{
	vec3_t min;
	vec3_t max;

	if ( ParseVector(val, min, max) == true )
	{
		FxRange_SetRange(&mAngle1Delta, min[0], max[0]);
		FxRange_SetRange(&mAngle2Delta, min[1], max[1]);
		FxRange_SetRange(&mAngle3Delta, min[2], max[2]);
		return true;
	}
	return false;
}

bool PrimitiveTemplate::ParseVelocity( const char *val )
{
	vec3_t min;
	vec3_t max;
	vec3_t range;
	float maxRange;

	if ( ParseVector(val, min, max) == true )
	{
		if ( min[0] > max[0] )
			Swap(min[0], max[0]);
		if ( min[1] > max[1] )
			Swap(min[1], max[1]);
		if ( min[2] > max[2] )
			Swap(min[2], max[2]);
		range[0] = GetBackCompatibleRange(min[0], max[0]);
		range[1] = GetBackCompatibleRange(min[1], max[1]);
		range[2] = GetBackCompatibleRange(min[2], max[2]);
		range[0] = range[0] * 2.0f;
		range[1] = range[1] * 2.0f;
		range[2] = range[2] * 2.0f;
		maxRange = I_fmax(I_fmax(range[0], range[1]), range[2]);
		CreateBackCompatibleVelocityCurve(min[0], maxRange, FX_CHANNEL_VELOCITY_X, maxRange);
		CreateBackCompatibleVelocityCurve(min[1], maxRange, FX_CHANNEL_VELOCITY_Y, maxRange);
		CreateBackCompatibleVelocityCurve(min[2], maxRange, FX_CHANNEL_VELOCITY_Z, maxRange);
		if ( !VectorCompare(min, max) )
		{
			mAttributeFlags |= FX_RAND_VELOCITY;
			CreateBackCompatibleVelocityCurve(max[0], maxRange, FX_CHANNEL_VELOCITY_X_RAND, 1.0f);
			CreateBackCompatibleVelocityCurve(max[1], maxRange, FX_CHANNEL_VELOCITY_Y_RAND, 1.0f);
			CreateBackCompatibleVelocityCurve(max[2], maxRange, FX_CHANNEL_VELOCITY_Z_RAND, 1.0f);
		}
		return true;
	}
	return false;
}

// Original name unknown.
bool PrimitiveTemplate::ParseFlag( const char *flag, const FxFlagEntry *flagEntries, int flagEntryCount )
{
	int i;
	const FxFlagEntry *entry;

	for ( i = 0; i < flagEntryCount; i++ )
	{
		entry = &flagEntries[i];
		if ( !stricmp(entry->flag, flag) )
		{
			mAttributeFlags |= entry->masks[0];
			mSpawnFlags |= entry->masks[1];
			return true;
		}
	}
	return false;
}

bool PrimitiveTemplate::ParseFlags( const char *line, const FxFlagEntry *flagEntries, int flagEntryCount )
{
	char *flag;
	const char *cursor;
	int len;
	int pos;
	int flagLen;
	bool ok;

	len = strlen(line);
	if ( !len )
		return false;
	flag = (char *)Hunk_AllocateTempMemoryInternal(len);
	cursor = line;
	pos = 0;
	ok = true;
	while ( pos < len )
	{
		if ( sscanf(cursor, "%s", flag) != 1 )
		{
			ok = false;
			break;
		}
		flagLen = strlen(flag);
		if ( !ParseFlag(flag, flagEntries, flagEntryCount) )
		{
			ok = false;
			break;
		}
		pos += flagLen + 1;
		cursor = &line[pos];
	}
	Hunk_FreeTempMemory(flag);
	return ok;
}

// Original name unknown.
bool PrimitiveTemplate::ParseAttributeFlags( const char *val )
{
	return ParseFlags(val, fxAttributeFlags, ARRAY_COUNT(fxAttributeFlags));
}

// Original name unknown.
bool PrimitiveTemplate::ParseSpawnFlags( const char *val )
{
	return ParseFlags(val, fxSpawnFlags, ARRAY_COUNT(fxSpawnFlags));
}

bool PrimitiveTemplate::ParseAcceleration( const char *val )
{
	vec3_t min;
	vec3_t max;
	vec3_t range;
	float lifetime;
	float maxRange;

	if ( ParseVector(val, min, max) == true )
	{
		if ( min[0] > max[0] )
			Swap(min[0], max[0]);
		if ( min[1] > max[1] )
			Swap(min[1], max[1]);
		if ( min[2] > max[2] )
			Swap(min[2], max[2]);
		lifetime = mLife.max;
		range[0] = GetBackCompatibleRange(min[0], max[0]);
		range[1] = GetBackCompatibleRange(min[1], max[1]);
		range[2] = GetBackCompatibleRange(min[2], max[2]);
		range[0] = range[0] * 2.0f;
		range[1] = range[1] * 2.0f;
		range[2] = range[2] * 2.0f;
		maxRange = I_fmax(I_fmax(range[0], range[1]), range[2]);
		maxRange = lifetime * 0.001f * maxRange;
		CreateBackCompatibleAccelerationCurve(min[0], maxRange, lifetime, FX_CHANNEL_VELOCITY2_X, maxRange);
		CreateBackCompatibleAccelerationCurve(min[1], maxRange, lifetime, FX_CHANNEL_VELOCITY2_Y, maxRange);
		CreateBackCompatibleAccelerationCurve(min[2], maxRange, lifetime, FX_CHANNEL_VELOCITY2_Z, maxRange);
		if ( !VectorCompare(min, max) )
		{
			mAttributeFlags |= FX_RAND_VELOCITY2;
			CreateBackCompatibleAccelerationCurve(max[0], maxRange, lifetime, FX_CHANNEL_VELOCITY2_X_RAND, 1.0f);
			CreateBackCompatibleAccelerationCurve(max[1], maxRange, lifetime, FX_CHANNEL_VELOCITY2_Y_RAND, 1.0f);
			CreateBackCompatibleAccelerationCurve(max[2], maxRange, lifetime, FX_CHANNEL_VELOCITY2_Z_RAND, 1.0f);
		}
		return true;
	}
	return false;
}

bool PrimitiveTemplate::ParseGravity( const char *val )
{
	float min;
	float max;

	if ( ParseFloat(val, &min, &max) )
	{
		FxRange_SetRange(&mGravity, min, max);
		return true;
	}
	return false;
}

bool PrimitiveTemplate::ParseDensity( const char *val )
{
	float min;
	float max;

	if ( ParseFloat(val, &min, &max) )
	{
		FxRange_SetRange(&mDensity, min, max);
		return true;
	}
	return false;
}

bool PrimitiveTemplate::ParseVariance( const char *val )
{
	float min;
	float max;

	if ( ParseFloat(val, &min, &max) )
	{
		FxRange_SetRange(&mVariance, min, max);
		return true;
	}
	return false;
}

bool PrimitiveTemplate::ParseSequenceStartFrameMode( const char *val )
{
	int mode;

	mode = atoi(val);
	if ( mode < START_FRAME_FIRST || mode >= START_FRAME_MODE_COUNT )
		return false;
	mSequenceStartFrameMode = (StartFrameMode)mode;
	return true;
}

bool PrimitiveTemplate::ParseSequenceFixedFrameValue( const char *val )
{
	mSequenceFixedFrameValue = atoi(val);
	if ( mSequenceFixedFrameValue < 1 )
		return false;
	return true;
}

bool PrimitiveTemplate::ParseSequencePlayRateMode( const char *val )
{
	int mode;

	mode = atoi(val);
	if ( mode < PLAY_RATE_LIFETIME || mode >= PLAY_RATE_MODE_COUNT )
		return false;
	mSequencePlayRateMode = (PlayRateMode)mode;
	return true;
}

bool PrimitiveTemplate::ParseSequenceFixedFpsValue( const char *val )
{
	mSequenceFixedFpsValue = atof(val);
	if ( mSequenceFixedFpsValue < 0.0f )
		return false;
	return true;
}

bool PrimitiveTemplate::ParseSequenceLoopMode( const char *val )
{
	int mode;

	mode = atoi(val);
	if ( mode < LOOP_NONE || mode >= LOOP_MODE_COUNT )
		return false;
	mSequenceLoopMode = (LoopMode)mode;
	return true;
}

bool PrimitiveTemplate::ParseSequenceLoopTimes( const char *val )
{
	mSequenceLoopTimes = atoi(val);
	if ( mSequenceLoopTimes < 0 )
		return false;
	return true;
}

bool PrimitiveTemplate::ParseSpawnFrustumCullRadius( const char *val )
{
	float radius;

	radius = atof(val);
	if ( radius < 0.0f )
		return false;
	spawnFrustumCullRadius = radius;
	return true;
}

// Original name unknown.
bool PrimitiveTemplate::ParseBackCompatibleStart3( BackCompatibleParameters *bcp, const char *val, FxChannelId channelId )
{
	vec3_t min;
	vec3_t max;
	int dim;

	if ( ParseVector(val, min, max) )
	{
		for ( dim = 0; dim != 3; dim++ )
			FxRange_SetRange(&bcp->fxChannels[channelId].start[dim], min[dim], max[dim]);
		bcp->fxChannels[channelId].used = true;
		return true;
	}
	return false;
}

// Original name unknown.
bool PrimitiveTemplate::ParseBackCompatibleEnd3( BackCompatibleParameters *bcp, const char *val, FxChannelId channelId )
{
	vec3_t min;
	vec3_t max;
	int dim;

	if ( ParseVector(val, min, max) )
	{
		for ( dim = 0; dim != 3; dim++ )
			FxRange_SetRange(&bcp->fxChannels[channelId].end[dim], min[dim], max[dim]);
		bcp->fxChannels[channelId].used = true;
		return true;
	}
	return false;
}

// Original name unknown.
bool PrimitiveTemplate::ParseBackCompatibleStart( BackCompatibleParameters *bcp, const char *val, FxChannelId channelId )
{
	float min;
	float max;

	if ( ParseFloat(val, &min, &max) )
	{
		if ( min > max )
			Swap(min, max);
		FxRange_SetRange(&bcp->fxChannels[channelId].start[0], min, max);
		bcp->fxChannels[channelId].used = true;
		return true;
	}
	return false;
}

// Original name unknown.
bool PrimitiveTemplate::ParseBackCompatibleEnd( BackCompatibleParameters *bcp, const char *val, FxChannelId channelId )
{
	float min;
	float max;

	if ( ParseFloat(val, &min, &max) )
	{
		if ( min > max )
			Swap(min, max);
		FxRange_SetRange(&bcp->fxChannels[channelId].end[0], min, max);
		bcp->fxChannels[channelId].used = true;
		return true;
	}
	return false;
}

// Original name unknown.
bool PrimitiveTemplate::ParseBackCompatibleParm( BackCompatibleParameters *bcp, const char *val, FxChannelId channelId )
{
	float min;
	float max;

	if ( ParseFloat(val, &min, &max) )
	{
		FxRange_SetRange(&bcp->fxChannels[channelId].parm, min, max);
		bcp->fxChannels[channelId].used = true;
		return true;
	}
	return false;
}

// Original name unknown.
bool PrimitiveTemplate::ParseBackCompatibleFlags( BackCompatibleParameters *bcp, const char *val, FxChannelId channelId )
{
	int flags;

	if ( ParseGroupFlags(val, &flags) )
	{
		bcp->fxChannels[channelId].flags |= flags;
		bcp->fxChannels[channelId].used = true;
		return true;
	}
	return false;
}

bool PrimitiveTemplate::ParseMaterials( GPValue *grp )
{
	const char *val;
	TMediaElement item;
	GPObject *list;

	if ( !g_rendererExists )
		return true;
	if ( grp->IsList() )
	{
		for ( list = grp->GetList(); list; list = list->GetNext() )
		{
			val = list->GetName();
			item.material = FX_RegisterMaterial(val);
			mMediaHandles.AddHandle(item);
		}
	}
	else
	{
		val = grp->GetTopValue();
		if ( val )
		{
			item.material = FX_RegisterMaterial(val);
			mMediaHandles.AddHandle(item);
		}
		else
		{
			FX_Print("PrimitiveTemplate::ParseMaterials called with an empty list!\n");
			return false;
		}
	}
	return true;
}

bool PrimitiveTemplate::ParseModels( GPValue *grp )
{
	const char *val;
	TMediaElement item;
	GPObject *list;

	if ( grp->IsList() )
	{
		for ( list = grp->GetList(); list; list = list->GetNext() )
		{
			val = list->GetName();
			item.model = FX_ModelRegister(val);
			if ( !item.model )
			{
				FX_Print("PrimitiveTemplate::ParseModels, could not register model '%s'\n", val);
				return false;
			}
			mMediaHandles.AddHandle(item);
		}
	}
	else
	{
		val = grp->GetTopValue();
		if ( !val )
		{
			FX_Print("PrimitiveTemplate::ParseModels called with an empty list!\n");
			return false;
		}
		item.model = FX_ModelRegister(val);
		if ( !item.model )
		{
			FX_Print("PrimitiveTemplate::ParseModels, could not register model '%s'\n", val);
			return false;
		}
		mMediaHandles.AddHandle(item);
	}
	return true;
}

bool PrimitiveTemplate::ParseImpactFxStrings( GPValue *grp )
{
	const char *val;
	FxEffectDef *fx;
	GPObject *list;

	if ( grp->IsList() )
	{
		for ( list = grp->GetList(); list; list = list->GetNext() )
		{
			val = list->GetName();
			fx = FX_RegisterEffect(val);
			if ( fx )
			{
				mImpactFxHandles.AddEffect(fx);
			}
			else
			{
				FX_Print("FxTemplate: Impact effect file not found.\n");
				return false;
			}
		}
	}
	else
	{
		val = grp->GetTopValue();
		if ( val )
		{
			fx = FX_RegisterEffect(val);
			if ( fx )
			{
				mImpactFxHandles.AddEffect(fx);
			}
			else
			{
				FX_Print("FxTemplate: Impact effect file not found.\n");
				return false;
			}
		}
		else
		{
			FX_Print("PrimitiveTemplate::ParseImpactFxStrings called with an empty list!\n");
			return false;
		}
	}
	mAttributeFlags |= FX_IMPACT_FX | FX_USE_PHYSICS;
	return true;
}

bool PrimitiveTemplate::ParseDeathFxStrings( GPValue *grp )
{
	const char *val;
	FxEffectDef *fx;
	GPObject *list;

	if ( grp->IsList() )
	{
		for ( list = grp->GetList(); list; list = list->GetNext() )
		{
			val = list->GetName();
			fx = FX_RegisterEffect(val);
			if ( fx )
			{
				mDeathFxHandles.AddEffect(fx);
			}
			else
			{
				FX_Print("FxTemplate: Death effect file not found.\n");
				return false;
			}
		}
	}
	else
	{
		val = grp->GetTopValue();
		if ( val )
		{
			fx = FX_RegisterEffect(val);
			if ( fx )
			{
				mDeathFxHandles.AddEffect(fx);
			}
			else
			{
				FX_Print("FxTemplate: Death effect file not found.\n");
				return false;
			}
		}
		else
		{
			FX_Print("PrimitiveTemplate::ParseDeathFxStrings called with an empty list!\n");
			return false;
		}
	}
	mAttributeFlags |= FX_DEATH_FX;
	return true;
}

bool PrimitiveTemplate::ParseEmitterFxStrings( GPValue *grp )
{
	const char *val;
	FxEffectDef *fx;
	GPObject *list;

	if ( grp->IsList() )
	{
		for ( list = grp->GetList(); list; list = list->GetNext() )
		{
			val = list->GetName();
			fx = FX_RegisterEffect(val);
			if ( fx )
			{
				mEmitterFxHandles.AddEffect(fx);
			}
			else
			{
				FX_Print("FxTemplate: Emitter effect file not found.\n");
				return false;
			}
		}
	}
	else
	{
		val = grp->GetTopValue();
		if ( val )
		{
			fx = FX_RegisterEffect(val);
			if ( fx )
			{
				mEmitterFxHandles.AddEffect(fx);
			}
			else
			{
				FX_Print("FxTemplate: Emitter effect file not found.\n");
				return false;
			}
		}
		else
		{
			FX_Print("PrimitiveTemplate::ParseEmitterFxStrings called with an empty list!\n");
			return false;
		}
	}
	mAttributeFlags |= FX_EMIT_FX;
	return true;
}

bool PrimitiveTemplate::ParsePlayFxStrings( GPValue *grp )
{
	const char *val;
	FxEffectDef *fx;
	GPObject *list;

	if ( grp->IsList() )
	{
		for ( list = grp->GetList(); list; list = list->GetNext() )
		{
			val = list->GetName();
			fx = FX_RegisterEffect(val);
			if ( fx )
			{
				mPlayFxHandles.AddEffect(fx);
			}
			else
			{
				FX_Print("FxTemplate: Effect file not found.\n");
				return false;
			}
		}
	}
	else
	{
		val = grp->GetTopValue();
		if ( val )
		{
			fx = FX_RegisterEffect(val);
			if ( fx )
			{
				mPlayFxHandles.AddEffect(fx);
			}
			else
			{
				FX_Print("FxTemplate: Effect file not found.\n");
				return false;
			}
		}
		else
		{
			FX_Print("PrimitiveTemplate::ParsePlayFxStrings called with an empty list!\n");
			return false;
		}
	}
	return true;
}

void PrimitiveTemplate::ParseChannelRgbCurve( GPValue *pairs, FxChannelId channel )
{
	GPObject *list;
	GPObject *item;
	int keyCount;
	float (*keys)[4];

	list = pairs->GetList();
	keyCount = 0;
	for ( item = list; item; item = item->GetNext() )
		keyCount++;
	keys = (float (*)[4])Hunk_AllocateTempMemoryInternal(keyCount * sizeof(keys[0]));
	keyCount = 0;
	for ( item = list; item; item = item->GetNext() )
	{
		sscanf(item->GetName(), "%f %f %f %f", &keys[keyCount][0], &keys[keyCount][1], &keys[keyCount][2], &keys[keyCount][3]);
		keyCount++;
	}
	mFxChannels[channel].curve = FxCurve_AllocAndCreateWithKeys(keys[0], 3, keyCount);
	Hunk_FreeTempMemory(keys);
}

void PrimitiveTemplate::ParseChannelCurve( GPValue *pairs, FxChannelId channel )
{
	GPObject *list;
	GPObject *item;
	int keyCount;
	float (*keys)[2];

	list = pairs->GetList();
	keyCount = 0;
	for ( item = list; item; item = item->GetNext() )
		keyCount++;
	keys = (float (*)[2])Hunk_AllocateTempMemoryInternal(keyCount * sizeof(keys[0]));
	keyCount = 0;
	for ( item = list; item; item = item->GetNext() )
	{
		sscanf(item->GetName(), "%f %f", &keys[keyCount][0], &keys[keyCount][1]);
		keyCount++;
	}
	mFxChannels[channel].curve = FxCurve_AllocAndCreateWithKeys(keys[0], 1, keyCount);
	Hunk_FreeTempMemory(keys);
}

// Original name unknown.
void PrimitiveTemplate::ParseChannelScale( const char *val, FxChannelId channel )
{
	float min;
	float max;

	if ( ParseFloat(val, &min, &max) )
		FxRange_SetRange(&mFxChannels[channel].scaleRange, min, max);
}

bool PrimitiveTemplate::ParseChannel( BackCompatibleParameters *bcp, GPGroup *grp, FxChannelId channelId )
{
	GPValue *pairs;
	const char *key;
	const char *val;

	for ( pairs = grp->GetPairs(); pairs; pairs = pairs->GetNext() )
	{
		key = pairs->GetName();
		val = pairs->GetTopValue();
		if ( !stricmp(key, "curve") )
		{
			if ( channelId == FX_CHANNEL_COLOR || channelId == FX_CHANNEL_COLOR_RAND )
				ParseChannelRgbCurve(pairs, channelId);
			else
				ParseChannelCurve(pairs, channelId);
		}
		else if ( !stricmp(key, "scale") )
		{
			ParseChannelScale(val, channelId);
		}
		else if ( !stricmp(key, "start") )
		{
			if ( channelId == FX_CHANNEL_COLOR || channelId == FX_CHANNEL_COLOR_RAND )
				ParseBackCompatibleStart3(bcp, val, channelId);
			else
				ParseBackCompatibleStart(bcp, val, channelId);
		}
		else if ( !stricmp(key, "end") )
		{
			if ( channelId == FX_CHANNEL_COLOR || channelId == FX_CHANNEL_COLOR_RAND )
				ParseBackCompatibleEnd3(bcp, val, channelId);
			else
				ParseBackCompatibleEnd(bcp, val, channelId);
		}
		else if ( !stricmp(key, "parm") || !stricmp(key, "parms") )
		{
			ParseBackCompatibleParm(bcp, val, channelId);
		}
		else if ( !stricmp(key, "flags") || !stricmp(key, "flag") )
		{
			ParseBackCompatibleFlags(bcp, val, channelId);
		}
		else
		{
			FX_Print("Unknown key parsing a channel: %s\n", key);
		}
	}
	return true;
}

// Original name unknown.
bool PrimitiveTemplate::ParseMaterialImpact( const char *val )
{
	I_strncpyz(mMaterialImpact, val, sizeof(mMaterialImpact));
	return true;
}

bool PrimitiveTemplate::ParsePrimitiveInternal( BackCompatibleParameters *bcp, GPGroup *grp )
{
	GPGroup *subGrp;
	GPValue *pairs;
	const char *key;
	const char *val;

	pairs = grp->GetPairs();
	key = NULL;
	InitBackCompatibleParameters(bcp);
	while ( pairs )
	{
		key = pairs->GetName();
		val = pairs->GetTopValue();
		if ( !stricmp(key, "count") )
		{
			if ( !ParseCount(val) )
				break;
		}
		else if ( !stricmp(key, "shaders") || !stricmp(key, "shader") )
		{
			if ( !ParseMaterials(pairs) )
				break;
		}
		else if ( !stricmp(key, "models") || !stricmp(key, "model") )
		{
			if ( !ParseModels(pairs) )
				break;
		}
		else if ( !stricmp(key, "impactfx") )
		{
			if ( !ParseImpactFxStrings(pairs) )
				break;
		}
		else if ( !stricmp(key, "deathfx") )
		{
			if ( !ParseDeathFxStrings(pairs) )
				break;
		}
		else if ( !stricmp(key, "emitfx") )
		{
			if ( !ParseEmitterFxStrings(pairs) )
				break;
		}
		else if ( !stricmp(key, "playfx") )
		{
			if ( !ParsePlayFxStrings(pairs) )
				break;
		}
		else if ( !stricmp(key, "life") )
		{
			if ( !ParseLife(val) )
				break;
		}
		else if ( !stricmp(key, "cullrange") )
		{
			mSpawnRange.max = atof(val);
		}
		else if ( !stricmp(key, "spawnRange") )
		{
			if ( !ParseSpawnRange(val) )
				break;
		}
		else if ( !stricmp(key, "delay") )
		{
			if ( !ParseDelay(val) )
				break;
		}
		else if ( !stricmp(key, "bounce") || !stricmp(key, "intensity") )
		{
			if ( !ParseElasticity(val) )
				break;
		}
		else if ( !stricmp(key, "min") )
		{
			if ( !ParseMin(val) )
				break;
		}
		else if ( !stricmp(key, "max") )
		{
			if ( !ParseMax(val) )
				break;
		}
		else if ( !stricmp(key, "angle") || !stricmp(key, "angles") )
		{
			if ( !ParseAngle(val) )
				break;
		}
		else if ( !stricmp(key, "angleDelta") )
		{
			if ( !ParseAngleDelta(val) )
				break;
		}
		else if ( !stricmp(key, "velocity") || !stricmp(key, "vel") )
		{
			if ( !ParseVelocity(val) )
				break;
		}
		else if ( !stricmp(key, "acceleration") || !stricmp(key, "accel") )
		{
			if ( !ParseAcceleration(val) )
				break;
		}
		else if ( !stricmp(key, "gravity") )
		{
			if ( !ParseGravity(val) )
				break;
		}
		else if ( !stricmp(key, "density") )
		{
			if ( !ParseDensity(val) )
				break;
		}
		else if ( !stricmp(key, "variance") )
		{
			if ( !ParseVariance(val) )
				break;
		}
		else if ( !stricmp(key, "origin") )
		{
			if ( !ParseOrigin1(val) )
				break;
		}
		else if ( !stricmp(key, "origin2") )
		{
			if ( !ParseOrigin2(val) )
				break;
		}
		else if ( !stricmp(key, "radius") )
		{
			if ( !ParseRadius(val) )
				break;
		}
		else if ( !stricmp(key, "height") )
		{
			if ( !ParseHeight(val) )
				break;
		}
		else if ( !stricmp(key, "wind") )
		{
			if ( !ParseWindModifier(val) )
				break;
		}
		else if ( !stricmp(key, "rotation") )
		{
			if ( !ParseRotation(val) )
				break;
		}
		else if ( !I_stricmp(key, "rotationDelta") )
		{
			if ( !ParseRotationDelta(val) )
				break;
		}
		else if ( !stricmp(key, "flags") || !stricmp(key, "flag") )
		{
			if ( !ParseAttributeFlags(val) )
				break;
		}
		else if ( !stricmp(key, "spawnFlags") || !stricmp(key, "spawnFlag") )
		{
			if ( !ParseSpawnFlags(val) )
				break;
		}
		else if ( !stricmp(key, "nonUniformScale") )
		{
			mNonUniformScale = atoi(val) != 0;
		}
		else if ( !stricmp(key, "useLength") )
		{
			useLength = atoi(val) != 0;
		}
		else if ( !stricmp(key, "name") )
		{
			if ( !val )
				break;
			I_strncpyz(mName, val, sizeof(mName));
		}
		else if ( !stricmp(key, "shaderImpact") )
		{
			if ( !ParseMaterialImpact(val) )
				break;
		}
		else if ( !stricmp(key, "sequenceStartFrameMode") )
		{
			if ( !ParseSequenceStartFrameMode(val) )
				break;
		}
		else if ( !stricmp(key, "sequenceFixedFrameValue") )
		{
			if ( !ParseSequenceFixedFrameValue(val) )
				break;
		}
		else if ( !stricmp(key, "sequencePlayRateMode") )
		{
			if ( !ParseSequencePlayRateMode(val) )
				break;
		}
		else if ( !stricmp(key, "sequenceFixedFpsValue") )
		{
			if ( !ParseSequenceFixedFpsValue(val) )
				break;
		}
		else if ( !stricmp(key, "sequenceLoopMode") )
		{
			if ( !ParseSequenceLoopMode(val) )
				break;
		}
		else if ( !stricmp(key, "sequenceLoopTimes") )
		{
			if ( !ParseSequenceLoopTimes(val) )
				break;
		}
		else if ( !stricmp(key, "spawnFrustumCullRadius") )
		{
			if ( !ParseSpawnFrustumCullRadius(val) )
				break;
		}
		else
		{
			FX_Print("Unknown key parsing an effect primitive: %s\n", key);
			return false;
		}
		pairs = pairs->GetNext();
		key = NULL;
	}
	if ( key )
	{
		FX_Print("^1FX Error while parsing key '%s'\n", key);
		return false;
	}
	if ( mMin[0] > mMax[0] || mMin[1] > mMax[1] || mMin[2] > mMax[2] )
	{
		FX_Print("^1FX bounding box mins / maxs invalid for effect '%s'\n", mName);
		return false;
	}
	if ( mMax[0] - mMin[0] > mMax[2] - mMin[2] || mMax[1] - mMin[1] > mMax[2] - mMin[2] )
	{
		FX_Print("^1FX bounding box width or depth is larger than height for effect '%s'\n", mName);
		return false;
	}
	subGrp = grp->GetSubGroups();
	while ( subGrp )
	{
		key = subGrp->GetName();
		if ( !stricmp(key, "rgb") )
		{
			if ( !ParseChannel(bcp, subGrp, FX_CHANNEL_COLOR) )
				break;
		}
		else if ( !stricmp(key, "rgb2") || !stricmp(key, "rgbRand") )
		{
			if ( !ParseChannel(bcp, subGrp, FX_CHANNEL_COLOR_RAND) )
				break;
		}
		else if ( !stricmp(key, "alpha") )
		{
			if ( !ParseChannel(bcp, subGrp, FX_CHANNEL_ALPHA) )
				break;
		}
		else if ( !stricmp(key, "alphaRand") )
		{
			if ( !ParseChannel(bcp, subGrp, FX_CHANNEL_ALPHA_RAND) )
				break;
		}
		else if ( !stricmp(key, "size") || !stricmp(key, "width") )
		{
			if ( !ParseChannel(bcp, subGrp, FX_CHANNEL_SIZE) )
				break;
		}
		else if ( !stricmp(key, "sizeRand") )
		{
			if ( !ParseChannel(bcp, subGrp, FX_CHANNEL_SIZE_RAND) )
				break;
		}
		else if ( !stricmp(key, "size2") || !stricmp(key, "width2") )
		{
			if ( !ParseChannel(bcp, subGrp, FX_CHANNEL_SIZE2) )
				break;
		}
		else if ( !stricmp(key, "size2Rand") )
		{
			if ( !ParseChannel(bcp, subGrp, FX_CHANNEL_SIZE2_RAND) )
				break;
		}
		else if ( !stricmp(key, "length") || !stricmp(key, "height") )
		{
			if ( !ParseChannel(bcp, subGrp, FX_CHANNEL_LENGTH) )
				break;
		}
		else if ( !stricmp(key, "lengthRand") )
		{
			if ( !ParseChannel(bcp, subGrp, FX_CHANNEL_LENGTH_RAND) )
				break;
		}
		else if ( !stricmp(key, "rotationDelta") )
		{
			if ( !ParseChannel(bcp, subGrp, FX_CHANNEL_ROTATION_DELTA) )
				break;
		}
		else if ( !stricmp(key, "rotationDeltaRand") )
		{
			if ( !ParseChannel(bcp, subGrp, FX_CHANNEL_ROTATION_DELTA_RAND) )
				break;
		}
		else if ( !stricmp(key, "velocityX") )
		{
			if ( !ParseChannel(bcp, subGrp, FX_CHANNEL_VELOCITY_X) )
				break;
		}
		else if ( !stricmp(key, "velocityY") )
		{
			if ( !ParseChannel(bcp, subGrp, FX_CHANNEL_VELOCITY_Y) )
				break;
		}
		else if ( !stricmp(key, "velocityZ") )
		{
			if ( !ParseChannel(bcp, subGrp, FX_CHANNEL_VELOCITY_Z) )
				break;
		}
		else if ( !stricmp(key, "velocityXRand") )
		{
			if ( !ParseChannel(bcp, subGrp, FX_CHANNEL_VELOCITY_X_RAND) )
				break;
		}
		else if ( !stricmp(key, "velocityYRand") )
		{
			if ( !ParseChannel(bcp, subGrp, FX_CHANNEL_VELOCITY_Y_RAND) )
				break;
		}
		else if ( !stricmp(key, "velocityZRand") )
		{
			if ( !ParseChannel(bcp, subGrp, FX_CHANNEL_VELOCITY_Z_RAND) )
				break;
		}
		else if ( !stricmp(key, "velocity2X") )
		{
			if ( !ParseChannel(bcp, subGrp, FX_CHANNEL_VELOCITY2_X) )
				break;
		}
		else if ( !stricmp(key, "velocity2Y") )
		{
			if ( !ParseChannel(bcp, subGrp, FX_CHANNEL_VELOCITY2_Y) )
				break;
		}
		else if ( !stricmp(key, "velocity2Z") )
		{
			if ( !ParseChannel(bcp, subGrp, FX_CHANNEL_VELOCITY2_Z) )
				break;
		}
		else if ( !stricmp(key, "velocity2XRand") )
		{
			if ( !ParseChannel(bcp, subGrp, FX_CHANNEL_VELOCITY2_X_RAND) )
				break;
		}
		else if ( !stricmp(key, "velocity2YRand") )
		{
			if ( !ParseChannel(bcp, subGrp, FX_CHANNEL_VELOCITY2_Y_RAND) )
				break;
		}
		else if ( !stricmp(key, "velocity2ZRand") )
		{
			if ( !ParseChannel(bcp, subGrp, FX_CHANNEL_VELOCITY2_Z_RAND) )
				break;
		}
		else
		{
			FX_Print("Unknown group key parsing a particle: %s\n", key);
			return false;
		}
		subGrp = subGrp->GetNext();
		key = NULL;
	}
	if ( key )
	{
		FX_Print("^1FX Error while parsing key '%s'\n", key);
		return false;
	}
	MigrateBackCompatibleParameters(bcp);
	return true;
}

bool PrimitiveTemplate::ParsePrimitive( GPGroup *grp )
{
	bool success;
	BackCompatibleParameters *bcp;

	bcp = (BackCompatibleParameters *)Hunk_AllocateTempMemoryInternal(sizeof(*bcp));
	success = ParsePrimitiveInternal(bcp, grp);
	Hunk_FreeTempMemory(bcp);
	return success;
}

void FX_Print( const char *fmt, ... )
{
	va_list ap;
	char msg[1024];

	va_start(ap, fmt);
	vsprintf(msg, fmt, ap);
	va_end(ap);
	Com_Printf(msg);
}

Material *FX_RegisterMaterial( const char *name )
{
	return re.RegisterMaterial(name, 3, 6);
}

XModel *FX_ModelRegister( const char *name )
{
	XModel *model;

	if ( !Com_ValidXModelName(name) )
		return NULL;
	model = FX_XModelPrecache(name + 7);
	return model;
}

// Never called; original name unknown.
// retail returns int (mov eax,1)
int FX_PrimTypeIsEmitter( PrimType type )
{
	switch ( type )
	{
	case PRIM_PARTICLE:
	case PRIM_TAIL:
	case PRIM_DECAL:
	case PRIM_ORIENTED_PARTICLE:
	case PRIM_CLOUD:
		return false;
	case PRIM_EMITTER:
		return true;
	default:
		return false;
	}
}

// retail 0x49b650, unreferenced; name unknown (ret 8 = stdcall like FxRange_GetValPct)
void __stdcall FxRange_Set_49b650( FxRange *range, float min, float max )
{
	range->min = min;
	range->max = max;
}

float FxRange_GetVal( const FxRange *range )
{
	if ( range->min == range->max )
		return range->min;
	return flrand(range->min, range->max);
}

float __stdcall FxRange_GetValPct( const FxRange *range, float pct )
{
	return range->min + ( range->max - range->min ) * pct;
}

