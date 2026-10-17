#include "qcommon.h"
#include <float.h>
#include "cmd.h"

// common.c -- misc functions used in client and server

#include "../script/script_public.h"
#include "../stringed/stringed_public.h"
#include "../universal/universal_public.h"
#include "../xanim/xanim_public.h"

const dvar_t *com_statmon;

// storage with no remaining references; size from the layout, name unknown
static char com_unreferenced0[0x16c];

static errorParm_t com_errorType;
static int com_unreferenced1;
fileHandle_t logfile;

dvar_t *com_viewlog;
dvar_t *com_developer;
static dvar_t *com_developer_script;
static dvar_t *com_timescale;
float com_timescaleValue;
static dvar_t *com_fixedtime;
static dvar_t *com_maxfps;
dvar_t *com_sv_running;
dvar_t *com_logfile;
dvar_t *version;
dvar_t *shortversion = NULL;
dvar_t *com_introPlayed;
dvar_t *cl_paused;
dvar_t *nextmap = NULL;
dvar_t *com_expectedHunkUsage;
dvar_t *sv_paused;
static int com_unreferenced2;
static dvar_t *com_animCheck;
dvar_t *com_recommendedSet;

int com_frameTime;
static int com_lastFrameTime;
static float com_codeTimeScale;
static int com_frameNumber;
qboolean com_errorEntered;
int com_fixedConsolePosition;
static qboolean com_fullyInitialized;

char com_errorMessage[MAXPRINTMSG] __attribute__((aligned(128)));

static dvar_t *ui_errorMessage;
static dvar_t *ui_errorTitle;

#define MAX_PUSHED_EVENTS 256
static int com_pushedEventsHead;
static int com_pushedEventsTail;
static sysEvent_t com_pushedEvents[MAX_PUSHED_EVENTS] __attribute__((aligned(128)));

static char com_unreferenced3[0x488];

static qboolean com_safemode;
static int com_unreferenced4;
static int iWeaponInfoSource;

static char		*rd_buffer;
static size_t	rd_buffersize;
static void		(*rd_flush)( char *buffer );

/*
============================================================================

COMMAND LINE FUNCTIONS

+ characters seperate the commandLine string into multiple console
command lines.

All of these are valid:

quake3 +set test blah +map test
quake3 set test blah+map test
quake3 set test blah + map test

============================================================================
*/

#define MAX_CONSOLE_LINES   32

void SND_ShutdownChannels( void );

// 1.0 build date, baked into the startup banner
#define BUILD_DATE "Oct  6 2005"

extern "C" const char *getBuildNumber( void );
void FxMem_Init( void );
// empty in the dedicated build
inline void Com_SyncThreads( void )
{
}

void Com_StartHunkUsers( void );
unsigned char CL_PacketEvent( netadr_t from, msg_t *msg, int time );
void CL_KeyEvent( int key, const qboolean down, const unsigned int time );
void CL_CharEvent( int key );
void CL_InitKeyCommands( void );
void CL_InitOnceForAllClients( void );
void CL_Init( void );
void CL_InitRenderer( void );
void CL_StartHunkUsers( void );
void CL_ShutdownHunkUsers( void );
void CIN_CloseAllVideos( void );
void SND_Init( void );
void UI_SetMap( const char *mapname, const char *gametype );
void Sys_HideSplashWindow( void );
void Sys_DestroySplashWindow( void );
void IN_Frame( void );
void Sys_LoadingKeepAlive( void );

void Com_Printf( const char *fmt, ... );
void Com_Quit_f();
static void Com_WriteConfig_f( void );
static void Com_WriteDefaults_f( void );
void Com_CloseLogfiles();
void Com_Close();
void Com_Restart();

// tab completion is not built into the dedicated server
void Field_CompleteCommand( field_t *field )
{
}


// Leading members only; the full layouts live in oc/headers/common_types.h.
struct refexport_t
{
	void (*Shutdown)( int );
	void *unused0[61];
	void (*ResetImageAllocations)( void );
	void *unused1[39];
	void (*SyncRenderThread)( void );
	void (*AbortRenderCommands)( void );
};
struct clientStatic_t
{
	qboolean quit;
	qboolean hunkUsersStarted;
	char servername[256];
	qboolean rendererStarted;
	qboolean soundStarted;
	qboolean uiStarted;
	char unused0[2919008];
	int whiteMaterial;
	int consoleMaterial;
	int consoleFont;
};
struct clientConnection_t
{
	char unused0[264100];
	qboolean demorecording;
	qboolean demoplaying;
	int unused1[3];
	fileHandle_t demofile;
};
extern clientConnection_t clientConnections[1];
#define clc clientConnections[0]
extern refexport_t re;
extern clientStatic_t cls;
extern unsigned char updateScreenCalled;

/*
=============
Com_ShutdownInternal
=============
*/
void CL_ShutdownCGame( void );
qboolean CL_ShutdownUI( void );
void FS_FCloseFile( fileHandle_t f );


// errors that end a session without a drop dialog
const char *noticeErrors[] =
{
	"EXE_SERVER_DISCONNECTED",
	"EXE_DISCONNECTED",
	"EXE_SERVERISFULL",
	"XBOXLIVE_SIGNEDOUTOFLIVE",
	"XBOXLIVE_CANTJOINSESSION",
	"XBOXLIVE_MPNOTALLOWED",
	"XBOXLIVE_MUSTLOGIN",
	""
};

/*
=================
Com_SetErrorMessage
=================
*/
const char *SEH_LocalizeTextMessage( const char *pszInputBuffer, const char *pszMessageType, msgLocErrType_t errType );

static int com_unreferenced5;


qboolean UI_SetActiveMenu( int menu );
void SND_ErrorCleanup( void );
void CL_FlushDebugData( qboolean fromServer );
void CL_ConsoleFixPosition( void );
qboolean UI_AnyFullScreenMenuVisible( void );
void SEH_UpdateLanguageInfo( void );


static int com_numConsoleLines;
static char *com_consoleLines[MAX_CONSOLE_LINES];

#define CDKEY_REGISTRY_PATH "SOFTWARE\\Activision\\Call of Duty 2"

char cl_cdkey[34] = "                                ";
char cl_cdkeychecksum[5] = "    ";

qboolean CL_CDKeyValidate( const char *key, const char *checksum );



extern const dvar_t *com_statmon;



void Com_CheckSetRecommended( void );

/*
================
Com_WriteConfigToFile
================
*/
extern void Key_WriteBindings( fileHandle_t f );


extern const dvar_t *com_playerProfile;

/*
==================
Com_UpdateStatMonWarnings
==================
*/
extern int com_fileAccessed;
extern void StatMon_Warning( int type, int duration, const char *materialName );




/*
================
Com_CloseLogfiles
================
*/








extern dvar_t *loc_language;

/*
========================================================================

AUTOCONFIGURE

Picks CPU/GPU dvar presets from configure_mp.csv based on measured hardware,
and re-runs the pick whenever the hardware or the csv itself changes.

========================================================================
*/

struct SysInfo
{
	double cpuGHz;
	int sysMB;
	bool SSE;
	char gpuDescription[512];
};

extern void Sys_GetInfo( SysInfo *info );
extern bool Sys_HasConfigureChecksumChanged( int checksum );
extern bool Sys_HasInfoChanged( void );
extern void Sys_ArchiveInfo( int checksum );
extern dvar_t *Dvar_SetFromStringByNameFromSource( const char *dvarName, const char *string, DvarSetSource source );
extern bool Dvar_AnyLatchedValues( void );

/*
==================
Com_GpuStringCompare
==================
*/

/*
==================
Com_SkipConfigureBlankLines
==================
*/

/*
==================
Com_HasConfigureChecksumChanged
==================
*/
// retail calls FS_FreeFile here
#pragma inline_depth(0)
#pragma inline_depth()


/*
================
Com_BeginRedirect
================
*/
void Com_BeginRedirect( char *buffer, int buffersize, void (*flush)( char *) )
{
	if (!buffer || !buffersize || !flush)
		return;
	rd_buffer = buffer;
	rd_buffersize = buffersize;
	rd_flush = flush;

	*rd_buffer = 0;
}

/*
================
Com_EndRedirect
================
*/
void Com_EndRedirect( void )
{
	if ( rd_flush )
	{
		rd_flush(rd_buffer);
	}

	rd_buffer = NULL;
	rd_buffersize = 0;
	rd_flush = NULL;
}

