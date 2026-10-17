#include "common_types.h"
#include "imports.h"
#include "headers/PC/cgame_mp/cg_local.h"

#include <math.h>

extern const dvar_t *cg_hudCompassSize;
extern const dvar_t *cg_hudCompassMinRange;
extern const dvar_t *cg_hudCompassMinRadius;
extern const dvar_t *cg_hudCompassMaxRange;
extern const dvar_t *cg_hudObjectiveMaxRange;
extern const dvar_t *cg_hudObjectiveMinAlpha;
extern const dvar_t *cg_hudCompassSoundPingFadeTime;
extern const dvar_t *hud_fade_compass;

extern float CG_FadeHudMenu(const dvar_t *fadeDvar, int displayStartTime, int duration);
extern void CG_UpdateCompPointerOrientation(void);
extern void UI_DrawHandlePic(float x, float y, float w, float h, int horzAlign, int vertAlign, const vec_t *color, MaterialHandle hMaterial);
extern void CG_DrawRotatedPic(float x, float y, float width, float height, int horzAlign, int vertAlign, float angle, const vec_t *color, MaterialHandle material);

extern float vectoyaw( const vec3_t vec );
static __forceinline float AngleNormalize360( float angle )
{
    return ( 360.0f / 65536 ) * ( (int)( angle * ( 65536 / 360.0f ) ) & 65535 );
}
extern vec_t Vec2Normalize( vec3_t v );

#define COMPASS_ACTOR_COUNT 64

static inline float I_sqrt(const float value)
{
    return sqrt(value);
}

// universal/com_math.h inlines
static inline float I_fabs(const float value)
{
    return fabs(value);
}

static inline vec_t Vec2Length(const vec2_t v)
{
    return I_sqrt(v[0] * v[0] + v[1] * v[1]);
}

