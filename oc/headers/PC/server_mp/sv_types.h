#ifndef CLEAN_PC_SERVER_MP_SV_TYPES_H
#define CLEAN_PC_SERVER_MP_SV_TYPES_H

#include "../../cod2_fwd.h"
#include "../../cod2_defs.h"

typedef struct cachedSnapshot_t cachedSnapshot_t;
typedef struct cachedClient_s cachedClient_s;
typedef struct snapshotEntityNumbers_t snapshotEntityNumbers_t;
typedef struct serverStatic_t serverStatic_t;
typedef struct ucmd_t ucmd_t;

struct cachedSnapshot_t {
    int archivedFrame;
    int time;
    int num_entities;
    int first_entity;
    int num_clients;
    int first_client;
    int usesDelta;
};

struct cachedClient_s {
    qboolean playerStateExists;
    clientState_t cs;
    playerState_t ps;
};

struct snapshotEntityNumbers_t {
    int numSnapshotEntities;
    int snapshotEntities[1024];
};

struct serverStatic_t {
    qboolean initialized;
    int time;
    int snapFlagServerBit;
    client_t *clients;
    int numSnapshotEntities;
    int numSnapshotClients;
    int nextSnapshotEntities;
    int nextSnapshotClients;
    entityState_t *snapshotEntities;
    char * (*snapshotClients)();
    qboolean archiveEnabled;
    int nextArchivedSnapshotFrames;
    char * (*archivedSnapshotFrames)();
    byte *archivedSnapshotBuffer;
    int nextArchivedSnapshotBuffer;
    int nextCachedSnapshotEntities;
    int nextCachedSnapshotClients;
    int nextCachedSnapshotFrames;
    fileHandle_t (*cachedSnapshotEntities)();
    client_t * (*cachedSnapshotClients)();
    cachedSnapshot_t *cachedSnapshotFrames;
    int nextHeartbeatTime;
    int nextStatusResponseTime;
    challenge_t challenges[1024];
    netadr_t redirectAddress;
    netadr_t authorizeAddress;
    netProfileInfo_t *pOOBProf;
    tempBanSlot_t tempBans[16];
};

COD2_ASSERT_FIELD(serverStatic_t, clients, 0xc);
COD2_ASSERT_FIELD(serverStatic_t, challenges, 0x5c);
#if COD2_IS_PATCH_13
COD2_ASSERT_FIELD(serverStatic_t, redirectAddress, 0x1d05c);
COD2_ASSERT_FIELD(serverStatic_t, authorizeAddress, 0x1d070);
COD2_ASSERT_FIELD(serverStatic_t, pOOBProf, 0x1d084);
COD2_ASSERT_FIELD(serverStatic_t, tempBans, 0x1d088);
COD2_ASSERT_SIZE(serverStatic_t, 0x1d108);
#else
COD2_ASSERT_FIELD(serverStatic_t, redirectAddress, 0xc05c);
COD2_ASSERT_FIELD(serverStatic_t, authorizeAddress, 0xc070);
COD2_ASSERT_FIELD(serverStatic_t, pOOBProf, 0xc084);
COD2_ASSERT_FIELD(serverStatic_t, tempBans, 0xc088);
COD2_ASSERT_SIZE(serverStatic_t, 0xc108);
#endif

struct ucmd_t {
    char *name;
    void (*func)();
};
#endif
