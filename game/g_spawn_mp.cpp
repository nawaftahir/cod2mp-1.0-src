#include "../qcommon/qcommon.h"
#include "g_shared.h"
#include "../script/script_public.h"

static const game_entity_field_t g_entity_fields[] =
{
	{ "classname", FOFS( classname ), F_STRING, Scr_ReadOnlyField },
	{ "origin", FOFS( r.currentOrigin ), F_VECTOR, Scr_SetOrigin },
	{ "model", FOFS( model ), F_MODEL, Scr_ReadOnlyField },
	{ "spawnflags", FOFS( spawnflags ), F_INT, Scr_ReadOnlyField },
	{ "target", FOFS( target ), F_STRING, NULL },
	{ "targetname", FOFS( targetname ), F_STRING, NULL },
	{ "count", FOFS( count ), F_INT, NULL },
	{ "health", FOFS( health ), F_INT, Scr_SetHealth },
	{ "dmg", FOFS( dmg ), F_INT, NULL },
	{ "angles", FOFS( r.currentAngles ), F_VECTOR, Scr_SetAngles },
	{ NULL, 0, F_INT, NULL } // field terminator
};

/*
==============
G_GetItemForClassname
==============
*/
// re-declared so gcc does not treat strcmp as a builtin (changes argument staging order)
int strcmp( const char *, const char * ) throw();

void Scr_SetGenericField( byte *b, int type, int ofs );
void Scr_GetGenericField( byte *b, int type, int ofs );
void Scr_AddEntity( gentity_t *ent );
gentity_t* Scr_GetEntity( unsigned int index );
void Scr_GetEnt();
void GScr_SetDynamicEntityField( gentity_t *ent, unsigned int index );

qboolean G_SpawnString( const char *key, const char *defaultString, const char **out );
qboolean G_SpawnFloat( const char *key, const char *defaultString, float *out );
qboolean G_SpawnInt( const char *key, const char *defaultString, int *out );
qboolean G_SpawnVector( const char *key, const char *defaultString, float *out );
void Scr_ReadOnlyField( gentity_t *self, int offset );
unsigned int G_SetEntityScriptVariableInternal( const char *key, const char *value );
void G_SetEntityScriptVariable( const char *key, const char *value, gentity_t *ent );
void G_ParseEntityFields( gentity_t *ent );
void G_SpawnStruct();
void G_DuplicateScriptFields( gentity_t *dest, const gentity_t *source );
void G_CallSpawn(void);
qboolean G_CallSpawnEntity( gentity_t *ent );
void GScr_AddFieldsForEntity();
void GScr_AddFieldsForRadiant();
int Scr_SetEntityField( int entnum, int offset );
void Scr_SetGenericField( byte *b, int type, int ofs );
int Scr_SetObjectField( unsigned int classnum, int entnum, int offset );
void Scr_GetGenericField( byte *b, int type, int ofs );
void Scr_GetObjectField( unsigned int classnum, int entnum, int offset );
void Scr_FreeEntityConstStrings( gentity_t *ent );
void Scr_FreeEntity( gentity_t *ent );
void Scr_AddEntity( gentity_t *ent );
gentity_t* Scr_GetEntity( unsigned int index );
void Scr_FreeHudElem( game_hudelem_t *hud );
void Scr_AddHudElem( game_hudelem_t *hud );
game_hudelem_t* Scr_GetHudElem( unsigned int index );
unsigned short Scr_ExecEntThread( gentity_t *ent, int handle, unsigned int paramcount );
void Scr_AddExecEntThread( gentity_t *ent, int handle, unsigned int paramcount );
void Scr_Notify( gentity_t *ent, unsigned short stringValue, unsigned int paramcount );
void Scr_GetEnt();
void Scr_GetEntArray();
void GScr_SetDynamicEntityField( gentity_t *ent, unsigned int index );
void G_SpawnEntitiesFromString( void );
void G_LoadStructs();

