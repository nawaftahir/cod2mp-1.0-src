// first declaration fixes the LTCG walk order
void CG_ProcessSnapshots();
#include "cod2_feature_config.h"
#include "common_types.h"
#include "imports.h"
#include "headers/PC/cgame_mp/cg_local.h"
#include <math.h>
extern bgs_t *bgs;
extern displayContextDef_t cgDC;

extern dvar_t *bg_bobMax;
extern const dvar_t *cg_dumpAnims;
extern const dvar_t *cg_errorDecay;
extern const dvar_t *cg_fov;
extern const dvar_t *cg_fovMin;
extern const dvar_t *cg_fovScale;
extern const dvar_t *cg_paused;
extern const dvar_t *cg_thirdPerson;
extern const dvar_t *cg_thirdPersonAngle;
extern const dvar_t *cg_thirdPersonRange;
extern dvar_t *cg_viewsize;
extern void SND_SetListener(int entnum, const vec_t *origin, vec3_t *axis);
extern void R_UpdateEffectsNonBolt(void);
extern void CG_AddViewWeapon(playerState_t *ps);
extern void CL_SetLodOrigin(const refdef_t *fd);
extern void CL_IssueDelayedDrawing(int marker);
extern void CL_Input(void);
extern void CL_EndDelayedDrawing(int marker);
extern void CG_UpdateShellShock(const shellshock_parms_t *parms, int start, int duration);
extern void CG_ProcessSnapshots(void);
extern void CG_DrawPlayerSprites(void);
extern void CG_DrawActive(void);
extern void CG_Draw3dHudElems(void);
extern void CG_Draw2D(void);
extern void CG_AddPacketEntities(void);
extern void CG_AddLocalEntities(void);
extern void CG_AddLagometerFrameInfo(void);

extern void Com_Printf(const char *fmt, ...);
extern void FX_FreeSystem(void);
extern int FX_InitSystem( bool rendererExists );
extern int Cmd_Argc(void);
extern const char *CG_Argv(int arg);
extern void I_strncpyz(char *dest, const char *src, int destsize);
extern EffectTemplate *FX_RegisterEffect(const char *name);
extern void FX_PlaySimpleEffect(const EffectTemplate *fx, const vec_t *origin);
extern double atof(const char *str);
extern int BG_GetViewmodelWeaponIndex(const playerState_t *ps);
extern int BG_GetNumWeapons(void);
extern int CL_BeginDelayedDrawing(void);
extern WeaponDef *BG_GetWeaponDef(int weapIndex);
extern int BG_IsAimDownSightWeapon(int weapIndex);
extern float atanf(float x);
extern float sinf(float x);
extern double tan(double x);
extern void CL_ResetSkeletonCache(int level);
extern void CG_UpdateViewWeaponAnim(playerState_t *ps);
extern struct DObj_s *Com_GetClientDObj(int handle, int localClientNum);
extern void CG_DObjUpdateInfo(struct DObj_s *obj);
extern void CG_ProcessClientNoteTracks(int clientNum);
extern void CG_CalcEntityLerpPositions(centity_t *cent);
extern void CG_ProcessEntity(centity_t *cent);
extern void DObjDisplayAnim(DObj *obj);
extern void R_UpdateEffectsBolt(void);
extern void CL_FX_AdjustCamera(refdef_t *refdef);
extern void FX_AdjustTime(int serverTime);
extern int CG_PointContents(const vec_t *point, int passEntityNum, int contentmask);
extern void CG_PredictPlayerState(void);
extern void AngleVectors( const vec3_t angles, vec3_t forward, vec3_t right, vec3_t up );
extern void CG_Trace(trace_t *result, const vec_t *start, const vec_t *mins, const vec_t *maxs, const vec_t *end, int skipNumber, int mask);
extern float CL_GetMenuBlurRadius(void);
extern void SetScreenScaling(float scaleX, float scaleY, float x, float y, float w, float h);
extern float BG_GetBobCycle(const playerState_t *ps);
extern float BG_GetSpeed(const playerState_t *ps, int time);
extern float BG_GetVerticalBobFactor(const playerState_t *ps, float cycle, float speed, float maxAmp);
extern float BG_GetHorizontalBobFactor(const playerState_t *ps, float cycle, float speed, float maxAmp);
extern void BG_CalculateViewAngles(viewState_t *vs, vec_t *angles);
extern void AddLeanToPosition(vec_t *position, const float fViewYaw, const float fLeanFrac, const float fViewRoll, const float fLeanDist);
extern void CG_ShakeCamera(void);
extern void CG_PerturbCamera(void);
extern void AnglesToAxis( const vec3_t angles, vec3_t axis[3] );
extern void AxisToAngles( vec3_t axis[3], vec3_t angles );
extern void MatrixMultiply43( const float in1[4][3], const float in2[4][3], float out[4][3] );
extern void ConvertQuatToMat( const DObjAnimMat *mat, float axis[3][3] );
extern int DObjGetBoneIndex( const DObj *obj, unsigned int boneName );
extern DObjAnimMat *DObjGetRotTransArray( const DObj_s *obj );
extern weaponInfo_t cg_weaponsArray[];
extern scr_const_t scr_const;
extern void VectorMA( const vec3_t v, float s, const vec3_t b, vec3_t o );
extern float crandom( void );
extern void Com_Error( errorParm_t code, const char *fmt, ... );
extern qboolean CG_DObjGetWorldTagPos( const centity_t *cent, struct DObj_s *obj, unsigned int tagName, vec_t *pos );

