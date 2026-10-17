#include "qcommon.h"
#include "cm_local.h"

#define VIS_HEADER  8

// raw lump descriptor as stored in the bsp header
struct cmvis_lump_t
{
	int filelen;
	int fileofs;
};

// the bsp header viewed as its lump table
struct cm_bsp_lumps_t
{
	cmvis_lump_t lumps[LUMP_COUNT];
};

static char *cmod_base;
static byte *cm_lumpData;

static void CM_InitBoxHull( void );

struct CollisionVert
{
	float x;
	float y;
	float z;
};

struct dbrush_t
{
	int16_t numSides;
	int16_t materialNum;
};

struct dbrushside_t
{
	union
	{
		int planeNum;
		float bound;
	};
	int materialNum;
};

static void CMod_LoadMaterials( cmvis_lump_t *l );
static cLeafBrushNode_s *CMod_AllocLeafBrushNode();
static void CMod_LoadSubmodels( cmvis_lump_t *l );
cLeafBrushNode_s* CMod_PartionLeafBrushes_r(uint16_t *leafBrushes, int numLeafBrushes, const vec3_t mins, const vec3_t maxs);
static void CMod_PartionLeafBrushes(uint16_t *leafBrushes, int numLeafBrushes, cLeaf_s *leaf);
static int CMod_GetLeafTerrainContents(cLeaf_s *leaf);
static void CMod_LoadSubmodelBrushNodes( cmvis_lump_t *l );
static void CMod_LoadNodes( cmvis_lump_t *l );
static void CMod_LoadBrushes( cmvis_lump_t *lBrushes, cmvis_lump_t *lSides );
static void CMod_LoadLeafs( cmvis_lump_t *l, bool usePvs );
static void CMod_LoadLeafBrushNodes( cmvis_lump_t *l );
static void CMod_LoadPlanes( const char *base, cmvis_lump_t *l );
static void CMod_LoadLeafBrushes( cmvis_lump_t *l );
static void CMod_LoadLeafSurfaces( cmvis_lump_t *l );
static void CMod_LoadCollisionVerts( cmvis_lump_t *l );
static void CMod_LoadCollisionEdges( cmvis_lump_t *l );
static void CMod_LoadCollisionTriangles( cmvis_lump_t *l );
static void CMod_LoadCollisionBorders( cmvis_lump_t *l );
static void CMod_LoadCollisionPartitions( cmvis_lump_t *l );
static void CMod_LoadCollisionAabbTrees( cmvis_lump_t *l );
static void CMod_LoadEntityString( cmvis_lump_t *l );
static void CMod_LoadVisibility( cmvis_lump_t *l );
static void CMod_LoadBrushRelated( cmvis_lump_t *lumps, bool usePvs );
void CM_LoadMapFromBsp(const char *name, bool usePvs);
static void CM_InitBoxHull( void );
void CM_Cleanup( void );
cplane_t *CM_GetPlaneNum( int planeNum );
int CM_LoadLumpFromFile( int lump, byte **data );

struct DiskBrushModel
{
	vec3_t mins;
	vec3_t maxs;
	int firstTriangle;
	int numTriangles;
	int firstSurface;
	int numSurfaces;
	int firstBrush;
	int numBrushes;
};

struct dnode_t
{
	int planeNum;
	int children[2];
	int mins[3];
	int maxs[3];
};

/*
=================
CMod_LoadBrushes
=================
*/
#define MIN_NUM_SIDES 6

struct DiskLeafCollision
{
	int cluster;
	int area;
	int firstCollAabbIndex;
	int collAabbCount;
	int firstLeafBrush;
	int numLeafBrushes;
	int cellNum;
	int firstLightIndex;
	int numLights;
};

struct dplane_t
{
	vec3_t normal;
	float dist;
};

struct EdgeInfo
{
	float discriminant;
	vec3_t origin;
	vec3_t axis[3];
	float discNormalDist;
};

struct DiskCollBorder
{
	float distEq[3];
	float zBase;
	float zSlope;
	float start;
	float length;
};

struct DiskCollPartition
{
	uint16_t checkStamp;
	byte triCount;
	byte borderCount;
	int firstTriIndex;
	int firstBorderIndex;
};

struct DiskCollAabbTree
{
	vec3_t origin;
	vec3_t halfSize;
	int16_t materialIndex;
	int16_t childCount;
	union
	{
		int firstChildIndex;
		int partitionIndex;
	};
};

/*
===================
CMod_LoadVisibility
===================
*/

/*
=================
CM_LoadLumpFromFile
=================
*/
#define BSP_HEADER_SIZE 320

/*
===================
CMod_LoadMaterials
===================
*/
static void CMod_LoadMaterials( cmvis_lump_t *l )
{
	dmaterial_t *in;
	dmaterial_t *out;
	int matIndex;
	int count;
	int size;

	in = (dmaterial_t *)( cmod_base + l->fileofs );

	if ( l->filelen % sizeof( dmaterial_t ) )
		Com_Error( ERR_DROP, "\x15" "CMod_LoadMaterials: funny lump size" );

	count = (unsigned int)l->filelen / sizeof( dmaterial_t );

	if ( count <= 0 )
	{
		Com_Error(ERR_DROP, "\x15" "Map with no materials");
	}

	size = ( count + 1 ) * sizeof( *cm.materials );
	cm.materials = (dmaterial_t *)CM_Hunk_Alloc( size, "CMod_LoadMaterials", 21 ) + 1;
	cm.numMaterials = count;

	Com_Memcpy( cm.materials, in, count * sizeof(*cm.materials) );

	if ( LittleLong(1) != 1 )
	{
		out = cm.materials;

		for ( matIndex = 0; matIndex < count; matIndex++, in++, out++ )
		{
			out->contentFlags = LittleLong(out->contentFlags);
			out->surfaceFlags = LittleLong(out->surfaceFlags);
		}
	}
}

/*
=================
CMod_AllocLeafBrushNode
=================
*/
static cLeafBrushNode_s *CMod_AllocLeafBrushNode()
{
	cLeafBrushNode_s *node;

	node = (cLeafBrushNode_s *)TempMalloc(sizeof(cLeafBrushNode_s));
	memset(node, 0, sizeof(cLeafBrushNode_s));
	node->data.children.dist = -FLT_MAX;

	return node;
}

