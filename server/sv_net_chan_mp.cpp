#include "../qcommon/qcommon.h"
#include "../qcommon/netchan.h"

void CL_DrawString( int x, int y, const char *pszString, int bShadow, int iCharHeight );

void SV_Netchan_Encode( client_t *client, byte *data, int cursize );
void SV_Netchan_Decode( client_t *client, byte *data, int remaining );
bool SV_Netchan_TransmitNextFragment( netchan_t *chan );
bool SV_Netchan_Transmit( client_t *client, byte *data, int length );
void SV_Netchan_AddOOBProfilePacket( int iLength );
void SV_Netchan_SendOOBPacket( int iLength, const void *pData, netadr_t to );
void SV_Netchan_UpdateProfileStats();
void SV_Netchan_PrintProfileStats( int bPrintToConsole );

/*
==============
SV_Netchan_Encode

	// first four bytes of the data are always:
	long reliableAcknowledge;

==============
*/
void SV_Netchan_Encode( client_t *client, byte *data, int cursize )
{
	int i;
	int index;
	byte key;
	byte *string;

	string = (byte *)client->lastClientCommandString;
	index = 0;
	// xor the client challenge with the netchan sequence number
	key = client->challenge ^ client->netchan.outgoingSequence;

	for ( i = 0; i < cursize; i++, data++ )
	{
		if ( !string[index] )
		{
			index = 0;
		}

		// modify the key with the last sent and acknowledged server command
		key ^= string[index] << ( i & 1 );
		index++;
		*data ^= key;
	}
}

/*
==============
SV_Netchan_Decode

	// first 12 bytes of the data are always:
	long serverId;
	long messageAcknowledge;
	long reliableAcknowledge;

==============
*/
void SV_Netchan_Decode( client_t *client, byte *data, int remaining )
{
	int i;
	int index;
	byte key;
	byte *string;

	string = (byte *)client->reliableCommandInfo[ client->reliableAcknowledge & ( MAX_RELIABLE_COMMANDS - 1 ) ].cmd;
	index = 0;
	key = client->challenge ^ (byte)client->serverId ^ client->messageAcknowledge;

	for ( i = 0; i < remaining; i++, data++ )
	{
		if ( !string[index] )
		{
			index = 0;
		}

		// modify the key with the last sent and acknowledged server command
		key ^= string[index] << ( i & 1 );
		index++;
		*data ^= key;
	}
}

/*
==================
SV_Netchan_TransmitNextFragment
==================
*/
bool SV_Netchan_TransmitNextFragment( netchan_t *chan )
{
	return Netchan_TransmitNextFragment(chan);
}

/*
==================
SV_Netchan_Transmit
==================
*/
bool SV_Netchan_Transmit( client_t *client, byte *data, int length )
{
	SV_Netchan_Encode(client, data + SV_ENCODE_START, length - SV_ENCODE_START);
	return Netchan_Transmit(&client->netchan, length, data);
}

/*
==================
SV_Netchan_AddOOBProfilePacket
==================
*/
void SV_Netchan_AddOOBProfilePacket( int iLength )
{
	if ( net_profile->current.integer )
	{
		NetProf_PrepProfiling(&svs.pOOBProf);
		NetProf_AddPacket(&svs.pOOBProf->send, iLength, 0);
	}
}

/*
==================
SV_Netchan_SendOOBPacket
==================
*/
void SV_Netchan_SendOOBPacket( int iLength, const void *pData, netadr_t to )
{
	if ( *(int *)pData != -1 )
		Com_Printf("SV_Netchan_SendOOBPacket used to send non-OOB packet.\n");

	NetProf_PrepProfiling(&svs.pOOBProf);
	NET_SendPacket(NS_SERVER, iLength, pData, to);
	SV_Netchan_AddOOBProfilePacket(iLength);
}

