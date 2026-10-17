#include "../qcommon/qcommon.h"
#include "com_files.h"
#include "com_memory.h"

static fileData_t *com_fileDataHashTable[FILEDATA_HASH_SIZE];
static fileData_t *com_hunkData;

// storage in its original order; the unreferenced blocks keep that layout
static int g_largeLocalPos;
static char mem_unreferenced0[0x58] __attribute__((aligned(4)));
static byte g_largeLocalBuf[0x80000];
static int mem_unreferenced1;

static hunkUsed_t hunk_low;
static hunkUsed_t hunk_high;
static byte *s_hunkData;
static byte *s_origHunkData;
static int s_hunkTotal;

void Hunk_Clear( void );
void *Hunk_AllocAlignInternal( int size, int aligment );
void *Hunk_AllocLowAlignInternal( int size, int aligment );

void Z_FreeInternal( void *ptr )
{
	free( ptr );
}

void Z_VirtualFreeInternal( void *ptr )
{
	VirtualFree( ptr, 0, MEM_RELEASE );
}

// retail 0x42a990, ri.Z_VirtualDecommitInternal
void Z_VirtualDecommitInternal( void *ptr, int size )
{
	VirtualFree( ptr, size, MEM_DECOMMIT );
}

static void Z_MallocFailed( int size )
{
	Sys_OutOfMemErrorInternal( ".\\universal\\com_memory.cpp", 220 );
}

static void *Z_MallocRaw( int size )
{
	return malloc( size );
}

void *Z_TryMallocInternal( int size )
{
	void *buf;

	buf = Z_MallocRaw( size );
	if ( buf )
		Com_Memset( buf, 0, size );

	return buf;
}

void *Z_MallocInternal( int size )
{
	void *buf;

	buf = Z_TryMallocInternal( size );
	if ( !buf )
		Z_MallocFailed( size );

	return buf;
}

void *Z_MallocGarbageInternal( int size )
{
	void *buf;

	buf = Z_MallocRaw( size );
	if ( !buf )
		Z_MallocFailed( size );

	return buf;
}

// retail 0x42aa50 (inlined in Z_VirtualAllocInternal); CoD4 name (SUPPORTED)
void *Z_TryVirtualAllocInternal( int size )
{
	return VirtualAlloc( NULL, size, MEM_COMMIT, PAGE_READWRITE );
}

void *Z_VirtualAllocInternal( unsigned int size )
{
	void *buf;

	buf = Z_TryVirtualAllocInternal( size );
	if ( !buf )
		Sys_OutOfMemErrorInternal( ".\\universal\\com_memory.cpp", 524 );

	return buf;
}

// retail 0x42aaa0, ri.Z_VirtualReserveInternal
void *Z_VirtualReserveInternal( int size )
{
	return VirtualAlloc( NULL, size, MEM_RESERVE, PAGE_READWRITE );
}

// retail 0x42aac0, ri.Z_VirtualCommitInternal
void Z_VirtualCommitInternal( void *ptr, int size )
{
	if ( !VirtualAlloc( ptr, size, MEM_COMMIT, PAGE_READWRITE ) )
		Sys_OutOfMemErrorInternal( ".\\universal\\com_memory.cpp", 549 );
}

char *CopyStringInternal( const char *in )
{
	char *out;

	out = (char *)Z_MallocInternal( I_strlen( in ) + 1 );
	strcpy( out, in );

	return out;
}

void ReplaceStringInternal( const char **str, const char *in )
{
	char *s;
	unsigned int len;

	len = I_strlen( in );
	s = (char *)*str;

	if ( s && strlen( s ) < len )
	{
		Z_FreeInternal( s );
		s = NULL;
	}

	if ( !s )
	{
		s = (char *)Z_MallocInternal( len + 1 );
		*str = s;
	}

	strcpy( s, in );
}

