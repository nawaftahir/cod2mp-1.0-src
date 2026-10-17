#include "common_types.h"
#include "imports.h"
#include "headers/PC/cgame_mp/cg_local.h"
#include <string.h>
extern const dvar_t *cg_connectionIconSize;
extern const dvar_t *cg_constantSizeHeadIcons;
extern const dvar_t *cg_scriptIconSize;
extern const dvar_t *cg_voiceIconSize;
extern const dvar_t *cg_youInKillCamSize;

extern const dvar_t *cg_debugPosition;
extern const dvar_t *cg_headIconMinScreenRadius;
extern scr_const_t scr_const;
extern float fabsf(float);

COD2_ASSERT_FIELD(WeaponDef, fAnimHorRotateInc, 0x564);

extern struct DObj_s * Com_GetClientDObj(int handle, int localClientNum);
extern void BG_UpdatePlayerDObj(DObj_s *pDObj, entityState_s *es, clientInfo_t *ci, int attachIgnoreCollision);
extern void BG_PlayerAnimation(const DObj_s *pDObj, entityState_s *es, clientInfo_t *ci);
extern void DObjUpdateClientInfo(struct DObj_s *obj, float dtime);
extern void XAnimClearTreeGoalWeights(XAnimTree *tree, unsigned int animIndex, float blendTime);
extern void XAnimSetCompleteGoalWeight(struct XAnimTree_s *pAnimTree, unsigned int animIndex, float goalWeight, float goalTime, float rate, unsigned int p6, unsigned int p7, int p8);
extern void Com_Printf(const char *fmt, ...);
extern void AnglesToAxis( const vec3_t angles, vec3_t axis[3] );
extern void CG_AddCEntityToScene(const GfxEntity *body, const struct DObj_s *obj, const centity_t *cent);
extern void CG_AddPlayerWeapon(GfxEntity *parent, playerState_t *ps, centity_t *cent, qboolean bDrawGun);
extern qboolean CG_DObjGetWorldTagPos(const centity_t *cent, struct DObj_s *obj, unsigned int tag, float *origin);
extern void CL_DrawSprite(MaterialHandle material, const byte *rgbaColor, const float *origin,
                          float radius, float minScreenRadius, int flags);
extern void XAnimClearTreeGoalWeightsStrict(struct XAnimTree_s *pAnimTree, unsigned int animIndex, float blendTime);
extern int XAnimSetGoalWeight(XAnimTree *tree, unsigned int animIndex, float goalWeight, float goalTime, float rate, unsigned int notifyName, unsigned int notifyType, int bRestart);
extern float XAnimGetWeight(const XAnimTree_s *pAnimTree, unsigned int animIndex);
extern int XAnimGetNumChildren(const struct XAnim_s *pXAnims, unsigned int animIndex);
extern unsigned int XAnimGetChildAt(const struct XAnim_s *pXAnims, unsigned int parentIndex, unsigned int childNum);
extern const char *XAnimGetAnimDebugName(const struct XAnim_s *pXAnims, unsigned int animIndex);
extern void XAnimCalcAbsDelta(struct XAnimTree_s *pAnimTree, unsigned int animIndex, float *rot, float *trans);
extern void Com_Error(errorParm_t level, const char *fmt, ...);
extern WeaponDef * BG_GetWeaponDef(int iWeapon);
extern vec_t vectosignedyaw(vec3_t vec);
extern float RotationToYaw(const vec2_t rot);
extern void YawToAxis(float yaw, vec3_t axis[3]);
extern void MatrixMultiply43(const float in1[4][3], const float in2[4][3], float out[4][3]);
extern void AxisToAngles( vec3_t axis[3], vec3_t angles );
extern void RotateVec2(vec2_t v, float degrees);
extern void CG_Trace(trace_t *result, const vec_t *start, const vec_t *mins, const vec_t *maxs, const vec_t *end, int skipNumber, int mask);
extern const vec3_t vec3_origin;
extern void ConvertQuatToMat(const DObjAnimMat *mat, float axis[3][3]);
extern void Vec3Lerp(const vec3_t start, const vec3_t end, float fraction, vec3_t endpos);
extern DObjAnimMat *CG_DObjGetLocalTagMatrix(const centity_t *cent, struct DObj_s *obj, unsigned int tagName);
extern const char *CL_GetConfigString(int index);
extern MaterialHandle CL_RegisterMaterial(const char *name, int flags);

