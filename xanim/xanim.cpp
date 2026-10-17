#include "../qcommon/qcommon.h"
#include "../script/script_public.h"

extern dvar_t *com_developer;
static XAnimInfo g_xAnimInfo[4096];
static unsigned int g_end;
static XAnimClientNotify g_notifyList[128];
static int g_notifyListSize = 0;
static bool g_anim_developer;

void XAnimSetNotifyIndex(const XAnimTree_s *tree, XAnimInfo *info, const XAnimEntry *entry);
void XAnimCalcParts_unsigned_short_(const XAnimParts_s *parts, const unsigned char *animToModel, float time, float weightScale, DObjAnimMat *rotTransArray, int *ignorePartBits);
void XAnimCalcParts_unsigned_char_(const XAnimParts_s *parts, const unsigned char *animToModel, float time, float weightScale, DObjAnimMat *rotTransArray, int *ignorePartBits);
bool XAnimIsLeafOrSync(const XAnim_s *anims, unsigned int animIndex);
void XAnimClearParentGoalWeights(XAnimTree_s *tree, unsigned int animIndex, float goalTime);

void XAnimFreeList(XAnim_s *anims);
void XAnim_GetTimeIndexCompressed_unsigned_char(XAnimTime *animTime, const unsigned char *indices, int tableSize, int *keyFrameIndex, float *keyFrameLerpFrac);
void XAnim_CalcRotDeltaDuring_unsigned_char_(XAnimDeltaPart *animDelta, const float time, int numFrames, float *rotDelta);
void XAnim_CalcPosDeltaDuring_unsigned_char_(XAnimDeltaPart *animDelta, const float time, int numFrames, float *posDelta);
void XAnim_GetTimeIndexCompressed_unsigned_short_(XAnimTime *animTime, const unsigned short *indices, int tableSize, int *keyFrameIndex, float *keyFrameLerpFrac);
void XAnim_CalcRotDeltaDuring_unsigned_short_(XAnimDeltaPart *animDelta, const float time, int numFrames, float *rotDelta);
void XAnim_CalcPosDeltaDuring_unsigned_short_(XAnimDeltaPart *animDelta, const float time, int numFrames, float *posDelta);
void XAnimProcessServerNotify(const XAnimTree_s *tree, XAnimInfo *info, const XAnimEntry *entry, float dtime);
const char* XAnimGetAnimDebugName(const XAnim_s *anims, unsigned int animIndex);
XAnimInfo* XAnimGetInfo(XAnimTree_s *tree, unsigned int infoIndex);
void XAnimClearTree(XAnimTree_s *tree);
int XAnimSetGoalWeightInternal(XAnimTree_s *tree, unsigned int animIndex, float goalWeight, float goalTime, float rate, bool useGoalWeight, unsigned int notifyName, unsigned int notifyType);
bool XAnimIsLeafOrSync( const XAnim_s *anims, unsigned int animIndex );
void XAnimSetNotifyIndex( const XAnimTree_s *tree, XAnimInfo *info, const XAnimEntry *entry );
void XAnimRestart(XAnimTree_s *tree, unsigned int infoIndex);
void XAnimClearParentGoalWeights( XAnimTree_s *tree, unsigned int animIndex, float blendTime );
int XAnimSetGoalWeight(XAnimTree_s *tree, unsigned int animIndex, float goalWeight, float goalTime, float rate, unsigned int notifyName, unsigned int notifyType, int bRestart);
void XAnimSetCompleteGoalWeight(XAnimTree_s *tree, unsigned int animIndex, float goalWeight, float goalTime, float rate, unsigned int notifyName, unsigned int notifyType, int bRestart);
void XAnimCalcParts_unsigned_char_( const XAnimParts_s *parts, const unsigned char *animToModel, float time, float weightScale, DObjAnimMat *rotTransArray, int *ignorePartBits );
void XAnimCalcParts_unsigned_short_( const XAnimParts_s *parts, const unsigned char *animToModel, float time, float weightScale, DObjAnimMat *rotTransArray, int *ignorePartBits );

static byte xanimUnusedFlag;

int XAnimGetNumChildren(const XAnim_s *anim, unsigned int animIndex)
{
	return anim->entries[animIndex].numAnims;
}

static inline float ShortLerpAsVec(short from, short to, float frac)
{
	return (float)from + (float)(to - from) * frac;
}

static inline void Short2LerpAsVec2(const short *from, const short *to, float frac, float *out)
{
	out[0] = ShortLerpAsVec(from[0], to[0], frac);
	out[1] = ShortLerpAsVec(from[1], to[1], frac);
}

static inline void Vec2MadShort2Lerp(const float *from, float scale, const short *to1, const short *to2, float frac, float *out)
{
	out[0] = from[0] + ShortLerpAsVec(to1[0], to2[0], frac) * scale;
	out[1] = from[1] + ShortLerpAsVec(to1[1], to2[1], frac) * scale;
}

static inline void Vec4MadShort4Lerp(const float *from, float scale, const short *to1, const short *to2, float frac, float *out)
{
	out[0] = from[0] + ShortLerpAsVec(to1[0], to2[0], frac) * scale;
	out[1] = from[1] + ShortLerpAsVec(to1[1], to2[1], frac) * scale;
	out[2] = from[2] + ShortLerpAsVec(to1[2], to2[2], frac) * scale;
	out[3] = from[3] + ShortLerpAsVec(to1[3], to2[3], frac) * scale;
}

static inline void Vec2MadShort2(const float *from, float frac, const short *to, float *out)
{
	out[0] = from[0] + (float)to[0] * frac;
	out[1] = from[1] + (float)to[1] * frac;
}

static inline void Vec4MadShort4(const float *from, float frac, const short *to, float *out)
{
	out[0] = from[0] + (float)to[0] * frac;
	out[1] = from[1] + (float)to[1] * frac;
	out[2] = from[2] + (float)to[2] * frac;
	out[3] = from[3] + (float)to[3] * frac;
}

static inline void Short4ScaleAsVec4(short *from, float scale, float *to)
{
	to[0] = scale * from[0];
	to[1] = scale * from[1];
	to[2] = scale * from[2];
	to[3] = scale * from[3];
}

static inline float FloatLerp(float from, float to, float frac)
{
	return (to - from) * frac + from;
}

static inline void Vec3MadVec3Lerp(const float *from, float scale, const float *to1, const float *to2, float frac, float *out)
{
	out[0] = from[0] + FloatLerp(to1[0], to2[0], frac) * scale;
	out[1] = from[1] + FloatLerp(to1[1], to2[1], frac) * scale;
	out[2] = from[2] + FloatLerp(to1[2], to2[2], frac) * scale;
}

static inline void Short2CopyAsVec2(const short *from, float *to)
{
	to[0] = (float)from[0];
	to[1] = (float)from[1];
}

void XAnimInit()
{
	int i;

	for ( i = 0; i < 4096; ++i )
	{
		g_xAnimInfo[i].prev = (i + 4095) % 4096;
		g_xAnimInfo[i].next = (i + 1) % 4096;
	}

	g_xAnimInfo->state.time = 0.0;
	g_xAnimInfo->state.oldTime = 0.0;
	g_xAnimInfo->state.time = 0.0;
	g_xAnimInfo->state.oldTime = 0.0;
	g_xAnimInfo->state.cycleCount = 0;
	g_xAnimInfo->state.oldCycleCount = 0;
	g_xAnimInfo->state.cycleCount = 0;
	g_xAnimInfo->state.oldCycleCount = 0;

	g_end = SL_GetString_("end", 0, 3);
	g_anim_developer = com_developer->current.integer != 0;
}

void XAnimShutdown()
{
	if ( g_end )
	{
		SL_RemoveRefToString(g_end);
		g_end = 0;
	}
}

void XAnimAbort()
{
	g_end = 0;
}

void XAnimFree(XAnimParts *parts)
{
	uint16_t *names;
	int i;
	int boneCount;
	XAnimNotifyInfo *notify;
	short count;

	names = parts->names;
	boneCount = parts->boneCount;

	for ( i = 0; i < boneCount; ++i )
		SL_RemoveRefToString(names[i]);

	count = 0;
	notify = parts->notify;

	while ( count < parts->notifyCount )
	{
		SL_RemoveRefToString(notify->name);
		++count;
		++notify;
	}
}

XAnimParts* XAnimFindData(const char *name)
{
	return (XAnimParts *)Hunk_FindDataForFile(FILEDATA_XANIM, name);
}

XAnimParts* XAnimLoadDefaultAnim(XAnimParts *animParts, void *(*Alloc)(int))
{
	uint16_t *names;
	int i;
	int bones;
	XAnimNotifyInfo *notify;
	short count;
	XAnimParts *anim;

	anim = (XAnimParts *)Alloc(sizeof(XAnimParts));
	*anim = *animParts;
	names = anim->names;
	bones = anim->boneCount;

	for ( i = 0; i < bones; ++i )
		SL_AddRefToString(names[i]);

	count = 0;
	notify = anim->notify;

	while ( count < anim->notifyCount )
	{
		SL_AddRefToString(notify->name);
		++count;
		++notify;
	}

	return anim;
}

XAnimParts* XAnimPrecache(const char *name, void *(*Alloc)(int))
{
	XAnimParts *anim;
	XAnimParts *defaultAnim;

	anim = XAnimFindData(name);

	if ( anim )
		return anim;

	anim = XAnimLoadFile(name, Alloc);

	if ( !anim )
	{
		Com_Printf("^3WARNING: Couldn't find xanim '%s', using default xanim '%s' instead\n", name, "void");
		defaultAnim = XAnimFindData("void");

		if ( !defaultAnim )
		{
			defaultAnim = XAnimLoadFile("void", Alloc);

			if ( !defaultAnim )
			{
				Com_Error(ERR_DROP, "\x15" "Cannot find xanim '%s'.", "void");
				return NULL;
			}

			Hunk_SetDataForFile(FILEDATA_XANIM, "void", defaultAnim, Alloc);
		}

		anim = XAnimLoadDefaultAnim(defaultAnim, Alloc);
		anim->isDefault = 1;
	}

	anim->name = Hunk_SetDataForFile(FILEDATA_XANIM, name, anim, (void *(*)(int))Alloc);
	return anim;
}

void XAnimCreate(XAnim_s *anims, unsigned int animIndex, const char *name)
{
	XAnimParts_s *parts;
	XAnimEntry *entry;
	char *dest;

	parts = XAnimFindData(name);

	if ( !parts )
	{
		Com_Error(ERR_DROP, "\x15" "Cannot find xanim '%s'", name);
	}
	else
	{
		entry = &anims->entries[animIndex];
		entry->numAnims = 0;
		entry->u.parts = parts;

		if ( anims->debugAnimNames )
		{
			dest = (char *)Z_MallocInternal(strlen(name) + 1);
			strcpy(dest, name);
			anims->debugAnimNames[animIndex] = dest;
		}
	}
}

void XAnimBlend(XAnim_s *anims, unsigned int animIndex,const char *name, unsigned int children, unsigned int num, unsigned int flags)
{
	unsigned int i;
	XAnimEntry *entry;
	char *dest;

	entry = &anims->entries[animIndex];
	entry->numAnims = num;
	entry->u.animParent.flags = flags;
	entry->u.animParent.children = children;

	for ( i = 0; i < num; ++i )
		anims->entries[i + entry->u.animParent.children].parent = animIndex;

	if ( anims->debugAnimNames )
	{
		dest = (char *)Z_MallocInternal(strlen(name) + 1);
		strcpy(dest, name);
		anims->debugAnimNames[animIndex] = dest;
	}
}

XAnim_s* XAnimCreateAnims(const char *debugName, int size, void *(*Alloc)(int))
{
	XAnim_s *newAnim;
	char *dest;
	size_t allocSize;

	allocSize = sizeof(XAnimEntry) * size + sizeof(XAnimTree_s);
	newAnim = (XAnim_s *)Alloc(allocSize);
	newAnim->size = size;

	if ( g_anim_developer )
	{
		dest = (char *)Z_MallocInternal(strlen(debugName) + 1);
		strcpy(dest, debugName);
		newAnim->debugName = dest;
		newAnim->debugAnimNames = (const char **)Z_MallocInternal(sizeof(intptr_t) * size);
	}

	if ( Hunk_DataOnHunk(newAnim) )
		Hunk_AddData(FILEDATA_XANIMLIST, newAnim, Alloc);

	return newAnim;
}

void XAnimFreeAnims( XAnim_s *anims, void (*Free)(void *, int) )
{
	int size;

	size = 8 * anims->size + 12;
	XAnimFreeList(anims);
	Free(anims, size);
}

void XAnimFreeList(XAnim_s *anims)
{
	unsigned int i;

	if ( anims->debugName )
	{
		Z_FreeInternal((void *)anims->debugName);
		anims->debugName = 0;
	}

	if ( anims->debugAnimNames )
	{
		for ( i = 0; i < anims->size; ++i )
		{
			if ( !anims->debugAnimNames[i] )
			{
				continue;
			}

			Z_FreeInternal((void *)anims->debugAnimNames[i]);
			anims->debugAnimNames[i] = 0;
		}

		Z_FreeInternal(anims->debugAnimNames);
		anims->debugAnimNames = 0;
	}
}

// unreferenced
void XAnimSetUnusedFlag()
{
	xanimUnusedFlag = 1;
}

int XAnimTreeSize(int size)
{
	register int total;

	total = size + size;
	total = size + 2 * total;
	total = total + 9;

	return total;
}

XAnimTree_s* XAnimCreateTree(XAnim_s *anims, void *(*Alloc)(int))
{
	XAnimTree_s *tree;
	int size;
	int animsSize;

	animsSize = anims->size;
	size = XAnimTreeSize(animsSize);
	tree = (XAnimTree_s *)Alloc(size);
	memset(tree, 0, size);
	tree->anims = anims;

	return tree;
}

void XAnimFreeTree( XAnimTree_s *tree, void (*Free)(void *, int) )
{
	int size;
	int treeSize;

	treeSize = tree->anims->size;
	XAnimClearTree(tree);

	if ( !Free )
		return;

	size = XAnimTreeSize(treeSize);
	Free(tree, size);
}

XAnim_s* XAnimGetAnims(const XAnimTree_s *tree)
{
	return tree->anims;
}

