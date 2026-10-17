#include "../qcommon/qcommon.h"
#include "bg_public.h"

void CG_Trace( trace_t *results, const float *start, const float *mins, const float *maxs, const float *end, int skipNumber, int mask );
int CG_PointContents( const float *point, int passEntityNum, int contentMask );

static viewLerpWaypoint_s viewLerp_StandCrouch[] =
{
	{ 0, 60, 0},
	{ 1, 59.5, 0},
	{ 4, 58.5, 0},
	{ 30, 56, 0},
	{ 80, 44, 0},
	{ 90, 41.5, 0},
	{ 95, 40.5, 0},
	{ 100, 40, 0},
	{ -1, 0, 0},
};

static viewLerpWaypoint_s viewLerp_CrouchStand[] =
{
	{ 0, 40, 0},
	{ 5, 40.5, 0},
	{ 10, 41.5, 0},
	{ 20, 44, 0},
	{ 70, 56, 0},
	{ 96, 58.5, 0},
	{ 99, 59.5, 0},
	{ 100, 60, 0},
	{ -1, 0, 0},
};

static viewLerpWaypoint_s viewLerp_CrouchProne[] =
{
	{ 0, 40, 0},
	{ 11, 38, 0},
	{ 22, 33, 0},
	{ 34, 25, 0},
	{ 45, 16, 0},
	{ 50, 15, 0},
	{ 55, 16, 0},
	{ 70, 18, 0},
	{ 90, 17, 0},
	{ 100, 11, 0},
	{ -1, 0, 0},
};

// unreferenced
static viewLerpWaypoint_s viewLerp_CrouchProneLinear[] =
{
	{ 0, 40, 0},
	{ 100, 11, 0},
	{ -1, 0, 0},
};

static viewLerpWaypoint_s viewLerp_ProneCrouch[] =
{
	{ 0, 11, 0},
	{ 5, 10, 0},
	{ 30, 21, 0},
	{ 50, 25, 0},
	{ 67, 31, 0},
	{ 83, 34, 0},
	{ 100, 40, 0},
	{ -1, 0, 0},
};

pmoveHandler_t pmoveHandlers[] =
{
	{ CG_Trace, CG_PointContents, NULL },
	{ G_TraceCapsule, SV_PointContents, G_PlayerEvent },
};

// unreferenced
const vec3_t pm_unusedVec0 = { 0.5f, 0.5f, 0.69999999f };
const vec3_t pm_unusedVec1 = { 19.0f, 12.0f, 9.0f };
const vec3_t pm_unusedVec2 = { 1.0f, 4.0f, 8.0f };
const vec3_t pm_unusedVec3 = { 1.0f, 16.0f, 5.0f };
const float pm_unusedScale = 0.40000001f;

const vec3_t CorrectSolidDeltas[] =
{
	{ 0, 0, 1 },
	{ -1, 0, 1 },
	{ 0, -1, 1 },
	{ 1, 0, 1 },
	{ 0, 1, 1 },
	{ -1, 0, 0 },
	{ 0, -1, 0 },
	{ 1, 0, 0 },
	{ 0, 1, 0 },
	{ 0, 0, -1 },
	{ -1, 0, -1 },
	{ 0, -1, -1 },
	{ 1, 0, -1 },
	{ 0, 1, -1 },
	{ -1, -1, 1 },
	{ 1, -1, 1 },
	{ 1, 1, 1 },
	{ -1, 1, 1 },
	{ -1, -1, 0 },
	{ 1, -1, 0 },
	{ 1, 1, 0 },
	{ -1, 1, 0 },
	{ -1, -1, -1 },
	{ 1, -1, -1 },
	{ 1, 1, -1 },
	{ -1, 1, -1 },
};

void PM_AddTouchEnt( pmove_t *pm, int entityNum );
float PM_GetViewHeightLerp(const pmove_t *pm, int iFromHeight, int iToHeight);
void PM_UpdateLean(playerState_t *ps, float msec, usercmd_t *cmd, void (*capsuleTrace)(trace_t *, const float *, const float *, const float *, const float *, int, int));

void PM_AddEvent( playerState_t *ps, int newEvent );
void PM_AddTouchEnt( pmove_t *pm, int entityNum );
void PM_ClipVelocity( const vec3_t in, const vec3_t normal, vec3_t out );
int PM_GetEffectiveStance( const playerState_t *ps );
static void PM_Friction( playerState_t *ps, pml_t *pml );
bool PM_DoPlayerInertia( playerState_t *ps, float accelspeed, const vec3_t wishdir );
float PM_PlayerInertia( playerState_t *ps, float accelspeed, const vec3_t wishdir );
void PM_Accelerate( playerState_t *ps, const pml_t *pml, vec3_t wishdir, float wishspeed, float accel );
static float PM_MoveScale( playerState_t *ps, float fmove, float rmove, float umove );
static float PM_CmdScale( playerState_t *ps, usercmd_t *cmd );
static float PM_CmdScale_Walk( pmove_t *pm, usercmd_t *cmd );
float PM_DamageScale_Walk( int damage_timer );
static void PM_SetMovementDir( pmove_t *pm, pml_t *pml );
int PM_GroundSurfaceType( pml_t *pml );
void PM_FlyMove( pmove_t *pm, pml_t *pml );
void PM_AirMove( pmove_t *pm, pml_t *pml );
void PM_WalkMove( pmove_t *pm, pml_t *pml );
static void PM_DeadMove( playerState_t *ps, pml_t *pml );
void PM_NoclipMove( pmove_t *pm, pml_t *pml );
void PM_UFOMove( pmove_t *pm, pml_t *pml );
int PM_FootstepForSurface( playerState_t *ps, pml_t *pml );
int PM_MediumLandingForSurface( pml_t *pml );
int PM_LightLandingForSurface( pml_t *pml );
int PM_HardLandingForSurface( pml_t *pml );
int PM_DamageLandingForSurface( pml_t *pml );
static void PM_CrashLand(playerState_t *ps, pml_t *pml);
static qboolean PM_CorrectAllSolid( pmove_t *pm, pml_t *pml, trace_t *trace );
static void PM_GroundTraceMissed(pmove_t *pm, pml_t *pml);
void PM_GroundTrace( pmove_t *pm, pml_t *pml );
int PM_GetViewHeightLerpTime( const playerState_t *ps, int iTarget, int bDown );
float PM_ViewHeightTableLerp( int iFrac, viewLerpWaypoint_s *pTable, float *pfPosOfs );
float PM_GetViewHeightLerp(const pmove_t *pm, int iFromHeight, int iToHeight);
void PM_ViewHeightAdjust( pmove_t *pm, pml_t *pml );
void PM_CheckDuck( pmove_t *pm, pml_t *pml );
void PM_FootstepEvent( pmove_t *pm, pml_t *pml, int iOldBobCycle, int iNewBobCycle, qboolean bFootStep );
qboolean PM_ShouldMakeFootsteps( pmove_t *pm );
int PM_GetFlinchAnim( float yaw );
void PM_Footsteps( pmove_t *pm, pml_t *pml );
void PM_FoliageSounds( pmove_t *pm );
static void PM_DropTimers( playerState_t *ps, pml_t *pml );
qboolean BG_CheckProneTurned( playerState_t *ps, float newProneYaw, byte handler );
void PM_UpdateViewAngles( playerState_t *ps, float msec, usercmd_t *cmd, byte handler );
void PM_UpdatePronePitch( pmove_t *pm, pml_t *pml );
void PM_SetProneMovementOverride( playerState_t *ps );
void PM_UpdatePlayerWalkingFlag( pmove_t *pm );
void PM_SetLadderFlag( playerState_t *ps );
void PM_ClearLadderFlag( playerState_t *ps );
void PM_CheckLadderMove( pmove_t *pm, pml_t *pml );
void PM_LadderMove( pmove_t *pm, pml_t *pml );
void PmoveSingle( pmove_t *pm );
void Pmove( pmove_t *pmove );
float BG_GetSpeed( const playerState_t *ps, int iTime );

/*
==============
PM_Accelerate

Handles user intended acceleration
==============
*/

/*
==============
PM_CheckDuck

Sets mins, maxs, and pm->ps->viewheight
==============
*/

/*
===============
PM_FootstepEvent
===============
*/

/*
===============
PM_Footsteps
===============
*/

/*
================
PM_UpdateViewAngles

This can be used as another entry point when only the viewangles
are being updated isntead of a full move

	!! NOTE !! Any changes to mounted/prone view should be duplicated in BotEntityWithinView()
================
*/


/*
================
PM_SetLadderFlag
================
*/


/*
===============
PM_trace
===============
*/
void PM_trace( pmove_t *pm, trace_t *results, const vec3_t start,
               const vec3_t mins, const vec3_t maxs, const vec3_t end,
               int passEntityNum, int contentMask )
{
	pmoveHandlers[pm->handler].trace(results, start, mins, maxs, end, passEntityNum, contentMask);
}

/*
===============
PM_playerTrace
===============
*/
void PM_playerTrace( pmove_t *pm, trace_t *results, const vec3_t start,
                     const vec3_t mins, const vec3_t maxs, const vec3_t end,
                     int passEntityNum, int contentMask )
{
	pmoveHandlers[pm->handler].trace(results, start, mins, maxs, end, passEntityNum, contentMask);

	if ( !results->startsolid )
	{
		return;
	}

	if ( !(results->contents & CONTENTS_BODY) )
	{
		return;
	}

	PM_AddTouchEnt(pm, results->entityNum);
	pm->tracemask &= ~CONTENTS_BODY;
	pmoveHandlers[pm->handler].trace(results, start, mins, maxs, end, passEntityNum, contentMask & ~CONTENTS_BODY);
}

/*
===============
PM_AddEvent
===============
*/
void PM_AddEvent( playerState_t *ps, int newEvent )
{
	BG_AddPredictableEventToPlayerstate(newEvent, 0, ps);
}

/*
===============
PM_AddTouchEnt
===============
*/
void PM_AddTouchEnt( pmove_t *pm, int entityNum )
{
	int i;

	if ( entityNum == ENTITYNUM_WORLD )
	{
		return;
	}
	if ( pm->numtouch == MAXTOUCH )
	{
		return;
	}

	// see if it is already added
	for ( i = 0 ; i < pm->numtouch ; i++ )
	{
		if ( pm->touchents[ i ] == entityNum )
		{
			return;
		}
	}

	// add it
	pm->touchents[pm->numtouch] = entityNum;
	pm->numtouch++;
}

/*
==================
PM_ClipVelocity

Slide off of the impacting surface
==================
*/
void PM_ClipVelocity( const vec3_t in, const vec3_t normal, vec3_t out )
{
	float backoff;

	backoff = DotProduct(in, normal);
	backoff = backoff - I_fabs(backoff) * 0.001f;

	VectorMA(in, -backoff, normal, out);
}

/*
===============
PM_GetEffectiveStance
===============
*/
int PM_GetEffectiveStance( const playerState_t *ps )
{
	if ( ps->viewHeightTarget == CROUCH_VIEWHEIGHT )
	{
		return PM_EFF_STANCE_CROUCH;
	}

	if ( ps->viewHeightTarget == PRONE_VIEWHEIGHT )
	{
		return PM_EFF_STANCE_PRONE;
	}

	return PM_EFF_STANCE_STAND;
}

/*
==================
PM_Friction

Handles both ground friction and water friction
==================
*/
static void PM_Friction( playerState_t *ps, pml_t *pml )
{
	vec3_t vec;
	float   *vel;
	float speed, newspeed, control;
	float drop;

	vel = ps->velocity;

	VectorCopy( vel, vec );
	if ( pml->walking )
	{
		vec[2] = 0; // ignore slope movement
	}

	speed = VectorLength( vec );
	// rain - #179 don't do this for PM_SPECTATOR/PM_NOCLIP, we always want them to stop
	if ( speed < 1  )
	{
		VectorClear(vel);     // allow sinking underwater
		// FIXME: still have z friction underwater?
		return;
	}

	drop = 0;

	// apply ground friction
	if ( pml->walking && !( pml->groundTrace.surfaceFlags & SURF_SLICK ) )
	{
		// if getting knocked back, no friction
		if ( !( ps->pm_flags & PMF_TIME_KNOCKBACK ) )
		{
			control = speed < stopspeed->current.decimal ? stopspeed->current.decimal : speed;

			if ( ps->pm_flags & PMF_TIME_SLIDE )
			{
				control *= 0.30000001f;
			}
			else if ( ps->pm_flags & PMF_TIME_LAND )
			{
				control *= Jump_ReduceFriction(ps);
			}

			drop += control * friction->current.decimal * pml->frametime;
		}
	}

	if ( ps->pm_type == PM_SPECTATOR )
	{
		drop += speed * 5 * pml->frametime;
	}

	// scale the velocity
	newspeed = speed - drop;
	if ( newspeed < 0 )
	{
		newspeed = 0;
	}
	newspeed /= speed;

	vel[0] = vel[0] * newspeed;
	vel[1] = vel[1] * newspeed;
	vel[2] = vel[2] * newspeed;
}

/*
=============
PM_DoPlayerInertia
=============
*/
bool PM_DoPlayerInertia( playerState_t *ps, float accelspeed, const vec3_t wishdir )
{
	float angle;
	vec2_t oldVel;
	vec2_t vel;

	VectorMA2(ps->velocity, accelspeed, wishdir, vel);
	Vector2Copy(ps->oldVelocity, oldVel);

	Vec2Normalize(oldVel);
	Vec2Normalize(vel);

	angle = Dot2Product(oldVel, vel);

	if ( angle >= inertiaAngle->current.decimal )
	{
		return false;
	}

	if ( inertiaDebug->current.boolean )
	{
		Com_Printf("angle is %f (oldVel is (%f,%f), vel is (%f, %f))\n", angle, oldVel[0], oldVel[1], vel[0], vel[1]);
		Com_Printf("clamping acceleration from %f to %f\n", accelspeed, inertiaMax->current.decimal);
	}

	return true;
}

/*
=============
PM_PlayerInertia
=============
*/
float PM_PlayerInertia( playerState_t *ps, float accelspeed, const vec3_t wishdir )
{
	if ( ps->pm_type == PM_NOCLIP )
	{
		return accelspeed;
	}

	if ( inertiaMax->current.decimal >= accelspeed )
	{
		return accelspeed;
	}

	if ( Vec2Multiply(ps->oldVelocity) < 0.0001 )
	{
		return accelspeed;
	}

	if ( !PM_DoPlayerInertia(ps, accelspeed, wishdir) )
	{
		return accelspeed;
	}

	return inertiaMax->current.decimal;
}

/*
============
PM_MoveScale

Returns the scale factor to apply to movements
This allows the clients to use axial -127 to 127 values for all directions
without getting a sqrt(2) distortion in speed.
============
*/
static float PM_MoveScale( playerState_t *ps, float fmove, float rmove, float umove )
{
	float max;
	float total;
	float scale;

	max = fabs(fmove);

	if ( fabs(rmove) > max )
		max = fabs(rmove);

	if ( fabs(umove) > max )
		max = fabs(umove);

	if ( max == 0.0f )
		return 0;

	total = I_sqrt( Square(fmove) + Square(rmove) + Square(umove) );
	scale = (float)ps->speed * max / ( total * 127.0f );

	if ( ps->pm_flags & PMF_ADS_WALK || ps->leanf != 0 )
		scale = scale * 0.40000001f;

	if ( ps->pm_type == PM_NOCLIP )
		scale = scale * 3;

	if ( ps->pm_type == PM_UFO )
		scale = scale * 6;

	if ( ps->pm_type == PM_SPECTATOR )
		scale = scale * player_spectateSpeedScale->current.decimal;

	return scale;
}

/*
============
PM_CmdScale

Returns the scale factor to apply to cmd movements
This allows the clients to use axial -127 to 127 values for all directions
without getting a sqrt(2) distortion in speed.
============
*/
static float PM_CmdScale( playerState_t *ps, usercmd_t *cmd )
{
	int max;
	float total;
	float scale;

	total = I_sqrt( Square(cmd->forwardmove) + Square(cmd->rightmove) );

	max = abs( cmd->forwardmove );

	if ( abs( cmd->rightmove ) > max )
		max = abs( cmd->rightmove );

	if ( !max )
		return 0;

	scale = (float)ps->speed * max / ( total * 127.0f );

	if ( ps->pm_flags & PMF_ADS_WALK || ps->leanf != 0 )
		scale = scale * 0.40000001f;

	if ( ps->pm_type == PM_NOCLIP )
		scale = scale * 3;

	if ( ps->pm_type == PM_UFO )
		scale = scale * 6;

	if ( ps->pm_type == PM_SPECTATOR )
		scale = scale * player_spectateSpeedScale->current.decimal;

	return scale;
}