extern void *imp_cg_headIconMinScreenRadius;
extern byte *cg_pmove_mins;

void CG_UpdatePlayerDObj(centity_t *cent);
void CG_ResetPlayerEntity(centity_t *cent);
static void CG_PlayerFloatSprite(centity_t *cent, MaterialHandle material, float additionalRadiusSize, int height, Bool fixedScreenSize);
void CG_PlayerSprites(centity_t *cent);
void CG_Player(centity_t *cent);
void CG_Corpse(centity_t *cent);

#define CI_STRIDE 1208

void CG_Player(centity_t *cent);

// retail 0x4d5690, unreferenced; name unknown
void CG_SetByte4_4d5690(unsigned char *out, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    out[0] = r;
    out[1] = g;
    out[2] = b;
    out[3] = a;
}

static void CG_PlayerFloatSprite(centity_t *cent, MaterialHandle material, float additionalRadiusSize, int height, Bool fixedScreenSize)
{
    byte *snap;
    struct DObj_s *obj;
    vec3_t origin;
    byte rgbaColor[4];
    float radius;
    int drawFlags;
    float minScreenRadius;

    snap = (*(byte **)&cg->nextSnap);

    if (*(int *)(snap + 0x18) & 0xc00000) {
        if (cent->nextState.number == ((snapshot_t *)snap)->ps.clientNum) {
            if (!cg->renderingThirdPerson)
                return;
        }
    }

    obj = Com_GetClientDObj(cent->nextState.number, cent->localClientNum);
    if (obj) {
        if (CG_DObjGetWorldTagPos(cent, obj, scr_const.j_head, origin)) {
            origin[2] += (float)height + 21.0f;
            goto draw;
        }
    }

    origin[0] = cent->lerpOrigin[0];
    origin[1] = cent->lerpOrigin[1];
    origin[2] = cent->lerpOrigin[2];
    origin[2] += (float)height + 82.0f;

draw:
    if (fixedScreenSize) {
        radius = (additionalRadiusSize + 10.0f) * 0.0043f;
        drawFlags = 0x3000;
    } else {
        radius = additionalRadiusSize + 10.0f;
        drawFlags = 0;
    }

    rgbaColor[0] = 0xFF;
    rgbaColor[1] = 0xFF;
    rgbaColor[2] = 0xFF;
    rgbaColor[3] = 0xFF;

    minScreenRadius = cg_headIconMinScreenRadius->current.value;
    CL_DrawSprite(material, rgbaColor, origin, radius, minScreenRadius, drawFlags);
}

