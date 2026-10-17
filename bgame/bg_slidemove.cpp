#include "../qcommon/qcommon.h"
#include "bg_public.h"

/*

input: origin, velocity, bounds, groundPlane, trace function

output: origin, velocity, impacts, stairup boolean

*/

#define MAX_CLIP_PLANES 8

qboolean PM_VerifyPronePosition( pmove_t *pm, const vec3_t vFallbackOrg, const vec3_t vFallbackVel );
static float PM_PermuteRestrictiveClipPlanes(const vec3_t velocity, int planeCount, const vec3_t *planes, int *permutation);
qboolean PM_SlideMove(pmove_t *pm, pml_t *pml, qboolean gravity);
void PM_StepSlideMove( pmove_t *pm, pml_t *pml, qboolean gravity );

/*
==================
PM_VerifyPronePosition
==================
*/
qboolean PM_VerifyPronePosition( pmove_t *pm, const vec3_t vFallbackOrg, const vec3_t vFallbackVel )
{
	playerState_t *ps;
	qboolean bProneOK;

	ps = pm->ps;
	assert(ps);

	if ( ps->pm_flags & PMF_PRONE )
	{
		bProneOK = BG_CheckProne( ps->clientNum, ps->origin, ps->maxs[0], 30.0,
		                          ps->proneDirection, &ps->fTorsoHeight, &ps->fTorsoPitch, &ps->fWaistPitch,
		                          qtrue, qtrue, NULL, pm->handler, PCT_CLIENT, 66.0 );

		if ( !bProneOK )
		{
			VectorCopy(vFallbackOrg, ps->origin);
			VectorCopy(vFallbackVel, ps->velocity);
		}

		return bProneOK;
	}

	return qtrue;
}

/*
==================
PM_PermuteRestrictiveClipPlanes
==================
*/
static float PM_PermuteRestrictiveClipPlanes(const vec3_t velocity, int planeCount, const vec3_t *planes, int *permutation)
{
	int planeIndex;
	float parallel[MAX_CLIP_PLANES];
	int permutedIndex;

	for (planeIndex = 0; planeIndex < planeCount; planeIndex++)
	{
		parallel[planeIndex] = DotProduct(velocity, planes[planeIndex]);

		for (permutedIndex = planeIndex; permutedIndex; permutedIndex--)
		{
			if (parallel[permutation[permutedIndex - 1]] < parallel[planeIndex])
				break;

			permutation[permutedIndex] = permutation[permutedIndex - 1];
		}

		permutation[permutedIndex] = planeIndex;
	}

	return parallel[*permutation];
}

