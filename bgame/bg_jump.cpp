#include "../qcommon/qcommon.h"
#include "bg_public.h"

static dvar_t *jump_height;
static dvar_t *jump_stepSize;
static dvar_t *jump_slowdownEnable;
static dvar_t *jump_ladderPushVel;
static dvar_t *jump_spreadAdd;

#define JUMP_LAND_SLOWDOWN_TIME 1800

float Jump_GetSlowdownFriction( playerState_t *ps );

void Jump_RegisterDvars();
void Jump_ClearState( playerState_t *ps );
bool Jump_GetStepHeight( playerState_t *ps, const vec3_t origin, float *stepSize );
bool Jump_IsPlayerAboveMax( playerState_t *ps );
void Jump_ActivateSlowdown( playerState_t *ps );
void Jump_ApplySlowdown( playerState_t *ps );
float Jump_GetSlowdownFriction( playerState_t *ps );
float Jump_ReduceFriction( playerState_t *ps );
void Jump_ClampVelocity( playerState_t *ps, const vec3_t origin );
float Jump_GetLandFactor( playerState_t *ps );
void Jump_Start( pmove_t *pm, pml_t *pml, float height );
void Jump_PushOffLadder( playerState_t *ps, pml_t *pml );
void Jump_AddSurfaceEvent( playerState_t *ps, pml_t *pml );

/*
==================
Jump_RegisterDvars
==================
*/
void Jump_RegisterDvars()
{
	unsigned short flags;

	flags = DVAR_CODINFO;
	flags |= DVAR_CHEAT | DVAR_CHANGEABLE_RESET;

	jump_height = Dvar_RegisterFloat("jump_height", 39, 0, 128, flags);
	jump_stepSize = Dvar_RegisterFloat("jump_stepSize", 18, 0, 64, flags);
	jump_slowdownEnable = Dvar_RegisterBool("jump_slowdownEnable", true, flags);
	jump_ladderPushVel = Dvar_RegisterFloat("jump_ladderPushVel", 128, 0, 1024, flags);
	jump_spreadAdd = Dvar_RegisterFloat("jump_spreadAdd", 64, 0, 512, flags);
}

/*
==================
Jump_ClearState
==================
*/
void Jump_ClearState( playerState_t *ps )
{
	ps->pm_flags &= ~PMF_TIME_LAND;
	ps->jumpOriginZ = 0;
}

/*
==================
Jump_GetStepHeight
==================
*/
bool Jump_GetStepHeight( playerState_t *ps, const vec3_t origin, float *stepSize )
{
	assert(ps->pm_flags & PMF_TIME_LAND);
	assert(origin);
	assert(stepSize);

	if ( origin[2] < ps->jumpOriginZ + jump_height->current.decimal )
	{
		*stepSize = jump_stepSize->current.decimal;

		if ( origin[2] + *stepSize > ps->jumpOriginZ + jump_height->current.decimal )
		{
			*stepSize = ps->jumpOriginZ + jump_height->current.decimal - origin[2];
		}

		return true;
	}

	return false;
}

/*
==================
Jump_IsPlayerAboveMax
==================
*/
bool Jump_IsPlayerAboveMax( playerState_t *ps )
{
	assert(ps->pm_flags & PMF_TIME_LAND);
	return ps->origin[2] >= ps->jumpOriginZ + jump_height->current.decimal;
}

/*
==================
Jump_ActivateSlowdown
==================
*/
void Jump_ActivateSlowdown( playerState_t *ps )
{
	if ( !ps->pm_time )
	{
		ps->pm_flags |= PMF_TIME_LAND;
		ps->pm_time = JUMP_LAND_SLOWDOWN_TIME;
	}
}

/*
==================
Jump_ApplySlowdown
==================
*/
void Jump_ApplySlowdown( playerState_t *ps )
{
	float scale;

	assert(ps->pm_flags & PMF_TIME_LAND);

	scale = 1.0f;

	if ( ps->pm_time > JUMP_LAND_SLOWDOWN_TIME )
	{
		Jump_ClearState(ps);
		scale = 0.64999998f;
	}
	else if ( !ps->pm_time )
	{
		if ( ps->origin[2] < ps->jumpOriginZ + 18.0f )
		{
			ps->pm_time = JUMP_LAND_SLOWDOWN_TIME;
			scale = 0.64999998f;
		}
		else
		{
			ps->pm_time = JUMP_LAND_SLOWDOWN_TIME - FRAMETIME - 500;
			scale = 0.5f;
		}
	}

	if ( !jump_slowdownEnable->current.boolean )
	{
		scale = 1.0f;
	}

	VectorScale(ps->velocity, scale, ps->velocity);
}

/*
==================
Jump_GetSlowdownFriction
==================
*/
float Jump_GetSlowdownFriction( playerState_t *ps )
{
	assert(ps->pm_flags & PMF_TIME_LAND);
	assert(ps->pm_time <= JUMP_LAND_SLOWDOWN_TIME);

	if ( !jump_slowdownEnable->current.boolean )
	{
		return 1.0f;
	}

	if ( ps->pm_time >= JUMP_LAND_SLOWDOWN_TIME - FRAMETIME )
	{
		return 2.5f;
	}

	return ps->pm_time * 1.5f * 0.00058823527f + 1.0f;
}

/*
==================
Jump_ReduceFriction
==================
*/
float Jump_ReduceFriction( playerState_t *ps )
{
	float result;

	if ( ps->pm_time <= JUMP_LAND_SLOWDOWN_TIME )
	{
		result = Jump_GetSlowdownFriction( ps );
	}
	else
	{
		Jump_ClearState( ps );
		result = 1.0f;
	}

	return result;
}

