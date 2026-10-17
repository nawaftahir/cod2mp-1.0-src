#include "cmd.h"

#define	MAX_CMD_BUFFER	0x10000
#define	MAX_CMD_LINE	0x1000

typedef struct
{
	byte	*data;
	int		maxsize;
	int		cursize;
} cmd_t;

static int			cmd_wait;
cmd_t		cmd_texts[1];
cmd_t		*cmd_text = cmd_texts;
// each command buffer sits on a 128-byte boundary
static byte		cmd_text_buf[MAX_CMD_BUFFER] __attribute__((aligned(128)));

typedef struct cmd_function_s
{
	cmd_function_s *next;
	char *name;
	const char *autoCompleteDir;
	const char *autoCompleteExt;
	xcommand_t function;
} cmd_function_t;

static int cmd_argc;
static char *cmd_argv[MAX_STRING_TOKENS] __attribute__((aligned(128)));        // points into cmd_tokenized
static char cmd_tokenized[BIG_INFO_STRING + MAX_STRING_TOKENS] __attribute__((aligned(128)));         // will have 0 bytes inserted
static cmd_function_t *cmd_functions;      // possible commands to execute

void Cbuf_Execute( void );
void Cmd_Exec_f( void );
int Cmd_Argc( void );
const char *Cmd_Argv( int arg );
void Cmd_TokenizeString( const char *text_in );
void Cmd_ExecuteString( const char *text );

void Cmd_Wait_f( void );
void Cbuf_Init( void );
void Cbuf_AddText( const char *text );
void Cbuf_InsertText( const char *text );
void Cbuf_ExecuteText( int exec_when, const char *text );
void Cbuf_ExecuteInternal( void );
void Cbuf_Execute( void );
void Cmd_Exec_f( void );
void Cmd_Vstr_f( void );
void Cmd_Echo_f( void );
int Cmd_Argc( void );
const char *Cmd_Argv( int arg );
void Cmd_ArgvBuffer( int arg, char *buffer, int bufferLength );
char *Cmd_Args( int start );
void Cmd_ArgsBuffer( char *buffer, int bufferLength );
void Cmd_TokenizeString( const char *text_in );
cmd_function_t* Cmd_FindCommand( const char *cmdName );
void Cmd_AddCommand( const char *cmd_name, xcommand_t function );
void Cmd_SetAutoComplete( const char *cmdName, const char *dir, const char *ext );
void Cmd_Shutdown();
void Cmd_ForEach( void(*callback)(const char *s) );
const char **Cmd_GetAutoCompleteFileList( const char *cmdName, int *fileCount, int allocTrackType );
void Cmd_ExecuteString( const char *text );
void Cmd_List_f( void );
void Cmd_Init( void );

// the server command context: same token table, separate entry points

/*
============
Cmd_Argc
============
*/
int Cmd_Argc( void )
{
	return cmd_argc;
}

// unreferenced; original name unknown
void Cmd_EmptyStub( void )
{
}

/*
============
Cmd_Wait_f
Causes execution of the remainder of the command buffer to be delayed until
next frame.  This allows commands like:
bind g "cmd use rocket ; +attack ; wait ; -attack ; cmd use blaster"
============
*/
void Cmd_Wait_f( void )
{
	if ( Cmd_Argc() == 2 )
	{
		cmd_wait = atoi( Cmd_Argv( 1 ) );
	}
	else
	{
		cmd_wait = 1;
	}
}

/*
============
Cbuf_Init
============
*/
void Cbuf_Init( void )
{
	int i;

	for ( i = 0; i < 1; i++ )
	{
		cmd_texts[i].data = &cmd_text_buf[i * 0x10000];
		cmd_texts[i].maxsize = MAX_CMD_BUFFER;
		cmd_texts[i].cursize = 0;
	}
}

/*
============
Cbuf_AddText
Adds command text at the end of the buffer, does NOT add a final \n
============
*/
void Cbuf_AddText( const char *text )
{
	int l;

	l = strlen( text );

	if ( cmd_text->cursize + l >= cmd_text->maxsize )
	{
		Com_Printf( "Cbuf_AddText: overflow\n" );
		return;
	}
	memcpy( &cmd_text->data[cmd_text->cursize], text, l );
	cmd_text->cursize += l;
}

