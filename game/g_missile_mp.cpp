#include "../qcommon/qcommon.h"
#include "g_shared.h"

extern void Server_SwitchToValidFxScheduler( void );
struct FxEffectDef;
extern FxEffectDef *FX_RegisterEffect( const char *name );
extern float FX_GetEffectLength( const FxEffectDef *fx );

void G_RunMissile_CreateWaterSplash( gentity_t *ent, trace_t *trace );


qboolean G_BounceMissile( gentity_t *ent, trace_t *trace );
void G_ExplodeMissile( gentity_t *ent );
void G_MissileTrace( trace_t *results, const vec3_t start, const vec3_t end, int passEntityNum, int contentmask );
void G_MissileTrace_IgnoreEntity( trace_t *results, int hitId, gentity_t *ent, const vec3_t origin );
void G_RunMissile_Destabilize( gentity_t *ent );
void G_RunMissile_CreateWaterSplash( gentity_t *ent, trace_t *trace );
void G_RunMissile( gentity_t *ent );
gentity_t *fire_grenade( gentity_t *self, vec3_t start, vec3_t dir, int grenadeWPID, int iTime );
gentity_t *fire_rocket( gentity_t *self, vec3_t start, vec3_t dir );

/*
================
G_RunMissile_CreateWaterSplash
================
*/

void G_MissileLandAngles( gentity_t *ent, trace_t *trace, vec3_t vAngles, qboolean bForceAlign )
{
	int hitTime;
	float fSurfacePitch;
	float fAngleDelta;
	float fAbsAngDelta;
	vec3_t vLastAngles;
	float fMaxAlignAngle = 45.0f;
	float fMaxBounceAngle = 80.0f;

	hitTime = level.previousTime + (int)((level.time - level.previousTime) * trace->fraction);
	BG_EvaluateTrajectory(&ent->s.apos, hitTime, vAngles);
	VectorCopy(vAngles, vLastAngles);

	if ( trace->normal[2] > 0.1f )
	{
		fSurfacePitch = PitchForYawOnNormal(vAngles[1], trace->normal);
		fAngleDelta = AngleSubtract(fSurfacePitch, vAngles[0]);
		fAbsAngDelta = I_fabs(fAngleDelta);

		if ( !bForceAlign )
		{
			VectorCopy(vAngles, ent->s.apos.trBase);
			ent->s.apos.trTime = hitTime;

			if ( fAbsAngDelta < 80.0f )
			{
				ent->s.apos.trDelta[0] = ent->s.apos.trDelta[0] * (randomf() * 0.3f + 0.85f) * -1.0f;
			}
			else
			{
				ent->s.apos.trDelta[0] = ent->s.apos.trDelta[0] * (randomf() * 0.3f + 0.85f);
			}
		}

		vAngles[0] = AngleNormalize180(vAngles[0]);

		if ( bForceAlign || fAbsAngDelta < 45.0f )
		{
			if ( I_fabs(vAngles[0]) > 90.0f )
			{
				vAngles[0] = AngleNormalize360(fSurfacePitch + 180.0f);
			}
			else
			{
				vAngles[0] = AngleNormalize360(fSurfacePitch);
			}
		}
		else
		{
			if ( fAbsAngDelta < 80.0f )
			{
				vAngles[0] = AngleNormalize360(vAngles[0] + fAngleDelta * 0.25f);
			}
			else
			{
				vAngles[0] = AngleNormalize360(vAngles[0]);
			}
		}
	}
	else if ( !bForceAlign )
	{
		ent->s.apos.trDelta[0] = AngleNormalize360(ent->s.apos.trDelta[0] + (float)((rand() & 0x7F) - 63));
	}
}

