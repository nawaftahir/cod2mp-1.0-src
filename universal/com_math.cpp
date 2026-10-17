#include "../qcommon/qcommon.h"
#include "../xanim/xanim_public.h"

// float consistency for this file only; the <math.h> float wrappers stay intrinsic (retail)
#pragma optimize("p", on)

void PerpendicularVector( const vec3_t src, vec3_t dst );

const vec2_t vec2_origin = { 0, 0 };
const vec3_t vec3_origin = { 0, 0, 0 };
const vec4_t vec4_origin = { 0, 0, 0, 0 };

static vec3_t bytedirs[] =
{
	{ -0.52573103,  0.0,  0.85065103 },
	{ -0.44286299,  0.238856,  0.86418802 },
	{ -0.29524201,  0.0,  0.955423 },
	{ -0.309017,  0.5,  0.809017 },
	{ -0.16246,  0.26286599,  0.951056 },
	{  0.0,  0.0,  1.0 },
	{  0.0,  0.85065103,  0.52573103 },
	{ -0.14762101,  0.71656698,  0.68171799 },
	{  0.14762101,  0.71656698,  0.68171799 },
	{  0.0,  0.52573103,  0.85065103 },
	{  0.309017,  0.5,  0.809017 },
	{  0.52573103,  0.0,  0.85065103 },
	{  0.29524201,  0.0,  0.955423 },
	{  0.44286299,  0.238856,  0.86418802 },
	{  0.16246,  0.26286599,  0.951056 },
	{ -0.68171799,  0.14762101,  0.71656698 },
	{ -0.809017,  0.309017,  0.5 },
	{ -0.58778501,  0.42532501,  0.688191 },
	{ -0.85065103,  0.52573103,  0.0 },
	{ -0.86418802,  0.44286299,  0.238856 },
	{ -0.71656698,  0.68171799,  0.14762101 },
	{ -0.688191,  0.58778501,  0.42532501 },
	{ -0.5,  0.809017,  0.309017 },
	{ -0.238856,  0.86418802,  0.44286299 },
	{ -0.42532501,  0.688191,  0.58778501 },
	{ -0.71656698,  0.68171799, -0.14762101 },
	{ -0.5,  0.809017, -0.309017 },
	{ -0.52573103,  0.85065103,  0.0 },
	{  0.0,  0.85065103, -0.52573103 },
	{ -0.238856,  0.86418802, -0.44286299 },
	{  0.0,  0.955423, -0.29524201 },
	{ -0.26286599,  0.951056, -0.16246 },
	{  0.0,  1.0,  0.0 },
	{  0.0,  0.955423,  0.29524201 },
	{ -0.26286599,  0.951056,  0.16246 },
	{  0.238856,  0.86418802,  0.44286299 },
	{  0.26286599,  0.951056,  0.16246 },
	{  0.5,  0.809017,  0.309017 },
	{  0.238856,  0.86418802, -0.44286299 },
	{  0.26286599,  0.951056, -0.16246 },
	{  0.5,  0.809017, -0.309017 },
	{  0.85065103,  0.52573103,  0.0 },
	{  0.71656698,  0.68171799,  0.14762101 },
	{  0.71656698,  0.68171799, -0.14762101 },
	{  0.52573103,  0.85065103,  0.0 },
	{  0.42532501,  0.688191,  0.58778501 },
	{  0.86418802,  0.44286299,  0.238856 },
	{  0.688191,  0.58778501,  0.42532501 },
	{  0.809017,  0.309017,  0.5 },
	{  0.68171799,  0.14762101,  0.71656698 },
	{  0.58778501,  0.42532501,  0.688191 },
	{  0.955423,  0.29524201,  0.0 },
	{  1.0,  0.0,  0.0 },
	{  0.951056,  0.16246,  0.26286599 },
	{  0.85065103, -0.52573103,  0.0 },
	{  0.955423, -0.29524201,  0.0 },
	{  0.86418802, -0.44286299,  0.238856 },
	{  0.951056, -0.16246,  0.26286599 },
	{  0.809017, -0.309017,  0.5 },
	{  0.68171799, -0.14762101,  0.71656698 },
	{  0.85065103,  0.0,  0.52573103 },
	{  0.86418802,  0.44286299, -0.238856 },
	{  0.809017,  0.309017, -0.5 },
	{  0.951056,  0.16246, -0.26286599 },
	{  0.52573103,  0.0, -0.85065103 },
	{  0.68171799,  0.14762101, -0.71656698 },
	{  0.68171799, -0.14762101, -0.71656698 },
	{  0.85065103,  0.0, -0.52573103 },
	{  0.809017, -0.309017, -0.5 },
	{  0.86418802, -0.44286299, -0.238856 },
	{  0.951056, -0.16246, -0.26286599 },
	{  0.14762101,  0.71656698, -0.68171799 },
	{  0.309017,  0.5, -0.809017 },
	{  0.42532501,  0.688191, -0.58778501 },
	{  0.44286299,  0.238856, -0.86418802 },
	{  0.58778501,  0.42532501, -0.688191 },
	{  0.688191,  0.58778501, -0.42532501 },
	{ -0.14762101,  0.71656698, -0.68171799 },
	{ -0.309017,  0.5, -0.809017 },
	{  0.0,  0.52573103, -0.85065103 },
	{ -0.52573103,  0.0, -0.85065103 },
	{ -0.44286299,  0.238856, -0.86418802 },
	{ -0.29524201,  0.0, -0.955423 },
	{ -0.16246,  0.26286599, -0.951056 },
	{  0.0,  0.0, -1.0 },
	{  0.29524201,  0.0, -0.955423 },
	{  0.16246,  0.26286599, -0.951056 },
	{ -0.44286299, -0.238856, -0.86418802 },
	{ -0.309017, -0.5, -0.809017 },
	{ -0.16246, -0.26286599, -0.951056 },
	{  0.0, -0.85065103, -0.52573103 },
	{ -0.14762101, -0.71656698, -0.68171799 },
	{  0.14762101, -0.71656698, -0.68171799 },
	{  0.0, -0.52573103, -0.85065103 },
	{  0.309017, -0.5, -0.809017 },
	{  0.44286299, -0.238856, -0.86418802 },
	{  0.16246, -0.26286599, -0.951056 },
	{  0.238856, -0.86418802, -0.44286299 },
	{  0.5, -0.809017, -0.309017 },
	{  0.42532501, -0.688191, -0.58778501 },
	{  0.71656698, -0.68171799, -0.14762101 },
	{  0.688191, -0.58778501, -0.42532501 },
	{  0.58778501, -0.42532501, -0.688191 },
	{  0.0, -0.955423, -0.29524201 },
	{  0.0, -1.0,  0.0 },
	{  0.26286599, -0.951056, -0.16246 },
	{  0.0, -0.85065103,  0.52573103 },
	{  0.0, -0.955423,  0.29524201 },
	{  0.238856, -0.86418802,  0.44286299 },
	{  0.26286599, -0.951056,  0.16246 },
	{  0.5, -0.809017,  0.309017 },
	{  0.71656698, -0.68171799,  0.14762101 },
	{  0.52573103, -0.85065103,  0.0 },
	{ -0.238856, -0.86418802, -0.44286299 },
	{ -0.5, -0.809017, -0.309017 },
	{ -0.26286599, -0.951056, -0.16246 },
	{ -0.85065103, -0.52573103,  0.0 },
	{ -0.71656698, -0.68171799, -0.14762101 },
	{ -0.71656698, -0.68171799,  0.14762101 },
	{ -0.52573103, -0.85065103,  0.0 },
	{ -0.5, -0.809017,  0.309017 },
	{ -0.238856, -0.86418802,  0.44286299 },
	{ -0.26286599, -0.951056,  0.16246 },
	{ -0.86418802, -0.44286299,  0.238856 },
	{ -0.809017, -0.309017,  0.5 },
	{ -0.688191, -0.58778501,  0.42532501 },
	{ -0.68171799, -0.14762101,  0.71656698 },
	{ -0.44286299, -0.238856,  0.86418802 },
	{ -0.58778501, -0.42532501,  0.688191 },
	{ -0.309017, -0.5,  0.809017 },
	{ -0.14762101, -0.71656698,  0.68171799 },
	{ -0.42532501, -0.688191,  0.58778501 },
	{ -0.16246, -0.26286599,  0.951056 },
	{  0.44286299, -0.238856,  0.86418802 },
	{  0.16246, -0.26286599,  0.951056 },
	{  0.309017, -0.5,  0.809017 },
	{  0.14762101, -0.71656698,  0.68171799 },
	{  0.0, -0.52573103,  0.85065103 },
	{  0.42532501, -0.688191,  0.58778501 },
	{  0.58778501, -0.42532501,  0.688191 },
	{  0.688191, -0.58778501,  0.42532501 },
	{ -0.955423,  0.29524201,  0.0 },
	{ -0.951056,  0.16246,  0.26286599 },
	{ -1.0,  0.0,  0.0 },
	{ -0.85065103,  0.0,  0.52573103 },
	{ -0.955423, -0.29524201,  0.0 },
	{ -0.951056, -0.16246,  0.26286599 },
	{ -0.86418802,  0.44286299, -0.238856 },
	{ -0.951056,  0.16246, -0.26286599 },
	{ -0.809017,  0.309017, -0.5 },
	{ -0.86418802, -0.44286299, -0.238856 },
	{ -0.951056, -0.16246, -0.26286599 },
	{ -0.809017, -0.309017, -0.5 },
	{ -0.68171799,  0.14762101, -0.71656698 },
	{ -0.68171799, -0.14762101, -0.71656698 },
	{ -0.85065103,  0.0, -0.52573103 },
	{ -0.688191,  0.58778501, -0.42532501 },
	{ -0.58778501,  0.42532501, -0.688191 },
	{ -0.42532501,  0.688191, -0.58778501 },
	{ -0.42532501, -0.688191, -0.58778501 },
	{ -0.58778501, -0.42532501, -0.688191 },
	{ -0.688191, -0.58778501, -0.42532501 }
};

static unsigned int holdrand = 0x89abcdef;

// golden-angle rotation used to spread points along a spiral
#define GOLDEN_SIN -0.67549032f
#define GOLDEN_COS -0.73736888f

