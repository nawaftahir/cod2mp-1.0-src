#include "../qcommon/qcommon.h"
#include "script_public.h"

void Scr_BeginLoadAnimTrees( int user );

int Scr_IsInOpcodeMemory( const char *pos );
int Scr_IsInScriptMemory( const char *pos );
bool Scr_IsIdentifier( const char *token );
int Scr_GetFunctionHandle( const char *filename, const char *name );
void Scr_InitEvaluate();
void Scr_EndLoadEvaluate();
unsigned int SL_TransferToCanonicalString( unsigned int stringValue );
unsigned int SL_GetCanonicalString( const char *str );
void Scr_BeginLoadScripts();
void Scr_BeginLoadScriptsRemote( void );
void Scr_BeginLoadAnimTrees( int user );
int Scr_ScanFile( char *buf, int max_size );
unsigned int Scr_LoadScript( const char *filename );
void Scr_PostCompileScripts();
void Scr_EndLoadScripts();
void Scr_PrecacheAnimTrees( void *(*Alloc)(int), int user );
void Scr_EndLoadAnimTrees();
void Scr_FreeScripts( unsigned char sys );

/*
============
Scr_IsInOpcodeMemory
============
*/
int Scr_IsInOpcodeMemory( const char *pos )
{
	assert(scrVarPub.programBuffer);
	assert(pos);
	return pos - scrVarPub.programBuffer < scrCompilePub.programLen;
}

/*
============
Scr_IsInScriptMemory
============
*/
int Scr_IsInScriptMemory( const char *pos )
{
	if ( pos >= scrVarPub.programBuffer && pos < scrVarPub.endScriptBuffer )
		return 1;

	return 0;
}

/*
============
Scr_IsIdentifier
============
*/
bool Scr_IsIdentifier( const char *token )
{
	while ( token[0] )
	{
		if ( !isalnum(token[0]) && token[0] != '_' )
		{
			return false;
		}

		token++;
	}

	return true;
}

/*
============
Scr_GetFunctionHandle
============
*/
int Scr_GetFunctionHandle( const char *filename, const char *name )
{
	unsigned int filePosId;
	unsigned int posId;
	unsigned int id;
	unsigned int str;
	int offset;
	unsigned int name2;
	unsigned int codePos;
	VariableValue pos;
	unsigned int posObj;
	unsigned int posVar;

	name2 = Scr_CreateCanonicalFilename(filename);
	filePosId = FindVariable(scrCompilePub.scriptsPos, name2);
	SL_RemoveRefToString(name2);

	if ( !filePosId )
	{
		return 0;
	}

	id = FindObject(filePosId);
	str = SL_FindLowercaseString(name);

	if ( !str )
	{
		return 0;
	}

	posId = FindVariable(id, str);

	if ( !posId )
	{
		return 0;
	}

	if ( GetObjectType(posId) != VAR_POINTER )
	{
		return 0;
	}

	posObj = FindObject(posId);
	posVar = FindVariable(posObj, 1);
	pos = Scr_EvalVariable(posVar);
	codePos = pos.u.pointerValue;

	if ( !Scr_IsInOpcodeMemory((const char *)codePos) )
	{
		return 0;
	}

	offset = codePos - (unsigned int)scrVarPub.programBuffer;
	return offset;
}

/*
============
Scr_InitEvaluate
============
*/
void Scr_InitEvaluate()
{
	scrVarPub.canonicalStrMark = Hunk_SetMark();
	scrCompilePub.archivedCanonicalStringsBuf = (unsigned short *)Hunk_AllocInternal(0x20000);
	scrVarPub.canonicalStrCount = 0;
}

/*
============
Scr_EndLoadEvaluate
============
*/
void Scr_EndLoadEvaluate()
{
	scrCompilePub.archivedCanonicalStringsBuf = 0;
	Hunk_ClearHigh(scrVarPub.canonicalStrMark);
}

/*
============
SL_TransferToCanonicalString
============
*/
unsigned int SL_TransferToCanonicalString( unsigned int stringValue )
{
	unsigned int canonical;

	SL_TransferRefToUser(stringValue, 2);
	canonical = scrCompilePub.archivedCanonicalStringsBuf[stringValue];

	if ( canonical )
	{
		return canonical;
	}

	scrVarPub.canonicalStrCount++;
	scrCompilePub.archivedCanonicalStringsBuf[stringValue] = scrVarPub.canonicalStrCount;

	return scrVarPub.canonicalStrCount;
}

