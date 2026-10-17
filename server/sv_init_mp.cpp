#include "../qcommon/qcommon.h"
#include "../universal/com_sndalias.h"

// both sit on 128-byte boundaries
serverStatic_t svs __attribute__((aligned(128)));
server_t sv __attribute__((aligned(128)));

dvar_t *sv_fps;
dvar_t *sv_timeout;
dvar_t *sv_zombietime;
dvar_t *rcon_password;
dvar_t *sv_privatePassword;
dvar_t *sv_allowDownload;
dvar_t *sv_maxclients;
dvar_t *sv_privateClients;
dvar_t *sv_hostname;
dvar_t *sv_reconnectlimit;
dvar_t *sv_padPackets;
dvar_t *sv_mapname;
dvar_t *sv_serverid;
dvar_t *sv_maxRate;
dvar_t *sv_minPing;
dvar_t *sv_maxPing;
dvar_t *sv_gametype;
dvar_t *sv_debugRate;
dvar_t *sv_debugReliableCmds;
dvar_t *sv_pure;
dvar_t *sv_floodProtect;
dvar_t *sv_cheats;
dvar_t *sv_allowAnonymous;
dvar_t *sv_showCommands;
dvar_t *sv_packet_info;
dvar_t *sv_showAverageBPS;
dvar_t *sv_mapRotation;
dvar_t *sv_mapRotationCurrent;
dvar_t *sv_kickBanTime;
dvar_t *sv_disableClientConsole;
dvar_t *sv_voice;
dvar_t *sv_voiceQuality;
dvar_t *sv_iwds;
dvar_t *sv_iwdNames;
dvar_t *sv_referencedIwds;
dvar_t *sv_referencedIwdNames;
static dvar_t *sv_allowedClan1;
static dvar_t *sv_allowedClan2;

extern dvar_t *com_expectedHunkUsage;

/*
===============
SV_InitDvar
===============
*/

/*
===============
SV_SetConfigstring
===============
*/
/*
================
SV_SpawnServer

Change the server to a new map, taking all connected
clients along with it.
This is NOT called for map_restart
================
*/
// client-side systems the dedicated server still initialises
int FX_InitSystem( bool rendererExists );
void FX_CreateDefaultEffect();
void Dvar_ClearServerInfoNoUpdateFlags();

bool SV_Loaded();
void SV_Shutdown( const char *finalmsg );

void SV_GetConfigstring( int index, char *buffer, int bufferSize );
const char *SV_GetConfigValueForKey( int start, int max, const char *key );
void SV_SetConfigValueForKey( int start, int max, const char *key, const char *value );
void SV_SetUserinfo( int index, const char *val );
void SV_GetUserinfo( int index, char *buffer, int bufferSize );
void SV_CreateBaseline( void );
void SV_BoundMaxClients( int minimum );
void SV_Startup( void );
void SV_ChangeMaxClients( void );
void SV_SetExpectedHunkUsage( char *mapname );
void SV_ClearServer( void );
void SV_EnableArchivedSnapshot( qboolean bEnable );
void SV_InitArchivedSnapshot();
void SV_FreeArchivedSnapshot();
void Dvar_ResetScriptInfo();
void SV_SaveSystemInfo();
bool SV_Loaded();
void SV_FinalMessage( const char *message );
void SV_DropAllClients();
void SV_Shutdown( const char *finalmsg );

void CL_InitLoad( const char *mapname, const char *gametype );
void CL_MapLoading( const char *mapname );
void CL_StartHunkUsers( void );
void CL_StartLoading( const char *mapname, const char *gametype );
void UI_LoadIngameMenus( void );

void SV_SetConfigstring( int index, const char *val )
{
	int len, i;
	int overhead;
	int maxChunk;
	client_t *client;
	int sent;
	int remaining;
	char cmd;
	char buf[MAX_STRING_CHARS];

	if ( index < 0 || index >= MAX_CONFIGSTRINGS )
	{
		Com_Error( ERR_DROP, "\x15" "SV_SetConfigstring: bad index %i\n", index );
	}

	if ( !val )
	{
		val = "";
	}

	if ( !sv.configstrings[ index ] )
	{
		return;
	}

	// don't bother broadcasting an update if no change
	if ( !strcmp( val, sv.configstrings[ index ] ) )
	{
		return;
	}

	// change the string in sv
	ReplaceStringInternal( (const char **)&sv.configstrings[index], val );

	// send it to all the clients if we aren't
	// spawning a new server
	if ( !SV_Loaded() && !sv.restarting )
	{
		return;
	}

	len = strlen( val );
	sprintf(buf, "%i", index);
	overhead = strlen(buf) + 4;
	maxChunk = sizeof(buf) - overhead;

	for ( i = 0, client = svs.clients; i < sv_maxclients->current.integer ; i++, client++ )
	{
		if ( client->state < CS_PRIMED )
		{
			continue;
		}

		if ( len > maxChunk )
		{
			sent = 0;
			remaining = len;

			while ( remaining > 0 )
			{
				if ( sent == 0 )
				{
					cmd = 'x';
				}
				else if ( remaining <= maxChunk )
				{
					cmd = 'z';
				}
				else
				{
					cmd = 'y';
				}

				Q_strncpyz( buf, &val[sent], maxChunk + 1 );
				SV_SendServerCommand( client, SV_CMD_RELIABLE, "%c %i %s", cmd, index, buf );

				sent += maxChunk;
				remaining -= maxChunk;
			}
		}
		else
		{
			// standard cs, just send it
			SV_SendServerCommand( client, SV_CMD_RELIABLE, "%c %i %s", 'd', index, val );
		}
	}
}

