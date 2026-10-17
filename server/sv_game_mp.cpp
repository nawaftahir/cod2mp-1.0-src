#include "../qcommon/qcommon.h"
#include "../universal/com_sndalias.h"

// names follow CoD2rev; the binary carries no symbols for these two
void FX_InitServer( void );
void Sys_LoadingKeepAlive( void );

qboolean gameInitialized = qfalse;

// the cache sits on a 128-byte boundary
static char g_sv_skel_memory[MAX_GENTITIES * MAX_MODELS] __attribute__((aligned(128)));
static char *g_sv_skel_memory_start;
static int g_sv_skel_warn_count;

int sv_serverId_value = 0;

#define SKEL_MEM_ALIGNMENT 16

// 1.0 calls this from the dedicated build too; the sound header only
// declares it for the client.
extern void Com_UnloadSoundAliases( snd_alias_system_t system );

void SV_GameSendServerCommand( int clientNum, int svscmd_type, const char *text );
void SV_GameDropClient( int clientNum, const char *reason );
void SV_SetBrushModel( gentity_t *ent );
qboolean SV_inSnapshot( const vec3_t origin, int iEntityNum );
qboolean SV_EntityContact( const vec3_t mins, const vec3_t maxs, const gentity_t *gEnt );
void SV_GetServerinfo( char *buffer, int bufferSize );
void* SV_Hunk_AllocInternal( int size );
void* SV_Hunk_AllocAlignInternal( int size, int aligment );
void* SV_Hunk_AllocateTempMemoryInternal( int size );
void SV_Hunk_FreeTempMemoryInternal( void *buf );
void SV_LocateGameData( gentity_t *gEnts, int numGEntities, int sizeofGEntity_t, playerState_t *clients, int sizeofGameClient );
void SV_GetUsercmd( int clientNum, usercmd_t *cmd );
void *SV_AllocXModelPrecache( int size );
void *SV_AllocXModelPrecacheColl( int size );
XModel* SV_XModelGet( const char *name );
void SV_DObjDumpInfo( gentity_t *ent );
void SV_ResetSkeletonCache();
char* SV_AllocSkelMemory( unsigned int size );
int SV_DObjCreateSkelForBone( gentity_t *ent, int boneIndex );
int SV_DObjCreateSkelForBones( gentity_t *ent, int *partBits );
int SV_DObjUpdateServerTime( gentity_t *ent, float dtime, int bNotify );
void SV_DObjInitServerTime( gentity_t *ent, float dtime );
void SV_DObjGetHierarchyBits( gentity_t *ent, int boneIndex, int *partBits );
void SV_DObjCalcAnim( gentity_t *ent, int *partBits );
void SV_DObjCalcSkel( gentity_t *ent, int *partBits );
int SV_DObjNumBones( gentity_t *ent );
int SV_DObjGetBoneIndex( const gentity_t *ent, unsigned int boneName );
DObjAnimMat* SV_DObjGetMatrixArray( gentity_t *ent );
void SV_DObjDisplayAnim( gentity_t *ent );
DObjAnimMat* SV_DObjGetRotTransArray( gentity_t *ent );
int SV_DObjSetRotTransIndex( const gentity_t *ent, int *partBits, int boneIndex );
int SV_DObjSetControlRotTransIndex( const gentity_t *ent, int *partBits, int boneIndex );
XAnimTree_s* SV_DObjGetTree( gentity_t *ent );
void SV_XModelDebugBoxes( gentity_s *ent );
qboolean SV_MapExists( const char *name );
void SV_ResetEntityParsePoint();
qboolean SV_DObjExists( gentity_t *ent );
void SV_SetWeaponInfoMemory();
void SV_FreeWeaponInfoMemory();
qboolean SV_GetEntityToken( char *buffer, int bufferSize );
int SV_GetGuid( int clientNum );
int SV_GetClientPing( int clientNum );
qboolean SV_IsLocalClient( int clientNum );
void SV_ShutdownGameProgs();
void SV_SetGametype();
void SV_InitGameVM( qboolean restart, qboolean registerDvars );
void SV_RestartGameProgs( qboolean savepersist );
void SV_InitGameProgs( qboolean savepersist );
qboolean SV_GameCommand( void );

