#include "../qcommon/qcommon.h"

struct GfxEntity;
struct Material;
struct GfxWorldVertex;

// Renderer exports; only the slots used here (client re 0x60f080).
struct refexport_t
{
	void *unused0[10];
	void (*SetIgnorePrecacheErrors)( int ignore );
	void *unused11[2];
	const char *(*GetMaterialName)( Material *material );
	int (*GetMaterialSubimageCount)( Material *material );
	bool (*IsMaterialRefractive)( Material *material );
	void *unused16[4];
	GfxEntity *(*AddRefEntityToScene)( const GfxEntity *refEnt, XModel *model, const void *cent );
	void *unused21[4];
	void (*AddPolyToScene)( Material *material, int lmapIndex, int vertCount, const GfxWorldVertex *verts );
	void (*AddLightToScene)( const float *org, float radius, float r, float g, float b );
	void *unused27[43];
	void (*AddPlume)( const float *origin, int score, const float *color, int duration );
};
extern refexport_t re;

// Only the leading fields are read here.
struct refdef_s
{
	int x;
	int y;
	int width;
	int height;
	float fov_x;
	float fov_y;
	vec3_t vieworg;
	vec3_t viewaxis[3];
};
typedef refdef_s refdef_t;

#include "FxCamera.h"

class FxArchive
{
public:
	bool IsReading();
	void ArchiveInt( int *value );
};

#include "FxHelper.h"

extern FxHelper *theFxHelper;
extern dvar_t *fx_freeze;

void FX_AddScheduledEffects();
void FX_UpdateAllNonBolt();
void FX_UpdateAllBolt();

void FxHelper::Init()
{
	time = 0;
	mTime = 0;
	mOldTime = 0;
	mFrameTime = 0;
	mTimeFrozen = 0;
	mSeed = 0;
}

FxHelper::FxHelper()
{
	mTime = 0;
	mOldTime = 0;
	mFrameTime = 0;
	mTimeFrozen = 0;
}

void FxHelper::AdjustTime( int intime )
{
	if ( fx_freeze->current.boolean )
	{
		mFrameTime = 0;
	}
	else
	{
		if ( !time )
		{
			mFrameTime = 0;
		}
		else
		{
			mFrameTime = intime - time;
			if ( mFrameTime < 0 )
				mFrameTime = 0;
			else if ( mFrameTime > 200 )
				mFrameTime = 200;
		}
		mOldTime = mTime;
		mTime += mFrameTime;
	}
	time = intime;
	mSeed = time;
	mPrevCamera = mCamera;
}

void FxHelper::WarpTime( int intime )
{
	int frameTime;

	if ( fx_freeze->current.boolean )
	{
		mFrameTime = 0;
		time = intime;
		mSeed = intime;
		return;
	}
	if ( !time )
	{
		mFrameTime = 0;
		time = intime;
		mSeed = intime;
		return;
	}
	mOldTime = mTime;
	frameTime = intime - mTime;
	if ( frameTime < 0 )
	{
		mTime = intime;
		time = intime;
		mSeed = intime;
		return;
	}
	mFrameTime = 0;
	while ( frameTime > 200 )
	{
		mFrameTime = 200;
		FX_AddScheduledEffects();
		FX_UpdateAllNonBolt();
		FX_UpdateAllBolt();
		mOldTime = mTime;
		mTime += mFrameTime;
		frameTime -= 200;
	}
	mFrameTime = frameTime;
	FX_AddScheduledEffects();
	FX_UpdateAllNonBolt();
	FX_UpdateAllBolt();
	mTime += mFrameTime;
	time = intime;
	mSeed = intime;
}

void FxHelper::CalcFrustumPlanes( const refdef_t *refdef, float zfar )
{
	int i;
	float halfFov;
	float s;
	float c;

	VectorCopy(refdef->viewaxis[0], mCamera.frustum[0]);
	halfFov = refdef->fov_x * 0.008726646259971648f;
	FastSinCos(halfFov, &s, &c);
	VectorScale(refdef->viewaxis[0], s, mCamera.frustum[1]);
	VectorMA(mCamera.frustum[1], c, refdef->viewaxis[1], mCamera.frustum[1]);
	VectorScale(refdef->viewaxis[0], s, mCamera.frustum[2]);
	VectorMA(mCamera.frustum[2], -c, refdef->viewaxis[1], mCamera.frustum[2]);
	halfFov = refdef->fov_y * 0.008726646259971648f;
	FastSinCos(halfFov, &s, &c);
	VectorScale(refdef->viewaxis[0], s, mCamera.frustum[3]);
	VectorMA(mCamera.frustum[3], c, refdef->viewaxis[2], mCamera.frustum[3]);
	VectorScale(refdef->viewaxis[0], s, mCamera.frustum[4]);
	VectorMA(mCamera.frustum[4], -c, refdef->viewaxis[2], mCamera.frustum[4]);
	mCamera.numPlanes = 5;
	if ( zfar > 0.0f )
	{
		VectorNegate(refdef->viewaxis[0], mCamera.frustum[5]);
		mCamera.numPlanes = 6;
	}
	for ( i = 0; i < mCamera.numPlanes; i++ )
		mCamera.frustum[i][3] = DotProduct(mCamera.vieworg, mCamera.frustum[i]);
	if ( zfar > 0.0f )
		mCamera.frustum[5][3] = mCamera.frustum[5][3] - zfar;
}

