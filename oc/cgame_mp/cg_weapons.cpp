#include "common_types.h"
#include "imports.h"
#include "headers/PC/cgame_mp/cg_local.h"

int removeMeWhenMPStopsCrashingInHere;
vec3_t ejectBrassCasingOrigin;
extern const dvar_t *cg_gun_x;
extern const dvar_t *cg_gun_y;
extern const dvar_t *cg_gun_z;
extern const dvar_t *cg_gun_move_f;
extern const dvar_t *cg_gun_move_r;
extern const dvar_t *cg_gun_move_u;
extern const dvar_t *cg_gun_ofs_f;
extern const dvar_t *cg_gun_ofs_r;
extern const dvar_t *cg_gun_ofs_u;
extern const dvar_t *cg_gun_move_rate;
extern const dvar_t *cg_gun_move_minspeed;
extern dvar_t *player_breath_hold_time;
extern dvar_t *player_breath_snd_delay;
extern dvar_t *player_breath_snd_lerp;

extern const dvar_t *cg_weaponCycleDelay;
extern dvar_t *loc_warnings;
extern dvar_t *loc_warningsAsErrors;

extern const dvar_t *cg_blood;
extern const dvar_t *cg_brass;
extern const dvar_t *cg_drawGun;
extern const dvar_t *cg_tracerChance;
extern const dvar_t *cg_tracerScale;
extern const dvar_t *cg_tracerScaleDistRange;
extern const dvar_t *cg_tracerScaleMinDist;
extern const dvar_t *cg_tracerSpeed;
extern const dvar_t *cg_tracerWidth;
extern const char *CL_GetConfigString(int index);
extern char *getenv(const char *name);
extern scr_const_t scr_const;

extern vec3_t ejectBrassCasingOrigin;
extern int removeMeWhenMPStopsCrashingInHere;
extern WeaponDef * BG_GetWeaponDef(int iWeapon);
extern void CG_SetWeaponDefToDefaultWeapon(int weaponNum);
extern void SCR_UpdateScreen(void);
extern XAnim *XAnimCreateAnims(const char *debugName, int size, Alloc_t Alloc);
extern void XAnimBlend(XAnim *anims, unsigned int animIndex, const char *name, unsigned int children, unsigned int num, unsigned int flags);
extern XAnimParts *XAnimPrecache(const char *name, Alloc_t Alloc);
extern void XAnimCreate(struct XAnim_s *anims, unsigned int animIndex, const char *name);
extern XAnimTree *XAnimCreateTree(XAnim *anims, Alloc_t Alloc);
extern void XAnimClearTreeGoalWeights(XAnimTree *tree, unsigned int animIndex, float blendTime);
extern int XAnimSetGoalWeight(XAnimTree *tree, unsigned int animIndex, float goalWeight, float goalTime, float rate, unsigned int notifyName, unsigned int notifyType, int bRestart);
extern void XAnimSetTime(XAnimTree *tree, unsigned int animIndex, float time);
extern bool XAnimIsLooped(const XAnim_s *anim, unsigned int animIndex);
extern int XAnimGetLengthMsec(const XAnim *anims, unsigned int animIndex);
extern qboolean XModelBad(XModel *model);
extern struct DObj_s * Com_GetClientDObj(int handle, int localClientNum);
extern void DObjUpdateClientInfo(struct DObj_s *obj, float timescale);
extern void Com_Error(errorParm_t code, const char *fmt, ...);
extern void Com_Printf(const char *fmt, ...);
extern MaterialHandle CL_RegisterMaterial(const char *name, int imageTrack);
extern MaterialHandle CL_RegisterMaterialNoMip(const char *name, int imageTrack);
extern struct XModel *CL_RegisterModel(const char *name);
extern EffectTemplate * FX_RegisterEffect(const char *fileName);
extern snd_alias_list_t *Com_FindSoundAlias( const char *name );
extern const char *SEH_StringEd_GetString(const char *str);
extern void Com_ClientDObjCreate(DObjModel_s *dobjModels, unsigned short numModels, struct XAnimTree_s *tree, int handle);
extern void I_strncpyz(char *dest, const char *src, int destsize);
extern void CL_RegisterHudMsgIconMaterial(const char *name);
extern qboolean CG_ScoreboardDisplayed(void);
extern void CG_ScrollScoreboardUp(void);
extern void CG_ScrollScoreboardDown(void);
extern void CG_GameMessage(const char *msg);
extern const char *SEH_LocalizeTextMessage(const char *msg, const char *context, msgLocErrType_t errType);
extern const char *CG_Argv(int arg);
extern int BG_GetWeaponSlotForName(const char *pszSlotName);
extern void CG_MenuShowNotify(int menuToShow);
extern void CL_SetADS(int ads);
extern int atoi(const char *nptr);
extern int BG_GetWeaponIndexForName( const char *name, int (*regWeap)(int) );
extern qboolean CG_DObjGetWorldTagPos(const centity_t *cent, struct DObj_s *obj, unsigned int tagName, vec_t *pos);
extern const char *SL_ConvertToString(unsigned int stringValue);
extern void Com_DPrintf(const char *fmt, ...);
extern int BG_GetNumWeapons(void);
extern int BG_IsPlayerWeaponInSlot( playerState_t *pPS, int iWeaponIndex, qboolean bAnyMode );
extern int BG_GetStackSlotForWeapon( const playerState_t *pPS, int iWeaponIndex, unsigned int preferedSlot );
extern bool BG_DoesWeaponNeedSlot(int weapon);
extern int BG_WeaponAmmo(const playerState_t *ps, int weapon);
extern qboolean BG_IsPlayerWeaponAnAlt(int iWeaponIndex, int iAltIndex);
extern int FX_GetBoneIndex(const int entNum, unsigned int bone);
extern void FX_PlayEntityEffect(const EffectTemplate *fx, const vec_t *org, const vec3_t *axis, const FxBoltInfo *bolt);
extern void FX_PlayEffect(const EffectTemplate *fx, const vec_t *org, const vec_t *fwd);
extern int strcmp(const char *s1, const char *s2);
extern struct XAnimTree_s *DObjGetTree(const struct DObj_s *obj);
extern void Com_SafeClientDObjFree(int handle);
extern int BG_GetViewmodelWeaponIndex(const playerState_t *ps);
extern bool XAnimHasFinished(const struct XAnimTree_s *tree, unsigned int animIndex);
extern int BG_ClipForWeapon(int weapon);
extern vec_t Vec3Normalize( vec3_t v );
extern vec_t Vec3NormalizeTo( const vec3_t v, vec3_t out );
extern float Vec3Distance(const vec3_t v1, const vec3_t v2);
extern localEntity_t *CG_AllocLocalEntity(void);
extern int rand(void);
extern float sqrtf(float x);
extern int CG_PlaySoundAlias(int entitynum, const vec_t *origin, snd_alias_list_t *aliasList);
extern void CL_DefaultVertexFrames(int vertCount, GfxWorldVertex *verts);
extern void CL_AddPolyToScene(MaterialHandle mtlHandle, unsigned short lmapIndex,
                              unsigned short vertCount, const GfxWorldVertex *verts);
extern void BG_WeaponFireRecoil(playerState_s *ps, float *vGunSpeed, float *kickAVel);
extern qboolean CG_DObjGetViewModelTagPos(struct DObj_s *obj, unsigned int tagName, vec_t *pos);
extern void BG_EvaluateTrajectory( const trajectory_t *tr, int atTime, vec3_t result );
extern void CG_CompassAddWeaponPingInfo(centity_t *cent, const vec_t *origin, int msec);
extern qboolean CL_DObjCreateSkelForBones(const struct DObj_s *obj, int *partBits, int localClientNum);
extern void DObjCalcAnim(const DObj *obj, int *partBits);
extern void DObjCalcSkel(DObj *obj, int *partBits);
extern int DObjGetClientNotifyList(XAnimNotify **notifyList);
extern int CG_PlayClientSoundAlias(snd_alias_list_t *aliasList);
extern int stricmp(const char *s1, const char *s2);
extern void AxisCopy( const vec3_t in[3], vec3_t out[3] );
extern void AxisToAngles( vec3_t axis[3], vec3_t angles );
extern void AngleVectors( const vec3_t angles, vec3_t forward, vec3_t right, vec3_t up );
extern float AngleNormalize360( float angle );
extern float GetLeanFraction( const float fFrac );
extern void AnglesToAxis( const vec3_t angles, vec3_t axis[3] );
extern void MatrixMultiply(const float in1[3][3], const float in2[3][3], float out[3][3]);
extern void AddLeanToPosition(vec_t *position, const float fViewYaw, const float fLeanFrac, const float fViewRoll, const float fLeanDist);
extern float DiffTrack(float tgt, float cur, float rate, float deltaTime);
extern int DObjGetBoneIndex(const DObj *obj, unsigned int boneName);
extern DObjAnimMat* DObjGetRotTransArray(const DObj_s *obj);
extern void MatrixTransformVector43(const vec3_t in1, const float in2[4][3], vec3_t out);
extern GfxEntity *R_AddRefEntityToScene(const GfxEntity *refEnt, GfxModel sceneModel, const centity_t *cent);
extern void R_SkinGfxEntityDelayed(GfxEntity *ent);
extern void SND_SetChannelVolumes(int priority, const float *channelvolume, int fademsec);
extern void SND_DeactivateChannelVolumes(int priority, int fademsec);
extern bool CG_GetWeapReticleZoom(float *fZoom);
extern qboolean BG_IsAimDownSightWeapon(int iWeapon);
extern void BG_CalculateWeaponPosition_Sway(const struct playerState_s *ps, float *swayViewAngles, float *swayOffset, float *swayAngles, float ssSwayScale, int frametime);
extern void BG_CalculateWeaponAngles(weaponState_t *ws, vec_t *angles);
extern void *Hunk_AllocXAnimPrecache(int size);
extern void *Hunk_AllocXAnimClient(int size);
extern itemInfo_t cg_itemsArray[];
extern weaponInfo_t cg_weaponsArray[];
extern gitem_t bg_itemlist[];
extern const int bg_numItems;
static scr_string_t *s_barrelTags[4] = { &scr_const.tag_flash, &scr_const.tag_flash_11, &scr_const.tag_flash_2, &scr_const.tag_flash_22 };

COD2_ASSERT_FIELD(WeaponDef, szGunXModel,       0x0c);
COD2_ASSERT_FIELD(WeaponDef, weapSlot,          0x80);
COD2_ASSERT_FIELD(WeaponDef, bSlotStackable,    0x88);
COD2_ASSERT_FIELD(WeaponDef, iHoldFireTime,     0x210);
COD2_ASSERT_FIELD(WeaponDef, iMeleeTime,        0x214);
COD2_ASSERT_FIELD(WeaponDef, iReloadEmptyTime,  0x21c);
COD2_ASSERT_FIELD(WeaponDef, iReloadStartTime,  0x224);
COD2_ASSERT_FIELD(WeaponDef, iDropTime,         0x230);
COD2_ASSERT_FIELD(WeaponDef, iRaiseTime,        0x234);
COD2_ASSERT_FIELD(WeaponDef, iAltDropTime,      0x238);
COD2_ASSERT_FIELD(WeaponDef, iAltRaiseTime,     0x23c);
COD2_ASSERT_FIELD(WeaponDef, quickDropTime,     0x240);
COD2_ASSERT_FIELD(WeaponDef, quickRaiseTime,    0x244);
COD2_ASSERT_FIELD(WeaponDef, bBoltAction,       0x328);
COD2_ASSERT_FIELD(WeaponDef, bADSPositionInfo,  0x32c);
COD2_ASSERT_FIELD(WeaponDef, iAltWeaponIndex,   0x36c);
COD2_ASSERT_FIELD(gitem_t,   pickup_sound,      0x04);
COD2_ASSERT_FIELD(gitem_t,   icon,              0x10);
COD2_ASSERT_FIELD(gitem_t,   giType,            0x1c);
COD2_ASSERT_FIELD(gitem_t,   giTag,             0x20);
COD2_ASSERT_FIELD(itemInfo_t, registered,       0x00);
COD2_ASSERT_FIELD(itemInfo_t, icons,            0x0c);
COD2_ASSERT_FIELD(itemInfo_t, pickupSound,      0x1c);
COD2_ASSERT_FIELD(itemInfo_t, ammoPickupSound,  0x20);
COD2_ASSERT_FIELD(centity_t, localClientNum, 0x220);
COD2_ASSERT_FIELD(centity_t, bMuzzleFlash,   0x1e2);

