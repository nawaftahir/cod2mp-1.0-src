// Stand-in for client code that passes these by pointer (renderer imports, callbacks)
#include "../qcommon/qcommon.h"
#include "../server/server.h"
#include "../bgame/bg_public.h"
#include "../qcommon/cm_public.h"
#include "../qcommon/cmd.h"
#include "../universal/com_memory.h"
#include "../universal/com_files.h"
#include "../universal/dvar.h"
#include "../xanim/xanim_public.h"
#include "../game/g_shared.h"
#include "../qcommon/netchan.h"
void G_LogPrintf( const char *fmt, ... );
void *spike_addr_taken[] = {
	(void *)va, (void *)Com_sprintf, (void *)Com_DPrintf, (void *)Com_Error, (void *)Com_Printf,
	(void *)FS_Printf, (void *)G_LogPrintf, (void *)SV_SendServerCommand, (void *)Sys_Error,
	(void *)BG_CalculateWeaponPosition_GunRecoil, (void *)CM_BoxTrace, (void *)CM_TraceThroughBrush,
	(void *)Cmd_RemoveCommand, (void *)DB_EnumXAssets, (void *)DObjCalcAnim, (void *)DObjCalcSkel,
	(void *)DObjCreate, (void *)DObjCreateSkel, (void *)DObjGetRotTransArray, (void *)DObjSkelAreBonesUpToDate,
	(void *)Dvar_GetBool, (void *)Dvar_RegisterFloat, (void *)FS_FOpenFileByMode, (void *)FS_FOpenFileWrite,
	(void *)FS_FreeFileList, (void *)FS_ListFiles, (void *)FS_Read, (void *)FS_Restart, (void *)G_SetFixedLink,
	(void *)Hunk_AllocateTempMemoryInternal, (void *)Netchan_Transmit, (void *)SV_EntityContact,
	(void *)VectorInverse, (void *)XModelBad, (void *)XModelGetLodForDist,
};
