#include "common_types.h"
#include "imports.h"
#include "headers/PC/cgame_mp/cg_local.h"
#include <string.h>

extern float floorf(float x);

extern const dvar_t *cg_shock_mouse;
extern const dvar_t *cg_shock_mouse_fadeTime;
extern const dvar_t *cg_shock_mouse_maxpitchspeed;
extern const dvar_t *cg_shock_mouse_maxyawspeed;
extern const dvar_t *cg_shock_mouse_sensitivityscale;
extern const dvar_t *cg_shock_screenBlendFadeTime;
extern const dvar_t *cg_shock_screenBlendTime;
extern const dvar_t *cg_shock_sound;
extern const dvar_t *cg_shock_soundDryLevel;
extern const dvar_t *cg_shock_soundFadeInTime;
extern const dvar_t *cg_shock_soundFadeOutTime;
extern const dvar_t *cg_shock_soundLoopEndDelay;
extern const dvar_t *cg_shock_soundLoopFadeTime;
extern const dvar_t *cg_shock_soundModEndDelay;
extern const dvar_t *cg_shock_soundRoomType;
extern const dvar_t *cg_shock_soundWetLevel;
extern const dvar_t *cg_shock_viewKickPeriod;
extern const dvar_t *cg_shock_viewKickRadius;
extern const dvar_t *cg_shock_volume_announcer;
extern const dvar_t *cg_shock_volume_auto;
extern const dvar_t *cg_shock_volume_auto2d;
extern const dvar_t *cg_shock_volume_body;
extern const dvar_t *cg_shock_volume_item;
extern const dvar_t *cg_shock_volume_local;
extern const dvar_t *cg_shock_volume_menu;
extern const dvar_t *cg_shock_volume_music;
extern const dvar_t *cg_shock_volume_shellshock;
extern const dvar_t *cg_shock_volume_voice;
extern const dvar_t *cg_shock_volume_weapon;

extern const dvar_t *cg_shock_viewKickFadeTime;


extern vec_t Vec3Normalize( vec3_t v );
extern void Vec3Cross(const vec3_t v0, const vec3_t v1, vec3_t cross);
extern void AxisCopy( const vec3_t in[3], vec3_t out[3] );
extern void MatrixMultiply(const float in1[3][3], const float in2[3][3], float out[3][3]);
extern qboolean Com_SaveDvarsToBuffer(const char **dvarnames, int numDvars, char *buffer, int bufsize);
extern qboolean Com_LoadDvarsFromBuffer(const char **dvarnames, int numDvars, const char *buffer, const char *filename);
extern char *va( const char *format, ... );
extern int FS_FOpenFileByMode(const char *name, int *fh, fsMode_t mode);
extern int FS_Write(const void *buf, int len, int fh);
extern int FS_Read(void *buf, int len, int fh);
extern void FS_FCloseFile(fileHandle_t fh);
extern "C" void *Z_MallocInternal( int size );
extern "C" void Z_FreeInternal( void *ptr );
extern void Com_Printf(const char *fmt, ...);
extern void CL_SaveScreen(void);
extern void CL_BlendSavedScreen(int blend);
extern const char *Dvar_EnumToString(const dvar_t *dvar);
extern void SND_SetChannelVolumes(int priority, const float *channelvolume, int fademsec);
extern void SND_SetEnvironmentEffects(int type, const char *name, float drylevel, float wetlevel, int flags);
extern void SND_DeactivateChannelVolumes(int type, int flags);
extern void SND_DeactivateEnvironmentEffects(int type, int flags);
extern snd_alias_t * CL_PickSoundAlias(const char *aliasname);
extern int SND_PlayBlendedSoundAliases(const snd_alias_t *pAlias0, const snd_alias_t *pAlias1, float fLerp, int entnum, const vec_t *org, int timeshift, snd_alias_system_t system);
extern int SND_PlaySoundAlias(const snd_alias_t *pAlias, int entnum, const vec_t *org, int timeshift, snd_alias_system_t system);
extern void CL_CapTurnRate(float min_rate, float max_rate);
extern void CL_SetUserCmdInShellshock(int inShellshock);

