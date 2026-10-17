#include "../qcommon/qcommon.h"
#include "script_public.h"

static scrCompileGlob_t scrCompileGlob;
scrCompilePub_t scrCompilePub;

void EmitByte( byte value );
void EmitCodepos( const char *pos );
void EmitValue( VariableCompileValue *constValue );
void EmitPrimitiveExpression( sval_u expr, scr_block_s *block );
bool EvalExpression( sval_u expr, VariableCompileValue *constValue );
bool EmitOrEvalExpression( sval_u expr, VariableCompileValue *constValue, scr_block_s *block );
void EmitExpression( sval_u expr, scr_block_s *block );
void EmitVariableExpressionRef( sval_u expr, scr_block_s *block );
void Scr_CalcLocalVarsVariableExpressionRef( sval_u expr, scr_block_s *block );
void EmitArrayPrimitiveExpressionRef( sval_u expr, sval_u sourcePos, scr_block_s *block );
void Scr_CalcLocalVarsArrayPrimitiveExpressionRef( sval_u expr, scr_block_s *block );
void EmitPrimitiveExpressionFieldObject( sval_u expr, sval_u sourcePos, scr_block_s *block );
void EmitExpressionFieldObject( sval_u expr, sval_u sourcePos, scr_block_s *block );
void EmitCaseStatementInfo( unsigned int name, sval_u sourcePos );
void EmitStatement( sval_u val, bool lastStatement, unsigned int endSourcePos, scr_block_s *block );
void Scr_CalcLocalVarsStatement( sval_u val, scr_block_s *block );
void EmitStatementList( sval_u val, bool lastStatement, unsigned int endSourcePos, scr_block_s *block );
void Scr_CalcLocalVarsStatementList( sval_u val, scr_block_s *block );
void Scr_CalcLocalVarsDeveloperStatementList( sval_u val, scr_block_s *block, sval_u *devStatBlock );
void EmitDeveloperStatementList( sval_u val, sval_u sourcePos, scr_block_s *block, sval_u *devStatBlock );
void AddRefToValue( VariableValue *value );

void Scr_CompileRemoveRefToString( unsigned int stringValue );
void EmitCanonicalString( unsigned int stringValue );
void EmitCanonicalStringConst( unsigned int stringValue );
void CompileTransferRefToString( unsigned int stringValue, unsigned int user );
void EmitOpcode( unsigned int op, int offset, int callType );
void EmitEnd();
void EmitReturn( void );
void EmitShort( short value );
void EmitUnsignedShort( unsigned short value );
void EmitByte( byte value );
void EmitFloat( float value );
void EmitString( unsigned int value );
void EmitCodepos( const char *pos );
void EvalUndefined( sval_u sourcePos, VariableCompileValue *constValue );
void EmitGetUndefined( sval_u sourcePos );
void EvalInteger( int value, sval_u sourcePos, VariableCompileValue *constValue );
void EmitGetInteger( int value, sval_u sourcePos );
void EvalFloat( float value, sval_u sourcePos, VariableCompileValue *constValue );
void EmitGetFloat( float value, sval_u sourcePos );
void EmitAnimTree( sval_u sourcePos );
void EmitSetVariableField( sval_u sourcePos );
int Scr_FindLocalVar( scr_block_s *block, int startIndex, unsigned int name );
int Scr_FindLocalVarIndex( unsigned int name, sval_u sourcePos, bool create, scr_block_s *block );
void EmitCreateLocalVars( scr_block_s *block, sval_u sourcePos );
void EmitRemoveLocalVars( scr_block_s *block, scr_block_s *outerBlock );
void EmitNOP2( bool lastStatement, unsigned int endSourcePos, scr_block_s *block );
void Scr_CheckMaxSwitchCases( int count );
void Scr_CheckLocalVarsCount( int localVarsCount );
void Scr_RegisterLocalVar( unsigned int name, sval_u sourcePos, scr_block_s *block );
void Scr_CopyBlock( scr_block_s *from, scr_block_s **to );
void Scr_InitFromChildBlocks( scr_block_s **childBlocks, int childCount, scr_block_s *block );
void Scr_AppendChildBlocks( scr_block_s **childBlocks, int childCount, scr_block_s *block );
void Scr_MergeChildBlocks( scr_block_s **childBlocks, int childCount, scr_block_s *block );
void Scr_TransferBlock( scr_block_s *from, scr_block_s *to );
void EmitSafeSetVariableField( sval_u expr, sval_u sourcePos, scr_block_s *block );
void EmitSafeSetWaittillVariableField( sval_u expr, sval_u sourcePos, scr_block_s *block );
void EvalString( unsigned int value, sval_u sourcePos, VariableCompileValue *constValue );
void EmitGetString( unsigned int value, sval_u sourcePos );
void EvalIString( unsigned int value, sval_u sourcePos, VariableCompileValue *constValue );
void EmitGetIString( unsigned int value, sval_u sourcePos );
void EmitGetVector( const vec3_t value, sval_u sourcePos );
void EmitValue( VariableCompileValue *constValue );
void Scr_PushValue( VariableCompileValue *constValue );
void Scr_PopValue();
void EmitCastBool( sval_u sourcePos );
void EmitBoolNot( sval_u sourcePos );
void EmitBoolComplement( sval_u sourcePos );
void EmitSize( sval_u expr, sval_u sourcePos, scr_block_s *block );
void EmitSelf( sval_u sourcePos );
void EmitLevel( sval_u sourcePos );
void EmitGame( sval_u sourcePos );
void EmitAnim( sval_u sourcePos );
void EmitSelfObject( sval_u sourcePos );
void EmitLevelObject( sval_u sourcePos );
void EmitAnimObject( sval_u sourcePos );
void EmitLocalVariable( sval_u expr, sval_u sourcePos, scr_block_s *block );
void EmitLocalVariableRef( sval_u expr, sval_u sourcePos, scr_block_s *block );
void EmitGameRef( sval_u sourcePos );
void EmitEvalArray( sval_u sourcePos, sval_u indexSourcePos );
void EmitEvalArrayRef( sval_u sourcePos, sval_u indexSourcePos );
void EmitClearArray( sval_u sourcePos, sval_u indexSourcePos );
void EmitEmptyArray( sval_u sourcePos );
void EmitAnimation( sval_u anim, sval_u sourcePos );
void EmitFieldVariable( sval_u expr, sval_u field, sval_u sourcePos, scr_block_s *block );
void EmitFieldVariableRef( sval_u expr, sval_u field, sval_u sourcePos, scr_block_s *block );
void EmitClearFieldVariable( sval_u expr, sval_u field, sval_u sourcePos, sval_u rhsSourcePos, scr_block_s *block );
void EmitObject( sval_u expr, sval_u sourcePos );
void EmitDecTop();
void EmitCastFieldObject( sval_u sourcePos );
void EmitArrayVariable( sval_u expr, sval_u index, sval_u sourcePos, sval_u indexSourcePos, scr_block_s *block );
void EmitArrayVariableRef( sval_u expr, sval_u index, sval_u sourcePos, sval_u indexSourcePos, scr_block_s *block );
void Scr_CalcLocalVarsArrayVariableRef( sval_u expr, scr_block_s *block );
void EmitClearArrayVariable( sval_u expr, sval_u index, sval_u sourcePos, sval_u indexSourcePos, scr_block_s *block );
void EmitVariableExpression( sval_u expr, scr_block_s *block );
int GetExpressionCount( sval_u exprlist );
int EmitExpressionList( sval_u exprlist, scr_block_s *block );
sval_u* GetSingleParameter( sval_u exprlist );
void AddExpressionListOpcodePos( sval_u exprlist );
unsigned int AddFilePrecache( unsigned int filename, unsigned int sourcePos, bool include );
void EmitFunction(sval_u func, sval_u sourcePos);
void EmitGetFunction( sval_u func, sval_u sourcePos );
int AddFunction( intptr_t func, const char *pName );
void EmitPostScriptFunction( sval_u func, int param_count, bool bMethod, sval_u nameSourcePos );
void EmitPostScriptFunctionPointer( sval_u expr, int param_count, bool bMethod, sval_u nameSourcePos, sval_u sourcePos, scr_block_s *block );
void EmitPostScriptThread( sval_u func, int param_count, bool bMethod, sval_u sourcePos );
void EmitPostScriptThreadPointer( sval_u expr, int param_count, bool bMethod, sval_u sourcePos, scr_block_s *block );
void EmitPostScriptFunctionCall( sval_u func_name, int param_count, bool bMethod, sval_u nameSourcePos, scr_block_s *block );
void EmitPostScriptThreadCall( sval_u func_name, int param_count, bool bMethod, sval_u sourcePos, sval_u nameSourcePos, scr_block_s *block );
void EmitPreFunctionCall( sval_u func_name );
void EmitPostFunctionCall( sval_u func_name, int param_count, bool bMethod, scr_block_s *block );
unsigned int Scr_GetBuiltin( sval_u func_name );
void Scr_BeginDevScript( int *type, char **savedPos );
void Scr_EndDevScript( int type, char **savedPos );
int Scr_GetCacheType( int type );
int Scr_GetUncacheType( int type );
void EmitCallBuiltinOpcode( int param_count, sval_u sourcePos );
void EmitCallBuiltinMethodOpcode( int param_count, sval_u sourcePos );
void EmitCall( sval_u func_name, sval_u params, bool bStatement, scr_block_s *block );
void EmitMethod( sval_u expr, sval_u func_name, sval_u params, sval_u methodSourcePos, bool bStatement, scr_block_s *block );
void LinkThread( unsigned int threadCountId, VariableValue *pos, bool allowFarCall );
void LinkFile( unsigned int fileId );
unsigned int SpecifyThreadPosition( unsigned int posId, unsigned int name, unsigned int sourcePos, int type );
void SetThreadPosition( unsigned int posId, sval_u sourcePos );
void EmitCallExpression( sval_u expr, bool bStatement, scr_block_s *block );
void EmitCallExpressionFieldObject( sval_u expr, scr_block_s *block );
void Scr_CreateVector( VariableCompileValue *constValue, VariableValue *value );
bool EvalPrimitiveExpressionList( sval_u exprlist, sval_u sourcePos, VariableCompileValue *constValue );
bool EmitOrEvalPrimitiveExpressionList( sval_u exprlist, sval_u sourcePos, VariableCompileValue *constValue, scr_block_s *block );
void EmitExpressionListFieldObject( sval_u exprlist, sval_u sourcePos, scr_block_s *block );
bool EvalPrimitiveExpression( sval_u expr, VariableCompileValue *constValue );
bool EmitOrEvalPrimitiveExpression( sval_u expr, VariableCompileValue *constValue, scr_block_s *block );
void EmitPrimitiveExpression( sval_u expr, scr_block_s *block );
void EmitBoolOrExpression( sval_u expr1, sval_u expr2, sval_u expr1sourcePos, sval_u expr2sourcePos, scr_block_s *block );
void EmitBoolAndExpression( sval_u expr1, sval_u expr2, sval_u expr1sourcePos, sval_u expr2sourcePos, scr_block_s *block );
bool EvalBinaryOperatorExpression( sval_u expr1, sval_u expr2, sval_u opcode, sval_u sourcePos, VariableCompileValue *constValue );
bool EmitOrEvalBinaryOperatorExpression( sval_u expr1, sval_u expr2, sval_u opcode, sval_u sourcePos, VariableCompileValue *constValue, scr_block_s *block );
void EmitBinaryEqualsOperatorExpression( sval_u lhs, sval_u rhs, sval_u opcode, sval_u sourcePos, scr_block_s *block );
void Scr_CalcLocalVarsBinaryEqualsOperatorExpression( sval_u expr, scr_block_s *block );
bool EvalExpression( sval_u expr, VariableCompileValue *constValue );
bool EmitOrEvalExpression( sval_u expr, VariableCompileValue *constValue, scr_block_s *block );
void EmitExpression( sval_u expr, scr_block_s *block );
void EmitVariableExpressionRef( sval_u expr, scr_block_s *block );
void Scr_CalcLocalVarsVariableExpressionRef( sval_u expr, scr_block_s *block );
void EmitArrayPrimitiveExpressionRef( sval_u expr, sval_u sourcePos, scr_block_s *block );
void Scr_CalcLocalVarsArrayPrimitiveExpressionRef( sval_u expr, scr_block_s *block );
void EmitPrimitiveExpressionFieldObject( sval_u expr, sval_u sourcePos, scr_block_s *block );
void EmitExpressionFieldObject( sval_u expr, sval_u sourcePos, scr_block_s *block );
__declspec(noinline) void ConnectBreakStatements();
void ConnectContinueStatements();
bool IsUndefinedPrimitiveExpression( sval_u expr );
int IsUndefinedExpression( sval_u expr );
bool EmitClearVariableExpression( sval_u expr, sval_u rhsSourcePos, scr_block_s *block );
void EmitAssignmentStatement( sval_u lhs, sval_u rhs, sval_u sourcePos, sval_u rhsSourcePos, scr_block_s *block );
void Scr_CalcLocalVarsAssignmentStatement( sval_u lhs, sval_u rhs, scr_block_s *block );
void EmitCallExpressionStatement( sval_u expr, scr_block_s *block );
void EmitReturnStatement( sval_u expr, sval_u sourcePos, scr_block_s *block );
void EmitEndStatement( sval_u sourcePos, scr_block_s *block );
void EmitWaitStatement( sval_u expr, sval_u sourcePos, sval_u waitSourcePos, scr_block_s *block );
void EmitWaittillFrameEnd( sval_u sourcePos );
void EmitIfStatement( sval_u expr, sval_u stmt, sval_u sourcePos, bool lastStatement, unsigned int endSourcePos, scr_block_s *block, sval_u *ifStatBlock );
void Scr_CalcLocalVarsIfStatement( sval_u stmt, scr_block_s *block, sval_u *ifStatBlock );
void EmitIfElseStatement( sval_u expr, sval_u stmt1, sval_u stmt2, sval_u sourcePos, sval_u elseSourcePos, bool lastStatement, unsigned int endSourcePos, scr_block_s *block, sval_u *ifStatBlock, sval_u *elseStatBlock );
void Scr_CalcLocalVarsIfElseStatement( sval_u stmt1, sval_u stmt2, scr_block_s *block, sval_u *ifStatBlock, sval_u *elseStatBlock );
void Scr_AddBreakBlock( scr_block_s *block );
void Scr_AddContinueBlock( scr_block_s *block );
void EmitWhileStatement( sval_u expr, sval_u stmt, sval_u sourcePos, sval_u whileSourcePos, scr_block_s *block, sval_u *whileStatBlock );
void Scr_CalcLocalVarsWhileStatement( sval_u expr, sval_u stmt, scr_block_s *block, sval_u *whileStatBlock );
void EmitForStatement( sval_u stmt1, sval_u expr, sval_u stmt2, sval_u stmt, sval_u sourcePos, sval_u forSourcePos, scr_block_s *block, sval_u *forStatBlock, sval_u *forStatPostBlock );
void Scr_CalcLocalVarsForStatement( sval_u stmt1, sval_u expr, sval_u stmt2, sval_u stmt, scr_block_s *block, sval_u *forStatBlock, sval_u *forStatPostBlock );
void EmitIncStatement( sval_u expr, sval_u sourcePos, scr_block_s *block );
void Scr_CalcLocalVarsIncStatement( sval_u expr, scr_block_s *block );
void EmitDecStatement( sval_u expr, sval_u sourcePos, scr_block_s *block );
void EmitFormalParameterListInternal( sval_u *node, scr_block_s *block );
void Scr_CalcLocalVarsFormalParameterListInternal( sval_u *node, scr_block_s *block );
void EmitFormalWaittillParameterListRefInternal( sval_u *node, scr_block_s *block );
void EmitWaittillStatement( sval_u obj, sval_u exprlist, sval_u sourcePos, sval_u waitSourcePos, scr_block_s *block );
void Scr_CalcLocalVarsWaittillStatement( sval_u exprlist, scr_block_s *block );
void EmitWaittillmatchStatement( sval_u obj, sval_u exprlist, sval_u sourcePos, sval_u waitSourcePos, scr_block_s *block );
void EmitNotifyStatement( sval_u obj, sval_u exprlist, sval_u sourcePos, sval_u notifySourcePos, scr_block_s *block );
void EmitEndOnStatement( sval_u obj, sval_u expr, sval_u sourcePos, sval_u exprSourcePos, scr_block_s *block );
int CompareCaseInfo( const void *elem1, const void *elem2 );
bool Scr_IsLastStatement( sval_u *node );
void EmitCaseStatement( sval_u expr, sval_u sourcePos );
void EmitDefaultStatement( sval_u sourcePos );
void EmitSwitchStatementList( sval_u val, bool lastStatement, unsigned int endSourcePos, scr_block_s *block );
void Scr_CalcLocalVarsSwitchStatement( sval_u stmtlist, scr_block_s *block );
void EmitSwitchStatement( sval_u expr, sval_u stmtlist, sval_u sourcePos, bool lastStatement, unsigned int endSourcePos, scr_block_s *block );
void EmitCaseStatementInfo( unsigned int name, sval_u sourcePos );
void EmitBreakStatement( sval_u sourcePos, scr_block_s *block );
void EmitContinueStatement( sval_u sourcePos, scr_block_s *block );
void EmitBreakpointStatement( sval_u sourcePos );
void EmitProfStatement( sval_u profileName, sval_u sourcePos, unsigned char op );
void EmitProfBeginStatement( sval_u profileName, sval_u sourcePos );
void EmitProfEndStatement( sval_u profileName, sval_u sourcePos );
void EmitStatement( sval_u val, bool lastStatement, unsigned int endSourcePos, scr_block_s *block );
void Scr_CalcLocalVarsStatement( sval_u val, scr_block_s *block );
void EmitStatementList( sval_u val, bool lastStatement, unsigned int endSourcePos, scr_block_s *block );
void Scr_CalcLocalVarsStatementList( sval_u val, scr_block_s *block );
void Scr_CalcLocalVarsDeveloperStatementList( sval_u val, scr_block_s *block, sval_u *devStatBlock );
void EmitDeveloperStatementList( sval_u val, sval_u sourcePos, scr_block_s *block, sval_u *devStatBlock );
void EmitFormalParameterList( sval_u exprlist, sval_u sourcePos, scr_block_s *block );
void Scr_CalcLocalVarsFormalParameterList( sval_u exprlist, scr_block_s *block );
void SpecifyThread( sval_u val );
void EmitThreadInternal( unsigned int threadId, sval_u val, sval_u sourcePos, sval_u endSourcePos, scr_block_s *block );
void Scr_CalcLocalVarsThread( sval_u exprlist, sval_u stmtlist, sval_u *stmttblock );
void InitThread( int type );
void EmitNormalThread( sval_u val, sval_u *stmttblock );
void EmitDeveloperThread( sval_u val, sval_u *stmttblock );
void EmitThread( sval_u val );
void EmitThreadList( sval_u val );
void EmitInclude( sval_u val );
void EmitIncludeList( sval_u val );
void ScriptCompile( sval_u val, unsigned int fileId, unsigned int scriptId );
void Scr_CompileStatement( sval_u parseData );
void Scr_CompileShutdown();
void AddRefToValue( VariableValue *value );

/*
============
EmitSwitchStatementList
============
*/

/*
============
EmitStatement
============
*/

/*
============
Scr_CalcLocalVarsStatement
============
*/

/*
============
EmitStatementList
============
*/

/*
============
Scr_CompileRemoveRefToString
============
*/
void Scr_CompileRemoveRefToString( unsigned int stringValue )
{
	assert(stringValue);

	if ( scrCompileGlob.bConstRefCount )
	{
		return;
	}

	SL_RemoveRefToString(stringValue);
}

/*
============
EmitCanonicalString
============
*/
void EmitCanonicalString( unsigned int stringValue )
{
	assert(stringValue);
	scrCompileGlob.codePos = (byte *)TempMallocAlign( sizeof( unsigned short ) );

	if ( scrCompilePub.developer_statement == SCR_DEV_IGNORE )
	{
		assert(!scrVarPub.developer_script);
		Scr_CompileRemoveRefToString(stringValue);
		return;
	}

	if ( scrCompileGlob.bConstRefCount )
	{
		SL_AddRefToString(stringValue);
	}

	*(unsigned short *)scrCompileGlob.codePos = SL_TransferToCanonicalString(stringValue);
}

/*
============
EmitCanonicalStringConst
============
*/
void EmitCanonicalStringConst( unsigned int stringValue )
{
	bool bConstRefCount = scrCompileGlob.bConstRefCount;
	scrCompileGlob.bConstRefCount = true;

	EmitCanonicalString(stringValue);
	scrCompileGlob.bConstRefCount = bConstRefCount;
}

/*
============
CompileTransferRefToString
============
*/
void CompileTransferRefToString( unsigned int stringValue, unsigned int user )
{
	assert(stringValue);

	if ( scrCompilePub.developer_statement == SCR_DEV_IGNORE )
	{
		Scr_CompileRemoveRefToString(stringValue);
		return;
	}

	if ( scrCompileGlob.bConstRefCount )
	{
		SL_AddRefToString(stringValue);
	}

	SL_TransferRefToUser(stringValue, user);
}

