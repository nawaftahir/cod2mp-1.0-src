#include "common_types.h"
#include "imports.h"
#include "headers/PC/cgame_mp/cg_local.h"
#include <float.h>
COD2_ASSERT_SIZE(cgs_t, 0xe7d4);
const dvar_t *cg_shock_soundWetLevel;

const dvar_t *cg_tracerLength;

const dvar_t *cg_marks;
const dvar_t *cg_hudGrenadePointerPulseMax;
const dvar_t *cg_shock_mouse_maxyawspeed;
const dvar_t *cg_shock_volume_weapon;
const dvar_t *cg_shock_volume_voice;
const dvar_t *cg_hudCompassMaxRange;
const dvar_t *cg_gun_y;
const dvar_t *cg_noTaunt;
const dvar_t *cg_scoreboardScrollStep;
const dvar_t *cg_hudSayPosition;
displayContextDef_t cgDC;
const dvar_t *cg_shock_volume_auto2d;
const dvar_t *cg_fovMin;
const dvar_t *cg_drawBreathHint;
const dvar_t *cg_shock_volume_shellshock;
const dvar_t *cg_hudCompassSoundPingFadeTime;
const dvar_t *cg_shock_volume_announcer;
const dvar_t *cg_drawFPS;
const dvar_t *cg_subtitlePosX;
const dvar_t *cg_subtitlePosY;
const dvar_t *cg_drawGameMessages;
const dvar_t *cg_thirdPerson;
const dvar_t *cg_drawHealth;
const dvar_t *cg_brass;
const dvar_t *cg_hudGrenadeIconMaxRange;
const dvar_t *cg_shock_volume_auto;
const dvar_t *cg_shock_soundFadeInTime;
const dvar_t *cg_drawCrosshair;
const dvar_t *cg_hudObjectiveMinHeight;
weaponInfo_t cg_weaponsArray[128];
const dvar_t *cg_hudObjectiveMinAlpha;
const dvar_t *cg_scoreboardItemHeight;
const dvar_t *cg_hudGrenadeIconWidth;
const dvar_t *cg_gun_z;
const dvar_t *cg_hudDamageIconOffset;
const dvar_t *cg_voiceIconSize;
const dvar_t *cg_hudCompassSpringyPointers;
const dvar_t *cg_gun_move_minspeed;
const dvar_t *cg_gun_ofs_u;
const dvar_t *cg_subtitles;
const dvar_t *cg_hudObjectiveMaxRange;
const dvar_t *cg_hudStanceHintPrints;
const dvar_t *cg_youInKillCamSize;
const dvar_t *cg_shock_sound;
const dvar_t *cg_hudDamageIconWidth;
const dvar_t *cg_hudProneY;
const dvar_t *cg_draw2D;
const dvar_t *cg_gun_x;
const dvar_t *cg_crosshairAlpha;
const dvar_t *cg_hudCompassMinRange;
const dvar_t *cg_debugPosition;
const dvar_t *cg_drawGun;
const dvar_t *cg_shock_soundModEndDelay;
const dvar_t *cg_hudGrenadeIconOffset;
const dvar_t *cg_centertime;
const dvar_t *cg_chatHeight;
const dvar_t *cg_hudGrenadePointerPulseFreq;
const dvar_t *cg_minicon;
const dvar_t *cg_drawCrosshairNamesPosX;
const dvar_t *cg_gun_move_rate;
const dvar_t *cg_tracerSpeed;
cgs_t cgsArray[1];
const dvar_t *cg_shock_screenBlendFadeTime;
const dvar_t *cg_shock_viewKickPeriod;
cg_t cgArray[1];
const dvar_t *cg_shock_volume_local;
const dvar_t *cg_hudGrenadePointerPivot;
const dvar_t *cg_crosshairAlphaMin;
const dvar_t *cg_shock_volume_body;
centity_t cg_entitiesArray[1][1024];
const dvar_t *cg_tracerChance;
const dvar_t *cg_hudGrenadeIconInScope;
const dvar_t *cg_fovScale;
const dvar_t *cg_crosshairEnemyColor;
const dvar_t *cg_hudCompassMinRadius;
const dvar_t *cg_tracerScale;
const dvar_t *cg_weaponCycleDelay;
const dvar_t *cg_gameMessageWidth;
const dvar_t *cg_thirdPersonRange;
const dvar_t *cg_drawSoundOverlay;
const dvar_t *cg_shock_soundLoopEndDelay;
const dvar_t *cg_gun_ofs_r;
const dvar_t *cg_hudGrenadeIconMaxHeight;
const dvar_t *cg_shock_screenBlendTime;
const dvar_t *cg_debugEvents;
const dvar_t *cg_shock_soundDryLevel;
const dvar_t *cg_hudStanceFlash;
const dvar_t *cg_drawScriptUsage;
const dvar_t *cg_shock_soundLoopFadeTime;
const dvar_t *cg_shock_volume_music;
const dvar_t *cg_crosshairDynamic;
const dvar_t *cg_hudDamageIconInScope;
const dvar_t *cg_drawCrosshairNames;
const dvar_t *cg_gun_move_r;
const dvar_t *cg_constantSizeHeadIcons;
const dvar_t *cg_footsteps;
const dvar_t *cg_shock_volume_menu;
const dvar_t *cg_gun_move_f;
const dvar_t *cg_hudGrenadeIconHeight;
const dvar_t *cg_shock_mouse_maxpitchspeed;
const dvar_t *cg_hudGrenadePointerWidth;
const dvar_t *cg_nopredict;
const dvar_t *cg_drawMaterial;
const dvar_t *cg_shock_viewKickRadius;
const dvar_t *cg_synchronousClients;
const dvar_t *cg_connectionIconSize;
const dvar_t *cg_dumpAnims;
const dvar_t *cg_errorDecay;
const dvar_t *cg_subtitleWidthStandard;
const dvar_t *cg_hudGrenadePointerPulseMin;
const dvar_t *cg_tracerScaleMinDist;
const dvar_t *cg_drawMantleHint;
const dvar_t *cg_gameBoldMessageWidth;
const dvar_t *cg_tracerWidth;
const dvar_t *cg_centerPrintY;
const dvar_t *cg_hudCompassSize;
const dvar_t *cg_drawCrosshairNamesPosY;
const dvar_t *cg_headIconMinScreenRadius;
const dvar_t *cg_chatTime;
const dvar_t *cg_shock_volume_item;
const dvar_t *cg_marksLimit;
const dvar_t *cg_hudDamageIconTime;
const dvar_t *cg_blood;
const dvar_t *cg_teamChatsOnly;
dvar_t *cg_viewsize;
const dvar_t *cg_weaponleftbone;
const dvar_t *cg_scriptIconSize;
const dvar_t *cg_showmiss;
const dvar_t *cg_predictItems;
const dvar_t *cg_shock_soundRoomType;
const dvar_t *cg_shock_mouse;
const dvar_t *cg_descriptiveText;
const dvar_t *cg_fov;
const dvar_t *cg_drawLagometer;
const dvar_t *cg_subtitleWidthWidescreen;
const dvar_t *cg_developer;
const dvar_t *cg_subtitleCharHeight;
itemInfo_t cg_itemsArray[256];
const dvar_t *cg_hudGrenadePointerHeight;
const dvar_t *cg_gun_move_u;
const dvar_t *cg_hintFadeTime;
const dvar_t *cg_shock_soundFadeOutTime;
const dvar_t *cg_shock_mouse_sensitivityscale;
const dvar_t *cg_subtitleMinTime;
const dvar_t *cg_tracerScaleDistRange;
const dvar_t *cg_paused;
const dvar_t *cg_cursorHints;
const dvar_t *cg_thirdPersonAngle;
const dvar_t *cg_gun_ofs_f;
const dvar_t *cg_scoreboardBannerHeight;
const dvar_t *hud_health_pulserate_injured;
const dvar_t *hud_health_startpulse_critical;
const dvar_t *hud_fade_offhand;
const dvar_t *hud_deathQuoteFadeTime;
const dvar_t *hud_fade_ammodisplay;
const dvar_t *hud_health_startpulse_injured;
const dvar_t *hud_fade_stance;
const dvar_t *hud_fade_compass;
const dvar_t *hud_health_pulserate_critical;
const dvar_t *hud_fade_healthbar;
extern int bg_iNumWeapons;

extern dvar_t *com_sv_running;

