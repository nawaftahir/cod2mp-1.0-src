#include "common_types.h"
#include "imports.h"
#include "www_download.h"
#include "cod2_feature_config.h"
#include "PC/qcommon/net_hardening.h"
#include <stdlib.h>

extern clientStatic_t cls;
#if COD2_IS_PATCH_13
extern dvar_t *com_developer;
extern const char* Dvar_GetString(const char *dvarName);
extern void FS_BuildOSPath(const char *base, const char *game, const char *qpath, char *ospath);
extern void Cbuf_ExecuteText(int execWhen, const char *text);
extern void Sys_OpenURL(const char *url, int activate);
extern int FS_ReadFile(const char *qpath, void **buffer);
extern void FS_FreeFile(void *buffer);
extern void FS_CopyFile(char *fromOSPath, char *toOSPath);
extern void Dvar_SetStringByName(const char *dvarName, const char *value);
void CL_WWWDownload(void);
void CL_ParseWWWDownload(msg_t *msg);
extern int CL_ClearStaticDownload(void);
#endif

extern int cl_connectedToPureServer;

/* svc_strings: server-command name table. Migrated from the ILP32 data blob
 * (build/native_gen/data32.c) to a typed C definition so it re-lays-out per
 * target -- 256 char* is 1024 B on x86 (byte-identical to the old blob) and
 * 2048 B on x64. The 8 live entries point at the reconstructed string literals
 * in common.c; entries [8..255] are NULL. (x64 port Stage 2 pilot; cf. the
 * noticeErrors / holdrand typed-data migrations.) */
static char *svc_strings[256] = {
    "svc_nop",
    "svc_gamestate",
    "svc_configstring",
    "svc_baseline",
    "svc_serverCommand",
    "svc_download",
    "svc_snapshot",
    "svc_EOF",
    /* [8 .. 255] implicitly NULL */
};

extern clientActive_t clients[1];
extern byte cls_ptr[];
extern clientConnection_t clientConnections[1];
#define clc clientConnections[0]
extern dvar_t *com_dedicated;
extern dvar_t *cl_paused;
extern dvar_t *net_lanauthorize;
extern byte **cl_packetdelay;
extern byte **download_ui_ptr;
extern const dvar_t *cl_shownet;
extern const dvar_t *cl_shownuments;

void CL_SystemInfoChanged(void);

void CL_ParseGamestate(msg_t *msg);
void CL_ParseDownload(msg_t *msg);
void CL_ParseSnapshot(msg_t *msg);
void CL_ParseServerMessage(msg_t *msg);


extern const char *Info_ValueForKey( const char *s, const char *key );
extern void Info_NextPair(const char **head, char *key, char *value);
extern void FS_PureServerSetLoadedIwds(const char *iwdSums, const char *iwdNames);
extern void FS_PureServerSetReferencedIwds(const char *iwdSums, const char *iwdNames);
extern void Dvar_SetFromStringByName(const char *dvarName, const char *string);
extern bool Dvar_GetBool(const char *dvarName);
extern void Dvar_SetCheatsToReset(void);
extern void Dvar_SetInt(dvar_t *dvar, int value);
extern void Con_Close(void);
extern void CL_ClearState(void);
extern void CL_SystemInfoChanged(void);
extern qboolean FS_ConditionalRestart(int checksumFeed);
extern qboolean Sys_IsLANAddress(netadr_t adr);
extern void CL_RequestAuthorization(void);
extern void CL_InitDownloads(void);
extern void CL_AddReliableCommand(const char *cmd);
extern void CL_WritePacket(void);
extern void CL_NextDownload(void);
extern void Com_Error(errorParm_t level, const char *fmt, ...);
extern void Com_Printf(const char *fmt, ...);
extern void Com_DPrintf(const char *fmt, ...);

