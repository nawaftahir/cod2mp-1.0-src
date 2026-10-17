#include "qcommon.h"
#include "cm_local.h"

/*
==================
CM_PositionTest
==================
*/
#define MAX_POSITION_LEAFS  1024

void CM_GetTrackThreadInfo( TraceThreadInfo *threadInfo );
void CM_GetBox( cbrush_t **box_brush, cmodel_t **box_model );
clipHandle_t CM_TempBoxModel( const vec3_t mins, const vec3_t maxs, int contents );
cmodel_t *CM_ClipHandleToModel( clipHandle_t handle );
int CM_ContentsOfModel( clipHandle_t handle );
float CM_RadiusOfModel( clipHandle_t handle );
void RotatePoint( vec3_t point, const vec3_t matrix[3] );
void TransposeMatrix( const vec3_t matrix[3], vec3_t transpose[3] );
void CreateRotationMatrix( const vec3_t angles, vec3_t matrix[3] );
void CM_TestBoxInBrush( traceWork_t *tw, cbrush_t *brush, trace_t *trace );
qboolean CM_TestInLeafBrushNode( traceWork_t *tw, cLeaf_t *leaf, trace_t *trace );
void CM_TestInLeaf( traceWork_t *tw, cLeaf_t *leaf, trace_t *trace );
void CM_TestCapsuleInCapsule( traceWork_t *tw, trace_t *trace );
void CM_PositionTest( traceWork_t *tw, trace_t *trace );
void CM_TraceThroughBrush( traceWork_t *tw, cbrush_t *brush, trace_t *trace );
static void CM_TraceThroughLeafBrushNode_r( traceWork_t *tw, cLeafBrushNode_t *node, const vec4_t p1_, const vec4_t p2, trace_t *trace );
bool CM_TraceThroughLeafBrushNode( traceWork_t *tw, cLeaf_t *leaf, trace_t *trace );
void CM_TraceThroughLeaf( traceWork_t *tw, cLeaf_t *leaf, trace_t *trace );
qboolean CM_TraceCylinderThroughCylinder( traceWork_t *tw, const vec3_t vStationary, float fStationaryHalfHeight, float radius, trace_t *trace );
void CM_TraceCapsuleThroughCapsule( traceWork_t *tw, trace_t *trace );
void CM_TraceThroughTree( traceWork_t *tw, int num, const vec4_t p1_, const vec4_t p2, trace_t *trace );
void CM_SetAxialCullOnly( traceWork_t *tw );
int CM_SightTraceThroughBrush( traceWork_t *tw, cbrush_t *brush );
static int CM_SightTraceThroughLeafBrushNode_r( traceWork_t *tw, cLeafBrushNode_t *remoteNode, const vec3_t p1_, const vec3_t p2 );
int CM_SightTraceThroughLeafBrushNode( traceWork_t *tw, cLeaf_t *leaf );
int CM_SightTraceThroughLeaf( traceWork_t *tw, cLeaf_t *leaf, trace_t *trace );
qboolean CM_SightTraceCylinderThroughCylinder( traceWork_t *tw, const vec3_t vStationary, float fStationaryHalfHeight, float radius, trace_t *trace );
int CM_SightTraceCapsuleThroughCapsule( traceWork_t *tw, trace_t *trace );
int CM_SightTraceThroughTree( traceWork_t *tw, int num, const vec3_t p1_, const vec3_t p2, trace_t *trace );
int CM_BoxSightTrace( int oldHitNum, const vec3_t start, const vec3_t end, const vec3_t mins, const vec3_t maxs, clipHandle_t model, int brushmask );

/*
==================
CM_GetTrackThreadInfo
==================
*/
void CM_GetTrackThreadInfo( TraceThreadInfo *threadInfo )
{
	TraceThreadInfo *value;

	value = (TraceThreadInfo *)Sys_GetValue(THREAD_VALUE_TRACE);
	value->checkcount++;

	*threadInfo = *value;
}

/*
==================
CM_GetBox
==================
*/
void CM_GetBox( cbrush_t **box_brush, cmodel_t **box_model )
{
	TraceThreadInfo *value;

	value = (TraceThreadInfo *)Sys_GetValue(THREAD_VALUE_TRACE);

	*box_brush = value->box_brush;
	*box_model = value->box_model;
}

/*
===================
CM_TempBoxModel

To keep everything totally uniform, bounding boxes are turned into small
BSP trees instead of being compared directly.
Capsules are handled differently though.
===================
*/
clipHandle_t CM_TempBoxModel( const vec3_t mins, const vec3_t maxs, int contents )
{
	cbrush_t *box_brush;
	cmodel_t *box_model;

	CM_GetBox(&box_brush, &box_model);

	VectorCopy(mins, box_model->mins);
	VectorCopy(maxs, box_model->maxs);

	VectorCopy(mins, box_brush->mins);
	VectorCopy(maxs, box_brush->maxs);

// DHM - Nerve
	box_brush->contents = contents;
// dhm

	return BOX_MODEL_HANDLE;
}

/*
==================
CM_ClipHandleToModel
==================
*/
cmodel_t *CM_ClipHandleToModel( clipHandle_t handle )
{
	cbrush_t *box_brush;
	cmodel_t *box_model;

	if ( handle < cm.numSubModels )
	{
		return &cm.cmodels[handle];
	}

	CM_GetBox(&box_brush, &box_model);

	return box_model;
}

/*
==================
CM_ContentsOfModel
==================
*/
int CM_ContentsOfModel( clipHandle_t handle )
{
	cmodel_t *cmod = CM_ClipHandleToModel(handle);

	return cmod->leaf.brushContents | cmod->leaf.terrainContents;
}

/*
==================
CM_RadiusOfModel
==================
*/
float CM_RadiusOfModel( clipHandle_t handle )
{
	cmodel_t *model;

	model = CM_ClipHandleToModel(handle);

	return model->radius;
}

/*
================
RotatePoint
================
*/
void RotatePoint( vec3_t point, const vec3_t matrix[3] )
{
	vec3_t tvec;

	VectorCopy( point, tvec );
	point[0] = DotProduct( matrix[0], tvec );
	point[1] = DotProduct( matrix[1], tvec );
	point[2] = DotProduct( matrix[2], tvec );
}

/*
================
TransposeMatrix
================
*/
void TransposeMatrix( const vec3_t matrix[3], vec3_t transpose[3] )
{
	int i, j;

	for ( i = 0; i < 3; i++ )
	{
		for ( j = 0; j < 3; j++ )
		{
			transpose[i][j] = matrix[j][i];
		}
	}
}

/*
================
CreateRotationMatrix
================
*/
void CreateRotationMatrix( const vec3_t angles, vec3_t matrix[3] )
{
	AngleVectors( angles, matrix[0], matrix[1], matrix[2] );
	VectorInverse( matrix[1] );
}

/*
================
CM_TestBoxInBrush
================
*/
void CM_TestBoxInBrush( traceWork_t *tw, cbrush_t *brush, trace_t *trace )
{
	int i;
	cplane_t *plane;
	float dist;
	float d1;
	cbrushside_t    *side;
	float offset;

	assert(!IS_NAN((tw->extents.start)[0]) && !IS_NAN((tw->extents.start)[1]) && !IS_NAN((tw->extents.start)[2]));
	assert(!IS_NAN((tw->extents.end)[0]) && !IS_NAN((tw->extents.end)[1]) && !IS_NAN((tw->extents.end)[2]));

	// special test for axial
	// the first 6 brush planes are always axial
	if ( 	   tw->bounds[0][0] > brush->maxs[0]
	           || tw->bounds[0][1] > brush->maxs[1]
	           || tw->bounds[0][2] > brush->maxs[2]
	           || tw->bounds[1][0] < brush->mins[0]
	           || tw->bounds[1][1] < brush->mins[1]
	           || tw->bounds[1][2] < brush->mins[2]
	   )
	{
		return;
	}

	// the first six planes are the axial planes, so we only
	// need to test the remainder
	side = brush->sides;
	assert(brush->numsides >= 0);
	for ( i = brush->numsides; i; i--, side++ )
	{
		assert(!IS_NAN(side->plane->dist));
		assert(!IS_NAN(tw->radius));
		assert(!IS_NAN((side->plane->normal)[0]) && !IS_NAN((side->plane->normal)[1]) && !IS_NAN((side->plane->normal)[2]));
		assert(!IS_NAN(tw->offsetZ));

		plane = side->plane;

		// adjust the plane distance apropriately for mins/maxs
		offset = I_fabs(plane->normal[2] * tw->offsetZ);
		dist = plane->normal[3] + tw->radius + offset;
		assert(!IS_NAN(dist));

		d1 = DotProduct(tw->extents.start, plane->normal) - dist;
		assert(!IS_NAN(d1));

		// if completely in front of face, no intersection
		if ( d1 > 0 )
		{
			return;
		}
	}

	// inside this brush
	trace->startsolid = trace->allsolid = qtrue;
	trace->fraction = 0;
	trace->contents = brush->contents;
}