/*
===============
SV_GetConfigstring
===============
*/
void SV_GetConfigstring( int index, char *buffer, int bufferSize )
{
	if ( bufferSize < 1 )
	{
		Com_Error( ERR_DROP, "\x15" "SV_GetConfigstring: bufferSize == %i", bufferSize );
	}

	if ( index < 0 || index >= MAX_CONFIGSTRINGS )
	{
		Com_Error( ERR_DROP, "\x15" "SV_GetConfigstring: bad index %i\n", index );
	}

	if ( !sv.configstrings[index] )
	{
		buffer[0] = 0;
		return;
	}

	Q_strncpyz( buffer, sv.configstrings[index], bufferSize );
}

/*
===============
SV_GetConfigstring
===============
*/
const char* SV_GetConfigstringConst( int index )
{
	const char *cs;

	cs = sv.configstrings[index];

	if ( !sv.configstrings[index] )
		cs = "";

	return cs;
}

/*
===============
SV_GetConfigValueForKey
===============
*/
const char *SV_GetConfigValueForKey( int start, int max, const char *key )
{
	int i;
	const char *cs;

	for ( i = 0; i < max; i++ )
	{
		cs = (&sv.configstrings[start])[i];

		if ( !cs[0] )
			return "";

		if ( !strcasecmp(key, cs) )
			return (&sv.configstrings[start + max])[i];
	}

	return "";
}

/*
===============
SV_SetConfigValueForKey
===============
*/
void SV_SetConfigValueForKey( int start, int max, const char *key, const char *value )
{
	int i;
	const char *cs;

	for ( i = 0; i < max; ++i )
	{
		cs = (&sv.configstrings[start])[i];

		if ( !cs[0] )
		{
			SV_SetConfigstring(start + i, key);
			break;
		}

		if ( !strcasecmp(key, cs) )
			break;
	}

	if ( i == max )
		Com_Error(ERR_DROP, "\x15SV_SetConfigValueForKey: overflow");

	SV_SetConfigstring(start + max + i, value);
}

/*
===============
SV_SetUserinfo
===============
*/
void SV_SetUserinfo( int index, const char *val )
{
	if ( index < 0 || index >= sv_maxclients->current.integer )
	{
		Com_Error( ERR_DROP, "\x15" "SV_SetUserinfo: bad index %i\n", index );
	}

	if ( !val )
	{
		val = "";
	}

	Q_strncpyz( svs.clients[index].userinfo, val, sizeof( svs.clients[ index ].userinfo ) );
	Q_strncpyz( svs.clients[index].name, Info_ValueForKey( val, "name" ), sizeof( svs.clients[index].name ) );
}

/*
===============
SV_GetUserinfo
===============
*/
void SV_GetUserinfo( int index, char *buffer, int bufferSize )
{
	if ( bufferSize < 1 )
	{
		Com_Error( ERR_DROP, "\x15" "SV_GetUserinfo: bufferSize == %i", bufferSize );
	}

	if ( index < 0 || index >= sv_maxclients->current.integer )
	{
		Com_Error( ERR_DROP, "\x15" "SV_GetUserinfo: bad index %i\n", index );
	}

	Q_strncpyz( buffer, svs.clients[ index ].userinfo, bufferSize );
}

