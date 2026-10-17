#include "../qcommon/qcommon.h"
#include "../script/script_public.h"

#define XMODEL_VERSION 20

void XModelConsumeQuat(const unsigned char **pos, short *out)
{
	int unused[2];
	int w;
	int z;
	int y;
	int x;
	int temp;

	out[0] = XModelDataReadShort(pos).value;
	out[1] = XModelDataReadShort(pos).value;
	out[2] = XModelDataReadShort(pos).value;

	x = out[0];
	y = out[1];
	z = out[2];

	temp = 1073676289 - (x * x + y * y + z * z);
	w = temp > 0 ? (int)floor((double)(0.5f + (float)sqrt((double)temp))) : 0;

	out[3] = w;
}

// Retail inlines QuatMultiply here (calls it only from QuatDiffAxisLengthSq).
static __forceinline void QuatMultiplyInl(const float *in1, const float *in2, float *out)
{
	out[0] = in1[0] * in2[3] + in1[3] * in2[0] + in1[2] * in2[1] - in1[1] * in2[2];
	out[1] = in1[1] * in2[3] - in1[2] * in2[0] + in1[3] * in2[1] + in1[0] * in2[2];
	out[2] = in1[2] * in2[3] + in1[1] * in2[0] - in1[0] * in2[1] + in1[3] * in2[2];
	out[3] = in1[3] * in2[3] - in1[0] * in2[0] - in1[1] * in2[1] - in1[2] * in2[2];
}

// Retail XModelCalcBasePose is /Op (store-reload on every int->float convert).
#pragma optimize("p", on)
void XModelCalcBasePose(XModelParts *modelParts)
{
	int numRootBones;
	short *quats;
	float *trans;
	XBoneHierarchy *hierarchy;
	int numBones;
	DObjAnimMat *quatTrans;
	byte *parentList;
	vec4_t tempQuat;

	hierarchy = modelParts->hierarchy;
	parentList = hierarchy->parentList;
	numBones = modelParts->numBones;
	quats = modelParts->quats;
	trans = modelParts->trans;
	quatTrans = &modelParts->skel.Mat;
	numRootBones = modelParts->numRootBones;

	while ( numRootBones )
	{
		VectorClear(quatTrans->quat);
		quatTrans->quat[3] = 1.0f;
		VectorClear(quatTrans->trans);
		quatTrans->transWeight = 2.0f;
		--numRootBones;
		++quatTrans;
	}

	numRootBones = numBones - modelParts->numRootBones;

	while ( numRootBones )
	{
		tempQuat[0] = (float)quats[0] * 0.000030518509f;
		tempQuat[1] = (float)quats[1] * 0.000030518509f;
		tempQuat[2] = (float)quats[2] * 0.000030518509f;
		tempQuat[3] = (float)quats[3] * 0.000030518509f;

		QuatMultiplyInl(tempQuat, (quatTrans - *parentList)->quat, quatTrans->quat);
		DObjCalcTransWeight(quatTrans);
		MatrixTransformVectorQuatTrans(trans, quatTrans - *parentList, quatTrans->trans);

		--numRootBones;
		quats += 4;
		trans += 3;
		++quatTrans;
		++parentList;
	}

	memset(&modelParts->skel, 255, 0x10u);
	memset(modelParts->skel.partBits.skel, 255, sizeof(modelParts->skel.partBits.skel));
}

#pragma optimize("p", off)
extern void FS_FreeFileInl(void *buffer);

