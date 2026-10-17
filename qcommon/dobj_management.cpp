#include "qcommon.h"

#define DOBJ_HANDLE_MAX (2 << 10)

static DObj objBuf[DOBJ_HANDLE_MAX] __attribute__((aligned(128)));
static bool objAlloced[DOBJ_HANDLE_MAX] __attribute__((aligned(128)));
static int objFreeCount;

// the maps are entity-indexed, not handle-indexed: the client side carries the
// extra per-client view slots, the server side is one entry per gentity
static short clientObjMap[MAX_GENTITIES + 2 * MAX_CLIENTS] __attribute__((aligned(128)));
static short serverObjMap[MAX_GENTITIES];

static int com_lastDObjIndex;
static qboolean g_bDObjInited;

void Com_ShutdownDObj(void);


DObj* Com_GetServerDObj( int handle );
void Com_ServerDObjCreate( DObjModel_s *dobjModels, unsigned short numModels, XAnimTree_s *tree, int handle );
void Com_SafeClientDObjFree( int handle );
void Com_SafeServerDObjFree( int handle );
void Com_ShutdownDObj(void);
void Com_AbortDObj();

/*
==================
Com_SafeClientDObjFree
==================
*/
void Com_SafeClientDObjFree( int handle )
{
	int index;

	assert(((unsigned)handle < ((((1<<10) + 512)) + 1)));
	assert(((unsigned)handle < (sizeof( clientObjMap ) / (sizeof( clientObjMap[0] ) * (sizeof( clientObjMap ) != 4 || sizeof( clientObjMap[0] ) <= 4)))));

	index = clientObjMap[handle];

	if ( !index )
	{
		return;
	}

	clientObjMap[handle] = 0;
	assert((unsigned)index < ARRAY_COUNT( objAlloced ));

	objAlloced[index] = false;
	objFreeCount++;

	DObjFree(&objBuf[index]);
}

DObj* Com_GetClientDObj( int handle, int localClientNum )
{
	handle += 1152 * localClientNum;

	if ( clientObjMap[handle] )
		return &objBuf[clientObjMap[handle]];
	else
		return NULL;
}

/*
==================
Com_GetServerDObj
==================
*/
DObj* Com_GetServerDObj( int handle )
{
	assert((unsigned)handle < ARRAY_COUNT(serverObjMap));
	assert((unsigned)serverObjMap[handle] < DOBJ_HANDLE_MAX);

	return serverObjMap[handle] ? &objBuf[serverObjMap[handle]] : NULL;
}

int Com_GetFreeDObjIndex()
{
	int index;

	for ( index = com_lastDObjIndex + 1; index < 2048; index++ )
	{
		if ( !objAlloced[index] )
		{
			com_lastDObjIndex = index;
			objAlloced[index] = true;
			objFreeCount--;
			return index;
		}
	}

	for ( index = 1; index <= com_lastDObjIndex; index++ )
	{
		if ( !objAlloced[index] )
		{
			com_lastDObjIndex = index;
			objAlloced[index] = true;
			objFreeCount--;
			return index;
		}
	}

	return 0;
}

void Com_ClientDObjCreate( DObjModel_s *dobjModels, unsigned short numModels, XAnimTree_s *tree, int handle )
{
	int index;

	index = Com_GetFreeDObjIndex();
	clientObjMap[handle] = index;
	DObjCreate(dobjModels, numModels, tree, &objBuf[index], 0);

	if ( !objFreeCount )
		Com_Error(ERR_DROP, "\x15No free DObjs");
}

void Com_ClientDObjClearAllSkel()
{
	int handle;
	int index;

	for ( handle = 0; handle < 1152; handle++ )
	{
		index = clientObjMap[handle];

		if ( !index )
			continue;

		DObjSkelClear(&objBuf[index]);
	}
}

/*
==================
Com_ServerDObjCreate
==================
*/
void Com_ServerDObjCreate( DObjModel_s *dobjModels, unsigned short numModels, XAnimTree_s *tree, int handle )
{
	int index;

	assert(dobjModels);
	assert(((unsigned)handle < ((1<<10))));
	assert(!Com_GetServerDObj( handle ));

	index = Com_GetFreeDObjIndex();
	assert((unsigned)handle < ARRAY_COUNT( serverObjMap ));

	serverObjMap[handle] = index;
	assert((unsigned)index < DOBJ_HANDLE_MAX);

	DObjCreate(dobjModels, numModels, tree, &objBuf[index], (unsigned short)(handle + 1));

	if ( !objFreeCount )
	{
		Com_Error(ERR_DROP, "\x15No free DObjs");
	}
}

/*
==================
Com_SafeServerDObjFree
==================
*/
void Com_SafeServerDObjFree( int handle )
{
	int index;

	assert(((unsigned)handle < ((1<<10))));
	index = serverObjMap[handle];

	if ( !index )
	{
		return;
	}

	serverObjMap[handle] = 0;
	assert((unsigned)index < ARRAY_COUNT( objAlloced ));

	objAlloced[index] = false;
	objFreeCount++;

	DObjFree(&objBuf[index]);
}

/*
==================
Com_ShutdownDObj
==================
*/
void Com_InitDObj()
{
	Com_Memset(objAlloced, 0, sizeof(objAlloced));
	objFreeCount = DOBJ_HANDLE_MAX - 1;

	Com_Memset(clientObjMap, 0, sizeof(clientObjMap));
	Com_Memset(serverObjMap, 0, sizeof(serverObjMap));

	com_lastDObjIndex = 1;
	g_bDObjInited = qtrue;
}

void Com_ShutdownDObj(void)
{
	if (!g_bDObjInited)
		return;

	g_bDObjInited = qfalse;
}

/*
==================
Com_AbortDObj
==================
*/
void Com_AbortDObj()
{
	g_bDObjInited = qfalse;
}

