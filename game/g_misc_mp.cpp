#include "../qcommon/qcommon.h"
#include "g_shared.h"

static turretInfo_s turretInfoStore[MAX_TURRETS];

void SP_light( gentity_t *self );
void SP_misc_model( gentity_t *self );
void SP_corona( gentity_t *self );
void G_InitTurrets();
void Turret_FillWeaponParms( gentity_t *ent, gentity_t *activator, weaponParms *wp );
void Fire_Lead( gentity_t *ent, gentity_t *activator, int unusedFlag );
void G_PlayerTurretPositionAndBlend( gentity_t *ent, gentity_t *pTurretEnt );
void turret_clientaim( gentity_t *self, gentity_t *other );
void turret_shoot_internal( gentity_t *self, gentity_t *other );
void turret_track( gentity_t *self, gentity_t *other );
void turret_UpdateSound( gentity_t *self );
void G_ClientStopUsingTurret( gentity_t *self );
void turret_think_client( gentity_t *self );
int turret_ReturnToDefaultPos( gentity_t *self, qboolean bManned );
void turret_think( gentity_t *self );
void turret_think_init( gentity_t *self );
void turret_controller( gentity_t *self, int *partBits );
qboolean turret_behind( gentity_t *self, gentity_t *other );
void G_FreeTurret( gentity_t *self );
qboolean G_IsTurretUsable( gentity_t *self, gentity_t *owner );
void G_SpawnTurret( gentity_t *self, const char *weaponinfoname );

/*
==============
turret_use
==============
*/
void turret_use( gentity_t *self, gentity_t *owner, gentity_t *activator ) // no activator for cod2
{
	turretInfo_s *pTurretInfo;
	playerState_t *ps;
	assert(self);
	pTurretInfo = self->pTurretInfo;
	assert(pTurretInfo);
	assert(owner);
	assert(owner->client);
	assert(owner->s.number < level.maxclients);
	ps = &owner->client->ps;
	assert(ps);
	owner->active = qtrue;
	self->active = qtrue;
	self->r.ownerNum = owner->s.number;
	ps->viewlocked = qtrue;
	ps->viewlocked_entNum = self->s.number;
	pTurretInfo->flags |= 0x800u;
	VectorCopy(owner->r.currentOrigin, pTurretInfo->userOrigin);
	owner->s.otherEntityNum = self->s.number;
	self->s.otherEntityNum = owner->s.number;
	if ( ps->pm_flags & PMF_PRONE )
		pTurretInfo->prevStance = TURRET_STANCE_PRONE;
	else if ( ps->pm_flags & PMF_DUCKED )
		pTurretInfo->prevStance = TURRET_STANCE_CROUCH;
	else
		pTurretInfo->prevStance = TURRET_STANCE_STAND;
	if ( pTurretInfo->stance == TURRET_STANCE_PRONE )
	{
		ps->eFlags |= EF_TURRET_PRONE;
		ps->eFlags &= ~EF_TURRET_DUCK;
	}
	else if ( pTurretInfo->stance == TURRET_STANCE_CROUCH )
	{
		ps->eFlags |= EF_TURRET_DUCK;
		ps->eFlags &= ~EF_TURRET_PRONE;
	}
	else
	{
		ps->eFlags |= EF_TURRET_ACTIVE;
	}
	// Compensating for the difference between the players view at the time of use and the start angles that the gun object has
	self->s.angles2[PITCH] = AngleSubtract(ps->viewangles[PITCH], self->r.currentAngles[PITCH]);
	self->s.angles2[PITCH] = I_fclamp(self->s.angles2[PITCH], pTurretInfo->arcmin[PITCH], pTurretInfo->arcmax[PITCH]);
	self->s.angles2[YAW] = AngleSubtract(ps->viewangles[YAW], self->r.currentAngles[YAW]);
	self->s.angles2[YAW] = I_fclamp(self->s.angles2[YAW], pTurretInfo->arcmin[YAW], pTurretInfo->arcmax[YAW]);
	self->s.angles2[ROLL] = 0;
	ps->viewAngleClampRange[PITCH] = AngleSubtract(pTurretInfo->arcmax[PITCH], pTurretInfo->arcmin[PITCH]) * 0.5f;
	ps->viewAngleClampBase[PITCH] = self->r.currentAngles[PITCH] + pTurretInfo->arcmax[PITCH];
	ps->viewAngleClampBase[PITCH] = AngleNormalize360Accurate(ps->viewAngleClampBase[PITCH] - ps->viewAngleClampRange[PITCH]);
	ps->viewAngleClampRange[YAW] = AngleSubtract(pTurretInfo->arcmax[YAW], pTurretInfo->arcmin[YAW]) * 0.5f;
	ps->viewAngleClampBase[YAW] = self->r.currentAngles[YAW] + pTurretInfo->arcmax[YAW];
	ps->viewAngleClampBase[YAW] = AngleNormalize360Accurate(ps->viewAngleClampBase[YAW] - ps->viewAngleClampRange[YAW]);
}

