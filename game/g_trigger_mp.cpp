#include "../qcommon/qcommon.h"
#include "g_shared.h"

void G_Trigger( gentity_t *self, gentity_t *other );
void InitTrigger( gentity_t *self );
void InitSentientTrigger( gentity_t *self );
void multi_trigger( gentity_t *ent, gentity_t *activator );
void Touch_Multi( gentity_t *self, gentity_t *other, qboolean bTouched );
void hurt_use( gentity_t *self, gentity_t *other, gentity_t *activator );
void SP_trigger_hurt( gentity_t *self );
qboolean Respond_trigger_damage( gentity_t *pEnt, int iMOD );
void Activate_trigger_damage( gentity_t *pEnt, gentity_t *pOther, int iDamage, int iMOD );
void Use_trigger_damage( gentity_t *pEnt, gentity_t *pOther, gentity_t *pActivator );
void Pain_trigger_damage( gentity_t *pSelf, gentity_t *pAttacker, int iDamage, const vec3_t vPoint, int iMod, const vec3_t vDir, int hitLoc );
void Die_trigger_damage( gentity_t *pSelf, gentity_t *pInflictor, gentity_t *pAttacker, int iDamage, int iMod, int iWeapon, const vec3_t vDir, int hitLoc, int timeOffset );
void SP_trigger_damage( gentity_t *pSelf );
void G_CheckHitTriggerDamage( gentity_t *pActivator, const vec3_t vStart, const vec3_t vEnd, int iDamage, int iMOD );
void SP_trigger_lookat( gentity_t *self );

/*
==============================================================================
trigger_hurt
==============================================================================
*/

/*
==============
G_Trigger
==============
*/
void G_Trigger( gentity_t *self, gentity_t *other )
{
	trigger_info_t *ti;

	assert(self->r.inuse);
	assert(other->r.inuse);

	if ( !Scr_IsSystemActive() )
	{
		return;
	}

	if ( level.pendingTriggerListSize == MAX_TRIGGERS )
	{
		Scr_AddEntity(other);
		Scr_Notify(self, scr_const.trigger, 1);
		return;
	}

	ti = &level.pendingTriggerList[level.pendingTriggerListSize];
	level.pendingTriggerListSize++;

	ti->entnum = self->s.number;
	ti->otherEntnum = other->s.number;

	ti->useCount = self->useCount;
	ti->otherUseCount = other->useCount;
}

/*
==============
InitTrigger
==============
*/
void InitTrigger( gentity_t *self )
{
	SV_SetBrushModel(self);

	self->r.contents = CONTENTS_LAVA | CONTENTS_TELEPORTER | CONTENTS_JUMPPAD | CONTENTS_CLUSTERPORTAL | CONTENTS_DONOTENTER_LARGE | CONTENTS_TRIGGER;
	self->r.svFlags = SVF_NOCLIENT;
	self->s.eFlags |= EF_NONSOLID_BMODEL;
}

void InitTriggerWait( gentity_t *ent, int spawnflag )
{
	float wait;

	if ( !level.spawnVar.spawnVarsValid )
		return;

	if ( !G_SpawnFloat("wait", "", &wait) )
		return;

	if ( wait <= 0 )
		ent->spawnflags |= spawnflag;
}

/*
==============
InitSentientTrigger
==============
*/
void InitSentientTrigger( gentity_t *self )
{
	assert(self);
	self->r.contents = 0;

	if ( !(self->spawnflags & 8) )
		self->r.contents |= CONTENTS_TRIGGER;

	if ( self->spawnflags & 1 )
		self->r.contents |= CONTENTS_TELEPORTER;

	if ( self->spawnflags & 2 )
		self->r.contents |= CONTENTS_JUMPPAD;

	if ( self->spawnflags & 4 )
		self->r.contents |= CONTENTS_CLUSTERPORTAL;
}

/*
==============
multi_trigger
==============
*/
// the trigger was just activated
// ent->activator should be set to the activator so it can be held through a delay
// so wait for the delay time before firing
void multi_trigger( gentity_t *ent, gentity_t *activator )
{
	if ( !(ent->spawnflags & 16) )
	{
		return;
	}

	G_FreeEntityDelay(ent);
}

/*
==============
Touch_Multi
==============
*/
void Touch_Multi( gentity_t *self, gentity_t *other, qboolean bTouched )
{
	G_Trigger(self, other);
	multi_trigger(self, other);
}

