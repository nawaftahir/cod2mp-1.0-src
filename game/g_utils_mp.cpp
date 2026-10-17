#include "../qcommon/qcommon.h"
#include "g_shared.h"
#include "../script/script_public.h"
#include "../server/server.h"

/*
=============
G_FindStr

Searches all active entities for the next one that holds
the matching string at fieldofs (use the FOFS() macro) in the structure.

Searches beginning at the entity after from, or the beginning if NULL
NULL will be returned if the end of the list is reached.

=============
*/
/*
=============
G_PickTarget

Selects a random entity from among the targets
=============
*/
#define MAXCHOICES 32

static XModel* cached_models[MAX_MODELS];

qboolean G_EntDetach( gentity_t *ent, const char *modelName, unsigned int tagName );
void G_EntUnlink( gentity_t *ent );
void G_UpdateTagInfoOfChildren( gentity_t *parent, qboolean bHasDObj );
void G_CalcTagAxis( gentity_t *ent, qboolean bAnglesOnly );
void G_SafeDObjFree( gentity_t *ent );
void G_DObjCalcBone( gentity_t *ent, int boneIndex );
void G_FreeEntity( gentity_t *ed );
void G_SetOrigin( gentity_t *ent, const vec3_t origin );
void G_SetAngle( gentity_t *ent, const vec3_t angle );

int G_FindConfigstringIndex( const char *name, int start, int max, qboolean create, const char *errormsg );
int G_LocalizedStringIndex( const char *string );
int G_ShaderIndex( const char *name );
int G_ModelIndex( const char *name );
XModel* G_GetModel( int index );
int G_XModelBad( int index );
const char* G_ModelName( int index );
int G_TagIndex( const char *name );
int G_EffectIndex( const char *name );
int G_ShellShockIndex( const char *name );
int G_SoundAliasIndex( const char *name );
void G_DObjUpdate( gentity_t *ent );
void G_SetModel( gentity_t *ent, const char *modelName );
void G_OverrideModel( int modelIndex, const char *defaultModelName );
qboolean G_EntAttach( gentity_t *ent, const char *modelName, unsigned int tagName, qboolean ignoreCollision );
qboolean G_EntDetach( gentity_t *ent, const char *modelName, unsigned int tagName );
void G_EntDetachAll( gentity_t *ent );
qboolean G_EntLinkTo( gentity_t *ent, gentity_t *parent, unsigned int tagName );
qboolean G_EntLinkToWithOffset( gentity_t *ent, gentity_t *parent, unsigned int tagName, const vec3_t originOffset, const vec3_t anglesOffset );
void G_EntUnlink( gentity_t *ent );
qboolean G_EntIsLinkedTo( gentity_t *ent, gentity_t *parent );
void G_UpdateTagInfo( gentity_t *parent, qboolean bParentHasDObj );
void G_UpdateTagInfoOfChildren( gentity_t *parent, qboolean bHasDObj );
void G_CalcTagParentRelAxis( gentity_t *ent, float parentAxis[4][3] );
void G_CalcTagAxis( gentity_t *ent, qboolean bAnglesOnly );
void G_SetFixedLink( gentity_t *ent, int eAngles );
void G_GeneralLink( gentity_t *ent );
void G_SafeDObjFree( gentity_t *ent );
int G_DObjUpdateServerTime( gentity_t *ent, qboolean bNotify );
void G_DObjCalcPose( gentity_t *ent );
void G_DObjCalcBone( gentity_t *ent, int boneIndex );
DObjAnimMat* G_DObjGetLocalTagMatrix( gentity_t *ent, unsigned int tagName );
qboolean G_DObjGetWorldTagMatrix( gentity_t *ent, unsigned int tagName, float tagMat[4][3] );
qboolean G_DObjGetWorldTagPos( gentity_t *ent, unsigned int tagName, vec3_t pos );
gentity_t *G_Find( gentity_t *from, int fieldofs, unsigned short match );
void G_InitGentity( gentity_t *ent );
void G_PrintEntities();
bool G_MaySpawnEntity( gentity_t *e );
gentity_t* G_Spawn( void );
gentity_t* G_SpawnPlayerClone();
void G_FreeEntityRefs( gentity_t *ent );
int G_GetPlayerCorpseIndex( gentity_t *ent );
void PlayerCorpse_Free( gentity_t *ent );
void G_FreeEntity( gentity_t *ed );
void G_FreeEntityDelay( gentity_t *ed );
gentity_t *G_TempEntity( const vec3_t origin, int event );
void G_KillBox( gentity_t *ent );
void G_AddPredictableEvent( gentity_t *ent, int event, int eventParm );
void G_AddEvent( gentity_t *ent, int event, int eventParm );
void G_PlaySoundAliasAtPoint( vec3_t origin, unsigned char alias );
void G_PlaySoundAlias( gentity_t *ent, unsigned char alias );
void G_PlaySoundAliasAsMaster( gentity_t *ent, unsigned char alias );
int G_AnimScriptSound( int client, snd_alias_list_t *aliasList );
void G_SetOrigin( gentity_t *ent, const vec3_t origin );
void G_SetAngle( gentity_t *ent, const vec3_t angle );
qboolean infront( gentity_t *self, gentity_t *other );
void G_SetConstString( unsigned short *to, const char *from );

