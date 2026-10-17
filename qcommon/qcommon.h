#pragma once

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <ctype.h>
#include <limits.h>
#include <setjmp.h>

// built against glibc 2.1, whose ctype predicates are macros over the
// exported __ctype_b table. Modern headers route them through __ctype_b_loc(),
// which emits a call instead of the table lookup the target contains.
#ifndef _WIN32
extern const unsigned short int *__ctype_b;
#undef isalnum
#undef isdigit
#undef isalpha
#undef isspace
#undef isupper
#undef islower
#undef isprint
#define isalnum( c )    ( __ctype_b[(int)( c )] & (unsigned short int)_ISalnum )
#define isdigit( c )    ( __ctype_b[(int)( c )] & (unsigned short int)_ISdigit )
#define isalpha( c )    ( __ctype_b[(int)( c )] & (unsigned short int)_ISalpha )
#define isspace( c )    ( __ctype_b[(int)( c )] & (unsigned short int)_ISspace )
#define isupper( c )    ( __ctype_b[(int)( c )] & (unsigned short int)_ISupper )
#define islower( c )    ( __ctype_b[(int)( c )] & (unsigned short int)_ISlower )
#define isprint( c )    ( __ctype_b[(int)( c )] & (unsigned short int)_ISprint )
#endif

enum print_msg_type_t
{
	PMSG_CONSOLE = 0x0,
	PMSG_GAME = 0x1,
	PMSG_BOLDGAME = 0x2,
	PMSG_SUBTITLE = 0x3,
	PMSG_LOGFILE = 0x4,
};
void Com_PrintMessage( print_msg_type_t channel, const char *msg );

#include "../universal/universal_public.h"


#ifndef MAXPRINTMSG
#define MAXPRINTMSG 4096
#endif

extern dvar_t *com_sv_running;
extern dvar_t *com_logfile;
extern dvar_t *com_dedicated;
extern dvar_t *com_viewlog;
extern dvar_t *com_developer;
extern dvar_t *cl_paused;

// returnbed by Sys_GetProcessorId
#define CPUID_GENERIC			0				// any unrecognized processor
#define CPUID_AXP				0x10
#define CPUID_INTEL_UNSUPPORTED	0x20			// Intel 386/486
#define CPUID_INTEL_PENTIUM		0x21			// Intel Pentium or PPro
#define CPUID_INTEL_MMX			0x22			// Intel Pentium/MMX or P2/MMX
#define CPUID_INTEL_KATMAI		0x23			// Intel Katmai
#define CPUID_AMD_3DNOW			0x30			// AMD K6 3DNOW!

int Sys_GetProcessorId( void );

typedef enum
{
	// bk001129 - make sure SE_NONE is zero
	SE_NONE = 0,	// evTime is still valid
	SE_KEY,		// evValue is a key code, evValue2 is the down flag
	SE_CHAR,	// evValue is an ascii char
	SE_MOUSE,	// evValue and evValue2 are reletive signed x / y moves
	SE_CONSOLE,	// evPtr is a char*
	SE_PACKET	// evPtr is a netadr_t followed by data bytes to evPtrLength
} sysEventType_t;

typedef struct
{
	int evTime;
	sysEventType_t evType;
	int evValue, evValue2;
	int evPtrLength;                // bytes of data pointed to by evPtr, for journaling
	void            *evPtr;         // this must be manually freed if not NULL
} sysEvent_t;

void Sys_SnapVector(vec3_t v);


int Com_AddToString(const char *add, char *msg, int len, int maxlen, int mayAddQuotes);
int Com_ModifyMsec( int msec );
void CL_SwitchToLocalClient( int localClientNum );
void Com_ErrorCleanup( void );
void Com_SetErrorMessage( const char *errorMessage );
void Com_SetCinematic();
void LargeLocalReset();
void Com_ClearTempMemory();
void Com_ExecStartupConfigs( const char *configFile );
void COM_PlayIntroMovies();
void Com_StartupConfigs();
void Com_InitPlayerProfiles();
int Com_HasPlayerProfile();
int Com_BuildPlayerProfilePath(char *path, int pathSize, const char *format, ...);
int Com_BuildPlayerProfilePathForPlayer(char *path, int pathSize, const char *playerName, const char *format, ...);
void Com_DedicatedModified();

