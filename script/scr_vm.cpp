#include "../qcommon/qcommon.h"
#include "script_public.h"
// Unreferenced storage at 0xdb00d8; original declaration unknown (sized from the layout).
int scrVmUnknown_DB00D8[65578];

static inline unsigned int Scr_GetLocalVarAtIndex( int index );
static inline unsigned int Scr_GetLocalVar( const char *pos );
static inline short Scr_ReadShort( const char **pos );
static inline int Scr_ReadInt( const char **pos );
static inline const float* Scr_ReadVector( const char **pos );
static inline const int* Scr_ReadIntArray( const char **pos, int count );
static inline unsigned short Scr_ReadUnsignedShort( const char **pos );
static inline unsigned int Scr_ReadUnsigned( const char **pos );
static inline float Scr_ReadFloat( const char **pos );
static inline const char* Scr_ReadCodePos( const char **pos );
void Scr_ClearErrorMessage();
void Scr_VM_Init();
void VM_Shutdown();
void Scr_Init();
void Scr_Settings( int developer, int developer_script, int abort_on_error );
void Scr_Shutdown();
void Scr_Abort();
void Scr_SetLoading( int bLoading );
void Scr_ErrorInternal();
unsigned int Scr_GetNumScriptThreads();
void Scr_ClearOutParams();
unsigned int GetDummyObject( void );
unsigned int GetDummyFieldValue( void );
unsigned int VM_ExecuteInternal( function_stack_t fs );
char* Scr_GetReturnPos( unsigned int *localId );
const char* Scr_GetNextCodepos( VariableValue *top, const char *pos, int opcode, int mode, unsigned int *localId );
void VM_CancelNotifyInternal( unsigned int notifyListOwnerId, unsigned int startLocalId, unsigned int notifyListId, unsigned int notifyNameListId, unsigned int stringValue );
void VM_CancelNotify( unsigned int notifyListOwnerId, unsigned int startLocalId );
VariableStackBuffer* VM_ArchiveStack( int size, const char *pos, VariableValue *top, unsigned int localVarCount, unsigned int *localId );
int Scr_AddLocalVars( unsigned int localId );
void VM_UnarchiveStack( unsigned int startLocalId, function_stack_t *fs, VariableStackBuffer *stackValue );
void VM_TerminateStack( unsigned int endLocalId, unsigned int startLocalId, VariableStackBuffer *stackValue );
void Scr_TerminateRunningThread( unsigned int localId );
void Scr_TerminateWaitThread( unsigned int localId, unsigned int startLocalId );
void Scr_CancelWaittill( unsigned int startLocalId );
void Scr_TerminateWaittillThread( unsigned int localId, unsigned int startLocalId );
unsigned int Scr_GetWaittillThreadStackId( unsigned int localId, unsigned int startLocalId );
bool Scr_IsEndonThread( unsigned int localId );
// retail calls RemoveVariable at the VM_Notify sites and TerminateThread out of line
void RemoveVariableNI( unsigned int parentId, unsigned int name );
extern scrVarGlob_t scrVarGlob;
unsigned int GetNewVariableIndexInternal2( unsigned int parentId, unsigned int name, unsigned int index );

// retail inlines GetVariable at the timeArrayId site but calls Internal2
static __declspec(noinline) unsigned int GetNewVariableIndexInternal2NI( unsigned int parentId, unsigned int name, unsigned int index )
{
	return GetNewVariableIndexInternal2( parentId, name, index );
}

__declspec(noinline) void Scr_TerminateThread( unsigned int localId );
void VM_Notify( unsigned int notifyListOwnerId, unsigned int stringValue, VariableValue *top );
void Scr_NotifyNum( int entnum, int classnum, unsigned int stringValue, unsigned int paramcount );
void Scr_CancelNotifyList( unsigned int notifyListOwnerId );
void VM_TerminateTime( unsigned int timeId );
void VM_Resume( unsigned int timeId );
void IncInParam();
unsigned int VM_Execute( unsigned int localId, const char *pos, unsigned int paramcount );
unsigned short Scr_ExecThread( int handle, unsigned int paramcount );
unsigned short Scr_ExecEntThreadNum( int entnum, int classnum, int handle, unsigned int paramcount );
void Scr_AddExecThread( int handle, unsigned int paramcount );
void Scr_AddExecEntThreadNum( int entnum, int classnum, int handle, unsigned int paramcount );
void Scr_FreeThread( unsigned short handle );
void Scr_ExecCode( const char *pos, unsigned int localId );
void VM_SetTime();
int Scr_FindAllThreadsInternal( unsigned int selfId, unsigned int threadId, int count, bool a4, unsigned int *threads );
void Scr_InitSystem( int sys );
void Scr_ShutdownSystem( unsigned char sys, int bComplete );
int Scr_IsSystemActive( bool unused );
int Scr_GetInt( unsigned int index );
scr_anim_s Scr_GetAnim( unsigned int index, XAnimTree_s *tree );
float Scr_GetFloat( unsigned int index );
unsigned int Scr_GetConstString( unsigned int index );
unsigned int Scr_GetConstLowercaseString( unsigned int index );
const char* Scr_GetString( unsigned int index );
unsigned int Scr_GetConstStringIncludeNull( unsigned int index );
const char* Scr_GetStringIncludeNull( unsigned int index );
const char* Scr_GetDebugString( unsigned int index );
unsigned int Scr_GetConstIString( unsigned int index );
const char* Scr_GetIString( unsigned int index );
void Scr_GetVector( unsigned int index, vec3_t vectorValue );
unsigned int Scr_GetFunc( unsigned int index );
scr_entref_t Scr_GetEntityRef( unsigned int index );
unsigned int Scr_GetObject( unsigned int index );
int Scr_GetType( unsigned int index );
const char* Scr_GetTypeName( unsigned int index );
int Scr_GetPointerType( unsigned int index );
unsigned int Scr_GetNumParam();
void Scr_AddBool( int value );
void Scr_AddInt( int value );
void Scr_AddFloat( float value );
void Scr_AddAnim( scr_anim_s value );
void Scr_AddUndefined();
void Scr_AddObject( unsigned int id );
void Scr_AddEntityNum( int entnum, int classnum );
void Scr_AddStruct();
void Scr_AddString( const char *value );
void Scr_AddIString( const char *value );
void Scr_AddConstString( unsigned int value );
void Scr_AddVector( const vec3_t value );
void Scr_MakeArray();
void Scr_AddArray();
void Scr_AddArrayStringIndexed( unsigned int stringValue );
void Scr_Error( const char *error );
void Scr_ErrorWithDialogMessage( const char *error, const char *dialog_error );
void Scr_TerminalError( const char *error );
void Scr_ParamError( unsigned int index, const char *error );
void Scr_ObjectError( const char *error );
bool SetEntityFieldValue( unsigned int classnum, int entnum, int offset, VariableValue *value );
VariableValue GetEntityFieldValue( unsigned int classnum, int entnum, int offset );
void Scr_SetStructField( unsigned int structId, unsigned int index );
void Scr_SetDynamicEntityField( int entnum, int classnum, unsigned int index );
void Scr_IncTime();
void Scr_DecTime();
void Scr_RunCurrentThreads();
void Scr_ResetTimeout();
void Scr_StackClear( void );
void Scr_TraverseScript( char const *pos );

char g_EndPos = 0;
static jmp_buf g_script_error[MAX_VM_STACK_DEPTH + 1];
static int g_script_error_level;
scrVmGlob_t scrVmGlob __attribute__((aligned(128)));
scrVmPub_t scrVmPub __attribute__((aligned(128)));
/*
==============
VM_Resume
==============
*/
unsigned int VM_ExecuteInternal( function_stack_t fs );
VariableStackBuffer* VM_ArchiveStack( int size, const char *pos, VariableValue *top, unsigned int localVarCount, unsigned int *localId );
void VM_Notify( unsigned int notifyListOwnerId, unsigned int stringValue, VariableValue *top );
unsigned int VM_Execute( unsigned int localId, const char *pos, unsigned int paramcount );
void Scr_Error( const char *error );
void Scr_TerminalError( const char *error );
void Scr_RunCurrentThreads();
void Scr_ResetTimeout();

/*
==============
Scr_IsSystemActive
==============
*/

/*
==============
Scr_ResetTimeout
==============
*/
// retail times scripts in rdtsc/4 ticks; the result is left in edx:eax
#pragma warning(disable: 4035)

/*
==============
Scr_ReadShort
==============
*/

/*
==============
Scr_ReadInt
==============
*/

/*
==============
Scr_ReadUnsignedShort
==============
*/

/*
==============
Scr_ReadUnsigned
==============
*/

/*
==============
Scr_ReadCodePos
==============
*/

/*
==============
VM_Shutdown
==============
*/

/*
==============
Scr_GetNumScriptThreads
==============
*/

/*
==============
Scr_GetNextCodepos
==============
*/

/*
==============
Scr_TerminateThread
==============
*/

/*
==============
VM_Notify
==============
*/

/*
==============
Scr_GetStringIncludeNull
==============
*/

/*
==============
Scr_GetIString
==============
*/

/*
==============
Scr_AddBool
==============
*/

/*
==============
Scr_AddInt
==============
*/


static __forceinline unsigned int GetVariableI2NI( unsigned int parentId, unsigned int name )
{
	unsigned int hash = ( name + parentId ) % ( VARIABLELIST_CHILD_SIZE - 1 ) + 1;
	unsigned int newIndex = FindVariableIndexInternal( name, hash );

	if ( !newIndex )
	{
		newIndex = GetNewVariableIndexInternal2NI( parentId, name, hash );
	}

	return scrVarGlob.variableList[ newIndex ].hash.id;
}
/*
==============
Scr_GetLocalVarAtIndex
==============
*/
static inline unsigned int Scr_GetLocalVarAtIndex( int index )
{
	return *(scrVmPub.localVars - index);
}

/*
==============
Scr_GetLocalVar
==============
*/
static inline unsigned int Scr_GetLocalVar( const char *pos )
{
	return Scr_GetLocalVarAtIndex( *(unsigned char *)pos );
}

/*
==============
Scr_ReadVector
==============
*/
static inline const float* Scr_ReadVector( const char **pos )
{
	const float *value = reinterpret_cast<const float *>(*pos);
	*pos += sizeof(vec3_t);

	return value;
}

/*
==============
Scr_ReadIntArray
==============
*/
static inline const int* Scr_ReadIntArray( const char **pos, int count )
{
	const int *value = reinterpret_cast<const int *>(*pos);
	*pos += sizeof(const int) * count;

	return value;
}

/*
==============
Scr_ReadFloat
==============
*/
static inline float Scr_ReadFloat( const char **pos )
{
	float value = *(reinterpret_cast<const float *>(*pos));
	*pos += sizeof(float);

	return value;
}

/*
==============
Scr_ClearErrorMessage
==============
*/
void Scr_ClearErrorMessage()
{
	scrVarPub.error_message = NULL;
	scrVmGlob.dialog_error_message = NULL;

	scrVarPub.error_index = 0;
}

/*
==============
Scr_VM_Init
==============
*/
void Scr_VM_Init()
{
	scrVmPub.maxstack = &scrVmPub.stack[MAX_VM_OPERAND_STACK - 1];
	scrVmPub.top = scrVmPub.stack;

	scrVmPub.function_count = 0;
	scrVmPub.function_frame = scrVmPub.function_frame_start;

	scrVmPub.localVars = scrVmGlob.localVarsStack - 1;

	scrVarPub.evaluate = false;
	scrVmPub.debugCode = false;

	Scr_ClearErrorMessage();

	scrVmPub.terminal_error = false;

	scrVmPub.outparamcount = 0;
	scrVmPub.inparamcount = 0;

	scrVarPub.tempVariable = AllocValue();

	scrVarPub.timeArrayId = 0;
	scrVarPub.pauseArrayId = 0;

	scrVarPub.levelId = 0;
	scrVarPub.gameId = 0;
	scrVarPub.animId = 0;

	scrVarPub.freeEntList = 0;

	scrVmPub.stack->type = VAR_CODEPOS;
	scrVmGlob.loading = false;
}

/*
==============
Scr_Init
==============
*/
void Scr_Init()
{
	if ( scrVarPub.bInited )
	{
		return;
	}

	SL_Restart();

	Var_Init();
	Scr_VM_Init();

	scrCompilePub.script_loading = false;
	scrAnimPub.animtree_loading = false;

	scrCompilePub.scriptsPos = 0;
	scrCompilePub.loadedscripts = 0;

	scrAnimPub.animtrees = 0;

	scrCompilePub.builtinMeth = 0;
	scrCompilePub.builtinFunc = 0;

	scrVarPub.bInited = true;
}

/*
==============
Scr_Settings
==============
*/
void Scr_Settings( int developer, int developer_script, int abort_on_error )
{
	assert(!abort_on_error || developer);

	scrVarPub.developer = developer != 0;
	scrVarPub.developer_script = developer_script != 0;

	scrVmPub.abort_on_error = abort_on_error != 0;
}

/*
==============
Scr_Shutdown
==============
*/
void Scr_Shutdown()
{
	if ( !scrVarPub.bInited )
	{
		return;
	}

	scrVarPub.bInited = false;

	VM_Shutdown();
	Scr_ShutdownVariables();
	SL_Shutdown();
}

/*
==============
Scr_Abort
==============
*/
void Scr_Abort()
{
	scrVarPub.timeArrayId = 0;
	scrVarPub.bInited = false;
}

/*
==============
Scr_SetLoading
==============
*/
void Scr_SetLoading( int bLoading )
{
	scrVmGlob.loading = bLoading;
}

/*
==============
Scr_ErrorInternal
==============
*/
void Scr_ErrorInternal()
{
	if ( scrVarPub.evaluate || scrCompilePub.script_loading )
	{
		// intentionally empty
		if ( scrVmPub.terminal_error )
		{
		}
		else
		{
			return;
		}
	}
	else
	{
		if ( scrVarPub.developer && scrVmGlob.loading )
		{
			scrVmPub.terminal_error = true;
		}

		if ( scrVmPub.function_count || scrVmPub.debugCode )
		{
			longjmp(g_script_error[g_script_error_level], -1);
		}
	}

	Com_Error(ERR_DROP, "\x15%s", scrVarPub.error_message);
}

/*
==============
Scr_ClearOutParams
==============
*/
void Scr_ClearOutParams()
{
	while ( scrVmPub.outparamcount )
	{
		RemoveRefToValue(scrVmPub.top);

		--scrVmPub.top;
		--scrVmPub.outparamcount;
	}
}

/*
==============
GetDummyObject
==============
*/
unsigned int GetDummyObject( void )
{
	ClearVariableValue(scrVarPub.tempVariable);
	return GetObjectA(scrVarPub.tempVariable);
}

/*
==============
GetDummyFieldValue
==============
*/
unsigned int GetDummyFieldValue( void )
{
	ClearVariableValue(scrVarPub.tempVariable);
	return scrVarPub.tempVariable;
}

