#include "../qcommon/qcommon.h"

static void SV_MapRestart_f( void );

static client_t *SV_GetPlayerByName( void );
static client_t *SV_GetPlayerByNum( void );
const char *SV_GetMapBaseName( const char *mapname );
static void SV_Map_f( void );
static void SV_MapRestart( qboolean fast_restart );
static void SV_MapRestart_f( void );
static void SV_FastRestart_f( void );
const char *UI_GetMapRotationToken();
static void SV_MapRotate_f( void );
static int SV_KickClient( client_t *cl, char *playerName, int maxPlayerNameLen );
static int SV_KickUser_f( char *playerName, int maxPlayerNameLen );
static int SV_KickClient_f( char *playerName, int maxPlayerNameLen );
static void SV_TempBan_f( void );
static void SV_Ban_f( void );
static void SV_BanNum_f( void );
static void SV_Unban_f( void );
static void SV_Drop_f( void );
static void SV_DropNum_f( void );
static void SV_TempBanNum_f( void );
static void SV_Status_f( void );
static void SV_ConSay_f( void );
static void SV_ConTell_f( void );
void SV_Heartbeat_f( void );
static void SV_Serverinfo_f( void );
static void SV_Systeminfo_f( void );
static void SV_DumpUser_f( void );
static void SV_GameCompleteStatus_f( void );
static void SV_ScriptUsage_f( void );
static void SV_StringUsage_f( void );
void SV_AddOperatorCommands( void );
void SV_RemoveOperatorCommands( void );
void SV_AddDedicatedCommands( void );
void SV_RemoveDedicatedCommands( void );

/*
==================
SV_GetPlayerByName

Returns the player with name from SV_Cmd_Argv(1)
==================
*/
static client_t *SV_GetPlayerByName( void )
{
	client_t *cl;
	int i;
	const char *s;
	char cleanName[64];

	// make sure server is running
	if ( !com_sv_running->current.boolean )
	{
		return NULL;
	}

	if ( SV_Cmd_Argc() < 2 )
	{
		Com_Printf( "No player specified.\n" );
		return NULL;
	}

	s = SV_Cmd_Argv( 1 );

	// check for a name match
	for ( i = 0, cl = svs.clients ; i < sv_maxclients->current.integer ; i++,cl++ )
	{
		if ( !cl->state )
		{
			continue;
		}
		if ( !Q_stricmp( cl->name, s ) )
		{
			return cl;
		}

		Q_strncpyz( cleanName, cl->name, sizeof( cleanName ) );
		Q_CleanStr( cleanName );
		if ( !Q_stricmp( cleanName, s ) )
		{
			return cl;
		}
	}

	Com_Printf( "Player %s is not on the server\n", s );

	return NULL;
}

/*
==================
SV_GetPlayerByNum

Returns the player with idnum from SV_Cmd_Argv(1)
==================
*/
static client_t *SV_GetPlayerByNum( void )
{
	client_t *cl;
	int i;
	int idnum;
	const char *s;

	// make sure server is running
	if ( !com_sv_running->current.boolean )
	{
		return NULL;
	}

	if ( SV_Cmd_Argc() < 2 )
	{
		Com_Printf("No player specified.\n" );
		return NULL;
	}

	s = SV_Cmd_Argv(1);

	for (i = 0; s[i]; i++)
	{
		if (s[i] < '0' || s[i] > '9')
		{
			Com_Printf("Bad slot number: %s\n", s);
			return NULL;
		}
	}
	idnum = atoi( s );
	if ( idnum < 0 || idnum >= sv_maxclients->current.integer )
	{
		Com_Printf("Bad client slot: %i\n", idnum );
		return NULL;
	}

	cl = &svs.clients[idnum];
	if ( !cl->state )
	{
		Com_Printf("Client %i is not active\n", idnum );
		return NULL;
	}
	return cl;
}

/*
==================
SV_GetMapBaseName
==================
*/
const char *SV_GetMapBaseName( const char *mapname )
{
	return FS_GetMapBaseName(mapname);
}

