#include "../qcommon/qcommon.h"
#include "bg_public.h"

int BG_WeaponShotCount( int weapIndex );
qboolean BG_SetPlayerWeaponForSlot( playerState_t *ps, int slot, int weaponIndex );


int bg_iNumWeapons = 0;
static WeaponDef *bg_weaponDefs[MAX_WEAPONS];

static WeaponDef *bg_weapAmmoTypes[MAX_WEAPONS];
static int bg_iNumAmmoTypes;

static WeaponDef *bg_sharedAmmoCaps[MAX_WEAPONS];
static int bg_iNumSharedAmmoCaps;

static WeaponDef *bg_weapClips[MAX_WEAPONS];
static int bg_iNumWeapClips;

WeaponDef *BG_GetWeaponDef( int iWeapon );
int BG_GetNumWeapons();
int BG_GetWeaponIndexForName( const char *name, int (*regWeap)(int) );
int CG_GetWeaponIndexForName( const char *name );
int BG_GetFirstEquippedOffhand( const playerState_t *ps, int offhandSlot );
int BG_IsPlayerWeaponInSlot( playerState_t *pPS, int iWeaponIndex, qboolean bAnyMode );
int BG_ClipForWeapon( int weapon );
int BG_AmmoForWeapon( int weapon );
int BG_WeaponIsClipOnly( int weapon );
int BG_WeaponAmmo( const playerState_t *ps, int weapon );
void PM_Weapon_Idle( playerState_t *ps );
void PM_ExitBinocularsQuick( playerState_t *ps );
WeaponDef *BG_LoadWeaponDef( const char *folder, const char *name );

void BG_ShutdownWeaponDefFiles();
void BG_FillInWeaponItems( int weapIndex );
void BG_FillInAmmoItems( int (*regWeap)(int) );
void BG_SetupAmmoIndexes( int weapIndex );
void BG_SetupSharedAmmoIndexes( int weapIndex );
void BG_SetupClipIndexes( int weapIndex );
void BG_SetupWeaponAlts( int weapIndex, int (*regWeap)(int) );
void BG_ClearWeaponDef();
void BG_SetupWeaponIndex( int weapIndex );
int BG_SetupWeaponDef( WeaponDef *weapDef, int (*regWeap)(int) );
void BG_FillInAllWeaponItems();
void PM_StartWeaponAnim( playerState_t *ps, int anim );
void PM_ContinueWeaponAnim( playerState_t *ps, int anim );
WeaponDef *BG_GetWeaponDef( int iWeapon );
void CG_SetWeaponDefToDefaultWeapon( int iWeapon );
int BG_GetNumWeapons();
int BG_GetNumAmmoTypes();
int BG_GetAmmoTypeMax( int iAmmoIndex );
int BG_GetNumAmmoClips();
int BG_GetAmmoClipSize( int iClipIndex );
int BG_GetSharedAmmoCapSize( int iAmmoIndex );
const char *BG_GetSharedAmmoCapName( int iAmmoIndex );
const char *BG_GetAmmoTypeName( int iAmmoIndex );
const char *BG_GetAmmoClipName( int iClipIndex );
int BG_GetAmmoTypeForName( const char *name );
int BG_GetAmmoClipForName( const char *name );
int BG_FindWeaponIndexForName( const char *name );
int BG_GetWeaponIndexForName( const char *name, int (*regWeap)(int) );
int CG_GetWeaponIndexForName( const char *name );
int BG_GetBinocularWeaponIndex();
int BG_GetViewmodelWeaponIndex( const playerState_t *ps );
int BG_GetFirstAvailableOffhand( const playerState_t *ps, int offhandSlot );
int BG_GetFirstEquippedOffhand( const playerState_t *ps, int offhandSlot );
qboolean BG_IsAimDownSightWeapon( int iWeapon );
bool BG_ValidateWeaponNumber( int weaponIndex );
bool BG_DoesWeaponRequireSlot( int weaponIndex );
bool BG_IsAnyEmptyPrimaryWeaponSlot( const playerState_t *ps );
bool BG_IsWeaponValid( const playerState_t *ps, int weaponIndex );
bool BG_DoesWeaponNeedSlot( int weaponIndex );
int BG_TakePlayerWeapon( playerState_t *pPS, int iWeaponIndex );
int BG_IsPlayerWeaponInSlot( playerState_t *pPS, int iWeaponIndex, qboolean bAnyMode );
int BG_GetEmptySlotForWeapon( playerState_t *pPS, int iWeaponIndex );
int BG_GetStackSlotForWeapon( const playerState_t *pPS, int iWeaponIndex, unsigned int preferedSlot );
int BG_GetMaxPickupableAmmo( const playerState_t *ps, int weaponIndex );
int BG_GetTotalAmmoReserve( const playerState_t *ps, int weaponIndex );
void BG_GetSpreadForWeapon( const playerState_t *ps, int weaponIndex, float *minSpread, float *maxSpread );
bool PM_IsBinocularADSAllowed( int iWeaponState );
bool PM_IsAdsAllowed( playerState_t *ps, pml_t *pml );
int PM_IsBinocularsADS( const playerState_t *ps );
void PM_UpdateAimDownSightFlag( pmove_t *pm, pml_t *pml );
void PM_ExitAimDownSight( playerState_t *ps );
void PM_UpdateAimDownSightLerp( pmove_t *pm, pml_t *pml );
void PM_StartHoldBreath( playerState_t *ps );
void PM_EndHoldBreath( playerState_t *ps );
void PM_HoldBreathFire( playerState_t *ps );
void PM_UpdateHoldBreath( pmove_t *pm, pml_t *pml );
qboolean PM_InteruptWeaponWithProneMove( playerState_t *ps );
int BG_ClipForWeapon( int weapon );
int BG_AmmoForWeapon( int weapon );
int BG_WeaponIsClipOnly( int weapon );
int BG_WeaponAmmo( const playerState_t *ps, int weapon );
void PM_ReloadClip( playerState_t *ps );
void PM_WeaponUseAmmo( playerState_t *ps, int weaponIndex, int amount );
int PM_WeaponAmmoAvailable( playerState_t *ps );
qboolean PM_WeaponClipEmpty( playerState_t *ps );
void PM_Weapon_FinishRechamber( playerState_t *ps );
qboolean PM_Weapon_CheckForRechamber( playerState_t *ps, qboolean delayedAction );
void PM_SetWeaponReloadAddAmmoDelay( playerState_t *ps );
void PM_SetReloadingState( playerState_t *ps );
void PM_BeginWeaponReload( playerState_t *ps );
void PM_BeginWeaponChange( playerState_t *ps, int newweapon );
void PM_Weapon_FinishWeaponChange( pmove_t *pm );
void PM_Weapon_FinishWeaponRaise( playerState_t *ps );
qboolean PM_Weapon_AllowReload( playerState_t *ps );
void PM_Weapon_ReloadDelayedAction( playerState_t *ps );
void PM_Weapon_FinishReloadStart( playerState_t *ps, qboolean delayedAction );
void PM_Weapon_FinishReload( playerState_t *ps, qboolean delayedAction );
void PM_Weapon_FinishReloadEnd( playerState_t *ps );
void PM_Weapon_CheckForReload( pmove_t *pm );
void PM_SwitchIfEmpty( playerState_t *ps );
void PM_AdjustAimSpreadScale( pmove_t *pm, pml_t *pml );
qboolean PM_Weapon_WeaponTimeAdjust( pmove_t *pm, pml_t *pml );
void PM_Weapon_CheckForChangeWeapon( pmove_t *pm );
qboolean PM_Weapon_FinishFiring( pmove_t *pm, qboolean delayedAction );
void PM_Weapon_StartFiring( playerState_t *ps, qboolean delayedAction );
qboolean PM_Weapon_CheckFiringAmmo( playerState_t *ps );
void PM_Weapon_SetFPSFireAnim( playerState_t *ps );
void PM_Weapon_AddFiringAimSpreadScale( playerState_t *ps );
void PM_Weapon_FireWeapon( playerState_t *ps, qboolean delayedAction );
void PM_Weapon_FireMelee( playerState_t *ps );
void PM_Weapon_FinishMelee( playerState_t *ps );
void PM_Weapon_CheckForMelee( pmove_t *pm, qboolean delayedAction );
void PM_Weapon_Idle( playerState_t *ps );
void PM_Weapon_OffHandInit( playerState_t *ps );
void PM_Weapon_OffHandPrepare( playerState_t *ps );
void PM_Weapon_OffHandHold( playerState_t *ps );
void PM_Weapon_OffHand( pmove_t *pm );
void PM_Weapon_OffHandEnd( playerState_t *ps );
void PM_SendEmtpyOffhandEvent( playerState_t *ps, int offhandSlot );
void PM_Weapon_CheckForOffHand( pmove_t *pm );
bool PM_UpdateGrenadeThrow( playerState_t *ps, pml_t *pml );
void PM_EnterBinoculars( playerState_t *ps );
void PM_ExitBinoculars( playerState_t *ps );
void PM_ExitBinocularsQuick( playerState_t *ps );
void Binocular_State_Init( playerState_t *ps );
void Binocular_State_Raise( playerState_t *ps );
void Binocular_State_Hold( playerState_t *ps );
void Binocular_State_Exit_ADS( playerState_t *ps );
void Binocular_State_Drop( playerState_t *ps );
void Binocular_State_End( pmove_t *pm );
void PM_Weapon_CheckForBinoculars( pmove_t *pm );
void PM_Weapon( pmove_t *pm, pml_t *pml );
void PM_ResetWeaponState( playerState_t *ps );
void BG_WeaponFireRecoil( playerState_t *ps, vec3_t vGunSpeed, vec3_t kickAVel );
float BG_GetBobCycle(const playerState_t *ps);
float BG_GetVerticalBobFactor( const playerState_t *ps, float cycle, float speed, float maxAmp );
float BG_GetHorizontalBobFactor(const playerState_t *ps, float cycle, float speed, float maxAmp);
void BG_CalculateWeaponPosition_BasePosition_angles( weaponState_t *ws, vec3_t angles );
void BG_CalculateWeaponPosition_BaseAngles(weaponState_t *ws, vec3_t angles);
void BG_CalculateWeaponPosition_IdleAngles(weaponState_t *ws, vec3_t angles);
void BG_CalculateWeaponPosition_BobOffset(weaponState_t *ws, vec3_t angles);
void BG_CalculateWeaponPosition_DamageKick(weaponState_t *ws, vec3_t angles);
void BG_CalculateWeaponPosition_GunRecoil( weaponState_t *ws, vec3_t angles );
void BG_CalculateWeaponAngles( weaponState_t *ws, vec3_t angles );
void BG_CalculateView_DamageKick(viewState_t *vs, vec3_t angles);
void BG_CalculateView_IdleAngles(viewState_t *vs, vec3_t angles);
void BG_CalculateView_BobAngles(viewState_t *vs, vec3_t angles);
void BG_CalculateView_Velocity(viewState_t *vs, vec3_t angles);
void BG_CalculateViewAngles(viewState_t *vs, vec3_t angles);
void BG_CalculateWeaponPosition_Sway( const playerState_t *ps, vec3_t swayViewAngles, vec3_t swayOffset, vec3_t swayAngles, float ssSwayScale, int frametime );
WeaponDef *BG_LoadWeaponDef( const char *folder, const char *name );

/*
===============
BG_GetFirstEquippedOffhand
===============
*/

/*
===============
BG_GetFirstAvailableOffhand
===============
*/

/*
===============
BG_CalculateWeaponPosition_GunRecoil_SingleAngle
===============
*/
qboolean BG_CalculateWeaponPosition_GunRecoil_SingleAngle( float *fOffset, float *speed, float fTimeStep,
        float fOfsCap, float fGunKickAccel, float fGunKickSpeedMax, float fGunKickSpeedDecay, float fGunKickStaticDecay )
{
	qboolean bCanStop;
	bCanStop = qfalse;
	if ( I_fabs(*fOffset) < 0.25f && I_fabs(*speed) < 1.0f )
	{
		*fOffset = 0;
		*speed = 0;
		bCanStop = 1;
	}
	else
	{
	*fOffset = *fOffset + *speed * fTimeStep;
	if ( *fOffset > fOfsCap )
	{
		*fOffset = fOfsCap;
		if ( *speed > 0 )
		{
			*speed = 0;
		}
	}
	else if ( *fOffset < -fOfsCap )
	{
		*fOffset = -fOfsCap;
		if ( *speed < 0 )
		{
			*speed = 0;
		}
	}
	if ( *fOffset > 0 )
	{
		*speed = *speed - fGunKickAccel * fTimeStep;
	}
	else if ( *fOffset < 0 )
	{
		*speed = *speed + fGunKickAccel * fTimeStep;
	}
	*speed = *speed - *speed * fGunKickSpeedDecay * fTimeStep;
	if ( *speed > 0 )
	{
		*speed = *speed - fGunKickStaticDecay * fTimeStep;
		if ( *speed < 0 )
		{
			*speed = 0;
		}
	}
	else
	{
		*speed = *speed + fGunKickStaticDecay * fTimeStep;
		if ( *speed > 0 )
		{
			*speed = 0;
		}
	}
	if ( *speed > fGunKickSpeedMax )
	{
		*speed = fGunKickSpeedMax;
	}
	else if ( *speed < -fGunKickSpeedMax )
	{
		*speed = -fGunKickSpeedMax;
	}
	}
	return bCanStop;
}

/*
===============
BG_ShutdownWeaponDefFiles
===============
*/
void BG_ShutdownWeaponDefFiles()
{
	bg_iNumWeapons = 0;
}

/*
===============
BG_FillInWeaponItems
===============
*/
void BG_FillInWeaponItems( int weapIndex )
{
	WeaponDef *weapDef;
	gitem_t *item;

	weapDef = BG_GetWeaponDef(weapIndex);
	item = &bg_itemlist[weapIndex];

	item->pickup_sound = weapDef->pickupSound;

	item->world_model[0] = weapDef->worldModel;
	item->world_model[1] = NULL;

	item->icon = weapDef->hudIcon;
	item->pickup_name = weapDef->displayName;
	item->quantity = weapDef->startAmmo;

	item->giType = IT_WEAPON;
	item->giTag = weapIndex;

	item->giAmmoIndex = weapDef->ammoIndex;
	item->giClipIndex = weapDef->clipIndex;
}

/*
===============
BG_FillInAmmoItems
===============
*/
void BG_FillInAmmoItems( int (*regWeap)(int) )
{
	int itemIdx;
	int weapIndex;
	WeaponDef *weapDef;
	gitem_t *item;
	char *c;
	char name[MAX_QPATH];

	for ( itemIdx = 129; itemIdx < bg_numItems; itemIdx++ )
	{
		item = &bg_itemlist[itemIdx];

		if ( item->giType != IT_AMMO )
		{
			continue;
		}

		I_strncpyz(name, item->pickup_name, sizeof(name));

		for ( c = name; *c; c++ )
		{
			if ( *c != ' ' )
			{
				continue;
			}

			*c = 0;
			break;
		}

		weapIndex = BG_GetWeaponIndexForName(name, regWeap);

		if ( weapIndex )
		{
			weapDef = BG_GetWeaponDef(weapIndex);

			item->giTag = weapIndex;
			item->giAmmoIndex = weapDef->ammoIndex;
			item->giClipIndex = weapDef->clipIndex;
			return;
		}
		else
		{
			Com_Printf("^3WARNING^7: Could not find weapon for ammo item %s\n", item->pickup_name);
			weapIndex = BG_GetWeaponIndexForName("defaultweapon_mp", regWeap);

			if ( !weapIndex )
			{
				Com_Error(ERR_DROP, "\x15" "BG_FillInAmmoItems: weapon def for default weapon could not be found!");
			}

			weapDef = BG_GetWeaponDef(weapIndex);

			item->giTag = WP_DEFAULTWEAPON;
			item->giAmmoIndex = weapDef->ammoIndex;
			item->giClipIndex = weapDef->clipIndex;
		}
	}
}

/*
===============
BG_SetupAmmoIndexes
===============
*/
void BG_SetupAmmoIndexes( int weapIndex )
{
	WeaponDef *weapDef;
	int ammoIndex;
	int altWeapIndex;
	WeaponDef *altWeapDef;

	weapDef = BG_GetWeaponDef(weapIndex);

	for ( ammoIndex = 0; ammoIndex < bg_iNumAmmoTypes; ammoIndex++ )
	{
		if ( strcmp(bg_weapAmmoTypes[ammoIndex]->ammoName, weapDef->ammoName) )
		{
			continue;
		}

	weapDef->ammoIndex = ammoIndex;

		if ( bg_weapAmmoTypes[ammoIndex]->maxAmmo != weapDef->maxAmmo && ammoIndex )
		{
			for ( altWeapIndex = 1; altWeapIndex < weapIndex; altWeapIndex++ )
			{
				altWeapDef = bg_weaponDefs[altWeapIndex];

				if ( !I_stricmp(bg_weapAmmoTypes[ammoIndex]->ammoName, altWeapDef->ammoName) && altWeapDef->maxAmmo == bg_weapAmmoTypes[ammoIndex]->maxAmmo )
				{
					Com_Error(
					    ERR_DROP,
					    "\x15Max ammo mismatch for \"%s\" ammo: '%s\" set it to %i, but \"%s\" already set it to %i.\n",
					    weapDef->ammoName,
					    weapDef->szInternalName,
					    weapDef->maxAmmo,
					    altWeapDef->szInternalName,
					    altWeapDef->maxAmmo);
				}
			}
		}
		return;
	}

	bg_weapAmmoTypes[ammoIndex] = weapDef;
	weapDef->ammoIndex = ammoIndex;
	bg_iNumAmmoTypes++;
}

/*
===============
BG_SetupSharedAmmoIndexes
===============
*/
void BG_SetupSharedAmmoIndexes( int weapIndex )
{
	int sharedAmmoIndex;
	WeaponDef *weapDef;
	int altWeapIndex;
	WeaponDef *altWeapDef;

	weapDef = BG_GetWeaponDef(weapIndex);

	weapDef->sharedAmmoCapIndex = -1;

	if ( !*weapDef->sharedAmmoCapName )
	{
		return;
	}

	Com_DPrintf("%s: %s\n", weapDef->szInternalName, weapDef->sharedAmmoCapName);

	for ( sharedAmmoIndex = 0; sharedAmmoIndex < bg_iNumSharedAmmoCaps; sharedAmmoIndex++ )
	{
		if ( I_stricmp(bg_sharedAmmoCaps[sharedAmmoIndex]->sharedAmmoCapName, weapDef->sharedAmmoCapName) )
		{
			continue;
		}

	weapDef->sharedAmmoCapIndex = sharedAmmoIndex;

		if ( bg_sharedAmmoCaps[sharedAmmoIndex]->sharedAmmoCap != weapDef->sharedAmmoCap && sharedAmmoIndex )
		{
			for ( altWeapIndex = 1; altWeapIndex < weapIndex; altWeapIndex++ )
			{
				altWeapDef = bg_weaponDefs[altWeapIndex];

				if ( !I_stricmp(bg_sharedAmmoCaps[sharedAmmoIndex]->sharedAmmoCapName, altWeapDef->sharedAmmoCapName) && altWeapDef->sharedAmmoCap == bg_sharedAmmoCaps[sharedAmmoIndex]->sharedAmmoCap )
				{
					Com_Error(ERR_DROP,
					          "\x15Shared ammo cap mismatch for \"%s\" shared ammo cap: '%s\" set it to %i, but \"%s\" already set it to %i.\n",
					          weapDef->sharedAmmoCapName,
					          weapDef->szInternalName,
					          weapDef->sharedAmmoCap,
					          altWeapDef->szInternalName,
					          altWeapDef->sharedAmmoCap);
				}
			}
		}
		return;
	}

	bg_sharedAmmoCaps[sharedAmmoIndex] = weapDef;
	weapDef->sharedAmmoCapIndex = sharedAmmoIndex;
	bg_iNumSharedAmmoCaps++;
}