unsigned int XAnimSetModel(const XAnimEntry *animEntry, XModel *const *model, int numModels)
{
	XAnimToXModel animToModel;
	int j;
	int boneIndex;
	int m;
	XAnimParts_s *parts;
	int boneCount;
	int childNumBones;
	uint16_t *names;
	unsigned short *childBoneNames;
	int childBone;
	int len;
	int i;
	unsigned int result;
	XBoneHierarchy *hierarchy;
	XModelParts_s *childPart;

	parts = animEntry->u.parts;
	names = parts->names;
	boneCount = parts->boneCount;
	len = boneCount + 16;

	for ( i = 0; i < 4; ++i )
		animToModel.partBits[i] = 0;

	for ( j = boneCount - 1; j >= 0; --j )
		animToModel.boneIndex[j] = 127;

	boneIndex = 0;

	for ( i = 0; i < numModels; ++i )
	{
		childPart = model[i]->parts;
		hierarchy = childPart->hierarchy;
		childBoneNames = hierarchy->names;
		childNumBones = childPart->numBones;

		for ( m = 0; m < childNumBones; ++m, ++boneIndex )
		{
			childBone = childBoneNames[m];

			for ( j = boneCount - 1; j >= 0; --j )
			{
				if ( childBone == names[j] )
				{
					if ( animToModel.boneIndex[j] != 127 )
						break;

					animToModel.boneIndex[j] = boneIndex;
					animToModel.partBits[boneIndex >> 5] |= 1 << (boneIndex & 0x1F);
					break;
				}
			}
		}
	}

	result = SL_GetStringOfLen((const char *)&animToModel.partBits[0], 0, len, 11);
	return result;
}

void XAnim_SetTime(float time, int frameCount, XAnimTime *animTime)
{
	animTime->time = time;
	animTime->frameCount = frameCount;
	animTime->frameFrac = time * frameCount;
	animTime->frameIndex = (int)animTime->frameFrac;
}

void XAnim_CalcSimpleRotEnd(const XAnimPartQuat *quat, float scale, float *rotDelta)
{
	const short *frame;

	frame = !quat->size ? quat->u.frame0 : quat->u.frames.u.frames2[quat->size];
	Vec2MadShort2(rotDelta, scale, frame, rotDelta);
}

void XAnim_CalcRotEnd( const XAnimPartQuat *quat, float scale, float *rotDelta )
{
	if ( !quat->size )
		Vec4MadShort4(rotDelta, scale, quat->u.frame0, rotDelta);
	else
		Vec4MadShort4(rotDelta, scale, quat->u.frames.u.frames[quat->size], rotDelta);
}

void XAnim_CalcPosEnd( const XAnimPartTrans *trans, float scale, float *posDelta )
{
	if ( !trans->size )
		VectorMA(posDelta, scale, trans->u.frame0, posDelta);
	else
		VectorMA(posDelta, scale, trans->u.frames.frames[trans->size], posDelta);
}

void XAnimCalcNonLoopEnd( const XAnimParts_s *parts, const unsigned char *animToModel, float weightScale, DObjAnimMat *rotTransArray, int *ignorePartBits )
{
	DObjAnimMat *totalRotTrans;
	int i;
	XAnimPart *childPart;
	float scale;
	int animPartIndex;
	int boneCount;
	char *simpleQuatBits;

	scale = weightScale * 0.000030518509f;
	simpleQuatBits = parts->simpleQuatBits;
	boneCount = parts->boneCount;

	for ( i = 0; i < boneCount; ++i )
	{
		animPartIndex = animToModel[i];

		if ( ( 1 << (animPartIndex & 0x1F) ) & ignorePartBits[animPartIndex >> 5] )
			continue;

		totalRotTrans = &rotTransArray[animPartIndex];
		childPart = &parts->parts[i];

		if ( ( 1 << (i & 7) ) & simpleQuatBits[i >> 3] )
		{
			if ( childPart->quat )
				XAnim_CalcSimpleRotEnd(childPart->quat, scale, &totalRotTrans->quat[2]);
			else
				totalRotTrans->quat[3] = totalRotTrans->quat[3] + weightScale;
		}
		else
		{
			XAnim_CalcRotEnd(childPart->quat, scale, totalRotTrans->quat);
		}

		if ( childPart->trans )
			XAnim_CalcPosEnd(childPart->trans, weightScale, totalRotTrans->trans);

		totalRotTrans->transWeight = totalRotTrans->transWeight + weightScale;
	}
}

void XAnimClearData(const DObj_s *obj, DObjAnimMat *rotTransArray, XAnimCalcAnimInfo *info)
{
	int i;

	for ( i = 0; i < obj->numBones; ++i, ++rotTransArray )
	{
		if ( (bool)( ( 1 << (i & 0x1F) ) & info->ignorePartBits[i >> 5] ) )
		{
		}
		else
		{
			Vector4Clear(rotTransArray->quat);
			rotTransArray->transWeight = 0.0;
			VectorClear(rotTransArray->trans);
		}
	}
}

void XAnim_CalcRotDeltaEntireInternal(XAnimDeltaPartQuat *quat, float *rotDelta)
{
	const short *rotDeltaLastFrame;

	rotDeltaLastFrame = !quat->size ? quat->u.frame0 : quat->u.frames.frames[quat->size];

	Short2CopyAsVec2(rotDeltaLastFrame, rotDelta);
}

void XAnim_CalcRotDeltaEntire(const XAnimDeltaPart *animDelta, float *rotDelta)
{
	if ( animDelta->quat )
	{
		XAnim_CalcRotDeltaEntireInternal(animDelta->quat, rotDelta);
	}
	else
	{
		rotDelta[0] = 0.0;
		rotDelta[1] = 32767.0;
	}
}

void XAnim_CalcPosDeltaEntireInternal(const XAnimDeltaPartTrans *trans, float *posDelta)
{
	if ( !trans->size )
		VectorCopy(trans->u.frame0, posDelta);
	else
		VectorCopy(trans->u.frames.frames[trans->size], posDelta);
}

void XAnim_CalcPosDeltaEntire(const XAnimDeltaPart *animDelta, float *posDelta)
{
	if ( animDelta->trans )
		XAnim_CalcPosDeltaEntireInternal(animDelta->trans, posDelta);
	else
		VectorClear(posDelta);
}

void XAnim_CalcDeltaForTime(const XAnimParts_s *anim, const float time, float *rotDelta, float *posDelta)
{
	int numFrames;
	bool isComplete;
	XAnimDeltaPart *animDelta;

	animDelta = anim->deltaPart;
	numFrames = anim->numframes;
	isComplete = time == 1.0f || !numFrames;

	if ( isComplete )
	{
		XAnim_CalcRotDeltaEntire(animDelta, rotDelta);
		XAnim_CalcPosDeltaEntire(animDelta, posDelta);
	}
	else if ( numFrames < 0x100 )
	{
		XAnim_CalcRotDeltaDuring_unsigned_char_(animDelta, time, numFrames, rotDelta);
		XAnim_CalcPosDeltaDuring_unsigned_char_(animDelta, time, numFrames, posDelta);
	}
	else
	{
		XAnim_CalcRotDeltaDuring_unsigned_short_(animDelta, time, numFrames, rotDelta);
		XAnim_CalcPosDeltaDuring_unsigned_short_(animDelta, time, numFrames, posDelta);
	}
}

void TransformToQuatRefFrame( const float *rot, float *trans )
{
	float zz;
	float zw;
	float r;
	float temp;

	zz = rot[0] * rot[0];
	r = zz + rot[1] * rot[1];

	if ( r == 0 )
		return;

	r = 2.0f / r;
	zz = zz * r;
	zw = rot[0] * rot[1] * r;
	temp = (1 - zz) * trans[0] + zw * trans[1];
	trans[1] = trans[1] - (zw * trans[0] + zz * trans[1]);
	trans[0] = temp;
}

void XAnimCalcRelDeltaParts(const XAnimParts_s *parts, float weightScale, float time1, float time2, XAnimSimpleRotPos *rotPos, int quatIndex)
{
	float Q[2][2];
	vec3_t vec1;
	vec3_t vec2;
	vec3_t pos;
	float rotWeightScale;
	XAnimDeltaPart *deltaPart;
	XAnimDeltaPartTrans *trans;

	XAnim_CalcDeltaForTime(parts, time1, Q[0], vec1);
	XAnim_CalcDeltaForTime(parts, time2, Q[1], vec2);

	if ( parts->bLoop )
	{
		if ( time2 < time1 )
		{
			deltaPart = parts->deltaPart;
			trans = deltaPart->trans;

			if ( trans )
			{
				if ( trans->size )
				{
					VectorAdd(vec2, trans->u.frames.frames[trans->size], vec2);
					VectorSubtract(vec2, trans->u.frames.frames[0], vec2);
				}
			}
		}
	}

	rotWeightScale = weightScale * 9.3137942e-10f;
	rotPos->rot[0] = rotPos->rot[0] + (Q[1][0] * Q[0][1] - Q[1][1] * Q[0][0]) * rotWeightScale;
	rotPos->rot[1] = rotPos->rot[1] + (Q[1][0] * Q[0][0] + Q[1][1] * Q[0][1]) * rotWeightScale;
	VectorSubtract(vec2, vec1, pos);
	TransformToQuatRefFrame(Q[quatIndex], pos);
	rotPos->posWeight = rotPos->posWeight + weightScale;
	VectorMA(rotPos->pos, weightScale, pos, rotPos->pos);
}

void XAnimCalcAbsDeltaParts(const XAnimParts_s *parts, const float weightScale, float time, XAnimSimpleRotPos *rotPos)
{
	float Q[2];
	vec3_t pos;

	XAnim_CalcDeltaForTime(parts, time, Q, pos);
	VectorMA2(rotPos->rot, weightScale * 0.000030518509f, Q, rotPos->rot);
	rotPos->posWeight = rotPos->posWeight + weightScale;
	VectorMA(rotPos->pos, weightScale, pos, rotPos->pos);
}

void XAnimFreeNotifyStrings(XAnimInfo *info)
{
	if ( info->notifyName )
	{
		SL_RemoveRefToString(info->notifyName);
		info->notifyName = 0;
	}

	info->notifyIndex = -1;
}

void XAnimFreeInfo(XAnimTree_s *tree, unsigned int infoIndex)
{
	XAnimInfo *info;

	info = &g_xAnimInfo[infoIndex];
	XAnimFreeNotifyStrings(info);
	info->prev = 0;
	info->next = g_xAnimInfo->next;
	g_xAnimInfo[g_xAnimInfo->next].prev = infoIndex;
	g_xAnimInfo->next = infoIndex;
}

float XAnimGetAverageRateFrequency(const XAnimTree_s *tree, unsigned int infoIndex)
{
	int numAnims;
	int i;
	uint16_t index;
	XAnimInfo *info;
	float weight;
	float trans;
	float rate;
	XAnimEntry *entry;
	XAnimParts *parts;
	float freq;

	entry = &tree->anims->entries[infoIndex];
	numAnims = entry->numAnims;

	if ( !numAnims )
	{
		parts = entry->u.parts;
		return parts->frequency;
	}

	trans = 0.0f;
	rate = 0.0f;

	for ( i = 0; i < numAnims; ++i )
	{
		index = tree->infoArray[i + entry->u.animParent.children];

		if ( !index )
		{
			continue;
		}

		info = &g_xAnimInfo[index];
		weight = info->state.weight;

		if ( weight == 0.0f )
		{
			continue;
		}

		freq = XAnimGetAverageRateFrequency(tree, i + entry->u.animParent.children);

		if ( freq == 0.0f )
		{
			continue;
		}

		trans = trans + weight;
		rate = freq * weight * info->state.rate + rate;
	}

	if ( trans != 0.0f )
	{
		return rate / trans;
	}

	return 0.0f;
}

unsigned short XAnimGetNextNotifyIndex( const XAnimEntry *entry, float dtime )
{
	XAnimParts *parts;
	XAnimNotifyInfo *partsNotify;
	float partsTime;
	float time;
	XAnimNotifyInfo *animNotify;
	int i;

	parts = entry->u.parts;
	animNotify = 0;
	time = 2.0;
	partsNotify = parts->notify;

	for ( i = 0; i < parts->notifyCount; i++, partsNotify++ )
	{
		partsTime = partsNotify->time;

		if ( partsTime < dtime )
			continue;

		if ( partsTime < time )
		{
			time = partsTime;
			animNotify = partsNotify;
		}
	}

	return animNotify - parts->notify;
}

float XAnimGetNotifyFracLeaf(const XAnimState *state, const XAnimState *nextState, float time, float dtime)
{
	if ( nextState->oldTime == 1.0f )
		return 1.0;

	if ( nextState->time < nextState->oldTime )
	{
		if ( nextState->time > time )
			return ((float)(nextState->oldCycleCount - state->oldCycleCount + 1) + (time - state->oldTime)) / dtime;

		if ( nextState->oldTime <= time )
			return ((float)(nextState->oldCycleCount - state->oldCycleCount) + (time - state->oldTime)) / dtime;

		return 1.0;
	}

	if ( (nextState->time > time || nextState->time == 1.0f) && nextState->oldTime <= time )
		return ((float)(nextState->oldCycleCount - state->oldCycleCount) + (time - state->oldTime)) / dtime;

	return 1.0;
}

float XAnimGetNotifyFracServer(const XAnimTree_s *tree, XAnimInfo *info, const XAnimEntry *entry, const XAnimState *state, const XAnimState *nextState, float dtime)
{
	XAnimParts *parts;

	if ( !tree->entnum || !info->notifyName )
	{
		return 1.0;
	}

	if ( entry->numAnims )
	{
		if ( !info->notifyChild )
		{
			return XAnimGetNotifyFracLeaf(state, nextState, 1.0, dtime);
		}

		entry = &tree->anims->entries[info->notifyChild];
	}

	parts = entry->u.parts;

	if ( info->notifyIndex < 0 )
	{
		XAnimSetNotifyIndex(tree, info, entry);

		if ( info->notifyIndex < 0 )
		{
			return XAnimGetNotifyFracLeaf(state, nextState, 1.0, dtime);
		}
	}

	return XAnimGetNotifyFracLeaf(state, nextState, parts->notify[info->notifyIndex].time, dtime);
}

