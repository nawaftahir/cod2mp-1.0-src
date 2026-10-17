#include "qcommon.h"
#include "netchan.h"

void CL_Netchan_AddOOBProfilePacket( int iLength );

/* net_chan.c sees a plain prototype, not glibc's builtin-tagged one:
 * the builtin form stages the first argument in a register before the size,
 * which the shipped code does not do. */
int memcmp( const void *, const void *, unsigned ) throw();
char *strstr( const char *, const char * ) throw();

#define PACKET_HEADER           10          // two ints and a short
#define FRAGMENT_BIT    ( 1 << 31 )

static const char *netsrcString[2] =
{
	"client1",
	"server"
};

static dvar_t      *showpackets;
static dvar_t      *showdrop;
static dvar_t      *packetDebug;
dvar_t      *net_profile;
dvar_t      *net_showprofile;
dvar_t      *net_lanauthorize;
int qport = 0;
static int net_iProfilingOn;

void CL_Netchan_PrintProfileStats( int bPrintToConsole );

// Unreferenced: a 32-aligned .bss object of up to this size sits between
// net_iProfilingOn and loopbacks. The binary is stripped, so only its extent is known.
static byte net_chan_unused[0x60];

// there needs to be enough loopback messages to hold a complete
// gamestate of maximum size
#define MAX_LOOPBACK    16

typedef struct
{
	byte data[MAX_PACKETLEN];
	int datalen;
	int port;
} loopmsg_t;

typedef struct
{
	loopmsg_t msgs[MAX_LOOPBACK];
	int get, send;
} loopback_t;

static loopback_t loopbacks[2];

const char* NET_AdrToString( netadr_t a );
void NetProf_PrepProfiling( netProfileInfo_t **pProf );
void NetProf_AddPacket( netProfileStream_t *pProfStream, int iSize, int bFragment );
void NetProf_NewSendPacket( netchan_t *pChan, int iSize, int bFragment );
void NetProf_NewRecievePacket( netchan_t *pChan, int iSize, int bFragment );
void NetProf_UpdateStatistics( netProfileStream_t *pStream );
static void Net_DumpProfile_f( void );
void Net_DisplayProfile( void );
void Netchan_Init(int port);
void Netchan_Setup( netsrc_t sock, netchan_t *chan, netadr_t adr, int qport );
bool Netchan_TransmitNextFragment( netchan_t *chan );
bool Netchan_Transmit( netchan_t *chan, int length, const byte *data );
qboolean Netchan_Process( netchan_t *chan, msg_t *msg );
int NET_CompareBaseAdrSigned( netadr_t *a, netadr_t *b );
qboolean NET_CompareBaseAdr( netadr_t a, netadr_t b );
int NET_CompareAdrSigned( netadr_t *a, netadr_t *b );
qboolean NET_CompareAdr( netadr_t a, netadr_t b );
qboolean NET_IsLocalAddress( netadr_t adr );
qboolean NET_GetPacket( netadr_t *net_from, msg_t *net_message );
qboolean NET_GetLoopPacket_Real( int sock, netadr_t *net_from, msg_t *net_message );
qboolean NET_GetLoopPacket( int sock, netadr_t *net_from, msg_t *net_message );
void NET_SendLoopPacket( int sock, int length, const void *data, netadr_t to );
bool NET_SendPacket( netsrc_t sock, int length, const void *data, netadr_t to );
unsigned char NET_OutOfBandPrint( netsrc_t sock, netadr_t adr, const char *data );
unsigned char NET_OutOfBandData( netsrc_t sock, netadr_t adr, byte *format, int len );
void NET_OutOfBandVoiceData( netsrc_t sock, netadr_t adr, byte *format, int len );
qboolean NET_StringToAdr( const char *s, netadr_t *a );

/*
===============
NET_CompareAdr
================
*/
qboolean NET_CompareAdr( netadr_t a, netadr_t b )
{
	return NET_CompareAdrSigned(&a, &b) == 0;
}

