#include "../qcommon/qcommon.h"
#include "g_shared.h"

void Player_UseEntity( gentity_t *playerEnt, gentity_t *useEnt );
bool Player_ActivateCmd( gentity_t *ent );
void Player_ActivateHoldCmd( gentity_t *ent );
void Player_UpdateActivate( gentity_t *ent );
static signed int compare_use( const void *num1, const void *num2 );
int Player_GetUseList( gentity_t *ent, useList_t *useList );
int Player_GetItemCursorHint( gclient_t *client, gentity_t *traceEnt );
void Player_SetTurretDropHint( gentity_t *ent );
void Player_UpdateCursorHints( gentity_t *ent );
void Player_UpdateLookAtEntity( gentity_t *ent );

extern const vec3_t useRadius;
static const vec3_t useRadius = { 192.0f, 192.0f, 96.0f };

/*
===============
Player_UpdateLookAtEntityTrace
===============
*/
gentity_t* Player_UpdateLookAtEntityTrace( trace_t *trace, const vec3_t start, const vec3_t end,
        int passentitynum, int contentmask, unsigned char *priorityMap, vec3_t vForward )
{
	vec3_t hitPos;
	float visibility;
	G_LocationalTrace(trace, start, end, passentitynum, contentmask, priorityMap);
	if ( trace->entityNum >= ENTITYNUM_WORLD )
	{
		return NULL;
	}
	// reject targets hidden by smoke
	VectorMA(start, trace->fraction * 15000.0f, vForward, hitPos);
	visibility = SV_FX_GetVisibility(start, hitPos);
	if ( visibility < 0.2f )
	{
		return NULL;
	}
	return &g_entities[trace->entityNum];
}

/*
===============
Player_UseEntity
===============
*/
void Player_UseEntity( gentity_t *playerEnt, gentity_t *useEnt )
{
	void (*use)(gentity_t *, gentity_t *, gentity_t *);
	void (*touch)(gentity_t *, gentity_t *, int);

	use = entityHandlers[useEnt->handler].use;
	touch = entityHandlers[useEnt->handler].touch;

	if ( useEnt->s.eType == ET_ITEM )
	{
		Scr_AddEntity(playerEnt);
		Scr_Notify(useEnt, scr_const.touch, 1);

		useEnt->active = qtrue;

		if ( touch )
		{
			touch(useEnt, playerEnt, 0);
		}
	}
	else if ( useEnt->s.eType != ET_TURRET || G_IsTurretUsable(useEnt, playerEnt) )
	{
		Scr_AddEntity(playerEnt);
		Scr_Notify(useEnt, scr_const.trigger, 1);

		if ( use )
		{
			use(useEnt, playerEnt, playerEnt);
		}
	}

	playerEnt->client->useHoldEntity = ENTITYNUM_NONE;
}

/*
===============
Player_ActivateCmd
===============
*/
bool Player_ActivateCmd( gentity_t *ent )
{
	if ( !Scr_IsSystemActive() )
	{
		return false;
	}

	ent->client->useHoldEntity = ENTITYNUM_NONE;

	if ( ent->active )
	{
		if ( ent->client->ps.eFlags & EF_TURRET_ACTIVE )
			ent->active = ACTIVE_TURRET;
		else
			ent->active = qfalse;

		return true;
	}

	if ( ent->client->ps.pm_flags & PMF_MANTLE )
	{
		return true;
	}

	if ( ent->client->ps.cursorHintEntIndex == ENTITYNUM_NONE )
	{
		return false;
	}

	ent->client->useHoldEntity = ent->client->ps.cursorHintEntIndex;
	ent->client->useHoldTime = level.time;

	return true;
}

/*
===============
Player_ActivateHoldCmd
===============
*/
void Player_ActivateHoldCmd( gentity_t *ent )
{
	gentity_t *useEnt;

	if ( !Scr_IsSystemActive() )
	{
		return;
	}

	if ( ent->client->useHoldEntity == ENTITYNUM_NONE )
	{
		return;
	}

	if ( level.time - ent->client->lastSpawnTime < g_useholdspawndelay->current.integer )
	{
		return;
	}

	if ( level.time - ent->client->useHoldTime < g_useholdtime->current.integer )
	{
		return;
	}

	useEnt = &g_entities[ent->client->useHoldEntity];
	Player_UseEntity(ent, useEnt);
}