static void CG_PlayADSAnim(int animIndex, XAnimTree *pAnimTree);
int CG_WeaponDObjHandle(int weaponNum);
void CG_Weapons_SetToDefault(int weaponNum, DObjModel_s *dobjModels);
void CG_HoldBreathInit(void);
void CG_SetupWeaponDef(void);
void CG_SelectWeaponIndex(int weaponIndex);
static qboolean __attribute_regparm__(3) CG_CalcMuzzlePoint(int entityNum, vec_t *muzzle, unsigned int flashTag);
void CG_EjectWeaponBrass(entityState_t *ent, int event);
void CG_FireWeapon(centity_t *cent, int event, int barrel);
void CG_RegisterWeapon(int weaponNum);
void CG_RegisterItemVisuals(int itemNum);
void CG_RegisterItems(void);
void CG_UpdateHandViewmodels(const char *handModel);
void CG_SpawnTracer(vec_t *pstart, vec_t *pend);
void CG_WhizbySound(vec_t *vStart, vec_t *vEnd);
void CG_BulletHitClientEvent(int sourceEntityNum, vec_t *position, int surfType, int event);
void CG_AddPlayerWeapon(GfxEntity *parent, playerState_t *ps, centity_t *cent, qboolean bDrawGun);
static void WeaponFlash(int dobjHandle, int weaponNum, int bViewFlash, const vec_t *origin, unsigned int flashTag);
void CG_AddViewWeapon(playerState_t *ps);
void CG_BulletHitEvent(int sourceEntityNum, vec_t *position, vec_t *normal, vec_t *reflected, int surfType, int event);
void CG_DrawTracer(vec_t *start, vec_t *finish);
void CG_WeaponSlot_f(void);
qboolean CG_SelectFirstWeaponNotInSlot(qboolean bNext, qboolean bIgnoreEmpty);
void CG_CycleWeap(qboolean bNext, qboolean bIgnoreEmpty);
void CG_OutOfAmmoChange(void);
void CG_PrevWeapon_f(void);
void CG_NextWeapon_f(void);
void CG_WeaponRunXModelAnims(playerState_t *ps, weaponInfo_t *weapInfo);
void CG_UpdateViewWeaponAnim(playerState_t *ps);

static void CG_PlayADSAnim(int animIndex, XAnimTree *pAnimTree);
void CG_SelectWeaponIndex(int weaponIndex);
static __forceinline qboolean CG_IsLocalClientEntity(cg_t *cg, entityState_t *ent);
void CG_RegisterWeapon(int weaponNum);
void CG_RegisterItemVisuals(int itemNum);
void CG_CycleWeap(qboolean next, qboolean checkAmmo);
static void StartWeaponAnim(int weaponNum, struct XAnimTree_s *tree, int activeAnim);
















// kisak name; retail 0x4cd480
static __declspec(noinline) void StartWeaponAnim(int weaponNum, struct XAnimTree_s *tree, int activeAnim)
{
    int i;
    weaponInfo_t *weapInfo;

    weapInfo = &cg_weaponsArray[weaponNum];
    for (i = 0; i < 20; i++) {
        if (activeAnim == i + 1)
            XAnimSetGoalWeight(tree, i + 1, 1.0f, 0.0f, weapInfo->viewModelAnimRates[i + 1], 0, 1, 1);
        else
            XAnimSetGoalWeight(tree, i + 1, 0.0f, 0.0f, weapInfo->viewModelAnimRates[i + 1], 0, 0, 0);
    }
}
static void CG_PlayADSAnim(int animIndex, XAnimTree *pAnimTree)
{
    if (animIndex == 0x15) {

        XAnimSetGoalWeight(pAnimTree, 0x15, 1.0f, 0.5f, 0.0f, 0, 1, 0);
        XAnimSetGoalWeight(pAnimTree, 0x16, 0.0f, 0.5f, 0.0f, 0, 0, 0);
    } else {

        XAnimSetGoalWeight(pAnimTree, 0x15, 0.0f, 0.5f, 0.0f, 0, 0, 0);
        XAnimSetGoalWeight(pAnimTree, 0x16, 1.0f, 0.5f, 0.0f, 0, 1, 0);
    }

    XAnimSetTime(pAnimTree, 0x15, cg->predictedPlayerState.fWeaponPosFrac);
    XAnimSetTime(pAnimTree, 0x16, (1.0f - cg->predictedPlayerState.fWeaponPosFrac));
}

static void CG_PlayViewWeaponNotetrackSounds(void)
{
    XAnimNotify *notifyList;
    weaponInfo_t *weapInfo;
    snd_alias_list_t *aliasList;
    int weaponIndex;
    int notifyCount;
    int i;

    aliasList = 0;
    weaponIndex = BG_GetViewmodelWeaponIndex(&cg->predictedPlayerState);
    if (!weaponIndex)
        return;

    weapInfo = &cg_weaponsArray[weaponIndex];
    notifyCount = DObjGetClientNotifyList(&notifyList);
    for (i = 0; i < notifyCount; i++) {
        if (!stricmp(notifyList[i].name, "noteTrackSoundA"))
            aliasList = weapInfo->noteTrackSound[0];
        else if (!stricmp(notifyList[i].name, "noteTrackSoundB"))
            aliasList = weapInfo->noteTrackSound[1];
        else if (!stricmp(notifyList[i].name, "noteTrackSoundC"))
            aliasList = weapInfo->noteTrackSound[2];
        else if (!stricmp(notifyList[i].name, "noteTrackSoundD"))
            aliasList = weapInfo->noteTrackSound[3];

        if (aliasList)
            CG_PlayClientSoundAlias(aliasList);
    }
}

void CG_WeaponRunXModelAnims(playerState_t *ps, weaponInfo_t *weapInfo)
{
    WeaponDef *weapDef;
    struct XAnimTree_s *pAnimTree;
    int weaponIndex;
    int i;
    qboolean adsIn;

    pAnimTree = DObjGetTree(weapInfo->viewModelDObj);
    adsIn = 0;
    weaponIndex = BG_GetViewmodelWeaponIndex(ps);
    weapDef = (WeaponDef *)BG_GetWeaponDef(weaponIndex);

    if (ps->weaponstate == 5 && ps->weaponTime - weapDef->iPositionReloadTransTime > 0) {
        adsIn = 0;
    } else if (ps->pm_flags & 0x40) {
        adsIn = 1;
    }
    if (ps->pm_flags & 0x10)
        adsIn = 0;
    if (weapDef->bADSPositionInfo) {
        int adsAnim = 0x15;
        if (!adsIn)
            adsAnim = 0x16;
        CG_PlayADSAnim(adsAnim, pAnimTree);
    }

    if (ps->weapAnim == weapInfo->iPrevAnim)
        return;

    switch (ps->weapAnim & ~0x200) {
    case 0:
        for (i = 1; i < 0x15; i++) {
            if (!XAnimHasFinished(pAnimTree, i)) {
                weapInfo->iPrevAnim = -1;
                return;
            }
        }
        if (!ps->ammoclip[BG_ClipForWeapon(weaponIndex)])
            StartWeaponAnim(weaponIndex, pAnimTree, 2);
        else
            StartWeaponAnim(weaponIndex, pAnimTree, 1);
        break;
    case 2: StartWeaponAnim(weaponIndex, pAnimTree, 3); break;
    case 3: StartWeaponAnim(weaponIndex, pAnimTree, 5); break;
    case 4: StartWeaponAnim(weaponIndex, pAnimTree, 6); break;
    case 5: StartWeaponAnim(weaponIndex, pAnimTree, 18); break;
    case 6: StartWeaponAnim(weaponIndex, pAnimTree, 19); break;
    case 7: StartWeaponAnim(weaponIndex, pAnimTree, 20); break;
    case 8: StartWeaponAnim(weaponIndex, pAnimTree, 7); break;
    case 9: StartWeaponAnim(weaponIndex, pAnimTree, 13); break;
    case 10: StartWeaponAnim(weaponIndex, pAnimTree, 12); break;
    case 11: StartWeaponAnim(weaponIndex, pAnimTree, 8); break;
    case 12: StartWeaponAnim(weaponIndex, pAnimTree, 9); break;
    case 13: StartWeaponAnim(weaponIndex, pAnimTree, 10); break;
    case 14: StartWeaponAnim(weaponIndex, pAnimTree, 11); break;
    case 15: StartWeaponAnim(weaponIndex, pAnimTree, 15); break;
    case 16: StartWeaponAnim(weaponIndex, pAnimTree, 14); break;
    case 17: StartWeaponAnim(weaponIndex, pAnimTree, 17); break;
    case 18: StartWeaponAnim(weaponIndex, pAnimTree, 16); break;
    case 19: StartWeaponAnim(weaponIndex, pAnimTree, 4); break;
    default:
        StartWeaponAnim(weaponIndex, pAnimTree, 1);
        Com_Printf("CG_WeaponRunXModelAnims: Unknown weapon animation %i\n", ps->weapAnim & ~0x200);
        break;
    }

    weapInfo->iPrevAnim = ps->weapAnim;
}

static void CG_ResetViewWeaponAnimTrees(playerState_t *ps)
{
    struct XAnimTree_s *tree;
    weaponInfo_t *weapInfo;
    int weaponNum;

    for (weaponNum = 1; weaponNum <= BG_GetNumWeapons(); weaponNum++) {
        weapInfo = &cg_weaponsArray[weaponNum];
        if (weapInfo->viewModelDObj) {
            tree = DObjGetTree(weapInfo->viewModelDObj);
            XAnimClearTreeGoalWeights(tree, 0, 0);
            XAnimSetGoalWeight(tree, 0, 1.0f, 0.0f, weapInfo->viewModelAnimRates[0], 0, 1, 0);

            if (!ps->ammoclip[BG_ClipForWeapon(weaponNum)])
                StartWeaponAnim(weaponNum, tree, 2);
            else
                StartWeaponAnim(weaponNum, tree, 1);
        }
    }
}

int CG_WeaponDObjHandle(int weaponNum)
{
    return weaponNum + 0x400;
}

void CG_Weapons_SetToDefault(int weaponNum, DObjModel_s *dobjModels)
{
    WeaponDef *weapDef;
    char modelFile[64];

    CG_SetWeaponDefToDefaultWeapon(weaponNum);
    weapDef = (WeaponDef *)BG_GetWeaponDef(weaponNum);
    Com_Printf("WARNING: gun and/or hand model file for weapon [%s] could not be found, switching to default weapon.\n",
               weapDef->szDisplayName);

    if (!weapDef->szGunXModel || !weapDef->szGunXModel[0] ||
        !weapDef->szHandXModel || !weapDef->szHandXModel[0])
        Com_Error(ERR_DROP, "\x15"
                     "CG_RegisterWeapon: No gun and/or hand model specified for default weapon!");

    sprintf(modelFile, "%s%s", "xmodel/", weapDef->szHandXModel);
    dobjModels[0].model = CL_RegisterModel(modelFile);

    sprintf(modelFile, "%s%s", "xmodel/", weapDef->szGunXModel);
    dobjModels[1].model = CL_RegisterModel(modelFile);
}