// retail 0x411b50, unreferenced; Q3 name
void SHOWNET(msg_t *msg, const char *s)
{
    if (cl_shownet->current.integer >= 2)
        Com_Printf("%3i:%s\n", msg->readcount - 1, s);
}
extern int MSG_ReadLong(msg_t *msg);
extern int MSG_ReadByte(msg_t *msg);
extern int MSG_ReadShort(msg_t *msg);
extern int MSG_ReadBits(msg_t *msg, int bits);
extern int MSG_ReadBit(msg_t *msg);
extern char *MSG_ReadCommandString(msg_t *msg);
extern char *MSG_ReadBigString(msg_t *msg);
extern void MSG_ReadData(msg_t *msg, void *data, int len);
extern void MSG_ReadDeltaPlayerstate(msg_t *msg, playerState_t *from, playerState_t *to);
extern qboolean MSG_ReadDeltaEntity(msg_t *msg, entityState_t *from, entityState_t *to, int number);
extern qboolean MSG_ReadDeltaClient(msg_t *msg, clientState_t *from, clientState_t *to, int number);
extern void MSG_Init(msg_t *buf, byte *data, int length);
extern int MSG_ReadBitsCompress(byte *from, byte *to, int size);
extern char *va( const char *format, ... );
extern void I_strncpyz(char *dest, const char *src, int destsize);
extern fileHandle_t FS_SV_FOpenFileWrite(const char *filename);
extern void FS_SV_Rename(const char *from, const char *to);
extern int FS_Write(const void *buffer, int len, fileHandle_t h);
extern void FS_FCloseFile(fileHandle_t h);

static int CL_ShowNetValue(void)
{
    return cl_shownet ? cl_shownet->current.integer : 0;
}

static qboolean CL_ShowPacketsEnabled(void)
{
    return cl_shownuments && cl_shownuments->current.enabled;
}

static void CL_DeltaEntity(msg_t *msg, clSnapshot_t *frame, int newnum, entityState_t *old, qboolean unchanged)
{
    entityState_t *state;

    state = &clients->parseEntities[clients->parseEntitiesNum & 0x7ff];

    if (unchanged) {
        *state = *old;
    } else if (MSG_ReadDeltaEntity(msg, old, state, newnum)) {
        // entity was removed
        return;
    }

    clients->parseEntitiesNum++;
    frame->numEntities++;
}

static void CL_DeltaClient(msg_t *msg, clSnapshot_t *frame, int newnum, clientState_t *old, qboolean unchanged)
{
    clientState_t *state;

    state = &clients->parseClients[clients->parseClientsNum & 0x7ff];

    if (unchanged) {
        *state = *old;
    } else if (MSG_ReadDeltaClient(msg, old, state, newnum)) {
        return;
    }

    clients->parseClientsNum++;
    frame->numClients++;
}

void CL_ParsePacketEntities(msg_t *msg, clSnapshot_t *oldframe, clSnapshot_t *newframe)
{
    int newnum;
    entityState_t *oldstate;
    int oldindex;
    int oldnum;

    newframe->parseEntitiesNum = clients->parseEntitiesNum;
    newframe->numEntities = 0;

    // delta from the entities present in oldframe
    oldindex = 0;
    oldstate = NULL;
    if (!oldframe) {
        oldnum = 99999;
    } else if (oldindex >= oldframe->numEntities) {
        oldnum = 99999;
    } else {
        oldstate = &clients->parseEntities[(oldframe->parseEntitiesNum + oldindex) & 0x7ff];
        oldnum = oldstate->number;
    }

    while (!msg->overflowed) {
        newnum = MSG_ReadBits(msg, 10);

        if (newnum == 0x3ff)
            break;

        if (msg->readcount > msg->cursize)
            Com_Error(ERR_DROP, "\x15" "CL_ParsePacketEntities: end of message");

        while (oldnum < newnum && !msg->overflowed) {
            // one or more entities from the old packet are unchanged
            if (cl_shownet->current.integer == 3)
                Com_Printf("%3i:  unchanged: %i\n", msg->readcount, oldnum);

            CL_DeltaEntity(msg, newframe, oldnum, oldstate, 1);

            oldindex++;
            if (oldindex >= oldframe->numEntities) {
                oldnum = 99999;
            } else {
                oldstate = &clients->parseEntities[(oldframe->parseEntitiesNum + oldindex) & 0x7ff];
                oldnum = oldstate->number;
            }
        }

        if (oldnum == newnum) {
            // delta from previous state
            if (cl_shownet->current.integer == 3)
                Com_Printf("%3i:  delta: %i\n", msg->readcount, newnum);

            CL_DeltaEntity(msg, newframe, newnum, oldstate, 0);

            oldindex++;
            if (oldindex >= oldframe->numEntities) {
                oldnum = 99999;
            } else {
                oldstate = &clients->parseEntities[(oldframe->parseEntitiesNum + oldindex) & 0x7ff];
                oldnum = oldstate->number;
            }
            continue;
        }

        // delta from baseline
        if (cl_shownet->current.integer == 3)
            Com_Printf("%3i:  baseline: %i\n", msg->readcount, newnum);

        CL_DeltaEntity(msg, newframe, newnum, &clients->entityBaselines[newnum], 0);
    }

    // any remaining entities in the old frame are copied over
    while (oldnum != 99999 && !msg->overflowed) {
        if (cl_shownet->current.integer == 3)
            Com_Printf("%3i:  unchanged: %i\n", msg->readcount, oldnum);

        CL_DeltaEntity(msg, newframe, oldnum, oldstate, 1);

        oldindex++;
        if (oldindex >= oldframe->numEntities) {
            oldnum = 99999;
        } else {
            oldstate = &clients->parseEntities[(oldframe->parseEntitiesNum + oldindex) & 0x7ff];
            oldnum = oldstate->number;
        }
    }

    if (cl_shownuments->current.enabled)
        Com_Printf("Entities in packet: %i\n", newframe->numEntities);
}

