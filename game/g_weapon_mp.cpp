#include "../qcommon/qcommon.h"
#include "g_shared.h"

// Original name unknown; unreferenced.
static const float unusedConstant = 8192.0f;

void G_CalcMuzzlePoints( gentity_t *ent, weaponParms *wp );

#define MAX_BULLET_RESURSIONS 12

// melee trace pattern: centre, then the four corners
extern const vec2_t traceOffsets[];

static const vec2_t traceOffsets[] =
{
	{ 0.000000, 0.000000},
	{ 1.000000, 1.000000},
	{ 1.000000, -1.000000},
	{ -1.000000, 1.000000},
	{ -1.000000, -1.000000},
};

int G_GetWeaponIndexForName( const char *name );
void G_SetEquippedOffHand( int clientNum, int offHandIndex );


void gunrandom( float *x, float *y );
void Bullet_RandomSpread( float spread, vec3_t end, const weaponParms *wp, float maxRange );
int Bullet_GetDamage( const weaponParms *wp, float dist );
void G_AntiLagRewindClientPos( int gameTime, AntilagClientStore *antilagStore );
void G_AntiLag_RestoreClientPos( AntilagClientStore *antilagStore );
void G_BulletFireSpread( const gentity_t *weaponEnt, gentity_t *attacker, const weaponParms *wp, int gameTime, float spread );
void Bullet_Fire( gentity_t *attacker, float spread, weaponParms *wp, gentity_t *weaponEnt, int gametime );
gentity_t *weapon_grenadelauncher_fire( gentity_t *ent, int grenType, weaponParms *wp );
void Weapon_RocketLauncher_Fire( gentity_t *ent, float spread, weaponParms *wp );
qboolean LogAccuracyHit( gentity_t *target, gentity_t *attacker );
void FireWeapon( gentity_t *ent, int gametime );
void G_UseOffHand( gentity_t *ent );
void FireWeaponMelee( gentity_t *ent );
void G_SetupWeaponDef();
int G_GetWeaponIndexForName( const char *name );
void G_SelectWeaponIndex( int clientnum, int iWeaponIndex );
void G_SetEquippedOffHand( int clientNum, int offHandIndex );

