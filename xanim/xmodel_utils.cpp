#include "../qcommon/qcommon.h"
#include "../script/script_public.h"

// Per-LOD distance override set from the console; a negative distance clears it.
struct XModelLodOverride
{
	bool enabled;
	float dist;
};

static XModelLodOverride xmodelLodOverride[4];

const char *XModelGetLodName(const XModel *model, int lod)
{
	return model->lodInfo[lod].filename;
}

const char *XModelGetName(const XModel *model)
{
	return model->name;
}

unsigned char XModelGetFlags(const XModel *model)
{
	return model->flags;
}

const char *XModelGetSurfaceName(const XModel *model, int surfIndex, int lod)
{
	unsigned short name;

	name = model->lodInfo[lod].surfNames[surfIndex];

	if ( name )
		return SL_ConvertToString(name);

	return "DEFAULT";
}

int XModelGetSurfaces(const XModel *model, XSurface ***surfaces, int lod, int **partBits)
{
	const XModelLodInfo *lodInfo;

	lodInfo = &model->lodInfo[lod];
	*surfaces = lodInfo->surfs->surf;
	*partBits = lodInfo->surfs->partBits;

	return lodInfo->numsurfs;
}

int XModelGetNumLods(const XModel *model)
{
	return model->numLods;
}

int XModelNumBones(const XModel *model)
{
	return model->parts->numBones;
}

DObjAnimMat *XModelGetBasePose(const XModel *model)
{
	return &model->parts->skel.Mat;
}

// retail 0x48ef60, ri.XModelGetBasePoseBone
char *XModelGetBasePoseOffset(const XModel *model, unsigned int offset)
{
	unsigned int half;

	half = offset >> 1;

	return (char *)&model->parts->skel.Mat + half;
}

float XModelGetLodOutDist(const XModel *model)
{
	int lod;

	lod = XModelGetNumLods(model) - 1;

	return !xmodelLodOverride[lod].enabled ? model->lodInfo[lod].dist : xmodelLodOverride[lod].dist;
}

int XModelGetLodForDist(const XModel *model, float dist)
{
	const XModelLodInfo *lodInfo;
	int numLods;
	int i;
	float lodDist;

	numLods = XModelGetNumLods(model);
	lodInfo = model->lodInfo;

	for ( i = 0; i < numLods; i++ )
	{
		lodDist = !xmodelLodOverride[i].enabled ? lodInfo[i].dist : xmodelLodOverride[i].dist;

		if ( lodDist == 0 || dist < lodDist )
			return i;
	}

	return -1;
}

void XModelSetLodOverride(int lod, float dist)
{
	xmodelLodOverride[lod].dist = dist;
	xmodelLodOverride[lod].enabled = dist >= 0;
}