void Com_Meminfo_f( void )
{
	Com_Printf( "%8i bytes total hunk\n", s_hunkTotal );
	Com_Printf( "\n" );
	Com_Printf( "%8i low permanent\n", hunk_low.permanent );
	if ( hunk_low.temp != hunk_low.permanent )
		Com_Printf( "%8i low temp\n", hunk_low.temp );
	Com_Printf( "\n" );
	Com_Printf( "%8i high permanent\n", hunk_high.permanent );
	if ( hunk_high.temp != hunk_high.permanent )
		Com_Printf( "%8i high temp\n", hunk_high.temp );
	Com_Printf( "\n" );
	Com_Printf( "%8i total hunk in use\n", hunk_low.permanent + hunk_high.permanent );
	Com_Printf( "\n" );
}

void Com_TouchMemory( void )
{
	int start;
	int end;
	int i;
	int j;
	int sum;

	start = Sys_MilliSeconds();

	sum = 0;

	j = hunk_low.permanent >> 2;
	for ( i = 0; i < j; i += 64 )
	{
		sum += ((int *)s_hunkData)[i];
	}

	i = ( s_hunkTotal - hunk_high.permanent ) >> 2;
	j = hunk_high.permanent >> 2;
	for ( ; i < j; i += 64 )
	{
		sum += ((int *)s_hunkData)[i];
	}

	end = Sys_MilliSeconds();

	Com_Printf( "Com_TouchMemory: %i msec. Using sum: %d\n", end - start, sum );
}

void Com_InitHunkMemory( void )
{
	dvar_t *com_hunkMegs;
	int nMegs;

	if ( FS_LoadStack() != 0 )
		Com_Error( ERR_FATAL, "\x15" "Hunk initialization failed. File system load stack not zero" );

	com_hunkMegs = Dvar_RegisterInt( "com_hunkMegs", 160, 1, 512, DVAR_ARCHIVE | DVAR_LATCH | DVAR_CHANGEABLE_RESET );
	nMegs = com_hunkMegs->current.integer;

	if ( nMegs < 80 )
	{
		Com_Printf( "Minimum com_hunkMegs for a dedicated server is %i, allocating %i megs.\n", 80, 80 );
		s_hunkTotal = 80 * 1024 * 1024;
	}
	else
	{
		s_hunkTotal = nMegs * 1024 * 1024;
	}

	s_hunkData = (byte *)VirtualAlloc( NULL, s_hunkTotal, MEM_COMMIT, PAGE_READWRITE );

	if ( !s_hunkData )
		Sys_OutOfMemErrorInternal( ".\\universal\\com_memory.cpp", 843 );

	s_origHunkData = s_hunkData;
	Hunk_Clear();
	Cmd_AddCommand( "meminfo", Com_Meminfo_f );
}

void Hunk_Shutdown( void )
{
	VirtualFree( s_origHunkData, 0, MEM_RELEASE );
	s_hunkData = NULL;
	s_origHunkData = NULL;
	s_hunkTotal = 0;
	memset( &hunk_low, 0, sizeof( hunk_low ) );
	memset( &hunk_high, 0, sizeof( hunk_high ) );
}

void *Hunk_FindDataForFileInternal( int type, const char *name, int hash )
{
	fileData_t *searchFileData;

	for ( searchFileData = com_fileDataHashTable[hash]; searchFileData; searchFileData = searchFileData->next )
	{
		if ( searchFileData->type != type )
			continue;

		if ( !strcasecmp( searchFileData->name, name ) )
			return searchFileData->data;
	}

	return NULL;
}

void *Hunk_FindDataForFile( int type, const char *name )
{
	int hash;

	hash = FS_HashFileName( name, FILEDATA_HASH_SIZE );

	return Hunk_FindDataForFileInternal( type, name, hash );
}

qboolean Hunk_DataOnHunk( void *data )
{
	if ( data < s_hunkData )
		return 0;

	if ( data >= &s_hunkData[s_hunkTotal] )
		return 0;

	return 1;
}

