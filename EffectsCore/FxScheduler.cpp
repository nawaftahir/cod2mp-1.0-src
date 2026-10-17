#include <string.h>
#include "com_math.h"

struct orientation_t
{
	vec3_t origin;
	vec3_t axis[3];
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
	FX_CHANNEL_COUNT = 24
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
	FxEffectDef *GetEffect() const;
	TMediaElement GetHandle() const;

	TMediaList mMediaList;
};

struct FxBoltInfo
{
	int dobjHandle;
	int boneIndex;
};

#include "FxCamera.h"

struct trace_t;
#include "FxHelper.h"

extern FxHelper *theFxHelper;

// Every pooled type is carved from 32 KB blocks of the shared effects arena.
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
void FxMemMgr<T>::MoveToFront( FxMemBlock *block )
{
	FxMem_UnlinkBlock( block );
	block->prev = 0;
	block->next = mHead;
	mHead->prev = block;
	mHead = block;
}

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

struct PrimitiveTemplate
{
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
	int mSequenceStartFrameMode;
	int mSequenceFixedFrameValue;
	int mSequencePlayRateMode;
	float mSequenceFixedFpsValue;
	int mSequenceLoopMode;
	int mSequenceLoopTimes;
	float spawnFrustumCullRadius;

	static FxMemMgr<PrimitiveTemplate> gmMemMgr;
};

struct FxEffectDef
{
	const char *mEffectName;
	int mPrimitiveCount;
	PrimitiveTemplate *mPrimitives[24];
};

void FxCurveIterator_Create( FxCurveIterator *createe, const FxCurve *master );
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
	float to;
	float from;

	from = FxCurveIterator_Interpolate1d( &inst->curveIterator, t );
	to = FxCurveIterator_Interpolate1d( &randInst->curveIterator, t );
	from = from + ( to - from ) * blend;
	return from * inst->scale;
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

class FxBoltFrame;

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

private:
	FxBoltFrame *mFrame;
};

// A bone orientation shared by every effect bolted to it, refreshed once per frame.
class FxBoltFrame
{
public:
	FxBoltFrame *AddRef()
	{
		refCount++;
		return this;
	}

	static FxBoltFramePtr Acquire( const FxBoltInfo &bolt );
	void Release();
	const orientation_t *GetOrientation();

	int refCount;
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

// One primitive of an effect about to be spawned.
struct EffectPrimitive
{
	const FxEffectDef *fx;
	const PrimitiveTemplate *primTemp;
	FxBoltFramePtr boltFrame;
};

class FxArchive
{
private:
	void ReadData( void *p, int byteCount );
	void WriteData( const void *p, int byteCount );

public:
	void ArchiveData( void *p, int byteCount );
	void ArchiveEffect( const FxEffectDef **fx );

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

	void ArchiveInt( int *value )
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

// An effect primitive whose spawn delay has not yet elapsed.
struct ScheduledEffect
{
	static void *operator new( unsigned int size )
	{
		return s_memMgr.Alloc( size );
	}

	static void operator delete( void *p )
	{
		s_memMgr.Free( p );
	}

	void Archive( FxArchive *arch );

	const FxEffectDef *mFx;
	int mPrimIndex;
	int mStartTime;
	FxBoltInfo mBolt;
	vec3_t mOrigin;
	vec3_t mAxis[3];
	int mSeed;
	int mIndexInBatch;
	ScheduledEffect *mScheduledNext;

	static FxMemMgr<ScheduledEffect> s_memMgr;
};

struct FxScheduler
{
	FxScheduler();
	void Clean( bool bRemoveTemplates, const FxEffectDef *fxToPreserve );
	void PlayEffect( const FxEffectDef *fx, const vec3_t origin );
	void PlayEffect( const FxEffectDef *fx, const vec3_t origin, const vec3_t dir );
	void PlayEffect( const FxEffectDef *fx, const vec3_t origin, const vec3_t forward, const vec3_t up );
	void PlayEffect( const FxEffectDef *fx, const vec3_t origin, const vec3_t axis[3], const FxBoltInfo *bolt );
	float GetEffectLength( const FxEffectDef *fx );
	void GetDecalColor( const PrimitiveTemplate *primTemp, vec3_t rgb );
	float GetDecalAlpha( const PrimitiveTemplate *primTemp );
	float GetDecalSize( const PrimitiveTemplate *primTemp );
	void CreateDecalEffect( const PrimitiveTemplate *primTemp, const vec3_t origin, const vec3_t axis[3] );
	void CreateEffect( const FxEffectDef *fx, const PrimitiveTemplate *primTemp, const FxBoltInfo *bolt, const vec3_t origin, const vec3_t axis[3], int lateTime, int indexInBatch );
	void Archive( FxArchive *arch );