/*
===============
SV_XModelDebugBoxes
===============
*/
extern void DObjGetBoneInfo( const DObj *obj, XBoneInfo **boneInfo );
extern int DObjGetNumModels( const DObj_s *obj );
extern bool DObjIgnoreCollision( const DObj_s *obj, int modelIndex );
extern void CL_AddDebugLine( const vec_t *start, const vec_t *end, const vec_t *color, qboolean depthTest, int duration, qboolean fromServer );
extern const int boxVerts[24][3] =
{
	{ 0, 0, 0 }, { 1, 0, 0 },
	{ 0, 0, 0 }, { 0, 1, 0 },
	{ 1, 1, 0 }, { 1, 0, 0 },
	{ 1, 1, 0 }, { 0, 1, 0 },
	{ 0, 0, 1 }, { 1, 0, 1 },
	{ 0, 0, 1 }, { 0, 1, 1 },
	{ 1, 1, 1 }, { 1, 0, 1 },
	{ 1, 1, 1 }, { 0, 1, 1 },
	{ 0, 0, 0 }, { 0, 0, 1 },
	{ 1, 0, 0 }, { 1, 0, 1 },
	{ 0, 1, 0 }, { 0, 1, 1 },
	{ 1, 1, 0 }, { 1, 1, 1 },
};

// these functions must be used instead of pointer arithmetic, because
// the game allocates gentities with private information after the server shared part
int SV_NumForGentity( gentity_t *ent )
{
	int num;

	num = ( (byte *)ent - (byte *)sv.gentities ) / sv.gentitySize;

	return num;
}

gentity_t *SV_GentityNum( int num )
{
	gentity_t *ent;

	ent = ( gentity_t * )( (byte *)sv.gentities + sv.gentitySize * ( num ) );

	return ent;
}

playerState_t *SV_GameClientNum( int num )
{
	playerState_t   *ps;

	ps = ( playerState_t * )( (byte *)sv.gameClients + sv.gameClientSize * ( num ) );

	return ps;
}

svEntity_t *SV_SvEntityForGentity( gentity_t *gEnt )
{
	if ( !gEnt || gEnt->s.number < 0 || gEnt->s.number >= MAX_GENTITIES )
	{
		Com_Error( ERR_DROP, "\x15" "SV_SvEntityForGentity: bad gEnt" );
	}

	return &sv.svEntities[ gEnt->s.number ];
}

gentity_t *SV_GEntityForSvEntity( svEntity_t *svEnt )
{
	int num;

	num = svEnt - sv.svEntities;
	return SV_GentityNum( num );
}

/*
===============
SV_GameSendServerCommand

Sends a command string to a client
===============
*/
void SV_GameSendServerCommand( int clientNum, int svscmd_type, const char *text )
{
	if ( clientNum == -1 )
	{
		SV_SendServerCommand( NULL, svscmd_type, "%s", text );
	}
	else
	{
		if ( clientNum < 0 || clientNum >= sv_maxclients->current.integer )
		{
			return;
		}
		SV_SendServerCommand( svs.clients + clientNum, svscmd_type, "%s", text );
	}
}

/*
===============
SV_GameDropClient

Disconnects the client with a message
===============
*/
void SV_GameDropClient( int clientNum, const char *reason )
{
	if ( clientNum < 0 || clientNum >= sv_maxclients->current.integer )
	{
		return;
	}

	SV_DropClient( svs.clients + clientNum, reason );
}

/*
=================
SV_SetBrushModel

sets mins and maxs for inline bmodels
=================
*/
void SV_SetBrushModel( gentity_t *ent )
{
	vec3_t mins;
	vec3_t maxs;

	assert(ent->r.inuse);
	CM_ModelBounds(ent->s.index, mins, maxs);
	VectorCopy(mins, ent->r.mins);
	VectorCopy(maxs, ent->r.maxs);
	ent->r.bmodel = qtrue;

	ent->r.contents = -1;     // we don't know exactly what is in the brushes

	SV_LinkEntity(ent);       // FIXME: remove
}