extern struct XModel *CL_RegisterModel(const char *name);
extern snd_alias_t *CL_PickSoundAlias(const char *aliasname);
extern const char *CL_GetConfigString(int index);
extern void CL_SubtitlePrint(const char *pszText, int iDuration, int iLineWidth);
extern int atoi(const char *nptr);
extern int sprintf(char *str, const char *format, ...);
extern const char *Info_ValueForKey( const char *s, const char *key );
extern const char *SEH_LocalizeTextMessage(const char *msg, const char *context, msgLocErrType_t errType);
extern void SND_PlayAmbientAlias(const snd_alias_t *pAlias, int fadetime, snd_alias_system_t system);
extern int SND_PlaySoundAliasAsMaster(const snd_alias_t *pAlias, int entnum, const vec_t *org, int timeshift, snd_alias_system_t system);
extern int SND_PlaySoundAlias(const snd_alias_t *pAlias, int entnum, const vec_t *org, int timeshift, snd_alias_system_t system);
extern snd_alias_t *Com_PickSoundAliasFromList( snd_alias_list_t *aliasList );
extern snd_alias_list_t *Com_FindSoundAlias( const char *name );
extern const char *Com_SurfaceTypeToName(int surfaceType);
extern MenuList *UI_LoadMenus(const char *menuFile, int imageTrack);
extern void UI_AddMenuList(displayContextDef_t *dc, MenuList *menuList);
extern int CL_GetLocalClientActiveCount(void);
extern menuDef_t *Menus_FindByName(displayContextDef_t *dc, const char *p);
extern void Com_ClientDObjCreate(DObjModel_s *dobjModels, unsigned short numModels, struct XAnimTree_s *tree, int handle);
extern int CG_WeaponDObjHandle(int weaponNum);
extern void Com_SafeClientDObjFree(int handle);
extern void XAnimFreeTree(struct XAnimTree_s *tree, Free_t Free);
extern int BG_GetNumWeapons(void);
extern void AxisCopy( const vec3_t in[3], vec3_t out[3] );
extern void I_strncpyz(char *dest, const char *src, int destsize);
extern void *memcpy(void *dest, const void *src, unsigned int n);
extern void *memset(void *s, int c, unsigned int n);
#if defined(__x86_64__) || defined(_M_X64)
#    include <string.h>
#endif
extern void CL_TrackStatistics(trStatistics_t *pStats);
extern void SND_FadeAllSounds(float volume, int fadetime);
extern void Mantle_ShutdownAnims(void);
extern void CG_FreeClientDObjInfo(void);
extern void CG_FreeEntityDObjInfo(void);
extern void CL_FreeWeaponInfoMemory(void);
extern void FX_FreeSystem(void);
extern void Scr_ShutdownGameStrings(void);
extern void *Hunk_AllocAlignInternal(int size, int alignment);
extern void *Hunk_AllocInternal(int size);
extern void CL_ConsolePrint(print_msg_type_t type, const char *txt, int duration, int linewidth);
extern void SCR_UpdateScreen(void);
extern void Com_Printf(const char *fmt, ...);
extern int FX_InitSystem( bool rendererExists );
extern void FX_CreateDefaultEffect(void);
extern float FX_GetEffectLength(const EffectTemplate *fx);
extern void FX_Rewind(int time);
extern void FX_WarpTime(int time);
extern void FX_PlayEffect(const EffectTemplate *fx, const vec_t *org, const vec_t *fwd);
extern void CG_LoadingString(const char *str);
extern MaterialHandle CL_RegisterMaterial(const char *name, int flags);
extern MaterialHandle CL_RegisterMaterialNoMip(const char *name, int imageTrack);
extern void CG_RegisterScoreboardGraphics(void);
extern void CG_RegisterItems(void);
extern int CM_NumInlineModels(void);
extern GfxBrushModel *CL_RegisterInlineModel(int index);
extern void CL_ModelBounds(GfxBrushModel *model, vec_t *mins, vec_t *maxs);
extern EffectTemplate * FX_RegisterEffect(const char *fileName);
extern void CG_SetShellShockParmsFromDvars(shellshock_parms_t *parms);
extern int CG_LoadShellShockDvars(const char *name);
extern void Com_Error(errorParm_t code, const char *fmt, ...);
extern FxImpactTable *CG_RegisterImpactEffects(const char *mapname);
extern void Controls_GetConfig(void);
extern int GetKeyBindingLocalizedString(const char *binding, char *buf);
extern const char *UI_SafeTranslateString(const char *key);
extern void AnglesToAxis( const vec3_t angles, vec3_t axis[3] );
extern void Cmd_ArgvBuffer(int arg, char *buffer, int bufferLength);
dvar_t *Dvar_RegisterBool(const char *dvarName, bool value, unsigned short flags);
dvar_t *Dvar_RegisterString(const char *dvarName, const char *value, unsigned short flags);
extern dvar_t *Dvar_RegisterInt(const char *dvarName, int value, int min, int max, unsigned short flags);
extern dvar_t *Dvar_RegisterFloat(const char *dvarName, float value, float min, float max, unsigned short flags);
extern dvar_t *Dvar_RegisterEnum(const char *dvarName, const char **valueList, int defaultIndex, unsigned short flags);
extern dvar_t *Dvar_RegisterColor(const char *dvarName, float r, float g, float b, float a, unsigned short flags);
extern dvar_t *Dvar_RegisterVec2(const char *dvarName, float x, float y, float min, float max, unsigned short flags);
extern void CL_SetADS(int ads);
extern void CG_OffhandRegisterDvars(void);
extern void BG_RegisterDvars(void);
extern void CG_ParseServerinfo(void);
extern void CG_ParseCodinfo(void);
extern void UI_LoadIngameMenus(void);
extern FontHandle CL_RegisterFont(const char *fontName, int imageTrack);
extern void CG_AntiBurnInHUD_RegisterDvars(void);
extern void CG_InitConsoleCommands(void);
extern void CL_GetScreenDimensions(int *width, int *height, float *aspect);
extern void Mantle_CreateAnims(MantleAnimAlloc xanimAlloc);
extern void CL_SetWeaponInfoMemory(void);
extern void BG_ClearWeaponDef(void);
extern void BG_FillInAmmoItems(int (*regWeap)(int));
extern void CG_SetupWeaponDef(void);
extern void CGScr_LoadAnimTrees(void);
extern void BG_LoadAnim();
extern XAnimTree *XAnimCreateTree(XAnim *anims, Alloc_t Alloc);
extern void GScr_LoadConsts(void);
extern void CL_CM_LoadMap(const char *mapname);
extern void Menu_Setup(displayContextDef_t *dc);
extern void LoadWorld(const char *mapname);
extern void CL_LoadSoundAliases(const char *loadspec);
extern void Com_StripExtension(const char *in, char *out);
extern void CG_InitLocalEntities(void);
extern void CG_InitMarkPolys(void);
extern void CG_SetConfigValues(void);
extern void CG_NorthDirectionChanged(void);
extern void CL_FinishLoadingModels(void);
extern void CG_ParseFog(void);
extern void SND_StopSounds(snd_stopsounds_arg_t which);
extern unsigned char scrMemTreeGlob[];


#define CG_MAX_ITEMS 256
#define CG_MAX_WEAPONS 128

extern const dvar_t *cg_centertime;
extern const dvar_t *cg_drawFPS;
extern const dvar_t *cg_drawSoundOverlay;
extern const dvar_t *cg_drawScriptUsage;
extern const dvar_t *cg_drawMaterial;
extern const dvar_t *cg_drawSnapshot;
extern const dvar_t *cg_drawCrosshair;
extern const dvar_t *cg_hudCompassSize;
extern const dvar_t *cg_hudCompassMaxRange;
extern const dvar_t *cg_hudCompassMinRange;
extern const dvar_t *cg_hudCompassMinRadius;
extern const dvar_t *cg_hudCompassSpringyPointers;
extern const dvar_t *cg_hudCompassSoundPingFadeTime;
extern const dvar_t *cg_hudObjectiveMinHeight;
extern const dvar_t *cg_hudObjectiveMaxRange;
extern const dvar_t *cg_hudObjectiveMinAlpha;
extern const dvar_t *cg_hudStanceFlash;
extern const dvar_t *cg_hudStanceHintPrints;
extern const dvar_t *cg_hudDamageIconWidth;
extern const dvar_t *cg_hudDamageIconHeight;
extern const dvar_t *cg_hudDamageIconOffset;
extern const dvar_t *cg_hudDamageIconTime;
extern const dvar_t *cg_hudDamageIconInScope;
extern const dvar_t *cg_hudGrenadeIconInScope;
extern const dvar_t *cg_hudGrenadeIconMaxRange;
extern const dvar_t *cg_hudGrenadeIconMaxHeight;
extern const dvar_t *cg_hudGrenadeIconOffset;
extern const dvar_t *cg_hudGrenadeIconHeight;
extern const dvar_t *cg_hudGrenadeIconWidth;
extern const dvar_t *cg_hudGrenadePointerHeight;
extern const dvar_t *cg_hudGrenadePointerWidth;
extern const dvar_t *cg_hudGrenadePointerPivot;
extern const dvar_t *cg_hudGrenadePointerPulseFreq;
extern const dvar_t *cg_hudGrenadePointerPulseMax;
extern const dvar_t *cg_hudGrenadePointerPulseMin;
extern const dvar_t *cg_hudChatPosition;
extern const dvar_t *cg_hudSayPosition;
extern const dvar_t *cg_hudProneY;
extern const dvar_t *cg_weaponCycleDelay;
extern const dvar_t *cg_crosshairAlpha;
extern const dvar_t *cg_crosshairAlphaMin;
extern const dvar_t *cg_crosshairDynamic;
extern const dvar_t *cg_crosshairEnemyColor;
extern const dvar_t *cg_drawHealth;
extern const dvar_t *cg_drawBreathHint;
extern const dvar_t *cg_drawMantleHint;
extern const dvar_t *cg_draw2D;
extern bgs_t *bgs;
extern const dvar_t *cg_debugEvents;
extern const dvar_t *cg_errorDecay;
extern const dvar_t *cg_nopredict;
extern const dvar_t *cg_showmiss;
extern const dvar_t *cg_footsteps;
extern const dvar_t *cg_marks;
extern const dvar_t *cg_marksLimit;
extern const dvar_t *cg_brass;
extern const dvar_t *cg_gun_x;
extern const dvar_t *cg_gun_y;
extern const dvar_t *cg_gun_z;
extern const dvar_t *cg_gun_move_f;
extern const dvar_t *cg_gun_move_r;
extern const dvar_t *cg_gun_move_u;
extern const dvar_t *cg_gun_ofs_f;
extern const dvar_t *cg_gun_ofs_r;
extern const dvar_t *cg_gun_ofs_u;
extern const dvar_t *cg_gun_move_rate;
extern const dvar_t *cg_gun_move_minspeed;
extern const dvar_t *cg_drawGun;
extern const dvar_t *cg_cursorHints;
extern const dvar_t *cg_hintFadeTime;
extern dvar_t *cg_viewsize;
extern const dvar_t *cg_tracerChance;
extern const dvar_t *cg_tracerWidth;
extern const dvar_t *cg_tracerLength;
extern const dvar_t *cg_tracerSpeed;
extern const dvar_t *cg_tracerScale;
extern const dvar_t *cg_tracerScaleMinDist;
extern const dvar_t *cg_tracerScaleDistRange;
extern const dvar_t *cg_fov;
extern const dvar_t *cg_fovMin;
extern const dvar_t *cg_fovScale;
extern const dvar_t *cg_thirdPersonRange;
extern const dvar_t *cg_thirdPersonAngle;
extern const dvar_t *cg_thirdPerson;
extern const dvar_t *cg_synchronousClients;
extern const dvar_t *cg_paused;
extern const dvar_t *cg_predictItems;
extern const dvar_t *cg_dumpAnims;
extern const dvar_t *cg_developer;
extern const dvar_t *cg_minicon;
extern const dvar_t *cg_subtitles;
extern const dvar_t *cg_subtitleMinTime;
extern const dvar_t *cg_subtitleWidthStandard;
extern const dvar_t *cg_subtitleWidthWidescreen;
extern const dvar_t *cg_subtitlePosX;
extern const dvar_t *cg_subtitlePosY;
extern const dvar_t *cg_subtitleCharHeight;
extern const dvar_t *cg_gameMessageWidth;
extern const dvar_t *cg_gameBoldMessageWidth;
extern const dvar_t *cg_drawGameMessages;
extern const dvar_t *cg_scoreboardScrollStep;
extern const dvar_t *cg_scoreboardBannerHeight;
extern const dvar_t *cg_scoreboardItemHeight;
extern const dvar_t *cg_chatTime;
extern const dvar_t *cg_chatHeight;
extern const dvar_t *cg_teamChatsOnly;
extern const dvar_t *cg_noTaunt;
extern const dvar_t *cg_voiceSpriteTime;
extern const dvar_t *cg_descriptiveText;
extern const dvar_t *cg_drawCrosshairNames;
extern const dvar_t *cg_drawTurretCrosshair;
extern const dvar_t *cg_debugPosition;
extern const dvar_t *cg_shock_screenBlendTime;
extern const dvar_t *cg_shock_screenBlendFadeTime;
extern const dvar_t *cg_shock_viewKickPeriod;
extern const dvar_t *cg_shock_viewKickRadius;
extern const dvar_t *cg_shock_viewKickFadeTime;
extern const dvar_t *cg_shock_sound;
extern const dvar_t *cg_shock_soundFadeInTime;
extern const dvar_t *cg_shock_soundFadeOutTime;
extern const dvar_t *cg_shock_soundLoopFadeTime;
extern const dvar_t *cg_shock_soundLoopEndDelay;
extern const dvar_t *cg_shock_soundRoomType;
extern const dvar_t *cg_shock_soundDryLevel;
extern const dvar_t *cg_shock_soundWetLevel;
extern const dvar_t *cg_shock_soundModEndDelay;
extern const dvar_t *cg_shock_volume_auto;
extern const dvar_t *cg_shock_volume_auto2d;
extern const dvar_t *cg_shock_volume_menu;
extern const dvar_t *cg_shock_volume_weapon;
extern const dvar_t *cg_shock_volume_voice;
extern const dvar_t *cg_shock_volume_item;
extern const dvar_t *cg_shock_volume_body;
extern const dvar_t *cg_shock_volume_local;
extern const dvar_t *cg_shock_volume_music;
extern const dvar_t *cg_shock_volume_announcer;
extern const dvar_t *cg_shock_volume_shellshock;
extern const dvar_t *cg_shock_mouse;
extern const dvar_t *cg_shock_mouse_maxpitchspeed;
extern const dvar_t *cg_shock_mouse_maxyawspeed;
extern const dvar_t *cg_shock_mouse_sensitivityscale;
extern const dvar_t *cg_shock_mouse_fadeTime;
extern const dvar_t *cg_scriptIconSize;
extern const dvar_t *cg_youInKillCamSize;
extern const dvar_t *cg_connectionIconSize;
extern const dvar_t *cg_voiceIconSize;
extern const dvar_t *cg_constantSizeHeadIcons;
extern const dvar_t *cg_headIconMinScreenRadius;
extern const dvar_t *cg_blood;
extern displayContextDef_t cgDC;
extern const dvar_t *cg_drawCrosshairNamesPosX;
extern const dvar_t *cg_drawCrosshairNamesPosY;
extern const dvar_t *cg_centerPrintY;
extern const dvar_t *cg_drawLagometer;
extern const dvar_t *cg_weaponleftbone;
extern const dvar_t *cg_weaponrightbone;
static char buffer[1024];
extern cg_t cgArray[1];
extern weaponInfo_t cg_weaponsArray[CG_MAX_WEAPONS];
extern itemInfo_t cg_itemsArray[CG_MAX_ITEMS];
static Bool g_ambientStarted;
static Bool g_mapLoaded;
/* migrated from the ILP32 data blob to typed C (x64 port Stage 2). */
static const char *cg_soundRoomTypes[27] = {
    "generic",
    "paddedcell",
    "room",
    "bathroom",
    "livingroom",
    "stoneroom",
    "auditorium",
    "concerthall",
    "cave",
    "arena",
    "hangar",
    "carpetedhallway",
    "hallway",
    "stonecorridor",
    "alley",
    "forest",
    "city",
    "mountains",
    "quarry",
    "plain",
    "parkinglot",
    "sewerpipe",
    "underwater",
    "drugged",
    "dizzy",
    "psychotic",
    NULL,
};