void CG_FxRestart(void);
void CG_FxTest(void);
float CG_GetViewFov(void);
static void CG_CalcFov(void);
void CG_FxSetTestPosition(void);
// inlined com_math VectorMA: retail evaluates through the pointer params
static __forceinline void Vec3MadInl(const vec_t *start, float scale, const vec_t *dir, vec_t *out)
{
    out[0] = start[0] + scale * dir[0];
    out[1] = start[1] + scale * dir[1];
    out[2] = start[2] + scale * dir[2];
}

static void CG_OffsetThirdPersonView(void);
static void CG_CalcViewValues(void);
void CG_InitView(void);
qboolean CG_DrawActiveFrame(int serverTime, DemoType demoType, CubemapShot cubemapShot, int cubemapSize, qboolean renderScreen);

void CG_FxSetTestPosition(void)
{
    float *start = cg->refdef.vieworg;
    float *dir = cg->refdef.viewaxis[0];
    float *result = cg->testFxPos;

    result[0] = start[0] + 100.0f * dir[0];
    result[1] = start[1] + 100.0f * dir[1];
    result[2] = start[2] + 100.0f * dir[2];

    Com_Printf((const char *)"\n\nFX Testing position set to: (%f, %f, %f)\n\n",
               (double)result[0], (double)result[1], (double)result[2]);
}

void CG_FxRestart(void)
{
    Com_Printf((const char *)"FX Restarting so off-line changes are loaded.\n");
    FX_FreeSystem();
    FX_InitSystem(1);
}

void CG_FxTest(void)
{
    char *fxName;
    EffectTemplate *fx;

    if (Cmd_Argc() < 2) {
        Com_Printf((const char *)"Must supply filename from base path.  Optional restart time.\n");
    }

    {
        fxName = cg->testFxName;
        I_strncpyz(fxName, CG_Argv(1), 0x40);

        fx = FX_RegisterEffect(fxName);
        Com_Printf((const char *)"Spawning Fx %s\n", fxName);
        FX_PlaySimpleEffect(fx, (const vec_t *)cg->testFxPos);
        cg->testFxTime = cg->time;

        if (Cmd_Argc() == 3) {
            cg->testFxRespawnTime = (int)(atof(CG_Argv(2)) * 1000.0);
        } else {
            cg->testFxRespawnTime = 0;
        }
    }
}

// Name from Q3.
static void CG_CalcVrect(int size)
{
    cg->refdef.width = cgs->viewWidth * size / 100;
    cg->refdef.width &= ~1;
    cg->refdef.height = cgs->viewHeight * size / 100;
    cg->refdef.height &= ~1;
    cg->refdef.x = (cgs->viewWidth - cg->refdef.width) / 2 + cgs->viewX;
    cg->refdef.y = (cgs->viewHeight - cg->refdef.height) / 2 + cgs->viewY;
    SetScreenScaling(1.0f, 1.0f, (float)cg->refdef.x, (float)cg->refdef.y,
                     (float)cg->refdef.width, (float)cg->refdef.height);
}

static void CG_OffsetThirdPersonView(void)
{
    static vec3_t mins = { -4.0f, -4.0f, -4.0f };
    static vec3_t maxs = { 4.0f, 4.0f, 4.0f };
    vec3_t forward, right, up;
    vec3_t view;
    vec3_t focusAngles;
    trace_t trace;
    vec3_t focusPoint;
    float focusDist;
    float distSq;

    cg->refdef.vieworg[2] += cg->predictedPlayerState.viewHeightCurrent;
    focusAngles[0] = cg->refdefViewAngles[0];
    focusAngles[1] = cg->refdefViewAngles[1];
    focusAngles[2] = cg->refdefViewAngles[2];
    // if dead, look at killer
    if (cg->predictedPlayerState.pm_type >= 6)
    {
        focusAngles[1] = (float)cg->predictedPlayerState.stats[1];
        cg->refdefViewAngles[1] = (float)cg->predictedPlayerState.stats[1];
    }
    if (cg->refdefViewAngles[0] > 45.0f)
        focusAngles[0] = 45.0f;
    AngleVectors(focusAngles, forward, NULL, NULL);
    Vec3MadInl(cg->refdef.vieworg, 512.0f, forward, focusPoint);


    view[0] = cg->refdef.vieworg[0];
    view[1] = cg->refdef.vieworg[1];
    view[2] = cg->refdef.vieworg[2] + 8.0f;
    cg->refdefViewAngles[0] *= 0.5;
    cg->refdefViewAngles[1] -= cg_thirdPersonAngle->current.value;
    AngleVectors(cg->refdefViewAngles, forward, right, up);
    VectorMA(view, -cg_thirdPersonRange->current.value, forward, view);
    // keep the camera out of solids
    CG_Trace(&trace, cg->refdef.vieworg, mins, maxs, view, cg->predictedPlayerState.clientNum, 0x811);
    if (trace.fraction != 1.0)
    {
        view[0] = (view[0] - cg->refdef.vieworg[0]) * trace.fraction + cg->refdef.vieworg[0];
        view[1] = (view[1] - cg->refdef.vieworg[1]) * trace.fraction + cg->refdef.vieworg[1];
        view[2] = (view[2] - cg->refdef.vieworg[2]) * trace.fraction + cg->refdef.vieworg[2];
        view[2] += (1.0f - trace.fraction) * 32.0f;
        // a tunnel ceiling may be close enough to poke through
        CG_Trace(&trace, cg->refdef.vieworg, mins, maxs, view, cg->predictedPlayerState.clientNum, 0x811);
        view[0] = (view[0] - cg->refdef.vieworg[0]) * trace.fraction + cg->refdef.vieworg[0];
        view[1] = (view[1] - cg->refdef.vieworg[1]) * trace.fraction + cg->refdef.vieworg[1];
        view[2] = (view[2] - cg->refdef.vieworg[2]) * trace.fraction + cg->refdef.vieworg[2];
    }
    cg->refdef.vieworg[0] = view[0];
    cg->refdef.vieworg[1] = view[1];
    cg->refdef.vieworg[2] = view[2];
    // pitch to look at the focus point
    focusPoint[0] = focusPoint[0] - cg->refdef.vieworg[0];
    focusPoint[1] = focusPoint[1] - cg->refdef.vieworg[1];
    focusPoint[2] = focusPoint[2] - cg->refdef.vieworg[2];
    distSq = focusPoint[1] * focusPoint[1] + focusPoint[0] * focusPoint[0];
    focusDist = (float)sqrt(distSq);
    if (focusDist < 1.0f)
        focusDist = 1.0f;
    cg->refdefViewAngles[0] = (float)atan2(focusPoint[2], focusDist) * -57.295776f;
}