/*
================
G_FindConfigstringIndex
================
*/
int G_FindConfigstringIndex( const char *name, int start, int max, qboolean create, const char *errormsg )
{
	int i;
	const char *s;

	if ( !name || !name[0] )
	{
		return 0;
	}

	for ( i = 1 ; i < max ; i++ )
	{
		s = SV_GetConfigstringConst( start + i );
		if ( !s[0] )
		{
			break;
		}
		if ( !strcasecmp( s, name ) )
		{
			return i;
		}
	}

	if ( !create )
	{
		if ( errormsg )
		{
			Scr_Error(va("%s \"%s\" not precached", errormsg, name));
		}

		return 0;
	}

	if ( i == max )
	{
		Com_Error(ERR_DROP, va("\x15" "G_FindConfigstringIndex: overflow (%d): '%s'", start, name));
	}

	SV_SetConfigstring( start + i, name );

	return i;
}

/*
================
G_LocalizedStringIndex
================
*/
int G_LocalizedStringIndex( const char *string )
{
	assert(string);
	if ( !string[0] )
	{
		return 0;
	}

	return G_FindConfigstringIndex( string, CS_LOCALIZED_STRINGS, MAX_LOCALIZED_STRINGS, level.initializing, "localized string" );
}

/*
================
G_ShaderIndex
================
*/
int G_ShaderIndex( const char *name )
{
	char s[MAX_QPATH];

	strcpy(s, name);
	I_strlwrInl(s);

	return G_FindConfigstringIndex( s, CS_SHADERS, MAX_SHADERS, level.initializing, "shader" );
}

/*
================
G_ModelIndex
================
*/
int G_ModelIndex( const char *name )
{
	int i;
	const char *s;

	if ( !name[0] )
	{
		return 0;
	}

	for ( i = 1; i < MAX_MODELS; i++ )
	{
		s = SV_GetConfigstringConst( CS_MODELS + i );

		if ( !s[0] )
		{
			break;
		}

		if ( !strcasecmp(s, name) )
		{
			return i;
		}
	}

	if ( !level.initializing )
	{
		Scr_Error(va("model '%s' not precached", name));
	}

	if ( i == MAX_MODELS )
	{
		Com_Error(ERR_DROP, "\x15" "G_ModelIndex: overflow");
	}

	cached_models[i] = SV_XModelGet(name);
	SV_SetConfigstring( CS_MODELS + i, name );

	return i;
}

/*
=================
G_GetModel
=================
*/
XModel* G_GetModel( int index )
{
	assert(index > 0 && index < MAX_MODELS);
	return cached_models[index];
}

/*
================
G_XModelBad
================
*/
int G_XModelBad( int index )
{
	assert(index);
	return XModelBad( G_GetModel(index) );
}

/*
===============
G_ModelName
===============
*/
const char* G_ModelName( int index )
{
	assert((unsigned)index < MAX_MODELS);
	return SV_GetConfigstringConst( CS_MODELS + index );
}

/*
================
G_TagIndex
================
*/
int G_TagIndex( const char *name )
{
	return G_FindConfigstringIndex( name, CS_TAGS, MAX_TAGS, qtrue, NULL );
}

/*
================
G_EffectIndex
================
*/
int G_EffectIndex( const char *name )
{
	return G_FindConfigstringIndex( name, CS_EFFECT_NAMES, MAX_EFFECT_NAMES, level.initializing, "effect" );
}

/*
================
G_ShellShockIndex
================
*/
int G_ShellShockIndex( const char *name )
{
	return G_FindConfigstringIndex( name, CS_SHELLSHOCKS, MAX_SHELLSHOCKS, qtrue, NULL );
}

/*
================
G_SoundAliasIndex
================
*/
int G_SoundAliasIndex( const char *name )
{
	return G_FindConfigstringIndex( name, CS_SOUND_ALIASES, MAX_SOUNDALIASES, qtrue, NULL );
}

/*
=================
G_DObjUpdate
=================
*/
void G_DObjUpdate( gentity_t *ent )
{
	DObjModel_s dobjModels[DOBJ_MAX_SUBMODELS];
	XModel *model;
	int numModels;
	int modelIndex;
	int i;

	if ( ent->client )
	{
		return;
	}

	G_SafeDObjFree(ent);

	modelIndex = ent->model;

	if ( !modelIndex )
	{
		G_UpdateTagInfoOfChildren(ent, qfalse);
		return;
	}

	model = G_GetModel(modelIndex);

	dobjModels->model = model;
	dobjModels->boneName = 0;
	dobjModels->ignoreCollision = 0;

	numModels = 1;

	if ( ent->s.eType == ET_GENERAL || ent->s.eType == ET_SCRIPTMOVER || ent->s.eType == ET_TURRET )
	{
		ent->s.index = modelIndex;
	}

	for ( i = 0; i < MAX_MODEL_ATTACHMENTS + 1; i++ )
	{
		modelIndex = ent->attachModelNames[i];

		if ( !modelIndex )
		{
			continue;
		}
		assert(numModels < DOBJ_MAX_SUBMODELS);

		dobjModels[numModels].model = G_GetModel(modelIndex);

		assert(dobjModels[numModels].model);
		assert(ent->attachTagNames[i]);

		dobjModels[numModels].boneName = SL_ConvertToString(ent->attachTagNames[i]);
		dobjModels[numModels].ignoreCollision = (ent->attachIgnoreCollision & (1 << i)) != 0;

		numModels++;
	}

	Com_ServerDObjCreate(dobjModels, numModels, NULL, ent->s.number);

	G_UpdateTagInfoOfChildren(ent, qtrue);
}

/*
================
G_SetModel
================
*/
void G_SetModel( gentity_t *ent, const char *modelName )
{
	int modelIndex;

	if ( !modelName[0] )
	{
		ent->model = 0;
		return;
	}

	modelIndex = G_ModelIndex( modelName );
	ent->model = modelIndex;
}