/*
============
SL_GetCanonicalString
============
*/
unsigned int SL_GetCanonicalString( const char *str )
{
	unsigned int canonical;

	canonical = scrCompilePub.archivedCanonicalStringsBuf[SL_FindString(str)];

	if ( canonical )
	{
		return canonical;
	}

	return SL_TransferToCanonicalString(SL_GetString_(str, 0, 15));
}

/*
============
Scr_BeginLoadScripts
============
*/
void Scr_BeginLoadScripts()
{
	assert(!scrCompilePub.script_loading);
	scrCompilePub.script_loading = true;

	Scr_InitOpcodeLookup();

	assert(!scrCompilePub.loadedscripts);
	scrCompilePub.loadedscripts = Scr_AllocArray();

	assert(!scrCompilePub.scriptsPos);
	scrCompilePub.scriptsPos = Scr_AllocArray();

	assert(!scrCompilePub.builtinFunc);
	scrCompilePub.builtinFunc = Scr_AllocArray();

	assert(!scrCompilePub.builtinMeth);
	scrCompilePub.builtinMeth = Scr_AllocArray();

	scrVarPub.programBuffer = (const char *)Hunk_AllocLowInternal(0);

	scrCompilePub.programLen = 0;
	scrVarPub.endScriptBuffer = 0;

	Scr_InitEvaluate();

	scrVarPub.fieldBuffer = NULL;
	scrCompilePub.value_count = 0;

	Scr_ClearErrorMessage();

	scrCompilePub.func_table_size = 0;

	Scr_BeginLoadAnimTrees(1);

	TempMemoryReset();
	//assert((!((int)scrVarPub.programBuffer & 31)));
}

/*
============
Scr_BeginLoadScriptsRemote
============
*/
void Scr_BeginLoadScriptsRemote( void )
{
	scrCompilePub.script_loading = true;
	Scr_InitOpcodeLookup();
	scrCompilePub.loadedscripts = Scr_AllocArray();
}

/*
============
Scr_BeginLoadAnimTrees
============
*/
void Scr_BeginLoadAnimTrees( int user )
{
	assert(!scrAnimPub.animtree_loading);
	scrAnimPub.animtree_loading = true;

	scrAnimPub.xanim_num[user] = 0;
	scrAnimPub.xanim_lookup[user][0].anims = NULL;

	assert(!scrAnimPub.animtrees);
	scrAnimPub.animtrees = Scr_AllocArray();
	scrAnimPub.animtree_node = 0;

	scrCompilePub.developer_statement = SCR_DEV_NO;
}

/*
============
Scr_ScanFile
============
*/
int Scr_ScanFile( char *buf, int max_size )
{
	char c;
	int n;

	c = '*';

	for ( n = 0; n < max_size && (c = *scrCompilePub.in_ptr++) != 0 && c != 10; n++ )
	{
		buf[n] = c;
	}

	if ( c == 10 )
	{
		buf[n++] = c;
	}
	else if ( !c )
	{
		if ( !scrCompilePub.parseBuf )
		{
			scrCompilePub.in_ptr--;
		}
		else
		{
			scrCompilePub.in_ptr = scrCompilePub.parseBuf;
			scrCompilePub.parseBuf = NULL;
		}
	}

	return n;
}

