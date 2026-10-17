#ifndef CLEAN_PC_QCOMMON_QCOMMON_FUNCS_H
#define CLEAN_PC_QCOMMON_QCOMMON_FUNCS_H

#include "common_types.h"
#include "bytematch.h"
#include <signal.h>

#ifndef WWW_DOWNLOAD_H
typedef enum {
    DL_STATUS_IDLE,
    DL_STATUS_IN_PROGRESS,
    DL_STATUS_DONE,
    DL_STATUS_FAILED,
    DL_STATUS_CANCELLED
} dlStatus_t;
#endif

#ifndef CM_MESH_REGPARM3_ABI
#define CM_MESH_REGPARM3_ABI COD2_REGPARM(3)
#endif
#ifndef CM_MESH_REGPARM3_SSE_ABI
#define CM_MESH_REGPARM3_SSE_ABI COD2_REGPARM(3) COD2_SSEREGPARM
#endif

extern "C" const char *getBuildNumber(void);

void CM_LoadMap(const char *name, int *checksum);
void CM_Shutdown(void);
int CM_NumInlineModels(void);
const char *CM_EntityString(void);
int CM_LeafCluster(int leafnum);
void *CM_Hunk_Alloc(int size, const char *name, int type);
void CM_Hunk_CheckTempMemoryClear(void);
void CM_Hunk_CheckTempMemoryHighClear(void);
void *CM_Hunk_AllocateTempMemoryHigh(int size, const char *name);
void CM_Hunk_ClearTempMemory(void);
void CM_Hunk_ClearTempMemoryHigh(void);
void CM_ModelBounds(clipHandle_t model, vec_t *mins, vec_t *maxs);
void CM_SaveLump(int lumpnum, byte *newLump, int size, int *checksum);

void CM_Cleanup(void);
cplane_t *CM_GetPlaneNum(int planeNum);
void CMod_LoadPlanes(const byte *base, const lump_t *l);
void CM_LoadMapFromBsp(const char *name, int usePvs);

void CM_TraceThroughAabbTree(const traceWork_t *tw, CollisionAabbTree *aabbTree, trace_t *trace);
void CM_SightTraceThroughAabbTree(const traceWork_t *tw, CollisionAabbTree *aabbTree, trace_t *trace);
short int CM_MeshTestInLeaf(const traceWork_t *tw, cLeaf_t *leaf, trace_t *trace);

float *CM_Hunk_AllocXModel(int size);
struct XModel *CM_XModelPrecache(const char *name);
void CM_TraceStaticModel(cStaticModel_t *sm, trace_t *results, const vec_t *start, const vec_t *end, int contentmask);
qboolean CM_TraceStaticModelComplete(cStaticModel_t *sm, const vec_t *start, const vec_t *end, int contentmask);

void CM_LoadStaticModels(void);

byte *CM_ClusterPVS(int cluster);
void CM_BoxLeafnums_r(leafList_t *ll, int nodenum);
int CM_BoxLeafnums(const vec_t *mins, const vec_t *maxs, int *list, int listsize, int *lastLeaf);
int CM_PointLeafnum(const vec_t *p);
int CM_PointContents(const vec_t *p, clipHandle_t model);
int CM_TransformedPointContents(const vec_t *p, clipHandle_t model, const vec_t *origin, const vec_t *angles);

cmodel_t *CM_ClipHandleToModel(clipHandle_t handle);
int CM_ContentsOfModel( clipHandle_t handle );
float CM_RadiusOfModel(clipHandle_t handle);
clipHandle_t CM_TempBoxModel(const vec_t *mins, const vec_t *maxs, int contents);
void CM_BoxTrace(trace_t *results, const float *start, const float *end, const float *mins, const float *maxs, clipHandle_t model, int brushmask);
int CM_BoxSightTrace( int oldHitNum, const vec3_t start, const vec3_t end, const vec3_t mins, const vec3_t maxs, clipHandle_t model, int brushmask );
int CM_TransformedBoxTrace(trace_t *results, const vec_t *start, const vec_t *end, const vec_t *mins, const vec_t *maxs, clipHandle_t model, int brushmask, const vec_t *origin, const vec_t *angles);
int CM_TransformedBoxTraceExternal(trace_t *results, const vec_t *start, const vec_t *end, const vec_t *mins, const vec_t *maxs, clipHandle_t model, int brushmask, const vec_t *origin, const vec_t *angles);
int CM_TransformedBoxSightTrace(int hitNum, const vec_t *start, const vec_t *end, const vec_t *mins, const vec_t *maxs, clipHandle_t model, int brushmask, const vec_t *origin, const vec_t *angles);