spawn_t spawns[] =
{
	{ "info_null", SP_info_null },
	{ "info_notnull", SP_info_notnull },
	{ "func_group", SP_info_null },
	{ "trigger_multiple", SP_trigger_multiple },
	{ "trigger_radius", SP_trigger_radius },
	{ "trigger_disk", SP_trigger_disk },
	{ "trigger_hurt", SP_trigger_hurt },
	{ "trigger_once", SP_trigger_once },
	{ "light", SP_light },
	{ "misc_model", SP_misc_model },
	{ "misc_mg42", SP_turret },
	{ "misc_turret", SP_turret },
	{ "corona", SP_corona },
	{ "trigger_use", trigger_use },
	{ "trigger_use_touch", trigger_use_touch },
	{ "trigger_damage", SP_trigger_damage },
	{ "trigger_lookat", SP_trigger_lookat },
	{ "script_brushmodel", SP_script_brushmodel },
	{ "script_model", SP_script_model },
	{ "script_origin", SP_script_origin },
	{ "script_struct", G_FreeEntity },
	{ NULL, NULL },
};
#define NUMSPAWNS   ( sizeof( spawns ) / sizeof( spawn_t ) )

/*
===============
G_SpawnString
===============
*/
qboolean G_SpawnString( const char *key, const char *defaultString, const char **out )
{
	return G_SpawnStringInternal(&level.spawnVar, key, defaultString, out);
}

/*
===============
G_SpawnFloat
===============
*/
qboolean G_SpawnFloat( const char *key, const char *defaultString, float *out )
{
	const char *s;
	qboolean present;

	present = G_SpawnString( key, defaultString, &s );
	*out = atof( s );
	return present;
}

/*
===============
G_SpawnInt
===============
*/
qboolean G_SpawnInt( const char *key, const char *defaultString, int *out )
{
	const char *s;
	qboolean present;

	present = G_SpawnString( key, defaultString, &s );
	*out = atoi( s );
	return present;
}

/*
===============
G_SpawnVector
===============
*/
qboolean G_SpawnVector( const char *key, const char *defaultString, float *out )
{
	const char *s;
	qboolean present;

	present = G_SpawnString( key, defaultString, &s );
	VectorClear( out );
	sscanf( s, "%f %f %f", &out[0], &out[1], &out[2] );
	return present;
}

/*
==============
Scr_ReadOnlyField
==============
*/
void Scr_ReadOnlyField( gentity_t *self, int offset )
{
	Scr_Error("Tried to set a read only entity field");
}

/*
==============
G_SetEntityScriptVariableInternal
==============
*/
unsigned int G_SetEntityScriptVariableInternal( const char *key, const char *value )
{
	int type;
	unsigned int index;
	vec3_t vec;

	index = Scr_FindField(key, &type);

	if ( !index )
	{
		return 0;
	}

	switch ( type )
	{
	case VAR_STRING:
		Scr_AddString(value);
		break;

	case VAR_FLOAT:
		Scr_AddFloat(atof(value));
		break;

	case VAR_INTEGER:
		Scr_AddInt(atoi(value));
		break;

	case VAR_VECTOR:
		VectorClear(vec);
		sscanf(value, "%f %f %f", &vec[0], &vec[1], &vec[2]);
		Scr_AddVector(vec);
		break;

	default:
		assert(va("G_SetEntityScriptVariableInternal: bad case %d", type));
		break;
	}

	return index;
}

/*
==============
G_SetEntityScriptVariable
==============
*/
void G_SetEntityScriptVariable( const char *key, const char *value, gentity_t *ent )
{
	unsigned int index = G_SetEntityScriptVariableInternal(key, value);

	if ( !index )
	{
		return;
	}

	GScr_SetDynamicEntityField(ent, index);
}

void G_ParseEntityField( const char *key, const char *value, gentity_t *ent )
{
	const game_entity_field_t *f;
	byte *b;
	vec3_t vec;
	int modelIndex;

	for ( f = g_entity_fields; f->name; f++ )
	{
		if ( !I_stricmp(f->name, key) )
		{
			b = (byte *)ent;

			switch ( f->type )
			{
			case F_STRING:
				Scr_SetString((unsigned short *)(b + f->ofs), 0);
				*(unsigned short *)(b + f->ofs) = G_NewString(value);
				break;

			case F_VECTOR:
				VectorClear(vec);
				sscanf(value, "%f %f %f", &vec[0], &vec[1], &vec[2]);
				((float *)(b + f->ofs))[0] = vec[0];
				((float *)(b + f->ofs))[1] = vec[1];
				((float *)(b + f->ofs))[2] = vec[2];
				break;

			case F_INT:
				*(int *)(b + f->ofs) = atoi(value);
				break;

			case F_FLOAT:
				*(float *)(b + f->ofs) = atof(value);
				break;

			case F_MODEL:
				if ( value[0] == '*' )
				{
					modelIndex = atoi(value + 1);
					ent->s.index = (unsigned short)modelIndex;
				}
				else
				{
					G_SetModel(ent, value);
				}
				break;
			}

			return;
		}
	}

	G_SetEntityScriptVariable(key, value, ent);
}