const char *Hunk_SetDataForFile( int type, const char *name, void *data, void *(*alloc)(int) )
{
	int hash;
	fileData_t *fileData;

	hash = FS_HashFileName( name, FILEDATA_HASH_SIZE );
	fileData = (fileData_t *)alloc( I_strlen( name ) + sizeof( fileData_t ) );
	fileData->data = data;
	fileData->type = type;
	strcpy( fileData->name, name );
	fileData->next = com_fileDataHashTable[hash];
	com_fileDataHashTable[hash] = fileData;

	return fileData->name;
}

void Hunk_AddData( int type, void *data, void *(*alloc)(int) )
{
	fileData_t *fileData;

	fileData = (fileData_t *)alloc( sizeof( fileData_t ) - 1 );
	fileData->data = data;
	fileData->type = type;
	fileData->next = com_hunkData;
	com_hunkData = fileData;
}

void Hunk_OverrideDataForFile( int type, const char *name, void *data )
{
	int hash;
	fileData_t *searchFileData;

	hash = FS_HashFileName( name, FILEDATA_HASH_SIZE );

	for ( searchFileData = com_fileDataHashTable[hash]; searchFileData; searchFileData = searchFileData->next )
	{
		if ( searchFileData->type != type )
			continue;

		if ( !strcasecmp( searchFileData->name, name ) )
		{
			searchFileData->data = data;
			return;
		}
	}
}

void Hunk_ClearDataFor( fileData_t **pFileData, unsigned char *low, unsigned char *high )
{
	fileData_t *fileData;
	void *data;

	while ( *pFileData )
	{
		fileData = *pFileData;

		if ( fileData < (fileData_t *)low || fileData >= (fileData_t *)high )
		{
			pFileData = &fileData->next;
		}
		else
		{
			*pFileData = fileData->next;
			data = fileData->data;

			switch ( fileData->type )
			{
			case FILEDATA_XMODELPARTS:
				XModelPartsFree( (XModelParts *)data );
				break;
			case FILEDATA_XMODEL:
				XModelFree( (XModel *)data );
				break;
			case FILEDATA_XANIM:
				XAnimFree( (XAnimParts *)data );
				break;
			case FILEDATA_XANIMLIST:
				XAnimFreeList( (XAnim *)data );
				break;
			}
		}
	}
}

void Hunk_ClearData( void )
{
	unsigned char *low;
	unsigned char *high;
	unsigned int hash;

	low = &s_hunkData[hunk_low.permanent];
	high = s_hunkData + s_hunkTotal - hunk_high.permanent;

	for ( hash = 0; hash < FILEDATA_HASH_SIZE; ++hash )
	{
		Hunk_ClearDataFor( &com_fileDataHashTable[hash], low, high );
	}

	Hunk_ClearDataFor( &com_hunkData, low, high );
}

void DB_EnumXAssetsFor( fileData_t *fileData, int fileDataType, void (*func)(XAssetHeader, void *), void *inData )
{
	void *data;
	XAssetHeader header;

	header.data = NULL;

	for ( ; fileData; fileData = fileData->next )
	{
		if ( fileData->type != fileDataType )
			continue;

		data = fileData->data;

		switch ( fileData->type )
		{
		case FILEDATA_XMODEL:
			header.model = (XModel *)data;
			func( header, inData );
			break;
		}
	}
}

void DB_EnumXAssets( XAssetType type, void (*func)(XAssetHeader, void *), void *inData, bool includeOverride )
{
	unsigned int i;
	fileData_t *fileData;
	int fileDataType;

	switch ( type )
	{
	case ASSET_TYPE_XMODEL:
		fileDataType = FILEDATA_XMODEL;
		for ( i = 0; i < FILEDATA_HASH_SIZE; ++i )
		{
			fileData = com_fileDataHashTable[i];
			DB_EnumXAssetsFor( fileData, fileDataType, func, inData );
		}
		break;
	}
}

