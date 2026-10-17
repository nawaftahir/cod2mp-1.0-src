#include "../qcommon/qcommon.h"
#include "g_shared.h"

vec3_t playerMins = { -15.0, -15.0, 0.0 };
vec3_t playerMaxs = { 15.0, 15.0, 70.0 };

void SetClientViewAngle( gentity_t *ent, const vec3_t angle );
void G_GetPlayerViewOrigin( gentity_t *ent, vec3_t origin );
void G_GetPlayerViewDirection( const gentity_t *ent, vec3_t forward, vec3_t right, vec3_t up );
const char *ClientConnect( int clientNum, unsigned short scriptPersId );
void ClientBegin( int clientNum );
void ClientSpawn( gentity_t *ent, const vec3_t spawn_origin, const vec3_t spawn_angles );
void ClientDisconnect( int clientNum );
unsigned int G_GetNonPVSFriendlyInfo( 	gentity_t *pSelf, vec3_t vPosition, int iLastUpdateEnt );
void G_BroadcastVoice( gentity_t *talker, VoicePacket_t *voicePacket );

/*
==================
SetClientViewAngle
==================
*/
void SetClientViewAngle( gentity_t *ent, const vec3_t angle )
{
	int i;
	vec3_t newAngle;
	float fDelta;

	VectorCopy(angle, newAngle);

	// prone and turret angles are handled by their own code
	if ( ent->client->ps.pm_flags & PMF_PRONE && !(ent->client->ps.eFlags & EF_TURRET_ACTIVE) )
	{
		// yaw
		fDelta = AngleDelta(ent->client->ps.proneDirection, newAngle[YAW]);
		fDelta = AngleNormalize180(fDelta);

		if ( fDelta > 45 || fDelta < -45 )
		{
			if ( fDelta > 45 )
				fDelta -= 45;
			else
				fDelta += 45;

			ent->client->ps.delta_angles[YAW] += ANGLE2SHORT(fDelta);

			if ( fDelta > 0 )
				newAngle[YAW] = AngleNormalize360(ent->client->ps.proneDirection - 45);
			else
				newAngle[YAW] = AngleNormalize360(ent->client->ps.proneDirection + 45);
		}

		// pitch
		fDelta = AngleDelta(ent->client->ps.proneTorsoPitch, newAngle[PITCH]);
		fDelta = AngleNormalize180(fDelta);

		if ( fDelta > 45 || fDelta < -15 )
		{
			if ( fDelta > 45 )
				fDelta -= 45;
			else
				fDelta += 15;

			ent->client->ps.delta_angles[PITCH] += ANGLE2SHORT(fDelta);

			if ( fDelta > 0 )
				newAngle[PITCH] = AngleNormalize180(ent->client->ps.proneTorsoPitch - 45);
			else
				newAngle[PITCH] = AngleNormalize180(ent->client->ps.proneTorsoPitch + 15);
		}
	}

	// set the delta angle
	for ( i = 0; i < 3; i++ )
	{
		int shortAngle;

		shortAngle = ANGLE2SHORT(newAngle[i]);
		ent->client->ps.delta_angles[i] = shortAngle - ent->client->sess.cmd.angles[i];
	}

	VectorCopy(newAngle, ent->r.currentAngles);
	VectorCopy(ent->r.currentAngles, ent->client->ps.viewangles);
}

