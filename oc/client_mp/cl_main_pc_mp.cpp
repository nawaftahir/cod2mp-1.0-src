#include "common_types.h"
#include "imports.h"
#define MAX_GLOBALSERVERS 20000

extern "C" __declspec(dllimport) unsigned short __stdcall ntohs(unsigned short netshort);

extern int qport;
extern ping_t cl_pinglist[];
extern clientActive_t clients[1];
extern clientConnection_t clientConnections[1];
#define clc clientConnections[0]
/* dvar globals */
extern const dvar_t *cl_serverStatusResendTime;
extern dvar_t *com_sv_running;
extern clientStatic_t cls;

typedef struct serverStatusRequest_s {
    char string[0x2000];
    netadr_t address;
    int time;
    int startTime;
    qboolean pending;
    qboolean print;
    qboolean retrieved;
} serverStatusRequest_t;

COD2_ASSERT_FIELD(serverStatusRequest_t, address, 0x2000);
COD2_ASSERT_FIELD(serverStatusRequest_t, time, 0x2014);
COD2_ASSERT_FIELD(serverStatusRequest_t, retrieved, 0x2024);
COD2_ASSERT_SIZE(serverStatusRequest_t, 0x2028);

static serverStatusRequest_t cl_serverStatusList[16];
static int serverStatusCount;
static Bool s_playerMute[64];
extern int NET_CompareAdrSigned(netadr_t *a, netadr_t *b);
extern qboolean NET_CompareAdr(netadr_t a, netadr_t b);
extern void qsort(void *base, unsigned int nmemb, unsigned int size, int (*compar)(const void *, const void *));
extern int atoi(const char *nptr);
extern const char *Info_ValueForKey( const char *s, const char *key );
extern void I_strncpyz(char *dest, const char *src, int destsize);
extern char *va( const char *format, ... );
extern void Cbuf_ExecuteText(int exec_when, const char *text);
extern void Com_PumpMessageLoop(void);
extern char *MSG_ReadCommandString(msg_t *msg);
extern char *MSG_ReadStringLine(msg_t *msg);
extern void Com_DPrintf(const char *fmt, ...);
extern const char* Dvar_GetString(const char *dvarName);
extern const char *NET_AdrToString(netadr_t adr);
extern void Info_SetValueForKey(char *s, const char *key, const char *value);
extern int Cmd_Argc(void);
extern const char *Cmd_Argv( int arg );
extern void Com_Printf(const char *fmt, ...);
extern int I_stricmp(const char *s0, const char *s1);
extern int I_strnicmp(const char *s0, const char *s1, int n);
extern int Com_AddToString(const char *add, char *msg, int len, int maxlen, qboolean mayAddQuotes);
extern void CL_Netchan_SendOOBPacket(int len, const void *data, netadr_t to);
extern qboolean NET_StringToAdr(const char *s, netadr_t *a);
extern dvar_t *cl_updatefiles;
void Sys_OpenURL(const char *url, qboolean doexit);
extern unsigned char NET_OutOfBandPrint(netsrc_t sock, netadr_t adr, const char *msg);
extern int Com_sprintf(char *dest, size_t size, const char *format, ...);
extern int Sys_Milliseconds(void);
extern int sscanf(const char *str, const char *format, ...);
extern char *strchr(const char *s, int c);
extern bool Dvar_GetBool(const char *dvarName);
extern int sprintf(char *str, const char *format, ...);
extern void SND_StopSounds(snd_stopsounds_arg_t a);
extern void SV_Frame(int a);
extern void CL_Disconnect(void);
extern void Con_Close(void);
extern qboolean NET_IsLocalAddress(netadr_t adr);
extern void UI_CloseAll(void);
extern void SCR_UpdateScreen(void);
extern void Com_Error(errorParm_t level, const char *fmt, ...);
extern void Com_Memset(void *dest, const int val, int count);
extern void Dvar_SetString(dvar_t *dvar, const char *value);

typedef struct rconGlob_s {
    char password[24];
    netadr_t address;
} rconGlob_t;

COD2_ASSERT_FIELD(rconGlob_t, address, 0x18);
COD2_ASSERT_SIZE(rconGlob_t, 0x2c);

static rconGlob_t rconGlob;