float randomf();
float crandom();
void GaussianRandom( float *f0, float *f1 );
void PointInCircleFromUniformDeviates( float radiusDeviate, float yawDeviate, float *point );
void PointOnSphereFromUniformDeviates( float heightDeviate, float yawDeviate, float *point );
void PointOnHemisphereFromUniformDeviates( float heightDeviate, float yawDeviate, float *point );
void SpiralPointsInCircle( int count, float *points, int stride );
void SpiralPointsOnHemisphere( int count, float *points, int stride );
void SpiralPointsOnSphere( int count, float *points, int stride );
float LinearTrack( float tgt, float cur, float rate, float deltaTime );
float LinearTrackAngle( float tgt, float cur, float rate, float deltaTime );
float DiffTrack( float tgt, float cur, float rate, float deltaTime );
float DiffTrackAngle( float tgt, float cur, float rate, float deltaTime );
float GraphGetValueFromFraction( int knotCount, const float (*knots)[2], float fraction );
int Q_log2( int val );
float Q_acos( float c );
signed char ClampChar( int i );
signed short ClampShort( int i );
byte DirToByte( const vec3_t dir );
void ByteToDir( const int b, vec3_t dir );
int VecNCompareCustomEpsilon( const float *v0, const float *v1, float epsilon, int coordCount );
float Vec3Distance( const vec3_t v1, const vec3_t v2 );
float Vec3DistanceSq( const vec3_t v1, const vec3_t v2 );
float DistSqToSegment( const vec3_t p, const vec3_t start, const vec3_t dir, float length );
float Vec2Distance( const vec2_t v1, const vec2_t v2 );
float Vec2DistanceSq( const vec2_t v1, const vec2_t v2 );
void Vec3Cross( const vec3_t v0, const vec3_t v1, vec3_t cross );
int VecLargestAxis( const vec3_t v );
void PickProjectionAxes( const vec3_t normal, int *i, int *j );
vec_t Vec3Normalize( vec3_t v );
vec_t Vec2Normalize( vec3_t v );
vec_t Vec4Normalize( vec4_t v );
vec_t Vec3NormalizeTo( const vec3_t v, vec3_t out );
vec_t Vec2NormalizeTo( const vec2_t v, vec2_t out );
int Vec3PackUnitVec( const vec3_t v );
void Vec3UnpackUnitVec( int packed, vec3_t v );
float Vec3MaxComponent( const vec3_t v );
void MatrixTransformVectorRows( const vec3_t in, const vec3_t mat[3], vec3_t out );
void MatrixTransformVectorCols( const vec3_t in, const vec3_t mat[3], vec3_t out );
void RotatePointAroundVector( vec3_t dst, const vec3_t dir, const vec3_t point, float degrees );
void AxisFromForwardYaw( vec3_t axis[3], float yaw );
void MakeNormalVectors( const vec3_t forward, vec3_t right, vec3_t up );
float vectoyaw( const vec3_t vec );
vec_t vectosignedyaw( vec3_t vec );
float vectopitch( const vec3_t vec );
float vectosignedpitch( const vec3_t vec );
void vectoangles( const vec3_t value1, vec3_t angles );
void vectoanglessigned( const vec3_t value1, vec3_t angles );
void AngleVectors( const vec3_t angles, vec3_t forward, vec3_t right, vec3_t up );
void YawVectors( const float yaw, vec3_t forward, vec3_t right );
void PerpendicularVector( const vec3_t src, vec3_t dst );
void PlaneNormalFromPoints( const vec3_t p0, const vec3_t p1, const vec3_t p2, vec3_t normal );
void ProjectPointOntoLine( const vec3_t point, const vec3_t start, const vec3_t end, vec3_t out );
float PointToLineDistSq( const vec3_t point, const vec3_t start, const vec3_t end );
float PointToBoxDistSq( const vec3_t point, const vec3_t mins, const vec3_t maxs );
void AxisClear( vec3_t axis[3] );
void MatrixIdentity44( float out[4][4] );
void MatrixSet44( float out[4][4], const vec3_t origin, const vec3_t axis[3], float scale );
void MatrixMultiply( const float in1[3][3], const float in2[3][3], float out[3][3] );
void MatrixMultiplyInPlace( const float in1[3][3], float inout[3][3] );
void MatrixMultiply34( const float in1[3][4], const float in2[3][4], float out[3][4] );
void MatrixMultiply43( const float in1[4][3], const float in2[4][3], float out[4][3] );
void MatrixMultiply44( const float in1[4][4], const float in2[4][4], float out[4][4] );
void MatrixTranspose( const float in[3][3], float out[3][3] );
void MatrixTranspose44( const float in[4][4], float out[4][4] );
void MatrixInverse( const float in1[3][3], float out[3][3] );
void MatrixInverseOrthogonal43( const float in[4][3], float out[4][3] );
void MatrixInverse44( const float *mat, float *dst );
void MatrixTransformVector( const vec3_t in1, const vec3_t in2[3], vec3_t out );
void MatrixTransformVector44( const vec4_t in, const float mat[4][4], vec4_t out );
void MatrixTransposeTransformVector( const vec3_t in1, const vec3_t in2[3], vec3_t out );
void MatrixTransformVector43( const vec3_t in1, const float in2[4][3], vec3_t out );
void MatrixTransposeTransformVector43( const vec3_t in1, const float in2[4][3], vec3_t out );
void MatrixTransformVector43Equals( vec3_t inout, const float in2[4][3] );
void RotateVec2( vec2_t v, float degrees );
void QuatMultiply( const float *in1, const float *in2, float *out );
void QuatInverse( const vec4_t in, vec4_t out );
void QuatToAxis( const vec4_t quat, vec3_t axis[3] );
float QuatAxisLengthSq( const vec4_t q );
float SinSquaredDeg( float angle );
float QuatDiffAxisLengthSq( const vec4_t a, const vec4_t b );
float RotationToYaw( const vec2_t rot );
void QuatFromPitch( float angle, vec4_t quat );
void QuatFromYaw( float angle, vec4_t quat );
void QuatFromRoll( float angle, vec4_t quat );
void QuatFromAxisAngle( float angle, const vec3_t axis, vec4_t quat );
void MatrixRotationX( vec3_t m[3], float angle );
void MatrixRotationY( vec3_t m[3], float angle );
void MatrixRotationZ( vec3_t m[3], float angle );
void InfinitePerspectiveMatrix( float mtx[4][4], float fovX, float fovY, float zNear );
void OrthographicMatrix( float mtx[4][4], float width, float height, float depth );
void MatrixForViewer( float mtx[4][4], const vec3_t origin, const vec3_t axis[3] );
unsigned int ColorBytesFromRGB( float r, float g, float b );
unsigned int ColorBytesFromRGBA( float r, float g, float b, float a );
float Vec3NormalizeByMax( const vec3_t v, vec3_t out );
float AngleMod( float a );
float LerpAngle( float from, float to, float frac );
float AngleSubtract( float a1, float a2 );
void AnglesSubtract( const vec3_t v1, const vec3_t v2, vec3_t v3 );
float AngleNormalize360( float angle );
float AngleNormalize180( float angle );
float AngleNormalize360Accurate( float angle );
float AngleNormalize180Accurate( float angle );
float AngleDelta( float angle1, float angle2 );
float RadiusFromBounds( const vec3_t mins, const vec3_t maxs );
float RadiusFromBounds2D( const vec2_t mins, const vec2_t maxs );
void ExpandBoundsByDelta( vec3_t mins, vec3_t maxs, const vec3_t delta );
void ExpandBoundsToWidth( vec3_t mins, vec3_t maxs );
void ShrinkBoundsToHeight( vec3_t mins, vec3_t maxs );
void ClearBounds( vec3_t mins, vec3_t maxs );
void ClearBounds2D( vec2_t mins, vec2_t maxs );
void AddPointToBounds( const vec3_t v, vec3_t mins, vec3_t maxs );
void AddPointToBounds2D( const vec2_t v, vec2_t mins, vec2_t maxs );
int PointInBounds( const vec3_t v, const vec3_t mins, const vec3_t maxs );
int PointInBounds2D( const vec2_t v, const vec2_t mins, const vec2_t maxs );
int BoundsOverlap( const vec3_t mins1, const vec3_t maxs1, const vec3_t mins2, const vec3_t maxs2 );
int BoundsOverlap2D( const vec2_t mins1, const vec2_t maxs1, const vec2_t mins2, const vec2_t maxs2 );
int BoundsOverlapEpsilon( const vec3_t mins1, const vec3_t maxs1, const vec3_t mins2, const vec3_t maxs2, float epsilon );
int BoundsOverlapEpsilon2D( const vec2_t mins1, const vec2_t maxs1, const vec2_t mins2, const vec2_t maxs2, float epsilon );
void ExpandBounds( const vec3_t addedmins, const vec3_t addedmaxs, vec3_t mins, vec3_t maxs );
void ExpandBounds2D( const vec2_t addedmins, const vec2_t addedmaxs, vec2_t mins, vec2_t maxs );
void TransformBounds( const vec3_t bounds[2], const vec3_t origin, const vec3_t axis[3], vec3_t out[2] );
void TransformBoundsExpand( const vec3_t bounds[2], const vec3_t origin, const vec3_t axis[3], vec3_t out[2] );
void AxisIdentity( vec3_t axis[3] );
void AxisCopy( const vec3_t in[3], vec3_t out[3] );
void AxisTranspose( const vec3_t in[3], vec3_t out[3] );
void AxisTransformComponents( const vec3_t axis[3], float x, float y, float z, vec3_t out );
void AnglesToAxis( const vec3_t angles, vec3_t axis[3] );
void YawToAxis( float yaw, vec3_t axis[3] );
void AxisToAngles( vec3_t axis[3], vec3_t angles );
void AxisToSignedAngles( const vec3_t axis[3], vec3_t angles );
int IntersectPlanes( const float **plane, vec3_t xyz );
void SnapPointToIntersectingPlanes( const float **planes, vec3_t xyz, float snapGrid, float snapEpsilon );
void SnapPointToPlanes( const vec4_t *planes, int planeCount, vec3_t xyz, float snapGrid, float snapEpsilon );
int PointInPolygon( const vec3_t *verts, int vertCount, const vec3_t normal, const vec3_t point );
int PlaneFromPoints( vec4_t plane, const vec3_t a, const vec3_t b, const vec3_t c );
void ProjectPointOnPlane( const vec3_t p, const vec3_t normal, vec3_t dst );
void SetPlaneSignbits( cplane_t *out );
int BoxOnPlaneSide( vec3_t emins, vec3_t emaxs, struct cplane_s *p );
int PointInCylinderArc( const vec3_t point, float radius, const vec3_t center, float thickness, float minYaw, float maxYaw, float halfHeight );
int BoxDistSqrdExceeds( const vec3_t absmin, const vec3_t absmax, const vec3_t org, float fogOpaqueDistSqrd );
float Q_round( float x );
float Vec3ScaleToUnitMax( const vec3_t v, vec3_t out );
void VectorRotate( const vec3_t vIn, const vec3_t vRotation, vec3_t out );
void VectorRotateAroundOrigin( const vec3_t point, const vec3_t angles, const vec3_t origin, vec3_t out );
void SphericalToCartesian( vec3_t out, float radius, float angle );
float PitchForYawOnNormal( const float fYaw, const vec3_t normal );
void Rand_Init( int seed );
float flrand( float min, float max );
int irand( int min, int max );
void TransformVectorByAnimMat( const float *in, const DObjAnimMat *mat, float *out );
void AxisToQuat( const vec3_t mat[3], vec4_t out );
void SinCosDegrees( float angle, float *pSin, float *pCos );
float SnapFloatBits( float f, int tolerance, int bits );
float SnapFloatToGrid( float value, float grid, float epsilon );

extern const float identityMatrix44[4][4];
static const float identityMatrix44[4][4] =
{
	{ 1, 0, 0, 0 },
	{ 0, 1, 0, 0 },
	{ 0, 0, 1, 0 },
	{ 0, 0, 0, 1 }
};

/*
==============
randomf
==============
*/
float randomf()
{
	return rand() / 32768.0f;
}

/*
==============
crandom
==============
*/
float crandom()
{
	return randomf() * 2.0f - 1.0f;
}

/*
==============
GaussianRandom

Two independent normally distributed values (polar Box-Muller).
==============
*/
void GaussianRandom( float *f0, float *f1 )
{
	float x;
	float y;
	float lenSq;

	do
	{
		x = crandom();
		y = crandom();
		lenSq = x * x + y * y;
	}
	while ( lenSq > 1.0f );

	lenSq = I_sqrt(log(lenSq) * -2.0f / lenSq);
	*f0 = x * lenSq;
	*f1 = y * lenSq;
}

/*
==============
PointInCircleFromUniformDeviates
==============
*/
void PointInCircleFromUniformDeviates( float radiusDeviate, float yawDeviate, float *point )
{
	float deviate;
	float radius;
	float yaw;
	float yawSin;
	float yawCos;

	deviate = radiusDeviate;
	radius = I_sqrt(deviate);
	yaw = yawDeviate * 6.283185307179586f;
	FastSinCos(yaw, &yawSin, &yawCos);
	point[0] = radius * yawCos;
	point[1] = radius * yawSin;
}

/*
==============
PointOnSphereFromUniformDeviates
==============
*/
void PointOnSphereFromUniformDeviates( float heightDeviate, float yawDeviate, float *point )
{
	float height;
	float radius;
	float yaw;
	float yawSin;
	float yawCos;

	height = heightDeviate * 2.0f - 1.0f;
	radius = I_sqrt(1.0f - height * height);
	yaw = yawDeviate * 6.283185307179586f;
	FastSinCos(yaw, &yawSin, &yawCos);
	point[0] = radius * yawCos;
	point[1] = radius * yawSin;
	point[2] = height;
}

/*
==============
PointOnHemisphereFromUniformDeviates
==============
*/
void PointOnHemisphereFromUniformDeviates( float heightDeviate, float yawDeviate, float *point )
{
	float height;
	float radius;
	float yaw;
	float yawSin;
	float yawCos;

	height = heightDeviate;
	radius = I_sqrt(1.0f - height * height);
	yaw = yawDeviate * 6.283185307179586f;
	FastSinCos(yaw, &yawSin, &yawCos);
	point[0] = radius * yawCos;
	point[1] = radius * yawSin;
	point[2] = height;
}

/*
==============
SpiralPointsInCircle

Evenly spread points in the unit circle; stride is in bytes.
==============
*/
void SpiralPointsInCircle( int count, float *points, int stride )
{
	int i;
	float spiralSin = GOLDEN_SIN;
	float spiralCos = GOLDEN_COS;
	float step = 1.0f / count;
	float deviate = step * 0.5f;
	float radius;
	float oldSin;
	float yawSin = 0.0f;
	float yawCos = 1.0f;

	for ( i = 0; ; ++i )
	{
		if ( i >= count )
			break;
		radius = I_sqrt(deviate);
		points[0] = radius * yawCos;
		points[1] = radius * yawSin;
		deviate = deviate + step;
		oldSin = yawSin;
		yawSin = yawSin * spiralCos + yawCos * spiralSin;
		yawCos = yawCos * spiralCos - oldSin * spiralSin;
		points = (float *)((char *)points + stride);
	}
}

/*
==============
SpiralPointsOnHemisphere
==============
*/
void SpiralPointsOnHemisphere( int count, float *points, int stride )
{
	int i;
	float spiralSin = GOLDEN_SIN;
	float spiralCos = GOLDEN_COS;
	float step = 1.0f / count;
	float deviate = step * 0.5f;
	float height;
	float radius;
	float oldSin;
	float yawSin = 0.0f;
	float yawCos = 1.0f;

	for ( i = 0; ; ++i )
	{
		if ( i >= count )
			break;
		height = deviate;
		radius = I_sqrt(1.0f - height * height);
		points[0] = radius * yawCos;
		points[1] = radius * yawSin;
		points[2] = height;
		deviate = deviate + step;
		oldSin = yawSin;
		yawSin = yawSin * spiralCos + yawCos * spiralSin;
		yawCos = yawCos * spiralCos - oldSin * spiralSin;
		points = (float *)((char *)points + stride);
	}
}

/*
==============
SpiralPointsOnSphere
==============
*/
void SpiralPointsOnSphere( int count, float *points, int stride )
{
	int i;
	float spiralSin = GOLDEN_SIN;
	float spiralCos = GOLDEN_COS;
	float step = 1.0f / count;
	float deviate = step * 0.5f;
	float height;
	float radius;
	float oldSin;
	float yawSin = 0.0f;
	float yawCos = 1.0f;

	for ( i = 0; ; ++i )
	{
		if ( i >= count )
			break;
		height = deviate * 2.0f - 1.0f;
		radius = I_sqrt(1.0f - height * height);
		points[0] = radius * yawCos;
		points[1] = radius * yawSin;
		points[2] = height;
		deviate = deviate + step;
		oldSin = yawSin;
		yawSin = yawSin * spiralCos + yawCos * spiralSin;
		yawCos = yawCos * spiralCos - oldSin * spiralSin;
		points = (float *)((char *)points + stride);
	}
}

/*
==============
LinearTrack

Moves cur toward tgt at a constant rate.
==============
*/
float LinearTrack( float tgt, float cur, float rate, float deltaTime )
{
	float err;
	float step;

	err = tgt - cur;

	if ( err > 0.0f )
		step = rate * deltaTime;
	else
		step = -rate * deltaTime;

	if ( I_fabs(err) > 0.001f )
	{
		if ( I_fabs(err) < I_fabs(step) )
			return tgt;
		else
			return cur + step;
	}
	else
		return tgt;
}

/*
==============
LinearTrackAngle
==============
*/
float LinearTrackAngle( float tgt, float cur, float rate, float deltaTime )
{
	float angle;

	while ( tgt - cur > 180.0f )
		tgt = tgt - 360.0f;
	while ( tgt - cur < -180.0f )
		tgt = tgt + 360.0f;

	angle = LinearTrack(tgt, cur, rate, deltaTime);
	return AngleNormalize180(angle);
}

/*
==============
DiffTrack

Moves cur toward tgt in proportion to the remaining error.
==============
*/
float DiffTrack( float tgt, float cur, float rate, float deltaTime )
{
	float err;
	float step;

	err = tgt - cur;
	step = rate * err * deltaTime;

	if ( I_fabs(err) > 0.001f )
	{
		if ( I_fabs(err) < I_fabs(step) )
			return tgt;
		else
			return cur + step;
	}
	else
		return tgt;
}

/*
==============
DiffTrackAngle
==============
*/
float DiffTrackAngle( float tgt, float cur, float rate, float deltaTime )
{
	float angle;

	while ( tgt - cur > 180.0f )
		tgt = tgt - 360.0f;
	while ( tgt - cur < -180.0f )
		tgt = tgt + 360.0f;

	angle = DiffTrack(tgt, cur, rate, deltaTime);
	return AngleNormalize180(angle);
}

/*
==============
GraphGetValueFromFraction

knots are (fraction, value) pairs in increasing fraction order.
==============
*/
float GraphGetValueFromFraction( int knotCount, const float (*knots)[2], float fraction )
{
	int i;
	float result;
	float adjustedFrac;

	result = -1.0f;

	for ( i = 1; i < knotCount; ++i )
	{
		if ( fraction <= knots[i][0] )
		{
			adjustedFrac = (fraction - knots[i - 1][0]) / (knots[i][0] - knots[i - 1][0]);
			result = knots[i - 1][1] + (knots[i][1] - knots[i - 1][1]) * adjustedFrac;
			break;
		}
	}

	return result;
}

/*
==============
Q_log2
==============
*/
int Q_log2( int val )
{
	int answer;

	answer = 0;

	while ( val >>= 1 )
		answer++;

	return answer;
}

/*
==============
Q_acos
==============
*/
float Q_acos( float c )
{
	float angle;

	angle = acos( c );

	if ( angle > M_PI )
	{
		return (float)M_PI;
	}

	if ( angle < -M_PI )
	{
		return (float)M_PI;
	}

	return angle;
}

/*
==============
ClampChar
==============
*/
signed char ClampChar( int i )
{
	if ( i < -128 )
		return -128;
	if ( i > 127 )
		return 127;
	return i;
}

/*
==============
ClampShort
==============
*/
signed short ClampShort( int i )
{
	if ( i < -32768 )
		return -32768;
	if ( i > 32767 )
		return 32767;
	return i;
}

