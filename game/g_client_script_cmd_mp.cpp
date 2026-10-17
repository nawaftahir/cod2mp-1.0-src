#include "../qcommon/qcommon.h"
#include "g_shared.h"

// Addresses the level bgs_t directly here, not through level_bgs_ptr.
#undef level_bgs
extern bgs_t level_bgs;

static const scr_method_t player_methods[] =
{
	{ "giveweapon", PlayerCmd_giveWeapon, qfalse, },
	{ "takeweapon", PlayerCmd_takeWeapon, qfalse, },
	{ "takeallweapons", PlayerCmd_takeAllWeapons, qfalse, },
	{ "getcurrentweapon", PlayerCmd_getCurrentWeapon, qfalse, },
	{ "getcurrentoffhand", PlayerCmd_getCurrentOffhand, qfalse, },
	{ "hasweapon", PlayerCmd_hasWeapon, qfalse, },
	{ "switchtoweapon", PlayerCmd_switchToWeapon, qfalse, },
	{ "switchtooffhand", PlayerCmd_switchToOffhand, qfalse, },
	{ "givestartammo", PlayerCmd_giveStartAmmo, qfalse, },
	{ "givemaxammo", PlayerCmd_giveMaxAmmo, qfalse, },
	{ "getfractionstartammo", PlayerCmd_getFractionStartAmmo, qfalse, },
	{ "getfractionmaxammo", PlayerCmd_getFractionMaxAmmo, qfalse, },
	{ "setorigin", PlayerCmd_setOrigin, qfalse, },
	{ "setplayerangles", PlayerCmd_setAngles, qfalse, },
	{ "getplayerangles", PlayerCmd_getAngles, qfalse, },
	{ "usebuttonpressed", PlayerCmd_useButtonPressed, qfalse, },
	{ "attackbuttonpressed", PlayerCmd_attackButtonPressed, qfalse, },
	{ "meleebuttonpressed", PlayerCmd_meleeButtonPressed, qfalse, },
	{ "playerads", PlayerCmd_playerADS, qfalse, },
	{ "isonground", PlayerCmd_isOnGround, qfalse, },
	{ "pingplayer", PlayerCmd_pingPlayer, qfalse, },
	{ "setviewmodel", PlayerCmd_SetViewmodel, qfalse, },
	{ "getviewmodel", PlayerCmd_GetViewmodel, qfalse, },
	{ "sayall", PlayerCmd_SayAll, qfalse, },
	{ "sayteam", PlayerCmd_SayTeam, qfalse, },
	{ "showscoreboard", PlayerCmd_showScoreboard, qfalse, },
	{ "setspawnweapon", PlayerCmd_setSpawnWeapon, qfalse, },
	{ "dropitem", PlayerCmd_dropItem, qfalse, },
	{ "finishplayerdamage", PlayerCmd_finishPlayerDamage, qfalse, },
	{ "suicide", PlayerCmd_Suicide, qfalse, },
	{ "openmenu", PlayerCmd_OpenMenu, qfalse, },
	{ "openmenunomouse", PlayerCmd_OpenMenuNoMouse, qfalse, },
	{ "closemenu", PlayerCmd_CloseMenu, qfalse, },
	{ "closeingamemenu", PlayerCmd_CloseInGameMenu, qfalse, },
	{ "freezecontrols", PlayerCmd_FreezeControls, qfalse, },
	{ "disableweapon", PlayerCmd_DisableWeapon, qfalse, },
	{ "enableweapon", PlayerCmd_EnableWeapon, qfalse, },
	{ "setreverb", PlayerCmd_SetReverb, qfalse, },
	{ "deactivatereverb", PlayerCmd_DeactivateReverb, qfalse, },
	{ "setchannelvolumes", PlayerCmd_SetChannelVolumes, qfalse, },
	{ "deactivatechannelvolumes", PlayerCmd_DeactivateChannelVolumes, qfalse, },
	{ "getweaponslotweapon", PlayerCmd_GetWeaponSlotWeapon, qfalse, },
	{ "setweaponslotweapon", PlayerCmd_SetWeaponSlotWeapon, qfalse, },
	{ "getweaponslotammo", PlayerCmd_GetWeaponSlotAmmo, qfalse, },
	{ "setweaponslotammo", PlayerCmd_SetWeaponSlotAmmo, qfalse, },
	{ "getweaponslotclipammo", PlayerCmd_GetWeaponSlotClipAmmo, qfalse, },
	{ "setweaponslotclipammo", PlayerCmd_SetWeaponSlotClipAmmo, qfalse, },
	{ "setweaponclipammo", PlayerCmd_SetWeaponClipAmmo, qfalse, },
	{ "iprintln", iclientprintln, qfalse, },
	{ "iprintlnbold", iclientprintlnbold, qfalse, },
	{ "spawn", PlayerCmd_spawn, qfalse, },
	{ "setentertime", PlayerCmd_setEnterTime, qfalse, },
	{ "cloneplayer", PlayerCmd_ClonePlayer, qfalse, },
	{ "setclientcvar", PlayerCmd_SetClientDvar, qfalse, },
	{ "islookingat", ScrCmd_IsLookingAt, qfalse, },
	{ "playlocalsound", ScrCmd_PlayLocalSound, qfalse, },
	{ "istalking", PlayerCmd_IsTalking, qfalse, },
	{ "allowspectateteam", PlayerCmd_AllowSpectateTeam, qfalse, },
	{ "getguid", PlayerCmd_GetGuid, qfalse, }
};


void PlayerCmd_giveWeapon( scr_entref_t entref );
void PlayerCmd_takeWeapon( scr_entref_t entref );
static qboolean ClientPlaying( gentity_t *pSelf );
void PlayerCmd_getCurrentWeapon( scr_entref_t entref );
void PlayerCmd_getCurrentOffhand( scr_entref_t entref );
void PlayerCmd_hasWeapon( scr_entref_t entref );
void PlayerCmd_switchToWeapon( scr_entref_t entref );
void PlayerCmd_switchToOffhand( scr_entref_t entref );
void PlayerCmd_giveStartAmmo( scr_entref_t entref );
void PlayerCmd_giveMaxAmmo( scr_entref_t entref );
void PlayerCmd_getFractionStartAmmo( scr_entref_t entref );
void PlayerCmd_getFractionMaxAmmo( scr_entref_t entref );
void PlayerCmd_setOrigin( scr_entref_t entref );
void PlayerCmd_setAngles( scr_entref_t entref );
void PlayerCmd_getAngles( scr_entref_t entref );
void PlayerCmd_useButtonPressed( scr_entref_t entref );
void PlayerCmd_attackButtonPressed( scr_entref_t entref );
void PlayerCmd_meleeButtonPressed( scr_entref_t entref );
void PlayerCmd_playerADS( scr_entref_t entref );
void PlayerCmd_isOnGround( scr_entref_t entref );
void PlayerCmd_pingPlayer( scr_entref_t entref );
void PlayerCmd_SetViewmodel( scr_entref_t entref );
void PlayerCmd_GetViewmodel( scr_entref_t entref );
void PlayerCmd_showScoreboard( scr_entref_t entref );
void PlayerCmd_setSpawnWeapon( scr_entref_t entref );
void PlayerCmd_dropItem( scr_entref_t entref );
void PlayerCmd_finishPlayerDamage( scr_entref_t entref );
void PlayerCmd_Suicide( scr_entref_t entref );
void PlayerCmd_OpenMenu( scr_entref_t entref );
void PlayerCmd_OpenMenuNoMouse( scr_entref_t entref );
void PlayerCmd_CloseMenu( scr_entref_t entref );
void PlayerCmd_CloseInGameMenu( scr_entref_t entref );
void PlayerCmd_GetWeaponSlotWeapon( scr_entref_t entref );
void PlayerCmd_SetWeaponSlotWeapon( scr_entref_t entref );
void PlayerCmd_GetWeaponSlotAmmo( scr_entref_t entref );
void PlayerCmd_SetWeaponSlotAmmo( scr_entref_t entref );
void PlayerCmd_GetWeaponSlotClipAmmo( scr_entref_t entref );
void PlayerCmd_SetWeaponSlotClipAmmo( scr_entref_t entref );
void PlayerCmd_SetWeaponClipAmmo( scr_entref_t entref );
void iclientprintln( scr_entref_t entref );
void iclientprintlnbold( scr_entref_t entref );
void PlayerCmd_spawn( scr_entref_t entref );
void PlayerCmd_setEnterTime( scr_entref_t entref );
void BodyEnd( gentity_t *ent );
void PlayerCmd_ClonePlayer( scr_entref_t entref );
void PlayerCmd_SetClientDvar( scr_entref_t entref );
void PlayerCmd_IsTalking( scr_entref_t entref );
void PlayerCmd_FreezeControls( scr_entref_t entref );
void PlayerCmd_DisableWeapon( scr_entref_t entref );
void PlayerCmd_EnableWeapon( scr_entref_t entref );
void PlayerCmd_SetReverb( scr_entref_t entref );
void PlayerCmd_DeactivateReverb( scr_entref_t entref );
void PlayerCmd_SetChannelVolumes( scr_entref_t entref );
void PlayerCmd_DeactivateChannelVolumes( scr_entref_t entref );
void ScrCmd_IsLookingAt( scr_entref_t entref );
void ScrCmd_PlayLocalSound( scr_entref_t entref );
void PlayerCmd_SayAll( scr_entref_t entref );
void PlayerCmd_SayTeam( scr_entref_t entref );
void PlayerCmd_AllowSpectateTeam( scr_entref_t entref );
void PlayerCmd_GetGuid( scr_entref_t entref );
void (*Player_GetMethod( const char **pName ))( scr_entref_t );

