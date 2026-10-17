#include "common_types.h"
#include "imports.h"
#include "headers/PC/cgame_mp/cg_local.h"
#include "bytematch.h"

extern const dvar_t *cg_nopredict;
extern const dvar_t *cg_synchronousClients;

extern void BG_EvaluateTrajectory( const trajectory_t *tr, int atTime, vec3_t result );
extern void BG_PlayerStateToEntityState(playerState_s *ps, entityState_s *s, int snap, byte handler);
extern void CG_ResetPlayerEntity(centity_t *cent);
extern void XAnimCloneAnimTree(const struct XAnimTree_s *srcTree, struct XAnimTree_s *destTree);
extern int I_stricmp(const char *s1, const char *s2);
extern void I_strncpyz(char *dest, const char *src, int destsize);
extern void CL_ResetSkeletonCache(int val);
extern void CG_SetFrameInterpolation(void);
extern void CG_ExecuteNewServerCommands(int serverCommandSequence);
extern void CG_CheckOpenWaitingScriptMenu(void);
extern void CG_Respawn(void);
extern void CG_BuildSolidList(void);
extern void CG_ClearSolidList(void);
extern void CG_PlaySmokeGrenadesAtTime(int time);
extern void CG_SetEquippedOffHand(int offHandIndex);
extern void CG_TransitionPlayerState(playerState_t *ps, playerState_t *ops);
extern void CG_CheckEvents(centity_t *cent);
extern void CG_UpdatePlayerDObj(centity_t *cent);
extern void CG_UpdateHandViewmodels(const char *configStr);
extern void CG_AddLagometerSnapshotInfo(snapshot_t *snap);
extern int CL_GetSnapshot(int snapshotNumber, snapshot_t *snap);
extern void CL_GetCurrentSnapshotNumber(int *snapshotNumber, int *serverTime);
extern const char *CL_GetConfigString(int index);
extern void CG_GameMessage(const char *msg);
extern void CG_BoldGameMessage(const char *msg);
extern void CG_SafeDObjFree(int entNum);
extern void CG_InitView(void);
extern void SND_SetListener(int entnum, const vec_t *origin, vec3_t *axis);
extern void SND_FadeAllSounds(float volume, int duration);
extern void AnglesToAxis( const vec3_t angles, vec3_t axis[3] );
extern char *va( const char *format, ... );
extern const char *UI_SafeTranslateString(const char *ref);
extern void Com_Error(errorParm_t level, const char *fmt, ...);
extern void Com_Printf(const char *fmt, ...);
extern int strcmp(const char *s1, const char *s2);
extern XAnim *XAnimGetAnims(const XAnimTree *tree);
extern bool XAnimIsLooped(const XAnim_s *anim, unsigned int animIndex);
extern int XAnimGetNumChildren(const struct XAnim_s *anims, unsigned int animIndex);
extern void XAnimSetTime(XAnimTree *tree, unsigned int animIndex, float time);
extern void XAnimSetTimeInl(XAnimTree *tree, unsigned int animIndex, float time);

#define CENT_STRIDE 0x224

#define CENT_CS_NUMBER 0x000
#define CENT_CS_ETYPE 0x004
#define CENT_CS_EFLAGS 0x008
#define CENT_CS_POS 0x00c
#define CENT_CS_APOS 0x030
#define CENT_CS_CLIENTNUM 0x090
#define CENT_CS_EVENTSEQUENCE 0x0a4
#define CENT_CS_LEANF 0x0d4
#define CENT_CS_DMGFLAGS 0x0d8

#define CENT_NS_NUMBER 0x0f0
#define CENT_NS_ETYPE 0x0f4
#define CENT_NS_EFLAGS 0x0f8
#define CENT_NS_POS 0x0fc
#define CENT_NS_APOS 0x120
#define CENT_NS_TIME 0x144
#define CENT_NS_TIME2 0x148
#define CENT_NS_INDEX 0x17c
#define CENT_NS_CLIENTNUM 0x180
#define CENT_NS_EVENTSEQUENCE 0x194
#define CENT_NS_LEANF 0x1c4

#define CENT_NEXTVALID 0x1e0
#define CENT_CULLIN 0x1e1
#define CENT_BMUZZLEFLASH 0x1e2
#define CENT_BTRAILMADE 0x1e3
#define CENT_PREVIOUSEVENTSEQ 0x1e4
#define CENT_MISCTIME 0x1e8
#define CENT_LERPORIGIN 0x1ec
#define CENT_LERPANGLES 0x1f8
#define CENT_LIGHTINGORIGIN 0x204
#define CENT_TREE 0x21c