/*
==============
DirToByte
==============
*/
byte DirToByte( const vec3_t dir )
{
	byte i;
	byte best;
	float d;
	float bestd;

	if ( !dir )
	{
		return 0;
	}

	bestd = 0.0f;
	best = 0;

	for ( i = 0; i < sizeof(bytedirs) / sizeof(bytedirs[0]); ++i )
	{
		d = DotProduct(dir, bytedirs[i]);

		if ( d > bestd )
		{
			bestd = d;
			best = i;
		}
	}

	return best;
}

/*
==============
ByteToDir
==============
*/
void ByteToDir( const int b, vec3_t dir )
{
	if ( b < 0 || b >= 162 )
	{
		VectorCopy(vec3_origin, dir);
	}
	else
	{
		VectorCopy(bytedirs[b], dir);
	}
}

/*
==============
VecNCompareCustomEpsilon
==============
*/
int VecNCompareCustomEpsilon( const float *v0, const float *v1, float epsilon, int coordCount )
{
	int i;

	for ( i = 0; i < coordCount; i++ )
	{
		if ( (v0[i] - v1[i]) * (v0[i] - v1[i]) > epsilon * epsilon )
			return 0;
	}

	return 1;
}

/*
==============
Vec3Distance
==============
*/
float Vec3Distance( const vec3_t v1, const vec3_t v2 )
{
	vec3_t dir;

	VectorSubtract(v2, v1, dir);
	return VectorLength(dir);
}

/*
==============
Vec3DistanceSq
==============
*/
float Vec3DistanceSq( const vec3_t v1, const vec3_t v2 )
{
	vec3_t dir;

	VectorSubtract(v2, v1, dir);
	return dir[0] * dir[0] + dir[1] * dir[1] + dir[2] * dir[2];
}

/*
==============
DistSqToSegment

Squared distance from p to the segment start + dir * [0, length]; dir is unit length.
==============
*/
float DistSqToSegment( const vec3_t p, const vec3_t start, const vec3_t dir, float length )
{
	vec3_t diff;
	vec3_t closest;
	float t;
	float distSq;

	VectorSubtract(p, start, diff);
	t = DotProduct(diff, dir);
	t = I_fclamp(t, 0.0f, length);
	VectorMA(start, t, dir, closest);
	distSq = Vec3DistanceSq(p, closest);
	return distSq;
}

/*
==============
Vec2Distance
==============
*/
float Vec2Distance( const vec2_t v1, const vec2_t v2 )
{
	vec2_t dir;

	Vector2Subtract(v2, v1, dir);
	return Vec2Length(dir);
}

/*
==============
Vec2DistanceSq
==============
*/
float Vec2DistanceSq( const vec2_t v1, const vec2_t v2 )
{
	vec2_t dir;

	Vector2Subtract(v2, v1, dir);
	return dir[0] * dir[0] + dir[1] * dir[1];
}

/*
==============
Vec3Cross
==============
*/
void Vec3Cross( const vec3_t v0, const vec3_t v1, vec3_t cross )
{
	cross[0] = v0[1] * v1[2] - v0[2] * v1[1];
	cross[1] = v0[2] * v1[0] - v0[0] * v1[2];
	cross[2] = v0[0] * v1[1] - v0[1] * v1[0];
}

/*
==============
VecLargestAxis
==============
*/
int VecLargestAxis( const vec3_t v )
{
	vec3_t sq;
	int i;

	Vec3Mul(v, v, sq);
	i = sq[1] > sq[0];

	if ( sq[2] > sq[i] )
		i = 2;

	return i;
}

/*
==============
PickProjectionAxes

Picks the two axes of the plane most facing normal, wound to keep its facing.
==============
*/
void PickProjectionAxes( const vec3_t normal, int *i, int *j )
{
	vec3_t sq;

	Vec3Mul(normal, normal, sq);

	if ( sq[2] >= sq[0] && sq[2] >= sq[1] )
	{
		if ( normal[2] > 0 )
		{
			*i = 0;
			*j = 1;
		}
		else
		{
			*i = 1;
			*j = 0;
		}
	}
	else if ( sq[1] >= sq[0] && sq[1] >= sq[2] )
	{
		if ( normal[1] > 0 )
		{
			*i = 2;
			*j = 0;
		}
		else
		{
			*i = 0;
			*j = 2;
		}
	}
	else
	{
		if ( normal[0] > 0 )
		{
			*i = 1;
			*j = 2;
		}
		else
		{
			*i = 2;
			*j = 1;
		}
	}
}

/*
==============
Vec3Normalize
==============
*/
vec_t Vec3Normalize( vec3_t v )
{
	float length, ilength;

	length = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
	length = I_sqrt( length );

	if ( length )
	{
		ilength = 1 / length;
		v[0] *= ilength;
		v[1] *= ilength;
		v[2] *= ilength;
	}

	return length;
}

/*
==============
Vec2Normalize
==============
*/
vec_t Vec2Normalize( vec3_t v )
{
	float length, ilength;

	length = v[0] * v[0] + v[1] * v[1];
	length = I_sqrt( length );

	if ( length )
	{
		ilength = 1 / length;
		v[0] *= ilength;
		v[1] *= ilength;
	}

	return length;
}

/*
==============
Vec4Normalize
==============
*/
vec_t Vec4Normalize( vec4_t v )
{
	float length, ilength;

	length = v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3];
	length = I_sqrt( length );

	if ( length )
	{
		ilength = 1 / length;
		v[0] *= ilength;
		v[1] *= ilength;
		v[2] *= ilength;
		v[3] *= ilength;
	}

	return length;
}

/*
==============
Vec3NormalizeTo
==============
*/
vec_t Vec3NormalizeTo( const vec3_t v, vec3_t out )
{
	float length;
	float ilength;

	length = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
	length = I_sqrt(length);

	if ( length != 0.0f )
	{
		ilength = 1.0f / length;
		out[0] = v[0] * ilength;
		out[1] = v[1] * ilength;
		out[2] = v[2] * ilength;
	}
	else
	{
		VectorClear(out);
	}

	return length;
}

/*
==============
Vec2NormalizeTo
==============
*/
vec_t Vec2NormalizeTo( const vec2_t v, vec2_t out )
{
	float length;
	float ilength;

	length = v[0] * v[0] + v[1] * v[1];
	length = I_sqrt(length);

	if ( length != 0.0f )
	{
		ilength = 1.0f / length;
		out[0] = v[0] * ilength;
		out[1] = v[1] * ilength;
	}
	else
	{
		Vector2Clear(out);
	}

	return length;
}

/*
==============
Vec3PackUnitVec

Packs a unit vector into three signed 10-bit fields.
==============
*/
int Vec3PackUnitVec( const vec3_t v )
{
	int x;
	int y;
	int z;

	x = I_fround(v[0] * 511.0f) & 0x3ff;
	y = I_fround(v[1] * 511.0f) & 0x3ff;
	z = I_fround(v[2] * 511.0f) & 0x3ff;

	return x | (y << 10) | (z << 20);
}

/*
==============
Vec3UnpackUnitVec
==============
*/
void Vec3UnpackUnitVec( int packed, vec3_t v )
{
	int x;
	int y;
	int z;

	x = ((packed & 0x3ff) << 22) >> 22;
	y = (((packed >> 10) & 0x3ff) << 22) >> 22;
	z = (((packed >> 20) & 0x3ff) << 22) >> 22;

	v[0] = x * (1.0f / 511.0f);
	v[1] = y * (1.0f / 511.0f);
	v[2] = z * (1.0f / 511.0f);
}

/*
==============
Vec3MaxComponent
==============
*/
float Vec3MaxComponent( const vec3_t v )
{
	float m;

	m = v[0] >= v[1] ? v[0] : v[1];
	return m >= v[2] ? m : v[2];
}

/*
==============
MatrixTransformVectorRows
==============
*/
void MatrixTransformVectorRows( const vec3_t in, const vec3_t mat[3], vec3_t out )
{
	out[0] = in[0] * mat[0][0] + in[1] * mat[0][1] + in[2] * mat[0][2];
	out[1] = in[0] * mat[1][0] + in[1] * mat[1][1] + in[2] * mat[1][2];
	out[2] = in[0] * mat[2][0] + in[1] * mat[2][1] + in[2] * mat[2][2];
}

/*
==============
MatrixTransformVectorCols
==============
*/
void MatrixTransformVectorCols( const vec3_t in, const vec3_t mat[3], vec3_t out )
{
	out[0] = in[0] * mat[0][0] + in[1] * mat[1][0] + in[2] * mat[2][0];
	out[1] = in[0] * mat[0][1] + in[1] * mat[1][1] + in[2] * mat[2][1];
	out[2] = in[0] * mat[0][2] + in[1] * mat[1][2] + in[2] * mat[2][2];
}

/*
==============
RotatePointAroundVector
==============
*/
void RotatePointAroundVector( vec3_t dst, const vec3_t dir, const vec3_t point, float degrees )
{
	float m[3][3];
	float im[3][3];
	float zrot[3][3];
	float tmpmat[3][3];
	float rot[3][3];
	int i;
	vec3_t vr, vup, vf;
	float rad;

	vf[0] = dir[0];
	vf[1] = dir[1];
	vf[2] = dir[2];

	PerpendicularVector(dir, vr);
	Vec3Cross(vr, vf, vup);

	m[0][0] = vr[0];
	m[1][0] = vr[1];
	m[2][0] = vr[2];

	m[0][1] = vup[0];
	m[1][1] = vup[1];
	m[2][1] = vup[2];

	m[0][2] = vf[0];
	m[1][2] = vf[1];
	m[2][2] = vf[2];

	memcpy(im, m, sizeof(im));

	im[0][1] = m[1][0];
	im[0][2] = m[2][0];
	im[1][0] = m[0][1];
	im[1][2] = m[2][1];
	im[2][0] = m[0][2];
	im[2][1] = m[1][2];

	memset(zrot, 0, sizeof(zrot));
	zrot[0][0] = zrot[1][1] = zrot[2][2] = 1.0f;

	rad = (degrees * 0.017453292519943295f);
	FastSinCos(rad, &zrot[0][1], &zrot[0][0]);
	zrot[1][0] = -zrot[0][1];
	zrot[1][1] = zrot[0][0];

	MatrixMultiply(m, zrot, tmpmat);
	MatrixMultiply(tmpmat, im, rot);

	for ( i = 0; i < 3; i++ )
	{
		dst[i] = rot[i][0] * point[0] + rot[i][1] * point[1] + rot[i][2] * point[2];
	}
}

/*
==============
AxisFromForwardYaw

Builds an axis around axis[0], rolled by yaw degrees.
==============
*/
void AxisFromForwardYaw( vec3_t axis[3], float yaw )
{
	vec3_t dir;

	PerpendicularVector(axis[0], axis[1]);

	if ( yaw != 0 )
	{
		VectorCopy(axis[1], dir);
		RotatePointAroundVector(axis[1], axis[0], dir, yaw);
	}

	Vec3Cross(axis[0], axis[1], axis[2]);
}

/*
==============
MakeNormalVectors
==============
*/
void MakeNormalVectors( const vec3_t forward, vec3_t right, vec3_t up )
{
	float d;

	// this rotate and negate guarantees a vector not colinear with the original
	right[1] = -forward[0];
	right[2] = forward[1];
	right[0] = forward[2];

	d = DotProduct(right, forward);
	VectorMA(right, -d, forward, right);
	Vec3Normalize(right);
	Vec3Cross(right, forward, up);
}

/*
==============
vectoyaw
==============
*/
float vectoyaw( const vec3_t vec )
{
	float yaw;

	if ( vec[1] == 0 && vec[0] == 0 )
	{
		yaw = 0;
	}
	else
	{
		yaw = atan2(vec[1], vec[0]) * 180.0f / M_PI;

		if ( yaw < 0 )
		{
			yaw += 360.0f;
		}
	}

	return yaw;
}

/*
==============
vectosignedyaw
==============
*/
vec_t vectosignedyaw( vec3_t vec )
{
	float yaw;

	if ( vec[1] == 0.0f && vec[0] == 0.0f )
	{
		yaw = 0.0f;
	}
	else
	{
		yaw = atan2(vec[1], vec[0]) * 180.0f / M_PI;
	}

	return yaw;
}

/*
==============
vectopitch
==============
*/
float vectopitch( const vec3_t vec )
{
	float forward;
	float pitch;

	if ( vec[1] == 0 && vec[0] == 0 )
	{
		if ( vec[2] > 0 )
		{
			pitch = 270;
		}
		else
		{
			pitch = 90;
		}
	}
	else
	{
		forward = I_sqrt(vec[0] * vec[0] + vec[1] * vec[1]);
		pitch = atan2(vec[2], forward) * -180.0f / M_PI;

		if ( pitch < 0 )
		{
			pitch += 360.0f;
		}
	}

	return pitch;
}

/*
==============
vectosignedpitch
==============
*/
float vectosignedpitch( const vec3_t vec )
{
	float forward;
	float pitch;

	if ( vec[1] == 0 && vec[0] == 0 )
	{
		if ( vec[2] > 0 )
		{
			pitch = -90;
		}
		else
		{
			pitch = 90;
		}
	}
	else
	{
		forward = I_sqrt(vec[0] * vec[0] + vec[1] * vec[1]);
		pitch = atan2(vec[2], forward) * -180.0f / M_PI;
	}

	return pitch;
}

/*
==============
vectoangles
==============
*/
void vectoangles( const vec3_t value1, vec3_t angles )
{
	float forward;
	float yaw, pitch;

	if ( value1[1] == 0 && value1[0] == 0 )
	{
		yaw = 0;
		if ( value1[2] > 0 )
		{
			pitch = 270;
		}
		else
		{
			pitch = 90;
		}
	}
	else
	{
		yaw = ( atan2( value1[1], value1[0] ) * 180 / M_PI );
		if ( yaw < 0.0f )
		{
			yaw += 360.0f;
		}
		forward = I_sqrt( value1[0] * value1[0] + value1[1] * value1[1] );
		pitch = ( atan2( value1[2], forward ) * -180 / M_PI );
		if ( pitch < 0.0f )
		{
			pitch += 360.0f;
		}
	}

	angles[PITCH] = pitch;
	angles[YAW] = yaw;
	angles[ROLL] = 0;
}

/*
==============
vectoanglessigned
==============
*/
void vectoanglessigned( const vec3_t value1, vec3_t angles )
{
	float forward;
	float yaw, pitch;

	if ( value1[1] == 0 && value1[0] == 0 )
	{
		yaw = 0;
		if ( value1[2] > 0 )
		{
			pitch = -90;
		}
		else
		{
			pitch = 90;
		}
	}
	else
	{
		yaw = atan2(value1[1], value1[0]) * 180.0f / M_PI;
		forward = I_sqrt(value1[0] * value1[0] + value1[1] * value1[1]);
		pitch = atan2(value1[2], forward) * -180.0f / M_PI;
	}

	angles[PITCH] = pitch;
	angles[YAW] = yaw;
	angles[ROLL] = 0;
}

/*
==============
AngleVectors
==============
*/
void AngleVectors( const vec3_t angles, vec3_t forward, vec3_t right, vec3_t up )
{
	float angle;
	float sr, sp, sy, cr, cp, cy;

	angle = angles[YAW] * (float)( M_PI * 2 / 360 );
	FastSinCos( angle, &sy, &cy );

	angle = angles[PITCH] * (float)( M_PI * 2 / 360 );
	FastSinCos( angle, &sp, &cp );

	if ( forward )
	{
		forward[0] = cp * cy;
		forward[1] = cp * sy;
		forward[2] = -sp;
	}

	if ( right || up )
	{
		angle = angles[ROLL] * (float)( M_PI * 2 / 360 );
		FastSinCos( angle, &sr, &cr );

		if ( right )
		{
			right[0] = -1 * sr * sp * cy + -1 * cr * -sy;
			right[1] = -1 * sr * sp * sy + -1 * cr * cy;
			right[2] = -1 * sr * cp;
		}

		if ( up )
		{
			up[0] = cr * sp * cy + -sr * -sy;
			up[1] = cr * sp * sy + -sr * cy;
			up[2] = cr * cp;
		}
	}
}