/*QUAKED info_null (0 0.5 0) (-4 -4 -4) (4 4 4)
Used as a positional target for calculations in the utilities (spotlights, etc), but removed during gameplay.
*/
void SP_info_null( gentity_t *self )
{
	G_FreeEntity( self );
}

/*QUAKED info_notnull (0 0.5 0) (-4 -4 -4) (4 4 4)
Used as a positional target for in-game calculation, like jumppad targets.
target_position does the same thing
*/
void SP_info_notnull( gentity_t *self )
{
	G_SetOrigin( self, self->r.currentOrigin );
}

/*
==============
SP_light
==============
*/
void SP_light( gentity_t *self )
{
	G_FreeEntity( self );
}

/*
=================================================================================

TELEPORTERS

=================================================================================
*/
void TeleportPlayer( gentity_t *player, vec3_t origin, vec3_t angles )
{
	qboolean linked;

	assert(player->client);
	assert(player->client->sess.connected != CON_DISCONNECTED);

	linked = player->r.linked;

	// unlink to make sure it can't possibly interfere with G_KillBox
	SV_UnlinkEntity( player );

	VectorCopy(origin, player->client->ps.origin);
	player->client->ps.origin[2] += 1;

	// toggle the teleport bit so the client knows to not lerp
	player->client->ps.eFlags ^= EF_TELEPORT_BIT;

	// set angles
	SetClientViewAngle( player, angles );

	// save results of pmove
	BG_PlayerStateToEntityState(&player->client->ps, &player->s, qtrue, PMOVE_HANDLER_SERVER);

	// use the precise origin for linking
	VectorCopy(player->client->ps.origin, player->r.currentOrigin);

	if ( linked )
	{
		SV_LinkEntity( player );
	}
}

/*
==============
SP_misc_model
==============
*/
void SP_misc_model( gentity_t *self )
{
	G_FreeEntity( self );
}

/*
 * @brief So level designers can toggle them on/off
 * @param[in,out] ent
 * @param other - unused
 * @param activator - unused
 */
void use_corona( gentity_t *ent, gentity_t *other, gentity_t *activator )
{
	if (ent->r.linked)
	{
		SV_UnlinkEntity(ent);
	}
	else
	{
		ent->active = qfalse;
		SV_LinkEntity(ent);
	}
}

/*
==============
SP_corona
==============
*/
void SP_corona( gentity_t *self )
{
	G_FreeEntity( self );
}

/*
==============
G_InitTurrets
==============
*/
void G_InitTurrets()
{
	for ( int i = 0; i < MAX_TURRETS; i++ )
	{
		turretInfoStore[i].inuse = qfalse;
	}
}

/*
==============
Turret_FillWeaponParms
==============
*/
void Turret_FillWeaponParms( gentity_t *ent, gentity_t *activator, weaponParms *wp )
{
	float flashTag[4][3];
	vec3_t playerPos;
	vec3_t diff;
	float dist;

	if ( !G_DObjGetWorldTagMatrix(ent, scr_const.tag_flash, flashTag) )
	{
		Com_Error(ERR_DROP, "\x15" "Couldn't find %s on turret (entity %d, classname '%s').\n", "tag_flash", ent->s.number, SL_ConvertToString(ent->classname));
	}

	G_GetPlayerViewOrigin(activator, playerPos);
	G_GetPlayerViewDirection(activator, wp->forward, wp->right, wp->up);

	VectorCopy(wp->forward, wp->gunForward);
	VectorSubtract(flashTag[3], playerPos, diff);

	dist = Vec3Normalize(diff);
	VectorMA(playerPos, dist, wp->forward, wp->muzzleTrace);
}

/*
==============
Fire_Lead
==============
*/
// the third argument is pushed as a constant 0 by the only call site and is never
// read in the body; its meaning is not pinned
void Fire_Lead( gentity_t *ent, gentity_t *activator, int unusedFlag )
{
	weaponParms wp;
	gentity_t *attacker;
	float spread;

	assert(activator);
	attacker = activator != &g_entities[ENTITYNUM_NONE] ? activator : &g_entities[ENTITYNUM_WORLD];

	Turret_FillWeaponParms(ent, attacker, &wp);
	assert(ent->pTurretInfo);
	wp.weapDef = BG_GetWeaponDef(ent->s.weapon);

	if ( !wp.weapDef->weaponType )
	{
		spread = ent->pTurretInfo->playerSpread;
		Bullet_Fire(attacker, spread, &wp, ent, level.time);
	}
	else
	{
		Weapon_RocketLauncher_Fire(ent, 0, &wp);
	}

	G_AddEvent(ent, EV_FIRE_WEAPON_MG42, attacker->s.number);
}

