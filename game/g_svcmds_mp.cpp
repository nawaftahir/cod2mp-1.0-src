#include "../qcommon/qcommon.h"
#include "g_shared.h"

#define MAX_CVAR_VALUE_STRING   256

typedef struct ipFilter_s
{
	unsigned mask;
	unsigned compare;
} ipFilter_t;

#define MAX_IPFILTERS   1024

// separate globals as Q3; retail numIPFilters 0x17cc478, ipFilters 0x17cc480
static ipFilter_t ipFilters[MAX_IPFILTERS];
static int numIPFilters;


void UpdateIPBans( void );
void AddIP( const char *str );
void G_ProcessIPBans( void );
void Svcmd_AddIP_f( void );
void Svcmd_RemoveIP_f( void );
void    Svcmd_EntityList_f( void );
qboolean ConsoleCommand();

qboolean StringToFilter( const char *s, ipFilter_t *f )
{
	char num[128];
	int i, j;
	byte b[4];
	byte m[4];

	for ( i = 0 ; i < 4 ; i++ )
	{
		b[i] = 0;
		m[i] = 0;
	}

	for ( i = 0 ; i < 4 ; i++ )
	{
		if ( *s < '0' || *s > '9' )
		{
			Com_Printf("Bad filter address: %s\n", s);
			return qfalse;
		}

		j = 0;
		while ( *s >= '0' && *s <= '9' )
		{
			num[j++] = *s++;
		}
		num[j] = 0;
		b[i] = atoi(num);
		if ( b[i] )
		{
			m[i] = 255;
		}

		if ( !*s )
		{
			break;
		}
		s++;
	}

	f->mask = *(unsigned *)m;
	f->compare = *(unsigned *)b;

	return qtrue;
}

/*
=================
UpdateIPBans
=================
*/
void UpdateIPBans( void )
{
	byte b[4];
	int i;
	char iplist_final[1024];

	*iplist_final = 0;
	for ( i = 0 ; i < numIPFilters ; i++ )
	{
		if ( ipFilters[i].compare == 0xffffffff )
		{
			continue;
		}

		*(unsigned *)b = ipFilters[i].compare;
		Com_sprintf( iplist_final + strlen( iplist_final ), sizeof( iplist_final ) - strlen( iplist_final ),
		             "%i.%i.%i.%i ", b[0], b[1], b[2], b[3] );
	}

	Dvar_SetString(g_banIPs, iplist_final);
}

/*
=================
AddIP
=================
*/
void AddIP( const char *str )
{
	int i;

	for ( i = 0; i < numIPFilters; i++ )
	{
		if (  ipFilters[i].compare == 0xffffffff )
		{
			break;      // free spot
		}
	}

	if ( i == numIPFilters )
	{
		if ( numIPFilters == MAX_IPFILTERS )
		{
			Com_Printf( "IP filter list is full\n" );
			return;
		}
		numIPFilters++;
	}

	if ( !StringToFilter( str, &ipFilters[i] ) )
	{
		ipFilters[i].compare = 0xffffffffu;
	}

	UpdateIPBans();
}

/*
=================
G_ProcessIPBans
=================
*/
void G_ProcessIPBans( void )
{
	char *s, *t;
	// parse a local copy, not the live dvar string
	char str[1024];

	numIPFilters = 0;

	Q_strncpyz( str, g_banIPs->current.string, sizeof( str ) );

	for ( t = s = str; *t; /* */ )
	{
		s = strchr( s, ' ' );
		if ( !s )
		{
			break;
		}
		while ( *s == ' ' )
			*s++ = 0;
		if ( *t )
		{
			AddIP( t );
		}
		t = s;
	}
}

/*
=================
Svcmd_AddIP_f
=================
*/
void Svcmd_AddIP_f( void )
{
	char str[MAX_TOKEN_CHARS];

	if ( SV_Cmd_Argc() < 2 )
	{
		Com_Printf( "Usage:  addip <ip-mask>\n" );
		return;
	}

	SV_Cmd_ArgvBuffer( 1, str, sizeof( str ) );

	AddIP( str );
}