/*
============
PM_CmdScale_Walk

Returns the scale factor to apply to cmd movements
This allows the clients to use axial -127 to 127 values for all directions
without getting a sqrt(2) distortion in speed.
============
*/
static float PM_CmdScale_Walk( pmove_t *pm, usercmd_t *cmd )
{
	float max;
	float fFwd;
	float fSide;
	float scale;
	float total;
	playerState_t *ps;
	float lerpFrac;
	int iStance;

	assert(pm);
	ps = pm->ps;
	assert(ps);

	total = I_sqrt( Square(cmd->forwardmove) + Square(cmd->rightmove) );

	if ( cmd->forwardmove < 0 )
		fFwd = I_fabs(cmd->forwardmove * player_backSpeedScale->current.decimal);
	else
		fFwd = I_fabs(cmd->forwardmove);

	fSide = I_fabs(cmd->rightmove * player_strafeSpeedScale->current.decimal);
	max = I_fmax(fFwd, fSide);

	if ( max == 0 )
	{
		return 0;
	}

	scale = (float)ps->speed * max / ( total * 127.0f );

	if ( ps->pm_flags & PMF_ADS_WALK || ps->leanf != 0 )
	{
		scale *= 0.40000001f;
	}

	if ( ps->pm_type == PM_NOCLIP )
	{
		scale *= 3.0f;
	}
	else if ( ps->pm_type == PM_UFO )
	{
		scale *= 6.0f;
	}
	else
	{
		lerpFrac = 0;
		iStance = PM_GetEffectiveStance(ps);
		lerpFrac = PM_GetViewHeightLerp(pm, CROUCH_VIEWHEIGHT, PRONE_VIEWHEIGHT);

		if ( lerpFrac != 0 )
		{
			scale *= (lerpFrac * 0.15000001f + (1.0f - lerpFrac) * 0.64999998f);
		}
		else
		{
			lerpFrac = PM_GetViewHeightLerp(pm, PRONE_VIEWHEIGHT, CROUCH_VIEWHEIGHT);

			if ( lerpFrac != 0 )
			{
				scale *= (lerpFrac * 0.64999998f + (1.0f - lerpFrac) * 0.15000001f);
			}
			else if ( iStance == PM_EFF_STANCE_PRONE )
			{
				scale *= 0.15000001f;
			}
			else if ( iStance == PM_EFF_STANCE_CROUCH )
			{
				scale *= 0.64999998f;
			}
		}
	}

	if ( ps->weapon && BG_GetWeaponDef(ps->weapon)->moveSpeedScale > 0 )
	{
		scale *= BG_GetWeaponDef(ps->weapon)->moveSpeedScale;
	}

	if ( cmd->buttons & BUTTON_ADS_LEGACY )
	{
		scale *= 0.40000001f;
	}

	return scale;
}

/*
===============
PM_DamageScale_Walk
===============
*/
float PM_DamageScale_Walk( int damage_timer )
{
	float result;
	float timer_max;
	float minScale;
	float scale;

	if ( !damage_timer )
	{
		return 1;
	}

	timer_max = player_dmgtimer_maxTime->current.decimal;

	if ( timer_max == 0 )
	{
		return 1;
	}

	minScale = player_dmgtimer_minScale->current.decimal;
	scale = -minScale / timer_max;
	result = damage_timer * scale + 1;

	return result;
}

/*
================
PM_SetMovementDir

Determine the rotation of the legs reletive
to the facing dir
================
*/
static void PM_SetMovementDir( pmove_t *pm, pml_t *pml )
{
	// Ridah, changed this for more realistic angles (at the cost of more network traffic?)
	float speed;
	vec3_t moved;
	int moveyaw;
	playerState_t *ps;

	assert(pm);
	ps = pm->ps;
	assert(ps);

	// prone move
	if ( ps->pm_flags & PMF_PRONE && !(ps->eFlags & EF_TURRET_ACTIVE) )
	{
		moveyaw = (int)AngleDelta(ps->proneDirection, ps->viewangles[YAW]);

		if ( abs( moveyaw ) > 90 )
		{
			if ( moveyaw > 0 )
			{
				moveyaw = 90;
			}
			else
			{
				moveyaw = -90;
			}
		}

		ps->movementDir = (signed char)moveyaw;
		return;
	}

	// ladder move
	if ( ps->pm_flags & PMF_LADDER )
	{
		speed = vectoyaw(ps->vLadderVec) + 180;
		moveyaw = (int)AngleDelta(speed, ps->viewangles[YAW]);

		if ( abs( moveyaw ) > 90 )
		{
			if ( moveyaw > 0 )
			{
				moveyaw = 90;
			}
			else
			{
				moveyaw = -90;
			}
		}

		ps->movementDir = (signed char)moveyaw;
		return;
	}

	VectorSubtract( ps->origin, pml->previous_origin, moved );

	if (    ( pm->cmd.forwardmove || pm->cmd.rightmove )
	        &&  ( ps->groundEntityNum != ENTITYNUM_NONE )
	        &&  ( speed = VectorLength( moved ) )
	        &&  ( speed > pml->frametime * 5 ) )   // if moving slower than 20 units per second, just face head angles
	{
		vec3_t dir;

		Vec3NormalizeTo( moved, dir );
		vectoangles( dir, dir );

		moveyaw = (int)AngleDelta( dir[YAW], ps->viewangles[YAW] );

		if ( pm->cmd.forwardmove < 0 )
		{
			moveyaw = (int)AngleNormalize180( moveyaw + 180.0f );
		}

		if ( abs( moveyaw ) > 90 )
		{
			if ( moveyaw > 0 )
			{
				moveyaw = 90;
			}
			else
			{
				moveyaw = -90;
			}
		}

		ps->movementDir = (signed char)moveyaw;
	}
	else
	{
		ps->movementDir = 0;
	}
}

/*
===============
PM_GroundSurfaceType
===============
*/
int PM_GroundSurfaceType( pml_t *pml )
{
	int surfType;
	int result;

	if ( pml->groundTrace.surfaceFlags & SURF_NOSTEPS )
	{
		result = 0;
	}
	else
	{
		surfType = (unsigned char)(( pml->groundTrace.surfaceFlags & 0x1f00000 ) >> 20);
		result = surfType;
	}

	return result;
}

/*
===================
PM_FlyMove
===================
*/
void PM_FlyMove( pmove_t *pm, pml_t *pml )
{
	int i;
	vec3_t wishvel;
	float wishspeed;
	vec3_t wishdir;
	float scale;
	playerState_t *ps;

	assert(pm);
	ps = pm->ps;
	assert(ps);

	// normal slowdown
	PM_Friction(ps, pml);

	scale = PM_CmdScale(ps, &pm->cmd);

	//
	// user intentions
	//
	if ( !scale )
	{
		wishvel[0] = 0;
		wishvel[1] = 0;
		wishvel[2] = 0;
	}
	else
	{
		for ( i = 0; i < 3; ++i )
		{
			wishvel[i] = scale * pml->forward[i] * pm->cmd.forwardmove + scale * pml->right[i] * pm->cmd.rightmove;
		}
	}

	if ( ps->speed )
	{
		scale = PM_MoveScale(ps, 0, 0, 127);

		if ( pm->cmd.buttons & BUTTON_LEANLEFT )
			wishvel[2] -= scale * 127;

		if ( pm->cmd.buttons & BUTTON_LEANRIGHT )
			wishvel[2] += scale * 127;
	}

	VectorCopy(wishvel, wishdir);
	wishspeed = Vec3Normalize(wishdir);

	PM_Accelerate(ps, pml, wishdir, wishspeed, 8);

	PM_StepSlideMove(pm, pml, qfalse);
}

/*
===============
PM_AirMove
===============
*/
void PM_AirMove( pmove_t *pm, pml_t *pml )
{
	int i;
	vec3_t wishvel;
	float fmove;
	float rmove;
	vec3_t wishdir;
	float wishspeed;
	float scale;
	usercmd_t cmd;
	playerState_t *ps;

	assert(pm);
	ps = pm->ps;
	assert(ps);

	PM_Friction(ps, pml);

	fmove = pm->cmd.forwardmove;
	rmove = pm->cmd.rightmove;

	cmd = pm->cmd;

	scale = PM_CmdScale(ps, &cmd);

	pml->forward[2] = 0;
	pml->right[2] = 0;

	Vec3Normalize(pml->forward);
	Vec3Normalize(pml->right);

	for ( i = 0; i < 2; i++ )
		wishvel[i] = pml->forward[i] * fmove + pml->right[i] * rmove;
	wishvel[2] = 0;

	VectorCopy(wishvel, wishdir);

	wishspeed = Vec3Normalize(wishdir);
	wishspeed *= scale;

	// not on ground, so little effect on velocity
	PM_Accelerate(ps, pml, wishdir, wishspeed, 1);

	// we may have a ground plane that is very steep, even
	// though we don't have a groundentity
	// slide along the steep plane
	if ( pml->groundPlane )
	{
		PM_ClipVelocity(ps->velocity, pml->groundTrace.normal, ps->velocity);
	}

	PM_StepSlideMove(pm, pml, qtrue);

	// Ridah, moved this down, so we use the actual movement direction
	// set the movementDir so clients can rotate the legs for strafing
	PM_SetMovementDir(pm, pml);
}

/*
===================
PM_WalkMove
===================
*/
void PM_WalkMove( pmove_t *pm, pml_t *pml )
{
	int i;
	vec3_t dir;
	vec3_t wishvel;
	float fmove;
	float rmove;
	vec3_t wishdir;
	float wishspeed;
	float scale;
	usercmd_t cmd;
	float acceleration;
	float vel;
	int iStance;
	playerState_t *ps;

	assert(pm);
	ps = pm->ps;
	assert(ps);

	if ( ps->pm_flags & PMF_TIME_LAND )
	{
		Jump_ApplySlowdown(ps);
	}

	if ( Jump_Check(pm, pml) )
	{
		// jumped away
		PM_AirMove(pm, pml);
		return;
	}

	PM_Friction(ps, pml);

	fmove = pm->cmd.forwardmove;
	rmove = pm->cmd.rightmove;

	cmd = pm->cmd;
	scale = PM_CmdScale_Walk(pm, &cmd);

	scale *= PM_DamageScale_Walk(ps->damageTimer);
	ps->damageTimer -= (int)(pml->frametime * 1000.0f);

	if ( ps->damageTimer <= 0.0f )
	{
		ps->damageTimer = 0;
	}

	// project moves down to flat plane
	pml->forward[2] = 0;
	pml->right[2] = 0;

	// project the forward and right directions onto the ground plane
	PM_ClipVelocity(pml->forward, pml->groundTrace.normal, pml->forward);
	PM_ClipVelocity(pml->right, pml->groundTrace.normal, pml->right);
	//
	Vec3Normalize(pml->forward);
	Vec3Normalize(pml->right);

	for ( i = 0; i < 3; i++ )
		dir[i] = pml->forward[i] * fmove + pml->right[i] * rmove;
	// when going up or down slopes the wish velocity should Not be zero
	//	wishvel[2] = 0;

	VectorCopy(dir, wishdir);
	wishspeed = Vec3Normalize(wishdir);
	wishspeed *= scale;

	iStance = PM_GetEffectiveStance(ps);

	// when a player gets hit, they temporarily lose
	// full control, which allows them to be moved a bit
	if ( pml->groundTrace.surfaceFlags & SURF_SLICK || ps->pm_flags & PMF_TIME_KNOCKBACK )
	{
		acceleration = 1;
	}
	else if ( iStance == PM_EFF_STANCE_PRONE )
	{
		acceleration = 19;
	}
	else if ( iStance == PM_EFF_STANCE_CROUCH )
	{
		acceleration = 12;
	}
	else
	{
		acceleration = 9;
	}

	if ( ps->pm_flags & PMF_TIME_SLIDE )
	{
		acceleration *= 0.25f;
	}

	PM_Accelerate(ps, pml, wishdir, wishspeed, acceleration);

	//Com_Printf("velocity = %1.1f %1.1f %1.1f\n", pm->ps->velocity[0], pm->ps->velocity[1], pm->ps->velocity[2]);
	//Com_Printf("velocity1 = %1.1f\n", VectorLength(pm->ps->velocity));

	if ( pml->groundTrace.surfaceFlags & SURF_SLICK || ps->pm_flags & PMF_TIME_KNOCKBACK )
	{
		ps->velocity[2] -= ps->gravity * pml->frametime;
	}
	else
	{
		// don't reset the z velocity for slopes
		//pm->ps->velocity[2] = 0;
	}

	vel = VectorLength(ps->velocity);

	VectorCopy(ps->velocity, wishvel);
	// slide along the ground plane
	PM_ClipVelocity(ps->velocity, pml->groundTrace.normal, ps->velocity);

	if ( DotProduct(ps->velocity, wishvel) > 0 )
	{
		Vec3Normalize(ps->velocity);
		VectorScale(ps->velocity, vel, ps->velocity);
	}

	// don't do anything if standing still
	if ( ps->velocity[0] || ps->velocity[1] )
	{
		PM_StepSlideMove(pm, pml, qfalse);
	}

	PM_SetMovementDir(pm, pml);
}

/*
==============
PM_DeadMove
==============
*/
static void PM_DeadMove( playerState_t *ps, pml_t *pml )
{
	float forward;

	if ( !pml->walking )
	{
		return;
	}

	// extra friction

	forward = VectorLength( ps->velocity );
	forward -= 20;
	if ( forward <= 0 )
	{
		VectorClear( ps->velocity );
	}
	else
	{
		Vec3Normalize( ps->velocity );
		VectorScale( ps->velocity, forward, ps->velocity );
	}
}

/*
===============
PM_NoclipMove
===============
*/
void PM_NoclipMove( pmove_t *pm, pml_t *pml )
{
	float speed;
	float drop;
	float curFriction;
	float control;
	float newspeed;
	int i;
	vec3_t wishvel;
	float fmove;
	float rmove;
	float umove;
	vec3_t wishdir;
	float wishspeed;
	float scale;
	playerState_t *ps;

	assert(pm);
	ps = pm->ps;
	assert(ps);

	ps->viewHeightTarget = DEFAULT_VIEWHEIGHT;

	// friction

	speed = VectorLength(ps->velocity);

	if ( speed < 1 )
	{
		VectorCopy(vec3_origin, ps->velocity);
	}
	else
	{
		drop = 0;
		curFriction = friction->current.decimal * 1.5f; // extra friction
		control = speed < stopspeed->current.decimal ? stopspeed->current.decimal : speed;

		drop += control * curFriction * pml->frametime;

		// scale the velocity
		newspeed = speed - drop;
		if ( newspeed < 0 )
			newspeed = 0;
		newspeed /= speed;

		VectorScale(ps->velocity, newspeed, ps->velocity);
	}

	fmove = pm->cmd.forwardmove;
	rmove = pm->cmd.rightmove;
	umove = 0;

	if ( pm->cmd.buttons & BUTTON_LEANRIGHT )
		umove += 127;

	if ( pm->cmd.buttons & BUTTON_LEANLEFT )
		umove -= 127;

	// accelerate
	scale = PM_MoveScale(ps, fmove, rmove, umove);

	for ( i = 0; i < 3; i++ )
		wishvel[i] = pml->forward[i] * fmove + pml->right[i] * rmove + pml->up[i] * umove;

	VectorCopy(wishvel, wishdir);

	wishspeed = Vec3Normalize(wishdir);
	wishspeed *= scale;

	PM_Accelerate(ps, pml, wishdir, wishspeed, 9);

	// move
	VectorMA(ps->origin, pml->frametime, ps->velocity, ps->origin);
}

