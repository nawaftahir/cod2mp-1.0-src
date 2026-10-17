/* MSVC <- GCC compatibility shim (force-included first on the MSVC build).
 *
 * Neutralises the GCC-isms sprinkled through the reconstructed headers/sources
 * so cl's frontend can parse them. This is the START of Stage 4 -- it is NOT
 * complete or fully correct yet; see the KNOWN-INCORRECT notes below.
 *
 * MinGW never sees this file (it is in shims-msvc/, an MSVC-only include dir).
 */
#ifndef COD2_MSVC_GCC_COMPAT_H
#define COD2_MSVC_GCC_COMPAT_H
#ifdef _MSC_VER

/* --- keyword spellings ---------------------------------------------------- */
#define __inline__    __inline
#define __volatile__  volatile
#define __restrict__  __restrict
#ifndef __asm__
/* NOTE: the *symbol-rename* form `T x __asm__("name")` is Stage 3 and cannot be
 * macro'd away. This only covers stray statement-form uses; rename sites are
 * removed at the source in Stage 3. */
#endif

/* --- __attribute__ --------------------------------------------------------
 * collapsed to nothing. Fine for the codegen/alignment-agnostic attributes
 * (noinline, used, visibility, regparm, sseregparm, cdecl/stdcall already
 * no-op'd in cod2_platform.h for non-GCC). The two layout-affecting cases:
 *   - ((packed)): the real packed structs (VoicePacket_t in cod2_defs.h /
 *     common_types.h, _d32_g_CurrentGenericPacket) are wrapped at the site with
 *     `#ifdef _MSC_VER #pragma pack(push,1)/pop` so MSVC packs them too.
 *   - ((aligned(n))): almost all are aligned(4) (== natural alignment, so the
 *     no-op is harmless). The generated data-blob _d32_ structs (added in
 *     Stage 2/6) carry packed/aligned and must be emitted with pragma packing
 *     when they enter the MSVC build. */
#ifndef __attribute__
#define __attribute__(x)
#endif

/* --- alignof spelling ------------------------------------------------------
 * GCC's __alignof__ -> cl's __alignof (used by speex's PUSH/PUSHS macros). */
#define __alignof__(x)  __alignof(x)

/* --- builtins -------------------------------------------------------------
 * Map the GCC builtins this tree uses onto MSVC/CRT equivalents.
 * IMPORTANT: leaving the float math builtins (__builtin_sqrtf/__builtin_fabsf)
 * unmapped makes cl's *backend* (p2) ICE during codegen, not just warn -- so
 * these must be real libc calls. */
#include <stddef.h>   /* offsetof */
#include <math.h>     /* sqrtf, fabsf */
#include <string.h>   /* memset, memcmp, strlen */
#include <stdlib.h>   /* _byteswap_* */
#include <malloc.h>   /* _alloca */
/* NOT <intrin.h>: it defines the real __m128, which collides with the engine's
 * own `typedef float __m128[4]` in cod2_defs.h. Declare the few intrinsics we
 * need by hand instead. */
#ifdef __cplusplus
extern "C" {
#endif
void *_ReturnAddress(void);
void  __debugbreak(void);
unsigned char _BitScanReverse(unsigned long *, unsigned long);
#ifdef __cplusplus
}
#endif
#pragma intrinsic(_ReturnAddress, _BitScanReverse)

#ifndef __builtin_expect
#define __builtin_expect(expr, c)  (expr)
#endif
#ifndef __builtin_offsetof
#define __builtin_offsetof(type, member)  offsetof(type, member)
#endif

#define __builtin_sqrtf(x)        sqrtf(x)
#define __builtin_sqrt(x)         sqrt(x)
#define __builtin_fabsf(x)        fabsf(x)
#define __builtin_fabs(x)         fabs(x)
#define __builtin_memset(d,c,n)   memset((d),(c),(n))
#define __builtin_memcmp(a,b,n)   memcmp((a),(b),(n))
#define __builtin_strlen(s)       strlen(s)
#define __builtin_alloca(n)       _alloca(n)
#define __builtin_bswap16(x)      _byteswap_ushort(x)
#define __builtin_bswap32(x)      _byteswap_ulong(x)
#define __builtin_bswap64(x)      _byteswap_uint64(x)
#define __builtin_return_address(lvl)  _ReturnAddress()   /* lvl>0 unsupported */
#define __builtin_debugtrap()     __debugbreak()
#define __builtin_prefetch(...)   ((void)0)
static __inline int __cod2_clz(unsigned x) { unsigned long i; return _BitScanReverse(&i, x) ? (int)(31 - i) : 32; }
#define __builtin_clz(x)          __cod2_clz(x)

