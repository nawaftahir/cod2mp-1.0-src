#include "common_types.h"
#include "imports.h"
#include "bytematch.h"
#include "headers/PC/cgame_mp/cg_local.h"
#include <string.h>
extern itemInfo_t cg_itemsArray[];
extern weaponInfo_t cg_weaponsArray[];
extern gitem_t bg_itemlist[];
extern const int bg_numItems;
extern scr_const_t scr_const;

/* File-scope alias: bound where no local can shadow `scr_const`, so uses below
   always reach the global even inside functions that declare their own `scr_const`. */
static scr_const_t * const scr_const_g = &scr_const;
extern void DObjUpdateClientInfo(struct DObj_s *obj, float timescale);
extern void* MT_Alloc(int numBytes, int type);
extern WeaponDef * BG_GetWeaponDef(int iWeapon);
extern XAnimTree *XAnimCreateTree(XAnim *anims, Alloc_t Alloc);
extern struct XAnim_s *XAnimCreateAnims(const char *debugName, int size, Alloc_t Alloc);
extern void XAnimBlend(XAnim *anims, unsigned int animIndex, const char *name, unsigned int children, unsigned int num, unsigned int flags);
extern XAnimParts *XAnimPrecache(const char *name, Alloc_t Alloc);
extern void *Hunk_AllocXAnimClient(int size);
extern void *Hunk_AllocXAnimPrecache(int size);
extern void XAnimCreate(struct XAnim_s *anims, unsigned int animIndex, const char *name);
extern struct DObj_s * Com_GetClientDObj(int handle, int localClientNum);
extern void Com_ClientDObjCreate(DObjModel_s *dobjModels, unsigned short numModels, struct XAnimTree_s *tree, int handle);
extern qboolean CG_CheckDObjInfoMatches(int iEntNum, int iEntType, struct XModel *pXModel);
extern void CG_SetDObjInfo(int iEntNum, int iEntType, struct XModel *pXModel);
extern void CG_SafeDObjFree(int iEntNum);
extern void CG_RegisterItemVisuals(int itemNum);
extern void Com_Error(errorParm_t code, const char *fmt, ...);
extern float AngleSubtract( float a1, float a2 );
extern float LerpAngle(float from, float to, float frac);
extern qboolean DObjSetControlTagAngles(const DObj_s *obj, int *partBits, unsigned int boneIndex, float *angles);
extern struct XAnimTree_s *DObjGetTree(const struct DObj_s *obj);
extern int XAnimSetCompleteGoalWeightKnobAll(XAnimTree_s *tree, unsigned int animIndex, unsigned int rootIndex, float goalWeight, float goalTime, float rate, unsigned int notifyName, int bRestart);
extern void BG_Player_DoControllers(struct DObj_s *obj, const entityState_t *es, int *partBits, clientInfo_t *ci, int frametime);
extern int DObjGetClientNotifyList(XAnimNotify **notifyList);
extern int stricmp(const char *s1, const char *s2);
extern const char *CL_GetConfigString(int index);
extern Bool CG_PlaySoundOnCurrentLocalClient(int entitynum, const vec_t *origin, const char *aliasname);
extern snd_alias_t *CL_PickSoundAlias(const char *aliasname);
extern int SND_PlayBlendedSoundAliases(const snd_alias_t *pAlias0, const snd_alias_t *pAlias1, float fLerp, int entnum, const vec_t *org, int timeshift, snd_alias_system_t system);
extern GfxEntity *R_AddRefEntityToScene(const GfxEntity *refEnt, GfxModel sceneModel, const centity_t *cent);
extern void R_UpdateXModelBoundsDelayed(GfxEntity *ent);
extern void R_SkinGfxEntityDelayed(GfxEntity *ent);
extern void BG_EvaluateTrajectory( const trajectory_t *tr, int atTime, vec3_t result );
extern int CG_PlaySoundAliasByName(int entitynum, const vec_t *origin, const char *aliasname);
extern void CL_AddLightToScene(const vec_t *org, float radius, float r, float g, float b);
extern int DObjGetBoneIndex(const DObj *obj, unsigned int boneName);
extern qboolean CL_DObjCreateSkelForBone(struct DObj_s *obj, int boneIndex, int localClientNum);
extern qboolean CL_DObjCreateSkelForBones(const struct DObj_s *obj, int *partBits, int localClientNum);
extern DObjAnimMat* DObjGetRotTransArray(const DObj_s *obj);
extern void DObjGetHierarchyBits(struct DObj_s *obj, int boneIndex, int *partBits);
extern void DObjCompleteHierarchyBits(DObj_s *obj, int *partBits);
extern void DObjCalcAnim(const DObj *obj, int *partBits);
extern void DObjCalcSkel(DObj *obj, int *partBits);
extern void AxisCopy( const vec3_t in[3], vec3_t out[3] );
extern void AnglesToAxis( const vec3_t angles, vec3_t axis[3] );
extern void ConvertQuatToMat(const DObjAnimMat *mat, vec3_t axis[3]);
extern void MatrixMultiply(const float in1[3][3], const float in2[3][3], float out[3][3]);
extern void MatrixTransformVector43(const vec3_t in1, const float in2[4][3], vec3_t out);
extern int BG_GetNumWeapons(void);
extern int CG_PlaySoundAlias(int entitynum, const vec_t *origin, snd_alias_list_t *aliasList);
extern int FX_GetBoneIndex(const int entNum, unsigned int bone);
extern void FX_PlayEntityEffect(const EffectTemplate *fx, const vec_t *org, const vec3_t *axis, const FxBoltInfo *bolt);
extern void FX_PlayEffect(const EffectTemplate *fx, const vec_t *org, const vec_t *fwd);
extern void FX_PlayOrientedEffect(const EffectTemplate *fx, const vec_t *org, const vec_t *fwd, const vec_t *up);
extern void FX_PlaySimpleEffect(const EffectTemplate *fx, const vec_t *org);
extern void Com_Printf(const char *fmt, ...);
extern void ByteToDir(const int b, vec3_t dir);
extern float Vec3DistanceSq(const vec3_t v1, const vec3_t v2);
extern vec_t Vec3Normalize( vec3_t v );
extern void CG_Player(centity_t *cent);
extern void CG_Corpse(centity_t *cent);

