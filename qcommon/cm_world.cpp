#include "qcommon.h"
#include "cm_local.h"
#include "../server/server.h"

#define SECTOR_HEAD 1

static cm_world_t cm_world;

static unsigned short CM_AllocWorldSector( vec2_t mins, vec2_t maxs );
void CM_ClearWorld();
void CM_LinkWorld();
void CM_UnlinkEntity( svEntity_t *ent );
static void CM_AddEntityToNode( svEntity_t *ent, unsigned short childNodeIndex );
static void CM_AddStaticModelToNode( cStaticModel_t *staticModel, unsigned short childNodeIndex );
static void CM_SortNode( unsigned short nodeIndex, vec2_t mins, vec2_t maxs );
void CM_LinkEntity( svEntity_t *ent, vec3_t absmin, vec3_t absmax, clipHandle_t clipHandle );
void CM_LinkStaticModel( cStaticModel_t *staticModel );
void CM_LinkAllStaticModels();
static void CM_AreaEntities_r( unsigned short nodeIndexIn, areaParms_t *ap );
int CM_AreaEntities( const vec3_t mins, const vec3_t maxs, int *entityList, int maxcount, int contentmask );
static void CM_PointTraceStaticModels_r( locTraceWork_t *tw, unsigned short nodeIndex, const vec4_t p1_, const vec4_t p2, trace_t *trace );
void CM_PointTraceStaticModels( trace_t *results, const vec3_t start, const vec3_t end, int contentmask );
static qboolean CM_PointTraceStaticModelsComplete_r( staticmodeltrace_t *clip, unsigned short nodeIndex, const vec3_t p1_, const vec3_t p2 );
qboolean CM_PointTraceStaticModelsComplete( const vec3_t start, const vec3_t end, int contentmask );
static void CM_ClipMoveToEntities_r( moveclip_t *clip, unsigned short nodeIndex, const vec4_t p1, const vec4_t p2, trace_t *trace );
void CM_ClipMoveToEntities( moveclip_t *clip, trace_t *trace );
static int CM_ClipSightTraceToEntities_r( sightclip_t *clip, unsigned short nodeIndex, const vec3_t p1, const vec3_t p2 );
int CM_ClipSightTraceToEntities( sightclip_t *clip );
static void CM_PointTraceToEntities_r( pointtrace_t *clip, unsigned short nodeIndex, const vec4_t p1, const vec4_t p2, trace_t *trace );
void CM_PointTraceToEntities( pointtrace_t *clip, trace_t *trace );
static int CM_PointSightTraceToEntities_r( sightpointtrace_t *clip, unsigned short nodeIndex, const vec3_t p1, const vec3_t p2 );
int CM_PointSightTraceToEntities( sightpointtrace_t *clip );

/*
================
CM_AllocWorldSector
================
*/
static unsigned short CM_AllocWorldSector( vec2_t mins, vec2_t maxs )
{
	vec2_t size;
	unsigned short nodeIndex;
	unsigned short axis;
	worldSector_t *node;

	nodeIndex = cm_world.freeHead;

	if ( !nodeIndex )
	{
		return 0;
	}

	Vector2Subtract(maxs, mins, size);

	axis = size[0] <= size[1];

	if ( size[axis] <= 512 )
	{
		return 0;
	}

	node = &cm_world.sectors[nodeIndex];

	assert(!node->contents.contentsStaticModels);
	assert(!node->contents.contentsEntities);
	assert(!node->contents.entities);
	assert(!node->contents.staticModels);

	cm_world.freeHead = node->tree.parent;

	node->tree.axis = axis;
	node->tree.dist = (maxs[axis] + mins[axis]) * 0.5f;

	assert(!node->tree.child[0]);
	assert(!node->tree.child[1]);

	return nodeIndex;
}

/*
================
CM_ClearWorld
================
*/
void CM_ClearWorld()
{
	int i;
	vec2_t bounds;

	memset(&cm_world, 0, sizeof(cm_world));

	CM_ModelBounds(0, cm_world.mins, cm_world.maxs);

	cm_world.freeHead = 2;

	for ( i = 2; (unsigned int)i < AREA_NODES - 1; i++ )
	{
		cm_world.sectors[i].tree.parent = i + 1;
	}

	cm_world.sectors[AREA_NODES - 1].tree.parent = 0;

	Vector2Subtract(cm_world.maxs, cm_world.mins, bounds);

	cm_world.sectors[SECTOR_HEAD].tree.axis = bounds[0] <= bounds[1];
	cm_world.sectors[SECTOR_HEAD].tree.dist = (cm_world.maxs[cm_world.sectors[SECTOR_HEAD].tree.axis] + cm_world.mins[cm_world.sectors[SECTOR_HEAD].tree.axis]) * 0.5f;

	assert(!cm_world.sectors[SECTOR_HEAD].tree.child[0]);
	assert(!cm_world.sectors[SECTOR_HEAD].tree.child[1]);
}