static int CL_CompareAdrSigned(const void *a, const void *b);
void CL_SetServerInfo(serverInfo_t *server, const char *info, int ping);
void CL_SetServerInfoByAddress(netadr_t from, const char *info, int ping);
void CL_ServerInfoPacket(netadr_t from, msg_t *msg, int time);
qboolean CL_CDKeyValidate(const char *key, const char *checksum);
void CL_SortGlobalServers(void);
void CL_RconInit(void);
void CL_Rcon_f(void);
void CL_ServerStatusResponse(netadr_t from, msg_t *msg);
void CL_ResetPlayerMuting(int clientIndex);
void CL_MutePlayer(int clientIndex);
Bool CL_IsPlayerMuted(int clientIndex);
void CL_ClearMutedList(void);
void CL_GlobalServers_f(void);
void CL_ServersResponsePacket(netadr_t from, msg_t *msg);
void CL_Ping_f(void);
void CL_Connect_f(void);
int CL_ServerStatus(char *serverAddress, char *serverStatusString, int maxLen);
void CL_ServerStatus_f(void);

void CL_SetServerInfo(serverInfo_t *server, const char *info, int ping);
void CL_ServerStatusResponse(netadr_t from, msg_t *msg);

short BigShort(short l);

void CL_ServerStatusResponse(netadr_t from, msg_t *msg);
// names from CoD1 (SUPPORTED)
extern qboolean autoupdateChecked;

short BigShort(short l);








static int CL_CompareAdrSigned(const void *a, const void *b)
{
    serverInfo_t *serverA = (serverInfo_t *)a;
    serverInfo_t *serverB = (serverInfo_t *)b;

    return NET_CompareAdrSigned(&serverA->adr, &serverB->adr);
}

void CL_SetServerInfo(serverInfo_t *server, const char *info, int ping)
{
    if (server == NULL) {
        return;
    }

    if (info != NULL) {
        server->clients = (byte)atoi(Info_ValueForKey(info, "clients"));
        I_strncpyz(server->hostName, Info_ValueForKey(info, "hostname"), 0x20);
        I_strncpyz(server->mapName, Info_ValueForKey(info, "mapname"), 0x20);
        server->maxClients = (byte)atoi(Info_ValueForKey(info, "sv_maxclients"));
        I_strncpyz(server->game, Info_ValueForKey(info, "game"), 0x18);
        I_strncpyz(server->gameType, Info_ValueForKey(info, "gametype"), 0x10);
        server->netType = (byte)atoi(Info_ValueForKey(info, "nettype"));
        server->minPing = (short)atoi(Info_ValueForKey(info, "minping"));
        server->maxPing = (short)atoi(Info_ValueForKey(info, "maxping"));
        server->allowAnonymous = (byte)atoi(Info_ValueForKey(info, "sv_allowAnonymous"));
        server->consoleDisabled = (char)atoi(Info_ValueForKey(info, "con_disabled"));
        server->bPassword = (byte)atoi(Info_ValueForKey(info, "pswrd"));
        server->pure = (byte)atoi(Info_ValueForKey(info, "pure"));
        server->friendlyfire = (char)atoi(Info_ValueForKey(info, "ff"));
        server->killcam = (char)atoi(Info_ValueForKey(info, "kc"));
        server->hardware = (byte)atoi(Info_ValueForKey(info, "hw"));
        server->mod = (byte)atoi(Info_ValueForKey(info, "mod"));
        server->voice = (byte)atoi(Info_ValueForKey(info, "voice"));
    }

    server->ping = (short)ping;
}