void CM_CalcTraceEntents(TraceExtents *extents);
qboolean CM_TraceBox(const TraceExtents *extents, const vec_t *mins, const vec_t *maxs, float fraction);

void CM_UnlinkEntity(svEntity_t *ent);
int CM_AreaEntities(const vec_t *mins, const vec_t *maxs, int *entityList, int maxcount, int contentmask);
int CM_PointSightTraceToEntities(sightpointtrace_t *clip);
qboolean CM_PointTraceStaticModelsComplete(const vec_t *start, const vec_t *end, int contentmask);
int CM_ClipSightTraceToEntities(sightclip_t *clip);
void CM_PointTraceStaticModels(trace_t *results, const vec_t *start, const vec_t *end, int contentmask);
void CM_PointTraceToEntities(pointtrace_t *clip, trace_t *trace);
void CM_ClipMoveToEntities(moveclip_t *clip, trace_t *trace);
void CM_LinkWorld(void);
void CM_LinkEntity(svEntity_t *ent, vec_t *absmin, vec_t *absmax, clipHandle_t clipHandle);

void Cmd_Wait_f(void);
void Cbuf_Init(void);
void Cbuf_AddText(const char *text);
void Cbuf_ExecuteText(int exec_when, const char *text);
void Cbuf_Execute(void);
void Cmd_Vstr_f(void);
int Cmd_Argc(void);
int SV_Cmd_Argc(void);
const char *Cmd_Argv( int arg );
char *SV_Cmd_Argv(int arg);
void Cmd_ArgvBuffer(int arg, char *buffer, int bufferLength);
void SV_Cmd_ArgvBuffer(int arg, char *buffer, int bufferLength);
char *Cmd_Args(int start);
void Cmd_ArgsBuffer(char *buffer, int bufferLength);
void Cmd_TokenizeString2(const char *text_in, int max_tokens);
void Cmd_TokenizeString(const char *text_in);
void SV_Cmd_TokenizeString(const char *text_in);
void Cmd_AddCommand(const char *cmdName, xcommand_t function);
void Cmd_RemoveCommand(const char *cmdName);
void Cmd_SetAutoComplete(const char *cmdName, const char *dir, const char *ext);
void Cmd_Shutdown(void);
void Cmd_ForEach(void (*callback)(const char *));
const char **Cmd_GetAutoCompleteFileList(const char *cmdName, int *fileCount, int allocTrackType);
void Cmd_ExecuteString(const char *text);
void SV_Cmd_ExecuteString(const char *text);
void Cmd_List_f(void);
void Cmd_Init(void);

const dheader_t *Com_GetBsp(int *fileSize, unsigned int *checksum);
void Com_UnloadBsp(void);
void Com_LoadBsp(const char *filename);
void Com_CleanupBsp(void);

extern "C" size_t FS_FileRead( void *ptr, size_t size, size_t nmemb, FILE *stream );
size_t FS_FileWrite(const void *ptr, size_t size, size_t nitems, FILE *stream);
extern "C" FILE *FS_FileOpen( const char *filename, const char *mode );
extern "C" int FS_FileClose( FILE *stream );
extern "C" int FS_FileSeek( FILE *file, int offset, int whence );

Bool Com_HasPlayerProfile(void);
int Com_BuildPlayerProfilePath(char *path, int pathSize, const char *format, ...);
int Com_BuildPlayerProfilePathForPlayer(char *path, int pathSize, const char *playerName, const char *format, ...);
Bool Com_DeletePlayerProfile(const char *profileName);
void Com_ChangePlayerProfile(const char *profileName);
void Com_InitPlayerProfiles(void);
Bool Com_NewPlayerProfile(const char *profileName);

Bool Com_IsMapProfilerActive(void);

void Com_BeginRedirect(char *buffer, int buffersize, void (*flush)(char *));
void Com_EndRedirect(void);
void Com_Printf(const char *fmt, ...);
void Com_PrintMessage( print_msg_type_t channel, const char *msg );
void Com_DPrintf(const char *fmt, ...);
void Com_Error(errorParm_t code, const char *fmt, ...);
qboolean Com_SafeMode(void);
void Com_StartupVariable(const char *match);
void Info_Print(const char *s);
void Com_ShutdownEvents(void);
void Com_WriteCDKey(void);
void Com_WriteConfig_f(void);
float Com_GetTimescaleForSnd(void);
void Com_Close(void);
void Field_Clear(field_t *edit);
void Com_SetWeaponInfoMemory(int iSource);
void Com_FreeWeaponInfoMemory(int iSource);
int Com_AddToString(const char *add, char *msg, int len, int maxlen, qboolean mayAddQuotes);
char Com_GetDecimalDelimiter(void);
void Com_SetRecommended(qboolean restart);
void Com_CheckSetRecommended(void);
void Com_ReadCDKey(void);
void Com_LocalizedFloatToString(float f, char *buffer, unsigned int maxlen, unsigned int numDecimalPlaces);
void Com_Quit_f(void);
void Com_Restart(void);
void Com_ShutdownInternal(char *finalmsg);
void Com_ExecStartupConfigs(const char *configFile);
int Com_EventLoop(void);
qboolean Debug_EventLoop(void);
BM_NOINLINE void Com_Frame_Try_Block_Function(void);
void Com_Shutdown(char *finalmsg);
void Com_Frame(void);
void Com_WriteDefaults_f(void);
int Com_Milliseconds(void);
void Com_PumpMessageLoop(void);
void Com_Init_Try_Block_Function(char *commandLine);
void Com_Init(char *commandLine);