/*
================
CM_LinkWorld
================
*/
void CM_LinkWorld()
{
	CM_ClearWorld();
	CM_LinkAllStaticModels();
}

/*
===============
CM_UnlinkEntity
===============
*/
void CM_UnlinkEntity( svEntity_t *ent )
{
	svEntity_t *scan;
	worldSector_t *node;
	unsigned short parentNodeIndex;
	int contents;
	unsigned short nodeIndex;

	nodeIndex = ent->worldSector;

	if ( !nodeIndex )
	{
		return;     // not linked in anywhere
	}

	node = &cm_world.sectors[nodeIndex];
	ent->worldSector = 0;

	assert(node->contents.entities);

	if ( &sv.svEntities[node->contents.entities - 1] == ent )
	{
		node->contents.entities = ent->nextEntityInWorldSector;
	}
	else
	{
		for ( scan = &sv.svEntities[node->contents.entities - 1] ; ; scan = &sv.svEntities[scan->nextEntityInWorldSector - 1] )
		{
			if ( &sv.svEntities[scan->nextEntityInWorldSector - 1] == ent )
			{
				assert(scan->nextEntityInWorldSector);
				scan->nextEntityInWorldSector = ent->nextEntityInWorldSector;
				break;
			}
		}
	}

	while ( !node->contents.entities && !node->contents.staticModels && !node->tree.child[0] && !node->tree.child[1] )
	{
		assert(!node->contents.contentsStaticModels);
		node->contents.contentsEntities = 0;

		if ( !node->tree.parent )
		{
			assert(nodeIndex == SECTOR_HEAD);
			break;
		}

		parentNodeIndex = node->tree.parent;
		node->tree.parent = cm_world.freeHead;

		cm_world.freeHead = nodeIndex;
		node = &cm_world.sectors[parentNodeIndex];

		if ( node->tree.child[0] == nodeIndex )
		{
			node->tree.child[0] = 0;
		}
		else
		{
			assert(node->tree.child[1] == nodeIndex);
			node->tree.child[1] = 0;
		}

		nodeIndex = parentNodeIndex;
	}

Loop:
	contents = cm_world.sectors[node->tree.child[0]].contents.contentsEntities | cm_world.sectors[node->tree.child[1]].contents.contentsEntities;

	if ( node->contents.entities )
	{
		for ( scan = &sv.svEntities[node->contents.entities - 1] ; ; scan = &sv.svEntities[scan->nextEntityInWorldSector - 1] )
		{
			contents |= SV_GEntityForSvEntity(scan)->r.contents;

			if ( !scan->nextEntityInWorldSector )
			{
				break;
			}
		}
	}

	node->contents.contentsEntities = contents;
	parentNodeIndex = node->tree.parent;

	if ( !parentNodeIndex )
	{
		return;
	}

	node = &cm_world.sectors[parentNodeIndex];
	goto Loop;
}

/*
================
CM_AddEntityToNode
================
*/
static void CM_AddEntityToNode( svEntity_t *ent, unsigned short childNodeIndex )
{
	unsigned short entnum;
	unsigned short *prevEnt;

	entnum = ent - sv.svEntities;
	prevEnt = &cm_world.sectors[childNodeIndex].contents.entities;

	while ( 1 )
	{
		if ( (unsigned short)(*prevEnt - 1) > entnum )
		{
			ent->worldSector = childNodeIndex;
			ent->nextEntityInWorldSector = *prevEnt;
			*prevEnt = entnum + 1;
			break;
		}

		prevEnt = &sv.svEntities[*prevEnt - 1].nextEntityInWorldSector;
	}
}

/*
================
CM_AddStaticModelToNode
================
*/
static void CM_AddStaticModelToNode( cStaticModel_t *staticModel, unsigned short childNodeIndex )
{
	unsigned short modelnum;
	unsigned short *prevStaticModel;

	modelnum = staticModel - cm.staticModelList;
	prevStaticModel = &cm_world.sectors[childNodeIndex].contents.staticModels;

	while ( 1 )
	{
		if ( (unsigned short)(*prevStaticModel - 1) > modelnum )
		{
			staticModel->writable.nextModelInWorldSector = *prevStaticModel;
			*prevStaticModel = modelnum + 1;
			break;
		}

		prevStaticModel = &cm.staticModelList[*prevStaticModel - 1].writable.nextModelInWorldSector;
	}
}