/*
===============
BG_SetupClipIndexes
===============
*/
void BG_SetupClipIndexes( int weapIndex )
{
	int clipIndex;
	WeaponDef *weapDef;
	int altWeapIndex;
	WeaponDef *altWeapDef;

	weapDef = BG_GetWeaponDef(weapIndex);

	for ( clipIndex = 0; clipIndex < bg_iNumWeapClips; clipIndex++ )
	{
		if ( strcmp(bg_weapClips[clipIndex]->clipName, weapDef->clipName) )
		{
			continue;
		}

	weapDef->clipIndex = clipIndex;

		if ( bg_weapClips[clipIndex]->clipSize != weapDef->clipSize && clipIndex )
		{
			for ( altWeapIndex = 1; altWeapIndex < weapIndex; altWeapIndex++ )
			{
				altWeapDef = bg_weaponDefs[altWeapIndex];

				if ( !I_stricmp(bg_weapClips[clipIndex]->clipName, altWeapDef->clipName) && altWeapDef->clipSize == bg_weapClips[clipIndex]->clipSize )
				{
					Com_Error(ERR_DROP,
					          "\x15" "Clip Size mismatch for \"%s\" clip: '%s\" set it to %i, but \"%s\" already set it to %i.\n",
					          weapDef->ammoName,
					          weapDef->szInternalName,
					          weapDef->clipSize,
					          altWeapDef->szInternalName,
					          altWeapDef->clipSize);
				}
			}
		}
		return;
	}

	bg_weapClips[clipIndex] = weapDef;
	weapDef->clipIndex = clipIndex;
	bg_iNumWeapClips++;
}

/*
===============
BG_SetupWeaponAlts
===============
*/
void BG_SetupWeaponAlts( int weapIndex, int (*regWeap)(int) )
{
	int altWeapIndex;
	WeaponDef *weapDef;
	WeaponDef *altWeapDef;

	weapDef = BG_GetWeaponDef(weapIndex);
	weapDef->altWeaponIndex = 0;

	if ( !*weapDef->altWeapon )
		return;

	altWeapIndex = BG_GetWeaponIndexForName(weapDef->altWeapon, regWeap);

	if ( !altWeapIndex )
		Com_Error(ERR_DROP, "\x15" "could not find altWeapon '%s' for weapon '%s'", weapDef->altWeapon, weapDef->szInternalName);

	weapDef->altWeaponIndex = altWeapIndex;
	altWeapDef = bg_weaponDefs[altWeapIndex];

	if ( weapDef->weaponSlot != altWeapDef->weaponSlot )
		Com_Error(ERR_DROP, "\x15" "weapon '%s' does not have same weaponSlot setting as its alt weapon '%s'", weapDef->szInternalName, altWeapDef->szInternalName);

	if ( weapDef->slotStackable != altWeapDef->slotStackable )
		Com_Error(ERR_DROP, "\x15" "weapon '%s' does not have same slotStackable setting as its alt weapon '%s'", weapDef->szInternalName, altWeapDef->szInternalName);
}

/*
===============
BG_ClearWeaponDef
===============
*/
void BG_ClearWeaponDef()
{
	int itemIdx;

	bg_weaponDefs[0] = BG_LoadDefaultWeaponDef();

	bg_weapAmmoTypes[0] = bg_weaponDefs[0];
	bg_iNumAmmoTypes = 1;

	bg_sharedAmmoCaps[0] = bg_weaponDefs[0];
	bg_iNumSharedAmmoCaps = 1;

	bg_weapClips[0] = bg_weaponDefs[0];
	bg_iNumWeapClips = 1;

	for ( itemIdx = 1; itemIdx <= MAX_WEAPONS; itemIdx++ )
	{
		bg_itemlist[itemIdx].giType = IT_BAD;
	}

	BG_LoadPlayerAnimTypes();
	BG_InitWeaponStrings();
}

/*
===============
BG_SetupWeaponIndex
===============
*/
void BG_SetupWeaponIndex( int weapIndex )
{
	BG_SetupAmmoIndexes(weapIndex);
	BG_SetupSharedAmmoIndexes(weapIndex);
	BG_SetupClipIndexes(weapIndex);
	BG_FillInWeaponItems(weapIndex);
}

/*
===============
BG_SetupWeaponDef
===============
*/
int BG_SetupWeaponDef( WeaponDef *weapDef, int (*regWeap)(int) )
{
	int weapIndex;

	bg_iNumWeapons++;
	weapIndex = bg_iNumWeapons;

	bg_weaponDefs[weapIndex] = weapDef;

	BG_SetupWeaponIndex(weapIndex);
	BG_SetupWeaponAlts(weapIndex, regWeap);

	if ( regWeap )
	{
		regWeap(weapIndex);
	}

	return weapIndex;
}

/*
===============
BG_FillInAllWeaponItems
===============
*/
void BG_FillInAllWeaponItems()
{
	int weaponIndex;
	int weaponCount;

	weaponCount = BG_GetNumWeapons();

	for ( weaponIndex = 1; weaponIndex <= weaponCount; weaponIndex++ )
		BG_SetupWeaponIndex(weaponIndex);
}

