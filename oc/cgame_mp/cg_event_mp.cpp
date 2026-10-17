// first declaration fixes the LTCG walk order
void CG_BulletHitEvent(int sourceEntityNum, float *position, float *normal, float *reflected, int surfType, int event);
#include "common_types.h"
#include "imports.h"
#include "headers/PC/cgame_mp/cg_local.h"
#include "bytematch.h"
extern itemInfo_t cg_itemsArray[];
extern weaponInfo_t cg_weaponsArray[];
extern gitem_t bg_itemlist[];
extern const int bg_numItems;
extern const char *eventnames[];
extern const dvar_t *cg_debugEvents;
extern const dvar_t *cg_footsteps;
extern dvar_t *bg_fallDamageMinHeight;
extern dvar_t *bg_fallDamageMaxHeight;
extern const dvar_t *cg_nopredict;
extern const dvar_t *cg_synchronousClients;

extern LegacyHacks *legacyHacks;

extern void Com_Printf(const char *msg, ...);
extern void Com_DPrintf(const char *msg, ...);
extern void Com_Error(errorParm_t code, const char *fmt, ...);
extern WeaponDef * BG_GetWeaponDef(int iWeapon);
extern int BG_WeaponIsClipOnly(int weapon);
extern int CG_PlayEntitySoundAlias(int entitynum, snd_alias_list_t *aliasList);   /* param 2 is a sound-alias ptr, not int (callers pass cgs->media.* which are snd_alias_list_t*) */
extern int CG_PlaySoundAlias(int entNum, const vec_t *origin, snd_alias_list_t *aliasList);
extern int CG_PlaySoundAliasByName(int entNum, const vec_t *origin, const char *aliasName);
extern int CG_PlaySoundAliasAsMasterByName(int entNum, const vec_t *origin, const char *aliasName);
extern void CG_FireWeapon(centity_t *cent, int weaponId, int hand);
extern void CG_EjectWeaponBrass(entityState_t *es, int weaponId);
extern void CG_PrepOffHand(entityState_t *es, int weaponId, int eventParm);
extern void CG_UseOffHand(centity_t *cent, int weaponId, int eventParm);
extern void CG_SetEquippedOffHand(int weaponId);
extern void CG_SelectWeaponIndex(int weaponId);
extern void CG_OutOfAmmoChange(void);
extern void CG_SwitchOffHandCmd(void);
extern void CG_MenuShowNotify(int val);

extern void CG_StartShakeCamera(float scale, int duration, const vec_t *src, float radius);
extern void CG_BulletHitEvent(int sourceEntityNum, vec_t *start, vec_t *position, vec_t *normal, int surfType, int event);
extern void CG_BulletHitClientEvent(int sourceEntityNum, vec_t *position, int surfType, int event);
extern void CG_CompassAddWeaponPingInfo(centity_t *cent, const vec_t *origin, int msec);
extern void CG_PriorityCenterPrint(const char *msg, float scale, int priority);
extern void CL_DeathMessagePrint(const char *attackerName, const float *attackerColor,
                                 const char *targetName, const float *victimColor,
                                 const char *iconShader, float iconWidth, float iconHeight,
                                 const float *iconColor, bool iconHorzFlip);
extern void CG_DrawScoreboard_GetTeamColor(int team, vec_t *color);
extern void CL_SetADS(int val);
extern void CG_CalcEntityLerpPositions(centity_t *cent);
extern void CG_CheckOpenWaitingScriptMenu(void);
extern void ByteToDir(const int b, vec3_t dir);
extern void AngleVectors( const vec3_t angles, vec3_t forward, vec3_t right, vec3_t up );
extern void FX_PlayOrientedEffect(const EffectTemplate *fx, const vec_t *origin, const vec_t *forward, const vec_t *up);
extern void MakeNormalVectors(const vec3_t forward, vec3_t right, vec3_t up);
extern void FX_PlayEntityEffect(const EffectTemplate *fx, const vec_t *org, const vec3_t *axis, const FxBoltInfo *bolt);
extern void FX_WarpTime(int time);
extern int FX_GetBoneIndex(int modelIndex, unsigned int boneName);
extern const char *CL_GetConfigString(int index);
extern unsigned int SL_GetString(const char *str, unsigned int user);
extern void Scr_SetString(scr_string_t *str, unsigned int a2);
extern void I_strncpyz(char *dest, const char *src, int destsize);
extern void I_strncat(char *dest, int maxlen, const char *src);
extern char *va( const char *format, ... );

#define ES_NUMBER 0x00
#define ES_ETYPE 0x04
#define ES_EFLAGS 0x08
#define ES_APOS_BASE 0x3c
#define ES_TIME 0x54
#define ES_ORIGIN2 0x5c
#define ES_ANGLES2 0x68
#define ES_OTHERENTNUM 0x74
#define ES_ATTACKERENTNUM 0x78
#define ES_SURFTYPE 0x88
#define ES_INDEX 0x8c
#define ES_CLIENTNUM 0x90
#define ES_EVENTPARM 0xa0
#define ES_EVENTSEQ 0xa4
#define ES_WEAPON 0xc8
#define ES_LEANF 0xd4
#define ES_DMGFLAGS 0xd8

