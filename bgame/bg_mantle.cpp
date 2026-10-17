// bg_public.h gives callers a bool prototype; the definition here returns int,
// a mismatch the original carried (callers test al, the body returns eax).
#define Mantle_IsWeaponInactive Mantle_IsWeaponInactive_callers
#include "../qcommon/qcommon.h"
#include "bg_public.h"
#undef Mantle_IsWeaponInactive

static dvar_t *mantle_enable;
static dvar_t *mantle_debug;
static dvar_t *mantle_check_range;
static dvar_t *mantle_check_radius;
static dvar_t *mantle_check_angle;
static dvar_t *mantle_view_yawcap;

enum MantleAnims
{
	MANTLE_ROOT,
	MANTLE_UP_57,
	MANTLE_UP_51,
	MANTLE_UP_45,
	MANTLE_UP_39,
	MANTLE_UP_33,
	MANTLE_UP_27,
	MANTLE_UP_21,
	MANTLE_OVER_HIGH,
	MANTLE_OVER_MID,
	MANTLE_OVER_LOW,
	MANTLE_ANIM_COUNT
};

static MantleAnimTransition s_mantleTrans[] =
{
	{ 1, 8, 57 },
	{ 2, 8, 51 },
	{ 3, 9, 45 },
	{ 4, 9, 39 },
	{ 5, 9, 33 },
	{ 6, 10, 27 },
	{ 7, 10, 21 },
};

static const char *s_mantleAnimNames[] =
{
	"mp_mantle_root",
	"mp_mantle_up_57",
	"mp_mantle_up_51",
	"mp_mantle_up_45",
	"mp_mantle_up_39",
	"mp_mantle_up_33",
	"mp_mantle_up_27",
	"mp_mantle_up_21",
	"mp_mantle_over_high",
	"mp_mantle_over_mid",
	"player_mantle_over_low",
};

static XAnim *s_mantleAnims;

void Mantle_RegisterDvars();
void* MantleXAnimPrecacheAlloc( int size );
void Mantle_DebugPrint( const char *msg );
int Mantle_GetUpLength( const MantleState *mstate );
int Mantle_GetOverLength( const MantleState *mstate );
int Mantle_GetAnim( MantleState *mstate );
void Mantle_GetAnimDelta( MantleState *mstate, int time, vec3_t delta );
int Mantle_FindTransition( float curHeight, float goalHeight );
void Mantle_CalcEndPos( pmove_t *pm, MantleResults *mresults );
void Mantle_Start( pmove_t *pm, playerState_t *ps, MantleResults *mresults );
bool Mantle_CheckLedge( pmove_t *pmove, pml_t *pml, MantleResults *mresults, float units );
void Mantle_CreateAnims( void *(*xanimAlloc)(int) );
void Mantle_ShutdownAnims();
bool Mantle_FindMantleSurface( pmove_t *pm, pml_t *pml, trace_t *trace, vec3_t mantleDir );
void Mantle_Check( pmove_t *pmove, pml_t *pml );
void Mantle_Move( pmove_t *pm, playerState_t *ps, pml_t *pml );
void Mantle_CapView( playerState_t *ps );
void Mantle_ClearHint( playerState_t *ps );
bool Mantle_IsWeaponInactive( const playerState_t *ps );

/*
==================
Mantle_RegisterDvars
==================
*/
void Mantle_RegisterDvars()
{
	unsigned short flags = 0x100;

	mantle_enable = Dvar_RegisterBool("mantle_enable", true, (unsigned short)(flags | 0x1080));
	mantle_debug = Dvar_RegisterBool("mantle_debug", false, (unsigned short)(flags | 0x1080));
	mantle_check_range = Dvar_RegisterFloat("mantle_check_range", 20, 0, 128, (unsigned short)(flags | 0x1080));
	mantle_check_radius = Dvar_RegisterFloat("mantle_check_radius", 0.1, 0, 15, (unsigned short)(flags | 0x1080));
	mantle_check_angle = Dvar_RegisterFloat("mantle_check_angle", 60, 0, 180, (unsigned short)(flags | 0x1080));
	mantle_view_yawcap = Dvar_RegisterFloat("mantle_view_yawcap", 60, 0, 180, (unsigned short)(flags | 0x1080));
}