void CG_RegisterWeapon(int weaponNum)
{
    WeaponDef *weapDef;
    weaponInfo_t *weapInfo;
    itemInfo_t *itemInfo;
    struct XAnim_s *pAnims;
    struct XAnimTree_s *pAnimTree;
    DObjModel_s dobjModels[2];
    char modelFile[64];
    int dobjHandle;
    int i;
    float animRate;

    removeMeWhenMPStopsCrashingInHere = weaponNum;

    if (!weaponNum)
        return;

    weapInfo = &cg_weaponsArray[weaponNum];
    weapDef = (WeaponDef *)BG_GetWeaponDef(weaponNum);
    if (weapInfo->registered)
        return;

    SCR_UpdateScreen();
    memset(weapInfo, 0, sizeof(*weapInfo));
    weapInfo->registered = 1;
    weapInfo->item = &((gitem_t *)((void *)&bg_itemlist))[weaponNum];
    itemInfo = &cg_itemsArray[weaponNum];

    CG_RegisterItemVisuals(weaponNum);
    weapInfo->iPrevAnim = -1;

    if (weapDef->szGunXModel[0]) {
        if (!weapDef->szHandXModel || !weapDef->szHandXModel[0])
            Com_Error(ERR_DROP, "\x15"
                         "CG_RegisterWeapon: No hand model specified for [%s]",
                      weapDef->szDisplayName);

        dobjModels[0].boneName = 0;
        dobjModels[0].ignoreCollision = 0;
        dobjModels[1].boneName = "tag_weapon";
        dobjModels[1].ignoreCollision = 0;

        sprintf(modelFile, "%s%s", "xmodel/", weapDef->szHandXModel);
        dobjModels[0].model = CL_RegisterModel(modelFile);

        sprintf(modelFile, "%s%s", "xmodel/", weapDef->szGunXModel);
        dobjModels[1].model = CL_RegisterModel(modelFile);

        if (XModelBad(dobjModels[0].model) || XModelBad(dobjModels[1].model))
            CG_Weapons_SetToDefault(weaponNum, dobjModels);

        if (!weapDef->szXAnims[1] || !weapDef->szXAnims[1][0])
            Com_Error(ERR_DROP, "\x15"
                         "CG_RegisterWeapon: No idle anim specified for [%s]",
                      weapDef->szDisplayName);

        pAnims = XAnimCreateAnims("VIEWMODEL", 0x17, Hunk_AllocXAnimClient);
        XAnimBlend(pAnims, 0, "root", 1, 0x16, 0);

        for (i = 1; i < 0x17; i++) {
            if (weapDef->szXAnims[i][0]) {
                XAnimPrecache(weapDef->szXAnims[i], Hunk_AllocXAnimPrecache);
                XAnimCreate(pAnims, i, weapDef->szXAnims[i]);
            } else {
                XAnimPrecache(weapDef->szXAnims[1], Hunk_AllocXAnimPrecache);
                XAnimCreate(pAnims, i, weapDef->szXAnims[1]);
            }
        }

        pAnimTree = (struct XAnimTree_s *)XAnimCreateTree(pAnims, Hunk_AllocXAnimClient);
        weapInfo->tree = pAnimTree;

        for (i = 0; i < 0x17; i++)
            weapInfo->viewModelAnimRates[i] = 1.0f;

        animRate = 0.0f;
        if (weapDef->iHoldFireTime > 0)
            animRate = (float)XAnimGetLengthMsec(pAnims, 4) / (float)weapDef->iHoldFireTime;
        weapInfo->viewModelAnimRates[4] = animRate;
        animRate = 0.0f;
        if (weapDef->iMeleeTime > 0)
            animRate = (float)XAnimGetLengthMsec(pAnims, 7) / (float)weapDef->iMeleeTime;
        weapInfo->viewModelAnimRates[7] = animRate;
        animRate = 0.0f;
        if (weapDef->iReloadTime > 0)
            animRate = (float)XAnimGetLengthMsec(pAnims, 8) / (float)weapDef->iReloadTime;
        weapInfo->viewModelAnimRates[8] = animRate;
        animRate = 0.0f;
        if (weapDef->iReloadEmptyTime > 0)
            animRate = (float)XAnimGetLengthMsec(pAnims, 9) / (float)weapDef->iReloadEmptyTime;
        weapInfo->viewModelAnimRates[9] = animRate;
        animRate = 0.0f;
        if (weapDef->iReloadStartTime > 0)
            animRate = (float)XAnimGetLengthMsec(pAnims, 10) / (float)weapDef->iReloadStartTime;
        weapInfo->viewModelAnimRates[10] = animRate;
        animRate = 0.0f;
        if (weapDef->iReloadTime > 0)
            animRate = (float)XAnimGetLengthMsec(pAnims, 11) / (float)weapDef->iReloadTime;
        weapInfo->viewModelAnimRates[11] = animRate;
        animRate = 0.0f;
        if (weapDef->iRaiseTime > 0)
            animRate = (float)XAnimGetLengthMsec(pAnims, 12) / (float)weapDef->iRaiseTime;
        weapInfo->viewModelAnimRates[12] = animRate;
        animRate = 0.0f;
        if (weapDef->iDropTime > 0)
            animRate = (float)XAnimGetLengthMsec(pAnims, 13) / (float)weapDef->iDropTime;
        weapInfo->viewModelAnimRates[13] = animRate;
        animRate = 0.0f;
        if (weapDef->iAltRaiseTime > 0)
            animRate = (float)XAnimGetLengthMsec(pAnims, 14) / (float)weapDef->iAltRaiseTime;
        weapInfo->viewModelAnimRates[14] = animRate;
        animRate = 0.0f;
        if (weapDef->iAltDropTime > 0)
            animRate = (float)XAnimGetLengthMsec(pAnims, 15) / (float)weapDef->iAltDropTime;
        weapInfo->viewModelAnimRates[15] = animRate;
        animRate = 0.0f;
        if (weapDef->quickRaiseTime > 0)
            animRate = (float)XAnimGetLengthMsec(pAnims, 16) / (float)weapDef->quickRaiseTime;
        weapInfo->viewModelAnimRates[16] = animRate;
        animRate = 0.0f;
        if (weapDef->quickDropTime > 0)
            animRate = (float)XAnimGetLengthMsec(pAnims, 17) / (float)weapDef->quickDropTime;
        weapInfo->viewModelAnimRates[17] = animRate;

        if (weapDef->szXAnims[21][0] && XAnimIsLooped(pAnims, 0x15))
            Com_Error(ERR_DROP, "\x15"
                         "CG_RegisterWeapon: ADS anim [%s] cannot be looping",
                      weapDef->szXAnims[21]);

        if (weapDef->szXAnims[22][0] && XAnimIsLooped(pAnims, 0x16))
            Com_Error(ERR_DROP, "\x15"
                         "CG_RegisterWeapon: ADS anim [%s] cannot be looping",
                      weapDef->szXAnims[22]);

        dobjHandle = weaponNum + 0x400;
        Com_ClientDObjCreate(dobjModels, 2, pAnimTree, dobjHandle);
        weapInfo->viewModelDObj = (DObj_s *)(Com_GetClientDObj(dobjHandle, 0));
        I_strncpyz(weapInfo->handModel, weapDef->szHandXModel, sizeof(weapInfo->handModel));

        XAnimClearTreeGoalWeights(pAnimTree, 0, 0);
        XAnimSetGoalWeight(pAnimTree, 0, 1.0f, 0.0f, weapInfo->viewModelAnimRates[0], 0, 1, 0);
        XAnimSetGoalWeight(pAnimTree, 1, 1.0f, 0.0f, weapInfo->viewModelAnimRates[1], 0, 1, 0);

        if (weapDef->szXAnims[22][0]) {
            XAnimSetGoalWeight(pAnimTree, 0x16, 1.0f, 0.0f, 0.0f, 0, 1, 0);
            XAnimSetTime(pAnimTree, 0x16, 1.0f);
        }

        DObjUpdateClientInfo(weapInfo->viewModelDObj, 0.05f);
    }

    if (weapDef->szWorldModel[0])
        weapInfo->worldSurfModel = CL_RegisterModel(weapDef->szWorldModel);

    if (weapDef->szWorldModel[0] && !weapInfo->worldSurfModel)
        Com_Printf("WARNING: Weapon %s could not load world model\n", weapDef->szWorldModel);

    if (weapDef->szReticleCenter[0])
        weapInfo->hReticleCenter = (MaterialHandle)CL_RegisterMaterialNoMip(weapDef->szReticleCenter, 7);

    if (weapDef->szReticleSide[0])
        weapInfo->hReticleSide = (MaterialHandle)CL_RegisterMaterialNoMip(weapDef->szReticleSide, 7);

    if (weapDef->szOverlayMaterial[0])
        weapInfo->hADSOverlay = (MaterialHandle)CL_RegisterMaterialNoMip(weapDef->szOverlayMaterial, 7);

    if (weapDef->szViewFlashEffect[0])
        weapInfo->viewFlashEffect = (struct FxEffectDef *)FX_RegisterEffect(weapDef->szViewFlashEffect);

    if (weapDef->szWorldFlashEffect[0])
        weapInfo->worldFlashEffect = (struct FxEffectDef *)FX_RegisterEffect(weapDef->szWorldFlashEffect);

    weapInfo->missileSound = Com_FindSoundAlias(weapDef->szProjectileSound);
    weapInfo->pullbackSound = Com_FindSoundAlias(weapDef->szPullbackSound);
    weapInfo->flashSound = Com_FindSoundAlias(weapDef->szFireSound);
    weapInfo->flashSoundPlayer = Com_FindSoundAlias(weapDef->szFireSoundPlayer);
    weapInfo->lastShotSound = Com_FindSoundAlias(weapDef->szFireLastSound);
    weapInfo->lastShotSoundPlayer = Com_FindSoundAlias(weapDef->szFireLastSoundPlayer);
    weapInfo->meleeSwipeSound = Com_FindSoundAlias(weapDef->meleeSwipeSound);
    weapInfo->rechamberSound = Com_FindSoundAlias(weapDef->szRechamberSound);
    weapInfo->rechamberSoundPlayer = Com_FindSoundAlias(weapDef->szRechamberSoundPlayer);
    weapInfo->reloadSound = Com_FindSoundAlias(weapDef->szReloadSound);
    weapInfo->reloadSoundPlayer = Com_FindSoundAlias(weapDef->szReloadSoundPlayer);
    weapInfo->reloadEmptySound = Com_FindSoundAlias(weapDef->szReloadEmptySound);
    weapInfo->reloadEmptySoundPlayer = Com_FindSoundAlias(weapDef->szReloadEmptySoundPlayer);
    weapInfo->reloadStartSound = Com_FindSoundAlias(weapDef->szReloadStartSound);
    weapInfo->reloadStartSoundPlayer = Com_FindSoundAlias(weapDef->szReloadStartSoundPlayer);
    weapInfo->reloadEndSound = Com_FindSoundAlias(weapDef->szReloadEndSound);
    weapInfo->reloadEndSoundPlayer = Com_FindSoundAlias(weapDef->szReloadEndSoundPlayer);
    weapInfo->raiseSound = Com_FindSoundAlias(weapDef->szRaiseSound);
    if (!weapInfo->raiseSound)
        weapInfo->raiseSound = Com_FindSoundAlias("weap_raise");
    weapInfo->altSwitchSound = Com_FindSoundAlias(weapDef->szAltSwitchSound);
    weapInfo->putawaySound = Com_FindSoundAlias(weapDef->szPutawaySound);
    if (!weapInfo->putawaySound)
        weapInfo->putawaySound = Com_FindSoundAlias("weap_putaway");
    weapInfo->noteTrackSound[0] = Com_FindSoundAlias(weapDef->szNoteTrackSoundA);
    weapInfo->noteTrackSound[1] = Com_FindSoundAlias(weapDef->szNoteTrackSoundB);
    weapInfo->noteTrackSound[2] = Com_FindSoundAlias(weapDef->szNoteTrackSoundC);
    weapInfo->noteTrackSound[3] = Com_FindSoundAlias(weapDef->szNoteTrackSoundD);

    if (!itemInfo->pickupSound)
        itemInfo->pickupSound = Com_FindSoundAlias("weap_pickup");

    itemInfo->ammoPickupSound = Com_FindSoundAlias(weapDef->szAmmoPickupSound);
    if (!itemInfo->ammoPickupSound)
        itemInfo->ammoPickupSound = Com_FindSoundAlias("weap_ammo_pickup");

    if (weapDef->szShellEjectEffect[0])
        weapInfo->shellEjectEffect = (struct FxEffectDef *)FX_RegisterEffect(weapDef->szShellEjectEffect);

    if (weapDef->szLastShotEjectEffect[0])
        weapInfo->lastShotEjectEffect = (struct FxEffectDef *)FX_RegisterEffect(weapDef->szLastShotEjectEffect);
    else
        weapInfo->lastShotEjectEffect = weapInfo->shellEjectEffect;

    if (weapDef->szProjectileModel[0]) {
        weapInfo->missileSurfModel = CL_RegisterModel(weapDef->szProjectileModel);
        if (!weapInfo->missileSurfModel)
            Com_Error(ERR_DROP, "\x15Weapon %s does not specify a valid projectile model (%s)\n", weapDef->szInternalName, weapDef->szWorldModel);
    }

    if (weapDef->szProjExplosionEffect[0])
        weapInfo->projExplosionEffect = (struct FxEffectDef *)FX_RegisterEffect(weapDef->szProjExplosionEffect);

    weapInfo->projExplosionSound = Com_FindSoundAlias(weapDef->szProjExplosionSound);

    if (weapDef->szProjTrailEffect[0])
        weapInfo->projTrailEffect = (struct FxEffectDef *)FX_RegisterEffect(weapDef->szProjTrailEffect);

    weapInfo->missileDlight = (float)weapDef->iProjectileDLight;

    if (weapDef->szHudIcon[0]) {
        weapInfo->hHudIcon = (MaterialHandle)CL_RegisterMaterial(weapDef->szHudIcon, 7);
        cgs->media.hintMaterials[weaponNum + 4] = weapInfo->hHudIcon;
    } else {
        cgs->media.hintMaterials[weaponNum + 4] = cgs->media.hintMaterials[2];
    }

    if (weapDef->killIcon[0]) {
        CL_RegisterMaterial(weapDef->killIcon, 7);
        CL_RegisterHudMsgIconMaterial(weapDef->killIcon);
    }

    if (weapDef->szModeIcon[0])
        weapInfo->hModeIcon = (MaterialHandle)CL_RegisterMaterial(weapDef->szModeIcon, 7);

    weapInfo->pszTranslatedDisplayName = SEH_StringEd_GetString(weapDef->szDisplayName);
    if (!weapInfo->pszTranslatedDisplayName) {
        if (loc_warnings->current.enabled) {
            if (loc_warningsAsErrors->current.enabled)
                Com_Error(ERR_LOCALIZATION, "Weapon %s: Could not translate display name \"%s\"", weapDef->szInternalName, weapDef->szDisplayName);
            else
                Com_Printf("^3WARNING: Weapon %s: Could not translate display name \"%s\"\n", weapDef->szInternalName, weapDef->szDisplayName);
        }
        weapInfo->pszTranslatedDisplayName = weapDef->szDisplayName;
    }

    weapInfo->pszTranslatedModename = SEH_StringEd_GetString(weapDef->szModeName);
    if (!weapInfo->pszTranslatedModename) {
        if (loc_warnings->current.enabled) {
            if (loc_warningsAsErrors->current.enabled)
                Com_Error(ERR_LOCALIZATION, "Weapon %s: Could not translate mode name \"%s\"", weapDef->szInternalName, weapDef->szModeName);
            else
                Com_Printf("^3WARNING: Weapon %s: Could not translate mode name \"%s\"\n", weapDef->szInternalName, weapDef->szModeName);
        }
        weapInfo->pszTranslatedModename = weapDef->szModeName;
    }

    weapInfo->pszTranslatedAIOverlayDescription = SEH_StringEd_GetString(weapDef->szOverlayName);
    if (!weapInfo->pszTranslatedAIOverlayDescription) {
        if (loc_warnings->current.enabled) {
            if (loc_warningsAsErrors->current.enabled)
                Com_Error(ERR_LOCALIZATION, "Weapon %s: Could not translate AI overlay description \"%s\"", weapDef->szInternalName, weapDef->szOverlayName);
            else
                Com_Printf("^3WARNING: Weapon %s: Could not translate AI overlay description \"%s\"\n", weapDef->szInternalName, weapDef->szOverlayName);
        }
        weapInfo->pszTranslatedAIOverlayDescription = weapDef->szOverlayName;
    }
}