/*
===============
SV_inSnapshot
===============
*/
qboolean SV_inSnapshot( const vec3_t origin, int iEntityNum )
{
	gentity_t *ent;
	svEntity_s *svEnt;
	int clientcluster;
	int leafnum;
	int l;
	int i;
	byte *clientpvs;
	byte *bitvector;
	float fogOpaqueDistSqrd;

	ent = SV_GentityNum(iEntityNum);

	if ( !ent->r.linked )
	{
		return qfalse;
	}

	if ( ent->r.broadcastTime )
	{
		return qtrue;
	}

	if ( ent->r.svFlags & SVF_NOCLIENT )
	{
		return qfalse;
	}

	if ( ent->r.svFlags & ( SVF_BROADCAST | SVF_OBJECTIVE ) )
	{
		return qtrue;
	}

	svEnt = SV_SvEntityForGentity(ent);
	leafnum = CM_PointLeafnum(origin);

	if ( !svEnt->numClusters )
	{
		return qfalse;
	}

	clientcluster = CM_LeafCluster(leafnum);
	clientpvs = CM_ClusterPVS(clientcluster);
	bitvector = clientpvs;
	l = 0;

	for ( i = 0; i < svEnt->numClusters; ++i )
	{
		l = svEnt->clusternums[i];

		if ( bitvector[l >> 3] & ( 1 << ( l & 7 ) ) )
		{
			break;
		}
	}

	if ( i == svEnt->numClusters )
	{
		if ( svEnt->lastCluster )
		{
			while ( l <= svEnt->lastCluster )
			{
				if ( bitvector[l >> 3] & ( 1 << ( l & 7 ) ) )
				{
					break;
				}

				++l;
			}

			if ( l == svEnt->lastCluster )
			{
				return qfalse;
			}
		}
		else
		{
			return qfalse;
		}
	}

	fogOpaqueDistSqrd = G_GetFogOpaqueDistSqrd();

	if ( fogOpaqueDistSqrd == FLT_MAX )
	{
		return qtrue;
	}

	return BoxDistSqrdExceeds(ent->r.absmin, ent->r.absmax, origin, fogOpaqueDistSqrd) == 0;
}

/*
===============
SV_EntityContact
===============
*/
qboolean SV_EntityContact( const vec3_t mins, const vec3_t maxs, const gentity_t *gEnt )
{
	unsigned int model;
	vec2_t center;
	float dist;
	float distSq;
	trace_t trace;

	if ( gEnt->r.svFlags & ( SVF_CYLINDER | SVF_DISK ) )
	{
		if ( gEnt->r.svFlags & SVF_CYLINDER )
		{
			assert(gEnt->r.mins[2] == 0);

			if ( maxs[2] <= gEnt->r.currentOrigin[2] )
			{
				return qfalse;
			}

			if ( mins[2] >= gEnt->r.currentOrigin[2] + gEnt->r.maxs[2] )
			{
				return qfalse;
			}

			Vector2Add(mins, maxs, center);
			Vec2Scale(center, 0.5, center);

			dist = gEnt->r.maxs[0] + (maxs[0] - center[0]);
			distSq = Square(dist);

			return Vec2DistanceSq(gEnt->r.currentOrigin, center) < distSq;
		}
		else
		{
			assert(gEnt->r.svFlags & SVF_DISK);

			Vector2Add(mins, maxs, center);
			Vec2Scale(center, 0.5, center);

			dist = gEnt->r.maxs[0] + (maxs[0] - center[0]) - 64;
			distSq = Square(dist);

			return Vec2DistanceSq(gEnt->r.currentOrigin, center) >= distSq;
		}
	}

	model = SV_ClipHandleForEntity(gEnt);
	CM_TransformedBoxTraceExternal(&trace, vec3_origin, vec3_origin, mins, maxs, model, -1, gEnt->r.currentOrigin, gEnt->r.currentAngles);

	return trace.startsolid;
}

/*
===============
SV_GetServerinfo
===============
*/
void SV_GetServerinfo( char *buffer, int bufferSize )
{
	if ( bufferSize < 1 )
	{
		Com_Error(ERR_DROP, "\x15" "SV_GetServerinfo: bufferSize == %i", bufferSize);
	}

	I_strncpyz(buffer, Dvar_InfoString(DVAR_SERVERINFO | DVAR_SERVERINFO_NOUPDATE), bufferSize);
}

/*
===============
SV_Hunk_AllocInternal
===============
*/
void* SV_Hunk_AllocInternal( int size )
{
	if ( sv.state != SS_LOADING )
		Com_Error( ERR_DROP, "\x15Hunk_Alloc can only be called in G_InitGame and the first few frames of G_RunFrame\n" );

	return Hunk_Alloc(size);
}