/* cg_drawSoundOverlayStrings: migrated from the ILP32 data blob to typed C
 * (re-lays-out per target; 4 entries + [4]=NULL; trailing blob bytes were
 * inter-symbol padding). (x64 port Stage 2.) */
static const char *cg_drawSoundOverlayStrings[5] = {
    "None", "3D",
    "Stream", "2D",
    /* [4] = NULL */
};

/* cg_drawFpsNames: migrated from the ILP32 data blob to typed C (re-lays-out per
 * target). 4 live entries + [4]=NULL; blob's extra trailing bytes were
 * inter-symbol padding (next symbol cl_shownet distinct). Targets are the
 * reconstructed literals in common.c. (x64 port Stage 2.) */
static const char *cg_drawFpsNames[5] = {
    "Off",  /* "Off"     */
    "Simple",  /* "Simple"  */
    "Verbose",
    "Time",  /* "Time"    */
    /* [4] = NULL */
};

void CG_GetEntityOrientation(int entnum, vec_t *origin_out, vec3_t *axis_out);
int CG_CrosshairPlayer(void);
void CG_GameMessage(const char *msg);
void CG_BoldGameMessage(const char *msg);
const char *CG_Argv(int arg);
void __attribute_regparm__(1) CG_RegisterGraphics(const char *mapname);
void CG_StartAmbient(void);
Bool CG_PlaySoundOnFirstClient(void);
Bool CG_PlaySoundOnCurrentLocalClient(int entitynum, const vec_t *origin, const char *aliasname);
static Bool CG_ReplaceDirective(int *searchPos, int *dstLen, char *dstString);
void CG_TranslateHudElemMessage(const char *message, const char *messageType, char *hudElemString);
static void CG_LoadHudMenu(void);
void CG_InitVote(void);
static struct XModel *CG_GetXModel(const char *modelName);
static void CG_CreateDObj(DObjModel_s *dobjModels, unsigned short numModels, struct XAnimTree_s *tree, int handle, clientInfo_t *ci);
void CG_FreeWeapons(void);
void CG_Shutdown(void);
void *Hunk_AllocXAnimPrecache(int size);
void *Hunk_AllocXAnimClient(int size);
int CG_PlaySoundAliasAsMasterByName(int entitynum, const vec_t *origin, const char *aliasname);
void CG_GetDObjOrientation(int dobjHandle, float (*axis)[3]);
void CG_PlaySmokeGrenadesAtTime(int gametime);
int CG_PlaySoundAlias(int entitynum, const vec_t *origin, snd_alias_list_t *aliasList);
int CG_PlaySoundAliasByName(int entitynum, const vec_t *origin, const char *aliasname);
void CG_SafeTranslateHudElemString(int index, char *hudElemString);
int CG_PlayClientSoundAliasByName(const char *aliasname);
int CG_PlayClientSoundAlias(snd_alias_list_t *aliasList);
int CG_PlayEntitySoundAlias(int entitynum, snd_alias_list_t *aliasList);
static void CG_RegisterSounds(void);
void CG_Init(int serverMessageNum, int serverCommandSequence, int clientNum);

void *Hunk_AllocXAnimClient(int size);
static void CG_SubtitlePrint(int msec, const snd_alias_t *pAlias);
int CG_PlaySoundAlias(int entitynum, const vec_t *origin, snd_alias_list_t *aliasList);
int CG_PlaySoundAliasByName(int entitynum, const vec_t *origin, const char *aliasname);