/*
==============
YawVectors
==============
*/
void YawVectors( const float yaw, vec3_t forward, vec3_t right )
{
	float angle;
	float sy;
	float cy;

	angle = yaw * 0.017453292519943295f;
	FastSinCos(angle, &sy, &cy);

	if ( forward )
	{
		forward[0] = cy;
		forward[1] = sy;
		forward[2] = 0;
	}

	if ( right )
	{
		right[0] = sy;
		right[1] = -cy;
		right[2] = 0;
	}
}

/*
==============
PerpendicularVector
==============
*/
void PerpendicularVector( const vec3_t src, vec3_t dst )
{
	int best;
	vec3_t sq;
	float scale;

	sq[0] = src[0] * src[0];
	sq[1] = src[1] * src[1];
	sq[2] = src[2] * src[2];

	best = sq[1] < sq[0];

	if ( sq[2] < sq[best] )
	{
		best = 2;
	}

	scale = -src[best];
	VectorScale(src, scale, dst);
	dst[best] += 1.0f;
	Vec3Normalize(dst);
}

/*
==============
PlaneNormalFromPoints
==============
*/
void PlaneNormalFromPoints( const vec3_t p0, const vec3_t p1, const vec3_t p2, vec3_t normal )
{
	vec3_t d1;
	vec3_t d2;

	VectorSubtract(p0, p1, d1);
	Vec3Normalize(d1);
	VectorSubtract(p0, p2, d2);
	Vec3Normalize(d2);
	Vec3Cross(d1, d2, normal);
	Vec3Normalize(normal);
}

/*
==============
ProjectPointOntoLine
==============
*/
void ProjectPointOntoLine( const vec3_t point, const vec3_t start, const vec3_t end, vec3_t out )
{
	vec3_t dir;
	vec3_t v;
	float lenSq;
	float dot;

	VectorSubtract(end, start, dir);
	VectorSubtract(point, start, v);
	dot = DotProduct(v, dir);
	lenSq = DotProduct(dir, dir);

	if ( lenSq == 0 )
	{
		VectorCopy(start, out);
	}
	else
	{
		VectorMA(start, dot / lenSq, dir, out);
	}
}

/*
==============
PointToLineDistSq
==============
*/
float PointToLineDistSq( const vec3_t point, const vec3_t start, const vec3_t end )
{
	vec3_t dir;
	vec3_t v;
	vec3_t perp;
	float lenSq;
	float dot;

	VectorSubtract(end, start, dir);
	VectorSubtract(point, start, v);
	dot = DotProduct(v, dir);
	lenSq = DotProduct(dir, dir);
	VectorMA(v, -(dot / lenSq), dir, perp);

	return DotProduct(perp, perp);
}

/*
==============
PointToBoxDistSq
==============
*/
float PointToBoxDistSq( const vec3_t point, const vec3_t mins, const vec3_t maxs )
{
	int i;
	float distSq;
	float d;

	distSq = 0;

	for ( i = 0; i < 3; i++ )
	{
		d = mins[i] - point[i];

		if ( d > 0 )
		{
			distSq += d * d;
		}
		else
		{
			d = point[i] - maxs[i];

			if ( d > 0 )
			{
				distSq += d * d;
			}
		}
	}

	return distSq;
}

/*
==============
AxisClear
==============
*/
void AxisClear( vec3_t axis[3] )
{
	memset(axis, 0, 36);
	axis[0][0] = 1.0f;
	axis[1][1] = 1.0f;
	axis[2][2] = 1.0f;
}

/*
==============
MatrixIdentity44
==============
*/
void MatrixIdentity44( float out[4][4] )
{
	memcpy(out, identityMatrix44, 64);
}

/*
==============
MatrixSet44
==============
*/
void MatrixSet44( float out[4][4], const vec3_t origin, const vec3_t axis[3], float scale )
{
	out[0][0] = axis[0][0] * scale;
	out[0][1] = axis[0][1] * scale;
	out[0][2] = axis[0][2] * scale;
	out[0][3] = 0.0f;
	out[1][0] = axis[1][0] * scale;
	out[1][1] = axis[1][1] * scale;
	out[1][2] = axis[1][2] * scale;
	out[1][3] = 0.0f;
	out[2][0] = axis[2][0] * scale;
	out[2][1] = axis[2][1] * scale;
	out[2][2] = axis[2][2] * scale;
	out[2][3] = 0.0f;
	out[3][0] = origin[0];
	out[3][1] = origin[1];
	out[3][2] = origin[2];
	out[3][3] = 1.0f;
}

/*
==============
MatrixMultiply
==============
*/
void MatrixMultiply( const float in1[3][3], const float in2[3][3], float out[3][3] )
{
	out[0][0] = in1[0][0] * in2[0][0] + in1[0][1] * in2[1][0] + in1[0][2] * in2[2][0];
	out[0][1] = in1[0][0] * in2[0][1] + in1[0][1] * in2[1][1] + in1[0][2] * in2[2][1];
	out[0][2] = in1[0][0] * in2[0][2] + in1[0][1] * in2[1][2] + in1[0][2] * in2[2][2];
	out[1][0] = in1[1][0] * in2[0][0] + in1[1][1] * in2[1][0] + in1[1][2] * in2[2][0];
	out[1][1] = in1[1][0] * in2[0][1] + in1[1][1] * in2[1][1] + in1[1][2] * in2[2][1];
	out[1][2] = in1[1][0] * in2[0][2] + in1[1][1] * in2[1][2] + in1[1][2] * in2[2][2];
	out[2][0] = in1[2][0] * in2[0][0] + in1[2][1] * in2[1][0] + in1[2][2] * in2[2][0];
	out[2][1] = in1[2][0] * in2[0][1] + in1[2][1] * in2[1][1] + in1[2][2] * in2[2][1];
	out[2][2] = in1[2][0] * in2[0][2] + in1[2][1] * in2[1][2] + in1[2][2] * in2[2][2];
}

/*
==============
MatrixMultiplyInPlace

inout = in1 * inout
==============
*/
void MatrixMultiplyInPlace( const float in1[3][3], float inout[3][3] )
{
	float temp[2][3];

	temp[0][0] = in1[0][0] * inout[0][0] + in1[0][1] * inout[1][0] + in1[0][2] * inout[2][0];
	temp[0][1] = in1[0][0] * inout[0][1] + in1[0][1] * inout[1][1] + in1[0][2] * inout[2][1];
	temp[0][2] = in1[0][0] * inout[0][2] + in1[0][1] * inout[1][2] + in1[0][2] * inout[2][2];
	temp[1][0] = in1[1][0] * inout[0][0] + in1[1][1] * inout[1][0] + in1[1][2] * inout[2][0];
	temp[1][1] = in1[1][0] * inout[0][1] + in1[1][1] * inout[1][1] + in1[1][2] * inout[2][1];
	temp[1][2] = in1[1][0] * inout[0][2] + in1[1][1] * inout[1][2] + in1[1][2] * inout[2][2];
	inout[2][0] = in1[2][0] * inout[0][0] + in1[2][1] * inout[1][0] + in1[2][2] * inout[2][0];
	inout[2][1] = in1[2][0] * inout[0][1] + in1[2][1] * inout[1][1] + in1[2][2] * inout[2][1];
	inout[2][2] = in1[2][0] * inout[0][2] + in1[2][1] * inout[1][2] + in1[2][2] * inout[2][2];

	VectorCopy(temp[0], inout[0]);
	VectorCopy(temp[1], inout[1]);
}

/*
==============
MatrixMultiply34
==============
*/
void MatrixMultiply34( const float in1[3][4], const float in2[3][4], float out[3][4] )
{
	out[0][0] = in1[0][0] * in2[0][0] + in1[0][1] * in2[1][0] + in1[0][2] * in2[2][0];
	out[0][1] = in1[0][0] * in2[0][1] + in1[0][1] * in2[1][1] + in1[0][2] * in2[2][1];
	out[0][2] = in1[0][0] * in2[0][2] + in1[0][1] * in2[1][2] + in1[0][2] * in2[2][2];
	out[0][3] = in1[0][0] * in2[0][3] + in1[0][1] * in2[1][3] + in1[0][2] * in2[2][3] + in1[0][3];
	out[1][0] = in1[1][0] * in2[0][0] + in1[1][1] * in2[1][0] + in1[1][2] * in2[2][0];
	out[1][1] = in1[1][0] * in2[0][1] + in1[1][1] * in2[1][1] + in1[1][2] * in2[2][1];
	out[1][2] = in1[1][0] * in2[0][2] + in1[1][1] * in2[1][2] + in1[1][2] * in2[2][2];
	out[1][3] = in1[1][0] * in2[0][3] + in1[1][1] * in2[1][3] + in1[1][2] * in2[2][3] + in1[1][3];
	out[2][0] = in1[2][0] * in2[0][0] + in1[2][1] * in2[1][0] + in1[2][2] * in2[2][0];
	out[2][1] = in1[2][0] * in2[0][1] + in1[2][1] * in2[1][1] + in1[2][2] * in2[2][1];
	out[2][2] = in1[2][0] * in2[0][2] + in1[2][1] * in2[1][2] + in1[2][2] * in2[2][2];
	out[2][3] = in1[2][0] * in2[0][3] + in1[2][1] * in2[1][3] + in1[2][2] * in2[2][3] + in1[2][3];
}

/*
==============
MatrixMultiply43
==============
*/
void MatrixMultiply43( const float in1[4][3], const float in2[4][3], float out[4][3] )
{
	out[0][0] = in1[0][0] * in2[0][0] + in1[0][1] * in2[1][0] + in1[0][2] * in2[2][0];
	out[1][0] = in1[1][0] * in2[0][0] + in1[1][1] * in2[1][0] + in1[1][2] * in2[2][0];
	out[2][0] = in1[2][0] * in2[0][0] + in1[2][1] * in2[1][0] + in1[2][2] * in2[2][0];
	out[0][1] = in1[0][0] * in2[0][1] + in1[0][1] * in2[1][1] + in1[0][2] * in2[2][1];
	out[1][1] = in1[1][0] * in2[0][1] + in1[1][1] * in2[1][1] + in1[1][2] * in2[2][1];
	out[2][1] = in1[2][0] * in2[0][1] + in1[2][1] * in2[1][1] + in1[2][2] * in2[2][1];
	out[0][2] = in1[0][0] * in2[0][2] + in1[0][1] * in2[1][2] + in1[0][2] * in2[2][2];
	out[1][2] = in1[1][0] * in2[0][2] + in1[1][1] * in2[1][2] + in1[1][2] * in2[2][2];
	out[2][2] = in1[2][0] * in2[0][2] + in1[2][1] * in2[1][2] + in1[2][2] * in2[2][2];
	out[3][0] = in1[3][0] * in2[0][0] + in1[3][1] * in2[1][0] + in1[3][2] * in2[2][0] + in2[3][0];
	out[3][1] = in1[3][0] * in2[0][1] + in1[3][1] * in2[1][1] + in1[3][2] * in2[2][1] + in2[3][1];
	out[3][2] = in1[3][0] * in2[0][2] + in1[3][1] * in2[1][2] + in1[3][2] * in2[2][2] + in2[3][2];
}

/*
==============
MatrixMultiply44
==============
*/
void MatrixMultiply44( const float in1[4][4], const float in2[4][4], float out[4][4] )
{
	out[0][0] = in1[0][0] * in2[0][0] + in1[0][1] * in2[1][0] + in1[0][2] * in2[2][0] + in1[0][3] * in2[3][0];
	out[0][1] = in1[0][0] * in2[0][1] + in1[0][1] * in2[1][1] + in1[0][2] * in2[2][1] + in1[0][3] * in2[3][1];
	out[0][2] = in1[0][0] * in2[0][2] + in1[0][1] * in2[1][2] + in1[0][2] * in2[2][2] + in1[0][3] * in2[3][2];
	out[0][3] = in1[0][0] * in2[0][3] + in1[0][1] * in2[1][3] + in1[0][2] * in2[2][3] + in1[0][3] * in2[3][3];
	out[1][0] = in1[1][0] * in2[0][0] + in1[1][1] * in2[1][0] + in1[1][2] * in2[2][0] + in1[1][3] * in2[3][0];
	out[1][1] = in1[1][0] * in2[0][1] + in1[1][1] * in2[1][1] + in1[1][2] * in2[2][1] + in1[1][3] * in2[3][1];
	out[1][2] = in1[1][0] * in2[0][2] + in1[1][1] * in2[1][2] + in1[1][2] * in2[2][2] + in1[1][3] * in2[3][2];
	out[1][3] = in1[1][0] * in2[0][3] + in1[1][1] * in2[1][3] + in1[1][2] * in2[2][3] + in1[1][3] * in2[3][3];
	out[2][0] = in1[2][0] * in2[0][0] + in1[2][1] * in2[1][0] + in1[2][2] * in2[2][0] + in1[2][3] * in2[3][0];
	out[2][1] = in1[2][0] * in2[0][1] + in1[2][1] * in2[1][1] + in1[2][2] * in2[2][1] + in1[2][3] * in2[3][1];
	out[2][2] = in1[2][0] * in2[0][2] + in1[2][1] * in2[1][2] + in1[2][2] * in2[2][2] + in1[2][3] * in2[3][2];
	out[2][3] = in1[2][0] * in2[0][3] + in1[2][1] * in2[1][3] + in1[2][2] * in2[2][3] + in1[2][3] * in2[3][3];
	out[3][0] = in1[3][0] * in2[0][0] + in1[3][1] * in2[1][0] + in1[3][2] * in2[2][0] + in1[3][3] * in2[3][0];
	out[3][1] = in1[3][0] * in2[0][1] + in1[3][1] * in2[1][1] + in1[3][2] * in2[2][1] + in1[3][3] * in2[3][1];
	out[3][2] = in1[3][0] * in2[0][2] + in1[3][1] * in2[1][2] + in1[3][2] * in2[2][2] + in1[3][3] * in2[3][2];
	out[3][3] = in1[3][0] * in2[0][3] + in1[3][1] * in2[1][3] + in1[3][2] * in2[2][3] + in1[3][3] * in2[3][3];
}

/*
==============
MatrixTranspose
==============
*/
void MatrixTranspose( const float in[3][3], float out[3][3] )
{
	out[0][0] = in[0][0];
	out[0][1] = in[1][0];
	out[0][2] = in[2][0];
	out[1][0] = in[0][1];
	out[1][1] = in[1][1];
	out[1][2] = in[2][1];
	out[2][0] = in[0][2];
	out[2][1] = in[1][2];
	out[2][2] = in[2][2];
}

/*
==============
MatrixTranspose44
==============
*/
void MatrixTranspose44( const float in[4][4], float out[4][4] )
{
	out[0][0] = in[0][0];
	out[0][1] = in[1][0];
	out[0][2] = in[2][0];
	out[0][3] = in[3][0];
	out[1][0] = in[0][1];
	out[1][1] = in[1][1];
	out[1][2] = in[2][1];
	out[1][3] = in[3][1];
	out[2][0] = in[0][2];
	out[2][1] = in[1][2];
	out[2][2] = in[2][2];
	out[2][3] = in[3][2];
	out[3][0] = in[0][3];
	out[3][1] = in[1][3];
	out[3][2] = in[2][3];
	out[3][3] = in[3][3];
}

