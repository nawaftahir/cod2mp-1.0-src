#include "../qcommon/qcommon.h"


/*
===============
SV_GetClientPositionsAtTime
===============
*/
// The frame biases are initialized data, not constants.

/*
====================
SV_RateMsec
Return the number of msec a given size message is supposed
to take to clear, based on the current rate
TTimo - use sv_maxRate or sv_dl_maxRate depending on regular or downloading client
====================
*/
#define HEADER_RATE_BYTES   48      // include our header, IP header, and some overhead

/*
=============
SV_BuildClientSnapshot

Decides which entities are going to be visible to the client, and
copies off the playerstate and areabits.

This properly handles multiple recursive portals, but the render
currently doesn't.

For viewing through other player's eyes, clent can be something other than client->gentity
=============
*/

cachedSnapshot_t* SV_GetCachedSnapshot( int *pArchiveTime );
qboolean SV_GetArchivedClientInfo( int clientNum, int *pArchiveTime, playerState_t *ps, clientState_t *cs );
void SV_SendClientSnapshot( client_t *client );

void SV_EmitPacketEntities( int entNum, int from_num_entities, int from_first_entity, int to_num_entities, int to_first_entity, msg_t *msg );
void SV_EmitPacketClients( int clientNum, int from_num_clients, int from_first_client, int to_num_clients, int to_first_client, msg_t *msg );
void SV_UpdateServerCommandsToClient( client_t *client, msg_t *msg );
void SV_UpdateServerCommandsToClient_PreventOverflow( client_t *client, msg_t *msg, int iMsgSize );
void SV_ShowClientUnAckCommands( client_t *client );
void SV_AddEntToSnapshot( int e, snapshotEntityNumbers_t *eNums );
void SV_AddArchivedEntToSnapshot( int e, snapshotEntityNumbers_t *eNums );
void SV_AddEntitiesVisibleFromPoint( vec3_t origin, int clientNum, snapshotEntityNumbers_t *eNums );
void SV_AddCachedEntitiesVisibleFromPoint( int from_num_entities, int from_first_entity, vec3_t origin, int clientNum, snapshotEntityNumbers_t *eNums, playerState_t *ps );
cachedSnapshot_t* SV_GetCachedSnapshotInternal( int archivedFrame );
cachedSnapshot_t* SV_GetCachedSnapshot( int *pArchiveTime );
qboolean SV_GetCurrentClientInfo( int clientNum, playerState_t *ps, clientState_t *cs );
qboolean SV_GetArchivedClientInfo( int clientNum, int *pArchiveTime, playerState_t *ps, clientState_t *cs );
void SV_SendMessageToClient( msg_t *msg, client_t *client );
void SV_SendClientSnapshot( client_t *client );
void SV_ArchiveSnapshot();
void SV_SendClientMessages( void );

/*
=============
SV_EmitPacketEntities

Writes a delta update of an entityState_t list to the message.
=============
*/
void SV_EmitPacketEntities( int entNum, int from_num_entities, int from_first_entity, int to_num_entities, int to_first_entity, msg_t *msg )
{
	entityState_t *oldent;
	entityState_t *newent;
	int oldindex;
	int newindex;
	int oldnum;
	int newnum;

	newent = NULL;
	oldent = NULL;

	newindex = 0;
	oldindex = 0;

	while ( newindex < to_num_entities || oldindex < from_num_entities )
	{
		if ( newindex >= to_num_entities )
		{
			newnum = 9999;
		}
		else
		{
			newent = &svs.snapshotEntities[(to_first_entity + newindex) % svs.numSnapshotEntities];
			newnum = newent->number;
		}

		if ( oldindex >= from_num_entities )
		{
			oldnum = 9999;
		}
		else
		{
			oldent = &svs.snapshotEntities[(from_first_entity + oldindex) % svs.numSnapshotEntities];
			oldnum = oldent->number;
		}

		if ( newnum == oldnum )
		{
			// delta update from old position
			// because the force parm is qfalse, this will not result
			// in any bytes being emited if the entity has not changed at all
			MSG_WriteDeltaEntity(msg, oldent, newent, qfalse);
			++oldindex;
			++newindex;
			continue;
		}

		if ( newnum < oldnum )
		{
			// this is a new entity, send it from the baseline
			MSG_WriteDeltaEntity(msg, &sv.svEntities[newnum].baseline.s, newent, qtrue);
			++newindex;
			continue;
		}

		if ( newnum > oldnum )
		{
			// the old entity isn't present in the new message
			MSG_WriteDeltaEntity(msg, oldent, NULL, qtrue);
			++oldindex;
			continue;
		}
	}

	MSG_WriteBits( msg, ( MAX_GENTITIES - 1 ), GENTITYNUM_BITS );   // end of packetentities
}

/*
===============
SV_EmitPacketClients
===============
*/
void SV_EmitPacketClients( int clientNum, int from_num_clients, int from_first_client, int to_num_clients, int to_first_client, msg_t *msg )
{
	clientState_t *oldclient;
	clientState_t *newclient;
	int oldindex;
	int newindex;
	int oldnum;
	int newnum;

	newclient = NULL;
	oldclient = NULL;

	newindex = 0;
	oldindex = 0;

	while ( newindex < to_num_clients || oldindex < from_num_clients )
	{
		if ( newindex >= to_num_clients )
		{
			newnum = 9999;
		}
		else
		{
			newclient = &svs.snapshotClients[(to_first_client + newindex) % svs.numSnapshotClients];
			newnum = newclient->clientIndex;
		}

		if ( oldindex >= from_num_clients )
		{
			oldnum = 9999;
		}
		else
		{
			oldclient = &svs.snapshotClients[(from_first_client + oldindex) % svs.numSnapshotClients];
			oldnum = oldclient->clientIndex;
		}

		if ( newnum == oldnum )
		{
			// delta update from old position
			// because the force parm is qfalse, this will not result
			// in any bytes being emited if the entity has not changed at all
			MSG_WriteDeltaClient(msg, oldclient, newclient, qfalse);
			++oldindex;
			++newindex;
			continue;
		}

		if ( newnum < oldnum )
		{
			// this is a new entity, send it from the baseline
			MSG_WriteDeltaClient(msg, NULL, newclient, qtrue);
			++newindex;
			continue;
		}

		if ( newnum > oldnum )
		{
			// the old entity isn't present in the new message
			MSG_WriteDeltaClient(msg, oldclient, NULL, qtrue);
			++oldindex;
			continue;
		}
	}

	MSG_WriteBit0( msg );
}

void SV_WriteSnapshotToClient( client_t *client, msg_t *msg )
{
	clientSnapshot_t *frame;
	clientSnapshot_t *oldframe;
	int lastframe;
	int i;
	int snapFlags;
	int from_num_entities;
	int from_first_entity;
	int from_num_clients;
	int from_first_client;

	frame = &client->frames[client->netchan.outgoingSequence & PACKET_MASK];

	if ( client->deltaMessage <= 0 || client->state != CS_ACTIVE )
	{
		oldframe = NULL;
		lastframe = 0;
	}
	else if ( client->netchan.outgoingSequence - client->deltaMessage >= PACKET_BACKUP - 3 )
	{
		Com_DPrintf("%s: Delta request from out of date packet.\n", client->name);
		oldframe = NULL;
		lastframe = 0;
	}
	else
	{
		oldframe = &client->frames[client->deltaMessage & PACKET_MASK];
		lastframe = client->netchan.outgoingSequence - client->deltaMessage;

		if ( oldframe->first_entity < svs.nextSnapshotEntities - svs.numSnapshotEntities )
		{
			Com_DPrintf("%s: Delta request from out of date entities.\n", client->name);
			oldframe = NULL;
			lastframe = 0;
		}
	}

	MSG_WriteByte(msg, svc_snapshot);
	MSG_WriteLong(msg, svs.time);
	MSG_WriteByte(msg, lastframe);

	snapFlags = svs.snapFlagServerBit;

	if ( client->rateDelayed )
	{
		snapFlags |= SNAPFLAG_RATE_DELAYED;
	}

	if ( client->state == CS_ACTIVE )
	{
		client->sendAsActive = qtrue;
	}
	else if ( client->state != CS_ZOMBIE )
	{
		client->sendAsActive = qfalse;
	}

	if ( !client->sendAsActive )
	{
		snapFlags |= SNAPFLAG_NOT_ACTIVE;
	}

	MSG_WriteByte(msg, snapFlags);

	if ( oldframe )
	{
		MSG_WriteDeltaPlayerstate(msg, &oldframe->ps, &frame->ps);
		from_num_entities = oldframe->num_entities;
		from_first_entity = oldframe->first_entity;
		from_num_clients = oldframe->num_clients;
		from_first_client = oldframe->first_client;
	}
	else
	{
		MSG_WriteDeltaPlayerstate(msg, NULL, &frame->ps);
		from_num_entities = 0;
		from_first_entity = 0;
		from_num_clients = 0;
		from_first_client = 0;
	}

	SV_EmitPacketEntities(client - svs.clients, from_num_entities, from_first_entity, frame->num_entities, frame->first_entity, msg);
	SV_EmitPacketClients(client - svs.clients, from_num_clients, from_first_client, frame->num_clients, frame->first_client, msg);

	for ( i = 0; i < sv_padPackets->current.integer; i++ )
	{
		MSG_WriteByte(msg, svc_nop);
	}
}