/*
================
CM_SightTraceThroughLeafBrushNode
================
*/
static void CM_TestInLeafBrushNode_r( traceWork_t *tw, cLeafBrushNode_t *node, trace_t *trace )
{
	int k;
	int brushIndex;
	cbrush_t *b;

	assert(node);

loop:
	if ( !(node->contents & tw->contents) )
	{
		return;
	}

	if ( node->leafBrushCount )
	{
		if ( node->leafBrushCount > 0 )
		{
			// test box position against all brushes in the leaf
			for ( k = 0; k < node->leafBrushCount; k++ )
			{
				brushIndex = node->data.leaf.brushes[k];
				b = &cm.brushes[brushIndex];

				if ( !(b->contents & tw->contents) )
				{
					continue;
				}

				CM_TestBoxInBrush(tw, b, trace);

				if ( trace->allsolid )
				{
					break;
				}
			}

			return;
		}

		CM_TestInLeafBrushNode_r(tw, node + 1, trace);

		if ( trace->allsolid )
		{
			return;
		}
	}

	if ( tw->bounds[0][node->axis] > node->data.children.dist )
	{
		node += node->data.children.childOffset[0];
		goto loop;
	}

	if ( tw->bounds[1][node->axis] >= node->data.children.dist )
	{
		CM_TestInLeafBrushNode_r(tw, &node[node->data.children.childOffset[0]], trace);

		if ( trace->allsolid )
		{
			return;
		}
	}

	node += node->data.children.childOffset[1];
	goto loop;
}

/*
================
CM_TestInLeafBrushNode
================
*/
qboolean CM_TestInLeafBrushNode( traceWork_t *tw, cLeaf_t *leaf, trace_t *trace )
{
	int i;

	assert(leaf->leafBrushNode);

	for ( i = 0; i < 3; i++ )
	{
		if ( tw->bounds[1][i] <= leaf->mins[i] )
		{
			return qfalse;
		}

		if ( tw->bounds[0][i] >= leaf->maxs[i] )
		{
			return qfalse;
		}
	}

	CM_TestInLeafBrushNode_r(tw, &cm.leafbrushNodes[leaf->leafBrushNode], trace);

	return trace->allsolid;
}

/*
================
CM_TestInLeaf
================
*/
void CM_TestInLeaf( traceWork_t *tw, cLeaf_t *leaf, trace_t *trace )
{
	if ( (leaf->brushContents & tw->contents) )
	{
		if ( CM_TestInLeafBrushNode(tw, leaf, trace) )
		{
			return;
		}
	}

	if ( leaf->terrainContents & tw->contents )
	{
		CM_MeshTestInLeaf(tw, leaf, trace);
	}
}

/*
==================
CM_TestCapsuleInCapsule

capsule inside capsule check
==================
*/
void CM_TestCapsuleInCapsule( traceWork_t *tw, trace_t *trace )
{
	int i;
	vec3_t top, bottom;
	vec3_t p1, p2, tmp;
	vec3_t offset, symetricSize[2];
	float radius, halfwidth, halfheight, offs, r;
	float fHeightDiff, fTotalHalfHeight;

	VectorCopy( tw->extents.start, top );
	top[2] = top[2] + tw->offsetZ;
	VectorCopy( tw->extents.start, bottom );
	bottom[2] = bottom[2] - tw->offsetZ;
	for ( i = 0 ; i < 3 ; i++ )
	{
		offset[i] = ( tw->threadInfo.box_model->mins[i] + tw->threadInfo.box_model->maxs[i] ) * 0.5f;
		symetricSize[0][i] = tw->threadInfo.box_model->mins[i] - offset[i];
		symetricSize[1][i] = tw->threadInfo.box_model->maxs[i] - offset[i];
	}
	halfwidth = symetricSize[ 1 ][ 0 ];
	halfheight = symetricSize[ 1 ][ 2 ];
	radius = ( halfwidth > halfheight ) ? halfheight : halfwidth;
	offs = halfheight - radius;

	r = Square( tw->radius + radius );
	// check if any of the spheres overlap
	VectorCopy( offset, p1 );
	p1[2] += offs;
	VectorSubtract( p1, top, tmp );
	if ( VectorLengthSquared( tmp ) < r )
	{
		trace->startsolid = trace->allsolid = qtrue;
		trace->fraction = 0;
	}
	VectorSubtract( p1, bottom, tmp );
	if ( VectorLengthSquared( tmp ) < r )
	{
		trace->startsolid = trace->allsolid = qtrue;
		trace->fraction = 0;
	}
	VectorCopy( offset, p2 );
	p2[2] -= offs;
	VectorSubtract( p2, top, tmp );
	if ( VectorLengthSquared( tmp ) < r )
	{
		trace->startsolid = trace->allsolid = qtrue;
		trace->fraction = 0;
	}
	VectorSubtract( p2, bottom, tmp );
	if ( VectorLengthSquared( tmp ) < r )
	{
		trace->startsolid = trace->allsolid = qtrue;
		trace->fraction = 0;
	}
	// if between cylinder up and lower bounds
	fHeightDiff = tw->extents.start[2] - offset[2];
	fTotalHalfHeight = offs + tw->size[2] - tw->radius;
	assert(fTotalHalfHeight >= 0);
	if ( I_fabs(fHeightDiff) <= fTotalHalfHeight )
	{
		// 2d coordinates
		top[2] = p1[2] = 0;
		// if the cylinders overlap
		VectorSubtract( top, p1, tmp );
		if ( VectorLengthSquared( tmp ) < r )
		{
			trace->startsolid = trace->allsolid = qtrue;
			trace->fraction = 0;
		}
	}
}

void CM_PositionTest( traceWork_t *tw, trace_t *trace )
{
	int leafs[MAX_POSITION_LEAFS];
	int i;
	leafList_t ll;

	if ( trace->allsolid )
	{
		return;
	}

	// identify the leafs we are touching
	VectorSubtract( tw->extents.start, tw->size, ll.bounds[0] );
	VectorAdd( tw->extents.start, tw->size, ll.bounds[1] );

	for ( i = 0 ; i < 3 ; i++ )
	{
		ll.bounds[0][i] -= 1;
		ll.bounds[1][i] += 1;
	}

	ll.count = 0;
	ll.maxcount = MAX_POSITION_LEAFS;
	ll.list = leafs;
	ll.lastLeaf = 0;
	ll.overflowed = qfalse;

	CM_BoxLeafnums_r( &ll, 0 );

	if ( !ll.count )
	{
		return;
	}

	// test the contents of the leafs
	for ( i = 0 ; i < ll.count ; i++ )
	{
		if ( trace->allsolid )
		{
			break;
		}
		CM_TestInLeaf( tw, &cm.leafs[leafs[i]], trace );
	}
}

