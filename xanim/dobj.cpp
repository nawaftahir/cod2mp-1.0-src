#include "../qcommon/qcommon.h"
#include "../script/script_public.h"

static unsigned int g_empty;

const char *DObjGetBoneName(const DObj_s *obj, int boneIndex);





void DObjInit()
{
	int duplicatePartBits[5];

	memset(duplicatePartBits, 0, sizeof(duplicatePartBits));
	g_empty = SL_GetStringOfLen((const char *)duplicatePartBits, 0, 17, 12);
}

void DObjShutdown()
{
	if ( g_empty )
	{
		SL_RemoveRefToStringOfLen(g_empty, 17);
		g_empty = 0;
	}
}

void DObjAbort()
{
	g_empty = 0;
}

void DObjDumpInfo(const DObj_s *obj)
{
	int j;
	int i;
	int numBones;
	int numModels;
	int bones;
	byte *partName;
	XModel *model;

	if ( !obj )
	{
		Com_Printf("No Dobj\n");
		return;
	}

	Com_Printf("\nModels:\n");
	numModels = obj->numModels;
	bones = 0;

	for ( i = 0; i < numModels; ++i )
	{
		model = obj->models[i];
		Com_Printf("%d: '%s'\n", bones, model->name);
		bones += model->parts->numBones;
	}

	Com_Printf("\nBones:\n");
	numBones = obj->numBones;

	for ( j = 0; j < numBones; ++j )
	{
		Com_Printf("Bone %d: '%s'\n", j, DObjGetBoneName(obj, j));
	}

	if ( obj->duplicateParts )
	{
		Com_Printf("\nPart duplicates:\n");

		for ( partName = (byte *)(SL_ConvertToString(obj->duplicateParts) + 16); *partName; partName += 2 )
		{
			Com_Printf("%d ('%s') -> %d ('%s')\n", *partName - 1, DObjGetBoneName(obj, *partName - 1), partName[1] - 1, DObjGetBoneName(obj, partName[1] - 1));
		}
	}
	else
	{
		Com_Printf("\nNo part duplicates.\n");
	}

	Com_Printf("\n");
}

bool DObjIgnoreCollision( const DObj_s *obj, int modelIndex )
{
	return (obj->ignoreCollision & (1 << modelIndex)) != 0;
}

int DObjGetBoneIndexInternal(const DObj_s *obj, unsigned int name)
{
	int numModels;
	int i;
	int index;
	XModel *model;
	int offset;

	numModels = obj->numModels;
	index = 0;

	for ( i = 0; i < numModels; ++i )
	{
		model = obj->models[i];
		offset = XModelGetBoneIndex(model, name);

		if ( offset >= 0 )
			return index + offset;

		index += model->parts->numBones;
	}

	return -1;
}

void DObjCreateDuplicateParts(DObj_s *obj)
{
	int boneCount;
	int numBones;
	XModel *model;
	int boneIndex;
	int localBoneIndex;
	int boneIter;
	XBoneHierarchy *hierarchy;
	unsigned short *name;
	byte *duplicateParts;
	int len;
	bool bRootMeld;
	int index;
	XModelParts *modelParts;
	byte duplicatePartBits[0x444];

	duplicateParts = &duplicatePartBits[16];
	memset(duplicatePartBits, 0, 16);
	len = 0;
	numBones = obj->models[0]->parts->numBones;
	boneCount = 1;

	while ( boneCount < obj->numModels )
	{
		model = obj->models[boneCount];

		if ( obj->modelParents[boneCount] != 0xFF )
			goto next;

		modelParts = model->parts;
		hierarchy = modelParts->hierarchy;
		name = hierarchy->names;
		boneIter = modelParts->numBones;
		bRootMeld = 0;
		boneIndex = -1;

		for ( localBoneIndex = 0; localBoneIndex < boneIter; ++localBoneIndex )
		{
			boneIndex = DObjGetBoneIndexInternal(obj, name[localBoneIndex]);

			if ( boneIndex == numBones + localBoneIndex )
				continue;

			if ( !localBoneIndex )
				bRootMeld = 1;

			index = numBones + localBoneIndex;
			duplicateParts[len] = index + 1;
			((int *)duplicatePartBits)[index >> 5] |= 1 << (index & 0x1F);
			duplicateParts[++len] = boneIndex + 1;
			++len;
		}

		if ( !bRootMeld )
		{
			Com_Printf(
			    "WARNING: Attempting to meld model, but root part '%s' of model '%s' not found in model '%s' or any of its descendants\n",
			    SL_ConvertToString(name[0]),
			    model->name,
			    obj->models[0]->name);
		}

next:
		++boneCount;
		numBones += model->parts->numBones;
	}

	if ( len )
	{
		duplicateParts[len++] = 0;
		obj->duplicateParts = SL_GetStringOfLen((const char *)duplicatePartBits, 0, len + 16, 12);
	}
	else
	{
		obj->duplicateParts = g_empty;
	}
}