/*
==============
G_PlayerTurretPositionAndBlend
==============
*/
void G_PlayerTurretPositionAndBlend( gentity_t *ent, gentity_t *pTurretEnt )
{
	int i;
	int clientNum;
	int numVertChildren;
	int numHorChildren;
	int iPrevBlend;
	int iBlend;
	clientInfo_t *ci;
	lerpFrame_t *pLerpAnim;
	WeaponDef *weapDef;
	float localYaw;
	float fDelta;
	float fPrevTransZ;
	float fPrevBlend;
	float yaw;
	float fBlend;
	float w;
	float fHeightRatio;
	vec2_t rot;
	vec3_t trans;
	vec3_t turretAxis[4];
	float tagHeight;
	float localAxis[4][3];
	float axis[4][3];
	vec3_t tagAxis[3];
	DObjAnimMat *tagMat;
	XAnimTree *pAnimTree;
	XAnim *pXAnims;
	unsigned int baseAnim;
	unsigned int leafAnim1;
	unsigned int leafAnim2;
	unsigned int heightAnim;
	vec3_t vDelta;
	vec3_t start;
	vec3_t end;
	trace_t trace;
	vec3_t endpos;

	clientNum = ent->s.clientNum;
	ci = &level_bgs.clientinfo[clientNum];
	pLerpAnim = &ci->legs;

	if ( !pLerpAnim->animationNumber || !pLerpAnim->animation || !( pLerpAnim->animation->flags & 4 ) )
	{
		return;
	}

	tagMat = G_DObjGetLocalTagMatrix(pTurretEnt, scr_const.tag_weapon);

	if ( !tagMat )
	{
		Com_Printf("WARNING: aborting player positioning on turret since 'tag_weapon' does not exist\n");
		return;
	}

	weapDef = BG_GetWeaponDef(pTurretEnt->s.weapon);
	pAnimTree = ci->pXAnimTree;
	pXAnims = level_bgs.animScriptData.animTree.anims;

	baseAnim = pLerpAnim->animationNumber & ~ANIM_TOGGLEBIT;

	ConvertQuatToMat(tagMat, tagAxis);
	localYaw = vectosignedyaw(tagAxis[0]);

	AnglesToAxis(pTurretEnt->r.currentAngles, turretAxis);

	VectorCopy(pTurretEnt->r.currentOrigin, turretAxis[3]);
	VectorSubtract(ent->r.currentOrigin, turretAxis[3], vDelta);

	tagHeight = DotProduct(vDelta, turretAxis[2]);
	fDelta = tagHeight - tagMat->trans[2];

	XAnimClearTreeGoalWeightsStrict(pAnimTree, baseAnim, 0);

	numVertChildren = XAnimGetNumChildren(pXAnims, baseAnim);

	fPrevTransZ = 0;
	fPrevBlend = 0;
	iPrevBlend = 0;
	leafAnim2 = 0;

	if ( !numVertChildren )
	{
		Com_Error(ERR_DROP, "\x15" "Player anim '%s' has no children", XAnimGetAnimDebugName(pXAnims, baseAnim));
	}

	i = 0;

	do
	{
		heightAnim = XAnimGetChildAt(pXAnims, baseAnim, i);
		XAnimSetGoalWeight(pAnimTree, heightAnim, 1.0f, 1.0f, 1.0f, 0, 0, 0);
		numHorChildren = XAnimGetNumChildren(pXAnims, heightAnim);

		if ( !numHorChildren )
		{
			Com_Error(ERR_DROP, "\x15" "Player anim '%s' has no children", XAnimGetAnimDebugName(pXAnims, heightAnim));
		}

		fBlend = numHorChildren * 0.5f - localYaw / weapDef->animHorRotateInc;

		if ( fBlend < 0 )
		{
			fBlend = 0;
		}
		else if ( fBlend >= numHorChildren - 1 )
		{
			fBlend = numHorChildren - 1;
		}

		iBlend = (int)fBlend;
		fBlend -= (float)(int)iBlend;

		leafAnim1 = XAnimGetChildAt(pXAnims, heightAnim, iBlend);
		XAnimSetGoalWeight(pAnimTree, leafAnim1, 1.0f - fBlend, 1.0f, 1.0f, 0, 0, 0);

		if ( fBlend != 0 )
		{
			leafAnim2 = XAnimGetChildAt(pXAnims, heightAnim, iBlend + 1);
			XAnimSetGoalWeight(pAnimTree, leafAnim2, fBlend, 1.0f, 1.0f, 0, 0, 0);
		}

		XAnimCalcAbsDelta(pAnimTree, heightAnim, rot, trans);

		if ( trans[2] >= fDelta )
		{
			break;
		}

		fPrevTransZ = trans[2];
		iPrevBlend = iBlend;
		fPrevBlend = fBlend;

		i++;
	}
	while ( i < numVertChildren );

	XAnimClearTreeGoalWeightsStrict(pAnimTree, baseAnim, 0);

	w = I_fabs(XAnimGetWeight(pAnimTree, leafAnim1) - (1.0f - fBlend)) * (1000.0f / level.frameTime);
	XAnimSetGoalWeight(pAnimTree, leafAnim1, 1.0f - fBlend, w > 0 ? 1.0f / w : 0, 1.0f, 0, 0, 0);

	if ( fBlend != 0 )
	{
		w = I_fabs(XAnimGetWeight(pAnimTree, leafAnim2) - fBlend) * (1000.0f / level.frameTime);
		XAnimSetGoalWeight(pAnimTree, leafAnim2, fBlend, w > 0 ? 1.0f / w : 0, 1.0f, 0, 0, 0);
	}

	if ( !i || i == numVertChildren )
	{
		w = I_fabs(XAnimGetWeight(pAnimTree, heightAnim) - 1.0f) * (1000.0f / level.frameTime);
		XAnimSetGoalWeight(pAnimTree, heightAnim, 1.0f, w > 0 ? 1.0f / w : 0, 1.0f, 0, 0, 0);
	}
	else
	{
		fHeightRatio = (fDelta - fPrevTransZ) / (trans[2] - fPrevTransZ);
		w = I_fabs(XAnimGetWeight(pAnimTree, heightAnim) - fHeightRatio) * (1000.0f / level.frameTime);
		XAnimSetGoalWeight(pAnimTree, heightAnim, fHeightRatio, w > 0 ? 1.0f / w : 0, 1.0f, 0, 0, 0);

		heightAnim = XAnimGetChildAt(pXAnims, baseAnim, i - 1);
		w = I_fabs(XAnimGetWeight(pAnimTree, heightAnim) - (1.0f - fHeightRatio)) * (1000.0f / level.frameTime);
		XAnimSetGoalWeight(pAnimTree, heightAnim, 1.0f - fHeightRatio, w > 0 ? 1.0f / w : 0, 1.0f, 0, 0, 0);

		leafAnim1 = XAnimGetChildAt(pXAnims, heightAnim, iPrevBlend);
		w = I_fabs(XAnimGetWeight(pAnimTree, leafAnim1) - (1.0f - fPrevBlend)) * (1000.0f / level.frameTime);
		XAnimSetGoalWeight(pAnimTree, leafAnim1, 1.0f - fPrevBlend, w > 0 ? 1.0f / w : 0, 1.0f, 0, 0, 0);

		if ( fPrevBlend != 0 )
		{
			leafAnim2 = XAnimGetChildAt(pXAnims, heightAnim, iPrevBlend + 1);
			w = I_fabs(XAnimGetWeight(pAnimTree, leafAnim2) - fPrevBlend) * (1000.0f / level.frameTime);
			XAnimSetGoalWeight(pAnimTree, leafAnim2, fPrevBlend, w > 0 ? 1.0f / w : 0, 1.0f, 0, 0, 0);
		}
	}

	XAnimCalcAbsDelta(pAnimTree, baseAnim, rot, trans);

	RotateVec2(trans, localYaw);
	Vector2Add(trans, tagMat->trans, localAxis[3]);
	localAxis[3][2] = tagHeight;

	yaw = RotationToYaw(rot) + localYaw;
	YawToAxis(yaw, localAxis);

	MatrixMultiply43(localAxis, turretAxis, axis);
	VectorCopy(axis[3], ent->client->ps.origin);

	VectorCopy(ent->client->ps.origin, start);
	VectorCopy(ent->client->ps.origin, end);

	start[2] += ent->client->ps.viewHeightCurrent;
	end[2] -= DEFAULT_VIEWHEIGHT;

	G_TraceCapsule(&trace, start, vec3_origin, vec3_origin, end, ent->s.number, MASK_PLAYERSOLID);

	if ( trace.fraction < 1.0f )
	{
		Vec3Lerp(start, end, trace.fraction, endpos);
		ent->client->ps.origin[2] = endpos[2];
	}

	BG_PlayerStateToEntityState(&ent->client->ps, &ent->s, qtrue, PMOVE_HANDLER_SERVER);
	VectorCopy(ent->client->ps.origin, ent->r.currentOrigin);

	AxisToAngles(axis, ent->r.currentAngles);
	SV_LinkEntity(ent);
}