/*
==============
PM_UFOMove
==============
*/
void PM_UFOMove( pmove_t *pm, pml_t *pml )
{
	float speed;
	float drop;
	float curFriction;
	float control;
	float newspeed;
	vec3_t wishvel;
	float fmove;
	float rmove;
	float umove;
	vec3_t wishdir;
	float wishspeed;
	float scale;
	playerState_t *ps;
	int i;
	vec3_t up;
	vec3_t forward;

	assert(pm);
	ps = pm->ps;
	assert(ps);

	ps->viewHeightTarget = DEFAULT_VIEWHEIGHT;

	fmove = pm->cmd.forwardmove;
	rmove = pm->cmd.rightmove;
	umove = 0;

	if ( pm->cmd.buttons & BUTTON_LEANRIGHT )
		umove += 127;

	if ( pm->cmd.buttons & BUTTON_LEANLEFT )
		umove -= 127;

	// friction

	if ( fmove == 0 && rmove == 0 && umove == 0 )
		speed = 0;
	else
		speed = VectorLength(ps->velocity);

	if ( speed < 1 )
	{
		VectorCopy(vec3_origin, ps->velocity);
	}
	else
	{
		drop = 0;
		curFriction = friction->current.decimal * 1.5f; // extra friction

		control = speed < stopspeed->current.decimal ? stopspeed->current.decimal : speed;

		drop += control * curFriction * pml->frametime;

		// scale the velocity
		newspeed = speed - drop;
		if ( newspeed < 0 )
			newspeed = 0;
		newspeed /= speed;

		VectorScale(ps->velocity, newspeed, ps->velocity);
	}

	// accelerate
	scale = PM_MoveScale(ps, fmove, rmove, umove);

	up[0] = up[1] = 0;
	up[2] = 1;

	Vec3Cross(up, pml->right, forward);

	for ( i = 0; i < 3; i++ )
		wishvel[i] = forward[i] * fmove + pml->right[i] * rmove + up[i] * umove;

	VectorCopy(wishvel, wishdir);

	wishspeed = Vec3Normalize(wishdir);
	wishspeed *= scale;

	PM_Accelerate(ps, pml, wishdir, wishspeed, 9);

	// move
	VectorMA(ps->origin, pml->frametime, ps->velocity, ps->origin);
}

/*
================
PM_FootstepForSurface

Returns an event number apropriate for the groundsurface
================
*/
int PM_FootstepForSurface( playerState_t *ps, pml_t *pml )
{
	int iSurfType;
	int pm_flags;

	iSurfType = PM_GroundSurfaceType(pml);

	if ( !iSurfType )
	{
#ifdef EXTRA_DEBUG
		Com_Printf(
		    "PM_FootstepForSurface has been called with a ground surface of type 'NONE' near (%.2f %.2f %.2f). \n"
		    "This means a player has landed on a surface that wasn't properly setup to be used as a ground surface. \n"
		    "Use a different material which has a surface type set.\n",
		    pml->previous_origin[0],
		    pml->previous_origin[1],
		    pml->previous_origin[2]);
#endif
		return EV_NONE;
	}

	pm_flags = ps->pm_flags;

	if ( (bool)(pm_flags & PMF_PRONE) )
	{
		return EV_FOOTSTEP_PRONE_DEFAULT + iSurfType;
	}

	if ( pm_flags & PMF_ADS_WALK || ps->leanf != 0 )
	{
		return EV_FOOTSTEP_WALK_DEFAULT + iSurfType;
	}

	return EV_FOOTSTEP_RUN_DEFAULT + iSurfType;
}

/*
=============
PM_MediumLandingForSurface
=============
*/
int PM_MediumLandingForSurface( pml_t *pml )
{
	int iSurfType;

	iSurfType = PM_GroundSurfaceType(pml);

	if ( !iSurfType )
	{
#ifdef EXTRA_DEBUG
		Com_Printf(
		    "PM_MediumLandingForSurface has been called with a ground surface of type 'NONE' near (%.2f %.2f %.2f). \n"
		    "This means a player has landed on a surface that wasn't properly setup to be used as a ground surface. \n"
		    "Use a different material which has a surface type set.\n",
		    pml->previous_origin[0],
		    pml->previous_origin[1],
		    pml->previous_origin[2]);
#endif
		return EV_NONE;
	}

	return EV_FOOTSTEP_WALK_DEFAULT + iSurfType;
}

/*
=============
PM_LightLandingForSurface
=============
*/
int PM_LightLandingForSurface( pml_t *pml )
{
	int iSurfType;

	iSurfType = PM_GroundSurfaceType(pml);

	if ( !iSurfType )
	{
#ifdef EXTRA_DEBUG
		Com_Printf(
		    "PM_LightLandingForSurface has been called with a ground surface of type 'NONE' near (%.2f %.2f %.2f). \n"
		    "This means a player has landed on a surface that wasn't properly setup to be used as a ground surface. \n"
		    "Use a different material which has a surface type set.\n",
		    pml->previous_origin[0],
		    pml->previous_origin[1],
		    pml->previous_origin[2]);
#endif
		return EV_NONE;
	}

	return EV_FOOTSTEP_RUN_DEFAULT + iSurfType;
}

/*
=============
PM_HardLandingForSurface
=============
*/
int PM_HardLandingForSurface( pml_t *pml )
{
	int iSurfType;

	iSurfType = PM_GroundSurfaceType(pml);

	if ( !iSurfType )
	{
#ifdef EXTRA_DEBUG
		Com_Printf(
		    "PM_HardLandingForSurface has been called with a ground surface of type 'NONE' near (%.2f %.2f %.2f). \n"
		    "This means a player has landed on a surface that wasn't properly setup to be used as a ground surface. \n"
		    "Use a different material which has a surface type set.\n",
		    pml->previous_origin[0],
		    pml->previous_origin[1],
		    pml->previous_origin[2]);
#endif
	}

	return EV_LANDING_DEFAULT + iSurfType;
}

/*
=============
PM_DamageLandingForSurface
=============
*/
int PM_DamageLandingForSurface( pml_t *pml )
{
	int iSurfType;

	iSurfType = PM_GroundSurfaceType(pml);

	if ( !iSurfType )
	{
#ifdef EXTRA_DEBUG
		Com_Printf(
		    "PM_DamageLandingForSurface has been called with a ground surface of type 'NONE' near (%.2f %.2f %.2f). \n"
		    "This means a player has landed on a surface that wasn't properly setup to be used as a ground surface. \n"
		    "Use a different material which has a surface type set.\n",
		    pml->previous_origin[0],
		    pml->previous_origin[1],
		    pml->previous_origin[2]);
#endif
	}

	return EV_LANDING_PAIN_DEFAULT + iSurfType;
}

/*
=================
PM_CrashLand

Check for hard landings that generate sound events
=================
*/
static void PM_CrashLand(playerState_t *ps, pml_t *pml)
{
	float landVel;
	float fallHeight;
	float dist;
	float vel;
	float acc;
	float t;
	float a;
	float b;
	float c;
	float den;
	float fSpeedMult;
	int viewDip;
	int damage;
	int stunTime;

	dist = pml->previous_origin[2] - ps->origin[2];
	vel = pml->previous_velocity[2];
	acc = -(float)ps->gravity;
	a = acc * 0.5f;
	b = vel;
	c = dist;
	den = b * b - 4 * a * c;
	if (den < 0)
		return;
	t = (-b - sqrt(den)) / (2 * a);
	landVel = (t * acc + vel) * -1.0f;
	fallHeight = landVel * landVel / (ps->gravity * 2.0f);
	if (bg_fallDamageMaxHeight->current.decimal <= bg_fallDamageMinHeight->current.decimal)
	{
		Com_Printf("bg_fallDamageMaxHeight must be greater than bg_fallDamageMinHeight\n");
		damage = 0;
	}
	else if (fallHeight <= bg_fallDamageMinHeight->current.decimal || pml->groundTrace.surfaceFlags & SURF_NODAMAGE || ps->pm_type >= PM_DEAD)
		damage = 0;
	else if (fallHeight >= bg_fallDamageMaxHeight->current.decimal)
		damage = 100;
	else
	{
		damage = (int)((fallHeight - bg_fallDamageMinHeight->current.decimal) / (bg_fallDamageMaxHeight->current.decimal - bg_fallDamageMinHeight->current.decimal) * 100);
		damage = I_clamp(damage, 0, 100);
	}
	if (fallHeight <= 12)
	{
		viewDip = 0;
	}
	else
	{
		viewDip = (int)(((fallHeight - 12) / 26 + 1) * 4);
		if (viewDip > 24)
			viewDip = 24;
		BG_AnimScriptEvent(ps, ANIM_ET_LAND, qfalse, qtrue);
	}
	if (damage)
	{
		if (damage < 100 && !(pml->groundTrace.surfaceFlags & SURF_SLICK))
		{
			stunTime = 35 * damage + 500;
			if (stunTime > 2000)
				stunTime = 2000;
			if (stunTime <= 500)
				fSpeedMult = 0.5;
			else if (stunTime >= 1500)
				fSpeedMult = 0.2;
			else
				fSpeedMult = 0.5f - ((stunTime - 500.0f) / 1000.0f) * 0.30000001f;
			ps->pm_time = stunTime;
			ps->pm_flags |= PMF_TIME_SLIDE;
			VectorScale(ps->velocity, fSpeedMult, ps->velocity);
		}
		else
		{
			VectorScale(ps->velocity, 0.67000002, ps->velocity);
		}
		BG_AddPredictableEventToPlayerstate(PM_DamageLandingForSurface(pml), damage, ps);
	}
	else if (fallHeight > 4)
	{
		if (fallHeight < 8)
		{
			PM_AddEvent(ps, PM_MediumLandingForSurface(pml));
		}
		else if (fallHeight < 12)
		{
			PM_AddEvent(ps, PM_LightLandingForSurface(pml));
		}
		else
		{
			VectorScale(ps->velocity, 0.67000002, ps->velocity);
			BG_AddPredictableEventToPlayerstate(PM_HardLandingForSurface(pml), viewDip, ps);
		}
	}
}

/*
=============
PM_CorrectAllSolid
=============
*/
static qboolean PM_CorrectAllSolid( pmove_t *pm, pml_t *pml, trace_t *trace )
{
	unsigned int i;
	vec3_t point;
	playerState_t *ps;

	ps = pm->ps;

	for ( i = 0; i < ARRAY_COUNT(CorrectSolidDeltas); i++ )
	{
		VectorAdd(ps->origin, CorrectSolidDeltas[i], point);
		PM_playerTrace(pm, trace, point, pm->mins, pm->maxs, point, ps->clientNum, pm->tracemask);

		if ( !trace->startsolid )
		{
			VectorCopy(point, ps->origin);

			point[0] = ps->origin[0];
			point[1] = ps->origin[1];
			point[2] = ps->origin[2] - 1.0f;

			PM_playerTrace(pm, trace, ps->origin, pm->mins, pm->maxs, point, ps->clientNum, pm->tracemask);
			pml->groundTrace = *trace;
			Vec3Lerp(ps->origin, point, trace->fraction, ps->origin);

			return qtrue;
		}
	}

	ps->groundEntityNum = ENTITYNUM_NONE;

	pml->groundPlane = qfalse;
	pml->almostGroundPlane = qfalse;
	pml->walking = qfalse;

	Jump_ClearState(ps);

	return qfalse;
}

/*
=============
PM_GroundTraceMissed

The ground trace didn't hit a surface, so we are in freefall
=============
*/
static void PM_GroundTraceMissed(pmove_t *pm, pml_t *pml)
{
	trace_t trace;
	vec3_t point;
	playerState_t *ps;

	ps = pm->ps;
	if (ps->groundEntityNum != ENTITYNUM_NONE)
	{
		VectorCopy(ps->origin, point);
		point[2] -= 64;
		pmoveHandlers[pm->handler].trace(&trace, ps->origin, pm->mins, pm->maxs, point, ps->clientNum, pm->tracemask);
		if (trace.fraction == 1.0)
		{
			if (pm->cmd.forwardmove >= 0)
				BG_AnimScriptEvent(ps, ANIM_ET_JUMP, qfalse, qtrue);
			else
				BG_AnimScriptEvent(ps, ANIM_ET_JUMPBK, qfalse, qtrue);
			pml->almostGroundPlane = qfalse;
		}
		else
		{
			pml->almostGroundPlane = trace.fraction < 0.015625f;
		}
	}
	else
	{
		VectorCopy(ps->origin, point);
		point[2] = point[2] - 1.0f;
		pmoveHandlers[pm->handler].trace(&trace, ps->origin, pm->mins, pm->maxs, point, ps->clientNum, pm->tracemask);
		pml->almostGroundPlane = trace.fraction != 1.0f;
	}
	ps->groundEntityNum = ENTITYNUM_NONE;
	pml->groundPlane = qfalse;
	pml->walking = qfalse;
}

/*
=============
PM_GroundTrace
=============
*/
void PM_GroundTrace( pmove_t *pm, pml_t *pml )
{
	vec3_t start;
	vec3_t point;
	trace_t trace;
	playerState_t *ps;

	assert(pm);
	ps = pm->ps;
	assert(ps);

	start[0] = ps->origin[0];
	start[1] = ps->origin[1];

	point[0] = ps->origin[0];
	point[1] = ps->origin[1];

	if ( ps->eFlags & EF_TURRET_ACTIVE )
	{
		start[2] = ps->origin[2];
		point[2] = ps->origin[2] - 1.0f;
	}
	else
	{
		start[2] = ps->origin[2] + 0.25f;
		point[2] = ps->origin[2] - 0.25f;
	}

	PM_playerTrace(pm, &trace, start, pm->mins, pm->maxs, point, ps->clientNum, pm->tracemask);
	pml->groundTrace = trace;

	// do something corrective if the trace starts in a solid...
	if ( trace.allsolid )
	{
		if ( !PM_CorrectAllSolid( pm, pml, &trace ) )
		{
			return;
		}
	}

	if ( trace.startsolid )
	{
		start[2] = ps->origin[2] - 0.001f;
		PM_playerTrace(pm, &trace, start, pm->mins, pm->maxs, point, ps->clientNum, pm->tracemask);

		if ( trace.startsolid )
		{
			ps->groundEntityNum = ENTITYNUM_NONE;
			pml->groundPlane = qfalse;
			pml->almostGroundPlane = qfalse;
			pml->walking = qfalse;
			return;
		}

		pml->groundTrace = trace;
	}

	// if the trace didn't hit anything, we are in free fall
	if ( trace.fraction == 1.0 )
	{
		PM_GroundTraceMissed(pm, pml);
		return;
	}

	assert(trace.normal[0] || trace.normal[1] || trace.normal[2]);

	// check if getting thrown off the ground
	if ( (ps->pm_flags & PMF_LADDER ) == 0 && ps->velocity[2] > 0 && DotProduct( ps->velocity, trace.normal ) > 10 )
	{
		// go into jump animation
		if ( pm->cmd.forwardmove >= 0 )
		{
			BG_AnimScriptEvent(ps, ANIM_ET_JUMP, qfalse, qfalse);
		}
		else
		{
			BG_AnimScriptEvent(ps, ANIM_ET_JUMPBK, qfalse, qfalse);
		}

		pml->almostGroundPlane = qfalse;
		ps->groundEntityNum = ENTITYNUM_NONE;
		pml->groundPlane = qfalse;
		pml->walking = qfalse;
		return;
	}

	// slopes that are too steep will not be considered onground
	if ( trace.normal[2] < (float)MIN_WALK_NORMAL )
	{
		ps->groundEntityNum = ENTITYNUM_NONE;
		pml->groundPlane = qtrue;
		pml->almostGroundPlane = qtrue;
		pml->walking = qfalse;
		Jump_ClearState(ps);
		return;
	}

	pml->groundPlane = qtrue;
	pml->almostGroundPlane = qtrue;
	pml->walking = qtrue;

	if ( ps->groundEntityNum == ENTITYNUM_NONE )
	{
		// just hit the ground
		PM_CrashLand(ps, pml);
	}

	ps->groundEntityNum = trace.entityNum;

	// don't reset the z velocity for slopes
	//	pm->ps->velocity[2] = 0;

	PM_AddTouchEnt(pm, trace.entityNum);
}

/*
===============
PM_GetViewHeightLerpTime
===============
*/
int PM_GetViewHeightLerpTime( const playerState_t *ps, int iTarget, int bDown )
{
	if ( iTarget == PRONE_VIEWHEIGHT )
		return 400;

	if ( iTarget == CROUCH_VIEWHEIGHT )
		return bDown ? 200 : 400;

	return 200;
}