/*
================
SV_CreateBaseline

Entity baselines are used to compress non-delta messages
to the clients -- only the fields that differ from the
baseline will be transmitted
================
*/
void SV_CreateBaseline( void )
{
	gentity_t *svent;
	int entnum;

	for ( entnum = 1; entnum < sv.num_entities ; entnum++ )
	{
		svent = SV_GentityNum( entnum );
		if ( !svent->r.linked )
		{
			continue;
		}
		svent->s.number = entnum;
		//
		// take current state as baseline
		//
		sv.svEntities[entnum].baseline.s = svent->s;
		sv.svEntities[entnum].baseline.r.svFlags = svent->r.svFlags;

		sv.svEntities[entnum].baseline.r.clientMask[0] = svent->r.clientMask[0];
		sv.svEntities[entnum].baseline.r.clientMask[1] = svent->r.clientMask[1];

		VectorCopy(svent->r.absmin, sv.svEntities[entnum].baseline.r.absmin);
		VectorCopy(svent->r.absmax, sv.svEntities[entnum].baseline.r.absmax);
	}
}

/*
===============
SV_BoundMaxClients
===============
*/
void SV_BoundMaxClients( int minimum )
{
	// get the current maxclients value
	sv_maxclients = Dvar_RegisterInt("sv_maxclients", 20, 1, MAX_CLIENTS, DVAR_ARCHIVE | DVAR_SERVERINFO | DVAR_LATCH | DVAR_CHANGEABLE_RESET);

	Dvar_ClearModified(sv_maxclients);

	if ( sv_maxclients->current.integer < minimum )
	{
		Dvar_SetInt(sv_maxclients, minimum);
	}
}

/*
===============
SV_Startup
Called when a host starts a map when it wasn't running
one before.  Successive map or map_restart commands will
NOT cause this to be called, unless the game is exited to
the menu system first.
===============
*/
void SV_Startup( void )
{
	if ( svs.initialized )
	{
		Com_Error( ERR_FATAL, "\x15" "SV_Startup: svs.initialized" );
	}

	SV_BoundMaxClients( 1 );

	// RF, avoid trying to allocate large chunk on a fragmented zone
	svs.clients = (client_t *)Z_VirtualAlloc( sizeof( client_t ) * sv_maxclients->current.integer );
	if ( !svs.clients )
	{
		Com_Error( ERR_FATAL, "\x15" "SV_Startup: unable to allocate svs.clients" );
	}

	if ( com_dedicated->current.integer )
	{
		svs.numSnapshotEntities = sv_maxclients->current.integer * PACKET_BACKUP * 64;
		svs.numSnapshotClients =  sv_maxclients->current.integer * sv_maxclients->current.integer * PACKET_BACKUP;
	}
	else
	{
		// we don't need nearly as many when playing locally
		svs.numSnapshotEntities = sv_maxclients->current.integer * 4 * 64;
		svs.numSnapshotClients = sv_maxclients->current.integer * sv_maxclients->current.integer * 4;
	}
	svs.initialized = qtrue;

	Dvar_SetBool( com_sv_running, true );
}

/*
==================
SV_ChangeMaxClients
==================
*/
void SV_ChangeMaxClients( void )
{
	int oldMaxClients;
	int i;
	client_t    *oldClients;
	int count;

	// get the highest client number in use
	count = 0;
	for ( i = 0 ; i < sv_maxclients->current.integer ; i++ )
	{
		if ( svs.clients[i].state >= CS_CONNECTED )
		{
			if ( i > count )
			{
				count = i;
			}
		}
	}
	count++;

	oldMaxClients = sv_maxclients->current.integer;
	// never go below the highest client number in use
	SV_BoundMaxClients( count );
	// if still the same
	if ( sv_maxclients->current.integer == oldMaxClients )
	{
		return;
	}

	oldClients = (client_t *)Hunk_AllocateTempMemory( count * sizeof( client_t ) );
	// copy the clients to hunk memory
	for ( i = 0 ; i < count ; i++ )
	{
		if ( svs.clients[i].state >= CS_CONNECTED )
		{
			oldClients[i] = svs.clients[i];
		}
		else
		{
			Com_Memset( &oldClients[i], 0, sizeof( client_t ) );
		}
	}

	// free old clients arrays
	//Z_Free( svs.clients );
	Z_VirtualFree( svs.clients );    // RF, avoid trying to allocate large chunk on a fragmented zone

	// allocate new clients
	// RF, avoid trying to allocate large chunk on a fragmented zone
	svs.clients = (client_t *)Z_VirtualAlloc( sizeof( client_t ) * sv_maxclients->current.integer );
	if ( !svs.clients )
	{
		Com_Error( ERR_FATAL, "\x15" "SV_Startup: unable to allocate svs.clients" );
	}

	Com_Memset( svs.clients, 0, sv_maxclients->current.integer * sizeof( client_t ) );

	// copy the clients over
	for ( i = 0 ; i < count ; i++ )
	{
		if ( oldClients[i].state >= CS_CONNECTED )
		{
			svs.clients[i] = oldClients[i];
		}
	}

	// free the old clients on the hunk
	Hunk_FreeTempMemory( oldClients );

	// allocate new snapshot entities
	if ( com_dedicated->current.integer )
	{
		svs.numSnapshotEntities = sv_maxclients->current.integer * PACKET_BACKUP * 64;
		svs.numSnapshotClients = sv_maxclients->current.integer * sv_maxclients->current.integer * PACKET_BACKUP;
	}
	else
	{
		// we don't need nearly as many when playing locally
		svs.numSnapshotEntities = sv_maxclients->current.integer * 4 * 64;
		svs.numSnapshotClients = sv_maxclients->current.integer * sv_maxclients->current.integer * 4;
	}
}

