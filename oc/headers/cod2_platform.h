#ifndef CLEAN_COD2_PLATFORM
#define CLEAN_COD2_PLATFORM

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <sys/types.h>
#ifndef _WIN32
#include <netinet/in.h>
#endif
#include <pthread.h>
#include <stdio.h>
#include "cod2_feature_config.h"

#if defined(__i386__) && defined(__GNUC__) && !defined(__clang__) && !defined(__EMSCRIPTEN__)
#define COD2_GCC_X86_ABI 1
#else
#define COD2_GCC_X86_ABI 0
#endif

#if COD2_GCC_X86_ABI && defined(__APPLE_CC__)
#define COD2_APPLE_GCC_X86_ABI 1
#else
#define COD2_APPLE_GCC_X86_ABI 0
#endif

#define __attribute_regparm__(n) __attribute__((regparm(n)))
#if COD2_GCC_X86_ABI
#define __attribute_sseregparm__ __attribute__((sseregparm))
#else
#define __attribute_sseregparm__
#endif

#if COD2_GCC_X86_ABI
#define COD2_REGPARM(n) __attribute__((regparm(n)))
#define COD2_SSEREGPARM __attribute__((sseregparm))
#define COD2_CDECL __attribute__((cdecl))
#define COD2_STDCALL __attribute__((stdcall))
#define COD2_FASTCALL __attribute__((fastcall))
#define COD2_FORCE_ALIGN_ARG_POINTER __attribute__((force_align_arg_pointer))
#define COD2_ALIGNED(n) __attribute__((aligned(n)))
#else
#define COD2_REGPARM(n)
#define COD2_SSEREGPARM
#define COD2_CDECL
#define COD2_STDCALL
#define COD2_FASTCALL
#define COD2_FORCE_ALIGN_ARG_POINTER
#define COD2_ALIGNED(n)
#endif

#if COD2_APPLE_GCC_X86_ABI
#define COD2_REGPARM_AND_STACKPARM __attribute__((regparmandstackparm))
#else
#define COD2_REGPARM_AND_STACKPARM
#endif

#endif