void CG_PlayerSprites(centity_t *cent)
{
    byte *snap;
    int iTeam;
    int localTeam;
    float additionalRadiusSize;
    int eFlags;
    int height;
    MaterialHandle material;

    additionalRadiusSize = 0.0f;

    if (!cg->bgs.clientinfo[cent->nextState.clientNum].infoValid)
        return;
    iTeam = cg->bgs.clientinfo[cent->nextState.clientNum].team;

    snap = (*(byte **)&cg->nextSnap);
    if (!cg->bgs.clientinfo[((snapshot_t *)snap)->ps.clientNum].infoValid)
        return;
    localTeam = cg->bgs.clientinfo[((snapshot_t *)snap)->ps.clientNum].team;

    {
        int headicon = cent->nextState.iHeadIcon;
        if (headicon) {
            int headiconTeam = cent->nextState.iHeadIconTeam;
            if (!headiconTeam || localTeam == 3 || headiconTeam == localTeam) {
                const char *str = CL_GetConfigString(headicon + 30);
                material = CL_RegisterMaterial(str, 7);
                if (material) {
                    CG_PlayerFloatSprite(cent, material, ((((const dvar_t *)cg_scriptIconSize))->current.value), 0,
                                         (Bool)((((const dvar_t *)cg_constantSizeHeadIcons))->current.enabled));
                    additionalRadiusSize = 16.0f + ((((const dvar_t *)cg_scriptIconSize))->current.value);
                }
            }
        }
    }

    if (cent->nextState.number == cg->clientNum) {
        if (cg->inKillCam ) {

            height = (int)additionalRadiusSize;
            material = cgs->media.youInKillCamMaterial;
            CG_PlayerFloatSprite(cent, material, ((((const dvar_t *)cg_youInKillCamSize))->current.value), height, 1);
            return;
        }
    }

    eFlags = cent->nextState.eFlags;
    if (eFlags & 0x80) {
        height = (int)additionalRadiusSize;
        material = cgs->media.connectionMaterial;
        CG_PlayerFloatSprite(cent, material, ((((const dvar_t *)cg_connectionIconSize))->current.value), height, 0);
        return;
    }

    if (iTeam != localTeam && localTeam != 3)
        return;

    if (cent->voiceChatSpriteTime > cg->time ) {
        height = (int)additionalRadiusSize;
        material = cent->voiceChatSprite;
        CG_PlayerFloatSprite(cent, material, ((((const dvar_t *)cg_voiceIconSize))->current.value), height, 0);
        return;
    }

    if (!(eFlags & 0x200000))
        return;

    additionalRadiusSize -= 5.0f;
    height = (int)additionalRadiusSize;
    material = cgs->media.balloonMaterial;
    CG_PlayerFloatSprite(cent, material, ((((const dvar_t *)cg_voiceIconSize))->current.value), height, 0);
}

// ease an anim toward a goal weight over one frame
#define CG_TURRET_GOAL_WEIGHT(anim, weight) do { \
    float rate = (float)fabs(XAnimGetWeight(pAnimTree, (anim)) - (weight)) * (1000.0f / (float)cg->frametime); \
    XAnimSetGoalWeight(pAnimTree, (anim), (weight), rate > 0.0f ? 1.0f / rate : 0.0f, 1.0f, 0, 0, 0); \
} while (0)

// retail 0x4d5970
// inlined com_math VectorCopy: retail loads through the pointer params
static __forceinline void Vec3CopyInl(const vec_t *a, vec_t *out)
{
    out[0] = a[0];
    out[1] = a[1];
    out[2] = a[2];
}