/*
================
CM_SortNode
================
*/
static void CM_SortNode( unsigned short nodeIndex, vec2_t mins, vec2_t maxs )
{
	svEntity_t *ent;
	cStaticModel_t *staticModel;
	int axis;
	float dist;
	svEntity_t *prevEnt;
	cStaticModel_t *prevStaticModel;
	unsigned short childNodeIndex;
	unsigned short entnum;
	unsigned short modelnum;
	worldSector_t *node;

	if ( cm_world.lockTree )
	{
		return;
	}

	node = &cm_world.sectors[nodeIndex];

	axis = node->tree.axis;
	dist = node->tree.dist;

	// Sort entities
	prevEnt = NULL;
	entnum = node->contents.entities;

	while ( entnum )
	{
		ent = &sv.svEntities[entnum - 1];

		if ( ent->linkmin[axis] > dist )
		{
			childNodeIndex = node->tree.child[0];

			if ( !childNodeIndex )
			{
				childNodeIndex = CM_AllocWorldSector(mins, maxs);

				if ( !childNodeIndex )
				{
					goto skipEntity;
				}

				node->tree.child[0] = childNodeIndex;
				cm_world.sectors[childNodeIndex].tree.parent = nodeIndex;
			}
		}
		else if ( ent->linkmax[axis] < dist )
		{
			childNodeIndex = node->tree.child[1];

			if ( !childNodeIndex )
			{
				childNodeIndex = CM_AllocWorldSector(mins, maxs);

				if ( childNodeIndex )
				{
					node->tree.child[1] = childNodeIndex;
					cm_world.sectors[childNodeIndex].tree.parent = nodeIndex;
				}
				else
				{
					goto skipEntity;
				}
			}
		}
		else
		{
skipEntity:
			prevEnt = ent;
			entnum = ent->nextEntityInWorldSector;
			continue;
		}

		entnum = ent->nextEntityInWorldSector;

		CM_AddEntityToNode(ent, childNodeIndex);

		cm_world.sectors[childNodeIndex].contents.contentsEntities |= SV_GEntityForSvEntity(ent)->r.contents;

		if ( !prevEnt )
			node->contents.entities = entnum;
		else
			prevEnt->nextEntityInWorldSector = entnum;
	}

	// Sort static models
	prevStaticModel = NULL;
	modelnum = node->contents.staticModels;

	while ( modelnum )
	{
		staticModel = &cm.staticModelList[modelnum - 1];

		if ( staticModel->absmin[axis] > dist )
		{
			childNodeIndex = node->tree.child[0];

			if ( !childNodeIndex )
			{
				childNodeIndex = CM_AllocWorldSector(mins, maxs);

				if ( !childNodeIndex )
				{
					goto skipStaticModel;
				}

				node->tree.child[0] = childNodeIndex;
				cm_world.sectors[childNodeIndex].tree.parent = nodeIndex;
			}
		}
		else if ( staticModel->absmax[axis] < dist )
		{
			childNodeIndex = node->tree.child[1];

			if ( !childNodeIndex )
			{
				childNodeIndex = CM_AllocWorldSector(mins, maxs);

				if ( childNodeIndex )
				{
					node->tree.child[1] = childNodeIndex;
					cm_world.sectors[childNodeIndex].tree.parent = nodeIndex;
				}
				else
				{
					goto skipStaticModel;
				}
			}
		}
		else
		{
skipStaticModel:
			prevStaticModel = staticModel;
			modelnum = staticModel->writable.nextModelInWorldSector;
			continue;
		}

		modelnum = staticModel->writable.nextModelInWorldSector;

		CM_AddStaticModelToNode(staticModel, childNodeIndex);

		cm_world.sectors[childNodeIndex].contents.contentsStaticModels |= XModelGetContents(staticModel->xmodel);

		if ( !prevStaticModel )
			node->contents.staticModels = modelnum;
		else
			prevStaticModel->writable.nextModelInWorldSector = modelnum;
	}
}