/*
================
G_BounceMissile
================
*/
qboolean G_BounceMissile( gentity_t *ent, trace_t *trace )
{
	WeaponDef *weapDef;
	vec3_t velocity;
	vec3_t vDelta;
	vec3_t vAngles;
	float dot;
	float len;
	int hitTime;
	int contents;
	int surfType;

	weapDef = BG_GetWeaponDef(ent->s.weapon);
	contents = SV_PointContents(ent->r.currentOrigin, -1, CONTENTS_WATER);
	surfType = (unsigned char)((trace->surfaceFlags & 0x1F00000) >> 20);

	hitTime = level.previousTime + (int)((level.time - level.previousTime) * trace->fraction);
	BG_EvaluateTrajectoryDelta(&ent->s.pos, hitTime, velocity);
	dot = DotProduct(velocity, trace->normal);
	VectorMA(velocity, dot * -2.0f, trace->normal, ent->s.pos.trDelta);

	if ( trace->normal[2] > 0.7 )
	{
		ent->s.groundEntityNum = trace->entityNum;
	}

	if ( ent->s.eFlags & EF_BOUNCE )
	{
		len = VectorLength(velocity);

		if ( len > 0 && dot <= 0 )
		{
			dot = dot / -len;
			len = (weapDef->perpendicularBounce[surfType] - weapDef->parallelBounce[surfType]) * dot + weapDef->parallelBounce[surfType];
			VectorScale(ent->s.pos.trDelta, len, ent->s.pos.trDelta);
		}

		if ( trace->normal[2] > 0.7 && VectorLength(ent->s.pos.trDelta) < 20.0f )
		{
			G_SetOrigin(ent, ent->r.currentOrigin);
			G_MissileLandAngles(ent, trace, vAngles, qtrue);
			G_SetAngle(ent, vAngles);
			return qfalse;
		}
	}

	VectorScale(trace->normal, 0.1f, vDelta);

	if ( vDelta[2] > 0 )
	{
		vDelta[2] = 0;
	}

	VectorAdd(ent->r.currentOrigin, vDelta, ent->r.currentOrigin);
	VectorCopy(ent->r.currentOrigin, ent->s.pos.trBase);
	ent->s.pos.trTime = level.time;

	G_MissileLandAngles(ent, trace, vAngles, qfalse);
	VectorCopy(vAngles, ent->s.apos.trBase);
	ent->s.apos.trTime = level.time;

	if ( contents )
	{
		return qfalse;
	}

	VectorSubtract(ent->s.pos.trDelta, velocity, velocity);
	dot = VectorLength(velocity);

	if ( dot > 100.0f )
	{
		return qtrue;
	}

	return qfalse;
}

void G_MissileImpact( gentity_t *ent, trace_t *trace, vec3_t dir, vec3_t endpos )
{
	gentity_t *other;
	qboolean hitClient;
	vec3_t velocity;
	int eType;
	qboolean noMarks;
	WeaponDef *weapDef;
	int mod;
	int splashMod;

	hitClient = qfalse;

	other = &g_entities[trace->entityNum];
	ent->s.surfType = (unsigned char)((trace->surfaceFlags & 0x1f00000) >> 20);

	// check for bounce
	if ( !other->takedamage && (ent->s.eFlags & EF_BOUNCE) )
	{
		if ( G_BounceMissile(ent, trace) && !trace->startsolid )
		{
			G_AddEvent(ent, EV_GRENADE_BOUNCE, (unsigned char)((trace->surfaceFlags & 0x1f00000) >> 20));
		}
		return;
	}

	weapDef = BG_GetWeaponDef(ent->s.weapon);
	mod = entityHandlers[ent->handler].methodOfDeath;

	// impact damage
	if ( other->takedamage )
	{
		if ( ent->dmg )
		{
			if ( LogAccuracyHit(other, &g_entities[ent->r.ownerNum]) )
			{
				hitClient = qtrue;
			}

			BG_EvaluateTrajectoryDelta(&ent->s.pos, level.time, velocity);

			if ( VectorLength(velocity) == 0 )
			{
				velocity[2] = 1;    // stepped on a grenade
			}

			G_Damage(other, ent, ent->r.ownerNum != ENTITYNUM_NONE ? &g_entities[ent->r.ownerNum] : NULL,
			         velocity, ent->r.currentOrigin, ent->dmg, 0, mod, HITLOC_NONE, 0);
		}
		else // if no damage value, then this is a splash damage grenade only
		{
			if ( other->client && !trace->surfaceFlags )
			{
				trace->surfaceFlags = SURF_FLESH;
			}

			if ( G_BounceMissile(ent, trace) && !trace->startsolid )
			{
				G_AddEvent(ent, EV_GRENADE_BOUNCE, (unsigned char)((trace->surfaceFlags & 0x1f00000) >> 20));
			}
			return;
		}
	}

	// damage triggers
	if ( ent->dmg )
	{
		G_CheckHitTriggerDamage(ent->r.ownerNum != ENTITYNUM_NONE ? &g_entities[ent->r.ownerNum] : &g_entities[ENTITYNUM_WORLD],
		                        ent->r.currentOrigin, endpos, ent->dmg, mod);
	}

	noMarks = hitClient || trace->partName;
	G_AddEvent(ent, noMarks ? EV_ROCKET_EXPLODE_NOMARKS : EV_ROCKET_EXPLODE, DirToByte(trace->normal));

	ent->s.surfType = (unsigned char)((trace->surfaceFlags & 0x1f00000) >> 20);
	ent->freeAfterEvent = qtrue;
	eType = ent->s.eType;
	ent->s.eType = ET_GENERAL;
	ent->s.eFlags ^= EF_TELEPORT_BIT;
	ent->s.eFlags |= EF_NODRAW;
	ent->flags |= FL_NODRAW;

	RoundFloatArray(endpos, ent->s.pos.trBase);
	G_SetOrigin(ent, endpos);

	// splash damage (doesn't apply to person directly hit)
	if ( weapDef->explosionInnerDamage )
	{
		splashMod = entityHandlers[ent->handler].splashMethodOfDeath;
		G_RadiusDamage(endpos, ent, ent->parent, weapDef->explosionInnerDamage, weapDef->explosionOuterDamage, weapDef->explosionRadius, other, splashMod);
	}

	SV_LinkEntity(ent);
}

