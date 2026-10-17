#include "cod2_feature_config.h"

#if COD2_FEATURE_RUMBLE

#    include <string.h>
#    include <stdlib.h>
#    include <math.h>

#    define __m128 __m128_cod2
#    include "common_types.h"
#    undef __m128

extern int FS_FOpenFileByMode(const char *filename, int *fh, fsMode_t mode);
extern int FS_Read(void *buffer, int len, int fh);
extern void FS_FCloseFile(int fh);
extern char *Com_Parse( const char *( *data_p ) );
extern void Com_BeginParseSession(const char *name);
extern void Com_EndParseSession(void);
extern void Com_Printf(const char *fmt, ...);
extern void Com_DPrintf(const char *fmt, ...);
dvar_t *Dvar_RegisterBool(const char *dvarName, bool value, unsigned short flags);
extern void Cmd_AddCommand(const char *cmd, void (*func)(void));
extern void Cmd_ArgvBuffer(int arg, char *buf, int bufSize);
extern int Cmd_Argc(void);
extern int Sys_Milliseconds(void);

extern void CL_Gamepad_SetRumble(float lowFreq, float highFreq);

#    define MAX_RUMBLE_GRAPH_KNOTS 16
#    define MAX_RUMBLE_INFOS 32
#    define MAX_ACTIVE_RUMBLES 32
#    define RUMBLE_DURATION_MS 1000

typedef struct {
    char name[64];
    int knotCount;
    vec2_t knots[MAX_RUMBLE_GRAPH_KNOTS];
    int loaded;
} RumbleGraph;

typedef struct {
    char name[64];
    RumbleGraph high;
    RumbleGraph low;
    int loaded;
} RumbleInfo;

typedef struct {
    int active;
    int infoIndex;
    int startTime;
    int loop;
} ActiveRumble;

static RumbleInfo g_rumbleInfos[MAX_RUMBLE_INFOS];
static int g_rumbleInfoCount;
static ActiveRumble g_activeRumbles[MAX_ACTIVE_RUMBLES];
static const dvar_t *cg_drawRumbleDebug;

static int I_stricmp_local(const char *a, const char *b);

static qboolean CG_ParseRumbleGraph(RumbleGraph *graph, const char *buffer,
                                    const char *fileName)
{
    const char *bufPtr = buffer;
    const char *token;
    int declaredCount;
    int i;

    Com_BeginParseSession(fileName);

    token = Com_Parse(&bufPtr);
    if (I_stricmp_local(token, "RUMBLEGRAPHFILE") != 0) {
        Com_Printf("^1\"%s\" does not appear to be a rumble graph file\n", fileName);
        Com_EndParseSessionInl();
        return 0;
    }

    token = Com_Parse(&bufPtr);
    declaredCount = atoi(token);

    for (i = 0; i < MAX_RUMBLE_GRAPH_KNOTS; ++i) {
        token = Com_Parse(&bufPtr);
        if (token[0] == '\0' || token[0] == '}')
            break;
        graph->knots[i][0] = (float)atof(token);

        token = Com_Parse(&bufPtr);
        if (token[0] == '\0' || token[0] == '}')
            break;
        graph->knots[i][1] = (float)atof(token);
    }

    if (i >= MAX_RUMBLE_GRAPH_KNOTS) {
        Com_Printf("^3WARNING: \"%s\" has too many graph knots\n", fileName);
        Com_EndParseSessionInl();
        return 0;
    }
    Com_EndParseSessionInl();

    if (declaredCount != i) {
        Com_Printf("^1\"%s\" Error in parsing a rumble graph file\n", fileName);
        return 0;
    }

    graph->knotCount = i;
    graph->loaded = 1;
    return 1;
}

static int I_stricmp_local(const char *a, const char *b)
{
    for (;;) {
        int ca = *a++, cb = *b++;
        if (ca >= 'A' && ca <= 'Z')
            ca += 32;
        if (cb >= 'A' && cb <= 'Z')
            cb += 32;
        if (ca != cb)
            return ca - cb;
        if (ca == '\0')
            return 0;
    }
}