/*
==================
SV_UpdateServerCommandsToClient
(re)send all server commands the client hasn't acknowledged yet
==================
*/
void SV_UpdateServerCommandsToClient( client_t *client, msg_t *msg )
{
	int i;

	assert(client - svs.clients >= 0 && client - svs.clients < MAX_CLIENTS);

	if ( client->reliableAcknowledge + 1 < client->reliableSequence && sv_debugReliableCmds->current.boolean )
		Com_Printf("Client %s has the following un-ack'd reliable commands:\n", client->name);

	// write any unacknowledged serverCommands
	for ( i = client->reliableAcknowledge + 1 ; i <= client->reliableSequence ; i++ )
	{
		MSG_WriteByte( msg, svc_serverCommand );
		MSG_WriteLong( msg, i );
		MSG_WriteString( msg, client->reliableCommandInfo[ i & ( MAX_RELIABLE_COMMANDS - 1 ) ].cmd );

		if ( sv_debugReliableCmds->current.boolean )
			Com_Printf("%i: %s\n", i - client->reliableAcknowledge - 1, client->reliableCommandInfo[ i & ( MAX_RELIABLE_COMMANDS - 1 ) ].cmd);
	}

	client->reliableSent = client->reliableSequence;
}

/*
===============
SV_UpdateServerCommandsToClient_PreventOverflow
===============
*/
void SV_UpdateServerCommandsToClient_PreventOverflow( client_t *client, msg_t *msg, int iMsgSize )
{
	int i;
	int cmdlen;

	for ( i = client->reliableAcknowledge + 1; i <= client->reliableSequence; i++ )
	{
		cmdlen = strlen(client->reliableCommandInfo[i & ( MAX_RELIABLE_COMMANDS - 1 )].cmd) + 6;

		if ( msg->cursize + cmdlen >= iMsgSize )
			break;

		MSG_WriteByte(msg, svc_serverCommand);
		MSG_WriteLong(msg, i);
		MSG_WriteString(msg, client->reliableCommandInfo[i & ( MAX_RELIABLE_COMMANDS - 1 )].cmd);
	}

	--i;

	if ( i > client->reliableSent )
		client->reliableSent = i;
}

/*
===============
SV_ShowClientUnAckCommands
===============
*/
void SV_ShowClientUnAckCommands( client_t *client )
{
	int i;

	Com_Printf("-- Unacknowledged Server Commands for client %i:%s --\n", client - svs.clients, client->name);

	for ( i = client->reliableAcknowledge + 1; i <= client->reliableSequence; ++i )
	{
		Com_Printf("cmd %5d: %8d: %s\n", i, client->reliableCommandInfo[i & (MAX_RELIABLE_COMMANDS -1)].time,
		           client->reliableCommandInfo[i & (MAX_RELIABLE_COMMANDS -1)].cmd );
	}

	Com_Printf("----------");
}

/*
===============
SV_AddEntToSnapshot
===============
*/
void SV_AddEntToSnapshot( int e, snapshotEntityNumbers_t *eNums )
{
	// if we are full, silently discard entities
	if ( eNums->numSnapshotEntities == MAX_SNAPSHOT_ENTITIES )
	{
		return;
	}

	eNums->snapshotEntities[ eNums->numSnapshotEntities ] = e;
	eNums->numSnapshotEntities++;
}

/*
===============
SV_AddArchivedEntToSnapshot
===============
*/
void SV_AddArchivedEntToSnapshot( int e, snapshotEntityNumbers_t *eNums )
{
	// if we are full, silently discard entities
	if ( eNums->numSnapshotEntities == MAX_SNAPSHOT_ENTITIES )
	{
		return;
	}

	eNums->snapshotEntities[ eNums->numSnapshotEntities ] = e;
	eNums->numSnapshotEntities++;
}

/*
===============
SV_AddEntitiesVisibleFromPoint
===============
*/
void SV_AddEntitiesVisibleFromPoint( vec3_t origin, int clientNum, snapshotEntityNumbers_t *eNums )
{
	int e, i;
	gentity_t *ent;
	svEntity_t  *svEnt;
	int l;
	int clientcluster;
	byte    *clientpvs;
	byte    *bitvector;
	int leafnum;
	float fogOpaqueDistSqrd;

	assert(SV_Loaded());

	leafnum = CM_PointLeafnum( origin );
	clientcluster = CM_LeafCluster( leafnum );

	if (clientcluster < 0 )
	{
		return;
	}

	clientpvs = CM_ClusterPVS( clientcluster );
	fogOpaqueDistSqrd = G_GetFogOpaqueDistSqrd();

	if ( fogOpaqueDistSqrd == FLT_MAX )
	{
		fogOpaqueDistSqrd = 0;
	}

	for ( e = 0 ; e < sv.num_entities ; e++ )
	{
		ent = SV_GentityNum( e );

		// never send entities that aren't linked in.
		// never send client's own entity, because it can
		// be regenerated from the playerstate
		if ( !ent->r.linked )
		{
			continue;
		}

		assert(ent->s.number == e);

		if ( e == clientNum )
		{
			continue;
		}

		if ( !ent->r.broadcastTime )
		{
			// entities can be flagged to explicitly not be sent to the client
			if ( ent->r.svFlags & SVF_NOCLIENT )
			{
				continue;
			}

			if ( (1 << (clientNum & 31)) & ent->r.clientMask[clientNum >> 5] )
			{
				continue;
			}
		}
		else
		{
			if ( ent->r.broadcastTime < 0 || ent->r.broadcastTime - svs.time >= 0 )
			{
				SV_AddEntToSnapshot( e, eNums );
				continue;
			}

			ent->r.broadcastTime = 0;
		}

		if ( ent->r.svFlags & ( SVF_BROADCAST | SVF_OBJECTIVE ) )
		{
			SV_AddEntToSnapshot( e, eNums );
			continue;
		}

		svEnt = SV_SvEntityForGentity( ent );

		// check individual leafs
		if ( !svEnt->numClusters )
		{
			continue;
		}

		{
			bitvector = clientpvs;
			l = 0;

			for ( i = 0 ; i < svEnt->numClusters ; i++ )
			{
				l = svEnt->clusternums[i];

				if ( bitvector[l >> 3] & ( 1 << ( l & 7 ) ) )
				{
					break;
				}
			}

			// if we haven't found it to be visible,
			// check overflow clusters that coudln't be stored
			if ( i == svEnt->numClusters )
			{
				if ( !svEnt->lastCluster )
				{
					continue;
				}

				for ( ; l <= svEnt->lastCluster ; l++ )
				{
					if ( bitvector[l >> 3] & ( 1 << ( l & 7 ) ) )
					{
						break;
					}
				}

				if ( l == svEnt->lastCluster )
				{
					continue;    // not visible
				}
			}

			if ( fogOpaqueDistSqrd != 0)
			{
				if ( BoxDistSqrdExceeds(ent->r.absmin, ent->r.absmax, origin, fogOpaqueDistSqrd) )
				{
					continue;
				}
			}

			// add it
			SV_AddEntToSnapshot( e, eNums );
		}
	}
}