/*
============
EmitOpcode
============
*/
void EmitOpcode( unsigned int op, int offset, int callType )
{
	int i;
	int value_count;
	unsigned int index;

	if ( scrCompilePub.value_count )
	{
		value_count = scrCompilePub.value_count;
		scrCompilePub.value_count = 0;

		for ( i = 0; i < value_count; i++ )
		{
			EmitValue(&scrCompileGlob.value_start[i]);
		}
	}

	if ( !scrCompileGlob.cumulOffset || callType == CALL_THREAD || callType == CALL_FUNCTION )
	{
		scrCompilePub.allowedBreakpoint = true;
	}
	else
	{
		scrCompilePub.allowedBreakpoint = false;
	}

	scrCompileGlob.cumulOffset += offset;

	if ( scrCompileGlob.maxOffset < scrCompileGlob.cumulOffset )
	{
		scrCompileGlob.maxOffset = scrCompileGlob.cumulOffset;
	}

	if ( callType != CALL_NONE && scrCompileGlob.maxCallOffset < scrCompileGlob.cumulOffset )
	{
		scrCompileGlob.maxCallOffset = scrCompileGlob.cumulOffset;
	}

	scrVarPub.checksum *= 31;
	scrVarPub.checksum += op;

	if ( scrCompilePub.opcodePos )
	{
		scrCompileGlob.codePos = scrCompilePub.opcodePos;

		switch ( op )
		{
		case OP_EvalArray:
		if ( *scrCompilePub.opcodePos == OP_EvalLocalVariableCached )
		{
			RemoveOpcodePos();
			*scrCompilePub.opcodePos = OP_EvalLocalArrayCached;
			return;
		}

		index = *scrCompilePub.opcodePos - OP_EvalLocalVariableCached0;

		if ( index <= OP_GetNegByte )
		{
			RemoveOpcodePos();
			*scrCompilePub.opcodePos = OP_EvalLocalArrayCached;
			EmitByte(index);
			return;
		}
		else
		{
		}
		break;

	case OP_EvalArrayRef:
		if ( *scrCompilePub.opcodePos == OP_EvalLocalVariableRefCached )
		{
			RemoveOpcodePos();
			*scrCompilePub.opcodePos = OP_EvalLocalArrayRefCached;
			return;
		}

		if ( *scrCompilePub.opcodePos == OP_EvalLocalVariableRefCached0 )
		{
			RemoveOpcodePos();
			*scrCompilePub.opcodePos = OP_EvalLocalArrayRefCached0;
			return;
		}
		else
		{
		}
		break;

	case OP_EvalFieldVariable:
		if ( *scrCompilePub.opcodePos == OP_GetSelfObject )
		{
			*scrCompilePub.opcodePos = OP_EvalSelfFieldVariable;
			return;
		}

		if ( *scrCompilePub.opcodePos == OP_GetLevelObject )
		{
			*scrCompilePub.opcodePos = OP_EvalLevelFieldVariable;
			return;
		}

		if ( *scrCompilePub.opcodePos == OP_GetAnimObject )
		{
			*scrCompilePub.opcodePos = OP_EvalAnimFieldVariable;
			return;
		}
		else
		{
		}
		break;

	case OP_EvalFieldVariableRef:
		if ( *scrCompilePub.opcodePos == OP_GetSelfObject )
		{
			*scrCompilePub.opcodePos = OP_EvalSelfFieldVariableRef;
			return;
		}

		if ( *scrCompilePub.opcodePos == OP_GetLevelObject )
		{
			*scrCompilePub.opcodePos = OP_EvalLevelFieldVariableRef;
			return;
		}

		if ( *scrCompilePub.opcodePos == OP_GetAnimObject )
		{
			*scrCompilePub.opcodePos = OP_EvalAnimFieldVariableRef;
			return;
		}
		else
		{
		}
		break;

	case OP_SafeSetVariableFieldCached0:
		if ( *scrCompilePub.opcodePos == OP_CreateLocalVariable )
		{
			*scrCompilePub.opcodePos = OP_SafeCreateVariableFieldCached;
			return;
		}
		else
		{
		}
		break;

	case OP_SetVariableField:
		if ( *scrCompilePub.opcodePos == OP_EvalLocalVariableRefCached )
		{
			RemoveOpcodePos();
			*scrCompilePub.opcodePos = OP_SetLocalVariableFieldCached;
			return;
		}

		if ( *scrCompilePub.opcodePos == OP_EvalLocalVariableRefCached0 )
		{
			RemoveOpcodePos();
			*scrCompilePub.opcodePos = OP_SetLocalVariableFieldCached0;
			return;
		}

		if ( *scrCompilePub.opcodePos == OP_EvalSelfFieldVariableRef )
		{
			RemoveOpcodePos();
			*scrCompilePub.opcodePos = OP_SetSelfFieldVariableField;
			return;
		}

		if ( *scrCompilePub.opcodePos == OP_EvalLevelFieldVariableRef )
		{
			RemoveOpcodePos();
			*scrCompilePub.opcodePos = OP_SetLevelFieldVariableField;
			return;
		}

		if ( *scrCompilePub.opcodePos == OP_EvalAnimFieldVariableRef )
		{
			RemoveOpcodePos();
			*scrCompilePub.opcodePos = OP_SetAnimFieldVariableField;
			return;
		}
		else
		{
		}
		break;

	case OP_ScriptFunctionCall:
		if ( *scrCompilePub.opcodePos == OP_PreScriptCall )
		{
			*scrCompilePub.opcodePos = OP_ScriptFunctionCall2;
			return;
		}
		else
		{
		}
		break;

	case OP_ScriptMethodCall:
		if ( *scrCompilePub.opcodePos == OP_GetSelf )
		{
			RemoveOpcodePos();
			*scrCompilePub.opcodePos = OP_ScriptFunctionCall;
			assert(scrCompileGlob.prevOpcodePos);

			if ( *scrCompileGlob.prevOpcodePos == OP_PreScriptCall )
			{
				assert(scrCompilePub.opcodePos == (byte *)TempMalloc( 0 ) - 1);
				TempMemorySetPos((char*)scrCompilePub.opcodePos);
				--scrCompilePub.opcodePos;
				scrCompileGlob.prevOpcodePos = NULL;
				scrCompileGlob.codePos = scrCompilePub.opcodePos;
				*scrCompilePub.opcodePos = OP_ScriptFunctionCall2;
			}
			return;
		}
		else
		{
		}
		break;

	case OP_ScriptMethodThreadCall:
		if ( *scrCompilePub.opcodePos == OP_GetSelf )
		{
			RemoveOpcodePos();
			*scrCompilePub.opcodePos = OP_ScriptThreadCall;
			return;
		}
		else
		{
		}
		break;

	case OP_CastFieldObject:
		if ( *scrCompilePub.opcodePos == OP_EvalLocalVariableCached )
		{
			*scrCompilePub.opcodePos = OP_EvalLocalVariableObjectCached;
			return;
		}

		index = *scrCompilePub.opcodePos - OP_EvalLocalVariableCached0;

		if ( index <= OP_GetNegByte )
		{
			*scrCompilePub.opcodePos = OP_EvalLocalVariableObjectCached;
			EmitByte(index);
			return;
		}
		else
		{
		}
		break;

	case OP_JumpOnFalse:
		if ( *scrCompilePub.opcodePos == OP_BoolNot )
		{
			RemoveOpcodePos();
			*scrCompilePub.opcodePos = OP_JumpOnTrue;
			return;
		}
		else
		{
		}
		break;
	}
	}

	scrCompileGlob.prevOpcodePos = scrCompilePub.opcodePos;
	scrCompilePub.opcodePos = (byte *)TempMalloc( sizeof( byte ) );
	scrCompileGlob.codePos = scrCompilePub.opcodePos;
	*scrCompilePub.opcodePos = op;
}

/*
============
EmitEnd
============
*/
void EmitEnd()
{
	EmitOpcode(OP_End, 0, CALL_NONE);
}

/*
============
EmitReturn
============
*/
void EmitReturn( void )
{
	EmitOpcode(OP_Return, -1, CALL_NONE);
}

/*
============
EmitCodepos
============
*/
void EmitInteger( int value )
{
	scrCompileGlob.codePos = (byte *)TempMallocAlign( sizeof( int ) );
	*(int *)scrCompileGlob.codePos = value;
}

/*
============
EmitShort
============
*/
void EmitShort( short value )
{
	scrCompileGlob.codePos = (byte *)TempMallocAlign( sizeof( short ) );
	*(short *)scrCompileGlob.codePos = value;
}

/*
============
EmitUnsignedShort
============
*/
void EmitUnsignedShort( unsigned short value )
{
	scrCompileGlob.codePos = (byte *)TempMallocAlign( sizeof( unsigned short ) );
	*(unsigned short *)scrCompileGlob.codePos = value;
}

/*
============
EmitByte
============
*/
void EmitByte( byte value )
{
	scrCompileGlob.codePos = (byte *)TempMalloc( sizeof( byte ) );
	*(byte *)scrCompileGlob.codePos = value;
}

/*
============
EmitFloat
============
*/
void EmitFloat( float value )
{
	scrCompileGlob.codePos = (byte *)TempMallocAlignStrict( sizeof( float ) );
	*(float *)scrCompileGlob.codePos = value;
}

/*
============
EmitString
============
*/
void EmitString( unsigned int value )
{
	scrCompileGlob.codePos = (byte *)TempMallocAlign( sizeof( unsigned short ) );
	*(unsigned short *)scrCompileGlob.codePos = value;
}

void EmitCodepos( const char *pos )
{
	scrCompileGlob.codePos = (byte *)TempMallocAlign( sizeof( const char * ) );
	*(const char **)scrCompileGlob.codePos = pos;
}

/*
============
EvalUndefined
============
*/
void EvalUndefined( sval_u sourcePos, VariableCompileValue *constValue )
{
	assert(constValue);
	constValue->value.type = VAR_UNDEFINED;
	constValue->sourcePos = sourcePos;
}

/*
============
EmitGetUndefined
============
*/
void EmitGetUndefined( sval_u sourcePos )
{
	EmitOpcode( OP_GetUndefined, 1, CALL_NONE );
	AddOpcodePos( sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT );
}

/*
============
EvalInteger
============
*/
void EvalInteger( int value, sval_u sourcePos, VariableCompileValue *constValue )
{
	assert(constValue);
	constValue->value.type = VAR_INTEGER;
	constValue->value.u.intValue = value;
	constValue->sourcePos = sourcePos;
}

/*
============
EmitGetInteger
============
*/
void EmitGetInteger( int value, sval_u sourcePos )
{
	if ( value >= 0 )
	{
		if ( value == 0 )
		{
			EmitOpcode(OP_GetZero, 1, CALL_NONE);
			AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
			return;
		}

		if ( value < UCHAR_MAX + 1 )
		{
			EmitOpcode(OP_GetByte, 1, CALL_NONE);
			AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
			EmitByte(value);
			return;
		}

		if ( value < USHRT_MAX + 1 )
		{
			EmitOpcode(OP_GetUnsignedShort, 1, CALL_NONE);
			AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
			EmitUnsignedShort(value);
			return;
		}
	}
	else
	{
		if ( value > -( UCHAR_MAX + 1 ) )
		{
			EmitOpcode(OP_GetNegByte, 1, CALL_NONE);
			AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
			EmitByte(-value);
			return;
		}

		if ( value > -( USHRT_MAX + 1 ) )
		{
			EmitOpcode(OP_GetNegUnsignedShort, 1, CALL_NONE);
			AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
			EmitUnsignedShort(-value);
			return;
		}
	}

	EmitOpcode(OP_GetInteger, 1, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
	EmitInteger(value);
}

/*
============
EvalFloat
============
*/
void EvalFloat( float value, sval_u sourcePos, VariableCompileValue *constValue )
{
	assert(constValue);
	constValue->value.type = VAR_FLOAT;
	constValue->value.u.floatValue = value;
	constValue->sourcePos = sourcePos;
}

/*
============
EmitGetFloat
============
*/
void EmitGetFloat( float value, sval_u sourcePos )
{
	EmitOpcode(OP_GetFloat, 1, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
	EmitFloat(value);
}

/*
============
EmitAnimTree
============
*/
void EmitAnimTree( sval_u sourcePos )
{
	if ( !scrAnimPub.animTreeIndex )
	{
		CompileError(sourcePos.sourcePosValue, "#using_animtree was not specified");
		return;
	}

	EmitGetInteger(scrAnimPub.animTreeIndex, sourcePos);
}

/*
============
EmitSetVariableField
============
*/
void EmitSetVariableField( sval_u sourcePos )
{
	EmitOpcode(OP_SetVariableField, -1, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_NONE);
}

/*
============
Scr_FindLocalVar
============
*/
int Scr_FindLocalVar( scr_block_s *block, int startIndex, unsigned int name )
{
	for ( int i = startIndex; i < block->localVarsCount; i++ )
	{
		if ( block->localVars[i].name == name )
		{
			return i;
		}
	}

	return -1;
}

/*
============
Scr_FindLocalVarIndex
============
*/
int Scr_FindLocalVarIndex( unsigned int name, sval_u sourcePos, bool create, scr_block_s *block )
{
	int i;
	byte mask;

	if ( !block )
	{
		goto unreachable;
	}

	for ( i = 0; i < block->localVarsCount; i++ )
	{
		if ( i == block->localVarsCreateCount )
		{
			block->localVarsCreateCount++;

			EmitOpcode(OP_CreateLocalVariable, 0, CALL_NONE);
			EmitCanonicalStringConst(block->localVars[i].name);
		}

		if ( block->localVars[i].name != name )
		{
			continue;
		}

		Scr_CompileRemoveRefToString(name);
		mask = 1 << ( i & 7 );

		if ( !( block->localVarsInitBits[ i >> 3 ] & mask ) )
		{
			if ( !create || scrCompileGlob.forceNotCreate )
			{
				break;
			}

			block->localVarsInitBits[ i >> 3 ] |= mask;
		}

		return block->localVarsCreateCount - i - 1;
	}

	if ( !create || scrCompileGlob.forceNotCreate )
	{
		CompileError(sourcePos.sourcePosValue, "uninitialised variable '%s'", SL_ConvertToString(name));
		return 0;
	}

unreachable:
	CompileError(sourcePos.sourcePosValue, "unreachable code");
	return 0;
}

/*
============
EmitCreateLocalVars
============
*/
void EmitCreateLocalVars( scr_block_s *block, sval_u sourcePos )
{
	assert(block->localVarsPublicCount >= block->localVarsCreateCount);

	if ( block->localVarsCreateCount == block->localVarsPublicCount )
	{
		return;
	}

	for ( int i = block->localVarsCreateCount; i < block->localVarsPublicCount; i++ )
	{
		EmitOpcode(OP_CreateLocalVariable, 0, CALL_NONE);
		EmitCanonicalStringConst(block->localVars[i].name);
	}

	block->localVarsCreateCount = block->localVarsPublicCount;
}

/*
============
EmitRemoveLocalVars
============
*/
void EmitRemoveLocalVars( scr_block_s *block, scr_block_s *outerBlock )
{
	if ( block->abortLevel != SCR_ABORT_NONE )
	{
		return;
	}

	assert(block->localVarsCreateCount >= block->localVarsPublicCount);
	assert(block->localVarsPublicCount >= outerBlock->localVarsPublicCount);

	int removeCount = block->localVarsCreateCount - outerBlock->localVarsPublicCount;
	assert(removeCount >= 0);

	if ( !removeCount )
	{
		return;
	}

	EmitOpcode(OP_RemoveLocalVariables, 0, CALL_NONE);
	EmitByte(removeCount);
	block->localVarsCreateCount = block->localVarsPublicCount;
}

/*
============
EmitNOP2
============
*/
void EmitNOP2( bool lastStatement, unsigned int endSourcePos, scr_block_s *block )
{
	int checksum = scrVarPub.checksum;

	if ( lastStatement )
	{
		EmitEnd();
		AddOpcodePos(endSourcePos, SOURCE_TYPE_BREAKPOINT);
	}
	else
	{
		EmitRemoveLocalVars(block, block);
	}

	scrVarPub.checksum = checksum + 1;
}

/*
============
Scr_CheckMaxSwitchCases
============
*/
void Scr_CheckMaxSwitchCases( int count )
{
	if ( count < MAX_SWITCH_CASES )
	{
		return;
	}

	Com_Error(ERR_DROP, "MAX_SWITCH_CASES exceeded");
}

/*
============
Scr_CheckLocalVarsCount
============
*/
void Scr_CheckLocalVarsCount( int localVarsCount )
{
	if ( localVarsCount < LOCAL_VAR_STACK_SIZE )
	{
		return;
	}

	Com_Error(ERR_DROP, "LOCAL_VAR_STACK_SIZE exceeded");
}

/*
============
Scr_RegisterLocalVar
============
*/
void Scr_RegisterLocalVar( unsigned int name, sval_u sourcePos, scr_block_s *block )
{
	if ( block->abortLevel != SCR_ABORT_NONE )
	{
		return;
	}

	for ( int i = 0; i < block->localVarsCount; i++ )
	{
		if ( block->localVars[i].name == name )
		{
			return;
		}
	}

	Scr_CheckLocalVarsCount(block->localVarsCount);

	block->localVars[block->localVarsCount].name = name;
	// block->localVars[block->localVarsCount].sourcePos = sourcePos.sourcePosValue;

	block->localVarsCount++;
}

/*
============
Scr_CopyBlock
============
*/
void Scr_CopyBlock( scr_block_s *from, scr_block_s **to )
{
	if ( *to == NULL )
	{
		*to = (scr_block_s *)Hunk_AllocateTempMemoryHighInternal( sizeof( **to ) );
	}

	**to = *from;
	to[0]->localVarsPublicCount = 0;
}

/*
============
Scr_InitFromChildBlocks
============
*/
void Scr_InitFromChildBlocks( scr_block_s **childBlocks, int childCount, scr_block_s *block )
{
	int i, childIndex;
	scr_block_s *childBlock;
	unsigned int name;
	int localVarsCreateCount;

	if ( !childCount )
	{
		return;
	}

	localVarsCreateCount = childBlocks[0]->localVarsPublicCount;

	for ( childIndex = 1; childIndex < childCount; childIndex++ )
	{
		childBlock = childBlocks[childIndex];

		if ( childBlock->localVarsPublicCount < localVarsCreateCount )
		{
			localVarsCreateCount = childBlock->localVarsPublicCount;
		}
	}

	assert(block->localVarsCreateCount <= localVarsCreateCount);
	assert(localVarsCreateCount <= block->localVarsCount);

	block->localVarsCreateCount = localVarsCreateCount;

	for ( i = 0; i < localVarsCreateCount; i++ )
	{
		assert(i < block->localVarsCount);

		if ( ( 1 << ( i & 7 ) ) & block->localVarsInitBits[ i >> 3 ] )
		{
			continue;
		}

		name = block->localVars[i].name;

		for ( childIndex = 0; childIndex < childCount; childIndex++ )
		{
			childBlock = childBlocks[childIndex];

			assert(localVarsCreateCount <= childBlock->localVarsPublicCount);
			assert(i < childBlock->localVarsPublicCount);
			assert(childBlock->localVars[i].name == name);

			if ( !( ( 1 << ( i & 7 ) ) & childBlock->localVarsInitBits[ i >> 3 ] ) )
			{
				goto out;
			}
		}

		block->localVarsInitBits[ i >> 3 ] |= 1 << ( i & 7 );

out:
		;
	}
}

/*
============
Scr_AppendChildBlocks
============
*/
void Scr_AppendChildBlocks( scr_block_s **childBlocks, int childCount, scr_block_s *block )
{
	int i;
	int childIndex;
	unsigned int name;
	scr_block_s *childBlock;

	if ( childCount )
	{
		if ( !block->abortLevel )
		{
			for ( childIndex = 0; childIndex < childCount; childIndex++ )
			{
				childBlock = childBlocks[childIndex];
				childBlock->abortLevel = SCR_ABORT_NONE;
			}

			for ( i = 0; i < childBlocks[0]->localVarsCount; i++ )
			{
				name = *(unsigned int *)( (char *)childBlocks[0] + sizeof( scr_localVar_t ) * i + offsetof( scr_block_s, localVars ) );

				if ( Scr_FindLocalVar(block, 0, name) >= 0 )
				{
					continue;
				}

				for ( childIndex = 1; childIndex < childCount; childIndex++ )
				{
					if ( Scr_FindLocalVar(childBlocks[childIndex], 0, name) < 0 )
					{
						goto out;
					}
				}

				*(unsigned int *)( (char *)block + sizeof( scr_localVar_t ) * block->localVarsCount + offsetof( scr_block_s, localVars ) ) = name;
				block->localVarsCount++;
out:
				;
			}
		}
	}
}

/*
============
Scr_MergeChildBlocks
============
*/
void Scr_MergeChildBlocks( scr_block_s **childBlocks, int childCount, scr_block_s *block )
{
	int i;
	int j;
	int childIndex;
	unsigned int name;
	scr_block_s *childBlock;

	if ( childCount )
	{
		if ( !block->abortLevel )
		{
			for ( childIndex = 0; childIndex < childCount; childIndex++ )
			{
				childBlock = childBlocks[childIndex];
				childBlock->localVarsPublicCount = block->localVarsCount;

				for ( i = 0; i < block->localVarsCount; i++ )
				{
					name = *(unsigned int *)( (char *)block + sizeof( scr_localVar_t ) * i + offsetof( scr_block_s, localVars ) );
					j = Scr_FindLocalVar(childBlock, i, name);

					if ( j < 0 )
					{
						j = childBlock->localVarsCount;

						Scr_CheckLocalVarsCount(childBlock->localVarsCount);
						childBlock->localVarsCount++;
					}

					while ( j > i )
					{
						*(unsigned int *)( (char *)childBlock + sizeof( scr_localVar_t ) * j + offsetof( scr_block_s, localVars ) ) =
							*(unsigned int *)( (char *)childBlock + sizeof( scr_localVar_t ) * j + offsetof( scr_block_s, localVarsInitBits ) + sizeof( unsigned int ) );
						j--;
					}

					*(unsigned int *)( (char *)childBlock + sizeof( scr_localVar_t ) * i + offsetof( scr_block_s, localVars ) ) = name;
				}
			}
		}
	}
}

/*
============
Scr_TransferBlock
============
*/
void Scr_TransferBlock( scr_block_s *from, scr_block_s *to )
{
	int i;
	int j;
	unsigned int name;

	for ( i = 0; i < to->localVarsPublicCount || i < from->localVarsCreateCount; i++ )
	{
		name = *(unsigned int *)( (char *)from + sizeof( scr_localVar_t ) * i + offsetof( scr_block_s, localVars ) );
		j = Scr_FindLocalVar(to, i, name);

		if ( j < 0 )
		{
			j = to->localVarsCount;

			Scr_CheckLocalVarsCount(to->localVarsCount);
			to->localVarsCount++;
		}

		if ( j >= to->localVarsPublicCount )
		{
			to->localVarsPublicCount++;
		}

		while ( j > i )
		{
			*(unsigned int *)( (char *)to + sizeof( scr_localVar_t ) * j + offsetof( scr_block_s, localVars ) ) =
				*(unsigned int *)( (char *)to + sizeof( scr_localVar_t ) * j + offsetof( scr_block_s, localVarsInitBits ) + sizeof( unsigned int ) );
			j--;
		}

		*(unsigned int *)( (char *)to + sizeof( scr_localVar_t ) * i + offsetof( scr_block_s, localVars ) ) = name;

		if ( ( 1 << ( i & 7 ) ) & from->localVarsInitBits[ i >> 3 ] )
		{
			to->localVarsInitBits[ i >> 3 ] |= 1 << ( i & 7 );
		}
	}

	to->localVarsCreateCount = from->localVarsCreateCount;
	to->abortLevel = SCR_ABORT_NONE;
}

/*
============
EmitSafeSetVariableField
============
*/
void EmitSafeSetVariableField( sval_u expr, sval_u sourcePos, scr_block_s *block )
{
	int index = Scr_FindLocalVarIndex(expr.idValue, sourcePos, true, block);

	EmitOpcode(index ? OP_SafeSetVariableFieldCached : OP_SafeSetVariableFieldCached0, 0, CALL_NONE);

	if ( index )
	{
		EmitByte(index);
	}

	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_NONE);
}

void Scr_CalcLocalVarsSafeSetVariableField( sval_u expr, sval_u sourcePos, scr_block_s *block )
{
	Scr_RegisterLocalVar(expr.idValue, sourcePos, block);
}

/*
============
EmitSafeSetWaittillVariableField
============
*/
void EmitSafeSetWaittillVariableField( sval_u expr, sval_u sourcePos, scr_block_s *block )
{
	int index = Scr_FindLocalVarIndex(expr.idValue, sourcePos, true, block);

	EmitOpcode(OP_SafeSetWaittillVariableFieldCached, 0, CALL_NONE);
	EmitByte(index);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_NONE);
}