#define CENT_NEXTSTATE 0xf0
#define CENT_EVPARM 0x190
#define CENT_EVSEQ 0x194
#define CENT_EVENTS 0x198
#define CENT_EVPARMS 0x1a8
#define CENT_NEXTVALID 0x1e0
#define CENT_PREVEVSEQ 0x1e4
#define CENT_MISCTIME 0x1e8
#define CENT_LERPORIGIN 0x1ec
#define CENT_ETYPE 0xf4

#define CG_CLIENTINFO 0xe0900

#define SNAP_FLAGS 0x18
#define SNAP_PS_CLIENTNUM 0xd8
#define SNAP_PS_WEAPON 0x5a0

#define CGS_SND_FOOTSTEP1_FIRST 0xbeb8
#define CGS_SND_FOOTSTEP1_THIRD 0xbf14
#define CGS_SND_FOOTSTEP2_THIRD 0xbf70
#define CGS_SND_FOOTSTEP2_FIRST 0xbfcc
#define CGS_SND_FOOTSTEP3_FIRST 0xc028
#define CGS_SND_FOOTSTEP3_THIRD 0xc084
#define CGS_SND_SPRINT_FIRST 0xc0e0
#define CGS_SND_SPRINT_THIRD 0xc13c
#define CGS_SND_PRONE_LOOP_3P 0xc198
#define CGS_SND_PRONE_LOOP_1P_NONVIEW 0xc19c
#define CGS_SND_PRONE_LOOP_1P_VIEW 0xc1a0
#define CGS_SND_PRONE_STOP_1P 0xc1a4
#define CGS_SND_WEAPON_PICKUP 0xc1a8
#define CGS_SND_NIGHTVISION_ON 0xc1b8
#define CGS_SND_NIGHTVISION_OFF 0xc1bc
#define CGS_SND_NOAMMO 0xbc84
#define CGS_SND_MELEE_HIT 0xbc88
#define CGS_SND_BULLET_WHIZBY 0xbc90
#define CGS_SND_BULLET_FLESH 0xbcec
#define CGS_SND_GRENADE_BOUNCE 0xbd48
#define CGS_FXLOOKUP 0xc200

static const float f_0_01 = 0.009999999776482582f;
static const float f_4_0 = 4.0f;
static const float f_12_0 = 12.0f;
static const float f_24_0 = 24.0f;
static const float f_26_0 = 26.0f;
static const float f_100_0 = 100.0f;
static const float f_0_9 = 0.8999999761581421f;
static const float f_neg16 = -16.0f;
static const float f_1_4 = 1.399999976158142f;
static const float f_2_8 = 2.799999952316284f;

// Impact effect table rows, SURF_TYPECOUNT entries each (cg_effects_load_obj g_TypeName).
#define SURF_TYPECOUNT 23
#define ENTITYNUM_WORLD 1022
#define IMPACT_GRENADE_BOUNCE 8
#define IMPACT_GRENADE_EXPLODE 9
#define IMPACT_ROCKET_EXPLODE 10

void CG_EntityEvent(centity_t *cent, int event);
void CG_PlayEquipmentSound_4da2e0(int entNum, bool isFirstPerson, bool running);
void CG_CheckEvents(centity_t *cent);

void CG_EntityEvent(centity_t *cent, int event);
void CG_CheckEvents(centity_t *cent);

static inline void VectorCopy(const vec3_t a, vec3_t b)
{
    b[0] = a[0];
    b[1] = a[1];
    b[2] = a[2];
}

static char **CG_EventNames(void)
{
    return (char **)((void *)&eventnames);
}

static void PlayProneSound(int entNum, int isFirstPerson, int soundOffset)
{
    snd_alias_list_t *alias = *(snd_alias_list_t **)((char *)cgs + soundOffset);   /* was *(int*) -- truncates the ptr on x64 */
    CG_PlayEntitySoundAlias(entNum, alias);
}