/*
==================
CM_TraceThroughBrush
==================
*/
void CM_TraceThroughBrush( traceWork_t *tw, cbrush_t *brush, trace_t *trace )
{
	int numsides;
	cplane_t *plane;
	float dist;
	float enterFrac;
	float leaveFrac;
	float d1;
	float d2;
	qboolean allsolid;
	float f;
	cbrushside_t *side;
	cbrushside_t *leadside;
	float distOffset;
	float delta;
	float sign;
	float *bounds;
	int j;
	cbrushside_t axialSide;
	cplane_t axialPlane;
	int index;
	float frac;

	assert(!IS_NAN((tw->extents.start)[0]) && !IS_NAN((tw->extents.start)[1]) && !IS_NAN((tw->extents.start)[2]));
	assert(!IS_NAN((tw->extents.end)[0]) && !IS_NAN((tw->extents.end)[1]) && !IS_NAN((tw->extents.end)[2]));

	enterFrac = 0.0;
	leaveFrac = trace->fraction;

	allsolid = qtrue;
	leadside = NULL;

	sign = -1.0;
	bounds = brush->mins;
	index = 0;

	while ( 2 )
	{
		//
		// compare the trace against all planes of the brush
		// find the latest time the trace crosses a plane towards the interior
		// and the earliest time the trace crosses a plane towards the exterior
		//

		assert(!IS_NAN((bounds)[0]) && !IS_NAN((bounds)[1]) && !IS_NAN((bounds)[2]));
		assert(!IS_NAN((tw->radiusOffset)[0]) && !IS_NAN((tw->radiusOffset)[1]) && !IS_NAN((tw->radiusOffset)[2]));

		for ( j = 0; j < 3; j++ )
		{
			d1 = (tw->extents.start[j] - bounds[j]) * sign - tw->radiusOffset[j];
			d2 = (tw->extents.end[j] - bounds[j]) * sign - tw->radiusOffset[j];

			assert(!IS_NAN(d1));
			assert(!IS_NAN(d2));

			if ( d1 > 0 )
			{
				if ( d2 >= (0.125f - d1 >= 0.0f ? d1 : 0.125f) )
				{
					return;
				}

				frac = (d1 - 0.125f) * tw->extents.invDelta[j] * sign;

				if ( frac >= leaveFrac )
				{
					return;
				}

				if ( d2 > 0 )
				{
					allsolid = qfalse;
				}

				if ( frac > enterFrac )
				{
					enterFrac = frac;
				}
				else
				{
					if ( leadside )
					{
						continue;
					}
				}

				axialSide.materialNum = brush->axialMaterialNum[index][j];
				VectorClear(axialPlane.normal);
				axialPlane.normal[j] = sign;
				axialSide.plane = &axialPlane;
				leadside = &axialSide;
			}
			else
			{
				if ( d2 > 0 )
				{
					frac = d1 * tw->extents.invDelta[j] * sign;

					if ( frac <= enterFrac )
					{
						return;
					}

					allsolid = qfalse;
					leaveFrac = (frac - leaveFrac >= 0.0f ? leaveFrac : frac);
				}
			}
		}

		if ( index )
		{
			break;
		}

		sign = 1.0;
		bounds = brush->maxs;
		index = 1;
		continue;
	}

	side = brush->sides;
	numsides = brush->numsides;

	while ( 2 )
	{
		//
		// compare the trace against all planes of the brush
		// find the latest time the trace crosses a plane towards the interior
		// and the earliest time the trace crosses a plane towards the exterior
		//

		if ( !numsides )
		{
			break;
		}

		plane = side->plane;

		distOffset = I_fabs(plane->normal[2] * tw->offsetZ);
		dist = plane->dist + tw->radius + distOffset;
		assert(!IS_NAN(dist));

		d1 = DotProduct(tw->extents.start, plane->normal) - dist;
		d2 = DotProduct(tw->extents.end, plane->normal) - dist;

		assert(!IS_NAN(d1));
		assert(!IS_NAN(d2));

		if ( d1 > 0 )
		{
			// if completely in front of face, no intersection with the entire brush
			if ( d2 >= (0.125f - d1 >= 0.0f ? d1 : 0.125f) )
			{
				return;
			}

			if ( d2 > 0 )
			{
				allsolid = qfalse;
			}

			delta = d1 - d2;

			assert(!IS_NAN(delta));
			assert(delta > 0);

			f = d1 - 0.125f;

			if ( f > enterFrac * delta )
			{
				enterFrac = f / delta;

				if ( enterFrac >= leaveFrac )
				{
					return;
				}
			}
			else
			{
				if ( leadside )
				{
					goto nextside;
				}
			}

			leadside = side;
		}
		else
		{
			if ( d2 > 0 )
			{
				delta = d1 - d2;

				assert(delta < 0);

				if ( d1 > leaveFrac * delta )
				{
					leaveFrac = d1 / delta;

					if ( enterFrac >= leaveFrac )
					{
						return;
					}
				}

				allsolid = qfalse;
			}
		}

nextside:
		--numsides;
		++side;
	}

	trace->contents = brush->contents;

	if ( !leadside )
	{
		trace->startsolid = qtrue;

		if ( allsolid )
		{
			trace->allsolid = qtrue;
			trace->fraction = 0.0;
		}

		return;
	}

	trace->fraction = enterFrac;
	assert(trace->fraction >= 0 && trace->fraction <= 1.0f);
	VectorCopy(leadside->plane->normal, trace->normal);
	trace->surfaceFlags = cm.materials[leadside->materialNum].surfaceFlags;
	trace->material = &cm.materials[leadside->materialNum];
}

/*
================
CM_TraceThroughLeafBrushNode_r
================
*/
static void CM_TraceThroughLeafBrushNode_r( traceWork_t *tw, cLeafBrushNode_t *node, const vec4_t p1_, const vec4_t p2, trace_t *trace )
{
	int i;
	int brushnum;
	cbrush_t *brush;
	float offset;
	float t1;
	float t2;
	float frc;
	float tmin;
	float tmax;
	vec4_t p1;
	float idist;
	int side;
	float frac;
	float frac2;
	vec4_t mid;
	float diff;
	float absDiff;

	assert(node);
	VectorCopy4(p1_, p1);

loop:
	if ( !(node->contents & tw->contents) )
	{
		return;
	}

	if ( node->leafBrushCount )
	{
		if ( node->leafBrushCount > 0 )
		{
			// trace line against all brushes in the leaf
			for ( i = 0; i < node->leafBrushCount; i++ )
			{
				brushnum = node->data.leaf.brushes[i];
				brush = &cm.brushes[brushnum];

				if ( !(brush->contents & tw->contents) )
				{
					continue;
				}

				CM_TraceThroughBrush(tw, brush, trace);
			}

			return;
		}

		CM_TraceThroughLeafBrushNode_r(tw, node + 1, p1, p2, trace);
	}

	t1 = p1[node->axis] - node->data.children.dist;
	t2 = p2[node->axis] - node->data.children.dist;

	offset = tw->size[node->axis] + (float)SURFACE_CLIP_EPSILON - node->data.children.range;

	tmax = I_fmax(t1, t2);
	tmin = I_fmin(t1, t2);

	if ( tmin >= offset )
	{
		if ( tmax <= -offset )
		{
			return;
		}

		node += node->data.children.childOffset[0];
		goto loop;
	}

	if ( tmax <= -offset )
	{
		node += node->data.children.childOffset[1];
		goto loop;
	}

	if ( trace->fraction <= p1[3] )
	{
		return;
	}

	diff = t2 - t1;
	absDiff = I_fabs(diff);

	if ( absDiff > 0.00000047683716f )
	{
		frc = I_fsel(diff, -t1, t1);
		idist = 1.0f / absDiff;
		frac2 = (frc - offset) * idist;
		frac = (frc + offset) * idist;
		side = I_side(diff);
	}
	else
	{
		side = 0;
		frac = 1.0f;
		frac2 = 0.0f;
	}

	frac = I_fmin(frac, 1.0);

	mid[0] = p1[0] + (p2[0] - p1[0]) * frac;
	mid[1] = p1[1] + (p2[1] - p1[1]) * frac;
	mid[2] = p1[2] + (p2[2] - p1[2]) * frac;
	mid[3] = p1[3] + (p2[3] - p1[3]) * frac;

	CM_TraceThroughLeafBrushNode_r(tw, &node[node->data.children.childOffset[side]], p1, mid, trace);

	frac2 = I_fmax(frac2, 0.0f);

	p1[0] = p1[0] + (p2[0] - p1[0]) * frac2;
	p1[1] = p1[1] + (p2[1] - p1[1]) * frac2;
	p1[2] = p1[2] + (p2[2] - p1[2]) * frac2;
	p1[3] = p1[3] + (p2[3] - p1[3]) * frac2;

	node += node->data.children.childOffset[1 - side];
		goto loop;
}

/*
================
CM_TraceThroughLeafBrushNode
================
*/
bool CM_TraceThroughLeafBrushNode( traceWork_t *tw, cLeaf_t *leaf, trace_t *trace )
{
	vec3_t absmin;
	vec3_t absmax;
	vec4_t start;
	vec4_t end;

	assert(leaf->leafBrushNode);

	VectorSubtract(leaf->mins, tw->size, absmin);
	VectorAdd(leaf->maxs, tw->size, absmax);

	if ( CM_TraceBox(&tw->extents, absmin, absmax, trace->fraction) )
	{
		return 0;
	}

	VectorCopy(tw->extents.start, start);
	VectorCopy(tw->extents.end, end);

	start[3] = 0.0;
	end[3] = trace->fraction;

	CM_TraceThroughLeafBrushNode_r(tw, &cm.leafbrushNodes[leaf->leafBrushNode], start, end, trace);

	return trace->fraction == 0;
}

/*
================
CM_TraceThroughLeaf
================
*/
void CM_TraceThroughLeaf( traceWork_t *tw, cLeaf_t *leaf, trace_t *trace )
{
	int k;

	if ( trace->fraction == 0 )
	{
		return;
	}

	if ( leaf->brushContents & tw->contents )
	{
		if ( CM_TraceThroughLeafBrushNode(tw, leaf, trace) )
		{
			return;
		}
	}

	if ( leaf->terrainContents & tw->contents )
	{
		// trace line against all brushes in the leaf
		for ( k = 0; k < leaf->collAabbCount; k++ )
		{
			if ( !trace->fraction )
			{
				return;
			}

			CM_TraceThroughAabbTree(tw, &cm.aabbTrees[k + leaf->firstCollAabbIndex], trace);
		}
	}
}

