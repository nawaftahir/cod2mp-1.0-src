#include "common_types.h"
#include "imports.h"
#include "bytematch.h"
#include "headers/PC/cgame_mp/cg_local.h"

GfxWorldVertex markVerts[1024];

MarkPoly *cg_freeMarkPolys;
MarkPoly cg_markPolys[1024];
extern FxHelper *theFxHelper;

extern const dvar_t *cg_marks;
extern const dvar_t *cg_marksLimit;

extern float floorf(float x);

extern MarkPoly cg_markPolys[1024];
extern MarkPoly *cg_freeMarkPolys;
extern GfxWorldVertex markVerts[1024];

extern vec_t Vec3NormalizeTo( const vec3_t v, vec3_t out );
extern void PerpendicularVector( const vec3_t src, vec3_t dst );
extern void RotatePointAroundVector( vec3_t dst, const vec3_t dir, const vec3_t point, float degrees );
extern void Vec3Cross(const vec3_t v0, const vec3_t v1, vec3_t cross);
extern void CL_AddPolyToScene(MaterialHandle mtlHandle, unsigned short lmapIndex,
                              unsigned short vertCount, const GfxWorldVertex *verts);
extern int CL_MarkFragments(const vec3_t *points, const vec_t *origin, const vec3_t *axis, float radius, int maxPoints, GfxWorldVertex *verts, int maxFragments, GfxMarkFragment *fragmentBuffer, MaterialHandle markMaterial);
extern void Com_Error(errorParm_t code, const char *fmt, ...);

#define CG_MARKS_DVAR cg_marks
#define FX_HELPER_PTR (((FxHelper *)theFxHelper))

void CG_InitMarkPolys(void);
void CG_AddMarks(void);
void CG_ImpactMark(MaterialHandle markMaterial, const vec_t *origin, const vec_t *dir, float orientation, const vec_t *color, float radius);

void CG_InitMarkPolys(void);
void CG_AddMarks(void);
void CG_ImpactMark(MaterialHandle markMaterial, const vec_t *origin, const vec_t *dir, float orientation, const vec_t *color, float radius);

// universal/com_math.h inlines
inline int I_sel(const int x, const int y, const int z)
{
    if (x >= 0)
        return y;
    return z;
}

inline int I_min(const int x, const int y)
{
    return I_sel(y - x, x, y);
}

inline int I_max(const int x, const int y)
{
    return I_sel(x - y, x, y);
}

// Name from CoD4 com_pack; body at retail 0x4c7df0.
static void Byte4PackVertexColor(const float *from, unsigned char *to)
{
    to[2] = (unsigned char)I_max(0, I_min(255, I_fround(from[0] * 255.0f)));
    to[1] = (unsigned char)I_max(0, I_min(255, I_fround(from[1] * 255.0f)));
    to[0] = (unsigned char)I_max(0, I_min(255, I_fround(from[2] * 255.0f)));
    to[3] = (unsigned char)I_max(0, I_min(255, I_fround(from[3] * 255.0f)));
}

void CG_InitMarkPolys(void)
{
    int lasttrav;
    int i;
    MarkPoly *sentinel;

    lasttrav = cg_marksLimit->current.integer;

    memset(cg_markPolys, 0, (size_t)lasttrav * sizeof(MarkPoly));

    sentinel = &cgArray[0].activeMarkPolys;
    sentinel->nextMark = sentinel;
    sentinel->prevMark = sentinel;

    cg_freeMarkPolys = &cg_markPolys[0];

    {
        MarkPoly *nxt = &cg_markPolys[1];
        MarkPoly *cur = &cg_markPolys[0];
        for (i = 0; i < lasttrav - 1; i++) {
            cur->nextMark = nxt;
            cur = nxt;
            nxt = nxt + 1;
        }
    }
}

void CG_AddMarks(void)
{
    MarkPoly *markPoly;

    if (!CG_MARKS_DVAR->current.enabled)
        return;

    markPoly = cg->activeMarkPolys.nextMark;

    if (markPoly == &cg->activeMarkPolys)
        return;

    do {

        if (!theFxHelper->CullSphere(markPoly->origin, markPoly->radius,
                                     theFxHelper->mCamera.numPlanes)) {

            markPoly->lastFrameDrawn = cg->clientFrame;

            CL_AddPolyToScene(markPoly->mtlHandle, markPoly->lmapIndex,
                              markPoly->vertCount, markPoly->verts);
        }

        markPoly = markPoly->nextMark;
    } while (markPoly != &cg->activeMarkPolys);
}