void FxHelper::AdjustCamera( const refdef_t *refdef, float zfar )
{
	VectorCopy(refdef->vieworg, mCamera.vieworg);
	CalcFrustumPlanes(refdef, zfar);
	if ( refdef->fov_x != 80.0f )
	{
		float halfFov = refdef->fov_x * 0.5f;
		adsZoomFactor = tan((double)0.69813174f) / tan(halfFov * (float)(M_PI / 180.0));
	}
	else
		adsZoomFactor = 1.0f;
}

bool FxHelper::CullSphere( const vec3_t worldPos, float radius, int planeCount )
{
	int i;
	float dist;

	for ( i = 0; i < planeCount; i++ )
	{
		dist = DotProduct(theFxHelper->mCamera.frustum[i], worldPos) - theFxHelper->mCamera.frustum[i][3];
		if ( dist < -radius )
			return true;
	}
	return false;
}

bool FxHelper::CullSpherePreviousFrame( const vec3_t worldPos, float radius )
{
	int i;
	float dist;

	for ( i = 0; i < theFxHelper->mPrevCamera.numPlanes; i++ )
	{
		dist = DotProduct(theFxHelper->mPrevCamera.frustum[i], worldPos) - theFxHelper->mPrevCamera.frustum[i][3];
		if ( dist < -radius )
			return true;
	}
	return false;
}

bool FxHelper::CullCylinder( const vec3_t worldPos0, const vec3_t worldPos1, float radius0, float radius1, int planeCount )
{
	int i;
	float dist;

	for ( i = 0; i < planeCount; i++ )
	{
		dist = DotProduct(theFxHelper->mCamera.frustum[i], worldPos0) - theFxHelper->mCamera.frustum[i][3];
		if ( dist > -radius0 )
			continue;
		dist = DotProduct(theFxHelper->mCamera.frustum[i], worldPos1) - theFxHelper->mCamera.frustum[i][3];
		if ( dist > -radius1 )
			continue;
		return true;
	}
	return false;
}

void FxHelper::Trace( trace_t *tr, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int skipEntNum, int flags )
{
	CM_BoxTrace(tr, start, end, mins, maxs, 0, flags);
	tr->entityNum = tr->fraction != 1.0 ? ENTITYNUM_NONE - 1 : ENTITYNUM_NONE;
}

void FxHelper::AddFxToScene( GfxEntity *ent, XModel *model )
{
	re.AddRefEntityToScene(ent, model, NULL);
}

void FxHelper::SetIgnorePrecacheErrors( bool ignore )
{
	re.SetIgnorePrecacheErrors(ignore);
}

void FxHelper::AddPlume( const float *origin, int score, const float *color, int duration )
{
	re.AddPlume(origin, score, color, duration);
}

const char *FxHelper::GetMaterialName( Material *material )
{
	return re.GetMaterialName(material);
}

void FxHelper::Archive( FxArchive *arch )
{
	arch->ArchiveInt(&mTime);
	arch->ArchiveInt(&mOldTime);
	arch->ArchiveInt(&mFrameTime);
	arch->ArchiveInt(&mTimeFrozen);
	if ( arch->IsReading() )
		time = 0;
}

int FxHelper::GetMaterialSubimageCount( Material *material )
{
	return re.GetMaterialSubimageCount(material);
}

bool FxHelper::IsMaterialRefractive( Material *material )
{
	return re.IsMaterialRefractive(material);
}

void FxHelper::AddLightToScene( const vec3_t org, float radius, float red, float green, float blue )
{
	re.AddLightToScene(org, radius, red, green, blue);
}

// Never called; name follows the re slot (lmap 0x1f = none).
void FxHelper::AddPolyToScene( Material *material, int vertCount, const GfxWorldVertex *verts )
{
	re.AddPolyToScene(material, 0x1f, vertCount, verts);
}

void FxHelper::CameraShake( const vec3_t origin, float intensity, int radius, int time )
{
}

int FxHelper::GetSeed()
{
	int seed;

	seed = mSeed;
	mSeed *= 0x369D035;
	return seed;
}

