#define SCE_CONST const
#include "../qcommon/qcommon.h"
#include "bg_public.h"


dvar_t *player_view_pitch_up;
dvar_t *player_view_pitch_down;
dvar_t *bg_ladder_yawcap;
dvar_t *bg_prone_yawcap;
dvar_t *bg_foliagesnd_minspeed;
dvar_t *bg_foliagesnd_maxspeed;
dvar_t *bg_foliagesnd_slowinterval;
dvar_t *bg_foliagesnd_fastinterval;
dvar_t *bg_foliagesnd_resetinterval;
dvar_t *bg_fallDamageMinHeight;
dvar_t *bg_fallDamageMaxHeight;
dvar_t *inertiaMax;
dvar_t *inertiaDebug;
dvar_t *inertiaAngle;
dvar_t *stopspeed;
dvar_t *friction;
dvar_t *bg_swingSpeed;
dvar_t *bg_bobAmplitudeStanding;
dvar_t *bg_bobAmplitudeDucked;
dvar_t *bg_bobAmplitudeProne;
dvar_t *bg_bobMax;
dvar_t *bg_aimSpreadMoveSpeedThreshold;
dvar_t *player_breath_hold_time;
dvar_t *player_breath_gasp_time;
dvar_t *player_breath_fire_delay;
dvar_t *player_breath_gasp_scale;
dvar_t *player_breath_hold_lerp;
dvar_t *player_breath_gasp_lerp;
dvar_t *player_breath_snd_lerp;
dvar_t *player_breath_snd_delay;
dvar_t *player_toggleBinoculars;
dvar_t *player_scopeExitOnDamage;
dvar_t *player_adsExitDelay;
dvar_t *player_moveThreshhold;
dvar_t *player_footstepsThreshhold;
dvar_t *player_strafeSpeedScale;
dvar_t *player_backSpeedScale;
dvar_t *player_spectateSpeedScale;
dvar_t *player_turnAnims;
dvar_t *player_dmgtimer_timePerPoint;
dvar_t *player_dmgtimer_maxTime;
dvar_t *player_dmgtimer_minScale;
dvar_t *player_dmgtimer_stumbleTime;
dvar_t *player_dmgtimer_flinchTime;