void CL_ServerInfoPacket(netadr_t from, msg_t *msg, int time)
{
    int i;
    int type;
    char info[1024];
    const char *infoString;
    const char *protoStr;
    int prot;
    int expectedProt;

    infoString = MSG_ReadCommandString(msg);

    // if this isn't the correct protocol version, ignore it
    prot = atoi(Info_ValueForKey(infoString, "protocol"));
    protoStr = Dvar_GetString("debug_protocol");
    if (*protoStr)
        expectedProt = atoi(protoStr);
    else
        expectedProt = 115;
    if (prot != expectedProt) {
        Com_DPrintf("Different protocol info packet: %s\n", infoString);
        return;
    }

    // iterate servers waiting for ping response
    for (i = 0; i < 16; i++) {
        if (cl_pinglist[i].adr.port && !cl_pinglist[i].time && NET_CompareAdr(from, cl_pinglist[i].adr)) {
            // calc ping time
            cl_pinglist[i].time = time - cl_pinglist[i].start + 1;
            Com_DPrintf("ping time %dms from %s\n", cl_pinglist[i].time, NET_AdrToString(from));

            // save of info
            I_strncpyz(cl_pinglist[i].info, infoString, sizeof(cl_pinglist[i].info));

            // tack on the net type
            switch (from.type) {
            case NA_BROADCAST:
            case NA_IP:
                type = 1;
                break;
            case NA_IPX:
            case NA_BROADCAST_IPX:
                type = 2;
                break;
            default:
                type = 0;
                break;
            }
            Info_SetValueForKey(cl_pinglist[i].info, "nettype", va("%d", type));
            CL_SetServerInfoByAddress(from, infoString, cl_pinglist[i].time);

            return;
        }
    }

    // if not just sent a local broadcast or pinging local servers
    if (cls.pingUpdateSource != 0)
        return;

    for (i = 0; i < 128; i++) {
        // empty slot
        if (cls.localServers[i].adr.port == 0)
            break;

        // avoid duplicate
        if (NET_CompareAdr(from, cls.localServers[i].adr))
            return;
    }

    if (i == 128) {
        Com_DPrintf("MAX_OTHER_SERVERS hit, dropping infoResponse\n");
        return;
    }

    // add this to the list
    cls.numlocalservers = i + 1;
    cls.localServers[i].adr = from;
    cls.localServers[i].clients = 0;
    cls.localServers[i].hostName[0] = '\0';
    cls.localServers[i].mapName[0] = '\0';
    cls.localServers[i].maxClients = 0;
    cls.localServers[i].maxPing = 0;
    cls.localServers[i].minPing = 0;
    cls.localServers[i].ping = -1;
    cls.localServers[i].game[0] = '\0';
    cls.localServers[i].gameType[0] = '\0';
    cls.localServers[i].netType = from.type;
    cls.localServers[i].allowAnonymous = 0;

    I_strncpyz(info, MSG_ReadCommandString(msg), 1024);
    if (strlen(info)) {
        if (info[strlen(info) - 1] != '\n')
            strncat(info, "\n", sizeof(info));
        Com_Printf("%s: %s", NET_AdrToString(from), info);
    }
}

void CL_Connect_f(void)
{
    const char *server;
    LegacyHacks *legacy;

    if (Cmd_Argc() != 2) {
        Com_Printf("usage: connect [server]\n");
        return;
    }

    SND_StopSounds((snd_stopsounds_arg_t)0);
    clc.serverMessage[0] = 0;
    server = Cmd_Argv(1);

    if (((dvar_t *)com_sv_running)->current.enabled && !strcmp(server, "localhost"))
        ((LegacyHacks *)::legacyHacks)->sv_killserver = 1;
    legacy = (LegacyHacks *)::legacyHacks;
    legacy->cl_serverloadmap[0] = 0;
    legacy->cl_serverloadgametype[0] = 0;

    SV_Frame(0);
    CL_Disconnect();
    Con_Close();

    I_strncpyz(cls.servername, server, sizeof(cls.servername));

    if (!NET_StringToAdr(cls.servername, &clc.serverAddress)) {
        Com_Printf("Bad server address\n");
        clc.state = (connstate_t)0;
        return;
    }
    if (clc.serverAddress.port == 0)
        clc.serverAddress.port = BigShort(28960);
    Com_Printf("%s resolved to %i.%i.%i.%i:%i\n", cls.servername, clc.serverAddress.ip[0], clc.serverAddress.ip[1],
               clc.serverAddress.ip[2], clc.serverAddress.ip[3], BigShort(clc.serverAddress.port));

    netadr_t adr = clc.serverAddress;
    if (!NET_IsLocalAddress(adr) && !CL_CDKeyValidate(cl_cdkey, cl_cdkeychecksum)) {
        Com_Error(ERR_DROP, "EXE_ERR_INVALID_CD_KEY");
        return;
    }

    adr = clc.serverAddress;
    if (NET_IsLocalAddress(adr))
        clc.state = (connstate_t)4;
    else
        clc.state = (connstate_t)3;

    clients->keyCatchers = 0;
    clients->displayHUDWithKeycatchUI = 0;
    clc.connectTime = -99999;
    clc.connectPacketCount = 0;
    clc.qport = ::qport;

    UI_CloseAll();
    SCR_UpdateScreen();
}

