#include "common_types.h"
#include "imports.h"
#include "bytematch.h"
#include "cod2_feature_config.h"
#include <math.h>

const dvar_t *cl_stanceHoldTime;
const dvar_t *cl_talking;
const dvar_t *cl_analog_attack_threshold;
const dvar_t *cl_yawspeed;
const dvar_t *cl_bypassMouseInput;
const dvar_t *cl_pitchspeed;
const dvar_t *cl_anglespeedkey;
extern clientActive_t clients[1];
extern clientConnection_t clientConnections[1];
#define clc clientConnections[0]
/* dvar globals */
extern LegacyHacks *legacyHacks;
extern const dvar_t *cl_nodelta;
extern const dvar_t *cl_packetdup;

#if COD2_FEATURE_GAMEPAD
extern void CL_Gamepad_Init(void);
extern void CL_Gamepad_Shutdown(void);
extern void CL_Gamepad_Move(usercmd_t *cmd);
extern void CL_Gamepad_NoteKbmActive(void);
#endif

extern const dvar_t *cl_yawspeed;
extern const dvar_t *cl_pitchspeed;
extern const dvar_t *cl_anglespeedkey;
extern const dvar_t *cl_talking;
extern const dvar_t *cl_bypassMouseInput;
extern const dvar_t *cl_analog_attack_threshold;
extern const dvar_t *cl_stanceHoldTime;
extern const dvar_t *cl_maxpackets;
extern const dvar_t *cl_freelook;
extern const dvar_t *cl_showSend;
extern const dvar_t *cl_sensitivity;
extern const dvar_t *cl_mouseAccel;
extern const dvar_t *cl_showMouseRate;
extern const dvar_t *m_pitch;
extern const dvar_t *m_yaw;
extern const dvar_t *m_forward;
extern const dvar_t *m_side;
extern const dvar_t *m_filter;
extern int atoi(const char *nptr);
extern const char *Cmd_Argv( int arg );
extern void Com_Printf(const char *fmt, ...);
extern void Cmd_AddCommand(const char *cmd, void (*func)(void));
extern void Cmd_RemoveCommand(const char *cmd);
dvar_t *Dvar_RegisterBool(const char *dvarName, bool value, unsigned short flags);
extern dvar_t *Dvar_RegisterFloat(const char *dvarName, float value, float min, float max, unsigned short flags);
extern dvar_t *Dvar_RegisterInt(const char *dvarName, int value, int min, int max, unsigned short flags);
extern void MSG_Init(msg_t *buf, byte *data, int length);
extern void MSG_WriteString(msg_t *msg, const char *s);
extern void MSG_WriteShort(msg_t *msg, int c);
extern void MSG_WriteByte(msg_t *msg, int c);
extern void MSG_WriteData(msg_t *buf, const void *data, int length);
extern void MSG_WriteLong(msg_t *msg, int c);
extern void MSG_WriteBits(msg_t *msg, int value, int bits);
extern int MSG_WriteBitsCompress(byte *from, byte *to, int size);
extern void MSG_SetDefaultUserCmd(playerState_t *from, usercmd_t *to);
extern void MSG_WriteDeltaUsercmdKey(msg_t *msg, int key, usercmd_t *from, usercmd_t *to);
extern int Com_HashKey( const char *string, int maxlen );
extern void CL_Netchan_Transmit(netchan_t *chan, byte *data, int length);
extern void CL_Netchan_TransmitNextFragment(netchan_t *chan);
extern void NET_OutOfBandVoiceData(netsrc_t sock, netadr_t adr, byte *format, int len);
extern void CL_SyncGpu(void);
extern void CL_SendCmdInternal(void);
extern int PM_IsBinocularsADS(const playerState_t *ps);
extern signed char ClampChar(int i);
extern float sqrtf(float x);
extern qboolean Sys_IsLANAddress(netadr_t adr);
extern void IN_Frame(void);
extern struct clientStatic_t cls;
extern int com_frameTime;
extern unsigned int frame_msec;

extern void UI_MouseEvent(int dx, int dy);
static kbutton_t playersKb[1][28];
static kbutton_t *kb = playersKb[0];

typedef struct
{
    const char *name;
    void (*handler)(void);
} inputCommandDef_t;

void IN_MLookDown(void);
void IN_CenterView(void);
void IN_KeyDown(kbutton_t *b);
void IN_KeyUp(kbutton_t *b);
void IN_UpDown(void);
void IN_UpUp(void);
void IN_LeftDown(void);
void IN_LeftUp(void);
void IN_RightDown(void);
void IN_RightUp(void);
void IN_ForwardDown(void);
void IN_ForwardUp(void);
void IN_BackDown(void);
void IN_BackUp(void);
void IN_LookupDown(void);
void IN_LookupUp(void);
void IN_LookdownDown(void);
void IN_LookdownUp(void);
void IN_MoveleftDown(void);
void IN_MoveleftUp(void);
void IN_MoverightDown(void);
void IN_MoverightUp(void);
void IN_SpeedDown(void);
void IN_SpeedUp(void);
void IN_StrafeDown(void);
void IN_StrafeUp(void);
void IN_Attack_Down(void);
void IN_Attack_Up(void);
void IN_Breath_Down(void);
void IN_Breath_Up(void);
void IN_MeleeBreath_Down(void);
void IN_MeleeBreath_Up(void);
void IN_Frag_Down(void);
void IN_Frag_Up(void);
void IN_Smoke_Down(void);
void IN_Smoke_Up(void);
void IN_Binoculars_Down(void);
void IN_Binoculars_Up(void);
void IN_BreathBinoculars_Down(void);
void IN_BreathBinoculars_Up(void);
void IN_Melee_Down(void);
void IN_Melee_Up(void);
void IN_Activate_Down(void);
void IN_Activate_Up(void);
void IN_Reload_Down(void);
void IN_Reload_Up(void);
void IN_UseReload_Down(void);
void IN_UseReload_Up(void);
void IN_LeanLeft_Down(void);
void IN_LeanLeft_Up(void);
void IN_LeanRight_Down(void);
void IN_LeanRight_Up(void);
void IN_Stance_Down(void);
void IN_Stance_Up(void);
void IN_ToggleADS(void);
void IN_LeaveADS(void);
void IN_LowerStance(void);
void IN_RaiseStance(void);
void IN_ToggleCrouch(void);
void IN_ToggleProne(void);
void IN_GoProne(void);
void IN_GoCrouch(void);
void IN_GoStandDown(void);
void IN_GoStandUp(void);
void IN_TalkDown(void);
void IN_TalkUp(void);
Bool IsTalking(void);
void CL_MouseEvent(const int dx, const int dy);
void CL_WriteVoicePacket(void);
void CL_WritePacket(void);
void CL_InitInput(void);
void CL_ShutdownInput(void);
void CL_CmdButtons(usercmd_t *cmd);
void CL_MouseMove(usercmd_t *cmd);
void IN_DownDown(void);
void IN_DownUp(void);
void IN_Prone_Down(void);
void IN_Prone_Up(void);
void IN_MLookUp(void);
void CL_AdjustAngles(void);
void CL_KeyMove(usercmd_t *cmd);
usercmd_t CL_CreateCmd(void);
void CL_SendCmdInternal(void);