const char *eventnames[] =
{
	"EV_NONE",
	"EV_FOOTSTEP_RUN_DEFAULT",
	"EV_FOOTSTEP_RUN_BARK",
	"EV_FOOTSTEP_RUN_BRICK",
	"EV_FOOTSTEP_RUN_CARPET",
	"EV_FOOTSTEP_RUN_CLOTH",
	"EV_FOOTSTEP_RUN_CONCRETE",
	"EV_FOOTSTEP_RUN_DIRT",
	"EV_FOOTSTEP_RUN_FLESH",
	"EV_FOOTSTEP_RUN_FOLIAGE",
	"EV_FOOTSTEP_RUN_GLASS",
	"EV_FOOTSTEP_RUN_GRASS",
	"EV_FOOTSTEP_RUN_GRAVEL",
	"EV_FOOTSTEP_RUN_ICE",
	"EV_FOOTSTEP_RUN_METAL",
	"EV_FOOTSTEP_RUN_MUD",
	"EV_FOOTSTEP_RUN_PAPER",
	"EV_FOOTSTEP_RUN_PLASTER",
	"EV_FOOTSTEP_RUN_ROCK",
	"EV_FOOTSTEP_RUN_SAND",
	"EV_FOOTSTEP_RUN_SNOW",
	"EV_FOOTSTEP_RUN_WATER",
	"EV_FOOTSTEP_RUN_WOOD",
	"EV_FOOTSTEP_RUN_ASPHALT",
	"EV_FOOTSTEP_WALK_DEFAULT",
	"EV_FOOTSTEP_WALK_BARK",
	"EV_FOOTSTEP_WALK_BRICK",
	"EV_FOOTSTEP_WALK_CARPET",
	"EV_FOOTSTEP_WALK_CLOTH",
	"EV_FOOTSTEP_WALK_CONCRETE",
	"EV_FOOTSTEP_WALK_DIRT",
	"EV_FOOTSTEP_WALK_FLESH",
	"EV_FOOTSTEP_WALK_FOLIAGE",
	"EV_FOOTSTEP_WALK_GLASS",
	"EV_FOOTSTEP_WALK_GRASS",
	"EV_FOOTSTEP_WALK_GRAVEL",
	"EV_FOOTSTEP_WALK_ICE",
	"EV_FOOTSTEP_WALK_METAL",
	"EV_FOOTSTEP_WALK_MUD",
	"EV_FOOTSTEP_WALK_PAPER",
	"EV_FOOTSTEP_WALK_PLASTER",
	"EV_FOOTSTEP_WALK_ROCK",
	"EV_FOOTSTEP_WALK_SAND",
	"EV_FOOTSTEP_WALK_SNOW",
	"EV_FOOTSTEP_WALK_WATER",
	"EV_FOOTSTEP_WALK_WOOD",
	"EV_FOOTSTEP_WALK_ASPHALT",
	"EV_FOOTSTEP_PRONE_DEFAULT",
	"EV_FOOTSTEP_PRONE_BARK",
	"EV_FOOTSTEP_PRONE_BRICK",
	"EV_FOOTSTEP_PRONE_CARPET",
	"EV_FOOTSTEP_PRONE_CLOTH",
	"EV_FOOTSTEP_PRONE_CONCRETE",
	"EV_FOOTSTEP_PRONE_DIRT",
	"EV_FOOTSTEP_PRONE_FLESH",
	"EV_FOOTSTEP_PRONE_FOLIAGE",
	"EV_FOOTSTEP_PRONE_GLASS",
	"EV_FOOTSTEP_PRONE_GRASS",
	"EV_FOOTSTEP_PRONE_GRAVEL",
	"EV_FOOTSTEP_PRONE_ICE",
	"EV_FOOTSTEP_PRONE_METAL",
	"EV_FOOTSTEP_PRONE_MUD",
	"EV_FOOTSTEP_PRONE_PAPER",
	"EV_FOOTSTEP_PRONE_PLASTER",
	"EV_FOOTSTEP_PRONE_ROCK",
	"EV_FOOTSTEP_PRONE_SAND",
	"EV_FOOTSTEP_PRONE_SNOW",
	"EV_FOOTSTEP_PRONE_WATER",
	"EV_FOOTSTEP_PRONE_WOOD",
	"EV_FOOTSTEP_PRONE_ASPHALT",
	"EV_JUMP_DEFAULT",
	"EV_JUMP_BARK",
	"EV_JUMP_BRICK",
	"EV_JUMP_CARPET",
	"EV_JUMP_CLOTH",
	"EV_JUMP_CONCRETE",
	"EV_JUMP_DIRT",
	"EV_JUMP_FLESH",
	"EV_JUMP_FOLIAGE",
	"EV_JUMP_GLASS",
	"EV_JUMP_GRASS",
	"EV_JUMP_GRAVEL",
	"EV_JUMP_ICE",
	"EV_JUMP_METAL",
	"EV_JUMP_MUD",
	"EV_JUMP_PAPER",
	"EV_JUMP_PLASTER",
	"EV_JUMP_ROCK",
	"EV_JUMP_SAND",
	"EV_JUMP_SNOW",
	"EV_JUMP_WATER",
	"EV_JUMP_WOOD",
	"EV_JUMP_ASPHALT",
	"EV_LANDING_DEFAULT",
	"EV_LANDING_BARK",
	"EV_LANDING_BRICK",
	"EV_LANDING_CARPET",
	"EV_LANDING_CLOTH",
	"EV_LANDING_CONCRETE",
	"EV_LANDING_DIRT",
	"EV_LANDING_FLESH",
	"EV_LANDING_FOLIAGE",
	"EV_LANDING_GLASS",
	"EV_LANDING_GRASS",
	"EV_LANDING_GRAVEL",
	"EV_LANDING_ICE",
	"EV_LANDING_METAL",
	"EV_LANDING_MUD",
	"EV_LANDING_PAPER",
	"EV_LANDING_PLASTER",
	"EV_LANDING_ROCK",
	"EV_LANDING_SAND",
	"EV_LANDING_SNOW",
	"EV_LANDING_WATER",
	"EV_LANDING_WOOD",
	"EV_LANDING_ASPHALT",
	"EV_LANDING_PAIN_DEFAULT",
	"EV_LANDING_PAIN_BARK",
	"EV_LANDING_PAIN_BRICK",
	"EV_LANDING_PAIN_CARPET",
	"EV_LANDING_PAIN_CLOTH",
	"EV_LANDING_PAIN_CONCRETE",
	"EV_LANDING_PAIN_DIRT",
	"EV_LANDING_PAIN_FLESH",
	"EV_LANDING_PAIN_FOLIAGE",
	"EV_LANDING_PAIN_GLASS",
	"EV_LANDING_PAIN_GRASS",
	"EV_LANDING_PAIN_GRAVEL",
	"EV_LANDING_PAIN_ICE",
	"EV_LANDING_PAIN_METAL",
	"EV_LANDING_PAIN_MUD",
	"EV_LANDING_PAIN_PAPER",
	"EV_LANDING_PAIN_PLASTER",
	"EV_LANDING_PAIN_ROCK",
	"EV_LANDING_PAIN_SAND",
	"EV_LANDING_PAIN_SNOW",
	"EV_LANDING_PAIN_WATER",
	"EV_LANDING_PAIN_WOOD",
	"EV_LANDING_PAIN_ASPHALT",
	"EV_FOLIAGE_SOUND",
	"EV_STANCE_FORCE_STAND",
	"EV_STANCE_FORCE_CROUCH",
	"EV_STANCE_FORCE_PRONE",
	"EV_STEP_VIEW",
	"EV_ITEM_PICKUP",
	"EV_AMMO_PICKUP",
	"EV_NOAMMO",
	"EV_EMPTYCLIP",
	"EV_EMPTY_OFFHAND",
	"EV_RESET_ADS",
	"EV_RELOAD",
	"EV_RELOAD_FROM_EMPTY",
	"EV_RELOAD_START",
	"EV_RELOAD_END",
	"EV_RAISE_WEAPON",
	"EV_PUTAWAY_WEAPON",
	"EV_WEAPON_ALT",
	"EV_PULLBACK_WEAPON",
	"EV_FIRE_WEAPON",
	"EV_FIRE_WEAPONB",
	"EV_FIRE_WEAPON_LASTSHOT",
	"EV_RECHAMBER_WEAPON",
	"EV_EJECT_BRASS",
	"EV_MELEE_SWIPE",
	"EV_FIRE_MELEE",
	"EV_PREP_OFFHAND",
	"EV_USE_OFFHAND",
	"EV_SWITCH_OFFHAND",
	"EV_BINOCULAR_ENTER",
	"EV_BINOCULAR_EXIT",
	"EV_BINOCULAR_FIRE",
	"EV_BINOCULAR_RELEASE",
	"EV_BINOCULAR_DROP",
	"EV_MELEE_HIT",
	"EV_MELEE_MISS",
	"EV_FIRE_WEAPON_MG42",
	"EV_FIRE_QUADBARREL_1",
	"EV_FIRE_QUADBARREL_2",
	"EV_BULLET_TRACER",
	"EV_SOUND_ALIAS",
	"EV_SOUND_ALIAS_AS_MASTER",
	"EV_BULLET_HIT_SMALL",
	"EV_BULLET_HIT_LARGE",
	"EV_SHOTGUN_HIT",
	"EV_BULLET_HIT_AP",
	"EV_BULLET_HIT_CLIENT_SMALL",
	"EV_BULLET_HIT_CLIENT_LARGE",
	"EV_GRENADE_BOUNCE",
	"EV_GRENADE_EXPLODE",
	"EV_ROCKET_EXPLODE",
	"EV_ROCKET_EXPLODE_NOMARKS",
	"EV_CUSTOM_EXPLODE",
	"EV_CUSTOM_EXPLODE_NOMARKS",
	"EV_BULLET",
	"EV_PLAY_FX",
	"EV_PLAY_FX_ON_TAG",
	"EV_EARTHQUAKE",
	"EV_GRENADE_SUICIDE",
	"EV_OBITUARY"
};