/*
==================
MantleXAnimPrecacheAlloc
==================
*/
void* MantleXAnimPrecacheAlloc( int size )
{
	return Hunk_AllocAlignInternal(size, 4);
}

/*
==================
Mantle_DebugPrint
==================
*/
void Mantle_DebugPrint( const char *msg )
{
	assert(msg);

	if ( !mantle_debug->current.boolean )
	{
		return;
	}

	Com_Printf("%s\n", msg);
}

/*
==================
Mantle_GetUpLength
==================
*/
int Mantle_GetUpLength( const MantleState *mstate )
{
	int animIndex;
	int result;

	animIndex = s_mantleTrans[mstate->transIndex].upAnimIndex;
	result = XAnimGetLengthMsec(s_mantleAnims, animIndex);

	return result;
}

/*
==================
Mantle_GetOverLength
==================
*/
int Mantle_GetOverLength( const MantleState *mstate )
{
	int animIndex;
	int length;
	int result;

	if ( !(mstate->flags & 1) )
	{
		result = 0;
	}
	else
	{
		animIndex = s_mantleTrans[mstate->transIndex].overAnimIndex;
		length = XAnimGetLengthMsec(s_mantleAnims, animIndex);
		result = length;
	}

	return result;
}

/*
==================
Mantle_GetAnim
==================
*/
int Mantle_GetAnim( MantleState *mstate )
{
	int upTime;
	int overTime;

	upTime = Mantle_GetUpLength(mstate);
	overTime = Mantle_GetOverLength(mstate);

	assert((mstate->timer >= 0) && (mstate->timer <= (upTime + overTime)));

	if ( mstate->timer <= upTime )
		return s_mantleTrans[mstate->transIndex].upAnimIndex;
	else
		return s_mantleTrans[mstate->transIndex].overAnimIndex;
}

/*
==================
Mantle_GetAnimDelta
==================
*/
void Mantle_GetAnimDelta( MantleState *mstate, int time, vec3_t delta )
{
	vec3_t trans;
	vec2_t rot;
	int upTime;
	int overTime;
	int animIndex;
	float frac;

	upTime = Mantle_GetUpLength(mstate);
	overTime = Mantle_GetOverLength(mstate);

	if ( time <= upTime )
	{
		frac = (float)time / (float)upTime;
		animIndex = s_mantleTrans[mstate->transIndex].upAnimIndex;
		XAnimGetAbsDelta(s_mantleAnims, animIndex, rot, delta, frac);
	}
	else
	{
		frac = (float)(time - upTime) / (float)overTime;
		animIndex = s_mantleTrans[mstate->transIndex].upAnimIndex;
		XAnimGetAbsDelta(s_mantleAnims, animIndex, rot, trans, 1.0);
		animIndex = s_mantleTrans[mstate->transIndex].overAnimIndex;
		XAnimGetAbsDelta(s_mantleAnims, animIndex, rot, delta, frac);

		VectorAdd(delta, trans, delta);
	}

	RotateVec2(delta, mstate->yaw);
}

/*
==================
Mantle_FindTransition
==================
*/
int Mantle_FindTransition( float curHeight, float goalHeight )
{
	float height;
	float diff;
	float bestDiff;
	int transIndex;
	int bestIndex;

	height = goalHeight - curHeight;
	assert(height > 0);

	bestIndex = 0;
	bestDiff = I_fabs(s_mantleTrans[0].height - height);

	for ( transIndex = 1; transIndex < (int)ARRAY_COUNT(s_mantleTrans); transIndex++ )
	{
		diff = I_fabs(s_mantleTrans[transIndex].height - height);

		if ( diff < bestDiff )
		{
			bestIndex = transIndex;
			bestDiff = diff;
		}
	}

	return bestIndex;
}

