// winding utilities from the q3 polylib; only the chop path survives on the server
#include "qcommon.h"

#define MAX_POINTS_ON_WINDING 64

#define MAX_MAP_BOUNDS 131072

#define SIDE_FRONT 0
#define SIDE_BACK 1
#define SIDE_ON 2

typedef struct
{
	int numpoints;
	vec3_t p[4];
} winding_t;

static int c_active_windings;
static int c_peak_windings;

static float dot;

winding_t *AllocWinding( int points )
{
	winding_t *w;
	int s;

	++c_active_windings;
	if ( c_active_windings > c_peak_windings )
	{
		c_peak_windings = c_active_windings;
	}

	s = sizeof(vec3_t) * points + sizeof(int);
	w = (winding_t *)Z_Malloc(s);

	return w;
}

void FreeWinding( winding_t *w )
{
	if ( *(unsigned *)w == 0xdeaddead )
	{
		Com_Error(ERR_FATAL, "\x15" "FreeWinding: freed a freed winding");
	}

	*(unsigned *)w = 0xdeaddead;
	--c_active_windings;

	Z_Free(w);
}

void WindingBounds( winding_t *w, vec3_t mins, vec3_t maxs )
{
	vec_t v;
	int i, j;

	mins[0] = mins[1] = mins[2] = MAX_MAP_BOUNDS;
	maxs[0] = maxs[1] = maxs[2] = -MAX_MAP_BOUNDS;

	for ( i = 0; i < w->numpoints; i++ )
	{
		for ( j = 0; j < 3; j++ )
		{
			v = w->p[i][j];
			if ( v < mins[j] )
				mins[j] = v;
			if ( v > maxs[j] )
				maxs[j] = v;
		}
	}
}

winding_t *BaseWindingForPlane( vec3_t normal, float dist )
{
	int i;
	int x;
	float max;
	float v;
	vec3_t org;
	vec3_t vright;
	vec3_t vup;
	winding_t *w;

	max = -MAX_MAP_BOUNDS;
	x = -1;

	for ( i = 0; i < 3; i++ )
	{
		v = I_fabs(normal[i]);

		if ( v > max )
		{
			x = i;
			max = v;
		}
	}

	if ( x == -1 )
	{
		Com_Error(ERR_DROP, "\x15" "BaseWindingForPlane: no axis found");
	}

	VectorCopy(vec3_origin, vup);

	switch ( x )
	{
	case 0:
	case 1:
		vup[2] = 1;
		break;
	case 2:
		vup[0] = 1;
		break;
	}

	v = DotProduct(vup, normal);
	VectorMA(vup, -v, normal, vup);
	Vec3NormalizeTo(vup, vup);

	VectorScale(normal, dist, org);

	Vec3Cross(vup, normal, vright);

	VectorScale(vup, MAX_MAP_BOUNDS, vup);
	VectorScale(vright, MAX_MAP_BOUNDS, vright);

	w = AllocWinding(4);

	VectorSubtract(org, vright, w->p[0]);
	VectorAdd(w->p[0], vup, w->p[0]);

	VectorAdd(org, vright, w->p[1]);
	VectorAdd(w->p[1], vup, w->p[1]);

	VectorAdd(org, vright, w->p[2]);
	VectorSubtract(w->p[2], vup, w->p[2]);

	VectorSubtract(org, vright, w->p[3]);
	VectorSubtract(w->p[3], vup, w->p[3]);

	w->numpoints = 4;

	return w;
}

winding_t *CopyWinding( winding_t *w )
{
	int size;
	winding_t *c;

	c = AllocWinding(w->numpoints);
	size = sizeof(vec3_t) * w->numpoints + sizeof(int);
	Com_Memcpy(c, w, size);

	return c;
}

void ChopWindingInPlace( winding_t **inout, vec3_t normal, float dist, float epsilon )
{
	winding_t *in;
	float dists[MAX_POINTS_ON_WINDING + 4];
	int sides[MAX_POINTS_ON_WINDING + 4];
	int counts[3];
	int i;
	int j;
	float *p1;
	float *p2;
	vec3_t mid;
	winding_t *f;
	int maxpts;

	in = *inout;
	counts[0] = counts[1] = counts[2] = 0;

	for ( i = 0; i < in->numpoints; i++ )
	{
		dot = DotProduct(in->p[i], normal);
		dot -= dist;
		dists[i] = dot;

		if ( dot > epsilon )
		{
			sides[i] = SIDE_FRONT;
		}
		else if ( dot < -epsilon )
		{
			sides[i] = SIDE_BACK;
		}
		else
		{
			sides[i] = SIDE_ON;
		}

		counts[sides[i]]++;
	}

	sides[i] = sides[0];
	dists[i] = dists[0];

	if ( !counts[0] )
	{
		FreeWinding(in);
		*inout = NULL;
		return;
	}

	if ( !counts[1] )
	{
		return;
	}

	maxpts = in->numpoints + 4;
	f = AllocWinding(maxpts);

	for ( i = 0; i < in->numpoints; i++ )
	{
		p1 = in->p[i];

		if ( sides[i] == SIDE_ON )
		{
			VectorCopy(p1, f->p[f->numpoints]);
			f->numpoints++;
			continue;
		}

		if ( sides[i] == SIDE_FRONT )
		{
			VectorCopy(p1, f->p[f->numpoints]);
			f->numpoints++;
		}

		if ( sides[i + 1] == SIDE_ON || sides[i + 1] == sides[i] )
		{
			continue;
		}

		p2 = in->p[(i + 1) % in->numpoints];
		dot = dists[i] / (dists[i] - dists[i + 1]);

		for ( j = 0; j < 3; j++ )
		{
			if ( normal[j] == 1.0f )
			{
				mid[j] = dist;
			}
			else if ( normal[j] == -1.0f )
			{
				mid[j] = -dist;
			}
			else
			{
				mid[j] = p1[j] + dot * (p2[j] - p1[j]);
			}
		}

		VectorCopy(mid, f->p[f->numpoints]);
		f->numpoints++;
	}

	if ( f->numpoints > maxpts )
	{
		Com_Error(ERR_DROP, "\x15" "ClipWinding: points exceeded estimate");
	}

	if ( f->numpoints > MAX_POINTS_ON_WINDING )
	{
		Com_Error(ERR_DROP, "\x15" "ClipWinding: MAX_POINTS_ON_WINDING");
	}

	FreeWinding(in);
	*inout = f;
}

