#include "../qcommon/qcommon.h"
#include "g_shared.h"


static const char *g_HitLocNames[] =
{
	"none",
	"helmet",
	"head",
	"neck",
	"torso_upper",
	"torso_lower",
	"right_arm_upper",
	"left_arm_upper",
	"right_arm_lower",
	"left_arm_lower",
	"right_hand",
	"left_hand",
	"right_leg_upper",
	"left_leg_upper",
	"right_leg_lower",
	"left_leg_lower",
	"right_foot",
	"left_foot",
	"gun",
};

// hit location priorities for G_LocationalTrace
unsigned char bulletPriorityMap[] = { 1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0 };
unsigned char riflePriorityMap[] = { 1, 9, 9, 9, 8, 7, 6, 6, 6, 6, 5, 5, 4, 4, 4, 4, 3, 3, 0 };

static float g_fHitLocDamageMult[HITLOC_NUM];
static unsigned short g_HitLocConstNames[HITLOC_NUM];

/*
============
CanDamage

Returns qtrue if the inflictor can directly damage the target.  Used for
explosions and melee attacks.
============
*/
#define MASK_CAN_DAMAGE ( CONTENTS_SOLID | CONTENTS_GLASS | CONTENTS_MISSILECLIP | CONTENTS_SKY | CONTENTS_CLIPSHOT | CONTENTS_UNKNOWN )

void G_HitLocStrcpy( unsigned char *member, const char *keyValue );
void G_ParseHitLocDmgTable();
void LookAtKiller( gentity_t *self, gentity_t *inflictor, gentity_t *attacker );
int G_IndexForMeansOfDeath( const char *name );
float G_GetWeaponHitLocationMultiplier( int hitLoc, int weapon );
void G_Damage( gentity_t *targ, gentity_t *inflictor, gentity_t *attacker, const vec3_t dir, const vec3_t point, int damage, int dflags, int mod, int hitLoc, int timeOffset );
qboolean G_RadiusDamage( const vec3_t origin, gentity_t *inflictor, gentity_t *attacker, float fInnerDamage, float fOuterDamage, float radius, gentity_t *ignore, int mod );
unsigned short G_GetHitLocationString( int hitLoc );
int G_GetHitLocationIndexFromString( unsigned short sString );

const char *modNames[] =
{
	"MOD_UNKNOWN",
	"MOD_PISTOL_BULLET",
	"MOD_RIFLE_BULLET",
	"MOD_GRENADE",
	"MOD_GRENADE_SPLASH",
	"MOD_PROJECTILE",
	"MOD_PROJECTILE_SPLASH",
	"MOD_MELEE",
	"MOD_HEAD_SHOT",
	"MOD_CRUSH",
	"MOD_TELEFRAG",
	"MOD_FALLING",
	"MOD_SUICIDE",
	"MOD_TRIGGER_HURT",
	"MOD_EXPLOSIVE",
};



/*
============
G_HitLocStrcpy
============
*/
void G_HitLocStrcpy( unsigned char *member, const char *keyValue )
{
	strcpy((char *)member, keyValue);
}

/*
==================
G_ParseHitLocDmgTable
==================
*/
void G_ParseHitLocDmgTable()
{
	int i;
	const char *filename;
	const char *ident;
	int n;
	cspField_t pFieldList[HITLOC_NUM];
	char loadBuffer[8192];
	int len;
	fileHandle_t f;

	filename = "info/mp_lochit_dmgtable";
	ident = "LOCDMGTABLE";
	n = strlen(ident);

	for ( i = 0; i < HITLOC_NUM; i++ )
	{
		g_fHitLocDamageMult[i] = 1.0f;

		pFieldList[i].szName = g_HitLocNames[i];
		pFieldList[i].iOffset = i * sizeof( float );
		pFieldList[i].iFieldType = CSPFT_FLOAT;

		g_HitLocConstNames[i] = Scr_AllocString(g_HitLocNames[i], 1);
	}

	// the gun itself takes no damage
	g_fHitLocDamageMult[HITLOC_GUN] = 0;

	len = FS_FOpenFileByMode(filename, &f, FS_READ);

	if ( len <= 0 )
	{
		Com_Error(ERR_DROP, "\x15" "Could not load hitloc damage table %s\n", filename);
	}

	FS_Read(loadBuffer, n, f);
	loadBuffer[n] = 0;

	if ( strncmp(loadBuffer, ident, n) )
	{
		Com_Error(ERR_DROP, "\x15" "\"%s\" does not appear to be a hitloc damage table\n", filename);
	}

	if ( len - n >= (int)sizeof(loadBuffer) )
	{
		Com_Error(ERR_DROP, "\x15" "\"%s\" Is too long of a hitloc damage table to parse\n", filename);
	}

	FS_Read(loadBuffer, len - n, f);
	loadBuffer[len - n] = 0;
	FS_FCloseFile(f);

	if ( !Info_Validate(loadBuffer) )
	{
		Com_Error(ERR_DROP, "\x15" "\"%s\" is not a valid hitloc damage table\n", filename);
	}

	if ( !ParseConfigStringToStruct((unsigned char *)g_fHitLocDamageMult, pFieldList, HITLOC_NUM, loadBuffer, 0, NULL, G_HitLocStrcpy) )
	{
		Com_Error(ERR_DROP, "\x15" "Error parsing hitloc damage table %s\n", filename);
	}
}