enum errorParm_t
{
	ERR_FATAL = 0x0,
	ERR_DROP = 0x1,
	ERR_SERVERDISCONNECT = 0x2,
	ERR_DISCONNECT = 0x3,
	ERR_SCRIPT = 0x4,
	ERR_SCRIPT_DROP = 0x5,
	ERR_LOCALIZATION = 0x6,
	ERR_MAPLOADERRORSUMMARY = 0x7,
};

typedef enum
{
	NA_BOT = 0,
	NA_BAD = 1,
	NA_LOOPBACK = 2,
	NA_BROADCAST = 3,
	NA_IP = 4,
	NA_IPX = 5,
	NA_BROADCAST_IPX = 6
} netadrtype_t;

typedef enum
{
	NS_CLIENT,
	NS_SERVER
} netsrc_t;

typedef struct
{
	int type;
	byte ip[4];
	unsigned short port;
	byte ipx[10];
} netadr_t;

void NET_Init( void );
void NET_Shutdown( void );
void NET_Restart( void );
void NET_Config( qboolean enableNetworking );
void NET_Sleep(int msec);


// Edit fields and command line history/completion
#define	MAX_EDIT_LINE	256
typedef struct
{
	int		cursor;
	int		scroll;
	int		drawWidth;
	int		widthInChars;
	float	charHeight;
	int		fixedSize;
	char	buffer[MAX_EDIT_LINE];
} field_t;

void Field_Clear( field_t *edit );
void Field_CompleteCommand( field_t *field );

typedef struct
{
	qboolean overflowed;
	byte *data;
	int maxsize;
	int cursize;
	int readcount;
	int bit;
} msg_t;

enum svc_ops_e
{
	svc_nop,
	svc_gamestate,
	svc_configstring,
	svc_baseline,
	svc_serverCommand,
	svc_download,
	svc_snapshot,
	svc_EOF
};

enum clc_ops_e
{
	clc_move,
	clc_moveNoDelta,
	clc_clientCommand,
	clc_EOF
};

typedef enum
{
	MSG_DEFAULT,
	MSG_NA,
	MSG_WARNING,
	MSG_ERROR,
	MSG_NORDPRINT
} msgtype_t;

enum DeltaFlags
{
	DELTA_FLAGS_NONE,
	DELTA_FLAGS_FORCE
};

typedef void (*xcommand_t)(void);

#define PORT_SERVER 28960
#define	PORT_ANY -1

// NERVE - SMF - wolf multiplayer master servers
#define MASTER_SERVER_NAME "cod2master.activision.com"
#define HEARTBEAT_GAME "COD-2"
#define HEARTBEAT_DEAD "flatline"
#define PORT_MASTER 20710

#define AUTHORIZE_SERVER_NAME "cod2master.activision.com"
#define PORT_AUTHORIZE 20700

#define	HEARTBEAT_MSEC	180000
#define	STATUS_MSEC		600000

#define	MAX_RELIABLE_COMMANDS 128 // max string commands buffered for restransmit

// max length of a message, which may
// be fragmented into multiple packets
#if PROTOCOL_VERSION == 118
#define MAX_MSGLEN					0x20000
#else
#define MAX_MSGLEN					0x4000
#endif

#define MAX_SNAPSHOT_MSG_LEN		0x20000
#define MAX_VOICE_MSG_LEN			0x20000

#define MAX_DOWNLOAD_WINDOW         8       // max of eight download frames
#ifdef LIBCOD
#define MAX_DOWNLOAD_BLKSIZE        1024    // 1024 byte block chunks
#else
#define MAX_DOWNLOAD_BLKSIZE        2048    // 2048 byte block chunks
#endif

#define PACKET_BACKUP   32  // number of old messages that must be kept on client and
// server for delta comrpession and ping estimation
#define PACKET_MASK     ( PACKET_BACKUP - 1 )

#define MAX_PACKET_USERCMDS     32      // max number of usercmd_t in a packet