void Bullet_Fire_Extended( const gentity_t *source, gentity_t *attacker,
                           vec3_t start, vec3_t end, float spread, int resursion,
                           const weaponParms *wp, const gentity_t *weaponEnt, int gameTime )
{
	trace_t tr;
	gentity_t *tent;
	gentity_t *traceEnt;
	vec3_t dist;
	float distLen;
	int damage, dflags = 0, mod;
	vec3_t hitPos;
	int event;
	vec3_t reflect;
	float dot, ndot;
	register byte *priorityMap;
	register int event2;
	register gentity_t *tent2;
	register int surfType;
	int surfFlags;
	if ( resursion > MAX_BULLET_RESURSIONS )
	{
		Com_DPrintf("Bullet_Fire_Extended: Too many resursions, bullet aborted\n");
		return;
	}
	if ( wp->weapDef->rifleBullet )
	{
		mod = MOD_RIFLE_BULLET;
		dflags = DAMAGE_PASSTHRU;
	}
	else
	{
		mod = MOD_PISTOL_BULLET;
	}
	if ( wp->weapDef->armorPiercing )
	{
		dflags |= DAMAGE_NO_ARMOR;
	}
	if ( wp->weapDef->rifleBullet )
		priorityMap = riflePriorityMap;
	else
		priorityMap = bulletPriorityMap;
	G_LocationalTrace(&tr, start, end, source->s.number, MASK_SHOT, priorityMap);
	Vec3Lerp(start, end, tr.fraction, hitPos);
	G_CheckHitTriggerDamage(attacker, start, hitPos, wp->weapDef->damage, mod);
	traceEnt = &g_entities[tr.entityNum];
	VectorSubtract(end, start, reflect);
	Vec3Normalize(reflect);
	dot = DotProduct(reflect, tr.normal) * -2.0f;
	VectorMA(reflect, dot, tr.normal, reflect);
	// send bullet impact
	if ( !( tr.surfaceFlags & SURF_SKY ) && !traceEnt->client && tr.fraction < 1.0f )
	{
		// legacy?
		if ( wp->weapDef->weaponClass == WEAPCLASS_SPREAD )
		{
			event = EV_BULLET_HIT_SMALL;
		}
		else if ( wp->weapDef->rifleBullet )
		{
			event = EV_SHOTGUN_HIT;
		}
		else
		{
			event = EV_BULLET_HIT_LARGE;
		}
		if ( wp->weapDef->rifleBullet )
			event2 = EV_SHOTGUN_HIT;
		else
			event2 = EV_BULLET_HIT_LARGE;
		tent = G_TempEntity(hitPos, event2);
		tent->s.eventParm  = DirToByte(tr.normal);
		tent->s.hintString = DirToByte(reflect);
		tent2 = tent;
		surfFlags = tr.surfaceFlags;
		if ( traceEnt->s.eType == ET_PLAYER_CORPSE )
			surfFlags = SURF_TYPE_FLESH << 20;
		surfType = (byte)((surfFlags & 0x1F00000) >> 20);
		tent2->s.surfType = surfType;
		tent->s.otherEntityNum = weaponEnt->s.number;
	}
	if ( tr.contents & SURF_NOIMPACT )
	{
		VectorSubtract(end, start, reflect);
		Vec3Normalize(reflect);
		ndot = -DotProduct(tr.normal, reflect);
		dot = ( ndot >= 0.125f ) ? 0.25f / ndot : 0.0f;
		VectorMA(hitPos, dot, reflect, start);
		Bullet_Fire_Extended(source, attacker, start, end, spread, resursion + 1, wp, weaponEnt, gameTime);
		return;
	}
	if ( traceEnt->takedamage )
	{
		if ( traceEnt != attacker )
		{
			VectorSubtract( start, hitPos, dist );
			distLen = VectorLength(dist);
			damage = Bullet_GetDamage( wp, distLen ) * spread;
			G_Damage( traceEnt, attacker, attacker, wp->forward, hitPos, damage, dflags, mod, tr.partGroup, level.time - gameTime );
			if ( traceEnt->client )
			{
				// allow bullets to "pass through" func_explosives if they break by taking another simultanious shot
				if ( dflags & DAMAGE_PASSTHRU )
				{
					if ( Dvar_GetInt("scr_friendlyfire") || !OnSameTeam(traceEnt, attacker) )
					{
						// start new bullet at position this hit the bmodel and continue to the end position (ignoring shot-through bmodel in next trace)
						// spread = 0 as this is an extension of an already spread shot
						Bullet_Fire_Extended( traceEnt, attacker, hitPos, end, spread * 0.5, resursion + 1, wp, weaponEnt, gameTime );
					}
				}
			}
		}
	}
}

bool Melee_Trace( gentity_t *ent, weaponParms *wp, int damage, float range, float width, float height, trace_t *traceResult, vec3_t hitOrigin )
{
	vec3_t endPos;
	int traceIndex;
	int numTraces;

	numTraces = width <= 0 && height <= 0 ? 1 : ARRAY_COUNT(traceOffsets);

	for ( traceIndex = 0; traceIndex < numTraces; traceIndex++ )
	{
		VectorMA(wp->muzzleTrace, range, wp->forward, endPos);

		VectorMA(endPos, width * traceOffsets[traceIndex][0], wp->right, endPos);
		VectorMA(endPos, (height * traceOffsets[traceIndex][1]), wp->up, endPos);

		G_LocationalTrace(traceResult, wp->muzzleTrace, endPos, ent->s.number, MASK_SHOT, bulletPriorityMap);
		Vec3Lerp(wp->muzzleTrace, endPos, traceResult->fraction, hitOrigin);

		if ( !traceIndex )
		{
			G_CheckHitTriggerDamage(ent, wp->muzzleTrace, hitOrigin, damage, MOD_MELEE);
		}

		if ( traceResult->surfaceFlags & SURF_NOIMPACT )
		{
			continue;
		}

		if ( traceResult->fraction == 1.0f )
		{
			continue;
		}

		return true;
	}

	return false;
}

void Weapon_Melee( gentity_t *ent, weaponParms *wp, float range, float width, float height )
{
	trace_t tr;
	gentity_t *traceEnt;
	gentity_t *tent;
	int damage;
	vec3_t hitOrigin;

	assert(wp);
	assert(wp->weapDef);

	damage = BG_GetWeaponDef(ent->s.weapon)->meleeDamage;

	if ( !Melee_Trace(ent, wp, damage, range, width, height, &tr, hitOrigin) )
	{
		return;
	}

	traceEnt = &g_entities[tr.entityNum];

	if ( traceEnt->client )
		tent = G_TempEntity(hitOrigin, EV_MELEE_HIT);
	else
		tent = G_TempEntity(hitOrigin, EV_MELEE_MISS);

	tent->s.otherEntityNum = tr.entityNum;
	tent->s.eventParm = DirToByte(tr.normal);
	tent->s.weapon = ent->s.weapon;

	if ( tr.entityNum == ENTITYNUM_WORLD )
	{
		return;
	}

	if ( !traceEnt->takedamage )
	{
		return;
	}

	G_Damage( traceEnt, ent, ent, wp->forward, hitOrigin, damage + rand() % 5, 0, MOD_MELEE, tr.partGroup, 0 );
}