	int mSeed;
	ScheduledEffect *mScheduledHead;
	int mScheduledCount;
};

extern FxScheduler *theFxScheduler;

// Only the leading fields are read here.
struct dvar_s
{
	const char *name;
	unsigned short flags;
	unsigned char type;
	bool modified;
	union
	{
		bool boolean;
		int integer;
		float decimal;
	} current;
};
typedef dvar_s dvar_t;

extern dvar_t *fx_enable;
extern dvar_t *fx_freeze;
extern const dvar_t *fx_count;

void FX_Print( const char *fmt, ... );
FxEffectDef *FX_RegisterEffect( const char *name );
FxEffectDef *FX_ParseEffect( class GenericParser2 *parser, const char *name );
void FX_CleanTemplate( FxEffectDef *fx );
bool FX_GetBoneOrientation( const FxBoltInfo *bolt, orientation_t *orient );
void FxChannelInstance_Create( const FxChannel *master, FxChannelInstance *createe );
float FxRange_GetVal( const FxRange *range );
float flrand( float min, float max );
float Vec3DistanceSq( const vec3_t p1, const vec3_t p2 );
void AxisCopy( const vec3_t in[3], vec3_t out[3] );
void MakeNormalVectors( const vec3_t forward, vec3_t right, vec3_t up );
void RotatePointAroundVector( vec3_t dst, const vec3_t dir, const vec3_t point, float degrees );

void FX_AddParticle( EffectPrimitive *prim, vec3_t ax[3], const vec3_t origin, int lateTime, int indexInBatch );
void FX_AddLine( EffectPrimitive *prim, vec3_t ax[3], const vec3_t origin, int lateTime, int indexInBatch );
void FX_AddTail( EffectPrimitive *prim, vec3_t ax[3], const vec3_t origin, int lateTime, int indexInBatch );
void FX_AddCylinder( EffectPrimitive *prim, vec3_t ax[3], const vec3_t origin, int lateTime, int indexInBatch );
void FX_AddEmitter( EffectPrimitive *prim, vec3_t ax[3], const vec3_t origin, int lateTime, int indexInBatch );
void FX_AddDecal( EffectPrimitive *prim, vec3_t ax[3], const vec3_t origin, int lateTime, int indexInBatch );
void FX_AddOrientedParticle( EffectPrimitive *prim, vec3_t ax[3], const vec3_t origin, int lateTime, int indexInBatch );
void FX_AddFxRunner( EffectPrimitive *prim, vec3_t ax[3], const vec3_t origin, int lateTime, int indexInBatch );
void FX_AddLight( EffectPrimitive *prim, vec3_t ax[3], const vec3_t origin, int lateTime, int indexInBatch );
void FX_AddCameraShake( EffectPrimitive *prim, vec3_t ax[3], const vec3_t origin, int lateTime, int indexInBatch );
void FX_AddFlash( EffectPrimitive *prim, vec3_t ax[3], const vec3_t origin, int lateTime, int indexInBatch );
void FX_AddCloud( EffectPrimitive *prim, vec3_t ax[3], const vec3_t origin, int lateTime, int indexInBatch );

class GenericParser2
{
public:
	GenericParser2();
	~GenericParser2();

private:
	char mGroup[48];
	void *mTextPoolList;
	bool mWriteable;
};

FxMemMgr<PrimitiveTemplate> PrimitiveTemplate::gmMemMgr;
static FxEffectDef *effectTemplateArray[256];
static int effectTemplateArrayCount;
FxMemMgr<ScheduledEffect> ScheduledEffect::s_memMgr;
FxScheduler *theFxSchedulers[2];
FxScheduler *theFxScheduler = NULL;
FxEffectDef *g_fxDefaultEffect;
// Unreferenced storage; original declarations unknown (sized from the layout).
static int unusedStorage[3];

static void FX_FreeTemplates( const FxEffectDef *fxToPreserve );

// Empty in the dedicated server; original name unknown.
void FX_SchedulerUnused()
{
}

FxEffectDef *MediaHandles::GetEffect() const
{
	return GetHandle().effect;
}

TMediaElement MediaHandles::GetHandle() const
{
	if ( !mMediaList.size )
	{
		TMediaElement none;

		none.data = 0;
		return none;
	}
	return mMediaList.elements[irand( 0, mMediaList.size )];
}

FxScheduler::FxScheduler()
{
	mScheduledHead = 0;
	mScheduledCount = 0;
}

void FxScheduler::Clean( bool bRemoveTemplates, const FxEffectDef *fxToPreserve )
{
	ScheduledEffect *sfx;

	while ( mScheduledHead )
	{
		sfx = mScheduledHead;
		mScheduledHead = mScheduledHead->mScheduledNext;
		delete sfx;
	}
	mScheduledCount = 0;
	if ( bRemoveTemplates )
		FX_FreeTemplates( fxToPreserve );
}

// Original name unknown.
static void FX_PrintInvalidEffect()
{
	FX_Print( "FxScheduler::PlayEffect called with invalid effect\n" );
}

void FxScheduler::PlayEffect( const FxEffectDef *fx, const vec3_t origin )
{
	vec3_t axis[3];

	VectorSet( axis[0], 0.0f, 0.0f, 1.0f );
	VectorSet( axis[1], 1.0f, 0.0f, 0.0f );
	VectorSet( axis[2], 0.0f, 1.0f, 0.0f );
	PlayEffect( fx, origin, axis, 0 );
}

void FxScheduler::PlayEffect( const FxEffectDef *fx, const vec3_t origin, const vec3_t dir )
{
	vec3_t axis[3];

	VectorCopy( dir, axis[0] );
	MakeNormalVectors( axis[0], axis[1], axis[2] );
	PlayEffect( fx, origin, axis, 0 );
}

void FxScheduler::PlayEffect( const FxEffectDef *fx, const vec3_t origin, const vec3_t forward, const vec3_t up )
{
	vec3_t axis[3];

	VectorCopy( forward, axis[0] );
	VectorCopy( up, axis[2] );
	Vec3Cross( axis[0], axis[2], axis[1] );
	PlayEffect( fx, origin, axis, 0 );
}

// Distance culling against the primitive's spawn range; original name unknown.
static bool FX_IsInSpawnRange( const PrimitiveTemplate *primTemp, const vec3_t origin )
{
	float distSq;
	bool haveDist;
	float nearSq;
	float farSq;

	haveDist = false;
	distSq = 0.0f;
	if ( primTemp->mSpawnRange.min != 0.0f )
	{
		haveDist = true;
		distSq = Vec3DistanceSq( origin, theFxHelper->mCamera.vieworg );
		nearSq = primTemp->mSpawnRange.min * theFxHelper->adsZoomFactor;
		nearSq *= nearSq;
		if ( distSq < nearSq )
			return false;
	}
	if ( primTemp->mSpawnRange.max != 0.0f )
	{
		if ( !haveDist )
			distSq = Vec3DistanceSq( origin, theFxHelper->mCamera.vieworg );
		farSq = primTemp->mSpawnRange.max * theFxHelper->adsZoomFactor;
		farSq *= farSq;
		if ( distSq > farSq )
			return false;
	}
	return true;
}

void FxScheduler::PlayEffect( const FxEffectDef *fx, const vec3_t origin, const vec3_t axis[3], const FxBoltInfo *bolt )
{
	const PrimitiveTemplate *primTemp;
	int primIndex;
	int spawnCount;
	int delay;
	ScheduledEffect *sfx;
	float spacing;
	orientation_t or_;
	int numAdded;
	int index;

	primIndex = 0;
	spawnCount = 0;
	delay = 0;
	spacing = 0.0f;
	mSeed = theFxHelper->GetSeed();
	Rand_Init( mSeed );
	if ( !fx )
	{
		theFxHelper->SetIgnorePrecacheErrors( true );
		fx = FX_RegisterEffect( "fx/error.efx" );
		theFxHelper->SetIgnorePrecacheErrors( false );
		if ( !fx )
		{
			FX_PrintInvalidEffect();
			return;
		}
	}
	if ( fx_freeze->current.boolean || !fx_enable->current.boolean )
		return;
	if ( bolt )
	{
		if ( bolt->dobjHandle < 0 )
			return;
		if ( !FX_GetBoneOrientation( bolt, &or_ ) )
			return;
	}
	else
	{
		if ( origin )
			VectorCopy( origin, or_.origin );
		else
			VectorClear( or_.origin );
		AxisCopy( axis, or_.axis );
	}
	numAdded = 0;
	for ( primIndex = 0; primIndex < fx->mPrimitiveCount; primIndex++ )
	{
		primTemp = fx->mPrimitives[primIndex];
		if ( !FX_IsInSpawnRange( primTemp, origin ) )
			continue;
		if ( ( primTemp->mSpawnFlags & 0x400 ) && theFxHelper->CullSpherePreviousFrame( origin, primTemp->spawnFrustumCullRadius ) )
			continue;
		spawnCount = (int)( FxRange_GetVal( &primTemp->mSpawnCount ) + 0.5f );
		if ( !spawnCount )
			continue;
		if ( primTemp->mSpawnFlags & 0x200 )
			spacing = I_fabs( primTemp->mSpawnDelay.max - primTemp->mSpawnDelay.min ) / spawnCount;
		numAdded += spawnCount;
		for ( index = 0; index < spawnCount; index++ )
		{
			if ( primTemp->mSpawnFlags & 0x200 )
				delay = (int)( index * spacing );
			else
				delay = (int)FxRange_GetVal( &primTemp->mSpawnDelay );
			if ( delay < 1 )
			{
				CreateEffect( fx, primTemp, bolt, or_.origin, or_.axis, -delay, index );
			}
			else
			{
				sfx = new ScheduledEffect;
				if ( sfx )
				{
					sfx->mStartTime = theFxHelper->mTime + delay;
					sfx->mFx = fx;
					sfx->mPrimIndex = primIndex;
					sfx->mIndexInBatch = index;
					sfx->mSeed = theFxHelper->GetSeed() + primIndex * 0x369D035;
					if ( bolt )
					{
						sfx->mBolt = *bolt;
					}
					else
					{
						sfx->mBolt.dobjHandle = -1;
						sfx->mBolt.boneIndex = -1;
					}
					VectorCopy( or_.origin, sfx->mOrigin );
					AxisCopy( or_.axis, sfx->mAxis );
					sfx->mScheduledNext = mScheduledHead;
					mScheduledHead = sfx;
					mScheduledCount++;
				}
			}
		}
	}
	if ( numAdded && fx_count->current.boolean )
		theFxHelper->AddPlume( or_.origin, numAdded, colorYellow, 3000 );
}

float FxScheduler::GetEffectLength( const FxEffectDef *fx )
{
	const PrimitiveTemplate *primTemp;
	int primIndex;
	float delay;
	float life;
	float maxLength;

	maxLength = 0.0f;
	for ( primIndex = 0; primIndex < fx->mPrimitiveCount; primIndex++ )
	{
		primTemp = fx->mPrimitives[primIndex];
		delay = primTemp->mSpawnDelay.max;
		life = primTemp->mLife.max;
		if ( life + delay > maxLength )
			maxLength = life + delay;
	}
	return maxLength;
}

void FxScheduler::GetDecalColor( const PrimitiveTemplate *primTemp, vec3_t rgb )
{
	float blend;
	FxChannelInstance colorInst;
	FxChannelInstance colorRandInst;

	FxChannelInstance_Create( &primTemp->mFxChannels[FX_CHANNEL_COLOR], &colorInst );
	if ( primTemp->mAttributeFlags & 0x2000 )
	{
		blend = flrand( 0.0f, 1.0f );
		FxChannelInstance_Create( &primTemp->mFxChannels[FX_CHANNEL_COLOR_RAND], &colorRandInst );
		FxChannelInstance_Blend3d( &colorInst, &colorRandInst, blend, rgb, 0.0f );
	}
	else
	{
		FxChannelInstance_GetValue3d( &colorInst, rgb, 0.0f );
	}
}

float FxScheduler::GetDecalAlpha( const PrimitiveTemplate *primTemp )
{
	float blend;
	FxChannelInstance alphaInst;
	FxChannelInstance alphaRandInst;
	float alpha;

	FxChannelInstance_Create( &primTemp->mFxChannels[FX_CHANNEL_ALPHA], &alphaInst );
	if ( primTemp->mAttributeFlags & 0x4000 )
	{
		blend = flrand( 0.0f, 1.0f );
		FxChannelInstance_Create( &primTemp->mFxChannels[FX_CHANNEL_ALPHA_RAND], &alphaRandInst );
		alpha = FxChannelInstance_Blend1d( &alphaInst, &alphaRandInst, blend, 0.0f );
	}
	else
	{
		alpha = FxChannelInstance_GetValue1d( &alphaInst, 0.0f );
	}
	return ClampFloat( alpha, 0.0f, 1.0f );
}

float FxScheduler::GetDecalSize( const PrimitiveTemplate *primTemp )
{
	float blend;
	FxChannelInstance sizeInst;
	FxChannelInstance sizeRandInst;

	FxChannelInstance_Create( &primTemp->mFxChannels[FX_CHANNEL_SIZE], &sizeInst );
	if ( primTemp->mAttributeFlags & 0x8000 )
	{
		blend = flrand( 0.0f, 1.0f );
		FxChannelInstance_Create( &primTemp->mFxChannels[FX_CHANNEL_SIZE_RAND], &sizeRandInst );
		return FxChannelInstance_Blend1d( &sizeInst, &sizeRandInst, blend, 0.0f );
	}
	else
	{
		return FxChannelInstance_GetValue1d( &sizeInst, 0.0f );
	}
}

void CG_ImpactMark( Material *markMaterial, const float *origin, const float *dir, float orientation, const float *color, float radius );

// retail 0x496e20
void FxScheduler::CreateDecalEffect( const PrimitiveTemplate *primTemp, const vec3_t origin, const vec3_t axis[3] )
{
	float rotation;
	vec4_t color;
	float size;

	rotation = FxRange_GetVal( &primTemp->mRotation );
	GetDecalColor( primTemp, color );
	color[3] = GetDecalAlpha( primTemp );
	size = GetDecalSize( primTemp );
	CG_ImpactMark( primTemp->mMediaHandles.GetHandle().material, origin, axis[0], rotation, color, size );
}

bool FX_GetBoltingFrame( const PrimitiveTemplate *primTemp, const FxBoltInfo *bolt, FxBoltFramePtr *boltFrame )
{
	if ( ( primTemp->mAttributeFlags & 2 ) && bolt && bolt->dobjHandle >= 0 )
	{
		*boltFrame = FxBoltFrame::Acquire( *bolt );
		if ( !boltFrame->IsValid() )
			return false;
		if ( !boltFrame->Get()->GetOrientation() )
			return false;
		return true;
	}
	return true;
}

// Spins the spawn axis a random amount about the effect's forward axis; original name unknown.
static void FX_RandomizeRoll( const PrimitiveTemplate *primTemp, vec3_t ax[3], const vec3_t axis[3] )
{
	if ( primTemp->mSpawnFlags & 0x100 )
	{
		RotatePointAroundVector( ax[1], ax[0], axis[1], flrand( 0.0f, 360.0f ) );
		Vec3Cross( ax[0], ax[1], ax[2] );
	}
}

// Original name unknown.
static void FX_AddPrimitive( EffectPrimitive *prim, vec3_t ax[3], const vec3_t origin, int lateTime, int indexInBatch )
{
	const PrimitiveTemplate *primTemp;

	primTemp = prim->primTemp;
	switch ( primTemp->mType )
	{
	case PRIM_PARTICLE:
		FX_AddParticle( prim, ax, origin, lateTime, indexInBatch );
		break;
	case PRIM_LINE:
		FX_AddLine( prim, ax, origin, lateTime, indexInBatch );
		break;
	case PRIM_TAIL:
		FX_AddTail( prim, ax, origin, lateTime, indexInBatch );
		break;
	case PRIM_CYLINDER:
		FX_AddCylinder( prim, ax, origin, lateTime, indexInBatch );
		break;
	case PRIM_EMITTER:
		FX_AddEmitter( prim, ax, origin, lateTime, indexInBatch );
		break;
	case PRIM_DECAL:
		FX_AddDecal( prim, ax, origin, lateTime, indexInBatch );
		break;
	case PRIM_ORIENTED_PARTICLE:
		FX_AddOrientedParticle( prim, ax, origin, lateTime, indexInBatch );
		break;
	case PRIM_FXRUNNER:
		FX_AddFxRunner( prim, ax, origin, lateTime, indexInBatch );
		break;
	case PRIM_LIGHT:
		FX_AddLight( prim, ax, origin, lateTime, indexInBatch );
		break;
	case PRIM_CAMERA_SHAKE:
		FX_AddCameraShake( prim, ax, origin, lateTime, indexInBatch );
		break;
	case PRIM_FLASH:
		FX_AddFlash( prim, ax, origin, lateTime, indexInBatch );
		break;
	case PRIM_CLOUD:
		FX_AddCloud( prim, ax, origin, lateTime, indexInBatch );
		break;
	}
}

void FxScheduler::CreateEffect( const FxEffectDef *fx, const PrimitiveTemplate *primTemp, const FxBoltInfo *bolt, const vec3_t origin, const vec3_t axis[3], int lateTime, int indexInBatch )
{
	vec3_t ax[3];
	EffectPrimitive prim;

	AxisCopy( axis, ax );
	FX_RandomizeRoll( primTemp, ax, axis );
	if ( !FX_GetBoltingFrame( primTemp, bolt, &prim.boltFrame ) )
		return;
	prim.fx = fx;
	prim.primTemp = primTemp;
	FX_AddPrimitive( &prim, ax, origin, lateTime, indexInBatch );
}

void FxScheduler::Archive( FxArchive *arch )
{
	int count;
	ScheduledEffect *sfx;

	if ( arch->IsReading() )
	{
		mScheduledHead = 0;
		mScheduledCount = 0;
		for ( count = arch->ReadInt(); count; count-- )
		{
			sfx = new ScheduledEffect;
			sfx->Archive( arch );
			if ( sfx->mFx && sfx->mPrimIndex >= 0 && sfx->mPrimIndex < sfx->mFx->mPrimitiveCount && sfx->mFx->mPrimitives[sfx->mPrimIndex] )
			{
				sfx->mScheduledNext = mScheduledHead;
				mScheduledHead = sfx;
				mScheduledCount++;
			}
			else
			{
				delete sfx;
			}
		}
	}
	else
	{
		arch->WriteInt( mScheduledCount );
		for ( sfx = theFxScheduler->mScheduledHead; sfx; sfx = sfx->mScheduledNext )
			sfx->Archive( arch );
	}
}

void ScheduledEffect::Archive( FxArchive *arch )
{
	arch->ArchiveEffect( &mFx );
	arch->ArchiveInt( &mPrimIndex );
	arch->ArchiveInt( &mStartTime );
	arch->ArchiveData( &mBolt, 8 );
	arch->ArchiveVec3( mOrigin );
	arch->ArchiveVec3( mAxis[0] );
	arch->ArchiveVec3( mAxis[1] );
	arch->ArchiveVec3( mAxis[2] );
	arch->ArchiveInt( &mSeed );
}

void FX_InitTemplates()
{
	effectTemplateArrayCount = 0;
}

// Original name unknown.
static FxEffectDef *FX_FindRegisteredEffect( const char *name )
{
	int effectIndex;
	FxEffectDef *fx;

	for ( effectIndex = 0; effectIndex < effectTemplateArrayCount; effectIndex++ )
	{
		fx = effectTemplateArray[effectIndex];
		if ( !strcmp( fx->mEffectName, name ) )
			return fx;
	}
	return 0;
}

// Original name unknown.
static bool FX_AddRegisteredEffect( FxEffectDef *fx )
{
	if ( effectTemplateArrayCount == 256 )
	{
		FX_Print( "^1Max effect templates of '%i' exceeded\n", 256 );
		return false;
	}
	effectTemplateArray[effectTemplateArrayCount] = fx;
	effectTemplateArrayCount++;
	return true;
}

FxEffectDef *FX_TryRegisterEffect( const char *name )
{
	FxEffectDef *registeredTemplate;
	FxEffectDef *fx;
	GenericParser2 parser;

	registeredTemplate = FX_FindRegisteredEffect( name );
	if ( registeredTemplate )
		return registeredTemplate;
	fx = FX_ParseEffect( &parser, name );
	if ( !fx )
		return 0;
	if ( !FX_AddRegisteredEffect( fx ) )
		return 0;
	return fx;
}

// Original name unknown.
static void FX_FreeTemplates( const FxEffectDef *fxToPreserve )
{
	int effectIndex;
	bool foundTemplateToPreserve;
	FxEffectDef *fx;

	foundTemplateToPreserve = false;
	for ( effectIndex = 0; effectIndex < effectTemplateArrayCount; effectIndex++ )
	{
		fx = effectTemplateArray[effectIndex];
		if ( fx == fxToPreserve )
		{
			foundTemplateToPreserve = true;
		}
		else
		{
			FX_CleanTemplate( fx );
			effectTemplateArray[effectIndex] = 0;
		}
	}
	effectTemplateArrayCount = 0;
	if ( foundTemplateToPreserve )
		effectTemplateArray[effectTemplateArrayCount++] = (FxEffectDef *)fxToPreserve;
}