/*
====================
SV_SetExpectedHunkUsage
  Sets com_expectedhunkusage, so the client knows how to draw the percentage bar
====================
*/
void SV_SetExpectedHunkUsage( char *mapname )
{
	int handle;
	const char *memlistfile = "hunkusage.dat";
	char *buf;
	const char *buftrav;
	char *token;
	int len;

	len = FS_FOpenFileByMode( memlistfile, &handle, FS_READ );
	if ( len >= 0 )   // the file exists, so read it in, strip out the current entry for this map, and save it out, so we can append the new value
	{
		buf = (char *)Z_Malloc( len + 1 );
		memset( buf, 0, len + 1 );

		FS_Read( (void *)buf, len, handle );
		FS_FCloseFile( handle );

		// now parse the file, filtering out the current map
		buftrav = buf;
		while ( ( token = Com_Parse( &buftrav ) ) != NULL && token[0] )
		{
			if ( !Q_stricmp( token, mapname ) )
			{
				// found a match
				token = Com_Parse( &buftrav );  // read the size
				assert(token);
				if ( token && token[0] )
				{
					// this is the usage
					Dvar_SetInt(com_expectedHunkUsage, atoi( token ));
					Z_Free( buf );
					return;
				}
			}
		}

		Z_Free( buf );
	}

	// just set it to a negative number,so the cgame knows not to draw the percent bar
	//com_expectedhunkusage = -1;
}

/*
================
SV_ClearServer
================
*/
void SV_ClearServer( void )
{
	int i;

	for ( i = 0 ; i < MAX_CONFIGSTRINGS ; i++ )
	{
		if ( sv.configstrings[i] )
		{
			Z_Free( sv.configstrings[i] );
		}
	}
	Com_Memset( &sv, 0, sizeof( sv ) );
}

/*
================
SV_EnableArchivedSnapshot
================
*/
void SV_EnableArchivedSnapshot( qboolean bEnable )
{
	svs.archiveEnabled = bEnable;

	if ( !svs.archiveEnabled )
	{
		return;
	}

	if ( svs.archivedSnapshotFrames )
	{
		return;
	}

	svs.cachedSnapshotEntities = (archivedEntity_s *)Z_MallocInternal(sizeof(archivedEntity_s) * CACHED_SNAPSHOT_ENTITY_SIZE);
	svs.cachedSnapshotClients = (cachedClient_s *)Z_MallocInternal(sizeof(cachedClient_s) * CACHED_SNAPSHOT_CLIENT_SIZE);
	svs.archivedSnapshotFrames = (archivedSnapshot_s *)Z_MallocInternal(sizeof(archivedSnapshot_s) * NUM_ARCHIVED_FRAMES);
	svs.archivedSnapshotBuffer = (byte *)Z_MallocInternal(ARCHIVED_SNAPSHOT_BUFFER_SIZE);
	svs.cachedSnapshotFrames = (cachedSnapshot_s *)Z_MallocInternal(sizeof(cachedSnapshot_s) * NUM_CACHED_FRAMES);
}

/*
===============
SV_InitArchivedSnapshot
===============
*/
void SV_InitArchivedSnapshot()
{
	svs.archiveEnabled = 0;
	svs.nextArchivedSnapshotFrames = 0;
	svs.nextArchivedSnapshotBuffer = 0;
	svs.nextCachedSnapshotEntities = 0;
	svs.nextCachedSnapshotClients = 0;
	svs.nextCachedSnapshotFrames = 0;
}