void RoundFloatArray(float *x, float *y)
{
	int i;

	for ( i = 0; i < 3; i++ )
	{
		if ( y[i] <= x[i] )
		{
			x[i] = floor((double)x[i]);
		}
		else
		{
			x[i] = ceil((double)x[i]);
		}
	}
}

/*
===============
gunrandom
===============
*/
void gunrandom( float *x, float *y )
{
	float theta, r, sinT, cosT;

	theta = randomf() * 360;
	r = randomf();

	FastSinCos((theta * RADINDEG), &sinT, &cosT);

	*x = r * cosT;
	*y = r * sinT;
}

/*
===============
Bullet_RandomSpread
===============
*/
void Bullet_RandomSpread( float spread, vec3_t end, const weaponParms *wp, float maxRange )
{
	float right;
	float up;
	float aimOffset;
	float aimAngle;

	assert(!IS_NAN(spread));
	assert(end);
	assert(wp);

	aimAngle = tan( spread * 0.017453292f );
	aimOffset = aimAngle * maxRange;
	assert(!IS_NAN(aimOffset));

	gunrandom(&right, &up);

	right *= aimOffset;
	up    *= aimOffset;
	assert(!IS_NAN(right));
	assert(!IS_NAN(up));

	assert(!IS_NAN((wp->muzzleTrace)[0]) && !IS_NAN((wp->muzzleTrace)[1]) && !IS_NAN((wp->muzzleTrace)[2]));

	assert(!IS_NAN((wp->forward)[0]) && !IS_NAN((wp->forward)[1]) && !IS_NAN((wp->forward)[2]));
	assert(!IS_NAN((wp->right)[0]) && !IS_NAN((wp->right)[1]) && !IS_NAN((wp->right)[2]));
	assert(!IS_NAN((wp->up)[0]) && !IS_NAN((wp->up)[1]) && !IS_NAN((wp->up)[2]));

	VectorMA(wp->muzzleTrace, maxRange, wp->forward, end);
	assert(!IS_NAN((end)[0]) && !IS_NAN((end)[1]) && !IS_NAN((end)[2]));

	VectorMA(end, right, wp->right, end);
	VectorMA(end, up, wp->up, end);
	assert(!IS_NAN((end)[0]) && !IS_NAN((end)[1]) && !IS_NAN((end)[2]));
}

/*
===============
Bullet_GetDamage
===============
*/
int Bullet_GetDamage( const weaponParms *wp, float dist )
{
	int damage;
	float range;
	float lerpAmount;

	if ( dist < wp->weapDef->maxDamageRange )
	{
		damage = wp->weapDef->damage;
	}
	else if ( dist < wp->weapDef->minDamageRange )
	{
		range = wp->weapDef->minDamageRange - wp->weapDef->maxDamageRange;

		if ( range == 0 )
		{
			damage = wp->weapDef->damage;
		}
		else
		{
			lerpAmount = (dist - wp->weapDef->maxDamageRange) / range;
			damage = (int)lerp( wp->weapDef->damage, wp->weapDef->minDamage, lerpAmount );
		}
	}
	else
	{
		damage = wp->weapDef->minDamage;
	}

	return damage;
}

/*
===============
G_AntiLagRewindClientPos
===============
*/
void G_AntiLagRewindClientPos( int gameTime, AntilagClientStore *antilagStore )
{
	int client;
	int snapshotTime;
	vec3_t clientPosition;

	if ( !g_antilag->current.boolean )
	{
		return;
	}

	assert(antilagStore);
	memset(antilagStore, 0, sizeof(AntilagClientStore));
	assert(gameTime > 0);

	if ( level.time - gameTime <= 1000 / sv_fps->current.integer )
	{
		return;
	}

	for ( client = 0; client < level.maxclients; client++ )
	{
		if ( level.clients[client].sess.connected == CON_CONNECTED
			&& level.clients[client].sess.sessionState == SESS_STATE_PLAYING
			&& SV_GetClientPositionsAtTime(client, gameTime, clientPosition) )
		{
			snapshotTime = gameTime;

			memcpy(antilagStore->realClientPositions[client], g_entities[client].r.currentOrigin, sizeof(antilagStore->realClientPositions[client]));

			SV_UnlinkEntity(&g_entities[client]);
			memcpy(g_entities[client].r.currentOrigin, clientPosition, sizeof(g_entities[client].r.currentOrigin));
			SV_LinkEntity(&g_entities[client]);

			antilagStore->clientMoved[client] = true;
		}
	}
}