/*
===============
SV_Hunk_AllocAlignInternal
===============
*/
void* SV_Hunk_AllocAlignInternal( int size, int aligment )
{
	if ( sv.state != SS_LOADING )
		Com_Error( ERR_DROP, "\x15Hunk_AllocAlign can only be called in G_InitGame and the first few frames of G_RunFrame\n" );

	return Hunk_AllocAlignInternal(size, aligment);
}

/*
===============
SV_Hunk_AllocateTempMemoryInternal
===============
*/
void* SV_Hunk_AllocateTempMemoryInternal( int size )
{
	return Hunk_AllocateTempMemory(size);
}

/*
===============
SV_Hunk_FreeTempMemoryInternal
===============
*/
void SV_Hunk_FreeTempMemoryInternal( void *buf )
{
	Hunk_FreeTempMemory(buf);
}

/*
===============
SV_LocateGameData
===============
*/
void SV_LocateGameData( gentity_t *gEnts, int numGEntities, int sizeofGEntity_t, playerState_t *clients, int sizeofGameClient )
{
	sv.gentities = gEnts;
	sv.gentitySize = sizeofGEntity_t;
	sv.num_entities = numGEntities;

	sv.gameClients = clients;
	sv.gameClientSize = sizeofGameClient;
}

/*
===============
SV_GetUsercmd
===============
*/
void SV_GetUsercmd( int clientNum, usercmd_t *cmd )
{
	*cmd = svs.clients[clientNum].lastUsercmd;
}

/*
===============
SV_AllocXModelPrecache
===============
*/
void *SV_AllocXModelPrecache( int size )
{
	return Hunk_Alloc(size);
}

/*
===============
SV_AllocXModelPrecacheColl
===============
*/
void *SV_AllocXModelPrecacheColl( int size )
{
	return Hunk_Alloc(size);
}

/*
===============
SV_XModelGet
===============
*/
XModel* SV_XModelGet( const char *name )
{
	if ( !Com_ValidXModelName(name) )
		Com_Error(ERR_DROP, "\x15" "bad model name '%s'", name);

	return XModelPrecache(name + 7, SV_AllocXModelPrecache, SV_AllocXModelPrecacheColl);
}

/*
===============
SV_DObjDumpInfo
===============
*/
void SV_DObjDumpInfo( gentity_t *ent )
{
	DObj *obj;

	if ( !com_developer->current.integer )
	{
		return;
	}

	obj = Com_GetServerDObj(ent->s.number);

	if ( !obj )
		Com_Printf("no model.\n");
	else
		DObjDumpInfo(obj);
}

/*
===============
SV_ResetSkeletonCache
===============
*/
void SV_ResetSkeletonCache()
{
	if ( !++sv.skelTimeStamp )
		++sv.skelTimeStamp;

	g_sv_skel_memory_start = (char *)PADP(g_sv_skel_memory, SKEL_MEM_ALIGNMENT);
	sv.skelMemPos = 0;
}

/*
===============
SV_AllocSkelMemory
===============
*/
char* SV_AllocSkelMemory( unsigned int size )
{
	char *pos;
	char *ret;

	size = PAD(size, SKEL_MEM_ALIGNMENT);

	while ( 1 )
	{
		pos = &g_sv_skel_memory_start[sv.skelMemPos];
		sv.skelMemPos += size;

		if ( sv.skelMemPos <= sizeof(g_sv_skel_memory) - SKEL_MEM_ALIGNMENT )
		{
			ret = pos;
			break;
		}

		if ( g_sv_skel_warn_count != sv.skelTimeStamp )
		{
			g_sv_skel_warn_count = sv.skelTimeStamp;
			Com_Printf("^3WARNING: SV_SKEL_MEMORY_SIZE exceeded\n");
		}

		SV_ResetSkeletonCache();
	}

	return ret;
}