/*
==================
SV_Map_f

Restart the server on a different map
==================
*/
static void SV_Map_f( void )
{
	const char *map;
	const char *cmd;
	const char *basename;
	const char *expanded;
	char mapname[MAX_QPATH];
	bool cheats;

	map = SV_Cmd_Argv(1);
	if ( !map[0] )
		return;

	if ( !Com_HasPlayerProfile() )
	{
		Com_Printf("Can't load a map without a player profile selected.\n");
		return;
	}

	basename = SV_GetMapBaseName(map);
	I_strncpyz(mapname, basename, sizeof(mapname));
	I_strlwr(mapname);

	expanded = va("maps/mp/%s.%s", mapname, GetBspExtension());
	if ( FS_ReadFileInl(expanded, NULL) == -1 )
	{
		Com_Printf("Can't find map %s\n", expanded);
		return;
	}

	cmd = SV_Cmd_Argv(0);
	cheats = I_stricmp(cmd, "devmap") == 0;
	FS_ConvertPath(mapname);
	SV_SpawnServer(mapname);
	Dvar_SetBool(sv_cheats, cheats);
}

/*
================
SV_MapRestart

Completely restarts a level, but doesn't send a new gamestate to the clients.
This allows fair starts with variable load times.
================
*/
static void SV_MapRestart( qboolean fast_restart )
{
	int i;
	client_t *cl;
	const char *dropreason;
	char mapname[MAX_QPATH];
	int savepersist;
	char restartCmd;

	// make sure server is running
	if ( !com_sv_running->current.boolean )
	{
		Com_Printf( "Server is not running.\n" );
		return;
	}

	SV_SetGametype();
	I_strncpyz(sv.gametype, sv_gametype->current.string, sizeof(sv.gametype));

	savepersist = G_GetSavePersist();

	// check for changes in variables that can't just be restarted
	if ( sv_maxclients->modified || strcasecmp(sv.gametype, sv_gametype->current.string) || !fast_restart )
	{
		G_SetSavePersist(qfalse);
		I_strncpyz(mapname, Dvar_GetString("mapname"), sizeof(mapname));
		FS_ConvertPathInl(mapname);
		SV_SpawnServer(mapname);
		return;
	}

	// make sure we aren't restarting twice in the same frame
	if ( com_frameTime == sv.start_frameTime )
		return;

	Dvar_ResetScriptInfo();
	SV_InitArchivedSnapshot();

	// toggle the server bit so clients can detect that a
	// map_restart has happened
	svs.snapFlagServerBit ^= SNAPFLAG_SERVERCOUNT;

	// generate a new serverid
	// TTimo - don't update restartedserverId there, otherwise we won't deal correctly with multiple map_restart
	sv_serverId_value = (sv_serverId_value & 0xF0) + ((sv_serverId_value + 1) & 0xF);
	Dvar_SetInt(sv_serverid, sv_serverId_value);

	sv.start_frameTime = com_frameTime;

	// reset all the vm data in place without changing memory allocation
	// note that we do NOT set sv.state = SS_LOADING, so configstrings that
	// had been changed from their default values will generate broadcast updates
	sv.state = SS_LOADING;
	sv.restarting = qtrue;

	SV_RestartGameProgs(savepersist);

	// run a few frames to allow everything to settle
	for ( i = 0; i < GAME_INIT_FRAMES; i++ )
	{
		svs.time += FRAMETIME;
		SV_RunFrame();
	}

	// connect and begin all the clients
	for ( i = 0; i < sv_maxclients->current.integer; i++ )
	{
		cl = &svs.clients[i];

		// send the new gamestate to all connected clients
		if ( cl->state < CS_CONNECTED )
		{
			continue;
		}

		// add the map_restart command
		restartCmd = savepersist ? 110 : 66;
		SV_AddServerCommand(cl, SV_CMD_RELIABLE, va("%c", restartCmd));

		// connect the client again, without the firstTime flag
		dropreason = ClientConnect(i, cl->scriptId);

		if ( dropreason )
		{
			// this generally shouldn't happen, because the client
			// was connected before the level change
			SV_DropClient(cl, dropreason);
			Com_Printf("SV_MapRestart_f: dropped client %i - denied!\n", i);
			continue;
		}

		if ( cl->state != CS_ACTIVE )
		{
			continue;
		}

		SV_ClientEnterWorld(cl, &cl->lastUsercmd);
	}

	sv.state = SS_GAME;
	sv.restarting = qfalse;
}