/* This is based on the Adaptive Huffman algorithm described in Sayood's Data
 * Compression book.  The ranks are not actually stored, but implicitly defined
 * by the location of a node within a doubly-linked list */
#define NYT HMAX                    /* NYT = Not Yet Transmitted */
#define INTERNAL_NODE ( HMAX + 1 )

typedef struct nodetype
{
	struct  nodetype *left, *right, *parent; /* tree structure */
	struct  nodetype *next, *prev; /* doubly-linked list */
	struct  nodetype **head; /* highest ranked node in block */
	int weight;
	int symbol;
} node_t;

#define HMAX 256 /* Maximum symbol */

typedef struct
{
	int blocNode;
	int blocPtrs;

	node_t*     tree;
	node_t*     lhead;
	node_t*     ltail;
	node_t*     loc[HMAX + 1];
	node_t**    freelist;

	node_t nodeList[768];
	node_t*     nodePtrs[768];
} huff_t;

#define	SV_ENCODE_START		4
#define	SV_DECODE_START		12
#define	CL_ENCODE_START		12
#define	CL_DECODE_START		4

typedef struct
{
	huff_t compressor;
	huff_t decompressor;
} huffman_t;

typedef struct NetField
{
	const char *name;
	int offset;
	int bits;
} netField_t;

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

#ifdef _WIN32
typedef DWORD threadid_t;
typedef HANDLE mutex_t;
#else
typedef pthread_t threadid_t;
typedef pthread_mutex_t mutex_t;
#endif

#define NUMTHREADS 1   /* 1.0 server keeps one copy of each per-thread block */
#define MAX_VASTRINGS 2

#define THREAD_CONTEXT_MAIN 0
#define THREAD_CONTEXT_DATABASE 1

struct va_info_t
{
	char va_string[MAX_VASTRINGS][MAX_STRING_CHARS];
	int index;
};

enum ThreadValue
{
	THREAD_VALUE_PROF_STACK,
	THREAD_VALUE_VA,
	THREAD_VALUE_COM_ERROR,
	THREAD_VALUE_TRACE,
	THREAD_VALUE_COUNT
};

enum CriticalSection
{
	CRITSECT_EXEC,
	CRITSECT_MYSQL,
	CRITSECT_SQLITE,
	CRITSECT_COUNT
};

#include "../game/g_shared.h"
#include "../server/server.h"
#include "../qcommon/cm_local.h"

extern va_info_t va_info[NUMTHREADS];
extern jmp_buf g_com_error[NUMTHREADS];
extern TraceThreadInfo g_traceThreadInfo[NUMTHREADS];

void Com_InitThreadData(int threadContext);
void* Sys_GetValue(int valueIndex);
void Sys_SetValue(int valueIndex, void* data);
unsigned char Sys_IsMainThread(void);
void Sys_InitMainThread();

void Sys_EnterCriticalSection(int section);
int Sys_TryEnterCriticalSection(int section);
void Sys_LeaveCriticalSection(int section);

qboolean Sys_CreateNewThread(void* (*ThreadMain)(void*), threadid_t *tid, void* arg);
void Sys_ExitThread(int code);
void Sys_SleepMSec(int msec);

bool Sys_SendPacket( int length, const void *data, netadr_t to );
qboolean Sys_GetPacket ( netadr_t *net_from, msg_t *net_message );
qboolean Sys_StringToAdr( const char *s, netadr_t *a );
qboolean Sys_IsLANAddress (netadr_t adr);

void Huff_Compress( msg_t *buf, int offset );
void Huff_Decompress( msg_t *buf, int offset );
void Huff_Init( huffman_t *huff );
void MSG_initHuffman();
void Huff_addRef( huff_t* huff, byte ch );
int  Huff_Receive( node_t *node, int *ch, byte *fin );
void Huff_transmit( huff_t *huff, int ch, byte *fout );
void Huff_offsetReceive( node_t *node, int *ch, byte *fin, int *offset );
void Huff_offsetTransmit( huff_t *huff, int ch, byte *fout, int *offset );
void Huff_putBit( int bit, byte *fout, int *offset );
int  Huff_getBit( byte *fout, int *offset );