/*
===============
SV_DObjCreateSkelForBone
===============
*/
int SV_DObjCreateSkelForBone( gentity_t *ent, int boneIndex )
{
	DObj *obj;
	int size;
	DSkel_t *buf;
	int result;

	obj = Com_GetServerDObj(ent->s.number);

	if ( DObjSkelExists(obj, sv.skelTimeStamp) )
	{
		result = DObjSkelIsBoneUpToDate(obj, boneIndex);
	}
	else
	{
		size = DObjGetAllocSkelSize(obj);
		buf = (DSkel_t *)SV_AllocSkelMemory(size);
		DObjCreateSkel(obj, buf, sv.skelTimeStamp);
		result = 0;
	}

	return result;
}

/*
===============
SV_DObjCreateSkelForBones
===============
*/
int SV_DObjCreateSkelForBones( gentity_t *ent, int *partBits )
{
	DObj *obj;
	int size;
	DSkel_t *buf;
	int result;

	obj = Com_GetServerDObj(ent->s.number);

	if ( DObjSkelExists(obj, sv.skelTimeStamp) )
	{
		result = DObjSkelAreBonesUpToDate(obj, partBits);
	}
	else
	{
		size = DObjGetAllocSkelSize(obj);
		buf = (DSkel_t *)SV_AllocSkelMemory(size);
		DObjCreateSkel(obj, buf, sv.skelTimeStamp);
		result = 0;
	}

	return result;
}

/*
===============
SV_DObjUpdateServerTime
===============
*/
int SV_DObjUpdateServerTime( gentity_t *ent, float dtime, int bNotify )
{
	DObj *obj;
	int result;

	obj = Com_GetServerDObj(ent->s.number);

	if ( !obj )
	{
		result = 0;
	}
	else
	{
		result = DObjUpdateServerInfo(obj, dtime, bNotify);
	}

	return result;
}

/*
===============
SV_DObjInitServerTime
===============
*/
void SV_DObjInitServerTime( gentity_t *ent, float dtime )
{
	DObj *obj;

	obj = Com_GetServerDObj(ent->s.number);

	if ( !obj )
		return;

	DObjInitServerTime(obj, dtime);
}

/*
===============
SV_DObjGetHierarchyBits
===============
*/
void SV_DObjGetHierarchyBits( gentity_t *ent, int boneIndex, int *partBits )
{
	DObj *obj;

	obj = Com_GetServerDObj(ent->s.number);
	DObjGetHierarchyBits(obj, boneIndex, partBits);
}

/*
===============
SV_DObjCalcAnim
===============
*/
void SV_DObjCalcAnim( gentity_t *ent, int *partBits )
{
	DObj *obj;

	obj = Com_GetServerDObj(ent->s.number);
	DObjCalcAnim(obj, partBits);
}

/*
===============
SV_DObjCalcSkel
===============
*/
void SV_DObjCalcSkel( gentity_t *ent, int *partBits )
{
	DObj *obj;

	obj = Com_GetServerDObj(ent->s.number);
	DObjCalcSkel(obj, partBits);
}

/*
===============
SV_DObjNumBones
===============
*/
int SV_DObjNumBones( gentity_t *ent )
{
	DObj *obj;

	obj = Com_GetServerDObj(ent->s.number);
	return DObjNumBones(obj);
}

/*
===============
SV_DObjGetBoneIndex
===============
*/
int SV_DObjGetBoneIndex( const gentity_t *ent, unsigned int boneName )
{
	DObj *obj;

	obj = Com_GetServerDObj(ent->s.number);

	if ( !obj )
	{
		return -1;
	}

	return DObjGetBoneIndex(obj, boneName);
}

/*
===============
SV_DObjGetMatrixArray
===============
*/
DObjAnimMat* SV_DObjGetMatrixArray( gentity_t *ent )
{
	DObj *obj;

	obj = Com_GetServerDObj(ent->s.number);
	return DObjGetRotTransArray(obj);
}

/*
===============
SV_DObjDisplayAnim
===============
*/
void SV_DObjDisplayAnim( gentity_t *ent )
{
	DObj *obj;

	obj = Com_GetServerDObj(ent->s.number);

	if ( !obj )
		return;

	DObjDisplayAnim(obj);
}

/*
===============
SV_DObjGetRotTransArray
===============
*/
DObjAnimMat* SV_DObjGetRotTransArray( gentity_t *ent )
{
	DObj *obj;

	obj = Com_GetServerDObj(ent->s.number);
	return DObjGetRotTransArray(obj);
}