/*
================
G_ExplodeMissile

Explode a missile without an impact
================
*/
void G_ExplodeMissile( gentity_t *ent )
{
	vec3_t up;
	vec3_t origin;
	vec3_t end;
	int oldEType;
	int contents;
	const FxEffectDef *fx;
	trace_t trace;
	WeaponDef *weapDef;
	int splashMod;

	assert(ent);
	assert(ent->s.weapon);

	weapDef = BG_GetWeaponDef(ent->s.weapon);
	assert(weapDef);

	if ( weapDef->offhandClass == OFFHAND_CLASS_SMOKE_GRENADE && ent->s.groundEntityNum == ENTITYNUM_NONE )
	{
		ent->nextthink = 50;
		return;
	}

	BG_EvaluateTrajectory(&ent->s.pos, level.time, origin);
	SnapVector(origin);
	G_SetOrigin(ent, origin);

	up[0] = up[1] = 0.0f;
	up[2] = 1.0f;

	oldEType = ent->s.eType;
	ent->s.eType = ET_GENERAL;
	ent->s.eFlags |= EF_NODRAW;
	ent->flags |= FL_NODRAW;
	ent->r.svFlags |= SVF_BROADCAST;

	VectorCopy(ent->r.currentOrigin, end);

	//bani - #560
	end[2] -= 16;

	G_TraceCapsule(&trace, ent->r.currentOrigin, vec3_origin, vec3_origin, end, ent->s.number, CONTENTS_SOLID | CONTENTS_GLASS | CONTENTS_SKY);

	if ( weapDef->projExplosionType == WEAPPROJEXP_NONE )
	{
		G_AddEvent(ent, EV_CUSTOM_EXPLODE, DirToByte(trace.normal));
	}
	else
	{
		G_AddEvent(ent, EV_GRENADE_EXPLODE, DirToByte(trace.normal));
	}

	contents = SV_PointContents(ent->r.currentOrigin, -1, CONTENTS_WATER);

	if ( contents )
		ent->s.surfType = SURF_TYPE_WATER;
	else
		ent->s.surfType = (unsigned char)( ( trace.surfaceFlags & 0x1f00000 ) >> 20 );

	if ( weapDef->projExplosionEffect && weapDef->projExplosionEffect[0] )
	{
		ent->s.eFlags |= EF_UNKNOWN;
		Server_SwitchToValidFxScheduler();
		fx = FX_RegisterEffect(weapDef->projExplosionEffect);
		ent->s.time = level.time;
		ent->s.time2 = level.time + (int)(FX_GetEffectLength(fx) + 1.0f);
	}
	else
	{
		ent->freeAfterEvent = qtrue;
	}

	if ( weapDef->explosionInnerDamage )
	{
		splashMod = entityHandlers[ent->handler].splashMethodOfDeath;
		G_RadiusDamage(ent->r.currentOrigin, ent, ent->parent, weapDef->explosionInnerDamage, weapDef->explosionOuterDamage, weapDef->explosionRadius, ent, splashMod);
	}

	SV_LinkEntity(ent);
}