void CL_ParsePacketClients(msg_t *msg, clSnapshot_t *oldframe, clSnapshot_t *newframe)
{
    int newnum;
    clientState_t *oldstate;
    int oldindex;
    int oldnum;
    clientState_t dummy;

    newframe->parseClientsNum = clients->parseClientsNum;
    newframe->numClients = 0;

    oldindex = 0;
    oldstate = NULL;
    if (!oldframe) {
        oldnum = 99999;
    } else if (oldindex >= oldframe->numClients) {
        oldnum = 99999;
    } else {
        oldstate = &clients->parseClients[(oldframe->parseClientsNum + oldindex) & 0x7ff];
        oldnum = oldstate->clientIndex;
    }

    while (!msg->overflowed) {
        if (!MSG_ReadBit(msg))
            break;

        newnum = MSG_ReadBits(msg, 6);

        if (msg->readcount > msg->cursize)
            Com_Error(ERR_DROP, "\x15" "CL_ParsePacketClients: end of message");

        while (oldnum < newnum) {
            if (cl_shownet->current.integer == 3)
                Com_Printf("%3i:  unchanged: %i\n", msg->readcount, oldnum);

            CL_DeltaClient(msg, newframe, oldnum, oldstate, 1);

            oldindex++;
            if (oldindex >= oldframe->numClients) {
                oldnum = 99999;
            } else {
                oldstate = &clients->parseClients[(oldframe->parseClientsNum + oldindex) & 0x7ff];
                oldnum = oldstate->clientIndex;
            }
        }

        if (oldnum == newnum) {
            if (cl_shownet->current.integer == 3)
                Com_Printf("%3i:  delta: %i\n", msg->readcount, newnum);

            CL_DeltaClient(msg, newframe, newnum, oldstate, 0);

            oldindex++;
            if (oldindex >= oldframe->numClients) {
                oldnum = 99999;
            } else {
                oldstate = &clients->parseClients[(oldframe->parseClientsNum + oldindex) & 0x7ff];
                oldnum = oldstate->clientIndex;
            }
            continue;
        }

        if (cl_shownet->current.integer == 3)
            Com_Printf("%3i:  baseline: %i\n", msg->readcount, newnum);

        memset(&dummy, 0, sizeof(dummy));
        CL_DeltaClient(msg, newframe, newnum, &dummy, 0);
    }

    while (oldnum != 99999 && !msg->overflowed) {
        if (cl_shownet->current.integer == 3)
            Com_Printf("%3i:  unchanged: %i\n", msg->readcount, oldnum);

        CL_DeltaClient(msg, newframe, oldnum, oldstate, 1);

        oldindex++;
        if (oldindex >= oldframe->numClients) {
            oldnum = 99999;
        } else {
            oldstate = &clients->parseClients[(oldframe->parseClientsNum + oldindex) & 0x7ff];
            oldnum = oldstate->clientIndex;
        }
    }

    if (cl_shownuments->current.enabled)
        Com_Printf("Clients in packet: %i\n", newframe->numClients);
}

