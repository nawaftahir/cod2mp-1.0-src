/* MSVC-only <sched.h> shim. The engine only needs sched_yield to exist; map it
 * to a Win32 thread-yield (Sleep(0)) without dragging in <windows.h>. */
#ifndef COD2_MSVC_SCHED_H
#define COD2_MSVC_SCHED_H
#ifdef _WIN32
__declspec(dllimport) void __stdcall Sleep(unsigned long);
static __inline int sched_yield(void) { Sleep(0); return 0; }
#endif
#endif
