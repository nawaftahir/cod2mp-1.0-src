#include "common_types.h"
#include "imports.h"
#include "bytematch.h"
#include "headers/PC/cgame_mp/cg_local.h"

extern float sinf(float x);
extern float cosf(float x);
extern float sqrtf(float x);

static pmove_t cg_pmove[1];
static int cg_numSolidEntities;
static centity_t *cg_solidEntities[256];
static int cg_numTriggerEntities;
static centity_t *cg_triggerEntities[256];

extern int CM_PointContents(const vec_t *point, unsigned int model);
extern int CM_TransformedPointContents(const vec_t *point, unsigned int model, const vec_t *origin, const vec_t *angles);
extern int CM_ContentsOfModel( clipHandle_t handle );
extern void CM_BoxTrace(trace_t *results, const float *start, const float *end, const float *mins, const float *maxs, clipHandle_t model, int brushmask);
extern clipHandle_t CM_TempBoxModel(const vec_t *mins, const vec_t *maxs, int contents);
extern int CM_TransformedBoxTraceExternal(trace_t *results, const vec_t *start, const vec_t *end, const vec_t *mins, const vec_t *maxs, clipHandle_t model, int brushmask, const vec_t *origin, const vec_t *angles);
extern float CM_RadiusOfModel(unsigned int model);
extern int CL_GetCurrentCmdNumber(void);
extern qboolean CL_GetUserCmd(int cmdNumber, usercmd_t *ucmd);
extern void PM_UpdateViewAngles(playerState_t *ps, float msec, usercmd_t *cmd, unsigned char handler);
extern float LerpAngle(float from, float to, float frac);
extern void Pmove(pmove_t *pm);
extern void CG_TransitionPlayerState(playerState_t *ps, playerState_t *ops);
extern void BG_PlayerStateToEntityState(playerState_s *ps, entityState_s *s, int snap, byte handler);
extern void BG_EvaluateTrajectory( const trajectory_t *tr, int atTime, vec3_t result );
extern WeaponDef *BG_GetWeaponDef(int weapon);
extern bool CG_GetWeapReticleZoom(float *fZoom);
extern float flrand(float min, float max);
extern float randomf(void);
extern float AngleNormalize360( float angle );
extern qboolean BG_PlayerTouchesItem(const playerState_s *ps, const entityState_s *item, int atTime);
extern qboolean BG_CanItemBeGrabbed(const entityState_t *ent, const playerState_t *ps, qboolean bTouched);
extern void BG_AddPredictableEventToPlayerstate(int newEvent, int eventParm, playerState_t *ps);
extern void CG_AdjustPositionForMover(const vec_t *in, int moverNum, int fromTime, int toTime, vec_t *out, vec_t *outDeltaAngles);
extern void Com_Printf(const char *fmt, ...);

extern const dvar_t *cg_nopredict;
extern const dvar_t *cg_synchronousClients;
extern const dvar_t *cg_showmiss;
extern const dvar_t *cg_predictItems;
extern const dvar_t *cg_errorDecay;

#define REAL_PS_SIZE 0x26a8

void CG_ClearSolidList(void);
int CG_PointContents(const vec_t *point, int passEntityNum, int contentmask);
static void CG_InterpolatePlayerState(qboolean grabAngles);
void CG_PredictPlayerState(void);
static void Trace_CalcBounds(const vec_t *start, const vec_t *mins, const vec_t *maxs, float fraction, const vec_t *end, vec3_t *bounds);
void CG_ClipMoveToEntities(const vec_t *start, const vec_t *mins, const vec_t *maxs, const vec_t *end, int skipNumber, int mask, int capsule, trace_t *tr);
void CG_BuildSolidList(void);

void CG_ClearSolidList(void)
{
    cg_numSolidEntities = 0;
    cg_numTriggerEntities = 0;
}

#if 0  // engine source
int CG_PointContents(const vec_t *point, int passEntityNum, int contentmask)
{
    int contents;
    int i;
    centity_t **pp;

    contents = CM_PointContents(point, 0);

    pp = (centity_t **)&cg_solidEntities;
    for (i = 0; i < cg_numSolidEntities; i++, pp++) {
        centity_t *cent = *pp;
        entityState_t *ent = &cent->nextState;

        if (ent->number == passEntityNum)
            continue;
        if (ent->solid != 0xffffff)
            continue;
        if (ent->index.brushmodel == 0)
            continue;

        contents |= CM_TransformedPointContents(point, ent->index.brushmodel,
                                                cent->lerpOrigin,
                                                cent->lerpAngles);
    }

    return contents & contentmask;
}
#endif