/*
=================
CMod_LoadSubmodels
=================
*/
static void CMod_LoadSubmodels( cmvis_lump_t *l )
{
	DiskBrushModel *in;
	cmodel_t *out;
	int bmodelIndex;
	int j;
	int count;
	int collAabbCount;
	int firstCollAabbIndex;
	int size;
	vec3_t extent;

	in = (DiskBrushModel *)( cmod_base + l->fileofs );

	if ( l->filelen % sizeof( *in ) )
	{
		Com_Error(ERR_DROP, "\x15" "CMod_LoadSubmodels: funny lump size");
	}

	count = l->filelen / sizeof( *in );

	if ( count <= 0 )
	{
		Com_Error(ERR_DROP, "\x15" "Map with no models");
	}

	size = count * sizeof( *cm.cmodels );
	cm.cmodels = (cmodel_t *)CM_Hunk_Alloc( size, "CMod_LoadSubmodels", 22 );
	cm.numSubModels = count;

	if ( count > MAX_SUBMODELS - 1 )
	{
		Com_Error(ERR_DROP, "\x15" "MAX_SUBMODELS exceeded");
	}

	for ( bmodelIndex = 0; bmodelIndex < count; bmodelIndex++, in++ )
	{
		out = &cm.cmodels[bmodelIndex];

		for ( j = 0; j < 3; j++ )
		{
			// spread the mins / maxs by a pixel
			out->mins[j] = FloatFromBits( FloatAsInt( in->mins[j] ) ) - 1;
			out->maxs[j] = FloatFromBits( FloatAsInt( in->maxs[j] ) ) + 1;

			extent[j] = I_fmax(I_fabs(out->mins[j]), I_fabs(out->maxs[j]));
		}

		out->radius = VectorLength(extent);

		if ( bmodelIndex == 0 )
		{
			continue;   // world model doesn't need other info
		}

		collAabbCount = LittleLong( in->numSurfaces );
		out->leaf.collAabbCount = collAabbCount;

		if ( out->leaf.collAabbCount != collAabbCount )
		{
			Com_Error(ERR_DROP, "\x15" "CMod_LoadSubmodels: collAabbCount exceeded");
		}

		firstCollAabbIndex = LittleLong( in->firstSurface );
		out->leaf.firstCollAabbIndex = firstCollAabbIndex;

		if ( out->leaf.firstCollAabbIndex != firstCollAabbIndex )
		{
			Com_Error(ERR_DROP, "\x15" "CMod_LoadSubmodels: firstCollAabbIndex exceeded");
		}
	}
}

/*
=================
CMod_AllocLeafBrushNode
=================
*/
static float CMod_GetPartitionScore(uint16_t *leafBrushes, int numLeafBrushes, int axis, const vec3_t mins, const vec3_t maxs, float *dist)
{
	int rightBrushCount;
	int leftBrushCount;
	int k;
	int brushIndex;
	cbrush_t *b;
	int minCount;
	float min;
	float max;

	rightBrushCount = -1;
	leftBrushCount = -1;

	min = -FLT_MAX;
	max = FLT_MAX;

	for ( k = 0; k < numLeafBrushes; k++ )
	{
		brushIndex = leafBrushes[k];
		b = &cm.brushes[brushIndex];

		if ( b->mins[axis] >= *dist )
		{
			rightBrushCount++;

			if ( b->mins[axis] < max )
			{
				max = b->mins[axis];
			}
		}
		else
		{
			if ( b->maxs[axis] <= *dist )
			{
				leftBrushCount++;

				if ( b->maxs[axis] > min )
				{
					min = b->maxs[axis];
				}
			}
		}
	}

	minCount = I_min(leftBrushCount, rightBrushCount);

	*dist = (min + max) * 0.5f;

	return minCount > 0 ? I_fmin(max - mins[axis], maxs[axis] - min) * minCount : 0;
}