void CL_SortGlobalServers(void)
{
    clientStatic_t *cls = (&::cls);
    int count = cls->numglobalservers;
    qsort(cls->globalServers, count, sizeof(serverInfo_t), CL_CompareAdrSigned);
}

// Retail inlines this in CL_ServersResponsePacket only.
static __forceinline void CL_InitServerInfoInl(serverInfo_t *server, netadr_t adr)
{
    server->adr = adr;
    server->clients = 0;
    server->hostName[0] = '\0';
    server->mapName[0] = '\0';
    server->maxClients = 0;
    server->maxPing = 0;
    server->minPing = 0;
    server->ping = -1;
    server->game[0] = '\0';
    server->gameType[0] = '\0';
    server->netType = 0;
    server->allowAnonymous = 0;
    server->dirty = 1;
    server->requestCount = 0;
}

static __declspec(noinline) void CL_InitServerInfo(serverInfo_t *server, netadr_t adr)
{
    CL_InitServerInfoInl(server, adr);
}

// Original name unknown.
static int CL_RefreshKnownGlobalServer(netadr_t adr)
{
    int low;
    int high;
    int mid;
    int cmp;

    low = 0;
    high = cls.numglobalservers;
    while (low < high) {
        mid = (low + high) / 2;
        cmp = NET_CompareAdrSigned(&adr, &cls.globalServers[mid].adr);
        if (cmp < 0) {
            high = mid;
        } else if (cmp > 0) {
            low = mid + 1;
        } else {
            while (--mid >= 0 && !NET_CompareAdrSigned(&adr, &cls.globalServers[mid].adr))
                ;
            mid++;
            do {
                CL_InitServerInfo(&cls.globalServers[mid], adr);
                mid++;
            } while (mid < cls.numglobalservers && !NET_CompareAdrSigned(&adr, &cls.globalServers[mid].adr));
            return 1;
        }
    }
    return 0;
}

void CL_GlobalServers_f(void)
{
    netadr_t to;
    int i;
    int count;
    char *buffptr;
    char command[1024];

    if (Cmd_Argc() < 3) {
        Com_Printf("usage: globalservers <master# 0-1> <protocol> [keywords]\n");
        return;
    }

    // age the existing entries
    for (i = 0; i < cls.numglobalservers; i++) {
        if (!++cls.globalServers[i].requestCount)
            cls.globalServers[i].requestCount = 255;
    }

    Com_Printf("Requesting servers from the master...\n");

    NET_StringToAdr("cod2master.activision.com", &to);
    cls.waitglobalserverresponse = 1;
    cls.pingUpdateSource = 1;
    to.type = NA_IP;
    to.port = BigShort(20710);

    sprintf(command, "getservers %s", Cmd_Argv(2));

    // tack on keywords
    buffptr = command + strlen(command);
    count = Cmd_Argc();
    for (i = 3; i < count; i++)
        buffptr += sprintf(buffptr, " %s", Cmd_Argv(i));

    // if we are a demo, automatically add a "demo" keyword
    if (Dvar_GetBool("fs_restrict"))
        buffptr += sprintf(buffptr, " demo");

    NET_OutOfBandPrint(NS_SERVER, to, command);
}

void CL_RconInit(void)
{
    rconGlob.password[0] = '\0';
    rconGlob.address.type = NA_BAD;
}

// helper names are ours; retail keeps them as separate functions
static void CL_RconLogin(void)
{
    const char *password;
    int len;

    if (Cmd_Argc() != 3) {
        Com_Printf("USAGE: rcon login <password>\n");
        return;
    }

    password = Cmd_Argv(2);
    len = strlen(password);
    if (len >= sizeof(rconGlob.password)) {
        Com_Printf("rcon password must be %i characters or less\n", sizeof(rconGlob.password));
        return;
    }

    memcpy(rconGlob.password, password, len + 1);
}

static void CL_RconLogout(void)
{
    if (!rconGlob.password[0]) {
        Com_Printf("Not logged in\n");
        return;
    }

    rconGlob.password[0] = 0;
}

static void CL_RconHost(void)
{
    if (Cmd_Argc() != 3) {
        Com_Printf("USAGE: rcon host <address>\n");
        return;
    }

    if (!NET_StringToAdr(Cmd_Argv(2), &rconGlob.address)) {
        Com_Printf("bad host address\n");
        return;
    }

    if (!rconGlob.address.port) {
        rconGlob.address.port = BigShort(28960);
    }
}