void DObjGetHierarchyBits(DObj_s *obj, int boneIndex, int *partBits)
{
	int numModels;
	int i;
	XModelParts_s *parts;
	const unsigned char *duplicateParts;
	byte *parentList;
	int localBoneIndex;
	const unsigned char *pos;
	int newBoneIndex;
	int startIndex[8];
	const int *duplicatePartBits;

	for ( i = 0; i < 4; ++i )
		partBits[i] = 0;

	numModels = obj->numModels;

	if ( !obj->duplicateParts )
		DObjCreateDuplicateParts(obj);

	duplicatePartBits = (const int *)SL_ConvertToString(obj->duplicateParts);
	duplicateParts = (const unsigned char *)(duplicatePartBits + 4);
	pos = duplicateParts;

	startIndex[0] = 0;

	for ( i = 0; ; startIndex[i] = newBoneIndex )
	{
		parts = obj->models[i]->parts;
		newBoneIndex = startIndex[i] + parts->numBones;

		if ( newBoneIndex > boneIndex )
			break;

		if ( ++i == numModels )
			return;
	}

	for ( parentList = parts->hierarchy->parentList; ; )
	{
		localBoneIndex = boneIndex - startIndex[i];

		while ( 1 )
		{
			partBits[boneIndex >> 5] |= 1 << (boneIndex & 0x1F);

			if ( !( duplicatePartBits[boneIndex >> 5] & ( 1 << ( boneIndex & 0x1F ) ) ) )
			{
				newBoneIndex = localBoneIndex - parts->numRootBones;

				if ( newBoneIndex >= 0 )
				{
					boneIndex -= parentList[newBoneIndex];
					break;
				}

				boneIndex = obj->modelParents[i];

				if ( boneIndex == 255 )
					return;
			}
			else
			{
				for ( pos = duplicateParts; boneIndex != *pos - 1; pos += 2 )
					;

				boneIndex = pos[1] - 1;
			}

		out:
			localBoneIndex = boneIndex - startIndex[--i];

			if ( localBoneIndex < 0 )
				goto out;

			parts = obj->models[i]->parts;
			parentList = parts->hierarchy->parentList;
		}
	}
}

void DObjCompleteHierarchyBits(DObj_s *obj, int *partBits)
{
	int numModels;
	int i;
	XModelParts_s *parts;
	const unsigned char *duplicateParts;
	byte *parentList;
	int localBoneIndex;
	const unsigned char *pos;
	int newBoneIndex;
	int startIndex[8];
	const int *duplicatePartBits;
	int boneIndex;

	boneIndex = obj->numBones - 1;
	numModels = obj->numModels;

	if ( !obj->duplicateParts )
		DObjCreateDuplicateParts(obj);

	duplicatePartBits = (const int *)SL_ConvertToString(obj->duplicateParts);
	duplicateParts = (const unsigned char *)(duplicatePartBits + 4);
	pos = duplicateParts;

	startIndex[0] = 0;

	for ( i = 0; ; startIndex[i] = newBoneIndex )
	{
		parts = obj->models[i]->parts;
		newBoneIndex = startIndex[i] + parts->numBones;

		if ( newBoneIndex > boneIndex )
			break;

		++i;
	}

	parentList = parts->hierarchy->parentList;

	while ( 1 )
	{
		localBoneIndex = boneIndex - startIndex[i];

		if ( localBoneIndex < 0 )
		{
			--i;

			if ( i < 0 )
				return;

			parts = obj->models[i]->parts;
			parentList = parts->hierarchy->parentList;
			continue;
		}

		if ( !(( 1 << (boneIndex & 0x1F) ) & partBits[boneIndex >> 5]) )
		{
			--boneIndex;
			continue;
		}

		if ( !( duplicatePartBits[boneIndex >> 5] & ( 1 << ( boneIndex & 0x1F ) ) ) )
		{
			newBoneIndex = localBoneIndex - parts->numRootBones;

			if ( newBoneIndex >= 0 )
			{
				newBoneIndex = boneIndex - parentList[newBoneIndex];
			}
			else
			{
				newBoneIndex = obj->modelParents[i];

				if ( newBoneIndex == 255 )
				{
					--boneIndex;
					continue;
				}
			}
		}
		else
		{
			for ( pos = duplicateParts; ; pos += 2 )
			{
				if ( boneIndex == *pos - 1 )
				{
					newBoneIndex = pos[1] - 1;
					break;
				}
			}
		}

		partBits[newBoneIndex >> 5] |= 1 << (newBoneIndex & 0x1F);
		--boneIndex;
	}
}

int DObjSkelIsBoneUpToDate(DObj_s *obj, int boneIndex)
{
	DSkel_t *skel = obj->skel;

	return (( 1 << (boneIndex & 0x1F) ) & skel->partBits.skel[boneIndex >> 5]) != 0;
}

int DObjSkelAreBonesUpToDate(const DObj_s *obj, int *partBits)
{
	int i;
	DSkel_t *skel;

	skel = obj->skel;

	for ( i = 0; i < 4; ++i )
	{
		if ( (partBits[i] & ~skel->partBits.skel[i]) != 0 )
			return 0;
	}

	return 1;
}

