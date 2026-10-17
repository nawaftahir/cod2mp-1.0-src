#pragma once

#include "../qcommon/qcommon.h"
#include "../xanim/xanim_public.h"

#define PAD(base, alignment)	(((base)+(alignment)-1) & ~((alignment)-1))
#define PADP(base, alignment)	((void *) PAD((intptr_t) (base), (alignment)))

extern "C" void Com_Memcpy( void *dest, const void *src, int count );
void Com_Memset( void *dest, int val, int count );
#define Com_Memcmp memcmp

#define HUNK_MAGIC  0x89537892
#define HUNK_FREE_MAGIC 0x89537893

#define FILEDATA_HASH_SIZE 1024

typedef struct
{
	int permanent;
	int temp;
} hunkUsed_t;

struct hunkHeader_t
{
	unsigned int magic;
	int size;
	const char *name;
	int dummy;
};

#pragma pack(push, 1)
typedef struct fileData_s
{
	void *data;
	fileData_s *next;
	unsigned char type;
	char name[1];
} fileData_t;
#pragma pack(pop)

enum filedata_type_t
{
	FILEDATA_AITYPE,
	FILEDATA_XMODELPIECES,
	FILEDATA_XMODELSURFS,
	FILEDATA_XMODELPARTS,
	FILEDATA_XMODEL,
	FILEDATA_XANIM,
	FILEDATA_XANIMLIST,
	FILEDATA_STRINGTABLE,
	FILEDATA_VEHICLEDEF
};

void *Z_TryMallocInternal( int size );
extern "C" void *Z_MallocInternal( int size );
void *Z_MallocGarbageInternal( int size );
extern "C" void Z_FreeInternal( void *ptr );
void *Z_VirtualAllocInternal( unsigned int size );
void Z_VirtualFreeInternal( void *ptr );
void Z_VirtualDecommitInternal( void *ptr, int size );
void *Z_VirtualReserveInternal( int size );
void Z_VirtualCommitInternal( void *ptr, int size );
char* CopyStringInternal(const char *in);
void ReplaceStringInternal( const char **str, const char *in );
void* Hunk_FindDataForFile(int type, const char *name);
void Hunk_OverrideDataForFile(int type, const char *name, void *data);
void Hunk_AddData(int type, void *data, void *(*alloc)(int));
const char* Hunk_SetDataForFile(int type, const char *name, void *data, void *(*alloc)(int));
qboolean Hunk_DataOnHunk(void *data);
void* Hunk_AllocAlignInternal(int size, int aligment);
void *Hunk_AllocInternal( int size );
void* Hunk_AllocateTempMemoryInternal(int size);
void* Hunk_AllocateTempMemoryHighInternal(int size);
void* Hunk_AllocLowAlignInternal(int size, int aligment);
void* Hunk_AllocLowInternal(int n);
void Hunk_ConvertTempToPermLowInternal();
void* Hunk_ReallocateTempMemoryInternal(int size);
void Hunk_FreeTempMemory(void* buf);
void Hunk_ClearData();
union XAssetHeader
{
	XModel *model;
	void *data;
};
void DB_EnumXAssets(XAssetType type, void (*func)(XAssetHeader, void *), void *inData, bool includeOverride);
void Hunk_ClearTempMemoryInternal();
void Hunk_ClearTempMemoryHighInternal();
void Hunk_ClearHigh(int memory);
void Hunk_ClearLow(int memory);
void Hunk_Clear();
int Hunk_SetMark();
int Hunk_ClearTempMemoryHigh();
void Hunk_Shutdown();
char* TempMalloc(int size);
char* TempMallocAlign(int size);
char* TempMallocAlignStrict(int size);
void TempMemorySetPos(char *pos);
void TempMemoryReset();
int Hunk_HideTempMemory();
void Hunk_ShowTempMemory(int memory);
void Com_InitHunkMemory( void );

#define Z_Malloc Z_MallocInternal
#define S_Malloc Z_MallocInternal
#define Z_MallocGarbage Z_MallocGarbageInternal
#define Z_Free Z_FreeInternal
#define Z_VirtualAlloc Z_VirtualAllocInternal
#define Z_VirtualFree Z_VirtualFreeInternal

#define Hunk_Alloc Hunk_AllocInternal
#define Hunk_AllocateTempMemory Hunk_AllocateTempMemoryInternal

#define CopyString CopyStringInternal
#define FreeString Z_FreeInternal

#ifdef __cplusplus
// Scoped temp-hunk buffer for large stack-hungry locals; 16 bytes in 1.0
// frames, fields past pos not referenced by any caller.
class LargeLocal
{
public:
	LargeLocal( int size );
	~LargeLocal();
	void *GetBuf();
private:
	int pos;
	int reserved[3];
};
#endif