/*
===================
Com_PrintMessage
===================
*/
void Com_PrintMessage( print_msg_type_t channel, const char *msg )
{
	static qboolean opening_qconsole = qfalse;

	if ( rd_buffer )
	{
		if (channel == PMSG_LOGFILE)
		{
			return;
		}

		if ((strlen (msg) + strlen(rd_buffer)) > (rd_buffersize - 1))
		{
			rd_flush(rd_buffer);
			*rd_buffer = 0;
		}

		I_strncat(rd_buffer, rd_buffersize, msg);
		// TTimo nooo .. that would defeat the purpose
		//rd_flush(rd_buffer);
		//*rd_buffer = 0;
		return;
	}

	// echo to console if we're not a dedicated server
	if ( channel != PMSG_LOGFILE && com_dedicated && !com_dedicated->current.integer )
	{
		CL_ConsolePrint(channel, msg, 0, 0);
	}

	if ( msg[0] == '^' && msg[1] )
		msg += 2;

	if ( channel != PMSG_LOGFILE )
	{
		// echo to dedicated console and early console
		Sys_Print( msg );
	}

	// logfile
	if ( com_logfile && com_logfile->current.integer && FS_Initialized() )
	{
		// TTimo: only open the qconsole.log if the filesystem is in an initialized state
		//   also, avoid recursing in the qconsole.log opening (i.e. if fs_debug is on)
		if ( !logfile && !opening_qconsole)
		{
			struct tm *newtime;
			time_t aclock;

			opening_qconsole = qtrue;

			time( &aclock );
			newtime = localtime( &aclock );

#ifdef DEDICATED
			logfile = FS_FOpenTextFileWrite( "console_mp_server.log" );
#else
			logfile = FS_FOpenTextFileWrite( "console_mp.log" );
#endif
			Com_Printf( "logfile opened on %s\n", asctime( newtime ) );

			opening_qconsole = qfalse;
		}

		if ( logfile )
		{
			FS_Write(msg, strlen(msg), logfile);

			if ( com_logfile->current.integer > 1 )
				FS_Flush(logfile);
		}
	}
}

/*
=============
Com_Printf

Both client and server can use this, and it will output
to the apropriate place.

A raw string should NEVER be passed as fmt, because of "%f" type crashers.
=============
*/
void Com_Printf( const char *fmt, ... )
{
	va_list		argptr;
	char		msg[MAXPRINTMSG];

	va_start (argptr,fmt);
	vsnprintf (msg, sizeof(msg), fmt, argptr);
	va_end (argptr);

	msg[MAXPRINTMSG - 1] = 0;
	Com_PrintMessage(PMSG_CONSOLE, msg);
}

/*
================
Com_DPrintf

A Com_Printf that only shows up if the "developer" cvar is set
================
*/
void Com_DPrintf( const char *fmt, ... )
{
	va_list		argptr;
	char		msg[MAXPRINTMSG];

	if ( !com_developer || !com_developer->current.integer )
	{
		return;			// don't confuse non-developers with techie stuff...
	}

	va_start( argptr, fmt );
	vsnprintf( msg, sizeof(msg), fmt, argptr );
	va_end( argptr );

	msg[MAXPRINTMSG - 1] = 0;

	Com_Printf( "%s", msg );
}

static void Com_ShutdownInternal( const char *finalmsg )
{
#ifndef DEDICATED
	CL_Disconnect();
	if ( re.SyncRenderThread )
		re.SyncRenderThread();
	if ( cls.hunkUsersStarted )
	{
		CL_ShutdownCGame();
		CL_ShutdownUI();
		cls.whiteMaterial = 0;
		cls.consoleMaterial = 0;
		cls.consoleFont = 0;
		cls.hunkUsersStarted = 0;
	}
	if ( re.Shutdown )
		re.Shutdown( 0 );
	cls.rendererStarted = 0;
	if ( clc.demofile )
	{
		FS_FCloseFile( clc.demofile );
		clc.demofile = 0;
		clc.demoplaying = 0;
		clc.demorecording = 0;
	}
#endif
	SV_Shutdown(finalmsg);
	Com_Restart();
}

/*
=================
Com_Shutdown
=================
*/
void Com_Shutdown( const char *finalmsg )
{
	Com_ShutdownInternal(finalmsg);
#ifndef DEDICATED
	UI_SetMap("", "");
	CL_StartHunkUsers();
#endif
}

/*
=================
Com_ClearTempMemory
=================
*/
void Com_ClearTempMemory()
{
	Hunk_ClearTempMemoryInternal();
	Hunk_ClearTempMemoryHighInternal();
}

/*
==================
Com_SetTimeScale
==================
*/
void Com_SetTimeScale( float timescale )
{
	com_codeTimeScale = timescale;
}

bool Com_IsNoticeError( const char *msg )
{
	int i;

	for ( i = 0; *noticeErrors[i]; i++ )
	{
		if ( !strcasecmp(noticeErrors[i], msg) )
			return true;
	}

	return false;
}

void Com_SetErrorMessage( const char *errorMessage )
{
	const char *title;
	const char *localized;

	ui_errorMessage = Dvar_RegisterString("com_errorMessage", "", DVAR_ROM | DVAR_CHANGEABLE_RESET);
	ui_errorTitle = Dvar_RegisterString("com_errorTitle", "", DVAR_ROM | DVAR_CHANGEABLE_RESET);

	if ( com_errorType != ERR_SERVERDISCONNECT && !Com_IsNoticeError(errorMessage) )
	{
		title = SEH_LocalizeTextMessage("MENU_ERROR", "error message", LOCMSG_NOERR);
		if ( title )
			Dvar_SetString(ui_errorTitle, title);
		else
			Dvar_SetString(ui_errorTitle, "MENU_ERROR");
	}
	else
	{
		title = SEH_LocalizeTextMessage("MENU_NOTICE", "error message", LOCMSG_NOERR);
		if ( title )
			Dvar_SetString(ui_errorTitle, title);
		else
			Dvar_SetString(ui_errorTitle, "MENU_NOTICE");
	}

	localized = SEH_LocalizeTextMessage(errorMessage, "error message", LOCMSG_NOERR);
	if ( localized )
	{
		Dvar_SetString(ui_errorMessage, localized);
		strncpy(com_errorMessage, localized, sizeof(com_errorMessage) - 1);
		com_errorMessage[sizeof(com_errorMessage) - 1] = 0;
	}
	else
	{
		Dvar_SetString(ui_errorMessage, errorMessage);
	}
}

/*
================
Com_ErrorCleanup
================
*/
void Com_ErrorCleanup( void )
{
	static int	lastErrorTime;
	static int	errorCount;
	int			currentTime;
	const char	*localized;
	char msgBuf[MAXPRINTMSG];

	LargeLocalReset();

	if ( re.AbortRenderCommands )
		re.AbortRenderCommands();

	Dvar_SetInAutoExec(0);
	Com_ClearTempMemory();
	Hunk_ClearTempMemoryHigh();
	Dvar_SetIntByName("cl_paused", 0);
	FS_PureServerSetLoadedIwds("", "");
	SEH_UpdateLanguageInfo();

	strcpy(msgBuf, com_errorMessage);

	if ( com_errorType != ERR_DISCONNECT )
	{
		if ( cls.uiStarted )
			UI_SetActiveMenu(0);

		Com_SetErrorMessage(com_errorMessage);
	}
	else if ( com_errorMessage[0] )
	{
		localized = SEH_LocalizeTextMessage(com_errorMessage, "error message", LOCMSG_NOERR);
		if ( localized )
			I_strncpyz(com_errorMessage, localized, sizeof(com_errorMessage));
	}

	if ( com_errorType != ERR_DISCONNECT )
		Scr_Abort();

	SND_ErrorCleanup();
	Com_CleanupBsp();
	CM_Cleanup();
	Com_ResetParseSessions();
	CL_FlushDebugData(qtrue);

	if ( re.ResetImageAllocations )
		re.ResetImageAllocations();

	FS_ResetFiles();

	if ( com_errorType == ERR_DROP )
		Cbuf_Init();

	// if we are getting a solid stream of ERR_DROP, do an ERR_FATAL
	currentTime = Sys_MilliSeconds();
	if ( currentTime - lastErrorTime < 100 )
	{
		if ( ++errorCount > 3 )
		{
			com_errorType = ERR_FATAL;
		}
	}
	else
	{
		errorCount = 0;
	}
	lastErrorTime = currentTime;

	if ( com_errorType != ERR_SERVERDISCONNECT && com_errorType != ERR_DROP && com_errorType != ERR_DISCONNECT )
		Sys_Error("%s", com_errorMessage);

	updateScreenCalled = 0;

	if ( com_errorType == ERR_SERVERDISCONNECT )
	{
		Com_ShutdownInternal("EXE_DISCONNECTEDFROMOWNLISTENSERVER");
	}
	else
	{
		Com_Printf("********************\nERROR: %s\n********************\n", com_errorMessage);

		if ( com_errorType == ERR_DROP && cls.uiStarted && !com_fixedConsolePosition )
			CL_ConsoleFixPosition();

		Com_ShutdownInternal(msgBuf);

		if ( com_errorType == ERR_DROP )
		{
			if ( QuitOnError() )
				Com_Quit_f();
		}
	}

	com_fixedConsolePosition = 0;
	com_errorEntered = 0;
}