/*
===============
PlayerCmd_giveWeapon
===============
*/
void PlayerCmd_giveWeapon( scr_entref_t entref )
{
	WeaponDef *weaponDef;
	const char *pszWeaponName;
	int iWeaponIndex;
	int ammoGive;
	int hadWeapon;
	gclient_t *client;
	gentity_t *pSelf;

	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		pSelf = &g_entities[entref.entnum];

		if ( pSelf->client == NULL )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}
	else
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
	}

	hadWeapon = 0;

	pszWeaponName = Scr_GetString(0);
	iWeaponIndex = G_GetWeaponIndexForName(pszWeaponName);

	client = pSelf->client;
	hadWeapon = Com_BitCheck(client->ps.weapons, iWeaponIndex);
	weaponDef = BG_GetWeaponDef(iWeaponIndex);

	if ( BG_DoesWeaponNeedSlot(iWeaponIndex) && !BG_GetEmptySlotForWeapon(&client->ps, iWeaponIndex) )
	{
		Scr_ParamError(0, va("Cannot give %s weapon %s without having an empty weapon slot - player currently has a %s and a %s\n",
		                     pSelf->client->sess.cs.name,
		                     weaponDef->displayName,
		                     BG_GetWeaponDef(client->ps.weaponslots[SLOT_PRIMARY])->displayName,
		                     BG_GetWeaponDef(client->ps.weaponslots[SLOT_PRIMARYB])->displayName));
	}

	if ( G_GivePlayerWeapon(&client->ps, iWeaponIndex) )
	{
		SV_GameSendServerCommand(pSelf - g_entities, SV_CMD_CAN_IGNORE, va("%c \"%i\"", 73, 1));
	}

	ammoGive = weaponDef->startAmmo - client->ps.ammo[weaponDef->ammoIndex];

	if ( ammoGive > 0 )
	{
		Add_Ammo(pSelf, iWeaponIndex, ammoGive, hadWeapon == qfalse);
	}
}

/*
===============
PlayerCmd_takeWeapon
===============
*/
void PlayerCmd_takeWeapon( scr_entref_t entref )
{
	const char *pszWeaponName;
	int iWeaponIndex;
	gentity_t *pSelf;

	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		pSelf = &g_entities[entref.entnum];

		if ( pSelf->client == NULL )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}
	else
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
	}

	pszWeaponName = Scr_GetString(0);
	iWeaponIndex = G_GetWeaponIndexForName(pszWeaponName);

	pSelf->client->ps.ammo[BG_AmmoForWeapon(iWeaponIndex)] = 0;
	pSelf->client->ps.ammoclip[BG_ClipForWeapon(iWeaponIndex)] = 0;

	BG_TakePlayerWeapon(&pSelf->client->ps, iWeaponIndex);
}

/*
===============
PlayerCmd_pingPlayer
===============
*/
void PlayerCmd_takeAllWeapons( scr_entref_t entref )
{
	int weapIndex;
	gentity_t *pSelf;

	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		pSelf = &g_entities[entref.entnum];

		if ( pSelf->client == NULL )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}
	else
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
	}

	pSelf->client->ps.weapon = WP_NONE;

	for ( weapIndex = 1; weapIndex <= BG_GetNumWeapons(); weapIndex++ )
	{
		pSelf->client->ps.ammo[BG_AmmoForWeapon(weapIndex)] = 0;
		pSelf->client->ps.ammoclip[BG_ClipForWeapon(weapIndex)] = 0;

		BG_TakePlayerWeapon(&pSelf->client->ps, weapIndex);
	}
}

/*
===============
ClientPlaying
===============
*/
static qboolean ClientPlaying( gentity_t *pSelf )
{
	assert(pSelf->client);
	assert(pSelf->client->sess.connected != CON_DISCONNECTED);

	return pSelf->client->sess.sessionState == SESS_STATE_PLAYING;
}

/*
===============
PlayerCmd_getCurrentWeapon
===============
*/
void PlayerCmd_getCurrentWeapon( scr_entref_t entref )
{
	WeaponDef *weapDef;
	gclient_t *client;
	int weapon;
	gentity_t *pSelf;

	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		pSelf = &g_entities[entref.entnum];

		if ( pSelf->client == NULL )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}
	else
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
	}

	if ( !ClientPlaying(pSelf) )
	{
		Scr_AddString("none");
	}
	else
	{
		client = pSelf->client;
		weapon = client->ps.weapon;

		if ( weapon > 0 )
		{
			weapDef = BG_GetWeaponDef(weapon);
			Scr_AddString(weapDef->szInternalName);
		}
		else
		{
			Scr_AddString("none");
		}
	}
}

/*
===============
PlayerCmd_getCurrentOffhand
===============
*/
void PlayerCmd_getCurrentOffhand( scr_entref_t entref )
{
	WeaponDef *weapDef;
	gentity_t *pSelf;

	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		pSelf = &g_entities[entref.entnum];

		if ( pSelf->client == NULL )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}
	else
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
	}

	if ( !ClientPlaying(pSelf) )
	{
		Scr_AddString("none");
	}
	else
	{
		if ( pSelf->client->ps.offHandIndex > 0 )
		{
			weapDef = BG_GetWeaponDef(pSelf->client->ps.offHandIndex);
			Scr_AddString(weapDef->szInternalName);
		}
		else
		{
			Scr_AddString("none");
		}
	}
}

/*
===============
PlayerCmd_hasWeapon
===============
*/
void PlayerCmd_hasWeapon( scr_entref_t entref )
{
	const char *pszWeaponName;
	int iWeaponIndex;
	gentity_t *pSelf;

	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		pSelf = &g_entities[entref.entnum];

		if ( pSelf->client == NULL )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}
	else
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
	}

	pszWeaponName = Scr_GetString(0);
	iWeaponIndex = BG_FindWeaponIndexForName(pszWeaponName);

	if ( iWeaponIndex && Com_BitCheck( pSelf->client->ps.weapons, iWeaponIndex ) )
	{
		Scr_AddBool(true);
	}
	else
	{
		Scr_AddBool(false);
	}
}

/*
===============
PlayerCmd_switchToWeapon
===============
*/
void PlayerCmd_switchToWeapon( scr_entref_t entref )
{
	const char *pszWeaponName;
	int iWeaponIndex;
	gentity_t *pSelf;

	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		pSelf = &g_entities[entref.entnum];

		if ( pSelf->client == NULL )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}
	else
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
	}

	pszWeaponName = Scr_GetString(0);
	iWeaponIndex = G_GetWeaponIndexForName(pszWeaponName);

	if ( !iWeaponIndex )
	{
		Scr_ParamError(0, va("unknown weapon '%s'", pszWeaponName));
	}

	if ( Com_BitCheck(pSelf->client->ps.weapons, iWeaponIndex) )
	{
		G_SelectWeaponIndex(entref.entnum, iWeaponIndex);
		Scr_AddBool(true);
	}
	else
	{
		Scr_AddBool(false);
	}
}

/*
===============
PlayerCmd_switchToOffhand
===============
*/
void PlayerCmd_switchToOffhand( scr_entref_t entref )
{
	const char *pszWeaponName;
	int iWeaponIndex;
	gentity_t *pSelf;

	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		pSelf = &g_entities[entref.entnum];

		if ( pSelf->client == NULL )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}
	else
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
	}

	pszWeaponName = Scr_GetString(0);
	iWeaponIndex = G_GetWeaponIndexForName(pszWeaponName);

	if ( !iWeaponIndex )
	{
		Scr_ParamError(0, va("unknown weapon '%s'", pszWeaponName));
	}

	if ( Com_BitCheck(pSelf->client->ps.weapons, iWeaponIndex) )
	{
		G_SetEquippedOffHand(entref.entnum, iWeaponIndex);
		Scr_AddBool(true);
	}
	else
	{
		Scr_AddBool(false);
	}
}

/*
===============
PlayerCmd_giveStartAmmo
===============
*/
void PlayerCmd_giveStartAmmo( scr_entref_t entref )
{
	WeaponDef *weapDef;
	const char *pszWeaponName;
	int iWeaponIndex;
	int ammoGive;
	gentity_t *pSelf;

	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		pSelf = &g_entities[entref.entnum];

		if ( pSelf->client == NULL )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}
	else
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
	}

	pszWeaponName = Scr_GetString(0);
	iWeaponIndex = G_GetWeaponIndexForName(pszWeaponName);

	if ( Com_BitCheck(pSelf->client->ps.weapons, iWeaponIndex) )
	{
		weapDef = BG_GetWeaponDef(iWeaponIndex);
		ammoGive = weapDef->startAmmo - pSelf->client->ps.ammo[weapDef->ammoIndex];

		if ( ammoGive > 0 )
		{
			Add_Ammo(pSelf, iWeaponIndex, ammoGive, qfalse);
		}
	}
}

