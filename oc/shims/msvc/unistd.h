/* MSVC-only <unistd.h> shim. Maps the POSIX file/process surface the engine
 * uses onto the CRT (<io.h>/<process.h>/<direct.h>) -- deliberately WITHOUT
 * <windows.h>, to keep the Windows SDK types out of engine TUs. MinGW supplies
 * its own; this dir is on the include path for MSVC only. */
#ifndef COD2_MSVC_UNISTD_H
#define COD2_MSVC_UNISTD_H
#ifdef _WIN32

#include <io.h>        /* _close _read _write _access _dup2 _isatty */
#include <process.h>   /* _getpid */
#include <direct.h>    /* _getcwd _chdir _rmdir */
#include <stdlib.h>

#define close   _close
#define read    _read
#define write   _write
#define access  _access
#define dup2    _dup2
#define isatty  _isatty
#define getpid  _getpid
#define getcwd  _getcwd
#define chdir   _chdir
#define rmdir   _rmdir
#define lseek   _lseek
#define unlink  _unlink

#ifndef F_OK
#define F_OK 0
#define X_OK 1
#define W_OK 2
#define R_OK 4
#endif
#ifndef STDIN_FILENO
#define STDIN_FILENO  0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2
#endif

/* Sleep lives in kernel32; declare it lean to avoid pulling <windows.h>. */
__declspec(dllimport) void __stdcall Sleep(unsigned long);
static __inline int usleep(unsigned long usec) { Sleep(usec / 1000u); return 0; }
static __inline unsigned int sleep(unsigned int sec) { Sleep(sec * 1000u); return 0; }

/* readlink: no symlink-resolve surface needed -- report "not a link". */
static __inline long readlink(const char *p, char *b, unsigned long n)
{ (void)p; (void)b; (void)n; return -1; }

#endif /* _WIN32 */
#endif
