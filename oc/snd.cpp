#include "common_types.h"
#include "imports.h"

snd_local_t g_snd;

const dvar_t *snd_errorOnMissing;
const dvar_t *snd_touchStreamFilesOnLoad;
const dvar_t *snd_khz;
const dvar_t *snd_bits;
const dvar_t *snd_stereo;
static const dvar_t *snd_volume;
const dvar_t *snd_enableReverb;
extern struct snd_local_t g_snd;
static const dvar_t *snd_slaveFadeTime;
static const dvar_t *snd_enable2D;
static const dvar_t *snd_enable3D;
static const dvar_t *snd_enableStream;
/* migrated from the ILP32 data blob to typed C (x64 port Stage 2). */
static const char *snd_roomStrings[] = {
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


float Com_GetVolumeFalloffCurveValue(SndCurve *volumeFalloffCurve, float fraction);
int stricmp(const char *str1, const char *str2);
void Com_Printf(const char *fmt, ...);
void Com_DPrintf(const char *fmt, ...);
extern "C" void Z_FreeInternal( void *ptr );
void AxisCopy( const vec3_t in[3], vec3_t out[3] );
void Voice_Shutdown(void);
void Com_UnloadSoundAliases(snd_alias_system_t system);
void SND_ShutdownDriver(void);
void Cmd_RemoveCommand(const char *cmdName);
int Cmd_Argc(void);
const char *Cmd_Argv( int arg );
int atoi(const char *str);
double atof(const char *str);
dvar_t *Dvar_RegisterBool(const char *dvarName, bool value, unsigned short flags);
dvar_t *Dvar_RegisterInt(const char *dvarName, int value, int min, int max, unsigned short flags);
dvar_t *Dvar_RegisterFloat(const char *dvarName, float value, float min, float max, unsigned short flags);
Bool SND_InitDriver(void);
int Sys_Milliseconds(void);
void Cmd_AddCommand(const char *cmdName, void (*function)(void));
Bool Voice_Init(void);
void CG_GetEntityOrientation(int entnum, vec_t *origin, vec3_t *axis);

Bool SND_Is2DChannelFree(int index);
Bool SND_Is3DChannelFree(int index);
Bool SND_IsStreamChannelFree(int index);
void SND_Stop2DChannel(int index);
void SND_Stop3DChannel(int index);
void SND_StopStreamChannel(int index);
void SND_Pause2DChannel(int index);
void SND_Pause3DChannel(int index);
void SND_PauseStreamChannel(int index);
void SND_Unpause2DChannel(int index, int timeshift);
void SND_Unpause3DChannel(int index, int timeshift);
void SND_UnpauseStreamChannel(int index, int timeshift);
void SND_SetRoomtype(int roomtype);
void SND_Update2DChannelReverb(int index);
void SND_Update3DChannelReverb(int index);
void SND_UpdateStreamChannelReverb(int index);
float SND_Get2DChannelVolume(int index);
float SND_Get3DChannelVolume(int index);
float SND_GetStreamChannelVolume(int index);
int SND_Get2DChannelPlaybackRate(int index);
int SND_Get3DChannelPlaybackRate(int index);
int SND_GetStreamChannelPlaybackRate(int index);
const char *Com_GetSoundFileName(const snd_alias_t *alias);
float Vec3Distance(const vec3_t v1, const vec3_t v2);
float Vec3DistanceSq(const vec3_t v1, const vec3_t v2);
void SND_Set3DPosition(int index, const vec_t *org);
extern dvar_t *cl_paused;
extern const dvar_t *com_statmon;
int SND_GetDriverCPUPercentage(void);
void StatMon_Warning(int severity, int timeout, const char *msg);
void SND_DriverPreUpdate(int frametime);
void SND_DriverPostUpdate(void);
float Com_GetTimescaleForSnd(void);
void MemFile_InitForReading(MemoryFile *memFile, int size, void *buffer);
void MemFile_WriteData(MemoryFile *memFile, int bytes, const void *data);
void MemFile_ReadData(MemoryFile *memFile, int bytes, void *data);
void MemFile_WriteCString(MemoryFile *memFile, const char *str);
const char *MemFile_ReadCString(MemoryFile *memFile);

int SND_GetListenerIndexNearestToOrigin(const vec_t *origin);
void SND_SaveListeners(snd_listener *listeners);
void SND_RestoreListeners(snd_listener *listeners);
float SND_GetLerpedSlavePercentage(float baseSlavePercentage);
float SND_Attenuate(SndCurve *volumeFalloffCurve, float radius, float mindist, float maxdist);
Bool SND_IsAliasChannel3D(int channel);
static void SND_PauseSounds(void);
static void SND_UnpauseSounds(void);
void SND_SetChannelVolumes(int priority, const float *channelvolume, int fademsec);
void SND_DeactivateChannelVolumes(int priority, int fademsec);
void SND_UpdateLoopingSounds(void);
Bool SND_UpdateBackgroundVolume(int track, int frametime);
void SND_DeactivateEnvironmentEffects(int priority, int fademsec);
void SND_SetEnvironmentEffects(int priority, const char *roomstring, float drylevel, float wetlevel, int fademsec);
void SND_UpdateReverbs(void);
static int SND_RoomtypeFromString(const char *string);
void SND_SetEnvironmentEffects_f(void);
void SND_DeactivateEnvironmentEffects_f(void);
void SND_ErrorCleanup(void);
void SND_DisconnectListener(void);
void SND_SetListener(int entnum, const vec_t *origin, vec3_t *axis);
void SND_SetChannelInfo(int index, int entnum, const snd_alias_t *pAlias0, const snd_alias_t *pAlias1, float lerp, const vec_t *origin, float volume, float pitch, int srcChannelCount, int baserate, int total_msec, int start_msec, int startDelay, Bool master, snd_alias_system_t system);
void SND_GetCurrent3DPosition(int entnum, const vec_t *offset, vec_t *pos_out);
int SND_GetSoundOverlay(snd_overlay_type_t type, snd_overlay_info_t *info, int maxcount, int *cpu);
static int SND_FindFreeStreamChannel(void);
static __attribute_regparm__(3) void SND_StartBackground(int track, const snd_alias_t *pAlias, int fadetime, snd_alias_system_t system);
static __attribute_regparm__(3) void SND_StartBackground(int track, const snd_alias_t *pAlias, int fadetime, snd_alias_system_t system);
void SND_Init(void);
static Bool SND_AnyActiveListeners(void);
static __attribute_regparm__(3) void SND_StartBackground(int track, const snd_alias_t *pAlias, int fadetime, snd_alias_system_t system);
int SND_PlayLocalSoundAliasByName(const char *aliasname, snd_alias_system_t system);
int SND_PlayLocalSoundAlias(snd_alias_list_t *aliasList, snd_alias_system_t system);
int SND_PlayBlendedSoundAliases(const snd_alias_t *pAlias0, const snd_alias_t *pAlias1, float fLerp, int entnum, const vec_t *org, int timeshift, snd_alias_system_t system);
int SND_PlaySoundAliasAsMaster(const snd_alias_t *pAlias, int entnum, const vec_t *org, int timeshift, snd_alias_system_t system);
int SND_PlaySoundAlias(const snd_alias_t *pAlias, int entnum, const vec_t *org, int timeshift, snd_alias_system_t system);
int SND_FindFree2DChannel(int entnum, int entchannel);
int SND_FindFree3DChannel(int entnum, int entchannel);
static __attribute_regparm__(3) void SND_StartBackground(int track, const snd_alias_t *pAlias, int fadetime, snd_alias_system_t system);
static __attribute_regparm__(3) void SND_StartBackground(int track, const snd_alias_t *pAlias, int fadetime, snd_alias_system_t system);
void SND_PlayAmbientAlias(const snd_alias_t *pAlias, int fadetime, snd_alias_system_t system);
void SND_PlayMusicAlias(const snd_alias_t *pAlias, snd_alias_system_t system);
void SND_StopMusic(int fadetime);
void SND_StopSounds(snd_stopsounds_arg_t which);
void SND_ShutdownChannels(void);
void SND_Shutdown(void);
void SND_FadeAllSounds(float volume, int fadetime);
const snd_alias_t *SND_RestoreSoundAlias(MemoryFile *memFile);
void SND_Save(MemoryFile *memFile);
void SND_Restore(MemoryFile *memFile);
void SND_Update(void);

int SND_GetAliasOffset(const snd_alias_t *alias);
snd_alias_t *SND_GetAliasWithOffset(const char *name, int offset);
int SND_Get3DChannelLength(int index);
int SND_Get2DChannelLength(int index);
int SND_GetStreamChannelLength(int index);
void SND_Get3DChannelSaveInfo(int index, snd_save_3D_sample_t *info);
void SND_Get2DChannelSaveInfo(int index, snd_save_2D_sample_t *info);
void SND_GetStreamChannelSaveInfo(int index, snd_save_stream_t *info);
void SND_Set2DChannelFromSaveInfo(int index, snd_save_2D_sample_t *info);
void SND_SetStreamChannelFromSaveInfo(int index, snd_save_stream_t *info);
int SND_StartAlias2DSample(const snd_alias_t *pAlias0, const snd_alias_t *pAlias1, float lerp, int entnum, float volume, float pitch, int timeshift, float fraction, int startDelay, Bool master, int *pChannel, snd_alias_system_t system);
int SND_StartAlias3DSample(const snd_alias_t *pAlias0, const snd_alias_t *pAlias1, float lerp, int entnum, const vec_t *org, float volume, float pitch, int timeshift, float fraction, int startDelay, Bool master, int *pChannel, snd_alias_system_t system);
int SND_StartAliasStreamOnChannel(const snd_alias_t *pAlias0, const snd_alias_t *pAlias1, float lerp, int entnum, const vec_t *org, float volume, float pitch, int timeshift, float fraction, int startDelay, Bool master, int index, snd_alias_system_t system);
void *Com_GetSoundFileMem(const snd_alias_t *alias);
float randomf(void);
void Dvar_ClearModified(dvar_t* var);
void SND_Update3DChannel(int index, int frametime);
void SND_UpdateStreamChannel(int index, int frametime);
void SND_Update2DChannel(int index, int frametime);
void SND_Set3DChannelPlaybackRate(int index, int rate);
void SND_SetStreamChannelPlaybackRate(int index, int rate);
void SND_Set2DChannelPlaybackRate(int index, int rate);
float floorf(float x);
void Com_Error(errorParm_t level, const char *fmt, ...);
snd_alias_t *Com_PickSoundAlias( const char *aliasName );
snd_alias_t *Com_PickSoundAliasFromList( snd_alias_list_t *aliasList );
int SND_GetListenerIndexNearestToOrigin(const vec_t *origin);
void SND_SaveListeners(snd_listener *listeners);
void SND_RestoreListeners(snd_listener *listeners);
float SND_GetLerpedSlavePercentage(float baseSlavePercentage);
float SND_Attenuate(SndCurve *volumeFalloffCurve, float radius, float mindist, float maxdist);
Bool SND_IsAliasChannel3D(int channel);
static Bool SND_AnyActiveListeners(void);
static __attribute_regparm__(3) Bool SND_ValidateSoundAliasBlend(const snd_alias_t *pAlias0, const snd_alias_t *pAlias1, Bool bReport);
static void SND_PauseSounds(void);
static void SND_UnpauseSounds(void);
void SND_SetChannelVolumes(int priority, const float *channelvolume, int fademsec);
void SND_DeactivateChannelVolumes(int priority, int fademsec);
void SND_UpdateLoopingSounds(void);
Bool SND_UpdateBackgroundVolume(int track, int frametime);
void SND_DeactivateEnvironmentEffects(int priority, int fademsec);
void SND_SetEnvironmentEffects(int priority, const char *roomstring, float drylevel, float wetlevel, int fademsec);
static int SND_RoomtypeFromString(const char *string);
void SND_UpdateReverbs(void);
void SND_ErrorCleanup(void);
void SND_DisconnectListener(void);
void SND_SetListener(int entnum, const vec_t *origin, vec3_t *axis);
void SND_SetChannelInfo(int index, int entnum, const snd_alias_t *pAlias0, const snd_alias_t *pAlias1, float lerp, const vec_t *origin, float volume, float pitch, int srcChannelCount, int baserate, int total_msec, int start_msec, int startDelay, Bool master, snd_alias_system_t system);
void SND_GetCurrent3DPosition(int entnum, const vec_t *offset, vec_t *pos_out);
int SND_GetSoundOverlay(snd_overlay_type_t type, snd_overlay_info_t *info, int maxcount, int *cpu);
static __attribute_regparm__(3) int SND_StartAliasStream(const snd_alias_t *pAlias0, const snd_alias_t *pAlias1, float lerp, int entnum, const vec_t *org, float volume, float pitch, int timeshift, float fraction, Bool treatAsMaster, int *pChannel, snd_alias_system_t system);
static __attribute_regparm__(2) void SND_SaveChanInfo(snd_channel_info_t *chaninfo, MemoryFile *memFile);
void SND_Init(void);
static __attribute_regparm__(3) int SND_PlaySoundAlias_Internal(const snd_alias_t *pAlias0, const snd_alias_t *pAlias1, float lerp, int entnum, const vec_t *org, int *pChannel, int timeshift, Bool treatAsMaster, snd_alias_system_t system);
int SND_PlayLocalSoundAliasByName(const char *aliasname, snd_alias_system_t system);
int SND_PlayLocalSoundAlias(snd_alias_list_t *aliasList, snd_alias_system_t system);
int SND_PlayBlendedSoundAliases(const snd_alias_t *pAlias0, const snd_alias_t *pAlias1, float fLerp, int entnum, const vec_t *org, int timeshift, snd_alias_system_t system);
int SND_PlaySoundAliasAsMaster(const snd_alias_t *pAlias, int entnum, const vec_t *org, int timeshift, snd_alias_system_t system);
int SND_PlaySoundAlias(const snd_alias_t *pAlias, int entnum, const vec_t *org, int timeshift, snd_alias_system_t system);
int SND_FindFree2DChannel(int entnum, int entchannel);
int SND_FindFree3DChannel(int entnum, int entchannel);
static __attribute_regparm__(2) void SND_RestoreChanInfo(snd_channel_info_t *chaninfo, MemoryFile *memFile);
static __attribute_regparm__(3) void SND_StartBackground(int track, const snd_alias_t *pAlias, int fadetime, snd_alias_system_t system);
void SND_PlayAmbientAlias(const snd_alias_t *pAlias, int fadetime, snd_alias_system_t system);
void SND_PlayMusicAlias(const snd_alias_t *pAlias, snd_alias_system_t system);
void SND_StopMusic(int fadetime);
void SND_StopSounds(snd_stopsounds_arg_t which);
void SND_ShutdownChannels(void);
void SND_Shutdown(void);
void SND_FadeAllSounds(float volume, int fadetime);
void SND_SetEnvironmentEffects_f(void);
void SND_DeactivateEnvironmentEffects_f(void);
void SND_Save(MemoryFile *memFile);
static __attribute_regparm__(2) Bool SND_RestoreStreamChannel(int channel, MemoryFile *memFile);
void SND_Restore(MemoryFile *memFile);
void SND_Update(void);

static __attribute_regparm__(3)
    Bool SND_ValidateSoundAliasBlend(const snd_alias_t *pAlias0, const snd_alias_t *pAlias1, Bool bReport)
{
    int flags0, flags1;
    int isLooping;
    if (pAlias0 == pAlias1)
        return 1;
    if (Com_GetSoundFileName(pAlias0) != Com_GetSoundFileName(pAlias1)) {
        if (!bReport)
            return 0;
        Com_Error(ERR_DROP, "\025tried to blend between sound aliases '%s' and '%s', but they don't use the same sound file\n", pAlias0->pszAliasName, pAlias1->pszAliasName);
        return 0;
    }
    if (pAlias0->volumeFalloffCurve != pAlias1->volumeFalloffCurve) {
        if (!bReport)
            return 0;
        Com_Error(ERR_DROP, "\025tried to blend between sound aliases '%s' and '%s', but they do not have the same volume falloff curve\n", pAlias0->pszAliasName, pAlias1->pszAliasName);
        return 0;
    }
    flags0 = pAlias0->flags;
    flags1 = pAlias1->flags;
    isLooping = flags0 & 1;
    if (isLooping != (flags1 & 1)) {
        if (!bReport)
            return 0;
        Com_Error(ERR_DROP, "\025tried to blend between sound aliases '%s' and '%s', but they do not have the same looping status\n", pAlias0->pszAliasName, pAlias1->pszAliasName);
        return 0;
    }
    if ((pAlias0->flags & 0x60) >> 5 != (pAlias1->flags & 0x60) >> 5) {
        if (!bReport)
            return 0;
        Com_Error(ERR_DROP, "\025tried to blend between sound aliases '%s' and '%s', but they are not both loaded or both streamed\n", pAlias0->pszAliasName, pAlias1->pszAliasName);
        return 0;
    }
    if ((flags0 & 0x780) != (flags1 & 0x780)) {
        if (!bReport)
            return 0;
        Com_Error(ERR_DROP, "\025tried to blend between sound aliases '%s' and '%s', but they don't use the same channel\n", pAlias0->pszAliasName, pAlias1->pszAliasName);
        return 0;
    }
    if ((flags0 & 2) != (flags1 & 2)) {
        if (!bReport)
            return 0;
        Com_Error(ERR_DROP, "\025tried to blend between sound aliases '%s' and '%s', but only one of them is a 'master' alias\n", pAlias0->pszAliasName, pAlias1->pszAliasName);
        return 0;
    }
    if ((flags0 & 4) != (flags1 & 4)) {
        if (!bReport)
            return 0;
        Com_Error(ERR_DROP, "\025tried to blend between sound aliases '%s' and '%s', but only one of them is a 'slave' alias\n", pAlias0->pszAliasName, pAlias1->pszAliasName);
        return 0;
    }
    if ((flags0 & 8) != (flags1 & 8)) {
        if (!bReport)
            return 0;
        Com_Error(ERR_DROP, "\025tried to blend between sound aliases '%s' and '%s', but they don't use the same fulldrylevel setting\n", pAlias0->pszAliasName, pAlias1->pszAliasName);
        return 0;
    }
    if ((flags0 & 0x10) != (flags1 & 0x10)) {
        if (!bReport)
            return 0;
        Com_Error(ERR_DROP, "\025tried to blend between sound aliases '%s' and '%s', but they don't use the same nowetlevel setting\n", pAlias0->pszAliasName, pAlias1->pszAliasName);
        return 0;
    }
    if (pAlias0->fSlavePercentage != pAlias1->fSlavePercentage) {
        if (!bReport)
            return 0;
        Com_Error(ERR_DROP, "\025tried to blend between sound aliases '%s' and '%s', but they do not have the same slave percentages.\n", pAlias0->pszAliasName, pAlias1->pszAliasName);
        return 0;
    }
    if (pAlias0->startDelay != pAlias1->startDelay) {
        if (!bReport)
            return 0;
        Com_Error(ERR_DROP, "\025tried to blend between sound aliases '%s' and '%s', but they do not have the same start delay.\n", pAlias0->pszAliasName, pAlias1->pszAliasName);
        return 0;
    }
    if (isLooping) {
        if (pAlias0->fPitchMin != pAlias0->fPitchMax || pAlias1->fPitchMin != pAlias1->fPitchMax) {
            if (!bReport)
                return 0;
            Com_Error(ERR_DROP, "\025tried to blend between sound aliases '%s' and '%s', but they are looping and at least one of them has a random pitch\n", pAlias0->pszAliasName, pAlias1->pszAliasName);
            return 0;
        }
    }
    if (flags0 & 1) {
        if (pAlias0->fVolMin != pAlias0->fVolMax || pAlias1->fVolMin != pAlias1->fVolMax) {
            if (!bReport)
                return 0;
            Com_Error(ERR_DROP, "\025tried to blend between sound aliases '%s' and '%s', but they are looping and at least one of them has a random volume\n", pAlias0->pszAliasName, pAlias1->pszAliasName);
            return 0;
        }
    }
    if (pAlias0->pszSecondaryAliasName || pAlias1->pszSecondaryAliasName) {
        if (!bReport)
            return 0;
        Com_Error(ERR_DROP, "\025tried to blend between sound aliases '%s' and '%s', but one or both has a secondary alias.\n", pAlias0->pszAliasName, pAlias1->pszAliasName);
        return 0;
    }
    return 1;
}

COD2_ASSERT_SIZE(snd_save_stream_t, 0x20);
COD2_ASSERT_FIELD(snd_save_stream_t, fraction, 0x0);
COD2_ASSERT_FIELD(snd_save_stream_t, rate, 0x4);
COD2_ASSERT_FIELD(snd_save_stream_t, basevolume, 0x8);
COD2_ASSERT_FIELD(snd_save_stream_t, volume, 0xc);
COD2_ASSERT_FIELD(snd_save_stream_t, pan, 0x10);
COD2_ASSERT_FIELD(snd_save_stream_t, org, 0x14);

extern "C" void *Z_MallocInternal( int size );
extern "C" void Com_Memcpy( void *dest, const void *src, int count );

inline void MemFile_WriteByte(MemoryFile *memFile, unsigned char value)
{
    MemFile_WriteData(memFile, 1, &value);
}

inline void MemFile_WriteShort(MemoryFile *memFile, short value)
{
    MemFile_WriteData(memFile, 2, &value);
}

inline void MemFile_WriteInt(MemoryFile *memFile, int value)
{
    MemFile_WriteData(memFile, 4, &value);
}

inline void MemFile_WriteFloat(MemoryFile *memFile, float value)
{
    MemFile_WriteData(memFile, 4, &value);
}

inline void MemFile_WriteVec3(MemoryFile *memFile, const float *value)
{
    MemFile_WriteData(memFile, 12, value);
}

inline unsigned char MemFile_ReadByte(MemoryFile *memFile)
{
    unsigned char value;

    MemFile_ReadData(memFile, 1, &value);
    return value;
}

inline short MemFile_ReadShort(MemoryFile *memFile)
{
    short value;

    MemFile_ReadData(memFile, 2, &value);
    return value;
}

inline int MemFile_ReadInt(MemoryFile *memFile)
{
    int value;

    MemFile_ReadData(memFile, 4, &value);
    return value;
}

inline float MemFile_ReadFloat(MemoryFile *memFile)
{
    float value;

    MemFile_ReadData(memFile, 4, &value);
    return value;
}

inline void MemFile_ReadVec3(MemoryFile *memFile, float *value)
{
    value[0] = MemFile_ReadFloat(memFile);
    value[1] = MemFile_ReadFloat(memFile);
    value[2] = MemFile_ReadFloat(memFile);
}

static bool SND_ValidateEnvEffectsPriorityValue(const char *priorityName, int *priority)
{
    const char *priorityStrings[3];
    int stringIndex;

    priorityStrings[0] = "none";
    priorityStrings[1] = "level";
    priorityStrings[2] = "shellshock";

    for (stringIndex = 1; stringIndex < 3; ++stringIndex) {
        if (!stricmp(priorityName, priorityStrings[stringIndex])) {
            *priority = stringIndex;
            return true;
        }
    }

    Com_Printf("invalid priority string '%s', it must be one of the following strings:\n", priorityName);
    for (stringIndex = 1; stringIndex < 3; ++stringIndex)
        Com_Printf("  %s\n", priorityStrings[stringIndex]);
    return false;
}

static int SND_RoomtypeFromString(const char *string)
{
    int stringIndex;

    for (stringIndex = 0; snd_roomStrings[stringIndex]; ++stringIndex) {
        if (!stricmp(string, snd_roomStrings[stringIndex]))
            return stringIndex;
    }

    Com_Printf("invalid roomtype string '%s', it must be one of the following strings:\n", string);
    for (stringIndex = 0; snd_roomStrings[stringIndex]; ++stringIndex) {
        if (*snd_roomStrings[stringIndex])
            Com_Printf("  %s\n", snd_roomStrings[stringIndex]);
    }
    return 0;
}

void SND_SetEnvironmentEffects_f(void)
{
    const char *roomstring;
    float drylevel;
    float wetlevel;
    int priority;
    int fademsec;

    if (Cmd_Argc() != 6) {
        Com_Printf("USAGE: snd_setEnvironmentEffects <const char *priority> <const char *roomstring> <float drylevel> <float wetlevel> <int fademsec>\n");
        return;
    }
    if (!SND_ValidateEnvEffectsPriorityValue(Cmd_Argv(1), &priority))
        return;

    roomstring = Cmd_Argv(2);
    if (!SND_RoomtypeFromString(roomstring) && stricmp(roomstring, snd_roomStrings[0]))
        return;

    drylevel = atof(Cmd_Argv(3));
    if (drylevel < 0.0f || drylevel > 1.0f) {
        Com_Printf("invalid 'drylevel' %g, must be in the range of 0.0-1.0\n", drylevel);
        return;
    }
    wetlevel = atof(Cmd_Argv(4));
    if (wetlevel < 0.0f || wetlevel > 1.0f) {
        Com_Printf("invalid 'wetlevel' %g, must be in the range of 0.0-1.0\n", wetlevel);
        return;
    }
    fademsec = atoi(Cmd_Argv(5));
    if (fademsec < 0) {
        Com_Printf("invalid 'fademsec' %i, must be greater than or equal to zero\n", fademsec);
        return;
    }
    SND_SetEnvironmentEffects(priority, roomstring, drylevel, wetlevel, fademsec);
}

void SND_DeactivateEnvironmentEffects_f(void)
{
    int priority;
    int fademsec;

    if (Cmd_Argc() != 3) {
        Com_Printf("USAGE: snd_deactivateEnvironmentEffects <int priority> <int fademsec>\n");
        return;
    }
    if (!SND_ValidateEnvEffectsPriorityValue(Cmd_Argv(1), &priority))
        return;

    fademsec = atoi(Cmd_Argv(2));
    if (fademsec < 0) {
        Com_Printf("invalid 'fademsec' %i, must be greater than or equal to zero\n", fademsec);
        return;
    }
    SND_DeactivateEnvironmentEffects(priority, fademsec);
}

static __declspec(noinline) Bool SND_AnyActiveListeners(void)
{
    int i;

    for (i = 0; i < 1; i++) {
        if (g_snd.listeners[i].active)
            return 1;
    }
    return 0;
}

void SND_DisconnectListener(void)
{
    memset(&g_snd.listeners[0], 0, sizeof(snd_listener));
}

void SND_SetListener(int entnum, const vec_t *origin, vec3_t *axis)
{
    if (!g_snd.Initialized2d) {
        return;
    }
    AxisCopy(axis, g_snd.listeners[0].orient.axis);
    g_snd.listeners[0].orient.origin[0] = origin[0];
    g_snd.listeners[0].orient.origin[1] = origin[1];
    g_snd.listeners[0].orient.origin[2] = origin[2];
    g_snd.listeners[0].entnum = entnum;
    g_snd.listeners[0].active = 1;
}

void SND_SaveListeners(snd_listener *listeners)
{
    *listeners = g_snd.listeners[0];
}

void SND_RestoreListeners(snd_listener *listeners)
{
    g_snd.listeners[0] = *listeners;
}

float SND_GetLerpedSlavePercentage(float baseSlavePercentage)
{
    return 1.0f - (1.0f - baseSlavePercentage) * g_snd.slaveLerp;
}

float SND_Attenuate(SndCurve *volumeFalloffCurve, float radius, float mindist, float maxdist)
{
    float dist = radius - mindist;
    if (dist <= 0.0f) {
        return 1.0f;
    }
    if (maxdist == mindist) {
        return 0.0f;
    }
    dist = dist / (maxdist - mindist);
    if (dist >= 1.0f) {
        return 0.0f;
    }
    return Com_GetVolumeFalloffCurveValue(volumeFalloffCurve, dist);
}

void SND_GetCurrent3DPosition(int entnum, const vec_t *offset, vec_t *pos_out)
{
    vec3_t org;
    vec3_t axis[3];
    float scale;

    CG_GetEntityOrientation(entnum, org, axis);

    scale = offset[0];
    org[0] += scale * axis[0][0];
    org[1] += scale * axis[0][1];
    org[2] += scale * axis[0][2];

    scale = offset[1];
    org[0] += scale * axis[1][0];
    org[1] += scale * axis[1][1];
    org[2] += scale * axis[1][2];

    scale = offset[2];
    org[0] += scale * axis[2][0];
    org[1] += scale * axis[2][1];
    org[2] += scale * axis[2][2];

    pos_out[0] = org[0];
    pos_out[1] = org[1];
    pos_out[2] = org[2];
}

Bool SND_IsAliasChannel3D(int channel)
{
    switch (channel) {
    case 1:
    case 2:
    case 7:
    case 8:
    case 9:
    case 10:
        return 0;
    default:
        return 1;
    }
}

void SND_SetChannelInfo(int index, int entnum, const snd_alias_t *pAlias0, const snd_alias_t *pAlias1, float lerp, const vec_t *origin, float volume, float pitch, int srcChannelCount, int baserate, int total_msec, int start_msec, int startDelay, Bool master, snd_alias_system_t system)
{
    vec3_t axis[3];
    vec3_t org;
    vec3_t delta;

    if (entnum >= 0 && entnum < 1024 && SND_IsAliasChannel3D((pAlias0->flags & 0x780) >> 7)) {
        CG_GetEntityOrientation(entnum, org, axis);
        delta[0] = origin[0]; delta[0] = delta[0] - org[0];
        delta[1] = origin[1]; delta[1] = delta[1] - org[1];
        delta[2] = origin[2] - org[2];
        g_snd.chaninfo[index].offset[0] = delta[0] * axis[0][0] + delta[1] * axis[0][1] + delta[2] * axis[0][2];
        g_snd.chaninfo[index].offset[1] = delta[0] * axis[1][0] + delta[1] * axis[1][1] + delta[2] * axis[1][2];
        g_snd.chaninfo[index].offset[2] = delta[0] * axis[2][0] + delta[1] * axis[2][1] + delta[2] * axis[2][2];
    } else {
        g_snd.chaninfo[index].offset[0] = 0.0f;
        g_snd.chaninfo[index].offset[1] = 0.0f;
        g_snd.chaninfo[index].offset[2] = 0.0f;
    }
    g_snd.chaninfo[index].entnum = entnum;
    g_snd.chaninfo[index].entchannel = (pAlias0->flags & 0x780) >> 7;
    g_snd.chaninfo[index].basevolume = volume;
    g_snd.chaninfo[index].pitch = pitch;
    g_snd.chaninfo[index].srcChannelCount = srcChannelCount;
    g_snd.chaninfo[index].baserate = baserate;
    g_snd.chaninfo[index].pAlias1 = pAlias1;
    g_snd.chaninfo[index].lerp = lerp;
    g_snd.chaninfo[index].startDelay = startDelay;
    g_snd.chaninfo[index].pAlias0 = pAlias0;
    g_snd.chaninfo[index].looptime = g_snd.looptime;
    g_snd.chaninfo[index].endtime = g_snd.time - start_msec + total_msec;
    g_snd.chaninfo[index].paused = (g_snd.paused && g_snd.pauseSettings[(pAlias0->flags & 0x780) >> 7]) ? 1 : 0;
    g_snd.chaninfo[index].master = master;
    g_snd.chaninfo[index].system = system;
}

// retail 0x44ab00, unreferenced (inlined in SND_FindReplaceableChannel); name unknown
int SND_CompareChannels_44ab00(int i, int best, int entnum)
{
    int diff;

    if (g_snd.chaninfo[i].entnum != g_snd.chaninfo[best].entnum) {
        if (g_snd.chaninfo[i].entnum == entnum)
            return -1;
        if (g_snd.chaninfo[best].entnum == entnum)
            return 1;
    }
    diff = g_snd.chaninfo[i].entchannel - g_snd.chaninfo[best].entchannel;
    if (!diff)
        diff = g_snd.chaninfo[i].endtime - g_snd.chaninfo[best].endtime;
    return diff;
}

// retail 0x44ab50, unreferenced; name unknown
Bool SND_ChannelEntchannelAtMost_44ab50(int index, int entchannel)
{
    return g_snd.chaninfo[index].entchannel <= entchannel;
}

// Original name unknown (KisakCOD: SND_FindReplaceableChannel).
static int SND_FindReplaceableChannel(int entnum, int entchannel, int first, int count)
{
    int diff;
    int best;
    int i;

    best = -1;

    for (i = first; i < first + count; i++) {
        if (!SND_ChannelEntchannelAtMost_44ab50(i, entchannel))
            continue;

        if (best >= 0) {
            diff = SND_CompareChannels_44ab00(i, best, entnum);
            if (!diff || diff >= 0)
                continue;
        }

        best = i;
    }

    return best;
}

int SND_FindFree2DChannel(int entnum, int entchannel)
{
    int i;
    int best;

    for (i = 0; i < g_snd.max_2D_channels; i++) {
        if (SND_Is2DChannelFree(i + 45))
            return i + 45;
    }

    best = SND_FindReplaceableChannel(entnum, entchannel, 45, g_snd.max_2D_channels);

    if (best >= 0)
        SND_Stop2DChannel(best);

    return best;
}

int SND_FindFree3DChannel(int entnum, int entchannel)
{
    int i;
    int best;

    for (i = 0; i < g_snd.max_3D_channels; i++) {
        if (SND_Is3DChannelFree(i))
            return i;
    }

    best = SND_FindReplaceableChannel(entnum, entchannel, 0, g_snd.max_3D_channels);

    if (best >= 0)
        SND_Stop3DChannel(best);

    return best;
}

// Original name unknown (KisakCOD: SND_FindFreeStreamChannel).
static int SND_FindFreeStreamChannel(void)
{
    int i;

    for (i = 3; i < g_snd.max_stream_channels; i++) {
        if (SND_IsStreamChannelFree(i + 32))
            return i + 32;
    }

    return -1;
}

static void SND_StopEntityChannel(int entnum, int entchannel)
{
    int i;

    for (i = 0; i < g_snd.max_3D_channels; i++) {
        if (g_snd.chaninfo[i].entnum == entnum && g_snd.chaninfo[i].entchannel == entchannel && !SND_Is3DChannelFree(i))
            SND_Stop3DChannel(i);
    }
    for (i = 32; i < 32 + g_snd.max_stream_channels; i++) {
        if (g_snd.chaninfo[i].entnum == entnum && g_snd.chaninfo[i].entchannel == entchannel && !SND_IsStreamChannelFree(i))
            SND_StopStreamChannel(i);
    }
    for (i = 45; i < 45 + g_snd.max_2D_channels; i++) {
        if (g_snd.chaninfo[i].entnum == entnum && g_snd.chaninfo[i].entchannel == entchannel && !SND_Is2DChannelFree(i))
            SND_Stop2DChannel(i);
    }
}

static int SND_StartAliasSample(const snd_alias_t *pAlias0, const snd_alias_t *pAlias1, float lerp, int entnum, const vec_t *org, float volume, float pitch, int timeshift, Bool treatAsMaster, int *pChannel, snd_alias_system_t system)
{
    int flags;

    if (!Com_GetSoundFileMem(pAlias0)) {
        Com_DPrintf("Tried to play sound '%s' from alias '%s', but it was not successfully loaded.\n",
                    Com_GetSoundFileName(pAlias0), pAlias0->pszAliasName);
        if (pChannel)
            *pChannel = -1;
        return 0;
    }

    flags = pAlias0->flags;
    if (SND_IsAliasChannel3D((flags & 0x780) >> 7)) {
        if (!snd_enable3D->current.enabled)
            return 0;
        if (!SND_AnyActiveListeners()) {
            Com_Error(ERR_DROP, "attempted to play spatialized alias '%s' while there is no active listener. Most likely this means you tried to play a spatialized sound while not in a level.\n", pAlias0->pszAliasName);
            return 0;
        }
        return SND_StartAlias3DSample(pAlias0, pAlias1, lerp, entnum, org, volume, pitch, timeshift, 0.0f, pAlias0->startDelay, treatAsMaster || (flags & 2), pChannel, system);
    } else {
        if (!snd_enable2D->current.enabled)
            return 0;
        return SND_StartAlias2DSample(pAlias0, pAlias1, lerp, entnum, volume, pitch, timeshift, 0.0f, pAlias0->startDelay, treatAsMaster || (flags & 2), pChannel, system);
    }
}

static __attribute_regparm__(3) int SND_StartAliasStream(const snd_alias_t *pAlias0, const snd_alias_t *pAlias1, float lerp, int entnum, const vec_t *org, float volume, float pitch, int timeshift, float fraction, Bool treatAsMaster, int *pChannel, snd_alias_system_t system)
{
    int entchannel;
    int channel;
    vec3_t listenerOrg;
    float distNew;
    float distOld;

    entchannel = (pAlias0->flags & 0x780) >> 7;

    channel = SND_FindFreeStreamChannel();
    if (channel < 0) {
        channel = SND_FindReplaceableChannel(entnum, entchannel, 35, g_snd.max_stream_channels - 3);
        if (channel >= 0) {
            if (g_snd.chaninfo[channel].pAlias0 != pAlias0) {
                SND_StopStreamChannel(channel);
            } else {
                listenerOrg[0] = g_snd.listeners[0].orient.origin[0];
                listenerOrg[1] = g_snd.listeners[0].orient.origin[1];
                listenerOrg[2] = g_snd.listeners[0].orient.origin[2];
                distNew = Vec3DistanceSq(org, listenerOrg);
                if (distNew < Vec3DistanceSq(g_snd.chaninfo[channel].org, listenerOrg))
                    SND_StopStreamChannel(channel);
                else
                    channel = -1;
            }
        }
    }

    if (pChannel)
        *pChannel = channel;

    if (channel < 0)
        return 0;

    if (!*(unsigned char *)((char *)snd_enableStream + 8))
        return 0;

    switch ((pAlias0->flags & 0x780) >> 7) {
    case 1:
    case 2:
    case 7:
    case 8:
    case 9:
    case 10:
        break;
    default:
        if (!SND_AnyActiveListeners())
            Com_Error(ERR_DROP, "attempted to play spatialized alias '%s' while there is no active listener. Most likely this means you tried to play a spatialized sound while not in a level.\n", pAlias0->pszAliasName);
        break;
    }

    return SND_StartAliasStreamOnChannel(pAlias0, pAlias1, lerp, entnum, org, volume, pitch, timeshift, fraction, pAlias0->startDelay, treatAsMaster || (pAlias0->flags & 2), channel, system);
}

static Bool SND_ContinueLoopingSound(const snd_alias_t *pAlias0, const snd_alias_t *pAlias1, float lerp, int entnum, const vec_t *org, int *pChannel)
{
    int i;
    int rate;

    for (i = 0; i < g_snd.max_3D_channels; i++) {
        if (g_snd.chaninfo[i].entnum != entnum)
            continue;
        if (SND_Is3DChannelFree(i))
            continue;
        if (!(g_snd.chaninfo[i].pAlias0->flags & 1))
            continue;
        if (g_snd.chaninfo[i].pAlias0->pszAliasName != pAlias0->pszAliasName)
            continue;
        if (g_snd.chaninfo[i].pAlias1->pszAliasName != pAlias1->pszAliasName)
            continue;

        g_snd.chaninfo[i].basevolume = (1.0f - lerp) * pAlias0->fVolMin + lerp * pAlias1->fVolMin;
        rate = I_fround((float)g_snd.chaninfo[i].baserate * g_snd.chaninfo[i].pitch * g_snd.timescale);
        SND_Set3DChannelPlaybackRate(i, rate);
        SND_Set3DPosition(i, org);
        g_snd.chaninfo[i].looptime = g_snd.looptime;
        g_snd.chaninfo[i].lerp = lerp;
        if (pChannel)
            *pChannel = i;
        return 1;
    }

    for (i = 45; i < 45 + g_snd.max_2D_channels; i++) {
        if (g_snd.chaninfo[i].entnum != entnum)
            continue;
        if (SND_Is2DChannelFree(i))
            continue;
        if (!(g_snd.chaninfo[i].pAlias0->flags & 1))
            continue;
        if (g_snd.chaninfo[i].pAlias0->pszAliasName != pAlias0->pszAliasName)
            continue;
        if (g_snd.chaninfo[i].pAlias1->pszAliasName != pAlias1->pszAliasName)
            continue;

        g_snd.chaninfo[i].basevolume = (1.0f - lerp) * pAlias0->fVolMin + lerp * pAlias1->fVolMin;
        rate = I_fround((float)g_snd.chaninfo[i].baserate * g_snd.chaninfo[i].pitch * g_snd.timescale);
        SND_Set2DChannelPlaybackRate(i, rate);
        g_snd.chaninfo[i].looptime = g_snd.looptime;
        g_snd.chaninfo[i].lerp = lerp;
        if (pChannel)
            *pChannel = i;
        return 1;
    }

    for (i = 32; i < 32 + g_snd.max_stream_channels; i++) {
        if (g_snd.chaninfo[i].entnum != entnum)
            continue;
        if (SND_IsStreamChannelFree(i))
            continue;
        if (!(g_snd.chaninfo[i].pAlias0->flags & 1))
            continue;
        if (g_snd.chaninfo[i].pAlias0->pszAliasName != pAlias0->pszAliasName)
            continue;
        if (g_snd.chaninfo[i].pAlias1->pszAliasName != pAlias1->pszAliasName)
            continue;

        g_snd.chaninfo[i].basevolume = (1.0f - lerp) * pAlias0->fVolMin + lerp * pAlias1->fVolMin;
        rate = I_fround((float)g_snd.chaninfo[i].baserate * g_snd.chaninfo[i].pitch * g_snd.timescale);
        SND_SetStreamChannelPlaybackRate(i, rate);
        g_snd.chaninfo[i].org[0] = org[0];
        g_snd.chaninfo[i].org[1] = org[1];
        g_snd.chaninfo[i].org[2] = org[2];
        g_snd.chaninfo[i].looptime = g_snd.looptime;
        g_snd.chaninfo[i].lerp = lerp;
        if (pChannel)
            *pChannel = i;
        return 1;
    }

    return 0;
}

static void SND_ChoosePitchAndVolume(const snd_alias_t *pAlias0, const snd_alias_t *pAlias1, float lerp, float *volume, float *pitch)
{
    float volMin;
    float volMax;
    float pitchMin;
    float pitchMax;

    volMin = (1.0f - lerp) * pAlias0->fVolMin + lerp * pAlias1->fVolMin;
    volMax = (1.0f - lerp) * pAlias0->fVolMax + lerp * pAlias1->fVolMax;
    pitchMin = (1.0f - lerp) * pAlias0->fPitchMin + lerp * pAlias1->fPitchMin;
    pitchMax = (1.0f - lerp) * pAlias0->fPitchMax + lerp * pAlias1->fPitchMax;
    *volume = volMin + randomf() * (volMax - volMin);
    *pitch = pitchMin + randomf() * (pitchMax - pitchMin);
}

static __attribute_regparm__(3) int SND_PlaySoundAlias_Internal(const snd_alias_t *pAlias0, const snd_alias_t *pAlias1, float lerp, int entnum, const vec_t *org, int *pChannel, int timeshift, Bool treatAsMaster, snd_alias_system_t system)
{
    const snd_alias_t *secondaryAlias;
    int entchannel;
    int playType;
    float fDistMax;
    float volume;
    float pitch;

    if (!g_snd.Initialized2d)
        return 0;

    if (pChannel)
        *pChannel = -1;

    if (pAlias0->pszSecondaryAliasName) {
        secondaryAlias = Com_PickSoundAlias(pAlias0->pszSecondaryAliasName);
        if (secondaryAlias) {
            if (!secondaryAlias->pszSecondaryAliasName) {
                SND_PlaySoundAlias_Internal(secondaryAlias, secondaryAlias, lerp, entnum, org, NULL, timeshift, treatAsMaster, system);
            } else {
                Com_Printf("^1Error: a secondaryAlias cannot have a secondary Alias of its own.\nAlias sequence: '%s'->'%s'->'%s'\n",
                           pAlias0->pszAliasName, pAlias0->pszSecondaryAliasName, secondaryAlias->pszSecondaryAliasName);
            }
        }
    }

    switch ((pAlias0->flags & 0x780) >> 7) {
    case 1:
    case 2:
    case 7:
    case 8:
    case 9:
    case 10:
        break;
    default:
        fDistMax = (1.0f - lerp) * pAlias0->fDistMax + lerp * pAlias1->fDistMax;
        if (Vec3DistanceSq(org, g_snd.listeners[0].orient.origin) > fDistMax * fDistMax)
            return 0;
        break;
    }

    if (SND_ContinueLoopingSound(pAlias0, pAlias1, lerp, entnum, org, pChannel))
        return 0;

    entchannel = (pAlias0->flags & 0x780) >> 7;
    if (entchannel != 0 && entchannel != 1 && entchannel != 2 && !pAlias0->pszSecondaryAliasName)
        SND_StopEntityChannel(entnum, entchannel);

    SND_ChoosePitchAndVolume(pAlias0, pAlias1, lerp, &volume, &pitch);

    playType = (pAlias0->flags & 0x60) >> 5;
    switch (playType) {
    case 1:
        return SND_StartAliasSample(pAlias0, pAlias1, lerp, entnum, org, volume, pitch, timeshift, treatAsMaster, pChannel, system);
    case 2:
    case 3:
        return SND_StartAliasStream(pAlias0, pAlias1, lerp, entnum, org, volume, pitch, timeshift, 0.0f, treatAsMaster, pChannel, system);
    }

    return 0;
}

int SND_PlaySoundAlias(const snd_alias_t *pAlias, int entnum, const vec_t *org, int timeshift, snd_alias_system_t system)
{
    if (!pAlias)
        return 0;
    return SND_PlaySoundAlias_Internal(pAlias, pAlias, 0.0f, entnum, org, NULL, timeshift, 0, system);
}

int SND_PlaySoundAliasAsMaster(const snd_alias_t *pAlias, int entnum, const vec_t *org, int timeshift, snd_alias_system_t system)
{
    if (!pAlias)
        return 0;
    return SND_PlaySoundAlias_Internal(pAlias, pAlias, 0.0f, entnum, org, NULL, timeshift, 1, system);
}

int SND_PlayBlendedSoundAliases(const snd_alias_t *pAlias0, const snd_alias_t *pAlias1, float fLerp, int entnum, const vec_t *org, int timeshift, snd_alias_system_t system)
{
    if (!pAlias0 || !pAlias1)
        return 0;

    SND_ValidateSoundAliasBlend(pAlias0, pAlias1, 1);
    return SND_PlaySoundAlias_Internal(pAlias0, pAlias1, fLerp, entnum, org, NULL, timeshift, 0, system);
}

int SND_PlayLocalSoundAlias(snd_alias_list_t *aliasList, snd_alias_system_t system)
{
    const snd_alias_t *pAlias;

    if (aliasList) {
        pAlias = Com_PickSoundAliasFromList(aliasList);
        if (pAlias)
            return SND_PlaySoundAlias_Internal(pAlias, pAlias, 0.0f, g_snd.listeners[0].entnum, g_snd.listeners[0].orient.origin, NULL, 0, 0, system);
    }

    return 0;
}

int SND_PlayLocalSoundAliasByName(const char *aliasname, snd_alias_system_t system)
{
    const snd_alias_t *pAlias;

    if (aliasname) {
        pAlias = Com_PickSoundAlias(aliasname);
        if (pAlias)
            return SND_PlaySoundAlias_Internal(pAlias, pAlias, 0.0f, g_snd.listeners[0].entnum, g_snd.listeners[0].orient.origin, NULL, 0, 0, system);
    }

    return 0;
}

// retail 0x44bc50, unreferenced; KisakCOD SND_SetPauseSettings (SUPPORTED)
void SND_SetPauseSettings(const Bool *pauseSettings)
{
    int i;

    for (i = 0; i < 11; i++)
        g_snd.pauseSettings[i] = pauseSettings[i];
}

// retail 0x44bc80, unreferenced; KisakCOD SND_ResetPauseSettingsToDefaults (SUPPORTED)
void SND_ResetPauseSettingsToDefaults(void)
{
    int i;

    for (i = 0; i < 11; i++)
        g_snd.pauseSettings[i] = g_snd.defaultPauseSettings[i];
}

static void SND_PauseSounds(void)
{
    int i;

    if (!g_snd.Initialized2d)
        return;
    if (g_snd.paused)
        return;

    for (i = 45; i < 45 + g_snd.max_2D_channels; i++) {
        if (SND_Is2DChannelFree(i))
            continue;
        if (!g_snd.pauseSettings[(g_snd.chaninfo[i].pAlias0->flags & 0x780) >> 7])
            continue;
        SND_Pause2DChannel(i);
    }

    for (i = 0; i < g_snd.max_3D_channels; i++) {
        if (SND_Is3DChannelFree(i))
            continue;
        if (!g_snd.pauseSettings[(g_snd.chaninfo[i].pAlias0->flags & 0x780) >> 7])
            continue;
        SND_Pause3DChannel(i);
    }

    for (i = 32; i < 32 + g_snd.max_stream_channels; i++) {
        if (SND_IsStreamChannelFree(i))
            continue;
        if (!g_snd.pauseSettings[(g_snd.chaninfo[i].pAlias0->flags & 0x780) >> 7])
            continue;
        SND_PauseStreamChannel(i);
    }

    g_snd.paused = 1;
    g_snd.pausetime = g_snd.time;
}

static void SND_UnpauseSounds(void)
{
    int i;
    int timeshift;

    if (!g_snd.Initialized2d)
        return;
    if (!g_snd.paused)
        return;

    timeshift = g_snd.time - g_snd.pausetime;

    for (i = 45; i < 45 + g_snd.max_2D_channels; i++) {
        if (SND_Is2DChannelFree(i))
            continue;
        if (!g_snd.chaninfo[i].paused)
            continue;
        SND_Unpause2DChannel(i, timeshift);
    }

    for (i = 0; i < g_snd.max_3D_channels; i++) {
        if (SND_Is3DChannelFree(i))
            continue;
        if (!g_snd.chaninfo[i].paused)
            continue;
        SND_Unpause3DChannel(i, timeshift);
    }

    for (i = 32; i < 32 + g_snd.max_stream_channels; i++) {
        if (SND_IsStreamChannelFree(i))
            continue;
        if (!g_snd.chaninfo[i].paused)
            continue;
        SND_UnpauseStreamChannel(i, timeshift);
    }

    g_snd.paused = 0;
    g_snd.pausetime = 0;
}

// retail 0x44c030, unreferenced; name unknown
void SND_UpdatePause_44c030(void)
{
    Bool paused;

    paused = (*(int *)((char *)cl_paused + 8) != 0);
    if (paused != g_snd.paused) {
        if (paused)
            SND_PauseSounds();
        else
            SND_UnpauseSounds();
    }
}

static __attribute_regparm__(3) void SND_StartBackground(int track, const snd_alias_t *pAlias, int fadetime, snd_alias_system_t system)
{
    int channel;
    int type;
    float volume;
    float pitch;
    Bool newPaused;

    newPaused = (*(int *)((char *)cl_paused + 8) != 0);
    if (newPaused != g_snd.paused) {
        if (newPaused)
            SND_PauseSounds();
        else
            SND_UnpauseSounds();
    }

    switch ((pAlias->flags & 0x780) >> 7) {
    case 1:
    case 2:
    case 7:
    case 8:
    case 9:
    case 10:
        break;
    default:
        Com_Error(ERR_DROP, "\025alias %s sound %s played as an ambient / music track uses a 3D channel type; should probably be channel 'local'\n", pAlias->pszAliasName, Com_GetSoundFileName(pAlias));
        break;
    }

    type = (pAlias->flags & 0x60) >> 5;
    if (type != 2 && type != 3) {
        Com_Error(ERR_DROP, "\025alias %s sound %s played as an ambient / music track is not streamed; type must be 'streamed'\n", pAlias->pszAliasName, Com_GetSoundFileName(pAlias));
    }

    volume = pAlias->fVolMin + (pAlias->fVolMax - pAlias->fVolMin) * randomf();

    pitch = pAlias->fPitchMin + (pAlias->fPitchMax - pAlias->fPitchMin) * randomf();

    channel = track + 32;

    if (!SND_IsStreamChannelFree(channel))
        SND_StopStreamChannel(channel);

    g_snd.chaninfo[channel].basevolume = volume;
    g_snd.background[track].goalvolume = volume;

    if (fadetime > 0) {
        g_snd.background[track].goalrate = g_snd.chaninfo[channel].basevolume / (float)fadetime;
        g_snd.chaninfo[channel].basevolume = 0.0f;
    } else {
        g_snd.background[track].goalrate = 0.0f;
    }

    if (!*(unsigned char *)((char *)snd_enableStream + 8))
        return;

    SND_StartAliasStreamOnChannel(pAlias, pAlias, 0.0f, 0x400, (vec_t *)&g_snd.listeners[0].orient.origin, g_snd.chaninfo[channel].basevolume, pitch, 0, 0.0f, pAlias->startDelay, ((unsigned int)pAlias->flags >> 1) & 1, channel, system);
}

static void SND_StopBackground(int track, int fadetime)
{
    if (SND_IsStreamChannelFree(track + 32))
        return;

    if (fadetime == 0) {
        SND_StopStreamChannel(track + 32);
        return;
    }

    if (g_snd.background[track].goalvolume > 0.0f) {
        g_snd.background[track].goalrate = -g_snd.background[track].goalvolume / (float)fadetime;
        g_snd.background[track].goalvolume = 0.0f;
    }
}

void SND_PlayMusicAlias(const snd_alias_t *pAlias, snd_alias_system_t system)
{
    if (!g_snd.Initialized2d)
        return;
    if (!pAlias)
        return;
    if (!SND_IsStreamChannelFree(32))
        return;

    SND_StartBackground(0, pAlias, 0, system);
}

void SND_StopMusic(int fadetime)
{
    SND_StopBackground(0, fadetime);
}

void SND_PlayAmbientAlias(const snd_alias_t *pAlias, int fadetime, snd_alias_system_t system)
{
    int tracknum;
    snd_background_info_t *trackptr;
    const snd_alias_t *curAlias;
    int curFlags, newFlags;
    float volume;

    if (!g_snd.Initialized2d)
        return;
    if (!pAlias)
        return;

    tracknum = g_snd.ambient_track + 32;
    trackptr = &g_snd.background[g_snd.ambient_track];

    if (!SND_IsStreamChannelFree(tracknum)) {

        curAlias = g_snd.chaninfo[tracknum].pAlias0;
        if (curAlias->pszAliasName == pAlias->pszAliasName)
            return;

        if (Com_GetSoundFileName(curAlias) == Com_GetSoundFileName(pAlias)) {
            curFlags = curAlias->flags;
            newFlags = pAlias->flags;
            if ((curFlags & 1) == (newFlags & 1) &&
                curAlias->fPitchMin == pAlias->fPitchMin &&
                curAlias->fPitchMax == pAlias->fPitchMax &&
                (curFlags & 0x780) == (newFlags & 0x780)) {

                g_snd.chaninfo[tracknum].pAlias0 = pAlias;
                g_snd.chaninfo[tracknum].pAlias1 = pAlias;
                volume = pAlias->fVolMin + randomf() * (pAlias->fVolMax - pAlias->fVolMin);
                if (fadetime != 0)
                    trackptr->goalrate = (volume - trackptr->goalvolume) / (float)fadetime;
                else
                    trackptr->goalrate = 0.0f;
                trackptr->goalvolume = volume;
                return;
            }
        }
    }

    SND_StopBackground(g_snd.ambient_track, fadetime);
    g_snd.ambient_track = 3 - g_snd.ambient_track;
    SND_StartBackground(g_snd.ambient_track, pAlias, fadetime, system);
}

void SND_FadeAllSounds(float volume, int fadetime)
{
    g_snd.mastervol.goalvolume = volume;
    g_snd.mastervol.goalrate = volume - g_snd.mastervol.volume;
    if (fadetime != 0) {
        g_snd.mastervol.goalrate /= (float)fadetime;
    } else if (volume == 0.0f) {
        SND_StopSounds( (snd_stopsounds_arg_t)(0));
    }
}

void SND_SetChannelVolumes(int priority, const float *channelvolume, int fademsec)
{
    int j;
    int k;
    snd_channelvolgroup *channelVolGroup;
    float curVolume;

    channelVolGroup = &g_snd.channelVolGroups[priority];
    channelVolGroup->active = 1;

    if (fademsec < 1)
        fademsec = 1;

    for (j = 0; j < 11; j++) {
        channelVolGroup->channelvol[j].goalvolume = channelvolume[j];
        curVolume = g_snd.channelvol->channelvol[j].volume;
        channelVolGroup->channelvol[j].volume = curVolume;
        channelVolGroup->channelvol[j].goalrate = (channelvolume[j] - curVolume) / (float)fademsec;
    }

    if (channelVolGroup == g_snd.channelvol)
        return;

    for (k = priority + 1; k < 4; k++) {
        if (g_snd.channelVolGroups[k].active)
            return;
    }

    g_snd.channelvol = channelVolGroup;
}

void SND_DeactivateChannelVolumes(int priority, int fademsec)
{
    int j;
    snd_channelvolgroup *channelVolGroup;
    int newPriority;

    channelVolGroup = &g_snd.channelVolGroups[priority];
    channelVolGroup->active = 0;

    if (channelVolGroup != g_snd.channelvol)
        return;

    newPriority = priority - 1;
    while (newPriority >= 0 && !g_snd.channelVolGroups[newPriority].active) {
        newPriority--;
    }

    if (fademsec < 1)
        fademsec = 1;

    g_snd.channelvol = &g_snd.channelVolGroups[newPriority];
    for (j = 0; j < 11; j++) {
        g_snd.channelvol->channelvol[j].volume = channelVolGroup->channelvol[j].volume;
        g_snd.channelvol->channelvol[j].goalrate = (g_snd.channelvol->channelvol[j].goalvolume - channelVolGroup->channelvol[j].volume) / (float)fademsec;
    }
}

void SND_UpdateLoopingSounds(void)
{
    int i;

    if (!g_snd.Initialized2d)
        return;
    if (g_snd.paused)
        return;

    for (i = 0; i < g_snd.max_3D_channels; i++) {
        if (SND_Is3DChannelFree(i))
            continue;
        if (!(g_snd.chaninfo[i].pAlias0->flags & 1))
            continue;
        if (g_snd.chaninfo[i].looptime == g_snd.looptime)
            continue;
        SND_Stop3DChannel(i);
    }

    for (i = 45; i < 45 + g_snd.max_2D_channels; i++) {
        if (SND_Is2DChannelFree(i))
            continue;
        if (!(g_snd.chaninfo[i].pAlias0->flags & 1))
            continue;
        if (g_snd.chaninfo[i].looptime == g_snd.looptime)
            continue;
        SND_Stop2DChannel(i);
    }

    for (i = 32; i < 32 + g_snd.max_stream_channels; i++) {
        if (SND_IsStreamChannelFree(i))
            continue;
        if (!(g_snd.chaninfo[i].pAlias0->flags & 1))
            continue;
        if (g_snd.chaninfo[i].looptime == g_snd.looptime)
            continue;
        SND_StopStreamChannel(i);
    }

    g_snd.looptime = g_snd.time;
}

Bool SND_UpdateBackgroundVolume(int track, int frametime)
{
    int channel;
    float newvol;

    channel = track + 32;
    newvol = (float)frametime * g_snd.background[track].goalrate + g_snd.chaninfo[channel].basevolume;

    if (g_snd.background[track].goalrate > 0.0f) {
        if (newvol > g_snd.background[track].goalvolume)
            newvol = g_snd.background[track].goalvolume;
    } else if (newvol < g_snd.background[track].goalvolume) {
        newvol = g_snd.background[track].goalvolume;
        if (newvol == 0.0f) {
            SND_StopStreamChannel(channel);
            return 0;
        }
    }

    g_snd.chaninfo[channel].looptime = g_snd.looptime;
    g_snd.chaninfo[channel].basevolume = newvol;
    return 1;
}

static void SND_UpdateVolume(snd_volume_info_t *vi, int frametime)
{
    vi->volume += (float)frametime * vi->goalrate;
    if (vi->goalrate < 0.0f) {
        if (vi->volume < vi->goalvolume) {
            vi->volume = vi->goalvolume;
            vi->goalrate = 0.0f;
        }
    } else if (vi->volume > vi->goalvolume) {
        vi->volume = vi->goalvolume;
        vi->goalrate = 0.0f;
    }
}

static void SND_UpdateVolumes(int frametime)
{
    int i;

    for (i = 0; i < 11; i++)
        SND_UpdateVolume(&g_snd.channelvol->channelvol[i], frametime);

    if (g_snd.mastervol.goalrate == 0.0f) {
        if (!*(unsigned char *)((char *)snd_volume + 7))
            return;
    } else {
        SND_UpdateVolume(&g_snd.mastervol, frametime);
        if (g_snd.mastervol.volume == 0.0f && g_snd.mastervol.goalrate == 0.0f)
            SND_StopSounds((snd_stopsounds_arg_t)0);
    }

    Dvar_ClearModified((dvar_t *)snd_volume);
    g_snd.volume = g_snd.mastervol.volume * *(float *)((char *)snd_volume + 8) * 0.75f;
}

// retail 0x44cdf0, unreferenced; name unknown
int SND_IsStreamChannelPlaying_44cdf0(int index)
{
    return !SND_IsStreamChannelFree(index) && !g_snd.chaninfo[index].startDelay;
}

static inline float I_fsel(const float x, const float y, const float z)
{
    return x >= 0.0f ? y : z;
}

static void SND_UpdateSlaveLerp(int frametime)
{
    Bool hasMaster;
    int fadeTime;
    int i;

    hasMaster = 0;
    for (i = 0; !hasMaster && i < g_snd.max_3D_channels; i++) {
        if (!SND_Is3DChannelFree(i) && g_snd.chaninfo[i].startDelay == 0 && g_snd.chaninfo[i].master)
            hasMaster = 1;
    }
    for (i = 32; !hasMaster && i < 32 + g_snd.max_stream_channels; i++) {
        if (!SND_IsStreamChannelFree(i) && g_snd.chaninfo[i].startDelay == 0 && g_snd.chaninfo[i].master)
            hasMaster = 1;
    }
    for (i = 45; !hasMaster && i < 45 + g_snd.max_2D_channels; i++) {
        if (!SND_Is2DChannelFree(i) && g_snd.chaninfo[i].startDelay == 0 && g_snd.chaninfo[i].master)
            hasMaster = 1;
    }

    fadeTime = *(int *)((char *)snd_slaveFadeTime + 8);
    if (!fadeTime) {
        g_snd.slaveLerp = hasMaster ? 1.0f : 0.0f;
        return;
    }

    if (hasMaster)
        g_snd.slaveLerp += (float)frametime / fadeTime;
    else
        g_snd.slaveLerp -= (float)frametime / fadeTime;

    g_snd.slaveLerp = I_fsel(-g_snd.slaveLerp, 0.0f, I_fsel(g_snd.slaveLerp - 1.0f, 1.0f, g_snd.slaveLerp));
}

static void SND_UpdateChannels(int frametime)
{
    int i;

    SND_UpdateSlaveLerp(frametime);

    for (i = 0; i < g_snd.max_3D_channels; i++) {
        if (!SND_Is3DChannelFree(i))
            SND_Update3DChannel(i, frametime);
    }

    for (i = 32; i < 32 + g_snd.max_stream_channels; i++) {
        if (!SND_IsStreamChannelFree(i))
            SND_UpdateStreamChannel(i, frametime);
    }

    for (i = 45; i < 45 + g_snd.max_2D_channels; i++) {
        if (!SND_Is2DChannelFree(i))
            SND_Update2DChannel(i, frametime);
    }
}

void SND_SetEnvironmentEffects(int priority, const char *roomstring, float drylevel, float wetlevel, int fademsec)
{
    snd_enveffect *effect;
    int roomtype;
    int k;

    if (!g_snd.Initialized2d)
        return;

    effect = &g_snd.envEffects[priority];
    effect->active = 1;

    roomtype = SND_RoomtypeFromString(roomstring);
    effect->roomtype = roomtype;

    if (fademsec < 1)
        fademsec = 1;

    effect->drygoal = drylevel;
    effect->drylevel = g_snd.effect->drylevel;
    effect->dryrate = (drylevel - g_snd.effect->drylevel) / fademsec;
    effect->wetgoal = wetlevel;
    effect->wetlevel = g_snd.effect->wetlevel;
    effect->wetrate = (wetlevel - g_snd.effect->wetlevel) / fademsec;

    if (effect == g_snd.effect) {
        SND_SetRoomtype(roomtype);
        return;
    }

    for (k = priority + 1; k < 3; k++) {
        if (g_snd.envEffects[k].active)
            return;
    }

    g_snd.effect = effect;
    SND_SetRoomtype(roomtype);
}

void SND_DeactivateEnvironmentEffects(int priority, int fademsec)
{
    snd_enveffect *effect;
    int newPriority;

    effect = &g_snd.envEffects[priority];
    effect->active = 0;

    if (effect != g_snd.effect)
        return;

    newPriority = priority - 1;
    while (newPriority >= 0 && !g_snd.envEffects[newPriority].active) {
        newPriority--;
    }

    if (fademsec < 1)
        fademsec = 1;

    g_snd.effect = &g_snd.envEffects[newPriority];
    SND_SetRoomtype(g_snd.effect->roomtype);
    g_snd.effect->drylevel = effect->drylevel;
    g_snd.effect->dryrate = (g_snd.effect->drygoal - effect->drylevel) / (float)fademsec;
    g_snd.effect->wetlevel = effect->wetlevel;
    g_snd.effect->wetrate = (g_snd.effect->wetgoal - effect->wetlevel) / (float)fademsec;
}

void SND_UpdateReverbs(void)
{
    int i;
    for (i = 0; i < g_snd.max_3D_channels; i++) {
        if (!SND_Is3DChannelFree(i)) {
            SND_Update3DChannelReverb(i);
        }
    }
    for (i = 45; i < 45 + g_snd.max_2D_channels; i++) {
        if (!SND_Is2DChannelFree(i)) {
            SND_Update2DChannelReverb(i);
        }
    }
    for (i = 32; i < 32 + g_snd.max_stream_channels; i++) {
        if (!SND_IsStreamChannelFree(i)) {
            SND_UpdateStreamChannelReverb(i);
        }
    }
}

static void SND_UpdateEnvironmentEffects(int frametime)
{
    if (g_snd.effect->dryrate == 0.0f && g_snd.effect->wetrate == 0.0f)
        return;

    if (g_snd.effect->dryrate != 0.0f) {
        g_snd.effect->drylevel += (float)frametime * g_snd.effect->dryrate;
        if (g_snd.effect->dryrate < 0.0f) {
            if (g_snd.effect->drylevel <= g_snd.effect->drygoal) {
                g_snd.effect->drylevel = g_snd.effect->drygoal;
                g_snd.effect->dryrate = 0.0f;
            }
        } else if (g_snd.effect->drylevel >= g_snd.effect->drygoal) {
            g_snd.effect->drylevel = g_snd.effect->drygoal;
            g_snd.effect->dryrate = 0.0f;
        }
    }

    if (g_snd.effect->wetrate != 0.0f) {
        g_snd.effect->wetlevel += (float)frametime * g_snd.effect->wetrate;
        if (g_snd.effect->wetrate < 0.0f) {
            if (g_snd.effect->wetlevel <= g_snd.effect->wetgoal) {
                g_snd.effect->wetlevel = g_snd.effect->wetgoal;
                g_snd.effect->wetrate = 0.0f;
            }
        } else if (g_snd.effect->wetlevel >= g_snd.effect->wetgoal) {
            g_snd.effect->wetlevel = g_snd.effect->wetgoal;
            g_snd.effect->wetrate = 0.0f;
        }
    }

    SND_UpdateReverbs();
}

static void SND_UpdateTimescale(void)
{
    float timescale;
    float ratio;
    int rate;
    int i;

    timescale = Com_GetTimescaleForSnd();
    if (timescale <= 0.0f)
        timescale = 1.0f;

    if (timescale == g_snd.timescale)
        return;

    ratio = timescale / g_snd.timescale;
    g_snd.timescale = timescale;

    for (i = 0; i < g_snd.max_3D_channels; i++) {
        if (SND_Is3DChannelFree(i))
            continue;
        rate = SND_Get3DChannelPlaybackRate(i);
        SND_Set3DChannelPlaybackRate(i, I_fround((float)rate * ratio));
    }

    for (i = 32; i < 32 + g_snd.max_stream_channels; i++) {
        if (SND_IsStreamChannelFree(i))
            continue;
        rate = SND_GetStreamChannelPlaybackRate(i);
        SND_SetStreamChannelPlaybackRate(i, I_fround((float)rate * ratio));
    }

    for (i = 45; i < 45 + g_snd.max_2D_channels; i++) {
        if (SND_Is2DChannelFree(i))
            continue;
        rate = SND_Get2DChannelPlaybackRate(i);
        SND_Set2DChannelPlaybackRate(i, I_fround((float)rate * ratio));
    }
}

void SND_Update(void)
{
    int frametime;
    Bool paused;
    MemoryFile memFile;

    if (!g_snd.Initialized2d)
        return;

    g_snd.cpu = SND_GetDriverCPUPercentage();
    if (*(unsigned char *)((char *)com_statmon + 8) && g_snd.cpu > 2)
        StatMon_Warning(2, 3000, "gfx/2d/warning@soundcpu.jpg");

    frametime = Sys_Milliseconds() - g_snd.time;
    g_snd.time += frametime;

    paused = *(int *)((char *)cl_paused + 8) != 0;
    if (paused != g_snd.paused) {
        if (paused)
            SND_PauseSounds();
        else
            SND_UnpauseSounds();
    }

    SND_UpdateVolumes(frametime);

    if (!g_snd.paused) {
        if (g_snd.restore.buffer) {
            MemFile_InitForReading(&memFile, g_snd.restore.size, g_snd.restore.buffer);
            SND_Restore(&memFile);
            Z_FreeInternal((void *)g_snd.restore.buffer);
            g_snd.restore.buffer = 0;
            g_snd.restore.size = 0;
        }

        SND_UpdateTimescale();
        SND_UpdateEnvironmentEffects(frametime);
    }

    SND_UpdateChannels(frametime);
    SND_DriverPostUpdate();
}

void SND_StopSounds(snd_stopsounds_arg_t which)
{
    int i;

    if (!g_snd.Initialized2d)
        return;

    if (!(which & 8)) {

        for (i = 45; i < g_snd.max_2D_channels + 45; i++) {
            if (!SND_Is2DChannelFree(i))
                SND_Stop2DChannel(i);
        }

        for (i = 0; i < g_snd.max_3D_channels; i++) {
            if (!SND_Is3DChannelFree(i))
                SND_Stop3DChannel(i);
        }
    }

    for (i = 32; i < g_snd.max_stream_channels + 32; i++) {
        if (SND_IsStreamChannelFree(i))
            continue;

        if ((which & 2) && i == 32)
            continue;
        if ((which & 4) && (i == 33 || i == 34))
            continue;

        SND_StopStreamChannel(i);
    }

    if (!(which & 1)) {
        for (i = 1; i < 3; i++) {
            SND_DeactivateEnvironmentEffects(i, 0);
        }
    }

    if (!(which & 0x10)) {
        for (i = 1; i < 4; i++) {
            SND_DeactivateChannelVolumes(i, 0);
        }
    }
}

void SND_Init(void)
{
    int i;

    Com_Printf("------- Sound Initialization -------\n");

    snd_errorOnMissing = Dvar_RegisterBool("snd_errorOnMissing", 0, 0x1001);
    snd_khz = Dvar_RegisterInt("snd_khz", 44, 11, 44, 0x1021);
    snd_bits = Dvar_RegisterInt("snd_bits", 16, 8, 16, 0x1021);
    snd_stereo = Dvar_RegisterBool("snd_stereo", 1, 0x1021);
    snd_volume = Dvar_RegisterFloat("snd_volume", 1.0f, 0.0f, 1.0f, 0x1001);
    snd_slaveFadeTime = Dvar_RegisterInt("snd_slaveFadeTime", 500, 0, 5000, 0x1081);
    snd_enable2D = Dvar_RegisterBool("snd_enable2D", 1, 0x1080);
    snd_enable3D = Dvar_RegisterBool("snd_enable3D", 1, 0x1080);
    snd_enableStream = Dvar_RegisterBool("snd_enableStream", 1, 0x1080);
    snd_enableReverb = Dvar_RegisterBool("snd_enableReverb", 1, 0x1080);
    snd_touchStreamFilesOnLoad = Dvar_RegisterBool("snd_touchStreamFilesOnLoad", 0, 0x1001);

    g_snd.effect = &g_snd.envEffects[0];
    g_snd.envEffects[0].roomtype = 0;
    g_snd.envEffects[0].drylevel = 1.0f;
    g_snd.envEffects[0].drygoal = 1.0f;
    g_snd.envEffects[0].dryrate = 0.0f;
    g_snd.envEffects[0].wetlevel = 0.0f;
    g_snd.envEffects[0].wetgoal = 0.0f;
    g_snd.envEffects[0].wetrate = 0.0f;
    g_snd.envEffects[0].active = 1;

    if (!SND_InitDriver())
        return;

    g_snd.mastervol.volume = 1.0f;
    g_snd.mastervol.goalvolume = 1.0f;
    g_snd.mastervol.goalrate = 0.0f;

    g_snd.channelvol = &g_snd.channelVolGroups[0];
    for (i = 0; i < 11; i++) {
        g_snd.channelvol->channelvol[i].volume = 1.0f;
        g_snd.channelvol->channelvol[i].goalvolume = 1.0f;
        g_snd.channelvol->channelvol[i].goalrate = 0.0f;
    }
    g_snd.channelvol->active = 1;

    g_snd.time = Sys_Milliseconds();
    g_snd.looptime = g_snd.time;
    g_snd.slaveLerp = 0.0f;

    g_snd.volume = 0.75f * *(float *)((char *)snd_volume + 8);

    for (i = 0; i < 11; i++) {
        g_snd.defaultPauseSettings[i] = 1;
    }
    g_snd.defaultPauseSettings[2] = 0;

    for (i = 0; i < 11; i++) {
        g_snd.pauseSettings[i] = g_snd.defaultPauseSettings[i];
    }

    Cmd_AddCommand("snd_setEnvironmentEffects", SND_SetEnvironmentEffects_f);
    Cmd_AddCommand("snd_deactivateEnvironmentEffects", SND_DeactivateEnvironmentEffects_f);

    Com_Printf("------- Sound Initialized -------\n");

    Voice_Init();
}

void SND_Shutdown(void)
{
    Voice_Shutdown();
    if (g_snd.restore.buffer) {
        Z_FreeInternal((void *)g_snd.restore.buffer);
    }
    if (!g_snd.Initialized2d) {
        return;
    }
    SND_StopSounds( (snd_stopsounds_arg_t)(0));
    Com_UnloadSoundAliases( (snd_alias_system_t)(1));
    Com_UnloadSoundAliases( (snd_alias_system_t)(0));
    SND_ShutdownDriver();
    memset(&g_snd, 0, sizeof(g_snd));
    Cmd_RemoveCommand("snd_setEnvironmentEffects");
    Cmd_RemoveCommand("snd_deactivateEnvironmentEffects");
}

void SND_ShutdownChannels(void)
{
    SND_StopSounds( (snd_stopsounds_arg_t)(0));
    memset(g_snd.chaninfo, 0, sizeof(g_snd.chaninfo));
}

static __attribute_regparm__(2) void SND_SaveChanInfo(snd_channel_info_t *chaninfo, MemoryFile *memFile)
{
    MemFile_WriteShort(memFile, (short)chaninfo->entnum);
    MemFile_WriteByte(memFile, (unsigned char)chaninfo->entchannel);
    MemFile_WriteByte(memFile, chaninfo->master);
    MemFile_WriteInt(memFile, chaninfo->startDelay);
    MemFile_WriteFloat(memFile, chaninfo->basevolume);
    MemFile_WriteFloat(memFile, chaninfo->lerp);
    MemFile_WriteVec3(memFile, chaninfo->offset);
}

static __attribute_regparm__(2) void SND_RestoreChanInfo(snd_channel_info_t *chaninfo, MemoryFile *memFile)
{
    memset(chaninfo, 0, sizeof(snd_channel_info_t));

    chaninfo->entnum = MemFile_ReadShort(memFile);
    chaninfo->entchannel = MemFile_ReadByte(memFile);
    chaninfo->master = MemFile_ReadByte(memFile) != 0;
    chaninfo->startDelay = MemFile_ReadInt(memFile);
    chaninfo->basevolume = MemFile_ReadFloat(memFile);
    chaninfo->lerp = MemFile_ReadFloat(memFile);
    MemFile_ReadVec3(memFile, chaninfo->offset);
}

static void SND_SaveSoundAlias(const snd_alias_t *alias, MemoryFile *memFile)
{
    short value;

    MemFile_WriteCString(memFile, alias->pszAliasName);
    value = SND_GetAliasOffset(alias);
    MemFile_WriteData(memFile, 2, &value);
}

const snd_alias_t *SND_RestoreSoundAlias(MemoryFile *memFile)
{
    const char *name;

    name = MemFile_ReadCString(memFile);
    if (!*name)
        return 0;

    return SND_GetAliasWithOffset(name, (unsigned short)MemFile_ReadShort(memFile));
}

static void SND_Save3DChannel(int chanIndex, MemoryFile *memFile)
{
    snd_save_3D_sample_t info;

    if (SND_Is3DChannelFree(chanIndex))
        return;
    if (g_snd.chaninfo[chanIndex].system != 1)
        return;
    if (!SND_Get3DChannelLength(chanIndex))
        return;
    if (!SND_Get3DChannelPlaybackRate(chanIndex))
        return;

    SND_Get3DChannelSaveInfo(chanIndex, &info);
    SND_SaveSoundAlias(g_snd.chaninfo[chanIndex].pAlias0, memFile);
    SND_SaveSoundAlias(g_snd.chaninfo[chanIndex].pAlias1, memFile);
    SND_SaveChanInfo(&g_snd.chaninfo[chanIndex], memFile);
    MemFile_WriteData(memFile, sizeof(info), &info);
}

static Bool SND_Restore3DChannel(MemoryFile *memFile)
{
    const snd_alias_t *pAlias0;
    const snd_alias_t *pAlias1;
    snd_channel_info_t chaninfo;
    snd_save_3D_sample_t info;
    int channel;
    float *offset;

    pAlias0 = SND_RestoreSoundAlias(memFile);
    if (!pAlias0)
        return 0;

    pAlias1 = SND_RestoreSoundAlias(memFile);
    SND_RestoreChanInfo(&chaninfo, memFile);
    MemFile_ReadData(memFile, sizeof(info), &info);

    if ((pAlias0->flags & 0x60) >> 5 != 1)
        return 1;
    if ((pAlias1->flags & 0x60) >> 5 != 1)
        return 1;
    if (!Com_GetSoundFileMem(pAlias0))
        return 1;
    if (Com_GetSoundFileMem(pAlias1) != Com_GetSoundFileMem(pAlias0))
        return 1;
    if (!SND_ValidateSoundAliasBlend(pAlias0, pAlias1, 0))
        return 1;
    if (!*(unsigned char *)((char *)snd_enable3D + 8))
        return 1;

    SND_StartAlias3DSample(pAlias0, pAlias1, chaninfo.lerp, chaninfo.entnum, info.org, info.volume, info.pitch, 0, info.fraction, chaninfo.startDelay, chaninfo.master, &channel, (snd_alias_system_t)(1));
    if (channel < 0)
        return 1;

    offset = g_snd.chaninfo[channel].offset;
    offset[0] = chaninfo.offset[0];
    offset[1] = chaninfo.offset[1];
    offset[2] = chaninfo.offset[2];
    return 1;
}

static void SND_Save2DChannel(int chanIndex, MemoryFile *memFile)
{
    snd_save_2D_sample_t info;

    if (SND_Is2DChannelFree(chanIndex))
        return;
    if (g_snd.chaninfo[chanIndex].system != 1)
        return;
    if (!SND_Get2DChannelLength(chanIndex))
        return;
    if (!SND_Get2DChannelPlaybackRate(chanIndex))
        return;

    SND_Get2DChannelSaveInfo(chanIndex, &info);
    SND_SaveSoundAlias(g_snd.chaninfo[chanIndex].pAlias0, memFile);
    SND_SaveSoundAlias(g_snd.chaninfo[chanIndex].pAlias1, memFile);
    SND_SaveChanInfo(&g_snd.chaninfo[chanIndex], memFile);
    MemFile_WriteData(memFile, sizeof(info), &info);
}

static Bool SND_Restore2DChannel(MemoryFile *memFile)
{
    const snd_alias_t *pAlias0;
    const snd_alias_t *pAlias1;
    snd_channel_info_t chaninfo;
    snd_save_2D_sample_t info;
    int channel;
    float *offset;

    pAlias0 = SND_RestoreSoundAlias(memFile);
    if (!pAlias0)
        return 0;

    pAlias1 = SND_RestoreSoundAlias(memFile);
    SND_RestoreChanInfo(&chaninfo, memFile);
    MemFile_ReadData(memFile, sizeof(info), &info);

    if ((pAlias0->flags & 0x60) >> 5 != 1)
        return 1;
    if ((pAlias1->flags & 0x60) >> 5 != 1)
        return 1;
    if (!Com_GetSoundFileMem(pAlias0))
        return 1;
    if (Com_GetSoundFileMem(pAlias1) != Com_GetSoundFileMem(pAlias0))
        return 1;
    if (!SND_ValidateSoundAliasBlend(pAlias0, pAlias1, 0))
        return 1;
    if (!*(unsigned char *)((char *)snd_enable2D + 8))
        return 1;

    SND_StartAlias2DSample(pAlias0, pAlias1, chaninfo.lerp, chaninfo.entnum, info.volume, info.pitch, 0, info.fraction, chaninfo.startDelay, chaninfo.master, &channel, (snd_alias_system_t)(1));
    if (channel < 0)
        return 1;

    SND_Set2DChannelFromSaveInfo(channel, &info);

    offset = g_snd.chaninfo[channel].offset;
    offset[0] = chaninfo.offset[0];
    offset[1] = chaninfo.offset[1];
    offset[2] = chaninfo.offset[2];
    return 1;
}

static void SND_SaveStreamChannel(int chanIndex, MemoryFile *memFile)
{
    snd_save_stream_t info;

    if (chanIndex >= 35) {
        if (SND_IsStreamChannelFree(chanIndex))
            return;
    } else {
        if (chanIndex == 33 || chanIndex == 34)
            return;
        if (SND_IsStreamChannelFree(chanIndex)) {
            MemFile_WriteCString(memFile, "");
            return;
        }
    }

    if (g_snd.chaninfo[chanIndex].system != 1)
        return;
    if (!SND_GetStreamChannelLength(chanIndex))
        return;

    SND_GetStreamChannelSaveInfo(chanIndex, &info);
    SND_SaveSoundAlias(g_snd.chaninfo[chanIndex].pAlias0, memFile);
    SND_SaveSoundAlias(g_snd.chaninfo[chanIndex].pAlias1, memFile);
    SND_SaveChanInfo(&g_snd.chaninfo[chanIndex], memFile);
    MemFile_WriteData(memFile, sizeof(info), &info);
}

static __attribute_regparm__(2)
    Bool SND_RestoreStreamChannel(int channel, MemoryFile *memFile)
{
    const snd_alias_t *pAlias0;
    const snd_alias_t *pAlias1;
    snd_channel_info_t chaninfo;
    snd_save_stream_t info;
    int rate;
    int type;
    pAlias0 = SND_RestoreSoundAlias(memFile);
    if (!pAlias0)
        return 0;
    pAlias1 = SND_RestoreSoundAlias(memFile);
    SND_RestoreChanInfo(&chaninfo, memFile);
    MemFile_ReadData(memFile, sizeof(info), &info);
    type = (pAlias0->flags & 0x60) >> 5;
    if (type != 2 && type != 3)
        return 1;
    type = (pAlias1->flags & 0x60) >> 5;
    if (type != 2 && type != 3)
        return 1;
    if (!SND_ValidateSoundAliasBlend(pAlias0, pAlias1, 0))
        return 1;
    if (channel < 0) {
        SND_StartAliasStream(pAlias0, pAlias1, chaninfo.lerp, chaninfo.entnum, info.org, info.basevolume, 1.0f, 0, info.fraction, 0, &channel, (snd_alias_system_t)(1));
    } else {
        if (!*(unsigned char *)((char *)snd_enableStream + 8))
            return 1;
        SND_StartAliasStreamOnChannel(pAlias0, pAlias1, chaninfo.lerp, chaninfo.entnum, info.org, info.basevolume, 1.0f, 0, info.fraction, chaninfo.startDelay, chaninfo.master, channel, (snd_alias_system_t)(1));
    }
    if (channel < 0)
        return 1;
    if (SND_IsStreamChannelFree(channel))
        return 1;
    SND_SetStreamChannelFromSaveInfo(channel, &info);
    rate = I_fround((float)info.rate * g_snd.chaninfo[channel].pitch * g_snd.timescale);
    SND_SetStreamChannelPlaybackRate(channel, rate);
    g_snd.chaninfo[channel].offset[0] = chaninfo.offset[0];
    g_snd.chaninfo[channel].offset[1] = chaninfo.offset[1];
    g_snd.chaninfo[channel].offset[2] = chaninfo.offset[2];
    return 1;
}

void SND_Save(MemoryFile *memFile)
{
    int i;

    for (i = 1; i < 4; i++)
        MemFile_WriteData(memFile, 0x88, &g_snd.channelVolGroups[i]);

    for (i = 1; i < 3; i++)
        MemFile_WriteData(memFile, 0x20, &g_snd.envEffects[i]);

    MemFile_WriteData(memFile, 8, &g_snd.background[1]);

    if (g_snd.Initialized2d) {
        for (i = 0; i < g_snd.max_3D_channels; i++)
            SND_Save3DChannel(i, memFile);
    }

    MemFile_WriteCString(memFile, "");

    if (g_snd.Initialized2d) {
        for (i = 45; i < g_snd.max_2D_channels + 45; i++)
            SND_Save2DChannel(i, memFile);
    }

    MemFile_WriteCString(memFile, "");

    if (g_snd.Initialized2d) {
        for (i = 32; i < g_snd.max_stream_channels + 32; i++)
            SND_SaveStreamChannel(i, memFile);
    }

    MemFile_WriteCString(memFile, "");
}

void SND_Restore(MemoryFile *memFile)
{
    int i;

    if (!g_snd.Initialized2d)
        return;

    for (i = 1; i < 4; i++) {
        MemFile_ReadData(memFile, 0x88, &g_snd.channelVolGroups[i]);
    }

    for (i = 0; i < 4; i++) {
        if (g_snd.channelVolGroups[i].active)
            g_snd.channelvol = &g_snd.channelVolGroups[i];
    }

    for (i = 1; i < 3; i++) {
        MemFile_ReadData(memFile, 0x20, &g_snd.envEffects[i]);
    }

    for (i = 0; i < 3; i++) {
        if (g_snd.envEffects[i].active)
            g_snd.effect = &g_snd.envEffects[i];
    }

    SND_SetRoomtype(g_snd.effect->roomtype);

    MemFile_ReadData(memFile, 8, &g_snd.background[1]);

    while (SND_Restore3DChannel(memFile))
        ;

    while (SND_Restore2DChannel(memFile))
        ;

    for (i = 0; i < 3; i++) {
        if (i != 1 && i != 2)
            SND_RestoreStreamChannel(32 + i, memFile);
    }

    while (SND_RestoreStreamChannel(-1, memFile))
        ;
}

// retail 0x44f360, unreferenced; CoD4 SND_RestoreEventually (SUPPORTED)
void SND_RestoreEventually(int size, const void *data)
{
    SND_StopSounds(SND_KEEP_MUSIC_AND_AMBIENT);
    if (g_snd.restore.buffer)
        Z_FreeInternal((void *)g_snd.restore.buffer);
    g_snd.restore.size = size;
    g_snd.restore.buffer = (byte *)Z_MallocInternal(size);
    Com_Memcpy((void *)g_snd.restore.buffer, data, size);
}

static int SND_Get2DChannelOverlay(snd_overlay_info_t *info, int maxcount)
{
    float vol;
    int rate;
    int i;

    if (maxcount > g_snd.max_2D_channels)
        maxcount = g_snd.max_2D_channels;
    for (i = 0; i < maxcount; i++) {
        if (SND_Is2DChannelFree(i + 45)) {
            info[i].pszSampleName = NULL;
        } else {
            info[i].pszSampleName = Com_GetSoundFileName(g_snd.chaninfo[45 + i].pAlias0);

            rate = SND_Get2DChannelPlaybackRate(i + 45);
            if (rate == 0)
                rate = g_snd.chaninfo[45 + i].baserate;
            info[i].fPitch = (float)rate / (float)g_snd.chaninfo[45 + i].baserate;

            info[i].fBaseVolume = g_snd.chaninfo[45 + i].basevolume;

            vol = SND_Get2DChannelVolume(i + 45);
            info[i].fCurVolume = vol;
            if (g_snd.volume != 0.0f)
                info[i].fCurVolume = vol / g_snd.volume;

            info[i].dist = -1;
        }
    }
    return maxcount;
}

static int SND_Get3DChannelOverlay(snd_overlay_info_t *info, int maxcount)
{
    vec3_t org;
    float dist;
    float vol;
    int rate;
    int i;

    if (maxcount > g_snd.max_3D_channels)
        maxcount = g_snd.max_3D_channels;
    for (i = 0; i < maxcount; i++) {
        if (SND_Is3DChannelFree(i)) {
            info[i].pszSampleName = NULL;
        } else {
            info[i].pszSampleName = Com_GetSoundFileName(g_snd.chaninfo[i].pAlias0);

            rate = SND_Get3DChannelPlaybackRate(i);
            if (rate == 0)
                rate = g_snd.chaninfo[i].baserate;
            info[i].fPitch = (float)rate / (float)g_snd.chaninfo[i].baserate;

            info[i].fBaseVolume = g_snd.chaninfo[i].basevolume;

            vol = SND_Get3DChannelVolume(i);
            info[i].fCurVolume = vol;
            if (g_snd.volume != 0.0f)
                info[i].fCurVolume = vol / g_snd.volume;

            SND_GetCurrent3DPosition(g_snd.chaninfo[i].entnum, g_snd.chaninfo[i].offset, org);
            dist = Vec3Distance(org, g_snd.listeners[0].orient.origin);
            info[i].dist = (int)dist;
        }
    }
    return maxcount;
}

static int SND_GetStreamChannelOverlay(snd_overlay_info_t *info, int maxcount)
{
    vec3_t org;
    float dist;
    float vol;
    int rate;
    int i;
    int category;
    const snd_alias_t *pAlias;

    if (maxcount > g_snd.max_stream_channels)
        maxcount = g_snd.max_stream_channels;
    for (i = 0; i < maxcount; i++) {
        if (SND_IsStreamChannelFree(i + 32)) {
            info[i].pszSampleName = NULL;
        } else {
            info[i].pszSampleName = Com_GetSoundFileName(g_snd.chaninfo[32 + i].pAlias0);

            rate = SND_GetStreamChannelPlaybackRate(i + 32);
            info[i].fPitch = (float)rate / (float)g_snd.chaninfo[32 + i].baserate;

            info[i].fBaseVolume = g_snd.chaninfo[32 + i].basevolume;

            vol = SND_GetStreamChannelVolume(i + 32);
            info[i].fCurVolume = vol;
            if (g_snd.volume != 0.0f)
                info[i].fCurVolume = vol / g_snd.volume;

            pAlias = g_snd.chaninfo[32 + i].pAlias0;
            category = (pAlias->flags & 0x780) >> 7;
            switch (category) {
            case 1:
            case 2:
            case 7:
            case 8:
            case 9:
            case 10:
                info[i].dist = -1;
                break;
            default:
                SND_GetCurrent3DPosition(g_snd.chaninfo[32 + i].entnum, g_snd.chaninfo[32 + i].offset, org);
                dist = Vec3Distance(org, g_snd.listeners[0].orient.origin);
                info[i].dist = (int)dist;
                break;
            }
        }
    }
    return maxcount;
}

int SND_GetSoundOverlay(snd_overlay_type_t type, snd_overlay_info_t *info, int maxcount, int *cpu)
{
    if (!g_snd.Initialized2d)
        return 0;

    if (cpu != NULL)
        *cpu = g_snd.cpu;

    switch (type) {
    case SND_OVERLAY_3D:
        return SND_Get3DChannelOverlay(info, maxcount);

    case SND_OVERLAY_STREAM:
        return SND_GetStreamChannelOverlay(info, maxcount);

    case SND_OVERLAY_2D:
        return SND_Get2DChannelOverlay(info, maxcount);

    default:
        return 0;
    }
}

int SND_GetListenerIndexNearestToOrigin(const vec_t *origin)
{
    return 0;
}

void SND_ErrorCleanup(void)
{
    if (g_snd.restore.buffer) {
        Z_FreeInternal((void *)g_snd.restore.buffer);
        g_snd.restore.buffer = 0;
    }
}

// retail 0x44cd50, unreferenced; name unknown (busy and not delayed)
int SND_Is2DChannelPlaying_44cd50(int index)
{
    return !SND_Is2DChannelFree(index) && !g_snd.chaninfo[index].startDelay;
}

// retail 0x44cda0, unreferenced; name unknown
int SND_Is3DChannelPlaying_44cda0(int index)
{
    return !SND_Is3DChannelFree(index) && !g_snd.chaninfo[index].startDelay;
}