// Engine state reached through a pointer; original name unknown. Only these members are referenced; the gap and the
// message extent are not.
struct engineState_t
{
	int unknown0;
	int clientActive;
	char unknown8[0xd5];
	bool serverKilled;
	char killMessage[1];
};
extern engineState_t *engineState;

// 1.0 msg_mp.cpp interface
void MSG_Init( msg_t *buf, byte *data, int length );
void MSG_BeginReading( msg_t *msg );
int MSG_GetUsedBitCount( const msg_t *msg );
void MSG_WriteBits( msg_t *msg, int value, int bits );
void MSG_WriteBit0( msg_t *msg );
void MSG_WriteBit1( msg_t *msg );
int MSG_ReadBits( msg_t *msg, int bits );
int MSG_ReadBit( msg_t *msg );
int MSG_WriteBitsCompress( byte *from, byte *to, int fromSizeBytes );
int MSG_ReadBitsCompress( byte *from, byte *to, int toSizeBytes );
void MSG_WriteByte( msg_t *msg, int c );
void MSG_WriteData( msg_t *msg, const void *data, int length );
void MSG_WriteShort( msg_t *msg, int c );
void MSG_WriteLong( msg_t *msg, int c );
void MSG_WriteInt64( msg_t *msg, int64_t c );
void MSG_WriteString( msg_t *sb, const char *s );
void MSG_WriteBigString( msg_t *sb, const char *s );
void MSG_WriteAngle( msg_t *sb, float f );
void MSG_WriteAngle16( msg_t *sb, float f );
int MSG_ReadByte( msg_t *msg );
int MSG_ReadShort( msg_t *msg );
int MSG_ReadLong( msg_t *msg );
int64_t MSG_ReadInt64( msg_t *msg );
char *MSG_ReadCommandString( msg_t *msg );
char *MSG_ReadBigString( msg_t *msg );
char *MSG_ReadStringLine( msg_t *msg );
float MSG_ReadAngle16( msg_t *msg );
void MSG_ReadData( msg_t *msg, void *data, int len );
void MSG_WriteDeltaKey( msg_t *msg, int key, int oldV, int newV, int bits );
int MSG_ReadDeltaKey( msg_t *msg, int key, int oldV, int bits );
void MSG_WriteKey( msg_t *msg, int key, int newV, int bits );
int MSG_ReadKey( msg_t *msg, int key, int bits );
void MSG_WriteDeltaKeyByte( msg_t *msg, int key, int oldV, int newV );
int MSG_ReadDeltaKeyByte( msg_t *msg, int key, int oldV );
void MSG_WriteDeltaKeyShort( msg_t *msg, int key, int oldV, int newV );
int MSG_ReadDeltaKeyShort( msg_t *msg, int key, int oldV );
void MSG_WriteReliableCommandToBuffer( const char *pszCommand, char *pszBuffer, int iBufferSize );
void MSG_SetDefaultUserCmd( playerState_t *ps, usercmd_t *cmd );
int MSG_GetMoveKeyMask( int forwardmove, int rightmove );
void MSG_UnmaskMoveKey( int keys, char *forwardmove, char *rightmove );
void MSG_WriteDeltaUsercmdKey( msg_t *msg, int key, usercmd_t *from, usercmd_t *to );
void MSG_ReadDeltaUsercmdKey( msg_t *msg, int key, usercmd_t *from, usercmd_t *to );
void MSG_WriteDeltaField( msg_t *msg, byte *from, byte *to, const netField_t *field );
void MSG_WriteDeltaObjective( msg_t *msg, byte *from, byte *to, qboolean force, int numFields, const netField_t *fields );
void MSG_WriteDeltaStruct( msg_t *msg, byte *from, byte *to, qboolean force, int numFields, int indexBits, const netField_t *stateFields, qboolean bChangeBit );
void MSG_WriteDeltaEntity( msg_t *msg, entityState_t *from, entityState_t *to, qboolean force );
void MSG_WriteDeltaArchivedEntity( msg_t *msg, archivedEntity_t *from, archivedEntity_t *to, qboolean force );
void MSG_WriteDeltaClient( msg_t *msg, clientState_t *from, clientState_t *to, qboolean force );
void MSG_ReadDeltaField( msg_t *msg, byte *from, byte *to, const netField_t *field, qboolean print );
void MSG_ReadDeltaObjective( msg_t *msg, byte *from, byte *to, int numFields, const netField_t *fields );
qboolean MSG_ReadDeltaStruct( msg_t *msg, byte *from, byte *to, int number, int numFields, int indexBits, const netField_t *stateFields );
qboolean MSG_ReadDeltaEntity( msg_t *msg, entityState_t *from, entityState_t *to, int number );
void MSG_ReadDeltaArchivedEntity( msg_t *msg, archivedEntity_t *from, archivedEntity_t *to, int number );
qboolean MSG_ReadDeltaClient( msg_t *msg, clientState_t *from, clientState_t *to, int number );
void MSG_WriteDeltaHudElems( msg_t *msg, hudelem_t *from, hudelem_t *to, int count );
void MSG_ReadDeltaHudElems( msg_t *msg, hudelem_t *from, hudelem_t *to, int count );
void MSG_WriteDeltaPlayerstate( msg_t *msg, playerState_t *from, playerState_t *to );
void MSG_ReadDeltaPlayerstate( msg_t *msg, playerState_t *from, playerState_t *to );

