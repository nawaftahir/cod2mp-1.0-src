#include <stdio.h>
#include <stdlib.h>
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

struct MemoryFile
{
	unsigned char *buffer;
	int bufferSize;
	int bytesUsed;
	bool errorOnOverflow;
	bool memoryOverflow;
	void (*archiveProc)( MemoryFile *memFile, int bytes, void *data );
};

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
extern dvar_t *fx_draw;
extern dvar_t *fx_cull;
extern dvar_t *fx_sort;
extern dvar_t *fx_profile;
extern dvar_t *fx_visMinTraceDist;

struct FxCurveIterator
{
	const struct FxCurve *master;
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
	const struct FxCurve *curve;
	FxRange scaleRange;
};

enum
{
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
};

struct FxEffectDef
{
	const char *mEffectName;
	int mPrimitiveCount;
	PrimitiveTemplate *mPrimitives[24];
};

struct FxBoltInfo
{
	int dobjHandle;
	int boneIndex;
};

#include "FxCamera.h"

class FxArchive;

#include "FxHelper.h"

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
extern "C" void *Z_MallocInternal( int size );
extern "C" void Z_FreeInternal( void *ptr );
float crandom();

template <class T>
class FxMemMgr
{
public:
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

class FxArchive
{
public:
	FxArchive();
	void BeginReading( MemoryFile *memFile );
	void BeginWriting( MemoryFile *memFile );

private:
	void ReadData( void *p, int byteCount );
	void WriteData( const void *p, int byteCount );

public:

	unsigned char ReadByte()
	{
		unsigned char value;

		ReadData( &value, 1 );
		return value;
	}

	int ReadInt()
	{
		int value;

		ReadData( &value, 4 );
		return value;
	}

	void WriteByte( unsigned char value )
	{
		WriteData( &value, 1 );
	}

	void WriteInt( int value )
	{
		WriteData( &value, 4 );
	}

	int GetUsedSize();

	MemoryFile *memFile;
	bool isReading;
	bool isWriting;
	int byteCount;
	int literalCount;
	int zeroCount;
	int controlPos;
};

class FxBoltFrame
{
public:
	const orientation_t *GetOrientation();