/*
==================
CM_TraceSphereThroughSphere
==================
*/
qboolean CM_TraceSphereThroughSphere( traceWork_t *tw, const vec3_t vStart, const vec3_t vEnd,
                                      const vec3_t vStationary, float radius, trace_t *trace )
{
	vec3_t vDelta;
	float fA;
	float fB;
	float fC;
	float fDiscriminant;
	float fRadiusSqrd;
	float fEntry;
	vec_t fDeltaLen;
	vec3_t vNormal;
	VectorSubtract(vStart, vStationary, vDelta);
	fRadiusSqrd = Square(radius + tw->radius);
	fC = DotProduct(vDelta, vDelta) - fRadiusSqrd;
	if ( fC <= 0 )
	{
		trace->fraction = 0;
		trace->startsolid = 1;
		Vec3NormalizeTo(vDelta, trace->normal);
		trace->contents = tw->threadInfo.box_brush->contents;
		VectorSubtract(vEnd, vStationary, vDelta);
		if ( VectorLengthSquared(vDelta) <= fRadiusSqrd )
		{
			trace->allsolid = 1;
		}
		return qfalse;
	}
	fB = DotProduct(tw->delta, vDelta);
	if ( fB >= 0 )
	{
		return qtrue;
	}
	fA = tw->deltaLenSq;
	assert(fA > 0.0f);
	fDiscriminant = Square(fB) - fA * fC;
	if ( fDiscriminant < 0 )
	{
		return qtrue;
	}
	fDeltaLen = Vec3NormalizeTo(vDelta, vNormal) * (float)SURFACE_CLIP_EPSILON;
	fEntry = (-fB - I_sqrt(fDiscriminant)) / fA + fDeltaLen / fB;
	if ( fEntry < trace->fraction )
	{
		trace->fraction = I_fmax(fEntry, 0);
		assert(trace->fraction >= 0 && trace->fraction <= 1.0f);
		VectorCopy(vNormal, trace->normal);
		trace->contents = tw->threadInfo.box_brush->contents;
		return qfalse;
	}
	return qtrue;
}

/*
==================
CM_TraceCylinderThroughCylinder
==================
*/
qboolean CM_TraceCylinderThroughCylinder( traceWork_t *tw, const vec3_t vStationary, float fStationaryHalfHeight, float radius, trace_t *trace )
{
	vec3_t vDelta;
	float fA;
	float fB;
	float fC;
	float fDiscriminant;
	float fRadiusSqrd;
	float fEntry;
	float fEpsilon;
	vec_t fDeltaLen;
	vec3_t vNormal;
	float fTotalHeight;
	float fHitHeight;

	VectorSubtract(tw->extents.start, vStationary, vDelta);
	fRadiusSqrd = Square(radius + tw->radius);
	fC = Dot2Product(vDelta, vDelta) - fRadiusSqrd;

	if ( fC <= 0 )
	{
		fTotalHeight = tw->size[2] - tw->radius + fStationaryHalfHeight;

		if ( I_fabs(vDelta[2]) > fTotalHeight )
		{
			return qtrue;
		}

		trace->fraction = 0.0;
		trace->startsolid = 1;

		vDelta[2] = 0.0;
		Vec3NormalizeTo(vDelta, trace->normal);
		trace->contents = tw->threadInfo.box_brush->contents;
		VectorSubtract(tw->extents.end, vStationary, vDelta);

		if ( fTotalHeight >= I_fabs(vDelta[2]) )
		{
			trace->allsolid = 1;
		}

		return qfalse;
	}

	fB = Dot2Product(tw->delta, vDelta);

	if ( fB >= 0 )
	{
		return qtrue;
	}

	fA = Dot2Product(tw->delta, tw->delta);
	fDiscriminant = Square(fB) - fA * fC;

	if ( fDiscriminant < 0 )
	{
		return qtrue;
	}

	vDelta[2] = 0.0;
	fDeltaLen = Vec3NormalizeTo(vDelta, vNormal) * (float)SURFACE_CLIP_EPSILON;
	fEpsilon = fDeltaLen / fB;
	fEntry = (-fB - I_sqrt(fDiscriminant)) / fA + fEpsilon;

	if ( fEntry < trace->fraction )
	{
		fTotalHeight = tw->size[2] - tw->radius + fStationaryHalfHeight;
		fHitHeight = tw->extents.start[2] + (fEntry - fEpsilon) * tw->delta[2] - vStationary[2];

		if ( I_fabs(fHitHeight) > fTotalHeight )
		{
			return qtrue;
		}

		trace->fraction = I_fmax(fEntry, 0.0);
		VectorCopy(vNormal, trace->normal);
		trace->contents = tw->threadInfo.box_brush->contents;

		return qfalse;
	}

	return qtrue;
}

/*
================
CM_TraceCapsuleThroughCapsule

capsule vs. capsule collision (not rotated)
================
*/
void CM_TraceCapsuleThroughCapsule( traceWork_t *tw, trace_t *trace )
{
	int i;
	vec3_t top, bottom, starttop, startbottom, endtop, endbottom;
	vec3_t offset, symetricSize[2];
	float radius, halfwidth, halfheight, offs;

	// test trace bounds vs. capsule bounds
	if ( 	   tw->bounds[0][0] > tw->threadInfo.box_model->maxs[0] + RADIUS_EPSILON
	           || tw->bounds[0][1] > tw->threadInfo.box_model->maxs[1] + RADIUS_EPSILON
	           || tw->bounds[0][2] > tw->threadInfo.box_model->maxs[2] + RADIUS_EPSILON
	           || tw->bounds[1][0] < tw->threadInfo.box_model->mins[0] - RADIUS_EPSILON
	           || tw->bounds[1][1] < tw->threadInfo.box_model->mins[1] - RADIUS_EPSILON
	           || tw->bounds[1][2] < tw->threadInfo.box_model->mins[2] - RADIUS_EPSILON
	   )
	{
		return;
	}
	// top origin and bottom origin of each sphere at start and end of trace
	VectorCopy( tw->extents.start, starttop );
	VectorCopy( tw->extents.start, startbottom );
	starttop[2] = starttop[2] + tw->offsetZ;
	startbottom[2] = startbottom[2] - tw->offsetZ;
	VectorCopy( tw->extents.end, endtop );
	endtop[2] = endtop[2] + tw->offsetZ;
	VectorCopy( tw->extents.end, endbottom );
	endbottom[2] = endbottom[2] - tw->offsetZ;

	// calculate top and bottom of the capsule spheres to collide with
	for ( i = 0 ; i < 3 ; i++ )
	{
		offset[i] = ( tw->threadInfo.box_model->maxs[i] + tw->threadInfo.box_model->mins[i] ) * 0.5f;
		symetricSize[0][i] = tw->threadInfo.box_model->mins[i] - offset[i];
		symetricSize[1][i] = tw->threadInfo.box_model->maxs[i] - offset[i];
	}
	halfwidth = symetricSize[ 1 ][ 0 ];
	halfheight = symetricSize[ 1 ][ 2 ];
	radius = ( halfwidth > halfheight ) ? halfheight : halfwidth;
	offs = halfheight - radius;
	VectorCopy( offset, top );
	top[2] += offs;
	VectorCopy( offset, bottom );
	bottom[2] -= offs;

	// test for collision between the spheres
	if ( startbottom[2] > top[2] )
	{
		if ( !CM_TraceSphereThroughSphere(tw, startbottom, endbottom, top, radius, trace) )
		{
			return;
		}

		if ( tw->delta[2] >= 0 )
		{
			return;
		}
	}
	else
	{
		if ( starttop[2] < bottom[2] )
		{
			if ( !CM_TraceSphereThroughSphere(tw, starttop, endtop, bottom, radius, trace) )
			{
				return;
			}

			if ( tw->delta[2] <= 0 )
			{
				return;
			}
		}
	}

	// height of the expanded cylinder is the height of both cylinders minus the radius of both spheres
	if ( !CM_TraceCylinderThroughCylinder(tw, offset, offs, radius, trace) )
	{
		return;
	}

	if ( endbottom[2] > top[2] )
	{
		if ( startbottom[2] <= top[2] )
		{
			if ( !CM_TraceSphereThroughSphere(tw, startbottom, endbottom, top, radius, trace) )
			{
				return;
			}
		}
	}
	else
	{
		if ( endtop[2] < bottom[2] )
		{
			if ( starttop[2] >= bottom[2] )
			{
				if ( !CM_TraceSphereThroughSphere(tw, starttop, endtop, bottom, radius, trace) )
				{
					return;
				}
			}
		}
	}
}