/*
============
Cbuf_InsertText
Adds command text immediately after the current command
Adds a \n to the text
============
*/
void Cbuf_InsertText( const char *text )
{
	int len;
	int i;

	len = strlen( text ) + 1;

	if ( len + cmd_text->cursize > cmd_text->maxsize )
	{
		Com_Printf( "Cbuf_InsertText overflowed\n" );
		return;
	}

	// move the existing command text
	for ( i = cmd_text->cursize - 1 ; i >= 0 ; i-- )
	{
		cmd_text->data[ i + len ] = cmd_text->data[ i ];
	}

	// copy the new text in
	memcpy( cmd_text->data, text, len - 1 );

	// add a \n
	cmd_text->data[ len - 1 ] = '\n';
	cmd_text->cursize += len;
}

/*
============
Cbuf_ExecuteText
============
*/
void Cbuf_ExecuteText( int exec_when, const char *text )
{
	switch ( exec_when )
	{
	case EXEC_NOW:
		if ( text && strlen( text ) > 0 )
		{
			Cmd_ExecuteString( text );
		}
		else
		{
			Cbuf_Execute();
		}
		break;
	case EXEC_INSERT:
		Cbuf_InsertText( text );
		break;
	case EXEC_APPEND:
		Cbuf_AddText( text );
		break;
	default:
		Com_Error( ERR_FATAL, "\x15" "Cbuf_ExecuteText: bad exec_when" );
	}
}

/*
============
Cbuf_ExecuteInternal
============
*/
void Cbuf_ExecuteInternal( void )
{
	int i;
	char    *text;
	char line[MAX_CMD_LINE];
	int quotes;

	while ( cmd_text->cursize )
	{
		if ( cmd_wait )
		{
			// skip out while text still remains in buffer, leaving it
			// for next frame
			cmd_wait--;
			break;
		}

		// find a \n or ; line break
		text = (char *)cmd_text->data;

		quotes = 0;
		for ( i = 0 ; i < cmd_text->cursize ; i++ )
		{
			if ( text[i] == '"' )
			{
				quotes++;
			}
			if ( !( quotes & 1 ) &&  text[i] == ';' )
			{
				break;  // don't break if inside a quoted string
			}
			if ( text[i] == '\n' || text[i] == '\r' )
			{
				break;
			}
		}

		if ( i >= ( MAX_CMD_LINE - 1 ) )
		{
			i = MAX_CMD_LINE - 1;
		}

		memcpy( line, text, i );
		line[i] = 0;

// delete the text from the command buffer and move remaining commands down
// this is necessary because commands (exec) can insert data at the
// beginning of the text buffer

		if ( i == cmd_text->cursize )
		{
			cmd_text->cursize = 0;
		}
		else
		{
			i++;
			cmd_text->cursize -= i;
			memmove( text, text + i, cmd_text->cursize );
		}

// execute the command line
		Cmd_ExecuteString( line );
	}
}

void Cbuf_Execute( void )
{
	Cbuf_ExecuteInternal();
}

/*
===============
Cmd_Exec_f
===============
*/
bool Cmd_ExecFromDisk( const char *filename )
{
	char *text;

	FS_ReadFile(filename, (void **)&text);

	if ( !text )
		return false;

	Com_Printf("execing %s\n", filename);
	Cbuf_InsertText(text);
	FS_FreeFile(text);

	return true;
}

void Cmd_Exec_f( void )
{
	char filename[MAX_QPATH];

	if ( Cmd_Argc() != 2 )
	{
		Com_Printf("exec <filename> : execute a script file\n");
		return;
	}

	I_strncpyz(filename, Cmd_Argv(1), sizeof(filename));
	Com_DefaultExtension(filename, sizeof(filename), ".cfg");

	if ( Cmd_ExecFromDisk(filename) )
	{
		return;
	}

	Com_Printf("couldn't exec %s\n", Cmd_Argv(1));
}