void CL_Rcon_f(void)
{
    char message[1024];
    int len;
    int i;
    const char *cmd;
    netadr_t to;

    if (Cmd_Argc() < 2) {
        Com_Printf("USAGE: rcon <command> <options...>\n");
        return;
    }

    cmd = Cmd_Argv(1);

    if (!I_stricmp(cmd, "login")) {
        CL_RconLogin();
        return;
    }

    if (!I_stricmp(cmd, "logout")) {
        CL_RconLogout();
        return;
    }

    if (!I_stricmp(cmd, "host")) {
        CL_RconHost();
        return;
    }

    if (!rconGlob.password[0]) {
        Com_Printf("You need to log in with 'rcon login <password>' before using rcon.\n");
        return;
    }

    message[0] = -1;
    message[1] = -1;
    message[2] = -1;
    message[3] = -1;
    message[4] = 0;

    len = Com_AddToString("rcon ", message, 4, sizeof(message), 0);
    len = Com_AddToString(rconGlob.password, message, len, sizeof(message), 0);

    for (i = 1; i < Cmd_Argc(); i++) {
        len = Com_AddToString(" ", message, len, sizeof(message), 0);
        len = Com_AddToString(Cmd_Argv(i), message, len, sizeof(message), 1);
    }

    if (len == sizeof(message)) {
        Com_Printf("rcon commands are limited to %i characters\n", sizeof(message) - 1);
        return;
    }

    message[len] = 0;

    if (clc.state >= 5) {
        to = clc.netchan.remoteAddress;
    } else if (rconGlob.address.type != NA_BAD) {
        to = rconGlob.address;
    } else {
        Com_Printf("Can't determine rcon target.  You can fix this by either:\n");
        Com_Printf("1) Joining the server as a player.\n");
        Com_Printf("2) Setting the host server with 'rcon host <address>'.\n");
        return;
    }

    CL_Netchan_SendOOBPacket(strlen(message) + 1, message, to);
}

ping_t *CL_GetFreePing(void)
{
    ping_t *pingptr;
    ping_t *best;
    int oldest;
    int i;
    int time;
    int now;

    now = Sys_Milliseconds();

    pingptr = cl_pinglist;
    for (i = 0; i < 16; i++, pingptr++) {
        // find free ping slot
        if (pingptr->adr.port) {
            if (!pingptr->time) {
                if (now - pingptr->start < 500) {
                    // still waiting for response
                    continue;
                }
            } else if (pingptr->time < 500) {
                // results have not been queried
                continue;
            }
        }

        // clear it
        pingptr->adr.port = 0;
        return pingptr;
    }

    // use oldest entry
    pingptr = cl_pinglist;
    best = cl_pinglist;
    oldest = 0x80000000;
    for (i = 0; i < 16; i++, pingptr++) {
        // scan for oldest
        time = now - pingptr->start;
        if (time > oldest) {
            oldest = time;
            best = pingptr;
        }
    }

    return best;
}
void CL_Ping_f(void)
{
    netadr_t to;
    ping_t *pingptr;
    const char *server;

    if (Cmd_Argc() != 2) {
        Com_Printf("usage: ping [server]\n");
        return;
    }

    memset(&to, 0, sizeof(netadr_t));

    server = Cmd_Argv(1);

    if (!NET_StringToAdr(server, &to))
        return;

    pingptr = CL_GetFreePing();

    memcpy(&pingptr->adr, &to, sizeof(netadr_t));
    pingptr->start = Sys_Milliseconds();
    pingptr->time = 0;

    CL_SetServerInfoByAddress(pingptr->adr, NULL, 0);

    NET_OutOfBandPrint(NS_CLIENT1, to, "getinfo xxx");
}

__declspec(noinline) serverStatusRequest_t *CL_GetServerStatus(netadr_t from)
{
    int i;
    int oldest;
    int oldestTime;

    for (i = 0; i < 16; i++) {
        if (NET_CompareAdr(from, cl_serverStatusList[i].address))
            return &cl_serverStatusList[i];
    }
    for (i = 0; i < 16; i++) {
        if (cl_serverStatusList[i].retrieved)
            return &cl_serverStatusList[i];
    }
    oldest = -1;
    oldestTime = 0;
    for (i = 0; i < 16; i++) {
        if (oldest == -1 || cl_serverStatusList[i].startTime < oldestTime) {
            oldest = i;
            oldestTime = cl_serverStatusList[i].startTime;
        }
    }
    if (oldest != -1)
        return &cl_serverStatusList[oldest];
    serverStatusCount++;
    return &cl_serverStatusList[serverStatusCount & 15];
}