/*
==============
MatrixInverse
==============
*/
void MatrixInverse( const float in1[3][3], float out[3][3] )
{
	float determinant;

	determinant = in1[0][0] * (in1[2][2] * in1[1][1] - in1[2][1] * in1[1][2])
	              - in1[1][0] * (in1[2][2] * in1[0][1] - in1[2][1] * in1[0][2])
	              + in1[2][0] * (in1[1][2] * in1[0][1] - in1[1][1] * in1[0][2]);
	determinant = 1.0f / determinant;
	out[0][0] = (in1[2][2] * in1[1][1] - in1[2][1] * in1[1][2]) * determinant;
	out[0][1] = -(in1[2][2] * in1[0][1] - in1[2][1] * in1[0][2]) * determinant;
	out[0][2] = (in1[1][2] * in1[0][1] - in1[1][1] * in1[0][2]) * determinant;
	out[1][0] = -(in1[2][2] * in1[1][0] - in1[2][0] * in1[1][2]) * determinant;
	out[1][1] = (in1[2][2] * in1[0][0] - in1[2][0] * in1[0][2]) * determinant;
	out[1][2] = -(in1[1][2] * in1[0][0] - in1[1][0] * in1[0][2]) * determinant;
	out[2][0] = (in1[2][1] * in1[1][0] - in1[2][0] * in1[1][1]) * determinant;
	out[2][1] = -(in1[2][1] * in1[0][0] - in1[2][0] * in1[0][1]) * determinant;
	out[2][2] = (in1[1][1] * in1[0][0] - in1[1][0] * in1[0][1]) * determinant;
}

/*
==============
MatrixInverseOrthogonal43
==============
*/
void MatrixInverseOrthogonal43( const float in[4][3], float out[4][3] )
{
	vec3_t origin;

	MatrixTranspose(in, out);
	VectorSubtract(vec3_origin, in[3], origin);
	MatrixTransformVector(origin, out, out[3]);
}

/*
==============
MatrixInverse44
==============
*/
void MatrixInverse44( const float *mat, float *dst )
{
	float tmp[12];
	float src[16];
	float det;
	int i;

	// transpose matrix
	for ( i = 0; i < 4; i++ )
	{
		src[i] = mat[i * 4];
		src[i + 4] = mat[i * 4 + 1];
		src[i + 8] = mat[i * 4 + 2];
		src[i + 12] = mat[i * 4 + 3];
	}

	// pairs for first 8 elements (cofactors)
	tmp[0] = src[10] * src[15];
	tmp[1] = src[11] * src[14];
	tmp[2] = src[9] * src[15];
	tmp[3] = src[11] * src[13];
	tmp[4] = src[9] * src[14];
	tmp[5] = src[10] * src[13];
	tmp[6] = src[8] * src[15];
	tmp[7] = src[11] * src[12];
	tmp[8] = src[8] * src[14];
	tmp[9] = src[10] * src[12];
	tmp[10] = src[8] * src[13];
	tmp[11] = src[9] * src[12];

	// first 8 elements (cofactors)
	dst[0] = tmp[0] * src[5] + tmp[3] * src[6] + tmp[4] * src[7];
	dst[0] -= tmp[1] * src[5] + tmp[2] * src[6] + tmp[5] * src[7];
	dst[1] = tmp[1] * src[4] + tmp[6] * src[6] + tmp[9] * src[7];
	dst[1] -= tmp[0] * src[4] + tmp[7] * src[6] + tmp[8] * src[7];
	dst[2] = tmp[2] * src[4] + tmp[7] * src[5] + tmp[10] * src[7];
	dst[2] -= tmp[3] * src[4] + tmp[6] * src[5] + tmp[11] * src[7];
	dst[3] = tmp[5] * src[4] + tmp[8] * src[5] + tmp[11] * src[6];
	dst[3] -= tmp[4] * src[4] + tmp[9] * src[5] + tmp[10] * src[6];
	dst[4] = tmp[1] * src[1] + tmp[2] * src[2] + tmp[5] * src[3];
	dst[4] -= tmp[0] * src[1] + tmp[3] * src[2] + tmp[4] * src[3];
	dst[5] = tmp[0] * src[0] + tmp[7] * src[2] + tmp[8] * src[3];
	dst[5] -= tmp[1] * src[0] + tmp[6] * src[2] + tmp[9] * src[3];
	dst[6] = tmp[3] * src[0] + tmp[6] * src[1] + tmp[11] * src[3];
	dst[6] -= tmp[2] * src[0] + tmp[7] * src[1] + tmp[10] * src[3];
	dst[7] = tmp[4] * src[0] + tmp[9] * src[1] + tmp[10] * src[2];
	dst[7] -= tmp[5] * src[0] + tmp[8] * src[1] + tmp[11] * src[2];

	// pairs for second 8 elements (cofactors)
	tmp[0] = src[2] * src[7];
	tmp[1] = src[3] * src[6];
	tmp[2] = src[1] * src[7];
	tmp[3] = src[3] * src[5];
	tmp[4] = src[1] * src[6];
	tmp[5] = src[2] * src[5];
	tmp[6] = src[0] * src[7];
	tmp[7] = src[3] * src[4];
	tmp[8] = src[0] * src[6];
	tmp[9] = src[2] * src[4];
	tmp[10] = src[0] * src[5];
	tmp[11] = src[1] * src[4];

	// second 8 elements (cofactors)
	dst[8] = tmp[0] * src[13] + tmp[3] * src[14] + tmp[4] * src[15];
	dst[8] -= tmp[1] * src[13] + tmp[2] * src[14] + tmp[5] * src[15];
	dst[9] = tmp[1] * src[12] + tmp[6] * src[14] + tmp[9] * src[15];
	dst[9] -= tmp[0] * src[12] + tmp[7] * src[14] + tmp[8] * src[15];
	dst[10] = tmp[2] * src[12] + tmp[7] * src[13] + tmp[10] * src[15];
	dst[10] -= tmp[3] * src[12] + tmp[6] * src[13] + tmp[11] * src[15];
	dst[11] = tmp[5] * src[12] + tmp[8] * src[13] + tmp[11] * src[14];
	dst[11] -= tmp[4] * src[12] + tmp[9] * src[13] + tmp[10] * src[14];
	dst[12] = tmp[2] * src[10] + tmp[5] * src[11] + tmp[1] * src[9];
	dst[12] -= tmp[4] * src[11] + tmp[0] * src[9] + tmp[3] * src[10];
	dst[13] = tmp[8] * src[11] + tmp[0] * src[8] + tmp[7] * src[10];
	dst[13] -= tmp[6] * src[10] + tmp[9] * src[11] + tmp[1] * src[8];
	dst[14] = tmp[6] * src[9] + tmp[11] * src[11] + tmp[3] * src[8];
	dst[14] -= tmp[10] * src[11] + tmp[2] * src[8] + tmp[7] * src[9];
	dst[15] = tmp[10] * src[10] + tmp[4] * src[8] + tmp[9] * src[9];
	dst[15] -= tmp[8] * src[9] + tmp[11] * src[10] + tmp[5] * src[8];

	// determinant
	det = src[0] * dst[0] + src[1] * dst[1] + src[2] * dst[2] + src[3] * dst[3];

	// matrix inverse
	det = 1 / det;

	for ( i = 0; i < 16; i++ )
	{
		dst[i] *= det;
	}
}

/*
==============
MatrixTransformVector
==============
*/
void MatrixTransformVector( const vec3_t in1, const vec3_t in2[3], vec3_t out )
{
	out[0] = in1[0] * in2[0][0] + in1[1] * in2[1][0] + in1[2] * in2[2][0];
	out[1] = in1[0] * in2[0][1] + in1[1] * in2[1][1] + in1[2] * in2[2][1];
	out[2] = in1[0] * in2[0][2] + in1[1] * in2[1][2] + in1[2] * in2[2][2];
}

/*
==============
MatrixTransformVector44
==============
*/
void MatrixTransformVector44( const vec4_t in, const float mat[4][4], vec4_t out )
{
	out[0] = in[0] * mat[0][0] + in[1] * mat[1][0] + in[2] * mat[2][0] + in[3] * mat[3][0];
	out[1] = in[0] * mat[0][1] + in[1] * mat[1][1] + in[2] * mat[2][1] + in[3] * mat[3][1];
	out[2] = in[0] * mat[0][2] + in[1] * mat[1][2] + in[2] * mat[2][2] + in[3] * mat[3][2];
	out[3] = in[0] * mat[0][3] + in[1] * mat[1][3] + in[2] * mat[2][3] + in[3] * mat[3][3];
}

/*
==============
MatrixTransposeTransformVector
==============
*/
void MatrixTransposeTransformVector( const vec3_t in1, const vec3_t in2[3], vec3_t out )
{
	out[0] = in1[0] * in2[0][0] + in1[1] * in2[0][1] + in1[2] * in2[0][2];
	out[1] = in1[0] * in2[1][0] + in1[1] * in2[1][1] + in1[2] * in2[1][2];
	out[2] = in1[0] * in2[2][0] + in1[1] * in2[2][1] + in1[2] * in2[2][2];
}

/*
==============
MatrixTransformVector43
==============
*/
void MatrixTransformVector43( const vec3_t in1, const float in2[4][3], vec3_t out )
{
	out[0] = in1[0] * in2[0][0] + in1[1] * in2[1][0] + in1[2] * in2[2][0] + in2[3][0];
	out[1] = in1[0] * in2[0][1] + in1[1] * in2[1][1] + in1[2] * in2[2][1] + in2[3][1];
	out[2] = in1[0] * in2[0][2] + in1[1] * in2[1][2] + in1[2] * in2[2][2] + in2[3][2];
}

/*
==============
MatrixTransposeTransformVector43
==============
*/
void MatrixTransposeTransformVector43( const vec3_t in1, const float in2[4][3], vec3_t out )
{
	vec3_t temp;

	VectorSubtract(in1, in2[3], temp);

	out[0] = in2[0][0] * temp[0] + in2[0][1] * temp[1] + in2[0][2] * temp[2];
	out[1] = in2[1][0] * temp[0] + in2[1][1] * temp[1] + in2[1][2] * temp[2];
	out[2] = in2[2][0] * temp[0] + in2[2][1] * temp[1] + in2[2][2] * temp[2];
}

/*
==============
MatrixTransformVector43Equals
==============
*/
void MatrixTransformVector43Equals( vec3_t inout, const float in2[4][3] )
{
	float temp[2];

	temp[0] = inout[0] * in2[0][0] + inout[1] * in2[1][0] + inout[2] * in2[2][0] + in2[3][0];
	temp[1] = inout[0] * in2[0][1] + inout[1] * in2[1][1] + inout[2] * in2[2][1] + in2[3][1];
	inout[2] = inout[0] * in2[0][2] + inout[1] * in2[1][2] + inout[2] * in2[2][2] + in2[3][2];
	inout[0] = temp[0];
	inout[1] = temp[1];
}

/*
==============
RotateVec2
==============
*/
void RotateVec2( vec2_t v, float degrees )
{
	float c;
	float s;
	float x;

	FastSinCos(degrees * 0.017453292519943295f, &s, &c);
	x = v[0] * c - v[1] * s;
	v[1] = v[1] * c + v[0] * s;
	v[0] = x;
}

/*
==============
QuatMultiply
==============
*/
void QuatMultiply( const float *in1, const float *in2, float *out )
{
	out[0] = in1[0] * in2[3] + in1[3] * in2[0] + in1[2] * in2[1] - in1[1] * in2[2];
	out[1] = in1[1] * in2[3] - in1[2] * in2[0] + in1[3] * in2[1] + in1[0] * in2[2];
	out[2] = in1[2] * in2[3] + in1[1] * in2[0] - in1[0] * in2[1] + in1[3] * in2[2];
	out[3] = in1[3] * in2[3] - in1[0] * in2[0] - in1[1] * in2[1] - in1[2] * in2[2];
}

/*
==============
QuatInverse
==============
*/
void QuatInverse( const vec4_t in, vec4_t out )
{
	out[0] = -in[0];
	out[1] = -in[1];
	out[2] = -in[2];
	out[3] = in[3];
}

/*
==============
QuatToAxis
==============
*/
void QuatToAxis( const vec4_t quat, vec3_t axis[3] )
{
	float xx;
	float xy;
	float xz;
	float xw;
	float yy;
	float yz;
	float yw;
	float zz;
	float zw;
	float ww;
	float magSq;
	float scale;
	float scaledX;
	float scaledY;

	xx = quat[0] * quat[0];
	yy = quat[1] * quat[1];
	zz = quat[2] * quat[2];
	ww = quat[3] * quat[3];
	magSq = xx + yy + zz + ww;
	scale = 2.0f / magSq;
	xx *= scale;
	yy *= scale;
	zz *= scale;
	scaledX = scale * quat[0];
	xy = scaledX * quat[1];
	xz = scaledX * quat[2];
	xw = scaledX * quat[3];
	scaledY = scale * quat[1];
	yz = scaledY * quat[2];
	yw = scaledY * quat[3];
	zw = scale * quat[2] * quat[3];

	axis[0][0] = 1.0f - (yy + zz);
	axis[0][1] = xy + zw;
	axis[0][2] = xz - yw;
	axis[1][0] = xy - zw;
	axis[1][1] = 1.0f - (xx + zz);
	axis[1][2] = yz + xw;
	axis[2][0] = xz + yw;
	axis[2][1] = yz - xw;
	axis[2][2] = 1.0f - (xx + yy);
}

/*
==============
QuatAxisLengthSq

Fraction of a quaternion's squared length held by its axis part.
==============
*/
float QuatAxisLengthSq( const vec4_t q )
{
	float x;
	float y;
	float z;
	float mag;

	x = q[0] * q[0];
	y = q[1] * q[1];
	z = q[2] * q[2];
	mag = x + y + z + q[3] * q[3];

	if ( mag != 0 )
	{
		mag = 1.0f / mag;
		x *= mag;
		y *= mag;
		z *= mag;
		return x + y + z;
	}

	return 0;
}

/*
==============
SinSquaredDeg
==============
*/
float SinSquaredDeg( float angle )
{
	double s;
	float r;

	angle = (float)sin((double)(angle * 0.017453292f));
	s = angle;
	r = s * s;
	return r;
}

/*
==============
QuatDiffAxisLengthSq
==============
*/
float QuatDiffAxisLengthSq( const vec4_t a, const vec4_t b )
{
	vec4_t inv;
	vec4_t diff;

	QuatInverse(b, inv);
	QuatMultiply(a, inv, diff);
	return QuatAxisLengthSq(diff);
}

/*
==============
RotationToYaw
==============
*/
float RotationToYaw( const vec2_t rot )
{
	float zz;
	float r;

	zz = rot[0] * rot[0];
	r = zz + rot[1] * rot[1];
	r = 2.0f / r;

	return atan2(rot[0] * rot[1] * r, 1.0f - zz * r) * DEGINRAD;
}

/*
==============
QuatFromPitch
==============
*/
void QuatFromPitch( float angle, vec4_t quat )
{
	angle *= 0.008726646259971648f;
	quat[0] = 0;
	quat[2] = 0;
	FastSinCos(angle, &quat[1], &quat[3]);
}

