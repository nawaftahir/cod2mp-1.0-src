#include "../qcommon/qcommon.h"
#include "g_shared.h"

struct pushed_t
{
	gentity_s *ent;
	vec3_t origin;
	vec3_t angles;
	float deltayaw;
};

pushed_t pushed[MAX_GENTITIES];
static pushed_t *pushed_p;

const char *hintStrings[] =
{
	"",
	"HINT_NONE",
	"HINT_ACTIVATE",
	"HINT_HEALTH",
	"HINT_FRIENDLY",
	NULL
};

void trigger_use( gentity_t *ent );

gentity_t *G_TestEntityPosition( gentity_t *ent, vec3_t org );
void G_CreateRotationMatrix( const vec3_t angles, vec3_t matrix[3] );
void G_TransposeMatrix( const vec3_t matrix[3], vec3_t transpose[3] );
void G_RotatePoint( vec3_t point, const vec3_t matrix[3] );
void G_RunMover( gentity_t *ent );
void use_trigger_use( gentity_t *ent, gentity_t *other, gentity_t *activator );
void trigger_use( gentity_t *ent );
void trigger_use_touch( gentity_t *ent );

/*
============
G_TestEntityPosition
============
*/
gentity_t *G_TestEntityPosition( gentity_t *ent, vec3_t org )
{
	trace_t tr;
	int mask;

	if ( ent->clipmask )
	{
//		if ( ent->r.contents == CONTENTS_CORPSE && ent->health <= 0 ) {	// Arnout: players waiting to be revived are important
		if ( ent->r.contents & CONTENTS_CORPSE )
		{
			// corpse aren't important
			//G_Damage( ent, NULL, NULL, NULL, NULL, 99999, 0, MOD_CRUSH );
			return NULL;
		}
		mask = ent->clipmask;
	}
	else
	{
		mask = MASK_SOLID;
	}

	if ( ent->s.eType == ET_MISSILE )
	{
		G_TraceCapsule(&tr, org, ent->r.mins, ent->r.maxs, org, ent->r.ownerNum, mask);
	}
	else
	{
		G_TraceCapsule(&tr, org, ent->r.mins, ent->r.maxs, org, ent->s.number, mask);
	}

	if ( tr.startsolid || tr.allsolid )
	{
		return &g_entities[ tr.entityNum ];
	}

	return NULL;
}

/*
==================
G_CreateRotationMatrix
==================
*/
void G_CreateRotationMatrix( const vec3_t angles, vec3_t matrix[3] )
{
	AngleVectors( angles, matrix[0], matrix[1], matrix[2] );
	VectorInverse( matrix[1] );
}

/*
==================
G_TransposeMatrix
==================
*/
void G_TransposeMatrix( const vec3_t matrix[3], vec3_t transpose[3] )
{
	int i, j;
	for ( i = 0; i < 3; i++ )
	{
		for ( j = 0; j < 3; j++ )
		{
			transpose[i][j] = matrix[j][i];
		}
	}
}

/*
==================
G_RotatePoint
==================
*/
void G_RotatePoint( vec3_t point, const vec3_t matrix[3] )
{
	vec3_t tvec;

	VectorCopy( point, tvec );
	point[0] = DotProduct( matrix[0], tvec );
	point[1] = DotProduct( matrix[1], tvec );
	point[2] = DotProduct( matrix[2], tvec );
}

