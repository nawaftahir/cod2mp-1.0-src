#include "common_types.h"
#include "imports.h"
#include "bytematch.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
extern clientActive_t clients[1];
extern clientConnection_t clientConnections[1];
#define clc clientConnections[0]
extern clientStatic_t cls;

extern int I_stricmp(const char *s1, const char *s2);

extern qboolean CL_UpdateDirtyPings_f(int source);
extern int CL_ServerStatus(char *serverAddress, char *serverStatus, int maxLen);
extern int SND_PlayLocalSoundAlias(snd_alias_list_t *aliasList, snd_alias_system_t channel);
extern int SND_PlayLocalSoundAliasByName(const char *aliasname, snd_alias_system_t channel);
extern void Com_LoadSoundAliases(const char *zone, const char *spec, snd_alias_system_t flags);
extern qboolean UI_CheckExecKey(int key);
extern void CL_SwitchToLocalClient(int localClientNum);
extern void UI_Init(void);
extern void UI_Component_Init(void);
extern qboolean UI_IsFullscreen(void);
extern qboolean UI_SetActiveMenu(int menu);
extern char *Key_KeynumToString(int keynum, int translate);
extern char *Key_GetBinding(int keynum);
extern void I_strncpyz(char *dest, const char *src, int destsize);
extern dvar_t *Dvar_RegisterString(const char *dvarName, const char *value, unsigned short flags);
extern void Com_WriteCDKey(void);
extern void Com_PumpMessageLoop(void);
extern void Info_SetValueForKey(char *s, const char *key, const char *value);
extern char *va( const char *format, ... );
extern const char *NET_AdrToString(netadr_t adr);
extern void Com_UnloadSoundAliases(snd_alias_system_t system);
extern void UI_Shutdown(void);

void GetClientState(uiClientState_t *state);
void LAN_ResetPings(int source);
int LAN_GetServerCount(int source);
qboolean LAN_WaitServerResponse(int source);
void LAN_GetServerInfo(int source, int n, char *buf, int buflen);
int LAN_GetServerPing(int source, int n);
void LAN_MarkServerDirty(int source, int n, qboolean dirty);
int LAN_ServerIsDirty(int source, int n);
qboolean LAN_UpdateDirtyPings(int source);
int LAN_GetServerStatus(char *serverAddress, char *serverStatus, int maxLen);
void Key_KeynumToStringBuf(int keynum, char *buf, int buflen);
void Key_GetBindingBuf(int keynum, char *buf, int buflen);
int Key_GetCatcher(void);
void Key_SetCatcher(int catcher);
void CLUI_GetCDKey(char *buf, int buflen, char *buf2, int buf2len);
void CLUI_SetCDKey(char *buf, char *buf2);
int GetClientname(int index, char *buf, int size);
int UI_PlayLocalSoundAlias(snd_alias_list_t *aliasList);
int UI_PlayLocalSoundAliasByName(const char *aliasname);
qboolean UI_ClientIsInGame(void);
qboolean CL_ShutdownUI(void);
void CL_InitUI(void);
qboolean UI_checkKeyExec(int key);
void UI_LoadSoundAliases(void);
int LAN_CompareHostname(const char *hostName1, const char *hostName2);
int LAN_CompareServers(int source, int sortKey, int sortDir, int s1, int s2);

void GetClientState(uiClientState_t *state)
{
    clientConnection_t *clc_local = &::clc;

    state->connectPacketCount = clc_local->connectPacketCount;
    state->connState = clc_local->state;
    I_strncpyz(state->servername, cls.servername, 0x400);
    I_strncpyz(state->updateInfoString, cls.updateInfoString, 0x400);
    I_strncpyz(state->messageString, clc_local->serverMessage, 0x400);
    state->clientNum = (clients)->snap.ps.clientNum;
}

void LAN_ResetPings(int source)
{
    serverInfo_t *servers;
    int count;

    switch (source) {
    case 0:
        count = 128;
        servers = cls.localServers;
        break;
    case 1:
        count = cls.numglobalservers;
        servers = cls.globalServers;
        break;
    case 2:
        count = 128;
        servers = cls.favoriteServers;
        break;
    default:
        return;
    }

    for (int i = 0; i < count; i++) {
        servers[i].ping = -1;
    }
}

