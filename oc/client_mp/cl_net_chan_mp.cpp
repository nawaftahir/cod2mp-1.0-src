#include "common_types.h"
#include "imports.h"
#include <stddef.h>
#include <stdio.h>

extern clientConnection_t clientConnections[1];
#define clc clientConnections[0]
extern byte *cl_ptr;
extern clientActive_t clients[1];
extern dvar_t *net_profile;
#if defined(COD2_X64) || defined(_M_X64) || defined(__x86_64__)
extern int clc_x64_lastChallenge;
#endif

#define CLC_CHALLENGE_OFF offsetof(clientConnection_t, challenge)
#define CLC_RELIABLEACK_OFF offsetof(clientConnection_t, reliableAcknowledge)
#define CLC_RELIABLECMDS_OFF offsetof(clientConnection_t, reliableCommands)
#define CLC_SERVERMSGSEQ_OFF offsetof(clientConnection_t, serverMessageSequence)
#define CLC_SERVERCMDSEQ_OFF offsetof(clientConnection_t, serverCommandSequence)
#define CLC_SERVERCMDS_OFF offsetof(clientConnection_t, serverCommands)
#define CLC_NETCHAN_OFF offsetof(clientConnection_t, netchan)
#define CLC_NETCHAN_PPROF_OFF (offsetof(clientConnection_t, netchan) + offsetof(netchan_t, pProf))
#define CLC_POOBPROF_OFF offsetof(clientConnection_t, pOOBProf)

#define CL_SERVERID_OFF 0x8628

#define RELIABLECMD_SIZE 1024

extern bool Netchan_TransmitNextFragment(netchan_t *chan);
extern bool Netchan_Transmit(netchan_t *chan, int length, const byte *data);
extern void NetProf_PrepProfiling(netProfileInfo_t **prof);
extern void NetProf_AddPacket(netProfileStream_t *stream, int iLength, qboolean bFragment);
extern void NetProf_UpdateStatistics(netProfileStream_t *stream);
extern int Com_sprintf(char *dest, size_t size, const char *format, ...);
extern void Com_Printf(const char *fmt, ...);
extern void CL_DrawString(int x, int y, const char *str, int color, int size);
extern bool NET_SendPacket( netsrc_t sock, int length, const void *data, netadr_t to );
static int cl_decode_count = 0;

void CL_Netchan_Decode(byte *data, int size);
void CL_Netchan_TransmitNextFragment(netchan_t *chan);
void CL_Netchan_Transmit(netchan_t *chan, byte *data, int length);
void CL_Netchan_AddOOBProfilePacket(int iLength);
void CL_Netchan_SendOOBPacket(int iLength, const void *pData, netadr_t to);
void CL_Netchan_PrintProfileStats(qboolean bPrintToConsole);

static void CL_Netchan_Encode(byte *data, int size)
{
    const char *string;
    byte key;
    int index;
    int i;

    string = clc.serverCommands[clc.serverCommandSequence & 0x7f];
    index = 0;
    key = (byte)clients[0].serverId ^ (byte)clc.challenge ^ (byte)clc.serverMessageSequence;

    for (i = 0; i < size; i++, data++) {
        if (!string[index])
            index = 0;
        key ^= string[index] << (i & 1);
        index++;
        *data ^= key;
    }
}