// events that are only sent to the client that caused them
SCE_CONST int singleClientEvents[] =
{
	EV_STANCE_FORCE_STAND,
	EV_STANCE_FORCE_CROUCH,
	EV_STANCE_FORCE_PRONE,
	EV_STEP_VIEW,
	EV_BULLET_HIT_CLIENT_SMALL,
	EV_BULLET_HIT_CLIENT_LARGE,
	-1,
};
#define AMMOFORWEAP BG_FindAmmoForWeapon( item->giTag )

void BG_EvaluateTrajectory( const trajectory_t *tr, int atTime, vec3_t result );

void BG_RegisterDvars();
gitem_t* BG_FindItemForWeapon( int weapon );
gitem_t* G_FindItem( const char *pickupName );
qboolean BG_PlayerTouchesItem( const playerState_t *ps, const entityState_t *item, int atTime );
qboolean BG_CanItemBeGrabbed( const entityState_t *ent, const playerState_t *ps, qboolean bTouched );
void BG_EvaluateTrajectory( const trajectory_t *tr, int atTime, vec3_t result );
void BG_EvaluateTrajectoryDelta( const trajectory_t *tr, int atTime, vec3_t result );
void BG_GetMarkDir( const vec3_t dir, const vec3_t normal, vec3_t out );
void BG_AddPredictableEventToPlayerstate( int newEvent, int eventParm, playerState_t *ps );
void BG_PlayerStateToEntityState( playerState_t *ps, entityState_t *s, qboolean snap, byte handler );
void BG_LerpHudColors( const hudelem_t *elem, int time, hudelem_color_t *toColor );