// Local client index for the "[client %i]" traces; always 0 on a dedicated
// server. Original name unknown: the copy is emitted at the end of this TU.
inline int Net_LocalClientNum()
{
	return 0;
}

/*
==============
NET_AdrToString
==============
*/
const char* NET_AdrToString( netadr_t a )
{
	static char s[64];

	if ( a.type == NA_LOOPBACK )
	{
		Com_sprintf( s, sizeof( s ), "loopback" );
	}
	else if ( a.type == NA_IP )
	{
		Com_sprintf( s, sizeof( s ), "%i.%i.%i.%i:%i",
		             a.ip[0], a.ip[1], a.ip[2], a.ip[3], BigShort( a.port ) );
	}
	else
	{
		Com_sprintf( s, sizeof( s ), "%02x%02x%02x%02x.%02x%02x%02x%02x%02x%02x:%i",
		             a.ipx[0], a.ipx[1], a.ipx[2], a.ipx[3], a.ipx[4], a.ipx[5], a.ipx[6], a.ipx[7], a.ipx[8], a.ipx[9],
		             BigShort( a.port ) );
	}

	return s;
}

/*
==============
NetProf_PrepProfiling
==============
*/
void NetProf_PrepProfiling( netProfileInfo_t **pProf )
{
	if ( net_profile->current.integer )
	{
		if ( !net_iProfilingOn )
		{
			if ( !com_sv_running->current.boolean || ( engineState->clientActive && net_profile->current.integer == 2 ) )
				net_iProfilingOn = 1;
			else
				net_iProfilingOn = 2;

			Com_Printf("Net Profiling turned on: %s\n", netsrcString[ net_iProfilingOn - 1 ]);
		}

		if ( !*pProf )
		{
			*pProf = (netProfileInfo_t *)malloc(sizeof(netProfileInfo_t));
			memset(*pProf, 0, sizeof(netProfileInfo_t));
		}
	}
	else
	{
		if ( net_iProfilingOn )
		{
			net_iProfilingOn = 0;
			Com_Printf("Net Profiling turned off\n");
		}

		if ( *pProf )
		{
			free(*pProf);
			*pProf = 0;
		}
	}
}

/*
==============
NetProf_AddPacket
==============
*/
void NetProf_AddPacket( netProfileStream_t *pProfStream, int iSize, int bFragment )
{
	netProfilePacket_t *pPacket;

	assert(net_iProfilingOn);

	pProfStream->iCurrPacket = (pProfStream->iCurrPacket + 1) % MAX_PROFILE_PACKETS;
	pPacket = &pProfStream->packets[pProfStream->iCurrPacket];
	pPacket->iTime = Sys_MilliSeconds();
	pPacket->iSize = iSize;
	pPacket->bFragment = bFragment;
}

/*
=============
NetProf_NewSendPacket
=============
*/
void NetProf_NewSendPacket( netchan_t *pChan, int iSize, int bFragment )
{
	if ( !net_iProfilingOn )
	{
		return;
	}

	NetProf_AddPacket(&pChan->pProf->send, iSize, bFragment);

	if ( net_showprofile->current.integer & 2 )
	{
		Com_Printf("%s send%s: %i\n", netsrcString[pChan->sock], bFragment ? " fragment" : "", iSize);
	}
}

/*
=============
NetProf_NewRecievePacket
=============
*/
void NetProf_NewRecievePacket( netchan_t *pChan, int iSize, int bFragment )
{
	if ( !net_iProfilingOn )
	{
		return;
	}

	NetProf_AddPacket(&pChan->pProf->recieve, iSize, bFragment);

	if ( net_showprofile->current.integer & 2 )
	{
		Com_Printf("%s recieve%s: %i\n", netsrcString[pChan->sock], bFragment ? " fragment" : "", iSize);
	}
}