/*
================
SV_FreeArchivedSnapshot
================
*/
void SV_FreeArchivedSnapshot()
{
	if ( svs.cachedSnapshotEntities )
	{
		Z_Free(svs.cachedSnapshotEntities);
		svs.cachedSnapshotEntities = NULL;
	}

	if ( svs.cachedSnapshotClients )
	{
		Z_Free(svs.cachedSnapshotClients);
		svs.cachedSnapshotClients = NULL;
	}

	if ( svs.archivedSnapshotFrames )
	{
		Z_Free(svs.archivedSnapshotFrames);
		svs.archivedSnapshotFrames = NULL;
	}

	if ( svs.archivedSnapshotBuffer )
	{
		Z_Free(svs.archivedSnapshotBuffer);
		svs.archivedSnapshotBuffer = NULL;
	}

	if ( svs.cachedSnapshotFrames )
	{
		Z_Free(svs.cachedSnapshotFrames);
		svs.cachedSnapshotFrames = NULL;
	}
}

/*
===============
Dvar_ResetScriptInfo
===============
*/
void Dvar_ResetScriptInfo()
{
	Dvar_ClearServerInfoNoUpdateFlags();
}

/*
================
SV_SaveSystemInfo
================
*/
void SV_SaveSystemInfo()
{
	char info[BIG_INFO_STRING];

	I_strncpyz(info, Dvar_InfoString_Big(DVAR_SYSTEMINFO), sizeof(info));

	dvar_modifiedFlags &= ~DVAR_SYSTEMINFO;
	SV_SetConfigstring(CS_SYSTEMINFO, info);

	SV_SetConfigstring(CS_SERVERINFO, Dvar_InfoString(DVAR_SERVERINFO | DVAR_SERVERINFO_NOUPDATE));
	dvar_modifiedFlags &= ~(DVAR_SERVERINFO | DVAR_SERVERINFO_NOUPDATE);

	SV_SetConfig(142, 96, 256);
	dvar_modifiedFlags &= ~DVAR_CODINFO;
}