unsigned int Com_BlockChecksum( void *buffer, int length );
unsigned int Com_BlockChecksumKey( void *buffer, int length, int key );

int Com_HashKey( const char *string, int maxlen );

void Com_InitDvars();
void Com_StartupVariable( const char *match );
void Com_PrintMessage( print_msg_type_t channel, const char *msg );
void CL_ConsolePrint( print_msg_type_t type, const char *txt, int duration, int linewidth );
void CL_RunOncePerClientFrame( int msec );
void CL_Shutdown( void );
qboolean UI_SetActiveMenu( int menu );
void Sys_NormalExit( void );
void SV_AddDedicatedCommands( void );
void CL_Frame( void );
void SCR_UpdateScreen( void );
void SCR_RunCinematic( void );
void SND_Update( void );
void SND_UpdateLoopingSounds( void );
void Com_Printf( const char *fmt, ...);
void Com_DPrintf( const char *fmt, ...);
void Com_Error(errorParm_t code, const char *fmt, ...);
void Com_WriteConfigToFile(const char *contents);

qboolean Com_SafeMode( void );
int Com_Milliseconds( void );

void Sys_Init (void);
int Sys_Milliseconds (void);

#define Sys_MilliSeconds Sys_Milliseconds

sysEvent_t Sys_GetEvent( void );

void Sys_Print( const char *msg );
const char *Sys_GetCurrentUser( void );

void Sys_Error( const char *error, ...);
void Sys_Quit(void);
char *Sys_GetClipboardData( void );

void Sys_ShowConsole( int level, qboolean quitOnClose );

char **Sys_ListFiles( const char *directory, const char *extension, const char *filter, int *numfiles, qboolean wantsubs );
void Sys_FreeFileList( char **list );
char *Sys_Cwd( void );
void Sys_StreamSeek( fileHandle_t f, int offset, int origin );
void Sys_EndStreamedFile( fileHandle_t f );
qboolean Sys_DirectoryHasContents(const char *dir);
void Sys_Mkdir( const char *path );
char *Sys_Cwd( void );
void Sys_SetDefaultCDPath(const char *path);
const char *Sys_DefaultCDPath(void);
void Sys_SetDefaultInstallPath(const char *path);
char *Sys_DefaultInstallPath(void);
void Sys_SetDefaultHomePath(const char *path);
const char *Sys_DefaultHomePath(void);

void Com_DvarDump(print_msg_type_t channel);

int Sys_MillisecondsRaw( void );

void Com_BeginRedirect (char *buffer, int buffersize, void (*flush)( char *) );
void Com_EndRedirect (void);

void Com_SetWeaponInfoMemory(int source);
void Com_FreeWeaponInfoMemory(int source);

void Info_Print( const char *s );

