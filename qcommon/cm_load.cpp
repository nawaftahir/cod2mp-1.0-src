#include "qcommon.h"
#include "cm_local.h"

#define BSP_VERSION     4
#define BSP_VERSION_ALT 61
#define BSP_LUMPS       39

struct BspLump
{
	int filelen;
	int fileofs;
};

struct BspHeader
{
	int ident;
	int version;
	BspLump lumps[BSP_LUMPS];
};

clipMap_t cm;
static int cm_unused;	// unreferenced; original name unknown
clipMapExtra_t cme;

void CM_InitThreadData( int threadContext );
void CM_InitAllThreadData();
void CM_LoadMapData( const char *name );
void CM_LoadMap( const char *name, int *checksum );
void CM_Shutdown();
extern void FS_FreeFileInl(void *buffer);
void CM_SaveLump( int lumpType, const void *data, int len, int *checksum );
int CM_NumInlineModels( void );
char *CM_EntityString();
int CM_LeafCluster( int leafnum );
void CM_ModelBounds( clipHandle_t model, vec3_t mins, vec3_t maxs );

/*
===================
CM_InitThreadData
===================
*/
void CM_InitThreadData( int threadContext )
{
	TraceThreadInfo *tti;

	tti = &g_traceThreadInfo[threadContext];

	tti->checkcount = 0;

	tti->partitions = (uint16_t *)Hunk_Alloc( cm.partitionCount * sizeof( *tti->partitions ) );
	tti->edges = (int32_t *)Hunk_Alloc( cm.edgeCount * sizeof( *tti->edges ) );
	tti->verts = (int32_t *)Hunk_Alloc( cm.vertCount * sizeof( *tti->verts ) );

	tti->box_brush = (cbrush_t *)Hunk_Alloc( sizeof( *tti->box_brush ) );
	*tti->box_brush = *cm.box_brush;

	tti->box_model = (cmodel_t *)Hunk_Alloc( sizeof( *tti->box_model ) );
	*tti->box_model = cm.box_model;
}

/*
===================
CM_InitAllThreadData
===================
*/
void CM_InitAllThreadData()
{
	CM_InitThreadData(THREAD_CONTEXT_MAIN);
}

/*
===================
CM_LoadMapData
===================
*/
void CM_LoadMapData( const char *name )
{
	CM_LoadMapFromBsp(name, true);
	CM_LoadStaticModels();
}

/*
===================
CM_LoadMap
===================
*/
void CM_LoadMap( const char *name, int *checksum )
{
	if ( !name || !*name )
	{
		Com_Error(ERR_DROP, "\x15" "CM_LoadMap: NULL name");
	}

	if ( !cm.name || strcasecmp(cm.name, name) )
	{
		CM_LoadMapData(name);
		CM_InitAllThreadData();
	}

	*checksum = cm.checksum;
}

/*
===================
CM_Shutdown
===================
*/
void CM_Shutdown()
{
	Com_Memset(&cm, 0, sizeof(cm));
}

// original name unknown; swaps only the first numLumps words of the header
static void CM_SwapHeader( BspHeader *header, int numLumps )
{
	int i;
	int *block;
	int count;	// the header's word count, computed but never used

	count = (sizeof(BspLump) * numLumps + 8) / sizeof(int);
	block = (int *)header;

	for ( i = 0; i < numLumps; i++ )
	{
		block[i] = LittleLong(block[i]);
	}
}

/*
===================
CM_SaveLump

Rewrites the current bsp with one lump replaced.
===================
*/
void CM_SaveLump( int lumpType, const void *data, int len, int *checksum )
{
	void *buf;
	const void *lumpData;
	BspHeader header;
	BspHeader oldHeader;
	int fileSize;
	int ofs;
	int headerSize;
	int i;
	int numLumps;
	int padding;
	int zero;
	int h;

	fileSize = FS_FOpenFileRead(cm.name, &h, 0);

	if ( !h )
	{
		Com_Error(ERR_DROP, "EXE_ERR_COULDNT_LOAD\x15%s", cm.name);
	}

	buf = Z_Malloc(fileSize + 1);
	FS_Read(buf, fileSize, h);
	((char *)buf)[fileSize] = 0;
	FS_FCloseFile(h);

	header = *(BspHeader *)buf;

	switch ( LittleLong(header.version) )
	{
	case BSP_VERSION:
	case BSP_VERSION_ALT:
		numLumps = BSP_LUMPS;
		break;
	default:
		Com_Error(ERR_DROP, "bad bsp version %d", header.version);
		return;
	}

	CM_SwapHeader(&header, numLumps);
	oldHeader = header;

	h = FS_OpenFileOverwrite(cm.name);

	if ( !h )
	{
		Com_Error(ERR_DROP, "Failed to open file %s for writing", cm.name);
		return;
	}

	headerSize = sizeof(BspLump) * numLumps + 8;
	ofs = headerSize;

	for ( i = 0; i < numLumps; i++ )
	{
		if ( i == lumpType )
		{
			header.lumps[i].filelen = len;
		}

		header.lumps[i].fileofs = ofs;
		ofs += (header.lumps[i].filelen + 3) & ~3;
	}

	CM_SwapHeader(&header, numLumps);
	FS_Write(&header, headerSize, h);
	CM_SwapHeader(&header, numLumps);

	zero = 0;

	for ( i = 0; i < numLumps; i++ )
	{
		if ( !header.lumps[i].filelen )
		{
			continue;
		}

		if ( i == lumpType )
		{
			lumpData = data;
		}
		else
		{
			lumpData = (char *)buf + oldHeader.lumps[i].fileofs;
		}

		FS_Write(lumpData, header.lumps[i].filelen, h);
		padding = ((header.lumps[i].filelen + 3) & ~3) - header.lumps[i].filelen;

		if ( padding )
		{
			FS_Write(&zero, padding, h);
		}
	}

	FS_FCloseFile(h);
	Z_Free(buf);

	if ( checksum )
	{
		ofs = FS_ReadFile(cm.name, &buf);
		*checksum = Com_BlockChecksum(buf, ofs);
		FS_FreeFileInl(buf);
	}
}

/*
===================
CM_NumInlineModels
===================
*/
int CM_NumInlineModels( void )
{
	return cm.numSubModels;
}

/*
===================
CM_EntityString
===================
*/
char *CM_EntityString()
{
	return cm.entityString;
}

/*
==================
CM_LeafCluster
==================
*/
int CM_LeafCluster( int leafnum )
{
	return cm.leafs[leafnum].cluster;
}

/*
===================
CM_ModelBounds
===================
*/
void CM_ModelBounds( clipHandle_t model, vec3_t mins, vec3_t maxs )
{
	cmodel_t *cmod;

	cmod = CM_ClipHandleToModel( model );

	VectorCopy( cmod->mins, mins );
	VectorCopy( cmod->maxs, maxs );
}

void *CM_Hunk_Alloc( int size, const char *name, int type )
{
	return Hunk_Alloc(size);
}

void CM_Hunk_CheckTempMemoryClear()
{
}

void CM_Hunk_CheckTempMemoryHighClear()
{
}

void *CM_Hunk_AllocateTempMemoryHigh( int size, const char *name )
{
	return Hunk_AllocateTempMemoryHighInternal(size);
}

void CM_Hunk_ClearTempMemory()
{
	Hunk_ClearTempMemoryInternal();
}

void CM_Hunk_ClearTempMemoryHigh()
{
	Hunk_ClearTempMemoryHighInternal();
}