void CL_Netchan_Decode(byte *data, int size)
{
    byte *clc_base;
    int reliableAcknowledge;
    const char *string;
    byte key;
    int challenge;
    int index;
    int i;
#if defined(COD2_X64) || defined(_M_X64) || defined(__x86_64__)
    static int x64DecodeTraceCount;
    byte beforeTrace[16];
    int beforeTraceLen;
#endif

    clc_base = (byte *)&::clc;

    reliableAcknowledge = ((clientConnection_t *)clc_base)->reliableAcknowledge;
    string = ((clientConnection_t *)clc_base)->reliableCommands[reliableAcknowledge & 0x7f];

    index = 0;
    key = (byte)(((clientConnection_t *)clc_base)->serverMessageSequence);
    challenge = ((clientConnection_t *)clc_base)->challenge;
#if defined(COD2_X64) || defined(_M_X64) || defined(__x86_64__)
    if (!challenge) {
        challenge = clc_x64_lastChallenge;
    }
#endif
    key ^= (byte)challenge;

#if defined(COD2_X64) || defined(_M_X64) || defined(__x86_64__)
    beforeTraceLen = size < (int)sizeof(beforeTrace) ? size : (int)sizeof(beforeTrace);
    for (i = 0; i < beforeTraceLen; ++i) {
        beforeTrace[i] = data[i];
    }
#endif
    for (i = 0; i < size; i++, data++) {
        if (!string[index])
            index = 0;
        key ^= string[index] << (i & 1);
        index++;
        *data ^= key;
    }

#if defined(COD2_X64) || defined(_M_X64) || defined(__x86_64__)
    if (x64DecodeTraceCount < 12) {
        FILE *f = fopen("x64_netchan_trace.txt", "a");
        if (f) {
            fprintf(f,
                    "cldec[%d] size=%d relAck=%d seq=%d challenge=%d keyEnd=%02x string=\"%.48s\" before:",
                    x64DecodeTraceCount,
                    size,
                    reliableAcknowledge,
                    ((clientConnection_t *)clc_base)->serverMessageSequence,
                    challenge,
                    key,
                    string);
            for (i = 0; i < beforeTraceLen; ++i) {
                fprintf(f, " %02x", beforeTrace[i]);
            }
            fprintf(f, " after:");
            for (i = 0; i < beforeTraceLen; ++i) {
                fprintf(f, " %02x", data[i]);
            }
            fprintf(f, "\n");
            fclose(f);
        }
        x64DecodeTraceCount++;
    }
#endif
}

void CL_Netchan_TransmitNextFragment(netchan_t *chan)
{
    Netchan_TransmitNextFragment(chan);
}

void CL_Netchan_Transmit(netchan_t *chan, byte *data, int length)
{
    CL_Netchan_Encode(data + 9, length - 9);
    Netchan_Transmit(chan, length, data);
}

void CL_Netchan_AddOOBProfilePacket(int iLength)
{
    if (net_profile->current.integer)
    {
        NetProf_PrepProfiling(&clc.pOOBProf);
        NetProf_AddPacket(&clc.pOOBProf->send, iLength, 0);
    }
}

void CL_Netchan_SendOOBPacket(int iLength, const void *pData, netadr_t to)
{
    byte *clc_base;

    if (*(int *)pData != -1) {
        Com_Printf("CL_Netchan_SendOOBPacket used to send non-OOB packet.\n");
    }

    clc_base = (byte *)&::clc;
    NetProf_PrepProfiling(&((clientConnection_t *)clc_base)->pOOBProf);

    NET_SendPacket(NS_CLIENT1, iLength, pData, to);

    if (net_profile->current.integer == 0)   /* was dvar+8 (x86 current offset; x64 is 16) */
        return;

    NetProf_PrepProfiling(&((clientConnection_t *)clc_base)->pOOBProf);
    NetProf_AddPacket(&((clientConnection_t *)clc_base)->pOOBProf->send, iLength, 0);
}

// Original name unknown.
static void CL_Netchan_DrawProfileLine(const char *line, int y)
{
    CL_DrawString(0x20, y, line, 0, 0xa);
}

// retail 0x411650; Kisak name
void CL_Netchan_UpdateProfileStats(void)
{
    if (clc.netchan.pProf) {
        NetProf_UpdateStatistics(&clc.netchan.pProf->send);
        NetProf_UpdateStatistics(&clc.netchan.pProf->recieve);
    }

    if (clc.pOOBProf) {
        NetProf_UpdateStatistics(&clc.pOOBProf->send);
        NetProf_UpdateStatistics(&clc.pOOBProf->recieve);
    }
}