/*
=================
CMod_PartionLeafBrushes_r
=================
*/
cLeafBrushNode_s* CMod_PartionLeafBrushes_r(uint16_t *leafBrushes, int numLeafBrushes, const vec3_t mins, const vec3_t maxs)
{
	unsigned char i;
	int j;
	int brushnum;
	cbrush_t *b;
	float score;
	float testDist;
	float dist;
	float bestScore;
	int axis;
	cLeafBrushNode_s *node;
	cLeafBrushNode_s *returnNode;
	cLeafBrushNode_s *childNode;
	int len;
	uint16_t *leafBrushesCopy;
	int numLeafBrushesChild;
	int side;
	vec3_t childMins;
	vec3_t childMaxs;
	int nodeOffset;
	float range;

	node = CMod_AllocLeafBrushNode();

	bestScore = 0.0f;
	axis = -1;
	dist = 0.0f;

	for ( i = 0; i <= 2; i++ )
	{
		for ( j = 0; j < numLeafBrushes; j++ )
		{
			brushnum = leafBrushes[j];
			b = &cm.brushes[brushnum];
			testDist = b->mins[i];

			score = CMod_GetPartitionScore(leafBrushes, numLeafBrushes, i, mins, maxs, &testDist);

			if ( score > bestScore )
			{
				bestScore = score;

				axis = i;
				dist = testDist;
			}

			testDist = b->maxs[i];

			score = CMod_GetPartitionScore(leafBrushes, numLeafBrushes, i, mins, maxs, &testDist);

			if ( score > bestScore )
			{
				bestScore = score;

				axis = i;
				dist = testDist;
			}
		}
	}

	if ( axis < 0 )
	{
		node->leafBrushCount = numLeafBrushes;

		if ( node->leafBrushCount != numLeafBrushes )
		{
			Com_Error(ERR_DROP, "\x15" "CMod_PartionLeafBrushes_r: leafBrushCount exceeded");
		}

		for ( j = 0; j < numLeafBrushes; j++ )
		{
			brushnum = leafBrushes[j];
			b = &cm.brushes[brushnum];
			node->contents |= b->contents;
		}

		node->data.leaf.brushes = leafBrushes;

		return node;
	}

	len = sizeof(*leafBrushes) * numLeafBrushes;
	leafBrushesCopy = (uint16_t *)CM_Hunk_AllocateTempMemoryHigh( len, "CMod_PartionLeafBrushes_r" );
	memcpy(leafBrushesCopy, leafBrushes, len);
	numLeafBrushesChild = 0;

	for ( j = 0; j < numLeafBrushes; j++ )
	{
		brushnum = leafBrushesCopy[j];
		b = &cm.brushes[brushnum];

		if ( b->mins[axis] >= dist )
		{
			continue;
		}

		if ( b->maxs[axis] <= dist )
		{
			continue;
		}

		leafBrushes[numLeafBrushesChild] = brushnum;
		numLeafBrushesChild++;
	}

	if ( numLeafBrushesChild )
	{
		returnNode = CMod_PartionLeafBrushes_r(leafBrushes, numLeafBrushesChild, mins, maxs);
		node->leafBrushCount = -1;
		node->contents = returnNode->contents;
		leafBrushes += numLeafBrushesChild;
	}

	range = FLT_MAX;

	node->axis = axis;
	node->data.children.dist = dist;

	side = 0;

	while ( side <= 1 )
	{
		numLeafBrushesChild = 0;

		for ( j = 0; j < numLeafBrushes; j++ )
		{
			brushnum = leafBrushesCopy[j];
			b = &cm.brushes[brushnum];

			if ( !side )
			{
				if ( b->mins[axis] < dist )
				{
					continue;
				}

				range = I_fmin(range, b->mins[axis] - dist);
			}
			else
			{
				if ( b->maxs[axis] > dist )
				{
					continue;
				}

				range = I_fmin(range, dist - b->maxs[axis]);
			}

			leafBrushes[numLeafBrushesChild] = brushnum;
			numLeafBrushesChild++;
		}

		VectorCopy(mins, childMins);
		VectorCopy(maxs, childMaxs);

		if ( !side )
		{
			childMins[axis] = dist + range;
		}
		else
		{
			childMaxs[axis] = dist - range;
		}

		childNode = CMod_PartionLeafBrushes_r(leafBrushes, numLeafBrushesChild, childMins, childMaxs);
		nodeOffset = childNode - node;
		node->data.children.childOffset[side] = nodeOffset;

		if ( node->data.children.childOffset[side] != nodeOffset )
		{
			Com_Error(ERR_DROP, "\x15" "CMod_PartionLeafBrushes_r: child exceeded");
		}

		node->contents |= childNode->contents;
		leafBrushes += numLeafBrushesChild;
		side++;
	}

	node->data.children.range = range;

	return node;
}

/*
=================
CMod_PartionLeafBrushes
=================
*/
static void CMod_PartionLeafBrushes(uint16_t *leafBrushes, int numLeafBrushes, cLeaf_s *leaf)
{
	vec3_t mins;
	vec3_t maxs;
	int i;
	int brushnum;
	cbrush_t *b;
	int j;

	if ( !numLeafBrushes )
	{
		return;
	}

	VectorSet(mins, FLT_MAX, FLT_MAX, FLT_MAX);
	VectorSet(maxs, -FLT_MAX, -FLT_MAX, -FLT_MAX);

	for ( i = 0; i < numLeafBrushes; i++ )
	{
		brushnum = leafBrushes[i];
		b = &cm.brushes[brushnum];

		for ( j = 0; j < 3; j++ )
		{
			if ( mins[j] > b->mins[j] )
			{
				mins[j] = b->mins[j];
			}

			if ( maxs[j] < b->maxs[j] )
			{
				maxs[j] = b->maxs[j];
			}
		}
	}

	VectorCopy(mins, leaf->mins);
	VectorCopy(maxs, leaf->maxs);

	for ( j = 0; j < 3; j++ )
	{
		leaf->mins[j] = leaf->mins[j] - (float)SURFACE_CLIP_EPSILON;
		leaf->maxs[j] = leaf->maxs[j] + (float)SURFACE_CLIP_EPSILON;
	}

	CM_Hunk_CheckTempMemoryHighClear();
	leaf->leafBrushNode = CMod_PartionLeafBrushes_r(leafBrushes, numLeafBrushes, mins, maxs) - cm.leafbrushNodes;
	CM_Hunk_ClearTempMemoryHigh();
}

/*
=================
CMod_GetLeafTerrainContents
=================
*/
static int CMod_GetLeafTerrainContents(cLeaf_s *leaf)
{
	int contents = 0;

	for ( int k = 0; k < leaf->collAabbCount; k++ )
	{
		contents |= cm.materials[cm.aabbTrees[k + leaf->firstCollAabbIndex].materialIndex].contentFlags;
	}

	return contents;
}

/*
=================
CMod_LoadSubmodelBrushNodes
=================
*/
static void CMod_LoadSubmodelBrushNodes( cmvis_lump_t *l )
{
	DiskBrushModel *in;
	cmodel_t *out;
	int bmodelIndex;
	int leafBrushIndex;
	uint16_t *indexes;
	int numLeafBrushes;
	int brushIndex;
	int contents;

	in = (DiskBrushModel *)( cmod_base + l->fileofs );

	for ( bmodelIndex = 0; bmodelIndex < cm.numSubModels; bmodelIndex++, in++ )
	{
		if ( !bmodelIndex )
		{
			continue;
		}

		out = &cm.cmodels[bmodelIndex];
		numLeafBrushes = LittleLong(in->numBrushes);

		indexes = (uint16_t *)CM_Hunk_Alloc( numLeafBrushes * sizeof(uint16_t), "CMod_LoadSubmodelBrushNodes", 22 );
		contents = 0;

		for ( leafBrushIndex = 0; leafBrushIndex < numLeafBrushes; leafBrushIndex++ )
		{
			brushIndex = LittleLong(in->firstBrush) + leafBrushIndex;
			indexes[leafBrushIndex] = brushIndex;

			if ( indexes[leafBrushIndex] != brushIndex )
			{
				Com_Error(ERR_DROP, "\x15" "CMod_LoadSubmodelBrushNodes: leafBrushes exceeded");
			}

			contents |= cm.brushes[brushIndex].contents;
		}

		out->leaf.brushContents = contents;
		out->leaf.terrainContents = CMod_GetLeafTerrainContents(&out->leaf);

		CMod_PartionLeafBrushes(indexes, numLeafBrushes, &out->leaf);
	}
}