void CG_PerturbCamera(void);
qboolean CG_SaveShellShockDvars(const char *name);
qboolean CG_LoadShellShockDvars(const char *name);
qboolean CG_DrawShellShockSavedScreenBlend(const shellshock_parms_t *parms, int start, int duration);
void CG_SetShellShockParmsFromDvars(shellshock_parms_t *parms);
void CG_UpdateShellShock(const shellshock_parms_t *parms, int start, int duration);

/* migrated from the ILP32 data blob to typed C (x64 port Stage 2). */
static const char *cg_shock_dvar_names[29] = {
    "cg_shock_screenBlendTime",
    "cg_shock_screenBlendFadeTime",
    "cg_shock_viewKickPeriod",
    "cg_shock_viewKickRadius",
    "cg_shock_sound",
    "cg_shock_soundFadeInTime",
    "cg_shock_soundFadeOutTime",
    "cg_shock_soundLoopFadeTime",
    "cg_shock_soundLoopEndDelay",
    "cg_shock_soundRoomType",
    "cg_shock_soundDryLevel",
    "cg_shock_soundWetLevel",
    "cg_shock_soundModEndDelay",
    "cg_shock_volume_auto",
    "cg_shock_volume_auto2d",
    "cg_shock_volume_menu",
    "cg_shock_volume_weapon",
    "cg_shock_volume_voice",
    "cg_shock_volume_item",
    "cg_shock_volume_body",
    "cg_shock_volume_local",
    "cg_shock_volume_music",
    "cg_shock_volume_announcer",
    "cg_shock_volume_shellshock",
    "cg_shock_mouse",
    "cg_shock_mouse_maxpitchspeed",
    "cg_shock_mouse_maxyawspeed",
    "cg_shock_mouse_sensitivityscale",
    "cg_shock_mouse_fadeTime",
};
static vec2_t cg_perturbations[131];
void CG_PerturbCamera(void);
qboolean CG_SaveShellShockDvars(const char *name);
qboolean CG_LoadShellShockDvars(const char *name);
qboolean CG_DrawShellShockSavedScreenBlend(const shellshock_parms_t *parms, int start, int duration);
void CG_SetShellShockParmsFromDvars(shellshock_parms_t *parms);
void CG_UpdateShellShock(const shellshock_parms_t *parms, int start, int duration);
extern const vec3_t vec3_origin;
extern float randomf();
extern float crandom();

static inline float dvar_get_float(const dvar_t *dvar)
{
    return dvar->current.value;
}

static inline int dvar_get_int(const dvar_t *dvar)
{
    return dvar->current.integer;
}

static inline int dvar_get_bool(const dvar_t *dvar)
{
    return (int)dvar->current.enabled;
}

static inline const dvar_t *dvar_get_ptr(const dvar_t *dvar)
{
    return dvar;
}