/*
===============
G_ParseEntityFields
===============
*/
void G_ParseEntityFields( gentity_t *ent )
{
	assert(level.spawnVar.spawnVarsValid);

	for ( int i = 0; i < level.spawnVar.numSpawnVars; i++ )
	{
		G_ParseEntityField(level.spawnVar.spawnVars[i][0], level.spawnVar.spawnVars[i][1], ent);
	}

	G_SetOrigin(ent, ent->r.currentOrigin);
	G_SetAngle(ent, ent->r.currentAngles);
}

/*
===============
G_SpawnStruct
===============
*/
void G_SpawnStruct()
{
	int i;
	unsigned int index;
	unsigned int structId;

	assert(level.spawnVar.spawnVarsValid);
	assert(g_scr_data.createstruct);

	Scr_AddExecThread(g_scr_data.createstruct, 0);
	structId = Scr_GetObject(0);

	for ( i = 0; i < level.spawnVar.numSpawnVars; i++ )
	{
		index = G_SetEntityScriptVariableInternal(level.spawnVar.spawnVars[i][0], level.spawnVar.spawnVars[i][1]);

		if ( !index )
		{
			continue;
		}

		Scr_SetStructField(structId, index);
	}
}

void G_DuplicateEntityFields( gentity_t *dest, const gentity_t *source )
{
	const game_entity_field_t *f;
	byte *d;
	const byte *s;

	d = (byte *)dest;
	s = (const byte *)source;

	for ( f = g_entity_fields; f->name; f++ )
	{
		switch ( f->type )
		{
		case F_STRING:
			Scr_SetString((unsigned short *)(d + f->ofs), *(unsigned short *)(s + f->ofs));
			break;

		case F_VECTOR:
			VectorCopy((float *)(s + f->ofs), (float *)(d + f->ofs));
			break;

		case F_INT:
			*(int *)(d + f->ofs) = *(int *)(s + f->ofs);
			break;

		case F_FLOAT:
			*(float *)(d + f->ofs) = *(float *)(s + f->ofs);
			break;

		case F_MODEL:
			*(byte *)(d + f->ofs) = *(byte *)(s + f->ofs);
			break;
		}
	}
}

/*
===============
G_DuplicateScriptFields
===============
*/
void G_DuplicateScriptFields( gentity_t *dest, const gentity_t *source )
{
	assert(dest->s.number == dest - g_entities);
	assert(source->s.number == source - g_entities);

	Scr_CopyEntityNum(source->s.number, dest->s.number, CLASS_NUM_ENTITY);
}

gitem_t* G_GetItemForClassname( const char *classname )
{
	gitem_t *item;
	int weapIndex;
	WeaponDef *weapDef;
	int iIndex;

	if ( !strncmp(classname, "weapon_", 7) )
	{
		weapIndex = G_GetWeaponIndexForName(classname + 7);

		if ( weapIndex != WP_NONE )
		{
			weapDef = BG_GetWeaponDef(weapIndex);
			return BG_FindItemForWeapon(weapIndex);
		}
	}

	for ( iIndex = 129; iIndex < bg_numItems; iIndex++ )
	{
		item = &bg_itemlist[iIndex];

		if ( !strcmp(item->classname, classname) )
		{
			return item;
		}
	}

	return NULL;
}

/*
===============
G_CallSpawn

Finds the spawn function for the entity and calls it,
returning qfalse if not found
===============
*/
void G_CallSpawn(void)
{
	spawn_t *s;
	gitem_t *item;
	gentity_t *ent;
	const char *classname;

	assert(level.spawnVar.spawnVarsValid);

	G_SpawnString("classname", "", &classname);

	if ( !classname )
	{
		Com_Printf("G_CallSpawn: NULL classname\n");
		return;
	}

	item = G_GetItemForClassname(classname);

	// check item spawn functions
	if ( item )
	{
		ent = G_Spawn();
		G_ParseEntityFields(ent);
		G_SpawnItem(ent, item);
		return;
	}

	// check normal spawn functions
	for ( s = spawns ; s->name ; s++ )
	{
		if ( !strcmp(s->name, classname) )
		{
			if ( s->spawn == G_FreeEntity )
			{
				return;
			}

			ent = G_Spawn();
			G_ParseEntityFields(ent);
			s->spawn(ent);
			return;
		}
	}

	// RF, entity scripting
	ent = G_Spawn();
	G_ParseEntityFields(ent);
}