/*
===================
CMod_LoadNodes
===================
*/
static void CMod_LoadNodes( cmvis_lump_t *l )
{
	dnode_t *in;
	int child;
	cNode_t *out;
	int nodeIter;
	int j;
	int count;
	int size;

	in = (dnode_t *)( cmod_base + l->fileofs );

	if ( l->filelen % sizeof( dnode_t ) )
		Com_Error( ERR_DROP, "\x15" "MOD_LoadBmodel: funny lump size" );

	count = (unsigned int)l->filelen / sizeof( dnode_t );

	if ( count <= 0 )
	{
		Com_Error(ERR_DROP, "\x15" "Map has no nodes");
	}

	size = count * sizeof( *cm.nodes );
	cm.nodes = (cNode_t *)CM_Hunk_Alloc( size, "CMod_LoadNodes", 21 );
	cm.numNodes = count;

	out = cm.nodes;

	for ( nodeIter = 0; nodeIter < count; nodeIter++, out++, in++ )
	{
		out->plane = &cme.planes[LittleLong(in->planeNum)];

		for ( j = 0; j < 2; j++ )
		{
			child = LittleLong(in->children[j]);
			out->children[j] = child;

			if ( out->children[j] != child )
			{
				Com_Error(ERR_DROP, "\x15" "CMod_LoadNodes: children exceeded");
			}
		}
	}
}

static void CMod_LoadBrushes( cmvis_lump_t *lBrushes, cmvis_lump_t *lSides )
{
	dbrush_t *inBrush;
	dbrushside_t *inSides;
	cbrush_t *outBrush;
	cbrushside_t *sides;
	int brushIter;
	int axisIter;
	int brushcount;
	int outnumsides;
	int materialNum;
	int planeNum;
	int index;
	float sign;
	int numBrushes;
	int size;
	int brushesSize;

	inBrush = (dbrush_t *)( cmod_base + lBrushes->fileofs );

	if ( lBrushes->filelen % sizeof( *inBrush ) )
	{
		Com_Error(ERR_DROP, "\x15" "CMod_LoadBrushes: funny lump size");
	}

	brushcount = lBrushes->filelen / sizeof( *inBrush );

	inSides = (dbrushside_t *)( cmod_base + lSides->fileofs );

	if ( lSides->filelen % sizeof( *inSides ) )
	{
		Com_Error(ERR_DROP, "\x15" "CMod_LoadBrushes: funny lump size");
	}

	outnumsides = ( lSides->filelen / sizeof( *inSides ) ) - MIN_NUM_SIDES * brushcount;

	if ( outnumsides < 0 )
	{
		Com_Error(ERR_DROP, "\x15" "CMod_LoadBrushes: bad side count");
	}

	size = sizeof( *cm.brushsides ) * outnumsides;

	cm.brushsides = (cbrushside_t *)( outnumsides ? CM_Hunk_Alloc( size, "CMod_LoadBrushSides", 22 ) : NULL );
	cm.numBrushSides = outnumsides;

	sides = cm.brushsides;

	numBrushes = BOX_BRUSHES + brushcount;
	brushesSize = numBrushes * sizeof( *cm.brushes );
	cm.brushes = (cbrush_t *)CM_Hunk_Alloc( brushesSize, "CMod_LoadBrushes", 22 );
	cm.numBrushes = brushcount;

	if ( cm.numBrushes != brushcount )
	{
		Com_Error(ERR_DROP, "\x15" "CMod_LoadBrushes: cm.numBrushes exceeded");
	}

	outBrush = cm.brushes;

	for ( brushIter = 0; brushIter < brushcount; brushIter++, outBrush++, inBrush++ )
	{
		outBrush->numsides = LittleShort(inBrush->numSides) - MIN_NUM_SIDES;

		if ( outBrush->numsides < 0 )
		{
			Com_Error(ERR_DROP, "\x15" "CMod_LoadBrushes: brush has less than 6 sides");
		}

		outBrush->sides = (outBrush->numsides != 0 ? sides : NULL);

		for ( axisIter = 0; axisIter < 3; axisIter++ )
		{
			sign = -1.0f;

			for( index = 0; index < 2; index++, inSides++, sign = 1.0f )
			{
				if ( !index )
				{
					outBrush->mins[axisIter] = FloatFromBits( FloatAsInt( inSides->bound ) );
				}
				else
				{
					outBrush->maxs[axisIter] = FloatFromBits( FloatAsInt( inSides->bound ) );
				}

				materialNum = LittleLong( inSides->materialNum );

				if ( materialNum < 0 || materialNum >= cm.numMaterials )
				{
					Com_Error(ERR_DROP, "\x15" "CMod_LoadBrushes: bad materialNum: %i", materialNum);
				}

				outBrush->axialMaterialNum[index][axisIter] = materialNum;

				if ( outBrush->axialMaterialNum[index][axisIter] != materialNum )
				{
					Com_Error(ERR_DROP, "\x15" "CMod_LoadBrushes: axialMaterialNum exceeded");
				}
			}
		}

		for ( axisIter = 0; axisIter < outBrush->numsides; axisIter++, inSides++, sides++ )
		{
			planeNum = LittleLong( inSides->planeNum );
			sides->plane = &cme.planes[planeNum];
			sides->materialNum = LittleLong( inSides->materialNum );

			if ( sides->materialNum < 0 || sides->materialNum >= cm.numMaterials )
			{
				Com_Error(ERR_DROP, "\x15" "CMod_LoadBrushes: bad materialNum: %i", sides->materialNum);
			}
		}

		materialNum = LittleShort(inBrush->materialNum);

		if ( materialNum < 0 || materialNum >= cm.numMaterials )
		{
			Com_Error(ERR_DROP, "\x15" "CMod_LoadBrushes: bad materialNum: %i", materialNum);
		}

		outBrush->contents = cm.materials[materialNum].contentFlags & ~(CONTENTS_TRANSLUCENT | CONTENTS_NONCOLLIDING);
	}
}