/*
==================
Jump_ClampVelocity
==================
*/
void Jump_ClampVelocity( playerState_t *ps, const vec3_t origin )
{
	float heightDiff;
	float maxJumpVel;

	assert(ps->pm_flags & PMF_TIME_LAND);
	assert(origin);

	if ( ps->origin[2] - origin[2] > 0 )
	{
	}
	else
	{
		return;
	}

	heightDiff = ps->jumpOriginZ + jump_height->current.decimal - ps->origin[2];

	if ( heightDiff < 0.1f )
	{
		ps->velocity[2] = 0;
		return;
	}

	maxJumpVel = I_sqrt(ps->gravity * (heightDiff * 2.0f));

	if ( ps->velocity[2] > maxJumpVel )
	{
		ps->velocity[2] = maxJumpVel;
	}
}

/*
==================
Jump_GetLandFactor
==================
*/
float Jump_GetLandFactor( playerState_t *ps )
{
	assert(ps->pm_flags & PMF_TIME_LAND);
	assert(ps->pm_time <= JUMP_LAND_SLOWDOWN_TIME);

	if ( !jump_slowdownEnable->current.boolean )
	{
		return 1.0f;
	}

	if ( ps->pm_time >= JUMP_LAND_SLOWDOWN_TIME - FRAMETIME )
	{
		return 2.5f;
	}

	return ps->pm_time * 1.5f * 0.00058823527f + 1.0f;
}

/*
==================
Jump_Start
==================
*/
void Jump_Start( pmove_t *pm, pml_t *pml, float height )
{
	float velocitySqrd;
	float factor;
	playerState_t *ps;

	ps = pm->ps;

	velocitySqrd = (height + height) * ps->gravity;

	if ( ps->pm_flags & PMF_TIME_LAND && ps->pm_time <= JUMP_LAND_SLOWDOWN_TIME )
	{
		factor = Jump_GetLandFactor(ps);
		velocitySqrd = velocitySqrd / factor;
	}

	pml->groundPlane = qfalse;
	pml->almostGroundPlane = qfalse;
	pml->walking = qfalse;

	ps->groundEntityNum = ENTITYNUM_NONE;
	ps->jumpTime = pm->cmd.serverTime;
	ps->jumpOriginZ = ps->origin[2];
	ps->pm_flags |= PMF_TIME_LAND;
	ps->pm_time = 0;
	ps->velocity[2] = I_sqrt(velocitySqrd);
	ps->aimSpreadScale = ps->aimSpreadScale + jump_spreadAdd->current.decimal;

	if ( ps->aimSpreadScale > 255 )
	{
		ps->aimSpreadScale = 255;
	}
}

/*
==================
Jump_PushOffLadder
==================
*/
void Jump_PushOffLadder( playerState_t *ps, pml_t *pml )
{
	vec3_t pushOffDir;
	vec3_t flatForward;
	float dot;

	assert(ps->pm_flags & PMF_LADDER);

	ps->velocity[2] = ps->velocity[2] * 0.75f;

	VectorSet(flatForward, pml->forward[0], pml->forward[1], 0);
	Vec3Normalize(flatForward);

	dot = DotProduct(ps->vLadderVec, pml->forward);

	if ( dot < 0 )
	{
		dot = DotProduct(flatForward, ps->vLadderVec);

		VectorMA(flatForward, dot * -2.0f, ps->vLadderVec, pushOffDir);
		Vec3Normalize(pushOffDir);
	}
	else
	{
		VectorCopy(flatForward, pushOffDir);
	}

	Vec2Scale(pushOffDir, jump_ladderPushVel->current.decimal, ps->velocity);
	ps->pm_flags &= ~PMF_LADDER;
}

/*
==================
Jump_AddSurfaceEvent
==================
*/
void Jump_AddSurfaceEvent( playerState_t *ps, pml_t *pml )
{
	if ( (ps->pm_flags & PMF_LADDER) )
	{
		PM_AddEvent(ps, EV_JUMP_WOOD);
		return;
	}

	int surfType = PM_GroundSurfaceType(pml);

	if ( !surfType )
	{
		return;
	}

	PM_AddEvent(ps, EV_JUMP_DEFAULT + surfType);
}

/*
==================
Jump_Start
==================
*/
bool Jump_Check( pmove_t *pm, pml_t *pml )
{
	playerState_t *ps;

	assert(pm);
	ps = pm->ps;
	assert(ps);

	if ( pm->cmd.serverTime - ps->jumpTime < 500 )
	{
		return false;
	}

	if ( ps->pm_flags & PMF_RESPAWNED )
	{
		return false;
	}

	if ( ps->pm_flags & PMF_MANTLE )
	{
		return false;
	}

	if ( ps->pm_type >= PM_DEAD )
	{
		return false;
	}

	if ( PM_GetEffectiveStance(ps) )
	{
		return false;
	}

	if ( !(pm->cmd.buttons & BUTTON_JUMP) )
	{
		return false;
	}

	if ( pm->oldcmd.buttons & BUTTON_JUMP )
	{
		pm->cmd.buttons &= ~BUTTON_JUMP;
		return false;
	}

	Jump_Start(pm, pml, jump_height->current.decimal);
	Jump_AddSurfaceEvent(ps, pml);

	if ( ps->pm_flags & PMF_LADDER )
	{
		Jump_PushOffLadder(ps, pml);
	}

	if ( pm->cmd.forwardmove >= 0 )
	{
		BG_AnimScriptEvent(ps, ANIM_ET_JUMP, qfalse, qtrue);
	}
	else
	{
		BG_AnimScriptEvent(ps, ANIM_ET_JUMPBK, qfalse, qtrue);
	}

	return true;
}