void SV_SpawnServer( char *server )
{
	int i;
	int checksum;
	int len;
	client_t *cl;
	qboolean savepersist;
	char mapname[MAX_QPATH];
	const char *iwdChecksums;

	Scr_ParseGameTypeList();
	SV_SetGametype();
	CL_InitLoad( server, sv_gametype->current.string );

	// ydnar: broadcast a level change to all connected clients
	if ( com_sv_running->current.boolean )
	{
		savepersist = G_GetSavePersist();

		for ( i = 0, cl = svs.clients; i < sv_maxclients->current.integer; i++, cl++ )
		{
			if ( cl->state < CS_PRIMED )
				continue;

			Com_sprintf(mapname, sizeof(mapname), "loadingnewmap\n%s\n%s", server, sv_gametype->current.string);
			NET_OutOfBandPrint(NS_SERVER, cl->netchan.remoteAddress, mapname);
		}

		NET_Sleep(250);
	}
	else
	{
		savepersist = qfalse;
	}

	// set serverinfo visible name
	Dvar_SetStringByName("mapname", server);

	CL_MapLoading( server );
	CL_ShutdownAll();

	// shut down the existing game if it is running
	SV_ShutdownGameProgs();

	Com_Printf("------ Server Initialization ------\n");
	Com_Printf("Server: %s\n", server);

	// wipe the entire per-level structure
	SV_ClearServer();

	if ( com_dedicated->current.integer )
		FX_FreeSystem();

	FS_Shutdown( 1 );
	FS_ClearIwdReferences();

	Com_Restart();

	// init client structures and svs.numSnapshotEntities
	if ( !com_sv_running->current.boolean )
	{
		SV_Startup();
	}
	else
	{
		SV_ChangeMaxClients();
	}

	I_strncpyz(sv.gametype, sv_gametype->current.string, sizeof(sv.gametype));

	// get a new checksum feed and restart the file system
	srand(Sys_MillisecondsRaw());
	sv.checksumFeed = rand() << 16 ^ rand() ^ Sys_MilliSeconds();

	FS_Restart(sv.checksumFeed);

	Com_sprintf(mapname, sizeof(mapname), "maps/mp/%s.%s", server, GetBspExtension());
	SV_SetExpectedHunkUsage(mapname);

	CL_StartLoading(server, sv.gametype);

	UI_LoadIngameMenus();

	// allocate empty config strings
	for ( i = 0 ; i < MAX_CONFIGSTRINGS ; i++ )
	{
		sv.configstrings[i] = CopyString( "" );
	}

	Dvar_ResetScriptInfo();

	// allocate the snapshot entities on the hunk
	svs.snapshotEntities = (entityState_t *)Hunk_Alloc(sizeof(entityState_t) * svs.numSnapshotEntities);
	svs.nextSnapshotEntities = 0;

	svs.snapshotClients = (clientState_t *)Hunk_Alloc(sizeof(clientState_t) * svs.numSnapshotClients);
	svs.nextSnapshotClients = 0;

	SV_InitArchivedSnapshot();

	// toggle the server bit so clients can detect that a
	// server has changed
	svs.snapFlagServerBit ^= SNAPFLAG_SERVERCOUNT;

	// set nextmap to the same map, but it may be overriden
	// by the game startup or another console command
	Dvar_SetString(nextmap, "map_restart");

	// make sure we are not paused
	Dvar_SetInt(cl_paused, 0);

	Com_sprintf(mapname, sizeof(mapname), "maps/mp/%s.%s", server, GetBspExtension());

	Com_LoadBsp(mapname);
	CM_LoadMap(mapname, &checksum);

	Com_UnloadBsp();
	CM_LinkWorld();

	// serverid should be different each time
	sv_serverId_value = (byte)(sv_serverId_value + 16);

	if ( (sv_serverId_value & 0xF0) == 0 )
	{
		sv_serverId_value += 16;
	}

	Dvar_SetInt(sv_serverid, sv_serverId_value);

	sv.start_frameTime = com_frameTime;

	// media configstring setting should be done during
	// the loading stage, so connected clients don't have
	// to load during actual gameplay
	sv.state = SS_LOADING;

	Com_sprintf(mapname, sizeof(mapname), "maps/mp/%s.%s", server, GetBspExtension());
	Com_LoadSoundAliases(mapname, "all_mp", SASYS_GAME);

	// load and spawn all other entities
	SV_InitGameProgs(savepersist);

	if ( com_dedicated->current.integer )
	{
		FX_InitSystem(0);
		FX_CreateDefaultEffect();
	}

	// run a few frames to allow everything to settle
	for( i = 0; i < GAME_INIT_FRAMES; ++i )
	{
		svs.time += FRAMETIME;
		SV_RunFrame();
	}

	// create a baseline for more efficient communications
	SV_CreateBaseline();

	for( i = 0; i < sv_maxclients->current.integer; ++i )
	{
		if ( svs.clients[i].state >= CS_CONNECTED )
		{
			const char *dropreason = ClientConnect(i, svs.clients[i].scriptId);

			if ( dropreason )
			{
				SV_DropClient(&svs.clients[i], dropreason);
			}
			else
			{
				svs.clients[i].state = CS_CONNECTED;
			}
		}
	}

	// the server sends these to the clients so they can figure
	// out which iwds should be auto-downloaded
	// NOTE: we consider the referencedIwds as 'required for operation'
	if ( sv_pure->current.boolean )
	{
		iwdChecksums = FS_LoadedIwdChecksums();
		Dvar_SetString(sv_iwds, iwdChecksums);

		len = strlen(iwdChecksums);

		if ( !len )
		{
			Com_Printf("WARNING: sv_pure set but no IWD files loaded\n");
		}

		iwdChecksums = FS_LoadedIwdNames();
		Dvar_SetString(sv_iwdNames, iwdChecksums);
	}
	else
	{
		Dvar_SetString(sv_iwds, "");
		Dvar_SetString(sv_iwdNames, "");
	}

	// we want the server to reference the mp_bin iwd that the client is expected to load from
	iwdChecksums = FS_ReferencedIwdChecksums();
	Dvar_SetString(sv_referencedIwds, iwdChecksums);

	iwdChecksums = FS_ReferencedIwdNames();
	Dvar_SetString(sv_referencedIwdNames, iwdChecksums);

	// save systeminfo and serverinfo strings
	SV_SaveSystemInfo();

	// any media configstring setting now should issue a warning
	// and any configstring changes should be reliably transmitted
	// to all clients
	sv.state = SS_GAME;

	// send a heartbeat now so the master will get up to date info
	SV_Heartbeat_f();

	Com_Printf("-----------------------------------\n");
}

/*
===============
SV_Loaded
===============
*/
bool SV_Loaded()
{
	return sv.state == SS_GAME;
}