/*
==============
QuatFromYaw
==============
*/
void QuatFromYaw( float angle, vec4_t quat )
{
	angle *= 0.008726646259971648f;
	quat[0] = 0;
	quat[1] = 0;
	FastSinCos(angle, &quat[2], &quat[3]);
}

/*
==============
QuatFromRoll
==============
*/
void QuatFromRoll( float angle, vec4_t quat )
{
	angle *= 0.008726646259971648f;
	quat[1] = 0;
	quat[2] = 0;
	FastSinCos(angle, &quat[0], &quat[3]);
}

/*
==============
QuatFromAxisAngle
==============
*/
void QuatFromAxisAngle( float angle, const vec3_t axis, vec4_t quat )
{
	float s;

	angle *= 0.008726646259971648f;
	FastSinCos(angle, &s, &quat[3]);
	VectorScale(axis, s, quat);
}

/*
==============
MatrixRotationX
==============
*/
void MatrixRotationX( vec3_t m[3], float angle )
{
	float c;
	float s;

	FastSinCos((angle * 0.017453292519943295f), &s, &c);
	m[0][0] = 1.0f;
	m[0][1] = 0;
	m[0][2] = 0;
	m[1][0] = 0;
	m[1][1] = c;
	m[1][2] = -s;
	m[2][0] = 0;
	m[2][1] = s;
	m[2][2] = c;
}

/*
==============
MatrixRotationY
==============
*/
void MatrixRotationY( vec3_t m[3], float angle )
{
	float c;
	float s;

	FastSinCos(angle * 0.017453292519943295f, &s, &c);
	m[0][0] = c;
	m[0][1] = 0;
	m[0][2] = s;
	m[1][0] = 0;
	m[1][1] = 1.0f;
	m[1][2] = 0;
	m[2][0] = -s;
	m[2][1] = 0;
	m[2][2] = c;
}

/*
==============
MatrixRotationZ
==============
*/
void MatrixRotationZ( vec3_t m[3], float angle )
{
	float c;
	float s;

	FastSinCos(angle * 0.017453292519943295f, &s, &c);
	m[0][0] = c;
	m[0][1] = -s;
	m[0][2] = 0;
	m[1][0] = s;
	m[1][1] = c;
	m[1][2] = 0;
	m[2][0] = 0;
	m[2][1] = 0;
	m[2][2] = 1.0f;
}

/*
==============
InfinitePerspectiveMatrix
==============
*/
void InfinitePerspectiveMatrix( float mtx[4][4], float fovX, float fovY, float zNear )
{
	float k;

	memset(mtx, 0, 64);
	k = 0.99950027f;
	mtx[0][0] = (float)tan((double)((90.0f - fovX * 0.5f) * 0.017453292f)) * k;
	mtx[1][1] = (float)tan((double)((90.0f - fovY * 0.5f) * 0.017453292f)) * k;
	mtx[2][2] = k;
	mtx[2][3] = 1.0f;
	mtx[3][2] = -zNear * k;
}

/*
==============
OrthographicMatrix
==============
*/
void OrthographicMatrix( float mtx[4][4], float width, float height, float depth )
{
	memset(mtx, 0, 64);
	mtx[0][0] = 2.0f / width;
	mtx[1][1] = 2.0f / height;
	mtx[2][2] = 0.5f / depth;
	mtx[3][2] = 0.5f;
	mtx[3][3] = 1.0f;
}

/*
==============
MatrixForViewer
==============
*/
void MatrixForViewer( float mtx[4][4], const vec3_t origin, const vec3_t axis[3] )
{
	mtx[0][0] = -axis[1][0];
	mtx[1][0] = -axis[1][1];
	mtx[2][0] = -axis[1][2];
	mtx[3][0] = -(origin[0] * mtx[0][0] + origin[1] * mtx[1][0] + origin[2] * mtx[2][0]);
	mtx[0][1] = axis[2][0];
	mtx[1][1] = axis[2][1];
	mtx[2][1] = axis[2][2];
	mtx[3][1] = -(origin[0] * mtx[0][1] + origin[1] * mtx[1][1] + origin[2] * mtx[2][1]);
	mtx[0][2] = axis[0][0];
	mtx[1][2] = axis[0][1];
	mtx[2][2] = axis[0][2];
	mtx[3][2] = -(origin[0] * mtx[0][2] + origin[1] * mtx[1][2] + origin[2] * mtx[2][2]);
	mtx[0][3] = 0;
	mtx[1][3] = 0;
	mtx[2][3] = 0;
	mtx[3][3] = 1.0f;
}

/*
==============
ColorBytesFromRGB
==============
*/
unsigned int ColorBytesFromRGB( float r, float g, float b )
{
	unsigned char c[4];

	c[0] = (unsigned char)(r * 255.0f);
	c[1] = (unsigned char)(g * 255.0f);
	c[2] = (unsigned char)(b * 255.0f);
	c[3] = 0xff;

	return *(unsigned int *)c;
}

/*
==============
ColorBytesFromRGBA
==============
*/
unsigned int ColorBytesFromRGBA( float r, float g, float b, float a )
{
	unsigned char c[4];

	c[0] = (unsigned char)(r * 255.0f);
	c[1] = (unsigned char)(g * 255.0f);
	c[2] = (unsigned char)(b * 255.0f);
	c[3] = (unsigned char)(a * 255.0f);

	return *(unsigned int *)c;
}

/*
==============
Vec3NormalizeByMax
==============
*/
float Vec3NormalizeByMax( const vec3_t v, vec3_t out )
{
	float max;

	max = v[0];

	if ( v[1] > max )
	{
		max = v[1];
	}

	if ( v[2] > max )
	{
		max = v[2];
	}

	if ( max == 0 )
	{
		VectorClear(out);
	}
	else
	{
		out[0] = v[0] / max;
		out[1] = v[1] / max;
		out[2] = v[2] / max;
	}

	return max;
}

/*
==============
AngleMod
==============
*/
float AngleMod( float a )
{
	return ( ( 360.0f / 65536 ) * ( (int)( a * ( 65536 / 360.0f ) ) & 65535 ) );
}

/*
==============
LerpAngle

Lerps from -> to along the shorter arc.
==============
*/
float LerpAngle( float from, float to, float frac )
{
	float result;
	float target;

	target = to;

	if ( target - from > 180 )
		target -= 360;

	if ( target - from < -180 )
		target += 360;

	result = from + frac * (target - from);
	return result;
}

/*
==============
AngleSubtract
==============
*/
float AngleSubtract( float a1, float a2 )
{
	float a = a1 - a2;

	while ( a > 180 )
	{
		a -= 360;
	}

	while ( a < -180 )
	{
		a += 360;
	}

	return a;
}

/*
==============
AnglesSubtract
==============
*/
void AnglesSubtract( const vec3_t v1, const vec3_t v2, vec3_t v3 )
{
	v3[0] = AngleSubtract(v1[0], v2[0]);
	v3[1] = AngleSubtract(v1[1], v2[1]);
	v3[2] = AngleSubtract(v1[2], v2[2]);
}

/*
==============
AngleNormalize360
==============
*/
float AngleNormalize360( float angle )
{
	return ( 360.0f / 65536 ) * ( (int)( angle * ( 65536 / 360.0f ) ) & 65535 );
}

/*
==============
AngleNormalize180
==============
*/
float AngleNormalize180( float angle )
{
	float normalized;

	normalized = AngleNormalize360(angle);

	if ( normalized > 180.0f )
		normalized = normalized - 360.0f;

	return normalized;
}

/*
==============
AngleNormalize360Accurate
==============
*/
float AngleNormalize360Accurate( float angle )
{
	if ( angle < 0.0f )
	{
		do
			angle = angle + 360.0f;
		while ( angle < 0.0f );
		return angle;
	}

	if ( angle >= 360.0f )
	{
		do
			angle = angle - 360.0f;
		while ( angle >= 360.0f );
		return angle;
	}

	return angle;
}

/*
==============
AngleNormalize180Accurate
==============
*/
float AngleNormalize180Accurate( float angle )
{
	if ( angle <= -180.0f )
	{
		do
			angle = angle + 360.0f;
		while ( angle <= -180.0f );
		return angle;
	}

	if ( angle > 180.0f )
	{
		do
			angle = angle - 360.0f;
		while ( angle > 180.0f );
		return angle;
	}

	return angle;
}

/*
==============
AngleDelta
==============
*/
float AngleDelta( float angle1, float angle2 )
{
	return AngleNormalize180( angle1 - angle2 );
}

/*
==============
RadiusFromBounds
==============
*/
float RadiusFromBounds( const vec3_t mins, const vec3_t maxs )
{
	int i;
	vec3_t corner;
	float a, b;

	for ( i = 0 ; i < 3 ; i++ )
	{
		a = I_fabs( mins[i] );
		b = I_fabs( maxs[i] );
		corner[i] = a > b ? a : b;
	}

	return VectorLength( corner );
}

/*
==============
RadiusFromBounds2D
==============
*/
float RadiusFromBounds2D( const vec2_t mins, const vec2_t maxs )
{
	int i;
	vec2_t corner;
	float a, b;

	for ( i = 0 ; i < 2 ; i++ )
	{
		a = I_fabs( mins[i] );
		b = I_fabs( maxs[i] );
		corner[i] = a > b ? a : b;
	}

	return Vec2Length( corner );
}

/*
==============
ExpandBoundsByDelta

Grows the bounds along a signed delta.
==============
*/
void ExpandBoundsByDelta( vec3_t mins, vec3_t maxs, const vec3_t delta )
{
	if ( delta[0] > 0 )
		maxs[0] += delta[0];
	else
		mins[0] += delta[0];

	if ( delta[1] > 0 )
		maxs[1] += delta[1];
	else
		mins[1] += delta[1];

	if ( delta[2] > 0 )
		maxs[2] += delta[2];
	else
		mins[2] += delta[2];
}

/*
==============
ExpandBoundsToWidth
==============
*/
void ExpandBoundsToWidth( vec3_t mins, vec3_t maxs )
{
	vec3_t size;
	float s;
	float diff;

	VectorSubtract(maxs, mins, size);
	s = I_fmax(size[0], size[1]);

	if ( s > size[2] )
	{
		diff = (s - size[2]) * 0.5f;
		mins[2] = mins[2] - diff;
		maxs[2] = maxs[2] + diff;
	}
}

/*
==============
ShrinkBoundsToHeight
==============
*/
void ShrinkBoundsToHeight( vec3_t mins, vec3_t maxs )
{
	vec3_t d;
	float half;

	VectorSubtract(maxs, mins, d);

	if ( d[0] > d[2] )
	{
		half = (d[0] - d[2]) * 0.5f;
		mins[0] += half;
		maxs[0] -= half;
	}

	if ( d[1] > d[2] )
	{
		half = (d[1] - d[2]) * 0.5f;
		mins[1] += half;
		maxs[1] -= half;
	}
}

/*
==============
ClearBounds
==============
*/
void ClearBounds( vec3_t mins, vec3_t maxs )
{
	VectorSet(mins, 131072, 131072, 131072);
	VectorSet(maxs, -131072, -131072, -131072);
}

/*
==============
ClearBounds2D
==============
*/
void ClearBounds2D( vec2_t mins, vec2_t maxs )
{
	Vector2Set(mins, 131072.0f, 131072.0f);
	Vector2Set(maxs, -131072.0f, -131072.0f);
}

/*
==============
AddPointToBounds
==============
*/
void AddPointToBounds( const vec3_t v, vec3_t mins, vec3_t maxs )
{
	if ( v[0] < mins[0] )
		mins[0] = v[0];
	if ( v[0] > maxs[0] )
		maxs[0] = v[0];
	if ( v[1] < mins[1] )
		mins[1] = v[1];
	if ( v[1] > maxs[1] )
		maxs[1] = v[1];
	if ( v[2] < mins[2] )
		mins[2] = v[2];
	if ( v[2] > maxs[2] )
		maxs[2] = v[2];
}

/*
==============
AddPointToBounds2D
==============
*/
void AddPointToBounds2D( const vec2_t v, vec2_t mins, vec2_t maxs )
{
	if ( v[0] < mins[0] )
	{
		mins[0] = v[0];
	}

	if ( v[0] > maxs[0] )
	{
		maxs[0] = v[0];
	}

	if ( v[1] < mins[1] )
	{
		mins[1] = v[1];
	}

	if ( v[1] > maxs[1] )
	{
		maxs[1] = v[1];
	}
}

/*
==============
PointInBounds
==============
*/
int PointInBounds( const vec3_t v, const vec3_t mins, const vec3_t maxs )
{
	if ( v[0] < mins[0] || v[0] > maxs[0] )
	{
		return 0;
	}

	if ( v[1] < mins[1] || v[1] > maxs[1] )
	{
		return 0;
	}

	if ( v[2] < mins[2] || v[2] > maxs[2] )
	{
		return 0;
	}

	return 1;
}

/*
==============
PointInBounds2D
==============
*/
int PointInBounds2D( const vec2_t v, const vec2_t mins, const vec2_t maxs )
{
	if ( v[0] < mins[0] || v[0] > maxs[0] )
	{
		return 0;
	}

	if ( v[1] < mins[1] || v[1] > maxs[1] )
	{
		return 0;
	}

	return 1;
}

/*
==============
BoundsOverlap
==============
*/
int BoundsOverlap( const vec3_t mins1, const vec3_t maxs1, const vec3_t mins2, const vec3_t maxs2 )
{
	if ( mins1[0] > maxs2[0] || mins2[0] > maxs1[0] )
	{
		return 0;
	}

	if ( mins1[1] > maxs2[1] || mins2[1] > maxs1[1] )
	{
		return 0;
	}

	if ( mins1[2] > maxs2[2] || mins2[2] > maxs1[2] )
	{
		return 0;
	}

	return 1;
}

/*
==============
BoundsOverlap2D
==============
*/
int BoundsOverlap2D( const vec2_t mins1, const vec2_t maxs1, const vec2_t mins2, const vec2_t maxs2 )
{
	if ( mins1[0] > maxs2[0] || mins2[0] > maxs1[0] )
	{
		return 0;
	}

	if ( mins1[1] > maxs2[1] || mins2[1] > maxs1[1] )
	{
		return 0;
	}

	return 1;
}

/*
==============
BoundsOverlapEpsilon
==============
*/
int BoundsOverlapEpsilon( const vec3_t mins1, const vec3_t maxs1, const vec3_t mins2, const vec3_t maxs2, float epsilon )
{
	if ( mins1[0] > maxs2[0] + epsilon || mins2[0] > maxs1[0] + epsilon )
	{
		return 0;
	}

	if ( mins1[1] > maxs2[1] + epsilon || mins2[1] > maxs1[1] + epsilon )
	{
		return 0;
	}

	if ( mins1[2] > maxs2[2] + epsilon || mins2[2] > maxs1[2] + epsilon )
	{
		return 0;
	}

	return 1;
}

/*
==============
BoundsOverlapEpsilon2D
==============
*/
int BoundsOverlapEpsilon2D( const vec2_t mins1, const vec2_t maxs1, const vec2_t mins2, const vec2_t maxs2, float epsilon )
{
	if ( mins1[0] > maxs2[0] + epsilon || mins2[0] > maxs1[0] + epsilon )
	{
		return 0;
	}

	if ( mins1[1] > maxs2[1] + epsilon || mins2[1] > maxs1[1] + epsilon )
	{
		return 0;
	}

	return 1;
}