/*
=============
Com_Error

Both client and server can use this, and it will
do the apropriate things.
=============
*/
void Com_Error( errorParm_t code, const char *fmt, ... )
{
	va_list		argptr;

	if ( com_errorEntered )
		Sys_Error("recursive error after: %s", com_errorMessage);

	com_errorEntered = 1;

	va_start(argptr, fmt);
	vsnprintf(com_errorMessage, sizeof(com_errorMessage), fmt, argptr);
	va_end (argptr);
	com_errorMessage[sizeof(com_errorMessage) - 1] = 0;

	if ( code == ERR_SCRIPT || code == ERR_LOCALIZATION )
	{
		if ( !com_fixedConsolePosition )
		{
			com_fixedConsolePosition = 1;
			CL_ConsoleFixPosition();
		}

		if ( cls.uiStarted )
		{
			if ( !UI_AnyFullScreenMenuVisible() )
			{
				Com_SetErrorMessage(com_errorMessage);
				UI_SetActiveMenu(1);
			}

			if ( cls.uiStarted )
			{
				com_errorEntered = 0;
				return;
			}
		}

		code = ERR_DROP;
	}
	else if ( code == ERR_SCRIPT_DROP )
	{
		com_fixedConsolePosition = 1;
		CL_ConsoleFixPosition();
		code = ERR_DROP;
	}
	else
	{
		com_fixedConsolePosition = 0;
	}

	com_errorType = code;
	longjmp(*(jmp_buf *)Sys_GetValue(THREAD_VALUE_COM_ERROR), -1);
}

void Com_ErrorAbort( void )
{
	int errorEntered;

	errorEntered = com_errorEntered;

	if ( errorEntered )
		longjmp(*(jmp_buf *)Sys_GetValue(THREAD_VALUE_COM_ERROR), -1);
}

/*
=============
Com_Quit_f

Both client and server can use this, and it will
do the apropriate things.
=============
*/
void Com_Quit_f()
{
	Com_Printf("quitting...\n");

	// don't try to shutdown if we are in a recursive error
	if ( !com_errorEntered )
	{
		Com_ClearTempMemory();
		Sys_DestroySplashWindow();
		CL_Shutdown();
		SV_Shutdown("EXE_SERVERQUIT");
		Com_Close();
		Com_CloseLogfiles();
		FS_Shutdown( 1 );
		FS_ShutdownServerIwdNames();
		FS_ShutdownServerReferencedIwds();
	}

	Sys_Quit();
}

/*
==================
Com_ParseCommandLine

Break it up into multiple console lines
==================
*/
void Com_ParseCommandLine( char *commandLine )
{
	com_consoleLines[0] = commandLine;
	com_numConsoleLines = 1;

	while ( *commandLine )
	{
		// look for a + seperating character
		// if commandLine came from a file, we might have real line seperators
		if ( *commandLine == '+' || *commandLine == '\n' )
		{
			if ( com_numConsoleLines == MAX_CONSOLE_LINES )
			{
				return;
			}
			com_consoleLines[com_numConsoleLines] = commandLine + 1;
			com_numConsoleLines++;
			*commandLine = 0;
		}
		commandLine++;
	}
}

/*
===================
Com_SafeMode

Check for "safe" on the command line, which will
skip loading of wolfconfig.cfg
===================
*/
qboolean Com_SafeMode( void )
{
	int i;

	for ( i = 0 ; i < com_numConsoleLines ; i++ )
	{
		Cmd_TokenizeString( com_consoleLines[i] );
		if ( !I_stricmp( Cmd_Argv( 0 ), "safe" ) || !I_stricmp( Cmd_Argv( 0 ), "dvar_restart" ) )
		{
			com_consoleLines[i][0] = 0;
			return qtrue;
		}
	}
	return com_safemode;
}

/*
=================
Com_ForceSafeMode
=================
*/
void Com_ForceSafeMode()
{
	com_safemode = qtrue;
}

/*
===============
Com_StartupVariable

Searches for command line parameters that are set commands.
If match is not NULL, only that cvar will be looked for.
That is necessary because cddir and basedir need to be set
before the filesystem is started, but all other sets shouls
be after execing the config and default.
===============
*/
void Com_StartupVariable( const char *match )
{
	int		i;
	for (i=0 ; i < com_numConsoleLines ; i++)
	{
		Cmd_TokenizeString( com_consoleLines[i] );

		if ( match && strcmp( Cmd_Argv(1), match ) )
		{
			continue;
		}

		if ( !strcasecmp( Cmd_Argv(0), "set" ) )
		{
			Dvar_Set_f();
			continue;
		}

		if ( !strcasecmp( Cmd_Argv(0), "seta" ) )
		{
			Dvar_SetA_f();
		}
	}
}

/*
=================
Com_AddStartupCommands

Adds command line parameters as script statements
Commands are seperated by + signs

Returns qtrue if any late commands were added, which
will keep the demoloop from immediately starting
=================
*/
qboolean Com_AddStartupCommands( void )
{
	int i;
	qboolean added;

	added = qfalse;
	// quote every token, so args with semicolons can work
	for ( i = 0 ; i < com_numConsoleLines ; i++ )
	{
		if ( !com_consoleLines[i] || !com_consoleLines[i][0] )
		{
			continue;
		}

		// set commands won't override menu startup
		if ( I_strnicmp( com_consoleLines[i], "set", 3 ) )
		{
			added = qtrue;
		}
		Cbuf_AddText( com_consoleLines[i] );
		Cbuf_AddText( "\n" );
	}

	return added;
}

/*
=============
Info_Print
=============
*/
void Info_Print( const char *s )
{
	char key[512];
	char value[512];
	char    *o;
	int l;

	if ( *s == '\\' )
	{
		s++;
	}
	while ( *s )
	{
		o = key;
		while ( *s && *s != '\\' )
			*o++ = *s++;

		l = o - key;
		if ( l < 20 )
		{
			memset( o, ' ', 20 - l );
			key[20] = 0;
		}
		else
		{
			*o = 0;
		}
		Com_Printf( "%s", key );

		if ( !*s )
		{
			Com_Printf( "MISSING VALUE\n" );
			return;
		}

		o = value;
		s++;
		while ( *s && *s != '\\' )
			*o++ = *s++;
		*o = 0;

		if ( *s )
		{
			s++;
		}
		Com_Printf( "%s\n", value );
	}
}

/*
=================
Com_InitPushEvent
=================
*/
// bk001129 - added
void Com_InitPushEvent( void )
{
	// clear the static buffer array
	// this requires SE_NONE to be accepted as a valid but NOP event
	memset( com_pushedEvents, 0, sizeof( com_pushedEvents ) );
	// reset counters while we are at it
	// beware: GetEvent might still return an SE_NONE from the buffer
	com_pushedEventsHead = 0;
	com_pushedEventsTail = 0;
}

