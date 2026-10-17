#include "../qcommon/qcommon.h"
#include "../script/script_public.h"

// Renderer export slots used by the surface loader
struct refexport_t
{
	void *unused0[96];
	XSurface *(*XModelReadSurface)( XModel *model, int *partBits, const unsigned char **pos, void *(*Alloc)(int) );
	trXSkin_t *(*LoadXSkins)( XModel *model );
};
extern refexport_t re;
bool XModelSurfsSupported();
XModelSurfs *XModelReadSurface(XModel *model, const char *name, void *(*Alloc)(int), short numsurfs, const char *modelName);

XModelSurfs *XModelLoadSurface(XModel *model, const char *name, void *(*Alloc)(int), short numsurfs, const char *modelName)
{
	XModelSurfs *surfs;

	surfs = XModelSurfsFindData(name);

	if ( surfs )
		return surfs;

	surfs = XModelReadSurface(model, name, Alloc, numsurfs, modelName);

	if ( !surfs )
	{
		Com_Printf("^1ERROR: Cannot find 'xmodelsurfs '%s'.\n", name);
		return NULL;
	}

	XModelSurfsSetData(name, surfs, Alloc);
	return surfs;
}

bool XModelLoadSurfaces(XModel *model, void *(*Alloc)(int))
{
	int i;
	XModelLodInfo *lodInfo;

	for ( i = 0; i < 4; ++i )
	{
		lodInfo = &model->lodInfo[i];

		if ( !*lodInfo->filename )
			break;

		lodInfo->surfs = XModelLoadSurface(model, lodInfo->filename, Alloc, lodInfo->numsurfs, model->name);

		if ( !lodInfo->surfs )
			return false;
	}

	return true;
}

XModel *XModelLoad(const char *name, void *(*Alloc)(int), void *(*AllocColl)(int))
{
	XModel *model;

	model = XModelLoadFile(name, Alloc, AllocColl);

	if ( !model )
		return NULL;

	if ( XModelSurfsSupported() )
	{
		if ( !XModelLoadSurfaces(model, Alloc) )
		{
			XModelFree(model);
			return NULL;
		}

		model->xskins = re.LoadXSkins(model);
	}

	return model;
}

qboolean XModelGetStaticBounds(const XModel *model, float (*axis)[3], float *mins, float *maxs)
{
	XModelCollSurf *surf;
	int i;
	int k;
	int j;
	vec3_t in;
	vec3_t out;
	qboolean result;

	if ( !model->numCollSurfs )
	{
		result = qfalse;
	}
	else
	{
		VectorSet(mins, 3.4028235e38, 3.4028235e38, 3.4028235e38);
		VectorSet(maxs, -3.4028235e38, -3.4028235e38, -3.4028235e38);

		for ( i = 0; i < model->numCollSurfs; ++i )
		{
			surf = &model->collSurfs[i];

			for ( j = 0; j < 8; ++j )
			{
				in[0] = (j & 1) != 0 ? surf->mins[0] : surf->maxs[0];
				in[1] = (j & 2) != 0 ? surf->mins[1] : surf->maxs[1];
				in[2] = (j & 4) != 0 ? surf->mins[2] : surf->maxs[2];

				MatrixTransformVector(in, axis, out);

				for ( k = 0; k < 3; ++k )
				{
					if ( mins[k] > out[k] )
						mins[k] = out[k];

					if ( maxs[k] < out[k] )
						maxs[k] = out[k];
				}
			}
		}

		result = qtrue;
	}

	return result;
}
