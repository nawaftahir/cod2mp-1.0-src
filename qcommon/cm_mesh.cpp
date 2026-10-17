#include "qcommon.h"
#include "cm_local.h"

bool CM_CullBox( traceWork_t *tw, const vec3_t origin, const vec3_t halfSize );
void CM_PositionTestCapsuleInTriangle( traceWork_t *tw, CollisionTriangle_t *tri, trace_t *trace );
static inline void CM_TracePointThroughTriangle( traceWork_t *tw, CollisionTriangle_t *tri, trace_t *trace );
static inline void CM_TraceCapsuleThroughTriangle( traceWork_t *tw, CollisionTriangle_t *tri, float offsetZ, trace_t *trace );
static inline void CM_TraceCapsuleThroughBorder( traceWork_t *tw, CollisionBorder *border, trace_t *trace );
static void CM_TraceThroughAabbTree_r( traceWork_t *tw, CollisionAabbTree_t *aabbTree, trace_t *trace );
static void CM_PositionTestInAabbTree_r( traceWork_t *tw, CollisionAabbTree_t *aabbTree, trace_t *trace );
void CM_TraceThroughAabbTree( traceWork_t *tw, CollisionAabbTree_t *aabbTree, trace_t *trace );
void CM_SightTraceThroughAabbTree( traceWork_t *tw, CollisionAabbTree_t *aabbTree, trace_t *trace );
void CM_MeshTestInLeaf( traceWork_t *tw, cLeaf_t *leaf, trace_t *trace );
bool CM_RayIntersectTriangle( const float *orig, const float *dir, const float *vert0, const float *vert1, const float *vert2, float *t, float *u, float *v );

/*
==================
CM_CullBox
==================
*/
bool CM_CullBox( traceWork_t *tw, const vec3_t origin, const vec3_t halfSize )
{
	vec3_t distorig;
	vec3_t mid;

	VectorSubtract(tw->midpoint, origin, distorig);
	VectorAdd(halfSize, tw->size, mid);

	if ( I_fabs(distorig[0]) > mid[0] + tw->halfDeltaAbs[0] )
	{
		return true;
	}

	if ( I_fabs(distorig[1]) > mid[1] + tw->halfDeltaAbs[1] )
	{
		return true;
	}

	if ( I_fabs(distorig[2]) > mid[2] + tw->halfDeltaAbs[2] )
	{
		return true;
	}

	if ( tw->axialCullOnly )
	{
		return false;
	}

	if ( I_fabs(tw->halfDelta[1] * distorig[2] - tw->halfDelta[2] * distorig[1]) > mid[1] * tw->halfDeltaAbs[2] + mid[2] * tw->halfDeltaAbs[1] )
	{
		return true;
	}

	if ( I_fabs(tw->halfDelta[2] * distorig[0] - tw->halfDelta[0] * distorig[2]) > mid[2] * tw->halfDeltaAbs[0] + mid[0] * tw->halfDeltaAbs[2] )
	{
		return true;
	}

	if ( I_fabs(tw->halfDelta[0] * distorig[1] - tw->halfDelta[1] * distorig[0]) > mid[0] * tw->halfDeltaAbs[1] + mid[1] * tw->halfDeltaAbs[0] )
	{
		return true;
	}

	return false;
}

/*
==================
CM_TracePointThroughTriangle
==================
*/
static inline void CM_TracePointThroughTriangle( traceWork_t *tw, CollisionTriangle_t *tri, trace_t *trace )
{
	float t;
	float projTriAreaScaledByTraceLenX2;
	float frac;
	float hitFrac;
	vec3_t triNormalScaledByAreaX2;
	float v;
	float negativeU;

	projTriAreaScaledByTraceLenX2 = DotProduct(tw->extents.end, tri->plane) - tri->plane[3];

	if ( projTriAreaScaledByTraceLenX2 >= 0.0f )
	{
		return;
	}

	t = DotProduct(tw->extents.start, tri->plane) - tri->plane[3];

	if ( t <= 0.0f )
	{
		return;
	}

	frac = (t - (float)SURFACE_CLIP_EPSILON) / (t - projTriAreaScaledByTraceLenX2);
	frac = I_fmax(frac, 0.0f);

	if ( frac >= trace->fraction )
	{
		return;
	}

	hitFrac = t / (t - projTriAreaScaledByTraceLenX2);
	VectorMA(tw->extents.start, hitFrac, tw->delta, triNormalScaledByAreaX2);
	v = DotProduct(triNormalScaledByAreaX2, tri->svec) - tri->svec[3];

	if ( v < -0.001f || v > 1.001f )
	{
		return;
	}

	negativeU = DotProduct(triNormalScaledByAreaX2, tri->tvec) - tri->tvec[3];

	if ( negativeU < -0.001f || v + negativeU > 1.001f )
	{
		return;
	}

	trace->fraction = frac;
	assert(trace->fraction >= 0 && trace->fraction <= 1.0f);
	VectorCopy(tri->plane, trace->normal);
}