void XAnimAddClientNotify( const XAnimEntry *entry, unsigned int notetrackName, float frac, unsigned int notifyType )
{
	XAnimClientNotify *notify;
	int i;

	for ( i = g_notifyListSize - 1; i >= 0; i-- )
	{
		notify = &g_notifyList[i];

		if ( frac >= notify->timeFrac )
			break;

		notify[1] = notify[0];
	}

	notify = &g_notifyList[i + 1];
	notify->name = SL_ConvertToString(notetrackName);
	notify->timeFrac = frac;
	notify->notifyName = notifyType;

	g_notifyListSize++;
}

void XAnimProcessClientNotify(XAnimInfo *info, const XAnimEntry *entry, float dtime)
{
	XAnimParts *parts;
	XAnimNotifyInfo *noteTrack;
	unsigned short nextNotify;
	const XAnimState *state;
	uint16_t type;

	state = &info->state;
	type = info->notifyType;

	if ( !type )
	{
		return;
	}

	{
		if ( state->oldTime == 1.0f )
		{
			XAnimAddClientNotify(entry, g_end, XAnimGetNotifyFracLeaf(state, state, 1.0, dtime), type);
			return;
		}
		if ( entry->numAnims )
		{
			if ( state->time < state->oldTime || state->time == 1.0f )
			{
				XAnimAddClientNotify(entry, g_end, XAnimGetNotifyFracLeaf(state, state, 1.0, dtime), type);
				return;
			}
		}
		else
		{
			parts = entry->u.parts;
			nextNotify = XAnimGetNextNotifyIndex(entry, state->oldTime);
			noteTrack = &parts->notify[nextNotify];
			if ( state->time < state->oldTime )
			{
				if ( state->time > noteTrack->time )
				{
					do
					{
						XAnimAddClientNotify(entry, noteTrack->name, XAnimGetNotifyFracLeaf(state, state, noteTrack->time, dtime), type);
						++noteTrack;
						++nextNotify;
					}
					while ( nextNotify < (unsigned int)parts->notifyCount && state->time > noteTrack->time );
				}
				else
				{
					if ( state->oldTime > noteTrack->time )
						return;

					do
					{
						XAnimAddClientNotify(entry, noteTrack->name, XAnimGetNotifyFracLeaf(state, state, noteTrack->time, dtime), type);
						++noteTrack;
						++nextNotify;
					}
					while ( nextNotify < (unsigned int)parts->notifyCount );
					for ( noteTrack = parts->notify; state->time > noteTrack->time; ++noteTrack, ++noteTrack )
					{
						XAnimAddClientNotify(entry, noteTrack->name, XAnimGetNotifyFracLeaf(state, state, noteTrack->time, dtime), type);
					}
				}
			}
			else
			{
				if ( state->time == 1.0f )
				{
					if ( state->oldTime > noteTrack->time )
						return;

					do
					{
						XAnimAddClientNotify(entry, noteTrack->name, XAnimGetNotifyFracLeaf(state, state, noteTrack->time, dtime), type);
						++noteTrack;
						++nextNotify;
					}
					while ( nextNotify < (unsigned int)parts->notifyCount );
				}
				else if ( !(state->time <= noteTrack->time) )
				{
					if ( state->oldTime > noteTrack->time )
						return;

					do
					{
						XAnimAddClientNotify(entry, noteTrack->name, XAnimGetNotifyFracLeaf(state, state, noteTrack->time, dtime), type);
						++noteTrack;
						++nextNotify;
					}
					while ( nextNotify < (unsigned int)parts->notifyCount && state->time > noteTrack->time );
				}
			}
		}
	}
}

void XAnimUpdateInfoSyncInternal(const XAnimTree_s *tree, unsigned int index, bool update, XAnimState *state, float dtime)
{
	int i;
	int numAnims;
	uint16_t childIndex;
	XAnimInfo *info;
	XAnimState *infoState;
	XAnimEntry *entry;

	childIndex = tree->infoArray[index];

	if ( !childIndex )
	{
	}
	else
	{
		info = &g_xAnimInfo[childIndex];
		infoState = &info->state;

		if ( infoState->weight == 0.0f )
		{
		}
		else
		{
			if ( infoState->goalWeight == 0.0f )
				update = 0;

			entry = &tree->anims->entries[index];

			if ( infoState->oldTime != state->oldTime || infoState->oldCycleCount != state->oldCycleCount )
			{
				infoState->time = state->oldTime;
				infoState->cycleCount = state->oldCycleCount;
				infoState->oldTime = state->oldTime;
				infoState->oldCycleCount = state->oldCycleCount;
				info->notifyIndex = -1;
			}

			if ( update )
				XAnimProcessServerNotify(tree, info, entry, state->time);

			infoState->time = state->time;
			infoState->cycleCount = state->cycleCount;
			info->notifyIndex = -1;

			if ( update )
				XAnimProcessClientNotify(info, entry, dtime);

			numAnims = entry->numAnims;

			for ( i = 0; i < numAnims; ++i )
				XAnimUpdateInfoSyncInternal(tree, i + entry->u.animParent.children, update, state, dtime);
		}
	}
}

void XAnimUpdateInfoInternal(XAnimTree_s *tree, unsigned int infoIndex, float dtime, bool update)
{
	int i;
	int numAnims;
	uint16_t index;
	XAnimInfo *info;
	XAnimState *state;
	XAnimEntry *entry;
	XAnimParts *parts;
	float frameTime;
	int16_t newCycleCount;

	index = tree->infoArray[infoIndex];

	if ( !index )
		return;

	info = &g_xAnimInfo[index];
	state = &info->state;

	if ( state->weight == 0.0f )
		return;

	if ( state->goalWeight == 0.0f )
		update = 0;

	entry = &tree->anims->entries[infoIndex];
	numAnims = entry->numAnims;

	if ( !numAnims )
	{
		parts = entry->u.parts;
		dtime = state->rate * parts->frequency * dtime;

		if ( dtime == 0.0f )
			return;

		frameTime = state->oldTime + dtime;
		newCycleCount = state->cycleCount;

		if ( frameTime >= 1.0f )
		{
			if ( !parts->bLoop )
			{
				frameTime = 1.0f;
			}
			else
			{
				do
				{
					frameTime = frameTime - 1.0f;
					++newCycleCount;
				}
				while ( frameTime >= 1.0f );
			}
		}

		if ( (float)(newCycleCount - state->cycleCount) < state->time - frameTime )
			return;

		if ( update )
			XAnimProcessServerNotify(tree, info, entry, frameTime);

		state->time = frameTime;
		state->cycleCount = newCycleCount;
		info->notifyIndex = -1;

		if ( update )
			XAnimProcessClientNotify(info, entry, dtime);

		return;
	}

	if ( (entry->u.animParent.flags & 3) != 0 )
	{
		dtime = XAnimGetAverageRateFrequency(tree, infoIndex) * state->rate * dtime;

		if ( dtime == 0.0f )
			return;

		frameTime = state->oldTime + dtime;
		newCycleCount = state->oldCycleCount;

		if ( frameTime >= 1.0f )
		{
			if ( (entry->u.animParent.flags & 2) != 0 )
			{
				frameTime = 1.0f;
			}
			else
			{
				do
				{
					frameTime = frameTime - 1.0f;
					++newCycleCount;
				}
				while ( frameTime >= 1.0f );
			}
		}

		if ( (float)(newCycleCount - state->cycleCount) < state->time - frameTime )
			return;

		if ( update )
			XAnimProcessServerNotify(tree, info, entry, frameTime);

		state->time = frameTime;
		state->cycleCount = newCycleCount;
		info->notifyIndex = -1;

		if ( update )
			XAnimProcessClientNotify(info, entry, dtime);

		for ( i = 0; i < numAnims; ++i )
			XAnimUpdateInfoSyncInternal(tree, i + entry->u.animParent.children, update, state, dtime);

		return;
	}

	dtime = dtime * state->rate;

	if ( dtime == 0.0f )
		return;

	for ( i = 0; i < numAnims; ++i )
		XAnimUpdateInfoInternal(tree, i + entry->u.animParent.children, dtime, update);
}

void XAnimResetInfo(XAnimInfo *info)
{
	memset(&info->state, 0, sizeof(info->state));
	info->notifyName = 0;
	info->notifyIndex = -1;
	info->notifyChild = 0;
	info->notifyType = 0;
}

void XAnimCloneAnimTreeOldState( const XAnimTree_s *from, XAnimTree_s *to )
{
	int infoIndex;
	int index;
	XAnimInfo *info;
	int size;
	const XAnimState *fromState;
	XAnimState *toState;

	size = from->anims->size;

	for ( infoIndex = 0; infoIndex < size; infoIndex++ )
	{
		index = from->infoArray[infoIndex];

		if ( !index )
		{
			index = to->infoArray[infoIndex];

			if ( index )
			{
				XAnimFreeInfo(to, index);
				to->infoArray[infoIndex] = 0;
			}
		}
		else
		{
			fromState = &g_xAnimInfo[index].state;

			if ( !to->infoArray[infoIndex] )
			{
				info = XAnimGetInfo(to, infoIndex);
				XAnimResetInfo(info);
			}
			else
			{
				info = &g_xAnimInfo[to->infoArray[infoIndex]];
			}

			toState = &info->state;
			toState->time = fromState->oldTime;
			toState->cycleCount = fromState->oldCycleCount;
			toState->goalWeight = fromState->goalWeight;
			toState->rate = fromState->rate;
			toState->goalTime = fromState->goalTime;
		}
	}
}

bool XAnimNeedClearState(XAnimInfo *info)
{
	XAnimState *state;

	state = &info->state;

	if ( state->time == 0.0f && !state->cycleCount )
		return false;

	state->time = 0.0;
	state->cycleCount = 0;
	state->oldTime = 0.0;
	state->oldCycleCount = 0;
	info->notifyIndex = -1;

	return true;
}

void XAnimResetTime(const XAnimTree_s *tree, unsigned int animIndex)
{
	int i;
	int numAnims;
	uint16_t childIndex;
	XAnimEntry *entry;

	childIndex = tree->infoArray[animIndex];

	if ( !childIndex )
	{
		return;
	}

	XAnimNeedClearState(&g_xAnimInfo[childIndex]);
	entry = &tree->anims->entries[animIndex];
	numAnims = entry->numAnims;

	for ( i = 0; i < numAnims; ++i )
		XAnimResetTime(tree, i + entry->u.animParent.children);
}

void XAnimUpdateOldTime(XAnimTree_s *tree, unsigned int infoIndex, XAnimState *syncState, float dtime, bool parentHasWeight, bool *childHasTimeForParent1, bool *childHasTimeForParent2)
{
	bool parentHasTime;
	int i;
	int numAnims;
	int index;
	XAnimInfo *info;
	XAnimState *state;
	XAnimEntry *entry;
	bool childHasWeight;
	bool childHasTime;
	XAnimParts *parts;
	bool bWeight;
	bool bTime;

	index = tree->infoArray[infoIndex];

	if ( !index )
	{
		return;
	}

	{
		info = &g_xAnimInfo[index];
		state = &info->state;
		if ( parentHasWeight && state->weight != 0.0f )
			bWeight = 1;
		else
			bWeight = 0;

		childHasWeight = bWeight;

		if ( !parentHasWeight || state->goalTime <= dtime )
		{
			state->weight = state->goalWeight;
			state->goalTime = 0.0f;
		}
		else
		{
			state->weight += (state->goalWeight - state->weight) * dtime / state->goalTime;

			if ( state->weight < 0.0000010000001f )
				state->weight = state->goalWeight * 0.001f;

			state->goalTime = state->goalTime - dtime;
		}

		if ( childHasWeight || state->goalWeight != 0.0f )
			parentHasTime = 1;
		else
			parentHasTime = 0;
		entry = &tree->anims->entries[infoIndex];
		numAnims = entry->numAnims;

		if ( !numAnims )
		{
			parts = entry->u.parts;
			childHasTime = parts->frequency != 0.0f;
		}
		else
		{
			childHasTime = 0;

			if ( (entry->u.animParent.flags & 4) != 0 )
				syncState = state;

			for ( i = 0; i < numAnims; ++i )
				XAnimUpdateOldTime(
				    tree,
				    i + entry->u.animParent.children,
				    syncState,
				    dtime,
				    childHasWeight,
				    &parentHasTime,
				    &childHasTime);
		}
		if ( !parentHasTime )
		{
			XAnimFreeInfo(tree, index);
			tree->infoArray[infoIndex] = 0;
		}
		else
		{
			if ( !childHasWeight || !childHasTime )
			{
				if ( numAnims && (entry->u.animParent.flags & 4) != 0 )
				{
					if ( XAnimNeedClearState(info) )
					{
						for ( i = 0; i < numAnims; ++i )
							XAnimResetTime(tree, i + entry->u.animParent.children);
					}
				}
				else if ( state->time != syncState->time || state->cycleCount != syncState->cycleCount )
				{
					state->time = syncState->time;
					state->cycleCount = syncState->cycleCount;
					info->notifyIndex = -1;
				}
			}
			else
			{
				*childHasTimeForParent2 = 1;
			}

			state->oldTime = state->time;
			state->oldCycleCount = state->cycleCount;
			*childHasTimeForParent1 = 1;
		}
	}
}

void XAnimAddServerNotifyNamed(const XAnimTree_s *tree, unsigned int notifyName, unsigned int name)
{
	Scr_AddConstString(name);
	Scr_NotifyNum(tree->entnum - 1, 0, notifyName, 1u);
}

float XAnimGetServerNotifyFracSyncTotal(const XAnimTree_s *tree, XAnimInfo *info, const XAnimEntry *entry, const XAnimState *state, const XAnimState *nextState, float dtime)
{
	float notifyFrac;
	int i;
	float totalFrac;
	int childIndex;
	uint16_t animIndex;

	notifyFrac = XAnimGetNotifyFracServer(tree, info, entry, state, nextState, dtime);

	for ( i = 0; i < entry->numAnims; ++i )
	{
		childIndex = i + entry->u.animParent.children;
		animIndex = tree->infoArray[childIndex];

		if ( !animIndex )
		{
			continue;
		}

		info = &g_xAnimInfo[animIndex];

		if ( info->state.weight == 0.0f || info->state.goalWeight == 0.0f )
		{
			continue;
		}

		totalFrac = XAnimGetServerNotifyFracSyncTotal(
		                tree,
		                info,
		                &tree->anims->entries[childIndex],
		                state,
		                nextState,
		                dtime);

		if ( totalFrac < notifyFrac )
			notifyFrac = totalFrac;
	}

	return notifyFrac;
}

