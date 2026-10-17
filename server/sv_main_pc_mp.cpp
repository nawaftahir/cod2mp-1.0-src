#include "../qcommon/qcommon.h"
#include "../qcommon/netchan.h"

#define SV_OUTPUTBUF_LENGTH ( 16384 - 16 )

static netadr_t masterAdr;

netadr_t *SV_MasterAddress();
void SV_MasterHeartbeat( const char *hbname );
void SV_MasterGameCompleteStatus();
void SV_MasterShutdown( void );
void SV_FlushRedirect( char *outputbuf );
void SVC_RemoteCommand( netadr_t from, msg_t *msg );
void SV_MatchEnd( void );

/*
=================
SV_MasterAddress
=================
*/
netadr_t *SV_MasterAddress()
{
	if ( !masterAdr.type )
	{
		Com_Printf( "Resolving %s\n", MASTER_SERVER_NAME );

		if ( !NET_StringToAdr( MASTER_SERVER_NAME, &masterAdr ) )
		{
			Com_Printf( "Couldn't resolve address: " MASTER_SERVER_NAME "\n" );
		}
		else
		{
			if ( !strstr( ":", MASTER_SERVER_NAME ) )
			{
				masterAdr.port = BigShort( PORT_MASTER );
			}

			Com_Printf( MASTER_SERVER_NAME " resolved to %i.%i.%i.%i:%i\n",
				masterAdr.ip[0], masterAdr.ip[1], masterAdr.ip[2], masterAdr.ip[3],
				BigShort( masterAdr.port ) );
		}
	}

	return &masterAdr;
}

/*
=================
SV_MasterHeartbeat

Send a message to the masters every few minutes to
let it know we are alive, and log information.
We also send a heartbeat when a server changes from empty to
non-empty, and full to non-full, but not on every player
count change.
=================
*/
void SV_MasterHeartbeat( const char *hbname )
{
	netadr_t *adr;

	// "dedicated 1" is for lan play, "dedicated 2" is for inet public play
	if ( !com_dedicated || com_dedicated->current.integer != 2 )
	{
		return;     // only dedicated servers send heartbeats
	}

	// if not time yet, don't send anything
	if ( svs.time >= svs.nextHeartbeatTime )
	{
		svs.nextHeartbeatTime = svs.time + HEARTBEAT_MSEC;

		adr = SV_MasterAddress();

		if ( adr->type != NA_BAD )
		{
			Com_Printf( "Sending heartbeat to " MASTER_SERVER_NAME "\n" );
			NET_OutOfBandPrint( NS_SERVER, *adr, va( "heartbeat %s\n", hbname ) );
		}
	}

	if ( svs.time >= svs.nextStatusResponseTime )
	{
		svs.nextStatusResponseTime = svs.time + STATUS_MSEC;

		adr = SV_MasterAddress();

		if ( adr->type != NA_BAD )
		{
			SVC_Status( *adr );
		}
	}
}

/*
=================
SV_MasterGameCompleteStatus
=================
*/
void SV_MasterGameCompleteStatus()
{
	netadr_t *adr;

	if ( com_dedicated && com_dedicated->current.integer == 2 )
	{
		adr = SV_MasterAddress();

		if ( adr->type != NA_BAD )
		{
			Com_Printf( "Sending gameCompleteStatus to " MASTER_SERVER_NAME "\n" );
			SVC_GameCompleteStatus( *adr );
		}
	}
}

/*
=================
SV_MasterShutdown

Informs all masters that this server is going down
=================
*/
void SV_MasterShutdown( void )
{
	// send a heartbeat right now
	svs.nextHeartbeatTime = INT_MIN;

	SV_MasterHeartbeat( HEARTBEAT_DEAD );
}

/*
=================
SV_FlushRedirect

Ships redirected console output to the rcon client in packet-sized pieces.
=================
*/
void SV_FlushRedirect( char *outputbuf )
{
	int len;
	char c;
	char buf[FRAGMENT_SIZE];
	int chunk;

	chunk = FRAGMENT_SIZE - 6;   // stored but never read; every use below is the constant

	len = strlen( outputbuf );

	while ( len > FRAGMENT_SIZE - 6 )
	{
		c = outputbuf[FRAGMENT_SIZE - 6];
		outputbuf[FRAGMENT_SIZE - 6] = 0;
		Com_sprintf( buf, sizeof( buf ), "print\n%s", outputbuf );
		NET_OutOfBandPrint( NS_SERVER, svs.redirectAddress, buf );
		len -= FRAGMENT_SIZE - 6;
		outputbuf += FRAGMENT_SIZE - 6;
		*outputbuf = c;
	}

	Com_sprintf( buf, sizeof( buf ), "print\n%s", outputbuf );
	NET_OutOfBandPrint( NS_SERVER, svs.redirectAddress, buf );
}

/*
===============
SVC_RemoteCommand

An rcon packet arrived from the network.
Shift down the remaining args
Redirect all printfs
===============
*/
void SVC_RemoteCommand( netadr_t from, msg_t *msg )
{
	qboolean valid;
	int i;
	int time;
	char remaining[1024];
	int remainingLen;
	int remainingSize;
	const char *password;
	char sv_outputbuf[SV_OUTPUTBUF_LENGTH];
	static int lasttime = 0;

	// prevent using rcon as an amplifier and make dictionary attacks impractical
	time = Com_Milliseconds();

	if ( lasttime && time - lasttime < 500 )
	{
		return;
	}

	lasttime = time;

	password = SV_Cmd_Argv( 1 );

	if ( !*rcon_password->current.string || strcmp( password, rcon_password->current.string ) )
	{
		valid = qfalse;
		Com_Printf( "Bad rcon from %s:\n%s\n", NET_AdrToString( from ), SV_Cmd_Argv( 2 ) );
	}
	else
	{
		valid = qtrue;
		Com_Printf( "Rcon from %s:\n%s\n", NET_AdrToString( from ), SV_Cmd_Argv( 2 ) );
	}

	// start redirecting all print outputs to the packet
	svs.redirectAddress = from;
	Com_BeginRedirect( sv_outputbuf, SV_OUTPUTBUF_LENGTH, SV_FlushRedirect );

	if ( !*rcon_password->current.string )
	{
		Com_Printf( "The server must set 'rcon_password' for clients to use 'rcon'.\n" );
	}
	else if ( !valid )
	{
		if ( *password )
		{
			Com_Printf( "Invalid password.\n" );
		}
		else
		{
			Com_Printf( "You must log in with 'rcon login <password>' before using 'rcon'.\n" );
		}
	}
	else
	{
		remainingLen = 0;
		remainingSize = sizeof( remaining );

		for ( i = 2; i < SV_Cmd_Argc(); i++ )
		{
			remainingLen = Com_AddToString( SV_Cmd_Argv( i ), remaining, remainingLen, remainingSize, 1 );
			remainingLen = Com_AddToString( " ", remaining, remainingLen, remainingSize, 0 );
		}

		if ( remainingLen < remainingSize )
		{
			remaining[remainingLen] = 0;
			SV_Cmd_ExecuteString( remaining );
		}
	}

	Com_EndRedirect();
}

/*
=================
SV_MatchEnd
=================
*/
void SV_MatchEnd( void )
{
}


// Unreferenced storage; original declarations unknown (sized from the layout).
static int unusedStorage;