/*
=================
CMod_LoadLeafs
=================
*/
static void CMod_LoadLeafs( cmvis_lump_t *l, bool usePvs )
{
	int leafIter;
	cLeaf_s *out;
	DiskLeafCollision *in;
	int count;
	int cluster;
	int firstCollAabbIndex;
	int collAabbCount;
	int size;

	in = (DiskLeafCollision *)( cmod_base + l->fileofs );

	if ( l->filelen % sizeof( *in ) )
	{
		Com_Error(ERR_DROP, "\x15" "CMod_LoadLeafs: funny lump size");
	}

	count = l->filelen / sizeof( *in );

	if ( count <= 0 )
	{
		Com_Error(ERR_DROP, "\x15" "Map with no leafs");
	}

	size = count * sizeof( *cm.leafs );
	cm.leafs = (cLeaf_s *)CM_Hunk_Alloc( size, "CMod_LoadLeafs", 21 );
	cm.numLeafs = count;

	cluster = 0;
	out = cm.leafs;

	for ( leafIter = 0; leafIter < count; leafIter++, in++, out++ )
	{
		if ( usePvs )
		{
			cluster = LittleLong( in->cluster );
			out->cluster = cluster;

			if ( out->cluster != cluster )
			{
				Com_Error(ERR_DROP, "\x15" "CMod_LoadLeafs: cluster exceeded");
			}
		}

		firstCollAabbIndex = LittleLong( in->firstCollAabbIndex );
		out->firstCollAabbIndex = firstCollAabbIndex;

		if ( out->firstCollAabbIndex != firstCollAabbIndex )
		{
			Com_Error(ERR_DROP, "\x15" "CMod_LoadLeafs: firstCollAabbIndex exceeded");
		}

		collAabbCount = LittleLong( in->collAabbCount );
		out->collAabbCount = collAabbCount;

		if ( out->collAabbCount != collAabbCount )
		{
			Com_Error(ERR_DROP, "\x15" "CMod_LoadLeafs: collAabbCount exceeded");
		}

		if ( usePvs && cluster >= cm.numClusters )
		{
			cm.numClusters = cluster + 1;
		}
	}
}

/*
=================
CMod_LoadLeafBrushNodes
=================
*/
static void CMod_LoadLeafBrushNodes( cmvis_lump_t *l )
{
	int leafIter;
	cLeaf_s *out;
	DiskLeafCollision *in;
	int numLeafBrushes;
	int indexFirstLeafBrush;
	int contents;
	int brushIter;
	int brushnum;

	in = (DiskLeafCollision *)( cmod_base + l->fileofs );
	out = cm.leafs;

	for ( leafIter = 0; leafIter < cm.numLeafs; leafIter++, in++, out++ )
	{
		numLeafBrushes = LittleLong(in->numLeafBrushes);
		indexFirstLeafBrush = LittleLong(in->firstLeafBrush);
		contents = 0;

		for ( brushIter = 0; brushIter < numLeafBrushes; brushIter++ )
		{
			brushnum = cm.leafbrushes[indexFirstLeafBrush + brushIter];
			contents |= cm.brushes[brushnum].contents;
		}

		out->brushContents = contents;
		out->terrainContents = CMod_GetLeafTerrainContents(out);

		CMod_PartionLeafBrushes(&cm.leafbrushes[indexFirstLeafBrush], numLeafBrushes, out);
	}
}

/*
===================
CMod_LoadPlanes
===================
*/
static void CMod_LoadPlanes( const char *base, cmvis_lump_t *l )
{
	int planeIter;
	int axisIter;
	cplane_t *out;
	dplane_t *in;
	int count;
	int size;
	char bits;

	in = (dplane_t *)( base + l->fileofs );

	if ( l->filelen % sizeof( *in ) )
	{
		Com_Error(ERR_DROP, "\x15" "MOD_LoadBmodel: funny lump size");
	}

	count = l->filelen / sizeof( *in );

	if ( count <= 0 )
	{
		Com_Error(ERR_DROP, "\x15" "Map with no planes");
	}

	size = count * sizeof( *cme.planes );
	cme.planes = (cplane_t *)CM_Hunk_Alloc( size, "CMod_LoadPlanes", 21 );
	cme.planeCount = count;

	out = cme.planes;

	for ( planeIter = 0; planeIter < count; planeIter++, in++, out++ )
	{
		bits = 0;

		for ( axisIter = 0; axisIter < 3; axisIter++ )
		{
			out->normal[axisIter] = FloatFromBits( FloatAsInt( in->normal[axisIter] ) );

			if ( out->normal[axisIter] < 0 )
			{
				bits |= 1 << axisIter;
			}
		}

		out->dist = FloatFromBits( FloatAsInt( in->dist ) );
		out->type = PlaneTypeForNormal(out->normal);
		out->signbits = bits;
	}
}

/*
=================
CMod_LoadLeafBrushes
=================
*/
static void CMod_LoadLeafBrushes( cmvis_lump_t *l )
{
	int iter;
	uint16_t *out;
	uint32_t *in;
	int count;
	int size;
	int brushIndex;

	in = (uint32_t *)( cmod_base + l->fileofs );

	if ( l->filelen & 3 )
		Com_Error( ERR_DROP, "\x15" "CMod_LoadLeafBrushes: funny lump size" );

	count = (unsigned int)l->filelen / 4;

	size = ( count + 1 ) * sizeof( *cm.leafbrushes );
	cm.leafbrushes = (uint16_t *)CM_Hunk_Alloc( size, "CMod_LoadLeafBrushes", 22 );
	cm.numLeafBrushes = count;

	out = cm.leafbrushes;

	for ( iter = 0; iter < count; iter++, in++, out++ )
	{
		brushIndex = LittleLong(*in);
		*out = brushIndex;

		if ( *out != brushIndex )
		{
			Com_Error(ERR_DROP, "\x15" "CMod_LoadLeafBrushes: brushIndex exceeded");
		}
	}
}