void CL_ParseSnapshot(msg_t *msg)
{
    clSnapshot_t *old;
    clSnapshot_t *newSnap;
    int deltaNum;
    int oldMessageNum;
    int i;
    int packetNum;

    LargeLocal newSnap_large_local(sizeof(clSnapshot_t));
    newSnap = (clSnapshot_t *)newSnap_large_local.GetBuf();

    memset(newSnap, 0, sizeof(*newSnap));

    // get the reliable sequence acknowledge number
    newSnap->serverCommandNum = clc.serverCommandSequence;

    newSnap->serverTime = MSG_ReadLong(msg);

    newSnap->messageNum = clc.serverMessageSequence;

    deltaNum = MSG_ReadByte(msg);
    if (!deltaNum) {
        newSnap->deltaNum = -1;
    } else {
        newSnap->deltaNum = newSnap->messageNum - deltaNum;
    }
    newSnap->snapFlags = MSG_ReadByte(msg);

    // an uncompressed frame is always valid
    if (newSnap->deltaNum <= 0) {
        newSnap->valid = 1;
        old = NULL;
        clc.demowaiting = 0;
    } else {
        old = &clients->snapshots[newSnap->deltaNum & 0x1f];
        if (!old->valid) {
            // should never happen
            Com_Printf("Delta from invalid frame (not supposed to happen!).\n");
        } else if (old->messageNum != newSnap->deltaNum) {
            // the frame that the server did the delta from has been overwritten
            Com_DPrintf("Delta frame too old.\n");
        } else if (clients->parseEntitiesNum - old->parseEntitiesNum > 0x780) {
            Com_DPrintf("Delta parseEntitiesNum too old.\n");
        } else if (clients->parseClientsNum - old->parseClientsNum > 0x780) {
            Com_DPrintf("Delta parseClientsNum too old.\n");
        } else {
            newSnap->valid = 1;
        }
    }

    if (cl_shownet->current.integer >= 2)
        Com_Printf("%3i:%s\n", msg->readcount - 1, "playerstate");

    if (old) {
        MSG_ReadDeltaPlayerstate(msg, &old->ps, &newSnap->ps);
    } else {
        MSG_ReadDeltaPlayerstate(msg, NULL, &newSnap->ps);
    }

    if (cl_shownet->current.integer >= 2)
        Com_Printf("%3i:%s\n", msg->readcount - 1, "packet entities");

    CL_ParsePacketEntities(msg, old, newSnap);

    if (cl_shownet->current.integer >= 2)
        Com_Printf("%3i:%s\n", msg->readcount - 1, "packet clients");

    CL_ParsePacketClients(msg, old, newSnap);

    // if not valid, dump the entire thing now that it has been properly read
    if (msg->overflowed) {
        newSnap->valid = 0;
        return;
    }

    if (!newSnap->valid)
        return;

    // clear the valid flags of any snapshots between the last received and this one
    oldMessageNum = clients->snap.messageNum + 1;

    if (newSnap->messageNum - oldMessageNum >= 32)
        oldMessageNum = newSnap->messageNum - 31;

    for ( ; oldMessageNum < newSnap->messageNum; oldMessageNum++)
        clients->snapshots[oldMessageNum & 0x1f].valid = 0;

    clients->oldSnapServerTime = clients->snap.serverTime;
    clients->snap = *newSnap;
    clients->snap.ping = 999;

    // calculate ping time
    for (i = 0; i < 32; i++) {
        packetNum = (clc.netchan.outgoingSequence - 1 - i) & 0x1f;
        if (clients->snap.ps.commandTime >= clients->outPackets[packetNum].p_serverTime) {
            clients->snap.ping = cls.realtime - clients->outPackets[packetNum].p_realtime;
            break;
        }
    }

    // save the frame off in the backup array for later delta comparisons
    clients->snapshots[clients->snap.messageNum & 0x1f] = clients->snap;

    if (cl_shownet->current.integer == 3)
        Com_Printf("   snapshot:%i  delta:%i  ping:%i\n", clients->snap.messageNum, clients->snap.deltaNum, clients->snap.ping);

    clients->newSnapshots = 1;
}