/*
===============
PlayerCmd_giveMaxAmmo
===============
*/
void PlayerCmd_giveMaxAmmo( scr_entref_t entref )
{
	WeaponDef *weapDef;
	const char *pszWeaponName;
	int iWeaponIndex;
	int ammoGive;
	gentity_t *pSelf;

	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		pSelf = &g_entities[entref.entnum];

		if ( pSelf->client == NULL )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}
	else
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
	}

	pszWeaponName = Scr_GetString(0);
	iWeaponIndex = G_GetWeaponIndexForName(pszWeaponName);

	if ( Com_BitCheck(pSelf->client->ps.weapons, iWeaponIndex) )
	{
		weapDef = BG_GetWeaponDef(iWeaponIndex);
		ammoGive = BG_GetAmmoTypeMax(weapDef->ammoIndex) - pSelf->client->ps.ammo[weapDef->ammoIndex];

		if ( ammoGive > 0 )
		{
			Add_Ammo(pSelf, iWeaponIndex, ammoGive, qfalse);
		}
	}
}

/*
===============
PlayerCmd_getFractionStartAmmo
===============
*/
void PlayerCmd_getFractionStartAmmo( scr_entref_t entref )
{
	WeaponDef *weapDef;
	const char *pszWeaponName;
	int iWeaponIndex;
	float fAmmoFrac;
	gentity_t *pSelf;

	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		pSelf = &g_entities[entref.entnum];

		if ( pSelf->client == NULL )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}
	else
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
	}

	pszWeaponName = Scr_GetString(0);
	iWeaponIndex = G_GetWeaponIndexForName(pszWeaponName);

	if ( Com_BitCheck(pSelf->client->ps.weapons, iWeaponIndex) )
	{
		weapDef = BG_GetWeaponDef(iWeaponIndex);

		if ( weapDef->startAmmo < 1 )
		{
			Scr_AddFloat(1.0f);
			return;
		}

		if ( pSelf->client->ps.ammo[weapDef->ammoIndex] < 1 )
		{
			Scr_AddFloat(0.0f);
			return;
		}

		fAmmoFrac = (float)pSelf->client->ps.ammo[weapDef->ammoIndex] / (float)weapDef->startAmmo;
		Scr_AddFloat(fAmmoFrac);
	}
	else
	{
		Scr_AddFloat(1.0f);
	}
}

/*
===============
PlayerCmd_getFractionMaxAmmo
===============
*/
void PlayerCmd_getFractionMaxAmmo( scr_entref_t entref )
{
	WeaponDef *weapDef;
	const char *pszWeaponName;
	int iWeaponIndex;
	float fAmmoFrac;
	gentity_t *pSelf;

	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		pSelf = &g_entities[entref.entnum];

		if ( pSelf->client == NULL )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}
	else
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
	}

	pszWeaponName = Scr_GetString(0);
	iWeaponIndex = G_GetWeaponIndexForName(pszWeaponName);

	if ( Com_BitCheck(pSelf->client->ps.weapons, iWeaponIndex) )
	{
		weapDef = BG_GetWeaponDef(iWeaponIndex);

		if ( BG_GetAmmoTypeMax(weapDef->ammoIndex) < 1 )
		{
			Scr_AddFloat(1.0f);
			return;
		}

		if ( pSelf->client->ps.ammo[weapDef->ammoIndex] < 1 )
		{
			Scr_AddFloat(0.0f);
			return;
		}

		fAmmoFrac = (float)pSelf->client->ps.ammo[weapDef->ammoIndex] / (float)BG_GetAmmoTypeMax(weapDef->ammoIndex);
		Scr_AddFloat(fAmmoFrac);
	}
	else
	{
		Scr_AddFloat(1.0f);
	}
}

/*
===============
PlayerCmd_setOrigin
===============
*/
void PlayerCmd_setOrigin( scr_entref_t entref )
{
	vec3_t vNewOrigin;
	gentity_t *pSelf;

	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		pSelf = &g_entities[entref.entnum];

		if ( pSelf->client == NULL )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}
	else
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
	}

	Scr_GetVector(0, vNewOrigin);

	SV_UnlinkEntity(pSelf);
	VectorCopy(vNewOrigin, pSelf->client->ps.origin);

	pSelf->client->ps.origin[2] += 1.0f;
	pSelf->client->ps.eFlags ^= EF_TELEPORT_BIT;

	BG_PlayerStateToEntityState(&pSelf->client->ps, &pSelf->s, qtrue, PMOVE_HANDLER_SERVER);

	VectorCopy(pSelf->client->ps.origin, pSelf->r.currentOrigin);
	SV_LinkEntity(pSelf);
}

/*
===============
PlayerCmd_setAngles
===============
*/
void PlayerCmd_setAngles( scr_entref_t entref )
{
	vec3_t angles;
	gentity_t *pSelf;

	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		pSelf = &g_entities[entref.entnum];

		if ( pSelf->client == NULL )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}
	else
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
	}

	Scr_GetVector(0, angles);
	SetClientViewAngle(pSelf, angles);
}

/*
===============
PlayerCmd_getAngles
===============
*/
void PlayerCmd_getAngles( scr_entref_t entref )
{
	gentity_t *pSelf;

	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		pSelf = &g_entities[entref.entnum];

		if ( pSelf->client == NULL )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}
	else
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
	}

	Scr_AddVector(pSelf->client->ps.viewangles);
}

/*
===============
PlayerCmd_useButtonPressed
===============
*/
void PlayerCmd_useButtonPressed( scr_entref_t entref )
{
	gentity_t *pSelf;

	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		pSelf = &g_entities[entref.entnum];

		if ( pSelf->client == NULL )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}
	else
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
	}

	if ( ( pSelf->client->buttonsSinceLastFrame | pSelf->client->buttons ) & ( BUTTON_USE | BUTTON_USERELOAD ) )
		Scr_AddInt( 1 );
	else
		Scr_AddInt( 0 );
}

/*
===============
PlayerCmd_attackButtonPressed
===============
*/
void PlayerCmd_attackButtonPressed( scr_entref_t entref )
{
	gentity_t *pSelf;

	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		pSelf = &g_entities[entref.entnum];

		if ( pSelf->client == NULL )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}
	else
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
	}

	if ( ( pSelf->client->buttonsSinceLastFrame | pSelf->client->buttons ) & BUTTON_ATTACK )
		Scr_AddInt( 1 );
	else
		Scr_AddInt( 0 );
}

/*
===============
PlayerCmd_meleeButtonPressed
===============
*/
void PlayerCmd_meleeButtonPressed( scr_entref_t entref )
{
	gentity_t *pSelf;

	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		pSelf = &g_entities[entref.entnum];

		if ( pSelf->client == NULL )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}
	else
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
	}

	if ( ( pSelf->client->buttonsSinceLastFrame | pSelf->client->buttons ) & BUTTON_MELEE )
		Scr_AddInt( 1 );
	else
		Scr_AddInt( 0 );
}

/*
===============
PlayerCmd_playerADS
===============
*/
void PlayerCmd_playerADS( scr_entref_t entref )
{
	gentity_t *pSelf;

	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		pSelf = &g_entities[entref.entnum];

		if ( pSelf->client == NULL )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}
	else
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
	}

	Scr_AddFloat( pSelf->client->ps.fWeaponPosFrac );
}

/*
===============
PlayerCmd_isOnGround
===============
*/
void PlayerCmd_isOnGround( scr_entref_t entref )
{
	gentity_t *pSelf;

	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		pSelf = &g_entities[entref.entnum];

		if ( pSelf->client == NULL )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}
	else
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
	}

	if ( pSelf->client->ps.groundEntityNum != ENTITYNUM_NONE )
		Scr_AddInt( 1 );
	else
		Scr_AddInt( 0 );
}

/*
===============
PlayerCmd_pingPlayer
===============
*/
void PlayerCmd_pingPlayer( scr_entref_t entref )
{
	gentity_t *pSelf;

	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		pSelf = &g_entities[entref.entnum];

		if ( pSelf->client == NULL )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}
	else
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
	}

	pSelf->client->ps.eFlags |= EF_TAUNT;
	pSelf->client->compassPingTime = level.time + 3000;
}

/*
===============
PlayerCmd_SetViewmodel
===============
*/
void PlayerCmd_SetViewmodel( scr_entref_t entref )
{
	const char *modelName;
	int viewmodelIndex;
	gentity_t *pSelf;

	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		pSelf = &g_entities[entref.entnum];

		if ( pSelf->client == NULL )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}
	else
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
	}

	modelName = Scr_GetString(0);

	if ( !modelName || !modelName[0] )
		Scr_ParamError(0, "usage: setviewmodel(<model name>)");

	viewmodelIndex = G_ModelIndex(modelName);
	pSelf->client->sess.viewmodelIndex = viewmodelIndex;
}