/*
==============
NetProf_UpdateStatistics
==============
*/
void NetProf_UpdateStatistics( netProfileStream_t *pStream )
{
	int i;
	int iNumPackets;
	int iNumFragments;
	int iOldestPacket;
	int iOldestPacketTime;
	int iTimeSpan;
	int iTotalBytes;
	int iSmallestSize;
	int iLargestSize;
	int iRecentPacketWindow = 1000;
	int iBPSCalcInterval = 100;

	assert(pStream);
	assert(net_iProfilingOn);

	iNumPackets = 0;
	iNumFragments = 0;
	iOldestPacket = -1;
	iOldestPacketTime = Sys_MilliSeconds();
	iTotalBytes = 0;
	iSmallestSize = 9999;
	iLargestSize = 0;

	for ( i = 0; i < MAX_PROFILE_PACKETS; ++i )
	{
		if ( !pStream->packets[i].iTime )
			continue;

		if ( Sys_MilliSeconds() > pStream->packets[i].iTime + 1000 )
			continue;

		iNumPackets++;

		if ( pStream->packets[i].bFragment )
			iNumFragments++;

		if ( pStream->packets[i].iTime < iOldestPacketTime )
		{
			iOldestPacket = i;
			iOldestPacketTime = pStream->packets[i].iTime;
		}

		iTotalBytes += pStream->packets[i].iSize;

		if ( pStream->packets[i].iSize < iSmallestSize )
			iSmallestSize = pStream->packets[i].iSize;

		if ( pStream->packets[i].iSize > iLargestSize )
			iLargestSize = pStream->packets[i].iSize;
	}

	if ( !iNumPackets )
	{
		pStream->iBytesPerSecond = 0;
		pStream->iLastBPSCalcTime = 0;
		pStream->iCountedPackets = 0;
		pStream->iCountedFragments = 0;
		pStream->iFragmentPercentage = 0;
		pStream->iLargestPacket = 0;
		pStream->iSmallestPacket = 0;
		return;
	}

	if ( iNumFragments )
		pStream->iFragmentPercentage = 100 * iNumFragments / iNumPackets;
	else
		pStream->iFragmentPercentage = 0;

	pStream->iLargestPacket = iLargestSize;
	pStream->iSmallestPacket = iSmallestSize;

	if ( pStream->iLastBPSCalcTime + 100 < Sys_MilliSeconds() )
	{
		iTimeSpan = Sys_MilliSeconds() - iOldestPacketTime;

		if ( iOldestPacket != -1 )
		{
			iTotalBytes -= pStream->packets[iOldestPacket].iSize;
			iNumPackets--;

			if ( pStream->packets[iOldestPacket].bFragment )
				iNumFragments--;
		}

		if ( iTimeSpan < 1 || !iNumPackets )
		{
			pStream->iBytesPerSecond = 0;
		}
		else
		{
			if ( !iTotalBytes )
				pStream->iBytesPerSecond = 0;
			else
				pStream->iBytesPerSecond = (int)((float)iTotalBytes / ((float)iTimeSpan * 0.001f));

			pStream->iLastBPSCalcTime = Sys_MilliSeconds();
		}
	}

	pStream->iCountedPackets = iNumPackets;
	pStream->iCountedFragments = iNumFragments;
}

/*
===============
Net_DumpProfile_f
================
*/
static void Net_DumpProfile_f( void )
{
	if ( !net_iProfilingOn )
	{
		Com_Printf("Network profiling is not on. Set net_profile to turn on network profiling\n");
		return;
	}

	if ( net_iProfilingOn == 1 )
		CL_Netchan_PrintProfileStats(qtrue);
	else
		SV_Netchan_PrintProfileStats(qtrue);
}

/*
===============
Net_DisplayProfile
================
*/
void Net_DisplayProfile( void )
{
	if ( !net_iProfilingOn )
	{
		return;
	}

	if ( net_iProfilingOn == 1 )
		CL_Netchan_PrintProfileStats(qfalse);
	else
		SV_Netchan_PrintProfileStats(qfalse);
}