/*
================
PM_ViewHeightTableLerp
================
*/
float PM_ViewHeightTableLerp( int iFrac, viewLerpWaypoint_s *pTable, float *pfPosOfs )
{
	int i;
	viewLerpWaypoint_s *pCurr;
	viewLerpWaypoint_s *pPrev;
	float fEntryFrac;

	if ( !iFrac )
	{
		*pfPosOfs = (float)pTable->iOffset;
		return pTable->fViewHeight;
	}

	assert(iFrac < 100);
	pCurr = pTable + 1;
	i = 1;

	do
	{
		if ( iFrac == pCurr->iFrac )
		{
			*pfPosOfs = (float)pCurr->iOffset;
			return pCurr->fViewHeight;
		}

		if ( pCurr->iFrac > iFrac )
		{
			pPrev = &pTable[i - 1];
			assert((pCurr->iFrac - pPrev->iFrac) > 0);
			fEntryFrac = (float)(iFrac - pPrev->iFrac) / (float)(pCurr->iFrac - pPrev->iFrac);
			*pfPosOfs = (float)pPrev->iOffset + (float)((float)(pCurr->iOffset - pPrev->iOffset) * fEntryFrac);
			return pPrev->fViewHeight + (pCurr->fViewHeight - pPrev->fViewHeight) * fEntryFrac;
		}

		pCurr = &pTable[++i];
	}
	while ( pCurr->iFrac != -1 );

	assert(va( "No encapsulating table entries found for fraction %i", iFrac ));
	*pfPosOfs = (float)pTable->iOffset;

	return pTable->fViewHeight;
}

/*
================
PM_GetViewHeightLerp
================
*/
float PM_GetViewHeightLerp(const pmove_t *pm, int iFromHeight, int iToHeight)
{
	playerState_t *ps;
	int iLerpTime;
	float fLerpFrac;

	ps = pm->ps;
	if (!ps->viewHeightLerpTime)
		return 0;
	if (iFromHeight == -1 || iToHeight == -1 || iToHeight == ps->viewHeightLerpTarget
		&& (iToHeight != 40 || iFromHeight == 11 && !ps->viewHeightLerpDown || iFromHeight == 60 && ps->viewHeightLerpDown))
	{
		iLerpTime = PM_GetViewHeightLerpTime(ps, ps->viewHeightLerpTarget, ps->viewHeightLerpDown);
		fLerpFrac = (float)(pm->cmd.serverTime - ps->viewHeightLerpTime) / iLerpTime;
		if (fLerpFrac < 0)
			fLerpFrac = 0;
		else if (fLerpFrac > 1)
			fLerpFrac = 1;
		return fLerpFrac;
	}
	return 0;
}

/*
============
PM_ViewHeightAdjust
============
*/
void PM_ViewHeightAdjust( pmove_t *pm, pml_t *pml )
{
	playerState_t *ps;
	int iLerpFrac;
	int iLerpTime;
	float fNewPosOfs;
	float scale;
	vec3_t vel;
	vec3_t vDir;

	assert(pm);
	ps = pm->ps;
	assert(ps);

	if ( !ps->viewHeightTarget || ps->viewHeightCurrent == 0 )
	{
		if ( ps->pm_type == PM_SPECTATOR )
		{
			ps->viewHeightCurrent = 0;
		}
		else
		{
			ps->viewHeightCurrent = (float)ps->viewHeightTarget;
		}

		return;
	}

	if ( ps->viewHeightCurrent != ps->viewHeightTarget || ps->viewHeightLerpTime )
	{
	}
	else
	{
		return;
	}

	iLerpFrac = 0;

	if ( ps->viewHeightTarget != PRONE_VIEWHEIGHT && ps->viewHeightTarget != CROUCH_VIEWHEIGHT && ps->viewHeightTarget != DEFAULT_VIEWHEIGHT )
	{
		ps->viewHeightLerpTime = 0;

		if ( ps->viewHeightCurrent < ps->viewHeightTarget )
		{
			ps->viewHeightCurrent += pml->frametime * 180;

			if ( ps->viewHeightCurrent >= ps->viewHeightTarget )
			{
				ps->viewHeightCurrent = (float)ps->viewHeightTarget;
			}
		}
		else
		{
			ps->viewHeightCurrent -= pml->frametime * 180;

			if ( ps->viewHeightCurrent <= ps->viewHeightTarget )
			{
				ps->viewHeightCurrent = (float)ps->viewHeightTarget;
			}
		}

		return;
	}

	if ( ps->viewHeightLerpTime )
	{
		iLerpTime = PM_GetViewHeightLerpTime(ps, ps->viewHeightLerpTarget, ps->viewHeightLerpDown);
		iLerpFrac = 100 * (pm->cmd.serverTime - ps->viewHeightLerpTime) / iLerpTime;

		if ( iLerpFrac < 0 )
		{
			iLerpFrac = 0;
		}
		else
		{
			if ( iLerpFrac > 100 )
			{
				iLerpFrac = 100;
			}
		}

		if ( iLerpFrac == 100 )
		{
			ps->viewHeightCurrent = (float)ps->viewHeightLerpTarget;
			ps->viewHeightLerpTime = 0;
			ps->viewHeightLerpPosAdj = 0;
		}
		else
		{
			if ( ps->viewHeightLerpTarget == PRONE_VIEWHEIGHT )
			{
				ps->viewHeightCurrent = PM_ViewHeightTableLerp(iLerpFrac, viewLerp_CrouchProne, &fNewPosOfs);
			}
			else if ( ps->viewHeightLerpTarget == CROUCH_VIEWHEIGHT )
			{
				if ( ps->viewHeightLerpDown )
					ps->viewHeightCurrent = PM_ViewHeightTableLerp(iLerpFrac, viewLerp_StandCrouch, &fNewPosOfs);
				else
					ps->viewHeightCurrent = PM_ViewHeightTableLerp(iLerpFrac, viewLerp_ProneCrouch, &fNewPosOfs);
			}
			else
			{
				ps->viewHeightCurrent = PM_ViewHeightTableLerp(iLerpFrac, viewLerp_CrouchStand, &fNewPosOfs);
			}

			if ( I_fabs(ps->viewHeightLerpPosAdj - fNewPosOfs) > 0.050000001f )
			{
				VectorCopy(ps->velocity, vel);
				scale = fNewPosOfs - ps->viewHeightLerpPosAdj;

				if ( ps->groundEntityNum == ENTITYNUM_NONE )
				{
					scale *= 0.5f;
				}

				scale = scale / pml->frametime;

				vDir[0] = pml->forward[0];
				vDir[1] = pml->forward[1];
				vDir[2] = 0.0;

				Vec3Normalize(vDir);
				VectorScale(vDir, scale, ps->velocity);

				PM_StepSlideMove(pm, pml, qtrue);
				VectorCopy(vel, ps->velocity);

				ps->viewHeightLerpPosAdj = fNewPosOfs;
			}
		}
	}

	if ( ps->viewHeightLerpTime )
	{
		if ( ps->viewHeightTarget != ps->viewHeightLerpTarget && (ps->viewHeightTarget < ps->viewHeightLerpTarget
		        && !ps->viewHeightLerpDown || ps->viewHeightTarget > ps->viewHeightLerpTarget && ps->viewHeightLerpDown) )
		{
			iLerpFrac = 100 - iLerpFrac;
			ps->viewHeightLerpDown ^= qtrue;

			if ( ps->viewHeightLerpDown )
			{
				if ( ps->viewHeightLerpTarget == DEFAULT_VIEWHEIGHT )
				{
					ps->viewHeightLerpTarget = CROUCH_VIEWHEIGHT;
				}
				else if ( ps->viewHeightLerpTarget == CROUCH_VIEWHEIGHT )
				{
					ps->viewHeightLerpTarget = PRONE_VIEWHEIGHT;
				}
			}
			else if ( ps->viewHeightLerpTarget == PRONE_VIEWHEIGHT )
			{
				ps->viewHeightLerpTarget = CROUCH_VIEWHEIGHT;
			}
			else if ( ps->viewHeightLerpTarget == CROUCH_VIEWHEIGHT )
			{
				ps->viewHeightLerpTarget = DEFAULT_VIEWHEIGHT;
			}

			if ( iLerpFrac == 100 )
			{
				ps->viewHeightCurrent = (float)ps->viewHeightLerpTarget;
				ps->viewHeightLerpTime = 0;
				ps->viewHeightLerpPosAdj = 0;
			}
			else
			{
				iLerpTime = PM_GetViewHeightLerpTime(ps, ps->viewHeightLerpTarget, ps->viewHeightLerpDown);
				ps->viewHeightLerpTime = pm->cmd.serverTime - (int)(iLerpFrac * 0.0099999998f * iLerpTime);

				if ( ps->viewHeightLerpTarget == PRONE_VIEWHEIGHT )
				{
					PM_ViewHeightTableLerp(iLerpFrac, viewLerp_CrouchProne, &fNewPosOfs);
				}
				else if ( ps->viewHeightLerpTarget == CROUCH_VIEWHEIGHT )
				{
					if ( ps->viewHeightLerpDown )
						PM_ViewHeightTableLerp(iLerpFrac, viewLerp_StandCrouch, &fNewPosOfs);
					else
						PM_ViewHeightTableLerp(iLerpFrac, viewLerp_ProneCrouch, &fNewPosOfs);
				}
				else
				{
					PM_ViewHeightTableLerp(iLerpFrac, viewLerp_CrouchStand, &fNewPosOfs);
				}

				ps->viewHeightLerpPosAdj = fNewPosOfs;
			}
		}
	}
	else if ( ps->viewHeightCurrent != ps->viewHeightTarget )
	{
		ps->viewHeightLerpTime = pm->cmd.serverTime;

		if ( ps->viewHeightTarget == PRONE_VIEWHEIGHT )
		{
			ps->viewHeightLerpDown = qtrue;

			if ( ps->viewHeightCurrent > CROUCH_VIEWHEIGHT )
				ps->viewHeightLerpTarget = CROUCH_VIEWHEIGHT;
			else
				ps->viewHeightLerpTarget = PRONE_VIEWHEIGHT;
		}
		else if ( ps->viewHeightTarget == CROUCH_VIEWHEIGHT )
		{
			if ( ps->viewHeightCurrent > ps->viewHeightTarget )
				ps->viewHeightLerpDown = qtrue;
			else
				ps->viewHeightLerpDown = qfalse;

			ps->viewHeightLerpTarget = CROUCH_VIEWHEIGHT;
		}
		else if ( ps->viewHeightTarget == DEFAULT_VIEWHEIGHT )
		{
			ps->viewHeightLerpDown = qfalse;

			if ( ps->viewHeightCurrent < CROUCH_VIEWHEIGHT )
				ps->viewHeightLerpTarget = CROUCH_VIEWHEIGHT;
			else
				ps->viewHeightLerpTarget = DEFAULT_VIEWHEIGHT;
		}
	}
}

/*
===============
PM_ShouldMakeFootsteps
===============
*/
qboolean PM_ShouldMakeFootsteps( pmove_t *pm )
{
	int stance;
	int bCanWalk;
	playerState_t *ps;

	ps = pm->ps;
	bCanWalk = ps->pm_flags & PMF_ADS_WALK;
	stance = PM_GetEffectiveStance(ps);

	if ( stance == PM_EFF_STANCE_PRONE )
	{
	}
	else if ( stance == PM_EFF_STANCE_CROUCH )
	{
	}
	else
	{
		if ( ps->pm_flags & PMF_BACKWARDS_RUN )
		{
			if ( !bCanWalk )
				return pm->xyspeed >= player_footstepsThreshhold->current.decimal;
		}
		else if ( !bCanWalk )
		{
			return pm->xyspeed >= player_footstepsThreshhold->current.decimal;
		}
	}

	return qfalse;
}

/*
================
PM_GetFlinchAnim
================
*/
int PM_GetFlinchAnim( float yaw )
{
	if ( yaw < 0 )
	{
		yaw += 360;
	}

	if ( yaw >= 315 || yaw < 45 )
	{
		return ANIM_MT_FLINCH_FORWARD;
	}

	if ( yaw >= 45 && yaw < 135 )
	{
		return ANIM_MT_FLINCH_LEFT;
	}

	if ( yaw >= 135 && yaw < 225 )
	{
		return ANIM_MT_FLINCH_BACKWARD;
	}

	return ANIM_MT_FLINCH_RIGHT;
}

/*
============
PM_FoliageSounds
============
*/
void PM_FoliageSounds( pmove_t *pm )
{
	float speedFrac;
	int interval;
	trace_t trace;
	vec3_t mins;
	vec3_t maxs;
	playerState_t *ps;

	ps = pm->ps;
	assert(ps);

	if ( pm->xyspeed < bg_foliagesnd_minspeed->current.decimal )
	{
		if ( ps->foliageSoundTime + bg_foliagesnd_resetinterval->current.integer < pm->cmd.serverTime )
		{
			ps->foliageSoundTime = 0;
		}

		return;
	}

	assert(bg_foliagesnd_maxspeed->current.decimal - bg_foliagesnd_minspeed->current.decimal > 0);
	speedFrac = (pm->xyspeed - bg_foliagesnd_minspeed->current.decimal) / (bg_foliagesnd_maxspeed->current.decimal - bg_foliagesnd_minspeed->current.decimal);

	if ( speedFrac > 1 )
	{
		speedFrac = 1;
	}

	interval = (bg_foliagesnd_fastinterval->current.integer - bg_foliagesnd_slowinterval->current.integer) * speedFrac + bg_foliagesnd_slowinterval->current.integer;

	if ( ps->foliageSoundTime + interval < pm->cmd.serverTime )
	{
		VectorScale(pm->mins, 0.75f, mins);
		VectorScale(pm->maxs, 0.75f, maxs);

		maxs[2] = pm->maxs[2] * 0.89999998f;

		PM_playerTrace(pm, &trace, ps->origin, mins, maxs, ps->origin, ps->clientNum, CONTENTS_FOILAGE);

		if ( trace.startsolid )
		{
			PM_AddEvent(ps, EV_FOLIAGE_SOUND);
			ps->foliageSoundTime = pm->cmd.serverTime;
		}
	}
}

/*
================
PM_DropTimers
================
*/
static void PM_DropTimers( playerState_t *ps, pml_t *pml )
{
	// drop misc timing counter
	if ( ps->pm_time )
	{
		if ( pml->msec >= ps->pm_time )
		{
			ps->pm_flags &= ~PMF_ALL_TIMES;
			ps->pm_time = 0;
		}
		else
		{
			ps->pm_time -= pml->msec;
		}
	}

	// drop animation counter
	if ( ps->legsTimer > 0 )
	{
		ps->legsTimer -= pml->msec;
		if ( ps->legsTimer < 0 )
		{
			ps->legsTimer = 0;
		}
	}

	if ( ps->torsoTimer > 0 )
	{
		ps->torsoTimer -= pml->msec;
		if ( ps->torsoTimer < 0 )
		{
			ps->torsoTimer = 0;
		}
	}
}

/*
==================
PM_UpdateLean
==================
*/
void PM_UpdateLean(playerState_t *ps, float msec, usercmd_t *cmd,
	void (*capsuleTrace)(trace_t *, const float *, const float *, const float *, const float *, int, int))
{
	vec3_t start;
	vec3_t end;
	vec3_t tmins;
	vec3_t tmaxs;
	int leaning;
	float leanofs;
	trace_t trace;
	float fLeanMax;
	int stance;
	float fLean;
	leaning = 0;
	leanofs = 0;
	if ((cmd->buttons & (BUTTON_LEANLEFT | BUTTON_LEANRIGHT)) && !(ps->pm_flags & PMF_FROZEN) && ps->pm_type < 6
		&& (ps->groundEntityNum != ENTITYNUM_NONE || ps->pm_type == PM_NORMAL_LINKED))
	{
		if (cmd->buttons & BUTTON_LEANLEFT)
			leaning--;
		if (cmd->buttons & BUTTON_LEANRIGHT)
			leaning++;
	}
	if (ps->eFlags & 0x300)
		leaning = 0;
	stance = PM_GetEffectiveStance(ps);
	if (stance == PM_EFF_STANCE_PRONE)
		fLeanMax = 0.25;
	else
		fLeanMax = 0.5;
	leanofs = ps->leanf;
	if (!leaning)
	{
		if (leanofs > 0)
		{
			leanofs = leanofs - fLeanMax * (msec / 280.0f);
			if (leanofs < 0)
				leanofs = 0;
		}
		else if (leanofs < 0)
		{
			leanofs = fLeanMax * (msec / 280.0f) + leanofs;
			if (leanofs > 0)
				leanofs = 0;
		}
	}
	else if (leaning > 0)
	{
		if (leanofs < fLeanMax)
			leanofs = fLeanMax * (msec / 350.0f) + leanofs;
		if (leanofs > fLeanMax)
			leanofs = fLeanMax;
	}
	else
	{
		if (leanofs > -fLeanMax)
			leanofs = leanofs - fLeanMax * (msec / 350.0f);
		if (leanofs < -fLeanMax)
			leanofs = -fLeanMax;
	}
	ps->leanf = leanofs;
	if (ps->leanf != 0)
	{
	fLean = FloatSign(ps->leanf);
	VectorCopy(ps->origin, start);
	start[2] = start[2] + ps->viewHeightCurrent;
	VectorCopy(start, end);
	AddLeanToPosition(end, ps->viewangles[1], fLean, 16.0, 20.0);
	VectorSet(tmins, -8.0, -8.0, -8.0);
	VectorSet(tmaxs, 8.0, 8.0, 8.0);
	capsuleTrace(&trace, start, tmins, tmaxs, end, ps->clientNum, MASK_PLAYERSOLID);
	fLean = UnGetLeanFraction(trace.fraction);
	if (I_fabs(ps->leanf) > fLean)
		ps->leanf = fLean * FloatSign(ps->leanf);
	}
}