/*
================
CM_LinkEntity
================
*/
void CM_LinkEntity( svEntity_t *ent, vec3_t absmin, vec3_t absmax, clipHandle_t clipHandle )
{
	unsigned short nodeIndex;
	int axis;
	float dist;
	vec2_t mins;
	vec2_t maxs;
	worldSector_t *node;
	int linkcontents;
	cmodel_t *cmod;
	cLeaf_t *leaf;

	cmod = CM_ClipHandleToModel(clipHandle);
	leaf = &cmod->leaf;
	linkcontents = leaf->brushContents | leaf->terrainContents;

	if ( !linkcontents )
	{
		CM_UnlinkEntity(ent);
		return;
	}

Loop:
	Vector2Copy(cm_world.mins, mins);
	Vector2Copy(cm_world.maxs, maxs);

	for ( nodeIndex = 1; ; nodeIndex = node->tree.child[1] )
	{
		while ( 1 )
		{
			cm_world.sectors[nodeIndex].contents.contentsEntities |= linkcontents;
			node = &cm_world.sectors[nodeIndex];

			axis = node->tree.axis;
			dist = node->tree.dist;

			if ( absmin[axis] > dist )
			{
			}
			else
			{
				break;
			}

			mins[axis] = dist;

			if ( node->tree.child[0] )
			{
				nodeIndex = node->tree.child[0];
			}
			else
			{
				goto LABEL_13;
			}
		}

		if ( absmax[axis] < dist )
		{
		}
		else
		{
			break;
		}

		maxs[axis] = dist;

		if ( node->tree.child[1] )
		{
		}
		else
		{
			goto LABEL_13;
		}
	}

	if ( nodeIndex == ent->worldSector && !(ent->linkcontents & ~linkcontents) )
	{
		ent->linkcontents = linkcontents;
		Vector2Copy(absmin, ent->linkmin);
		Vector2Copy(absmax, ent->linkmax);
		return;
	}
LABEL_13:
	if ( ent->worldSector )
	{
		if ( nodeIndex == ent->worldSector && !(ent->linkcontents & ~linkcontents) )
		{
			goto LABEL_18;
		}

		CM_UnlinkEntity(ent);
		goto Loop;
	}
	CM_AddEntityToNode(ent, nodeIndex);
LABEL_18:
	ent->linkcontents = linkcontents;
	Vector2Copy(absmin, ent->linkmin);
	Vector2Copy(absmax, ent->linkmax);
	CM_SortNode(nodeIndex, mins, maxs);
}

/*
================
CM_LinkStaticModel
================
*/
void CM_LinkStaticModel( cStaticModel_t *staticModel )
{
	unsigned short nodeIndex;
	int axis;
	float dist;
	int contents;
	vec2_t mins;
	vec2_t maxs;
	worldSector_t *node;

	contents = XModelGetContents(staticModel->xmodel);
	assert(contents);

	Vector2Copy(cm_world.mins, mins);
	Vector2Copy(cm_world.maxs, maxs);

	for ( nodeIndex = 1; ; nodeIndex = node->tree.child[1] )
	{
		while ( 1 )
		{
			cm_world.sectors[nodeIndex].contents.contentsStaticModels |= contents;

			node = &cm_world.sectors[nodeIndex];

			axis = node->tree.axis;
			dist = node->tree.dist;

			if ( staticModel->absmin[axis] > dist )
			{
			}
			else
			{
				break;
			}

			mins[axis] = dist;

			if ( node->tree.child[0] )
			{
				nodeIndex = node->tree.child[0];
			}
			else
			{
				goto done;
			}
		}

		if ( staticModel->absmax[axis] < dist )
		{
		}
		else
		{
			break;
		}

		maxs[axis] = dist;

		if ( node->tree.child[1] )
		{
		}
		else
		{
			break;
		}
	}

done:
	CM_AddStaticModelToNode(staticModel, nodeIndex);
	CM_SortNode(nodeIndex, mins, maxs);
}

/*
================
CM_LinkAllStaticModels
================
*/
void CM_LinkAllStaticModels()
{
	cStaticModel_t *staticModel;
	int i;

	for ( i = 0, staticModel = cm.staticModelList; i < cm.numStaticModels ; i++, staticModel++ )
	{
		assert(staticModel->xmodel);

		if ( !XModelGetContents(staticModel->xmodel) )
		{
			continue;
		}

		CM_LinkStaticModel(staticModel);
	}
}

/*
================
CM_AreaEntities_r
================
*/
static void CM_AreaEntities_r( unsigned short nodeIndexIn, areaParms_t *ap )
{
	unsigned short nodeIndex;
	svEntity_t *svEnt;
	gentity_t *gcheck;
	unsigned short entnum;
	worldSector_t *node;

	nodeIndex = nodeIndexIn;
	node = &cm_world.sectors[nodeIndex];

	if ( !(node->contents.contentsEntities & ap->contentmask) )
	{
		return;
	}

	for ( entnum = node->contents.entities; entnum; entnum = svEnt->nextEntityInWorldSector )
	{
		svEnt = &sv.svEntities[entnum - 1];
		gcheck = SV_GEntityForSvEntity(svEnt);

		if ( !(gcheck->r.contents & ap->contentmask) )
		{
			continue;
		}

		if (	          	 gcheck->r.absmin[ 0 ] > ap->maxs[ 0 ]
		                     || gcheck->r.absmax[ 0 ] < ap->mins[ 0 ]
		                     || gcheck->r.absmin[ 1 ] > ap->maxs[ 1 ]
		                     || gcheck->r.absmax[ 1 ] < ap->mins[ 1 ]
		                     || gcheck->r.absmin[ 2 ] > ap->maxs[ 2 ]
		                     || gcheck->r.absmax[ 2 ] < ap->mins[ 2 ] )
		{
			continue;
		}

		if ( ap->count == ap->maxcount )
		{
			Com_DPrintf("CM_AreaEntities: MAXCOUNT\n");
			return;
		}

		ap->list[ap->count] = svEnt - sv.svEntities;
		ap->count++;
	}

	// recurse down both sides
	if ( ap->maxs[node->tree.axis] > node->tree.dist )
	{
		CM_AreaEntities_r(node->tree.child[0], ap);
	}

	if ( ap->mins[node->tree.axis] < node->tree.dist )
	{
		CM_AreaEntities_r(node->tree.child[1], ap);
	}
}