float XAnimFindServerNoteTrack(const XAnimTree_s *anim, unsigned int infoIndex, float dtime)
{
	int i;
	int numAnims;
	unsigned short childIndex;
	XAnimInfo *info;
	XAnimState *animState;
	XAnimEntry *animEntry;
	XAnimParts *animParts;
	float newAnimTime;
	int16_t newCycleCount;
	float noteTrack;
	float rate;
	XAnimState localState;

	childIndex = anim->infoArray[infoIndex];

	if ( !childIndex )
		return 1.0f;

	info = &g_xAnimInfo[childIndex];
	animState = &info->state;

	if ( animState->weight == 0.0f || animState->goalWeight == 0.0f )
		return 1.0f;

	animEntry = &anim->anims->entries[infoIndex];
	numAnims = animEntry->numAnims;

	if ( !numAnims )
	{
		animParts = animEntry->u.parts;
		dtime = animState->rate * animParts->frequency * dtime;

		if ( dtime == 0.0f )
			return 1.0f;

		newAnimTime = animState->oldTime + dtime;
		newCycleCount = animState->oldCycleCount;

		if ( !animParts->bLoop )
		{
			if ( newAnimTime >= 1.0f )
				newAnimTime = 1.0f;
		}
		else
		{
			while ( newAnimTime >= 1.0f )
			{
				newAnimTime = newAnimTime - 1.0f;
				++newCycleCount;
			}
		}

		if ( (float)(newCycleCount - animState->cycleCount) < animState->time - newAnimTime )
			return 1.0f;

		localState.oldTime = animState->time;
		localState.oldCycleCount = animState->cycleCount;
		localState.time = newAnimTime;
		localState.cycleCount = newCycleCount;

		return XAnimGetNotifyFracServer(anim, info, animEntry, animState, &localState, dtime);
	}

	if ( (animEntry->u.animParent.flags & 3) != 0 )
	{
		dtime = XAnimGetAverageRateFrequency(anim, infoIndex) * animState->rate * dtime;

		if ( dtime == 0.0f )
			return 1.0f;

		newAnimTime = animState->oldTime + dtime;
		newCycleCount = animState->oldCycleCount;

		if ( (animEntry->u.animParent.flags & 2) != 0 )
		{
			if ( newAnimTime >= 1.0f )
				newAnimTime = 1.0f;
		}
		else
		{
			while ( newAnimTime >= 1.0f )
			{
				newAnimTime = newAnimTime - 1.0f;
				++newCycleCount;
			}
		}

		if ( (float)(newCycleCount - animState->cycleCount) < animState->time - newAnimTime )
			return 1.0f;

		localState.oldTime = animState->time;
		localState.oldCycleCount = animState->cycleCount;
		localState.time = newAnimTime;
		localState.cycleCount = newCycleCount;

		return XAnimGetServerNotifyFracSyncTotal(anim, info, animEntry, animState, &localState, dtime);
	}

	dtime = dtime * animState->rate;

	if ( dtime == 0.0f )
		return 1.0f;

	rate = 1.0f;

	for ( i = 0; i < numAnims; ++i )
	{
		noteTrack = XAnimFindServerNoteTrack(anim, i + animEntry->u.animParent.children, dtime);

		if ( noteTrack < rate )
			rate = noteTrack;
	}

	return rate;
}

void XAnimProcessServerNotify(const XAnimTree_s *tree, XAnimInfo *info, const XAnimEntry *entry, float dtime)
{
	XAnimParts *parts;
	XAnimNotifyInfo *notify;
	int notifyIndex;

	if ( !tree->entnum )
	{
		return;
	}

	if ( !info->notifyName )
	{
		return;
	}

	{
		if ( info->state.time == 1.0f )
		{
			Scr_AddConstString(g_end);
			Scr_NotifyNum(tree->entnum - 1, 0, info->notifyName, 1u);
			return;
		}

		if ( info->notifyIndex < 0 && (XAnimSetNotifyIndex(tree, info, entry), info->notifyIndex < 0) )
		{
			if ( dtime < info->state.time || dtime == 1.0f )
			{
				Scr_AddConstString(g_end);
				Scr_NotifyNum(tree->entnum - 1, 0, info->notifyName, 1u);
			}
		}
		else
		{
			if ( entry->numAnims )
				entry = &tree->anims->entries[info->notifyChild];

			parts = entry->u.parts;
			notifyIndex = info->notifyIndex;
			notify = &parts->notify[notifyIndex];

			if ( dtime < info->state.time )
			{
				if ( dtime > notify->time )
				{
					do
					{
						XAnimAddServerNotifyNamed(tree, info->notifyName, notify->name);
						++notify;
						++notifyIndex;
					}
					while ( notifyIndex < parts->notifyCount && dtime > notify->time );
				}
				else
				{
					if ( !(info->state.time > notify->time) )
					{
						do
						{
							XAnimAddServerNotifyNamed(tree, info->notifyName, notify->name);
							++notify;
							++notifyIndex;
						}
						while ( notifyIndex < parts->notifyCount );

						for ( notify = parts->notify; dtime > notify->time; ++notify )
							XAnimAddServerNotifyNamed(tree, info->notifyName, notify->name);
					}
				}
			}
			else
			{
				if ( dtime == 1.0f )
				{
					if ( !(info->state.time > notify->time) )
					{
						do
						{
							XAnimAddServerNotifyNamed(tree, info->notifyName, notify->name);
							++notify;
							++notifyIndex;
						}
						while ( notifyIndex < parts->notifyCount );
					}
				}
				else if ( !(dtime <= notify->time) )
				{
					{
						if ( !(info->state.time > notify->time) )
						{
							do
							{
								XAnimAddServerNotifyNamed(tree, info->notifyName, notify->name);
								++notify;
								++notifyIndex;
							}
							while ( notifyIndex < parts->notifyCount && dtime > notify->time );
						}
						}
					}
				}
		}
	}
}

void XAnimCalc(const DObj_s *obj, unsigned int entry, float weightScale, DObjAnimMat *rotTransArray, bool bClear, bool bNormQuat, XAnimCalcAnimInfo *animInfo, int bufferIndex)
{
	float secondWeight;
	float firstWeight;
	int i;
	int j;
	int numAnims;
	DObjAnimMat *calcBuffer;
	float scale;
	unsigned short infoIndex;
	XAnimEntry *animEntry;
	byte *modelCounters;
	XAnimTree_s *tree;
	XAnimToXModel *animToModel;
	XAnimParts_s *parts;
	float time;

	tree = obj->tree;
	animEntry = &tree->anims->entries[entry];
	numAnims = animEntry->numAnims;

	if ( !numAnims )
	{
		if ( bClear )
			XAnimClearData(obj, rotTransArray, animInfo);

		// per-anim model counters follow the animToModel table
		modelCounters = (byte *)&obj->animToModel[tree->anims->size];

		if ( !obj->animToModel[entry] )
		{
			modelCounters[entry + 1] = *modelCounters;
		}
		else
		{
			if ( modelCounters[entry + 1] == *modelCounters )
				goto modelSet;

			modelCounters[entry + 1] = *modelCounters;
			SL_RemoveRefToStringOfLen(obj->animToModel[entry], animEntry->u.parts->boneCount + 16);
		}

		obj->animToModel[entry] = XAnimSetModel(animEntry, obj->models, obj->numModels);

modelSet:

		animToModel = (XAnimToXModel *)SL_ConvertToString(obj->animToModel[entry]);

		for ( i = 0; i <= 3; ++i )
			animInfo->animPartBits[i] |= animToModel->partBits[i] & ~animInfo->ignorePartBits[i];

		parts = animEntry->u.parts;
		time = g_xAnimInfo[tree->infoArray[entry]].state.time;

		if ( time == 1.0f || !parts->numframes )
			XAnimCalcNonLoopEnd(parts, animToModel->boneIndex, weightScale, rotTransArray, animInfo->ignorePartBits);
		else if ( parts->numframes < 0x100 )
			XAnimCalcParts_unsigned_char_(parts, animToModel->boneIndex, time, weightScale, rotTransArray, animInfo->ignorePartBits);
		else
			XAnimCalcParts_unsigned_short_(parts, animToModel->boneIndex, time, weightScale, rotTransArray, animInfo->ignorePartBits);

		return;
	}

	for ( i = 0; i < numAnims; ++i )
	{
		infoIndex = tree->infoArray[i + animEntry->u.animParent.children];

		if ( !infoIndex )
			continue;

		firstWeight = g_xAnimInfo[infoIndex].state.weight;

		if ( firstWeight == 0.0f )
			continue;

		for ( j = i + 1; j < numAnims; ++j )
		{
			infoIndex = tree->infoArray[j + animEntry->u.animParent.children];

			if ( !infoIndex )
				continue;

			secondWeight = g_xAnimInfo[infoIndex].state.weight;

			if ( secondWeight == 0.0f )
				continue;

			if ( bClear )
			{
				calcBuffer = rotTransArray;
			}
			else
			{
				calcBuffer = &animInfo->rotTransArray[bufferIndex];
				bufferIndex += obj->numBones;

				if ( bufferIndex > 512 )
				{
					Com_Printf("MAX_CALC_ANIM_BUFFER exceeded\n");
					return;
				}
			}

			XAnimCalc(obj, i + animEntry->u.animParent.children, firstWeight, calcBuffer, 1, 1, animInfo, bufferIndex);
			XAnimCalc(obj, j + animEntry->u.animParent.children, secondWeight, calcBuffer, 0, 1, animInfo, bufferIndex);

			for ( ++j; j < numAnims; ++j )
			{
				infoIndex = tree->infoArray[j + animEntry->u.animParent.children];

				if ( !infoIndex )
					continue;

				secondWeight = g_xAnimInfo[infoIndex].state.weight;

				if ( secondWeight == 0.0f )
					continue;

				XAnimCalc(obj, j + animEntry->u.animParent.children, secondWeight, calcBuffer, 0, 1, animInfo, bufferIndex);
			}

			if ( !bNormQuat )
			{
				for ( i = 0; i < obj->numBones; ++i, ++rotTransArray )
				{
					if ( ( 1 << (i & 0x1F) ) & animInfo->ignorePartBits[i >> 5] )
						continue;

					if ( rotTransArray->transWeight != 0.0f )
					{
						scale = 1.0f / rotTransArray->transWeight;
						VectorScale4(rotTransArray->quat, scale, rotTransArray->quat);
						VectorScale(rotTransArray->trans, scale, rotTransArray->trans);
					}
				}
			}
			else if ( bClear )
			{
				for ( i = 0; i < obj->numBones; ++i, ++rotTransArray )
				{
					if ( ( 1 << (i & 0x1F) ) & animInfo->ignorePartBits[i >> 5] )
						continue;

					scale = Vec4LengthSq(rotTransArray->quat);

					if ( scale != 0.0f )
						VectorScale4(rotTransArray->quat, I_rsqrt(scale) * weightScale, rotTransArray->quat);

					if ( rotTransArray->transWeight != 0.0f )
					{
						VectorScale(rotTransArray->trans, weightScale / rotTransArray->transWeight, rotTransArray->trans);
						rotTransArray->transWeight = weightScale;
					}
				}
			}
			else
			{
				for ( i = 0; i < obj->numBones; ++i, ++rotTransArray, ++calcBuffer )
				{
					if ( ( 1 << (i & 0x1F) ) & animInfo->ignorePartBits[i >> 5] )
						continue;

					scale = Vec4LengthSq(calcBuffer->quat);

					if ( scale != 0.0f )
						VectorMA4(rotTransArray->quat, I_rsqrt(scale) * weightScale, calcBuffer->quat, rotTransArray->quat);

					if ( calcBuffer->transWeight != 0.0f )
					{
						VectorMA(rotTransArray->trans, weightScale / calcBuffer->transWeight, calcBuffer->trans, rotTransArray->trans);
						rotTransArray->transWeight = rotTransArray->transWeight + weightScale;
					}
				}
			}

			return;
		}

		XAnimCalc(obj, i + animEntry->u.animParent.children, weightScale, rotTransArray, bClear, bNormQuat, animInfo, bufferIndex);
		return;
	}

	if ( bClear )
		XAnimClearData(obj, rotTransArray, animInfo);
}