static void CG_RegisterDvars(void)
{
    cg_drawGun = Dvar_RegisterBool("cg_drawGun", 1, 0x1080);
    cg_cursorHints = Dvar_RegisterInt("cg_cursorHints", 4, 0, 4, 0x1001);
    cg_hintFadeTime = Dvar_RegisterInt("cg_hintFadeTime", 0x64, 0, 0x7fffffff, 0x1001);
    cg_fov = Dvar_RegisterFloat("cg_fov", 80.0f, 1.0f, 160.0f, 0x1080);
    cg_fovScale = Dvar_RegisterFloat("cg_fovScale", 1.0f, 0.200000003f, 2.0f, 0x1080);
    cg_fovMin = Dvar_RegisterFloat("cg_fovMin", 10.0f, 1.0f, 160.0f, 0x1080);
    cg_viewsize = Dvar_RegisterInt("cg_viewsize", 0x64, 0x1e, 0x64, 0x1001);
    cg_draw2D = Dvar_RegisterBool("cg_draw2D", 1, 0x1080);
    cg_drawHealth = Dvar_RegisterBool("cg_drawHealth", 0, 0x1080);
    cg_drawBreathHint = Dvar_RegisterBool("cg_drawBreathHint", 1, 0x1001);
    cg_drawMantleHint = Dvar_RegisterBool("cg_drawMantleHint", 1, 0x1001);
    cg_drawFPS = Dvar_RegisterEnum("cg_drawFPS", cg_drawFpsNames, 0, 0x1001);
    cg_drawSoundOverlay = Dvar_RegisterEnum("cg_drawSoundOverlay", cg_drawSoundOverlayStrings, 0, 0x1000);
    cg_drawScriptUsage = Dvar_RegisterBool("cg_drawScriptUsage", 0, 0x1000);
    cg_drawMaterial = Dvar_RegisterBool("cg_drawMaterial", 0, 0x1080);
    cg_drawSnapshot = Dvar_RegisterBool("cg_drawSnapshot", 0, 0x1001);
    cg_drawTurretCrosshair = Dvar_RegisterBool("cg_drawTurretCrosshair", 1, 0x1001);
    cg_drawCrosshairNames = Dvar_RegisterBool("cg_drawCrosshairNames", 1, 0x1001);
    cg_drawCrosshairNamesPosX = Dvar_RegisterInt("cg_drawCrosshairNamesPosX", 0x12c, 0, 0x280, 0x1000);
    cg_drawCrosshairNamesPosY = Dvar_RegisterInt("cg_drawCrosshairNamesPosY", 0xb4, 0, 0x1e0, 0x1000);
    cg_hudCompassSize = Dvar_RegisterFloat("cg_hudCompassSize", 1.0f, 0.0f, FLT_MAX, 0x1001);
    cg_hudCompassMaxRange = Dvar_RegisterFloat("cg_hudCompassMaxRange", 1500.0f, 0.0f, FLT_MAX, 0x1001);
    cg_hudCompassMinRange = Dvar_RegisterFloat("cg_hudCompassMinRange", 0.0f, 0.0f, FLT_MAX, 0x1001);
    cg_hudCompassMinRadius = Dvar_RegisterFloat("cg_hudCompassMinRadius", 0.0f, 0.0f, FLT_MAX, 0x1001);
    cg_hudCompassSpringyPointers = Dvar_RegisterBool("cg_hudCompassSpringyPointers", 0, 0x1001);
    cg_hudCompassSoundPingFadeTime = Dvar_RegisterFloat("cg_hudCompassSoundPingFadeTime", 2.0f, 0.0f, 10.0f, 0x1081);
    cg_hudObjectiveMinHeight = Dvar_RegisterFloat("cg_hudObjectiveMinHeight", -70.0f, -FLT_MAX, 0.0f, 0x1001);
    cg_hudObjectiveMaxRange = Dvar_RegisterFloat("cg_hudObjectiveMaxRange", 2048.0f, 0.0f, FLT_MAX, 0x1001);
    cg_hudObjectiveMinAlpha = Dvar_RegisterFloat("cg_hudObjectiveMinAlpha", 1.0f, 0.0f, 1.0f, 0x1001);
    cg_hudStanceFlash = Dvar_RegisterColor("cg_hudStanceFlash", 1.0f, 1.0f, 1.0f, 1.0f, 0x1000);
    cg_hudStanceHintPrints = Dvar_RegisterBool("cg_hudStanceHintPrints", 0, 0x1001);
    cg_hudDamageIconWidth = Dvar_RegisterFloat("cg_hudDamageIconWidth", 128.0f, 0.0f, 512.0f, 0x1001);
    cg_hudDamageIconHeight = Dvar_RegisterFloat("cg_hudDamageIconHeight", 64.0f, 0.0f, 512.0f, 0x1001);
    cg_hudDamageIconOffset = Dvar_RegisterFloat("cg_hudDamageIconOffset", 128.0f, 0.0f, 512.0f, 0x1001);
    cg_hudDamageIconTime = Dvar_RegisterInt("cg_hudDamageIconTime", 0x7d0, 0, 0x7fffffff, 0x1001);
    cg_hudDamageIconInScope = Dvar_RegisterBool("cg_hudDamageIconInScope", 0, 0x1080);
    cg_hudGrenadeIconMaxRange = Dvar_RegisterFloat("cg_hudGrenadeIconMaxRange", 250.0f, 0.0f, 1000.0f, 0x1080);
    cg_hudGrenadeIconMaxHeight = Dvar_RegisterFloat("cg_hudGrenadeIconMaxHeight", 104.0f, 0.0f, 1000.0f, 0x1080);
    cg_hudGrenadeIconInScope = Dvar_RegisterBool("cg_hudGrenadeIconInScope", 0, 0x1080);
    cg_hudGrenadeIconOffset = Dvar_RegisterFloat("cg_hudGrenadeIconOffset", 50.0f, 0.0f, 512.0f, 0x1001);
    cg_hudGrenadeIconHeight = Dvar_RegisterFloat("cg_hudGrenadeIconHeight", 25.0f, 0.0f, 512.0f, 0x1001);
    cg_hudGrenadeIconWidth = Dvar_RegisterFloat("cg_hudGrenadeIconWidth", 25.0f, 0.0f, 512.0f, 0x1001);
    cg_hudGrenadePointerHeight = Dvar_RegisterFloat("cg_hudGrenadePointerHeight", 12.0f, 0.0f, 512.0f, 0x1001);
    cg_hudGrenadePointerWidth = Dvar_RegisterFloat("cg_hudGrenadePointerWidth", 25.0f, 0.0f, 512.0f, 0x1001);
    cg_hudGrenadePointerPivot = Dvar_RegisterVec2("cg_hudGrenadePointerPivot", 12.0f, 27.0f, 0.0f, 512.0f, 0x1001);
    cg_hudGrenadePointerPulseFreq = Dvar_RegisterFloat("cg_hudGrenadePointerPulseFreq", 1.70000005f, 0.100000001f, 50.0f, 0x1000);
    cg_hudGrenadePointerPulseMax = Dvar_RegisterFloat("cg_hudGrenadePointerPulseMax", 1.85000002f, 0.0f, 3.0f, 0x1000);
    cg_hudGrenadePointerPulseMin = Dvar_RegisterFloat("cg_hudGrenadePointerPulseMin", 0.300000012f, -3.0f, 1.0f, 0x1000);
    cg_hudChatPosition = Dvar_RegisterVec2("cg_hudChatPosition", 5.0f, 150.0f, 0.0f, 640.0f, 0x1001);
    cg_hudSayPosition = Dvar_RegisterVec2("cg_hudSayPosition", 5.0f, 126.0f, 0.0f, 640.0f, 0x1001);
    cg_drawLagometer = Dvar_RegisterBool("cg_drawLagometer", 0, 0x1001);
    cg_centerPrintY = Dvar_RegisterFloat("cg_centerPrintY", -60.0f, -10000.0f, 10000.0f, 0x1001);
    cg_hudProneY = Dvar_RegisterFloat("cg_hudProneY", -160.0f, -10000.0f, 10000.0f, 0x1001);
    cg_weaponCycleDelay = Dvar_RegisterInt("cg_weaponCycleDelay", 0, 0, 0x7fffffff, 0x1001);
    cg_crosshairAlpha = Dvar_RegisterFloat("cg_crosshairAlpha", 1.0f, 0.0f, 1.0f, 0x1081);
    cg_crosshairAlphaMin = Dvar_RegisterFloat("cg_crosshairAlphaMin", 0.0f, 0.0f, 1.0f, 0x1081);
    cg_crosshairDynamic = Dvar_RegisterBool("cg_crosshairDynamic", 0, 0x1081);
    cg_crosshairEnemyColor = Dvar_RegisterBool("cg_crosshairEnemyColor", 1, 0x1081);
    cg_brass = Dvar_RegisterBool("cg_brass", 1, 0x1001);
    cg_marks = Dvar_RegisterBool("cg_marks", 1, 0x1001);
    cg_marksLimit = Dvar_RegisterInt("cg_marksLimit", 0x400, 0, 0x400, 0x1001);
    cg_gun_x = Dvar_RegisterFloat("cg_gun_x", 0.0f, -FLT_MAX, FLT_MAX, 0x1080);
    cg_gun_y = Dvar_RegisterFloat("cg_gun_y", 0.0f, -FLT_MAX, FLT_MAX, 0x1080);
    cg_gun_z = Dvar_RegisterFloat("cg_gun_z", 0.0f, -FLT_MAX, FLT_MAX, 0x1080);
    cg_gun_move_f = Dvar_RegisterFloat("cg_gun_move_f", 0.0f, -FLT_MAX, FLT_MAX, 0x1080);
    cg_gun_move_r = Dvar_RegisterFloat("cg_gun_move_r", 0.0f, -FLT_MAX, FLT_MAX, 0x1080);
    cg_gun_move_u = Dvar_RegisterFloat("cg_gun_move_u", 0.0f, -FLT_MAX, FLT_MAX, 0x1080);
    cg_gun_ofs_f = Dvar_RegisterFloat("cg_gun_ofs_f", 0.0f, -FLT_MAX, FLT_MAX, 0x1080);
    cg_gun_ofs_r = Dvar_RegisterFloat("cg_gun_ofs_r", 0.0f, -FLT_MAX, FLT_MAX, 0x1080);
    cg_gun_ofs_u = Dvar_RegisterFloat("cg_gun_ofs_u", 0.0f, -FLT_MAX, FLT_MAX, 0x1080);
    cg_gun_move_rate = Dvar_RegisterFloat("cg_gun_move_rate", 0.0f, -FLT_MAX, FLT_MAX, 0x1080);
    cg_gun_move_minspeed = Dvar_RegisterFloat("cg_gun_move_minspeed", 0.0f, -FLT_MAX, FLT_MAX, 0x1080);
    cg_centertime = Dvar_RegisterFloat("cg_centertime", 5.0f, 0.0f, FLT_MAX, 0x1080);
    cg_debugPosition = Dvar_RegisterBool("cg_debugposition", 0, 0x1080);
    cg_debugEvents = Dvar_RegisterBool("cg_debugevents", 0, 0x1080);
    cg_errorDecay = Dvar_RegisterFloat("cg_errordecay", 100.0f, 0.0f, FLT_MAX, 0x1000);
    cg_nopredict = Dvar_RegisterBool("cg_nopredict", 0, 0x1000);
    cg_showmiss = Dvar_RegisterInt("cg_showmiss", 0, 0, 2, 0x1000);
    cg_footsteps = Dvar_RegisterBool("cg_footsteps", 1, 0x1080);
    cg_tracerChance = Dvar_RegisterFloat("cg_tracerchance", 0.400000006f, 0.0f, 1.0f, 0x1080);
    cg_tracerWidth = Dvar_RegisterFloat("cg_tracerwidth", 0.800000012f, 0.0f, 8.0f, 0x1080);
    cg_tracerSpeed = Dvar_RegisterFloat("cg_tracerSpeed", 4500.0f, 0.0f, FLT_MAX, 0x1080);
    cg_tracerLength = Dvar_RegisterFloat("cg_tracerlength", 160.0f, 0.0f, FLT_MAX, 0x1080);
    cg_tracerScale = Dvar_RegisterFloat("cg_tracerScale", 50.0f, 1.0f, FLT_MAX, 0x1080);
    cg_tracerScaleMinDist = Dvar_RegisterFloat("cg_tracerScaleMinDist", 5000.0f, 0.0f, FLT_MAX, 0x1080);
    cg_tracerScaleDistRange = Dvar_RegisterFloat("cg_tracerScaleDistRange", 25000.0f, 0.0f, FLT_MAX, 0x1080);
    cg_thirdPersonRange = Dvar_RegisterFloat("cg_thirdPersonRange", 120.0f, 0.0f, 1024.0f, 0x1080);
    cg_thirdPersonAngle = Dvar_RegisterFloat("cg_thirdPersonAngle", 0.0f, -180.0f, 360.0f, 0x1080);
    cg_thirdPerson = Dvar_RegisterBool("cg_thirdPerson", 0, 0x1080);
    cg_chatTime = Dvar_RegisterInt("cg_chatTime", 0x2ee0, 0, 0xea60, 0x1001);
    cg_chatHeight = Dvar_RegisterInt("cg_chatHeight", 8, 0, 8, 0x1001);
    cg_predictItems = Dvar_RegisterBool("cg_predictItems", 1, 0x1003);
    cg_noTaunt = Dvar_RegisterBool("cg_noTaunt", 0, 0x1001);
    cg_voiceSpriteTime = Dvar_RegisterInt("cg_voiceSpriteTime", 0x1770, 0, 0xea60, 0x1001);
    cg_teamChatsOnly = Dvar_RegisterBool("cg_teamChatsOnly", 0, 0x1001);
    cg_paused = Dvar_RegisterInt("cl_paused", 0, 0, 2, 0x1040);
    cg_synchronousClients = Dvar_RegisterBool("g_synchronousClients", 0, 0x1008);
    cg_dumpAnims = Dvar_RegisterInt("cg_dumpAnims", -1, -1, 0x3ff, 0x1080);
    cg_developer = Dvar_RegisterInt("developer", 0, 0, 2, 0x1000);
    cg_minicon = Dvar_RegisterBool("con_minicon", 0, 0x1001);
    cg_subtitles = Dvar_RegisterBool("cg_subtitles", 1, 0x1001);
    cg_subtitleMinTime = Dvar_RegisterFloat("cg_subtitleMinTime", 3.0f, 0.0f, FLT_MAX, 0x1001);
    cg_subtitleWidthStandard = Dvar_RegisterInt("cg_subtitleWidthStandard", 0x50, 0x14, 0x100, 0x1001);
    cg_subtitleWidthWidescreen = Dvar_RegisterInt("cg_subtitleWidthWidescreen", 0x50, 0x14, 0x100, 0x1001);
    cg_subtitlePosX = Dvar_RegisterInt("cg_subtitlePosX", 0x78, 0, 0x280, 0x1001);
    cg_subtitlePosY = Dvar_RegisterInt("cg_subtitlePosY", 0x190, 0, 0x1e0, 0x1001);
    cg_subtitleCharHeight = Dvar_RegisterInt("cg_subtitleCharHeight", 0xc, 1, 0x1e0, 0x1001);
    cg_gameMessageWidth = Dvar_RegisterInt("cg_gameMessageWidth", 0x46, 0x14, 0x100, 0x1001);
    cg_gameBoldMessageWidth = Dvar_RegisterInt("cg_gameBoldMessageWidth", 0x3c, 0x14, 0x100, 0x1001);
    cg_drawGameMessages = Dvar_RegisterBool("cg_drawGameMessages", 1, 0x1000);
    cg_scoreboardScrollStep = Dvar_RegisterInt("cg_scoreboardScrollStep", 3, 1, 8, 0x1001);
    cg_scoreboardBannerHeight = Dvar_RegisterInt("cg_scoreboardBannerHeight", 0x18, 1, 0x64, 0x1001);
    cg_scoreboardItemHeight = Dvar_RegisterInt("cg_scoreboardItemHeight", 0x12, 1, 0x64, 0x1001);
    cg_descriptiveText = Dvar_RegisterBool("cg_descriptiveText", 1, 0x1001);
    cg_shock_screenBlendTime = Dvar_RegisterFloat("cg_shock_screenBlendTime", 0.400000006f, 0.00100000005f, 10.0f, 0x1080);
    cg_shock_screenBlendFadeTime = Dvar_RegisterFloat("cg_shock_screenBlendFadeTime", 1.0f, 0.00100000005f, 10.0f, 0x1080);
    cg_shock_viewKickPeriod = Dvar_RegisterFloat("cg_shock_viewKickPeriod", 0.75f, 0.00100000005f, 10.0f, 0x1080);
    cg_shock_viewKickRadius = Dvar_RegisterFloat("cg_shock_viewKickRadius", 0.0500000007f, 0.0f, 1.0f, 0x1080);
    cg_shock_sound = Dvar_RegisterBool("cg_shock_sound", 1, 0x1080);
    cg_shock_soundFadeInTime = Dvar_RegisterFloat("cg_shock_soundFadeInTime", 0.25f, 0.00100000005f, 10.0f, 0x1080);
    cg_shock_soundFadeOutTime = Dvar_RegisterFloat("cg_shock_soundFadeOutTime", 2.5f, 0.00100000005f, 10.0f, 0x1080);
    cg_shock_soundLoopFadeTime = Dvar_RegisterFloat("cg_shock_soundLoopFadeTime", 1.5f, 0.00100000005f, 10.0f, 0x1080);
    cg_shock_soundLoopEndDelay = Dvar_RegisterFloat("cg_shock_soundLoopEndDelay", -3.0f, -10.0f, 10.0f, 0x1080);
    cg_shock_soundRoomType = Dvar_RegisterEnum("cg_shock_soundRoomType", cg_soundRoomTypes, 0, 0x1080);
    cg_shock_soundDryLevel = Dvar_RegisterFloat("cg_shock_soundDryLevel", 1.0f, 0.0f, 1.0f, 0x1080);
    cg_shock_soundWetLevel = Dvar_RegisterFloat("cg_shock_soundWetLevel", 0.5f, 0.0f, 1.0f, 0x1080);
    cg_shock_soundModEndDelay = Dvar_RegisterFloat("cg_shock_soundModEndDelay", 2.0f, -10.0f, 10.0f, 0x1080);
    cg_shock_volume_auto = Dvar_RegisterFloat("cg_shock_volume_auto", 0.100000001f, 0.0f, 1.0f, 0x1080);
    cg_shock_volume_auto2d = Dvar_RegisterFloat("cg_shock_volume_auto2d", 0.100000001f, 0.0f, 1.0f, 0x1080);
    cg_shock_volume_menu = Dvar_RegisterFloat("cg_shock_volume_menu", 1.0f, 0.0f, 1.0f, 0x1080);
    cg_shock_volume_weapon = Dvar_RegisterFloat("cg_shock_volume_weapon", 0.5f, 0.0f, 1.0f, 0x1080);
    cg_shock_volume_voice = Dvar_RegisterFloat("cg_shock_volume_voice", 0.200000003f, 0.0f, 1.0f, 0x1080);
    cg_shock_volume_item = Dvar_RegisterFloat("cg_shock_volume_item", 0.100000001f, 0.0f, 1.0f, 0x1080);
    cg_shock_volume_body = Dvar_RegisterFloat("cg_shock_volume_body", 0.100000001f, 0.0f, 1.0f, 0x1080);
    cg_shock_volume_local = Dvar_RegisterFloat("cg_shock_volume_local", 1.0f, 0.0f, 1.0f, 0x1080);
    cg_shock_volume_music = Dvar_RegisterFloat("cg_shock_volume_music", 1.0f, 0.0f, 1.0f, 0x1080);
    cg_shock_volume_announcer = Dvar_RegisterFloat("cg_shock_volume_announcer", 1.0f, 0.0f, 1.0f, 0x1080);
    cg_shock_volume_shellshock = Dvar_RegisterFloat("cg_shock_volume_shellshock", 1.0f, 0.0f, 1.0f, 0x1080);
    cg_shock_mouse = Dvar_RegisterBool("cg_shock_mouse", 1, 0x1080);
    cg_shock_mouse_maxpitchspeed = Dvar_RegisterFloat("cg_shock_mouse_maxpitchspeed", 90.0f, 0.0f, FLT_MAX, 0x1080);
    cg_shock_mouse_maxyawspeed = Dvar_RegisterFloat("cg_shock_mouse_maxyawspeed", 90.0f, 0.0f, FLT_MAX, 0x1080);
    cg_shock_mouse_sensitivityscale = Dvar_RegisterFloat("cg_shock_mouse_sensitivityscale", 0.5f, 0.0f, 2.0f, 0x1080);
    cg_shock_mouse_fadeTime = Dvar_RegisterFloat("cg_shock_mouse_fadeTime", 2.0f, 0.00100000005f, 10.0f, 0x1080);
    cg_youInKillCamSize = Dvar_RegisterFloat("cg_youInKillCamSize", 6.0f, 0.0f, 100.0f, 0x1001);
    cg_scriptIconSize = Dvar_RegisterFloat("cg_scriptIconSize", 0.0f, 0.0f, 100.0f, 0x1001);
    cg_connectionIconSize = Dvar_RegisterFloat("cg_connectionIconSize", 0.0f, 0.0f, 100.0f, 0x1001);
    cg_voiceIconSize = Dvar_RegisterFloat("cg_voiceIconSize", 0.0f, 0.0f, 100.0f, 0x1001);
    cg_constantSizeHeadIcons = Dvar_RegisterBool("cg_constantSizeHeadIcons", 0, 0x1080);
    cg_headIconMinScreenRadius = Dvar_RegisterFloat("cg_headIconMinScreenRadius", 0.0199999996f, 0.0f, 1.0f, 0x1001);
    cg_blood = Dvar_RegisterBool("cg_blood", 1, 0x1001);
    ((LegacyHacks *)::legacyHacks)->cl_serverloadmap[0] = 0;
    ((LegacyHacks *)::legacyHacks)->cl_serverloadgametype[0] = 0;
    ((LegacyHacks *)::legacyHacks)->cl_serverloadwaiting = 0;
    cg_weaponleftbone = Dvar_RegisterString("cg_weaponleftbone", "tag_weapon_left", 0x1000);
    cg_weaponrightbone = Dvar_RegisterString("cg_weaponrightbone", "tag_weapon_right", 0x1000);
    ((cgs_t *)cgs)->localServer = com_sv_running->current.enabled;
    ((LegacyHacks *)::legacyHacks)->cl_stance = 0;
    CL_SetADS(0);
    (*(unsigned char *)&((cg_t *)cg)->objectiveText[0]) = 0;
    CG_OffhandRegisterDvars();
    BG_RegisterDvars();
}