/*
===============
SV_AddCachedEntitiesVisibleFromPoint
===============
*/
void SV_AddCachedEntitiesVisibleFromPoint( int from_num_entities, int from_first_entity, vec3_t origin, int clientNum, snapshotEntityNumbers_t *eNums, playerState_t *ps )
{
	int i, j;
	archivedEntity_t *ent;
	int clientcluster;
	byte    *clientpvs;
	int boxleafnums;
	int clusternums[MAX_TOTAL_ENT_LEAFS];
	int lastLeaf;
	int l;
	int leafnum;
	float fogOpaqueDistSqrd;

	assert(SV_Loaded());

	leafnum = CM_PointLeafnum( origin );
	clientcluster = CM_LeafCluster( leafnum );

	if ( clientcluster < 0 )
	{
		return;
	}

	clientpvs = CM_ClusterPVS( clientcluster );
	fogOpaqueDistSqrd = G_GetFogOpaqueDistSqrd();

	if ( fogOpaqueDistSqrd == FLT_MAX )
	{
		fogOpaqueDistSqrd = 0;
	}

	for ( i = 0 ; i < from_num_entities ; i++ )
	{
		ent = &svs.cachedSnapshotEntities[(from_first_entity + i) % CACHED_SNAPSHOT_ENTITY_SIZE];
		//assert(!entityIndex[ent->s.number]);

		if ( (1 << (clientNum & 31)) & ent->r.clientMask[clientNum >> 5] )
		{
			continue;
		}

		if ( ent->s.number == clientNum )
		{
			continue;
		}

		{
			if ( ent->r.svFlags & ( SVF_BROADCAST | SVF_OBJECTIVE ) )
			{
				SV_AddArchivedEntToSnapshot( i, eNums );
				continue;
			}

			boxleafnums = CM_BoxLeafnums(ent->r.absmin, ent->r.absmax, clusternums, sizeof(clusternums) / sizeof(clusternums[0]), &lastLeaf);

			if ( !boxleafnums )
			{
				continue;
			}

			for ( j = 0 ; j < boxleafnums ; j++ )
			{
				l = CM_LeafCluster(clusternums[j]);

				if ( l != -1 && ( clientpvs[l >> 3] & ( 1 << ( l & 7 ) ) ) )
				{
					break;
				}
			}

			if ( j == boxleafnums )
			{
				continue;
			}

			if ( fogOpaqueDistSqrd != 0 && BoxDistSqrdExceeds(ent->r.absmin, ent->r.absmax, origin, fogOpaqueDistSqrd) )
			{
				continue;
			}

			// add it
			SV_AddArchivedEntToSnapshot( i, eNums );
		}
	}
}