qboolean G_TryPushingEntity( gentity_t *check, gentity_t *pusher, vec3_t move, vec3_t amove )
{
	vec3_t amove2, org2, move2;
	gentity_t *block;
	vec3_t matrix[3], transpose[3];
	float x, fx, y, fy, z, fz;
	vec3_t org;
#define JITTER_INC  4
#define JITTER_MAX  ( check->r.maxs[0] / 2.0 )

	// figure movement due to the pusher's amove
	VectorAdd(check->r.currentOrigin, move, org);
	G_CreateRotationMatrix(amove, transpose);
	G_TransposeMatrix(transpose, matrix);
	VectorSubtract(org, pusher->r.currentOrigin, amove2);
	VectorCopy(amove2, org2);
	G_RotatePoint(org2, matrix);
	VectorSubtract(org2, amove2, move2);
	VectorAdd(org, move2, org);

	block = G_TestEntityPosition(check, org);

	if ( !block )
	{
		// pushed ok
		if ( check->s.groundEntityNum != pusher->s.number )
			check->s.groundEntityNum = ENTITYNUM_NONE;

		// try moving the contacted entity
		VectorCopy(org, check->r.currentOrigin);
		VectorCopy(org, check->s.pos.trBase);

		if ( check->client )
		{
			// make sure the client's view rotates when on a rotating mover
			// RF, this is done client-side now
			// ydnar: only do this if player is prone or using set mortar
			check->client->ps.delta_angles[1] += (int)(amove[YAW] * 182.04445f) & 65535;
			VectorCopy(org, check->client->ps.origin);
		}

		++pushed_p;
		return qtrue;
	}

	// RF, if still not valid, move them around to see if we can find a good spot
	if ( JITTER_MAX > JITTER_INC )
	{
		VectorCopy(org, amove2);

		for ( z = 0; z < JITTER_MAX; z += JITTER_INC )
			for ( fz = -z; fz <= z; fz += 2 * z )
			{
				for ( x = JITTER_INC; x < JITTER_MAX; x += JITTER_INC )
					for ( fx = -x; fx <= x; fx += 2 * x )
					{
						for ( y = JITTER_INC; y < JITTER_MAX; y += JITTER_INC )
							for ( fy = -y; fy <= y; fy += 2 * y )
							{
								VectorSet(move2, fx, fy, fz);
								VectorAdd(amove2, move2, org2);

								//
								// do the test
								block = G_TestEntityPosition( check, org2 );
								if ( !block )
								{
									// pushed ok
									if ( check->s.groundEntityNum != pusher->s.number )
										check->s.groundEntityNum = ENTITYNUM_NONE;

									VectorCopy(org2, check->r.currentOrigin);
									VectorCopy(org2, check->s.pos.trBase);

									if ( check->client )
									{
										check->client->ps.delta_angles[1] += (int)(amove[YAW] * 182.04445f) & 65535;
										VectorCopy(org2, check->client->ps.origin);
									}

									++pushed_p;
									return qtrue;
								}
							}
					}
				if ( !fz )
				{
					break;
				}
			}
	}

	// if it is ok to leave in the old position, do it
	// this is only relevent for riding entities, not pushed
	// Sliding trapdoors can cause this.
	block = G_TestEntityPosition( check, check->r.currentOrigin );

	if ( !block )
	{
		check->s.groundEntityNum = ENTITYNUM_NONE;
		return 1;
	}

	// blocked
	return qfalse;
}