void XAnimDisplay(const XAnimTree_s *tree, unsigned int infoIndex, int depth)
{
	int i;
	int numAnims;
	unsigned short treeIndex;
	XAnimEntry *animEntry;
	XAnimInfo *anim;
	XAnimState *animState;
	XAnimParts *parts;
	float goalTime;
	float deltaTime;
	const char *AnimDebugName;
	const char *color;

	treeIndex = tree->infoArray[infoIndex];

	if ( !treeIndex )
	{
		return;
	}

	{
		animEntry = &tree->anims->entries[infoIndex];
		numAnims = animEntry->numAnims;
		anim = &g_xAnimInfo[treeIndex];
		animState = &anim->state;

		for ( i = 0; i < depth; ++i )
			Com_Printf(" ");

		AnimDebugName = XAnimGetAnimDebugName(tree->anims, infoIndex);

		if ( animState->weight < animState->goalWeight )
			color = "^4";
		else if ( animState->weight > animState->goalWeight )
			color = "^1";
		else
			color = "";

		if ( !numAnims )
		{
			parts = animEntry->u.parts;
			goalTime = animState->time - animState->oldTime;

			if ( goalTime < 0.0f )
				goalTime = goalTime + 1.0f;

			deltaTime = parts->frequency != 0.0f ? goalTime / parts->frequency : 0.0f;

			if ( anim->notifyName )
			{
				Com_Printf(
				    "%s%s: (weight) %.2f -> %.2f, (time) %.2f -> %.2f, (realtimedelta) %.2f, '%s'\n",
				    color,
				    AnimDebugName,
				    animState->weight,
				    animState->goalWeight,
				    animState->oldTime,
				    animState->time,
				    deltaTime,
				    SL_ConvertToString(anim->notifyName));
			}
			else
			{
				Com_Printf(
				    "%s%s: (weight) %.2f -> %.2f, (time) %.2f -> %.2f, (realtimedelta) %.2f\n",
				    color,
				    AnimDebugName,
				    animState->weight,
				    animState->goalWeight,
				    animState->oldTime,
				    animState->time,
				    deltaTime);
			}
		}
		else
		{
			if ( anim->notifyName )
			{
				if ( XAnimIsLeafOrSync(tree->anims, infoIndex) )
				{
					if ( !anim->notifyChild )
					{
						Com_Printf(
						    "%s%s: (weight) %.2f -> %.2f, (time) %.2f -> %.2f, '%s'\n",
						    color,
						    AnimDebugName,
						    animState->weight,
						    animState->goalWeight,
						    animState->oldTime,
						    animState->time,
						    SL_ConvertToString(anim->notifyName));
					}
					else
					{
						Com_Printf(
						    "%s%s: (weight) %.2f -> %.2f, (time) %.2f -> %.2f, '%s'\n",
						    color,
						    AnimDebugName,
						    animState->weight,
						    animState->goalWeight,
						    animState->oldTime,
						    animState->time,
						    SL_ConvertToString(anim->notifyName));
					}
				}
				else
				{
					Com_Printf(
					    "%s%s: (weight) %.2f -> %.2f, '%s'\n",
					    color,
					    AnimDebugName,
					    animState->weight,
					    animState->goalWeight,
					    SL_ConvertToString(anim->notifyName));
				}
			}
			else if ( XAnimIsLeafOrSync(tree->anims, infoIndex) )
			{
				Com_Printf(
				    "%s%s: (weight) %.2f -> %.2f, (time) %.2f -> %.2f\n",
				    color,
				    AnimDebugName,
				    animState->weight,
				    animState->goalWeight,
				    animState->oldTime,
				    animState->time);
			}
			else
			{
				Com_Printf("%s%s: (weight) %.2f -> %.2f\n", color, AnimDebugName, animState->weight, animState->goalWeight);
			}

			for ( i = 0; i < numAnims; ++i )
			{
				infoIndex = i + animEntry->u.animParent.children;
				XAnimDisplay(tree, infoIndex, depth + 1);
			}
		}
	}
}

void XAnimCalcDeltaTree(const XAnimTree_s *tree, unsigned int animIndex, float weightScale, bool bClear, bool bNormQuat, XAnimSimpleRotPos *rotPos)
{
	float secondWeight;
	float firstWeight;
	int i;
	int j;
	int numAnims;
	XAnimSimpleRotPos *calcRotPos;
	float lenSq;
	XAnimSimpleRotPos tempRotPos;
	unsigned short infoIndex;
	XAnimEntry *animEntry;
	XAnimParts_s *parts;
	XAnimInfo *info;
	XAnimState *state;

	animEntry = &tree->anims->entries[animIndex];
	numAnims = animEntry->numAnims;

	if ( !numAnims )
	{
		if ( bClear )
		{
			Vector2Clear(rotPos->rot);
			rotPos->posWeight = 0;
			VectorClear(rotPos->pos);
		}

		parts = animEntry->u.parts;

		if ( !parts->bDelta )
			return;

		infoIndex = tree->infoArray[animIndex];

		if ( !infoIndex )
			return;

		info = &g_xAnimInfo[infoIndex];
		state = &info->state;

		if ( tree->bAbs )
			XAnimCalcAbsDeltaParts(parts, weightScale, state->time, rotPos);
		else
			XAnimCalcRelDeltaParts(parts, weightScale, state->oldTime, state->time, rotPos, 1);

		return;
	}

	for ( i = 0; i < numAnims; ++i )
	{
		infoIndex = tree->infoArray[i + animEntry->u.animParent.children];

		if ( !infoIndex )
			continue;

		state = &g_xAnimInfo[infoIndex].state;
		firstWeight = !tree->bUseGoalWeight ? state->weight : state->goalWeight;

		if ( firstWeight == 0.0f )
			continue;

		for ( j = i + 1; j < numAnims; ++j )
		{
			infoIndex = tree->infoArray[j + animEntry->u.animParent.children];

			if ( !infoIndex )
				continue;

			state = &g_xAnimInfo[infoIndex].state;
			secondWeight = !tree->bUseGoalWeight ? state->weight : state->goalWeight;

			if ( secondWeight == 0.0f )
				continue;

			calcRotPos = bClear ? rotPos : &tempRotPos;
			XAnimCalcDeltaTree(tree, i + animEntry->u.animParent.children, firstWeight, 1, 1, calcRotPos);
			XAnimCalcDeltaTree(tree, j + animEntry->u.animParent.children, secondWeight, 0, 1, calcRotPos);
			++j;

			for ( ; j < numAnims; ++j )
			{
				infoIndex = tree->infoArray[j + animEntry->u.animParent.children];

				if ( !infoIndex )
					continue;

				state = &g_xAnimInfo[infoIndex].state;
				secondWeight = !tree->bUseGoalWeight ? state->weight : state->goalWeight;

				if ( secondWeight == 0.0f )
					continue;

				XAnimCalcDeltaTree(tree, j + animEntry->u.animParent.children, secondWeight, 0, 1, calcRotPos);
			}

			if ( !bNormQuat )
			{
				if ( rotPos->posWeight != 0.0f )
					VectorScale(rotPos->pos, 1.0f / rotPos->posWeight, rotPos->pos);

				return;
			}

			if ( bClear )
			{
				lenSq = Vec2Multiply(rotPos->rot);

				if ( lenSq != 0.0f )
					Vec2Scale(rotPos->rot, I_rsqrt(lenSq) * weightScale, rotPos->rot);

				if ( rotPos->posWeight != 0.0f )
				{
					VectorScale(rotPos->pos, weightScale / rotPos->posWeight, rotPos->pos);
					rotPos->posWeight = weightScale;
				}
			}
			else
			{
				lenSq = Vec2Multiply(calcRotPos->rot);

				if ( lenSq != 0.0f )
					VectorMA2(rotPos->rot, I_rsqrt(lenSq) * weightScale, calcRotPos->rot, rotPos->rot);

				if ( calcRotPos->posWeight != 0.0f )
				{
					VectorMA(rotPos->pos, weightScale / calcRotPos->posWeight, calcRotPos->pos, rotPos->pos);
					rotPos->posWeight = rotPos->posWeight + weightScale;
				}
			}

			return;
		}

		XAnimCalcDeltaTree(tree, i + animEntry->u.animParent.children, weightScale, bClear, bNormQuat, rotPos);
		return;
	}

	if ( bClear )
	{
		Vector2Clear(rotPos->rot);
		rotPos->posWeight = 0;
		VectorClear(rotPos->pos);
	}
}

float XAnimGetLength(const XAnim_s *anims, unsigned int animIndex)
{
	XAnimEntry *entry;
	XAnimParts *parts;

	entry = (XAnimEntry *)&anims->entries[animIndex];
	parts = entry->u.parts;

	return (float)parts->numframes / parts->framerate;
}

int XAnimGetLengthMsec(const XAnim_s *anim, unsigned int animIndex)
{
	return (int)(XAnimGetLength(anim, animIndex) * 1000.0f);
}

float XAnimGetTime(const XAnimTree_s *tree, unsigned int animIndex)
{
	uint16_t childIndex;

	childIndex = tree->infoArray[animIndex];

	return childIndex ? g_xAnimInfo[childIndex].state.time : 0.0f;
}

float XAnimGetWeight(const XAnimTree_s *tree, unsigned int animIndex)
{
	uint16_t childIndex;

	childIndex = tree->infoArray[animIndex];

	return childIndex ? g_xAnimInfo[childIndex].state.weight : 0.0f;
}

bool XAnimHasFinished( const XAnimTree_s *tree, unsigned int animIndex )
{
	unsigned short index;
	const XAnimState *state;

	index = tree->infoArray[animIndex];

	if ( !index )
		return true;

	state = &g_xAnimInfo[index].state;
	return state->time < state->oldTime || state->time == 1.0f || state->cycleCount > state->oldCycleCount;
}

unsigned int XAnimGetChildAt(const XAnim_s *anim, unsigned int animIndex, unsigned int childIndex)
{
	return childIndex + anim->entries[animIndex].u.animParent.children;
}

const char* XAnimGetAnimName(const XAnim_s *anims, unsigned int animIndex)
{
	const XAnimEntry *entry;

	entry = &anims->entries[animIndex];

	if ( !entry->numAnims )
		return entry->u.parts->name;

	return "";
}

bool XanimIsDefaultPart(XAnimParts *animParts)
{
	return animParts->isDefault;
}

const char* XAnimGetAnimDebugName(const XAnim_s *anims, unsigned int animIndex)
{
	const XAnimEntry *entry;
	const char *format;
	bool missing;

	entry = &anims->entries[animIndex];

	if ( !anims->debugAnimNames )
	{
		return !entry->numAnims ? entry->u.parts->name : va("%i", animIndex);
	}

	format = anims->debugAnimNames[animIndex];

	missing = !entry->numAnims && XanimIsDefaultPart(entry->u.parts);

	return !missing ? format : va("^3%s (missing)", format);
}

const char* XAnimGetAnimTreeDebugName(const XAnim_s *anims)
{
	return anims->debugName;
}

unsigned int XAnimGetAnimTreeSize(const XAnim_s *anims)
{
	return anims->size;
}

void DObjUpdateServerOldTime( DObj_s *obj, DObj_s *serverObj, float dtime )
{
	bool hasWeight;
	bool hasTime;
	XAnimState syncState;

	if ( !obj->tree )
		return;

	syncState.time = 0;
	syncState.cycleCount = 0;
	XAnimUpdateOldTime(serverObj->tree, 0, &syncState, dtime, 1, &hasWeight, &hasTime);
	XAnimCloneAnimTreeOldState(obj->tree, serverObj->tree);
}

void DObjInitServerTime(DObj_s *obj, float dtime)
{
	bool hasWeight;
	bool hasTime;
	XAnimState syncState;

	if ( !obj->tree )
	{
		return;
	}

	syncState.time = 0.0;
	syncState.cycleCount = 0;

	XAnimUpdateOldTime(obj->tree, 0, &syncState, dtime, 1, &hasWeight, &hasTime);
}

void DObjUpdateClientInfo( DObj_s *obj, float dtime )
{
	bool hasWeight;
	bool hasTime;
	XAnimState syncState;

	g_notifyListSize = 0;

	if ( !obj->tree )
		return;

	syncState.time = 0;
	syncState.cycleCount = 0;
	XAnimUpdateOldTime(obj->tree, 0, &syncState, dtime, 1, &hasWeight, &hasTime);
	XAnimUpdateInfoInternal(obj->tree, 0, dtime, 1);
}

int DObjUpdateServerInfo(DObj_s *obj, float dtime, int bNotify)
{
	float track;
	float time;

	if ( !obj->tree )
		return 0;

	if ( !bNotify )
	{
		XAnimUpdateInfoInternal(obj->tree, 0, dtime, 0);
		return 0;
	}

	track = XAnimFindServerNoteTrack(obj->tree, 0, dtime);

	if ( track != 1.0f )
	{
		time = dtime * track + 0.001f;

		if ( time <= dtime )
		{
			XAnimUpdateInfoInternal(obj->tree, 0, time, 1);
			return 1;
		}
	}

	XAnimUpdateInfoInternal(obj->tree, 0, dtime, 1);
	return 0;
}

int DObjGetClientNotifyList( XAnimClientNotify **notifyList )
{
	*notifyList = g_notifyList;
	return g_notifyListSize;
}

void DObjCalcAnim(const DObj_s *obj, int *partBits)
{
	int boneCount;
	DSkel_t *skel;
	DObjAnimMat *Mat;
	int i;
	XModelParts_s *parts;
	short *quats;
	int boneIndex;
	XBoneHierarchy *hierarchy;
	bool bNoPartBits;
	XAnimCalcAnimInfo animInfo;

	skel = obj->skel;
	bNoPartBits = 1;

	for ( boneCount = 0; boneCount <= 3; ++boneCount )
	{
		animInfo.animPartBits[boneCount] = ~partBits[boneCount] | skel->partBits.anim[boneCount];

		if ( ~animInfo.animPartBits[boneCount] )
			bNoPartBits = 0;
	}

	if ( bNoPartBits )
		return;

	for ( boneCount = 0; boneCount <= 3; ++boneCount )
	{
		skel->partBits.anim[boneCount] |= partBits[boneCount];
		animInfo.ignorePartBits[boneCount] = animInfo.animPartBits[boneCount];
	}

	Mat = &skel->Mat;

	if ( obj->tree )
	{
		animInfo.ignorePartBits[3] |= 0x80000000;
		XAnimCalc(obj, 0, 1.0, Mat, 1, 0, &animInfo, 0);
	}

	boneIndex = 0;

	for ( i = 0; i < obj->numModels; ++i )
	{
		parts = obj->models[i]->parts;
		boneCount = parts->numRootBones;

		while ( boneCount )
		{
			if ( animInfo.animPartBits[boneIndex >> 5] & (1 << (boneIndex & 0x1F)) )
			{
			}
			else
			{
				Mat->quat[0] = 0.0;
				Mat->quat[1] = 0.0;
				Mat->quat[2] = 0.0;
				Mat->quat[3] = 1.0;
				VectorClear(Mat->trans);
			}

			--boneCount;
			++Mat;
			++boneIndex;
		}

		quats = parts->quats;
		hierarchy = parts->hierarchy;
		boneCount = parts->numBones - parts->numRootBones;

		while ( boneCount )
		{
			if ( animInfo.animPartBits[boneIndex >> 5] & (1 << (boneIndex & 0x1F)) )
			{
			}
			else
			{
				Short4ScaleAsVec4(quats, 0.000030518509, Mat->quat);
				VectorClear(Mat->trans);
			}

			--boneCount;
			++Mat;
			++boneIndex;
			quats += 4;
		}
	}
}

void DObjDisplayAnim(DObj_s *obj)
{
	if ( !obj->tree )
	{
		Com_Printf("NO TREE\n");
		return;
	}

	XAnimDisplay(obj->tree, 0, 0);
	Com_Printf("\n");
}