	int refCount;
	int mTime;
	orientation_t mOrientation;
	FxBoltFrame *next;
	FxBoltInfo mBolt;
};

class FxBoltFramePtr
{
public:
	bool IsValid() const;
	FxBoltFrame *Get() const;

private:
	FxBoltFrame *mFrame;
};

inline bool FxBoltFramePtr::IsValid() const
{
	return mFrame != 0;
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

// The field setters below are header inlines; their original names are unknown.
class Effect
{
public:
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

	void ClearFlags( int flags )
	{
		mFlags &= ~flags;
	}

	void SetMins( const vec3_t mins )
	{
		if ( mins )
			VectorCopy( mins, mMins );
		else
			VectorClear( mMins );
	}

	void SetMaxs( const vec3_t maxs )
	{
		if ( maxs )
			VectorCopy( maxs, mMaxs );
		else
			VectorClear( mMaxs );
	}

	void SetOrigin( const vec3_t origin )
	{
		if ( origin )
			VectorCopy( origin, mOrigin );
		else
			VectorClear( mOrigin );
	}

	void SetFlags( int flags )
	{
		mFlags = flags;
	}

	void SetGroupFlags( int flags )
	{
		mGroupFlags = flags;
	}

	void SetImpactFx( const FxEffectDef *fx )
	{
		mImpactFx = fx;
	}

	void SetDeathFx( const FxEffectDef *fx )
	{
		mDeathFx = fx;
	}

	void SetFx( const FxEffectDef *fx )
	{
		mFx = fx;
	}

	void SetPrimIndex( int index )
	{
		mPrimIndex = index;
	}

	int GetTimeStart()
	{
		return mTimeStart;
	}

	const FxEffectDef *GetFx()
	{
		return mFx;
	}

	int GetPrimIndex()
	{
		return mPrimIndex;
	}

	void SetTimeStartEnd( int start, int end );
	void SetBoltFrame( const FxBoltFramePtr &boltFrame );

	int GetDuration()
	{
		return mTimeEnd - mTimeStart;
	}

	static void operator delete( void *p );

	vec3_t mOrigin;
	int mGroupFlags;
	vec3_t mMins;
	vec3_t mMaxs;
	const FxEffectDef *mImpactFx;
	const FxEffectDef *mDeathFx;
	const FxEffectDef *mFx;
	int mPrimIndex;
	float mNormTime;
	FxGfxEntity mRefEnt;
	int mFlags;
	int mClusterId;
	int mSortGroup;
	XModel *mModel;
	int mTimeStart;
	int mTimeEnd;
	FxBoltFramePtr mBolt;
};

class Light : public Effect
{
public:
	Light();
	virtual ~Light();
	virtual bool Update();
	virtual bool Cull();
	virtual void Draw();
	virtual void CreateChannelInstances( const PrimitiveTemplate *primTemp );
	virtual unsigned char TypeID();
	virtual void Archive( FxArchive *arch );
	virtual void FixupArchiveLoad( const PrimitiveTemplate *primTemplate );

	void SetColorBlendFactor( float blend )
	{
		mColorBlendFactor = blend;
	}

	void SetSizeBlendFactor( float blend )
	{
		mSizeBlendFactor = blend;
	}

	static void *operator new( unsigned int size )
	{
		return s_memMgr.Alloc( size );
	}

	static void operator delete( void *p )
	{
		s_memMgr.Free( p );
	}

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

	void SetMaterial( Material *material )
	{
		mRefEnt.customMaterial = material;
	}

	void Init();
};

class Particle : public Effect
{
public:
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

	void SetMaterial( Material *material )
	{
		mRefEnt.customMaterial = material;
	}

	void SetGravity( float gravity )
	{
		mGravity = gravity;
	}

	void SetRotation( float rotation )
	{
		mRefEnt.rotation = rotation;
	}

	void SetElasticity( float elasticity )
	{
		mElasticity = elasticity;
	}

	void SetNonUniformScale( bool nonUniformScale )
	{
		mNonUniformScale = nonUniformScale;
	}

	void SetStartFrame( int frame )
	{
		mStartFrame = frame;
	}

	void SetFrameRate( float rate )
	{
		mFrameRate = rate;
	}

	void SetLoopMode( int mode )
	{
		mLoopMode = mode;
	}

	void SetLoopTimes( int times )
	{
		mLoopTimes = times;
	}

	void SetColorBlendFactor( float blend )
	{
		mColorBlendFactor = blend;
	}

	void SetAlphaBlendFactor( float blend )
	{
		mAlphaBlendFactor = blend;
	}

	void SetSizeBlendFactor( float blend )
	{
		mSizeBlendFactor = blend;
	}

	void SetSize2BlendFactor( float blend )
	{
		mSize2BlendFactor = blend;
	}

	void SetRotationBlendFactor( float blend )
	{
		mRotationBlendFactor = blend;
	}

	void SetWindModifier( float windModifier )
	{
		mWindModifier = windModifier;
	}

	static void *operator new( unsigned int size )
	{
		return s_memMgr.Alloc( size );
	}

	static void operator delete( void *p )
	{
		s_memMgr.Free( p );
	}

	void SetAxis( const vec3_t *ax );
	void SetRandomVelocityWeights( float weight1, float weight2, float weight3 );
	void SetRandomVelocity2Weights( float weight1, float weight2, float weight3 );
	void IntegrateTotalVelocity( int duration, vec3_t outVector );
	void GetTotalVelocityAtTime0( vec3_t outVector );

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

	static void *operator new( unsigned int size )
	{
		return s_memMgr.Alloc( size );
	}

	static void operator delete( void *p )
	{
		s_memMgr.Free( p );
	}

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
	Line();
	virtual ~Line();
	virtual void Die();
	virtual bool Update();
	virtual bool Cull();
	virtual void Draw();
	virtual unsigned char TypeID();
	virtual void Archive( FxArchive *arch );

	void SetEndpoint( const vec3_t endpoint )
	{
		VectorCopy( endpoint, mEndpoint );
	}

	static void *operator new( unsigned int size )
	{
		return s_memMgr.Alloc( size );
	}

	static void operator delete( void *p )
	{
		s_memMgr.Free( p );
	}

	vec3_t mEndpoint;

	static FxMemMgr<Line> s_memMgr;
};

class OrientedParticle : public Particle
{
public:
	OrientedParticle();
	virtual ~OrientedParticle();
	virtual bool Update();
	virtual bool Cull();
	virtual void Draw();
	virtual unsigned char TypeID();
	virtual void Archive( FxArchive *arch );

	void SetNormal( const vec3_t normal )
	{
		VectorCopy( normal, mNormal );
	}

	static void *operator new( unsigned int size )
	{
		return s_memMgr.Alloc( size );
	}

	static void operator delete( void *p )
	{
		s_memMgr.Free( p );
	}

	vec3_t mNormal;

	static FxMemMgr<OrientedParticle> s_memMgr;
};

class Tail : public Particle
{
public:
	Tail();
	virtual ~Tail();
	virtual bool Update();
	virtual bool Cull();
	virtual void Draw();
	virtual void CreateChannelInstances( const PrimitiveTemplate *primTemp );
	virtual unsigned char TypeID();
	virtual void Archive( FxArchive *arch );
	virtual void FixupArchiveLoad( const PrimitiveTemplate *primTemplate );

	void SetEndpoint( const vec3_t endpoint )
	{
		VectorCopy( endpoint, mEndpoint );
	}

	static void *operator new( unsigned int size )
	{
		return s_memMgr.Alloc( size );
	}

	static void operator delete( void *p )
	{
		s_memMgr.Free( p );
	}

	void InitEndPoint();

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
	Cylinder();
	virtual ~Cylinder();
	virtual bool Update();
	virtual bool Cull();
	virtual void Draw();
	virtual unsigned char TypeID();
	virtual void Archive( FxArchive *arch );

	void SetAxis( const vec3_t axis )
	{
		VectorCopy( axis, mRefEnt.axis[0] );
	}

	static void *operator new( unsigned int size )
	{
		return s_memMgr.Alloc( size );
	}

	static void operator delete( void *p )
	{
		s_memMgr.Free( p );
	}

	static FxMemMgr<Cylinder> s_memMgr;
};

class Emitter : public Particle
{
public:
	void RandomizeEmitDist()
	{
		mEmitDist = mEmitDistBase + crandom() * mEmitDistRand;
	}

	void SetModel( XModel *model )
	{
		mModel = model;
	}

	void SetAngles( const vec3_t angles )
	{
		Vec3CopyOrClear( angles, mAngles );
	}

	void SetAngleDelta( const vec3_t angleDelta )
	{
		Vec3CopyOrClear( angleDelta, mAngleDelta );
	}

	void SetEmitFx( const FxEffectDef *fx )
	{
		mEmitFx = fx;
	}

	void SetEmitDistBase( float dist )
	{
		mEmitDistBase = dist;
	}

	void SetEmitDistRand( float dist )
	{
		mEmitDistRand = dist;
	}

	void SetEmitTime( int time )
	{
		mEmitTime = time;
	}

	void SetEmitOrigin( const vec3_t origin )
	{
		Vec3CopyOrClear( origin, mEmitOrigin );
	}

	void SetEmitVelocity( const vec3_t velocity )
	{
		Vec3CopyOrClear( velocity, mEmitVelocity );
	}

	void SetBoltOffset( const vec3_t offset )
	{
		Vec3CopyOrClear( offset, mBoltOffset );
	}

	static void *operator new( unsigned int size )
	{
		return s_memMgr.Alloc( size );
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

struct EffectPrimitive
{
	const FxEffectDef *fx;
	const PrimitiveTemplate *primTemp;
	FxBoltFramePtr boltFrame;
};
// An effect primitive whose spawn delay has not yet elapsed.
struct ScheduledEffect
{
	static void operator delete( void *p );
	const FxEffectDef *mFx;
	int mPrimIndex;
	int mStartTime;
	FxBoltInfo mBolt;
	vec3_t mOrigin;
	vec3_t mAxis[3];
	int mSeed;
	int mIndexInBatch;
	ScheduledEffect *mScheduledNext;
};
struct FxScheduler
{
	static void *operator new( unsigned int size )
	{
		return Z_MallocInternal( sizeof( FxScheduler ) );
	}
	static void operator delete( void *p )
	{
		Z_FreeInternal( p );
	}
	FxScheduler();
	void Clean( bool bRemoveTemplates, const FxEffectDef *fxToPreserve );
	void PlayEffect( const FxEffectDef *fx, const vec3_t origin, const vec3_t dir );
	void PlayEffect( const FxEffectDef *fx, const vec3_t origin, const vec3_t axis[3], const FxBoltInfo *bolt );
	void CreateDecalEffect( const PrimitiveTemplate *primTemp, const vec3_t origin, const vec3_t axis[3] );
	void CreateEffect( const FxEffectDef *fx, const PrimitiveTemplate *primTemp, const FxBoltInfo *bolt, const vec3_t origin, const vec3_t axis[3], int lateTime, int indexInBatch );
	void Archive( FxArchive *arch );
	int mSeed;
	ScheduledEffect *mScheduledHead;
	int mScheduledCount;
};
struct EffectCluster
{
	vec3_t origin;
	int refCount;
};
struct SortedEffect
{
	Effect *fx;
	float distSq;
};
struct SortedCluster
{
	int index;
	float distSq;
};
struct EffectVisibility
{
	vec3_t origin;
	float radiusSq;
	float visibility;
};
extern FxScheduler *theFxScheduler;
// Original name unknown.
extern FxScheduler *theFxSchedulers[1];
extern volatile int fx_camera_valid;
int Net_LocalClientNum() throw();
float FxRange_GetVal( const FxRange *range );
float flrand( float min, float max );
int irand( int min, int max );
void Rand_Init( int seed );
float Vec3Normalize( float *v );
float Vec3DistanceSq( const vec3_t p1, const vec3_t p2 );
void Vec3Cross( const float *v0, const float *v1, float *cross );
void Vec3Lerp( const vec3_t start, const vec3_t end, float fraction, vec3_t endpos );
void AxisTransformComponents( const vec3_t axis[3], float x, float y, float z, vec3_t out );
void MakeNormalVectors( const vec3_t forward, vec3_t right, vec3_t up );
void RotatePointAroundVector( vec3_t dst, const vec3_t dir, const vec3_t point, float degrees );
void vectoangles( const float *value1, float *angles );
void OrientationPosToLocal( const orientation_t *orient, const float *pos, float *out );
void OrientationDirToLocal( const orientation_t *orient, const float *dir, float *out );
void FX_InitTemplates();
void FX_Print( const char *fmt, ... );
bool g_rendererExists = true;
EffectCluster effectClusterArray[1800];
EffectCluster *effectClusters = effectClusterArray;
int effectClusterCount;
// Original name unknown; unused on the dedicated server.
int effectClusterUnused;
int *clusterSort;
// Original name and type unknown; unused on the dedicated server.
int effectClusterReserved[24];
Effect *effectListArrayBolt[1800];
// Original name and type unknown; unused on the dedicated server.
int effectListReserved[24];
Effect *effectListArrayNonBolt[1800];
Effect **effectListBolt = effectListArrayBolt;
Effect **effectListNonBolt = effectListArrayNonBolt;
int g_effectVisArrayCount;
// Original name and type unknown; unused on the dedicated server.
int effectVisReserved[16];
EffectVisibility g_effectVisArray[1800];
int effectActiveCountBolt;
int effectActiveCountNonBolt;
int effectActiveCount;
int privateEffectActiveCountBolt;
int privateEffectActiveCountNonBolt;
int initialEffectActiveCountBolt;
int initialEffectActiveCountNonBolt;
// Original names unknown; unused on the dedicated server.
int effectCountUnused1;
int effectCountUnused2;
int cullEffectCountBolt;
int cullEffectCountNonBolt;
int effectBlockSightCount;
FxHelper theFxHelpers[1];
FxHelper *theFxHelper = theFxHelpers;
int fxInitialized[1] = {0};
SortedEffect visibleEffectsNonBolt[1800];
int visibleEffectCountNonBolt;
SortedEffect visibleEffectsBolt[1800];
int visibleEffectCountBolt;

struct DObj_s;
struct DObjAnimMat
{
	vec4_t quat;
	vec3_t trans;
	float transWeight;
};
extern const dvar_t *fx_debugBolt;
extern const vec4_t colorRed;
extern const vec4_t colorGreen;
extern const vec4_t colorBlue;
DObj_s *Com_GetClientDObj( int handle, int localClientNum );
int DObjNumBones( const DObj_s *obj );
DObjAnimMat *DObjGetRotTransArray( const DObj_s *obj );
void CG_DObjCalcBoneGeneric( int handle, int localClientNum, int boneIndex );
void CG_GetDObjOrientation( int dobjHandle, float (*axis)[3] );
void ConvertQuatToMat( const DObjAnimMat *mat, float axis[3][3] );
void AxisCopy( const float in[3][3], float out[3][3] );
void CL_AddDebugLine( const float *start, const float *end, const float *color, int depthTest, int duration, int fromServer );

extern void StatMon_Warning( int type, int duration, const char *materialName );

// Original name unknown.
inline int FxArchive::GetUsedSize()
{
	return memFile->bytesUsed;
}

// Original name unknown.
static inline void FX_SwapBolt( int i, int j )
{
	Effect *temp;

	temp = effectListBolt[i];
	effectListBolt[i] = effectListBolt[j];
	effectListBolt[j] = temp;
}

// Original name unknown.
static inline void FX_SwapNonBolt( int i, int j )
{
	Effect *temp;

	temp = effectListNonBolt[i];
	effectListNonBolt[i] = effectListNonBolt[j];
	effectListNonBolt[j] = temp;
}

int FX_GetCluster( const vec3_t origin )
{
	int i;
	float distSq;

	for ( i = 0; i < effectClusterCount; i++ )
	{
		distSq = Vec3DistanceSq(origin, effectClusters[i].origin);
		if ( distSq < 131072.0f )
		{
			effectClusters[i].refCount++;
			return i;
		}
	}
	VectorCopy(origin, effectClusters[effectClusterCount].origin);
	effectClusters[effectClusterCount].refCount = 1;
	effectClusterCount++;
	return effectClusterCount - 1;
}

void FX_RemoveCluster( int clusterId )
{
	int i;

	effectClusters[clusterId].refCount--;
	if ( effectClusters[clusterId].refCount > 0 )
		return;
	effectClusterCount--;
	if ( clusterId == effectClusterCount )
		return;
	effectClusters[clusterId] = effectClusters[effectClusterCount];
	for ( i = 0; i < effectActiveCountBolt; i++ )
	{
		if ( effectListBolt[i]->mClusterId == effectClusterCount )
			effectListBolt[i]->mClusterId = clusterId;
	}
	for ( i = 0; i < effectActiveCountNonBolt; i++ )
	{
		if ( effectListNonBolt[i]->mClusterId == effectClusterCount )
			effectListNonBolt[i]->mClusterId = clusterId;
	}
}

// Original name unknown.
void FX_UtilUnused()
{
}

// Original name unknown.
void FX_KillAllEffects()
{
	int i;

	for ( i = 0; i < effectActiveCountBolt; i++ )
		delete effectListBolt[i];
	for ( i = 0; i < effectActiveCountNonBolt; i++ )
		delete effectListNonBolt[i];
	effectActiveCountBolt = 0;
	effectActiveCountNonBolt = 0;
	effectActiveCount = 0;
	effectBlockSightCount = 0;
	effectClusterCount = 0;
}

void FX_Free( bool bRemoveTemplates )
{
	FX_KillAllEffects();
	if ( !theFxScheduler )
		return;
	theFxScheduler->Clean(bRemoveTemplates, 0);
	if ( !bRemoveTemplates )
		return;
	delete theFxScheduler;
	theFxScheduler = 0;
	theFxSchedulers[Net_LocalClientNum()] = 0;
}

// Original name unknown.
void FX_Reset()
{
	FX_KillAllEffects();
	theFxScheduler->Clean(false, 0);
}

bool FX_GetBoneOrientation( const FxBoltInfo *bolt, orientation_t *orient )
{
	float dobjAxis[4][3];
	float boneAxis[3][3];
	vec3_t end;
	DObj_s *obj;
	DObjAnimMat *boneMtx;
	DObjAnimMat *mats;

	CG_GetDObjOrientation(bolt->dobjHandle, dobjAxis);
	if ( bolt->boneIndex < 0 )
	{
		VectorCopy(dobjAxis[3], orient->origin);
		AxisCopy(dobjAxis, orient->axis);
		return true;
	}

	obj = Com_GetClientDObj(bolt->dobjHandle, 0);
	if ( !obj )
		return false;
	if ( bolt->boneIndex >= DObjNumBones(obj) )
		return false;

	CG_DObjCalcBoneGeneric(bolt->dobjHandle, 0, bolt->boneIndex);
	mats = DObjGetRotTransArray(obj);
	if ( !mats )
		return false;

	boneMtx = mats + bolt->boneIndex;
	ConvertQuatToMat(boneMtx, boneAxis);
	MatrixMultiply(boneAxis, dobjAxis, orient->axis);
	MatrixTransformVector43(boneMtx->trans, dobjAxis, orient->origin);

	if ( fx_debugBolt->current.decimal != 0.0f )
	{
		VectorMA(orient->origin, fx_debugBolt->current.decimal, orient->axis[0], end);
		CL_AddDebugLine(orient->origin, end, colorRed, 1, 0, 0);
		VectorMA(orient->origin, fx_debugBolt->current.decimal, orient->axis[1], end);
		CL_AddDebugLine(orient->origin, end, colorGreen, 1, 0, 0);
		VectorMA(orient->origin, fx_debugBolt->current.decimal, orient->axis[2], end);
		CL_AddDebugLine(orient->origin, end, colorBlue, 1, 0, 0);
	}
	return true;
}

void FX_InitServer()
{
	g_effectVisArrayCount = 0;
}

int FX_Init( bool rendererExists )
{
	if ( !fxInitialized[Net_LocalClientNum()] )
	{
		fxInitialized[Net_LocalClientNum()] = 1;
		effectActiveCountBolt = 0;
		effectActiveCountNonBolt = 0;
		effectActiveCount = 0;
		effectBlockSightCount = 0;
		effectClusterCount = 0;
	}
	FX_Free(true);
	g_rendererExists = rendererExists;
	theFxScheduler = new FxScheduler;
	FX_InitTemplates();
	theFxSchedulers[Net_LocalClientNum()] = theFxScheduler;
	theFxHelper->Init();
	return 1;
}

// Original name unknown.
static void FX_RemoveEffect( Effect *fx, bool bDie )
{
	if ( bDie )
		fx->Die();
	if ( fx->mFlags & 0x1000 )
		effectBlockSightCount--;
}

// Original name unknown.
static void FX_RemoveBolt( int index, bool bDie )
{
	Effect *fx;

	fx = effectListBolt[index];
	FX_SwapBolt(index, --privateEffectActiveCountBolt);
	FX_RemoveEffect(fx, bDie);
}

// Original name unknown.
static void FX_RemoveNonBolt( int index, bool bDie )
{
	Effect *fx;

	fx = effectListNonBolt[index];
	FX_SwapNonBolt(index, --privateEffectActiveCountNonBolt);
	FX_RemoveEffect(fx, bDie);
}

// Original name unknown.
static void FX_EffectLimitReached()
{
	StatMon_Warning(3, 3000, "gfx/2d/warning@maxeffects.jpg");
}

// Original name unknown.
static Effect **FX_AllocBolt()
{
	int count;

	count = ++effectActiveCount;
	if ( count <= 1800 )
	{
		++effectActiveCountBolt;
		return &effectListBolt[effectActiveCountBolt - 1];
	}
	--effectActiveCount;
	return 0;
}

// Original name unknown.
static Effect **FX_AllocNonBolt()
{
	int count;

	count = ++effectActiveCount;
	if ( count <= 1800 )
	{
		++effectActiveCountNonBolt;
		return &effectListNonBolt[effectActiveCountNonBolt - 1];
	}
	--effectActiveCount;
	return 0;
}

// Original name unknown.
float FX_GetEffectVisibility( const vec3_t start, const vec3_t end )
{
	Effect *fx;
	int i;
	vec3_t dir;
	float len;
	float halfLen;
	float visibility;

	if ( !effectBlockSightCount )
		return 1.0f;
	VectorSubtract(end, start, dir);
	len = Vec3Normalize(dir);
	if ( len < fx_visMinTraceDist->current.decimal )
		return 1.0f;
	halfLen = len * 0.5f;
	visibility = 1.0f;
	for ( i = 0; i < effectActiveCountNonBolt; i++ )
	{
		fx = effectListNonBolt[i];
		if ( fx->mFlags & 0x1000 )
			visibility *= fx->GetVisibility(start, dir, halfLen);
	}
	for ( i = 0; i < effectActiveCountBolt; i++ )
	{
		fx = effectListBolt[i];
		if ( fx->mFlags & 0x1000 )
			visibility *= fx->GetVisibility(start, dir, halfLen);
	}
	return visibility;
}

// Original name unknown.
void FX_AddVisibility()
{
	Effect *fx;
	int i;

	g_effectVisArrayCount = 0;
	if ( !effectBlockSightCount )
		return;
	for ( i = 0; i < effectActiveCountNonBolt; i++ )
	{
		fx = effectListNonBolt[i];
		if ( fx->mFlags & 0x1000 )
			fx->AddVisibility();
	}
	for ( i = 0; i < effectActiveCountBolt; i++ )
	{
		fx = effectListBolt[i];
		if ( fx->mFlags & 0x1000 )
			fx->AddVisibility();
	}
}

float FX_GetVisibility( const vec3_t start, const vec3_t end )
{
	int i;
	vec3_t dir;
	float len;
	float halfLen;
	float visibility;
	EffectVisibility *vis;
	vec3_t delta;
	vec3_t closest;
	float proj;
	float distSq;

	if ( !g_effectVisArrayCount )
		return 1.0f;
	VectorSubtract(end, start, dir);
	len = Vec3Normalize(dir);
	if ( len < fx_visMinTraceDist->current.decimal )
		return 1.0f;
	halfLen = len * 0.5f;
	visibility = 1.0f;
	for ( i = 0; i < g_effectVisArrayCount; i++ )
	{
		vis = &g_effectVisArray[i];
		VectorSubtract(vis->origin, start, delta);
		proj = DotProduct(delta, dir);
		if ( I_fabs(proj - halfLen) > halfLen )
			continue;
		VectorMA(start, proj, dir, closest);
		distSq = Vec3DistanceSq(vis->origin, closest);
		if ( distSq >= vis->radiusSq )
			continue;
		visibility *= vis->visibility;
	}
	return visibility;
}

// Original name unknown.
static void FX_FreeRemovedBolt()
{
	Effect *fx;
	int i;

	for ( i = privateEffectActiveCountBolt; i < initialEffectActiveCountBolt; i++, effectActiveCount-- )
	{
		fx = effectListBolt[i];
		FX_RemoveCluster(fx->mClusterId);
		delete fx;
		effectActiveCountBolt--;
		effectListBolt[i] = effectListBolt[effectActiveCountBolt];
	}
}

// Original name unknown.
static void FX_FreeRemovedNonBolt()
{
	Effect *fx;
	int i;

	for ( i = privateEffectActiveCountNonBolt; i < initialEffectActiveCountNonBolt; i++, effectActiveCount-- )
	{
		fx = effectListNonBolt[i];
		FX_RemoveCluster(fx->mClusterId);
		delete fx;
		effectActiveCountNonBolt--;
		effectListNonBolt[i] = effectListNonBolt[effectActiveCountNonBolt];
	}
}

void FX_Rewind( int time )
{
	int i;

	privateEffectActiveCountBolt = effectActiveCountBolt;
	initialEffectActiveCountBolt = effectActiveCountBolt;
	i = 0;
	while ( i < privateEffectActiveCountBolt )
	{
		if ( effectListBolt[i]->GetTimeStart() > time )
			FX_RemoveBolt(i, false);
		else
			i++;
	}
	FX_FreeRemovedBolt();
	privateEffectActiveCountNonBolt = effectActiveCountNonBolt;
	initialEffectActiveCountNonBolt = effectActiveCountNonBolt;
	i = 0;
	while ( i < privateEffectActiveCountNonBolt )
	{
		if ( effectListNonBolt[i]->GetTimeStart() > time )
			FX_RemoveNonBolt(i, false);
		else
			i++;
	}
	FX_FreeRemovedNonBolt();
	theFxScheduler->Clean(false, 0);
}

// Original name unknown.
static void FX_ArchiveEffect( Effect *fx, FxArchive *arch )
{
	arch->WriteByte(fx->TypeID());
	arch->WriteInt(fx->mTimeEnd);
	fx->Archive(arch);
}

// Original name unknown.
int FX_Save( MemoryFile *memFile )
{
	FxArchive arch;
	int i;
	Effect *fx;

	arch.BeginWriting(memFile);
	theFxHelper->Archive(&arch);
	theFxScheduler->Archive(&arch);
	for ( i = 0; i < effectActiveCountBolt; i++ )
	{
		fx = effectListBolt[i];
		FX_ArchiveEffect(fx, &arch);
	}
	for ( i = 0; i < effectActiveCountNonBolt; i++ )
	{
		fx = effectListNonBolt[i];
		FX_ArchiveEffect(fx, &arch);
	}
	arch.WriteByte(0);
	return arch.GetUsedSize();
}

// Original name unknown.
static const PrimitiveTemplate *FX_GetPrimTemplate( const FxEffectDef *fx, int index )
{
	const PrimitiveTemplate *primTemp;

	if ( index >= fx->mPrimitiveCount )
		return 0;
	primTemp = fx->mPrimitives[index];
	return primTemp;
}

int FX_Restore( MemoryFile *memFile )
{
	int i;
	int type;
	int timeEnd;
	Effect *fx;
	FxArchive arch;
	const PrimitiveTemplate *primTemp;

	FX_Free(false);
	arch.BeginReading(memFile);
	theFxHelper->Archive(&arch);
	theFxScheduler->Archive(&arch);
	for ( i = 0; ; i++ )
	{
		type = arch.ReadByte();
		if ( !type )
			break;
		timeEnd = arch.ReadInt();
		switch ( type )
		{
		case PRIM_PARTICLE:
			fx = new Particle;
			break;
		case PRIM_LINE:
			fx = new Line;
			break;
		case PRIM_TAIL:
			fx = new Tail;
			break;
		case PRIM_CYLINDER:
			fx = new Cylinder;
			break;
		case PRIM_EMITTER:
			fx = new Emitter;
			break;
		case PRIM_ORIENTED_PARTICLE:
			fx = new OrientedParticle;
			break;
		case PRIM_LIGHT:
			fx = new Light;
			break;
		case PRIM_FLASH:
			fx = new Flash;
			break;
		case PRIM_CLOUD:
			fx = new Cloud;
			break;
		default:
			continue;
		}
		fx->Archive(&arch);
		primTemp = FX_GetPrimTemplate(fx->GetFx(), fx->GetPrimIndex());
		if ( !primTemp )
		{
			delete fx;
			continue;
		}
		fx->FixupArchiveLoad(primTemp);
		if ( fx->mFlags & 0x1000 )
			effectBlockSightCount++;
		if ( fx->mBolt.IsValid() )
		{
			effectListBolt[effectActiveCountBolt] = fx;
			effectActiveCountBolt++;
			effectActiveCount++;
		}
		else
		{
			effectListNonBolt[effectActiveCountNonBolt] = fx;
			effectActiveCountNonBolt++;
			effectActiveCount++;
		}
	}
	return arch.GetUsedSize();
}

// Original name unknown.
static int FX_CompareSortedEffects( const void *arg1, const void *arg2 )
{
	int unused;
	const SortedEffect *se2;
	const SortedEffect *se1;
	Effect *fx2;
	Effect *fx1;
	int diff;
	int result;
	int order;
	int unused2;

	se1 = (const SortedEffect *)arg1;
	se2 = (const SortedEffect *)arg2;
	fx1 = se1->fx;
	fx2 = se2->fx;
	diff = fx1->mSortGroup - fx2->mSortGroup;
	if ( diff )
	{
		result = diff;
	}
	else
	{
		diff = clusterSort[fx1->mClusterId] - clusterSort[fx2->mClusterId];
		if ( diff )
		{
			result = diff;
		}
		else
		{
			diff = (int)fx1->mRefEnt.customMaterial - (int)fx2->mRefEnt.customMaterial;
			if ( diff )
			{
				result = diff;
			}
			else
			{
				if ( se1->distSq < se2->distSq )
					order = 1;
				else
					order = -1;
				result = order;
			}
		}
	}
	return result;
}

// Original name unknown.
static int FX_CompareSortedClusters( const void *arg1, const void *arg2 )
{
	const SortedCluster *sc2;
	const SortedCluster *sc1;
	int result;
	int unused;

	sc1 = (const SortedCluster *)arg1;
	sc2 = (const SortedCluster *)arg2;
	if ( sc1->distSq < sc2->distSq )
		result = 1;
	else
		result = -1;
	return result;
}

// Original name unknown.
static void FX_SortEffects( SortedEffect *list, int count )
{
	int i;
	SortedCluster sorted[1800];
	int rank[1800];

	for ( i = 0; i < effectClusterCount; i++ )
	{
		sorted[i].index = i;
		sorted[i].distSq = Vec3DistanceSq(effectClusters[i].origin, theFxHelper->mCamera.vieworg);
	}
	qsort(sorted, effectClusterCount, sizeof(SortedCluster), FX_CompareSortedClusters);
	for ( i = 0; i < effectClusterCount; i++ )
		rank[sorted[i].index] = i;
	clusterSort = rank;
	qsort(list, count, sizeof(SortedEffect), FX_CompareSortedEffects);
	clusterSort = 0;
}

// Original name unknown.
void FX_UpdateAllBolt()
{
	int i;
	Effect *fx;

	privateEffectActiveCountBolt = effectActiveCountBolt;
	initialEffectActiveCountBolt = effectActiveCountBolt;
	for ( i = 0; i < privateEffectActiveCountBolt; )
	{
		fx = effectListBolt[i];
		if ( theFxHelper->mTime > fx->mTimeEnd )
		{
			fx->ClearFlags(0x400);
			FX_RemoveBolt(i, true);
		}
		else if ( !fx->Update() )
		{
			FX_RemoveBolt(i, true);
		}
		else
		{
			i++;
		}
	}
	FX_FreeRemovedBolt();
}

// Original name unknown.
void FX_UpdateAllNonBolt()
{
	int i;
	Effect *fx;

	privateEffectActiveCountNonBolt = effectActiveCountNonBolt;
	initialEffectActiveCountNonBolt = effectActiveCountNonBolt;
	for ( i = 0; i < privateEffectActiveCountNonBolt; )
	{
		fx = effectListNonBolt[i];
		if ( theFxHelper->mTime > fx->mTimeEnd )
		{
			fx->ClearFlags(0x400);
			FX_RemoveNonBolt(i, true);
		}
		else if ( !fx->Update() )
		{
			FX_RemoveNonBolt(i, true);
		}
		else
		{
			i++;
		}
	}
	FX_FreeRemovedNonBolt();
}

// Original name unknown.
void FX_UpdateNewBolt( int start )
{
	Effect *fx;
	bool keep;
	int i;

	initialEffectActiveCountBolt = privateEffectActiveCountBolt;
	for ( i = start; i < privateEffectActiveCountBolt; )
	{
		fx = effectListBolt[i];
		if ( theFxHelper->mTime > fx->mTimeEnd )
		{
			fx->ClearFlags(0x400);
			FX_RemoveBolt(i, true);
		}
		else
		{
			keep = fx->Update();
			if ( !keep )
				FX_RemoveBolt(i, true);
			else
				i++;
		}
	}
}

// Original name unknown.
static void FX_CollectVisibleBolt()
{
	Effect *fx;
	int i;

	for ( i = cullEffectCountBolt; i < privateEffectActiveCountBolt; i++ )
	{
		fx = effectListBolt[i];
		if ( fx_cull->current.boolean && fx->Cull() )
			continue;
		visibleEffectsBolt[visibleEffectCountBolt].fx = fx;
		visibleEffectsBolt[visibleEffectCountBolt].distSq = Vec3DistanceSq(fx->mRefEnt.origin, theFxHelper->mCamera.vieworg);
		visibleEffectCountBolt++;
	}
	cullEffectCountBolt = privateEffectActiveCountBolt;
}

// Original name unknown.
void FX_UpdateNewNonBolt( int start )
{
	Effect *fx;
	bool keep;
	int i;

	initialEffectActiveCountNonBolt = privateEffectActiveCountNonBolt;
	for ( i = start; i < privateEffectActiveCountNonBolt; )
	{
		fx = effectListNonBolt[i];
		if ( theFxHelper->mTime > fx->mTimeEnd )
		{
			fx->ClearFlags(0x400);
			FX_RemoveNonBolt(i, true);
		}
		else
		{
			keep = fx->Update();
			if ( !keep )
				FX_RemoveNonBolt(i, true);
			else
				i++;
		}
	}
}

// Original name unknown.
static void FX_CollectVisibleNonBolt()
{
	Effect *fx;
	int i;

	if ( !fx_camera_valid )
		return;
	for ( i = cullEffectCountNonBolt; i < privateEffectActiveCountNonBolt; i++ )
	{
		fx = effectListNonBolt[i];
		if ( fx_cull->current.boolean && fx->Cull() )
			continue;
		visibleEffectsNonBolt[visibleEffectCountNonBolt].fx = fx;
		visibleEffectsNonBolt[visibleEffectCountNonBolt].distSq = Vec3DistanceSq(fx->mRefEnt.origin, theFxHelper->mCamera.vieworg);
		visibleEffectCountNonBolt++;
	}
	cullEffectCountNonBolt = privateEffectActiveCountNonBolt;
}

void FX_DrawAll()
{
	int i;

	FX_CollectVisibleNonBolt();
	FX_CollectVisibleBolt();
	for ( i = 0; i < visibleEffectCountBolt; i++ )
	{
		visibleEffectsNonBolt[visibleEffectCountNonBolt] = visibleEffectsBolt[i];
		visibleEffectCountNonBolt++;
	}
	FX_AddVisibility();
	if ( fx_sort->current.boolean )
		FX_SortEffects(visibleEffectsNonBolt, visibleEffectCountNonBolt);
	if ( fx_draw->current.boolean )
	{
		for ( i = 0; i < visibleEffectCountNonBolt; i++ )
			visibleEffectsNonBolt[i].fx->Draw();
	}
	if ( fx_profile->current.boolean )
	{
		FX_Print("Active    FX: %i\n", effectActiveCount);
		FX_Print("Drawn     FX: %i\n", visibleEffectCountNonBolt);
		FX_Print("Scheduled FX: %i\n", theFxScheduler->mScheduledCount);
	}
}

// Original name unknown.
void FX_CalcOriginAndAxis( EffectPrimitive *prim, vec3_t out, const vec3_t origin, vec3_t axis[3] )
{
	float yaw;
	float pitch;
	float radius;
	float height;
	float sinYaw;
	float cosYaw;
	float sinPitch;
	float cosPitch;
	float len;
	vec3_t dir;
	vec3_t point;
	float pointRadius;
	float pointHeight;
	vec3_t up = { 0.0f, 0.0f, 1.0f };
	vec3_t pos;
	const orientation_t *orient;
	const PrimitiveTemplate *primTemp;

	primTemp = prim->primTemp;
	if ( primTemp->mSpawnFlags & 0x40 )
		VectorSet(pos, FxRange_GetVal(&primTemp->mOrigin1X), FxRange_GetVal(&primTemp->mOrigin1Y), FxRange_GetVal(&primTemp->mOrigin1Z));
	else
		AxisTransformComponents(axis, FxRange_GetVal(&primTemp->mOrigin1X), FxRange_GetVal(&primTemp->mOrigin1Y), FxRange_GetVal(&primTemp->mOrigin1Z), pos);
	VectorAdd(pos, origin, pos);
	if ( primTemp->mSpawnFlags & 1 )
	{
		yaw = flrand(0.0f, 360.0f) * 0.017453292f;
		FastSinCos(yaw, &sinYaw, &cosYaw);
		pitch = flrand(0.0f, 180.0f) * 0.017453292f;
		FastSinCos(pitch, &sinPitch, &cosPitch);
		radius = FxRange_GetVal(&primTemp->mRadius);
		height = FxRange_GetVal(&primTemp->mHeight);
		radius *= sinPitch; VectorSet(dir, radius * sinYaw, radius * cosYaw, height * cosPitch);
		VectorAdd(pos, dir, pos);
		if ( primTemp->mSpawnFlags & 2 )
		{
			len = VectorLength(dir);
			if ( len != 0.0f )
			{
				VectorScale(dir, 1.0f / len, axis[0]);
				MakeNormalVectors(axis[0], axis[1], axis[2]);
			}
		}
	}
	else if ( primTemp->mSpawnFlags & 4 )
	{
		pointHeight = flrand(-0.5f, 0.5f) * FxRange_GetVal(&primTemp->mHeight);
		pointRadius = FxRange_GetVal(&primTemp->mRadius);
		VectorScale(axis[1], pointRadius, point);
		VectorMA(point, pointHeight, axis[0], point);
		RotatePointAroundVector(dir, axis[0], point, flrand(0.0f, 360.0f));
		VectorAdd(pos, dir, pos);
		if ( primTemp->mSpawnFlags & 2 )
		{
			len = VectorLength(dir);
			if ( len != 0.0f )
			{
				VectorScale(dir, 1.0f / len, axis[0]);
				if ( I_fabs(axis[0][2]) >= 0.999f )
					VectorSet(up, 0.0f, 1.0f, 0.0f);
				Vec3Cross(up, axis[0], axis[1]);
				Vec3Normalize(axis[1]);
				Vec3Cross(axis[0], axis[1], axis[2]);
			}
		}
	}
	if ( prim->boltFrame.IsValid() )
	{
		orient = prim->boltFrame.Get()->GetOrientation();
		OrientationPosToLocal(orient, pos, out);
	}
	else
	{
		VectorCopy(pos, out);
	}
}

// Original name unknown.
void FX_CalcImpactOrigin( const PrimitiveTemplate *primTemp, const vec3_t origin, vec3_t out, const vec3_t offset, const vec3_t *axis )
{
	trace_t trace;
	vec3_t end;
	vec3_t delta;

	if ( primTemp->mSpawnFlags & 8 )
	{
		VectorMA(origin, 16384.0f, axis[0], end);
		if ( primTemp->mSpawnFlags & 0x20 )
		{
			if ( ( primTemp->mSpawnFlags & 0x80 ) != 0 )
			{
				VectorSet(out, FxRange_GetVal(&primTemp->mOrigin2X), FxRange_GetVal(&primTemp->mOrigin2Y), FxRange_GetVal(&primTemp->mOrigin2Z));
				VectorAdd(out, end, end);
			}
			else
			{
				AxisTransformComponents(axis, FxRange_GetVal(&primTemp->mOrigin2X), FxRange_GetVal(&primTemp->mOrigin2Y), FxRange_GetVal(&primTemp->mOrigin2Z), delta);
				VectorAdd(end, delta, end);
			}
		}
		theFxHelper->Trace(&trace, origin, vec3_origin, vec3_origin, end, -1, 1);
		Vec3Lerp(origin, end, trace.fraction, out);
		if ( primTemp->mSpawnFlags & 0x10 )
			theFxScheduler->PlayEffect(primTemp->mImpactFxHandles.GetEffect(), out, trace.normal);
	}
	else
	{
		if ( ( primTemp->mSpawnFlags & 0x80 ) != 0 )
			VectorSet(out, FxRange_GetVal(&primTemp->mOrigin2X), FxRange_GetVal(&primTemp->mOrigin2Y), FxRange_GetVal(&primTemp->mOrigin2Z));
		else
			AxisTransformComponents(axis, FxRange_GetVal(&primTemp->mOrigin2X), FxRange_GetVal(&primTemp->mOrigin2Y), FxRange_GetVal(&primTemp->mOrigin2Z), out);
		VectorAdd(out, offset, out);
	}
}

// Original name unknown.
void FX_ApplyLateStart( const PrimitiveTemplate *primTemp, Particle *particle, vec3_t origin, int lateTime )
{
	vec3_t delta;
	float scale;

	if ( lateTime > 0 )
	{
		scale = lateTime * 0.001f;
		particle->IntegrateTotalVelocity(lateTime, delta);
		VectorMA(origin, scale, delta, origin);
	}
}

bool FX_AddPrimitive( EffectPrimitive *prim, Effect *fx, const vec3_t origin )
{
	Effect **slot;
	const PrimitiveTemplate *primTemp;
	int clusterId;

	primTemp = prim->primTemp;
	if ( prim->boltFrame.IsValid() )
		slot = FX_AllocBolt();
	else
		slot = FX_AllocNonBolt();
	clusterId = FX_GetCluster(origin);
	if ( !slot )
	{
		FX_EffectLimitReached();
		return false;
	}
	if ( primTemp->mAttributeFlags & 0x1000 )
		effectBlockSightCount++;
	*slot = fx;
	fx->SetFlags(primTemp->mAttributeFlags);
	fx->SetTimeStartEnd(theFxHelper->mTime, theFxHelper->mTime + (int)FxRange_GetVal(&primTemp->mLife));
	fx->SetFx(prim->fx);
	fx->SetPrimIndex(primTemp->mParentPrimIndex);
	fx->SetGroupFlags(primTemp->mGroupFlags);
	fx->SetMins(primTemp->mMin);
	fx->SetMaxs(primTemp->mMax);
	fx->SetDeathFx(primTemp->mDeathFxHandles.GetEffect());
	fx->SetImpactFx(primTemp->mImpactFxHandles.GetEffect());
	fx->CreateChannelInstances(primTemp);
	fx->SetBoltFrame(prim->boltFrame);
	fx->mClusterId = clusterId;
	return true;
}

// Original name unknown.
void FX_SetSortGroup( Effect *fx )
{
	fx->mSortGroup = 0;
	if ( fx->mRefEnt.customMaterial && theFxHelper->IsMaterialRefractive(fx->mRefEnt.customMaterial) )
		fx->mSortGroup = -1;
}

// Original name unknown.
void FX_SetMaterialAndSequenceParams( const PrimitiveTemplate *primTemp, Particle *particle, int duration, int indexInBatch )
{
	TMediaElement material;
	int count;
	int startFrame;
	float frameRate;

	material = primTemp->mMediaHandles.GetHandle();
	if ( !material.material )
	{
		startFrame = 0;
		frameRate = 0.0f;
	}
	else
	{
		count = theFxHelper->GetMaterialSubimageCount(material.material);
		if ( count == 1 )
		{
			startFrame = 0;
			frameRate = 0.0f;
		}
		else
		{
			if ( primTemp->mSequenceStartFrameMode == 0 )
				startFrame = primTemp->mSequenceFixedFrameValue - 1;
			else if ( primTemp->mSequenceStartFrameMode == 1 )
				startFrame = irand(0, count);
			else if ( primTemp->mSequenceStartFrameMode == 2 )
				startFrame = indexInBatch;
			else
				startFrame = 0;
			if ( primTemp->mSequencePlayRateMode == 0 )
				frameRate = primTemp->mSequenceFixedFpsValue / 1000.0f;
			else if ( primTemp->mSequencePlayRateMode == 1 )
				frameRate = (float)count / duration;
			else
				frameRate = 0.0f;
		}
	}
	particle->SetStartFrame(startFrame);
	particle->SetFrameRate(frameRate);
	particle->SetLoopMode(primTemp->mSequenceLoopMode);
	particle->SetLoopTimes(primTemp->mSequenceLoopTimes);
	particle->SetMaterial(material.material);
	FX_SetSortGroup(particle);
}

// Original name unknown.
void FX_InitParticle( EffectPrimitive *prim, Particle *particle, vec3_t newOrigin, const vec3_t origin, vec3_t axis[3], int indexInBatch )
{
	const PrimitiveTemplate *primTemp;

	primTemp = prim->primTemp;
	if ( primTemp->mAttributeFlags & 0x2000 )
		particle->SetColorBlendFactor(flrand(0.0f, 1.0f));
	if ( primTemp->mAttributeFlags & 0x4000 )
		particle->SetAlphaBlendFactor(flrand(0.0f, 1.0f));
	if ( ( primTemp->mAttributeFlags & 0x8000 ) != 0 )
		particle->SetSizeBlendFactor(flrand(0.0f, 1.0f));
	if ( primTemp->mAttributeFlags & 0x10000 )
		particle->SetSize2BlendFactor(flrand(0.0f, 1.0f));
	if ( primTemp->mAttributeFlags & 0x40000 )
		particle->SetRotationBlendFactor(flrand(0.0f, 1.0f));
	if ( primTemp->mAttributeFlags & 0x80000 )
		particle->SetRandomVelocityWeights(flrand(0.0f, 1.0f), flrand(0.0f, 1.0f), flrand(0.0f, 1.0f));
	if ( primTemp->mAttributeFlags & 0x100000 )
		particle->SetRandomVelocity2Weights(flrand(0.0f, 1.0f), flrand(0.0f, 1.0f), flrand(0.0f, 1.0f));
	particle->SetGravity(FxRange_GetVal(&primTemp->mGravity));
	particle->SetWindModifier(FxRange_GetVal(&primTemp->mWindModifier));
	FX_CalcOriginAndAxis(prim, newOrigin, origin, axis);
	particle->SetAxis(axis);
	particle->SetNonUniformScale(primTemp->mNonUniformScale);
	particle->SetElasticity(FxRange_GetVal(&primTemp->mElasticity));
	particle->SetRotation(FxRange_GetVal(&primTemp->mRotation));
}

// Original name unknown.
void FX_AddParticle( EffectPrimitive *prim, vec3_t axis[3], const vec3_t origin, int lateTime, int indexInBatch )
{
	Particle *particle;
	vec3_t newOrigin;
	bool added;

	particle = new Particle;
	if ( !particle )
		return;
	added = FX_AddPrimitive(prim, particle, origin);
	if ( !added )
	{
		delete particle;
		return;
	}
	FX_InitParticle(prim, particle, newOrigin, origin, axis, indexInBatch);
	FX_SetMaterialAndSequenceParams(prim->primTemp, particle, particle->GetDuration(), indexInBatch);
	FX_ApplyLateStart(prim->primTemp, particle, newOrigin, lateTime);
	particle->SetOrigin(newOrigin);
}

// Original name unknown.
void FX_AddLine( EffectPrimitive *prim, vec3_t axis[3], const vec3_t origin, int lateTime, int indexInBatch )
{
	Line *line;
	TMediaElement material;
	vec3_t start;
	vec3_t end;
	vec3_t startOut;
	vec3_t endOut;
	vec3_t localEnd;
	const orientation_t *orient;
	bool added;
	const PrimitiveTemplate *primTemp;

	line = new Line;
	if ( !line )
		return;
	added = FX_AddPrimitive(prim, line, origin);
	if ( !added )
	{
		delete line;
		return;
	}
	FX_CalcOriginAndAxis(prim, start, origin, axis);
	line->SetAxis(axis);
	primTemp = prim->primTemp;
	FX_CalcImpactOrigin(primTemp, start, end, origin, axis);
	VectorCopy(start, startOut);
	VectorCopy(end, endOut);
	material = primTemp->mMediaHandles.GetHandle();
	if ( prim->boltFrame.IsValid() )
	{
		orient = prim->boltFrame.Get()->GetOrientation();
		OrientationPosToLocal(orient, endOut, localEnd);
		line->SetEndpoint(localEnd);
	}
	else
	{
		line->SetEndpoint(endOut);
	}
	line->SetOrigin(startOut);
	line->SetMaterial(material.material);
	if ( primTemp->mAttributeFlags & 0x2000 )
		line->SetColorBlendFactor(flrand(0.0f, 1.0f));
	if ( primTemp->mAttributeFlags & 0x4000 )
		line->SetAlphaBlendFactor(flrand(0.0f, 1.0f));
	if ( ( primTemp->mAttributeFlags & 0x8000 ) != 0 )
		line->SetSizeBlendFactor(flrand(0.0f, 1.0f));
	if ( primTemp->mAttributeFlags & 0x10000 )
		line->SetSize2BlendFactor(flrand(0.0f, 1.0f));
	FX_SetSortGroup(line);
}

// Original name unknown.
void FX_AddTail( EffectPrimitive *prim, vec3_t axis[3], const vec3_t origin, int lateTime, int indexInBatch )
{
	Tail *tail;
	vec3_t newOrigin;
	vec3_t end;
	bool added;

	tail = new Tail;
	if ( !tail )
		return;
	added = FX_AddPrimitive(prim, tail, origin);
	if ( !added )
	{
		delete tail;
		return;
	}
	FX_InitParticle(prim, tail, newOrigin, origin, axis, indexInBatch);
	FX_SetMaterialAndSequenceParams(prim->primTemp, tail, tail->GetDuration(), indexInBatch);
	FX_ApplyLateStart(prim->primTemp, tail, newOrigin, lateTime);
	VectorMA(newOrigin, -1.0f, axis[0], end);
	tail->SetOrigin(newOrigin);
	tail->SetEndpoint(end);
	tail->mLengthBlendFactor = flrand(0.0f, 1.0f);
	tail->InitEndPoint();
}

// Original name unknown.
void FX_AddCylinder( EffectPrimitive *prim, vec3_t axis[3], const vec3_t origin, int lateTime, int indexInBatch )
{
	Cylinder *cylinder;
	vec3_t newOrigin;
	vec3_t originOut;
	vec3_t axisOut;
	vec3_t localAxis;
	const orientation_t *orient;
	bool added;

	cylinder = new Cylinder;
	if ( !cylinder )
		return;
	added = FX_AddPrimitive(prim, cylinder, origin);
	if ( !added )
	{
		delete cylinder;
		return;
	}
	FX_InitParticle(prim, cylinder, newOrigin, origin, axis, indexInBatch);
	FX_SetMaterialAndSequenceParams(prim->primTemp, cylinder, cylinder->GetDuration(), indexInBatch);
	VectorCopy(newOrigin, originOut);
	VectorCopy(axis[0], axisOut);
	if ( prim->boltFrame.IsValid() )
	{
		orient = prim->boltFrame.Get()->GetOrientation();
		OrientationDirToLocal(orient, axisOut, localAxis);
		cylinder->SetAxis(localAxis);
	}
	else
	{
		cylinder->SetAxis(axisOut);
	}
	cylinder->SetOrigin(originOut);
}

// Original name unknown.
void FX_AddEmitter( EffectPrimitive *prim, vec3_t axis[3], const vec3_t origin, int lateTime, int indexInBatch )
{
	Emitter *emitter;
	const FxEffectDef *emitFx;
	float emitDistBase;
	float emitDistRand;
	TMediaElement model;
	vec3_t angles;
	vec3_t angleDelta;
	vec3_t axisAngles;
	vec3_t newOrigin;
	vec3_t velocity;
	const orientation_t *orient;
	bool added;
	const PrimitiveTemplate *primTemp;

	emitter = new Emitter;
	if ( !emitter )
		return;
	added = FX_AddPrimitive(prim, emitter, origin);
	if ( !added )
	{
		delete emitter;
		return;
	}
	FX_InitParticle(prim, emitter, newOrigin, origin, axis, indexInBatch);
	primTemp = prim->primTemp;
	FX_ApplyLateStart(primTemp, emitter, newOrigin, lateTime);
	VectorSet(angles, FxRange_GetVal(&primTemp->mAngle1), FxRange_GetVal(&primTemp->mAngle2), FxRange_GetVal(&primTemp->mAngle3));
	vectoangles(axis[0], axisAngles);
	VectorAdd(angles, axisAngles, angles);
	VectorSet(angleDelta, FxRange_GetVal(&primTemp->mAngle1Delta), FxRange_GetVal(&primTemp->mAngle2Delta), FxRange_GetVal(&primTemp->mAngle3Delta));
	model = primTemp->mMediaHandles.GetHandle();
	emitFx = primTemp->mEmitterFxHandles.GetEffect();
	emitDistBase = FxRange_GetVal(&primTemp->mDensity);
	emitDistRand = FxRange_GetVal(&primTemp->mVariance);
	if ( prim->boltFrame.IsValid() )
	{
		orient = prim->boltFrame.Get()->GetOrientation();
		emitter->SetBoltOffset(orient->origin);
	}
	else
	{
		emitter->SetBoltOffset(0);
	}
	if ( !model.model )
		emitter->ClearFlags(0x10);
	emitter->GetTotalVelocityAtTime0(velocity);
	emitter->SetOrigin(newOrigin);
	emitter->SetEmitOrigin(newOrigin);
	emitter->SetEmitVelocity(velocity);
	emitter->SetAngles(angles);
	emitter->SetAngleDelta(angleDelta);
	emitter->SetModel(model.model);
	emitter->SetEmitFx(emitFx);
	emitter->SetEmitDistBase(emitDistBase);
	emitter->SetEmitDistRand(emitDistRand);
	emitter->SetEmitTime(theFxHelper->mTime);
	emitter->RandomizeEmitDist();
}

void FX_AddDecal( EffectPrimitive *prim, vec3_t axis[3], const vec3_t origin, int lateTime, int indexInBatch )
{
	vec3_t newOrigin;

	FX_CalcOriginAndAxis(prim, newOrigin, origin, axis);
	theFxScheduler->CreateDecalEffect(prim->primTemp, newOrigin, axis);
}

void FX_AddFxRunner( EffectPrimitive *prim, vec3_t axis[3], const vec3_t origin, int lateTime, int indexInBatch )
{
	vec3_t newOrigin;
	const PrimitiveTemplate *primTemp;

	FX_CalcOriginAndAxis(prim, newOrigin, origin, axis);
	primTemp = prim->primTemp;
	if ( prim->boltFrame.IsValid() )
		theFxScheduler->PlayEffect(primTemp->mPlayFxHandles.GetEffect(), newOrigin, 0, &prim->boltFrame.Get()->mBolt);
	else
		theFxScheduler->PlayEffect(primTemp->mPlayFxHandles.GetEffect(), newOrigin, axis, 0);
}

void FX_AddCameraShake( EffectPrimitive *prim, vec3_t axis[3], const vec3_t origin, int lateTime, int indexInBatch )
{
	vec3_t newOrigin;
	const PrimitiveTemplate *primTemp;

	FX_CalcOriginAndAxis(prim, newOrigin, origin, axis);
	primTemp = prim->primTemp;
	theFxHelper->CameraShake(newOrigin, FxRange_GetVal(&primTemp->mElasticity), (int)FxRange_GetVal(&primTemp->mRadius), (int)FxRange_GetVal(&primTemp->mLife));
}

void FX_AddLight( EffectPrimitive *prim, vec3_t axis[3], const vec3_t origin, int lateTime, int indexInBatch )
{
	Light *light;
	vec3_t newOrigin;
	bool added;
	const PrimitiveTemplate *primTemp;

	light = new Light;
	if ( !light )
		return;
	added = FX_AddPrimitive(prim, light, origin);
	if ( !added )
	{
		delete light;
		return;
	}
	FX_CalcOriginAndAxis(prim, newOrigin, origin, axis);
	light->SetOrigin(newOrigin);
	primTemp = prim->primTemp;
	if ( primTemp->mAttributeFlags & 0x2000 )
		light->SetColorBlendFactor(flrand(0.0f, 1.0f));
	if ( ( primTemp->mAttributeFlags & 0x8000 ) != 0 )
		light->SetSizeBlendFactor(flrand(0.0f, 1.0f));
}

void FX_AddOrientedParticle( EffectPrimitive *prim, vec3_t axis[3], const vec3_t origin, int lateTime, int indexInBatch )
{
	OrientedParticle *particle;
	vec3_t newOrigin;
	vec3_t normal;
	vec3_t localNormal;
	const orientation_t *orient;
	bool added;
	const PrimitiveTemplate *primTemp;

	particle = new OrientedParticle;
	if ( !particle )
		return;
	added = FX_AddPrimitive(prim, particle, origin);
	if ( !added )
	{
		delete particle;
		return;
	}
	FX_InitParticle(prim, particle, newOrigin, origin, axis, indexInBatch);
	primTemp = prim->primTemp;
	FX_SetMaterialAndSequenceParams(primTemp, particle, particle->GetDuration(), indexInBatch);
	FX_ApplyLateStart(primTemp, particle, newOrigin, lateTime);
	VectorCopy(axis[0], normal);
	if ( prim->boltFrame.IsValid() )
	{
		orient = prim->boltFrame.Get()->GetOrientation();
		OrientationDirToLocal(orient, normal, localNormal);
		particle->SetNormal(localNormal);
	}
	else
	{
		particle->SetNormal(normal);
	}
	particle->SetOrigin(newOrigin);
}

void FX_AddFlash( EffectPrimitive *prim, vec3_t axis[3], const vec3_t origin, int lateTime, int indexInBatch )
{
	Flash *flash;
	TMediaElement material;
	bool added;
	const PrimitiveTemplate *primTemp;

	flash = new Flash;
	if ( !flash )
		return;
	added = FX_AddPrimitive(prim, flash, origin);
	if ( !added )
	{
		delete flash;
		return;
	}
	primTemp = prim->primTemp;
	material = primTemp->mMediaHandles.GetHandle();
	flash->SetOrigin(origin);
	flash->SetMaterial(material.material);
	if ( primTemp->mAttributeFlags & 0x2000 )
		flash->SetColorBlendFactor(flrand(0.0f, 1.0f));
	FX_SetSortGroup(flash);
	flash->Init();
}

void FX_AddCloud( EffectPrimitive *prim, vec3_t axis[3], const vec3_t origin, int lateTime, int indexInBatch )
{
	Cloud *cloud;
	vec3_t newOrigin;
	bool added;
	const PrimitiveTemplate *primTemp;

	cloud = new Cloud;
	if ( !cloud )
		return;
	added = FX_AddPrimitive(prim, cloud, origin);
	if ( !added )
	{
		delete cloud;
		return;
	}
	FX_InitParticle(prim, cloud, newOrigin, origin, axis, indexInBatch);
	primTemp = prim->primTemp;
	FX_SetMaterialAndSequenceParams(primTemp, cloud, cloud->GetDuration(), indexInBatch);
	FX_ApplyLateStart(primTemp, cloud, newOrigin, lateTime);
	cloud->SetOrigin(newOrigin);
	cloud->mLengthBlendFactor = flrand(0.0f, 1.0f);
	cloud->mUseLength = primTemp->useLength;
}

// Original name unknown.
void FX_AddScheduledEffects()
{
	orientation_t orient;
	ScheduledEffect *sfx;
	ScheduledEffect **prevNext;
	const FxEffectDef *fx;
	const PrimitiveTemplate *primTemp;

	if ( !fx_enable->current.boolean )
		return;
	prevNext = &theFxScheduler->mScheduledHead;
	for ( sfx = *prevNext; sfx; sfx = *prevNext )
	{
		if ( sfx->mStartTime > theFxHelper->mTime )
		{
			prevNext = &sfx->mScheduledNext;
			continue;
		}
		fx = sfx->mFx;
		primTemp = fx->mPrimitives[sfx->mPrimIndex];
		Rand_Init(sfx->mSeed);
		*prevNext = sfx->mScheduledNext;
		theFxScheduler->mScheduledCount--;
		if ( sfx->mBolt.dobjHandle >= 0 )
		{
			if ( FX_GetBoneOrientation(&sfx->mBolt, &orient) )
				theFxScheduler->CreateEffect(fx, primTemp, &sfx->mBolt, orient.origin, orient.axis, theFxHelper->mTime - sfx->mStartTime, sfx->mIndexInBatch);
		}
		else
		{
			theFxScheduler->CreateEffect(fx, primTemp, &sfx->mBolt, sfx->mOrigin, sfx->mAxis, theFxHelper->mTime - sfx->mStartTime, sfx->mIndexInBatch);
		}
		delete sfx;
	}
}

// Original name unknown.
void FX_UpdateNonBolt()
{
	if ( !fx_enable->current.boolean )
		return;
	cullEffectCountNonBolt = 0;
	visibleEffectCountNonBolt = 0;
	FX_UpdateNewNonBolt(0);
	FX_CollectVisibleNonBolt();
}

// Original name unknown.
void FX_UpdateBolt()
{
	if ( !fx_enable->current.boolean )
		return;
	cullEffectCountBolt = 0;
	visibleEffectCountBolt = 0;
	FX_UpdateNewBolt(0);
	FX_CollectVisibleBolt();
}

// Original name unknown.
void FX_UpdateAndDraw()
{
	int count;

	if ( !fx_enable->current.boolean )
		return;
	FX_FreeRemovedNonBolt();
	FX_FreeRemovedBolt();
	FX_AddScheduledEffects();
	count = privateEffectActiveCountNonBolt;
	privateEffectActiveCountNonBolt = effectActiveCountNonBolt;
	FX_UpdateNewNonBolt(count);
	FX_FreeRemovedNonBolt();
	count = privateEffectActiveCountBolt;
	privateEffectActiveCountBolt = effectActiveCountBolt;
	FX_UpdateNewBolt(count);
	FX_FreeRemovedBolt();
	FX_DrawAll();
}

