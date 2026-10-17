#include "../qcommon/qcommon.h"

struct FxEffectDef;

struct FxRange
{
	float min;
	float max;
};

enum PrimType
{
	PRIM_NONE,
	PRIM_PARTICLE,
	PRIM_LINE,
	PRIM_TAIL,
	PRIM_CYLINDER,
	PRIM_EMITTER,
	PRIM_DECAL,
	PRIM_ORIENTED_PARTICLE,
	PRIM_FXRUNNER,
	PRIM_LIGHT,
	PRIM_CAMERA_SHAKE,
	PRIM_FLASH,
	PRIM_CLOUD
};

union TMediaElement
{
	XModel *model;
	Material *material;
	FxEffectDef *effect;
	void *data;
};

struct TMediaList
{
	TMediaElement *elements;
	unsigned short size;
	unsigned short maxSize;
};

struct MediaHandles
{
	void Shutdown();
	void AddHandle( TMediaElement item );
	void AddEffect( FxEffectDef *fx );

	TMediaList mMediaList;
};

struct GPGroup;

struct GPObject
{
	const char *GetName() { return mName; }

	const char *mName;
	GPObject *mNext;
	GPObject *mSortedNext;
	GPObject *mSortedPrev;
};

struct GPValue;

struct GPGroup : GPObject
{
	GPGroup *GetNext() { return (GPGroup *)mNext; }
	GPGroup *GetSubGroups() { return mSubGroups; }

	GPValue *mPairs;
	GPValue *mPairsSorted;
	GPValue *mPairsLast;
	GPGroup *mSubGroups;
	GPGroup *mSubGroupsSorted;
	GPGroup *mSubGroupsLast;
	GPGroup *mParent;
	bool mClean;
};

class GenericParser2
{
public:
	bool Parse( char **dataPtr, bool cleanFirst, bool writeable );
	GPGroup *GetBaseParseGroup() { return &mTopLevel; }

private:
	GPGroup mTopLevel;
	void *mTextPoolList;
	bool mWriteable;
};

// Only the fields read while loading are named; see FxScheduler for the full layout.
struct PrimitiveTemplate
{
	void Init();
	void Shutdown();
	bool ParsePrimitive( GPGroup *grp );

	char mName[32];
	char mMaterialImpact[32];
	PrimType mType;
	int mParentPrimIndex;
	FxRange mSpawnDelay;
	FxRange mSpawnCount;
	FxRange mLife;
	FxRange mSpawnRange;
	MediaHandles mMediaHandles;
	char mRest[0x2a4 - 0x70];
};

struct FxEffectDef
{
	const char *mEffectName;
	int mPrimitiveCount;
	PrimitiveTemplate *mPrimitives[24];
};

void FX_Print( const char *fmt, ... );
FxEffectDef *FX_TryRegisterEffect( const char *name );

extern FxEffectDef *g_fxDefaultEffect;
extern bool g_rendererExists;

// Original name unknown.
static void *FX_Alloc( int size )
{
	return Hunk_AllocAlignInternal(size, 4);
}

void FX_CleanTemplate( FxEffectDef *fx )
{
	int i;

	for ( i = 0; i < fx->mPrimitiveCount; ++i )
		fx->mPrimitives[i]->Shutdown();
}

// Original name unknown.
static bool FX_LoadEffectFile( GenericParser2 *parser, const char *name )
{
	int len;
	fileHandle_t f;
	char *buf;
	char path[64];
	char *text;

	sprintf(path, "fx/%s.efx", name);
	len = FS_FOpenFileByMode(path, &f, FS_READ);
	if ( len < 0 )
	{
		FX_Print("Effect file load failed: %s: file not found\n", path);
		return false;
	}
	buf = (char *)Hunk_AllocateTempMemoryInternal(len + 1);
	FS_Read(buf, len, f);
	FS_FCloseFile(f);
	buf[len] = 0;
	text = buf;
	parser->Parse(&text, true, false);
	Hunk_FreeTempMemory(buf);
	return true;
}

void FX_CreateDefaultEffect()
{
	g_fxDefaultEffect = FX_TryRegisterEffect("misc/missing_fx");
	if ( !g_fxDefaultEffect )
		Com_Error(ERR_DROP, "^1ERROR: could not load default effect file '%s'", "misc/missing_fx");
}

FxEffectDef *FX_RegisterEffect( const char *name )
{
	FxEffectDef *fx;
	char baseName[64];

	if ( *name == '/' || *name == '\\' )
		name++;
	if ( !strnicmp(name, "fx/", 3) )
	{
		Com_StripExtension(name + 3, baseName);
		strlwr(baseName);
		fx = FX_TryRegisterEffect(baseName);
		if ( fx )
			return fx;
	}
	else
	{
		FX_Print("Effect file '%s' must start with fx/.\n", name);
		fx = NULL;
	}
	fx = g_fxDefaultEffect;
	return fx;
}

// Original name unknown.
static void FX_AddPrimitiveToEffect( FxEffectDef *fx, PrimitiveTemplate *primTemp )
{
	int count;

	count = fx->mPrimitiveCount;
	if ( count >= 24 )
	{
		FX_Print("FxScheduler:  Error--too many primitives in an effect\n");
		return;
	}
	fx->mPrimitives[count] = primTemp;
	fx->mPrimitiveCount++;
}