static void CG_InterpolatePlayerState(qboolean grabAngles)
{
    playerState_t *out;
    snapshot_t *prev;
    snapshot_t *next;
    usercmd_t cmd;
    float f;
    int i;

    out = &cg->predictedPlayerState;
    prev = cg->snap;
    next = cg->nextSnap;

    memcpy(out, &next->ps, REAL_PS_SIZE);

    if (grabAngles) {
        int cmdNum = CL_GetCurrentCmdNumber();
        CL_GetUserCmd(cmdNum, &cmd);
        PM_UpdateViewAngles(out, 0.0f, &cmd, 0);
    }

    if (next->serverTime <= prev->serverTime)
        return;

    f = cg->frameInterpolation;

    i = next->ps.bobCycle;
    if (i < prev->ps.bobCycle)
        i += 256;
    out->bobCycle = prev->ps.bobCycle + (int)((i - prev->ps.bobCycle) * cg->frameInterpolation);

    out->aimSpreadScale = prev->ps.aimSpreadScale +
                          (next->ps.aimSpreadScale - prev->ps.aimSpreadScale) * cg->frameInterpolation;

    for (i = 0; i < 3; i++) {
        out->origin[i] = prev->ps.origin[i] + (next->ps.origin[i] - prev->ps.origin[i]) * f;
        if (!grabAngles)
            out->viewangles[i] = LerpAngle(prev->ps.viewangles[i], next->ps.viewangles[i], f);
        out->velocity[i] = prev->ps.velocity[i] + (next->ps.velocity[i] - prev->ps.velocity[i]) * f;
    }

    out->viewHeightCurrent = prev->ps.viewHeightCurrent +
                             (next->ps.viewHeightCurrent - prev->ps.viewHeightCurrent) * f;
    out->leanf = prev->ps.leanf + (next->ps.leanf - prev->ps.leanf) * f;
    out->fWeaponPosFrac = prev->ps.fWeaponPosFrac +
                          (next->ps.fWeaponPosFrac - prev->ps.fWeaponPosFrac) * f;
}

static inline void FastSinCos(const float value, float *pSin, float *pCos)
{
    __asm
    {
        push eax
        push ecx
        fld value
        fsincos
        mov ecx, pCos
        mov eax, pSin
        fstp dword ptr [ecx]
        fstp dword ptr [eax]
        pop ecx
        pop eax
    }
}

static inline bool VectorCompare(const vec3_t v1, const vec3_t v2)
{
    return v1[0] == v2[0] && v1[1] == v2[1] && v1[2] == v2[2];
}