void Sys_CrashSetField(const char *name, const char *value);
void Sys_InstallCrashHandler(const char *appName, const char *version, const char *buildDate, const char *cmdline);

void DL_InitDownload(void);
int DL_BeginDownload(const char *localName, const char *remoteName, int debug);
void DL_DownloadLoop(void);
void DL_CancelDownload(void);
int DL_InProgress(void);
int DL_DLIsMotd(void);
dlStatus_t DL_GetStatus(void);

struct DObj_s *Com_GetClientDObj(int handle, int localClientNum);
struct DObj_s *Com_GetServerDObj(int handle);
void Com_ClientDObjClearAllSkel(void);
void Com_ServerDObjCreate(DObjModel_s *dobjModels, int numModels, struct XAnimTree_s *tree, int handle);
void Com_SafeClientDObjFree(int handle);
void Com_SafeServerDObjFree(int handle);
void Com_InitDObj(void);
void Com_ShutdownDObj(void);
void Com_AbortDObj(void);
void Com_ClientDObjCreate(DObjModel_s *dobjModels, unsigned short numModels, struct XAnimTree_s *tree, int handle);

void Dvar_ForEach(void (*callback)());
qboolean Dvar_Command(void);
void Dvar_Toggle_f(void);
void Dvar_TogglePrint_f(void);
void Dvar_Set_f(void);
void Dvar_SetU_f(void);
void Dvar_SetS_f(void);
void Dvar_SetA_f(void);
void Dvar_SetFromDvar_f(void);
void Dvar_Reset_f(void);
void Dvar_WriteVariables(fileHandle_t f);
void Dvar_WriteDefaults(fileHandle_t f);
void Dvar_List_f(void);
void Com_DvarDump(print_msg_type_t type);
void Dvar_Dump_f(void);
void SV_SetConfig(int start, int max, int bit);
char *Dvar_InfoString(int bit);
char *Dvar_InfoString_Big(int bit);
void Dvar_AddCommands(void);

fileHandle_t FS_SV_FOpenFileWrite(const char *filename);
int FS_SV_FOpenFileRead(const char *filename, fileHandle_t *fp);
void FS_SV_Rename(const char *from, const char *to);
char *FS_ShiftStr(const char *string, int shift);
void FS_Dir_f(void);
void FS_NewDir_f(void);
void FS_TouchFile_f(void);
qboolean FS_iwIwd(char *iwd, char *base);
void FS_AddCommands(void);
void FS_SetRestrictions(void);
const char *FS_LoadedIwdChecksums(void);
const char *FS_LoadedIwdNames(void);
const char *FS_LoadedIwdPureChecksums(void);
const char *FS_ReferencedIwdChecksums(void);
const char *FS_ReferencedIwdNames(void);
const char *FS_ReferencedIwdPureChecksums(void);
void FS_PureServerSetLoadedIwds(const char *iwdSums, const char *iwdNames);
void FS_PureServerSetReferencedIwds(const char *iwdSums, const char *iwdNames);
char *FS_GetMapBaseName(const char *mapname);
int FS_GetModList(char *listbuf, int bufsize);
qboolean FS_CompareIwds(char *needediwds, int len, qboolean dlstring);

void Huff_offsetReceive(node_t *node, int *ch, byte *fin, int *offset);
void huffman_send(node_t *node, node_t *child, byte *fout);
void Huff_addRef(huff_t *huff, int ch, msg_t *mbuf, int offset);
void Huff_offsetTransmit(huff_t *huff, int ch, byte *fout, int *offset);
void Huff_Init(huffman_t *huff);

void MD4Update(MD4_CTX *context, const unsigned char *input, unsigned int inputLen);
void MD4Final(unsigned char *digest, MD4_CTX *context);
unsigned int Com_BlockChecksumKey(void *buffer, int length, int key);
unsigned int Com_BlockChecksum(const void *buffer, int length);