static __forceinline unsigned __int64 Sys_Rdtsc()
{
	__asm rdtsc
}
/*
==============
VM_Execute
==============
*/
unsigned int VM_ExecuteInternal( function_stack_t fs )
{
	unsigned int fieldValueId;
	unsigned int objectId;
	unsigned int selfId;
	unsigned int outparamcount;
	VariableValue tempValue;
	scr_entref_t entref;
	int waitTime;
	unsigned int stringValue;
	const char *tempCodePos;
	unsigned int threadId;
	int gOpcode;
	int gThreadCount;
	unsigned int parentLocalId;
	unsigned int id;
	int type;
	int gCaseCount;
	unsigned int caseValue;
	unsigned char removeCount;
	int jumpOffset;
	int classnum;
	int entnum;
	unsigned int currentCaseValue;
	const char *currentCodePos;
	unsigned int fieldName;
	VariableValue stackValue;
	unsigned int stackId;
	unsigned int builtinIndex;

	gThreadCount = 0;
	g_script_error_level++;

	if ( !sigsetjmp( g_script_error[ g_script_error_level ], 0 ) )
	{
		goto loop;
loop_dec_top:
		fs.top--;
loop:
		gOpcode = *(const unsigned char *)fs.pos++;

		switch ( gOpcode )
		{
		case OP_End:
			parentLocalId = GetSafeParentLocalId(fs.localId);
			Scr_KillThread(fs.localId);
			scrVmPub.localVars -= fs.localVarCount;
			while ( fs.top->type != VAR_CODEPOS )
			{
				RemoveRefToValue(fs.top);
				fs.top--;
			}
			scrVmPub.function_count--;
			scrVmPub.function_frame--;
			if ( !parentLocalId )
				goto thread_end;
			fs.top->type = VAR_UNDEFINED;
end:
			RemoveRefToObject(fs.localId);
			fs.pos = scrVmPub.function_frame->fs.pos;
			fs.localVarCount = scrVmPub.function_frame->fs.localVarCount;
			fs.localId = parentLocalId;
			goto loop;

		case OP_Return:
			parentLocalId = GetSafeParentLocalId(fs.localId);
			Scr_KillThread(fs.localId);
			scrVmPub.localVars -= fs.localVarCount;
			tempValue = *fs.top;
			fs.top--;
			while ( fs.top->type != VAR_CODEPOS )
			{
				RemoveRefToValue(fs.top);
				fs.top--;
			}
			scrVmPub.function_count--;
			scrVmPub.function_frame--;
			if ( !parentLocalId )
			{
				fs.top[1] = tempValue;
				goto thread_return;
			}
			*fs.top = tempValue;
			goto end;

		case OP_GetUndefined:
			fs.top++;
			fs.top->type = VAR_UNDEFINED;
			goto loop;

		case OP_GetZero:
			fs.top++;
			fs.top->type = VAR_INTEGER;
			fs.top->u.intValue = 0;
			goto loop;

		case OP_GetByte:
			fs.top++;
			fs.top->type = VAR_INTEGER;
			fs.top->u.intValue = *(const unsigned char *)fs.pos++;
			goto loop;

		case OP_GetNegByte:
			fs.top++;
			fs.top->type = VAR_INTEGER;
			fs.top->u.intValue = -*(const unsigned char *)fs.pos++;
			goto loop;

		case OP_GetUnsignedShort:
			fs.top++;
			fs.top->type = VAR_INTEGER;
			fs.top->u.intValue = Scr_ReadUnsignedShort(&fs.pos);
			goto loop;

		case OP_GetNegUnsignedShort:
			fs.top++;
			fs.top->type = VAR_INTEGER;
			fs.top->u.intValue = -Scr_ReadUnsignedShort(&fs.pos);
			goto loop;

		case OP_GetInteger:
			fs.top++;
			fs.top->type = VAR_INTEGER;
			fs.top->u.intValue = Scr_ReadInt(&fs.pos);
			goto loop;

		case OP_GetFloat:
			fs.top++;
			fs.top->type = VAR_FLOAT;
			fs.top->u.floatValue = Scr_ReadFloat(&fs.pos);
			goto loop;

		case OP_GetString:
			fs.top++;
			fs.top->type = VAR_STRING;
			fs.top->u.stringValue = Scr_ReadUnsignedShort(&fs.pos);
			SL_AddRefToString(fs.top->u.stringValue);
			goto loop;

		case OP_GetIString:
			fs.top++;
			fs.top->type = VAR_ISTRING;
			fs.top->u.stringValue = Scr_ReadUnsignedShort(&fs.pos);
			SL_AddRefToString(fs.top->u.stringValue);
			goto loop;

		case OP_GetVector:
			fs.top++;
			fs.top->type = VAR_VECTOR;
			fs.top->u.vectorValue = Scr_ReadVector(&fs.pos);
			goto loop;

		case OP_GetLevelObject:
			objectId = scrVarPub.levelId;
			goto loop;

		case OP_GetAnimObject:
			objectId = scrVarPub.animId;
			goto loop;

		case OP_GetSelf:
			fs.top++;
			fs.top->type = VAR_POINTER;
			fs.top->u.pointerValue = Scr_GetSelf(fs.localId);
			AddRefToObject(fs.top->u.pointerValue);
			goto loop;

		case OP_GetLevel:
			fs.top++;
			fs.top->type = VAR_POINTER;
			fs.top->u.pointerValue = scrVarPub.levelId;
			AddRefToObject(scrVarPub.levelId);
			goto loop;

		case OP_GetGame:
			fs.top++;
			*fs.top = Scr_EvalVariable(scrVarPub.gameId);
			goto loop;

		case OP_GetAnim:
			fs.top++;
			fs.top->type = VAR_POINTER;
			fs.top->u.pointerValue = scrVarPub.animId;
			AddRefToObject(scrVarPub.animId);
			goto loop;

		case OP_GetAnimation:
			fs.top++;
			fs.top->type = VAR_ANIMATION;
			fs.top->u.intValue = Scr_ReadInt(&fs.pos);
			goto loop;

		case OP_GetGameRef:
			fieldValueId = scrVarPub.gameId;
			goto loop;

		case OP_GetFunction:
			fs.top++;
			fs.top->type = VAR_FUNCTION;
			fs.top->u.codePosValue = Scr_ReadCodePos(&fs.pos);
			goto loop;

		case OP_CreateLocalVariable:
			scrVmPub.localVars++;
			fs.localVarCount++;
			*scrVmPub.localVars = GetNewVariable(fs.localId, Scr_ReadUnsignedShort(&fs.pos));
			goto loop;

		case OP_RemoveLocalVariables:
			removeCount = *fs.pos;
			fs.pos++;
			scrVmPub.localVars -= removeCount;
			fs.localVarCount -= removeCount;
			while ( removeCount )
			{
				RemoveNextVariable(fs.localId);
				removeCount--;
			}
			goto loop;

		case OP_EvalLocalVariableCached0:
			fs.top++;
			*fs.top = Scr_EvalVariable(*scrVmPub.localVars);
			goto loop;

		case OP_EvalLocalVariableCached1:
			fs.top++;
			*fs.top = Scr_EvalVariable(Scr_GetLocalVarAtIndex(1));
			goto loop;

		case OP_EvalLocalVariableCached2:
			fs.top++;
			*fs.top = Scr_EvalVariable(Scr_GetLocalVarAtIndex(2));
			goto loop;

		case OP_EvalLocalVariableCached3:
			fs.top++;
			*fs.top = Scr_EvalVariable(Scr_GetLocalVarAtIndex(3));
			goto loop;

		case OP_EvalLocalVariableCached4:
			fs.top++;
			*fs.top = Scr_EvalVariable(Scr_GetLocalVarAtIndex(4));
			goto loop;

		case OP_EvalLocalVariableCached5:
			fs.top++;
			*fs.top = Scr_EvalVariable(Scr_GetLocalVarAtIndex(5));
			goto loop;

		case OP_EvalLocalVariableCached:
			fs.top++;
			*fs.top = Scr_EvalVariable(Scr_GetLocalVar(fs.pos));
			fs.pos++;
			goto loop;

		case OP_EvalLocalArrayCached:
			fs.top++;
			*fs.top = Scr_EvalVariable(Scr_GetLocalVar(fs.pos));
			fs.pos++;

		case OP_EvalArray:
			Scr_EvalArray(fs.top, fs.top - 1);
			goto loop_dec_top;

		case OP_EvalLocalArrayRefCached0:
			fieldValueId = *scrVmPub.localVars;
			goto eval_array_ref;

		case OP_EvalLocalArrayRefCached:
			fieldValueId = Scr_GetLocalVar(fs.pos);
			fs.pos++;

		case OP_EvalArrayRef:
eval_array_ref:
			fieldValueId = Scr_EvalArrayIndex(Scr_EvalArrayRef(fieldValueId), fs.top);
			goto loop_dec_top;

		case OP_ClearArray:
			ClearArray(fieldValueId, fs.top);
			goto loop_dec_top;

		case OP_EmptyArray:
			fs.top++;
			fs.top->type = VAR_POINTER;
			fs.top->u.pointerValue = Scr_AllocArray();
			goto loop;

		case OP_GetSelfObject:
			objectId = Scr_GetSelf(fs.localId);
			if ( IsFieldObject(objectId) )
				goto loop;
			goto not_a_field_object;

		case OP_EvalLevelFieldVariable:
			objectId = scrVarPub.levelId;
eval_field_variable:
			fs.top++;
			*fs.top = Scr_EvalVariable(FindVariable(objectId, Scr_ReadUnsignedShort(&fs.pos)));
			goto loop;

		case OP_EvalAnimFieldVariable:
			objectId = scrVarPub.animId;
			goto eval_field_variable;

		case OP_EvalSelfFieldVariable:
			objectId = Scr_GetSelf(fs.localId);
			if ( !IsFieldObject(objectId) )
			{
				fs.top++;
				Scr_ReadUnsignedShort(&fs.pos);
				goto not_a_field_object;
			}

		case OP_EvalFieldVariable:
			fs.top++;
			*fs.top = Scr_FindVariableField(objectId, Scr_ReadUnsignedShort(&fs.pos));
			goto loop;

		case OP_EvalLevelFieldVariableRef:
			objectId = scrVarPub.levelId;
			goto eval_field_variable_ref;

		case OP_EvalAnimFieldVariableRef:
			objectId = scrVarPub.animId;
			goto eval_field_variable_ref;

		case OP_EvalSelfFieldVariableRef:
			objectId = Scr_GetSelf(fs.localId);

		case OP_EvalFieldVariableRef:
eval_field_variable_ref:
			fieldValueId = Scr_GetVariableField(objectId, Scr_ReadUnsignedShort(&fs.pos));
			goto loop;

		case OP_ClearFieldVariable:
			ClearVariableField(objectId, Scr_ReadUnsignedShort(&fs.pos), fs.top);
			goto loop;

		case OP_SafeCreateVariableFieldCached:
			scrVmPub.localVars++;
			fs.localVarCount++;
			*scrVmPub.localVars = GetNewVariable(fs.localId, Scr_ReadUnsignedShort(&fs.pos));

		case OP_SafeSetVariableFieldCached0:
			if ( fs.top->type != VAR_PRECODEPOS )
			{
				SetVariableValue(*scrVmPub.localVars, fs.top);
				goto loop_dec_top;
			}
			goto loop;

		case OP_SafeSetVariableFieldCached:
			if ( fs.top->type != VAR_PRECODEPOS )
			{
				SetVariableValue(Scr_GetLocalVar(fs.pos), fs.top);
				fs.pos++;
				goto loop_dec_top;
			}
			fs.pos++;
			goto loop;

		case OP_SafeSetWaittillVariableFieldCached:
			if ( fs.top->type != VAR_CODEPOS )
			{
				SetVariableValue(Scr_GetLocalVar(fs.pos), fs.top);
				fs.pos++;
				goto loop_dec_top;
			}
			ClearVariableValue(Scr_GetLocalVar(fs.pos));
			fs.pos++;
			goto loop;

		case OP_clearparams:
			assert(fs.top->type != VAR_PRECODEPOS);
			while ( fs.top->type != VAR_CODEPOS )
			{
				RemoveRefToValue(fs.top);
				fs.top--;
			}
			goto loop;

		case OP_checkclearparams:
			if ( fs.top->type == VAR_PRECODEPOS )
			{
				fs.top->type = VAR_CODEPOS;
				goto loop;
			}
			Scr_Error("function called with too many parameters");

		case OP_EvalLocalVariableRefCached0:
			fieldValueId = *scrVmPub.localVars;
			goto loop;

		case OP_EvalLocalVariableRefCached:
			fieldValueId = Scr_GetLocalVar(fs.pos);
			fs.pos++;
			goto loop;

		case OP_SetLevelFieldVariableField:
			SetVariableValue(GetVariable(scrVarPub.levelId, Scr_ReadUnsignedShort(&fs.pos)), fs.top);
			goto loop_dec_top;

		case OP_SetSelfFieldVariableField:
			fieldName = Scr_ReadUnsignedShort(&fs.pos);
			objectId = Scr_GetSelf(fs.localId);
			fieldValueId = Scr_GetVariableField(objectId, fieldName);

		case OP_SetVariableField:
set_variable_field:
			SetVariableFieldValue(fieldValueId, fs.top);
			goto loop_dec_top;

		case OP_SetAnimFieldVariableField:
			SetVariableValue(GetVariable(scrVarPub.animId, Scr_ReadUnsignedShort(&fs.pos)), fs.top);
			goto loop_dec_top;

		case OP_SetLocalVariableFieldCached0:
			SetVariableValue(*scrVmPub.localVars, fs.top);
			goto loop_dec_top;

		case OP_SetLocalVariableFieldCached:
			SetVariableValue(Scr_GetLocalVar(fs.pos), fs.top);
			fs.pos++;
			goto loop_dec_top;

		case OP_CallBuiltin1:
			scrVmPub.outparamcount = 1;
			goto call_builtin;

		case OP_CallBuiltin2:
			scrVmPub.outparamcount = 2;
			goto call_builtin;

		case OP_CallBuiltin3:
			scrVmPub.outparamcount = 3;
			goto call_builtin;

		case OP_CallBuiltin4:
			scrVmPub.outparamcount = 4;
			goto call_builtin;

		case OP_CallBuiltin5:
			scrVmPub.outparamcount = 5;
			goto call_builtin;

		case OP_CallBuiltin:
			scrVmPub.outparamcount = *(const unsigned char *)fs.pos++;

		case OP_CallBuiltin0:
call_builtin:
			scrVmPub.top = fs.top;
			builtinIndex = Scr_ReadUnsignedShort(&fs.pos);
			scrVmPub.function_frame->fs.pos = fs.pos;
			((void (*)(void))scrCompilePub.func_table[builtinIndex])();
post_builtin:
			fs.top = scrVmPub.top;
			fs.pos = scrVmPub.function_frame->fs.pos;
			if ( scrVmPub.outparamcount )
			{
				outparamcount = scrVmPub.outparamcount;
				scrVmPub.outparamcount = 0;
				scrVmPub.top -= outparamcount;
				do
				{
					RemoveRefToValue(fs.top);
					fs.top--;
					outparamcount--;
				}
				while ( outparamcount );
			}
			if ( scrVmPub.inparamcount )
			{
				scrVmPub.inparamcount = 0;
				goto loop;
			}
			fs.top++;
			fs.top->type = VAR_UNDEFINED;
			goto loop;

		case OP_CallBuiltinMethod1:
			scrVmPub.outparamcount = 1;
			goto call_builtin_method;

		case OP_CallBuiltinMethod2:
			scrVmPub.outparamcount = 2;
			goto call_builtin_method;

		case OP_CallBuiltinMethod3:
			scrVmPub.outparamcount = 3;
			goto call_builtin_method;

		case OP_CallBuiltinMethod4:
			scrVmPub.outparamcount = 4;
			goto call_builtin_method;

		case OP_CallBuiltinMethod5:
			scrVmPub.outparamcount = 5;
			goto call_builtin_method;

		case OP_CallBuiltinMethod:
			scrVmPub.outparamcount = *(const unsigned char *)fs.pos++;

		case OP_CallBuiltinMethod0:
call_builtin_method:
			scrVmPub.top = fs.top - 1;
			builtinIndex = Scr_ReadUnsignedShort(&fs.pos);
			if ( fs.top->type == VAR_POINTER )
			{
				objectId = fs.top->u.pointerValue;
				if ( GetObjectType(objectId) == VAR_ENTITY )
				{
					entref = Scr_GetEntityIdRef(objectId);
					RemoveRefToObject(objectId);
					scrVmPub.function_frame->fs.pos = fs.pos;
					((void (*)(scr_entref_t))scrCompilePub.func_table[builtinIndex])(entref);
					goto post_builtin;
				}
				type = GetObjectType(objectId);
				RemoveRefToObject(objectId);
				scrVarPub.error_index = -1;
				Scr_Error(va("%s is not an entity", var_typename[type]));
			}
			type = fs.top->type;
			RemoveRefToValue(fs.top);
			scrVarPub.error_index = -1;
			Scr_Error(va("%s is not an entity", var_typename[type]));

		case OP_wait:
			if ( fs.top->type == VAR_FLOAT )
			{
				if ( fs.top->u.floatValue < 0 )
					goto negative_wait;
				waitTime = I_fround(fs.top->u.floatValue * 20.0f);
				if ( !waitTime && fs.top->u.floatValue != 0 )
					waitTime = 1;
			}
			else if ( fs.top->type == VAR_INTEGER )
			{
				waitTime = fs.top->u.intValue * 20;
			}
			else
			{
				scrVarPub.error_index = 2;
				Scr_Error(va("type %s is not a float", var_typename[fs.top->type]));
			}
			if ( (unsigned int)waitTime < 0xFFFFFF )
			{
				if ( waitTime )
					Scr_ResetTimeout();
				waitTime = (scrVarPub.time + waitTime) & 0xFFFFFF;
				fs.top--;
				stackValue.type = VAR_STACK;
				stackValue.u.stackValue = VM_ArchiveStack(fs.top - fs.startTop, fs.pos, fs.top, fs.localVarCount, &fs.localId);
				id = GetArray(GetVariable(scrVarPub.timeArrayId, waitTime));
				stackId = GetNewObjectVariable(id, fs.localId);
				SetNewVariableValue(stackId, &stackValue);
				Scr_SetThreadWaitTime(fs.localId, waitTime);
				goto thread_end;
			}
			scrVarPub.error_index = 2;
			if ( waitTime >= 0 )
				Scr_Error("wait is too long");
negative_wait:
			Scr_Error("negative wait is not allowed");

		case OP_waittillFrameEnd:
			stackValue.type = VAR_STACK;
			stackValue.u.stackValue = VM_ArchiveStack(fs.top - fs.startTop, fs.pos, fs.top, fs.localVarCount, &fs.localId);
			id = GetArray(GetVariable(scrVarPub.timeArrayId, scrVarPub.time));
			stackId = GetNewObjectVariableReverse(id, fs.localId);
			SetNewVariableValue(stackId, &stackValue);
			Scr_SetThreadWaitTime(fs.localId, scrVarPub.time);
			goto thread_end;

		case OP_PreScriptCall:
			fs.top++;
			fs.top->type = VAR_PRECODEPOS;
			goto loop;

		case OP_ScriptFunctionCall2:
			fs.top++;
			fs.top->type = VAR_PRECODEPOS;

		case OP_ScriptFunctionCall:
			if ( scrVmPub.function_count < MAX_VM_STACK_DEPTH - 1 )
			{
				selfId = Scr_GetSelf(fs.localId);
				AddRefToObject(selfId);
				fs.localId = AllocChildThread(selfId, fs.localId);
				scrVmPub.function_frame->fs.pos = fs.pos;
				fs.pos = Scr_ReadCodePos(&scrVmPub.function_frame->fs.pos);
				goto function_call;
			}
			Scr_Error("script stack overflow (too many embedded function calls)");

		case OP_ScriptFunctionCallPointer:
			if ( fs.top->type == VAR_FUNCTION )
			{
				if ( scrVmPub.function_count < MAX_VM_STACK_DEPTH - 1 )
				{
					selfId = Scr_GetSelf(fs.localId);
					AddRefToObject(selfId);
					fs.localId = AllocChildThread(selfId, fs.localId);
					scrVmPub.function_frame->fs.pos = fs.pos;
					fs.pos = fs.top->u.codePosValue;
					fs.top--;
					goto function_call;
				}
				scrVarPub.error_index = 1;
				Scr_Error("script stack overflow (too many embedded function calls)");
			}
			Scr_Error(va("%s is not a function pointer", var_typename[fs.top->type]));

		case OP_ScriptMethodCall:
			if ( fs.top->type == VAR_POINTER )
			{
				if ( scrVmPub.function_count < MAX_VM_STACK_DEPTH - 1 )
				{
					fs.localId = AllocChildThread(fs.top->u.pointerValue, fs.localId);
					fs.top--;
					scrVmPub.function_frame->fs.pos = fs.pos;
					fs.pos = Scr_ReadCodePos(&scrVmPub.function_frame->fs.pos);
					goto function_call;
				}
				Scr_Error("script stack overflow (too many embedded function calls)");
			}
			goto not_an_object1;

		case OP_ScriptMethodCallPointer:
			if ( fs.top->type == VAR_FUNCTION )
			{
				tempCodePos = fs.top->u.codePosValue;
				fs.top--;
				if ( fs.top->type == VAR_POINTER )
				{
					if ( scrVmPub.function_count < MAX_VM_STACK_DEPTH - 1 )
					{
						fs.localId = AllocChildThread(fs.top->u.pointerValue, fs.localId);
						fs.top--;
						scrVmPub.function_frame->fs.pos = fs.pos;
						fs.pos = tempCodePos;
						goto function_call;
					}
					scrVarPub.error_index = 1;
					Scr_Error("script stack overflow (too many embedded function calls)");
				}
				goto not_an_object2;
			}
			RemoveRefToValue(fs.top);
			fs.top--;
			Scr_Error(va("%s is not a function pointer", var_typename[fs.top[1].type]));

		case OP_ScriptThreadCall:
			if ( scrVmPub.function_count < MAX_VM_STACK_DEPTH - 1 )
			{
				selfId = Scr_GetSelf(fs.localId);
				AddRefToObject(selfId);
				fs.localId = AllocThread(selfId);
				scrVmPub.function_frame->fs.pos = fs.pos;
				scrVmPub.function_frame->fs.startTop = fs.startTop;
				fs.pos = Scr_ReadCodePos(&scrVmPub.function_frame->fs.pos);
				fs.startTop = fs.top - Scr_ReadInt(&scrVmPub.function_frame->fs.pos);
				goto thread_call;
			}
			scrVarPub.error_index = 1;
			Scr_Error("script stack overflow (too many embedded function calls)");

		case OP_ScriptThreadCallPointer:
			if ( fs.top->type == VAR_FUNCTION )
			{
				if ( scrVmPub.function_count < MAX_VM_STACK_DEPTH - 1 )
				{
					tempCodePos = fs.top->u.codePosValue;
					fs.top--;
					selfId = Scr_GetSelf(fs.localId);
					AddRefToObject(selfId);
					fs.localId = AllocThread(selfId);
					scrVmPub.function_frame->fs.pos = fs.pos;
					scrVmPub.function_frame->fs.startTop = fs.startTop;
					fs.pos = tempCodePos;
					fs.startTop = fs.top - Scr_ReadInt(&scrVmPub.function_frame->fs.pos);
					goto thread_call;
				}
				scrVarPub.error_index = 1;
				Scr_Error("script stack overflow (too many embedded function calls)");
			}
			Scr_Error(va("%s is not a function pointer", var_typename[fs.top->type]));

		case OP_ScriptMethodThreadCall:
			if ( fs.top->type == VAR_POINTER )
			{
				if ( scrVmPub.function_count < MAX_VM_STACK_DEPTH - 1 )
				{
					fs.localId = AllocThread(fs.top->u.pointerValue);
					fs.top--;
					scrVmPub.function_frame->fs.pos = fs.pos;
					scrVmPub.function_frame->fs.startTop = fs.startTop;
					fs.pos = Scr_ReadCodePos(&scrVmPub.function_frame->fs.pos);
					fs.startTop = fs.top - Scr_ReadInt(&scrVmPub.function_frame->fs.pos);
					goto thread_call;
				}
				scrVarPub.error_index = 1;
				Scr_Error("script stack overflow (too many embedded function calls)");
			}
			goto not_an_object2;

		case OP_ScriptMethodThreadCallPointer:
			if ( fs.top->type == VAR_FUNCTION )
			{
				tempCodePos = fs.top->u.codePosValue;
				fs.top--;
				if ( fs.top->type == VAR_POINTER )
				{
					if ( scrVmPub.function_count < MAX_VM_STACK_DEPTH - 1 )
					{
						fs.localId = AllocThread(fs.top->u.pointerValue);
						fs.top--;
						scrVmPub.function_frame->fs.pos = fs.pos;
						scrVmPub.function_frame->fs.startTop = fs.startTop;
						fs.pos = tempCodePos;
						fs.startTop = fs.top - Scr_ReadInt(&scrVmPub.function_frame->fs.pos);
						goto thread_call;
					}
					scrVarPub.error_index = 1;
					Scr_Error("script stack overflow (too many embedded function calls)");
				}
				goto not_an_object2;
			}
			RemoveRefToValue(fs.top);
			fs.top--;
			Scr_Error(va("%s is not a function pointer", var_typename[fs.top[1].type]));

		case OP_DecTop:
			RemoveRefToValue(fs.top);
			goto loop_dec_top;

		case OP_CastFieldObject:
			objectId = Scr_EvalFieldObject(scrVarPub.tempVariable, fs.top);
			goto loop_dec_top;

		case OP_EvalLocalVariableObjectCached:
			objectId = Scr_EvalVariableObject(Scr_GetLocalVar(fs.pos));
			fs.pos++;
			goto loop;

		case OP_CastBool:
			Scr_CastBool(fs.top);
			goto loop;

		case OP_BoolNot:
			Scr_EvalBoolNot(fs.top);
			goto loop;

		case OP_BoolComplement:
			Scr_EvalBoolComplement(fs.top);
			goto loop;

		case OP_JumpOnFalse:
			Scr_CastBool(fs.top);
			jumpOffset = Scr_ReadUnsignedShort(&fs.pos);
			if ( fs.top->u.intValue )
				goto loop_dec_top;
			fs.pos += jumpOffset;
			goto loop_dec_top;

		case OP_JumpOnTrue:
			Scr_CastBool(fs.top);
			jumpOffset = Scr_ReadUnsignedShort(&fs.pos);
			if ( !fs.top->u.intValue )
				goto loop_dec_top;
			fs.pos += jumpOffset;
			goto loop_dec_top;

		case OP_JumpOnFalseExpr:
			Scr_CastBool(fs.top);
			jumpOffset = Scr_ReadUnsignedShort(&fs.pos);
			if ( fs.top->u.intValue )
				goto loop_dec_top;
			fs.pos += jumpOffset;
			goto loop;

		case OP_JumpOnTrueExpr:
			Scr_CastBool(fs.top);
			jumpOffset = Scr_ReadUnsignedShort(&fs.pos);
			if ( !fs.top->u.intValue )
				goto loop_dec_top;
			fs.pos += jumpOffset;
			goto loop;

		case OP_jump:
			jumpOffset = Scr_ReadInt(&fs.pos);
			fs.pos += jumpOffset;
			goto loop;

		case OP_jumpback:
			if ( (int)(Sys_Rdtsc() >> 2) - scrVmGlob.starttime >= -0x1000000 )
			{
				jumpOffset = Scr_ReadUnsignedShort(&fs.pos);
				fs.pos -= jumpOffset;
				goto loop;
			}
			if ( scrVmGlob.loading )
			{
				Com_Printf("script runtime warning: potential infinite loop in script.\n");
				Scr_PrintPrevCodePos(PMSG_CONSOLE, fs.pos, 0);
				jumpOffset = Scr_ReadUnsignedShort(&fs.pos);
				fs.pos -= jumpOffset;
				Scr_ResetTimeout();
				goto loop;
			}
			if ( !scrVmPub.abort_on_error )
			{
				Com_Printf("script runtime error: potential infinite loop in script - killing thread.\n");
				Scr_PrintPrevCodePos(PMSG_CONSOLE, fs.pos, 0);
				Scr_ResetTimeout();
				goto kill_thread;
			}
			Scr_TerminalError("potential infinite loop in script");

		case OP_inc:
			fs.top++;
			*fs.top = Scr_EvalVariableFieldInternal(fieldValueId);
			if ( fs.top->type == VAR_INTEGER )
			{
				fs.top->u.intValue++;
				fs.pos++;
				goto set_variable_field;
			}
			Scr_Error(va("++ must be applied to an int (applied to %s)", var_typename[fs.top->type]));

		case OP_dec:
			fs.top++;
			*fs.top = Scr_EvalVariableFieldInternal(fieldValueId);
			if ( fs.top->type == VAR_INTEGER )
			{
				fs.top->u.intValue--;
				fs.pos++;
				goto set_variable_field;
			}
			Scr_Error(va("-- must be applied to an int (applied to %s)", var_typename[fs.top->type]));

		case OP_bit_or:
			Scr_EvalOr(fs.top - 1, fs.top);
			goto loop_dec_top;

		case OP_bit_ex_or:
			Scr_EvalExOr(fs.top - 1, fs.top);
			goto loop_dec_top;

		case OP_bit_and:
			Scr_EvalAnd(fs.top - 1, fs.top);
			goto loop_dec_top;

		case OP_equality:
			Scr_EvalEquality(fs.top - 1, fs.top);
			goto loop_dec_top;

		case OP_inequality:
			Scr_EvalInequality(fs.top - 1, fs.top);
			goto loop_dec_top;

		case OP_less:
			Scr_EvalLess(fs.top - 1, fs.top);
			goto loop_dec_top;

		case OP_greater:
			Scr_EvalGreater(fs.top - 1, fs.top);
			goto loop_dec_top;

		case OP_less_equal:
			Scr_EvalLessEqual(fs.top - 1, fs.top);
			goto loop_dec_top;

		case OP_greater_equal:
			Scr_EvalGreaterEqual(fs.top - 1, fs.top);
			goto loop_dec_top;

		case OP_shift_left:
			Scr_EvalShiftLeft(fs.top - 1, fs.top);
			goto loop_dec_top;

		case OP_shift_right:
			Scr_EvalShiftRight(fs.top - 1, fs.top);
			goto loop_dec_top;

		case OP_plus:
			Scr_EvalPlus(fs.top - 1, fs.top);
			goto loop_dec_top;

		case OP_minus:
			Scr_EvalMinus(fs.top - 1, fs.top);
			goto loop_dec_top;

		case OP_multiply:
			Scr_EvalMultiply(fs.top - 1, fs.top);
			goto loop_dec_top;

		case OP_divide:
			Scr_EvalDivide(fs.top - 1, fs.top);
			goto loop_dec_top;

		case OP_mod:
			Scr_EvalMod(fs.top - 1, fs.top);
			goto loop_dec_top;

		case OP_size:
			Scr_EvalSizeValue(fs.top);
			goto loop;

		case OP_waittillmatch:
		case OP_waittill:
			if ( fs.top->type == VAR_POINTER )
			{
				if ( IsFieldObject(fs.top->u.pointerValue) )
				{
					tempValue.u.pointerValue = fs.top->u.pointerValue;
					fs.top--;
					if ( fs.top->type == VAR_STRING )
					{
						stringValue = fs.top->u.stringValue;
						fs.top--;
						stackValue.type = VAR_STACK;
						stackValue.u.stackValue = VM_ArchiveStack(fs.top - fs.startTop, fs.pos, fs.top, fs.localVarCount, &fs.localId);
						id = GetArray(GetVariable(GetArray(GetVariable(tempValue.u.pointerValue, OBJECT_NOTIFY_LIST)), stringValue));
						stackId = GetNewObjectVariable(id, fs.localId);
						SetNewVariableValue(stackId, &stackValue);
						tempValue.type = VAR_POINTER;
						SetNewVariableValue(GetNewObjectVariable(GetArray(GetObjectVariable(scrVarPub.pauseArrayId, Scr_GetSelf(fs.localId))), fs.localId), &tempValue);
						Scr_SetThreadNotifyName(fs.localId, stringValue);
						goto thread_end;
					}
					fs.top++;
					scrVarPub.error_index = 3;
					Scr_Error("first parameter of waittill must evaluate to a string");
				}
				goto not_a_field_object2;
			}
			goto not_an_object2;

		case OP_notify:
			if ( fs.top->type == VAR_POINTER )
			{
				id = fs.top->u.pointerValue;
				if ( IsFieldObject(id) )
				{
					fs.top--;
					if ( fs.top->type == VAR_STRING )
					{
						stringValue = fs.top->u.stringValue;
						fs.top--;
						scrVmPub.function_frame->fs.pos = fs.pos;
						VM_Notify(id, stringValue, fs.top);
						fs.pos = scrVmPub.function_frame->fs.pos;
						RemoveRefToObject(id);
						SL_RemoveRefToString(stringValue);
						while ( fs.top->type != VAR_PRECODEPOS )
						{
							RemoveRefToValue(fs.top);
							fs.top--;
						}
						goto loop_dec_top;
					}
					fs.top++;
					scrVarPub.error_index = 1;
					Scr_Error("first parameter of notify must evaluate to a string");
				}
				goto not_a_field_object2;
			}
			goto not_an_object2;

		case OP_endon:
			if ( fs.top->type == VAR_POINTER )
			{
				if ( IsFieldObject(fs.top->u.pointerValue) )
				{
					if ( fs.top[-1].type == VAR_STRING )
					{
						stringValue = fs.top[-1].u.stringValue;
						AddRefToObject(fs.localId);
						threadId = AllocThread(fs.localId);
						GetObjectVariable(GetArray(GetVariable(GetArray(GetVariable(fs.top->u.pointerValue, OBJECT_NOTIFY_LIST)), stringValue)), threadId);
						RemoveRefToObject(threadId);
						tempValue.type = VAR_POINTER;
						tempValue.u.pointerValue = fs.top->u.pointerValue;
						SetNewVariableValue(GetNewObjectVariable(GetArray(GetObjectVariable(scrVarPub.pauseArrayId, fs.localId)), threadId), &tempValue);
						Scr_SetThreadNotifyName(threadId, stringValue);
						fs.top -= 2;
						goto loop;
					}
					Scr_Error("first parameter of endon must evaluate to a string");
				}
				goto not_a_field_object1;
			}
			goto not_an_object1;

		case OP_voidCodepos:
			fs.top++;
			fs.top->type = VAR_PRECODEPOS;
			goto loop;

		case OP_switch:
			jumpOffset = Scr_ReadInt(&fs.pos);
			fs.pos += jumpOffset;
			gCaseCount = Scr_ReadUnsignedShort(&fs.pos);
			switch ( fs.top->type )
			{
			case VAR_INTEGER:
				if ( IsValidArrayIndex(fs.top->u.intValue) )
				{
					caseValue = GetInternalVariableIndex(fs.top->u.intValue);
					break;
				}
				Scr_Error(va("switch index %d out of range", fs.top->u.intValue));
			case VAR_STRING:
				caseValue = fs.top->u.stringValue;
				SL_RemoveRefToString(fs.top->u.stringValue);
				break;
			default:
				Scr_Error(va("cannot switch on %s", var_typename[fs.top->type]));
			}
			if ( gCaseCount )
			{
				do
				{
					currentCaseValue = Scr_ReadUnsigned(&fs.pos);
					currentCodePos = Scr_ReadCodePos(&fs.pos);
					if ( currentCaseValue == caseValue )
					{
						fs.pos = currentCodePos;
						goto loop_dec_top;
					}
					gCaseCount--;
				}
				while ( gCaseCount );
				if ( !currentCaseValue )
					fs.pos = currentCodePos;
			}
			goto loop_dec_top;

		case OP_endswitch:
			gCaseCount = Scr_ReadUnsignedShort(&fs.pos);
			Scr_ReadIntArray(&fs.pos, 2 * gCaseCount);
			goto loop;

		case OP_vector:
			fs.top -= 2;
			Scr_CastVector(fs.top);
			goto loop;

		case OP_NOP:
			goto loop;

		case OP_abort:
			g_script_error_level--;
			return 0;

		case OP_object:
			fs.top++;
			classnum = Scr_ReadInt(&fs.pos);
			entnum = Scr_ReadInt(&fs.pos);
			fs.top->u.pointerValue = FindEntityId(entnum, classnum);
			if ( fs.top->u.pointerValue )
				goto object_found;
			fs.top->type = VAR_UNDEFINED;
			Scr_Error("unknown object");
object_found:
			fs.top->type = VAR_POINTER;
			AddRefToObject(fs.top->u.pointerValue);
			goto loop;

		case OP_thread_object:
			fs.top++;
			fs.top->u.pointerValue = Scr_ReadUnsignedShort(&fs.pos);
			goto object_found;

		case OP_EvalLocalVariable:
			fs.top++;
			*fs.top = Scr_EvalVariable(FindVariable(fs.localId, Scr_ReadUnsignedShort(&fs.pos)));
			goto loop;

		case OP_EvalLocalVariableRef:
			fieldValueId = FindVariable(fs.localId, Scr_ReadUnsignedShort(&fs.pos));
			if ( fieldValueId )
				goto loop;
			Scr_Error("cannot create a new local variable in the debugger");

		case OP_prof_begin:
			fs.pos++;
			goto loop;

		case OP_prof_end:
			fs.pos++;
			goto loop;
		}

thread_call:
		scrVmPub.function_frame->fs.top = fs.startTop;
		scrVmPub.function_frame->topType = fs.startTop->type;
		fs.startTop->type = VAR_PRECODEPOS;
		gThreadCount++;
function_call:
		scrVmPub.function_frame->fs.localVarCount = fs.localVarCount;
		fs.localVarCount = 0;
		scrVmPub.function_count++;
		scrVmPub.function_frame++;
		scrVmPub.function_frame->fs.localId = fs.localId;
		goto loop;

thread_end:
		fs.startTop[1].type = VAR_UNDEFINED;
thread_return:
		if ( !gThreadCount )
		{
			g_script_error_level--;
			return fs.localId;
		}
		gThreadCount--;
		RemoveRefToObject(fs.localId);
		fs = scrVmPub.function_frame->fs;
		fs.top->type = scrVmPub.function_frame->topType;
		fs.top++;
		goto loop;

kill_thread:
		parentLocalId = GetSafeParentLocalId(fs.localId);
		Scr_KillThread(fs.localId);
		scrVmPub.localVars -= fs.localVarCount;
		while ( fs.top->type != VAR_CODEPOS )
		{
			RemoveRefToValue(fs.top);
			fs.top--;
		}
		scrVmPub.function_count--;
		scrVmPub.function_frame--;
		if ( !parentLocalId )
			goto thread_end;
		RemoveRefToObject(fs.localId);
		fs.pos = scrVmPub.function_frame->fs.pos;
		fs.localVarCount = scrVmPub.function_frame->fs.localVarCount;
		fs.localId = parentLocalId;
		fs.top--;
		goto kill_thread;

not_an_object1:
		type = fs.top->type;
		goto error_index_1;
not_a_field_object1:
		type = GetObjectType(fs.top->u.pointerValue);
		goto error_index_1;
not_an_object2:
		type = fs.top->type;
		goto error_index_2;
not_a_field_object2:
		type = GetObjectType(fs.top->u.pointerValue);
		goto error_index_2;
error_index_1:
		scrVarPub.error_index = 1;
		goto not_an_object_error;
error_index_2:
		scrVarPub.error_index = 2;
		goto not_an_object_error;
not_a_field_object:
		type = GetObjectType(objectId);
not_an_object_error:
		Scr_Error(va("%s is not an object", var_typename[type]));
	}

	switch ( gOpcode )
	{
	case OP_EvalLocalArrayRefCached0:
	case OP_EvalLocalArrayRefCached:
	case OP_EvalArrayRef:
	case OP_ClearArray:
	case OP_EvalLocalVariableRef:
		if ( scrVarPub.error_index < 0 )
			scrVarPub.error_index = 1;
		break;

	case OP_EvalSelfFieldVariable:
	case OP_EvalFieldVariable:
	case OP_ClearFieldVariable:
	case OP_SetVariableField:
	case OP_SetSelfFieldVariableField:
	case OP_inc:
	case OP_dec:
		scrVarPub.error_index = 0;
		break;

	case OP_CallBuiltin0:
	case OP_CallBuiltin1:
	case OP_CallBuiltin2:
	case OP_CallBuiltin3:
	case OP_CallBuiltin4:
	case OP_CallBuiltin5:
	case OP_CallBuiltin:
		if ( scrVarPub.error_index > 0 )
			scrVarPub.error_index = scrVmPub.outparamcount - scrVarPub.error_index + 1;
		break;

	case OP_CallBuiltinMethod0:
	case OP_CallBuiltinMethod1:
	case OP_CallBuiltinMethod2:
	case OP_CallBuiltinMethod3:
	case OP_CallBuiltinMethod4:
	case OP_CallBuiltinMethod5:
	case OP_CallBuiltinMethod:
		if ( scrVarPub.error_index > 0 )
			scrVarPub.error_index = scrVmPub.outparamcount - scrVarPub.error_index + 2;
		else if ( scrVarPub.error_index < 0 )
			scrVarPub.error_index = 1;
		break;
	}

	RuntimeError(fs.pos, scrVarPub.error_index, scrVarPub.error_message, scrVmGlob.dialog_error_message);
	Scr_ClearErrorMessage();

	switch ( gOpcode )
	{
	case OP_EvalSelfFieldVariableRef:
	case OP_EvalFieldVariableRef:
		fieldValueId = GetDummyFieldValue();
		break;

	case OP_EvalLocalArrayRefCached0:
	case OP_EvalLocalArrayRefCached:
	case OP_EvalArrayRef:
	case OP_EvalLocalVariableRef:
		fieldValueId = GetDummyFieldValue();
		goto remove_ref_dec_top;

	case OP_CastFieldObject:
		objectId = GetDummyObject();
		goto dec_top;

	case OP_EvalLocalVariableObjectCached:
		fs.pos++;

	case OP_GetSelfObject:
		objectId = GetDummyObject();
		break;

	case OP_EvalSelfFieldVariable:
	case OP_EvalFieldVariable:
		fs.top->type = VAR_UNDEFINED;
		break;

	case OP_ClearFieldVariable:
		if ( scrVmPub.outparamcount )
			scrVmPub.outparamcount = 0;
		break;

	case OP_SetSelfFieldVariableField:
		RemoveRefToValue(fs.top);
		scrVmPub.outparamcount = 0;
		goto dec_top;

	case OP_SetVariableField:
		if ( scrVmPub.outparamcount )
		{
			RemoveRefToValue(fs.top);
			scrVmPub.outparamcount = 0;
		}
		goto dec_top;

	case OP_CallBuiltin0:
	case OP_CallBuiltin1:
	case OP_CallBuiltin2:
	case OP_CallBuiltin3:
	case OP_CallBuiltin4:
	case OP_CallBuiltin5:
	case OP_CallBuiltin:
	case OP_CallBuiltinMethod0:
	case OP_CallBuiltinMethod1:
	case OP_CallBuiltinMethod2:
	case OP_CallBuiltinMethod3:
	case OP_CallBuiltinMethod4:
	case OP_CallBuiltinMethod5:
	case OP_CallBuiltinMethod:
		Scr_ClearOutParams();
		fs.top = scrVmPub.top + 1;
		fs.top->type = VAR_UNDEFINED;
		break;

	case OP_jumpback:
		jumpOffset = Scr_ReadUnsignedShort(&fs.pos);
		fs.pos -= jumpOffset;
		break;

	case OP_waittillmatch:
		fs.pos++;

	case OP_waittill:
	case OP_endon:
		RemoveRefToValue(fs.top);
		fs.top--;

	case OP_ClearArray:
	case OP_wait:
remove_ref_dec_top:
		RemoveRefToValue(fs.top);

	case OP_bit_or:
	case OP_bit_ex_or:
	case OP_bit_and:
	case OP_equality:
	case OP_inequality:
	case OP_less:
	case OP_greater:
	case OP_less_equal:
	case OP_greater_equal:
	case OP_shift_left:
	case OP_shift_right:
	case OP_plus:
	case OP_minus:
	case OP_multiply:
	case OP_divide:
	case OP_mod:
dec_top:
		fs.top--;
		break;

	case OP_EvalLocalArrayCached:
	case OP_EvalArray:
		RemoveRefToValue(fs.top);
		fs.top--;
		RemoveRefToValue(fs.top);
		fs.top->type = VAR_UNDEFINED;
		break;

	case OP_checkclearparams:
		assert(fs.top->type != VAR_CODEPOS);
		while ( fs.top->type != VAR_PRECODEPOS )
		{
			RemoveRefToValue(fs.top);
			fs.top--;
		}
		fs.top->type = VAR_CODEPOS;
		break;

	case OP_ScriptFunctionCall2:
	case OP_ScriptFunctionCall:
	case OP_ScriptMethodCall:
		Scr_ReadCodePos(&fs.pos);

	case OP_ScriptFunctionCallPointer:
	case OP_ScriptMethodCallPointer:
		assert(fs.top->type != VAR_CODEPOS);
		while ( fs.top->type != VAR_PRECODEPOS )
		{
			RemoveRefToValue(fs.top);
			fs.top--;
		}
		fs.top->type = VAR_UNDEFINED;
		break;

	case OP_ScriptThreadCall:
	case OP_ScriptMethodThreadCall:
		Scr_ReadCodePos(&fs.pos);

	case OP_ScriptThreadCallPointer:
	case OP_ScriptMethodThreadCallPointer:
		for ( outparamcount = Scr_ReadInt(&fs.pos); outparamcount; outparamcount-- )
		{
			RemoveRefToValue(fs.top);
			fs.top--;
		}
		fs.top++;
		fs.top->type = VAR_UNDEFINED;
		break;

	case OP_JumpOnFalse:
	case OP_JumpOnTrue:
	case OP_JumpOnFalseExpr:
	case OP_JumpOnTrueExpr:
		Scr_ReadUnsignedShort(&fs.pos);
		goto dec_top;

	case OP_notify:
		assert(fs.top->type != VAR_CODEPOS);
		while ( fs.top->type != VAR_PRECODEPOS )
		{
			RemoveRefToValue(fs.top);
			fs.top--;
		}
		goto remove_ref_dec_top;

	case OP_switch:
		while ( gCaseCount )
		{
			currentCaseValue = Scr_ReadUnsigned(&fs.pos);
			currentCodePos = Scr_ReadCodePos(&fs.pos);
			gCaseCount--;
		}
		if ( !currentCaseValue )
			fs.pos = currentCodePos;
		goto remove_ref_dec_top;
	}

	goto loop;
}

