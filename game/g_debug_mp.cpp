// Debug drawing; lines go to the client debug-line list.
#include "../qcommon/qcommon.h"
#include "g_shared.h"

void PerpendicularVector( const vec3_t src, vec3_t dst );
void G_DebugCircleEx( const float *center, float radius, const float *dir, const float *color, int depthTest, int duration );

void CL_AddDebugLine( const float *start, const float *end, const float *color, int depthTest, int duration, int fromServer );

void G_DebugCircleEx( const float *center, float radius, const float *dir, const float *color, int depthTest, int duration );

void G_DebugLine( const float *start, const float *end, const float *color, int depthTest, int duration )
{
	CL_AddDebugLine(start, end, color, depthTest, duration, 0);
}

void G_DebugBox( const float *origin, const float *mins, const float *maxs, float yaw, const float *color, int depthTest, int duration )
{
	static const int iEdgePairs[12][2] =
	{
		{ 0, 1 }, { 0, 2 }, { 0, 4 }, { 1, 3 }, { 1, 5 }, { 2, 3 },
		{ 2, 6 }, { 3, 7 }, { 4, 5 }, { 4, 6 }, { 5, 7 }, { 6, 7 },
	};
	int i;
	int j;
	vec3_t v[8];
	float fSin;
	float fCos;
	vec3_t fRotated;

	FastSinCos(yaw * 0.017453292f, &fSin, &fCos);

	for ( i = 0; i < 8; i++ )
	{
		for ( j = 0; j < 3; j++ )
		{
			v[i][j] = ( i & ( 1 << j ) ) ? maxs[j] : mins[j];
		}
		fRotated[0] = v[i][0] * fCos - v[i][1] * fSin;
		fRotated[1] = v[i][0] * fSin + v[i][1] * fCos;
		Vector2Copy(fRotated, v[i]);
		VectorAdd(v[i], origin, v[i]);
	}

	for ( i = 0; i < sizeof(iEdgePairs) / sizeof(iEdgePairs[0]); i++ )
	{
		G_DebugLine(v[iEdgePairs[i][0]], v[iEdgePairs[i][1]], color, depthTest, duration);
	}
}

void G_DebugCircle( const float *center, float radius, const float *color, int depthTest, int onGround, int duration )
{
	vec3_t normal;
	vec3_t viewOrigin;

	if ( onGround )
	{
		VectorSet(normal, 0.0f, 0.0f, 1.0f);
	}
	else
	{
		VectorCopy(level.clients->ps.origin, viewOrigin);
		viewOrigin[2] += level.clients->ps.viewHeightCurrent;
		VectorSubtract(center, viewOrigin, normal);
	}
	G_DebugCircleEx(center, radius, normal, color, depthTest, duration);
}

void G_DebugCircleEx( const float *center, float radius, const float *dir, const float *color, int depthTest, int duration )
{
	vec3_t verts[16];
	vec3_t normal;
	vec3_t right;
	vec3_t up;
	int i;
	float angle;
	float unused;	// never referenced
	float s;
	float c;

	Vec3NormalizeTo(dir, normal);
	PerpendicularVector(normal, right);
	Vec3Cross(normal, right, up);

	for ( i = 0; i < sizeof(verts) / sizeof(verts[0]); i++ )
	{
		angle = (float)i * 0.39269909f;
		FastSinCos(angle, &s, &c);
		s *= radius;
		c *= radius;
		VectorMA(center, s, up, verts[i]);
		VectorMA(verts[i], c, right, verts[i]);
	}

	for ( i = 0; i < sizeof(verts) / sizeof(verts[0]); i++ )
	{
		G_DebugLine(verts[i], verts[(i + 1) % (sizeof(verts) / sizeof(verts[0]))], color, depthTest, duration);
	}
}

void G_DebugArc( const float *center, float radius, float angle0, float angle1, const float *color, int depthTest, int duration )
{
	vec3_t verts[16];
	int i;
	float angleStep;
	float angle;
	float unused;	// never referenced
	float s;
	float c;

	angleStep = ( angle1 - angle0 ) / 15.0f;
	if ( angleStep < 0.0f )
	{
		angle0 -= 360.0f;
		angleStep = ( angle1 - angle0 ) / 15.0f;
	}

	for ( i = 0; i < sizeof(verts) / sizeof(verts[0]); i++ )
	{
		angle = (( angle0 + i * angleStep ) * 0.017453292f);
		FastSinCos(angle, &s, &c);
		verts[i][0] = center[0] + c * radius;
		verts[i][1] = center[1] + s * radius;
		verts[i][2] = center[2];
	}

	for ( i = 0; i < sizeof(verts) / sizeof(verts[0]) - 1; i++ )
	{
		G_DebugLine(verts[i], verts[i + 1], color, depthTest, duration);
	}
}