/*
==================
CM_PositionTestCapsuleInTriangle
==================
*/
void CM_PositionTestCapsuleInTriangle( traceWork_t *tw, CollisionTriangle_t *tri, trace_t *trace )
{
	int i;
	vec3_t start;
	float s;
	float t;
	int vertToCheck;
	vec3_t hitDelta;
	float d;
	int edgeIndex;
	CollisionEdge_t *edge;
	int vertId;
	float *vert;
	float radiusNegU;
	float radius;
	float hitFrac;
	float planeDist;
	vec3_t sphereStart;
	float offsetZ2;
	float hitDist;
	float sScale;
	float tScale;
	float lengthSq;
	float offsetZ;

	offsetZ = tw->offsetZ;

	if ( tri->plane[2] < 0.0f )
	{
		offsetZ = -offsetZ;
	}

	VectorCopy(tw->extents.start, sphereStart);
	sphereStart[2] = sphereStart[2] - offsetZ;

	radius = tw->radius;
	hitDist = DotProduct(sphereStart, tri->plane) - tri->plane[3];

	if ( hitDist >= radius )
	{
		return;
	}

	radiusNegU = -radius;

	if ( hitDist <= radiusNegU )
	{
		offsetZ2 = offsetZ * 2.0f;
		planeDist = hitDist + offsetZ2 * tri->plane[2];

		if ( planeDist <= radiusNegU )
		{
			return;
		}

		hitFrac = (radiusNegU - hitDist) / tri->plane[2];
		sScale = DotProduct(sphereStart, tri->svec) - tri->svec[3];
		tScale = DotProduct(sphereStart, tri->tvec) - tri->tvec[3];
		s = sScale + hitFrac * tri->svec[2];

		if ( s >= 0.0f && (t = tScale + hitFrac * tri->tvec[2], t >= 0.0f) && s + t <= 1.0f )
		{
			trace->startsolid = qtrue;
			trace->allsolid = qtrue;
			trace->fraction = 0.0f;
		}
		else
		{
			if ( planeDist < radius )
			{
				hitFrac = offsetZ2;
			}
			else
			{
				hitFrac = (radius - hitDist) / tri->plane[2];
			}
			s = sScale + hitFrac * tri->svec[2];

			if ( s < 0.0f )
			{
				return;
			}

			t = tScale + hitFrac * tri->tvec[2];

			if ( t < 0.0f )
			{
				return;
			}

			if ( s + t > 1.0f )
			{
				return;
			}

			trace->startsolid = qtrue;
			trace->allsolid = qtrue;
			trace->fraction = 0.0f;
			return;
		}
	}
	else
	{
		vertToCheck = 0;
		VectorMA(sphereStart, -hitDist, tri->plane, start);

		s = DotProduct(start, tri->svec) - tri->svec[3];
		t = DotProduct(start, tri->tvec) - tri->tvec[3];

		vertToCheck = vertToCheck | ((s + t > 1.0f) ? 1 : 0);
		vertToCheck = vertToCheck | ((s < 0.0f) << 1);
		vertToCheck = vertToCheck | ((t < 0.0f) << 2);

		if ( vertToCheck == 0 )
		{
			trace->startsolid = qtrue;
			trace->allsolid = qtrue;
			trace->fraction = 0.0f;
		}
		else
		{
			for ( i = 0; i < 3; i++ )
			{
				if ( vertToCheck & (1 << i) )
				{
					edgeIndex = tri->edges[i];

					if ( edgeIndex < 0 )
					{
						continue;
					}

					edge = &cm.edges[edgeIndex];

					VectorSubtract(sphereStart, edge->origin, hitDelta);
					d = DotProduct(hitDelta, edge->axis[2]);

					if ( I_fabs(d - 0.5f) <= 0.5f )
					{
						lengthSq = VectorLengthSquared(edge->axis[2]);
						d = d / lengthSq;
						VectorMA(hitDelta, -d, edge->axis[2], hitDelta);

						if ( VectorLengthSquared(hitDelta) < Square(tw->radius) )
						{
							trace->startsolid = qtrue;
							trace->allsolid = qtrue;
							trace->fraction = 0.0f;
							return;
						}
					}
				}
				else
				{
					vertId = tri->verts[i];

					if ( vertId < 0 )
					{
						continue;
					}

					vert = cm.verts[vertId];

					VectorSubtract(sphereStart, vert, hitDelta);

					if ( VectorLengthSquared(hitDelta) < Square(tw->radius) )
					{
						trace->startsolid = qtrue;
						trace->allsolid = qtrue;
						trace->fraction = 0.0f;
						return;
					}
				}
			}
		}
	}
}