// Name from CoD4; original CoD2 name unknown.
static void ChangeViewmodelDobj(int weaponIdx, const char *handModel)
{
    weaponInfo_t *weapInfo;
    WeaponDef *weapDef;
    struct XAnimTree_s *pAnimTree;
    DObjModel_s dobjModels[2];
    int dobjHandle;
    char modelFile[64];

    if (!weaponIdx)
        return;

    weapDef = (WeaponDef *)BG_GetWeaponDef(weaponIdx);
    weapInfo = &cg_weaponsArray[weaponIdx];
    if (!weapInfo->viewModelDObj)
        return;

    if (!weapDef->szGunXModel[0])
        return;

    pAnimTree = DObjGetTree(weapInfo->viewModelDObj);
    dobjHandle = weaponIdx + 0x400;
    Com_SafeClientDObjFree(dobjHandle);

    dobjModels[0].boneName = 0;
    dobjModels[0].ignoreCollision = 0;
    dobjModels[1].boneName = "tag_weapon";
    dobjModels[1].ignoreCollision = 0;
    dobjModels[0].model = CL_RegisterModel(handModel);

    sprintf(modelFile, "%s%s", "xmodel/", weapDef->szGunXModel);
    dobjModels[1].model = CL_RegisterModel(modelFile);

    if (XModelBad(dobjModels[0].model) || XModelBad(dobjModels[1].model))
        CG_Weapons_SetToDefault(weaponIdx, dobjModels);

    Com_ClientDObjCreate(dobjModels, 2, pAnimTree, dobjHandle);
    weapInfo->viewModelDObj = (DObj_s *)(Com_GetClientDObj(dobjHandle, 0));
    I_strncpyz(weapInfo->handModel, handModel, 0x40);
    DObjUpdateClientInfo(weapInfo->viewModelDObj, 0.05f);
}

void CG_UpdateHandViewmodels(const char *handModel)
{
    int weaponIdx;

    for (weaponIdx = 1; weaponIdx <= BG_GetNumWeapons(); weaponIdx++) {
        if (strcmp(cg_weaponsArray[weaponIdx].handModel, handModel))
            ChangeViewmodelDobj(weaponIdx, handModel);
    }
}

void CG_RegisterItemVisuals(int itemNum)
{
    itemInfo_t *itemInfo;
    gitem_t *item;
    int i;

    itemInfo = &cg_itemsArray[itemNum];
    if (itemInfo->registered)
        return;

    item = &bg_itemlist[itemNum];
    itemInfo->registered = 0;

    for (i = 0; i < 2; i++) {
        if (item->world_model[i] && item->world_model[i][0]) {
            SCR_UpdateScreen();
            itemInfo->models[i] = CL_RegisterModel(item->world_model[i]);
        }
    }

    itemInfo->icons[0] = CL_RegisterMaterial(item->icon, 7);

    if (item->pickup_sound)
        itemInfo->pickupSound = Com_FindSoundAlias(item->pickup_sound);

    itemInfo->ammoPickupSound = itemInfo->pickupSound;

    if (item->giType == IT_WEAPON)
        CG_RegisterWeapon(item->giTag);

    itemInfo->registered = 1;
}

void CG_RegisterItems(void)
{
    char items[0x104];

    strcpy(items, CL_GetConfigString(8));

    for (int i = 1; i < bg_numItems; i++) {
        int c = (signed char)items[(unsigned int)i >> 2];

        if (c <= '9')
            c -= '0';
        else
            c -= 'W';

        if (c & (1 << (i & 3)))
            CG_RegisterItemVisuals(i);
    }
}

void CalculateWeaponPosition_Sway(void)
{
    WeaponDef *weapDef;
    int ssDT;
    float ssScale;
    float ssSwayScale;

    weapDef = BG_GetWeaponDef(BG_GetViewmodelWeaponIndex(&cg->predictedPlayerState));
    ssDT = cg->shellshock.duration - cg->time + cg->shellshock.startTime;

    if (ssDT > 0)
    {
        ssScale = 1.0f;
        if (ssDT < cg->shellshock.parms->view.fadeTime)
            ssScale = (float)ssDT / (float)cg->shellshock.parms->view.fadeTime;

        ssScale = (3.0f - ssScale * 2.0f) * ssScale * ssScale;
        ssSwayScale = (weapDef->swayShellShockScale - 1.0f) * ssScale + 1.0f;
    }
    else
    {
        ssSwayScale = 1.0f;
    }

    BG_CalculateWeaponPosition_Sway(&cg->predictedPlayerState, cg->swayViewAngles, cg->swayOffset, cg->swayAngles,
                                    ssSwayScale, cg->frametime);
}

void CalculateWeaponPosition_SwayMovement(float *origin)
{
    origin[1] = origin[1] - cg->swayOffset[1];
    origin[2] = cg->swayOffset[2] + origin[2];
}

// com_math.h
inline float I_fsel(const float x, const float y, const float z)
{
    return x >= 0.0f ? y : z;
}
inline float I_fmax(const float x, const float y)
{
    return I_fsel(x - y, x, y);
}
inline float I_fmin(const float x, const float y)
{
    return I_fsel(y - x, x, y);
}
void CalculateWeaponPosition_BasePosition_movement(float *origin)
{
    WeaponDef *weapDef;
    float minSpeed;
    float frac;
    float moveRate;
    float delta;
    vec3_t targetPos;
    int i;

    weapDef = BG_GetWeaponDef(BG_GetViewmodelWeaponIndex(&cg->predictedPlayerState));

    if (cg->predictedPlayerEntity.nextState.eFlags & 8)
        minSpeed = cg_gun_move_minspeed->current.value + weapDef->fProneMoveMinSpeed;
    else if (cg->predictedPlayerEntity.nextState.eFlags & 4)
        minSpeed = cg_gun_move_minspeed->current.value + weapDef->fDuckedMoveMinSpeed;
    else
        minSpeed = cg_gun_move_minspeed->current.value + weapDef->fStandMoveMinSpeed;

    if (cg->xyspeed > minSpeed && cg->predictedPlayerState.weaponstate != 5)
    {
        frac = (cg->xyspeed - minSpeed) / ((float)cg->predictedPlayerState.speed - minSpeed);
        frac = I_fmax(0.0f, frac);
        frac = I_fmin(1.0f, frac);

        if (cg->predictedPlayerEntity.nextState.eFlags & 8)
        {
            targetPos[0] = frac * weapDef->vProneMove[0];
            targetPos[1] = frac * weapDef->vProneMove[1];
            targetPos[2] = frac * weapDef->vProneMove[2];
        }
        else if (cg->predictedPlayerEntity.nextState.eFlags & 4)
        {
            targetPos[0] = frac * weapDef->vDuckedMove[0];
            targetPos[1] = frac * weapDef->vDuckedMove[1];
            targetPos[2] = frac * weapDef->vDuckedMove[2];
        }
        else
        {
            targetPos[0] = frac * weapDef->vStandMove[0];
            targetPos[1] = frac * weapDef->vStandMove[1];
            targetPos[2] = frac * weapDef->vStandMove[2];
        }

        targetPos[0] = cg_gun_move_f->current.value * frac + targetPos[0];
        targetPos[1] = cg_gun_move_r->current.value * frac + targetPos[1];
        targetPos[2] = cg_gun_move_u->current.value * frac + targetPos[2];
    }

    else
    {
        targetPos[0] = 0.0f;
        targetPos[1] = 0.0f;
        targetPos[2] = 0.0f;
    }

    if (cg->predictedPlayerState.viewHeightTarget == 40)
    {
        targetPos[0] = targetPos[0] + weapDef->vDuckedOfs[0];
        targetPos[1] = targetPos[1] + weapDef->vDuckedOfs[1];
        targetPos[2] = targetPos[2] + weapDef->vDuckedOfs[2];
        targetPos[0] = targetPos[0] + cg_gun_ofs_f->current.value;
        targetPos[1] = targetPos[1] + cg_gun_ofs_r->current.value;
        targetPos[2] = targetPos[2] + cg_gun_ofs_u->current.value;
    }
    else if (cg->predictedPlayerState.viewHeightTarget == 11)
    {
        targetPos[0] = targetPos[0] + weapDef->vProneOfs[0];
        targetPos[1] = targetPos[1] + weapDef->vProneOfs[1];
        targetPos[2] = targetPos[2] + weapDef->vProneOfs[2];
        targetPos[0] = targetPos[0] + cg_gun_ofs_f->current.value;
        targetPos[1] = targetPos[1] + cg_gun_ofs_r->current.value;
        targetPos[2] = targetPos[2] + cg_gun_ofs_u->current.value;
    }
    for (i = 0; i <= 2; i++)
    for (i = 0; i < 3; i++)
    {
        if (targetPos[i] != cg->playerEntity.vLastMoveOrg[i])
        {

            if (cg->predictedPlayerState.viewHeightCurrent == 11.0f)
                moveRate = cg_gun_move_rate->current.value + weapDef->fPosProneMoveRate;
            else
                moveRate = cg_gun_move_rate->current.value + weapDef->fPosMoveRate;

            delta = (targetPos[i] - cg->playerEntity.vLastMoveOrg[i]) * moveRate * (float)cg->frametime * 0.001f;

            if (cg->playerEntity.vLastMoveOrg[i] < targetPos[i])
            {
                if (delta < (float)cg->frametime * (0.001f * 0.1f))
                    delta = (float)cg->frametime * (0.001f * 0.1f);
                cg->playerEntity.vLastMoveOrg[i] = cg->playerEntity.vLastMoveOrg[i] + delta;
                if (cg->playerEntity.vLastMoveOrg[i] > targetPos[i])
                    cg->playerEntity.vLastMoveOrg[i] = targetPos[i];
            }
            else
            {
                if (delta > (float)cg->frametime * (0.001f * -0.1f))
                    delta = (float)cg->frametime * (0.001f * -0.1f);
                cg->playerEntity.vLastMoveOrg[i] = cg->playerEntity.vLastMoveOrg[i] + delta;
                if (cg->playerEntity.vLastMoveOrg[i] < targetPos[i])
                    cg->playerEntity.vLastMoveOrg[i] = targetPos[i];
            }
        }
    }

    if (cg->predictedPlayerState.fWeaponPosFrac == 0.0f)
    {
        origin[0] = cg->playerEntity.vLastMoveOrg[0] + origin[0];
        origin[1] = cg->playerEntity.vLastMoveOrg[1] + origin[1];
        origin[2] = cg->playerEntity.vLastMoveOrg[2] + origin[2];
    }
    else if (cg->predictedPlayerState.fWeaponPosFrac < 0.5f)
    {
        delta = 1.0f - (cg->predictedPlayerState.fWeaponPosFrac + cg->predictedPlayerState.fWeaponPosFrac);
        origin[0] = cg->playerEntity.vLastMoveOrg[0] * delta + origin[0];
        origin[1] = cg->playerEntity.vLastMoveOrg[1] * delta + origin[1];
        origin[2] = cg->playerEntity.vLastMoveOrg[2] * delta + origin[2];
    }
}

