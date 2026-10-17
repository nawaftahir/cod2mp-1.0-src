#include "../qcommon/qcommon.h"
#include "../script/script_public.h"

int XModelTraceLine(const XModel *model, trace_t *results, DObjAnimMat *pose, const float *localStart, const float *localEnd, int contentmask);

// Backing storage for the placeholder model used when a load fails.
struct XModelDefaults
{
	unsigned short boneNames[1];
	XBoneHierarchy hierarchy;
	XModelParts modelPart;
	XModelSurfs surface;
	XBoneInfo boneInfo;
	byte partClassification;
	unsigned short surfName;
};
static XModelDefaults xmodelDefaults;
static int xmodelUnusedValue;

// grouping pins retail fadd order (XModelTraceLine 0x48d430)
inline void AxisTransformVector(const vec3_t in, const vec3_t mat[3], vec3_t out)
{
	out[0] = in[0] * mat[0][0] + (in[1] * mat[1][0] + in[2] * mat[2][0]);
	out[1] = in[0] * mat[0][1] + (in[1] * mat[1][1] + in[2] * mat[2][1]);
	out[2] = in[0] * mat[0][2] + (in[1] * mat[1][2] + in[2] * mat[2][2]);
}

inline void AxisTransposeTransformVector(const vec3_t in, const vec3_t mat[3], vec3_t out)
{
	out[0] = in[0] * mat[0][0] + in[1] * mat[0][1] + in[2] * mat[0][2];
	out[1] = in[0] * mat[1][0] + in[1] * mat[1][1] + in[2] * mat[1][2];
	out[2] = in[0] * mat[2][0] + in[1] * mat[2][1] + in[2] * mat[2][2];
}

XModelSurfs *SetDefaultSurface()
{
	XModelSurfs *surface;

	surface = &xmodelDefaults.surface;
	surface->surf = NULL;

	return surface;
}

qboolean XModelBad(XModel *model)
{
	return model->bad;
}

// unreferenced; the stored value is never read
void XModelSetUnusedValue(int value)
{
	xmodelUnusedValue = value;
}

void XModelPartsFree(XModelParts *modelParts)
{
	XBoneHierarchy *hierarchy;
	unsigned short *name;
	int i;
	int numBones;

	hierarchy = modelParts->hierarchy;
	name = hierarchy->names;
	numBones = modelParts->numBones;

	for ( i = 0; i < numBones; ++i )
		SL_RemoveRefToString(name[i]);
}

void XModelFree(XModel *model)
{
	int i;
	int j;

	if ( XModelBad(model) )
		return;

	for ( i = 0; i <= 3; ++i )
	{
		if ( !model->lodInfo[i].surfNames )
			continue;

		for ( j = 0; j < model->lodInfo[i].numsurfs; ++j )
			SL_RemoveRefToString(model->lodInfo[i].surfNames[j]);

		model->lodInfo[i].surfNames = NULL;
	}
}

XModelParts *SetDefaultModelPart()
{
	XModelParts *parts;
	XBoneHierarchy *hierarchy;
	unsigned short *names;
	short numBones;
	short numRootBones;

	numRootBones = 1;
	numBones = 1;

	names = xmodelDefaults.boneNames;
	hierarchy = &xmodelDefaults.hierarchy;
	hierarchy->names = names;

	parts = &xmodelDefaults.modelPart;
	parts->hierarchy = hierarchy;
	parts->quats = NULL;
	parts->trans = NULL;
	parts->numBones = numBones;
	parts->numRootBones = numRootBones;
	parts->partClassification = &xmodelDefaults.partClassification;

	xmodelDefaults.partClassification = 0;
	names[0] = 0;

	return parts;
}