/*
================
G_MissileTrace
================
*/
void G_MissileTrace( trace_t *results, const vec3_t start, const vec3_t end, int passEntityNum, int contentmask )
{
	vec3_t dir;

	G_LocationalTrace(results, start, end, passEntityNum, contentmask, bulletPriorityMap);

	if ( results->startsolid )
	{
		results->fraction = 0;

		VectorSubtract(start, end, dir);
		Vec3NormalizeTo(dir, results->normal);
	}
}

/*
================
G_MissileTrace_IgnoreEntity
================
*/
void G_MissileTrace_IgnoreEntity( trace_t *results, int hitId, gentity_t *ent, const vec3_t origin )
{
	gentity_t *hitEnt;
	int prevContents;

	hitEnt = &g_entities[hitId];
	prevContents = hitEnt->r.contents;
	hitEnt->r.contents = 0;

	G_MissileTrace(results, ent->r.currentOrigin, origin, ent->r.ownerNum, ent->clipmask);
	hitEnt->r.contents = prevContents;
}

float G_RunMissile_GetPerturbation( float destabilizationCurvatureMax )
{
	float angle;

	angle = destabilizationCurvatureMax * 0.017453292f;
	return tan(angle);
}

/*
================
G_RunMissile_Destabilize
================
*/
void G_RunMissile_Destabilize( gentity_t *ent )
{
	vec3_t newAPos;
	vec3_t newAngleAccel;
	int i;
	float perturbationMax;
	WeaponDef *weapDef;

	if ( ent->s.pos.trTime + (int)ent->missile.time < level.time )
	{
		weapDef = BG_GetWeaponDef(ent->s.weapon);

		VectorCopy(ent->s.pos.trDelta, newAPos);
		Vec3Normalize(newAPos);

		perturbationMax = G_RunMissile_GetPerturbation(weapDef->destabilizationAngleMax);

		for ( i = 0; i < 3; i++ )
		{
			newAngleAccel[i] = flrand(-1.0, 1.0);
		}

		VectorScale(newAngleAccel, perturbationMax, newAngleAccel);
		VectorAdd(newAPos, newAngleAccel, newAPos);

		Vec3Normalize(newAPos);

		VectorScale(newAPos, weapDef->projectileSpeed, ent->s.pos.trDelta);
		VectorCopy(ent->r.currentOrigin, ent->s.pos.trBase);

		vectoangles(newAPos, ent->r.currentAngles);
		G_SetAngle(ent, ent->r.currentAngles);

		ent->s.pos.trTime = level.time;

		if ( !(ent->flags & FL_MISSILE_DESTABILIZED) )
		{
			ent->missile.time = weapDef->destabilizationBaseTime * 1000.0f;
		}
		else
		{
			ent->missile.time *= weapDef->destabilizationTimeReductionRatio;
		}

		ent->flags |= FL_MISSILE_DESTABILIZED;
	}
}

/*
================
G_RunMissile_CreateWaterSplash
================
*/
void G_RunMissile_CreateWaterSplash( gentity_t *ent, trace_t *trace )
{
	vec3_t reflect;
	gentity_t *tent;

	assert(ent);
	assert(trace);

	Vec3NormalizeTo(ent->s.pos.trDelta, reflect);

	if ( reflect[2] < 0 )
	{
		reflect[2] *= -1.0f;
	}

	tent = G_TempEntity(ent->r.currentOrigin, EV_BULLET_HIT_LARGE);

	tent->s.eventParm = DirToByte(trace->normal);
	tent->s.eventParm2 = DirToByte(reflect);
	tent->s.surfType = (unsigned char)( ( trace->surfaceFlags & 0x1f00000 ) >> 20 );
	tent->s.otherEntityNum = ent->s.number;
}