/*
===============
Cmd_Vstr_f
Inserts the current value of a variable as command text
===============
*/
void Cmd_Vstr_f( void )
{
	dvar_t *dvar;
	const char *cmd;

	if ( Cmd_Argc() != 2 )
	{
		Com_Printf("vstr <variablename> : execute a variable command\n");
		return;
	}

	cmd = Cmd_Argv(1);
	dvar = Dvar_FindVar(cmd);

	if ( !dvar )
	{
		Com_Printf("%s doesn't exist\n", cmd);
		return;
	}

	if ( dvar->type != DVAR_TYPE_STRING && dvar->type != DVAR_TYPE_ENUM )
	{
		Com_Printf("%s is not a string-based dvar\n", dvar->name);
		return;
	}

	Cbuf_InsertText( va( "%s\n", dvar->current.string ) );
}

/*
===============
Cmd_Echo_f

Just prints the rest of the line to the console (not registered in 1.0)
===============
*/
void Cmd_Echo_f( void )
{
	int i;

	for ( i = 1; i < Cmd_Argc(); i++ )
		Com_Printf( "^3%s ", Cmd_Argv( i ) );

	Com_Printf( "\n" );
}

int SV_Cmd_Argc( void )
{
	return cmd_argc;
}

/*
============
Cmd_Argv
============
*/
const char *Cmd_Argv( int arg )
{
	if ( (unsigned)arg >= cmd_argc )
	{
		return "";
	}

	return cmd_argv[arg];
}

const char *SV_Cmd_Argv( int arg )
{
	if ( (unsigned)arg >= cmd_argc )
	{
		return "";
	}

	return cmd_argv[arg];
}

/*
============
Cmd_ArgvBuffer
The interpreted versions use this because
they can't have pointers returned to them
============
*/
void Cmd_ArgvBuffer( int arg, char *buffer, int bufferLength )
{
	Q_strncpyz( buffer, Cmd_Argv( arg ), bufferLength );
}

void SV_Cmd_ArgvBuffer( int arg, char *buffer, int bufferLength )
{
	Q_strncpyz( buffer, SV_Cmd_Argv( arg ), bufferLength );
}

/*
============
Cmd_Args
Returns a single string containing argv(1) to argv(argc()-1)
============
*/
char *Cmd_Args( int start )
{
	static char cmd_args[MAX_STRING_CHARS] __attribute__((aligned(128)));
	int i;

	cmd_args[0] = 0;

	for ( i = start; i < cmd_argc; i++ )
	{
		I_strncat(cmd_args, sizeof(cmd_args), cmd_argv[i]);

		if ( i != cmd_argc - 1 )
			I_strncat(cmd_args, sizeof(cmd_args), " ");
	}

	return cmd_args;
}

/*
============
Cmd_ArgsBuffer
============
*/
void Cmd_ArgsBuffer( char *buffer, int bufferLength )
{
	I_strncpyz( buffer, Cmd_Args( 1 ), bufferLength );
}