static void CG_Obituary(entityState_t *ent)
{
    const char *iconShader;
    float iconWidth;
    int attacker;
    bool iconHorzFlip;
    int target;
    int mod;
    float victimColor[4];
    float attackerColor[4];
    float iconColor[4];
    char attackerName[34];
    char targetName[34];
    clientInfo_t *victimCI;
    clientInfo_t *attackerCI;

    attackerColor[0] = 1.0f;
    attackerColor[1] = 1.0f;
    attackerColor[2] = 1.0f;
    attackerColor[3] = 1.0f;
    victimColor[0] = 1.0f;
    victimColor[1] = 1.0f;
    victimColor[2] = 1.0f;
    victimColor[3] = 1.0f;
    iconColor[0] = 1.0f;
    iconColor[1] = 1.0f;
    iconColor[2] = 1.0f;
    iconColor[3] = 1.0f;

    target = ent->otherEntityNum;
    attacker = ent->attackerEntityNum;
    iconWidth = 1.4f;
    iconHorzFlip = 0;

    if (ent->eventParm & 0x80)
    {
        mod = ent->eventParm & ~0x80;
        iconShader = "killicondied";
    }
    else
    {
        WeaponDef *weapDef = BG_GetWeaponDef(ent->eventParm);

        mod = 0;
        if (*weapDef->killIcon)
        {
            iconShader = weapDef->killIcon;
            if (weapDef->wideKillIcon)
                iconWidth = 2.8f;
            if (weapDef->flipKillIcon)
                iconHorzFlip = 1;
        }
        else
        {
            iconShader = "killicondied";
        }
    }

    switch (mod)
    {
    case 7:
        iconWidth = 1.4f;
        iconShader = "killiconmelee";
        break;
    case 8:
        iconWidth = 1.4f;
        iconShader = "killiconheadshot";
        break;
    case 9:
        iconWidth = 1.4f;
        iconShader = "killiconcrush";
        break;
    case 11:
        iconWidth = 1.4f;
        iconShader = "killiconfalling";
        break;
    case 12:
        iconWidth = 1.4f;
        iconShader = "killiconsuicide";
        break;
    case 13:
        iconWidth = 1.4f;
        iconShader = "killicondied";
        break;
    }

    if (target < 0 || target >= 64)
        Com_Error(ERR_DROP, "\x15" "CG_Obituary: target out of range");

    victimCI = &cg->bgs.clientinfo[target];
    if (!victimCI->infoValid)
        return;

    I_strncpyz(targetName, victimCI->name, 32);
    I_strncat(targetName, 34, "^7");
    CG_DrawScoreboard_GetTeamColor(victimCI->oldteam, victimColor);

    if (!cg->bgs.clientinfo[cg->clientNum].infoValid)
        return;

    if (attacker >= 0 && attacker < 64)
    {
        attackerCI = &cg->bgs.clientinfo[attacker];
        if (!attackerCI->infoValid)
            return;

        I_strncpyz(attackerName, attackerCI->name, 32);
        I_strncat(attackerName, 34, "^7");
        CG_DrawScoreboard_GetTeamColor(attackerCI->oldteam, attackerColor);

        if (target == cg->nextSnap->ps.clientNum)
            I_strncpyz(cg->killerName, attackerName, 32);
    }
    else
    {
        attacker = 0x3fe;
        attackerCI = NULL;
        attackerName[0] = '\0';
    }

    if (attacker == target)
    {
        attackerName[0] = '\0';
    }
    else if (attacker == cg->nextSnap->ps.clientNum)
    {
        const char *msg;

        if (attackerCI->oldteam && victimCI->oldteam == attackerCI->oldteam)
            msg = va("CGAME_YOUKILLED\x15^1&&2^7 %s\x14%s", targetName, "CGAME_TEAMMATE");
        else
            msg = va("CGAME_YOUKILLED\x15%s", targetName);

        if (!cg->inKillCam)
            CG_PriorityCenterPrint(msg, 9.6f, 1);
    }
    else if (target == cg->nextSnap->ps.clientNum && attackerCI)
    {
        const char *msg;

        if (attackerCI->oldteam && victimCI->oldteam == attackerCI->oldteam)
            msg = va("CGAME_YOUWEREKILLED\x15^1&&2^7 %s\x14%s", attackerName, "CGAME_TEAMMATE");
        else
            msg = va("CGAME_YOUWEREKILLED\x15%s", attackerName);

        if (!cg->inKillCam)
            CG_PriorityCenterPrint(msg, 9.6f, 1);
    }

    if (!cg->inKillCam)
        CL_DeathMessagePrint(attackerName, attackerColor, targetName, victimColor, iconShader, iconWidth, 1.4f, iconColor, iconHorzFlip);
}

static __forceinline bool CG_IsLocalEntity(const entityState_t *ent)
{
    const snapshot_t *snap = cg->nextSnap;

    if ((snap->ps.pm_flags & 0xc00000) && ent->number == snap->ps.clientNum)
        return true;

    return false;
}

void CG_ItemPickup(int itemNum)
{
    int weapIndex;
    WeaponDef *weapDef;

    weapIndex = bg_itemlist[itemNum].giTag;
    if (bg_itemlist[itemNum].giType != IT_WEAPON)
        return;

    weapDef = BG_GetWeaponDef(weapIndex);
    if (weapDef->weapClass == WEAPCLASS_ITEM)
        return;

    if (weapDef->offhandClass)
    {
        if (!cg->equippedOffHand)
            CG_SetEquippedOffHand(weapIndex);
    }
    else if (!cg->weaponSelect)
    {
        CG_SelectWeaponIndex(weapIndex);
    }
}