int LAN_GetServerCount(int source)
{
    switch (source) {
    case 0:
        return cls.numlocalservers;
    case 1:
        return cls.numglobalservers;
    case 2:
        return cls.numfavoriteservers;
    }
    return 0;
}

qboolean LAN_WaitServerResponse(int source)
{
    switch (source) {
    case 1:
        return cls.waitglobalserverresponse;
    }
    return 0;
}

void LAN_GetServerInfo(int source, int n, char *buf, int buflen)
{
    char info[1024];
    serverInfo_t *server = NULL;

    info[0] = '\0';

    switch (source) {
    case 0:
        if (n < 0 || n >= 128)
            goto fail;
        server = &cls.localServers[n];
        break;
    case 1:
        if (n < 0 || n >= cls.numglobalservers)
            goto fail;
        server = &cls.globalServers[n];
        break;
    case 2:
        if (n < 0 || n >= 128)
            goto fail;
        server = &cls.favoriteServers[n];
        break;
    default:
        goto fail;
    }

    if (!server)
        goto fail;
    if (!buf)
        return;

    *buf = '\0';
    Info_SetValueForKey(info, "hostname", server->hostName);
    Info_SetValueForKey(info, "mapname", server->mapName);
    Info_SetValueForKey(info, "clients", va("%i", (int)server->clients));
    Info_SetValueForKey(info, "sv_maxclients", va("%i", (int)server->maxClients));
    Info_SetValueForKey(info, "ping", va("%i", (int)server->ping));
    Info_SetValueForKey(info, "minping", va("%i", (int)server->minPing));
    Info_SetValueForKey(info, "maxping", va("%i", (int)server->maxPing));
    Info_SetValueForKey(info, "game", server->game);
    Info_SetValueForKey(info, "gametype", server->gameType);
    Info_SetValueForKey(info, "nettype", va("%i", (int)server->netType));
    {
        netadr_t adr;
        memcpy(&adr, &server->adr, sizeof(netadr_t));
        Info_SetValueForKey(info, "addr", NET_AdrToString(adr));
    }
    Info_SetValueForKey(info, "sv_allowAnonymous", va("%i", (int)server->allowAnonymous));
    Info_SetValueForKey(info, "con_disabled", va("%i", (int)server->consoleDisabled));
    Info_SetValueForKey(info, "pswrd", va("%i", (int)server->bPassword));
    Info_SetValueForKey(info, "pure", va("%i", (int)server->pure));
    Info_SetValueForKey(info, "ff", va("%i", (int)server->friendlyfire));
    Info_SetValueForKey(info, "kc", va("%i", (int)server->killcam));
    Info_SetValueForKey(info, "hw", va("%i", (int)server->hardware));
    Info_SetValueForKey(info, "mod", va("%i", (int)server->mod));
    Info_SetValueForKey(info, "voice", va("%i", (int)server->voice));
    I_strncpyz(buf, info, buflen);
    return;

fail:
    if (buf)
        *buf = '\0';
}

int LAN_GetServerPing(int source, int n)
{
    serverInfo_t *server;

    switch (source) {
    case 0:
        if (n < 0 || n >= 128)
            return -1;
        server = &cls.localServers[n];
        break;
    case 1:
        if (n < 0 || n >= cls.numglobalservers)
            return -1;
        server = &cls.globalServers[n];
        break;
    case 2:
        if (n < 0 || n >= 128)
            return -1;
        server = &cls.favoriteServers[n];
        break;
    default:
        return -1;
    }

    if (!server)
        return -1;
    return server->ping;
}

static serverInfo_t *LAN_GetServerPtr(int source, int n)
{
    switch (source) {
    case 0:
        if (n >= 0 && n < 128)
            return &cls.localServers[n];
        break;
    case 1:
        if (n >= 0 && n < cls.numglobalservers)
            return &cls.globalServers[n];
        break;
    case 2:
        if (n >= 0 && n < 128)
            return &cls.favoriteServers[n];
        break;
    }
    return NULL;
}

static void LAN_CleanHostname(const char *hostName, char *cleanHostName)
{
    unsigned char c;

    while (1) {
        c = *hostName;
        if (!*hostName)
            break;
        if (isalpha(c))
            *cleanHostName++ = c;
        hostName++;
    }
    *cleanHostName = 0;
}

