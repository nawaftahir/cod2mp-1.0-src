/* MSVC-only <strings.h> shim (BSD case-insensitive string ops).
 * MinGW provides its own; this dir is on the include path for MSVC only.
 * Only the surface the engine uses is mapped -- deliberately NOT defining
 * index()/rindex() (the codebase uses `index` widely as an identifier). */
#ifndef COD2_MSVC_STRINGS_H
#define COD2_MSVC_STRINGS_H
#ifdef _WIN32
#include <string.h>
#define strcasecmp  _stricmp
#define strncasecmp _strnicmp
#endif
#endif
