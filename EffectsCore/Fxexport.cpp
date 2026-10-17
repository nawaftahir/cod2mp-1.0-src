#include "../qcommon/qcommon.h"

struct FxEffectDef;
struct refdef_s;
typedef refdef_s refdef_t;

struct FxBoltInfo
{
	int dobjHandle;
	int boneIndex;
};

#include "FxHelper.h"

struct FxScheduler
{
	void PlayEffect( const FxEffectDef *fx, const vec3_t origin );
	void PlayEffect( const FxEffectDef *fx, const vec3_t origin, const vec3_t dir );
	void PlayEffect( const FxEffectDef *fx, const vec3_t origin, const vec3_t forward, const vec3_t up );
	void PlayEffect( const FxEffectDef *fx, const vec3_t origin, const vec3_t axis[3], const FxBoltInfo *bolt );
	float GetEffectLength( const FxEffectDef *fx );
};

extern FxHelper *theFxHelper;
extern FxScheduler *theFxScheduler;
// Original name unknown.
extern FxScheduler *theFxSchedulers[1];

volatile qboolean fx_camera_valid;
extern int effectActiveCountBolt;
extern int effectActiveCountNonBolt;
extern int privateEffectActiveCountBolt;
extern int privateEffectActiveCountNonBolt;

DObj *Com_GetClientDObj( int handle, int localClientNum );
int DObjGetBoneIndex( const DObj *obj, unsigned int name );
int Net_LocalClientNum();
int FX_Init( bool rendererExists );
void FX_Free( bool bRemoveTemplates );

// Nothing to switch on the dedicated server. Original name unknown.
static void FX_SwitchToScheduler( int index )
{
}

void Server_SwitchToValidFxScheduler()
{
	int i;

	for ( i = 0; i < 1; i++ )
	{
		if ( theFxSchedulers[i] )
		{
			FX_SwitchToScheduler(i);
			return;
		}
	}
}

int FX_GetBoneIndex( int entNum, unsigned int bone )
{
	DObj *obj;

	obj = Com_GetClientDObj(entNum, Net_LocalClientNum());
	if ( !obj )
		return -1;
	return DObjGetBoneIndex(obj, bone);
}

void FX_PlaySimpleEffect( const FxEffectDef *fx, const float *origin )
{
	theFxScheduler->PlayEffect(fx, origin);
}

void FX_PlayEffect( const FxEffectDef *fx, const float *origin, const float *dir )
{
	theFxScheduler->PlayEffect(fx, origin, dir);
}

// Original name unknown.
void FX_PlayOrientedEffect( const FxEffectDef *fx, const float *origin, const float *forward, const float *up )
{
	theFxScheduler->PlayEffect(fx, origin, forward, up);
}

void FX_PlayEntityEffect( const FxEffectDef *fx, const float *origin, const vec3_t *axis, const FxBoltInfo *bolt )
{
	theFxScheduler->PlayEffect(fx, origin, axis, bolt);
}

int FX_InitSystem( bool rendererExists )
{
	return FX_Init(rendererExists);
}

void FX_FreeSystem()
{
	FX_Free(true);
}

void FX_FreeActive()
{
	FX_Free(false);
}

void FX_AdjustCamera( const refdef_t *refdef, float zfar )
{
	theFxHelper->AdjustCamera(refdef, zfar);
	fx_camera_valid = qtrue;
}

void FX_AdjustTime( int time )
{
	fx_camera_valid = qfalse;
	theFxHelper->AdjustTime(time);
	privateEffectActiveCountBolt = effectActiveCountBolt;
	privateEffectActiveCountNonBolt = effectActiveCountNonBolt;
}

void FX_WarpTime( int time )
{
	theFxHelper->WarpTime(time);
}

float FX_GetEffectLength( const FxEffectDef *fx )
{
	return theFxScheduler->GetEffectLength(fx);
}