/*
===============
Netchan_Init
===============
*/
void Netchan_Init(int port)
{
	showpackets = Dvar_RegisterBool("showpackets", false, DVAR_CHANGEABLE_RESET);
	showdrop = Dvar_RegisterBool("showdrop", false, DVAR_CHANGEABLE_RESET);
	packetDebug = Dvar_RegisterBool("packetDebug", false, DVAR_CHANGEABLE_RESET);
	port &= 0xffff;
	qport = port;
	net_profile = Dvar_RegisterInt("net_profile", 0, 0, 2, DVAR_CHANGEABLE_RESET);
	net_showprofile = Dvar_RegisterInt("net_showprofile", 0, 0, 3, DVAR_CHANGEABLE_RESET);
	net_lanauthorize = Dvar_RegisterBool("net_lanauthorize", false, DVAR_CHANGEABLE_RESET);
	Cmd_AddCommand("net_dumpprofile", Net_DumpProfile_f);
}

/*
==============
Netchan_Setup

called to open a channel to a remote system
==============
*/
void Netchan_Setup( netsrc_t sock, netchan_t *chan, netadr_t adr, int qport )
{
	memset( chan, 0, sizeof( *chan ) );

	chan->sock = sock;
	chan->remoteAddress = adr;
	chan->qport = qport;
	chan->incomingSequence = 0;
	chan->outgoingSequence = 1;
	NetProf_PrepProfiling(&chan->pProf);
}

/*
=================
Netchan_TransmitNextFragment

Send one fragment of the current message
=================
*/
bool Netchan_TransmitNextFragment( netchan_t *chan )
{
	msg_t send;
	byte send_buf[MAX_PACKETLEN];
	int fragmentLength;
	int res;

	NetProf_PrepProfiling(&chan->pProf);

	// write the packet header
	MSG_Init( &send, send_buf, sizeof( send_buf ) );                // <-- only do the oob here
	MSG_WriteLong( &send, chan->outgoingSequence | FRAGMENT_BIT );

	// send the qport if we are a client
	if ( chan->sock < NS_SERVER )
	{
		MSG_WriteShort( &send, chan->qport );
	}

	// copy the reliable message to the packet first
	fragmentLength = FRAGMENT_SIZE;

	if ( chan->unsentFragmentStart  + fragmentLength > chan->unsentLength )
	{
		fragmentLength = chan->unsentLength - chan->unsentFragmentStart;
	}

#ifdef LIBCOD
	if (chan->protocol == 118)
		MSG_WriteLong( &send, chan->unsentFragmentStart );
	else
		MSG_WriteShort( &send, chan->unsentFragmentStart );
#else
#if PROTOCOL_VERSION == 118
	MSG_WriteLong( &send, chan->unsentFragmentStart );
#else
	MSG_WriteShort( &send, chan->unsentFragmentStart );
#endif
#endif

	MSG_WriteShort( &send, fragmentLength );
	MSG_WriteData( &send, chan->unsentBuffer + chan->unsentFragmentStart, fragmentLength );

	// send the datagram
	res = NET_SendPacket( chan->sock, send.cursize, send.data, chan->remoteAddress );
	NetProf_NewSendPacket(chan, send.cursize, qtrue);

	if ( showpackets->current.boolean )
	{
		Com_Printf( "[client %i] %s send %4i : s=%i fragment=%i,%i\n"
		            , Net_LocalClientNum()
		            , netsrcString[ chan->sock ]
		            , send.cursize
		            , chan->outgoingSequence - 1
		            , chan->unsentFragmentStart
		            , fragmentLength );
	}

	chan->unsentFragmentStart += fragmentLength;

	// this exit condition is a little tricky, because a packet
	// that is exactly the fragment length still needs to send
	// a second packet of zero length so that the other side
	// can tell there aren't more to follow
	if ( chan->unsentFragmentStart == chan->unsentLength && fragmentLength != FRAGMENT_SIZE )
	{
		chan->outgoingSequence++;
		chan->unsentFragments = qfalse;
	}

	return res > 0;
}