// Retail fills axis[0..2] and the origin in row 3.
void CG_GetDObjOrientation(int dobjHandle, float (*axis)[3])
{
    centity_t *cent;

    if (dobjHandle >= 0 && dobjHandle < 1024) {
        cent = &cg_entities[dobjHandle];
        axis[3][0] = cent->lerpOrigin[0];
        axis[3][1] = cent->lerpOrigin[1];
        axis[3][2] = cent->lerpOrigin[2];
        AnglesToAxis(cent->lerpAngles, axis);
    } else if (dobjHandle >= 1024 && dobjHandle < 1152) {
        axis[3][0] = cg->viewModelOrigin[0];
        axis[3][1] = cg->viewModelOrigin[1];
        axis[3][2] = cg->viewModelOrigin[2];
        AxisCopy(cg->viewModelAxis, axis);
    }
}

void CG_GetEntityOrientation(int entnum, vec_t *origin_out, vec3_t *axis_out)
{
    origin_out[0] = cg_entities[entnum].lerpOrigin[0];
    origin_out[1] = cg_entities[entnum].lerpOrigin[1];
    origin_out[2] = cg_entities[entnum].lerpOrigin[2];
    AnglesToAxis(cg_entities[entnum].lerpAngles, axis_out);
}

int CG_CrosshairPlayer(void)
{
    if (cgArray[0].time > cgArray[0].crosshairClientTime + 0x3e8)
        return -1;
    return cgArray[0].crosshairClientNum;
}

void CG_GameMessage(const char *msg)
{
    CL_ConsolePrint((print_msg_type_t)1, msg, 0, cg_gameMessageWidth->current.integer);
}

void CG_BoldGameMessage(const char *msg)
{
    CL_ConsolePrint((print_msg_type_t)2, msg, 0, cg_gameBoldMessageWidth->current.integer);
}