/*
=============
SV_GetCachedSnapshotInternal
=============
*/
cachedSnapshot_t* SV_GetCachedSnapshotInternal( int archivedFrame )
{
	cachedSnapshot_t *cachedFrame;
	cachedSnapshot_t *oldCachedFrame;
	archivedEntity_t *cachedEntity;
	cachedClient_t *cachedClient;
	cachedClient_t *oldCachedClient;
	int i;
	int newnum;
	archivedSnapshot_t *frame;
	int startIndex;
	int partSize;
	int firstCachedSnapshotFrame;
	int oldArchivedFrame;
	int oldindex;
	int oldnum;
	msg_t msg;
	LargeLocal msgLarge( MAX_SNAPSHOT_MSG_LEN );
	byte *msg_buf = (byte *)msgLarge.GetBuf();

	frame = &svs.archivedSnapshotFrames[archivedFrame % NUM_ARCHIVED_FRAMES];
	assert(frame->size);

	if ( frame->start < svs.nextArchivedSnapshotBuffer - ARCHIVED_SNAPSHOT_BUFFER_SIZE )
	{
		return NULL;
	}

	firstCachedSnapshotFrame = svs.nextCachedSnapshotFrames - NUM_CACHED_FRAMES;

	if ( firstCachedSnapshotFrame < 0 )
	{
		firstCachedSnapshotFrame = 0;
	}

	for ( i = svs.nextCachedSnapshotFrames - 1; i >= firstCachedSnapshotFrame; --i )
	{
		cachedFrame = &svs.cachedSnapshotFrames[i % NUM_CACHED_FRAMES];

		if ( cachedFrame->archivedFrame != archivedFrame )
		{
			continue;
		}

		assert(cachedFrame->first_entity >= 0);

		if ( cachedFrame->first_entity < svs.nextCachedSnapshotEntities - CACHED_SNAPSHOT_ENTITY_SIZE )
		{
			break;
		}

		if ( cachedFrame->first_client < svs.nextCachedSnapshotClients - CACHED_SNAPSHOT_CLIENT_SIZE )
		{
			break;
		}

		return cachedFrame;
	}

	MSG_Init(&msg, msg_buf, MAX_SNAPSHOT_MSG_LEN);
	msg.cursize = frame->size;

	startIndex = frame->start % ARCHIVED_SNAPSHOT_BUFFER_SIZE;
	partSize = ARCHIVED_SNAPSHOT_BUFFER_SIZE - startIndex;

	if ( msg.cursize <= partSize )
	{
		memcpy(msg.data, &svs.archivedSnapshotBuffer[startIndex], msg.cursize);
	}
	else
	{
		memcpy(msg.data, &svs.archivedSnapshotBuffer[startIndex], partSize);
		memcpy(&msg.data[partSize], svs.archivedSnapshotBuffer, msg.cursize - partSize);
	}

	if ( !MSG_ReadBit(&msg) )
	{
		assert(!msg.overflowed);

		oldArchivedFrame = MSG_ReadLong(&msg);

		if ( oldArchivedFrame < svs.nextArchivedSnapshotFrames - NUM_ARCHIVED_FRAMES )
		{
			return NULL;
		}

		frame = &svs.archivedSnapshotFrames[oldArchivedFrame % NUM_ARCHIVED_FRAMES];

		if ( frame->start < svs.nextArchivedSnapshotBuffer - ARCHIVED_SNAPSHOT_BUFFER_SIZE )
		{
			return NULL;
		}

		oldCachedFrame = SV_GetCachedSnapshotInternal(oldArchivedFrame);

		if ( !oldCachedFrame )
		{
			return NULL;
		}

		assert(!oldCachedFrame->usesDelta);

		cachedFrame = &svs.cachedSnapshotFrames[svs.nextCachedSnapshotFrames % NUM_CACHED_FRAMES];

		cachedFrame->archivedFrame = archivedFrame;
		cachedFrame->num_entities = 0;

		cachedFrame->first_entity = svs.nextCachedSnapshotEntities;
		cachedFrame->num_clients = 0;

		cachedFrame->first_client = svs.nextCachedSnapshotClients;
		cachedFrame->usesDelta = 1;

		cachedFrame->time = MSG_ReadLong(&msg);

		oldindex = 0;
		oldCachedClient = NULL;

		if ( oldindex >= oldCachedFrame->num_clients )
		{
			oldnum = 99999;
		}
		else
		{
			oldCachedClient = &svs.cachedSnapshotClients[(oldCachedFrame->first_client + oldindex) % CACHED_SNAPSHOT_CLIENT_SIZE];
			oldnum = oldCachedClient->cs.clientIndex;
		}

		for ( ; MSG_ReadBit(&msg); )
		{
			newnum = MSG_ReadBits(&msg, CLIENTNUM_BITS);
			assert(newnum >= 0);

			if ( msg.readcount > msg.cursize )
			{
				Com_Error(ERR_DROP, "\x15" "SV_GetCachedSnapshot: end of message");
			}

			while ( oldnum < newnum )
			{
				oldindex++;

				if ( oldindex >= oldCachedFrame->num_clients )
				{
					oldnum = 99999;
				}
				else
				{
					oldCachedClient = &svs.cachedSnapshotClients[(oldCachedFrame->first_client + oldindex) % CACHED_SNAPSHOT_CLIENT_SIZE];
					oldnum = oldCachedClient->cs.clientIndex;
				}
			}

			if ( oldnum == newnum )
			{
				cachedClient = &svs.cachedSnapshotClients[svs.nextCachedSnapshotClients % CACHED_SNAPSHOT_CLIENT_SIZE];
				assert(cachedClient != oldCachedClient);

				MSG_ReadDeltaClient(&msg, &oldCachedClient->cs, &cachedClient->cs, newnum);

				cachedClient->playerStateExists = MSG_ReadBit(&msg);

				if ( cachedClient->playerStateExists )
				{
					MSG_ReadDeltaPlayerstate(&msg, &oldCachedClient->ps, &cachedClient->ps);
				}

				svs.nextCachedSnapshotClients++;

				if ( svs.nextCachedSnapshotClients >= 0x7FFFFFFE )
				{
					Com_Error(ERR_FATAL, "\x15" "svs.nextCachedSnapshotClients wrapped");
				}

				++cachedFrame->num_clients;

				oldindex++;

				if ( oldindex >= oldCachedFrame->num_clients )
				{
					oldnum = 99999;
				}
				else
				{
					oldCachedClient = &svs.cachedSnapshotClients[(oldCachedFrame->first_client + oldindex) % CACHED_SNAPSHOT_CLIENT_SIZE];
					oldnum = oldCachedClient->cs.clientIndex;
				}
			}
			else
			{
				assert(oldnum > newnum);
				cachedClient = &svs.cachedSnapshotClients[svs.nextCachedSnapshotClients % CACHED_SNAPSHOT_CLIENT_SIZE];

				MSG_ReadDeltaClient(&msg, NULL, &cachedClient->cs, newnum);

				cachedClient->playerStateExists = MSG_ReadBit(&msg);

				if ( cachedClient->playerStateExists )
				{
					MSG_ReadDeltaPlayerstate(&msg, NULL, &cachedClient->ps);
				}

				svs.nextCachedSnapshotClients++;

				if ( svs.nextCachedSnapshotClients >= 0x7FFFFFFE )
				{
					Com_Error(ERR_FATAL, "\x15" "svs.nextCachedSnapshotClients wrapped");
				}

				++cachedFrame->num_clients;
			}
		}

		for ( ; ; )
		{
			newnum = MSG_ReadBits(&msg, GENTITYNUM_BITS);

			if ( newnum == ENTITYNUM_NONE )
			{
				break;
			}

			if ( msg.readcount > msg.cursize )
			{
				Com_Error(ERR_DROP, "\x15" "SV_GetCachedSnapshot: end of message");
			}

			cachedEntity = &svs.cachedSnapshotEntities[svs.nextCachedSnapshotEntities % CACHED_SNAPSHOT_ENTITY_SIZE];
			MSG_ReadDeltaArchivedEntity(&msg, &sv.svEntities[newnum].baseline, cachedEntity, newnum);

			svs.nextCachedSnapshotEntities++;

			if ( svs.nextCachedSnapshotEntities >= 0x7FFFFFFE )
			{
				Com_Error(ERR_FATAL, "\x15" "svs.nextCachedSnapshotEntities wrapped");
			}

			++cachedFrame->num_entities;
		}

		svs.nextCachedSnapshotFrames++;

		if ( svs.nextCachedSnapshotFrames >= 0x7FFFFFFE )
		{
			Com_Error(ERR_FATAL, "\x15" "svs.nextCachedSnapshotFrames wrapped");
		}
	}
	else
	{
		assert(!msg.overflowed);

	cachedFrame = &svs.cachedSnapshotFrames[svs.nextCachedSnapshotFrames % NUM_CACHED_FRAMES];

	cachedFrame->archivedFrame = archivedFrame;
	cachedFrame->num_entities = 0;

	cachedFrame->first_entity = svs.nextCachedSnapshotEntities;
	cachedFrame->num_clients = 0;

	cachedFrame->first_client = svs.nextCachedSnapshotClients;
	cachedFrame->usesDelta = 0;

	cachedFrame->time = MSG_ReadLong(&msg);

	for ( ; MSG_ReadBit(&msg); )
	{
		newnum = MSG_ReadBits(&msg, CLIENTNUM_BITS);

		if ( msg.readcount > msg.cursize )
		{
			Com_Error(ERR_DROP, "\x15" "SV_GetCachedSnapshot: end of message");
		}

		cachedClient = &svs.cachedSnapshotClients[svs.nextCachedSnapshotClients % CACHED_SNAPSHOT_CLIENT_SIZE];

		MSG_ReadDeltaClient(&msg, NULL, &cachedClient->cs, newnum);

		cachedClient->playerStateExists = MSG_ReadBit(&msg);

		if ( cachedClient->playerStateExists )
		{
			MSG_ReadDeltaPlayerstate(&msg, NULL, &cachedClient->ps);
		}

		svs.nextCachedSnapshotClients++;

		if ( svs.nextCachedSnapshotClients >= 0x7FFFFFFE )
		{
			Com_Error(ERR_FATAL, "\x15" "svs.nextCachedSnapshotClients wrapped");
		}

		++cachedFrame->num_clients;
	}

	for ( ; ; )
	{
		newnum = MSG_ReadBits(&msg, GENTITYNUM_BITS);

		if ( newnum == ENTITYNUM_NONE )
		{
			break;
		}

		if ( msg.readcount > msg.cursize )
		{
			Com_Error(ERR_DROP, "\x15" "SV_GetCachedSnapshot: end of message");
		}

		cachedEntity = &svs.cachedSnapshotEntities[svs.nextCachedSnapshotEntities % CACHED_SNAPSHOT_ENTITY_SIZE];
		MSG_ReadDeltaArchivedEntity( &msg, &sv.svEntities[newnum].baseline, cachedEntity, newnum );

		svs.nextCachedSnapshotEntities++;

		if ( svs.nextCachedSnapshotEntities >= 0x7FFFFFFE )
		{
			Com_Error(ERR_FATAL, "\x15" "svs.nextCachedSnapshotEntities wrapped");
		}

		++cachedFrame->num_entities;
	}

		svs.nextCachedSnapshotFrames++;

		if ( svs.nextCachedSnapshotFrames >= 0x7FFFFFFE )
		{
			Com_Error(ERR_FATAL, "\x15" "svs.nextCachedSnapshotFrames wrapped");
		}
	}

	return cachedFrame;
}

/*
==================
SV_GetCachedSnapshot
==================
*/
cachedSnapshot_t* SV_GetCachedSnapshot( int *pArchiveTime )
{
	int archiveTime;
	int archivedFrame;
	cachedSnapshot_t *cachedFrame;

	assert(SV_Loaded());
	assert(sv_fps->current.integer);

	if ( !svs.archiveEnabled )
	{
		return NULL;
	}

	archiveTime = *pArchiveTime;

	if ( archiveTime <= 0 )
	{
		return NULL;
	}

	archivedFrame = svs.nextArchivedSnapshotFrames - archiveTime * sv_fps->current.integer / 1000;

	if ( archivedFrame < svs.nextArchivedSnapshotFrames - NUM_ARCHIVED_FRAMES )
	{
		archivedFrame = svs.nextArchivedSnapshotFrames - NUM_ARCHIVED_FRAMES;
		*pArchiveTime = 1000 * (svs.nextArchivedSnapshotFrames - archivedFrame) / sv_fps->current.integer;
	}

	if ( archivedFrame < 0 )
	{
		archivedFrame = 0;
		*pArchiveTime = 1000 * svs.nextArchivedSnapshotFrames / sv_fps->current.integer;
	}

	while ( archivedFrame < svs.nextArchivedSnapshotFrames )
	{
		cachedFrame = SV_GetCachedSnapshotInternal(archivedFrame);

		if ( cachedFrame )
		{
			return cachedFrame;
		}

		archivedFrame++;
	}

	*pArchiveTime = 0;
	return NULL;
}

