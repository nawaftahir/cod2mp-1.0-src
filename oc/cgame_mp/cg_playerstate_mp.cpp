#include "common_types.h"
#include "imports.h"
#include "headers/PC/cgame_mp/cg_local.h"

extern const dvar_t *cg_hudDamageIconTime;

extern char *cg_globUI[];
extern LegacyHacks *legacyHacks;
extern void *imp_cg_hudDamageIconTime;

extern void AngleVectors( const vec3_t angles, vec3_t forward, vec3_t right, vec3_t up );
extern float AngleNormalize360( float angle );
extern float randomf(void);
extern void CL_SetADS(int ads);
extern void CG_SetEquippedOffHand(int offHandIndex);
extern void CG_HoldBreathInit(void);
extern void CG_ResetLowHealthOverlay(void);
extern void CG_MenuShowNotify(int menuToShow);
extern void CG_EntityEvent(centity_t *cent, int event);

void CG_Respawn(void);
void CG_DamageFeedback(int yawByte, int pitchByte, int damage);
void CG_TransitionPlayerState(playerState_t *ps, playerState_t *ops);

#define CG_SNAP_PTR 0x20
#define CG_OFF_25BB0 0x25bb0
#define CG_OFF_25BBC 0x25bbc
#define CG_OFF_25BC4 0x25bc4
#define CG_OFF_25C94 0x25c94
#define CG_OFF_25C98 0x25c98
#define CG_OFF_28490 0x28490
#define CG_OFF_284C4 0x284c4
#define CG_OFF_285D4 0x285d4
#define CG_OFF_285E0 0x285e0
#define CG_OFF_285EC 0x285ec
#define CG_OFF_28594 0x28594
#define CG_OFF_285A0 0x285a0
#define CG_OFF_2826C 0x2826c
#define CG_OFF_2BDE8 0x2bde8
#define CG_OFF_2BDEC 0x2bdec
#define CG_OFF_2BE30 0x2be30
#define CG_OFF_2BE48 0x2be48
#define CG_OFF_2BE50 0x2be50
#define CG_OFF_2BE54 0x2be54
#define CG_OFF_2BE70 0x2be70
#define CG_OFF_2BE74 0x2be74
#define CG_OFF_2BE80 0x2be80
#define CG_OFF_2BED4 0x2bed4
#define CG_OFF_2BF0C 0x2bf0c
#define CG_OFF_2BF10 0x2bf10
#define CG_OFF_2BF14 0x2bf14
#define CG_OFF_2BF1C 0x2bf1c
#define CG_OFF_2C030 0x2c030
#define CG_OFF_2C03C 0x2c03c
#define CG_OFF_2C0A4 0x2c0a4
#define CG_OFF_2C0B0 0x2c0b0
#define CG_OFF_2C50C 0x2c50c
#define CG_OFF_2CD10 0x2cd10

#define CENT_EVENT_PARM 0x190

#define CG_INT(base, off) (*(int *)((char *)(base) + (off)))
#define CG_FLOAT(base, off) (*(float *)((char *)(base) + (off)))
#define CG_PTR(base, off) (*(char **)((char *)(base) + (off)))

#define VectorClear(v) ((v)[0] = (v)[1] = (v)[2] = 0.0f)

void CG_DamageFeedback(int yawByte, int pitchByte, int damage)
{
    float kick;
    float yaw;
    float pitch;
    vec3_t angles;
    vec3_t dir;
    int slot;
    int i;

    cg->attackerTime = cg->time;

    kick = damage * 0.2f;
    if (kick < 5.0f)
        kick = 5.0f;
    else if (kick > 90.0f)
        kick = 90.0f;

    if (yawByte == 255 && pitchByte == 255) {
        cg->v_dmg_roll = 0.0f;
        cg->v_dmg_pitch = -kick;
    } else {
        pitch = pitchByte / 255.0f * 360.0f;
        yaw = yawByte / 255.0f * 360.0f;
        angles[0] = pitch;
        angles[1] = yaw;
        angles[2] = 0.0f;
        AngleVectors(angles, dir, NULL, NULL);

        cg->v_dmg_roll = -((dir[0] * cg->refdef.viewaxis[1][0] + dir[1] * cg->refdef.viewaxis[1][1] + dir[2] * cg->refdef.viewaxis[1][2]) * kick);
        cg->v_dmg_pitch = (dir[0] * cg->refdef.viewaxis[0][0] + dir[1] * cg->refdef.viewaxis[0][1] + dir[2] * cg->refdef.viewaxis[0][2]) * kick;

        slot = 0;
        for (i = 1; i < 8; i++) {
            if (cg->viewDamage[i].time < cg->viewDamage[slot].time)
                slot = i;
        }

        cg->viewDamage[slot].time = cg->snap->serverTime;
        cg->viewDamage[slot].duration = cg_hudDamageIconTime->current.integer;
        cg->viewDamage[slot].yaw = AngleNormalize360(yaw + (randomf() - 0.5f) * 20.0f);
    }

    cg->v_dmg_time = cg->time + 500;
    cg->damageTime = cg->snap->serverTime;

    CG_MenuShowNotify(0);
}