/*
===============
Netchan_Transmit

Sends a message to a connection, fragmenting if necessary
A 0 length will still generate a packet.
================
*/
bool Netchan_Transmit( netchan_t *chan, int length, const byte *data )
{
	msg_t send;
	byte send_buf[MAX_PACKETLEN];
	int res;

	if ( length > MAX_MSGLEN )
	{
		Com_Error( ERR_DROP, "\x15" "Netchan_Transmit: length = %i", length );
	}

	chan->unsentFragmentStart = 0;

	// fragment large reliable messages
	if ( length >= FRAGMENT_SIZE )
	{
		chan->unsentFragments = qtrue;
		chan->unsentLength = length;
		Com_Memcpy( chan->unsentBuffer, data, length );

		// only send the first fragment now
		Netchan_TransmitNextFragment( chan );

		return qtrue;
	}

	NetProf_PrepProfiling(&chan->pProf);

	// write the packet header
	MSG_Init( &send, send_buf, sizeof( send_buf ) );

	MSG_WriteLong( &send, chan->outgoingSequence );
	chan->outgoingSequence++;

	// send the qport if we are a client
	if ( chan->sock < NS_SERVER )
	{
		MSG_WriteShort( &send, chan->qport );
	}

	if ( packetDebug->current.boolean )
		Com_Printf("Adding %i byte payload to packet\n", length);

	MSG_WriteData( &send, data, length );

	if ( packetDebug->current.boolean )
		Com_Printf("Sending %i byte packet\n", send.cursize);

	// send the datagram
	res = NET_SendPacket( chan->sock, send.cursize, send.data, chan->remoteAddress );
	NetProf_NewSendPacket(chan, send.cursize, 0);

	if ( showpackets->current.boolean )
	{
		Com_Printf( "[client %i] %s send %4i : s=%i ack=%i\n"
		            , Net_LocalClientNum() + 1
		            , netsrcString[ chan->sock ]
		            , send.cursize
		            , chan->outgoingSequence - 1
		            , chan->incomingSequence );
	}

	return res > 0;
}