static __forceinline const char *G_ModelNameInl( int index )
{
	const char *cs = sv.configstrings[CS_MODELS + index];

	return cs ? cs : "";
}

/*
====================
G_OverrideModel
====================
*/
void G_OverrideModel( int modelIndex, const char *defaultModelName )
{
	assert(modelIndex);
	const char *modelName = G_ModelNameInl(modelIndex);
	assert(modelName[0]);
	XModel *model = SV_XModelGet(defaultModelName);

	cached_models[modelIndex] = model;
	Hunk_OverrideDataForFile(FILEDATA_XMODEL, modelName + 7, model);
}

/*
=================
G_EntAttach
=================
*/
qboolean G_EntAttach( gentity_t *ent, const char *modelName, unsigned int tagName, qboolean ignoreCollision )
{
	int i;
	int modelIndex;

	assert(tagName);
	assert(!G_EntDetach( ent, modelName, tagName ));

	for ( i = 0; i < MAX_MODEL_ATTACHMENTS + 1; i++ )
	{
		if ( ent->attachModelNames[i] )
		{
			continue;
		}

		assert(!ent->attachTagNames[i]);
		modelIndex = G_ModelIndex(modelName);
		ent->attachModelNames[i] = modelIndex;

		Scr_SetString(&ent->attachTagNames[i], tagName);
		assert(!(ent->attachIgnoreCollision & (1 << i)));

		if ( ignoreCollision )
		{
			ent->attachIgnoreCollision |= 1 << i;
		}

		G_DObjUpdate(ent);

		return qtrue;
	}

	return qfalse;
}

/*
=================
G_EntDetach
=================
*/
qboolean G_EntDetach( gentity_t *ent, const char *modelName, unsigned int tagName )
{
	int i;

	assert(tagName);

	for ( i = 0; i < MAX_MODEL_ATTACHMENTS + 1; i++ )
	{
		if ( ent->attachTagNames[i] != tagName )
		{
			continue;
		}
		else
		{
			if ( strcasecmp(G_ModelName(ent->attachModelNames[i]), modelName) )
			{
				continue;
			}

			ent->attachModelNames[i] = 0;
			Scr_SetString(&ent->attachTagNames[i], 0);

	for ( ; i < MAX_MODEL_ATTACHMENTS; i++ )
	{
		ent->attachModelNames[i] = ent->attachModelNames[ i + 1 ];
		ent->attachTagNames[i] = ent->attachTagNames[ i + 1 ];

		if ( ent->attachIgnoreCollision & ( 1 << ( i + 1 ) ) )
		{
			ent->attachIgnoreCollision = ent->attachIgnoreCollision | ( 1 << i );
		}
		else
		{
			ent->attachIgnoreCollision = ent->attachIgnoreCollision & ~( 1 << i );
		}
	}

	ent->attachModelNames[i] = 0;
	ent->attachTagNames[i] = 0;

	ent->attachIgnoreCollision &= ~( 1 << i );

			G_DObjUpdate(ent);

			return qtrue;
		}
	}

	return qfalse;
}

/*
=================
G_EntDetachAll
=================
*/
void G_EntDetachAll( gentity_t *ent )
{
	for ( int i = 0; i < MAX_MODEL_ATTACHMENTS + 1; i++ )
	{
		ent->attachModelNames[i] = 0;
		Scr_SetString(&ent->attachTagNames[i], 0);
	}

	ent->attachIgnoreCollision = 0;
	G_DObjUpdate(ent);
}

qboolean G_EntLinkToInternal( gentity_t *ent, gentity_t *parent, unsigned int tagName )
{
	tagInfo_s *tagInfo;
	gentity_t *checkEnt;
	int index;

	G_EntUnlink(ent);

	if ( tagName )
	{
		if ( !SV_DObjExists(parent) )
		{
			return qfalse;
		}

		index = SV_DObjGetBoneIndex(parent, tagName);

		if ( index < 0 )
		{
			return qfalse;
		}
	}
	else
	{
		index = -1;
	}

	for ( checkEnt = parent; ; checkEnt = checkEnt->tagInfo->parent )
	{
		if ( checkEnt == ent )
		{
			return qfalse;
		}

		if ( !checkEnt->tagInfo )
		{
			break;
		}
	}

	tagInfo = (tagInfo_s *)MT_Alloc(sizeof(*tagInfo), 16);

	tagInfo->parent = parent;
	tagInfo->name = 0;

	Scr_SetString(&tagInfo->name, tagName);

	tagInfo->next = parent->tagChildren;
	tagInfo->index = index;

	memset(tagInfo->axis, 0, sizeof(tagInfo->axis));

	parent->tagChildren = ent;
	ent->tagInfo = tagInfo;

	memset(tagInfo->parentInvAxis, 0, sizeof(tagInfo->parentInvAxis));

	return qtrue;
}

/*
=================
G_EntLinkTo
=================
*/
qboolean G_EntLinkTo( gentity_t *ent, gentity_t *parent, unsigned int tagName )
{
	if ( !G_EntLinkToInternal(ent, parent, tagName) )
	{
		return qfalse;
	}

	G_CalcTagAxis(ent, qfalse);
	return qtrue;
}

/*
====================
G_EntLinkToWithOffset
====================
*/
qboolean G_EntLinkToWithOffset( gentity_t *ent, gentity_t *parent, unsigned int tagName, const vec3_t originOffset, const vec3_t anglesOffset )
{
	if ( !G_EntLinkToInternal(ent, parent, tagName) )
	{
		return qfalse;
	}

	tagInfo_s *tagInfo = ent->tagInfo;

	AnglesToAxis(anglesOffset, tagInfo->axis);
	VectorCopy(originOffset, tagInfo->axis[3]);

	return qtrue;
}

