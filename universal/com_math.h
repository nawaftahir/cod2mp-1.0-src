#pragma once

#include "math.h"

typedef unsigned char byte;

typedef float vec_t;

typedef vec_t vec2_t[2];
typedef vec_t vec3_t[3];
typedef vec_t vec4_t[4];

typedef int fixed4_t;
typedef int fixed8_t;
typedef int fixed16_t;

extern const vec2_t vec2_origin;
extern const vec3_t vec3_origin;
extern const vec4_t vec4_origin;
extern const vec4_t colorBlack;
extern const vec4_t colorRed;
extern const vec4_t colorGreen;
extern const vec4_t colorLtGreen;
extern const vec4_t colorBlue;
extern const vec4_t colorLtBlue;
extern const vec4_t colorYellow;
extern const vec4_t colorLtYellow;
extern const vec4_t colorMdYellow;
extern const vec4_t colorMagenta;
extern const vec4_t colorCyan;
extern const vec4_t colorLtCyan;
extern const vec4_t colorMdCyan;
extern const vec4_t colorDkCyan;
extern const vec4_t colorWhite;
extern const vec4_t colorLtGrey;
extern const vec4_t colorMdGrey;
extern const vec4_t colorDkGrey;
extern const vec4_t colorOrange;
extern const vec4_t colorLtOrange;

#define IS_NAN isnan

#undef M_PI
#define M_PI 3.14159265358979323846f

// angle indexes
#define PITCH               0       // up / down
#define YAW                 1       // left / right
#define ROLL                2       // fall over

#define DEG2RAD( a ) ( ( ( a ) * M_PI ) / 180.0F )
#define RAD2DEG( a ) ( ( ( a ) * 180.0f ) / M_PI )

#define DEGINRAD  57.2957763671875f // degrees in one radian
#define RADINDEG  0.017453292519943295f // radian in one degree

#define Square( x ) ( ( x ) * ( x ) )

// scales by one single-precision constant
#define ANGLE2SHORT( x )  ( (int)( ( x ) * ( 65536.0f / 360.0f ) ) & 65535 )
#define SHORT2ANGLE( x )  ( ( x ) * ( 360.0f / 65536.0f ) )

#define SnapVector( v ) {v[0] = ( (int)( v[0] ) ); v[1] = ( (int)( v[1] ) ); v[2] = ( (int)( v[2] ) );}

float Q_fabs( float f );
float Q_acos( float c );

#ifndef TU_STATIC
#define TU_STATIC inline
#endif

TU_STATIC int I_fround(const float x);

// Scalar helpers. Definition order is emission order: each TU's used copies
// follow its last function in exactly this sequence.

/*
==============
I_fsel
==============
*/
TU_STATIC float I_fsel(const float x, const float y, const float z)
{
	return x >= 0.0f ? y : z;
}

/*
==============
I_sgn
==============
*/
TU_STATIC float I_sgn(const float x)
{
	return I_fsel(x, 1.0, -1.0);
}

/*
==============
I_fabs
==============
*/
TU_STATIC float I_fabs(const float value)
{
	return fabs(value);
}

/*
==============
I_sqrt
==============
*/
TU_STATIC float I_sqrt(const float value)
{
	return sqrt(value);
}

/*
==============
I_sel
==============
*/
TU_STATIC int I_sel(const int x, const int y, const int z)
{
	return x >= 0 ? y : z;
}

/*
==============
I_side
==============
*/
TU_STATIC int I_side(const float x)
{
	return x >= 0.0f;
}

/*
==============
I_fmax
==============
*/
TU_STATIC float I_fmax(const float x, const float y)
{
	return I_fsel(x - y, x, y);
}

/*
==============
I_fmin
==============
*/
TU_STATIC float I_fmin(const float x, const float y)
{
	return I_fsel(y - x, x, y);
}

/*
==============
I_fround
==============
*/

/*
==============
I_max
==============
*/
TU_STATIC int I_max(const int x, const int y)
{
	return I_sel(x - y, x, y);
}

/*
==============
I_min
==============
*/
TU_STATIC int I_min(const int x, const int y)
{
	return I_sel(y - x, x, y);
}

/*
==============
I_fclamp
==============
*/
TU_STATIC float I_fclamp(const float val, const float min, const float max)
{
	return I_fsel(min - val, min, I_fsel(val - max, max, val));
}

/*
==============
FloatAsInt
==============
*/
TU_STATIC int FloatAsInt(const float &f)
{
	return *(const int *)&f;
}

/*
==============
FloatIdentity
==============
*/
TU_STATIC float FloatIdentity(const float f)
{
	return f;
}

