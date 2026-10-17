#include <string.h>
#include "com_math.h"

struct orientation_t
{
	vec3_t origin;
	vec3_t axis[3];
};

struct trace_t
{
	float fraction;
	vec3_t normal;
	int surfaceFlags;
	int contents;
	const char *material;
	unsigned short entityNum;
	unsigned short partName;
	unsigned short partGroup;
	unsigned char allsolid;
	unsigned char startsolid;
};

struct XModel;
struct Material;
struct FxEffectDef;
struct MemoryFile;

struct FxCurve
{
	int dimensionCount;
	int keyCount;
	float keys[1];
};

struct FxCurveIterator
{
	const FxCurve *master;
	int currentKeyIndex;
};

struct FxChannelInstance
{
	FxCurveIterator curveIterator;
	float scale;
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

enum
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

// Only the channel table is read here.
struct PrimitiveTemplate
{
	char header[0x100];
	FxChannel mFxChannels[FX_CHANNEL_COUNT];
};

struct FxBoltInfo
{
	int dobjHandle;
	int boneIndex;
};

#include "FxCamera.h"

struct GfxEntity
{
	int reType;
	int renderFxFlags;
	vec3_t lightingOrigin;
	vec3_t axis[3];
	float scale;
	vec3_t origin;
	vec3_t endpos;
	Material *customMaterial;
	unsigned char materialRGBA[4];
	float materialTime;
	int materialSubimageIndex;
	float radius[2];
	float rotation;
	float minScreenRadius;
};

#include "FxHelper.h"

struct FxScheduler
{
	void PlayEffect( const FxEffectDef *fx, const vec3_t origin, const vec3_t dir );
	void PlayEffect( const FxEffectDef *fx, const vec3_t origin, const vec3_t axis[3], const FxBoltInfo *bolt );
};

extern FxHelper *theFxHelper;
extern FxScheduler *theFxScheduler;

// Every primitive type is carved from 32 KB blocks of the shared effects arena.
struct FxMemBlock
{
	char data[0x7FF0];
	int freeCount;
	void *freeList;
	FxMemBlock *next;
	FxMemBlock *prev;
};

FxMemBlock *FxMem_AllocBlock( FxMemBlock *head, int itemCount, int itemSize );
void FxMem_UnlinkBlock( FxMemBlock *block );
void FxMem_FreeBlock( FxMemBlock *block );
FxMemBlock *FxMem_FreeItem( void *item, unsigned int itemSize ) throw();

void Com_Memset( void *dest, int val, int count );

template <class T>
class FxMemMgr
{
public:
	FxMemMgr()
	{
		mItemsPerBlock = sizeof( ( (FxMemBlock *)0 )->data ) / sizeof( T );
		mHead = 0;
	}

	void *Alloc( unsigned int size )
	{
		void *item;
		FxMemBlock *block;

		for ( block = mHead; block; block = block->next )
		{
			if ( block->freeList )
			{
				if ( block != mHead )
					MoveToFront( block );
				break;
			}
		}
		if ( !block )
		{
			block = FxMem_AllocBlock( mHead, mItemsPerBlock, size );
			if ( !block )
				return 0;
			mHead = block;
		}
		item = block->freeList;
		block->freeList = *(void **)block->freeList;
		block->freeCount--;
		Com_Memset( item, 0, sizeof( T ) );
		return item;
	}

	void Free( void *item );

private:
	void MoveToFront( FxMemBlock *block );

	int mItemsPerBlock;
	FxMemBlock *mHead;
};

template <class T>
void FxMemMgr<T>::Free( void *item )
{
	FxMemBlock *block;

	block = FxMem_FreeItem( item, sizeof( T ) );
	if ( !mHead->freeList )
	{
		MoveToFront( block );
		return;
	}
	if ( block->freeCount < mItemsPerBlock )
		return;
	if ( block == mHead )
	{
		if ( !block->next || !block->next->freeCount )
			return;
		mHead = block->next;
	}
	FxMem_FreeBlock( block );
}

template <class T>
void FxMemMgr<T>::MoveToFront( FxMemBlock *block )
{
	FxMem_UnlinkBlock( block );
	block->prev = 0;
	block->next = mHead;
	mHead->prev = block;
	mHead = block;
}

void FxCurveIterator_Create( FxCurveIterator *createe, const FxCurve *master );
float FxCurve_Integrate( const FxCurve *curve, float normDuration );
float FxCurve_Interpolate1d( const float *key, float t ) throw();

inline void FxCurve_Interpolate3d( const float *key, float t, vec3_t out )
{
	float t0;
	float t1;
	const float *v0;
	const float *v1;

	t0 = key[0];
	t1 = key[4];
	v0 = &key[1];
	v1 = &key[5];
	Vec3LerpFrom( v0, v1, ( t - t0 ) / ( t1 - t0 ), out );
}

inline void FxCurveIterator_Advance( FxCurveIterator *iter, float t )
{
	const float *key;
	int keySize;

	keySize = iter->master->dimensionCount + 1;
	key = &iter->master->keys[iter->currentKeyIndex * keySize];
	if ( t < key[0] )
	{
		iter->currentKeyIndex = 0;
		key = iter->master->keys;
	}
	while ( t > key[keySize] )
	{
		iter->currentKeyIndex++;
		key += keySize;
	}
}

inline float FxCurveIterator_Interpolate1d( FxCurveIterator *iter, float t )
{
	const float *key;
	float value;

	FxCurveIterator_Advance( iter, t );
	key = &iter->master->keys[iter->currentKeyIndex * 2];
	value = FxCurve_Interpolate1d( key, t );
	return value;
}

inline void FxCurveIterator_Interpolate3d( FxCurveIterator *iter, vec3_t out, float t )
{
	const float *key;

	FxCurveIterator_Advance( iter, t );
	key = &iter->master->keys[iter->currentKeyIndex * 4];
	FxCurve_Interpolate3d( key, t, out );
}

inline float FxChannelInstance_GetValue1d( FxChannelInstance *inst, float t )
{
	return FxCurveIterator_Interpolate1d( &inst->curveIterator, t ) * inst->scale;
}

inline void FxChannelInstance_GetValue3d( FxChannelInstance *inst, vec3_t out, float t )
{
	FxCurveIterator_Interpolate3d( &inst->curveIterator, out, t );
	VectorScale( out, inst->scale, out );
}

inline float FxChannelInstance_Blend1d( FxChannelInstance *inst, FxChannelInstance *randInst, float blend, float t )
{
	float value[2];

	value[0] = FxCurveIterator_Interpolate1d( &inst->curveIterator, t );
	value[1] = FxCurveIterator_Interpolate1d( &randInst->curveIterator, t );
	value[0] = value[0] + ( value[1] - value[0] ) * blend;
	return value[0] * inst->scale;
}

inline void FxChannelInstance_Blend3d( FxChannelInstance *inst, FxChannelInstance *randInst, float blend, vec3_t out, float t )
{
	vec3_t to;
	vec3_t delta;

	FxCurveIterator_Interpolate3d( &inst->curveIterator, out, t );
	FxCurveIterator_Interpolate3d( &randInst->curveIterator, to, t );
	VectorSubtract( to, out, delta );
	VectorMA( out, blend, delta, out );
	VectorScale( out, inst->scale, out );
}

inline float FxChannelInstance_Integrate( FxChannelInstance *inst, float normDuration )
{
	return FxCurve_Integrate( inst->curveIterator.master, normDuration ) * inst->scale;
}

inline float FxChannelInstance_IntegrateBlend( FxChannelInstance *inst, FxChannelInstance *randInst, float blend, float normDuration )
{
	float value[2];

	value[0] = FxCurve_Integrate( inst->curveIterator.master, normDuration );
	value[1] = FxCurve_Integrate( randInst->curveIterator.master, normDuration );
	value[0] = value[0] + ( value[1] - value[0] ) * blend;
	return value[0] * inst->scale;
}

float crandom();

class FxArchive;
class FxBoltFramePtr;

// A bone orientation shared by every effect bolted to it, refreshed once per frame.
class FxBoltFrame
{
public:
	FxBoltFrame *AddRef()
	{
		refCount++;
		return this;
	}

	static void *operator new( unsigned int size )
	{
		return s_memMgr.Alloc( size );
	}

	static void operator delete( void *p )
	{
		s_memMgr.Free( p );
	}

	FxBoltFrame( const FxBoltInfo &bolt );

	static FxBoltFramePtr Acquire( const FxBoltInfo &bolt );
	void Release();
	const orientation_t *GetOrientation();

	int refCount;
	int mTime;
	orientation_t mOrientation;
	FxBoltFrame *next;
	FxBoltInfo mBolt;

	static FxBoltFrame *g_mFrameList;
	static FxMemMgr<FxBoltFrame> s_memMgr;
};

class FxBoltFramePtr
{
public:
	FxBoltFramePtr();
	FxBoltFramePtr( FxBoltFrame *frame );
	~FxBoltFramePtr();
	void operator=( const FxBoltFramePtr &other );
	bool IsValid() const;
	void operator=( FxBoltFrame *frame );
	FxBoltFrame *Get() const;
	void Archive( FxArchive *arch );

private:
	FxBoltFrame *mFrame;
};

inline FxBoltFramePtr::FxBoltFramePtr()
{
	mFrame = 0;
}

inline FxBoltFramePtr::FxBoltFramePtr( FxBoltFrame *frame )
{
	mFrame = frame->AddRef();
}

inline FxBoltFramePtr::~FxBoltFramePtr()
{
	if ( mFrame )
		mFrame->Release();
}

inline void FxBoltFramePtr::operator=( const FxBoltFramePtr &other )
{
	if ( mFrame == other.mFrame )
		return;
	if ( mFrame )
	{
		mFrame->Release();
		mFrame = 0;
	}
	if ( other.IsValid() )
		mFrame = other.mFrame->AddRef();
}

inline bool FxBoltFramePtr::IsValid() const
{
	return mFrame != 0;
}

inline void FxBoltFramePtr::operator=( FxBoltFrame *frame )
{
	if ( mFrame == frame )
		return;
	if ( mFrame )
		mFrame->Release();
	mFrame = frame->AddRef();
}

inline FxBoltFrame *FxBoltFramePtr::Get() const
{
	return mFrame;
}

struct FxGfxEntity
{
	Material *customMaterial;
	float rotation;
	vec3_t axis[3];
	vec3_t dlightColor;
	float materialTime;
	vec3_t origin;
	float radius[2];
	unsigned char materialRGBA[4];
	int materialSubimageIndex;
	float scale;
	vec3_t endpos;
};

class Effect
{
public:
	Effect();
	virtual ~Effect();
	virtual void Die();
	virtual bool Update();
	virtual bool Cull();
	virtual void Draw();