/*
============
Cmd_TokenizeString
Parses the given string into command line tokens.
The text is copied to a seperate buffer and 0 characters
are inserted in the apropriate place, The argv array
will point into this temporary buffer.
============
*/
int Cmd_TokenizeStringInternal( const char *text_in, int max_tokens, char **argv, char *textOut )
{
	const char *str;
#ifdef TKN_DBG
	// FIXME TTimo blunt hook to try to find the tokenization of userinfo
	Com_DPrintf("Cmd_TokenizeString: %s\n", text_in);
#endif

	// clear previous args
	int argc = 0;

	str = text_in;

	for ( ;; )
	{
		if ( argc == MAX_STRING_TOKENS )
		{
			return 0;			// this is usually something malicious
		}

		if ( !--max_tokens )
			break;

		for ( ;; )
		{
			// skip whitespace
			while ( *str && *str <= ' ' )
			{
				str++;
			}
			if ( !*str )
			{
				return argc;			// all tokens parsed
			}

			// skip // comments
			if ( str[0] == '/' && str[1] == '/' )
			{
				return argc;			// all tokens parsed
			}

			// skip /* */ comments
			if ( str[0] == '/' && str[1] =='*' )
			{
				while ( *str && ( str[0] != '*' || str[1] != '/' ) )
				{
					str++;
				}
				if ( !*str )
				{
					return argc;		// all tokens parsed
				}
				str += 2;
			}
			else
			{
				break;			// we are ready to parse a token
			}
		}

		// handle quoted strings
		// NOTE TTimo this doesn't handle \" escaping
		if ( *str == '"' )
		{
			argv[argc++] = textOut;

			++str;
			for ( ; *str && *str != '"'; )
			{
				if ( *str == '\\' && str[1] == '"' )
					++str;

				*textOut++ = *str++;
			}

			*textOut++ = 0;

			if ( !*str )
				return argc;

			++str;

			if ( !*str )
				return argc;

			if ( *str <= ' ' )
				++str;
		}
		else
		{
			// regular token
			argv[argc] = textOut;
			argc++;

			// skip until whitespace, quote, or command
			while ( *str > ' ' )
			{
				if ( str[0] == '"' )
				{
					break;
				}

				if ( str[0] == '/' && str[1] == '/' )
				{
					break;
				}

				// skip /* */ comments
				if ( str[0] == '/' && str[1] =='*' )
				{
					break;
				}

				*textOut++ = *str++;
			}

			*textOut++ = 0;

			if ( !*str )
				return argc;

			if ( *str <= ' ' )
				++str;
		}
	}

	if ( !*str )
		return argc;

	argv[argc++] = textOut;

	while ( *str )
	{
		*textOut++ = *str++;
	}

	*textOut++ = 0;

	return argc;
}

void Cmd_TokenizeStringWithLimit( const char *text_in, int max_tokens )
{
	cmd_argc = Cmd_TokenizeStringInternal(text_in, max_tokens, cmd_argv, cmd_tokenized);
}

void SV_Cmd_TokenizeStringWithLimit( const char *text_in, int max_tokens )
{
	cmd_argc = Cmd_TokenizeStringInternal(text_in, max_tokens, cmd_argv, cmd_tokenized);
}

void Cmd_TokenizeString( const char *text_in )
{
	Cmd_TokenizeStringWithLimit(text_in, 0);
}

void SV_Cmd_TokenizeString( const char *text_in )
{
	SV_Cmd_TokenizeStringWithLimit(text_in, 0);
}

/*
==============
Cmd_FindCommand
==============
*/
cmd_function_t* Cmd_FindCommand( const char *cmdName )
{
	cmd_function_t *cmd;

	for (cmd = cmd_functions; cmd; cmd = cmd->next)
	{
		if (!strcmp(cmdName, cmd->name))
		{
			return cmd;
		}
	}

	return NULL;
}

/*
============
Cmd_AddCommand
============
*/
void Cmd_AddCommand( const char *cmd_name, xcommand_t function )
{
	cmd_function_t  *cmd;

	// fail if the command already exists
	cmd = Cmd_FindCommand(cmd_name);

	if (cmd)
	{
		if (function != NULL)
		{
			Com_Printf("Cmd_AddCommand: %s already defined\n", cmd_name);
		}
		return;
	}

	// use a small malloc to avoid zone fragmentation
	cmd = (cmd_function_t *)S_Malloc( sizeof( cmd_function_t ) );
	cmd->name = CopyString( cmd_name );
	cmd->function = function;
	cmd->next = cmd_functions;
	cmd_functions = cmd;
}

void Cmd_RemoveCommand(const char *cmd_name)
{
	cmd_function_t *cmd;
	cmd_function_t **back;

	back = &cmd_functions;
	while (1)
	{
		cmd = *back;
		if (!cmd)
			return;
		if (!strcmp(cmd_name, cmd->name))
		{
			*back = cmd->next;
			if (cmd->name)
				Z_Free(cmd->name);
			Z_Free(cmd);
			return;
		}
		back = &cmd->next;
	}
}