static void CG_StepOffset(void)
{
    int timeDelta;

    timeDelta = cg->time - cg->stepTime;
    if (timeDelta < 0)
        cg->stepTime = cg->time;
    if (timeDelta < 100)
        cg->refdef.vieworg[2] -= (float)(100 - timeDelta) * cg->stepChange * 0.01f;
}

// Name from CoD4; original CoD2 name unknown.
static void CG_KickAngles(void)
{
    WeaponDef *weapDef;
    int weapIndex;
    int t;
    int i;
    float ft;
    float idealCenterSpeed;
    float kickChange;

    weapIndex = BG_GetViewmodelWeaponIndex(&cg->predictedPlayerState);
    weapDef = BG_GetWeaponDef(weapIndex);
    t = cg->frametime;
    if (cg->frametime <= 0)
        return;
    do {
        ft = (float)(t > 5 ? 5 : t) * 0.001f;
        for (i = 0; i < 3; i++) {
            if (cg->kickAVel[i] == 0.0f && cg->kickAngles[i] == 0.0f)
                continue;
            if (cg->kickAngles[i] != 0.0f) {
                idealCenterSpeed = cg->kickAngles[i] > 0.0f ? -1.0f : 1.0f;
                if (weapIndex) {
                    if (cg->predictedPlayerState.fWeaponPosFrac > 0.5f)
                        idealCenterSpeed *= weapDef->fAdsViewKickCenterSpeed;
                    else
                        idealCenterSpeed *= weapDef->fHipViewKickCenterSpeed;
                } else {
                    idealCenterSpeed *= 2400.0f;
                }
                cg->kickAVel[i] += idealCenterSpeed * ft;
            }
            kickChange = cg->kickAVel[i] * ft;
            if (cg->kickAngles[i] * kickChange < 0.0f)
                kickChange *= 0.06f;
            if ((cg->kickAngles[i] + kickChange) * cg->kickAngles[i] >= 0.0f) {
                cg->kickAngles[i] = cg->kickAngles[i] + kickChange;
                if (cg->kickAngles[i] == 0.0f) {
                    cg->kickAVel[i] = 0.0f;
                } else if (fabs(cg->kickAngles[i]) > 10.0f) {
                    cg->kickAngles[i] = cg->kickAngles[i] > 0.0f ? 10.0f : -10.0f;
                    cg->kickAVel[i] = 0.0f;
                }
            } else {
                cg->kickAngles[i] = 0.0f;
                cg->kickAVel[i] = 0.0f;
            }
        }
        t -= 5;
    } while (t > 0);
}

static void CG_OffsetFirstPersonView(void)
{
    viewState_t vs;
    vec3_t angles;
    vec3_t right;
    float delta;
    float deltaB;
    float f;
    float floorZ;

    if (cg->nextSnap->ps.pm_type == 5)
        return;
    if (cg->predictedPlayerState.eFlags & 0x300)
        return;

    vs.ps = &cg->predictedPlayerState;
    if (cg->damageTime)
        vs.damageTime = cg->damageTime - cg->predictedPlayerState.deltaTime;
    else
        vs.damageTime = 0;
    vs.time = cg->time - cg->predictedPlayerState.deltaTime;
    vs.v_dmg_pitch = cg->v_dmg_pitch;
    vs.v_dmg_roll = cg->v_dmg_roll;
    vs.xyspeed = cg->xyspeed;
    vs.frametime = (float)cg->frametime * 0.001f;
    vs.fLastIdleFactor = cg->playerEntity.fLastIdleFactor;
    vs.weapIdleTime = &cg->weapIdleTime;
    BG_CalculateViewAngles(&vs, angles);

    cg->refdefViewAngles[0] += angles[0];
    cg->refdefViewAngles[1] += angles[1];
    cg->refdefViewAngles[2] += angles[2];

    cg->refdef.vieworg[2] += cg->predictedPlayerState.viewHeightCurrent;

    delta = BG_GetVerticalBobFactor(&cg->predictedPlayerState, cg->fBobCycle, cg->xyspeed, bg_bobMax->current.value);
    cg->refdef.vieworg[2] += delta;

    deltaB = BG_GetHorizontalBobFactor(&cg->predictedPlayerState, cg->fBobCycle, cg->xyspeed, bg_bobMax->current.value);
    AngleVectors(cg->refdefViewAngles, NULL, right, NULL);
    cg->refdef.vieworg[0] += right[0] * deltaB;
    cg->refdef.vieworg[1] += right[1] * deltaB;
    cg->refdef.vieworg[2] += right[2] * deltaB;

    delta = (float)(cg->time - cg->landTime);
    if (delta < 0.0f)
        cg->landTime = cg->time - 450;
    if (delta < 150.0f) {
        f = delta / 150.0f;
        cg->refdef.vieworg[2] += cg->landChange * f;
    } else if (delta < 450.0f) {
        delta -= 150.0f;
        f = 1.0f - delta / 300.0f;
        cg->refdef.vieworg[2] += cg->landChange * f;
    }

    CG_StepOffset();

    AddLeanToPosition(cg->refdef.vieworg, cg->refdefViewAngles[1], cg->predictedPlayerState.leanf, 16.0f, 20.0f);

    floorZ = cg->predictedPlayerState.origin[2] + 8.0f;
    if (cg->refdef.vieworg[2] < floorZ)
        cg->refdef.vieworg[2] = floorZ;
}