void SV_Init()
{
	SV_AddOperatorCommands();

	// serverinfo vars
	sv_gametype = Dvar_RegisterString("g_gametype", "dm", DVAR_SERVERINFO | DVAR_LATCH | DVAR_CHANGEABLE_RESET);

	Dvar_RegisterString("sv_keywords", "", DVAR_SERVERINFO | DVAR_CHANGEABLE_RESET);
	Dvar_RegisterInt("protocol", PROTOCOL_VERSION, PROTOCOL_VERSION, PROTOCOL_VERSION, DVAR_SERVERINFO | DVAR_ROM | DVAR_CHANGEABLE_RESET);

	sv_mapname = Dvar_RegisterString("mapname", "", DVAR_SERVERINFO | DVAR_ROM | DVAR_CHANGEABLE_RESET);
	sv_privateClients = Dvar_RegisterInt("sv_privateClients", 0, 0, MAX_CLIENTS, DVAR_SERVERINFO | DVAR_CHANGEABLE_RESET);
	sv_maxclients = Dvar_RegisterInt("sv_maxclients", 20, 1, MAX_CLIENTS, DVAR_ARCHIVE | DVAR_SERVERINFO | DVAR_LATCH | DVAR_CHANGEABLE_RESET);
	sv_hostname = Dvar_RegisterString("sv_hostname", "CoD2Host", DVAR_ARCHIVE | DVAR_SERVERINFO | DVAR_CHANGEABLE_RESET);

	sv_maxRate = Dvar_RegisterInt("sv_maxRate", 0, 0, 25000, DVAR_ARCHIVE | DVAR_SERVERINFO | DVAR_CHANGEABLE_RESET);
	sv_minPing = Dvar_RegisterInt("sv_minPing", 0, 0, 999, DVAR_ARCHIVE | DVAR_SERVERINFO | DVAR_CHANGEABLE_RESET);
	sv_maxPing = Dvar_RegisterInt("sv_maxPing", 0, 0, 999, DVAR_ARCHIVE | DVAR_SERVERINFO | DVAR_CHANGEABLE_RESET);

	sv_floodProtect = Dvar_RegisterBool("sv_floodProtect", true, DVAR_ARCHIVE | DVAR_SERVERINFO | DVAR_CHANGEABLE_RESET);
	sv_allowAnonymous = Dvar_RegisterBool("sv_allowAnonymous", false, DVAR_SERVERINFO | DVAR_CHANGEABLE_RESET);
	sv_showCommands = Dvar_RegisterBool("sv_showCommands", false, DVAR_CHANGEABLE_RESET);
	sv_disableClientConsole = Dvar_RegisterBool("sv_disableClientConsole", false, DVAR_SYSTEMINFO | DVAR_CHANGEABLE_RESET);

	sv_voice = Dvar_RegisterBool("sv_voice", false, DVAR_ARCHIVE | DVAR_SERVERINFO | DVAR_SYSTEMINFO | DVAR_CHANGEABLE_RESET);
	sv_voiceQuality = Dvar_RegisterInt("sv_voiceQuality", 1, 0, 9, DVAR_SYSTEMINFO | DVAR_CHANGEABLE_RESET);

	sv_cheats = Dvar_RegisterBool("sv_cheats", false, DVAR_SYSTEMINFO | DVAR_INIT | DVAR_CHANGEABLE_RESET);
	sv_serverid = Dvar_RegisterInt("sv_serverid", 0, INT_MIN, INT_MAX, DVAR_SYSTEMINFO | DVAR_ROM | DVAR_CHANGEABLE_RESET);

	// systeminfo
	sv_pure = Dvar_RegisterBool("sv_pure", true, DVAR_SERVERINFO | DVAR_SYSTEMINFO | DVAR_CHANGEABLE_RESET);
	sv_iwds = Dvar_RegisterString("sv_iwds", "", DVAR_SYSTEMINFO | DVAR_ROM | DVAR_CHANGEABLE_RESET);
	sv_iwdNames = Dvar_RegisterString("sv_iwdNames", "", DVAR_SYSTEMINFO | DVAR_ROM | DVAR_CHANGEABLE_RESET);
	sv_referencedIwds = Dvar_RegisterString("sv_referencedIwds", "", DVAR_SYSTEMINFO | DVAR_ROM | DVAR_CHANGEABLE_RESET);
	sv_referencedIwdNames = Dvar_RegisterString("sv_referencedIwdNames", "", DVAR_SYSTEMINFO | DVAR_ROM | DVAR_CHANGEABLE_RESET);

	// server vars
	rcon_password = Dvar_RegisterString("rcon_password", "", DVAR_CHANGEABLE_RESET);
	sv_privatePassword = Dvar_RegisterString("sv_privatePassword", "", DVAR_CHANGEABLE_RESET);
	sv_fps = Dvar_RegisterInt("sv_fps", 20, 10, 1000, DVAR_CHANGEABLE_RESET);
	sv_timeout = Dvar_RegisterInt("sv_timeout", 240, 0, 1800, DVAR_CHANGEABLE_RESET);
	sv_zombietime = Dvar_RegisterInt("sv_zombietime", 2, 0, 1800, DVAR_CHANGEABLE_RESET);

	sv_allowDownload = Dvar_RegisterBool("sv_allowDownload", true, DVAR_ARCHIVE | DVAR_CHANGEABLE_RESET);
	sv_reconnectlimit = Dvar_RegisterInt("sv_reconnectlimit", 3, 0, 1800, DVAR_ARCHIVE | DVAR_CHANGEABLE_RESET);
	sv_padPackets = Dvar_RegisterInt("sv_padPackets", 0, 0, INT_MAX, DVAR_CHANGEABLE_RESET);

	engineState->serverKilled = 0;

	sv_allowedClan1 = Dvar_RegisterString("sv_allowedClan1", "", DVAR_CHANGEABLE_RESET);
	sv_allowedClan2 = Dvar_RegisterString("sv_allowedClan2", "", DVAR_CHANGEABLE_RESET);

	sv_packet_info = Dvar_RegisterBool("sv_packet_info", false, DVAR_CHANGEABLE_RESET);
	sv_showAverageBPS = Dvar_RegisterBool("sv_showAverageBPS", false, DVAR_CHANGEABLE_RESET);

	sv_kickBanTime = Dvar_RegisterFloat("sv_kickBanTime", 300.0f, 0.0f, 3600.0f, DVAR_CHANGEABLE_RESET);
	sv_mapRotation = Dvar_RegisterString("sv_mapRotation", "", DVAR_CHANGEABLE_RESET);

	sv_mapRotationCurrent = Dvar_RegisterString("sv_mapRotationCurrent", "", DVAR_CHANGEABLE_RESET);

	sv_debugRate = Dvar_RegisterBool("sv_debugRate", false, DVAR_CHANGEABLE_RESET);
	sv_debugReliableCmds = Dvar_RegisterBool("sv_debugReliableCmds", false, DVAR_CHANGEABLE_RESET);

	nextmap = Dvar_RegisterString("nextmap", "", DVAR_CHANGEABLE_RESET);
	com_expectedHunkUsage = Dvar_RegisterInt("com_expectedHunkUsage", 0, 0, INT_MAX, DVAR_ROM | DVAR_CHANGEABLE_RESET);
}