/*
============
BG_CheckProneValid
============
*/
static qboolean BG_CheckProneValid( int passEntityNum, const vec3_t vPos, float fSize,  float fHeight,float fYaw,
                                    float *pfTorsoHeight, float *pfTorsoPitch, float *pfWaistPitch, qboolean bAlreadyProne, qboolean bOnGround,
                                    const vec3_t vGroundNormal, byte handler, int proneCheckType,  float prone_feet_dist)
{
	int contentMask;
	trace_t trace;
	vec3_t vEnd;
	vec3_t vForward;
	vec3_t vMins;
	vec3_t vMaxs;
	vec3_t vFeetPos;
	vec3_t vDelta;
	vec3_t vRight;
	vec3_t vUp;
	vec3_t vStart;
	vec3_t vTorsoPos;
	vec3_t vWaistPos;
	float fTorsoPitch;
	float fWaistPitch;
	float dist;
	float fFirstTraceDist;
	float fTraceHeight;
	float fWaistDistance;
	int bFirstTraceHit;
	float fPitchDiff;
	void (*traceFunc)(trace_t *, const float *, const float *, const float *, const float *, int, int);
	byte bLocational;
	bFirstTraceHit = qfalse;
	bLocational = qtrue;
	traceFunc = pmoveHandlers[handler].trace;
	VectorSet(vMins, -fSize, -fSize, 0);
	VectorAdd(vMins, vPos, vMins);
	VectorSet(vMaxs, fSize, fSize, fHeight);
	VectorAdd(vMaxs, vPos, vMaxs);
	if ( !proneCheckType )
	{
		contentMask = CONTENTS_SOLID | CONTENTS_GLASS | CONTENTS_PLAYERCLIP | CONTENTS_UNKNOWN;
	}
	else
	{
		contentMask = CONTENTS_SOLID | CONTENTS_GLASS | CONTENTS_MONSTERCLIP | CONTENTS_UNKNOWN;
	}
	if ( !bAlreadyProne )
	{
		VectorSet(vMins, -fSize, -fSize, 0);
		VectorSet(vMaxs, fSize, fSize, fHeight);
		VectorCopy(vPos, vEnd);
		VectorCopy(vPos, vForward);
		vForward[2] = vForward[2] + 10.0f;
		traceFunc(&trace, vEnd, vMins, vMaxs, vForward, passEntityNum, contentMask);
		if ( trace.allsolid )
		{
			return qfalse;
		}
	}
	if ( bOnGround && vGroundNormal && vGroundNormal[2] < 0.7f )
	{
		return qfalse;
	}
	VectorSet(vMins, -6.0f, -6.0f, -6.0f);
	VectorSet(vMaxs, 6.0f, 6.0f, 6.0f);
	VectorSet(vForward, 0, fYaw - 180.0f, 0);
	AngleVectors(vForward, vDelta, vRight, vUp);
	fFirstTraceDist = fHeight - 6.0f;
	fTraceHeight = fFirstTraceDist - 6.0f;
	VectorCopy(vPos, vEnd);
	vEnd[2] = vEnd[2] + fFirstTraceDist;
	VectorMA(vEnd, prone_feet_dist - 6.0f, vDelta, vForward);
	traceFunc(&trace, vEnd, vMins, vMaxs, vForward, passEntityNum, contentMask);
	if ( trace.fraction < 1.0f )
	{
		if ( !bOnGround )
		{
			return qfalse;
		}
		bFirstTraceHit = qtrue;
		fWaistDistance = (prone_feet_dist - 6.0f) * trace.fraction + 6.0f;
		if ( fSize + 2.0f > fWaistDistance )
		{
			return qfalse;
		}
		if ( fWaistDistance < fFirstTraceDist * 0.7f + 48.0f )
		{
			bFirstTraceHit = qfalse;
			vForward[2] = vForward[2] + 22.0f;
			VectorSubtract(vForward, vEnd, vFeetPos);
			dist = Vec3NormalizeTo(vFeetPos, vDelta);
			traceFunc(&trace, vEnd, vMins, vMaxs, vForward, passEntityNum, contentMask);
			if ( trace.fraction < 1.0f )
			{
				bFirstTraceHit = qtrue;
				fWaistDistance = trace.fraction * dist + 6.0f;
				if ( fWaistDistance < fFirstTraceDist * 0.7f + 48.0f )
				{
					return qfalse;
				}
			}
			else
			{
				fWaistDistance = prone_feet_dist;
			}
		}
	}
	else
	{
		fWaistDistance = prone_feet_dist;
	}
	Vec3Lerp(vEnd, vForward, trace.fraction, vWaistPos);
	VectorMA(vPos, 48.0f, vDelta, vEnd);
	vEnd[2] = vEnd[2] + fFirstTraceDist;
	VectorCopy(vEnd, vForward);
	vForward[2] = vForward[2] - (fSize * 2.5f + fFirstTraceDist - 6.0f);
	traceFunc(&trace, vEnd, vMins, vMaxs, vForward, passEntityNum, contentMask);
	if ( trace.fraction == 1.0f )
	{
		goto fail;
	}
	if ( trace.normal[2] < 0.7f )
	{
		return qfalse;
	}
	fPitchDiff = (fSize * 2.5f + fFirstTraceDist - 6.0f) * trace.fraction + 6.0f;
	Vec3Lerp(vEnd, vForward, trace.fraction, vTorsoPos);
	vTorsoPos[2] = vTorsoPos[2] - 6.0f;
	if ( bFirstTraceHit )
	{
		if ( fWaistDistance - fPitchDiff < fPitchDiff * -0.75f )
		{
			goto fail;
		}
		VectorSubtract(vWaistPos, vTorsoPos, vFeetPos);
		VectorMA(vFeetPos, 6.0f, vDelta, vFeetPos);
		vFeetPos[2] = (vFeetPos[2] + 6.0f);
		Vec3Normalize(vFeetPos);
		VectorMA(vEnd, prone_feet_dist - 6.0f - 48.0f, vFeetPos, vForward);
		vForward[0] = (vPos[0] + (prone_feet_dist - 6.0f) * vDelta[0] + vForward[0]) * 0.5f;
		vForward[1] = (vPos[1] + (prone_feet_dist - 6.0f) * vDelta[1] + vForward[1]) * 0.5f;
		traceFunc(&trace, vEnd, vMins, vMaxs, vForward, passEntityNum, contentMask);
		if ( trace.fraction < 1.0f )
		{
			Vec3Lerp(vEnd, vForward, trace.fraction, vEnd);
			vEnd[2] = vEnd[2] + 18.0f;
			vForward[2] = vForward[2] + 18.0f;
			traceFunc(&trace, vEnd, vMins, vMaxs, vForward, passEntityNum, contentMask);
			if ( trace.fraction < 1.0f )
			{
				goto fail;
			}
		}
		Vec3Lerp(vEnd, vForward, trace.fraction, vWaistPos);
	}
	VectorCopy(vWaistPos, vEnd);
	VectorCopy(vEnd, vForward);
	vForward[2] = vForward[2] - ((vEnd[2] - vTorsoPos[2]) * 2.0f + fSize);
	traceFunc(&trace, vEnd, vMins, vMaxs, vForward, passEntityNum, contentMask);
	if ( trace.fraction == 1.0f )
	{
		goto fail;
	}
	if ( trace.normal[2] < 0.7f )
	{
		return qfalse;
	}
	Vec3Lerp(vEnd, vForward, trace.fraction, vWaistPos);
	vWaistPos[2] = vWaistPos[2] - 6.0f;
	VectorCopy(vPos, vStart);
	VectorSubtract(vTorsoPos, vStart, vFeetPos);
	fTorsoPitch = vectopitch(vFeetPos);
	VectorSubtract(vWaistPos, vTorsoPos, vFeetPos);
	fWaistPitch = vectopitch(vFeetPos);
	dist = AngleSubtract(fWaistPitch, fTorsoPitch);
	if ( dist < -50.0f || dist > 70.0f )
	{
		bLocational = qfalse;
	}
	VectorSet(vMins, -0.0f, -0.0f, -0.0f);
	VectorSet(vMaxs, 0.0f, 0.0f, 0.0f);
	VectorCopy(vStart, vEnd);
	vEnd[2] = vEnd[2] + 5.0f;
	VectorCopy(vTorsoPos, vForward);
	vForward[2] = vForward[2] + 5.0f;
	traceFunc(&trace, vEnd, vMins, vMaxs, vForward, passEntityNum, contentMask);
	if ( trace.fraction < 1.0f )
	{
		bLocational = qfalse;
	}
	VectorCopy(vForward, vEnd);
	VectorCopy(vWaistPos, vForward);
	vForward[2] = vForward[2] + 5.0f;
	traceFunc(&trace, vEnd, vMins, vMaxs, vForward, passEntityNum, contentMask);
	if ( trace.fraction < 1.0f )
	{
		bLocational = qfalse;
	}
	if ( pfTorsoHeight )
	{
		*pfTorsoHeight = 0;
	}
	if ( pfTorsoPitch )
	{
		VectorSubtract(vStart, vTorsoPos, vFeetPos);
		*pfTorsoPitch = AngleNormalize180(vectopitch(vFeetPos));
	}
	if ( pfWaistPitch )
	{
		VectorSubtract(vTorsoPos, vWaistPos, vFeetPos);
		*pfWaistPitch = AngleNormalize180(vectopitch(vFeetPos));
	}
	if ( bLocational )
	{
		return qtrue;
	}
fail:
	if ( !bOnGround )
	{
		if ( pfTorsoHeight )
		{
			*pfTorsoHeight = 0;
		}
		if ( pfTorsoPitch )
		{
			*pfTorsoPitch = 0;
		}
		if ( pfWaistPitch )
		{
			*pfWaistPitch = 0;
		}
		return qtrue;
	}
	return qfalse;
}
/*
============
BG_CheckProne
============
*/
qboolean BG_CheckProne(int passEntityNum, const vec3_t vPos, float fSize, float fHeight, float fYaw,
                       float *pfTorsoHeight, float *pfTorsoPitch, float *pfWaistPitch, qboolean bAlreadyProne, qboolean bOnGround,
                       const vec3_t vGroundNormal, byte handler, int proneCheckType, float prone_feet_dist)
{
	return BG_CheckProneValid(passEntityNum, vPos, fSize, fHeight, fYaw, pfTorsoHeight, pfTorsoPitch, pfWaistPitch, bAlreadyProne, bOnGround, vGroundNormal, handler, proneCheckType, prone_feet_dist);
}