/*

===================
CMod_LoadLeafSurfaces
===================
*/
static void CMod_LoadLeafSurfaces( cmvis_lump_t *l )
{
	int i;
	int *out;
	int *in;
	int count;
	int size;

	in = (int *)( cmod_base + l->fileofs );

	if ( l->filelen & 3 )
		Com_Error( ERR_DROP, "\x15" "CMod_LoadLeafSurfaces: funny lump size" );

	count = (unsigned int)l->filelen / 4;

	size = count * sizeof( *cm.leafsurfaces );
	cm.leafsurfaces = (int *)CM_Hunk_Alloc( size, "CMod_LoadLeafSurfaces", 24 );
	cm.numLeafSurfaces = count;

	out = cm.leafsurfaces;

	for ( i = 0 ; i < count ; i++, in++, out++ )
	{
		*out = LittleLong( *in );
	}
}

/*
===================
CMod_LoadCollisionVerts
===================
*/
static void CMod_LoadCollisionVerts( cmvis_lump_t *l )
{
	int vertIter;
	CollisionVert *out;
	vec4_t *in;
	int count;
	int size;

	in = (vec4_t *)( cmod_base + l->fileofs );

	if ( l->filelen % sizeof( vec4_t ) )
		Com_Error( ERR_DROP, "\x15" "CMod_LoadCollisionVerts: funny lump size" );

	count = (unsigned int)l->filelen / sizeof( vec4_t );

	size = count * sizeof( *cm.verts );
	cm.verts = (vec3_t *)CM_Hunk_Alloc( size, "CMod_LoadCollisionVerts", 24 );
	cm.vertCount = count;

	out = (CollisionVert *)cm.verts;

	for ( vertIter = 0; vertIter < count; vertIter++, in++, out++ )
	{
		out->x = FloatFromBits( FloatAsInt( in[0][1] ) );
		out->y = FloatFromBits( FloatAsInt( in[0][2] ) );
		out->z = FloatFromBits( FloatAsInt( in[0][3] ) );
	}
}

/*
===================
CMod_LoadCollisionEdges
===================
*/
static void CMod_LoadCollisionEdges( cmvis_lump_t *l )
{
	int edgeIter;
	CollisionEdge_s *out;
	EdgeInfo *in;
	int count;
	int size;
	float discNormalDist;
	float dist;

	in = (EdgeInfo *)( cmod_base + l->fileofs );

	if ( l->filelen % sizeof( *in ) )
	{
		Com_Error(ERR_DROP, "\x15" "CMod_LoadCollisionEdges: funny lump size");
	}

	count = l->filelen / sizeof( *in );

	size = count * sizeof( *cm.edges );
	cm.edges = (CollisionEdge_s *)CM_Hunk_Alloc( size, "CMod_LoadCollisionEdges", 24 );
	cm.edgeCount = count;

	out = cm.edges;

	for ( edgeIter = 0; edgeIter < count; edgeIter++, in++, out++ )
	{
		out->origin[0] = FloatFromBits( FloatAsInt( in->origin[0] ) );
		out->origin[1] = FloatFromBits( FloatAsInt( in->origin[1] ) );
		out->origin[2] = FloatFromBits( FloatAsInt( in->origin[2] ) );

		out->axis[0][0] = FloatFromBits( FloatAsInt( in->axis[0][0] ) );
		out->axis[0][1] = FloatFromBits( FloatAsInt( in->axis[0][1] ) );
		out->axis[0][2] = FloatFromBits( FloatAsInt( in->axis[0][2] ) );

		out->axis[1][0] = FloatFromBits( FloatAsInt( in->axis[1][0] ) );
		out->axis[1][1] = FloatFromBits( FloatAsInt( in->axis[1][1] ) );
		out->axis[1][2] = FloatFromBits( FloatAsInt( in->axis[1][2] ) );

		out->axis[2][0] = FloatFromBits( FloatAsInt( in->axis[2][0] ) );
		out->axis[2][1] = FloatFromBits( FloatAsInt( in->axis[2][1] ) );
		out->axis[2][2] = FloatFromBits( FloatAsInt( in->axis[2][2] ) );

		discNormalDist = FloatFromBits( FloatAsInt( in->discNormalDist ) );
		dist = 1.0f / discNormalDist;
		VectorScale( out->axis[2], dist, out->axis[2] );
	}
}

/*
===================
CMod_LoadCollisionTriangles
===================
*/
static void CMod_LoadCollisionTriangles( cmvis_lump_t *l )
{
	int triIter;
	CollisionTriangle_s *out;
	CollisionTriangle_s *in;
	int count;
	int j;
	int size;

	in = (CollisionTriangle_s *)( cmod_base + l->fileofs );

	if ( l->filelen % sizeof( *in ) )
	{
		Com_Error(ERR_DROP, "\x15" "CMod_LoadCollisionTriangles: funny lump size");
	}

	count = l->filelen / sizeof( *in );

	size = count * sizeof( *cm.tris );
	cm.tris = (CollisionTriangle_s *)CM_Hunk_Alloc( size, "CMod_LoadCollisionTriangles", 24 );
	cm.triCount = count;

	out = cm.tris;

	for ( triIter = 0; triIter < count; triIter++, in++, out++ )
	{
		out->plane[0] = FloatFromBits( FloatAsInt( in->plane[0] ) );
		out->plane[1] = FloatFromBits( FloatAsInt( in->plane[1] ) );
		out->plane[2] = FloatFromBits( FloatAsInt( in->plane[2] ) );
		out->plane[3] = FloatFromBits( FloatAsInt( in->plane[3] ) );

		out->svec[0] = FloatFromBits( FloatAsInt( in->svec[0] ) );
		out->svec[1] = FloatFromBits( FloatAsInt( in->svec[1] ) );
		out->svec[2] = FloatFromBits( FloatAsInt( in->svec[2] ) );
		out->svec[3] = FloatFromBits( FloatAsInt( in->svec[3] ) );

		out->tvec[0] = FloatFromBits( FloatAsInt( in->tvec[0] ) );
		out->tvec[1] = FloatFromBits( FloatAsInt( in->tvec[1] ) );
		out->tvec[2] = FloatFromBits( FloatAsInt( in->tvec[2] ) );
		out->tvec[3] = FloatFromBits( FloatAsInt( in->tvec[3] ) );

		for ( j = 0; j < 3; ++j )
		{
			out->edges[j] = LittleLong( in->edges[j] );
			out->verts[j] = LittleLong( in->verts[j] );
		}
	}
}