/*
===============
SV_GetCurrentClientInfo
===============
*/
qboolean SV_GetCurrentClientInfo( int clientNum, playerState_t *ps, clientState_t *cs )
{
	client_t *client;

	assert(clientNum >= 0 && clientNum < MAX_CLIENTS);

	client = &svs.clients[clientNum];

	if ( client->state != CS_ACTIVE )
	{
		return qfalse;
	}

	if ( !GetFollowPlayerState(clientNum, ps) )
	{
		return qfalse;
	}

	*cs = *G_GetClientState(clientNum);
	return qtrue;
}

bool SV_GetClientPositionsAtTime( int clientNum, int gametime, vec3_t pos )
{
	static int sv_clientPosStartFrameBias = 2;
	static int sv_clientPosEndFrameBias = 1;
	int j;
	float progress;
	int startOffset;
	int endOffset;
	vec3_t startPos;
	vec3_t endPos;
	bool foundStart;
	bool foundEnd;
	playerState_t ps;
	clientState_t cs;
	int i;
	int frameTime;
	int frameHistCount;
	int pArchiveTime;
	int archiveSearchDepth = 10;

	frameTime = 1000 / sv_fps->current.integer;
	frameHistCount = (svs.time / frameTime) * frameTime;

	startOffset = (sv_clientPosStartFrameBias + (frameHistCount - gametime) / frameTime) * frameTime;
	endOffset = (sv_clientPosEndFrameBias + (frameHistCount - gametime) / frameTime) * frameTime;

	foundStart = false;
	foundEnd = false;

	i = 0;
	pArchiveTime = startOffset;

	for ( i = 0; i < 10; ++i )
	{
		if ( SV_GetArchivedClientInfo(clientNum, &pArchiveTime, &ps, &cs) )
		{
			foundStart = true;
			startOffset = pArchiveTime;
			memcpy(startPos, ps.origin, sizeof(startPos));
			break;
		}

		pArchiveTime += frameTime;
	}

	i = 0;
	pArchiveTime = endOffset;

	for ( i = 0; i < 10; ++i )
	{
		if ( SV_GetArchivedClientInfo(clientNum, &pArchiveTime, &ps, &cs) )
		{
			foundEnd = true;
			endOffset = pArchiveTime;
			memcpy(endPos, ps.origin, sizeof(endPos));
			break;
		}

		pArchiveTime -= frameTime;
	}

	if ( foundStart && foundEnd )
	{
		progress = (float)(gametime % frameTime) / (float)(startOffset - endOffset);
	}
	else if ( foundStart )
	{
		progress = 0.0;
		VectorClear(endPos);
	}
	else
	{
		if ( foundEnd )
		{
			progress = 1.0;
			VectorClear(startPos);
		}
		else
		{
			return false;
		}
	}

	for ( j = 0; j < 3; ++j )
	{
		pos[j] = lerp(startPos[j], endPos[j], progress);
	}

	return true;
}

/*
===============
SV_GetArchivedClientInfo
===============
*/
qboolean SV_GetArchivedClientInfo( int clientNum, int *pArchiveTime, playerState_t *ps, clientState_t *cs )
{
	cachedSnapshot_t *cachedSnapshot;
	int i;
	int offsettime;
	cachedClient_t *cachedClient;

	cachedSnapshot = SV_GetCachedSnapshot(pArchiveTime);

	if ( !cachedSnapshot )
	{
		if ( *pArchiveTime <= 0 )
		{
			return SV_GetCurrentClientInfo(clientNum, ps, cs);
		}

		return qfalse;
	}

	assert(*pArchiveTime > 0);

	offsettime = svs.time - cachedSnapshot->time;
	cachedClient = NULL;

	for ( i = 0; ; ++i )
	{
		if ( i >= cachedSnapshot->num_clients )
		{
			goto fail;
		}

		cachedClient = &svs.cachedSnapshotClients[(cachedSnapshot->first_client + i) % CACHED_SNAPSHOT_CLIENT_SIZE];

		if ( cachedClient->cs.clientIndex == clientNum )
		{
			if ( cachedClient->playerStateExists )
			{
				break;
			}

			goto fail;
		}
	}

	goto found;

fail:
	return qfalse;

found:
	assert(cachedClient);

	// VoroN: Just in case
	assert(ps);
	assert(cs);

	*ps = cachedClient->ps;
	*cs = cachedClient->cs;

	if ( ps->commandTime )
	{
		ps->commandTime += offsettime;
	}

	if ( ps->pm_time )
	{
		ps->pm_time += offsettime;
	}

	if ( ps->foliageSoundTime )
	{
		ps->foliageSoundTime += offsettime;
	}

	if ( ps->jumpTime )
	{
		ps->jumpTime += offsettime;
	}

	if ( ps->viewHeightLerpTime )
	{
		ps->viewHeightLerpTime += offsettime;
	}

	if ( ps->shellshockTime )
	{
		ps->shellshockTime += offsettime;
	}

	for ( i = 0; i < ARRAY_COUNT( ps->hud.archival ); ++i )
	{
		if ( ps->hud.archival[i].time )
		{
			ps->hud.archival[i].time += offsettime;
		}

		if ( ps->hud.archival[i].fadeStartTime )
		{
			ps->hud.archival[i].fadeStartTime += offsettime;

			if ( ps->hud.archival[i].fadeStartTime > svs.time )
			{
				ps->hud.archival[i].fadeStartTime = svs.time;
			}
		}

		if ( ps->hud.archival[i].scaleStartTime )
		{
			ps->hud.archival[i].scaleStartTime += offsettime;
		}

		if ( ps->hud.archival[i].moveStartTime )
		{
			ps->hud.archival[i].moveStartTime += offsettime;
		}
	}

	ps->deltaTime += offsettime;

	return qtrue;
}