/*
============
EvalString
============
*/
void EvalString( unsigned int value, sval_u sourcePos, VariableCompileValue *constValue )
{
	assert(constValue);
	constValue->value.type = VAR_STRING;
	constValue->value.u.stringValue = value;
	constValue->sourcePos = sourcePos;
}

/*
============
EmitGetString
============
*/
void EmitGetString( unsigned int value, sval_u sourcePos )
{
	EmitOpcode(OP_GetString, 1, CALL_NONE);
	AddOpcodePos(sourcePos.stringValue, 1);
	EmitString(value);
	CompileTransferRefToString(value, 1);
}

/*
============
EvalIString
============
*/
void EvalIString( unsigned int value, sval_u sourcePos, VariableCompileValue *constValue )
{
	assert(constValue);
	constValue->value.type = VAR_ISTRING;
	constValue->value.u.stringValue = value;
	constValue->sourcePos = sourcePos;
}

/*
============
EmitGetIString
============
*/
void EmitGetIString( unsigned int value, sval_u sourcePos )
{
	EmitOpcode(OP_GetIString, 1, CALL_NONE);
	AddOpcodePos(sourcePos.stringValue, SOURCE_TYPE_BREAKPOINT);
	EmitString(value);
	CompileTransferRefToString(value, 1);
}

/*
============
EmitGetVector
============
*/
void EmitGetVector( const vec3_t value, sval_u sourcePos )
{
	EmitOpcode(OP_GetVector, 1, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);

	for ( int i = 0; i < 3; i++ )
	{
		EmitFloat(value[i]);
	}

	RemoveRefToVector(value);
}

/*
============
EmitValue
============
*/
void EmitValue( VariableCompileValue *constValue )
{
	switch ( constValue->value.type )
	{
	case VAR_UNDEFINED:
		EmitGetUndefined(constValue->sourcePos);
		break;

	case VAR_INTEGER:
		EmitGetInteger(constValue->value.u.intValue, constValue->sourcePos);
		break;

	case VAR_FLOAT:
		EmitGetFloat(constValue->value.u.floatValue, constValue->sourcePos);
		break;

	case VAR_STRING:
		EmitGetString(constValue->value.u.stringValue, constValue->sourcePos);
		break;

	case VAR_ISTRING:
		EmitGetIString(constValue->value.u.stringValue, constValue->sourcePos);
		break;

	case VAR_VECTOR:
		EmitGetVector(constValue->value.u.vectorValue, constValue->sourcePos);
		break;

	default:
		break;
	}
}

/*
============
Scr_PushValue
============
*/
void Scr_PushValue( VariableCompileValue *constValue )
{
	if ( scrCompilePub.value_count >= VALUE_STACK_SIZE )
	{
		CompileError(constValue->sourcePos.sourcePosValue, "VALUE_STACK_SIZE exceeded");
		return;
	}

	scrCompileGlob.value_start[scrCompilePub.value_count] = *constValue;
	scrCompilePub.value_count++;
}

/*
============
Scr_PopValue
============
*/
void Scr_PopValue()
{
	assert(scrCompilePub.value_count);
	scrCompilePub.value_count--;
}

/*
============
EmitCastBool
============
*/
void EmitCastBool( sval_u sourcePos )
{
	EmitOpcode(OP_CastBool, 0, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_NONE);
}

/*
============
EmitBoolNot
============
*/
void EmitBoolNot( sval_u sourcePos )
{
	EmitOpcode(OP_BoolNot, 0, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_NONE);
}

/*
============
EmitBoolComplement
============
*/
void EmitBoolComplement( sval_u sourcePos )
{
	EmitOpcode(OP_BoolComplement, 0, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_NONE);
}

/*
============
EmitSize
============
*/
void EmitSize( sval_u expr, sval_u sourcePos, scr_block_s *block )
{
	EmitPrimitiveExpression(expr, block);
	EmitOpcode(OP_size, 0, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_NONE);
}