/*
===================
CMod_LoadCollisionBorders
===================
*/
static void CMod_LoadCollisionBorders( cmvis_lump_t *l )
{
	int index;
	CollisionBorder *out;
	DiskCollBorder *in;
	int count;
	int size;

	in = (DiskCollBorder *)( cmod_base + l->fileofs );

	if ( l->filelen % sizeof( DiskCollBorder ) )
		Com_Error( ERR_DROP, "\x15" "CMod_LoadCollisionBorders: funny lump size" );

	count = (unsigned int)l->filelen / sizeof( DiskCollBorder );

	size = count * sizeof( *cm.borders );
	cm.borders = (CollisionBorder *)CM_Hunk_Alloc( size, "CMod_LoadCollisionBorders", 24 );
	cm.borderCount = count;

	out = cm.borders;

	for ( index = 0; index < count; index++, in++, out++ )
	{
		out->distEq[0] = FloatFromBits( FloatAsInt( in->distEq[0] ) );
		out->distEq[1] = FloatFromBits( FloatAsInt( in->distEq[1] ) );
		out->distEq[2] = FloatFromBits( FloatAsInt( in->distEq[2] ) );

		out->zBase = FloatFromBits( FloatAsInt( in->zBase ) );
		out->zSlope = FloatFromBits( FloatAsInt( in->zSlope ) );
		out->start = FloatFromBits( FloatAsInt( in->start ) );
		out->length = FloatFromBits( FloatAsInt( in->length ) );
	}
}

/*
===================
CMod_LoadCollisionPartitions
===================
*/
static void CMod_LoadCollisionPartitions( cmvis_lump_t *l )
{
	int index;
	CollisionPartition *out;
	DiskCollPartition *in;
	int count;
	int size;

	in = (DiskCollPartition *)( cmod_base + l->fileofs );

	if ( l->filelen % sizeof( DiskCollPartition ) )
		Com_Error( ERR_DROP, "\x15" "CMod_LoadCollisionPartitions: funny lump size" );

	count = (unsigned int)l->filelen / sizeof( DiskCollPartition );

	size = count * sizeof( *cm.partitions );
	cm.partitions = (CollisionPartition *)CM_Hunk_Alloc( size, "CMod_LoadCollisionPartitions", 24 );
	cm.partitionCount = count;

	out = cm.partitions;

	for ( index = 0; index < count; index++, in++, out++ )
	{
		out->triCount = in->triCount;
		out->borderCount = in->borderCount;

		out->tris = &cm.tris[LittleLong(in->firstTriIndex)];
		out->borders = &cm.borders[LittleLong(in->firstBorderIndex)];
	}
}

/*
=================
CMod_LoadCollisionAabbTrees
=================
*/
static void CMod_LoadCollisionAabbTrees( cmvis_lump_t *l )
{
	int index;
	CollisionAabbTree_s *out;
	DiskCollAabbTree *in;
	int count;
	int size;

	in = (DiskCollAabbTree *)( cmod_base + l->fileofs );

	if ( l->filelen % sizeof( DiskCollAabbTree ) )
		Com_Error( ERR_DROP, "\x15" "CMod_LoadCollisionAabbTrees: funny lump size" );

	count = (unsigned int)l->filelen / sizeof( DiskCollAabbTree );

	size = count * sizeof( *cm.aabbTrees );
	cm.aabbTrees = (CollisionAabbTree_s *)CM_Hunk_Alloc( size, "CMod_LoadCollisionAabbTrees", 24 );
	cm.aabbTreeCount = count;

	out = cm.aabbTrees;

	for ( index = 0; index < count; index++, in++, out++ )
	{
		out->origin[0] = FloatFromBits( FloatAsInt( in->origin[0] ) );
		out->origin[1] = FloatFromBits( FloatAsInt( in->origin[1] ) );
		out->origin[2] = FloatFromBits( FloatAsInt( in->origin[2] ) );

		out->halfSize[0] = FloatFromBits( FloatAsInt( in->halfSize[0] ) );
		out->halfSize[1] = FloatFromBits( FloatAsInt( in->halfSize[1] ) );
		out->halfSize[2] = FloatFromBits( FloatAsInt( in->halfSize[2] ) );

		out->materialIndex = LittleShort(in->materialIndex);
		out->childCount = LittleShort(in->childCount);
		out->u.firstChildIndex = LittleLong(in->firstChildIndex);
	}
}

/*
===================
CMod_LoadEntityString
===================
*/
static void CMod_LoadEntityString( cmvis_lump_t *l )
{
	cm.numEntityChars = l->filelen;
	cm.entityString = (char *)CM_Hunk_Alloc(l->filelen, "CMod_LoadEntityString", 9);
	Com_Memcpy(cm.entityString, cmod_base + l->fileofs, l->filelen);
}

static void CMod_LoadVisibility( cmvis_lump_t *l )
{
	int len;
	byte    *buf;

	len = l->filelen;

	if ( !len )
	{
		cm.clusterBytes = ( cm.numClusters + 31 ) & ~31;
		cm.visibility = (byte *)CM_Hunk_Alloc( cm.clusterBytes, "CMod_LoadVisibility", 9 );
		Com_Memset( cm.visibility, 255, cm.clusterBytes );
		return;
	}

	buf = (byte *)( cmod_base + l->fileofs );
	cm.vised = qtrue;
	cm.numClusters = LittleLong( ( (int *)buf )[0] );
	cm.clusterBytes = LittleLong( ( (int *)buf )[1] );
	cm.visibility = (byte *)CM_Hunk_Alloc( len - VIS_HEADER, "CMod_LoadVisibility", 9 );
	Com_Memcpy( cm.visibility, buf + VIS_HEADER, len - VIS_HEADER );
}