int CL_ServerStatus(char *serverAddress, char *serverStatusString, int maxLen)
{
    int i;
    netadr_t to;
    serverStatusRequest_t *serverStatus;

    if (!serverAddress) {
        for (i = 0; i < 16; i++) {
            cl_serverStatusList[i].address.port = 0;
            cl_serverStatusList[i].retrieved = 1;
        }
        return 0;
    }
    if (!NET_StringToAdr(serverAddress, &to))
        return 0;
    serverStatus = CL_GetServerStatus(to);
    if (!serverStatusString) {
        serverStatus->retrieved = 1;
        return 0;
    }
    if (NET_CompareAdr(to, serverStatus->address)) {
        if (!serverStatus->pending) {
            I_strncpyz(serverStatusString, serverStatus->string, maxLen);
            serverStatus->retrieved = 1;
            serverStatus->startTime = 0;
            return 1;
        }
        else if (serverStatus->startTime < Sys_Milliseconds() - cl_serverStatusResendTime->current.integer) {
            serverStatus->print = 0;
            serverStatus->pending = 1;
            serverStatus->retrieved = 0;
            serverStatus->time = 0;
            serverStatus->startTime = Sys_Milliseconds();
            NET_OutOfBandPrint(NS_CLIENT1, to, "getstatus");
            return 0;
        }
    }
    else if (serverStatus->retrieved) {
        serverStatus->address = to;
        serverStatus->print = 0;
        serverStatus->pending = 1;
        serverStatus->retrieved = 0;
        serverStatus->startTime = Sys_Milliseconds();
        serverStatus->time = 0;
        NET_OutOfBandPrint(NS_CLIENT1, to, "getstatus");
        return 0;
    }
    return 0;
}

void CL_ServerStatus_f(void)
{
    netadr_t to;
    const char *serverAddr;
    serverStatusRequest_t *serverStatus;

    Com_Memset(&to, 0, sizeof(netadr_t));

    if (Cmd_Argc() != 2) {
        if (clc.state != 8 || clc.demoplaying != 0) {
            Com_Printf((const char *)"Not connected to a server.\n");
            Com_Printf((const char *)"Usage: serverstatus [server]\n");
            return;
        }
        serverAddr = cls.servername;
    } else {
        serverAddr = Cmd_Argv(1);
    }

    if (!NET_StringToAdr(serverAddr, &to))
        return;

    NET_OutOfBandPrint(NS_CLIENT1, to, (const char *)"getstatus");

    serverStatus = CL_GetServerStatus(to);
    serverStatus->address = to;
    serverStatus->print = 1;
    serverStatus->pending = 1;
}

void CL_ServerStatusResponse(netadr_t from, msg_t *msg)
{
    int i;
    serverStatusRequest_t *serverStatus = NULL;
    const char *line;
    char info[1024];
    int infoField;

    for (i = 0; i < 16; i++) {
        if (NET_CompareAdr(from, cl_serverStatusList[i].address)) {
            serverStatus = &cl_serverStatusList[i];
            goto found_status;
        }
    }
    return;

found_status:
    if (!serverStatus)
        return;

    line = MSG_ReadStringLine(msg);
    Com_sprintf(serverStatus->string, 0x2000, (const char *)"%s", line);

    if (serverStatus->print) {
        Com_Printf((const char *)"Server settings:\n");

        while (*line) {
            for (infoField = 0; infoField < 2 && *line; infoField++) {
                int j;
                if (*line == '\\')
                    line++;
                j = 0;
                while (*line) {
                    info[j++] = *line;
                    if (j >= 0x3ff)
                        break;
                    line++;
                    if (*line == '\\')
                        break;
                }
                info[j] = '\0';

                if (infoField)
                    Com_Printf((const char *)"%s\n", info);
                else
                    Com_Printf((const char *)"%-24s", info);
            }
        }
    }

    {
        int statusLen = strlen(serverStatus->string);
        Com_sprintf(serverStatus->string + statusLen, 0x2000 - statusLen, (const char *)"\\");
    }

    if (serverStatus->print) {
        Com_Printf((const char *)"\nPlayers:\n");
        Com_Printf((const char *)"num: score: ping: name:\n");
    }

    {
        int playerIdx = 0;
        line = MSG_ReadStringLine(msg);
        while (*line) {
            {
                int statusLen = strlen(serverStatus->string);
                Com_sprintf(serverStatus->string + statusLen, 0x2000 - statusLen, (const char *)"\\%s", line);
            }

            if (serverStatus->print) {
                int ping = 0, score = 0;
                const char *name;
                sscanf(line, (const char *)"%d %d", &score, &ping);
                name = strchr(line, ' ');
                if (name) {
                    name = strchr(name + 1, ' ');
                    if (name)
                        name++;
                    else
                        name = (const char *)"unknown";
                } else {
                    name = (const char *)"unknown";
                }
                Com_Printf((const char *)"%-2d   %-3d    %-3d   %s\n", playerIdx, score, ping, name);
            }

            line = MSG_ReadStringLine(msg);
            playerIdx++;
        }
    }

    {
        int statusLen = strlen(serverStatus->string);
        Com_sprintf(serverStatus->string + statusLen, 0x2000 - statusLen, (const char *)"\\");
    }

    serverStatus->time = Sys_Milliseconds();
    serverStatus->address = from;
    serverStatus->pending = 0;

    if (serverStatus->print) {
        serverStatus->retrieved = 1;
    }
}