// retail 0x4da2e0 (also inlined into CG_EntityEvent); name unknown
void CG_PlayEquipmentSound_4da2e0(int entNum, bool isFirstPerson, bool running)
{
    if (isFirstPerson) {
        if (running)
            CG_PlayEntitySoundAlias(entNum, cgs->media.runningEquipmentSoundPlayer);
        else
            CG_PlayEntitySoundAlias(entNum, cgs->media.walkingEquipmentSoundPlayer);
    } else {
        if (running)
            CG_PlayEntitySoundAlias(entNum, cgs->media.runningEquipmentSound);
        else
            CG_PlayEntitySoundAlias(entNum, cgs->media.walkingEquipmentSound);
    }
}

void CG_PlayFx(centity_t *cent, const float *angles)
{
    const EffectTemplate *fx;
    int fxId;
    vec3_t forward;
    vec3_t up;

    fxId = cent->nextState.eventParm;
    if (fxId > 0 && fxId < 64)
    {
        fx = cgs->fxs[fxId];
        AngleVectors(angles, forward, NULL, up);
        FX_PlayOrientedEffect(fx, cent->lerpOrigin, forward, up);
    }
    else
    {
        Com_Printf("ERROR: CG_PlayFx called with invalid effect id %i\n", fxId);
    }
}

void CG_PlayFxOnTag(centity_t *cent, int eventParm)
{
    const char *tagAndEffect;
    const EffectTemplate *fx;
    FxBoltInfo bolt;
    scr_string_t tagName;

    tagAndEffect = CL_GetConfigString(eventParm + 0x38e);
    fx = cgs->fxs[10 * (tagAndEffect[0] - '0') + tagAndEffect[1] - '0'];
    bolt.dobjHandle = cent->nextState.number;
    tagName = SL_GetString(tagAndEffect + 2, 0);
    bolt.boneIndex = FX_GetBoneIndex(bolt.dobjHandle, tagName);
    Scr_SetString(&tagName, 0);
    if (bolt.boneIndex >= 0)
        FX_PlayEntityEffect(fx, cent->lerpOrigin, NULL, &bolt);
}