extern const int boxVerts[24][3];

COD2_ASSERT_FIELD(cg_t, rumbleScale, 0x2c5a0);

struct XAnim_s *CG_GetMG42Anims(centity_t *cent);
static void CG_mg42_DoControllers(const centity_t *cent, int *partBits);
static void CG_RunDObjControllers(const centity_t *cent, int *partBits);
void CG_SetFrameInterpolation(void);
void CG_ProcessClientNoteTracks(int clientNum);
void CG_DObjUpdateInfo(struct DObj_s *obj);
void CG_SoundBlend(centity_t *cent);
static long unsigned int *CG_AllocAnimTree(int size);
void CG_UsedDObjCalcPose(const centity_t *cent);
void CG_CullIn(const centity_t *cent);
void CG_AddCEntityToScene(const GfxEntity *ent, const struct DObj_s *obj, const centity_t *cent);
static void CG_InterpolateEntityPosition(centity_t *cent);
qboolean CG_DObjGetViewModelTagPos(struct DObj_s *obj, unsigned int tagName, vec_t *pos);
void CG_DObjCalcBoneGeneric(int handle, int localClientNum, int boneIndex);
void CG_AdjustPositionForMover(const vec_t *in, int moverNum, int fromTime, int toTime, vec_t *out, vec_t *outDeltaAngles);
void CG_CalcEntityLerpPositions(centity_t *cent);
void CG_EntityEffects(centity_t *cent);
static struct DObj_s *CG_PreProcess_GetDObj(int iEntNum, int iEntType, struct XModel *model);
void CG_DObjCalcPose(const centity_t *cent, const struct DObj_s *obj, int *partBits);
void CG_DObjCalcBone(const centity_t *cent, struct DObj_s *obj, int boneIndex);
DObjAnimMat *CG_DObjGetLocalTagMatrix(const centity_t *cent, struct DObj_s *obj, unsigned int tagName);
qboolean CG_DObjGetWorldTagPos(const centity_t *cent, struct DObj_s *obj, unsigned int tagName, vec_t *pos);
void CG_General(centity_t *cent);
void CG_Item(centity_t *cent);
void CG_Missile(centity_t *cent);
void CG_ScriptMover(centity_t *cent);
void CG_ProcessEntity(centity_t *cent);
void CG_AddPacketEntities(void);

struct XAnim_s *CG_GetMG42Anims(centity_t *cent);
static void CG_mg42_DoControllers(const centity_t *cent, int *partBits);
void CG_SetFrameInterpolation(void);
void CG_ProcessClientNoteTracks(int clientNum);
void CG_DObjUpdateInfo(struct DObj_s *obj);
void CG_SoundBlend(centity_t *cent);
static long unsigned int *CG_AllocAnimTree(int size);
void CG_UsedDObjCalcPose(const centity_t *cent);
void CG_CullIn(const centity_t *cent);
void CG_AddCEntityToScene(const GfxEntity *ent, const struct DObj_s *obj, const centity_t *cent);
static void CG_InterpolateEntityPosition(centity_t *cent);
qboolean CG_DObjGetViewModelTagPos(struct DObj_s *obj, unsigned int tagName, vec_t *pos);
void CG_DObjCalcBoneGeneric(int handle, int localClientNum, int boneIndex);
void CG_AdjustPositionForMover(const vec_t *in, int moverNum, int fromTime, int toTime, vec_t *out, vec_t *outDeltaAngles);
void CG_CalcEntityLerpPositions(centity_t *cent);
void CG_EntityEffects(centity_t *cent);
static struct DObj_s *CG_PreProcess_GetDObj(int iEntNum, int iEntType, struct XModel *model);
void CG_DObjCalcPose(const centity_t *cent, const struct DObj_s *obj, int *partBits);
void CG_DObjCalcBone(const centity_t *cent, struct DObj_s *obj, int boneIndex);
DObjAnimMat *CG_DObjGetLocalTagMatrix(const centity_t *cent, struct DObj_s *obj, unsigned int tagName);
qboolean CG_DObjGetWorldTagPos(const centity_t *cent, struct DObj_s *obj, unsigned int tagName, vec_t *pos);
void CG_General(centity_t *cent);
void CG_Item(centity_t *cent);
void CG_Missile(centity_t *cent);
void CG_ScriptMover(centity_t *cent);
void CG_ProcessEntity(centity_t *cent);
void CG_AddPacketEntities(void);

static __forceinline void EntVectorCopy(const vec_t *a, vec_t *b);

static clientInfo_t *CG_CorpseInfoForEntityNum(int entNum)
{
    return &cgs->corpseinfo[entNum - 0x40];
}

static void CG_LockLightingOrigin(centity_t *cent, GfxEntity *ent)
{
    if (cent->nextState.eFlags & 0x400) {
        if (cent->lightingOrigin[0] == 0.0f &&
            cent->lightingOrigin[1] == 0.0f &&
            cent->lightingOrigin[2] == 0.0f) {
            EntVectorCopy(cent->lerpOrigin, cent->lightingOrigin);
        }

        EntVectorCopy(cent->lightingOrigin, ent->lighting.origin);
        ent->renderFxFlags |= 0x80;
    } else {
        cent->lightingOrigin[0] = 0.0f;
        cent->lightingOrigin[1] = 0.0f;
        cent->lightingOrigin[2] = 0.0f;
    }
}