void SV_BuildClientSnapshot( client_t *client )
{
	vec3_t org;
	clientSnapshot_t            *frame;
	snapshotEntityNumbers_t entityNumbers;
	int i;
	gentity_t              		*ent;
	entityState_t               *entState;
	clientState_t               *clientState;
	gentity_t					*clent;
	int clientNum;
	playerState_t               *ps;
	cachedSnapshot_t			*cachedSnap;
	archivedEntity_t			*aent;
	int snapTime;
	cachedClient_t				*cachedClient;
	int							archiveTime;

	// this is the frame we are creating
	frame = &client->frames[ client->netchan.outgoingSequence & PACKET_MASK ];

	// show_bug.cgi?id=62
	frame->num_entities = 0;
	frame->num_clients = 0;

	clent = client->gentity;

	if ( !clent || client->state == CS_ZOMBIE )
	{
		return;
	}

	frame->first_entity = svs.nextSnapshotEntities;
	frame->first_client = svs.nextSnapshotClients;

	// tests sv.state inline here rather than through SV_Loaded()
	if ( sv.state != SS_GAME )
	{
		return;
	}

	// clear everything in this snapshot
	entityNumbers.numSnapshotEntities = 0;

	clientNum = client - svs.clients;
	archiveTime = G_GetClientArchiveTime(clientNum);
	cachedSnap = SV_GetCachedSnapshot(&archiveTime);
	G_SetClientArchiveTime(clientNum, archiveTime);

	snapTime = cachedSnap ? svs.time - cachedSnap->time : 0;

	// grab the current playerState_t
	ps = &frame->ps;
	*ps = *SV_GameClientNum( clientNum );

	// never send client's own entity, because it can
	// be regenerated from the playerstate
	clientNum = ps->clientNum;
	if ( clientNum < 0 || clientNum >= MAX_GENTITIES )
	{
		Com_Error( ERR_DROP, "\x15" "SV_BuildClientSnapshot: bad gEnt" );
	}

	// find the client's viewpoint
	VectorCopy( ps->origin, org );
	org[2] += ps->viewHeightCurrent;

//----(SA)	added for 'lean'
	// need to account for lean, so areaportal doors draw properly
	AddLeanToPosition(org, ps->viewangles[1], ps->leanf, 16.0, 20.0);
//----(SA)	end

	if ( !cachedSnap )
	{
		// add all the entities directly visible to the eye, which
		// may include portal entities that merge other viewpoints
		SV_AddEntitiesVisibleFromPoint( org, clientNum, &entityNumbers );

		// copy the entity states out
		for ( i = 0 ; i < entityNumbers.numSnapshotEntities ; i++ )
		{
			ent = SV_GentityNum( entityNumbers.snapshotEntities[i] );
			entState = &svs.snapshotEntities[svs.nextSnapshotEntities % svs.numSnapshotEntities];

			*entState = ent->s;

			svs.nextSnapshotEntities++;
			// this should never hit, map should always be restarted first in SV_Frame
			if ( svs.nextSnapshotEntities >= 0x7FFFFFFE )
			{
				Com_Error( ERR_FATAL, "\x15" "svs.nextSnapshotEntities wrapped" );
			}

			frame->num_entities++;
		}

		// copy the client states out
		for ( i = 0, client = svs.clients ; i < sv_maxclients->current.integer ; i++, client++ )
		{
			if ( client->state < CS_CONNECTED )
			{
				continue;
			}

			clientState = &svs.snapshotClients[svs.nextSnapshotClients % svs.numSnapshotClients];

			*clientState = *G_GetClientState(i);

			if ( clientState->clientIndex != i )
			{
				continue;
			}

			svs.nextSnapshotClients++;
			// this should never hit, map should always be restarted first in SV_Frame
			if ( svs.nextSnapshotClients >= 0x7FFFFFFE )
			{
				Com_Error( ERR_FATAL, "\x15" "svs.nextSnapshotClients wrapped" );
			}

			frame->num_clients++;
		}
	}
	else
	{
		SV_AddCachedEntitiesVisibleFromPoint(cachedSnap->num_entities, cachedSnap->first_entity, org, clientNum, &entityNumbers, ps);

		for ( i = 0 ; i < entityNumbers.numSnapshotEntities; ++i )
		{
			aent = &svs.cachedSnapshotEntities[(cachedSnap->first_entity + entityNumbers.snapshotEntities[i]) % CACHED_SNAPSHOT_ENTITY_SIZE];
			entState = &svs.snapshotEntities[svs.nextSnapshotEntities % svs.numSnapshotEntities];

			*entState = aent->s;

			if ( entState->pos.trTime )
				entState->pos.trTime += snapTime;

			if ( entState->apos.trTime )
				entState->apos.trTime += snapTime;

			if ( entState->time )
				entState->time += snapTime;

			if ( entState->time2 )
				entState->time2 += snapTime;

			svs.nextSnapshotEntities++;
			// this should never hit, map should always be restarted first in SV_Frame
			if ( svs.nextSnapshotEntities >= 0x7FFFFFFE )
			{
				Com_Error( ERR_FATAL, "\x15" "svs.nextSnapshotEntities wrapped" );
			}

			frame->num_entities++;
		}

		for ( i = 0; i < cachedSnap->num_clients; ++i )
		{
			cachedClient = &svs.cachedSnapshotClients[(cachedSnap->first_client + i) % CACHED_SNAPSHOT_CLIENT_SIZE];
			clientState = &svs.snapshotClients[svs.nextSnapshotClients % svs.numSnapshotClients];

			*clientState = cachedClient->cs;

			svs.nextSnapshotClients++;
			// this should never hit, map should always be restarted first in SV_Frame
			if ( svs.nextSnapshotClients >= 0x7FFFFFFE )
			{
				Com_Error( ERR_FATAL, "\x15" "svs.nextSnapshotClients wrapped" );
			}

			frame->num_clients++;
		}
	}
}

static int SV_RateMsec( client_t *client, int messageSize )
{
	int rate;
	int rateMsec;

	// individual messages will never be larger than fragment size
	if ( messageSize > 1500 )
	{
		messageSize = 1500;
	}

	rate = client->rate;

	// low watermark for sv_maxRate, never 0 < sv_maxRate < 1000 (0 is no limitation)
	if ( sv_maxRate->current.integer )
	{
		if ( sv_maxRate->current.integer < 1000 )
		{
			Dvar_SetInt(sv_maxRate, 1000);
		}

		if ( sv_maxRate->current.integer < rate )
		{
			rate = sv_maxRate->current.integer;
		}
	}

	rateMsec = ( 1000 * messageSize + (HEADER_RATE_BYTES * 1000) ) / rate;

	if ( sv_debugRate->current.boolean )
	{
		Com_Printf( "It would take %ims to send %i bytes to client %s (rate %i)\n", rateMsec, messageSize, client->name, client->rate );
	}

	return rateMsec;
}

/*
=======================
SV_SendMessageToClient
Called by SV_SendClientSnapshot and SV_SendClientGameState
=======================
*/
// the 16k compress scratch is scoped through the LargeLocal RAII helper
// rather than a stack array
void SV_SendMessageToClient( msg_t *msg, client_t *client )
{
	int rateMsec;
	int compressedSize;
	LargeLocal svCompressBufLarge( MAX_MSGLEN );
	byte *svCompressBuf = (byte *)svCompressBufLarge.GetBuf();

	assert(client - svs.clients >= 0 && client - svs.clients < MAX_CLIENTS);
	assert(msg->cursize >= SV_ENCODE_START);

	memcpy(svCompressBuf, msg->data, SV_ENCODE_START);
	compressedSize = MSG_WriteBitsCompress(msg->data + SV_ENCODE_START, svCompressBuf + SV_ENCODE_START, msg->cursize - SV_ENCODE_START) + SV_ENCODE_START;

	if ( client->dropReason )
	{
		SV_DropClient(client, client->dropReason);

		assert(!client->dropReason);
		assert(client->state == CS_ZOMBIE);
	}

	// record information about the message
	client->frames[client->netchan.outgoingSequence & PACKET_MASK].messageSize = compressedSize;
	client->frames[client->netchan.outgoingSequence & PACKET_MASK].messageSent = svs.time;
	client->frames[client->netchan.outgoingSequence & PACKET_MASK].messageAcked = -1;

	// send the datagram
	SV_Netchan_Transmit(client, svCompressBuf, compressedSize);

	// set nextSnapshotTime based on rate and requested number of updates
	// local clients get snapshots every frame
	// TTimo - show_bug.cgi?id=491
	// added sv_lanForceRate check
	if ( client->netchan.remoteAddress.type == NA_LOOPBACK || Sys_IsLANAddress( client->netchan.remoteAddress ) )
	{
		client->nextSnapshotTime = svs.time - 1;
		return;
	}

	// normal rate / snapshotMsec calculation
	rateMsec = SV_RateMsec( client, compressedSize );

	// TTimo - during a download, ignore the snapshotMsec
	// the update server on steroids, with this disabled and sv_fps 60, the download can reach 30 kb/s
	// on a regular server, we will still top at 20 kb/s because of sv_fps 20
	if ( rateMsec < client->snapshotMsec )
	{
		// never send more packets than this, no matter what the rate is at
		rateMsec = client->snapshotMsec;
		client->rateDelayed = qfalse;
	}
	else
	{
		client->rateDelayed = qtrue;
	}

	client->nextSnapshotTime = svs.time + rateMsec;

	// don't pile up empty snapshots while connecting
	if ( client->state != CS_ACTIVE )
	{
		// a gigantic connection message may have already put the nextSnapshotTime
		// more than a second away, so don't shorten it
		// do shorten if client is downloading
		if ( !*client->downloadName && client->nextSnapshotTime < svs.time + 1000 )
		{
			client->nextSnapshotTime = svs.time + 1000;
		}
	}

	sv.bpsTotalBytes += compressedSize;
}