/*
==================
SV_Netchan_UpdateProfileStats
==================
*/
void SV_Netchan_UpdateProfileStats()
{
	int i;
	client_t *cl;

	if ( !svs.clients )
		return;

	if ( svs.pOOBProf )
	{
		NetProf_UpdateStatistics(&svs.pOOBProf->send);
		NetProf_UpdateStatistics(&svs.pOOBProf->recieve);
	}

	for (i = 0, cl = svs.clients; i < sv_maxclients->current.integer; ++i, ++cl )
	{
		if ( !cl->state )
			continue;

		if ( !cl->netchan.pProf )
			continue;

		NetProf_UpdateStatistics(&cl->netchan.pProf->send);
		NetProf_UpdateStatistics(&cl->netchan.pProf->recieve);
	}
}

// Original name unknown.
static void SV_Netchan_DrawProfileLine( const char *line, int y )
{
	CL_DrawString(32, y, line, 0, 8);
}

/*
==================
SV_Netchan_PrintProfileStats
==================
*/
void SV_Netchan_PrintProfileStats( int bPrintToConsole )
{
	int i;
	int iTotalPacketsSent = 0;
	int iTotalBPSSent = 0;
	int iTotalFragmentsSent = 0;
	int iTotalPacketsRecieved = 0;
	int iTotalFragmentsRecieved = 0;
	int iTotalMaxSent = 0;
	int iTotalMinSent = 9999;
	int iTotalMaxRecieved = 0;
	int iTotalMinRecieved = 9999;
	int iYPos = 80;
	int iFragmentTotal;
	netProfileInfo_t *pProf;
	client_t *cl;
	char szClientName[32];
	char szLine[1024];
	int iTotalBPSRecieved = 0;

	if ( !svs.clients )
	{
		return;
	}

	SV_Netchan_UpdateProfileStats();

	if ( bPrintToConsole )
	{
		Com_Printf("\n\n");
	}

	Com_sprintf(szLine, 1024, "====================");

	if ( bPrintToConsole )
	{
		Com_Printf("%s\n", szLine);
	}
	else
	{
		iYPos = 90;
		SV_Netchan_DrawProfileLine(szLine, iYPos);
	}

	Com_sprintf(szLine, 1024, "Server Network Profile:");

	if ( bPrintToConsole )
	{
		Com_Printf("%s\n\n", szLine);
	}
	else
	{
		iYPos += 10;
		SV_Netchan_DrawProfileLine(szLine, iYPos);
		iYPos += 10;
	}

	Com_sprintf(szLine, 1024, "                    | Sent To                | Recieved From          | Total Source Traffic   |");

	if ( bPrintToConsole )
	{
		Com_Printf("%s\n", szLine);
	}
	else
	{
		iYPos += 10;
		SV_Netchan_DrawProfileLine(szLine, iYPos);
	}

	Com_sprintf(szLine, 1024, "              Source|   bps|  max|  min|frag%%|   bps|  max|  min|frag%%|   bps|  max|  min|frag%%|");

	if ( bPrintToConsole )
	{
		Com_Printf("%s\n", szLine);
	}
	else
	{
		iYPos += 10;
		SV_Netchan_DrawProfileLine(szLine, iYPos);
	}

	if ( svs.pOOBProf )
	{
		pProf = svs.pOOBProf;

		iTotalBPSSent += pProf->send.iBytesPerSecond;
		iTotalPacketsSent += pProf->send.iCountedPackets;
		iTotalFragmentsSent += pProf->send.iCountedFragments;
		iTotalBPSRecieved += pProf->recieve.iBytesPerSecond;
		iTotalPacketsRecieved += pProf->recieve.iCountedPackets;
		iTotalFragmentsRecieved += pProf->recieve.iCountedFragments;

		if ( pProf->send.iLargestPacket > iTotalMaxSent )
			iTotalMaxSent = pProf->send.iLargestPacket;
		if ( pProf->send.iSmallestPacket < iTotalMinSent )
			iTotalMinSent = pProf->send.iSmallestPacket;
		if ( pProf->recieve.iLargestPacket > iTotalMaxRecieved )
			iTotalMaxRecieved = pProf->recieve.iLargestPacket;
		if ( pProf->recieve.iSmallestPacket < iTotalMinRecieved )
			iTotalMinRecieved = pProf->recieve.iSmallestPacket;
	}

	for ( i = 0, cl = svs.clients; i < sv_maxclients->current.integer; i++, cl++ )
	{
		if ( !cl->state )
			continue;

		if ( !cl->netchan.pProf )
			continue;

		pProf = cl->netchan.pProf;

		iTotalBPSSent += pProf->send.iBytesPerSecond;
		iTotalPacketsSent += pProf->send.iCountedPackets;
		iTotalFragmentsSent += pProf->send.iCountedFragments;
		iTotalBPSRecieved += pProf->recieve.iBytesPerSecond;
		iTotalPacketsRecieved += pProf->recieve.iCountedPackets;
		iTotalFragmentsRecieved += pProf->recieve.iCountedFragments;

		if ( pProf->send.iLargestPacket > iTotalMaxSent )
			iTotalMaxSent = pProf->send.iLargestPacket;
		if ( pProf->send.iSmallestPacket < iTotalMinSent )
			iTotalMinSent = pProf->send.iSmallestPacket;
		if ( pProf->recieve.iLargestPacket > iTotalMaxRecieved )
			iTotalMaxRecieved = pProf->recieve.iLargestPacket;
		if ( pProf->recieve.iSmallestPacket < iTotalMinRecieved )
			iTotalMinRecieved = pProf->recieve.iSmallestPacket;
	}

	if ( iTotalPacketsSent + iTotalPacketsRecieved > 0
		&& iTotalFragmentsSent + iTotalFragmentsRecieved > 0 )
	{
		iFragmentTotal = 100 * ( iTotalFragmentsSent + iTotalFragmentsRecieved ) / ( iTotalPacketsSent + iTotalPacketsRecieved );
	}
	else
	{
		iFragmentTotal = 0;
	}

	Com_sprintf(szLine, 1024, "              Totals:%6i|%5i|%5i| %3i%%|%6i|%5i|%5i| %3i%%|%6i|%5i|%5i| %3i%%|",
		iTotalBPSSent,
		iTotalMaxSent,
		iTotalMinSent,
		iTotalPacketsSent ? 100 * iTotalFragmentsSent / iTotalPacketsSent : 0,
		iTotalBPSRecieved,
		iTotalMaxRecieved,
		iTotalMinRecieved,
		iTotalPacketsRecieved ? 100 * iTotalFragmentsRecieved / iTotalPacketsRecieved : 0,
		iTotalBPSSent + iTotalBPSRecieved,
		(double)I_fmax( (float)iTotalMaxSent, (float)iTotalMaxRecieved ),
		(double)I_fmin( (float)iTotalMinSent, (float)iTotalMinRecieved ),
		iFragmentTotal );

	if ( bPrintToConsole )
	{
		Com_Printf("%s\n", szLine);
	}
	else
	{
		iYPos += 10;
		SV_Netchan_DrawProfileLine(szLine, iYPos);
	}

	if ( svs.pOOBProf )
	{
		pProf = svs.pOOBProf;

		if ( pProf->send.iCountedPackets + pProf->recieve.iCountedPackets > 0
			&& pProf->send.iCountedFragments + pProf->recieve.iCountedFragments > 0 )
		{
			iFragmentTotal = 100 * ( pProf->send.iCountedFragments + pProf->recieve.iCountedFragments ) / ( pProf->send.iCountedPackets + pProf->recieve.iCountedPackets );
		}
		else
		{
			iFragmentTotal = 0;
		}

		Com_sprintf(szLine, 1024, "  OutOfBand Messages: %5i|%5i|%5i| %3i%%| %5i|%5i|%5i| %3i%%| %5i|%5i|%5i| %3i%%|",
			pProf->send.iBytesPerSecond,
			pProf->send.iLargestPacket,
			pProf->send.iSmallestPacket,
			pProf->send.iFragmentPercentage,
			pProf->recieve.iBytesPerSecond,
			pProf->recieve.iLargestPacket,
			pProf->recieve.iSmallestPacket,
			pProf->recieve.iFragmentPercentage,
			pProf->send.iBytesPerSecond + pProf->recieve.iBytesPerSecond,
			(double)I_fmax( (float)pProf->send.iLargestPacket, (float)pProf->recieve.iLargestPacket ),
			(double)I_fmin( (float)pProf->send.iSmallestPacket, (float)pProf->recieve.iSmallestPacket ),
			iFragmentTotal );

		if ( bPrintToConsole )
		{
			Com_Printf("%s\n", szLine);
		}
		else
		{
			iYPos += 10;
			SV_Netchan_DrawProfileLine(szLine, iYPos);
		}
	}
	else
	{
		Com_sprintf(szLine, 1024, "  OutOfBand Messages:     0|    0|    0|   - |     0|    0|    0|   - |     0|    0|    0|   - |");

		if ( bPrintToConsole )
		{
			Com_Printf("%s\n", szLine);
		}
		else
		{
			iYPos += 10;
			SV_Netchan_DrawProfileLine(szLine, iYPos);
		}
	}

	for ( i = 0, cl = svs.clients; i < sv_maxclients->current.integer; i++, cl++ )
	{
		if ( !cl->state )
			continue;

		strncpy( szClientName, cl->name, 17 );
		szClientName[16] = 0;

		pProf = cl->netchan.pProf;

		if ( pProf )
		{
			if ( pProf->send.iCountedPackets + pProf->recieve.iCountedPackets > 0
				&& pProf->send.iCountedFragments + pProf->recieve.iCountedFragments > 0 )
			{
				iFragmentTotal = 100 * ( pProf->send.iCountedFragments + pProf->recieve.iCountedFragments ) / ( pProf->send.iCountedPackets + pProf->recieve.iCountedPackets );
			}
			else
			{
				iFragmentTotal = 0;
			}

			Com_sprintf(szLine, 1024, "#%2i-%16s: %5i|%5i|%5i| %3i%%| %5i|%5i|%5i| %3i%%| %5i|%5i|%5i| %3i%%|",
				i,
				szClientName,
				pProf->send.iBytesPerSecond,
				pProf->send.iLargestPacket,
				pProf->send.iSmallestPacket,
				pProf->send.iFragmentPercentage,
				pProf->recieve.iBytesPerSecond,
				pProf->recieve.iLargestPacket,
				pProf->recieve.iSmallestPacket,
				pProf->recieve.iFragmentPercentage,
				pProf->send.iBytesPerSecond + pProf->recieve.iBytesPerSecond,
				(double)I_fmax( (float)pProf->send.iLargestPacket, (float)pProf->recieve.iLargestPacket ),
				(double)I_fmin( (float)pProf->send.iSmallestPacket, (float)pProf->recieve.iSmallestPacket ),
				iFragmentTotal );

			if ( bPrintToConsole )
			{
				Com_Printf("%s\n", szLine);
			}
			else
			{
				iYPos += 10;
				SV_Netchan_DrawProfileLine(szLine, iYPos);
			}
		}
		else
		{
			Com_sprintf(szLine, 1024, "#%2i-%16s:     0|    0|    0|   0%%|     0|    0|    0|   0%%|     0|    0|    0|   0%%|",
				i,
				szClientName );

			if ( bPrintToConsole )
			{
				Com_Printf("%s\n", szLine);
			}
			else
			{
				iYPos += 10;
				SV_Netchan_DrawProfileLine(szLine, iYPos);
			}
		}
	}
}