/*
==============
turret_clientaim
==============
*/
void turret_clientaim( gentity_t *self, gentity_t *other )
{
	turretInfo_s *pTurretInfo;
	playerState_t *ps;

	assert(self);

	pTurretInfo = self->pTurretInfo;
	assert(pTurretInfo);

	assert(other);
	assert(other->client);

	ps = &other->client->ps;
	assert(ps);
	assert(self->active);

	ps->viewlocked = qtrue;
	ps->viewlocked_entNum = self->s.number;

	self->s.angles2[0] = AngleSubtract(ps->viewangles[0], self->r.currentAngles[0]);
	self->s.angles2[0] = I_fclamp(self->s.angles2[0], pTurretInfo->arcmin[0], pTurretInfo->arcmax[0]);

	self->s.angles2[1] = AngleSubtract(ps->viewangles[1], self->r.currentAngles[1]);
	self->s.angles2[1] = I_fclamp(self->s.angles2[1], pTurretInfo->arcmin[1], pTurretInfo->arcmax[1]);

	self->s.angles2[2] = 0;

	if ( pTurretInfo->flags & 0x800 )
	{
		pTurretInfo->flags &= ~0x800;
		self->s.eFlags ^= EF_TELEPORT_BIT;
	}
}

/*
==============
turret_shoot_internal
==============
*/
void turret_shoot_internal( gentity_t *self, gentity_t *other )
{
	assert(self->pTurretInfo);
	assert(other);

	self->pTurretInfo->fireSndDelay = 3 * BG_GetWeaponDef(self->s.weapon)->fireTime;

	if ( other->client )
	{
		Fire_Lead(self, other, 0);
		other->client->ps.viewlocked = PLAYERVIEWLOCK_WEAPONJITTER;
	}
}