void CG_EntityEvent(centity_t *cent, int event)
{
    entityState_t *es;
    vec_t *position;
    int eventParm;
    int clientNum;
    bool isFirstPerson;
    int entNum;
    float dir[3];
    float reflect[3];
    vec3_t axis[3];
    const EffectTemplate *fx;
    char *snap;
    int weapon;
    float height;

    if (event == 0) {

        if (cg_debugEvents->current.enabled != 0) {

            Com_Printf((const char *)"CG_EntityEvent:ZERO EVENT\n");
        }
        return;
    }

    position = (vec_t *)((char *)cent + CENT_LERPORIGIN);

    es = &cent->nextState;

    eventParm = es->eventParm;

    isFirstPerson = CG_IsLocalEntity(es);

    if (cg_debugEvents->current.enabled != 0) {

        Com_Printf((const char *)"ent:%3i  event:%3i ", es->number, event);
    }

    if (cg_debugEvents->current.enabled != 0) {

        char **eventNames = CG_EventNames();
        Com_Printf((const char *)"CG_EntityEvent:%s\n", eventNames[event]);
    }

    clientNum = es->clientNum;

    if (clientNum < 0 || clientNum >= 64) {
        clientNum = 0;
    }

    weapon = es->weapon;
    if (weapon != 0) {
        BG_GetWeaponDef(weapon);
    }

    if (event >= 1 && event < 24) {

        int sndIdx = event - 1;

        if (cg_footsteps->current.enabled != 0) {

            if (isFirstPerson) {

                CG_PlayEntitySoundAlias(es->number,
                                        cgs->media.stepRunSoundPlayer[sndIdx]);
            } else {

                CG_PlayEntitySoundAlias(es->number,
                                        cgs->media.stepRunSound[sndIdx]);
            }
        }

        entNum = es->number;

        CG_PlayEquipmentSound_4da2e0(entNum, isFirstPerson, true);
        return;
    }

    if (event >= 24 && event < 47) {

        int sndIdx = event - 24;

        if (cg_footsteps->current.enabled != 0) {
            if (isFirstPerson) {

                CG_PlayEntitySoundAlias(es->number,
                                        cgs->media.stepWalkSoundPlayer[sndIdx]);
            } else {

                CG_PlayEntitySoundAlias(es->number,
                                        cgs->media.stepWalkSound[sndIdx]);
            }
        }

        entNum = es->number;

        CG_PlayEquipmentSound_4da2e0(entNum, isFirstPerson, false);
        return;
    }

    if (event >= 47 && event < 70) {

        int sndIdx = event - 47;

        if (cg_footsteps->current.enabled != 0) {
            if (isFirstPerson) {

                CG_PlayEntitySoundAlias(es->number,
                                        cgs->media.stepProneSoundPlayer[sndIdx]);
            } else {

                CG_PlayEntitySoundAlias(es->number,
                                        cgs->media.stepProneSound[sndIdx]);
            }
        }

        entNum = es->number;

        CG_PlayEquipmentSound_4da2e0(entNum, isFirstPerson, false);
        return;
    }

    if (event >= 70 && event < 93) {

        int sndIdx = event - 70;

        if (isFirstPerson) {

            CG_PlayEntitySoundAlias(es->number,
                                    cgs->media.stepRunSoundPlayer[sndIdx]);
        } else {

            CG_PlayEntitySoundAlias(es->number,
                                    cgs->media.stepRunSound[sndIdx]);
        }

        entNum = es->number;

        CG_PlayEquipmentSound_4da2e0(entNum, isFirstPerson, true);
        return;
    }

    if (event >= 93 && event < 116) {

        int sndIdx = event - 93;

        if (isFirstPerson) {

            CG_PlayEntitySoundAlias(es->number,
                                    cgs->media.landSoundPlayer[sndIdx]);
        } else {

            CG_PlayEntitySoundAlias(es->number,
                                    cgs->media.landSound[sndIdx]);
        }

        if (clientNum == cg->predictedPlayerState.clientNum) {

            float neg = -(float)eventParm;
            cg->landChange = neg;

            cg->landTime = cg->time;
        }
        return;
    }

    if (event >= 116 && event < 139) {

        int sndIdx = event - 116;

        if (isFirstPerson) {

            CG_PlayEntitySoundAlias(es->number,
                                    cgs->media.landSoundPlayer[sndIdx]);
        } else {

            CG_PlayEntitySoundAlias(es->number,
                                    cgs->media.landSound[sndIdx]);
        }

        CG_PlayEntitySoundAlias(es->number,
                                cgs->media.landDmgSound);

        if (clientNum == cg->predictedPlayerState.clientNum) {
            // fall height in units from the damage fraction
            height = (bg_fallDamageMaxHeight->current.value - bg_fallDamageMinHeight->current.value)
                * ((float)eventParm * 0.01f) + bg_fallDamageMinHeight->current.value;
            if (!(height <= 12.0f)) {
                eventParm = (int)(((height - 12.0f) * (1.0f / 26.0f) + 1.0f) * 4.0f);
                if (eventParm > 24)
                    eventParm = 24;
                else if (eventParm <= 0)
                    return;
                cg->landChange = -(float)eventParm;
                cg->landTime = cg->time;
            }
        }
        return;
    }

    {
        switch (event) {

        case 0x93:
        case 0xa4:
        case 0xa8:
        case 0xa9:
        case 0xaa:
        case 0xab:
        case 0xac:
            break;

        case 0x8b: {
            CG_PlayEntitySoundAlias(es->number,
                                    cgs->media.foliageMovement);
            break;
        }

        case 0x8c: {
            if (clientNum != cg->predictedPlayerState.clientNum) {

                char **eventNames2 = CG_EventNames();
                Com_DPrintf((const char *)"Event %s just for client %i was sent to other clients\n", eventNames2[event], clientNum);
                break;
            }

            if (!legacyHacks->cl_stanceTemp)
                legacyHacks->cl_stance = 0;
            break;
        }

        case 0x8d: {
            if (clientNum != cg->predictedPlayerState.clientNum) {
                char **eventNames2 = CG_EventNames();
                Com_DPrintf((const char *)"Event %s just for client %i was sent to other clients\n", eventNames2[event], clientNum);
                break;
            }

            if (!legacyHacks->cl_stanceTemp)
                legacyHacks->cl_stance = 1;
            break;
        }

        case 0x8e: {
            if (clientNum != cg->predictedPlayerState.clientNum) {
                char **eventNames2 = CG_EventNames();
                Com_DPrintf((const char *)"Event %s just for client %i was sent to other clients\n", eventNames2[event], clientNum);
                break;
            }

            if (!legacyHacks->cl_stanceTemp)
                legacyHacks->cl_stance = 2;
            break;
        }

        case 0x8f: {
            if (clientNum != cg->predictedPlayerState.clientNum) {
                char **eventNames2 = CG_EventNames();
                Com_DPrintf((const char *)"Event %s just for client %i was sent to other clients\n", eventNames2[event], clientNum);
                break;
            }

            if (cg->demoType != 0)
                break;
            if (cg_nopredict->current.enabled != 0)
                break;
            if (cg_synchronousClients->current.enabled != 0)
                break;

            // smooth out stair climbing, q3 EV_STEP
            int deltaTime = cg->time - cg->stepTime;
            float oldStep;
            if (deltaTime < 100) {
                { oldStep = (float)(100 - deltaTime); oldStep = oldStep * (cg->stepChange); } oldStep = oldStep / 100.0f;
                oldStep *= 0.9f;
            } else {
                oldStep = 0.0f;
            }
            { cg->stepChange = (float)(eventParm - 128); cg->stepChange = cg->stepChange + oldStep; }
            if (cg->stepChange > 24.0f)
                cg->stepChange = 24.0f;
            else if (cg->stepChange < -16.0f)
                cg->stepChange = -16.0f;
            cg->stepTime = cg->time;
            break;
        }

        case 0x90:
        case 0x91:
        {
            int itemIndex = es->eventParm;
            if (itemIndex < 1 || itemIndex >= bg_numItems)
                break;

            itemInfo_t *item = &cg_itemsArray[itemIndex];
            if (event == 0x90)
                CG_PlayEntitySoundAlias(es->number, item->pickupSound);
            else if (event == 0x91)
                CG_PlayEntitySoundAlias(es->number, item->ammoPickupSound);

            if (CG_IsLocalEntity(es))
                CG_ItemPickup(itemIndex);
            break;
        }

        case 0x96:
            if (isFirstPerson && cg_weaponsArray[es->weapon].reloadSoundPlayer)
                CG_PlayEntitySoundAlias(es->number, cg_weaponsArray[es->weapon].reloadSoundPlayer);
            else if (isFirstPerson && cg_weaponsArray[es->weapon].reloadEmptySoundPlayer)
                CG_PlayEntitySoundAlias(es->number, cg_weaponsArray[es->weapon].reloadEmptySoundPlayer);
            else if (cg_weaponsArray[es->weapon].reloadSound)
                CG_PlayEntitySoundAlias(es->number, cg_weaponsArray[es->weapon].reloadSound);
            else if (cg_weaponsArray[es->weapon].reloadEmptySound)
                CG_PlayEntitySoundAlias(es->number, cg_weaponsArray[es->weapon].reloadEmptySound);
            break;

        case 0x97:
            if (isFirstPerson && cg_weaponsArray[es->weapon].reloadEmptySoundPlayer)
                CG_PlayEntitySoundAlias(es->number, cg_weaponsArray[es->weapon].reloadEmptySoundPlayer);
            else if (isFirstPerson && cg_weaponsArray[es->weapon].reloadSoundPlayer)
                CG_PlayEntitySoundAlias(es->number, cg_weaponsArray[es->weapon].reloadSoundPlayer);
            else if (cg_weaponsArray[es->weapon].reloadEmptySound)
                CG_PlayEntitySoundAlias(es->number, cg_weaponsArray[es->weapon].reloadEmptySound);
            else if (cg_weaponsArray[es->weapon].reloadSound)
                CG_PlayEntitySoundAlias(es->number, cg_weaponsArray[es->weapon].reloadSound);
            break;

        case 0x98:
            if (isFirstPerson && cg_weaponsArray[es->weapon].reloadStartSoundPlayer)
                CG_PlayEntitySoundAlias(es->number, cg_weaponsArray[es->weapon].reloadStartSoundPlayer);
            else if (cg_weaponsArray[es->weapon].reloadStartSound)
                CG_PlayEntitySoundAlias(es->number, cg_weaponsArray[es->weapon].reloadStartSound);
            break;

        case 0x99:
            if (isFirstPerson && cg_weaponsArray[es->weapon].reloadEndSoundPlayer)
                CG_PlayEntitySoundAlias(es->number, cg_weaponsArray[es->weapon].reloadEndSoundPlayer);
            else if (cg_weaponsArray[es->weapon].reloadEndSound)
                CG_PlayEntitySoundAlias(es->number, cg_weaponsArray[es->weapon].reloadEndSound);
            break;

        case 0x92:
        {
            int w = es->weapon;
            if (!BG_WeaponIsClipOnly(w)) {

                CG_PlayEntitySoundAlias(es->number,
                                        cgs->media.noAmmoSound);
            }

            snap = (char *)cg->nextSnap;
            if (!(*(int *)(snap + SNAP_FLAGS) & 0xc00000))
                break;
            if (es->number != *(int *)(snap + SNAP_PS_CLIENTNUM))
                break;

            CG_OutOfAmmoChange();
            break;
        }

        case 0x94:
        {
            snap = (char *)cg->nextSnap;
            if (!(*(int *)(snap + SNAP_FLAGS) & 0xc00000))
                break;
            if (es->number != *(int *)(snap + SNAP_PS_CLIENTNUM))
                break;

            CG_MenuShowNotify(4);

            CG_SwitchOffHandCmd();
            break;
        }

        case 0x95:
        {
            snap = (char *)cg->nextSnap;
            if (!(*(int *)(snap + SNAP_FLAGS) & 0xc00000))
                break;
            if (es->number != *(int *)(snap + SNAP_PS_CLIENTNUM))
                break;

            CL_SetADS(0);
            break;
        }

        case 0x9a:
            if (cg_weaponsArray[es->weapon].raiseSound)
                CG_PlayEntitySoundAlias(es->number, cg_weaponsArray[es->weapon].raiseSound);
            break;

        case 0x9b:
            if (cg_weaponsArray[es->weapon].putawaySound)
                CG_PlayEntitySoundAlias(es->number, cg_weaponsArray[es->weapon].putawaySound);
            break;

        case 0x9c:
            if (cg_weaponsArray[es->weapon].altSwitchSound)
                CG_PlayEntitySoundAlias(es->number, cg_weaponsArray[es->weapon].altSwitchSound);
            break;

        case 0xb0:
        {

            CG_FireWeapon(cent, event, 0);

            CG_FireWeapon(cent, event, 1);
            break;
        }

        case 0xb1:
        {

            CG_FireWeapon(cent, event, 2);

            CG_FireWeapon(cent, event, 3);
            break;
        }

        case 0xaf:
        {

            CG_StartShakeCamera(0.05f, 100, (const vec_t *)position, 100.0f);

            CG_FireWeapon(cent, event, 0);

            {
                int ep = eventParm;
                int entStride = ep * 16 + ep;
                entStride = (ep + entStride * 8);
                char *ents = (char *)cg_entities;
                char *entData = ents + entStride * 4;

                if (((centity_t *)entData)->nextValid == 0)
                    break;
                if (((centity_t *)entData)->nextState.eType != 1)
                    break;
                snap = (char *)cg->nextSnap;
                int snapWeapon = *(int *)(snap + SNAP_PS_WEAPON) ;
                if (snapWeapon == cent->nextState.number)
                    break;

                CG_CompassAddWeaponPingInfo((centity_t *)entData, position, 50);
            }
            break;
        }

        case 0x9d:
            if (cg_weaponsArray[es->weapon].pullbackSound)
                CG_PlayEntitySoundAlias(es->number, cg_weaponsArray[es->weapon].pullbackSound);
            break;

        case 0x9e:
        case 0x9f:
        case 0xa0:
        {

            CG_FireWeapon(cent, event, 0);
            break;
        }

        case 0xa1:
            if (isFirstPerson && cg_weaponsArray[es->weapon].rechamberSoundPlayer)
            {
                CG_PlayEntitySoundAlias(es->number, cg_weaponsArray[es->weapon].rechamberSoundPlayer);
                break;
            }
            if (cg_weaponsArray[es->weapon].rechamberSound)
                CG_PlayEntitySoundAlias(es->number, cg_weaponsArray[es->weapon].rechamberSound);
            break;

        case 0xa2: {

            CG_EjectWeaponBrass(es, event);
            break;
        }

        case 0xa3:
            if (cg_weaponsArray[es->weapon].meleeSwipeSound)
                CG_PlayEntitySoundAlias(es->number, cg_weaponsArray[es->weapon].meleeSwipeSound);
            break;

        case 0xa5:
        {

            CG_PrepOffHand(es, event, es->eventParm);
            break;
        }

        case 0xa6:
        {

            CG_UseOffHand(cent, event, es->eventParm);
            break;
        }

        case 0xa7:
        {

            if (!isFirstPerson)
                break;
            CG_SetEquippedOffHand(es->eventParm);
            break;
        }

        case 0xad:
        {
            CG_PlayEntitySoundAlias(es->otherEntityNum,
                                    cgs->media.meleeHit);
            break;
        }

        case 0xae:
        {
            CG_PlaySoundAlias(es->otherEntityNum, position,
                              cgs->media.meleeHitOther);
            break;
        }

        case 0xc5:
        {
            dir[0] = 0.0f;
            dir[1] = 0.0f;
            dir[2] = 1.0f;

            CG_PlaySoundAlias(ENTITYNUM_WORLD, position, cgs->media.grenadeExplodeSound[0]);

            fx = cgs->media.fx->table[IMPACT_GRENADE_EXPLODE * SURF_TYPECOUNT];
            if (fx)
            {
                VectorCopy(dir, axis[0]);
                MakeNormalVectors(axis[0], axis[1], axis[2]);
                FX_PlayEntityEffect(fx, position, axis, NULL);
            }
            break;
        }

        case 0xb5:
        case 0xb6:
        case 0xb7:
            ByteToDir(es->eventParm, dir);
            ByteToDir(es->eventParm2, reflect);
            CG_BulletHitEvent(es->otherEntityNum, position, dir, reflect, es->surfType, event);
            break;

        case 0xb9:
        case 0xba:
            CG_BulletHitClientEvent(es->otherEntityNum, position, es->surfType, event);
            break;

        case 0xbb:
        {
            ByteToDir(es->eventParm, dir);

            CG_PlaySoundAlias(ENTITYNUM_WORLD, position, cgs->media.grenadeBounceSound[es->surfType]);

            fx = cgs->media.fx->table[IMPACT_GRENADE_BOUNCE * SURF_TYPECOUNT + es->surfType];
            if (fx)
            {
                VectorCopy(dir, axis[0]);
                MakeNormalVectors(axis[0], axis[1], axis[2]);
                FX_PlayEntityEffect(fx, position, axis, NULL);
            }
            break;
        }

        case 0xbc:
        {
            ByteToDir(es->eventParm, dir);

            CG_PlaySoundAlias(ENTITYNUM_WORLD, position, cgs->media.grenadeExplodeSound[es->surfType]);

            const EffectTemplate *impactFx = cgs->media.fx->table[IMPACT_GRENADE_EXPLODE * SURF_TYPECOUNT + es->surfType];
            if (impactFx)
            {
                VectorCopy(dir, axis[0]);
                MakeNormalVectors(axis[0], axis[1], axis[2]);
                FX_PlayEntityEffect(impactFx, position, axis, NULL);
            }

            fx = cg_weaponsArray[es->weapon].projExplosionEffect;
            if (fx)
            {
                VectorCopy(dir, axis[0]);
                MakeNormalVectors(axis[0], axis[1], axis[2]);
                FX_PlayEntityEffect(fx, position, axis, NULL);
            }

            if (cg_weaponsArray[es->weapon].projExplosionSound)
                CG_PlaySoundAlias(ENTITYNUM_WORLD, position, cg_weaponsArray[es->weapon].projExplosionSound);
            break;
        }

        case 0xbf:
        {
            ByteToDir(es->eventParm, dir);

            if (cg_weaponsArray[es->weapon].projExplosionEffect)
            {
                FX_WarpTime(es->time);
                VectorCopy(dir, axis[0]);
                MakeNormalVectors(axis[0], axis[1], axis[2]);
                FX_PlayEntityEffect(cg_weaponsArray[es->weapon].projExplosionEffect, position, axis, NULL);
                FX_WarpTime(cg->time);
            }

            if (!cg_weaponsArray[es->weapon].projExplosionSound)
                break;

            if ((es->eFlags & 0x10000) && cg->time - es->time >= 50)
                break;

            CG_PlaySoundAlias(ENTITYNUM_WORLD, position, cg_weaponsArray[es->weapon].projExplosionSound);
            break;
        }

        case 0xbe:
            cg->nomarks = 1;
            // fall through

        case 0xbd:
        {
            ByteToDir(es->eventParm, dir);

            CG_PlaySoundAlias(ENTITYNUM_WORLD, position, cgs->media.rocketExplodeSound[es->surfType]);

            const EffectTemplate *impactFx = cgs->media.fx->table[IMPACT_ROCKET_EXPLODE * SURF_TYPECOUNT + es->surfType];
            if (impactFx)
            {
                VectorCopy(dir, axis[0]);
                MakeNormalVectors(axis[0], axis[1], axis[2]);
                FX_PlayEntityEffect(impactFx, position, axis, NULL);
            }

            fx = cg_weaponsArray[es->weapon].projExplosionEffect;
            if (fx)
            {
                VectorCopy(dir, axis[0]);
                MakeNormalVectors(axis[0], axis[1], axis[2]);
                FX_PlayEntityEffect(fx, position, axis, NULL);
            }

            if (cg_weaponsArray[es->weapon].projExplosionSound)
                CG_PlaySoundAlias(ENTITYNUM_WORLD, position, cg_weaponsArray[es->weapon].projExplosionSound);

            cg->nomarks = 0;
            break;
        }

        case 0xb3:
        {
            int csIndex = es->eventParm + 0x24e;
            const char *csStr = CL_GetConfigString(csIndex);
            CG_PlaySoundAliasByName(es->number, es->pos.trBase, csStr);
            break;
        }

        case 0xb4:
        {
            int csIndex = es->eventParm + 0x24e;
            const char *csStr = CL_GetConfigString(csIndex);
            CG_PlaySoundAliasAsMasterByName(es->number, es->pos.trBase, csStr);
            break;
        }

        case 0xc6:
            CG_Obituary(es);
            break;

        case 0xc2:
            CG_PlayFx(cent, es->apos.trBase);
            break;

        case 0xc3:
            CG_PlayFxOnTag(cent, es->eventParm);
            break;

        case 0xc4:
        {
            CG_StartShakeCamera(es->angles2[0], es->time, position, es->angles2[1]);
            break;
        }

        default:
            Com_Error(ERR_DROP, (const char *)"\x15Unknown event: '%s'", CG_EventNames()[event]);
            break;

        }
    }
}