#define SNAP_NUMCLIENTS 0x26b8
#define SNAP_CLIENTS 0x116b0

#define CI_STRIDE 0x4b8
#define CI_INFOVALID 0x000
#define CI_NEXTVALID 0x004
#define CI_CLIENTNUM 0x008
#define CI_NAME 0x00c
#define CI_TEAM 0x02c
#define CI_MODEL 0x040
#define CI_ATTACHMODELNAMES 0x080
#define CI_ATTACHTAGNAMES 0x200
#define CI_DOBJDIRTY 0x3f8
#define CI_PXANIMTREE 0x4a4

#define CLSTATE_STRIDE 0x5c

#define ES_BINSIZE 0x0f0

#define VectorCopy3(src, dst)                  \
    do {                                       \
        ((int *)(dst))[0] = ((int *)(src))[0]; \
        ((int *)(dst))[1] = ((int *)(src))[1]; \
        ((int *)(dst))[2] = ((int *)(src))[2]; \
    } while (0)

#define VectorClear3(v)      \
    do {                     \
        ((int *)(v))[0] = 0; \
        ((int *)(v))[1] = 0; \
        ((int *)(v))[2] = 0; \
    } while (0)

static void CG_ResetEntity(char *centPtr);
void CG_SetNextSnap(snapshot_t *snap);
void CG_SetInitialSnapshot(snapshot_t *snap_param);
void CG_ProcessSnapshots(void);

static void CG_ResetEntity(char *cent);
void CG_SetNextSnap(snapshot_t *snap);
void CG_SetInitialSnapshot(snapshot_t *snap);
void CG_ProcessSnapshots(void);

static inline void VectorClear(vec3_t v)
{
    v[0] = 0;
    v[1] = 0;
    v[2] = 0;
}

static inline void VectorCopy(const vec3_t a, vec3_t b)
{
    b[0] = a[0];
    b[1] = a[1];
    b[2] = a[2];
}

static char *CG_EntityPtr(int entNum)
{
    return (char *)&cg_entities[entNum];
}

static char *CG_ClientInfoPtr(char *cg, int clientNum)
{
    return (char *)&((cg_t *)cg)->bgs.clientinfo[clientNum];
}

// Name from CoD4; original CoD2 name unknown.
static void CG_CopyCorpseInfo(clientInfo_t *corpseInfo, const clientInfo_t *ci)
{
    int attachIndex;

    memcpy(corpseInfo, ci, sizeof(clientInfo_t));
    for (attachIndex = 0; attachIndex < 6; attachIndex++) {
        if (!I_stricmp(corpseInfo->attachTagNames[attachIndex], "J_Spine4")) {
            corpseInfo->attachModelNames[attachIndex][0] = 0;
            corpseInfo->attachTagNames[attachIndex][0] = 0;
        }
    }
}

static void CG_ResetEntity(char *centPtr)
{
    centity_t *cent = (centity_t *)centPtr;
    clientInfo_t *ci;
    clientInfo_t *corpseInfo;
    XAnimTree_s *pXAnimTree;

    VectorClear(cent->lightingOrigin);
    cent->currentState = cent->nextState;
    cent->bTrailMade = 0;
    cent->cullIn = 0;
    BG_EvaluateTrajectory(&cent->nextState.pos, cg->time, cent->lerpOrigin);
    BG_EvaluateTrajectory(&cent->nextState.apos, cg->time, cent->lerpAngles);

    switch (cent->nextState.eType) {
    case 0:
    case 4:
        cent->previousEventSequence = 0;
        break;

    case 1:
        cent->previousEventSequence = cent->nextState.eventSequence;
        ci = &cg->bgs.clientinfo[cent->nextState.clientNum];
        ci->lerpMoveDir = cent->nextState.angles2[1];
        ci->lerpLean = cent->nextState.leanf;
        VectorCopy(cent->lerpAngles, ci->playerAngles);
        cent->lerpAngles[0] = 0;
        cent->lerpAngles[2] = 0;
        CG_ResetPlayerEntity(cent);
        break;

    case 2:
        ci = &cg->bgs.clientinfo[cent->nextState.clientNum];
        corpseInfo = &cgs->corpseinfo[cent->nextState.number - 64];
        pXAnimTree = corpseInfo->pXAnimTree;
        if (cent->nextState.eFlags & 0x80000) {
            CG_CopyCorpseInfo(corpseInfo, ci);
            corpseInfo->pXAnimTree = pXAnimTree;
            XAnimCloneAnimTree(ci->pXAnimTree, pXAnimTree);
            cent->previousEventSequence = 0;
        } else {
            if (!corpseInfo->model[0] || corpseInfo->clientNum != ci->clientNum) {
                CG_CopyCorpseInfo(corpseInfo, ci);
                corpseInfo->pXAnimTree = pXAnimTree;
            }
            cent->previousEventSequence = cent->nextState.eventSequence;
        }
        corpseInfo->dobjDirty = 1;
        break;

    default:
        cent->previousEventSequence = cent->nextState.eventSequence;
        break;
    }
}