/*
===============
G_CallSpawnEntity
===============
*/
qboolean G_CallSpawnEntity( gentity_t *ent )
{
	spawn_t *s;
	gitem_t *item;
	const char *classname;

	assert(!level.spawnVar.spawnVarsValid);

	if ( !ent->classname )
	{
		Com_Printf("G_CallSpawnEntity: NULL classname\n");
		return qfalse;
	}

	classname = SL_ConvertToString(ent->classname);
	item = G_GetItemForClassname(classname);

	// check item spawn functions
	if ( item )
	{
		G_SpawnItem(ent, item);
		return qtrue;
	}

	// check normal spawn functions
	for ( s = spawns; s->name; s++ )
	{
		if ( !strcmp(s->name, classname) )
		{
			// found it
			s->spawn(ent);
			assert(ent->r.inuse);
			return qtrue;
		}
	}

	Com_Printf("%s doesn't have a spawn function\n", SL_ConvertToString(ent->classname));

	return qfalse;
}

/*
===============
GScr_AddFieldsForEntity
===============
*/
void GScr_AddFieldsForEntity()
{
	const game_entity_field_t *f;

	for ( f = g_entity_fields; f->name; f++ )
	{
		assert(((f - g_entity_fields) & ENTFIELD_MASK) == ENTFIELD_ENTITY);
		assert((f - g_entity_fields) == (unsigned short)( f - g_entity_fields ));

		Scr_AddClassField(CLASS_NUM_ENTITY, f->name, (unsigned short)( f - g_entity_fields ));
	}

	GScr_AddFieldsForClient();
}

/*
===============
GScr_AddFieldsForRadiant
===============
*/
void GScr_AddFieldsForRadiant()
{
	Scr_AddFields("radiant", "txt");
}

/*
==============
Scr_SetEntityField
==============
*/
int Scr_SetEntityField( int entnum, int offset )
{
	const game_entity_field_t *f;
	gentity_t *ent;
	int result;

	assert((unsigned)entnum < MAX_GENTITIES);
	ent = &g_entities[entnum];
	assert(ent->r.inuse);

	if ( (offset & ENTFIELD_MASK) == ENTFIELD_CLIENT )
	{
		if ( !ent->client )
		{
			result = qfalse;
		}
		else
		{
			Scr_SetClientField(ent->client, offset & ~ENTFIELD_CLIENT);
			result = qtrue;
		}
	}
	else
	{
		assert((unsigned)offset < ARRAY_COUNT( g_entity_fields ) - 1);
		f = &g_entity_fields[offset];

		if ( f->callback )
		{
			f->callback(ent, offset);
			result = qtrue;
		}
		else
		{
			Scr_SetGenericField((byte *)ent, f->type, f->ofs);
			result = qtrue;
		}
	}

	return result;
}

/*
===============
Scr_SetGenericField
===============
*/
void Scr_SetGenericField( byte *b, int type, int ofs )
{
	vec3_t vec;

	switch ( type )
	{
	case F_STRING:
		Scr_SetString(( unsigned short * )( ofs + b ), Scr_GetConstStringIncludeNull(0));
		break;

	case F_VECTOR:
		Scr_GetVector(0, vec);
		( ( float * )( ofs + b ) )[0] = vec[0];
		( ( float * )( ofs + b ) )[1] = vec[1];
		( ( float * )( ofs + b ) )[2] = vec[2];
		break;

	case F_INT:
		*( int * )( ofs + b ) = Scr_GetInt(0);
		break;

	case F_FLOAT:
		*( float * )( ofs + b ) = Scr_GetFloat(0);
		break;

	case F_VECTORHACK:
		Scr_GetVector(0, vec);
		( ( float * )( ofs + b ) )[0] = vec[1];
		break;

	case F_ENTITY:
		*( gentity_t ** )( ofs + b ) = Scr_GetEntity(0);
		break;
	}
}