/*
===============
SV_DObjSetRotTransIndex
===============
*/
int SV_DObjSetRotTransIndex( const gentity_t *ent, int *partBits, int boneIndex )
{
	DObj *obj;

	obj = Com_GetServerDObj(ent->s.number);
	return DObjSetRotTransIndex(obj, partBits, boneIndex);
}

/*
===============
SV_DObjSetControlRotTransIndex
===============
*/
int SV_DObjSetControlRotTransIndex( const gentity_t *ent, int *partBits, int boneIndex )
{
	DObj *obj;

	obj = Com_GetServerDObj(ent->s.number);
	return DObjSetControlRotTransIndex(obj, partBits, boneIndex);
}

/*
===============
SV_DObjGetTree
===============
*/
XAnimTree_s* SV_DObjGetTree( gentity_t *ent )
{
	DObj *obj;

	obj = Com_GetServerDObj(ent->s.number);

	return obj ? DObjGetTree(obj) : NULL;
}

void SV_XModelDebugBoxes( gentity_s *ent )
{
	DObj_s *obj;
	DObjAnimMat *mat;
	XBoneInfo *boneInfoArray[128];
	XBoneInfo *boneInfo;
	vec3_t axis[3];
	float boneMat[4][3];
	vec4_t color;
	vec3_t org;
	vec3_t vec;
	vec3_t start;
	vec3_t end;
	int numModels;
	int modelIndex;
	int numBones;
	int localBoneIndex;
	int edge;
	const int *vert;

	obj = Com_GetServerDObj(ent->s.number);
	DObjGetBoneInfo(obj, boneInfoArray);
	mat = DObjGetRotTransArray(obj);

	color[0] = 1.0f;
	color[1] = 1.0f;
	color[2] = 1.0f;
	color[3] = 0.0f;

	AnglesToAxis(ent->r.currentAngles, axis);

	numModels = DObjGetNumModels(obj);

	for ( modelIndex = 0; modelIndex < numModels; modelIndex++ )
	{
		if ( DObjIgnoreCollision(obj, modelIndex) )
			continue;

		numBones = XModelNumBones(DObjGetModel(obj, modelIndex));

		for ( localBoneIndex = 0; localBoneIndex < numBones; localBoneIndex++, mat++ )
		{
			boneInfo = boneInfoArray[localBoneIndex];
			vert = boxVerts[0];
			ConvertQuatToMat(mat, boneMat);
			VectorCopy(mat->trans, boneMat[3]);

			for ( edge = 0; edge < 12; edge++ )
			{
				org[0] = boneInfo->bounds[vert[0]][0];
				org[1] = boneInfo->bounds[vert[1]][1];
				org[2] = boneInfo->bounds[vert[2]][2];
				vert += 3;
				MatrixTransformVector43(org, boneMat, vec);
				MatrixTransformVector(vec, axis, start);
				VectorAdd(start, ent->r.currentOrigin, start);

				org[0] = boneInfo->bounds[vert[0]][0];
				org[1] = boneInfo->bounds[vert[1]][1];
				org[2] = boneInfo->bounds[vert[2]][2];
				vert += 3;
				MatrixTransformVector43(org, boneMat, vec);
				MatrixTransformVector(vec, axis, end);
				VectorAdd(end, ent->r.currentOrigin, end);

				CL_AddDebugLine(start, end, color, 0, 0, 1);
			}
		}
	}
}

/*
====================
SV_MapExists
====================
*/
qboolean SV_MapExists( const char *name )
{
	const char *basename;
	const char *fullpath;

	basename = SV_GetMapBaseName(name);
	fullpath = va("maps/mp/%s.%s", basename, GetBspExtension());

	return FS_ReadFile(fullpath, 0) >= 0;
}

/*
===============
SV_ResetEntityParsePoint
===============
*/
void SV_ResetEntityParsePoint()
{
	sv.entityParsePoint = CM_EntityString();
}

/*
===============
SV_DObjExists
===============
*/
qboolean SV_DObjExists( gentity_t *ent )
{
	return Com_GetServerDObj(ent->s.number) != NULL;
}

/*
===============
SV_SetWeaponInfoMemory
===============
*/
void SV_SetWeaponInfoMemory()
{
	Com_SetWeaponInfoMemory(1);
}

/*
===============
SV_FreeWeaponInfoMemory
===============
*/
void SV_FreeWeaponInfoMemory()
{
	Com_FreeWeaponInfoMemory(1);
}