static const inputCommandDef_t s_inputCommands[] = {
    { "centerview", IN_CenterView },
    { "+moveup", IN_UpDown },
    { "-moveup", IN_UpUp },
    { "+movedown", IN_DownDown },
    { "-movedown", IN_DownUp },
    { "+left", IN_LeftDown },
    { "-left", IN_LeftUp },
    { "+right", IN_RightDown },
    { "-right", IN_RightUp },
    { "+forward", IN_ForwardDown },
    { "-forward", IN_ForwardUp },
    { "+back", IN_BackDown },
    { "-back", IN_BackUp },
    { "+lookup", IN_LookupDown },
    { "-lookup", IN_LookupUp },
    { "+lookdown", IN_LookdownDown },
    { "-lookdown", IN_LookdownUp },
    { "+strafe", IN_StrafeDown },
    { "-strafe", IN_StrafeUp },
    { "+moveleft", IN_MoveleftDown },
    { "-moveleft", IN_MoveleftUp },
    { "+moveright", IN_MoverightDown },
    { "-moveright", IN_MoverightUp },
    { "+speed", IN_SpeedDown },
    { "-speed", IN_SpeedUp },
    { "+attack", IN_Attack_Down },
    { "-attack", IN_Attack_Up },
    { "+melee", IN_Melee_Down },
    { "-melee", IN_Melee_Up },
    { "+holdbreath", IN_Breath_Down },
    { "-holdbreath", IN_Breath_Up },
    { "+melee_breath", IN_MeleeBreath_Down },
    { "-melee_breath", IN_MeleeBreath_Up },
    { "+frag", IN_Frag_Down },
    { "-frag", IN_Frag_Up },
    { "+smoke", IN_Smoke_Down },
    { "-smoke", IN_Smoke_Up },
    { "+binoculars", IN_Binoculars_Down },
    { "-binoculars", IN_Binoculars_Up },
    { "+breath_binoculars", IN_BreathBinoculars_Down },
    { "-breath_binoculars", IN_BreathBinoculars_Up },
    { "+activate", IN_Activate_Down },
    { "-activate", IN_Activate_Up },
    { "+reload", IN_Reload_Down },
    { "-reload", IN_Reload_Up },
    { "+usereload", IN_UseReload_Down },
    { "-usereload", IN_UseReload_Up },
    { "+leanleft", IN_LeanLeft_Down },
    { "-leanleft", IN_LeanLeft_Up },
    { "+leanright", IN_LeanRight_Down },
    { "-leanright", IN_LeanRight_Up },
    { "+prone", IN_Prone_Down },
    { "-prone", IN_Prone_Up },
    { "+stance", IN_Stance_Down },
    { "-stance", IN_Stance_Up },
    { "+mlook", IN_MLookDown },
    { "-mlook", IN_MLookUp },
    { "toggleads", IN_ToggleADS },
    { "leaveads", IN_LeaveADS },
    { "lowerstance", IN_LowerStance },
    { "raisestance", IN_RaiseStance },
    { "togglecrouch", IN_ToggleCrouch },
    { "toggleprone", IN_ToggleProne },
    { "goprone", IN_GoProne },
    { "gocrouch", IN_GoCrouch },
    { "+gostand", IN_GoStandDown },
    { "-gostand", IN_GoStandUp },
    { "+talk", IN_TalkDown },
    { "-talk", IN_TalkUp },
};
void CL_Input(void);
void CL_SendCmd(void);

void CL_WritePacket(void);

static float I_fsel(const float x, const float y, const float z)
{
    return x >= 0.0f ? y : z;
}

static float I_fmax(const float x, const float y)
{
    return I_fsel(x - y, x, y);
}

static float I_fmin(const float x, const float y)
{
    return I_fsel(y - x, x, y);
}














void IN_MLookDown(void)
{
    kb[13].active = 1;
}

void IN_MLookUp(void)
{
    kb[13].active = 0;
    if ((cl_freelook)->current.enabled == 0) {
        clientActive_t *cl = clients;
        cl->viewangles[0] = -((float)cl->snap.ps.delta_angles[0] * 0.0054931640625f);
    }
}

void IN_KeyDown(kbutton_t *b)
{
    const char *keyText;
    int key;

    keyText = Cmd_Argv(1);
    key = keyText[0] ? atoi(keyText) : -1;

    if (key == b->down[0] || key == b->down[1]) {
        return;
    }

    if (!b->down[0]) {
        b->down[0] = key;
    } else if (!b->down[1]) {
        b->down[1] = key;
    } else {
        Com_Printf("Three keys down for a button!\n");
        return;
    }

    if (b->active) {
        return;
    }

    b->downtime = atoi(Cmd_Argv(2));
    b->active = 1;
    b->wasPressed = 1;
}