/*
================
SV_MapRestart_f
================
*/
static void SV_MapRestart_f( void )
{
	SV_MapRestart(qfalse);
}

/*
================
SV_FastRestart_f
================
*/
static void SV_FastRestart_f( void )
{
	SV_MapRestart(qtrue);
}

/*
================
UI_GetMapRotationToken
================
*/
const char *UI_GetMapRotationToken()
{
	const char *value;
	const char *token;

	value = sv_mapRotationCurrent->current.string;
	token = Com_Parse(&value);

	if ( !value )
	{
		Dvar_SetString(sv_mapRotationCurrent, "");
		return NULL;
	}

	Dvar_SetString(sv_mapRotationCurrent, value);
	return token;
}

/*
================
SV_MapRotate_f
================
*/
static void SV_MapRotate_f( void )
{
	const char *token;

	// DHM - Nerve :: Check for invalid gametype
	Com_Printf("map_rotate...\n\n");
	Com_Printf("\"sv_mapRotation\" is:\"%s\"\n\n", sv_mapRotation->current.string);
	Com_Printf("\"sv_mapRotationCurrent\" is:\"%s\"\n\n", sv_mapRotationCurrent->current.string);

	if ( !sv_mapRotationCurrent->current.string[0] )
		Dvar_SetString(sv_mapRotationCurrent, sv_mapRotation->current.string);

	token = UI_GetMapRotationToken();

	if ( !token )
	{
		Dvar_SetString(sv_mapRotationCurrent, sv_mapRotation->current.string);
		token = UI_GetMapRotationToken();
	}

	for ( ;; )
	{
		if ( !token )
		{
			Com_Printf("No map specified in sv_mapRotation - forcing map_restart.\n");
			SV_FastRestart_f();
			return;
		}

		if ( !stricmp(token, "gametype") )
		{
			token = UI_GetMapRotationToken();

			if ( !token )
			{
				Com_Printf("No gametype specified after 'gametype' keyword in sv_mapRotation - forcing map_restart.\n");
				SV_FastRestart_f();
				return;
			}

			Com_Printf("Setting g_gametype: %s.\n", token);

			if ( com_sv_running->current.boolean && stricmp(sv_gametype->current.string, token) )
			{
				G_SetSavePersist(0);
			}

			Dvar_SetString(sv_gametype, token);
		}
		else if ( !stricmp(token, "map") )
		{
			token = UI_GetMapRotationToken();

			if ( !token )
			{
				Com_Printf("No map specified after 'map' keyword in sv_mapRotation - forcing map_restart.\n");
				SV_FastRestart_f();
				return;
			}

			Com_Printf("Setting map: %s.\n", token);
			Cbuf_ExecuteText(EXEC_NOW, va("map %s\n", token));
			return;
		}
		else
		{
			Com_Printf("Unknown keyword '%s' in sv_mapRotation.\n", token);
		}

		token = UI_GetMapRotationToken();
	}
}

/*
==================
SV_KickClient
==================
*/
static int SV_KickClient( client_t *cl, char *playerName, int maxPlayerNameLen )
{
	int guid;

	assert(cl);

	if ( cl->netchan.remoteAddress.type == NA_LOOPBACK )
	{
		SV_SendServerCommand(NULL, SV_CMD_CAN_IGNORE, "%c \"EXE_CANNOTKICKHOSTPLAYER\"", 101);
		return 0;
	}

	if ( playerName )
	{
		I_strncpyz(playerName, cl->name, maxPlayerNameLen);
		I_CleanStr(playerName);
	}

	guid = cl->guid;
	SV_DropClient(cl, "EXE_PLAYERKICKED"); // JPW NERVE to match front menu message
	cl->lastPacketTime = svs.time; // in case there is a funny zombie

	return guid;
}