void DObjCalcSkel(DObj_s *obj, int *partBits)
{
	DObjAnimMat *parentMat;
	int i;
	int j;
	XModelParts_s *parts;
	byte *parentList;
	unsigned char parent;
	DObjAnimMat *Mat;
	float *trans;
	int controlPartBits[4];
	int ignorePartBits[4];
	int calcPartBits[4];
	short *quats;
	int boneIndex;
	int boneIndexHigh;
	int boneBit;
	int numModels;
	XBoneHierarchy *hierarchy;
	const unsigned char *duplicateParts;
	DObjAnimMat *childMat;
	const unsigned char *pos;
	int quatIndex;
	DSkel_t *skel;
	char bFinished;
	const int *savedDuplicatePartBits;

	skel = obj->skel;
	bFinished = 1;

	for ( i = 0; i < 4; ++i )
	{
		ignorePartBits[i] = ~partBits[i] | skel->partBits.skel[i];

		if ( ~ignorePartBits[i] )
			bFinished = 0;
	}

	if ( bFinished )
		return;

	if ( !obj->duplicateParts )
		DObjCreateDuplicateParts(obj);

	savedDuplicatePartBits = (const int *)SL_ConvertToString(obj->duplicateParts);
	pos = (const unsigned char *)(savedDuplicatePartBits + 4);

	for ( i = 0; i < 4; ++i )
	{
		skel->partBits.skel[i] |= partBits[i];
		controlPartBits[i] = skel->partBits.control[i] & ~ignorePartBits[i];
		calcPartBits[i] = ignorePartBits[i] | controlPartBits[i] | savedDuplicatePartBits[i];
	}

	for ( i = 0; i < 4; ++i )
		controlPartBits[i] |= ~calcPartBits[i];

	numModels = obj->numModels;
	Mat = &skel->Mat;
	parentMat = Mat;
	boneIndex = 0;
	duplicateParts = pos;

	for ( j = 0; j < numModels; ++j )
	{
		parts = obj->models[j]->parts;
		parent = obj->modelParents[j];

		if ( parent == 0xFF )
		{
			i = parts->numRootBones;

			while ( i )
			{
				boneIndexHigh = boneIndex >> 5;
				boneBit = 1 << (boneIndex & 0x1F);

				if ( (controlPartBits[boneIndexHigh] & boneBit) != 0 )
				{
					DObjCalcTransWeight(parentMat);
				}
				else if ( boneIndex == *duplicateParts - 1 )
				{
					duplicateParts += 2;

					if ( (ignorePartBits[boneIndexHigh] & boneBit) != 0 )
						goto next;

					quatIndex = *(duplicateParts - 1) - 1;
					childMat = &Mat[quatIndex];

					*parentMat = *childMat;
				}
			next:
				--i;
				++parentMat;
				++boneIndex;
			}
		}
		else
		{
			childMat = &Mat[parent];
			i = parts->numRootBones;

			while ( i )
			{
				boneIndexHigh = boneIndex >> 5;
				boneBit = 1 << (boneIndex & 0x1F);

				if ( (controlPartBits[boneIndexHigh] & boneBit) != 0 )
				{
					if ( (calcPartBits[boneIndexHigh] & boneBit) == 0 )
						QuatMultiplyReverseEquals(parentMat->quat, childMat->quat);
					else
						QuatMultiplyEquals(childMat->quat, parentMat->quat);

					DObjCalcTransWeight(parentMat);
					MatrixTransformVectorQuatTransEquals(parentMat->trans, childMat);
				}

				--i;
				++parentMat;
				++boneIndex;
			}
		}

		quats = parts->quats;
		trans = parts->trans;
		hierarchy = parts->hierarchy;
		parentList = hierarchy->parentList;
		i = parts->numBones - parts->numRootBones;

		while ( i )
		{
			boneIndexHigh = boneIndex >> 5;
			boneBit = 1 << (boneIndex & 0x1F);

			if ( (controlPartBits[boneIndexHigh] & boneBit) != 0 )
			{
				if ( (calcPartBits[boneIndexHigh] & boneBit) == 0 )
					QuatMultiplyReverseEquals(parentMat->quat, (parentMat - *parentList)->quat);
				else
					QuatMultiplyEquals((parentMat - *parentList)->quat, parentMat->quat);

				DObjCalcTransWeight(parentMat);
				VectorAdd(parentMat->trans, trans, parentMat->trans);
				MatrixTransformVectorQuatTransEquals(parentMat->trans, (parentMat - *parentList));
			}
			else if ( boneIndex == *duplicateParts - 1 )
			{
				duplicateParts += 2;

				if ( (ignorePartBits[boneIndexHigh] & boneBit) != 0 )
					goto nextbone;

				quatIndex = *(duplicateParts - 1) - 1;
				childMat = &Mat[quatIndex];

				*parentMat = *childMat;
			}
		nextbone:
			--i;
			++parentMat;
			quats += 4;
			trans += 3;
			++parentList;
			++boneIndex;
		}
	}
}

void DObjSetTree(DObj_s *obj, XAnimTree_s *tree)
{
	unsigned int infoOffset;
	unsigned int animTreeSize;
	byte *parent;
	byte childInfoIndex;

	obj->tree = tree;

	if ( !tree )
	{
		obj->animToModel = 0;
		return;
	}

	animTreeSize = tree->anims->size;
	infoOffset = animTreeSize * 2 + 8;
	obj->animToModel = (unsigned short *)((byte *)tree + infoOffset);
	parent = (byte *)obj->animToModel + animTreeSize * 2;
	childInfoIndex = *parent + 1;

	if ( childInfoIndex == 0 )
	{
		childInfoIndex = 1;
		memset(parent + 1, 0, animTreeSize);
	}

	*parent = childInfoIndex;
}