/*
===============
PlayerCmd_GetViewmodel
===============
*/
void PlayerCmd_GetViewmodel( scr_entref_t entref )
{
	const char *modelName;
	gentity_t *pSelf;

	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		pSelf = &g_entities[entref.entnum];

		if ( pSelf->client == NULL )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}
	else
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
	}

	modelName = G_ModelName(pSelf->client->sess.viewmodelIndex);
	Scr_AddString(modelName);
}

/*
===============
PlayerCmd_showScoreboard
===============
*/
void PlayerCmd_showScoreboard( scr_entref_t entref )
{
	gentity_t *pSelf;

	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		pSelf = &g_entities[entref.entnum];

		if ( pSelf->client == NULL )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}
	else
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
	}

	Cmd_Score_f(pSelf);
}

/*
===============
PlayerCmd_setSpawnWeapon
===============
*/
void PlayerCmd_setSpawnWeapon( scr_entref_t entref )
{
	const char *pszWeaponName;
	int iWeaponIndex;
	gentity_t *pSelf;

	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		pSelf = &g_entities[entref.entnum];

		if ( pSelf->client == NULL )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}
	else
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
	}

	pszWeaponName = Scr_GetString(0);
	iWeaponIndex = G_GetWeaponIndexForName(pszWeaponName);

	if ( BG_IsWeaponValid(&pSelf->client->ps, iWeaponIndex) )
	{
		pSelf->client->ps.weapon = iWeaponIndex;
		pSelf->client->ps.weaponstate = WEAPON_READY;
	}
}

/*
===============
PlayerCmd_dropItem
===============
*/
void PlayerCmd_dropItem( scr_entref_t entref )
{
	const char *pszItemName;
	int iWeaponIndex;
	unsigned int dropTag;
	gentity_t *pEnt;
	gitem_t *pItem;
	gentity_t *pSelf;

	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		pSelf = &g_entities[entref.entnum];

		if ( pSelf->client == NULL )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}
	else
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
	}

	pszItemName = Scr_GetString(0);
	iWeaponIndex = G_GetWeaponIndexForName(pszItemName);

	if ( iWeaponIndex )
	{
		if ( Scr_GetNumParam() > 1 )
			dropTag = Scr_GetConstLowercaseString(1);
		else
			dropTag = scr_const.tag_weapon_right;

		pEnt = Drop_Weapon(pSelf, iWeaponIndex, dropTag);
	}
	else
	{
		pItem = G_FindItem(pszItemName);

		if ( pItem )
			pEnt = Drop_Item(pSelf, pItem, 0, qfalse);
		else
			pEnt = NULL;
	}

	GScr_AddEntity(pEnt);
}

/*
===============
PlayerCmd_finishPlayerDamage
===============
*/
void PlayerCmd_finishPlayerDamage( scr_entref_t entref )
{
	gentity_t *inflictor;
	gentity_t *attacker;
	vec3_t vDir;
	float *dir;
	vec3_t vPoint;
	float *point;
	int damage;
	int dflags;
	int modIndex;
	int iWeapon;
	int hitLoc;
	int knockback;
	float knockbackMod;
	vec3_t localdir;
	vec3_t kvel;
	float mass;
	int t;
	int iSurfType;
	gentity_t *tempBulletHitEntity;
	void (*die)(gentity_t *, gentity_t *, gentity_t *, int, int, const int, const float *, int, int);
	void (*pain)(gentity_t *, gentity_t *, int, const float *, int, const float *, int);
	float time_per_point;
	float max_damage_time;
	float damage_time;
	float flinchYawDir;
	gentity_t *pSelf;

	inflictor = &g_entities[ENTITYNUM_WORLD];
	attacker = &g_entities[ENTITYNUM_WORLD];

	dir = NULL;
	point = NULL;

	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		pSelf = &g_entities[entref.entnum];

		if ( pSelf->client == NULL )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}
	else
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
	}

	damage = Scr_GetInt(2);

	if ( damage <= 0 )
	{
		return;
	}

	if ( Scr_GetType(0) && Scr_GetPointerType(0) == VAR_ENTITY )
	{
		inflictor = Scr_GetEntity(1);
	}

	if ( Scr_GetType(1) && Scr_GetPointerType(1) == VAR_ENTITY )
	{
		attacker = Scr_GetEntity(1);
	}

	dflags = Scr_GetInt(3);
	modIndex = G_IndexForMeansOfDeath(Scr_GetString(4));
	iWeapon = G_GetWeaponIndexForName(Scr_GetString(5));

	if ( Scr_GetType(6) )
	{
		Scr_GetVector(6, vPoint);
		point = vPoint;
	}

	if ( Scr_GetType(7) )
	{
		Scr_GetVector(7, vDir);
		dir = vDir;
	}

	hitLoc = G_GetHitLocationIndexFromString(Scr_GetConstString(8));
	iSurfType = Scr_GetInt(9);

	if ( dir )
	{
		Vec3NormalizeTo(dir, localdir);
	}
	else
	{
		VectorClear(localdir);
	}

	if ( (pSelf->flags & FL_NO_KNOCKBACK) || (dflags & DAMAGE_NO_KNOCKBACK) )
	{
		knockback = 0;
	}
	else
	{
		knockbackMod = 0.3f;

		if ( pSelf->client->ps.pm_flags & PMF_PRONE )
		{
			knockbackMod = 0.02f;
		}
		else if ( pSelf->client->ps.pm_flags & PMF_DUCKED )
		{
			knockbackMod = 0.15f;
		}

		knockback = (int)(damage * knockbackMod);

		if ( knockback > 60 )
		{
			knockback = 60;
		}

		if ( knockback )
		{
			if ( !(pSelf->client->ps.eFlags & EF_TURRET_ACTIVE) )
			{
				mass = 250.0f;
				VectorScale(localdir, g_knockback->current.decimal * knockback / mass, kvel);
				VectorAdd(pSelf->client->ps.velocity, kvel, pSelf->client->ps.velocity);

				if ( !pSelf->client->ps.pm_time )
				{
					t = knockback * 2;

					if ( t < 50 )
					{
						t = 50;
					}

					if ( t > 200 )
					{
						t = 200;
					}

					pSelf->client->ps.pm_time = t;
					pSelf->client->ps.pm_flags |= PMF_TIME_KNOCKBACK;
				}
			}
		}
	}

	if ( pSelf->flags & FL_GODMODE )
	{
		return;
	}

	if ( iWeapon && BG_GetWeaponDef(iWeapon)->weaponType == WEAPTYPE_BULLET )
	{
		tempBulletHitEntity = G_TempEntity(vPoint, BG_GetWeaponDef(iWeapon)->rifleBullet ? EV_SHOTGUN_HIT : EV_BULLET_HIT_LARGE);
		tempBulletHitEntity->s.eventParm = DirToByte(localdir);
		tempBulletHitEntity->s.hintString = DirToByte(localdir);
		tempBulletHitEntity->s.surfType = SURF_TYPE_FLESH;
		tempBulletHitEntity->s.otherEntityNum = attacker->s.number;
		tempBulletHitEntity->r.clientMask[pSelf->client->ps.clientNum >> 5] |= 1 << (pSelf->client->ps.clientNum & 0x1F);

		tempBulletHitEntity = G_TempEntity(vPoint, BG_GetWeaponDef(iWeapon)->rifleBullet ? EV_BULLET_HIT_CLIENT_LARGE : EV_BULLET_HIT_CLIENT_SMALL);
		tempBulletHitEntity->s.surfType = SURF_TYPE_FLESH;
		tempBulletHitEntity->s.otherEntityNum = attacker->s.number;
		tempBulletHitEntity->s.clientNum = pSelf->client->ps.clientNum;
		tempBulletHitEntity->r.clientMask[0] = -1;
		tempBulletHitEntity->r.clientMask[1] = -1;
		tempBulletHitEntity->r.clientMask[pSelf->client->ps.clientNum >> 5] &= ~(1 << (pSelf->client->ps.clientNum & 0x1F));
	}

	pSelf->client->damage_blood += damage;

	if ( dir )
	{
		VectorCopy(localdir, pSelf->client->damage_from);
		pSelf->client->damage_fromWorld = qfalse;
	}
	else
	{
		VectorCopy(pSelf->r.currentOrigin, pSelf->client->damage_from);
		pSelf->client->damage_fromWorld = qtrue;
	}

	if ( (pSelf->flags & FL_DEMI_GODMODE) && pSelf->health - damage <= 0 )
	{
		damage = pSelf->health - 1;
	}

	time_per_point = player_dmgtimer_timePerPoint->current.decimal;
	max_damage_time = player_dmgtimer_maxTime->current.decimal;
	damage_time = damage * time_per_point;

	pSelf->client->ps.damageTimer += (int)damage_time;

	if ( dir )
	{
		pSelf->client->ps.flinchYaw = (int)vectoyaw(dir);
	}
	else
	{
		pSelf->client->ps.flinchYaw = 0;
	}

	flinchYawDir = pSelf->client->ps.viewangles[YAW];

	if ( flinchYawDir < 0 )
	{
		flinchYawDir = flinchYawDir + 360;
	}

	pSelf->client->ps.flinchYaw -= flinchYawDir;

	if ( pSelf->client->ps.damageTimer > max_damage_time )
	{
		pSelf->client->ps.damageTimer = (int)max_damage_time;
	}

	pSelf->client->ps.damageDuration = pSelf->client->ps.damageTimer;
	pSelf->health -= damage;

	Scr_AddEntity(attacker);
	Scr_AddInt(damage);
	Scr_Notify(pSelf, scr_const.damage, 2);

	if ( pSelf->health <= 0 )
	{
		if ( pSelf->health < -999 )
		{
			pSelf->health = -999;
		}

		die = entityHandlers[pSelf->handler].die;

		if ( die )
		{
			die(pSelf, inflictor, attacker, damage, modIndex, iWeapon, localdir, hitLoc, iSurfType);
		}

		if ( !pSelf->r.inuse )
		{
			return;
		}
	}
	else
	{
		pain = entityHandlers[pSelf->handler].pain;

		if ( pain )
		{
			pain(pSelf, attacker, damage, point, modIndex, localdir, hitLoc);
		}
	}

	pSelf->client->ps.stats[STAT_HEALTH] = pSelf->health;
}