bool G_MoverPush( gentity_t *pusher, vec3_t move, vec3_t amove, gentity_t **obstacle )
{
	int i, e;
	gentity_t *check;
	vec3_t mins, maxs;
	int entityList[MAX_GENTITIES];
	int moveList[MAX_GENTITIES];
	int listedEntities, moveEntities;
	vec3_t totalMins, totalMaxs;
	float radius;
	bool moved;

	*obstacle = NULL;
	moved = true;

	// mins/maxs are the bounds at the destination
	// totalMins / totalMaxs are the bounds for the entire move
	if ( pusher->r.currentAngles[0] || pusher->r.currentAngles[1] || pusher->r.currentAngles[2]
	        || amove[0] || amove[1] || amove[2] )
	{
		radius = RadiusFromBounds( pusher->r.mins, pusher->r.maxs );
		for ( i = 0; i < 3; i++ )
		{
			mins[i] = pusher->r.currentOrigin[i] - radius + move[i];
			maxs[i] = pusher->r.currentOrigin[i] + radius + move[i];
			totalMins[i] = pusher->r.currentOrigin[i] - radius;
			totalMaxs[i] = pusher->r.currentOrigin[i] + radius;
		}
	}
	else
	{
		for ( i = 0; i < 3; i++ )
		{
			mins[i] = pusher->r.absmin[i] + move[i];
			maxs[i] = pusher->r.absmax[i] + move[i];
		}

		VectorCopy( pusher->r.absmin, totalMins );
		VectorCopy( pusher->r.absmax, totalMaxs );
	}

	for ( i = 0; i < 3; i++ )
	{
		if ( move[i] > 0 )
		{
			totalMaxs[i] += move[i];
		}
		else
		{
			totalMins[i] += move[i];
		}
	}

	// unlink the pusher so we don't get it in the entityList
	SV_UnlinkEntity( pusher );

	listedEntities = CM_AreaEntities( totalMins, totalMaxs, entityList, MAX_GENTITIES, CONTENTS_MISSILECLIP | CONTENTS_UNKNOWNCLIP | CONTENTS_BODY );

	// move the pusher to its final position
	VectorAdd( pusher->r.currentOrigin, move, pusher->r.currentOrigin );
	VectorAdd( pusher->r.currentAngles, amove, pusher->r.currentAngles );
	SV_LinkEntity( pusher );

	moveEntities = 0;

	// see if any solid entities are inside the final position
	for ( e = 0; e < listedEntities; e++ )
	{
		check = &g_entities[ entityList[e] ];

		// only push items and players
		if ( check->s.eType != ET_MISSILE && check->s.eType != ET_ITEM && check->s.eType != ET_PLAYER && !check->physicsObject )
		{
			continue;
		}

		// if the entity is standing on the pusher, it will definitely be moved
		if ( check->s.groundEntityNum != pusher->s.number )
		{
			// see if the ent needs to be tested
			if ( check->r.absmin[0] >= maxs[0]
			        || check->r.absmin[1] >= maxs[1]
			        || check->r.absmin[2] >= maxs[2]
			        || check->r.absmax[0] <= mins[0]
			        || check->r.absmax[1] <= mins[1]
			        || check->r.absmax[2] <= mins[2] )
			{
				continue;
			}

			// see if the ent's bbox is inside the pusher's final position
			if ( G_TestEntityPosition( check, check->r.currentOrigin ) != pusher )
			{
				continue;
			}
		}

		moveList[moveEntities++] = entityList[e];
	}

	// unlink all to be moved entities so they cannot get stuck in each other
	for ( e = 0; e < moveEntities; e++ )
	{
		check = &g_entities[ moveList[e] ];
		SV_UnlinkEntity( check );
	}

	for ( e = 0; e < moveEntities; e++ )
	{
		check = &g_entities[ moveList[e] ];

		// the entity needs to be pushed
		pushed_p->ent = check;
		VectorCopy( check->r.currentOrigin, pushed_p->origin );
		pushed_p->deltayaw = amove[YAW];

		if ( G_TryPushingEntity( check, pusher, move, amove ) )
		{
			// link it in now so nothing else tries to clip into us
			SV_LinkEntity( check );
			continue;
		}

		if ( check->s.eType == ET_ITEM )
		{
			SV_LinkEntity( check );
			continue;
		}

		// bobbing entities are instant-kill and never get blocked
		if ( pusher->s.pos.trType == TR_SINE || pusher->s.apos.trType == TR_SINE )
		{
			G_Damage( check, pusher, pusher, NULL, NULL, 99999, 0, MOD_CRUSH, HITLOC_NONE, 0 );
			continue;
		}

		// save off the obstacle so we can call the block function (crush, etc)
		*obstacle = check;

		// movement failed
		moved = false;
	}

	// link all entities at their final position
	for ( e = 0; e < moveEntities; e++ )
	{
		check = &g_entities[ moveList[e] ];
		SV_LinkEntity( check );
	}

	// movement was successful
	return moved;
}