/*
==============
Scr_GetReturnPos
==============
*/
char* Scr_GetReturnPos( unsigned int *localId )
{
	char *pos;

	if ( scrVmPub.function_count > 1 )
	{
		pos = (char *)scrVmPub.function_frame[-1].fs.pos;

		if ( pos != &g_EndPos )
		{
			*localId = scrVmPub.function_frame[-1].fs.localId;
			return pos;
		}
	}

	return 0;
}

/*
==============
VM_CancelNotifyInternal
==============
*/
void VM_CancelNotifyInternal( unsigned int notifyListOwnerId, unsigned int startLocalId, unsigned int notifyListId, unsigned int notifyNameListId, unsigned int stringValue )
{
	assert(stringValue == Scr_GetThreadNotifyName( startLocalId ));
	assert(notifyListId == FindObject( FindVariable( notifyListOwnerId, OBJECT_NOTIFY_LIST ) ));
	assert(notifyNameListId == FindObject( FindVariable( notifyListId, stringValue ) ));

	Scr_RemoveThreadNotifyName(startLocalId);
	RemoveObjectVariable(notifyNameListId, startLocalId);

	if ( !GetArraySize(notifyNameListId) )
	{
		RemoveVariable(notifyListId, stringValue);

		if ( !GetArraySize(notifyListId) )
		{
			RemoveVariable(notifyListOwnerId, OBJECT_NOTIFY_LIST);
		}
	}
}