static void CG_PlayerTurretPositionAndBlend(centity_t *cent)
{
    clientInfo_t *ci;
    lerpFrame_t *pLerpAnim;
    centity_t *pTurretCEnt;
    struct DObj_s *turretObj;
    DObjAnimMat *tagMat;
    WeaponDef *weapDef;
    XAnimTree_s *pAnimTree;
    XAnim_s *pXAnims;
    unsigned int baseAnim;
    unsigned int heightAnim;
    unsigned int leafAnim1;
    unsigned int leafAnim2;
    int numVertChildren;
    int numHorChildren;
    int i;
    int iBlend;
    int iPrevBlend;
    float localYaw;
    float tagHeight;
    float fDelta;
    float fBlend;
    float fPrevBlend;
    float fPrevTransZ;
    float fHeightRatio;
    float rot[2];
    float trans[3];
    float vDelta[3];
    float tagAxis[3][3];
    float turretAxis[4][3];
    float axis[4][3];
    float out[4][3];
    float start[3];
    float end[3];
    float endpos[3];
    trace_t trace;

    if (cent->nextState.otherEntityNum < 64)
        return;
    if (cent->nextState.otherEntityNum == 1023)
        return;
    ci = &cg->bgs.clientinfo[cent->nextState.clientNum];
    if (!ci->infoValid)
        return;
    pLerpAnim = &ci->legs;
    if (!pLerpAnim->animationNumber)
        return;
    if (!pLerpAnim->animation)
        return;
    if (!(pLerpAnim->animation->flags & 4))
        return;
    pTurretCEnt = &cg_entities[cent->nextState.otherEntityNum];
    if (!pTurretCEnt->nextValid)
        return;
    turretObj = Com_GetClientDObj(pTurretCEnt->nextState.number, pTurretCEnt->localClientNum);
    if (!turretObj)
        return;
    tagMat = CG_DObjGetLocalTagMatrix(pTurretCEnt, turretObj, scr_const.tag_weapon);
    if (!tagMat)
    {
        Com_Printf("WARNING: aborting player positioning on turret since 'tag_weapon' does not exist\n");
        return;
    }
    if (!cg->frametime)
        return;

    pAnimTree = ci->pXAnimTree;
    weapDef = BG_GetWeaponDef(pTurretCEnt->nextState.weapon);
    pXAnims = cg->bgs.animScriptData.animTree.anims;
    baseAnim = pLerpAnim->animationNumber & ~0x200;
    ConvertQuatToMat(tagMat, tagAxis);
    localYaw = vectosignedyaw(tagAxis[0]);
    AnglesToAxis(pTurretCEnt->lerpAngles, turretAxis);
    turretAxis[3][0] = pTurretCEnt->lerpOrigin[0];
    turretAxis[3][1] = pTurretCEnt->lerpOrigin[1];
    turretAxis[3][2] = pTurretCEnt->lerpOrigin[2];
    vDelta[0] = cent->lerpOrigin[0] - turretAxis[3][0];
    vDelta[1] = cent->lerpOrigin[1] - turretAxis[3][1];
    vDelta[2] = cent->lerpOrigin[2] - turretAxis[3][2];
    tagHeight = vDelta[0] * turretAxis[2][0] + vDelta[1] * turretAxis[2][1] + vDelta[2] * turretAxis[2][2];
    fDelta = tagHeight - tagMat->trans[2];
    XAnimClearTreeGoalWeightsStrict(pAnimTree, baseAnim, 0.0f);
    numVertChildren = XAnimGetNumChildren(pXAnims, baseAnim);
    fPrevTransZ = 0.0f;
    fPrevBlend = 0.0f;
    iPrevBlend = 0;
    leafAnim2 = 0;
    if (!numVertChildren)
        Com_Error(ERR_DROP, "\x15Player anim '%s' has no children", XAnimGetAnimDebugName(pXAnims, baseAnim));
    i = 0;
    do
    {
        heightAnim = XAnimGetChildAt(pXAnims, baseAnim, i);
        XAnimSetGoalWeight(pAnimTree, heightAnim, 1.0f, 1.0f, 1.0f, 0, 0, 0);
        numHorChildren = XAnimGetNumChildren(pXAnims, heightAnim);
        if (!numHorChildren)
            Com_Error(ERR_DROP, "\x15Player anim '%s' has no children", XAnimGetAnimDebugName(pXAnims, heightAnim));
        fBlend = (float)numHorChildren * 0.5f - localYaw / weapDef->fAnimHorRotateInc;
        if (fBlend < 0.0f)
            fBlend = 0.0f;
        else if (fBlend >= (float)(numHorChildren - 1))
            fBlend = (float)(numHorChildren - 1);
        iBlend = (int)fBlend;
        leafAnim1 = XAnimGetChildAt(pXAnims, heightAnim, iBlend);
        fBlend = fBlend - (float)iBlend;
        XAnimSetGoalWeight(pAnimTree, leafAnim1, 1.0f - fBlend, 1.0f, 1.0f, 0, 0, 0);
        if (fBlend != 0.0f)
        {
            leafAnim2 = XAnimGetChildAt(pXAnims, heightAnim, iBlend + 1);
            XAnimSetGoalWeight(pAnimTree, leafAnim2, fBlend, 1.0f, 1.0f, 0, 0, 0);
        }
        XAnimCalcAbsDelta(pAnimTree, heightAnim, rot, trans);
        if (trans[2] >= fDelta)
            break;
        fPrevTransZ = trans[2];
        iPrevBlend = iBlend;
        fPrevBlend = fBlend;
        ++i;
    }
    while (i < numVertChildren);

    XAnimClearTreeGoalWeightsStrict(pAnimTree, baseAnim, 0.0f);
    CG_TURRET_GOAL_WEIGHT(leafAnim1, 1.0f - fBlend);
    if (fBlend != 0.0f)
        CG_TURRET_GOAL_WEIGHT(leafAnim2, fBlend);
    if (i && i != numVertChildren)
    {
        fHeightRatio = (fDelta - fPrevTransZ) / (trans[2] - fPrevTransZ);
        CG_TURRET_GOAL_WEIGHT(heightAnim, fHeightRatio);
        heightAnim = XAnimGetChildAt(pXAnims, baseAnim, i - 1);
        CG_TURRET_GOAL_WEIGHT(heightAnim, 1.0f - fHeightRatio);
        leafAnim1 = XAnimGetChildAt(pXAnims, heightAnim, iPrevBlend);
        CG_TURRET_GOAL_WEIGHT(leafAnim1, 1.0f - fPrevBlend);
        if (fPrevBlend != 0.0f)
        {
            leafAnim2 = XAnimGetChildAt(pXAnims, heightAnim, iPrevBlend + 1);
            CG_TURRET_GOAL_WEIGHT(leafAnim2, fPrevBlend);
        }
    }
    else
    {
        CG_TURRET_GOAL_WEIGHT(heightAnim, 1.0f);
    }

    XAnimCalcAbsDelta(pAnimTree, baseAnim, rot, trans);
    RotateVec2(trans, localYaw);
    axis[3][0] = trans[0] + tagMat->trans[0];
    axis[3][1] = trans[1] + tagMat->trans[1];
    axis[3][2] = tagHeight;
    YawToAxis(RotationToYaw(rot) + localYaw, axis);
    MatrixMultiply43(axis, turretAxis, out);
    AxisToAngles(out, cent->lerpAngles);
    Vec3CopyInl(out[3], cent->lerpOrigin);



    start[0] = cent->lerpOrigin[0];
    start[1] = cent->lerpOrigin[1];
    end[0] = cent->lerpOrigin[0];
    end[1] = cent->lerpOrigin[1];
    end[2] = cent->lerpOrigin[2];
    start[2] = pTurretCEnt->lerpOrigin[2];
    CG_Trace(&trace, start, vec3_origin, vec3_origin, end, cent->nextState.number, 0x2810011);
    if (trace.fraction < 1.0f)
    {
        Vec3Lerp(start, end, trace.fraction, endpos);
        cent->lerpOrigin[2] = endpos[2];
    }
}