/*
==================
SV_KickUser_f

Kick a user off of the server
==================
*/
static int SV_KickUser_f( char *playerName, int maxPlayerNameLen )
{
	const char *cmd;
	client_t *cl;
	int i;

	if ( !com_sv_running->current.boolean )
	{
		Com_Printf("Server is not running.\n");
		return 0;
	}

	if ( SV_Cmd_Argc() != 2 )
	{
		cmd = SV_Cmd_Argv(0);
		Com_Printf("Usage: %s <player name>\n%s all = kick everyone\n", cmd, cmd);
		return 0;
	}

	cl = SV_GetPlayerByName();
	if ( !cl )
	{
		if ( !I_stricmp(SV_Cmd_Argv(1), "all") )
		{
			for ( i = 0, cl = svs.clients; i < sv_maxclients->current.integer; i++, cl++ )
			{
				if ( !cl->state )
					continue;

				SV_KickClient(cl, NULL, 0);
			}
		}

		return 0;
	}

	return SV_KickClient(cl, playerName, maxPlayerNameLen);
}

/*
==================
SV_KickClient_f
==================
*/
static int SV_KickClient_f( char *playerName, int maxPlayerNameLen )
{
	client_t *cl;

	if ( !com_sv_running->current.boolean )
	{
		Com_Printf("Server is not running.\n");
		return 0;
	}

	if ( SV_Cmd_Argc() != 2 )
	{
		Com_Printf("Usage: %s <client number>\n", SV_Cmd_Argv(0));
		return 0;
	}

	cl = SV_GetPlayerByNum();
	if ( !cl )
		return 0;

	return SV_KickClient(cl, playerName, maxPlayerNameLen);
}

/*
==================
SV_TempBan_f
==================
*/
static void SV_TempBan_f( void )
{
	int guid;
	char name[64];

	guid = SV_KickUser_f(name, sizeof(name));

	if ( guid )
	{
		Com_Printf("%s (guid %i) was kicked for cheating\n", name, guid);
		SV_BanGuidBriefly(guid);
	}
}

/*
==================
SV_Ban_f

Ban a user from being able to play on this server through the auth
server
==================
*/
static void SV_Ban_f( void )
{
	client_t *cl;

	if ( !com_sv_running->current.boolean )
	{
		Com_Printf("Server is not running.\n");
		return;
	}

	if ( SV_Cmd_Argc() != 2 )
	{
		Com_Printf("Usage: banUser <player name>\n");
		return;
	}

	cl = SV_GetPlayerByName();
	if ( !cl )
		return;

	SV_BanClient(cl);
}

/*
==================
SV_BanNum_f
==================
*/
static void SV_BanNum_f( void )
{
	client_t *cl;

	if ( !com_sv_running->current.boolean )
	{
		Com_Printf("Server is not running.\n");
		return;
	}

	if ( SV_Cmd_Argc() != 2 )
	{
		Com_Printf("Usage: banClient <client number>\n");
		return;
	}

	cl = SV_GetPlayerByNum();
	if ( !cl )
		return;

	SV_BanClient(cl);
}

/*
==================
SV_Unban_f
==================
*/
static void SV_Unban_f( void )
{
	if ( SV_Cmd_Argc() != 2 )
	{
		Com_Printf("Usage: unban <client name>\n");
		return;
	}

	SV_UnbanClient(SV_Cmd_Argv(1));
}

/*
==================
SV_Drop_f
==================
*/
static void SV_Drop_f( void )
{
	SV_KickUser_f(NULL, 0);
}

/*
==================
SV_DropNum_f
==================
*/
static void SV_DropNum_f( void )
{
	SV_KickClient_f(NULL, 0);
}

/*
==================
SV_TempBanNum_f
==================
*/
static void SV_TempBanNum_f( void )
{
	int guid;
	char name[64];

	guid = SV_KickClient_f(name, sizeof(name));

	if ( guid )
	{
		Com_Printf("%s (guid %i) was kicked for cheating\n", name, guid);
		SV_BanGuidBriefly(guid);
	}
}