/*
==============
VM_CancelNotify
==============
*/
void VM_CancelNotify( unsigned int notifyListOwnerId, unsigned int startLocalId )
{
	unsigned int stringValue, notifyListId, notifyNameListId;

	notifyListId = FindObject( FindVariable( notifyListOwnerId, OBJECT_NOTIFY_LIST ) );
	stringValue = Scr_GetThreadNotifyName(startLocalId);
	assert(stringValue);
	notifyNameListId = FindObject( FindVariable( notifyListId, stringValue ) );

	VM_CancelNotifyInternal(notifyListOwnerId, startLocalId, notifyListId, notifyNameListId, stringValue);
}

/*
==============
VM_ArchiveStack
==============
*/
VariableStackBuffer* VM_ArchiveStack( int size, const char *pos, VariableValue *top, unsigned int localVarCount, unsigned int *localId )
{
	VariableStackBuffer *stackValue;
	char *buf;
	int bufLen;
	unsigned int id;

	assert(size == (unsigned short)size);
	bufLen = STACKBUF_BUFFER_SIZE * size + sizeof(*stackValue) - 1;
	assert(bufLen == (unsigned short)bufLen);

	stackValue = (VariableStackBuffer *)MT_Alloc(bufLen, 1);
	id = *localId;

	stackValue->localId = id;
	stackValue->size = size;
	stackValue->bufLen = bufLen;
	stackValue->pos = pos;
	stackValue->time = scrVarPub.time;

	scrVmPub.localVars -= localVarCount;
	buf = stackValue->buf;
	buf += STACKBUF_BUFFER_SIZE * size;

	while ( size )
	{
		buf -= sizeof(VariableUnion);

		if ( top->type == VAR_CODEPOS )
		{
			--scrVmPub.function_count;
			--scrVmPub.function_frame;

			*(const char **)buf = scrVmPub.function_frame->fs.pos;
			scrVmPub.localVars -= scrVmPub.function_frame->fs.localVarCount;

			id = GetParentLocalId(id);
		}
		else
		{
			*(const char **)buf = top->u.codePosValue;
		}

		buf -= sizeof(unsigned char);
		assert(top->type >= 0 && top->type < (1 << 8));
		*(unsigned char *)buf = top->type;

		top--;
		size--;
	}

	scrVmPub.function_count--;
	scrVmPub.function_frame--;

	AddRefToObject(id);
	*localId = id;

	return stackValue;
}

/*
==============
Scr_AddLocalVars
==============
*/
int Scr_AddLocalVars( unsigned int localId )
{
	unsigned int fieldIndex;
	int localVarCount = 0;

	for ( fieldIndex = FindPrevSibling(localId); fieldIndex; fieldIndex = FindPrevSibling(fieldIndex) )
	{
		*scrVmPub.localVars++;
		*scrVmPub.localVars = fieldIndex;

		localVarCount++;
	}

	return localVarCount;
}

