#include "qcommon.h"
#include "cm_local.h"

int CM_BoxLeafnums( const vec3_t mins, const vec3_t maxs, int *list, int listsize, int *lastLeaf );

static int CM_PointLeafnum_r( const vec3_t p, int num );
void CM_StoreLeafs( leafList_t *ll, int nodenum );
int CM_BoxLeafnums( const vec3_t mins, const vec3_t maxs, int *list, int listsize, int *lastLeaf );
static int CM_PointContentsLeafBrushNode_r( const vec3_t p, cLeafBrushNode_s *node );
int CM_PointContents( const vec3_t p, clipHandle_t model );
int CM_TransformedPointContents( const vec3_t p, clipHandle_t model, const vec3_t origin, const vec3_t angles );
byte *CM_ClusterPVS( int cluster );

/*
======================================================================
LEAF LISTING
======================================================================
*/

/*
===============================================================================
PVS
===============================================================================
*/

/*
==================
CM_PointLeafnum_r
==================
*/
static int CM_PointLeafnum_r( const vec3_t p, int num )
{
	float d;
	cNode_t     *node;
	cplane_t    *plane;

	while ( num >= 0 )
	{
		node = &cm.nodes[num];
		plane = node->plane;

		if ( plane->type < 3 )
		{
			d = p[plane->type] - plane->dist;
		}
		else
		{
			d = DotProduct( plane->normal, p ) - plane->dist;
		}
		if ( d < 0 )
		{
			num = node->children[1];
		}
		else
		{
			num = node->children[0];
		}
	}

	return -1 - num;
}

int CM_PointLeafnum( const vec3_t p )
{
	return CM_PointLeafnum_r( p, 0 );
}

/*
==================
CM_StoreLeafs
==================
*/
void CM_StoreLeafs( leafList_t *ll, int nodenum )
{
	int leafNum;

	leafNum = -1 - nodenum;

	// store the lastLeaf even if the list is overflowed
	if ( cm.leafs[ leafNum ].cluster != -1 )
	{
		ll->lastLeaf = leafNum;
	}

	if ( ll->count >= ll->maxcount )
	{
		ll->overflowed = qtrue;
		return;
	}

	ll->list[ ll->count++ ] = leafNum;
}

/*
=============
CM_BoxLeafnums
Fills in a list of all the leafs touched
=============
*/
void CM_BoxLeafnums_r( leafList_t *ll, int nodenum )
{
	cplane_t    *plane;
	cNode_t     *node;
	int s;

	while ( 1 )
	{
		if ( nodenum < 0 )
		{
			CM_StoreLeafs( ll, nodenum );
			return;
		}

		node = &cm.nodes[nodenum];
		plane = node->plane;
		s = BoxOnPlaneSide( ll->bounds[0], ll->bounds[1], plane );

		if ( s == 1 )
		{
			nodenum = node->children[0];
		}
		else if ( s == 2 )
		{
			nodenum = node->children[1];
		}
		else
		{
			CM_BoxLeafnums_r(ll, node->children[0]);
			nodenum = node->children[1];
		}
	}
}

/*
==================
CM_BoxLeafnums
==================
*/
int CM_BoxLeafnums( const vec3_t mins, const vec3_t maxs, int *list, int listsize, int *lastLeaf )
{
	leafList_t ll;

	VectorCopy( mins, ll.bounds[0] );
	VectorCopy( maxs, ll.bounds[1] );
	ll.count = 0;
	ll.maxcount = listsize;
	ll.list = list;
	ll.lastLeaf = 0;
	ll.overflowed = qfalse;

	CM_BoxLeafnums_r( &ll, 0 );

	*lastLeaf = ll.lastLeaf;

	return ll.count;
}

/*
==================
CM_PointContentsLeafBrushNode_r
==================
*/
static int CM_PointContentsLeafBrushNode_r( const vec3_t p, cLeafBrushNode_s *node )
{
	int leafIndex;
	int brushIndex;
	cbrush_t *b;
	int i;
	cbrushside_t *side;
	int contents;

	contents = 0;

	while ( 1 )
	{
		if ( node->leafBrushCount )
		{
			if ( node->leafBrushCount > 0 )
			{
				for ( leafIndex = 0; leafIndex < node->leafBrushCount; leafIndex++ )
				{
					brushIndex = node->data.leaf.brushes[leafIndex];
					b = &cm.brushes[brushIndex];

					for ( i = 0; i < 3; ++i )
					{
						if ( p[i] < b->mins[i] || p[i] > b->maxs[i] )
						{
							goto miss;
						}
					}

					side = b->sides;

					for ( i = b->numsides; i; i--, side++ )
					{
						if ( DotProduct(p, side->plane->normal) > side->plane->dist )
						{
							goto miss;
						}
					}

					contents |= b->contents;
miss:
					;
				}

				return contents;
			}

			contents |= CM_PointContentsLeafBrushNode_r(p, node + 1);
		}
appendnode:
		node += node->data.children.childOffset[p[node->axis] <= node->data.children.dist];
	}
}

/*
==================
CM_PointContents
==================
*/
int CM_PointContents( const vec3_t p, clipHandle_t model )
{
	int leafnum;
	cLeaf_s *leaf;
	cmodel_t *cmod;
	int i;

	assert(cm.numNodes);

	if ( model )
	{
		cmod = CM_ClipHandleToModel(model);
		leaf = &cmod->leaf;
	}
	else
	{
		leafnum = CM_PointLeafnum_r(p, 0);
		leaf = &cm.leafs[leafnum];
	}

	if ( !leaf->leafBrushNode )
		return 0;

	for ( i = 0; i < 3; ++i )
	{
		if ( p[i] <= leaf->mins[i] )
			return 0;

		if ( p[i] >= leaf->maxs[i] )
			return 0;
	}

	return CM_PointContentsLeafBrushNode_r(p, &cm.leafbrushNodes[leaf->leafBrushNode]);
}

/*
==================
CM_TransformedPointContents
Handles offseting and rotation of the end points for moving and
rotating entities
==================
*/
int CM_TransformedPointContents( const vec3_t p, clipHandle_t model, const vec3_t origin, const vec3_t angles )
{
	vec3_t p_l;
	vec3_t temp;
	vec3_t forward, right, up;

	// subtract origin offset
	VectorSubtract( p, origin, p_l );

	// rotate start and end into the models frame of reference
	if ( ( angles[0] || angles[1] || angles[2] ) )
	{
		AngleVectors( angles, forward, right, up );

		VectorCopy( p_l, temp );
		p_l[0] = DotProduct( temp, forward );
		p_l[1] = -DotProduct( temp, right );
		p_l[2] = DotProduct( temp, up );
	}

	return CM_PointContents( p_l, model );
}

/*
==================
CM_ClusterPVS
==================
*/
byte *CM_ClusterPVS( int cluster )
{
	if ( cluster < 0 || cluster >= cm.numClusters || !cm.vised )
	{
		return cm.visibility;
	}

	return cm.visibility + cluster * cm.clusterBytes;
}