static inline int float_seconds_to_ms(float val)
{
    return I_fround(val * 1000.0f);
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

// Name from CoD4; original CoD2 name unknown.
static float CubicInterpolate(float t, float x0, float x1, float x2, float x3)
{
    float a;
    float b;
    float c;

    a = x3 - x2 + x1 - x0;
    b = x0 - x1 - a;
    c = x2 - x0;
    return ((t * a + b) * t + c) * t + x1;
}

// Original name unknown.
static void CG_RandomPerturbation(const float *prev, float *out, float minDist)
{
    float angle;
    float radius;
    float s;
    float c;
    float delta[2];
    float minDistSq;

    minDistSq = minDist * minDist;
    do
    {
        angle = crandom() * 3.1415927f;
        FastSinCos(angle, &s, &c);
        radius = randomf();
        out[0] = radius * c;
        out[1] = radius * s;
        delta[0] = out[0] - prev[0];
        delta[1] = out[1] - prev[1];
    } while (delta[1] * delta[1] + delta[0] * delta[0] < minDistSq);
}

// Original name unknown.
void CG_PrintPerturbations(void)
{
    vec2_t table[128];
    unsigned int i;

    CG_RandomPerturbation(vec3_origin, table[0], 0.5f);
    for (i = 1; i < 128; i++)
        CG_RandomPerturbation(table[i - 1], table[i], 0.5f);

    for (i = 0; i < 128; i++)
        Com_Printf("\t{%f, %f},\n", table[i][0], table[i][1]);
}

void CG_PerturbCamera(void)
{
    float rot[3][3];
    float axis[3][3];

    if (cg->shellshock.viewDelta[0] != 0.0f || cg->shellshock.viewDelta[1] != 0.0f) {
        rot[0][0] = 1.0f;
        rot[0][1] = cg->shellshock.viewDelta[0];
        rot[0][2] = cg->shellshock.viewDelta[1];
        rot[2][0] = 0.0f;
        rot[2][1] = 0.0f;
        rot[2][2] = 1.0f;
        Vec3Normalize(rot[0]);
        Vec3Cross(rot[2], rot[0], rot[1]);
        Vec3Normalize(rot[1]);
        Vec3Cross(rot[0], rot[1], rot[2]);
        AxisCopy(cg->refdef.viewaxis, axis);
        MatrixMultiply(rot, axis, cg->refdef.viewaxis);
    }
}

qboolean CG_DrawShellShockSavedScreenBlend(const shellshock_parms_t *parms, int start, int duration)
{
    int dt;
    int screenBlendTime;

    if (start && duration > 0) {
        dt = duration + start - cg->time;
        if (dt > 0) {
            screenBlendTime = parms->screenBlend.effectTime;
            if (dt < parms->screenBlend.fadeTime)
                screenBlendTime = I_fround((float)dt / (float)parms->screenBlend.fadeTime * (float)screenBlendTime);
            if (cg->shellshock.hasSavedScreen)
                CL_BlendSavedScreen(screenBlendTime);
            CL_SaveScreen();
            cg->shellshock.hasSavedScreen = 1;
            return 1;
        } else {
            cg->shellshock.hasSavedScreen = 0;
            return 0;
        }
    } else {
        cg->shellshock.hasSavedScreen = 0;
        return 0;
    }
}

qboolean CG_SaveShellShockDvars(const char *name)
{
    char filebuf[0x10000];
    int fh;
    const char *path;

    if (!Com_SaveDvarsToBuffer(cg_shock_dvar_names, 29, filebuf, 0x10000)) {
        return 0;
    }

    path = va("shock/%s.shock", name);

    if (FS_FOpenFileByMode(path, &fh, FS_WRITE) < 0) {
        return 0;
    }

    FS_Write(filebuf, (int)strlen(filebuf), fh);

    FS_FCloseFile(fh);
    return 1;
}

qboolean CG_LoadShellShockDvars(const char *name)
{
    const char *fullpath;
    int fh;
    int filesize;
    char *buf;
    qboolean result;

    fullpath = va("shock/%s.shock", name);

    filesize = FS_FOpenFileByMode(fullpath, &fh, FS_READ);
    if (filesize < 0) {

        Com_Printf("^1couldn't open '%s'.\n", fullpath);

        filesize = FS_FOpenFileByMode("shock/default.shock", &fh, FS_READ);
        if (filesize < 0) {

            Com_Printf("^1couldn't open 'shock/default.shock'.\n");
            filesize = 0;
            return filesize;
        }
    }

    buf = (char *)Z_MallocInternal(filesize + 1);

    FS_Read(buf, filesize, fh);

    buf[filesize] = '\0';

    FS_FCloseFile(fh);

    result = Com_LoadDvarsFromBuffer(cg_shock_dvar_names, 29, buf, fullpath);

    Z_FreeInternal(buf);

    return result;
}

void CG_SetShellShockParmsFromDvars(shellshock_parms_t *parms)
{
    float val;
    float epsilon;

    parms->screenBlend.fadeTime = float_seconds_to_ms(dvar_get_float(cg_shock_screenBlendFadeTime));

    parms->screenBlend.effectTime = float_seconds_to_ms(dvar_get_float(cg_shock_screenBlendTime));

    parms->view.fadeTime = 3000;

    val = dvar_get_float(cg_shock_viewKickPeriod);
    epsilon = 0.001f;
    if (epsilon - val >= 0.0f)
        val = epsilon;
    parms->view.kickRate = epsilon / val;

    parms->view.kickRadius = dvar_get_float(cg_shock_viewKickRadius);

    parms->sound.use = dvar_get_bool(cg_shock_sound);

    parms->sound.fadeInTime = float_seconds_to_ms(dvar_get_float(cg_shock_soundFadeInTime));

    parms->sound.fadeOutTime = float_seconds_to_ms(dvar_get_float(cg_shock_soundFadeOutTime));

    parms->sound.loopFadeTime = float_seconds_to_ms(dvar_get_float(cg_shock_soundLoopFadeTime));

    parms->sound.loopEndDelay = float_seconds_to_ms(dvar_get_float(cg_shock_soundLoopEndDelay));

    strncpy(parms->sound.roomtype, Dvar_EnumToString(dvar_get_ptr(cg_shock_soundRoomType)), 0xf);
    parms->sound.roomtype[15] = 0;

    *(int *)&parms->sound.drylevel = dvar_get_int(cg_shock_soundDryLevel);

    *(int *)&parms->sound.wetlevel = dvar_get_int(cg_shock_soundWetLevel);

    parms->sound.modEndDelay = float_seconds_to_ms(dvar_get_float(cg_shock_soundModEndDelay));

    val = dvar_get_float(cg_shock_volume_auto);
    if (0.0f <= -val)
        val = 0.0f;
    if (0.0f <= val - 1.0f)
        val = 1.0f;
    parms->sound.channelvolume[0] = val;

    val = dvar_get_float(cg_shock_volume_auto2d);
    if (0.0f <= -val)
        val = 0.0f;
    if (0.0f <= val - 1.0f)
        val = 1.0f;
    parms->sound.channelvolume[1] = val;

    val = dvar_get_float(cg_shock_volume_menu);
    if (0.0f <= -val)
        val = 0.0f;
    if (0.0f <= val - 1.0f)
        val = 1.0f;
    parms->sound.channelvolume[2] = val;

    val = dvar_get_float(cg_shock_volume_weapon);
    if (0.0f <= -val)
        val = 0.0f;
    if (0.0f <= val - 1.0f)
        val = 1.0f;
    parms->sound.channelvolume[5] = val;

    val = dvar_get_float(cg_shock_volume_voice);
    if (0.0f <= -val)
        val = 0.0f;
    if (0.0f <= val - 1.0f)
        val = 1.0f;
    parms->sound.channelvolume[6] = val;

    val = dvar_get_float(cg_shock_volume_item);
    if (0.0f <= -val)
        val = 0.0f;
    if (0.0f <= val - 1.0f)
        val = 1.0f;
    parms->sound.channelvolume[4] = val;

    val = dvar_get_float(cg_shock_volume_body);
    if (0.0f <= -val)
        val = 0.0f;
    if (0.0f <= val - 1.0f)
        val = 1.0f;
    parms->sound.channelvolume[3] = val;

    val = dvar_get_float(cg_shock_volume_local);
    if (0.0f <= -val)
        val = 0.0f;
    if (0.0f <= val - 1.0f)
        val = 1.0f;
    parms->sound.channelvolume[7] = val;

    val = dvar_get_float(cg_shock_volume_music);
    if (0.0f <= -val)
        val = 0.0f;
    if (0.0f <= val - 1.0f)
        val = 1.0f;
    parms->sound.channelvolume[8] = val;

    val = dvar_get_float(cg_shock_volume_announcer);
    if (0.0f <= -val)
        val = 0.0f;
    if (0.0f <= val - 1.0f)
        val = 1.0f;
    parms->sound.channelvolume[9] = val;

    val = dvar_get_float(cg_shock_volume_shellshock);
    if (0.0f <= -val)
        val = 0.0f;
    if (0.0f <= val - 1.0f)
        val = 1.0f;
    parms->sound.channelvolume[10] = val;

    parms->mouse.use = dvar_get_bool(cg_shock_mouse);

    parms->mouse.fadeTime = float_seconds_to_ms(dvar_get_float(cg_shock_mouse_fadeTime));

    *(int *)&parms->mouse.maxPitchSpeed = dvar_get_int(cg_shock_mouse_maxpitchspeed);

    *(int *)&parms->mouse.maxYawSpeed = dvar_get_int(cg_shock_mouse_maxyawspeed);

    *(int *)&parms->mouse.sensitivity = dvar_get_int(cg_shock_mouse_sensitivityscale);
}

// Name from CoD4; original CoD2 name unknown.
static void EndShellShockSound(void)
{
    SND_DeactivateChannelVolumes(3, 0);
    SND_DeactivateEnvironmentEffects(2, 0);

    if (cg->shellshock.loopEndTime != 0) {
        cg->shellshock.loopEndTime = 0;

        {
            void *alias = CL_PickSoundAlias("shellshock_end_abort");
            SND_PlaySoundAlias((const snd_alias_t *)alias, 0x3ff, vec3_origin, 0, (snd_alias_system_t)1);
        }
    }
}

// Name from CoD4; original CoD2 name unknown.
static void EndShellShockLookControl(void)
{
    cg->shellshock.sensitivity = 1.0f;
    CL_CapTurnRate(0.0f, 0.0f);
}

// Name from CoD4; original CoD2 name unknown.
static void EndShellShockCamera(void)
{
    cg->shellshock.viewDelta[0] = 0.0f;
    cg->shellshock.viewDelta[1] = 0.0f;
}

// Name from CoD4; original CoD2 name unknown.
void EndShellShock(void)
{
    EndShellShockSound();
    EndShellShockLookControl();
    EndShellShockCamera();
    CL_SetUserCmdInShellshock(0);
}

// Name from CoD4; original CoD2 name unknown.
static void UpdateShellShockSound(const shellshock_parms_t *parms, int time, int duration)
{
    float channelvolume[11];
    float fade;
    void *alias0;
    void *alias1;
    void *alias;
    int dt;
    int end;
    int i;

    if (parms->sound.use == 0) {
        EndShellShockSound();
        return;
    }

    dt = parms->sound.modEndDelay - time + parms->sound.fadeOutTime + duration;
    if (dt < parms->sound.fadeOutTime)
        fade = (float)dt / parms->sound.fadeOutTime;
    else if (time < parms->sound.fadeInTime)
        fade = (float)time / parms->sound.fadeInTime;
    else
        fade = 1.0f;

    if (fade < 0.0f)
        fade = 0.0f;

    if (fade != 0.0f) {
        for (i = 0; i < 11; i++)
            channelvolume[i] = (parms->sound.channelvolume[i] - 1.0f) * fade + 1.0f;
        SND_SetChannelVolumes(3, channelvolume, 0);
        SND_SetEnvironmentEffects(2, parms->sound.roomtype,
                                  fade * parms->sound.drylevel,
                                  fade * parms->sound.wetlevel, 0);
    } else {
        SND_DeactivateChannelVolumes(3, 0);
        SND_DeactivateEnvironmentEffects(2, 0);
    }

    dt = parms->sound.loopEndDelay - time + duration + parms->sound.loopFadeTime;
    if (dt > 0) {
        alias0 = CL_PickSoundAlias("shellshock_loop");
        alias1 = CL_PickSoundAlias("shellshock_loop_silent");
        if (parms->sound.loopFadeTime) {
            fade = 1.0f - (float)dt / parms->sound.loopFadeTime;
            if (fade < 0.0f)
                fade = 0.0f;
        }
        SND_PlayBlendedSoundAliases((const snd_alias_t *)alias0, (const snd_alias_t *)alias1, fade, 0x3ff, vec3_origin, 0, (snd_alias_system_t)1);
    }

    end = parms->sound.loopEndDelay - time + cg->time + duration;
    if (cg->time < end) {
        if (cg->shellshock.loopEndTime) {
            cg->shellshock.loopEndTime = 0;
            alias = CL_PickSoundAlias("shellshock_end_abort");
            SND_PlaySoundAlias((const snd_alias_t *)alias, 0x3ff, vec3_origin, 0, (snd_alias_system_t)1);
        }
    } else if (end != cg->shellshock.loopEndTime) {
        cg->shellshock.loopEndTime = end;
        alias = CL_PickSoundAlias("shellshock_end");
        SND_PlaySoundAlias((const snd_alias_t *)alias, 0x3ff, vec3_origin, cg->time - end, (snd_alias_system_t)1);
    }
}

// Name from CoD4; original CoD2 name unknown.
static void UpdateShellShockLookControl(const shellshock_parms_t *parms, int time, int duration)
{
    float fade;
    float scale;

    if (parms->mouse.use) {
        if (duration - time >= parms->mouse.fadeTime)
            fade = 1.0f;
        else if (duration - time > 0)
            fade = (float)(duration - time) / parms->mouse.fadeTime;
        else
            goto endLook;

        if (fade == 1.0f) {
            cg->shellshock.sensitivity = parms->mouse.sensitivity;
            CL_CapTurnRate(parms->mouse.maxPitchSpeed, parms->mouse.maxYawSpeed);
        } else {
            cg->shellshock.sensitivity = (parms->mouse.sensitivity - 1.0f) * fade + 1.0f;
            scale = 1.0f / fade;
            CL_CapTurnRate(parms->mouse.maxPitchSpeed * scale, parms->mouse.maxYawSpeed * scale);
        }
        return;
    }

endLook:
    EndShellShockLookControl();
}

// Name from CoD4; original CoD2 name unknown.
static void UpdateShellShockCamera(const shellshock_parms_t *parms, int time, int duration)
{
    int dt;
    float scale;
    float radius;
    float t;
    int base;
    const float *perturb;

    dt = duration - time;
    if (dt <= 0) {
        EndShellShockCamera();
        return;
    }

    scale = 1.0f;
    if (dt < parms->view.fadeTime)
        scale = (float)dt / parms->view.fadeTime;
    radius = ((3.0f - scale * 2.0f) * scale * scale) * parms->view.kickRadius;

    t = (float)time * parms->view.kickRate;
    base = FastFloor(t);
    t = t - (float)base;
    perturb = cg_perturbations[(base + 61 * duration) & 0x7f];

    cg->shellshock.viewDelta[0] = CubicInterpolate(t, perturb[0], perturb[2], perturb[4], perturb[6]) * radius;
    cg->shellshock.viewDelta[1] = CubicInterpolate(t, perturb[1], perturb[3], perturb[5], perturb[7]) * radius;
}

void CG_UpdateShellShock(const shellshock_parms_t *parms, int start, int duration)
{
    int time;

    time = cg->time - start;
    if (start == 0 || time < 0) {
        EndShellShock();
        return;
    }

    UpdateShellShockSound(parms, time, duration);
    UpdateShellShockLookControl(parms, time, duration);
    UpdateShellShockCamera(parms, time, duration);
    CL_SetUserCmdInShellshock(time < duration ? 1 : 0);
}

// retail 0x4cd070, unreferenced; CoD4 CG_StartShellShock (SUPPORTED)
void CG_StartShellShock(const shellshock_parms_t *parms, int start, int duration)
{
    cg->shellshock.parms = parms;
    cg->shellshock.startTime = start;
    cg->shellshock.duration = duration;
}