/*
================
G_RunMissile
================
*/
void G_RunMissile( gentity_t *ent )
{
	vec3_t origin;
	vec3_t dir;
	trace_t tr;
	trace_t trDown;
	vec3_t vOldOrigin;
	int mod;
	vec3_t endpos;
	WeaponDef *weapDef;

	assert(ent);

	if ( ent->s.pos.trType == TR_STATIONARY && ent->s.groundEntityNum != ENTITYNUM_WORLD )
	{
		VectorCopy(ent->r.currentOrigin, origin);
		origin[2] -= 1.5f;
		G_MissileTrace(&tr, ent->r.currentOrigin, origin, ent->r.ownerNum, ent->clipmask);

		if ( tr.fraction == 1.0f )
		{
			ent->s.pos.trType = TR_GRAVITY;
			ent->s.pos.trTime = level.time;
			ent->s.pos.trDuration = 0;

			VectorCopy(ent->r.currentOrigin, ent->s.pos.trBase);
			VectorClear(ent->s.pos.trDelta);
		}
	}

	VectorCopy(ent->r.currentOrigin, vOldOrigin);

	// get current position
	BG_EvaluateTrajectory(&ent->s.pos, level.time, origin);
	VectorSubtract(origin, ent->r.currentOrigin, dir);

	if ( Vec3Normalize(dir) < 0.001f )
	{
		G_RunThink(ent);
		return;
	}

	// trace a line from the previous position to the current position,
	// ignoring interactions with the missile owner
	if ( I_fabs(ent->s.pos.trDelta[2]) > 30.0f && !SV_PointContents(ent->r.currentOrigin, -1, CONTENTS_WATER) )
	{
		G_MissileTrace(&tr, ent->r.currentOrigin, origin, ent->r.ownerNum, ent->clipmask | CONTENTS_WATER);
	}
	else
	{
		G_MissileTrace(&tr, ent->r.currentOrigin, origin, ent->r.ownerNum, ent->clipmask);
	}

	if ( ( tr.surfaceFlags & 0x1f00000 ) == ( SURF_TYPE_WATER << 20 ) )
	{
		G_RunMissile_CreateWaterSplash(ent, &tr);
		G_MissileTrace(&tr, ent->r.currentOrigin, origin, ent->r.ownerNum, ent->clipmask);
	}

	mod = entityHandlers[ent->handler].methodOfDeath;

	if ( mod == MOD_GRENADE && g_entities[tr.entityNum].flags & FL_GRENADE_BOUNCE )
	{
		G_MissileTrace_IgnoreEntity(&tr, tr.entityNum, ent, origin);
	}

	Vec3Lerp(ent->r.currentOrigin, origin, tr.fraction, endpos);
	VectorCopy(endpos, ent->r.currentOrigin);

	if ( ent->s.eFlags & EF_BOUNCE )
	{
		if ( tr.fraction == 1.0f || tr.fraction < 1.0f && tr.normal[2] > (float)MIN_WALK_NORMAL )
		{
			VectorCopy(ent->r.currentOrigin, origin);
			origin[2] -= 1.5f;

			G_MissileTrace(&trDown, ent->r.currentOrigin, origin, ent->r.ownerNum, ent->clipmask);

			if ( trDown.fraction != 1.0f && trDown.entityNum == ENTITYNUM_WORLD )
			{
				tr = trDown;
				Vec3Lerp(ent->r.currentOrigin, origin, tr.fraction, endpos);

				ent->s.pos.trBase[2] = ent->s.pos.trBase[2] + ( endpos[2] + 1.5f - ent->r.currentOrigin[2] );

				VectorCopy(endpos, ent->r.currentOrigin);
				ent->r.currentOrigin[2] = ent->r.currentOrigin[2] + 1.5f;
			}
		}
	}

	SV_LinkEntity(ent);

	weapDef = BG_GetWeaponDef(ent->s.weapon);
	assert(weapDef);

	if ( mod == MOD_GRENADE )
	{
		G_GrenadeTouchTriggerDamage(ent, vOldOrigin, ent->r.currentOrigin, weapDef->explosionInnerDamage, mod);
	}

	if ( tr.fraction != 1.0f )
	{
		if ( tr.surfaceFlags & SURF_NOIMPACT )
		{
			G_FreeEntity(ent);
			return;
		}

		G_MissileImpact(ent, &tr, dir, endpos);

		if ( ent->s.eType != ET_MISSILE )
		{
			return;
		}
	}
	else
	{
		if ( VectorLength(ent->s.pos.trDelta) != 0 )
		{
			ent->s.groundEntityNum = ENTITYNUM_NONE;

			if ( weapDef->weaponType == WEAPTYPE_PROJECTILE && !(ent->flags & FL_STABLE_MISSILES) )
			{
				G_RunMissile_Destabilize(ent);
			}
		}
	}

	G_RunThink(ent);
}