/*
==================
Mantle_CalcEndPos
==================
*/
void Mantle_CalcEndPos( pmove_t *pm, MantleResults *mresults )
{
	trace_t trace;
	vec3_t start;
	vec3_t end;
	vec3_t mins;
	vec3_t maxs;
	playerState_t *ps;

	ps = pm->ps;
	assert(ps);

	if ( !(mresults->flags & 1) )
	{
		VectorCopy(mresults->ledgePos, mresults->endPos);
		return;
	}

	VectorCopy(ps->mins, mins);
	VectorCopy(ps->maxs, maxs);
	maxs[2] = 50.0;

	VectorCopy(mresults->ledgePos, start);
	VectorMA(start, 31.0, mresults->dir, start);

	VectorCopy(start, end);
	end[2] = end[2] - 18.0f;

	PM_trace(pm, &trace, start, mins, maxs, end, ps->clientNum, pm->tracemask);

	if ( trace.startsolid || trace.fraction < 1.0f )
	{
		mresults->flags &= ~1;
		VectorCopy(mresults->ledgePos, mresults->endPos);
	}
	else
	{
		VectorCopy(end, mresults->endPos);
		mresults->endPos[2] = (end[2] - start[2]) * trace.fraction + start[2];
	}
}

/*
==================
Mantle_Start
==================
*/
void Mantle_Start( pmove_t *pm, playerState_t *ps, MantleResults *mresults )
{
	MantleState *mstate;
	vec3_t trans;
	int mantleTime;

	mstate = &ps->mantleState;

	mstate->yaw = vectoyaw(mresults->dir);

	mstate->timer = 0;
	mstate->transIndex = Mantle_FindTransition(mresults->startPos[2], mresults->ledgePos[2]);
	mstate->flags = mresults->flags;

	mantleTime = Mantle_GetUpLength(mstate) + Mantle_GetOverLength(mstate);
	Mantle_GetAnimDelta(mstate, mantleTime, trans);

	VectorSubtract(mresults->endPos, trans, ps->origin);

	ps->pm_flags |= PMF_MANTLE;
	ps->eFlags |= EF_MANTLE;

	VectorCopy(mresults->endPos, pm->mantleEndPos);

	pm->mantleDuration = mantleTime;
	pm->mantleStarted = true;
}

/*
==================
Mantle_CheckLedge
==================
*/
bool Mantle_CheckLedge( pmove_t *pmove, pml_t *pml, MantleResults *mresults, float units )
{
	float m0;
	trace_t trace;
	vec3_t start;
	vec3_t end;
	vec3_t mins;
	vec3_t maxs;
	playerState_t *ps;

	ps = pmove->ps;
	assert(ps);

	m0 = ps->maxs[0];
	Mantle_DebugPrint(va("Checking for ledge at %f units", units));

	VectorSet(mins, -m0, -m0, 0.0);
	VectorSet(maxs, m0, m0, (m0 * 2.0f));

	VectorCopy(mresults->startPos, start);
	start[2] = start[2] + units;

	VectorMA(start, 16.0, mresults->dir, end);
	PM_trace(pmove, &trace, start, mins, maxs, end, ps->clientNum, pmove->tracemask);

	if ( trace.startsolid || trace.fraction < 1.0f )
	{
		Mantle_DebugPrint("Mantle Failed: Ledge is too far away");
		return false;
	}

	VectorCopy(end, start);
	end[2] = mresults->startPos[2] + 18.0f;

	PM_trace(pmove, &trace, start, mins, maxs, end, ps->clientNum, pmove->tracemask);

	if ( trace.startsolid || trace.fraction == 1.0f )
	{
		Mantle_DebugPrint("Mantle Failed: Can't find ledge");
		return false;
	}

	if ( trace.normal[2] < 0.69999999f )
	{
		Mantle_DebugPrint("Mantle Failed: Ledge is too steep");
		return false;
	}

	VectorCopy(end, mresults->ledgePos);
	mresults->ledgePos[2] = (end[2] - start[2]) * trace.fraction + start[2];
	assert((((mresults->ledgePos[2] - mresults->startPos[2]) > 0.0f)));
	maxs[2] = 50.0;

	PM_trace(pmove, &trace, mresults->ledgePos, mins, maxs, mresults->ledgePos, ps->clientNum, pmove->tracemask);

	if ( trace.startsolid )
	{
		Mantle_DebugPrint("Mantle Failed: Player can't fit crouched on ledge");
		return false;
	}

	ps->mantleState.flags |= 8;
	mresults->flags |= 8;

	Mantle_DebugPrint("Mantle available!");

	if ( !(pmove->cmd.buttons & BUTTON_JUMP) )
	{
		return true;
	}

	Mantle_CalcEndPos(pmove, mresults);

	if ( !(ps->eFlags & EF_CROUCH) )
	{
		PM_trace(pmove, &trace, mresults->ledgePos, ps->mins, ps->maxs, mresults->ledgePos, ps->clientNum, pmove->tracemask);

		if ( trace.startsolid )
		{
			mresults->flags |= 2;
		}

		PM_trace(pmove, &trace, mresults->endPos,  ps->mins, ps->maxs, mresults->endPos, ps->clientNum, pmove->tracemask);

		if ( !trace.startsolid )
		{
			mresults->flags |= 4;
		}
	}

	Mantle_Start(pmove, ps, mresults);
	return true;
}