void IN_KeyUp(kbutton_t *b)
{
    int k;
    const char *c;
    unsigned int uptime;

    c = Cmd_Argv(1);
    if (c[0]) {
        k = atoi(c);
    } else {
        // typed manually at the console, assume for unsticking, so clear all
        b->down[0] = b->down[1] = 0;
        b->active = 0;
        return;
    }

    if (b->down[0] == k) {
        b->down[0] = 0;
    } else if (b->down[1] == k) {
        b->down[1] = 0;
    } else {
        return;     // key up without coresponding down (menu pass through)
    }
    if (b->down[0] || b->down[1]) {
        return;     // some other key is still holding it down
    }

    b->active = 0;

    // save timestamp for partial frame summing
    c = Cmd_Argv(2);
    uptime = atoi(c);
    if (uptime) {
        b->msec += uptime - b->downtime;
    } else {
        b->msec += frame_msec / 2;
    }

    b->active = 0;
}

float CL_KeyState(kbutton_t *key)
{
    int msec;

    msec = key->msec;
    key->msec = 0;

    if (key->active) {
        if (!key->downtime) {
            msec = com_frameTime;
        } else {
            msec += com_frameTime - key->downtime;
        }

        key->downtime = com_frameTime;
    }

    if (msec <= 0) {
        return 0.0f;
    }

    if (msec >= frame_msec) {
        return 1.0f;
    }

    return (float)msec / (float)frame_msec;
}

// retail 0x406db0, unreferenced; name unknown
void IN_SetStanceFromKeys_406db0(void)
{
    if (!kb[25].active && !kb[11].active)
        legacyHacks->cl_stanceTemp = 0;
    else
        legacyHacks->cl_stanceTemp = 1;
}

void IN_UpDown(void)
{
    int *stance;

    IN_KeyDown(&kb[12]);
    if (kb[25].active || kb[11].active ) {
        return;
    }

    stance = &legacyHacks->cl_stance;
    if (*stance > 1) {
        *stance = 1;
    } else if (*stance > 0) {
        *stance = 0;
    } else {
        IN_KeyDown(&kb[10]);
    }
}

void IN_LeftDown(void)
{
    IN_KeyDown(kb);
}

void IN_LeftUp(void)
{
    IN_KeyUp(kb);
}

void IN_RightDown(void)
{
    IN_KeyDown(&kb[1]);
}

void IN_RightUp(void)
{
    IN_KeyUp(&kb[1]);
}

void IN_ForwardDown(void)
{
    IN_KeyDown(&kb[2]);
}

void IN_ForwardUp(void)
{
    IN_KeyUp(&kb[2]);
}

void IN_BackDown(void)
{
    IN_KeyDown(&kb[3]);
}

void IN_BackUp(void)
{
    IN_KeyUp(&kb[3]);
}

void IN_LookupDown(void)
{
    IN_KeyDown(&kb[4]);
}

void IN_LookupUp(void)
{
    IN_KeyUp(&kb[4]);
}

void IN_LookdownDown(void)
{
    IN_KeyDown(&kb[5]);
}

void IN_LookdownUp(void)
{
    IN_KeyUp(&kb[5]);
}

void IN_MoveleftDown(void)
{
    IN_KeyDown(&kb[6]);
}

void IN_MoveleftUp(void)
{
    IN_KeyUp(&kb[6]);
}

void IN_MoverightDown(void)
{
    IN_KeyDown(&kb[7]);
}

void IN_MoverightUp(void)
{
    IN_KeyUp(&kb[7]);
}

void IN_SpeedDown(void)
{
    IN_KeyDown(&kb[9]);
}

void IN_SpeedUp(void)
{
    IN_KeyUp(&kb[9]);
}

void IN_StrafeDown(void)
{
    IN_KeyDown(&kb[8]);
}

void IN_StrafeUp(void)
{
    IN_KeyUp(&kb[8]);
}

void IN_Attack_Down(void)
{
    IN_KeyDown(&kb[14]);
}

void IN_Attack_Up(void)
{
    IN_KeyUp(&kb[14]);
}

void IN_Breath_Down(void)
{
    IN_KeyDown(&kb[15]);
}

void IN_Breath_Up(void)
{
    IN_KeyUp(&kb[15]);
}

void IN_Frag_Down(void)
{
    IN_KeyDown(&kb[16]);
}

void IN_Frag_Up(void)
{
    IN_KeyUp(&kb[16]);
}

void IN_Smoke_Down(void)
{
    IN_KeyDown(&kb[17]);
}

void IN_Smoke_Up(void)
{
    IN_KeyUp(&kb[17]);
}

void IN_Binoculars_Down(void)
{
    IN_KeyDown(&kb[18]);
}

void IN_Binoculars_Up(void)
{
    IN_KeyUp(&kb[18]);
}

void IN_Melee_Down(void)
{
    IN_KeyDown(&kb[19]);
}

void IN_Melee_Up(void)
{
    IN_KeyUp(&kb[19]);
}

void IN_Activate_Down(void)
{
    IN_KeyDown(&kb[20]);
}

void IN_Activate_Up(void)
{
    IN_KeyUp(&kb[20]);
}

void IN_Reload_Down(void)
{
    IN_KeyDown(&kb[21]);
}

void IN_Reload_Up(void)
{
    IN_KeyUp(&kb[21]);
}

void IN_UseReload_Down(void)
{
    IN_KeyDown(&kb[22]);
}

void IN_UseReload_Up(void)
{
    IN_KeyUp(&kb[22]);
}

void IN_LeanLeft_Down(void)
{
    IN_KeyDown(&kb[23]);
}

void IN_LeanLeft_Up(void)
{
    IN_KeyUp(&kb[23]);
}

void IN_LeanRight_Down(void)
{
    IN_KeyDown(&kb[24]);
}

void IN_LeanRight_Up(void)
{
    IN_KeyUp(&kb[24]);
}

void IN_Stance_Down(void)
{
    clientActive_t *cl;

    if (kb[25].active || kb[11].active ) {
        return;
    }

    cl = clients;
    cl->stanceHeld = 1;
    cl->stancePosition = legacyHacks->cl_stance;
    cl->stanceTime = ((int)com_frameTime);
    if (cl->stancePosition != 1) {
        legacyHacks->cl_stance = 1;
    }
}