void DObjSetBounds( DObj *obj )
{
	int numModels;
	int i;
	vec3_t modelmins;
	vec3_t modelmaxs;
	vec3_t dobjmins;
	vec3_t dobjmaxs;

	numModels = obj->numModels;

	VectorSet(dobjmins, 0, 0, 0);
	VectorSet(dobjmaxs, 0, 0, 0);

	for ( i = 0; i < numModels; ++i )
	{
		if ( obj->models[i] )
		{
			XModelGetBounds(obj->models[i], modelmins, modelmaxs);

			VectorAdd(dobjmins, modelmins, dobjmins);
			VectorAdd(dobjmaxs, modelmaxs, dobjmaxs);
		}
	}

	VectorCopy(dobjmins, obj->mins);
	VectorCopy(dobjmaxs, obj->maxs);
}

void DObjCreate(DObjModel_s *dobjModels, unsigned int numModels, XAnimTree_s *tree, DObj *obj, unsigned int entnum)
{
	DObj *dobj;
	unsigned int i;
	int j;
	int numBones;
	int totalBones;
	XModel *model;
	const char *parentName;
	unsigned int name;
	int boneIndex;
	int boneIter;
	XBoneHierarchy *hierarchy;
	unsigned short *names;
	int modelIndex;
	DObjModel_s *models;
	XModelParts *modelParts;

	dobj = obj;

	dobj->skel = 0;
	dobj->timeStamp = 0;
	dobj->duplicateParts = 0;
	dobj->ignoreCollision = 0;

	DObjSetTree(dobj, tree);

	if ( tree )
		tree->entnum = entnum;

	modelIndex = 0;
	numBones = 0;

	models = dobjModels;

	for ( i = 0; i < numModels; ++i, ++models )
	{
		model = models->model;

		dobj->models[modelIndex] = model;
		dobj->modelParents[modelIndex] = -1;
		dobj->matOffset[modelIndex] = numBones;

		if ( models->ignoreCollision )
			dobj->ignoreCollision |= 1 << i;

		if ( !i )
			goto setmodel;

		modelParts = model->parts;
		hierarchy = modelParts->hierarchy;
		names = hierarchy->names;
		boneIter = modelParts->numBones;

		parentName = models->boneName;

		if ( parentName )
		{
			if ( !parentName[0] )
				goto setmodel;

			name = SL_FindString(parentName);

			if ( name )
			{
				for ( j = 0; j < modelIndex; ++j )
				{
					boneIndex = XModelGetBoneIndex(dobj->models[j], name);

					if ( boneIndex >= 0 )
					{
						dobj->modelParents[modelIndex] = dobj->matOffset[j] + boneIndex;
						goto setmodel;
					}
				}
			}

			Com_Printf(
			    "WARNING: Part '%s' not found in model '%s' or any of its descendants\n",
			    parentName,
			    dobj->models[0]->name);
		}
setmodel:
		if ( model )
		{
			totalBones = numBones + model->parts->numBones;

			if ( totalBones >= 128 )
			{
				Com_Error(ERR_DROP, "\x15" "dobj for xmodel '%s' has more than %d bones", dobj->models[0]->name, 127);
				break;
			}

			numBones = totalBones;
		}

		++modelIndex;
	}

	dobj->numModels = modelIndex;
	dobj->numBones = numBones;

	DObjSetBounds(dobj);
}

void DObjClone( const DObj *from, XAnimTree_s *tree, DObj *obj )
{
	DObj *to;

	to = obj;
	*to = *from;
	to->skel = 0;

	if ( to->duplicateParts && to->duplicateParts != g_empty )
		SL_AddRefToString(to->duplicateParts);

	DObjSetTree(to, tree);
}

void DObjFree(DObj *obj)
{
	XAnimTree_s *tree;

	tree = obj->tree;

	if ( tree )
	{
		obj->animToModel = 0;
		obj->tree = 0;
	}

	if ( obj->duplicateParts )
	{
		if ( obj->duplicateParts != g_empty )
		{
			SL_RemoveRefToStringOfLen(obj->duplicateParts, I_strlen(SL_ConvertToString(obj->duplicateParts) + 16) + 17);
		}

		obj->duplicateParts = 0;
	}
}

void DObjGetCreateParms( const DObj_s *obj, DObjModel_s *dobjModels, unsigned short *numModels, XAnimTree_s **tree, unsigned short *entnum )
{
	DObjModel_s *model;
	int i;
	int j;
	int boneIndex;
	unsigned short *boneNames;

	*numModels = obj->numModels;
	*tree = obj->tree;
	*entnum = obj->tree ? obj->tree->entnum : 0;

	for ( i = 0, model = dobjModels; i < obj->numModels; i++, model++ )
	{
		model->model = obj->models[i];
		model->boneName = 0;
		model->ignoreCollision = ( obj->ignoreCollision & (1 << i) ) ? qtrue : qfalse;

		if ( obj->modelParents[i] != 255 )
		{
			for ( j = i - 1; j >= 0; j-- )
			{
				if ( obj->modelParents[i] < obj->matOffset[j] )
					continue;

				boneIndex = obj->modelParents[i] - obj->matOffset[j];
				boneNames = XModelBoneNames(obj->models[j]);
				model->boneName = SL_ConvertToString(boneNames[boneIndex]);
				break;
			}
		}
	}
}

int DObjGetAllocSkelSize(const DObj_s *obj)
{
	return sizeof(DObjAnimMat) * obj->numBones + sizeof(DSkelPartBits_s);
}

int DObjSkelExists(DObj_s *obj, int timeStamp)
{
	if ( obj->timeStamp == timeStamp )
		return obj->skel != 0;

	obj->skel = 0;
	return 0;
}