/*
==============
Q_rint
==============
*/
TU_STATIC int Q_rint(const float in)
{
	return I_fround(in);
}

/*
==============
I_clamp
==============
*/
TU_STATIC int I_clamp(const int val, const int min, const int max)
{
	return I_sel(min - val, min, I_sel(val - max, max, val));
}

/*
==============
ClampFloat
==============
*/
TU_STATIC float ClampFloat(float x, float minval, float maxval)
{
	if (x < minval)
		return minval;
	if (x > maxval)
		return maxval;
	return x;
}

/*
==============
I_fround
==============
*/
TU_STATIC int I_fround(const float x)
{
#ifdef _MSC_VER
	// fistp rounds to nearest; the bias sends exact halves up
	const double bias = 1.0 / (1 << 30);
	int result;

	__asm fld x
	__asm fadd bias
	__asm fistp result

	return result;
#else
	return (int)floor(x + 0.5f);
#endif
}

/*
==============
I_square
==============
*/
TU_STATIC float I_square(const float x)
{
	return x * x;
}

/*
==============
FloatIsNegative
==============
*/
TU_STATIC int const FloatIsNegative(const float x)
{
	return *(const int *)&x < 0;
}

/*
==============
FloatSign
==============
*/
TU_STATIC int const FloatSign(const float x)
{
	return -2 * FloatIsNegative(x) + 1;
}

/*
==============
FastCeil
==============
*/
TU_STATIC int const FastCeil(const float x)
{
#ifdef _MSC_VER
	// fistp rounds to nearest; 0.5 - 2^-30 sends non-integers up and keeps
	// exact integers in place
	const double bias = 0.5 - 1.0 / (1 << 30);
	int result;

	__asm fld x
	__asm fadd bias
	__asm fistp result

	return result;
#else
	return (int)ceil(x);
#endif
}

/*
==============
FastFloor

Original name unknown.
==============
*/
TU_STATIC int const FastFloor(const float x)
{
#ifdef _MSC_VER
	// fistp rounds to nearest; 0.5 - 2^-30 sends non-integers down and keeps
	// exact integers in place
	const double bias = 0.5 - 1.0 / (1 << 30);
	int result;

	__asm fld x
	__asm fsub bias
	__asm fistp result

	return result;
#else
	return (int)floor(x);
#endif
}

/*
==============
I_rsqrt
==============
*/
TU_STATIC float I_rsqrt(const float number)
{
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = number * 0.5F;
	y = number;
	i = *(long*)&y;                        // evil floating point bit level hacking
	i = 0x5f3759df - (i >> 1);               // what the fuck?
	y = *(float*)&i;
	y = y * (threehalfs - (x2 * y * y));   // 1st iteration
//	y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

	return y;
}

/*
==============
lerp
==============
*/
TU_STATIC float lerp(float from, float to, float t)
{
	return (1 - t) * from + to * t;
}

/*
==============
FastSinCos
==============
*/
TU_STATIC void FastSinCos(const float value, float *pSin, float *pCos)
{
	__asm
	{
		push eax
		push ecx
		fld value
		fsincos
		mov ecx, pCos
		mov eax, pSin
		fstp dword ptr [ecx]
		fstp dword ptr [eax]
		pop ecx
		pop eax
	}
}

/*
==============
SinCos
==============
*/
TU_STATIC void SinCos(double value, double *pSin, double *pCos)
{
	__asm
	{
		push eax
		push ecx
		fld value
		fsincos
		mov ecx, pCos
		mov eax, pSin
		fstp qword ptr [ecx]
		fstp qword ptr [eax]
		pop ecx
		pop eax
	}
}

// Vector helpers, same rule; this block follows the scalar one.

/*
==============
Vector2Clear
==============
*/
TU_STATIC void Vector2Clear(vec2_t v)
{
	v[0] = 0;
	v[1] = 0;
}

/*
==============
Vector2Set
==============
*/
TU_STATIC void Vector2Set(vec2_t v, const float x, const float y)
{
	v[0] = x;
	v[1] = y;
}

/*
==============
Vector2Copy
==============
*/
TU_STATIC void Vector2Copy(const vec2_t a, vec2_t b)
{
	b[0] = a[0];
	b[1] = a[1];
}

/*
==============
Vector2Compare
==============
*/
TU_STATIC bool Vector2Compare(const vec2_t v1, const vec2_t v2)
{
	return v1[0] == v2[0] && v1[1] == v2[1];
}

/*
==============
Vector2Add
==============
*/
TU_STATIC void Vector2Add(const vec2_t a, const vec2_t b, vec2_t c)
{
	c[0] = a[0] + b[0];
	c[1] = a[1] + b[1];
}