/*
===========
G_GetPlayerViewOrigin
============
*/
void G_GetPlayerViewOrigin( gentity_t *ent, vec3_t origin )
{
	gclient_t *client;
	gentity_t *turret;
	vec3_t vRight;
	float deltaVert;
	float deltaHorz;
	float fBobCycle;
	float xyspeed;

	client = ent->client;
	assert(client);

	if ( client->ps.eFlags & EF_TURRET_ACTIVE )
	{
		turret = &g_entities[client->ps.viewlocked_entNum];

		if ( !G_DObjGetWorldTagPos(turret, scr_const.tag_player, origin) )
		{
			Com_Error(ERR_DROP, "\x15G_GetPlayerViewOrigin: Couldn't find [tag_player] on turret\n");
		}

		return;
	}

	VectorCopy(client->ps.origin, origin);
	origin[2] = origin[2] + client->ps.viewHeightCurrent;

	fBobCycle = BG_GetBobCycle(&client->ps);
	xyspeed = BG_GetSpeed(&client->ps, level.time);

	deltaVert = BG_GetVerticalBobFactor(&client->ps, fBobCycle, xyspeed, bg_bobMax->current.decimal);
	origin[2] = (origin[2] + deltaVert);

	deltaHorz = BG_GetHorizontalBobFactor(&client->ps, fBobCycle, xyspeed, bg_bobMax->current.decimal);
	G_GetPlayerViewDirection(ent, 0, vRight, 0);
	VectorMA(origin, deltaHorz, vRight, origin);

	AddLeanToPosition(origin, client->ps.viewangles[1], client->ps.leanf, 16.0, 20.0);

	if ( origin[2] < client->ps.origin[2] + 8.0f )
	{
		origin[2] = client->ps.origin[2] + 8.0f;
	}
}

/*
==================
G_GetPlayerViewDirection
==================
*/
void G_GetPlayerViewDirection( const gentity_t *ent, vec3_t forward, vec3_t right, vec3_t up )
{
	assert(ent);
	assert(ent->client);
	AngleVectors(ent->client->ps.viewangles, forward, right, up);
}

/*
===========
ClientCheckName
============
*/
static void ClientCleanName( const char *in, char *out, int outSize )
{
	int		len, colorlessLen;
	char	ch;
	char	*p;
	int		spaces;

	//save room for trailing null byte
	outSize--;

	len = 0;
	colorlessLen = 0;
	p = out;
	*p = 0;
	spaces = 0;

	while( 1 )
	{
		ch = *in++;

		if( !ch )
		{
			break;
		}

		// don't allow leading spaces
		if( !*p && ch == ' ' )
		{
			continue;
		}

		// check colors
		if( ch == Q_COLOR_ESCAPE )
		{
			// solo trailing carat is not a color prefix
			if( !*in )
			{
				break;
			}

			// don't allow black in a name, period
			if( (unsigned char)ColorIndex( (unsigned char)*in ) == 0 )
			{
				in++;
				continue;
			}

			// make sure room in dest for both chars
			if( len > outSize - 2 )
			{
				break;
			}

			*out++ = ch;
			*out++ = *in++;
			len += 2;
			continue;
		}

		// don't allow too many consecutive spaces
		if( ch == ' ' )
		{
			spaces++;
			if( spaces > 3 )
			{
				continue;
			}
		}
		else
		{
			spaces = 0;
		}

		if( len > outSize - 1 )
		{
			break;
		}

		*out++ = ch;
		colorlessLen++;
		len++;
	}

	*out = 0;

	// don't allow empty names
	if( *p == 0 || colorlessLen == 0 )
	{
		Q_strncpyz( p, "UnnamedPlayer", outSize );
	}
}