int DObjSkelIsValid( const DObj *obj, int timeStamp )
{
	return obj->timeStamp == timeStamp && obj->skel;
}

void DObjSkelClear( DObj_s *obj )
{
	obj->timeStamp = 0;
	obj->skel = 0;
}

void DObjCreateSkel(DObj_s *obj, DSkel_t *skel, int time)
{
	int i;
	DSkel_t *pSkel;

	pSkel = skel;
	obj->skel = pSkel;
	obj->timeStamp = time;

	for ( i = 0; i < 4; ++i )
	{
		pSkel->partBits.anim[i] = 0;
		pSkel->partBits.control[i] = 0;
		pSkel->partBits.skel[i] = 0;
	}
}

int DObjGetNumModels( const DObj_s *obj )
{
	return obj->numModels;
}

XModel *DObjGetModel(const DObj_s *obj, int modelIndex)
{
	return obj->models[modelIndex];
}

void DObjGetBounds(const DObj_s *obj, float *mins, float *maxs)
{
	VectorCopy(obj->mins, mins);
	VectorCopy(obj->maxs, maxs);
}

DObjAnimMat* DObjGetRotTransArray(const DObj_s *obj)
{
	if ( obj->skel )
		return &obj->skel->Mat;
	else
		return 0;
}

int DObjGetMatOffset( const DObj_s *obj, int modelIndex )
{
	return obj->matOffset[modelIndex];
}

void DObjGetBoneInfo( const DObj *obj, XBoneInfo **boneInfo )
{
	int numBones;
	int j;
	int i;
	XModel *model;
	XModelParts *parts;

	for ( i = 0; i < obj->numModels; i++ )
	{
		model = obj->models[i];
		parts = model->parts;
		numBones = parts->numBones;

		for ( j = 0; j < numBones; j++, boneInfo++ )
			*boneInfo = &model->boneInfo[j];
	}
}

int DObjSetRotTransIndex(const DObj_s *obj, const int *partBits, int boneIndex)
{
	int boneIndexHigh;
	int boneIndexLow;
	DSkel_t *skel;

	boneIndexHigh = boneIndex >> 5;
	boneIndexLow = 1 << (boneIndex & 0x1F);

	if ( (partBits[boneIndexHigh] & boneIndexLow) == 0 )
		return 0;

	skel = obj->skel;

	if ( (skel->partBits.skel[boneIndexHigh] & boneIndexLow) != 0 )
		return 0;

	skel->partBits.anim[boneIndexHigh] |= boneIndexLow;

	return 1;
}

int DObjSetControlRotTransIndex(const DObj_s *obj, const int *partBits, int boneIndex)
{
	int boneIndexHigh;
	int boneIndexLow;
	DSkel_t *skel;

	boneIndexHigh = boneIndex >> 5;
	boneIndexLow = 1 << (boneIndex & 0x1F);

	if ( (partBits[boneIndexHigh] & boneIndexLow) == 0 )
		return 0;

	skel = obj->skel;

	if ( (skel->partBits.skel[boneIndexHigh] & boneIndexLow) != 0 )
		return 0;

	skel->partBits.control[boneIndexHigh] |= boneIndexLow;
	skel->partBits.anim[boneIndexHigh] |= boneIndexLow;

	return 1;
}

int DObjGetSurfaceCount( const DObj *obj, const char *lods )
{
	int count;
	int i;
	XModelSurfs *surfs;
	XModelLodInfo *lodInfo;

	count = 0;

	for ( i = obj->numModels - 1; i >= 0; i-- )
	{
		if ( lods[i] < 0 )
			continue;

		lodInfo = &obj->models[i]->lodInfo[lods[i]];
		surfs = lodInfo->surfs;

		if ( !surfs )
			continue;

		count += lodInfo->numsurfs;
	}

	return count;
}

void *DObjGetModelSurface( const DObj *obj, int modelIndex, int surfIndex, int lod )
{
	return obj->models[modelIndex]->lodInfo[lod].surfs->surf[surfIndex];
}

const char *DObjGetModelSurfaceName( const DObj *obj, int modelIndex, int surfIndex, int lod )
{
	unsigned short name;

	name = obj->models[modelIndex]->lodInfo[lod].surfNames[surfIndex];

	if ( name )
		return SL_ConvertToString(name);

	return "DEFAULT";
}