/*
==================
PM_SlideMove
==================
*/
qboolean PM_SlideMove(pmove_t *pm, pml_t *pml, qboolean gravity)
{
	int bumpcount, numbumps;
	vec3_t dir;
	float d;
	int numplanes;
	vec3_t planes[MAX_CLIP_PLANES];
	int permutation[MAX_CLIP_PLANES];
	vec3_t primal_velocity;
	vec3_t clipVelocity;
	int i, j, k;
	trace_t trace;
	vec3_t end;
	float time_left;
	float into;
	vec3_t endVelocity;
	vec3_t endClipVelocity;
	playerState_t *ps;

	ps = pm->ps;

	numbumps = 4;

	VectorCopy(ps->velocity, primal_velocity);
	VectorCopy(ps->velocity, endVelocity);

	if (gravity)
	{
		endVelocity[2] = endVelocity[2] - (float)ps->gravity * pml->frametime;
		ps->velocity[2] = (ps->velocity[2] + endVelocity[2]) * 0.5f;
		primal_velocity[2] = endVelocity[2];

		if (pml->groundPlane)
		{
			PM_ClipVelocity(ps->velocity, pml->groundTrace.normal, ps->velocity);
		}
	}

	time_left = pml->frametime;

	if (pml->groundPlane)
	{
		VectorCopy(pml->groundTrace.normal, planes[0]);
		numplanes = 1;
	}
	else
	{
		numplanes = 0;
	}

	Vec3NormalizeTo(ps->velocity, planes[numplanes]);
	numplanes++;

	for (bumpcount = 0; bumpcount < numbumps; bumpcount++)
	{
		VectorMA(ps->origin, time_left, ps->velocity, end);

		PM_playerTrace(pm, &trace, ps->origin, pm->mins, pm->maxs, end, ps->clientNum, pm->tracemask);

		if (trace.allsolid)
		{
			ps->velocity[2] = 0;
			return 1;
		}

		if (trace.fraction > 0)
		{
			Vec3Lerp(ps->origin, end, trace.fraction, ps->origin);
		}

		if (trace.fraction == 1)
		{
			break;
		}

		PM_AddTouchEnt(pm, trace.entityNum);

		time_left -= time_left * trace.fraction;

		if (numplanes >= MAX_CLIP_PLANES)
		{
			VectorClear(ps->velocity);
			return 1;
		}

		for (i = 0; i < numplanes; i++)
		{
			if (DotProduct(trace.normal, planes[i]) > 0.99900001f)
			{
				PM_ClipVelocity(ps->velocity, trace.normal, ps->velocity);
				VectorAdd(trace.normal, ps->velocity, ps->velocity);
				break;
			}
		}

		if (i < numplanes)
		{
			continue;
		}
		VectorCopy(trace.normal, planes[numplanes]);
		numplanes++;

		into = PM_PermuteRestrictiveClipPlanes(ps->velocity, numplanes, planes, permutation);

		if (into >= 0.1f)
		{
			continue;
		}

		if (-into > pml->impactSpeed)
		{
			pml->impactSpeed = -into;
		}

		PM_ClipVelocity(ps->velocity, planes[permutation[0]], clipVelocity);

		PM_ClipVelocity(endVelocity, planes[permutation[0]], endClipVelocity);

		for (j = 1; j < numplanes; j++)
		{
			if (DotProduct(clipVelocity, planes[permutation[j]]) >= 0.1f)
			{
				continue;
			}

			PM_ClipVelocity(clipVelocity, planes[permutation[j]], clipVelocity);
			PM_ClipVelocity(endClipVelocity, planes[permutation[j]], endClipVelocity);

			if (DotProduct(clipVelocity, planes[permutation[0]]) >= 0)
			{
				continue;
			}

			Vec3Cross(planes[permutation[0]], planes[permutation[j]], dir);
			Vec3Normalize(dir);
			d = DotProduct(dir, ps->velocity);
			VectorScale(dir, d, clipVelocity);

			d = DotProduct(dir, endVelocity);
			VectorScale(dir, d, endClipVelocity);

			for (k = 1; k < numplanes; k++)
			{
				if (k == j)
				{
					continue;
				}

				if (DotProduct(clipVelocity, planes[permutation[k]]) >= 0.1f)
				{
					continue;
				}

				VectorClear(ps->velocity);
				return 1;
			}
		}

		VectorCopy(clipVelocity, ps->velocity);
		VectorCopy(endClipVelocity, endVelocity);
	}

	if (gravity)
	{
		VectorCopy(endVelocity, ps->velocity);
	}

	if (ps->pm_time)
	{
		VectorCopy(primal_velocity, ps->velocity);
	}

	return (qboolean)(bumpcount != 0);
}