/*
==============
Cmd_SetAutoComplete
==============
*/
void Cmd_SetAutoComplete( const char *cmdName, const char *dir, const char *ext )
{
	cmd_function_t* cmd;

	cmd = Cmd_FindCommand(cmdName);
	cmd->autoCompleteDir = dir;
	cmd->autoCompleteExt = ext;
}

/*
==============
Cmd_Shutdown
==============
*/
void Cmd_Shutdown()
{
	cmd_function_t *func;

	while ( cmd_functions )
	{
		func = cmd_functions;
		cmd_functions = cmd_functions->next;
		Z_FreeInternal(func->name);
		Z_FreeInternal(func);
	}
}

/*
============
Cmd_ForEach
============
*/
void Cmd_ForEach( void(*callback)(const char *s) )
{
	cmd_function_t	*cmd;

	for (cmd=cmd_functions ; cmd ; cmd=cmd->next)
	{
		callback( cmd->name );
	}
}

/*
============
Cmd_GetAutoCompleteFileList
============
*/
const char **Cmd_GetAutoCompleteFileList( const char *cmdName, int *fileCount, int allocTrackType )
{
	cmd_function_t *cmd;

	*fileCount = 0;
	cmd = Cmd_FindCommand( cmdName );

	if ( !cmd->autoCompleteDir || !cmd->autoCompleteExt )
		return NULL;

	return (const char **)FS_ListFiles( cmd->autoCompleteDir, cmd->autoCompleteExt, FS_LIST_PURE_ONLY, fileCount, allocTrackType );
}

/*
============
Cmd_ExecuteString
A complete command line has been parsed, so try to execute it
============
*/
void Cmd_ExecuteString( const char *text )
{
	cmd_function_t  *cmd, **prev;

	// execute the command line
	Cmd_TokenizeString( text );

	if ( !Cmd_Argc() )
	{
		return;     // no tokens
	}

	// check registered command functions
	for ( prev = &cmd_functions ; *prev ; prev = &cmd->next )
	{
		cmd = *prev;

		if ( !I_stricmp( cmd_argv[0],cmd->name ) )
		{
			// rearrange the links so that the command will be
			// near the head of the list next time it is used
			*prev = cmd->next;
			cmd->next = cmd_functions;
			cmd_functions = cmd;

			// perform the action
			if ( !cmd->function )
			{
				// let the cgame or game handle it
				break;
			}
			else
			{
				cmd->function();
			}
			return;
		}
	}

	// check dvars
	if ( Dvar_Command() )
	{
		return;
	}
	// check client game commands
	if ( legacyHacks->cl_running && CL_GameCommand() )
	{
		return;
	}
	// check server game commands
	if ( com_sv_running && com_sv_running->current.boolean && SV_GameCommand() )
	{
		return;
	}
	// send it as a server command if we are connected
	// this will usually result in a chat message
	CL_ForwardCommandToServer( text );
}

void SV_Cmd_ExecuteString( const char *text )
{
	Cmd_ExecuteString( text );
}

/*
============
Cmd_List_f
============
*/
void Cmd_List_f( void )
{
	cmd_function_t  *cmd;
	int i;
	const char            *match;

	if ( Cmd_Argc() > 1 )
	{
		match = Cmd_Argv( 1 );
	}
	else
	{
		match = NULL;
	}

	i = 0;
	for ( cmd = cmd_functions ; cmd ; cmd = cmd->next )
	{
		if ( match && !Com_Filter( match, cmd->name, qfalse ) )
		{
			continue;
		}

		Com_Printf( "%s\n", cmd->name );
		i++;
	}
	Com_Printf( "%i commands\n", i );
}

/*
============
Cmd_Init
============
*/
void Cmd_Init( void )
{
	Cmd_AddCommand("cmdlist", Cmd_List_f);
	Cmd_AddCommand("exec", Cmd_Exec_f);
	Cmd_AddCommand("vstr", Cmd_Vstr_f);
	Cmd_AddCommand("wait", Cmd_Wait_f);
}

