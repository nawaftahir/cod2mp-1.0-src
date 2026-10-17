/* MSVC-only <sys/time.h> shim: gettimeofday backed by the CRT _ftime64
 * (sys/timeb.h) -- no <windows.h>, no <winsock2.h>. MSVC-build include dir only. */
#ifndef COD2_MSVC_SYS_TIME_H
#define COD2_MSVC_SYS_TIME_H
#ifdef _WIN32

#include <sys/timeb.h>
#include <time.h>

/* struct timeval may already come from <winsock2.h>; guard against redefinition. */
#ifndef _TIMEVAL_DEFINED
#define _TIMEVAL_DEFINED
struct timeval { long tv_sec; long tv_usec; };
#endif
struct timezone { int tz_minuteswest; int tz_dsttime; };

static __inline int gettimeofday(struct timeval *tv, void *tz)
{
    struct __timeb64 tb;
    (void)tz;
    if (!tv) return -1;
    _ftime64(&tb);
    tv->tv_sec  = (long)tb.time;
    tv->tv_usec = (long)tb.millitm * 1000;
    return 0;
}

#endif /* _WIN32 */
#endif
