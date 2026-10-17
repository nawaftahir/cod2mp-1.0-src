/* MSVC-only pthread shim, backed by the Win32 thread API.
 *
 * CRITICAL: this must NOT include <windows.h>. The engine defines its own
 * complete Win32 type universe in cod2_defs.h (HANDLE, RTL_CRITICAL_SECTION,
 * ...), and pulling in the real Windows SDK headers collides with all of them.
 * So we declare the handful of Win32 entry points ourselves against opaque,
 * layout-compatible storage. (MinGW supplies its own winpthread <pthread.h>;
 * this dir is on the include path for the MSVC build only.)
 *
 * Only the surface the engine uses is provided: recursive mutexes + create/join.
 */
#ifndef COD2_MSVC_PTHREAD_H
#define COD2_MSVC_PTHREAD_H

#ifndef _WIN32
#error "shims-msvc/pthread.h is for the MSVC/Win32 build only"
#endif

#include <stdint.h>

/* CRITICAL_SECTION is 24 bytes on x86 (6 pointer/long fields). Keep it opaque
 * and over-aligned so the kernel32 calls below operate on correct storage. */
typedef struct cod2_critsec_ { void *opaque[6]; } pthread_mutex_t;
typedef void *pthread_t;
typedef int   pthread_mutexattr_t;
typedef int   pthread_attr_t;

#define PTHREAD_MUTEX_RECURSIVE   1
#define PTHREAD_MUTEX_INITIALIZER { { 0, 0, 0, 0, 0, 0 } }

/* Hand-rolled __stdcall (WINAPI) prototypes -- resolved from kernel32 at link,
 * no SDK header needed. void* stands in for LPCRITICAL_SECTION / HANDLE. */
#ifdef __cplusplus
extern "C" {
#endif
__declspec(dllimport) void __stdcall InitializeCriticalSection(void *);
__declspec(dllimport) void __stdcall DeleteCriticalSection(void *);
__declspec(dllimport) void __stdcall EnterCriticalSection(void *);
__declspec(dllimport) void __stdcall LeaveCriticalSection(void *);
__declspec(dllimport) int  __stdcall TryEnterCriticalSection(void *);
__declspec(dllimport) unsigned long __stdcall WaitForSingleObject(void *, unsigned long);
__declspec(dllimport) int  __stdcall CloseHandle(void *);
__declspec(dllimport) void *__stdcall GetCurrentThread(void);
#ifdef __cplusplus
}
#endif
/* _beginthreadex: include the CRT header (no Win32 type collisions) so its
 * exact linkage matches -- a hand-rolled prototype clashes (C2375). */
#include <process.h>

static __inline int pthread_mutexattr_init(pthread_mutexattr_t *a)            { if (a) *a = 0; return 0; }
static __inline int pthread_mutexattr_destroy(pthread_mutexattr_t *a)         { (void)a; return 0; }
static __inline int pthread_mutexattr_settype(pthread_mutexattr_t *a, int t)  { if (a) *a = t; return 0; }

static __inline int pthread_mutex_init(pthread_mutex_t *m, const pthread_mutexattr_t *a) {
    (void)a; InitializeCriticalSection(m); return 0;   /* CRITICAL_SECTION is recursive */
}
static __inline int pthread_mutex_destroy(pthread_mutex_t *m) { DeleteCriticalSection(m); return 0; }
static __inline int pthread_mutex_lock(pthread_mutex_t *m)    { EnterCriticalSection(m);  return 0; }
static __inline int pthread_mutex_unlock(pthread_mutex_t *m)  { LeaveCriticalSection(m);  return 0; }
static __inline int pthread_mutex_trylock(pthread_mutex_t *m) { return TryEnterCriticalSection(m) ? 0 : 16 /*EBUSY*/; }

static __inline int pthread_create(pthread_t *t, const pthread_attr_t *a,
                                   void *(*fn)(void *), void *arg) {
    uintptr_t h;
    (void)a;
    h = _beginthreadex(0, 0, (unsigned(__stdcall *)(void *))fn, arg, 0, 0);
    if (!h) return -1;
    if (t) *t = (pthread_t)h;
    return 0;
}
static __inline int pthread_join(pthread_t t, void **ret) {
    WaitForSingleObject(t, 0xFFFFFFFFu /*INFINITE*/);
    if (ret) *ret = 0;
    CloseHandle(t);
    return 0;
}
static __inline pthread_t pthread_self(void) { return GetCurrentThread(); }

/* pthread_main_np is provided by headers/win32_compat.h -- not redefined here. */

#endif /* COD2_MSVC_PTHREAD_H */