/*
==============
Vector2Subtract
==============
*/
TU_STATIC void Vector2Subtract(const vec2_t a, const vec2_t b, vec2_t c)
{
	c[0] = a[0] - b[0];
	c[1] = a[1] - b[1];
}

/*
==============
Vec2Scale
==============
*/
TU_STATIC void Vec2Scale(const vec2_t v, const float s, vec2_t o)
{
	o[0] = s * v[0];
	o[1] = s * v[1];
}

/*
==============
VectorMA2
==============
*/
TU_STATIC void VectorMA2(const vec2_t v, const float s, const vec2_t b, vec2_t o)
{
	o[0] = v[0] + s * b[0];
	o[1] = v[1] + s * b[1];
}

/*
==============
Dot2Product
==============
*/
TU_STATIC float Dot2Product(const vec2_t a, const vec2_t b)
{
	return a[0] * b[0] + a[1] * b[1];
}

/*
==============
Vec2Multiply
==============
*/
TU_STATIC float Vec2Multiply(const vec2_t v)
{
	return v[0] * v[0] + v[1] * v[1];
}

/*
==============
Vec2Length
==============
*/
TU_STATIC vec_t Vec2Length(const vec2_t v)
{
	return I_sqrt(v[0] * v[0] + v[1] * v[1]);
}

/*
==============
VectorClear
==============
*/
TU_STATIC void VectorClear(vec3_t v)
{
	v[0] = 0;
	v[1] = 0;
	v[2] = 0;
}

/*
==============
VectorSet
==============
*/
TU_STATIC void VectorSet(vec3_t v, const float x, const float y, const float z)
{
	v[0] = x;
	v[1] = y;
	v[2] = z;
}

/*
==============
VectorCopy
==============
*/
TU_STATIC void VectorCopy(const vec3_t a, vec3_t b)
{
	b[0] = a[0];
	b[1] = a[1];
	b[2] = a[2];
}

/*
==============
Vec3CopyOrClear
==============
*/
TU_STATIC void Vec3CopyOrClear(const vec3_t in, vec3_t out)
{
	if (in)
		VectorCopy(in, out);
	else
		VectorClear(out);
}

/*
==============
VectorNegate
==============
*/
TU_STATIC void VectorNegate(const vec3_t a, vec3_t b)
{
	b[0] = -a[0];
	b[1] = -a[1];
	b[2] = -a[2];
}

/*
==============
VectorInverse
==============
*/
TU_STATIC void VectorInverse(vec3_t v)
{
	v[0] = -v[0];
	v[1] = -v[1];
	v[2] = -v[2];
}

/*
==============
VectorAdd
==============
*/
TU_STATIC void VectorAdd(const vec3_t a, const vec3_t b, vec3_t c)
{
	c[0] = a[0] + b[0];
	c[1] = a[1] + b[1];
	c[2] = a[2] + b[2];
}

/*
==============
VectorSubtract
==============
*/
TU_STATIC void VectorSubtract(const vec3_t a, const vec3_t b, vec3_t c)
{
	c[0] = a[0] - b[0];
	c[1] = a[1] - b[1];
	c[2] = a[2] - b[2];
}

/*
==============
Vec3Avg
==============
*/
TU_STATIC void Vec3Avg(const vec3_t a, const vec3_t b, vec3_t sum)
{
	sum[0] = (a[0] + b[0]) * 0.5f;
	sum[1] = (a[1] + b[1]) * 0.5f;
	sum[2] = (a[2] + b[2]) * 0.5f;
}

/*
==============
Vec3LerpFrom
==============
*/
TU_STATIC void Vec3LerpFrom(const vec3_t from, const vec3_t to, float frac, vec3_t out)
{
	out[0] = from[0] + (to[0] - from[0]) * frac;
	out[1] = from[1] + (to[1] - from[1]) * frac;
	out[2] = from[2] + (to[2] - from[2]) * frac;
}

/*
==============
VectorScale
==============
*/
TU_STATIC void VectorScale(const vec3_t v, const float s, vec3_t o)
{
	o[0] = s * v[0];
	o[1] = s * v[1];
	o[2] = s * v[2];
}

/*
==============
Vec3Mul
==============
*/
TU_STATIC void Vec3Mul(const vec3_t a, const vec3_t b, vec3_t o)
{
	o[0] = a[0] * b[0];
	o[1] = a[1] * b[1];
	o[2] = a[2] * b[2];
}