/*
===============
G_AntiLag_RestoreClientPos
===============
*/
void G_AntiLag_RestoreClientPos( AntilagClientStore *antilagStore )
{
	if ( !g_antilag->current.boolean )
	{
		return;
	}

	assert(antilagStore);

	for ( int client = 0; client < level.maxclients; client++ )
	{
		if ( antilagStore->clientMoved[client] )
		{
			SV_UnlinkEntity(&g_entities[client]);
			memcpy(g_entities[client].r.currentOrigin, antilagStore->realClientPositions[client], sizeof(g_entities[client].r.currentOrigin));
			SV_LinkEntity(&g_entities[client]);
		}
	}
}

/*
===============
G_BulletFireSpread
===============
*/
void G_BulletFireSpread( const gentity_t *weaponEnt, gentity_t *attacker, const weaponParms *wp, int gameTime, float spread )
{
	vec3_t end;
	vec3_t start;
	int i;

	VectorCopy(wp->muzzleTrace, start);

	for ( i = 0; i < wp->weapDef->shotCount; i++ )
	{
		Bullet_RandomSpread(spread, end, wp, wp->weapDef->minDamageRange);
		Bullet_Fire_Extended(weaponEnt, attacker, start, end, 1.0, 0, wp, weaponEnt, gameTime);
	}
}

/*
===============
Bullet_Fire
===============
*/
void Bullet_Fire( gentity_t *attacker, float spread, weaponParms *wp, gentity_t *weaponEnt, int gametime )
{
	vec3_t endpos;
	AntilagClientStore antilagClients;

	assert(attacker);
	assert(wp);
	assert(wp->weapDef);
	assert(wp->weapDef->weaponType == WEAPTYPE_BULLET);

	G_AntiLagRewindClientPos(gametime, &antilagClients);

	if ( wp->weapDef->weaponClass == WEAPCLASS_SPREAD )
	{
		G_BulletFireSpread(weaponEnt, attacker, wp, gametime, spread);
	}
	else
	{
		Bullet_RandomSpread(spread, endpos, wp, 8192);
		Bullet_Fire_Extended(weaponEnt, attacker, wp->muzzleTrace, endpos, 1.0, 0, wp, weaponEnt, gametime);
	}

	G_AntiLag_RestoreClientPos(&antilagClients);
}

/*
===============
weapon_grenadelauncher_fire
===============
*/
gentity_t *weapon_grenadelauncher_fire( gentity_t *ent, int grenType, weaponParms *wp )
{
	gentity_t *m;
	vec3_t vTossVel;
	float dot;

	assert(ent);
	assert(wp);

	VectorScale(wp->forward, wp->weapDef->projectileSpeed, vTossVel);
	vTossVel[2] += wp->weapDef->projectileSpeedUp;

	m = fire_grenade(ent, wp->muzzleTrace, vTossVel, grenType, wp->weapDef->fuseTime);

	Vec3Normalize(vTossVel);
	dot = DotProduct(ent->client->ps.velocity, vTossVel);
	VectorMA(m->s.pos.trDelta, dot, vTossVel, m->s.pos.trDelta);

	return m;
}

/*
===============
Weapon_RocketLauncher_Fire
===============
*/
void Weapon_RocketLauncher_Fire( gentity_t *ent, float spread, weaponParms *wp )
{
	float fKickDist;
	float fRandomRight;
	float fRandomUp;
	float fAimOffset;
	vec3_t dir;
	vec3_t launchpos;
	gentity_t *m;

	assert(ent);
	assert(wp);

	fKickDist = 16;
	fAimOffset = (float)tan(spread * RADINDEG) * 16.0f;

	gunrandom(&fRandomRight, &fRandomUp);

	fRandomRight *= fAimOffset;
	fRandomUp *= fAimOffset;

	VectorScale(wp->forward, 16.0f, dir);

	VectorMA(dir, fRandomRight, wp->right, dir);
	VectorMA(dir, fRandomUp, wp->up, dir);

	Vec3Normalize(dir);
	VectorCopy(wp->muzzleTrace, launchpos);

	m = fire_rocket(ent, launchpos, dir);

	if ( ent->client )
	{
		VectorMA(ent->client->ps.velocity, -64, wp->forward, ent->client->ps.velocity);
	}
}

