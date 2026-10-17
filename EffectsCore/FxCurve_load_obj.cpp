#include <string.h>

struct FxCurve
{
	int dimensionCount;
	int keyCount;
	float keys[1];
};

void *Hunk_AllocAlignInternal( int size, int alignment );

// Pads the key array with keys at t = 0 and t = 1 when the source lacks them.
const FxCurve *FxCurve_AllocAndCreateWithKeys( const float *keyArray, int dimensionCount, int keyCount )
{
	bool needStartKey;
	bool needEndKey;
	int totalKeys;
	int keyIndex;
	int dimensionId;
	FxCurve *newCurve;
	int keySize;

	keySize = dimensionCount + 1;
	needStartKey = keyArray[0] != 0.0f;
	needEndKey = keyArray[( keyCount - 1 ) * keySize] != 1.0f;
	totalKeys = keyCount + ( needStartKey != 0 ) + ( needEndKey != 0 );
	newCurve = (FxCurve *)Hunk_AllocAlignInternal( keySize * totalKeys * 4 + 8, 4 );
	newCurve->dimensionCount = dimensionCount;
	keyIndex = 0;
	if ( needStartKey )
	{
		newCurve->keys[keyIndex * keySize] = 0.0f;
		for ( dimensionId = 0; dimensionId != dimensionCount; dimensionId++ )
			newCurve->keys[keyIndex * keySize + dimensionId + 1] = keyArray[dimensionId + 1];
		keyIndex++;
	}
	memcpy( &newCurve->keys[keyIndex * keySize], keyArray, 4 * keyCount * keySize );
	keyIndex += keyCount;
	if ( needEndKey )
	{
		newCurve->keys[keyIndex * keySize] = 1.0f;
		for ( dimensionId = 0; dimensionId != dimensionCount; dimensionId++ )
			newCurve->keys[keyIndex * keySize + dimensionId + 1] = keyArray[dimensionId + 1];
		keyIndex++;
	}
	newCurve->keyCount = keyCount;
	return newCurve;
}

// Curves live in the hunk and are shared, so a copy is the same pointer.
const FxCurve *FxCurve_Reference( const FxCurve *curve )
{
	return curve;
}