/*
===============
Scr_SetObjectField
===============
*/
int Scr_SetObjectField( unsigned int classnum, int entnum, int offset )
{
	switch ( classnum )
	{
	case CLASS_NUM_ENTITY:
		return Scr_SetEntityField(entnum, offset);

	case CLASS_NUM_HUDELEM:
		Scr_SetHudElemField(entnum, offset);

	default:
		return qtrue;
	}
}

/*
===============
Scr_GetEnt
===============
*/
void Scr_GetEntityField( int entnum, int offset )
{
	const game_entity_field_t *field;
	gentity_t *ent;

	assert((unsigned)entnum < MAX_GENTITIES);
	ent = &g_entities[entnum];
	assert(ent->r.inuse);

	if ( (offset & ENTFIELD_MASK) == ENTFIELD_CLIENT )
	{
		if ( !ent->client )
		{
		}
		else
		{
			Scr_GetClientField(ent->client, offset & ~ENTFIELD_CLIENT);
		}
	}
	else
	{
		assert((unsigned)offset < ARRAY_COUNT( g_entity_fields ) - 1);
		field = &g_entity_fields[offset];
		Scr_GetGenericField((byte *)ent, field->type, field->ofs);
	}
}

/*
===============
Scr_GetGenericField
===============
*/
void Scr_GetGenericField( byte *b, int type, int ofs )
{
	const char *str;
	unsigned short s;
	vec3_t vec;
	unsigned short o;

	switch ( type )
	{
	case F_STRING:
		s = *( unsigned short * )( b + ofs );
		if ( s )
			Scr_AddConstString(s);
		break;

	case F_LSTRING:
		str = ( const char * )( b + ofs );
		Scr_AddString(str);
		break;

	case F_VECTOR:
		Scr_AddVector( ( float * )( b + ofs ) );
		break;

	case F_INT:
		Scr_AddInt( *( int * )( b + ofs ) );
		break;

	case F_FLOAT:
		Scr_AddFloat( *( float * )( b + ofs ) );
		break;

	case F_VECTORHACK:
		vec[0] = 0;
		vec[1] = *( float * )( b + ofs );
		vec[2] = 0;
		Scr_AddVector(vec);
		break;

	case F_ENTITY:
		if ( *( gentity_t ** )( b + ofs ) )
			Scr_AddEntity( *( gentity_t ** )( b + ofs ) );
		break;

	case F_OBJECT:
		o = *( unsigned short * )( b + ofs );
		if ( o )
			Scr_AddObject(o);
		break;

	case F_MODEL:
		Scr_AddString( G_ModelName( *( byte * )( b + ofs ) ) );
		break;
	}
}

/*
===============
Scr_GetObjectField
===============
*/
void Scr_GetObjectField( unsigned int classnum, int entnum, int offset )
{
	switch ( classnum )
	{
	case CLASS_NUM_ENTITY:
		Scr_GetEntityField(entnum, offset);
		break;

	case CLASS_NUM_HUDELEM:
		Scr_GetHudElemField(entnum, offset);
		break;

	default:
		assert(va("bad class num %u", classnum));
		break;
	}
}

/*
===============
Scr_FreeEntityConstStrings
===============
*/
void Scr_FreeEntityConstStrings( gentity_t *ent )
{
	const game_entity_field_t *field;
	byte *pEnt;
	int i;

	for ( field = g_entity_fields; field->name; field++ )
	{
		if ( field->type == F_STRING )
		{
			pEnt = (byte *)ent;
			Scr_SetString((unsigned short *)(pEnt + field->ofs), 0);
		}
	}

	for ( i = 0; i < MAX_MODEL_ATTACHMENTS + 1; i++ )
	{
		ent->attachModelNames[i] = 0;
		Scr_SetString(&ent->attachTagNames[i], 0);
	}
}

/*
===============
Scr_FreeEntity
===============
*/
void Scr_FreeEntity( gentity_t *ent )
{
	Scr_FreeEntityConstStrings(ent);
	Scr_FreeEntityNum(ent->s.number, CLASS_NUM_ENTITY);
}

/*
===============
Scr_AddEntity
===============
*/
void Scr_AddEntity( gentity_t *ent )
{
	Scr_AddEntityNum(ent->s.number, CLASS_NUM_ENTITY);
}