/* va_list builtins -> <stdarg.h> spellings. */
#include <stdarg.h>
#define __builtin_va_list         va_list
#define __builtin_va_start(ap,l)  va_start(ap, l)
#define __builtin_va_end(ap)      va_end(ap)

/* TODO(Stage 4): __builtin_add_overflow / __builtin_mul_overflow (4 sites) need
 * correctly-typed checked-arithmetic impls; left unmapped for now. */

/* --- GCC __sync_* atomics -> MSVC _Interlocked* intrinsics ------------------ */
#ifdef __cplusplus
extern "C" {
#endif
long _InterlockedExchangeAdd(long volatile *, long);
long _InterlockedExchange(long volatile *, long);
long _InterlockedCompareExchange(long volatile *, long, long);
#ifdef __cplusplus
}
#endif
#pragma intrinsic(_InterlockedExchangeAdd, _InterlockedExchange, _InterlockedCompareExchange)
#define __sync_fetch_and_add(p, v)            _InterlockedExchangeAdd((long volatile *)(p), (long)(v))
#define __sync_add_and_fetch(p, v)            (_InterlockedExchangeAdd((long volatile *)(p), (long)(v)) + (long)(v))
#define __sync_lock_test_and_set(p, v)        _InterlockedExchange((long volatile *)(p), (long)(v))
#define __sync_val_compare_and_swap(p, o, n)  _InterlockedCompareExchange((long volatile *)(p), (long)(n), (long)(o))
#define __sync_bool_compare_and_swap(p, o, n) (_InterlockedCompareExchange((long volatile *)(p), (long)(n), (long)(o)) == (long)(o))

/* BSD case-insensitive compares used without including <strings.h> */
#define strcasecmp  _stricmp
#define strncasecmp _strnicmp

/* GCC __attribute__((constructor)) functions don't run on MSVC (the attribute
 * no-ops). COD2_CONSTRUCTOR(fn){..} registers fn in the CRT's .CRT$XCU init
 * array so it runs before main, as the GCC constructor would.
 *   usage: COD2_CONSTRUCTOR(init_rune_locale) { ... }   (replaces the attribute) */
#pragma section(".CRT$XCU", read)   /* declare the CRT init array section once */
#define COD2_CONSTRUCTOR(fn)                                                 \
    static void fn(void);                                                   \
    __declspec(allocate(".CRT$XCU")) static void (*fn##__crtreg)(void) = fn;\
    static void fn(void)

/* POSIX stat type-test macros MSVC's <sys/stat.h> lacks (else they look like
 * undefined functions -> unresolved _S_ISDIR/_S_ISREG). */
#ifndef S_ISDIR
#define S_ISDIR(m) (((m) & 0xF000) == 0x4000)
#define S_ISREG(m) (((m) & 0xF000) == 0x8000)
#endif

/* COD2_ALT(lhs, rhs): emit a linker /alternatename directive from UNDECORATED C
 * symbol names. x86 cl prefixes C symbols with one leading '_'; x64 cl uses no
 * prefix. The alias-seam lists (msvc_alias_pragmas.h, msvc_seam_aliases.h,
 * msvc_link_glue.c) pass bare names so this adds the right decoration per arch.
 * (Adjacent string-literal concatenation inside __pragma is well-defined.) */
#if defined(_M_X64) || defined(_M_ARM64)
#define COD2_ALT_U_ ""
#else
#define COD2_ALT_U_ "_"
#endif
#define COD2_ALT(lhs, rhs) \
    __pragma(comment(linker, "/alternatename:" COD2_ALT_U_ lhs "=" COD2_ALT_U_ rhs))

#endif /* _MSC_VER */
#endif /* COD2_MSVC_GCC_COMPAT_H */