/*
====================
G_EntUnlink
====================
*/
void G_EntUnlink( gentity_t *ent )
{
	tagInfo_s *tagInfo;
	gentity_t *parent;
	gentity_t *next;
	gentity_t *prev;
	vec3_t viewAngles;

	tagInfo = ent->tagInfo;

	if ( !tagInfo )
	{
		return;
	}

	G_SetOrigin(ent, ent->r.currentOrigin);
	G_SetAngle(ent, ent->r.currentAngles);

	if ( ent->client )
	{
		VectorCopy(ent->client->ps.viewangles, viewAngles);
		viewAngles[2] = 0;

		SetClientViewAngle(ent, viewAngles);
	}

	parent = tagInfo->parent;
	assert(parent);
	prev = NULL;
	next = parent->tagChildren;

	while ( next != ent )
	{
		assert(next->tagInfo);
		prev = next;
		next = next->tagInfo->next;
		assert(next);
	}

	if ( prev )
		prev->tagInfo->next = tagInfo->next;
	else
		parent->tagChildren = tagInfo->next;

	ent->tagInfo = NULL;

	Scr_SetString(&tagInfo->name, 0);
	MT_Free(tagInfo, sizeof(*tagInfo));
}

/*
===============
G_EntIsLinkedTo
===============
*/
qboolean G_EntIsLinkedTo( gentity_t *ent, gentity_t *parent )
{
	return ent->tagInfo && ent->tagInfo->parent == parent;
}

/*
====================
G_UpdateTagInfo
====================
*/
void G_UpdateTagInfo( gentity_t *parent, qboolean bParentHasDObj )
{
	tagInfo_s *tagInfo = parent->tagInfo;
	assert(tagInfo);

	if ( tagInfo->name )
	{
		if ( bParentHasDObj )
		{
			tagInfo->index = SV_DObjGetBoneIndex(tagInfo->parent, tagInfo->name);

			if ( tagInfo->index >= 0 )
			{
				return;
			}
		}

		G_EntUnlink(parent);
	}
	else
	{
		tagInfo->index = -1;
	}
}

/*
====================
G_UpdateTagInfoOfChildren
====================
*/
void G_UpdateTagInfoOfChildren( gentity_t *parent, qboolean bHasDObj )
{
	gentity_s *ent, *next;

	for ( ent = parent->tagChildren; ent; ent = next )
	{
		next = ent->tagInfo->next;
		G_UpdateTagInfo(ent, bHasDObj);
	}
}

void G_CalcTagParentAxis( gentity_t *ent, float parentAxis[4][3] )
{
	tagInfo_s *tagInfo;
	gentity_t *parent;
	float tempAxis[4][3];
	float axis[3][3];
	DObjAnimMat *mat;

	tagInfo = ent->tagInfo;
	parent = tagInfo->parent;

	if ( tagInfo->index >= 0 )
	{
		AnglesToAxis(parent->r.currentAngles, tempAxis);
		VectorCopy(parent->r.currentOrigin, tempAxis[3]);

		G_DObjCalcBone(parent, tagInfo->index);

		mat = &SV_DObjGetMatrixArray(parent)[tagInfo->index];

		ConvertQuatToMat(mat, axis);

		MatrixMultiply(axis, tempAxis, parentAxis);
		MatrixTransformVector43(mat->trans, tempAxis, parentAxis[3]);
	}
	else
	{
		AnglesToAxis(parent->r.currentAngles, parentAxis);
		VectorCopy(parent->r.currentOrigin, parentAxis[3]);
	}
}

/*
=================
G_CalcTagParentRelAxis
=================
*/
void G_CalcTagParentRelAxis( gentity_t *ent, float parentAxis[4][3] )
{
	float invParentAxis[4][3];
	tagInfo_s *tagInfo;

	tagInfo = ent->tagInfo;
	assert(tagInfo);

	G_CalcTagParentAxis(ent, invParentAxis);
	MatrixMultiply43(tagInfo->parentInvAxis, invParentAxis, parentAxis);
}

void G_CalcTagAxis( gentity_t *ent, qboolean bAnglesOnly )
{
	tagInfo_s *tagInfo;
	float axis[4][3];
	float parentAxis[4][3];
	float invParentAxis[4][3];
	G_CalcTagParentAxis(ent, parentAxis);
	AnglesToAxis(ent->r.currentAngles, axis);

	tagInfo = ent->tagInfo;
	assert(tagInfo);

	if ( bAnglesOnly )
	{
		MatrixTranspose(parentAxis, invParentAxis);
		MatrixMultiply(axis, invParentAxis, tagInfo->axis);
	}
	else
	{
		MatrixInverseOrthogonal43(parentAxis, invParentAxis);
		VectorCopy(ent->r.currentOrigin, axis[3]);
		MatrixMultiply43(axis, invParentAxis, tagInfo->axis);
	}
}

