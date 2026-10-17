#include <math.h>
#include "com_math.h"

struct FxCurve;

struct FxRange
{
	float min;
	float max;
};

struct FxChannel
{
	const FxCurve *curve;
	FxRange scaleRange;
};

// Pre-curve channel description kept for old effect files.
struct FxChannelBackwardCompatible
{
	FxRange start[3];
	FxRange end[3];
	FxRange parm;
	int flags;
};

const FxCurve *FxCurve_AllocAndCreateWithKeys( const float *keyArray, int dimensionCount, int keyCount );
const FxCurve *FxCurve_Reference( const FxCurve *curve );
void FxRange_SetRange( FxRange *range, float min, float max );
float __stdcall FxRange_GetValPct( const FxRange *range, float pct );
float flrand( float min, float max );

void FxChannel_CreateDefault( FxChannel *createe, int dimensions, float value1, float value2 )
{
	const int keyCount = 2;
	float keys[16];
	int keySize;
	int dimensionId;

	keySize = dimensions + 1;
	keys[0] = 0.0f;
	keys[keySize] = 1.0f;
	for ( dimensionId = 0; dimensionId != dimensions; dimensionId++ )
	{
		keys[dimensionId + 1] = value1;
		keys[keySize + dimensionId + 1] = value2;
	}
	createe->curve = FxCurve_AllocAndCreateWithKeys( keys, dimensions, keyCount );
	FxRange_SetRange( &createe->scaleRange, 1.0f, 1.0f );
}

// Nothing in the game calls this; original name unknown.
void FxChannel_Copy( const FxChannel *source, FxChannel *target )
{
	target->curve = FxCurve_Reference( source->curve );
	target->scaleRange = source->scaleRange;
}

// The three curve shapes a migrated channel can take; original names unknown.
static void FxChannel_CreateLinear( FxChannel *target, int dimensions, const float *startVals, const float *endVals )
{
	float keys[16];
	int keySize;
	int dimensionId;

	keySize = dimensions + 1;
	keys[0] = 0.0f;
	keys[keySize] = 1.0f;
	for ( dimensionId = 0; dimensionId != dimensions; dimensionId++ )
	{
		keys[dimensionId + 1] = startVals[dimensionId];
		keys[keySize + dimensionId + 1] = endVals[dimensionId];
	}
	target->curve = FxCurve_AllocAndCreateWithKeys( keys, dimensions, 2 );
}

static void FxChannel_CreateDelayed( FxChannel *target, int dimensions, const float *startVals, const float *endVals, float delayFraction )
{
	float keys[24];
	int keySize;
	int dimensionId;

	keySize = dimensions + 1;
	keys[0] = 0.0f;
	keys[keySize] = delayFraction;
	keys[keySize * 2] = 1.0f;
	for ( dimensionId = 0; dimensionId != dimensions; dimensionId++ )
	{
		keys[dimensionId + 1] = startVals[dimensionId];
		keys[keySize + dimensionId + 1] = startVals[dimensionId];
		keys[keySize * 2 + dimensionId + 1] = endVals[dimensionId];
	}
	target->curve = FxCurve_AllocAndCreateWithKeys( keys, dimensions, 3 );
}

static void FxChannel_CreateSampled( FxChannel *target, int dimensions, int flags, const float *startVals, const float *endVals, float delayFraction, float waveParm )
{
	float keys[128];
	float t;
	unsigned int timeStep;
	float val;
	int keySize;
	float *key;
	int dimensionId;
	float lerp;
	float envelope;

	keySize = dimensions + 1;
	timeStep = 0;
	t = 0.0f;
	while ( timeStep != 16 )
	{
		if ( t > 1.0f )
			t = 1.0f;
		lerp = 1.0f;
		envelope = 1.0f;
		if ( flags & 1 )
			lerp = 1.0f - t;
		if ( ( flags & 0xC ) == 4 )
		{
			if ( t > delayFraction )
				envelope = 1.0f - ( t - delayFraction ) / ( 1.0f - delayFraction );
			if ( flags & 1 )
				lerp = lerp * 0.5f + envelope * 0.5f;
			else
				lerp = envelope;
		}
		else if ( ( flags & 0xC ) == 8 )
		{
			lerp = (float)cos( t * waveParm ) * lerp;
		}
		else if ( ( flags & 0xC ) == 0xC )
		{
			if ( t < delayFraction )
				envelope = ( delayFraction - t ) / delayFraction;
			else
				envelope = 0.0f;
			if ( flags & 1 )
				lerp = lerp * 0.5f + envelope * 0.5f;
			else
				lerp = envelope;
		}
		key = &keys[timeStep * keySize];
		*key = t;
		for ( dimensionId = 0; dimensionId != dimensions; dimensionId++ )
		{
			val = startVals[dimensionId] * lerp + endVals[dimensionId] * ( 1.0f - lerp );
			if ( val < 0.0f )
				val = 0.0f;
			else if ( lerp > 1.0f )
				val = 1.0f;
			if ( flags & 2 )
				val = flrand( 0.0f, val );
			key[dimensionId + 1] = val;
		}
		timeStep++;
		t = t + 1.0f / 15.0f;
	}
	target->curve = FxCurve_AllocAndCreateWithKeys( keys, dimensions, 16 );
}

void FxChannel_CreateViaMigration( const FxChannelBackwardCompatible *source, int dimensions, float lifetime, bool forceUnitScale, FxChannel *target )
{
	float scaleFactor;
	float invScale;
	float halfRange;
	float startVals[3];
	float endVals[3];
	float parmVal;
	float delayFraction;
	float waveParm;
	int flags;
	int dimensionId;

	scaleFactor = 1.0f;
	if ( !forceUnitScale )
	{
		scaleFactor = I_fmax( FxRange_GetValPct( &source->start[0], 0.5f ), FxRange_GetValPct( &source->end[0], 0.5f ) ) * 1.3333334f;
	}
	if ( scaleFactor < 1.0f )
		scaleFactor = 1.0f;
	halfRange = ( source->start[0].max - source->start[0].min ) / 2.0f;
	FxRange_SetRange( &target->scaleRange, scaleFactor - halfRange, scaleFactor + halfRange );
	invScale = 1.0f / scaleFactor;
	parmVal = 0.0f;
	for ( dimensionId = 0; dimensionId != dimensions; dimensionId++ )
	{
		startVals[dimensionId] = FxRange_GetValPct( &source->start[dimensionId], 0.5f ) * invScale;
		endVals[dimensionId] = FxRange_GetValPct( &source->end[dimensionId], 0.5f ) * invScale;
		parmVal = FxRange_GetValPct( &source->parm, 0.5f ) * invScale;
	}
	delayFraction = parmVal * 0.01f;
	waveParm = parmVal * ( 3.14159265f * 0.001f ) * lifetime;
	flags = source->flags;
	if ( ( flags & 1 ) && !( flags & 0xC ) )
		FxChannel_CreateLinear( target, dimensions, startVals, endVals );
	else if ( !( flags & 1 ) && ( flags & 0xC ) == 4 )
		FxChannel_CreateDelayed( target, dimensions, startVals, endVals, delayFraction );
	else
		FxChannel_CreateSampled( target, dimensions, flags, startVals, endVals, delayFraction, waveParm );
}