/*
==============
Com_PushEvent
==============
*/
void Com_PushEvent( sysEvent_t *event )
{
	sysEvent_t      *ev;
	static int printedWarning = 0; // bk001129 - init, bk001204 - explicit int

	ev = &com_pushedEvents[ com_pushedEventsHead & ( MAX_PUSHED_EVENTS - 1 ) ];

	if ( com_pushedEventsHead - com_pushedEventsTail >= MAX_PUSHED_EVENTS )
	{

		// don't print the warning constantly, or it can give time for more...
		if ( !printedWarning )
		{
			printedWarning = qtrue;
			Com_Printf( "WARNING: Com_PushEvent overflow\n" );
		}

		if ( ev->evPtr )
		{
			Z_Free( ev->evPtr );
		}
		com_pushedEventsTail++;
	}
	else
	{
		printedWarning = qfalse;
	}

	*ev = *event;
	com_pushedEventsHead++;
}

void Com_ShutdownEvents( void )
{
	sysEvent_t ev;

	while ( com_pushedEventsHead > com_pushedEventsTail )
	{
		ev = com_pushedEvents[com_pushedEventsTail & ( MAX_PUSHED_EVENTS - 1 )];
		com_pushedEventsTail++;

		if ( ev.evPtr )
			Z_Free(ev.evPtr);
	}
}

/*
================
Com_GetEvent
================
*/
sysEvent_t Com_GetEvent( void )
{
	if ( com_pushedEventsHead > com_pushedEventsTail )
	{
		com_pushedEventsTail++;
		return com_pushedEvents[ ( com_pushedEventsTail - 1 ) & ( MAX_PUSHED_EVENTS - 1 ) ];
	}
	return Sys_GetEvent();
}

int Com_EventLoop( void )
{
	sysEvent_t	ev;
	msg_t		buf;
	netadr_t	evFrom;
	LargeLocal	bufLarge( MAX_MSGLEN );
	byte		*bufData = (byte *)bufLarge.GetBuf();

	MSG_Init( &buf, bufData, MAX_MSGLEN );

	while ( 1 )
	{
		ev = Com_GetEvent();

		switch ( ev.evType )
		{
		case SE_NONE:
			while ( NET_GetLoopPacket( NS_CLIENT, &evFrom, &buf ) )
			{
				CL_PacketEvent( evFrom, &buf, ev.evTime );
			}

			while ( NET_GetLoopPacket( NS_SERVER, &evFrom, &buf ) )
			{
				if ( com_sv_running->current.boolean )
				{
					SV_PacketEvent( evFrom, &buf );
				}
			}
			return ev.evTime;

		case SE_KEY:
			CL_KeyEvent( ev.evValue, ev.evValue2, ev.evTime );
			break;

		case SE_CHAR:
			CL_CharEvent( ev.evValue );
			break;

		case SE_MOUSE:
			break;

		case SE_CONSOLE:
			Cbuf_AddText( (char *)ev.evPtr );
			Z_Free( ev.evPtr );
			Cbuf_AddText( "\n" );
			break;

		case SE_PACKET:
			evFrom = *(netadr_t *)ev.evPtr;
			buf.cursize = ev.evPtrLength - sizeof( evFrom );

			// the size check is unsigned
			if ( (unsigned int)buf.cursize > (unsigned int)buf.maxsize )
			{
				Z_Free( ev.evPtr );
				Com_Printf( "Com_EventLoop: oversize packet\n" );
				break;
			}

			memcpy( buf.data, (byte *)((netadr_t *)ev.evPtr + 1), buf.cursize );
			Z_Free( ev.evPtr );

			if ( com_sv_running->current.boolean )
			{
				SV_PacketEvent( evFrom, &buf );
			}
			else
			{
				CL_PacketEvent( evFrom, &buf, ev.evTime );
			}
			break;

		default:
			Com_Error( ERR_FATAL, "\x15" "Com_EventLoop: bad event type %i", ev.evType );
			break;
		}
	}
}

/*
===================
Debug_EventLoop
===================
*/
int Debug_EventLoop( void )
{
	sysEvent_t	ev;
	int			processed;

	processed = 0;

	while ( 1 )
	{
		ev = Com_GetEvent();

		switch ( ev.evType )
		{
		case SE_NONE:
			return processed;

		case SE_KEY:
			CL_KeyEvent( ev.evValue, ev.evValue2, ev.evTime );
			break;

		case SE_CHAR:
			CL_CharEvent( ev.evValue );
			break;

		case SE_MOUSE:
			break;

		case SE_CONSOLE:
			Cbuf_AddText( (char *)ev.evPtr );
			Z_Free( ev.evPtr );
			Cbuf_AddText( "\n" );
			break;

		case SE_PACKET:
			Z_Free( ev.evPtr );
			break;

		default:
			Com_Error( ERR_FATAL, "\x15" "Com_EventLoop: bad event type %i", ev.evType );
			break;
		}

		processed = 1;
	}
}

/*
================
Com_Milliseconds
================
*/
int Com_Milliseconds( void )
{
	sysEvent_t	ev;

	// get events and push them until we get a null event with the current time
	do
	{
		ev = Sys_GetEvent();
		if ( ev.evType != SE_NONE )
		{
			Com_PushEvent( &ev );
		}
	}
	while ( ev.evType != SE_NONE );

	return ev.evTime;
}

/*
================
Com_PumpMessageLoop
================
*/
void Com_PumpMessageLoop()
{
	sysEvent_t ev;

	while ( 1 )
	{
		ev = Sys_GetEvent();
		if ( ev.evType == SE_NONE )
		{
			break;
		}
		Com_PushEvent(&ev);
	}
}

/*
=============
Com_Error_f

Just throw a fatal error to
test error shutdown procedures
=============
*/
static void Com_Error_f( void )
{
	if ( Cmd_Argc() > 1 )
	{
		Com_Error( ERR_DROP, "\x15" "Testing drop error" );
	}
	else
	{
		Com_Error( ERR_FATAL, "\x15" "Testing fatal error" );
	}
}

/*
=============
Com_Freeze_f

Just freeze in place for a given number of seconds to test
error recovery
=============
*/
static void Com_Freeze_f( void )
{
	float	s;
	int		start, now;

	if ( Cmd_Argc() != 2 )
	{
		Com_Printf( "freeze <seconds>\n" );
		return;
	}

	s = atof( Cmd_Argv(1) );

	start = Com_Milliseconds();

	while ( 1 )
	{
		now = Com_Milliseconds();
		if ( ( now - start ) * 0.001 > s )
		{
			break;
		}
	}
}

/*
=================
Com_Crash_f

A way to force a bus error for development reasons
=================
*/
static void Com_Crash_f( void )
{
	* ( int * ) 0 = 0x12345678;
}

// retail 0x4317f0; name unknown
static void Com_ClearCDKey( void )
{
	strcpy( cl_cdkey, "                " );
}

/*
=================
Com_ReadCDKey
=================
*/
void Com_ReadCDKey( void )
{
	HKEY hKey;
	DWORD size;
	DWORD type;
	char buffer[21];

	if ( RegOpenKeyA( HKEY_LOCAL_MACHINE, CDKEY_REGISTRY_PATH, &hKey ) != ERROR_SUCCESS )
	{
		strcpy( cl_cdkey, "                " );
		return;
	}

	type = REG_SZ;
	size = sizeof( buffer );
	if ( RegQueryValueExA( hKey, "codkey", NULL, &type, (LPBYTE)buffer, &size ) != ERROR_SUCCESS )
	{
		RegCloseKey( hKey );
		strcpy( cl_cdkey, "                " );
		return;
	}

	RegCloseKey( hKey );
	if ( size == sizeof( buffer ) )
	{
		memcpy( cl_cdkey, buffer, 16 );
		cl_cdkey[16] = 0;
		memcpy( cl_cdkeychecksum, buffer + 16, 4 );
		cl_cdkeychecksum[4] = 0;
		if ( CL_CDKeyValidate( cl_cdkey, cl_cdkeychecksum ) )
			return;
	}
	Com_ClearCDKey();
}

/*
=================
Com_WriteCDKey

Stores a valid key and its checksum in the registry; an invalid key is blanked.
=================
*/
void Com_WriteCDKey( void )
{
	HKEY hKey;
	char buffer[21];

	if ( !CL_CDKeyValidate( cl_cdkey, cl_cdkeychecksum ) )
	{
		strcpy( cl_cdkey, "                " );
		return;
	}

	if ( RegCreateKeyA( HKEY_LOCAL_MACHINE, CDKEY_REGISTRY_PATH, &hKey ) != ERROR_SUCCESS )
		return;

	memcpy( buffer, cl_cdkey, 16 );
	memcpy( buffer + 16, cl_cdkeychecksum, 4 );
	buffer[20] = 0;

	RegSetValueExA( hKey, "codkey", 0, REG_SZ, (const BYTE *)buffer, sizeof( buffer ) );
	RegCloseKey( hKey );
}

