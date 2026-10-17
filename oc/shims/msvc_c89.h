#ifndef COD2_MSVC_C89_H
#define COD2_MSVC_C89_H

/* cl 7.1 is a C89 frontend. Decompiler-derived C uses C99 spellings; map the
 * common ones so the reference source parses. Anything the 2003 compiler
 * cannot represent must be edited in the adopted source, not shimmed away. */

#define inline __inline
#define __inline__ __inline
#define restrict __restrict
#define __restrict__ __restrict

/* __asm__ symbol-rename form cannot be macro'd; opencod2 uses it rarely and it
 * is stripped at adoption time. Statement-form __asm__ is handled by
 * msvc_gcc_compat.h already. */

#endif