/*
============
Scr_LoadScript
============
*/
unsigned int Scr_LoadScript( const char *filename )
{
	const char *sourceBuffer;
	sval_u parseData;
	unsigned int name;
	char extFilename[MAX_QPATH];
	const char *oldFilename, *oldSourceBuf;
	unsigned int scriptId, filePosId, filePosPtr;

	assert(scrCompilePub.script_loading);
	assert(strlen( filename ) < MAX_QPATH);

	name = Scr_CreateCanonicalFilename(filename);

	if ( FindVariable(scrCompilePub.loadedscripts, name) )
	{
		SL_RemoveRefToString(name);
		filePosPtr = FindVariable(scrCompilePub.scriptsPos, name);

		return filePosPtr ? FindObject(filePosPtr) : 0;
	}

	scriptId = GetNewVariable(scrCompilePub.loadedscripts, name);
	SL_RemoveRefToString(name);

	Com_sprintf(extFilename, sizeof(extFilename), "%s.gsc", SL_ConvertToString(name));
	oldSourceBuf = scrParserPub.sourceBuf;

	sourceBuffer = Scr_AddSourceBuffer(SL_ConvertToString(name), extFilename, TempMalloc(0), true);

	if ( !sourceBuffer )
	{
		return 0;
	}

	scrAnimPub.animTreeNames = 0;
	scrCompilePub.far_function_count = 0;

	oldFilename = scrParserPub.scriptfilename;
	scrParserPub.scriptfilename = extFilename;

	scrCompilePub.in_ptr = "+";
	scrCompilePub.parseBuf = sourceBuffer;

	ScriptParse(&parseData, 0);

	filePosId = GetObjectA(GetVariable(scrCompilePub.scriptsPos, name));

	ScriptCompile(parseData, filePosId, scriptId);

	scrParserPub.scriptfilename = oldFilename;
	scrParserPub.sourceBuf = oldSourceBuf;

	return filePosId;
}

/*
============
Scr_PostCompileScripts
============
*/
void Scr_PostCompileScripts()
{
	assert( scrCompilePub.programLen == static_cast< uint >( TempMalloc( 0 ) - scrVarPub.programBuffer ) );
	assert( !scrAnimPub.animTreeNames );

	Hunk_ConvertTempToPermLowInternal();
}

/*
============
Scr_EndLoadScripts
============
*/
void Scr_EndLoadScripts()
{
	Scr_EndLoadEvaluate();
	SL_ShutdownSystem(2);

	scrCompilePub.script_loading = false;

	assert(scrCompilePub.loadedscripts);
	ClearObject(scrCompilePub.loadedscripts);
	RemoveRefToObject(scrCompilePub.loadedscripts);
	scrCompilePub.loadedscripts = 0;

	assert(scrCompilePub.scriptsPos);
	ClearObject(scrCompilePub.scriptsPos);
	RemoveRefToObject(scrCompilePub.scriptsPos);
	scrCompilePub.scriptsPos = 0;

	assert(scrCompilePub.builtinFunc);
	ClearObject(scrCompilePub.builtinFunc);
	RemoveRefToObject(scrCompilePub.builtinFunc);
	scrCompilePub.builtinFunc = 0;

	assert(scrCompilePub.builtinMeth);
	ClearObject(scrCompilePub.builtinMeth);
	RemoveRefToObject(scrCompilePub.builtinMeth);
	scrCompilePub.builtinMeth = 0;
}

/*
============
Scr_PrecacheAnimTrees
============
*/
void Scr_PrecacheAnimTrees( void *(*Alloc)(int), int user )
{
	for ( int i = 1; i <= (int)scrAnimPub.xanim_num[user]; i++ )
	{
		Scr_LoadAnimTreeAtIndex(i, Alloc, user);
	}
}

/*
============
Scr_EndLoadAnimTrees
============
*/
void Scr_EndLoadAnimTrees()
{
	assert(scrAnimPub.animtrees);
	ClearObject(scrAnimPub.animtrees);
	RemoveRefToObject(scrAnimPub.animtrees);
	scrAnimPub.animtrees = 0;

	if ( scrAnimPub.animtree_node )
	{
		RemoveRefToObject(scrAnimPub.animtree_node);
	}

	SL_ShutdownSystem(2);
	scrVarPub.endScriptBuffer = (const char *)Hunk_AllocLowInternal(0);

	scrAnimPub.animtree_loading = false;
}

/*
============
Scr_FreeScripts
============
*/
void Scr_FreeScripts( unsigned char sys )
{
	if ( scrCompilePub.script_loading )
	{
		scrCompilePub.script_loading = 0;
		Scr_EndLoadScripts();
	}

	if ( scrAnimPub.animtree_loading )
	{
		scrAnimPub.animtree_loading = 0;
		Scr_EndLoadAnimTrees();
	}

	SL_ShutdownSystem(1);
	Scr_ShutdownOpcodeLookup();

	scrVarPub.programBuffer = NULL;
	scrCompilePub.programLen = 0;

	scrVarPub.endScriptBuffer = NULL;
	scrVarPub.checksum = 0;
}