/*
==================
Com_SetRecommended
==================
*/
// retail 0x431a50, unreferenced; name unknown
int Com_ConfigureChecksum_431a50( const char *buffer, int size )
{
	int checksum;
	int i;

	checksum = 0;
	for ( i = 0; i < size; i++ )
		checksum = checksum * 0x7a69 + buffer[i];

	return ( checksum & 0xfffffff ) + 1;
}

static bool Com_HasConfigureChecksumChanged( void )
{
	void *csv;
	int filesize;
	int checksum;
	int i;

	filesize = FS_ReadFile( "configure_mp.csv", &csv );
	if ( filesize < 0 )
		Com_Error( ERR_FATAL, "EXE_ERR_NOT_FOUND" "\x15" "configure_mp.csv" );

	checksum = 0;
	for ( i = 0; i < filesize; i++ )
		checksum = checksum * 0x7a69 + ( (char *)csv )[i];

	FS_FreeFile( csv );
	checksum &= 0xfffffff;

	return Sys_HasConfigureChecksumChanged( checksum + 1 );
}

static int Com_GpuStringCompare( const char *wild, const char *s )
{
	char charWild;
	char charRef;
	int delta;

	do
	{
		charWild = *wild++;
		if ( charWild == '*' )
		{
			if ( !*wild )
				return 0;
			if ( *s && !Com_GpuStringCompare( wild - 1, s + 1 ) )
				return 0;
		}
		else if ( charWild == ' ' )
		{
			if ( *s && !isdigit( *s ) && !Com_GpuStringCompare( wild - 1, s + 1 ) )
				return 0;
		}
		else
		{
			charRef = *s++;
			if ( charWild != charRef && charWild != '?' )
			{
				delta = tolower( charWild ) - tolower( charRef );
				if ( delta )
					return delta >= 0 ? 1 : -1;
			}
		}
	} while ( charWild );
	return 0;
}
/*
==================
Com_MatchGpuTemplate
==================
*/
static bool Com_MatchGpuTemplate( const char *find, const char *gpuDescription )
{
	char wildcard[0x400];
	int len;
	const char *p;

	wildcard[0] = '*';
	len = 1;

	if ( *find )
	{
		for ( p = find; *p; p++ )
		{
			if ( isspace(*p) )
			{
				if ( wildcard[len - 1] == ' ' )
					continue;

				wildcard[len] = ' ';
			}
			else
			{
				wildcard[len] = *p;
			}

			len++;

			if ( len == 0x3ff )
				Com_Error( ERR_FATAL, "\x15" "configure_mp.csv: \"find\" string is too long\n" );
		}
	}

	if ( wildcard[len - 1] != '*' )
		wildcard[len++] = '*';

	wildcard[len] = 0;

	return Com_GpuStringCompare( wildcard, gpuDescription ) == 0;
}

/*
==================
Com_GetConfigureDvarNames
==================
*/
static int Com_GetConfigureDvarNames( const char **text, char *dvarNames )
{
	int dvarCount = 0;

	for ( ;; )
	{
		const char *token = Com_ParseOnLine( text );

		if ( !*text )
			Com_Error( ERR_FATAL, "\x15" "configure_mp.csv: unexpected end-of-file" );
		if ( !*token )
			return dvarCount;

		if ( strlen( token ) > 0x1f )
			Com_Error( ERR_FATAL, "\x15" "configure_mp.csv: dvar name \"%s\" longer than %i\n", token, 0x1f );
		if ( dvarCount >= 0x40 )
			Com_Error( ERR_FATAL, "\x15" "configure_mp.csv: more than %i dvars\n", 0x40 );

		I_strncpyz( dvarNames + dvarCount * 0x20, token, 0x20 );
		dvarCount++;
	}
}

/*
==================
Com_GetConfigureDvarValues
==================
*/
static void Com_GetConfigureDvarValues( int dvarCount, const char **text, char *dvarValues )
{
	int dvarIndex;

	for ( dvarIndex = 0; dvarIndex < dvarCount; dvarIndex++ )
	{
		const char *token = Com_ParseOnLine( text );

		if ( !*text )
			Com_Error( ERR_FATAL, "\x15" "configure_mp.csv: unexpected end-of-file" );
		if ( !*token )
			Com_Error( ERR_FATAL, "\x15" "configure_mp.csv: missing entry in dvar value column %i\n", dvarIndex );
		if ( strlen( token ) > 0x1f )
			Com_Error( ERR_FATAL, "\x15" "configure_mp.csv: entry '%s' in dvar value column %i is longer than %i\n", token, dvarIndex, 0x1f );

		if ( dvarValues )
			I_strncpyz( &dvarValues[0x20 * dvarIndex], token, 0x20 );
	}

	{
		const char *token = Com_ParseOnLine( text );
		if ( *token )
			Com_Error( ERR_FATAL, "\x15" "configure_mp.csv: extra dvar value column(s): value = %s\n", token );
	}
}

/*
==================
Com_SetConfigureDvars
==================
*/
static void Com_SetConfigureDvars( int dvarCount, const char *dvarNames, const char *dvarValues )
{
	int dvarIndex;

	for ( dvarIndex = 0; dvarIndex < dvarCount; dvarIndex++ )
	{
		const char *name = dvarNames + dvarIndex * 0x20;
		const char *value = dvarValues + dvarIndex * 0x20;

		Dvar_SetFromStringByNameFromSource( name, value, DVAR_SOURCE_EXTERNAL );

		Dvar_AddFlags( Dvar_FindVar( name ), DVAR_ARCHIVE );
	}
}

/*
==================
Com_ParseCpuConfig
==================
*/
static bool Com_ParseCpuConfig( const SysInfo *info, const char **text )
{
	char dvarNames[0x40 * 0x20];
	char dvarValues[0x40 * 0x20];
	SysInfo row;
	SysInfo best;
	int dvarCount;
	bool foundCpuMatch;
	bool isBest;
	const char *token;

	best.cpuGHz = -1.0;
	dvarCount = 0;
	best.sysMB = 0;
	foundCpuMatch = false;

	for ( ;; )
	{
		token = Com_Parse( text );
		if ( !text )
			break;

		if ( !*token || *token == '#' )
		{
			Com_SkipRestOfLine( text );
			continue;
		}

		if ( !stricmp( token, "gpu" ) )
		{
			Com_UngetToken();
			break;
		}

		if ( !dvarCount )
		{
			if ( stricmp( token, "cpu ghz" ) )
				Com_Error( ERR_FATAL, "\x15" "configure_mp.csv: \"cpu ghz\" should be the first column\n" );
			token = Com_ParseOnLine( text );
			if ( stricmp( token, "sys mb" ) )
				Com_Error( ERR_FATAL, "\x15" "configure_mp.csv: \"sys mb\" should be the second column\n" );
			dvarCount = Com_GetConfigureDvarNames( text, dvarNames );
			continue;
		}

		row.cpuGHz = atof( token );
		if ( row.cpuGHz < 0.0 )
			Com_Error( ERR_FATAL, "\x15" "configure_mp.csv: cpu ghz %g not allowed to be less than 0\n", row.cpuGHz );

		row.sysMB = atoi( Com_ParseOnLine( text ) );
		if ( row.sysMB < 0x80 )
			Com_Error( ERR_FATAL, "\x15" "configure_mp.csv: sys mb %i not allowed to be less than 128\n", row.sysMB );

		isBest = false;
		if ( info->cpuGHz >= row.cpuGHz && info->sysMB >= row.sysMB )
		{
			if ( best.cpuGHz < row.cpuGHz || ( best.cpuGHz == row.cpuGHz && best.sysMB < row.sysMB ) )
			{
				isBest = true;
				best = row;
				foundCpuMatch = true;
			}
		}
		Com_GetConfigureDvarValues( dvarCount, text, isBest ? dvarValues : NULL );
	}

	if ( !foundCpuMatch )
		return false;

	Com_Printf( "configure_mp.csv: using CPU configuration %.0f GHz %i MB\n", best.cpuGHz, best.sysMB );
	Cbuf_AddText( "exec configure_mp.cfg" );
	Cbuf_Execute();
	Com_SetConfigureDvars( dvarCount, dvarNames, dvarValues );

	return true;
}