/*
====================
SV_GetEntityToken
====================
*/
qboolean SV_GetEntityToken( char *buffer, int bufferSize )
{
	const char *s;

	s = Com_Parse(&sv.entityParsePoint);
	I_strncpyz(buffer, s, bufferSize);

	return sv.entityParsePoint || *s;
}

/*
===============
SV_GetGuid
===============
*/
int SV_GetGuid( int clientNum )
{
	assert(sv_maxclients);

	if ( clientNum < 0 || clientNum >= sv_maxclients->current.integer )
	{
		return 0;
	}

	return svs.clients[clientNum].guid;
}

/*
====================
SV_GetClientPing
====================
*/
int SV_GetClientPing( int clientNum )
{
	assert(clientNum >= 0 && clientNum < MAX_CLIENTS);
	return svs.clients[clientNum].ping;
}

/*
====================
SV_IsLocalClient
====================
*/
qboolean SV_IsLocalClient( int clientNum )
{
	assert(clientNum >= 0 && clientNum < MAX_CLIENTS);
	return NET_IsLocalAddress(svs.clients[clientNum].netchan.remoteAddress);
}

/*
===============
SV_ShutdownGameProgs

Called every time a map changes
===============
*/
void SV_ShutdownGameProgs()
{
	sv.state = SS_DEAD;

	Com_UnloadSoundAliases(SASYS_GAME);

	if ( !gameInitialized )
	{
		return;
	}

	G_ShutdownGame(qtrue);
	SV_FreeWeaponInfoMemory();
	gameInitialized = qfalse;
}

/*
===============
SV_SetGametype
===============
*/
void SV_SetGametype()
{
	char *pszGametype;
	char gametype[MAX_QPATH];

	Dvar_RegisterString("g_gametype", "dm", DVAR_SERVERINFO | DVAR_LATCH | DVAR_CHANGEABLE_RESET);

	if ( com_sv_running->current.boolean && G_GetSavePersist() )
		I_strncpyz(gametype, sv.gametype, sizeof(gametype));
	else
		I_strncpyz(gametype, sv_gametype->current.string, sizeof(gametype));

	for ( pszGametype = gametype; *pszGametype; pszGametype++ )
		*pszGametype = tolower(*pszGametype);

	if ( !Scr_IsValidGameType(gametype) )
	{
		Com_Printf("g_gametype %s is not a valid gametype, defaulting to dm\n", gametype);
		strcpy(gametype, "dm");
	}

	Dvar_SetString(sv_gametype, gametype);
}

/*
==================
SV_InitGameVM

Called for both a full init and a restart
==================
*/
void SV_InitGameVM( qboolean restart, qboolean registerDvars )
{
	int i;

	FX_InitServer();

	// start the entity parsing at the beginning
	sv.entityParsePoint = CM_EntityString();

	Sys_LoadingKeepAlive();

	// use the current msec count for a random seed
	// init for this gamestate
	G_InitGame(svs.time, Sys_MillisecondsRaw(), restart, registerDvars);

	Sys_LoadingKeepAlive();

	// clear all gentity pointers that might still be set from
	// a previous level
	for ( i = 0 ; i < sv_maxclients->current.integer ; i++ )
	{
		svs.clients[i].gentity = NULL;
	}

	if ( com_dedicated->current.integer )
	{
		Com_DvarDump(PMSG_LOGFILE);
	}
}

/*
===================
SV_RestartGameProgs

Called on a map_restart, but not on a normal map change
===================
*/
void SV_RestartGameProgs( qboolean savepersist )
{
	G_ShutdownGame(qfalse);
	com_fixedConsolePosition = 0;
	SV_InitGameVM(qtrue, savepersist);
}

/*
===============
SV_InitGameProgs

Called on a normal map change, not on a map_restart
===============
*/
void SV_InitGameProgs( qboolean savepersist )
{
	gameInitialized = qtrue;
	SV_InitGameVM(qfalse, savepersist);
}

/*
====================
SV_GameCommand

See if the current console command is claimed by the game
====================
*/
qboolean SV_GameCommand( void )
{
	// tests sv.state inline here rather than through SV_Loaded()
	if ( sv.state != SS_GAME )
	{
		return qfalse;
	}

	return ConsoleCommand();
}