/*
==================
Mantle_CreateAnims
==================
*/
void Mantle_CreateAnims( void *(*xanimAlloc)(int) )
{
	vec3_t delta;
	vec3_t rot;
	int animIndex;
	int transIndex;

	if ( s_mantleAnims )
	{
		return;
	}

	s_mantleAnims = XAnimCreateAnims("PLAYER_MANTLE", ARRAY_COUNT(s_mantleAnimNames), xanimAlloc);
	assert(s_mantleAnims);

	XAnimBlend(s_mantleAnims, 0, s_mantleAnimNames[0], 1, ARRAY_COUNT(s_mantleAnimNames) - 1, 0);

	for ( animIndex = 1; animIndex < (int)ARRAY_COUNT(s_mantleAnimNames); animIndex++ )
	{
		XAnimPrecache(s_mantleAnimNames[animIndex], MantleXAnimPrecacheAlloc);
		XAnimCreate(s_mantleAnims, animIndex, s_mantleAnimNames[animIndex]);
	}

	for ( transIndex = 0; transIndex < (int)ARRAY_COUNT(s_mantleTrans); transIndex++ )
	{
		animIndex = s_mantleTrans[transIndex].upAnimIndex;
		XAnimGetAbsDelta(s_mantleAnims, animIndex, rot, delta, 1.0);

		if ( I_fabs(delta[0] - 16.0f) > 1.0f )
		{
			Com_Error(ERR_DROP, "Mantle anim [%s] has X translation %f, should be %f\n", s_mantleAnimNames[animIndex], delta[0], 16.0);
		}

		if ( I_fabs(delta[1]) > 1.0f )
		{
			Com_Error(ERR_DROP, "Mantle anim [%s] has Y translation %f, should be %f\n", s_mantleAnimNames[animIndex], delta[1], 0.0);
		}

		if ( I_fabs(delta[2] - s_mantleTrans[transIndex].height) > 1.0f )
		{
			Com_Error(ERR_DROP, "Mantle anim [%s] has Z translation %f, should be %f\n", s_mantleAnimNames[animIndex], delta[2], s_mantleTrans[transIndex].height);
		}

		animIndex = s_mantleTrans[transIndex].overAnimIndex;
		XAnimGetAbsDelta(s_mantleAnims, animIndex, rot, delta, 1.0);

		if ( I_fabs(delta[0] - 31.0f) > 1.0f )
		{
			Com_Error(ERR_DROP, "Mantle anim [%s] has X translation %f, should be %f\n", s_mantleAnimNames[animIndex], delta[0], 31.0);
		}

		if ( I_fabs(delta[1]) > 1.0f )
		{
			Com_Error(ERR_DROP, "Mantle anim [%s] has Y translation %f, should be %f\n", s_mantleAnimNames[animIndex], delta[1], 0.0);
		}

		if ( I_fabs(delta[2] - -18.0f) > 1.0f )
		{
			Com_Error(ERR_DROP, "Mantle anim [%s] has Z translation %f, should be %f\n", s_mantleAnimNames[animIndex], delta[2], -18.0);
		}
	}
}

/*
==================
Mantle_ShutdownAnims
==================
*/
void Mantle_ShutdownAnims()
{
	s_mantleAnims = NULL;
}