/*
===============
Player_UpdateActivate
===============
*/
void Player_UpdateActivate( gentity_t *ent )
{
	bool useSucceeded;

	ent->client->ps.pm_flags &= ~PMF_RELOAD;

	// using binoculars
	if ( ent->client->ps.weaponstate >= WEAPON_BINOCULARS_INIT && ent->client->ps.weaponstate <= WEAPON_BINOCULARS_END )
	{
		return;
	}

	useSucceeded = false;

	if ( ent->client->useHoldEntity != ENTITYNUM_NONE && ent->client->oldbuttons & BUTTON_USERELOAD && !(ent->client->buttons & BUTTON_USERELOAD) )
	{
		ent->client->ps.pm_flags |= PMF_RELOAD;
		return;
	}

	if ( ent->client->latched_buttons & ( BUTTON_USE | BUTTON_USERELOAD ) )
	{
		useSucceeded = Player_ActivateCmd(ent);
	}

	if ( ent->client->useHoldEntity == ENTITYNUM_NONE && !useSucceeded )
	{
		if ( ent->client->latched_buttons & BUTTON_USERELOAD )
		{
			ent->client->ps.pm_flags |= PMF_RELOAD;
		}
	}
	else if ( ent->client->buttons & ( BUTTON_USE | BUTTON_USERELOAD ) )
	{
		Player_ActivateHoldCmd(ent);
	}
}

/*
===============
compare_use
===============
*/
static signed int compare_use( const void *num1, const void *num2 )
{
	useList_t *a;
	useList_t *b;

	a = (useList_t *)num1;
	b = (useList_t *)num2;

	return a->score - b->score;
}

/*
===============
Player_GetUseList
===============
*/
int Player_GetUseList( gentity_t *ent, useList_t *useList )
{
	playerState_t *ps;
	gentity_t *gEnt;
	vec3_t forward;
	vec3_t mins;
	vec3_t maxs;
	vec3_t origin;
	vec3_t midpoint;
	vec3_t dest;
	float dist;
	float frac;
	int entityList[MAX_GENTITIES];
	int num;
	int useCount;
	int i;
	int otherEntCount;
	vec3_t absmin;
	vec3_t absmax;
	float maxDist;
	int itemEntCount;
	float dot;

	maxDist = 256.0f; // never read
	itemEntCount = 0;

	ps = &ent->client->ps;

	G_GetPlayerViewOrigin(ent, origin);
	G_GetPlayerViewDirection(ent, forward, NULL, NULL);

	VectorAdd(ps->origin, ps->mins, absmin);
	VectorAdd(ps->origin, ps->maxs, absmax);

	VectorSubtract(origin, useRadius, mins);
	VectorAdd(origin, useRadius, maxs);

	num = CM_AreaEntities(mins, maxs, entityList, MAX_GENTITIES, CONTENTS_DONOTENTER);
	useCount = 0;

	for ( i = 0; i < num; i++ )
	{
		gEnt = &g_entities[entityList[i]];

		if ( ent == gEnt )
		{
			continue;
		}

		if ( gEnt->s.eType != ET_ITEM && !(gEnt->r.contents & CONTENTS_DONOTENTER) )
		{
			continue;
		}

		if ( gEnt->classname == scr_const.trigger_use_touch )
		{
			if ( gEnt->r.absmin[0] > absmax[0] )
			{
				continue;
			}

			if ( gEnt->r.absmax[0] < absmin[0] )
			{
				continue;
			}

			if ( gEnt->r.absmin[1] > absmax[1] )
			{
				continue;
			}

			if ( gEnt->r.absmax[1] < absmin[1] )
			{
				continue;
			}

			if ( gEnt->r.absmin[2] > absmax[2] )
			{
				continue;
			}

			if ( gEnt->r.absmax[2] < absmin[2] )
			{
				continue;
			}

			if ( !SV_EntityContact(absmin, absmax, gEnt) )
			{
				continue;
			}

			useList[useCount].score = -256.0f;
			useList[useCount].ent = gEnt;
			useCount++;
		}
		else
		{
			VectorAdd(gEnt->r.absmin, gEnt->r.absmax, midpoint);
			VectorScale(midpoint, 0.5f, midpoint);
			VectorSubtract(midpoint, origin, dest);

			dist = Vec3Normalize(dest);

			if ( dist > 128.0f )
			{
				continue;
			}

			dot = DotProduct(dest, forward);
			frac = 1.0f - (dot + 1.0f) * 0.5f;
			useList[useCount].score = frac * 256.0f;

			if ( gEnt->classname == scr_const.trigger_use )
			{
				useList[useCount].score -= 256.0f;
			}

			if ( gEnt->s.eType == ET_ITEM && !BG_CanItemBeGrabbed(&gEnt->s, &ent->client->ps, qfalse) )
			{
				useList[useCount].score += 10000.0f;
				itemEntCount++;
			}

			useList[useCount].ent = gEnt;
			useList[useCount].score += dist;
			useCount++;
		}
	}

	qsort(useList, useCount, sizeof(useList_t), compare_use);
	useCount -= itemEntCount;
	otherEntCount = 0;

	for ( i = 0; i < useCount; i++ )
	{
		ent = useList[i].ent;

		if ( ent->classname == scr_const.trigger_use_touch )
		{
			continue;
		}

		VectorAdd(ent->r.absmin, ent->r.absmax, midpoint);
		VectorScale(midpoint, 0.5f, midpoint);

		if ( ent->s.eType == ET_TURRET )
		{
			G_DObjGetWorldTagPos(ent, scr_const.tag_aim, midpoint);
		}

		if ( G_TraceCapsuleComplete(origin, vec3_origin, vec3_origin, midpoint, ps->clientNum, CONTENTS_SOLID | CONTENTS_GLASS) )
		{
			continue;
		}

		useList[i].score += 10000.0f;
		otherEntCount++;
	}

	qsort(useList, useCount, sizeof(useList_t), compare_use);
	useCount -= otherEntCount;

	return useCount;
}