	virtual float GetVisibility( const vec3_t start, const vec3_t dir, float halfLen )
	{
		return 1.0f;
	}

	virtual void AddVisibility()
	{
	}

	virtual void CreateChannelInstances( const PrimitiveTemplate *primTemp ) = 0;
	virtual unsigned char TypeID();
	virtual void Archive( FxArchive *arch );
	virtual void FixupArchiveLoad( const PrimitiveTemplate *primTemplate );

	bool GetBoltOrientation( const orientation_t **orient );
	int GetCullPlaneCount();
	void SetTimeStartEnd( int start, int end );
	int GetAge();
	void SetBoltFrame( const FxBoltFramePtr &boltFrame );

	int GetDuration()
	{
		return mTimeEnd - mTimeStart;
	}

	static void operator delete( void *p )
	{
		s_memMgr.Free( p );
	}

	vec3_t mOrigin;
	int mUnknown10;			// archived, never read here
	vec3_t mMins;
	vec3_t mMaxs;
	const FxEffectDef *mImpactFx;
	const FxEffectDef *mDeathFx;
	const FxEffectDef *mUnknownFx;	// archived, never read here
	int mUnknown38;			// archived, never read here
	float mNormTime;
	FxGfxEntity mRefEnt;
	int mFlags;
	int mClusterId;
	int mSortGroup;
	XModel *mModel;
	int mTimeStart;
	int mTimeEnd;
	FxBoltFramePtr mBolt;

	static FxMemMgr<Effect> s_memMgr;
};

class Light : public Effect
{
public:
	static void operator delete( void *p )
	{
		s_memMgr.Free( p );
	}

	Light();
	virtual ~Light();
	virtual bool Update();
	virtual bool Cull();
	virtual void Draw();
	virtual void CreateChannelInstances( const PrimitiveTemplate *primTemp );
	virtual unsigned char TypeID();
	virtual void Archive( FxArchive *arch );
	virtual void FixupArchiveLoad( const PrimitiveTemplate *primTemplate );

	void UpdateSize();
	void UpdateRGB();

	float mColorBlendFactor;
	float mSizeBlendFactor;
	FxChannelInstance mColorChannelInstance;
	FxChannelInstance mColorRandChannelInstance;
	FxChannelInstance mSizeChannelInstance;
	FxChannelInstance mSizeRandChannelInstance;

	static FxMemMgr<Light> s_memMgr;
};

class Flash : public Light
{
public:
	virtual ~Flash() {}
	virtual bool Update();

	virtual bool Cull()
	{
		return false;
	}

	virtual void Draw();
	virtual unsigned char TypeID();
	virtual void Archive( FxArchive *arch );

	void Init();
};

class Particle : public Effect
{
public:
	static void operator delete( void *p )
	{
		s_memMgr.Free( p );
	}

	Particle();
	virtual ~Particle();
	virtual void Die();
	virtual bool Update();
	virtual bool Cull();
	virtual void Draw();
	virtual float GetVisibility( const vec3_t start, const vec3_t dir, float halfLen );
	virtual void AddVisibility();
	virtual void CreateChannelInstances( const PrimitiveTemplate *primTemp );
	virtual unsigned char TypeID();
	virtual void Archive( FxArchive *arch );
	virtual void FixupArchiveLoad( const PrimitiveTemplate *primTemplate );

	void SetAxis( const vec3_t *ax );
	void SetRandomVelocityWeights( float weight1, float weight2, float weight3 );
	void SetRandomVelocity2Weights( float weight1, float weight2, float weight3 );
	float GetNormTime( float age, float life );
	void StopMoving();
	void ApplyImpact( const orientation_t *or_, float normTime, const vec3_t velocity, float traceFraction, const vec3_t traceNormal );
	bool UpdateOrigin( const orientation_t *or_ );
	void CalcVelocityValue( float normTime, vec3_t outVector, const orientation_t *or_ );
	void CalcVelocity2Value( float normTime, vec3_t outVector, const orientation_t *or_ );
	void CalcGravityValue( float normTime, vec3_t outVector, const orientation_t *or_ );
	void IntegrateVelocity( float normDuration, vec3_t outVector );
	void IntegrateVelocity2( float normDuration, vec3_t outVector );
	void IntegrateGravity( int duration, vec3_t outVector );
	void IntegrateTotalVelocity( int duration, vec3_t outVector );
	void GetTotalVelocityAtTime0( vec3_t outVector );
	void GetTotalVelocity( float normTime, vec3_t outVector, const orientation_t *or_ );
	void UpdateSize();
	void UpdateSize2();
	void UpdateRGB();
	void UpdateAlpha();
	void UpdateRotation();
	void UpdateSubimage();

	vec3_t mImpactVelocity;
	vec3_t mDisplayAxis[3];
	float mGravity;
	float mWindModifier;
	int mUnknownFC;
	float mElasticity;
	bool mNonUniformScale;
	int mStartFrame;
	float mFrameRate;
	int mLoopMode;
	int mLoopTimes;
	float mColorBlendFactor;
	float mAlphaBlendFactor;
	float mSizeBlendFactor;
	float mSize2BlendFactor;
	float mRotationBlendFactor;
	vec3_t mVelocityWeights;
	vec3_t mVelocity2Weights;
	FxChannelInstance mColorChannelInstance;
	FxChannelInstance mColorRandChannelInstance;
	FxChannelInstance mAlphaChannelInstance;
	FxChannelInstance mAlphaRandChannelInstance;
	FxChannelInstance mSizeChannelInstance;
	FxChannelInstance mSizeRandChannelInstance;
	FxChannelInstance mSize2ChannelInstance;
	FxChannelInstance mSize2RandChannelInstance;
	FxChannelInstance mRotationDeltaChannelInstance;
	FxChannelInstance mRotationDeltaRandChannelInstance;
	FxChannelInstance mVelocityChannelInstance[3];
	FxChannelInstance mVelocityRandChannelInstance[3];
	FxChannelInstance mVelocity2ChannelInstance[3];
	FxChannelInstance mVelocity2RandChannelInstance[3];

	static FxMemMgr<Particle> s_memMgr;
};

class Cloud : public Particle
{
public:
	static void operator delete( void *p )
	{
		s_memMgr.Free( p );
	}

	Cloud();
	virtual ~Cloud();
	virtual void Die();
	virtual bool Update();
	virtual bool Cull();
	virtual void Draw();
	virtual void CreateChannelInstances( const PrimitiveTemplate *primTemp );
	virtual unsigned char TypeID();
	virtual void Archive( FxArchive *arch );
	virtual void FixupArchiveLoad( const PrimitiveTemplate *primTemplate );

	void RandomizeAxis();
	void UpdateLength();

	vec3_t mRotationAxis;
	float mLength;
	bool mUseLength;
	float mLengthBlendFactor;
	FxChannelInstance mLengthChannelInstance;
	FxChannelInstance mLengthRandChannelInstance;

	static FxMemMgr<Cloud> s_memMgr;
};

class Line : public Particle
{
public:
	static void operator delete( void *p )
	{
		s_memMgr.Free( p );
	}

	Line();
	virtual ~Line();
	virtual void Die();
	virtual bool Update();
	virtual bool Cull();
	virtual void Draw();
	virtual unsigned char TypeID();
	virtual void Archive( FxArchive *arch );

	vec3_t mEndpoint;

	static FxMemMgr<Line> s_memMgr;
};

class OrientedParticle : public Particle
{
public:
	static void operator delete( void *p )
	{
		s_memMgr.Free( p );
	}

	OrientedParticle();
	virtual ~OrientedParticle();
	virtual bool Update();
	virtual bool Cull();
	virtual void Draw();
	virtual unsigned char TypeID();
	virtual void Archive( FxArchive *arch );

	vec3_t mNormal;

	static FxMemMgr<OrientedParticle> s_memMgr;
};

class Tail : public Particle
{
public:
	static void operator delete( void *p )
	{
		s_memMgr.Free( p );
	}

	Tail();
	virtual ~Tail();
	virtual bool Update();
	virtual bool Cull();
	virtual void Draw();
	virtual void CreateChannelInstances( const PrimitiveTemplate *primTemp );
	virtual unsigned char TypeID();
	virtual void Archive( FxArchive *arch );
	virtual void FixupArchiveLoad( const PrimitiveTemplate *primTemplate );

	void InitEndPoint();
	void UpdateLength();
	void CalcNewEndpoint( const orientation_t *or_ );

	vec3_t mEndpoint;
	float mLength;
	float mLengthBlendFactor;
	FxChannelInstance mLengthChannelInstance;
	FxChannelInstance mLengthRandChannelInstance;

	static FxMemMgr<Tail> s_memMgr;
};

class Cylinder : public Tail
{
public:
	static void operator delete( void *p )
	{
		s_memMgr.Free( p );
	}

	Cylinder();
	virtual ~Cylinder();
	virtual bool Update();
	virtual bool Cull();
	virtual void Draw();
	virtual unsigned char TypeID();
	virtual void Archive( FxArchive *arch );

	static FxMemMgr<Cylinder> s_memMgr;
};

class Emitter : public Particle
{
public:
	void RandomizeEmitDist()
	{
		mEmitDist = mEmitDistBase + crandom() * mEmitDistRand;
	}

	static void operator delete( void *p )
	{
		s_memMgr.Free( p );
	}

	Emitter();
	virtual ~Emitter();
	virtual bool Update();
	virtual bool Cull();
	virtual void Draw();
	virtual unsigned char TypeID();
	virtual void Archive( FxArchive *arch );

	void UpdateEmitFx( const vec3_t bindVelocity, const orientation_t *or_ );
	void UpdateAngles();

	vec3_t mEmitOrigin;
	vec3_t mEmitVelocity;
	vec3_t mBoltOffset;
	int mEmitTime;
	float mEmitDist;
	vec3_t mAngles;
	vec3_t mAngleDelta;
	const FxEffectDef *mEmitFx;
	float mEmitDistBase;
	float mEmitDistRand;