void CalculateWeaponPosition_BasePosition(float *origin)
{
    vec3_t vGunOfs;

    vGunOfs[0] = 0.0f;
    vGunOfs[1] = 0.0f;
    vGunOfs[2] = 0.0f;
    CalculateWeaponPosition_BasePosition_movement(vGunOfs);
    cg->playerEntity.vPositionLastOrg[0] = vGunOfs[0];
    cg->playerEntity.vPositionLastOrg[1] = vGunOfs[1];
    cg->playerEntity.vPositionLastOrg[2] = vGunOfs[2];
    origin[0] = vGunOfs[0] + origin[0];
    origin[1] = vGunOfs[1] + origin[1];
    origin[2] = vGunOfs[2] + origin[2];
}

inline void VectorCopy(const vec3_t a, vec3_t b)
{
    b[0] = a[0];
    b[1] = a[1];
    b[2] = a[2];
}
inline void VectorMA(const vec3_t v, const float s, const vec3_t b, vec3_t o)
{
    o[0] = v[0] + s * b[0];
    o[1] = v[1] + s * b[1];
    o[2] = v[2] + s * b[2];
}
void CalculateWeaponPosition_ToWorldPosition(float *origin)
{
    vec3_t vOffset;
    vec3_t axis[3];

    VectorCopy(origin, vOffset);
    AngleVectors(cg->viewModelAngles, axis[0], axis[1], axis[2]);
    VectorCopy(cg->viewModelOrigin, origin);
    VectorMA(origin, vOffset[0], axis[0], origin);
    VectorMA(origin, -vOffset[1], axis[1], origin);
    VectorMA(origin, vOffset[2], axis[2], origin);
}

void CalculateWeaponPosition_SaveOffsetMovement(float *origin)
{
    if (BG_IsAimDownSightWeapon(BG_GetViewmodelWeaponIndex(&cg->predictedPlayerState))
        && cg->predictedPlayerState.fWeaponPosFrac != 0.0f)
    {
        cg->gunXOfs = (origin[0] - cg->refdef.vieworg[0]) * cg->predictedPlayerState.fWeaponPosFrac;
        cg->gunYOfs = (origin[1] - cg->refdef.vieworg[1]) * cg->predictedPlayerState.fWeaponPosFrac;
        cg->gunZOfs = (origin[2] - cg->refdef.vieworg[2]) * cg->predictedPlayerState.fWeaponPosFrac;
    }
    else
    {
        cg->gunXOfs = 0.0f;
        cg->gunYOfs = 0.0f;
        cg->gunZOfs = 0.0f;
    }
}

void CalculateWeaponPosition(float *origin)
{
    int delta;
    float fLean;
    float fDist;
    vec3_t tempAngles;
    vec3_t right;

    origin[0] = 0.0f;
    origin[1] = 0.0f;
    origin[2] = 0.0f;

    if (cg->predictedPlayerState.leanf != 0.0f && cg->predictedPlayerState.fWeaponPosFrac < 1.0f)
    {
        tempAngles[0] = 0.0f;
        tempAngles[1] = 0.0f;
        fLean = GetLeanFraction(cg->predictedPlayerState.leanf);
        tempAngles[2] = 0.0f - (fLean + fLean);
        fDist = (1.0f - cg->predictedPlayerState.fWeaponPosFrac) * fLean * 1.6f;
        AngleVectors(tempAngles, NULL, right, NULL);
        VectorMA(origin, fDist, right, origin);
    }

    CalculateWeaponPosition_BasePosition(origin);
    CalculateWeaponPosition_SwayMovement(origin);
    CalculateWeaponPosition_ToWorldPosition(origin);

    delta = cg->time - cg->landTime;
    if (delta < 150)
        origin[2] = (float)delta * cg->landChange * 0.25f / 150.0f + origin[2];
    else if (delta < 450)
        origin[2] = (float)(450 - delta) * cg->landChange * 0.25f / 300.0f + origin[2];

    CalculateWeaponPosition_SaveOffsetMovement(origin);
}

void CalculateWeaponPostion_PositionToADS(void)
{
    if (BG_IsAimDownSightWeapon(BG_GetViewmodelWeaponIndex(&cg->predictedPlayerState)))
    {
        if (cg->predictedPlayerState.fWeaponPosFrac == 1.0f)
            cg->playerEntity.bPositionToADS = 0;
        else if (cg->predictedPlayerState.fWeaponPosFrac == 0.0f)
            cg->playerEntity.bPositionToADS = 1;
        cg->playerEntity.fLastWeaponPosFrac = cg->predictedPlayerState.fWeaponPosFrac;
    }
}

// Name from CoD4; original CoD2 name unknown.
static void WeaponFlash(int dobjHandle, int weaponNum, int bViewFlash, const vec_t *origin, unsigned int flashTag)
{
    void *fx;
    int bolt[2];

    if (bViewFlash)
        fx = cg_weaponsArray[weaponNum].viewFlashEffect;
    else
        fx = cg_weaponsArray[weaponNum].worldFlashEffect;

    if (!fx)
        return;

    bolt[0] = dobjHandle;
    bolt[1] = FX_GetBoneIndex(dobjHandle, flashTag);
    if (bolt[1] < 0)
        return;

    FX_PlayEntityEffect( (EffectTemplate *)(fx), origin, 0, (const FxBoltInfo *)(bolt));
}

void CG_HoldBreathInit(void)
{
    cg->holdBreathTime = -1;
    cg->holdBreathInTime = 0;
    cg->holdBreathDelay = 0;
    cg->holdBreathFrac = 0.0f;
}

static void CG_SetHoldBreathChannelVolumes(float frac)
{
    float volumes[11];
    int i;

    if (frac == 0.0f) {
        SND_DeactivateChannelVolumes(1, 0);
        return;
    }

    for (i = 0; i < 11; i++)
        volumes[i] = 1.0f + (cgs->holdBreathParams.sound.channelvolume[i] - 1.0f) * frac;

    SND_SetChannelVolumes(1, volumes, 0);
}

static void CG_UpdateHoldBreathSound(void)
{
    int delayMsec;

    if (cg->holdBreathDelay > 0)
        cg->holdBreathDelay -= cg->frametime;

    if (cg->predictedPlayerState.pm_flags & 0x4000) {
        cg->holdBreathFrac = DiffTrack(1.0f, cg->holdBreathFrac,
                                       player_breath_snd_lerp->current.value,
                                       (float)cg->frametime * 0.001f);

        if (cg->holdBreathTime < 0) {
            cg->holdBreathTime = 0;
            if (cg->holdBreathDelay <= 0) {
                cg->holdBreathInTime = CG_PlayClientSoundAlias(cgs->media.playerBreathInSound);
                cg->holdBreathDelay = (int)(player_breath_snd_delay->current.value * 1000.0f);
            } else {
                cg->holdBreathInTime = 0;
            }
        } else if (cg->holdBreathTime > cg->holdBreathInTime) {
            CG_PlayClientSoundAlias(cgs->media.playerHeartBeatSound);
        }

        cg->holdBreathTime += cg->frametime;
    } else {
        if (cg->holdBreathTime >= 0) {
            cg->holdBreathTime += cg->frametime;
            if (cg->holdBreathTime > (int)(player_breath_hold_time->current.value * 1000.0f)) {
                CG_PlayClientSoundAlias(cgs->media.playerBreathGaspSound);
            } else if (cg->holdBreathDelay <= 0) {
                CG_PlayClientSoundAlias(cgs->media.playerBreathOutSound);
                cg->holdBreathDelay = (int)(player_breath_snd_delay->current.value * 1000.0f);
            }
        }

        cg->holdBreathTime = -1;
        cg->holdBreathInTime = 0;
        cg->holdBreathFrac = 0.0f;
    }

    CG_SetHoldBreathChannelVolumes(cg->holdBreathFrac);
}

void CG_AddPlayerWeapon(GfxEntity *parent, playerState_t *ps, centity_t *cent, qboolean bDrawGun)
{
    entityState_t *ent;
    weaponInfo_t *weapInfo;
    int weaponNum;
    qboolean bViewModel;
    qboolean isLocalClientEntity;

    ent = &cent->nextState;
    isLocalClientEntity = CG_IsLocalClientEntity(cg, ent);

    if (ps) {
        bViewModel = 1;
        weaponNum = BG_GetViewmodelWeaponIndex(ps);
    } else {
        bViewModel = 0;
        weaponNum = ent->weapon;
    }

    if (weaponNum <= 0)
        return;

    if (ent->eFlags & 0x300)
        return;

    ;

    if (bViewModel) {
        if (bDrawGun) {
            GfxEntity gun;
            GfxModel model;
            GfxEntity *skinEnt;

            memset(&gun, 0, sizeof(gun));
            gun.reType = RT_DOBJ;
            gun.renderFxFlags = 0x8c;
            gun.materialRGBA[0] = 0xff;
            gun.materialRGBA[1] = 0xff;
            gun.materialRGBA[2] = 0xff;
            gun.materialRGBA[3] = 0xff;

            gun.lighting.origin[0] = ps->origin[0];
            gun.lighting.origin[1] = ps->origin[1];
            gun.lighting.origin[2] = ps->origin[2] + ps->viewHeightCurrent;
            AddLeanToPosition(gun.lighting.origin, ps->viewangles[1],
                              ps->leanf, 16.0f, 20.0f);

            gun.origin[0] = parent->origin[0];
            gun.origin[1] = parent->origin[1];
            gun.origin[2] = parent->origin[2];
            AxisCopy(parent->axis, gun.axis);

            model.obj = (&cg_weaponsArray[weaponNum])->viewModelDObj;
            skinEnt = R_AddRefEntityToScene(&gun, model, 0);
#if COD2_TRACE
            {
                static int apwTrace;
                if (apwTrace < 8) {
                    if (getenv("DBGSPAM"))
                        fprintf(stderr, "[viewmodel] AddPlayerWeapon: dObj=%p skinEnt=%p gunOrg=(%.0f,%.0f,%.0f) lightOrg=(%.0f,%.0f,%.0f) registered=%d\n",
                                (void *)(&cg_weaponsArray[weaponNum])->viewModelDObj, (void *)skinEnt,
                                gun.origin[0], gun.origin[1], gun.origin[2],
                                gun.lighting.origin[0], gun.lighting.origin[1], gun.lighting.origin[2],
                                (&cg_weaponsArray[weaponNum])->registered);
                    ++apwTrace;
                }
            }
#endif
            if (skinEnt)
                R_SkinGfxEntityDelayed(skinEnt);
        }

        cg->viewModelOrigin[0] = parent->origin[0];
        cg->viewModelOrigin[1] = parent->origin[1];
        cg->viewModelOrigin[2] = parent->origin[2];
        AxisCopy(parent->axis, cg->viewModelAxis);
        AxisToAngles(cg->viewModelAxis, cg->viewModelAngles);

        {
            int boneIndex;

            boneIndex = DObjGetBoneIndex((&cg_weaponsArray[weaponNum])->viewModelDObj, scr_const.tag_brass);
            if (boneIndex > -1) {
                DObjAnimMat *rotTransArray = DObjGetRotTransArray((&cg_weaponsArray[weaponNum])->viewModelDObj);
                if (rotTransArray) {
                    vec3_t entAxis[4];

                    VectorCopy(parent->axis[0], entAxis[0]);
                    VectorCopy(parent->axis[1], entAxis[1]);
                    VectorCopy(parent->axis[2], entAxis[2]);
                    VectorCopy(parent->origin, entAxis[3]);
                    MatrixTransformVector43(rotTransArray[boneIndex].trans, entAxis, ejectBrassCasingOrigin);
                }
            }
        }

        CG_UpdateHoldBreathSound();
    }

    if (!cent->bMuzzleFlash)
        return;

    if (isLocalClientEntity && !bViewModel)
        return;

    cent->bMuzzleFlash = 0;

    if (!bDrawGun)
        return;

    if (bViewModel)
        WeaponFlash(CG_WeaponDObjHandle(weaponNum), weaponNum, 1, cg->viewModelOrigin, *s_barrelTags[0]);
    else
        WeaponFlash(ent->number, weaponNum, 0, cent->lerpOrigin, *s_barrelTags[0]);
}