const char *CG_Argv(int arg)
{
    Cmd_ArgvBuffer(arg, (char *)&buffer, 0x400);
    return (const char *)&buffer;
}

static void CG_RegisterSurfaceSoundAliases(const char *aliasBase, snd_alias_list_t **dest)
{
    char aliasName[256];
    int surfaceType;

    for (surfaceType = 0; surfaceType < 0x17; surfaceType++) {
        sprintf(aliasName, "%s_%s", aliasBase, Com_SurfaceTypeToName(surfaceType));
        dest[surfaceType] = Com_FindSoundAlias(aliasName);
    }
}

static void CG_RegisterSounds(void)
{
    cgs->media.noAmmoSound = Com_FindSoundAlias("player_out_of_ammo");
    cgs->media.landDmgSound = Com_FindSoundAlias("land_damage");

    CG_RegisterSurfaceSoundAliases("grenade_bounce", cgs->media.grenadeBounceSound);
    CG_RegisterSurfaceSoundAliases("grenade_explode", cgs->media.grenadeExplodeSound);
    CG_RegisterSurfaceSoundAliases("rocket_explode", cgs->media.rocketExplodeSound);
    CG_RegisterSurfaceSoundAliases("bullet_small", cgs->media.bulletHitSmallSound);
    CG_RegisterSurfaceSoundAliases("bulletspray_small", cgs->media.shotgunHitSound);
    CG_RegisterSurfaceSoundAliases("bullet_large", cgs->media.bulletHitLargeSound);
    CG_RegisterSurfaceSoundAliases("step_run", cgs->media.stepRunSound);
    CG_RegisterSurfaceSoundAliases("step_run_plr", cgs->media.stepRunSoundPlayer);
    CG_RegisterSurfaceSoundAliases("step_walk", cgs->media.stepWalkSound);
    CG_RegisterSurfaceSoundAliases("step_walk_plr", cgs->media.stepWalkSoundPlayer);
    CG_RegisterSurfaceSoundAliases("step_prone", cgs->media.stepProneSound);
    CG_RegisterSurfaceSoundAliases("step_prone_plr", cgs->media.stepProneSoundPlayer);
    CG_RegisterSurfaceSoundAliases("land", cgs->media.landSound);
    CG_RegisterSurfaceSoundAliases("land_plr", cgs->media.landSoundPlayer);

    cgs->media.runningEquipmentSound = Com_FindSoundAlias("gear_rattle_run");
    cgs->media.runningEquipmentSoundPlayer = Com_FindSoundAlias("gear_rattle_plr_run");
    cgs->media.walkingEquipmentSound = Com_FindSoundAlias("gear_rattle_walk");
    cgs->media.walkingEquipmentSoundPlayer = Com_FindSoundAlias("gear_rattle_plr_walk");
    cgs->media.foliageMovement = Com_FindSoundAlias("movement_foliage");
    cgs->media.bulletWhizby = Com_FindSoundAlias("whizby");
    cgs->media.meleeSwingLarge = Com_FindSoundAlias("melee_swing_large");
    cgs->media.meleeSwingSmall = Com_FindSoundAlias("melee_swing_small");
    cgs->media.meleeHit = Com_FindSoundAlias("melee_hit");
    cgs->media.meleeHitOther = Com_FindSoundAlias("melee_hit_other");
    cgs->media.playerHeartBeatSound = Com_FindSoundAlias("weap_sniper_heartbeat");
    cgs->media.playerBreathInSound = Com_FindSoundAlias("weap_sniper_breathin");
    cgs->media.playerBreathOutSound = Com_FindSoundAlias("weap_sniper_breathout");
    cgs->media.playerBreathGaspSound = Com_FindSoundAlias("weap_sniper_breathgasp");
    cgs->media.playerSwapOffhand = Com_FindSoundAlias("weap_offhand_select");
}

void __attribute_regparm__(1) CG_RegisterGraphics(const char *mapname)
{
    int i, j;
    const char *str;
    vec3_t mins, maxs;

    SCR_UpdateScreen();
    Com_Printf("^5---------- Fx System Initialization ---------\n");
    FX_InitSystem(1);
    FX_CreateDefaultEffect();
    Com_Printf("^5----- Fx System Initialization Complete -----\n");

    CG_LoadingString(" - textures");

    cgs->media.lagometerMaterial = (MaterialHandle)(CL_RegisterMaterial("lagometer", 7));
    cgs->media.connectionMaterial = (MaterialHandle)(CL_RegisterMaterial("headicondisconnected", 7));
    cgs->media.youInKillCamMaterial = (MaterialHandle)(CL_RegisterMaterial("headiconyouinkillcam", 7));
    CL_RegisterMaterial("killiconmelee", 7);
    CL_RegisterMaterial("killiconsuicide", 7);
    CL_RegisterMaterial("killiconfalling", 7);
    CL_RegisterMaterial("killiconcrush", 7);
    CL_RegisterMaterial("killicondied", 7);
    cgs->media.tracerMaterial = (MaterialHandle)(CL_RegisterMaterial("gfx/misc/tracer", 6));
    cgs->media.hintMaterials[2] = (MaterialHandle)(CL_RegisterMaterial("gfx/icons/hint_usable", 7));
    cgs->media.hintMaterials[3] = (MaterialHandle)(CL_RegisterMaterial("hint_health", 7));
    cgs->media.hintMaterials[4] = (MaterialHandle)(CL_RegisterMaterial("hint_friendly", 7));
    cgs->media.stanceMaterials[0] = (MaterialHandle)(CL_RegisterMaterial("stance_stand", 7));
    cgs->media.stanceMaterials[1] = (MaterialHandle)(CL_RegisterMaterial("stance_crouch", 7));
    cgs->media.stanceMaterials[2] = (MaterialHandle)(CL_RegisterMaterial("stance_prone", 7));
    cgs->media.stanceMaterials[3] = (MaterialHandle)(CL_RegisterMaterial("stance_flash", 7));
    cgs->media.objectiveMaterials[0] = (MaterialHandle)(CL_RegisterMaterial("objective", 7));
    cgs->media.friendMaterials[0] = (MaterialHandle)(CL_RegisterMaterial("objective_friendly", 7));
    cgs->media.friendMaterials[1] = (MaterialHandle)(CL_RegisterMaterial("objective_friendly_chat", 7));
    cgs->media.damageMaterial = (MaterialHandle)(CL_RegisterMaterial("hit_direction", 7));
    cgs->media.mantleHint = (MaterialHandle)(CL_RegisterMaterial("hint_mantle", 7));
    cgs->media.checkbox_clear = (MaterialHandle)(CL_RegisterMaterialNoMip("ui/assets/checkbox_clear", 7));
    cgs->media.checkbox_checked = (MaterialHandle)(CL_RegisterMaterialNoMip("ui/assets/checkbox_checked", 7));
    cgs->media.checkbox_fail = (MaterialHandle)(CL_RegisterMaterialNoMip("ui/assets/checkbox_fail", 7));
    cgs->media.compassping_friendlyfiring = (MaterialHandle)(CL_RegisterMaterialNoMip("compassping_friendlyfiring", 7));
    cgs->media.compassping_friendlyyelling = (MaterialHandle)(CL_RegisterMaterialNoMip("compassping_friendlyyelling", 7));
    cgs->media.compassping_enemyfiring = (MaterialHandle)(CL_RegisterMaterialNoMip("compassping_enemyfiring", 7));
    cgs->media.compassping_enemyyelling = (MaterialHandle)(CL_RegisterMaterialNoMip("compassping_enemyyelling", 7));
    cgs->media.compassping_grenade = (MaterialHandle)(CL_RegisterMaterialNoMip("compassping_grenade", 7));
    cgs->media.compassping_explosion = (MaterialHandle)(CL_RegisterMaterialNoMip("compassping_explosion", 7));
    cgs->media.grenadeIcon = (MaterialHandle)(CL_RegisterMaterialNoMip("hud_grenadeicon", 7));
    cgs->media.grenadePointer = (MaterialHandle)(CL_RegisterMaterialNoMip("hud_grenadepointer", 7));
    cgs->media.teamStatusBar = (MaterialHandle)(CL_RegisterMaterial("hudcolorbar", 7));

    CG_LoadingString(" - models");
    cgs->media.voiceChatMaterial = (MaterialHandle)(CL_RegisterMaterial("headiconvoicechat", 7));
    cgs->media.balloonMaterial = (MaterialHandle)(CL_RegisterMaterial("headicontalkballoon", 7));
    CG_RegisterScoreboardGraphics();

    memset((void *)cg_itemsArray, 0, CG_MAX_ITEMS * sizeof(itemInfo_t));
    memset((void *)cg_weaponsArray, 0, CG_MAX_WEAPONS * sizeof(weaponInfo_t));

    CG_LoadingString(" - items");
    CG_RegisterItems();

    CG_LoadingString(" - inline models");
    cgs->numInlineModels = CM_NumInlineModels();

    for (i = 1; i < cgs->numInlineModels; i++) {
        cgs->inlineDrawModel[i] = CL_RegisterInlineModel(i);

        GfxBrushModel *model = cgs->inlineDrawModel[i];
        CL_ModelBounds(model, mins, maxs);

        for (j = 0; j < 3; j++) {
            float mid = (float)(mins[j] + (maxs[j] - mins[j]) * 0.5);
            cgs->inlineModelMidpoints[i][j] = mid;
        }
    }

    CG_LoadingString(" - server models");
    for (i = 1; i < 256; i++) {
        str = CL_GetConfigString(0x14e + i);
        if (str[0] == '\0')
            break;

        SCR_UpdateScreen();
        cgs->gameModels[i] = CL_RegisterModel(str);
    }

    for (i = 1; i < 64; i++) {
        str = CL_GetConfigString(0x34e + i);
        if (str[0] == '\0')
            break;
        cgs->fxs[i] = (EffectTemplate *)(FX_RegisterEffect(str));
    }

    cgs->smokeGrenadeFx = (EffectTemplate *)(FX_RegisterEffect("fx/props/american_smoke_grenade.efx"));

    for (i = 1; i < 16; i++) {
        str = CL_GetConfigString(0x48e + i);
        if (str[0] == '\0')
            break;
        if (!CG_LoadShellShockDvars(str)) {
            Com_Error(ERR_DROP, "couldn't register shell shock '%s' -- see console\n", str);
        }
        CG_SetShellShockParmsFromDvars(&cgs->shellshockParms[i]);
    }

    if (!CG_LoadShellShockDvars("hold_breath")) {
        Com_Error(ERR_DROP, "Couldn't find shock file [hold_breath.shock]\n");
    }
    CG_SetShellShockParmsFromDvars(&cgs->holdBreathParams);

    cgs->media.fx = CG_RegisterImpactEffects(mapname);
    if (!cgs->media.fx) {
        Com_Error(ERR_DROP, "Error reading CSV files in the fx directory to identify impact effects");
    }
    cgs->media.fxNoBloodFleshHit = (EffectTemplate *)(FX_RegisterEffect("fx/impacts/flesh_hit_noblood.efx"));

    CG_LoadingString(" - game media done");
}