void CL_CheckAutoUpdate(void)
{
    int validServerNum = 0;
    int i, rnd;
    netadr_t temp;
    char *servername[5];

    if (autoupdateChecked)
        return;

    for (i = 0; i < 5; i++) {
        if (NET_StringToAdr(cls.autoupdateServerNames[i], &temp)) {
            servername[validServerNum] = cls.autoupdateServerNames[i];
            validServerNum++;
        }
    }

    if (!validServerNum) {
        Com_DPrintf("Couldn't resolve an AutoUpdate Server address.\n");
    } else {
        rnd = rand() % validServerNum;

        Com_DPrintf("Resolving AutoUpdate Server... ");
        if (!NET_StringToAdr(servername[rnd], &cls.autoupdateServer)) {
            Com_DPrintf("\nCouldn't resolve first address, trying others... ");

            for (i = 1; i < validServerNum; i++) {
                if (NET_StringToAdr(servername[(i + rnd) % validServerNum], &cls.autoupdateServer)) {
                    Com_DPrintf("\nAlternate server address resolved... ");
                    break;
                }
            }

            if (i == validServerNum) {
                Com_DPrintf("\nFailed to resolve any Auto-update servers.\n");
                autoupdateChecked = 1;
                return;
            }
        }

        cls.autoupdateServer.port = BigShort(28960);
        Com_DPrintf("%i.%i.%i.%i:%i\n", cls.autoupdateServer.ip[0], cls.autoupdateServer.ip[1],
                    cls.autoupdateServer.ip[2], cls.autoupdateServer.ip[3],
                    BigShort(cls.autoupdateServer.port));

        NET_OutOfBandPrint(NS_CLIENT1, cls.autoupdateServer,
                           va("getUpdateInfo2 \"%s\" \"%s\" \"%s\"\n", "CoD2 MP", "1.0", "win-x86"));
    }

    autoupdateChecked = 1;
}

void CL_GetAutoUpdate(void)
{
    if (!autoupdateChecked)
        return;

    if (!strlen(cl_updatefiles->current.string))
        return;

    Sys_OpenURL(cl_updatefiles->current.string, 1);
}

void CL_ResetPlayerMuting(int clientIndex)
{
    s_playerMute[clientIndex] = 0;
}

void CL_MutePlayer(int clientIndex)
{
    s_playerMute[clientIndex] = !s_playerMute[clientIndex];
    if (s_playerMute[clientIndex]) {
        Cbuf_ExecuteText(2, va("muteplayer %i", clientIndex));
    } else {
        Cbuf_ExecuteText(2, va("unmuteplayer %i", clientIndex));
    }
}

Bool CL_IsPlayerMuted(int clientIndex)
{
    return s_playerMute[clientIndex];
}

void CL_ClearMutedList(void)
{
    memset(s_playerMute, 0, sizeof(s_playerMute));
}

