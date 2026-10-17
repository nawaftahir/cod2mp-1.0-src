#ifndef CLEAN_PC_GFX_D3D_RB_STATE_H
#define CLEAN_PC_GFX_D3D_RB_STATE_H

#include "../../cod2_fwd.h"
#include "../../cod2_defs.h"

struct XSurface_s {
    byte tileMode;
    short int vertCount;
    short int triCount;
    short int boneOffset;
    int (*triIndices)[4];
    XVertexBuffer *verts;
    struct XRigidSurface surfRigid;
    IDirect3DIndexBuffer9 *indexBuffer;
};

struct XVertexBuffer {
    XVertexInfo v;
    XBlendInfo w[1];
};
#endif