void CG_Player(centity_t *cent)
{
    entityState_t *es;
    int iClientNum;
    struct DObj_s *obj;
    GfxEntity body;

    es = &cent->nextState;

    if (es->eFlags & 0x20)
        return;

    if (es->eFlags & 0x20000)
        return;

    {
        const snapshot_t *snap = cg->nextSnap;
        if (snap->ps.pm_flags & 0xc00000) {
            if (es->number == snap->ps.clientNum) {
                if (!cg->renderingThirdPerson)
                    return;
            }
        }
    }

    iClientNum = es->clientNum;

    obj = Com_GetClientDObj(es->number, cent->localClientNum);
    if (!obj)
        return;

    memset(&body, 0, sizeof(body));
    body.materialRGBA[0] = 0xFF;
    body.materialRGBA[1] = 0xFF;
    body.materialRGBA[2] = 0xFF;
    body.materialRGBA[3] = 0xFF;

    {
        clientInfo_t *ci = &cg->bgs.clientinfo[iClientNum];
        BG_PlayerAnimation(obj, es, ci);
    }

    if (es->eFlags & 0x300)
        CG_PlayerTurretPositionAndBlend(cent);

    AnglesToAxis(cent->lerpAngles, body.axis);

    body.origin[0] = cent->lerpOrigin[0];
    body.origin[1] = cent->lerpOrigin[1];
    body.origin[2] = cent->lerpOrigin[2];

    body.lighting.baseCoords[0] = body.origin[0];
    body.lighting.baseCoords[1] = body.origin[1];

    {
        float z = body.origin[2] + es->fTorsoHeight;

        if (es->eFlags & 8)
            z += 12.0f;
        else if (es->eFlags & 4)
            z += 20.0f;
        else
            z += 32.0f;

        body.lighting.baseCoords[2] = z;
    }

    body.reType = RT_DOBJ;

    body.renderFxFlags = 0x80;

    CG_AddCEntityToScene(&body, obj, cent);

    if (es->eFlags & 0x20000)
        return;

    CG_AddPlayerWeapon(&body, 0, cent, 1);
}