/*
==================
CM_TraceCapsuleThroughTriangle
==================
*/
static inline void CM_TraceCapsuleThroughTriangle( traceWork_t *tw, CollisionTriangle_t *tri, float offsetZ, trace_t *trace )
{
	int i;
	float radiusNegU;
	float radius;
	float hitDist;
	float scaledPlaneDist;
	float hitFrac;
	vec3_t start;
	float s;
	float t;
	int vertToCheck;
	float offsetLenSq;
	vec3_t hitDelta;
	vec3_t deltaOffset;
	float d;
	vec3_t offset;
	float discriminant;
	float a;
	float b;
	float c;
	int edgeIndex;
	CollisionEdge_t *edge;
	int vertId;
	float *vert;
	vec3_t sphereStart;
	vec3_t endpos;
	float planeDist;
	float offsetZ2;
	float sScale;
	float tScale;
	float startDist;

	VectorCopy(tw->extents.end, endpos);
	endpos[2] = endpos[2] - offsetZ;

	radius = tw->radius + 0.125f;
	scaledPlaneDist = DotProduct(endpos, tri->plane) - tri->plane[3];

	if ( scaledPlaneDist >= radius )
	{
		return;
	}

	VectorCopy(tw->extents.start, sphereStart);
	sphereStart[2] = sphereStart[2] - offsetZ;

	hitDist = DotProduct(sphereStart, tri->plane) - tri->plane[3];
	startDist = hitDist - scaledPlaneDist;

	if ( startDist <= 0.000099999997f )
	{
		return;
	}

	{
		{
			radiusNegU = -radius;

			if ( hitDist <= radiusNegU )
			{
				offsetZ2 = offsetZ * 2.0f;
				planeDist = hitDist + offsetZ2 * tri->plane[2];

				if ( planeDist <= radiusNegU )
				{
					return;
				}

				hitFrac = (radiusNegU - hitDist) / tri->plane[2];
				sScale = DotProduct(sphereStart, tri->svec) - tri->svec[3];
				tScale = DotProduct(sphereStart, tri->tvec) - tri->tvec[3];
				s = sScale + hitFrac * tri->svec[2];

				if ( s >= 0.0f && (t = tScale + hitFrac * tri->tvec[2], t >= 0.0f) && s + t <= 1.0f )
				{
					VectorCopy(tri->plane, trace->normal);
					trace->fraction = 0.0f;
					trace->startsolid = qtrue;
					return;
				}

				if ( planeDist < radius )
				{
					hitFrac = offsetZ2;
				}
				else
				{
					hitFrac = (radius - hitDist) / tri->plane[2];
				}

				s = sScale + hitFrac * tri->svec[2];

				if ( s < 0.0f )
				{
					return;
				}

				t = tScale + hitFrac * tri->tvec[2];

				if ( t < 0.0f )
				{
					return;
				}

				if ( s + t > 1.0f )
				{
					return;
				}

				VectorCopy(tri->plane, trace->normal);
				trace->fraction = 0.0f;
				trace->startsolid = qtrue;
				return;
			}
			else
			{
				if ( hitDist - radius <= 0.0f )
				{
					hitFrac = 0.0f;
					VectorCopy(sphereStart, start);
				}
				else
				{
					hitFrac = (hitDist - radius) / startDist;

					if ( hitFrac > trace->fraction )
					{
						return;
					}

					VectorMA(sphereStart, hitFrac, tw->delta, start);
				}

				s = DotProduct(start, tri->svec) - tri->svec[3];
				t = DotProduct(start, tri->tvec) - tri->tvec[3];

				vertToCheck = s + t > 1.0f;
				vertToCheck = vertToCheck | ((s < 0.0f) << 1);
				vertToCheck = vertToCheck | ((t < 0.0f) << 2);

				if ( vertToCheck == 0 )
				{
					VectorCopy(tri->plane, trace->normal);
					trace->fraction = hitFrac;

					if ( hitDist < tw->radius )
					{
						trace->startsolid = qtrue;
					}

					return;
				}

				for ( i = 0; i < 3; i++ )
				{
					if ( vertToCheck & (1 << i) )
					{
						edgeIndex = tri->edges[i];

						if ( edgeIndex < 0 )
						{
							continue;
						}

						if ( tw->threadInfo.edges[edgeIndex] == tw->threadInfo.checkcount )
						{
							continue;
						}

						tw->threadInfo.edges[edgeIndex] = tw->threadInfo.checkcount;
						edge = &cm.edges[edgeIndex];

						VectorSubtract(sphereStart, edge->origin, hitDelta);
						offset[0] = DotProduct(hitDelta, edge->axis[0]);
						offset[1] = DotProduct(hitDelta, edge->axis[1]);
						deltaOffset[0] = DotProduct(tw->delta, edge->axis[0]);
						deltaOffset[1] = DotProduct(tw->delta, edge->axis[1]);
						b = Dot2Product(deltaOffset, offset);

						if ( b >= 0.0f )
						{
							continue;
						}

						offsetLenSq = Dot2Product(offset, offset);
						c = offsetLenSq - Square(radius);

						if ( c <= 0.0f )
						{
							d = DotProduct(hitDelta, edge->axis[2]);

							if ( I_fabs(d - 0.5f) > 0.5f )
							{
								continue;
							}

							{
								VectorScale(edge->axis[0], offset[0], trace->normal);
								VectorMA(trace->normal, offset[1], edge->axis[1], trace->normal);
								Vec3Normalize(trace->normal);

								if ( tri->plane[2] >= 0.7f && trace->normal[2] >= 0.0f && trace->normal[2] < 0.7f && sphereStart[2] > endpos[2] )
								{
									VectorCopy(tri->plane, trace->normal);
								}

								trace->fraction = 0.0f;

								if ( offsetLenSq < Square(tw->radius) )
								{
									trace->startsolid = qtrue;
								}

								return;
							}
						}
						else
						{
							a = Vec2Multiply(deltaOffset);
							discriminant = Square(b) - a * c;

							if ( discriminant <= 0.0f )
							{
								continue;
							}

							hitFrac = (-I_sqrt(discriminant) - b) / a;

							if ( hitFrac < trace->fraction )
							{
								VectorMA(hitDelta, hitFrac, tw->delta, hitDelta);
								start[2] = DotProduct(hitDelta, edge->axis[2]);

								if ( I_fabs(start[2] - 0.5f) > 0.5f )
								{
									continue;
								}

								start[0] = (hitFrac * deltaOffset[0] + offset[0]) / radius;
								start[1] = (hitFrac * deltaOffset[1] + offset[1]) / radius;

								VectorScale(edge->axis[0], start[0], trace->normal);
								VectorMA(trace->normal, start[1], edge->axis[1], trace->normal);

								if ( tri->plane[2] >= 0.7f && trace->normal[2] >= 0.0f && trace->normal[2] < 0.7f && sphereStart[2] > endpos[2] )
								{
									VectorCopy(tri->plane, trace->normal);
								}

								trace->fraction = hitFrac;
							}
						}
					}
					else
					{
						vertId = tri->verts[i];

						if ( vertId < 0 )
						{
							continue;
						}

						if ( tw->threadInfo.verts[vertId] == tw->threadInfo.checkcount )
						{
							continue;
						}

						tw->threadInfo.verts[vertId] = tw->threadInfo.checkcount;
						vert = cm.verts[vertId];

						VectorSubtract(sphereStart, vert, hitDelta);
						b = DotProduct(tw->delta, hitDelta);

						if ( b >= 0.0f )
						{
							continue;
						}

						offsetLenSq = DotProduct(hitDelta, hitDelta);
						c = offsetLenSq - Square(radius);

						if ( c <= 0.0f )
						{
							hitFrac = 1.0f / I_sqrt(offsetLenSq);
							VectorScale(hitDelta, hitFrac, trace->normal);

							if ( tri->plane[2] >= 0.7f && trace->normal[2] >= 0.0f && trace->normal[2] < 0.7f && sphereStart[2] > endpos[2] )
							{
								VectorCopy(tri->plane, trace->normal);
							}

							trace->fraction = 0.0f;

							if ( offsetLenSq < Square(tw->radius) )
							{
								trace->startsolid = qtrue;
							}

							return;
						}

						a = tw->deltaLenSq;
						discriminant = Square(b) - a * c;

						if ( discriminant < 0.0f )
						{
							continue;
						}

						hitFrac = (-I_sqrt(discriminant) - b) / a;

						if ( hitFrac < trace->fraction )
						{
							VectorMA(hitDelta, hitFrac, tw->delta, trace->normal);
							VectorScale(trace->normal, 1.0f / radius, trace->normal);

							if ( tri->plane[2] >= 0.7f && trace->normal[2] >= 0.0f && trace->normal[2] < 0.7f && sphereStart[2] > endpos[2] )
							{
								VectorCopy(tri->plane, trace->normal);
							}

							trace->fraction = hitFrac;
						}
					}
				}
			}
		}
	}
}

