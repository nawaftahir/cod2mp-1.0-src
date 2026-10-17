#include "../qcommon/qcommon.h"
#include "../qcommon/netchan.h"

void SV_WriteClientVoiceData( client_t *client, msg_t *msg );
bool SV_ClientWantsVoiceData( int clientNum );
bool SV_ClientHasClientMuted( int listener, int talker );
void SV_QueueVoicePacket( int talkerNum, int clientNum, VoicePacket_t *voicePacket );
void SV_UserVoice( client_t *cl, msg_t *msg );
void SV_PreGameUserVoice( client_t *cl, msg_t *msg );

/*
==================
SV_WriteClientVoiceData
==================
*/
void SV_WriteClientVoiceData( client_t *client, msg_t *msg )
{
	int i;

	MSG_WriteByte(msg, client->voicePacketCount);

	for ( i = 0; i < client->voicePacketCount; ++i )
	{
		MSG_WriteByte(msg, client->voicePackets[i].talker);
		MSG_WriteByte(msg, client->voicePackets[i].dataSize);
		MSG_WriteData(msg, client->voicePackets[i].data, client->voicePackets[i].dataSize);
	}
}

void SV_SendClientVoiceData( client_t *client )
{
	msg_t msg;
	LargeLocal msgBufLarge( MAX_VOICE_MSG_LEN );
	byte *msg_buf = (byte *)msgBufLarge.GetBuf();

	assert(client->voicePacketCount >= 0);

	if ( client->state != CS_ACTIVE || !client->voicePacketCount )
		return;

	MSG_Init(&msg, msg_buf, MAX_VOICE_MSG_LEN);
	assert(msg.cursize == 0);
	assert(msg.bit == 0);

	MSG_WriteString(&msg, "v");
	SV_WriteClientVoiceData(client, &msg);

	if ( msg.overflowed )
	{
		Com_Printf("WARNING: voice msg overflowed for %s\n", client->name);
		return;
	}

	NET_OutOfBandVoiceData(NS_SERVER, client->netchan.remoteAddress, msg.data, msg.cursize);
	client->voicePacketCount = 0;
}

/*
==================
SV_ClientWantsVoiceData
==================
*/
bool SV_ClientWantsVoiceData( int clientNum )
{
	assert(clientNum >= 0 && clientNum < MAX_CLIENTS);
	return svs.clients[clientNum].sendVoice;
}

/*
==================
SV_ClientHasClientMuted
==================
*/
bool SV_ClientHasClientMuted( int listener, int talker )
{
	assert(listener >= 0 && listener < MAX_CLIENTS);
	assert(talker >= 0 && talker < MAX_CLIENTS);
	return svs.clients[listener].muteList[talker];
}

/*
==================
SV_QueueVoicePacket
==================
*/
void SV_QueueVoicePacket( int talkerNum, int clientNum, VoicePacket_t *voicePacket )
{
	client_t *talker;
	client_t *client;

	assert(talkerNum >= 0 && talkerNum < MAX_CLIENTS);
	assert(clientNum >= 0 && clientNum < MAX_CLIENTS);

	talker = &svs.clients[talkerNum];
	client = &svs.clients[clientNum];

	if ( client->voicePacketCount < MAX_VOICE_PACKETS )
	{
		client->voicePackets[client->voicePacketCount].dataSize = voicePacket->dataSize;
		memcpy(client->voicePackets[client->voicePacketCount].data, voicePacket->data, voicePacket->dataSize);
		assert(talkerNum == static_cast<byte>(talkerNum));
		client->voicePackets[client->voicePacketCount].talker = talkerNum;
		client->voicePacketCount++;
	}
}

/*
==================
SV_UserVoice
==================
*/
void SV_UserVoice( client_t *cl, msg_t *msg )
{
	VoicePacket_t voicePacket;
	int unused = 0;
	int packetCount;
	int packet;

	if ( !sv_voice->current.boolean )
	{
		return;
	}

	packetCount = MSG_ReadByte(msg);
	assert(cl->gentity);

	for ( packet = 0; packet < packetCount; packet++ )
	{
		voicePacket.dataSize = MSG_ReadByte(msg);

		if ( voicePacket.dataSize <= 0 || voicePacket.dataSize > MAX_VOICE_PACKET_DATA )
		{
			Com_Printf("Received invalid voice packet of size %i from %s\n", voicePacket.dataSize, cl->name);
			return;
		}

		assert(msg->data);
		assert(voicePacket.data);

		MSG_ReadData(msg, voicePacket.data, voicePacket.dataSize);
		G_BroadcastVoice(cl->gentity, &voicePacket);
	}
}

/*
==================
SV_PreGameUserVoice
==================
*/
void SV_PreGameUserVoice( client_t *cl, msg_t *msg )
{
	VoicePacket_t voicePacket;
	int unused = 0;
	int packetCount;
	int packet;
	int listener;
	int talker;

	if ( !sv_voice->current.boolean )
	{
		return;
	}

	talker = cl - svs.clients;
	assert(talker >= 0 && talker < MAX_CLIENTS);
	packetCount = MSG_ReadByte(msg);

	for ( packet = 0; packet < packetCount; packet++ )
	{
		voicePacket.dataSize = MSG_ReadShort(msg);

		if ( voicePacket.dataSize <= 0 || voicePacket.dataSize > MAX_VOICE_PACKET_DATA )
		{
			Com_Printf("Received invalid voice packet of size %i from %s\n", voicePacket.dataSize, cl->name);
			return;
		}

		assert(msg->data);
		assert(voicePacket.data);

		MSG_ReadData(msg, voicePacket.data, voicePacket.dataSize);

		for ( listener = 0; listener < MAX_CLIENTS; listener++ )
		{
			if ( listener == talker )
				continue;

			if ( svs.clients[listener].state < CS_CONNECTED )
				continue;

			if ( SV_ClientHasClientMuted(listener, talker) )
				continue;

			if ( SV_ClientWantsVoiceData(listener) )
			{
				SV_QueueVoicePacket(talker, listener, &voicePacket);
			}
		}
	}
}