/*
==================
Mantle_FindMantleSurface
==================
*/
bool Mantle_FindMantleSurface( pmove_t *pm, pml_t *pml, trace_t *trace, vec3_t mantleDir )
{
	float v16;
	vec3_t mins;
	vec3_t maxs;
	vec3_t start;
	vec3_t end;
	vec3_t traceDir;
	float v10;
	float v9;
	float v7;
	playerState_t *ps;

	ps = pm->ps;
	assert(ps);

	v16 = ps->maxs[0];

	VectorSet(mins, -mantle_check_radius->current.decimal, -mantle_check_radius->current.decimal, ps->mins[2]);
	VectorSet(maxs, mantle_check_radius->current.decimal, mantle_check_radius->current.decimal, ps->maxs[2]);

	assert((maxs[0] - mins[0]) <= (ps->maxs[2] - ps->mins[2]));
	assert((maxs[1] - mins[1]) <= (ps->maxs[2] - ps->mins[2]));

	v9 = v16 - mantle_check_radius->current.decimal;
	v10 = mantle_check_range->current.decimal + v9;

	VectorCopy(pml->forward, traceDir);
	traceDir[2] = 0;

	Vec3Normalize(traceDir);

	VectorMA(ps->origin, -v9, traceDir, start);
	VectorMA(ps->origin, v10, traceDir, end);

	PM_trace(pm, trace, start, mins, maxs, end, ps->clientNum, CONTENTS_MANTLE);

	if ( trace->startsolid || trace->allsolid )
	{
		Mantle_DebugPrint("Mantle Failed: Mantle brush is too thick");
		return false;
	}

	if ( trace->fraction == 1.0f )
	{
		Mantle_DebugPrint("Mantle Failed: No mantle surface found");
		return false;
	}

	if ( !(trace->surfaceFlags & (SURF_MANTLEON | SURF_MANTLEOVER)) )
	{
		Mantle_DebugPrint("Mantle Failed: No mantle surface with MANTLE_ON or MANTLE_OVER found");
		return false;
	}

	VectorNegate(trace->normal, mantleDir);
	mantleDir[2] = 0;

	v7 = Vec3Normalize(mantleDir);
	if ( v7 < 0.0001f )
	{
		Mantle_DebugPrint("Mantle Failed: Mantle surface has vertical normal");
		return false;
	}

	if ( acos( traceDir[1] * mantleDir[1] + (traceDir[2] * mantleDir[2] + traceDir[0] * mantleDir[0]) ) * 57.295776f > mantle_check_angle->current.decimal )
	{
		Mantle_DebugPrint("Mantle Failed: Player is not facing mantle surface");
		return false;
	}

	return true;
}

/*
==================
Mantle_Check
==================
*/
void Mantle_Check( pmove_t *pmove, pml_t *pml )
{
	MantleResults mresults;
	trace_t trace;
	vec3_t mantleDir;
	playerState_t *ps;

	Mantle_DebugPrint("====== Mantle Debug ======");

	if ( !mantle_enable->current.boolean )
	{
		Mantle_DebugPrint("Mantle Failed: Not enabled");
		return;
	}

	ps = pmove->ps;
	assert(ps);

	Mantle_ClearHint(ps);

	if ( ps->pm_type >= PM_DEAD )
	{
		Mantle_DebugPrint("Mantle Failed: Player is dead");
		return;
	}

	if ( ps->pm_flags & PMF_MANTLE )
	{
		Mantle_DebugPrint("Mantle Failed: Player already mantling");
		return;
	}

	if ( ps->eFlags & (EF_CROUCH | EF_PRONE) )
	{
		Mantle_DebugPrint("Mantle Failed: Player not standing");
		return;
	}

	if ( ps->weaponstate >= WEAPON_BINOCULARS_INIT && ps->weaponstate <= WEAPON_BINOCULARS_END )
	{
		Mantle_DebugPrint("Mantle Failed: Player using binoculars");
		return;
	}

	if ( !Mantle_FindMantleSurface(pmove, pml, &trace, mantleDir) )
	{
		return;
	}

	memset(&mresults, 0, sizeof(mresults));

	VectorCopy(mantleDir, mresults.dir);
	VectorCopy(ps->origin, mresults.startPos);

	if ( trace.surfaceFlags & SURF_MANTLEOVER )
	{
		mresults.flags |= 1;
	}

	if ( Mantle_CheckLedge(pmove, pml, &mresults, 60.0) )
	{
		return;
	}

	if ( Mantle_CheckLedge(pmove, pml, &mresults, 40.0) )
	{
		return;
	}

	Mantle_CheckLedge(pmove, pml, &mresults, 20.0);
}