void IN_Stance_Up(void)
{
    clientActive_t *ptr;
    if (kb[25].active || kb[11].active )
        return;
    ptr = clients;
    if (ptr->stanceHeld && ptr->stancePosition == 1) {
        legacyHacks->cl_stance = 0;
    }
    ptr->stanceHeld = 0;
}

// retail 0x407630, unreferenced; name unknown
void IN_Kb26_Down_407630(void)
{
    IN_KeyDown(&kb[26]);
}

// retail 0x407650, unreferenced; name unknown
void IN_Kb26_Up_407650(void)
{
    IN_KeyUp(&kb[26]);
}

void IN_CenterView(void)
{
    clientActive_t *cl = clients;
    cl->viewangles[0] = -((float)cl->snap.ps.delta_angles[0] * 0.0054931640625f);
}

void IN_ToggleADS(void)
{
    clientActive_t *p = clients;
    p->usingAds = (p->usingAds == 0) ? 1 : 0;
}

void IN_LeaveADS(void)
{
    (clients)->usingAds = 0;
}

void IN_LowerStance(void)
{
    int *statePtr;
    int val;
    if (kb[25].active || kb[11].active )
        return;
    statePtr = &((LegacyHacks *)::legacyHacks)->cl_stance;
    val = *statePtr;
    if (val < 1)
        *statePtr = 1;
    else if (val < 2)
        *statePtr = 2;
}

void IN_RaiseStance(void)
{
    if (kb[25].active != 0 || kb[11].active != 0 )
        return;
    int *stance = &legacyHacks->cl_stance;
    if (*stance > 1)
        *stance = 1;
    else if (*stance > 0)
        *stance = 0;
}

void IN_ToggleCrouch(void)
{
    if (kb[25].active != 0 || kb[11].active != 0 )
        return;
    int *stance = &legacyHacks->cl_stance;
    *stance = (*stance != 1) ? 1 : 0;
}

void IN_ToggleProne(void)
{
    if (kb[25].active != 0 || kb[11].active != 0 )
        return;
    int *stance = &legacyHacks->cl_stance;
    *stance = (*stance != 2) ? 2 : 0;
}

void IN_GoProne(void)
{
    if (kb[25].active != 0 || kb[11].active != 0 )
        return;
    legacyHacks->cl_stance = 2;
}

void IN_GoCrouch(void)
{
    if (kb[25].active != 0 || kb[11].active != 0 )
        return;
    legacyHacks->cl_stance = 1;
}

void IN_GoStandDown(void)
{
    int *stance;

    IN_KeyDown(&kb[12]);
    stance = &legacyHacks->cl_stance;
    if (!*stance) {
        IN_KeyDown(&kb[10]);
        return;
    }

    if (kb[25].active || kb[11].active ) {
        return;
    }

    *stance = 0;
}

void IN_TalkDown(void)
{
    IN_KeyDown(&kb[27]);
}

void IN_TalkUp(void)
{
    IN_KeyUp(&kb[27]);
}

Bool IsTalking(void)
{
    return kb[27].active;
}

void CL_AdjustAngles(void)
{
    float speed;

#define KB_AT(index) (&kb[(index)])

    if (KB_AT(9)->active) {
        speed = (float)cls.frametime * cl_anglespeedkey->current.value * 0.001f;
    } else {
        speed = (float)cls.frametime * 0.001f;
    }

    if (!KB_AT(8)->active) {
        clients[0].viewangles[1] -= CL_KeyState(KB_AT(1)) * speed * cl_yawspeed->current.value;
        clients[0].viewangles[1] += CL_KeyState(KB_AT(0)) * speed * cl_yawspeed->current.value;
    }
    clients[0].viewangles[0] -= CL_KeyState(KB_AT(4)) * speed * cl_pitchspeed->current.value;
    clients[0].viewangles[0] += CL_KeyState(KB_AT(5)) * speed * cl_pitchspeed->current.value;

#undef KB_AT
}

void CL_StanceButtonUpdate(void)
{
    clientActive_t *cl = clients;

    if (cl->stanceHeld && (int)com_frameTime - cl->stanceTime >= cl_stanceHoldTime->current.integer) {
        if (cl->stancePosition == 2)
            legacyHacks->cl_stance = 0;
        else
            legacyHacks->cl_stance = 2;
        cl->stanceHeld = 0;
    }
}

void CL_KeyMove(usercmd_t *cmd)
{
    clientActive_t *cl;
    int *legacyStance;
    int side;
    int forward;

#define KB_AT(index) (&kb[(index)])

    cl = clients;

    (void)cl;

    if (KB_AT(25)->active || KB_AT(11)->active) {
        if (KB_AT(25)->active) {
            cmd->buttons |= 0x100;
            cmd->buttons &= ~0x200;
        } else {
            cmd->buttons |= 0x200;
            cmd->buttons &= ~0x100;
        }
        cmd->buttons |= 0x2000;
    } else {
        int clStance;
        CL_StanceButtonUpdate();
        clStance = legacyHacks->cl_stance;
        switch (clStance) {
        case 1:
            cmd->buttons |= 0x200;
            cmd->buttons &= ~0x100;
            break;
        case 2:
            cmd->buttons |= 0x100;
            cmd->buttons &= ~0x200;
            break;
        default:
            cmd->buttons &= ~0x300;
            break;
        }
        cmd->buttons &= ~0x2000;
    }

    if ((KB_AT(9)->active == (cl->usingAds == 0)) || PM_IsBinocularsADS(&cl->snap.ps)) {
        cmd->buttons |= 0x1000;
    } else {
        cmd->buttons &= ~0x1000;
    }

    side = 0;
    if (KB_AT(8)->active) {
        side += (int)(CL_KeyState(KB_AT(1)) * 127.0f);
        side += (int)(CL_KeyState(KB_AT(0)) * -127.0f);
    }

    side += (int)(CL_KeyState(KB_AT(7)) * 127.0f);
    side += (int)(CL_KeyState(KB_AT(6)) * -127.0f);

    forward = (int)(CL_KeyState(KB_AT(2)) * 127.0f);
    forward += (int)(CL_KeyState(KB_AT(3)) * -127.0f);

    cmd->forwardmove = ClampChar(forward);
    cmd->rightmove = ClampChar(side);

#if COD2_FEATURE_GAMEPAD

    CL_Gamepad_Move(cmd);
#endif

#undef KB_AT
}