/*
==============
VM_UnarchiveStack
==============
*/
void VM_UnarchiveStack( unsigned int startLocalId, function_stack_t *fs, VariableStackBuffer *stackValue )
{
	char *buf;
	int size;
	VariableValue *top;
	unsigned int localId;
	int function_count;

	assert(!scrVmPub.function_count);
	assert(stackValue->pos);
	assert(fs->startTop == &scrVmPub.stack[0]);

	scrVmPub.function_frame->fs.pos = stackValue->pos;

	scrVmPub.function_count++;
	scrVmPub.function_frame++;

	size = stackValue->size;
	buf = stackValue->buf;

	top = fs->startTop;

	while ( size )
	{
		top++;
		size--;

		top->type = *(unsigned char *)buf;
		buf += sizeof(unsigned char);

		if ( top->type == VAR_CODEPOS )
		{
			assert(scrVmPub.function_count < MAX_VM_STACK_DEPTH);
			scrVmPub.function_frame->fs.pos = *(const char **)buf;

			scrVmPub.function_count++;
			scrVmPub.function_frame++;
		}
		else
		{
			top->u.codePosValue = *(const char **)buf;
		}

		buf += sizeof(VariableUnion);
	}

	fs->pos = stackValue->pos;
	fs->top = top;

	localId = stackValue->localId;
	fs->localId = localId;
	Scr_ClearWaitTime(startLocalId);

	assert(scrVmPub.function_count < MAX_VM_STACK_DEPTH);
	function_count = scrVmPub.function_count;

	while ( 1 )
	{
		scrVmPub.function_frame_start[function_count].fs.localId = localId;
		function_count--;

		if ( !function_count )
		{
			break;
		}

		localId = GetParentLocalId(localId);
	}

	for ( ; ++function_count != scrVmPub.function_count; )
	{
		scrVmPub.function_frame_start[function_count].fs.localVarCount = Scr_AddLocalVars(scrVmPub.function_frame_start[function_count].fs.localId);
	}

	fs->localVarCount = Scr_AddLocalVars(fs->localId);

	// compare as unsigned char on both sides
	if ( stackValue->time != (const unsigned char)scrVarPub.time )
	{
		Scr_ResetTimeout();
	}

	MT_Free(stackValue, stackValue->bufLen);
	assert(scrVmPub.stack[0].type == VAR_CODEPOS);
}

/*
==============
VM_TerminateStack
==============
*/
void VM_TerminateStack( unsigned int endLocalId, unsigned int startLocalId, VariableStackBuffer *stackValue )
{
	unsigned int localId;
	unsigned int parentLocalId;
	VariableUnion value;
	char *buf;
	int size;
	unsigned char type;
	unsigned int stackId;
	VariableValue tempValue;

	assert(startLocalId);

	size = stackValue->size;
	localId = stackValue->localId;
	buf = stackValue->buf;
	buf += STACKBUF_BUFFER_SIZE * size;

	while ( size )
	{
		buf -= sizeof(VariableUnion);
		value.codePosValue = *(const char **)buf;

		buf -= sizeof(unsigned char);
		type = *(unsigned char *)buf;

		size--;

		if ( type != VAR_CODEPOS )
		{
			RemoveRefToValue(type, value);
			continue;
		}

		parentLocalId = GetParentLocalId(localId);

		Scr_KillThread(localId);
		RemoveRefToObject(localId);

		if ( localId != endLocalId )
		{
			localId = parentLocalId;
			continue;
		}

		assert(startLocalId != localId);
		size++;
		*buf = 0;
		assert(stackValue->size >= size);

		Scr_SetThreadWaitTime(startLocalId, scrVarPub.time);

		assert(value.codePosValue);
		stackValue->pos = value.codePosValue;
		stackValue->localId = parentLocalId;
		stackValue->size = size;

		tempValue.type = VAR_STACK;
		tempValue.u.stackValue = stackValue;

		stackId = GetNewObjectVariable(GetArray(GetVariable(scrVarPub.timeArrayId, scrVarPub.time)), startLocalId);

		SetNewVariableValue(stackId, &tempValue);
		return;
	}

	assert(localId == endLocalId);
	assert(startLocalId == localId);

	Scr_KillThread(localId);
	RemoveRefToObject(localId);

	MT_Free(stackValue, stackValue->bufLen);
}

void VM_TrimStack( unsigned int startLocalId, VariableStackBuffer *stackValue, bool fromEndon )
{
	unsigned int localId;
	unsigned int parentLocalId;
	VariableUnion u;
	char *buf;
	int size;
	unsigned char type;
	VariableValue tempValue;

	size = stackValue->size;
	localId = stackValue->localId;
	buf = stackValue->buf;
	buf += 5 * size;

	while ( size )
	{
		buf -= sizeof(VariableUnion);
		u.codePosValue = *(const char **)buf;
		buf--;
		type = *(unsigned char *)buf;
		size--;

		if ( type != VAR_CODEPOS )
		{
			RemoveRefToValue(type, u);
			continue;
		}

		if ( FindObjectVariable(scrVarPub.pauseArrayId, localId) )
		{
			size++;
			stackValue->localId = localId;
			stackValue->size = size;
			Scr_StopThread(localId);

			if ( fromEndon )
			{
				return;
			}

			Scr_SetThreadNotifyName(startLocalId, 0);
			stackValue->pos = 0;
			tempValue.type = VAR_STACK;
			tempValue.u.stackValue = stackValue;
			SetNewVariableValue(GetNewVariable(startLocalId, OBJECT_STACK), &tempValue);
			return;
		}

		parentLocalId = GetParentLocalId(localId);
		Scr_KillThread(localId);
		RemoveRefToObject(localId);
		localId = parentLocalId;
	}

	if ( fromEndon )
	{
		RemoveVariable(startLocalId, OBJECT_STACK);
	}

	Scr_KillThread(startLocalId);
	RemoveRefToObject(startLocalId);
	MT_Free(stackValue, stackValue->bufLen);
}

/*
==============
Scr_TerminateRunningThread
==============
*/
void Scr_TerminateRunningThread( unsigned int localId )
{
	int function_count, topThread, threadId;

	function_count = scrVmPub.function_count;
	topThread = function_count;

	while ( 1 )
	{
		threadId = scrVmPub.function_frame_start[function_count].fs.localId;

		if ( threadId == localId )
		{
			while ( topThread >= function_count )
			{
				scrVmPub.function_frame_start[topThread].fs.pos = &g_EndPos;
				topThread--;
			}

			return;
		}

		function_count--;

		if ( !GetSafeParentLocalId(threadId) )
		{
			topThread = function_count;
		}
	}
}

/*
==============
Scr_TerminateWaitThread
==============
*/
void Scr_TerminateWaitThread( unsigned int localId, unsigned int startLocalId )
{
	unsigned int id, iTime, stackId;
	VariableStackBuffer *stackValue;

	iTime = Scr_GetThreadWaitTime(startLocalId);
	Scr_ClearWaitTime(startLocalId);

	id = FindObject(FindVariable(scrVarPub.timeArrayId, iTime));
	stackId = FindObjectVariable(id, startLocalId);

	assert(stackId);
	assert(GetObjectType( stackId ) == VAR_STACK);

	stackValue = GetVariableValueAddress(stackId)->stackValue;

	RemoveObjectVariable(id, startLocalId);

	if ( !GetArraySize(id) && iTime != scrVarPub.time )
	{
		RemoveVariable(scrVarPub.timeArrayId, iTime);
	}

	VM_TerminateStack(localId, startLocalId, stackValue);
}

/*
==============
Scr_CancelWaittill
==============
*/
void Scr_CancelWaittill( unsigned int startLocalId )
{
	unsigned int selfId;
	unsigned int selfNameId;
	unsigned int stackId;

	selfId = Scr_GetSelf(startLocalId);
	selfNameId = FindObject(FindObjectVariable(scrVarPub.pauseArrayId, selfId));
	stackId = GetVariableValueAddress(FindObjectVariable(selfNameId, startLocalId))->pointerValue;
	VM_CancelNotify(stackId, startLocalId);
	RemoveObjectVariable(selfNameId, startLocalId);

	if ( !GetArraySize(selfNameId) )
	{
		RemoveObjectVariable(scrVarPub.pauseArrayId, selfId);
	}
}

/*
==============
Scr_TerminateWaittillThread
==============
*/
void Scr_TerminateWaittillThread( unsigned int localId, unsigned int startLocalId )
{
	unsigned int selfId, selfNameId, stringValue, notifyListOwnerId, notifyNameListId, notifyListId, stackId;
	VariableStackBuffer *stackValue;

	stringValue = Scr_GetThreadNotifyName(startLocalId);

	if ( stringValue )
	{
		selfId = Scr_GetSelf(startLocalId);
		selfNameId = FindObject(FindObjectVariable(scrVarPub.pauseArrayId, selfId));

		notifyListOwnerId = GetVariableValueAddress(FindObjectVariable(selfNameId, startLocalId))->pointerValue;
		notifyListId = FindObject(FindVariable(notifyListOwnerId, OBJECT_NOTIFY_LIST));
		notifyNameListId = FindObject(FindVariable(notifyListId, stringValue));

		stackId = FindObjectVariable(notifyNameListId, startLocalId);

		assert(stackId);
		assert(GetObjectType( stackId ) == VAR_STACK);

		stackValue = GetVariableValueAddress(stackId)->stackValue;

		VM_CancelNotifyInternal(notifyListOwnerId, startLocalId, notifyListId, notifyNameListId, stringValue);
		RemoveObjectVariable(selfNameId, startLocalId);

		if ( !GetArraySize(selfNameId) )
		{
			RemoveObjectVariable(scrVarPub.pauseArrayId, selfId);
		}
	}
	else
	{
		stackId = FindVariable(startLocalId, OBJECT_STACK);

		assert(stackId);
		assert(GetObjectType( stackId ) == VAR_STACK);

		stackValue = GetVariableValueAddress(stackId)->stackValue;

		RemoveVariable(startLocalId, OBJECT_STACK);
	}

	VM_TerminateStack(localId, startLocalId, stackValue);
}

/*
==============
Scr_GetWaittillThreadStackId
==============
*/
unsigned int Scr_GetWaittillThreadStackId( unsigned int localId, unsigned int startLocalId )
{
	unsigned int selfId;
	unsigned int selfNameId;
	unsigned int notifyName;
	unsigned int stackId;
	unsigned int notifyNameListId;
	unsigned int notifyListId;

	notifyName = Scr_GetThreadNotifyName(startLocalId);

	if ( notifyName )
	{
		selfId = Scr_GetSelf(startLocalId);
		selfNameId = FindObject(FindObjectVariable(scrVarPub.pauseArrayId, selfId));
		stackId = GetVariableValueAddress(FindObjectVariable(selfNameId, startLocalId))->pointerValue;
		notifyListId = FindObject(FindVariable(stackId, OBJECT_NOTIFY_LIST));
		notifyNameListId = FindObject(FindVariable(notifyListId, notifyName));
		return FindObjectVariable(notifyNameListId, startLocalId);
	}

	return FindVariable(startLocalId, OBJECT_STACK);
}

/*
==============
Scr_IsEndonThread
==============
*/
bool Scr_IsEndonThread( unsigned int localId )
{
	unsigned int stackId;
	int type;

	if ( GetObjectType(localId) != VAR_NOTIFY_THREAD )
	{
		return false;
	}

	if ( GetStartLocalId(localId) != localId )
	{
		return false;
	}

	stackId = Scr_GetWaittillThreadStackId(localId, localId);
	type = GetObjectType(stackId);

	assert((type == VAR_UNDEFINED) || (type == VAR_STACK));
	return type == VAR_UNDEFINED;
}

/*
==============
Scr_NotifyNum
==============
*/
void Scr_NotifyNum( int entnum, int classnum, unsigned int stringValue, unsigned int paramcount )
{
	unsigned int id;
	VariableValue *startTop;
	int type;

	assert(scrVarPub.timeArrayId);
	assert(paramcount <= scrVmPub.inparamcount);

	Scr_ClearOutParams();

	startTop = scrVmPub.top - paramcount;
	paramcount = scrVmPub.inparamcount - paramcount;

	id = FindEntityId(entnum, classnum);

	if ( id )
	{
		type = startTop->type;

		startTop->type = VAR_PRECODEPOS;
		scrVmPub.inparamcount = 0;

		VM_Notify(id, stringValue, scrVmPub.top);

		startTop->type = type;
	}

	while ( scrVmPub.top != startTop )
	{
		RemoveRefToValue(scrVmPub.top);
		scrVmPub.top--;
	}

	assert(!scrVmPub.outparamcount);
	scrVmPub.inparamcount = paramcount;
}

/*
==============
Scr_CancelNotifyList
==============
*/
void Scr_CancelNotifyList( unsigned int notifyListOwnerId )
{
	unsigned int stackId, notifyNameListId, notifyListId, startLocalId;
	VariableStackBuffer *stackValue;
	unsigned int selfLocalId, selfStartLocalId;

	while ( 1 )
	{
		notifyListId = FindVariable(notifyListOwnerId, OBJECT_NOTIFY_LIST);

		if ( !notifyListId )
		{
			break;
		}

		notifyListId = FindObject(notifyListId);
		notifyNameListId = FindNextSibling(notifyListId);

		if ( !notifyNameListId )
		{
			break;
		}

		notifyNameListId = FindObject(notifyNameListId);
		stackId = FindNextSibling(notifyNameListId);

		if ( !stackId )
		{
			break;
		}

		startLocalId = GetVariableKeyObject(stackId);
		assert(startLocalId);

		if ( GetObjectType(stackId) == VAR_STACK )
		{
			stackValue = GetVariableValueAddress(stackId)->stackValue;
			Scr_CancelWaittill(startLocalId);

			VM_TrimStack(startLocalId, stackValue, false);
			continue;
		}

		AddRefToObject(startLocalId);
		Scr_CancelWaittill(startLocalId);

		selfLocalId = Scr_GetSelf(startLocalId);
		selfStartLocalId = GetStartLocalId(selfLocalId);

		stackId = FindVariable(selfStartLocalId, OBJECT_STACK);

		if ( stackId )
		{
			stackValue = GetVariableValueAddress(stackId)->stackValue;

			assert(!Scr_GetThreadNotifyName( selfStartLocalId ));
			assert(GetObjectType( stackId ) == VAR_STACK);
			assert(!stackValue->pos);

			VM_TrimStack(selfStartLocalId, stackValue, true);
		}

		Scr_KillEndonThread(startLocalId);
		RemoveRefToEmptyObject(startLocalId);
	}
}

/*
==============
VM_TerminateTime
==============
*/
void VM_TerminateTime( unsigned int timeId )
{
	unsigned int stackId, startLocalId;
	VariableStackBuffer *stackValue;

	assert(timeId);
	assert(!scrVmPub.function_count);

	AddRefToObject(timeId);

	while ( 1 )
	{
		stackId = FindNextSibling(timeId);

		if ( !stackId )
		{
			break;
		}

		startLocalId = GetVariableKeyObject(stackId);

		assert(startLocalId);
		assert(GetObjectType( stackId ) == VAR_STACK);

		stackValue = GetVariableValueAddress(stackId)->stackValue;

		RemoveObjectVariable(timeId, startLocalId);
		Scr_ClearWaitTime(startLocalId);

		VM_TerminateStack(startLocalId, startLocalId, stackValue);
	}

	RemoveRefToObject(timeId);
}

void VM_Resume( unsigned int timeId )
{
	function_stack_t fs;
	unsigned int stackId, startLocalId;
	VariableStackBuffer *stackValue;

	assert(scrVmPub.top == scrVmPub.stack);
	Scr_ResetTimeout();

	assert(timeId);
	AddRefToObject(timeId);

	for ( fs.startTop = scrVmPub.stack; ; RemoveRefToValue(fs.startTop + 1) )
	{
		assert(!scrVarPub.error_index);
		assert(!scrVmPub.outparamcount);
		assert(!scrVmPub.inparamcount);
		assert(!scrVmPub.function_count);
		assert(scrVmPub.localVars == scrVmGlob.localVarsStack - 1);
		assert(fs.startTop == &scrVmPub.stack[0]);

		stackId = FindNextSibling(timeId);

		if ( !stackId )
		{
			break;
		}

		startLocalId = GetVariableKeyObject(stackId);

		assert(startLocalId);
		assert(GetObjectType( stackId ) == VAR_STACK);

		stackValue = GetVariableValueAddress(stackId)->stackValue;
		RemoveObjectVariable(timeId, startLocalId);

		VM_UnarchiveStack( startLocalId, &fs, stackValue );
		RemoveRefToObject( VM_ExecuteInternal( fs ) );
	}

	RemoveRefToObject(timeId);
	ClearVariableValue(scrVarPub.tempVariable);

	scrVmPub.top = scrVmPub.stack;
}

/*
==============
IncInParam
==============
*/
void IncInParam()
{
	assert(((scrVmPub.top >= scrVmGlob.eval_stack - 1) && (scrVmPub.top <= scrVmGlob.eval_stack)) || ((scrVmPub.top >= scrVmPub.stack) && (scrVmPub.top <= scrVmPub.maxstack)));

	Scr_ClearOutParams();

	if ( scrVmPub.top == scrVmPub.maxstack )
	{
		Com_Error(ERR_DROP, "\x15" "Internal script stack overflow");
	}

	scrVmPub.top++;
	scrVmPub.inparamcount++;

	assert(((scrVmPub.top >= scrVmGlob.eval_stack) && (scrVmPub.top <= scrVmGlob.eval_stack + 1)) || ((scrVmPub.top >= scrVmPub.stack) && (scrVmPub.top <= scrVmPub.maxstack)));
}

