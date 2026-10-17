/* spike: build gcc-era server source with MSVC 7.1 */
#define __attribute__(x)
#define ALIGNED(n) __declspec(align(n))
#define __inline__ __inline
#define __asm__ __asm
#define static_assert(e, m)
#define _Pragma(x)
#ifndef __cplusplus
#define _Bool unsigned char
#define inline __inline
#endif
#define WIN32 1
#define M_PI 3.14159265358979323846
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#define snprintf _snprintf
#define vsnprintf _vsnprintf
#define __sigsetjmp(b, m) setjmp(b)
#define sigsetjmp(b, m) setjmp(b)
#define chmod(p, m) 0
#define S_IRUSR 0
#define S_IWUSR 0