void CL_SystemInfoChanged(void)
{
    char *key;
    char *value;
    char *systemInfo;
    const char *s;

    LargeLocal key_large_local(0x2000);
    key = (char *)key_large_local.GetBuf();

    LargeLocal value_large_local(0x2000);
    value = (char *)value_large_local.GetBuf();

    systemInfo = clients[0].gameState.stringData + clients[0].gameState.stringOffsets[1];

    {
        const char *sid_str = Info_ValueForKey(systemInfo, "sv_serverid");
        int sid_val = atoi(sid_str);
        clients[0].serverId = sid_val;
    }

    if ((&::clc)->demoplaying != 0) {
        return;
    }

    s = Info_ValueForKey(systemInfo, "sv_cheats");
    if (atoi(s) == 0) {

        Dvar_SetCheatsToReset();
    }

    s = Info_ValueForKey(systemInfo, "sv_iwds");

    FS_PureServerSetLoadedIwds(s, Info_ValueForKey(systemInfo, "sv_iwdNames"));

    s = Info_ValueForKey(systemInfo, "sv_referencedIwds");

    FS_PureServerSetReferencedIwds(s, Info_ValueForKey(systemInfo, "sv_referencedIwdNames"));

    if (!com_dedicated->current.enabled) {

        s = systemInfo;
        while (s) {
            Info_NextPair(&s, key, value);
            if (key[0] == '\0')
                break;
#if COD2_FEATURE_NET_HARDENING

            if (!NetHardening_ServerMaySetCvar(key)) {
                Com_Printf("WARNING: server tried to set protected cvar '%s' "
                           "via systeminfo - ignored\n",
                           key);
                continue;
            }
#endif

            Dvar_SetFromStringByName(key, value);
        }
    }

    cl_connectedToPureServer = (int)Dvar_GetBool("sv_pure");

}

void CL_ParseGamestate(msg_t *msg)
{
    clientConnection_t *conn;
    clientActive_t *cla;
    int newnum;
    int i;
    char *s;
    int len;
    entityState_t nullstate;

    Con_Close();

    conn = &::clc;
    conn->connectPacketCount = 0;

    CL_ClearState();

    conn->serverCommandSequence = MSG_ReadLong(msg);

    cla = clients;
    cla->gameState.dataCount = 1;

    for (;;) {

        int cmd = MSG_ReadByte(msg);

        if (cmd == 7)
            break;

        if (cmd == 2) {

            i = MSG_ReadShort(msg);
            if (i < 0 || i >= 0x800) {
                Com_Error(ERR_DROP, "\x15"
                             "configstring > MAX_CONFIGSTRINGS");
            }

            s = MSG_ReadBigString(msg);

            len = strlen(s);

            cla = clients;
            if (len + 1 + cla->gameState.dataCount > MAX_GAMESTATE_CHARS) {
                Com_Error(ERR_DROP, "\x15MAX_GAMESTATE_CHARS exceeded");
            }

            cla->gameState.stringOffsets[i] = cla->gameState.dataCount;

            memcpy(cla->gameState.stringData + cla->gameState.dataCount, s, len + 1);

            cla->gameState.dataCount = cla->gameState.dataCount + len + 1;
        } else if (cmd == 3) {

            newnum = MSG_ReadBits(msg, 10);
            if (newnum < 0 || newnum >= 0x400) {
                Com_Error(ERR_DROP, "\x15"
                             "Baseline number out of range: %i",
                          newnum);
            }

            memset(&nullstate, 0, sizeof(entityState_t));

            MSG_ReadDeltaEntity(msg, &nullstate,
                                &cla->entityBaselines[newnum],
                                newnum);
        } else {

            Com_Error(ERR_DROP, "\x15"
                         "CL_ParseGamestate: bad command byte");
        }
    }

    conn = &::clc;
    conn->clientNum = MSG_ReadLong(msg);

    conn->checksumFeed = MSG_ReadLong(msg);

    CL_SystemInfoChanged();

    FS_ConditionalRestart(conn->checksumFeed);

    if (net_lanauthorize->current.enabled || !Sys_IsLANAddress(conn->serverAddress)) {
        CL_RequestAuthorization();
    }

    CL_InitDownloads();

    Dvar_SetInt((dvar_t *)cl_paused, 0);
}

