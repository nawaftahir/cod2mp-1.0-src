#include "../qcommon/qcommon.h"
#include "script_public.h"

unsigned int FindVariableIndexHash( unsigned int parentId, unsigned int name );

scrVarGlob_t scrVarGlob;
scrVarPub_t scrVarPub;

static struct scr_classStruct_t scrClassMap[] =
{
	{ 0, 0, 'e', "entity" },
	{ 0, 0, 'h', "hudelem" },
	{ 0, 0, 'p', "pathnode" },
	{ 0, 0, 'v', "vehiclenode" }
};

const char *var_typename[] =
{
	"undefined",
	"object",
	"string",
	"localized string",
	"vector",
	"float",
	"int",
	"codepos",
	"precodepos",
	"function",
	"stack",
	"animation",
	"developer codepos",
	"include codepos",
	"thread list",
	"thread",
	"thread",
	"thread",
	"thread",
	"struct",
	"removed entity",
	"entity",
	"array",
	"removed thread"
};
/*
==============
Scr_DumpScriptThreads
==============
*/
void *Z_TryMallocInternal( int size );

unsigned short Scr_GetThreadNotifyName( unsigned int startLocalId );
void Scr_KillEndonThread( unsigned int threadId );
void FreeValue( unsigned int id );
void AddRefToObject( unsigned int id );
void RemoveRefToObject( unsigned int id );
void RemoveRefToEmptyObject( unsigned int id );
void AddRefToValue( int type, VariableUnion u );
void RemoveRefToValue( int type, VariableUnion u );
unsigned int FindVariable( unsigned int parentId, unsigned int index );
unsigned int FindObjectVariable( unsigned int parentId, unsigned int id );
VariableValue Scr_GetArrayIndexValue( unsigned int name );
unsigned int GetVariable( unsigned int parentId, unsigned int name );
void RemoveVariable( unsigned int parentId, unsigned int name );
void RemoveObjectVariable( unsigned int parentId, unsigned int id );
void RemoveArrayVariable( unsigned int parentId, unsigned int index );
void SafeRemoveVariable( unsigned int parentId, unsigned int name );
void SetNewVariableValue( unsigned int id, VariableValue *value );
VariableUnion *GetVariableValueAddress( unsigned int id );
VariableValue Scr_EvalVariable( unsigned int id );
VariableValue Scr_EvalVariableEntityField( unsigned int entId, unsigned int name );
unsigned int GetArraySize( unsigned int id );
unsigned int FindNextSibling( unsigned int id );
unsigned int FindObject( unsigned int id );
void Scr_CastBool( VariableValue *value );
float Scr_GetEntryUsage( VariableValueInternal *entryValue );
float Scr_GetObjectUsage( unsigned int parentId );
float Scr_GetThreadUsage( VariableStackBuffer *stackBuf, float *endonUsage );
void Scr_AddFields( const char *path, const char *extension );

bool IsObjectValue( VariableValueInternal *entryValue );
bool IsObjectId( unsigned int id );
int ThreadInfoCompare( const void *info1, const void *info2 );
void Scr_DumpScriptVariablesDefault();
void InitVariables();
void Var_Init();
void Scr_ShutdownVariables();
unsigned int Scr_GetNumScriptVars();
unsigned int GetVariableKeyObject( unsigned int id );
unsigned int FindVariableIndexInternal( unsigned int name, unsigned int index );
unsigned int FindVariableIndexHash( unsigned int parentId, unsigned int name );
unsigned int GetNewVariableIndexInternal3( unsigned int parentId, unsigned int name, unsigned int index );
unsigned int GetNewVariableIndexInternal2( unsigned int parentId, unsigned int name, unsigned int index );
unsigned int GetNewVariableIndexReverseInternal2( unsigned int parentId, unsigned int name, unsigned int index );
unsigned int GetNewVariableIndexInternal( unsigned int parentId, unsigned int name );
unsigned int GetNewVariableIndexReverseInternal( unsigned int parentId, unsigned int name );
unsigned int GetVariableIndexInternal( unsigned int parentId, unsigned int name );
void MakeVariableExternal( VariableValueInternal *entry, VariableValueInternal *parentValue );
void ClearObjectInternal( unsigned int parentId );
void ClearObject( unsigned int parentId );
void Scr_SetThreadNotifyName( unsigned int startLocalId, unsigned int notifyName );
void Scr_ClearThread( unsigned int parentId );
void Scr_StopThread( unsigned int threadId );
void Scr_RemoveThreadNotifyName( unsigned int startLocalId );
void Scr_RemoveThreadEmptyNotifyName( unsigned int startLocalId );
unsigned short Scr_GetThreadNotifyName( unsigned int startLocalId );
void Scr_SetThreadWaitTime( unsigned int startLocalId, unsigned int waitTime );
void Scr_ClearWaitTime( unsigned int startLocalId );
unsigned int Scr_GetThreadWaitTime( unsigned int startLocalId );
unsigned int GetParentLocalId( unsigned int threadId );
unsigned int GetSafeParentLocalId( unsigned int threadId );
unsigned int GetStartLocalId( unsigned int threadId );
void Scr_KillThread( unsigned int parentId );
void Scr_KillEndonThread( unsigned int threadId );
unsigned short AllocVariable();
void FreeVariable( unsigned int id );
unsigned int AllocValue();
unsigned int AllocObject();
unsigned int AllocEntity( int classnum, unsigned short entnum );
unsigned int Scr_AllocArray();
unsigned int AllocThread( unsigned int self );
unsigned int AllocChildThread( unsigned int self, unsigned int parentLocalId );
unsigned int Scr_GetSelf( unsigned int threadId );
void FreeValue( unsigned int id );
void AddRefToObject( unsigned int id );
void RemoveRefToObject( unsigned int id );
void RemoveRefToEmptyObject( unsigned int id );
int Scr_GetRefCountToObject( unsigned int id );
float *Scr_AllocVectorInternal();
float *Scr_AllocVector( const float *v );
void AddRefToVector( const float *vectorValue );
void RemoveRefToVector( const vec3_t vectorValue );
void AddRefToValue( int type, VariableUnion u );
void RemoveRefToValue( int type, VariableUnion u );
bool IsValidArrayIndex( unsigned int index );
unsigned int GetInternalVariableIndex( unsigned int index );
unsigned int FindArrayVariableIndex( unsigned int parentId, unsigned int index );
unsigned int FindArrayVariable( unsigned int parentId, unsigned int index );
unsigned int FindVariable( unsigned int parentId, unsigned int index );
unsigned int FindObjectVariable( unsigned int parentId, unsigned int id );
unsigned int GetArrayVariableIndex( unsigned int parentId, unsigned int index );
unsigned int GetNewArrayVariableIndex( unsigned int parentId, unsigned int index );
VariableValue Scr_FindVariableField( unsigned int parentId, unsigned int name );
unsigned int Scr_FindAllVariableField( unsigned int parentId, unsigned int *names );
VariableValue Scr_GetArrayIndexValue( unsigned int name );
void ClearVariableField( unsigned int parentId, unsigned int name, VariableValue *value );
unsigned int GetArrayVariable( unsigned int parentId, unsigned int name );
unsigned int GetNewArrayVariable( unsigned int parentId, unsigned int name );
unsigned int GetVariable( unsigned int parentId, unsigned int name );
unsigned int GetNewVariable( unsigned int parentId, unsigned int name );
unsigned int GetObjectVariable( unsigned int parentId, unsigned int id );
unsigned int GetNewObjectVariable( unsigned int parentId, unsigned int id );
unsigned int GetNewObjectVariableReverse( unsigned int parentId, unsigned int id );
void RemoveVariable( unsigned int parentId, unsigned int name );
void RemoveNextVariable( unsigned int parentId );
void RemoveObjectVariable( unsigned int parentId, unsigned int id );
void SafeRemoveArrayVariable( unsigned int parentId, unsigned int index );
void RemoveArrayVariable( unsigned int parentId, unsigned int index );
void SafeRemoveVariable( unsigned int parentId, unsigned int name );
void CopyArray( unsigned int parentId, unsigned int newParentId );
void SetVariableValue( unsigned int id, VariableValue *value );
void SetNewVariableValue( unsigned int id, VariableValue *value );
VariableUnion *GetVariableValueAddress( unsigned int id );
void SetVariableEntityFieldValue( unsigned int entId, unsigned int fieldName, VariableValue *value );
__declspec(noinline) void ClearVariableValue( unsigned int id );
void SetVariableFieldValue( unsigned int id, VariableValue *value );
VariableValue Scr_EvalVariable( unsigned int id );
unsigned int Scr_EvalVariableObject( unsigned int id );
VariableValue Scr_EvalVariableEntityField( unsigned int entId, unsigned int name );
VariableValue Scr_EvalVariableFieldInternal( unsigned int id );
void Scr_EvalSizeValue( VariableValue *value );
unsigned int GetArraySize( unsigned int id );
unsigned int FindNextSibling( unsigned int id );
unsigned int FindPrevSibling( unsigned int id );
unsigned int GetVariableName( unsigned int id );
unsigned int GetObjectA( unsigned int id );
unsigned int GetArray( unsigned int id );
unsigned int FindObject( unsigned int id );
bool IsFieldObject( unsigned int id );
int Scr_IsThreadAlive( unsigned int thread );
void Scr_EvalBoolNot( VariableValue *value );
void Scr_EvalBoolComplement( VariableValue *value );
void Scr_CastBool( VariableValue *value );
bool Scr_CastString( VariableValue *value );
void Scr_CastDebugString( VariableValue *value );
char Scr_GetEntClassId( unsigned int id );
int Scr_GetEntNum( unsigned int id );
void Scr_ClearVector( VariableValue *value );
void Scr_CastVector( VariableValue *value );
unsigned int Scr_EvalFieldObject( unsigned int tempVariable, VariableValue *value );
void Scr_UnmatchingTypesError( VariableValue *value1, VariableValue *value2 );
void Scr_CastWeakerPair( VariableValue *value1, VariableValue *value2 );
void Scr_CastWeakerStringPair( VariableValue *value1, VariableValue *value2 );
void Scr_EvalOr( VariableValue *value1, VariableValue *value2 );
void Scr_EvalExOr( VariableValue *value1, VariableValue *value2 );
void Scr_EvalAnd( VariableValue *value1, VariableValue *value2 );
void Scr_EvalEquality( VariableValue *value1, VariableValue *value2 );
void Scr_EvalInequality( VariableValue *value1, VariableValue *value2 );
void Scr_EvalLess(VariableValue *value1, VariableValue *value2);
void Scr_EvalGreaterEqual( VariableValue *value1, VariableValue *value2 );
void Scr_EvalGreater( VariableValue *value1, VariableValue *value2 );
void Scr_EvalLessEqual( VariableValue *value1, VariableValue *value2 );
void Scr_EvalShiftLeft( VariableValue *value1, VariableValue *value2 );
void Scr_EvalShiftRight( VariableValue *value1, VariableValue *value2 );
void Scr_EvalPlus( VariableValue *value1, VariableValue *value2 );
void Scr_EvalMinus( VariableValue *value1, VariableValue *value2 );
void Scr_EvalMultiply( VariableValue *value1, VariableValue *value2 );
void Scr_EvalDivide( VariableValue *value1, VariableValue *value2 );
void Scr_EvalMod( VariableValue *value1, VariableValue *value2 );
void Scr_EvalBinaryOperator( int op, VariableValue *value1, VariableValue *value2 );
bool IsObjectFree( unsigned int id );
int GetObjectType( unsigned int id );
void Scr_FreeEntityNum( int entnum, int classnum );
void Scr_FreeEntityList();
void Scr_FreeObjects();
void Scr_SetClassMap( int classnum );
void Scr_RemoveClassMap( int classnum );
void Scr_AddClassField( int classnum, const char *name, unsigned int offset );
int Scr_GetOffset( unsigned int classnum, const char *name );
unsigned int FindEntityId( int entnum, int classnum );
unsigned int Scr_GetEntityId( int entnum, int classnum );
unsigned int Scr_EvalArrayIndex( unsigned int parentId, VariableValue *index );
unsigned int Scr_FindArrayIndex( unsigned int parentId, VariableValue *index );
void Scr_EvalArray( VariableValue *value, VariableValue *index );
unsigned int Scr_EvalArrayRef( unsigned int parentId );
void ClearArray( unsigned int parentId, VariableValue *value );
void SetEmptyArray( unsigned int parentId );
scr_entref_t Scr_GetEntityIdRef( unsigned int entId );
void CopyEntity( unsigned int parentId, unsigned int newParentId );
void Scr_CopyEntityNum( int fromEntnum, int toEntnum, int classnum );
float Scr_GetEntryUsage( VariableValueInternal *entryValue );
float Scr_GetEntryUsage( VariableValueInternal *entryValue );
float Scr_GetEndonUsage( unsigned int parentId );
float Scr_GetObjectUsage( unsigned int parentId );
float Scr_GetThreadUsage( VariableStackBuffer *stackBuf, float *endonUsage );
unsigned int Scr_FindField( const char *name, int *type );
void Scr_AddFieldsForFile( const char *filename );
void Scr_AddFields( const char *path, const char *extension );
void Scr_FreeValue( unsigned int id );
int Scr_MakeValuePrimitive( unsigned int parentId );
void Scr_AllocGameVariable();
void Scr_FreeGameVariable( int bComplete );
void Scr_GetChecksum( int *checksum );
int Scr_GetClassnumForCharId( char charId );
unsigned int Scr_InitStringSet();
bool Scr_AddStringSet( unsigned int setId, const char *string );
void Scr_ShutdownStringSet( unsigned int setId );

/*
==============
IsObjectValue
==============
*/
bool IsObjectValue( VariableValueInternal *entryValue )
{
	return IsObject( entryValue );
}

/*
==============
IsObjectId
==============
*/
bool IsObjectId( unsigned int id )
{
	return GetObjectType( id ) >= VAR_THREAD;
}

/*
==============
ThreadInfoCompare
==============
*/
int ThreadInfoCompare( const void *info1, const void *info2 )
{
	int i;
	const char *pos1;
	const char *pos2;

	for ( i = 0; ; i++ )
	{
		if ( i >= ((ThreadDebugInfo *)info1)->posSize || i >= ((ThreadDebugInfo *)info2)->posSize )
		{
			return ((ThreadDebugInfo *)info1)->posSize - ((ThreadDebugInfo *)info2)->posSize;
		}

		pos1 = ((ThreadDebugInfo *)info1)->pos[i];
		pos2 = ((ThreadDebugInfo *)info2)->pos[i];

		if ( pos1 != pos2 )
		{
			return pos1 - pos2;
		}
	}
}

void Scr_DumpScriptThreads()
{
	unsigned int id;
	VariableValueInternal *entryValue;
	ThreadDebugInfo *infoArray;
	int num;
	ThreadDebugInfo *pInfo;
	int i;
	int j;
	int count;
	unsigned int classnum;
	unsigned int entId;
	ThreadDebugInfo info;
	VariableStackBuffer *stackBuf;
	int size;
	const char *pos;
	const char *buf;
	unsigned char type;
	const char *value;

	infoArray = (ThreadDebugInfo *)Z_TryMallocInternal( sizeof( *infoArray ) * VARIABLELIST_CHILD_SIZE );

	if ( !infoArray )
	{
		Com_Printf("Cannot dump script threads: out of memory\n");
		return;
	}

	num = 0;

	for ( id = 1; id < VARIABLELIST_CHILD_SIZE; id++ )
	{
		entryValue = &scrVarGlob.variableList[id];

		if ( (entryValue->w.status & VAR_STAT_MASK) == VAR_STAT_FREE )
		{
			continue;
		}

		if ( (entryValue->w.type & VAR_MASK) != VAR_STACK )
		{
			continue;
		}

		pInfo = &infoArray[num];

		num++;
		info.posSize = 0;

		stackBuf = entryValue->u.u.stackValue;

		size = stackBuf->size;
		pos = stackBuf->pos;
		buf = stackBuf->buf;

		while ( size )
		{
			size--;

			type = *(unsigned char *)buf;
			buf += sizeof( unsigned char );

			value = *(const char **)buf;
			buf += sizeof( VariableUnion );

			if ( type == VAR_CODEPOS )
			{
				info.pos[info.posSize] = value;
				info.posSize++;
			}
		}

		info.pos[info.posSize] = pos;
		info.posSize++;

		pInfo->varUsage = Scr_GetThreadUsage(stackBuf, &pInfo->endonUsage);
		pInfo->posSize = info.posSize;

		info.posSize--;

		for ( j = 0; j < pInfo->posSize; j++ )
		{
			pInfo->pos[j] = info.pos[info.posSize - j];
		}
	}

	qsort(infoArray, num, sizeof(*infoArray), ThreadInfoCompare);
	Com_Printf("********************************\n");

	i = 0;

	while ( i < num )
	{
		pInfo = &infoArray[i];

		count = 0;
		info.varUsage = 0;
		info.endonUsage = 0;

		do
		{
			count++;

			info.varUsage = info.varUsage + infoArray[i].varUsage;
			info.endonUsage = info.endonUsage + infoArray[i].endonUsage;

			i++;
		}
		while ( i < num && !ThreadInfoCompare(pInfo, &infoArray[i]) );

		Com_Printf("count: %d, var usage: %d, endon usage: %d\n", count, (int)info.varUsage, (int)info.endonUsage);
		Scr_PrintPrevCodePos(PMSG_CONSOLE, pInfo->pos[0], 0);

		for ( j = 1; j < pInfo->posSize; j++ )
		{
			Com_Printf("called from:\n");
			Scr_PrintPrevCodePos(PMSG_CONSOLE, pInfo->pos[j], 0);
		}
	}

	Z_Free(infoArray);
	Com_Printf("********************************\n");

	for ( classnum = 0; classnum < CLASS_NUM_COUNT; classnum++ )
	{
		if ( !scrClassMap[classnum].entArrayId )
		{
			continue;
		}

		info.varUsage = 0;
		count = 0;

		for ( entId = FindNextSibling(scrClassMap[classnum].entArrayId);
			entId;
			entId = FindNextSibling(entId) )
		{
			count++;

			if ( (scrVarGlob.variableList[entId].w.type & VAR_MASK) != VAR_POINTER )
			{
				continue;
			}

			info.varUsage = Scr_GetObjectUsage(scrVarGlob.variableList[entId].u.u.pointerValue) + info.varUsage;
		}

		Com_Printf("ent type '%s'... count: %d, var usage: %d\n", scrClassMap[classnum].name, count, (int)info.varUsage);
	}

	Com_Printf("********************************\n");
}