void CG_UpdateViewWeaponAnim(playerState_t *ps)
{
    byte *psBytes;
    weaponInfo_t *weapInfo;
    int weaponIndex;
    int partBits[4];

    psBytes = (byte *)ps;

    if (*(int *)(psBytes + 4) >= 6) {
        CG_ResetViewWeaponAnimTrees(ps);
        return;
    }

    weaponIndex = BG_GetViewmodelWeaponIndex(ps);
    if (weaponIndex <= 0)
        return;

    CG_RegisterWeapon(weaponIndex);
    weapInfo = &((weaponInfo_t *)cg_weaponsArray)[weaponIndex];
    CG_WeaponRunXModelAnims(ps, weapInfo);

    DObjUpdateClientInfo(weapInfo->viewModelDObj, (float)cg->frametime * 0.0010000000474974513f);

    memset(partBits, 0xFF, sizeof(partBits));

    if (!CL_DObjCreateSkelForBones(weapInfo->viewModelDObj, partBits, 0)) {
        DObjCalcAnim(weapInfo->viewModelDObj, partBits);
        DObjCalcSkel((DObj *)weapInfo->viewModelDObj, partBits);
    }

    CG_PlayViewWeaponNotetrackSounds();
}
void CG_AddViewWeapon(playerState_t *ps)
{
    int drawGun;
    int weaponIndex;
    float fZoom;
    vec3_t angles;
    vec3_t vAxis[3];
    vec3_t vAxis2[3];
    weaponState_t ws;
    GfxEntity hand;

    drawGun = 1;

    if (ps->pm_type == 4 || ps->pm_type == 5)
        return;

    if (cg->renderingThirdPerson)
        return;

    if (cg->cubemapShot || !cg_drawGun->current.enabled || CG_GetWeapReticleZoom(&fZoom))
        drawGun = 0;

    if (cg->testGun)
        return;

    if (ps->eFlags & 0x300)
        return;

    weaponIndex = BG_GetViewmodelWeaponIndex(ps);
    if (weaponIndex > 0)
    {
        CalculateWeaponPosition_Sway();
        CalculateWeaponPostion_PositionToADS();
        memset(&hand, 0, sizeof(hand));
        CalculateWeaponPosition(hand.origin);
        VectorMA(hand.origin, cg_gun_x->current.value, cg->viewModelAxis[0], hand.origin);
        VectorMA(hand.origin, cg_gun_y->current.value, cg->viewModelAxis[1], hand.origin);
        VectorMA(hand.origin, cg_gun_z->current.value, cg->viewModelAxis[2], hand.origin);
        AnglesToAxis(cg->viewModelAngles, vAxis2);

        ws.ps = ps;
        ws.xyspeed = cg->xyspeed;
        ws.frametime = (float)cg->frametime * 0.001f;
        ws.vLastMoveAng[0] = cg->playerEntity.vLastMoveAng[0];
        ws.vLastMoveAng[1] = cg->playerEntity.vLastMoveAng[1];
        ws.vLastMoveAng[2] = cg->playerEntity.vLastMoveAng[2];
        ws.fLastIdleFactor = cg->playerEntity.fLastIdleFactor;
        ws.time = cg->time - ps->deltaTime;
        if (cg->damageTime)
            ws.damageTime = cg->damageTime - ps->deltaTime;
        else
            ws.damageTime = 0;
        ws.v_dmg_pitch = cg->v_dmg_pitch;
        ws.v_dmg_roll = cg->v_dmg_roll;
        ws.vGunOffset[0] = cg->vGunOffset[0];
        ws.vGunOffset[1] = cg->vGunOffset[1];
        ws.vGunOffset[2] = cg->vGunOffset[2];
        ws.vGunSpeed[0] = cg->vGunSpeed[0];
        ws.vGunSpeed[1] = cg->vGunSpeed[1];
        ws.vGunSpeed[2] = cg->vGunSpeed[2];
        ws.swayAngles[0] = cg->swayAngles[0];
        ws.swayAngles[1] = cg->swayAngles[1];
        ws.swayAngles[2] = cg->swayAngles[2];
        ws.weapIdleTime = &cg->weapIdleTime;
        BG_CalculateWeaponAngles(&ws, angles);
        AnglesToAxis(angles, vAxis);
        MatrixMultiply(vAxis, vAxis2, hand.axis);

        if (BG_IsAimDownSightWeapon(ps->weapon) && ps->fWeaponPosFrac != 0.0f
            && !BG_GetWeaponDef(weaponIndex)->overlayReticle)
        {
            AxisToAngles(hand.axis, angles);
            cg->gunPitch = AngleNormalize360(angles[0]);
            cg->gunYaw = AngleNormalize360(angles[1]);
        }
        else
        {
            cg->gunPitch = cg->refdefViewAngles[0];
            cg->gunYaw = cg->refdefViewAngles[1];
        }

        cg->playerEntity.vLastMoveAng[0] = ws.vLastMoveAng[0];
        cg->playerEntity.vLastMoveAng[1] = ws.vLastMoveAng[1];
        cg->playerEntity.vLastMoveAng[2] = ws.vLastMoveAng[2];
        cg->playerEntity.fLastIdleFactor = ws.fLastIdleFactor;
        cg->vGunOffset[0] = ws.vGunOffset[0];
        cg->vGunOffset[1] = ws.vGunOffset[1];
        cg->vGunOffset[2] = ws.vGunOffset[2];
        cg->vGunSpeed[0] = ws.vGunSpeed[0];
        cg->vGunSpeed[1] = ws.vGunSpeed[1];
        cg->vGunSpeed[2] = ws.vGunSpeed[2];
        hand.renderFxFlags = 0xc;
        CG_AddPlayerWeapon(&hand, ps, &cg->predictedPlayerEntity, drawGun);
    }
    else
    {
        cg->gunPitch = cg->refdefViewAngles[0];
        cg->gunYaw = cg->refdefViewAngles[1];
        cg->gunXOfs = 0.0f;
        cg->gunYOfs = 0.0f;
        cg->gunZOfs = 0.0f;
    }
}

static qboolean CG_WeaponSelectable(int weapon)
{
    return (cg->predictedPlayerState.weapons[weapon >> 5] & (1 << (weapon & 0x1f))) != 0;
}

void CG_AltWeapon_f(void)
{
    int alt;

    if (!cg->nextSnap)
        return;
    if (cg->predictedPlayerState.pm_flags & 0x8000)
        return;
    if (!(cg->nextSnap->ps.pm_flags & 0x800000))
        return;
    if (cg->predictedPlayerState.eFlags & 0x300)
        return;
    if (cg->time - cg->weaponSelectTime < cg_weaponCycleDelay->current.integer)
        return;

    alt = BG_GetWeaponDef(cg->weaponSelect)->iAltWeaponIndex;
    if (!alt)
    {
        CG_GameMessage(SEH_LocalizeTextMessage("CGAME_THIS_WEAPON_HAS_NO_ALTERNATE", "game message", (msgLocErrType_t)0));
        return;
    }

    if (CG_WeaponSelectable(alt))
        CG_SelectWeaponIndex(alt);
}

void CG_NextWeapon_f(void)
{
    if (cg->showScores)
    {
        CG_ScrollScoreboardUp();
        return;
    }

    if (!cg->nextSnap)
        return;
    if (cg->predictedPlayerState.pm_flags & 0x8000)
        return;
    if (!(cg->nextSnap->ps.pm_flags & 0x800000))
        return;
    if (cg->time - cg->weaponSelectTime < cg_weaponCycleDelay->current.integer)
        return;

    cg->weaponSelectTime = cg->time;
    CG_CycleWeap(1, 0);
}

void CG_PrevWeapon_f(void)
{
    if (cg->showScores)
    {
        CG_ScrollScoreboardDown();
        return;
    }

    if (!cg->nextSnap)
        return;
    if (cg->predictedPlayerState.pm_flags & 0x8000)
        return;
    if (!(cg->nextSnap->ps.pm_flags & 0x800000))
        return;
    if (cg->time - cg->weaponSelectTime < cg_weaponCycleDelay->current.integer)
        return;

    cg->weaponSelectTime = cg->time;
    CG_CycleWeap(0, 0);
}

void CG_OutOfAmmoChange(void)
{
    WeaponDef *weapDef;
    int slots[2];
    int i;

    if (!cg->nextSnap)
        return;

    weapDef = BG_GetWeaponDef(cg->predictedPlayerState.weapon);

    if (weapDef->bSlotStackable)
    {
        for (i = 1; i <= BG_GetNumWeapons(); i++)
        {
            if (!CG_WeaponSelectable(i))
                continue;
            if (!BG_GetWeaponDef(i)->bSlotStackable)
                continue;
            if (BG_GetWeaponDef(i)->weapSlot != weapDef->weapSlot)
                continue;
            if (BG_WeaponAmmo(&cg->predictedPlayerState, i))
            {
                CG_SelectWeaponIndex(i);
                return;
            }
        }
    }

    if (BG_IsPlayerWeaponInSlot(&cg->predictedPlayerState, cg->predictedPlayerState.weapon, 1))
    {
        slots[0] = 1;
        slots[1] = 2;

        for (i = 0; i < sizeof(slots) / sizeof(slots[0]); i++)
        {
            if (!cg->predictedPlayerState.weaponslots[slots[i]])
                continue;
            if (BG_WeaponAmmo(&cg->predictedPlayerState, (signed char)cg->predictedPlayerState.weaponslots[slots[i]]))
            {
                CG_SelectWeaponIndex((signed char)cg->predictedPlayerState.weaponslots[slots[i]]);
                return;
            }
        }
    }

    CG_CycleWeap(1, 1);
}

void CG_WeaponSlot_f(void)
{
    int slot;
    int weapon;

    if (!cg->nextSnap)
        return;
    if (cg->predictedPlayerState.pm_flags & 0x8000)
        return;
    if (!(cg->nextSnap->ps.pm_flags & 0x800000))
        return;
    if (cg->time - cg->weaponSelectTime < cg_weaponCycleDelay->current.integer)
        return;
    cg->weaponSelectTime = cg->time;

    slot = BG_GetWeaponSlotForName(CG_Argv(1));
    if (!slot)
        slot = atoi(CG_Argv(1));
    if (slot <= 0 || slot >= 3)
        return;

    weapon = (signed char)cg->predictedPlayerState.weaponslots[slot];
    if (!weapon)
        return;
    if (!CG_WeaponSelectable(weapon))
        return;

    CG_SelectWeaponIndex(weapon);
}