/*
============
BG_CheckProneTurned
============
*/
qboolean BG_CheckProneTurned( playerState_t *ps, float newProneYaw, byte handler )
{
	float delta;
	float testYaw;
	float fraction;
	float feet_dist;

	delta = AngleDelta(newProneYaw, ps->viewangles[YAW]);
	fraction = I_fabs(delta) / 240.0f;
	testYaw = AngleNormalize360Accurate(newProneYaw - (1.0f - fraction) * delta);
	feet_dist = fraction * 45.0f + (1.0f - fraction) * 66.0f;

	return BG_CheckProne( ps->clientNum, ps->origin, ps->maxs[0], 30.0, testYaw,
	                      &ps->fTorsoHeight, &ps->fTorsoPitch, &ps->fWaistPitch,
	                      qtrue, ps->groundEntityNum != ENTITYNUM_NONE, NULL, handler, PCT_CLIENT, feet_dist );
}

/*
==================
PM_UpdatePronePitch
==================
*/
void PM_UpdatePronePitch( pmove_t *pm, pml_t *pml )
{
	float fTargPitch;
	float delta;
	playerState_t *ps;
	qboolean bProneOK;

	assert(pm);
	ps = pm->ps;
	assert(ps);

	if ( !( ps->pm_flags & PMF_PRONE ) )
	{
		return;
	}

	if ( ps->groundEntityNum == ENTITYNUM_NONE )
	{
		bProneOK = BG_CheckProne( ps->clientNum, ps->origin, ps->maxs[0], 30.0f, ps->proneDirection, &ps->fTorsoHeight, &ps->fTorsoPitch,
		                          &ps->fWaistPitch, qtrue, ps->groundEntityNum != ENTITYNUM_NONE,
		                          pml->groundPlane ? pml->groundTrace.normal : NULL, pm->handler, PCT_CLIENT, 66.0f);

		if ( !bProneOK )
		{
			BG_AddPredictableEventToPlayerstate(EV_STANCE_FORCE_CROUCH, 0, ps);
			ps->pm_flags |= PMF_PRONE_BLOCKED;
		}
	}
	else
	{
		if ( pml->groundPlane && pml->groundTrace.normal[2] < 0.69999999f )
		{
			BG_AddPredictableEventToPlayerstate(EV_STANCE_FORCE_CROUCH, 0, ps);
		}
	}

	if ( pml->groundPlane )
	{
		fTargPitch = PitchForYawOnNormal(ps->proneDirection, pml->groundTrace.normal);
	}
	else
	{
		fTargPitch = 0;
	}

	delta = AngleDelta(fTargPitch, ps->proneDirectionPitch);

	if ( delta != 0 )
	{
		if ( I_fabs(delta) > pml->frametime * 70.0f )
		{
			ps->proneDirectionPitch += FloatSign(delta) * (pml->frametime * 70.0f);
		}
		else
		{
			ps->proneDirectionPitch = ps->proneDirectionPitch + delta;
		}

		ps->proneDirectionPitch = AngleNormalize180Accurate(ps->proneDirectionPitch);
	}

	if ( pml->groundPlane )
	{
		fTargPitch = PitchForYawOnNormal(ps->viewangles[1], pml->groundTrace.normal);
	}
	else
	{
		fTargPitch = 0;
	}

	delta = AngleDelta(fTargPitch, ps->proneTorsoPitch);

	if ( delta != 0 )
	{
		if ( I_fabs(delta) > pml->frametime * 70.0f )
		{
			ps->proneTorsoPitch += FloatSign(delta) * (pml->frametime * 70.0f);
		}
		else
		{
			ps->proneTorsoPitch = ps->proneTorsoPitch + delta;
		}

		ps->proneTorsoPitch = AngleNormalize180Accurate(ps->proneTorsoPitch);
	}
}

/*
===============
PM_SetProneMovementOverride
===============
*/
void PM_SetProneMovementOverride( playerState_t *ps )
{
	if ( ps->pm_flags & PMF_PRONE )
	{
		ps->pm_flags |= PMF_ADS_OVERRIDE;
	}
}

/*
================
PM_UpdatePlayerWalkingFlag
================
*/
void PM_UpdatePlayerWalkingFlag( pmove_t *pm )
{
	playerState_t *ps;

	ps = pm->ps;
	assert(ps);

	ps->pm_flags &= ~PMF_ADS_WALK;

	if ( ps->pm_type >= PM_DEAD )
	{
		return;
	}

	if ( !(pm->cmd.buttons & BUTTON_ADS) )
	{
		return;
	}

	if ( ps->pm_flags & PMF_PRONE )
	{
		return;
	}

	if ( !(ps->pm_flags & PMF_ADS) )
	{
		return;
	}

	if ( ps->weaponstate == WEAPON_RELOADING
		|| ps->weaponstate == WEAPON_RELOAD_START
		|| ps->weaponstate == WEAPON_RELOAD_END
		|| ps->weaponstate == WEAPON_RELOAD_START_INTERUPT
		|| ps->weaponstate == WEAPON_RELOADING_INTERUPT )
	{
		return;
	}

	ps->pm_flags |= PMF_ADS_WALK;
}

/*
================
PM_ClearLadderFlag
================
*/
void PM_ClearLadderFlag( playerState_t *ps )
{
	if ( ps->pm_flags & PMF_LADDER )
	{
		ps->pm_flags |= PMF_LADDER_END;
	}

	ps->pm_flags &= ~PMF_LADDER;
}

/*
================
PM_CheckLadderMove

  Checks to see if we are on a ladder
================
*/
void PM_CheckLadderMove( pmove_t *pm, pml_t *pml )
{
#define TRACE_LADDER_DIST   30.0f
	vec3_t spot;
	vec3_t vLadderCheckDir;
	vec3_t mins;
	vec3_t maxs;
	trace_t trace;
	float tracedist;
	qboolean fellOffLadderInAir;
	playerState_t *ps;

	assert(pm);
	ps = pm->ps;
	assert(ps);

	if ( pml->walking )
	{
		ps->pm_flags &= ~PMF_LADDER_END;
	}

	if ( ps->pm_time && ps->pm_flags & ( PMF_TIME_SLIDE | PMF_TIME_KNOCKBACK) )
	{
		return;
	}

	if ( pml->walking )
	{
		tracedist = 8.0f;
	}
	else
	{
		tracedist = TRACE_LADDER_DIST;
	}

	fellOffLadderInAir = ( ps->pm_flags & PMF_LADDER ) && ps->groundEntityNum == ENTITYNUM_NONE;

	if ( fellOffLadderInAir )
	{
		VectorNegate(ps->vLadderVec, vLadderCheckDir);
	}
	else
	{
		// check for ladder
		vLadderCheckDir[0] = pml->forward[0];
		vLadderCheckDir[1] = pml->forward[1];
		vLadderCheckDir[2] = 0.0f;

		Vec3Normalize(vLadderCheckDir);
	}

	if ( ps->pm_type >= PM_DEAD )
	{
		ps->groundEntityNum = ENTITYNUM_NONE;

		pml->groundPlane = qfalse;
		pml->almostGroundPlane = qfalse;
		pml->walking = qfalse;

		PM_ClearLadderFlag(ps);
		return;
	}

	if ( ps->pm_flags & PMF_LADDER_END )
	{
		PM_ClearLadderFlag(ps);
		return;
	}

	if ( PM_GetEffectiveStance(ps) == PM_EFF_STANCE_PRONE )
	{
		PM_ClearLadderFlag(ps);
		return;
	}

	if ( pm->cmd.serverTime - ps->jumpTime < 300 )
	{
		PM_ClearLadderFlag(ps);
		return;
	}

	VectorCopy(pm->mins, mins);

	mins[0] = mins[0] + 6.0f;
	mins[1] = mins[1] + 6.0f;
	mins[2] = 8.0f;

	VectorCopy(pm->maxs, maxs);

	maxs[0] = maxs[0] - 6.0f;
	maxs[1] = maxs[1] - 6.0f;

	if ( maxs[2] < mins[2] )
	{
		maxs[2] = mins[2];
	}

	assert(maxs[0] >= mins[0]);
	assert(maxs[1] >= mins[1]);
	assert(maxs[2] >= mins[2]);

	VectorMA(ps->origin, tracedist, vLadderCheckDir, spot);
	PM_playerTrace(pm, &trace, ps->origin, mins, maxs, spot, ps->clientNum, pm->tracemask);

	if ( ( trace.fraction < 1 ) && ( trace.surfaceFlags & SURF_LADDER ) )
	{
		if ( !pml->walking || pm->cmd.forwardmove > 0 )
		{
			if ( !( ps->pm_flags & PMF_LADDER ) )
			{
				// if we are only just on the ladder, don't do this yet, or it may throw us back off the ladder
				VectorCopy(trace.normal, ps->vLadderVec);
				VectorNegate(ps->vLadderVec, vLadderCheckDir);

				VectorMA(ps->origin, tracedist, vLadderCheckDir, spot);
				PM_playerTrace(pm, &trace, ps->origin, mins, maxs, spot, ps->clientNum, pm->tracemask);

				if ( ( trace.fraction < 1 ) && ( trace.surfaceFlags & SURF_LADDER ) )
				{
					PM_SetLadderFlag(ps); // set ladder bit
					return;
				}
			}
			else
			{
				PM_SetLadderFlag(ps); // set ladder bit
				return;
			}
		}
	}

	PM_ClearLadderFlag(ps);

	if ( !fellOffLadderInAir )
	{
	}
	else
	{
		BG_AnimScriptEvent(ps, ANIM_ET_JUMP, qfalse, qtrue);
	}
}

/*
============
PM_LadderMove
============
*/
void PM_LadderMove( pmove_t *pm, pml_t *pml )
{
	float wishspeed;
	float scale;
	vec3_t wishdir;
	vec3_t wishvel;
	vec3_t vTempRight;
	float upscale;
	vec2_t vSideDir;
	float fSideSpeed;
	float fSpeedDrop;
	int moveyaw;
	playerState_t *ps;

	assert(pm);
	ps = pm->ps;
	assert(ps);

	if ( Jump_Check(pm, pml) )
	{
		// jumped away
		PM_AirMove(pm, pml);
		return;
	}

	upscale = ( pml->forward[2] + 0.25f ) * 2.5f;
	if ( upscale > 1.0f )
	{
		upscale = 1.0f;
	}
	else if ( upscale < -1.0f )
	{
		upscale = -1.0f;
	}

	// forward/right should be horizontal only
	pml->forward[2] = 0;
	Vec3Normalize(pml->forward);
	pml->right[2] = 0;
	Vec3NormalizeTo(pml->right, vTempRight);

	ProjectPointOnPlane(vTempRight, ps->vLadderVec, pml->right);

	// move depending on the view, if view is straight forward, then go up
	// if view is down more then X degrees, start going down
	// if they are back pedalling, then go in reverse of above
	scale = PM_CmdScale(ps, &pm->cmd);
	VectorClear(wishvel);

	if ( pm->cmd.forwardmove )
	{
		wishvel[2] = upscale * 0.5f * scale * pm->cmd.forwardmove;
	}

	if ( pm->cmd.rightmove )
	{
		VectorMA(wishvel, scale * 0.2f * pm->cmd.rightmove, pml->right, wishvel);
	}

	wishspeed = Vec3NormalizeTo(wishvel, wishdir);
	PM_Accelerate(ps, pml, wishdir, wishspeed, 9);

	if ( !pm->cmd.forwardmove )
	{
		if ( ps->velocity[2] > 0 )
		{
			ps->velocity[2] -= ps->gravity * pml->frametime;

			if ( ps->velocity[2] < 0 )
			{
				ps->velocity[2] = 0;
			}
		}
		else
		{
			ps->velocity[2] += ps->gravity * pml->frametime;

			if ( ps->velocity[2] > 0 )
			{
				ps->velocity[2] = 0;
			}
		}
	}

	if ( !pm->cmd.rightmove )
	{
		Vector2Copy(pml->right, vSideDir);
		Vec2Normalize(vSideDir);

		fSideSpeed = Dot2Product(vSideDir, ps->velocity);

		if ( fSideSpeed != 0 )
		{
			VectorMA2(ps->velocity, -fSideSpeed, vSideDir, ps->velocity);
			fSpeedDrop = fSideSpeed * pml->frametime * 16.0f;

			if ( I_fabs(fSideSpeed) > I_fabs(fSpeedDrop) )
			{
				if ( I_fabs(fSpeedDrop) < 1 )
				{
					fSpeedDrop = FloatSign(fSpeedDrop);
				}

				fSideSpeed = fSideSpeed - fSpeedDrop;
				VectorMA2(ps->velocity, fSideSpeed, vSideDir, ps->velocity);
			}
		}
	}

	if ( !pml->walking )
	{
		fSideSpeed = Dot2Product(ps->vLadderVec, ps->velocity);
		VectorMA2(ps->velocity, -fSideSpeed, ps->vLadderVec, ps->velocity);

		if ( Vec2Multiply(ps->velocity) <= I_square(ps->velocity[2]) )
		{
			fSideSpeed = -50;
			VectorMA2(ps->velocity, fSideSpeed, ps->vLadderVec, ps->velocity);
		}
	}

	PM_StepSlideMove(pm, pml, qfalse);  // no gravity while going up ladder

	scale = vectoyaw(ps->vLadderVec) + 180.0f;
	moveyaw = (int)AngleDelta(scale, ps->viewangles[YAW]);

	if ( abs( moveyaw ) > 75 )
	{
		if ( moveyaw > 0 )
		{
			moveyaw = 75;
		}
		else
		{
			moveyaw = -75;
		}
	}

	// always point legs forward
	ps->movementDir = (signed char)moveyaw;
}