/*
==================
Com_ParseGpuConfig
==================
*/
static bool Com_ParseGpuConfig( const char **text, const SysInfo *info )
{
	char dvarNames[0x40 * 0x20];
	char dvarValues[0x40 * 0x20];
	int dvarCount;
	bool foundGpuMatch;
	const char *token;

	token = Com_Parse( text );
	if ( stricmp( token, "gpu" ) )
	{
		Com_UngetToken();
		return false;
	}

	dvarCount = Com_GetConfigureDvarNames( text, dvarNames );
	foundGpuMatch = false;

	for ( ;; )
	{
		token = Com_Parse( text );
		if ( !*text )
			break;

		if ( !*token || *token == '#' )
		{
			Com_SkipRestOfLine( text );
			continue;
		}

		if ( !foundGpuMatch && Com_MatchGpuTemplate( token, info->gpuDescription ) )
		{
			Com_Printf( "configure_mp.csv: using GPU configuration \"%s\"\n", token );
			Com_GetConfigureDvarValues( dvarCount, text, dvarValues );
			Com_SetConfigureDvars( dvarCount, dvarNames, dvarValues );
			foundGpuMatch = true;
		}
		else
		{
			Com_GetConfigureDvarValues( dvarCount, text, NULL );
		}
	}

	return foundGpuMatch;
}

void Com_SetRecommended( qboolean restart )
{
	SysInfo info;
	void *csv;
	int filesize;
	const char *text;
	int checksum;
	int i;

	Com_Printf( "========= autoconfigure\n" );

	Sys_GetInfo( &info );
	// startup code can leave MMX state live, which corrupts the first x87 op
	info.cpuGHz *= 1.02;
	if ( info.sysMB >= 0x80 )
		info.sysMB += 8;
	else
		info.sysMB = 0x80;

	filesize = FS_ReadFile( "configure_mp.csv", &csv );
	if ( filesize < 0 )
		Com_Error( ERR_FATAL, "EXE_ERR_NOT_FOUND" "\x15" "configure_mp.csv" );

	text = (const char *)csv;
	Com_BeginParseSession( "configure_mp.csv" );
	Com_SetCSV( qtrue );

	if ( !Com_ParseCpuConfig( &info, &text ) )
	{
		Sys_GetInfo( &info );
		Com_Error( ERR_FATAL, "\x15" "configure_mp.csv: " "\x14" "EXE_ERR_COULDNT_CONFIGURE" "\x15" " %.0f GHz %i MB\n", info.cpuGHz, info.sysMB );
	}

	if ( !Com_ParseGpuConfig( &text, &info ) )
		Com_Error( ERR_FATAL, "\x15" "configure_mp.csv: " "\x14" "EXE_ERR_COULDNT_CONFIGURE" "\x15" " \"%s\"\n", info.gpuDescription );

	Com_EndParseSessionInl();

	checksum = 0;
	for ( i = 0; i < filesize; i++ )
		checksum = checksum * 0x7a69 + ( (char *)csv )[i];

	FS_FreeFile( csv );
	checksum &= 0xfffffff;
	Sys_ArchiveInfo( checksum + 1 );

	if ( restart && Dvar_AnyLatchedValues() )
		Cbuf_AddText( "snd_restart\n" );
}

/*
==================
Com_CheckSetRecommended
==================
*/
void Com_CheckSetRecommended( void )
{
	if ( !com_recommendedSet->current.boolean || Com_HasConfigureChecksumChanged() )
	{
		Com_SetRecommended( qfalse );
		Dvar_SetBool( com_recommendedSet, qtrue );
	}

	if ( Sys_HasInfoChanged() )
		Com_SetRecommended( qfalse );
}

void Com_SetScriptSettings( void )
{
	int developer;
	int developerLevel;

	developer = com_developer->current.integer || com_logfile->current.integer;
	developerLevel = com_developer->current.integer;
	Scr_Settings(developer, com_developer_script->current.boolean, developerLevel);
}

/*
==================
COM_PlayIntroMovies
==================
*/
void COM_PlayIntroMovies()
{
	if ( com_dedicated->current.integer )
		return;

	if ( com_introPlayed->current.boolean )
		return;

	Cbuf_AddText("cinematic atvi\n");
	Dvar_SetString(nextmap, "cinematic IW_logo; set nextmap cinematic cod_intro");
	Dvar_SetBool(com_introPlayed, true);
}