float CG_GetViewFov(void)
{
    int weapIndex;
    char *weapDef;
    float fov_x;

    {
        weapIndex = BG_GetViewmodelWeaponIndex( (const playerState_t *)((void *)&cg->predictedPlayerState));
        weapDef = (char *)BG_GetWeaponDef(weapIndex);

        if (cg->predictedPlayerState.pm_type == 5) {
            fov_x = 90.0f;
        } else {
            fov_x = cg_fov->current.value;

            if (BG_IsAimDownSightWeapon(weapIndex)) {
                float adsFov = ((WeaponDef *)weapDef)->fAdsZoomFov;
                int *peBase = (int *)&((cg_t *)cg)->playerEntity.fLastWeaponPosFrac;

                if (cg->predictedPlayerState.fWeaponPosFrac == 1.0f) {
                    fov_x = adsFov;
                } else if (cg->predictedPlayerState.fWeaponPosFrac != 0.0f) {
                    float normalizedLerp;

                    if (peBase[1] != 0) {
                        normalizedLerp = cg->predictedPlayerState.fWeaponPosFrac - (1.0f - ((WeaponDef *)weapDef)->fAdsZoomInFrac);
                        if (normalizedLerp > 0.0f) {
                            normalizedLerp /= ((WeaponDef *)weapDef)->fAdsZoomInFrac;
                        }
                    } else {
                        normalizedLerp = cg->predictedPlayerState.fWeaponPosFrac - (1.0f - ((WeaponDef *)weapDef)->fAdsZoomOutFrac);
                        if (normalizedLerp > 0.0f) {
                            normalizedLerp /= ((WeaponDef *)weapDef)->fAdsZoomOutFrac;
                        }
                    }

                    if (normalizedLerp > 0.0f) {
                        fov_x -= (fov_x - adsFov) * normalizedLerp;
                    }
                }
            }
        }
    }

    if (cg->predictedPlayerState.eFlags & 0x300) {
        fov_x = 55.0f;
    }

    fov_x *= cg_fovScale->current.value;

    {
        float fovMin = (cg_fovMin)->current.value;
        fov_x = (fovMin - fov_x >= 0.0f) ? fovMin : fov_x;
    }

    return fov_x;
}

static void CG_CalcFov(void)
{
    float fov;
    float tanVal;
    float fov_x, fov_y;
    float wave;

    fov = CG_GetViewFov();

    tanVal = tan((fov * 0.017453292f) * 0.5f) * 0.75f;
    fov_x = (atan(tanVal * cgs->viewAspect) * 57.295776f) * 2.0f;
    fov_y = (atan(tanVal) * 57.295776f) * 2.0f;

    if (CG_PointContents((const vec_t *)cg->refdef.vieworg, -1, 0x20)) {
        // 0.4 Hz underwater warp: 2 * pi * 0.4 / 1000
        wave = sin(cg->time * 0.0025132743f);
        fov_x += wave;
        fov_y -= wave;
    }

    cg->refdef.fov_x = fov_x;
    cg->refdef.fov_y = fov_y;
    cg->zoomSensitivity = fov_x / cg_fov->current.value;
}