/*
================
PmoveSingle
================
*/
void PmoveSingle( pmove_t *pm )
{
	int iStance;
	vec2_t oldVel;
	playerState_t *ps;
	pml_t pml;
	vec3_t move;
	float moveLenSq, velLenSq;

	ps = pm->ps;
	assert(ps);

	// RF, update conditional values for anim system
	BG_AnimUpdatePlayerStateConditions(pm);

	if ( ps->pm_flags & PMF_FROZEN )
	{
		pm->cmd.buttons &= (BUTTON_PRONE | BUTTON_CROUCH | BUTTON_CANNOT_PRONE);
		pm->cmd.forwardmove = 0;
		pm->cmd.rightmove = 0;

		VectorSet(ps->velocity, 0, 0, 0);
	}

	// if talk button is down, dissallow all other input
	// this is to prevent any possible intercept proxy from
	// adding fake talk balloons
	if ( pm->cmd.buttons & BUTTON_TALK )
	{
		pm->cmd.buttons &= (BUTTON_PRONE | BUTTON_CROUCH | BUTTON_ADS | BUTTON_CANNOT_PRONE | BUTTON_TALK);
		pm->cmd.forwardmove = 0;
		pm->cmd.rightmove = 0;
	}

	ps->pm_flags &= ~PMF_PRONE_BLOCKED;

	if ( ps->pm_type >= PM_DEAD )
	{
		pm->tracemask &= ~CONTENTS_BODY; // corpses can fly through bodies
	}

	// make sure walking button is clear if they are running, to avoid
	// proxy no-footsteps cheats
	if ( ps->pm_flags & PMF_PRONE )
	{
		if ( pm->cmd.forwardmove != pm->oldcmd.forwardmove )
		{
			if ( I_fabs(pm->cmd.forwardmove) > I_fabs(pm->oldcmd.forwardmove) )
				goto interrupt;
		}

		if ( pm->cmd.rightmove != pm->oldcmd.rightmove
		        && I_fabs(pm->cmd.rightmove) > I_fabs(pm->oldcmd.rightmove) )
		{
interrupt:
			if ( PM_InteruptWeaponWithProneMove(ps) )
			{
				ps->pm_flags &= ~PMF_ADS_OVERRIDE;
				PM_ExitAimDownSight(ps);
			}
		}
		else if ( !(ps->pm_flags & PMF_ADS) && (ps->weaponstate == WEAPON_READY || ps->weaponstate == WEAPON_RAISING || ps->weaponstate == WEAPON_DROPPING || ps->weaponstate == WEAPON_RELOADING) )
		{
			ps->pm_flags &= ~PMF_ADS_OVERRIDE;
		}
	}
	else
	{
		ps->pm_flags &= ~PMF_ADS_OVERRIDE;
	}

	iStance = PM_GetEffectiveStance(ps);

	if ( ps->pm_flags & PMF_ADS && iStance == PM_EFF_STANCE_PRONE )
	{
		pm->cmd.forwardmove = 0;
		pm->cmd.rightmove = 0;
	}

	// set the talk balloon flag
	if ( pm->cmd.buttons & BUTTON_TALK)
		ps->eFlags |= EF_TALK;
	else
		ps->eFlags &= ~EF_TALK;

	// set the firing flag for continuous beam weapons
	ps->eFlags &= ~EF_FIRING;

	if ( ( ps->pm_type != PM_INTERMISSION ) && !( ps->pm_flags & PMF_RESPAWNED ) )
	{
		if ( ps->weaponstate == WEAPON_READY || ps->weaponstate == WEAPON_FIRING )
		{
			// check for ammo
			if ( PM_WeaponAmmoAvailable( ps ) )
			{
				// all clear, fire!
				if ( pm->cmd.buttons & BUTTON_ATTACK )
				{
					ps->eFlags |= EF_FIRING;
				}
			}
		}
	}

	// clear the respawned flag if attack and use are cleared
	if ( ps->pm_type < PM_DEAD
	        && !(pm->cmd.buttons & (BUTTON_BINOCULARS | BUTTON_ATTACK)) )
	{
		ps->pm_flags &= ~PMF_RESPAWNED;
	}

	// clear all pmove local vars
	memset(&pml, 0, sizeof(pml));

	// determine the time
	pml.msec = pm->cmd.serverTime - ps->commandTime;
	if ( pml.msec < 1 )
	{
		pml.msec = 1;
	}
	else if ( pml.msec > 200 )
	{
		pml.msec = 200;
	}
	ps->commandTime = pm->cmd.serverTime;

	// save old org in case we get stuck
	VectorCopy(ps->origin, pml.previous_origin);

	// save old velocity for crashlanding
	VectorCopy(ps->velocity, pml.previous_velocity);

	pml.frametime = pml.msec * 0.001f;

	PM_AdjustAimSpreadScale(pm, &pml);

	// update the viewangles
	PM_UpdateViewAngles(ps, pml.msec, &pm->cmd, pm->handler);
	AngleVectors(ps->viewangles, pml.forward, pml.right, pml.up);

	// decide if backpedaling animations should be used
	if ( pm->cmd.forwardmove < 0 )
	{
		ps->pm_flags |= PMF_BACKWARDS_RUN;
	}
	else
	{
		if ( pm->cmd.forwardmove > 0 || !pm->cmd.forwardmove && pm->cmd.rightmove )
		{
			ps->pm_flags &= ~PMF_BACKWARDS_RUN;
		}
	}

	if ( ps->pm_type >= PM_DEAD ) // DHM - Nerve
	{
		pm->cmd.forwardmove = 0;
		pm->cmd.rightmove = 0;
	}

	if ( iStance == PM_EFF_STANCE_PRONE && ps->pm_flags & PMF_ADS_OVERRIDE )
	{
		pm->cmd.forwardmove = 0;
		pm->cmd.rightmove = 0;
	}

	Mantle_ClearHint(ps);

	switch ( ps->pm_type )
	{
	case PM_SPECTATOR:
		PM_ClearLadderFlag(ps);
		PM_UpdateAimDownSightFlag(pm, &pml);
		PM_UpdatePlayerWalkingFlag(pm);
		PM_CheckDuck(pm, &pml);
		PM_FlyMove(pm, &pml);
		PM_DropTimers(ps, &pml);
		PM_UpdateAimDownSightLerp(pm, &pml);
		return;

	case PM_NOCLIP:
		PM_ClearLadderFlag(ps);
		PM_UpdateAimDownSightFlag(pm, &pml);
		PM_UpdatePlayerWalkingFlag(pm);
		PM_NoclipMove(pm, &pml);
		PM_DropTimers(ps, &pml);
		PM_UpdateAimDownSightLerp(pm, &pml);
		return;

	case PM_UFO:
		PM_ClearLadderFlag(ps);
		PM_UpdateAimDownSightFlag(pm, &pml);
		PM_UpdatePlayerWalkingFlag(pm);
		PM_UFOMove(pm, &pml);
		PM_DropTimers(ps, &pml);
		PM_UpdateAimDownSightLerp(pm, &pml);
		return;

	case PM_INTERMISSION:
		PM_ClearLadderFlag(ps);
		PM_UpdateAimDownSightFlag(pm, &pml);
		PM_UpdateAimDownSightLerp(pm, &pml);
		return;

	case PM_NORMAL_LINKED:
	case PM_DEAD_LINKED:
		PM_ClearLadderFlag(ps);
		ps->groundEntityNum = ENTITYNUM_NONE;
		pml.groundPlane = qfalse;
		pml.almostGroundPlane = qfalse;
		pml.walking = qfalse;
		VectorClear(ps->velocity);
		PM_UpdateAimDownSightFlag(pm, &pml);
		PM_UpdatePlayerWalkingFlag(pm);
		PM_CheckDuck(pm, &pml);
		PM_DropTimers(ps, &pml);
		PM_Footsteps(pm, &pml);
		PM_Weapon(pm, &pml);
		return;

	default:
		if ( ps->eFlags & EF_TURRET_ACTIVE )
		{
			PM_ClearLadderFlag(ps);
			ps->groundEntityNum = ENTITYNUM_NONE;
			pml.groundPlane = qfalse;
			pml.almostGroundPlane = qfalse;
			pml.walking = qfalse;
			VectorClear(ps->velocity);
			PM_UpdateAimDownSightFlag(pm, &pml);
			PM_UpdatePlayerWalkingFlag(pm);
			PM_CheckDuck(pm, &pml);
			PM_DropTimers(ps, &pml);
			PM_Footsteps(pm, &pml);
			PM_ResetWeaponState(ps);
			return;
		}

		if ( !(ps->pm_flags & PMF_MANTLE) )
		{
			PM_CheckDuck(pm, &pml);
			PM_GroundTrace(pm, &pml);
		}

		Mantle_Check(pm, &pml);

		if ( ps->pm_flags & PMF_MANTLE)
		{
			PM_ClearLadderFlag(ps);
			ps->groundEntityNum = ENTITYNUM_NONE;
			pml.groundPlane = qfalse;
			pml.walking = qfalse;
			PM_UpdateAimDownSightFlag(pm, &pml);
			PM_UpdatePlayerWalkingFlag(pm);
			PM_CheckDuck(pm, &pml);
			Mantle_Move(pm, ps, &pml);
			PM_Weapon(pm, &pml);
			return;
		}

		PM_UpdateAimDownSightFlag(pm, &pml);
		PM_UpdatePlayerWalkingFlag(pm);
		PM_UpdatePronePitch(pm, &pml);

		if ( ps->pm_type == PM_DEAD )
		{
			PM_DeadMove(ps, &pml);
		}

		PM_CheckLadderMove(pm, &pml);
		PM_DropTimers(ps, &pml);

		if ( ps->pm_flags & PMF_LADDER )
		{
			PM_LadderMove(pm, &pml);
		}
		else if ( pml.walking )
		{
			PM_WalkMove(pm, &pml);
		}
		else
		{
			PM_AirMove(pm, &pml);
		}

		PM_GroundTrace(pm, &pml);
		PM_Footsteps(pm, &pml);
		PM_Weapon(pm, &pml);
		PM_FoliageSounds(pm);

		VectorSubtract(ps->origin, pml.previous_origin, move);

		moveLenSq = VectorLengthSquared(move) / (pml.frametime * pml.frametime);
		velLenSq = VectorLengthSquared(ps->velocity);

		if ( moveLenSq < velLenSq * 0.25f )
		{
			VectorScale(move, 1.0f / pml.frametime, ps->velocity);
		}

		Vector2Subtract(ps->velocity, ps->oldVelocity, oldVel);
		Vec2Scale(oldVel, I_fmin(pml.frametime, 1.0f), oldVel);
		Vector2Add(ps->oldVelocity, oldVel, ps->oldVelocity);

		// snap some parts of playerstate to save network bandwidth
		Sys_SnapVector(ps->velocity);
	}
}

/*
================
Pmove

Can be called by either the server or the client
================
*/
void Pmove( pmove_t *pmove )
{
	int finalTime;
	playerState_t *ps;
	int msec;

	ps = pmove->ps;
	assert(ps);

	finalTime = pmove->cmd.serverTime;

	if ( finalTime < ps->commandTime )
	{
		return; // should not happen
	}

	if ( finalTime > ps->commandTime + 1000 )
	{
		ps->commandTime = finalTime - 1000;
	}

	pmove->numtouch = 0;

	// chop the move up if it is too long, to prevent framerate
	// dependent behavior
	while ( ps->commandTime != finalTime )
	{
		msec = finalTime - ps->commandTime;

		// rain - this was 66 (15fps), but I've changed it to
		// 50 (20fps, max rate of mg42) to alleviate some of the
		// framerate dependency with the mg42.
		// in reality, this should be split according to sv_fps,
		// and pmove() shouldn't handle weapon firing
		if ( msec > 66 )
		{
			msec = 66;
		}

		pmove->cmd.serverTime = ps->commandTime + msec;
		PmoveSingle(pmove);

		pmove->oldcmd = pmove->cmd;
	}
}

/*
===============
BG_GetSpeed
===============
*/
float BG_GetSpeed( const playerState_t *ps, int iTime )
{
	if ( ps->pm_flags & PMF_LADDER )
	{
		if ( iTime - ps->jumpTime < 500 )
		{
			return 0;
		}

		return ps->velocity[2];
	}

	return Vec2Length(ps->velocity);
}