unsigned int VM_Execute( unsigned int localId, const char *pos, unsigned int paramcount )
{
	function_stack_t fs;
	int type;

	fs.localId = localId;

	Scr_ClearOutParams();

	fs.startTop = scrVmPub.top - paramcount;
	paramcount = scrVmPub.inparamcount - paramcount;

	if ( scrVmPub.function_count < MAX_EMBEDDED_FUNCTION_CALLS )
	{
		if ( scrVmPub.function_count )
		{
			scrVmPub.function_count++;
			scrVmPub.function_frame++;

			scrVmPub.function_frame->fs.localId = 0;
		}

		scrVmPub.function_frame->fs.pos = pos;

		scrVmPub.function_count++;
		scrVmPub.function_frame++;

		scrVmPub.function_frame->fs.localId = fs.localId;

		type = fs.startTop->type;
		fs.startTop->type = VAR_PRECODEPOS;

		scrVmPub.inparamcount = 0;

		fs.top = scrVmPub.top;
		fs.pos = pos;
		fs.localVarCount = 0;

		fs.localId = VM_ExecuteInternal( fs );

		fs.startTop->type = type;
		scrVmPub.top = fs.startTop + 1;

		scrVmPub.inparamcount = paramcount + 1;

		ClearVariableValue(scrVarPub.tempVariable);

		if ( scrVmPub.function_count )
		{
			scrVmPub.function_count--;
			scrVmPub.function_frame--;
		}

		return fs.localId;
	}

	Scr_KillThread(fs.localId);
	scrVmPub.inparamcount = paramcount + 1;

	while ( paramcount )
	{
		RemoveRefToValue(scrVmPub.top);

		scrVmPub.top--;
		paramcount--;
	}

	scrVmPub.top++;
	scrVmPub.top->type = VAR_UNDEFINED;

	RuntimeError(pos, 0, "script stack overflow (too many embedded function calls)", NULL);

	return fs.localId;
}

/*
==============
Scr_ExecThread
==============
*/
unsigned short Scr_ExecThread( int handle, unsigned int paramcount )
{
	unsigned int id;
	const char *pos;

	pos = &scrVarPub.programBuffer[handle];

	if ( !scrVmPub.function_count )
	{
		Scr_ResetTimeout();
	}

	Scr_IsInOpcodeMemory( pos );

	AddRefToObject(scrVarPub.levelId);

	id = VM_Execute(AllocThread(scrVarPub.levelId), pos, paramcount);

	RemoveRefToValue(scrVmPub.top);
	scrVmPub.top->type = VAR_UNDEFINED;

	scrVmPub.top--;
	scrVmPub.inparamcount--;

	assert(scrVmPub.localVars == scrVmGlob.localVarsStack - 1);

	return id;
}

/*
==============
Scr_ExecEntThreadNum
==============
*/
unsigned short Scr_ExecEntThreadNum( int entnum, int classnum, int handle, unsigned int paramcount )
{
	unsigned int id, selfId;
	const char *pos;

	pos = &scrVarPub.programBuffer[handle];

	if ( !scrVmPub.function_count )
	{
		Scr_ResetTimeout();
	}

	selfId = Scr_GetEntityId(entnum, classnum);
	AddRefToObject(selfId);

	id = VM_Execute(AllocThread(selfId), pos, paramcount);

	RemoveRefToValue(scrVmPub.top);
	scrVmPub.top->type = VAR_UNDEFINED;

	scrVmPub.top--;
	scrVmPub.inparamcount--;

	//assert(scrVmPub.localVars == scrVmGlob.localVarsStack - 1);

	return id;
}

/*
==============
Scr_AddExecThread
==============
*/
void Scr_AddExecThread( int handle, unsigned int paramcount )
{
	const char *pos;
	unsigned int register id, localId;

	pos = &scrVarPub.programBuffer[handle];

	if ( !scrVmPub.function_count )
	{
		assert(scrVmPub.localVars == scrVmGlob.localVarsStack - 1);
		Scr_ResetTimeout();
	}

	assert(scrVarPub.timeArrayId);
	assert(handle);
	assert(Scr_IsInScriptMemory( pos ));

	AddRefToObject(scrVarPub.levelId);

	localId = AllocThread(scrVarPub.levelId);
	id = VM_Execute(localId, pos, paramcount);

	RemoveRefToObject(id);

	scrVmPub.outparamcount++;
	scrVmPub.inparamcount--;

	assert(scrVmPub.localVars == scrVmGlob.localVarsStack - 1);
}

/*
==============
Scr_AddExecEntThreadNum
==============
*/
void Scr_AddExecEntThreadNum( int entnum, int classnum, int handle, unsigned int paramcount )
{
	const char *pos;
	unsigned int selfId;

	pos = &scrVarPub.programBuffer[handle];

	if ( !scrVmPub.function_count )
	{
		Scr_ResetTimeout();
	}

	selfId = Scr_GetEntityId(entnum, classnum);
	AddRefToObject(selfId);

	RemoveRefToObject(VM_Execute(AllocThread(selfId), pos, paramcount));

	scrVmPub.outparamcount++;
	scrVmPub.inparamcount--;

	assert(scrVmPub.localVars == scrVmGlob.localVarsStack - 1);
}

/*
==============
Scr_FreeThread
==============
*/
void Scr_FreeThread( unsigned short handle )
{
	assert(scrVarPub.timeArrayId);
	assert(handle);

	RemoveRefToObject(handle);
}

/*
==============
Scr_ExecCode
==============
*/
void Scr_ExecCode( const char *pos, unsigned int localId )
{
	Scr_ResetTimeout();

	assert(scrVarPub.timeArrayId);
	assert(!scrVmPub.inparamcount);
	assert(!scrVmPub.outparamcount);
	assert(!scrVarPub.evaluate);
	assert(!scrVmPub.debugCode);

	scrVmPub.debugCode = true;

	if ( localId )
	{
		VM_Execute(localId, pos, 0);
	}
	else
	{
		AddRefToObject(scrVarPub.levelId);
		localId = AllocThread(scrVarPub.levelId);

		VM_Execute(localId, pos, 0);

		Scr_KillThread(localId);
		RemoveRefToObject(localId);
	}

	assert(scrVmPub.debugCode);

	scrVmPub.debugCode = false;

	assert(scrVmPub.inparamcount == 1);
	assert(!scrVmPub.outparamcount);

	if ( scrVmPub.function_count )
	{
		scrVmPub.function_count--;
		scrVmPub.function_frame--;
	}

	scrVmPub.top--;
	scrVmPub.inparamcount = 0;
}

/*
==============
VM_SetTime
==============
*/
void VM_SetTime()
{
	unsigned int id;

	assert(!(scrVarPub.time & ~VAR_NAME_LOW_MASK));

	if ( !scrVarPub.timeArrayId )
	{
		return;
	}

	id = FindVariable(scrVarPub.timeArrayId, scrVarPub.time);

	if ( !id )
	{
		return;
	}

	VM_Resume(FindObject(id));
	SafeRemoveVariable(scrVarPub.timeArrayId, scrVarPub.time);
}

/*
==============
Scr_FindAllThreadsInternal
==============
*/
int Scr_FindAllThreadsInternal( unsigned int selfId, unsigned int threadId, int count, bool a4, unsigned int *threads )
{
	unsigned int stackId;
	unsigned int localId;
	unsigned int id;
	VariableStackBuffer *stackValue;

	id = FindObject(threadId);

	for ( stackId = FindNextSibling(id); stackId; stackId = FindNextSibling(stackId) )
	{
		if ( GetObjectType(stackId) != VAR_STACK )
		{
			continue;
		}

		stackValue = GetVariableValueAddress(stackId)->stackValue;

		for ( localId = stackValue->localId; localId; localId = GetSafeParentLocalId(localId) )
		{
			if ( a4 && selfId != Scr_GetSelf(localId) )
			{
				continue;
			}

			if ( threads )
			{
				threads[count] = localId;
			}

			count++;
			break;
		}
	}

	return count;
}

int Scr_FindAllThreads( unsigned int selfId, unsigned int *threads, unsigned int localId )
{
	unsigned int threadId;
	unsigned int id;
	int count;

	count = 0;

	if ( localId && selfId == Scr_GetSelf(localId) )
	{
		if ( threads )
			threads[count] = localId;

		count++;
	}

	for ( threadId = FindNextSibling(scrVarPub.timeArrayId); threadId; threadId = FindNextSibling(threadId) )
		count = Scr_FindAllThreadsInternal(selfId, threadId, count, true, threads);

	id = FindVariable(selfId, OBJECT_NOTIFY_LIST);

	if ( id )
	{
		id = FindObject(id);

		for ( id = FindNextSibling(id); id; id = FindNextSibling(id) )
			count = Scr_FindAllThreadsInternal(selfId, id, count, false, threads);
	}

	return count;
}

/*
==============
Scr_InitSystem
==============
*/
void Scr_InitSystem( int sys )
{
	assert(!scrVarPub.timeArrayId);
	scrVarPub.timeArrayId = AllocObject();

	assert(!scrVarPub.pauseArrayId);
	scrVarPub.pauseArrayId = Scr_AllocArray();

	assert(!scrVarPub.levelId);
	scrVarPub.levelId = AllocObject();

	assert(!scrVarPub.animId);
	scrVarPub.animId = AllocObject();

	scrVarPub.time = 0;
	g_script_error_level = -1;
}

/*
==============
Scr_ShutdownSystem
==============
*/
void Scr_ShutdownSystem( unsigned char sys, int bComplete )
{
	unsigned int id, parentId;

	Scr_CompileShutdown();
	Scr_FreeEntityList();

	if ( !scrVarPub.timeArrayId )
	{
		return;
	}

	Scr_FreeGameVariable(bComplete);

	for ( id = FindNextSibling(scrVarPub.timeArrayId); id; id = FindNextSibling(id) )
	{
		VM_TerminateTime(FindObject(id));
	}

	for ( ; ; )
	{
		id = FindNextSibling(scrVarPub.pauseArrayId);

		if ( !id )
		{
			break;
		}

		id = FindNextSibling(FindObject(id));
		assert(id);

		parentId = GetVariableValueAddress(id)->pointerValue;

		AddRefToObject(parentId);
		Scr_CancelNotifyList(parentId);
		RemoveRefToObject(parentId);
	}

	assert(scrVarPub.levelId);
	ClearObject(scrVarPub.levelId);
	RemoveRefToEmptyObject(scrVarPub.levelId);
	scrVarPub.levelId = 0;

	assert(scrVarPub.animId);
	ClearObject(scrVarPub.animId);
	RemoveRefToEmptyObject(scrVarPub.animId);
	scrVarPub.animId = 0;

	assert(scrVarPub.timeArrayId);
	ClearObject(scrVarPub.timeArrayId);
	RemoveRefToEmptyObject(scrVarPub.timeArrayId);
	scrVarPub.timeArrayId = 0;

	assert(scrVarPub.pauseArrayId);
	RemoveRefToEmptyObject(scrVarPub.pauseArrayId);
	scrVarPub.pauseArrayId = 0;

	assert(!scrVarPub.freeEntList);
	Scr_FreeObjects();
}

/*
==============
Scr_GetInt
==============
*/
int Scr_GetInt( unsigned int index )
{
	VariableValue *value;

	if ( index < scrVmPub.outparamcount )
	{
		value = scrVmPub.top - index;

		if ( value->type == VAR_INTEGER )
		{
			return value->u.intValue;
		}

		scrVarPub.error_index = index + 1;
		Scr_Error(va("type %s is not an int", var_typename[value->type]));
	}

	Scr_Error(va("parameter %d does not exist", index + 1));
	return 0;
}

/*
==============
Scr_GetAnim
==============
*/
scr_anim_s Scr_GetAnim( unsigned int index, XAnimTree_s *tree )
{
	VariableValue *value;
	const char *linkPointer;
	scr_anim_s anim;

	if ( index < scrVmPub.outparamcount )
	{
		value = scrVmPub.top - index;

		if ( value->type == VAR_ANIMATION )
		{
			linkPointer = value->u.codePosValue;
			anim.linkPointer = linkPointer;

			if ( tree && Scr_GetAnims(anim.tree) != XAnimGetAnims(tree) )
			{
				scrVarPub.error_message = va("anim '%s' in animtree '%s' does not belong to the entity's animtree '%s'",
				                             XAnimGetAnimDebugName(Scr_GetAnims(anim.tree), anim.index),
				                             XAnimGetAnimTreeDebugName(Scr_GetAnims(anim.tree)),
				                             XAnimGetAnimTreeDebugName(XAnimGetAnims(tree)));

				goto cleanup;
			}

			return anim;
		}

		scrVarPub.error_message = va("type %s is not an anim", var_typename[value->type]);

cleanup:
		RemoveRefToValue(value);
		value->type = VAR_UNDEFINED;

		scrVarPub.error_index = index + 1;
		Scr_ErrorInternal();
	}

error:
	Scr_Error(va("parameter %d does not exist", index + 1));

	anim.index = 0;
	anim.tree = 0;

	return anim;
}

/*
==============
Scr_GetAnimTree
==============
*/
scr_animtree_t Scr_GetAnimTree( unsigned int index ) // untested
{
	VariableValue *value;
	if ( index < scrVmPub.outparamcount )
	{
		value = scrVmPub.top - index;
		if ( value->type == VAR_INTEGER )
		{
			if ( value->u.intValue <= scrAnimPub.xanim_num[SCR_XANIM_SERVER] )
			{
				if ( scrAnimPub.xanim_lookup[SCR_XANIM_SERVER][value->u.intValue].anims )
					return scrAnimPub.xanim_lookup[SCR_XANIM_SERVER][value->u.intValue];
			}
			scrVarPub.error_message = "bad anim tree";
		}
		else
		{
			scrVarPub.error_message = va("type %s is not an animtree", var_typename[value->type]);
		}
		RemoveRefToValue(value);
		value->type = VAR_UNDEFINED;
		scrVarPub.error_index = index + 1;
		Scr_ErrorInternal();
	}
	Scr_Error(va("parameter %d does not exist", index + 1));
	return scrAnimPub.xanim_lookup[SCR_XANIM_SERVER][0];
}

/*
==============
Scr_GetFloat
==============
*/
float Scr_GetFloat( unsigned int index )
{
	VariableValue *value;

	if ( index < scrVmPub.outparamcount )
	{
		value = scrVmPub.top - index;

		if ( value->type == VAR_FLOAT )
		{
			return value->u.floatValue;
		}

		if ( value->type == VAR_INTEGER )
		{
			return (float)value->u.intValue;
		}

		scrVarPub.error_index = index + 1;
		Scr_Error(va("type %s is not a float", var_typename[value->type]));
	}

	Scr_Error(va("parameter %d does not exist", index + 1));
	return 0;
}

/*
==============
Scr_GetConstString
==============
*/
unsigned int Scr_GetConstString( unsigned int index )
{
	VariableValue *value;

	if ( index < scrVmPub.outparamcount )
	{
		value = scrVmPub.top - index;

		if ( Scr_CastString(value) )
		{
			return value->u.stringValue;
		}

		scrVarPub.error_index = index + 1;
		Scr_ErrorInternal();
	}

	Scr_Error(va("parameter %d does not exist", index + 1));
	return 0;
}

/*
==============
Scr_GetConstLowercaseString
==============
*/
unsigned int Scr_GetConstLowercaseString( unsigned int index )
{
	char tempString[8192];
	int i;
	const char *string;
	VariableValue *value;
	unsigned int stringValue;

	if ( index < scrVmPub.outparamcount )
	{
		value = scrVmPub.top - index;

		if ( Scr_CastString(value) )
		{
			stringValue = value->u.stringValue;
			string = SL_ConvertToString(stringValue);

			for ( i = 0; ; i++ )
			{
				tempString[i] = tolower(string[i]);

				if ( !string[i] )
				{
					break;
				}
			}

			value->u.stringValue = SL_GetString(tempString, 0);
			SL_RemoveRefToString(stringValue);

			return value->u.stringValue;
		}

		scrVarPub.error_index = index + 1;
		Scr_ErrorInternal();
	}

	Scr_Error(va("parameter %d does not exist", index + 1));
	return 0;
}

/*
==============
Scr_GetString
==============
*/
const char* Scr_GetString( unsigned int index )
{
	return SL_ConvertToString( Scr_GetConstString( index ) );
}

/*
==============
Scr_GetConstStringIncludeNull
==============
*/
unsigned int Scr_GetConstStringIncludeNull( unsigned int index )
{
	if ( index < scrVmPub.outparamcount && (scrVmPub.top - index)->type == VAR_UNDEFINED )
	{
		return 0;
	}

	return Scr_GetConstString(index);
}