// Name from CoD4.
static void CG_CalcCubemapViewValues(void)
{
    cg->refdef.x = 0;
    cg->refdef.y = 0;
    cg->refdef.width = cg->cubemapSize + 2;
    cg->refdef.height = cg->cubemapSize + 2;
    cg->refdef.fov_x = (float)(atan2((double)(cg->cubemapSize + 2), (double)cg->cubemapSize) * 114.59155902616465);
    cg->refdef.fov_y = cg->refdef.fov_x;
    cg->refdef.vieworg[0] = cg->predictedPlayerState.origin[0];
    cg->refdef.vieworg[1] = cg->predictedPlayerState.origin[1];
    cg->refdef.vieworg[2] = cg->predictedPlayerState.origin[2];
    cg->refdef.vieworg[2] = cg->refdef.vieworg[2] + cg->predictedPlayerState.viewHeightCurrent;

    switch (cg->cubemapShot) {
    case 1:
        cg->refdef.viewaxis[0][0] = 0.0f;
        cg->refdef.viewaxis[0][1] = 0.0f;
        cg->refdef.viewaxis[0][2] = 1.0f;
        cg->refdef.viewaxis[1][0] = 0.0f;
        cg->refdef.viewaxis[1][1] = 1.0f;
        cg->refdef.viewaxis[1][2] = 0.0f;
        cg->refdef.viewaxis[2][0] = -1.0f;
        cg->refdef.viewaxis[2][1] = 0.0f;
        cg->refdef.viewaxis[2][2] = 0.0f;
        break;
    case 2:
        cg->refdef.viewaxis[0][0] = 0.0f;
        cg->refdef.viewaxis[0][1] = 0.0f;
        cg->refdef.viewaxis[0][2] = -1.0f;
        cg->refdef.viewaxis[1][0] = 0.0f;
        cg->refdef.viewaxis[1][1] = 1.0f;
        cg->refdef.viewaxis[1][2] = 0.0f;
        cg->refdef.viewaxis[2][0] = 1.0f;
        cg->refdef.viewaxis[2][1] = 0.0f;
        cg->refdef.viewaxis[2][2] = 0.0f;
        break;
    case 3:
        cg->refdef.viewaxis[0][0] = -1.0f;
        cg->refdef.viewaxis[0][1] = 0.0f;
        cg->refdef.viewaxis[0][2] = 0.0f;
        cg->refdef.viewaxis[1][0] = 0.0f;
        cg->refdef.viewaxis[1][1] = -1.0f;
        cg->refdef.viewaxis[1][2] = 0.0f;
        cg->refdef.viewaxis[2][0] = 0.0f;
        cg->refdef.viewaxis[2][1] = 0.0f;
        cg->refdef.viewaxis[2][2] = 1.0f;
        break;
    case 4:
        cg->refdef.viewaxis[0][0] = 1.0f;
        cg->refdef.viewaxis[0][1] = 0.0f;
        cg->refdef.viewaxis[0][2] = 0.0f;
        cg->refdef.viewaxis[1][0] = 0.0f;
        cg->refdef.viewaxis[1][1] = 1.0f;
        cg->refdef.viewaxis[1][2] = 0.0f;
        cg->refdef.viewaxis[2][0] = 0.0f;
        cg->refdef.viewaxis[2][1] = 0.0f;
        cg->refdef.viewaxis[2][2] = 1.0f;
        break;
    case 5:
        cg->refdef.viewaxis[0][0] = 0.0f;
        cg->refdef.viewaxis[0][1] = -1.0f;
        cg->refdef.viewaxis[0][2] = 0.0f;
        cg->refdef.viewaxis[1][0] = 1.0f;
        cg->refdef.viewaxis[1][1] = 0.0f;
        cg->refdef.viewaxis[1][2] = 0.0f;
        cg->refdef.viewaxis[2][0] = 0.0f;
        cg->refdef.viewaxis[2][1] = 0.0f;
        cg->refdef.viewaxis[2][2] = 1.0f;
        break;
    case 6:
        cg->refdef.viewaxis[0][0] = 0.0f;
        cg->refdef.viewaxis[0][1] = 1.0f;
        cg->refdef.viewaxis[0][2] = 0.0f;
        cg->refdef.viewaxis[1][0] = -1.0f;
        cg->refdef.viewaxis[1][1] = 0.0f;
        cg->refdef.viewaxis[1][2] = 0.0f;
        cg->refdef.viewaxis[2][0] = 0.0f;
        cg->refdef.viewaxis[2][1] = 0.0f;
        cg->refdef.viewaxis[2][2] = 1.0f;
        break;
    }
}

// Original name unknown.
static void CG_CalcTurretViewValues(void)
{
    centity_t *cent;
    struct DObj_s *obj;

    if (!(cg->predictedPlayerState.eFlags & 0x300))
        return;

    cent = &cg_entities[cg->predictedPlayerState.viewlocked_entNum];
    obj = Com_GetClientDObj(cent->nextState.number, cent->localClientNum);
    if (!obj)
        return;

    if (!CG_DObjGetWorldTagPos(cent, obj, scr_const.tag_player, cg->refdef.vieworg))
        Com_Error(ERR_DROP, "\x15Turret has no bone: tag_player");

    if (cg->predictedPlayerState.viewlocked == 2 && !cg->renderingThirdPerson)
    {
        cg->refdefViewAngles[0] = crandom() * BG_GetWeaponDef(cent->nextState.weapon)->vertViewJitter + cg->refdefViewAngles[0];
        cg->refdefViewAngles[1] = crandom() * BG_GetWeaponDef(cent->nextState.weapon)->horizViewJitter + cg->refdefViewAngles[1];
    }
}