/*
===========
ClientUserInfoChanged

Called from ClientConnect when the player first connects and
directly by the server system when the player updates a userinfo variable.

The game can override any of the settings and call trap_SetUserinfo
if desired.
============
*/
void ClientUserinfoChanged( int clientNum )
{
	gentity_t *ent;
	const char *s;
	char oldname[MAX_STRING_CHARS];
	gclient_t *client;
	char userinfo[MAX_INFO_STRING];
	clientInfo_t *ci;

	ent = &g_entities[clientNum];
	client = ent->client;

	assert(client);
	assert(clientNum >= 0 && clientNum < MAX_CLIENTS);

	SV_GetUserinfo( clientNum, userinfo, sizeof(userinfo) );

	// check for malformed or illegal info strings
	if ( !Info_Validate( userinfo ) )
	{
		strcpy( userinfo, "\\name\\badinfo" );
	}

	client->sess.localClient = SV_IsLocalClient(clientNum);

	s = Info_ValueForKey(userinfo, "cg_predictItems");
	if ( !atoi(s) )
	{
		client->sess.predictItemPickup = qfalse;
	}
	else
	{
		client->sess.predictItemPickup = qtrue;
	}

	// set name
	if ( client->sess.connected == CON_CONNECTED && level.manualNameChange )
	{
		s = Info_ValueForKey( userinfo, "name" );
		ClientCleanName( s, client->sess.newnetname, sizeof(client->sess.newnetname) );
	}
	else
	{
		Q_strncpyz( oldname, client->sess.cs.name, sizeof( oldname ) );
		s = Info_ValueForKey( userinfo, "name" );
		ClientCleanName( s, client->sess.cs.name, sizeof(client->sess.cs.name) );
		Q_strncpyz( client->sess.newnetname, client->sess.cs.name, sizeof(client->sess.cs.name) );
	}

	ci = &level_bgs.clientinfo[clientNum];
	assert(ci->infoValid);

	ci->clientNum = clientNum;
	Q_strncpyz( ci->name, client->sess.cs.name, sizeof(client->sess.cs.name) );
	ci->team = client->sess.cs.team;
}

/*
===========
ClientConnect

Called when a player begins connecting to the server.
Called again for every map change or tournement restart.

The session information will be valid after exit.

Return NULL if the client should be allowed, otherwise return
a string with the reason for denial.

Otherwise, the client will be sent the current gamestate
and will eventually get to ClientBegin.

firstTime will be qtrue the very first time a client connects
to the server machine, but qfalse on map changes and tournement
restarts.
============
*/
const char *ClientConnect( int clientNum, unsigned short scriptPersId )
{
	const char *value;
	gclient_t *client;
	char userinfo[MAX_INFO_STRING];
	gentity_t *ent;
	clientInfo_t *ci;
	XAnimTree_s *pXAnimTree;

	assert(scriptPersId);
	assert(clientNum >= 0 && clientNum < level.maxclients);

	// they can connect
	ent = &g_entities[clientNum];
	client = &level.clients[clientNum];

	memset( client, 0, sizeof( *client ) );

	ci = &level_bgs.clientinfo[clientNum];
	pXAnimTree = ci->pXAnimTree;

	memset( ci, 0, sizeof( *ci ) );

	ci->pXAnimTree = pXAnimTree;

	ci->infoValid = qtrue;
	ci->nextValid = qtrue;

	client->sess.connected = CON_CONNECTING;
	client->sess.scriptPersId = scriptPersId;

	// force into spectator
	client->sess.cs.team = TEAM_SPECTATOR;
	client->sess.sessionState = SESS_STATE_SPECTATOR;

	client->spectatorClient = -1;
	client->sess.forceSpectatorClient = -1;

	G_InitGentity(ent);

	ent->handler = ENT_HANDLER_NULL;
	ent->client = client;

	client->useHoldEntity = ENTITYNUM_NONE;
	client->sess.cs.clientIndex = clientNum;
	client->ps.clientNum = clientNum;

	// get and distribute relevent paramters
	ClientUserinfoChanged(clientNum);
	SV_GetUserinfo(clientNum, userinfo, sizeof(userinfo));

	// we don't check password for bots and local client
	// NOTE: local client <-> "ip" "localhost"
	//   this means this client is not running in our current process
	if ( !client->sess.localClient )
	{
		// check for a password
		value = Info_ValueForKey( userinfo, "password" );
		if ( g_password->current.string[0] && Q_stricmp( g_password->current.string, "none" ) && strcmp( g_password->current.string, value ) != 0 )
		{
			G_FreeEntity(ent);
			return "GAME_INVALIDPASSWORD";
		}
	}

#if LIBCOD_COMPILE_PLAYER == 1 // proxy support
	extern char proxy_realip[MAX_CLIENTS][16];
	char realIP[16];
	strncpy(realIP, Info_ValueForKey(userinfo, "ip"), sizeof(proxy_realip[clientNum]) - 1);

	if (strlen(realIP) != 0)
	{
		strncpy(proxy_realip[clientNum], realIP, sizeof(proxy_realip[clientNum]) - 1);
		Com_DPrintf("realip = %s\n", proxy_realip[clientNum]);
	}
#endif

	// don't do the "xxx connected" messages if they were caried over from previous level
	//		TAT 12/10/2002 - Don't display connected messages in single player
	Scr_PlayerConnect(ent);

	// count current clients and rank for scoreboard
	CalculateRanks();
	assert(client->ps.clientNum == ent->s.number);

	return NULL;
}