/*QUAKED trigger_multiple (.5 .5 .5) ? AXIS_ONLY ALLIED_ONLY NOBOT BOTONLY SOLDIERONLY LTONLY MEDICONLY ENGINEERONLY COVERTOPSONLY
"wait" : Seconds between triggerings, 0.5 default, -1 = one time only.
"random"	wait variance, default is 0
Variable sized repeatable trigger.  Must be targeted at one or more entities.
so, the basic time between firing is a random time between
(wait - random) and (wait + random)
*/
void SP_trigger_multiple( gentity_t *ent )
{
	ent->handler = ENT_HANDLER_TRIGGER_MULTIPLE;

	InitTriggerWait(ent, 16);
	InitTrigger(ent);
	InitSentientTrigger(ent);

	SV_LinkEntity(ent);
}

void SP_trigger_radius( gentity_t *ent )
{
	float radius, height;

	if ( level.spawnVar.spawnVarsValid )
	{
		if ( !G_SpawnFloat("radius", "", &radius) )
		{
			Com_Error(ERR_DROP, va("radius not specified for trigger_radius at (%g %g %g)",
			                       ent->r.currentOrigin[0], ent->r.currentOrigin[1], ent->r.currentOrigin[2]));
		}

		if ( !G_SpawnFloat("height", "", &height) )
		{
			Com_Error(ERR_DROP, va("height not specified for trigger_radius at (%g %g %g)",
			                       ent->r.currentOrigin[0], ent->r.currentOrigin[1], ent->r.currentOrigin[2]));
		}
	}
	else
	{
		if ( Scr_GetNumParam() < 5 )
		{
			Scr_Error("USAGE: spawn( \"trigger_radius\", <origin>, <spawnflags>, <radius>, <height> )");
		}

		radius = Scr_GetFloat(3);
		height = Scr_GetFloat(4);
	}

	ent->handler = ENT_HANDLER_TRIGGER_MULTIPLE;

	ent->r.mins[0] = -radius;
	ent->r.mins[1] = -radius;
	ent->r.mins[2] = 0;

	ent->r.maxs[0] = radius;
	ent->r.maxs[1] = radius;
	ent->r.maxs[2] = height;

	ent->r.svFlags = SVF_NOCLIENT | SVF_CYLINDER;

	InitTriggerWait(ent, 16);
	InitSentientTrigger(ent);

	SV_LinkEntity(ent);
}

void SP_trigger_disk( gentity_t *ent )
{
	float radius;

	if ( !G_SpawnFloat("radius", "", &radius) )
	{
		Com_Error(ERR_DROP, va("radius not specified for trigger_radius at (%g %g %g)",
		                       ent->r.currentOrigin[0], ent->r.currentOrigin[1], ent->r.currentOrigin[2]));
	}

	ent->handler = ENT_HANDLER_TRIGGER_MULTIPLE;
	radius += 64;

	ent->r.mins[0] = -radius;
	ent->r.mins[1] = -radius;
	ent->r.mins[2] = -100000;

	ent->r.maxs[0] = radius;
	ent->r.maxs[1] = radius;
	ent->r.maxs[2] = 100000;

	ent->r.svFlags = SVF_NOCLIENT | SVF_DISK;

	InitTriggerWait(ent, 16);
	InitSentientTrigger(ent);

	SV_LinkEntity(ent);
}

/*QUAKED trigger_hurt (.5 .5 .5) ? START_OFF - SILENT NO_PROTECTION SLOW ONCE
Any entity that touches this will be hurt.
It does dmg points of damage each server frame
Targeting the trigger will toggle its on / off state.

SILENT			supresses playing the sound
SLOW			changes the damage rate to once per second
NO_PROTECTION	*nothing* stops the damage

"dmg"			default 5 (whole numbers only)

"life"	time this brush will exist if value is zero will live for ever ei 0.5 sec 2.sec
default is zero

the entity must be used first before it will count down its life
*/
void hurt_touch( gentity_t *self, gentity_t *other, qboolean bTouched )
{
	int dflags;

	if ( !other->takedamage )
	{
		return;
	}

	if ( self->trigger.timestamp > level.time )
	{
		return;
	}

	G_Trigger(self, other);

	if ( self->spawnflags & 16 )
	{
		self->trigger.timestamp = level.time + 1000;
	}
	else
	{
		self->trigger.timestamp = level.time + 50;
	}

	if ( self->spawnflags & 8 )
	{
		dflags = DAMAGE_NO_PROTECTION;
	}
	else
	{
		dflags = 0;
	}
	G_Damage( other, self, self, NULL, NULL, self->dmg, dflags, MOD_TRIGGER_HURT, HITLOC_NONE, 0 );

	if ( self->spawnflags & 32 )
	{
		assert(self->handler == ENT_HANDLER_TRIGGER_HURT_TOUCH);
		self->handler = ENT_HANDLER_TRIGGER_HURT;
	}
}