static qboolean CG_SelectFirstWeaponInSlot(qboolean next, qboolean checkAmmo)
{
    int slot;
    int step;

    if (next)
    {
        step = 1;
        slot = 1;
    }
    else
    {
        step = -1;
        slot = 2;
    }

    for (; slot >= 1 && slot < 3; slot += step)
    {
        if (!cg->predictedPlayerState.weaponslots[slot])
            continue;
        if (!checkAmmo || BG_WeaponAmmo(&cg->predictedPlayerState, (signed char)cg->predictedPlayerState.weaponslots[slot]))
        {
            CG_SelectWeaponIndex((signed char)cg->predictedPlayerState.weaponslots[slot]);
            return 1;
        }
    }

    return 0;
}

qboolean CG_SelectFirstWeaponNotInSlot(qboolean next, qboolean checkAmmo)
{
    WeaponDef *weapDef;
    int weapon;
    int step;

    if (next)
    {
        step = 1;
        weapon = 1;
    }
    else
    {
        weapon = BG_GetNumWeapons();
        step = -1;
    }

    for (; weapon >= 1 && weapon <= BG_GetNumWeapons(); weapon += step)
    {
        if (!CG_WeaponSelectable(weapon))
            continue;
        if (BG_IsPlayerWeaponInSlot(&cg->predictedPlayerState, weapon, 1))
            continue;

        weapDef = BG_GetWeaponDef(weapon);
        if (weapDef->bSlotStackable && weapDef->weapSlot > 0 && weapDef->weapSlot <= 2)
        {
            if (!cg->predictedPlayerState.weaponslots[1]
                || BG_GetWeaponDef((signed char)cg->predictedPlayerState.weaponslots[1])->bSlotStackable
                || !cg->predictedPlayerState.weaponslots[2]
                || BG_GetWeaponDef((signed char)cg->predictedPlayerState.weaponslots[2])->bSlotStackable)
                continue;
        }
        if (weapDef->weapClass == 9)
            continue;
        if (weapDef->offhandClass)
            continue;
        if (checkAmmo && !BG_WeaponAmmo(&cg->predictedPlayerState, weapon))
            continue;

        CG_SelectWeaponIndex(weapon);
        return 1;
    }

    return 0;
}

void CG_CycleWeap(qboolean next, qboolean checkAmmo)
{
    WeaponDef *weapDef;
    int slot;
    int endSlot;
    int step;
    int endWeapon;
    int weapon;

    if (!cg->nextSnap)
        return;
    if (!(cg->nextSnap->ps.pm_flags & 0x800000))
        return;

    if (next)
    {
        step = 1;
        endSlot = 1;
        endWeapon = 1;
    }
    else
    {
        step = -1;
        endSlot = 2;
        endWeapon = BG_GetNumWeapons();
    }

    weapon = cg->weaponSelect;
    slot = BG_IsPlayerWeaponInSlot(&cg->predictedPlayerState, weapon, 1);
    if (!slot)
        slot = BG_GetStackSlotForWeapon(&cg->predictedPlayerState, weapon, (weapSlot_t)0);

    if (slot)
    {
        for (slot = (slot + step + 1) % 2 + 1; slot != endSlot; slot = (slot + step + 1) % 2 + 1)
        {
            if (!cg->predictedPlayerState.weaponslots[slot])
                continue;
            if (!checkAmmo || BG_WeaponAmmo(&cg->predictedPlayerState, (signed char)cg->predictedPlayerState.weaponslots[slot]))
            {
                CG_SelectWeaponIndex((signed char)cg->predictedPlayerState.weaponslots[slot]);
                return;
            }
        }

        if (CG_SelectFirstWeaponNotInSlot(next, checkAmmo))
            return;
        CG_SelectFirstWeaponInSlot(next, checkAmmo);
    }
    else
    {
        while (1)
        {
            weapon = (BG_GetNumWeapons() + step - 1 + weapon) % BG_GetNumWeapons() + 1;
            if (weapon == endWeapon)
                break;
            if (!CG_WeaponSelectable(weapon))
                continue;
            if (BG_IsPlayerWeaponAnAlt(weapon, cg->weaponSelect))
                continue;
            if (BG_IsPlayerWeaponInSlot(&cg->predictedPlayerState, weapon, 1))
                continue;

            weapDef = BG_GetWeaponDef(weapon);
            if (weapDef->bSlotStackable && weapDef->weapSlot > 0 && weapDef->weapSlot <= 2)
            {
                if (!cg->predictedPlayerState.weaponslots[1]
                    || BG_GetWeaponDef((signed char)cg->predictedPlayerState.weaponslots[1])->bSlotStackable
                    || !cg->predictedPlayerState.weaponslots[2]
                    || BG_GetWeaponDef((signed char)cg->predictedPlayerState.weaponslots[2])->bSlotStackable)
                    continue;
            }
            if (checkAmmo && !BG_WeaponAmmo(&cg->predictedPlayerState, weapon))
                continue;
            if (weapDef->weapClass == 9)
                continue;
            if (weapDef->offhandClass)
                continue;

            CG_SelectWeaponIndex(weapon);
            return;
        }

        if (CG_SelectFirstWeaponInSlot(next, checkAmmo))
            return;
        CG_SelectFirstWeaponNotInSlot(next, checkAmmo);
    }

    if (!CG_WeaponSelectable(cg->weaponSelect))
        CG_SelectWeaponIndex(0);
}

void CG_EjectWeaponBrass(entityState_t *ent, int event)
{
    byte *entBytes;
    weaponInfo_t *weapInfo;
    snapshot_t *snap;
    int weaponNum;
    int bolt[2];
    void *effect;
    unsigned int tagBrass;

    if (!cg_brass->current.enabled)
        return;

    entBytes = (byte *)ent;
    if (*(int *)(entBytes + 4) >= 10)
        return;

    weaponNum = *(int *)(entBytes + 0xc8);
    if (!weaponNum)
        return;

    if (weaponNum > BG_GetNumWeapons()) {
        Com_Error(ERR_DROP, "\x15"
                     "CG_EjectWeaponBrass: ent->weapon > BG_GetNumWeapons()");
        return;
    }

    weapInfo = &cg_weaponsArray[weaponNum];
    effect = weapInfo->lastShotEjectEffect;
    if (effect && event == 0xa0) {
        snap = cg->nextSnap;
        if ((snap->ps.pm_flags & 0xc00000) && ent->number == snap->ps.clientNum)
            bolt[0] = weaponNum + 0x400;
        else
            bolt[0] = ent->number;

        tagBrass = scr_const.tag_brass;
        bolt[1] = FX_GetBoneIndex(bolt[0], tagBrass);
        if (bolt[1] >= 0)
            FX_PlayEntityEffect( (EffectTemplate *)(effect), ejectBrassCasingOrigin, 0, (const FxBoltInfo *)(bolt));
    } else {
        effect = weapInfo->shellEjectEffect;
        if (!effect)
            return;

        snap = cg->nextSnap;
        if ((snap->ps.pm_flags & 0xc00000) && ent->number == snap->ps.clientNum)
            bolt[0] = weaponNum + 0x400;
        else
            bolt[0] = ent->number;

        tagBrass = scr_const.tag_brass;
        bolt[1] = FX_GetBoneIndex(bolt[0], tagBrass);
        if (bolt[1] >= 0)
            FX_PlayEntityEffect( (EffectTemplate *)(effect), ejectBrassCasingOrigin, 0, (const FxBoltInfo *)(bolt));
    }
}

void CG_FireWeapon(centity_t *cent, int event, int barrel)
{
    entityState_t *ent;
    weaponInfo_t *weapInfo;
    WeaponDef *weapDef;
    snd_alias_list_t *fireSound;
    vec3_t origin;
    int duration;
    qboolean localTurret;
    struct DObj_s *viewModel;
    struct DObj_s *obj;
    int hasTag;

    ent = &cent->nextState;
    if (!ent->weapon)
        return;

    if (ent->weapon > BG_GetNumWeapons()) {
        Com_Error(ERR_DROP, "\x15"
                     "CG_FireWeapon: ent->weapon > BG_GetNumWeapons()");
        return;
    }

    cent->bMuzzleFlash = 1;
    weapInfo = &cg_weaponsArray[ent->weapon];

    if (CG_IsLocalClientEntity(cg, ent))
        BG_WeaponFireRecoil(&cg->predictedPlayerState, cg->vGunSpeed, cg->kickAVel);

    localTurret = 0;
    if (ent->eType == 9 && !cg->renderingThirdPerson && (cg->nextSnap->ps.eFlags & 0x300) && cg->nextSnap->ps.viewlocked_entNum == ent->number)
        localTurret = 1;
    if (ent->eType == 9)
        WeaponFlash(ent->number, ent->weapon, localTurret, cent->lerpOrigin, *s_barrelTags[barrel]);

    fireSound = weapInfo->flashSound;
    if (weapInfo->flashSoundPlayer && CG_IsLocalClientEntity(cg, ent))
        fireSound = weapInfo->flashSoundPlayer;

    if (event == 0xa0) {
        if (weapInfo->lastShotSoundPlayer && CG_IsLocalClientEntity(cg, ent))
            fireSound = weapInfo->lastShotSoundPlayer;
        else if (weapInfo->lastShotSound)
            fireSound = weapInfo->lastShotSound;
    }

    if (fireSound) {
        if (CG_IsLocalClientEntity(cg, ent)) {
            viewModel = weapInfo->viewModelDObj;
            hasTag = viewModel && CG_DObjGetViewModelTagPos(viewModel, scr_const.tag_flash, origin);
        } else {
            obj = (DObj_s *)(Com_GetClientDObj(ent->number, cent->localClientNum));
            hasTag = obj && CG_DObjGetWorldTagPos(cent, obj, scr_const.tag_flash, origin);
        }
        if (!hasTag)
            BG_EvaluateTrajectory(&ent->pos, cg->time, origin);
        duration = CG_PlaySoundAlias(ent->number, origin, fireSound);
        if (cent->nextState.eType == 1)
            CG_CompassAddWeaponPingInfo(cent, origin, duration);
    }

    weapDef = (WeaponDef *)BG_GetWeaponDef(ent->weapon);
    if (!weapDef->bBoltAction)
        CG_EjectWeaponBrass(ent, event);
}

static __forceinline void Vec3SubInl(const vec_t *a, const vec_t *b, vec_t *out)
{
    out[0] = a[0] - b[0];
    out[1] = a[1] - b[1];
    out[2] = a[2] - b[2];
}
static __forceinline float Vec3DotInl(const vec_t *a, const vec_t *b)
{
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}
static __forceinline float Vec3LengthInl(const vec_t *v)
{
    return sqrtf(v[2] * v[2] + (v[1] * v[1] + v[0] * v[0]));
}
void CG_WhizbySound(vec_t *vStart, vec_t *vEnd)
{
    vec3_t viewDelta;
    vec3_t delta;
    vec3_t dir;
    vec3_t projPos;
    float dist;
    float viewDist;

    Vec3SubInl(vEnd, vStart, delta);
    Vec3NormalizeTo(delta, dir);
    dist = Vec3DotInl(dir, delta);
    Vec3SubInl(cg->refdef.vieworg, vStart, viewDelta);
    viewDist = Vec3DotInl(viewDelta, dir);
    if (viewDist < 64.0f)
        return;
    if (viewDist - -64.0f > dist)
        return;
    projPos[0] = viewDist * dir[0] + vStart[0];
    projPos[1] = viewDist * dir[1] + vStart[1];
    projPos[2] = viewDist * dir[2] + vStart[2];
    Vec3SubInl(projPos, cg->refdef.vieworg, delta);
    if (Vec3LengthInl(delta) > 140.0f)
        return;
    projPos[0] = projPos[0] - dir[0] * 16.0f;
    projPos[1] = projPos[1] - dir[1] * 16.0f;
    projPos[2] = projPos[2] - dir[2] * 16.0f;
    CG_PlaySoundAlias(0x3fe, projPos, cgs->media.bulletWhizby);
}