void PM_Accelerate( playerState_t *ps, const pml_t *pml, vec3_t wishdir, float wishspeed, float accel )
{
	float addspeed;
	float accelspeed;
	float currentspeed;
	float canPush;
	float control;
	vec3_t wishVelocity;
	vec3_t pushDir;

	if ( !(ps->pm_flags & PMF_LADDER) )
	{
		currentspeed = DotProduct(ps->velocity, wishdir);
		addspeed = wishspeed - currentspeed;

		if ( addspeed <= 0 )
			return;

		control = wishspeed < stopspeed->current.decimal ? stopspeed->current.decimal : wishspeed;
		accelspeed = accel * pml->frametime * control;

		if ( accelspeed > addspeed )
			accelspeed = addspeed;

		canPush = PM_PlayerInertia(ps, accelspeed, wishdir);
		VectorMA(ps->velocity, canPush, wishdir, ps->velocity);
	}
	else
	{
		wishVelocity[0] = wishdir[0] * wishspeed; wishVelocity[1] = wishdir[1] * wishspeed; wishVelocity[2] = wishdir[2] * wishspeed;
		VectorSubtract(wishVelocity, ps->velocity, pushDir);
		control = Vec3Normalize(pushDir);
		canPush = accel * pml->frametime * wishspeed;

		if ( canPush > control )
			canPush = control;

		VectorMA(ps->velocity, canPush, pushDir, ps->velocity);
	}
}
void PM_CheckDuck( pmove_t *pm, pml_t *pml )
{
	qboolean bWasProne;
	qboolean bWasStanding;
	trace_t trace;
	int iStance;
	playerState_t *ps;
	vec3_t vPoint;
	float delta;
	vec3_t vEnd;

	assert(pm);
	ps = pm->ps;
	assert(ps);

	pm->proneChange = qfalse;

	if ( ps->pm_type == PM_SPECTATOR )
	{
		// Ridah, modified this for configurable bounding boxes
		pm->mins[0] = -8.0;
		pm->mins[1] = -8.0;
		pm->mins[2] = -8.0;

		pm->maxs[0] = 8.0;
		pm->maxs[1] = 8.0;
		pm->maxs[2] = 16.0;

		ps->pm_flags &= ~(PMF_PRONE | PMF_DUCKED);

		if ( pm->cmd.buttons & BUTTON_PRONE )
		{
			pm->cmd.buttons &= ~BUTTON_PRONE;
			BG_AddPredictableEventToPlayerstate(EV_STANCE_FORCE_STAND, 0, ps);
		}

		ps->viewHeightTarget = 0;
		ps->viewHeightCurrent = 0;

		return;
	}

	bWasProne = ps->pm_flags & PMF_PRONE ? qtrue : qfalse;
	bWasStanding = !(ps->pm_flags & (PMF_PRONE | PMF_DUCKED));

	// Ridah, modified this for configurable bounding boxes
	pm->mins[0] = ps->mins[0];
	pm->mins[1] = ps->mins[1];

	pm->maxs[0] = ps->maxs[0];
	pm->maxs[1] = ps->maxs[1];

	pm->mins[2] = ps->mins[2];

	if ( ps->pm_type >= PM_DEAD )
	{
		pm->maxs[2] = ps->maxs[2];  // NOTE: must set death bounding box in game code
		ps->viewHeightTarget = DEAD_VIEWHEIGHT;
		PM_ViewHeightAdjust(pm, pml);
		return;
	}

	if ( ps->eFlags & EF_TURRET_ACTIVE )
	{
		if ( ps->eFlags & EF_TURRET_PRONE && !(ps->eFlags & EF_TURRET_DUCK) )
		{
			ps->pm_flags |= PMF_PRONE;
			ps->pm_flags &= ~PMF_DUCKED;
		}
		else
		{
			if ( ps->eFlags & EF_TURRET_DUCK && !(ps->eFlags & EF_TURRET_PRONE) )
			{
				ps->pm_flags |= PMF_DUCKED;
				ps->pm_flags &= ~PMF_PRONE;
			}
			else
			{
				ps->pm_flags &= ~(PMF_PRONE | PMF_DUCKED);
			}
		}
	}
	else if ( ps->pm_flags & PMF_FROZEN )
	{
	}
	else
	{
		if ( ps->pm_flags & PMF_LADDER && pm->cmd.buttons & (BUTTON_PRONE | BUTTON_CROUCH) )
		{
			pm->cmd.buttons &= ~(BUTTON_PRONE | BUTTON_CROUCH);
			BG_AddPredictableEventToPlayerstate(EV_STANCE_FORCE_STAND, 0, ps);
		}

		if ( pm->cmd.buttons & BUTTON_PRONE )
		{
			if ( ps->pm_flags & PMF_PRONE || ps->groundEntityNum != ENTITYNUM_NONE
			        && BG_CheckProne( ps->clientNum, ps->origin, pm->maxs[0], 30.0,
			                          ps->viewangles[YAW], &ps->fTorsoHeight, &ps->fTorsoPitch, &ps->fWaistPitch,
			                          qfalse, ps->groundEntityNum != ENTITYNUM_NONE, NULL, pm->handler, PCT_CLIENT, 66.0) )
			{
				ps->pm_flags |= PMF_PRONE;
				ps->pm_flags &= ~PMF_DUCKED;
			}
			else if ( ps->groundEntityNum != ENTITYNUM_NONE )
			{
				ps->pm_flags |= PMF_PRONE_BLOCKED;

				if ( !(pm->cmd.buttons & BUTTON_CANNOT_PRONE) )
				{
					if ( ps->pm_flags & PMF_DUCKED)
						BG_AddPredictableEventToPlayerstate(EV_STANCE_FORCE_CROUCH, 0, ps);
					else
						BG_AddPredictableEventToPlayerstate(EV_STANCE_FORCE_STAND, 0, ps);
				}
			}
		}
		else if ( pm->cmd.buttons & BUTTON_CROUCH )
		{
			if ( ps->pm_flags & PMF_PRONE )
			{
				pm->maxs[2] = 50.0;
				pmoveHandlers[pm->handler].trace(&trace, ps->origin, pm->mins, pm->maxs, ps->origin, ps->clientNum, pm->tracemask & ~CONTENTS_BODY);

				if ( !trace.allsolid )
				{
					BG_AnimScriptEvent(ps, ANIM_ET_PRONE_TO_CROUCH, qfalse, qfalse);

					ps->pm_flags &= ~PMF_PRONE;
					ps->pm_flags |= PMF_DUCKED;
				}
				else
				{
					if ( !(pm->cmd.buttons & BUTTON_CANNOT_PRONE) )
					{
						BG_AddPredictableEventToPlayerstate(EV_STANCE_FORCE_PRONE, 2, ps);
					}
				}
			}
			else
			{
				BG_AnimScriptEvent(ps, ANIM_ET_STAND_TO_CROUCH, qfalse, qfalse);
				ps->pm_flags |= PMF_DUCKED;
			}
		}
		else if ( ps->pm_flags & PMF_PRONE )
		{
			pm->maxs[2] = ps->maxs[2];
			pmoveHandlers[pm->handler].trace(&trace, ps->origin, pm->mins, pm->maxs, ps->origin, ps->clientNum, pm->tracemask & ~CONTENTS_BODY);

			if ( !trace.allsolid )
			{
				BG_AnimScriptEvent(ps, ANIM_ET_PRONE_TO_STAND, qfalse, qfalse);
				ps->pm_flags &= ~(PMF_PRONE | PMF_DUCKED);
			}
			else
			{
				pm->maxs[2] = 50.0;
				pmoveHandlers[pm->handler].trace(&trace, ps->origin, pm->mins, pm->maxs, ps->origin, ps->clientNum, pm->tracemask & ~CONTENTS_BODY);

				if ( !trace.allsolid )
				{
					ps->pm_flags &= ~PMF_PRONE;
					ps->pm_flags |= PMF_DUCKED;
				}
				else
				{
					if ( !(pm->cmd.buttons & BUTTON_CANNOT_PRONE) )
					{
						BG_AddPredictableEventToPlayerstate(EV_STANCE_FORCE_PRONE, 1, ps);
					}
				}
			}
		}
		else if ( ps->pm_flags & PMF_DUCKED )
		{
			pm->maxs[2] = ps->maxs[2];
			pmoveHandlers[pm->handler].trace(&trace, ps->origin, pm->mins, pm->maxs, ps->origin, ps->clientNum, pm->tracemask & ~CONTENTS_BODY);

			if ( !trace.allsolid )
			{
				BG_AnimScriptEvent(ps, ANIM_ET_CROUCH_TO_STAND, qfalse, qfalse);
				ps->pm_flags &= ~PMF_DUCKED;
			}
			else
			{
				if ( !(pm->cmd.buttons & BUTTON_CANNOT_PRONE) )
				{
					BG_AddPredictableEventToPlayerstate(EV_STANCE_FORCE_CROUCH, 1, ps);
				}
			}
		}
	}

	if ( !ps->viewHeightLerpTime )
	{
		if ( ps->pm_flags & PMF_PRONE )
		{
			if ( ps->viewHeightTarget == DEFAULT_VIEWHEIGHT )
			{
				ps->viewHeightTarget = CROUCH_VIEWHEIGHT;
			}
			else if ( ps->viewHeightTarget != PRONE_VIEWHEIGHT )
			{
				ps->viewHeightTarget = PRONE_VIEWHEIGHT;
				pm->proneChange = qtrue;

				BG_PlayAnim(ps, ANIM_MT_UNUSED, ANIM_BP_TORSO, 0, qfalse, qtrue, qtrue);
				Jump_ActivateSlowdown(ps);
			}
		}
		else if ( ps->viewHeightTarget == PRONE_VIEWHEIGHT )
		{
			ps->viewHeightTarget = CROUCH_VIEWHEIGHT;
			pm->proneChange = qtrue;

			BG_PlayAnim(ps, ANIM_MT_UNUSED, ANIM_BP_TORSO, 0, qfalse, qtrue, qtrue);
		}
		else if ( ps->pm_flags & PMF_DUCKED )
		{
			ps->viewHeightTarget = CROUCH_VIEWHEIGHT;
		}
		else
		{
			ps->viewHeightTarget = DEFAULT_VIEWHEIGHT;
		}
	}

	PM_ViewHeightAdjust(pm, pml);
	iStance = PM_GetEffectiveStance(ps);

	if ( iStance == PM_EFF_STANCE_PRONE )
	{
		pm->maxs[2] = 30.0;

		ps->eFlags |= EF_PRONE;
		ps->eFlags &= ~EF_CROUCH;

		ps->pm_flags |= PMF_PRONE;
		ps->pm_flags &= ~PMF_DUCKED;
	}
	else
	{
		if ( iStance == PM_EFF_STANCE_CROUCH )
		{
			pm->maxs[2] = 50.0;

			ps->eFlags |= EF_CROUCH;
			ps->eFlags &= ~EF_PRONE;

			ps->pm_flags |= PMF_DUCKED;
			ps->pm_flags &= ~PMF_PRONE;
		}
		else
		{
			pm->maxs[2] = ps->maxs[2];

			ps->eFlags &= ~(EF_CROUCH | EF_PRONE);
			ps->pm_flags = ps->pm_flags & ~(PMF_PRONE | PMF_DUCKED);
		}
	}

	if ( ps->pm_flags & PMF_PRONE && !bWasProne )
	{
		if ( pm->cmd.forwardmove || pm->cmd.rightmove )
		{
			ps->pm_flags &= ~PMF_ADS_OVERRIDE;
			PM_ExitAimDownSight(ps);
		}

		VectorCopy(ps->origin, vEnd);
		vEnd[2] = vEnd[2] + 10.0f;

		pmoveHandlers[pm->handler].trace(&trace, ps->origin, pm->mins, pm->maxs, vEnd, ps->clientNum, pm->tracemask & ~CONTENTS_BODY);

		Vec3Lerp(ps->origin, vEnd, trace.fraction, vEnd);
		pmoveHandlers[pm->handler].trace(&trace, vEnd, pm->mins, pm->maxs, ps->origin, ps->clientNum, pm->tracemask & ~CONTENTS_BODY);

		Vec3Lerp(vEnd, ps->origin, trace.fraction, ps->origin);
		ps->proneDirection = ps->viewangles[YAW];

		VectorCopy(ps->origin, vPoint);
		vPoint[2] = vPoint[2] - 0.25f;

		pmoveHandlers[pm->handler].trace(&trace, ps->origin, pm->mins, pm->maxs, vPoint, ps->clientNum, pm->tracemask & ~CONTENTS_BODY);

		if ( !trace.startsolid && trace.fraction < 1.0f )
			ps->proneDirectionPitch = PitchForYawOnNormal(ps->proneDirection, trace.normal);
		else
			ps->proneDirectionPitch = 0;

		delta = AngleDelta(ps->proneDirectionPitch, ps->viewangles[PITCH]);

		if ( delta < -45.0f )
			ps->proneTorsoPitch = ps->viewangles[PITCH] - 45.0f;
		else if ( delta > 45.0f )
			ps->proneTorsoPitch = ps->viewangles[PITCH] + 45.0f;
		else
			ps->proneTorsoPitch = ps->proneDirectionPitch;
	}
}
void PM_FootstepEvent( pmove_t *pm, pml_t *pml, int iOldBobCycle, int iNewBobCycle, qboolean bFootStep )
{
	playerState_t *ps;
	int iSurfaceType;
	int iClipMask;
	int iClientNum;
	float fTraceDist;
	vec3_t mins;
	vec3_t maxs;
	vec3_t vEnd;
	trace_t trace;

	ps = pm->ps;
	assert(ps);

	if ( ( ( iOldBobCycle + 64 ) ^ ( iNewBobCycle + 64 ) ) & 128 )
	{
		if ( ps->groundEntityNum == ENTITYNUM_NONE )
		{
			if ( bFootStep )
			{
				if ( ps->pm_flags & PMF_LADDER )
				{
					VectorCopy(pm->mins, mins);

					mins[0] = mins[0] + 6.0f;
					mins[1] = mins[1] + 6.0f;
					mins[2] = 8.0f;

					VectorCopy(pm->maxs, maxs);

					maxs[0] = maxs[0] - 6.0f;
					maxs[1] = maxs[1] - 6.0f;

					if ( maxs[2] < mins[2] )
					{
						maxs[2] = mins[2];
					}

					assert(maxs[0] >= mins[0]);
					assert(maxs[1] >= mins[1]);
					assert(maxs[2] >= mins[2]);

					iClipMask = pm->tracemask & ~( CONTENTS_BODY | CONTENTS_PLAYERCLIP );
					iClientNum = ps->clientNum;
					fTraceDist = -31.0f;

					VectorMA(ps->origin, fTraceDist, ps->vLadderVec, vEnd);

					PM_playerTrace(pm, &trace, ps->origin, mins, maxs, vEnd, iClientNum, iClipMask);
					iSurfaceType = (byte)( ( trace.surfaceFlags & ( 0x1f << SURF_START_BIT ) ) >> SURF_START_BIT );

					if ( trace.fraction == 1.0f || !iSurfaceType )
					{
						iSurfaceType = SURF_TYPE_WOOD;
					}

					PM_AddEvent(ps, iSurfaceType + 1);
				}
			}
		}
		else if ( bFootStep )
		{
			PM_AddEvent(ps, PM_FootstepForSurface(ps, pml));
		}
	}
}
void PM_Footsteps( pmove_t *pm, pml_t *pml )
{
	float bobmove;
	int old;
	int footstep;
	int iStance;
	float scale;
	float lerpFrac;
	playerState_t *ps;
	int stumbleTime;
	int stumble_end_time;
	int flinchTime;
	int flinch_end_time;
	int flinchAnim;
	clientInfo_t *ci;
	int turnAdjust;
	int animResult;
	qboolean walking;

	ps = pm->ps;
	animResult = -1;

	if ( ps->pm_type >= PM_DEAD )
	{
		return;
	}

	if ( ps->clientNum < MAX_CLIENTS )
	{
		ci = &bgs->clientinfo[ps->clientNum];
	}
	else
	{
		ci = NULL;
	}

	stumbleTime = player_dmgtimer_stumbleTime->current.integer;
	stumble_end_time = ps->damageDuration - stumbleTime;

	if ( stumble_end_time < 0 )
	{
		stumble_end_time = 0;
	}

	flinchTime = player_dmgtimer_flinchTime->current.integer;
	flinch_end_time = ps->damageDuration - flinchTime;

	if ( flinch_end_time < 0 )
	{
		flinch_end_time = 0;
	}

	//
	// calculate speed and cycle to be used for
	// all cyclic walking effects
	//
	pm->xyspeed = I_sqrt( Square(ps->velocity[0]) + Square(ps->velocity[1]) );

	// mg42, always idle
	if ( ps->eFlags & EF_TURRET_ACTIVE )
	{
		if ( ps->pm_flags & PMF_PRONE )
		{
			BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_IDLEPRONE, qtrue);
			return;
		}

		if ( ps->pm_flags & PMF_DUCKED )
		{
			BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_IDLECR, qtrue);
			return;
		}

		BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_IDLE, qtrue);
		return;
	}

	iStance = PM_GetEffectiveStance(ps);

	// in the air
	if ( ps->groundEntityNum == ENTITYNUM_NONE && ps->pm_type != PM_NORMAL_LINKED )
	{
		if ( ps->pm_flags & PMF_LADDER ) // on ladder
		{
			float fLadderSpeed;

			if ( pm->cmd.serverTime - ps->jumpTime < 300 )
			{
				return;
			}

			fLadderSpeed = ps->velocity[2];
			scale = (0.5 * 1.5) * 127.0;

			if ( (ps->pm_flags & PMF_ADS_WALK) || ps->leanf != 0 )
			{
				bobmove = ( fLadderSpeed / ( scale * 0.40000001f ) ) * 0.34999999f;
			}
			else
			{
				bobmove = ( fLadderSpeed / scale ) * 0.44999999f;
			}

			if ( fLadderSpeed >= 0.0f )
			{
				animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_CLIMBUP, qtrue);
			}
			else
			{
				animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_CLIMBDOWN, qtrue);
			}

			// check for footstep / splash sounds
			old = ps->bobCycle;
			ps->bobCycle = (int)( old + pml->msec * bobmove ) & 255;
			PM_FootstepEvent(pm, pml, old, ps->bobCycle, qtrue);
		}

		if ( iStance == (ps->pm_flags & ( PMF_DUCKED | PMF_PRONE) ) )
		{
			return;
		}
	}

	walking = (ps->pm_flags & PMF_ADS_WALK) || ps->leanf != 0;

	// if not trying to move
	if ( pm->xyspeed < player_moveThreshhold->current.decimal || ps->pm_type == PM_NORMAL_LINKED )
	{
		if ( pm->xyspeed < 1 )
		{
			ps->bobCycle = 0; // start at beginning of cycle again
		}

		turnAdjust = ANIM_MT_UNUSED;

		if ( ci && player_turnAnims->current.boolean )
		{
			if ( ci->turnAnimType && ci->turnAnimEndTime )
			{
				Com_DPrintf("turn anim end time is %i, time is %i\n", ci->turnAnimEndTime, bgs->time);
			}

			if ( ci->legs.yawing )
			{
				if ( ci->legs.yawAngle > ci->torso.yawAngle )
					turnAdjust = ANIM_MT_TURNRIGHT;
				else
					turnAdjust = ANIM_MT_TURNLEFT;

				ci->legs.yawAngle = ci->torso.yawAngle;
				ci->turnAnimType = turnAdjust;

				if ( ci->turnAnimEndTime < bgs->time )
				{
					ci->turnAnimEndTime = 0;
				}
			}
			else
			{
				if ( ci->turnAnimEndTime > bgs->time )
				{
					turnAdjust = ci->turnAnimType;
					ci->legs.yawAngle = ci->torso.yawAngle;
					return;
				}

				if ( ci->turnAnimEndTime )
				{
					ci->turnAnimEndTime = 0;
					ci->legs.yawAngle = ci->torso.yawAngle;
				}
			}
		}

		if ( ps->viewHeightTarget == PRONE_VIEWHEIGHT )
		{
			animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_IDLEPRONE, qtrue);
		}
		else if ( ps->viewHeightTarget == CROUCH_VIEWHEIGHT )
		{
			if ( turnAdjust == ANIM_MT_TURNRIGHT )
			{
				animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_TURNRIGHTCR, qtrue);

				if ( animResult > 0 && !ci->turnAnimEndTime )
				{
					ci->turnAnimEndTime = bgs->time + ps->legsAnimDuration;
				}
			}
			else if ( turnAdjust == ANIM_MT_TURNLEFT )
			{
				animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_TURNLEFTCR, qtrue);

				if ( animResult > 0 && !ci->turnAnimEndTime )
				{
					ci->turnAnimEndTime = bgs->time + ps->legsAnimDuration;
				}
			}
			else
			{
				animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_IDLECR, qtrue);
			}
		}
		else if ( turnAdjust == ANIM_MT_TURNRIGHT )
		{
			animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_TURNRIGHT, qtrue);

			if ( animResult > 0 && !ci->turnAnimEndTime )
			{
				ci->turnAnimEndTime = bgs->time + ps->legsAnimDuration;
			}
		}
		else if ( turnAdjust == ANIM_MT_TURNLEFT )
		{
			animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_TURNLEFT, qtrue);

			if ( animResult > 0 && !ci->turnAnimEndTime )
			{
				ci->turnAnimEndTime = bgs->time + ps->legsAnimDuration;
			}
		}
		else
		{
			if ( ps->damageTimer > stumble_end_time )
			{
				flinchAnim = PM_GetFlinchAnim(ps->flinchYaw);
				animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, flinchAnim, qtrue);
				return;
			}

			animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_IDLE, qtrue);
		}

		if ( animResult < 0 )
		{
			if ( ps->viewHeightTarget == CROUCH_VIEWHEIGHT )
			{
				animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_IDLECR, qtrue);
				return;
			}

			if ( ps->damageTimer > stumble_end_time )
			{
				animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_FLINCH_FORWARD, qtrue);
				return;
			}

			animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_IDLE, qtrue);
			return;
		}

		return;
	}

	scale = ps->speed;

	if ( pm->cmd.forwardmove )
	{
		if ( pm->cmd.rightmove )
		{
			scale = ( ( ( ( player_strafeSpeedScale->current.decimal - 1.0f ) * 0.75f + 1.0f ) + 1.0f ) * scale ) * 0.5f;

			if ( pm->cmd.forwardmove < 0 )
			{
				scale = (player_backSpeedScale->current.decimal + 1.0f) * 0.5f * scale;
			}
		}
		else if ( pm->cmd.forwardmove < 0 )
		{
			scale = scale * player_backSpeedScale->current.decimal;
		}

		BG_UpdateConditionValue(ps->clientNum, ANIM_COND_STRAFING, LEANING_NOT, qtrue);
	}
	else if ( pm->cmd.rightmove )
	{
		scale = ((player_strafeSpeedScale->current.decimal - 1.0f) * 0.75f + 1.0f) * scale;

		if ( pm->cmd.rightmove > 0 )
			BG_UpdateConditionValue(ps->clientNum, ANIM_COND_STRAFING, LEANING_RIGHT, qtrue);
		else
			BG_UpdateConditionValue(ps->clientNum, ANIM_COND_STRAFING, LEANING_LEFT, qtrue);
	}

	if ( walking )
	{
		scale = scale * 0.40000001f;
	}

	lerpFrac = PM_GetViewHeightLerp(pm, CROUCH_VIEWHEIGHT, PRONE_VIEWHEIGHT);

	if ( lerpFrac != 0 )
	{
		scale = (lerpFrac * 0.15000001f + (1.0f - lerpFrac) * 0.64999998f) * scale;
	}
	else
	{
		lerpFrac = PM_GetViewHeightLerp(pm, PRONE_VIEWHEIGHT, CROUCH_VIEWHEIGHT);

		if ( lerpFrac != 0 )
		{
			scale = (lerpFrac * 0.64999998f + (1.0f - lerpFrac) * 0.15000001f) * scale;
		}
		else if ( iStance == PM_EFF_STANCE_PRONE )
		{
			scale = scale * 0.15000001f;
		}
		else if ( iStance == PM_EFF_STANCE_CROUCH )
		{
			scale = scale * 0.64999998f;
		}
	}

	if ( iStance == PM_EFF_STANCE_PRONE )
	{
		if ( walking )
			bobmove = pm->xyspeed / scale * 0.23999999f;
		else
			bobmove = pm->xyspeed / scale * 0.25f;

		if ( ps->pm_flags & PMF_BACKWARDS_RUN )
			animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_WALKPRONEBK, qtrue);
		else
			animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_WALKPRONE, qtrue);
	}
	else if ( iStance == PM_EFF_STANCE_CROUCH )
	{
		if ( walking )
			bobmove = pm->xyspeed / scale * 0.315f;
		else
			bobmove = pm->xyspeed / scale * 0.34f;

		if ( ps->pm_flags & PMF_BACKWARDS_RUN )
		{
			if ( walking )
			{
				if ( ps->damageTimer > stumble_end_time )
				{
					animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_STUMBLE_CROUCH_BACKWARD, qtrue);
					goto footstep;
				}

				animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_WALKCRBK, qtrue);
				goto footstep;
			}
			else
			{
				if ( ps->damageTimer > stumble_end_time )
				{
					animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_STUMBLE_CROUCH_BACKWARD, qtrue);
					goto footstep;
				}

				animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_RUNCRBK, qtrue);
				goto footstep;
			}
		}
		else
		{
			if ( walking )
			{
				if ( ps->damageTimer > stumble_end_time )
				{
					animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_STUMBLE_CROUCH_FORWARD, qtrue);
					goto footstep;
				}

				animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_WALKCR, qtrue);
				goto footstep;
			}
			else
			{
				if ( ps->damageTimer > stumble_end_time )
				{
					animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_STUMBLE_CROUCH_FORWARD, qtrue);
					goto footstep;
				}

				animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_RUNCR, qtrue);
				goto footstep;
			}
		}
	}
	else if ( ps->pm_flags & PMF_BACKWARDS_RUN )
	{
		if ( walking )
		{
			bobmove = pm->xyspeed / scale * 0.32499999f;

			if ( ps->damageTimer > stumble_end_time )
				animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_STUMBLE_WALK_BACKWARD, qtrue);
			else
				animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_WALKBK, qtrue);
		}
		else
		{
			bobmove = pm->xyspeed / scale * 0.36000001f;

			if ( ps->damageTimer > stumble_end_time )
				animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_STUMBLE_BACKWARD, qtrue);
			else
				animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_RUNBK, qtrue);
		}
	}
	else if ( walking )
	{
		bobmove = pm->xyspeed / scale * 0.30500001f;

		if ( ps->damageTimer > stumble_end_time )
			animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_STUMBLE_WALK_FORWARD, qtrue);
		else
			animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_WALK, qtrue);
	}
	else
	{
		bobmove = pm->xyspeed / scale * 0.33500001f;

		if ( ps->damageTimer > stumble_end_time )
			animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_STUMBLE_FORWARD, qtrue);
		else
			animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_RUN, qtrue);
	}