/*
==================
Mantle_Move
==================
*/
void Mantle_Move( pmove_t *pm, playerState_t *ps, pml_t *pml )
{
	MantleState *mstate;
	vec3_t trans;
	vec3_t prevTrans;
	int prevTime;
	int deltaTime;
	int mantleLength;
	int animIndex;

	assert(ps->pm_flags & PMF_MANTLE);
	assert(pml->msec >= 0);

	if ( !mantle_enable->current.boolean )
	{
		return;
	}

	mstate = &ps->mantleState;
	mstate->flags &= ~8;

	if ( mstate->flags & 2 )
	{
		BG_AddPredictableEventToPlayerstate(EV_STANCE_FORCE_CROUCH, 0, ps);
	}

	mantleLength = Mantle_GetUpLength(mstate) + Mantle_GetOverLength(mstate);
	prevTime = mstate->timer;
	mstate->timer = mstate->timer + pml->msec;

	if ( mstate->timer > mantleLength )
	{
		mstate->timer = mantleLength;
	}

	deltaTime = mstate->timer - prevTime;

	Mantle_GetAnimDelta(mstate, prevTime, prevTrans);
	Mantle_GetAnimDelta(mstate, mstate->timer, trans);

	animIndex = Mantle_GetAnim(mstate);
	BG_AnimScriptAnimation(ps, AISTATE_COMBAT, animIndex + ANIM_MT_MANTLE_ROOT, qtrue);

	VectorSubtract(trans, prevTrans, trans);
	VectorAdd(trans, ps->origin, ps->origin);
	VectorScale(trans, 1.0f / (deltaTime * 0.001f), ps->velocity);

	if ( mstate->timer == mantleLength )
	{
		ps->pm_flags &= ~PMF_MANTLE;
		pm->mantleStarted = false;

		if ( mstate->flags & 1 )
		{
			BG_AnimScriptEvent(ps, ANIM_ET_JUMP, qfalse, qtrue);
		}

		if ( mstate->flags & 4 )
		{
			BG_AddPredictableEventToPlayerstate(EV_STANCE_FORCE_STAND, 0, ps);
			ps->eFlags &= ~EF_MANTLE;
		}
	}
}

/*
==================
Mantle_CapView
==================
*/
void Mantle_CapView( playerState_t *ps )
{
	float delta;
	float value;

	assert(ps);
	assert(ps->pm_flags & PMF_MANTLE);

	if ( !mantle_enable->current.boolean )
	{
		return;
	}

	delta = AngleDelta(ps->mantleState.yaw, ps->viewangles[YAW]);

	if ( delta >= -mantle_view_yawcap->current.decimal && delta <= mantle_view_yawcap->current.decimal )
	{
		return;
	}

	while ( delta < -mantle_view_yawcap->current.decimal )
	{
		delta = delta + mantle_view_yawcap->current.decimal;
	}

	while ( delta > mantle_view_yawcap->current.decimal )
	{
		delta = delta - mantle_view_yawcap->current.decimal;
	}

	value = delta > 0 ? -mantle_view_yawcap->current.decimal : mantle_view_yawcap->current.decimal;

	ps->delta_angles[YAW] += ANGLE2SHORT(delta);
	ps->viewangles[YAW] = AngleNormalize360Accurate(ps->mantleState.yaw + value);
}

/*
==================
Mantle_ClearHint
==================
*/
void Mantle_ClearHint( playerState_t *ps )
{
	assert(ps);
	ps->mantleState.flags &= ~8;
}

/*
==================
Mantle_IsWeaponInactive
==================
*/
bool Mantle_IsWeaponInactive( const playerState_t *ps )
{
	int animIndex;

	if ( !mantle_enable->current.boolean )
	{
		return false;
	}

	if ( !(ps->pm_flags & PMF_MANTLE) )
	{
		return false;
	}

	animIndex = s_mantleTrans[ps->mantleState.transIndex].overAnimIndex;

	return animIndex != MANTLE_OVER_LOW;
}


// Unreferenced storage; original declarations unknown (sized from the layout).
static int unusedStorage;