void CG_General(centity_t *cent)
{
    entityState_t *s1 = &cent->nextState;
    struct DObj_s *obj;
    GfxEntity ent;

    if (s1->eFlags & 0x20)
        return;

    obj = CG_PreProcess_GetDObj(s1->number, s1->eType, cgs->gameModels[s1->index.xmodel]);
    if (!obj)
        return;

    memset(&ent, 0, sizeof(ent));
    ent.origin[0] = cent->lerpOrigin[0];
    ent.origin[1] = cent->lerpOrigin[1];
    ent.origin[2] = cent->lerpOrigin[2];
    AnglesToAxis(cent->lerpAngles, ent.axis);
    ent.reType = (refEntityType_t)(0);

    CG_LockLightingOrigin(cent, &ent);
    CG_AddCEntityToScene(&ent, obj, cent);
}

// Original name unknown.
qboolean CG_FlareInViewCone(const centity_t *cent, int atTime)
{
    vec3_t origin;
    vec3_t dir;
    float dist;
    float dot;

    BG_EvaluateTrajectory(&cent->currentState.pos, atTime, origin);
    dir[0] = cg->refdef.vieworg[0] - origin[0];
    dir[1] = cg->refdef.vieworg[1] - origin[1];
    dir[2] = cg->refdef.vieworg[2] - origin[2];
    dist = Vec3Normalize(dir);
    if (dist > 255.0f)
        return 0;

    dot = cg->refdef.viewaxis[0][2] * dir[2] + cg->refdef.viewaxis[0][1] * dir[1] + cg->refdef.viewaxis[0][0] * dir[0];
    return !(dot > -0.94f - (dist * (1.0f / 255.0f)) * 0.057f);
}

void CG_Item(centity_t *cent)
{
    entityState_t *s1 = &cent->nextState;
    itemInfo_t *itemInfo;
    gitem_t *item;
    struct DObj_s *obj;
    GfxEntity ent;

    if (s1->index.item >= bg_numItems)
        Com_Error(ERR_DROP, (const char *)"\x15"
                                   "Bad item index %i on entity",
                  s1->index.item);

    if (s1->eFlags & 0x20)
        return;

    item = &((gitem_t *)((void *)&bg_itemlist))[s1->index.item];
    itemInfo = &(cg_itemsArray)[s1->index.item];
    if (!itemInfo->registered) {
        CG_RegisterItemVisuals(s1->index.item);
        return;
    }

    if (!itemInfo->models[0])
        Com_Error(ERR_DROP, (const char *)"\x15No XModel loaded for item index %i (%s)", s1->index.item, item->pickup_name);

    obj = CG_PreProcess_GetDObj(s1->number, s1->eType, itemInfo->models[0]);
    if (!obj)
        return;

    memset(&ent, 0, sizeof(ent));
    AnglesToAxis(cent->lerpAngles, ent.axis);
    ent.origin[0] = cent->lerpOrigin[0];
    ent.origin[1] = cent->lerpOrigin[1];
    ent.origin[2] = cent->lerpOrigin[2];
    ent.reType = (refEntityType_t)(0);
    CG_AddCEntityToScene(&ent, obj, cent);
}