/*
==============
Scr_GetDebugString
==============
*/
const char* Scr_GetDebugString( unsigned int index )
{
	VariableValue *value;

	if ( index < scrVmPub.outparamcount )
	{
		value = scrVmPub.top - index;
		Scr_CastDebugString(value);
		return SL_ConvertToString(value->u.stringValue);
	}

	Scr_Error(va("parameter %d does not exist", index + 1));
	return 0;
}

/*
==============
Scr_GetConstIString
==============
*/
unsigned int Scr_GetConstIString( unsigned int index )
{
	VariableValue *value;

	if ( index < scrVmPub.outparamcount )
	{
		value = scrVmPub.top - index;

		if ( value->type == VAR_ISTRING )
		{
			return value->u.stringValue;
		}

		scrVarPub.error_index = index + 1;
		Scr_Error(va("type %s is not a localized string", var_typename[value->type]));
	}

	Scr_Error(va("parameter %d does not exist", index + 1));
	return 0;
}

/*
==============
Scr_GetVector
==============
*/
void Scr_GetVector( unsigned int index, vec3_t vectorValue )
{
	VariableValue *value;

	if ( index < scrVmPub.outparamcount )
	{
		value = scrVmPub.top - index;

		if ( value->type == VAR_VECTOR )
		{
			VectorCopy(value->u.vectorValue, vectorValue);
			return;
		}

		scrVarPub.error_index = index + 1;
		Scr_Error(va("type %s is not a vector", var_typename[value->type]));
	}

	Scr_Error(va("parameter %d does not exist", index + 1));
}

/*
==============
Scr_GetFunc
==============
*/
unsigned int Scr_GetFunc( unsigned int index )
{
	VariableValue *value;

	if ( index < scrVmPub.outparamcount )
	{
		value = scrVmPub.top - index;

		if ( value->type == VAR_FUNCTION )
		{
			return value->u.codePosValue - scrVarPub.programBuffer;
		}

		scrVarPub.error_index = index + 1;
		Scr_Error(va("type %s is not a function", var_typename[value->type]));
	}

	Scr_Error(va("parameter %d does not exist", index + 1));
	return 0;
}

/*
==============
Scr_GetEntityRef
==============
*/
scr_entref_t Scr_GetEntityRef( unsigned int index )
{
	unsigned int id;
	scr_entref_t entref;
	VariableValue *value;

	if ( index < scrVmPub.outparamcount )
	{
		value = scrVmPub.top - index;

		if ( value->type == VAR_POINTER )
		{
			id = value->u.pointerValue;

			if ( GetObjectType(id) == VAR_ENTITY )
			{
				return Scr_GetEntityIdRef(id);
			}

			scrVarPub.error_index = index + 1;
			Scr_Error(va("type %s is not an entity", var_typename[GetObjectType(id)]));
		}

		scrVarPub.error_index = index + 1;
		Scr_Error(va("type %s is not an entity", var_typename[value->type]));
	}

	Scr_Error(va("parameter %d does not exist", index + 1));

	entref.classnum = 0;
	entref.entnum = 0;

	return entref;
}

/*
==============
Scr_GetObject
==============
*/
unsigned int Scr_GetObject( unsigned int index )
{
	VariableValue *value;

	if ( index < scrVmPub.outparamcount )
	{
		value = scrVmPub.top - index;

		if ( value->type == VAR_POINTER )
		{
			return value->u.pointerValue;
		}

		scrVarPub.error_index = index + 1;
		Scr_Error(va("type %s is not an object", var_typename[value->type]));
	}

	Scr_Error(va("parameter %d does not exist", index + 1));
	return 0;
}

/*
==============
Scr_GetType
==============
*/
int Scr_GetType( unsigned int index )
{
	if ( index < scrVmPub.outparamcount )
	{
		return (scrVmPub.top - index)->type;
	}

	Scr_Error(va("parameter %d does not exist", index + 1));
	return 0;
}

/*
==============
Scr_GetTypeName
==============
*/
const char* Scr_GetTypeName( unsigned int index )
{
	if ( index < scrVmPub.outparamcount )
	{
		return var_typename[(scrVmPub.top - index)->type];
	}

	Scr_Error(va("parameter %d does not exist", index + 1));
	return NULL;
}

/*
==============
Scr_GetPointerType
==============
*/
int Scr_GetPointerType( unsigned int index )
{
	if ( index < scrVmPub.outparamcount )
	{
		if ( (scrVmPub.top - index)->type == VAR_POINTER )
		{
			return GetObjectType((scrVmPub.top - index)->u.pointerValue);
		}

		Scr_Error(va("type %s is not a pointer", var_typename[(scrVmPub.top - index)->type]));
	}

	Scr_Error(va("parameter %d does not exist", index + 1));
	return 0;
}

/*
==============
Scr_GetNumParam
==============
*/
unsigned int Scr_GetNumParam()
{
	return scrVmPub.outparamcount;
}

/*
==============
Scr_AddFloat
==============
*/
void Scr_AddFloat( float value )
{
	IncInParam();

	scrVmPub.top->type = VAR_FLOAT;
	scrVmPub.top->u.floatValue = value;
}

/*
==============
Scr_AddAnim
==============
*/
void Scr_AddAnim( scr_anim_s value )
{
	IncInParam();

	scrVmPub.top->type = VAR_ANIMATION;
	scrVmPub.top->u.codePosValue = value.linkPointer;
}

/*
==============
Scr_AddUndefined
==============
*/
void Scr_AddUndefined()
{
	IncInParam();
	scrVmPub.top->type = VAR_UNDEFINED;
}

/*
==============
Scr_AddObject
==============
*/
void Scr_AddObject( unsigned int id )
{
	assert(id);
	assert(GetObjectType( id ) != VAR_THREAD);
	assert(GetObjectType( id ) != VAR_NOTIFY_THREAD);
	assert(GetObjectType( id ) != VAR_TIME_THREAD);
	assert(GetObjectType( id ) != VAR_CHILD_THREAD);
	assert(GetObjectType( id ) != VAR_DEAD_THREAD);

	IncInParam();

	scrVmPub.top->type = VAR_POINTER;
	scrVmPub.top->u.pointerValue = id;

	AddRefToObject(id);
}

/*
==============
Scr_AddEntityNum
==============
*/
void Scr_AddEntityNum( int entnum, int classnum )
{
	Scr_AddObject( Scr_GetEntityId( entnum, classnum ) );
}

/*
==============
Scr_AddStruct
==============
*/
void Scr_AddStruct()
{
	unsigned int id = AllocObject();

	Scr_AddObject(id);
	RemoveRefToObject(id);
}

/*
==============
Scr_AddString
==============
*/
void Scr_AddString( const char *value )
{
	assert(value);
	IncInParam();

	scrVmPub.top->type = VAR_STRING;
	scrVmPub.top->u.stringValue = SL_GetString(value, 0);
}

/*
==============
Scr_AddIString
==============
*/
void Scr_AddIString( const char *value )
{
	assert(value);
	IncInParam();

	scrVmPub.top->type = VAR_ISTRING;
	scrVmPub.top->u.stringValue = SL_GetString(value, 0);
}

/*
==============
Scr_AddConstString
==============
*/
void Scr_AddConstString( unsigned int value )
{
	assert(value);
	IncInParam();

	scrVmPub.top->type = VAR_STRING;
	scrVmPub.top->u.stringValue = value;

	SL_AddRefToString(value);
}

/*
==============
Scr_AddVector
==============
*/
void Scr_AddVector( const vec3_t value )
{
	IncInParam();

	scrVmPub.top->type = VAR_VECTOR;
	scrVmPub.top->u.vectorValue = Scr_AllocVector(value);
}

/*
==============
Scr_MakeArray
==============
*/
void Scr_MakeArray()
{
	IncInParam();

	scrVmPub.top->type = VAR_POINTER;
	scrVmPub.top->u.pointerValue = Scr_AllocArray();
}

/*
==============
Scr_AddArray
==============
*/
void Scr_AddArray()
{
	unsigned int id;

	assert(scrVmPub.inparamcount);

	--scrVmPub.top;
	--scrVmPub.inparamcount;

	assert(scrVmPub.top->type == VAR_POINTER);

	id = GetNewArrayVariable(scrVmPub.top->u.pointerValue, GetArraySize(scrVmPub.top->u.pointerValue));
	SetNewVariableValue(id, scrVmPub.top + 1);
}

/*
==============
Scr_AddArrayStringIndexed
==============
*/
void Scr_AddArrayStringIndexed( unsigned int stringValue )
{
	unsigned int id;

	assert(scrVmPub.inparamcount);

	--scrVmPub.top;
	--scrVmPub.inparamcount;

	assert(scrVmPub.top->type == VAR_POINTER);

	id = GetNewVariable(scrVmPub.top->u.pointerValue, stringValue);
	SetNewVariableValue(id, scrVmPub.top + 1);
}

/*
==============
Scr_Error
==============
*/
void Scr_Error( const char *error )
{
	if ( scrVarPub.error_message == NULL )
	{
		scrVarPub.error_message = error;
	}

	Scr_ErrorInternal();
}

/*
==============
Scr_ErrorWithDialogMessage
==============
*/
void Scr_ErrorWithDialogMessage( const char *error, const char *dialog_error )
{
	scrVarPub.error_message = error;
	scrVmGlob.dialog_error_message = dialog_error;

	Scr_ErrorInternal();
}

/*
==============
Scr_TerminalError
==============
*/
void Scr_TerminalError( const char *error )
{
	Scr_DumpScriptThreads();
	Scr_DumpScriptVariablesDefault();

	scrVmPub.terminal_error = true;
	Scr_Error(error);
}

/*
==============
Scr_ParamError
==============
*/
void Scr_ParamError( unsigned int index, const char *error )
{
	scrVarPub.error_index = index + 1;
	Scr_Error(error);
}

/*
==============
Scr_ObjectError
==============
*/
void Scr_ObjectError( const char *error )
{
	scrVarPub.error_index = -1;
	Scr_Error(error);
}

/*
==============
SetEntityFieldValue
==============
*/
bool SetEntityFieldValue( unsigned int classnum, int entnum, int offset, VariableValue *value )
{
	assert(!scrVmPub.inparamcount);
	assert(!scrVmPub.outparamcount);

	scrVmPub.outparamcount = 1;
	scrVmPub.top = value;

	if ( !Scr_SetObjectField(classnum, entnum, offset) )
	{
		assert(!scrVmPub.inparamcount);
		assert(scrVmPub.outparamcount == 1);

		scrVmPub.outparamcount = 0;
		return false;
	}

	assert(!scrVmPub.inparamcount);

	if ( scrVmPub.outparamcount )
	{
		assert(scrVmPub.outparamcount == 1);

		RemoveRefToValue(scrVmPub.top);
		--scrVmPub.top;

		scrVmPub.outparamcount = 0;
	}

	return true;
}

/*
==============
GetEntityFieldValue
==============
*/
VariableValue GetEntityFieldValue( unsigned int classnum, int entnum, int offset )
{
	VariableValue value;

	assert(!scrVmPub.inparamcount);
	assert(!scrVmPub.outparamcount);

	scrVmPub.top = scrVmGlob.eval_stack - 1;
	scrVmGlob.eval_stack->type = VAR_UNDEFINED;

	Scr_GetObjectField(classnum, entnum, offset);

	assert(!scrVmPub.inparamcount || scrVmPub.inparamcount == 1);
	assert(!scrVmPub.outparamcount);
	assert(scrVmPub.top - scrVmPub.inparamcount == scrVmGlob.eval_stack - 1);

	scrVmPub.inparamcount = 0;

	value = *scrVmGlob.eval_stack;

	return value;
}

/*
==============
Scr_SetStructField
==============
*/
void Scr_SetStructField( unsigned int structId, unsigned int index )
{
	unsigned int fieldValueId;

	assert(!scrVmPub.outparamcount);
	assert(scrVmPub.inparamcount == 1);

	fieldValueId = Scr_GetVariableField(structId, index);

	assert(scrVmPub.inparamcount == 1);
	scrVmPub.inparamcount = 0;

	SetVariableFieldValue(fieldValueId, scrVmPub.top);

	assert(!scrVmPub.inparamcount);
	assert(!scrVmPub.outparamcount);

	scrVmPub.top--;
}

/*
==============
Scr_SetDynamicEntityField
==============
*/
void Scr_SetDynamicEntityField( int entnum, int classnum, unsigned int index )
{
	unsigned int entId;

	entId = Scr_GetEntityId(entnum, classnum);
	assert(GetObjectType( entId ) == VAR_ENTITY);

	Scr_SetStructField(entId, index);
}

/*
==============
Scr_IncTime
==============
*/
void Scr_IncTime()
{
	Scr_RunCurrentThreads();
	Scr_FreeEntityList();

	assert(!(scrVarPub.time & ~VAR_NAME_LOW_MASK));

	scrVarPub.time++;
	scrVarPub.time &= VAR_NAME_LOW_MASK;
}

/*
==============
Scr_DecTime
==============
*/
void Scr_DecTime()
{
	assert(!(scrVarPub.time & ~VAR_NAME_LOW_MASK));
	--scrVarPub.time;
	scrVarPub.time &= VAR_NAME_LOW_MASK;
}

/*
==============
Scr_RunCurrentThreads
==============
*/
void Scr_RunCurrentThreads()
{
	assert(!scrVmPub.function_count);
	assert(!scrVarPub.error_message);
	assert(!scrVarPub.error_index);
	assert(!scrVmPub.outparamcount);
	assert(!scrVmPub.inparamcount);
	assert(scrVmPub.top == scrVmPub.stack);

	VM_SetTime();
}

void Scr_ResetTimeout()
{
	scrVmGlob.starttime = (int)(Sys_Rdtsc() >> 2);
}

/*
==============
Scr_StackClear
==============
*/
void Scr_StackClear( void )
{
	scrVmPub.top = scrVmPub.stack;
}

/*
==============
Scr_TraverseScript
==============
*/
void Scr_TraverseScript( char const *pos )
{
	int opcode;

loop:
	opcode = *(unsigned char *)pos;
	pos++;

	switch ( opcode )
	{
	case OP_End:
	case OP_Return:
	case OP_GetUndefined:
	case OP_GetZero:
	case OP_GetLevelObject:
	case OP_GetAnimObject:
	case OP_GetSelf:
	case OP_GetLevel:
	case OP_GetGame:
	case OP_GetAnim:
	case OP_GetGameRef:
	case OP_EvalLocalVariableCached0:
	case OP_EvalLocalVariableCached1:
	case OP_EvalLocalVariableCached2:
	case OP_EvalLocalVariableCached3:
	case OP_EvalLocalVariableCached4:
	case OP_EvalLocalVariableCached5:
	case OP_EvalArray:
	case OP_EvalLocalArrayRefCached0:
	case OP_EvalArrayRef:
	case OP_ClearArray:
	case OP_EmptyArray:
	case OP_GetSelfObject:
	case OP_SafeSetVariableFieldCached0:
	case OP_clearparams:
	case OP_checkclearparams:
	case OP_EvalLocalVariableRefCached0:
	case OP_SetVariableField:
	case OP_SetLocalVariableFieldCached0:
	case OP_wait:
	case OP_waittillFrameEnd:
	case OP_PreScriptCall:
	case OP_ScriptFunctionCallPointer:
	case OP_ScriptMethodCallPointer:
	case OP_DecTop:
	case OP_CastFieldObject:
	case OP_CastBool:
	case OP_BoolNot:
	case OP_BoolComplement:
	case OP_inc:
	case OP_dec:
	case OP_bit_or:
	case OP_bit_ex_or:
	case OP_bit_and:
	case OP_equality:
	case OP_inequality:
	case OP_less:
	case OP_greater:
	case OP_less_equal:
	case OP_greater_equal:
	case OP_shift_left:
	case OP_shift_right:
	case OP_plus:
	case OP_minus:
	case OP_multiply:
	case OP_divide:
	case OP_mod:
	case OP_size:
	case OP_waittillmatch:
	case OP_waittill:
	case OP_notify:
	case OP_endon:
	case OP_voidCodepos:
	case OP_vector:
	goto loop;

	case OP_ScriptThreadCall:
	case OP_ScriptMethodThreadCall:
		Scr_ReadCodePos(&pos);
		Scr_ReadInt(&pos);
	goto loop;

	case OP_GetUnsignedShort:
	case OP_GetNegUnsignedShort:
	case OP_CreateLocalVariable:
	case OP_EvalLevelFieldVariable:
	case OP_EvalAnimFieldVariable:
	case OP_EvalSelfFieldVariable:
	case OP_EvalFieldVariable:
	case OP_EvalLevelFieldVariableRef:
	case OP_EvalAnimFieldVariableRef:
	case OP_EvalSelfFieldVariableRef:
	case OP_EvalFieldVariableRef:
	case OP_ClearFieldVariable:
	case OP_SafeCreateVariableFieldCached:
	case OP_SetLevelFieldVariableField:
	case OP_SetAnimFieldVariableField:
	case OP_SetSelfFieldVariableField:
	case OP_CallBuiltin0:
	case OP_CallBuiltin1:
	case OP_CallBuiltin2:
	case OP_CallBuiltin3:
	case OP_CallBuiltin4:
	case OP_CallBuiltin5:
	case OP_CallBuiltinMethod0:
	case OP_CallBuiltinMethod1:
	case OP_CallBuiltinMethod2:
	case OP_CallBuiltinMethod3:
	case OP_CallBuiltinMethod4:
	case OP_CallBuiltinMethod5:
	case OP_JumpOnFalse:
	case OP_JumpOnTrue:
	case OP_JumpOnFalseExpr:
	case OP_JumpOnTrueExpr:
	case OP_jumpback:
		Scr_ReadUnsignedShort(&pos);
	goto loop;

	case OP_GetInteger:
	case OP_ScriptThreadCallPointer:
	case OP_ScriptMethodThreadCallPointer:
	case OP_jump:
	case OP_switch:
		Scr_ReadInt(&pos);
	goto loop;

	case OP_GetFloat:
		Scr_ReadFloat(&pos);
	goto loop;

	case OP_GetString:
	case OP_GetIString:
		Scr_ReadUnsignedShort(&pos);
	goto loop;

	case OP_GetVector:
		Scr_ReadVector(&pos);
	goto loop;

	case OP_GetAnimation:
		Scr_ReadInt(&pos);
	goto loop;

	case OP_GetFunction:
	case OP_ScriptFunctionCall2:
	case OP_ScriptFunctionCall:
	case OP_ScriptMethodCall:
		Scr_ReadCodePos(&pos);
	goto loop;

	case OP_GetByte:
	case OP_GetNegByte:
	case OP_RemoveLocalVariables:
	case OP_EvalLocalVariableCached:
	case OP_EvalLocalArrayCached:
	case OP_EvalLocalArrayRefCached:
	case OP_SafeSetVariableFieldCached:
	case OP_SafeSetWaittillVariableFieldCached:
	case OP_EvalLocalVariableRefCached:
	case OP_SetLocalVariableFieldCached:
	case OP_EvalLocalVariableObjectCached:
	case OP_prof_begin:
	case OP_prof_end:
		pos++;
	goto loop;

	case OP_CallBuiltin:
	case OP_CallBuiltinMethod:
		pos++;
		Scr_ReadUnsignedShort(&pos);
	goto loop;

	case OP_endswitch:
		Scr_ReadIntArray(&pos, 2 * Scr_ReadUnsignedShort(&pos));
	goto loop;

	default:
		return;
	}
}