// retail 0x4c7f70; Q3 cg_marks.c, keeps Q3's error string
void CG_FreeMarkPoly(MarkPoly *le)
{
    if (!le->prevMark)
        Com_Error(ERR_DROP, "CG_FreeLocalEntity: not active");
    le->prevMark->nextMark = le->nextMark;
    le->nextMark->prevMark = le->prevMark;
    le->nextMark = cg_freeMarkPolys;
    cg_freeMarkPolys = le;
}

// retail 0x4c7fb0, unreferenced (CG_AllocMark has its own copy); name unknown
MarkPoly *CG_LeastRecentlyDrawnMark_4c7fb0(void)
{
    MarkPoly *mark;
    MarkPoly *le;

    le = cg->activeMarkPolys.prevMark;
    for (mark = le; mark != &cg->activeMarkPolys; mark = mark->prevMark) {
        if (mark->lastFrameDrawn < le->lastFrameDrawn)
            le = mark;
    }
    return le;
}

// retail 0x4c7fe0; Q3 shape, CG_FreeMarkPoly inlined (SUPPORTED)
static MarkPoly *CG_AllocMark(void)
{
    MarkPoly *le;
    MarkPoly *mark;

    if (!cg_freeMarkPolys) {
        // free the least recently drawn mark
        mark = cg->activeMarkPolys.prevMark;
        le = mark;
        while (mark != &cg->activeMarkPolys) {
            if (mark->lastFrameDrawn < le->lastFrameDrawn)
                le = mark;
            mark = mark->prevMark;
        }
        CG_FreeMarkPoly(le);
    }

    le = cg_freeMarkPolys;
    cg_freeMarkPolys = cg_freeMarkPolys->nextMark;

    le->nextMark = cg->activeMarkPolys.nextMark;
    le->prevMark = &cg->activeMarkPolys;
    cg->activeMarkPolys.nextMark->prevMark = le;
    cg->activeMarkPolys.nextMark = le;
    return le;
}

void CG_ImpactMark(MaterialHandle markMaterial, const vec_t *origin, const vec_t *dir, float orientation, const vec_t *color, float radius)
{
    vec3_t axis[3];
    vec3_t originalPoints[4];
    GfxColor nativeColor;
    int i, j;
    int numFragments;
    GfxMarkFragment markFragments[384];
    GfxMarkFragment *mf;
    MarkPoly *mark;
    GfxWorldVertex *verts;

    if (cg_marks->current.enabled && !cg->nomarks) {
        // create the texture axis
        Vec3NormalizeTo(dir, axis[0]);
        PerpendicularVector(axis[0], axis[1]);
        RotatePointAroundVector(axis[2], axis[0], axis[1], orientation);
        Vec3Cross(axis[0], axis[2], axis[1]);

        // create the full polygon
        for (i = 0; i < 3; i++) {
            originalPoints[0][i] = origin[i] - radius * axis[1][i] - radius * axis[2][i];
            originalPoints[1][i] = origin[i] + radius * axis[1][i] - radius * axis[2][i];
            originalPoints[2][i] = origin[i] + radius * axis[1][i] + radius * axis[2][i];
            originalPoints[3][i] = radius * axis[2][i] + (origin[i] - radius * axis[1][i]);
        }

        numFragments = CL_MarkFragments(originalPoints, origin, axis, radius, 1024, markVerts, 384, markFragments, markMaterial);
        Byte4PackVertexColor(color, nativeColor.array);

        for (i = 0, mf = markFragments; i < numFragments; i++, mf++) {
            verts = &markVerts[mf->firstPoint];
            for (j = 0; j < mf->pointCount; j++)
                verts[j].color.packed = nativeColor.packed;

            mark = CG_AllocMark();
            mark->origin[0] = origin[0];
            mark->origin[1] = origin[1];
            mark->origin[2] = origin[2];
            mark->radius = radius;
            mark->mtlHandle = mf->markMaterial;
            mark->lmapIndex = mf->lmapIndex;
            mark->vertCount = mf->pointCount;
            mark->lastFrameDrawn = cg->clientFrame;
            memcpy(mark->verts, &markVerts[mf->firstPoint], mf->pointCount * sizeof(GfxWorldVertex));
        }
    }
}