/*
================
CM_AreaEntities
================
*/
int CM_AreaEntities( const vec3_t mins, const vec3_t maxs, int *entityList, int maxcount, int contentmask )
{
	areaParms_t ap;

	ap.mins = mins;
	ap.maxs = maxs;
	ap.list = entityList;
	ap.count = 0;
	ap.maxcount = maxcount;
	ap.contentmask = contentmask;

	CM_AreaEntities_r(1, &ap);

	return ap.count;
}

/*
================
CM_PointTraceStaticModels_r
================
*/
static void CM_PointTraceStaticModels_r( locTraceWork_t *tw, unsigned short nodeIndex, const vec4_t p1_, const vec4_t p2, trace_t *trace )
{
	float t1;
	float t2;
	float frac;
	int side;
	vec4_t mid;
	cStaticModel_t *check;
	unsigned short modelnum;
	worldSector_t *node;
	vec4_t p1;

	VectorCopy4(p1_, p1);

	while ( 1 )
	{
		node = &cm_world.sectors[nodeIndex];

		if ( !(node->contents.contentsStaticModels & tw->contents) )
		{
			break;
		}

		for ( modelnum = node->contents.staticModels; modelnum; modelnum = check->writable.nextModelInWorldSector )
		{
			check = &cm.staticModelList[modelnum - 1];

			if ( !(XModelGetContents(check->xmodel) & tw->contents) )
			{
				continue;
			}

			if ( CM_TraceBox(&tw->extents, check->absmin, check->absmax, trace->fraction) )
			{
				continue;
			}

			CM_TraceStaticModel(check, trace, tw->extents.start, tw->extents.end, tw->contents);
		}

		t1 = p1[node->tree.axis] - node->tree.dist;
		t2 = p2[node->tree.axis] - node->tree.dist;

		if ( (float)(t1 * t2) >= 0.0f )
		{
			nodeIndex = node->tree.child[1 - I_side(I_fmin(t1, t2))];
			continue;
		}

		if ( trace->fraction <= p1[3] )
		{
			return;
		}

		assert(t1 - t2);

		frac = t1 / (float)(t1 - t2);

		assert(frac >= 0);
		assert(frac <= 1.f);

		mid[0] = p1[0] + (float)((float)(p2[0] - p1[0]) * frac);
		mid[1] = p1[1] + (float)((float)(p2[1] - p1[1]) * frac);
		mid[2] = p1[2] + (float)((float)(p2[2] - p1[2]) * frac);
		mid[3] = p1[3] + (float)((float)(p2[3] - p1[3]) * frac);

		side = I_side(t2);

		CM_PointTraceStaticModels_r(tw, node->tree.child[side], p1, mid, trace);

		nodeIndex = node->tree.child[1 - side];

		VectorCopy4(mid, p1);
	}
}

/*
================
CM_PointTraceStaticModels
================
*/
void CM_PointTraceStaticModels( trace_t *results, const vec3_t start, const vec3_t end, int contentmask )
{
	locTraceWork_t tw;
	vec4_t start_;
	vec4_t end_;

	tw.contents = contentmask;

	VectorCopy(start, tw.extents.start);
	VectorCopy(end, tw.extents.end);

	CM_CalcTraceEntents(&tw.extents);

	VectorCopy(tw.extents.start, start_);
	start_[3] = 0.0;

	VectorCopy(tw.extents.end, end_);
	end_[3] = results->fraction;

	CM_PointTraceStaticModels_r(&tw, 1, start_, end_, results);
}