/*
=================
Netchan_Process

Returns qfalse if the message should not be processed due to being
out of order or a fragment.

Msg must be large enough to hold MAX_MSGLEN, because if this is the
final fragment of a multi-part message, the entire thing will be
copied out.
=================
*/
qboolean Netchan_Process( netchan_t *chan, msg_t *msg )
{
	int sequence;
	int qport;
	int fragmentStart, fragmentLength;
	qboolean fragmented;

	NetProf_PrepProfiling(&chan->pProf);

	// get sequence numbers
	MSG_BeginReading( msg );
	sequence = MSG_ReadLong( msg );

	// check for fragment information
	if ( sequence & FRAGMENT_BIT )
	{
		sequence &= ~FRAGMENT_BIT;
		fragmented = qtrue;
	}
	else
	{
		fragmented = qfalse;
	}

	// read the qport if we are a server
	if ( chan->sock == NS_SERVER )
	{
		qport = MSG_ReadShort( msg );
	}

	// read the fragment information
	if ( fragmented )
	{
		fragmentStart = MSG_ReadShort( msg );
		fragmentLength = MSG_ReadShort( msg );
	}
	else
	{
		fragmentStart = 0;      // stop warning message
		fragmentLength = 0;
	}

	NetProf_NewRecievePacket(chan, msg->cursize, fragmented);

	if ( showpackets->current.boolean )
	{
		if ( fragmented )
		{
			Com_Printf( "[client %i] %s recv %4i : s=%i fragment=%i,%i\n"
			            , Net_LocalClientNum() + 1
			            , netsrcString[ chan->sock ]
			            , msg->cursize
			            , sequence
			            , fragmentStart, fragmentLength );
		}
		else
		{
			Com_Printf( "[client %i] %s recv %4i : s=%i\n"
			            , Net_LocalClientNum() + 1
			            , netsrcString[ chan->sock ]
			            , msg->cursize
			            , sequence );
		}
	}

	//
	// discard out of order or duplicated packets
	//
	if ( sequence <= chan->incomingSequence )
	{
		if ( showdrop->current.boolean || showpackets->current.boolean )
		{
			Com_Printf( "[client %i] %s:Out of order packet %i at %i\n"
			            , Net_LocalClientNum() + 1
			            , NET_AdrToString( chan->remoteAddress )
			            ,  sequence
			            , chan->incomingSequence );
		}
		return qfalse;
	}

	//
	// dropped packets don't keep the message from being used
	//
	chan->dropped = sequence - ( chan->incomingSequence + 1 );

	if ( chan->dropped > 0 )
	{
		if ( showdrop->current.boolean || showpackets->current.boolean )
		{
			Com_Printf( "[client %i] %s: Dropped %i packets at %i\n"
			            , Net_LocalClientNum() + 1
			            , NET_AdrToString( chan->remoteAddress )
			            , chan->dropped
			            , sequence );
		}
	}

	//
	// if this is the final framgent of a reliable message,
	// bump incoming_reliable_sequence
	//
	if ( fragmented )
	{
	// TTimo
	// make sure we add the fragments in correct order
	// either a packet was dropped, or we received this one too soon
	// we don't reconstruct the fragments. we will wait till this fragment gets to us again
	// (NOTE: we could probably try to rebuild by out of order chunks if needed)
	if ( sequence != chan->fragmentSequence )
	{
		chan->fragmentSequence = sequence;
		chan->fragmentLength = 0;
	}

	// if we missed a fragment, dump the message
	if ( fragmentStart != chan->fragmentLength )
	{
		if ( showdrop->current.boolean || showpackets->current.boolean )
		{
			Com_Printf( "%s:Dropped a message fragment\n"
			            , NET_AdrToString( chan->remoteAddress )
			            , sequence );
		}
		// we can still keep the part that we have so far,
		// so we don't need to clear chan->fragmentLength
		return qfalse;
	}

	// copy the fragment to the fragment buffer
	if ( fragmentLength < 0 || msg->readcount + fragmentLength > msg->cursize ||
	        chan->fragmentLength + fragmentLength > sizeof( chan->fragmentBuffer ) )
	{
		if ( showdrop->current.boolean || showpackets->current.boolean )
		{
			Com_Printf( "%s:illegal fragment length\n"
			            , NET_AdrToString( chan->remoteAddress ) );
		}
		return qfalse;
	}

	memcpy( chan->fragmentBuffer + chan->fragmentLength,
	            msg->data + msg->readcount, fragmentLength );

	chan->fragmentLength += fragmentLength;

	// if this wasn't the last fragment, don't process anything
	if ( fragmentLength == FRAGMENT_SIZE )
	{
		return qfalse;
	}

	if ( chan->fragmentLength > msg->maxsize )
	{
		Com_Printf( "%s:fragmentLength %i > msg->maxsize\n"
		            , NET_AdrToString( chan->remoteAddress ),
		            chan->fragmentLength );
		return qfalse;
	}

	// copy the full message over the partial fragment

	// make sure the sequence number is still there
	*(int *)msg->data = LittleLong( sequence );

	memcpy( msg->data + 4, chan->fragmentBuffer, chan->fragmentLength );
	msg->cursize = chan->fragmentLength + 4;
	chan->fragmentLength = 0;
	MSG_BeginReading( msg ); // past the sequence number
	MSG_ReadLong( msg );  // past the sequence number
	}

	// TTimo
	// clients were not acking fragmented messages
	chan->incomingSequence = sequence;

	return qtrue;
}

/*
===============
NET_CompareBaseAdrSigned
================
*/
int NET_CompareBaseAdrSigned( netadr_t *a, netadr_t *b )
{
	if ( a->type != b->type )
	{
		return a->type - b->type;
	}

	if ( a->type == NA_LOOPBACK )
	{
		return a->port - b->port;
	}

	if ( a->type == NA_BOT )
	{
		return a->port - b->port;
	}

	if ( a->type == NA_IP )
	{
		return memcmp(a->ip, b->ip, sizeof(a->ip));
	}

	if ( a->type == NA_IPX )
	{
		return memcmp(a->ipx, b->ipx, sizeof(a->ipx));
	}

	Com_Printf("NET_CompareBaseAdrSigned: bad address type\n");
	return 0;
}