/*
==============
VectorMA
==============
*/
TU_STATIC void VectorMA(const vec3_t v, const float s, const vec3_t b, vec3_t o)
{
	o[0] = v[0] + s * b[0];
	o[1] = v[1] + s * b[1];
	o[2] = v[2] + s * b[2];
}

/*
==============
DotProduct
==============
*/
TU_STATIC float DotProduct(const vec3_t a, const vec3_t b)
{
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

/*
==============
VectorCompare
==============
*/
TU_STATIC bool VectorCompare(const vec3_t v1, const vec3_t v2)
{
	return v1[0] == v2[0] && v1[1] == v2[1] && v1[2] == v2[2];
}

/*
==============
VectorLengthSquared
==============
*/
TU_STATIC vec_t VectorLengthSquared(const vec3_t v)
{
	return v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
}

/*
==============
VectorLength
==============
*/
TU_STATIC vec_t VectorLength(const vec3_t v)
{
	return I_sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

/*
==============
Byte4PackRgba
==============
*/
TU_STATIC void Byte4PackRgba(const float *from, unsigned char *to)
{
	to[0] = I_max(0, I_min(255, I_fround(from[0] * 255.0f)));
	to[1] = I_max(0, I_min(255, I_fround(from[1] * 255.0f)));
	to[2] = I_max(0, I_min(255, I_fround(from[2] * 255.0f)));
	to[3] = I_max(0, I_min(255, I_fround(from[3] * 255.0f)));
}


/*
==============
Vector4Clear
==============
*/
TU_STATIC void Vector4Clear(vec4_t v)
{
	v[0] = 0;
	v[1] = 0;
	v[2] = 0;
	v[3] = 0;
}

/*
==============
Vector4Set
==============
*/
TU_STATIC void Vector4Set(vec4_t v, const float x, const float y, const float z, const float w)
{
	v[0] = x;
	v[1] = y;
	v[2] = z;
	v[3] = w;
}

/*
==============
VectorCopy4
==============
*/
TU_STATIC void VectorCopy4(const vec4_t a, vec4_t b)
{
	b[0] = a[0];
	b[1] = a[1];
	b[2] = a[2];
	b[3] = a[3];
}

/*
==============
Vector4Compare
==============
*/
TU_STATIC bool Vector4Compare(const vec4_t v1, const vec4_t v2)
{
	return v1[0] == v2[0] && v1[1] == v2[1] && v1[2] == v2[2] && v1[3] == v2[3];
}

/*
==============
Byte4Copy
==============
*/
TU_STATIC void Byte4Copy(const unsigned char *from, unsigned char *to)
{
	*(int *)to = *(const int *)from;
}

/*
==============
Byte4Compare
==============
*/
TU_STATIC bool Byte4Compare(const unsigned char *a, const unsigned char *b)
{
	return *(const int *)a == *(const int *)b;
}

/*
==============
VectorAdd4
==============
*/
TU_STATIC void VectorAdd4(const vec4_t a, const vec4_t b, vec4_t c)
{
	c[0] = a[0] + b[0];
	c[1] = a[1] + b[1];
	c[2] = a[2] + b[2];
	c[3] = a[3] + b[3];
}

/*
==============
VectorSubtract4
==============
*/
TU_STATIC void VectorSubtract4(const vec4_t a, const vec4_t b, vec4_t c)
{
	c[0] = a[0] - b[0];
	c[1] = a[1] - b[1];
	c[2] = a[2] - b[2];
	c[3] = a[3] - b[3];
}

/*
==============
VectorScale4
==============
*/
TU_STATIC void VectorScale4(const vec4_t v, const float s, vec4_t o)
{
	o[0] = s * v[0];
	o[1] = s * v[1];
	o[2] = s * v[2];
	o[3] = s * v[3];
}

/*
==============
VectorMA4
==============
*/
TU_STATIC void VectorMA4(const vec4_t v, const float s, const vec4_t b, vec4_t o)
{
	o[0] = v[0] + s * b[0];
	o[1] = v[1] + s * b[1];
	o[2] = v[2] + s * b[2];
	o[3] = v[3] + s * b[3];
}

/*
==============
Vec4LengthSq
==============
*/
TU_STATIC vec_t Vec4LengthSq(const vec4_t v)
{
	return v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3];
}

/*
==============
DotProduct4
==============
*/
TU_STATIC float DotProduct4(const vec4_t a, const vec4_t b)
{
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2] + a[3] * b[3];
}