const char *GetBspExtension();
void Com_LoadBsp(const char *filename);
void Com_UnloadBsp();
void Com_CleanupBsp();
bool Com_IsBspLoaded();
void *Com_GetBspHeader(int *size, int *checksum);

void Com_ServerDObjCreate(DObjModel_s *dobjModels, unsigned short numModels, XAnimTree_s *tree, int handle);
void Com_SafeServerDObjFree(int handle);
DObj* Com_GetServerDObj(int handle);
void Com_InitDObj();
void Com_ShutdownDObj();
void FX_FreeSystem();
void Com_AbortDObj();

void Sys_OutOfMemErrorInternal(const char *filename, int line);
void Sys_UnimplementedFunctionInternal(const char *function);

#define UNIMPLEMENTED Sys_UnimplementedFunctionInternal

void Com_Init(char* commandLine);
void Com_Restart();
void Com_Shutdown(const char *reason);
void Com_Frame(void);
void Com_Quit_f( void );

extern int com_frameTime;
extern int com_fixedConsolePosition;

qboolean SV_GameCommand();
qboolean CL_GameCommand();
void CL_ForwardCommandToServer( const char *string );
void CL_Disconnect();
void CL_ShutdownAll();
void CL_ShutdownDemo();

// client-side state shared with the cgame/ui modules
struct LegacyHacks
{
	int cg_norender;
	int cl_running;
	int cl_stance;
	int cl_stanceTemp;
	int cl_downloadSize;
	int cl_downloadCount;
	int cl_downloadTime;
	char cl_downloadName[64];
	char cl_serverloadmap[64];
	char cl_serverloadgametype[64];
	unsigned char cl_serverloadwaiting;
	unsigned char sv_killserver;
	char sv_killreason[256];
	char ui_scriptMenu[256];
	int ui_scriptMenuIndex;
	char ui_newScriptMenu[256];
	int ui_newScriptMenuIndex;
	char ui_waitingScriptMenu[256];
	int ui_waitingScriptMenuIndex;
	unsigned char ui_waitingScriptMenuNoMouse;
	unsigned char ui_scriptMenuAllowResponse;
	char ui_savegameName[256];
	char ui_savegameInfo[256];
	unsigned char winnt;
	int persid;
};

extern LegacyHacks *legacyHacks;

#include "cm_public.h"

void CM_InitThreadData(int threadContext);
void CM_LoadMapFromBsp(const char *name, bool usePvs);
void* CM_Hunk_Alloc(int size, const char *name, int type);
void CM_Hunk_CheckTempMemoryHighClear();
void CM_Hunk_ClearTempMemoryHigh();
void* CM_Hunk_AllocateTempMemoryHigh(int size, const char *name);
void CM_Hunk_CheckTempMemoryClear();
void CM_Hunk_ClearTempMemory();
byte* Com_GetBspLump(int type, int elemSize, int *count);
bool Com_BspHasLump(int type);
void CM_LoadMapFromBsp(const char *name, bool usePvs);
void CM_LoadMap(const char *name, int *checksum);
void CM_LoadStaticModels();
void CM_Cleanup(void);
void CM_Shutdown();

void CM_LinkEntity(svEntity_t *ent, float *absmin, float *absmax, clipHandle_t clipHandle);
void CM_UnlinkEntity(svEntity_t *ent);

clipHandle_t CM_TempBoxModel(const vec3_t mins, const vec3_t maxs, int capsule);
int CM_LeafCluster( int leafnum );
void CM_ModelBounds( clipHandle_t model, vec3_t mins, vec3_t maxs );
int CM_BoxLeafnums( const vec3_t mins, const vec3_t maxs, int *list, int listsize, int *lastLeaf );
byte *CM_ClusterPVS( int cluster );
int CM_PointLeafnum( const vec3_t p );

void CM_GetBox(cbrush_t **box_brush, cmodel_t **box_model);

int Com_GetFreeDObjIndex();

#include "../xanim/xanim_public.h"
XModel* CM_XModelPrecache(const char *name);

#include "../game/g_shared.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>
#include <stddef.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/time.h>
#include <ctype.h>