/*
==================
CM_TraceThroughTree

Traverse all the contacted leafs from the start to the end position.
If the trace is a point, they will be exactly in order, but for larger
trace volumes it is possible to hit something in a later leaf with
a smaller intercept fraction.
==================
*/
void CM_TraceThroughTree( traceWork_t *tw, int num, const vec4_t p1_, const vec4_t p2, trace_t *trace )
{
	cNode_t *node;
	cplane_t *plane;
	float t1;
	float t2;
	float offset;
	float frc;
	float frac;
	float frac2;
	vec4_t mid;
	int side;
	vec4_t p1;
	float diff;
	float absDiff;
	float idist;

	VectorCopy4(p1_, p1);

	while ( 1 )
	{
		if ( num < 0 )
		{
			CM_TraceThroughLeaf(tw, &cm.leafs[-1 - num], trace);
			return;
		}

		//
		// find the point distances to the seperating plane
		// and the offset for the size of the box
		//

		node = &cm.nodes[num];
		plane = node->plane;

		// adjust the plane distance apropriately for mins/maxs
		if ( plane->type < 3 )
		{
			t1 = p1[plane->type] - plane->dist;
			t2 = p2[plane->type] - plane->dist;

			offset = tw->size[plane->type] + (float)SURFACE_CLIP_EPSILON;
		}
		else
		{
			t1 = DotProduct(plane->normal, p1) - plane->dist;
			t2 = DotProduct(plane->normal, p2) - plane->dist;

			if ( tw->isPoint )
			{
				offset = (float)SURFACE_CLIP_EPSILON;
			}
			else
			{
				// this is silly
				offset = 2048;
			}
		}

		if ( I_fmin(t1, t2) >= offset )
		{
			num = node->children[0];
			continue;
		}

		if ( I_fmax(t1, t2) <= -offset )
		{
			num = node->children[1];
			continue;
		}

		if ( trace->fraction <= p1[3] )
		{
			return;
		}

		diff = t2 - t1;
		absDiff = I_fabs(diff);

		if ( absDiff > 0.00000047683716f )
		{
			frc = I_fsel(diff, -t1, t1);
			idist = 1.0f / absDiff;
			frac2 = (frc - offset) * idist;
			frac = (frc + offset) * idist;
			side = I_side(diff);
		}
		else
		{
			side = 0;
			frac = 1.0f;
			frac2 = 0.0f;
		}

		frac = I_fmin(frac, 1.0);

		mid[0] = p1[0] + (p2[0] - p1[0]) * frac;
		mid[1] = p1[1] + (p2[1] - p1[1]) * frac;
		mid[2] = p1[2] + (p2[2] - p1[2]) * frac;
		mid[3] = p1[3] + (p2[3] - p1[3]) * frac;

		CM_TraceThroughTree(tw, node->children[side], p1, mid, trace);

		frac2 = I_fmax(frac2, 0.0f);

		p1[0] = p1[0] + (p2[0] - p1[0]) * frac2;
		p1[1] = p1[1] + (p2[1] - p1[1]) * frac2;
		p1[2] = p1[2] + (p2[2] - p1[2]) * frac2;
		p1[3] = p1[3] + (p2[3] - p1[3]) * frac2;

		num = node->children[side ^ 1];
	}
}

/*
==================
CM_SetAxialCullOnly
==================
*/
void CM_SetAxialCullOnly( traceWork_t *tw )
{
	vec3_t d;
	float dVolume;
	float s;
	float c;

	VectorSubtract(tw->bounds[1], tw->bounds[0], d);

	dVolume = d[0] * d[1] * d[2];
	s = tw->size[0] * tw->size[1] * tw->size[2];
	c = s * 16.0f * tw->deltaLen;
	tw->axialCullOnly = c > dVolume;
}

/*
==================
CM_Trace
==================
*/
void CM_Trace( trace_t *results, const vec3_t start, const vec3_t end,
               const vec3_t mins, const vec3_t maxs,
               clipHandle_t model, int brushmask )
{
	int i;
	traceWork_t tw;
	vec3_t offset;
	cmodel_t    *cmod;
	vec4_t _start, _end;
	assert(cm.numNodes);
	assert(mins);
	assert(maxs);
	assert(!IS_NAN((end)[0]) && !IS_NAN((end)[1]) && !IS_NAN((end)[2]));
	cmod = CM_ClipHandleToModel( model );
	// set basic parms
	tw.contents = brushmask;
	// adjust so that mins and maxs are always symetric, which
	// avoids some complications with plane expanding of rotated
	// bmodels
	for ( i = 0 ; i < 3 ; i++ )
	{
		assert(maxs[i] >= mins[i]);
		offset[i] = (mins[i] + maxs[i]) * 0.5f;
		tw.size[i] = maxs[i] - offset[i];
		tw.extents.start[i] = start[i] + offset[i];
		tw.extents.end[i] = end[i] + offset[i];
		tw.midpoint[i] = (tw.extents.start[i] + tw.extents.end[i]) * 0.5f;
		tw.delta[i] = tw.extents.end[i] - tw.extents.start[i];
		tw.halfDelta[i] = tw.delta[i] * 0.5f;
		tw.halfDeltaAbs[i] = I_fabs(tw.halfDelta[i]);
	}
	CM_CalcTraceEntents(&tw.extents);
	tw.deltaLenSq = VectorLengthSquared(tw.delta);
	tw.deltaLen = I_sqrt(tw.deltaLenSq);
	// selects through a float temp: a conditional expression, not an if/else
	tw.radius = tw.size[0] > tw.size[2] ? tw.size[2] : tw.size[0];
	tw.offsetZ = tw.size[2] - tw.radius;
	// calculate bounds
	for ( i = 0; i < 2; i++ )
	{
		if ( tw.extents.start[i] < tw.extents.end[i] )
		{
			tw.bounds[0][i] = tw.extents.start[i] - tw.radius;
			tw.bounds[1][i] = tw.extents.end[i] + tw.radius;
		}
		else
		{
			tw.bounds[0][i] = tw.extents.end[i] - tw.radius;
			tw.bounds[1][i] = tw.extents.start[i] + tw.radius;
		}
	}
	assert(tw.offsetZ >= 0);
	if ( tw.extents.start[2] < tw.extents.end[2] )
	{
		tw.bounds[0][2] = tw.extents.start[2] - tw.offsetZ - tw.radius;
		tw.bounds[1][2] = tw.extents.end[2] + tw.offsetZ + tw.radius;
	}
	else
	{
		tw.bounds[0][2] = tw.extents.end[2] - tw.offsetZ - tw.radius;
		tw.bounds[1][2] = tw.extents.start[2] + tw.offsetZ + tw.radius;
	}
	CM_SetAxialCullOnly(&tw);
	CM_GetTrackThreadInfo(&tw.threadInfo);
	//
	// check for position test special case
	//
	if ( VectorCompare(start, end) )
	{
		tw.isPoint = 0;
		if ( model )
		{
			if ( model == CAPSULE_MODEL_HANDLE )
			{
				if ( (tw.threadInfo.box_brush->contents & tw.contents) )
				{
					CM_TestCapsuleInCapsule(&tw, results);
					assert(!IS_NAN(results->fraction));
				}
			}
			else
			{
				if ( !results->allsolid )
				{
					CM_TestInLeaf(&tw, &cmod->leaf, results);
					assert(!IS_NAN(results->fraction));
				}
			}
		}
		else
		{
			CM_PositionTest(&tw, results);
		}
	}
	else
	{
		assert(tw.size[0] >= 0);
		assert(tw.size[1] >= 0);
		assert(tw.size[2] >= 0);
		tw.isPoint = tw.size[0] + tw.size[1] + tw.size[2] == 0;
		assert(tw.offsetZ >= 0);
		tw.radiusOffset[0] = tw.radius;
		tw.radiusOffset[1] = tw.radius;
		tw.radiusOffset[2] = tw.radius + tw.offsetZ;
		if ( model )
		{
			if ( model == CAPSULE_MODEL_HANDLE )
			{
				if ( (tw.threadInfo.box_brush->contents & tw.contents) )
				{
					CM_TraceCapsuleThroughCapsule(&tw, results);
				}
			}
			else
			{
				CM_TraceThroughLeaf(&tw, &cmod->leaf, results);
			}
		}
		else
		{
			VectorCopy(tw.extents.start, _start);
			_start[3] = 0.0;
			VectorCopy(tw.extents.end, _end);
			_end[3] = results->fraction;
			CM_TraceThroughTree(&tw, 0, _start, _end, results);
		}
	}
}
/*
==================
CM_BoxTrace
==================
*/
void CM_BoxTrace( trace_t *results, const vec3_t start, const vec3_t end,
                  const vec3_t mins, const vec3_t maxs,
                  clipHandle_t model, int brushmask )
{
	memset(results, 0, sizeof(trace_t));
	results->fraction = 1.0;
	CM_Trace(results, start, end, mins, maxs, model, brushmask);
}
/*
==================
CM_TransformedBoxTrace
Handles offseting and rotation of the end points for moving and
rotating entities
==================
*/
void CM_TransformedBoxTrace( trace_t *results, const vec3_t start, const vec3_t end,
                             const vec3_t mins, const vec3_t maxs,
                             clipHandle_t model, int brushmask,
                             const vec3_t origin, const vec3_t angles )
{
	vec3_t start_l, end_l;
	qboolean rotated;
	vec3_t offset;
	vec3_t symetricSize[2];
	vec3_t matrix[3], transpose[3];
	int i;
	// the frame keeps two dead float copies of symetricSize[1]
	float symetricSizeX;
	float symetricSizeZ;
	assert(mins);
	assert(maxs);
	// adjust so that mins and maxs are always symetric, which
	// avoids some complications with plane expanding of rotated
	// bmodels
	for ( i = 0 ; i < 3 ; i++ )
	{
		offset[i] = ( mins[i] + maxs[i] ) * 0.5f;
		symetricSize[0][i] = mins[i] - offset[i];
		symetricSize[1][i] = maxs[i] - offset[i];
		start_l[i] = start[i] + offset[i];
		end_l[i] = end[i] + offset[i];
	}
	// subtract origin offset
	VectorSubtract( start_l, origin, start_l );
	VectorSubtract( end_l, origin, end_l );
	// rotate start and end into the models frame of reference
	rotated = ( angles[0] || angles[1] || angles[2] );
	symetricSizeX = symetricSize[1][0];
	symetricSizeZ = symetricSize[1][2];
	if ( rotated )
	{
		// rotation on trace line (start-end) instead of rotating the bmodel
		// NOTE: This is still incorrect for bounding boxes because the actual bounding
		//		 box that is swept through the model is not rotated. We cannot rotate
		//		 the bounding box or the bmodel because that would make all the brush
		//		 bevels invalid.
		//		 However this is correct for capsules since a capsule itself is rotated too.
		CreateRotationMatrix( angles, matrix );
		RotatePoint( start_l, matrix );
		RotatePoint( end_l, matrix );
	}
	float oldFraction = results->fraction;
	// sweep the box through the model
	CM_Trace( results, start_l, end_l, symetricSize[0], symetricSize[1], model, brushmask );
	// if the bmodel was rotated and there was a collision
	if ( rotated )
	{
		if ( results->fraction < oldFraction )
		{
			// rotation of bmodel collision plane
			TransposeMatrix( matrix, transpose );
			RotatePoint( results->normal, transpose );
		}
	}
}
/*
==================
CM_TransformedBoxTraceExternal
==================
*/
void CM_TransformedBoxTraceExternal( trace_t *results, const vec3_t start, const vec3_t end,
                                     const vec3_t mins, const vec3_t maxs,
                                     clipHandle_t model, int brushmask,
                                     const vec3_t origin, const vec3_t angles )
{
	memset(results, 0, sizeof(trace_t));
	results->fraction = 1.0;
	CM_TransformedBoxTrace(results, start, end, mins, maxs, model, brushmask, origin, angles);
}