// retail 0x407bd0, unreferenced (CL_CmdButtons inlines it); name unknown
void CL_CmdButton_407bd0(int *buttons, int index, int bit)
{
    if (kb[index].active || kb[index].wasPressed)
        *buttons |= bit;
    kb[index].wasPressed = 0;
}

void CL_MouseMove(usercmd_t *cmd)
{
    float mx, my;
    float accelSensitivity;
    float rate;
    float cap;
    float yaw, pitch;

    // allow mouse smoothing
    if (m_filter->current.enabled) {
        mx = (float)(clients->mouseDx[0] + clients->mouseDx[1]) * 0.5f;
        my = (float)(clients->mouseDy[0] + clients->mouseDy[1]) * 0.5f;
    } else {
        mx = (float)clients->mouseDx[clients->mouseIndex];
        my = (float)clients->mouseDy[clients->mouseIndex];
    }
    clients->mouseIndex ^= 1;
    clients->mouseDx[clients->mouseIndex] = 0;
    clients->mouseDy[clients->mouseIndex] = 0;

    if (frame_msec <= 0)
        return;

    rate = sqrt(mx * mx + my * my) / (float)frame_msec;
    accelSensitivity = rate * cl_mouseAccel->current.value + cl_sensitivity->current.value;
    accelSensitivity *= clients->cgameSensitivity;

    if (rate != 0.0f && cl_showMouseRate->current.enabled)
        Com_Printf("%f : %f\n", rate, accelSensitivity);

    if (clients->snap.ps.pm_flags & 0x8000)
        return;

    if (clients->snap.ps.eFlags & 0x300) {
        mx *= 2.5;
        my *= 2.0f;
    } else {
        mx *= accelSensitivity;
        my *= accelSensitivity;
    }

    if (!mx && !my)
        return;

    // add mouse X/Y movement to cmd
    if (kb[8].active) {
        cmd->rightmove = ClampChar(cmd->rightmove + (int)(mx * m_side->current.value));
    } else {
        yaw = mx * m_yaw->current.value;
        if (clients->cgameMaxYawSpeed != 0.0f) {
            cap = (float)frame_msec * clients->cgameMaxYawSpeed * 0.001f;
            yaw = I_fmax(-cap, I_fmin(cap, yaw));
        }
        clients->viewangles[1] -= yaw;
    }

    if ((kb[13].active || cl_freelook->current.enabled) && !kb[8].active) {
        pitch = my * m_pitch->current.value;
        if (clients->cgameMaxPitchSpeed != 0.0f) {
            cap = (float)frame_msec * clients->cgameMaxPitchSpeed * 0.001f;
            pitch = I_fmax(-cap, I_fmin(cap, pitch));
        }
        clients->viewangles[0] += pitch;
    } else {
        cmd->forwardmove = ClampChar(cmd->forwardmove - (int)(my * m_forward->current.value));
    }
}

void CL_CmdButtons(usercmd_t *cmd)
{
    clientActive_t *cl_ = clients;
#define KB_AT(index) (&kb[(index)])
#define CONSUME(index, bit)                          \
    do {                                             \
        kbutton_t *b__ = KB_AT(index);               \
        if (b__->active || b__->wasPressed)          \
            cmd->buttons |= (bit);                   \
        b__->wasPressed = 0;                         \
    } while (0)

    CONSUME(14, 0x1);
    CONSUME(15, 0x8000);
    CONSUME(16, 0x10000);
    CONSUME(17, 0x20000);
    CONSUME(18, 0x4000);
    CONSUME(19, 0x4);
    CONSUME(20, 0x8);
    CONSUME(21, 0x10);
    CONSUME(22, 0x20);
    CONSUME(23, 0x40);
    CONSUME(24, 0x80);
    CONSUME(25, 0x100);
    CONSUME(26, 0x200);
    CONSUME(10, 0x400);

    if (cl_->keyCatchers && !cl_bypassMouseInput->current.enabled) {
        cmd->buttons |= 0x40000;
    }

    if (cl_->snap.ps.pm_type == 2 || cl_->snap.ps.pm_type == 3 || cl_->snap.ps.pm_type == 4) {
        CONSUME(12, 0x400);
    }

    if (cl_->cgameInShellshock) {
        cmd->buttons |= 0x800;
    }

#undef CONSUME
#undef KB_AT
}

// retail 0x408180
static void CL_FinishMove(usercmd_t *cmd)
{
    clientActive_t *cl;
    int serverTime;
    int i;

    cl = clients;
    cmd->weapon = (byte)cl->cgameUserCmdValue;
    cmd->offHandIndex = (byte)cl->cgameUserHoldableValue;

    serverTime = cl->serverTime;
    if (serverTime - cl->snap.serverTime > 5000)
        serverTime = cl->snap.serverTime + 5000;
    cmd->serverTime = serverTime;

    for (i = 0; i < 3; ++i)
        cmd->angles[i] = ((int)((clients[0].viewangles[i] + clients[0].cgameKickAngles[i]) * 182.04444885253906f)) & 0xffff;
}

usercmd_t CL_CreateCmd(void)
{
    clientActive_t *cl;
    usercmd_t cmd;
    vec3_t oldAngles;

    cl = clients;
    oldAngles[0] = cl->viewangles[0];
    oldAngles[1] = cl->viewangles[1];
    oldAngles[2] = cl->viewangles[2];

    CL_AdjustAngles();

    memset(&cmd, 0, sizeof(cmd));

    CL_CmdButtons(&cmd);
    CL_KeyMove(&cmd);
    CL_MouseMove(&cmd);

    if (cl->viewangles[0] - oldAngles[0] > 90.0f)
        cl->viewangles[0] = oldAngles[0] + 90.0f;
    else if (oldAngles[0] - cl->viewangles[0] > 90.0f)
        cl->viewangles[0] = oldAngles[0] - 90.0f;

    CL_FinishMove(&cmd);

    return cmd;
}