/*
=================
G_SetFixedLink
=================
*/
void G_SetFixedLink( gentity_t *ent, int eAngles )
{
	tagInfo_s *tagInfo;
	float axis[4][3];
	float parentAxis[4][3];

	G_CalcTagParentAxis(ent, parentAxis);

	tagInfo = ent->tagInfo;
	assert(tagInfo);

	switch ( eAngles )
	{
	case FIXED_LINK_ANGLES:
		MatrixMultiply43(tagInfo->axis, parentAxis, axis);
		VectorCopy(axis[3], ent->r.currentOrigin);
		AxisToAngles(axis, ent->r.currentAngles);
		break;

	case FIXED_LINK_ANGLES_YAW_ONLY:
		MatrixMultiply43(tagInfo->axis, parentAxis, axis);
		VectorCopy(axis[3], ent->r.currentOrigin);
		ent->r.currentAngles[1] = vectoyaw(axis[0]);
		break;

	case FIXED_LINK_ANGLES_NONE:
		MatrixTransformVector43(tagInfo->axis[3], parentAxis, axis[3]);
		VectorCopy(axis[3], ent->r.currentOrigin);
		break;
	}
}

/*
=================
G_GeneralLink
=================
*/
void G_GeneralLink( gentity_t *ent )
{
	assert(ent->tagInfo);
	G_SetFixedLink(ent, FIXED_LINK_ANGLES);

	G_SetOrigin(ent, ent->r.currentOrigin);
	G_SetAngle(ent, ent->r.currentAngles);

	ent->s.pos.trType = TR_INTERPOLATE;
	ent->s.apos.trType = TR_INTERPOLATE;

	SV_LinkEntity(ent);
}

/*
====================
G_SafeDObjFree
====================
*/
void G_SafeDObjFree( gentity_t *ent )
{
	Com_SafeServerDObjFree(ent->s.number);
}

/*
=================
G_DObjUpdateServerTime
=================
*/
int G_DObjUpdateServerTime( gentity_t *ent, qboolean bNotify )
{
	return SV_DObjUpdateServerTime(ent, level.frameTime * 0.001f, bNotify);
}

/*
=================
G_DObjCalcPose
=================
*/
void G_DObjCalcPose( gentity_t *ent )
{
	void (*controller)(gentity_t *, int *);
	int partBits[DOBJ_PART_BITS];

	memset(partBits, UCHAR_MAX, sizeof(partBits));

	if ( SV_DObjCreateSkelForBones(ent, partBits) )
	{
		return;
	}

	SV_DObjCalcAnim(ent, partBits);

	controller = entityHandlers[ent->handler].controller;

	if ( controller )
	{
		controller(ent, partBits);
	}

	SV_DObjCalcSkel(ent, partBits);
}

/*
=================
G_DObjCalcBone
=================
*/
void G_DObjCalcBone( gentity_t *ent, int boneIndex )
{
	void (*controller)(gentity_t *, int *);
	int partBits[DOBJ_PART_BITS];

	if ( SV_DObjCreateSkelForBone(ent, boneIndex) )
	{
		return;
	}

	SV_DObjGetHierarchyBits(ent, boneIndex, partBits);
	SV_DObjCalcAnim(ent, partBits);

	controller = entityHandlers[ent->handler].controller;

	if ( controller )
	{
		controller(ent, partBits);
	}

	SV_DObjCalcSkel(ent, partBits);
}

/*
=================
G_DObjGetLocalTagMatrix
=================
*/
DObjAnimMat* G_DObjGetLocalTagMatrix( gentity_t *ent, unsigned int tagName )
{
	DObjAnimMat *mat;
	int boneIndex;

	boneIndex = SV_DObjGetBoneIndex(ent, tagName);

	if ( boneIndex < 0 )
	{
		return NULL;
	}

	G_DObjCalcBone(ent, boneIndex);
	mat = SV_DObjGetMatrixArray(ent);

	return &mat[boneIndex];
}

/*
=================
G_DObjGetWorldTagMatrix
=================
*/
qboolean G_DObjGetWorldTagMatrix( gentity_t *ent, unsigned int tagName, float tagMat[4][3] )
{
	DObjAnimMat *mat;
	float ent_axis[4][3];
	float axis[3][3];

	mat = G_DObjGetLocalTagMatrix(ent, tagName);

	if ( !mat )
	{
		return qfalse;
	}

	AnglesToAxis(ent->r.currentAngles, ent_axis);
	VectorCopy(ent->r.currentOrigin, ent_axis[3]);

	ConvertQuatToMat(mat, axis);

	MatrixMultiply(axis, ent_axis, tagMat);
	MatrixTransformVector43(mat->trans, ent_axis, tagMat[3]);

	return qtrue;
}

/*
=================
G_DObjGetWorldTagPos
=================
*/
qboolean G_DObjGetWorldTagPos( gentity_t *ent, unsigned int tagName, vec3_t pos )
{
	DObjAnimMat *mat;
	float ent_axis[4][3];

	mat = G_DObjGetLocalTagMatrix(ent, tagName);

	if ( !mat )
	{
		return qfalse;
	}

	AnglesToAxis(ent->r.currentAngles, ent_axis);
	VectorCopy(ent->r.currentOrigin, ent_axis[3]);
	MatrixTransformVector43(mat->trans, ent_axis, pos);

	return qtrue;
}

/*
=============
G_Find

Searches all active entities for the next one that holds
the matching string at fieldofs (use the FOFS() macro) in the structure.

Searches beginning at the entity after from, or the beginning if NULL
NULL will be returned if the end of the list is reached.

=============
*/
gentity_t *G_Find( gentity_t *from, int fieldofs, unsigned short match )
{
	unsigned short s;

	if ( !from )
	{
		from = g_entities;
	}
	else
	{
		from++;
	}

	for ( ; from < &g_entities[level.num_entities]; from++ )
	{
		if ( !from->r.inuse )
		{
			continue;
		}
		s = *( unsigned short * )( (byte *)from + fieldofs );
		if ( !s )
		{
			continue;
		}
		if ( s == match )
		{
			return from;
		}
	}

	return NULL;
}