/*
==================
CM_SightTraceThroughBrush
==================
*/
int CM_SightTraceThroughBrush( traceWork_t *tw, cbrush_t *brush )
{
	int k;
	cplane_t *plane;
	float dist;
	float enterFrac;
	float leaveFrac;
	float delta;
	float d1;
	float d2;
	cbrushside_t *side;
	float f;
	float frac;
	int j;
	float sign;
	float *bounds;
	int index;

	assert(!IS_NAN((tw->extents.start)[0]) && !IS_NAN((tw->extents.start)[1]) && !IS_NAN((tw->extents.start)[2]));
	assert(!IS_NAN((tw->extents.end)[0]) && !IS_NAN((tw->extents.end)[1]) && !IS_NAN((tw->extents.end)[2]));

	enterFrac = 0.0f;
	leaveFrac = 1.0;

	sign = -1.0;
	bounds = brush->mins;

	for ( index = 0; ; index = 1 )
	{
		for ( j = 0; j < 3; j++ )
		{
			d1 = (tw->extents.start[j] - bounds[j]) * sign - tw->radiusOffset[j];
			d2 = (tw->extents.end[j] - bounds[j]) * sign - tw->radiusOffset[j];

			if ( d1 > 0 )
			{
				if ( d2 > 0 )
				{
					return 0;
				}

				frac = d1 * tw->extents.invDelta[j] * sign;

				if ( frac >= leaveFrac )
				{
					return 0;
				}

				enterFrac = I_fmax(enterFrac, frac);
				continue;
			}

			if ( d2 > 0 )
			{
				frac = d1 * tw->extents.invDelta[j] * sign;

				if ( frac <= enterFrac )
				{
					return 0;
				}

				leaveFrac = I_fmin(leaveFrac, frac);
				continue;
			}
		}

		if ( index )
		{
			break;
		}

		sign = 1.0;
		bounds = brush->maxs;
	}

	side = brush->sides;
	for ( k = brush->numsides; k; k--, side++ )
	{
		plane = side->plane;
		f = I_fabs(plane->normal[2] * tw->offsetZ);
		dist = plane->normal[3] + tw->radius + f;

		d1 = DotProduct(tw->extents.start, plane->normal) - dist;
		d2 = DotProduct(tw->extents.end, plane->normal) - dist;

		if ( d1 > 0 )
		{
			delta = d1 - d2;

			if ( d2 > 0 )
			{
				return 0;
			}

			if ( d1 > enterFrac * delta )
			{
				enterFrac = d1 / delta;

				if ( enterFrac >= leaveFrac )
				{
					return 0;
				}
			}

			continue;
		}

		if ( d2 > 0 )
		{
			delta = d1 - d2;

			if ( d1 > leaveFrac * delta )
			{
				leaveFrac = d1 / delta;

				if ( enterFrac >= leaveFrac )
				{
					return 0;
				}
			}

			continue;
		}
	}

	return brush - cm.brushes + 1;
}

/*
================
CM_SightTraceThroughLeafBrushNode_r
================
*/
static int CM_SightTraceThroughLeafBrushNode_r( traceWork_t *tw, cLeafBrushNode_t *remoteNode, const vec3_t p1_, const vec3_t p2 )
{
	int i;
	int brushnum;
	cbrush_t *brush;
	int hitNum;
	float offset;
	float t1;
	float t2;
	float frc;
	float tmin;
	float tmax;
	vec3_t p1;
	float idist;
	int side;
	float frac;
	float frac2;
	vec3_t mid;
	float diff;
	float absDiff;

	assert(remoteNode);
	VectorCopy(p1_, p1);

loop:
	if ( !(remoteNode->contents & tw->contents) )
	{
		return 0;
	}

	if ( remoteNode->leafBrushCount )
	{
		if ( remoteNode->leafBrushCount > 0 )
		{
			// trace line against all brushes in the leaf
			for ( i = 0; i < remoteNode->leafBrushCount; i++ )
			{
				brushnum = remoteNode->data.leaf.brushes[i];
				brush = &cm.brushes[brushnum];

				if ( !(brush->contents & tw->contents) )
				{
					continue;
				}

				hitNum = CM_SightTraceThroughBrush(tw, brush);

				if ( hitNum )
				{
					return hitNum;
				}
			}

			return 0;
		}

		hitNum = CM_SightTraceThroughLeafBrushNode_r(tw, remoteNode + 1, p1, p2);

		if ( hitNum )
		{
			return hitNum;
		}
	}

	t1 = p1[remoteNode->axis] - remoteNode->data.children.dist;
	t2 = p2[remoteNode->axis] - remoteNode->data.children.dist;

	offset = tw->size[remoteNode->axis] + (float)SURFACE_CLIP_EPSILON - remoteNode->data.children.range;

	tmax = I_fmax(t1, t2);
	tmin = I_fmin(t1, t2);

	if ( tmin >= offset )
	{
		if ( tmax <= -offset )
		{
			return 0;
		}

		remoteNode += remoteNode->data.children.childOffset[0];
		goto loop;
	}

	if ( tmax <= -offset )
	{
		remoteNode += remoteNode->data.children.childOffset[1];
		goto loop;
	}

	diff = t2 - t1;
	absDiff = I_fabs(diff);

	if ( absDiff > 0.00000047683716f )
	{
		frc = I_fsel(diff, -t1, t1);
		idist = 1.0f / absDiff;
		frac2 = (frc - offset) * idist;
		frac = (frc + offset) * idist;
		side = I_side(diff);
	}
	else
	{
		side = 0;
		frac = 1.0f;
		frac2 = 0.0f;
	}

	frac = I_fmin(frac, 1.0);

	mid[0] = p1[0] + (p2[0] - p1[0]) * frac;
	mid[1] = p1[1] + (p2[1] - p1[1]) * frac;
	mid[2] = p1[2] + (p2[2] - p1[2]) * frac;

	hitNum = CM_SightTraceThroughLeafBrushNode_r(tw, &remoteNode[remoteNode->data.children.childOffset[side]], p1, mid);

	if ( hitNum )
	{
		return hitNum;
	}

	frac2 = I_fmax(frac2, 0.0f);

	p1[0] = p1[0] + (p2[0] - p1[0]) * frac2;
	p1[1] = p1[1] + (p2[1] - p1[1]) * frac2;
	p1[2] = p1[2] + (p2[2] - p1[2]) * frac2;

	remoteNode += remoteNode->data.children.childOffset[1 - side];
	goto loop;
}

/*
================
CM_SightTraceThroughLeafBrushNode
================
*/
int CM_SightTraceThroughLeafBrushNode( traceWork_t *tw, cLeaf_t *leaf )
{
	vec3_t absmin;
	vec3_t absmax;

	assert(leaf->leafBrushNode);

	VectorSubtract(leaf->mins, tw->size, absmin);
	VectorAdd(leaf->maxs, tw->size, absmax);

	if ( CM_TraceBox(&tw->extents, absmin, absmax, 1.0) )
	{
		return 0;
	}

	return CM_SightTraceThroughLeafBrushNode_r(tw, &cm.leafbrushNodes[leaf->leafBrushNode], tw->extents.start, tw->extents.end);
}