/*
===============
NET_CompareBaseAdr
================
*/
qboolean NET_CompareBaseAdr( netadr_t a, netadr_t b )
{
	return NET_CompareBaseAdrSigned(&a, &b) == 0;
}

/*
===============
NET_CompareAdrSigned
================
*/
int NET_CompareAdrSigned( netadr_t *a, netadr_t *b )
{
	if ( a->type != b->type )
	{
		return a->type - b->type;
	}

	if ( a->type == NA_LOOPBACK )
	{
		return 0;
	}

	if ( a->type == NA_IP )
	{
		if ( a->port != b->port )
		{
			return a->port - b->port;
		}

		return memcmp(a->ip, b->ip, sizeof(a->ip));
	}

	if ( a->type == NA_IPX )
	{
		if ( a->port != b->port )
		{
			return a->port - b->port;
		}

		return memcmp(a->ipx, b->ipx, sizeof(a->ipx));
	}

	Com_Printf("NET_CompareAdrSigned: bad address type\n");
	return 0;
}

/*
==============
NET_IsLocalAddress
==============
*/
qboolean NET_IsLocalAddress( netadr_t adr )
{
	return adr.type == NA_LOOPBACK || adr.type == NA_BOT;
}

/*
=============
NET_GetPacket
=============
*/
qboolean NET_GetPacket( netadr_t *net_from, msg_t *net_message )
{
	return Sys_GetPacket(net_from, net_message);
}

/*
==============
NET_GetLoopPacket_Real
==============
*/
qboolean NET_GetLoopPacket_Real( int sock, netadr_t *net_from, msg_t *net_message )
{
	int i;
	loopback_t *loop;

	loop = &loopbacks[sock];

	if ( loop->send - loop->get > MAX_LOOPBACK )
	{
		loop->get = loop->send - MAX_LOOPBACK;
	}

	if ( loop->get >= loop->send )
	{
		return qfalse;
	}

	i = loop->get & ( MAX_LOOPBACK - 1 );
	loop->get++;

	memcpy( net_message->data, loop->msgs[i].data, loop->msgs[i].datalen );
	net_message->cursize = loop->msgs[i].datalen;
	memset( net_from, 0, sizeof( *net_from ) );
	net_from->type = NA_LOOPBACK;
	net_from->port = loop->msgs[i].port;
	return qtrue;
}

/*
==============
NET_GetLoopPacket
==============
*/
qboolean NET_GetLoopPacket( int sock, netadr_t *net_from, msg_t *net_message )
{
	return NET_GetLoopPacket_Real(sock, net_from, net_message);
}

/*
==============
NET_SendLoopPacket
==============
*/
void NET_SendLoopPacket( int sock, int length, const void *data, netadr_t to )
{
	int i;
	loopback_t *loop;
	int port;

	port = 0;

	if ( sock < NS_SERVER )
	{
		port = sock;
		sock = NS_SERVER;
	}
	else if ( sock == NS_SERVER )
	{
		sock = to.port;
	}

	loop = &loopbacks[sock];

	i = loop->send & ( MAX_LOOPBACK - 1 );
	loop->send++;

	memcpy(&loop->msgs[i], data, length);

	loop->msgs[i].datalen = length;
	loop->msgs[i].port = port;
}

/*
=============
NET_SendPacket
=============
*/
bool NET_SendPacket( netsrc_t sock, int length, const void *data, netadr_t to )
{
	// sequenced packets are shown in netchan, so just show oob
	if ( showpackets->current.boolean && *(int *)data == -1 )
	{
		Com_Printf("[client %i] send packet %4i\n", Net_LocalClientNum(), length);
	}

	if ( to.type == NA_LOOPBACK )
	{
		NET_SendLoopPacket(sock, length, data, to);
		return qtrue;
	}

	if ( to.type == NA_BAD )
	{
		return qfalse;
	}

	if ( to.type == NA_BOT )
	{
		return qfalse;
	}

	return Sys_SendPacket( length, data, to );
}