// Original name unknown.
static void CG_SetViewModelFromRefdef(void)
{
    cg->viewModelOrigin[0] = cg->refdef.vieworg[0];
    cg->viewModelOrigin[1] = cg->refdef.vieworg[1];
    cg->viewModelOrigin[2] = cg->refdef.vieworg[2];
    cg->viewModelAxis[0][0] = cg->refdef.viewaxis[0][0];
    cg->viewModelAxis[0][1] = cg->refdef.viewaxis[0][1];
    cg->viewModelAxis[0][2] = cg->refdef.viewaxis[0][2];
    cg->viewModelAxis[1][0] = cg->refdef.viewaxis[1][0];
    cg->viewModelAxis[1][1] = cg->refdef.viewaxis[1][1];
    cg->viewModelAxis[1][2] = cg->refdef.viewaxis[1][2];
    cg->viewModelAxis[2][0] = cg->refdef.viewaxis[2][0];
    cg->viewModelAxis[2][1] = cg->refdef.viewaxis[2][1];
    cg->viewModelAxis[2][2] = cg->refdef.viewaxis[2][2];
    cg->viewModelAngles[0] = cg->refdefViewAngles[0];
    cg->viewModelAngles[1] = cg->refdefViewAngles[1];
    cg->viewModelAngles[2] = cg->refdefViewAngles[2];
}

// Name from CoD4.
static void CG_ApplyViewAnimation(void)
{
    int weaponIndex;
    weaponInfo_t *weapInfo;
    int boneIndex;
    DObjAnimMat *boneMtx;
    float out[4][3];
    float viewMtx[4][3];
    float boneAxis[4][3];

    if (cg->predictedPlayerState.pm_type == 4 || cg->predictedPlayerState.pm_type == 5)
        return;
    if (cg->renderingThirdPerson)
        return;
    if (cg->predictedPlayerState.eFlags & 0x300)
        return;

    weaponIndex = BG_GetViewmodelWeaponIndex(&cg->predictedPlayerState);
    if (weaponIndex <= 0)
        return;

    weapInfo = &cg_weaponsArray[weaponIndex];
    if (!weapInfo->viewModelDObj)
        return;

    boneIndex = DObjGetBoneIndex(weapInfo->viewModelDObj, scr_const.tag_camera);
    if (boneIndex < 0)
        return;

    boneMtx = DObjGetRotTransArray(weapInfo->viewModelDObj);
    if (!boneMtx)
        return;

    boneMtx += boneIndex;
    ConvertQuatToMat(boneMtx, boneAxis);
    boneAxis[3][0] = boneMtx->trans[0];
    boneAxis[3][1] = boneMtx->trans[1];
    boneAxis[3][2] = boneMtx->trans[2];
    viewMtx[0][0] = cg->refdef.viewaxis[0][0];
    viewMtx[0][1] = cg->refdef.viewaxis[0][1];
    viewMtx[0][2] = cg->refdef.viewaxis[0][2];
    viewMtx[1][0] = cg->refdef.viewaxis[1][0];
    viewMtx[1][1] = cg->refdef.viewaxis[1][1];
    viewMtx[1][2] = cg->refdef.viewaxis[1][2];
    viewMtx[2][0] = cg->refdef.viewaxis[2][0];
    viewMtx[2][1] = cg->refdef.viewaxis[2][1];
    viewMtx[2][2] = cg->refdef.viewaxis[2][2];
    viewMtx[3][0] = cg->refdef.vieworg[0];
    viewMtx[3][1] = cg->refdef.vieworg[1];
    viewMtx[3][2] = cg->refdef.vieworg[2];
    MatrixMultiply43(boneAxis, viewMtx, out);
    cg->refdef.viewaxis[0][0] = out[0][0];
    cg->refdef.viewaxis[0][1] = out[0][1];
    cg->refdef.viewaxis[0][2] = out[0][2];
    cg->refdef.viewaxis[1][0] = out[1][0];
    cg->refdef.viewaxis[1][1] = out[1][1];
    cg->refdef.viewaxis[1][2] = out[1][2];
    cg->refdef.viewaxis[2][0] = out[2][0];
    cg->refdef.viewaxis[2][1] = out[2][1];
    cg->refdef.viewaxis[2][2] = out[2][2];
    cg->refdef.vieworg[0] = out[3][0];
    cg->refdef.vieworg[1] = out[3][1];
    cg->refdef.vieworg[2] = out[3][2];
    AxisToAngles(cg->refdef.viewaxis, cg->refdefViewAngles);
}

static void CG_CalcViewValues(void)
{
    int size;
    float menuBlur;
    float f;

    memset(&cg->refdef, 0, sizeof(refdef_t));

    menuBlur = CL_GetMenuBlurRadius();
    cg->refdef.blurRadius = sqrtf(cgDC.blurRadiusOut * cgDC.blurRadiusOut + menuBlur * menuBlur);

    if (cg->cubemapShot) {
        CG_CalcCubemapViewValues();
        return;
    }

    if (cg->nextSnap->ps.pm_type == 5)
        size = 100;
    else
        size = cg_viewsize->current.integer;
    CG_CalcVrect(size);

    if (cg->predictedPlayerState.pm_type == 5) {
        cg->refdef.vieworg[0] = cg->predictedPlayerState.origin[0];
        cg->refdef.vieworg[1] = cg->predictedPlayerState.origin[1];
        cg->refdef.vieworg[2] = cg->predictedPlayerState.origin[2];
        cg->refdefViewAngles[0] = cg->predictedPlayerState.viewangles[0];
        cg->refdefViewAngles[1] = cg->predictedPlayerState.viewangles[1];
        cg->refdefViewAngles[2] = cg->predictedPlayerState.viewangles[2];
        AnglesToAxis(cg->refdefViewAngles, cg->refdef.viewaxis);
        CG_CalcFov();
        return;
    }

    cg->fBobCycle = BG_GetBobCycle(&cg->predictedPlayerState);
    cg->xyspeed = BG_GetSpeed(&cg->predictedPlayerState, cg->time);

    cg->refdef.vieworg[0] = cg->predictedPlayerState.origin[0];
    cg->refdef.vieworg[1] = cg->predictedPlayerState.origin[1];
    cg->refdef.vieworg[2] = cg->predictedPlayerState.origin[2];
    cg->refdefViewAngles[0] = cg->predictedPlayerState.viewangles[0];
    cg->refdefViewAngles[1] = cg->predictedPlayerState.viewangles[1];
    cg->refdefViewAngles[2] = cg->predictedPlayerState.viewangles[2];

    if (cg_errorDecay->current.value > 0.0f) {
        f = (cg_errorDecay->current.value - (float)(cg->time - cg->predictedErrorTime)) / cg_errorDecay->current.value;
        if (f > 0.0f && f < 1.0f)
            VectorMA(cg->refdef.vieworg, f, cg->predictedError, cg->refdef.vieworg);
        else
            cg->predictedErrorTime = 0;
    }

    CG_CalcTurretViewValues();

    if (cg->renderingThirdPerson)
        CG_OffsetThirdPersonView();
    else
        CG_OffsetFirstPersonView();


    CG_ShakeCamera();

    AnglesToAxis(cg->refdefViewAngles, cg->refdef.viewaxis);

    CG_SetViewModelFromRefdef();
    CG_ApplyViewAnimation();

    CG_PerturbCamera();
    CG_CalcFov();
}