void MSG_BeginReading(msg_t *msg);
void MSG_WriteBits(msg_t *msg, int value, int bits);
void MSG_WriteBit0(msg_t *msg);
void MSG_WriteBit1(msg_t *msg);
int MSG_ReadBits(msg_t *msg, int bits);
int MSG_ReadBit(msg_t *msg);
int MSG_WriteBitsCompress(byte *from, byte *to, int size);
int MSG_ReadBitsCompress(byte *from, byte *to, int size);
void MSG_WriteByte(msg_t *msg, int c);
void MSG_WriteData(msg_t *buf, const void *data, int length);
int MSG_ReadByte(msg_t *msg);
int MSG_ReadShort(msg_t *msg);
int MSG_ReadLong(msg_t *msg);
void MSG_ReadData(msg_t *msg, void *data, int len);
void MSG_WriteReliableCommandToBuffer(const char *pszCommand, char *pszBuffer, int iBufferSize);
void MSG_SetDefaultUserCmd(playerState_t *ps, usercmd_t *cmd);
char *MSG_ReadString(msg_t *msg);
char *MSG_ReadBigString(msg_t *msg);
char *MSG_ReadStringLine(msg_t *msg);
void MSG_WriteShort(msg_t *msg, int c);
void MSG_WriteLong(msg_t *msg, int c);
void MSG_Init(msg_t *buf, byte *data, int length);
void MSG_WriteString(msg_t *sb, const char *s);
void MSG_WriteBigString( msg_t *sb, const char *s );
void MSG_ReadDeltaField(msg_t *msg, byte *from, byte *to, const NetField *field, qboolean print);
qboolean MSG_ReadDeltaClient(msg_t *msg, clientState_t *from, clientState_t *to, int number);
qboolean MSG_ReadDeltaArchivedEntity(msg_t *msg, archivedEntity_t *from, archivedEntity_t *to, int number);
qboolean MSG_ReadDeltaEntity(msg_t *msg, entityState_t *from, entityState_t *to, int number);
void MSG_ReadDeltaPlayerstate(msg_t *msg, playerState_t *from, playerState_t *to);
void MSG_ReadDeltaUsercmdKey(msg_t *msg, int key, usercmd_t *from, usercmd_t *to);
void MSG_WriteDeltaUsercmdKey(msg_t *msg, int key, usercmd_t *from, usercmd_t *to);
void MSG_WriteDeltaClient(msg_t *msg, clientState_s *from, clientState_s *to, qboolean force);
void MSG_WriteDeltaArchivedEntity(msg_t *msg, archivedEntity_t *from, archivedEntity_t *to, qboolean force);
void MSG_WriteDeltaEntity(msg_t *msg, entityState_s *from, entityState_s *to, qboolean force);
void MSG_WriteDeltaPlayerstate(msg_t *msg, playerState_s *from, playerState_s *to);

void NetProf_PrepProfiling(netProfileInfo_t **prof);
void NetProf_AddPacket(netProfileStream_t *pProfStream, int iSize, qboolean bFragment);
void NetProf_UpdateStatistics(netProfileStream_t *pStream);
void Net_DumpProfile_f(void);
void Net_DisplayProfile(void);
void Netchan_Init(int port);
void Netchan_Setup(netsrc_t sock, netchan_t *chan, struct netadr_t adr, int qport);
int NET_CompareAdrSigned(netadr_t *a, netadr_t *b);
qboolean NET_IsLocalAddress(netadr_t adr);
qboolean NET_GetPacket(netadr_t *net_from, msg_t *net_message);
const char *NET_AdrToString(netadr_t a);
qboolean NET_StringToAdr(const char *s, netadr_t *a);
bool NET_SendPacket( netsrc_t sock, int length, const void *data, netadr_t to );
void NET_OutOfBandVoiceData(netsrc_t sock, netadr_t adr, byte *format, int len);
unsigned char NET_OutOfBandData(netsrc_t sock, netadr_t adr, byte *format, int len);
unsigned char NET_OutOfBandPrint(netsrc_t sock, netadr_t adr, const char *msg);
qboolean NET_CompareAdr(netadr_t a, netadr_t b);
qboolean NET_CompareBaseAdr(netadr_t a, netadr_t b);
qboolean NET_GetLoopPacket(netsrc_t sock, netadr_t *net_from, msg_t *net_message);
bool Netchan_TransmitNextFragment(netchan_t *chan);
bool Netchan_Transmit(netchan_t *chan, int length, const byte *data);
qboolean Netchan_Process(netchan_t *chan, msg_t *msg);

void StatMon_Warning(int type, int duration, const char *materialName);
void StatMon_GetStatsArray(const statmonitor_t **array, int *count);
void StatMon_Reset(void);

#endif