/*
==============
Scr_DumpScriptVariablesDefault
==============
*/
void Scr_DumpScriptVariablesDefault()
{
}

/*
==============
InitVariables
==============
*/
void InitVariables()
{
	unsigned int index;
	VariableValueInternal *entry;
	unsigned short prev;
	VariableValueInternal *value;

	prev = 0;

	for ( index = 1; index < VARIABLELIST_CHILD_SIZE; index++ )
	{
		value = &scrVarGlob.variableList[index];
		entry = value;
		value->w.status = 0;
		entry->hash.id = index;
		value->v.next = index;
		scrVarGlob.variableList[prev].u.next = index;
		entry->hash.u.prev = prev;
		prev = index;
	}

	value = &scrVarGlob.variableList[0];
	value->w.status = 0;
	value->w.type = value->w.status;
	value->hash.id = 0;
	value->v.next = 0;
	scrVarGlob.variableList[prev].u.next = 0;
	value->hash.u.prev = prev;
}

/*
==============
Var_Init
==============
*/
void Var_Init()
{
	int i;

	InitVariables();

	for ( i = 0; i < CLASS_NUM_COUNT; i++ )
	{
		scrClassMap[i].entArrayId = 0;
		scrClassMap[i].id = 0;
	}
}

/*
==============
Scr_ShutdownVariables
==============
*/
void Scr_ShutdownVariables()
{
	if ( scrVarPub.gameId )
	{
		FreeValue(scrVarPub.gameId);
		scrVarPub.gameId = 0;
	}
}

/*
==============
Scr_GetNumScriptVars
==============
*/
unsigned int Scr_GetNumScriptVars()
{
	return 0;
}

/*
==============
GetVariableKeyObject
==============
*/
unsigned int GetVariableKeyObject( unsigned int id )
{
	return ( scrVarGlob.variableList[id].w.name >> VAR_NAME_BITS ) - SL_MAX_STRING_INDEX;
}

/*
==============
FindVariableIndexInternal
==============
*/
unsigned int FindVariableIndexInternal( unsigned int name, unsigned int index )
{
	VariableValueInternal *entryValue;
	VariableValueInternal *entry;
	VariableValueInternal *newEntry;
	unsigned int newIndex;
	VariableValueInternal *newEntryValue;

	entry = &scrVarGlob.variableList[index];
	entryValue = &scrVarGlob.variableList[entry->hash.id];

	if ( (entryValue->w.status & VAR_STAT_MASK) == VAR_STAT_HEAD )
	{
		if ( entryValue->w.name >> VAR_NAME_BITS == name )
		{
			return index;
		}

		newIndex = entryValue->v.index;

		for ( newEntry = &scrVarGlob.variableList[newIndex]; newEntry != entry; newEntry = &scrVarGlob.variableList[newIndex] )
		{
			newEntryValue = &scrVarGlob.variableList[newEntry->hash.id];

			if ( newEntryValue->w.name >> VAR_NAME_BITS == name )
			{
				return newIndex;
			}

			newIndex = newEntryValue->v.index;
		}
	}

	return 0;
}

/*
==============
FindVariableIndexHash
==============
*/
unsigned int FindVariableIndexHash( unsigned int parentId, unsigned int name )
{
	unsigned short hash;

	hash = ( name + parentId ) % ( VARIABLELIST_CHILD_SIZE - 1 ) + 1;
	return FindVariableIndexInternal( name, hash );
}

/*
==============
GetNewVariableIndexInternal3
==============
*/
unsigned int GetNewVariableIndexInternal3( unsigned int parentId, unsigned int name, unsigned int index )
{
	VariableValueInternal *entryValue, *entry;
	unsigned short nextSiblingIndex, prev, next, prevId;
	VariableValueInternal *newEntry;
	unsigned short id, newIndex;
	VariableValueInternal *newEntryValue, *parentValue;
	VariableValue value;
	int type;

	assert(!(name & ~VAR_NAME_LOW_MASK));

	entry = &scrVarGlob.variableList[index];
	entryValue = &scrVarGlob.variableList[entry->hash.id];
	type = entryValue->w.status & VAR_STAT_MASK;

	if ( type == VAR_STAT_FREE )
	{
		newIndex = entry->v.index;
		next = entryValue->u.next;

		if ( newIndex != entry->hash.id && !(entry->w.status & VAR_STAT_MASK) )
		{
			scrVarGlob.variableList[newIndex].hash.id = entry->hash.id;
			entry->hash.id = index;

			entryValue->v.index = newIndex;
			entryValue->u.next = entry->u.next;

			newEntryValue = entry;
		}
		else
		{
			newEntryValue = entryValue;
		}

		prev = entry->hash.u.prev;

		assert(!scrVarGlob.variableList[prev].hash.id || (scrVarGlob.variableList[scrVarGlob.variableList[prev].hash.id].w.status & VAR_STAT_MASK) == VAR_STAT_FREE);
		assert(!scrVarGlob.variableList[next].hash.id || (scrVarGlob.variableList[scrVarGlob.variableList[next].hash.id].w.status & VAR_STAT_MASK) == VAR_STAT_FREE);

		scrVarGlob.variableList[scrVarGlob.variableList[prev].hash.id].u.next = next;
		scrVarGlob.variableList[next].hash.u.prev = prev;

		newEntryValue->w.status = VAR_STAT_HEAD;
		newEntryValue->v.index = index;
	}
	else if ( type == VAR_STAT_HEAD )
	{
		if ( !(entry->w.status & VAR_STAT_MASK) )
		{
			newIndex = entry->v.index;
			newEntry = &scrVarGlob.variableList[newIndex];
			newEntryValue = entry;

			prev = newEntry->hash.u.prev;
			next = newEntryValue->u.next;

			scrVarGlob.variableList[scrVarGlob.variableList[prev].hash.id].u.next = next;
			scrVarGlob.variableList[next].hash.u.prevSibling = prev;

			newEntry->hash.id = entry->hash.id;
			entry->hash.id = index;
			newEntry->hash.u.prev = entry->hash.u.prev;

			scrVarGlob.variableList[scrVarGlob.variableList[newEntry->hash.u.prev].hash.id].nextSibling = newIndex;
			scrVarGlob.variableList[entryValue->nextSibling].hash.u.prevSibling = newIndex;

			entryValue->w.type &= ~VAR_STAT_MASK;
			entryValue->w.type |= VAR_STAT_MOVABLE;

			newEntryValue->w.status = VAR_STAT_HEAD;
		}
		else
		{
			index = scrVarGlob.variableList[0].u.next;

			if ( !index )
			{
				Scr_TerminalError("exceeded maximum number of script variables");
			}

			entry = &scrVarGlob.variableList[index];
			newEntryValue = &scrVarGlob.variableList[entry->hash.id];
			assert((newEntryValue->w.status & VAR_STAT_MASK) == VAR_STAT_FREE);
			next = newEntryValue->u.next;

			scrVarGlob.variableList[0].u.next = next;
			scrVarGlob.variableList[next].hash.u.prev = 0;

			newEntryValue->w.status = VAR_STAT_MOVABLE;
			newEntryValue->v.next = entryValue->v.next;

			entryValue->v.index = index;
		}
	}
	else
	{
		assert(type == VAR_STAT_MOVABLE || type == VAR_STAT_EXTERNAL);
		if ( !(entry->w.status & VAR_STAT_MASK) )
		{
			assert(entry != entryValue);
			newIndex = entry->v.index;
			newEntry = &scrVarGlob.variableList[newIndex];
			newEntryValue = entry;

			prev = newEntry->hash.u.prev;
			next = newEntryValue->u.next;

			scrVarGlob.variableList[scrVarGlob.variableList[prev].hash.id].u.next = next;
			scrVarGlob.variableList[next].hash.u.prev = prev;
		}
		else
		{
			newIndex = scrVarGlob.variableList[0].u.next;

			if ( !newIndex )
			{
				Scr_TerminalError("exceeded maximum number of script variables");
			}

			newEntry = &scrVarGlob.variableList[newIndex];
			newEntryValue = &scrVarGlob.variableList[newEntry->hash.id];
			assert((newEntryValue->w.status & VAR_STAT_MASK) == VAR_STAT_FREE);

			next = newEntryValue->u.next;

			scrVarGlob.variableList[0].u.next = next;
			scrVarGlob.variableList[next].hash.u.prev = 0;
		}

		nextSiblingIndex = entryValue->nextSibling;

		scrVarGlob.variableList[scrVarGlob.variableList[entry->hash.u.prev].hash.id].nextSibling = newIndex;
		scrVarGlob.variableList[nextSiblingIndex].hash.u.prev = newIndex;

		if ( type == VAR_STAT_MOVABLE )
		{
			nextSiblingIndex = entryValue->v.index;
			prevId = scrVarGlob.variableList[nextSiblingIndex].hash.id;
			assert((scrVarGlob.variableList[prevId].w.status & VAR_STAT_MASK) == VAR_STAT_MOVABLE || (scrVarGlob.variableList[prevId].w.status & VAR_STAT_MASK) == VAR_STAT_HEAD);

			while ( 1 )
			{
				if ( scrVarGlob.variableList[prevId].v.index == index )
				{
					break;
				}

				prevId = scrVarGlob.variableList[scrVarGlob.variableList[prevId].v.next].hash.id;
				assert((scrVarGlob.variableList[prevId].w.status & VAR_STAT_MASK) == VAR_STAT_MOVABLE || (scrVarGlob.variableList[prevId].w.status & VAR_STAT_MASK) == VAR_STAT_HEAD);
			}

			scrVarGlob.variableList[prevId].v.index = newIndex;
		}
		else
		{
			assert(type == VAR_STAT_EXTERNAL);
			entryValue->v.index = newIndex;
		}

		newEntry->hash.u.prev = entry->hash.u.prev;
		id = newEntry->hash.id;
		newEntry->hash.id = entry->hash.id;
		entry->hash.id = id;
		newEntryValue->w.status = VAR_STAT_HEAD;
		newEntryValue->v.index = index;
	}

	assert(entry == &scrVarGlob.variableList[index]);
	assert(newEntryValue == &scrVarGlob.variableList[entry->hash.id]);

	newEntryValue->w.type &= 0xFF;
	newEntryValue->w.name |= name << VAR_NAME_BITS;

	parentValue = &scrVarGlob.variableList[parentId];

	if ( ( parentValue->w.type & VAR_MASK ) == VAR_ARRAY )
	{
		parentValue->u.o.u.size++;
		value = Scr_GetArrayIndexValue( name );
		AddRefToValue( &value );
	}

	return index;
}

/*
==============
GetNewVariableIndexInternal2
==============
*/
unsigned int GetNewVariableIndexInternal2( unsigned int parentId, unsigned int name, unsigned int index )
{
	VariableValueInternal *entry;
	VariableValueInternal *siblingEntry;
	unsigned short siblingId;
	VariableValueInternal *siblingValue;
	VariableValueInternal *parentValue;

	index = GetNewVariableIndexInternal3(parentId, name, index);
	parentValue = &scrVarGlob.variableList[parentId];
	siblingId = parentValue->nextSibling;
	siblingValue = &scrVarGlob.variableList[siblingId];
	entry = &scrVarGlob.variableList[index];
	siblingEntry = &scrVarGlob.variableList[entry->hash.id];
	siblingEntry->nextSibling = siblingId;
	siblingValue->hash.u.prevSibling = index;
	entry->hash.u.prevSibling = parentValue->v.next;
	parentValue->nextSibling = index;
	return index;
}

/*
==============
GetNewVariableIndexReverseInternal2
==============
*/
unsigned int GetNewVariableIndexReverseInternal2( unsigned int parentId, unsigned int name, unsigned int index )
{
	VariableValueInternal *entry, *entryValue;
	unsigned short siblingId;
	VariableValueInternal *siblingValue, *parentValue, *parent;

	index = GetNewVariableIndexInternal3(parentId, name, index);

	parentValue = &scrVarGlob.variableList[parentId];
	parent = &scrVarGlob.variableList[scrVarGlob.variableList[parentValue->nextSibling].hash.u.prev];

	siblingId = parent->hash.u.prevSibling;
	siblingValue = &scrVarGlob.variableList[scrVarGlob.variableList[siblingId].hash.id];

	entry = &scrVarGlob.variableList[index];
	entryValue = &scrVarGlob.variableList[entry->hash.id];

	entryValue->nextSibling = parentValue->v.next;
	parent->hash.u.prev = index;

	entry->hash.u.prevSibling = siblingId;
	siblingValue->nextSibling = index;

	return index;
}

/*
==============
GetNewVariableIndexInternal
==============
*/
unsigned int GetNewVariableIndexInternal( unsigned int parentId, unsigned int name )
{
	unsigned int hash;

	hash = ( name + parentId ) % ( VARIABLELIST_CHILD_SIZE - 1 ) + 1;
	return GetNewVariableIndexInternal2( parentId, name, hash );
}

/*
==============
GetNewVariableIndexReverseInternal
==============
*/
unsigned int GetNewVariableIndexReverseInternal( unsigned int parentId, unsigned int name )
{
	unsigned int hash;

	hash = ( name + parentId ) % ( VARIABLELIST_CHILD_SIZE - 1 ) + 1;
	return GetNewVariableIndexReverseInternal2( parentId, name, hash );
}

/*
==============
GetVariableIndexInternal
==============
*/
unsigned int GetVariableIndexInternal( unsigned int parentId, unsigned int name )
{
	unsigned int hash;
	unsigned int newIndex;
	unsigned int result;

	hash = ( name + parentId ) % ( VARIABLELIST_CHILD_SIZE - 1 ) + 1;
	newIndex = FindVariableIndexInternal( name, hash );
	result = newIndex;

	if ( !newIndex )
	{
		result = GetNewVariableIndexInternal2( parentId, name, hash );
	}

	return result;
}

/*
==============
MakeVariableExternal
==============
*/
void MakeVariableExternal( VariableValueInternal *entry, VariableValueInternal *parentValue )
{
	unsigned int oldIndex;
	VariableValueInternal *oldEntry;
	VariableValueInternal *prev;
	unsigned int index;
	VariableValueInternal *entryValue;
	VariableValueInternal *oldEntryValue;
	Variable tempEntry;
	unsigned int prevSiblingIndex;
	unsigned int nextSiblingIndex;
	unsigned int oldPrevSiblingIndex;
	unsigned int oldNextSiblingIndex;

	index = entry - scrVarGlob.variableList;
	entryValue = &scrVarGlob.variableList[entry->hash.id];

	if ( (parentValue->w.type & VAR_MASK) == VAR_ARRAY )
	{
		unsigned int name;
		VariableValue value;

		parentValue->u.o.u.size--;

		name = entryValue->w.name >> VAR_NAME_BITS;
		value = Scr_GetArrayIndexValue(name);
		RemoveRefToValue(&value);
	}

	if ( (entryValue->w.status & VAR_STAT_MASK) == VAR_STAT_HEAD )
	{
		oldIndex = entryValue->v.index;
		oldEntry = &scrVarGlob.variableList[oldIndex];

		oldEntryValue = &scrVarGlob.variableList[oldEntry->hash.id];

		if ( oldEntry != entry )
		{
			oldEntryValue->w.type &= ~VAR_STAT_MASK;
			oldEntryValue->w.type |= VAR_STAT_HEAD;

			prevSiblingIndex = entry->hash.u.prevSibling;
			nextSiblingIndex = entryValue->nextSibling;

			oldPrevSiblingIndex = oldEntry->hash.u.prevSibling;
			oldNextSiblingIndex = oldEntryValue->nextSibling;

			scrVarGlob.variableList[oldNextSiblingIndex].hash.u.prev = index;
			scrVarGlob.variableList[scrVarGlob.variableList[oldPrevSiblingIndex].hash.id].nextSibling = index;

			scrVarGlob.variableList[nextSiblingIndex].hash.u.prev = oldIndex;
			scrVarGlob.variableList[scrVarGlob.variableList[prevSiblingIndex].hash.id].nextSibling = oldIndex;

			tempEntry = entry->hash;
			entry->hash = oldEntry->hash;

			oldEntry->hash = tempEntry;
			entry = oldEntry;
			index = oldIndex;
		}
	}
	else
	{
		oldEntry = entry;
		oldEntryValue = entryValue;

		do
		{
			prev = oldEntry;

			oldIndex = oldEntryValue->v.index;
			oldEntry = &scrVarGlob.variableList[oldIndex];

			oldEntryValue = &scrVarGlob.variableList[oldEntry->hash.id];
		}
		while ( oldEntry != entry );

		scrVarGlob.variableList[prev->hash.id].v.index = entryValue->v.index;
	}

	entryValue->w.type &= ~VAR_STAT_MASK;
	entryValue->w.type |= VAR_STAT_EXTERNAL;

	entryValue->v.index = index;
}