/*
==================
CM_TraceCapsuleThroughBorder
==================
*/
static inline void CM_TraceCapsuleThroughBorder( traceWork_t *tw, CollisionBorder *border, trace_t *trace )
{
	float radius;
	float d;
	float traceDeltaDot;
	float t;
	float s;
	vec3_t endpos;
	float z;
	vec2_t offset;
	float b;
	float offsetLenSq;
	float discriminant;
	float c;
	vec3_t edgePoint;

	traceDeltaDot = Dot2Product(tw->delta, border->distEq);

	if ( traceDeltaDot >= 0.0f )
	{
		return;
	}

	radius = tw->radius + 0.125f;

	d = Dot2Product(tw->extents.start, border->distEq) - border->distEq[2];
	t = (radius - d) / traceDeltaDot;

	if ( t >= trace->fraction || t * tw->deltaLen < -radius )
	{
		return;
	}

	VectorMA(tw->extents.start, t, tw->delta, endpos);
	s = border->distEq[1] * endpos[0] - border->distEq[0] * endpos[1] - border->start;

	if ( s < 0.0f )
	{
		edgePoint[0] = border->distEq[1] * border->start + border->distEq[0] * border->distEq[2];
		edgePoint[1] = border->distEq[1] * border->distEq[2] - border->distEq[0] * border->start;

		Vector2Subtract(tw->extents.start, edgePoint, offset);
		b = Dot2Product(tw->delta, offset);

		if ( b >= 0.0f )
		{
			return;
		}

		offsetLenSq = Dot2Product(offset, offset);
		c = offsetLenSq - Square(radius);

		if ( c < 0.0f )
		{
			edgePoint[2] = border->zBase;

			if ( I_fabs(edgePoint[2] - tw->extents.start[2]) > tw->offsetZ )
			{
				return;
			}

			VectorSet(trace->normal, border->distEq[0], border->distEq[1], 0.0f);
			trace->fraction = 0.0f;

			if ( offsetLenSq < Square(tw->radius) )
			{
				trace->startsolid = qtrue;
			}

			return;
		}

		discriminant = Square(b) - tw->deltaLenSq * c;

		if ( discriminant < 0.0f )
		{
			return;
		}

		t = (-b - I_sqrt(discriminant)) / tw->deltaLenSq;

		if ( t >= trace->fraction || t <= 0.0f )
		{
			return;
		}

		VectorMA(tw->extents.start, t, tw->delta, endpos);
		s = 0.0f;

		goto tail;
	}
	else
	{
		if ( s > border->length )
		{
			edgePoint[0] = border->distEq[1] * (border->start + border->length) + border->distEq[0] * border->distEq[2];
			edgePoint[1] = border->distEq[1] * border->distEq[2] - border->distEq[0] * (border->start + border->length);

			Vector2Subtract(tw->extents.start, edgePoint, offset);
			b = Dot2Product(tw->delta, offset);

			if ( b >= 0.0f )
			{
				return;
			}

			offsetLenSq = Dot2Product(offset, offset);
			c = offsetLenSq - Square(radius);

			if ( c < 0.0f )
			{
				edgePoint[2] = border->zBase + border->zSlope * border->length;

				if ( I_fabs(tw->extents.start[2] - edgePoint[2]) > tw->offsetZ )
				{
					return;
				}

				VectorSet(trace->normal, border->distEq[0], border->distEq[1], 0.0f);
				trace->fraction = 0.0f;

				if ( offsetLenSq < Square(tw->radius) )
				{
					trace->startsolid = qtrue;
				}

				return;
			}

			discriminant = Square(b) - tw->deltaLenSq * c;

			if ( discriminant < 0.0f )
			{
				return;
			}

			t = (-b - I_sqrt(discriminant)) / tw->deltaLenSq;

			if ( t >= trace->fraction || t <= 0.0f )
			{
				return;
			}

			VectorMA(tw->extents.start, t, tw->delta, endpos);
			s = border->length;

			goto tail;
		}
		else
		{
			if ( t < 0.0f )
			{
				t = 0.0f;
			}
		}
	}

tail:
	z = border->zBase + s * border->zSlope;

	if ( I_fabs(endpos[2] - z) > tw->offsetZ )
	{
		return;
	}

	trace->fraction = t;
	VectorSet(trace->normal, border->distEq[0], border->distEq[1], 0.0f);
}