// Original name unknown.
static bool FX_HasMaterials( const PrimitiveTemplate *primTemp )
{
	if ( !g_rendererExists )
		return true;
	if ( !primTemp->mMediaHandles.mMediaList.size )
		return false;
	return true;
}

// Original name unknown.
static bool FX_ValidatePrimitive( const PrimitiveTemplate *primTemp )
{
	if ( ( primTemp->mType == PRIM_PARTICLE || primTemp->mType == PRIM_ORIENTED_PARTICLE || primTemp->mType == PRIM_TAIL ) && !FX_HasMaterials(primTemp) )
	{
		FX_Print("^1FX Error, no materials defined for primitive template of type '%i'\n", primTemp->mType);
		return false;
	}
	return true;
}

FxEffectDef *FX_ParseEffect( GenericParser2 *parser, const char *name )
{
	GPGroup *group;
	PrimitiveTemplate *primTemp;
	const char *grpName;
	FxEffectDef *fx;
	PrimType type;
	int len;
	GPGroup *base;
	int index;

	if ( !FX_LoadEffectFile(parser, name) )
		return NULL;
	base = parser->GetBaseParseGroup();
	fx = (FxEffectDef *)FX_Alloc(sizeof(FxEffectDef));
	len = strlen(name) + 1;
	fx->mEffectName = (char *)FX_Alloc(len);
	strcpy((char *)fx->mEffectName, name);
	group = base->GetSubGroups();
	index = 0;
	while ( group )
	{
		grpName = group->GetName();
		if ( !strcasecmp(grpName, "particle") )
			type = PRIM_PARTICLE;
		else if ( !strcasecmp(grpName, "line") )
			type = PRIM_LINE;
		else if ( !strcasecmp(grpName, "tail") )
			type = PRIM_TAIL;
		else if ( !strcasecmp(grpName, "cylinder") )
			type = PRIM_CYLINDER;
		else if ( !strcasecmp(grpName, "emitter") )
			type = PRIM_EMITTER;
		else if ( !strcasecmp(grpName, "decal") )
			type = PRIM_DECAL;
		else if ( !strcasecmp(grpName, "orientedparticle") )
			type = PRIM_ORIENTED_PARTICLE;
		else if ( !strcasecmp(grpName, "fxrunner") )
			type = PRIM_FXRUNNER;
		else if ( !strcasecmp(grpName, "light") )
			type = PRIM_LIGHT;
		else if ( !strcasecmp(grpName, "cameraShake") )
			type = PRIM_CAMERA_SHAKE;
		else if ( !strcasecmp(grpName, "flash") )
			type = PRIM_FLASH;
		else if ( !strcasecmp(grpName, "cloud") )
			type = PRIM_CLOUD;
		else
			type = PRIM_NONE;
		if ( type )
		{
			primTemp = (PrimitiveTemplate *)FX_Alloc(sizeof(PrimitiveTemplate));
			primTemp->Init();
			primTemp->mType = type;
			primTemp->mParentPrimIndex = index;
			if ( !primTemp->ParsePrimitive(group) )
			{
				primTemp->Shutdown();
				FX_CleanTemplate(fx);
				FX_Print("^1FX Error while parsing segment type '%s'\n", group->GetName());
				return NULL;
			}
			if ( !FX_ValidatePrimitive(primTemp) )
			{
				primTemp->Shutdown();
				FX_CleanTemplate(fx);
				FX_Print("^1FX Error, invalid primitive template for effect '%s'\n", name);
				return NULL;
			}
			FX_AddPrimitiveToEffect(fx, primTemp);
		}
		group = group->GetNext();
		++index;
	}
	return fx;
}

void MediaHandles::AddHandle( TMediaElement item )
{
	TMediaElement *elements;

	if ( mMediaList.size == mMediaList.maxSize )
	{
		if ( mMediaList.maxSize )
			mMediaList.maxSize *= 2;
		else
			mMediaList.maxSize = 4;
		elements = (TMediaElement *)Z_MallocInternal(mMediaList.maxSize * sizeof(TMediaElement));
		if ( mMediaList.elements )
		{
			memcpy(elements, mMediaList.elements, mMediaList.size * sizeof(TMediaElement));
			Z_FreeInternal(mMediaList.elements);
		}
		mMediaList.elements = elements;
	}
	mMediaList.elements[mMediaList.size] = item;
	mMediaList.size++;
}

void MediaHandles::AddEffect( FxEffectDef *fx )
{
	TMediaElement item;

	item.effect = fx;
	AddHandle(item);
}

// Original name unknown; unreferenced in retail.
void FX_StripExtensionLower_497d90(const char *name, char *out)
{
	Com_StripExtension(name, out);
	_strlwr(out);
}

void MediaHandles::Shutdown()
{
	if ( !mMediaList.elements )
		return;
	Z_FreeInternal(mMediaList.elements);
	mMediaList.elements = NULL;
	mMediaList.size = 0;
	mMediaList.maxSize = 0;
}


