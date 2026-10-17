#ifndef CLEAN_PC_QCOMMON_QCOMMON_TYPES_H
#define CLEAN_PC_QCOMMON_QCOMMON_TYPES_H

#include "../../cod2_fwd.h"
#include "../../cod2_defs.h"

typedef struct CollisionAabbTree CollisionAabbTree;
typedef struct CollisionBorder CollisionBorder;
typedef struct CollisionEdge CollisionEdge;
typedef struct CollisionPartition CollisionPartition;
typedef struct CollisionTriangle CollisionTriangle;
typedef struct CollisionVertex CollisionVertex;
typedef struct MD4_CTX MD4_CTX;
typedef struct NetField NetField;
typedef struct areaParms_t areaParms_t;
typedef struct cLeafBrushNode_s cLeafBrushNode_s;
typedef struct cNode_t cNode_t;
typedef struct cStaticModel_s cStaticModel_s;
typedef struct cbrush_t cbrush_t;
typedef struct cbrushside_t cbrushside_t;
typedef struct cmd_t cmd_t;
typedef struct clipMap_t clipMap_t;
typedef struct loopback_t loopback_t;
typedef struct huffman_t huffman_t;
typedef struct staticmodeltrace_t staticmodeltrace_t;
typedef struct statmonitor_s statmonitor_s;
typedef struct cm_world_t cm_world_t;

struct CollisionAabbTree {
    vec3_t origin;
    vec3_t halfSize;
    short unsigned int materialIndex;
    short unsigned int childCount;
    union CollisionAabbTreeIndex u;
};

struct CollisionBorder {
    vec3_t distEq;
    vec_t zBase;
    vec_t zSlope;
    vec_t start;
    vec_t length;
};

struct CollisionEdge {
    vec3_t origin;
    vec3_t axis[3];
};

struct CollisionPartition {
    byte triCount;
    byte borderCount;
    CollisionTriangle *tris;
    CollisionBorder *borders;
};

struct CollisionTriangle {
    vec4_t plane;
    vec4_t svec;
    vec4_t tvec;
    int verts[3];
    int edges[3];
};

struct CollisionVertex {
    vec3_t xyz;
};

struct MD4_CTX {
    UINT4 state[4];
    UINT4 count[2];
    unsigned char buffer[64];
};

struct NetField {
    char *name;
    int offset;
    int bits;
};

struct areaParms_t {
    const float *start;
    const float *mins;
    const float *maxs;
    int *list;
    int count;
    int maxcount;
    int contentmask;
    trace_t *results;
};

struct cLeafBrushNode_s {
    byte axis;
    short int leafBrushCount;
    int contents;
    union cLeafBrushNodeData_t data;
};

struct cNode_t {
    cplane_t *plane;
    short int children[2];
};

struct cStaticModel_s {
    struct cStaticModelWritable writable;
    struct XModel *xmodel;
    vec3_t origin;
    vec3_t invAxis[3];
    vec3_t absmin;
    vec3_t absmax;
};

struct cbrush_t {
    vec3_t mins;
    vec3_t maxs;
    int contents;
    int numsides;
    cbrushside_t *sides;
    short int axialMaterialNum[2][3];
};

struct cbrushside_t {
    cplane_t *plane;
    int materialNum;
};

struct cmd_t {
    byte *data;
    int maxsize;
    int cmdsize;
};

struct clipMap_t {
    const char *name;
    int numStaticModels;
    cStaticModel_t *staticModelList;
    int numMaterials;
    dmaterial_t *materials;
    int numBrushSides;
    cbrushside_t *brushsides;
    int numNodes;
    cNode_t *nodes;
    int numLeafs;
    cLeaf_t *leafs;
    int leafbrushNodesCount;
    cLeafBrushNode_t *leafbrushNodes;
    int numLeafBrushes;
    short unsigned int *leafbrushes;
    int numLeafSurfaces;
    int *leafsurfaces;
    int vertCount;
    CollisionVertex *verts;
    int edgeCount;
    CollisionEdge *edges;
    int triCount;
    CollisionTriangle *tris;
    int borderCount;
    CollisionBorder *borders;
    int partitionCount;
    CollisionPartition *partitions;
    int aabbTreeCount;
    CollisionAabbTree *aabbTrees;
    int numSubModels;
    cmodel_t *cmodels;
    short unsigned int numBrushes;
    cbrush_t *brushes;
    int numClusters;
    int clusterBytes;
    byte *visibility;
    qboolean vised;
    int numEntityChars;
    char *entityString;
    cbrush_t *box_brush;
    cmodel_t box_model;
    int pathNodeCount;
    pathnode_t *pathNodes;
    int chainNodeCount;
    short unsigned int *chainNodes;
    short unsigned int *chainNodesRev;
    int visBytes;
    byte *pathVis;
    int nodeTreeCount;
    pathnode_tree_t *nodeTree;
    unsigned int checksum;
};

struct loopback_t {
    loopmsg_t msgs[16];
    int get;
    int send;
};

struct huffman_t {
    struct huff_t compressor;
    huff_t decompressor;
};

struct staticmodeltrace_t {
    struct TraceExtents extents;
    int contents;
};

struct statmonitor_s {
    int endtime;
    MaterialHandle material;
};

struct cm_world_t {
    vec3_t mins;
    vec3_t maxs;
    Bool lockTree;
    short unsigned int freeHead;
    worldSector_t sectors[1024];
};
#endif