void CL_ParseDownload(msg_t *msg)
{
    int size;
    byte *data;
    int block;

    LargeLocal data_large_local(MAX_MSGLEN);
    data = (byte *)data_large_local.GetBuf();

    // read the data
    block = MSG_ReadShort(msg);

#if COD2_IS_PATCH_13

    if (block == -1) {
        if (!cls.wwwDlInProgress) {
            CL_ParseWWWDownload(msg);
        } else {

            MSG_ReadCommandString(msg);
            MSG_ReadLong(msg);
            MSG_ReadLong(msg);
        }
        return;
    }
#endif

    if (!block) {
        // block zero is special, contains file size
        clc.downloadSize = MSG_ReadLong(msg);
        legacyHacks->cl_downloadSize = clc.downloadSize;

        if (clc.downloadSize < 0) {
            Com_Error(ERR_DROP, va("%s", MSG_ReadCommandString(msg)));
            return;
        }
    }

    size = MSG_ReadShort(msg);
    if (size > 0) {
        MSG_ReadData(msg, data, size);
    }

    if (clc.downloadBlock != block) {
        Com_DPrintf("CL_ParseDownload: Expected block %d, got %d\n", clc.downloadBlock, block);
        if (block > clc.downloadBlock) {
            Com_DPrintf("CL_ParseDownload: Sending retransmit request to get the missed block\n");
            CL_AddReliableCommand(va("retransdl %d", clc.downloadBlock));
        }
        return;
    }

#if COD2_FEATURE_NET_HARDENING

    if (clc.downloadTempName[0] &&
        !NetHardening_ValidDownloadName(clc.downloadTempName, 0)) {
        Com_Printf("WARNING: refusing malicious download temp path '%s'\n",
                   clc.downloadTempName);
        CL_AddReliableCommand("stopdl");
        clc.downloadName[0] = 0;
        clc.downloadTempName[0] = 0;
        return;
    }
    if (clc.downloadName[0] &&
        !NetHardening_ValidDownloadName(clc.downloadName, 1)) {
        Com_Printf("WARNING: refusing malicious download name '%s'\n",
                   clc.downloadName);
        CL_AddReliableCommand("stopdl");
        clc.downloadName[0] = 0;
        clc.downloadTempName[0] = 0;
        return;
    }
#endif

    // open the file if not opened yet
    if (!clc.download) {
        if (!*clc.downloadTempName) {
            Com_Printf("Server sending download, but no download was requested\n");
            CL_AddReliableCommand("stopdl");
            return;
        }

        clc.download = FS_SV_FOpenFileWrite(clc.downloadTempName);

        if (!clc.download) {
            Com_Printf("Could not create %s\n", clc.downloadTempName);
            CL_AddReliableCommand("stopdl");
            CL_NextDownload();
            return;
        }
    }

    if (size) {
        FS_Write(data, size, clc.download);
    }

    CL_AddReliableCommand(va("nextdl %d", clc.downloadBlock));
    clc.downloadBlock++;

    clc.downloadCount += size;

    // So UI gets access to it
    legacyHacks->cl_downloadCount = clc.downloadCount;

    if (!size) {
        // A zero length block means EOF
        if (clc.download) {
            FS_FCloseFile(clc.download);

            clc.download = 0;
            // rename the file
            FS_SV_Rename(clc.downloadTempName, clc.downloadName);
        }
        *clc.downloadTempName = *clc.downloadName = 0;
        legacyHacks->cl_downloadName[0] = 0;

        // send intentions now
        // We need this because without it, we would hold the last nextdl and then start
        // loading right away.  If we take a while to load, the server is happily trying
        // to send us that last block over and over.
        // Write it twice to help make sure we acknowledge the download
        CL_WritePacket();
        CL_WritePacket();

        // get another file if needed
        CL_NextDownload();
    }
}

#if COD2_IS_PATCH_13