	static FxMemMgr<Emitter> s_memMgr;
};

class FxArchive
{
private:
	void ReadData( void *p, int byteCount );
	void WriteData( const void *p, int byteCount );

public:
	void ArchiveData( void *p, int byteCount );
	void ArchiveEffect( const FxEffectDef **fx );
	void ArchiveMaterial( Material **material );
	void ArchiveModel( XModel **model );
	void ArchiveChannelInstance( FxChannelInstance *channelInstance );

	bool IsReading()
	{
		return isReading;
	}

	int ReadInt()
	{
		int value;

		ReadData( &value, 4 );
		return value;
	}

	void WriteInt( int value )
	{
		WriteData( &value, 4 );
	}

	void ArchiveByte( unsigned char *value )
	{
		ArchiveData( value, 1 );
	}

	void ArchiveInt( int *value )
	{
		ArchiveData( value, 4 );
	}

	void ArchiveFloat( float *value )
	{
		ArchiveData( value, 4 );
	}

	void ArchiveVec3( vec3_t value )
	{
		ArchiveData( value, 12 );
	}

	MemoryFile *memFile;
	bool isReading;
	bool isWriting;
	int byteCount;
	int literalCount;
	int zeroCount;
	int controlPos;
};

struct EffectVisibility
{
	vec3_t origin;
	float radiusSq;
	float visibility;
};

extern EffectVisibility g_effectVisArray[];
extern int g_effectVisArrayCount;

// Only the field read here: the skeleton frame stamp that invalidates
// cached bolt orientations. The leading members are server_t's (server.h).
struct server_t
{
	char leading[0x5f430];
	int skelTimeStamp;
};
extern server_t sv;

void *Hunk_AllocInternal( int size );
XModel *XModelPrecache( const char *name, void *( *Alloc )( int size ), void *( *AllocColl )( int size ) );
bool FX_GetBoneOrientation( const FxBoltInfo *bolt, orientation_t *orient );
int FX_GetCluster( const vec3_t origin );
void FX_SetSortGroup( Effect *effect );
float FxRange_GetVal( const FxRange *range );
float flrand( float min, float max );
float Vec3Normalize( vec3_t v );
float Vec3DistanceSq( const vec3_t p1, const vec3_t p2 );
void Vec3Lerp( const vec3_t start, const vec3_t end, float fraction, vec3_t endpos );
void AxisCopy( const vec3_t in[3], vec3_t out[3] );
void AxisTransformComponents( const vec3_t axis[3], float x, float y, float z, vec3_t out );
void MakeNormalVectors( const vec3_t forward, vec3_t right, vec3_t up );
void QuatFromAxisAngle( float angle, const vec3_t axis, vec4_t quat );
void QuatToAxis( const vec4_t quat, vec3_t axis[3] );
void AnglesToAxis( const vec3_t angles, vec3_t axis[3] );
void OrientationPosToWorld( const orientation_t *orient, const float *pos, float *out );
void OrientationDirToWorld( const orientation_t *orient, const float *dir, float *out );
void OrientationDirToLocal( const orientation_t *orient, const float *dir, float *out );

FxBoltFrame *FxBoltFrame::g_mFrameList = NULL;
FxMemMgr<FxBoltFrame> FxBoltFrame::s_memMgr;
FxMemMgr<Effect> Effect::s_memMgr;
FxMemMgr<Light> Light::s_memMgr;
FxMemMgr<Particle> Particle::s_memMgr;
FxMemMgr<Line> Line::s_memMgr;
FxMemMgr<OrientedParticle> OrientedParticle::s_memMgr;
FxMemMgr<Tail> Tail::s_memMgr;
FxMemMgr<Cylinder> Cylinder::s_memMgr;
FxMemMgr<Emitter> Emitter::s_memMgr;
FxMemMgr<Cloud> Cloud::s_memMgr;

static void FX_ColorToBytes( const vec3_t rgb, unsigned char *out )
{
	int value;
	int i;

	for ( i = 0; i < 3; i++ )
	{
		value = I_clamp( (int)( rgb[i] * 255.0f ), 0, 255 );
		out[i] = value;
	}
}

FxBoltFramePtr FxBoltFrame::Acquire( const FxBoltInfo &bolt )
{
	FxBoltFrame *frame;

	for ( frame = g_mFrameList; frame; frame = frame->next )
	{
		if ( frame->mBolt.dobjHandle == bolt.dobjHandle && frame->mBolt.boneIndex == bolt.boneIndex )
			return FxBoltFramePtr( frame );
	}
	return FxBoltFramePtr( new FxBoltFrame( bolt ) );
}

void FxBoltFrame::Release()
{
	FxBoltFrame **link;

	refCount--;
	if ( !refCount )
	{
		for ( link = &g_mFrameList; *link; link = &( *link )->next )
		{
			if ( *link == this )
			{
				*link = next;
				break;
			}
		}
		delete this;
	}
}

const orientation_t *FxBoltFrame::GetOrientation()
{
	if ( mBolt.dobjHandle < 0 )
		return 0;
	if ( mTime != sv.skelTimeStamp )
	{
		mTime = sv.skelTimeStamp;
		if ( !FX_GetBoneOrientation( &mBolt, &mOrientation ) )
		{
			mBolt.dobjHandle = -1;
			mBolt.boneIndex = -1;
			return 0;
		}
	}
	return &mOrientation;
}

FxBoltFrame::FxBoltFrame( const FxBoltInfo &bolt )
{
	refCount = 0;
	mTime = 0;
	mBolt = bolt;
	next = g_mFrameList;
	g_mFrameList = this;
}

void FxBoltFramePtr::Archive( FxArchive *arch )
{
	FxBoltInfo bolt;

	if ( arch->IsReading() )
	{
		bolt.dobjHandle = arch->ReadInt();
		if ( bolt.dobjHandle >= 0 )
		{
			bolt.boneIndex = arch->ReadInt();
			*this = FxBoltFrame::Acquire( bolt );
		}
		else
		{
			*this = 0;
		}
	}
	else if ( mFrame )
	{
		arch->WriteInt( mFrame->mBolt.dobjHandle );
		arch->WriteInt( mFrame->mBolt.boneIndex );
	}
	else
	{
		arch->WriteInt( -1 );
	}
}

static void *FxModelAlloc( int size )
{
	return Hunk_AllocInternal( size );
}

XModel *FX_XModelPrecache( const char *name )
{
	return XModelPrecache( name, FxModelAlloc, FxModelAlloc );
}

Effect::Effect()
{
}

Effect::~Effect()
{
}

void Effect::Die()
{
}

bool Effect::Cull()
{
	return false;
}

void Effect::Draw()
{
}

bool Effect::GetBoltOrientation( const orientation_t **orient )
{
	if ( mBolt.IsValid() )
	{
		*orient = mBolt.Get()->GetOrientation();
		if ( !*orient )
			return false;
	}
	return true;
}

// Frustum planes to test against; some effects skip the far planes.
int Effect::GetCullPlaneCount()
{
	if ( mFlags & 0x2000000 )
		return I_min( 5, theFxHelper->mCamera.numPlanes );
	return theFxHelper->mCamera.numPlanes;
}

bool Effect::Update()
{
	int duration;

	if ( mTimeStart > theFxHelper->mTime )
		return false;
	duration = GetDuration();
	mNormTime = (float)( theFxHelper->mTime - mTimeStart ) / duration;
	if ( mNormTime > 1.0f )
		mNormTime = 1.0f;
	if ( mNormTime < 0.0f )
		mNormTime = 0.0f;
	return true;
}

void Effect::SetTimeStartEnd( int start, int end )
{
	mTimeStart = start;
	mTimeEnd = end;
}

int Effect::GetAge()
{
	int age;

	age = theFxHelper->mTime - mTimeStart;
	return age;
}

void Effect::SetBoltFrame( const FxBoltFramePtr &boltFrame )
{
	mBolt = boltFrame;
}

unsigned char Effect::TypeID()
{
	return 0;
}

void Effect::Archive( FxArchive *arch )
{
	arch->ArchiveVec3( mOrigin );
	arch->ArchiveInt( &mTimeStart );
	arch->ArchiveInt( &mTimeEnd );
	arch->ArchiveInt( &mFlags );
	arch->ArchiveInt( &mUnknown10 );
	arch->ArchiveVec3( mMins );
	arch->ArchiveVec3( mMaxs );
	arch->ArchiveEffect( &mImpactFx );
	arch->ArchiveEffect( &mDeathFx );
	arch->ArchiveData( &mRefEnt, sizeof( mRefEnt ) );
	arch->ArchiveEffect( &mUnknownFx );
	arch->ArchiveInt( &mUnknown38 );
	arch->ArchiveMaterial( &mRefEnt.customMaterial );
	arch->ArchiveModel( &mModel );
	mBolt.Archive( arch );
	if ( arch->IsReading() )
	{
		mClusterId = FX_GetCluster( mRefEnt.origin );
		FX_SetSortGroup( this );
	}
}

void Effect::FixupArchiveLoad( const PrimitiveTemplate *primTemplate )
{
}

static void FX_AddFxToScene( Effect *effect, int reType )
{
	GfxEntity ent;

	memset( &ent, 0, sizeof( ent ) );
	ent.reType = reType;
	ent.customMaterial = effect->mRefEnt.customMaterial;
	ent.rotation = effect->mRefEnt.rotation;
	AxisCopy( effect->mRefEnt.axis, ent.axis );
	VectorCopy( effect->mRefEnt.origin, ent.origin );
	ent.radius[0] = effect->mRefEnt.radius[0];
	ent.radius[1] = effect->mRefEnt.radius[1];
	ent.materialRGBA[0] = effect->mRefEnt.materialRGBA[0];
	ent.materialRGBA[1] = effect->mRefEnt.materialRGBA[1];
	ent.materialRGBA[2] = effect->mRefEnt.materialRGBA[2];
	ent.materialRGBA[3] = effect->mRefEnt.materialRGBA[3];
	ent.materialSubimageIndex = effect->mRefEnt.materialSubimageIndex;
	ent.scale = effect->mRefEnt.scale;
	VectorCopy( effect->mRefEnt.endpos, ent.endpos );
	if ( effect->mFlags & 1 )
		ent.renderFxFlags |= 8;
	if ( effect->mFlags & 0x4000000 )
		ent.renderFxFlags |= 0x80;
	theFxHelper->AddFxToScene( &ent, effect->mModel );
}

Particle::Particle()
{
	VectorClear( mImpactVelocity );
}

Particle::~Particle()
{
}

void Particle::Die()
{
	vec3_t dir;
	float len;

	if ( ( mFlags & 0x200 ) && !( mFlags & 0x400 ) && mDeathFx )
	{
		VectorSet( dir, flrand( -1.0f, 1.0f ), flrand( -1.0f, 1.0f ), flrand( -1.0f, 1.0f ) );
		len = VectorLength( dir );
		if ( len < 1e-6 )
			VectorSet( dir, 0.0f, 0.0f, 1.0f );
		else
			VectorScale( dir, 1.0f / len, dir );
		theFxScheduler->PlayEffect( mDeathFx, mOrigin, dir );
	}
}

bool Particle::Cull()
{
	return theFxHelper->CullSphere( mRefEnt.origin, mRefEnt.radius[0], GetCullPlaneCount() );
}

void Particle::Draw()
{
	if ( mRefEnt.radius[0] == 0.0f || mRefEnt.radius[1] == 0.0f )
		return;
	FX_AddFxToScene( this, 4 );
}

bool Particle::Update()
{
	const orientation_t *orient;

	orient = 0;
	if ( !Effect::Update() )
		return false;
	if ( !GetBoltOrientation( &orient ) )
		return false;
	if ( !UpdateOrigin( orient ) )
		return false;
	if ( orient )
		OrientationPosToWorld( orient, mOrigin, mRefEnt.origin );
	else
		VectorCopy( mOrigin, mRefEnt.origin );
	UpdateSize();
	if ( mRefEnt.radius[0] == 0.0f )
		return true;
	UpdateSize2();
	if ( mRefEnt.radius[1] == 0.0f )
		return true;
	UpdateRGB();
	UpdateAlpha();
	UpdateRotation();
	UpdateSubimage();
	return true;
}

void Particle::SetAxis( const vec3_t *ax )
{
	Vec3CopyOrClear( ax[0], mDisplayAxis[0] );
	Vec3CopyOrClear( ax[1], mDisplayAxis[1] );
	Vec3CopyOrClear( ax[2], mDisplayAxis[2] );
}

void FxChannelInstance_Create( const FxChannel *master, FxChannelInstance *createe )
{
	FxCurveIterator_Create( &createe->curveIterator, master->curve );
	createe->scale = FxRange_GetVal( &master->scaleRange );
}

void Particle::CreateChannelInstances( const PrimitiveTemplate *primTemp )
{
	FxChannelInstance_Create( &primTemp->mFxChannels[FX_CHANNEL_COLOR], &mColorChannelInstance );
	FxChannelInstance_Create( &primTemp->mFxChannels[FX_CHANNEL_COLOR_RAND], &mColorRandChannelInstance );
	FxChannelInstance_Create( &primTemp->mFxChannels[FX_CHANNEL_ALPHA], &mAlphaChannelInstance );
	FxChannelInstance_Create( &primTemp->mFxChannels[FX_CHANNEL_ALPHA_RAND], &mAlphaRandChannelInstance );
	FxChannelInstance_Create( &primTemp->mFxChannels[FX_CHANNEL_SIZE], &mSizeChannelInstance );
	FxChannelInstance_Create( &primTemp->mFxChannels[FX_CHANNEL_SIZE_RAND], &mSizeRandChannelInstance );
	FxChannelInstance_Create( &primTemp->mFxChannels[FX_CHANNEL_SIZE2], &mSize2ChannelInstance );
	FxChannelInstance_Create( &primTemp->mFxChannels[FX_CHANNEL_SIZE2_RAND], &mSize2RandChannelInstance );
	FxChannelInstance_Create( &primTemp->mFxChannels[FX_CHANNEL_ROTATION_DELTA], &mRotationDeltaChannelInstance );
	FxChannelInstance_Create( &primTemp->mFxChannels[FX_CHANNEL_ROTATION_DELTA_RAND], &mRotationDeltaRandChannelInstance );
	FxChannelInstance_Create( &primTemp->mFxChannels[FX_CHANNEL_VELOCITY_X], &mVelocityChannelInstance[0] );
	FxChannelInstance_Create( &primTemp->mFxChannels[FX_CHANNEL_VELOCITY_Y], &mVelocityChannelInstance[1] );
	FxChannelInstance_Create( &primTemp->mFxChannels[FX_CHANNEL_VELOCITY_Z], &mVelocityChannelInstance[2] );
	FxChannelInstance_Create( &primTemp->mFxChannels[FX_CHANNEL_VELOCITY_X_RAND], &mVelocityRandChannelInstance[0] );
	FxChannelInstance_Create( &primTemp->mFxChannels[FX_CHANNEL_VELOCITY_Y_RAND], &mVelocityRandChannelInstance[1] );
	FxChannelInstance_Create( &primTemp->mFxChannels[FX_CHANNEL_VELOCITY_Z_RAND], &mVelocityRandChannelInstance[2] );
	FxChannelInstance_Create( &primTemp->mFxChannels[FX_CHANNEL_VELOCITY2_X], &mVelocity2ChannelInstance[0] );
	FxChannelInstance_Create( &primTemp->mFxChannels[FX_CHANNEL_VELOCITY2_Y], &mVelocity2ChannelInstance[1] );
	FxChannelInstance_Create( &primTemp->mFxChannels[FX_CHANNEL_VELOCITY2_Z], &mVelocity2ChannelInstance[2] );
	FxChannelInstance_Create( &primTemp->mFxChannels[FX_CHANNEL_VELOCITY2_X_RAND], &mVelocity2RandChannelInstance[0] );
	FxChannelInstance_Create( &primTemp->mFxChannels[FX_CHANNEL_VELOCITY2_Y_RAND], &mVelocity2RandChannelInstance[1] );
	FxChannelInstance_Create( &primTemp->mFxChannels[FX_CHANNEL_VELOCITY2_Z_RAND], &mVelocity2RandChannelInstance[2] );
}

void Particle::SetRandomVelocityWeights( float weight1, float weight2, float weight3 )
{
	VectorSet( mVelocityWeights, weight1, weight2, weight3 );
}

void Particle::SetRandomVelocity2Weights( float weight1, float weight2, float weight3 )
{
	VectorSet( mVelocity2Weights, weight1, weight2, weight3 );
}

float Particle::GetVisibility( const vec3_t start, const vec3_t dir, float halfLen )
{
	vec3_t delta;
	vec3_t closest;
	float along;
	float distSq;

	VectorSubtract( mRefEnt.origin, start, delta );
	along = DotProduct( delta, dir );
	if ( I_fabs( along - halfLen ) > halfLen )
		return 1.0f;
	VectorMA( start, along, dir, closest );
	distSq = Vec3DistanceSq( mRefEnt.origin, closest );
	if ( mRefEnt.radius[0] * mRefEnt.radius[0] > distSq )
		return 1.0f - mRefEnt.materialRGBA[3] * 0.003921569f;
	return 1.0f;
}

void Particle::AddVisibility()
{
	EffectVisibility *vis;

	vis = &g_effectVisArray[g_effectVisArrayCount];
	g_effectVisArrayCount++;
	VectorCopy( mRefEnt.origin, vis->origin );
	vis->radiusSq = mRefEnt.radius[0] * mRefEnt.radius[0];
	vis->visibility = 1.0f - mRefEnt.materialRGBA[3] * 0.0039215689f;
}

float Particle::GetNormTime( float age, float life )
{
	float normTime;

	normTime = age / life;
	if ( normTime > 1.0f )
		normTime = 1.0f;
	return normTime;
}

// A slow particle landing on a floor, or one stuck at its start, comes to rest.
static bool FX_ShouldStopMoving( const vec3_t velocity, const vec3_t normal, float fraction )
{
	if ( normal[2] > 0.0f && VectorLengthSquared( velocity ) < 16.0f )
		return true;
	if ( fraction == 0.0f )
		return true;
	return false;
}

void Particle::StopMoving()
{
	mFlags &= ~0x820;
	mFlags |= 0x1000000;
}

void Particle::ApplyImpact( const orientation_t *or_, float normTime, const vec3_t velocity, float traceFraction, const vec3_t traceNormal )
{
	vec3_t normal;
	vec3_t newVelocity;
	float dot;
	vec3_t delta;

	if ( FX_ShouldStopMoving( velocity, traceNormal, traceFraction ) )
	{
		StopMoving();
		return;
	}
	if ( or_ )
		OrientationDirToLocal( or_, traceNormal, normal );
	else
		VectorCopy( traceNormal, normal );
	GetTotalVelocity( normTime, newVelocity, or_ );
	VectorSubtract( newVelocity, mImpactVelocity, delta );
	VectorScale( newVelocity, mElasticity, newVelocity );
	VectorSubtract( newVelocity, delta, mImpactVelocity );
	dot = DotProduct( newVelocity, normal );
	VectorMA( mImpactVelocity, dot * -2.0f, normal, mImpactVelocity );
}

static bool FX_TraceHit( const trace_t *trace )
{
	if ( trace->startsolid )
		return false;
	if ( trace->allsolid )
		return false;
	if ( trace->fraction == 1.0f )
		return false;
	return true;
}

bool Particle::UpdateOrigin( const orientation_t *or_ )
{
	trace_t trace;
	vec3_t end;
	vec3_t velocity;
	vec3_t start;
	vec3_t endWorld;
	float frameTime;
	float normTime;
	float life;
	float age;

	if ( mFlags & 0x1000000 )
		return true;
	if ( !theFxHelper->mFrameTime )
		return true;
	frameTime = theFxHelper->mFrameTime * 0.001f;
	age = GetAge() * 0.001f;
	life = GetDuration() * 0.001f;
	normTime = GetNormTime( age, life );
	GetTotalVelocity( normTime, velocity, or_ );
	VectorMA( mOrigin, frameTime, velocity, end );
	if ( mFlags & 0x20 )
	{
		if ( or_ )
		{
			OrientationPosToWorld( or_, mOrigin, start );
			OrientationPosToWorld( or_, end, endWorld );
		}
		else
		{
			VectorCopy( mOrigin, start );
			VectorCopy( end, endWorld );
		}
		if ( mFlags & 0x40 )
		{
			vec3_t mins;
			vec3_t maxs;

			VectorCopy( mMins, mins );
			VectorCopy( mMaxs, maxs );
			theFxHelper->Trace( &trace, start, mins, maxs, endWorld, -1, 1 );
		}
		else
			theFxHelper->Trace( &trace, start, vec3_origin, vec3_origin, endWorld, -1, 1 );
		if ( FX_TraceHit( &trace ) )
		{
			if ( mFlags & 0x800 )
			{
				vec3_t hit;

				Vec3Lerp( start, endWorld, trace.fraction, hit );
				theFxScheduler->PlayEffect( mImpactFx, hit, trace.normal );
			}
			if ( mFlags & 0x400 )
				return false;
			normTime = GetNormTime( frameTime * trace.fraction + age, life );
			ApplyImpact( or_, normTime, velocity, trace.fraction, trace.normal );
			Vec3Lerp( mOrigin, end, trace.fraction, mOrigin );
			return true;
		}
	}
	VectorCopy( end, mOrigin );
	return true;
}

void Particle::CalcVelocityValue( float normTime, vec3_t outVector, const orientation_t *or_ )
{
	vec3_t value;

	if ( mFlags & 0x80000 )
	{
		value[0] = FxChannelInstance_Blend1d( &mVelocityChannelInstance[0], &mVelocityRandChannelInstance[0], mVelocityWeights[0], normTime );
		value[1] = FxChannelInstance_Blend1d( &mVelocityChannelInstance[1], &mVelocityRandChannelInstance[1], mVelocityWeights[1], normTime );
		value[2] = FxChannelInstance_Blend1d( &mVelocityChannelInstance[2], &mVelocityRandChannelInstance[2], mVelocityWeights[2], normTime );
	}
	else
	{
		value[0] = FxChannelInstance_GetValue1d( &mVelocityChannelInstance[0], normTime );
		value[1] = FxChannelInstance_GetValue1d( &mVelocityChannelInstance[1], normTime );
		value[2] = FxChannelInstance_GetValue1d( &mVelocityChannelInstance[2], normTime );
	}
	if ( mFlags & 0x200000 )
	{
		if ( or_ )
			OrientationDirToLocal( or_, value, outVector );
		else
			VectorCopy( value, outVector );
	}
	else if ( or_ )
		VectorCopy( value, outVector );
	else
		AxisTransformComponents( mDisplayAxis, value[0], value[1], value[2], outVector );
}

void Particle::CalcVelocity2Value( float normTime, vec3_t outVector, const orientation_t *or_ )
{
	vec3_t value;

	if ( mFlags & 0x100000 )
	{
		value[0] = FxChannelInstance_Blend1d( &mVelocity2ChannelInstance[0], &mVelocity2RandChannelInstance[0], mVelocity2Weights[0], normTime );
		value[1] = FxChannelInstance_Blend1d( &mVelocity2ChannelInstance[1], &mVelocity2RandChannelInstance[1], mVelocity2Weights[1], normTime );
		value[2] = FxChannelInstance_Blend1d( &mVelocity2ChannelInstance[2], &mVelocity2RandChannelInstance[2], mVelocity2Weights[2], normTime );
	}
	else
	{
		value[0] = FxChannelInstance_GetValue1d( &mVelocity2ChannelInstance[0], normTime );
		value[1] = FxChannelInstance_GetValue1d( &mVelocity2ChannelInstance[1], normTime );
		value[2] = FxChannelInstance_GetValue1d( &mVelocity2ChannelInstance[2], normTime );
	}
	if ( mFlags & 0x400000 )
	{
		if ( or_ )
			OrientationDirToLocal( or_, value, outVector );
		else
			VectorCopy( value, outVector );
	}
	else if ( or_ )
		VectorCopy( value, outVector );
	else
		AxisTransformComponents( mDisplayAxis, value[0], value[1], value[2], outVector );
}

void Particle::CalcGravityValue( float normTime, vec3_t outVector, const orientation_t *or_ )
{
	vec3_t value;

	IntegrateGravity( (int)( GetDuration() * normTime ), value );
	if ( or_ )
		OrientationDirToLocal( or_, value, outVector );
	else
		VectorCopy( value, outVector );
}

void Particle::IntegrateVelocity( float normDuration, vec3_t outVector )
{
	vec3_t value;

	if ( mFlags & 0x80000 )
	{
		value[0] = FxChannelInstance_IntegrateBlend( &mVelocityChannelInstance[0], &mVelocityRandChannelInstance[0], mVelocityWeights[0], normDuration );
		value[1] = FxChannelInstance_IntegrateBlend( &mVelocityChannelInstance[1], &mVelocityRandChannelInstance[1], mVelocityWeights[1], normDuration );
		value[2] = FxChannelInstance_IntegrateBlend( &mVelocityChannelInstance[2], &mVelocityRandChannelInstance[2], mVelocityWeights[2], normDuration );
	}
	else
	{
		value[0] = FxChannelInstance_Integrate( &mVelocityChannelInstance[0], normDuration );
		value[1] = FxChannelInstance_Integrate( &mVelocityChannelInstance[1], normDuration );
		value[2] = FxChannelInstance_Integrate( &mVelocityChannelInstance[2], normDuration );
	}
	if ( mFlags & 0x200000 )
		VectorCopy( value, outVector );
	else
		AxisTransformComponents( mDisplayAxis, value[0], value[1], value[2], outVector );
	VectorScale( outVector, GetDuration() * 0.001f, outVector );
}

void Particle::IntegrateVelocity2( float normDuration, vec3_t outVector )
{
	vec3_t value;

	if ( mFlags & 0x80000 )
	{
		value[0] = FxChannelInstance_IntegrateBlend( &mVelocity2ChannelInstance[0], &mVelocity2RandChannelInstance[0], mVelocity2Weights[0], normDuration );
		value[1] = FxChannelInstance_IntegrateBlend( &mVelocity2ChannelInstance[1], &mVelocity2RandChannelInstance[1], mVelocity2Weights[1], normDuration );
		value[2] = FxChannelInstance_IntegrateBlend( &mVelocity2ChannelInstance[2], &mVelocity2RandChannelInstance[2], mVelocity2Weights[2], normDuration );
	}
	else
	{
		value[0] = FxChannelInstance_Integrate( &mVelocity2ChannelInstance[0], normDuration );
		value[1] = FxChannelInstance_Integrate( &mVelocity2ChannelInstance[1], normDuration );
		value[2] = FxChannelInstance_Integrate( &mVelocity2ChannelInstance[2], normDuration );
	}
	if ( mFlags & 0x400000 )
		VectorCopy( value, outVector );
	else
		AxisTransformComponents( mDisplayAxis, value[0], value[1], value[2], outVector );
	VectorScale( outVector, GetDuration() * 0.001f, outVector );
}

void Particle::IntegrateGravity( int duration, vec3_t outVector )
{
	outVector[0] = 0.0f;
	outVector[1] = 0.0f;
	outVector[2] = mGravity * duration * 0.001f;
}

void Particle::IntegrateTotalVelocity( int duration, vec3_t outVector )
{
	int life;
	float normDuration;
	vec3_t velocity;
	vec3_t velocity2;
	vec3_t gravity;

	life = GetDuration();
	normDuration = duration >= life ? 1.0f : (float)duration / life;
	IntegrateVelocity( normDuration, velocity );
	IntegrateVelocity2( normDuration, velocity2 );
	IntegrateGravity( duration, gravity );
	VectorScale( gravity, duration * 0.001f, gravity );
	VectorAdd( velocity, velocity2, outVector );
	VectorAdd( outVector, gravity, outVector );
}

void Particle::GetTotalVelocityAtTime0( vec3_t outVector )
{
	const orientation_t *orient;

	if ( mBolt.IsValid() )
	{
		orient = mBolt.Get()->GetOrientation();
		if ( !orient )
		{
			VectorClear( outVector );
			return;
		}
	}
	else
		orient = 0;
	GetTotalVelocity( 0.0f, outVector, orient );
}

void Particle::GetTotalVelocity( float normTime, vec3_t outVector, const orientation_t *or_ )
{
	vec3_t velocity;
	vec3_t velocity2;
	vec3_t gravity;

	CalcVelocityValue( normTime, velocity, or_ );
	CalcVelocity2Value( normTime, velocity2, or_ );
	CalcGravityValue( normTime, gravity, or_ );
	VectorAdd( velocity, velocity2, outVector );
	VectorAdd( outVector, gravity, outVector );
	VectorAdd( outVector, mImpactVelocity, outVector );
}

void Particle::UpdateSize()
{
	if ( mFlags & 0x8000 )
		mRefEnt.radius[0] = FxChannelInstance_Blend1d( &mSizeChannelInstance, &mSizeRandChannelInstance, mSizeBlendFactor, mNormTime );
	else
		mRefEnt.radius[0] = FxChannelInstance_GetValue1d( &mSizeChannelInstance, mNormTime );
}

void Particle::UpdateSize2()
{
	if ( mNonUniformScale )
	{
		if ( mFlags & 0x10000 )
			mRefEnt.radius[1] = FxChannelInstance_Blend1d( &mSize2ChannelInstance, &mSize2RandChannelInstance, mSize2BlendFactor, mNormTime );
		else
			mRefEnt.radius[1] = FxChannelInstance_GetValue1d( &mSize2ChannelInstance, mNormTime );
	}
	else
	{
		mRefEnt.radius[1] = mRefEnt.radius[0];
	}
}

void Particle::UpdateRGB()
{
	vec3_t rgb;

	if ( mFlags & 0x2000 )
		FxChannelInstance_Blend3d( &mColorChannelInstance, &mColorRandChannelInstance, mColorBlendFactor, rgb, mNormTime );
	else
		FxChannelInstance_GetValue3d( &mColorChannelInstance, rgb, mNormTime );
	FX_ColorToBytes( rgb, mRefEnt.materialRGBA );
}

void Particle::UpdateAlpha()
{
	float alpha;

	if ( mFlags & 0x4000 )
		alpha = FxChannelInstance_Blend1d( &mAlphaChannelInstance, &mAlphaRandChannelInstance, mAlphaBlendFactor, mNormTime );
	else
		alpha = FxChannelInstance_GetValue1d( &mAlphaChannelInstance, mNormTime );
	alpha = I_fclamp( alpha, 0.0f, 1.0f );
	if ( mFlags & 0x80 )
		mRefEnt.materialRGBA[3] = (unsigned char)( alpha * 255.0f );
	else
	{
		mRefEnt.materialRGBA[0] = (unsigned char)( mRefEnt.materialRGBA[0] * alpha );
		mRefEnt.materialRGBA[1] = (unsigned char)( mRefEnt.materialRGBA[1] * alpha );
		mRefEnt.materialRGBA[2] = (unsigned char)( mRefEnt.materialRGBA[2] * alpha );
		mRefEnt.materialRGBA[3] = 255;
	}
}

void Particle::UpdateRotation()
{
	float delta;

	if ( mFlags & 0x40000 )
		delta = FxChannelInstance_Blend1d( &mRotationDeltaChannelInstance, &mRotationDeltaRandChannelInstance, mRotationBlendFactor, mNormTime );
	else
		delta = FxChannelInstance_GetValue1d( &mRotationDeltaChannelInstance, mNormTime );
	mRefEnt.rotation += theFxHelper->mFrameTime * 0.01f * delta;
}

void Particle::UpdateSubimage()
{
	int count;

	count = theFxHelper->GetMaterialSubimageCount( mRefEnt.customMaterial );
	if ( count == 1 )
	{
		mRefEnt.materialSubimageIndex = 0;
		return;
	}
	mRefEnt.materialSubimageIndex = mStartFrame + (int)( ( theFxHelper->mTime - mTimeStart ) * mFrameRate );
	if ( mLoopMode == 0 )
	{
		if ( mRefEnt.materialSubimageIndex >= count )
			mRefEnt.materialSubimageIndex %= count;
	}
	else if ( mLoopMode == 1 )
	{
		if ( mLoopTimes > 0 )
		{
			if ( mRefEnt.materialSubimageIndex >= count )
			{
				if ( mRefEnt.materialSubimageIndex >= ( mLoopTimes + 1 ) * count )
				{
					mLoopTimes = 0;
					mRefEnt.materialSubimageIndex = count - 1;
				}
				else
					mRefEnt.materialSubimageIndex %= count;
			}
		}
		else if ( mRefEnt.materialSubimageIndex >= count )
			mRefEnt.materialSubimageIndex = count - 1;
	}
}

unsigned char Particle::TypeID()
{
	return 1;
}

void Particle::Archive( FxArchive *arch )
{
	Effect::Archive( arch );
	arch->ArchiveVec3( mImpactVelocity );
	arch->ArchiveVec3( mDisplayAxis[0] );
	arch->ArchiveVec3( mDisplayAxis[1] );
	arch->ArchiveVec3( mDisplayAxis[2] );
	arch->ArchiveFloat( &mElasticity );
	arch->ArchiveByte( (unsigned char *)&mNonUniformScale );
	arch->ArchiveChannelInstance( &mColorChannelInstance );
	arch->ArchiveChannelInstance( &mColorRandChannelInstance );
	arch->ArchiveChannelInstance( &mAlphaChannelInstance );
	arch->ArchiveChannelInstance( &mAlphaRandChannelInstance );
	arch->ArchiveChannelInstance( &mSizeChannelInstance );
	arch->ArchiveChannelInstance( &mSizeRandChannelInstance );
	arch->ArchiveChannelInstance( &mSize2ChannelInstance );
	arch->ArchiveChannelInstance( &mSize2RandChannelInstance );
	arch->ArchiveChannelInstance( &mRotationDeltaChannelInstance );
	arch->ArchiveChannelInstance( &mRotationDeltaRandChannelInstance );
	arch->ArchiveChannelInstance( &mVelocityChannelInstance[0] );
	arch->ArchiveChannelInstance( &mVelocityChannelInstance[1] );
	arch->ArchiveChannelInstance( &mVelocityChannelInstance[2] );
	arch->ArchiveChannelInstance( &mVelocityRandChannelInstance[0] );
	arch->ArchiveChannelInstance( &mVelocityRandChannelInstance[1] );
	arch->ArchiveChannelInstance( &mVelocityRandChannelInstance[2] );
	arch->ArchiveChannelInstance( &mVelocity2ChannelInstance[0] );
	arch->ArchiveChannelInstance( &mVelocity2ChannelInstance[1] );
	arch->ArchiveChannelInstance( &mVelocity2ChannelInstance[2] );
	arch->ArchiveChannelInstance( &mVelocity2RandChannelInstance[0] );
	arch->ArchiveChannelInstance( &mVelocity2RandChannelInstance[1] );
	arch->ArchiveChannelInstance( &mVelocity2RandChannelInstance[2] );
	arch->ArchiveInt( &mStartFrame );
	arch->ArchiveFloat( &mFrameRate );
	arch->ArchiveInt( &mLoopMode );
	arch->ArchiveInt( &mLoopTimes );
	arch->ArchiveFloat( &mColorBlendFactor );
	arch->ArchiveFloat( &mAlphaBlendFactor );
	arch->ArchiveFloat( &mSizeBlendFactor );
	arch->ArchiveFloat( &mSize2BlendFactor );
	arch->ArchiveFloat( &mRotationBlendFactor );
	arch->ArchiveVec3( mVelocityWeights );
	arch->ArchiveVec3( mVelocity2Weights );
}

void Particle::FixupArchiveLoad( const PrimitiveTemplate *primTemplate )
{
	Effect::FixupArchiveLoad( primTemplate );
	mColorChannelInstance.curveIterator.master = primTemplate->mFxChannels[FX_CHANNEL_COLOR].curve;
	mColorRandChannelInstance.curveIterator.master = primTemplate->mFxChannels[FX_CHANNEL_COLOR_RAND].curve;
	mAlphaChannelInstance.curveIterator.master = primTemplate->mFxChannels[FX_CHANNEL_ALPHA].curve;
	mAlphaRandChannelInstance.curveIterator.master = primTemplate->mFxChannels[FX_CHANNEL_ALPHA_RAND].curve;
	mSizeChannelInstance.curveIterator.master = primTemplate->mFxChannels[FX_CHANNEL_SIZE].curve;
	mSizeRandChannelInstance.curveIterator.master = primTemplate->mFxChannels[FX_CHANNEL_SIZE_RAND].curve;
	mSize2ChannelInstance.curveIterator.master = primTemplate->mFxChannels[FX_CHANNEL_SIZE2].curve;
	mSize2RandChannelInstance.curveIterator.master = primTemplate->mFxChannels[FX_CHANNEL_SIZE2_RAND].curve;
	mRotationDeltaChannelInstance.curveIterator.master = primTemplate->mFxChannels[FX_CHANNEL_ROTATION_DELTA].curve;
	mRotationDeltaRandChannelInstance.curveIterator.master = primTemplate->mFxChannels[FX_CHANNEL_ROTATION_DELTA_RAND].curve;
	mVelocityChannelInstance[0].curveIterator.master = primTemplate->mFxChannels[FX_CHANNEL_VELOCITY_X].curve;
	mVelocityChannelInstance[1].curveIterator.master = primTemplate->mFxChannels[FX_CHANNEL_VELOCITY_Y].curve;
	mVelocityChannelInstance[2].curveIterator.master = primTemplate->mFxChannels[FX_CHANNEL_VELOCITY_Z].curve;
	mVelocityRandChannelInstance[0].curveIterator.master = primTemplate->mFxChannels[FX_CHANNEL_VELOCITY_X_RAND].curve;
	mVelocityRandChannelInstance[1].curveIterator.master = primTemplate->mFxChannels[FX_CHANNEL_VELOCITY_Y_RAND].curve;
	mVelocityRandChannelInstance[2].curveIterator.master = primTemplate->mFxChannels[FX_CHANNEL_VELOCITY_Z_RAND].curve;
	mVelocity2ChannelInstance[0].curveIterator.master = primTemplate->mFxChannels[FX_CHANNEL_VELOCITY2_X].curve;
	mVelocity2ChannelInstance[1].curveIterator.master = primTemplate->mFxChannels[FX_CHANNEL_VELOCITY2_Y].curve;
	mVelocity2ChannelInstance[2].curveIterator.master = primTemplate->mFxChannels[FX_CHANNEL_VELOCITY2_Z].curve;
	mVelocity2RandChannelInstance[0].curveIterator.master = primTemplate->mFxChannels[FX_CHANNEL_VELOCITY2_X_RAND].curve;
	mVelocity2RandChannelInstance[1].curveIterator.master = primTemplate->mFxChannels[FX_CHANNEL_VELOCITY2_Y_RAND].curve;
	mVelocity2RandChannelInstance[2].curveIterator.master = primTemplate->mFxChannels[FX_CHANNEL_VELOCITY2_Z_RAND].curve;
}

OrientedParticle::OrientedParticle()
{
}

OrientedParticle::~OrientedParticle()
{
}

bool OrientedParticle::Cull()
{
	return theFxHelper->CullSphere( mRefEnt.origin, mRefEnt.radius[0], GetCullPlaneCount() );
}

void OrientedParticle::Draw()
{
	FX_AddFxToScene( this, 7 );
}

bool OrientedParticle::Update()
{
	const orientation_t *orient;

	orient = 0;
	if ( !Effect::Update() )
		return false;
	if ( !GetBoltOrientation( &orient ) )
		return false;
	if ( !UpdateOrigin( orient ) )
		return false;
	UpdateSize();
	UpdateSize2();
	UpdateRGB();
	UpdateAlpha();
	UpdateRotation();
	UpdateSubimage();
	if ( orient )
	{
		OrientationPosToWorld( orient, mOrigin, mRefEnt.origin );
		OrientationDirToWorld( orient, mNormal, mRefEnt.axis[0] );
	}
	else
	{
		VectorCopy( mOrigin, mRefEnt.origin );
		VectorCopy( mNormal, mRefEnt.axis[0] );
	}
	return true;
}

unsigned char OrientedParticle::TypeID()
{
	return 7;
}

void OrientedParticle::Archive( FxArchive *arch )
{
	Particle::Archive( arch );
	arch->ArchiveVec3( mNormal );
}

Cloud::Cloud()
{
	RandomizeAxis();
}

Cloud::~Cloud()
{
}

void Cloud::Die()
{
}

bool Cloud::Cull()
{
	return theFxHelper->CullSphere( mRefEnt.origin, mRefEnt.scale + I_fmax( mRefEnt.radius[0], mRefEnt.radius[1] ), GetCullPlaneCount() );
}

void Cloud::RandomizeAxis()
{
	float lengthSq;
	int attempt;
	int i;

	for ( attempt = 0; attempt != 4; attempt++ )
	{
		for ( i = 0; i != 3; i++ )
			mRotationAxis[i] = flrand( -1.0f, 1.0f );
		lengthSq = VectorLengthSquared( mRotationAxis );
		if ( lengthSq >= 0.01f && lengthSq <= 1.0f )
		{
			Vec3Normalize( mRotationAxis );
			return;
		}
	}
	mRotationAxis[0] = 1.0f;
	mRotationAxis[1] = 0.0f;
	mRotationAxis[2] = 0.0f;
}

void Cloud::Draw()
{
	FX_AddFxToScene( this, 6 );
}

void Cloud::CreateChannelInstances( const PrimitiveTemplate *primTemp )
{
	Particle::CreateChannelInstances( primTemp );
	FxChannelInstance_Create( &primTemp->mFxChannels[FX_CHANNEL_LENGTH], &mLengthChannelInstance );
	FxChannelInstance_Create( &primTemp->mFxChannels[FX_CHANNEL_LENGTH_RAND], &mLengthRandChannelInstance );
}

void Cloud::UpdateLength()
{
	if ( !mUseLength )
		return;
	{
		if ( mFlags & 0x20000 )
			mLength = FxChannelInstance_Blend1d( &mLengthChannelInstance, &mLengthRandChannelInstance, mLengthBlendFactor, mNormTime );
		else
			mLength = FxChannelInstance_GetValue1d( &mLengthChannelInstance, mNormTime );
	}
}

bool Cloud::Update()
{
	const orientation_t *orient;
	vec4_t quat;
	vec3_t prevOrigin;

	orient = 0;
	if ( !Effect::Update() )
		return false;
	if ( !GetBoltOrientation( &orient ) )
		return false;
	VectorCopy( mOrigin, prevOrigin );
	if ( !UpdateOrigin( orient ) )
		return false;
	UpdateSize();
	UpdateSize2();
	UpdateLength();
	UpdateRGB();
	UpdateAlpha();
	UpdateRotation();
	QuatFromAxisAngle( mRefEnt.rotation, mRotationAxis, quat );
	QuatToAxis( quat, mRefEnt.axis );
	mRefEnt.scale = mRefEnt.radius[0];
	mRefEnt.radius[0] = mRefEnt.radius[1];
	mRefEnt.radius[1] = mUseLength ? mLength : mRefEnt.radius[0];
	if ( orient )
		OrientationPosToWorld( orient, mOrigin, mRefEnt.origin );
	else
		VectorCopy( mOrigin, mRefEnt.origin );
	VectorCopy( prevOrigin, mRefEnt.endpos );
	return true;
}

unsigned char Cloud::TypeID()
{
	return 12;
}

void Cloud::Archive( FxArchive *arch )
{
	Particle::Archive( arch );
	arch->ArchiveVec3( mRotationAxis );
	arch->ArchiveChannelInstance( &mLengthChannelInstance );
	arch->ArchiveChannelInstance( &mLengthRandChannelInstance );
	arch->ArchiveFloat( &mLengthBlendFactor );
	arch->ArchiveByte( (unsigned char *)&mUseLength );
}

void Cloud::FixupArchiveLoad( const PrimitiveTemplate *primTemplate )
{
	Particle::FixupArchiveLoad( primTemplate );
	mLengthChannelInstance.curveIterator.master = primTemplate->mFxChannels[FX_CHANNEL_LENGTH].curve;
	mLengthRandChannelInstance.curveIterator.master = primTemplate->mFxChannels[FX_CHANNEL_LENGTH_RAND].curve;
}

Line::Line()
{
}

Line::~Line()
{
}

void Line::Die()
{
}

bool Line::Cull()
{
	return theFxHelper->CullCylinder( mRefEnt.origin, mRefEnt.endpos, mRefEnt.radius[0], mRefEnt.radius[0], GetCullPlaneCount() );
}

void Line::Draw()
{
	FX_AddFxToScene( this, 8 );
}

bool Line::Update()
{
	const orientation_t *orient;

	orient = 0;
	if ( !Effect::Update() )
		return false;
	if ( !GetBoltOrientation( &orient ) )
		return false;
	if ( orient )
	{
		OrientationPosToWorld( orient, mOrigin, mRefEnt.origin );
		OrientationPosToWorld( orient, mEndpoint, mRefEnt.endpos );
	}
	else
	{
		VectorCopy( mOrigin, mRefEnt.origin );
		VectorCopy( mEndpoint, mRefEnt.endpos );
	}
	UpdateSize();
	UpdateRGB();
	UpdateAlpha();
	return true;
}

unsigned char Line::TypeID()
{
	return 2;
}

void Line::Archive( FxArchive *arch )
{
	Particle::Archive( arch );
	arch->ArchiveVec3( mEndpoint );
}

Tail::Tail()
{
}

Tail::~Tail()
{
}

void Tail::InitEndPoint()
{
	UpdateLength();
	CalcNewEndpoint( 0 );
}

bool Tail::Cull()
{
	return theFxHelper->CullCylinder( mRefEnt.origin, mRefEnt.endpos, mRefEnt.radius[0], mRefEnt.radius[0], GetCullPlaneCount() );
}

void Tail::CreateChannelInstances( const PrimitiveTemplate *primTemp )
{
	Particle::CreateChannelInstances( primTemp );
	FxChannelInstance_Create( &primTemp->mFxChannels[FX_CHANNEL_LENGTH], &mLengthChannelInstance );
	FxChannelInstance_Create( &primTemp->mFxChannels[FX_CHANNEL_LENGTH_RAND], &mLengthRandChannelInstance );
}

void Tail::Draw()
{
	FX_AddFxToScene( this, 8 );
}

bool Tail::Update()
{
	const orientation_t *orient;

	orient = 0;
	if ( !Effect::Update() )
		return false;
	if ( !GetBoltOrientation( &orient ) )
		return false;
	VectorCopy( mOrigin, mEndpoint );
	if ( !UpdateOrigin( orient ) )
		return false;
	if ( orient )
		OrientationPosToWorld( orient, mOrigin, mRefEnt.origin );
	else
		VectorCopy( mOrigin, mRefEnt.origin );
	UpdateSize();
	UpdateLength();
	UpdateRGB();
	UpdateAlpha();
	UpdateSubimage();
	CalcNewEndpoint( orient );
	return true;
}

void Tail::UpdateLength()
{
	if ( mFlags & 0x20000 )
		mLength = FxChannelInstance_Blend1d( &mLengthChannelInstance, &mLengthRandChannelInstance, mLengthBlendFactor, mNormTime );
	else
		mLength = FxChannelInstance_GetValue1d( &mLengthChannelInstance, mNormTime );
}

void Tail::CalcNewEndpoint( const orientation_t *or_ )
{
	float len;
	vec3_t dir;

	VectorSubtract( mEndpoint, mOrigin, dir );
	len = VectorLength( dir );
	if ( len > 0.0f )
	{
		VectorScale( dir, 1.0f / len, dir );
		if ( or_ )
		{
			VectorMA( mOrigin, mLength, dir, dir );
			OrientationPosToWorld( or_, dir, mRefEnt.endpos );
		}
		else
			VectorMA( mOrigin, mLength, dir, mRefEnt.endpos );
	}
}

unsigned char Tail::TypeID()
{
	return 3;
}

void Tail::Archive( FxArchive *arch )
{
	Particle::Archive( arch );
	arch->ArchiveVec3( mEndpoint );
	arch->ArchiveChannelInstance( &mLengthChannelInstance );
	arch->ArchiveChannelInstance( &mLengthRandChannelInstance );
	arch->ArchiveFloat( &mLengthBlendFactor );
}

void Tail::FixupArchiveLoad( const PrimitiveTemplate *primTemplate )
{
	Particle::FixupArchiveLoad( primTemplate );
	mLengthChannelInstance.curveIterator.master = primTemplate->mFxChannels[FX_CHANNEL_LENGTH].curve;
	mLengthRandChannelInstance.curveIterator.master = primTemplate->mFxChannels[FX_CHANNEL_LENGTH_RAND].curve;
}

Cylinder::Cylinder()
{
}

Cylinder::~Cylinder()
{
}

bool Cylinder::Cull()
{
	return theFxHelper->CullCylinder( mRefEnt.origin, mRefEnt.endpos, mRefEnt.radius[1], mRefEnt.radius[0], GetCullPlaneCount() );
}

void Cylinder::Draw()
{
	FX_AddFxToScene( this, 9 );
}

bool Cylinder::Update()
{
	const orientation_t *orient;
	vec3_t end;

	orient = 0;
	if ( !Effect::Update() )
		return false;
	if ( !GetBoltOrientation( &orient ) )
		return false;
	UpdateSize();
	UpdateSize2();
	UpdateLength();
	UpdateRGB();
	UpdateAlpha();
	if ( orient )
	{
		OrientationPosToWorld( orient, mOrigin, mRefEnt.origin );
		VectorMA( mOrigin, mLength, mRefEnt.axis[0], end );
		OrientationPosToWorld( orient, end, mRefEnt.endpos );
	}
	else
	{
		VectorCopy( mOrigin, mRefEnt.origin );
		VectorMA( mOrigin, mLength, mRefEnt.axis[0], mRefEnt.endpos );
	}
	return true;
}

unsigned char Cylinder::TypeID()
{
	return 4;
}

void Cylinder::Archive( FxArchive *arch )
{
	Tail::Archive( arch );
}

Emitter::Emitter()
{
}

Emitter::~Emitter()
{
}

bool Emitter::Cull()
{
	return false;
}

void Emitter::Draw()
{
	if ( ( mFlags & 0x10 ) && mRefEnt.scale != 0.0f )
		FX_AddFxToScene( this, 1 );
}

bool Emitter::Update()
{
	const orientation_t *orient;
	vec3_t prevOrigin;
	vec3_t bindVelocity;
	vec3_t localAxis[3];
	int i;

	orient = 0;
	if ( !Effect::Update() )
		return false;
	if ( !GetBoltOrientation( &orient ) )
		return false;
	VectorCopy( mOrigin, prevOrigin );
	if ( !UpdateOrigin( orient ) )
		return false;
	if ( VectorCompare( prevOrigin, mOrigin ) )
		VectorScale( mAngleDelta, 0.7f, mAngleDelta );
	UpdateAngles();
	UpdateSize();
	if ( orient )
		VectorClear( bindVelocity );
	if ( mFlags & 0x10 )
	{
		if ( orient )
		{
			OrientationPosToWorld( orient, mOrigin, mRefEnt.origin );
			for ( i = 0; i <= 2; i++ )
			{
				VectorCopy( mRefEnt.axis[i], localAxis[i] );
				OrientationDirToWorld( orient, localAxis[i], mRefEnt.axis[i] );
			}
			mRefEnt.scale = mRefEnt.radius[0];
			if ( mEmitTime < theFxHelper->mTime )
			{
				VectorSubtract( orient->origin, mBoltOffset, bindVelocity );
				VectorScale( bindVelocity, ( theFxHelper->mTime - mEmitTime ) * 0.001f, bindVelocity );
			}
		}
		else
		{
			VectorCopy( mOrigin, mRefEnt.origin );
			mRefEnt.scale = mRefEnt.radius[0];
		}
	}
	UpdateEmitFx( bindVelocity, orient );
	return true;
}

// Spawns the emitted effect at even spacing along the path travelled this frame.
void Emitter::UpdateEmitFx( const vec3_t bindVelocity, const orientation_t *or_ )
{
	vec3_t axis[3];
	vec3_t emitPos;
	vec3_t basePos;
	vec3_t nextVelocity;
	vec3_t velocity;
	float dt;
	float emitDistSq;
	int i;
	int time;
	int msec;
	float elapsed;
	float age;
	float life;
	float normTime;
	float len;
	vec3_t worldPos;
	float speedSq;
	float step;
	float stepSq;
	float denom;
	float numer;
	int backup;

	if ( !( mFlags & 0x100 ) )
		return;
	if ( !theFxHelper->mFrameTime )
		return;
	emitDistSq = mEmitDist * mEmitDist;
	msec = 0;
	elapsed = 0.0f;
	time = mEmitTime;
	age = GetAge() * 0.001f;
	life = GetDuration() * 0.001f;
	while ( time < theFxHelper->mTime )
	{
		msec += 12;
		dt = msec * 0.001f;
		VectorAdd( mEmitOrigin, mBoltOffset, basePos );
		normTime = GetNormTime( age + elapsed, life );
		GetTotalVelocity( normTime, velocity, or_ );
		VectorMA( mEmitOrigin, dt, velocity, emitPos );
		if ( or_ )
		{
			for ( i = 0; i <= 2; i++ )
				emitPos[i] += dt * bindVelocity[i];
		}
		if ( emitDistSq > Vec3DistanceSq( emitPos, basePos ) )
		{
			time += 12;
			continue;
		}
		if ( or_ )
		{
			OrientationPosToWorld( or_, emitPos, worldPos );
			theFxScheduler->PlayEffect( mEmitFx, worldPos, 0, &mBolt.Get()->mBolt );
		}
		else
		{
			len = VectorLength( velocity );
			if ( len > 0.0f )
			{
				VectorScale( velocity, 1.0f / len, axis[0] );
				MakeNormalVectors( axis[0], axis[1], axis[2] );
			}
			else
			{
				VectorCopy( mRefEnt.axis[0], axis[0] );
				VectorCopy( mRefEnt.axis[1], axis[1] );
				VectorCopy( mRefEnt.axis[2], axis[2] );
			}
			theFxScheduler->PlayEffect( mEmitFx, emitPos, axis, 0 );
		}
		speedSq = DotProduct( velocity, velocity );
		step = msec * 0.001f;
		stepSq = step * step;
		denom = 2.0f * speedSq * step;
		if ( denom != 0.0f )
		{
			numer = speedSq * stepSq - emitDistSq;
			backup = I_fround( numer / denom * 1000.0f );
			msec -= backup;
			if ( msec < time - mEmitTime )
				msec = time - mEmitTime;
			dt = msec * 0.001f;
			normTime = GetNormTime( age + elapsed + dt, life );
			GetTotalVelocity( normTime, velocity, or_ );
			VectorMA( mEmitOrigin, dt, velocity, emitPos );
		}
		normTime = GetNormTime( age + elapsed + dt, life );
		GetTotalVelocity( normTime, nextVelocity, or_ );
		VectorCopy( emitPos, mEmitOrigin );
		VectorCopy( nextVelocity, mEmitVelocity );
		if ( or_ )
			VectorMA( mBoltOffset, msec * 0.001f, bindVelocity, mBoltOffset );
		mEmitTime += msec;
		elapsed += dt;
		msec = 0;
		time = mEmitTime;
		RandomizeEmitDist();
		emitDistSq = mEmitDist * mEmitDist;
	}
}

void Emitter::UpdateAngles()
{
	VectorMA( mAngles, theFxHelper->mFrameTime * 0.01f, mAngleDelta, mAngles );
	AnglesToAxis( mAngles, mRefEnt.axis );
}

unsigned char Emitter::TypeID()
{
	return 5;
}

void Emitter::Archive( FxArchive *arch )
{
	Particle::Archive( arch );
	arch->ArchiveVec3( mEmitOrigin );
	arch->ArchiveVec3( mEmitVelocity );
	arch->ArchiveVec3( mBoltOffset );
	arch->ArchiveInt( &mEmitTime );
	arch->ArchiveFloat( &mEmitDist );
	arch->ArchiveVec3( mAngles );
	arch->ArchiveVec3( mAngleDelta );
	arch->ArchiveEffect( &mEmitFx );
	arch->ArchiveFloat( &mEmitDistBase );
	arch->ArchiveFloat( &mEmitDistRand );
	if ( !mModel )
		mFlags &= ~0x10;
}

Light::Light()
{
}

Light::~Light()
{
}

void Light::CreateChannelInstances( const PrimitiveTemplate *primTemp )
{
	FxChannelInstance_Create( &primTemp->mFxChannels[FX_CHANNEL_COLOR], &mColorChannelInstance );
	FxChannelInstance_Create( &primTemp->mFxChannels[FX_CHANNEL_COLOR_RAND], &mColorRandChannelInstance );
	FxChannelInstance_Create( &primTemp->mFxChannels[FX_CHANNEL_SIZE], &mSizeChannelInstance );
	FxChannelInstance_Create( &primTemp->mFxChannels[FX_CHANNEL_SIZE_RAND], &mSizeRandChannelInstance );
}

bool Light::Cull()
{
	return theFxHelper->CullSphere( mRefEnt.origin, mRefEnt.radius[0], GetCullPlaneCount() );
}

void Light::Draw()
{
	theFxHelper->AddLightToScene( mRefEnt.origin, mRefEnt.radius[0], mRefEnt.dlightColor[0], mRefEnt.dlightColor[1], mRefEnt.dlightColor[2] );
}

bool Light::Update()
{
	const orientation_t *orient;

	orient = 0;
	if ( !Effect::Update() )
		return false;
	if ( !GetBoltOrientation( &orient ) )
		return false;
	UpdateSize();
	UpdateRGB();
	if ( orient )
		OrientationPosToWorld( orient, mOrigin, mRefEnt.origin );
	else
		VectorCopy( mOrigin, mRefEnt.origin );
	return true;
}

void Light::UpdateSize()
{
	if ( mFlags & 0x8000 )
		mRefEnt.radius[0] = FxChannelInstance_Blend1d( &mSizeChannelInstance, &mSizeRandChannelInstance, mSizeBlendFactor, mNormTime );
	else
		mRefEnt.radius[0] = FxChannelInstance_GetValue1d( &mSizeChannelInstance, mNormTime );
}

void Light::UpdateRGB()
{
	if ( mFlags & 0x2000 )
		FxChannelInstance_Blend3d( &mColorChannelInstance, &mColorRandChannelInstance, mColorBlendFactor, mRefEnt.dlightColor, mNormTime );
	else
		FxChannelInstance_GetValue3d( &mColorChannelInstance, mRefEnt.dlightColor, mNormTime );
}

unsigned char Light::TypeID()
{
	return 9;
}

void Light::Archive( FxArchive *arch )
{
	Effect::Archive( arch );
	arch->ArchiveChannelInstance( &mColorChannelInstance );
	arch->ArchiveChannelInstance( &mColorRandChannelInstance );
	arch->ArchiveChannelInstance( &mSizeChannelInstance );
	arch->ArchiveChannelInstance( &mSizeRandChannelInstance );
	arch->ArchiveFloat( &mColorBlendFactor );
	arch->ArchiveFloat( &mSizeBlendFactor );
}

void Light::FixupArchiveLoad( const PrimitiveTemplate *primTemplate )
{
	Effect::FixupArchiveLoad( primTemplate );
	mColorChannelInstance.curveIterator.master = primTemplate->mFxChannels[FX_CHANNEL_COLOR].curve;
	mColorRandChannelInstance.curveIterator.master = primTemplate->mFxChannels[FX_CHANNEL_COLOR_RAND].curve;
	mSizeChannelInstance.curveIterator.master = primTemplate->mFxChannels[FX_CHANNEL_SIZE].curve;
	mSizeRandChannelInstance.curveIterator.master = primTemplate->mFxChannels[FX_CHANNEL_SIZE_RAND].curve;
}

bool Flash::Update()
{
	if ( !Effect::Update() )
		return false;
	UpdateRGB();
	return true;
}

// Fades the flash by distance from the camera and by how far off-axis it is.
void Flash::Init()
{
	vec3_t dir;
	float dot = 1.0f;
	float dist;

	VectorSubtract( mOrigin, theFxHelper->mCamera.vieworg, dir );
	dist = Vec3Normalize( dir );
	dot = DotProduct( dir, theFxHelper->mCamera.frustum[0] );
	if ( dist > 600.0f || ( dot < 0.5f && dist > 100.0f ) )
		dot = 0.0f;
	else if ( dot < 0.5f && dist <= 100.0f )
		dot += 1.1f;
	dot = ( 1.0f - dist * dist / 360000.0f ) * dot;
	mColorChannelInstance.scale *= dot;
}

void Flash::Draw()
{
	int i;
	vec4_t rgba;
	FxHelper *helper;

	for ( i = 0; i <= 2; i++ )
		rgba[i] = ClampFloat( mRefEnt.dlightColor[i], 0.0f, 1.0f );
	rgba[3] = 1.0f;
	Byte4PackRgba( rgba, mRefEnt.materialRGBA );
	helper = theFxHelper;
	VectorCopy( helper->mCamera.vieworg, mRefEnt.origin );
	VectorMA( mRefEnt.origin, 8.0f, helper->mCamera.frustum[0], mRefEnt.origin );
	mRefEnt.radius[0] = 12.0f;
	mRefEnt.radius[1] = mRefEnt.radius[0];
	FX_AddFxToScene( this, 4 );
}

unsigned char Flash::TypeID()
{
	return 11;
}

void Flash::Archive( FxArchive *arch )
{
	Light::Archive( arch );
}