int LAN_CompareHostname(const char *hostName1, const char *hostName2)
{
    char cleanHostName1[32];
    char cleanHostName2[32];
    int res;

    LAN_CleanHostname(hostName1, cleanHostName1);
    LAN_CleanHostname(hostName2, cleanHostName2);
    res = I_stricmp(cleanHostName1, cleanHostName2);
    if (res)
        return res;
    return I_stricmp(hostName1, hostName2);
}

int LAN_CompareServers(int source, int sortKey, int sortDir, int s1, int s2)
{
    serverInfo_t *server1;
    serverInfo_t *server2;
    int diff;

    server1 = LAN_GetServerPtr(source, s1);
    server2 = LAN_GetServerPtr(source, s2);

    if (!server1 || !server2)
        return 0;

    diff = 0;
    switch (sortKey) {
    case 0:
        diff = server1->bPassword - server2->bPassword;
        if (diff)
            goto apply_sortdir;
        break;
    case 1:
        diff = server1->hardware - server2->hardware;
        if (!diff)
            break;
        if (diff < 0) {
            if (!server1->hardware)
                diff = 1;
        } else {
            if (!server2->hardware)
                diff = -1;
        }
        goto apply_sortdir;
    case 8:
        diff = server1->mod - server2->mod;
        if (diff)
            goto apply_sortdir;
        break;
    case 6:
        diff = server1->voice - server2->voice;
        if (diff)
            goto apply_sortdir;
        break;
    case 7:
        diff = server1->pure - server2->pure;
        if (diff)
            goto apply_sortdir;
        break;
    case 2:
        diff = LAN_CompareHostname(server1->hostName, server2->hostName);
        if (diff)
            goto apply_sortdir;
        break;
    case 3:
        diff = I_stricmp(server1->mapName, server2->mapName);
        if (diff)
            goto apply_sortdir;
        break;
    case 4:
        diff = server1->clients - server2->clients;
        if (diff)
            goto apply_sortdir;
        break;
    case 5:
        diff = I_stricmp(server1->gameType, server2->gameType);
        if (diff)
            goto apply_sortdir;
        break;
    case 9:
        goto compare_ping;
    default:
        goto apply_sortdir;
    }

    sortDir = 0;
compare_ping:
    diff = server1->ping - server2->ping;
    if (!diff) {
        diff = I_stricmp(server1->gameType, server2->gameType);
        if (!diff)
            diff = LAN_CompareHostname(server1->hostName, server2->hostName);
    }

apply_sortdir:
    if (sortDir)
        diff = -diff;
    return diff;
}

void LAN_MarkServerDirty(int source, int n, qboolean dirty)
{
    serverInfo_t *servers;
    int count;
    int i;

    Com_PumpMessageLoop();

    if (n == -1) {
        count = 128;
        switch (source) {
        case 0:
            servers = cls.localServers;
            break;
        case 1:
            servers = cls.globalServers;
            count = cls.numglobalservers;
            break;
        case 2:
            servers = cls.favoriteServers;
            break;
        default:
            return;
        }
        for (i = 0; i < count; i++) {
            servers[i].dirty = (byte)dirty;
        }
    } else {
        switch (source) {
        case 0:
            if (n < 0 || n >= 128)
                return;
            cls.localServers[n].dirty = (byte)dirty;
            break;
        case 1:
            if (n < 0 || n >= cls.numglobalservers)
                return;
            cls.globalServers[n].dirty = (byte)dirty;
            break;
        case 2:
            if (n < 0 || n >= 128)
                return;
            cls.favoriteServers[n].dirty = (byte)dirty;
            break;
        }
    }
}

int LAN_ServerIsDirty(int source, int n)
{
    switch (source) {
    case 0:
        if (n < 0 || n >= 128)
            return 0;
        return cls.localServers[n].dirty;
    case 1:
        if (n < 0 || n >= cls.numglobalservers)
            return 0;
        return cls.globalServers[n].dirty;
    case 2:
        if (n < 0 || n >= 128)
            return 0;
        return cls.favoriteServers[n].dirty;
    }
    return 0;
}

qboolean LAN_UpdateDirtyPings(int source)
{
    return CL_UpdateDirtyPings_f(source);
}

int LAN_GetServerStatus(char *serverAddress, char *serverStatus, int maxLen)
{
    return CL_ServerStatus(serverAddress, serverStatus, maxLen);
}