/*
===============
PM_StartWeaponAnim
===============
*/
void PM_StartWeaponAnim( playerState_t *ps, int anim )
{
	assert(ps);

	if ( ps->pm_type >= PM_DEAD )
	{
		return;
	}

	ps->weapAnim = ( anim | ( ps->weapAnim & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT );
}

/*
===============
PM_ContinueWeaponAnim
===============
*/
void PM_ContinueWeaponAnim( playerState_t *ps, int anim )
{
	assert(ps);

	if ( (ps->weapAnim & ~ANIM_TOGGLEBIT) == anim )
	{
		return;
	}

	PM_StartWeaponAnim(ps, anim);
}

/*
===============
BG_GetWeaponDef
===============
*/
WeaponDef *BG_GetWeaponDef( int iWeapon )
{
	return bg_weaponDefs[iWeapon];
}

/*
===============
CG_SetWeaponDefToDefaultWeapon
===============
*/
void CG_SetWeaponDefToDefaultWeapon( int iWeapon )
{
	int animIter;
	int weapIndex;

	weapIndex = CG_GetWeaponIndexForName("defaultweapon_mp");

	if ( !weapIndex )
	{
		Com_Error(ERR_DROP, "\x15" "CG_SetWeaponDefToDefaultWeapon: weapon def for default weapon could not be found!");
	}

	bg_weaponDefs[iWeapon]->gunModel = bg_weaponDefs[weapIndex]->gunModel;
	bg_weaponDefs[iWeapon]->handModel = bg_weaponDefs[weapIndex]->handModel;

	for ( animIter = 0; animIter < 23; animIter++ )
	{
		bg_weaponDefs[iWeapon]->XAnims[animIter] = bg_weaponDefs[weapIndex]->XAnims[animIter];
	}
}

/*
===============
BG_GetNumWeapons
===============
*/
int BG_GetNumWeapons()
{
	return bg_iNumWeapons;
}

/*
===============
BG_GetNumAmmoTypes
===============
*/
int BG_GetNumAmmoTypes()
{
	return bg_iNumAmmoTypes;
}

/*
===============
BG_GetAmmoTypeMax
===============
*/
int BG_GetAmmoTypeMax( int iAmmoIndex )
{
	return bg_weapAmmoTypes[iAmmoIndex]->maxAmmo;
}

/*
===============
BG_GetNumAmmoClips
===============
*/
int BG_GetNumAmmoClips()
{
	return bg_iNumWeapClips;
}

/*
===============
BG_GetAmmoClipSize
===============
*/
int BG_GetAmmoClipSize( int iClipIndex )
{
	return bg_weapClips[iClipIndex]->clipSize;
}

/*
===============
BG_GetSharedAmmoCapSize
===============
*/
int BG_GetSharedAmmoCapSize( int iAmmoIndex )
{
	return bg_sharedAmmoCaps[iAmmoIndex]->sharedAmmoCap;
}

/*
===============
BG_GetSharedAmmoCapName
===============
*/
const char *BG_GetSharedAmmoCapName( int iAmmoIndex )
{
	return bg_sharedAmmoCaps[iAmmoIndex]->sharedAmmoCapName;
}

/*
===============
BG_GetAmmoTypeName
===============
*/
const char *BG_GetAmmoTypeName( int iAmmoIndex )
{
	return bg_weapAmmoTypes[iAmmoIndex]->ammoName;
}

/*
===============
BG_GetAmmoClipName
===============
*/
const char *BG_GetAmmoClipName( int iClipIndex )
{
	return bg_weapClips[iClipIndex]->clipName;
}

/*
===============
BG_GetAmmoTypeForName
===============
*/
int BG_GetAmmoTypeForName( const char *name )
{
	int curAmmoIndex;

	for ( curAmmoIndex = 0; curAmmoIndex < bg_iNumAmmoTypes; curAmmoIndex++ )
	{
		if ( !I_stricmp(bg_weapAmmoTypes[curAmmoIndex]->ammoName, name) )
		{
			return curAmmoIndex;
		}
	}

	Com_DPrintf("Couldn't find ammo type \"%s\"\n", name);
	return 0;
}

/*
===============
BG_GetAmmoClipForName
===============
*/
int BG_GetAmmoClipForName( const char *name )
{
	int curClipIndex;

	for ( curClipIndex = 0; curClipIndex < bg_iNumWeapClips; curClipIndex++ )
	{
		if ( !I_stricmp(bg_weapClips[curClipIndex]->clipName, name) )
		{
			return curClipIndex;
		}
	}

	Com_DPrintf("Couldn't find ammo clip \"%s\"\n", name);
	return 0;
}

/*
===============
BG_FindWeaponIndexForName
===============
*/
int BG_FindWeaponIndexForName( const char *name )
{
	int curWeaponIndex;

	for ( curWeaponIndex = 1; curWeaponIndex <= bg_iNumWeapons; curWeaponIndex++ )
	{
		if ( !I_stricmp(name, bg_weaponDefs[curWeaponIndex]->szInternalName) )
		{
			return curWeaponIndex;
		}
	}

	return WP_NONE;
}

/*
===============
BG_GetWeaponIndexForName
===============
*/
int BG_GetWeaponIndexForName( const char *name, int (*regWeap)(int) )
{
	int weapIndex;
	WeaponDef *weapDef;

	if ( !name[0] || !strcasecmp(name, "none") )
	{
		return WP_NONE;
	}

	weapIndex = BG_FindWeaponIndexForName(name);

	if ( weapIndex )
	{
		return weapIndex;
	}

	weapDef = BG_LoadWeaponDef("mp", name);

	if ( !weapDef )
	{
		Com_DPrintf("Couldn't find weapon \"%s\"\n", name);
		return WP_NONE;
	}

	return BG_SetupWeaponDef(weapDef, regWeap);
}

/*
===============
CG_GetWeaponIndexForName
===============
*/
int CG_GetWeaponIndexForName( const char *name )
{
	return BG_GetWeaponIndexForName(name, WP_NONE);
}

/*
===============
BG_GetBinocularWeaponIndex
===============
*/
int BG_GetBinocularWeaponIndex()
{
	int curWeaponIndex;

	for ( curWeaponIndex = 1; curWeaponIndex <= bg_iNumWeapons; curWeaponIndex++ )
	{
		if ( bg_weaponDefs[curWeaponIndex]->weaponType == WEAPTYPE_BINOCULARS )
		{
			return (unsigned char)curWeaponIndex;
		}
	}

	return WP_NONE;
}

/*
===============
BG_GetViewmodelWeaponIndex
===============
*/
int BG_GetViewmodelWeaponIndex( const playerState_t *ps )
{
	assert(ps);

	if ( ps->pm_flags & PMF_BINOCULARS )
	{
		return BG_GetBinocularWeaponIndex();
	}

	return ( ps->pm_flags & PMF_FRAG ) ? ps->offHandIndex : ps->weapon;
}

int BG_GetFirstAvailableOffhand( const playerState_t *ps, int offhandSlot )
{
	WeaponDef *weapDef;
	int weapIndex;
	int numWeps;

	assert(ps);

	numWeps = BG_GetNumWeapons();

	for ( weapIndex = 1; weapIndex <= numWeps; weapIndex++ )
	{
		weapDef = BG_GetWeaponDef(weapIndex);

		if ( weapDef->offhandClass != offhandSlot )
		{
			continue;
		}

		if ( !Com_BitCheck(ps->weapons, weapIndex) )
		{
			continue;
		}

		if ( !BG_WeaponAmmo(ps, weapIndex) )
		{
			continue;
		}

		return weapIndex;
	}

	return WP_NONE;
}

int BG_GetFirstEquippedOffhand( const playerState_t *ps, int offhandSlot )
{
	WeaponDef *weapDef;
	int weapIndex;
	int numWeps;

	numWeps = BG_GetNumWeapons();

	for ( weapIndex = 1; weapIndex <= numWeps; weapIndex++ )
	{
		weapDef = BG_GetWeaponDef(weapIndex);

		if ( weapDef->offhandClass != offhandSlot )
			continue;

		if ( !Com_BitCheck(ps->weapons, weapIndex) )
			continue;

		return weapIndex;
	}

	return WP_NONE;
}

/*
===============
BG_IsAimDownSightWeapon
===============
*/
qboolean BG_IsAimDownSightWeapon( int iWeapon )
{
	return BG_GetWeaponDef(iWeapon)->aimDownSight;
}

/*
===============
BG_ValidateWeaponNumber
===============
*/
bool BG_ValidateWeaponNumber( int weaponIndex )
{
	return weaponIndex >= WP_NONE && weaponIndex <= BG_GetNumWeapons();
}

/*
===============
BG_DoesWeaponRequireSlot
===============
*/
bool BG_DoesWeaponRequireSlot( int weaponIndex )
{
	const int *weaponSlot;

	weaponSlot = &BG_GetWeaponDef(weaponIndex)->weaponSlot;
	return *weaponSlot == SLOT_PRIMARY || *weaponSlot == SLOT_PRIMARYB;
}

/*
===============
BG_IsAnyEmptyPrimaryWeaponSlot
===============
*/
bool BG_IsAnyEmptyPrimaryWeaponSlot( const playerState_t *ps )
{
	assert(ps);
	return !ps->weaponslots[SLOT_PRIMARY] || !ps->weaponslots[SLOT_PRIMARYB];
}

/*
===============
BG_IsWeaponValid
===============
*/
bool BG_IsWeaponValid( const playerState_t *ps, int weaponIndex )
{
	bool valid;
	int altWeaponIndex;
	WeaponDef *weapDef;

	assert(ps);

	valid = true;

	if ( !BG_ValidateWeaponNumber(weaponIndex) )
	{
		valid = false;
	}

	if ( !Com_BitCheck(ps->weapons, weaponIndex) )
	{
		valid = false;
	}

	weapDef = BG_GetWeaponDef(weaponIndex);
	assert(weapDef);

	if ( !weapDef->offhandClass )
	{
		altWeaponIndex = weapDef->altWeaponIndex;

		if ( ps->weaponslots[SLOT_PRIMARY] != weaponIndex
		  && ps->weaponslots[SLOT_PRIMARYB] != weaponIndex
		  && altWeaponIndex != weaponIndex )
		{
			valid = false;
		}
	}

	return valid;
}

/*
===============
BG_DoesWeaponNeedSlot
===============
*/
bool BG_DoesWeaponNeedSlot( int weaponIndex )
{
	WeaponDef *weapDef;

	weapDef = BG_GetWeaponDef(weaponIndex);
	if ( weapDef->weaponClass == WEAPCLASS_ITEM )
	{
		return false;
	}

	if ( weapDef->offhandClass )
	{
		return false;
	}

	return true;
}

/*
===============
BG_TakePlayerWeapon
===============
*/
int BG_TakePlayerWeapon( playerState_t *pPS, int iWeaponIndex )
{
	int curWeaponIndex;
	int slot;
	WeaponDef *weapDef;
	WeaponDef *weapCheck;

	assert(pPS);

	if ( !Com_BitCheck(pPS->weapons, iWeaponIndex) )
	{
		return qfalse;
	}

	weapDef = BG_GetWeaponDef(iWeaponIndex);
	slot = BG_IsPlayerWeaponInSlot(pPS, iWeaponIndex, qtrue);

	if ( slot )
	{
		if ( weapDef->slotStackable )
		{
			for ( curWeaponIndex = 1; curWeaponIndex <= bg_iNumWeapons; curWeaponIndex++ )
			{
				weapCheck = BG_GetWeaponDef(iWeaponIndex);

				if ( weapCheck->slotStackable
				        && weapCheck->weaponSlot == weapDef->weaponSlot
				        && Com_BitCheck(pPS->weapons, curWeaponIndex)
				        && !BG_IsPlayerWeaponInSlot(pPS, curWeaponIndex, qtrue) )
				{
					pPS->weaponslots[slot] = curWeaponIndex;
					break;
				}
			}

			if ( curWeaponIndex > bg_iNumWeapons )
			{
				pPS->weaponslots[slot] = WP_NONE;
			}
		}
		else
		{
			pPS->weaponslots[slot] = WP_NONE;
		}
	}

	Com_BitClear(pPS->weapons, iWeaponIndex);

	for ( curWeaponIndex = weapDef->altWeaponIndex; curWeaponIndex && Com_BitCheck(pPS->weapons, curWeaponIndex); curWeaponIndex = BG_GetWeaponDef(curWeaponIndex)->altWeaponIndex )
	{
		Com_BitClear(pPS->weapons, curWeaponIndex);
	}

	if ( iWeaponIndex == pPS->weapon )
	{
		pPS->weapon = WP_NONE;
	}

	return qtrue;
}

/*
===============
BG_GetAmmoClipSize
===============
*/
qboolean BG_SetPlayerWeaponForSlot( playerState_t *ps, int slot, int weaponIndex )
{
	int weaponSlot;

	if ( !Com_BitCheck(ps->weapons, weaponIndex) )
	{
		return qfalse;
	}

	weaponSlot = BG_GetWeaponDef(weaponIndex)->weaponSlot;

	if ( weaponSlot <= 0 || weaponSlot > 2 )
	{
		return qfalse;
	}

	if ( slot != SLOT_PRIMARY && slot != SLOT_PRIMARYB )
	{
		return qfalse;
	}

	if ( weaponIndex != ps->weaponslots[slot] && weaponIndex != ps->weapon )
	{
		ps->weapon = 0;
	}

	ps->weaponslots[slot] = weaponIndex;

	return qtrue;
}

/*
===============
BG_IsPlayerWeaponInSlot
===============
*/
int BG_IsPlayerWeaponInSlot( playerState_t *pPS, int iWeaponIndex, qboolean bAnyMode )
{
	int index;
	WeaponDef *weapDef;

	assert(pPS);

	if ( !Com_BitCheck(pPS->weapons, iWeaponIndex) )
	{
		return SLOT_NONE;
	}

	index = iWeaponIndex;

	do
	{
		weapDef = BG_GetWeaponDef(index);

		if ( weapDef->weaponSlot <= 0 || weapDef->weaponSlot > 2 )
		{
			return SLOT_NONE;
		}

		if ( pPS->weaponslots[SLOT_PRIMARY] == index )
		{
			return SLOT_PRIMARY;
		}

		if ( pPS->weaponslots[SLOT_PRIMARYB] == index )
		{
			return SLOT_PRIMARYB;
		}

		if ( bAnyMode && weapDef->altWeaponIndex )
		{
			index = weapDef->altWeaponIndex;
		}
	}
	while ( index != iWeaponIndex );

	return SLOT_NONE;
}

/*
===============
BG_GetEmptySlotForWeapon
===============
*/
int BG_GetEmptySlotForWeapon( playerState_t *pPS, int iWeaponIndex )
{
	int weaponSlot;

	weaponSlot = BG_GetWeaponDef(iWeaponIndex)->weaponSlot;

	if ( weaponSlot > SLOT_NONE && weaponSlot <= SLOT_PRIMARYB )
	{
		if ( !pPS->weaponslots[SLOT_PRIMARY] )
		{
			return SLOT_PRIMARY;
		}

		if ( !pPS->weaponslots[SLOT_PRIMARYB] )
		{
			return SLOT_PRIMARYB;
		}
	}

	return SLOT_NONE;
}

/*
===============
BG_GetStackSlotForWeapon
===============
*/
int BG_GetStackSlotForWeapon( const playerState_t *pPS, int iWeaponIndex, unsigned int preferedSlot )
{
	WeaponDef *weapDef;

	weapDef = BG_GetWeaponDef(iWeaponIndex);

	if ( !weapDef->slotStackable )
	{
		return SLOT_NONE;
	}

	if ( weapDef->weaponSlot <= 0 || weapDef->weaponSlot > 2 )
	{
		return SLOT_NONE;
	}

	if ( preferedSlot == SLOT_PRIMARY || preferedSlot == SLOT_PRIMARYB )
	{
		if ( !pPS->weaponslots[preferedSlot] )
		{
			return preferedSlot;
		}

		if ( BG_GetWeaponDef(pPS->weaponslots[preferedSlot])->slotStackable )
		{
			return preferedSlot;
		}
	}

	if ( !pPS->weaponslots[SLOT_PRIMARY] )
	{
		return SLOT_PRIMARY;
	}

	if ( BG_GetWeaponDef(pPS->weaponslots[SLOT_PRIMARY])->slotStackable )
	{
		return SLOT_PRIMARY;
	}

	if ( pPS->weaponslots[SLOT_PRIMARYB] && !BG_GetWeaponDef(pPS->weaponslots[SLOT_PRIMARYB])->slotStackable )
	{
		return SLOT_NONE;
	}

	return SLOT_PRIMARYB;
}

// Name from the Mac 1.3 client (bg_weapons).
// True if altWeaponIndex is reachable from weaponIndex's alt-weapon chain.
qboolean BG_IsPlayerWeaponAnAlt( int weaponIndex, int altWeaponIndex )
{
	int index;

	for ( index = BG_GetWeaponDef(weaponIndex)->altWeaponIndex; index; index = BG_GetWeaponDef(index)->altWeaponIndex )
	{
		if ( index == altWeaponIndex )
			return qtrue;

		if ( index == weaponIndex )
			return qfalse;
	}

	return qfalse;
}

/*
===============
BG_GetMaxPickupableAmmo
===============
*/
int BG_GetMaxPickupableAmmo( const playerState_t *ps, int weaponIndex )
{
	int curWeaponIndex;
	WeaponDef *weapDef;
	WeaponDef *curWeapDef;
	int ammo;
	int iAmmoIndex;
	int iClipIndex;
	qboolean ammoCounted[MAX_WEAPONS];
	qboolean clipCounted[MAX_WEAPONS];


	memset(ammoCounted, 0, sizeof(ammoCounted));
	memset(clipCounted, 0, sizeof(clipCounted));

	weapDef = BG_GetWeaponDef(weaponIndex);

	iAmmoIndex = BG_AmmoForWeapon(weaponIndex);
	iClipIndex = BG_ClipForWeapon(weaponIndex);

	if ( weapDef->sharedAmmoCapIndex >= 0 )
	{
		ammo = BG_GetSharedAmmoCapSize(weapDef->sharedAmmoCapIndex);

		for ( curWeaponIndex = 1; curWeaponIndex <= bg_iNumWeapons; curWeaponIndex++ )
		{
			if ( !Com_BitCheck(ps->weapons, curWeaponIndex) )
			{
				continue;
			}

			curWeapDef = BG_GetWeaponDef(curWeaponIndex);

			if ( curWeapDef->sharedAmmoCapIndex == weapDef->sharedAmmoCapIndex )
			{
				if ( BG_WeaponIsClipOnly(curWeaponIndex) )
				{
					if ( !clipCounted[BG_ClipForWeapon(curWeaponIndex)] )
					{
						clipCounted[BG_ClipForWeapon(curWeaponIndex)] = qtrue;
						ammo -= ps->ammoclip[BG_ClipForWeapon(curWeaponIndex)];
					}
				}
				else
				{
					if ( !ammoCounted[BG_AmmoForWeapon(curWeaponIndex)] )
					{
						ammoCounted[BG_AmmoForWeapon(curWeaponIndex)] = qtrue;
						ammo -= ps->ammo[BG_AmmoForWeapon(curWeaponIndex)];
					}
				}
			}
		}
	}
	else
	{
		if ( BG_WeaponIsClipOnly(weaponIndex) )
		{
			ammo = BG_GetAmmoClipSize(iClipIndex) - ps->ammoclip[iClipIndex];
		}
		else
		{
			ammo = BG_GetAmmoTypeMax(iAmmoIndex) - ps->ammo[iAmmoIndex];
		}
	}

	return ammo;
}

/*
===============
BG_GetTotalAmmoReserve
===============
*/
int BG_GetTotalAmmoReserve( const playerState_t *ps, int weaponIndex )
{
	int curWeaponIndex;
	WeaponDef *weapDef;
	WeaponDef *curWeapDef;
	int ammo;
	int iAmmoIndex;
	int iClipIndex;
	qboolean ammoCounted[MAX_WEAPONS];
	qboolean clipCounted[MAX_WEAPONS];

	ammo = 0;

	iAmmoIndex = BG_AmmoForWeapon(weaponIndex);
	iClipIndex = BG_ClipForWeapon(weaponIndex);

	memset(ammoCounted, 0, sizeof(ammoCounted));
	memset(clipCounted, 0, sizeof(clipCounted));

	weapDef = BG_GetWeaponDef(weaponIndex);

	if ( weapDef->sharedAmmoCapIndex >= 0 )
	{
		for ( curWeaponIndex = 1; curWeaponIndex <= bg_iNumWeapons; curWeaponIndex++ )
		{
			if ( !Com_BitCheck(ps->weapons, curWeaponIndex) )
			{
				continue;
			}

			curWeapDef = BG_GetWeaponDef(curWeaponIndex);

			if ( curWeapDef->sharedAmmoCapIndex == weapDef->sharedAmmoCapIndex )
			{
				if ( BG_WeaponIsClipOnly(curWeaponIndex) )
				{
					if ( !clipCounted[BG_ClipForWeapon(curWeaponIndex)] )
					{
						clipCounted[BG_ClipForWeapon(curWeaponIndex)] = qtrue;
						ammo += ps->ammoclip[BG_ClipForWeapon(curWeaponIndex)];
					}
				}
				else
				{
					if ( !ammoCounted[BG_AmmoForWeapon(curWeaponIndex)] )
					{
						ammoCounted[BG_AmmoForWeapon(curWeaponIndex)] = qtrue;
						ammo += ps->ammo[BG_AmmoForWeapon(curWeaponIndex)];
					}
				}
			}
		}
	}
	else
	{
		if ( BG_WeaponIsClipOnly(weaponIndex) )
		{
			ammo = ps->ammoclip[iClipIndex];
		}
		else
		{
			ammo = ps->ammo[iAmmoIndex];
		}
	}

	return ammo;
}

/*
===============
BG_GetSpreadForWeapon
===============
*/
void BG_GetSpreadForWeapon( const playerState_t *ps, int weaponIndex, float *minSpread, float *maxSpread )
{
	WeaponDef *weapDef;
	float frac;

	assert(ps);
	weapDef = BG_GetWeaponDef(weaponIndex);

	if ( ps->viewHeightCurrent > CROUCH_VIEWHEIGHT )
	{
		frac = (ps->viewHeightCurrent - CROUCH_VIEWHEIGHT) / 20.0f;

		*minSpread = (weapDef->hipSpreadStandMin - weapDef->hipSpreadDuckedMin) * frac + weapDef->hipSpreadDuckedMin;
		*maxSpread = (weapDef->hipSpreadMax - weapDef->hipSpreadDuckedMax) * frac + weapDef->hipSpreadDuckedMax;
	}
	else
	{
		frac = (ps->viewHeightCurrent - PRONE_VIEWHEIGHT) / 29.0f;

		*minSpread = (weapDef->hipSpreadDuckedMin - weapDef->hipSpreadProneMin) * frac + weapDef->hipSpreadProneMin;
		*maxSpread = (weapDef->hipSpreadDuckedMax - weapDef->hipSpreadProneMax) * frac + weapDef->hipSpreadProneMax;
	}
}

/*
===============
PM_IsBinocularADSAllowed
===============
*/
bool PM_IsBinocularADSAllowed( int iWeaponState )
{
	return iWeaponState == WEAPON_BINOCULARS_HOLD || iWeaponState == WEAPON_BINOCULARS_EXIT_ADS;
}

/*
===============
PM_IsAdsAllowed
===============
*/
bool PM_IsAdsAllowed( playerState_t *ps, pml_t *pml )
{
	WeaponDef *weapDef;
	int weapIndex;

	assert(ps);
	assert(pml);

	switch ( ps->pm_type )
	{
	case PM_NORMAL_LINKED:
		if ( pml->almostGroundPlane )
		{
			return false;
		}
		break;

	case PM_NOCLIP:
	case PM_UFO:
	case PM_SPECTATOR:
	case PM_INTERMISSION:
	case PM_DEAD:
	case PM_DEAD_LINKED:
		return false;

	default:
		break;
	}

	weapIndex = BG_GetViewmodelWeaponIndex(ps);
	weapDef = BG_GetWeaponDef(weapIndex);

	if ( !weapDef->aimDownSight )
	{
		return false;
	}

	if ( ps->weaponstate >= WEAPON_OFFHAND_INIT && ps->weaponstate <= WEAPON_OFFHAND_END )
	{
		return false;
	}

	if ( ps->weaponstate == WEAPON_MELEE_INIT || ps->weaponstate == WEAPON_MELEE_FIRE )
	{
		return false;
	}

	if ( ps->weaponstate == WEAPON_RAISING || ps->weaponstate == WEAPON_DROPPING )
	{
		return false;
	}

	if ( ps->weaponstate >= WEAPON_BINOCULARS_INIT && ps->weaponstate <= WEAPON_BINOCULARS_END )
	{
		if ( !PM_IsBinocularADSAllowed(ps->weaponstate) )
		{
			return false;
		}
	}

	return true;
}

/*
===============
PM_IsBinocularsADS
===============
*/
int PM_IsBinocularsADS( const playerState_t *ps )
{
	assert(ps);

	if ( ps->weaponstate == WEAPON_BINOCULARS_HOLD || ps->weaponstate == WEAPON_BINOCULARS_EXIT_ADS )
	{
		return 1;
	}

	return 0;
}

/*
===============
PM_UpdateAimDownSightFlag
===============
*/
void PM_UpdateAimDownSightFlag( pmove_t *pm, pml_t *pml )
{
	bool adsAllowed;
	bool adsRequested;
	playerState_t *ps;

	assert(pm);
	ps = pm->ps;
	assert(ps);

	ps->pm_flags &= ~PMF_ADS;

	adsAllowed = PM_IsAdsAllowed(ps, pml);
	adsRequested = ( pm->cmd.buttons & BUTTON_ADS ) != 0 || (unsigned char)PM_IsBinocularsADS(ps);

	if ( adsRequested && adsAllowed )
	{
		if ( ps->pm_flags & PMF_PRONE )
		{
			if ( !(pm->oldcmd.buttons & BUTTON_ADS) || !pm->cmd.forwardmove && !pm->cmd.rightmove )
			{
				ps->pm_flags |= PMF_ADS;
				ps->pm_flags |= PMF_ADS_OVERRIDE;
			}
		}
		else
		{
			ps->pm_flags |= PMF_ADS;
		}
	}

	if ( ps->pm_flags & PMF_ADS )
		BG_UpdateConditionValue(ps->clientNum, ANIM_COND_WEAPON_POSITION, qtrue, qtrue);
	else
		BG_UpdateConditionValue(ps->clientNum, ANIM_COND_WEAPON_POSITION, qfalse, qtrue);
}

/*
===============
PM_ExitAimDownSight
===============
*/
void PM_ExitAimDownSight( playerState_t *ps )
{
	assert(ps);
	PM_AddEvent(ps, EV_RESET_ADS);
	ps->pm_flags &= ~PMF_ADS;
}

/*
===============
PM_UpdateAimDownSightLerp
===============
*/
void PM_UpdateAimDownSightLerp( pmove_t *pm, pml_t *pml )
{
	WeaponDef *weapDef;
	int weapIndex;
	bool adsRequested;
	bool bFullyAds;
	playerState_t *ps;

	assert(pm);
	ps = pm->ps;
	assert(ps);

	weapIndex = BG_GetViewmodelWeaponIndex(ps);
	weapDef = BG_GetWeaponDef(weapIndex);

	if ( player_scopeExitOnDamage->current.boolean && ps->damageCount && weapDef->adsOverlayReticle )
	{
		PM_ExitAimDownSight(ps);
		ps->fWeaponPosFrac = 0;
		ps->adsDelayTime = 0;
		return;
	}

	if ( !weapDef->aimDownSight )
	{
		ps->fWeaponPosFrac = 0;
		ps->adsDelayTime = 0;
		return;
	}

	adsRequested = false;

	if ( !weapDef->segmentedReload
	        && ps->weaponstate == WEAPON_RELOADING
	        && ps->weaponTime - weapDef->adsReloadTransTime > 0
	        || weapDef->segmentedReload
	        && (ps->weaponstate == WEAPON_RELOADING
	            || ps->weaponstate == WEAPON_RELOADING_INTERUPT
	            || ps->weaponstate == WEAPON_RELOAD_START
	            || ps->weaponstate == WEAPON_RELOAD_START_INTERUPT
	            || ps->weaponstate == WEAPON_RELOAD_END && ps->weaponTime - weapDef->adsReloadTransTime > 0)
	        || !weapDef->rechamberWhileAds && ps->weaponstate == WEAPON_RECHAMBERING )
	{
		adsRequested = false;
	}
	else if ( ps->pm_flags & PMF_ADS )
	{
		adsRequested = true;
	}

	if ( weapDef->adsFire && ps->weaponDelay && ps->weaponstate == WEAPON_FIRING )
	{
		adsRequested = true;
	}

	bFullyAds = ps->fWeaponPosFrac == 1;

	if ( bFullyAds && !adsRequested && player_adsExitDelay->current.integer > 0 )
	{
		if ( !ps->adsDelayTime )
		{
			ps->adsDelayTime = pm->cmd.serverTime + player_adsExitDelay->current.integer;
		}

		if ( ps->adsDelayTime > pm->cmd.serverTime )
			adsRequested = true;
		else
			ps->adsDelayTime = 0;
	}
	else
	{
		ps->adsDelayTime = 0;
	}

	if ( adsRequested && ps->fWeaponPosFrac != 1 || !adsRequested && ps->fWeaponPosFrac != 0 )
	{
		if ( adsRequested )
			ps->fWeaponPosFrac += pml->msec * weapDef->OOPosAnimLength[0];
		else
			ps->fWeaponPosFrac -= pml->msec * weapDef->OOPosAnimLength[1];

		ps->fWeaponPosFrac = I_fclamp(ps->fWeaponPosFrac, 0, 1);
	}
}

/*
===============
PM_StartHoldBreath
===============
*/
void PM_StartHoldBreath( playerState_t *ps )
{
	assert(ps);
	ps->pm_flags |= PMF_BREATH;
}

/*
===============
PM_EndHoldBreath
===============
*/
void PM_EndHoldBreath( playerState_t *ps )
{
	assert(ps);
	ps->pm_flags &= ~PMF_BREATH;
}

/*
===============
PM_HoldBreathFire
===============
*/
void PM_HoldBreathFire( playerState_t *ps )
{
	int breathHoldTime;
	int breathFireDelay;
	WeaponDef *weapDef;
	int weapIndex;

	assert(ps);

	weapIndex = BG_GetViewmodelWeaponIndex(ps);
	weapDef = BG_GetWeaponDef(weapIndex);

	if ( ps->fWeaponPosFrac != 1.0f )
	{
		return;
	}

	if ( weapDef->adsOverlayReticle && weapDef->weaponClass != WEAPCLASS_ITEM )
	{
		breathHoldTime = player_breath_hold_time->current.decimal * 1000.0f;
		breathFireDelay = player_breath_fire_delay->current.decimal * 1000.0f;

		if ( ps->holdBreathTimer < breathHoldTime )
		{
			ps->holdBreathTimer += breathFireDelay;

			if ( ps->holdBreathTimer > breathHoldTime )
			{
				ps->holdBreathTimer = breathHoldTime;
			}
		}

		PM_EndHoldBreath(ps);
	}
}

/*
===============
PM_UpdateHoldBreath
===============
*/
void PM_UpdateHoldBreath( pmove_t *pm, pml_t *pml )
{
	WeaponDef *weapDef;
	int weapIndex;
	float targetScale;
	float lerp;
	int breathHoldTime;
	int breathGaspTime;
	playerState_t *ps;

	assert(pm);
	ps = pm->ps;
	assert(ps);

	weapIndex = BG_GetViewmodelWeaponIndex(ps);
	weapDef = BG_GetWeaponDef(weapIndex);

	breathHoldTime = player_breath_hold_time->current.decimal * 1000;
	breathGaspTime = player_breath_gasp_time->current.decimal * 1000;

	if ( breathHoldTime <= 0 )
	{
		PM_EndHoldBreath(ps);
		ps->holdBreathScale = 1.0f;
		ps->holdBreathTimer = 0;
		return;
	}

	if ( !(ps->fWeaponPosFrac == 1.0f && weapDef->adsOverlayReticle && weapDef->weaponClass != WEAPCLASS_ITEM) )
	{
		PM_EndHoldBreath(ps);
	}
	else if ( !(pm->cmd.buttons & BUTTON_HOLDBREATH) )
	{
		PM_EndHoldBreath(ps);
	}
	else if ( !ps->holdBreathTimer )
	{
		PM_StartHoldBreath(ps);
	}

	if ( ps->pm_flags & PMF_BREATH )
		ps->holdBreathTimer += pml->msec;
	else
		ps->holdBreathTimer -= pml->msec;

	if ( ps->holdBreathTimer < 0 )
	{
		ps->holdBreathTimer = 0;
	}

	if ( ps->pm_flags & PMF_BREATH )
	{
		if ( ps->holdBreathTimer > breathHoldTime )
		{
			ps->holdBreathTimer = breathHoldTime + breathGaspTime;
			PM_EndHoldBreath(ps);
		}
	}

	targetScale = 1.0f;

	if ( ps->pm_flags & PMF_BREATH )
	{
		targetScale = 0;
		lerp = player_breath_hold_lerp->current.decimal;
	}
	else
	{
		lerp = ps->holdBreathTimer / (float)(breathHoldTime + breathGaspTime);
		targetScale = (player_breath_gasp_scale->current.decimal - 1.0f) * lerp + 1.0f;
		lerp = player_breath_gasp_lerp->current.decimal;
	}

	targetScale = (targetScale - 1.0f) * ps->fWeaponPosFrac + 1.0f;

	ps->holdBreathScale = DiffTrack(targetScale, ps->holdBreathScale, lerp, pml->frametime);
}

/*
===============
PM_InteruptWeaponWithProneMove
===============
*/
qboolean PM_InteruptWeaponWithProneMove( playerState_t *ps )
{
	assert(ps);

	if ( ps->weaponstate == WEAPON_READY
	        || ps->weaponstate == WEAPON_RAISING
	        || ps->weaponstate == WEAPON_DROPPING
	        || ps->weaponstate == WEAPON_RELOADING
	        || ps->weaponstate == WEAPON_RELOAD_START
	        || ps->weaponstate == WEAPON_RELOAD_END
	        || ps->weaponstate == WEAPON_RELOAD_START_INTERUPT
	        || ps->weaponstate == WEAPON_RELOADING_INTERUPT
	        || ps->weaponstate == WEAPON_RECHAMBERING )
	{
		return qtrue;
	}

	if ( ps->weaponstate == WEAPON_FIRING
	        || ps->weaponstate == WEAPON_MELEE_FIRE
	        || ( ps->weaponstate >= WEAPON_OFFHAND_INIT && ps->weaponstate <= WEAPON_OFFHAND_END ) )
	{
		return qfalse;
	}

	PM_Weapon_Idle(ps);
	return qtrue;
}

/*
===============
BG_ClipForWeapon
===============
*/
int BG_ClipForWeapon( int weapon )
{
	return BG_GetWeaponDef(weapon)->clipIndex;
}

/*
===============
BG_AmmoForWeapon
===============
*/
int BG_AmmoForWeapon( int weapon )
{
	return BG_GetWeaponDef(weapon)->ammoIndex;
}

/*
===============
BG_WeaponIsClipOnly
===============
*/
int BG_WeaponIsClipOnly( int weapon )
{
	return BG_GetWeaponDef(weapon)->clipOnly;
}

/*
===============
BG_WeaponAmmo
===============
*/
int BG_WeaponAmmo( const playerState_t *ps, int weapon )
{
	int clipIndex;
	int ammoIndex;

	assert(ps);

	ammoIndex = BG_AmmoForWeapon(weapon);
	clipIndex = BG_ClipForWeapon(weapon);

	return ps->ammo[ammoIndex] + ps->ammoclip[clipIndex];
}

/*
===============
PM_ReloadClip
===============
*/
void PM_ReloadClip( playerState_t *ps )
{
	WeaponDef *weapDef;
	int ammoIndex;
	int clipIndex;
	int ammoReserve;
	int ammoInClip;
	int ammoAdd;

	assert(ps);
	weapDef = BG_GetWeaponDef(ps->weapon);

	if ( ps->weaponstate == WEAPON_RELOAD_START || ps->weaponstate == WEAPON_RELOAD_START_INTERUPT )
	{
		if ( !weapDef->reloadStartAdd )
		{
			return;
		}
	}

	ammoIndex = BG_AmmoForWeapon(ps->weapon);
	clipIndex = BG_ClipForWeapon(ps->weapon);

	ammoReserve = ps->ammo[ammoIndex];
	ammoInClip = ps->ammoclip[clipIndex];

	ammoAdd = BG_GetAmmoClipSize(clipIndex) - ammoInClip;

	if ( ammoAdd > ammoReserve )
	{
		ammoAdd = ammoReserve;
	}

	if ( ps->weaponstate == WEAPON_RELOAD_START || ps->weaponstate == WEAPON_RELOAD_START_INTERUPT )
	{
		if ( weapDef->reloadStartAdd < BG_GetAmmoClipSize(clipIndex) && ammoAdd > weapDef->reloadStartAdd )
		{
			ammoAdd = weapDef->reloadStartAdd;
		}
	}
	else
	{
		if ( weapDef->reloadAmmoAdd && weapDef->reloadAmmoAdd < BG_GetAmmoClipSize(clipIndex) && ammoAdd > weapDef->reloadAmmoAdd )
		{
			ammoAdd = weapDef->reloadAmmoAdd;
		}
	}

	if ( ammoAdd )
	{
		ps->ammo[ammoIndex] -= ammoAdd;
		ps->ammoclip[clipIndex] += ammoAdd;
	}
}

/*
===============
PM_WeaponUseAmmo
===============
*/
void PM_WeaponUseAmmo( playerState_t *ps, int weaponIndex, int amount )
{
	int clipIndex;

	assert(ps);

	clipIndex = BG_ClipForWeapon(weaponIndex);
	ps->ammoclip[clipIndex] -= amount;
}

/*
===============
PM_WeaponAmmoAvailable
===============
*/
int PM_WeaponAmmoAvailable( playerState_t *ps )
{
	int clipIndex;

	assert(ps);

	clipIndex = BG_ClipForWeapon(ps->weapon);
	return ps->ammoclip[clipIndex];
}

/*
===============
PM_WeaponClipEmpty
===============
*/
qboolean PM_WeaponClipEmpty( playerState_t *ps )
{
	int result;

	if ( ps->ammoclip[BG_ClipForWeapon(ps->weapon)] == 0 )
		result = 1;
	else
		result = 0;

	return result;
}

/*
===============
PM_Weapon_FinishRechamber
===============
*/
void PM_Weapon_FinishRechamber( playerState_t *ps )
{
	assert(ps);
	PM_ContinueWeaponAnim(ps, WEAP_IDLE);
	ps->weaponstate = WEAPON_READY;
}

/*
===============
PM_Weapon_CheckForRechamber
===============
*/
qboolean PM_Weapon_CheckForRechamber( playerState_t *ps, qboolean delayedAction )
{
	WeaponDef *weapDef;

	assert(ps);
	weapDef = BG_GetWeaponDef(ps->weapon);

	if ( ps->weaponstate >= WEAPON_BINOCULARS_INIT && ps->weaponstate <= WEAPON_BINOCULARS_END )
	{
		return qfalse;
	}

	if ( ps->weaponstate >= WEAPON_OFFHAND_INIT && ps->weaponstate <= WEAPON_OFFHAND_END )
	{
		return qfalse;
	}

	if ( weapDef->boltAction && Com_BitCheck(ps->weaponrechamber, ps->weapon) )
	{
		if ( ps->weaponstate == WEAPON_RECHAMBERING )
		{
			if ( delayedAction )
			{
				Com_BitClear(ps->weaponrechamber, ps->weapon);
				PM_AddEvent(ps, EV_EJECT_BRASS);

				if ( ps->weaponTime )
				{
					return qtrue;
				}
			}
		}

		if ( !ps->weaponTime
		        || ( ps->weaponstate != WEAPON_FIRING && ps->weaponstate != WEAPON_RECHAMBERING
		             && ps->weaponstate != WEAPON_MELEE_INIT
		             && ps->weaponstate != WEAPON_MELEE_FIRE && !ps->weaponDelay ) )
		{
			if ( ps->weaponstate == WEAPON_RECHAMBERING )
			{
				PM_Weapon_FinishRechamber(ps);
			}
			else if ( ps->weaponstate == WEAPON_READY )
			{
				if ( ps->fWeaponPosFrac > 0.75f )
					PM_StartWeaponAnim(ps, WEAP_ADS_RECHAMBER);
				else
					PM_StartWeaponAnim(ps, WEAP_RECHAMBER);

				ps->weaponstate = WEAPON_RECHAMBERING;
				ps->weaponTime = weapDef->rechamberTime;

				if ( weapDef->rechamberBoltTime && weapDef->rechamberBoltTime < weapDef->rechamberTime )
					ps->weaponDelay = weapDef->rechamberBoltTime;
				else
					ps->weaponDelay = 1;

				PM_AddEvent(ps, EV_RECHAMBER_WEAPON);
			}
		}
	}

	return qfalse;
}

/*
===============
PM_SetWeaponReloadAddAmmoDelay
===============
*/
void PM_SetWeaponReloadAddAmmoDelay( playerState_t *ps )
{
	WeaponDef *weapDef;
	int iReloadAddTime;
	int clipIndex;

	assert(ps);

	weapDef = BG_GetWeaponDef(ps->weapon);

	if ( ps->weaponstate == WEAPON_RELOAD_START || ps->weaponstate == WEAPON_RELOAD_START_INTERUPT )
	{
		if ( weapDef->reloadStartAddTime )
		{
			if ( weapDef->reloadStartAddTime < weapDef->reloadStartTime )
				iReloadAddTime = weapDef->reloadStartAddTime;
			else
				iReloadAddTime = weapDef->reloadStartTime;
		}
		else
		{
			iReloadAddTime = 0;
		}
	}
	else
	{
		clipIndex = BG_ClipForWeapon(ps->weapon);

		if ( !ps->ammoclip[clipIndex] && !weapDef->weaponType )
			iReloadAddTime = weapDef->reloadEmptyTime;
		else
			iReloadAddTime = weapDef->reloadTime;

		if ( weapDef->reloadAddTime && weapDef->reloadAddTime < iReloadAddTime )
			iReloadAddTime = weapDef->reloadAddTime;
	}

	if ( weapDef->boltAction && Com_BitCheck(ps->weaponrechamber, ps->weapon) )
	{
		if ( !iReloadAddTime )
			iReloadAddTime = ps->weaponTime;

		if ( weapDef->rechamberBoltTime < iReloadAddTime )
			iReloadAddTime = weapDef->rechamberBoltTime;

		if ( !iReloadAddTime )
			iReloadAddTime = 1;

		ps->weaponDelay = iReloadAddTime;
	}
	else if ( iReloadAddTime )
	{
		ps->weaponDelay = iReloadAddTime;
	}
}

/*
===============
PM_SetReloadingState
===============
*/
void PM_SetReloadingState( playerState_t *ps )
{
	WeaponDef *weapDef;
	int clipIndex;

	assert(ps);
	weapDef = BG_GetWeaponDef(ps->weapon);
	clipIndex = BG_ClipForWeapon(ps->weapon);

	if ( !ps->ammoclip[clipIndex] && !weapDef->weaponType )
	{
		PM_StartWeaponAnim(ps, WEAP_RELOAD_EMPTY);
		ps->weaponTime = weapDef->reloadEmptyTime;
		PM_AddEvent(ps, EV_RELOAD_FROM_EMPTY);
	}
	else
	{
		PM_StartWeaponAnim(ps, WEAP_RELOAD);
		ps->weaponTime = weapDef->reloadTime;
		PM_AddEvent(ps, EV_RELOAD);
	}

	if ( ps->weaponstate == WEAPON_RELOAD_START_INTERUPT )
		ps->weaponstate = WEAPON_RELOADING_INTERUPT;
	else
		ps->weaponstate = WEAPON_RELOADING;

	PM_SetWeaponReloadAddAmmoDelay(ps);
}

/*
===============
PM_BeginWeaponReload
===============
*/
void PM_BeginWeaponReload( playerState_t *ps )
{
	WeaponDef *weapDef;

	assert(ps);
	weapDef = BG_GetWeaponDef(ps->weapon);

	if ( ps->weaponstate != WEAPON_READY && ps->weaponstate != WEAPON_FIRING && ps->weaponstate != WEAPON_RECHAMBERING )
	{
		return;
	}

	if ( !ps->weapon || (int)ps->weapon > BG_GetNumWeapons() )
	{
		return;
	}

	if ( !BG_WeaponIsClipOnly(ps->weapon) )
	{
		BG_AnimScriptEvent(ps, ANIM_ET_RELOAD, qfalse, qtrue);
	}

	PM_AddEvent(ps, EV_RESET_ADS);

	if ( weapDef->segmentedReload && weapDef->reloadStartTime )
	{
		PM_StartWeaponAnim(ps, WEAP_RELOAD_START);

		ps->weaponTime = weapDef->reloadStartTime;
		ps->weaponstate = WEAPON_RELOAD_START;

		PM_AddEvent(ps, EV_RELOAD_START);
		PM_SetWeaponReloadAddAmmoDelay(ps);
	}
	else
	{
		PM_SetReloadingState(ps);
	}
}

/*
===============
PM_BeginWeaponChange
===============
*/
void PM_BeginWeaponChange( playerState_t *ps, int newweapon )
{
	qboolean altswitch;
	qboolean oldWeaponNoAmmo;
	WeaponDef *weapDef;
	int oldweapon;
	int slot;

	assert(ps);

	if ( newweapon && !Com_BitCheck(ps->weapons, newweapon))
	{
		return;
	}

	if ( ps->weaponstate == WEAPON_DROPPING )
	{
		return;
	}

	ps->weaponDelay = 0;
	oldweapon = ps->weapon;

	if ( !oldweapon || !Com_BitCheck(ps->weapons, oldweapon) || ps->grenadeTimeLeft > 0 )
	{
		ps->weaponTime = 0;
		ps->weaponstate = WEAPON_DROPPING;
		ps->grenadeTimeLeft = 0;
		PM_SetProneMovementOverride(ps);
		return;
	}

	weapDef = BG_GetWeaponDef(oldweapon);
	altswitch = newweapon && newweapon == weapDef->altWeaponIndex;
	oldWeaponNoAmmo = qtrue;

	if ( BG_WeaponIsClipOnly(oldweapon) )
	{
		if ( !ps->ammoclip[BG_ClipForWeapon(oldweapon)] )
			oldWeaponNoAmmo = qfalse;
	}

	ps->grenadeTimeLeft = 0;

	if ( altswitch )
	{
		PM_AddEvent(ps, EV_WEAPON_ALT);
		PM_StartWeaponAnim(ps, WEAP_ALTSWITCHFROM);
	}
	else if ( oldWeaponNoAmmo )
	{
		PM_AddEvent(ps, EV_PUTAWAY_WEAPON);
		PM_StartWeaponAnim(ps, WEAP_DROP);
	}

	if ( !altswitch && !(ps->pm_flags & PMF_MANTLE) )
	{
		BG_AnimScriptEvent(ps, ANIM_ET_DROPWEAPON, qfalse, qfalse);
	}

	ps->weaponstate = WEAPON_DROPPING;
	PM_SetProneMovementOverride(ps);

	if ( altswitch )
		ps->weaponTime = weapDef->altDropTime;
	else
		ps->weaponTime = weapDef->dropTime;

	if ( altswitch )
	{
		slot = BG_IsPlayerWeaponInSlot(ps, oldweapon, qtrue);

		if ( slot )
		{
			BG_SetPlayerWeaponForSlot(ps, slot, newweapon);
		}
	}

}

/*
===============
PM_Weapon_FinishWeaponChange
===============
*/
void PM_Weapon_FinishWeaponChange( pmove_t *pm )
{
	WeaponDef *curWeapDef;
	qboolean bAltRaise;
	int oldweapon;
	int newweapon;
	playerState_t *ps;
	WeaponDef *weapDef;

	ps = pm->ps;
	curWeapDef = BG_GetWeaponDef(ps->weapon);

	if ( Mantle_IsWeaponInactive(ps) )
	{
		newweapon = WP_NONE;
	}
	else if ( ps->pm_flags & PMF_LADDER || !Com_BitCheck(ps->weapons, pm->cmd.weapon) )
	{
		newweapon = WP_NONE;
	}
	else if ( ps->pm_flags & PMF_DISABLEWEAPON || !Com_BitCheck(ps->weapons, pm->cmd.weapon) )
	{
		newweapon = WP_NONE;
	}
	else
	{
		newweapon = pm->cmd.weapon;

		if ( newweapon < 0 || newweapon > BG_GetNumWeapons() )
		{
			newweapon = WP_NONE;
		}
	}

	if ( !Com_BitCheck(ps->weapons, newweapon) )
	{
		newweapon = WP_NONE;
	}

	oldweapon = ps->weapon;
	ps->weapon = (unsigned char)newweapon;
	curWeapDef = BG_GetWeaponDef(ps->weapon);

	if ( oldweapon == newweapon )
	{
		ps->weaponstate = WEAPON_READY;
		PM_StartWeaponAnim(ps, WEAP_IDLE);
	}
	else if ( !oldweapon )
	{
		ps->weaponstate = WEAPON_RAISING;
		ps->weaponTime = BG_GetWeaponDef(newweapon)->raiseTime;
		ps->aimSpreadScale = 255;

		PM_StartWeaponAnim(ps, WEAP_RAISE);
		PM_SetProneMovementOverride(ps);
	}
	else
	{
		ps->weaponstate = WEAPON_RAISING;
		PM_SetProneMovementOverride(ps);

		bAltRaise = newweapon && newweapon == BG_GetWeaponDef(oldweapon)->altWeaponIndex;

		if ( bAltRaise )
		{
			ps->weaponTime = BG_GetWeaponDef(newweapon)->altRaiseTime;
		}
		else
		{
			PM_AddEvent(ps, EV_RAISE_WEAPON);
			ps->weaponTime = BG_GetWeaponDef(newweapon)->raiseTime;
		}

		weapDef = BG_GetWeaponDef(newweapon);
			BG_UpdateConditionValue(ps->clientNum, ANIM_COND_PLAYERANIMTYPE, weapDef->playerAnimType, qtrue);
		BG_UpdateConditionValue(ps->clientNum, ANIM_COND_WEAPONCLASS, weapDef->weaponClass, qtrue);

		if ( newweapon && newweapon == BG_GetWeaponDef(oldweapon)->altWeaponIndex )
		{
			if ( ps->aimSpreadScale < 128 )
			{
				ps->aimSpreadScale = 128;
			}

			PM_StartWeaponAnim(ps, WEAP_ALTSWITCHTO);
		}
		else
		{
			BG_AnimScriptEvent(ps, ANIM_ET_RAISEWEAPON, qfalse, qfalse);
			ps->aimSpreadScale = 255;

			PM_StartWeaponAnim(ps, WEAP_RAISE);
		}
	}
}

/*
===============
PM_Weapon_FinishWeaponRaise
===============
*/
void PM_Weapon_FinishWeaponRaise( playerState_t *ps )
{
	assert(ps);
	assert(ps->weaponstate == WEAPON_RAISING);

	ps->weaponstate = WEAPON_READY;
	PM_StartWeaponAnim(ps, WEAP_IDLE);
}

/*
===============
PM_Weapon_AllowReload
===============
*/
qboolean PM_Weapon_AllowReload( playerState_t *ps )
{
	WeaponDef *weapDef;
	int weapIndex;
	int ammoIndex;

	assert(ps);

	weapDef = BG_GetWeaponDef(ps->weapon);
	weapIndex = BG_ClipForWeapon(ps->weapon);
	ammoIndex = BG_AmmoForWeapon(ps->weapon);

	if ( ps->ammo[ammoIndex] && ps->ammoclip[weapIndex] < BG_GetAmmoClipSize(weapIndex) )
	{
		if ( weapDef->noPartialReload )
		{
			if ( !weapDef->reloadAmmoAdd || weapDef->reloadAmmoAdd >= BG_GetAmmoClipSize(weapIndex) )
			{
				if ( !ps->ammoclip[weapIndex] )
				{
					return qtrue;
				}
			}
			else if ( BG_GetAmmoClipSize(weapIndex) - ps->ammoclip[weapIndex] >= weapDef->reloadAmmoAdd )
			{
				return qtrue;
			}
		}
		else
		{
			return qtrue;
		}
	}

	return qfalse;
}

/*
===============
PM_Weapon_ReloadDelayedAction
===============
*/
void PM_Weapon_ReloadDelayedAction( playerState_t *ps )
{
	WeaponDef *weapDef;
	int reloadTime;
	int boltTime;
	int clipIndex;

	assert(ps);
	weapDef = BG_GetWeaponDef(ps->weapon);

	if ( weapDef->boltAction && Com_BitCheck(ps->weaponrechamber, ps->weapon) )
	{
		Com_BitClear(ps->weaponrechamber, ps->weapon);
		PM_AddEvent(ps, EV_EJECT_BRASS);

		if ( ps->weaponstate == WEAPON_RELOAD_START || ps->weaponstate == WEAPON_RELOAD_START_INTERUPT )
		{
			if ( !weapDef->reloadStartAddTime )
			{
				return;
			}
		}

		if ( ps->weaponTime )
		{
			if ( ps->weaponstate == WEAPON_RELOAD_START || ps->weaponstate == WEAPON_RELOAD_START_INTERUPT )
			{
				if ( weapDef->reloadStartAddTime < weapDef->reloadStartTime )
					reloadTime = weapDef->reloadStartAddTime;
				else
					reloadTime = weapDef->reloadStartTime;
			}
			else
			{
				clipIndex = BG_ClipForWeapon(ps->weapon);

				if ( !ps->ammoclip[clipIndex] && !weapDef->weaponType )
					reloadTime = weapDef->reloadEmptyTime;
				else
					reloadTime = weapDef->reloadTime;

				if ( weapDef->reloadAddTime && weapDef->reloadAddTime < reloadTime )
				{
					reloadTime = weapDef->reloadAddTime;
				}
			}

			boltTime = reloadTime;

			if ( weapDef->rechamberBoltTime < reloadTime )
				boltTime = weapDef->rechamberBoltTime;
			else
				boltTime = qtrue;

			reloadTime -= boltTime;

			if ( reloadTime < 1 )
			{
				PM_ReloadClip(ps);
			}
			else
			{
				ps->weaponDelay = reloadTime;
			}
		}
		else
		{
			PM_ReloadClip(ps);
		}
	}
	else
	{
		PM_ReloadClip(ps);
	}
}

/*
===============
PM_Weapon_FinishReloadStart
===============
*/
void PM_Weapon_FinishReloadStart( playerState_t *ps, qboolean delayedAction )
{
	WeaponDef *weapDef;

	assert(ps);
	weapDef = BG_GetWeaponDef(ps->weapon);
	assert((ps->weaponstate == WEAPON_RELOAD_START) || (ps->weaponstate == WEAPON_RELOAD_START_INTERUPT));

	if ( delayedAction )
	{
		PM_Weapon_ReloadDelayedAction(ps);
	}

	if ( !ps->weaponTime )
	{

		if ( ps->weaponstate != WEAPON_RELOAD_START_INTERUPT || !ps->ammoclip[BG_ClipForWeapon(ps->weapon)] )
		{
			if ( PM_Weapon_AllowReload(ps) )
			{
				PM_SetReloadingState(ps);
				return;
			}
		}

		Com_BitClear(ps->weaponrechamber, ps->weapon);

		if ( weapDef->reloadEndTime )
		{
			ps->weaponstate = WEAPON_RELOAD_END;
			PM_StartWeaponAnim(ps, WEAP_RELOAD_END);

			ps->weaponTime = weapDef->reloadEndTime;
			PM_AddEvent(ps, EV_RELOAD_END);
		}
		else
		{
			ps->weaponstate = WEAPON_READY;
			PM_StartWeaponAnim(ps, WEAP_IDLE);
		}
	}
}

/*
===============
PM_Weapon_FinishReload
===============
*/
void PM_Weapon_FinishReload( playerState_t *ps, qboolean delayedAction )
{
	WeaponDef *weapDef;

	assert(ps);
	assert(ps->weaponstate == WEAPON_RELOADING || ps->weaponstate == WEAPON_RELOADING_INTERUPT);

	weapDef = BG_GetWeaponDef(ps->weapon);

	if ( delayedAction )
	{
		PM_Weapon_ReloadDelayedAction(ps);

		if ( ps->weaponTime )
		{
			return;
		}
	}

	if ( !ps->weaponTime )
	{

		Com_BitClear(ps->weaponrechamber, ps->weapon);

		if ( weapDef->segmentedReload )
		{
			if ( ps->weaponstate != WEAPON_RELOADING_INTERUPT )
			{
				if ( PM_Weapon_AllowReload(ps) )
				{
					PM_SetReloadingState(ps);
					return;
				}
			}

			if ( weapDef->reloadEndTime )
			{
				ps->weaponstate = WEAPON_RELOAD_END;
				PM_StartWeaponAnim(ps, WEAP_RELOAD_END);

				ps->weaponTime = weapDef->reloadEndTime;
				PM_AddEvent(ps, EV_RELOAD_END);
			}
			else
			{
				ps->weaponstate = WEAPON_READY;
				PM_StartWeaponAnim(ps, WEAP_IDLE);
			}
		}
		else
		{
			ps->weaponstate = WEAPON_READY;
			PM_StartWeaponAnim(ps, WEAP_IDLE);
		}
	}
}

/*
===============
PM_Weapon_FinishReloadEnd
===============
*/
void PM_Weapon_FinishReloadEnd( playerState_t *ps )
{
	assert(ps);
	assert(ps->weaponstate == WEAPON_RELOAD_END);

	ps->weaponstate = WEAPON_READY;
	PM_StartWeaponAnim(ps, WEAP_IDLE);
}

/*
===============
PM_Weapon_CheckForReload
===============
*/
void PM_Weapon_CheckForReload( pmove_t *pm )
{
	WeaponDef *weapDef;
	qboolean reloadRequested;
	qboolean doReload = qfalse;
	int clipIndex;
	int ammoIndex;
	playerState_t *ps;

	assert(pm);
	ps = pm->ps;
	assert(ps);

	weapDef = BG_GetWeaponDef(ps->weapon);

	// Using binoculars
	if ( ps->weaponstate >= WEAPON_BINOCULARS_INIT && ps->weaponstate <= WEAPON_BINOCULARS_END )
	{
		return;
	}

	// Using offhand
	if ( ps->weaponstate >= WEAPON_OFFHAND_INIT && ps->weaponstate <= WEAPON_OFFHAND_END )
	{
		return;
	}

	reloadRequested = qfalse;

	if ( pm->cmd.buttons & BUTTON_RELOAD )
	{
		reloadRequested = qtrue;
	}

	// Deprecated flag, not used
	if ( ps->pm_flags & PMF_RELOAD )
	{
		ps->pm_flags &= ~PMF_RELOAD;
		reloadRequested = qtrue;
	}

	if ( weapDef->segmentedReload && ( ps->weaponstate == WEAPON_RELOAD_START || ps->weaponstate == WEAPON_RELOADING ) )
	{
		if ( pm->cmd.buttons & BUTTON_ATTACK && !( pm->oldcmd.buttons & BUTTON_ATTACK ) )
		{
			if ( ps->weaponstate == WEAPON_RELOAD_START )
			{
				ps->weaponstate = WEAPON_RELOAD_START_INTERUPT;
			}
			else if ( ps->weaponstate == WEAPON_RELOADING )
			{
				ps->weaponstate = WEAPON_RELOADING_INTERUPT;
			}
		}
	}

	switch ( ps->weaponstate )
	{
	case WEAPON_RAISING:
	case WEAPON_DROPPING:
	case WEAPON_MELEE_INIT:
	case WEAPON_MELEE_FIRE:
		return;

	case WEAPON_RELOADING:
	case WEAPON_RELOADING_INTERUPT:
	case WEAPON_RELOAD_START:
	case WEAPON_RELOAD_START_INTERUPT:
	case WEAPON_RELOAD_END:
		if ( pm->proneChange )
		{
			if ( !BG_WeaponIsClipOnly(ps->weapon) )
			{
				BG_AnimScriptEvent(ps, ANIM_ET_RELOAD, qfalse, qtrue);
			}
		}
		return;

	default:
		break;
	}

	clipIndex = BG_ClipForWeapon(ps->weapon);
	ammoIndex = BG_AmmoForWeapon(ps->weapon);

	if ( reloadRequested )
	{
		if ( PM_Weapon_AllowReload(ps) )
		{
			doReload = qtrue;
		}
	}

	if ( !ps->ammoclip[clipIndex] && ps->ammo[ammoIndex] )
	{
		if ( ps->weaponstate != WEAPON_FIRING )
		{
			// No ammo and player is proning and moving, reload not allowed
			if ( !(ps->pm_flags & PMF_PRONE) || !pm->cmd.forwardmove && !pm->cmd.rightmove )
			{
				doReload = qtrue;
			}
		}
	}

	if ( doReload )
	{
		PM_BeginWeaponReload(ps);
	}
}

/*
===============
PM_SwitchIfEmpty
===============
*/
void PM_SwitchIfEmpty( playerState_t *ps )
{
	assert(ps);

	if ( !BG_WeaponIsClipOnly(ps->weapon) )
	{
		return;
	}

	if ( ps->ammoclip[BG_ClipForWeapon(ps->weapon)] )
	{
		return;
	}

	if ( ps->ammo[BG_AmmoForWeapon(ps->weapon)] )
	{
		return;
	}

	BG_TakePlayerWeapon(ps, ps->weapon);
	PM_AddEvent(ps, EV_NOAMMO);
}

/*
===============
PM_AdjustAimSpreadScale
===============
*/
void PM_AdjustAimSpreadScale( pmove_t *pm, pml_t *pml )
{
	WeaponDef *weapDef;
	int i;
	float increase;
	float decrease;
	float viewchange;
	float wpnScale;
	playerState_t *ps;

	ps = pm->ps;
	assert(ps);
	float turnScale = 0.0099999998f;
	float moveScale = 0.0099999998f;

	weapDef = BG_GetWeaponDef(ps->weapon);
	wpnScale = weapDef->hipSpreadDecayRate;

	if ( wpnScale != 0 )
	{
		if ( ps->groundEntityNum == ENTITYNUM_NONE && ps->pm_type != PM_NORMAL_LINKED )
		{
			wpnScale *= 0.5f;
		}
		else if ( ps->eFlags & EF_PRONE )
		{
			wpnScale *= weapDef->hipSpreadProneDecay;
		}
		else if ( ps->eFlags & EF_CROUCH )
		{
			wpnScale *= weapDef->hipSpreadDuckedDecay;
		}

		decrease = wpnScale * pml->frametime;

		if ( ps->fWeaponPosFrac == 1 )
		{
			increase = 0;
		}
		else
		{
			viewchange = 0;

			if ( weapDef->hipSpreadTurnAdd != 0 )
			{
				for ( i = 0; i < 2; i++ )
				{
					viewchange = ( I_fabs(AngleSubtract(pm->cmd.angles[i] * (float)(360.0 / 65536), pm->oldcmd.angles[i] * (float)(360.0 / 65536)))
					             * weapDef->hipSpreadTurnAdd * 0.0099999998f ) / pml->frametime + viewchange;
				}
			}

			if ( weapDef->hipSpreadMoveAdd != 0 && (pm->cmd.forwardmove || pm->cmd.rightmove) )
			{
				if ( Vec2Multiply(ps->velocity) > I_square(bg_aimSpreadMoveSpeedThreshold->current.decimal) )
				{
					viewchange = viewchange + weapDef->hipSpreadMoveAdd;
				}
			}

			if ( ps->groundEntityNum == ENTITYNUM_NONE && ps->pm_type != PM_NORMAL_LINKED )
			{
				for ( i = 0; i < 2; i++ )
				{
					viewchange = viewchange + 1.28f;
				}
			}

			increase = viewchange * pml->frametime;
		}
	}
	else
	{
		increase = 0;
		decrease = 1;
	}

	ps->aimSpreadScale += (increase - decrease) * 255;

	if ( ps->aimSpreadScale < 0 )
	{
		ps->aimSpreadScale = 0;
	}
	else if ( ps->aimSpreadScale > 255 )
	{
		ps->aimSpreadScale = 255;
	}
}

/*
===============
PM_Weapon_WeaponTimeAdjust
===============
*/
qboolean PM_Weapon_WeaponTimeAdjust( pmove_t *pm, pml_t *pml )
{
	WeaponDef *weapDef;
	playerState_t *ps;
	qboolean result;

	ps = pm->ps;
	assert(ps);

	weapDef = BG_GetWeaponDef(ps->weapon);

	if ( ps->weaponRestrictKickTime > 0 )
	{
		ps->weaponRestrictKickTime -= pml->msec;
		ps->weaponRestrictKickTime = I_max(ps->weaponRestrictKickTime, 0);
	}

	if ( ps->weaponTime )
	{
		ps->weaponTime -= pml->msec;

		if ( ps->weaponTime <= 0 )
		{
			if ( ( ps->weaponstate < WEAPON_OFFHAND_INIT || ps->weaponstate > WEAPON_OFFHAND_END )
			        && weapDef->semiAuto
			        && pm->cmd.buttons & BUTTON_ATTACK
			        && ps->weapon == pm->cmd.weapon
			        && PM_WeaponAmmoAvailable(ps) )
			{
				ps->weaponTime = 1;

				if ( ps->weaponstate == WEAPON_RELOADING
				        || ps->weaponstate == WEAPON_RELOAD_START
				        || ps->weaponstate == WEAPON_RELOAD_END
				        || ps->weaponstate == WEAPON_RELOAD_START_INTERUPT
				        || ps->weaponstate == WEAPON_RELOADING_INTERUPT )
				{
					ps->weaponTime = 0;
				}

				if ( (ps->weaponstate == WEAPON_RELOADING
				        || ps->weaponstate == WEAPON_RELOAD_START
				        || ps->weaponstate == WEAPON_RELOAD_END
				        || ps->weaponstate == WEAPON_RELOAD_START_INTERUPT
				        || ps->weaponstate == WEAPON_RELOADING_INTERUPT)
				        && ps->weaponstate != WEAPON_RELOAD_END
				        && weapDef->segmentedReload
				        && weapDef->reloadStartTime )
				{
					ps->weaponTime = 0;
				}
				else if ( ps->weaponstate == WEAPON_RECHAMBERING )
				{
					PM_Weapon_FinishRechamber(ps);
				}
				else if ( ps->weaponstate == WEAPON_FIRING
				          || ps->weaponstate == WEAPON_RECHAMBERING
				          || ps->weaponstate == WEAPON_MELEE_INIT
				          || ps->weaponstate == WEAPON_MELEE_FIRE )
				{
					PM_ContinueWeaponAnim(ps, WEAP_IDLE);
					ps->weaponstate = WEAPON_READY;
				}
			}
			else
			{
				ps->weaponTime = 0;
			}
		}
	}

	if ( ps->weaponDelay )
	{
		ps->weaponDelay -= pml->msec;

		if ( ps->weaponDelay <= 0 )
		{
			ps->weaponDelay = 0;
			result = qtrue;
			goto weaponTimeAdjust_done;
		}
	}

	result = qfalse;

weaponTimeAdjust_done:
	return result;
}

/*
===============
PM_Weapon_CheckForChangeWeapon
===============
*/
void PM_Weapon_CheckForChangeWeapon( pmove_t *pm )
{
	playerState_t *ps;

	assert(pm);
	ps = pm->ps;
	assert(ps);

	// Using binoculars
	if ( ps->weaponstate >= WEAPON_BINOCULARS_INIT && ps->weaponstate <= WEAPON_BINOCULARS_END )
	{
		return;
	}

	// Using offhand
	if ( ps->weaponstate >= WEAPON_OFFHAND_INIT && ps->weaponstate <= WEAPON_OFFHAND_END )
	{
		return;
	}

	if ( !( !ps->weaponTime
	        || ps->weaponstate == WEAPON_RELOADING
	        || ps->weaponstate == WEAPON_RELOAD_START
	        || ps->weaponstate == WEAPON_RELOAD_END
	        || ps->weaponstate == WEAPON_RELOAD_START_INTERUPT
	        || ps->weaponstate == WEAPON_RELOADING_INTERUPT
	        || ps->weaponstate == WEAPON_RECHAMBERING
	        || ps->weaponstate != WEAPON_FIRING
	        && ps->weaponstate != WEAPON_RECHAMBERING
	        && ps->weaponstate != WEAPON_MELEE_INIT
	        && ps->weaponstate != WEAPON_MELEE_FIRE
	        && !ps->weaponDelay ) )
	{
		return;
	}

	if ( Mantle_IsWeaponInactive(ps) )
	{
		if ( !ps->weapon )
		{
			return;
		}

		PM_BeginWeaponChange(ps, WP_NONE);
		return;
	}

	if ( ps->pm_flags & PMF_LADDER )
	{
		if ( !ps->weapon )
		{
			return;
		}

		PM_BeginWeaponChange(ps, WP_NONE);
		return;
	}

	if ( ps->pm_flags & PMF_DISABLEWEAPON )
	{
		if ( !ps->weapon )
		{
			return;
		}

		PM_BeginWeaponChange(ps, WP_NONE);
		return;
	}

	if ( ps->weapon != pm->cmd.weapon )
	{
		if ( ( !(ps->pm_flags & PMF_FROZEN) || !ps->weapon ) )
		{
			if ( !pm->cmd.weapon || BG_IsWeaponValid(ps, pm->cmd.weapon) )
			{
				PM_BeginWeaponChange(ps, pm->cmd.weapon);
				return;
			}
		}
	}

	if ( ps->weapon )
	{
		if ( !Com_BitCheck(ps->weapons, ps->weapon) )
		{
			PM_BeginWeaponChange(ps, WP_NONE);
		}
	}
}

/*
===============
PM_Weapon_FinishFiring
===============
*/
qboolean PM_Weapon_FinishFiring( pmove_t *pm, qboolean delayedAction )
{
	playerState_t *ps;

	assert(pm);

	if ( !(pm->cmd.buttons & BUTTON_ATTACK) && !delayedAction )
	{
		ps = pm->ps;
		assert(ps);

		if ( ps->weaponstate == WEAPON_FIRING )
		{
			PM_ContinueWeaponAnim(ps, WEAP_IDLE);
		}

		ps->weaponstate = WEAPON_READY;
		return qtrue;
	}

	return qfalse;
}

/*
===============
PM_Weapon_StartFiring
===============
*/
void PM_Weapon_StartFiring( playerState_t *ps, qboolean delayedAction )
{
	WeaponDef *weapDef;

	assert(ps);

	weapDef = BG_GetWeaponDef(ps->weapon);

	if ( weapDef->weaponType == WEAPTYPE_GRENADE )
	{
		if ( !delayedAction )
		{
			if ( PM_WeaponAmmoAvailable(ps) )
			{
				ps->grenadeTimeLeft = weapDef->fuseTime;

				PM_StartWeaponAnim(ps, WEAP_HOLD_FIRE);
				PM_AddEvent(ps, EV_PULLBACK_WEAPON);
			}

			ps->weaponDelay = weapDef->holdFireTime;
			ps->weaponTime = 0;
		}
	}
	else
	{

		ps->weaponDelay = weapDef->fireDelay;
		ps->weaponTime = weapDef->fireTime;

		if ( weapDef->adsFire )
		{
			ps->weaponDelay = (int)((1.0f - ps->fWeaponPosFrac) * (1.0f / weapDef->OOPosAnimLength[0]));
		}

		BG_AnimScriptEvent(ps, ANIM_ET_FIREWEAPON, qfalse, qtrue);

		if ( weapDef->boltAction )
		{
			Com_BitSet(ps->weaponrechamber, ps->weapon);
		}

		if ( ps->weaponstate != WEAPON_FIRING )
		{
			if ( ps->fWeaponPosFrac >= 1.0f )
				ps->weaponRestrictKickTime = weapDef->fireTime * weapDef->adsGunKickReducedKickBullets + weapDef->fireDelay;
			else
				ps->weaponRestrictKickTime = weapDef->fireTime * weapDef->hipGunKickReducedKickBullets + weapDef->fireDelay;
		}

	}

	ps->weaponstate = WEAPON_FIRING;
	PM_SetProneMovementOverride(ps);
}

// Shots fired per trigger pull; always one in this build.
int BG_WeaponShotCount( int weapIndex )
{
	return 1;
}

/*
===============
PM_Weapon_CheckFiringAmmo
===============
*/
qboolean PM_Weapon_CheckFiringAmmo( playerState_t *ps )
{
	WeaponDef *weapDef;
	int shotCount;
	int ammoAvail;
	int hasAmmo;
	int notGrenade = 1;

	assert(ps);
	assert(ps->weapon != WP_NONE);

	weapDef = BG_GetWeaponDef(ps->weapon);
	shotCount = BG_WeaponShotCount(ps->weapon);
	ammoAvail = PM_WeaponAmmoAvailable(ps);

	if ( shotCount > ammoAvail )
	{
		hasAmmo = shotCount <= ps->ammo[BG_AmmoForWeapon(ps->weapon)];

		if ( weapDef->weaponType == WEAPTYPE_GRENADE )
		{
			notGrenade = 0;
		}

		if ( notGrenade )
		{
			if ( !hasAmmo )
			{
				PM_AddEvent(ps, EV_NOAMMO);
			}
		}

		if ( hasAmmo )
		{
			PM_BeginWeaponReload(ps);
		}
		else
		{
			Com_BitClear(ps->weaponrechamber, ps->weapon);
			PM_ContinueWeaponAnim(ps, WEAP_IDLE);
			ps->weaponTime += 500;
		}

		return qfalse;
	}

	return qtrue;
}

/*
===============
PM_Weapon_SetFPSFireAnim
===============
*/
void PM_Weapon_SetFPSFireAnim( playerState_t *ps )
{
	int anim;

	if ( ps->fWeaponPosFrac > 0.75f )
	{
		if ( PM_WeaponClipEmpty(ps) )
			anim = WEAP_ADS_ATTACK_LASTSHOT;
		else
			anim = WEAP_ADS_ATTACK;
	}
	else
	{
		if ( PM_WeaponClipEmpty(ps) )
			anim = WEAP_ATTACK_LASTSHOT;
		else
			anim = WEAP_ATTACK;
	}

	PM_StartWeaponAnim(ps, anim);
}

/*
===============
PM_Weapon_AddFiringAimSpreadScale
===============
*/
void PM_Weapon_AddFiringAimSpreadScale( playerState_t *ps )
{
	WeaponDef *weapDef;

	assert(ps);
	weapDef = BG_GetWeaponDef(ps->weapon);

	if ( ps->fWeaponPosFrac == 1.0f )
	{
		return;
	}

	ps->aimSpreadScale += weapDef->hipSpreadFireAdd * 255.0f;

	if ( ps->aimSpreadScale > 255 )
	{
		ps->aimSpreadScale = 255;
	}
}

/*
===============
PM_Weapon_FireWeapon
===============
*/
void PM_Weapon_FireWeapon( playerState_t *ps, qboolean delayedAction )
{
	WeaponDef *weapDef;

	assert(ps);
	weapDef = BG_GetWeaponDef(ps->weapon);

	PM_Weapon_StartFiring(ps, delayedAction);

	if ( !PM_Weapon_CheckFiringAmmo(ps) )
	{
		return;
	}

	if ( ps->weaponDelay )
	{
		return;
	}

	if ( PM_WeaponAmmoAvailable(ps) != -1 && !(ps->eFlags & EF_TURRET_ACTIVE) )
	{
		PM_WeaponUseAmmo(ps, ps->weapon, BG_WeaponShotCount(ps->weapon));
	}

	if ( weapDef->weaponType == WEAPTYPE_GRENADE )
	{
		ps->weaponTime = weapDef->fireTime;
	}

	PM_Weapon_SetFPSFireAnim(ps);

	if ( PM_WeaponClipEmpty(ps) )
		PM_AddEvent(ps, EV_FIRE_WEAPON_LASTSHOT);
	else
		PM_AddEvent(ps, EV_FIRE_WEAPON);

	PM_HoldBreathFire(ps);
	PM_Weapon_AddFiringAimSpreadScale(ps);
	PM_SwitchIfEmpty(ps);
}

/*
===============
PM_Weapon_FireMelee
===============
*/
void PM_Weapon_FireMelee( playerState_t *ps )
{
	WeaponDef *weapDef;

	assert(ps);
	weapDef = BG_GetWeaponDef(ps->weapon);

	if ( ps->weaponTime < weapDef->meleeTime - weapDef->meleeDelay )
	{
		ps->weaponTime = weapDef->meleeTime - weapDef->meleeDelay;
	}

	PM_AddEvent(ps, EV_FIRE_MELEE);
	ps->weaponstate = WEAPON_MELEE_FIRE;

	PM_SetProneMovementOverride(ps);
}

/*
===============
PM_Weapon_FinishMelee
===============
*/
void PM_Weapon_FinishMelee( playerState_t *ps )
{
	assert(ps);
	PM_ContinueWeaponAnim(ps, WEAP_IDLE);
	ps->weaponstate = WEAPON_READY;
}

/*
===============
PM_Weapon_CheckForMelee
===============
*/
void PM_Weapon_CheckForMelee( pmove_t *pm, qboolean delayedAction )
{
	WeaponDef *weapDef;
	playerState_t *ps;

	assert(pm);
	ps = pm->ps;
	assert(ps);

	weapDef = BG_GetWeaponDef(ps->weapon);

	// Using binoculars
	if ( ps->weaponstate >= WEAPON_BINOCULARS_INIT && ps->weaponstate <= WEAPON_BINOCULARS_END )
	{
		return;
	}

	// Using grenade
	if ( ps->weaponstate >= WEAPON_OFFHAND_INIT && ps->weaponstate <= WEAPON_OFFHAND_END )
	{
		return;
	}

	if ( !weapDef->meleeDamage )
	{
		return;
	}

	if ( delayedAction )
	{
		return;
	}

	// Reloading
	if ( ps->weaponDelay
	        && ps->weaponstate != WEAPON_RELOADING
	        && ps->weaponstate != WEAPON_RELOAD_START
	        && ps->weaponstate != WEAPON_RELOAD_END
	        && ps->weaponstate != WEAPON_RELOAD_START_INTERUPT
	        && ps->weaponstate != WEAPON_RELOADING_INTERUPT )
	{
		return;
	}

	if ( !(pm->cmd.buttons & BUTTON_MELEE) )
	{
		ps->pm_flags &= ~PMF_MELEE;
		return;
	}

	if ( ps->pm_flags & PMF_MELEE )
	{
		return;
	}

	ps->pm_flags |= PMF_MELEE;

	if ( ps->fWeaponPosFrac > 0 && weapDef->adsOverlayReticle )
	{
		return;
	}

	switch ( ps->weaponstate )
	{
	case WEAPON_RAISING:
	case WEAPON_DROPPING:
	case WEAPON_MELEE_INIT:
	case WEAPON_MELEE_FIRE:
		return;

	default:
		break;
	}

	BG_AnimScriptEvent(ps, ANIM_ET_MELEEATTACK, qfalse, qtrue);
	PM_StartWeaponAnim(ps, WEAP_MELEE_ATTACK);

	PM_AddEvent(ps, EV_MELEE_SWIPE);

	if ( weapDef->meleeDelay )
	{
		ps->weaponTime = weapDef->meleeTime;
		ps->weaponDelay = weapDef->meleeDelay;
		ps->weaponstate = WEAPON_MELEE_INIT;

		PM_SetProneMovementOverride(ps);
	}
	else
	{
		PM_Weapon_FireMelee(ps);
	}
}

/*
===============
PM_Weapon_Idle
===============
*/
void PM_Weapon_Idle( playerState_t *ps )
{
	assert(ps);

	ps->pm_flags &= ~(PMF_FRAG | PMF_ADS_OVERRIDE);

	if ( ps->weaponstate >= WEAPON_BINOCULARS_INIT && ps->weaponstate <= WEAPON_BINOCULARS_END )
	{
		PM_ExitBinocularsQuick(ps);
	}

	ps->weaponTime = 0;
	ps->weaponDelay = 0;
	ps->weaponstate = WEAPON_READY;

	PM_StartWeaponAnim(ps, WEAP_IDLE);
}

/*
===============
PM_Weapon_OffHandInit
===============
*/
void PM_Weapon_OffHandInit( playerState_t *ps )
{
	WeaponDef *weapDef;

	assert(ps);
	assert(ps->offHandIndex != WP_NONE);

	weapDef = BG_GetWeaponDef(ps->weapon);

	ps->weaponstate = WEAPON_OFFHAND_INIT;
	ps->weaponTime = weapDef->quickDropTime;
	ps->weaponDelay = 0;

	ps->pm_flags &= ~PMF_FRAG;

	PM_ExitAimDownSight(ps);
	PM_StartWeaponAnim(ps, WEAP_EMPTY_DROP);
}

/*
===============
PM_Weapon_OffHandPrepare
===============
*/
void PM_Weapon_OffHandPrepare( playerState_t *ps )
{
	WeaponDef *weapDef;

	assert(ps);
	assert(ps->offHandIndex != WP_NONE);

	weapDef = BG_GetWeaponDef(ps->offHandIndex);

	ps->weaponstate = WEAPON_OFFHAND_PREPARE;
	ps->weaponTime = weapDef->holdFireTime;
	ps->weaponDelay = 0;
	ps->pm_flags |= PMF_FRAG;

	BG_AddPredictableEventToPlayerstate(EV_PREP_OFFHAND, ps->offHandIndex, ps);
	PM_StartWeaponAnim(ps, WEAP_HOLD_FIRE);

	PM_SetProneMovementOverride(ps);
}

/*
===============
PM_Weapon_OffHandHold
===============
*/
void PM_Weapon_OffHandHold( playerState_t *ps )
{
	WeaponDef *weapDef;

	assert(ps);
	assert(ps->offHandIndex != WP_NONE);

	weapDef = BG_GetWeaponDef(ps->offHandIndex);

	ps->weaponstate = WEAPON_OFFHAND_HOLD;
	ps->weaponTime = 0;
	ps->weaponDelay = 0;
	ps->pm_flags |= PMF_FRAG;
	ps->grenadeTimeLeft = weapDef->fuseTime;
}

/*
===============
PM_Weapon_OffHand
===============
*/
void PM_Weapon_OffHand( pmove_t *pm )
{
	WeaponDef *weapDef;
	playerState_t *ps;

	assert(pm);
	ps = pm->ps;
	assert(ps);

	assert(ps->offHandIndex != WP_NONE);

	if ( pm->oldcmd.buttons & (BUTTON_FRAG | BUTTON_SMOKE) && pm->cmd.buttons & (BUTTON_FRAG | BUTTON_SMOKE) )
	{
		ps->weaponDelay = 1;
		return;
	}

	weapDef = BG_GetWeaponDef(ps->offHandIndex);

	ps->weaponstate = WEAPON_OFFHAND;
	ps->weaponTime = weapDef->fireTime;
	ps->weaponDelay = 0;

	ps->pm_flags |= PMF_FRAG;

	BG_AddPredictableEventToPlayerstate(EV_USE_OFFHAND, ps->offHandIndex, ps);
	PM_StartWeaponAnim(ps, WEAP_ATTACK);

	PM_WeaponUseAmmo(ps, ps->offHandIndex, 1);
	BG_AnimScriptEvent(ps, ANIM_ET_FIREWEAPON, qfalse, qtrue);

	if ( !BG_WeaponAmmo(ps, ps->offHandIndex) )
	{
		PM_AddEvent(ps, EV_EMPTY_OFFHAND);
	}
}

/*
===============
PM_Weapon_OffHandEnd
===============
*/
void PM_Weapon_OffHandEnd( playerState_t *ps )
{
	WeaponDef *weapDef;

	assert(ps);

	if ( !ps->weapon )
	{
		ps->weaponTime = 0;
		ps->weaponDelay = 1;
	}
	else
	{
		weapDef = BG_GetWeaponDef(ps->weapon);
		ps->weaponTime = weapDef->quickRaiseTime;
		ps->weaponDelay = 0;

		PM_StartWeaponAnim(ps, WEAP_EMPTY_RAISE);
	}

	ps->weaponstate = WEAPON_OFFHAND_END;
	ps->pm_flags &= ~(PMF_FRAG | PMF_ADS_OVERRIDE);
}

/*
===============
PM_SendEmtpyOffhandEvent
===============
*/
void PM_SendEmtpyOffhandEvent( playerState_t *ps, int offhandSlot )
{
	assert(ps);
	PM_AddEvent(ps, EV_EMPTY_OFFHAND);
}

/*
===============
PM_Weapon_CheckForOffHand
===============
*/
void PM_Weapon_CheckForOffHand( pmove_t *pm )
{
	WeaponDef *weapDef;
	playerState_t *ps;
	int offHandIndex;
	int offhandSlot;

	assert(pm);
	ps = pm->ps;
	assert(ps);

	if ( ps->eFlags & EF_TURRET_ACTIVE )
	{
		return;
	}

	if ( ps->pm_flags & PMF_DISABLEWEAPON )
	{
		return;
	}

	// Using binoculars
	if ( ps->weaponstate >= WEAPON_BINOCULARS_INIT && ps->weaponstate <= WEAPON_BINOCULARS_END )
	{
		return;
	}

	// Using offhand
	if ( ps->weaponstate >= WEAPON_OFFHAND_INIT && ps->weaponstate <= WEAPON_OFFHAND_END && ps->weaponstate != WEAPON_OFFHAND_END )
	{
		return;
	}

	if ( Com_BitCheck(ps->weapons, pm->cmd.offHandIndex) )
	{
		ps->offHandIndex = pm->cmd.offHandIndex;
	}

	assert( ps->offHandIndex == WP_NONE || BG_GetWeaponDef( ps->offHandIndex )->offhandClass != OFFHAND_CLASS_NONE );

	if ( pm->cmd.buttons & BUTTON_FRAG )
	{
		offhandSlot = OFFHAND_SLOT_FRAG_GRENADE;
	}
	else if ( pm->cmd.buttons & BUTTON_SMOKE )
	{
		offhandSlot = OFFHAND_SLOT_SMOKE_GRENADE;
	}
	else
	{
		return;
	}

	offHandIndex = BG_GetFirstAvailableOffhand(ps, offhandSlot);
	assert( offHandIndex == WP_NONE || BG_GetWeaponDef( offHandIndex )->offhandClass != OFFHAND_CLASS_NONE );

	if ( offHandIndex )
	{
		BG_AddPredictableEventToPlayerstate(EV_SWITCH_OFFHAND, offHandIndex, ps);
		ps->offHandIndex = offHandIndex;
	}
	else
	{
		PM_SendEmtpyOffhandEvent(ps, offhandSlot);
		return;
	}

	weapDef = BG_GetWeaponDef(ps->offHandIndex);

	if ( weapDef->weaponType != WEAPTYPE_GRENADE )
	{
		Com_Error(ERR_DROP, "[%s] Only grenades are currently supported for off hand use\n", weapDef->szInternalName);
	}

	if ( !weapDef->offhandClass )
	{
		Com_Error(ERR_DROP, "[%s] No offhand class set\n", weapDef->szInternalName);
	}

	if ( !ps->weapon || ps->weaponstate == WEAPON_OFFHAND_END )
		PM_Weapon_OffHandPrepare(ps);
	else
		PM_Weapon_OffHandInit(ps);
}

/*
===============
PM_UpdateGrenadeThrow
===============
*/
bool PM_UpdateGrenadeThrow( playerState_t *ps, pml_t *pml )
{
	WeaponDef *weapDef;
	int weapIndex;

	assert(ps);
	assert(pml);

	if ( !(ps->pm_flags & PMF_FRAG) )
	{
		return false;
	}

	weapIndex = ps->offHandIndex;
	weapDef = BG_GetWeaponDef(weapIndex);

	if ( weapDef->weaponType != WEAPTYPE_GRENADE )
	{
		return false;
	}

	if ( ps->grenadeTimeLeft <= 0 )
	{
		return false;
	}

	if ( weapDef->cookOffHold )
	{
		ps->grenadeTimeLeft -= pml->msec;
	}

	if ( ps->grenadeTimeLeft <= 0 )
	{
		PM_WeaponUseAmmo(ps, weapIndex, 1);
		PM_AddEvent(ps, EV_GRENADE_SUICIDE);

		return true;
	}

	return false;
}

/*
===============
PM_EnterBinoculars
===============
*/
void PM_EnterBinoculars( playerState_t *ps )
{
	assert(ps);
	ps->pm_flags |= PMF_BINOCULARS;
	PM_AddEvent(ps, EV_BINOCULAR_ENTER);
}

/*
===============
PM_ExitBinoculars
===============
*/
void PM_ExitBinoculars( playerState_t *ps )
{
	assert(ps);
	ps->pm_flags &= ~PMF_BINOCULARS;
	PM_AddEvent(ps, EV_BINOCULAR_EXIT);
}

/*
===============
PM_ExitBinocularsQuick
===============
*/
void PM_ExitBinocularsQuick( playerState_t *ps )
{
	assert(ps);

	ps->fWeaponPosFrac = 0;

	PM_ExitAimDownSight(ps);
	PM_ExitBinoculars(ps);
}

/*
===============
Binocular_State_Init
===============
*/
void Binocular_State_Init( playerState_t *ps )
{
	WeaponDef *weapDef;

	assert(ps);
	weapDef = BG_GetWeaponDef(ps->weapon);

	PM_ExitAimDownSight(ps);

	ps->weaponTime = 0;
	ps->weaponDelay = weapDef->adsTransOutTime + player_adsExitDelay->current.integer;
	ps->weaponstate = WEAPON_BINOCULARS_INIT;
}

/*
===============
Binocular_State_Raise
===============
*/
void Binocular_State_Raise( playerState_t *ps )
{
	WeaponDef *weapDef;

	assert(ps);
	weapDef = BG_GetWeaponDef(ps->weapon);

	ps->weaponstate = WEAPON_BINOCULARS_RAISE;
	ps->weaponTime = weapDef->quickDropTime;
	ps->weaponDelay = 0;

	PM_StartWeaponAnim(ps, WEAP_EMPTY_DROP);
}

/*
===============
Binocular_State_Hold
===============
*/
void Binocular_State_Hold( playerState_t *ps )
{
	WeaponDef *weapDef;
	int weapIndex;

	assert(ps);

	weapIndex = BG_GetBinocularWeaponIndex();
	weapDef = BG_GetWeaponDef(weapIndex);

	ps->weaponstate = WEAPON_BINOCULARS_HOLD;
	ps->weaponTime = weapDef->adsTransInTime;
	ps->weaponDelay = 0;

	PM_EnterBinoculars(ps);
	PM_StartWeaponAnim(ps, WEAP_IDLE);
}

/*
===============
Binocular_State_Exit_ADS
===============
*/
void Binocular_State_Exit_ADS( playerState_t *ps )
{
	assert(ps);

	ps->weaponstate = WEAPON_BINOCULARS_EXIT_ADS;
	ps->weaponTime = 0;
	ps->weaponDelay = 0;
}

/*
===============
Binocular_State_Drop
===============
*/
void Binocular_State_Drop( playerState_t *ps )
{
	WeaponDef *weapDef;
	int weapIndex;

	assert(ps);

	weapIndex = BG_GetBinocularWeaponIndex();
	weapDef = BG_GetWeaponDef(weapIndex);

	ps->weaponstate = WEAPON_BINOCULARS_DROP;
	ps->weaponTime = weapDef->adsTransOutTime + player_adsExitDelay->current.integer;
	ps->weaponDelay = 0;

	PM_AddEvent(ps, EV_BINOCULAR_DROP);
	PM_StartWeaponAnim(ps, WEAP_IDLE);
}

/*
===============
Binocular_State_End
===============
*/
void Binocular_State_End( pmove_t *pm )
{
	WeaponDef *weapDef;
	playerState_t *ps;

	assert(pm);
	ps = pm->ps;
	assert(ps);

	ps->weaponstate = WEAPON_BINOCULARS_END;
	PM_ExitBinoculars(ps);

	if ( !pm->cmd.weapon )
	{
		ps->weaponTime = 0;
		ps->weaponDelay = 1;
	}
	else
	{
		weapDef = BG_GetWeaponDef(ps->weapon);
		ps->weaponTime = weapDef->quickRaiseTime;
		ps->weaponDelay = 0;
		PM_StartWeaponAnim(ps, WEAP_EMPTY_RAISE);
	}
}

/*
===============
PM_Weapon_CheckForBinoculars
===============
*/
void PM_Weapon_CheckForBinoculars( pmove_t *pm )
{
	WeaponDef *weapDef;
	int weaponButtons;
	bool pressed;
	bool raise;
	bool drop;
	bool fire;
	bool release;
	bool switching;
	bool weaponChanged;
	bool offHandChanged;
	playerState_t *ps;

	ps = pm->ps;
	weaponButtons = BUTTON_ATTACK | BUTTON_MELEE | BUTTON_RELOAD | BUTTON_FRAG | BUTTON_SMOKE;

	// Disable binoculars while using scope
	if ( !( ps->weaponstate >= WEAPON_BINOCULARS_INIT && ps->weaponstate <= WEAPON_BINOCULARS_END ) )
	{
		if ( ps->fWeaponPosFrac > 0.0f )
		{
			weapDef = BG_GetWeaponDef(ps->weapon);

			if ( weapDef->adsOverlayReticle )
			{
				return;
			}
		}

		if ( pm->cmd.buttons & weaponButtons )
		{
			return;
		}
	}

	// Optimization: no states except READY and binoculars are allowed
	if ( ps->weaponstate == WEAPON_RELOADING
	        || ps->weaponstate == WEAPON_RELOAD_START
	        || ps->weaponstate == WEAPON_RELOAD_END
	        || ps->weaponstate == WEAPON_RELOAD_START_INTERUPT
	        || ps->weaponstate == WEAPON_RELOADING_INTERUPT
	        || ps->weaponstate == WEAPON_FIRING
	        || ps->weaponstate == WEAPON_RECHAMBERING
	        || ps->weaponstate == WEAPON_MELEE_INIT
	        || ps->weaponstate == WEAPON_MELEE_FIRE
	        || ps->weaponstate == WEAPON_MELEE_INIT
	        || ps->weaponstate == WEAPON_MELEE_FIRE
	        || ps->weaponstate == WEAPON_RAISING
	        || ps->weaponstate == WEAPON_DROPPING )
	{
		return;
	}

	if ( ps->weaponstate >= WEAPON_OFFHAND_INIT && ps->weaponstate <= WEAPON_OFFHAND_END )
	{
		return;
	}

	// Switching weapon ?
	weaponChanged = ( pm->oldcmd.weapon != pm->cmd.weapon );
	offHandChanged = ( pm->oldcmd.offHandIndex != pm->cmd.offHandIndex );
	switching = ( ps->damageCount || weaponChanged || offHandChanged || ( pm->cmd.buttons & weaponButtons ) );

	if ( ps->weaponstate >= WEAPON_BINOCULARS_INIT && ps->weaponstate <= WEAPON_BINOCULARS_END && switching )
	{
		PM_Weapon_Idle(ps);

		if ( pm->cmd.buttons & BUTTON_ATTACK )
		{
			ps->weaponTime = 1;
		}

		return;
	}

	if ( pm->cmd.buttons & BUTTON_ATTACK )
	{
		return;
	}

	// Fire or release, used by Scr_Notify system
	if ( ps->weaponstate == WEAPON_BINOCULARS_EXIT_ADS )
	{
		fire = ( !( pm->oldcmd.buttons & ( BUTTON_USE | BUTTON_USERELOAD ) ) && ( pm->cmd.buttons & ( BUTTON_USE | BUTTON_USERELOAD ) ) );
		release = ( ( pm->oldcmd.buttons & ( BUTTON_USE | BUTTON_USERELOAD ) ) && !( pm->cmd.buttons & ( BUTTON_USE | BUTTON_USERELOAD ) ) );

		if ( fire )
		{
			PM_AddEvent(ps, EV_BINOCULAR_FIRE);
		}
		else if ( release )
		{
			PM_AddEvent(ps, EV_BINOCULAR_RELEASE);
		}
	}

	// default mode is toggle
	if ( player_toggleBinoculars->current.boolean )
	{
		pressed = ( !( pm->oldcmd.buttons & BUTTON_BINOCULARS ) && pm->cmd.buttons & BUTTON_BINOCULARS );
		raise = ( pressed && ( ps->weaponstate < WEAPON_BINOCULARS_INIT || ps->weaponstate > WEAPON_BINOCULARS_END ) );
		drop = ( pressed && ( ps->weaponstate >= WEAPON_BINOCULARS_RAISE && ps->weaponstate <= WEAPON_BINOCULARS_EXIT_ADS ) );
	}
	else
	{
		pressed = ( ( pm->cmd.buttons & BUTTON_BINOCULARS ) != 0 );
		raise = ( pressed && ( ps->weaponstate < WEAPON_BINOCULARS_INIT || ps->weaponstate > WEAPON_BINOCULARS_END ) );
		drop = ( !pressed && ( ps->weaponstate >= WEAPON_BINOCULARS_RAISE && ps->weaponstate <= WEAPON_BINOCULARS_EXIT_ADS ) );
	}

	if ( raise )
	{
		// Pressed binoculars button - switch to binoculars
		if ( !BG_GetBinocularWeaponIndex() )
		{
			return; // No binoculars weapon file loaded
		}

		if ( !pm->cmd.weapon )
		{
			Binocular_State_Hold(ps);
		}
		else if ( pm->cmd.buttons & BUTTON_ADS )
		{
			Binocular_State_Init(ps);
		}
		else
		{
			Binocular_State_Raise(ps);
		}
	}
	else if ( drop )
	{
		// Pressed binoculars button while using binoculars - drop them
		if ( ps->weaponstate == WEAPON_BINOCULARS_RAISE )
		{
			Binocular_State_End(pm);
		}
		else
		{
			Binocular_State_Drop(ps);
		}
	}
}

/*
===============
PM_Weapon
===============
*/
void PM_Weapon( pmove_t *pm, pml_t *pml )
{
	static const int unused = 40;	// unreferenced
	qboolean delayedAction;
	playerState_t *ps;

	assert(pm);
	ps = pm->ps;
	assert(ps);

	if ( ps->pm_flags & PMF_RESPAWNED )
	{
		return;
	}

	if ( ps->pm_type >= PM_DEAD )
	{
		ps->weapon = WP_NONE;
		return;
	}

	if ( ps->eFlags & EF_TURRET_ACTIVE )
	{
		return;
	}

	PM_UpdateAimDownSightLerp(pm, pml);
	PM_UpdateHoldBreath(pm, pml);

	if ( PM_UpdateGrenadeThrow(ps, pml) )
	{
		return;
	}

	delayedAction = PM_Weapon_WeaponTimeAdjust(pm, pml);

	PM_Weapon_CheckForBinoculars(pm);
	PM_Weapon_CheckForOffHand(pm);
	PM_Weapon_CheckForChangeWeapon(pm);
	PM_Weapon_CheckForReload(pm);
	PM_Weapon_CheckForMelee(pm, delayedAction);

	if ( PM_Weapon_CheckForRechamber(ps, delayedAction) )
	{
		return;
	}

	if ( ps->pm_flags & PMF_PRONE && (pm->cmd.forwardmove || pm->cmd.rightmove)
	        || ps->weaponstate == WEAPON_MELEE_INIT
	        || ps->weaponstate == WEAPON_MELEE_FIRE )
	{
		ps->aimSpreadScale = 255;
	}

	assert((ps->weaponTime >= 0) && (ps->weaponDelay >= 0));

	if ( !delayedAction && (ps->weaponTime || ps->weaponDelay) )
	{
		return;
	}

	switch ( ps->weaponstate )
	{
	case WEAPON_RELOAD_START:
	case WEAPON_RELOAD_START_INTERUPT:
		PM_Weapon_FinishReloadStart(ps, delayedAction);
		break;

	case WEAPON_RELOADING:
	case WEAPON_RELOADING_INTERUPT:
		PM_Weapon_FinishReload(ps, delayedAction);
		break;

	case WEAPON_RELOAD_END:
		PM_Weapon_FinishReloadEnd(ps);
		break;

	case WEAPON_MELEE_INIT:
		PM_Weapon_FireMelee(ps);
		break;

	case WEAPON_MELEE_FIRE:
		PM_Weapon_FinishMelee(ps);
		break;

	case WEAPON_DROPPING:
		PM_Weapon_FinishWeaponChange(pm);
		break;

	case WEAPON_RAISING:
		PM_Weapon_FinishWeaponRaise(ps);
		break;

	case WEAPON_OFFHAND_INIT:
		PM_Weapon_OffHandPrepare(ps);
		break;

	case WEAPON_OFFHAND_PREPARE:
		PM_Weapon_OffHandHold(ps);
		break;

	case WEAPON_OFFHAND_HOLD:
		PM_Weapon_OffHand(pm);
		break;

	case WEAPON_OFFHAND:
		PM_Weapon_OffHandEnd(ps);
		break;

	case WEAPON_OFFHAND_END:
		PM_Weapon_Idle(ps);
		break;

	case WEAPON_BINOCULARS_INIT:
		Binocular_State_Raise(ps);
		break;

	case WEAPON_BINOCULARS_RAISE:
		Binocular_State_Hold(ps);
		break;

	case WEAPON_BINOCULARS_HOLD:
		Binocular_State_Exit_ADS(ps);
		break;

	case WEAPON_BINOCULARS_DROP:
		Binocular_State_End(pm);
		break;

	case WEAPON_BINOCULARS_END:
		PM_Weapon_Idle(ps);
		break;

	case WEAPON_BINOCULARS_EXIT_ADS:
		break;

	default:
		if ( !ps->weapon )
		{
			break;
		}

		if ( PM_Weapon_FinishFiring(pm, delayedAction) )
		{
			break;
		}

		PM_Weapon_FireWeapon(ps, delayedAction);
		break;
	}
}

/*
===============
PM_ResetWeaponState
===============
*/
void PM_ResetWeaponState( playerState_t *ps )
{
	assert(ps);
	PM_Weapon_Idle(ps);
}

/*
===============
BG_WeaponFireRecoil
===============
*/
void BG_WeaponFireRecoil( playerState_t *ps, vec3_t vGunSpeed, vec3_t kickAVel )
{
	WeaponDef *weapDef;
	float fPitchKick;
	float fYawKick;
	float fPosLerp;
	int weapIndex;
	float fReducePercent;

	assert(ps);

	weapIndex = BG_GetViewmodelWeaponIndex(ps);
	weapDef = BG_GetWeaponDef(weapIndex);

	fPosLerp = ps->fWeaponPosFrac;
	fReducePercent = 1.0f;

	if ( ps->weaponRestrictKickTime > 0 )
	{
		if ( fPosLerp == 1.0f )
		{
			fReducePercent = weapDef->adsGunKickReducedKickPercent * 0.0099999998f;
		}
		else
		{
			fReducePercent = weapDef->hipGunKickReducedKickPercent * 0.0099999998f;
		}
	}

	if ( fPosLerp == 1.0f )
	{
		fPitchKick = weapDef->adsViewKickPitchMin + (weapDef->adsViewKickPitchMax - weapDef->adsViewKickPitchMin) * randomf();
		fYawKick = weapDef->adsViewKickYawMin + (weapDef->adsViewKickYawMax - weapDef->adsViewKickYawMin) * randomf();
	}
	else
	{
		fPitchKick = weapDef->hipViewKickPitchMin + (weapDef->hipViewKickPitchMax - weapDef->hipViewKickPitchMin) * randomf();
		fYawKick = weapDef->hipViewKickYawMin + (weapDef->hipViewKickYawMax - weapDef->hipViewKickYawMin) * randomf();
	}

	fPitchKick *= fReducePercent;
	fYawKick *= fReducePercent;

	kickAVel[0] = -fPitchKick;
	kickAVel[1] = fYawKick;
	kickAVel[2] = kickAVel[1] * -0.5f;

	if ( fPosLerp > 0 )
	{
		fPitchKick = weapDef->adsGunKickPitchMin + (weapDef->adsGunKickPitchMax - weapDef->adsGunKickPitchMin) * randomf();
		fYawKick = weapDef->adsGunKickYawMin + (weapDef->adsGunKickYawMax - weapDef->adsGunKickYawMin) * randomf();
	}
	else
	{
		fPitchKick = weapDef->hipGunKickPitchMin + (weapDef->hipGunKickPitchMax - weapDef->hipGunKickPitchMin) * randomf();
		fYawKick = weapDef->hipGunKickYawMin + (weapDef->hipGunKickYawMax - weapDef->hipGunKickYawMin) * randomf();
	}

	fPitchKick *= fReducePercent;
	fYawKick *= fReducePercent;

	vGunSpeed[0] = vGunSpeed[0] + fPitchKick;
	vGunSpeed[1] = vGunSpeed[1] + fYawKick;
}

/*
===============
BG_GetBobCycle
===============
*/
float BG_GetBobCycle(const playerState_t *ps)
{
	return ((float)(ps->bobCycle & 255) / 255.0f * (M_PI * 2)) + M_PI * 2;
}

/*
===============
BG_GetVerticalBobFactor
===============
*/
float BG_GetVerticalBobFactor( const playerState_t *ps, float cycle, float speed, float maxAmp )
{
	float amplitude;

	assert(ps);

	if ( ps->viewHeightTarget == PRONE_VIEWHEIGHT )
	{
		amplitude = speed * bg_bobAmplitudeProne->current.decimal;
	}
	else
	{
		if ( ps->viewHeightTarget == CROUCH_VIEWHEIGHT )
			amplitude = speed * bg_bobAmplitudeDucked->current.decimal;
		else
			amplitude = speed * bg_bobAmplitudeStanding->current.decimal;
	}

	if ( amplitude > maxAmp )
	{
		amplitude = maxAmp;
	}

	return 0.75f * ((float)sin( cycle * 2 ) + (float)sin( cycle * 4.0f + 1.5707964f ) * 0.2f) * amplitude;
}

/*
===============
BG_GetHorizontalBobFactor
===============
*/
float BG_GetHorizontalBobFactor(const playerState_t *ps, float cycle, float speed, float maxAmp)
{
	float amplitude;
	float bob;

	if (ps->viewHeightTarget == PRONE_VIEWHEIGHT)
		amplitude = speed * bg_bobAmplitudeProne->current.decimal;
	else if (ps->viewHeightTarget == CROUCH_VIEWHEIGHT)
		amplitude = speed * bg_bobAmplitudeDucked->current.decimal;
	else
		amplitude = speed * bg_bobAmplitudeStanding->current.decimal;
	if (amplitude > maxAmp)
		amplitude = maxAmp;
	bob = sin(cycle);
	return bob * amplitude;
}

/*
===============
BG_CalculateWeaponPosition_BasePosition_angles
===============
*/
void BG_CalculateWeaponPosition_BasePosition_angles( weaponState_t *ws, vec3_t angles )
{
	int i;
	float fraction;
	float speedFrac;
	float minSpeed;
	vec3_t targetAngles;
	playerState_t *ps;
	WeaponDef *weapDef;
	int weapIndex;

	ps = ws->ps;

	weapIndex = BG_GetViewmodelWeaponIndex(ps);
	weapDef = BG_GetWeaponDef(weapIndex);

	if ( ps->eFlags & EF_PRONE )
	{
		minSpeed = weapDef->proneRotMinSpeed;
	}
	else if ( ps->eFlags & EF_CROUCH )
	{
		minSpeed = weapDef->duckedRotMinSpeed;
	}
	else
	{
		minSpeed = weapDef->standRotMinSpeed;
	}

	if ( ws->xyspeed > minSpeed && ps->weaponstate != WEAPON_RELOADING )
	{
		fraction = (ws->xyspeed - minSpeed) / (ps->speed - minSpeed);
		fraction = I_fmin(1.0f, I_fmax(0.f, fraction));

		if ( ps->eFlags & EF_PRONE )
		{
			VectorScale(&weapDef->proneRotP, fraction, targetAngles);
		}
		else if ( ps->eFlags & EF_CROUCH )
		{
			VectorScale(&weapDef->duckedRotP, fraction, targetAngles);
		}
		else
		{
			VectorScale(&weapDef->standRotP, fraction, targetAngles);
		}
	}
	else
	{
		VectorClear(targetAngles);
	}

	if ( ps->fWeaponPosFrac != 0 )
	{
		speedFrac = 1.0f - ps->fWeaponPosFrac;
		VectorScale(targetAngles, speedFrac, targetAngles);
	}

	for ( i = 0; i < 3; ++i )
	{
		if ( ws->vLastMoveAng[i] == targetAngles[i] )
		{
			continue;
		}

		if ( ps->viewHeightCurrent == PRONE_VIEWHEIGHT )
		{
			speedFrac = ws->frametime * (targetAngles[i] - ws->vLastMoveAng[i]) * weapDef->posProneRotRate;
		}
		else
		{
			speedFrac = ws->frametime * (targetAngles[i] - ws->vLastMoveAng[i]) * weapDef->posRotRate;
		}

		if ( ws->vLastMoveAng[i] < targetAngles[i] )
		{
			if ( speedFrac < ws->frametime * 0.1f )
			{
				speedFrac = ws->frametime * 0.1f;
			}

			ws->vLastMoveAng[i] = ws->vLastMoveAng[i] + speedFrac;

			if ( ws->vLastMoveAng[i] > targetAngles[i] )
			{
				ws->vLastMoveAng[i] = targetAngles[i];
			}
		}
		else
		{
			if ( speedFrac > ws->frametime * -0.1f )
			{
				speedFrac = ws->frametime * -0.1f;
			}

			ws->vLastMoveAng[i] = ws->vLastMoveAng[i] + speedFrac;

			if ( ws->vLastMoveAng[i] < targetAngles[i] )
			{
				ws->vLastMoveAng[i] = targetAngles[i];
			}
		}
	}

	if ( ps->fWeaponPosFrac != 0 )
	{
		if ( ps->fWeaponPosFrac < 0.5f )
		{
			speedFrac = 1.0f - ps->fWeaponPosFrac * 2.0f;
			VectorMA(angles, speedFrac, ws->vLastMoveAng, angles);
		}
	}
	else
	{
		VectorAdd(angles, ws->vLastMoveAng, angles);
	}
}

/*
===============
BG_CalculateWeaponPosition_BaseAngles
===============
*/
void BG_CalculateWeaponPosition_BaseAngles(weaponState_t *ws, vec3_t angles)
{
	playerState_t *ps;
	WeaponDef *weapDef;
	int weapIndex;

	ps = ws->ps;
	weapIndex = BG_GetViewmodelWeaponIndex(ps);
	weapDef = BG_GetWeaponDef(weapIndex);
	if (BG_IsAimDownSightWeapon(weapIndex))
		angles[0] += ps->fWeaponPosFrac * weapDef->adsAimPitch;
	BG_CalculateWeaponPosition_BasePosition_angles(ws, angles);
}

/*
===============
BG_CalculateWeaponPosition_IdleAngles
===============
*/
void BG_CalculateWeaponPosition_IdleAngles(weaponState_t *ws, vec3_t angles)
{
	WeaponDef *weapDef;
	float s;
	float fTargScale;
	float fTargFactor;
	float fIdleLerp;
	playerState_t *ps;
	int weapIndex;
	float idleSpeed;

	fIdleLerp = 0.5f;
	ps = ws->ps;
	weapIndex = BG_GetViewmodelWeaponIndex(ps);
	weapDef = BG_GetWeaponDef(weapIndex);
	if (BG_IsAimDownSightWeapon(weapIndex))
	{
		fTargScale = weapDef->hipIdleAmount + ps->fWeaponPosFrac * (weapDef->adsIdleAmount - weapDef->hipIdleAmount);
		idleSpeed = weapDef->hipIdleSpeed + ps->fWeaponPosFrac * (weapDef->adsIdleSpeed - weapDef->hipIdleSpeed);
	}
	else if (weapDef->hipIdleAmount != 0)
	{
		fTargScale = weapDef->hipIdleAmount;
		idleSpeed = weapDef->hipIdleSpeed;
	}
	else
	{
		fTargScale = 80;
		idleSpeed = 1;
	}
	if (ps->eFlags & EF_PRONE)
		fTargFactor = weapDef->idleProneFactor;
	else if (ps->eFlags & EF_CROUCH)
		fTargFactor = weapDef->idleCrouchFactor;
	else
		fTargFactor = 1;
	if ((!weapDef->adsOverlayReticle || ps->fWeaponPosFrac == 0) && fTargFactor != ws->fLastIdleFactor)
	{
		if (fTargFactor > ws->fLastIdleFactor)
		{
			ws->fLastIdleFactor += ws->frametime * 0.5f;
			if (ws->fLastIdleFactor > fTargFactor)
				ws->fLastIdleFactor = fTargFactor;
		}
		else
		{
			ws->fLastIdleFactor -= ws->frametime * 0.5f;
			if (ws->fLastIdleFactor < fTargFactor)
				ws->fLastIdleFactor = fTargFactor;
		}
	}
	fTargScale = fTargScale * ws->fLastIdleFactor;
	if (weapDef->adsOverlayReticle)
		fTargScale = (1 - ps->fWeaponPosFrac) * fTargScale;
	*ws->weapIdleTime += (int)(ws->frametime * 1000 * idleSpeed);
	s = sin(*ws->weapIdleTime * 0.00050000002f);
	angles[2] += fTargScale * s * 0.0099999998f;
	s = sin(*ws->weapIdleTime * 0.00069999998f);
	angles[1] += fTargScale * s * 0.0099999998f;
	s = sin(*ws->weapIdleTime * 0.001f);
	angles[0] += fTargScale * s * 0.0099999998f;
}

/*
===============
BG_CalculateWeaponPosition_BobOffset
===============
*/
void BG_CalculateWeaponPosition_BobOffset(weaponState_t *ws, vec3_t angles)
{
	float fPositionLerp;
	float cycle;
	float speed;
	vec3_t vAngOfs;
	float bobCycle;
	playerState_t *ps;
	WeaponDef *weapDef;
	int weapIndex;

	ps = ws->ps;
	weapIndex = BG_GetViewmodelWeaponIndex(ps);
	weapDef = BG_GetWeaponDef(weapIndex);
	bobCycle = (float)(ps->bobCycle & 255) / 255.0f * M_PI * 2 + M_PI * 2;
	cycle = bobCycle + 0.78539819f + M_PI * 2;
	speed = ws->xyspeed * 0.16f;
	vAngOfs[0] = BG_GetVerticalBobFactor(ps, cycle, speed, 10) * -1.0f;
	vAngOfs[1] = BG_GetHorizontalBobFactor(ps, cycle, speed, 10) * -1.0f;
	cycle -= 0.47123894f;
	speed *= 1.5f;
	vAngOfs[2] = I_fmin(BG_GetHorizontalBobFactor(ps, cycle, speed, 10), 0);
	fPositionLerp = ps->fWeaponPosFrac;
	if (fPositionLerp != 0)
	{
		speed = 1 - (1 - weapDef->adsBobFactor) * fPositionLerp;
		vAngOfs[0] *= speed;
		vAngOfs[1] *= speed;
		vAngOfs[2] *= speed;
	}
	if (weapDef->adsOverlayReticle)
		VectorScale(vAngOfs, 1 - fPositionLerp, vAngOfs);
	VectorAdd(angles, vAngOfs, angles);
}

/*
===============
BG_CalculateWeaponPosition_DamageKick
===============
*/
void BG_CalculateWeaponPosition_DamageKick(weaponState_t *ws, vec3_t angles)
{
	float fRatio;
	float fFactor;
	float fDeflectTime;
	float fReturnTime;
	playerState_t *ps;
	WeaponDef *weapDef;
	int weapIndex;

	if (!ws->damageTime)
		return;
	ps = ws->ps;
	weapIndex = BG_GetViewmodelWeaponIndex(ps);
	weapDef = BG_GetWeaponDef(weapIndex);
	fFactor = ps->fWeaponPosFrac * 0.5f + 0.5f;
	fDeflectTime = fFactor * 100;
	fReturnTime = fFactor * 400;
	if (ps->fWeaponPosFrac != 0 && weapDef->adsOverlayReticle)
		fFactor = (1 - ps->fWeaponPosFrac * 0.75f) * fFactor;
	fRatio = ws->time - ws->damageTime;
	if (fRatio < fDeflectTime)
	{
		fRatio = fRatio / fDeflectTime;
		fRatio = GetLeanFraction(fRatio);
		fRatio = fRatio * fFactor;
		angles[0] += fRatio * ws->v_dmg_pitch * 0.5f;
		angles[1] -= fRatio * ws->v_dmg_roll;
		angles[2] += fRatio * ws->v_dmg_roll * 0.5f;
	}
	else
	{
		fRatio = 1 - (fRatio - fDeflectTime) / fReturnTime;
		if (fRatio > 0)
		{
			fRatio = 1 - GetLeanFraction(1 - fRatio);
			fRatio = fRatio * fFactor;
			angles[0] += fRatio * ws->v_dmg_pitch * 0.5f;
			angles[1] -= fRatio * ws->v_dmg_roll;
			angles[2] += fRatio * ws->v_dmg_roll * 0.5f;
		}
	}
}

/*
===============
BG_CalculateWeaponPosition_GunRecoil
===============
*/
void BG_CalculateWeaponPosition_GunRecoil( weaponState_t *ws, vec3_t angles )
{
	float fTotalTime;
	float fTimeStep;
	qboolean bCanStop;
	float fGunKickAccel;
	float fGunKickSpeedMax;
	float fGunKickSpeedDecay;
	float fGunKickStaticDecay;
	float fPosLerp;
	playerState_t *ps;
	WeaponDef *weapDef;
	int weapIndex;

	ps = ws->ps;

	weapIndex = BG_GetViewmodelWeaponIndex(ps);
	weapDef = BG_GetWeaponDef(weapIndex);

	if ( !BG_IsAimDownSightWeapon(weapIndex) )
	{
		return;
	}

	fPosLerp = ps->fWeaponPosFrac;

	fGunKickAccel = weapDef->hipGunKickAccel + (weapDef->adsGunKickAccel - weapDef->hipGunKickAccel) * fPosLerp;
	fGunKickSpeedMax = weapDef->hipGunKickSpeedMax + (weapDef->adsGunKickSpeedMax - weapDef->hipGunKickSpeedMax) * fPosLerp;
	fGunKickSpeedDecay = weapDef->hipGunKickSpeedDecay + (weapDef->adsGunKickSpeedDecay - weapDef->hipGunKickSpeedDecay) * fPosLerp;
	fGunKickStaticDecay = weapDef->hipGunKickStaticDecay + (weapDef->adsGunKickStaticDecay - weapDef->hipGunKickStaticDecay) * fPosLerp;

	bCanStop = qfalse;
	fTotalTime = ws->frametime;

	while ( fTotalTime > 0 )
	{
		if ( fTotalTime > 0.005 )
		{
			fTimeStep = 0.005f;
			fTotalTime -= 0.005;
		}
		else
		{
			fTimeStep = fTotalTime;
			fTotalTime = 0;
		}

		bCanStop = BG_CalculateWeaponPosition_GunRecoil_SingleAngle(ws->vGunOffset, ws->vGunSpeed, fTimeStep,
		            weapDef->gunMaxPitch, fGunKickAccel, fGunKickSpeedMax, fGunKickSpeedDecay, fGunKickStaticDecay);

		if ( BG_CalculateWeaponPosition_GunRecoil_SingleAngle(&ws->vGunOffset[YAW], &ws->vGunSpeed[YAW], fTimeStep,
		            weapDef->gunMaxYaw, fGunKickAccel, fGunKickSpeedMax, fGunKickSpeedDecay, fGunKickStaticDecay) && bCanStop )
		{
			break;
		}
	}

	VectorAdd(ws->vGunOffset, angles, angles);
}

/*
===============
BG_CalculateWeaponAngles
===============
*/
void BG_CalculateWeaponAngles( weaponState_t *ws, vec3_t angles )
{
	float leanFrac;
	playerState_t *ps;

	ps = ws->ps;
	assert(ps);

	VectorClear(angles);

	if ( ps->leanf != 0 )
	{
		leanFrac = GetLeanFraction(ps->leanf);
		angles[2] = angles[2] - 2 * leanFrac;
	}

	BG_CalculateWeaponPosition_BaseAngles(ws, angles);
	BG_CalculateWeaponPosition_IdleAngles(ws, angles);
	BG_CalculateWeaponPosition_BobOffset(ws, angles);
	BG_CalculateWeaponPosition_DamageKick(ws, angles);
	BG_CalculateWeaponPosition_GunRecoil(ws, angles);

	angles[0] = AngleSubtract(angles[0], ws->swayAngles[0]);
	angles[1] = AngleSubtract(angles[1], ws->swayAngles[1]);
}

/*
===============
BG_CalculateView_DamageKick
===============
*/
void BG_CalculateView_DamageKick(viewState_t *vs, vec3_t angles)
{
	float fRatio;
	float fFactor;
	playerState_t *ps;
	WeaponDef *weapDef;
	int weapIndex;

	if (!vs->damageTime)
		return;
	ps = vs->ps;
	weapIndex = BG_GetViewmodelWeaponIndex(ps);
	weapDef = BG_GetWeaponDef(weapIndex);
	fFactor = 1 - ps->fWeaponPosFrac * 0.5f;
	if (ps->fWeaponPosFrac != 0 && weapDef->adsOverlayReticle)
		fFactor = (ps->fWeaponPosFrac * 0.5f + 1) * fFactor;
	fRatio = vs->time - vs->damageTime;
	if (fRatio < 100)
	{
		fRatio = fRatio / 100;
		fRatio = GetLeanFraction(fRatio);
		fRatio = fRatio * fFactor;
		angles[0] += fRatio * vs->v_dmg_pitch;
		angles[2] += fRatio * vs->v_dmg_roll;
	}
	else
	{
		fRatio = 1 - (fRatio - 100) / 400;
		if (fRatio > 0)
		{
			fRatio = 1 - GetLeanFraction(1 - fRatio);
			fRatio = fRatio * fFactor;
			angles[0] += fRatio * vs->v_dmg_pitch;
			angles[2] += fRatio * vs->v_dmg_roll;
		}
	}
}

/*
===============
BG_CalculateView_IdleAngles
===============
*/
void BG_CalculateView_IdleAngles(viewState_t *vs, vec3_t angles)
{
	WeaponDef *weapDef;
	float s;
	float fTargScale;
	float fTargFactor;
	float fIdleLerp;
	playerState_t *ps;
	int weapIndex;
	float idleSpeed;

	fIdleLerp = 0.5f;
	ps = vs->ps;
	weapIndex = BG_GetViewmodelWeaponIndex(ps);
	weapDef = BG_GetWeaponDef(weapIndex);
	if (!weapDef->adsOverlayReticle)
		return;
	if (BG_IsAimDownSightWeapon(weapIndex))
	{
		fTargScale = weapDef->hipIdleAmount + ps->fWeaponPosFrac * (weapDef->adsIdleAmount - weapDef->hipIdleAmount);
		idleSpeed = weapDef->hipIdleSpeed + ps->fWeaponPosFrac * (weapDef->adsIdleSpeed - weapDef->hipIdleSpeed);
	}
	else if (weapDef->hipIdleAmount != 0)
	{
		fTargScale = weapDef->hipIdleAmount;
		idleSpeed = weapDef->hipIdleSpeed;
	}
	else
	{
		fTargScale = 80;
		idleSpeed = 1;
	}
	if (ps->eFlags & EF_PRONE)
		fTargFactor = weapDef->idleProneFactor;
	else if (ps->eFlags & EF_CROUCH)
		fTargFactor = weapDef->idleCrouchFactor;
	else
		fTargFactor = 1;
	if (weapDef->adsOverlayReticle && ps->fWeaponPosFrac != 0 && fTargFactor != vs->fLastIdleFactor)
	{
		if (fTargFactor > vs->fLastIdleFactor)
		{
			vs->fLastIdleFactor += vs->frametime * 0.5f;
			if (vs->fLastIdleFactor > fTargFactor)
				vs->fLastIdleFactor = fTargFactor;
		}
		else
		{
			vs->fLastIdleFactor -= vs->frametime * 0.5f;
			if (vs->fLastIdleFactor < fTargFactor)
				vs->fLastIdleFactor = fTargFactor;
		}
	}
	fTargScale = fTargScale * vs->fLastIdleFactor;
	fTargScale = fTargScale * ps->fWeaponPosFrac;
	fTargScale = fTargScale * ps->holdBreathScale;
	*vs->weapIdleTime += (int)(ps->holdBreathScale * vs->frametime * 1000 * idleSpeed);
	s = sin(*vs->weapIdleTime * 0.00069999998f);
	angles[1] += fTargScale * s * 0.0099999998f;
	s = sin(*vs->weapIdleTime * 0.001f);
	angles[0] += fTargScale * s * 0.0099999998f;
}

/*
===============
BG_CalculateView_BobAngles
===============
*/
void BG_CalculateView_BobAngles(viewState_t *vs, vec3_t angles)
{
	float fPositionLerp;
	float cycle;
	float speed;
	vec3_t vAngOfs;
	float bobCycle;
	playerState_t *ps;
	WeaponDef *weapDef;
	int weapIndex;

	ps = vs->ps;
	weapIndex = BG_GetViewmodelWeaponIndex(ps);
	weapDef = BG_GetWeaponDef(weapIndex);
	if (!weapDef->adsOverlayReticle)
		return;
	bobCycle = (float)(ps->bobCycle & 255) / 255.0f * M_PI * 2 + M_PI * 2;
	cycle = bobCycle + 0.78539819f + M_PI * 2;
	speed = vs->xyspeed * 0.16f;
	vAngOfs[0] = BG_GetVerticalBobFactor(ps, cycle, speed, 10) * -1.0f;
	vAngOfs[1] = BG_GetHorizontalBobFactor(ps, cycle, speed, 10) * -1.0f;
	cycle -= 0.47123894f;
	speed *= 1.5f;
	vAngOfs[2] = I_fmin(BG_GetHorizontalBobFactor(ps, cycle, speed, 10), 0);
	fPositionLerp = ps->fWeaponPosFrac;
	if (fPositionLerp != 0)
	{
		speed = 1 - (1 - weapDef->adsBobFactor) * fPositionLerp;
		vAngOfs[0] *= speed;
		vAngOfs[1] *= speed;
		vAngOfs[2] *= speed;
	}
	VectorScale(vAngOfs, fPositionLerp, vAngOfs);
	VectorAdd(angles, vAngOfs, angles);
}

/*
===============
BG_CalculateView_Velocity
===============
*/
void BG_CalculateView_Velocity(viewState_t *vs, vec3_t angles)
{
	float fBob;
	float fSway;
	float bobCycle;
	playerState_t *ps;
	WeaponDef *weapDef;
	int weapIndex;

	ps = vs->ps;
	weapIndex = BG_GetViewmodelWeaponIndex(ps);
	weapDef = BG_GetWeaponDef(weapIndex);
	if (!(ps->eFlags & 0x300) && ps->fWeaponPosFrac != 0 && weapDef->adsViewBobMult != 0)
	{
		bobCycle = (float)(ps->bobCycle & 255) / 255.0f * M_PI * 2 + M_PI * 2;
		fBob = BG_GetVerticalBobFactor(ps, bobCycle, vs->xyspeed, 45);
		fBob = ps->fWeaponPosFrac * weapDef->adsViewBobMult * fBob;
		angles[0] -= fBob;
		fSway = BG_GetHorizontalBobFactor(ps, bobCycle, vs->xyspeed, 45);
		fSway = ps->fWeaponPosFrac * weapDef->adsViewBobMult * fSway;
		angles[1] -= fSway;
	}
}

/*
===============
BG_CalculateViewAngles
===============
*/
void BG_CalculateViewAngles(viewState_t *vs, vec3_t angles)
{
	VectorClear(angles);

	BG_CalculateView_DamageKick(vs, angles);
	BG_CalculateView_IdleAngles(vs, angles);
	BG_CalculateView_BobAngles(vs, angles);
	BG_CalculateView_Velocity(vs, angles);
}

/*
===============
BG_CalculateWeaponPosition_Sway
===============
*/
// retail Sway is /Op (store-reload on int->float)
#pragma optimize("p", on)
void BG_CalculateWeaponPosition_Sway( const playerState_t *ps, vec3_t swayViewAngles, vec3_t swayOffset, vec3_t swayAngles, float ssSwayScale, int frametime )
{
	WeaponDef *weapDef;
	vec3_t deltaAngles;
	vec3_t deltaOffset;
	float swayMaxAngle;
	float swayLerpSpeed;
	float swayPitchScale;
	float swayYawScale;
	float swayHorizScale;
	float swayVertScale;
	float fWeaponPosFrac;
	float dt;
	float scale;
	int weapon;

	assert(ps);
	fWeaponPosFrac = ps->fWeaponPosFrac;

	if ( !frametime )
	{
		return;
	}

	weapon = BG_GetViewmodelWeaponIndex(ps);
	weapDef = BG_GetWeaponDef(weapon);

	dt = frametime * 0.001f;

	if ( BG_IsAimDownSightWeapon(weapon) )
	{
		if ( fWeaponPosFrac > 0 && weapDef->adsOverlayReticle )
		{
			return;
		}

		swayMaxAngle = (weapDef->adsSwayMaxAngle - weapDef->swayMaxAngle) * fWeaponPosFrac + weapDef->swayMaxAngle;
		swayLerpSpeed = (weapDef->adsSwayLerpSpeed - weapDef->swayLerpSpeed) * fWeaponPosFrac + weapDef->swayLerpSpeed;
		swayPitchScale = (weapDef->adsSwayPitchScale - weapDef->swayPitchScale) * fWeaponPosFrac + weapDef->swayPitchScale;
		swayYawScale = (weapDef->adsSwayYawScale - weapDef->swayYawScale) * fWeaponPosFrac + weapDef->swayYawScale;
		swayHorizScale = (weapDef->adsSwayHorizScale - weapDef->swayHorizScale) * fWeaponPosFrac + weapDef->swayHorizScale;
		swayVertScale = (weapDef->adsSwayVertScale - weapDef->swayVertScale) * fWeaponPosFrac + weapDef->swayVertScale;
	}
	else
	{
		swayMaxAngle = weapDef->swayMaxAngle;
		swayLerpSpeed = weapDef->swayLerpSpeed;
		swayPitchScale = weapDef->swayPitchScale;
		swayYawScale = weapDef->swayYawScale;
		swayHorizScale = weapDef->swayHorizScale;
		swayVertScale = weapDef->swayVertScale;
	}

	swayPitchScale *= ssSwayScale;
	swayYawScale *= ssSwayScale;
	swayHorizScale *= ssSwayScale;
	swayVertScale *= ssSwayScale;

	deltaAngles[0] = AngleSubtract(ps->viewangles[0], swayViewAngles[0]);
	deltaAngles[1] = AngleSubtract(ps->viewangles[1], swayViewAngles[1]);

	assert(dt);
	scale = 1.0f / (dt * 60.0f);
	deltaAngles[0] *= scale;
	deltaAngles[1] *= scale;

	deltaAngles[0] = I_fclamp(deltaAngles[0], -swayMaxAngle, swayMaxAngle);
	deltaAngles[1] = I_fclamp(deltaAngles[1], -swayMaxAngle, swayMaxAngle);

	deltaOffset[1] = deltaAngles[1] * swayHorizScale;
	deltaOffset[2] = deltaAngles[0] * swayVertScale;

	swayOffset[1] = DiffTrack(deltaOffset[1], swayOffset[1], swayLerpSpeed, dt);
	swayOffset[2] = DiffTrack(deltaOffset[2], swayOffset[2], swayLerpSpeed, dt);

	deltaAngles[0] = deltaAngles[0] * swayPitchScale;
	deltaAngles[1] = deltaAngles[1] * swayYawScale;

	swayAngles[0] = DiffTrackAngle(deltaAngles[0], swayAngles[0], swayLerpSpeed, dt);
	swayAngles[1] = DiffTrackAngle(deltaAngles[1], swayAngles[1], swayLerpSpeed, dt);

	VectorCopy(ps->viewangles, swayViewAngles);
}
#pragma optimize("p", off)

/*
===============
BG_LoadWeaponDef
===============
*/
WeaponDef *BG_LoadWeaponDef( const char *folder, const char *name )
{
	WeaponDef *weapDef;

	if ( !name[0] )
	{
		return NULL;
	}

	weapDef = BG_LoadWeaponDefInternal(folder, name);

	if ( weapDef )
	{
		return weapDef;
	}

	weapDef = BG_LoadWeaponDefInternal(folder, "defaultweapon_mp");

	if ( !weapDef )
	{
		Com_Error(ERR_DROP, "\x15" "BG_LoadWeaponDef: Could not find default weapon");
	}

	SetConfigString((char **)&weapDef->szInternalName, name);
	return weapDef;
}