/*
=======================
SV_SendClientSnapshot

Also called by SV_FinalCommand

=======================
*/
void SV_SendClientSnapshot( client_t *client )
{
	msg_t msg;
	LargeLocal msgBufLarge( 0x20000 );
	byte *msg_buf = (byte *)msgBufLarge.GetBuf();

	//bani
	if ( client->state == CS_ACTIVE || client->state == CS_ZOMBIE )
	{
		// build the snapshot
		SV_BuildClientSnapshot(client);
	}

	MSG_Init( &msg, msg_buf, 0x20000 );

	// NOTE, MRE: all server->client messages now acknowledge
	// let the client know which reliable clientCommands we have received
	MSG_WriteLong( &msg, client->lastClientCommand );

	//bani
	if ( client->state == CS_ACTIVE || client->state == CS_ZOMBIE )
	{
		// (re)send any reliable server commands
		SV_UpdateServerCommandsToClient(client, &msg);

		// send over all the relevant entityState_t
		// and the playerState_t
		SV_WriteSnapshotToClient(client, &msg);
	}

	// Add any download data if the client is downloading
	if ( client->state != CS_ZOMBIE )
	{
		SV_WriteDownloadToClient(client, &msg);
	}

	MSG_WriteByte(&msg, svc_EOF);

	// check for overflow
	if ( msg.overflowed )
	{
		Com_Printf("WARNING: msg overflowed for %s, trying to recover\n", client->name);

		//bani
		if ( client->state == CS_ACTIVE || client->state == CS_ZOMBIE )
		{
			SV_ShowClientUnAckCommands(client);

			MSG_Init(&msg, msg_buf, 0x20000);
			MSG_WriteLong(&msg, client->lastClientCommand);

			SV_UpdateServerCommandsToClient_PreventOverflow(client, &msg, 0x20000);

			MSG_WriteByte(&msg, svc_EOF);
		}

		// check for overflow
		if ( msg.overflowed )
		{
			Com_Printf("WARNING: client disconnected for msg overflow: %s\n", client->name);
			NET_OutOfBandPrint(NS_SERVER, client->netchan.remoteAddress, "disconnect");
			SV_DropClient(client, "EXE_SERVERMESSAGEOVERFLOW");
		}
	}

	SV_SendMessageToClient( &msg, client );
}

/*
==================
SV_ArchiveSnapshot
==================
*/
void SV_ArchiveSnapshot()
{
	int j;
	gentity_t *ent;
	archivedEntity_t to;
	svEntity_t *svEnt;
	int i;
	client_t *cl;
	archivedSnapshot_t *aSnap;
	int partSize;
	int v20;
	playerState_t ps;
	cachedSnapshot_t *cachedFrame;
	int firstCachedSnapshotFrame;
	int minArchivedFrame;
	cachedClient_t *cachedClient;
	archivedEntity_t *archEnt;
	int maxClients;
	int numClients;
	int clientNum;
	int newindex;
	int newnum;
	msg_t msg;
	LargeLocal msgLarge( MAX_SNAPSHOT_MSG_LEN );
	byte *msg_buf = (byte *)msgLarge.GetBuf();

	if ( sv.state != SS_GAME )
	{
		return;
	}

	if ( !svs.archiveEnabled )
	{
		return;
	}

	{
		MSG_Init(&msg, msg_buf, MAX_SNAPSHOT_MSG_LEN);

		firstCachedSnapshotFrame = svs.nextCachedSnapshotFrames - NUM_CACHED_FRAMES;

		if ( firstCachedSnapshotFrame < 0 )
		{
			firstCachedSnapshotFrame = 0;
		}

		minArchivedFrame = svs.nextArchivedSnapshotFrames - sv_fps->current.integer;

		for ( i = svs.nextCachedSnapshotFrames - 1; i >= firstCachedSnapshotFrame; --i )
		{
			cachedFrame = &svs.cachedSnapshotFrames[i % NUM_CACHED_FRAMES];

			if ( cachedFrame->archivedFrame < minArchivedFrame )
			{
				continue;
			}

			if ( cachedFrame->usesDelta )
			{
				continue;
			}

			if ( cachedFrame->first_entity < svs.nextCachedSnapshotEntities - CACHED_SNAPSHOT_ENTITY_SIZE )
			{
				break;
			}

			if ( cachedFrame->first_client < svs.nextCachedSnapshotClients - CACHED_SNAPSHOT_CLIENT_SIZE )
			{
				break;
			}
			{
					MSG_WriteBit0(&msg);
					MSG_WriteLong(&msg, cachedFrame->archivedFrame);
					MSG_WriteLong(&msg, svs.time);

					maxClients = sv_maxclients->current.integer;
					numClients = cachedFrame->num_clients;

					cachedClient = NULL;
					clientNum = 0;
					newindex = 0;

					while ( clientNum < maxClients || newindex < numClients )
					{
						if ( clientNum < maxClients && svs.clients[clientNum].state < CS_CONNECTED )
						{
							++clientNum;
							continue;
						}

						if ( newindex >= numClients )
						{
							newnum = 9999;
						}
						else
						{
							cachedClient = &svs.cachedSnapshotClients[(cachedFrame->first_client + newindex) % CACHED_SNAPSHOT_CLIENT_SIZE];
							newnum = cachedClient->cs.clientIndex;
						}

						if ( clientNum == newnum )
						{
							MSG_WriteDeltaClient(&msg, &cachedClient->cs, G_GetClientState(clientNum), qtrue);

							if ( GetFollowPlayerState(clientNum, &ps) )
							{
								MSG_WriteBit1(&msg);
								MSG_WriteDeltaPlayerstate(&msg, &cachedClient->ps, &ps);
							}
							else
							{
								MSG_WriteBit0(&msg);
							}

							++newindex;
							++clientNum;
							continue;
						}

						if ( clientNum < newnum )
						{
							MSG_WriteDeltaClient(&msg, NULL, G_GetClientState(clientNum), qtrue);

							if ( GetFollowPlayerState(clientNum, &ps) )
							{
								MSG_WriteBit1(&msg);
								MSG_WriteDeltaPlayerstate(&msg, NULL, &ps);
							}
							else
							{
								MSG_WriteBit0(&msg);
							}

							++clientNum;
							continue;
						}

						if ( clientNum > newnum )
						{
							++newindex;
						}
					}

					MSG_WriteBit0(&msg);

					for ( j = 0; j < sv.num_entities; ++j )
					{
						ent = SV_GentityNum(j);

						if ( !ent->r.linked )
						{
							continue;
						}

						if ( !ent->r.broadcastTime )
						{
							if ( ent->r.svFlags & SVF_NOCLIENT )
							{
								continue;
							}

							svEnt = SV_SvEntityForGentity(ent);

							if ( !( ent->r.svFlags & ( SVF_BROADCAST | SVF_OBJECTIVE ) ) )
							{
								if ( !svEnt->numClusters )
								{
									continue;
								}
							}
						}

						to.s = ent->s;
						to.r.svFlags = ent->r.svFlags;

						if ( ent->r.broadcastTime )
						{
							to.r.svFlags |= SVF_BROADCAST;
						}

						to.r.clientMask[0] = ent->r.clientMask[0];
						to.r.clientMask[1] = ent->r.clientMask[1];

						VectorCopy(ent->r.absmin, to.r.absmin);
						VectorCopy(ent->r.absmax, to.r.absmax);

						MSG_WriteDeltaArchivedEntity(&msg, &sv.svEntities[ent->s.number].baseline, &to, DELTA_FLAGS_FORCE);
					}

					goto LABEL_70;
			}
		}

		MSG_WriteBit1(&msg);
		MSG_WriteLong(&msg, svs.time);

		cachedFrame = &svs.cachedSnapshotFrames[svs.nextCachedSnapshotFrames % NUM_CACHED_FRAMES];

		cachedFrame->archivedFrame = svs.nextArchivedSnapshotFrames;
		cachedFrame->num_entities = 0;

		cachedFrame->first_entity = svs.nextCachedSnapshotEntities;
		cachedFrame->num_clients = 0;

		cachedFrame->first_client = svs.nextCachedSnapshotClients;
		cachedFrame->usesDelta = 0;

		cachedFrame->time = svs.time;

		for ( i = 0, cl = svs.clients; i < sv_maxclients->current.integer; i++, cl++ )
		{
			if ( cl->state < CS_CONNECTED )
			{
				continue;
			}

			cachedClient = &svs.cachedSnapshotClients[svs.nextCachedSnapshotClients % CACHED_SNAPSHOT_CLIENT_SIZE];
			cachedClient->cs = *G_GetClientState(i);

			MSG_WriteDeltaClient(&msg, NULL, &cachedClient->cs, qtrue);

			cachedClient->playerStateExists = GetFollowPlayerState(i, &cachedClient->ps);

			if ( cachedClient->playerStateExists )
			{
				MSG_WriteBit1(&msg);
				MSG_WriteDeltaPlayerstate(&msg, NULL, &cachedClient->ps);
			}
			else
			{
				MSG_WriteBit0(&msg);
			}

			if ( ++svs.nextCachedSnapshotClients >= 0x7ffffffe )
			{
				Com_Error(ERR_FATAL, "\x15" "svs.nextCachedSnapshotClients wrapped");
			}

			++cachedFrame->num_clients;
		}

		MSG_WriteBit0(&msg);

		for ( j = 0; j < sv.num_entities; ++j )
		{
			ent = SV_GentityNum(j);

			if ( !ent->r.linked )
			{
				continue;
			}

			if ( !ent->r.broadcastTime )
			{
				if ( ent->r.svFlags & SVF_NOCLIENT )
				{
					continue;
				}

				svEnt = SV_SvEntityForGentity(ent);

				if ( !( ent->r.svFlags & ( SVF_BROADCAST | SVF_OBJECTIVE ) ) )
				{
					if ( !svEnt->numClusters )
					{
						continue;
					}
				}
			}

			archEnt = &svs.cachedSnapshotEntities[svs.nextCachedSnapshotEntities % CACHED_SNAPSHOT_ENTITY_SIZE];

			archEnt->s = ent->s;
			archEnt->r.svFlags = ent->r.svFlags;

			if ( ent->r.broadcastTime )
			{
				archEnt->r.svFlags |= SVF_BROADCAST;
			}

			archEnt->r.clientMask[0] = ent->r.clientMask[0];
			archEnt->r.clientMask[1] = ent->r.clientMask[1];

			VectorCopy(ent->r.absmin, archEnt->r.absmin);
			VectorCopy(ent->r.absmax, archEnt->r.absmax);

			MSG_WriteDeltaArchivedEntity(&msg, &sv.svEntities[ent->s.number].baseline, archEnt, DELTA_FLAGS_FORCE);

			if ( ++svs.nextCachedSnapshotEntities >= 0x7ffffffe )
			{
				Com_Error(ERR_FATAL, "\x15" "svs.nextCachedSnapshotEntities wrapped");
			}

			++cachedFrame->num_entities;
		}

		if ( ++svs.nextCachedSnapshotFrames >= 0x7ffffffe )
		{
			Com_Error(ERR_FATAL, "\x15" "svs.nextCachedSnapshotFrames wrapped");
		}

LABEL_70:
		MSG_WriteBits( &msg, ( MAX_GENTITIES - 1 ), GENTITYNUM_BITS );   // end of packetentities

		if ( msg.overflowed )
		{
			Com_DPrintf("SV_ArchiveSnapshot: ignoring snapshot because it overflowed.\n");
			return;
		}

		aSnap = &svs.archivedSnapshotFrames[svs.nextArchivedSnapshotFrames % NUM_ARCHIVED_FRAMES];

		aSnap->start = svs.nextArchivedSnapshotBuffer;
		aSnap->size = msg.cursize;

		partSize = svs.nextArchivedSnapshotBuffer % ARCHIVED_SNAPSHOT_BUFFER_SIZE;

		svs.nextArchivedSnapshotBuffer += msg.cursize;

		if ( svs.nextArchivedSnapshotBuffer >= 0x7ffffffe )
		{
			Com_Error(ERR_FATAL, "\x15" "svs.nextArchivedSnapshotBuffer wrapped");
		}

		v20 = ARCHIVED_SNAPSHOT_BUFFER_SIZE - partSize;

		if ( msg.cursize <= v20 )
		{
			memcpy(&svs.archivedSnapshotBuffer[partSize], msg.data, msg.cursize);
		}
		else
		{
			memcpy(&svs.archivedSnapshotBuffer[partSize], msg.data, v20);
			memcpy(svs.archivedSnapshotBuffer, &msg.data[v20], msg.cursize - v20);
		}

		if ( ++svs.nextArchivedSnapshotFrames >= 0x7ffffffe )
		{
			Com_Error(ERR_FATAL, "\x15" "svs.nextArchivedSnapshotFrames wrapped");
		}
	}
}