/*
==============
hurt_use
==============
*/
void hurt_use( gentity_t *self, gentity_t *other, gentity_t *activator )
{
	if ( self->handler == ENT_HANDLER_TRIGGER_HURT_TOUCH )
	{
		self->handler = ENT_HANDLER_TRIGGER_HURT;
		return;
	}

	assert(self->handler == ENT_HANDLER_TRIGGER_HURT);
	self->handler = ENT_HANDLER_TRIGGER_HURT_TOUCH;
}

/*
==============
SP_trigger_hurt
==============
*/
void SP_trigger_hurt( gentity_t *self )
{
	const char *sound;

	InitTrigger(self);

	G_SpawnString("sound", "world_hurt_me", &sound);

	if ( !self->dmg )
	{
		self->dmg = 5;
	}

	self->r.contents = CONTENTS_LAVA | CONTENTS_TELEPORTER | CONTENTS_JUMPPAD | CONTENTS_CLUSTERPORTAL | CONTENTS_DONOTENTER_LARGE | CONTENTS_TRIGGER;

	// link in to the world if starting active
	if ( !(self->spawnflags & 1) )
	{
		self->handler = ENT_HANDLER_TRIGGER_HURT_TOUCH;
	}
	else
	{
		self->handler = ENT_HANDLER_TRIGGER_HURT;
	}
}

/*QUAKED trigger_once (.5 .5 .5) ? AI_Touch
Must be targeted at one or more entities.
Once triggered, this entity is destroyed
(you can actually do the same thing with trigger_multiple with a wait of -1)
*/
void SP_trigger_once( gentity_t *ent )
{
	ent->handler = ENT_HANDLER_TRIGGER_MULTIPLE;
	ent->spawnflags |= 16;

	InitTrigger(ent);
	InitSentientTrigger(ent);

	SV_LinkEntity(ent);
}

/*
==============
Respond_trigger_damage
==============
*/
qboolean Respond_trigger_damage( gentity_t *pEnt, int iMOD )
{
	int flags = pEnt->spawnflags;
	if ( (bool)(flags & 1) && iMOD == MOD_PISTOL_BULLET )
	{
		return qfalse;
	}

	if ( flags & 2 && iMOD == MOD_RIFLE_BULLET )
	{
		return qfalse;
	}

	if ( flags & 4 )
	{
		switch ( iMOD )
		{
		case MOD_GRENADE:
		case MOD_GRENADE_SPLASH:
		case MOD_PROJECTILE:
		case MOD_PROJECTILE_SPLASH:
			return qfalse;
		}
	}

	if ( flags & 8 )
	{
		switch ( iMOD )
		{
		case MOD_GRENADE:
		case MOD_GRENADE_SPLASH:
		case MOD_PROJECTILE:
		case MOD_PROJECTILE_SPLASH:
		case MOD_EXPLOSIVE:
			return qfalse;
		}
	}

	if ( flags & 16 )
	{
		switch ( iMOD )
		{
		case MOD_GRENADE_SPLASH:
		case MOD_PROJECTILE_SPLASH:
			return qfalse;
		}
	}

	if ( flags & 32 && iMOD == MOD_MELEE )
	{
		return qfalse;
	}

	if ( flags & 256 )
	{
		switch ( iMOD )
		{
		case MOD_UNKNOWN:
		case MOD_CRUSH:
		case MOD_TELEFRAG:
		case MOD_FALLING:
		case MOD_SUICIDE:
		case MOD_TRIGGER_HURT:
			return qfalse;
		}
	}

	return qtrue;
}

/*
==============
Activate_trigger_damage
==============
*/
void Activate_trigger_damage( gentity_t *pEnt, gentity_t *pOther, int iDamage, int iMOD )
{
	if ( pEnt->trigger.threshold > 0 && iDamage < pEnt->trigger.threshold )
	{
		return;
	}

	if ( !Respond_trigger_damage(pEnt, iMOD) )
	{
		return;
	}

	if ( pEnt->trigger.accumulate && 32000 - pEnt->health < pEnt->trigger.accumulate )
	{
		return;
	}

	if ( iMOD != -1 )
	{
		G_Trigger(pEnt, pOther);
	}

	pEnt->health = 32000;

	if ( !(pEnt->spawnflags & 512) )
	{
		return;
	}

	G_FreeEntityDelay(pEnt);
}

/*
==============
Use_trigger_damage
==============
*/
void Use_trigger_damage( gentity_t *pEnt, gentity_t *pOther, gentity_t *pActivator )
{
	Activate_trigger_damage(pEnt, pOther, pEnt->trigger.accumulate + 1, -1);
}