/*
================
SV_Status_f
================
*/
static void SV_Status_f( void )
{
	int i, j, l;
	client_t    *cl;
	playerState_t *ps;
	const char      *s;
	int ping;

	// make sure server is running
	if ( !com_sv_running->current.boolean )
	{
		Com_Printf( "Server is not running.\n" );
		return;
	}

	Com_Printf( "map: %s\n", sv_mapname->current.string );

	Com_Printf( "num score ping guid   name            lastmsg address               qport rate\n" );
	Com_Printf( "--- ----- ---- ------ --------------- ------- --------------------- ----- -----\n" );
	for ( i = 0,cl = svs.clients ; i < sv_maxclients->current.integer ; i++,cl++ )
	{
		if ( !cl->state )
		{
			continue;
		}
		Com_Printf( "%3i ", i );
		ps = SV_GameClientNum(i);
		Com_Printf( "%5i ", G_GetClientScore(cl - svs.clients) );

		if ( cl->state == CS_CONNECTED )
		{
			Com_Printf( "CNCT " );
		}
		else if ( cl->state == CS_ZOMBIE )
		{
			Com_Printf( "ZMBI " );
		}
		else
		{
			ping = cl->ping < 9999 ? cl->ping : 9999;
			Com_Printf( "%4i ", ping );
		}

		Com_Printf( "%6i ", cl->guid );
		Com_Printf( "%s^7", cl->name );
		l = 16 - I_DrawStrlen( cl->name );
		for ( j = 0 ; j < l ; j++ )
			Com_Printf( " " );

		Com_Printf( "%7i ", svs.time - cl->lastPacketTime );

		s = NET_AdrToString( cl->netchan.remoteAddress );
		Com_Printf( "%s", s );
		l = 22 - strlen( s );
		for ( j = 0 ; j < l ; j++ )
			Com_Printf( " " );

		Com_Printf( "%5i", cl->netchan.qport );

		Com_Printf( " %5i", cl->rate );

		Com_Printf( "\n" );
	}
	Com_Printf( "\n" );
}

/*
==================
SV_ConSay_f
==================
*/
static void SV_ConSay_f( void )
{
	char    *p;
	char text[MAX_STRING_CHARS];

	// make sure server is running
	if ( !com_sv_running->current.boolean )
	{
		Com_Printf( "Server is not running.\n" );
		return;
	}

	if ( SV_Cmd_Argc() < 2 )
	{
		return;
	}

	strcpy( text, "console: " );
	p = Cmd_Args(1);

	if ( *p == '"' )
	{
		p++;
		p[strlen( p ) - 1] = 0;
	}

	I_strncat(text, sizeof(text), p);
	SV_SendServerCommand(NULL, SV_CMD_CAN_IGNORE, "%c \"\x15%s\"", 104, text);
}

/*
==================
SV_ConTell_f
==================
*/
static void SV_ConTell_f( void )
{
	char    *p;
	char text[MAX_STRING_CHARS];
	client_t *cl;
	int num;

	// make sure server is running
	if ( !com_sv_running->current.boolean )
	{
		Com_Printf( "Server is not running.\n" );
		return;
	}

	if ( SV_Cmd_Argc() < 3 )
	{
		return;
	}

	num = atoi(SV_Cmd_Argv(1));

	if (num < 0 || num >= sv_maxclients->current.integer)
		return;

	cl = &svs.clients[num];

	if (cl->state != CS_ACTIVE)
	{
		return;
	}

	strcpy( text, "console: " );
	p = Cmd_Args(2);

	if ( *p == '"' )
	{
		p++;
		p[strlen( p ) - 1] = 0;
	}

	I_strncat(text, sizeof(text), p);
	SV_SendServerCommand(cl, SV_CMD_CAN_IGNORE, "%c \"\x15%s\"", 104, text);
}

/*
==================
SV_Heartbeat_f

Also called by SV_DropClient, SV_DirectConnect, and SV_SpawnServer
==================
*/
void SV_Heartbeat_f( void )
{
	svs.nextHeartbeatTime = INT_MIN;
}

/*
===========
SV_Serverinfo_f

Examine the serverinfo string
===========
*/
static void SV_Serverinfo_f( void )
{
	Com_Printf("Server info settings:\n");
	Info_Print(Dvar_InfoString(DVAR_SERVERINFO | DVAR_SERVERINFO_NOUPDATE));
}

/*
===========
SV_Systeminfo_f

Examine or change the serverinfo string
===========
*/
static void SV_Systeminfo_f( void )
{
	Com_Printf("System info settings:\n");
	Info_Print(Dvar_InfoString(DVAR_SYSTEMINFO));
}