/*
===============
PlayerCmd_Suicide
===============
*/
void PlayerCmd_Suicide( scr_entref_t entref )
{
	gentity_t *pSelf;

	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		pSelf = &g_entities[entref.entnum];

		if ( pSelf->client == NULL )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}
	else
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
	}

	pSelf->flags &= ~( FL_GODMODE | FL_DEMI_GODMODE );
	pSelf->client->ps.stats[STAT_HEALTH] = pSelf->health = 0;

	player_die(pSelf, pSelf, pSelf, 100000, MOD_SUICIDE, WP_NONE, NULL, HITLOC_NONE, 0);
}

/*
===============
PlayerCmd_OpenMenu
===============
*/
void PlayerCmd_OpenMenu( scr_entref_t entref )
{
	int iMenuIndex;
	gentity_t *pSelf;

	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		pSelf = &g_entities[entref.entnum];

		if ( pSelf->client == NULL )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}
	else
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
	}

	if ( pSelf->client->sess.connected == CON_CONNECTED )
	{
		iMenuIndex = GScr_GetScriptMenuIndex(Scr_GetString(0));

		SV_GameSendServerCommand(entref.entnum, SV_CMD_RELIABLE, va("%c %i", 116, iMenuIndex));
		Scr_AddInt(1);
	}
	else
	{
		Scr_AddInt(0);
	}
}

/*
===============
PlayerCmd_OpenMenuNoMouse
===============
*/
void PlayerCmd_OpenMenuNoMouse( scr_entref_t entref )
{
	int iMenuIndex;
	gentity_t *pSelf;

	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		pSelf = &g_entities[entref.entnum];

		if ( pSelf->client == NULL )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}
	else
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
	}

	if ( pSelf->client->sess.connected == CON_CONNECTED )
	{
		iMenuIndex = GScr_GetScriptMenuIndex(Scr_GetString(0));

		SV_GameSendServerCommand(entref.entnum, SV_CMD_RELIABLE, va("%c %i 1", 116, iMenuIndex));
		Scr_AddInt(1);
	}
	else
	{
		Scr_AddInt(0);
	}
}

/*
===============
PlayerCmd_CloseMenu
===============
*/
void PlayerCmd_CloseMenu( scr_entref_t entref )
{
	gentity_t *pSelf;

	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		pSelf = &g_entities[entref.entnum];

		if ( pSelf->client == NULL )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}
	else
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
	}

	SV_GameSendServerCommand(entref.entnum, SV_CMD_RELIABLE, va("%c", 117));
}

/*
===============
PlayerCmd_CloseInGameMenu
===============
*/
void PlayerCmd_CloseInGameMenu( scr_entref_t entref )
{
	gentity_t *pSelf;

	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		pSelf = &g_entities[entref.entnum];

		if ( pSelf->client == NULL )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}
	else
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
	}

	SV_GameSendServerCommand(entref.entnum, SV_CMD_RELIABLE, va("%c", 75));
}

/*
===============
PlayerCmd_GetWeaponSlotWeapon
===============
*/
void PlayerCmd_GetWeaponSlotWeapon( scr_entref_t entref )
{
	unsigned short slotName;
	int slot;
	int iWeaponIndex;
	WeaponDef *weapDef;
	gentity_t *pSelf;

	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		pSelf = &g_entities[entref.entnum];

		if ( pSelf->client == NULL )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}
	else
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
	}

	if ( !ClientPlaying(pSelf) )
	{
		Scr_AddConstString(scr_const.none);
		return;
	}

	slotName = Scr_GetConstString(0);
	slot = BG_GetWeaponSlotForName(SL_ConvertToString(slotName));

	if ( !slot )
	{
		Scr_ParamError(0, va("Unknown weaponslot name %s. Valid weaponslots are \"primary\" and \"primaryb\"", SL_ConvertToString(slotName)));
	}

	iWeaponIndex = pSelf->client->ps.weaponslots[slot];

	if ( !iWeaponIndex )
	{
		Scr_AddConstString(scr_const.none);
		return;
	}

	weapDef = BG_GetWeaponDef(iWeaponIndex);
	Scr_AddString(weapDef->szInternalName);
}

/*
===============
PlayerCmd_SetWeaponSlotWeapon
===============
*/
void PlayerCmd_SetWeaponSlotWeapon( scr_entref_t entref )
{
	unsigned short slotName;
	int slot;
	int slotWeapon;
	int iWeaponIndex;
	int ammoCount;
	const char *pszWeaponName;
	WeaponDef *weapDef;
	qboolean hadWeapon;
	qboolean isOnlyWeapon = qfalse;
	gentity_t *pSelf;

	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		pSelf = &g_entities[entref.entnum];

		if ( pSelf->client == NULL )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}
	else
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
	}

	slotName = Scr_GetConstString(0);
	slot = BG_GetWeaponSlotForName(SL_ConvertToString(slotName));

	if ( !slot )
	{
		Scr_ParamError(0, va("Unknown weaponslot name %s. Valid weaponslots are \"primary\" and \"primaryb\"", SL_ConvertToString(slotName)));
	}

	pszWeaponName = Scr_GetString(1);

	if ( !I_stricmp(pszWeaponName, "none") )
	{
		iWeaponIndex = WP_NONE;
		weapDef = NULL;
	}
	else
	{
		iWeaponIndex = G_GetWeaponIndexForName(pszWeaponName);

		if ( !iWeaponIndex )
		{
			Scr_ParamError(1, va("Unknown weapon %s.", pszWeaponName));
		}

		weapDef = BG_GetWeaponDef(iWeaponIndex);

		if ( weapDef->weaponSlot != slot )
		{
			if ( weapDef->weaponSlot != SLOT_PRIMARY && weapDef->weaponSlot != SLOT_PRIMARYB || slot != SLOT_PRIMARY && slot != SLOT_PRIMARYB )
			{
				Scr_ParamError(1, va("Weapon %s goes in the %s weaponslot, not the %s weaponslot.", pszWeaponName, BG_GetWeaponSlotNameForIndex(weapDef->weaponSlot), BG_GetWeaponSlotNameForIndex(slot)));
			}
		}
	}

	slotWeapon = pSelf->client->ps.weaponslots[slot];

	if ( slotWeapon )
	{
		BG_TakePlayerWeapon(&pSelf->client->ps, slotWeapon);
	}

	if ( !iWeaponIndex )
	{
		return;
	}

	if ( slot == SLOT_PRIMARYB && !pSelf->client->ps.weaponslots[SLOT_PRIMARY] )
	{
		isOnlyWeapon = qtrue;
	}

	hadWeapon = Com_BitCheck(pSelf->client->ps.weapons, iWeaponIndex);
	G_GivePlayerWeapon(&pSelf->client->ps, iWeaponIndex);

	if ( isOnlyWeapon )
	{
		pSelf->client->ps.weaponslots[SLOT_PRIMARYB] = pSelf->client->ps.weaponslots[SLOT_PRIMARY];
		pSelf->client->ps.weaponslots[SLOT_PRIMARY]  = WP_NONE;
	}

	ammoCount = weapDef->startAmmo - pSelf->client->ps.ammo[weapDef->ammoIndex];

	if ( ammoCount > 0 )
	{
		Add_Ammo(pSelf, iWeaponIndex, ammoCount, hadWeapon == qfalse);
	}
}

/*
===============
PlayerCmd_GetWeaponSlotAmmo
===============
*/
void PlayerCmd_GetWeaponSlotAmmo( scr_entref_t entref )
{
	unsigned short slotName;
	int slot;
	int iWeaponIndex;
	int ammoIndex;
	gentity_t *pSelf;

	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		pSelf = &g_entities[entref.entnum];

		if ( pSelf->client == NULL )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}
	else
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
	}

	if ( !ClientPlaying(pSelf) )
	{
		Scr_AddInt(0);
		return;
	}

	slotName = Scr_GetConstString(0);
	slot = BG_GetWeaponSlotForName(SL_ConvertToString(slotName));

	if ( !slot )
	{
		Scr_ParamError(0, va("Unknown weaponslot name %s. Valid weaponslots are \"primary\" and \"primaryb\"", SL_ConvertToString(slotName)));
	}

	iWeaponIndex = pSelf->client->ps.weaponslots[slot];

	if ( !iWeaponIndex )
	{
		Scr_AddInt(0);
		return;
	}

	if ( BG_WeaponIsClipOnly(iWeaponIndex) )
	{
		ammoIndex = BG_ClipForWeapon(iWeaponIndex);
		Scr_AddInt(pSelf->client->ps.ammoclip[ammoIndex]);
	}
	else
	{
		ammoIndex = BG_AmmoForWeapon(iWeaponIndex);
		Scr_AddInt(pSelf->client->ps.ammo[ammoIndex]);
	}
}

