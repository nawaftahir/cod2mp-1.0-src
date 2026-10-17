struct FxCurve
{
	int dimensionCount;
	int keyCount;
	float keys[1];
};

struct FxCurveIterator
{
	const FxCurve *master;
	int currentKeyIndex;
};

float FxCurve_Interpolate1d( const float *key, float t );

float FxCurve_Interpolate1d( const float *key, float t )
{
	float t0;
	float t1;
	float v0;
	float v1;

	t0 = key[0];
	t1 = key[2];
	v0 = key[1];
	v1 = key[3];
	return ( t - t0 ) * ( v1 - v0 ) / ( t1 - t0 ) + v0;
}

void FxCurveIterator_Create( FxCurveIterator *createe, const FxCurve *master )
{
	createe->master = master;
	createe->currentKeyIndex = 0;
}

// Nothing in the game calls this; original name unknown.
void FxCurveIterator_Release( FxCurveIterator *iterator )
{
	iterator->master = 0;
}

static float FxCurve_IntegrateSegment( float t0, float t1, float v0, float v1 )
{
	return ( ( v0 + v1 ) * 0.5f ) * ( t1 - t0 );
}

float FxCurve_Integrate( const FxCurve *curve, float normDuration )
{
	float result;
	int keyCount;
	int keySize;
	float val;
	const float *nextKey;
	const float *key;

	keyCount = curve->keyCount;
	keySize = curve->dimensionCount + 1;
	key = curve->keys;
	nextKey = key + keySize;
	result = 0.0f;
	while ( nextKey[0] < normDuration )
	{
		result += FxCurve_IntegrateSegment( key[0], nextKey[0], key[1], nextKey[1] );
		key = nextKey;
		nextKey += keySize;
	}
	val = FxCurve_Interpolate1d( key, normDuration );
	result += FxCurve_IntegrateSegment( key[0], normDuration, key[1], val );
	return result;
}