/*
===============
Player_GetItemCursorHint
===============
*/
int Player_GetItemCursorHint( gclient_t *client, gentity_t *traceEnt )
{
	gitem_t *item;
	int hint;
	WeaponDef *weapDef;

	item = &bg_itemlist[traceEnt->item.index];
	hint = 0;

	switch ( item->giType )
	{
	case IT_WEAPON:
		weapDef = BG_GetWeaponDef(item->giTag);

		if ( weapDef->weaponType == WEAPTYPE_GRENADE )
			break;

		if ( !Com_BitCheck(client->ps.weapons, item->giTag) )
			hint = item->giTag + 4;
		break;
	}

	return hint;
}

/*
===============
Player_SetTurretDropHint
===============
*/
void Player_SetTurretDropHint( gentity_t *ent )
{
	gentity_t *turret;
	playerState_t *ps;

	ps = &ent->client->ps;
	turret = &level.gentities[ps->viewlocked_entNum];

	if ( *BG_GetWeaponDef(turret->s.weapon)->dropHintString )
	{
		ps->cursorHintEntIndex = ENTITYNUM_NONE;
		ps->cursorHint = turret->s.weapon + 4;
		ps->cursorHintString = BG_GetWeaponDef(turret->s.weapon)->dropHintStringIndex;
	}
}

/*
===============
Player_UpdateCursorHints
===============
*/
void Player_UpdateCursorHints( gentity_t *ent )
{
	useList_t useList[MAX_GENTITIES];
	playerState_t *ps;
	gentity_t *traceEnt = NULL;
	int hint;
	int itemHint;
	int hintString;
	int num;
	int i;

	ps = &ent->client->ps;
	ps->cursorHint = 0;
	ps->cursorHintString = -1;
	ps->cursorHintEntIndex = ENTITYNUM_NONE;

	// dead
	if ( ent->health <= 0 )
		return;

	// using binoculars
	if ( ent->client->ps.weaponstate >= WEAPON_BINOCULARS_INIT && ent->client->ps.weaponstate <= WEAPON_BINOCULARS_END )
		return;

	// using a turret
	if ( ent->active )
	{
		if ( ps->eFlags & EF_TURRET_ACTIVE )
			Player_SetTurretDropHint(ent);
		return;
	}

	// mantling
	if ( ent->client->ps.pm_flags & PMF_MANTLE )
		return;

	num = Player_GetUseList(ent, useList);

	if ( !num )
		return;

	hint = 0;
	hintString = -1;

	for ( i = 0; i < num; i++ )
	{
		traceEnt = useList[i].ent;

		switch ( traceEnt->s.eType )
		{
		case ET_GENERAL:
			if ( traceEnt->classname != scr_const.trigger_use && traceEnt->classname != scr_const.trigger_use_touch )
				break;

			if ( (!traceEnt->team || traceEnt->team == ent->client->sess.cs.team)
			        && (traceEnt->trigger.singleUserEntIndex == ENTITYNUM_NONE || traceEnt->trigger.singleUserEntIndex == ent->client->ps.clientNum) )
			{
				hint = traceEnt->s.hintType;

				if ( traceEnt->s.hintType && traceEnt->s.hintString != 255 )
					hintString = traceEnt->s.hintString;
				break;
			}
			continue;

		case ET_TURRET:
			if ( !G_IsTurretUsable(traceEnt, ent) )
				continue;

			hint = traceEnt->s.weapon + 4;

			if ( *BG_GetWeaponDef(traceEnt->s.weapon)->useHintString )
				hintString = BG_GetWeaponDef(traceEnt->s.weapon)->useHintStringIndex;
			break;

		case ET_ITEM:
			itemHint = Player_GetItemCursorHint(ent->client, traceEnt);

			if ( !itemHint )
				continue;

			hint = itemHint;
			break;

		default:
			continue;
		}

		ps->cursorHintEntIndex = traceEnt->s.number;
		ps->cursorHint = hint;
		ps->cursorHintString = hintString;

		if ( !ps->cursorHint )
			ps->cursorHintEntIndex = ENTITYNUM_NONE;

		return;
	}
}