void CL_ParseWWWDownload(msg_t *msg)
{
    char toOSPath[256];
    const char *fs_homepath = Dvar_GetString("fs_homepath");

    I_strncpyz(cls.originalDownloadName, cls.downloadName, sizeof(cls.originalDownloadName));
    I_strncpyz(cls.downloadName, MSG_ReadCommandString(msg), sizeof(cls.downloadName));
    cls.downloadSize = MSG_ReadLong(msg);
    cls.downloadFlags = MSG_ReadLong(msg);

#    if COD2_FEATURE_NET_HARDENING && COD2_FEATURE_WWW_DOWNLOAD

    if (!(cls.downloadFlags & 0x2) && !NetHardening_ValidUrl(cls.downloadName)) {
        Com_Printf("WARNING: refusing wwwdl redirect to non-http(s) URL '%s'\n",
                   cls.downloadName);
        CL_AddReliableCommand("wwwdl fail");
        DL_CancelDownload();
        cls.wwwDlInProgress = 0;
        return;
    }
    if (cls.originalDownloadName[0] &&
        !NetHardening_ValidDownloadName(cls.originalDownloadName, 1)) {
        Com_Printf("WARNING: refusing malicious wwwdl local name '%s'\n",
                   cls.originalDownloadName);
        CL_AddReliableCommand("wwwdl fail");
        DL_CancelDownload();
        cls.wwwDlInProgress = 0;
        return;
    }
#    endif

    *(int *)((*download_ui_ptr) + 0x10) = cls.downloadSize;

    if (cls.downloadFlags & 0x2) {
        Sys_OpenURL(cls.downloadName, 1);
        Cbuf_ExecuteText(2, "quit\n");
        CL_AddReliableCommand("wwwdl bbl8r");
        DL_CancelDownload();
        cls.wwwDlInProgress = 0;
        return;
    }

    Com_DPrintf("Server redirected download: %s\n", cls.downloadName);
    cls.wwwDlInProgress = 1;
    CL_AddReliableCommand("wwwdl ack");

    FS_BuildOSPath(fs_homepath, cls.downloadTempName, "", toOSPath);
    I_strncpyz(cls.downloadTempName, toOSPath, sizeof(cls.downloadTempName));

    if (cls.downloadTempName[0])
        cls.downloadTempName[strlen(cls.downloadTempName) - 1] = '\0';

    if (DL_BeginDownload(cls.downloadTempName, cls.downloadName, com_developer->current.integer)) {
        if (cls.downloadFlags & 0x1) {
            CL_AddReliableCommand("wwwdl bbl8r");
            cls.wwwDlDisconnected = 1;
        }
        return;
    }

    CL_AddReliableCommand("wwwdl fail");
    DL_CancelDownload();
    cls.wwwDlInProgress = 0;
    Com_Printf("Failed to initialize download for '%s'\n", cls.downloadName);
    if (cls.downloadFlags & 0x1) {
        CL_AddReliableCommand("wwwdl bbl8r");
        cls.wwwDlDisconnected = 1;
    }
}

void CL_WWWDownload(void)
{
    const char *fs_homepath = Dvar_GetString("fs_homepath");
    dlStatus_t st;

    DL_DownloadLoop();
    st = DL_GetStatus();

    if (st == DL_STATUS_IN_PROGRESS || st == DL_STATUS_IDLE)
        return;

    if (DL_DLIsMotd()) {
        if (st == DL_STATUS_DONE) {
            void *buf;
            if (FS_ReadFile("motd.txt", &buf) >= 0) {
                Dvar_SetStringByName("motd", (const char *)buf);
                FS_FreeFile(buf);
            }
        }
        cls.wwwDlInProgress = 0;
        return;
    }

    if (st == DL_STATUS_DONE) {
        char toOSPath[256];

        cls.download = 0;
        FS_BuildOSPath(fs_homepath, cls.downloadName, "", toOSPath);
        if (toOSPath[0])
            toOSPath[strlen(toOSPath) - 1] = '\0';

        if (rename(cls.downloadTempName, toOSPath) != 0) {
            FS_CopyFile(cls.downloadTempName, toOSPath);
            remove(cls.downloadTempName);
        }

        cls.downloadTempName[0] = '\0';
        cls.downloadName[0] = '\0';
        *(char *)((*download_ui_ptr) + 0x1c) = 0;

        if (cls.wwwDlDisconnected)
            Cbuf_ExecuteText(2, "reconnect\n");
        else
            CL_AddReliableCommand("wwwdl done");

        cls.wwwDlInProgress = 0;
        CL_NextDownload();
    } else {
        if (cls.wwwDlDisconnected) {
            char *err = va("Download failure while getting '%s'\n", cls.downloadName);
            cls.wwwDlDisconnected = 0;
            CL_ClearStaticDownload();
            Com_Error(ERR_DROP, "%s", err);
            return;
        }
        Com_Printf("Download failure while getting '%s'\n", cls.downloadName);
        CL_AddReliableCommand("wwwdl fail");
        cls.wwwDlInProgress = 0;
    }
}
#endif