/*
================
CM_PointTraceStaticModelsComplete_r
================
*/
static qboolean CM_PointTraceStaticModelsComplete_r( staticmodeltrace_t *clip, unsigned short nodeIndex, const vec3_t p1_, const vec3_t p2 )
{
	float t1;
	float t2;
	float frac;
	int side;
	vec3_t mid;
	cStaticModel_t *check;
	unsigned short modelnum;
	worldSector_t *node;
	vec3_t p1;

	p1[0] = p1_[0];
	p1[1] = p1_[1];
	for ( p1[2] = p1_[2]; ; p1[2] = mid[2] )
	{
		while ( 1 )
		{
			node = &cm_world.sectors[nodeIndex];

			if ( !(node->contents.contentsStaticModels & clip->contents) )
			{
				return qtrue;
			}

			for ( modelnum = node->contents.staticModels; modelnum; modelnum = check->writable.nextModelInWorldSector )
			{
				check = &cm.staticModelList[modelnum - 1];

				if ( !(XModelGetContents(check->xmodel) & clip->contents) )
				{
					continue;
				}

				if ( CM_TraceBox(&clip->extents, check->absmin, check->absmax, 1.0) )
				{
					continue;
				}

				if ( !CM_TraceStaticModelComplete(check, clip->extents.start, clip->extents.end, clip->contents) )
				{
					return qfalse;
				}
			}

			t1 = p1[node->tree.axis] - node->tree.dist;
			t2 = p2[node->tree.axis] - node->tree.dist;

			if ( (float)(t1 * t2) >= 0.0f )
			{
				frac = (float)(t2 - t1) >= 0.0f ? t1 : t2;
				nodeIndex = node->tree.child[1 - (frac >= 0.0f)];
				continue;
			}

			break;
		}

		assert(t1 - t2);

		frac = t1 / (float)(t1 - t2);

		assert(frac >= 0);
		assert(frac <= 1.f);

		mid[0] = p1[0] + (float)((float)(p2[0] - p1[0]) * frac);
		mid[1] = p1[1] + (float)((float)(p2[1] - p1[1]) * frac);
		mid[2] = p1[2] + (float)((float)(p2[2] - p1[2]) * frac);

		side = I_side(t2);

		if ( !CM_PointTraceStaticModelsComplete_r(clip, node->tree.child[side], p1, mid) )
		{
			break;
		}

		nodeIndex = node->tree.child[1 - side];
		p1[0] = mid[0];
		p1[1] = mid[1];
	}

	return qfalse;
}

/*
================
CM_PointTraceStaticModelsComplete
================
*/
qboolean CM_PointTraceStaticModelsComplete( const vec3_t start, const vec3_t end, int contentmask )
{
	staticmodeltrace_t clip;
	qboolean hit;

	clip.contents = contentmask;

	VectorCopy(start, clip.extents.start);
	VectorCopy(end, clip.extents.end);

	CM_CalcTraceEntents(&clip.extents);

	hit = CM_PointTraceStaticModelsComplete_r(&clip, 1, clip.extents.start, clip.extents.end);

	return hit;
}

/*
================
CM_ClipMoveToEntities_r
================
*/
static void CM_ClipMoveToEntities_r( moveclip_t *clip, unsigned short nodeIndex, const vec4_t p1, const vec4_t p2, trace_t *trace )
{
	float t1;
	float t2;
	float frac;
	float frac2;
	int side;
	vec4_t mid;
	svEntity_t *check;
	float offset;
	float invDiff;
	unsigned short entnum;
	worldSector_t *node;
	vec4_t p;
	float diff;
	float absDiff;
	float sideDist;

	VectorCopy4(p1, p);

	while ( 1 )
	{
		node = &cm_world.sectors[nodeIndex];

		if ( !(node->contents.contentsEntities & clip->contentmask) )
		{
			return;
		}

		for ( entnum = node->contents.entities; entnum; entnum = check->nextEntityInWorldSector )
		{
			check = &sv.svEntities[entnum - 1];

			// if it doesn't have any brushes of a type we
			// are looking for, ignore it
			if ( !(clip->contentmask & check->linkcontents) )
			{
				continue;
			}

			SV_ClipMoveToEntity(clip, check, trace);
		}

		t1 = p[node->tree.axis] - node->tree.dist;
		t2 = p2[node->tree.axis] - node->tree.dist;

		offset = clip->outerSize[node->tree.axis];

		if ( I_fmin(t1, t2) >= offset )
		{
			nodeIndex = node->tree.child[0];
			continue;
		}

		if ( I_fmax(t1, t2) <= -offset )
		{
			nodeIndex = node->tree.child[1];
			continue;
		}

		if ( trace->fraction <= p[3] )
		{
			return;
		}

		diff = t2 - t1;

		if ( diff != 0.0f )
		{
			absDiff = I_fabs(diff);
			sideDist = I_fsel(diff, -t1, t1);
			invDiff = 1.0f / absDiff;
			frac2 = (float)(sideDist - offset) * invDiff;
			frac = (float)(sideDist + offset) * invDiff;
			side = I_side(diff);
		}
		else
		{
			side = 0;
			frac = 1.0;
			frac2 = 0.0f;
		}

		frac = I_fmin(frac, 1.0);

		mid[0] = p[0] + (float)((float)(p2[0] - p[0]) * frac);
		mid[1] = p[1] + (float)((float)(p2[1] - p[1]) * frac);
		mid[2] = p[2] + (float)((float)(p2[2] - p[2]) * frac);
		mid[3] = p[3] + (float)((float)(p2[3] - p[3]) * frac);

		CM_ClipMoveToEntities_r(clip, node->tree.child[side], p, mid, trace);

		frac2 = I_fmax(frac2, 0.0f);

		p[0] = p[0] + (float)((float)(p2[0] - p[0]) * frac2);
		p[1] = p[1] + (float)((float)(p2[1] - p[1]) * frac2);
		p[2] = p[2] + (float)((float)(p2[2] - p[2]) * frac2);
		p[3] = p[3] + (float)((float)(p2[3] - p[3]) * frac2);

		nodeIndex = node->tree.child[1 - side];
	}
}