/*
===============
GScr_SetDynamicEntityField
===============
*/
void GScr_SetDynamicEntityField( gentity_t *ent, unsigned int index )
{
	Scr_SetDynamicEntityField(ent->s.number, CLASS_NUM_ENTITY, index);
}

/*
===============
Scr_GetEntity
===============
*/
gentity_t* Scr_GetEntity( unsigned int index )
{
	scr_entref_t entref;

	entref = Scr_GetEntityRef(index);

	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		return &g_entities[entref.entnum];
	}

	Scr_ParamError(index, "not an entity");
	return NULL;
}

/*
===============
Scr_FreeHudElem
===============
*/
void Scr_FreeHudElem( game_hudelem_t *hud )
{
	assert(hud);
	assert(hud - g_hudelems >= 0 && hud - g_hudelems < MAX_HUDELEMS_TOTAL);
	assert(hud->elem.type != HE_TYPE_FREE);

	Scr_FreeHudElemConstStrings(hud);
	Scr_FreeEntityNum(hud - g_hudelems, CLASS_NUM_HUDELEM);
}

/*
===============
Scr_AddHudElem
===============
*/
void Scr_AddHudElem( game_hudelem_t *hud )
{
	assert(hud);
	assert(hud - g_hudelems >= 0 && hud - g_hudelems < MAX_HUDELEMS_TOTAL);
	assert(hud->elem.type != HE_TYPE_FREE);

	Scr_AddEntityNum(hud - g_hudelems, CLASS_NUM_HUDELEM);
}

/*
===============
Scr_GetHudElem
===============
*/
game_hudelem_t* Scr_GetHudElem( unsigned int index )
{
	scr_entref_t entref;

	entref = Scr_GetEntityRef(index);

	if ( entref.classnum == CLASS_NUM_HUDELEM )
	{
		assert(entref.entnum < MAX_HUDELEMS_TOTAL);
		return &g_hudelems[entref.entnum];
	}
	else
	{
		Scr_ParamError(index, "not a hudelem");
		return NULL;
	}
}

/*
==============
Scr_ExecEntThread
==============
*/
unsigned short Scr_ExecEntThread( gentity_t *ent, int handle, unsigned int paramcount )
{
	return Scr_ExecEntThreadNum(ent->s.number, CLASS_NUM_ENTITY, handle, paramcount);
}

/*
==============
Scr_AddExecEntThread
==============
*/
void Scr_AddExecEntThread( gentity_t *ent, int handle, unsigned int paramcount )
{
	Scr_AddExecEntThreadNum(ent->s.number, CLASS_NUM_ENTITY, handle, paramcount);
}

/*
==============
Scr_Notify
==============
*/
void Scr_Notify( gentity_t *ent, unsigned short stringValue, unsigned int paramcount )
{
	Scr_NotifyNum(ent->s.number, CLASS_NUM_ENTITY, stringValue, paramcount);
}

/*
===============
Scr_GetEnt
===============
*/
void Scr_GetEnt()
{
	const game_entity_field_t *field;
	gentity_t *ent;
	byte *b;
	const char *key;
	unsigned short value;
	unsigned short name;
	int offset;
	gentity_t *result;
	int i;

	name = Scr_GetConstString(0);
	key = Scr_GetString(1);
	offset = Scr_GetOffset(0, key);

	if ( offset < 0 )
	{
		return;
	}

	field = &g_entity_fields[offset];

	if ( field->type != F_STRING )
	{
		return;
	}

	for ( result = NULL, i = 0, ent = g_entities; i < level.num_entities; i++, ent++ )
	{
		if ( !ent->r.inuse )
		{
			continue;
		}

		b = (byte *)ent;
		value = *( unsigned short * )( b + field->ofs );

		if ( !value )
		{
			continue;
		}

		if ( value != name )
		{
			continue;
		}

		if ( result )
		{
			Scr_Error("getent used with more than one entity");
		}

		result = ent;
	}

	if ( result )
	{
		Scr_AddEntity(result);
	}
}

