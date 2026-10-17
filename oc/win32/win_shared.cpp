#include "common_types.h"
#include "imports.h"

#ifdef __cplusplus
extern "C" {
#endif
__declspec(dllimport) DWORD __stdcall timeGetTime(void);
#ifdef __cplusplus
}
#endif

extern int sys_timeBase;
static qboolean initialized;

int Sys_Milliseconds(void)
{
    if (!initialized) {
        sys_timeBase = timeGetTime();
        initialized = 1;
    }
    return timeGetTime() - sys_timeBase;
}

int Sys_MillisecondsRaw(void)
{
    return timeGetTime();
}

void Sys_SnapVector(vec3_t v)
{
    v[0] = (float)(int)v[0];
    v[1] = (float)(int)v[1];
    v[2] = (float)(int)v[2];
}