/*
================
CM_ClipMoveToEntities
================
*/
void CM_ClipMoveToEntities( moveclip_t *clip, trace_t *trace )
{
	vec4_t start;
	vec4_t end;

	assert(trace->fraction <= 1.f);

	VectorCopy(clip->extents.start, start);
	VectorCopy(clip->extents.end, end);

	start[3] = 0.0;
	end[3] = trace->fraction;

	CM_ClipMoveToEntities_r(clip, 1, start, end, trace);
}

/*
================
CM_ClipSightTraceToEntities_r
================
*/
static int CM_ClipSightTraceToEntities_r( sightclip_t *clip, unsigned short nodeIndex, const vec3_t p1, const vec3_t p2 )
{
	float t1;
	float t2;
	float frac;
	float frac2;
	int side;
	vec3_t mid;
	svEntity_t *check;
	float offset;
	float invDiff;
	int hitNum;
	unsigned short entnum;
	worldSector_t *node;
	vec3_t p;
	float diff;
	float absDiff;
	float sideDist;

	VectorCopy(p1, p);

	while ( 1 )
	{
		node = &cm_world.sectors[nodeIndex];

		if ( !(node->contents.contentsEntities & clip->contentmask) )
		{
			return 0;
		}

		for ( entnum = node->contents.entities; entnum; entnum = check->nextEntityInWorldSector )
		{
			check = &sv.svEntities[entnum - 1];
			hitNum = SV_ClipSightToEntity(clip, check);

			if ( hitNum )
			{
				return hitNum;
			}
		}

		t1 = p[node->tree.axis] - node->tree.dist;
		t2 = p2[node->tree.axis] - node->tree.dist;

		offset = clip->outerSize[node->tree.axis];

		if ( I_fmin(t1, t2) >= offset )
		{
			nodeIndex = node->tree.child[0];
			continue;
		}

		if ( I_fmax(t1, t2) <= -offset )
		{
			nodeIndex = node->tree.child[1];
			continue;
		}

		diff = t2 - t1;

		if ( diff != 0.0f )
		{
			absDiff = I_fabs(diff);
			sideDist = I_fsel(diff, -t1, t1);
			invDiff = 1.0f / absDiff;
			frac2 = (float)(sideDist - offset) * invDiff;
			frac = (float)(sideDist + offset) * invDiff;
			side = I_side(diff);
		}
		else
		{
			side = 0;
			frac = 1.0;
			frac2 = 0.0f;
		}

		frac = I_fmin(frac, 1.0);

		mid[0] = p[0] + (float)((float)(p2[0] - p[0]) * frac);
		mid[1] = p[1] + (float)((float)(p2[1] - p[1]) * frac);
		mid[2] = p[2] + (float)((float)(p2[2] - p[2]) * frac);

		hitNum = CM_ClipSightTraceToEntities_r(clip, node->tree.child[side], p, mid);

		if ( hitNum )
		{
			return hitNum;
		}

		frac2 = I_fmax(frac2, 0.0f);

		p[0] = p[0] + (float)((float)(p2[0] - p[0]) * frac2);
		p[1] = p[1] + (float)((float)(p2[1] - p[1]) * frac2);
		p[2] = p[2] + (float)((float)(p2[2] - p[2]) * frac2);

		nodeIndex = node->tree.child[1 - side];
	}
}

/*
================
CM_ClipSightTraceToEntities
================
*/
int CM_ClipSightTraceToEntities( sightclip_t *clip )
{
	int hitNum;

	hitNum = CM_ClipSightTraceToEntities_r(clip, 1, clip->start, clip->end);
	return hitNum;
}