/*
==============
ExpandBounds
==============
*/
void ExpandBounds( const vec3_t addedmins, const vec3_t addedmaxs, vec3_t mins, vec3_t maxs )
{
	if ( mins[0] > addedmins[0] )
	{
		mins[0] = addedmins[0];
	}

	if ( maxs[0] < addedmaxs[0] )
	{
		maxs[0] = addedmaxs[0];
	}

	if ( mins[1] > addedmins[1] )
	{
		mins[1] = addedmins[1];
	}

	if ( maxs[1] < addedmaxs[1] )
	{
		maxs[1] = addedmaxs[1];
	}

	if ( mins[2] > addedmins[2] )
	{
		mins[2] = addedmins[2];
	}

	if ( maxs[2] < addedmaxs[2] )
	{
		maxs[2] = addedmaxs[2];
	}
}

/*
==============
ExpandBounds2D
==============
*/
void ExpandBounds2D( const vec2_t addedmins, const vec2_t addedmaxs, vec2_t mins, vec2_t maxs )
{
	if ( mins[0] > addedmins[0] )
	{
		mins[0] = addedmins[0];
	}

	if ( maxs[0] < addedmaxs[0] )
	{
		maxs[0] = addedmaxs[0];
	}

	if ( mins[1] > addedmins[1] )
	{
		mins[1] = addedmins[1];
	}

	if ( maxs[1] < addedmaxs[1] )
	{
		maxs[1] = addedmaxs[1];
	}
}

/*
==============
TransformBounds

Axis-aligned bounds of a box after rotating by axis and moving to origin;
the sign bit of each axis entry picks the min or max corner.
==============
*/
void TransformBounds( const vec3_t bounds[2], const vec3_t origin, const vec3_t axis[3], vec3_t out[2] )
{
	int i;
	int offset;

	for ( i = 0; i < 3; i++ )
	{
		out[0][i] = origin[i];
		out[1][i] = origin[i];

		offset = FloatAsInt(axis[0][i]) < 0 ? 12 : 0;
		out[0][i] += ((const float *)((const char *)bounds + offset))[0] * axis[0][i];
		out[1][i] += ((const float *)((const char *)bounds - offset))[3] * axis[0][i];

		offset = FloatAsInt(axis[1][i]) < 0 ? 12 : 0;
		out[0][i] += ((const float *)((const char *)bounds + offset))[1] * axis[1][i];
		out[1][i] += ((const float *)((const char *)bounds - offset))[4] * axis[1][i];

		offset = FloatAsInt(axis[2][i]) < 0 ? 12 : 0;
		out[0][i] += ((const float *)((const char *)bounds + offset))[2] * axis[2][i];
		out[1][i] += ((const float *)((const char *)bounds - offset))[5] * axis[2][i];
	}
}

/*
==============
TransformBoundsExpand
==============
*/
void TransformBoundsExpand( const vec3_t bounds[2], const vec3_t origin, const vec3_t axis[3], vec3_t out[2] )
{
	vec3_t transformed[2];

	TransformBounds(bounds, origin, axis, transformed);
	ExpandBounds(transformed[0], transformed[1], out[0], out[1]);
}

/*
==============
AxisIdentity
==============
*/
void AxisIdentity( vec3_t axis[3] )
{
	axis[0][0] = 1.0f;
	axis[0][1] = 0;
	axis[0][2] = 0;
	axis[1][0] = 0;
	axis[1][1] = 1.0f;
	axis[1][2] = 0;
	axis[2][0] = 0;
	axis[2][1] = 0;
	axis[2][2] = 1.0f;
}

/*
==============
AxisCopy
==============
*/
void AxisCopy( const vec3_t in[3], vec3_t out[3] )
{
	VectorCopy(in[0], out[0]);
	VectorCopy(in[1], out[1]);
	VectorCopy(in[2], out[2]);
}

/*
==============
AxisTranspose
==============
*/
void AxisTranspose( const vec3_t in[3], vec3_t out[3] )
{
	out[0][0] = in[0][0];
	out[0][1] = in[1][0];
	out[0][2] = in[2][0];
	out[1][0] = in[0][1];
	out[1][1] = in[1][1];
	out[1][2] = in[2][1];
	out[2][0] = in[0][2];
	out[2][1] = in[1][2];
	out[2][2] = in[2][2];
}

/*
==============
AxisTransformComponents

x * axis[0] + y * axis[1] + z * axis[2]
==============
*/
void AxisTransformComponents( const vec3_t axis[3], float x, float y, float z, vec3_t out )
{
	out[0] = x * axis[0][0] + y * axis[1][0] + z * axis[2][0];
	out[1] = x * axis[0][1] + y * axis[1][1] + z * axis[2][1];
	out[2] = x * axis[0][2] + y * axis[1][2] + z * axis[2][2];
}

/*
==============
AnglesToAxis
==============
*/
void AnglesToAxis( const vec3_t angles, vec3_t axis[3] )
{
	vec3_t right;

	AngleVectors( angles, axis[0], right, axis[2] );
	VectorSubtract( vec3_origin, right, axis[1] );
}

/*
==============
YawToAxis
==============
*/
void YawToAxis( float yaw, vec3_t axis[3] )
{
	vec3_t right;

	YawVectors(yaw, axis[0], right);
	axis[2][0] = 0;
	axis[2][1] = 0;
	axis[2][2] = 1.0f;
	VectorSubtract(vec3_origin, right, axis[1]);
}

/*
==============
AxisToAngles
==============
*/
void AxisToAngles( vec3_t axis[3], vec3_t angles )
{
	vec3_t right;
	float a;
	float fSin;
	float fCos;
	float temp;
	float pitch;

	// first get the pitch and yaw from the forward vector
	vectoangles( axis[0], angles );

	// now get the roll from the right vector
	VectorCopy( axis[1], right );

	// get the angle difference between the tmpAxis[2] and axis[2] after they have been reverse-rotated
	a = ((-angles[YAW]) * 0.017453292519943295f);
	FastSinCos(a, &fSin, &fCos);

	temp = fCos * right[0] - fSin * right[1];
	right[1] = fSin * right[0] + fCos * right[1];

	a = ((-angles[PITCH]) * 0.017453292519943295f);
	FastSinCos(a, &fSin, &fCos);

	right[0] = (fSin * right[2]) + (fCos * temp);
	right[2] = (fCos * right[2]) - (fSin * temp);

	// now find the angles, the PITCH is effectively our ROLL
	pitch = vectosignedpitch(right);

	if ( right[1] < 0.0f )
	{
		angles[ROLL] = pitch + ( pitch < 0.0f ? 180.0f : -180.0f );
	}
	else
	{
		angles[ROLL] = -pitch;
	}
}

/*
==============
AxisToSignedAngles
==============
*/
void AxisToSignedAngles( const vec3_t axis[3], vec3_t angles )
{
	vec3_t right;
	float a;
	float fSin;
	float fCos;
	float temp;
	float pitch;

	vectoanglessigned(axis[0], angles);
	VectorCopy(axis[1], right);

	a = (-angles[YAW]) * 0.017453292519943295f;
	FastSinCos(a, &fSin, &fCos);

	temp = fCos * right[0] - fSin * right[1];
	right[1] = fSin * right[0] + fCos * right[1];

	a = (-angles[PITCH]) * 0.017453292519943295f;
	FastSinCos(a, &fSin, &fCos);

	right[0] = fSin * right[2] + fCos * temp;
	right[2] = fCos * right[2] - fSin * temp;

	pitch = vectosignedpitch(right);

	if ( right[1] < 0.0f )
	{
		angles[ROLL] = pitch + ( pitch < 0.0f ? 180.0f : -180.0f );
	}
	else
	{
		angles[ROLL] = -pitch;
	}
}

/*
==============
IntersectPlanes

Point shared by three planes; false when they are nearly parallel.
==============
*/
int IntersectPlanes( const float **plane, vec3_t xyz )
{
	double v[3];
	double det;
	double invDet;

	v[0] = plane[0][0] * (plane[1][1] * plane[2][2] - plane[2][1] * plane[1][2]);
	v[1] = plane[1][0] * (plane[2][1] * plane[0][2] - plane[0][1] * plane[2][2]);
	v[2] = plane[2][0] * (plane[0][1] * plane[1][2] - plane[1][1] * plane[0][2]);
	det = v[0] + v[1] + v[2];

	if ( fabs(det) < 0.001f )
	{
		return 0;
	}

	invDet = 1.0f / det;

	v[0] = plane[0][3] * (plane[1][1] * plane[2][2] - plane[2][1] * plane[1][2]);
	v[1] = plane[1][3] * (plane[2][1] * plane[0][2] - plane[0][1] * plane[2][2]);
	v[2] = plane[2][3] * (plane[0][1] * plane[1][2] - plane[1][1] * plane[0][2]);
	xyz[0] = (v[0] + v[1] + v[2]) * invDet;

	v[0] = plane[0][3] * (plane[1][2] * plane[2][0] - plane[2][2] * plane[1][0]);
	v[1] = plane[1][3] * (plane[2][2] * plane[0][0] - plane[0][2] * plane[2][0]);
	v[2] = plane[2][3] * (plane[0][2] * plane[1][0] - plane[1][2] * plane[0][0]);
	xyz[1] = (v[0] + v[1] + v[2]) * invDet;

	v[0] = plane[0][3] * (plane[1][0] * plane[2][1] - plane[2][0] * plane[1][1]);
	v[1] = plane[1][3] * (plane[2][0] * plane[0][1] - plane[0][0] * plane[2][1]);
	v[2] = plane[2][3] * (plane[0][0] * plane[1][1] - plane[1][0] * plane[0][1]);
	xyz[2] = (v[0] + v[1] + v[2]) * invDet;

	return 1;
}

/*
==============
SnapPointToIntersectingPlanes

Snaps to the grid only when that does not move the point further off its planes.
==============
*/
void SnapPointToIntersectingPlanes( const float **planes, vec3_t xyz, float snapGrid, float snapEpsilon )
{
	float baseError;
	float snapError;
	float maxSnapError;
	float maxBaseError;
	float rounded;
	vec3_t snapped;
	int planeIndex;
	int axis;

	for ( axis = 0; axis < 3; axis++ )
	{
		rounded = I_fround(xyz[axis] / snapGrid) * snapGrid;

		if ( I_fabs(rounded - xyz[axis]) < snapEpsilon )
		{
			snapped[axis] = rounded;
		}
		else
		{
			snapped[axis] = xyz[axis];
		}
	}

	if ( VectorCompare(snapped, xyz) )
	{
		return;
	}

	maxSnapError = 0;
	maxBaseError = snapEpsilon;

	for ( planeIndex = 0; planeIndex < 3; planeIndex++ )
	{
		snapError = I_fabs(DotProduct(planes[planeIndex], snapped) - planes[planeIndex][3]);

		if ( maxSnapError < snapError )
		{
			maxSnapError = snapError;
		}

		baseError = I_fabs(DotProduct(planes[planeIndex], snapped) - planes[planeIndex][3]);

		if ( maxBaseError < baseError )
		{
			maxBaseError = baseError;
		}
	}

	if ( maxSnapError < maxBaseError )
	{
		VectorCopy(snapped, xyz);
	}
}

/*
==============
SnapPointToPlanes
==============
*/
void SnapPointToPlanes( const vec4_t *planes, int planeCount, vec3_t xyz, float snapGrid, float snapEpsilon )
{
	float d;
	float baseError;
	float snapError;
	float maxSnapError;
	float maxBaseError;
	float rounded;
	vec3_t snapped;
	int planeIndex;
	int axis;

	for ( planeIndex = 0; planeIndex < planeCount; planeIndex++ )
	{
		d = DotProduct(xyz, planes[planeIndex]) - planes[planeIndex][3];

		if ( d <= snapEpsilon && -snapEpsilon <= d )
		{
			VectorMA(xyz, -d, planes[planeIndex], xyz);
		}
	}

	for ( axis = 0; axis < 3; axis++ )
	{
		rounded = I_fround(xyz[axis] / snapGrid) * snapGrid;

		if ( I_fabs(rounded - xyz[axis]) < snapEpsilon )
		{
			snapped[axis] = rounded;
		}
		else
		{
			snapped[axis] = xyz[axis];
		}
	}

	if ( VectorCompare(snapped, xyz) )
	{
		return;
	}

	maxSnapError = 0;
	maxBaseError = snapEpsilon;

	for ( planeIndex = 0; planeIndex < planeCount; planeIndex++ )
	{
		snapError = I_fabs(DotProduct(planes[planeIndex], snapped) - planes[planeIndex][3]);

		if ( maxSnapError < snapError )
		{
			maxSnapError = snapError;
		}

		baseError = I_fabs(DotProduct(planes[planeIndex], snapped) - planes[planeIndex][3]);

		if ( maxBaseError < baseError )
		{
			maxBaseError = baseError;
		}
	}

	if ( maxSnapError < maxBaseError )
	{
		VectorCopy(snapped, xyz);
	}
}

/*
==============
PointInPolygon

True when point lies inside the convex polygon, tested in its dominant plane.
==============
*/
int PointInPolygon( const vec3_t *verts, int vertCount, const vec3_t normal, const vec3_t point )
{
	int i;
	int j;
	int k;
	int prev;
	vec2_t edge;
	vec2_t delta;
	float dot;

	PickProjectionAxes(normal, &i, &j);
	prev = vertCount - 1;

	for ( k = 0; k < vertCount; k++ )
	{
		edge[0] = verts[k][j] - verts[prev][j];
		edge[1] = verts[prev][i] - verts[k][i];
		delta[0] = point[i] - verts[prev][i];
		delta[1] = point[j] - verts[prev][j];
		dot = Dot2Product(delta, edge);

		if ( dot < 0 )
		{
			return 0;
		}

		prev = k;
	}

	return 1;
}

/*
==============
PlaneFromPoints
==============
*/
int PlaneFromPoints( vec4_t plane, const vec3_t a, const vec3_t b, const vec3_t c )
{
	vec3_t d1;
	vec3_t d2;

	VectorSubtract(b, a, d1);
	VectorSubtract(c, a, d2);
	Vec3Cross(d2, d1, plane);

	if ( Vec3Normalize(plane) == 0 )
	{
		return 0;
	}

	plane[3] = DotProduct(a, plane);
	return 1;
}

/*
==============
ProjectPointOnPlane
==============
*/
void ProjectPointOnPlane( const vec3_t p, const vec3_t normal, vec3_t dst )
{
	float scale;

	scale = -DotProduct(normal, p);
	VectorMA(p, scale, normal, dst);
}

/*
==============
SetPlaneSignbits
==============
*/
void SetPlaneSignbits( cplane_t *out )
{
	unsigned char bits;
	int j;

	bits = 0;

	for ( j = 0; j < 3; j++ )
	{
		if ( out->normal[j] < 0 )
		{
			bits |= 1 << j;
		}
	}

	out->signbits = bits;
}

/*
==============
BoxOnPlaneSide
==============
*/
// retail = Q3 naked asm (jae, not jge)
#pragma warning( disable: 4035 )