XModelParts *XModelPartsLoadFile(XModel *model, const char *name, void *(*Alloc)(int))
{
	XModelParts *modelParts;
	char filename[64];
	byte *buf;
	const unsigned char *pos;
	XBoneHierarchy *boneHierarchy;
	unsigned short *boneNames;
	byte *parentList;
	short numBones;
	short numRootBones;
	short numChildBones;
	int i;
	float *trans;
	int len;
	int parentOffset;
	short version;
	short *quats;
	int size;
	int fileSize;

	if ( Com_sprintf(filename, sizeof(filename), "xmodelparts/%s", name) < 0 )
	{
		Com_Printf("^1ERROR: filename '%s' too long\n", filename);
		return NULL;
	}

	fileSize = FS_ReadFile(filename, (void **)&buf);

	if ( fileSize < 0 )
	{
		Com_Printf("^1ERROR: xmodelparts '%s' not found\n", name);
		return NULL;
	}

	if ( fileSize == 0 )
	{
		Com_Printf("^1ERROR: xmodelparts '%s' has 0 length\n", name);
		FS_FreeFile(buf);
		return NULL;
	}

	pos = buf;
	version = XModelDataReadShort(&pos).value;

	if ( version != XMODEL_VERSION )
	{
		FS_FreeFile(buf);
		Com_Printf("^1ERROR: xmodelparts '%s' out of date (version %d, expecting %d).\n", name, version, XMODEL_VERSION);
		return NULL;
	}

	numChildBones = XModelDataReadShort(&pos).value;
	numRootBones = XModelDataReadShort(&pos).value;
	numBones = numChildBones + numRootBones;

	size = sizeof(*boneNames) * numBones;
	boneNames = (unsigned short *)Alloc(size);
	model->memUsage += size;

	if ( numBones >= 128 )
	{
		FS_FreeFile(buf);
		Com_Printf("^1ERROR: xmodel '%s' has more than %d bones\n", name, 127);
		return NULL;
	}

	size = numChildBones + sizeof(XBoneHierarchy) - 1;
	boneHierarchy = (XBoneHierarchy *)Alloc(size);
	model->memUsage += size;
	boneHierarchy->names = boneNames;
	parentList = boneHierarchy->parentList;

	size = sizeof(DObjAnimMat) * numBones + sizeof(XModelParts) - sizeof(DObjAnimMat);
	modelParts = (XModelParts *)Alloc(size);
	model->memUsage += size;
	modelParts->hierarchy = boneHierarchy;

	if ( numChildBones )
	{
		size = 4 * sizeof(short) * numChildBones;
		modelParts->quats = (short *)Alloc(size);
		model->memUsage += size;

		size = 4 * sizeof(float) * numChildBones;
		modelParts->trans = (float *)Alloc(size);
		model->memUsage += size;
	}
	else
	{
		modelParts->quats = NULL;
		modelParts->trans = NULL;
	}

	size = numBones;
	modelParts->partClassification = (byte *)Alloc(size);
	model->memUsage += size;

	modelParts->numBones = numBones;
	modelParts->numRootBones = numRootBones;

	quats = modelParts->quats;
	trans = modelParts->trans;

	for ( i = numRootBones; i < numBones; )
	{
		parentOffset = *pos++;
		*parentList = i - parentOffset;
		trans[0] = XModelDataReadFloat(&pos);
		trans[1] = XModelDataReadFloat(&pos);
		trans[2] = XModelDataReadFloat(&pos);
		XModelConsumeQuat(&pos, quats);
		++i;
		quats += 4;
		trans += 3;
		++parentList;
	}

	for ( i = 0; i < numBones; ++i )
	{
		len = I_strlen((const char *)pos) + 1;
		boneNames[i] = SL_GetStringOfLen((const char *)pos, 0, len, 10);
		pos += len;
	}

	memcpy(modelParts->partClassification, pos, numBones);
	pos += numBones;

	FS_FreeFile(buf);
	XModelCalcBasePose(modelParts);

	return modelParts;
}

bool XModelLoadConfigFile(const char *name, const unsigned char **pos, XModelConfig *config)
{
	int i;
	short version;

	version = XModelDataReadShort(pos).value;

	if ( version != XMODEL_VERSION )
	{
		Com_Printf("^1ERROR: xmodel '%s' out of date (version %d, expecting %d).\n", name, version, XMODEL_VERSION);
		return false;
	}

	config->flags = **pos;
	++*pos;

	config->mins[0] = XModelDataReadFloat(pos);
	config->mins[1] = XModelDataReadFloat(pos);
	config->mins[2] = XModelDataReadFloat(pos);
	config->maxs[0] = XModelDataReadFloat(pos);
	config->maxs[1] = XModelDataReadFloat(pos);
	config->maxs[2] = XModelDataReadFloat(pos);

	for ( i = 0; i <= 3; ++i )
	{
		config->entries[i].dist = XModelDataReadFloat(pos);
		strcpy(config->entries[i].filename, (const char *)*pos);
		*pos += strlen((const char *)*pos) + 1;
	}

	config->collLod = XModelDataReadInt(pos);

	return true;
}