/*
==================
LookAtKiller
==================
*/
void LookAtKiller( gentity_t *self, gentity_t *inflictor, gentity_t *attacker )
{
	vec3_t		dir;

	if ( attacker && attacker != self )
	{
		VectorSubtract (attacker->r.currentOrigin, self->r.currentOrigin, dir);
	}
	else if ( inflictor && inflictor != self )
	{
		VectorSubtract (inflictor->r.currentOrigin, self->r.currentOrigin, dir);
	}
	else
	{
		self->client->ps.stats[STAT_DEAD_YAW] = self->r.currentAngles[YAW];
		return;
	}

	assert(self->client);
	self->client->ps.stats[STAT_DEAD_YAW] = vectoyaw ( dir );
}

/*
==================
G_IndexForMeansOfDeath
==================
*/
int G_IndexForMeansOfDeath( const char *name )
{
	for ( int i = 0; i < MOD_NUM; i++ )
	{
		if ( !I_stricmp(name, modNames[i]) )
		{
			return i;
		}
	}

	Com_Printf("Unknown means of death string '%s'\n", name);
	return MOD_UNKNOWN;
}

/*
==================
player_die
==================
*/
void player_die( gentity_t *self, gentity_t *inflictor, gentity_t *attacker,
                 int damage, int meansOfDeath, int iWeapon, const vec3_t vDir,
                 int hitLoc, int psTimeOffset )
{
	int i;
	int deathAnimDuration;
	vec3_t launchvel;
	vec3_t launchspot;
	if ( !Com_GetServerDObj(self->client->ps.clientNum) )
	{
		return;
	}
	// only a living player can die
	if ( self->client->ps.pm_type != PM_NORMAL )
	{
		if ( self->client->ps.pm_type == PM_NORMAL_LINKED )
		{
			goto alive;
		}
		return;
	}
alive:
	if ( !(self->client->ps.pm_flags & PMF_FOLLOW) )
	{
		bgs = &level_bgs;
		if ( attacker->s.eType == ET_TURRET && attacker->r.ownerNum != ENTITYNUM_NONE )
		{
			attacker = &g_entities[attacker->r.ownerNum];
		}
		Scr_AddEntity(attacker);
		Scr_Notify(self, scr_const.death, 1);
		if ( iWeapon && attacker->client && (attacker->client->ps.eFlags & EF_TURRET_ACTIVE) )
		{
			gentity_t *turret = &g_entities[attacker->s.otherEntityNum];
			if ( turret->s.eType == ET_TURRET )
			{
				iWeapon = turret->s.weapon;
			}
		}
		// drop a grenade the player was still holding
		if ( self->client->ps.grenadeTimeLeft )
		{
			launchvel[0] = crandom();
			launchvel[1] = crandom();
			launchvel[2] = randomf();
			VectorScale(launchvel, 160, launchvel);
			VectorCopy(self->r.currentOrigin, launchspot);
			launchspot[2] += 40;
			fire_grenade(self, launchspot, launchvel, self->client->ps.offHandIndex, self->client->ps.grenadeTimeLeft);
		}
		self->client->ps.pm_type = self->client->ps.pm_type == PM_NORMAL_LINKED ? PM_DEAD_LINKED : PM_DEAD;
		deathAnimDuration = BG_AnimScriptEvent(&self->client->ps, ANIM_ET_DEATH, qfalse, qtrue);
		Scr_PlayerKilled(self, inflictor, attacker, damage, meansOfDeath, iWeapon, vDir, hitLoc, psTimeOffset, deathAnimDuration);
		// send updated scores to any clients that are following this one
		for ( i = 0; i < level.maxclients; i++ )
		{
			gclient_t *client = &level.clients[i];
			if ( client->sess.connected != CON_CONNECTED )
			{
				continue;
			}
			if ( client->sess.sessionState != SESS_STATE_SPECTATOR )
			{
				continue;
			}
			if ( client->spectatorClient == self->s.number )
			{
				Cmd_Score_f(&g_entities[i]);
			}
		}
		self->takedamage = qtrue;
		self->r.contents = CONTENTS_CORPSE;
		self->r.currentAngles[ROLL] = 0;
		LookAtKiller(self, inflictor, attacker);
		VectorCopy(self->r.currentAngles, self->client->ps.viewangles);
		self->s.loopSound = 0;
		SV_UnlinkEntity(self);
		self->r.maxs[2] = 30;
		SV_LinkEntity(self);
		self->health = 0;
		self->handler = ENT_HANDLER_CLIENT_DEAD;
	}
}

