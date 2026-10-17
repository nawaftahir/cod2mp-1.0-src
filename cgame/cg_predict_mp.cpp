#include "../qcommon/qcommon.h"
#include "../bgame/bg_public.h"

// Client entity; only the collision fields are used on the server.
struct centity_t
{
	entityState_t currentState;
	entityState_t nextState;
	byte nextValid;
	byte cullIn;
	byte bMuzzleFlash;
	byte bTrailMade;
	int previousEventSequence;
	int miscTime;
	vec3_t lerpOrigin;
	vec3_t lerpAngles;
};

#define MAX_SOLID_ENTITIES 256

static pmove_t cg_pmove;
static int cg_numSolidEntities;
static centity_t *cg_solidEntities[MAX_SOLID_ENTITIES];
static int cg_numTriggerEntities;
static centity_t *cg_triggerEntities[MAX_SOLID_ENTITIES];

static void Trace_CalcBounds( const float *start, const float *mins, const float *maxs, const float *end, float fraction, vec3_t bounds[2] );
void CG_ClipMoveToEntities( const float *start, const float *mins, const float *maxs, const float *end, int skipNumber, int mask, int capsule, trace_t *tr );
void CG_Trace( trace_t *results, const float *start, const float *mins, const float *maxs, const float *end, int skipNumber, int mask );
int CG_PointContents( const float *point, int passEntityNum, int contentMask );

/*
==================
Trace_CalcBounds

Bounds of the swept box from start to the fraction reached so far.
==================
*/
static void Trace_CalcBounds( const float *start, const float *mins, const float *maxs, const float *end, float fraction, vec3_t bounds[2] )
{
	vec3_t hit;

	Vec3Lerp( start, end, fraction, hit );

	bounds[0][0] = I_fmin( start[0], hit[0] ) + mins[0];
	bounds[0][1] = I_fmin( start[1], hit[1] ) + mins[1];
	bounds[0][2] = I_fmin( start[2], hit[2] ) + mins[2];
	bounds[1][0] = I_fmax( start[0], hit[0] ) + maxs[0];
	bounds[1][1] = I_fmax( start[1], hit[1] ) + maxs[1];
	bounds[1][2] = I_fmax( start[2], hit[2] ) + maxs[2];
}

/*
==================
CG_ClipMoveToEntities
==================
*/
void CG_ClipMoveToEntities( const float *start, const float *mins, const float *maxs, const float *end, int skipNumber, int mask, int capsule, trace_t *tr )
{
	int i;
	int x;
	int zd;
	int zu;
	trace_t trace;
	entityState_t *ent;
	clipHandle_t cmodel;
	vec3_t bmins;
	vec3_t bmaxs;
	vec3_t angles;
	centity_t *cent;
	int contents;
	vec3_t bounds[2];
	float radius;

	Trace_CalcBounds( start, mins, maxs, end, tr->fraction, bounds );

	for ( i = 0; i < cg_numSolidEntities; i++ )
	{
		cent = cg_solidEntities[i];
		ent = &cent->nextState;

		if ( ent->number == skipNumber )
		{
			continue;
		}

		if ( ent->solid == SOLID_BMODEL )
		{
			cmodel = ent->index;
			contents = CM_ContentsOfModel( cmodel );

			if ( !(contents & mask) )
			{
				continue;
			}

			radius = CM_RadiusOfModel( cmodel );

			if ( cent->lerpOrigin[0] - radius >= bounds[1][0]
			        || cent->lerpOrigin[1] - radius >= bounds[1][1]
			        || bounds[0][0] >= cent->lerpOrigin[0] + radius
			        || bounds[0][1] >= cent->lerpOrigin[1] + radius
			        || cent->lerpOrigin[2] - radius >= bounds[1][2]
			        || bounds[0][2] >= cent->lerpOrigin[2] + radius )
			{
				continue;
			}

			VectorCopy( cent->lerpAngles, angles );
		}
		else
		{
			switch ( ent->eType )
			{
			case ET_PLAYER:
				contents = CONTENTS_BODY;
				break;
			default:
				contents = CONTENTS_SOLID;
				break;
			}

			if ( !(contents & mask) )
			{
				continue;
			}

			x = ent->solid & 255;
			zd = ((ent->solid >> 8) & 255) - 1;
			zu = ((ent->solid >> 16) & 255) - 32;

			bmins[0] = bmins[1] = 1.0f - x;
			bmaxs[0] = bmaxs[1] = x - 1.0f;
			bmins[2] = 1.0f - zd;
			bmaxs[2] = zu - 1.0f;

			if ( cent->lerpOrigin[0] + bmins[0] >= bounds[1][0]
			        || cent->lerpOrigin[1] + bmins[1] >= bounds[1][1]
			        || bounds[0][0] >= cent->lerpOrigin[0] + bmaxs[0]
			        || bounds[0][1] >= cent->lerpOrigin[1] + bmaxs[1]
			        || cent->lerpOrigin[2] + bmins[2] >= bounds[1][2]
			        || bounds[0][2] >= cent->lerpOrigin[2] + bmaxs[2] )
			{
				continue;
			}

			cmodel = CM_TempBoxModel( bmins, bmaxs, contents );
			VectorClear( angles );
		}

		CM_TransformedBoxTraceExternal( &trace, start, end, mins, maxs, cmodel, mask, cent->lerpOrigin, angles );

		if ( trace.fraction < tr->fraction )
		{
			trace.entityNum = ent->number;
			*tr = trace;
			Trace_CalcBounds( start, mins, maxs, end, trace.fraction, bounds );
		}
		else if ( trace.allsolid )
		{
			trace.entityNum = ent->number;
			*tr = trace;
		}
		else if ( trace.startsolid )
		{
			tr->startsolid = 1;
		}

		if ( tr->allsolid )
		{
			return;
		}
	}
}

/*
==================
CG_Trace
==================
*/
void CG_Trace( trace_t *results, const float *start, const float *mins, const float *maxs, const float *end, int skipNumber, int mask )
{
	CM_BoxTrace( results, start, end, mins, maxs, 0, mask );
	results->entityNum = results->fraction != 1.0f ? ENTITYNUM_WORLD : ENTITYNUM_NONE;

	if ( results->fraction != 0.0f )
	{
		CG_ClipMoveToEntities( start, mins, maxs, end, skipNumber, mask, 1, results );
	}
}

/*
==================
CG_PointContents
==================
*/
int CG_PointContents( const float *point, int passEntityNum, int contentMask )
{
	int i;
	entityState_t *ent;
	centity_t *cent;
	clipHandle_t cmodel;
	int contents;

	contents = CM_PointContents( point, 0 );

	for ( i = 0; i < cg_numSolidEntities; i++ )
	{
		cent = cg_solidEntities[i];
		ent = &cent->nextState;

		if ( ent->number == passEntityNum )
		{
			continue;
		}

		if ( ent->solid != SOLID_BMODEL )
		{
			continue;
		}

		cmodel = ent->index;

		if ( !cmodel )
		{
			continue;
		}

		contents |= CM_TransformedPointContents( point, cmodel, cent->lerpOrigin, cent->lerpAngles );
	}

	return contents & contentMask;
}