footstep:
	// check for footstep / splash sounds
	footstep = PM_ShouldMakeFootsteps(pm);
	old = ps->bobCycle;
	ps->bobCycle = (int)( old + pml->msec * bobmove ) & 255;

	if ( !pm->cmd.forwardmove && !pm->cmd.rightmove )
	{
		if ( pm->xyspeed > 120.0f )
		{
			return; // continue what they were doing last frame, until we stop
		}

		if ( ps->viewHeightTarget == PRONE_VIEWHEIGHT )
		{
			animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_IDLEPRONE, qtrue);
		}
		else if ( ps->viewHeightTarget == CROUCH_VIEWHEIGHT )
		{
			animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_IDLECR, qtrue);
		}

		if ( animResult < 0 )
		{
			if ( ps->damageTimer > stumble_end_time )
			{
				animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_FLINCH_FORWARD, qtrue);
				return;
			}

			animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_IDLE, qtrue);
			return;
		}

		return;
	}

	if ( animResult < 0 )
	{
		animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_IDLE, qtrue);
	}

	PM_FootstepEvent(pm, pml, old, ps->bobCycle, footstep);
}
// rain - take a tracemask as well - we can't use anything out of pm
__declspec(noinline) float AngleDeltaNI( float angle1, float angle2 )
{
	return AngleDelta(angle1, angle2);
}
void PM_UpdateViewAngles( playerState_t *ps, float msec, usercmd_t *cmd, byte handler )
{
	int pmFlags;
	short temp;
	int i;
	float delta;
	float newProneYaw;
	qboolean bProneOK;
	qboolean bRetry;
	float oldViewYaw;
	float newViewYaw;
	qboolean proneBlocked;
	float deltaYaw1;
	float deltaYaw2;
	float ladderFacing;
	int minPitch;
	int maxPitch;

	if ( ps->pm_type == PM_INTERMISSION )
	{
		return; // no view changes at all
	}

	if ( ps->pm_type >= PM_DEAD )
	{
		temp = cmd->angles[YAW] + ps->delta_angles[YAW];

		if ( ps->stats[STAT_DEAD_YAW] == 999 )
		{
			ps->stats[STAT_DEAD_YAW] = SHORT2ANGLE ( temp );
		}

		PM_UpdateLean(ps, msec, cmd, pmoveHandlers[handler].trace);
		return; // no view changes at all
	}

	oldViewYaw = ps->viewangles[YAW];

	minPitch = ANGLE2SHORT(player_view_pitch_up->current.decimal);
	maxPitch = ANGLE2SHORT(player_view_pitch_down->current.decimal);

	for ( i = 0; i < 3; i++ )
	{
		temp = cmd->angles[i] + ps->delta_angles[i];

		if ( i == PITCH )
		{
			if ( temp > maxPitch )
			{
				ps->delta_angles[i] = maxPitch - cmd->angles[i];
				temp = maxPitch;
			}
			else if ( temp < -minPitch )
			{
				ps->delta_angles[i] = -minPitch - cmd->angles[i];
				temp = -minPitch;
			}
		}

		ps->viewangles[i] = SHORT2ANGLE ( temp );
	}

	newViewYaw = ps->viewangles[YAW];

	if ( ps->eFlags & EF_TURRET_ACTIVE )
	{
		for ( i = 0; i < 2; i++ )
		{
			delta = AngleDelta(ps->viewAngleClampBase[i], ps->viewangles[i]);

			if ( delta > ps->viewAngleClampRange[i] || -ps->viewAngleClampRange[i] > delta )
			{
				if ( delta > ps->viewAngleClampRange[i] )
					delta -= ps->viewAngleClampRange[i];
				else
					delta += ps->viewAngleClampRange[i];

				ps->delta_angles[i] += ANGLE2SHORT(delta);

				if ( delta > 0 )
					ps->viewangles[i] = AngleNormalize360Accurate(ps->viewAngleClampBase[i] - ps->viewAngleClampRange[i]);
				else
					ps->viewangles[i] = AngleNormalize360Accurate(ps->viewAngleClampBase[i] + ps->viewAngleClampRange[i]);
			}
		}

		return;
	}

	pmFlags = ps->pm_flags;

	if ( pmFlags & PMF_MANTLE )
	{
		Mantle_CapView(ps);
		return;
	}

	if ( pmFlags & PMF_LADDER && ps->groundEntityNum == ENTITYNUM_NONE && bg_ladder_yawcap->current.decimal != 0 )
	{
		ladderFacing = vectoyaw(ps->vLadderVec) + 180;
		delta = AngleDelta(ladderFacing, ps->viewangles[YAW]);

		if ( delta > bg_ladder_yawcap->current.decimal || -bg_ladder_yawcap->current.decimal > delta )
		{
			if ( delta > bg_ladder_yawcap->current.decimal )
				delta -= bg_ladder_yawcap->current.decimal;
			else
				delta += bg_ladder_yawcap->current.decimal;

			ps->delta_angles[YAW] += ANGLE2SHORT(delta);

			if ( delta <= 0 )
				ps->viewangles[YAW] = AngleNormalize360Accurate(ladderFacing + bg_ladder_yawcap->current.decimal);
			else
				ps->viewangles[YAW] = AngleNormalize360Accurate(ladderFacing - bg_ladder_yawcap->current.decimal);
		}
	}

	if ( pmFlags & PMF_PRONE )
	{
		proneBlocked = qfalse;

		delta = AngleDelta(ps->proneDirection, ps->viewangles[YAW]);

		if ( delta > bg_prone_yawcap->current.decimal - 5.0f || -(bg_prone_yawcap->current.decimal - 5.0f) > delta || (cmd->forwardmove || cmd->rightmove) && delta != 0 )
		{
			if ( I_fabs(delta) < msec * 55.0f * 0.001f )
			{
				newProneYaw = ps->viewangles[YAW];
			}
			else
			{
				if ( delta > 0 )
					newProneYaw = ps->proneDirection - msec * 55.0f * 0.001f;
				else
					newProneYaw = ps->proneDirection + msec * 55.0f * 0.001f;
			}

			bRetry = qtrue;

			while ( 1 )
			{
				if ( BG_CheckProneTurned(ps, newProneYaw, handler) )
				{
					bProneOK = BG_CheckProne(ps->clientNum, ps->origin, ps->maxs[0], 30.0, ps->viewangles[YAW],
					                         NULL, NULL, NULL, qtrue, ps->groundEntityNum != ENTITYNUM_NONE, NULL, handler, PCT_CLIENT, PRONE_FEET_DIST_TURNED);
					if ( bProneOK )
					{
						bProneOK = BG_CheckProne(ps->clientNum, ps->origin, ps->maxs[0], 30.0, newProneYaw,
						                         NULL, NULL, NULL, qtrue, ps->groundEntityNum != ENTITYNUM_NONE, NULL, handler, PCT_CLIENT, PRONE_FEET_DIST_TURNED);
						if ( bProneOK )
						{
							ps->proneDirection = newProneYaw;
						}
					}

					if ( !bProneOK )
					{
						proneBlocked = qtrue;
					}

					break;
				}

				if ( !bRetry )
				{
					break;
				}

				delta = AngleDelta(ps->proneDirection, newProneYaw);

				bRetry = I_fabs(delta) > 1;

				if ( bRetry )
				{
					if ( delta > 0 )
						delta = 1;
					else
						delta = -1;
				}
				else
				{
					proneBlocked = qtrue;
				}

				newProneYaw = AngleNormalize360Accurate(newProneYaw + delta);
			}
		}

		delta = AngleDelta(ps->proneDirection, ps->viewangles[YAW]);

		if ( delta != 0 )
		{
			newProneYaw = ps->proneDirection;
			bRetry = qtrue;

			while ( 1 )
			{
				bProneOK = BG_CheckProne(ps->clientNum, ps->origin, ps->maxs[0], 30.0, newProneYaw,
				                         NULL, NULL, NULL, qtrue, ps->groundEntityNum != ENTITYNUM_NONE, NULL, handler, PCT_CLIENT, PRONE_FEET_DIST_TURNED);
				if ( bProneOK )
				{
					if ( BG_CheckProneTurned(ps, newProneYaw, handler) )
					{
						ps->proneDirection = newProneYaw;
						break;
					}
				}

				if ( !bRetry )
				{
					break;
				}

				bRetry = I_fabs(delta) > 1;

				if ( bRetry )
				{
					if ( delta > 0 )
						delta = 1;
					else
						delta = -1;
				}

				proneBlocked = qtrue;

				ps->delta_angles[YAW] += ANGLE2SHORT(delta);
				ps->viewangles[YAW] = AngleNormalize360Accurate(ps->viewangles[YAW] + delta);

				delta = AngleDelta(ps->proneDirection, ps->viewangles[YAW]);

				if ( !bProneOK )
				{
					newProneYaw = AngleNormalize360Accurate(newProneYaw + delta);
				}
			}
		}

		if ( delta > bg_prone_yawcap->current.decimal || -bg_prone_yawcap->current.decimal > delta )
		{
			if ( delta > bg_prone_yawcap->current.decimal )
				delta -= bg_prone_yawcap->current.decimal;
			else
				delta += bg_prone_yawcap->current.decimal;

			ps->delta_angles[YAW] += ANGLE2SHORT(delta);

			if ( delta > 0 )
				ps->viewangles[YAW] = AngleNormalize360Accurate(ps->proneDirection - bg_prone_yawcap->current.decimal);
			else
				ps->viewangles[YAW] = AngleNormalize360Accurate(ps->proneDirection + bg_prone_yawcap->current.decimal);
		}

		if ( proneBlocked )
		{
			ps->pm_flags |= PMF_PRONE_BLOCKED;
			deltaYaw1 = AngleDelta(oldViewYaw, ps->viewangles[YAW]);

			if ( I_fabs(deltaYaw1) <= 1 )
			{
				deltaYaw2 = AngleDeltaNI(newViewYaw, ps->viewangles[YAW]);

				if ( deltaYaw1 * deltaYaw2 > 0 )
				{
					deltaYaw1 *= 0.98000002f;
					ps->viewangles[YAW] = AngleNormalize360Accurate(ps->viewangles[YAW] + deltaYaw1);
					ps->delta_angles[YAW] += ANGLE2SHORT(deltaYaw1);
				}
			}
		}

		delta = AngleDelta(ps->proneTorsoPitch, ps->viewangles[PITCH]);

		if ( delta > 45 || delta < -45 )
		{
			if ( delta > 45 )
				delta -= 45;
			else
				delta += 45;

			ps->delta_angles[PITCH] += ANGLE2SHORT(delta);

			if ( delta > 0 )
				ps->viewangles[PITCH] = AngleNormalize180Accurate(ps->proneTorsoPitch - 45);
			else
				ps->viewangles[PITCH] = AngleNormalize180Accurate(ps->proneTorsoPitch + 45);
		}
	}

	if ( ps->pm_type == PM_UFO )
	{
		return;
	}

	if ( ps->pm_type == PM_NOCLIP )
	{
		return;
	}

	if ( ps->pm_type == PM_SPECTATOR )
	{
		return;
	}

	PM_UpdateLean(ps, msec, cmd, pmoveHandlers[handler].trace);
}
void PM_SetLadderFlag( playerState_t *ps )
{
	ps->pm_flags |= PMF_LADDER;
}