/*
===============
G_GetWeaponHitLocationMultiplier
===============
*/
float G_GetWeaponHitLocationMultiplier( int hitLoc, int weapon )
{
	WeaponDef *weapDef;

	assert((hitLoc >= HITLOC_NONE) && (hitLoc < HITLOC_NUM));

	if ( !weapon )
	{
		return g_fHitLocDamageMult[hitLoc];
	}

	weapDef = BG_GetWeaponDef(weapon);

	if ( !weapDef || weapDef->weaponType != WEAPTYPE_BULLET )
	{
		return g_fHitLocDamageMult[hitLoc];
	}

	return weapDef->locationDamageMultipliers[hitLoc];
}

/*
==================
G_DamageClient
==================
*/
void G_DamageClient( gentity_t *self, gentity_t *inflictor, gentity_t *attacker,
                     const vec3_t vDir, const vec3_t vPoint, int damage, int dflags,
                     int meansOfDeath, int hitLoc, int timeOffset )
{
	int weapon;
	if ( !self->takedamage )
	{
		return;
	}
	if ( self->client->noclip || self->client->ufo )
	{
		return;
	}
	if ( self->client->sess.connected != CON_CONNECTED )
	{
		return;
	}
	if ( inflictor )
	{
		weapon = inflictor->s.weapon;
	}
	else if ( attacker )
	{
		weapon = attacker->s.weapon;
	}
	else
	{
		weapon = WP_NONE;
	}
	assert((hitLoc >= HITLOC_NONE) && (hitLoc < HITLOC_NUM));
	damage *= G_GetWeaponHitLocationMultiplier(hitLoc, weapon);
	if ( damage <= 0 )
	{
		return;
	}
	Scr_PlayerDamage(self, inflictor, attacker, damage, dflags, meansOfDeath, weapon, vPoint, vDir, hitLoc, timeOffset);
}