/*
===============
LogAccuracyHit
===============
*/
qboolean LogAccuracyHit( gentity_t *target, gentity_t *attacker )
{
	assert(target);
	assert(attacker);

	if ( !target->takedamage )
	{
		return qfalse;
	}

	if ( target == attacker )
	{
		return qfalse;
	}

	if ( !target->client )
	{
		return qfalse;
	}

	if ( !attacker->client )
	{
		return qfalse;
	}

	if ( target->client->ps.pm_type >= PM_DEAD )
	{
		return qfalse;
	}

	if ( OnSameTeam( target, attacker ) )
	{
		return qfalse;
	}

	return qtrue;
}

void G_CalcMuzzlePoints( gentity_t *ent, weaponParms *wp )
{
	vec3_t viewang;

	VectorCopy(ent->client->ps.viewangles, viewang);

	viewang[0] = ent->client->fGunPitch;
	viewang[1] = ent->client->fGunYaw;

	AngleVectors(viewang, wp->forward, wp->right, wp->up);
	G_GetPlayerViewOrigin(ent, wp->muzzleTrace);
}

/*
===============
FireWeapon
===============
*/
void FireWeapon( gentity_t *ent, int gametime )
{
	weaponParms wp;
	float aimSpreadScale, fAimSpreadAmount, minSpread, maxSpread;

	if ( ent->client->ps.eFlags & EF_TURRET_ACTIVE && ent->active )
	{
		return;
	}

	wp.weapDef = BG_GetWeaponDef(ent->s.weapon);
	G_CalcMuzzlePoints(ent, &wp);

	aimSpreadScale = ent->client->currentAimSpreadScale;
	BG_GetSpreadForWeapon(&ent->client->ps, ent->s.weapon, &minSpread, &maxSpread);

	if ( ent->client->ps.fWeaponPosFrac == 1.0f )
		fAimSpreadAmount = wp.weapDef->adsSpread + (maxSpread - wp.weapDef->adsSpread) * aimSpreadScale;
	else
		fAimSpreadAmount = minSpread + (maxSpread - minSpread) * aimSpreadScale;

	if ( wp.weapDef->weaponType == WEAPTYPE_BULLET )
	{
		Bullet_Fire(ent, fAimSpreadAmount, &wp, ent, gametime);
		return;
	}

	if ( wp.weapDef->weaponType == WEAPTYPE_GRENADE )
	{
		weapon_grenadelauncher_fire(ent, ent->s.weapon, &wp);
		return;
	}

	if ( wp.weapDef->weaponType == WEAPTYPE_PROJECTILE )
	{
		Weapon_RocketLauncher_Fire(ent, fAimSpreadAmount, &wp);
		return;
	}

	Com_Error(ERR_DROP, "\x15" "Unknown weapon type %i for %s\n", wp.weapDef->weaponType, wp.weapDef->szInternalName);
}

void FireWeaponAntiLag( gentity_t *ent, int gametime )
{
	FireWeapon(ent, level.time);
}

/*
===============
G_UseOffHand
===============
*/
void G_UseOffHand( gentity_t *ent )
{
	weaponParms wp;

	assert(ent->client);
	assert(ent->client->ps.offHandIndex != WP_NONE);

	wp.weapDef = BG_GetWeaponDef(ent->client->ps.offHandIndex);
	assert(wp.weapDef->weaponType == WEAPTYPE_GRENADE);

	G_CalcMuzzlePoints(ent, &wp);
	weapon_grenadelauncher_fire(ent, ent->client->ps.offHandIndex, &wp);
}

/*
===============
FireWeaponMelee
===============
*/
void FireWeaponMelee( gentity_t *ent )
{
	weaponParms wp;

	assert(ent);
	assert(ent->client);

	if ( ent->client->ps.eFlags & EF_TURRET_ACTIVE && ent->active )
	{
		return;
	}

	wp.weapDef = BG_GetWeaponDef(ent->s.weapon);

	G_GetPlayerViewOrigin(ent, wp.muzzleTrace);
	G_GetPlayerViewDirection(ent, wp.forward, wp.right, wp.up);

	assert(player_meleeRange);
	assert(player_meleeWidth);
	assert(player_meleeHeight);

	Weapon_Melee( ent, &wp, player_meleeRange->current.decimal, player_meleeWidth->current.decimal, player_meleeHeight->current.decimal );
}

