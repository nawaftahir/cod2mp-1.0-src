#ifndef WIN32_COMPAT_H
#define WIN32_COMPAT_H

#ifdef _WIN32

#include <io.h>
#include <direct.h>

#ifndef COD2_NO_MKDIR_SHIM
#define mkdir(path, mode) _mkdir(path)
#endif

static __inline int pthread_main_np(void) { return 1; }

#endif
#endif