void XModelLoadCollData(const unsigned char **pos, XModel *model, void *(*AllocColl)(int), const char *name)
{
	XModelCollSurf *surf;
	XModelCollTri_s *tri;
	int i;
	int j;

	model->numCollSurfs = XModelDataReadInt(pos);

	if ( !model->numCollSurfs )
		return;

	model->collSurfs = (XModelCollSurf *)AllocColl(sizeof(XModelCollSurf) * model->numCollSurfs);

	for ( i = 0; i < model->numCollSurfs; i++ )
	{
		surf = &model->collSurfs[i];
		surf->numCollTris = XModelDataReadInt(pos);
		surf->collTris = (XModelCollTri_s *)AllocColl(sizeof(XModelCollTri_s) * surf->numCollTris);

		for ( j = 0; j < surf->numCollTris; j++ )
		{
			tri = &surf->collTris[j];
			tri->plane[0] = XModelDataReadFloat(pos);
			tri->plane[1] = XModelDataReadFloat(pos);
			tri->plane[2] = XModelDataReadFloat(pos);
			tri->plane[3] = XModelDataReadFloat(pos);
			tri->svec[0] = XModelDataReadFloat(pos);
			tri->svec[1] = XModelDataReadFloat(pos);
			tri->svec[2] = XModelDataReadFloat(pos);
			tri->svec[3] = XModelDataReadFloat(pos);
			tri->tvec[0] = XModelDataReadFloat(pos);
			tri->tvec[1] = XModelDataReadFloat(pos);
			tri->tvec[2] = XModelDataReadFloat(pos);
			tri->tvec[3] = XModelDataReadFloat(pos);
		}

		surf->mins[0] = XModelDataReadFloat(pos) - 0.001f;
		surf->mins[1] = XModelDataReadFloat(pos) - 0.001f;
		surf->mins[2] = XModelDataReadFloat(pos) - 0.001f;
		surf->maxs[0] = XModelDataReadFloat(pos) + 0.001f;
		surf->maxs[1] = XModelDataReadFloat(pos) + 0.001f;
		surf->maxs[2] = XModelDataReadFloat(pos) + 0.001f;
		surf->boneIdx = XModelDataReadInt(pos);
		surf->contents = XModelDataReadInt(pos) & ~(CONTENTS_TRANSLUCENT | CONTENTS_NONCOLLIDING);
		surf->surfFlags = XModelDataReadInt(pos);
		model->contents |= surf->contents;
	}
}

XModelParts *XModelPartsLoad(XModel *model, const char *name, void *(*Alloc)(int))
{
	XModelParts *parts;

	parts = XModelPartsFindData(name);

	if ( parts )
		return parts;

	parts = XModelPartsLoadFile(model, name, Alloc);

	if ( !parts )
	{
		Com_Printf("^1ERROR: Cannot find xmodelparts '%s'.\n", name);
		return NULL;
	}

	XModelPartsSetData(name, parts, Alloc);
	return parts;
}