void CG_CheckEvents(centity_t *cent)
{
    int eventSequence;
    int i;
    byte oldEventParm;

    if (cent->nextState.eType > 10)
    {
        if (!cent->previousEventSequence)
        {
            cent->previousEventSequence = 1;
            CG_CalcEntityLerpPositions(cent);
            CG_EntityEvent(cent, cent->nextState.eType - 10);
        }
        return;
    }

    eventSequence = cent->nextState.eventSequence;
    if (!eventSequence)
    {
        cent->previousEventSequence = 0;
        return;
    }

    if (eventSequence < cent->previousEventSequence)
        cent->previousEventSequence -= 256;

    if (eventSequence - cent->previousEventSequence > 4)
        cent->previousEventSequence = eventSequence - 4;

    if (cent->previousEventSequence < eventSequence)
    {
        CG_CalcEntityLerpPositions(cent);

        oldEventParm = cent->nextState.eventParm;
        for (i = cent->previousEventSequence; i != cent->nextState.eventSequence; i++)
        {
            int ev = cent->nextState.events[i & 3];

            cent->nextState.eventParm = cent->nextState.eventParms[i & 3];
            CG_EntityEvent(cent, ev);
        }
        cent->nextState.eventParm = oldEventParm;
        eventSequence = cent->nextState.eventSequence;
    }

    cent->previousEventSequence = eventSequence;
}