int Hunk_SetMark( void )
{
	return hunk_high.permanent;
}

void Hunk_ClearHigh( int memory )
{
	hunk_high.permanent = hunk_high.temp = memory;
	Hunk_ClearData();
}

int Hunk_GetLowPermanent( void )
{
	return hunk_low.permanent;
}

void Hunk_ClearLow( int memory )
{
	hunk_low.permanent = hunk_low.temp = memory;
	Hunk_ClearData();
}

void Hunk_Clear( void )
{
	hunk_low.permanent = 0;
	hunk_low.temp = 0;
	hunk_high.permanent = 0;
	hunk_high.temp = 0;
	Hunk_ClearData();
}

int Hunk_Used( void )
{
	return hunk_low.permanent + hunk_high.permanent;
}

void *Hunk_AllocInternal( int size )
{
	return Hunk_AllocAlignInternal( size, 32 );
}

void *Hunk_AllocAlignInternal( int size, int aligment )
{
	void *buf;

	--aligment;
	hunk_high.permanent += size;
	hunk_high.permanent = ( hunk_high.permanent + aligment ) & ~aligment;
	buf = s_hunkData + s_hunkTotal - hunk_high.permanent;
	hunk_high.temp = hunk_high.permanent;

	if ( hunk_low.temp + hunk_high.temp > s_hunkTotal )
	{
		Com_Error(
		    ERR_DROP,
		    "\x15Hunk_AllocAlign failed on %i bytes (total %i MB, low %i MB, high %i MB)",
		    size,
		    s_hunkTotal / ( 1024 * 1024 ),
		    hunk_low.temp / ( 1024 * 1024 ),
		    hunk_high.temp / ( 1024 * 1024 ) );
	}

	memset( buf, 0, size );

	return buf;
}

void *Hunk_AllocateTempMemoryHighInternal( int size )
{
	void *buf;

	hunk_high.temp += size;
	hunk_high.temp = PAD( hunk_high.temp, 16 );

	if ( hunk_low.temp + hunk_high.temp > s_hunkTotal )
	{
		Com_Error(
		    ERR_DROP,
		    "\x15Hunk_AllocateTempMemoryHigh: failed on %i bytes (total %i MB, low %i MB, high %i MB)",
		    size,
		    s_hunkTotal / ( 1024 * 1024 ),
		    hunk_low.temp / ( 1024 * 1024 ),
		    hunk_high.temp / ( 1024 * 1024 ) );
	}

	buf = s_hunkData + s_hunkTotal - hunk_high.temp;

	return buf;
}

void Hunk_ClearTempMemoryHighInternal( void )
{
	hunk_high.temp = hunk_high.permanent;
}

void *Hunk_AllocLowInternal( int size )
{
	return Hunk_AllocLowAlignInternal( size, 32 );
}

void *Hunk_AllocLowAlignInternal( int size, int aligment )
{
	void *buf;

	--aligment;
	hunk_low.permanent = ( hunk_low.permanent + aligment ) & ~aligment;
	buf = &s_hunkData[hunk_low.permanent];
	hunk_low.permanent += size;
	hunk_low.temp = hunk_low.permanent;

	if ( hunk_low.temp + hunk_high.temp > s_hunkTotal )
	{
		Com_Error(
		    ERR_DROP,
		    "\x15Hunk_AllocLowAlign failed on %i bytes (total %i MB, low %i MB, high %i MB)",
		    size,
		    s_hunkTotal / ( 1024 * 1024 ),
		    hunk_low.temp / ( 1024 * 1024 ),
		    hunk_high.temp / ( 1024 * 1024 ) );
	}

	memset( buf, 0, size );

	return buf;
}

void Hunk_ConvertTempToPermLowInternal( void )
{
	hunk_low.permanent = hunk_low.temp;
}