/*
==================
CM_TraceThroughAabbTree_r
==================
*/
static void CM_TraceThroughAabbTree_r( traceWork_t *tw, CollisionAabbTree_t *aabbTree, trace_t *trace )
{
	int childIndex;
	CollisionAabbTree_t *child;
	int partitionIndex;
	CollisionPartition *partition;
	int16_t checkcount;
	int borderIndex;

	if ( CM_CullBox(tw, aabbTree->origin, aabbTree->halfSize) )
	{
		return;
	}

	if ( aabbTree->childCount )
	{
		for ( childIndex = 0, child = &cm.aabbTrees[aabbTree->u.firstChildIndex]; childIndex < aabbTree->childCount; childIndex++, child++ )
		{
			CM_TraceThroughAabbTree_r(tw, child, trace);
		}

		return;
	}

	partitionIndex = aabbTree->u.firstChildIndex;
	checkcount = tw->threadInfo.checkcount;

	if ( tw->threadInfo.partitions[partitionIndex] == checkcount )
	{
		return;
	}

	tw->threadInfo.partitions[partitionIndex] = checkcount;
	partition = &cm.partitions[partitionIndex];

	if ( tw->isPoint )
	{
		int i;

		for ( i = 0; i < partition->triCount; i++ )
		{
			CM_TracePointThroughTriangle(tw, &partition->tris[i], trace);
		}

		return;
	}

	{
	int i;
	CollisionTriangle_s *tri;

	for ( i = 0; i < partition->triCount; i++ )
	{
		tri = &partition->tris[i];
		CM_TraceCapsuleThroughTriangle(tw, tri, tw->offsetZ, trace);

		if ( !( tri->plane[2] < 0.0f ) )
		{
			continue;
		}

		CM_TraceCapsuleThroughTriangle(tw, tri, -tw->offsetZ, trace);
	}
	}

	if ( (tw->delta[0] || tw->delta[1] ) && tw->offsetZ != 0.0f )
	{
		for ( borderIndex = 0; borderIndex < partition->borderCount; borderIndex++ )
		{
			CM_TraceCapsuleThroughBorder(tw, &partition->borders[borderIndex], trace);
		}
	}
}

