#include "qcommon.h"
#include "cm_local.h"

void CM_CalcTraceEntents( TraceExtents *extents );
qboolean CM_TraceBox( TraceExtents *extents, const vec3_t mins, const vec3_t maxs, float fraction );

/*
===================
CM_CalcTraceEntents
===================
*/
void CM_CalcTraceEntents( TraceExtents *extents )
{
	int i;
	float diff;

	for ( i = 0; i < 3; ++i )
	{
		diff = extents->start[i] - extents->end[i];
		extents->invDelta[i] = diff != 0.0f ? 1.0f / diff : 0.0f;
	}
}

/*
===================
CM_TraceBox
===================
*/
qboolean CM_TraceBox( TraceExtents *extents, const vec3_t mins, const vec3_t maxs, float fraction )
{
	float enterFrac;
	float exitFrac;
	float sign;
	float startDist;
	float endDist;
	float frac;
	int i;
	const float *bounds;

	enterFrac = 0.0f;
	exitFrac = fraction;
	sign = -1.0f;
	bounds = mins;

	while ( 1 )
	{
		for ( i = 0; i < 3; ++i )
		{
			startDist = (extents->start[i] - bounds[i]) * sign;
			endDist = (extents->end[i] - bounds[i]) * sign;

			if ( startDist > 0.0f )
			{
				if ( endDist > 0.0f )
					return qtrue;

				frac = startDist * extents->invDelta[i] * sign;

				if ( frac >= exitFrac )
					return qtrue;

				enterFrac = I_fmax(enterFrac, frac);
			}
			else if ( endDist > 0.0f )
			{
				frac = startDist * extents->invDelta[i] * sign;

				if ( enterFrac >= frac )
					return qtrue;

				exitFrac = I_fmin(exitFrac, frac);
			}
		}

		if ( sign == 1.0f )
			break;

		sign = 1.0f;
		bounds = maxs;
	}

	return qfalse;
}