void Com_InitDvars()
{
	com_dedicated = Dvar_RegisterInt("dedicated", 0, 0, 2, DVAR_LATCH | DVAR_CHANGEABLE_RESET);

	if ( com_dedicated->current.integer )
	{
		Dvar_RegisterInt("dedicated", 0, 0, 2, DVAR_ROM | DVAR_CHANGEABLE_RESET);
	}

	com_maxfps = Dvar_RegisterInt("com_maxfps", 85, 0, 1000, DVAR_ARCHIVE | DVAR_CHANGEABLE_RESET);
	com_developer = Dvar_RegisterInt("developer", 0, 0, 2, DVAR_CHANGEABLE_RESET);
	com_developer_script = Dvar_RegisterBool("developer_script", false, DVAR_CHANGEABLE_RESET);
	com_logfile = Dvar_RegisterInt("logfile", 0, 0, 2, DVAR_CHANGEABLE_RESET);
	com_statmon = Dvar_RegisterBool("com_statmon", false, DVAR_CHANGEABLE_RESET);
	com_timescale = Dvar_RegisterFloat("timescale", 1.0, 0.001, 1000.0, DVAR_SYSTEMINFO | DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
	com_fixedtime = Dvar_RegisterInt("fixedtime", 0, 0, 1000, DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
	com_viewlog = Dvar_RegisterInt("viewlog", 0, 0, 2, DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
	sv_paused = Dvar_RegisterInt("sv_paused", 0, 0, 2, DVAR_ROM | DVAR_CHANGEABLE_RESET);
	cl_paused = Dvar_RegisterInt("cl_paused", 0, 0, 2, DVAR_ROM | DVAR_CHANGEABLE_RESET);
	com_sv_running = Dvar_RegisterBool("sv_running", false, DVAR_ROM | DVAR_CHANGEABLE_RESET);

	engineState->clientActive = 0;

	com_introPlayed = Dvar_RegisterBool("com_introPlayed", false, DVAR_ARCHIVE | DVAR_CHANGEABLE_RESET);
	com_animCheck = Dvar_RegisterBool("com_animCheck", false, DVAR_CHANGEABLE_RESET);

	if ( com_dedicated->current.integer )
	{
		if ( !com_viewlog->current.integer )
			Dvar_SetInt(com_viewlog, 1);
	}
}

/*
===================
Com_RunAutoExec
===================
*/
void Com_RunAutoExec()
{
	Dvar_SetInAutoExec(qtrue);
	Cbuf_Execute();
	Dvar_SetInAutoExec(qfalse);
}

/*
===================
Com_ExecStartupConfigs
===================
*/
void Com_ExecStartupConfigs( const char *configFile )
{
	Cbuf_AddText("exec default_mp.cfg\n");
	Cbuf_AddText("exec language.cfg\n");

	if ( configFile )
	{
		Cbuf_AddText(va("exec %s\n", configFile));
	}

	Cbuf_Execute();

	Com_RunAutoExec();

	if ( Com_SafeMode() )
	{
#ifdef DEDICATED
		Cbuf_AddText("exec safemode_mp_server.cfg\n");
#else
		Cbuf_AddText("exec safemode_mp.cfg\n");
#endif
	}

	Cbuf_Execute();
}

static void Com_Init_Try_Block_Function( char *commandLine )
{
	char *src;
	int i;

	Com_Printf("%s %s build %s %s\n", GAME_STRING,PRODUCT_VERSION,CPUSTRING, BUILD_DATE);
	Com_InitPushEvent();
	Com_ParseCommandLine(commandLine);
	Swap_Init();
	Cbuf_Init();
	Cmd_Init();
	Com_StartupVariable(0);
	Com_StartupVariable("developer");
	CL_InitKeyCommands();
	FS_InitFilesystem();
	Com_InitDvars();

	for ( i = 0; i <= 0; ++i )
	{
		CL_SwitchToLocalClient( i );
		Com_StartupConfigs();
	}

	CL_SwitchToLocalClient( 0 );
	Cbuf_Execute();
	com_recommendedSet = Dvar_RegisterBool("com_recommendedSet", false, DVAR_ARCHIVE | DVAR_CHANGEABLE_RESET);
	Com_CheckSetRecommended();
	_controlfp( _PC_24, _MCW_PC );
	_controlfp( _MCW_EM, _MCW_EM );
	Com_StartupVariable(0);
	SEH_UpdateLanguageInfo();

	if ( com_dedicated->current.integer )
	{
		Sys_HideSplashWindow();
		Sys_ShowConsole( 1, qtrue );
		Sys_NormalExit();
	}

	Com_InitHunkMemory();
	dvar_modifiedFlags &= ~DVAR_ARCHIVE;
	com_codeTimeScale = 1.0;
	if ( com_developer->current.integer )
	{
		Cmd_AddCommand("error", Com_Error_f);
		Cmd_AddCommand("crash", Com_Crash_f);
		Cmd_AddCommand("freeze", Com_Freeze_f);
	}
	Cmd_AddCommand("quit", Com_Quit_f);
	Cmd_AddCommand("writeconfig", Com_WriteConfig_f);
	Cmd_AddCommand("writedefaults", Com_WriteDefaults_f);
	src = va("%s %s build %s %s", GAME_STRING,PRODUCT_VERSION,getBuildNumber(),CPUSTRING);
	version = Dvar_RegisterString("version", "", DVAR_ROM | DVAR_CHANGEABLE_RESET);
	Dvar_SetString(version, src);
	shortversion = Dvar_RegisterString("shortversion", PRODUCT_VERSION, DVAR_SERVERINFO | DVAR_ROM | DVAR_CHANGEABLE_RESET);
	FxMem_Init();
	Sys_Init();
	Netchan_Init(Com_Milliseconds() & 0xffff);
	Scr_Init();
	Com_SetScriptSettings();
	XAnimInit();
	DObjInit();
	SV_Init();
	NET_Init();
	Dvar_ClearModified( com_dedicated );

	if ( !com_dedicated->current.integer )
	{
		CL_InitOnceForAllClients();
		CL_Init();
		Sys_ShowConsole( com_viewlog->current.integer, qfalse );
	}

	com_frameTime = Com_Milliseconds();
	Com_AddStartupCommands();

	if ( !com_dedicated->current.integer )
	{
		cls.rendererStarted = qtrue;
		CL_InitRenderer();
		cls.soundStarted = qtrue;
		SND_Init();
	}

	Sys_LoadingKeepAlive();

	if ( !com_dedicated->current.integer )
		Sys_ShowConsole( com_viewlog->current.integer, qfalse );

	COM_PlayIntroMovies();
	com_fullyInitialized = 1;
	Com_Printf("--- Common Initialization Complete ---\n");
	Cbuf_Execute();

	if ( !com_sv_running->current.boolean )
	{
		UI_SetMap("", "");
		CL_StartHunkUsers();
	}
}

/*
=================
Com_Init
=================
*/
void Com_Init( char* commandLine )
{
	if ( !__sigsetjmp(*(jmp_buf*)Sys_GetValue(THREAD_VALUE_COM_ERROR), 0) )
	{
		Com_Init_Try_Block_Function(commandLine);
	}
	else
	{
		Sys_Error(va("Error during initialization:\n%s\n", com_errorMessage));
	}
}

void Com_WriteConfigToFile( const char *filename )
{
	fileHandle_t f = FS_FOpenFileWrite(filename);

	if ( !f )
	{
		Com_Printf("Couldn't write %s.\n", filename);
		return;
	}

	FS_Printf(f, "// generated by Call of Duty, do not modify\n");
	FS_Printf(f, "unbindall\n");
	Key_WriteBindings(f);
	Dvar_WriteVariables(f);
	FS_FCloseFile(f);
}

void Com_WriteDefaultsToFile( const char *filename )
{
	fileHandle_t f;

	f = FS_FOpenFileWrite(filename);

	if ( !f )
	{
		Com_Printf("Couldn't write %s.\n", filename);
		return;
	}

	FS_Printf(f, "// generated by Call of Duty, do not modify\n");
	Dvar_WriteDefaults(f);
	FS_FCloseFile(f);
}

/*
===============
Com_WriteConfiguration

Writes key bindings and archived cvars to config file if modified
===============
*/
void Com_WriteConfiguration( void )
{
	char filename[MAX_QPATH];

	// if we are quiting without fully initializing, make sure
	// we don't write out anything
	if ( !com_fullyInitialized )
		return;

	if ( !(dvar_modifiedFlags & DVAR_ARCHIVE ) )
		return;

	dvar_modifiedFlags &= ~DVAR_ARCHIVE;

	if ( !com_playerProfile->current.string[0] )
		return;

	Com_BuildPlayerProfilePath(filename, sizeof(filename), "config_mp.cfg");
	Com_WriteConfigToFile( filename );
}

/*
=============
Com_WriteConfig_f
=============
*/
static void Com_WriteConfig_f( void )
{
	char filename[MAX_QPATH];

	if ( Cmd_Argc() != 2 )
	{
		Com_Printf("Usage: writeconfig <filename>\n");
		return;
	}

	I_strncpyz(filename, Cmd_Argv(1), sizeof(filename));

	Com_DefaultExtension(filename, sizeof(filename), ".cfg");
	Com_Printf("Writing %s.\n", filename);
	Com_WriteConfigToFile(filename);
}

/*
=============
Com_WriteDefaults_f
=============
*/
static void Com_WriteDefaults_f( void )
{
	char filename[MAX_QPATH];

	if ( Cmd_Argc() != 2 )
	{
		Com_Printf("Usage: writedefaults <filename>\n");
		return;
	}

	I_strncpyz(filename, Cmd_Argv(1), sizeof(filename));

	Com_DefaultExtension(filename, sizeof(filename), ".cfg");
	Com_Printf("Writing %s.\n", filename);
	Com_WriteDefaultsToFile(filename);
}

/*
==============
Com_GetTimescaleForSnd
==============
*/
float Com_GetTimescaleForSnd()
{
	if ( com_fixedtime->current.integer )
		return (float)com_fixedtime->current.integer;

	return com_timescale->current.decimal;
}

/*
================
Com_ModifyMsec
================
*/
int Com_ModifyMsec( int msec )
{
	int clampTime;
	bool useTimescale;
	int originalMsec;

	originalMsec = msec;

	//
	// modify time for debugging values
	//
	if ( com_fixedtime->current.integer )
	{
		msec = com_fixedtime->current.integer;
		useTimescale = true;
	}
	else if ( com_timescale->current.decimal != 1.0f || com_codeTimeScale != 1.0f )
	{
		msec = I_fround( msec * com_timescale->current.decimal * com_codeTimeScale );
		useTimescale = true;
	}
	else
	{
		useTimescale = false;
	}

	// don't let it scale below 1 msec
	if ( msec < 1 )
		msec = 1;

	if ( com_dedicated->current.integer )
	{
		// dedicated servers don't want to clamp for a much longer
		// period, because it would mess up all the client's views
		// of time.
		if ( msec > 500 && msec < 500000 )
		{
			Com_Printf("Hitch warning: %i msec frame time\n", msec);
		}
		clampTime = 5000;
	}
	else if ( !com_sv_running->current.boolean )
	{
		// for local single player gaming
		// we may want to clamp the time to prevent players from
		// flying off edges when something hitches.
		clampTime = 5000;
	}
	else
	{
		// clients of remote servers do not want to clamp time, because
		// it would skew their view of the server's time temporarily
		clampTime = 200;
	}

	if ( msec > clampTime )
		msec = clampTime;

	com_timescaleValue = useTimescale && originalMsec ? (float)msec / (float)originalMsec : 1.0f;

	return msec;
}

/*
==================
Com_DedicatedModified
==================
*/
void Com_DedicatedModified()
{
	if ( com_dedicated->flags & DVAR_ROM )
		return;

	if ( com_dedicated->latched.integer == com_dedicated->current.integer )
		return;

	com_dedicated = Dvar_RegisterInt("dedicated", 0, 0, 2, DVAR_LATCH | DVAR_CHANGEABLE_RESET);

	if ( com_dedicated->current.integer )
		Dvar_RegisterInt("dedicated", 0, 0, 2, DVAR_ROM | DVAR_CHANGEABLE_RESET);

	Dvar_ClearModified( com_dedicated );
	CL_Shutdown();
	Sys_ShowConsole( 1, qtrue );
	Sys_NormalExit();
	SV_AddDedicatedCommands();
}

static void Com_UpdateStatMonWarnings()
{
	static int lastFrameTime;
	int prevTime;
	int frameTime;

	if ( !com_statmon->current.boolean )
		return;

	if ( com_fileAccessed )
	{
		StatMon_Warning(1, 3000, "gfx/2d/warning@file.jpg");
		com_fileAccessed = 0;
	}

	prevTime = lastFrameTime;
	frameTime = Sys_Milliseconds();
	lastFrameTime = frameTime;

	if ( com_statmon->current.boolean && frameTime - prevTime > 33 && prevTime )
		StatMon_Warning(0, 3000, "gfx/2d/warning@fps.jpg");
}

/*
==================
Com_ViewLogModified
==================
*/
static void Com_ViewLogModified()
{
	if ( !com_viewlog->modified )
		return;

	if ( !com_dedicated->current.integer )
		Sys_ShowConsole( com_viewlog->current.integer, qfalse );

	Dvar_ClearModified( com_viewlog );
}

/*
=================
Com_Frame_Try_Block_Function
=================
*/
static void Com_Frame_Try_Block_Function()
{
	// the frame reserves a third int slot here that the code never reads
	int msec, minMsec, unusedMsec;

	Com_WriteConfiguration();
	Com_ViewLogModified();
	SetAnimCheck(com_animCheck->current.boolean);

	// we may want to spin here if things are going too fast
	minMsec = 1;
	if ( com_maxfps->current.integer > 0 && !com_dedicated->current.integer )
	{
		minMsec = 1000 / com_maxfps->current.integer;
		if ( !minMsec )
			minMsec = 1;
	}

retry:
	com_frameTime = Com_EventLoop();

	if ( com_lastFrameTime > com_frameTime )
		com_lastFrameTime = com_frameTime;

	msec = com_frameTime - com_lastFrameTime;

	if ( msec >= minMsec )
		goto done;

	NET_Sleep(0);
	goto retry;

done:
	Cbuf_Execute();
	com_lastFrameTime = com_frameTime;
	msec = Com_ModifyMsec( msec );
	CL_SwitchToLocalClient( 0 );
	SV_Frame( msec );
	Com_DedicatedModified();

	if ( com_dedicated->current.integer )
		return;

	CL_RunOncePerClientFrame( msec );
	Com_EventLoop();
	Cbuf_Execute();
	SND_UpdateLoopingSounds();
	SND_Update();
	CL_Frame();
	SCR_UpdateScreen();
	SCR_RunCinematic();
	Com_UpdateStatMonWarnings();
}

// empty in the dedicated build
// retail 0x433820
// retail 0x433800, unreferenced (inlined at its callers); name unknown
void Com_StartHunkUsersNoMap_433800( void )
{
	UI_SetMap("", "");
	CL_StartHunkUsers();
}

void Com_StartHunkUsers( void )
{
	if ( !__sigsetjmp(*(jmp_buf*)Sys_GetValue(THREAD_VALUE_COM_ERROR), 0) )
	{
		UI_SetMap("", "");
		CL_StartHunkUsers();
		IN_Frame();
		Com_EventLoop();

		if ( com_dedicated && !com_dedicated->current.integer )
			UI_SetActiveMenu(1);
	}
	else
	{
		Sys_Error( "Error during initialization" );
	}
}

/*
================
Com_ResetFrametime
================
*/
void Com_ResetFrametime()
{
	com_lastFrameTime = Sys_MilliSeconds();
}

void Com_Frame( void )
{
	_controlfp( _PC_24, _MCW_PC );
	_controlfp( _MCW_EM, _MCW_EM );

	if ( !__sigsetjmp(*(jmp_buf*)Sys_GetValue(THREAD_VALUE_COM_ERROR), 0) )
	{
		Com_Frame_Try_Block_Function();
		com_frameNumber++;
	}

	if ( com_errorEntered )
	{
		Com_ErrorCleanup();
		Com_StartHunkUsers();
	}
}

/*
===============
Com_Close
===============
*/
void Com_Close()
{
	Com_ShutdownDObj();
	DObjShutdown();
	XAnimShutdown();
	CM_Shutdown();
	SND_ShutdownChannels();
	Hunk_Clear();
	Scr_Shutdown();
}

void Field_Clear( field_t *edit )
{
	memset(edit->buffer, 0, sizeof(edit->buffer));
	edit->cursor = 0;
	edit->scroll = 0;
	edit->drawWidth = 256;
}

/*
===============
Com_Restart
===============
*/
void Com_Restart()
{
	CL_ShutdownHunkUsers();
	SV_ShutdownGameProgs();
	CIN_CloseAllVideos();
	Com_ShutdownDObj();
	DObjShutdown();
	XAnimShutdown();
	CM_Shutdown();
	// an empty stub in the dedicated build
	SND_ShutdownChannels();
	Hunk_Clear();
	Scr_Init();
	Com_SetScriptSettings();
	com_fixedConsolePosition = 0;
	XAnimInit();
	DObjInit();
	Com_InitDObj();
}

void *MT_AllocAnimTree( int numBytes )
{
	return MT_Alloc(numBytes, 5);
}

XAnimTree_s *Com_XAnimCreateSmallTree( XAnim_s *anims )
{
	return XAnimCreateTree(anims, MT_AllocAnimTree);
}

void Com_XAnimFreeSmallTree( XAnimTree_s *animtree )
{
	XAnimFreeTree(animtree, MT_Free);
}

/*
==============
Com_SetWeaponInfoMemory
==============
*/
void Com_SetWeaponInfoMemory( int iSource )
{
	iWeaponInfoSource = iSource;
}

void Com_FreeWeaponInfoMemory( int iSource )
{
	if ( iSource != iWeaponInfoSource )
		return;

	iWeaponInfoSource = 0;
	BG_ShutdownWeaponDefFiles();
}

int Com_AddToString( const char *add, char *msg, int len, int maxlen, int mayAddQuotes )
{
	int addQuotes;
	int i;

	addQuotes = 0;

	if ( mayAddQuotes )
	{
		if ( !*add )
		{
			addQuotes = 1;
		}
		else
		{
			for ( i = 0; i < maxlen - len && add[i]; ++i )
			{
				if ( add[i] <= ' ' )
				{
					addQuotes = 1;
					break;
				}
			}
		}
	}

	if ( addQuotes && len < maxlen )
		msg[len++] = '"';

	for ( i = 0; len < maxlen && add[i]; ++i )
		msg[len++] = add[i];

	if ( addQuotes && len < maxlen )
		msg[len++] = '"';

	return len;
}

char Com_GetDecimalDelimiter( void )
{
	int lang;

	lang = loc_language->current.integer;
	if ( lang == 1 || lang == 2 || lang == 3 || lang == 4 || lang == 6 || lang == 7 )
		return ',';
	else
		return '.';
}

/*
================
Com_LocalizedFloatToString
================
*/
void Com_LocalizedFloatToString( float f, char *buffer, unsigned int maxlen, unsigned int numDecimalPlaces )
{
	unsigned int charPos;
	char delimiter;

	snprintf(buffer, maxlen - 1, "%.*f", numDecimalPlaces, f);
	buffer[maxlen - 1] = 0;

	delimiter = Com_GetDecimalDelimiter();

	if ( delimiter != '.' )
	{
		for ( charPos = 0; charPos < maxlen; charPos++ )
		{
			if ( buffer[charPos] == '.' )
			{
				buffer[charPos] = delimiter;
				break;
			}
		}
	}
}

void Com_StartupConfigs( void )
{
	Com_InitPlayerProfiles();
}
// empty in the dedicated build
void Com_CheckSyncFrame( void )
{
}
void Com_CloseLogfiles()
{
	if ( logfile )
	{
		FS_FCloseFile(logfile);
		logfile = 0;
	}
}
static void Com_SkipConfigureBlankLines( const char **text )
{
	while ( *text && **text && ( **text == '\r' || **text == '\n' ) )
		Com_SkipRestOfLine( text );
}