int DObjGetSurfaceList( const DObj *obj, DObjSurfRef *list, unsigned int *surfPartBits, const char *lods )
{
	int numModels;
	int surfIndex;
	int modelIndex;
	int numsurfs;
	XModelSurfs *surfs;
	int bitOffset;
	int k;
	unsigned int *partBits;
	int maxWord;
	XModel *model;
	int word;
	int shift;
	int invShift;
	int startWord;
	int total;
	int j;
	XSurface **surfaces;
	XModelLodInfo *lodInfo;
	int *modelPartBits;

	memset(surfPartBits, 0, 16);
	numModels = obj->numModels;
	total = 0;

	for ( modelIndex = 0; modelIndex < numModels; modelIndex++ )
	{
		if ( lods[modelIndex] < 0 )
			continue;

		model = obj->models[modelIndex];
		lodInfo = &model->lodInfo[lods[modelIndex]];
		surfs = lodInfo->surfs;

		if ( !surfs )
			continue;

		maxWord = (model->parts->numBones - 1) >> 5;
		numsurfs = lodInfo->numsurfs;

		if ( total + numsurfs > 64 )
		{
			Com_Printf("ERROR: models with more than %i total surfaces\n", 64);

			for ( j = 0; j < numModels; j++ )
			{
				model = DObjGetModel(obj, j);
				Com_Printf("  model '%s' lod %i has %i surfaces\n", XModelGetName(model), lods[j],
				           XModelGetSurfaces(model, &surfaces, lods[j], &modelPartBits));
			}

			Com_Error(ERR_DROP, "Max surfs exceeded - see console for details");
		}

		for ( surfIndex = 0; surfIndex < numsurfs; surfIndex++, total++ )
		{
			list[total].surfIndex = surfIndex;
			list[total].modelIndex = modelIndex;
		}

		bitOffset = obj->matOffset[modelIndex];
		partBits = (unsigned int *)surfs->partBits;
		startWord = bitOffset >> 5;
		shift = bitOffset & 31;

		if ( shift )
		{
			invShift = 32 - shift;
			surfPartBits[startWord] |= partBits[0] << shift;
			word = startWord + 1;

			for ( k = 0; k < maxWord; k++ )
			{
				surfPartBits[word] |= (partBits[k] >> invShift) | (partBits[k + 1] << shift);
				word++;
			}

			surfPartBits[word] |= partBits[k] >> invShift;
		}
		else
		{
			word = startWord;

			for ( k = 0; k <= maxWord; k++ )
			{
				surfPartBits[word] |= partBits[k];
				word++;
			}
		}
	}

	return total;
}

int DObjGetBoneIndex(const DObj_s *obj, unsigned int name)
{
	int index;

	index = DObjGetBoneIndexInternal(obj, name);
	return index;
}

const char *DObjGetBoneName(const DObj_s *obj, int boneIndex)
{
	int numModels;
	int i;
	int offset;
	XModel *model;
	int numBones;
	unsigned short *names;
	int localIndex;
	XBoneHierarchy *hierarchy;
	XModelParts *parts;

	numModels = obj->numModels;
	offset = 0;

	for ( i = 0; i < numModels; i++ )
	{
		model = obj->models[i];
		parts = model->parts;
		hierarchy = parts->hierarchy;
		names = hierarchy->names;
		numBones = parts->numBones;
		localIndex = boneIndex - offset;

		if ( localIndex < numBones )
			return SL_ConvertToString(names[localIndex]);

		offset += numBones;
	}

	return NULL;
}

int DObjBad( const DObj *obj )
{
	int i;

	for ( i = obj->numModels - 1; i >= 0; i-- )
	{
		if ( XModelBad(obj->models[i]) )
			return 1;
	}

	return 0;
}

int DObjNumBones(const DObj *obj)
{
	return obj->numBones;
}

void InvMatrixTransformVectorQuatTrans(const float *in, const DObjAnimMat *mat, float *out)
{
	float temp[3];
	float axis[3][3];

	VectorSubtract(in, mat->trans, temp);
	ConvertQuatToMat(mat, axis);

	out[0] = temp[0] * axis[0][0] + temp[1] * axis[0][1] + temp[2] * axis[0][2];
	out[1] = temp[0] * axis[1][0] + temp[1] * axis[1][1] + temp[2] * axis[1][2];
	out[2] = temp[0] * axis[2][0] + temp[1] * axis[2][1] + temp[2] * axis[2][2];
}

void DObjGeomTraceline(DObj_s *obj, float *localStart, float *localEnd, int contentmask, DObjTrace_s *results)
{
	trace_t trace;
	XModel *model;
	XModelParts_s *parts;
	DObjAnimMat *pose;
	int i;
	unsigned short *name;
	int partIndex;
	XBoneHierarchy *hierarchy;

	results->partName = 0;
	results->partGroup = 0;
	trace.fraction = results->fraction;
	trace.surfaceFlags = 0;
	VectorClear(trace.normal);

	pose = DObjGetRotTransArray(obj);

	if ( pose )
	{
		for ( i = 0; i < obj->numModels; ++i )
		{
			model = obj->models[i];
			parts = model->parts;
			hierarchy = parts->hierarchy;
			name = hierarchy->names;
			partIndex = XModelTraceLine(model, &trace, pose, localStart, localEnd, contentmask);

			if ( partIndex >= 0 )
				results->partName = name[partIndex];

			pose += parts->numBones;
		}
	}

	results->fraction = trace.fraction;
	results->surfaceflags = trace.surfaceFlags;

	VectorCopy(trace.normal, results->normal);
}

int DObjGetModelLodForDist(const DObj_s *obj, int modelIndex, float dist)
{
	return XModelGetLodForDist(obj->models[modelIndex], dist);
}

float DObjGetMaxLodOutDist(const DObj_s *obj)
{
	int i;
	float dist;
	float maxDist;

	maxDist = 0;

	for ( i = 0; i < obj->numModels; ++i )
	{
		dist = XModelGetLodOutDist(obj->models[i]);

		if ( maxDist < dist )
			maxDist = dist;
	}

	return maxDist;
}

int DObjHasContents(DObj_s *obj, int contentmask)
{
	int i;

	for ( i = 0; i < obj->numModels; ++i )
	{
		if ( (contentmask & XModelGetContents(obj->models[i])) != 0 )
			return 1;
	}

	return 0;
}