void CG_StartAmbient(void)
{
    const char *infoString;
    const snd_alias_t *alias;
    int fadeTime;
    int diff;

    infoString = CL_GetConfigString(3);
    alias = CL_PickSoundAlias(Info_ValueForKey(infoString, "n"));
    fadeTime = atoi(Info_ValueForKey(infoString, "t"));
    diff = fadeTime - cg->time;
    if (diff < 0 || cg->time == 0)
        diff = 0;
    SND_PlayAmbientAlias(alias, diff, (snd_alias_system_t)(1));
}

Bool CG_PlaySoundOnFirstClient(void)
{
    return 1;
}

Bool CG_PlaySoundOnCurrentLocalClient(int entitynum, const vec_t *origin, const char *aliasname)
{
    return 1;
}

int CG_PlayClientSoundAlias(snd_alias_list_t *aliasList)
{
    return CG_PlaySoundAlias(cgArray[0].nextSnap->ps.clientNum, cgArray[0].nextSnap->ps.origin, aliasList);
}

int CG_PlayClientSoundAliasByName(const char *aliasname)
{
    return CG_PlaySoundAliasByName(cgArray[0].nextSnap->ps.clientNum, cgArray[0].nextSnap->ps.origin, aliasname);
}

int CG_PlayEntitySoundAlias(int entitynum, snd_alias_list_t *aliasList)
{
    const vec_t *origin;
    const snd_alias_t *alias;
    int msec;

    origin = (const vec_t *)((const byte *)cg_entities + entitynum * 548 + 0x108);
    alias = Com_PickSoundAliasFromList(aliasList);
    if (!alias)
        return 0;

    msec = SND_PlaySoundAlias(alias, entitynum, origin, 0, SASYS_CGAME);
    CG_SubtitlePrint(msec, alias);
    return msec;
}

// retail 0x4ba080, unreferenced; name unknown (by-name CG_PlayEntitySoundAlias)
int CG_PlayEntitySoundAliasByName(int entitynum, const char *aliasname)
{
    const vec_t *origin;
    const snd_alias_t *alias;
    int msec;

    origin = (const vec_t *)((const byte *)cg_entities + entitynum * 548 + 0x108);
    alias = Com_PickSoundAliasFromList(Com_FindSoundAlias(aliasname));
    if (!alias)
        return 0;

    msec = SND_PlaySoundAlias(alias, entitynum, origin, 0, SASYS_CGAME);
    CG_SubtitlePrint(msec, alias);
    return msec;
}

static void CG_SubtitlePrint(int msec, const snd_alias_t *pAlias)
{
    int minMsec;
    int subtitleWidth;

    if (msec && pAlias->pszSubtitle) {
        if (cgs->viewAspect > 1.3333334f) {
            subtitleWidth = cg_subtitleWidthWidescreen->current.integer;
        } else {
            subtitleWidth = cg_subtitleWidthStandard->current.integer;
        }

        minMsec = I_fround(cg_subtitleMinTime->current.value * 1000.0f);
        if (minMsec - msec >= 0)
            msec = minMsec;

        CL_SubtitlePrint(pAlias->pszSubtitle, msec, subtitleWidth);
    }
}

int CG_PlaySoundAlias(int entitynum, const vec_t *origin, snd_alias_list_t *aliasList)
{
    const snd_alias_t *alias;
    int msec;

    alias = Com_PickSoundAliasFromList(aliasList);
    if (!alias)
        return 0;

    msec = SND_PlaySoundAlias(alias, entitynum, origin, 0, SASYS_CGAME);
    CG_SubtitlePrint(msec, alias);
    return msec;
}

int CG_PlaySoundAliasByName(int entitynum, const vec_t *origin, const char *aliasname)
{
    const snd_alias_t *alias;
    int msec;

    alias = CL_PickSoundAlias(aliasname);
    if (!alias)
        return 0;

    msec = SND_PlaySoundAlias(alias, entitynum, origin, 0, SASYS_CGAME);
    CG_SubtitlePrint(msec, alias);
    return msec;
}

int CG_PlaySoundAliasAsMasterByName(int entitynum, const vec_t *origin, const char *aliasname)
{
    const snd_alias_t *alias;
    int msec;

    alias = CL_PickSoundAlias(aliasname);
    if (!alias)
        return 0;

    msec = SND_PlaySoundAliasAsMaster(alias, entitynum, origin, 0, SASYS_CGAME);
    CG_SubtitlePrint(msec, alias);
    return msec;
}

static Bool CG_ReplaceDirective(int *searchPos, int *dstLen, char *dstString)
{
    char keyBinding[0x100];
    char directive[0x100];
    char srcString[0x100];
    char *pFound;
    char *pEnd;
    int directiveLen;
    int bindingLen;
    int newStringLen;
    int beginLen;
    int endLen;

    memcpy(srcString, dstString, *dstLen);
    srcString[*dstLen] = '\0';

    pFound = (char *)strstr(srcString + *searchPos, (const char *)"[{");
    if (!pFound)
        return 0;

    pEnd = (char *)strstr(pFound, (const char *)"}]");
    if (!pEnd)
        return 0;

    directiveLen = (int)(pEnd - pFound) - 2;
    if (directiveLen == 0)
        return 0;

    Controls_GetConfig();

    memcpy(directive, pFound + 2, directiveLen);
    directive[directiveLen] = '\0';

    if (!GetKeyBindingLocalizedString(directive, keyBinding)) {

        I_strncpyz(keyBinding, UI_SafeTranslateString((const char *)"KEY_UNBOUND"), 0x100);
    }

    bindingLen = (int)strlen(keyBinding);

    newStringLen = *dstLen - directiveLen + bindingLen - 4;

    if (newStringLen + 1 > 0x100)
        return 0;

    beginLen = (int)(pFound - srcString);

    dstString += beginLen;
    memcpy(dstString, keyBinding, bindingLen);
    dstString += bindingLen;

    endLen = newStringLen - beginLen - bindingLen;
    memcpy(dstString, pEnd + 2, endLen);
    dstString[endLen] = '\0';

    *searchPos = bindingLen + beginLen;

    *dstLen = newStringLen;

    return 1;
}

static void CG_LocalizeHudElemString(const char *message, const char *messageType, char *hudElemString)
{
    const char *localizedString;
    int searchPos;
    int stringLen;

    localizedString = SEH_LocalizeTextMessage(message, messageType, (msgLocErrType_t)0);
    stringLen = (int)strlen(localizedString);

    if (stringLen + 1 > 0x100) {
        return;
    }

    memcpy(hudElemString, localizedString, stringLen);
    hudElemString[stringLen] = '\0';

    searchPos = 0;
    while (CG_ReplaceDirective(&searchPos, &stringLen, hudElemString)) {
    }
}

void CG_TranslateHudElemMessage(const char *message, const char *messageType, char *hudElemString)
{
    CG_LocalizeHudElemString(message, messageType, hudElemString);
}

void CG_SafeTranslateHudElemString(int index, char *hudElemString)
{
    if (index == 0) {
        return;
    }

    CG_LocalizeHudElemString(CL_GetConfigString(index + 0x51e), "hudelem string", hudElemString);
}

// oldest smoke grenade that went off in [minTime, gametime] and is still live
static entityState_t *CG_FindSmokeGrenadeEntityState(int minTime, int gametime)
{
    entityState_t *es;
    int i;
    int bestTime;
    int bestIndex;
    bool found;

    found = false;
    bestIndex = 0;
    bestTime = 0;
    for (i = 0; i < cg->nextSnap->numEntities; i++) {
        es = &cg->nextSnap->entities[i];
        if (!(es->eFlags & 0x10000))
            continue;
        if (es->time2 < minTime)
            continue;
        if (es->time < minTime || es->time > gametime)
            continue;
        if (!bestTime || bestTime > es->time) {
            found = true;
            bestIndex = i;
            bestTime = es->time;
        }
    }

    if (!found)
        return NULL;
    return &cg->nextSnap->entities[bestIndex];
}

void CG_PlaySmokeGrenadesAtTime(int gametime)
{
    entityState_t *smokeGrenadeES;
    int smokeTime;
    vec3_t up;

    if (!cgs->smokeGrenadeFx)
        return;

    Com_Printf("Playing smoke grenades at time %i\n", gametime);

    smokeGrenadeES = CG_FindSmokeGrenadeEntityState(gametime - (int)(FX_GetEffectLength(cgs->smokeGrenadeFx) + 1.0f), gametime);
    if (smokeGrenadeES) {
        FX_Rewind(smokeGrenadeES->time);
        FX_WarpTime(gametime + 120000);
        Com_Printf("Warping FX time to %i to kill off existing effects\n", gametime + 120000);

        do {
            smokeTime = smokeGrenadeES->time;

            Com_Printf("Warping FX time to %i\n", smokeTime);
            FX_WarpTime(smokeTime);

            Com_Printf("Playing smoke grenade at time %i\n", smokeTime);
            up[0] = 0.0f;
            up[1] = 0.0f;
            up[2] = 1.0f;
            FX_PlayEffect(cgs->smokeGrenadeFx, smokeGrenadeES->pos.trBase, up);

            smokeGrenadeES = CG_FindSmokeGrenadeEntityState(smokeTime + 1, gametime);
        } while (smokeGrenadeES);
    } else {
        FX_Rewind(gametime);
        FX_WarpTime(gametime + 120000);
    }

    Com_Printf("Done with smoke grenades, now warping FX time to current time %i\n", gametime);
    FX_WarpTime(gametime);
}

static void CG_LoadHudMenu(void)
{
    MenuList *menuList;
    menuDef_t *menu;

    menuList = UI_LoadMenus("ui_mp/hud.txt", 7);
    UI_AddMenuList(&cgDC, menuList);

    if (CL_GetLocalClientActiveCount() == 1) {
        menu = Menus_FindByName(&cgDC, "Compass");
    } else {
        menu = Menus_FindByName(&cgDC, "Compass_mp");
    }

    if (menu != NULL) {
        cgs_t *m = (cgs_t *)cgs;
        m->compassWidth = menu->window.rect[0].w;
        m->compassHeight = menu->window.rect[0].h;
        m->compassY = menu->window.rect[0].y;
    }
}