qboolean G_GivePlayerWeapon( playerState_t *pPS, int iWeaponIndex )
{
	WeaponDef *weapDef;
	WeaponDef *oldWeapDef;
	int iCurrIndex;
	int newOffHandIndex;
	int weaponSlot;

	if ( Com_BitCheck(pPS->weapons, iWeaponIndex) )
	{
		return qfalse;
	}

	weapDef = BG_GetWeaponDef(iWeaponIndex);

	if ( weapDef->weaponClass == WEAPCLASS_TURRET )
	{
		return qfalse;
	}

	if ( weapDef->weaponClass == WEAPCLASS_NON_PLAYER )
	{
		return qfalse;
	}

	Com_BitSet(pPS->weapons, iWeaponIndex);
	Com_BitClear(pPS->weaponrechamber, iWeaponIndex);

	if ( weapDef->weaponClass == WEAPCLASS_ITEM )
	{
		return qtrue;
	}

	if ( weapDef->offhandClass != OFFHAND_CLASS_NONE )
	{
		if ( !pPS->offHandIndex )
		{
			pPS->offHandIndex = iWeaponIndex;
			G_SetEquippedOffHand(pPS->clientNum, pPS->offHandIndex);
		}
		else
		{
			if ( BG_WeaponAmmo(pPS, pPS->offHandIndex) <= 0 )
			{
				oldWeapDef = BG_GetWeaponDef(pPS->offHandIndex);
				newOffHandIndex = BG_GetFirstAvailableOffhand(pPS, oldWeapDef->offhandClass);

				if ( newOffHandIndex )
					pPS->offHandIndex = newOffHandIndex;
				else
					pPS->offHandIndex = iWeaponIndex;

				G_SetEquippedOffHand(pPS->clientNum, pPS->offHandIndex);
			}
		}

		return qtrue;
	}

	weaponSlot = weapDef->weaponSlot;

	if ( weaponSlot > 0 && weaponSlot <= 2 )
	{
		if ( !pPS->weaponslots[SLOT_PRIMARY] )
		{
			pPS->weaponslots[SLOT_PRIMARY] = iWeaponIndex;
		}
		else if ( !pPS->weaponslots[SLOT_PRIMARYB] )
		{
			pPS->weaponslots[SLOT_PRIMARYB] = iWeaponIndex;
		}
	}

	for ( iCurrIndex = weapDef->altWeaponIndex; iCurrIndex && !Com_BitCheck(pPS->weapons, iCurrIndex); iCurrIndex = BG_GetWeaponDef(iCurrIndex)->altWeaponIndex )
	{
		Com_BitSet(pPS->weapons, iCurrIndex);
		Com_BitClear(pPS->weaponrechamber, iWeaponIndex);
	}

	return qtrue;
}

/*
===============
G_SetupWeaponDef
===============
*/
void G_SetupWeaponDef()
{
	Com_DPrintf("----------------------\n");
	Com_DPrintf("Game: G_SetupWeaponDef\n");

	if ( !bg_iNumWeapons )
	{
		SV_SetWeaponInfoMemory();
		ClearRegisteredItems();

		BG_ClearWeaponDef();
		BG_FillInAmmoItems(G_RegisterWeapon);

		G_GetWeaponIndexForName("defaultweapon_mp");
	}

	Com_DPrintf("----------------------\n");
}

/*
===============
G_GetWeaponIndexForName
===============
*/
int G_GetWeaponIndexForName( const char *name )
{
	return level.initializing ? BG_GetWeaponIndexForName(name, G_RegisterWeapon) : BG_FindWeaponIndexForName(name);
}

/*
===============
G_SelectWeaponIndex
===============
*/
void G_SelectWeaponIndex( int clientnum, int iWeaponIndex )
{
	SV_GameSendServerCommand(clientnum, SV_CMD_RELIABLE, va("%c %i", 97, iWeaponIndex));
}

/*
===============
G_SetEquippedOffHand
===============
*/
void G_SetEquippedOffHand( int clientNum, int offHandIndex )
{
	SV_GameSendServerCommand(clientNum, SV_CMD_RELIABLE, va("%c %i", 67, offHandIndex));
}