static qboolean CG_ReadRumbleGraph(RumbleGraph *graph, const char *fileName)
{
    char path[256];
    char buffer[4096];
    int fh = 0;
    int len;

    if (!fileName || !fileName[0])
        return 0;

    {
        int n = 0;
        const char *p = "rumble/";
        while (*p)
            path[n++] = *p++;
        p = fileName;
        while (*p && n < (int)sizeof(path) - 5)
            path[n++] = *p++;
        p = ".rmb";
        while (*p)
            path[n++] = *p++;
        path[n] = '\0';
    }

    Com_DPrintf("Parsing rumble graph file \"%s\"..\n", path);

    len = FS_FOpenFileByMode(path, &fh, FS_READ );
    if (len <= 0 || !fh) {
        Com_Printf("^1Could not load rumble graph file \"%s\"\n", path);
        if (fh)
            FS_FCloseFile(fh);
        return 0;
    }
    if (len > (int)sizeof(buffer) - 1)
        len = (int)sizeof(buffer) - 1;
    FS_Read(buffer, len, fh);
    buffer[len] = '\0';
    FS_FCloseFile(fh);

    return CG_ParseRumbleGraph(graph, buffer, path);
}

static qboolean CG_LoadRumbleGraph(RumbleInfo *info, const char *highName,
                                   const char *lowName)
{
    info->high.loaded = 0;
    info->low.loaded = 0;

    if (highName && highName[0]) {
        if (!CG_ReadRumbleGraph(&info->high, highName))
            return 0;
    }
    if (lowName && lowName[0]) {
        if (!CG_ReadRumbleGraph(&info->low, lowName))
            return 0;
    }
    return (info->high.loaded || info->low.loaded);
}

int CG_GetRumbleInfoIndexFromName(const char *name)
{
    int i;
    for (i = 0; i < g_rumbleInfoCount; ++i) {
        if (I_stricmp_local(g_rumbleInfos[i].name, name) == 0)
            return i;
    }
    return -1;
}

int CG_LoadRumble(const char *name)
{
    int idx;
    char highName[64], lowName[64];
    RumbleInfo *info;

    if (!name || !name[0])
        return -1;

    idx = CG_GetRumbleInfoIndexFromName(name);
    if (idx >= 0)
        return idx;

    if (g_rumbleInfoCount >= MAX_RUMBLE_INFOS) {
        Com_Printf("^1No more room to allocate anymore rumble infos\n");
        return -1;
    }

    {
        int n = 0;
        const char *p = name;
        while (*p && n < 60) {
            highName[n] = *p;
            lowName[n] = *p;
            n++;
            p++;
        }
        highName[n] = '_';
        highName[n + 1] = 'h';
        highName[n + 2] = '\0';
        lowName[n] = '_';
        lowName[n + 1] = 'l';
        lowName[n + 2] = '\0';
    }

    info = &g_rumbleInfos[g_rumbleInfoCount];
    memset(info, 0, sizeof(*info));
    if (!CG_LoadRumbleGraph(info, highName, lowName)) {
        Com_Printf("^1Could not load rumble info file '%s'\n", name);
        return -1;
    }

    {
        int n = 0;
        const char *p = name;
        while (*p && n < 63) {
            info->name[n++] = *p++;
        }
        info->name[n] = '\0';
    }
    info->loaded = 1;
    return g_rumbleInfoCount++;
}

void CG_InvalidateActiveRumble(int index)
{
    if (index < 0 || index >= MAX_ACTIVE_RUMBLES)
        return;
    g_activeRumbles[index].active = 0;
    g_activeRumbles[index].infoIndex = -1;
    g_activeRumbles[index].loop = 0;
}

void CG_StopAllRumbles(void)
{
    int i;
    for (i = 0; i < MAX_ACTIVE_RUMBLES; ++i)
        CG_InvalidateActiveRumble(i);
    CL_Gamepad_SetRumble(0.0f, 0.0f);
}

static int CG_FindClosestToDyingActiveRumble(void)
{
    int i, best = -1, now = Sys_Milliseconds();
    int bestRemaining = 0x7fffffff;
    for (i = 0; i < MAX_ACTIVE_RUMBLES; ++i) {
        int remaining;
        if (!g_activeRumbles[i].active)
            return i;
        if (g_activeRumbles[i].loop)
            continue;
        remaining = RUMBLE_DURATION_MS - (now - g_activeRumbles[i].startTime);
        if (remaining < bestRemaining) {
            bestRemaining = remaining;
            best = i;
        }
    }
    return best;
}

void CG_RemoveInactiveRumbles(void)
{
    int i, now = Sys_Milliseconds();
    for (i = 0; i < MAX_ACTIVE_RUMBLES; ++i) {
        if (!g_activeRumbles[i].active)
            continue;
        if (g_activeRumbles[i].loop)
            continue;
        if (now - g_activeRumbles[i].startTime >= RUMBLE_DURATION_MS)
            CG_InvalidateActiveRumble(i);
    }
}