void CL_ParseCommandString(msg_t *msg)
{
    char *s;
    int seq;
    int index;

    seq = MSG_ReadLong(msg);
    s = MSG_ReadCommandString(msg);

    if (clc.serverCommandSequence >= seq) {
        return;
    }
    clc.serverCommandSequence = seq;

    index = seq & 0x7f;
    I_strncpyz(clc.serverCommands[index], s, sizeof(clc.serverCommands[index]));
}

void CL_ParseServerMessage(msg_t *msg)
{
    byte *msgCompressed_buf;
    msg_t msgCompressed;
    int cmd;
#if defined(COD2_X64) || defined(_M_X64) || defined(__x86_64__)
    static int x64TraceCount;
#endif

    LargeLocal msgCompressed_buf_large_local(MAX_MSG_DECOMPRESS_BYTES);
    msgCompressed_buf = (byte *)msgCompressed_buf_large_local.GetBuf();

    if (cl_shownet->current.integer == 1) {

        Com_Printf("%i ", msg->cursize);
    } else if (cl_shownet->current.integer >= 2) {

        Com_Printf("------------------\n");
    }

    MSG_Init(&msgCompressed, msgCompressed_buf, MAX_MSG_DECOMPRESS_BYTES);

    msgCompressed.cursize = MSG_ReadBitsCompress(msg->data + msg->readcount,
                                                 msgCompressed_buf,
                                                 msg->cursize - msg->readcount);

#if defined(COD2_X64) || defined(_M_X64) || defined(__x86_64__)
    if (x64TraceCount < 16) {
        FILE *f = fopen("x64_msg_trace.txt", x64TraceCount ? "a" : "w");
        if (f) {
            int i;
            fprintf(f, "msg[%d] srcSize=%d srcRead=%d decSize=%d first:",
                    x64TraceCount, msg->cursize, msg->readcount, msgCompressed.cursize);
            for (i = 0; i < msgCompressed.cursize && i < 32; i++) {
                fprintf(f, " %02x", msgCompressed_buf[i]);
            }
            fprintf(f, "\n");
            fclose(f);
        }
        x64TraceCount++;
    }
#endif

    while (!msgCompressed.overflowed) {

        cmd = MSG_ReadByte(&msgCompressed);

        if (cmd == 7) {

            if (cl_shownet->current.integer >= 2) {
                Com_Printf("%3i:%s\n", msgCompressed.readcount - 1, "END OF MESSAGE");
            }
            break;
        }

        if (cl_shownet->current.integer >= 2) {
            if (svc_strings[cmd] == NULL) {
                Com_Printf("%3i:BAD CMD %i\n", msgCompressed.readcount - 1, cmd);
            } else {
                Com_Printf("%3i:%s\n", msgCompressed.readcount - 1, svc_strings[cmd]);
            }
        }

        switch (cmd) {
        case 0:

            break;
        case 4:
            CL_ParseCommandString(&msgCompressed);
            break;
        case 1:
            CL_ParseGamestate(&msgCompressed);
            break;
        case 6:
            CL_ParseSnapshot(&msgCompressed);
            break;
        case 5:
            CL_ParseDownload(&msgCompressed);
            break;
        default:

            Com_Error(ERR_DROP, "\x15"
                         "CL_ParseServerMessage: Illegible server message %d\n",
                      cmd);
            break;
        }
    }

}