/*
==================
SV_FinalMessage

Used by SV_Shutdown to send a final message to all
connected clients before the server goes down.  The messages are sent immediately,
not just stuck on the outgoing message list, because the server is going
to totally exit after returning from this function.
==================
*/
void SV_FinalMessage( const char *message )
{
	int			i, j;
	client_t	*cl;

	// send it twice, ignoring rate
	for ( j = 0 ; j < 2 ; j++ )
	{
		for ( i = 0, cl = svs.clients; i < sv_maxclients->current.integer; i++, cl++)
		{
			if (cl->state >= CS_CONNECTED )
			{
				// don't send a disconnect to a local client
				if ( cl->netchan.remoteAddress.type != NA_LOOPBACK )
				{
					SV_SendServerCommand(cl, SV_CMD_CAN_IGNORE, "%c \"%s\"", 101, message);
					SV_SendServerCommand(cl, SV_CMD_RELIABLE, "%c \"%s\"", 119, message);
				}
				// force a snapshot to be sent
				cl->nextSnapshotTime = -1;
				SV_SendClientSnapshot( cl );
			}
		}
	}
}

/*
==================
SV_DropAllClients
==================
*/
void SV_DropAllClients()
{
	int i;
	client_t *cl;

	for ( i = 0, cl = svs.clients; i < sv_maxclients->current.integer; i++, cl++ )
	{
		if ( cl->state >= CS_CONNECTED )
			SV_DropClient(cl, "EXE_DISCONNECTED");
	}
}

/*
================
SV_Shutdown

Called when each game quits,
before Sys_Quit or Sys_Error
================
*/
void SV_Shutdown( const char *finalmsg )
{
	bool bLoading;

	assert(Sys_IsMainThread());

	if ( !com_sv_running || !com_sv_running->current.boolean )
	{
		return;
	}

	Com_Printf("----- Server Shutdown -----\n");

	bLoading = sv.state == SS_LOADING;

	if ( svs.clients )
	{
		SV_FinalMessage(finalmsg);
	}

	SV_RemoveOperatorCommands();
	SV_MasterShutdown();
	SV_ShutdownGameProgs();

	// drop all clients
	SV_DropAllClients();

	// free current level
	SV_ClearServer();

	// free server static data
	if ( svs.clients )
	{
		SV_FreeClients(); // RF, avoid trying to allocate large chunk on a fragmented zone
	}

	SV_FreeArchivedSnapshot();
	memset(&svs, 0, sizeof(svs));

	if ( com_dedicated->current.integer )
		FX_FreeSystem();

	Dvar_SetBool(com_sv_running, false);

	Com_Printf("---------------------------\n");

	// disconnect any local clients
	CL_Disconnect();

	if ( !bLoading )
	{
		return;
	}

	Com_AbortDObj();
	DObjAbort();
	XAnimAbort();
	Scr_Abort();

	bgs = NULL;
}