/*
============
G_Damage

targ		entity that is being damaged
inflictor	entity that is causing the damage
attacker	entity that caused the inflictor to damage targ
	example: targ=monster, inflictor=rocket, attacker=player

dir			direction of the attack for knockback
point		point at which the damage is being inflicted, used for headshots
damage		amount of damage being inflicted
knockback	force to be applied against targ as a result of the damage

inflictor, attacker, dir, and point can be NULL for environmental effects

dflags		these flags are used to control how T_Damage works
	DAMAGE_RADIUS			damage was indirect (from a nearby explosion)
	DAMAGE_NO_ARMOR			armor does not protect from this damage
	DAMAGE_NO_KNOCKBACK		do not affect velocity, just view angles
	DAMAGE_NO_PROTECTION	kills godmode, armor, everything
============
*/
void G_Damage( gentity_t *targ, gentity_t *inflictor, gentity_t *attacker, const vec3_t dir, const vec3_t point, int damage, int dflags, int mod, int hitLoc, int timeOffset )
{
	vec3_t localdir;
	void (*die)(gentity_t *, gentity_t *, gentity_t *, int, int, const int, const float *, int, int);
	void (*pain)(gentity_t *, gentity_t *, int, const float *, const int, const float *, int);

	if ( targ->client )
	{
		G_DamageClient(targ, inflictor, attacker, dir, point, damage, dflags, mod, hitLoc, timeOffset);
		return;
	}

	if ( !targ->takedamage )
	{
		return;
	}

	if ( !inflictor )
	{
		inflictor = &g_entities[ENTITYNUM_WORLD];
	}
	if ( !attacker )
	{
		attacker = &g_entities[ENTITYNUM_WORLD];
	}

	Vec3NormalizeTo(dir, localdir);

	// check for godmode
	if ( targ->flags & FL_GODMODE )
	{
		return;
	}

	if ( damage < 1 )
	{
		damage = 1;
	}

	if ( targ->flags & FL_DEMI_GODMODE && targ->health - damage <= 0 )
	{
		damage = targ->health - 1;
	}

	if ( g_debugDamage->current.boolean )
	{
		Com_Printf("target:%i health:%i damage:%i\n", targ->s.number, targ->health, damage);
	}

	// do the damage
	targ->health -= damage;

	Scr_AddEntity(attacker);
	Scr_AddInt(damage);
	Scr_Notify(targ, scr_const.damage, 2);

	if ( targ->health <= 0 )
	{
		if ( targ->health < -999 )
		{
			targ->health = -999;
		}

		Scr_AddEntity(attacker);
		Scr_Notify(targ, scr_const.death, 1);

		die = entityHandlers[targ->handler].die;

		if ( die )
		{
			die(targ, inflictor, attacker, damage, mod, inflictor->s.weapon, localdir, hitLoc, timeOffset);
		}

		// Original statement unknown. A discarded 64-bit register value here
		// reproduces the original branch layout and generates no code.
		register long long unused = ~(long long)damage;
		return;
	}

	pain = entityHandlers[targ->handler].pain;

	if ( pain )
	{
		pain(targ, attacker, damage, point, mod, localdir, hitLoc);
	}
}

float CanDamage( gentity_t *targ, const vec3_t centerPos )
{
	float fOffset;
	vec3_t dest[5];
	int i;
	vec3_t forward;
	vec3_t right;
	vec3_t eyeOrigin;
	float halfHeight;
	int hits;

	fOffset = 15.0f;

	if ( targ->client )
	{
		G_GetPlayerViewOrigin(targ, eyeOrigin);

		halfHeight = (eyeOrigin[2] - targ->r.currentOrigin[2]) * 0.5f;
		VectorSubtract(centerPos, targ->r.currentOrigin, forward);
		forward[2] = 0;

		Vec3Normalize(forward);

		right[0] = -forward[1];
		right[1] = forward[0];
		right[2] = forward[2];

		VectorAdd(eyeOrigin, targ->r.currentOrigin, dest[0]);

		VectorScale(dest[0], 0.5f, dest[0]);
		VectorMA(dest[0], fOffset, right, dest[1]);

		dest[1][2] += halfHeight;
		VectorMA(dest[0], fOffset, right, dest[2]);

		dest[2][2] -= halfHeight;
		VectorMA(dest[0], -fOffset, right, dest[3]);

		dest[3][2] += halfHeight;
		VectorMA(dest[0], -fOffset, right, dest[4]);

		dest[4][2] -= halfHeight;

		hits = 0;

		for ( i = 0; i < 5; i++ )
		{
			if ( G_LocationalTracePassed(centerPos, dest[i], targ->s.number, MASK_CAN_DAMAGE) )
			{
				hits++;
			}
		}

		if ( !hits )
		{
			return 0;
		}

		if ( hits > 3 )
		{
			return 1.0f;
		}

		return hits / 3.0f;
	}

	// this should probably check in the plane of projection,
	// rather than in world coordinate
	VectorAdd(targ->r.absmin, targ->r.absmax, dest[0]);
	VectorScale(dest[0], 0.5f, dest[0]);

	VectorCopy(dest[0], dest[1]);
	dest[1][0] += 15.0f;
	dest[1][1] += 15.0f;

	VectorCopy(dest[0], dest[2]);
	dest[2][0] += 15.0f;
	dest[2][1] -= 15.0f;

	VectorCopy(dest[0], dest[3]);
	dest[3][0] -= 15.0f;
	dest[3][1] += 15.0f;

	VectorCopy(dest[0], dest[4]);
	dest[4][0] -= 15.0f;
	dest[4][1] -= 15.0f;

	for ( i = 0; i < 5; i++ )
	{
		if ( G_LocationalTracePassed(dest[i], centerPos, targ->s.number, MASK_CAN_DAMAGE) )
		{
			return 1.0f;
		}
	}

	return 0;
}