void CL_Netchan_PrintProfileStats(qboolean bPrintToConsole)
{
    char szLine[1024];
    int iTotalBPSSent = 0;
    int iTotalBPSRecieved = 0;
    int iYPos = 0x50;
    netProfileInfo_t *pOOBProf;
    netProfileInfo_t *pProf;

    if (clc.netchan.pProf) {
        NetProf_UpdateStatistics(&clc.netchan.pProf->send);
        NetProf_UpdateStatistics(&clc.netchan.pProf->recieve);
    }

    if (clc.pOOBProf) {
        NetProf_UpdateStatistics(&clc.pOOBProf->send);
        NetProf_UpdateStatistics(&clc.pOOBProf->recieve);
    }

    if (bPrintToConsole) {
        Com_Printf("\n\n");
    }

    Com_sprintf(szLine, 0x400, "====================");
    if (bPrintToConsole) {
        Com_Printf("%s\n", szLine);
    } else {
        iYPos = 0x5a;
        CL_Netchan_DrawProfileLine(szLine, iYPos);
    }

    Com_sprintf(szLine, 0x400, "Client Network Profile:");
    if (bPrintToConsole) {
        Com_Printf("%s\n\n", szLine);
    } else {
        iYPos += 0xa;
        CL_Netchan_DrawProfileLine(szLine, iYPos);
        iYPos += 0xa;
    }

    Com_sprintf(szLine, 0x400, "      Source    bps   max   min frag%%");
    if (bPrintToConsole) {
        Com_Printf("%s\n", szLine);
    } else {
        iYPos += 0xa;
        CL_Netchan_DrawProfileLine(szLine, iYPos);
    }

    if (clc.pOOBProf) {
        pOOBProf = clc.pOOBProf;
        iTotalBPSSent += pOOBProf->send.iBytesPerSecond;
        iTotalBPSRecieved += pOOBProf->recieve.iBytesPerSecond;

        Com_sprintf(szLine, 0x400, "    OOB Sent: %5i %5i %5i    -",
                    iTotalBPSSent,
                    pOOBProf->send.iLargestPacket,
                    pOOBProf->send.iSmallestPacket);
        if (bPrintToConsole) {
            Com_Printf("%s\n", szLine);
        } else {
            iYPos += 0xa;
            CL_Netchan_DrawProfileLine(szLine, iYPos);
        }

        Com_sprintf(szLine, 0x400, "OOB Recieved: %5i %5i %5i    -",
                    pOOBProf->recieve.iBytesPerSecond,
                    pOOBProf->recieve.iLargestPacket,
                    pOOBProf->recieve.iSmallestPacket);
        if (bPrintToConsole) {
            Com_Printf("%s\n", szLine);
        } else {
            iYPos += 0xa;
            CL_Netchan_DrawProfileLine(szLine, iYPos);
        }
    } else {

        Com_sprintf(szLine, 0x400, "    OOB Sent:     0     0     0    -");
        if (bPrintToConsole) {
            Com_Printf("%s\n", szLine);
        } else {
            iYPos += 0xa;
            CL_Netchan_DrawProfileLine(szLine, iYPos);
        }

        Com_sprintf(szLine, 0x400, "OOB Recieved:     0     0     0    -");
        if (bPrintToConsole) {
            Com_Printf("%s\n", szLine);
        } else {
            iYPos += 0xa;
            CL_Netchan_DrawProfileLine(szLine, iYPos);
        }
    }

    if (clc.netchan.pProf) {
        pProf = clc.netchan.pProf;
        iTotalBPSSent += pProf->send.iBytesPerSecond;
        iTotalBPSRecieved += pProf->recieve.iBytesPerSecond;

        Com_sprintf(szLine, 0x400, "        Sent: %5i %5i %5i  %3i%%",
                    pProf->send.iBytesPerSecond,
                    pProf->send.iLargestPacket,
                    pProf->send.iSmallestPacket,
                    pProf->send.iFragmentPercentage);
        if (bPrintToConsole) {
            Com_Printf("%s\n", szLine);
        } else {
            iYPos += 0xa;
            CL_Netchan_DrawProfileLine(szLine, iYPos);
        }

        Com_sprintf(szLine, 0x400, "    Recieved: %5i %5i %5i  %3i%%",
                    pProf->recieve.iBytesPerSecond,
                    pProf->recieve.iLargestPacket,
                    pProf->recieve.iSmallestPacket,
                    pProf->recieve.iFragmentPercentage);
        if (bPrintToConsole) {
            Com_Printf("%s\n", szLine);
        } else {
            iYPos += 0xa;
            CL_Netchan_DrawProfileLine(szLine, iYPos);
        }
    } else {

        Com_sprintf(szLine, 0x400, "        Sent:     0     0     0    0%");
        if (bPrintToConsole) {
            Com_Printf("%s\n", szLine);
        } else {
            iYPos += 0xa;
            CL_Netchan_DrawProfileLine(szLine, iYPos);
        }

        Com_sprintf(szLine, 0x400, "    Recieved:     0     0     0    0%");
        if (bPrintToConsole) {
            Com_Printf("%s\n", szLine);
        } else {
            iYPos += 0xa;
            CL_Netchan_DrawProfileLine(szLine, iYPos);
        }
    }

    Com_sprintf(szLine, 0x400, "       Total: %5i",
                iTotalBPSSent + iTotalBPSRecieved);
    if (bPrintToConsole) {
        Com_Printf("%s\n", szLine);
    } else {
        iYPos += 0xa;
        CL_Netchan_DrawProfileLine(szLine, iYPos);
    }
}