__declspec( naked ) int BoxOnPlaneSide (vec3_t emins, vec3_t emaxs, struct cplane_s *p)
{
	static int bops_initialized;
	static int Ljmptab[8];

	__asm {

		push ebx
			
		cmp bops_initialized, 1
		je  initialized
		mov bops_initialized, 1
		
		mov Ljmptab[0*4], offset Lcase0
		mov Ljmptab[1*4], offset Lcase1
		mov Ljmptab[2*4], offset Lcase2
		mov Ljmptab[3*4], offset Lcase3
		mov Ljmptab[4*4], offset Lcase4
		mov Ljmptab[5*4], offset Lcase5
		mov Ljmptab[6*4], offset Lcase6
		mov Ljmptab[7*4], offset Lcase7
			
initialized:

		mov edx,dword ptr[4+12+esp]
		mov ecx,dword ptr[4+4+esp]
		xor eax,eax
		mov ebx,dword ptr[4+8+esp]
		mov al,byte ptr[17+edx]
		cmp al,8
		jae Lerror
		fld dword ptr[0+edx]
		fld st(0)
		jmp dword ptr[Ljmptab+eax*4]
Lcase0:
		fmul dword ptr[ebx]
		fld dword ptr[0+4+edx]
		fxch st(2)
		fmul dword ptr[ecx]
		fxch st(2)
		fld st(0)
		fmul dword ptr[4+ebx]
		fld dword ptr[0+8+edx]
		fxch st(2)
		fmul dword ptr[4+ecx]
		fxch st(2)
		fld st(0)
		fmul dword ptr[8+ebx]
		fxch st(5)
		faddp st(3),st(0)
		fmul dword ptr[8+ecx]
		fxch st(1)
		faddp st(3),st(0)
		fxch st(3)
		faddp st(2),st(0)
		jmp LSetSides
Lcase1:
		fmul dword ptr[ecx]
		fld dword ptr[0+4+edx]
		fxch st(2)
		fmul dword ptr[ebx]
		fxch st(2)
		fld st(0)
		fmul dword ptr[4+ebx]
		fld dword ptr[0+8+edx]
		fxch st(2)
		fmul dword ptr[4+ecx]
		fxch st(2)
		fld st(0)
		fmul dword ptr[8+ebx]
		fxch st(5)
		faddp st(3),st(0)
		fmul dword ptr[8+ecx]
		fxch st(1)
		faddp st(3),st(0)
		fxch st(3)
		faddp st(2),st(0)
		jmp LSetSides
Lcase2:
		fmul dword ptr[ebx]
		fld dword ptr[0+4+edx]
		fxch st(2)
		fmul dword ptr[ecx]
		fxch st(2)
		fld st(0)
		fmul dword ptr[4+ecx]
		fld dword ptr[0+8+edx]
		fxch st(2)
		fmul dword ptr[4+ebx]
		fxch st(2)
		fld st(0)
		fmul dword ptr[8+ebx]
		fxch st(5)
		faddp st(3),st(0)
		fmul dword ptr[8+ecx]
		fxch st(1)
		faddp st(3),st(0)
		fxch st(3)
		faddp st(2),st(0)
		jmp LSetSides
Lcase3:
		fmul dword ptr[ecx]
		fld dword ptr[0+4+edx]
		fxch st(2)
		fmul dword ptr[ebx]
		fxch st(2)
		fld st(0)
		fmul dword ptr[4+ecx]
		fld dword ptr[0+8+edx]
		fxch st(2)
		fmul dword ptr[4+ebx]
		fxch st(2)
		fld st(0)
		fmul dword ptr[8+ebx]
		fxch st(5)
		faddp st(3),st(0)
		fmul dword ptr[8+ecx]
		fxch st(1)
		faddp st(3),st(0)
		fxch st(3)
		faddp st(2),st(0)
		jmp LSetSides
Lcase4:
		fmul dword ptr[ebx]
		fld dword ptr[0+4+edx]
		fxch st(2)
		fmul dword ptr[ecx]
		fxch st(2)
		fld st(0)
		fmul dword ptr[4+ebx]
		fld dword ptr[0+8+edx]
		fxch st(2)
		fmul dword ptr[4+ecx]
		fxch st(2)
		fld st(0)
		fmul dword ptr[8+ecx]
		fxch st(5)
		faddp st(3),st(0)
		fmul dword ptr[8+ebx]
		fxch st(1)
		faddp st(3),st(0)
		fxch st(3)
		faddp st(2),st(0)
		jmp LSetSides
Lcase5:
		fmul dword ptr[ecx]
		fld dword ptr[0+4+edx]
		fxch st(2)
		fmul dword ptr[ebx]
		fxch st(2)
		fld st(0)
		fmul dword ptr[4+ebx]
		fld dword ptr[0+8+edx]
		fxch st(2)
		fmul dword ptr[4+ecx]
		fxch st(2)
		fld st(0)
		fmul dword ptr[8+ecx]
		fxch st(5)
		faddp st(3),st(0)
		fmul dword ptr[8+ebx]
		fxch st(1)
		faddp st(3),st(0)
		fxch st(3)
		faddp st(2),st(0)
		jmp LSetSides
Lcase6:
		fmul dword ptr[ebx]
		fld dword ptr[0+4+edx]
		fxch st(2)
		fmul dword ptr[ecx]
		fxch st(2)
		fld st(0)
		fmul dword ptr[4+ecx]
		fld dword ptr[0+8+edx]
		fxch st(2)
		fmul dword ptr[4+ebx]
		fxch st(2)
		fld st(0)
		fmul dword ptr[8+ecx]
		fxch st(5)
		faddp st(3),st(0)
		fmul dword ptr[8+ebx]
		fxch st(1)
		faddp st(3),st(0)
		fxch st(3)
		faddp st(2),st(0)
		jmp LSetSides
Lcase7:
		fmul dword ptr[ecx]
		fld dword ptr[0+4+edx]
		fxch st(2)
		fmul dword ptr[ebx]
		fxch st(2)
		fld st(0)
		fmul dword ptr[4+ecx]
		fld dword ptr[0+8+edx]
		fxch st(2)
		fmul dword ptr[4+ebx]
		fxch st(2)
		fld st(0)
		fmul dword ptr[8+ecx]
		fxch st(5)
		faddp st(3),st(0)
		fmul dword ptr[8+ebx]
		fxch st(1)
		faddp st(3),st(0)
		fxch st(3)
		faddp st(2),st(0)
LSetSides:
		faddp st(2),st(0)
		fcomp dword ptr[12+edx]
		xor ecx,ecx
		fnstsw ax
		fcomp dword ptr[12+edx]
		and ah,1
		xor ah,1
		add cl,ah
		fnstsw ax
		and ah,1
		add ah,ah
		add cl,ah
		pop ebx
		mov eax,ecx
		ret
Lerror:
		int 3
	}
}
#pragma warning( default: 4035 )

/*
==============
PointInCylinderArc

True when point lies in a vertical cylinder shell of the given radius and
thickness, within halfHeight of center and between minYaw and maxYaw.
==============
*/
int PointInCylinderArc( const vec3_t point, float radius, const vec3_t center, float thickness, float minYaw, float maxYaw, float halfHeight )
{
	vec3_t delta;
	float dist;
	float d;
	float yaw;

	VectorSubtract(point, center, delta);
	dist = Vec2Normalize(delta);
	d = dist - radius;

	if ( d * d > thickness * thickness )
	{
		return 0;
	}

	if ( point[2] < center[2] - halfHeight || point[2] > center[2] + halfHeight )
	{
		return 0;
	}

	yaw = vectoyaw(delta);
	yaw = AngleNormalize360(yaw);

	if ( minYaw < maxYaw )
	{
		if ( yaw < maxYaw && yaw > minYaw )
		{
			return 1;
		}
	}
	else
	{
		if ( yaw < maxYaw || yaw > minYaw )
		{
			return 1;
		}
	}

	return 0;
}

/*
==============
BoxDistSqrdExceeds
==============
*/
int BoxDistSqrdExceeds( const vec3_t absmin, const vec3_t absmax, const vec3_t org, float fogOpaqueDistSqrd )
{
	int i;
	vec3_t mins;
	vec3_t maxs;
	float total;
	float minsSqrd;
	float maxsSqrd;
	float d;

	VectorSubtract(absmin, org, mins);
	VectorSubtract(absmax, org, maxs);

	total = 0.0f;

	for ( i = 0; i < 3; ++i )
	{
		if ( mins[i] * maxs[i] <= 0.0f )
		{
			continue;
		}

		minsSqrd = mins[i] * mins[i];
		maxsSqrd = maxs[i] * maxs[i];

		if ( maxsSqrd < minsSqrd )
		{
			d = maxsSqrd;
		}
		else
		{
			d = minsSqrd;
		}

		total += d;
	}

	return total > fogOpaqueDistSqrd;
}

/*
==============
Q_round
==============
*/
float Q_round( float x )
{
	return floor(x + 0.5f);
}

/*
==============
Vec3ScaleToUnitMax

Scales v so its largest component is 1 and returns that component.
==============
*/
float Vec3ScaleToUnitMax( const vec3_t v, vec3_t out )
{
	float max;
	float scale;

	max = v[0];

	if ( v[1] > max )
		max = v[1];

	if ( v[2] > max )
		max = v[2];

	if ( max == 0 )
	{
		out[0] = out[1] = out[2] = 1.0f;
		return 0;
	}

	scale = 1.0f / max;
	VectorScale(v, scale, out);
	return max;
}

/*
==============
VectorRotate

Rotates about x, then y, then z by the given angles in degrees.
==============
*/
void VectorRotate( const vec3_t vIn, const vec3_t vRotation, vec3_t out )
{
	vec3_t vWork;
	vec3_t va;
	int nIndex[3][2];
	int i;
	double s;
	double c;

	VectorCopy(vIn, va);
	VectorCopy(va, vWork);

	nIndex[0][0] = 1;
	nIndex[0][1] = 2;
	nIndex[1][0] = 2;
	nIndex[1][1] = 0;
	nIndex[2][0] = 0;
	nIndex[2][1] = 1;

	for ( i = 0; i < 3; i++ )
	{
		if ( vRotation[i] != 0 )
		{
			SinCos((vRotation[i] * M_PI / 180.0), &s, &c);
			vWork[nIndex[i][0]] = va[nIndex[i][0]] * c - va[nIndex[i][1]] * s;
			vWork[nIndex[i][1]] = va[nIndex[i][0]] * s + va[nIndex[i][1]] * c;
		}

		VectorCopy(vWork, va);
	}

	VectorCopy(vWork, out);
}

/*
==============
VectorRotateAroundOrigin
==============
*/
void VectorRotateAroundOrigin( const vec3_t point, const vec3_t angles, const vec3_t origin, vec3_t out )
{
	vec3_t delta;
	vec3_t rotated;

	VectorSubtract(point, origin, delta);
	VectorRotate(delta, angles, rotated);
	VectorAdd(rotated, origin, out);
}

/*
==============
SphericalToCartesian
==============
*/
void SphericalToCartesian( vec3_t out, float radius, float angle )
{
	float sy;
	float cy;
	float sp;
	float cp;

	FastSinCos(angle, &sy, &cy);
	FastSinCos(angle, &sp, &cp);
	out[0] = radius * cy * cp;
	out[1] = radius * sy * cp;
	out[2] = radius * sp;
}

/*
==============
PitchForYawOnNormal
==============
*/
float PitchForYawOnNormal( const float fYaw, const vec3_t normal )
{
	vec3_t forward;
	vec3_t dst;

	YawVectors(fYaw, forward, 0);
	ProjectPointOnPlane(forward, normal, dst);

	return vectopitch(dst);
}

/*
==============
Rand_Init
==============
*/
void Rand_Init( int seed )
{
	holdrand = seed;
}

/*
==============
flrand
==============
*/
float flrand( float min, float max )
{
	float result;

	holdrand = 214013 * holdrand + 2531011;
	result = (float)(holdrand >> 17);
	result = ((max - min) * result) / 32768.0f + min;
	return result;
}

/*
==============
irand
==============
*/
int irand( int min, int max )
{
	int result;

	holdrand = holdrand * 214013 + 2531011;
	result = holdrand >> 17;
	result = ((max - min) * result >> 15) + min;
	return result;
}

/*
==============
TransformVectorByAnimMat
==============
*/
// unreferenced; its name is not known
void TransformVectorByAnimMat( const float *in, const DObjAnimMat *mat, float *out )
{
	MatrixTransformVectorQuatTrans(in, mat, out);
}

/*
==============
AxisToQuat
==============
*/
void AxisToQuat( const vec3_t mat[3], vec4_t out )
{
	vec4_t test[4];
	int i;
	int best;
	float len;
	float bestLen;
	float invLen;

	test[0][0] = mat[1][2] - mat[2][1];
	test[0][1] = mat[2][0] - mat[0][2];
	test[0][2] = mat[0][1] - mat[1][0];
	test[0][3] = mat[0][0] + mat[1][1] + mat[2][2] + 1.0f;
	test[1][0] = mat[0][0] - mat[1][1] - mat[2][2] + 1.0f;
	test[1][1] = mat[1][0] + mat[0][1];
	test[1][2] = mat[2][0] + mat[0][2];
	test[1][3] = test[0][0];
	test[2][0] = test[1][1];
	test[2][1] = mat[1][1] - mat[0][0] - mat[2][2] + 1.0f;
	test[2][2] = mat[2][1] + mat[1][2];
	test[2][3] = test[0][1];
	test[3][0] = test[1][2];
	test[3][1] = test[2][2];
	test[3][2] = mat[2][2] - mat[1][1] - mat[0][0] + 1.0f;
	test[3][3] = test[0][2];

	best = -1;
	bestLen = 0.0f;

	for ( i = 0; i < 4; i++ )
	{
		len = Vec4LengthSq(test[i]);

		if ( !(bestLen >= len) )
		{
			bestLen = len;
			best = i;
		}
	}

	invLen = 1.0f / sqrtf(bestLen);
	VectorScale4(test[best], invLen, out);
}

/*
==============
SinCosDegrees

Exact results on the four axis angles.
==============
*/
void SinCosDegrees( float angle, float *pSin, float *pCos )
{
	if ( angle < 0.0f )
		angle = angle + 360.0f;

	if ( angle == 0.0f )
	{
		*pCos = 1.0f;
		*pSin = 0.0f;
	}
	else if ( angle == 90.0f )
	{
		*pCos = 0.0f;
		*pSin = 1.0f;
	}
	else if ( angle == 180.0f )
	{
		*pCos = -1.0f;
		*pSin = 0.0f;
	}
	else if ( angle == 270.0f )
	{
		*pCos = 0.0f;
		*pSin = -1.0f;
	}
	else
	{
		FastSinCos(angle * 0.017453292f, pSin, pCos);
	}
}

/*
==============
SnapFloatBits

Rounds away the low mantissa bits when they are within tolerance of a multiple.
==============
*/
float SnapFloatBits( float f, int tolerance, int bits )
{
	union
	{
		float f;
		int i;
	} u;
	int low;
	int one;
	int mask;

	one = 1 << bits;
	mask = one - 1;
	u.f = f;
	low = u.i & mask;

	if ( low <= tolerance )
	{
		u.i -= low;
	}
	else if ( one - low <= tolerance )
	{
		u.i += one - low;
	}

	return u.f;
}

/*
==============
SnapFloatToGrid
==============
*/
float SnapFloatToGrid( float value, float grid, float epsilon )
{
	float snapped;
	float err;

	value = SnapFloatBits(value, 4, 12);
	snapped = I_fround(value * grid) / grid;
	err = I_fabs(snapped - value);

	if ( err <= epsilon )
	{
		return snapped;
	}

	return value;
}