/*
============
EmitSelf
============
*/
void EmitSelf( sval_u sourcePos )
{
	EmitOpcode(OP_GetSelf, 1, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
}

/*
============
EmitLevel
============
*/
void EmitLevel( sval_u sourcePos )
{
	EmitOpcode(OP_GetLevel, 1, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
}

/*
============
EmitGame
============
*/
void EmitGame( sval_u sourcePos )
{
	EmitOpcode(OP_GetGame, 1, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
}

/*
============
EmitAnim
============
*/
void EmitAnim( sval_u sourcePos )
{
	EmitOpcode(OP_GetAnim, 1, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
}

/*
============
EmitSelfObject
============
*/
void EmitSelfObject( sval_u sourcePos )
{
	EmitOpcode(OP_GetSelfObject, 0, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
}

/*
============
EmitLevelObject
============
*/
void EmitLevelObject( sval_u sourcePos )
{
	EmitOpcode(OP_GetLevelObject, 0, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
}

/*
============
EmitAnimObject
============
*/
void EmitAnimObject( sval_u sourcePos )
{
	EmitOpcode(OP_GetAnimObject, 0, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
}

/*
============
EmitLocalVariable
============
*/
void EmitLocalVariable( sval_u expr, sval_u sourcePos, scr_block_s *block )
{
	int index = Scr_FindLocalVarIndex(expr.idValue, sourcePos, false, block);
	int opcode;

	if ( index <= 5 )
	{
		opcode = OP_EvalLocalVariableCached0 + index;
	}
	else
	{
		opcode = OP_EvalLocalVariableCached;
	}

	EmitOpcode(opcode, 1, CALL_NONE);

	if ( opcode == OP_EvalLocalVariableCached )
	{
		EmitByte(index);
	}

	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
}

/*
============
EmitLocalVariableRef
============
*/
void EmitLocalVariableRef( sval_u expr, sval_u sourcePos, scr_block_s *block )
{
	int index = Scr_FindLocalVarIndex(expr.idValue, sourcePos, true, block);

	EmitOpcode(index ? OP_EvalLocalVariableRefCached : OP_EvalLocalVariableRefCached0, 0, CALL_NONE);

	if ( index )
	{
		EmitByte(index);
	}

	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
}

/*
============
Scr_CalcLocalVarsSafeSetVariableField
============
*/
void Scr_CalcLocalVarsLocalVariableRef( sval_u expr, sval_u sourcePos, scr_block_s *block )
{
	Scr_RegisterLocalVar(expr.idValue, sourcePos, block);
}

/*
============
EmitGameRef
============
*/
void EmitGameRef( sval_u sourcePos )
{
	EmitOpcode(OP_GetGameRef, 0, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
}

/*
============
EmitEvalArray
============
*/
void EmitEvalArray( sval_u sourcePos, sval_u indexSourcePos )
{
	EmitOpcode(OP_EvalArray, -1, CALL_NONE);
	AddOpcodePos(indexSourcePos.sourcePosValue, SOURCE_TYPE_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
}

/*
============
EmitEvalArrayRef
============
*/
void EmitEvalArrayRef( sval_u sourcePos, sval_u indexSourcePos )
{
	EmitOpcode(OP_EvalArrayRef, -1, CALL_NONE);
	AddOpcodePos(indexSourcePos.sourcePosValue, SOURCE_TYPE_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
}

/*
============
EmitClearArray
============
*/
void EmitClearArray( sval_u sourcePos, sval_u indexSourcePos )
{
	EmitOpcode(OP_ClearArray, -1, CALL_NONE);
	AddOpcodePos(indexSourcePos.sourcePosValue, SOURCE_TYPE_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_NONE);
}

/*
============
EmitEmptyArray
============
*/
void EmitEmptyArray( sval_u sourcePos )
{
	EmitOpcode(OP_EmptyArray, 1, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
}

/*
============
EmitAnimation
============
*/
void EmitAnimation( sval_u anim, sval_u sourcePos )
{
	EmitOpcode(OP_GetAnimation, 1, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
	EmitInteger(-1);

	Scr_EmitAnimation((char *)scrCompileGlob.codePos, anim.stringValue, sourcePos.sourcePosValue);
	Scr_CompileRemoveRefToString(anim.stringValue);
}

/*
============
EmitFieldVariable
============
*/
void EmitFieldVariable( sval_u expr, sval_u field, sval_u sourcePos, scr_block_s *block )
{
	EmitPrimitiveExpressionFieldObject(expr, sourcePos, block);
	EmitOpcode(OP_EvalFieldVariable, 1, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_NONE);
	EmitCanonicalString(field.stringValue);
}

/*
============
EmitFieldVariableRef
============
*/
void EmitFieldVariableRef( sval_u expr, sval_u field, sval_u sourcePos, scr_block_s *block )
{
	EmitPrimitiveExpressionFieldObject(expr, sourcePos, block);
	EmitOpcode(OP_EvalFieldVariableRef, 0, CALL_NONE);
	EmitCanonicalString(field.stringValue);
}

/*
============
EmitClearFieldVariable
============
*/
void EmitClearFieldVariable( sval_u expr, sval_u field, sval_u sourcePos, sval_u rhsSourcePos, scr_block_s *block )
{
	EmitPrimitiveExpressionFieldObject(expr, sourcePos, block);
	EmitOpcode(OP_ClearFieldVariable, 0, CALL_NONE);
	AddOpcodePos(rhsSourcePos.sourcePosValue, SOURCE_TYPE_NONE);
	EmitCanonicalString(field.stringValue);
}

/*
============
EmitObject
============
*/
void EmitObject( sval_u expr, sval_u sourcePos )
{
	int classnum, entnum;
	unsigned int idValue;
	unsigned short id;
	const char *s;

	if ( scrCompilePub.script_loading )
	{
		CompileError(sourcePos.sourcePosValue, "$ can only be used in the script debugger");
		return;
	}

	s = SL_ConvertToString(expr.stringValue);

	if ( s[0] == 't' )
	{
		idValue = atoi(s + 1);

		if ( idValue && idValue < VARIABLELIST_CHILD_SIZE )
		{
			id = idValue;

			if ( !IsObjectFree(id) )
			{
				switch ( (int)GetObjectType(id) )
				{
				case VAR_THREAD:
				case VAR_NOTIFY_THREAD:
				case VAR_TIME_THREAD:
				case VAR_CHILD_THREAD:
				case VAR_DEAD_THREAD:
					EmitOpcode(OP_thread_object, 1, CALL_NONE);
					EmitShort(id);
					return;
				}
			}
		}

		CompileError(sourcePos.sourcePosValue, "bad expression");
		return;
	}
	else
	{
		classnum = Scr_GetClassnumForCharId(s[0]);

		if ( classnum < 0 )
		{
			CompileError(sourcePos.sourcePosValue, "bad expression");
			return;
		}

		entnum = atoi(s + 1);

		if ( entnum == 0 && s[1] != '0' )
		{
			CompileError(sourcePos.sourcePosValue, "bad expression");
			return;
		}

		EmitOpcode(OP_object, 1, CALL_NONE);

		EmitInteger(classnum);
		EmitInteger(entnum);
	}
}

/*
============
EmitDecTop
============
*/
void EmitDecTop()
{
	EmitOpcode(OP_DecTop, -1, CALL_NONE);
}

/*
============
EmitCastFieldObject
============
*/
void EmitCastFieldObject( sval_u sourcePos )
{
	EmitOpcode(OP_CastFieldObject, -1, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_NONE);
}

/*
============
EmitArrayVariable
============
*/
void EmitArrayVariable( sval_u expr, sval_u index, sval_u sourcePos, sval_u indexSourcePos, scr_block_s *block )
{
	EmitExpression(index, block);
	EmitPrimitiveExpression(expr, block);
	EmitEvalArray(sourcePos, indexSourcePos);
}

/*
============
EmitArrayVariableRef
============
*/
void EmitArrayVariableRef( sval_u expr, sval_u index, sval_u sourcePos, sval_u indexSourcePos, scr_block_s *block )
{
	EmitExpression(index, block);
	EmitArrayPrimitiveExpressionRef(expr, sourcePos, block);
	EmitEvalArrayRef(sourcePos, indexSourcePos);
}

/*
============
Scr_CalcLocalVarsArrayVariableRef
============
*/
void Scr_CalcLocalVarsArrayVariableRef( sval_u expr, scr_block_s *block )
{
	Scr_CalcLocalVarsArrayPrimitiveExpressionRef(expr, block);
}

/*
============
EmitClearArrayVariable
============
*/
void EmitClearArrayVariable( sval_u expr, sval_u index, sval_u sourcePos, sval_u indexSourcePos, scr_block_s *block )
{
	EmitExpression(index, block);
	EmitArrayPrimitiveExpressionRef(expr, sourcePos, block);
	EmitClearArray(sourcePos, indexSourcePos);
}

/*
============
EmitVariableExpression
============
*/
void EmitVariableExpression( sval_u expr, scr_block_s *block )
{
	switch ( expr.node[0].type )
	{
	case ENUM_local_variable:
		EmitLocalVariable(expr.node[1], expr.node[2], block);
		break;

	case ENUM_array_variable:
		EmitArrayVariable(expr.node[1], expr.node[2], expr.node[3], expr.node[4], block);
		break;

	case ENUM_field_variable:
		EmitFieldVariable(expr.node[1], expr.node[2], expr.node[3], block);
		break;

	// 1.0's node-type enum has no ENUM_vector: ENUM_object is 0x4F here, one below
	// the CoD2rev (1.3) value. Target: cmp DWORD PTR [ebp-0x8],0x4f
	case ENUM_object - 1:
		EmitObject(expr.node[1], expr.node[2]);
		break;

	case ENUM_self_field:
		CompileError(expr.node[2].sourcePosValue, !scrCompilePub.script_loading
			? "self field in assignment expression not currently supported"
			: "self field can only be used in the script debugger");
		break;
	}
}

/*
============
GetExpressionCount
============
*/
int GetExpressionCount( sval_u exprlist )
{
	sval_u *node;
	int expr_count = 0;

	for ( node = exprlist.node[0].node; node; node = node[1].node )
	{
		expr_count++;
	}

	return expr_count;
}

/*
============
EmitExpressionList
============
*/
int EmitExpressionList( sval_u exprlist, scr_block_s *block )
{
	sval_u *node;
	int expr_count = 0;

	for ( node = exprlist.node[0].node; node; node = node[1].node )
	{
		EmitExpression(node[0].node[0], block);
		expr_count++;
	}

	return expr_count;
}

/*
============
GetSingleParameter
============
*/
sval_u* GetSingleParameter( sval_u exprlist )
{
	sval_u *node;

	node = exprlist.node[0].node;

	if ( node == 0 )
	{
		return 0;
	}

	if ( node[1].node == 0 )
	{
		return node;
	}

	return 0;
}

/*
============
AddExpressionListOpcodePos
============
*/
void AddExpressionListOpcodePos( sval_u exprlist )
{
	if ( !scrVarPub.developer )
	{
		return;
	}

	for ( sval_u *node = exprlist.node[0].node; node; node = node[1].node )
	{
		AddOpcodePos( node[0].node[1].sourcePosValue, SOURCE_TYPE_NONE );
	}
}

/*
============
AddFilePrecache
============
*/
unsigned int AddFilePrecache( unsigned int filename, unsigned int sourcePos, bool include )
{
	assert(scrCompileGlob.precachescriptList);

	SL_AddRefToString(filename);
	Scr_CompileRemoveRefToString(filename);

	scrCompileGlob.precachescriptList->filename = filename;
	scrCompileGlob.precachescriptList->sourcePos = sourcePos;
	scrCompileGlob.precachescriptList->include = include;

	scrCompileGlob.precachescriptList++;

	return GetObjectA( GetVariable( scrCompilePub.scriptsPos, filename ) );
}

/*
============
EmitFunction
============
*/
void EmitFunction(sval_u func, sval_u sourcePos)
{
	VariableValue count;
	VariableValue value;
	unsigned int threadId;
	unsigned int valueId;
	unsigned int countId;
	unsigned int newValueId;
	unsigned int fileId;
	unsigned int filename;
	bool bExists;
	unsigned int posId;
	VariableValue pos;
	int scope;

	if ( scrCompilePub.developer_statement == SCR_DEV_IGNORE )
	{
		Scr_CompileRemoveRefToString(func.node[1].stringValue);

		if ( func.node[0].type == ENUM_far_function )
		{
			Scr_CompileRemoveRefToString(func.node[2].stringValue);
			scrCompilePub.far_function_count--;
		}

		return;
	}

	threadId = 0;

	switch ( func.node[0].type )
	{
	case ENUM_local_function:
		scope = FUNC_SCOPE_LOCAL;
		valueId = GetVariable(scrCompileGlob.fileId, func.node[1].idValue);

		CompileTransferRefToString(func.node[1].stringValue, 2);

		threadId = GetObjectA(valueId);

		goto emit_function;
	}

	scope = FUNC_SCOPE_FAR;

	filename = Scr_CreateCanonicalFilename( SL_ConvertToString( func.node[1].stringValue ) );
	Scr_CompileRemoveRefToString( func.node[1].stringValue );

	value = Scr_EvalVariable( FindVariable( scrCompilePub.loadedscripts, filename ) );
	bExists = value.type != VAR_UNDEFINED;

	fileId = AddFilePrecache(filename, sourcePos.sourcePosValue, false);

	if ( bExists )
	{
		valueId = FindVariable(fileId, func.node[2].idValue);

		if ( !valueId )
		{
			CompileError(sourcePos.sourcePosValue, "unknown function");
			return;
		}

		if ( GetObjectType(valueId) != VAR_POINTER )
		{
			CompileError(sourcePos.sourcePosValue, "unknown function");
			return;
		}
	}
	else
	{
		valueId = GetVariable(fileId, func.node[2].idValue);
	}

	CompileTransferRefToString(func.node[2].stringValue, 2);

	threadId = GetObjectA(valueId);
	posId = FindVariable(threadId, 1);

	if ( !posId )
	{
		goto emit_function;
	}

	pos = Scr_EvalVariable(posId);

	if ( pos.type == VAR_INCLUDE_CODEPOS )
	{
		CompileError(sourcePos.sourcePosValue, "unknown function");
		return;
	}

	if ( !pos.u.codePosValue )
	{
		goto emit_function;
	}

	if ( pos.type != VAR_CODEPOS && scrCompilePub.developer_statement == SCR_DEV_NO )
	{
		CompileError(sourcePos.sourcePosValue, "normal script cannot reference a function in a /# ... #/ comment");
		return;
	}

	EmitCodepos(pos.u.codePosValue);
	return;

emit_function:
	EmitInteger(scope);

	countId = GetVariable(threadId, 0);
	count = Scr_EvalVariable(countId);

	if ( count.type == VAR_UNDEFINED )
	{
		count.type = VAR_INTEGER;
		count.u.intValue = 0;
	}

	newValueId = GetNewVariable(threadId, count.u.intValue + 2);
	value.u.codePosValue = (const char *)scrCompileGlob.codePos;

	if ( scrCompilePub.developer_statement != SCR_DEV_NO )
	{
		value.type = VAR_DEVELOPER_CODEPOS;
	}
	else
	{
		value.type = VAR_CODEPOS;
	}

	SetNewVariableValue(newValueId, &value);
	count.u.intValue++;

	SetVariableValue(countId, &count);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_NONE);
}

/*
============
EmitGetFunction
============
*/
void EmitGetFunction( sval_u func, sval_u sourcePos )
{
	EmitOpcode(OP_GetFunction, 1, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT | SOURCE_TYPE_CALL);
	EmitFunction(func, sourcePos);
}

/*
============
AddFunction
============
*/
int AddFunction( intptr_t func, const char *pName )
{
	int i;

	for ( i = 0; i < scrCompilePub.func_table_size; i++ )
	{
		if ( scrCompilePub.func_table[i] == func )
		{
			return i;
		}
	}

	assert(i == scrCompilePub.func_table_size);

	if ( scrCompilePub.func_table_size == SCR_FUNC_TABLE_SIZE )
	{
		Com_Error(ERR_DROP, "\x15" "SCR_FUNC_TABLE_SIZE exceeded");
	}

	scrCompilePub.func_table[scrCompilePub.func_table_size] = func;
	scrCompilePub.func_table_size++;

	return i;
}

/*
============
EmitPostScriptFunction
============
*/
void EmitPostScriptFunction( sval_u func, int param_count, bool bMethod, sval_u nameSourcePos )
{
	if ( !bMethod )
		EmitOpcode(OP_ScriptFunctionCall, -param_count, CALL_FUNCTION);
	else
		EmitOpcode(OP_ScriptMethodCall, -param_count - 1, CALL_FUNCTION);

	AddOpcodePos(nameSourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT | SOURCE_TYPE_CALL);
	EmitFunction(func, nameSourcePos);
}

/*
============
EmitPostScriptFunctionPointer
============
*/
void EmitPostScriptFunctionPointer( sval_u expr, int param_count, bool bMethod, sval_u nameSourcePos, sval_u sourcePos, scr_block_s *block )
{
	EmitExpression(expr, block);

	if ( !bMethod )
		EmitOpcode(OP_ScriptFunctionCallPointer, -param_count - 1, CALL_FUNCTION);
	else
		EmitOpcode(OP_ScriptMethodCallPointer, -param_count - 2, CALL_FUNCTION);

	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_NONE);
	AddOpcodePos(nameSourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
}

/*
============
EmitPostScriptThread
============
*/
void EmitPostScriptThread( sval_u func, int param_count, bool bMethod, sval_u sourcePos )
{
	if ( !bMethod )
		EmitOpcode(OP_ScriptThreadCall, 1 - param_count, CALL_THREAD);
	else
		EmitOpcode(OP_ScriptMethodThreadCall, -param_count, CALL_THREAD);

	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT | SOURCE_TYPE_CALL);
	EmitFunction(func, sourcePos);
	EmitInteger(param_count);
}

/*
============
EmitPostScriptThreadPointer
============
*/
void EmitPostScriptThreadPointer( sval_u expr, int param_count, bool bMethod, sval_u sourcePos, scr_block_s *block )
{
	EmitExpression(expr, block);

	if ( !bMethod )
		EmitOpcode(OP_ScriptThreadCallPointer, -param_count, CALL_THREAD);
	else
		EmitOpcode(OP_ScriptMethodThreadCallPointer, -param_count - 1, CALL_THREAD);

	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
	EmitInteger(param_count);
}

/*
============
EmitPostScriptFunctionCall
============
*/
void EmitPostScriptFunctionCall( sval_u func_name, int param_count, bool bMethod, sval_u nameSourcePos, scr_block_s *block )
{
	switch ( func_name.node[0].type )
	{
	case ENUM_function:
		EmitPostScriptFunction(func_name.node[1], param_count, bMethod, nameSourcePos);
		break;

	case ENUM_function_pointer:
		EmitPostScriptFunctionPointer(func_name.node[1], param_count, bMethod, nameSourcePos, func_name.node[2], block);
		break;
	}
}

/*
============
EmitPostScriptThreadCall
============
*/
void EmitPostScriptThreadCall( sval_u func_name, int param_count, bool bMethod, sval_u sourcePos, sval_u nameSourcePos, scr_block_s *block )
{
	switch ( func_name.node[0].type )
	{
	case ENUM_function:
		EmitPostScriptThread(func_name.node[1], param_count, bMethod, nameSourcePos);
		break;

	case ENUM_function_pointer:
		EmitPostScriptThreadPointer(func_name.node[1], param_count, bMethod, func_name.node[2], block);
		break;
	}

	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_NONE);
}

/*
============
EmitPreFunctionCall
============
*/
void EmitPreFunctionCall( sval_u func_name )
{
	if ( func_name.node[0].type == ENUM_script_call )
	{
		EmitOpcode(OP_PreScriptCall, 1, CALL_NONE);
	}
}

/*
============
EmitPostFunctionCall
============
*/
void EmitPostFunctionCall( sval_u func_name, int param_count, bool bMethod, scr_block_s *block )
{
	switch ( func_name.node[0].type )
	{
	case ENUM_script_call:
		EmitPostScriptFunctionCall( func_name.node[1], param_count, bMethod, func_name.node[2], block );
		break;

	case ENUM_script_thread_call:
		EmitPostScriptThreadCall( func_name.node[1], param_count, bMethod, func_name.node[2], func_name.node[3], block );
		break;
	}
}

/*
============
Scr_GetBuiltin
============
*/
unsigned int Scr_GetBuiltin( sval_u func_name )
{
	if ( func_name.node[0].type != ENUM_script_call )
	{
		return 0;
	}

	func_name = func_name.node[1];

	if ( func_name.node[0].type != ENUM_function )
	{
		return 0;
	}

	func_name = func_name.node[1];

	if ( func_name.node[0].type != ENUM_local_function )
	{
		return 0;
	}

	if ( FindVariable(scrCompileGlob.fileId, func_name.node[1].idValue) )
	{
		return 0;
	}

	return func_name.node[1].idValue;
}

/*
============
Scr_BeginDevScript
============
*/
void Scr_BeginDevScript( int *type, char **savedPos )
{
	if ( scrCompilePub.developer_statement != SCR_DEV_NO )
	{
		*type = BUILTIN_ANY;
	}
	else
	{
		if ( !scrVarPub.developer_script )
		{
			*savedPos = (char *)TempMalloc(0);
			scrCompilePub.developer_statement = SCR_DEV_IGNORE;
		}
		else
		{
			scrCompilePub.developer_statement = SCR_DEV_YES;
		}

		*type = BUILTIN_DEVELOPER_ONLY;
	}
}

/*
============
Scr_EndDevScript
============
*/
void Scr_EndDevScript( int type, char **savedPos )
{
	if ( type != BUILTIN_DEVELOPER_ONLY )
	{
		return;
	}

	assert(type == BUILTIN_DEVELOPER_ONLY);
	scrCompilePub.developer_statement = SCR_DEV_NO;

	if ( !scrVarPub.developer_script )
	{
		TempMemorySetPos(*savedPos);
	}
}

/*
============
Scr_GetCacheType
============
*/
int Scr_GetCacheType( int type )
{
	switch ( type )
	{
	case BUILTIN_ANY:
		return VAR_CODEPOS;

	default:
		return VAR_DEVELOPER_CODEPOS;
	}
}

/*
============
Scr_GetUncacheType
============
*/
int Scr_GetUncacheType( int type )
{
	switch ( type )
	{
	case VAR_CODEPOS:
		return 0;

	default:
		return 1;
	}
}

/*
============
EmitCallBuiltinOpcode
============
*/
void EmitCallBuiltinOpcode( int param_count, sval_u sourcePos )
{
	unsigned int opcode;

	if ( param_count <= 5 )
		opcode = OP_CallBuiltin0 + param_count;
	else
		opcode = OP_CallBuiltin;

	EmitOpcode(opcode, 1 - param_count, CALL_BUILTIN);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);

	if ( opcode != OP_CallBuiltin )
	{
	}
	else
	{
		EmitByte(param_count);
	}
}

/*
============
EmitCallBuiltinMethodOpcode
============
*/
void EmitCallBuiltinMethodOpcode( int param_count, sval_u sourcePos )
{
	unsigned int opcode;

	if ( param_count <= 5 )
		opcode = OP_CallBuiltinMethod0 + param_count;
	else
		opcode = OP_CallBuiltinMethod;

	EmitOpcode(opcode, -param_count, CALL_BUILTIN);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);

	if ( opcode != OP_CallBuiltinMethod )
	{
	}
	else
	{
		EmitByte(param_count);
	}
}

/*
============
EmitCall
============
*/
void EmitCall( sval_u func_name, sval_u params, bool bStatement, scr_block_s *block )
{
	int param_count;
	unsigned int name;
	sval_u sourcePos;
	const char *pName;
	int type;
	void (*func)();
	char *savedPos;
	unsigned int funcId;
	VariableValue value;

	name = Scr_GetBuiltin(func_name);

	if ( name )
	{
		pName = SL_ConvertToString(name);
		sourcePos = func_name.node[2];

		funcId = FindVariable(scrCompilePub.builtinFunc, name);

		if ( funcId )
		{
			value = Scr_EvalVariable(funcId);
			type = Scr_GetUncacheType(value.type);

			func = (void (*)())value.u.pointerValue;
		}
		else
		{
			type = BUILTIN_ANY;
			func = Scr_GetFunction(&pName, &type);

			funcId = GetNewVariable(scrCompilePub.builtinFunc, name);

			value.type = Scr_GetCacheType(type);
			value.u.pointerValue = (intptr_t)func;

			SetVariableValue(funcId, &value);
		}

		if ( !func )
		{
			goto script_function;
		}

		if ( type == BUILTIN_DEVELOPER_ONLY )
		{
			Scr_BeginDevScript(&type, &savedPos);

			if ( type == BUILTIN_DEVELOPER_ONLY && !bStatement )
			{
				CompileError(sourcePos.sourcePosValue, "return value of developer command can not be accessed if not in a /# ... #/ comment");
				return;
			}
		}

		param_count = EmitExpressionList(params, block);

		if ( param_count >= 256 )
		{
			CompileError(sourcePos.stringValue, "parameter count exceeds 256");
			return;
		}

		Scr_CompileRemoveRefToString(name);
		EmitCallBuiltinOpcode(param_count, sourcePos);

		EmitUnsignedShort( AddFunction( (intptr_t)func, pName ) );

		AddExpressionListOpcodePos(params);

		if ( bStatement )
		{
			EmitDecTop();
		}

		Scr_EndDevScript(type, &savedPos);
	}
	else
	{
script_function:
		EmitPreFunctionCall(func_name);

		param_count = EmitExpressionList(params, block);

		EmitPostFunctionCall(func_name, param_count, 0, block);
		AddExpressionListOpcodePos(params);

		if ( bStatement )
		{
			EmitDecTop();
		}
	}
}

/*
============
EmitMethod
============
*/
void EmitMethod( sval_u expr, sval_u func_name, sval_u params, sval_u methodSourcePos, bool bStatement, scr_block_s *block )
{
	int param_count;
	unsigned int name;
	sval_u sourcePos;
	const char *pName;
	int type;
	void (*meth)(scr_entref_t);
	char *savedPos;
	unsigned int methId;
	VariableValue value;

	name = Scr_GetBuiltin(func_name);

	if ( name )
	{
		pName = SL_ConvertToString(name);
		sourcePos = func_name.node[2];

		methId = FindVariable(scrCompilePub.builtinMeth, name);

		if ( methId )
		{
			value = Scr_EvalVariable(methId);
			type = Scr_GetUncacheType(value.type);

			meth = (void (*)(scr_entref_t))value.u.pointerValue;
		}
		else
		{
			type = BUILTIN_ANY;
			meth = Scr_GetMethod(&pName, &type);

			methId = GetNewVariable(scrCompilePub.builtinMeth, name);

			value.type = Scr_GetCacheType(type);
			value.u.pointerValue = (intptr_t)meth;

			SetVariableValue(methId, &value);
		}

		if ( !meth )
		{
			goto script_function;
		}

		if ( type == BUILTIN_DEVELOPER_ONLY )
		{
			Scr_BeginDevScript(&type, &savedPos);

			if ( type == BUILTIN_DEVELOPER_ONLY && !bStatement )
			{
				CompileError(sourcePos.sourcePosValue, "return value of developer command can not be accessed if not in a /# ... #/ comment");
				return;
			}
		}

		param_count = EmitExpressionList(params, block);
		EmitPrimitiveExpression(expr, block);

		if ( param_count >= 256 )
		{
			CompileError(sourcePos.sourcePosValue, "parameter count exceeds 256");
			return;
		}

		Scr_CompileRemoveRefToString(name);
		EmitCallBuiltinMethodOpcode(param_count, sourcePos);

		EmitUnsignedShort( AddFunction( (intptr_t)meth, pName ) );

		AddOpcodePos(methodSourcePos.sourcePosValue, SOURCE_TYPE_NONE);
		AddExpressionListOpcodePos(params);

		if ( bStatement )
		{
			EmitDecTop();
		}

		Scr_EndDevScript(type, &savedPos);
	}
	else
	{
script_function:
		EmitPreFunctionCall(func_name);

		param_count = EmitExpressionList(params, block);

		EmitPrimitiveExpression(expr, block);
		EmitPostFunctionCall(func_name, param_count, true, block);

		AddOpcodePos(methodSourcePos.sourcePosValue, SOURCE_TYPE_NONE);
		AddExpressionListOpcodePos(params);

		if ( bStatement )
		{
			EmitDecTop();
		}
	}
}

/*
============
LinkThread
============
*/
void LinkThread( unsigned int threadCountId, VariableValue *pos, bool allowFarCall )
{
	VariableValue count;
	unsigned int countId;
	VariableUnion *value;
	unsigned int valueId;
	int i;
	int type;

	countId = FindVariable(threadCountId, 0);

	if ( !countId )
	{
		return;
	}

	count = Scr_EvalVariable(countId);

	for ( i = 0; i < count.u.intValue; i++ )
	{
		valueId = FindVariable(threadCountId, i + 2);
		value = GetVariableValueAddress(valueId);
		type = GetObjectType(valueId);

		if ( pos->type == VAR_DEVELOPER_CODEPOS && type == VAR_CODEPOS )
		{
			CompileError2(value->codePosValue, "normal script cannot reference a function in a /# ... #/ comment");
			return;
		}

		if ( pos->type == VAR_UNDEFINED )
		{
			CompileError2(value->codePosValue, "unknown function");
			return;
		}

		if ( !allowFarCall && *(intptr_t *)value->codePosValue == FUNC_SCOPE_FAR )
		{
			CompileError2(value->codePosValue, "unknown function");
			return;
		}

		*(const char **)value->codePosValue = pos->u.codePosValue;
	}
}

/*
============
LinkFile
============
*/
void LinkFile( unsigned int fileId )
{
	unsigned int threadCountPtr, threadCountId, posId;
	VariableValue pos, emptyValue;

	emptyValue.type = VAR_UNDEFINED;
	emptyValue.u.intValue = 0;

	for ( threadCountPtr = FindNextSibling(fileId); threadCountPtr; threadCountPtr = FindNextSibling(threadCountPtr) )
	{
		threadCountId = FindObject(threadCountPtr);
		assert(threadCountId);

		posId = FindVariable(threadCountId, 1);

		if ( posId )
		{
			pos = Scr_EvalVariable(posId);

			if ( pos.type == VAR_INCLUDE_CODEPOS )
			{
				SetVariableValue(threadCountPtr, &emptyValue);
				continue;
			}

			assert(pos.type == VAR_CODEPOS || pos.type == VAR_DEVELOPER_CODEPOS);
			assert(pos.u.codePosValue);

			LinkThread(threadCountId, &pos, true);
		}
		else
		{
			LinkThread(threadCountId, &emptyValue, true);
		}
	}
}

/*
============
SpecifyThreadPosition
============
*/
unsigned int SpecifyThreadPosition( unsigned int posId, unsigned int name, unsigned int sourcePos, int type )
{
	VariableValue pos;
	unsigned int id;
	unsigned int bufIndex;

	id = GetVariable(posId, 1);
	pos = Scr_EvalVariable(id);

	if ( pos.type != VAR_UNDEFINED )
	{
		if ( pos.u.intValue )
		{
			bufIndex = Scr_GetSourceBuffer(pos.u.codePosValue);
			CompileError(sourcePos, "function '%s' already defined in '%s'", SL_ConvertToString(name), scrParserPub.sourceBufferLookup[bufIndex].buf);
		}
		else
		{
			CompileError(sourcePos, "function '%s' already defined", SL_ConvertToString(name));
		}

		return 0;
	}

	pos.type = type;
	pos.u.intValue = 0;

	SetNewVariableValue(id, &pos);

	return id;
}

/*
============
SetThreadPosition
============
*/
void SetThreadPosition( unsigned int posId, sval_u sourcePos )
{
	char *codePos;

	codePos = (char *)TempMalloc(0);
	GetVariableValueAddress( FindVariable( posId, 1 ) )->codePosValue = codePos;
}

/*
============
EmitCallExpression
============
*/
void EmitCallExpression( sval_u expr, bool bStatement, scr_block_s *block )
{
	switch ( expr.node[0].type )
	{
	case ENUM_call:
		EmitCall( expr.node[1], expr.node[2], bStatement, block );
		break;

	case ENUM_method:
		EmitMethod( expr.node[1], expr.node[2], expr.node[3], expr.node[4], bStatement, block );
		break;
	}
}

/*
============
EmitCallExpressionFieldObject
============
*/
void EmitCallExpressionFieldObject( sval_u expr, scr_block_s *block )
{
	switch ( expr.node[0].type )
	{
	case ENUM_call:
		EmitCall( expr.node[1], expr.node[2], false, block );
		EmitCastFieldObject( expr.node[3] );
		break;

	case ENUM_method:
		EmitMethod( expr.node[1], expr.node[2], expr.node[3], expr.node[4], false, block );
		EmitCastFieldObject( expr.node[5] );
		break;
	}
}

/*
============
Scr_CreateVector
============
*/
void Scr_CreateVector( VariableCompileValue *constValue, VariableValue *value )
{
	vec3_t vec;
	int i;
	int type;

	for ( i = 0; i < 3; i++ )
	{
		type = constValue[i].value.type;

		if ( type == VAR_FLOAT )
		{
			vec[2 - i] = constValue[i].value.u.floatValue;
		}
		else if ( type == VAR_INTEGER )
		{
			vec[2 - i] = (float)constValue[i].value.u.intValue;
		}
		else
		{
			CompileError(constValue[i].sourcePos.sourcePosValue, "type %s is not a float", var_typename[type]);
			return;
		}
	}

	value->type = VAR_VECTOR;
	value->u.vectorValue = Scr_AllocVector(vec);
}

/*
============
EvalPrimitiveExpressionList
============
*/
bool EvalPrimitiveExpressionList( sval_u exprlist, sval_u sourcePos, VariableCompileValue *constValue )
{
	int expr_count;
	sval_u *node;
	int i;
	VariableCompileValue constValue2[3];

	assert(constValue);
	expr_count = GetExpressionCount(exprlist);

	if ( expr_count == 1 )
	{
		node = exprlist.node[0].node;
		return EvalExpression(node[0].node[0], constValue);
	}

	if ( expr_count == 3 )
	{
		for ( i = 0, node = exprlist.node[0].node; node; i++, node = node[1].node )
		{
			if ( !EvalExpression(node[0].node[0], &constValue2[i]) )
			{
				return false;
			}
		}

		Scr_CreateVector(constValue2, &constValue->value);
		constValue->sourcePos = sourcePos;

		return true;
	}

	return false;
}

/*
============
EmitOrEvalPrimitiveExpressionList
============
*/
bool EmitOrEvalPrimitiveExpressionList( sval_u exprlist, sval_u sourcePos, VariableCompileValue *constValue, scr_block_s *block )
{
	int expr_count;
	sval_u *node;
	bool success;
	VariableCompileValue constValue2;

	assert(constValue);
	expr_count = GetExpressionCount(exprlist);

	if ( expr_count == 1 )
	{
		node = exprlist.node[0].node;
		return EmitOrEvalExpression(node[0].node[0], constValue, block);
	}

	if ( expr_count == 3 )
	{
		success = true;

		for ( node = exprlist.node[0].node; node; node = node[1].node )
		{
			if ( success )
			{
				success = EmitOrEvalExpression(node[0].node[0], &constValue2, block);

				if ( success )
				{
					Scr_PushValue(&constValue2);
				}
			}
			else
			{
				EmitExpression(node[0].node[0], block);
			}
		}

		if ( success )
		{
			assert(scrCompilePub.value_count >= 3);
			scrCompilePub.value_count -= 3;
			Scr_CreateVector(&scrCompileGlob.value_start[scrCompilePub.value_count], &constValue->value);
			constValue->sourcePos = sourcePos;
			return true;
		}

		EmitOpcode(OP_vector, -2, CALL_NONE);
		AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
		AddExpressionListOpcodePos(exprlist);
		return false;
	}

	CompileError(sourcePos.sourcePosValue, "expression list must have 1 or 3 parameters");
	return 0;
}

/*
============
EmitExpressionListFieldObject
============
*/
void EmitExpressionListFieldObject( sval_u exprlist, sval_u sourcePos, scr_block_s *block )
{
	sval_u *node = GetSingleParameter(exprlist);

	if ( node )
	{
		EmitExpressionFieldObject(node[0].node[0], node[0].node[1], block);
		return;
	}

	CompileError(sourcePos.sourcePosValue, "not an object");
}

/*
============
EvalPrimitiveExpression
============
*/
bool EvalPrimitiveExpression( sval_u expr, VariableCompileValue *constValue )
{
	float value;

	switch ( expr.node[0].type )
	{
	case ENUM_expression_list:
		return EvalPrimitiveExpressionList( expr.node[1], expr.node[2], constValue );

	case ENUM_integer:
		EvalInteger( expr.node[1].intValue, expr.node[2], constValue );
		return true;

	case ENUM_float:
		value = expr.node[1].floatValue;
		goto evalFloat;

	case ENUM_minus_integer:
		EvalInteger( -expr.node[1].intValue, expr.node[2], constValue );
		return true;

	case ENUM_minus_float:
		value = -expr.node[1].floatValue;

evalFloat:
		EvalFloat( value, expr.node[2], constValue );
		return true;

	case ENUM_string:
		EvalString( expr.node[1].stringValue, expr.node[2], constValue );
		return true;

	case ENUM_istring:
		EvalIString( expr.node[1].stringValue, expr.node[2], constValue );
		return true;

	case ENUM_undefined:
		EvalUndefined( expr.node[1], constValue );
		return true;

	case ENUM_false:
		EvalInteger( false, expr.node[1], constValue );
		return true;

	case ENUM_true:
		EvalInteger( true, expr.node[1], constValue );
		return true;

	default:
		return false;
	}
}

/*
============
EmitOrEvalPrimitiveExpression
============
*/
bool EmitOrEvalPrimitiveExpression( sval_u expr, VariableCompileValue *constValue, scr_block_s *block )
{
	switch ( expr.node[0].type )
	{
	case ENUM_expression_list:
		return EmitOrEvalPrimitiveExpressionList(expr.node[1], expr.node[2], constValue, block);

	case ENUM_call_expression:
		EmitCallExpression(expr.node[1], false, block);
		return false;

	case ENUM_variable:
		EmitVariableExpression(expr.node[1], block);
		return false;

	case ENUM_self:
		EmitSelf(expr.node[1]);
		return false;

	case ENUM_level:
		EmitLevel(expr.node[1]);
		return false;

	case ENUM_game:
		EmitGame(expr.node[1]);
		return false;

	case ENUM_anim:
		EmitAnim(expr.node[1]);
		return false;

	case ENUM_size_field:
		EmitSize(expr.node[1], expr.node[2], block);
		return false;

	case ENUM_function:
		EmitGetFunction(expr.node[1], expr.node[2]);
		return false;

	case ENUM_empty_array:
		EmitEmptyArray(expr.node[1]);
		return false;

	case ENUM_animation:
		EmitAnimation(expr.node[1], expr.node[2]);
		return false;

	case ENUM_animtree:
		EmitAnimTree(expr.node[1]);
		return false;

	default:
		return EvalPrimitiveExpression(expr, constValue);
	}
}

/*
============
EmitPrimitiveExpression
============
*/
void EmitPrimitiveExpression( sval_u expr, scr_block_s *block )
{
	VariableCompileValue constValue;

	if ( !EmitOrEvalPrimitiveExpression(expr, &constValue, block) )
	{
	}
	else
	{
		EmitValue(&constValue);
	}
}

/*
============
EmitBoolOrExpression
============
*/
void EmitBoolOrExpression( sval_u expr1, sval_u expr2, sval_u expr1sourcePos, sval_u expr2sourcePos, scr_block_s *block )
{
	const char *pos, *nextPos;
	unsigned int offset;

	EmitExpression(expr1, block);

	EmitOpcode(OP_JumpOnTrueExpr, -1, CALL_NONE);
	AddOpcodePos(expr1sourcePos.stringValue, SOURCE_TYPE_NONE);

	EmitUnsignedShort(0);

	pos = (const char *)scrCompileGlob.codePos;
	nextPos = TempMalloc(0);

	EmitExpression(expr2, block);
	EmitCastBool(expr2sourcePos);

	offset = TempMalloc(0) - nextPos;
	assert(offset < 65536);

	*(unsigned short *)pos = offset;
}

/*
============
EmitBoolAndExpression
============
*/
void EmitBoolAndExpression( sval_u expr1, sval_u expr2, sval_u expr1sourcePos, sval_u expr2sourcePos, scr_block_s *block )
{
	const char *pos, *nextPos;
	unsigned int offset;

	EmitExpression(expr1, block);

	EmitOpcode(OP_JumpOnFalseExpr, -1, CALL_NONE);
	AddOpcodePos(expr1sourcePos.sourcePosValue, SOURCE_TYPE_NONE);

	EmitUnsignedShort(0);

	pos = (const char *)scrCompileGlob.codePos;
	nextPos = TempMalloc(0);

	EmitExpression(expr2, block);
	EmitCastBool(expr2sourcePos);

	offset = TempMalloc(0) - nextPos;
	assert(offset < 65536);

	*(unsigned short *)pos = offset;
}

/*
============
EvalBinaryOperatorExpression
============
*/
bool EvalBinaryOperatorExpression( sval_u expr1, sval_u expr2, sval_u opcode, sval_u sourcePos, VariableCompileValue *constValue )
{
	VariableCompileValue constValue1;
	VariableCompileValue constValue2;

	if ( !EvalExpression(expr1, &constValue1) )
	{
		return false;
	}

	if ( !EvalExpression(expr2, &constValue2) )
	{
		return false;
	}

	AddRefToValue(&constValue1.value);
	AddRefToValue(&constValue2.value);

	Scr_EvalBinaryOperator(opcode.type, &constValue1.value, &constValue2.value);

	if ( scrVarPub.error_message )
	{
		CompileError(sourcePos.sourcePosValue, "%s", scrVarPub.error_message);
		return false;
	}

	constValue->value = constValue1.value;
	constValue->sourcePos = sourcePos;

	return true;
}

/*
============
EmitOrEvalBinaryOperatorExpression
============
*/
bool EmitOrEvalBinaryOperatorExpression( sval_u expr1, sval_u expr2, sval_u opcode, sval_u sourcePos, VariableCompileValue *constValue, scr_block_s *block )
{
	VariableCompileValue constValue1, constValue2;

	if ( !EmitOrEvalExpression(expr1, &constValue1, block) )
	{
		EmitExpression(expr2, block);
	}
	else
	{
		Scr_PushValue(&constValue1);

		if ( !EmitOrEvalExpression(expr2, &constValue2, block) )
		{
		}
		else
		{
			Scr_PopValue();
			Scr_EvalBinaryOperator(opcode.intValue, &constValue1.value, &constValue2.value);

			if ( scrVarPub.error_message )
			{
				CompileError(sourcePos.sourcePosValue, "%s", scrVarPub.error_message);
				return false;
			}

			constValue->value = constValue1.value;
			constValue->sourcePos = sourcePos;

			return true;
		}
	}

	EmitOpcode((char)opcode.type, -1, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_NONE);

	return false;
}

/*
============
EmitBinaryEqualsOperatorExpression
============
*/
void EmitBinaryEqualsOperatorExpression( sval_u lhs, sval_u rhs, sval_u opcode, sval_u sourcePos, scr_block_s *block )
{
	assert(!scrCompileGlob.bConstRefCount);
	scrCompileGlob.bConstRefCount = true;
	EmitVariableExpression(lhs, block);

	assert(scrCompileGlob.bConstRefCount);
	scrCompileGlob.bConstRefCount = false;
	EmitExpression(rhs, block);

	// the opcode is stored in the node's low byte
	EmitOpcode(*(const char *)&opcode, -1, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_NONE);

	EmitVariableExpressionRef(lhs, block);
	EmitSetVariableField(sourcePos);
}

/*
============
Scr_CalcLocalVarsBinaryEqualsOperatorExpression
============
*/
void Scr_CalcLocalVarsBinaryEqualsOperatorExpression( sval_u expr, scr_block_s *block )
{
	Scr_CalcLocalVarsVariableExpressionRef(expr, block);
}

/*
============
EvalExpression
============
*/
bool EvalExpression( sval_u expr, VariableCompileValue *constValue )
{
	switch ( expr.node[0].type )
	{
	case ENUM_primitive_expression:
		return EvalPrimitiveExpression( expr.node[1], constValue );

	case ENUM_binary:
		return EvalBinaryOperatorExpression( expr.node[1], expr.node[2], expr.node[3], expr.node[4], constValue );
	}

	return false;
}

/*
============
EmitOrEvalExpression
============
*/
bool EmitOrEvalExpression( sval_u expr, VariableCompileValue *constValue, scr_block_s *block )
{
	switch ( expr.node[0].type )
	{
	case ENUM_primitive_expression:
		return EmitOrEvalPrimitiveExpression(expr.node[1], constValue, block);

	case ENUM_bool_or:
		EmitBoolOrExpression(expr.node[1], expr.node[2], expr.node[3], expr.node[4], block);
		return false;

	case ENUM_bool_and:
		EmitBoolAndExpression(expr.node[1], expr.node[2], expr.node[3], expr.node[4], block);
		return false;

	case ENUM_binary:
		return EmitOrEvalBinaryOperatorExpression(expr.node[1], expr.node[2], expr.node[3], expr.node[4], constValue, block);

	case ENUM_bool_not:
		EmitExpression(expr.node[1], block);
		EmitBoolNot(expr.node[2]);
		return false;

	case ENUM_bool_complement:
		EmitExpression(expr.node[1], block);
		EmitBoolComplement(expr.node[2]);
		return false;

	default:
		return false;
	}
}

/*
============
EmitExpression
============
*/
void EmitExpression( sval_u expr, scr_block_s *block )
{
	VariableCompileValue constValue;

	if ( EmitOrEvalExpression(expr, &constValue, block) )
	{
		EmitValue(&constValue);
	}
}

/*
============
EmitVariableExpressionRef
============
*/
void EmitVariableExpressionRef( sval_u expr, scr_block_s *block )
{
	switch ( expr.node[0].type )
	{
	case ENUM_local_variable:
		EmitLocalVariableRef(expr.node[1], expr.node[2], block);
		break;

	case ENUM_array_variable:
		EmitArrayVariableRef(expr.node[1], expr.node[2], expr.node[3], expr.node[4], block);
		break;

	case ENUM_field_variable:
		EmitFieldVariableRef(expr.node[1], expr.node[2], expr.node[3], block);
		break;

	case ENUM_self_field:
	// 1.0's node-type enum has no ENUM_vector: ENUM_object is 0x4F here, one below
	// the CoD2rev (1.3) value. Target: cmp DWORD PTR [ebp-0x8],0x4f
	case ENUM_object - 1:
		CompileError(expr.node[2].sourcePosValue, !scrCompilePub.script_loading
			? "not an lvalue"
			: "$ and self field can only be used in the script debugger");
		break;
	}
}

/*
============
Scr_CalcLocalVarsVariableExpressionRef
============
*/
void Scr_CalcLocalVarsVariableExpressionRef( sval_u expr, scr_block_s *block )
{
	switch ( expr.node[0].type )
	{
	case ENUM_local_variable:
		Scr_CalcLocalVarsLocalVariableRef(expr.node[1], expr.node[2], block);
		break;

	case ENUM_array_variable:
		Scr_CalcLocalVarsArrayVariableRef(expr.node[1], block);
		break;
	}
}

/*
============
EmitArrayPrimitiveExpressionRef
============
*/
void EmitArrayPrimitiveExpressionRef( sval_u expr, sval_u sourcePos, scr_block_s *block )
{
	switch ( expr.node[0].type )
	{
	case ENUM_variable:
		EmitVariableExpressionRef(expr.node[1], block);
		break;

	case ENUM_game:
		EmitGameRef(expr.node[1]);
		break;

	default:
		CompileError(sourcePos.sourcePosValue, "not an lvalue");
		break;
	}
}

/*
============
Scr_CalcLocalVarsArrayPrimitiveExpressionRef
============
*/
void Scr_CalcLocalVarsArrayPrimitiveExpressionRef( sval_u expr, scr_block_s *block )
{
	switch ( expr.node[0].type )
	{
	case ENUM_variable:
		Scr_CalcLocalVarsVariableExpressionRef( expr.node[1], block );
		break;
	}
}

/*
============
EmitPrimitiveExpressionFieldObject
============
*/
void EmitPrimitiveExpressionFieldObject( sval_u expr, sval_u sourcePos, scr_block_s *block )
{
	switch ( expr.node[0].type )
	{
	case ENUM_expression_list:
		EmitExpressionListFieldObject(expr.node[1], sourcePos, block);
		break;

	case ENUM_call_expression:
		EmitCallExpressionFieldObject(expr.node[1], block);
		break;

	case ENUM_variable:
		EmitVariableExpression(expr.node[1], block);
		EmitCastFieldObject(expr.node[2]);
		break;

	case ENUM_self:
		EmitSelfObject(expr.node[1]);
		break;

	case ENUM_level:
		EmitLevelObject(expr.node[1]);
		break;

	case ENUM_anim:
		EmitAnimObject(expr.node[1]);
		break;

	default:
		CompileError(sourcePos.sourcePosValue, "not an object");
		break;
	}
}

/*
============
EmitExpressionFieldObject
============
*/
void EmitExpressionFieldObject( sval_u expr, sval_u sourcePos, scr_block_s *block )
{
	switch ( expr.node[0].type )
	{
	case ENUM_primitive_expression:
		EmitPrimitiveExpressionFieldObject(expr.node[1], expr.node[2], block);
		break;

	default:
		CompileError(sourcePos.sourcePosValue, "not an object");
		break;
	}
}

/*
============
ConnectContinueStatements
============
*/
void ConnectContinueStatements()
{
	const char *codePos = (char *)TempMalloc(0);

	for ( ContinueStatementInfo *statement = scrCompileGlob.currentContinueStatement; statement; statement = statement->next )
	{
		*(intptr_t *)statement->codePos = codePos - statement->nextCodePos;
	}
}

/*
============
ConnectBreakStatements
============
*/
__declspec(noinline) void ConnectBreakStatements()
{
	assert(!scrCompilePub.value_count);
	const char *codePos = (char *)TempMalloc(0);

	for ( BreakStatementInfo *statement = scrCompileGlob.currentBreakStatement; statement; statement = statement->next )
	{
		*(intptr_t *)statement->codePos = codePos - statement->nextCodePos;
	}
}

/*
============
IsUndefinedPrimitiveExpression
============
*/
bool IsUndefinedPrimitiveExpression( sval_u expr )
{
	return expr.node[0].type == ENUM_undefined;
}

/*
============
IsUndefinedExpression
============
*/
int IsUndefinedExpression( sval_u expr )
{
	if ( expr.node[0].type == ENUM_primitive_expression )
		return IsUndefinedPrimitiveExpression( expr.node[1] );
	return 0;
}

/*
============
EmitClearVariableExpression
============
*/
bool EmitClearVariableExpression( sval_u expr, sval_u rhsSourcePos, scr_block_s *block )
{
	switch ( expr.node[0].type )
	{
	case ENUM_local_variable:
		return false;

	case ENUM_array_variable:
		EmitClearArrayVariable(expr.node[1], expr.node[2], expr.node[3], expr.node[4], block);
		break;

	case ENUM_field_variable:
		EmitClearFieldVariable(expr.node[1], expr.node[2], expr.node[3], rhsSourcePos, block);
		break;

	case ENUM_self_field:
	// 1.0's node-type enum has no ENUM_vector: ENUM_object is 0x4F here, one below
	// the CoD2rev (1.3) value. Target: cmp DWORD PTR [ebp-0xc],0x4f
	case ENUM_object - 1:
		CompileError(expr.node[2].sourcePosValue, !scrCompilePub.script_loading
			? "not an lvalue"
			: "$ and self field can only be used in the script debugger");
		break;
	}

	return true;
}

/*
============
EmitAssignmentStatement
============
*/
void EmitAssignmentStatement( sval_u lhs, sval_u rhs, sval_u sourcePos, sval_u rhsSourcePos, scr_block_s *block )
{
	if ( (unsigned char)IsUndefinedExpression(rhs) )
	{
		if ( EmitClearVariableExpression(lhs, rhsSourcePos, block) )
		{
			return;
		}
	}

	EmitExpression(rhs, block);
	EmitVariableExpressionRef(lhs, block);
	EmitSetVariableField(sourcePos);
}

/*
============
Scr_CalcLocalVarsAssignmentStatement
============
*/
void Scr_CalcLocalVarsAssignmentStatement( sval_u lhs, sval_u rhs, scr_block_s *block )
{
	Scr_CalcLocalVarsVariableExpressionRef(lhs, block);
}

/*
============
EmitCallExpressionStatement
============
*/
void EmitCallExpressionStatement( sval_u expr, scr_block_s *block )
{
	EmitCallExpression( expr, true, block );
}

/*
============
EmitReturnStatement
============
*/
void EmitReturnStatement( sval_u expr, sval_u sourcePos, scr_block_s *block )
{
	if ( block->abortLevel == SCR_ABORT_NONE )
	{
		block->abortLevel = SCR_ABORT_RETURN;
	}

	EmitExpression(expr, block);
	EmitReturn();

	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_NONE);
}

/*
============
EmitEndStatement
============
*/
void EmitEndStatement( sval_u sourcePos, scr_block_s *block )
{
	if ( block->abortLevel == SCR_ABORT_NONE )
	{
		block->abortLevel = SCR_ABORT_RETURN;
	}

	EmitEnd();
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
}

/*
============
EmitWaitStatement
============
*/
void EmitWaitStatement( sval_u expr, sval_u sourcePos, sval_u waitSourcePos, scr_block_s *block )
{
	EmitExpression(expr, block);
	EmitOpcode(OP_wait, -1, CALL_NONE);

	AddOpcodePos(waitSourcePos.sourcePosValue, SOURCE_TYPE_NONE);
	AddOpcodePos(waitSourcePos.sourcePosValue, SOURCE_TYPE_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_NONE);
}

/*
============
EmitWaittillFrameEnd
============
*/
void EmitWaittillFrameEnd( sval_u sourcePos )
{
	EmitOpcode(OP_waittillFrameEnd, 0, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_NONE);
}

/*
============
EmitIfStatement
============
*/
void EmitIfStatement( sval_u expr, sval_u stmt, sval_u sourcePos, bool lastStatement, unsigned int endSourcePos, scr_block_s *block, sval_u *ifStatBlock )
{
	const char *pos, *nextPos;
	unsigned int offset;

	EmitExpression(expr, block);
	EmitOpcode(OP_JumpOnFalse, -1, CALL_NONE);
	AddOpcodePos(sourcePos.stringValue, SOURCE_TYPE_NONE);
	EmitUnsignedShort(0);

	pos = (const char *)scrCompileGlob.codePos;
	nextPos = TempMalloc(0);

	Scr_TransferBlock(block, ifStatBlock->block);

	EmitStatement(stmt, lastStatement, endSourcePos, ifStatBlock->block);
	assert(ifStatBlock->block->localVarsPublicCount == block->localVarsCreateCount);
	EmitNOP2(lastStatement, endSourcePos, ifStatBlock->block);

	offset = TempMalloc(0) - nextPos;
	assert(offset < 65536);
	*(unsigned short *)pos = offset;
}

/*
============
Scr_CalcLocalVarsIfStatement
============
*/
void Scr_CalcLocalVarsIfStatement( sval_u stmt, scr_block_s *block, sval_u *ifStatBlock )
{
	Scr_CopyBlock( block, &ifStatBlock->block );
	Scr_CalcLocalVarsStatement( stmt, ifStatBlock->block );
	Scr_MergeChildBlocks( &ifStatBlock->block, 1, block );
}

/*
============
EmitIfElseStatement
============
*/
void EmitIfElseStatement( sval_u expr, sval_u stmt1, sval_u stmt2, sval_u sourcePos, sval_u elseSourcePos, bool lastStatement, unsigned int endSourcePos, scr_block_s *block, sval_u *ifStatBlock, sval_u *elseStatBlock )
{
	const char *pos1, *pos2, *nextPos1, *nextPos2;
	int checksum;
	scr_block_s *childBlocks[2];
	int childCount;
	unsigned int offset;

	childCount = 0;

	EmitExpression(expr, block);
	EmitOpcode(OP_JumpOnFalse, -1, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_NONE);
	EmitUnsignedShort(0);

	pos1 = (const char *)scrCompileGlob.codePos;
	nextPos1 = TempMalloc(0);

	Scr_TransferBlock(block, ifStatBlock->block);

	EmitStatement(stmt1, lastStatement, endSourcePos, ifStatBlock->block);
	EmitRemoveLocalVars(ifStatBlock->block, ifStatBlock->block);

	if ( ifStatBlock->block->abortLevel == SCR_ABORT_NONE )
	{
		childBlocks[childCount] = ifStatBlock->block;
		childCount++;
	}

	checksum = scrVarPub.checksum;

	if ( lastStatement )
	{
		EmitEnd();
		EmitInteger(0);
		AddOpcodePos(endSourcePos, SOURCE_TYPE_BREAKPOINT);

		pos2 = NULL;
		nextPos2 = NULL;
	}
	else
	{
		EmitOpcode(OP_jump, 0, CALL_NONE);
		AddOpcodePos(elseSourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
		EmitInteger(0);

		pos2 = (const char *)scrCompileGlob.codePos;
		nextPos2 = TempMalloc(0);
	}

	scrVarPub.checksum = checksum + 1;

	offset = TempMalloc(0) - nextPos1;
	assert(offset < 65536);
	*(unsigned short *)pos1 = offset;

	Scr_TransferBlock(block, elseStatBlock->block);

	EmitStatement(stmt2, lastStatement, endSourcePos, elseStatBlock->block);
	EmitNOP2(lastStatement, endSourcePos, elseStatBlock->block);

	if ( elseStatBlock->block->abortLevel == SCR_ABORT_NONE )
	{
		childBlocks[childCount] = elseStatBlock->block;
		childCount++;
	}

	if ( !lastStatement )
	{
		*(intptr_t *)pos2 = TempMalloc(0) - nextPos2;
	}

	Scr_InitFromChildBlocks(childBlocks, childCount, block);
}

/*
============
Scr_CalcLocalVarsIfElseStatement
============
*/
void Scr_CalcLocalVarsIfElseStatement( sval_u stmt1, sval_u stmt2, scr_block_s *block, sval_u *ifStatBlock, sval_u *elseStatBlock )
{
	scr_block_s *childBlocks[2];
	int childCount;
	int abortLevel;

	childCount = 0;
	abortLevel = SCR_ABORT_RETURN;

	Scr_CopyBlock(block, &ifStatBlock->block);
	Scr_CalcLocalVarsStatement(stmt1, ifStatBlock->block);

	if ( ifStatBlock->node[0].intValue <= abortLevel )
	{
		abortLevel = ifStatBlock->node[0].intValue;

		if ( abortLevel == SCR_ABORT_NONE )
		{
			childBlocks[childCount] = ifStatBlock->block;
			childCount++;
		}
	}

	Scr_CopyBlock(block, &elseStatBlock->block);
	Scr_CalcLocalVarsStatement(stmt2, elseStatBlock->block);

	if ( elseStatBlock->node[0].intValue <= abortLevel )
	{
		abortLevel = elseStatBlock->node[0].intValue;

		if ( abortLevel == SCR_ABORT_NONE )
		{
			childBlocks[childCount] = elseStatBlock->block;
			childCount++;
		}
	}

	if ( block->abortLevel == SCR_ABORT_NONE )
	{
		block->abortLevel = abortLevel;
	}

	Scr_AppendChildBlocks(childBlocks, childCount, block);
	Scr_MergeChildBlocks(childBlocks, childCount, block);
}

/*
============
Scr_AddBreakBlock
============
*/
void Scr_AddBreakBlock( scr_block_s *block )
{
	if ( block->abortLevel )
	{
		return;
	}

	if ( !scrCompileGlob.breakChildBlocks )
	{
		return;
	}

	Scr_CheckMaxSwitchCases(*scrCompileGlob.breakChildCount);

	scrCompileGlob.breakChildBlocks[*scrCompileGlob.breakChildCount] = block;
	(*scrCompileGlob.breakChildCount)++;
}

/*
============
Scr_AddContinueBlock
============
*/
void Scr_AddContinueBlock( scr_block_s *block )
{
	if ( block->abortLevel )
	{
		return;
	}

	if ( !scrCompileGlob.continueChildBlocks )
	{
		return;
	}

	Scr_CheckMaxSwitchCases(*scrCompileGlob.continueChildCount);

	scrCompileGlob.continueChildBlocks[*scrCompileGlob.continueChildCount] = block;
	(*scrCompileGlob.continueChildCount)++;
}

/*
============
EmitWhileStatement
============
*/
void EmitWhileStatement( sval_u expr, sval_u stmt, sval_u sourcePos, sval_u whileSourcePos, scr_block_s *block, sval_u *whileStatBlock )
{
	const char *pos1;
	const char *pos2;
	const char *nextPos2;
	bool bOldCanBreak;
	bool bOldCanIgnoreBreak;
	BreakStatementInfo *oldBreakStatement;
	bool bOldCanContinue;
	bool bOldCanIgnoreContinue;
	ContinueStatementInfo *oldContinueStatement;
	bool constConditional;
	VariableCompileValue constValue;
	scr_block_s **oldBreakChildBlocks;
	int *oldBreakChildCount;
	scr_block_s **breakChildBlocks;
	int breakChildCount;
	scr_block_s **oldContinueChildBlocks;
	int *oldContinueChildCount;
	scr_block_s *oldBreakBlock;
	unsigned int offset;

	bOldCanBreak = scrCompileGlob.bCanBreak;
	bOldCanIgnoreBreak = scrCompileGlob.bCanIgnoreBreak;

	oldBreakStatement = scrCompileGlob.currentBreakStatement;

	scrCompileGlob.bCanBreak = false;
	scrCompileGlob.bCanIgnoreBreak = false;

	bOldCanContinue = scrCompileGlob.bCanContinue;
	bOldCanIgnoreContinue = scrCompileGlob.bCanIgnoreContinue;

	oldContinueStatement = scrCompileGlob.currentContinueStatement;

	scrCompileGlob.bCanContinue = false;
	scrCompileGlob.bCanIgnoreContinue = false;

	Scr_TransferBlock(block, whileStatBlock->block);

	EmitCreateLocalVars(whileStatBlock->block, whileSourcePos);
	assert(whileStatBlock->block->localVarsCreateCount <= block->localVarsCount);
	block->localVarsCreateCount = whileStatBlock->block->localVarsCreateCount;

	pos1 = TempMalloc(0);
	constConditional = false;

	if ( EmitOrEvalExpression(expr, &constValue, block) )
	{
		if ( constValue.value.type == VAR_INTEGER || constValue.value.type == VAR_FLOAT )
		{
			Scr_CastBool(&constValue.value);

			if ( !constValue.value.u.intValue )
			{
				CompileError(sourcePos.sourcePosValue, "conditional expression cannot be always false");
			}

			constConditional = true;
		}
		else
		{
			EmitValue(&constValue);
		}
	}

	oldBreakChildBlocks = scrCompileGlob.breakChildBlocks;
	oldBreakChildCount = scrCompileGlob.breakChildCount;

	oldBreakBlock = scrCompileGlob.breakBlock;

	oldContinueChildBlocks = scrCompileGlob.continueChildBlocks;
	oldContinueChildCount = scrCompileGlob.continueChildCount;

	breakChildCount = 0;
	scrCompileGlob.continueChildBlocks = 0;

	scrCompileGlob.breakBlock = whileStatBlock->block;

	if ( !constConditional )
	{
		EmitOpcode(OP_JumpOnFalse, -1, CALL_NONE);
		AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_NONE);

		EmitUnsignedShort(0);

		pos2 = (const char *)scrCompileGlob.codePos;
		nextPos2 = TempMalloc(0);

		breakChildBlocks = NULL;
	}
	else
	{
		pos2 = NULL;
		nextPos2 = NULL;

		breakChildBlocks = (scr_block_s **)Hunk_AllocateTempMemoryHighInternal( sizeof( scr_block_s ** ) * MAX_SWITCH_CASES );
		scrCompileGlob.breakChildCount = &breakChildCount;
	}

	scrCompileGlob.breakChildBlocks = breakChildBlocks;

	scrCompileGlob.bCanBreak = true;
	scrCompileGlob.bCanIgnoreBreak = scrCompilePub.developer_statement != SCR_DEV_NO;

	scrCompileGlob.currentBreakStatement = 0;

	scrCompileGlob.bCanContinue = true;
	scrCompileGlob.bCanIgnoreContinue = scrCompilePub.developer_statement != SCR_DEV_NO;

	scrCompileGlob.currentContinueStatement = 0;

	EmitStatement(stmt, false, 0, whileStatBlock->block);

	if ( whileStatBlock->block->abortLevel != SCR_ABORT_RETURN )
	{
		whileStatBlock->block->abortLevel = SCR_ABORT_NONE;
	}

	scrCompileGlob.bCanBreak = false;
	scrCompileGlob.bCanIgnoreBreak = false;

	scrCompileGlob.bCanContinue = false;
	scrCompileGlob.bCanIgnoreContinue = false;

	ConnectContinueStatements();

	EmitOpcode(OP_jumpback, 0, CALL_NONE);
	AddOpcodePos(whileSourcePos.sourcePosValue, SOURCE_TYPE_NONE);

	if ( stmt.node[0].type == ENUM_statement_list )
	{
		AddOpcodePos(stmt.node[3].sourcePosValue, SOURCE_TYPE_BREAKPOINT);
	}

	EmitUnsignedShort(0);

	offset = TempMalloc(0) - pos1;
	assert(offset < 65536);

	*(unsigned short *)scrCompileGlob.codePos = offset;

	if ( pos2 )
	{
		offset = TempMalloc(0) - nextPos2;
		assert(offset < 65536);

		*(unsigned short *)pos2 = offset;
	}

	ConnectBreakStatements();

	scrCompileGlob.bCanBreak = bOldCanBreak;
	scrCompileGlob.bCanIgnoreBreak = bOldCanIgnoreBreak;

	scrCompileGlob.currentBreakStatement = oldBreakStatement;

	scrCompileGlob.bCanContinue = bOldCanContinue;
	scrCompileGlob.bCanIgnoreContinue = bOldCanIgnoreContinue;

	scrCompileGlob.currentContinueStatement = oldContinueStatement;

	if ( constConditional )
	{
		Scr_InitFromChildBlocks(breakChildBlocks, breakChildCount, block);
	}

	scrCompileGlob.breakChildBlocks = oldBreakChildBlocks;
	scrCompileGlob.breakChildCount = oldBreakChildCount;

	scrCompileGlob.breakBlock = oldBreakBlock;

	scrCompileGlob.continueChildBlocks = oldContinueChildBlocks;
	scrCompileGlob.continueChildCount = oldContinueChildCount;
}

/*
============
Scr_CalcLocalVarsWhileStatement
============
*/
void Scr_CalcLocalVarsWhileStatement( sval_u expr, sval_u stmt, scr_block_s *block, sval_u *whileStatBlock )
{
	int abortLevel;
	bool constConditional;
	VariableCompileValue constValue;
	scr_block_s **oldBreakChildBlocks;
	int *oldBreakChildCount;
	scr_block_s **breakChildBlocks;
	int breakChildCount;
	scr_block_s **continueChildBlocks;
	int continueChildCount;
	scr_block_s **oldContinueChildBlocks;
	int *oldContinueChildCount;
	int i;

	constConditional = false;

	if ( EvalExpression(expr, &constValue) )
	{
		if ( constValue.value.type == VAR_INTEGER || constValue.value.type == VAR_FLOAT )
		{
			Scr_CastBool(&constValue.value);

			if ( constValue.value.u.intValue )
			{
				constConditional = true;
			}
		}

		RemoveRefToValue(&constValue.value);
	}

	oldBreakChildBlocks = scrCompileGlob.breakChildBlocks;
	oldBreakChildCount = scrCompileGlob.breakChildCount;

	oldContinueChildBlocks = scrCompileGlob.continueChildBlocks;
	oldContinueChildCount = scrCompileGlob.continueChildCount;

	breakChildCount = 0;
	continueChildCount = 0;

	continueChildBlocks = (scr_block_s **)Hunk_AllocateTempMemoryHighInternal( sizeof( scr_block_s ** ) * MAX_SWITCH_CASES );

	scrCompileGlob.continueChildBlocks = continueChildBlocks;
	scrCompileGlob.continueChildCount = &continueChildCount;

	abortLevel = block->abortLevel;

	if ( constConditional )
	{
		breakChildBlocks = (scr_block_s **)Hunk_AllocateTempMemoryHighInternal( sizeof( scr_block_s ** ) * MAX_SWITCH_CASES );
		scrCompileGlob.breakChildCount = &breakChildCount;
	}
	else
	{
		breakChildBlocks = NULL;
	}

	scrCompileGlob.breakChildBlocks = breakChildBlocks;

	Scr_CopyBlock(block, &whileStatBlock->block);

	Scr_CalcLocalVarsStatement(stmt, whileStatBlock->block);
	Scr_AddContinueBlock(whileStatBlock->block);

	for ( i = 0; i < continueChildCount; i++ )
	{
		Scr_AppendChildBlocks(&continueChildBlocks[i], 1, block);
	}

	if ( constConditional )
	{
		Scr_AppendChildBlocks(breakChildBlocks, breakChildCount, block);
	}

	Scr_MergeChildBlocks(&whileStatBlock->block, 1, block);

	scrCompileGlob.breakChildBlocks = oldBreakChildBlocks;
	scrCompileGlob.breakChildCount = oldBreakChildCount;

	scrCompileGlob.continueChildBlocks = oldContinueChildBlocks;
	scrCompileGlob.continueChildCount = oldContinueChildCount;
}

/*
============
EmitForStatement
============
*/
void EmitForStatement( sval_u stmt1, sval_u expr, sval_u stmt2, sval_u stmt, sval_u sourcePos, sval_u forSourcePos, scr_block_s *block, sval_u *forStatBlock, sval_u *forStatPostBlock )
{
	const char *pos1;
	const char *pos2;
	const char *nextPos2;
	bool bOldCanBreak;
	bool bOldCanIgnoreBreak;
	BreakStatementInfo *oldBreakStatement;
	bool bOldCanContinue;
	bool bOldCanIgnoreContinue;
	ContinueStatementInfo *oldContinueStatement;
	bool constConditional;
	VariableCompileValue constValue;
	scr_block_s **oldBreakChildBlocks;
	int *oldBreakChildCount;
	scr_block_s **breakChildBlocks;
	int breakChildCount;
	scr_block_s **continueChildBlocks;
	int continueChildCount;
	scr_block_s **oldContinueChildBlocks;
	int *oldContinueChildCount;
	scr_block_s *oldBreakBlock;
	unsigned int offset;

	bOldCanBreak = scrCompileGlob.bCanBreak;
	bOldCanIgnoreBreak = scrCompileGlob.bCanIgnoreBreak;

	oldBreakStatement = scrCompileGlob.currentBreakStatement;

	scrCompileGlob.bCanBreak = false;
	scrCompileGlob.bCanIgnoreBreak = false;

	bOldCanContinue = scrCompileGlob.bCanContinue;
	bOldCanIgnoreContinue = scrCompileGlob.bCanIgnoreContinue;

	oldContinueStatement = scrCompileGlob.currentContinueStatement;

	scrCompileGlob.bCanContinue = false;
	scrCompileGlob.bCanIgnoreContinue = false;

	EmitStatement(stmt1, false, 0, block);
	Scr_TransferBlock(block, forStatBlock->block);

	EmitCreateLocalVars(forStatBlock->block, forSourcePos);
	assert(forStatBlock->block->localVarsCreateCount <= block->localVarsCount);

	block->localVarsCreateCount = forStatBlock->block->localVarsCreateCount;
	Scr_TransferBlock(block, forStatPostBlock->block);

	pos1 = TempMalloc(0);

	if ( expr.node[0].type == ENUM_expression )
	{
		constConditional = false;

		if ( EmitOrEvalExpression(expr.node[1], &constValue, block) )
		{
			if ( constValue.value.type == VAR_INTEGER || constValue.value.type == VAR_FLOAT )
			{
				Scr_CastBool(&constValue.value);

				if ( !constValue.value.u.intValue )
				{
					CompileError(sourcePos.sourcePosValue, "conditional expression cannot be always false");
				}

				constConditional = true;
			}
			else
			{
				EmitValue(&constValue);
			}
		}
	}
	else
	{
		constConditional = true;
	}

	oldBreakChildBlocks = scrCompileGlob.breakChildBlocks;
	oldBreakChildCount = scrCompileGlob.breakChildCount;

	oldBreakBlock = scrCompileGlob.breakBlock;

	oldContinueChildBlocks = scrCompileGlob.continueChildBlocks;
	oldContinueChildCount = scrCompileGlob.continueChildCount;

	breakChildCount = 0;
	continueChildCount = 0;

	continueChildBlocks = (scr_block_s **)Hunk_AllocateTempMemoryHighInternal( sizeof( scr_block_s ** ) * MAX_SWITCH_CASES );

	scrCompileGlob.continueChildBlocks = continueChildBlocks;
	scrCompileGlob.continueChildCount = &continueChildCount;

	scrCompileGlob.breakBlock = forStatBlock->block;

	if ( !constConditional )
	{
		EmitOpcode(OP_JumpOnFalse, -1, CALL_NONE);
		AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_NONE);

		EmitUnsignedShort(0);
		pos2 = (const char *)scrCompileGlob.codePos;

		nextPos2 = TempMalloc(0);
		breakChildBlocks = NULL;
	}
	else
	{
		pos2 = NULL;
		nextPos2 = NULL;

		breakChildBlocks = (scr_block_s **)Hunk_AllocateTempMemoryHighInternal( sizeof( scr_block_s ** ) * MAX_SWITCH_CASES );
		scrCompileGlob.breakChildCount = &breakChildCount;
	}

	scrCompileGlob.breakChildBlocks = breakChildBlocks;

	scrCompileGlob.bCanBreak = true;
	scrCompileGlob.bCanIgnoreBreak = scrCompilePub.developer_statement != SCR_DEV_NO;
	scrCompileGlob.currentBreakStatement = 0;

	scrCompileGlob.bCanContinue = true;
	scrCompileGlob.bCanIgnoreContinue = scrCompilePub.developer_statement != SCR_DEV_NO;
	scrCompileGlob.currentContinueStatement = 0;

	EmitStatement(stmt, false, 0, forStatBlock->block);
	Scr_AddContinueBlock(forStatBlock->block);

	scrCompileGlob.bCanBreak = false;
	scrCompileGlob.bCanIgnoreBreak = false;

	scrCompileGlob.bCanContinue = false;
	scrCompileGlob.bCanIgnoreContinue = false;

	ConnectContinueStatements();

	Scr_InitFromChildBlocks(continueChildBlocks, continueChildCount, forStatPostBlock->block);

	EmitStatement(stmt2, false, 0, forStatPostBlock->block);
	EmitOpcode(OP_jumpback, 0, CALL_NONE);
	AddOpcodePos(forSourcePos.stringValue, SOURCE_TYPE_NONE);

	if ( stmt.node[0].type == ENUM_statement_list )
	{
		AddOpcodePos(stmt.node[3].sourcePosValue, SOURCE_TYPE_BREAKPOINT);
	}

	EmitUnsignedShort(0);

	offset = TempMalloc(0) - pos1;
	assert(offset < 65536);

	*(unsigned short *)scrCompileGlob.codePos = offset;

	if ( pos2 )
	{
		offset = TempMalloc(0) - nextPos2;
		assert(offset < 65536);

		*(unsigned short *)pos2 = offset;
	}

	ConnectBreakStatements();

	scrCompileGlob.bCanBreak = bOldCanBreak;
	scrCompileGlob.bCanIgnoreBreak = bOldCanIgnoreBreak;

	scrCompileGlob.currentBreakStatement = oldBreakStatement;

	scrCompileGlob.bCanContinue = bOldCanContinue;
	scrCompileGlob.bCanIgnoreContinue = bOldCanIgnoreContinue;

	scrCompileGlob.currentContinueStatement = oldContinueStatement;

	if ( constConditional )
	{
		Scr_InitFromChildBlocks(breakChildBlocks, breakChildCount, block);
	}

	scrCompileGlob.breakChildBlocks = oldBreakChildBlocks;
	scrCompileGlob.breakChildCount = oldBreakChildCount;

	scrCompileGlob.breakBlock = oldBreakBlock;

	scrCompileGlob.continueChildBlocks = oldContinueChildBlocks;
	scrCompileGlob.continueChildCount = oldContinueChildCount;
}

/*
============
Scr_CalcLocalVarsForStatement
============
*/
void Scr_CalcLocalVarsForStatement( sval_u stmt1, sval_u expr, sval_u stmt2, sval_u stmt, scr_block_s *block, sval_u *forStatBlock, sval_u *forStatPostBlock )
{
	int abortLevel;
	bool constConditional;
	VariableCompileValue constValue;
	scr_block_s **oldBreakChildBlocks;
	int *oldBreakChildCount;
	scr_block_s **breakChildBlocks;
	int breakChildCount;
	scr_block_s **continueChildBlocks;
	int continueChildCount;
	scr_block_s **oldContinueChildBlocks;
	int *oldContinueChildCount;
	int i;

	Scr_CalcLocalVarsStatement(stmt1, block);

	if ( expr.node[0].type == ENUM_expression )
	{
		constConditional = false;

		if ( EvalExpression(expr.node[1], &constValue) )
		{
			if ( constValue.value.type == VAR_INTEGER || constValue.value.type == VAR_FLOAT )
			{
				Scr_CastBool(&constValue.value);

				if ( constValue.value.u.intValue )
				{
					constConditional = true;
				}
			}

			RemoveRefToValue(&constValue.value);
		}
	}
	else
	{
		constConditional = true;
	}

	oldBreakChildBlocks = scrCompileGlob.breakChildBlocks;
	oldBreakChildCount = scrCompileGlob.breakChildCount;

	oldContinueChildBlocks = scrCompileGlob.continueChildBlocks;
	oldContinueChildCount = scrCompileGlob.continueChildCount;

	breakChildCount = 0;
	continueChildCount = 0;

	continueChildBlocks = (scr_block_s **)Hunk_AllocateTempMemoryHighInternal( sizeof( scr_block_s ** ) * MAX_SWITCH_CASES );

	scrCompileGlob.continueChildBlocks = continueChildBlocks;
	scrCompileGlob.continueChildCount = &continueChildCount;

	abortLevel = block->abortLevel;

	if ( constConditional )
	{
		breakChildBlocks = (scr_block_s **)Hunk_AllocateTempMemoryHighInternal( sizeof( scr_block_s ** ) * MAX_SWITCH_CASES );
		scrCompileGlob.breakChildCount = &breakChildCount;
	}
	else
	{
		breakChildBlocks = NULL;
	}

	scrCompileGlob.breakChildBlocks = breakChildBlocks;

	Scr_CopyBlock(block, &forStatBlock->block);
	Scr_CopyBlock(block, &forStatPostBlock->block);

	Scr_CalcLocalVarsStatement(stmt, forStatBlock->block);
	Scr_AddContinueBlock(forStatBlock->block);

	for ( i = 0; i < continueChildCount; i++ )
	{
		Scr_AppendChildBlocks(&continueChildBlocks[i], 1, block);
	}

	Scr_CalcLocalVarsStatement(stmt2, forStatPostBlock->block);

	Scr_AppendChildBlocks(&forStatPostBlock->block, 1, block);
	Scr_MergeChildBlocks(&forStatPostBlock->block, 1, block);

	if ( constConditional )
	{
		Scr_AppendChildBlocks(breakChildBlocks, breakChildCount, block);
	}

	Scr_MergeChildBlocks(&forStatBlock->block, 1, block);

	scrCompileGlob.breakChildBlocks = oldBreakChildBlocks;
	scrCompileGlob.breakChildCount = oldBreakChildCount;

	scrCompileGlob.continueChildBlocks = oldContinueChildBlocks;
	scrCompileGlob.continueChildCount = oldContinueChildCount;
}

/*
============
EmitIncStatement
============
*/
void EmitIncStatement( sval_u expr, sval_u sourcePos, scr_block_s *block )
{
	assert(!scrCompileGlob.forceNotCreate);
	scrCompileGlob.forceNotCreate = true;
	EmitVariableExpressionRef(expr, block);

	assert(scrCompileGlob.forceNotCreate);
	scrCompileGlob.forceNotCreate = false;
	EmitOpcode(OP_inc, 1, CALL_NONE);

	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_NONE);
	EmitSetVariableField(sourcePos);
}

/*
============
Scr_CalcLocalVarsIncStatement
============
*/
void Scr_CalcLocalVarsIncStatement( sval_u expr, scr_block_s *block )
{
	Scr_CalcLocalVarsVariableExpressionRef(expr, block);
}

/*
============
EmitDecStatement
============
*/
void EmitDecStatement( sval_u expr, sval_u sourcePos, scr_block_s *block )
{
	assert(!scrCompileGlob.forceNotCreate);
	scrCompileGlob.forceNotCreate = true;
	EmitVariableExpressionRef(expr, block);

	assert(scrCompileGlob.forceNotCreate);
	scrCompileGlob.forceNotCreate = false;
	EmitOpcode(OP_dec, 1, CALL_NONE);

	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_NONE);
	EmitSetVariableField(sourcePos);
}

/*
============
EmitFormalParameterListInternal
============
*/
void EmitFormalParameterListInternal( sval_u *node, scr_block_s *block )
{
	while ( 1 )
	{
		node = node[1].node;

		if ( !node )
		{
			break;
		}

		EmitSafeSetVariableField(node[0].node[0], node[0].node[1], block);
	}
}

/*
============
Scr_CalcLocalVarsFormalParameterListInternal
============
*/
void Scr_CalcLocalVarsFormalParameterListInternal( sval_u *node, scr_block_s *block )
{
	while ( 1 )
	{
		node = node[1].node;

		if ( !node )
		{
			break;
		}

		Scr_CalcLocalVarsSafeSetVariableField(node[0].node[0], node[0].node[1], block);
	}
}

/*
============
EmitFormalWaittillParameterListRefInternal
============
*/
void EmitFormalWaittillParameterListRefInternal( sval_u *node, scr_block_s *block )
{
	while ( 1 )
	{
		node = node[1].node;

		if ( !node )
		{
			break;
		}

		EmitSafeSetWaittillVariableField(node[0].node[0], node[0].node[1], block);
	}
}

/*
============
EmitWaittillStatement
============
*/
void EmitWaittillStatement( sval_u obj, sval_u exprlist, sval_u sourcePos, sval_u waitSourcePos, scr_block_s *block )
{
	sval_u *node = exprlist.node[0].node[1].node;
	assert(node);

	EmitExpression(node[0].node[0], block);
	EmitPrimitiveExpression(obj, block);
	EmitOpcode(OP_waittill, -2, CALL_NONE);

	AddOpcodePos(waitSourcePos.sourcePosValue, SOURCE_TYPE_NONE);
	AddOpcodePos(waitSourcePos.sourcePosValue, SOURCE_TYPE_NONE);

	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_NONE);
	AddOpcodePos(node[0].node[1].sourcePosValue, SOURCE_TYPE_NONE);

	EmitFormalWaittillParameterListRefInternal(node, block);
	EmitOpcode(OP_clearparams, 0, CALL_NONE);
}

/*
============
Scr_CalcLocalVarsWaittillStatement
============
*/
void Scr_CalcLocalVarsWaittillStatement( sval_u exprlist, scr_block_s *block )
{
	sval_u *node;

	node = exprlist.node[0].node[1].node;
	Scr_CalcLocalVarsFormalParameterListInternal(node, block);
}

/*
============
EmitWaittillmatchStatement
============
*/
void EmitWaittillmatchStatement( sval_u obj, sval_u exprlist, sval_u sourcePos, sval_u waitSourcePos, scr_block_s *block )
{
	sval_u *node;
	int exprCount;

	node = exprlist.node[0].node[1].node;
	assert(node);

	for ( exprCount = 0; ; exprCount++ )
	{
		node = node[1].node;

		if ( !node )
		{
			break;
		}

		EmitExpression(node[0].node[0], block);
	}

	node = exprlist.node[0].node[1].node;
	assert(node);

	EmitExpression(node[0].node[0], block);
	EmitPrimitiveExpression(obj, block);

	EmitOpcode(OP_waittillmatch, -2 - exprCount, CALL_NONE);

	AddOpcodePos(waitSourcePos.sourcePosValue, SOURCE_TYPE_NONE);
	AddOpcodePos(waitSourcePos.sourcePosValue, SOURCE_TYPE_NONE);

	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_NONE);
	AddOpcodePos(node[0].node[1].sourcePosValue, SOURCE_TYPE_NONE);

	while ( 1 )
	{
		node = node[1].node;

		if ( !node )
		{
			break;
		}

		AddOpcodePos(node[0].node[1].sourcePosValue, SOURCE_TYPE_NONE);
	}

	assert(exprCount < 256);

	EmitByte(exprCount);
	EmitOpcode(OP_clearparams, 0, CALL_NONE);
}

/*
============
EmitNotifyStatement
============
*/
void EmitNotifyStatement( sval_u obj, sval_u exprlist, sval_u sourcePos, sval_u notifySourcePos, scr_block_s *block )
{
	sval_u *start_node, *node;
	int expr_count;

	EmitOpcode(OP_voidCodepos, 1, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);

	expr_count = 0;
	start_node = NULL;

	for ( node = exprlist.node[0].node; node; node = node[1].node )
	{
		start_node = node;
		EmitExpression(node[0].node[0], block);
		expr_count++;
	}

	assert(start_node);

	EmitPrimitiveExpression(obj, block);
	EmitOpcode(OP_notify, -expr_count - 2, CALL_NONE);

	AddOpcodePos(notifySourcePos.sourcePosValue, SOURCE_TYPE_NONE);
	AddOpcodePos(start_node[0].node[1].sourcePosValue, SOURCE_TYPE_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_NONE);
}

/*
============
EmitEndOnStatement
============
*/
void EmitEndOnStatement( sval_u obj, sval_u expr, sval_u sourcePos, sval_u exprSourcePos, scr_block_s *block )
{
	EmitExpression(expr, block);
	EmitPrimitiveExpression(obj, block);
	EmitOpcode(OP_endon, -2, CALL_NONE);
	AddOpcodePos(exprSourcePos.sourcePosValue, SOURCE_TYPE_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_NONE);
}

/*
============
CompareCaseInfo
============
*/
int CompareCaseInfo( const void *elem1, const void *elem2 )
{
	if ( *(unsigned int *)elem1 > *(unsigned int *)elem2 )
	{
		return -1;
	}

	if ( *(unsigned int *)elem1 < *(unsigned int *)elem2 )
	{
		return 1;
	}

	return 0;
}

/*
============
Scr_IsLastStatement
============
*/
bool Scr_IsLastStatement( sval_u *node )
{
	if ( !node )
	{
		return true;
	}

	if ( scrVarPub.developer_script )
	{
		return false;
	}

	while ( node )
	{
		if ( node[0].node[0].type != ENUM_developer_statement_list )
		{
			return false;
		}

		node = node[1].node;
	}

	return true;
}

/*
============
EmitCaseStatement
============
*/
void EmitCaseStatement( sval_u expr, sval_u sourcePos )
{
	unsigned int name;

	switch ( expr.node[0].type )
	{
	case ENUM_integer:
		if ( !IsValidArrayIndex(expr.node[1].intValue) )
		{
			CompileError(sourcePos.sourcePosValue, va("case index %d out of range", expr.node[1].intValue));
			return;
		}

		name = GetInternalVariableIndex(expr.node[1].intValue);
		break;

	case ENUM_string:
		name = expr.node[1].stringValue;
		CompileTransferRefToString(expr.node[1].stringValue, 1);
		break;

	default:
		CompileError(sourcePos.sourcePosValue, "case expression must be an int or string");
		return;
	}

	EmitCaseStatementInfo(name, sourcePos);
}

/*
============
EmitDefaultStatement
============
*/
void EmitDefaultStatement( sval_u sourcePos )
{
	EmitCaseStatementInfo( 0, sourcePos );
}

void EmitSwitchStatementList( sval_u val, bool lastStatement, unsigned int endSourcePos, scr_block_s *block )
{
	sval_u *node, *nextNode;
	bool hasDefault;
	scr_block_s **breakChildBlocks;
	int breakChildCount;
	scr_block_s **oldBreakChildBlocks;
	int *oldBreakChildCount;
	scr_block_s *oldBreakBlock;

	oldBreakChildBlocks = scrCompileGlob.breakChildBlocks;
	oldBreakChildCount = scrCompileGlob.breakChildCount;
	oldBreakBlock = scrCompileGlob.breakBlock;

	breakChildCount = 0;
	breakChildBlocks = (scr_block_s **)Hunk_AllocateTempMemoryHighInternal( sizeof( scr_block_s ** ) * MAX_SWITCH_CASES );

	scrCompileGlob.breakChildBlocks = breakChildBlocks;
	scrCompileGlob.breakChildCount = &breakChildCount;

	scrCompileGlob.breakBlock = NULL;
	hasDefault = false;

	for ( node = val.node[0].node[1].node; node; node = nextNode )
	{
		nextNode = node[1].node;

		if ( node[0].node[0].type == ENUM_case || node[0].node[0].type == ENUM_default )
		{
			if ( scrCompileGlob.breakBlock )
			{
				assert(scrCompileGlob.bCanBreak);
				scrCompileGlob.bCanBreak = false;
				EmitRemoveLocalVars(scrCompileGlob.breakBlock, scrCompileGlob.breakBlock);
			}

			if ( node[0].node[0].type == ENUM_case )
			{
				scrCompileGlob.breakBlock = node[0].node[3].block;
				EmitCaseStatement(node[0].node[1], node[0].node[2]);
			}

			else
			{
				scrCompileGlob.breakBlock = node[0].node[2].block;
				hasDefault = true;
				EmitDefaultStatement(node[0].node[1]);
			}

			Scr_TransferBlock(block, scrCompileGlob.breakBlock);
			assert(!scrCompileGlob.bCanBreak);
			scrCompileGlob.bCanBreak = true;
		}
		else
		{
			if ( !scrCompileGlob.breakBlock )
			{
				CompileError(endSourcePos, "missing case statement");
				return;
			}

			EmitStatement(node[0], lastStatement && Scr_IsLastStatement(nextNode), endSourcePos, scrCompileGlob.breakBlock);

			if ( !(scrCompileGlob.breakBlock && scrCompileGlob.breakBlock->abortLevel) )
			{
			}
			else
			{
				scrCompileGlob.breakBlock = NULL;
				assert(scrCompileGlob.bCanBreak);
				scrCompileGlob.bCanBreak = false;
			}
		}
	}

	if ( scrCompileGlob.breakBlock )
	{
		assert(scrCompileGlob.bCanBreak);
		scrCompileGlob.bCanBreak = false;
		EmitRemoveLocalVars(scrCompileGlob.breakBlock, scrCompileGlob.breakBlock);
	}

	if ( hasDefault )
	{
		if ( scrCompileGlob.breakBlock )
		{
			Scr_AddBreakBlock(scrCompileGlob.breakBlock);
		}

		Scr_InitFromChildBlocks(breakChildBlocks, breakChildCount, block);
	}

	scrCompileGlob.breakChildBlocks = oldBreakChildBlocks;
	scrCompileGlob.breakChildCount = oldBreakChildCount;
	scrCompileGlob.breakBlock = oldBreakBlock;
}

/*
============
Scr_CalcLocalVarsSwitchStatement
============
*/
void Scr_CalcLocalVarsSwitchStatement( sval_u stmtlist, scr_block_s *block )
{
	sval_u *node;
	scr_block_s **breakChildBlocks;
	int breakChildCount;
	scr_block_s **oldBreakChildBlocks;
	int *oldBreakChildCount;
	scr_block_s *currentBlock;
	bool hasDefault;
	int abortLevel = SCR_ABORT_RETURN;
	scr_block_s **childBlocks;
	int childCount;

	oldBreakChildBlocks = scrCompileGlob.breakChildBlocks;
	oldBreakChildCount = scrCompileGlob.breakChildCount;

	breakChildCount = 0;
	breakChildBlocks = (scr_block_s **)Hunk_AllocateTempMemoryHighInternal( sizeof( scr_block_s ** ) * MAX_SWITCH_CASES );

	scrCompileGlob.breakChildBlocks = breakChildBlocks;
	scrCompileGlob.breakChildCount = &breakChildCount;

	childCount = 0;
	currentBlock = NULL;

	hasDefault = false;
	childBlocks = (scr_block_s **)Hunk_AllocateTempMemoryHighInternal( sizeof( scr_block_s ** ) * MAX_SWITCH_CASES );

	for ( node = stmtlist.node[0].node[1].node; node; node = node[1].node )
	{
		if ( node[0].node[0].type == ENUM_case || node[0].node[0].type == ENUM_default )
		{
			currentBlock = NULL;
			Scr_CopyBlock(block, &currentBlock);

			if ( node[0].node[0].type == ENUM_case )
			{
				node[0].node[3].block = currentBlock;
			}
			else
			{
				node[0].node[2].block = currentBlock;
				hasDefault = true;
			}
		}
		else if ( !currentBlock )
		{
		}
		else
		{
			Scr_CalcLocalVarsStatement(node[0], currentBlock);

			if ( !(currentBlock->abortLevel != SCR_ABORT_NONE) )
			{
			}
			else
			{
				if ( currentBlock->abortLevel == SCR_ABORT_BREAK )
				{
					currentBlock->abortLevel = SCR_ABORT_NONE;
					abortLevel = SCR_ABORT_NONE;

					Scr_CheckMaxSwitchCases(childCount);

					childBlocks[childCount] = currentBlock;
					childCount++;
				}
				else if ( currentBlock->abortLevel <= abortLevel )
				{
					abortLevel = currentBlock->abortLevel;
				}

				currentBlock = NULL;
			}
		}
	}

	if ( hasDefault )
	{
		if ( currentBlock )
		{
			Scr_AddBreakBlock(currentBlock);
			Scr_CheckMaxSwitchCases(childCount);

			childBlocks[childCount] = currentBlock;
			childCount++;
		}

		if ( block->abortLevel == SCR_ABORT_NONE )
		{
			block->abortLevel = abortLevel;
		}

		Scr_AppendChildBlocks(breakChildBlocks, breakChildCount, block);
		Scr_MergeChildBlocks(childBlocks, childCount, block);
	}

	scrCompileGlob.breakChildBlocks = oldBreakChildBlocks;
	scrCompileGlob.breakChildCount = oldBreakChildCount;
}

/*
============
EmitSwitchStatement
============
*/
void EmitSwitchStatement( sval_u expr, sval_u stmtlist, sval_u sourcePos, bool lastStatement, unsigned int endSourcePos, scr_block_s *block )
{
	bool oldbCanIgnoreCase;
	CaseStatementInfo *oldCaseStatement;
	CaseStatementInfo *caseStatement;
	bool oldbCanBreak;
	bool oldbCanIgnoreBreak;
	BreakStatementInfo *oldBreakStatement;
	const char *pos1;
	const char *pos2;
	char *pos3;
	const char *nextPos1;
	int num;

	oldbCanIgnoreCase = scrCompileGlob.bCanIgnoreCase;
	oldCaseStatement = scrCompileGlob.currentCaseStatement;

	scrCompileGlob.bCanIgnoreCase = false;
	oldbCanBreak = scrCompileGlob.bCanBreak;

	oldbCanIgnoreBreak = scrCompileGlob.bCanIgnoreBreak;
	oldBreakStatement = scrCompileGlob.currentBreakStatement;

	scrCompileGlob.bCanBreak = false;
	scrCompileGlob.bCanIgnoreBreak = false;

	EmitExpression(expr, block);
	EmitOpcode(OP_switch, -1, CALL_NONE);
	EmitInteger(0);

	pos1 = (const char *)scrCompileGlob.codePos;
	nextPos1 = TempMalloc(0);

	scrCompileGlob.bCanIgnoreCase = scrCompilePub.developer_statement != SCR_DEV_NO;
	scrCompileGlob.currentCaseStatement = NULL;

	scrCompileGlob.bCanIgnoreBreak = scrCompilePub.developer_statement != SCR_DEV_NO;
	scrCompileGlob.currentBreakStatement = NULL;

	EmitSwitchStatementList(stmtlist, lastStatement, endSourcePos, block);

	scrCompileGlob.bCanIgnoreCase = false;
	scrCompileGlob.bCanIgnoreBreak = false;

	EmitOpcode(OP_endswitch, 0, CALL_NONE);
	AddOpcodePos(sourcePos.stringValue, SOURCE_TYPE_NONE);
	EmitShort(0);

	pos2 = (const char *)scrCompileGlob.codePos;
	*(intptr_t *)pos1 = scrCompileGlob.codePos - (byte *)nextPos1;
	pos3 = TempMallocAlignStrict(0);

	for ( num = 0, caseStatement = scrCompileGlob.currentCaseStatement; caseStatement; caseStatement = caseStatement->next, num++ )
	{
		EmitInteger(caseStatement->name);
		EmitCodepos(caseStatement->codePos);
	}

	*(unsigned short *)pos2 = num;
	qsort(pos3, num, 8, CompareCaseInfo);

	// FIXME: This is bad!!
	while ( num > 1 )
	{
		if ( *(intptr_t *)pos3 == *((intptr_t *)pos3 + 2) )
		{
			for ( caseStatement = scrCompileGlob.currentCaseStatement; caseStatement; caseStatement = caseStatement->next )
			{
				if ( caseStatement->name == *(intptr_t *)pos3 )
				{
					CompileError(caseStatement->sourcePos, "duplicate case expression");
					return;
				}
			}
		}

		--num;
		pos3 += 8;
	}

	ConnectBreakStatements();

	scrCompileGlob.bCanIgnoreCase = oldbCanIgnoreCase;
	scrCompileGlob.currentCaseStatement = oldCaseStatement;

	scrCompileGlob.bCanBreak = oldbCanBreak;
	scrCompileGlob.bCanIgnoreBreak = oldbCanIgnoreBreak;

	scrCompileGlob.currentBreakStatement = oldBreakStatement;
}

/*
============
EmitCaseStatementInfo
============
*/
void EmitCaseStatementInfo( unsigned int name, sval_u sourcePos )
{
	CaseStatementInfo *newCaseStatement;

	if ( scrCompilePub.developer_statement == SCR_DEV_IGNORE )
	{
		assert(!scrVarPub.developer_script);
		return;
	}

	newCaseStatement = (CaseStatementInfo *)Hunk_AllocateTempMemoryHighInternal(sizeof(*newCaseStatement));

	newCaseStatement->name = name;
	newCaseStatement->codePos = (char *)TempMalloc(0);
	newCaseStatement->sourcePos = sourcePos.sourcePosValue;
	newCaseStatement->next = scrCompileGlob.currentCaseStatement;

	scrCompileGlob.currentCaseStatement = newCaseStatement;
}

/*
============
EmitBreakStatement
============
*/
void EmitBreakStatement( sval_u sourcePos, scr_block_s *block )
{
	BreakStatementInfo *newBreakStatement;

	if ( !scrCompileGlob.bCanBreak || block->abortLevel != SCR_ABORT_NONE )
	{
		CompileError(sourcePos.sourcePosValue, "illegal break statement");
		return;
	}

	assert(scrCompileGlob.breakBlock);
	Scr_AddBreakBlock(block);
	EmitRemoveLocalVars(block, scrCompileGlob.breakBlock);

	block->abortLevel = SCR_ABORT_BREAK;

	EmitOpcode(OP_jump, 0, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);

	EmitInteger(0);

	newBreakStatement = (BreakStatementInfo *)Hunk_AllocateTempMemoryHighInternal( sizeof( *newBreakStatement ) );
	newBreakStatement->codePos = (char *)scrCompileGlob.codePos;
	newBreakStatement->nextCodePos = TempMalloc(0);
	newBreakStatement->next = scrCompileGlob.currentBreakStatement;

	scrCompileGlob.currentBreakStatement = newBreakStatement;
}

/*
============
EmitContinueStatement
============
*/
void EmitContinueStatement( sval_u sourcePos, scr_block_s *block )
{
	ContinueStatementInfo *newContinueStatement;

	if ( !scrCompileGlob.bCanContinue || block->abortLevel != SCR_ABORT_NONE )
	{
		CompileError(sourcePos.stringValue, "illegal continue statement");
		return;
	}

	Scr_AddContinueBlock(block);
	EmitRemoveLocalVars(block, block);

	block->abortLevel = SCR_ABORT_CONTINUE;

	EmitOpcode(OP_jump, 0, CALL_NONE);
	AddOpcodePos(sourcePos.stringValue, SOURCE_TYPE_BREAKPOINT);

	EmitInteger(0);

	newContinueStatement = (ContinueStatementInfo *)Hunk_AllocateTempMemoryHighInternal( sizeof( *newContinueStatement ) );
	newContinueStatement->codePos = (char *)scrCompileGlob.codePos;
	newContinueStatement->nextCodePos = TempMalloc(0);
	newContinueStatement->next = scrCompileGlob.currentContinueStatement;

	scrCompileGlob.currentContinueStatement = newContinueStatement;
}

/*
============
EmitBreakpointStatement
============
*/
void EmitBreakpointStatement( sval_u sourcePos )
{
}

/*
============
EmitProfStatement
============
*/
void EmitProfStatement( sval_u profileName, sval_u sourcePos, unsigned char op )
{
	if ( !scrVarPub.developer_script )
	{
		Scr_CompileRemoveRefToString(profileName.stringValue);
		return;
	}

	Scr_CompileRemoveRefToString(profileName.stringValue);
	EmitOpcode(op, 0, CALL_NONE);
	EmitByte(0);
}

/*
============
EmitProfBeginStatement
============
*/
void EmitProfBeginStatement( sval_u profileName, sval_u sourcePos )
{
	EmitProfStatement(profileName, sourcePos, OP_prof_begin);
}

/*
============
EmitProfEndStatement
============
*/
void EmitProfEndStatement( sval_u profileName, sval_u sourcePos )
{
	EmitProfStatement(profileName, sourcePos, OP_prof_end);
}

void EmitStatement( sval_u val, bool lastStatement, unsigned int endSourcePos, scr_block_s *block )
{
	switch ( val.node[0].type )
	{
	case ENUM_assignment:
		EmitAssignmentStatement(val.node[1], val.node[2], val.node[3], val.node[4], block);
		break;

	case ENUM_call_expression_statement:
		EmitCallExpressionStatement(val.node[1], block);
		break;

	case ENUM_return:
		EmitReturnStatement(val.node[1], val.node[2], block);
		break;

	case ENUM_return2:
		EmitEndStatement(val.node[1], block);
		break;

	case ENUM_wait:
		EmitWaitStatement(val.node[1], val.node[2], val.node[3], block);
		break;

	case ENUM_if:
		EmitIfStatement(val.node[1], val.node[2], val.node[3], lastStatement, endSourcePos, block, &val.node[4]);
		break;

	case ENUM_if_else:
		EmitIfElseStatement(val.node[1], val.node[2], val.node[3], val.node[4], val.node[5], lastStatement, endSourcePos, block, &val.node[6], &val.node[7]);
		break;

	case ENUM_while:
		EmitWhileStatement(val.node[1], val.node[2], val.node[3], val.node[4], block, &val.node[5]);
		break;

	case ENUM_for:
		EmitForStatement(val.node[1], val.node[2], val.node[3], val.node[4], val.node[5], val.node[6], block, &val.node[7], &val.node[8]);
		break;

	case ENUM_inc:
		EmitIncStatement(val.node[1], val.node[2], block);
		break;

	case ENUM_dec:
		EmitDecStatement(val.node[1], val.node[2], block);
		break;

	case ENUM_binary_equals:
		EmitBinaryEqualsOperatorExpression(val.node[1], val.node[2], val.node[3], val.node[4], block);
		break;

	case ENUM_statement_list:
		EmitStatementList(val.node[1], lastStatement, endSourcePos, block);
		break;

	case ENUM_developer_statement_list:
		EmitDeveloperStatementList(val.node[1], val.node[2], block, &val.node[3]);
		break;

	case ENUM_waittill:
		EmitWaittillStatement(val.node[1], val.node[2], val.node[3], val.node[4], block);
		break;

	case ENUM_waittillmatch:
		EmitWaittillmatchStatement(val.node[1], val.node[2], val.node[3], val.node[4], block);
		break;

	case ENUM_waittillFrameEnd:
		EmitWaittillFrameEnd(val.node[1]);
		break;

	case ENUM_notify:
		EmitNotifyStatement(val.node[1], val.node[2], val.node[3], val.node[4], block);
		break;

	case ENUM_endon:
		EmitEndOnStatement(val.node[1], val.node[2], val.node[3], val.node[4], block);
		break;

	case ENUM_switch:
		EmitSwitchStatement(val.node[1], val.node[2], val.node[3], lastStatement, endSourcePos, block);
		break;

	case ENUM_case:
		CompileError(val.node[2].sourcePosValue, "illegal case statement");
		break;

	case ENUM_default:
		CompileError(val.node[1].sourcePosValue, "illegal default statement");
		break;

	case ENUM_break:
		EmitBreakStatement(val.node[1], block);
		break;

	case ENUM_continue:
		EmitContinueStatement(val.node[1], block);
		break;

	case ENUM_breakpoint:
		EmitBreakpointStatement(val.node[1]);
		break;

	case ENUM_prof_begin:
		EmitProfBeginStatement(val.node[1], val.node[2]);
		break;

	case ENUM_prof_end:
		EmitProfEndStatement(val.node[1], val.node[2]);
		break;

	default:
		return;
	}
}

void Scr_CalcLocalVarsStatement( sval_u val, scr_block_s *block )
{
	switch ( val.node[0].type )
	{
	case ENUM_assignment:
		Scr_CalcLocalVarsAssignmentStatement(val.node[1], val.node[2], block);
		break;

	case ENUM_return:
	case ENUM_return2:
		if ( block->abortLevel == SCR_ABORT_NONE )
			block->abortLevel = SCR_ABORT_RETURN;
		break;

	case ENUM_if:
		Scr_CalcLocalVarsIfStatement(val.node[2], block, &val.node[4]);
		break;

	case ENUM_if_else:
		Scr_CalcLocalVarsIfElseStatement(val.node[2], val.node[3], block, &val.node[6], &val.node[7]);
		break;

	case ENUM_while:
		Scr_CalcLocalVarsWhileStatement(val.node[1], val.node[2], block, &val.node[5]);
		break;

	case ENUM_for:
		Scr_CalcLocalVarsForStatement(val.node[1], val.node[2], val.node[3], val.node[4], block, &val.node[7], &val.node[8]);
		break;

	case ENUM_inc:
	case ENUM_dec:
		Scr_CalcLocalVarsIncStatement(val.node[1], block);
		break;

	case ENUM_binary_equals:
		Scr_CalcLocalVarsBinaryEqualsOperatorExpression(val.node[1], block);
		break;

	case ENUM_statement_list:
		Scr_CalcLocalVarsStatementList(val.node[1], block);
		break;

	case ENUM_developer_statement_list:
		Scr_CalcLocalVarsDeveloperStatementList(val.node[1], block, &val.node[3]);
		break;

	case ENUM_waittill:
		Scr_CalcLocalVarsWaittillStatement(val.node[2], block);
		break;

	case ENUM_switch:
		Scr_CalcLocalVarsSwitchStatement(val.node[2], block);
		break;

	case ENUM_break:
		Scr_AddBreakBlock(block);
		if ( block->abortLevel == SCR_ABORT_NONE )
			block->abortLevel = SCR_ABORT_BREAK;
		break;

	case ENUM_continue:
		Scr_AddContinueBlock(block);
		if ( block->abortLevel == SCR_ABORT_NONE )
			block->abortLevel = SCR_ABORT_CONTINUE;
		break;
	}
}

void EmitStatementList( sval_u val, bool lastStatement, unsigned int endSourcePos, scr_block_s *block )
{
	sval_u *next_node;
	sval_u *node;

	for ( next_node = val.node[0].node[1].node; next_node; next_node = node )
	{
		node = next_node[1].node;
		EmitStatement(next_node[0], lastStatement && Scr_IsLastStatement(node), endSourcePos, block);
	}
}

/*
============
Scr_CalcLocalVarsStatementList
============
*/
void Scr_CalcLocalVarsStatementList( sval_u val, scr_block_s *block )
{
	for ( sval_u *node = val.node[0].node[1].node; node; node = node[1].node )
	{
		Scr_CalcLocalVarsStatement( node[0], block );
	}
}

/*
============
Scr_CalcLocalVarsDeveloperStatementList
============
*/
void Scr_CalcLocalVarsDeveloperStatementList( sval_u val, scr_block_s *block, sval_u *devStatBlock )
{
	Scr_CopyBlock( block, &devStatBlock->block );
	Scr_CalcLocalVarsStatementList( val, devStatBlock->block );
	Scr_MergeChildBlocks( &devStatBlock->block, 1, block );
}

/*
============
EmitDeveloperStatementList
============
*/
void EmitDeveloperStatementList( sval_u val, sval_u sourcePos, scr_block_s *block, sval_u *devStatBlock )
{
	unsigned int savedChecksum;
	char *savedPos;

	if ( scrCompilePub.developer_statement != SCR_DEV_NO )
	{
		CompileError(sourcePos.sourcePosValue, "cannot recurse /#");
		return;
	}

	savedChecksum = scrVarPub.checksum;
	Scr_TransferBlock(block, devStatBlock->block);

	if ( !scrVarPub.developer_script )
	{
		savedPos = TempMalloc(0);
		scrCompilePub.developer_statement = SCR_DEV_IGNORE;
		EmitStatementList(val, false, 0, devStatBlock->block);
		TempMemorySetPos(savedPos);
	}
	else
	{
		scrCompilePub.developer_statement = SCR_DEV_YES;
		EmitStatementList(val, false, 0, devStatBlock->block);
		EmitRemoveLocalVars(devStatBlock->block, devStatBlock->block);
	}

	scrCompilePub.developer_statement = SCR_DEV_NO;
	scrVarPub.checksum = savedChecksum;
}

/*
============
EmitFormalParameterList
============
*/
void EmitFormalParameterList( sval_u exprlist, sval_u sourcePos, scr_block_s *block )
{
	EmitFormalParameterListInternal(exprlist.node[0].node, block);

	EmitOpcode(OP_checkclearparams, 0, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_NONE);
}

/*
============
Scr_CalcLocalVarsFormalParameterList
============
*/
void Scr_CalcLocalVarsFormalParameterList( sval_u exprlist, scr_block_s *block )
{
	Scr_CalcLocalVarsFormalParameterListInternal(exprlist.node[0].node, block);
}

/*
============
SpecifyThread
============
*/
void SpecifyThread( sval_u val )
{
	switch ( val.node[0].type )
	{
	case ENUM_begin_developer_thread:
		if ( scrCompileGlob.in_developer_thread )
		{
			CompileError(val.node[1].sourcePosValue, "cannot recurse /#");
			return;
		}

		scrCompileGlob.in_developer_thread = true;
		scrCompileGlob.developer_thread_sourcePos = val.node[1].sourcePosValue;
		break;

	case ENUM_end_developer_thread:
		if ( !scrCompileGlob.in_developer_thread )
		{
			CompileError(val.node[1].sourcePosValue, "#/ has no matching /#");
			return;
		}

		scrCompileGlob.in_developer_thread = false;
		break;

	case ENUM_thread:
		if ( scrCompileGlob.in_developer_thread && !scrVarPub.developer_script )
		{
			return;
		}

		SpecifyThreadPosition(GetObjectA( GetVariable( scrCompileGlob.fileId, val.node[1].idValue ) ),
			val.node[1].sourcePosValue, val.node[4].sourcePosValue,
			!scrCompileGlob.in_developer_thread ? VAR_CODEPOS : VAR_DEVELOPER_CODEPOS);
		break;
	}
}

/*
============
EmitThreadInternal
============
*/
void EmitThreadInternal( unsigned int threadId, sval_u val, sval_u sourcePos, sval_u endSourcePos, scr_block_s *block )
{
	scrCompileGlob.threadId = threadId;
	AddThreadStartOpcodePos(sourcePos.sourcePosValue);

	scrCompileGlob.cumulOffset = 0;
	scrCompileGlob.maxOffset = 0;
	scrCompileGlob.maxCallOffset = 0;

	CompileTransferRefToString(val.node[1].stringValue, 2);

	EmitFormalParameterList(val.node[2], sourcePos, block);
	EmitStatementList(val.node[3], true, endSourcePos.sourcePosValue, block);

	EmitEnd();

	AddOpcodePos(endSourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
	AddOpcodePos(0xFFFFFFFE, SOURCE_TYPE_NONE);

	assert(!scrCompileGlob.cumulOffset);

	if ( scrCompileGlob.maxOffset + MAX_VM_STACK_DEPTH * scrCompileGlob.maxCallOffset >= MAX_VM_OPERAND_STACK )
	{
		CompileError(sourcePos.sourcePosValue, "function exceeds operand stack size");
	}
}

/*
============
Scr_CalcLocalVarsThread
============
*/
void Scr_CalcLocalVarsThread( sval_u exprlist, sval_u stmtlist, sval_u *stmttblock )
{
	scrCompileGlob.forceNotCreate = false;

	stmttblock->block = (scr_block_s *)Hunk_AllocateTempMemoryHighInternal( sizeof( *stmttblock->block ) );

	stmttblock->block->abortLevel = SCR_ABORT_NONE;
	stmttblock->block->localVarsCreateCount = 0;
	stmttblock->block->localVarsCount = 0;
	stmttblock->block->localVarsPublicCount = 0;

	memset(stmttblock->block->localVarsInitBits, 0, sizeof(stmttblock->block->localVarsInitBits));

	Scr_CalcLocalVarsFormalParameterList(exprlist, stmttblock->block);
	Scr_CalcLocalVarsStatementList(stmtlist, stmttblock->block);
}

/*
============
InitThread
============
*/
void InitThread( int type )
{
	scrCompileGlob.bCanIgnoreCase = false;
	scrCompileGlob.currentCaseStatement = NULL;
	scrCompileGlob.bCanBreak = false;
	scrCompileGlob.bCanIgnoreBreak = false;
	scrCompileGlob.currentBreakStatement = NULL;
	scrCompileGlob.bCanContinue = false;
	scrCompileGlob.bCanIgnoreContinue = false;
	scrCompileGlob.currentContinueStatement = NULL;
	scrCompileGlob.breakChildBlocks = NULL;
	scrCompileGlob.continueChildBlocks = NULL;

	if ( !scrCompileGlob.firstThread[type] )
	{
		return;
	}

	scrCompileGlob.firstThread[type] = false;
	EmitEnd();

	AddOpcodePos(0, SOURCE_TYPE_NONE);
	AddOpcodePos(0xFFFFFFFE, SOURCE_TYPE_NONE);
}

/*
============
EmitNormalThread
============
*/
void EmitNormalThread( sval_u val, sval_u *stmttblock )
{
	unsigned int threadId;

	InitThread(0);

	threadId = FindObject(FindVariable(scrCompileGlob.fileId, val.node[1].sourcePosValue));

	SetThreadPosition(threadId, val.node[4]);
	EmitThreadInternal(threadId, val, val.node[4], val.node[5], stmttblock->block);
}

/*
============
EmitDeveloperThread
============
*/
void EmitDeveloperThread( sval_u val, sval_u *stmttblock )
{
	unsigned int savedChecksum;
	unsigned int threadId;
	char *begin_pos;

	assert(scrCompilePub.developer_statement == SCR_DEV_NO);

	if ( !scrVarPub.developer_script )
	{
		begin_pos = TempMalloc(0);
		savedChecksum = scrVarPub.checksum;

		scrCompilePub.developer_statement = SCR_DEV_IGNORE;
		InitThread(1);

		EmitThreadInternal(0, val, val.node[4], val.node[5], stmttblock->block);

		TempMemorySetPos(begin_pos);
		scrVarPub.checksum = savedChecksum;
	}
	else
	{
		scrCompilePub.developer_statement = SCR_DEV_YES;
		InitThread(1);

		threadId = FindObject(FindVariable(scrCompileGlob.fileId, val.node[1].sourcePosValue));

		SetThreadPosition(threadId, val.node[4]);
		EmitThreadInternal(threadId, val, val.node[4], val.node[5], stmttblock->block);
	}

	scrCompilePub.developer_statement = SCR_DEV_NO;
}

/*
============
EmitThread
============
*/
void EmitThread( sval_u val )
{
	switch ( val.node[0].type )
	{
	case ENUM_begin_developer_thread:
		scrCompileGlob.in_developer_thread = true;
		break;

	case ENUM_end_developer_thread:
		scrCompileGlob.in_developer_thread = false;
		break;

	case ENUM_thread:
		Scr_CalcLocalVarsThread( val.node[2], val.node[3], &val.node[6] );

		if ( !scrCompileGlob.in_developer_thread )
			EmitNormalThread( val, &val.node[6] );
		else
			EmitDeveloperThread( val, &val.node[6] );
		break;

	case ENUM_usingtree:
		if ( scrCompileGlob.in_developer_thread )
			CompileError( val.node[2].sourcePosValue, "cannot put #using_animtree inside /# ... #/ comment" );
		else
		{
			Scr_UsingTree( SL_ConvertToString( val.node[1].stringValue ), val.node[3].sourcePosValue );
			Scr_CompileRemoveRefToString( val.node[1].stringValue );
		}
		break;
	}
}

/*
============
EmitThreadList
============
*/
void EmitThreadList( sval_u val )
{
	sval_u *node;

	scrCompileGlob.in_developer_thread = false;

	for ( node = val.node[0].node[1].node; node; node = node[1].node )
	{
		SpecifyThread(node[0]);
	}

	if ( scrCompileGlob.in_developer_thread )
	{
		CompileError(scrCompileGlob.developer_thread_sourcePos, "/# has no matching #/");
	}

	scrCompileGlob.firstThread[0] = true;
	scrCompileGlob.firstThread[1] = true;

	assert(!scrCompileGlob.in_developer_thread);

	for ( node = val.node[0].node[1].node; node; node = node[1].node )
	{
		EmitThread(node[0]);
	}

	assert(!scrCompileGlob.in_developer_thread);
}

/*
============
EmitInclude
============
*/
void EmitInclude( sval_u val )
{
	assert( val.node[0].type == ENUM_include );

	unsigned int filename = Scr_CreateCanonicalFilename( SL_ConvertToString( val.node[1].stringValue ) );
	Scr_CompileRemoveRefToString( val.node[1].stringValue );

	AddFilePrecache( filename, val.node[2].sourcePosValue, true );
}

/*
============
EmitIncludeList
============
*/
void EmitIncludeList( sval_u val )
{
	for ( sval_u *node = val.node[0].node[1].node; node; node = node[1].node )
	{
		EmitInclude(node[0]);
	}
}

/*
============
ScriptCompile
============
*/
void ScriptCompile( sval_u val, unsigned int fileId, unsigned int scriptId )
{
	void *ptr;
	int i;
	int j;
	int func_count;
	unsigned short filename;
	PrecacheEntry *precachescript;
	PrecacheEntry *precachescript2;
	int includeFilePosId;
	int k;
	int object;
	VariableValue value;
	unsigned short name;
	int posId;
	VariableValue includePos;
	int toThreadCountId;
	int includeThreadId;

	scrCompileGlob.fileId = fileId;
	scrCompileGlob.bConstRefCount = 0;

	scrAnimPub.animTreeIndex = 0;
	scrCompilePub.developer_statement = SCR_DEV_NO;

	ptr = scrCompilePub.far_function_count ? (PrecacheEntry *)Z_Malloc( sizeof( *precachescript ) * scrCompilePub.far_function_count ) : NULL;

	scrCompileGlob.precachescriptList = (PrecacheEntry *)ptr;

	if ( ptr )
	{
		((PrecacheEntry *)ptr)->next = scrCompileGlob.precachescriptListHead;
		scrCompileGlob.precachescriptListHead = (PrecacheEntry *)ptr;
	}

	EmitIncludeList(val.node[0]);
	EmitThreadList(val.node[1]);

	scrCompilePub.programLen = (char *)TempMalloc(0) - scrVarPub.programBuffer;
	Hunk_ClearTempMemoryHighInternal();

	func_count = scrCompilePub.far_function_count;

	for ( i = 0; i < func_count; i++ )
	{
		precachescript = (PrecacheEntry *)((char *)ptr + sizeof( *precachescript ) * i);

		filename = precachescript->filename;
		includeFilePosId = Scr_LoadScript(SL_ConvertToString(filename));

		if ( !includeFilePosId )
		{
			CompileError(precachescript->sourcePos, "Could not find script '%s'", SL_ConvertToString(filename));
			return;
		}

		SL_RemoveRefToString(filename);

		if ( !precachescript->include )
		{
			continue;
		}

		for ( j = i + 1; j < func_count; j++ )
		{
			precachescript2 = (PrecacheEntry *)((char *)ptr + sizeof( *precachescript2 ) * j);

			if ( !precachescript2->include )
			{
				break;
			}

			if ( precachescript2->filename != *(const unsigned short *)&filename )
			{
				continue;
			}

			CompileError(precachescript2->sourcePos, "Duplicate #include");
			return;
		}

		precachescript->include = false;

		for ( k = FindNextSibling(includeFilePosId); k; k = FindNextSibling(k) )
		{
			if ( GetObjectType(k) != VAR_POINTER )
			{
				continue;
			}

			object = FindObject(k);
			posId = FindVariable(object, 1);

			if ( !posId )
			{
				continue;
			}

			includePos = Scr_EvalVariable(posId);

			if ( includePos.type == VAR_INCLUDE_CODEPOS )
			{
				continue;
			}

			name = GetVariableName(k);
			toThreadCountId = GetObjectA( GetVariable( fileId, name ) );

			includeThreadId = SpecifyThreadPosition( toThreadCountId, name, precachescript->sourcePos, VAR_INCLUDE_CODEPOS );

			*GetVariableValueAddress( includeThreadId ) = *GetVariableValueAddress( posId );

			LinkThread(toThreadCountId, &includePos, false);
		}
	}

	if ( ptr )
	{
		scrCompileGlob.precachescriptListHead = ((PrecacheEntry *)ptr)->next;
		Z_Free(ptr);
	}

	LinkFile(fileId);

	value.type = VAR_INTEGER;
	SetVariableValue(scriptId, &value);
}

/*
============
Scr_CompileStatement
============
*/
void Scr_CompileStatement( sval_u parseData )
{
	EmitStatement( parseData, false, 0, NULL );
	EmitOpcode( OP_abort, 0, CALL_NONE );
}

/*
============
Scr_CompileShutdown
============
*/
void Scr_CompileShutdown()
{
	PrecacheEntry *entry;

	while ( scrCompileGlob.precachescriptListHead )
	{
		entry = scrCompileGlob.precachescriptListHead;
		scrCompileGlob.precachescriptListHead = entry->next;
		Z_FreeInternal(entry);
	}
}

/*
============
AddRefToValue
============
*/
void AddRefToValue( VariableValue *value )
{
	AddRefToValue(value->type, value->u);
}


