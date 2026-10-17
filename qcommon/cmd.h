#pragma once

#include "qcommon.h"

// paramters for command buffer stuffing
typedef enum
{
	EXEC_NOW,			// don't return until completed, a VM should NEVER use this,
	// because some commands might cause the VM to be unloaded...
	EXEC_INSERT,		// insert at current position, but don't run yet
	EXEC_APPEND			// add to end of the command buffer (normal case)
} cbufExec_t;

void Cbuf_Init (void);
void Cmd_Init( void );
void Cmd_Shutdown();
int Cmd_Argc( void );
const char *Cmd_Argv( int arg );
char *Cmd_Args( int start );
void Cmd_ArgvBuffer( int arg, char *buffer, int bufferLength );
void Cmd_ForEach( void(*callback)(const char *s) );
void Cbuf_Execute( void );
void Cbuf_ExecuteText( int exec_when, const char *text );
void Cbuf_AddText( const char *text );
void Cbuf_InsertText( const char *text );
void Cmd_TokenizeString( const char *text_in );
void Cmd_ExecuteString( const char *text );
void Cmd_SetAutoComplete(const char *cmdName, const char *dir, const char *ext);
void Cmd_AddCommand( const char *cmd_name, xcommand_t function );
void Cmd_RemoveCommand( const char *cmd_name );

int SV_Cmd_Argc( void );
const char *SV_Cmd_Argv( int arg );
void SV_Cmd_ArgvBuffer( int arg, char *buffer, int bufferLength );
void SV_Cmd_ExecuteString( const char *text );
void SV_Cmd_TokenizeString( const char *text_in );