void CL_CreateNewCommands(void)
{
    clientConnection_t *conn;
    clientActive_t *cl;

    conn = &::clc;
    if (conn->state >= CA_PRIMED) {
        int cmdNum;

        cl = clients;
        cmdNum = ++cl->cmdNumber & 127;
        cl->cmds[cmdNum] = CL_CreateCmd();
    }
}

qboolean CL_ReadyToSendPacket(void)
{
    clientConnection_t *conn;
    clientActive_t *cl;
    outPacket_t *outPacket;
    int realtime;

    conn = &::clc;
    if (conn->demoplaying || conn->state == CA_CINEMATIC || conn->state == CA_LOGO) {
        return 0;
    }

    realtime = cls.realtime;

    if (conn->downloadTempName[0] && realtime - conn->lastPacketSentTime < 50) {
        return 0;
    }

    if (conn->state != CA_ACTIVE && conn->state != CA_PRIMED && !conn->downloadTempName[0]
        && realtime - conn->lastPacketSentTime < 1000) {
        return 0;
    }

    if (conn->serverAddress.type == NA_LOOPBACK) {
        return 1;
    }

    if (Sys_IsLANAddress(conn->serverAddress)) {
        return 1;
    }

    cl = clients;
    outPacket = &cl->outPackets[(conn->netchan.outgoingSequence - 1) & 31];
    return cls.realtime - outPacket->p_realtime >= 1000 / cl_maxpackets->current.integer;
}

void CL_WriteVoicePacket(void)
{
    clientConnection_t *conn;
    clientActive_t *cl;
    msg_t msg;
    byte data[MAX_MSGLEN];
    int voicePacket;

    conn = &::clc;
    if (conn->demoplaying || (conn->state != CA_ACTIVE && conn->state != CA_LOADING)) {
        return;
    }

    MSG_Init(&msg, data, sizeof(data));
    MSG_WriteString(&msg, (const char *)"v");
    MSG_WriteShort(&msg, conn->qport);

    cl = clients;
    MSG_WriteByte(&msg, (byte)cl->voicePacketCount);

    for (voicePacket = 0; voicePacket < cl->voicePacketCount; ++voicePacket) {
        MSG_WriteByte(&msg, cl->voicePackets[voicePacket].dataSize);
        MSG_WriteData(&msg, cl->voicePackets[voicePacket].data, cl->voicePackets[voicePacket].dataSize);
    }

    if ((cl_showSend)->current.enabled) {
        Com_Printf((const char *)"voice: %i\n", msg.cursize);
    }

    conn = &::clc;
    NET_OutOfBandVoiceData(conn->netchan.sock, conn->serverAddress, msg.data, msg.cursize);
}

void CL_WritePacket(void)
{
    msg_t buf;
    byte data[MAX_MSGLEN];
    byte dataCompressed[MAX_MSGLEN];
    int i, j;
    usercmd_t *cmd, *oldcmd;
    usercmd_t nullcmd;
    int packetNum;
    int oldPacketNum;
    int count, key;
    int compressedSize;

    if (::clc.demoplaying || ::clc.state == 1 || ::clc.state == 2)
        return;

    MSG_SetDefaultUserCmd(&clients[0].snap.ps, &nullcmd);
    oldcmd = &nullcmd;

    MSG_Init(&buf, data, sizeof(data));
    MSG_WriteByte(&buf, clients[0].serverId);
    MSG_WriteLong(&buf, ::clc.serverMessageSequence);
    MSG_WriteLong(&buf, ::clc.serverCommandSequence);

    for (i = ::clc.reliableAcknowledge + 1; i <= ::clc.reliableSequence; i++) {
        MSG_WriteBits(&buf, 2, 3);
        MSG_WriteLong(&buf, i);
        MSG_WriteString(&buf, (const char *)&::clc.reliableCommands[i & 0x7f]);
    }

    oldPacketNum = (::clc.netchan.outgoingSequence - cl_packetdup->current.integer - 1) & 0x1f;
    count = clients[0].cmdNumber - clients[0].outPackets[oldPacketNum].p_cmdNumber;
    if (count > 0x20) {
        count = 0x20;
        Com_Printf((const char *)"MAX_PACKET_USERCMDS\n");
    }

    if (count >= 1) {
        if (cl_showSend->current.enabled)
            Com_Printf((const char *)"(%i)", count);

        if (cl_nodelta->current.enabled || !clients[0].snap.valid || ::clc.demowaiting
            || ::clc.serverMessageSequence != clients[0].snap.messageNum)
            MSG_WriteBits(&buf, 1, 3);
        else
            MSG_WriteBits(&buf, 0, 3);

        MSG_WriteByte(&buf, count);

        key = ::clc.checksumFeed;
        key ^= ::clc.serverMessageSequence;
        key ^= Com_HashKey(::clc.serverCommands[::clc.serverCommandSequence & 0x7f], 0x20);

        for (i = 0; i < count; i++) {
            j = (clients[0].cmdNumber - count + i + 1) & 0x7f;
            cmd = &clients[0].cmds[j];
            MSG_WriteDeltaUsercmdKey(&buf, key, oldcmd, cmd);
            oldcmd = cmd;
        }
    }

    MSG_WriteBits(&buf, 3, 3);

    memcpy(dataCompressed, buf.data, 9);
    compressedSize = MSG_WriteBitsCompress(buf.data + 9, dataCompressed + 9, buf.cursize - 9) + 9;

    packetNum = ::clc.netchan.outgoingSequence & 0x1f;
    clients[0].outPackets[packetNum].p_realtime = cls.realtime;
    clients[0].outPackets[packetNum].p_serverTime = oldcmd->serverTime;
    clients[0].outPackets[packetNum].p_cmdNumber = clients[0].cmdNumber;
    ::clc.lastPacketSentTime = cls.realtime;

    if (cl_showSend->current.enabled)
        Com_Printf((const char *)"%i ", compressedSize);

    CL_Netchan_Transmit(&::clc.netchan, dataCompressed, compressedSize);

    while (::clc.netchan.unsentFragments)
        CL_Netchan_TransmitNextFragment(&::clc.netchan);
}