/*
===============
NET_OutOfBandPrint

Sends a text message in an out-of-band datagram
================
*/
unsigned char NET_OutOfBandPrint( netsrc_t sock, netadr_t adr, const char *data )
{
	int len;
	int res;
	LargeLocal stringLarge( MAX_MSGLEN );
	char *string = (char *)stringLarge.GetBuf();

	// set the header
	string[0] = -1;
	string[1] = -1;
	string[2] = -1;
	string[3] = -1;

	if ( showpackets->current.boolean )
		Com_DPrintf("OOB Print: %s\n", data);

	if ( strlen(data) + 1 > MAX_MSGLEN - 4 )
	{
		Com_DPrintf("OOB Packet is %i bytes - too large to send\n", strlen(data));
		return qfalse;
	}

	memcpy(string + 4, data, strlen(data) + 1);
	len = strlen(string);
	res = NET_SendPacket(sock, len, string, adr);

	if ( sock == NS_SERVER )
		SV_Netchan_AddOOBProfilePacket(len);
	else
		CL_Netchan_AddOOBProfilePacket(len);

	return res > 0;
}

/*
===============
NET_OutOfBandData

Sends a data message in an out-of-band datagram (only used for "connect")
================
*/
unsigned char NET_OutOfBandData( netsrc_t sock, netadr_t adr, byte *format, int len )
{
	int i;
	msg_t mbuf;
	int res;
	LargeLocal stringLarge( MAX_MSGLEN );
	byte *string = (byte *)stringLarge.GetBuf();

	// set the header
	string[0] = 0xff;
	string[1] = 0xff;
	string[2] = 0xff;
	string[3] = 0xff;

	for ( i = 0; i < len; i++ )
	{
		string[i + 4] = format[i];
	}

	mbuf.data = string;
	mbuf.cursize = len + 4;
	// send the datagram
	res = NET_SendPacket(sock, mbuf.cursize, mbuf.data, adr);

	if ( sock == NS_SERVER )
		SV_Netchan_AddOOBProfilePacket(mbuf.cursize);
	else
		CL_Netchan_AddOOBProfilePacket(mbuf.cursize);

	return res > 0;
}

/*
===============
NET_OutOfBandVoiceData
================
*/
void NET_OutOfBandVoiceData( netsrc_t sock, netadr_t adr, byte *format, int len )
{
	byte string[32768];
	byte pad[16];
	int iLen;
	int pad2;
	byte *pString;

	// set the header
	string[0] = 0xff;
	string[1] = 0xff;
	string[2] = 0xff;
	string[3] = 0xff;

	memcpy(string + 4, format, len);

	pString = string;
	iLen = len + 4;

	NET_SendPacket(sock, iLen, pString, adr);

	if ( sock == NS_SERVER )
		SV_Netchan_AddOOBProfilePacket(iLen);
	else
		CL_Netchan_AddOOBProfilePacket(iLen);
}

/*
=============
NET_StringToAdr

Traps "localhost" for loopback, passes everything else to system
=============
*/
qboolean NET_StringToAdr( const char *s, netadr_t *a )
{
	qboolean r;
	char base[MAX_STRING_CHARS];
	char    *port;

	if ( !strcmp( s, "localhost" ) )
	{
		memset( a, 0, sizeof( *a ) );
		a->type = NA_LOOPBACK;
		return qtrue;
	}

	// look for a port number
	Q_strncpyz( base, s, sizeof( base ) );
	port = strstr( base, ":" );
	if ( port )
	{
		*port = 0;
		port++;
	}

	r = Sys_StringToAdr( base, a );

	if ( !r )
	{
		a->type = NA_BAD;
		return qfalse;
	}

	// inet_addr returns this if out of range
	if ( a->ip[0] == 255 && a->ip[1] == 255 && a->ip[2] == 255 && a->ip[3] == 255 )
	{
		a->type = NA_BAD;
		return qfalse;
	}

	if ( port )
	{
		a->port = BigShort( (short)atoi( port ) );
	}
	else
	{
		a->port = BigShort( PORT_SERVER );
	}

	return qtrue;
}