static inline void VectorCopy(const vec3_t a, vec3_t b)
{
    b[0] = a[0];
    b[1] = a[1];
    b[2] = a[2];
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


void CG_ApplyCompassPointerRadiusScale(float *radiusScale);
void CG_CompassAddWeaponPingInfo(centity_t *cent, const vec_t *origin, int msec);
void CG_DrawCompassFriendlies(rectDef_t *rect, MaterialHandle material, vec_t *color);

// retail 0x4d4a60
void CG_ApplyCompassPointerRadiusScale(float *radiusScale)
{
    *radiusScale = cg_hudCompassSize->current.value * *radiusScale * 43.75;
}

void CG_CompassAddWeaponPingInfo(centity_t *cent, const vec_t *origin, int msec)
{
    int eType = cent->nextState.eType;

    if (eType == 2)
        return;

    int localClientNum = cg->clientNum;

    clientInfo_t *localClientInfo = &cg->bgs.clientinfo[localClientNum];

    int entClientNum = cent->nextState.number;

    clientInfo_t *entClientInfo = &cg->bgs.clientinfo[entClientNum];

    if (localClientInfo == entClientInfo)
        return;

    int entTeam = entClientInfo->team;
    if (entTeam == 3)
        return;

    cg->compassActors[entClientNum].beginFadeTime = cg->time + msec;

    int localTeam = localClientInfo->team;
    int enemyFlag;
    if (localTeam == 0 || entTeam != localTeam)
        enemyFlag = 1;
    else
        enemyFlag = 0;
    cg->compassActors[entClientNum].enemy = enemyFlag;

    if (cg->compassActors[entClientNum].enemy == 0)
        return;

    cg->compassActors[entClientNum].vLastPos[0] = origin[0];
    cg->compassActors[entClientNum].vLastPos[1] = origin[1];
}

// retail 0x4d4a80, name unknown
void CompassDistToRadius(float dist, float *radius)
{
    *radius = (dist - cg_hudCompassMinRange->current.value) / (cg_hudCompassMaxRange->current.value - cg_hudCompassMinRange->current.value);
    *radius *= 1.0f - cg_hudCompassMinRadius->current.value;
    *radius += cg_hudCompassMinRadius->current.value;
    CG_ApplyCompassPointerRadiusScale(radius);
}

// retail 0x4d4ad0, name unknown
void CompassIconSize(float *w, float *h)
{
    *w = cg_hudCompassSize->current.value * 10.0f;
    *h = cg_hudCompassSize->current.value * 10.0f;
}

// float consistency from here on (retail)
#pragma optimize("p", on)

// retail 0x4d4af0
void CG_DrawCompassFriendlies(rectDef_t *rect, MaterialHandle material, vec_t *color)
{
    float compassFadeOutAlpha;
    float centerX;
    float centerY;
    int team;
    int i;
    int num;
    int eFlags;
    centity_t *cent;
    vec2_t delta;
    float fadeTime;
    vec4_t fadedColor;

    const dvar_t *fadeDvar = hud_fade_compass;
    int duration = I_fround(fadeDvar->current.value * 1000.0f);
    compassFadeOutAlpha = CG_FadeHudMenu(fadeDvar, cg->compassFadeTime, duration);

    if (compassFadeOutAlpha == 0.0f)
        return;

    if (!cg->bgs.clientinfo[cg->nextSnap->ps.clientNum].infoValid)
        return;

    team = cg->bgs.clientinfo[cg->nextSnap->ps.clientNum].team;
    if (team == 3)
        return;

    CG_UpdateCompPointerOrientation();

    { centerX = rect->x; centerX = centerX + rect->w * cg_hudCompassSize->current.value * 0.5f; }
    centerY = rect->y + rect->h - cg_hudCompassSize->current.value * rect->h * 0.5f;

    fadedColor[0] = color[0];
    fadedColor[1] = color[1];
    fadedColor[2] = color[2];
    fadedColor[3] = color[3];

    if (team != 0)
    {
        for (i = 0; i < cg->nextSnap->numEntities; i++)
        {
            num = cg->nextSnap->entities[i].number;
            cent = &cg_entities[num];
            if (cent->nextState.eType != 1)
                continue;
            eFlags = cent->nextState.eFlags;
            if (eFlags & 0x20000)
                continue;
            if (num < 0)
                continue;
            if (!cg->bgs.clientinfo[num].infoValid)
                continue;
            if (cg->bgs.clientinfo[num].team != team)
                continue;

            cg->compassActors[num].iLastUpdate = cg->time;
            cg->compassActors[num].vLastPos[0] = cent->lerpOrigin[0];
            cg->compassActors[num].vLastPos[1] = cent->lerpOrigin[1];
            cg->compassActors[num].fLastYaw = cent->lerpAngles[1];
            if ((eFlags & 0x400000) && cg->compassActors[num].pingTime <= cg->time)
                cg->compassActors[num].pingTime = cg->time + 3000;
        }

        if (cg->nextSnap->ps.iCompassFriendInfo)
        {
            num = cg->nextSnap->ps.iCompassFriendInfo & 0x3f;
            cg->compassActors[num].iLastUpdate = cg->time;
            delta[0] = (float)(((cg->nextSnap->ps.iCompassFriendInfo >> 6) & 0x1ff) * 4 - 1020);
            delta[1] = (float)(((cg->nextSnap->ps.iCompassFriendInfo >> 15) & 0x1ff) * 4 - 1020);
            if (delta[0] != 1024.0f && delta[0] != -1020.0f && delta[1] != 1024.0f && delta[1] != -1020.0f)
            {
                cg->compassActors[num].vLastPos[0] = cg->refdef.vieworg[0]; cg->compassActors[num].vLastPos[0] = cg->compassActors[num].vLastPos[0] + delta[0];
                cg->compassActors[num].vLastPos[1] = cg->refdef.vieworg[1] + delta[1];
            }
            else
            {
                Vec2Normalize(delta);
                cg->compassActors[num].vLastPos[0] = delta[0];
                cg->compassActors[num].vLastPos[1] = delta[1];
            }
            cg->compassActors[num].fLastYaw = (float)(char)(cg->nextSnap->ps.iCompassFriendInfo >> 24) * 1.40625f;
            if ((cg->nextSnap->ps.eFlags & 0x800000) && cg->compassActors[num].pingTime <= cg->time)
                cg->compassActors[num].pingTime = cg->time + 3000;
        }

        if (compassFadeOutAlpha < color[3])
            color[3] = compassFadeOutAlpha;
        if (compassFadeOutAlpha < fadedColor[3])
            fadedColor[3] = compassFadeOutAlpha;

        for (i = 0; i < COMPASS_ACTOR_COUNT; i++)
        {
            if (cg->compassActors[i].iLastUpdate > cg->time)
                cg->compassActors[i].iLastUpdate = 0;
            if (cg->compassActors[i].iLastUpdate < cg->time - 800)
                continue;
            if (cg->nextSnap->ps.clientNum == i)
                continue;
            float yaw, dist, radius, s, c, x, y, w, h, angle;
            int flash;
            MaterialHandle pingMat;
            vec4_t actorColor;

            if (I_fabs(cg->compassActors[i].vLastPos[0]) <= 1.0f && I_fabs(cg->compassActors[i].vLastPos[1]) <= 1.0f)
            {
                yaw = AngleNormalize360(vectoyaw(cg->compassActors[i].vLastPos) - cg->compPointerYaw);
                dist = cg_hudCompassMaxRange->current.value;
                fadedColor[3] = (1.0f - cg_hudObjectiveMinAlpha->current.value) * 0.5f + cg_hudObjectiveMinAlpha->current.value;
            }
            else
            {
                delta[0] = cg->compassActors[i].vLastPos[0] - cg->refdef.vieworg[0];
                delta[1] = cg->compassActors[i].vLastPos[1] - cg->refdef.vieworg[1];
                yaw = AngleNormalize360(vectoyaw(delta) - cg->compPointerYaw);
                dist = Vec2Length(delta);

                fadedColor[3] = dist;
                if (dist > cg_hudObjectiveMaxRange->current.value)
                    fadedColor[3] = cg_hudObjectiveMaxRange->current.value;
                else if (dist < cg_hudCompassMaxRange->current.value)
                    fadedColor[3] = cg_hudCompassMaxRange->current.value;
                fadedColor[3] = (fadedColor[3] - cg_hudCompassMaxRange->current.value); fadedColor[3] = fadedColor[3] / ((cg_hudObjectiveMaxRange->current.value - cg_hudCompassMaxRange->current.value));
                fadedColor[3] = (cg_hudObjectiveMinAlpha->current.value - 1.0f) * fadedColor[3] + 1.0f;

                if (dist > cg_hudCompassMaxRange->current.value)
                    dist = cg_hudCompassMaxRange->current.value;
                else if (dist < cg_hudCompassMinRange->current.value)
                    dist = cg_hudCompassMinRange->current.value;
            }

            CompassDistToRadius(dist, &radius);
            FastSinCos(yaw * 0.017453292f, &s, &c);
            CompassIconSize(&w, &h);
            x = centerX - w * 0.5f; x = x - s * radius;
            y = centerY - h * 0.5f - c * radius;
            angle = AngleNormalize360(cg->refdefViewAngles[1] - cg->compassActors[i].fLastYaw);

            if (cg->compassActors[i].pingTime > cg->time)
                flash = (cg->compassActors[i].pingTime - cg->time) % 500 >= 250;
            else
                flash = 0;

            ;
            pingMat = NULL;
            if ((float)cg->compassActors[i].beginFadeTime + (cg_hudCompassSoundPingFadeTime->current.value * 1000.0f) >= (float)cg->time)
            {
                if (cg->compassActors[i].beginFadeTime >= cg->time)
                    fadedColor[3] = 1.0f;
                else
                    fadedColor[3] = 1.0f - (float)(cg->time - cg->compassActors[i].beginFadeTime) / (cg_hudCompassSoundPingFadeTime->current.value * 1000.0f);
                pingMat = cgs->media.compassping_friendlyfiring;
            }

            actorColor[0] = color[0];
            actorColor[1] = color[1];
            actorColor[2] = color[2];
            actorColor[3] = color[3];
            if (compassFadeOutAlpha < actorColor[3])
                actorColor[3] = compassFadeOutAlpha;
            if (compassFadeOutAlpha < fadedColor[3])
                fadedColor[3] = compassFadeOutAlpha;

            if (flash == 1)
            {
                UI_DrawHandlePic(x, y, w, h, rect->horzAlign, rect->vertAlign, fadedColor, cgs->media.friendMaterials[1]);
            }
            else
            {
                if (!pingMat || fadedColor[3] != 1.0f)
                    CG_DrawRotatedPic(x, y, w, h, rect->horzAlign, rect->vertAlign, angle, actorColor, cgs->media.friendMaterials[flash]);
                if (pingMat)
                    CG_DrawRotatedPic(x, y, w, h, rect->horzAlign, rect->vertAlign, angle, fadedColor, pingMat);
            }
        }
    }

    for (i = 0; i < COMPASS_ACTOR_COUNT; i++)
    {
        if (!cg->compassActors[i].enemy)
            continue;
        if (cg_hudCompassSoundPingFadeTime->current.value * 1000.0f + (float)cg->compassActors[i].beginFadeTime < (float)cg->time)
            continue;
        if (!cg->compassActors[i].beginFadeTime)
            continue;

        float yaw, dist, radius, s, c, x, y, w, h;
        delta[0] = cg->compassActors[i].vLastPos[0] - cg->refdef.vieworg[0];
        delta[1] = cg->compassActors[i].vLastPos[1] - cg->refdef.vieworg[1];
        yaw = AngleNormalize360(vectoyaw(delta) - cg->compPointerYaw);
        dist = Vec2Length(delta);
        if (dist > cg_hudCompassMaxRange->current.value)
            dist = cg_hudCompassMaxRange->current.value;

        CompassDistToRadius(dist, &radius);
        FastSinCos(yaw * 0.017453292f, &s, &c);
        CompassIconSize(&w, &h);
        x = centerX - w * 0.5f - s * radius;
        y = centerY - h * 0.5f - c * radius;

        if (cg->compassActors[i].beginFadeTime >= cg->time)
            fadedColor[3] = 1.0f;
        else
        {
            fadedColor[3] = 1.0f;
            fadedColor[3] = fadedColor[3] - (float)(cg->time - cg->compassActors[i].beginFadeTime) / (cg_hudCompassSoundPingFadeTime->current.value * 1000.0f);
        }
        if (compassFadeOutAlpha < fadedColor[3])
            fadedColor[3] = compassFadeOutAlpha;

        UI_DrawHandlePic(x, y, w, h, rect->horzAlign, rect->vertAlign, fadedColor, cgs->media.compassping_enemyfiring);
    }
}

extern qboolean CG_ServerMaterialName(int index, char *materialName, int maxLen);
extern MaterialHandle CL_RegisterMaterialNoMip(const char *name, int imageTrack);
extern void CompassIconSize_4bf300(float *w, float *h);

void CG_DrawPlayerCompassPointers(const rectDef_t *rect, vec_t *color)
{
    float fadeAlpha;
    float centerX;
    float centerY;
    int i;
    objective_t *obj;
    vec3_t origin;
    vec3_t delta;
    float yaw;
    float dist;
    float radius;
    float s;
    float c;
    float x;
    float y;
    float w;
    float h;
    vec4_t drawColor;
    MaterialHandle material;
    char materialName[64];

    fadeAlpha = CG_FadeHudMenu(hud_fade_compass, cg->compassFadeTime, I_fround(hud_fade_compass->current.value * 1000.0f));
    if (fadeAlpha == 0.0f)
        return;

    CG_UpdateCompPointerOrientation();

    centerX = rect->w * cg_hudCompassSize->current.value * 0.5f + rect->x;
    centerY = (cg_hudCompassSize->current.value * 0.5f) * rect->h - (cg_hudCompassSize->current.value - 1.0f) * rect->h + rect->y;

    if (!cg->bgs.clientinfo[cg->nextSnap->ps.clientNum].infoValid)
        return;

    for (i = 0; i < 16; i++)
    {
        obj = &cg->nextSnap->ps.objective[i];
        if (obj->state != 4)
            continue;

        if (obj->entNum != 1023)
            VectorCopy(cg_entities[obj->entNum].lerpOrigin, origin);
        else
            VectorCopy(obj->origin, origin);
        delta[0] = origin[0] - cg->refdef.vieworg[0];
        delta[1] = origin[1] - cg->refdef.vieworg[1];
        delta[2] = origin[2] - cg->refdef.vieworg[2];
        yaw = AngleNormalize360(vectoyaw(delta) - cg->compPointerYaw);
        dist = Vec2Length(delta);

        drawColor[0] = color[0];
        drawColor[1] = color[1];
        drawColor[2] = color[2];
        drawColor[3] = color[3];

        if (dist <= cg_hudCompassMaxRange->current.value)
        {
            drawColor[3] = 1.0f;
        }
        else if (dist >= cg_hudObjectiveMaxRange->current.value)
        {
            drawColor[3] = cg_hudObjectiveMinAlpha->current.value;
        }
        else
        {
            float lerp = 0.0f;
            if (cg_hudObjectiveMaxRange->current.value - cg_hudCompassMaxRange->current.value != 0.0f)
                lerp = (dist - cg_hudCompassMaxRange->current.value) / (cg_hudObjectiveMaxRange->current.value - cg_hudCompassMaxRange->current.value);
            drawColor[3] = 1.0f - (1.0f - cg_hudObjectiveMinAlpha->current.value) * lerp;
        }

        if (dist <= cg_hudCompassMinRange->current.value)
        {
            radius = cg_hudCompassMinRadius->current.value;
        }
        else if (dist >= cg_hudCompassMaxRange->current.value)
        {
            radius = 1.0f;
        }
        else
        {
            float frac = 0.0f;
            if (cg_hudCompassMaxRange->current.value - cg_hudCompassMinRange->current.value != 0.0f)
                frac = (cg_hudCompassMaxRange->current.value - dist) / (cg_hudCompassMaxRange->current.value - cg_hudCompassMinRange->current.value);
            radius = 1.0f - (1.0f - cg_hudCompassMinRadius->current.value) * frac;
        }
        CG_ApplyCompassPointerRadiusScale(&radius);

        FastSinCos((yaw * 0.017453292f), &s, &c);
        CompassIconSize_4bf300(&w, &h);
        x = centerX - w * 0.5f - s * radius;
        y = centerY - w * 0.5f - c * radius;

        if (obj->icon && CG_ServerMaterialName(obj->icon, materialName, 64))
            material = CL_RegisterMaterialNoMip(materialName, 7);
        else
            material = cgs->media.objectiveMaterials[0];

        if (fadeAlpha < drawColor[3])
            drawColor[3] = fadeAlpha;

        UI_DrawHandlePic(x, y, w, h, rect->horzAlign, rect->vertAlign, drawColor, material);
    }
}