/*
================
CM_PointTraceToEntities_r
================
*/
static void CM_PointTraceToEntities_r( pointtrace_t *clip, unsigned short nodeIndex, const vec4_t p1, const vec4_t p2, trace_t *trace )
{
	float t1;
	float t2;
	float frac;
	int side;
	vec4_t mid;
	svEntity_t *check;
	unsigned short entnum;
	worldSector_t *node;
	vec4_t p;

	VectorCopy4(p1, p);

	while ( 1 )
	{
		node = &cm_world.sectors[nodeIndex];

		if ( !(node->contents.contentsEntities & clip->contentmask) )
		{
			break;
		}

		for ( entnum = node->contents.entities; entnum; entnum = check->nextEntityInWorldSector )
		{
			check = &sv.svEntities[entnum - 1];
			SV_PointTraceToEntity(clip, check, trace);
		}

		t1 = p[node->tree.axis] - node->tree.dist;
		t2 = p2[node->tree.axis] - node->tree.dist;

		if ( (float)(t1 * t2) >= 0.0f )
		{
			nodeIndex = node->tree.child[1 - I_side(I_fmin(t1, t2))];
			continue;
		}

		if ( trace->fraction <= p[3] )
		{
			return;
		}

		frac = t1 / (float)(t1 - t2);

		assert(frac >= 0);
		assert(frac <= 1.f);

		mid[0] = p[0] + (float)((float)(p2[0] - p[0]) * frac);
		mid[1] = p[1] + (float)((float)(p2[1] - p[1]) * frac);
		mid[2] = p[2] + (float)((float)(p2[2] - p[2]) * frac);
		mid[3] = p[3] + (float)((float)(p2[3] - p[3]) * frac);

		side = I_side(t2);

		CM_PointTraceToEntities_r(clip, node->tree.child[side], p, mid, trace);

		nodeIndex = node->tree.child[1 - side];

		VectorCopy4(mid, p);
	}
}

/*
================
CM_PointTraceToEntities
================
*/
void CM_PointTraceToEntities( pointtrace_t *clip, trace_t *trace )
{
	vec4_t start;
	vec4_t end;

	assert(trace->fraction <= 1.f);

	VectorCopy(clip->extents.start, start);
	VectorCopy(clip->extents.end, end);

	start[3] = 0.0;
	end[3] = trace->fraction;

	CM_PointTraceToEntities_r(clip, 1, start, end, trace);
}

/*
================
CM_PointSightTraceToEntities_r
================
*/
static int CM_PointSightTraceToEntities_r( sightpointtrace_t *clip, unsigned short nodeIndex, const vec3_t p1, const vec3_t p2 )
{
	float t1;
	float t2;
	float frac;
	int side;
	vec3_t mid;
	svEntity_t *check;
	int hitNum;
	unsigned short entnum;
	worldSector_t *node;

	node = &cm_world.sectors[nodeIndex];

	if ( !(node->contents.contentsEntities & clip->contentmask) )
	{
		return 0;
	}

	t1 = p1[node->tree.axis] - node->tree.dist;
	t2 = p2[node->tree.axis] - node->tree.dist;

	if ( (float)(t1 * t2) >= 0.0f )
	{
		hitNum = CM_PointSightTraceToEntities_r(clip, node->tree.child[1 - I_side(I_fmin(t1, t2))], p1, p2);

		if ( hitNum )
		{
			return hitNum;
		}
	}
	else
	{
		frac = t1 / (float)(t1 - t2);

		assert(frac >= 0);
		assert(frac <= 1.f);

		mid[0] = p1[0] + (float)((float)(p2[0] - p1[0]) * frac);
		mid[1] = p1[1] + (float)((float)(p2[1] - p1[1]) * frac);
		mid[2] = p1[2] + (float)((float)(p2[2] - p1[2]) * frac);

		side = I_side(t2);

		hitNum = CM_PointSightTraceToEntities_r(clip, node->tree.child[side], p1, mid);

		if ( hitNum )
		{
			return hitNum;
		}

		hitNum = CM_PointSightTraceToEntities_r(clip, node->tree.child[1 - side], mid, p2);

		if ( hitNum )
		{
			return hitNum;
		}
	}

	for ( entnum = node->contents.entities; entnum; entnum = check->nextEntityInWorldSector )
	{
		check = &sv.svEntities[entnum - 1];
		hitNum = SV_PointSightTraceToEntity(clip, check);

		if ( hitNum )
		{
			return hitNum;
		}
	}

	return 0;
}

/*
================
CM_PointSightTraceToEntities
================
*/
int CM_PointSightTraceToEntities( sightpointtrace_t *clip )
{
	int hitNum;

	hitNum = CM_PointSightTraceToEntities_r(clip, 1, clip->start, clip->end);
	return hitNum;
}