/*
===============
PlayerCmd_SetWeaponSlotAmmo
===============
*/
void PlayerCmd_SetWeaponSlotAmmo( scr_entref_t entref )
{
	unsigned short slotName;
	int slot;
	int iWeaponIndex;
	int ammoCount;
	int ammoIndex;
	gentity_t *pSelf;

	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		pSelf = &g_entities[entref.entnum];

		if ( pSelf->client == NULL )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}
	else
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
	}

	slotName = Scr_GetConstString(0);
	slot = BG_GetWeaponSlotForName(SL_ConvertToString(slotName));

	if ( !slot )
	{
		Scr_ParamError(0, va("Unknown weaponslot name %s. Valid weaponslots are \"primary\" and \"primaryb\"", SL_ConvertToString(slotName)));
	}

	ammoCount = Scr_GetInt(1);
	iWeaponIndex = pSelf->client->ps.weaponslots[slot];

	if ( !iWeaponIndex )
	{
		return;
	}

	if ( BG_WeaponIsClipOnly(iWeaponIndex) )
	{
		ammoIndex = BG_ClipForWeapon(iWeaponIndex);

		if ( !ammoIndex )
		{
			return;
		}

		if ( ammoCount < 0 )
		{
			ammoCount = 0;
		}
		else if ( ammoCount > BG_GetAmmoClipSize(ammoIndex) )
		{
			ammoCount = BG_GetAmmoClipSize(ammoIndex);
		}

		pSelf->client->ps.ammoclip[ammoIndex] = ammoCount;
	}
	else
	{
		ammoIndex = BG_AmmoForWeapon(iWeaponIndex);

		if ( !ammoIndex )
		{
			return;
		}

		if ( ammoCount < 0 )
		{
			ammoCount = 0;
		}
		else if ( ammoCount > BG_GetAmmoTypeMax(ammoIndex) )
		{
			ammoCount = BG_GetAmmoTypeMax(ammoIndex);
		}

		pSelf->client->ps.ammo[ammoIndex] = ammoCount;
	}
}

/*
===============
PlayerCmd_GetWeaponSlotClipAmmo
===============
*/
void PlayerCmd_GetWeaponSlotClipAmmo( scr_entref_t entref )
{
	unsigned short slotName;
	int slot;
	int iWeaponIndex;
	int clipIndex;
	gentity_t *pSelf;

	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		pSelf = &g_entities[entref.entnum];

		if ( pSelf->client == NULL )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}
	else
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
	}

	if ( !ClientPlaying(pSelf) )
	{
		Scr_AddInt(0);
		return;
	}

	slotName = Scr_GetConstString(0);
	slot = BG_GetWeaponSlotForName(SL_ConvertToString(slotName));

	if ( !slot )
	{
		Scr_ParamError(0, va("Unknown weaponslot name %s. Valid weaponslots are \"primary\" and \"primaryb\"", SL_ConvertToString(slotName)));
	}

	iWeaponIndex = pSelf->client->ps.weaponslots[slot];

	if ( !iWeaponIndex )
	{
		Scr_AddInt(0);
		return;
	}

	clipIndex = BG_ClipForWeapon(iWeaponIndex);

	if ( !clipIndex )
	{
		Scr_AddInt(0);
		return;
	}

	Scr_AddInt(pSelf->client->ps.ammoclip[clipIndex]);
}

/*
===============
PlayerCmd_SetWeaponSlotClipAmmo
===============
*/
void PlayerCmd_SetWeaponSlotClipAmmo( scr_entref_t entref )
{
	unsigned short slotName;
	int slot;
	int iWeaponIndex;
	int ammoCount;
	int clipIndex;
	gentity_t *pSelf;

	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		pSelf = &g_entities[entref.entnum];

		if ( pSelf->client == NULL )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}
	else
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
	}

	slotName = Scr_GetConstString(0);
	slot = BG_GetWeaponSlotForName(SL_ConvertToString(slotName));

	if ( !slot )
	{
		Scr_ParamError(0, va("Unknown weaponslot name %s. Valid weaponslots are \"primary\" and \"primaryb\"", SL_ConvertToString(slotName)));
	}

	ammoCount = Scr_GetInt(1);
	iWeaponIndex = pSelf->client->ps.weaponslots[slot];

	if ( !iWeaponIndex )
	{
		Scr_AddInt(0);
		return;
	}

	clipIndex = BG_ClipForWeapon(iWeaponIndex);

	if ( !clipIndex )
	{
		return;
	}

	if ( ammoCount < 0 )
		ammoCount = 0;

	if ( ammoCount > BG_GetAmmoClipSize(clipIndex) )
		ammoCount = BG_GetAmmoClipSize(clipIndex);

	pSelf->client->ps.ammoclip[clipIndex] = ammoCount;
}

/*
===============
PlayerCmd_SetWeaponClipAmmo
===============
*/
void PlayerCmd_SetWeaponClipAmmo( scr_entref_t entref )
{
	const char *weapName;
	int weaponIndex;
	int ammoCount;
	int clipIndex;
	gentity_t *pSelf;

	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		pSelf = &g_entities[entref.entnum];

		if ( pSelf->client == NULL )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}
	else
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
	}

	weapName = Scr_GetString(0);
	ammoCount = Scr_GetInt(1);

	weaponIndex = G_GetWeaponIndexForName(weapName);

	if ( !weaponIndex )
	{
		Scr_AddInt(0);
		return;
	}

	clipIndex = BG_ClipForWeapon(weaponIndex);

	if ( !clipIndex )
	{
		return;
	}

	if ( ammoCount < 0 )
		ammoCount = 0;

	if ( ammoCount > BG_GetAmmoClipSize(clipIndex) )
		ammoCount = BG_GetAmmoClipSize(clipIndex);

	pSelf->client->ps.ammoclip[clipIndex] = ammoCount;
}

/*
===============
iclientprintln
===============
*/
void iclientprintln( scr_entref_t entref )
{
	gentity_t *pSelf;

	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		pSelf = &g_entities[entref.entnum];

		if ( pSelf->client == NULL )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}
	else
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
	}

	Scr_MakeGameMessage(entref.entnum, va("%c", 102));
}

/*
===============
iclientprintlnbold
===============
*/
void iclientprintlnbold( scr_entref_t entref )
{
	gentity_t *pSelf;

	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		pSelf = &g_entities[entref.entnum];

		if ( pSelf->client == NULL )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}
	else
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
	}

	Scr_MakeGameMessage(entref.entnum, va("%c", 103));
}

/*
===============
PlayerCmd_spawn
===============
*/
void PlayerCmd_spawn( scr_entref_t entref )
{
	vec3_t spawn_origin;
	vec3_t spawn_angles;
	gentity_t *pSelf;

	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		pSelf = &g_entities[entref.entnum];

		if ( pSelf->client == NULL )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}
	else
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
	}

	Scr_GetVector(0, spawn_origin);
	Scr_GetVector(1, spawn_angles);

	ClientSpawn(pSelf, spawn_origin, spawn_angles);
}

/*
===============
PlayerCmd_setEnterTime
===============
*/
void PlayerCmd_setEnterTime( scr_entref_t entref )
{
	gentity_t *pSelf;

	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		pSelf = &g_entities[entref.entnum];

		if ( pSelf->client == NULL )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}
	else
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
	}

	pSelf->client->sess.enterTime = Scr_GetInt(0);
}

/*
===============
BodyEnd
===============
*/
void BodyEnd( gentity_t *ent )
{
	ent->s.eFlags &= ~EF_BODY_START;
	ent->r.contents = CONTENTS_CORPSE;
	ent->r.svFlags = 0;
}