/*
==============
turret_track
==============
*/
void turret_track( gentity_t *self, gentity_t *other )
{
	turretInfo_s *turretInfo;
	WeaponDef *weapDef;

	turretInfo = self->pTurretInfo;
	assert(turretInfo);
	assert(self->active);
	assert(other->client);

	turret_clientaim(self, other);
	G_PlayerTurretPositionAndBlend(other, self);

	weapDef = BG_GetWeaponDef(self->s.weapon);

	other->client->ps.viewlocked = PLAYERVIEWLOCK_FULL;
	self->s.eFlags &= ~EF_FIRING;
	turretInfo->fireTime -= 50;

	if ( turretInfo->fireTime <= 0 )
	{
		turretInfo->fireTime = 0;

		if ( other->client->buttons & BUTTON_ATTACK )
		{
			turretInfo->fireTime = weapDef->fireTime;
			turret_shoot_internal(self, other);
			self->s.eFlags |= EF_FIRING;
		}
	}
}

/*
==============
turret_UpdateSound
==============
*/
void turret_UpdateSound( gentity_t *self )
{
	turretInfo_s *pTurretInfo;

	pTurretInfo = self->pTurretInfo;
	assert(pTurretInfo);

	self->s.loopSound = 0;

	if ( pTurretInfo->fireSndDelay > 0 )
	{
		self->s.loopSound = (unsigned char)pTurretInfo->fireSnd;
		pTurretInfo->fireSndDelay -= 50;

		if ( pTurretInfo->fireSndDelay <= 0 && pTurretInfo->stopSnd )
		{
			self->s.loopSound = 0;
			G_PlaySoundAlias(self, pTurretInfo->stopSnd);
		}
	}
}

/*
==============
G_ClientStopUsingTurret
==============
*/
void G_ClientStopUsingTurret( gentity_t *self )
{
	gentity_t *owner;
	turretInfo_s *pTurretInfo;

	pTurretInfo = self->pTurretInfo;
	assert(pTurretInfo);

	owner = &g_entities[self->r.ownerNum];
	assert(owner->client);

	pTurretInfo->fireSndDelay = 0;
	self->s.loopSound = 0;

	if ( pTurretInfo->prevStance != TURRET_STANCE_INVALID )
	{
		if ( pTurretInfo->prevStance == TURRET_STANCE_PRONE )
		{
			G_AddEvent(owner, EV_STANCE_FORCE_PRONE, 0);
		}
		else if ( pTurretInfo->prevStance == TURRET_STANCE_CROUCH )
		{
			G_AddEvent(owner, EV_STANCE_FORCE_CROUCH, 0);
		}
		else
		{
			G_AddEvent(owner, EV_STANCE_FORCE_STAND, 0);
		}

		pTurretInfo->prevStance = TURRET_STANCE_INVALID;
	}

	TeleportPlayer(owner, pTurretInfo->userOrigin, owner->r.currentAngles);

	owner->client->ps.eFlags &= ~EF_TURRET_ACTIVE;

	owner->client->ps.viewlocked = 0;
	owner->client->ps.viewlocked_entNum = ENTITYNUM_NONE;

	owner->active = qfalse;
	owner->s.otherEntityNum = 0;

	self->active = qfalse;
	self->r.ownerNum = ENTITYNUM_NONE;

	pTurretInfo->flags &= ~0x800;
}

/*
==============
turret_think_client
==============
*/
void turret_think_client( gentity_t *self )
{
	gentity_t *owner;

	owner = &g_entities[self->r.ownerNum];
	assert(owner->client);

	if ( owner->active == qtrue && owner->client->sess.sessionState == SESS_STATE_PLAYING )
	{
		turret_track(self, owner);
		turret_UpdateSound(self);
	}
	else
	{
		G_ClientStopUsingTurret(self);
	}
}