// retail calls NET_CompareAdr at the favorites loop only
static __declspec(noinline) qboolean NET_CompareAdrNI(netadr_t a, netadr_t b)
{
    return NET_CompareAdr(a, b);
}
void CL_SetServerInfoByAddress(netadr_t from, const char *info, int ping)
{
    int i;
    int low;
    int high;
    int compare;

    Com_PumpMessageLoop();

    for (i = 0; i < 128; i++) {
        if (NET_CompareAdr(from, cls.localServers[i].adr))
            CL_SetServerInfo(&cls.localServers[i], info, ping);
    }

    // global servers are sorted by address
    low = 0;
    high = cls.numglobalservers;
    while (low < high) {
        i = (high + low) / 2;
        compare = NET_CompareAdrSigned(&from, &cls.globalServers[i].adr);
        if (compare < 0) {
            high = i;
        } else if (compare > 0) {
            low = i + 1;
        } else {
            // back up to the first matching entry
            do {
                i--;
            } while (i >= 0 && !NET_CompareAdrSigned(&from, &cls.globalServers[i].adr));
            i++;
            do {
                CL_SetServerInfo(&cls.globalServers[i], info, ping);
                i++;
            } while (i < cls.numglobalservers && !NET_CompareAdrSigned(&from, &cls.globalServers[i].adr));
            break;
        }
    }

    for (i = 0; i < 128; i++) {
        if (NET_CompareAdrNI(from, cls.favoriteServers[i].adr))
            CL_SetServerInfo(&cls.favoriteServers[i], info, ping);
    }
}
qboolean CL_CDKeyValidate(const char *key, const char *checksum)
{
    unsigned int crc = 0;
    char chs[8];
    int i;

    for (i = 0; i < 16; i++) {
        int ch = (signed char)key[i];
        int j;
        crc ^= (unsigned int)ch;
        for (j = 8; j != 0; j--) {
            if (crc & 1) {
                crc >>= 1;
                crc ^= 0xa001;
            } else {
                crc >>= 1;
            }
        }
    }

    sprintf(chs, (const char *)"%04x", crc);

    if (checksum == NULL || I_strnicmp(chs, checksum, 4) == 0)
        return 1;

    return 0;
}
void CL_ServersResponsePacket(netadr_t from, msg_t *msg)
{
    int i, count, numservers;
    serverAddress_t addresses[256];
    byte *buffptr;
    byte *buffend;
    netadr_t adr;
    serverInfo_t *server;

    Com_PumpMessageLoop();
    Com_Printf("CL_ServersResponsePacket\n");

    cls.waitglobalserverresponse = 0;

    numservers = 0;
    buffptr = msg->data;
    buffend = buffptr + msg->cursize;
    while (buffptr + 1 < buffend) {
        do {
            if (*buffptr++ == '\\')
                break;
        } while (buffptr < buffend);

        if (buffptr >= buffend - 6)
            break;

        addresses[numservers].ip[0] = *buffptr++;
        addresses[numservers].ip[1] = *buffptr++;
        addresses[numservers].ip[2] = *buffptr++;
        addresses[numservers].ip[3] = *buffptr++;

        addresses[numservers].port = (*buffptr++) << 8;
        addresses[numservers].port += *buffptr++;
        addresses[numservers].port = BigShort(addresses[numservers].port);

        if (*buffptr != '\\')
            break;

        Com_DPrintf("server: %d ip: %d.%d.%d.%d:%d\n", numservers,
                    addresses[numservers].ip[0],
                    addresses[numservers].ip[1],
                    addresses[numservers].ip[2],
                    addresses[numservers].ip[3],
                    addresses[numservers].port);

        numservers++;
        if (numservers >= 256)
            break;

        if (buffptr[1] == 'E' && buffptr[2] == 'O' && buffptr[3] == 'T')
            break;
    }

    count = cls.numglobalservers;
    for (i = 0; i < numservers && count < MAX_GLOBALSERVERS; i++) {
        adr.type = NA_IP;
        adr.ip[0] = addresses[i].ip[0];
        adr.ip[1] = addresses[i].ip[1];
        adr.ip[2] = addresses[i].ip[2];
        adr.ip[3] = addresses[i].ip[3];
        adr.port = addresses[i].port;

        if (!CL_RefreshKnownGlobalServer(adr)) {
            server = &cls.globalServers[count];
            count++;
            CL_InitServerInfoInl(server, adr);
        }
    }

    cls.numglobalservers = count;
    CL_SortGlobalServers();

    Com_Printf("%d servers parsed (total %d)\n", numservers, count);
}