int Scr_IsSystemActive( bool unused )
{
	return scrVarPub.timeArrayId != 0;
}
static inline short Scr_ReadShort( const char **pos )
{
	short value = *(reinterpret_cast<const short *>(*pos));
	*pos += sizeof(short);

	return value;
}
static inline int Scr_ReadInt( const char **pos )
{
	int value = *(reinterpret_cast<const int *>(*pos));
	*pos += sizeof(int);

	return value;
}
static inline unsigned short Scr_ReadUnsignedShort( const char **pos )
{
	return (unsigned short)Scr_ReadShort(pos);
}
static inline unsigned int Scr_ReadUnsigned( const char **pos )
{
	return Scr_ReadInt( pos );
}
static inline const char* Scr_ReadCodePos( const char **pos )
{
	return (const char *)Scr_ReadInt( pos );
}
void VM_Shutdown()
{
	if ( scrVarPub.tempVariable )
	{
		FreeValue(scrVarPub.tempVariable);
		scrVarPub.tempVariable = 0;
	}
}
unsigned int Scr_GetNumScriptThreads()
{
	return 0;
}
const char* Scr_GetNextCodepos( VariableValue *top, const char *pos, int opcode, int mode, unsigned int *localId )
{
	VariableValue value;
	unsigned int caseCount;
	unsigned int caseValue;
	int offset;
	unsigned int currentCaseValue;
	const char *currentCodePos;

	*localId = scrVmPub.function_frame->fs.localId;

	while ( 2 )
	{
		pos++;

		if ( mode == 2 )
		{
			switch ( opcode )
			{
			case 0x52:
			case 0x56:
				if ( top->type != 1 )
				{
					goto LABEL_11;
				}
				// fallthrough
			case 0x4f:
			case 0x50:
			case 0x54:
				if ( scrVmPub.function_count >= 32 )
				{
					goto LABEL_11;
				}
				*localId = 0;
				return Scr_ReadCodePos( &pos );

			case 0x53:
			case 0x57:
				if ( top[-1].type != 1 )
				{
					goto LABEL_11;
				}
				// fallthrough
			case 0x51:
			case 0x55:
				if ( top->type != 9 )
				{
					goto LABEL_11;
				}
				if ( scrVmPub.function_count >= 32 )
				{
					goto LABEL_11;
				}
				*localId = 0;
				return (const char *)top->u.intValue;

			default:
				goto LABEL_11;
			}
		}
		else
		{
		LABEL_11:
			switch ( opcode )
			{
			case 0:
			case 1:
				return Scr_GetReturnPos( localId );

			case 94:
			case 96:
				value = *top;
				AddRefToValue( value.type, value.u );
				Scr_CastBool( &value );
				offset = Scr_ReadUnsignedShort( &pos );
				if ( !scrVarPub.error_message )
				{
					if ( value.u.intValue )
					{
						return pos;
					}
					return &pos[offset];
				}
				goto LABEL_44;

			case 95:
			case 97:
				value = *top;
				AddRefToValue( value.type, value.u );
				Scr_CastBool( &value );
				offset = Scr_ReadUnsignedShort( &pos );
				if ( !scrVarPub.error_message )
				{
					if ( !value.u.intValue )
					{
						return pos;
					}
					return &pos[offset];
				}
				goto LABEL_44;

			case 98:
				offset = Scr_ReadInt( &pos );
				return &pos[offset];

			case 99:
				offset = (unsigned short)Scr_ReadUnsignedShort( &pos );
				return &pos[-offset];

			case 124:
				offset = Scr_ReadInt( &pos );
				pos += offset;
				caseCount = Scr_ReadUnsignedShort( &pos );

				switch ( top->type )
				{
				case 6:
					if ( IsValidArrayIndex( top->u.intValue ) )
					{
						caseValue = GetInternalVariableIndex( top->u.intValue );
					}
					else
					{
						pos += 8 * caseCount;
						return pos;
					}
					break;

				case 2:
					caseValue = top->u.intValue;
					break;

				default:
					pos += 8 * caseCount;
					return pos;
				}

				top--;

				if ( caseCount )
				{
					do
					{
						currentCaseValue = Scr_ReadUnsigned( &pos );
						currentCodePos = Scr_ReadCodePos( &pos );

						if ( currentCaseValue == caseValue )
						{
							pos = currentCodePos;
							return pos;
						}

						caseCount--;
					}
					while ( caseCount );

					if ( !currentCaseValue )
					{
						pos = currentCodePos;
					}
				}
				return pos;

			case 125:
				caseCount = Scr_ReadUnsignedShort( &pos );
				Scr_ReadIntArray( &pos, 2 * caseCount );
				return pos;

			case 4:
			case 5:
			case 23:
			case 30:
			case 31:
			case 34:
			case 50:
			case 51:
			case 55:
			case 61:
			case 90:
			case 119:
			case 133:
			case 134:
				pos++;
				goto LABEL_44;

			case 10:
			case 11:
			case 22:
			case 39:
			case 40:
			case 41:
			case 42:
			case 43:
			case 44:
			case 45:
			case 46:
			case 47:
			case 48:
			case 56:
			case 58:
			case 59:
			case 130:
			case 131:
			case 132:
				Scr_ReadUnsignedShort( &pos );
				goto LABEL_44;

			case 68:
			case 75:
				pos++;
				Scr_ReadUnsignedShort( &pos );
				goto LABEL_44;

			case 6:
			case 7:
			case 62:
			case 63:
			case 64:
			case 65:
			case 66:
			case 67:
			case 69:
			case 70:
			case 71:
			case 72:
			case 73:
			case 74:
				Scr_ReadUnsignedShort( &pos );
				goto LABEL_44;

			case 8:
			case 9:
			case 19:
			case 21:
			case 79:
			case 80:
			case 82:
			case 85:
			case 87:
				Scr_ReadInt( &pos );
				goto LABEL_44;

			case 84:
			case 86:
			case 129:
				Scr_ReadInt( &pos );
				Scr_ReadInt( &pos );
				goto LABEL_44;

			case 12:
				Scr_ReadVector( &pos );
				goto LABEL_44;

			default:
			LABEL_44:
				Scr_ClearErrorMessage();

				if ( ( opcode = *(const char *)pos ) != 57 )
				{
					return pos;
				}
				else
				{
					continue;
				}
			}
		}
	}
}
__declspec(noinline) void Scr_TerminateThread( unsigned int localId )
{
	unsigned int startLocalId = GetStartLocalId(localId);

	switch ( (int)GetObjectType(startLocalId) )
	{
	case VAR_THREAD:
		Scr_TerminateRunningThread(localId);
		break;

	case VAR_NOTIFY_THREAD:
		Scr_TerminateWaittillThread(localId, startLocalId);
		break;

	case VAR_TIME_THREAD:
		Scr_TerminateWaitThread(localId, startLocalId);
		break;

	default:
		assert(0); // unreachable
		break;
	}
}
void VM_Notify( unsigned int notifyListOwnerId, unsigned int stringValue, VariableValue *top )
{
	unsigned int notifyNameListId;
	unsigned int startLocalId;
	unsigned int notifyListId;
	VariableValue *currentValue;
	VariableUnion *tempValue;
	VariableStackBuffer *stackValue;
	VariableStackBuffer *newStackValue;
	char *buf;
	int size;
	int newSize;
	int len;
	int bufLen;
	unsigned int selfId;
	unsigned int selfNameId;
	unsigned int notifyListEntry;
	bool bNoStack;
	VariableValue tempValue2;
	VariableValue tempValue3;
	VariableValue tempValue5;
	unsigned int stackId;
	int type;

	notifyListId = FindVariable(notifyListOwnerId, OBJECT_NOTIFY_LIST);

	if ( !notifyListId )
	{
		return;
	}

	notifyListId = FindObject(notifyListId);
	assert(notifyListId);

	notifyNameListId = FindVariable(notifyListId, stringValue);

	if ( !notifyNameListId )
	{
		return;
	}

	notifyNameListId = FindObject(notifyNameListId);
	assert(notifyNameListId);

	AddRefToObject(notifyNameListId);

	assert(!scrVarPub.evaluate);
	scrVarPub.evaluate = true;

	notifyListEntry = notifyNameListId;

	while ( 1 )
	{
next:
		notifyListEntry = FindPrevSibling(notifyListEntry);

		if ( !notifyListEntry )
		{
			break;
		}

		startLocalId = GetVariableKeyObject(notifyListEntry);
		selfId = Scr_GetSelf(startLocalId);
		selfNameId = FindObject(FindObjectVariable(scrVarPub.pauseArrayId, selfId));

		if ( !GetObjectType(notifyListEntry) )
		{
			VM_CancelNotifyInternal(notifyListOwnerId, startLocalId, notifyListId, notifyNameListId, stringValue);
			Scr_KillEndonThread(startLocalId);

			RemoveVariableNI(selfNameId, startLocalId + SL_MAX_STRING_INDEX);

			if ( !GetArraySize(selfNameId) )
			{
				RemoveVariableNI(scrVarPub.pauseArrayId, selfId + SL_MAX_STRING_INDEX);
			}

			Scr_TerminateThread(selfId);

			notifyListEntry = notifyNameListId;
			continue;
		}

		assert(GetObjectType( notifyListEntry ) == VAR_STACK);

		tempValue = GetVariableValueAddress(notifyListEntry);
		stackValue = tempValue->stackValue;

		if ( *((unsigned char *)stackValue->pos - 1) == OP_waittillmatch )
		{
			size = *stackValue->pos;
			assert(size >= 0);
			assert(size <= stackValue->size);

			buf = &stackValue->buf[STACKBUF_BUFFER_SIZE * (stackValue->size - size)];

			for ( currentValue = top; size; currentValue-- )
			{
				assert(currentValue->type != VAR_CODEPOS);

				if ( currentValue->type == VAR_PRECODEPOS )
				{
					goto next;
				}

				size--;

				tempValue3.type = *(unsigned char *)buf;
				assert(tempValue3.type != VAR_CODEPOS);

				if ( tempValue3.type == VAR_PRECODEPOS )
				{
					break;
				}

				buf += sizeof(unsigned char);

				tempValue3.u.codePosValue = *(const char **)buf;
				buf += sizeof(VariableUnion);

				AddRefToValue(&tempValue3);

				tempValue2 = *currentValue;

				AddRefToValue(&tempValue2);

				Scr_EvalEquality(&tempValue3, &tempValue2);

			if ( !scrVarPub.error_message )
			{
				if ( !tempValue3.u.intValue )
				{
					goto next;
				}
			}
			else
			{
				goto error;
			}
		}

		goto done;
	error:
		RuntimeError( stackValue->pos, *stackValue->pos - size + int( sizeof(VariableUnion) - sizeof(unsigned char) ), scrVarPub.error_message, scrVmGlob.dialog_error_message );
		Scr_ClearErrorMessage();

		goto next;
	done:
		stackValue->pos++;
		bNoStack = true;
		}
		else
		{
			bNoStack = top->type == VAR_PRECODEPOS;
		}

		tempValue5.type = VAR_STACK;
		tempValue5.u.stackValue = stackValue;

		stackId = GetNewObjectVariable(GetArray(GetVariableI2NI(scrVarPub.timeArrayId, scrVarPub.time)), startLocalId);
		SetNewVariableValue(stackId, &tempValue5);

		tempValue = GetVariableValueAddress(stackId);

		VM_CancelNotifyInternal(notifyListOwnerId, startLocalId, notifyListId, notifyNameListId, stringValue);
		RemoveVariableNI(selfNameId, startLocalId + SL_MAX_STRING_INDEX);

		if ( !GetArraySize(selfNameId) )
		{
			RemoveVariableNI(scrVarPub.pauseArrayId, selfId + SL_MAX_STRING_INDEX);
		}

		Scr_SetThreadWaitTime(startLocalId, scrVarPub.time);

		if ( bNoStack )
		{
			notifyListEntry = notifyNameListId;
		}
		else
		{
			assert(top->type != VAR_PRECODEPOS);
			assert(top->type != VAR_CODEPOS);

			size = stackValue->size;
			newSize = size;
			currentValue = top;

			do
			{
				newSize++;
				assert(currentValue[-1].type != VAR_CODEPOS);
			}
			while ( (currentValue--)[-1].type != VAR_PRECODEPOS );
			assert(newSize >= 0 && newSize < (1 << 16));

			len = STACKBUF_BUFFER_SIZE; len = len * size;
			bufLen = STACKBUF_BUFFER_SIZE * newSize + sizeof(*stackValue) - 1;

			if ( !MT_Realloc(stackValue->bufLen, bufLen) )
			{
				newStackValue = (VariableStackBuffer *)MT_Alloc(bufLen, 1);

				newStackValue->bufLen = bufLen;
				newStackValue->pos = stackValue->pos;
				newStackValue->localId = stackValue->localId;

				memcpy(newStackValue->buf, stackValue->buf, len);
				MT_Free(stackValue, stackValue->bufLen);

				stackValue = newStackValue;
				tempValue->stackValue = stackValue;
			}

			stackValue->size = newSize;
			buf = &stackValue->buf[len];

			newSize -= size;
			assert(newSize);

			do
			{
				currentValue++;
				AddRefToValue(currentValue);

				*buf = currentValue->type;
				buf += sizeof(unsigned char);

				*(const char **)buf = currentValue->u.codePosValue;
				buf += sizeof(VariableUnion);

				newSize--;
			}
			while ( newSize );

			assert(buf - (const char *)stackValue == bufLen);
			notifyListEntry = notifyNameListId;
		}
	}

	RemoveRefToObject(notifyNameListId);

	assert(scrVarPub.evaluate);
	scrVarPub.evaluate = false;
}
const char* Scr_GetStringIncludeNull( unsigned int index )
{
	return SL_ConvertToString( Scr_GetConstString( index ) );
}
const char* Scr_GetIString( unsigned int index )
{
	return SL_ConvertToString( Scr_GetConstIString( index ) );
}
void Scr_AddBool( int value )
{
	assert(value == false || value == true);
	IncInParam();

	scrVmPub.top->type = VAR_INTEGER;
	scrVmPub.top->u.intValue = value;
}
void Scr_AddInt( int value )
{
	IncInParam();

	scrVmPub.top->type = VAR_INTEGER;
	scrVmPub.top->u.intValue = value;
}