/*
==============
ClearObjectInternal
==============
*/
void ClearObjectInternal( unsigned int parentId )
{
	unsigned int nextId, id;
	VariableValueInternal *entryValue, *parentValue;

	parentValue = &scrVarGlob.variableList[parentId];
	assert((parentValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert(IsObject( parentValue ));

	entryValue = &scrVarGlob.variableList[parentValue->nextSibling];
	//assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	//assert(!IsObject( entryValue ));

	for ( nextId = entryValue->hash.id; nextId != parentId; nextId = entryValue->hash.id )
	{
		MakeVariableExternal(entryValue, parentValue);
		entryValue = &scrVarGlob.variableList[scrVarGlob.variableList[nextId].nextSibling];
	}

	nextId = scrVarGlob.variableList[parentValue->nextSibling].hash.id;

	while ( nextId != parentId )
	{
		id = nextId;
		nextId = scrVarGlob.variableList[scrVarGlob.variableList[id].nextSibling].hash.id;
		FreeValue(id);
	}
}

/*
==============
ClearObject
==============
*/
void ClearObject( unsigned int parentId )
{
	assert((scrVarGlob.variableList[parentId].w.status & VAR_STAT_MASK) != VAR_STAT_FREE);

	AddRefToObject(parentId);
	ClearObjectInternal(parentId);
	RemoveRefToEmptyObject(parentId);
}

/*
==============
Scr_SetThreadNotifyName
==============
*/
void Scr_SetThreadNotifyName( unsigned int startLocalId, unsigned int notifyName )
{
	VariableValueInternal *entryValue = &scrVarGlob.variableList[startLocalId];

	assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert(((entryValue->w.type & VAR_MASK) == VAR_THREAD));

	entryValue->w.type &= ~VAR_MASK;
	entryValue->w.type &= 0xFF;
	entryValue->w.type |= VAR_NOTIFY_THREAD;

	entryValue->w.notifyName |= notifyName << VAR_NAME_BITS;
}

/*
==============
Scr_ClearThread
==============
*/
void Scr_ClearThread( unsigned int parentId )
{
	VariableValueInternal *parentValue;

	assert(parentId);
	parentValue = &scrVarGlob.variableList[parentId];
	assert((parentValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert(((parentValue->w.type & VAR_MASK) >= VAR_THREAD) && ((parentValue->w.type & VAR_MASK) <= VAR_CHILD_THREAD));
	assert(!FindVariable( parentId, OBJECT_STACK ));

	if ( scrVarGlob.variableList[parentValue->nextSibling].hash.id != parentId )
	{
		ClearObjectInternal(parentId);
	}

	RemoveRefToObject(parentValue->u.o.u.self);
}

/*
==============
Scr_StopThread
==============
*/
void Scr_StopThread( unsigned int threadId )
{
	assert(threadId);
	Scr_ClearThread(threadId);

	scrVarGlob.variableList[threadId].u.o.u.self = scrVarPub.levelId;
	AddRefToObject(scrVarPub.levelId);
}

/*
==============
Scr_RemoveThreadNotifyName
==============
*/
void Scr_RemoveThreadNotifyName( unsigned int startLocalId )
{
	unsigned short stringValue;
	VariableValueInternal *entryValue;

	entryValue = &scrVarGlob.variableList[startLocalId];

	assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert((entryValue->w.type & VAR_MASK) == VAR_NOTIFY_THREAD);

	stringValue = Scr_GetThreadNotifyName(startLocalId);
	assert(stringValue);

	SL_RemoveRefToString(stringValue);

	entryValue->w.type &= ~VAR_MASK;
	entryValue->w.type |= VAR_THREAD;
}

/*
==============
Scr_RemoveThreadEmptyNotifyName
==============
*/
void Scr_RemoveThreadEmptyNotifyName( unsigned int startLocalId )
{
	VariableValueInternal *entryValue;

	entryValue = &scrVarGlob.variableList[startLocalId];
	entryValue->w.status &= ~VAR_MASK;
	entryValue->w.status |= VAR_THREAD;
}

/*
==============
Scr_GetThreadNotifyName
==============
*/
unsigned short Scr_GetThreadNotifyName( unsigned int startLocalId )
{
	return scrVarGlob.variableList[startLocalId].w.notifyName >> VAR_NAME_BITS;
}

/*
==============
Scr_SetThreadWaitTime
==============
*/
void Scr_SetThreadWaitTime( unsigned int startLocalId, unsigned int waitTime )
{
	VariableValueInternal *entryValue = &scrVarGlob.variableList[startLocalId];

	assert(((entryValue->w.status & VAR_STAT_MASK) == VAR_STAT_EXTERNAL));
	assert(((entryValue->w.type & VAR_MASK) == VAR_THREAD) || !Scr_GetThreadNotifyName(startLocalId));

	entryValue->w.type &= ~VAR_MASK;
	entryValue->w.type &= 0xFF;
	entryValue->w.type |= VAR_TIME_THREAD;

	scrVarGlob.variableList[startLocalId].w.waitTime |= waitTime << VAR_NAME_BITS;
}

/*
==============
Scr_ClearWaitTime
==============
*/
void Scr_ClearWaitTime( unsigned int startLocalId )
{
	VariableValueInternal *entryValue = &scrVarGlob.variableList[startLocalId];

	assert(((entryValue->w.status & VAR_STAT_MASK) == VAR_STAT_EXTERNAL));
	assert((entryValue->w.type & VAR_MASK) == VAR_TIME_THREAD);

	entryValue->w.type &= ~VAR_MASK;
	entryValue->w.type |= VAR_THREAD;
}

/*
==============
Scr_GetThreadWaitTime
==============
*/
unsigned int Scr_GetThreadWaitTime( unsigned int startLocalId )
{
	return scrVarGlob.variableList[startLocalId].w.waitTime >> VAR_NAME_BITS;
}

/*
==============
GetParentLocalId
==============
*/
unsigned int GetParentLocalId( unsigned int threadId )
{
	return scrVarGlob.variableList[threadId].w.parentLocalId >> VAR_NAME_BITS;
}

/*
==============
GetSafeParentLocalId
==============
*/
unsigned int GetSafeParentLocalId( unsigned int threadId )
{
	if ( ( scrVarGlob.variableList[threadId].w.type & VAR_MASK ) == VAR_CHILD_THREAD )
	{
		return scrVarGlob.variableList[threadId].w.parentLocalId >> VAR_NAME_BITS;
	}

	return 0;
}

/*
==============
GetStartLocalId
==============
*/
unsigned int GetStartLocalId( unsigned int threadId )
{
	assert((scrVarGlob.variableList[threadId].w.status & VAR_STAT_MASK) == VAR_STAT_EXTERNAL);
	assert((scrVarGlob.variableList[threadId].w.type & VAR_MASK) >= VAR_THREAD && (scrVarGlob.variableList[threadId].w.type & VAR_MASK) <= VAR_CHILD_THREAD);

	while ( (scrVarGlob.variableList[threadId].w.type & VAR_MASK) == VAR_CHILD_THREAD )
	{
		threadId = scrVarGlob.variableList[threadId].w.parentLocalId >> VAR_NAME_BITS;
	}

	assert((scrVarGlob.variableList[threadId].w.status & VAR_STAT_MASK) == VAR_STAT_EXTERNAL);
	assert((scrVarGlob.variableList[threadId].w.type & VAR_MASK) >= VAR_THREAD && (scrVarGlob.variableList[threadId].w.type & VAR_MASK) <= VAR_TIME_THREAD);

	return threadId;
}

/*
==============
Scr_KillThread
==============
*/
__declspec(noinline) void RemoveVariableNI( unsigned int parentId, unsigned int name )
{
	RemoveVariable(parentId, name);
}

void Scr_KillThread( unsigned int parentId )
{
	VariableValueInternal *parentValue;
	unsigned int name;
	unsigned int id;
	unsigned int selfNameId;
	unsigned int notifyListEntry;
	unsigned int nameIndex;

	assert(parentId);
	parentValue = &scrVarGlob.variableList[parentId];

	assert((parentValue->w.status & VAR_STAT_MASK) == VAR_STAT_EXTERNAL);
	assert(((parentValue->w.type & VAR_MASK) >= VAR_THREAD) && ((parentValue->w.type & VAR_MASK) <= VAR_CHILD_THREAD));
	Scr_ClearThread(parentId);

	id = FindObjectVariable(scrVarPub.pauseArrayId, parentId);

	if ( id )
	{
		for ( selfNameId = FindObject(id); ; RemoveObjectVariable(selfNameId, nameIndex) )
		{
			notifyListEntry = FindNextSibling(selfNameId);

			if ( !notifyListEntry )
			{
				break;
			}

			name = scrVarGlob.variableList[notifyListEntry].w.name >> VAR_NAME_BITS;
			nameIndex = (unsigned short)name;
			//assert((name - SL_MAX_STRING_INDEX) < (1 << 16));

			VM_CancelNotify(GetVariableValueAddress(FindObjectVariable(selfNameId, nameIndex))->pointerValue, nameIndex);
			Scr_KillEndonThread(nameIndex);
		}

		assert(!GetArraySize(selfNameId));
		RemoveVariableNI(scrVarPub.pauseArrayId, parentId + SL_MAX_STRING_INDEX);
	}

	parentValue->w.type &= ~VAR_MASK;
	parentValue->w.type |= VAR_DEAD_THREAD;
}

/*
==============
Scr_KillEndonThread
==============
*/
void Scr_KillEndonThread( unsigned int threadId )
{
	VariableValueInternal *parentValue;

	parentValue = &scrVarGlob.variableList[threadId];
	assert((parentValue->w.status & VAR_STAT_MASK) == VAR_STAT_EXTERNAL);
	assert((parentValue->w.type & VAR_MASK) == VAR_THREAD);
	//assert(!parentValue->nextSibling);

	RemoveRefToObject(parentValue->u.o.u.self);
	assert(!FindObjectVariable( scrVarPub.pauseArrayId, threadId ));

	parentValue->w.type &= ~VAR_MASK;
	parentValue->w.type |= VAR_DEAD_THREAD;
}

/*
==============
AllocVariable
==============
*/
unsigned short AllocVariable()
{
	unsigned short index;
	VariableValueInternal *entry;
	VariableValueInternal *entryValue;
	unsigned short next;
	unsigned short newIndex;

	index = scrVarGlob.variableList[0].u.next;

	if ( !index )
	{
		Scr_TerminalError("exceeded maximum number of script variables");
	}

	entry = &scrVarGlob.variableList[index];
	entryValue = &scrVarGlob.variableList[entry->hash.id];

	next = entryValue->u.next;

	if ( entry != entryValue && !(entry->w.status & VAR_STAT_MASK) )
	{
		newIndex = entry->v.next;

		scrVarGlob.variableList[newIndex].hash.id = entry->hash.id;
		entry->hash.id = index;

		entryValue->v.index = newIndex;
		entryValue->u.next = entry->u.next;

		entryValue = entry;
	}

	scrVarGlob.variableList[0].u.next = next;
	scrVarGlob.variableList[next].hash.u.prev = 0;

	entryValue->v.index = index;

	entryValue->nextSibling = index;
	entry->hash.u.prevSibling = index;

	return entry->hash.id;
}

/*
==============
FreeVariable
==============
*/
void FreeVariable( unsigned int id )
{
	VariableValueInternal *entry, *entryValue;
	unsigned short prevSibling, nextSibling, index;

	entryValue = &scrVarGlob.variableList[id];

	index = entryValue->v.index;
	entry = &scrVarGlob.variableList[index];

	prevSibling = entry->hash.u.prev;
	nextSibling = entryValue->nextSibling;

	scrVarGlob.variableList[nextSibling].hash.u.prev = prevSibling;
	scrVarGlob.variableList[scrVarGlob.variableList[prevSibling].hash.id].nextSibling = nextSibling;

	entryValue->w.type = VAR_UNDEFINED;
	entryValue->u.next = scrVarGlob.variableList[0].u.next;

	entry->hash.u.prev = 0;

	scrVarGlob.variableList[scrVarGlob.variableList[0].u.next].hash.u.prev = index;

	scrVarGlob.variableList[0].u.next = index;
}

/*
==============
AllocValue
==============
*/
unsigned int AllocValue()
{
	unsigned int id;
	VariableValueInternal *entryValue;

	id = AllocVariable();
	entryValue = &scrVarGlob.variableList[id];

	entryValue->w.status = VAR_STAT_EXTERNAL;
	entryValue->w.type = entryValue->w.type;

	return id;
}

/*
==============
AllocObject
==============
*/
unsigned int AllocObject()
{
	unsigned int id;
	VariableValueInternal *entryValue;

	id = AllocVariable();
	entryValue = &scrVarGlob.variableList[id];
	entryValue->w.status = VAR_STAT_EXTERNAL;
	entryValue->w.type |= VAR_OBJECT;
	entryValue->u.o.refCount = 0;
	return id;
}

/*
==============
AllocEntity
==============
*/
unsigned int AllocEntity( int classnum, unsigned short entnum )
{
	unsigned int id;
	VariableValueInternal *entryValue;

	id = AllocVariable();
	entryValue = &scrVarGlob.variableList[id];
	entryValue->w.status = VAR_STAT_EXTERNAL;
	entryValue->w.type |= VAR_ENTITY;
	entryValue->w.classnum = entryValue->w.classnum | ( classnum << VAR_NAME_BITS );
	entryValue->u.o.refCount = 0;
	entryValue->u.o.u.entnum = entnum;
	return id;
}

/*
==============
Scr_AllocArray
==============
*/
unsigned int Scr_AllocArray()
{
	unsigned int id;
	VariableValueInternal *entryValue;

	id = AllocVariable();
	entryValue = &scrVarGlob.variableList[id];
	entryValue->w.status = VAR_STAT_EXTERNAL;
	entryValue->w.type |= VAR_ARRAY;
	entryValue->u.o.refCount = 0;
	entryValue->u.o.u.size = 0;
	return id;
}

/*
==============
AllocThread
==============
*/
unsigned int AllocThread( unsigned int self )
{
	unsigned int id;
	VariableValueInternal *entryValue;

	id = AllocVariable();
	entryValue = &scrVarGlob.variableList[id];
	entryValue->w.status = VAR_STAT_EXTERNAL;
	entryValue->w.type |= VAR_THREAD;
	entryValue->u.o.refCount = 0;
	entryValue->u.o.u.self = self;
	return id;
}

/*
==============
AllocChildThread
==============
*/
unsigned int AllocChildThread( unsigned int self, unsigned int parentLocalId )
{
	unsigned int id;
	VariableValueInternal *entryValue;

	id = AllocVariable();

	entryValue = &scrVarGlob.variableList[id];
	entryValue->w.status = VAR_STAT_EXTERNAL;

	assert(!(entryValue->w.type & VAR_MASK));
	entryValue->w.type |= VAR_CHILD_THREAD;

	assert(!(entryValue->w.parentLocalId & VAR_NAME_HIGH_MASK));
	entryValue->w.parentLocalId |= parentLocalId << VAR_NAME_BITS;

	entryValue->u.o.refCount = 0;
	entryValue->u.o.u.self = self;

	return id;
}

/*
==============
Scr_GetSelf
==============
*/
unsigned int Scr_GetSelf( unsigned int threadId )
{
	return scrVarGlob.variableList[threadId].u.o.u.self;
}

/*
==============
FreeValue
==============
*/
void FreeValue( unsigned int id )
{
	VariableValueInternal *entryValue = &scrVarGlob.variableList[id];

	assert(((entryValue->w.status & VAR_STAT_MASK) == VAR_STAT_EXTERNAL));
	assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert(!IsObject( entryValue ));
	assert(scrVarGlob.variableList[entryValue->v.index].hash.id == id);

	RemoveRefToValue(entryValue->w.type & VAR_MASK, entryValue->u.u);
	FreeVariable(id);
}

/*
==============
AddRefToObject
==============
*/
void AddRefToObject( unsigned int id )
{
	++scrVarGlob.variableList[id].u.o.refCount;
}

/*
==============
RemoveRefToObject
==============
*/
void RemoveRefToObject( unsigned int id )
{
	VariableValueInternal *entryValue;
	unsigned short entArrayId;
	unsigned int classnum;

	entryValue = &scrVarGlob.variableList[id];

	if ( entryValue->u.o.refCount )
	{
		entryValue->u.o.refCount--;

		if ( !entryValue->u.o.refCount )
		{
			if ( (entryValue->w.type & VAR_MASK) == VAR_ENTITY )
			{
				if ( scrVarGlob.variableList[entryValue->nextSibling].hash.id == id )
				{
					entryValue->w.type &= ~VAR_MASK;
					entryValue->w.type |= VAR_DEAD_ENTITY;

					classnum = entryValue->w.classnum >> VAR_NAME_BITS;
					entArrayId = scrClassMap[classnum].entArrayId;

					RemoveArrayVariable(entArrayId, entryValue->u.o.u.entnum);
				}
			}
		}
	}
	else
	{
		if ( scrVarGlob.variableList[entryValue->nextSibling].hash.id != id )
		{
			ClearObject(id);
		}

		FreeVariable(id);
	}
}

/*
==============
RemoveRefToEmptyObject
==============
*/
void RemoveRefToEmptyObject( unsigned int id )
{
	VariableValueInternal *entryValue = &scrVarGlob.variableList[id];

	if ( entryValue->u.o.refCount )
	{
		entryValue->u.o.refCount--;
	}
	else
	{
		FreeVariable(id);
	}
}

/*
==============
Scr_GetRefCountToObject
==============
*/
int Scr_GetRefCountToObject( unsigned int id )
{
	VariableValueInternal *entryValue = &scrVarGlob.variableList[id];

	assert(((entryValue->w.status & VAR_STAT_MASK) == VAR_STAT_EXTERNAL));
	assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert(IsObject( entryValue ));

	return entryValue->u.o.refCount;
}

/*
==============
Scr_AllocVectorInternal
==============
*/
float *Scr_AllocVectorInternal()
{
	float *vec;

	vec = (float *)MT_Alloc( sizeof( RefVector ), 2 ) + 1;
	( (RefVector *)( vec - 1 ) )->head = 0;
	return vec;
}

/*
==============
Scr_AllocVector
==============
*/
float *Scr_AllocVector( const float *v )
{
	float *result;

	result = Scr_AllocVectorInternal();
	result[0] = v[0];
	result[1] = v[1];
	result[2] = v[2];
	return result;
}

/*
==============
AddRefToVector
==============
*/
void AddRefToVector( const float *vectorValue )
{
	// byteLen is the high byte of the header, refCount its low half
	if ( ( (const unsigned char *)vectorValue )[-1] )
	{
		return;
	}

	( (unsigned short *)vectorValue )[-2]++;
}

/*
==============
RemoveRefToVector
==============
*/
void RemoveRefToVector( const vec3_t vectorValue )
{
	if ( *((byte *)vectorValue - 1) )
	{
		return;
	}

	if ( *((unsigned short *)((byte *)vectorValue - 4)) )
	{
		(*((unsigned short *)((byte *)vectorValue - 4)))--;
		return;
	}

	MT_Free( (void *)((byte *)vectorValue - 4), 16 );
}

/*
==============
AddRefToValue
==============
*/
void AddRefToValue( int type, VariableUnion u )
{
	switch ( type )
	{
	case VAR_POINTER:
		AddRefToObject(u.pointerValue);
		break;

	case VAR_STRING:
	case VAR_ISTRING:
		SL_AddRefToString(u.stringValue);
		break;

	case VAR_VECTOR:
		assert(type - 1 == VAR_VECTOR - VAR_BEGIN_REF);
		AddRefToVector(u.vectorValue);
		break;
	}
}

/*
==============
RemoveRefToValue
==============
*/
void RemoveRefToValue( int type, VariableUnion u )
{
	switch ( type )
	{
	case VAR_POINTER:
		RemoveRefToObject(u.pointerValue);
		break;

	case VAR_STRING:
	case VAR_ISTRING:
		SL_RemoveRefToString(u.stringValue);
		break;

	case VAR_VECTOR:
		assert(type - 1 == VAR_VECTOR - VAR_BEGIN_REF);
		RemoveRefToVector(u.vectorValue);
		break;
	}
}

/*
==============
IsValidArrayIndex
==============
*/
bool IsValidArrayIndex( unsigned int index )
{
	return index + 0x7E0002 <= 0xFE0001;
}

/*
==============
GetInternalVariableIndex
==============
*/
unsigned int GetInternalVariableIndex( unsigned int index )
{
	assert(IsValidArrayIndex( index ));
	return ( index + MAX_ARRAYINDEX ) & VAR_NAME_LOW_MASK;
}

/*
==============
FindArrayVariableIndex
==============
*/
unsigned int FindArrayVariableIndex( unsigned int parentId, unsigned int index )
{
	assert(IsValidArrayIndex( index ));
	return FindVariableIndexHash( parentId, ( index + MAX_ARRAYINDEX ) & VAR_NAME_LOW_MASK );
}

/*
==============
FindArrayVariable
==============
*/
unsigned int FindArrayVariable( unsigned int parentId, unsigned int index )
{
	return scrVarGlob.variableList[ FindArrayVariableIndex( parentId, index ) ].hash.id;
}

/*
==============
FindVariable
==============
*/
unsigned int FindVariable( unsigned int parentId, unsigned int index )
{
	return scrVarGlob.variableList[FindVariableIndexHash(parentId, index)].hash.id;
}

/*
==============
FindObjectVariable
==============
*/
unsigned int FindObjectVariable( unsigned int parentId, unsigned int id )
{
	return scrVarGlob.variableList[FindVariableIndexHash(parentId, id + SL_MAX_STRING_INDEX)].hash.id;
}

/*
==============
GetArrayVariableIndex
==============
*/
unsigned int GetArrayVariableIndex( unsigned int parentId, unsigned int index )
{
	assert(IsValidArrayIndex( index ));
	return GetVariableIndexInternal( parentId, ( index + MAX_ARRAYINDEX ) & VAR_NAME_LOW_MASK );
}

/*
==============
GetNewArrayVariableIndex
==============
*/
unsigned int GetNewArrayVariableIndex( unsigned int parentId, unsigned int index )
{
	assert(IsValidArrayIndex( index ));
	return GetNewVariableIndexInternal( parentId, ( index + MAX_ARRAYINDEX ) & VAR_NAME_LOW_MASK );
}

/*
==============
GetVariable
==============
*/
unsigned int Scr_GetVariableField( unsigned int parentId, unsigned int name )
{
	unsigned int index;
	VariableValueInternal *entryValue;
	int type;

	assert(parentId);
	entryValue = &scrVarGlob.variableList[parentId];

	assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert(IsObject(entryValue));

	type = entryValue->w.type & VAR_MASK;

	if ( type <= VAR_OBJECT )
	{
		return GetVariable(parentId, name);
	}

	if ( type == VAR_ENTITY )
	{
		index = FindVariable(parentId, name);

		if ( index )
		{
			return index;
		}

		scrVarPub.entId = parentId;
		scrVarPub.entFieldName = name;

		return VARIABLELIST_CHILD_SIZE;
	}

	Scr_Error(va("cannot set field of %s", var_typename[type]));
	return 0;
}

/*
==============
Scr_FindVariableField
==============
*/
VariableValue Scr_FindVariableField( unsigned int parentId, unsigned int name )
{
	unsigned int id;
	VariableValueInternal *entryValue;
	VariableValue value;

	assert(parentId);

	id = FindVariable(parentId, name);

	if ( id )
	{
		return Scr_EvalVariable(id);
	}

	entryValue = &scrVarGlob.variableList[parentId];

	if ( (entryValue->w.type & VAR_MASK) != VAR_ENTITY )
	{
		value.type = VAR_UNDEFINED;
		return value;
	}

	return Scr_EvalVariableEntityField(parentId, name);
}

/*
==============
Scr_FindAllVariableField
==============
*/
unsigned int Scr_FindAllVariableField( unsigned int parentId, unsigned int *names )
{
	VariableValueInternal *parentValue;
	VariableValueInternal *entryValue;
	unsigned int id;
	unsigned int count;
	unsigned int name;
	unsigned int classIndex;
	VariableValueInternal *classValue;
	unsigned int classnum;

	parentValue = &scrVarGlob.variableList[parentId];
	count = 0;

	switch ( parentValue->w.type & VAR_MASK )
	{
	case VAR_ENTITY:
		classIndex = parentValue->w.name >> VAR_NAME_BITS;
		classnum = scrClassMap[classIndex].id;
		classValue = &scrVarGlob.variableList[classnum];
		for ( id = scrVarGlob.variableList[classValue->nextSibling].hash.id; id != classnum; id = scrVarGlob.variableList[entryValue->nextSibling].hash.id )
		{
			entryValue = &scrVarGlob.variableList[id];
			name = ( entryValue->w.name >> VAR_NAME_BITS ) - MAX_ARRAYINDEX;
			if ( name <= scrVarPub.canonicalStrCount && !FindVariable(parentId, name) )
			{
				if ( names )
				{
					names[count] = name;
				}
				count++;
			}
		}
		// fall through
	case VAR_THREAD:
	case VAR_NOTIFY_THREAD:
	case VAR_TIME_THREAD:
	case VAR_CHILD_THREAD:
	case VAR_OBJECT:
	case VAR_DEAD_ENTITY:
		for ( id = scrVarGlob.variableList[parentValue->nextSibling].hash.id; id != parentId; id = scrVarGlob.variableList[entryValue->nextSibling].hash.id )
		{
			entryValue = &scrVarGlob.variableList[id];
			name = entryValue->w.name >> VAR_NAME_BITS;
			if ( name == OBJECT_NOTIFY_LIST || name == OBJECT_STACK )
			{
				continue;
			}
			if ( names )
			{
				names[count] = name;
			}
			count++;
		}
		break;

	case VAR_ARRAY:
		for ( id = scrVarGlob.variableList[parentValue->nextSibling].hash.id; id != parentId; id = scrVarGlob.variableList[entryValue->nextSibling].hash.id )
		{
			entryValue = &scrVarGlob.variableList[id];
			name = entryValue->w.name >> VAR_NAME_BITS;
			if ( names )
			{
				names[count] = name;
			}
			count++;
		}
		break;
	}

	return count;
}

/*
==============
SetNewVariableValue
==============
*/
VariableValue Scr_GetArrayIndexValue( unsigned int name )
{
	VariableValue value;

	assert(name);

	if ( name < SL_MAX_STRING_INDEX )
	{
		value.type = VAR_STRING;
		value.u.stringValue = (unsigned short)name;
	}
	else if ( name < OBJECT_NOTIFY_LIST )
	{
		value.type = VAR_POINTER;
		value.u.pointerValue = name - SL_MAX_STRING_INDEX;
	}
	else
	{
		value.type = VAR_INTEGER;
		value.u.intValue = name - MAX_ARRAYINDEX;
	}

	return value;
}

/*
==============
ClearVariableField
==============
*/
void ClearVariableField( unsigned int parentId, unsigned int name, VariableValue *value )
{
	unsigned int index;
	unsigned int fieldId;
	VariableValueInternal *parentValue;
	unsigned int classnum;
	assert((scrVarGlob.variableList[parentId].w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert(IsObject(&scrVarGlob.variableList[parentId]));
	assert(((scrVarGlob.variableList[parentId].w.type & VAR_MASK) >= FIRST_OBJECT && (scrVarGlob.variableList[parentId].w.type & VAR_MASK) < FIRST_NONFIELD_OBJECT) || ((scrVarGlob.variableList[parentId].w.type & VAR_MASK) >= FIRST_DEAD_OBJECT));

	index = FindVariableIndexHash(parentId, name);

	if ( index )
	{
		RemoveVariable(parentId, name);
		return;
	}

	parentValue = &scrVarGlob.variableList[parentId];
	assert((parentValue->w.classnum >> VAR_NAME_BITS) < CLASS_NUM_COUNT);

	if ( (parentValue->w.type & VAR_MASK) != VAR_ENTITY )
	{
		return;
	}

	classnum = parentValue->w.classnum >> VAR_NAME_BITS;
	fieldId = FindArrayVariable(scrClassMap[classnum].id, name);

	if ( !fieldId )
	{
		return;
	}

	value += 1;
	value->type = VAR_UNDEFINED;

	SetEntityFieldValue( classnum, parentValue->u.o.u.entnum, scrVarGlob.variableList[fieldId].u.u.entityOffset, value );
}

/*
==============
GetArrayVariable
==============
*/
unsigned int GetArrayVariable( unsigned int parentId, unsigned int name )
{
	return scrVarGlob.variableList[ GetArrayVariableIndex( parentId, name ) ].hash.id;
}

/*
==============
GetNewArrayVariable
==============
*/
unsigned int GetNewArrayVariable( unsigned int parentId, unsigned int name )
{
	return scrVarGlob.variableList[ GetNewArrayVariableIndex( parentId, name ) ].hash.id;
}

/*
==============
GetVariable
==============
*/
unsigned int GetVariable( unsigned int parentId, unsigned int name )
{
	return scrVarGlob.variableList[ GetVariableIndexInternal( parentId, name ) ].hash.id;
}

/*
==============
GetNewVariable
==============
*/
unsigned int GetNewVariable( unsigned int parentId, unsigned int name )
{
	return scrVarGlob.variableList[ GetNewVariableIndexInternal( parentId, name ) ].hash.id;
}

/*
==============
GetObjectVariable
==============
*/
unsigned int GetObjectVariable( unsigned int parentId, unsigned int id )
{
	assert((scrVarGlob.variableList[parentId].w.type & VAR_MASK) == VAR_ARRAY);
	return scrVarGlob.variableList[ GetVariableIndexInternal( parentId, id + SL_MAX_STRING_INDEX ) ].hash.id;
}

/*
==============
GetNewObjectVariable
==============
*/
unsigned int GetNewObjectVariable( unsigned int parentId, unsigned int id )
{
	assert((scrVarGlob.variableList[parentId].w.type & VAR_MASK) == VAR_ARRAY);
	return scrVarGlob.variableList[ GetNewVariableIndexInternal( parentId, id + SL_MAX_STRING_INDEX ) ].hash.id;
}

/*
==============
GetNewObjectVariableReverse
==============
*/
unsigned int GetNewObjectVariableReverse( unsigned int parentId, unsigned int id )
{
	assert((scrVarGlob.variableList[parentId].w.type & VAR_MASK) == VAR_ARRAY);
	return scrVarGlob.variableList[ GetNewVariableIndexReverseInternal( parentId, id + SL_MAX_STRING_INDEX ) ].hash.id;
}

/*
==============
RemoveVariable
==============
*/
void RemoveVariable( unsigned int parentId, unsigned int name )
{
	VariableValueInternal *entryValue;
	unsigned int index;
	unsigned int id;

	assert((scrVarGlob.variableList[parentId].w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	index = FindVariableIndexHash(parentId, name);
	entryValue = &scrVarGlob.variableList[index];

	id = entryValue->hash.id;
	assert(id);

	MakeVariableExternal(entryValue, &scrVarGlob.variableList[parentId]);
	FreeValue(id);
}

/*
==============
RemoveNextVariable
==============
*/
void RemoveNextVariable( unsigned int parentId )
{
	VariableValueInternal *entryValue;
	unsigned short nextSibling;
	unsigned short id;

	assert((scrVarGlob.variableList[parentId].w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	nextSibling = scrVarGlob.variableList[parentId].nextSibling;
	entryValue = &scrVarGlob.variableList[nextSibling];

	id = entryValue->hash.id;
	assert(id);

	MakeVariableExternal(entryValue, &scrVarGlob.variableList[parentId]);
	FreeValue(id);
}

/*
==============
RemoveObjectVariable
==============
*/
void RemoveObjectVariable( unsigned int parentId, unsigned int id )
{
	assert((scrVarGlob.variableList[parentId].w.type & VAR_MASK) == VAR_ARRAY);
	RemoveVariable(parentId, id + SL_MAX_STRING_INDEX);
}

/*
==============
SafeRemoveArrayVariable
==============
*/
void SafeRemoveArrayVariable( unsigned int parentId, unsigned int index )
{
	assert(IsValidArrayIndex( index ));
	SafeRemoveVariable( parentId, ( index + MAX_ARRAYINDEX ) & VAR_NAME_LOW_MASK );
}

/*
==============
RemoveArrayVariable
==============
*/
void RemoveArrayVariable( unsigned int parentId, unsigned int index )
{
	assert(IsValidArrayIndex( index ));
	RemoveVariable( parentId, ( index + MAX_ARRAYINDEX ) & VAR_NAME_LOW_MASK );
}

/*
==============
SafeRemoveVariable
==============
*/
void SafeRemoveVariable( unsigned int parentId, unsigned int name )
{
	VariableValueInternal *entryValue;
	unsigned int id;
	unsigned int index;

	index = FindVariableIndexHash( parentId, name );

	if ( !index )
	{
		return;
	}

	entryValue = &scrVarGlob.variableList[index];
	id = entryValue->hash.id;

	MakeVariableExternal( entryValue, &scrVarGlob.variableList[parentId] );
	FreeValue( id );
}

/*
==============
CopyArray
==============
*/
void CopyArray( unsigned int parentId, unsigned int newParentId )
{
	VariableValueInternal *parentValue;
	unsigned short *newValue;
	int type;
	VariableValueInternal *entryValue;
	VariableValueInternal *newEntryValue;
	VariableValueInternal *nextValue;
	unsigned int id;

	parentValue = &scrVarGlob.variableList[parentId];

	nextValue = &scrVarGlob.variableList[ scrVarGlob.variableList[parentId].nextSibling ];
	id = nextValue->hash.id;

	for ( ; ; )
	{
		if ( id == parentId )
		{
			break;
		}

		entryValue = &scrVarGlob.variableList[id];
		type = entryValue->w.type & VAR_MASK;

		newValue = &scrVarGlob.variableList[ GetVariableIndexInternal( newParentId, entryValue->w.name >> VAR_NAME_BITS ) ].hash.id;
		newEntryValue = &scrVarGlob.variableList[ *newValue ];

		newEntryValue->w.type |= type;

		if ( type != VAR_POINTER )
		{
			newEntryValue->u.u = entryValue->u.u;
			AddRefToValue(type, entryValue->u.u);
		}
		else if ( ( scrVarGlob.variableList[entryValue->u.u.pointerValue].w.type & VAR_MASK ) == VAR_ARRAY )
		{
			newEntryValue->u.u.pointerValue = Scr_AllocArray();
			CopyArray(entryValue->u.u.pointerValue, newEntryValue->u.u.pointerValue);
		}
		else
		{
			newEntryValue->u.u.pointerValue = entryValue->u.u.pointerValue;
			AddRefToObject(entryValue->u.u.pointerValue);
		}

		nextValue = &scrVarGlob.variableList[ scrVarGlob.variableList[id].nextSibling ];
		id = nextValue->hash.id;
	}
}

/*
==============
SetVariableValue
==============
*/
void SetVariableValue( unsigned int id, VariableValue *value )
{
	VariableValueInternal *entryValue;

	assert(id);
	assert(!IsObjectVal( value ));
	assert(value->type >= 0 && value->type < VAR_COUNT);
	assert(value->type != VAR_STACK);

	entryValue = &scrVarGlob.variableList[id];

	assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert(!IsObject( entryValue ));
	assert((entryValue->w.type & VAR_MASK) != VAR_STACK);

	RemoveRefToValue(entryValue->w.type & VAR_MASK, entryValue->u.u);

	entryValue->w.type &= ~VAR_MASK;
	entryValue->w.type |= value->type;
	entryValue->u.u = value->u;
}

/*
==============
SetNewVariableValue
==============
*/
void SetNewVariableValue( unsigned int id, VariableValue *value )
{
	VariableValueInternal *entryValue;

	assert((value->type & VAR_MASK) < VAR_THREAD);
	entryValue = &scrVarGlob.variableList[id];

	assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert(!IsObject( entryValue ));
	assert(value->type >= 0 && value->type < VAR_COUNT);
	assert((entryValue->w.type & VAR_MASK) == VAR_UNDEFINED);
	assert((value->type != VAR_POINTER) || ((entryValue->w.type & VAR_MASK) < FIRST_DEAD_OBJECT));
	assert(!(entryValue->w.type & VAR_MASK));

	entryValue->w.type |= value->type;
	entryValue->u.u = value->u;
}

/*
==============
GetVariableValueAddress
==============
*/
VariableUnion *GetVariableValueAddress( unsigned int id )
{
	VariableValueInternal *entryValue;

	assert(id);
	entryValue = &scrVarGlob.variableList[id];

	assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert((entryValue->w.type & VAR_MASK) != VAR_UNDEFINED);
	assert(!IsObject( entryValue ));

	return &entryValue->u.u;
}

/*
==============
SetVariableEntityFieldValue
==============
*/
void SetVariableEntityFieldValue( unsigned int entId, unsigned int fieldName, VariableValue *value )
{
	VariableValueInternal *entryValue;
	VariableValueInternal *entValue;
	unsigned int fieldId;

	assert(!IsObjectVal( value ));
	assert(value->type != VAR_STACK);

	entValue = &scrVarGlob.variableList[entId];
	assert((entValue->w.type & VAR_MASK) == VAR_ENTITY);
	assert((entValue->w.classnum >> VAR_NAME_BITS) < CLASS_NUM_COUNT);

	fieldId = FindArrayVariable(scrClassMap[entValue->w.name >> VAR_NAME_BITS].id, fieldName);

	if ( fieldId )
	{
		if ( SetEntityFieldValue( entValue->w.classnum >> VAR_NAME_BITS, entValue->u.o.u.entnum, scrVarGlob.variableList[fieldId].u.u.entityOffset, value ) )
		{
			return;
		}
	}

	entryValue = &scrVarGlob.variableList[GetNewVariable(entId, fieldName)];
	assert(!(entryValue->w.type & VAR_MASK));

	entryValue->w.type |= value->type;
	entryValue->u.u = value->u;
}

/*
==============
ClearVariableValue
==============
*/
__declspec(noinline) void ClearVariableValue( unsigned int id )
{
	VariableValueInternal *entryValue;

	assert(id);
	entryValue = &scrVarGlob.variableList[id];

	assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert(!IsObject( entryValue ));
	assert((entryValue->w.type & VAR_MASK) != VAR_STACK);

	RemoveRefToValue(entryValue->w.type & VAR_MASK, entryValue->u.u);

	entryValue->w.type &= ~VAR_MASK;
	assert((entryValue->w.type & VAR_MASK) == VAR_UNDEFINED);
}

/*
==============
SetVariableFieldValue
==============
*/
void SetVariableFieldValue( unsigned int id, VariableValue *value )
{
	if ( id != VARIABLELIST_CHILD_SIZE )
	{
		SetVariableValue( id, value );
		return;
	}

	SetVariableEntityFieldValue( scrVarPub.entId, scrVarPub.entFieldName, value );
}

/*
==============
Scr_EvalVariable
==============
*/
VariableValue Scr_EvalVariable( unsigned int id )
{
	VariableValueInternal *entryValue;
	VariableValue value;

	entryValue = &scrVarGlob.variableList[id];
	assert(((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE) || !id);

	value.type = entryValue->w.type & VAR_MASK;
	value.u = entryValue->u.u;

	assert(!IsObject(entryValue));
	AddRefToValue(&value);

	return value;
}

/*
==============
Scr_EvalVariableObject
==============
*/
unsigned int Scr_EvalVariableObject( unsigned int id )
{
	VariableValueInternal *entryValue;
	int type;

	entryValue = &scrVarGlob.variableList[id];
	assert(((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE) || !id);

	type = entryValue->w.type & VAR_MASK;

	if ( type == VAR_POINTER )
	{
		type = scrVarGlob.variableList[entryValue->u.u.pointerValue].w.type & VAR_MASK;

		if ( type < VAR_ARRAY )
		{
			assert(type >= FIRST_OBJECT);
			return entryValue->u.u.pointerValue;
		}
	}

	Scr_Error(va("%s is not a field object", var_typename[type]));
	return 0;
}

/*
==============
Scr_EvalVariableEntityField
==============
*/
VariableValue Scr_EvalVariableEntityField( unsigned int entId, unsigned int name )
{
	VariableValueInternal *entryValue, *entValue;
	unsigned int fieldId, id;
	VariableValue value;

	entValue = &scrVarGlob.variableList[entId];
	assert((entValue->w.type & VAR_MASK) == VAR_ENTITY);
	assert((entValue->w.classnum >> VAR_NAME_BITS) < CLASS_NUM_COUNT);

	fieldId = FindArrayVariable(scrClassMap[entValue->w.parentLocalId >> VAR_NAME_BITS].id, name);

	if ( fieldId )
	{
		value = GetEntityFieldValue(entValue->w.classnum >> VAR_NAME_BITS, entValue->u.o.u.entnum, scrVarGlob.variableList[fieldId].u.u.entityOffset);

		if ( value.type != VAR_POINTER )
		{
			return value;
		}

		entryValue = &scrVarGlob.variableList[value.u.pointerValue];

		if ( (entryValue->w.type & VAR_MASK) != VAR_ARRAY )
		{
			return value;
		}

		if ( entryValue->u.o.refCount )
		{
			id = value.u.pointerValue;
			RemoveRefToObject(id);
			value.u.pointerValue = Scr_AllocArray();
			CopyArray(id, value.u.pointerValue);
		}

		return value;
	}

	value.type = VAR_UNDEFINED;
	return value;
}

/*
==============
Scr_EvalVariableFieldInternal
==============
*/
VariableValue Scr_EvalVariableFieldInternal( unsigned int id )
{
	return id != VARIABLELIST_CHILD_SIZE
	       ? Scr_EvalVariable(id)
	       : Scr_EvalVariableEntityField(scrVarPub.entId, scrVarPub.entFieldName);
}

/*
==============
Scr_EvalSizeValue
==============
*/
void Scr_EvalSizeValue( VariableValue *value )
{
	VariableValueInternal *entryValue;
	unsigned int id;
	unsigned int stringValue;
	const char *error_message;

	if ( value->type == VAR_POINTER )
	{
		id = value->u.pointerValue;
		entryValue = &scrVarGlob.variableList[id];
		value->type = VAR_INTEGER;
		value->u.intValue = (entryValue->w.type & VAR_MASK) == VAR_ARRAY ? entryValue->u.o.u.size : 1;
		RemoveRefToObject(id);
	}
	else if ( value->type == VAR_STRING )
	{
		value->type = VAR_INTEGER;
		stringValue = value->u.stringValue;
		value->u.intValue = strlen(SL_ConvertToString(stringValue));
		SL_RemoveRefToString(stringValue);
	}
	else
	{
		error_message = va("size cannot be applied to %s", var_typename[value->type]);
		RemoveRefToValue(value);
		value->type = VAR_UNDEFINED;
		Scr_Error(error_message);
	}
}

/*
==============
GetArraySize
==============
*/
unsigned int GetArraySize( unsigned int id )
{
	VariableValueInternal *entryValue;

	assert(id);
	entryValue = &scrVarGlob.variableList[id];
	assert((entryValue->w.type & VAR_MASK) == VAR_ARRAY);

	return entryValue->u.o.u.size;
}

/*
==============
FindNextSibling
==============
*/
unsigned int FindNextSibling( unsigned int id )
{
	unsigned int childId;

	childId = scrVarGlob.variableList[scrVarGlob.variableList[id].nextSibling].hash.id;

	return !IsObject( ( &scrVarGlob.variableList[childId] ) ) ? childId : 0;
}

/*
==============
FindPrevSibling
==============
*/
unsigned int FindPrevSibling( unsigned int id )
{
	unsigned int childId;

	childId = scrVarGlob.variableList[scrVarGlob.variableList[scrVarGlob.variableList[scrVarGlob.variableList[id].nextSibling].hash.u.prevSibling].hash.u.prevSibling].hash.id;

	return !IsObject( ( &scrVarGlob.variableList[childId] ) ) ? childId : 0;
}

/*
==============
GetVariableName
==============
*/
unsigned int GetVariableName( unsigned int id )
{
	return scrVarGlob.variableList[id].w.name >> VAR_NAME_BITS;
}

/*
==============
GetObjectA
==============
*/
unsigned int GetObjectA( unsigned int id )
{
	VariableValueInternal *entryValue;

	assert(id);
	entryValue = &scrVarGlob.variableList[id];
	assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert((entryValue->w.type & VAR_MASK) == VAR_UNDEFINED || (entryValue->w.type & VAR_MASK) == VAR_POINTER);

	if ( (entryValue->w.type & VAR_MASK) == VAR_UNDEFINED )
	{
		entryValue->w.type |= VAR_POINTER;
		entryValue->u.u.pointerValue = AllocObject();
	}

	assert((entryValue->w.type & VAR_MASK) == VAR_POINTER);
	return entryValue->u.u.pointerValue;
}

/*
==============
GetArray
==============
*/
unsigned int GetArray( unsigned int id )
{
	assert(id);

	VariableValueInternal *entryValue = &scrVarGlob.variableList[id];

	assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert((entryValue->w.type & VAR_MASK) == VAR_UNDEFINED || (entryValue->w.type & VAR_MASK) == VAR_POINTER);

	if ( (entryValue->w.type & VAR_MASK) == VAR_UNDEFINED )
	{
		entryValue->w.type |= VAR_POINTER;
		entryValue->u.u.pointerValue = Scr_AllocArray();
	}

	assert((entryValue->w.type & VAR_MASK) == VAR_POINTER);

	return entryValue->u.u.pointerValue;
}

/*
==============
FindObject
==============
*/
unsigned int FindObject( unsigned int id )
{
	VariableValueInternal *entryValue;

	assert(id);
	entryValue = &scrVarGlob.variableList[id];

	assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert((entryValue->w.type & VAR_MASK) == VAR_POINTER);

	return entryValue->u.u.pointerValue;
}

/*
==============
IsFieldObject
==============
*/
bool IsFieldObject( unsigned int id )
{
	VariableValueInternal *entryValue;

	assert(id);
	entryValue = &scrVarGlob.variableList[id];

	assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert(IsObject( entryValue ));

	return (entryValue->w.type & VAR_MASK) < VAR_ARRAY;
}

/*
==============
Scr_IsThreadAlive
==============
*/
int Scr_IsThreadAlive( unsigned int thread )
{
	VariableValueInternal *entryValue;

	assert(scrVarPub.timeArrayId);
	entryValue = &scrVarGlob.variableList[thread];

	assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert(((entryValue->w.type & VAR_MASK) >= VAR_THREAD && (entryValue->w.type & VAR_MASK) <= VAR_CHILD_THREAD) || (entryValue->w.type & VAR_MASK) == VAR_DEAD_THREAD);

	return (entryValue->w.type & VAR_MASK) != VAR_DEAD_THREAD;
}

/*
==============
Scr_EvalBoolNot
==============
*/
void Scr_EvalBoolNot( VariableValue *value )
{
	Scr_CastBool(value);

	if ( value->type == VAR_INTEGER )
	{
		value->u.intValue = value->u.intValue == 0;
	}
}

/*
==============
Scr_EvalBoolComplement
==============
*/
#pragma inline_depth(0)
void Scr_EvalBoolComplement( VariableValue *value )
{
	int type;

	if ( value->type == VAR_INTEGER )
	{
		value->u.intValue = ~value->u.intValue;
		return;
	}

	type = value->type;
	RemoveRefToValue(value->type, value->u);
	value->type = VAR_UNDEFINED;
	{
		const char *error = va("~ cannot be applied to \"%s\"", var_typename[type]);
		if ( scrVarPub.error_message == NULL )
			scrVarPub.error_message = error;
		Scr_ErrorInternal();
	}
}
#pragma inline_depth()

/*
==============
Scr_CastBool
==============
*/
void Scr_CastBool( VariableValue *value )
{
	int type;

	if ( value->type == VAR_INTEGER )
	{
		value->u.intValue = value->u.intValue != 0;
	}
	else if ( value->type == VAR_FLOAT )
	{
		value->type = VAR_INTEGER;
		value->u.intValue = value->u.floatValue != 0;
	}
	else
	{
		type = value->type;
		RemoveRefToValue(value->type, value->u);
		value->type = VAR_UNDEFINED;
		Scr_Error(va("cannot cast %s to bool", var_typename[type]));
	}
}

/*
==============
Scr_CastString
==============
*/
bool Scr_CastString( VariableValue *value )
{
	const float *constTempVector;

	if ( value->type == VAR_STRING )
	{
		return true;
	}

	if ( value->type == VAR_INTEGER )
	{
		value->type = VAR_STRING;
		value->u.stringValue = SL_GetStringForInt(value->u.intValue);
		return true;
	}

	if ( value->type == VAR_FLOAT )
	{
		value->type = VAR_STRING;
		value->u.stringValue = SL_GetStringForFloat(value->u.floatValue);
		return true;
	}

	if ( value->type == VAR_VECTOR )
	{
		value->type = VAR_STRING;
		constTempVector = value->u.vectorValue;
		value->u.stringValue = SL_GetStringForVector(constTempVector);
		RemoveRefToVector(constTempVector);
		return true;
	}

	scrVarPub.error_message = va("cannot cast %s to string", var_typename[value->type]);

	RemoveRefToValue(value);
	value->type = VAR_UNDEFINED;

	return false;
}

/*
==============
Scr_CastDebugString
==============
*/
void Scr_CastDebugString( VariableValue *value )
{
	// the animation payload is read as two 16-bit halves of the union word
	union
	{
		VariableUnion u;
		struct
		{
			unsigned short animIndex;
			unsigned short animTree;
		} anim;
	} v;
	const char *name;
	unsigned int stringValue;
	unsigned int animValue;

	switch ( value->type )
	{
	case VAR_STRING:
	case VAR_VECTOR:
	case VAR_FLOAT:
	case VAR_INTEGER:
		Scr_CastString(value);
		return;

	case VAR_ISTRING:
		value->type = VAR_STRING;
		return;

	case VAR_ANIMATION:
		animValue = value->u.pointerValue;
		v.u.pointerValue = animValue;
		name = XAnimGetAnimDebugName( Scr_GetAnims( v.anim.animTree ), v.anim.animIndex );
		break;

	case VAR_POINTER:
		name = var_typename[ GetObjectType( value->u.pointerValue ) ];
		break;

	default:
		name = var_typename[ value->type ];
		break;
	}

	stringValue = SL_GetString_( name, 0, 14 );

	RemoveRefToValue(value);

	value->type = VAR_STRING;
	value->u.stringValue = stringValue;
}

/*
==============
Scr_GetEntClassId
==============
*/
char Scr_GetEntClassId( unsigned int id )
{
	assert(GetObjectType( id ) == VAR_ENTITY);
	return scrClassMap[scrVarGlob.variableList[id].w.name >> VAR_NAME_BITS].charId;
}

/*
==============
Scr_GetEntNum
==============
*/
int Scr_GetEntNum( unsigned int id )
{
	assert(GetObjectType( id ) == VAR_ENTITY);
	return scrVarGlob.variableList[id].u.o.u.entnum;
}

/*
==============
Scr_ClearVector
==============
*/
void Scr_ClearVector( VariableValue *value )
{
	for ( int i = 2; i >= 0; i-- )
	{
		RemoveRefToValue( &value[i] );
	}

	value->type = VAR_UNDEFINED;
}

/*
==============
Scr_CastVector
==============
*/
void Scr_CastVector( VariableValue *value )
{
	vec3_t vec;
	int i;
	int type;

	for ( i = 2; i >= 0; i-- )
	{
		type = value[i].type;

		if ( type == VAR_FLOAT )
		{
			vec[2 - i] = value[i].u.floatValue;
		}
		else if ( type == VAR_INTEGER )
		{
			vec[2 - i] = (float)value[i].u.intValue;
		}
		else
		{
			scrVarPub.error_index = i + 1;
			Scr_ClearVector(value);
			Scr_Error(va("type %s is not a float", var_typename[type]));
			return;
		}
	}

	value->type = VAR_VECTOR;
	value->u.vectorValue = Scr_AllocVector(vec);
}

/*
==============
Scr_EvalFieldObject
==============
*/
unsigned int Scr_EvalFieldObject( unsigned int tempVariable, VariableValue *value )
{
	int type;
	VariableValue tempValue;

	type = value->type;

	if ( type == VAR_POINTER )
	{
		type = scrVarGlob.variableList[value->u.pointerValue].w.type & VAR_MASK;

		if ( type < VAR_ARRAY )
		{
			assert(type >= FIRST_OBJECT);

			tempValue.type = VAR_POINTER;
			tempValue.u = value->u;

			SetVariableValue(tempVariable, &tempValue);
			return tempValue.u.pointerValue;
		}
	}

	RemoveRefToValue(value);
	Scr_Error(va("%s is not a field object", var_typename[type]));
	return 0;
}

/*
==============
Scr_UnmatchingTypesError
==============
*/
void Scr_UnmatchingTypesError( VariableValue *value1, VariableValue *value2 )
{
	int type1, type2;
	const char *error_message;

	if ( !scrVarPub.error_message )
	{
		type1 = value1->type;
		type2 = value2->type;

		Scr_CastDebugString(value1);
		Scr_CastDebugString(value2);

		assert(value1->type == VAR_STRING);
		assert(value2->type == VAR_STRING);

		error_message = va("pair '%s' and '%s' has unmatching types '%s' and '%s'",
		                   SL_ConvertToString(value1->u.stringValue),
		                   SL_ConvertToString(value2->u.stringValue),
		                   var_typename[type1],
		                   var_typename[type2]);
	}
	else
	{
		error_message = NULL;
	}

	RemoveRefToValue(value1);
	value1->type = VAR_UNDEFINED;

	RemoveRefToValue(value2);
	value2->type = VAR_UNDEFINED;

	Scr_Error(error_message);
}

/*
==============
Scr_CastWeakerPair
==============
*/
void Scr_CastWeakerPair( VariableValue *value1, VariableValue *value2 )
{
	int type1, type2;

	type1 = value1->type;
	type2 = value2->type;

	if ( type1 == type2 )
	{
		return;
	}

	if ( type1 == VAR_FLOAT && type2 == VAR_INTEGER )
	{
		value2->type = VAR_FLOAT;
		value2->u.floatValue = (float)value2->u.intValue;
		return;
	}

	if ( type1 == VAR_INTEGER && type2 == VAR_FLOAT )
	{
		value1->type = VAR_FLOAT;
		value1->u.floatValue = (float)value1->u.intValue;
		return;
	}

	Scr_UnmatchingTypesError(value1, value2);
}

/*
==============
Scr_CastWeakerStringPair
==============
*/
void Scr_CastWeakerStringPair( VariableValue *value1, VariableValue *value2 )
{
	int type1, type2;
	const float *constTempVector;

	type1 = value1->type;
	type2 = value2->type;

	if ( type1 == type2 )
	{
		return;
	}

	if ( type1 < type2 )
	{
		switch ( type1 )
		{
		case VAR_STRING:
			switch ( type2 )
			{
			case VAR_VECTOR:
				value2->type = VAR_STRING;
				constTempVector = value2->u.vectorValue;
				value2->u.intValue = SL_GetStringForVector(constTempVector);
				RemoveRefToVector(constTempVector);
				return;

			case VAR_FLOAT:
				value2->type = VAR_STRING;
				value2->u.intValue = SL_GetStringForFloat(value2->u.floatValue);
				return;

			case VAR_INTEGER:
				value2->type = VAR_STRING;
				value2->u.intValue = SL_GetStringForInt(value2->u.intValue);
				return;
			}

		case VAR_FLOAT:
			if ( type2 == VAR_INTEGER )
			{
				value2->type = VAR_FLOAT;
				value2->u.floatValue = (float)value2->u.intValue;
				return;
			}

		}
	}
	else
	{
		switch ( type2 )
		{
		case VAR_STRING:
			switch ( type1 )
			{
			case VAR_VECTOR:
				value1->type = VAR_STRING;
				constTempVector = value1->u.vectorValue;
				value1->u.intValue = SL_GetStringForVector(constTempVector);
				RemoveRefToVector(constTempVector);
				return;

			case VAR_FLOAT:
				value1->type = VAR_STRING;
				value1->u.intValue = SL_GetStringForFloat(value1->u.floatValue);
				return;

			case VAR_INTEGER:
				value1->type = VAR_STRING;
				value1->u.intValue = SL_GetStringForInt(value1->u.intValue);
				return;
			}

		case VAR_FLOAT:
			if ( type1 == VAR_INTEGER )
			{
				value1->type = VAR_FLOAT;
				value1->u.floatValue = (float)value1->u.intValue;
				return;
			}

		}
	}

	Scr_UnmatchingTypesError(value1, value2);
}

/*
==============
Scr_EvalOr
==============
*/
void Scr_EvalOr( VariableValue *value1, VariableValue *value2 )
{
	if ( value1->type == VAR_INTEGER && value2->type == VAR_INTEGER )
	{
		value1->u.intValue |= value2->u.intValue;
	}
	else
	{
		Scr_UnmatchingTypesError(value1, value2);
	}
}

/*
==============
Scr_EvalExOr
==============
*/
void Scr_EvalExOr( VariableValue *value1, VariableValue *value2 )
{
	if ( value1->type == VAR_INTEGER && value2->type == VAR_INTEGER )
	{
		value1->u.intValue ^= value2->u.intValue;
	}
	else
	{
		Scr_UnmatchingTypesError(value1, value2);
	}
}

/*
==============
Scr_EvalAnd
==============
*/
void Scr_EvalAnd( VariableValue *value1, VariableValue *value2 )
{
	if ( value1->type == VAR_INTEGER && value2->type == VAR_INTEGER )
	{
		value1->u.intValue &= value2->u.intValue;
	}
	else
	{
		Scr_UnmatchingTypesError(value1, value2);
	}
}

/*
==============
Scr_EvalEquality
==============
*/
void Scr_EvalEquality( VariableValue *value1, VariableValue *value2 )
{
	int equal;

	Scr_CastWeakerPair(value1, value2);

	switch ( value1->type )
	{
	case VAR_UNDEFINED:
		value1->type = VAR_INTEGER;
		value1->u.intValue = 1;
		break;

	case VAR_INTEGER:
		value1->u.intValue = value1->u.intValue == value2->u.intValue;
		break;

	case VAR_FLOAT:
		value1->type = VAR_INTEGER;
		value1->u.intValue = I_fabs(value1->u.floatValue - value2->u.floatValue) < 0.000001f;
		break;

	case VAR_STRING:
	case VAR_ISTRING:
		value1->type = VAR_INTEGER;
		equal = value1->u.intValue == value2->u.intValue;

		SL_RemoveRefToString(value1->u.intValue);
		SL_RemoveRefToString(value2->u.intValue);

		value1->u.intValue = equal;
		break;

	case VAR_VECTOR:
		value1->type = VAR_INTEGER;
		equal = value1->u.vectorValue[0] == value2->u.vectorValue[0]
			&& value1->u.vectorValue[1] == value2->u.vectorValue[1]
			&& value1->u.vectorValue[2] == value2->u.vectorValue[2];

		RemoveRefToVector(value1->u.vectorValue);
		RemoveRefToVector(value2->u.vectorValue);

		value1->u.intValue = equal;
		break;

	case VAR_POINTER:
		if ( (scrVarGlob.variableList[value1->u.intValue].w.type & VAR_MASK) == VAR_ARRAY || (scrVarGlob.variableList[value2->u.intValue].w.type & VAR_MASK) == VAR_ARRAY )
		{
			if ( !scrVarPub.evaluate )
			{
				goto unmatched;
			}
		}

		value1->type = VAR_INTEGER;
		equal = value1->u.intValue == value2->u.intValue;

		RemoveRefToObject(value1->u.intValue);
		RemoveRefToObject(value2->u.intValue);

		value1->u.intValue = equal;
		break;

	case VAR_ANIMATION:
		value1->type = VAR_INTEGER;
		value1->u.intValue = value1->u.intValue == value2->u.intValue;
		break;

	case VAR_FUNCTION:
		value1->type = VAR_INTEGER;
		value1->u.intValue = value1->u.intValue == value2->u.intValue;
		break;

	default:
unmatched:
		Scr_UnmatchingTypesError(value1, value2);
		break;
	}
}

/*
==============
Scr_EvalLess
==============
*/
void Scr_EvalLess(VariableValue *value1, VariableValue *value2)
{
	Scr_CastWeakerPair(value1, value2);
	assert(value1->type == value2->type);

	switch ( value1->type )
	{
	case VAR_INTEGER:
		value1->u.intValue = value1->u.intValue < value2->u.intValue;
		break;

	case VAR_FLOAT:
		value1->type = VAR_INTEGER;
		value1->u.intValue = value1->u.floatValue < value2->u.floatValue;
		break;

	default:
		Scr_UnmatchingTypesError(value1, value2);
		break;
	}
}

/*
==============
Scr_EvalGreaterEqual
==============
*/
void Scr_EvalGreaterEqual( VariableValue *value1, VariableValue *value2 )
{
	Scr_EvalLess(value1, value2);
	assert((value1->type == VAR_INTEGER) || (value1->type == VAR_UNDEFINED));

	value1->u.intValue = value1->u.intValue == 0;
}

/*
==============
Scr_EvalInequality
==============
*/
void Scr_EvalInequality( VariableValue *value1, VariableValue *value2 )
{
	Scr_EvalEquality(value1, value2);
	assert((value1->type == VAR_INTEGER) || (value1->type == VAR_UNDEFINED));

	value1->u.intValue = value1->u.intValue == 0;
}

/*
==============
Scr_EvalGreater
==============
*/
void Scr_EvalGreater( VariableValue *value1, VariableValue *value2 )
{
	Scr_CastWeakerPair(value1, value2);
	assert(value1->type == value2->type);

	switch ( value1->type )
	{
	case VAR_INTEGER:
		value1->u.intValue = value1->u.intValue > value2->u.intValue;
		break;

	case VAR_FLOAT:
		value1->type = VAR_INTEGER;
		value1->u.intValue = value1->u.floatValue > value2->u.floatValue;
		break;

	default:
		Scr_UnmatchingTypesError(value1, value2);
		break;
	}
}

/*
==============
Scr_EvalLessEqual
==============
*/
void Scr_EvalLessEqual( VariableValue *value1, VariableValue *value2 )
{
	Scr_EvalGreater(value1, value2);
	assert((value1->type == VAR_INTEGER) || (value1->type == VAR_UNDEFINED));

	value1->u.intValue = value1->u.intValue == 0;
}

/*
==============
Scr_EvalShiftLeft
==============
*/
void Scr_EvalShiftLeft( VariableValue *value1, VariableValue *value2 )
{
	if ( value1->type == VAR_INTEGER && value2->type == VAR_INTEGER )
	{
		value1->u.intValue <<= value2->u.intValue;
	}
	else
	{
		Scr_UnmatchingTypesError(value1, value2);
	}
}

/*
==============
Scr_EvalShiftRight
==============
*/
void Scr_EvalShiftRight( VariableValue *value1, VariableValue *value2 )
{
	if ( value1->type == VAR_INTEGER && value2->type == VAR_INTEGER )
	{
		value1->u.intValue >>= value2->u.intValue;
	}
	else
	{
		Scr_UnmatchingTypesError(value1, value2);
	}
}

/*
==============
Scr_EvalPlus
==============
*/
void Scr_EvalPlus( VariableValue *value1, VariableValue *value2 )
{
	unsigned int s;
	float *tempVector;
	char str[8192];
	int len, s1len;
	const char *s1, *s2;

	Scr_CastWeakerStringPair(value1, value2);

	switch ( value1->type )
	{
	case VAR_INTEGER:
		value1->u.intValue += value2->u.intValue;
		break;

	case VAR_FLOAT:
		value1->u.floatValue = value1->u.floatValue + value2->u.floatValue;
		break;

	case VAR_STRING:
		s1 = SL_ConvertToString(value1->u.stringValue);
		s2 = SL_ConvertToString(value2->u.stringValue);

		s1len = SL_GetStringLen(value1->u.stringValue);
		len = s1len + SL_GetStringLen(value2->u.stringValue) + 1;

		// the target tests the FIRST string's length against the 8192 literal
		// (signed cmp/jle against 0x2000 on the s1len slot), not the concatenated
		// length -- reproduced as the binary has it.
		if ( s1len > 8192 )
		{
			SL_RemoveRefToString(value1->u.stringValue);
			SL_RemoveRefToString(value2->u.stringValue);

			value1->type = VAR_UNDEFINED;
			value2->type = VAR_UNDEFINED;

			Scr_Error(va("cannot concat \"%s\" and \"%s\" - max string length exceeded", s1, s2));
			return;
		}

		strcpy(str, s1);
		strcpy(str + s1len, s2);

		s = SL_GetStringOfLen(str, 0, len, 14);

		SL_RemoveRefToString(value1->u.stringValue);
		SL_RemoveRefToString(value2->u.stringValue);

		value1->u.stringValue = s;
		break;

	case VAR_VECTOR:
		tempVector = Scr_AllocVectorInternal();

		tempVector[0] = value1->u.vectorValue[0] + value2->u.vectorValue[0];
		tempVector[1] = value1->u.vectorValue[1] + value2->u.vectorValue[1];
		tempVector[2] = value1->u.vectorValue[2] + value2->u.vectorValue[2];

		RemoveRefToVector(value1->u.vectorValue);
		RemoveRefToVector(value2->u.vectorValue);

		value1->u.vectorValue = tempVector;
		break;

	default:
		Scr_UnmatchingTypesError(value1, value2);
		break;
	}
}

/*
==============
Scr_EvalMinus
==============
*/
void Scr_EvalMinus( VariableValue *value1, VariableValue *value2 )
{
	float *tempVector;

	Scr_CastWeakerPair(value1, value2);

	switch ( value1->type )
	{
	case VAR_INTEGER:
		value1->u.intValue = value1->u.intValue - value2->u.intValue;
		break;

	case VAR_FLOAT:
		value1->u.floatValue = value1->u.floatValue - value2->u.floatValue;
		break;

	case VAR_VECTOR:
		tempVector = Scr_AllocVectorInternal();

		tempVector[0] = value1->u.vectorValue[0] - value2->u.vectorValue[0];
		tempVector[1] = value1->u.vectorValue[1] - value2->u.vectorValue[1];
		tempVector[2] = value1->u.vectorValue[2] - value2->u.vectorValue[2];

		RemoveRefToVector(value1->u.vectorValue);
		RemoveRefToVector(value2->u.vectorValue);

		value1->u.vectorValue = tempVector;
		break;

	default:
		Scr_UnmatchingTypesError(value1, value2);
		break;
	}
}

/*
==============
Scr_EvalMultiply
==============
*/
void Scr_EvalMultiply( VariableValue *value1, VariableValue *value2 )
{
	Scr_CastWeakerPair(value1, value2);

	switch ( value1->type )
	{
	case VAR_INTEGER:
		value1->u.intValue *= value2->u.intValue;
		break;

	case VAR_FLOAT:
		value1->u.floatValue *= value2->u.floatValue;
		break;

	default:
		Scr_UnmatchingTypesError(value1, value2);
		break;
	}
}

/*
==============
Scr_EvalDivide
==============
*/
void Scr_EvalDivide( VariableValue *value1, VariableValue *value2 )
{
	Scr_CastWeakerPair(value1, value2);

	switch ( value1->type )
	{
	case VAR_INTEGER:
		value1->type = VAR_FLOAT;

		if ( value2->u.intValue )
		{
			value1->u.floatValue = (float)value1->u.intValue / (float)value2->u.intValue;
		}
		else
		{
			value1->u.floatValue = 0;
			Scr_Error("divide by 0");
		}
		break;

	case VAR_FLOAT:
		if ( value2->u.floatValue != 0 )
		{
			value1->u.floatValue = value1->u.floatValue / value2->u.floatValue;
		}
		else
		{
			value1->u.floatValue = 0;
			Scr_Error("divide by 0");
		}
		break;

	default:
		Scr_UnmatchingTypesError(value1, value2);
		break;
	}
}

/*
==============
Scr_EvalMod
==============
*/
void Scr_EvalMod( VariableValue *value1, VariableValue *value2 )
{
	if ( value1->type == VAR_INTEGER && value2->type == VAR_INTEGER )
	{
		if ( value2->u.intValue )
		{
			value1->u.intValue %= value2->u.intValue;
		}
		else
		{
			value1->u.intValue = 0;
			Scr_Error("divide by 0");
		}
	}
	else
	{
		Scr_UnmatchingTypesError(value1, value2);
	}
}

/*
==============
Scr_EvalBinaryOperator
==============
*/
void Scr_EvalBinaryOperator( int op, VariableValue *value1, VariableValue *value2 )
{
	switch ( op )
	{
	case OP_bit_or:
		Scr_EvalOr(value1, value2);
		break;

	case OP_bit_ex_or:
		Scr_EvalExOr(value1, value2);
		break;

	case OP_bit_and:
		Scr_EvalAnd(value1, value2);
		break;

	case OP_equality:
		Scr_EvalEquality(value1, value2);
		break;

	case OP_inequality:
		Scr_EvalInequality(value1, value2);
		break;

	case OP_less:
		Scr_EvalLess(value1, value2);
		break;

	case OP_greater:
		Scr_EvalGreater(value1, value2);
		break;

	case OP_less_equal:
		Scr_EvalLessEqual(value1, value2);
		break;

	case OP_greater_equal:
		Scr_EvalGreaterEqual(value1, value2);
		break;

	case OP_shift_left:
		Scr_EvalShiftLeft(value1, value2);
		break;

	case OP_shift_right:
		Scr_EvalShiftRight(value1, value2);
		break;

	case OP_plus:
		Scr_EvalPlus(value1, value2);
		break;

	case OP_minus:
		Scr_EvalMinus(value1, value2);
		break;

	case OP_multiply:
		Scr_EvalMultiply(value1, value2);
		break;

	case OP_divide:
		Scr_EvalDivide(value1, value2);
		break;

	case OP_mod:
		Scr_EvalMod(value1, value2);
		break;
	}
}

/*
==============
IsObjectFree
==============
*/
bool IsObjectFree( unsigned int id )
{
	return ( scrVarGlob.variableList[id].w.status & VAR_STAT_MASK ) == VAR_STAT_FREE;
}

/*
==============
GetObjectType
==============
*/
int GetObjectType( unsigned int id )
{
	return scrVarGlob.variableList[id].w.type & VAR_MASK;
}

/*
==============
Scr_FreeEntityNum
==============
*/
void Scr_FreeEntityNum( int entnum, int classnum )
{
	unsigned int entArrayId, entnumId, entId;
	VariableValueInternal *entryValue;

	if ( !scrVarPub.bInited )
	{
		return;
	}

	entArrayId = scrClassMap[classnum].entArrayId;
	assert(entArrayId);

	entnumId = FindArrayVariable(entArrayId, entnum);

	if ( !entnumId )
	{
		return;
	}

	entId = FindObject(entnumId);
	assert(entId);

	entryValue = &scrVarGlob.variableList[entId];
	assert((entryValue->w.type & VAR_MASK) == VAR_ENTITY);
	assert((entryValue->w.classnum >> VAR_NAME_BITS) == classnum);

	entryValue->w.type &= ~VAR_MASK;
	entryValue->w.type |= VAR_DEAD_ENTITY;

	AddRefToObject(entId);

	entryValue->u.o.u.nextEntId = scrVarPub.freeEntList;
	scrVarPub.freeEntList = entId;

	RemoveArrayVariable(entArrayId, entnum);
}

/*
==============
Scr_FreeEntityList
==============
*/
void Scr_FreeEntityList()
{
	unsigned int register entId;
	VariableValueInternal *entryValue;

	while ( scrVarPub.freeEntList )
	{
		entId = scrVarPub.freeEntList;
		entryValue = &scrVarGlob.variableList[entId];

		scrVarPub.freeEntList = entryValue->u.o.u.nextEntId;
		entryValue->u.o.u.entnum = 0;

		Scr_CancelNotifyList(entId);

		if ( scrVarGlob.variableList[entryValue->nextSibling].hash.id != entId )
		{
			ClearObjectInternal(entId);
		}

		RemoveRefToObject(entId);
	}
}

/*
==============
Scr_FreeObjects
==============
*/
void Scr_FreeObjects()
{
	unsigned int id;
	VariableValueInternal *entryValue;

	for ( id = 1; id < VARIABLELIST_CHILD_SIZE; id++ )
	{
		entryValue = &scrVarGlob.variableList[id];

		if ( !( entryValue->w.status & VAR_STAT_MASK ) )
		{
			continue;
		}

		if ( (entryValue->w.type & VAR_MASK) == VAR_OBJECT || (entryValue->w.type & VAR_MASK) == VAR_DEAD_ENTITY )
		{
			Scr_CancelNotifyList(id);
			ClearObject(id);
		}
	}
}

/*
==============
Scr_SetClassMap
==============
*/
void Scr_SetClassMap( int classnum )
{
	assert(!scrClassMap[classnum].entArrayId);
	assert(!scrClassMap[classnum].id);

	scrClassMap[classnum].entArrayId = Scr_AllocArray();
	scrClassMap[classnum].id = Scr_AllocArray();
}

/*
==============
Scr_RemoveClassMap
==============
*/
void Scr_RemoveClassMap( int classnum )
{
	if ( !scrVarPub.bInited )
	{
		return;
	}

	RemoveRefToObject(scrClassMap[classnum].entArrayId);
	scrClassMap[classnum].entArrayId = 0;

	RemoveRefToObject(scrClassMap[classnum].id);
	scrClassMap[classnum].id = 0;
}

/*
==============
Scr_AddClassField
==============
*/
void Scr_AddClassField( int classnum, const char *name, unsigned int offset )
{
	unsigned int fieldId;
	VariableValueInternal *entryValue;
	unsigned int str;
	unsigned int classId;

	classId = scrClassMap[classnum].id;
	str = SL_GetCanonicalString(name);
	assert(!FindArrayVariable(classId, (unsigned)str));

	fieldId = GetNewArrayVariable(classId, str);
	entryValue = &scrVarGlob.variableList[fieldId];

	entryValue->w.type &= ~VAR_MASK;
	entryValue->w.type |= VAR_INTEGER;
	entryValue->u.u.intValue = (unsigned short)offset;

	str = SL_GetString_(name, 0, 15);
	assert(!FindVariable(classId, str));

	fieldId = GetNewVariable(classId, str);
	SL_RemoveRefToString(str);

	entryValue = &scrVarGlob.variableList[fieldId];

	entryValue->w.type &= ~VAR_MASK;
	entryValue->w.type |= VAR_INTEGER;
	entryValue->u.u.intValue = (unsigned short)offset;
}

/*
==============
Scr_GetOffset
==============
*/
int Scr_GetOffset( unsigned int classnum, const char *name )
{
	unsigned int fieldId;
	unsigned int classId;

	classId = scrClassMap[classnum].id;
	fieldId = FindVariable( classId, SL_ConvertFromString( name ) );

	if ( fieldId )
	{
		return scrVarGlob.variableList[fieldId].u.u.entityOffset;
	}

	return -1;
}

/*
==============
FindEntityId
==============
*/
unsigned int FindEntityId( int entnum, int classnum )
{
	unsigned int entArrayId, id;
	VariableValueInternal *entryValue;

	assert((unsigned)entnum < (1 << 16));
	entArrayId = scrClassMap[classnum].entArrayId;

	assert(entArrayId);
	id = FindArrayVariable(entArrayId, entnum);

	if ( !id )
	{
		return 0;
	}

	entryValue = &scrVarGlob.variableList[id];

	assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert((entryValue->w.type & VAR_MASK) == VAR_POINTER);
	assert(entryValue->u.u.pointerValue);

	return entryValue->u.u.pointerValue;
}

/*
==============
Scr_GetEntityId
==============
*/
unsigned int Scr_GetEntityId( int entnum, int classnum )
{
	unsigned int entArrayId;
	unsigned int id;
	unsigned int entId;
	VariableValueInternal *entryValue;
	unsigned int result;

	entArrayId = scrClassMap[classnum].entArrayId;
	id = GetArrayVariable( entArrayId, entnum );
	entryValue = &scrVarGlob.variableList[id];

	if ( ( entryValue->w.type & VAR_MASK ) != VAR_UNDEFINED )
	{
		result = entryValue->u.u.pointerValue;
	}
	else
	{
		entId = AllocEntity( classnum, entnum );

		entryValue->w.type |= VAR_POINTER;
		entryValue->u.u.pointerValue = entId;

		result = entId;
	}

	return result;
}

/*
==============
Scr_EvalArrayIndex
==============
*/
unsigned int Scr_EvalArrayIndex( unsigned int parentId, VariableValue *index )
{
	unsigned int stringValue;

	if ( index->type == VAR_INTEGER )
	{
		if ( IsValidArrayIndex(index->u.pointerValue) )
			return GetArrayVariable(parentId, index->u.pointerValue);

		Scr_Error(va("array index %d out of range", index->u.pointerValue));
		return 0;
	}

	if ( index->type == VAR_STRING )
	{
		stringValue = GetVariable(parentId, index->u.stringValue);
		SL_RemoveRefToString(index->u.stringValue);
		return stringValue;
	}

	Scr_Error(va("%s is not an array index", var_typename[index->type]));
	return 0;
}

/*
==============
Scr_FindArrayIndex
==============
*/
unsigned int Scr_FindArrayIndex( unsigned int parentId, VariableValue *index )
{
	unsigned int id;

	if ( index->type == VAR_INTEGER )
	{
		if ( IsValidArrayIndex(index->u.intValue) )
			return FindArrayVariable(parentId, index->u.intValue);

		Scr_Error(va("array index %d out of range", index->u.intValue));
		AddRefToObject(parentId);
		return 0;
	}

	if ( index->type == VAR_STRING )
	{
		id = FindVariable(parentId, index->u.stringValue);
		SL_RemoveRefToString(index->u.stringValue);
		return id;
	}

	Scr_Error(va("%s is not an array index", var_typename[index->type]));
	AddRefToObject(parentId);
	return 0;
}

/*
==============
Scr_EvalArray
==============
*/
void Scr_EvalArray( VariableValue *value, VariableValue *index )
{
	VariableValueInternal *entryValue;
	const char *s;
	char c[2];

	switch( value->type )
	{
	case VAR_STRING:
		if ( index->type == VAR_INTEGER )
		{
			if ( index->u.intValue >= 0 )
			{
				s = SL_ConvertToString(value->u.stringValue);

				if ( index->u.intValue < strlen(s) )
				{
					index->type = VAR_STRING;

					c[0] = s[index->u.intValue];
					c[1] = 0;

					index->u.stringValue = SL_GetStringOfLen(c, 0, sizeof(c), 14);

					SL_RemoveRefToString(value->u.stringValue);
				}
				else
				{
					goto string_index_range;
				}
			}
			else
			{
				goto string_index_range;
			}
		}
		else
		{
			goto not_string_index;
		}
		break;

	string_index_range:
		Scr_Error(va("string index %d out of range", index->u.intValue));
		break;

	not_string_index:
		Scr_Error(va("%s is not a string index", var_typename[index->type]));
		break;

	case VAR_VECTOR:
		if ( index->type == VAR_INTEGER )
		{
			if ( index->u.pointerValue < 3 )
			{
				index->type = VAR_FLOAT;
				index->u.floatValue = value->u.vectorValue[index->u.intValue];

				RemoveRefToVector(value->u.vectorValue);
			}
			else
			{
				goto vector_index_range;
			}
		}
		else
		{
			goto not_vector_index;
		}
		break;

	vector_index_range:
		Scr_Error(va("vector index %d out of range", index->u.intValue));
		break;

	not_vector_index:
		Scr_Error(va("%s is not a vector index", var_typename[index->type]));
		break;

	case VAR_POINTER:
		entryValue = &scrVarGlob.variableList[value->u.pointerValue];
		assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
		assert(IsObject(entryValue));

		if ( (entryValue->w.type & VAR_MASK) != VAR_ARRAY )
		{
			scrVarPub.error_index = 1;
			Scr_Error(va("%s is not an array", var_typename[entryValue->w.type & VAR_MASK]));
			return;
		}

		*index = Scr_EvalVariable(Scr_FindArrayIndex(value->u.pointerValue, index));

		RemoveRefToObject(value->u.pointerValue);
		break;

	default:
		assert(value->type != VAR_STACK);
		scrVarPub.error_index = 1;
		Scr_Error(va("%s is not an array, string, or vector", var_typename[value->type]));
		break;
	}
}

/*
==============
Scr_EvalArrayRef
==============
*/
unsigned int Scr_EvalArrayRef( unsigned int parentId )
{
	VariableValueInternal *parentValue, *entryValue, *entValue;
	unsigned int id;
	VariableValue varValue;
	unsigned int fieldId;

	if ( parentId == VARIABLELIST_CHILD_SIZE )
	{
		entValue = &scrVarGlob.variableList[scrVarPub.entId];
		assert((entValue->w.type & VAR_MASK) == VAR_ENTITY);
		assert((entValue->w.classnum >> VAR_NAME_BITS) < CLASS_NUM_COUNT);

		fieldId = FindArrayVariable(scrClassMap[(entValue->w.classnum >> VAR_NAME_BITS)].id, scrVarPub.entFieldName);

		// both arms share the alloc/undefined exits below
		if ( fieldId )
		{
			varValue = GetEntityFieldValue(entValue->w.classnum >> VAR_NAME_BITS, entValue->u.o.u.entnum, scrVarGlob.variableList[fieldId].u.u.entityOffset);

			if ( varValue.type != VAR_UNDEFINED )
			{
				if ( varValue.type == VAR_POINTER && !scrVarGlob.variableList[varValue.u.pointerValue].u.o.refCount )
				{
					RemoveRefToValue(&varValue);
					scrVarPub.error_index = 1;
					Scr_Error("read-only array cannot be changed");
					return 0;
				}

				RemoveRefToValue(&varValue);
				parentValue = NULL;
				goto haveArray;
			}
		}

		parentValue = &scrVarGlob.variableList[GetNewVariable(scrVarPub.entId, scrVarPub.entFieldName)];
		goto allocArray;
	}

	parentValue = &scrVarGlob.variableList[parentId];
	varValue.type = parentValue->w.type & VAR_MASK;

	if ( varValue.type == VAR_UNDEFINED )
	{
allocArray:
		parentValue->w.type |= VAR_POINTER;
		parentValue->u.u.pointerValue = Scr_AllocArray();

		return parentValue->u.u.pointerValue;
	}

	varValue.u = parentValue->u.u;

haveArray:
	if ( varValue.type != VAR_POINTER )
	{
		assert(varValue.type != VAR_STACK);
		scrVarPub.error_index = 1;

		switch ( varValue.type )
		{
		case VAR_STRING:
			Scr_Error("string characters cannot be individually changed");
			return 0;

		case VAR_VECTOR:
			Scr_Error("vector components cannot be individually changed");
			return 0;

		default:
			Scr_Error(va("%s is not an array", var_typename[varValue.type]));
			return 0;
		}
	}

	entryValue = &scrVarGlob.variableList[varValue.u.pointerValue];
	assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert(IsObject(entryValue));

	if ( (entryValue->w.type & VAR_MASK) != VAR_ARRAY )
	{
		scrVarPub.error_index = 1;
		Scr_Error(va("%s is not an array", var_typename[(entryValue->w.type & VAR_MASK)]));
		return 0;
	}

	if ( entryValue->u.o.refCount )
	{
		id = varValue.u.pointerValue;
		RemoveRefToObject(id);
		varValue.u.pointerValue = Scr_AllocArray();
		CopyArray(id, varValue.u.pointerValue);
		assert(parentValue);
		parentValue->u.u = varValue.u;
	}

	return varValue.u.pointerValue;
}

/*
==============
ClearArray
==============
*/
void ClearArray( unsigned int parentId, VariableValue *value )
{
	VariableValueInternal *parentValue, *entryValue, *entValue;
	unsigned int id;
	VariableValue varValue;
	unsigned int fieldId;

	if ( parentId == VARIABLELIST_CHILD_SIZE )
	{
		entValue = &scrVarGlob.variableList[scrVarPub.entId];
		assert((entValue->w.type & VAR_MASK) == VAR_ENTITY);
		assert((entValue->w.classnum >> VAR_NAME_BITS) < CLASS_NUM_COUNT);

		fieldId = FindArrayVariable(scrClassMap[(entValue->w.classnum >> VAR_NAME_BITS)].id, scrVarPub.entFieldName);

		if ( fieldId )
		{
			varValue = GetEntityFieldValue(entValue->w.classnum >> VAR_NAME_BITS, entValue->u.o.u.entnum, scrVarGlob.variableList[fieldId].u.u.entityOffset);
		}
		else
		{
			goto notArrayType;
		}

		if ( varValue.type != VAR_UNDEFINED )
		{
			if ( varValue.type == VAR_POINTER && !scrVarGlob.variableList[varValue.u.pointerValue].u.o.refCount )
			{
				RemoveRefToValue(&varValue);
				scrVarPub.error_index = 1;
				Scr_Error("read-only array cannot be changed");
				return;
			}

			RemoveRefToValue(&varValue);
			assert((varValue.type != VAR_POINTER) || !scrVarGlob.variableList[varValue.u.pointerValue].u.o.refCount);
			parentValue = NULL;
			goto notArrayTypeDone;
		}
		else
		{
			goto notArrayType;
		}
	notArrayType:
		varValue.type = VAR_UNDEFINED;
		goto notArray;
	notArrayTypeDone: ;
	}
	else
	{
		parentValue = &scrVarGlob.variableList[parentId];
		assert((parentValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);

		varValue.type = parentValue->w.type & VAR_MASK;
		varValue.u = parentValue->u.u;
	}

	if ( varValue.type != VAR_POINTER )
	{
notArray:
		assert(varValue.type != VAR_STACK);
		scrVarPub.error_index = 1;
		Scr_Error(va("%s is not an array", var_typename[varValue.type]));
		return;
	}

	entryValue = &scrVarGlob.variableList[varValue.u.pointerValue];
	assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert(IsObject(entryValue));

	if ( (entryValue->w.type & VAR_MASK) != VAR_ARRAY )
	{
		scrVarPub.error_index = 1;
		Scr_Error(va("%s is not an array", var_typename[(entryValue->w.type & VAR_MASK)]));
		return;
	}

	if ( entryValue->u.o.refCount )
	{
		id = varValue.u.pointerValue;
		RemoveRefToObject(id);
		varValue.u.pointerValue = Scr_AllocArray();
		CopyArray(id, varValue.u.pointerValue);
		assert(parentValue);
		parentValue->u.u = varValue.u;
	}

	if ( value->type == VAR_INTEGER )
	{
		if ( IsValidArrayIndex(value->u.pointerValue) )
			SafeRemoveArrayVariable(varValue.u.pointerValue, value->u.stringValue);
		else
			Scr_Error(va("array index %d out of range", value->u.pointerValue));
	}
	else if ( value->type == VAR_STRING )
	{
		SL_RemoveRefToString(value->u.stringValue);
		SafeRemoveVariable(varValue.u.pointerValue, value->u.stringValue);
	}
	else
	{
		Scr_Error(va("%s is not an array index", var_typename[value->type]));
	}
}

/*
==============
SetEmptyArray
==============
*/
void SetEmptyArray( unsigned int parentId )
{
	VariableValue tempValue;

	tempValue.type = VAR_POINTER;
	tempValue.u.pointerValue = Scr_AllocArray();

	SetVariableValue(parentId, &tempValue);
}

/*
==============
Scr_GetEntityIdRef
==============
*/
scr_entref_t Scr_GetEntityIdRef( unsigned int entId )
{
	scr_entref_t entref;
	VariableValueInternal *entValue;

	entValue = &scrVarGlob.variableList[entId];

	assert((entValue->w.type & VAR_MASK) == VAR_ENTITY);
	assert((entValue->w.name >> VAR_NAME_BITS) < CLASS_NUM_COUNT);

	entref.entnum = entValue->u.o.u.entnum;
	entref.classnum = entValue->w.classnum >> VAR_NAME_BITS;

	return entref;
}

/*
==============
CopyEntity
==============
*/
void CopyEntity( unsigned int parentId, unsigned int newParentId )
{
	VariableValueInternal *parentValue;
	unsigned int id;
	unsigned int name;
	VariableValueInternal *entryValue;
	VariableValueInternal *newEntryValue;
	int type;

	assert(parentId);
	assert(newParentId);

	parentValue = &scrVarGlob.variableList[parentId];

	assert((parentValue->w.status & VAR_STAT_MASK) == VAR_STAT_EXTERNAL);
	assert((parentValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert(IsObject( parentValue ));
	assert((parentValue->w.type & VAR_MASK) == VAR_ENTITY);

	assert((scrVarGlob.variableList[newParentId]->w.status & VAR_STAT_MASK) == VAR_STAT_EXTERNAL);
	assert((scrVarGlob.variableList[newParentId].w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert(IsObject( &scrVarGlob.variableList[newParentId] ));
	assert((scrVarGlob.variableList[newParentId].w.type & VAR_MASK) == VAR_ENTITY);

	for ( id = FindNextSibling(parentId); id; id = FindNextSibling(id) )
	{
		entryValue = &scrVarGlob.variableList[id];

		assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE && (entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_EXTERNAL);
		assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
		assert(!IsObject( entryValue ));

		name = entryValue->w.name >> VAR_NAME_BITS;
		assert(name != OBJECT_STACK);

		if ( name == OBJECT_NOTIFY_LIST )
		{
			continue;
		}

		assert(!FindVariableIndexHash( newParentId, name ));
		newEntryValue = &scrVarGlob.variableList[GetVariable(newParentId, name)];

		assert((newEntryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE && (newEntryValue->w.status & VAR_STAT_MASK) != VAR_STAT_EXTERNAL);
		assert((newEntryValue->w.type & VAR_MASK) == VAR_UNDEFINED);

		type = entryValue->w.type & VAR_MASK;
		assert(!(newEntryValue->w.type & VAR_MASK));

		newEntryValue->w.type |= type;
		assert((newEntryValue->w.name >> VAR_NAME_BITS) == name);
		newEntryValue->u.u = entryValue->u.u;

		AddRefToValue(type, newEntryValue->u.u);
	}
}

/*
==============
Scr_CopyEntityNum
==============
*/
void Scr_CopyEntityNum( int fromEntnum, int toEntnum, int classnum )
{
	unsigned int fromEntId = FindEntityId( fromEntnum, classnum );

	if ( fromEntId )
	{
		if ( !FindNextSibling(fromEntId) )
		{
			return;
		}

		assert( !FindEntityId( toEntnum, classnum ) );
		CopyEntity( fromEntId, Scr_GetEntityId( toEntnum, classnum ) );
	}
}

/*
==============
Scr_GetEntryUsage
==============
*/
float Scr_GetEntryUsage( unsigned int type, VariableUnion u )
{
	VariableValueInternal *parentValue;

	switch ( type )
	{
	case VAR_POINTER:
		parentValue = &scrVarGlob.variableList[u.pointerValue];

		if ( ( parentValue->w.type & VAR_MASK ) == VAR_ARRAY )
		{
			return Scr_GetObjectUsage( u.pointerValue ) / ( parentValue->u.o.refCount + 1.0f );
		}
	}

	return 0;
}

/*
==============
Scr_GetEntryUsage
==============
*/
float Scr_GetEntryUsage( VariableValueInternal *entryValue )
{
	return Scr_GetEntryUsage( entryValue->w.type & VAR_MASK, entryValue->u.u ) + 1.0f;
}

/*
==============
Scr_GetEndonUsage
==============
*/
float Scr_GetEndonUsage( unsigned int parentId )
{
	VariableValueInternal *parentValue;
	unsigned int id;

	parentValue = &scrVarGlob.variableList[parentId];
	assert((parentValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert(IsObject( parentValue ));

	id = FindObjectVariable( scrVarPub.pauseArrayId, parentId );

	if ( id )
	{
		return Scr_GetObjectUsage( FindObject( id ) );
	}
	else
	{
		return 0;
	}
}

/*
==============
Scr_GetObjectUsage
==============
*/
float Scr_GetObjectUsage( unsigned int parentId )
{
	VariableValueInternal *parentValue;
	unsigned int id;
	float usage;

	parentValue = &scrVarGlob.variableList[parentId];
	usage = 1.0;

	for ( id = FindNextSibling( parentId ); id; id = FindNextSibling( id ) )
	{
		usage += Scr_GetEntryUsage( &scrVarGlob.variableList[id] );
	}

	return usage;
}

/*
==============
Scr_GetThreadUsage
==============
*/
float Scr_GetThreadUsage( VariableStackBuffer *stackBuf, float *endonUsage )
{
	VariableUnion u;
	char *buf;
	int size;
	unsigned char type;
	float usage;
	unsigned int localId;

	size = stackBuf->size;
	buf = stackBuf->buf;
	buf += STACKBUF_BUFFER_SIZE * size;

	usage = Scr_GetObjectUsage(stackBuf->localId);
	*endonUsage = Scr_GetEndonUsage(stackBuf->localId);

	localId = stackBuf->localId;

	while ( size )
	{
		buf -= sizeof(VariableUnion);
		u.codePosValue = *(const char **)buf;

		buf -= sizeof(unsigned char);
		type = *(unsigned char *)buf;

		size--;

		if ( type != VAR_CODEPOS )
		{
			usage += Scr_GetEntryUsage( type, u );
			continue;
		}

		localId = GetParentLocalId(localId);

		usage += Scr_GetObjectUsage(localId);
		*endonUsage += Scr_GetEndonUsage(localId);
	}

	return usage;
}

/*
==============
Scr_FindField
==============
*/
unsigned int Scr_FindField( const char *name, int *type )
{
	const char *pos;
	unsigned int index;
	int len;

	assert(scrVarPub.fieldBuffer);

	for ( pos = scrVarPub.fieldBuffer; *pos; )
	{
		len = strlen(pos) + 1;

		if ( strcasecmp(name, pos) )
		{
			pos = &pos[len] + 3;
		}
		else
		{
			pos += len;
			index = *(unsigned short *)pos;
			pos += 2;
			*type = *pos;
			pos++;

			return index;
		}
	}

	return 0;
}

/*
==============
Scr_AddFieldsForFile
==============
*/
void Scr_AddFieldsForFile( const char *filename )
{
	int len;
	char *sourceBuffer;
	const char *sourcePos;
	char *token;
	char *targetPos;
	int type;
	unsigned int index;
	int lenPlus3;
	int i;
	int tempType;
	fileHandle_t f;

	len = FS_FOpenFileByMode(filename, &f, FS_READ);

	if ( len < 0 )
	{
		Com_Error(ERR_DROP, va("\x15" "cannot find '%s'", filename));
	}

	sourceBuffer = (char *)Hunk_AllocateTempMemoryHighInternal( len + 1 );
	FS_Read(sourceBuffer, len, f);

	sourceBuffer[len] = 0;
	FS_FCloseFile(f);

	sourcePos = sourceBuffer;
	Com_BeginParseSession("Scr_AddFields");

	while ( 1 )
	{
		token = Com_Parse(&sourcePos);

		if ( !sourcePos )
		{
			break;
		}

		if ( !strcmp(token, "float") )
		{
			type = VAR_FLOAT;
		}
		else if ( !strcmp(token, "int") )
		{
			type = VAR_INTEGER;
		}
		else if ( !strcmp(token, "string") )
		{
			type = VAR_STRING;
		}
		else if ( !strcmp(token, "vector") )
		{
			type = VAR_VECTOR;
		}
		else
		{
			Com_Error(ERR_DROP, va("\x15" "unknown type '%s' in '%s'", token, filename));
			return;
		}

		token = Com_Parse(&sourcePos);

		if ( !sourcePos )
		{
			Com_Error(ERR_DROP, va("\x15" "missing field name in '%s'", filename));
		}

		len = strlen(token) + 1;

		for ( i = len - 1; i >= 0; i-- )
		{
			token[i] = tolower(token[i]);
		}

		index = SL_GetCanonicalString(token);

		if ( Scr_FindField(token, &tempType) )
		{
			Com_Error(ERR_DROP, "\x15" "duplicate key '%s' in '%s'", token, filename);
		}

		//assert(targetPos == TempMalloc( 0 ) - 1);
		lenPlus3 = len + 3;
		targetPos = TempMalloc(lenPlus3);
		strcpy(targetPos, token);
		targetPos += len;

		*(unsigned short *)targetPos = index;
		targetPos += 2;

		*targetPos = type;
		*targetPos++;

		*targetPos = 0;
	}

	Com_EndParseSessionInl();
	Hunk_ClearTempMemoryHighInternal();
}

/*
==============
Scr_AddFields
==============
*/
void Scr_AddFields( const char *path, const char *extension )
{
	int numFiles;
	char **files;
	int i;
	char filename[MAX_QPATH];
	char *pTerm;

	files = FS_ListFiles(path, extension, FS_LIST_PURE_ONLY, &numFiles, 10);

	TempMemoryReset();

	scrVarPub.fieldBuffer = (const char *)Hunk_AllocLowInternal(0);
	*(char *)scrVarPub.fieldBuffer = 0;

	for ( i = 0; i < numFiles; i++ )
	{
		sprintf(filename, "%s/%s", path, files[i]);
		Scr_AddFieldsForFile(filename);
	}

	if ( files )
	{
		FS_FreeFileList(files, 10);
	}

	pTerm = (char *)TempMalloc(1);
	*pTerm = 0;

	Hunk_ConvertTempToPermLowInternal();
}

/*
==============
Scr_FreeValue
==============
*/
void Scr_FreeValue( unsigned int id )
{
	assert(id);
	RemoveRefToObject(id);
}

/*
==============
Scr_MakeValuePrimitive
==============
*/
int Scr_MakeValuePrimitive( unsigned int parentId )
{
	VariableValueInternal *parentValue;
	unsigned int id;
	unsigned int name;
	VariableValueInternal *entryValue;

	parentValue = &scrVarGlob.variableList[parentId];

	if ( ( parentValue->w.type & VAR_MASK ) != VAR_ARRAY )
	{
		return 0;
	}

restart:
	for ( id = FindNextSibling(parentId); id; id = FindNextSibling(id) )
	{
		entryValue = &scrVarGlob.variableList[id];
		name = entryValue->w.name >> VAR_NAME_BITS;

		switch ( entryValue->w.type & VAR_MASK )
		{
		case VAR_CODEPOS:
		case VAR_PRECODEPOS:
		case VAR_FUNCTION:
		case VAR_STACK:
		case VAR_ANIMATION:
			RemoveVariable(parentId, name);
			goto restart;

		case VAR_POINTER:
			if ( !Scr_MakeValuePrimitive(entryValue->u.u.pointerValue) )
			{
				RemoveVariable(parentId, name);
				goto restart;
			}
			break;

		case VAR_UNDEFINED:
		case VAR_STRING:
		case VAR_ISTRING:
		case VAR_VECTOR:
		case VAR_FLOAT:
		case VAR_INTEGER:
			break;
		}
	}

	return 1;
}

/*
==============
Scr_AllocGameVariable
==============
*/
void Scr_AllocGameVariable()
{
	if ( scrVarPub.gameId )
	{
		return;
	}

	scrVarPub.gameId = AllocValue();
	SetEmptyArray(scrVarPub.gameId);
}

/*
==============
Scr_FreeGameVariable
==============
*/
void Scr_FreeGameVariable( int bComplete )
{
	VariableValueInternal *entryValue;

	assert(scrVarPub.gameId);

	if ( bComplete )
	{
		FreeValue(scrVarPub.gameId);
		scrVarPub.gameId = 0;
		return;
	}

	entryValue = &scrVarGlob.variableList[scrVarPub.gameId];
	assert((entryValue->w.type & VAR_MASK) == VAR_POINTER);

	Scr_MakeValuePrimitive(entryValue->u.u.pointerValue);
}

/*
==============
Scr_GetChecksum
==============
*/
void Scr_GetChecksum( int *checksum )
{
	checksum[0] = scrVarPub.checksum;
	checksum[1] = scrCompilePub.programLen;
	checksum[2] = scrVarPub.endScriptBuffer - scrVarPub.programBuffer;
}

/*
==============
Scr_GetClassnumForCharId
==============
*/
int Scr_GetClassnumForCharId( char charId )
{
	for ( int i = 0; i < CLASS_NUM_COUNT; i++ )
	{
		if ( scrClassMap[i].charId == charId )
		{
			return i;
		}
	}

	return -1;
}

/*
==============
Scr_InitStringSet
==============
*/
unsigned int Scr_InitStringSet()
{
	return Scr_AllocArray();
}

/*
==============
Scr_AddStringSet
==============
*/
bool Scr_AddStringSet( unsigned int setId, const char *string )
{
	unsigned int name;
	VariableValue value;
	unsigned int id;
	bool result;

	name = SL_GetLowercaseString(string, 0);

	if ( FindVariable(setId, name) )
	{
		SL_RemoveRefToString(name);
		result = false;
	}
	else
	{
		id = GetVariable(setId, name);
		SL_RemoveRefToString(name);
		value.type = VAR_INTEGER;
		value.u.intValue = 0;
		SetVariableValue(id, &value);
		result = true;
	}

	return result;
}

/*
==============
Scr_ShutdownStringSet
==============
*/
void Scr_ShutdownStringSet( unsigned int setId )
{
	RemoveRefToObject( setId );
}


// Unreferenced storage; original declarations unknown (sized from the layout).
static char unusedStorage[262304];