// inlined com_math vector helpers: retail evaluates through their pointer params
static __forceinline void Vec3CopyInl(const vec_t *a, vec_t *out)
{
    out[0] = a[0];
    out[1] = a[1];
    out[2] = a[2];
}
static __forceinline void Vec3ScaleInl(const vec_t *v, float scale, vec_t *out)
{
    out[0] = v[0] * scale;
    out[1] = v[1] * scale;
    out[2] = v[2] * scale;
}
void CG_SpawnTracer(vec_t *pstart, vec_t *pend)
{
    vec3_t start;
    vec3_t end;
    vec3_t dir;
    float dist;
    int time;
    int jitter;
    localEntity_t *le;

    Vec3CopyInl(pstart, start);
    end[0] = pend[0];
    end[1] = pend[1];
    end[2] = pend[2];

    Vec3SubInl(end, start, dir);

    dist = Vec3Normalize(dir);
    le = CG_AllocLocalEntity();
    le->leType = LE_MOVING_TRACER;
    le->tracerClipDist = dist;

    jitter = 0;
    if (cg->frametime)
        jitter = (rand() % cg->frametime) / 2;
    time = cg->time - jitter;

    le->endTime = time - (int)(dist / cg_tracerSpeed->current.value * -1000.0f);
    le->pos.trType = TR_LINEAR;
    le->pos.trTime = time;

    Vec3CopyInl(start, le->pos.trBase);
    Vec3ScaleInl(dir, cg_tracerSpeed->current.value, le->pos.trDelta);
}

// retail 0x4d1340; Kisak name (SUPPORTED)
static float CalcTracerFinalScale(float tracerScaleDistRange, float dist, float tracerScale)
{
    float scale;

    if (tracerScaleDistRange > 0.0f) {
        scale = dist / tracerScaleDistRange * tracerScale;
        if (scale < 1.0f)
            return 1.0f;
        if (scale > tracerScale)
            return tracerScale;
        return scale;
    }
    return tracerScale;
}

// retail 0x4d1390; Kisak name (SUPPORTED)
static void ScaleTracer(const float *start, const float *finish, float *startWidth, float *finishWidth)
{
    float startDist;
    float finishDist;
    float tracerScale;
    float tracerScaleMinDist;
    float tracerScaleDistRange;

    tracerScale = cg_tracerScale->current.value;
    tracerScaleMinDist = cg_tracerScaleMinDist->current.value;
    tracerScaleDistRange = cg_tracerScaleDistRange->current.value;
    if (tracerScale != 1.0f) {
        startDist = Vec3Distance(start, cg->refdef.vieworg);
        finishDist = Vec3Distance(finish, cg->refdef.vieworg);
        if (tracerScaleMinDist != 0.0f) {
            startDist = startDist - tracerScaleMinDist;
            finishDist = finishDist - tracerScaleMinDist;
        }
        if (startDist > 0.0f)
            *startWidth = CalcTracerFinalScale(tracerScaleDistRange, startDist, tracerScale) * *startWidth;
        if (finishDist > 0.0f)
            *finishWidth = CalcTracerFinalScale(tracerScaleDistRange, finishDist, tracerScale) * *finishWidth;
    }
}

inline void VectorScale(const vec3_t v, const float s, vec3_t o)
{
    o[0] = s * v[0];
    o[1] = s * v[1];
    o[2] = s * v[2];
}
// Q3 CG_Tracer shape
void CG_DrawTracer(vec_t *start, vec_t *finish)
{
    vec3_t forward;
    vec3_t right;
    vec3_t line;
    float scale;
    float startWidth;
    float finishWidth;
    GfxWorldVertex verts[4];

    forward[0] = finish[0] - start[0];
    forward[1] = finish[1] - start[1];
    forward[2] = finish[2] - start[2];

    line[0] = forward[0] * cg->refdef.viewaxis[1][0] + forward[1] * cg->refdef.viewaxis[1][1] + forward[2] * cg->refdef.viewaxis[1][2];
    line[1] = forward[0] * cg->refdef.viewaxis[2][0] + forward[1] * cg->refdef.viewaxis[2][1] + forward[2] * cg->refdef.viewaxis[2][2];

    VectorScale(cg->refdef.viewaxis[1], line[1], right);
    scale = -line[0];
    right[0] = cg->refdef.viewaxis[2][0] * scale + right[0];
    right[1] = cg->refdef.viewaxis[2][1] * scale + right[1];
    right[2] = cg->refdef.viewaxis[2][2] * scale + right[2];
    Vec3Normalize(right);

    startWidth = cg_tracerWidth->current.value;
    finishWidth = cg_tracerWidth->current.value;
    ScaleTracer(start, finish, &startWidth, &finishWidth);

    VectorMA(finish, finishWidth, right, verts[0].xyz);
    verts[0].color.packed = 0xffffffff;
    verts[0].texCoord[0] = 1.0f;
    verts[0].texCoord[1] = 1.0f;
    verts[0].lmapCoord[0] = 1.0f;
    verts[0].lmapCoord[1] = 1.0f;

    VectorMA(finish, -finishWidth, right, verts[1].xyz);
    verts[1].color.packed = 0xffffffff;
    verts[1].texCoord[0] = 1.0f;
    verts[1].texCoord[1] = 0.0f;
    verts[1].lmapCoord[0] = 1.0f;
    verts[1].lmapCoord[1] = 0.0f;

    VectorMA(start, -startWidth, right, verts[2].xyz);
    verts[2].color.packed = 0xffffffff;
    verts[2].texCoord[0] = 0.0f;
    verts[2].texCoord[1] = 0.0f;
    verts[2].lmapCoord[0] = 0.0f;
    verts[2].lmapCoord[1] = 0.0f;

    VectorMA(start, startWidth, right, verts[3].xyz);
    verts[3].color.packed = 0xffffffff;
    verts[3].texCoord[0] = 0.0f;
    verts[3].texCoord[1] = 1.0f;
    verts[3].lmapCoord[0] = 0.0f;
    verts[3].lmapCoord[1] = 1.0f;

    CL_DefaultVertexFrames(4, verts);
    CL_AddPolyToScene(cgs->media.tracerMaterial, 0x1f, 4, verts);
}

static qboolean __attribute_regparm__(3) CG_CalcMuzzlePoint(int entityNum, vec_t *muzzle, unsigned int flashTag)
{
    snapshot_t *snap;
    centity_t *cent;
    void *dobj;
    int entFlags;

    snap = cg->nextSnap;

    if ((snap->ps.pm_flags & 0xc00000) && entityNum == snap->ps.clientNum) {
        muzzle[0] = cg->refdef.vieworg[0];
        muzzle[1] = cg->refdef.vieworg[1];
        muzzle[2] = cg->refdef.vieworg[2];
        return 1;
    }

    cent = &cg_entities[entityNum];
    dobj = Com_GetClientDObj(cent->nextState.number, cent->localClientNum);
    if (!dobj)
        return 0;

    if (CG_DObjGetWorldTagPos(cent, (struct DObj_s *)dobj, flashTag, muzzle))
        return 1;

    muzzle[0] = cent->nextState.pos.trBase[0];
    muzzle[1] = cent->nextState.pos.trBase[1];
    muzzle[2] = cent->nextState.pos.trBase[2];

    if (entityNum >= 64)
        return 1;

    Com_DPrintf("No %s in CG_CalcMuzzlePoint on entity %d.\n", SL_ConvertToString(flashTag), entityNum);

    entFlags = cent->nextState.eFlags;
    if (entFlags & 8)
        muzzle[2] += 11.0f;
    else if (entFlags & 4)
        muzzle[2] += 40.0f;
    else
        muzzle[2] += 60.0f;

    return 1;
}

// retail 0x4d1850, name unknown: tracer + whizby for a bullet hit
static void CG_BulletHitTracer(int sourceEntityNum, vec_t *position, unsigned int flashTag)
{
    vec3_t muzzle;

    if (cg_tracerChance->current.value <= 0.0f)
        return;
    if (!CG_CalcMuzzlePoint(sourceEntityNum, muzzle, flashTag))
        return;
    if (!((cg->nextSnap->ps.pm_flags & 0xc00000) && sourceEntityNum == cg->nextSnap->ps.clientNum)
        && (float)rand() < cg_tracerChance->current.value * 32768.0f)
        CG_SpawnTracer(muzzle, position);
    CG_WhizbySound(muzzle, position);
}

void CG_BulletHitEvent(int sourceEntityNum, vec_t *position, vec_t *normal, vec_t *reflected, int surfType, int event)
{
    snd_alias_list_t *alias;
    // table rows: 23 surface types each
    const FxEffectDef *fxNormal;
    const FxEffectDef *fxReflect;

    if (event == 0xb6) {
        alias = cgs->media.bulletHitSmallSound[surfType];
        fxNormal = cgs->media.fx->table[surfType];
        fxReflect = cgs->media.fx->table[1 * 23 + surfType];
    } else if (event == 0xb7) {
        alias = cgs->media.bulletHitLargeSound[surfType];
        fxNormal = cgs->media.fx->table[2 * 23 + surfType];
        fxReflect = cgs->media.fx->table[3 * 23 + surfType];
    } else {
        alias = cgs->media.shotgunHitSound[surfType];
        fxNormal = cgs->media.fx->table[4 * 23 + surfType];
        fxReflect = cgs->media.fx->table[5 * 23 + surfType];
    }

    if (!cg_blood->current.enabled && surfType == 7) {
        fxNormal = cgs->media.fxNoBloodFleshHit;
        fxReflect = 0;
    }

    CG_PlaySoundAlias(0x3fe, position, alias);

    if (fxNormal)
        FX_PlayEffect((EffectTemplate *)fxNormal, position, normal);

    if (fxReflect)
        FX_PlayEffect((EffectTemplate *)fxReflect, position, reflected);

    CG_BulletHitTracer(sourceEntityNum, position, *s_barrelTags[0]);
}

void CG_BulletHitClientEvent(int sourceEntityNum, vec_t *position, int surfType, int event)
{
    snd_alias_list_t *alias;

    if (event == 0xb9)
        alias = cgs->media.bulletHitSmallSound[surfType];
    else if (event == 0xb7)
        alias = cgs->media.bulletHitLargeSound[surfType];
    else
        alias = cgs->media.shotgunHitSound[surfType];

    CG_PlaySoundAlias(0x3fe, position, alias);
    CG_BulletHitTracer(sourceEntityNum, position, *s_barrelTags[0]);
}

// Name from CoD4; original CoD2 name unknown.
static void ParseWeaponDefFiles(const char **ppszFiles, int iNumFiles)
{
    const char *name;
    int i;

    for (i = 0; i < iNumFiles; i++) {
        name = ppszFiles[i];
        if (BG_GetWeaponIndexForName(name, 0) != i + 1)
            Com_Error(ERR_DROP, "Weapon index mismatch for '%s'", name);
    }
}

void CG_SetupWeaponDef(void)
{
    char szString[0x2000];
    char *pszFiles[0x7f];
    char *scan;
    int iNumFiles;

    memset(pszFiles, 0, sizeof(pszFiles));
    iNumFiles = 0;
    strcpy(szString, CL_GetConfigString(7));
    scan = szString;
    pszFiles[iNumFiles++] = szString;

    while (*scan) {
        if (*scan == ' ') {
            *scan++ = '\0';
            if (*scan && *scan != ' ')
                pszFiles[iNumFiles++] = scan;
        } else {
            scan++;
        }
    }

    ParseWeaponDefFiles((const char **)pszFiles, iNumFiles);
}

void CG_SelectWeaponIndex(int weaponIndex)
{
    int oldWeaponIndex;
    qboolean keepAds;

    cg->weaponSelectTime = cg->time;

    oldWeaponIndex = cg->weaponSelect;
    if (oldWeaponIndex == weaponIndex)
        return;

    keepAds = weaponIndex && weaponIndex == ((WeaponDef *)BG_GetWeaponDef(oldWeaponIndex))->iAltWeaponIndex;
    cg->weaponSelect = weaponIndex;
    CG_MenuShowNotify(1);

    if (!keepAds)
        CL_SetADS(0);
}

static __forceinline qboolean CG_IsLocalClientEntity(cg_t *cg, entityState_t *ent)
{
    snapshot_t *snap = cg->nextSnap;

    return (snap->ps.pm_flags & 0xc00000) && ent->number == snap->ps.clientNum;
}