int turret_UpdateTargetAngles( gentity_t *self, const vec3_t desiredAngles, qboolean bManned )
{
	int i;
	vec2_t fSpeed;
	float fDelta;
	turretInfo_s *pTurretInfo;
	qboolean bComplete;
	float prevPitch;
	float pitch;

	pTurretInfo = self->pTurretInfo;

	bComplete = qtrue;

	prevPitch = self->s.angles2[0];
	self->s.angles2[0] = self->s.angles2[0] + self->s.angles2[2];

	if ( bManned )
	{
		fSpeed[0] = BG_GetWeaponDef(self->s.weapon)->maxVertTurnSpeed;
		fSpeed[1] = BG_GetWeaponDef(self->s.weapon)->maxHorTurnSpeed;
	}
	else
	{
		fSpeed[0] = 200;
		fSpeed[1] = 200;
	}

	if ( pTurretInfo->flags & 0x200 && pTurretInfo->flags & 0x100 && fSpeed[0] < 360 )
	{
		fSpeed[0] = 360;
	}

	for ( i = 0; i < 2; i++ )
	{
		fSpeed[i] = fSpeed[i] * 0.050000001f;

		fDelta = AngleSubtract(desiredAngles[i], self->s.angles2[i]);

		if ( fDelta > fSpeed[i] )
		{
			bComplete = qfalse;
			fDelta = fSpeed[i];
		}
		else if ( fDelta < -fSpeed[i] )
		{
			bComplete = qfalse;
			fDelta = -fSpeed[i];
		}

		self->s.angles2[i] = self->s.angles2[i] + fDelta;
	}

	pitch = self->s.angles2[0];
	self->s.angles2[2] = pitch;

	if ( pTurretInfo->flags & 0x200 )
	{
		if ( !(pTurretInfo->flags & 0x400) )
		{
			if ( self->s.angles2[0] > pTurretInfo->triggerDown )
				pitch = pTurretInfo->triggerDown;
			else
				pTurretInfo->flags = pTurretInfo->flags & ~0x100;
		}
		else if ( self->s.angles2[0] < pTurretInfo->triggerDown )
		{
			pitch = pTurretInfo->triggerDown;
		}
		else
		{
			pTurretInfo->flags = pTurretInfo->flags & ~0x100;
		}
	}

	fDelta = AngleSubtract(pitch, prevPitch);

	if ( fDelta > fSpeed[0] )
	{
		bComplete = qfalse;
		fDelta = fSpeed[0];
	}
	else if ( fDelta < -fSpeed[0] )
	{
		bComplete = qfalse;
		fDelta = -fSpeed[0];
	}

	self->s.angles2[0] = prevPitch + fDelta;
	self->s.angles2[2] = self->s.angles2[2] - self->s.angles2[0];

	return bComplete;
}

/*
==============
turret_ReturnToDefaultPos
==============
*/
int turret_ReturnToDefaultPos( gentity_t *self, qboolean bManned )
{
	turretInfo_s *pTurretInfo;
	vec2_t desiredAngles;

	pTurretInfo = self->pTurretInfo;

	desiredAngles[0] = bManned ? 0.0f : pTurretInfo->dropPitch;
	desiredAngles[1] = 0;

	return turret_UpdateTargetAngles( self, desiredAngles, bManned );
}

/*
==============
turret_think
==============
*/
void turret_think( gentity_t *self )
{
	gentity_t *owner;
	turretInfo_s *pTurretInfo;

	pTurretInfo = self->pTurretInfo;
	self->nextthink = level.time + 50;
	if ( self->tagInfo )
	{
		G_GeneralLink(self);
	}
	owner = &g_entities[self->r.ownerNum];
	if ( owner->client )
	{
	}
	else
	{
		turret_UpdateSound(self);
		self->s.eFlags &= ~EF_FIRING;
		turret_ReturnToDefaultPos(self, qfalse);
	}
}

/*
==============
turret_think_init
==============
*/
void turret_think_init( gentity_t *self )
{
	const int numSteps = 30;
	turretInfo_s *pTurretInfo;
	DObjAnimMat *aimMtx;
	DObjAnimMat *weaponMtx;
	float baseMtx[4][3];
	float mtx[3][3];
	vec3_t angles;
	vec3_t dir;
	vec3_t transDir;
	vec3_t start;
	vec3_t end;
	trace_t trace;
	int i;

	assert(self);
	assert(self->handler == ENT_HANDLER_TURRET_INIT);

	pTurretInfo = self->pTurretInfo;
	assert(pTurretInfo);

	self->handler = ENT_HANDLER_TURRET;
	self->nextthink = level.time + 50;

	aimMtx = G_DObjGetLocalTagMatrix(self, scr_const.tag_aim);

	if ( !aimMtx )
	{
		return;
	}

	weaponMtx = G_DObjGetLocalTagMatrix(self, scr_const.tag_butt);

	if ( !weaponMtx )
	{
		return;
	}

	AnglesToAxis(self->r.currentAngles, baseMtx);
	VectorCopy(self->r.currentOrigin, baseMtx[3]);
	VectorSubtract(weaponMtx->trans, aimMtx->trans, dir);
	MatrixTransformVector43(aimMtx->trans, baseMtx, start);

	for ( i = 0; i <= numSteps; i++ )
	{
		angles[0] = i * (-90.0f / numSteps);
		angles[1] = 0;
		angles[2] = 0;

		AnglesToAxis(angles, mtx);
		MatrixTransformVector(dir, mtx, transDir);
		VectorAdd(aimMtx->trans, transDir, transDir);
		MatrixTransformVector43(transDir, baseMtx, end);

		G_LocationalTrace(&trace, start, end, self->s.number, CONTENTS_SOLID | CONTENTS_GLASS | CONTENTS_SKY, bulletPriorityMap);

		if ( trace.fraction < 1.0f )
		{
			pTurretInfo->dropPitch = angles[0];
			return;
		}
	}
}