/*
================
CM_SightTraceThroughLeaf
================
*/
int CM_SightTraceThroughLeaf( traceWork_t *tw, cLeaf_t *leaf, trace_t *trace )
{
	int k;
	int hitNum;

	if ( (leaf->brushContents & tw->contents) )
	{
		hitNum = CM_SightTraceThroughLeafBrushNode(tw, leaf);

		if ( hitNum )
		{
			return hitNum;
		}
	}

	assert(trace->fraction == 1.0f);

	if ( (leaf->terrainContents & tw->contents) )
	{
		// trace line against all brushes in the leaf
		for ( k = 0; k < leaf->collAabbCount; k++ )
		{
			CM_SightTraceThroughAabbTree(tw, &cm.aabbTrees[k + leaf->firstCollAabbIndex], trace);

			if ( trace->fraction != 1 )
			{
				return cm.numBrushes + leaf->firstCollAabbIndex + k + 1;
			}
		}
	}

	return 0;
}

/*
==================
CM_SightTraceSphereThroughSphere
==================
*/
qboolean CM_SightTraceSphereThroughSphere( traceWork_t *tw, const vec3_t vStart, const vec3_t vEnd,
        const vec3_t vStationary, float radius, trace_t *trace )
{
	vec3_t vDelta;
	float fA;
	float fB;
	float fC;
	float fDiscriminant;
	float fRadiusSqrd;
	float fEntry;
	float fEpsilon;
	vec_t fDeltaLen;
	vec3_t vNormal;
	VectorSubtract(vStart, vStationary, vDelta);
	fRadiusSqrd = Square(radius + tw->radius);
	fC = DotProduct(vDelta, vDelta) - fRadiusSqrd;
	if ( fC <= 0 )
	{
		return qfalse;
	}
	fB = DotProduct(tw->delta, vDelta);
	if ( fB >= 0 )
	{
		return qtrue;
	}
	fA = tw->deltaLenSq;
	fDiscriminant = Square(fB) - fA * fC;
	if ( fDiscriminant < 0 )
	{
		return qtrue;
	}
	fEpsilon = fB * (float)SURFACE_CLIP_EPSILON;
	fDeltaLen = Vec3NormalizeTo(vDelta, vNormal);
	fEpsilon = fEpsilon / fDeltaLen;
	fEntry = (-fB - I_sqrt(fDiscriminant)) / fA + fEpsilon;
	return fEntry >= trace->fraction;
}

/*
==================
CM_SightTraceCylinderThroughCylinder
==================
*/
qboolean CM_SightTraceCylinderThroughCylinder( traceWork_t *tw, const vec3_t vStationary, float fStationaryHalfHeight, float radius, trace_t *trace )
{
	vec3_t vDelta;
	float fA;
	float fB;
	float fC;
	float fDiscriminant;
	float fRadiusSqrd;
	float fEntry;
	float fEpsilon;
	vec_t fDeltaLen;
	vec3_t vNormal;
	float fTotalHeight;
	float fHitHeight;

	VectorSubtract(tw->extents.start, vStationary, vDelta);
	fRadiusSqrd = Square(radius + tw->radius);
	fC = Dot2Product(vDelta, vDelta) - fRadiusSqrd;

	if ( fC <= 0 )
	{
		fTotalHeight = tw->size[2] - tw->radius + fStationaryHalfHeight;
		assert(fTotalHeight >= 0);

		return I_fabs(vDelta[2]) > fTotalHeight;
	}

	fB = Dot2Product(tw->delta, vDelta);

	if ( fB >= 0 )
	{
		return qtrue;
	}

	fA = tw->deltaLenSq;
	fDiscriminant = Square(fB) - fA * fC;

	if ( fDiscriminant < 0 )
	{
		return qtrue;
	}

	fEpsilon = fB * (float)SURFACE_CLIP_EPSILON;
	vDelta[2] = 0.0;
	fDeltaLen = Vec3NormalizeTo(vDelta, vNormal);
	fEpsilon = fEpsilon / fDeltaLen;
	fEntry = (-fB - I_sqrt(fDiscriminant)) / fA + fEpsilon;

	if ( fEntry >= trace->fraction )
	{
		return qtrue;
	}

	fTotalHeight = tw->size[2] - tw->radius + fStationaryHalfHeight;
	fHitHeight = tw->extents.start[2] + (fEntry - fEpsilon) * tw->delta[2] - vStationary[2];
	assert(fTotalHeight >= 0);

	return I_fabs(fHitHeight) > fTotalHeight;
}

/*
================
CM_SightTraceCapsuleThroughCapsule

capsule vs. capsule collision (not rotated)
================
*/
int CM_SightTraceCapsuleThroughCapsule( traceWork_t *tw, trace_t *trace )
{
	int i;
	vec3_t top, bottom, starttop, startbottom, endtop, endbottom;
	vec3_t offset, symetricSize[2];
	float radius, halfwidth, halfheight, offs;

	// test trace bounds vs. capsule bounds
	if ( 	   tw->bounds[0][0] > tw->threadInfo.box_model->maxs[0] + RADIUS_EPSILON
	           || tw->bounds[0][1] > tw->threadInfo.box_model->maxs[1] + RADIUS_EPSILON
	           || tw->bounds[0][2] > tw->threadInfo.box_model->maxs[2] + RADIUS_EPSILON
	           || tw->bounds[1][0] < tw->threadInfo.box_model->mins[0] - RADIUS_EPSILON
	           || tw->bounds[1][1] < tw->threadInfo.box_model->mins[1] - RADIUS_EPSILON
	           || tw->bounds[1][2] < tw->threadInfo.box_model->mins[2] - RADIUS_EPSILON
	   )
	{
		return 0;
	}
	// top origin and bottom origin of each sphere at start and end of trace
	VectorCopy( tw->extents.start, starttop );
	starttop[2] = starttop[2] + tw->offsetZ;
	VectorCopy( tw->extents.start, startbottom );
	startbottom[2] = startbottom[2] - tw->offsetZ;
	VectorCopy( tw->extents.end, endtop );
	endtop[2] = endtop[2] + tw->offsetZ;
	VectorCopy( tw->extents.end, endbottom );
	endbottom[2] = endbottom[2] - tw->offsetZ;

	// calculate top and bottom of the capsule spheres to collide with
	for ( i = 0 ; i < 3 ; i++ )
	{
		offset[i] = ( tw->threadInfo.box_model->mins[i] + tw->threadInfo.box_model->maxs[i] ) * 0.5f;
		symetricSize[0][i] = tw->threadInfo.box_model->mins[i] - offset[i];
		symetricSize[1][i] = tw->threadInfo.box_model->maxs[i] - offset[i];
	}
	halfwidth = symetricSize[ 1 ][ 0 ];
	halfheight = symetricSize[ 1 ][ 2 ];
	radius = ( halfwidth > halfheight ) ? halfheight : halfwidth;
	offs = halfheight - radius;
	VectorCopy( offset, top );
	top[2] += offs;
	VectorCopy( offset, bottom );
	bottom[2] -= offs;

	// test for collision between the spheres
	if ( startbottom[2] > top[2] )
	{
		if ( !CM_SightTraceSphereThroughSphere(tw, startbottom, endbottom, top, radius, trace) )
		{
			return -1;
		}

		if ( tw->delta[2] >= 0 )
		{
			return 0;
		}
	}
	else
	{
		if ( starttop[2] < bottom[2] )
		{
			if ( !CM_SightTraceSphereThroughSphere(tw, starttop, endtop, bottom, radius, trace) )
			{
				return -1;
			}

			if ( tw->delta[2] <= 0 )
			{
				return 0;
			}
		}
	}

	// height of the expanded cylinder is the height of both cylinders minus the radius of both spheres
	if ( !CM_SightTraceCylinderThroughCylinder(tw, offset, offs, radius, trace) )
	{
		return -1;
	}

	if ( endbottom[2] > top[2] )
	{
		if ( startbottom[2] <= top[2] )
		{
			if ( !CM_SightTraceSphereThroughSphere(tw, startbottom, endbottom, top, radius, trace) )
			{
				return -1;
			}
		}
	}
	else
	{
		if ( endtop[2] < bottom[2] )
		{
			if ( starttop[2] >= bottom[2] )
			{
				if ( !CM_SightTraceSphereThroughSphere(tw, starttop, endtop, bottom, radius, trace) )
				{
					return -1;
				}
			}
		}
	}

	return 0;
}