static float CG_EvalRumbleGraph(const RumbleGraph *g, float t)
{
    int i;
    if (!g->loaded || g->knotCount <= 0)
        return 0.0f;
    if (t <= g->knots[0][0])
        return g->knots[0][1];
    if (t >= g->knots[g->knotCount - 1][0])
        return g->knots[g->knotCount - 1][1];
    for (i = 1; i < g->knotCount; ++i) {
        if (t <= g->knots[i][0]) {
            float t0 = g->knots[i - 1][0], v0 = g->knots[i - 1][1];
            float t1 = g->knots[i][0], v1 = g->knots[i][1];
            float span = t1 - t0;
            float frac = (span > 0.0f) ? (t - t0) / span : 0.0f;
            return v0 + (v1 - v0) * frac;
        }
    }
    return g->knots[g->knotCount - 1][1];
}

void CG_CalcActiveRumbles(void)
{
    int i, now = Sys_Milliseconds();
    float low = 0.0f, high = 0.0f;

    for (i = 0; i < MAX_ACTIVE_RUMBLES; ++i) {
        ActiveRumble *ar = &g_activeRumbles[i];
        RumbleInfo *info;
        float t;
        if (!ar->active)
            continue;
        if (ar->infoIndex < 0 || ar->infoIndex >= g_rumbleInfoCount)
            continue;
        info = &g_rumbleInfos[ar->infoIndex];

        t = (float)(now - ar->startTime) / (float)RUMBLE_DURATION_MS;
        if (ar->loop) {
            t = t - (float)((int)t);
            if (t < 0.0f)
                t += 1.0f;
        } else if (t > 1.0f) {
            t = 1.0f;
        }

        high += CG_EvalRumbleGraph(&info->high, t);
        low += CG_EvalRumbleGraph(&info->low, t);
    }

    if (low > 1.0f)
        low = 1.0f;
    if (high > 1.0f)
        high = 1.0f;
    if (low < 0.0f)
        low = 0.0f;
    if (high < 0.0f)
        high = 0.0f;

    CL_Gamepad_SetRumble(low, high);

    if (cg_drawRumbleDebug && cg_drawRumbleDebug->current.enabled &&
        (low > 0.0f || high > 0.0f)) {
        Com_Printf("Rumble: Low %.3f, High %.3f\n", low, high);
    }
}

void CG_UpdateRumble(void)
{
    CG_RemoveInactiveRumbles();
    CG_CalcActiveRumbles();
}

static void CG_PlayRumbleInternal(const char *name, int loop)
{
    int infoIndex, slot;

    if (!name || !name[0])
        return;

    infoIndex = CG_LoadRumble(name);
    if (infoIndex < 0) {
        Com_Printf("^1Could not play rumble asset '%s'\n", name);
        return;
    }

    slot = CG_FindClosestToDyingActiveRumble();
    if (slot < 0)
        return;

    g_activeRumbles[slot].active = 1;
    g_activeRumbles[slot].infoIndex = infoIndex;
    g_activeRumbles[slot].startTime = Sys_Milliseconds();
    g_activeRumbles[slot].loop = loop;
}

void CG_PlayRumble(const char *name)
{
    CG_PlayRumbleInternal(name, 0);
}
void CG_PlayLoopRumble(const char *name)
{
    CG_PlayRumbleInternal(name, 1);
}

void CG_StopRumble(const char *name)
{
    int idx = CG_GetRumbleInfoIndexFromName(name);
    int i;
    if (idx < 0)
        return;
    for (i = 0; i < MAX_ACTIVE_RUMBLES; ++i) {
        if (g_activeRumbles[i].active && g_activeRumbles[i].infoIndex == idx)
            CG_InvalidateActiveRumble(i);
    }
}

static void CG_PlayRumble_f(void)
{
    char arg[256];
    if (Cmd_Argc() != 2) {
        Com_Printf("USAGE: playrumble <rumblename>\n");
        return;
    }
    Cmd_ArgvBuffer(1, arg, sizeof(arg));
    CG_PlayRumble(arg);
}

void CG_InitRumble(void)
{
    int i;
    g_rumbleInfoCount = 0;
    for (i = 0; i < MAX_ACTIVE_RUMBLES; ++i)
        CG_InvalidateActiveRumble(i);

    cg_drawRumbleDebug = Dvar_RegisterBool("cg_drawrumbledebug", 0, 0x0);
    Cmd_AddCommand("playrumble", CG_PlayRumble_f);
}

void CG_ShutdownRumble(void)
{
    CG_StopAllRumbles();
}

#else

typedef int cg_rumble_mp_translation_unit_not_empty;

#endif
