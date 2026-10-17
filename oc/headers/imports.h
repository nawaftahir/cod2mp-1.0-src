#ifndef IMPORTS_H
#define IMPORTS_H

#include "imports/agl.h"
#include "imports/audiotoolbox.h"
#include "imports/carbon.h"
#include "imports/corefoundation.h"
#include "imports/iokit.h"
#include "imports/opengl.h"
#include "imports/quicktime.h"
#include "imports/libc.h"
#include "imports/libstdcxx.h"
#include "generated_syms.h"

#ifdef GFX_REAL_D3D9
#define D3DVTCC __attribute__((stdcall))
#else
#define D3DVTCC
#endif

#endif