/*
==============
Pain_trigger_damage
==============
*/
void Pain_trigger_damage( gentity_t *pSelf, gentity_t *pAttacker, int iDamage, const vec3_t vPoint, int iMod, const vec3_t vDir, int hitLoc )
{
	Activate_trigger_damage(pSelf, pAttacker, iDamage, iMod);

	if ( !pSelf->trigger.accumulate )
	{
		pSelf->health = 32000;
	}
}

/*
==============
Die_trigger_damage
==============
*/
void Die_trigger_damage( gentity_t *pSelf, gentity_t *pInflictor, gentity_t *pAttacker, int iDamage, int iMod, int iWeapon, const vec3_t vDir, int hitLoc, int timeOffset )
{
	Activate_trigger_damage(pSelf, pAttacker, iDamage, iMod);

	if ( !pSelf->trigger.accumulate )
	{
		pSelf->health = 32000;
	}
}

/*
==============
SP_trigger_damage
==============
*/
void SP_trigger_damage( gentity_t *pSelf )
{
	G_SpawnInt("accumulate", "0", &pSelf->trigger.accumulate);
	G_SpawnInt("threshold", "0", &pSelf->trigger.threshold);

	pSelf->health = 32000;
	pSelf->takedamage = qtrue;
	pSelf->handler = ENT_HANDLER_TRIGGER_DAMAGE;

	InitTriggerWait(pSelf, 512);
	InitTrigger(pSelf);

	SV_LinkEntity(pSelf);
}

/*
==============
G_CheckHitTriggerDamage
==============
*/
void G_CheckHitTriggerDamage( gentity_t *pActivator, const vec3_t vStart, const vec3_t vEnd, int iDamage, int iMOD )
{
	int i;
	int num;
	int entityList[MAX_GENTITIES];
	vec3_t mins;
	vec3_t maxs;
	vec3_t absmin;
	vec3_t absmax;
	gentity_t *pEnt;

	VectorCopy(vStart, mins);
	VectorCopy(vStart, maxs);

	AddPointToBounds(vEnd, mins, maxs);

	num = CM_AreaEntities(mins, maxs, entityList, MAX_GENTITIES, CONTENTS_DONOTENTER_LARGE);

	for ( i = 0; i < num; i++ )
	{
		pEnt = &g_entities[entityList[i]];

		if ( pEnt->classname != scr_const.trigger_damage )
		{
			continue;
		}

		VectorAdd(pEnt->r.currentOrigin, pEnt->r.mins, absmin);
		VectorAdd(pEnt->r.currentOrigin, pEnt->r.maxs, absmax);

		if ( !SV_SightTraceToEntity(vStart, vec3_origin, vec3_origin, vEnd, pEnt->s.number, -1) )
		{
			continue;
		}

		Scr_AddEntity(pActivator);
		Scr_AddInt(iDamage);
		Scr_Notify(pEnt, scr_const.damage, 2);

		Activate_trigger_damage(pEnt, pActivator, iDamage, iMOD);

		if ( !pEnt->trigger.accumulate )
		{
			pEnt->health = 32000;
		}
	}
}

void G_GrenadeTouchTriggerDamage( gentity_t *pActivator, const vec3_t vStart, const vec3_t vEnd, int iDamage, int iMOD )
{
	int i;
	int num;
	int entityList[MAX_GENTITIES];
	vec3_t mins;
	vec3_t maxs;
	gentity_t *pEnt;

	VectorCopy(vStart, mins);
	VectorCopy(vStart, maxs);

	AddPointToBounds(vEnd, mins, maxs);

	num = CM_AreaEntities(mins, maxs, entityList, MAX_GENTITIES, CONTENTS_DONOTENTER_LARGE);

	for ( i = 0; i < num; i++ )
	{
		pEnt = &g_entities[entityList[i]];

		if ( pEnt->classname != scr_const.trigger_damage )
		{
			continue;
		}

		if ( !(pEnt->flags & FL_GRENADE_TOUCH_DAMAGE) )
		{
			continue;
		}

		if ( !SV_SightTraceToEntity(vStart, vec3_origin, vec3_origin, vEnd, pEnt->s.number, -1) )
		{
			continue;
		}

		Scr_AddEntity(pActivator);
		Scr_AddInt(iDamage);
		Scr_Notify(pEnt, scr_const.damage, 2);

		Activate_trigger_damage(pEnt, pActivator, iDamage, iMOD);

		if ( !pEnt->trigger.accumulate )
		{
			pEnt->health = 32000;
		}
	}
}

/*
==============
SP_trigger_lookat
==============
*/
void SP_trigger_lookat( gentity_t *self )
{
	SV_SetBrushModel(self);

	self->r.contents = CONTENTS_TRANSLUCENT;
	self->r.svFlags = SVF_NOCLIENT;
	self->s.eFlags |= EF_NONSOLID_BMODEL;

	SV_LinkEntity(self);
}