/*
===========
ClientBegin

called when a client has finished connecting, and is ready
to be placed into the level.  This will happen every level load,
and on transition between teams, but doesn't happen on respawns
============
*/
void ClientBegin( int clientNum )
{
	gclient_t *client;

	assert(clientNum >= 0 && clientNum < level.maxclients);
	client = &level.clients[clientNum];

	client->sess.connected = CON_CONNECTED;
	client->ps.pm_type = PM_SPECTATOR;

	// count current clients and rank for scoreboard
	CalculateRanks();

	Scr_Notify(&g_entities[clientNum], scr_const.begin, 0);
}

/*
===========
ClientSpawn

Called every time a client is placed fresh in the world:
after the first ClientBegin, and after each respawn
Initializes all non-persistant parts of playerState
============
*/
void ClientSpawn( gentity_t *ent, const vec3_t spawn_origin, const vec3_t spawn_angles )
{
	int index;
	gclient_s *client;
	int iFlags;
	clientSession_t savedSess;
	int savedSpawnCount;

	assert(ent);

	index = ent - g_entities;
	assert(index >= 0 && index < level.maxclients);

	client = ent->client;
	assert(ent->client == &level.clients[index]);
	assert(client);

	assert(ent->r.inuse);

	if ( client->ps.pm_flags & PMF_PLAYER && client->ps.eFlags & EF_TURRET_ACTIVE )
	{
		assert(client->ps.clientNum == ent->s.number);
		assert(client->ps.viewlocked_entNum != ENTITYNUM_NONE);

		G_ClientStopUsingTurret(&level.gentities[client->ps.viewlocked_entNum]);
	}

	G_EntUnlink(ent);

	if ( ent->r.linked )
	{
		SV_UnlinkEntity(ent);
	}

	// clear entity values
	ent->s.groundEntityNum = ENTITYNUM_NONE;
	Scr_SetString(&ent->classname, scr_const.player);

	ent->clipmask = MASK_PLAYERSOLID;
	ent->r.svFlags |= SVF_NOCLIENT;
	ent->takedamage = qfalse;

	G_SetClientContents(ent);

	ent->handler = ENT_HANDLER_CLIENT_SPECTATOR;
	ent->flags = FL_SUPPORTS_LINKTO;

	VectorCopy(playerMins, ent->r.mins);
	VectorCopy(playerMaxs, ent->r.maxs);

	// clear everything but the persistant data
	iFlags = client->ps.eFlags & ( EF_TELEPORT_BIT | EF_VOTED );

	savedSess       = client->sess;
	savedSpawnCount = client->ps.stats[STAT_SPAWN_COUNT];

	memset( client, 0, sizeof( *client ) );
	client->sess    = savedSess;

	client->spectatorClient = -1;
	client->useHoldEntity = ENTITYNUM_NONE;

	// increment the spawncount so the client will detect the respawn
	client->ps.stats[STAT_SPAWN_COUNT] = savedSpawnCount + 1;
	client->ps.stats[STAT_MAX_HEALTH]  = client->sess.maxHealth;

	client->ps.eFlags = iFlags;

	client->sess.cs.clientIndex = index;
	client->ps.clientNum = index;
	client->ps.viewlocked_entNum = ENTITYNUM_NONE;

	assert(client - level.clients >= 0 && client - level.clients < level.maxclients);
	SV_GetUsercmd(client - level.clients, &client->sess.cmd);

	// toggle the teleport bit so the client knows to not lerp
	client->ps.eFlags ^= EF_TELEPORT_BIT;

	// Ridah, setup the bounding boxes and viewheights for prediction
	VectorCopy( ent->r.mins, client->ps.mins );
	VectorCopy( ent->r.maxs, client->ps.maxs );

	client->ps.viewHeightTarget = 60;
	client->ps.viewHeightCurrent = 60;
	client->ps.viewHeightLerpTime = 0;
	client->ps.viewHeightLerpPosAdj = 0;

	G_SetOrigin(ent, spawn_origin);
	VectorCopy(spawn_origin, client->ps.origin);

	// the respawned flag will be cleared after the attack and jump keys come up
	client->ps.pm_flags |= PMF_RESPAWNED;

	SetClientViewAngle(ent, spawn_angles);

	client->inactivityTime = level.time + 1000 * g_inactivity->current.integer;
	client->buttons = client->sess.cmd.buttons;

	level.clientIsSpawning = qtrue;

	client->lastSpawnTime = level.time;
	client->sess.cmd.serverTime = level.time;

	client->ps.commandTime = level.time - 100;

	// run a client frame to drop exactly to the floor,
	// initialize animations and other things
	ClientEndFrame(ent);
	ClientThink_real(ent, &client->sess.cmd);

	level.clientIsSpawning = qfalse;

	BG_PlayerStateToEntityState(&client->ps, &ent->s, qtrue, PMOVE_HANDLER_SERVER);
}