/*
===============
PlayerCmd_ClonePlayer
===============
*/
void PlayerCmd_ClonePlayer( scr_entref_t entref )
{
	int deathAnimDuration;
	gentity_t *body;
	gclient_t *client;
	DObj *obj;
	int corpseIndex;
	corpseInfo_t *corpseInfo;
	XAnimTree *tree;
	int i;
	gentity_t *pSelf;

	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		pSelf = &g_entities[entref.entnum];

		if ( pSelf->client == NULL )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}
	else
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
	}

	deathAnimDuration = Scr_GetInt(0);

	client = pSelf->client;
	assert(client);
	assert(client->sess.connected != CON_DISCONNECTED);

	body = G_SpawnPlayerClone();

	body->s.clientNum = client->ps.clientNum;
	body->s.eFlags = client->ps.eFlags & ~EF_TELEPORT_BIT | body->s.eFlags & EF_TELEPORT_BIT | ( EF_DEAD | EF_BODY_START );

	G_SetOrigin(body, client->ps.origin);
	G_SetAngle(body, pSelf->r.currentAngles);

	body->s.pos.trType = TR_GRAVITY;
	body->s.pos.trTime = level.time;

	VectorCopy(client->ps.velocity, body->s.pos.trDelta);
	assert(!IS_NAN((body->s.pos.trDelta)[0]) && !IS_NAN((body->s.pos.trDelta)[1]) && !IS_NAN((body->s.pos.trDelta)[2]));

	body->s.eType = ET_PLAYER_CORPSE;
	body->physicsObject = qtrue;

	obj = Com_GetServerDObj(client->ps.clientNum);
	tree = DObjGetTree(obj);

	for ( i = 0; i < 2; ++i )
	{
		if ( body->s.pos.trDelta[i] > g_clonePlayerMaxVelocity->current.decimal )
		{
			body->s.pos.trDelta[i] = g_clonePlayerMaxVelocity->current.decimal;
		}
	}
	assert(!IS_NAN((body->s.pos.trDelta)[0]) && !IS_NAN((body->s.pos.trDelta)[1]) && !IS_NAN((body->s.pos.trDelta)[2]));

	body->corpse.deathAnimStartTime = level.time;

	corpseIndex = G_GetFreePlayerCorpseIndex();
	corpseInfo = &g_scr_data.playerCorpseInfo[corpseIndex];

	corpseInfo->entnum = body->s.number;

	corpseInfo->time = level.time;
	corpseInfo->falling = qtrue;

	assert(client->ps.clientNum >= 0 && client->ps.clientNum < MAX_CLIENTS);

	memcpy(&corpseInfo->ci, &level_bgs.clientinfo[client->ps.clientNum], sizeof(corpseInfo->ci));
	corpseInfo->ci.pXAnimTree = corpseInfo->tree;

	XAnimCloneAnimTree(tree, corpseInfo->tree);

	body->s.groundEntityNum = ENTITYNUM_NONE;
	assert(!body->r.svFlags);
	body->r.svFlags = SVF_BODY;

	VectorCopy(pSelf->r.mins, body->r.mins);
	VectorCopy(pSelf->r.maxs, body->r.maxs);

	VectorCopy(pSelf->r.absmin, body->r.absmin);
	VectorCopy(pSelf->r.absmax, body->r.absmax);

	body->s.legsAnim = client->ps.legsAnim;
	body->s.torsoAnim = client->ps.torsoAnim;

	body->clipmask = CONTENTS_SOLID | CONTENTS_PLAYERCLIP;
	body->r.contents = CONTENTS_CLIPSHOT | CONTENTS_CORPSE;

	SV_LinkEntity(body);

	body->nextthink = level.time + deathAnimDuration;
	body->handler = ENT_HANDLER_PLAYER_CLONE;

	GScr_AddEntity(body);
}

/*
===============
PlayerCmd_SetClientDvar
===============
*/
void PlayerCmd_SetClientDvar( scr_entref_t entref )
{
	int i;
	int len;
	int type;
	const char *pszDvar;
	char *pszText;
	char *pCh;
	char szString[MAX_STRING_CHARS];
	char szOutString[MAX_STRING_CHARS];
	gentity_t *pSelf;

	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		pSelf = &g_entities[entref.entnum];

		if ( pSelf->client == NULL )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}
	else
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
	}

	pszDvar = Scr_GetString(0);
	type = Scr_GetType(1);

	if ( type == VAR_ISTRING )
	{
		Scr_ConstructMessageString(1, Scr_GetNumParam() - 1, "Client Dvar Value", szString, sizeof(szString));
		pszText = szString;
	}
	else
	{
		pszText = (char *)Scr_GetString(1);
	}

	// strlen'd here but the result is unused (dead)
	// as GScr_SetDvar's inline value-sanitization prologue.
	len = strlen(pszText);

	if ( !Dvar_IsValidName(pszDvar) )
	{
		Scr_Error(va("Dvar %s has an invalid dvar name", pszDvar));
	}
	else
	{
		pCh = szOutString;
		memset(szOutString, 0, sizeof(szOutString));

		for ( i = 0; i < 0x2000 && pszText[i]; i++, pCh++ )
		{
			*pCh = I_CleanChar(pszText[i]);

			if ( *pCh == '\"' )
			{
				*pCh = '\'';
			}
		}

		SV_GameSendServerCommand(entref.entnum, SV_CMD_RELIABLE, va("%c %s \"%s\"", 118, pszDvar, szOutString));
	}
}

/*
===============
PlayerCmd_IsTalking
===============
*/
void PlayerCmd_IsTalking( scr_entref_t entref )
{
	int elapsedTime;
	gentity_t *pSelf;

	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		pSelf = &g_entities[entref.entnum];

		if ( pSelf->client == NULL )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}
	else
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
	}

	elapsedTime = level.time - pSelf->client->lastVoiceTime;

	if ( elapsedTime >= 0 && elapsedTime < g_voiceChatTalkingDuration->current.integer )
	{
		Scr_AddInt(true);
	}
	else
	{
		Scr_AddInt(false);
	}
}

/*
===============
PlayerCmd_FreezeControls
===============
*/
void PlayerCmd_FreezeControls( scr_entref_t entref )
{
	gentity_t *pSelf;

	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		pSelf = &g_entities[entref.entnum];

		if ( pSelf->client == NULL )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}
	else
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
	}

	pSelf->client->bFrozen = Scr_GetInt(0);
}

/*
===============
PlayerCmd_DisableWeapon
===============
*/
void PlayerCmd_DisableWeapon( scr_entref_t entref )
{
	gentity_t *pSelf;

	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		pSelf = &g_entities[entref.entnum];

		if ( pSelf->client == NULL )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}
	else
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
	}

	pSelf->client->ps.pm_flags |= PMF_DISABLEWEAPON;
}

/*
===============
PlayerCmd_EnableWeapon
===============
*/
void PlayerCmd_EnableWeapon( scr_entref_t entref )
{
	gentity_t *pSelf;

	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		pSelf = &g_entities[entref.entnum];

		if ( pSelf->client == NULL )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}
	else
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
	}

	pSelf->client->ps.pm_flags &= ~PMF_DISABLEWEAPON;
}

/*
===============
PlayerCmd_SetReverb
===============
*/
void PlayerCmd_SetReverb( scr_entref_t entref )
{
	unsigned short priorityString;
	int priority;
	const char *roomtype;
	float drylevel;
	float wetlevel;
	float fadetime;
	gentity_t *pSelf;


	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		pSelf = &g_entities[entref.entnum];

		if ( pSelf->client == NULL )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}
	else
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
	}

	fadetime = 0;
	drylevel = 1.0f;
	wetlevel = 0.5f;

	switch ( Scr_GetNumParam() )
	{
	case 5:
		fadetime = Scr_GetFloat(4);

	case 4:
		wetlevel = Scr_GetFloat(3);

	case 3:
		drylevel = Scr_GetFloat(2);

	case 2:
		roomtype = Scr_GetString(1);
		priorityString = Scr_GetConstString(0);
		priority = SND_ENVEFFECTPRIO_LEVEL;

		if ( priorityString == scr_const.snd_enveffectsprio_level )
		{
			priority = SND_ENVEFFECTPRIO_LEVEL;
		}
		else if ( priorityString == scr_const.snd_enveffectsprio_shellshock )
		{
			priority = SND_ENVEFFECTPRIO_SHELLSHOCK;
		}
		else
		{
			Scr_Error("priority must be 'snd_enveffectsprio_level' or 'snd_enveffectsprio_shellshock'\n");
		}

		SV_GameSendServerCommand(entref.entnum, SV_CMD_RELIABLE, va("%c %i \"%s\" %g %g %g", 114, priority, roomtype, drylevel, wetlevel, fadetime));
		break;

	default:
		Scr_Error(
		    "USAGE: player setReverb(\"priority\", \"roomtype\", drylevel = 1.0, wetlevel = 0.5, fadetime = 0);\n"
		    "Valid priorities are \"snd_enveffectsprio_level\" or \"snd_enveffectsprio_shellshock\", dry level is a float f"
		    "rom 0 (no source sound) to 1 (full source sound), wetlevel is a float from 0 (no effect) to 1 (full effect), f"
		    "adetime is in sec and modifies drylevel and wetlevel\n");
		break;
	}

}

/*
===============
PlayerCmd_DeactivateReverb
===============
*/
void PlayerCmd_DeactivateReverb( scr_entref_t entref )
{
	unsigned short priorityString;
	int priority;
	float fadetime;
	gentity_t *pSelf;

	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		pSelf = &g_entities[entref.entnum];

		if ( pSelf->client == NULL )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}
	else
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
	}

	fadetime = 0;

	switch ( Scr_GetNumParam() )
	{
	case 2:
		fadetime = Scr_GetFloat(1);

	case 1:

		priorityString = Scr_GetConstString(0);
		priority = SND_ENVEFFECTPRIO_LEVEL;

		if ( priorityString == scr_const.snd_enveffectsprio_level )
		{
			priority = SND_ENVEFFECTPRIO_LEVEL;
		}
		else if ( priorityString == scr_const.snd_enveffectsprio_shellshock )
		{
			priority = SND_ENVEFFECTPRIO_SHELLSHOCK;
		}
		else
		{
			Scr_Error("priority must be \'snd_enveffectsprio_level\' or \'snd_enveffectsprio_shellshock\'\n");
		}

		SV_GameSendServerCommand(entref.entnum, SV_CMD_RELIABLE, va("%c %i \"%s\" %g %g %g", 68, priority, fadetime));
		break;

	default:
		Scr_Error(
		    "USAGE: player deactivateReverb(\"priority\", fadetime = 0);\n"
		    "Valid priorities are \"snd_enveffectsprio_level\" or \"snd_enveffectsprio_shellshock\", fadetime is the time spe"
		    "nt fading to the next lowest active reverb priority level in seconds\n");
		break;
	}

}