/*
=======================
SV_SendClientMessages
=======================
*/
void SV_SendClientMessages( void )
{
	int i;
	client_t    *c;
	int numclients = 0;         // NERVE - SMF - net debugging

	sv.bpsTotalBytes = 0;       // NERVE - SMF - net debugging
	sv.ubpsTotalBytes = 0;      // NERVE - SMF - net debugging

	// send a message to each connected client
	for ( i = 0, c = svs.clients; i < sv_maxclients->current.integer; i++, c++ )
	{
		if ( !c->state )
		{
			continue;       // not connected
		}

		if ( svs.time < c->nextSnapshotTime )
		{
			continue;       // not time yet
		}

		numclients++;       // NERVE - SMF - net debugging

		// send additional message fragments if the last message
		// was too large to send at once
		if ( c->netchan.unsentFragments )
		{
			c->nextSnapshotTime = svs.time + SV_RateMsec( c, c->netchan.unsentLength - c->netchan.unsentFragmentStart );
			SV_Netchan_TransmitNextFragment(&c->netchan);
			continue;
		}

		// generate and send a new message
		SV_SendClientSnapshot( c );
		SV_SendClientVoiceData( c );
	}

	// NERVE - SMF - net debugging
	if ( sv_showAverageBPS->current.boolean && numclients > 0 )
	{
		float ave = 0, uave = 0;

		for ( i = 0; i < MAX_BPS_WINDOW - 1; i++ )
		{
			sv.bpsWindow[i] = sv.bpsWindow[i + 1];
			ave += sv.bpsWindow[i];

			sv.ubpsWindow[i] = sv.ubpsWindow[i + 1];
			uave += sv.ubpsWindow[i];
		}

		sv.bpsWindow[MAX_BPS_WINDOW - 1] = sv.bpsTotalBytes;
		ave += sv.bpsTotalBytes;

		sv.ubpsWindow[MAX_BPS_WINDOW - 1] = sv.ubpsTotalBytes;
		uave += sv.ubpsTotalBytes;

		if ( sv.bpsTotalBytes >= sv.bpsMaxBytes )
		{
			sv.bpsMaxBytes = sv.bpsTotalBytes;
		}

		if ( sv.ubpsTotalBytes >= sv.ubpsMaxBytes )
		{
			sv.ubpsMaxBytes = sv.ubpsTotalBytes;
		}

		sv.bpsWindowSteps++;

		if ( sv.bpsWindowSteps >= MAX_BPS_WINDOW )
		{
			float comp_ratio;

			sv.bpsWindowSteps = 0;

			ave = ( ave / (float)MAX_BPS_WINDOW );
			uave = ( uave / (float)MAX_BPS_WINDOW );

			comp_ratio = ( 1 - ave / uave ) * 100.f;
			sv.ucompAve += comp_ratio;
			sv.ucompNum++;

			Com_DPrintf( "bpspc(%2.0f) bps(%2.0f) pk(%i) ubps(%2.0f) upk(%i) cr(%2.2f) acr(%2.2f)\n",
			             ave / (float)numclients, ave, sv.bpsMaxBytes, uave, sv.ubpsMaxBytes, comp_ratio, sv.ucompAve / sv.ucompNum );
		}
	}
	// -NERVE - SMF
}