void CG_Corpse(centity_t *cent)
{
    entityState_t *es;
    clientInfo_t *ci;
    int clientNum;
    struct DObj_s *obj;
    GfxEntity body;

    es = &cent->nextState;

    if (es->eFlags & 0x20)
        return;

    clientNum = cent->nextState.number;

    ci = &cgs->corpseinfo[clientNum - 0x40];

    obj = Com_GetClientDObj(clientNum, cent->localClientNum);
    BG_UpdatePlayerDObj(obj, es, ci, 0);

    obj = Com_GetClientDObj(cent->nextState.number, cent->localClientNum);
    if (!obj)
        return;

    memset(&body, 0, sizeof(body));

    body.materialRGBA[0] = 0xFF;
    body.materialRGBA[1] = 0xFF;
    body.materialRGBA[2] = 0xFF;
    body.materialRGBA[3] = 0xFF;

    BG_PlayerAnimation(obj, es, ci);

    AnglesToAxis(cent->lerpAngles, body.axis);

    body.origin[0] = cent->lerpOrigin[0];
    body.origin[1] = cent->lerpOrigin[1];
    body.origin[2] = cent->lerpOrigin[2];
    body.lighting.baseCoords[0] = body.origin[0];
    body.lighting.baseCoords[1] = body.origin[1];
    body.lighting.baseCoords[2] = body.origin[2] + es->fTorsoHeight;

    if (es->eFlags & 8) {
        body.lighting.baseCoords[2] += 12.0f;
    } else if (es->eFlags & 4) {
        body.lighting.baseCoords[2] += 20.0f;
    } else {
        body.lighting.baseCoords[2] += 32.0f;
    }

    body.reType = RT_DOBJ;

    body.renderFxFlags = 0x80;

    CG_AddCEntityToScene(&body, obj, cent);
}

void CG_UpdatePlayerDObj(centity_t *cent)
{
    entityState_t *es;
    int clientNum;
    clientInfo_t *ci;
    struct DObj_s *obj;

    if (!cent->nextValid)
        return;

    es = &cent->nextState;
    clientNum = es->clientNum;

    obj = Com_GetClientDObj(clientNum, cent->localClientNum);
    ci = &cg->bgs.clientinfo[clientNum];
    BG_UpdatePlayerDObj(obj, es, ci, 0);
}

void CG_ResetPlayerEntity(centity_t *cent)
{
    clientInfo_t *ci;
    XAnimTree_s *pAnimTree;

    ci = &cg->bgs.clientinfo[cent->nextState.clientNum];

    if (!(cent->nextState.eFlags & 0x20000)) {
        pAnimTree = ci->pXAnimTree;
        XAnimClearTreeGoalWeights(pAnimTree, (*(unsigned short *)&cg->bgs.generic_human.root.index), 0);
        XAnimSetCompleteGoalWeight(pAnimTree, (*(unsigned short *)&cg->bgs.animScriptData.torsoAnim), 0.0f, 0.0f, 1.0f, 0, 0, 0);
        XAnimSetCompleteGoalWeight(pAnimTree, (*(unsigned short *)&cg->bgs.animScriptData.legsAnim), 1.0f, 0.0f, 1.0f, 0, 0, 0);
        XAnimSetCompleteGoalWeight(pAnimTree, (*(unsigned short *)&cg->bgs.animScriptData.turningAnim), 0.0f, 0.0f, 1.0f, 0, 0, 0);

        memset(&ci->legs, 0, sizeof(ci->legs));
        ci->legs.yawAngle = ci->playerAngles[1];
        ci->legs.yawing = 0;
        ci->legs.pitchAngle = 0;
        ci->legs.pitching = 0;

        memset(&ci->torso, 0, sizeof(ci->torso));
        ci->torso.yawAngle = ci->playerAngles[1];
        ci->torso.yawing = 0;
        ci->torso.pitchAngle = ci->playerAngles[0];
        ci->torso.pitching = 0;
    }

    if (cg_debugPosition->current.enabled)
        Com_Printf("%i ResetPlayerEntity yaw=%i\n", cent->nextState.number, ci->torso.yawAngle);
}