/*
==================
CM_SightTraceThroughTree
==================
*/
int CM_SightTraceThroughTree( traceWork_t *tw, int num, const vec3_t p1_, const vec3_t p2, trace_t *trace )
{
	cNode_t *node;
	cplane_t *plane;
	float t1;
	float t2;
	float offset;
	float frc;
	float frac;
	float frac2;
	vec3_t mid;
	int side;
	int hitNum;
	vec3_t p1;
	float diff;
	float absDiff;
	float idist;

	VectorCopy(p1_, p1);

	while ( num >= 0 )
	{
		node = &cm.nodes[num];
		plane = node->plane;

		if ( plane->type < 3 )
		{
			t1 = p1[plane->type] - plane->dist;
			t2 = p2[plane->type] - plane->dist;

			offset = tw->size[plane->type] + (float)SURFACE_CLIP_EPSILON;
		}
		else
		{
			t1 = DotProduct(plane->normal, p1) - plane->dist;
			t2 = DotProduct(plane->normal, p2) - plane->dist;

			if ( tw->isPoint )
			{
				offset = (float)SURFACE_CLIP_EPSILON;
			}
			else
			{
				offset = 2048;
			}
		}

		if ( I_fmin(t1, t2) >= offset )
		{
			num = node->children[0];
			continue;
		}

		if ( I_fmax(t1, t2) <= -offset )
		{
			num = node->children[1];
			continue;
		}

		diff = t2 - t1;
		absDiff = I_fabs(diff);

		if ( absDiff > 0.00000047683716f )
		{
			frc = I_fsel(diff, -t1, t1);
			idist = 1.0f / absDiff;
			frac2 = (frc - offset) * idist;
			frac = (frc + offset) * idist;
			side = I_side(diff);
		}
		else
		{
			side = 0;
			frac = 1.0f;
			frac2 = 0.0f;
		}

		frac = I_fmin(frac, 1.0);

		mid[0] = p1[0] + (p2[0] - p1[0]) * frac;
		mid[1] = p1[1] + (p2[1] - p1[1]) * frac;
		mid[2] = p1[2] + (p2[2] - p1[2]) * frac;

		hitNum = CM_SightTraceThroughTree(tw, node->children[side], p1, mid, trace);

		if ( hitNum )
		{
			return hitNum;
		}

		frac2 = I_fmax(frac2, 0.0f);

		p1[0] = p1[0] + (p2[0] - p1[0]) * frac2;
		p1[1] = p1[1] + (p2[1] - p1[1]) * frac2;
		p1[2] = p1[2] + (p2[2] - p1[2]) * frac2;

		num = node->children[side ^ 1];
	}

	return CM_SightTraceThroughLeaf(tw, &cm.leafs[-1 - num], trace);
}

/*
==================
CM_BoxSightTrace
==================
*/
int CM_BoxSightTrace( int oldHitNum, const vec3_t start, const vec3_t end, const vec3_t mins, const vec3_t maxs, clipHandle_t model, int brushmask )
{
	int i;
	traceWork_t tw;
	vec3_t offset;
	cmodel_t *cmod;
	int hitNum;
	trace_t trace;

	assert(cm.numNodes);
	assert(mins);
	assert(maxs);

	cmod = CM_ClipHandleToModel(model);

	trace.fraction = 1.0;
	trace.startsolid = 0;
	trace.allsolid = 0;

	// set basic parms
	tw.contents = brushmask;

	// adjust so that mins and maxs are always symetric, which
	// avoids some complications with plane expanding of rotated
	// bmodels
	for ( i = 0; i < 3; i++ )
	{
		assert(maxs[i] >= mins[i]);

		offset[i] = (mins[i] + maxs[i]) * 0.5f;
		tw.size[i] = maxs[i] - offset[i];
		tw.extents.start[i] = start[i] + offset[i];
		tw.extents.end[i] = end[i] + offset[i];
		tw.midpoint[i] = (tw.extents.start[i] + tw.extents.end[i]) * 0.5f;
		tw.delta[i] = tw.extents.end[i] - tw.extents.start[i];
		tw.halfDelta[i] = tw.delta[i] * 0.5f;
		tw.halfDeltaAbs[i] = I_fabs(tw.halfDelta[i]);
	}

	CM_CalcTraceEntents(&tw.extents);

	tw.deltaLenSq = VectorLengthSquared(tw.delta);
	tw.deltaLen = I_sqrt(tw.deltaLenSq);

	tw.radius = ( tw.size[0] > tw.size[2] ) ? tw.size[2] : tw.size[0];

	tw.offsetZ = tw.size[2] - tw.radius;

	// calculate bounds
	for ( i = 0; i < 2; i++ )
	{
		if ( tw.extents.start[i] < tw.extents.end[i] )
		{
			tw.bounds[0][i] = tw.extents.start[i] - tw.radius;
			tw.bounds[1][i] = tw.extents.end[i] + tw.radius;
		}
		else
		{
			tw.bounds[0][i] = tw.extents.end[i] - tw.radius;
			tw.bounds[1][i] = tw.extents.start[i] + tw.radius;
		}
	}

	assert(tw.offsetZ >= 0);

	if ( tw.extents.start[2] < tw.extents.end[2] )
	{
		tw.bounds[0][2] = tw.extents.start[2] - tw.offsetZ - tw.radius;
		tw.bounds[1][2] = tw.extents.end[2] + tw.offsetZ + tw.radius;
	}
	else
	{
		tw.bounds[0][2] = tw.extents.end[2] - tw.offsetZ - tw.radius;
		tw.bounds[1][2] = tw.extents.start[2] + tw.offsetZ + tw.radius;
	}

	CM_SetAxialCullOnly(&tw);

	assert(tw.size[0] >= 0);
	assert(tw.size[1] >= 0);
	assert(tw.size[2] >= 0);

	// check for point special case
	tw.isPoint = 0.0f == tw.size[0] + tw.size[1] + tw.size[2];

	assert(tw.offsetZ >= 0);

	tw.radiusOffset[0] = tw.radius;
	tw.radiusOffset[1] = tw.radius;
	tw.radiusOffset[2] = tw.radius + tw.offsetZ;

	CM_GetTrackThreadInfo(&tw.threadInfo);

	//
	// general sweeping through world
	//
	if ( model )
	{
		if ( model == CAPSULE_MODEL_HANDLE )
		{
			if ( (tw.threadInfo.box_brush->contents & tw.contents) )
			{
				hitNum = CM_SightTraceCapsuleThroughCapsule(&tw, &trace);
			}
			else
			{
				hitNum = 0;
			}
		}
		else
		{
			hitNum = CM_SightTraceThroughLeaf(&tw, &cmod->leaf, &trace);
		}
	}
	else
	{
		hitNum = 0;

		if ( oldHitNum > 0 )
		{
			oldHitNum--;

			if ( oldHitNum < cm.numBrushes )
			{
				hitNum = CM_SightTraceThroughBrush(&tw, &cm.brushes[oldHitNum]);
			}
		}

		if ( !hitNum )
		{
			hitNum = CM_SightTraceThroughTree(&tw, 0, tw.extents.start, tw.extents.end, &trace);
		}
	}

	return hitNum;
}

/*
==================
CM_TransformedBoxSightTrace
==================
*/
int CM_TransformedBoxSightTrace( int hitNum, const vec3_t start, const vec3_t end,
                                 const vec3_t mins, const vec3_t maxs,
                                 clipHandle_t model, int brushmask,
                                 const vec3_t origin, const vec3_t angles )
{
	vec3_t start_l, end_l;
	qboolean rotated;
	vec3_t offset;
	vec3_t symetricSize[2];
	vec3_t matrix[3];
	int i;
	float halfwidth, halfheight;
	qboolean v10;

	assert(mins);
	assert(maxs);

	// adjust so that mins and maxs are always symetric, which
	// avoids some complications with plane expanding of rotated
	// bmodels
	for ( i = 0 ; i < 3 ; i++ )
	{
		offset[i] = ( mins[i] + maxs[i] ) * 0.5f;
		symetricSize[0][i] = mins[i] - offset[i];
		symetricSize[1][i] = maxs[i] - offset[i];
		start_l[i] = start[i] + offset[i];
		end_l[i] = end[i] + offset[i];
	}

	// subtract origin offset
	VectorSubtract( start_l, origin, start_l );
	VectorSubtract( end_l, origin, end_l );

	v10 = qfalse;
	if ( ( angles[0] || angles[1] || angles[2] ) )
	{
		v10 = qtrue;
	}
	rotated = v10;

	halfwidth = symetricSize[1][0];
	halfheight = symetricSize[1][2];

	if ( rotated )
	{
		// rotation on trace line (start-end) instead of rotating the bmodel
		// NOTE: This is still incorrect for bounding boxes because the actual bounding
		//		 box that is swept through the model is not rotated. We cannot rotate
		//		 the bounding box or the bmodel because that would make all the brush
		//		 bevels invalid.
		//		 However this is correct for capsules since a capsule itself is rotated too.
		CreateRotationMatrix( angles, matrix );
		RotatePoint( start_l, matrix );
		RotatePoint( end_l, matrix );
	}

	return CM_BoxSightTrace( hitNum, start_l, end_l, symetricSize[0], symetricSize[1], model, brushmask );
}

