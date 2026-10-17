#include "common_types.h"
#include "imports.h"

extern qboolean NET_StringToAdr(const char *s, netadr_t *a);
extern qboolean NET_CompareAdr(netadr_t a, netadr_t b);
extern const char *NET_AdrToString(netadr_t a);
extern int NET_CompareAdrSigned(netadr_t *a, netadr_t *b);
extern void I_strncpyz(char *dest, const char *src, int destsize);
extern "C" void Com_Memcpy( void *dest, const void *src, int count );
extern int FS_SV_FOpenFileRead(const char *filename, fileHandle_t *fp);
extern fileHandle_t FS_SV_FOpenFileWrite(const char *filename);
extern int FS_Read(void *buffer, int len, fileHandle_t h);
extern int FS_Write(const void *buffer, int len, fileHandle_t h);
extern void FS_FCloseFile(fileHandle_t h);
extern void CL_SortGlobalServers(void);

extern clientStatic_t cls;

#define CLS ((byte *)&cls)

#define MAX_FAVORITESERVERS 128
#define MAX_OTHER_SERVERS 128
#define MAX_GLOBALSERVERS 20000

int LAN_AddServer(int source, const char *name, const char *address);
void LAN_RemoveServer(int source, const char *addr);
void LAN_GetServerAddressString(int source, int n, char *buf, int buflen);
void LAN_LoadCachedServers(void);
void LAN_SaveServersToCache(void);

int LAN_AddServer(int source, const char *name, const char *address)
{
    int max, *count, i;
    netadr_t adr;
    serverInfo_t *servers = NULL;

    max = MAX_OTHER_SERVERS;
    count = 0;
    switch (source) {
    case 2:
        count = &cls.numfavoriteservers;
        servers = &cls.favoriteServers[0];
        break;
    }
    if (servers && *count < max) {
        if (!NET_StringToAdr(address, &adr))
            return -2;
        for (i = 0; i < *count; i++) {
            if (NET_CompareAdr(servers[i].adr, adr))
                break;
        }
        if (i >= *count) {
            servers[*count].adr = adr;
            I_strncpyz(servers[*count].hostName, name, sizeof(servers[*count].hostName));
            servers[*count].dirty = 1;
            (*count)++;
            return 1;
        }
        return 0;
    }
    return -1;
}

void LAN_RemoveServer(int source, const char *addr)
{
    int *count, i;
    serverInfo_t *servers = NULL;

    count = 0;
    switch (source) {
    case 2:
        count = &cls.numfavoriteservers;
        servers = &cls.favoriteServers[0];
        break;
    }
    if (servers) {
        netadr_t comp;
        NET_StringToAdr(addr, &comp);
        for (i = 0; i < *count; i++) {
            if (NET_CompareAdr(comp, servers[i].adr)) {
                int j = i;
                while (j < *count - 1) {
                    Com_Memcpy(&servers[j], &servers[j + 1], sizeof(servers[j]));
                    j++;
                }
                (*count)--;
                break;
            }
        }
    }
}

void LAN_GetServerAddressString(int source, int n, char *buf, int buflen)
{
    switch (source) {
    case 0:
        if (n >= 0 && n < MAX_OTHER_SERVERS) {
            I_strncpyz(buf, NET_AdrToString(cls.localServers[n].adr), buflen);
            return;
        }
        break;
    case 1:
        if (n >= 0 && n < cls.numglobalservers) {
            I_strncpyz(buf, NET_AdrToString(cls.globalServers[n].adr), buflen);
            return;
        }
        break;
    case 2:
        if (n >= 0 && n < MAX_OTHER_SERVERS) {
            I_strncpyz(buf, NET_AdrToString(cls.favoriteServers[n].adr), buflen);
            return;
        }
        break;
    }
    buf[0] = 0;
}

// Original name unknown.
static qboolean LAN_ReadServerCache(fileHandle_t fileIn)
{
    int version;
    int size;

    if (FS_Read(&version, sizeof(version), fileIn) != sizeof(version))
        return 0;
    if (version != 1)
        return 0;
    if (FS_Read(&cls.numglobalservers, sizeof(int), fileIn) != sizeof(int)
        || (unsigned int)cls.numglobalservers >= MAX_GLOBALSERVERS)
        return 0;
    if (FS_Read(&cls.numfavoriteservers, sizeof(int), fileIn) != sizeof(int)
        || (unsigned int)cls.numfavoriteservers >= MAX_FAVORITESERVERS)
        return 0;
    if (FS_Read(&size, sizeof(size), fileIn) != sizeof(size)
        || size != sizeof(cls.globalServers) + sizeof(cls.favoriteServers))
        return 0;
    if (FS_Read(cls.globalServers, sizeof(cls.globalServers), fileIn) != sizeof(cls.globalServers))
        return 0;
    return FS_Read(cls.favoriteServers, sizeof(cls.favoriteServers), fileIn) == sizeof(cls.favoriteServers);
}

void LAN_LoadCachedServers(void)
{
    fileHandle_t fileIn;
    qboolean success;

    if (FS_SV_FOpenFileRead("servercache.dat", &fileIn)) {
        success = LAN_ReadServerCache(fileIn);
        FS_FCloseFile(fileIn);
        if (success) {
            CL_SortGlobalServers();
            return;
        }
    }
    cls.numglobalservers = 0;
    cls.numfavoriteservers = 0;
}

void LAN_SaveServersToCache(void)
{
    int i;
    int version;
    int size;
    fileHandle_t fileOut;

    fileOut = FS_SV_FOpenFileWrite("servercache.dat");
    if (!fileOut)
        return;

    version = 1;
    FS_Write(&version, sizeof(version), fileOut);

    for (i = cls.numglobalservers - 1; i >= 0; i--) {
        if (cls.globalServers[i].requestCount >= 3
            || (i && !NET_CompareAdrSigned(&cls.globalServers[i].adr, &cls.globalServers[i - 1].adr))) {
            cls.numglobalservers--;
            memcpy(&cls.globalServers[i], &cls.globalServers[cls.numglobalservers], sizeof(serverInfo_t));
        }
    }
    CL_SortGlobalServers();

    FS_Write(&cls.numglobalservers, sizeof(int), fileOut);
    FS_Write(&cls.numfavoriteservers, sizeof(int), fileOut);
    size = sizeof(cls.globalServers) + sizeof(cls.favoriteServers);
    FS_Write(&size, sizeof(size), fileOut);
    FS_Write(cls.globalServers, sizeof(cls.globalServers), fileOut);
    FS_Write(cls.favoriteServers, sizeof(cls.favoriteServers), fileOut);
    FS_FCloseFile(fileOut);
}