void G_MoverTeam( gentity_t *ent )
{
	vec3_t move;
	vec3_t amove;
	gentity_t *obstacle;
	vec3_t origin;
	vec3_t angles;
	gentity_t *part;
	pushed_t *push;
	void (*blocked)(gentity_t *, gentity_t *);
	void (*reached)(gentity_t *);

	obstacle = NULL;

	// make sure all team slaves can move before commiting
	// any moves or calling any think functions
	// if the move is blocked, all moved objects will be backed out
	pushed_p = pushed;

	// get current position
	BG_EvaluateTrajectory( &ent->s.pos, level.time, origin );
	BG_EvaluateTrajectory( &ent->s.apos, level.time, angles );
	VectorSubtract( origin, ent->r.currentOrigin, move );
	VectorSubtract( angles, ent->r.currentAngles, amove );

	if ( !G_MoverPush(ent, move, amove, &obstacle) )
	{
		for ( push = pushed_p - 1; push >= pushed; --push )
		{
			part = push->ent;

			VectorCopy(push->origin, part->r.currentOrigin);
			VectorCopy(push->origin, part->s.pos.trBase);

			if ( part->client )
			{
				part->client->ps.delta_angles[YAW] -= ANGLE2SHORT(push->deltayaw);
				VectorCopy(push->origin, part->client->ps.origin);
			}

			SV_LinkEntity(part);
		}

		ent->s.pos.trTime += level.time - level.previousTime;
		ent->s.apos.trTime += level.time - level.previousTime;

		BG_EvaluateTrajectory(&ent->s.pos, level.time, ent->r.currentOrigin);
		BG_EvaluateTrajectory(&ent->s.apos, level.time, ent->r.currentAngles);

		SV_LinkEntity(ent);

		blocked = entityHandlers[ent->handler].blocked;

		// if the pusher has a "blocked" function, call it
		if ( blocked )
		{
			blocked(ent, obstacle);
		}
	}
	else
	{
		if ( ent->s.pos.trType != TR_STATIONARY )
		{
			if ( level.time >= ent->s.pos.trTime + ent->s.pos.trDuration )
			{
				reached = entityHandlers[ent->handler].reached;

				if ( reached )
				{
					reached(ent);
				}
			}
		}

		if ( ent->s.apos.trType != TR_STATIONARY )
		{
			if ( level.time >= ent->s.apos.trTime + ent->s.apos.trDuration )
			{
				reached = entityHandlers[ent->handler].reached;

				if ( reached )
				{
					reached(ent);
				}
			}
		}
	}
}

/*
================
G_RunMover
================
*/
void G_RunMover( gentity_t *ent )
{
	// if not a team captain, don't do anything, because
	// the captain will handle everything

	if ( ent->tagInfo )
	{
		G_GeneralLink(ent);
	}
	else if ( ent->s.pos.trType != TR_STATIONARY || ent->s.apos.trType != TR_STATIONARY )
	{
		G_MoverTeam(ent);
	}

	// check think function
	G_RunThink( ent );
}

/*
==================
use_trigger_use
==================
*/
void use_trigger_use( gentity_t *ent, gentity_t *other, gentity_t *activator )
{
	;
}

void trigger_use_shared( gentity_t *self )
{
	int i;
	const char *cursorhint;
	char szConfigString[MAX_STRING_CHARS];

	SV_SetBrushModel(self);
	SV_LinkEntity(self);

	self->trigger.singleUserEntIndex = ENTITYNUM_NONE;
	self->s.pos.trType = TR_STATIONARY;

	VectorCopy(self->r.currentOrigin, self->s.pos.trBase);

	self->r.contents = CONTENTS_DONOTENTER;
	self->r.svFlags = SVF_NOCLIENT;
	self->handler = ENT_HANDLER_TRIGGER_USE;
	self->s.hintType = HINT_ACTIVATE;

	if ( G_SpawnString("cursorhint", "", &cursorhint) )
	{
		if ( !I_stricmp(cursorhint, "HINT_INHERIT") )
		{
			self->s.hintType = -1;
		}
		else
		{
			for ( i = 1; i < ARRAY_COUNT(hintStrings); i++ )
			{
				if ( !I_stricmp(cursorhint, hintStrings[i]) )
				{
					self->s.hintType = i;
					break;
				}
			}
		}
	}

	self->s.hintString = 255;

	if ( G_SpawnString("hintstring", "", &cursorhint) )
	{
		for ( i = 0; i < MAX_HINTSTRINGS; i++ )
		{
			SV_GetConfigstring(i + CS_HINTSTRINGS, szConfigString, sizeof(szConfigString));

			if ( !szConfigString[0] )
			{
				SV_SetConfigstring(i + CS_HINTSTRINGS, cursorhint);
				self->s.hintString = (byte)i;
				break;
			}

			if ( !strcmp(cursorhint, szConfigString) )
			{
				self->s.hintString = (byte)i;
				break;
			}
		}

		if ( i == MAX_HINTSTRINGS )
		{
			Com_Error(ERR_DROP, "\x15" "Too many different hintstring key values on trigger_use entities. Max allowed is %i", MAX_HINTSTRINGS);
		}
	}
}

/*
==================
trigger_use
==================
*/
void trigger_use( gentity_t *ent )
{
	trigger_use_shared( ent );
}

/*
==================
trigger_use_touch
==================
*/
void trigger_use_touch( gentity_t *ent )
{
	trigger_use_shared( ent );
}