void XAnimCalcDelta(XAnimTree_s *tree, unsigned int animIndex, float *rot, float *trans, bool bUseGoalWeight)
{
	XAnimSimpleRotPos rotPos;

	tree->bAbs = 0;
	tree->bUseGoalWeight = bUseGoalWeight;

	XAnimCalcDeltaTree(tree, animIndex, 1.0, 1, 0, &rotPos);

	if ( rotPos.rot[0] != 0.0f && rotPos.rot[1] != 0.0f )
	{
		Vector2Copy(rotPos.rot, rot);
	}
	else
	{
		rot[0] = 0.0f;
		rot[1] = 1.0;
	}

	VectorCopy(rotPos.pos, trans);
}

void XAnimCalcAbsDelta(XAnimTree_s *tree, unsigned int animIndex, float *rot, float *trans)
{
	XAnimSimpleRotPos rotPos;

	tree->bAbs = 1;
	tree->bUseGoalWeight = 1;

	XAnimCalcDeltaTree(tree, animIndex, 1.0, 1, 0, &rotPos);

	if ( rotPos.rot[0] != 0.0f || rotPos.rot[1] != 0.0f )
	{
		Vector2Copy(rotPos.rot, rot);
	}
	else
	{
		rot[0] = 0.0f;
		rot[1] = 1.0;
	}

	VectorCopy(rotPos.pos, trans);
}

void XAnimGetRelDelta(const XAnim_s *anims, unsigned int animIndex, float *rot, float *trans, float time1, float time2)
{
	XAnimSimpleRotPos rotPos;
	const XAnimEntry *entry;
	int numAnims;
	XAnimParts_s *parts;

	entry = &anims->entries[animIndex];
	numAnims = entry->numAnims;

	if ( !numAnims && (parts = entry->u.parts, parts->bDelta) )
	{
		Vector2Clear(rotPos.rot);
		rotPos.posWeight = 0.0f;
		VectorClear(rotPos.pos);
		XAnimCalcRelDeltaParts(parts, 1.0, time1, time2, &rotPos, 0);

		if ( rotPos.rot[0] != 0.0f || rotPos.rot[1] != 0.0f )
		{
			Vector2Copy(rotPos.rot, rot);
		}
		else
		{
			rot[0] = 0.0f;
			rot[1] = 1.0;
		}

		VectorCopy(rotPos.pos, trans);
	}
	else
	{
		rot[0] = 0.0f;
		rot[1] = 1.0;
		VectorClear(trans);
	}
}

void XAnimGetAbsDelta(const XAnim_s *anims, unsigned int animIndex, float *rot, float *trans, float time)
{
	XAnimSimpleRotPos rotPos;
	const XAnimEntry *entry;
	int numAnims;
	XAnimParts_s *parts;

	entry = &anims->entries[animIndex];
	numAnims = entry->numAnims;

	if ( !numAnims && (parts = entry->u.parts, parts->bDelta) )
	{
		Vector2Clear(rotPos.rot);
		rotPos.posWeight = 0.0f;
		VectorClear(rotPos.pos);
		XAnimCalcAbsDeltaParts(parts, 1.0, time, &rotPos);

		if ( rotPos.rot[0] != 0.0f || rotPos.rot[1] != 0.0f )
		{
			Vector2Copy(rotPos.rot, rot);
		}
		else
		{
			rot[0] = 0.0f;
			rot[1] = 1.0;
		}

		VectorCopy(rotPos.pos, trans);
	}
	else
	{
		rot[0] = 0.0f;
		rot[1] = 1.0;
		VectorClear(trans);
	}
}

XAnimInfo* XAnimGetInfo(XAnimTree_s *tree, unsigned int infoIndex)
{
	uint16_t next;
	uint16_t newHead;

	next = g_xAnimInfo->next;

	if ( !next )
	{
		Com_Error(ERR_DROP, "\x15" "exceeded maximum number of anim info");
		return NULL;
	}

	newHead = g_xAnimInfo[next].next;
	g_xAnimInfo->next = newHead;
	g_xAnimInfo[newHead].prev = 0;
	tree->infoArray[infoIndex] = next;

	return &g_xAnimInfo[next];
}

void XAnimClearGoalWeight(XAnimTree_s *tree, unsigned int animIndex, float blendTime)
{
	uint16_t childIndex;
	XAnimInfo *info;
	XAnimState *state;

	childIndex = tree->infoArray[animIndex];

	if ( !childIndex )
	{
	}
	else
	{
		info = &g_xAnimInfo[childIndex];
		state = &info->state;

		if ( state->goalWeight != 0.0f )
		{
			state->goalTime = blendTime;
		}
		else
		{
			if ( state->goalTime > blendTime )
				state->goalTime = blendTime;
		}

		state->goalWeight = 0.0f;

		if ( blendTime == 0.0f )
			state->weight = 0.0f;

		XAnimFreeNotifyStrings(info);
	}
}

void XAnimClearTreeGoalWeights(XAnimTree_s *tree, unsigned int animIndex, float blendTime)
{
	int numAnims;
	int i;
	XAnimEntry *entry;

	if ( !tree->infoArray[animIndex] )
	{
		return;
	}

	XAnimClearGoalWeight(tree, animIndex, blendTime);
	entry = &tree->anims->entries[animIndex];
	numAnims = entry->numAnims;

	for ( i = 0; i < numAnims; ++i )
		XAnimClearTreeGoalWeights(tree, i + entry->u.animParent.children, blendTime);
}

void XAnimClearTreeGoalWeightsStrict( XAnimTree_s *tree, unsigned int animIndex, float blendTime )
{
	int numAnims;
	int i;
	XAnimEntry *entry;

	entry = &tree->anims->entries[animIndex];
	numAnims = entry->numAnims;

	for ( i = 0; i < numAnims; ++i )
		XAnimClearTreeGoalWeights(tree, i + entry->u.animParent.children, blendTime);
}

void XAnimClearGoalWeightKnobInternal(XAnimTree_s *tree, unsigned int infoIndex, float goalWeight, float goalTime)
{
	int i;
	int numAnims;
	XAnimEntry *selfEntry;
	XAnimEntry *entry;
	int parentIndex;
	float blendTime;
	float absWeight;
	float newGoalWeight;
	float weight;
	uint16_t childIndex;

	if ( !infoIndex )
	{
		return;
	}

	parentIndex = tree->anims->entries[infoIndex].parent;
	selfEntry = &tree->anims->entries[infoIndex];
	entry = &tree->anims->entries[parentIndex];

	newGoalWeight = 0.0f;
	numAnims = entry->numAnims;

	for ( i = 0; i < numAnims; ++i )
	{
		childIndex = tree->infoArray[i + entry->u.animParent.children];
		weight = childIndex ? g_xAnimInfo[childIndex].state.weight : 0.0f;
		absWeight = i + entry->u.animParent.children == infoIndex ? I_fabs(goalWeight - weight) : weight;

		if ( absWeight > newGoalWeight )
		{
			newGoalWeight = absWeight;
		}
	}

	blendTime = newGoalWeight * goalTime;

	for ( i = 0; i < numAnims; ++i )
	{
		if ( i + entry->u.animParent.children == infoIndex )
		{
			continue;
		}

		XAnimClearGoalWeight(tree, i + entry->u.animParent.children, blendTime);
	}
}

void XAnimSetCompleteGoalWeightKnob( XAnimTree_s *tree, unsigned int animIndex, float goalWeight, float goalTime, float rate, unsigned int notifyName, int bRestart )
{
	if ( goalWeight < 0.001f )
		goalWeight = 0;

	XAnimClearGoalWeightKnobInternal(tree, animIndex, goalWeight, goalTime);
	XAnimSetCompleteGoalWeight(tree, animIndex, goalWeight, goalTime, rate, notifyName, 0, bRestart);
}

int XAnimSetCompleteGoalWeightKnobAll(XAnimTree_s *tree, unsigned int animIndex, unsigned int rootIndex, float goalWeight, float goalTime, float rate, unsigned int notifyName, int bRestart)
{
	int result;

	if ( goalWeight < 0.001f )
		goalWeight = 0.0;

	XAnimClearGoalWeightKnobInternal(tree, animIndex, goalWeight, goalTime);
	result = XAnimSetGoalWeightInternal(tree, animIndex, goalWeight, goalTime, rate, 0, notifyName, 0);
	XAnimClearParentGoalWeights(tree, animIndex, goalTime);

	if ( bRestart )
		XAnimRestart(tree, animIndex);

	for ( ;; )
	{
		if ( !animIndex )
			return 1;

		animIndex = tree->anims->entries[animIndex].parent;

		if ( animIndex == rootIndex )
			return result;

		XAnimClearGoalWeightKnobInternal(tree, animIndex, 1.0, goalTime);
		XAnimSetGoalWeightInternal(tree, animIndex, 1.0, goalTime, 1.0, 0, 0, 0);

		if ( bRestart )
			XAnimRestart(tree, animIndex);
	}
}

void XAnimSetGoalWeightKnob( XAnimTree_s *tree, unsigned int animIndex, float goalWeight, float goalTime, float rate, unsigned int notifyName, int bRestart )
{
	if ( goalWeight < 0.001f )
		goalWeight = 0;

	XAnimClearGoalWeightKnobInternal(tree, animIndex, goalWeight, goalTime);
	XAnimSetGoalWeight(tree, animIndex, goalWeight, goalTime, rate, notifyName, 0, bRestart);
}

void XAnimClearChildGoalWeights(XAnimTree_s *tree, unsigned int animIndex, float blendTime)
{
	int i;
	int numAnims;
	XAnimEntry *entry;

	entry = &tree->anims->entries[animIndex];
	numAnims = entry->numAnims;

	for ( i = 0; i < numAnims; ++i )
		XAnimClearGoalWeight(tree, i + entry->u.animParent.children, blendTime);
}

void XAnimClearTreeWeights(XAnimTree_s *info, int index)
{
	int numAnims;
	int i;
	int childIndex;
	XAnimEntry *entry;

	childIndex = info->infoArray[index];

	if ( !childIndex )
	{
	}
	else
	{
		entry = &info->anims->entries[index];
		numAnims = entry->numAnims;

		for ( i = 0; i < numAnims; ++i )
			XAnimClearTreeWeights(info, i + entry->u.animParent.children);

		XAnimFreeInfo(info, childIndex);
		info->infoArray[index] = 0;
	}
}

void XAnimClearTreeParts(XAnimTree_s *tree)
{
	int offset;
	int size;
	unsigned short *parts;
	int i;

	size = tree->anims->size;
	// per-anim string handles follow the info array
	offset = 2 * size + 8;
	parts = (unsigned short *)((char *)tree + offset);

	for ( i = 0; i < size; i++ )
	{
		if ( !parts[i] )
			continue;

		{
			XAnimEntry *entry = &tree->anims->entries[i];
			XAnimParts_s *animParts = entry->u.parts;

			SL_RemoveRefToStringOfLen(parts[i], animParts->boneCount + 16);
			parts[i] = 0;
		}
	}
}

void XAnimClearTree(XAnimTree_s *tree)
{
	XAnimClearTreeWeights(tree, 0);
	XAnimClearTreeParts(tree);
}

unsigned int XAnimGetDescendantWithGreatestWeight(const XAnimTree_s *tree, unsigned int infoIndex)
{
	float weight;
	float goalWeight;
	unsigned int result;
	unsigned int greatest;
	int i;
	XAnimEntry *entry;

	entry = &tree->anims->entries[infoIndex];

	if ( !entry->numAnims )
		return infoIndex;

	weight = 0.0;
	greatest = 0;

	for ( i = 0; i < entry->numAnims; ++i )
	{
		goalWeight = g_xAnimInfo[tree->infoArray[i + entry->u.animParent.children]].state.goalWeight;

		if ( goalWeight <= weight )
		{
			continue;
		}

		result = XAnimGetDescendantWithGreatestWeight(tree, i + entry->u.animParent.children);

		if ( !result )
		{
			continue;
		}

		weight = goalWeight;
		greatest = result;
	}

	return greatest;
}

int XAnimSetGoalWeightInternal(XAnimTree_s *tree, unsigned int animIndex, float goalWeight, float goalTime, float rate, bool useGoalWeight, unsigned int notifyName, unsigned int notifyType)
{
	int infoIndex;
	XAnimInfo *info;
	XAnimState *state;
	XAnimEntry *entry;

	infoIndex = tree->infoArray[animIndex];

	if ( !infoIndex )
	{
		if ( goalWeight == 0.0f )
		{
			if ( !useGoalWeight )
				return 0;
		}

		info = XAnimGetInfo(tree, animIndex);
		XAnimResetInfo(info);
	}
	else
	{
		info = &g_xAnimInfo[infoIndex];
		XAnimFreeNotifyStrings(info);
	}
retry:
	if ( !animIndex )
	{
		goalWeight = 1.0;
		goalTime = 0.0f;
		rate = 1.0;
	}

	state = &info->state;

	if ( goalTime == 0.0f )
	{
		state->weight = goalWeight;
	}
	else if ( state->weight == 0.0f )
	{
		state->weight = goalWeight * 0.001f;
	}

	if ( goalWeight != 0.0f )
	{
		if ( goalWeight >= state->weight )
		{
			state->goalTime = (goalWeight - state->weight) * goalTime / goalWeight;
		}
		else
		{
			state->goalTime = (state->weight - goalWeight) * goalTime / state->weight;
		}
	}
	else
	{
		if ( state->goalWeight != 0.0f )
		{
			state->goalTime = goalTime;
		}
		else if ( state->goalTime > goalTime )
		{
			state->goalTime = goalTime;
		}
	}

	state->goalWeight = goalWeight;
	info->state.rate = rate;
	info->notifyName = notifyName;

	if ( notifyName )
		SL_AddRefToString(notifyName);

	entry = &tree->anims->entries[animIndex];

	if ( notifyName
	        && entry->numAnims
	        && (entry->u.animParent.flags & 3) != 0 )
	{
		info->notifyChild = XAnimGetDescendantWithGreatestWeight(tree, animIndex);

		if ( !info->notifyChild )
			return 2;
	}
	else
	{
		info->notifyChild = 0;
	}

	info->notifyType = notifyType;
	return 0;
}

void XAnimSetAnimRateInternal(XAnimTree_s *tree, unsigned int animIndex, float rate)
{
	uint16_t childIndex;

	childIndex = tree->infoArray[animIndex];

	g_xAnimInfo[childIndex].state.rate = rate;
}