/*
===========
ClientDisconnect

Called when a player drops from the server.
Will not be called between levels.

This should NOT be called directly by any game logic,
call trap_DropClient(), which will call this and do
server system housekeeping.
============
*/
void ClientDisconnect( int clientNum )
{
	gentity_t *ent;
	int i;
	gclient_t *client;

	assert(clientNum >= 0 && clientNum < level.maxclients);
	client = &level.clients[clientNum];
	ent = &g_entities[clientNum];
	assert(ent->r.inuse);

	if ( Scr_IsSystemActive(1) )
	{
		Scr_AddString("disconnect");
		Scr_AddString("-1");
		Scr_Notify(ent, scr_const.menuresponse, 2);
	}

	// stop any following clients
	for ( i = 0; i < level.maxclients; i++ )
	{
		if ( !level.clients[i].sess.connected )
		{
			continue;
		}

		if ( level.clients[i].sess.sessionState == SESS_STATE_SPECTATOR )
		{
			if ( level.clients[i].spectatorClient == clientNum )
			{
				StopFollowing(&g_entities[i]);
			}
		}
	}

	assert(ent->client == client);
	HudElem_ClientDisconnect(ent);

	if ( Scr_IsSystemActive(1) )
	{
		Scr_PlayerDisconnect(ent);
	}

	G_FreeEntity(ent);
	client->sess.connected = CON_DISCONNECTED;

	memset(&client->sess.cs, 0, sizeof(client->sess.cs));

	CalculateRanks();
}