XModel *XModelLoadFile(const char *name, void *(*Alloc)(int), void *(*AllocColl)(int))
{
	XModelConfig config;
	XModel *model;
	int lodStringLens[4];
	const unsigned char *pos;
	char *dest;
	int nameLenTotal;
	int i;
	int j;
	char filename[64];
	byte *buf;
	const char *surfName;
	int numBones;
	float *mins;
	float *maxs;
	float *offset;
	vec3_t radius;
	XBoneInfo *boneInfo;
	int size;
	int fileSize;

	if ( Com_sprintf(filename, sizeof(filename), "xmodel/%s", name) < 0 )
	{
		Com_Printf("^1ERROR: filename '%s' too long\n", filename);
		return NULL;
	}

	fileSize = FS_ReadFile(filename, (void **)&buf);

	if ( fileSize < 0 )
	{
		Com_Printf("^1ERROR: xmodel '%s' not found\n", name);
		return NULL;
	}

	if ( !fileSize )
	{
		Com_Printf("^1ERROR: xmodel '%s' has 0 length\n", name);
		FS_FreeFile(buf);
		return NULL;
	}

	pos = buf;

	if ( !XModelLoadConfigFile(name, &pos, &config) )
	{
		FS_FreeFile(buf);
		return NULL;
	}

	nameLenTotal = 0;

	for ( i = 0; i < 4; ++i )
	{
		lodStringLens[i] = I_strlen(config.entries[i].filename) + 1;
		nameLenTotal += lodStringLens[i];
	}

	size = nameLenTotal + sizeof(XModel);
	model = (XModel *)Alloc(size);
	model->memUsage = size;
	XModelLoadCollData(&pos, model, AllocColl, name);
	dest = (char *)&model[1];
	model->numLods = 0;

	for ( i = 0; i < 4; ++i )
	{
		strcpy(dest, config.entries[i].filename);
		model->lodInfo[i].filename = dest;

		if ( *dest )
		{
			++model->numLods;
			model->lodInfo[i].numsurfs = XModelDataReadShort(&pos).value;
			size = sizeof(unsigned short) * model->lodInfo[i].numsurfs;
			model->lodInfo[i].surfNames = (unsigned short *)Alloc(size);
			model->memUsage += size;

			for ( j = 0; j < model->lodInfo[i].numsurfs; ++j )
			{
				surfName = (const char *)pos;
				pos += strlen(surfName) + 1;
				model->lodInfo[i].surfNames[j] = SL_GetString_(surfName, 0, 8);
			}
		}
		else
		{
			model->lodInfo[i].surfNames = NULL;
		}

		model->lodInfo[i].dist = config.entries[i].dist;
		dest += lodStringLens[i];
	}

	model->parts = XModelPartsLoad(model, model->lodInfo[0].filename, Alloc);

	if ( !model->parts )
	{
		FS_FreeFile(buf);
		XModelFree(model);
		return NULL;
	}

	numBones = model->parts->numBones;
	size = sizeof(XBoneInfo) * numBones;
	boneInfo = (XBoneInfo *)Alloc(size);
	model->memUsage += size;

	for ( i = 0; i < numBones; ++i )
	{
		mins = boneInfo[i].bounds[0];
		mins[0] = XModelDataReadFloat(&pos);
		mins[1] = XModelDataReadFloat(&pos);
		mins[2] = XModelDataReadFloat(&pos);
		maxs = boneInfo[i].bounds[1];
		maxs[0] = XModelDataReadFloat(&pos);
		maxs[1] = XModelDataReadFloat(&pos);
		maxs[2] = XModelDataReadFloat(&pos);
		offset = boneInfo[i].offset;
		Vec3Avg(mins, maxs, offset);
		VectorSubtract(maxs, offset, radius);
		boneInfo[i].radiusSquared = VectorLengthSquared(radius);
	}

	model->boneInfo = boneInfo;
	FS_FreeFileInl(buf);
	VectorCopy(config.mins, model->mins);
	VectorCopy(config.maxs, model->maxs);
	model->collLod = config.collLod;
	model->flags = config.flags;

	return model;
}

// unreferenced
bool XModelSkinsSupported()
{
	return false;
}

// unreferenced
bool XModelCollSupported()
{
	return false;
}

// Renderer export slots used by the surface loader
struct refexport_t
{
	void *unused0[96];
	XSurface *(*XModelReadSurface)( XModel *model, int *partBits, const unsigned char **pos, void *(*Alloc)(int) );
	trXSkin_t *(*LoadXSkins)( XModel *model );
};
extern refexport_t re;

// retail 0x48ea60/0x48ea80, unreferenced; names unknown
XSurface *XModelReadSurface_48ea60(XModel *model, int *partBits, const unsigned char **pos, void *(*Alloc)(int))
{
	return re.XModelReadSurface(model, partBits, pos, Alloc);
}

trXSkin_t *XModelLoadXSkins_48ea80(XModel *model)
{
	return re.LoadXSkins(model);
}

bool XModelSurfsSupported()
{
	return re.XModelReadSurface != NULL;
}