static void CG_ClearClientInfos_Inline(char *dest, char *src, char *tagBase, int size)
{
    int attachIndex;
    char *tagSrc = tagBase;
    char *dst = dest;

    memcpy(dest, src, size);

    for (attachIndex = 0; attachIndex < 6; attachIndex++) {
        if (I_stricmp(tagSrc, (const char *)"J_Spine4") == 0) {
            *(char *)(dst + 0x80)  = 0;
            *(char *)(dst + 0x200)  = 0;
        }
        tagSrc += 0x40;
        dst += 0x40;
    }
}

void CG_SetInitialSnapshot(snapshot_t *snap_param)
{
    char *snap = (char *)snap_param;
    float clientViewOrigin[3];
    float clientViewAxis[9];

    CG_SetNextSnap(NULL);

    cg->snap = (snapshot_t *)snap;
    cg->nextSnap = (snapshot_t *)snap;

    {
        int serverTime = ((snapshot_t *)snap)->serverTime;
        cg->time = serverTime;
        cg->bgs.time = serverTime;
        cg->oldTime = serverTime;
    }

    clientViewOrigin[0] = ((snapshot_t *)snap)->ps.origin[0];
    clientViewOrigin[1] = ((snapshot_t *)snap)->ps.origin[1];
    clientViewOrigin[2] = ((snapshot_t *)snap)->ps.origin[2];
    clientViewOrigin[2] += ((snapshot_t *)snap)->ps.viewHeightCurrent;

    AnglesToAxis(((snapshot_t *)snap)->ps.viewangles, (vec3_t *)clientViewAxis);

    {
        int clientNum = ((snapshot_t *)snap)->ps.clientNum;
        SND_SetListener(clientNum, clientViewOrigin, (vec3_t (*))(clientViewAxis));
    }

    SND_FadeAllSounds(1.0f, 0);

    CG_Respawn();

    CG_PlaySmokeGrenadesAtTime(cg->time);

    CG_InitView();

    cg->nextSnap = NULL;
}

// Original name unknown.
static void CG_UpdateKillCam(void)
{
    centity_t *cent;
    clientInfo_t *ci;
    int corpseIndex;
    XAnimTree *pXAnimTree;
    unsigned int anim;
    XAnim_s *anims;
    int i;

    if (!cg->inKillCam && cg->nextSnap->ps.deltaTime) {
        cg->inKillCam = 1;
        CG_SetEquippedOffHand(0);
        CG_PlaySmokeGrenadesAtTime(cg->time);
    }
    if (cg->inKillCam && !cg->nextSnap->ps.deltaTime) {
        cg->inKillCam = 0;
        CG_PlaySmokeGrenadesAtTime(cg->time);
        for (i = 0; i < cg->nextSnap->numEntities; ++i) {
            cent = &cg_entities[cg->nextSnap->entities[i].number];
            if (cent->nextState.eType == 2) {
                corpseIndex = cent->nextState.number - 64;
                ci = &cgs->corpseinfo[corpseIndex];
                pXAnimTree = ci->pXAnimTree;
                anim = ci->legs.animationNumber & 0xFFFFFDFF;
                anims = XAnimGetAnims(pXAnimTree);
                if (anim && !XAnimIsLooped(anims, anim) && !XAnimGetNumChildren(anims, anim))
                    XAnimSetTimeInl(pXAnimTree, anim, 1.0f);
            }
        }
    }
}