void CL_SendCmd(void)
{
    int state = (&::clc)->state;
    if (state < CA_CONNECTED || state == CA_ACTIVE)
        return;
    CL_SendCmdInternal();
}

void CL_Input(void)
{
    if ((&::clc)->state != 8)
        return;
    CL_SyncGpu();
    IN_Frame();
    CL_SendCmdInternal();
}

void CL_InitInput(void)
{
    Cmd_AddCommand("centerview", IN_CenterView);
    Cmd_AddCommand("+moveup", IN_UpDown);
    Cmd_AddCommand("-moveup", IN_UpUp);
    Cmd_AddCommand("+movedown", IN_DownDown);
    Cmd_AddCommand("-movedown", IN_DownUp);
    Cmd_AddCommand("+left", IN_LeftDown);
    Cmd_AddCommand("-left", IN_LeftUp);
    Cmd_AddCommand("+right", IN_RightDown);
    Cmd_AddCommand("-right", IN_RightUp);
    Cmd_AddCommand("+forward", IN_ForwardDown);
    Cmd_AddCommand("-forward", IN_ForwardUp);
    Cmd_AddCommand("+back", IN_BackDown);
    Cmd_AddCommand("-back", IN_BackUp);
    Cmd_AddCommand("+lookup", IN_LookupDown);
    Cmd_AddCommand("-lookup", IN_LookupUp);
    Cmd_AddCommand("+lookdown", IN_LookdownDown);
    Cmd_AddCommand("-lookdown", IN_LookdownUp);
    Cmd_AddCommand("+strafe", IN_StrafeDown);
    Cmd_AddCommand("-strafe", IN_StrafeUp);
    Cmd_AddCommand("+moveleft", IN_MoveleftDown);
    Cmd_AddCommand("-moveleft", IN_MoveleftUp);
    Cmd_AddCommand("+moveright", IN_MoverightDown);
    Cmd_AddCommand("-moveright", IN_MoverightUp);
    Cmd_AddCommand("+speed", IN_SpeedDown);
    Cmd_AddCommand("-speed", IN_SpeedUp);
    Cmd_AddCommand("+attack", IN_Attack_Down);
    Cmd_AddCommand("-attack", IN_Attack_Up);
    Cmd_AddCommand("+melee", IN_Melee_Down);
    Cmd_AddCommand("-melee", IN_Melee_Up);
    Cmd_AddCommand("+holdbreath", IN_Breath_Down);
    Cmd_AddCommand("-holdbreath", IN_Breath_Up);
    Cmd_AddCommand("+melee_breath", IN_MeleeBreath_Down);
    Cmd_AddCommand("-melee_breath", IN_MeleeBreath_Up);
    Cmd_AddCommand("+frag", IN_Frag_Down);
    Cmd_AddCommand("-frag", IN_Frag_Up);
    Cmd_AddCommand("+smoke", IN_Smoke_Down);
    Cmd_AddCommand("-smoke", IN_Smoke_Up);
    Cmd_AddCommand("+binoculars", IN_Binoculars_Down);
    Cmd_AddCommand("-binoculars", IN_Binoculars_Up);
    Cmd_AddCommand("+breath_binoculars", IN_BreathBinoculars_Down);
    Cmd_AddCommand("-breath_binoculars", IN_BreathBinoculars_Up);
    Cmd_AddCommand("+activate", IN_Activate_Down);
    Cmd_AddCommand("-activate", IN_Activate_Up);
    Cmd_AddCommand("+reload", IN_Reload_Down);
    Cmd_AddCommand("-reload", IN_Reload_Up);
    Cmd_AddCommand("+usereload", IN_UseReload_Down);
    Cmd_AddCommand("-usereload", IN_UseReload_Up);
    Cmd_AddCommand("+leanleft", IN_LeanLeft_Down);
    Cmd_AddCommand("-leanleft", IN_LeanLeft_Up);
    Cmd_AddCommand("+leanright", IN_LeanRight_Down);
    Cmd_AddCommand("-leanright", IN_LeanRight_Up);
    Cmd_AddCommand("+prone", IN_Prone_Down);
    Cmd_AddCommand("-prone", IN_Prone_Up);
    Cmd_AddCommand("+stance", IN_Stance_Down);
    Cmd_AddCommand("-stance", IN_Stance_Up);
    Cmd_AddCommand("+mlook", IN_MLookDown);
    Cmd_AddCommand("-mlook", IN_MLookUp);
    Cmd_AddCommand("toggleads", IN_ToggleADS);
    Cmd_AddCommand("leaveads", IN_LeaveADS);
    Cmd_AddCommand("lowerstance", IN_LowerStance);
    Cmd_AddCommand("raisestance", IN_RaiseStance);
    Cmd_AddCommand("togglecrouch", IN_ToggleCrouch);
    Cmd_AddCommand("toggleprone", IN_ToggleProne);
    Cmd_AddCommand("goprone", IN_GoProne);
    Cmd_AddCommand("gocrouch", IN_GoCrouch);
    Cmd_AddCommand("+gostand", IN_GoStandDown);
    Cmd_AddCommand("-gostand", IN_GoStandUp);
    Cmd_AddCommand("+talk", IN_TalkDown);
    Cmd_AddCommand("-talk", IN_TalkUp);

    cl_analog_attack_threshold = Dvar_RegisterFloat("cl_analog_attack_threshold", 0.8f, 0.0001f, 1.0f, 0x1000);
    cl_stanceHoldTime = Dvar_RegisterInt("cl_stanceHoldTime", 300, 0, 1000, 0x1000);
    {
        const dvar_t *nodelta = Dvar_RegisterBool("cl_nodelta", 0, 0x1000);
        cl_nodelta = nodelta;
    }

#if COD2_FEATURE_GAMEPAD
    CL_Gamepad_Init();
#endif
}