void DObjSetLocalTagInternal( const DObj_s *obj, const float *trans, const float *angles, int boneIndex )
{
	DObjAnimMat *rotTrans;
	float unused;
	vec2_t yaw;
	vec2_t roll;
	vec2_t pitch;
	vec4_t tempQuat;

	rotTrans = DObjGetRotTransArray(obj);

	if ( !rotTrans )
		return;

	rotTrans += boneIndex;

	if ( angles )
	{
		FastSinCos(angles[1] * 0.0087266462f, &yaw[0], &yaw[1]);
		FastSinCos(angles[0] * 0.0087266462f, &pitch[0], &pitch[1]);
		FastSinCos(angles[2] * 0.0087266462f, &roll[0], &roll[1]);

		tempQuat[0] = -pitch[0] * yaw[0];
		tempQuat[1] = pitch[0] * yaw[1];
		tempQuat[2] = pitch[1] * yaw[0];
		tempQuat[3] = pitch[1] * yaw[1];

		rotTrans->quat[0] = roll[0] * tempQuat[3] + roll[1] * tempQuat[0];
		rotTrans->quat[1] = roll[1] * tempQuat[1] + roll[0] * tempQuat[2];
		rotTrans->quat[2] = -roll[0] * tempQuat[1] + roll[1] * tempQuat[2];
		rotTrans->quat[3] = roll[1] * tempQuat[3] - roll[0] * tempQuat[0];
	}
	else
	{
		Vector4Set(rotTrans->quat, 0, 0, 0, 1);
	}

	rotTrans->transWeight = 0;
	VectorCopy(trans, rotTrans->trans);
}

qboolean DObjSetControlTagAngles(const DObj_s *obj, int *partBits, unsigned int boneIndex, float *angles)
{
	int index;

	index = DObjGetBoneIndex(obj, boneIndex);

	if ( index < 0 )
		return false;

	if ( !DObjSetControlRotTransIndex(obj, partBits, index) )
		return false;

	DObjSetLocalTagInternal(obj, vec3_origin, angles, index);
	return true;
}

qboolean DObjSetLocalTag(const DObj_s *obj, int *partBits, unsigned int boneIndex, const float *trans, const float *angles)
{
	int index;

	index = DObjGetBoneIndex(obj, boneIndex);

	if ( index < 0 )
	{
		return false;
	}

	if ( !DObjSetRotTransIndex(obj, partBits, index) )
	{
		return false;
	}

	DObjSetLocalTagInternal(obj, trans, angles, index);
	return true;
}

qboolean DObjSetLocalBoneIndex(const DObj_s *obj, int *partBits, int boneIndex, const float *trans, const float *angles)
{
	if ( !DObjSetRotTransIndex(obj, partBits, boneIndex) )
		return false;

	DObjSetLocalTagInternal(obj, trans, angles, boneIndex);
	return true;
}

// retail 0x485fc0, ri.DObjSetModel
void DObjSetModel(DObj_s *obj, XModel *model)
{
	obj->skel = &model->parts->skel;
	obj->numBones = (byte)model->parts->numBones;
	obj->models[0] = model;
}