void SetDefaultModel(XModel *model)
{
	int i;
	XBoneInfo *boneInfo;
	float *mins;
	float *maxs;

	model->bad = 1;
	model->parts = SetDefaultModelPart();

	for ( i = 0; i < 4; ++i )
	{
		model->lodInfo[i].surfs = NULL;
		model->lodInfo[i].filename = "";
		model->lodInfo[i].dist = 0;
		model->lodInfo[i].numsurfs = 1;
		model->lodInfo[i].surfNames = &xmodelDefaults.surfName;
		xmodelDefaults.surfName = 0;
	}

	model->lodInfo[0].surfs = SetDefaultSurface();
	model->numLods = 1;
	model->collLod = 0;
	model->name = "DEFAULT";

	boneInfo = &xmodelDefaults.boneInfo;

	mins = boneInfo->bounds[0];
	mins[0] = -16;
	mins[1] = -16;
	mins[2] = -16;

	maxs = boneInfo->bounds[1];
	maxs[0] = 16;
	maxs[1] = 16;
	maxs[2] = 16;

	model->boneInfo = boneInfo;
}

XModel *AllocDefaultModel(void *(*Alloc)(int))
{
	XModel *model;

	model = (XModel *)Alloc(sizeof(XModel));
	SetDefaultModel(model);

	return model;
}

XModelParts *XModelPartsFindData(const char *partName)
{
	return (XModelParts *)Hunk_FindDataForFile(FILEDATA_XMODELPARTS, partName);
}

const char *XModelPartsSetData(const char *partName, XModelParts *parts, void *(*Alloc)(int))
{
	return Hunk_SetDataForFile(FILEDATA_XMODELPARTS, partName, parts, Alloc);
}

XModelSurfs *XModelSurfsFindData(const char *surfName)
{
	return (XModelSurfs *)Hunk_FindDataForFile(FILEDATA_XMODELSURFS, surfName);
}

const char *XModelSurfsSetData(const char *surfName, XModelSurfs *surface, void *(*Alloc)(int))
{
	return Hunk_SetDataForFile(FILEDATA_XMODELSURFS, surfName, surface, Alloc);
}

XModel *XModelFindData(const char *modelName)
{
	return (XModel *)Hunk_FindDataForFile(FILEDATA_XMODEL, modelName);
}

XModel *XModelLoadDefaultModel(const char *name, void *(*Alloc)(int))
{
	XModel *model;

	model = AllocDefaultModel(Alloc);
	Hunk_SetDataForFile(FILEDATA_XMODEL, name, model, Alloc);

	return model;
}

void XModelReplace(XModel *model, XModel *replacement)
{
	int i;
	int j;
	const char *name;

	if ( XModelBad(replacement) )
		Com_Error(ERR_DROP, "Could not load replacement model.");

	name = model->name;
	XModelFree(model);
	*model = *replacement;
	model->name = name;

	for ( i = 0; i <= 3; ++i )
	{
		if ( !model->lodInfo[i].surfNames )
			continue;

		for ( j = 0; j < model->lodInfo[i].numsurfs; ++j )
			SL_AddRefToString(model->lodInfo[i].surfNames[j]);
	}
}

XModel *XModelPrecache(const char *name, void *(*Alloc)(int), void *(*AllocColl)(int))
{
	XModel *model;

	model = (XModel *)Hunk_FindDataForFile(FILEDATA_XMODEL, name);

	if ( model )
		return model;

	model = XModelLoad(name, Alloc, AllocColl);

	if ( model )
	{
		model->name = Hunk_SetDataForFile(FILEDATA_XMODEL, name, model, Alloc);
		return model;
	}

	Com_Printf("^1ERROR: Cannot find xmodel '%s'.\n", name);
	return XModelLoadDefaultModel(name, Alloc);
}

unsigned short *XModelBoneNames(XModel *model)
{
	return model->parts->hierarchy->names;
}

int XModelGetBoneIndex(const XModel *model, unsigned int name)
{
	int i;
	int numBones;
	unsigned short *names;
	XBoneHierarchy *hierarchy;
	XModelParts *parts;

	parts = model->parts;
	hierarchy = parts->hierarchy;
	names = hierarchy->names;
	numBones = parts->numBones;

	for ( i = numBones - 1; i >= 0; --i )
	{
		if ( name == names[i] )
			break;
	}

	return i;
}

void XModelGetBounds(const XModel *model, float *mins, float *maxs)
{
	VectorCopy(model->mins, mins);
	VectorCopy(model->maxs, maxs);
}