// Name from CoD4; original CoD2 name unknown.
static void CG_UpdateTestFX(void)
{
    if (cg->testFxRespawnTime >= 1) {
        if (cg->time > cg->testFxRespawnTime + cg->testFxTime) {
            FX_PlaySimpleEffect(FX_RegisterEffect(cg->testFxName), cg->testFxPos);
            cg->testFxTime = cg->time;
        }
    }
}

// Name from CoD4; original CoD2 name unknown.
static void CG_UpdateEntInfo(void)
{
    struct DObj_s *obj;
    centity_t *cent;
    int num;
    int entnum;

    for (num = 0; num < cg->nextSnap->numEntities; num++) {
        entnum = cg->nextSnap->entities[num].number;
        obj = Com_GetClientDObj(entnum, 0);
        if (obj) {
            CG_DObjUpdateInfo(obj);
            CG_ProcessClientNoteTracks(entnum);
        }
        cent = &cg_entities[entnum];
        if (cent->nextState.eType < 10)
            CG_CalcEntityLerpPositions(cent);
    }
}

// Name from CoD4; original CoD2 name unknown.
static void CG_UpdateThirdPerson(void)
{
    if (cg_thirdPerson->current.enabled != 0 ||
        cg->nextSnap->ps.pm_type >= 6) {
        cg->renderingThirdPerson = 1;
    } else {
        cg->renderingThirdPerson = 0;
    }
}

void CG_InitView(void)
{
    ((cg_t *)cg)->refdef.time = cg->time;
    (*(int *)&((cg_t *)cg)->refdef.zNear) = 0x3f800000;

    if (cg_thirdPerson->current.enabled != 0 ||
        cg->nextSnap->ps.pm_type >= 6) {
        cg->renderingThirdPerson = 1;
    } else {
        cg->renderingThirdPerson = 0;
    }

    CG_PredictPlayerState();
    CL_ResetSkeletonCache(0);

    CG_UpdateViewWeaponAnim(&cg->predictedPlayerState);
    CG_CalcViewValues();
    CL_FX_AdjustCamera( (refdef_t *)((void *)&cg->refdef));
    FX_AdjustTime(cg->time);
}

static inline void Vec3LerpFrom(const vec3_t from, const vec3_t to, float frac, vec3_t out)
{
    out[0] = from[0] + (to[0] - from[0]) * frac;
    out[1] = from[1] + (to[1] - from[1]) * frac;
    out[2] = from[2] + (to[2] - from[2]) * frac;
}

// Name from CoD4; original CoD2 name unknown.
static void DumpAnims(void)
{
    int dumpClient = cg_dumpAnims->current.integer;
    if (dumpClient >= 0 && cg_paused->current.integer == 0) {
        struct DObj_s *obj = Com_GetClientDObj(dumpClient, 0);
        if (obj != 0) {
            Com_Printf("client:\n");
            DObjDisplayAnim(obj);
        }
    }
}