static void *Hunk_AllocXAnimPrecache(int size)
{
	return Hunk_AllocAlignInternal(size, 4);
}

void XAnimFillInSyncNodes_r(XAnim_s *anims, unsigned int animIndex, bool bLoop)
{
	XAnimEntry *entry;
	int numAnims;
	int i;
	int count;

	entry = &anims->entries[animIndex];
	numAnims = entry->numAnims;

	if ( !numAnims )
	{
		if ( entry->u.parts->bLoop != bLoop )
		{
			if ( XanimIsDefaultPart(entry->u.parts) )
			{
				XAnimPrecache("void_loop", Hunk_AllocXAnimPrecache);
				entry->u.parts = XAnimFindData("void_loop");

				if ( !entry->u.parts )
					Com_Error(ERR_DROP, "\x15" "Cannot find 'xanim/%s'.\nThis is a default xanim file that you should have.\n", "void_loop");
			}
			else if ( bLoop )
			{
				Com_Error(ERR_DROP, "\x15" "animation '%s' in '%s' cannot be sync looping and nonlooping", XAnimGetAnimDebugName(anims, animIndex), anims->debugName);
			}
			else
			{
				Com_Error(ERR_DROP, "\x15" "animation '%s' in '%s' cannot be sync nonlooping and looping", XAnimGetAnimDebugName(anims, animIndex), anims->debugName);
			}
		}
	}
	else
	{
		if ( (entry->u.animParent.flags & 3) != 0 )
		{
			count = 0;

			do
			{
				++count;
				entry = &anims->entries[entry->u.animParent.children];
			}
			while ( entry->numAnims );

			Com_Error(ERR_DROP, "\x15" "duplicate specification of animation sync in '%s', %d nodes above '%s'", anims->debugName, count, XAnimGetAnimDebugName(anims, animIndex));
		}

		entry->u.animParent.flags |= bLoop ? 1 : 2;

		for ( i = 0; i < numAnims; ++i )
			XAnimFillInSyncNodes_r(anims, i + entry->u.animParent.children, bLoop);
	}
}

void XAnimSetupSyncNodes_r(XAnim_s *anims, unsigned int animIndex)
{
	XAnimEntry *entry;
	int numAnims;
	int i;
	int parFlags;
	bool bLoop;

	entry = &anims->entries[animIndex];
	numAnims = entry->numAnims;

	if ( !numAnims )
	{
	}
	else
	{
		parFlags = entry->u.animParent.flags & 3;

		if ( parFlags )
		{
			if ( parFlags == 3 )
				Com_Error(ERR_DROP, "\x15" "animation cannot be sync looping and sync nonlooping");

			entry->u.animParent.flags |= 4u;
			bLoop = parFlags == 1;

			for ( i = 0; i < numAnims; ++i )
				XAnimFillInSyncNodes_r(anims, i + entry->u.animParent.children, bLoop);
		}
		else
		{
			for ( i = 0; i < numAnims; ++i )
				XAnimSetupSyncNodes_r(anims, i + entry->u.animParent.children);
		}
	}
}

void XAnimSetupSyncNodes(XAnim_s *anims)
{
	XAnimSetupSyncNodes_r(anims, 0);
}

bool XAnimIsLeafOrSync( const XAnim_s *anims, unsigned int animIndex )
{
	const XAnimEntry *entry;

	entry = &anims->entries[animIndex];
	return !entry->numAnims || (entry->u.animParent.flags & 3);
}

bool XAnimIsPrimitive(XAnim_s *anim, unsigned int animIndex)
{
	return anim->entries[animIndex].numAnims == 0;
}

// Retail inlines XAnimSetTime in CG_UpdateKillCam, calls it from BG_SetNewAnimation
__forceinline void XAnimSetTimeInl(XAnimTree_s *tree, unsigned int animIndex, float time)
{
	uint16_t childIndex;
	XAnimInfo *info;
	XAnimState *state;
	XAnimEntry *entry;

	childIndex = tree->infoArray[animIndex];

	if ( !childIndex )
	{
	}
	else
	{
		entry = &tree->anims->entries[animIndex];
		info = &g_xAnimInfo[childIndex];
		state = &info->state;
		state->time = time;
		state->cycleCount = 0;
		state->oldTime = time;
		state->oldCycleCount = 0;
		info->notifyIndex = -1;
	}
}

void XAnimSetTime(XAnimTree_s *tree, unsigned int animIndex, float time)
{
	XAnimSetTimeInl(tree, animIndex, time);
}

void XAnimSetNotifyIndex( const XAnimTree_s *tree, XAnimInfo *info, const XAnimEntry *entry )
{
	if ( info->state.time == 1.0f )
		return;

	if ( entry->numAnims )
	{
		if ( !info->notifyChild )
			return;

		entry = &tree->anims->entries[info->notifyChild];
	}

	info->notifyIndex = XAnimGetNextNotifyIndex(entry, info->state.time);
}

void XAnimRestart(XAnimTree_s *tree, unsigned int infoIndex)
{
	XAnimInfo *info;
	unsigned int i;
	XAnimEntry *entry;
	int numAnims;
	int j;

	for ( i = infoIndex; i; i = tree->anims->entries[i].parent )
	{
		info = &g_xAnimInfo[tree->infoArray[i]];
		entry = &tree->anims->entries[i];

		if ( !entry->numAnims )
		{
		}
		else if ( !(entry->u.animParent.flags & 4) )
		{
		}
		else
		{
			if ( !XAnimNeedClearState(info) )
			{
			}
			else
			{
				numAnims = entry->numAnims;

				for ( j = 0; j < numAnims; ++j )
					XAnimResetTime(tree, j + entry->u.animParent.children);
			}

			return;
		}
	}

	entry = &tree->anims->entries[infoIndex];
	numAnims = entry->numAnims;

	if ( numAnims )
	{
	}
	else
	{
		XAnimNeedClearState(&g_xAnimInfo[tree->infoArray[infoIndex]]);
	}
}

void XAnimClearParentGoalWeights( XAnimTree_s *tree, unsigned int animIndex, float blendTime )
{
	unsigned int parentIndex;

	parentIndex = animIndex;

	while ( parentIndex )
	{
		parentIndex = tree->anims->entries[parentIndex].parent;

		if ( tree->infoArray[parentIndex] )
			return;

		XAnimSetGoalWeightInternal(tree, parentIndex, 0, blendTime, 1, 1, 0, 0);
	}
}

int XAnimSetGoalWeight(XAnimTree_s *tree, unsigned int animIndex, float goalWeight, float goalTime, float rate, unsigned int notifyName, unsigned int notifyType, int bRestart)
{
	int infoIndex;

	if ( goalWeight < 0.001f )
		goalWeight = 0.0f;

	infoIndex = XAnimSetGoalWeightInternal(tree, animIndex, goalWeight, goalTime, rate, 0, notifyName, notifyType);
	XAnimClearParentGoalWeights(tree, animIndex, goalTime);

	if ( bRestart )
		XAnimRestart(tree, animIndex);

	return infoIndex;
}

void XAnimSetAnimRate(XAnimTree_s *tree, unsigned int animIndex, float rate)
{
	XAnimSetAnimRateInternal(tree, animIndex, rate);
}

bool XAnimIsLooped(const XAnim_s *anim, unsigned int animIndex)
{
	const XAnimEntry *entry;

	entry = &anim->entries[animIndex];

	if ( entry->numAnims )
		return entry->u.animParent.flags & 1;

	return entry->u.parts->bLoop;
}

bool XAnimNotetrackExists(const XAnim_s *anims, unsigned int animIndex, unsigned int name)
{
	const XAnimEntry *entry;
	XAnimParts_s *parts;
	XAnimNotifyInfo *info;
	int i;

	entry = &anims->entries[animIndex];
	parts = entry->u.parts;
	info = parts->notify;

	if ( !info )
		return 0;

	for ( i = 0; i < parts->notifyCount; ++i, ++info )
	{
		if ( info->name == name )
			return 1;
	}

	return 0;
}

void XAnimSetCompleteGoalWeight(XAnimTree_s *tree, unsigned int animIndex, float goalWeight, float goalTime, float rate, unsigned int notifyName, unsigned int notifyType, int bRestart)
{
	int info;
	unsigned int childIndex;

	if ( goalWeight < 0.001f )
		goalWeight = 0.0f;

	XAnimSetGoalWeightInternal(tree, animIndex, goalWeight, goalTime, rate, 0, notifyName, notifyType);
	childIndex = animIndex;

	while ( childIndex )
	{
		childIndex = tree->anims->entries[childIndex].parent;
		info = tree->infoArray[childIndex];

		if ( info && g_xAnimInfo[info].state.goalWeight != 0.0f )
		{
		}
		else
		{
			XAnimSetGoalWeightInternal(tree, childIndex, 1.0, goalTime, 1.0, 0, 0, 0);
		}
	}

	if ( bRestart )
		XAnimRestart(tree, animIndex);
}

void XAnimCloneInfo(XAnimInfo *from, XAnimInfo *to)
{
	*to = *from;

	if ( to->notifyName )
		SL_AddRefToString(to->notifyName);
}

void XAnimCloneAnimTree(const XAnimTree_s *from, XAnimTree_s *to)
{
	signed int infoIndex;
	int index;
	XAnimInfo *info;
	signed int size;

	size = from->anims->size;

	for ( infoIndex = 0; infoIndex < size; ++infoIndex )
	{
		index = from->infoArray[infoIndex];

		if ( !index )
		{
			index = to->infoArray[infoIndex];

			if ( index )
			{
				XAnimFreeInfo(to, index);
				to->infoArray[infoIndex] = 0;
			}
		}
		else
		{
			if ( !to->infoArray[infoIndex] )
			{
				info = XAnimGetInfo(to, infoIndex);
			}
			else
			{
				info = &g_xAnimInfo[to->infoArray[infoIndex]];
				XAnimFreeNotifyStrings(info);
			}

			XAnimCloneInfo(&g_xAnimInfo[index], info);
		}
	}
}

void XAnimCopyInfoNoNotify( const XAnimInfo *from, XAnimInfo *to )
{
	*to = *from;
	to->notifyChild = 0;
	to->notifyIndex = -1;
	to->notifyName = 0;
}

void XAnimCopyAnimTreeNoNotify( const XAnimTree_s *from, XAnimTree_s *to )
{
	int i;
	unsigned short index;
	XAnimInfo *info;
	int size;

	size = from->anims->size;

	for ( i = 0; i < size; i++ )
	{
		index = from->infoArray[i];

		if ( !index )
			continue;

		info = XAnimGetInfo(to, i);
		XAnimCopyInfoNoNotify(&g_xAnimInfo[index], info);
	}
}

XAnimInfo *XAnimGetInfoByIndex( unsigned int index )
{
	return &g_xAnimInfo[index];
}

void XAnim_CalcSimpleRotDuring_unsigned_char_(XAnimTime *animTime, const XAnimPartQuat *quat, float scale, float *rotDelta)
{
	int keyFrameIndex;
	float keyFrameLerpFrac;

	if ( !quat->size )
	{
		Vec2MadShort2(rotDelta, scale, quat->u.frame0, rotDelta);
	}
	else
	{
		XAnim_GetTimeIndexCompressed_unsigned_char(
		    animTime,
		    quat->u.frames.indices._1,
		    quat->size,
		    &keyFrameIndex,
		    &keyFrameLerpFrac);
		Vec2MadShort2Lerp(
		    rotDelta,
		    scale,
		    quat->u.frames.u.frames2[keyFrameIndex],
		    quat->u.frames.u.frames2[keyFrameIndex + 1],
		    keyFrameLerpFrac,
		    rotDelta);
	}
}

void XAnim_CalcRotDuring_unsigned_char_(XAnimTime *animTime, const XAnimPartQuat *quat, float scale, float *rotDelta)
{
	int keyFrameIndex;
	float keyFrameLerpFrac;

	if ( !quat->size )
	{
		Vec4MadShort4(rotDelta, scale, quat->u.frame0, rotDelta);
	}
	else
	{
		XAnim_GetTimeIndexCompressed_unsigned_char(
		    animTime,
		    quat->u.frames.indices._1,
		    quat->size,
		    &keyFrameIndex,
		    &keyFrameLerpFrac);
		Vec4MadShort4Lerp(
		    rotDelta,
		    scale,
		    quat->u.frames.u.frames[keyFrameIndex],
		    quat->u.frames.u.frames[keyFrameIndex + 1],
		    keyFrameLerpFrac,
		    rotDelta);
	}
}

void XAnim_CalcPosDuring_unsigned_char_(XAnimTime *animTime, const XAnimPartTrans *trans, float scale, float *posDelta)
{
	int keyFrameIndex;
	float keyFrameLerpFrac;

	if ( !trans->size )
	{
		VectorMA(posDelta, scale, trans->u.frame0, posDelta);
	}
	else
	{
		XAnim_GetTimeIndexCompressed_unsigned_char(
		    animTime,
		    trans->u.frames.indices._1,
		    trans->size,
		    &keyFrameIndex,
		    &keyFrameLerpFrac);
		Vec3MadVec3Lerp(
		    posDelta,
		    scale,
		    trans->u.frames.frames[keyFrameIndex],
		    trans->u.frames.frames[keyFrameIndex + 1],
		    keyFrameLerpFrac,
		    posDelta);
	}
}

void XAnimCalcParts_unsigned_char_( const XAnimParts_s *parts, const unsigned char *animToModel, float time, float weightScale, DObjAnimMat *rotTransArray, int *ignorePartBits )
{
	DObjAnimMat *totalRotTrans;
	int i;
	XAnimPart *childPart;
	float scale;
	int animPartIndex;
	int boneCount;
	int numframes;
	char *simpleQuatBits;
	XAnimTime animTime;

	numframes = parts->numframes;
	XAnim_SetTime(time, numframes, &animTime);
	scale = weightScale * 0.000030518509f;
	simpleQuatBits = parts->simpleQuatBits;
	boneCount = parts->boneCount;

	for ( i = 0; i < boneCount; ++i )
	{
		animPartIndex = animToModel[i];

		if ( ( 1 << (animPartIndex & 0x1F) ) & ignorePartBits[animPartIndex >> 5] )
			continue;

		totalRotTrans = &rotTransArray[animPartIndex];
		childPart = &parts->parts[i];

		if ( ( 1 << (i & 7) ) & simpleQuatBits[i >> 3] )
		{
			if ( childPart->quat )
				XAnim_CalcSimpleRotDuring_unsigned_char_(&animTime, childPart->quat, scale, &totalRotTrans->quat[2]);
			else
				totalRotTrans->quat[3] = totalRotTrans->quat[3] + weightScale;
		}
		else
		{
			XAnim_CalcRotDuring_unsigned_char_(&animTime, childPart->quat, scale, totalRotTrans->quat);
		}

		if ( childPart->trans )
			XAnim_CalcPosDuring_unsigned_char_(&animTime, childPart->trans, weightScale, totalRotTrans->trans);

		totalRotTrans->transWeight = totalRotTrans->transWeight + weightScale;
	}
}