void CG_InitVote(void)
{
    cgs_t *m = (cgs_t *)cgs;
    m->voteTime = atoi(CL_GetConfigString(0xf));
    m->voteYes = atoi(CL_GetConfigString(0x11));
    m->voteNo = atoi(CL_GetConfigString(0x12));
    I_strncpyz(m->voteString, SEH_LocalizeTextMessage(CL_GetConfigString(0x10), "vote string", (msgLocErrType_t)0), sizeof(m->voteString));
}

static struct XModel *CG_GetXModel(const char *modelName)
{
    return CL_RegisterModel(modelName);
}

static void CG_CreateDObj(DObjModel_s *dobjModels, unsigned short numModels, struct XAnimTree_s *tree, int handle, clientInfo_t *ci)
{
    int weaponNum;
    weaponInfo_t *weapInfo;

    weaponNum = ci->iDObjWeapon;
    if (weaponNum) {
        weapInfo = &cg_weaponsArray[weaponNum];
        if (weapInfo->worldSurfModel) {
            dobjModels[numModels].model = weapInfo->worldSurfModel;
            dobjModels[numModels].boneName = ci->leftHandGun ? cg_weaponleftbone->current.string : cg_weaponrightbone->current.string;
            dobjModels[numModels].ignoreCollision = 0;
            ++numModels;
        }
    }

    Com_ClientDObjCreate(dobjModels, numModels, tree, handle);
}

static void CG_InitXAnimTrees(void)
{
    XAnim *anims;
    unsigned int i;

    anims = (XAnim *)cg->bgs.generic_human.tree.anims;
    for (i = 0; i < 64; i++)
        cg->bgs.clientinfo[i].pXAnimTree = XAnimCreateTree(anims, (Alloc_t)Hunk_AllocXAnimClient);
    for (i = 0; i < 8; i++)
        cgs->corpseinfo[i].pXAnimTree = XAnimCreateTree(anims, (Alloc_t)Hunk_AllocXAnimClient);
}

// retail 0x4ba930 (Kisak name)
void CG_InitEntities(void)
{
    int entityIndex;

    for (entityIndex = 0; entityIndex < 1024; entityIndex++)
        cg_entities[entityIndex].localClientNum = 0;
    ((cg_t *)cg)->predictedPlayerEntity.localClientNum = 0;
}

// retail 0x4ba960 (Kisak name); only inlined copies are called
void CG_InitViewDimensions(void)
{
    cgs->viewX = 0;
    CL_GetScreenDimensions((int *)&cgs->viewWidth, (int *)&cgs->viewHeight, (float *)&cgs->viewAspect);
}

void CG_Init(int serverMessageNum, int serverCommandSequence, int clientNum)
{
#    define cgBase ((byte *)cg)

#    define cgsBase ((byte *)cgs)
    char mapname[64];
    const char *game;

    memset((void *)cgs, 0, sizeof(cgs_t));

    memset((void *)cg, 0, sizeof(cg_t));
    memset(&cgDC, 0, sizeof(displayContextDef_t));
    memset((void *)cg_entities, 0, 0x89000);
    memset((void *)cg_weaponsArray, 0, CG_MAX_WEAPONS * sizeof(weaponInfo_t));
    memset((void *)cg_itemsArray, 0, CG_MAX_ITEMS * sizeof(itemInfo_t));

    CG_RegisterDvars();

    (*(void **)&((cg_t *)cgBase)->bgs.animScriptData.soundAlias) = (void *)Com_FindSoundAlias;
    (*(void **)&((cg_t *)cgBase)->bgs.animScriptData.playSoundAlias) = (void *)CG_PlayEntitySoundAlias;
    (*(void **)&((cg_t *)cgBase)->bgs.GetXModel) = (void *)CG_GetXModel;
    (*(void **)&((cg_t *)cgBase)->bgs.CreateDObj) = (void *)CG_CreateDObj;
    (*(void **)&((cg_t *)cgBase)->bgs.SafeDObjFree) = (void *)Com_SafeClientDObjFree;
    (*(void **)&((cg_t *)cgBase)->bgs.AllocXAnim) = (void *)Hunk_AllocXAnimClient;
    ((cg_t *)cgBase)->bgs.anim_user = 0;

    *(int *)(cgBase + 4) = clientNum;
    ((cg_t *)cgBase)->drawHud = 1;
    ((cg_t *)cgBase)->lastHealthLerpDelay = 1;
    ((cgs_t *)cgsBase)->processedSnapshotNum = serverMessageNum;
    ((cgs_t *)cgsBase)->serverCommandSequence = serverCommandSequence;

    CG_ParseServerinfo();
    CG_ParseCodinfo();
    UI_LoadIngameMenus();
    SCR_UpdateScreen();

    ((cgs_t *)cgsBase)->media.whiteMaterial = CL_RegisterMaterial("white", 7);
    ((cgs_t *)cgsBase)->media.softLineMaterial = CL_RegisterMaterial("hudsoftline", 7);
    ((cgs_t *)cgsBase)->media.softLineHMaterial = CL_RegisterMaterial("hudsoftlineh", 7);
    ((cgs_t *)cgsBase)->media.smallDevFont = CL_RegisterFont("fonts/smallDevFont", 1);
    ((cgs_t *)cgsBase)->media.bigDevFont = CL_RegisterFont("fonts/bigDevFont", 1);
    CL_RegisterMaterial("net_disconnect", 7);
    CL_RegisterMaterial("killicondied", 7);
    CL_RegisterMaterial("killiconcrush", 7);
    CL_RegisterMaterial("killiconfalling", 7);
    CL_RegisterMaterial("killiconsuicide", 7);
    CL_RegisterMaterial("killiconheadshot", 7);
    CL_RegisterMaterial("killiconmelee", 7);

    CG_AntiBurnInHUD_RegisterDvars();
    CG_InitConsoleCommands();

    CG_InitViewDimensions();

    game = CL_GetConfigString(2);
    if (strcmp(game, "cod"))
        Com_Error(ERR_DROP, "\x15"
                     "Client/Server game mismatch: %s/%s",
                  "cod", game);

    game = CL_GetConfigString(0xd);
    ((cgs_t *)cgsBase)->levelStartTime = atoi(game);
    SCR_UpdateScreen();

    if (!com_sv_running->current.enabled)
        Mantle_CreateAnims(Hunk_AllocXAnimClient);

    if (((int)bg_iNumWeapons) == 0) {
        CL_SetWeaponInfoMemory();
        BG_ClearWeaponDef();
        BG_FillInAmmoItems(0);
    }

    CG_SetupWeaponDef();
    CGScr_LoadAnimTrees();

    bgs = (bgs_t *)((char *)cgBase + offsetof(cg_t, bgs.animScriptData.animations[0].name[0]));
    BG_LoadAnim();
    CG_InitXAnimTrees();

    if (((cgs_t *)cgsBase)->localServer == 0)
        GScr_LoadConsts();

    CG_LoadingString("collision map");
    CL_CM_LoadMap(cgs->mapname);
    Menu_Setup(&cgDC);
    CG_LoadingString("graphics");

    memset(((char *)cgBase + offsetof(cg_t, refdef.x)), 0, 0x58);
    if (!g_mapLoaded) {
        CG_LoadingString(cgs->mapname);
        LoadWorld(cgs->mapname);
        g_mapLoaded = 1;
        CG_LoadingString("sound aliases");
        CL_LoadSoundAliases(cgs->mapname);
    }

    CG_LoadingString("game media");
    Com_StripExtension((const char *)&((cgs_t *)cgsBase)->mapname[5], mapname);
    CG_RegisterGraphics(mapname);
    CG_LoadingString("sounds");
    CG_RegisterSounds();
    CG_LoadingString("clients");
    CG_LoadHudMenu();

    CG_InitEntities();
    CG_InitLocalEntities();
    CG_InitMarkPolys();
    (*(unsigned char *)&((cg_t *)cgBase)->infoScreenText[0]) = 0;
    CG_SetConfigValues();
    CG_LoadingString("");
    CG_NorthDirectionChanged();
    CL_FinishLoadingModels();

    if (!g_mapLoaded)
        SND_StopSounds((snd_stopsounds_arg_t)0);

    CG_ParseFog();
    if (!g_ambientStarted) {
        CG_StartAmbient();
        g_ambientStarted = 1;
    }

    CL_SetADS(0);
    CG_InitVote();
    bgs = 0;
#    undef cgBase
#    undef cgsBase
}

void CG_FreeWeapons(void)
{
    weaponInfo_t *weapons = (weaponInfo_t *)cg_weaponsArray;
    int i;

    for (i = 1; i <= BG_GetNumWeapons(); ++i) {
        Com_SafeClientDObjFree(CG_WeaponDObjHandle(i));
        if (weapons[i].tree) {
            XAnimFreeTree(weapons[i].tree, 0);
            weapons[i].tree = NULL;
        }
    }

    memset((void *)cg_itemsArray, 0, CG_MAX_ITEMS * sizeof(itemInfo_t));
    memset((void *)cg_weaponsArray, 0, CG_MAX_WEAPONS * sizeof(weaponInfo_t));
}

// retail 0x4ba8c0
static void CG_FreeAnimTreeInstances(void)
{
    int i;

    for (i = 0; i < 64; ++i) {
        if (cg->bgs.clientinfo[i].pXAnimTree) {
            XAnimFreeTree(cg->bgs.clientinfo[i].pXAnimTree, 0);
            cg->bgs.clientinfo[i].pXAnimTree = NULL;
        }
    }
    for (i = 0; i < 8; ++i) {
        if (cgs->corpseinfo[i].pXAnimTree) {
            XAnimFreeTree(cgs->corpseinfo[i].pXAnimTree, 0);
            cgs->corpseinfo[i].pXAnimTree = NULL;
        }
    }
}

void CG_Shutdown(void)
{

    CL_TrackStatistics(0);
    SND_FadeAllSounds(1.0f, 0);
    g_ambientStarted = 0;
    g_mapLoaded = 0;
    Mantle_ShutdownAnims();
    CG_FreeWeapons();
    CG_FreeClientDObjInfo();
    CG_FreeEntityDObjInfo();
    CL_FreeWeaponInfoMemory();
    FX_FreeSystem();

    CG_FreeAnimTreeInstances();

    if (cgs->localServer == 0) {
        Scr_ShutdownGameStrings();
    }

    memset((void *)cg, 0, sizeof(cg_t));
}

void *Hunk_AllocXAnimPrecache(int size)
{
    return Hunk_AllocAlignInternal(size, 4);
}

void *Hunk_AllocXAnimClient(int size)
{
    return Hunk_AllocInternal(size);
}


