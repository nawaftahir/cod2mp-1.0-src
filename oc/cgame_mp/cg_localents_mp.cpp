#include "common_types.h"
#include "imports.h"
#include "headers/PC/cgame_mp/cg_local.h"
localEntity_t cg_eachClientLocalEntities[1][128];
localEntity_t cg_eachClientActiveLocalEntities[1];
localEntity_t *cg_localEntities = cg_eachClientLocalEntities[0];
localEntity_t *cg_activeLocalEntities = cg_eachClientActiveLocalEntities;
localEntity_t *cg_freeLocalEntities = NULL;
/* dvar globals */
extern const dvar_t *cg_tracerLength;

extern void BG_EvaluateTrajectory( const trajectory_t *tr, int atTime, vec3_t result );
extern vec_t Vec3NormalizeTo( const vec3_t v, vec3_t out );
extern void CG_DrawTracer(vec_t *start, vec_t *finish);
extern void Com_Error(errorParm_t code, const char *fmt, ...);

extern localEntity_t cg_eachClientActiveLocalEntities[1];
extern localEntity_t *cg_eachClientFreeLocalEntities[1];
extern localEntity_t *cg_localEntities;
extern localEntity_t *cg_activeLocalEntities;
extern localEntity_t *cg_freeLocalEntities;

void CG_InitLocalEntities(void);
void CG_AddMovingTracer(localEntity_t *le);
static void __static_initialization_and_destruction_0(int initialize, int priority);
static void GLOBAL__I_cg_eachClientActiveLocalEntities(void);
localEntity_t *CG_AllocLocalEntity(void);
void CG_AddLocalEntities(void);

static void CG_FreeLocalEntity(localEntity_t *le)
{
    if (!le->prev) {
        Com_Error(ERR_DROP, "CG_FreeLocalEntity: not active");
    }

    le->prev->next = le->next;
    le->next->prev = le->prev;

    le->next = cg_freeLocalEntities;
    cg_freeLocalEntities = le;
}

void CG_InitLocalEntities(void)
{
    int i;

    memset(cg_localEntities, 0, 128 * sizeof(localEntity_t));

    cg_activeLocalEntities->next = cg_activeLocalEntities;
    cg_activeLocalEntities->prev = cg_activeLocalEntities;

    cg_freeLocalEntities = cg_localEntities;
    for (i = 0; i < 127; i++) {
        cg_localEntities[i].next = &cg_localEntities[i + 1];
    }
}

localEntity_t *CG_AllocLocalEntity(void)
{
    localEntity_t *le;

    if (!cg_freeLocalEntities) {

        CG_FreeLocalEntity(cg_activeLocalEntities->prev);
    }

    le = cg_freeLocalEntities;
    cg_freeLocalEntities = cg_freeLocalEntities->next;

    memset(le, 0, sizeof(localEntity_t));

    le->next = cg_activeLocalEntities->next;
    le->prev = cg_activeLocalEntities;
    cg_activeLocalEntities->next->prev = le;
    cg_activeLocalEntities->next = le;

    return le;
}

void CG_AddMovingTracer(localEntity_t *le)
{
    vec3_t start;
    vec3_t end;
    vec3_t dir;
    vec3_t delta;
    float dist;

    BG_EvaluateTrajectory(&le->pos, cg->time, start);

    Vec3NormalizeTo(le->pos.trDelta, dir);

    delta[0] = start[0] - le->pos.trBase[0];
    delta[1] = start[1] - le->pos.trBase[1];
    delta[2] = start[2] - le->pos.trBase[2];
    dist = le->tracerClipDist - (delta[0] * dir[0] + delta[1] * dir[1] + delta[2] * dir[2]);
    if (!(dist < cg_tracerLength->current.value)) {
        dist = cg_tracerLength->current.value;
    }

    end[0] = start[0] + dist * dir[0];
    end[1] = start[1] + dist * dir[1];
    end[2] = start[2] + dist * dir[2];

    CG_DrawTracer(start, end);
}

static void __static_initialization_and_destruction_0(int initialize, int priority)
{
    if (priority == 0xffff && initialize == 1)
        cg_freeLocalEntities = cg_eachClientFreeLocalEntities[0];
}

static void GLOBAL__I_cg_eachClientActiveLocalEntities(void)
{
    __static_initialization_and_destruction_0(1, 0xffff);
}

void CG_AddLocalEntities(void)
{
    localEntity_t *le;
    localEntity_t *next;
    int time;

    le = cg_activeLocalEntities->prev;

    while (le != cg_activeLocalEntities) {
        next = le->prev;

        time = cg->time;

        if (time >= le->endTime || time < le->pos.trTime) {

            CG_FreeLocalEntity(le);
        } else {
            CG_AddMovingTracer(le);
        }

        le = next;
    }
}