void XAnim_CalcSimpleRotDuring_unsigned_short_(XAnimTime *animTime, const XAnimPartQuat *quat, float scale, float *rotDelta)
{
	int keyFrameIndex;
	float keyFrameLerpFrac;

	if ( !quat->size )
	{
		Vec2MadShort2(rotDelta, scale, quat->u.frame0, rotDelta);
	}
	else
	{
		XAnim_GetTimeIndexCompressed_unsigned_short_(
		    animTime,
		    quat->u.frames.indices._2,
		    quat->size,
		    &keyFrameIndex,
		    &keyFrameLerpFrac);
		Vec2MadShort2Lerp(
		    rotDelta,
		    scale,
		    quat->u.frames.u.frames2[keyFrameIndex],
		    quat->u.frames.u.frames2[keyFrameIndex + 1],
		    keyFrameLerpFrac,
		    rotDelta);
	}
}

void XAnim_CalcRotDuring_unsigned_short_(XAnimTime *animTime, const XAnimPartQuat *quat, float scale, float *rotDelta)
{
	int keyFrameIndex;
	float keyFrameLerpFrac;

	if ( !quat->size )
	{
		Vec4MadShort4(rotDelta, scale, quat->u.frame0, rotDelta);
	}
	else
	{
		XAnim_GetTimeIndexCompressed_unsigned_short_(
		    animTime,
		    quat->u.frames.indices._2,
		    quat->size,
		    &keyFrameIndex,
		    &keyFrameLerpFrac);
		Vec4MadShort4Lerp(
		    rotDelta,
		    scale,
		    quat->u.frames.u.frames[keyFrameIndex],
		    quat->u.frames.u.frames[keyFrameIndex + 1],
		    keyFrameLerpFrac,
		    rotDelta);
	}
}

void XAnim_CalcPosDuring_unsigned_short_(XAnimTime *animTime, const XAnimPartTrans *trans, float scale, float *posDelta)
{
	int keyFrameIndex;
	float keyFrameLerpFrac;

	if ( !trans->size )
	{
		VectorMA(posDelta, scale, trans->u.frame0, posDelta);
	}
	else
	{
		XAnim_GetTimeIndexCompressed_unsigned_short_(
		    animTime,
		    trans->u.frames.indices._2,
		    trans->size,
		    &keyFrameIndex,
		    &keyFrameLerpFrac);
		Vec3MadVec3Lerp(
		    posDelta,
		    scale,
		    trans->u.frames.frames[keyFrameIndex],
		    trans->u.frames.frames[keyFrameIndex + 1],
		    keyFrameLerpFrac,
		    posDelta);
	}
}

void XAnimCalcParts_unsigned_short_( const XAnimParts_s *parts, const unsigned char *animToModel, float time, float weightScale, DObjAnimMat *rotTransArray, int *ignorePartBits )
{
	DObjAnimMat *totalRotTrans;
	int i;
	XAnimPart *childPart;
	float scale;
	int animPartIndex;
	int boneCount;
	int numframes;
	char *simpleQuatBits;
	XAnimTime animTime;

	numframes = parts->numframes;
	XAnim_SetTime(time, numframes, &animTime);
	scale = weightScale * 0.000030518509f;
	simpleQuatBits = parts->simpleQuatBits;
	boneCount = parts->boneCount;

	for ( i = 0; i < boneCount; ++i )
	{
		animPartIndex = animToModel[i];

		if ( ( 1 << (animPartIndex & 0x1F) ) & ignorePartBits[animPartIndex >> 5] )
			continue;

		totalRotTrans = &rotTransArray[animPartIndex];
		childPart = &parts->parts[i];

		if ( ( 1 << (i & 7) ) & simpleQuatBits[i >> 3] )
		{
			if ( childPart->quat )
				XAnim_CalcSimpleRotDuring_unsigned_short_(&animTime, childPart->quat, scale, &totalRotTrans->quat[2]);
			else
				totalRotTrans->quat[3] = totalRotTrans->quat[3] + weightScale;
		}
		else
		{
			XAnim_CalcRotDuring_unsigned_short_(&animTime, childPart->quat, scale, totalRotTrans->quat);
		}

		if ( childPart->trans )
			XAnim_CalcPosDuring_unsigned_short_(&animTime, childPart->trans, weightScale, totalRotTrans->trans);

		totalRotTrans->transWeight = totalRotTrans->transWeight + weightScale;
	}
}

void XAnim_GetTimeIndex_unsigned_char_(const XAnimTime *animTime, const unsigned char *indices, int tableSize, int *keyFrameIndex, float *keyFrameLerpFrac)
{
	int low;
	int index;
	int frameIndex;
	int high;

	index = (int)(animTime->time * (float)tableSize);
	frameIndex = animTime->frameIndex;

	if ( frameIndex < indices[index] )
	{
		low = 0;
		high = index;

		while ( 1 )
		{
			index = (low + high) / 2;

			if ( frameIndex < indices[index] )
			{
				high = index;
				continue;
			}

			if ( frameIndex < indices[index + 1] )
				break;

			low = index + 1;

			if ( frameIndex >= indices[--high] )
			{
				index = high;
				break;
			}
		}
	}
	else if ( frameIndex >= indices[index + 1] )
	{
		low = index + 1;
		high = tableSize;

		while ( 1 )
		{
			index = (low + high) / 2;

			if ( frameIndex < indices[index] )
			{
				high = index;
				low++;

				if ( frameIndex < indices[low] )
				{
					index = low - 1;
					break;
				}
			}
			else if ( frameIndex >= indices[index + 1] )
			{
				low = index + 1;
			}
			else
			{
				break;
			}
		}
	}

	*keyFrameLerpFrac = (animTime->frameFrac - (float)indices[index])
	                    / (float)(indices[index + 1] - indices[index]);
	*keyFrameIndex = index;
}

void XAnim_GetTimeIndexCompressed_unsigned_char(XAnimTime *animTime, const unsigned char *indices, int tableSize, int *keyFrameIndex, float *keyFrameLerpFrac)
{
	if ( tableSize < animTime->frameCount )
	{
		XAnim_GetTimeIndex_unsigned_char_(animTime, indices, tableSize, keyFrameIndex, keyFrameLerpFrac);
	}
	else
	{
		*keyFrameLerpFrac = animTime->frameFrac - (float)animTime->frameIndex;
		*keyFrameIndex = animTime->frameIndex;
	}
}

void XAnim_CalcRotDeltaDuringInternal_unsigned_char_(XAnimDeltaPartQuat *quat, const float time, int numFrames, float *rotDelta)
{
	XAnimTime animTime;
	int keyFrameIndex;
	float keyFrameLerpFrac;

	if ( !quat->size )
	{
		Short2CopyAsVec2(quat->u.frame0, rotDelta);
	}
	else
	{
		XAnim_SetTime(time, numFrames, &animTime);
		XAnim_GetTimeIndexCompressed_unsigned_char(
		    &animTime,
		    quat->u.frames.indices._1,
		    quat->size,
		    &keyFrameIndex,
		    &keyFrameLerpFrac);
		Short2LerpAsVec2(
		    quat->u.frames.frames[keyFrameIndex],
		    quat->u.frames.frames[keyFrameIndex + 1],
		    keyFrameLerpFrac,
		    rotDelta);
	}
}

void XAnim_CalcRotDeltaDuring_unsigned_char_(XAnimDeltaPart *animDelta, const float time, int numFrames, float *rotDelta)
{
	if ( animDelta->quat )
	{
		XAnim_CalcRotDeltaDuringInternal_unsigned_char_(animDelta->quat, time, numFrames, rotDelta);
	}
	else
	{
		rotDelta[0] = 0.0;
		rotDelta[1] = 32767.0;
	}
}

void XAnim_CalcPosDeltaDuringInternal_unsigned_char_(XAnimDeltaPartTrans *trans, const float time, int numFrames, float *posDelta)
{
	XAnimTime animTime;
	int keyFrameIndex;
	float keyFrameLerpFrac;

	if ( !trans->size )
	{
		VectorCopy(trans->u.frame0, posDelta);
	}
	else
	{
		XAnim_SetTime(time, numFrames, &animTime);
		XAnim_GetTimeIndexCompressed_unsigned_char(
		    &animTime,
		    trans->u.frames.indices._1,
		    trans->size,
		    &keyFrameIndex,
		    &keyFrameLerpFrac);
		Vec3LerpFrom(
		    trans->u.frames.frames[keyFrameIndex],
		    trans->u.frames.frames[keyFrameIndex + 1],
		    keyFrameLerpFrac,
		    posDelta);
	}
}

void XAnim_CalcPosDeltaDuring_unsigned_char_(XAnimDeltaPart *animDelta, const float time, int numFrames, float *posDelta)
{
	if ( animDelta->trans )
		XAnim_CalcPosDeltaDuringInternal_unsigned_char_(animDelta->trans, time, numFrames, posDelta);
	else
		VectorClear(posDelta);
}

void XAnim_GetTimeIndex_unsigned_short_(const XAnimTime *animTime, const unsigned short *indices, int tableSize, int *keyFrameIndex, float *keyFrameLerpFrac)
{
	int low;
	int index;
	int frameIndex;
	int high;

	index = (int)(animTime->time * (float)tableSize);
	frameIndex = animTime->frameIndex;

	if ( frameIndex < indices[index] )
	{
		low = 0;
		high = index;

		while ( 1 )
		{
			index = (low + high) / 2;

			if ( frameIndex < indices[index] )
			{
				high = index;
				continue;
			}

			if ( frameIndex < indices[index + 1] )
				break;

			low = index + 1;

			if ( frameIndex >= indices[--high] )
			{
				index = high;
				break;
			}
		}
	}
	else if ( frameIndex >= indices[index + 1] )
	{
		low = index + 1;
		high = tableSize;

		while ( 1 )
		{
			index = (low + high) / 2;

			if ( frameIndex < indices[index] )
			{
				high = index;
				low++;

				if ( frameIndex < indices[low] )
				{
					index = low - 1;
					break;
				}
			}
			else if ( frameIndex >= indices[index + 1] )
			{
				low = index + 1;
			}
			else
			{
				break;
			}
		}
	}

	*keyFrameLerpFrac = (animTime->frameFrac - (float)indices[index])
	                    / (float)(indices[index + 1] - indices[index]);
	*keyFrameIndex = index;
}

void XAnim_GetTimeIndexCompressed_unsigned_short_(XAnimTime *animTime, const unsigned short *indices, int tableSize, int *keyFrameIndex, float *keyFrameLerpFrac)
{
	if ( tableSize < animTime->frameCount )
	{
		XAnim_GetTimeIndex_unsigned_short_(animTime, indices, tableSize, keyFrameIndex, keyFrameLerpFrac);
	}
	else
	{
		*keyFrameLerpFrac = animTime->frameFrac - (float)animTime->frameIndex;
		*keyFrameIndex = animTime->frameIndex;
	}
}

void XAnim_CalcRotDeltaDuringInternal_unsigned_short_(XAnimDeltaPartQuat *quat, const float time, int numFrames, float *rotDelta)
{
	XAnimTime animTime;
	int keyFrameIndex;
	float keyFrameLerpFrac;

	if ( !quat->size )
	{
		Short2CopyAsVec2(quat->u.frame0, rotDelta);
	}
	else
	{
		XAnim_SetTime(time, numFrames, &animTime);
		XAnim_GetTimeIndexCompressed_unsigned_short_(
		    &animTime,
		    quat->u.frames.indices._2,
		    quat->size,
		    &keyFrameIndex,
		    &keyFrameLerpFrac);
		Short2LerpAsVec2(
		    quat->u.frames.frames[keyFrameIndex],
		    quat->u.frames.frames[keyFrameIndex + 1],
		    keyFrameLerpFrac,
		    rotDelta);
	}
}

void XAnim_CalcRotDeltaDuring_unsigned_short_(XAnimDeltaPart *animDelta, const float time, int numFrames, float *rotDelta)
{
	if ( animDelta->quat )
	{
		XAnim_CalcRotDeltaDuringInternal_unsigned_short_(animDelta->quat, time, numFrames, rotDelta);
	}
	else
	{
		rotDelta[0] = 0.0;
		rotDelta[1] = 32767.0;
	}
}

void XAnim_CalcPosDeltaDuringInternal_unsigned_short_(XAnimDeltaPartTrans *trans, const float time, int numFrames, float *posDelta)
{
	XAnimTime animTime;
	int keyFrameIndex;
	float keyFrameLerpFrac;

	if ( !trans->size )
	{
		VectorCopy(trans->u.frame0, posDelta);
	}
	else
	{
		XAnim_SetTime(time, numFrames, &animTime);
		XAnim_GetTimeIndexCompressed_unsigned_short_(
		    &animTime,
		    trans->u.frames.indices._2,
		    trans->size,
		    &keyFrameIndex,
		    &keyFrameLerpFrac);
		Vec3LerpFrom(
		    trans->u.frames.frames[keyFrameIndex],
		    trans->u.frames.frames[keyFrameIndex + 1],
		    keyFrameLerpFrac,
		    posDelta);
	}
}

void XAnim_CalcPosDeltaDuring_unsigned_short_(XAnimDeltaPart *animDelta, const float time, int numFrames, float *posDelta)
{
	if ( animDelta->trans )
		XAnim_CalcPosDeltaDuringInternal_unsigned_short_(animDelta->trans, time, numFrames, posDelta);
	else
		VectorClear(posDelta);
}