static void CG_TransitionSnapshot(void)
{
    centity_t *cent;
    int i;

    for (i = 0; i < cg->snap->numClients; i++) {
        clientState_t *cs = &cg->snap->clients[i];
        clientInfo_t *ci = &cg->bgs.clientinfo[cs->clientIndex];

        if (ci->nextValid) {
            ci->nextValid = 0;
        } else {
            XAnimTree_s *tree = ci->pXAnimTree;

            memset(ci, 0, sizeof(*ci));
            ci->pXAnimTree = tree;
            CG_SafeDObjFree(cs->clientIndex);
        }
    }

    cg->snap = cg->nextSnap;

    if (cg->snap->ps.pm_flags & 0xc00000) {
        cent = &cg_entities[cg->snap->ps.clientNum];
        memcpy(&cent->currentState, &cent->nextState, sizeof(cent->currentState));
    }

    for (i = 0; i < cg->snap->numEntities; i++) {
        cent = &cg_entities[cg->snap->entities[i].number];
        memcpy(&cent->currentState, &cent->nextState, sizeof(cent->currentState));
    }
}

void CG_SetNextSnap(snapshot_t *snap)
{
    char centInPrevSnapshot[1024];
    entityState_t *es;
    clientState_t *clientState;
    clientInfo_t *ci;
    centity_t *cent;
    const char *modelName;
    int entnum;
    int num;
    int i;

    memset(centInPrevSnapshot, 0, sizeof(centInPrevSnapshot));
    if (cg->nextSnap) {
        for (num = 0; num < cg->nextSnap->numEntities; num++) {
            entnum = cg->nextSnap->entities[num].number;
            cg_entities[entnum].nextValid = 0;
            centInPrevSnapshot[entnum] = 1;
        }
        entnum = cg->nextSnap->ps.clientNum;
        if (cg_entities[entnum].nextValid) {
            cg_entities[entnum].nextValid = 0;
            centInPrevSnapshot[entnum] = 1;
        }
    }

    cg->nextSnap = snap;
    if (!snap) {
        CG_ClearSolidList();
        return;
    }

    CL_ResetSkeletonCache(0);

    CG_SetFrameInterpolation();
    CG_ExecuteNewServerCommands(snap->serverCommandSequence);
    CG_CheckOpenWaitingScriptMenu();

    for (num = 0; num < snap->numClients; num++) {
        clientState = &snap->clients[num];
        ci = &cg->bgs.clientinfo[clientState->clientIndex];

        ci->oldteam = ci->infoValid ? ci->team : clientState->team;
        ci->infoValid = 1;
        ci->nextValid = 1;
        ci->clientNum = clientState->clientIndex;
        ci->team = clientState->team;

        if (strcmp(ci->name, clientState->name)) {
            if (ci->name[0])
                CG_GameMessage(va("%s^7 %s %s", ci->name, UI_SafeTranslateString("CGAME_PLAYERRENAMES"), clientState->name));
            I_strncpyz(ci->name, clientState->name, sizeof(ci->name));
        }

        modelName = CL_GetConfigString(clientState->modelindex + 0x14e);
        if (strcmp(ci->model, modelName)) {
            I_strncpyz(ci->model, modelName, sizeof(ci->model));
            ci->dobjDirty = 1;
        }

        for (i = 0; i < 6; i++) {
            modelName = CL_GetConfigString(clientState->attachModelIndex[i] + 0x14e);
            if (strcmp(ci->attachModelNames[i], modelName)) {
                I_strncpyz(ci->attachModelNames[i], modelName, sizeof(ci->attachModelNames[i]));
                ci->dobjDirty = 1;
            }

            modelName = CL_GetConfigString(clientState->attachTagIndex[i] + 0x6e);
            if (strcmp(ci->attachTagNames[i], modelName)) {
                I_strncpyz(ci->attachTagNames[i], modelName, sizeof(ci->attachTagNames[i]));
                ci->dobjDirty = 1;
            }
        }
    }

    cg->identifyClientNum = snap->ps.stats[3];
    cg->identifyClientHealth = snap->ps.stats[4];

    entnum = snap->ps.clientNum;
    if (snap->ps.pm_flags & 0xc00000) {
        cent = &cg_entities[entnum];
        cent->nextState.number = (unsigned short)entnum;
        BG_PlayerStateToEntityState(&snap->ps, &cent->nextState, 0, 0);
        cent->nextValid = 1;
        if (!cg->mapRestart && snap->ps.stats[5] == cg->snap->ps.stats[5] && entnum == cg->snap->ps.clientNum) {
            if (!centInPrevSnapshot[entnum] || ((cent->nextState.eFlags ^ cent->currentState.eFlags) & 2)) {
                memcpy(&cg->snap->ps, &snap->ps, sizeof(cg->snap->ps));
                CG_ResetEntity((char *)cent);
                cg->predictedError[0] = 0.0f;
                cg->predictedError[1] = 0.0f;
                cg->predictedError[2] = 0.0f;
            }
        } else {
            memcpy(&cg->snap->ps, &snap->ps, sizeof(cg->snap->ps));
            CG_ResetEntity((char *)cent);
            CG_Respawn();
        }
    } else if (cg->mapRestart || snap->ps.stats[5] != cg->snap->ps.stats[5] || entnum != cg->snap->ps.clientNum) {
        memcpy(&cg->snap->ps, &snap->ps, sizeof(cg->snap->ps));
        CG_Respawn();
    }

    for (num = 0; num < snap->numEntities; num++) {
        es = &snap->entities[num];
        entnum = es->number;
        cent = &cg_entities[entnum];
        memcpy(&cent->nextState, es, sizeof(cent->nextState));
        cent->nextValid = 1;
        if (!centInPrevSnapshot[entnum] || ((es->eFlags ^ cent->currentState.eFlags) & 2))
            CG_ResetEntity((char *)cent);
    }

    for (num = 0; num < snap->numClients; num++)
        CG_UpdatePlayerDObj(&cg_entities[snap->clients[num].clientIndex]);

    if (snap->ps.viewmodelIndex > 0)
        CG_UpdateHandViewmodels(CL_GetConfigString(snap->ps.viewmodelIndex + 0x14e));

    CG_BuildSolidList();
    CG_UpdateKillCam();

    for (num = 0; num < snap->numEntities; num++)
        CG_CheckEvents(&cg_entities[snap->entities[num].number]);

    if (cg->demoType || (cg->nextSnap->ps.pm_flags & 0x400000) || cg_nopredict->current.enabled || cg_synchronousClients->current.enabled)
        CG_TransitionPlayerState(&cg->nextSnap->ps, &cg->snap->ps);
}