/*
===========
SV_DumpUser_f

Examine all a users info strings FIXME: move to game
===========
*/
static void SV_DumpUser_f( void )
{
	client_t *cl;

	// make sure server is running
	if ( !com_sv_running->current.boolean )
	{
		Com_Printf( "Server is not running.\n" );
		return;
	}

	if ( SV_Cmd_Argc() != 2 )
	{
		Com_Printf( "Usage: info <userid>\n" );
		return;
	}

	cl = SV_GetPlayerByName();
	if ( !cl )
	{
		return;
	}

	Com_Printf( "userinfo\n" );
	Com_Printf( "--------\n" );
	Info_Print( cl->userinfo );
}

/*
=================
SV_KillServer
=================
*/
static void SV_KillServer_f( void )
{
	Com_Shutdown("EXE_SERVERKILLED");
}

/*
=================
SV_GameCompleteStatus_f

NERVE - SMF
=================
*/
static void SV_GameCompleteStatus_f( void )
{
	SV_MasterGameCompleteStatus();
}

/*
=================
SV_ScriptUsage_f
=================
*/
static void SV_ScriptUsage_f( void )
{
	Scr_DumpScriptThreads();
}

/*
=================
SV_StringUsage_f
=================
*/
static void SV_StringUsage_f( void )
{
	MT_DumpTree();
}

/*
==================
SV_AddOperatorCommands
==================
*/
void SV_AddOperatorCommands( void )
{
	static qboolean initialized;

	if ( initialized )
	{
		return;
	}

	initialized = qtrue;

	Cmd_AddCommand("heartbeat", SV_Heartbeat_f);
	Cmd_AddCommand("onlykick", SV_Drop_f);
	// Arnout: banning requires auth server
	Cmd_AddCommand("banUser", SV_Ban_f);
	Cmd_AddCommand("banClient", SV_BanNum_f);
	Cmd_AddCommand("kick", SV_TempBan_f);
	Cmd_AddCommand("tempBanUser", SV_TempBan_f);
	Cmd_AddCommand("tempBanClient", SV_TempBanNum_f);
	Cmd_AddCommand("unbanUser", SV_Unban_f);
	Cmd_AddCommand("clientkick", SV_DropNum_f);
	Cmd_AddCommand("status", SV_Status_f);
	Cmd_AddCommand("serverinfo", SV_Serverinfo_f);
	Cmd_AddCommand("systeminfo", SV_Systeminfo_f);
	Cmd_AddCommand("dumpuser", SV_DumpUser_f);
	Cmd_AddCommand("map_restart", SV_MapRestart_f);
	Cmd_AddCommand("fast_restart", SV_FastRestart_f);
	Cmd_AddCommand("map", SV_Map_f);
	Cmd_SetAutoComplete("map", "maps/mp", "d3dbsp");
	Cmd_AddCommand("map_rotate", SV_MapRotate_f);
	Cmd_AddCommand("gameCompleteStatus", SV_GameCompleteStatus_f);  // NERVE - SMF
	Cmd_AddCommand("devmap", SV_Map_f);
	Cmd_SetAutoComplete("devmap", "maps/mp", "d3dbsp");
	Cmd_AddCommand("killserver", SV_KillServer_f);

	if ( com_dedicated->current.integer )
	{
		SV_AddDedicatedCommands();
	}

	Cmd_AddCommand("scriptUsage", SV_ScriptUsage_f);
	Cmd_AddCommand("stringUsage", SV_StringUsage_f);
}

/*
==================
SV_RemoveOperatorCommands
==================
*/
void SV_RemoveOperatorCommands( void )
{
	// removing these won't let the server start again
}

/*
==================
SV_AddDedicatedCommands
==================
*/
void SV_AddDedicatedCommands( void )
{
	Cmd_AddCommand("say", SV_ConSay_f);
	Cmd_AddCommand("tell", SV_ConTell_f);
}

/*
==================
SV_RemoveDedicatedCommands
==================
*/
void SV_RemoveDedicatedCommands( void )
{
	Cmd_RemoveCommand("say");
	Cmd_RemoveCommand("tell");
}