/*
==================
CM_PositionTestInAabbTree_r
==================
*/
static void CM_PositionTestInAabbTree_r( traceWork_t *tw, CollisionAabbTree_t *aabbTree, trace_t *trace )
{
	int childIndex;
	CollisionAabbTree_t *child;
	int partitionIndex;
	CollisionPartition *partition;
	int16_t checkStamp;
	int triCount;

	if ( CM_CullBox(tw, aabbTree->origin, aabbTree->halfSize) )
	{
		return;
	}

	if ( aabbTree->childCount )
	{
		for ( childIndex = 0, child = &cm.aabbTrees[aabbTree->u.firstChildIndex]; childIndex < aabbTree->childCount; childIndex++, child++ )
		{
			CM_PositionTestInAabbTree_r(tw, child, trace);
		}

		return;
	}

	partitionIndex = aabbTree->u.firstChildIndex;
	checkStamp = tw->threadInfo.checkcount;

	if ( tw->threadInfo.partitions[partitionIndex] == checkStamp )
	{
		return;
	}

	tw->threadInfo.partitions[partitionIndex] = checkStamp;
	partition = &cm.partitions[partitionIndex];

	for ( triCount = 0; triCount < partition->triCount; triCount++ )
	{
		CM_PositionTestCapsuleInTriangle(tw, &partition->tris[triCount], trace);
	}
}