/*
===============
PlayerCmd_SetChannelVolumes
===============
*/
void PlayerCmd_SetChannelVolumes( scr_entref_t entref )
{
	unsigned short priorityString;
	int priority;
	int shockIndex;
	float fadetime;
	gentity_t *pSelf;


	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		pSelf = &g_entities[entref.entnum];

		if ( pSelf->client == NULL )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}
	else
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
	}

	fadetime = 0;

	switch ( Scr_GetNumParam() )
	{
	case 3:
		fadetime = Scr_GetFloat(2);

	case 2:
		shockIndex = G_FindConfigstringIndex(Scr_GetString(1), CS_SHELLSHOCKS, MAX_SHELLSHOCKS, qfalse, NULL);
		priorityString = Scr_GetConstString(0);
		priority = SND_CHANNELVOLPRIO_HOLDBREATH;

		if ( priorityString == scr_const.snd_channelvolprio_holdbreath )
		{
			priority = SND_CHANNELVOLPRIO_HOLDBREATH;
		}
		else if ( priorityString == scr_const.snd_channelvolprio_pain )
		{
			priority = SND_CHANNELVOLPRIO_PAIN;
		}
		else if ( priorityString == scr_const.snd_channelvolprio_shellshock )
		{
			priority = SND_CHANNELVOLPRIO_SHELLSHOCK;
		}
		else
		{
			Scr_Error("priority must be 'snd_channelvolprio_holdbreath', 'snd_channelvolprio_pain', or 'snd_channelvolprio_shellshock'\n");
		}

		SV_GameSendServerCommand(entref.entnum, SV_CMD_RELIABLE, va("%c %i %i %g", 69, priority, shockIndex, fadetime));
		break;

	default:
		Scr_Error(
		    "USAGE: player setchannelvolumes(\"priority\", \"shock name\", fadetime = 0);\n"
		    "Valid priorities are \"snd_channelvolprio_holdbreath\", \"snd_channelvolprio_pain\", or \"snd_channelvolprio_she"
		    "llshock\", fadetime is in sec\n");
		break;
	}

}

/*
===============
PlayerCmd_DeactivateChannelVolumes
===============
*/
void PlayerCmd_DeactivateChannelVolumes( scr_entref_t entref )
{
	unsigned short priorityString;
	int priority;
	float fadetime;
	gentity_t *pSelf;


	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		pSelf = &g_entities[entref.entnum];

		if ( pSelf->client == NULL )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}
	else
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
	}

	fadetime = 0;

	switch ( Scr_GetNumParam() )
	{
	case 2:
		fadetime = Scr_GetFloat(1);

	case 1:
		priorityString = Scr_GetConstString(0);
		priority = SND_CHANNELVOLPRIO_HOLDBREATH;

		if ( priorityString == scr_const.snd_channelvolprio_holdbreath )
		{
			priority = SND_CHANNELVOLPRIO_HOLDBREATH;
		}
		else if ( priorityString == scr_const.snd_channelvolprio_pain )
		{
			priority = SND_CHANNELVOLPRIO_PAIN;
		}
		else if ( priorityString == scr_const.snd_channelvolprio_shellshock )
		{
			priority = SND_CHANNELVOLPRIO_SHELLSHOCK;
		}
		else
		{
			Scr_Error("priority must be \'snd_channelvolprio_holdbreath\', \'snd_channelvolprio_pain\', or \'snd_channelvolprio_shellshock\'\n");
		}

		SV_GameSendServerCommand(entref.entnum, SV_CMD_RELIABLE, va("%c %i \"%s\" %g %g %g", 70, priority, fadetime));
		break;

	default:
		Scr_Error(
		    "USAGE: player deactivatechannelvolumes(\"priority\", fadetime = 0);\n"
		    "Valid priorities are \"snd_channelvolprio_holdbreath\", \"snd_channelvolprio_pain\", or \"snd_channelvolprio_she"
		    "llshock\", fadetime is the time spent fading to the next lowest active reverb priority level in seconds\n");
		break;
	}

}

/*
===============
ScrCmd_IsLookingAt
===============
*/
void ScrCmd_IsLookingAt( scr_entref_t entref )
{
	gentity_t *pSelf;

	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		pSelf = &g_entities[entref.entnum];

		if ( pSelf->client == NULL )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}
	else
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
	}

	Scr_AddInt( pSelf->client->pLookatEnt == Scr_GetEntity(0) );
}

/*
===============
ScrCmd_PlayLocalSound
===============
*/
void ScrCmd_PlayLocalSound( scr_entref_t entref )
{
	const char *aliasName;
	gentity_t *pSelf;

	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		pSelf = &g_entities[entref.entnum];

		if ( pSelf->client == NULL )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}
	else
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
	}

	aliasName = Scr_GetString(0);

	// truncates the alias index to a byte before formatting it
	SV_GameSendServerCommand(entref.entnum, SV_CMD_CAN_IGNORE, va("%c %i", 115, (unsigned char)G_SoundAliasIndex(aliasName)));
}

/*
===============
PlayerCmd_SayAll
===============
*/
void PlayerCmd_SayAll( scr_entref_t entref )
{
	char szString[MAX_STRING_CHARS];
	gentity_t *pSelf;

	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		pSelf = &g_entities[entref.entnum];

		if ( pSelf->client == NULL )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}
	else
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
	}

	Scr_ConstructMessageString(0, Scr_GetNumParam() - 1, "Client Chat Message", &szString[1], sizeof(szString) - 1);
	szString[0] = 20;

	G_Say(pSelf, NULL, SAY_ALL, szString);
}

/*
===============
PlayerCmd_SayTeam
===============
*/
void PlayerCmd_SayTeam( scr_entref_t entref )
{
	char szString[MAX_STRING_CHARS];
	gentity_t *pSelf;

	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		pSelf = &g_entities[entref.entnum];

		if ( pSelf->client == NULL )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}
	else
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
	}

	Scr_ConstructMessageString(0, Scr_GetNumParam() - 1, "Client Chat Message", &szString[1], sizeof(szString) - 1);
	szString[0] = 20;

	G_Say(pSelf, NULL, SAY_TEAM, szString);
}

/*
===============
PlayerCmd_AllowSpectateTeam
===============
*/
void PlayerCmd_AllowSpectateTeam( scr_entref_t entref )
{
	unsigned short teamString;
	int teamBit;
	int allow;
	gentity_t *pSelf;

	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		pSelf = &g_entities[entref.entnum];

		if ( pSelf->client == NULL )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}
	else
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
	}

	teamString = Scr_GetConstString(0);
	teamBit = 0;

	if ( teamString == scr_const.axis )
	{
		teamBit = 2;
	}
	else if ( teamString == scr_const.allies )
	{
		teamBit = 4;
	}
	else if ( teamString == scr_const.none )
	{
		teamBit = 1;
	}
	else if ( teamString == scr_const.freelook )
	{
		teamBit = 16;
	}
	else
	{
		Scr_ParamError(0, "team must be \"axis\", \"allies\", \"none\", or \"freelook\"");
	}

	allow = Scr_GetInt(1);

	if ( allow )
	{
		pSelf->client->sess.noSpectate &= ~teamBit;
	}
	else
	{
		pSelf->client->sess.noSpectate |= teamBit;
	}
}

/*
===============
PlayerCmd_GetGuid
===============
*/
void PlayerCmd_GetGuid( scr_entref_t entref )
{
	gentity_t *pSelf;

	if ( entref.classnum == CLASS_NUM_ENTITY )
	{
		pSelf = &g_entities[entref.entnum];

		if ( pSelf->client == NULL )
		{
			Scr_ObjectError(va("entity %i is not a player", entref.entnum));
		}
	}
	else
	{
		Scr_ObjectError("not an entity");
		pSelf = NULL;
	}

	if ( Scr_GetNumParam() )
	{
		Scr_Error("USAGE: self getGuid()\n");
	}

	Scr_AddInt(SV_GetGuid(entref.entnum));
}

/*
===============
Player_GetMethod
===============
*/
void (*Player_GetMethod( const char **pName ))( scr_entref_t )
{
	int i;
	const char *name;

	name = *pName;

	for ( i = 0; i < ARRAY_COUNT( player_methods ); i++ )
	{
		if ( !strcmp(name, player_methods[i].name) )
		{
			*pName = player_methods[i].name;
			return player_methods[i].call;
		}
	}

	return NULL;
}