gentity_t *G_FindStr( gentity_t *from, int fieldofs, const char *match )
{
	char *s;

	if ( !from )
		from = g_entities;
	else
		from++;

	for ( ; from < &g_entities[level.num_entities]; from++ )
	{
		if ( !from->r.inuse )
			continue;
		s = *( char ** )( (byte *)from + fieldofs );
		if ( !s )
			continue;
		if ( !I_stricmp( s, match ) )
			return from;
	}

	return NULL;
}

gentity_t *G_PickTarget( unsigned short targetname )
{
	gentity_t *ent = NULL;
	int num_choices = 0;
	gentity_t *choice[MAXCHOICES];

	if ( !targetname )
	{
		return NULL;
	}

	for ( ;; )
	{
		ent = G_Find( ent, FOFS( targetname ), targetname );

		if ( !ent )
		{
			break;
		}

		choice[num_choices++] = ent;

		if ( num_choices == MAXCHOICES )
		{
			break;
		}
	}

	if ( !num_choices )
	{
		Com_Printf( "G_PickTarget: target %s not found\n", SL_ConvertToString( targetname ) );
		return NULL;
	}

	return choice[rand() % num_choices];
}

/*
====================
G_InitGentity
====================
*/
void G_InitGentity( gentity_t *ent )
{
	ent->nextFree = 0;
	ent->r.inuse = qtrue;

	Scr_SetString(&ent->classname, scr_const.noclass);

	ent->s.number = ent - g_entities;
	ent->r.ownerNum = ENTITYNUM_NONE;

	ent->eventTime = 0;
	ent->freeAfterEvent = qfalse;
}

/*
=================
G_PrintEntities
=================
*/
void G_PrintEntities()
{
	int i;
	const char *classname;

	for ( i = 0; i < level.num_entities; i++ )
	{
		if ( g_entities[i].classname )
			classname = SL_ConvertToString(g_entities[i].classname);
		else
			classname = "";

		Com_Printf(
		    "%4i: '%s', origin: %f %f %f\n",
		    i,
		    classname,
		    g_entities[i].r.currentOrigin[0],
		    g_entities[i].r.currentOrigin[1],
		    g_entities[i].r.currentOrigin[2]);
	}
}

/*
=================
G_MaySpawnEntity
=================
*/
bool G_MaySpawnEntity( gentity_t *e )
{
	if ( !e )
	{
		return false;
	}

	if ( level.time - e->eventTime < 500 && level.num_entities < ENTITYNUM_WORLD )
	{
		return false;
	}

	return true;
}

/*
=================
G_Spawn

Either finds a free entity, or allocates a new one.

  The slots from 0 to MAX_CLIENTS-1 are always reserved for clients, and will
never be used by anything else.

Try to avoid reusing an entity that was recently freed, because it
can cause the client to think the entity morphed into something else
instead of being removed and recreated, which can cause interpolated
angles and bad trails.
=================
*/
gentity_t* G_Spawn( void )
{
	gentity_t *e;

	e = level.firstFreeEnt;

	// if we go through all entities and can't find one to free,
	// override the normal minimum times before use
	if ( !G_MaySpawnEntity( e ) )
	{
		if ( level.num_entities == ENTITYNUM_WORLD )
		{
			G_PrintEntities();
			Com_Error(ERR_DROP, "\x15" "G_Spawn: no free entities");
		}

		e = &level.gentities[level.num_entities];

		// open up a new slot
		level.num_entities++;

		// let the server system know that there are more entities
		SV_LocateGameData( level.gentities, level.num_entities, sizeof( gentity_t ),
		                   &level.clients[0].ps, sizeof( level.clients[0] ) );
	}
	else
	{
		level.firstFreeEnt = level.firstFreeEnt->nextFree;

		if ( !level.firstFreeEnt )
		{
			level.lastFreeEnt = NULL;
		}

		e->nextFree = NULL;
		// reuse this slot
	}

	G_InitGentity( e );
	return e;
}

/*
=================
G_SpawnPlayerClone
=================
*/
gentity_t* G_SpawnPlayerClone()
{
	gentity_t *e;
	int flags;

	e = &level.gentities[level.currentPlayerClone + MAX_CLIENTS];
	level.currentPlayerClone = (level.currentPlayerClone + 1) % MAX_PLAYER_CORPSES;

	flags = e->s.eFlags & EF_TELEPORT_BIT;
	flags ^= EF_TELEPORT_BIT;

	if ( e->r.inuse )
	{
		G_FreeEntity(e);
	}

	G_InitGentity(e);
	e->s.eFlags = flags;

	return e;
}

/*
===============
G_FreeEntityRefs
===============
*/
void G_FreeEntityRefs( gentity_t *ent )
{
	gentity_t *other;
	int i;
	int entnum;
	gclient_t *pClient;

	entnum = ent->s.number;

	for ( i = 0; i < level.num_entities; i++ )
	{
		other = &g_entities[i];

		if ( !other->r.inuse )
		{
			continue;
		}

		if ( other->parent == ent )
		{
			other->parent = NULL;
		}

		if ( other->r.ownerNum == entnum )
		{
			other->r.ownerNum = ENTITYNUM_NONE;

			if ( other->s.eType == ET_TURRET )
			{
				other->active = qfalse;
			}
		}

		if ( other->s.groundEntityNum == entnum )
		{
			other->s.groundEntityNum = ENTITYNUM_NONE;
		}
	}

	for ( i = 0; i < MAX_CLIENTS; i++ )
	{
		other = &g_entities[i];

		if ( !other->r.inuse )
		{
			continue;
		}

		pClient = other->client;

		if ( pClient->pLookatEnt == ent )
		{
			pClient->pLookatEnt = NULL;
		}

		if ( pClient->useHoldEntity == entnum )
		{
			pClient->useHoldEntity = ENTITYNUM_NONE;
		}

		if ( pClient->ps.cursorHintEntIndex == entnum )
		{
			pClient->ps.cursorHintEntIndex = ENTITYNUM_NONE;
		}
	}

	for ( i = 0; i < MAX_DROPPED_WEAPONS; i++ )
	{
		if ( level.droppedWeaponCue[i] == ent )
		{
			level.droppedWeaponCue[i] = NULL;
		}
	}
}

