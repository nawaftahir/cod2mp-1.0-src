#include "qcommon.h"

#define BSP_IDENT   (('P' << 24) + ('S' << 16) + ('B' << 8) + 'I')
#define BSP_VERSION 4
#define BSP_LUMPS   39

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

struct comBspGlob_t
{
	BspHeader *header;
	int fileSize;
	int checksum;
};

static comBspGlob_t comBspGlob;

bool Com_IsBspLoaded()
{
	return comBspGlob.header != NULL;
}

void *Com_GetBspHeader( int *size, int *checksum )
{
	if ( size )
	{
		*size = comBspGlob.fileSize;
	}

	if ( checksum )
	{
		*checksum = comBspGlob.checksum;
	}

	return comBspGlob.header;
}

void Com_LoadBsp( const char *filename )
{
	int bytesRead;
	int i;
	int h;

	comBspGlob.fileSize = FS_FOpenFileRead(filename, &h, 0);

	if ( !h )
	{
		Com_Error(ERR_DROP, va("EXE_ERR_COULDNT_LOAD\x15%s", filename));
	}

	comBspGlob.header = (BspHeader *)Z_MallocGarbage(comBspGlob.fileSize);
	bytesRead = FS_Read(comBspGlob.header, comBspGlob.fileSize, h);
	FS_FCloseFile(h);

	if ( bytesRead != comBspGlob.fileSize || comBspGlob.fileSize < sizeof(BspHeader) )
	{
		Z_Free(comBspGlob.header);
		Com_Error(ERR_DROP, va("EXE_ERR_COULDNT_LOAD\x15%s", filename));
	}

	comBspGlob.checksum = Com_BlockChecksum(comBspGlob.header, comBspGlob.fileSize);
	comBspGlob.header->ident = LittleLong(comBspGlob.header->ident);
	comBspGlob.header->version = LittleLong(comBspGlob.header->version);

	if ( comBspGlob.header->ident != BSP_IDENT || comBspGlob.header->version != BSP_VERSION )
	{
		Z_Free(comBspGlob.header);
		Com_Error(ERR_DROP, va("EXE_ERR_WRONG_MAP_VERSION_NUM\x15%s", filename));
	}

	for ( i = 0; i != BSP_LUMPS; i++ )
	{
		comBspGlob.header->lumps[i].filelen = LittleLong(comBspGlob.header->lumps[i].filelen);
		comBspGlob.header->lumps[i].fileofs = LittleLong(comBspGlob.header->lumps[i].fileofs);
	}
}

void Com_UnloadBsp()
{
	Z_Free(comBspGlob.header);
	comBspGlob.header = NULL;
}

void Com_CleanupBsp()
{
	if ( Com_IsBspLoaded() )
	{
		Com_UnloadBsp();
	}
}