/*
=================
Svcmd_RemoveIP_f
=================
*/
void Svcmd_RemoveIP_f( void )
{
	ipFilter_t f;
	int i;
	char str[MAX_TOKEN_CHARS];

	if ( SV_Cmd_Argc() < 2 )
	{
		Com_Printf( "Usage:  sv removeip <ip-mask>\n" );
		return;
	}

	SV_Cmd_ArgvBuffer( 1, str, sizeof( str ) );

	if ( !StringToFilter( str, &f ) )
	{
		return;
	}

	for ( i = 0 ; i < numIPFilters ; i++ )
	{
		if ( ipFilters[i].mask == f.mask   &&
		        ipFilters[i].compare == f.compare )
		{
			ipFilters[i].compare = 0xffffffffu;
			Com_Printf( "Removed.\n" );

			UpdateIPBans();
			return;
		}
	}

	Com_Printf( "Didn't find %s.\n", str );
}

/*
===================
Svcmd_EntityList_f
===================
*/
void    Svcmd_EntityList_f( void )
{
	int e;
	gentity_t       *check;

	check = g_entities + 1;
	for ( e = 1; e < level.num_entities ; e++, check++ )
	{
		if ( !check->r.inuse )
		{
			continue;
		}
		Com_Printf( "%3i:", e );
		switch ( check->s.eType )
		{
		case ET_GENERAL:
			Com_Printf( "ET_GENERAL          " );
			break;
		case ET_PLAYER:
			Com_Printf( "ET_PLAYER           " );
			break;
		case ET_ITEM:
			Com_Printf( "ET_ITEM             " );
			break;
		case ET_MISSILE:
			Com_Printf( "ET_MISSILE          " );
			break;
		case ET_INVISIBLE:
			Com_Printf( "ET_INVISIBLE        " );
			break;
		case ET_SCRIPTMOVER:
			Com_Printf( "ET_SCRIPTMOVER      " );
			break;
		default:
			Com_Printf( "%3i                 ", check->s.eType );
			break;
		}

		if ( check->classname )
		{
			Com_Printf( "%s", SL_ConvertToString( check->classname ) );
		}
		Com_Printf( "\n" );
	}
}

gclient_t *ClientForString( const char *s )
{
	gclient_t *cl;
	int i;
	int idnum;

	// numeric values are just slot numbers
	if ( s[0] >= '0' && s[0] <= '9' )
	{
		idnum = atoi(s);

		if ( idnum < 0 || idnum >= level.maxclients )
		{
			Com_Printf("Bad client slot: %i\n", idnum);
			return NULL;
		}

		cl = &level.clients[idnum];

		if ( cl->sess.connected == CON_DISCONNECTED )
		{
			Com_Printf("Client %i is not connected\n", idnum);
			return NULL;
		}

		return cl;
	}

	// check for a name match
	for ( i = 0; i < level.maxclients; i++ )
	{
		cl = &level.clients[i];

		if ( cl->sess.connected == CON_DISCONNECTED )
			continue;

		if ( !Q_stricmp(cl->sess.cs.name, s) )
			return cl;
	}

	Com_Printf("User %s is not on the server\n", s);
	return NULL;
}

/*
=================
ConsoleCommand
=================
*/
qboolean ConsoleCommand()
{
	char cmd[MAX_TOKEN_CHARS];

	SV_Cmd_ArgvBuffer( 0, cmd, sizeof( cmd ) );

	if ( Q_stricmp( cmd, "entitylist" ) == 0 )
	{
		Svcmd_EntityList_f();
		return qtrue;
	}

	if ( Q_stricmp( cmd, "addip" ) == 0 )
	{
		Svcmd_AddIP_f();
		return qtrue;
	}

	if ( Q_stricmp( cmd, "removeip" ) == 0 )
	{
		Svcmd_RemoveIP_f();
		return qtrue;
	}

	if ( Q_stricmp( cmd, "listip" ) == 0 )
	{
		Cbuf_ExecuteText(EXEC_INSERT, "g_banIPs\n");
		return qtrue;
	}

	if ( g_dedicated->current.integer )
	{
		if ( Q_stricmp( cmd, "say" ) == 0 )
		{
			SV_GameSendServerCommand(-1, SV_CMD_CAN_IGNORE, va("%c \"GAME_SERVER\x15: %s\"", 101, ConcatArgs(1)));
			return qtrue;
		}
	}

	return qfalse;
}