/*
===============
G_GetPlayerCorpseIndex
===============
*/
int G_GetPlayerCorpseIndex( gentity_t *ent )
{
	int num;
	int i;

	num = ent->s.number;
	for ( i = 0; i < 8; ++i )
	{
		if ( (&g_scr_data.playerCorpseInfo[0].entnum)[306 * i] == num )
		{
			return i;
		}
	}

	return 0;
}

int G_GetFreePlayerCorpseIndex()
{
	int i;
	gentity_t *ent;
	float vDistSq;
	float bestDistSq;
	vec3_t playerPos;
	int bestIndex;

	bestDistSq = -1.0;
	bestIndex = 0;

	ent = G_Find( NULL, FOFS( classname ), scr_const.player );

	VectorCopy(ent->s.pos.trBase, playerPos);

	for ( i = 0; i < MAX_PLAYER_CORPSES; i++ )
	{
		if ( g_scr_data.playerCorpseInfo[i].entnum == -1 )
		{
			return i;
		}

		ent = &level.gentities[g_scr_data.playerCorpseInfo[i].entnum];
		vDistSq = Vec3DistanceSq(ent->r.currentOrigin, playerPos);

		if ( bestDistSq < vDistSq )
		{
			bestDistSq = vDistSq;
			bestIndex = i;
		}
	}

	ent = &level.gentities[g_scr_data.playerCorpseInfo[bestIndex].entnum];

	G_FreeEntity(ent);
	g_scr_data.playerCorpseInfo[bestIndex].entnum = -1;

	return bestIndex;
}

/*
================
PlayerCorpse_Free
================
*/
void PlayerCorpse_Free( gentity_t *ent )
{
	int index;

	index = G_GetPlayerCorpseIndex(ent);
	g_scr_data.playerCorpseInfo[index].entnum = -1;
}

/*
=================
G_FreeEntity

Marks the entity as free
=================
*/
void G_FreeEntity( gentity_t *ed )
{
	int useCount;
	XAnimTree *tree;

	G_EntUnlink(ed);

	while ( ed->tagChildren )
	{
		G_EntUnlink(ed->tagChildren);
	}

	SV_UnlinkEntity(ed); // unlink from world

	tree = SV_DObjGetTree(ed);

	if ( tree )
	{
		XAnimClearTree(tree);
	}

	Com_SafeServerDObjFree(ed->s.number);
	G_FreeEntityRefs(ed);

	if ( ed->pTurretInfo )
	{
		assert(ed->pTurretInfo->inuse);
		G_FreeTurret(ed);
		assert(ed->pTurretInfo == NULL);
	}

	if ( ed->s.eType == ET_PLAYER_CORPSE )
	{
		PlayerCorpse_Free(ed);
	}

	assert(ed->r.inuse);
	Scr_FreeEntity(ed);
	assert(ed->classname == 0);
	useCount = ed->useCount;

	memset( ed, 0, sizeof( *ed ) );
	ed->eventTime = level.time;

	if ( ed - level.gentities >= MAX_CLIENTS + MAX_PLAYER_CORPSES )
	{
		if ( level.lastFreeEnt )
		{
			level.lastFreeEnt->nextFree = ed;
		}
		else
		{
			level.firstFreeEnt = ed;
		}

		level.lastFreeEnt = ed;
		level.lastFreeEnt->nextFree = NULL;
	}

	ed->useCount = useCount + 1;
	assert(!ed->r.inuse);
}

/*
=================
G_FreeEntityDelay
=================
*/
void G_FreeEntityDelay( gentity_t *ed )
{
	assert(g_scr_data.deletestruct);
	unsigned short hThread = Scr_ExecEntThread(ed, g_scr_data.deletestruct, 0);
	Scr_FreeThread(hThread);
}

/*
=================
G_TempEntity

Spawns an event entity that will be auto-removed
The origin will be snapped to save net bandwidth, so care
must be taken if the origin is right on a surface (snap towards start vector first)
=================
*/
gentity_t *G_TempEntity( const vec3_t origin, int event )
{
	gentity_t       *e;
	vec3_t snapped;

	e = G_Spawn();
	e->s.eType = ET_EVENTS + event;

	Scr_SetString(&e->classname, scr_const.tempEntity);
	e->eventTime = level.time;
	e->r.eventTime = level.time;
	e->freeAfterEvent = qtrue;

	VectorCopy( origin, snapped );
	SnapVector( snapped );      // save network bandwidth
	G_SetOrigin( e, snapped );

	// find cluster for PVS
	SV_LinkEntity( e );

	return e;
}