/*
===========
G_GetNonPVSFriendlyInfo
============
*/
unsigned int G_GetNonPVSFriendlyInfo( 	gentity_t *pSelf, vec3_t vPosition, int iLastUpdateEnt )
{
	int iBaseEnt;
	int iEntCount;
	int iCurrentEnt;
	int iEntityNum;
	int iPos[2];
	vec2_t vTemp;
	float fScale[2];
	gentity_t *pEnt;
	int team;

	assert(pSelf);
	team = pSelf->client->sess.cs.team;

	if ( team == TEAM_FREE || team == TEAM_SPECTATOR )
	{
		return 0;
	}

	if ( iLastUpdateEnt == ENTITYNUM_NONE )
		iBaseEnt = 0;
	else
		iBaseEnt = iLastUpdateEnt + 1;

	for ( iEntCount = 0; iEntCount < MAX_CLIENTS; iEntCount++ )
	{
		iCurrentEnt = ( iBaseEnt + iEntCount ) % MAX_CLIENTS;
		pEnt = &g_entities[iCurrentEnt];

		if ( !pEnt->r.inuse )
		{
			continue;
		}

		if ( pEnt->client == NULL )
		{
			continue;
		}

		if ( pEnt->client->sess.sessionState != SESS_STATE_PLAYING )
		{
			continue;
		}

		if ( pEnt->client->sess.cs.team != team )
		{
			continue;
		}

		if ( pSelf == pEnt )
		{
			continue;
		}

		if ( SV_inSnapshot(vPosition, pEnt->s.number) )
		{
			continue;
		}

		iEntityNum = pEnt->s.number;
		Vector2Subtract(pEnt->r.currentOrigin, vPosition, vTemp);

	iPos[0] = (int)(vTemp[0] + 0.5f);
	iPos[1] = (int)(vTemp[1] + 0.5f);

	fScale[0] = 1.0f;
	fScale[1] = 1.0f;

	if ( iPos[0] > 1024 )
	{
		fScale[0] = 1024.0f / iPos[0];
	}
	else if ( iPos[0] < -1022 )
	{
		fScale[0] = -1022.0f / iPos[0];
	}

	if ( iPos[1] > 1024 )
	{
		fScale[1] = 1024.0f / iPos[1];
	}
	else if ( iPos[1] < -1022 )
	{
		fScale[1] = -1022.0f / iPos[1];
	}

	if ( fScale[0] < 1.0f || fScale[1] < 1.0f )
	{
		if ( fScale[0] < fScale[1] )
		{
			iPos[1] = (int)(iPos[1] * fScale[0]);
		}
		else if ( fScale[1] < fScale[0] )
		{
			iPos[0] = (int)(iPos[0] * fScale[1]);
		}
	}

	if ( iPos[0] > 1024 )
	{
		iPos[0] = 1024;
	}
	else if ( iPos[0] < -1022 )
	{
		iPos[0] = -1022;
	}

	if ( iPos[1] > 1024 )
	{
		iPos[1] = 1024;
	}
	else if ( iPos[1] < -1022 )
	{
		iPos[1] = -1022;
	}

		iEntityNum = (iEntityNum & 0xFFFF803F) | ((((iPos[0] + 2) / 4) + 255) & 0x1FF) << 6;
		iEntityNum = (iEntityNum & 0xFF007FFF) | ((((iPos[1] + 2) / 4) + 255) & 0x1FF) << 15;
		iEntityNum = (iEntityNum & 0xFFFFFF) | (((int)(pEnt->r.currentAngles[1] * 0.71111113f) & 0xFF) << 24);

		return iEntityNum;
	}

	return 0;
}

/*
===============
G_BroadcastVoice
===============
*/
void G_BroadcastVoice( gentity_t *talker, VoicePacket_t *voicePacket )
{
	int otherPlayer;
	gentity_t *ent;

	assert(talker);
	talker->client->lastVoiceTime = level.time;

	for ( otherPlayer = 0; otherPlayer < MAX_CLIENTS; otherPlayer++ )
	{
		ent = &g_entities[otherPlayer];

		if ( !ent->r.inuse )
		{
			continue;
		}

		if ( ent->client == NULL )
		{
			continue;
		}

		if ( !voice_global->current.boolean )
		{
			if ( !OnSameTeam(talker, ent) )
			{
				if ( talker->client->sess.cs.team != TEAM_FREE )
				{
					continue;
				}
			}
		}

		if ( ent->client->sess.sessionState != talker->client->sess.sessionState )
		{
			if ( ent->client->sess.sessionState != SESS_STATE_DEAD )
			{
				if ( talker->client->sess.sessionState == SESS_STATE_DEAD )
				{
				}
				else
				{
					continue;
				}
			}

			if ( !voice_deadChat->current.boolean )
			{
				continue;
			}
		}

		if ( talker == ent )
		{
			if ( !voice_localEcho->current.boolean )
			{
				continue;
			}
		}

		if ( SV_ClientHasClientMuted(otherPlayer, talker->s.number) )
		{
			continue;
		}

		if ( SV_ClientWantsVoiceData(otherPlayer) )
		{
			SV_QueueVoicePacket(talker->s.number, otherPlayer, voicePacket);
		}
	}
}