void CG_EntityEffects(centity_t *cent)
{
    int loopSound = cent->nextState.loopSound;

    if (loopSound) {
        if (cent->nextState.solid == 0xffffff) {
            vec3_t origin;
            vec_t *midpoint = cgs->inlineModelMidpoints[cent->nextState.index.brushmodel];

            origin[0] = cent->lerpOrigin[0] + midpoint[0];
            origin[1] = cent->lerpOrigin[1] + midpoint[1];
            origin[2] = cent->lerpOrigin[2] + midpoint[2];
            CG_PlaySoundAliasByName(cent->nextState.number, origin, CL_GetConfigString(loopSound + 0x24e));
        } else {
            CG_PlaySoundAliasByName(cent->nextState.number, cent->lerpOrigin, CL_GetConfigString(loopSound + 0x24e));
        }
    }

    if (cent->nextState.constantLight) {
        int cl;
        int i, r, g, b;

        cl = cent->nextState.constantLight;
        r = cl & 255;
        g = (cl >> 8) & 255;
        b = (cl >> 16) & 255;
        i = ((cl >> 24) & 255) * 4;
        CL_AddLightToScene(cent->lerpOrigin, (float)i, (float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f);
    }
}

struct XAnim_s *CG_GetMG42Anims(centity_t *cent)
{
    WeaponDef *weapDef;
    struct XAnim_s *pAnims;

    weapDef = (WeaponDef *)(BG_GetWeaponDef(cent->nextState.weapon));

    pAnims = XAnimCreateAnims((const char *)"MG42", 3, Hunk_AllocXAnimClient);

    XAnimBlend(pAnims, 0, (const char *)"root", 1, 2, 0);

    XAnimPrecache(weapDef->szXAnims[1], Hunk_AllocXAnimPrecache);

    XAnimCreate(pAnims, 1, weapDef->szXAnims[1]);

    XAnimPrecache(weapDef->szXAnims[3], Hunk_AllocXAnimPrecache);

    XAnimCreate(pAnims, 2, weapDef->szXAnims[3]);

    return pAnims;
}

static void CG_mg42_DoControllers(const centity_t *cent, int *partBits)
{
    playerState_t *ps;
    const entityState_t *s1;
    struct DObj_s *obj;
    float angles[3];
    const scr_const_t *scr;
    struct XAnimTree_s *tree;
    int animIndex;

    s1 = (const entityState_t *)((const byte *)cent + 0xf0);

    ps = &cg->predictedPlayerState;

    obj = Com_GetClientDObj(s1->number, cent->localClientNum);

    if ((ps->eFlags & 0x300) && ps->viewlocked_entNum == s1->number) {

        angles[0] = AngleSubtract(cg->refdefViewAngles[0], cent->lerpAngles[0]);
        angles[1] = AngleSubtract(cg->refdefViewAngles[1], cent->lerpAngles[1]);
        angles[2] = 0.0f;
    } else {

        angles[0] = LerpAngle(s1->angles2[0], s1->angles2[0], cg->frameInterpolation);
        angles[1] = LerpAngle(s1->angles2[1], s1->angles2[1], cg->frameInterpolation);
        angles[2] = 0.0f;
    }

    scr = (const scr_const_t *)scr_const_g;
    DObjSetControlTagAngles(obj, partBits, scr->tag_aim, angles);

    DObjSetControlTagAngles(obj, partBits, scr->tag_aim_animated, angles);

    angles[0] = LerpAngle(s1->angles2[2], s1->angles2[2], cg->frameInterpolation);
    angles[1] = 0.0f;

    DObjSetControlTagAngles(obj, partBits, scr->tag_flash, angles);

    tree = (struct XAnimTree_s *)DObjGetTree(obj);

    if (cg->predictedPlayerState.eFlags & 0x300) {
        if (cg->predictedPlayerState.viewlocked_entNum == s1->number) {
            animIndex = 1;
        } else {
            if (*(byte *)&cent->nextState.eFlags & 0x40) {
                animIndex = 2;
            } else {
                animIndex = 1;
            }
        }
    } else {
        if (*(byte *)&cent->nextState.eFlags & 0x40) {
            animIndex = 2;
        } else {
            animIndex = 1;
        }
    }

    {
        float goalWeight = 1.0f;
        float goalTime = 0.1f;
        float rate = 1.0f;
        XAnimSetCompleteGoalWeightKnobAll(tree, animIndex, 0, goalWeight, goalTime, rate, 0, 0);
    }
}

static void CG_Turret(centity_t *cent)
{
    entityState_t *s1 = &cent->nextState;
    struct DObj_s *obj;
    GfxEntity ent;

    if (s1->eFlags & 0x20)
        return;

    obj = CG_PreProcess_GetDObj(s1->number, s1->eType, cgs->gameModels[s1->index.xmodel]);
    if (!obj)
        return;

    memset(&ent, 0, sizeof(ent));
    ent.origin[0] = cent->lerpOrigin[0];
    ent.origin[1] = cent->lerpOrigin[1];
    ent.origin[2] = cent->lerpOrigin[2];
    ent.lighting.origin[0] = ent.origin[0];
    ent.lighting.origin[1] = ent.origin[1];
    ent.lighting.origin[2] = ent.origin[2] + 32.0f;
    ent.renderFxFlags = 0x80;
    AnglesToAxis(cent->lerpAngles, ent.axis);
    ent.reType = (refEntityType_t)(0);
    CG_AddCEntityToScene(&ent, obj, cent);
}

void CG_Missile(centity_t *cent)
{
    entityState_t *s1 = &cent->nextState;
    weaponInfo_t *weapInfo;
    struct DObj_s *obj;
    GfxEntity ent;

    if (s1->eFlags & 0x20)
        return;

    if (s1->time > cg->time)
        return;

    if (s1->weapon > BG_GetNumWeapons())
        s1->weapon = 0;

    weapInfo = &(cg_weaponsArray)[s1->weapon];
    if (weapInfo->missileSound)
        CG_PlaySoundAlias(s1->number, cent->lerpOrigin, weapInfo->missileSound);

    obj = CG_PreProcess_GetDObj(s1->number, s1->eType, weapInfo->missileSurfModel);
    if (!obj)
        return;

    if (weapInfo->projTrailEffect && !cent->bTrailMade) {
        FxBoltInfo bolt;

        bolt.dobjHandle = s1->number;
        bolt.boneIndex = FX_GetBoneIndex(s1->number, scr_const.tag_flash);
        if (bolt.boneIndex >= 0)
            FX_PlayEntityEffect(weapInfo->projTrailEffect, cent->lerpOrigin, 0, &bolt);

        cent->bTrailMade = 1;
    }

    if (weapInfo->missileDlight != 0.0f) {
        CL_AddLightToScene(cent->lerpOrigin, weapInfo->missileDlight,
                           weapInfo->missileDlightColor[0],
                           weapInfo->missileDlightColor[1],
                           weapInfo->missileDlightColor[2]);
    }

    memset(&ent, 0, sizeof(ent));
    ent.origin[0] = cent->lerpOrigin[0];
    ent.origin[1] = cent->lerpOrigin[1];
    ent.origin[2] = cent->lerpOrigin[2];
    ent.renderFxFlags = weapInfo->missileRenderfx | 0x40;
    AnglesToAxis(cent->lerpAngles, ent.axis);
    ent.reType = (refEntityType_t)(0);
    CG_AddCEntityToScene(&ent, obj, cent);
}

void CG_ScriptMover(centity_t *cent)
{
    entityState_t *s1 = &cent->nextState;
    GfxEntity ent;

    if (s1->eFlags & 0x20)
        return;

    memset(&ent, 0, sizeof(ent));
    ent.origin[0] = cent->lerpOrigin[0];
    ent.origin[1] = cent->lerpOrigin[1];
    ent.origin[2] = cent->lerpOrigin[2];
    AnglesToAxis(cent->lerpAngles, ent.axis);
    ent.renderFxFlags = 0x40;

    if (s1->solid != 0xffffff) {
        struct DObj_s *obj = CG_PreProcess_GetDObj(s1->number, s1->eType, cgs->gameModels[s1->index.xmodel]);
        if (!obj)
            return;

        ent.reType = (refEntityType_t)(0);
        CG_LockLightingOrigin(cent, &ent);
        CG_AddCEntityToScene(&ent, obj, cent);
    } else {
        GfxModel model;

        ent.reType = (refEntityType_t)(3);
        model.bmodel = cgs->inlineDrawModel[s1->index.brushmodel];
        R_AddRefEntityToScene(&ent, model, 0);
    }
}

void CG_AdjustPositionForMover(const vec_t *in, int moverNum, int fromTime, int toTime, vec_t *out, vec_t *outDeltaAngles)
{
    centity_t *cent;
    vec3_t oldOrigin;
    vec3_t oldAngles;
    vec3_t origin;
    vec3_t angles;
    vec3_t deltaOrigin;
    vec3_t deltaAngles;

    if (outDeltaAngles) {
        outDeltaAngles[0] = 0.0f;
        outDeltaAngles[1] = 0.0f;
        outDeltaAngles[2] = 0.0f;
    }

    if (moverNum > 0 && moverNum < 1022) {
        cent = &(((centity_t *)cg_entities))[moverNum];
        if (cent->nextState.eType != 6) {
            out[0] = in[0];
            out[1] = in[1];
            out[2] = in[2];
            return;
        }
        BG_EvaluateTrajectory(&cent->currentState.pos, fromTime, oldOrigin);
        BG_EvaluateTrajectory(&cent->currentState.apos, fromTime, oldAngles);
        BG_EvaluateTrajectory(&cent->currentState.pos, toTime, origin);
        BG_EvaluateTrajectory(&cent->currentState.apos, toTime, angles);

        deltaOrigin[0] = origin[0] - oldOrigin[0];
        deltaOrigin[1] = origin[1] - oldOrigin[1];
        deltaOrigin[2] = origin[2] - oldOrigin[2];
        deltaAngles[0] = angles[0] - oldAngles[0];
        deltaAngles[1] = angles[1] - oldAngles[1];
        deltaAngles[2] = angles[2] - oldAngles[2];
        out[0] = in[0] + deltaOrigin[0];
        out[1] = in[1] + deltaOrigin[1];
        out[2] = in[2] + deltaOrigin[2];

        if (outDeltaAngles) {
            outDeltaAngles[0] = deltaAngles[0];
            outDeltaAngles[1] = deltaAngles[1];
            outDeltaAngles[2] = deltaAngles[2];
        }
    } else {
        out[0] = in[0];
        out[1] = in[1];
        out[2] = in[2];
    }
}

void CG_SetFrameInterpolation(void)
{
    int snapTime = cg->snap->serverTime;
    int nextSnapTime = cg->nextSnap->serverTime;
    int delta = nextSnapTime - snapTime;

    if (delta) {
        cg->frameInterpolation = (float)(cg->time - snapTime) / (float)delta;
        if (cg->frameInterpolation < 0.0f)
            cg->frameInterpolation = 0.0f;
    } else {
        cg->frameInterpolation = 0.0f;
    }
}

void CG_ProcessClientNoteTracks(int clientNum)
{
    XAnimNotify *noteList;
    int listSize;
    int i;

    if (clientNum < 0 || clientNum >= 64)
        return;

    listSize = DObjGetClientNotifyList(&noteList);
    if (listSize <= 0)
        return;

    for (i = 0; i < listSize; ++i)
    {
        switch (noteList[i].type)
        {
        case 1:
            if (!stricmp(noteList[i].name, "anim_gunhand = \"left\""))
            {
                cg->bgs.clientinfo[clientNum].leftHandGun = 1;
                cg->bgs.clientinfo[clientNum].dobjDirty = 1;
            }
            else if (!stricmp(noteList[i].name, "anim_gunhand = \"right\""))
            {
                cg->bgs.clientinfo[clientNum].leftHandGun = 0;
                cg->bgs.clientinfo[clientNum].dobjDirty = 1;
            }
            break;
        }
    }
}

void CG_AddPacketEntities(void)
{
    int num;
    int viewlocked_entNum;

    *(int *)&cg->rumbleScale = 0;

    cg->autoAnglesSlow[0] = 0.0f;
    cg->autoAnglesSlow[1] = (float)(360 * (cg->time & 0xfff)) / 4095.0f;
    cg->autoAnglesSlow[2] = 0.0f;

    cg->autoAngles[0] = 0.0f;
    cg->autoAngles[1] = (float)(360 * (cg->time & 0x7ff)) / 2048.0f;
    cg->autoAngles[2] = 0.0f;

    cg->autoAnglesFast[0] = 0.0f;
    cg->autoAnglesFast[1] = (float)(360 * (cg->time & 0x3ff)) / 1024.0f;
    cg->autoAnglesFast[2] = 0.0f;

    AnglesToAxis(cg->autoAnglesSlow, cg->autoAxisSlow);
    AnglesToAxis(cg->autoAngles, cg->autoAxis);
    AnglesToAxis(cg->autoAnglesFast, cg->autoAxisFast);

    viewlocked_entNum = cg->predictedPlayerState.viewlocked_entNum;
    for (num = 0; num < cg->nextSnap->numEntities; ++num) {
        if (cg->nextSnap->entities[num].number != viewlocked_entNum) {
            centity_t *cent;

            cent = &(((centity_t *)cg_entities))[cg->nextSnap->entities[num].number];
            if (cent->nextState.eType < 10)
                CG_ProcessEntity(cent);
        }
    }
}

void CG_DObjUpdateInfo(struct DObj_s *obj)
{
    float timescale = (float)(cg->frametime) * 0.001f;
    DObjUpdateClientInfo(obj, timescale);
}

DObjAnimMat *CG_DObjGetLocalTagMatrix(const centity_t *cent, struct DObj_s *obj, unsigned int tagName)
{
    DObjAnimMat *mat;
    int boneIndex;

    boneIndex = DObjGetBoneIndex(obj, tagName);
    if (boneIndex < 0)
        return 0;

    CG_DObjCalcBone(cent, obj, boneIndex);
    mat = DObjGetRotTransArray(obj);
    return mat ? &mat[boneIndex] : 0;
}

// retail 0x4c6ae0, unreferenced (G_DObjGetWorldTagMatrix twin)
qboolean CG_DObjGetWorldTagMatrix(const centity_t *cent, struct DObj_s *obj, unsigned int tagName, float tagMat[4][3])
{
    DObjAnimMat *mat;
    float ent_axis[4][3];
    float axis[3][3];

    mat = CG_DObjGetLocalTagMatrix(cent, obj, tagName);

    if (!mat)
        return 0;

    AnglesToAxis(cent->lerpAngles, ent_axis);
    ent_axis[3][0] = cent->lerpOrigin[0];
    ent_axis[3][1] = cent->lerpOrigin[1];
    ent_axis[3][2] = cent->lerpOrigin[2];

    ConvertQuatToMat(mat, axis);

    MatrixMultiply(axis, ent_axis, tagMat);
    MatrixTransformVector43(mat->trans, ent_axis, tagMat[3]);

    return 1;
}

qboolean CG_DObjGetWorldTagPos(const centity_t *cent, struct DObj_s *obj, unsigned int tagName, vec_t *pos)
{
    DObjAnimMat *mat;
    vec3_t ent_axis[4];

    mat = CG_DObjGetLocalTagMatrix(cent, obj, tagName);
    if (!mat)
        return 0;

    AnglesToAxis(cent->lerpAngles, ent_axis);
    ent_axis[3][0] = cent->lerpOrigin[0];
    ent_axis[3][1] = cent->lerpOrigin[1];
    ent_axis[3][2] = cent->lerpOrigin[2];
    pos[0] = mat->trans[1] * ent_axis[1][0] + mat->trans[0] * ent_axis[0][0] + mat->trans[2] * ent_axis[2][0] + ent_axis[3][0];
    pos[1] = mat->trans[1] * ent_axis[1][1] + mat->trans[0] * ent_axis[0][1] + mat->trans[2] * ent_axis[2][1] + ent_axis[3][1];
    pos[2] = mat->trans[1] * ent_axis[1][2] + mat->trans[0] * ent_axis[0][2] + mat->trans[2] * ent_axis[2][2] + ent_axis[3][2];
    return 1;
}

qboolean CG_DObjGetViewModelTagPos(struct DObj_s *obj, unsigned int tagName, vec_t *pos)
{
    DObjAnimMat *mat;
    vec3_t ent_axis[4];
    int partBits[4];
    int boneIndex;

    boneIndex = DObjGetBoneIndex(obj, tagName);
    if (boneIndex < 0)
        return 0;

    if (!CL_DObjCreateSkelForBone(obj, boneIndex, 0)) {
        DObjGetHierarchyBits(obj, boneIndex, partBits);
        DObjCalcAnim(obj, partBits);
        DObjCalcSkel(obj, partBits);
    }

    mat = DObjGetRotTransArray(obj);
    if (!mat)
        return 0;

    AxisCopy(cg->viewModelAxis, ent_axis);
    {
        float *vmo = &cg->viewModelOrigin[0];
        ent_axis[3][0] = vmo[0];
        ent_axis[3][1] = vmo[1];
        ent_axis[3][2] = vmo[2];
    }

    MatrixTransformVector43(mat[boneIndex].trans, ent_axis, pos);
    return 1;
}

void CG_SoundBlend(centity_t *cent)
{
    entityState_t *nextState = &cent->nextState;
    snd_alias_t *alias0;
    snd_alias_t *alias1;
    float lerp;
    int ep1;

    if (!nextState->eventParms[0])
        return;
    ep1 = nextState->eventParms[1];
    if (!ep1)
        return;

    if (!CG_PlaySoundOnCurrentLocalClient(nextState->number, cent->lerpOrigin,
                                          CL_GetConfigString(nextState->eventParms[0] + 0x24e)))
        return;

    alias0 = CL_PickSoundAlias(CL_GetConfigString(nextState->eventParms[0] + 0x24e));
    alias1 = CL_PickSoundAlias(CL_GetConfigString(nextState->eventParms[1] + 0x24e));
    if (!alias0 || !alias1)
        return;

    lerp = cent->currentState.leanf +
           (nextState->leanf - cent->currentState.leanf) * cg->frameInterpolation;
    SND_PlayBlendedSoundAliases(alias0, alias1, lerp, nextState->number, cent->lerpOrigin, 0, (snd_alias_system_t)(1));
}

static void CG_PlayLoopedFx(centity_t *cent)
{
    entityState_t *s1 = &cent->nextState;
    int repeat;
    int fxIndex;
    EffectTemplate *fx;
    const vec_t *forward;
    float cullDist;

    if (cg->time >= cent->dl_time) {
        repeat = (int)s1->angles2[1];
        if (cg->time - cent->dl_time < repeat)
            return;

        do {
            cent->dl_time += repeat;
        } while (cg->time - cent->dl_time >= repeat);
    } else {
        cent->dl_time = cg->time;
    }

    cullDist = s1->angles2[0];
    if (cullDist != 0.0f) {
        if (Vec3DistanceSq(cg->refdef.vieworg, cent->lerpOrigin) >= cullDist * cullDist)
            return;
    }

    fxIndex = s1->scale;
    if (fxIndex > 0 && fxIndex < 64) {
        fx = cgs->fxs[fxIndex];
        forward = s1->origin2;
        if (forward[0] * forward[0] + forward[1] * forward[1] + forward[2] * forward[2] != 0.0f) {
            if (!s1->eventParm) {
                FX_PlayEffect(fx, cent->lerpOrigin, forward);
            } else {
                vec3_t up;
                float dot;
                float scale;

                ByteToDir(s1->eventParm, up);
                dot = (up[2] * forward[2] + up[1] * forward[1]) + up[0] * forward[0];
                scale = -dot;
                up[0] = scale * forward[0] + up[0];
                up[1] = scale * forward[1] + up[1];
                up[2] = scale * forward[2] + up[2];
                Vec3Normalize(up);
                FX_PlayOrientedEffect(fx, cent->lerpOrigin, forward, up);
            }
        } else {
            FX_PlaySimpleEffect(fx, cent->lerpOrigin);
        }
    } else {
        Com_Printf("ERROR: CG_PlayFx called with invalid effect id %i\n", fxIndex);
    }
}

static void CG_InterpolateEntityPosition(centity_t *cent)
{
    clientInfo_t *ci;
    vec3_t current;
    vec3_t next;
    float f = cg->frameInterpolation;

    BG_EvaluateTrajectory(&cent->currentState.pos, cg->snap->serverTime, current);
    BG_EvaluateTrajectory(&cent->nextState.pos, cg->nextSnap->serverTime, next);
    cent->lerpOrigin[0] = current[0] + (next[0] - current[0]) * f;
    cent->lerpOrigin[1] = current[1] + (next[1] - current[1]) * f;
    cent->lerpOrigin[2] = current[2] + (next[2] - current[2]) * f;

    BG_EvaluateTrajectory(&cent->currentState.apos, cg->snap->serverTime, current);
    BG_EvaluateTrajectory(&cent->nextState.apos, cg->nextSnap->serverTime, next);
    cent->lerpAngles[0] = LerpAngle(current[0], next[0], f);
    cent->lerpAngles[1] = LerpAngle(current[1], next[1], f);
    cent->lerpAngles[2] = LerpAngle(current[2], next[2], f);

    if (cent->nextState.eType == 1) {
        ci = &cg->bgs.clientinfo[cent->nextState.clientNum];
        ci->lerpMoveDir = LerpAngle(cent->currentState.angles2[1], cent->nextState.angles2[1], f);
        ci->playerAngles[0] = cent->lerpAngles[0];
        ci->playerAngles[1] = cent->lerpAngles[1];
        ci->playerAngles[2] = cent->lerpAngles[2];
        cent->lerpAngles[0] = 0.0f;
        cent->lerpAngles[2] = 0.0f;
        ci->lerpLean = LerpAngle(cent->currentState.leanf, cent->nextState.leanf, f);
    }
}

// com_math VectorCopy (not included here); the pointer param shapes the loads
static __forceinline void EntVectorCopy(const vec_t *a, vec_t *b)
{
    b[0] = a[0];
    b[1] = a[1];
    b[2] = a[2];
}

void CG_CalcEntityLerpPositions(centity_t *cent)
{
    clientInfo_t *ci;

    if (cent->currentState.pos.trType == TR_INTERPOLATE) {
        CG_InterpolateEntityPosition(cent);
        return;
    }

    if (cent->currentState.pos.trType == TR_LINEAR_STOP && cent->currentState.number < 64) {
        CG_InterpolateEntityPosition(cent);
        return;
    }

    BG_EvaluateTrajectory(&cent->currentState.pos, cg->time, cent->lerpOrigin);
    BG_EvaluateTrajectory(&cent->currentState.apos, cg->time, cent->lerpAngles);

    if (cent->nextState.eType == 1) {
        ci = &cg->bgs.clientinfo[cent->nextState.clientNum];
        ci->lerpMoveDir = cent->nextState.angles2[1];
        EntVectorCopy(cent->lerpAngles, ci->playerAngles);
        cent->lerpAngles[2] = 0.0f;
        cent->lerpAngles[0] = 0.0f;
        ci->lerpLean = cent->nextState.leanf;
    } else if (cent->nextState.eType == 2) {
        ci = CG_CorpseInfoForEntityNum(cent->nextState.number);
        ci->lerpMoveDir = cent->nextState.angles2[1];
        EntVectorCopy(cent->lerpAngles, ci->playerAngles);
        cent->lerpAngles[0] = 0.0f;
        cent->lerpAngles[2] = 0.0f;
        ci->lerpLean = cent->nextState.leanf;
    }

    if (cent != &cg->predictedPlayerEntity)
        CG_AdjustPositionForMover(cent->lerpOrigin, cent->currentState.groundEntityNum, cg->snap->serverTime, cg->time, cent->lerpOrigin, NULL);
}

// retail 0x4c78e0, unreferenced; name unknown
struct XAnim_s *CG_GetEntityAnims_4c78e0(int eType, int entnum)
{
    if (eType != 9)
        return 0;
    return CG_GetMG42Anims(&(((centity_t *)cg_entities))[entnum]);
}

static long unsigned int *CG_AllocAnimTree(int size)
{
    return (long unsigned int *)MT_Alloc(size, 5);
}

static struct DObj_s *CG_PreProcess_GetDObj(int iEntNum, int iEntType, struct XModel *model)
{
    DObjModel_s dobjModel;
    struct XAnimTree_s *tree;
    struct XAnim_s *anims;
    struct DObj_s *pDObj;

    pDObj = (struct DObj_s *)Com_GetClientDObj(iEntNum, 0);
    if (pDObj) {
        if (model && CG_CheckDObjInfoMatches(iEntNum, iEntType, model))
            return pDObj;

        CG_SafeDObjFree(iEntNum);
        pDObj = 0;
    }

    if (!model)
        return pDObj;

    if (iEntType == 9 && (anims = CG_GetMG42Anims(&(((centity_t *)cg_entities))[iEntNum])) != 0)
        tree = (struct XAnimTree_s *)XAnimCreateTree((XAnim *)anims, (Alloc_t)CG_AllocAnimTree);
    else
        tree = 0;

    (((centity_t *)cg_entities))[iEntNum].tree = tree;
    dobjModel.model = model;
    dobjModel.boneName = 0;
    dobjModel.ignoreCollision = 1;
    Com_ClientDObjCreate(&dobjModel, 1, tree, iEntNum);
    CG_SetDObjInfo(iEntNum, iEntType, model);
    return (struct DObj_s *)Com_GetClientDObj(iEntNum, 0);
}

// Name from CoD4; original CoD2 name unknown.
static void CG_Player_DoControllers(const centity_t *cent, int *partBits)
{
    const entityState_t *es = &cent->nextState;
    struct DObj_s *obj = (struct DObj_s *)Com_GetClientDObj(es->number, cent->localClientNum);
    clientInfo_t *ci = &cg->bgs.clientinfo[es->clientNum];

    if (ci->infoValid)
        BG_Player_DoControllers(obj, es, partBits, ci, cg->frametime);
}

static void CG_RunDObjControllers(const centity_t *cent, int *partBits)
{
    switch (cent->nextState.eType) {
    case 1:
        CG_Player_DoControllers(cent, partBits);
        break;
    case 9:
        CG_mg42_DoControllers(cent, partBits);
        break;
    }
}

void CG_DObjCalcPose(const centity_t *cent, const struct DObj_s *obj, int *partBits)
{
    int completePartBits[4];

    memcpy(completePartBits, partBits, sizeof(completePartBits));

    DObjCompleteHierarchyBits((DObj_s *)obj, completePartBits);
    if (CL_DObjCreateSkelForBones(obj, completePartBits, cent->localClientNum))
        return;

    DObjCalcAnim(obj, completePartBits);
    CG_RunDObjControllers(cent, completePartBits);
    DObjCalcSkel((DObj *)obj, completePartBits);
}

void CG_DObjCalcBone(const centity_t *cent, struct DObj_s *obj, int boneIndex)
{
    int partBits[4];

    if (CL_DObjCreateSkelForBone(obj, boneIndex, cent->localClientNum))
        return;

    DObjGetHierarchyBits(obj, boneIndex, partBits);
    DObjCalcAnim(obj, partBits);
    CG_RunDObjControllers(cent, partBits);
    DObjCalcSkel(obj, partBits);
}

void CG_DObjCalcBoneGeneric(int handle, int localClientNum, int boneIndex)
{
    struct DObj_s *obj;
    centity_t *cent;
    int partBits[4];

    obj = (struct DObj_s *)Com_GetClientDObj(handle, localClientNum);
    if (CL_DObjCreateSkelForBone(obj, boneIndex, 0))
        return;

    DObjGetHierarchyBits(obj, boneIndex, partBits);
    DObjCalcAnim(obj, partBits);

    if (handle < 1024) {
        cent = &(((centity_t *)cg_entities))[handle];
        CG_RunDObjControllers(cent, partBits);
    }

    DObjCalcSkel(obj, partBits);
}

void CG_ProcessEntity(centity_t *cent)
{
    int eType;

    CG_EntityEffects(cent);

    eType = cent->nextState.eType;
    switch (eType) {
    case 9:
        CG_Turret(cent);
        break;
    case 0:
        CG_General(cent);
        break;
    case 1:
        CG_Player(cent);
        break;
    case 2:
        CG_Corpse(cent);
        break;
    case 3:
        CG_Item(cent);
        break;
    case 4:
        CG_Missile(cent);
        break;
    case 5:
        break;
    case 6:
        CG_ScriptMover(cent);
        break;
    case 7:
        CG_SoundBlend(cent);
        break;
    case 8:
        CG_PlayLoopedFx(cent);
        break;
    default:
        Com_Error(ERR_DROP, (const char *)"\x15"
                                   "Bad entity type: %i\n",
                  eType);
        return;
    }
}

void CG_UsedDObjCalcPose(const centity_t *cent)
{
    if (cent != NULL && cent->cullIn == 0)
        ((centity_t *)cent)->cullIn = 1;
}

void CG_CullIn(const centity_t *cent)
{
    if (cent != NULL)
        ((centity_t *)cent)->cullIn = 2;
}

void CG_AddCEntityToScene(const GfxEntity *ent, const struct DObj_s *obj, const centity_t *cent)
{
    GfxModel model;
    GfxEntity *sceneEnt;

    model.obj = obj;
    sceneEnt = R_AddRefEntityToScene(ent, model, cent);
    if (!sceneEnt)
        return;

    if (cent->cullIn == 1) {
        ((centity_t *)cent)->cullIn = 0;
        R_UpdateXModelBoundsDelayed(sceneEnt);
    } else if (cent->cullIn == 2) {
        ((centity_t *)cent)->cullIn = 0;
        R_SkinGfxEntityDelayed(sceneEnt);
    }
}


