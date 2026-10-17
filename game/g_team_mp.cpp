#include "../qcommon/qcommon.h"
#include "g_shared.h"

qboolean OnSameTeam( gentity_t *ent1, gentity_t *ent2 );
void CheckTeamStatus();

/*
==============
OnSameTeam
==============
*/
qboolean OnSameTeam( gentity_t *ent1, gentity_t *ent2 )
{
	if ( !ent1->client || !ent2->client )
	{
		return qfalse;
	}
	if ( ent1->client->sess.cs.team == TEAM_FREE )
	{
		return qfalse;
	}

	if ( ent1->client->sess.cs.team == ent2->client->sess.cs.team )
	{
		return qtrue;
	}

	return qfalse;
}

/*
==================
TeamplayLocationsMessage

Format:
	clientNum location health armor weapon powerups

==================
*/
void TeamplayInfoMessage( gentity_t *ent )
{
	int num;
	int health;
	vec3_t vForward;
	vec3_t vStart;
	vec3_t vEnd;
	trace_t trace;
	const float fUnused = 8; // never read
	const float fTraceDist = 8192;

	if ( ent->client->sess.sessionState == SESS_STATE_PLAYING )
	{
		if ( ent->client->sess.cs.team == TEAM_FREE )
		{
			ent->client->ps.stats[STAT_IDENT_CLIENT_NUM] = -1;
			ent->client->ps.stats[STAT_IDENT_CLIENT_HEALTH] = 0;
			return;
		}

		G_GetPlayerViewOrigin(ent, vStart);
		G_GetPlayerViewDirection(ent, vForward, NULL, NULL);

		VectorMA(vStart, fTraceDist, vForward, vEnd);
	}
	else
	{
		G_GetPlayerViewOrigin(ent, vStart);
		G_GetPlayerViewDirection(ent, vForward, NULL, NULL);

		if ( ent->client->ps.viewHeightCurrent < DEAD_VIEWHEIGHT )
		{
			vStart[2] += DEAD_VIEWHEIGHT - ent->client->ps.viewHeightCurrent;
		}

		VectorMA(vStart, fTraceDist, vForward, vEnd);
	}

	G_TraceCapsule(&trace, vStart, vec3_origin, vec3_origin, vEnd, ent->client->ps.clientNum, CONTENTS_SOLID | CONTENTS_BODY);
	num = trace.entityNum;

	if ( num < MAX_CLIENTS && g_entities[num].client && (!G_IsPlaying(ent) || g_entities[num].client->sess.cs.team == ent->client->sess.cs.team) )
	{
		health = g_entities[num].health;
	}
	else
	{
		num = -1;
		health = 0;
	}

	ent->client->ps.stats[STAT_IDENT_CLIENT_NUM] = num;
	ent->client->ps.stats[STAT_IDENT_CLIENT_HEALTH] = health;
}

/*
==============
CheckTeamStatus
==============
*/
void CheckTeamStatus()
{
	int i;
	gentity_t *ent;

	if ( level.time - level.lastTeammateHealthTime > 0 )
	{
		level.lastTeammateHealthTime = level.time;

		for ( i = 0; i < g_maxclients->current.integer; i++ )
		{
			ent = &g_entities[i];

			if ( ent->r.inuse )
			{
				if ( !(ent->client->ps.pm_flags & PMF_FOLLOW) )
				{
					TeamplayInfoMessage(ent);
				}
			}
		}
	}
}