/*
==================
CM_TraceThroughAabbTree
==================
*/
void CM_TraceThroughAabbTree( traceWork_t *tw, CollisionAabbTree_t *aabbTree, trace_t *trace )
{
	float oldFraction;
	dmaterial_t *materialInfo;

	materialInfo = &cm.materials[aabbTree->materialIndex];

	if ( !(tw->contents & materialInfo->contentFlags) )
	{
		return;
	}

	oldFraction = trace->fraction;
	CM_TraceThroughAabbTree_r(tw, aabbTree, trace);

	if ( trace->fraction < oldFraction )
	{
		trace->surfaceFlags = materialInfo->surfaceFlags;
		trace->contents = materialInfo->contentFlags;
		trace->material = materialInfo;
	}
}

/*
==================
CM_SightTraceThroughAabbTree
==================
*/
void CM_SightTraceThroughAabbTree( traceWork_t *tw, CollisionAabbTree_t *aabbTree, trace_t *trace )
{
	if ( !(tw->contents & cm.materials[aabbTree->materialIndex].contentFlags) )
	{
		return;
	}

	CM_TraceThroughAabbTree(tw, aabbTree, trace);
}

/*
==================
CM_MeshTestInLeaf
==================
*/
void CM_MeshTestInLeaf( traceWork_t *tw, cLeaf_t *leaf, trace_t *trace )
{
	int k;
	dmaterial_t *materialInfo;
	CollisionAabbTree_t *aabbTree;

	assert(!tw->isPoint);
	assert(!trace->allsolid);

	for ( k = 0; k < leaf->collAabbCount; k++ )
	{
		aabbTree = &cm.aabbTrees[k + leaf->firstCollAabbIndex];
		materialInfo = &cm.materials[aabbTree->materialIndex];

		if ( !(tw->contents & materialInfo->contentFlags) )
		{
			continue;
		}

		CM_PositionTestInAabbTree_r(tw, aabbTree, trace);

		if ( !trace->allsolid )
		{
			continue;
		}

		trace->surfaceFlags = materialInfo->surfaceFlags;
		trace->contents = materialInfo->contentFlags;
		trace->material = materialInfo;
		return;
	}
}

/*
==================
CM_RayIntersectTriangle
==================
*/
bool CM_RayIntersectTriangle( const float *orig, const float *dir, const float *vert0, const float *vert1, const float *vert2, float *t, float *u, float *v )
{
	float uDet;
	float vDet;
	vec3_t edge1;
	vec3_t edge2;
	vec3_t tvec;
	vec3_t pvec;
	vec3_t qvec;
	float det;

	VectorSubtract(vert1, vert0, edge1);
	VectorSubtract(vert2, vert0, edge2);
	Vec3Cross(dir, edge2, pvec);
	det = DotProduct(edge1, pvec);
	if ( det < 0.001f )
	{
		return false;
	}

	VectorSubtract(orig, vert0, tvec);
	uDet = DotProduct(tvec, pvec);
	if ( uDet < 0.0f || uDet > det )
	{
		return false;
	}

	Vec3Cross(tvec, edge1, qvec);
	vDet = DotProduct(dir, qvec);
	if ( vDet < 0.0f || uDet + vDet > det )
	{
		return false;
	}

	*t = DotProduct(edge2, qvec) / det;
	if ( u )
	{
		*u = uDet / det;
	}
	if ( v )
	{
		*v = vDet / det;
	}
	return true;
}