/*
=================
fire_grenade

	NOTE!!!! NOTE!!!!!

	This accepts a /non-normalized/ direction vector to allow specification
	of how hard it's thrown.  Please scale the vector before calling.

=================
*/
gentity_t *fire_grenade( gentity_t *self, vec3_t start, vec3_t dir, int grenadeWPID, int iTime )
{
	gentity_t *bolt;
	WeaponDef *weapDef;

	bolt = G_Spawn();

	// no self->client for shooter_grenade's
	if ( self->client && self->client->ps.grenadeTimeLeft )
	{
		bolt->nextthink = level.time + self->client->ps.grenadeTimeLeft;
		self->client->ps.grenadeTimeLeft = 0;
	}
	else
	{
		bolt->nextthink = level.time + iTime;
	}

	if ( self->client )
	{
		self->client->ps.grenadeTimeLeft = 0;
	}

	bolt->handler = ENT_HANDLER_GRENADE;
	bolt->s.eType = ET_MISSILE;
	bolt->r.svFlags = SVF_BROADCAST;
	bolt->s.weapon = grenadeWPID;
	bolt->r.ownerNum = self->s.number;
	bolt->parent = self;

	weapDef = BG_GetWeaponDef(grenadeWPID);
	Scr_SetString(&bolt->classname, scr_const.grenade);

	bolt->dmg = weapDef->damage;
	bolt->s.eFlags = EF_BOUNCE;
	bolt->clipmask = MASK_MISSILESHOT & ~CONTENTS_WATER;

	bolt->s.time = level.time + MISSILE_PRESTEP_TIME;     // move a bit on the very first frame
	bolt->s.pos.trType = TR_GRAVITY;
	bolt->s.pos.trTime = level.time;

	VectorCopy(start, bolt->s.pos.trBase);
	VectorCopy(dir, bolt->s.pos.trDelta);

	SnapVector(bolt->s.pos.trDelta);          // save net bandwidth

	bolt->s.apos.trType = TR_LINEAR;
	bolt->s.apos.trTime = level.time;
	vectoangles(dir, bolt->s.apos.trBase);

	bolt->s.apos.trBase[0] = AngleNormalize360(bolt->s.apos.trBase[0] - 120);

	bolt->s.apos.trDelta[0] = flrand(-45, 45) + 720;
	bolt->s.apos.trDelta[1] = 0;
	bolt->s.apos.trDelta[2] = flrand(-45, 45) + 360;

	VectorCopy(start, bolt->r.currentOrigin);
	VectorCopy(bolt->s.apos.trBase, bolt->r.currentAngles);

	return bolt;
}

/*
=================
fire_rocket
=================
*/
gentity_t *fire_rocket( gentity_t *self, vec3_t start, vec3_t dir )
{
	gentity_t *bolt;
	WeaponDef *weapDef;

	Vec3Normalize( dir );
	weapDef = BG_GetWeaponDef( self->s.weapon );

	bolt = G_Spawn();
	Scr_SetString(&bolt->classname, scr_const.rocket);
	bolt->nextthink = level.time + 30000;    // push it out a little
	bolt->handler = ENT_HANDLER_ROCKET;
	bolt->s.eType = ET_MISSILE;
	bolt->s.eFlags |= EF_PROJECTILE;
	bolt->r.svFlags = SVF_BROADCAST;

	//DHM - Nerve :: Use the correct weapon in multiplayer
	bolt->s.weapon = self->s.weapon;

	bolt->r.ownerNum = self->s.number;
	bolt->parent = self;
	bolt->dmg = weapDef->damage; // JPW NERVE
	bolt->clipmask = MASK_MISSILESHOT & ~CONTENTS_WATER;

	bolt->s.time = level.time + MISSILE_PRESTEP_TIME;
	bolt->s.pos.trType = TR_LINEAR;
	bolt->s.pos.trTime = level.time - MISSILE_PRESTEP_TIME;      // move a bit on the very first frame

	VectorCopy(start, bolt->s.pos.trBase);
	VectorScale(dir, weapDef->projectileSpeed, bolt->s.pos.trDelta);
	SnapVector(bolt->s.pos.trDelta);          // save net bandwidth
	VectorCopy(start, bolt->r.currentOrigin);

	vectoangles(dir, bolt->r.currentAngles);
	G_SetAngle(bolt, bolt->r.currentAngles);

	bolt->missile.time = (float)weapDef->destabilizeDistance / (float)weapDef->projectileSpeed * 1000.0f;
	bolt->flags |= self->flags & 0x20000;

	return bolt;
}