/*
===============
Scr_GetEntArray
===============
*/
void Scr_GetEntArray()
{
	const game_entity_field_t *field;
	gentity_t *ent;
	byte *b;
	const char *key;
	unsigned short value;
	unsigned short name;
	int offset;
	int i;

	if ( !Scr_GetNumParam() )
	{
		Scr_MakeArray();

		for ( i = 0, ent = g_entities; i < level.num_entities; i++, ent++ )
		{
			if ( !ent->r.inuse )
			{
				continue;
			}

			Scr_AddEntity(ent);
			Scr_AddArray();
		}

		return;
	}

	name = Scr_GetConstString(0);
	key = Scr_GetString(1);
	offset = Scr_GetOffset(0, key);

	if ( offset < 0 )
	{
		return;
	}

	field = &g_entity_fields[offset];

	if ( field->type != F_STRING )
	{
		return;
	}

	Scr_MakeArray();

	for ( i = 0, ent = g_entities; i < level.num_entities; i++, ent++ )
	{
		if ( !ent->r.inuse )
		{
			continue;
		}

		b = (byte *)ent;
		value = *( unsigned short * )( b + field->ofs );

		if ( !value )
		{
			continue;
		}

		if ( value != name )
		{
			continue;
		}

		Scr_AddEntity(ent);
		Scr_AddArray();
	}
}

/*QUAKED worldspawn (0 0 0) ? NO_GT_WOLF NO_GT_STOPWATCH NO_GT_CHECKPOINT NO_LMS

Every map should have exactly one worldspawn.
"music"     Music wav file
"gravity"   800 is default gravity
"message" Text to print during connection process
"ambient"  Ambient light value (must use '_color')
"_color"    Ambient light color (must be used with 'ambient')
"sun"        Shader to use for 'sun' image
*/
void SP_worldspawn( void )
{
	const char *s;

	G_SpawnString( "classname", "", &s );
	if ( Q_stricmp( s, "worldspawn" ) )
	{
		Com_Error( ERR_DROP, "\x15" "SP_worldspawn: The first entity isn't 'worldspawn'" );
	}

	// make some data visible to connecting client
	SV_SetConfigstring( CS_GAME_VERSION, GAME_VERSION );

	SV_SetConfigstring( CS_LEVEL_START_TIME, va( "%i", level.startTime ) );

	G_SpawnString( "ambienttrack", "", &s );
	if ( s[0] )
	{
		SV_SetConfigstring( CS_AMBIENT, va("n\\%s", s) );
	}
	else
	{
		SV_SetConfigstring( CS_AMBIENT, "" );
	}

	G_SpawnString( "message", "", &s );
	SV_SetConfigstring( CS_MESSAGE, s );              // map specific message

	SV_SetConfigstring( CS_MOTD, g_motd->current.string );      // message of the day

	G_SpawnString( "gravity", "800", &s );
	Dvar_SetFloat( g_gravity, atof(s) );

	G_SpawnString( "northyaw", "", &s );
	if ( s[0] )
		SV_SetConfigstring( CS_NORTHYAW, s );
	else
		SV_SetConfigstring( CS_NORTHYAW, "0" );

	G_SpawnString( "spawnflags", "0", &s );
	g_entities[ENTITYNUM_WORLD].spawnflags = atoi( s );

	g_entities[ENTITYNUM_WORLD].s.number = ENTITYNUM_WORLD;
	Scr_SetString(&g_entities[ENTITYNUM_WORLD].classname, scr_const.worldspawn);

	g_entities[ENTITYNUM_WORLD].r.inuse = qtrue;
}

/*
==============
G_SpawnEntitiesFromString

Parses textual entity definitions out of an entstring and spawns gentities.
==============
*/
void G_SpawnEntitiesFromString( void )
{
	// the worldspawn is not an actual entity, but it still
	// has a "spawn" function to perform any global setup
	// needed by a level (setting configstrings or cvars, etc)
	if ( !G_ParseSpawnVars( &level.spawnVar ) )
	{
		Com_Error( ERR_DROP, "\x15" "SpawnEntities: no entities" );
	}
	SP_worldspawn();

	// parse ents
	while ( G_ParseSpawnVars( &level.spawnVar ) )
	{
		G_CallSpawn();
	}
}

/*
==============
G_LoadStructs
==============
*/
void G_LoadStructs()
{
	unsigned short hThread;
	const char *classname;

	assert(g_scr_data.initstructs);
	hThread = Scr_ExecThread(g_scr_data.initstructs, 0);
	Scr_FreeThread(hThread);

	// parse structs
	while ( G_ParseSpawnVars(&level.spawnVar) )
	{
		G_SpawnString("classname", "", &classname);

		if ( !strcmp("script_struct", classname) )
		{
			G_SpawnStruct();
		}
	}

	SV_ResetEntityParsePoint();
}