XAnimTree_s* DObjGetTree(const DObj_s *obj)
{
	return obj->tree;
}
void DObjTraceline(DObj_s *obj, float *start, float *end, unsigned char *priorityMap, DObjTrace_s *trace)
{
	vec3_t delta;
	float invL2;
	XModel *model;
	XModelParts_s *parts;
	float d2;
	int numBones;
	DObjAnimMat *boneMatrix;
	DObjAnimMat *hitBoneMatrix;
	float axis[3][3];
	vec3_t center;
	vec3_t startOffset;
	vec3_t enfOffset;
	float sphereFraction;
	vec3_t offset;
	float diff2;
	vec3_t localStart;
	vec3_t localEnd;
	float enterFrac;
	float fraction;
	float solidHitFrac;
	float dist1;
	float dist2;
	float dist;
	int i;
	float *bounds;
	char bStartSolid;
	char bEndSolid;
	float sign;
	int localBoneIndex;
	float hitSign;
	int hitT;
	int traceHitT;
	int globalBoneIndex;
	DSkel_t *skel;
	int modelIter;
	unsigned int lowestPriority;
	unsigned int currentPriority;
	unsigned short *name;
	const unsigned char *pos;
	XBoneInfo *boneInfo;
	int ignoreCollision;
	unsigned char parentIndex;
	XBoneHierarchy *hierarchy;
	unsigned short classification;
	float deltaLengthSq;
	unsigned short classificationArray[128];

	trace->surfaceflags = 0;
	trace->partName = 0;
	trace->partGroup = 0;
	VectorClear(trace->normal);
	VectorSubtract(end, start, delta);
	deltaLengthSq = VectorLengthSquared(delta);

	if ( deltaLengthSq == 0.0f )
		return;

	boneMatrix = DObjGetRotTransArray(obj);

	if ( !boneMatrix )
		return;

	invL2 = 1.0f / deltaLengthSq;
	lowestPriority = 2;
	skel = obj->skel;
	pos = (const unsigned char *)(SL_ConvertToString(obj->duplicateParts) + 16);
	globalBoneIndex = 0;
	hitT = -1;
	traceHitT = -1;
	hitSign = 0.0f;
	hitBoneMatrix = 0;
	solidHitFrac = trace->fraction;
	modelIter = 0;
start:
	if ( modelIter >= obj->numModels )
		goto done;

	{
		model = obj->models[modelIter];
		parts = model->parts;
		hierarchy = parts->hierarchy;
		;
		numBones = parts->numBones;
		ignoreCollision = obj->ignoreCollision & (1 << modelIter);
		localBoneIndex = 0;

		while ( 1 )
		{
			if ( localBoneIndex >= numBones )
			{
				break;
			}

			classification = parts->partClassification[localBoneIndex];
			currentPriority = priorityMap[classification];

			if ( globalBoneIndex == *pos - 1 )
			{
				pos += 2;

				if ( currentPriority == 1 )
				{
					classification = classificationArray[*(pos - 1) - 1];
					currentPriority = priorityMap[classification];
				}
			}
			else if ( currentPriority == 1 )
			{
				if ( localBoneIndex < parts->numRootBones )
				{
					parentIndex = obj->modelParents[modelIter];

					classification = parentIndex != 0xFF ? classificationArray[parentIndex] : 0;
				}
				else
				{
					classification = classificationArray[globalBoneIndex
					                                     - hierarchy->parentList[localBoneIndex - parts->numRootBones]];
				}

				currentPriority = priorityMap[classification];
			}

			classificationArray[globalBoneIndex] = classification;

			if ( ignoreCollision )
				goto out;

			boneInfo = &model->boneInfo[localBoneIndex];

			if ( boneInfo->radiusSquared == 0.0f )
				goto out;
			if ( lowestPriority > currentPriority )
				goto out;

			{
				MatrixTransformVectorQuatTrans(boneInfo->offset, boneMatrix, center);
				VectorSubtract(start, center, startOffset);
				sphereFraction = -DotProduct(startOffset, delta) * invL2;

				if ( sphereFraction < 1.0f )
				{
					if ( sphereFraction > 0.0f )
					{
						VectorMA(startOffset, sphereFraction, delta, offset);
						d2 = VectorLengthSquared(offset);
					}
					else
					{
						d2 = VectorLengthSquared(startOffset);
					}
				}
				else
				{
					VectorSubtract(end, center, enfOffset);
					d2 = VectorLengthSquared(enfOffset);
				}

				diff2 = boneInfo->radiusSquared - d2;

				if ( diff2 <= 0.0f )
					goto out;
				if ( lowestPriority == currentPriority
				        && (float)(sphereFraction - sqrtf(diff2 * invL2)) >= trace->fraction )
					goto out;

				{
					InvMatrixTransformVectorQuatTrans(start, boneMatrix, localStart);
					InvMatrixTransformVectorQuatTrans(end, boneMatrix, localEnd);
					enterFrac = 0.0f;

					if ( lowestPriority == currentPriority )
						fraction = trace->fraction;
					else
						fraction = solidHitFrac;

					bStartSolid = 1;
					bEndSolid = 1;
					sign = -1.0;

					for ( bounds = boneInfo->bounds[0]; ; bounds += 3 )
					{
						for ( i = 0; i < 3; ++i )
						{
							dist1 = (localStart[i] - bounds[i]) * sign;
							dist2 = (localEnd[i] - bounds[i]) * sign;

							if ( dist1 > 0.0f )
							{
								if ( dist2 > 0.0f )
									goto out;

								bStartSolid = 0;
								dist = dist1 - dist2;

								if ( dist1 > enterFrac * dist )
								{
									enterFrac = dist1 / dist;
									if ( enterFrac >= fraction )
										goto out;

									hitSign = sign;
									hitT = i;
								}
							}
							else
							{
								if ( dist2 > 0.0f )
								{
									bEndSolid = 0;
									dist = dist1 - dist2;

									if ( dist1 > fraction * dist )
									{
										fraction = dist1 / dist;

										if ( enterFrac >= fraction )
											goto out;
									}
								}
							}
						}

						if ( sign == 1.0f )
							break;

						sign = 1.0;
					}

					if ( bStartSolid )
					{
						if ( bEndSolid == 0 )
							goto out;
						if ( Dot2Product(delta, start) > 0.0f )
							goto out;

						trace->fraction = 0.0f;
						trace->partName = hierarchy->names[localBoneIndex];
						trace->partGroup = classification;

						if ( delta[0] != 0.0f || delta[1] != 0.0f )
						{
							Vector2Copy(start, trace->normal);
							Vec2Normalize(trace->normal);
						}
						else
						{
							trace->normal[2] = -I_sgn(delta[2]);
						}
						return;
					}
					else
					{
						if ( lowestPriority == currentPriority )
						{
							if ( enterFrac >= trace->fraction )
								goto out;
						}
						else
						{
							lowestPriority = currentPriority;
						}

						trace->fraction = enterFrac;
						trace->partName = hierarchy->names[localBoneIndex];
						trace->partGroup = classification;
						traceHitT = hitT;
						hitBoneMatrix = boneMatrix;
					}
				}
			}
out:
			++localBoneIndex;
			++boneMatrix;
			++globalBoneIndex;
		}

		++modelIter;
		goto start;
	}

done:
	if ( hitBoneMatrix )
	{
		ConvertQuatToMat(hitBoneMatrix, axis);
		VectorScale(axis[traceHitT], hitSign, trace->normal);
	}
}
void DObjInitFromModel( DObj *obj, XModel *model )
{
	obj->skel = &model->parts->skel;
	obj->numBones = model->parts->numBones;
	obj->models[0] = model;
}