void Key_KeynumToStringBuf(int keynum, char *buf, int buflen)
{
    I_strncpyz(buf, Key_KeynumToString(keynum, 1), buflen);
}

void Key_GetBindingBuf(int keynum, char *buf, int buflen)
{
    const char *binding = Key_GetBinding(keynum);
    if (binding) {
        I_strncpyz(buf, binding, buflen);
        return;
    }
    *buf = 0;
}

int Key_GetCatcher(void)
{
    return (clients)->keyCatchers;
}

void Key_SetCatcher(int catcher)
{
    clientActive_t *ptr = clients;
    if (ptr->keyCatchers & 1)
        ptr->keyCatchers = catcher | 1;
    else
        ptr->keyCatchers = catcher;
    ptr = clients;
    if (!(ptr->keyCatchers & 8))
        ptr->displayHUDWithKeycatchUI = 0;
}

void CLUI_GetCDKey(char *buf, int buflen, char *buf2, int buf2len)
{
    char *cdkey;
    char *cdkey2;

    Dvar_RegisterString((const char *)"fs_game", (const char *)"", 0x101c);

    cdkey = cl_cdkey;
    memcpy(buf, cdkey, 16);
    buf[16] = '\0';

    cdkey2 = cl_cdkeychecksum;
    memcpy(buf2, cdkey2, 4);
    buf2[4] = '\0';
}

void CLUI_SetCDKey(char *buf, char *buf2)
{
    char *cdkey = cl_cdkey;
    char *cdkey2;

    memcpy(cdkey, buf, 16);
    cdkey[16] = '\0';

    cdkey2 = cl_cdkeychecksum;
    memcpy(cdkey2, buf2, 4);
    cdkey2[4] = '\0';

    Com_WriteCDKey();
}

int GetClientname(int index, char *buf, int size)
{
    clientActive_t *cl;
    int count;
    int i;

    *buf = '\0';

    cl = clients;
    if (!cl->snap.valid)
        return 0;

    count = *(int *)((char *)cl + 0x26e0);

    for (i = 0; i < count; i++) {
        int slot = (*(int *)((char *)cl + 0x26e8) + i) & 0x7ff;
        clientState_t *entry = &cl->parseClients[slot];

        if (entry->clientIndex == index) {
            strncpy(buf, entry->name, size);
            return 1;
        }
    }

    return 0;
}

int UI_PlayLocalSoundAlias(snd_alias_list_t *aliasList)
{
    return SND_PlayLocalSoundAlias(aliasList, (snd_alias_system_t)0);
}

int UI_PlayLocalSoundAliasByName(const char *aliasname)
{
    return SND_PlayLocalSoundAliasByName(aliasname, (snd_alias_system_t)0);
}

qboolean UI_ClientIsInGame(void)
{
    clientConnection_t *clc_local = &::clc;
    return clc_local->state == 8;
}

qboolean CL_ShutdownUI(void)
{
    clientStatic_t *cls_local = &cls;
    clientActive_t *cl;
    int started = cls_local->uiStarted;

    if (!started)
        return 0;

    Com_UnloadSoundAliases(SASYS_UI);

    cl = clients;
    cl->keyCatchers &= ~8;
    cl->displayHUDWithKeycatchUI = 0;

    CL_SwitchToLocalClient(0);
    UI_Shutdown();
    CL_SwitchToLocalClient(0);

    cls_local->uiStarted = 0;
    return 1;
}

void CL_InitUI(void)
{
#if COD2_TRACE
    extern void Com_Printf(const char *, ...);
    extern int g_disableRendering;
#endif
    cls.uiStarted = 1;
    CL_SwitchToLocalClient(0);
#if COD2_TRACE
    Com_Printf("[disrend] CL_InitUI before UI_Init = %d\n", g_disableRendering);
#endif
    UI_Init();
#if COD2_TRACE
    Com_Printf("[disrend] CL_InitUI after UI_Init = %d\n", g_disableRendering);
#endif
    CL_SwitchToLocalClient(0);
    if (cls.uiStarted)
        UI_Component_Init();
}

qboolean UI_checkKeyExec(int key)
{
    if (cls.uiStarted)
        return UI_CheckExecKey(key);
    return 0;
}

void UI_LoadSoundAliases(void)
{
    Com_LoadSoundAliases("menu", "all_mp", (snd_alias_system_t)(0));
}