/*
==============
turret_controller
==============
*/
void turret_controller( gentity_t *self, int *partBits )
{
	vec3_t angles;
	DObj *obj;

	angles[1] = self->s.angles2[1];
	angles[0] = self->s.angles2[0];
	angles[2] = 0;

	obj = Com_GetServerDObj(self->s.number);

	DObjSetControlTagAngles(obj, partBits, scr_const.tag_aim, angles);
	DObjSetControlTagAngles(obj, partBits, scr_const.tag_aim_animated, angles);

	angles[0] = self->s.angles2[2];
	angles[1] = 0;

	DObjSetControlTagAngles(obj, partBits, scr_const.tag_flash, angles);
}

/*
==============
turret_behind
==============
*/
qboolean turret_behind( gentity_t *self, gentity_t *other )
{
	turretInfo_s *pTurretInfo;
	vec3_t dir;
	vec3_t forward;
	float minYaw;
	float yawSpan;
	float centerYaw;
	float dot;
	float angle;

	pTurretInfo = self->pTurretInfo;
	assert(pTurretInfo);
	assert(other->client);

	minYaw = self->r.currentAngles[1] + pTurretInfo->arcmin[1];
	yawSpan = (I_fabs(pTurretInfo->arcmin[1]) + I_fabs(pTurretInfo->arcmax[1])) * 0.5f;
	centerYaw = AngleNormalize180(minYaw + yawSpan);

	YawVectors(centerYaw, forward, 0);
	Vec3Normalize(forward);

	VectorSubtract(self->r.currentOrigin, other->r.currentOrigin, dir);
	dir[2] = 0;

	Vec3Normalize(dir);

	dot = DotProduct(forward, dir);
	angle = Q_acos(I_fclamp(dot, -1.0, 1.0)) * DEGINRAD;

	if ( angle > yawSpan )
	{
		return qfalse;
	}

	return qtrue;
}

/*
==============
G_FreeTurret
==============
*/
void G_FreeTurret( gentity_t *self )
{
	gentity_t *owner;
	turretInfo_s *pTurretInfo;

	owner = &g_entities[self->r.ownerNum];
	pTurretInfo = self->pTurretInfo;

	if ( owner->client )
	{
		G_ClientStopUsingTurret(self);
	}

	self->active = qfalse;

	assert(self->pTurretInfo);
	self->pTurretInfo->inuse = qfalse;
	self->pTurretInfo = NULL;
}

/*
==============
G_IsTurretUsable
==============
*/
qboolean G_IsTurretUsable( gentity_t *self, gentity_t *owner )
{
	if ( self->active || !self->pTurretInfo )
	{
		return qfalse;
	}

	if ( !turret_behind(self, owner) )
	{
		return qfalse;
	}

	if ( owner->client->ps.grenadeTimeLeft )
	{
		return qfalse;
	}

	if ( owner->client->ps.groundEntityNum == ENTITYNUM_NONE )
	{
		return qfalse;
	}

	return qtrue;
}