/*
===================
CMod_LoadBrushRelated
===================
*/
static void CMod_LoadBrushRelated( cmvis_lump_t *lumps, bool usePvs )
{
	int leafbrushNodesCount;
	int size;
	cLeafBrushNode_s *leafbrushNodes;

	CMod_LoadBrushes(&lumps[7], &lumps[6]);
	CMod_LoadLeafBrushes(&lumps[28]);
	CMod_LoadCollisionAabbTrees(&lumps[35]);
	CMod_LoadLeafs(&lumps[27], usePvs);
	CMod_LoadSubmodels(&lumps[36]);

	CM_Hunk_CheckTempMemoryClear();
	TempMemoryReset();

	cm.leafbrushNodes = ((cLeafBrushNode_s*)TempMalloc(0) - 1);

	CMod_LoadLeafBrushNodes(&lumps[27]);
	CMod_LoadSubmodelBrushNodes(&lumps[36]);

	CM_InitBoxHull();

	cm.leafbrushNodes++;
	leafbrushNodesCount = ((cLeafBrushNode_s*)TempMalloc(0)) - cm.leafbrushNodes;
	size = leafbrushNodesCount * sizeof( cLeafBrushNode_s );
	cm.leafbrushNodesCount = leafbrushNodesCount + 1;

	leafbrushNodes = (cLeafBrushNode_s *)CM_Hunk_Alloc( cm.leafbrushNodesCount * sizeof( cLeafBrushNode_s ), "CMod_LoadBrushRelated", 22 );
	memcpy(&leafbrushNodes[1], cm.leafbrushNodes, size);

	cm.leafbrushNodes = leafbrushNodes;

	CM_Hunk_ClearTempMemory();
}

/*
===================
CM_LoadMapFromBsp
===================
*/
void CM_LoadMapFromBsp(const char *name, bool usePvs)
{
	dheader_t *header;

	// free old stuff (1.0 CM state is 0x110 bytes; the trailing plane block is separate)
	Com_Memset( &cm, 0, 0x110 );
	Com_Memset( &cme.planeCount, 0, 0xc );

	cm.name = (char *)CM_Hunk_Alloc(strlen(name) + 1, "CM_LoadMapFromBsp", 21);
	strcpy(cm.name, name);

	header = (dheader_t *)Com_GetBspHeader(0, (int *)&cm.checksum);
	cmod_base = (char *)header;

	// load into heap
	CMod_LoadMaterials(&((cmvis_lump_t *)header)[1]);
	CMod_LoadPlanes(cmod_base, &((cmvis_lump_t *)header)[5]);
	CMod_LoadBrushRelated((cmvis_lump_t *)header, usePvs);
	CMod_LoadNodes(&((cmvis_lump_t *)header)[26]);
	CMod_LoadLeafSurfaces(&((cmvis_lump_t *)header)[29]);
	CMod_LoadCollisionVerts(&((cmvis_lump_t *)header)[30]);
	CMod_LoadCollisionEdges(&((cmvis_lump_t *)header)[31]);
	CMod_LoadCollisionTriangles(&((cmvis_lump_t *)header)[32]);
	CMod_LoadCollisionBorders(&((cmvis_lump_t *)header)[33]);
	CMod_LoadCollisionPartitions(&((cmvis_lump_t *)header)[34]);

	if ( usePvs )
	{
		CMod_LoadVisibility(&((cmvis_lump_t *)header)[37]);
	}
	else if ( ((cm_bsp_lumps_t *)header)->lumps[37].filelen )
	{
		Com_Error(ERR_DROP, "In single player, do not compile the bsp with visibility");
	}

	CMod_LoadEntityString(&((cmvis_lump_t *)header)[38]);

	cmod_base = NULL;
}

/*
===================
CM_InitBoxHull

Set up the planes and nodes so that the six floats of a bounding box
can just be stored out and get a proper clipping hull structure.
===================
*/
static void CM_InitBoxHull( void )
{
	cLeafBrushNode_s *node;

	cm.box_brush = &cm.brushes[cm.numBrushes];

	cm.box_brush->numsides = 0;
	cm.box_brush->sides = 0;
	cm.box_brush->contents = -1;
	cm.box_model.leaf.brushContents = -1;
	cm.box_model.leaf.terrainContents = 0;

	VectorSet(cm.box_model.leaf.mins, FLT_MAX, FLT_MAX, FLT_MAX);
	VectorSet(cm.box_model.leaf.maxs, -FLT_MAX, -FLT_MAX, -FLT_MAX);

	cm.box_brush->axialMaterialNum[0][0] = -1;
	cm.box_brush->axialMaterialNum[0][1] = -1;
	cm.box_brush->axialMaterialNum[0][2] = -1;
	cm.box_brush->axialMaterialNum[1][0] = -1;
	cm.box_brush->axialMaterialNum[1][1] = -1;
	cm.box_brush->axialMaterialNum[1][2] = -1;

	node = CMod_AllocLeafBrushNode();
	cm.box_model.leaf.leafBrushNode = node - cm.leafbrushNodes;

	node->leafBrushCount = 1;
	node->data.leaf.brushes = &cm.leafbrushes[cm.numLeafBrushes];

	cm.leafbrushes[cm.numLeafBrushes] = cm.numBrushes;
}

/*
=================
CM_Cleanup
=================
*/
void CM_Cleanup( void )
{
	cmod_base = NULL;
}

/*
=================
CM_GetPlaneNum
=================
*/
cplane_t *CM_GetPlaneNum( int planeNum )
{
	return &cme.planes[planeNum];
}

int CM_LoadLumpFromFile( int lump, byte **data )
{
	int header[BSP_HEADER_SIZE / 4];
	unsigned int i;
	int h;
	int len;

	FS_FOpenFileRead(cm.name, &h, 0);

	if ( !h )
	{
		Com_Error(ERR_DROP, "EXE_ERR_COULDNT_LOAD\x15%s", cm.name);
	}

	FS_Read(header, BSP_HEADER_SIZE, h);

	for ( i = 0; i < BSP_HEADER_SIZE / 4; i++ )
	{
		*(header + i) = LittleLong(*(header + i));
	}

	len = header[2 * lump + 2];

	if ( !len )
	{
		FS_FCloseFile(h);
		return 0;
	}

	FS_SeekInternal(h, header[2 * lump + 3] - BSP_HEADER_SIZE, 0);
	cm_lumpData = (byte *)Hunk_AllocateTempMemory(len);
	FS_Read(cm_lumpData, len, h);
	FS_FCloseFile(h);
	*data = cm_lumpData;

	return len;
}

// unreferenced; original name unknown
void CM_FreeLumpData( void )
{
	Hunk_FreeTempMemory(cm_lumpData);
}


// Unreferenced storage; original declarations unknown (sized from the layout).
static int unusedStorage[2];