/*
=================
G_KillBox

Kills all entities that would touch the proposed new positioning
of ent.  Ent should be unlinked before calling this!
=================
*/
void G_KillBox( gentity_t *ent )
{
	int i, num;
	int touch[MAX_GENTITIES];
	gentity_t   *hit;
	vec3_t mins, maxs;

	VectorAdd( ent->client->ps.origin, ent->r.mins, mins );
	VectorAdd( ent->client->ps.origin, ent->r.maxs, maxs );
	num = CM_AreaEntities( mins, maxs, touch, MAX_GENTITIES, CONTENTS_BODY );

	for ( i = 0 ; i < num ; i++ )
	{
		hit = &g_entities[touch[i]];
		if ( !hit->client )
		{
			continue;
		}
		if ( !hit->r.linked )   // RF, inactive AI shouldn't be gibbed
		{
			continue;
		}

		// nail it
		G_Damage( hit, ent, ent, NULL, NULL,
		          100000, DAMAGE_NO_PROTECTION, MOD_TELEFRAG,
		          HITLOC_NONE, 0				  );
	}

}

/*
===============
G_AddPredictableEvent

Use for non-pmove events that would also be predicted on the
client side: jumppads and item pickups
Adds an event+parm and twiddles the event counter
===============
*/
void G_AddPredictableEvent( gentity_t *ent, int event, int eventParm )
{
	if ( !ent->client )
	{
		return;
	}
	BG_AddPredictableEventToPlayerstate( event, eventParm, &ent->client->ps );
}

/*
===============
G_AddEvent

Adds an event+parm and twiddles the event counter
===============
*/
void G_AddEvent( gentity_t *ent, int event, int eventParm )
{
//	int		bits;

	assert(event);
	assert(event >= 0 && event < UCHAR_MAX);
	assert(eventParm >= 0 && eventParm < EVENT_PARM_MAX);
	assert(ent->s.eType < ET_EVENTS);

	// Ridah, use the sequential event list
	if ( ent->client )
	{
		// NERVE - SMF - commented in - externalEvents not being handled properly in Wolf right now
		ent->client->ps.events[ent->client->ps.eventSequence & ( MAX_EVENTS - 1 )] = event;
		ent->client->ps.eventParms[ent->client->ps.eventSequence & ( MAX_EVENTS - 1 )] = eventParm;
		ent->client->ps.eventSequence++;
		// -NERVE - SMF
	}
	else
	{
		// NERVE - SMF - commented in - externalEvents not being handled properly in Wolf right now
		ent->s.events[ent->s.eventSequence & ( MAX_EVENTS - 1 )] = event;
		ent->s.eventParms[ent->s.eventSequence & ( MAX_EVENTS - 1 )] = eventParm;
		ent->s.eventSequence++;
		// -NERVE - SMF
	}
	ent->eventTime = level.time;
	ent->r.eventTime = level.time;
}

/*
=================
G_PlaySoundAliasAtPoint
=================
*/
void G_PlaySoundAliasAtPoint( vec3_t origin, unsigned char alias )
{
	gentity_t *ent;

	if ( alias )
	{
		ent = G_TempEntity(origin, EV_SOUND_ALIAS);
		ent->s.eventParm = alias;
	}
}

/*
================
G_PlaySoundAlias
================
*/
void G_PlaySoundAlias( gentity_t *ent, unsigned char alias )
{
	if ( alias )
	{
		G_AddEvent(ent, EV_SOUND_ALIAS, alias);
	}
}

/*
================
G_PlaySoundAliasAsMaster
================
*/
void G_PlaySoundAliasAsMaster( gentity_t *ent, unsigned char alias )
{
	if ( alias )
	{
		G_AddEvent(ent, EV_SOUND_ALIAS_AS_MASTER, alias);
	}
}

/*
================
G_AnimScriptSound
================
*/
int G_AnimScriptSound( int client, snd_alias_list_t *aliasList )
{
	G_PlaySoundAlias( &g_entities[client], G_SoundAliasIndex(aliasList->aliasName) );
	return 0;
}

/*
================
G_SetOrigin

Sets the pos trajectory for a fixed position
================
*/
void G_SetOrigin( gentity_t *ent, const vec3_t origin )
{
	VectorCopy( origin, ent->s.pos.trBase );
	ent->s.pos.trType = TR_STATIONARY;
	ent->s.pos.trTime = 0;
	ent->s.pos.trDuration = 0;
	VectorClear( ent->s.pos.trDelta );

	VectorCopy( origin, ent->r.currentOrigin );
}

/*
==============
G_SetAngle
==============
*/
void G_SetAngle( gentity_t *ent, const vec3_t angle )
{
	VectorCopy( angle, ent->s.apos.trBase );
	ent->s.apos.trType = TR_STATIONARY;
	ent->s.apos.trTime = 0;
	ent->s.apos.trDuration = 0;
	VectorClear( ent->s.apos.trDelta );

	VectorCopy( angle, ent->r.currentAngles );

//	VectorCopy (ent->s.angles, ent->s.apos.trDelta );
}

/*
====================
infront
====================
*/
qboolean infront( gentity_t *self, gentity_t *other )
{
	vec3_t vec;
	float dot;
	vec3_t forward;

	AngleVectors( self->r.currentAngles, forward, NULL, NULL );
	VectorSubtract( other->r.currentOrigin, self->r.currentOrigin, vec );
	Vec3Normalize( vec );
	dot = DotProduct( vec, forward );
	// G_Printf( "other %5.2f\n",	dot);
	if ( dot > 0.0 )
	{
		return qtrue;
	}
	return qfalse;
}

// unreferenced; its name is not known
int G_Unused( void )
{
	return 0;
}

/*
====================
G_SetConstString
====================
*/
void G_SetConstString( unsigned short *to, const char *from )
{
	Scr_SetString(to, 0);
	*to = SL_GetString(from, 0);
}