static inline vec_t VectorLength(const vec3_t v)
{
    return sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

static void CG_TouchItem(centity_t *cent)
{
    if (!cg_predictItems->current.enabled)
        return;
    if (!BG_PlayerTouchesItem(&cg->predictedPlayerState, &cent->currentState, cg->time))
        return;
    if (cent->miscTime == cg->time)
        return;
    if (!BG_CanItemBeGrabbed(&cent->currentState, &cg->predictedPlayerState, 1))
        return;

    cent->nextState.eFlags |= 0x20;
    cent->miscTime = cg->time;
    BG_AddPredictableEventToPlayerstate(0x90, cent->nextState.index.item, &cg->predictedPlayerState);
}

static void CG_TouchTriggerPrediction(void)
{
    int i;
    int spectator;
    centity_t *cent;
    entityState_t *ent;
    unsigned int cmodel;
    trace_t trace;

    if (cg->predictedPlayerState.pm_type >= 6)
        return;

    spectator = cg->predictedPlayerState.pm_type == 4;
    if (cg->predictedPlayerState.pm_type != 0 && cg->predictedPlayerState.pm_type != 1 && !spectator)
        return;

    for (i = 0; i < cg_numTriggerEntities; i++) {
        cent = cg_triggerEntities[i];
        ent = &cent->nextState;

        if (ent->eType == 3 && !spectator) {
            CG_TouchItem(cent);
            continue;
        }

        if (ent->solid != 0xffffff)
            continue;

        cmodel = ent->index.brushmodel;
        if (!cmodel)
            continue;

        CM_BoxTrace(&trace, cg->predictedPlayerState.origin, cg->predictedPlayerState.origin,
                    cg_pmove[0].mins, cg_pmove[0].maxs, cmodel, -1);
    }
}

// Name from CoD4; original CoD2 name unknown.
static void CG_PredictPlayerState_Internal(void)
{
    playerState_t *oldPlayerState;
    int cmdNum, current;
    usercmd_t oldestCmd;
    usercmd_t latestCmd;
    qboolean moved;
    vec3_t adjusted;
    vec3_t deltaAngles;
    vec3_t delta;
    float len;
    float f;
    LargeLocal oldPlayerStateBuf(REAL_PS_SIZE);

    oldPlayerState = (playerState_t *)oldPlayerStateBuf.GetBuf();

    if (cg->demoType || (cg->nextSnap->ps.pm_flags & 0x400000)) {
        CG_InterpolatePlayerState(0);
        return;
    }

    if (cg_nopredict->current.enabled || cg_synchronousClients->current.enabled) {
        CG_InterpolatePlayerState(1);
        return;
    }

    cg_pmove[0].ps = &cg->predictedPlayerState;
    cg_pmove[0].handler = 0;
    if (cg->predictedPlayerState.pm_type >= 6)
        cg_pmove[0].tracemask = 0x810011;
    else
        cg_pmove[0].tracemask = 0x2810011;
    if (cg->nextSnap->ps.pm_type == 4)
        cg_pmove[0].tracemask &= ~0x2010000;

    current = CL_GetCurrentCmdNumber();
    cmdNum = current - 127;
    if (!CL_GetUserCmd(cmdNum, &oldestCmd)) {
        if (cg_showmiss->current.integer)
            Com_Printf("exceeded PACKET_BACKUP on commands\n");
        return;
    }

    memcpy(oldPlayerState, &cg->predictedPlayerState, REAL_PS_SIZE);
    CL_GetUserCmd(current, &latestCmd);

    memcpy(&cg->predictedPlayerState, &cg->nextSnap->ps, REAL_PS_SIZE);
    cg->physicsTime = cg->nextSnap->serverTime;
    cg->predictedPlayerState.oldVelocity[0] = oldPlayerState->oldVelocity[0];
    cg->predictedPlayerState.oldVelocity[1] = oldPlayerState->oldVelocity[1];

    moved = 0;
    if (cg->nextSnap->ps.commandTime < latestCmd.serverTime) {
        for (; cmdNum <= current; cmdNum++) {
            CL_GetUserCmd(cmdNum, &cg_pmove[0].cmd);

            if (cg_pmove[0].cmd.serverTime <= cg->predictedPlayerState.commandTime)
                continue;
            if (cg_pmove[0].cmd.serverTime > latestCmd.serverTime)
                continue;
            if (!CL_GetUserCmd(cmdNum - 1, &cg_pmove[0].oldcmd))
                continue;

            if (cg->predictedPlayerState.commandTime == oldPlayerState->commandTime) {
                CG_AdjustPositionForMover(cg->predictedPlayerState.origin, cg->predictedPlayerState.groundEntityNum,
                                          cg->physicsTime, cg->oldTime, adjusted, deltaAngles);
                cg->predictedPlayerState.delta_angles[1] += (int)(deltaAngles[1] * (65536.0f / 360.0f)) & 0xffff;

                if (cg_showmiss->current.integer) {
                    if (!VectorCompare(oldPlayerState->origin, adjusted))
                        Com_Printf("prediction error\n");
                }

                delta[0] = oldPlayerState->origin[0] - adjusted[0];
                delta[1] = oldPlayerState->origin[1] - adjusted[1];
                delta[2] = oldPlayerState->origin[2] - adjusted[2];
                len = VectorLength(delta);
                if (len > 0.1f) {
                    if (cg_showmiss->current.integer)
                        Com_Printf("Prediction miss: %f\n", len);

                    if (cg_errorDecay->current.value != 0.0f) {
                        f = (cg_errorDecay->current.value - (float)(cg->time - cg->predictedErrorTime)) / cg_errorDecay->current.value;
                        if (f < 0.0f)
                            f = 0.0f;
                        if (f > 0.0f && cg_showmiss->current.integer)
                            Com_Printf("Double prediction decay: %f\n", f);
                        cg->predictedError[0] *= f;
                        cg->predictedError[1] *= f;
                        cg->predictedError[2] *= f;
                    } else {
                        cg->predictedError[0] = 0.0f;
                        cg->predictedError[1] = 0.0f;
                        cg->predictedError[2] = 0.0f;
                    }

                    cg->predictedError[0] += delta[0];
                    cg->predictedError[1] += delta[1];
                    cg->predictedError[2] += delta[2];
                    cg->predictedErrorTime = cg->oldTime;
                }
            }

            Pmove(&cg_pmove[0]);
            CG_TouchTriggerPrediction();
            moved = 1;
        }
    }

    if (cg_showmiss->current.integer > 1)
        Com_Printf("[%i : %i] ", cg_pmove[0].cmd.serverTime, cg->time);

    if (!moved) {
        if (cg_showmiss->current.integer)
            Com_Printf("no prediction run\n");
    } else {
        CG_AdjustPositionForMover(cg->predictedPlayerState.origin, cg->predictedPlayerState.groundEntityNum,
                                  cg->physicsTime, cg->time, cg->predictedPlayerState.origin, deltaAngles);
    }

    CG_TransitionPlayerState(&cg->predictedPlayerState, oldPlayerState);
}

void CG_PredictPlayerState(void)
{
    playerState_t *ps;
    centity_t *cent;
    WeaponDef *weapDef;
    float fZoom;
    float size;
    float angle;
    float fSin;
    float fCos;

    CG_PredictPlayerState_Internal();

    ps = &cg->predictedPlayerState;
    cent = &cg_entities[ps->clientNum];
    cent->lerpOrigin[0] = ps->origin[0];
    cent->lerpOrigin[1] = ps->origin[1];
    cent->lerpOrigin[2] = ps->origin[2];
    BG_EvaluateTrajectory(&cent->currentState.apos, cg->time, cent->lerpAngles);

    weapDef = BG_GetWeaponDef(ps->weapon);
    if ((cg->nextSnap->ps.pm_flags & 0x800000) && CG_GetWeapReticleZoom(&fZoom)) {
        if (weapDef->adsViewErrorMax != 0.0f && !cg->adsViewErrorDone) {
            cg->adsViewErrorDone = 1;
            size = flrand(weapDef->adsViewErrorMin, weapDef->adsViewErrorMax);
            angle = randomf() * (2.0f * 3.14159265358979323846f);
            FastSinCos(angle, &fSin, &fCos);
            cg->offsetAngles[0] = AngleNormalize360(fSin * size + cg->offsetAngles[0]);
            cg->offsetAngles[1] = AngleNormalize360(fCos * size + cg->offsetAngles[1]);
        }
    } else {
        cg->adsViewErrorDone = 0;
    }

    cg->predictedPlayerEntity.nextState.number = (unsigned short)ps->clientNum;
    BG_PlayerStateToEntityState(ps, &cg->predictedPlayerEntity.nextState, 0, 0);
    memcpy(&cg->predictedPlayerEntity.currentState, &cg->predictedPlayerEntity.nextState, sizeof(entityState_t));
}

static void Trace_CalcBounds(const vec_t *start, const vec_t *mins, const vec_t *maxs, float fraction, const vec_t *end, vec3_t *bounds)
{
    float hit0, hit1, hit2;

    hit0 = start[0] + (end[0] - start[0]) * fraction;
    hit1 = start[1] + (end[1] - start[1]) * fraction;
    hit2 = start[2] + (end[2] - start[2]) * fraction;

    bounds[0][0] = (start[0] < hit0 ? start[0] : hit0) + mins[0];
    bounds[0][1] = (start[1] < hit1 ? start[1] : hit1) + mins[1];
    bounds[0][2] = (start[2] < hit2 ? start[2] : hit2) + mins[2];

    bounds[1][0] = (start[0] > hit0 ? start[0] : hit0) + maxs[0];
    bounds[1][1] = (start[1] > hit1 ? start[1] : hit1) + maxs[1];
    bounds[1][2] = (start[2] > hit2 ? start[2] : hit2) + maxs[2];
}

#if 0  // engine source
void CG_ClipMoveToEntities(const vec_t *start, const vec_t *mins, const vec_t *maxs, const vec_t *end, int skipNumber, int mask, int capsule, trace_t *tr)
{
    int i;
    vec3_t bounds[2];
    trace_t trace;
    vec3_t bmins, bmaxs;
    vec3_t angles;
    unsigned int cmodel;
    centity_t **pp;

    Trace_CalcBounds(start, mins, maxs, tr->fraction, end, bounds);

    pp = (centity_t **)&cg_solidEntities;
    for (i = 0; i < cg_numSolidEntities; i++, pp++) {
        centity_t *cent = *pp;
        entityState_t *ent = &cent->nextState;

        if (ent->number == skipNumber)
            continue;

        if (ent->solid == 0xffffff) {

            cmodel = ent->index.brushmodel;

            if (!(CM_ContentsOfModel(cmodel) & mask))
                continue;

            {
                float radius = CM_RadiusOfModel(cmodel);

                if (cent->lerpOrigin[0] - radius >= bounds[1][0])
                    continue;
                if (cent->lerpOrigin[1] - radius >= bounds[1][1])
                    continue;
                if (cent->lerpOrigin[0] + radius <= bounds[0][0])
                    continue;
                if (cent->lerpOrigin[1] + radius <= bounds[0][1])
                    continue;
                if (cent->lerpOrigin[2] - radius >= bounds[1][2])
                    continue;
                if (cent->lerpOrigin[2] + radius <= bounds[0][2])
                    continue;
            }

            angles[0] = cent->lerpAngles[0];
            angles[1] = cent->lerpAngles[1];
            angles[2] = cent->lerpAngles[2];
        } else {

            int solid = ent->solid;
            int w, h1, h2;
            int contentFlags;

            contentFlags = (ent->eType == 1) ? 0x2000000 : 1;
            if (!(contentFlags & mask))
                continue;

            w = solid & 0xFF;
            h1 = (solid >> 8) & 0xFF;
            h2 = (solid >> 16) & 0xFF;

            bmins[0] = bmins[1] = 1.0f - (float)w;
            bmaxs[0] = bmaxs[1] = (float)w - 1.0f;
            bmins[2] = 1.0f - (float)(h1 - 1);
            bmaxs[2] = (float)(h2 - 32) - 1.0f;

            if (cent->lerpOrigin[0] + bmins[0] >= bounds[1][0])
                continue;
            if (cent->lerpOrigin[1] + bmins[1] >= bounds[1][1])
                continue;
            if (cent->lerpOrigin[0] + bmaxs[0] <= bounds[0][0])
                continue;
            if (cent->lerpOrigin[1] + bmaxs[1] <= bounds[0][1])
                continue;
            if (cent->lerpOrigin[2] + bmins[2] >= bounds[1][2])
                continue;
            if (cent->lerpOrigin[2] + bmaxs[2] <= bounds[0][2])
                continue;

            cmodel = CM_TempBoxModel(bmins, bmaxs, capsule);

            angles[0] = 0.0f;
            angles[1] = 0.0f;
            angles[2] = 0.0f;
        }

        CM_TransformedBoxTraceExternal(&trace, start, end, mins, maxs,
                                       cmodel, mask, cent->lerpOrigin, angles);

        if (trace.fraction < tr->fraction) {

            trace.entityNum = ent->number;
            *tr = trace;

            Trace_CalcBounds(start, mins, maxs, trace.fraction, end, bounds);
        } else if (trace.allsolid) {
            trace.entityNum = ent->number;
            *tr = trace;
        } else if (trace.startsolid) {
            tr->startsolid = 1;
        }

        if (tr->allsolid)
            return;
    }
}
#endif

void CG_BuildSolidList(void)
{
    int i;
    snapshot_t *snap;

    cg_numSolidEntities = 0;
    cg_numTriggerEntities = 0;

    snap = cg->nextSnap;

    for (i = 0; i < snap->numEntities; i++) {
        entityState_t *snapEnt = &snap->entities[i];
        int entityNum = snapEnt->number;
        centity_t *cent = &cg_entities[entityNum];
        entityState_t *ent = &cent->nextState;

        if (ent->solid == 0xffffff) {
            if (ent->eFlags & 1)
                continue;
            if (CM_ContentsOfModel(ent->index.brushmodel) == 0)
                continue;
        }

        if (ent->eType == 3) {
            cg_triggerEntities[cg_numTriggerEntities] = cent;
            cg_numTriggerEntities++;
        } else if (ent->solid != 0) {
            ((centity_t **)&cg_solidEntities)[cg_numSolidEntities] = cent;
            cg_numSolidEntities++;
        }
    }
}

const unsigned char h1[256] = {
    0xc9, 0xd5, 0x16, 0x38, 0x41, 0x9e, 0xeb, 0x38, 0x10, 0xa6, 0xe7, 0xb8, 0x77, 0x4a, 0x92, 0xb9,
    0x3a, 0x02, 0x71, 0x39, 0xaf, 0x11, 0x1c, 0x3a, 0xe8, 0x77, 0xc8, 0xb9, 0xab, 0x31, 0x95, 0xba,
    0x01, 0x29, 0x0b, 0x3a, 0x85, 0x2a, 0x02, 0x3b, 0xec, 0xac, 0x23, 0xba, 0x02, 0x11, 0x54, 0xbb,
    0xb0, 0x8d, 0x16, 0x3a, 0x00, 0x7c, 0xa3, 0x3b, 0x28, 0x84, 0x87, 0xb9, 0x4f, 0x68, 0xf1, 0xbb,
    0x1a, 0xb2, 0xfe, 0xb9, 0x17, 0x25, 0x2c, 0x3c, 0x0d, 0x58, 0xf8, 0x3a, 0x81, 0x1b, 0x6f, 0xbc,
    0xb8, 0x59, 0x8d, 0xbb, 0xde, 0x60, 0xa3, 0x3c, 0x8c, 0xc8, 0x07, 0x3c, 0xcd, 0x7f, 0xde, 0xbc,
    0x11, 0x5e, 0x73, 0xbc, 0x9a, 0x36, 0x1a, 0x3d, 0x61, 0xa7, 0xd8, 0x3c, 0x23, 0x0d, 0x63, 0xbd,
    0x0e, 0xb6, 0x50, 0xbd, 0xa0, 0x46, 0xc8, 0x3d, 0xd1, 0x8d, 0x0d, 0x3e, 0xfa, 0x91, 0xeb, 0xbe,
    0xfa, 0x91, 0xeb, 0x3e, 0xd1, 0x8d, 0x0d, 0xbe, 0xa0, 0x46, 0xc8, 0xbd, 0x0e, 0xb6, 0x50, 0x3d,
    0x23, 0x0d, 0x63, 0x3d, 0x61, 0xa7, 0xd8, 0xbc, 0x9a, 0x36, 0x1a, 0xbd, 0x11, 0x5e, 0x73, 0x3c,
    0xcd, 0x7f, 0xde, 0x3c, 0x8c, 0xc8, 0x07, 0xbc, 0xde, 0x60, 0xa3, 0xbc, 0xb8, 0x59, 0x8d, 0x3b,
    0x81, 0x1b, 0x6f, 0x3c, 0x0d, 0x58, 0xf8, 0xba, 0x17, 0x25, 0x2c, 0xbc, 0x1a, 0xb2, 0xfe, 0x39,
    0x4f, 0x68, 0xf1, 0x3b, 0x28, 0x84, 0x87, 0x39, 0x00, 0x7c, 0xa3, 0xbb, 0xb0, 0x8d, 0x16, 0xba,
    0x02, 0x11, 0x54, 0x3b, 0xec, 0xac, 0x23, 0x3a, 0x85, 0x2a, 0x02, 0xbb, 0x01, 0x29, 0x0b, 0xba,
    0xab, 0x31, 0x95, 0x3a, 0xe8, 0x77, 0xc8, 0x39, 0xaf, 0x11, 0x1c, 0xba, 0x3a, 0x02, 0x71, 0xb9,
    0x77, 0x4a, 0x92, 0x39, 0x10, 0xa6, 0xe7, 0x38, 0x41, 0x9e, 0xeb, 0xb8, 0xc9, 0xd5, 0x16, 0xb8
};