static snapshot_t *CG_ReadNextSnapshot(void)
{
    snapshot_t *dest;

    if (cg->latestSnapshotNum > cgs->processedSnapshotNum + 1000) {
        Com_Printf("WARNING: CG_ReadNextSnapshot: way out of range, %i > %i\n",
                   cg->latestSnapshotNum, cgs->processedSnapshotNum);
    }

    while (cgs->processedSnapshotNum < cg->latestSnapshotNum) {
        if (cg->snap == &cg->activeSnapshots[0])
            dest = &cg->activeSnapshots[1];
        else
            dest = &cg->activeSnapshots[0];

        cgs->processedSnapshotNum++;
        if (CL_GetSnapshot(cgs->processedSnapshotNum, dest)) {
            CG_AddLagometerSnapshotInfo(dest);
            return dest;
        }

        CG_AddLagometerSnapshotInfo(NULL);
    }

    return NULL;
}

void CG_ProcessSnapshots(void)
{
    snapshot_t *snap;
    int n;

    CL_GetCurrentSnapshotNumber(&n, &cg->latestSnapshotTime);
    if (n != cg->latestSnapshotNum) {
        if (n < cg->latestSnapshotNum) {
            Com_Error(ERR_DROP, (const char *)"\x15"
                                       "CG_ProcessSnapshots: n < cg->latestSnapshotNum");
        }
        cg->latestSnapshotNum = n;
    }

    cg->bgs.latestSnapshotTime = cg->latestSnapshotTime;

    while (!cg->snap) {
        snap = CG_ReadNextSnapshot();
        if (!snap)
            return;

        if (!(snap->snapFlags & 2)) {
            CG_SetInitialSnapshot(snap);
            CG_SetNextSnap(snap);
            CG_TransitionSnapshot();
        }
    }

    CG_SetFrameInterpolation();

    for (;;) {
        if (cg->nextSnap == cg->snap || cg->cubemapShot) {
            snap = CG_ReadNextSnapshot();
            if (!snap)
                break;

            if ((snap->snapFlags ^ cg->nextSnap->snapFlags) & 4) {
                CG_SetInitialSnapshot(snap);
                CG_SetNextSnap(snap);
                CG_TransitionSnapshot();
                continue;
            }

            if (snap->serverTime - cg->nextSnap->serverTime < 0) {
                Com_Error(ERR_DROP, (const char *)"\x15"
                                           "CG_ProcessSnapshots: Server time went backwards");
            }

            CG_SetNextSnap(snap);
        }

        if (cg->time - cg->snap->serverTime >= 0 && cg->time - cg->nextSnap->serverTime < 0)
            break;

        CG_TransitionSnapshot();
    }

    if (cg->time - cg->snap->serverTime < 0) {
        cg->bgs.time = cg->time = cg->snap->serverTime;
    }
}