const char *XModelGetLodFilename(const XModel *model, int lod)
{
	return model->lodInfo[lod].filename;
}

int XModelGetContents(const XModel *model)
{
	return model->contents;
}

int XModelGetCollLod(const XModel *model)
{
	return model->collLod;
}

const trXSkin_t *XModelGetSkins(const XModel *model)
{
	return model->xskins;
}

int XModelGetMemUsage(const XModel *model)
{
	return model->memUsage;
}

int XModelTraceLine(const XModel *model, trace_t *results, DObjAnimMat *pose, const float *localStart, const float *localEnd, int contentmask)
{
	int boneIdx;
	int numBones;
	DObjAnimMat *Mat;
	vec3_t startDelta;
	vec3_t endDelta;
	TraceExtents boneExtents;
	int boneIndex;
	XModelCollSurf_s *surf;
	XModelCollTri_s *tri;
	vec3_t boneVec;
	vec3_t hit;
	float startDist;
	float endDist;
	float fraction;
	float dist;
	float s;
	float t;
	int i;
	int j;
	vec3_t normal;
	float axis[3][3];

	boneIndex = -1;
	numBones = XModelNumBones(model);

	for ( i = 0; i < model->numCollSurfs; ++i )
	{
		surf = &model->collSurfs[i];

		if ( !(surf->contents & contentmask) )
		{
			continue;
		}

		boneIdx = surf->boneIdx;
		Mat = &pose[boneIdx];
		VectorSubtract(localStart, Mat->trans, startDelta);
		VectorSubtract(localEnd, Mat->trans, endDelta);
		ConvertQuatToMat(Mat, axis);
		AxisTransposeTransformVector(startDelta, axis, boneExtents.start);
		AxisTransposeTransformVector(endDelta, axis, boneExtents.end);
		CM_CalcTraceEntents(&boneExtents);

		if ( CM_TraceBox(&boneExtents, surf->mins, surf->maxs, results->fraction) )
		{
			continue;
		}

		VectorSubtract(boneExtents.end, boneExtents.start, boneVec);

		for ( j = 0; j < surf->numCollTris; ++j )
		{
			tri = &surf->collTris[j];

			startDist = DotProduct(boneExtents.end, tri->plane) - tri->plane[3];

			if ( startDist >= 0.0f )
			{
				continue;
			}

			endDist = DotProduct(boneExtents.start, tri->plane) - tri->plane[3];

			if ( endDist <= 0.0f )
			{
				continue;
			}

			fraction = (endDist - 0.125f) / (endDist - startDist);
			fraction = I_fmax(fraction, 0.0f);

			if ( fraction >= results->fraction )
			{
				continue;
			}

			dist = endDist / (endDist - startDist);
			VectorMA(boneExtents.start, dist, boneVec, hit);
			s = DotProduct(hit, tri->svec) - tri->svec[3];

			if ( s < -0.001f )
			{
				continue;
			}

			if ( s > 1.001f )
			{
				continue;
			}

			t = DotProduct(hit, tri->tvec) - tri->tvec[3];

			if ( t < -0.001f )
			{
				continue;
			}

			if ( s + t > 1.001f )
			{
				continue;
			}

			boneIndex = boneIdx;
			results->startsolid = 0;
			results->allsolid = 0;
			results->fraction = fraction;
			results->surfaceFlags = surf->surfFlags;
			results->contents = surf->contents;
			VectorCopy(tri->plane, results->normal);
		}
	}

	if ( boneIndex < 0 )
	{
		return -1;
	}

	Mat = &pose[boneIndex];
	ConvertQuatToMat(Mat, axis);
	AxisTransformVector(results->normal, axis, normal);
	VectorCopy(normal, results->normal);

	return boneIndex;
}

bool Com_ValidXModelName(const char *name)
{
	return !strncasecmp(name, "xmodel", 6) && (name[6] == '/' || name[6] == PATH_SEP);
}

// unreferenced
int XModelReturnZero()
{
	return 0;
}

// unreferenced
void XModelIgnoreChar(char c)
{
}