vec_t Vec2LengthSq( const vec2_t v );
void MatrixTransformVector(const vec3_t in1, const vec3_t in2[3], vec3_t out);
void MatrixTransformVector43(const vec3_t in1, const float in2[4][3], vec3_t out);
void MatrixTransposeTransformVector(const vec3_t in1, const vec3_t in2[3], vec3_t out);
void MatrixTransposeTransformVector43(const vec3_t in1, const float in2[4][3], vec3_t out);
void MatrixInverse(const float in1[3][3], float out[3][3]);
void MatrixMultiply( const float in1[3][3], const float in2[3][3], float out[3][3] );
void AngleVectors( const vec3_t angles, vec3_t forward, vec3_t right, vec3_t up );
void AnglesToAxis( const vec3_t angles, vec3_t axis[3] );
vec_t Vec2Normalize( vec3_t v );
vec_t Vec3Normalize( vec3_t v );
vec_t Vec4Normalize( vec4_t v );
vec_t Vec2NormalizeTo( const vec2_t v, vec2_t out );
vec_t Vec3NormalizeTo( const vec3_t v, vec3_t out );
void Vec3Cross(const vec3_t v0, const vec3_t v1, vec3_t cross);
float AngleNormalize360( float angle );
float AngleNormalize180( float angle );
float AngleDelta( float angle1, float angle2 );
int BoxOnPlaneSide( vec3_t emins, vec3_t emaxs, struct cplane_s *p );
float RadiusFromBounds( const vec3_t mins, const vec3_t maxs );
float RadiusFromBounds2D( const vec2_t mins, const vec2_t maxs );
void SnapAngles(vec3_t angles);
void vectoangles( const vec3_t value1, vec3_t angles );
void AxisToAngles( vec3_t axis[3], vec3_t angles );
float AngleMod( float a );
float AngleSubtract( float a1, float a2 );
vec_t vectosignedyaw(vec3_t vec);
int BoxDistSqrdExceeds(const vec3_t absmin, const vec3_t absmax, const vec3_t org, float fogOpaqueDistSqrd);
float Vec3Distance(const vec3_t v1, const vec3_t v2);
float Vec3DistanceSq(const vec3_t v1, const vec3_t v2);
inline void Vec3Lerp(const vec3_t start, const vec3_t end, float fraction, vec3_t endpos)
{
	endpos[0] = start[0] + fraction * (end[0] - start[0]);
	endpos[1] = start[1] + fraction * (end[1] - start[1]);
	endpos[2] = start[2] + fraction * (end[2] - start[2]);
}
float DiffTrack(float tgt, float cur, float rate, float deltaTime);
float DiffTrackAngle(float tgt, float cur, float rate, float deltaTime);
float AngleNormalize180Accurate(float angle);
float AngleNormalize360Accurate(float angle);
void RotateVec2( vec2_t v, float degrees );
float vectoyaw( const vec3_t vec );
float vectopitch( const vec3_t vec );
float PitchForYawOnNormal(const float fYaw, const vec3_t normal);
float Abs(const vec3_t v);
void YawVectors2D(const float yaw, vec2_t forward, vec2_t right);
void YawVectors(const float yaw, vec3_t forward, vec3_t right);
void ShrinkBoundsToHeight(vec3_t mins, vec3_t maxs);
void ClearBounds( vec3_t mins, vec3_t maxs );
void AddPointToBounds(const vec3_t v, vec3_t mins, vec3_t maxs);
void AddPointToBounds2D(const vec2_t v, vec2_t mins, vec2_t maxs);
byte DirToByte(const vec3_t dir);
void ByteToDir(const int b, vec3_t dir);
void Rand_Init(int seed);
float flrand(float min, float max);
int irand(int min, int max);
float RotationToYaw(const vec2_t rot);
void MatrixTranspose(const float in[3][3], float out[3][3]);
void MatrixMultiply43(const float in1[4][3], const float in2[4][3], float out[4][3]);
void MatrixInverseOrthogonal43(const float in[4][3], float out[4][3]);
void ExpandBoundsToWidth(vec3_t mins, vec3_t maxs);
void YawToAxis(float yaw, vec3_t axis[3]);
void ProjectPointOnPlane(const vec3_t p, const vec3_t normal, vec3_t dst);
void RoundFloatArray(vec3_t x, vec3_t y);
void TransposeMatrix( const vec3_t matrix[3], vec3_t transpose[3] );
void RotatePoint( vec3_t point, const vec3_t matrix[3] );
void AnglesSubtract(const vec3_t v1, const vec3_t v2, vec3_t v3);
float Vec2DistanceSq(const vec2_t v1, const vec2_t v2);
float Vec2Distance(const vec2_t v1, const vec2_t v2);
void CreateRotationMatrix( const vec3_t angles, vec3_t matrix[3] );
bool Vec3IsNormalized(const vec3_t v);
bool Vec4IsNormalized(const vec4_t v);