/*
==============
G_SpawnTurret
==============
*/
void G_SpawnTurret( gentity_t *self, const char *weaponinfoname )
{
	WeaponDef *weapDef;
	turretInfo_s *turretInfo = NULL;
	int i;

	for ( i = 0; i < MAX_TURRETS; i++ )
	{
		turretInfo = &turretInfoStore[i];

		if ( !turretInfo->inuse )
		{
			break;
		}
	}

	if ( i == MAX_TURRETS )
	{
		Com_Error(ERR_DROP, "\x15" "G_SpawnTurret: max number of turrets (%d) exceeded", i);
	}

	memset(turretInfo, 0, sizeof(*turretInfo));
	self->pTurretInfo = turretInfo;
	turretInfo->inuse = qtrue;

	self->s.weapon = G_GetWeaponIndexForName(weaponinfoname);

	if ( !self->s.weapon )
	{
		Com_Error(ERR_DROP, "\x15" "bad weaponinfo '%s' specified for turret", weaponinfoname);
	}

	weapDef = BG_GetWeaponDef(self->s.weapon);

	if ( weapDef->weaponClass != WEAPCLASS_TURRET )
	{
		Scr_Error(va("G_SpawnTurret: weapon '%s' isn't a turret. This usually indicates that the weapon failed to load.", weaponinfoname));
	}

	if ( !level.initializing && !IsItemRegistered(self->s.weapon) )
	{
		Scr_Error(va("turret '%s' not precached", weaponinfoname));
	}

	turretInfo->fireTime = 0;
	turretInfo->stance = weapDef->stance;
	turretInfo->prevStance = TURRET_STANCE_INVALID;
	turretInfo->fireSndDelay = 0;

	if ( weapDef->loopFireSound && *weapDef->loopFireSound )
		turretInfo->fireSnd = G_SoundAliasIndex(weapDef->loopFireSound);
	else
		turretInfo->fireSnd = 0;

	if ( weapDef->loopFireSoundPlayer && *weapDef->loopFireSoundPlayer )
		turretInfo->fireSndPlayer = G_SoundAliasIndex(weapDef->loopFireSoundPlayer);
	else
		turretInfo->fireSndPlayer = 0;

	if ( weapDef->stopFireSound && *weapDef->stopFireSound )
		turretInfo->stopSnd = G_SoundAliasIndex(weapDef->stopFireSound);
	else
		turretInfo->stopSnd = 0;

	if ( weapDef->stopFireSoundPlayer && *weapDef->stopFireSoundPlayer )
		turretInfo->stopSndPlayer = G_SoundAliasIndex(weapDef->stopFireSoundPlayer);
	else
		turretInfo->stopSndPlayer = 0;

	if ( !level.spawnVar.spawnVarsValid || !G_SpawnFloat("rightarc", "", &turretInfo->arcmin[1]) )
	{
		turretInfo->arcmin[1] = weapDef->rightArc;
	}

	turretInfo->arcmin[1] = turretInfo->arcmin[1] * -1.0f;

	if ( turretInfo->arcmin[1] > 0 )
	{
		turretInfo->arcmin[1] = 0;
	}

	if ( !level.spawnVar.spawnVarsValid || !G_SpawnFloat("leftarc", "", &turretInfo->arcmax[1]) )
	{
		turretInfo->arcmax[1] = weapDef->leftArc;
	}

	if ( turretInfo->arcmax[1] < 0 )
	{
		turretInfo->arcmax[1] = 0;
	}

	if ( !level.spawnVar.spawnVarsValid || !G_SpawnFloat("toparc", "", turretInfo->arcmin) )
	{
		turretInfo->arcmin[0] = weapDef->topArc;
	}

	turretInfo->arcmin[0] = turretInfo->arcmin[0] * -1.0f;

	if ( turretInfo->arcmin[0] > 0 )
	{
		turretInfo->arcmin[0] = 0;
	}

	if ( !level.spawnVar.spawnVarsValid || !G_SpawnFloat("bottomarc", "", turretInfo->arcmax) )
	{
		turretInfo->arcmax[0] = weapDef->bottomArc;
	}

	if ( turretInfo->arcmax[0] < 0 )
	{
		turretInfo->arcmax[0] = 0;
	}

	turretInfo->dropPitch = -90;

	if ( !self->health )
	{
		self->health = 100;
	}

	if ( !level.spawnVar.spawnVarsValid || !G_SpawnInt("damage", "0", &self->dmg) )
	{
		self->dmg = weapDef->damage;
	}

	if ( self->dmg < 0 )
	{
		self->dmg = 0;
	}

	if ( !level.spawnVar.spawnVarsValid || !G_SpawnFloat("playerSpread", "1", &turretInfo->playerSpread) )
	{
		turretInfo->playerSpread = weapDef->playerSpread;
	}

	if ( turretInfo->playerSpread < 0 )
	{
		turretInfo->playerSpread = 0;
	}

	turretInfo->flags = 3;

	self->clipmask = CONTENTS_SOLID;
	self->r.contents = CONTENTS_NONCOLLIDING | CONTENTS_DONOTENTER;
	self->r.svFlags = 0;
	self->s.eType = ET_TURRET;
	self->flags |= 0x1000u;

	G_DObjUpdate(self);

	VectorSet(self->r.mins, -32, -32, 0);
	VectorSet(self->r.maxs, 32, 32, 56);

	G_SetOrigin(self, self->r.currentOrigin);
	G_SetAngle(self, self->r.currentAngles);

	VectorClear(self->s.angles2);

	self->handler = ENT_HANDLER_TURRET_INIT;
	self->nextthink = level.time + 50;

	self->s.apos.trType = TR_LINEAR_STOP;
	self->takedamage = qfalse;

	SV_LinkEntity(self);
}

/*QUAKED misc_mg42 (1 0 0) (-16 -16 -24) (16 16 24) HIGH NOTRIPOD
harc = horizonal fire arc Default is 57.5 (left and right)
varc = vertical fire arc Default is 45 (upwards and 22.5 down)
health = how much damage can it take default is 50
damage = determines how much the weapon will inflict if a non player uses it
"accuracy" all guns are 100% accurate an entry of 0.5 would make it 50%
*/
void SP_turret( gentity_t *self )
{
	const char *weaponinfoname;

	if ( !G_SpawnString("weaponinfo", "", &weaponinfoname) )
	{
		Com_Error( ERR_DROP, "\x15" "no weaponinfo specified for turret" );
	}

	G_SpawnTurret( self, weaponinfoname );
}