/*
============
G_RadiusDamage
============
*/
qboolean G_RadiusDamage( const vec3_t origin, gentity_t *inflictor, gentity_t *attacker, float fInnerDamage, float fOuterDamage, float radius, gentity_t *ignore, int mod )
{
	float points;
	float dist;
	float damageAmount;
	gentity_t *ent;
	int entityList[MAX_GENTITIES];
	int numListedEntities;
	vec3_t mins;
	vec3_t maxs;
	vec3_t v;
	vec3_t dir;
	int i;
	int e;
	qboolean hitClient;
	float boxradius;
	vec3_t dest;
	trace_t tr;
	vec3_t midpoint;

	hitClient = qfalse;

	if ( !attacker )
	{
		return qfalse;
	}

	if ( radius < 1.0f )
	{
		radius = 1.0;
	}

	// radius * sqrt(2) for bounding box enlargement
	boxradius = radius * 1.4142135f;

	for ( i = 0; i < 3; i++ )
	{
		mins[i] = origin[i] - boxradius;
		maxs[i] = origin[i] + boxradius;
	}

	numListedEntities = CM_AreaEntities(mins, maxs, entityList, MAX_GENTITIES, -1);

	for ( e = 0; e < numListedEntities; e++ )
	{
		ent = &g_entities[entityList[e]];

		if ( ent == ignore )
		{
			continue;
		}

		if ( !ent->takedamage )
		{
			continue;
		}

		if ( !ent->r.bmodel )
		{
			VectorSubtract(ent->r.currentOrigin, origin, v);
		}
		else
		{
			for ( i = 0; i < 3; i++ )
			{
				if ( origin[i] < ent->r.absmin[i] )
				{
					v[i] = ent->r.absmin[i] - origin[i];
				}
				else if ( origin[i] > ent->r.absmax[i] )
				{
					v[i] = origin[i] - ent->r.absmax[i];
				}
				else
				{
					v[i] = 0.0f;
				}
			}
		}

		dist = VectorLength(v);

		if ( dist >= radius )
		{
			continue;
		}

		if ( ent->client && level.bPlayerIgnoreRadiusDamage )
		{
			continue;
		}

		points = fOuterDamage + (fInnerDamage - fOuterDamage) * (1.0f - dist / radius);
		damageAmount = CanDamage(ent, origin);

		if ( damageAmount > 0.0f )
		{
			if ( LogAccuracyHit(ent, attacker) )
			{
				hitClient = qtrue;
			}

			// push the center of mass higher than the origin so players
			// get knocked into the air more
			VectorSubtract(ent->r.currentOrigin, origin, dir);
			dir[2] += 24.0f;

			G_Damage(ent, inflictor, attacker, dir, origin, (int)(points * damageAmount), DAMAGE_RADIUS, mod, HITLOC_NONE, 0);
		}
		else
		{
			VectorAdd(ent->r.absmin, ent->r.absmax, midpoint);
			VectorScale(midpoint, 0.5f, midpoint);
			VectorCopy(midpoint, dest);

			G_TraceCapsule(&tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, CONTENTS_SOLID | CONTENTS_GLASS | CONTENTS_SKY);

			if ( tr.fraction < 1.0 )
			{
				VectorSubtract(dest, origin, dest);
				dist = VectorLength(dest);

				// closer than a fifth of the radius
				if ( dist < radius * 0.2f )
				{
					if ( LogAccuracyHit(ent, attacker) )
					{
						hitClient = qtrue;
					}

					VectorSubtract(ent->r.currentOrigin, origin, dir);
					dir[2] += 24.0f;

					G_Damage(ent, inflictor, attacker, dir, origin, (int)(points * 0.1f), DAMAGE_RADIUS, mod, HITLOC_NONE, 0);
				}
			}
		}
	}

	return hitClient;
}

/*
===============
G_GetHitLocationString
===============
*/
unsigned short G_GetHitLocationString( int hitLoc )
{
	assert((unsigned)hitLoc < HITLOC_NUM);
	return g_HitLocConstNames[hitLoc];
}

/*
===============
G_GetHitLocationIndexFromString
===============
*/
int G_GetHitLocationIndexFromString( unsigned short sString )
{
	for ( int i = 0; i < HITLOC_NUM; i++ )
	{
		if ( g_HitLocConstNames[i] == sString )
		{
			return i;
		}
	}

	return 0;
}

// unreferenced; original name unknown
void G_NullStub()
{
}