void CG_Respawn(void)
{
    char *snap;

    CG_INT(cg, CG_OFF_25BBC) = 0;

    snap = CG_PTR(cg, CG_SNAP_PTR);
    memcpy((char *)cg + CG_OFF_25BC4, snap + 0xc, sizeof(playerState_t));

    CG_INT(cg, CG_OFF_2BE50) = CG_INT(cg, CG_OFF_25C98);

    CG_INT(cg, CG_OFF_2BE54) = CG_INT(cg, CG_OFF_25BB0);

    CG_INT(cg, CG_OFF_2BE70) = CG_INT(cg, CG_OFF_25C94);

    CG_INT(cg, CG_OFF_2BDE8) = 0;
    CG_INT(cg, CG_OFF_2BDEC) = 0;
    CG_INT(cg, CG_OFF_2BE30) = 0;

    VectorClear(&CG_FLOAT(cg, CG_OFF_285D4));
    VectorClear(&CG_FLOAT(cg, CG_OFF_285E0));
    VectorClear(&CG_FLOAT(cg, CG_OFF_285EC));

    VectorClear(&CG_FLOAT(cg, CG_OFF_2C03C));
    VectorClear(&CG_FLOAT(cg, CG_OFF_2C030));

    CG_FLOAT(cg, CG_OFF_2BF1C) = 0.0f;

    memset((char *)cg + CG_OFF_28490, 0, 48);

    CG_INT(cg, CG_OFF_2BED4) = 0;

    CG_FLOAT(cg, CG_OFF_2BF10) = 0.0f;
    CG_FLOAT(cg, CG_OFF_2BF14) = 0.0f;

    VectorClear(&CG_FLOAT(cg, CG_OFF_2C0A4));
    VectorClear(&CG_FLOAT(cg, CG_OFF_2C0B0));

    memset((char *)cg + CG_OFF_2BE74, 0, 0x60);

    memset((char *)cg + CG_OFF_2C50C, 0, 0x90);

    VectorClear(&CG_FLOAT(cg, CG_OFF_284C4));

    CG_INT(cg, CG_OFF_2CD10) = 0;

    {
        char *uiBase = (char *)::legacyHacks;
        CG_INT(uiBase, 8) = 0;
    }

    CL_SetADS(0);

    CG_SetEquippedOffHand(CG_INT(cg, CG_OFF_25C94));

    CG_HoldBreathInit();

    CG_ResetLowHealthOverlay();
}

static void CG_CheckPlayerstateEvents(playerState_t *ps, playerState_t *ops)
{
    char *cent;
    int i;
    int event;

    cent = (char *)cg + CG_OFF_2826C;
    for (i = ps->eventSequence - 4; i < ps->eventSequence; i++) {
        if (i >= ops->eventSequence
            || (i > ops->eventSequence - 4 && ps->events[i & 3] != ops->events[i & 3])) {
            event = ps->events[i & 3];
            CG_INT(cent, CENT_EVENT_PARM) = ps->eventParms[i & 3];
            CG_EntityEvent((centity_t *)cent, event);
        }
    }
}

void CG_TransitionPlayerState(playerState_t *ps, playerState_t *ops)
{
    if (ps->damageEvent != ops->damageEvent && ps->damageCount)
        CG_DamageFeedback(ps->damageYaw, ps->damagePitch, ps->damageCount);

    CG_CheckPlayerstateEvents(ps, ops);
}