/*
===============
Player_UpdateLookAtEntity
===============
*/
void Player_UpdateLookAtEntity( gentity_t *ent )
{
	WeaponDef *weapDef;
	playerState_t *ps;
	gentity_t *lookAtEnt;
	trace_t trace;
	vec3_t vEyePosition;
	vec3_t vEnd;
	vec3_t vForward;
	vec3_t vDelta;
	unsigned char *priorityMap;

	ps = &ent->client->ps;
	ps->pm_flags &= ~( PMF_LOOKAT_FRIEND | PMF_LOOKAT_ENEMY );
	ent->client->pLookatEnt = NULL;

	G_GetPlayerViewOrigin(ent, vEyePosition);
	G_GetPlayerViewDirection(ent, vForward, NULL, NULL);

	if ( ps->eFlags & EF_TURRET_ACTIVE )
		weapDef = BG_GetWeaponDef(g_entities[ps->viewlocked_entNum].s.weapon);
	else
		weapDef = BG_GetWeaponDef(ent->client->ps.weapon);

	if ( ent->client->ps.weapon && weapDef->rifleBullet )
		priorityMap = riflePriorityMap;
	else
		priorityMap = bulletPriorityMap;

	VectorMA(vEyePosition, 15000.0f, vForward, vEnd);

	lookAtEnt = Player_UpdateLookAtEntityTrace(&trace, vEyePosition, vEnd, ent->s.number,
	            CONTENTS_SOLID | CONTENTS_SKY | CONTENTS_CLIPSHOT | CONTENTS_UNKNOWN | CONTENTS_BODY | CONTENTS_TRANSLUCENT,
	            priorityMap, vForward);

	if ( !lookAtEnt )
		return;

	if ( lookAtEnt->classname == scr_const.trigger_lookat )
	{
		ent->client->pLookatEnt = lookAtEnt;
		G_Trigger(lookAtEnt, ent);

		lookAtEnt = Player_UpdateLookAtEntityTrace(&trace, vEyePosition, vEnd, ent->s.number,
		            CONTENTS_SOLID | CONTENTS_SKY | CONTENTS_CLIPSHOT | CONTENTS_UNKNOWN | CONTENTS_BODY,
		            priorityMap, vForward);

		if ( !lookAtEnt )
			return;
	}

	if ( lookAtEnt->s.eType == ET_PLAYER )
	{
		if ( trace.surfaceFlags & SURF_NOIMPACT )
			return;

		VectorSubtract(lookAtEnt->r.currentOrigin, vEyePosition, vDelta);

		if ( lookAtEnt->client->sess.cs.team == ent->client->sess.cs.team && ent->client->sess.cs.team )
		{
			if ( VectorLengthSquared(vDelta) < I_square(g_friendlyNameDist->current.decimal) && !ent->client->pLookatEnt )
				ent->client->pLookatEnt = lookAtEnt;

			// looking at a friend
			if ( VectorLengthSquared(vDelta) < I_square(g_friendlyfireDist->current.decimal) )
				ps->pm_flags |= PMF_LOOKAT_FRIEND;
		}
		else if ( VectorLengthSquared(vDelta) < I_square(weapDef->enemyCrosshairRange) )
		{
			if ( !ent->client->pLookatEnt )
				ent->client->pLookatEnt = lookAtEnt;

			// looking at an enemy
			ps->pm_flags |= PMF_LOOKAT_ENEMY;
		}
	}
}