/*
================
BG_RegisterDvars
================
*/
void BG_RegisterDvars()
{
	unsigned short flags = DVAR_CODINFO;

	player_view_pitch_up = Dvar_RegisterFloat("player_view_pitch_up", 85, 0, 90, flags | DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
	player_view_pitch_down = Dvar_RegisterFloat("player_view_pitch_down", 85, 0, 90, flags | DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
	bg_ladder_yawcap = Dvar_RegisterFloat("bg_ladder_yawcap", 100, 0, 360, flags | DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
	bg_prone_yawcap = Dvar_RegisterFloat("bg_prone_yawcap", 85, 0, 360, flags | DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
	bg_foliagesnd_minspeed = Dvar_RegisterFloat("bg_foliagesnd_minspeed", 40, 0, FLT_MAX, flags | DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
	bg_foliagesnd_maxspeed = Dvar_RegisterFloat("bg_foliagesnd_maxspeed", 180, 0, FLT_MAX, flags | DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
	bg_foliagesnd_slowinterval = Dvar_RegisterInt("bg_foliagesnd_slowinterval", 1500, 0, INT_MAX, flags | DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
	bg_foliagesnd_fastinterval = Dvar_RegisterInt("bg_foliagesnd_fastinterval", 500, 0, INT_MAX, flags | DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
	bg_foliagesnd_resetinterval = Dvar_RegisterInt("bg_foliagesnd_resetinterval", 500, 0, INT_MAX, flags | DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
	bg_fallDamageMinHeight = Dvar_RegisterFloat("bg_fallDamageMinHeight", 256, 1, FLT_MAX, flags | DVAR_SYSTEMINFO | DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
	bg_fallDamageMaxHeight = Dvar_RegisterFloat("bg_fallDamageMaxHeight", 480, 1, FLT_MAX, flags | DVAR_SYSTEMINFO | DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
	inertiaMax = Dvar_RegisterFloat("inertiaMax", 50, 0, 1000, flags | DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
	inertiaDebug = Dvar_RegisterBool("inertiaDebug", false, flags | DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
	inertiaAngle = Dvar_RegisterFloat("inertiaAngle", 0, -1, 1, flags | DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
	friction = Dvar_RegisterFloat("friction", 5.5, 0, 100, flags | DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
	stopspeed = Dvar_RegisterFloat("stopspeed", 100, 0, 1000, flags | DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
	bg_swingSpeed = Dvar_RegisterFloat("bg_swingSpeed", 0.2, 0, 1, DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
	bg_bobAmplitudeStanding = Dvar_RegisterFloat("bg_bobAmplitudeStanding", 0.0070000002, 0, 1, flags | DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
	bg_bobAmplitudeDucked = Dvar_RegisterFloat("bg_bobAmplitudeDucked", 0.0074999998, 0, 1, flags | DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
	bg_bobAmplitudeProne = Dvar_RegisterFloat("bg_bobAmplitudeProne", 0.029999999, 0, 1, flags | DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
	bg_bobMax = Dvar_RegisterFloat("bg_bobMax", 8, 0, 36, flags | DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
	bg_aimSpreadMoveSpeedThreshold = Dvar_RegisterFloat("bg_aimSpreadMoveSpeedThreshold", 11, 0, 300, flags | DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
	player_breath_hold_time = Dvar_RegisterFloat("player_breath_hold_time", 4.5, 0, 30, flags | DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
	player_breath_gasp_time = Dvar_RegisterFloat("player_breath_gasp_time", 1, 0, 30, flags | DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
	player_breath_fire_delay = Dvar_RegisterFloat("player_breath_fire_delay", 0, 0, 30, flags | DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
	player_breath_gasp_scale = Dvar_RegisterFloat("player_breath_gasp_scale", 4.5, 0, 50, flags | DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
	player_breath_hold_lerp = Dvar_RegisterFloat("player_breath_hold_lerp", 4, 0, 50, flags | DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
	player_breath_gasp_lerp = Dvar_RegisterFloat("player_breath_gasp_lerp", 6, 0, 50, flags | DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
	player_breath_snd_lerp = Dvar_RegisterFloat("player_breath_snd_lerp", 2, 0, 100, flags | DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
	player_breath_snd_delay = Dvar_RegisterFloat("player_breath_snd_delay", 1, 0, 2, flags | DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
	player_toggleBinoculars = Dvar_RegisterBool("player_toggleBinoculars", true, flags | DVAR_CHANGEABLE_RESET);
	player_scopeExitOnDamage = Dvar_RegisterBool("player_scopeExitOnDamage", false, flags | DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
	player_adsExitDelay = Dvar_RegisterInt("player_adsExitDelay", 0, 0, 1000, flags | DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
	player_moveThreshhold = Dvar_RegisterFloat("player_moveThreshhold", 10, 0.0000000099999999, 20, flags | DVAR_ROM | DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
	player_footstepsThreshhold = Dvar_RegisterFloat("player_footstepsThreshhold", 0, 0, 50000, flags | DVAR_ROM | DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
	player_strafeSpeedScale = Dvar_RegisterFloat("player_strafeSpeedScale", 0.80000001, 0, 20, flags | DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
	player_backSpeedScale = Dvar_RegisterFloat("player_backSpeedScale", 0.69999999, 0, 20, flags | DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
	player_spectateSpeedScale = Dvar_RegisterFloat("player_spectateSpeedScale", 2, 0, 20, flags | DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
	player_turnAnims = Dvar_RegisterBool("player_turnAnims", false, flags | DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
	player_dmgtimer_timePerPoint = Dvar_RegisterFloat("player_dmgtimer_timePerPoint", 100, 0, FLT_MAX, DVAR_CHEAT | DVAR_CODINFO | DVAR_CHANGEABLE_RESET);
	player_dmgtimer_maxTime = Dvar_RegisterFloat("player_dmgtimer_maxTime", 750, 0, FLT_MAX, DVAR_CHEAT | DVAR_CODINFO | DVAR_CHANGEABLE_RESET);
	player_dmgtimer_minScale = Dvar_RegisterFloat("player_dmgtimer_minScale", 0, 0, 1, DVAR_CHEAT | DVAR_CODINFO | DVAR_CHANGEABLE_RESET);
	player_dmgtimer_stumbleTime = Dvar_RegisterInt("player_dmgtimer_stumbleTime", 500, 0, 2000, DVAR_CHEAT | DVAR_CODINFO | DVAR_CHANGEABLE_RESET);
	player_dmgtimer_flinchTime = Dvar_RegisterInt("player_dmgtimer_flinchTime", 500, 0, 2000, DVAR_CHEAT | DVAR_CODINFO | DVAR_CHANGEABLE_RESET);

	Jump_RegisterDvars();
	Mantle_RegisterDvars();
}

/*
================
BG_FindItemForWeapon
================
*/
gitem_t* BG_FindItemForWeapon( int weapon )
{
	assert(BG_ValidateWeaponNumber(weapon));
	return &bg_itemlist[weapon];
}

/*
===============
G_FindItem
===============
*/
gitem_t* G_FindItem( const char *pickupName )
{
	int iIndex;
	gitem_t *it;

	for ( iIndex = MAX_WEAPONS + 1; iIndex < bg_numItems; iIndex++ )
	{
		it = &bg_itemlist[iIndex];

		if ( !I_stricmp(it->pickup_name, pickupName) || !I_stricmp(it->classname, pickupName) )
		{
			return it;
		}
	}

	iIndex = G_GetWeaponIndexForName(pickupName);

	if ( iIndex )
	{
		return &bg_itemlist[iIndex];
	}

	return NULL;
}

/*
============
BG_PlayerTouchesItem

Items can be picked up without actually touching their physical bounds to make
grabbing them easier
============
*/
qboolean BG_PlayerTouchesItem( const playerState_t *ps, const entityState_t *item, int atTime )
{
	vec3_t origin;

	BG_EvaluateTrajectory( &item->pos, atTime, origin );

	// we are ignoring ducked differences here
	if (	   ps->origin[0] - origin[0] > 36
	           || ps->origin[0] - origin[0] < -36
	           || ps->origin[1] - origin[1] > 36
	           || ps->origin[1] - origin[1] < -36
	           || ps->origin[2] - origin[2] > 18
	           || ps->origin[2] - origin[2] < -88 )
	{
		return qfalse;
	}

	return qtrue;
}

/*
================
BG_CanItemBeGrabbed

Returns false if the item should not be picked up.
This needs to be the same for client side prediction and server use.
================
*/
qboolean BG_CanItemBeGrabbed( const entityState_t *ent, const playerState_t *ps, qboolean bTouched )
{
	gitem_t *item;

	assert(ent);
	assert(ps);

	if ( ent->index < 1 || ent->index >= bg_numItems )
	{
		Com_Error(ERR_DROP, va("\x15" "BG_CanItemBeGrabbed: index out of range (index is %i, eType is %i)", ent->index, ent->eType));
	}

	item = &bg_itemlist[ent->index];

	if ( ent->clientNum == ps->clientNum )
	{
		return qfalse;
	}

	switch ( item->giType )
	{
	case IT_WEAPON:
		if ( !BG_DoesWeaponNeedSlot(item->giTag) || Com_BitCheck(ps->weapons, item->giTag) )
		{
			if ( BG_GetMaxPickupableAmmo(ps, item->giTag) <= 0 )
				return qfalse;
		}
		else if ( bTouched || BG_GetMaxPickupableAmmo(ps, item->giTag) <= 0 )
			return qfalse;

		return qtrue;

	case IT_HEALTH:
		if ( ps->stats[STAT_HEALTH] >= ps->stats[STAT_MAX_HEALTH] )
			return qfalse;

		return qtrue;

	case IT_AMMO:
		if ( Com_BitCheck(ps->weapons, item->giTag) )
		{
			if ( BG_GetMaxPickupableAmmo(ps, item->giTag) <= 0 )
				return qfalse;
		}
		else if ( !BG_WeaponIsClipOnly(item->giTag) || BG_GetMaxPickupableAmmo(ps, item->giTag) <= 0 )
			return qfalse;

		return qtrue;

	case IT_BAD:
		Com_Error( ERR_DROP, "\x15" "BG_CanItemBeGrabbed: IT_BAD" );
	}

	return qfalse;
}

/*
================
BG_EvaluateTrajectory
================
*/
void BG_EvaluateTrajectory( const trajectory_t *tr, int atTime, vec3_t result )
{
	float deltaTime;
	float phase;
	vec3_t v;

	switch ( tr->trType )
	{
	case TR_STATIONARY:
	case TR_INTERPOLATE:
	case TR_GRAVITY_PAUSED: //----(SA)
		VectorCopy( tr->trBase, result );
		break;
	case TR_LINEAR:
		deltaTime = ( atTime - tr->trTime ) * 0.001f;    // milliseconds to seconds
		VectorMA( tr->trBase, deltaTime, tr->trDelta, result );
		break;
	case TR_SINE:
		deltaTime = ( atTime - tr->trTime ) / (float) tr->trDuration;
		phase = sin( deltaTime * M_PI * 2 );
		VectorMA( tr->trBase, phase, tr->trDelta, result );
		break;
	case TR_LINEAR_STOP:
		if ( atTime > tr->trTime + tr->trDuration )
		{
			atTime = tr->trTime + tr->trDuration;
		}
		deltaTime = ( atTime - tr->trTime ) * 0.001f;    // milliseconds to seconds
		if ( deltaTime < 0 )
		{
			deltaTime = 0;
		}
		VectorMA( tr->trBase, deltaTime, tr->trDelta, result );
		break;
	case TR_GRAVITY:
		deltaTime = ( atTime - tr->trTime ) * 0.001f;    // milliseconds to seconds
		VectorMA( tr->trBase, deltaTime, tr->trDelta, result );
		result[2] -= 0.5f * DEFAULT_GRAVITY * deltaTime * deltaTime;     // FIXME: local gravity...
		break;
	// RF, acceleration
	case TR_ACCELERATE:     // trDelta is the ultimate speed
		if ( atTime > tr->trTime + tr->trDuration )
		{
			atTime = tr->trTime + tr->trDuration;
		}
		deltaTime = ( atTime - tr->trTime ) * 0.001f;    // milliseconds to seconds
		// phase is the acceleration constant
		phase = VectorLength( tr->trDelta ) / ( tr->trDuration * 0.001f );
		// trDelta at least gives us the acceleration direction
		Vec3NormalizeTo( tr->trDelta, result );
		// get distance travelled at current time
		VectorMA( tr->trBase, phase * 0.5f * deltaTime * deltaTime, result, result );
		break;
	case TR_DECCELERATE:    // trDelta is the starting speed
		if ( atTime > tr->trTime + tr->trDuration )
		{
			atTime = tr->trTime + tr->trDuration;
		}
		deltaTime = ( atTime - tr->trTime ) * 0.001f;    // milliseconds to seconds
		// phase is the breaking constant
		phase = VectorLength( tr->trDelta ) / ( tr->trDuration * 0.001f );
		// trDelta at least gives us the acceleration direction
		Vec3NormalizeTo( tr->trDelta, result );
		// get distance travelled at current time (without breaking)
		VectorMA( tr->trBase, deltaTime, tr->trDelta, v );
		// subtract breaking force
		VectorMA( v, -phase * 0.5f * deltaTime * deltaTime, result, result );
		break;
	default:
		Com_Error( ERR_DROP, "\x15" "BG_EvaluateTrajectory: unknown trType: %i", tr->trType );
	}
}

/*
================
BG_EvaluateTrajectoryDelta
For determining velocity at a given time
================
*/
void BG_EvaluateTrajectoryDelta( const trajectory_t *tr, int atTime, vec3_t result )
{
	float deltaTime;
	float phase;

	switch ( tr->trType )
	{
	case TR_STATIONARY:
	case TR_INTERPOLATE:
		VectorClear( result );
		break;
	case TR_LINEAR:
		VectorCopy( tr->trDelta, result );
		break;
	case TR_SINE:
		deltaTime = ( atTime - tr->trTime ) / (float) tr->trDuration;
		phase = cos( deltaTime * M_PI * 2 );    // derivative of sin = cos
		phase *= 0.5f;
		VectorScale( tr->trDelta, phase, result );
		break;
	case TR_LINEAR_STOP:
		if ( atTime > tr->trTime + tr->trDuration )
		{
			VectorClear( result );
			return;
		}
		VectorCopy( tr->trDelta, result );
		break;
	case TR_GRAVITY:
		deltaTime = ( atTime - tr->trTime ) * 0.001f;    // milliseconds to seconds
		VectorCopy( tr->trDelta, result );
		result[2] -= DEFAULT_GRAVITY * deltaTime;       // FIXME: local gravity...
		break;
	// RF, acceleration
	case TR_ACCELERATE: // trDelta is eventual speed
		if ( atTime > tr->trTime + tr->trDuration )
		{
			VectorClear( result );
			return;
		}
		deltaTime = ( atTime - tr->trTime ) * 0.001f;    // milliseconds to seconds
		phase = deltaTime / (float)tr->trDuration;
		VectorScale( tr->trDelta, deltaTime * deltaTime, result );
		break;
	case TR_DECCELERATE:    // trDelta is breaking force
		if ( atTime > tr->trTime + tr->trDuration )
		{
			VectorClear( result );
			return;
		}
		deltaTime = ( atTime - tr->trTime ) * 0.001f;    // milliseconds to seconds
		VectorScale( tr->trDelta, deltaTime, result );
		break;
	default:
		Com_Error( ERR_DROP, "\x15" "BG_EvaluateTrajectoryDelta: unknown trType: %i", tr->trType );
	}
}

/*
============
BG_GetMarkDir

  used to find a good directional vector for a mark projection, which will be more likely
  to wrap around adjacent surfaces

  dir is the direction of the projectile or trace that has resulted in a surface being hit
============
*/
void BG_GetMarkDir( const vec3_t dir, const vec3_t normal, vec3_t out )
{
	vec3_t ndir, lnormal;
	float minDot = 0.3;

	/*
	if ( dir[0] < 0.001 && dir[1] < 0.001 )
	{
		VectorCopy( dir, out );
		return;
	}
	*/

	if ( VectorLength( normal ) < 1.0 )      // this is needed to get rid of (0,0,0) normals (happens with entities?)
	{
		VectorSet( lnormal, 0.f, 0.f, 1.f );
	}
	else
	{
		VectorCopy( normal, lnormal );
		//VectorNormalizeFast( lnormal );
		//VectorNormalize2( normal, lnormal );
	}

	VectorNegate( dir, ndir );
	Vec3Normalize( ndir );
	if ( normal[2] > .8f )
	{
		minDot = .7f;
	}

	// make sure it makrs the impact surface
	while ( minDot > DotProduct( ndir, lnormal ) )
	{
		VectorMA( ndir, .5, lnormal, ndir );
		Vec3Normalize( ndir );
	}

	VectorCopy( ndir, out );
}

/*
================
BG_AddPredictableEventToPlayerstate
================
*/
void BG_AddPredictableEventToPlayerstate( int newEvent, int eventParm, playerState_t *ps )
{
	assert(ps);

	if ( !newEvent )
	{
		return;
	}

	ps->events[ps->eventSequence & ( MAX_EVENTS - 1 )] = (unsigned char)newEvent;
	ps->eventParms[ps->eventSequence & ( MAX_EVENTS - 1 )] = (unsigned char)eventParm;
	ps->eventSequence++;
}

/*
========================
BG_PlayerStateToEntityState

This is done after each set of usercmd_t on the server,
and after local prediction on the client
========================
*/
void BG_PlayerStateToEntityState( playerState_t *ps, entityState_t *s, qboolean snap, byte handler )
{
	int i;
	void (*playerEvent)(int, int);
	float fLerpFrac;
	int seq;
	byte event;

	if ( ps->pm_flags & ( PMF_FOLLOW | PMF_PLAYER ) ) // JPW NERVE limbo
	{
		s->eType = ET_PLAYER;
	}
	else
	{
		s->eType = ET_INVISIBLE;
	}

	s->pos.trType = TR_INTERPOLATE;
	VectorCopy(ps->origin, s->pos.trBase);
	if ( snap )
	{
		SnapVector( s->pos.trBase );
	}

	s->apos.trType = TR_INTERPOLATE;
	VectorCopy(ps->viewangles, s->apos.trBase);
	if ( snap )
	{
		SnapVector( s->apos.trBase );
	}

	s->angles2[YAW] = (float)ps->movementDir;

	s->legsAnim = ps->legsAnim;
	s->torsoAnim = ps->torsoAnim;
	s->clientNum = ps->clientNum; // ET_PLAYER looks here instead of at number

	s->eFlags = ps->eFlags;

	// Ridah, let clients know if this person is using a mounted weapon
	// so they don't show any client muzzle flashes
	if ( ps->eFlags & EF_TURRET_ACTIVE )
	{
		s->otherEntityNum = ps->viewlocked_entNum;
	}

	if ( ps->pm_type >= PM_DEAD )
	{
		s->eFlags |= EF_DEAD;
	}
	else
	{
		s->eFlags &= ~EF_DEAD;
	}

	if ( ps->pm_flags & PMF_ADS )
	{
		s->eFlags |= EF_AIMDOWNSIGHT;
	}
	else
	{
		s->eFlags &= ~EF_AIMDOWNSIGHT;
	}

	s->leanf = ps->leanf;

	if ( PM_GetEffectiveStance(ps) == PM_EFF_STANCE_PRONE )
	{
		fLerpFrac = 0.0f;

		if ( ps->viewHeightLerpTime )
		{
			seq = PM_GetViewHeightLerpTime(ps, ps->viewHeightLerpTarget, ps->viewHeightLerpDown);
			fLerpFrac = (ps->commandTime - ps->viewHeightLerpTime) / (float)seq;

			if ( fLerpFrac < 0 )
			{
				fLerpFrac = 0;
			}
			else
			{
				if ( fLerpFrac > 1.0f )
				{
					fLerpFrac = 1.0f;
				}
			}

			if ( !ps->viewHeightLerpDown )
			{
				fLerpFrac = 1.0f - fLerpFrac;
			}
		}
		else
		{
			fLerpFrac = 1.0f;
		}

		s->fTorsoHeight = ps->fTorsoHeight * fLerpFrac;
		s->fTorsoPitch = AngleNormalize180(ps->fTorsoPitch) * fLerpFrac;
		s->fWaistPitch = AngleNormalize180(ps->fWaistPitch) * fLerpFrac;
	}
	else
	{
		s->fTorsoHeight = 0;
		s->fTorsoPitch = 0;
		s->fWaistPitch = 0;
	}

// from MP
	if ( ps->entityEventSequence - ps->eventSequence < 0 )
	{
		if ( ps->eventSequence - ps->entityEventSequence > MAX_EVENTS )
		{
			ps->entityEventSequence = ps->eventSequence - MAX_EVENTS;
		}
		seq = ps->entityEventSequence & ( MAX_EVENTS - 1 );
		s->eventParm = (byte)ps->eventParms[ seq ];
		ps->entityEventSequence++;
	}
	else
	{
		s->eventParm = 0;
	}
// end
	// Ridah, now using a circular list of events for all entities
	// add any new events that have been added to the playerState_t
	// (possibly overwriting entityState_t events)
	for ( i = ps->oldEventSequence; i != ps->eventSequence; i++ )
	{
		event = ps->events[i & ( MAX_EVENTS - 1 )];
		playerEvent = pmoveHandlers[handler].playerEvent;

		if ( playerEvent )
		{
			playerEvent(s->number, event);
		}

		for ( seq = 0; singleClientEvents[seq] > 0; seq++ )
		{
			if ( singleClientEvents[seq] == event )
			{
				break;
			}
		}

		if ( singleClientEvents[seq] >= 0 )
		{
			continue;
		}

		s->events[s->eventSequence & ( MAX_EVENTS - 1 )] = event;
		s->eventParms[s->eventSequence & ( MAX_EVENTS - 1 )] = (byte)ps->eventParms[i & ( MAX_EVENTS - 1 )];
		s->eventSequence++;
	}
	ps->oldEventSequence = ps->eventSequence;

	s->weapon = (byte)ps->weapon; // Ridah
//	s->loopSound = ps->loopSound;
	s->groundEntityNum = (unsigned short)ps->groundEntityNum; // xkan, 1/10/2003
}

/*
==================
BG_LerpHudColors
==================
*/
void BG_LerpHudColors( const hudelem_t *elem, int time, hudelem_color_t *toColor )
{
	float lerp;
	int timeSinceFadeStarted;

	timeSinceFadeStarted = time - elem->fadeStartTime;

	if ( elem->fadeTime > 0 && timeSinceFadeStarted < elem->fadeTime )
	{
		if ( timeSinceFadeStarted < 0 )
		{
			timeSinceFadeStarted = 0;
		}

		lerp = (float)timeSinceFadeStarted / (float)elem->fadeTime;
		assert((lerp >= 0.0f && lerp <= 1.0f));

		toColor->split.r = elem->fromColor.split.r + (elem->color.split.r - elem->fromColor.split.r) * lerp;
		toColor->split.g = elem->fromColor.split.g + (elem->color.split.g - elem->fromColor.split.g) * lerp;
		toColor->split.b = elem->fromColor.split.b + (elem->color.split.b - elem->fromColor.split.b) * lerp;
		toColor->split.a = elem->fromColor.split.a + (elem->color.split.a - elem->fromColor.split.a) * lerp;
	}
	else
	{
		*toColor = elem->color;
	}
}