qboolean CG_DrawActiveFrame(int serverTime, DemoType demoType,
                            CubemapShot cubemapShot, int cubemapSize,
                            qboolean renderScreen)
{
    snapshot_t *snap;
    snapshot_t *nextSnap;

    cg->oldTime = cg->time;
    cg->time = serverTime;
    cg->bgs.time = serverTime;
    cg->demoType = demoType;
    cg->cubemapShot = cubemapShot;
    cg->cubemapSize = cubemapSize;
    cg->renderScreen = renderScreen;
    cg->frametime = serverTime - cg->oldTime;

    if (cg->frametime < 0) {
        cg->frametime = 0;
        cg->oldTime = serverTime;
    }

    CG_AddLagometerFrameInfo();
    cg->bgs.frametime = cg->frametime;

    if (cg->infoScreenText[0] == '\0') {
        bgs = &cg->bgs;

        CG_ProcessSnapshots();

        if (cg->renderScreen && cg->nextSnap && !(cg->nextSnap->snapFlags & 2)
            && !(legacyHacks->cl_serverloadmap[0] && legacyHacks->cl_serverloadgametype[0])) {
            // Retail tests the load state twice.
            if (legacyHacks->cl_serverloadmap[0] && legacyHacks->cl_serverloadgametype[0]) {
                bgs = 0;
                return 0;
            }
            legacyHacks->cl_serverloadwaiting = 0;

            cg->clientFrame++;

            {
                float frac = cg->frameInterpolation;
                snap = cg->snap;
                nextSnap = cg->nextSnap;
                Vec3LerpFrom(snap->ps.origin, nextSnap->ps.origin, frac, cg->refdef.vieworg);
                cg->refdef.vieworg[2] += (float)cg->nextSnap->ps.viewHeightCurrent;
            }

            CL_SetLodOrigin(&cg->refdef);

            FX_AdjustTime(cg->time - cg->nextSnap->ps.deltaTime);

            {
                snapshot_t *shockSnap = cg->snap;
                int shockIndex = shockSnap->ps.shellshockIndex;
                if (shockIndex == 0) {
                    cg->shellshock.parms = &cgs->shellshockParms[0];
                    cg->shellshock.startTime = cg->testShock.time;
                    cg->shellshock.duration = cg->testShock.duration;
                } else {
                    cg->shellshock.parms = &cgs->shellshockParms[shockIndex];
                    cg->shellshock.startTime = shockSnap->ps.shellshockTime;
                    cg->shellshock.duration = shockSnap->ps.shellshockDuration;
                }
                CG_UpdateShellShock(cg->shellshock.parms,
                                    cg->shellshock.startTime,
                                    cg->shellshock.duration);
            }

            CG_UpdateThirdPerson();

            CG_UpdateEntInfo();

            if (cg->nextSnap->ps.pm_flags & 0x800000) {
                CG_KickAngles();
            } else {
                cg->kickAVel[0] = 0;
                cg->kickAVel[1] = 0;
                cg->kickAVel[2] = 0;
                cg->kickAngles[0] = 0;
                cg->kickAngles[1] = 0;
                cg->kickAngles[2] = 0;
            }

            CL_ResetSkeletonCache(0);
            CG_AddPacketEntities();
            R_UpdateEffectsNonBolt();
            CG_AddLocalEntities();
            CL_Input();
            CG_PredictPlayerState();

            CG_UpdateViewWeaponAnim(&cg->predictedPlayerState);
            CG_CalcViewValues();

            SND_SetListener(cg->nextSnap->ps.clientNum, cg->refdef.vieworg, cg->refdef.viewaxis);

            CG_AddViewWeapon(&cg->predictedPlayerState);
            CL_FX_AdjustCamera(&cg->refdef);

            CG_UpdateTestFX();

            if ((cg->nextSnap->ps.pm_flags & 0xc00000) != 0) {
                struct DObj_s *obj = Com_GetClientDObj(cg->nextSnap->ps.clientNum, 0);
                if (obj != 0) {
                    CG_DObjUpdateInfo(obj);
                    CG_ProcessClientNoteTracks(cg->nextSnap->ps.clientNum);
                }
                CG_CalcEntityLerpPositions(&cg->predictedPlayerEntity);
                CG_ProcessEntity(&cg->predictedPlayerEntity);
            }

            {
                int viewlockedEnt = cg->predictedPlayerState.viewlocked_entNum;
                if (viewlockedEnt != 0x3ff) {
                    CG_ProcessEntity(&cg_entities[viewlockedEnt]);
                }
            }

            DumpAnims();

            R_UpdateEffectsBolt();

            {
                int delayedHandle = CL_BeginDelayedDrawing();

                CG_DrawPlayerSprites();
                CG_Draw3dHudElems();
                CG_Draw2D();
                CL_EndDelayedDrawing(delayedHandle);

                if (cg->weaponSelect < 0 || cg->weaponSelect > BG_GetNumWeapons()) {
                    Com_Printf("WARNING: Invalid weaponSelect setting %i (out of range 0 - %i)\n",
                               cg->weaponSelect, BG_GetNumWeapons());
                    cg->weaponSelect = 0;
                }

                CG_DrawActive();
                CL_IssueDelayedDrawing(delayedHandle);
            }

#if COD2_FEATURE_RUMBLE

            {
                extern void CG_UpdateRumble(void);
                CG_UpdateRumble();
            }
#endif

            bgs = 0;
            return 1;
        }
    }

    bgs = 0;
    return 0;
}

const unsigned char colorWhiteFaded[160] = {
    0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x40, 0x3f,
    0xff, 0xff, 0xff, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xff, 0xff, 0xff, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x07, 0x1d, 0x1d, 0x00, 0xab, 0x22, 0x1d, 0x00, 0xf8, 0x22, 0x1d, 0x00, 0x45, 0x23, 0x1d, 0x00,
    0x92, 0x23, 0x1d, 0x00, 0xdb, 0x23, 0x1d, 0x00, 0x10, 0x24, 0x1d, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xff, 0xff, 0xff, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xa4, 0x43, 0x1d, 0x00, 0x32, 0x44, 0x1d, 0x00, 0x00, 0x44, 0x1d, 0x00, 0x00, 0x44, 0x1d, 0x00,
    0x3f, 0x44, 0x1d, 0x00, 0x92, 0x44, 0x1d, 0x00, 0x9f, 0x44, 0x1d, 0x00, 0x00, 0x00, 0x00, 0x00
};
