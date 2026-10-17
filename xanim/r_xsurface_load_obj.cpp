#include "../qcommon/qcommon.h"
#include "../script/script_public.h"

#define XMODEL_VERSION 20

// Renderer export slots used by the surface loader
struct refexport_t
{
	void *unused0[96];
	XSurface *(*XModelReadSurface)( XModel *model, int *partBits, const unsigned char **pos, void *(*Alloc)(int) );
	trXSkin_t *(*LoadXSkins)( XModel *model );
};
extern refexport_t re;

// retail 0x48eaa0
static __declspec(noinline) void XModelReadSurfaces(XModel *model, XSurface **out, int *partBits, const unsigned char **pos, void *(*Alloc)(int), int numsurfs)
{
	int i;

	for ( i = 0; i < numsurfs; ++i )
		out[i] = re.XModelReadSurface(model, partBits, pos, Alloc);
}

// retail 0x48eae0
XModelSurfs *XModelReadSurface(XModel *model, const char *name, void *(*Alloc)(int), short numsurfs, const char *modelName)
{
	XModelSurfs *surfs;
	char filename[64];
	byte *buf;
	const unsigned char *pos;
	short version;
	short fileNumSurfs;
	int size;
	int fileSize;

	if ( Com_sprintf(filename, sizeof(filename), "xmodelsurfs/%s", name) < 0 )
	{
		Com_Printf("^1ERROR: filename '%s' too long\n", filename);
		return NULL;
	}

	fileSize = FS_ReadFile(filename, (void **)&buf);

	if ( fileSize < 0 )
	{
		Com_Printf("^1ERROR: xmodelsurf '%s' not found\n", name);
		return NULL;
	}

	if ( fileSize == 0 )
	{
		Com_Printf("^1ERROR: xmodelsurf '%s' has 0 length\n", name);
		FS_FreeFile(buf);
		return NULL;
	}

	pos = buf;
	version = XModelDataReadShort(&pos).value;

	if ( version != XMODEL_VERSION )
	{
		FS_FreeFile(buf);
		Com_Printf("^1ERROR: xmodelsurfs '%s' out of date (version %d, expecting %d).\n", name, version, XMODEL_VERSION);
		return NULL;
	}

	fileNumSurfs = XModelDataReadShort(&pos).value;

	if ( fileNumSurfs != numsurfs )
	{
		FS_FreeFile(buf);
		Com_Printf("^1ERROR: File conflict (between non-iwd and iwd file) on xmodelsurfs '%s' for xmodel '%s'.\nRename the export file to fix.\n", name, modelName);
		return NULL;
	}

	size = numsurfs * 4 + sizeof(XModelSurfs);
	surfs = (XModelSurfs *)Alloc(size);
	model->memUsage += size;
	surfs->surf = (XSurface **)(surfs + 1);
	XModelReadSurfaces(model, surfs->surf, surfs->partBits, &pos, Alloc, numsurfs);
	FS_FreeFile(buf);
	return surfs;
}
