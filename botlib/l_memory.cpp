#include "qcommon.h"

#define MEM_ID 0x12345678

void *GetMemory( int size )
{
	void *ptr;
	int *memid;

	ptr = Z_Malloc(size + sizeof(int));

	if ( !ptr )
	{
		return NULL;
	}

	memid = (int *)ptr;
	*memid = MEM_ID;

	return (char *)ptr + sizeof(int);
}

void *GetClearedMemory( int size )
{
	void *ptr;

	ptr = GetMemory(size);
	memset(ptr, 0, size);

	return ptr;
}

void FreeMemory( void *ptr )
{
	int *memid;

	memid = (int *)((char *)ptr - sizeof(int));

	if ( *memid == MEM_ID )
	{
		Z_Free(memid);
	}
}