void CL_ShutdownInput(void)
{
    Cmd_RemoveCommand("centerview");
    Cmd_RemoveCommand("+moveup");
    Cmd_RemoveCommand("-moveup");
    Cmd_RemoveCommand("+movedown");
    Cmd_RemoveCommand("-movedown");
    Cmd_RemoveCommand("+left");
    Cmd_RemoveCommand("-left");
    Cmd_RemoveCommand("+right");
    Cmd_RemoveCommand("-right");
    Cmd_RemoveCommand("+forward");
    Cmd_RemoveCommand("-forward");
    Cmd_RemoveCommand("+back");
    Cmd_RemoveCommand("-back");
    Cmd_RemoveCommand("+lookup");
    Cmd_RemoveCommand("-lookup");
    Cmd_RemoveCommand("+lookdown");
    Cmd_RemoveCommand("-lookdown");
    Cmd_RemoveCommand("+strafe");
    Cmd_RemoveCommand("-strafe");
    Cmd_RemoveCommand("+moveleft");
    Cmd_RemoveCommand("-moveleft");
    Cmd_RemoveCommand("+moveright");
    Cmd_RemoveCommand("-moveright");
    Cmd_RemoveCommand("+speed");
    Cmd_RemoveCommand("-speed");
    Cmd_RemoveCommand("+attack");
    Cmd_RemoveCommand("-attack");
    Cmd_RemoveCommand("+melee");
    Cmd_RemoveCommand("-melee");
    Cmd_RemoveCommand("+holdbreath");
    Cmd_RemoveCommand("-holdbreath");
    Cmd_RemoveCommand("+melee_breath");
    Cmd_RemoveCommand("-melee_breath");
    Cmd_RemoveCommand("+frag");
    Cmd_RemoveCommand("-frag");
    Cmd_RemoveCommand("+smoke");
    Cmd_RemoveCommand("-smoke");
    Cmd_RemoveCommand("+binoculars");
    Cmd_RemoveCommand("-binoculars");
    Cmd_RemoveCommand("+breath_binoculars");
    Cmd_RemoveCommand("-breath_binoculars");
    Cmd_RemoveCommand("+activate");
    Cmd_RemoveCommand("-activate");
    Cmd_RemoveCommand("+reload");
    Cmd_RemoveCommand("-reload");
    Cmd_RemoveCommand("+usereload");
    Cmd_RemoveCommand("-usereload");
    Cmd_RemoveCommand("+leanleft");
    Cmd_RemoveCommand("-leanleft");
    Cmd_RemoveCommand("+leanright");
    Cmd_RemoveCommand("-leanright");
    Cmd_RemoveCommand("+prone");
    Cmd_RemoveCommand("-prone");
    Cmd_RemoveCommand("+stance");
    Cmd_RemoveCommand("-stance");
    Cmd_RemoveCommand("+mlook");
    Cmd_RemoveCommand("-mlook");
    Cmd_RemoveCommand("toggleads");
    Cmd_RemoveCommand("leaveads");
    Cmd_RemoveCommand("lowerstance");
    Cmd_RemoveCommand("raisestance");
    Cmd_RemoveCommand("togglecrouch");
    Cmd_RemoveCommand("toggleprone");
    Cmd_RemoveCommand("goprone");
    Cmd_RemoveCommand("gocrouch");
    Cmd_RemoveCommand("+gostand");
    Cmd_RemoveCommand("-gostand");
    Cmd_RemoveCommand("+talk");
    Cmd_RemoveCommand("-talk");

#if COD2_FEATURE_GAMEPAD
    CL_Gamepad_Shutdown();
#endif
}

// retail 0x409170, unreferenced; name unknown
void IN_ClearFirstKey_409170(void)
{
    kb[0].down[0] = 0;
}

void IN_UpUp(void)
{
    IN_KeyUp(&kb[12]);
    IN_KeyUp(&kb[10]);
}
void IN_MeleeBreath_Down(void)
{
    IN_KeyDown(&kb[19]);
    IN_KeyDown(&kb[15]);
}
void IN_MeleeBreath_Up(void)
{
    IN_KeyUp(&kb[19]);
    IN_KeyUp(&kb[15]);
}
void IN_BreathBinoculars_Down(void)
{
    IN_KeyDown(&kb[15]);
    IN_KeyDown(&kb[18]);
}
void IN_BreathBinoculars_Up(void)
{
    IN_KeyUp(&kb[15]);
    IN_KeyUp(&kb[18]);
}
void IN_GoStandUp(void)
{
    IN_KeyUp(&kb[12]);
    IN_KeyUp(&kb[10]);
}
void CL_MouseEvent(int dx, int dy)
{
    if ((clients->keyCatchers & 8) && !cl_bypassMouseInput->current.enabled) {
        UI_MouseEvent(dx, dy);
    } else {
        clients->mouseDx[clients->mouseIndex] += dx;
        clients->mouseDy[clients->mouseIndex] += dy;
    }
}
void IN_DownDown(void)
{
    IN_KeyDown(&kb[11]);
    if (kb[25].active || kb[11].active )
        legacyHacks->cl_stanceTemp = 1;
    else
        legacyHacks->cl_stanceTemp = 0;
}
void IN_DownUp(void)
{
    IN_KeyUp(&kb[11]);
    if (kb[25].active || kb[11].active )
        legacyHacks->cl_stanceTemp = 1;
    else
        legacyHacks->cl_stanceTemp = 0;
}
void IN_Prone_Down(void)
{
    IN_KeyDown(&kb[25]);
    if (kb[25].active || kb[11].active )
        legacyHacks->cl_stanceTemp = 1;
    else
        legacyHacks->cl_stanceTemp = 0;
}
void IN_Prone_Up(void)
{
    IN_KeyUp(&kb[25]);
    if (kb[25].active || kb[11].active )
        legacyHacks->cl_stanceTemp = 1;
    else
        legacyHacks->cl_stanceTemp = 0;
}
void CL_SendCmdInternal(void)
{
    CL_CreateNewCommands();

    if (!CL_ReadyToSendPacket()) {
        if (cl_showSend->current.enabled) {
            Com_Printf((const char *)". ");
        }
        return;
    }

    CL_WritePacket();
}