/*
==================
PM_StepSlideMove
==================
*/
void PM_StepSlideMove( pmove_t *pm, pml_t *pml, qboolean gravity )
{
	vec3_t start_o;
	vec3_t start_v;
	vec3_t down_o;
	vec3_t down_v;
	trace_t trace;
	vec3_t up;
	vec3_t down;
	vec2_t flatDelta;
	vec2_t vel;
	qboolean bCloser;
	float fStepSize;
	int iBumps;
	float fStepAmount;
	int bHadGround;
	vec3_t endpos;
	qboolean jumping;
	playerState_t *ps;
	int iDelta;
	float fScale;
	float fStepHeight;
	float fSpeedScale;
	int old;
	float fBobDelta;

	fStepAmount = 0;

	ps = pm->ps;

	jumping = qfalse;

	if (ps->pm_flags & PMF_LADDER)
	{
		bHadGround = qfalse;
		Jump_ClearState(ps);
	}
	else if (pml->groundPlane)
	{
		bHadGround = qtrue;
	}
	else
	{
		bHadGround = qfalse;

		if (ps->pm_flags & PMF_TIME_LAND && ps->pm_time)
		{
			Jump_ClearState(ps);
		}
	}

	VectorCopy(ps->origin, start_o);
	VectorCopy(ps->velocity, start_v);

	iBumps = PM_SlideMove(pm, pml, gravity);

	if (ps->pm_flags & PMF_PRONE)
	{
		fStepSize = 10.0f;
	}
	else
	{
		fStepSize = 18.0f;
	}

	if (ps->groundEntityNum == ENTITYNUM_NONE)
	{
		if (ps->pm_flags & PMF_TIME_LAND && ps->pm_time)
		{
			Jump_ClearState(ps);
		}

		if (iBumps && ps->pm_flags & PMF_TIME_LAND)
		{
			if (Jump_GetStepHeight(ps, start_o, &fStepSize))
			{
				if (fStepSize < 1)
				{
					return;
				}

				jumping = qtrue;
			}
		}

		if (!jumping && !(ps->pm_flags & PMF_LADDER && ps->velocity[2] > 0))
		{
			return;
		}
	}

	VectorCopy(ps->origin, down_o);
	VectorCopy(ps->velocity, down_v);

	Vector2Subtract(down_o, start_o, flatDelta);

	if (iBumps)
	{
		VectorCopy(start_o, up);
		up[2] += fStepSize + 1.0f;

		PM_playerTrace(pm, &trace, start_o, pm->mins, pm->maxs, up, ps->clientNum, pm->tracemask);

		fStepAmount = (fStepSize + 1.0f) * trace.fraction - 1.0f;

		if (fStepAmount < 1)
		{
			fStepAmount = 0;
		}
		else
		{
			VectorSet(ps->origin, up[0], up[1], start_o[2] + fStepAmount);
			VectorCopy(start_v, ps->velocity);

			PM_SlideMove(pm, pml, gravity);
		}
	}

	if (bHadGround || fStepAmount != 0)
	{
		VectorCopy(ps->origin, down);
		down[2] -= fStepAmount;

		if (bHadGround)
		{
			down[2] -= 9.0f;
		}

		PM_playerTrace(pm, &trace, ps->origin, pm->mins, pm->maxs, down, ps->clientNum, pm->tracemask);

		if (trace.entityNum < MAX_CLIENTS)
		{
			VectorCopy(down_o, ps->origin);
			VectorCopy(down_v, ps->velocity);
			return;
		}

		if (trace.fraction < 1)
		{
			if (trace.normal[2] < 0.30000001f)
			{
				VectorCopy(down_o, ps->origin);
				VectorCopy(down_v, ps->velocity);
				return;
			}

			Vec3Lerp(ps->origin, down, trace.fraction, ps->origin);
			PM_ClipVelocity(ps->velocity, trace.normal, ps->velocity);
		}
		else
		{
			if (fStepAmount != 0)
			{
				ps->origin[2] = ps->origin[2] - fStepAmount;
			}
		}
	}

	Vector2Subtract(ps->origin, start_o, vel);
	bCloser = Dot2Product(vel, ps->velocity) <= Dot2Product(flatDelta, ps->velocity) + 0.001f;

	if (bCloser || jumping && Jump_IsPlayerAboveMax(ps))
	{
		VectorCopy(down_o, ps->origin);
		VectorCopy(down_v, ps->velocity);

		fStepAmount = 0;

		if (bHadGround)
		{
			VectorCopy(ps->origin, down);
			down[2] -= 9.0f;

			PM_playerTrace(pm, &trace, ps->origin, pm->mins, pm->maxs, down, ps->clientNum, pm->tracemask);

			if (trace.fraction < 1.0f)
			{
				Vec3Lerp(ps->origin, down, trace.fraction, endpos);
				fStepAmount = endpos[2] - ps->origin[2];
				VectorCopy(endpos, ps->origin);
				PM_ClipVelocity(ps->velocity, trace.normal, ps->velocity);
			}
		}
	}

	if (jumping)
	{
		Jump_ClampVelocity(ps, down_o);
	}

	if (bHadGround)
	{
		if (ps->pm_type < PM_DEAD)
		{
			if (PM_VerifyPronePosition(pm, start_o, start_v))
			{
				if (I_fabs(ps->origin[2] - down_o[2]) > 0.5f)
				{
					iDelta = I_fround(ps->origin[2] - down_o[2]);

					if (iDelta)
					{
						fScale = 0.80000001f;

						if (iDelta < -16)
						{
							iDelta = -16;
						}
						else if (iDelta > 24)
						{
							iDelta = 24;
						}

						iDelta += 128;
						BG_AddPredictableEventToPlayerstate(EV_STEP_VIEW, iDelta, ps);
						fStepHeight = I_fabs(ps->origin[2] - start_o[2]);
						fSpeedScale = (1.0f - fStepHeight / fStepSize) * 0.80000001f + 0.19999999f;
						VectorScale(ps->velocity, fSpeedScale, ps->velocity);
						iDelta -= 128;

						if (abs(iDelta) > 3)
						{
							if (ps->groundEntityNum != ENTITYNUM_NONE)
							{
								if (PM_ShouldMakeFootsteps(pm))
								{
									iDelta = abs(iDelta) / 2;

									if (iDelta > 4)
									{
										iDelta = 4;
									}

									fBobDelta = iDelta * 1.25f + 7.0f;
									old = ps->bobCycle;
									ps->bobCycle = (int)(old + fBobDelta) & 255;
									PM_FootstepEvent(pm, pml, old, ps->bobCycle, qtrue);
								}
							}
						}
					}
				}
			}
		}
	}
}