void *Hunk_AllocateTempMemoryInternal( int size )
{
	void *buf;
	hunkHeader_t *hdr;
	int prevTemp;

	if ( !s_hunkData )
		return Z_MallocInternal( size );

	size += sizeof( hunkHeader_t );
	prevTemp = hunk_low.temp;
	hunk_low.temp = PAD( hunk_low.temp, 16 );
	buf = &s_hunkData[hunk_low.temp];
	hunk_low.temp += size;

	if ( hunk_low.temp + hunk_high.temp > s_hunkTotal )
	{
		Com_Error(
		    ERR_DROP,
		    "\x15Hunk_AllocateTempMemory: failed on %i bytes (total %i MB, low %i MB, high %i MB), needs %i more hunk bytes",
		    size,
		    s_hunkTotal / ( 1024 * 1024 ),
		    hunk_low.temp / ( 1024 * 1024 ),
		    hunk_high.temp / ( 1024 * 1024 ),
		    hunk_low.temp + hunk_high.temp - s_hunkTotal );
	}

	hdr = (hunkHeader_t *)buf;
	buf = (void *)( hdr + 1 );

	hdr->magic = HUNK_MAGIC;
	hdr->size = hunk_low.temp - prevTemp;

	return buf;
}

void *Hunk_ReallocateTempMemoryInternal( int size )
{
	void *buf;

	hunk_low.temp = PAD( hunk_low.permanent, 32 );
	buf = &s_hunkData[hunk_low.temp];
	hunk_low.temp += size;

	if ( hunk_low.temp + hunk_high.temp > s_hunkTotal )
	{
		Com_Error(
		    ERR_DROP,
		    "\x15Hunk_ReallocateTempMemory: failed on %i bytes (total %i MB, low %i MB, high %i MB)",
		    size,
		    s_hunkTotal / ( 1024 * 1024 ),
		    hunk_low.temp / ( 1024 * 1024 ),
		    hunk_high.temp / ( 1024 * 1024 ) );
	}

	return buf;
}

void Hunk_FreeTempMemory( void *buf )
{
	hunkHeader_t *hdr;

	if ( !s_hunkData )
	{
		Z_FreeInternal( buf );
		return;
	}

	hdr = (hunkHeader_t *)buf - 1;

	if ( hdr->magic != HUNK_MAGIC )
		Com_Error( ERR_FATAL, "\x15Hunk_FreeTempMemory: bad magic" );

	hdr->magic = HUNK_FREE_MAGIC;
	hunk_low.temp -= hdr->size;
}

void Hunk_ClearTempMemoryInternal( void )
{
	if ( s_hunkData )
		hunk_low.temp = hunk_low.permanent;
}

int Hunk_HideTempMemory( void )
{
	int permanent;

	permanent = hunk_low.permanent;
	hunk_low.permanent = hunk_low.temp;

	return permanent;
}

void Hunk_ShowTempMemory( int memory )
{
	hunk_low.permanent = memory;
}

int Hunk_HideTempMemoryHigh( void )
{
	int permanent;

	permanent = hunk_high.permanent;
	hunk_high.permanent = hunk_high.temp;

	return permanent;
}

void Hunk_ShowTempMemoryHigh( int memory )
{
	hunk_high.permanent = memory;
}

int LargeLocalBegin( int size )
{
	int startPos;

	size = ( size + 3 ) & ~3;
	startPos = g_largeLocalPos;
	g_largeLocalPos += size;

	return startPos;
}

void LargeLocalEnd( int startPos )
{
	g_largeLocalPos = startPos;
}

void *LargeLocalGetBuf( int startPos )
{
	return &g_largeLocalBuf[startPos];
}

LargeLocal::LargeLocal( int size )
{
	pos = LargeLocalBegin( size );
}

LargeLocal::~LargeLocal()
{
	LargeLocalEnd( pos );
}

void *LargeLocal::GetBuf()
{
	return LargeLocalGetBuf( pos );
}

void LargeLocalReset( void )
{
	g_largeLocalPos = 0;
}

