#include "common_types.h"
#include "imports.h"
#include "bytematch.h"

const dvar_t *mss_3d_provider;
const dvar_t *mss_q3fs;
extern const dvar_t *snd_bits;
extern const dvar_t *snd_enableReverb;
extern const dvar_t *snd_khz;
extern const dvar_t *snd_stereo;
extern snd_local_t g_snd;

extern float floorf(float x);

extern void FS_FCloseFile(fileHandle_t f);
extern int FS_FOpenFileReadStream(const char *filename, int *file, int uniqueFILE);
extern int FS_SeekInternal(int f, int offset, int origin);
extern int FS_FTell(int f);
extern int FS_Read(void *buffer, int len, int f);
extern int FS_ReadFile(const char *qpath, void **buffer);
extern void FS_FreeFile(void *buffer);
extern const char *FS_ShortOSFilePath(const char *name);
extern "C" __declspec(dllimport) int __stdcall AIL_digital_CPU_percent(void *dig);
extern "C" __declspec(dllimport) int __stdcall AIL_sample_playback_rate(void *S);
extern "C" __declspec(dllimport) void __stdcall AIL_set_sample_playback_rate(void *S, int rate);
extern "C" __declspec(dllimport) int __stdcall AIL_3D_sample_playback_rate(void *S);
extern "C" __declspec(dllimport) void __stdcall AIL_set_3D_sample_playback_rate(void *S, int rate);
extern "C" __declspec(dllimport) int __stdcall AIL_stream_playback_rate(void *stream);
extern "C" __declspec(dllimport) void __stdcall AIL_set_stream_playback_rate(void *stream, int rate);
extern "C" __declspec(dllimport) float __stdcall AIL_3D_sample_volume(void *S);
extern "C" __declspec(dllimport) int __stdcall AIL_3D_sample_length(void *S);
extern "C" __declspec(dllimport) void __stdcall AIL_end_sample(void *S);
extern "C" __declspec(dllimport) void __stdcall AIL_stop_sample(void *S);
extern "C" __declspec(dllimport) void __stdcall AIL_resume_sample(void *S);
extern "C" __declspec(dllimport) long unsigned int __stdcall AIL_sample_status(void *S);
extern "C" __declspec(dllimport) void __stdcall AIL_end_3D_sample(void *S);
extern "C" __declspec(dllimport) void __stdcall AIL_stop_3D_sample(void *S);
extern "C" __declspec(dllimport) void __stdcall AIL_resume_3D_sample(void *S);
extern "C" __declspec(dllimport) long unsigned int __stdcall AIL_3D_sample_status(void *S);
extern "C" __declspec(dllimport) void __stdcall AIL_close_stream(void *stream);
extern "C" __declspec(dllimport) void __stdcall AIL_pause_stream(void *stream, long int onoff);
extern "C" __declspec(dllimport) long int __stdcall AIL_stream_status(void *stream);
extern "C" __declspec(dllimport) void __stdcall AIL_sample_volume_levels(void *S, float *left_level, float *right_level);
extern "C" __declspec(dllimport) void __stdcall AIL_stream_volume_levels(void *S, float *left_level, float *right_level);
extern "C" __declspec(dllimport) void __stdcall AIL_sample_ms_position(void *S, long int *total_milliseconds, long int *current_milliseconds);
extern "C" __declspec(dllimport) void __stdcall AIL_stream_ms_position(void *S, long int *total_milliseconds, long int *current_milliseconds);
extern "C" __declspec(dllimport) void __stdcall AIL_close_3D_provider(void *lib);
extern "C" __declspec(dllimport) void __stdcall AIL_shutdown(void);
extern "C" __declspec(dllimport) void __stdcall AIL_set_stream_reverb_levels(void *S, float dry_level, float wet_level);
extern "C" __declspec(dllimport) void __stdcall AIL_sample_volume_pan(void *S, float *volume, float *pan);
extern "C" __declspec(dllimport) void __stdcall AIL_stream_volume_pan(void *S, float *volume, float *pan);
extern "C" __declspec(dllimport) long unsigned int __stdcall AIL_3D_sample_offset(void *S);
extern "C" __declspec(dllimport) void __stdcall AIL_3D_position(void *obj, float *X, float *Y, float *Z);
extern Bool SND_IsAliasChannel3D(int channel);
extern "C" __declspec(dllimport) void __stdcall AIL_release_sample_handle(void *S);
extern "C" __declspec(dllimport) void *__stdcall AIL_allocate_sample_handle(void *dig);
extern "C" __declspec(dllimport) void *__stdcall AIL_allocate_3D_sample_handle(void *provider);
extern "C" __declspec(dllimport) void __stdcall AIL_init_sample(void *S);
extern "C" __declspec(dllimport) void __stdcall AIL_set_sample_type(void *S, int format, int flags);
extern "C" __declspec(dllimport) void __stdcall AIL_set_sample_volume_levels(void *S, float left, float right);
extern "C" __declspec(dllimport) void __stdcall AIL_set_stream_volume_levels(void *S, float left, float right);
extern "C" __declspec(dllimport) int __stdcall AIL_minimum_sample_buffer_size(void *dig, int rate, int format);
extern "C" __declspec(dllimport) void __stdcall AIL_set_sample_address(void *S, void *ptr, int len);
extern "C" __declspec(dllimport) void __stdcall AIL_set_sample_adpcm_block_size(void *S, int blocksize);
extern "C" __declspec(dllimport) void __stdcall AIL_set_sample_loop_count(void *S, int count);
extern "C" __declspec(dllimport) void __stdcall AIL_set_sample_reverb_levels(void *S, float dry_level, float wet_level);
extern "C" __declspec(dllimport) void __stdcall AIL_set_sample_ms_position(void *S, int ms);
extern "C" __declspec(dllimport) long unsigned int __stdcall AIL_sample_position(void *S);
extern "C" __declspec(dllimport) void __stdcall AIL_set_3D_sample_volume(void *S, float volume);
extern "C" __declspec(dllimport) void __stdcall AIL_set_3D_sample_distances(void *S, float max_dist, float min_dist);
extern "C" __declspec(dllimport) void __stdcall AIL_set_3D_sample_loop_count(void *S, int count);
extern "C" __declspec(dllimport) void __stdcall AIL_set_3D_sample_effects_level(void *S, float level);
extern "C" __declspec(dllimport) void __stdcall AIL_set_3D_sample_info(void *S, void *info);
extern "C" __declspec(dllimport) void __stdcall AIL_set_3D_sample_offset(void *S, int offset);
extern "C" __declspec(dllimport) void __stdcall AIL_set_3D_position(void *S, float x, float y, float z);
extern "C" __declspec(dllimport) void __stdcall AIL_set_stream_loop_count(void *S, int count);
extern "C" __declspec(dllimport) void __stdcall AIL_set_stream_ms_position(void *S, int ms);
extern "C" __declspec(dllimport) void *__stdcall AIL_open_stream(void *dig, const char *filename, int flags);
extern "C" __declspec(dllimport) void __stdcall AIL_stream_info(void *stream, int *a, int *b, int *c, int *d);
extern "C" __declspec(dllimport) int __stdcall AIL_sample_buffer_ready(void *S);
extern "C" __declspec(dllimport) void __stdcall AIL_load_sample_buffer(void *S, int buf_num, void *buffer, int size);
extern "C" __declspec(dllimport) int __stdcall AIL_WAV_info(const void *data, AILSOUNDINFO *info);
extern "C" __declspec(dllimport) int __stdcall AIL_size_processed_digital_audio(unsigned int rate, unsigned int format, int num, const AILMIXINFO *info);
extern "C" __declspec(dllimport) int __stdcall AIL_process_digital_audio(void *dest, int size, unsigned int rate, unsigned int format, int num, AILMIXINFO *info);
extern "C" __declspec(dllimport) void __stdcall AIL_set_digital_master_room_type(void *dig, int room_type);
extern "C" __declspec(dllimport) void __stdcall AIL_set_DirectSound_HWND(void *dig, HWND wnd);
extern "C" __declspec(dllimport) void __stdcall AIL_set_digital_master_reverb_levels(void *dig, float dry, float wet);
extern "C" __declspec(dllimport) void __stdcall AIL_set_3D_room_type(void *provider, int room_type);
extern "C" __declspec(dllimport) int __stdcall AIL_startup(void);
// mss.h: register AIL_shutdown with atexit on first startup (MSS_auto_cleanup lives in mss32.lib)
extern "C" int __cdecl MSS_auto_cleanup(void);
#define AIL_startup() (MSS_auto_cleanup(), AIL_startup())
extern "C" __declspec(dllimport) void __stdcall AIL_set_redist_directory(const char *dir);
extern "C" __declspec(dllimport) void *__stdcall AIL_open_digital_driver(int freq, int bits, int channels, int flags);
extern "C" __declspec(dllimport) int __stdcall AIL_set_preference(int pref, int value);
extern "C" __declspec(dllimport) int __stdcall AIL_enumerate_3D_providers(void *provider, void **handle, const char **name);
extern "C" __declspec(dllimport) int __stdcall AIL_open_3D_provider(void *handle);
extern "C" __declspec(dllimport) void __stdcall AIL_3D_provider_attribute(void *provider, const char *name, void *val);
extern "C" __declspec(dllimport) void __stdcall AIL_set_3D_distance_factor(void *provider, float factor);
extern "C" __declspec(dllimport) void __stdcall AIL_set_3D_rolloff_factor(void *provider, float factor);
extern "C" __declspec(dllimport) void __stdcall AIL_set_file_callbacks(void *open_cb, void *close_cb, void *seek_cb, void *read_cb);
extern "C" __declspec(dllimport) const char *__stdcall AIL_last_error(void);
extern "C" void *Z_MallocInternal( int size );
extern "C" void Z_FreeInternal( void *ptr );
extern void *Hunk_AllocInternal(int size);
extern void Com_Error(errorParm_t code, const char *fmt, ...);
extern void Com_Printf(const char *fmt, ...);
extern void Com_DPrintf(const char *fmt, ...);
extern "C" void Com_Memcpy( void *dest, const void *src, int count );
extern void *Com_GetSoundFileMem(const snd_alias_t *alias);
extern const char *Com_GetSoundFileName(const snd_alias_t *alias);
extern void SND_Update(void);
extern Bool SND_UpdateBackgroundVolume(int streamIdx, int frametime);
extern float SND_Attenuate(SndCurve *volumeFalloffCurve, float radius, float mindist, float maxdist);
extern float SND_GetLerpedSlavePercentage(float slave);
extern int SND_GetListenerIndexNearestToOrigin(const float *org);
extern void SND_GetCurrent3DPosition(int entnum, const vec_t *offset, vec_t *pos_out);
extern vec_t Vec3Normalize( vec3_t v );
extern float Vec3Distance(const vec3_t v1, const vec3_t v2);
extern float Vec3DistanceSq(const vec3_t v1, const vec3_t v2);

static long unsigned int __stdcall MSS_FileOpenCallback(const char *pszFilename, long unsigned int *phFileHandle);
static void __stdcall MSS_FileCloseCallback(long unsigned int hFileHandle);
static long int __stdcall MSS_FileSeekCallback(long unsigned int hFileHandle, long int offset, long unsigned int type);
static long unsigned int __stdcall MSS_FileReadCallback(long unsigned int hFileHandle, void *pBuffer, long unsigned int bytes);
void SND_ShutdownDriver(void);
int SND_GetDriverCPUPercentage(void);
void SND_Stop2DChannel(int index);
void SND_Pause2DChannel(int index);
void SND_Unpause2DChannel(int index, int timeshift);
Bool SND_Is2DChannelFree(int index);
void SND_Stop3DChannel(int index);
void SND_Pause3DChannel(int index);
void SND_Unpause3DChannel(int index, int timeshift);
Bool SND_Is3DChannelFree(int index);
void SND_StopStreamChannel(int index);
void SND_PauseStreamChannel(int index);
void SND_UnpauseStreamChannel(int index, int timeshift);
Bool SND_IsStreamChannelFree(int index);
float SND_Get2DChannelVolume(int index);
float SND_Get3DChannelVolume(int index);
float SND_GetStreamChannelVolume(int index);
int SND_Get2DChannelPlaybackRate(int index);
void SND_Set2DChannelPlaybackRate(int index, int rate);
int SND_Get3DChannelPlaybackRate(int index);
void SND_Set3DChannelPlaybackRate(int index, int rate);
int SND_GetStreamChannelPlaybackRate(int index);
void SND_SetStreamChannelPlaybackRate(int index, int rate);
void SND_UpdateStreamChannelReverb(int index);
int SND_Get2DChannelLength(int index);
int SND_Get3DChannelLength(int index);
int SND_GetStreamChannelLength(int index);
void SND_Get2DChannelSaveInfo(int index, snd_save_2D_sample_t *info);
void SND_Get3DChannelSaveInfo(int index, snd_save_3D_sample_t *info);
void SND_GetStreamChannelSaveInfo(int index, snd_save_stream_t *info);
void SND_EndRawSamples(void);
int SND_RawSamplesTime(void);
void SND_RawSamples(int samples, int rate, int width, int s_channels, const byte *data);
int SND_GetSoundFileSize(const void *pSoundFile);
void SND_DriverPreUpdate(int frametime);
void SND_SetRoomtype(int roomtype);
void SND_Update2DChannelReverb(int index);
void SND_Update3DChannelReverb(int index);
void SND_Set3DPosition(int index, const vec_t *org);
static void MSS_SpatializeStream(float *volume, float *pan);
static void MSS_SpatializeStreamImpl(int i, float *volume, float *pan);
void SND_SetStreamChannelVolumePan(int index, float volume, float pan);
void SND_UpdateStreamChannel(int i, int frametime);
void SND_Update2DChannel(int i, int frametime);
void SND_Set2DChannelVolumePan(int index, float volume, float pan);
MssSound *SND_LoadFromBuffer(void *buffer, const char *realname, MssSound *sound);
void SND_LoadSoundFile(SoundFile *soundFile);
int SND_StartAlias2DSample(const snd_alias_t *pAlias0, const snd_alias_t *pAlias1, float lerp, int entnum, float volume, float pitch, int timeshift, float fraction, int startDelay, Bool master, int *pChannel, snd_alias_system_t system);
int SND_StartAlias3DSample(const snd_alias_t *pAlias0, const snd_alias_t *pAlias1, float lerp, int entnum, const vec_t *org, float volume, float pitch, int timeshift, float fraction, int startDelay, Bool master, int *pChannel, snd_alias_system_t system);
int SND_StartAliasStreamOnChannel(const snd_alias_t *pAlias0, const snd_alias_t *pAlias1, float lerp, int entnum, const vec_t *org, float volume, float pitch, int timeshift, float fraction, int startDelay, Bool master, int index, snd_alias_system_t system);
void SND_Update3DChannel(int i, int frametime);
void SND_Set2DChannelFromSaveInfo(int index, snd_save_2D_sample_t *info);
Bool SND_InitDriver(void);
void SND_SetStreamChannelFromSaveInfo(int index, snd_save_stream_t *info);
void SND_DriverPostUpdate(void);

extern int SND_FindFree2DChannel(int entnum, int channel);
extern int SND_FindFree3DChannel(int entnum, int channel);
extern void SND_SetChannelInfo(int index, int entnum, const snd_alias_t *pAlias0, const snd_alias_t *pAlias1, float lerp, const vec_t *origin, float volume, float pitch, int srcChannelCount, int baserate, int total_msec, int start_msec, int startDelay, Bool master, snd_alias_system_t system);
extern dvar_t *Dvar_RegisterBool(const char *dvarName, bool value, unsigned short flags);
extern dvar_t *Dvar_RegisterString(const char *dvarName, const char *value, unsigned short flags);
extern void Dvar_SetString(dvar_t *dvar, const char *value);
extern int Dvar_GetInt(const char *name);
extern int stricmp(const char *a, const char *b);
extern const dvar_t *mss_3d_provider;
extern const dvar_t *mss_q3fs;
static struct MssLocal milesGlob;
static long unsigned int __stdcall MSS_FileOpenCallback(const char *pszFilename, long unsigned int *phFileHandle);
static void __stdcall MSS_FileCloseCallback(long unsigned int hFileHandle);
static long int __stdcall MSS_FileSeekCallback(long unsigned int hFileHandle, long int offset, long unsigned int type);
static long unsigned int __stdcall MSS_FileReadCallback(long unsigned int hFileHandle, void *pBuffer, long unsigned int bytes);
void SND_ShutdownDriver(void);
int SND_GetDriverCPUPercentage(void);
void SND_Stop2DChannel(int index);
void SND_Pause2DChannel(int index);
void SND_Unpause2DChannel(int index, int timeshift);
Bool SND_Is2DChannelFree(int index);
void SND_Stop3DChannel(int index);
void SND_Pause3DChannel(int index);
void SND_Unpause3DChannel(int index, int timeshift);
Bool SND_Is3DChannelFree(int index);
void SND_StopStreamChannel(int index);
void SND_PauseStreamChannel(int index);
void SND_UnpauseStreamChannel(int index, int timeshift);
Bool SND_IsStreamChannelFree(int index);
float SND_Get2DChannelVolume(int index);
float SND_Get3DChannelVolume(int index);
float SND_GetStreamChannelVolume(int index);
int SND_Get2DChannelPlaybackRate(int index);
void SND_Set2DChannelPlaybackRate(int index, int rate);
int SND_Get3DChannelPlaybackRate(int index);
void SND_Set3DChannelPlaybackRate(int index, int rate);
int SND_GetStreamChannelPlaybackRate(int index);
void SND_SetStreamChannelPlaybackRate(int index, int rate);
void SND_UpdateStreamChannelReverb(int index);
int SND_Get2DChannelLength(int index);
int SND_Get3DChannelLength(int index);
int SND_GetStreamChannelLength(int index);
void SND_Get2DChannelSaveInfo(int index, snd_save_2D_sample_t *info);
void SND_Get3DChannelSaveInfo(int index, snd_save_3D_sample_t *info);
void SND_GetStreamChannelSaveInfo(int index, snd_save_stream_t *info);
void SND_EndRawSamples(void);
int SND_RawSamplesTime(void);
void SND_RawSamples(int samples, int rate, int width, int s_channels, const byte *data);
int SND_GetSoundFileSize(const void *pSoundFile);
void SND_DriverPreUpdate(int frametime);
void SND_SetRoomtype(int roomtype);
void SND_Update2DChannelReverb(int index);
void SND_Update3DChannelReverb(int index);
void SND_Set3DPosition(int index, const vec_t *org);
static void MSS_SpatializeStream(float *volume, float *pan);
static void MSS_SpatializeStreamImpl(int i, float *volume, float *pan);
void SND_SetStreamChannelVolumePan(int index, float volume, float pan);
void SND_UpdateStreamChannel(int i, int frametime);
void SND_Set2DChannelVolumePan(int index, float volume, float pan);
void SND_Update2DChannel(int i, int frametime);
void SND_LoadSoundFile(SoundFile *soundFile);
int SND_StartAlias2DSample(const snd_alias_t *pAlias0, const snd_alias_t *pAlias1, float lerp, int entnum, float volume, float pitch, int timeshift, float fraction, int startDelay, Bool master, int *pChannel, snd_alias_system_t system);
int SND_StartAlias3DSample(const snd_alias_t *pAlias0, const snd_alias_t *pAlias1, float lerp, int entnum, const vec_t *org, float volume, float pitch, int timeshift, float fraction, int startDelay, Bool master, int *pChannel, snd_alias_system_t system);
int SND_StartAliasStreamOnChannel(const snd_alias_t *pAlias0, const snd_alias_t *pAlias1, float lerp, int entnum, const vec_t *org, float volume, float pitch, int timeshift, float fraction, int startDelay, Bool master, int index, snd_alias_system_t system);
void SND_Update3DChannel(int i, int frametime);
void SND_Set2DChannelFromSaveInfo(int index, snd_save_2D_sample_t *info);
Bool SND_InitDriver(void);
void SND_SetStreamChannelFromSaveInfo(int index, snd_save_stream_t *info);
void SND_DriverPostUpdate(void);

// universal/com_math.h inlines
static inline int I_sel(const int x, const int y, const int z)
{
    if (x >= 0) {
        return y;
    }
    return z;
}

static inline float DotProduct(const vec3_t a, const vec3_t b)
{
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}




void *Hunk_AllocAlignInternal(int size, int align);















// retail 0x4b2580, unreferenced; name unknown
void *MSS_HunkAlloc_4b2580(int size)
{
    return Hunk_AllocAlignInternal(size, 32);
}

static long unsigned int __stdcall MSS_FileOpenCallback(const char *pszFilename, long unsigned int *phFileHandle)
{

    return FS_FOpenFileReadStream(pszFilename, (int *)phFileHandle, 1) >= 0;
}

static void __stdcall MSS_FileCloseCallback(long unsigned int hFileHandle)
{
    FS_FCloseFile((int)hFileHandle);
}

static long int __stdcall MSS_FileSeekCallback(long unsigned int hFileHandle, long int offset, long unsigned int type)
{
    int origin;

    switch (type) {
    case 2:
        origin = 1;
        break;
    case 1:
        origin = 0;
        break;
    case 0:
        origin = 2;
        break;
    default:
        return 0;
    }
    FS_SeekInternal((int)hFileHandle, offset, origin);
    return FS_FTell((int)hFileHandle);
}

static long unsigned int __stdcall MSS_FileReadCallback(long unsigned int hFileHandle, void *pBuffer, long unsigned int bytes)
{
    return FS_Read(pBuffer, (int)bytes, (int)hFileHandle);
}

// retail 0x4b2670, unreferenced; name unknown
HDIGDRIVER MSS_OpenDigitalDriver_4b2670(int hertz, int bits, int channels)
{
    AIL_set_preference(1, 53);
    return (HDIGDRIVER)AIL_open_digital_driver(hertz, bits, channels, 0);
}

// retail 0x4b26a0, unreferenced; KisakCOD MSS_GetDryLevel (SUPPORTED)
float MSS_GetDryLevel(void)
{
    return 1.0f;
}

static float MSS_GetWetLevel(const snd_alias_t *pAlias)
{
    if (!pAlias)
        return g_snd.effect->wetlevel;

    if (!((dvar_t *)snd_enableReverb)->current.enabled || (pAlias->flags & 0x10))
        return 0.0f;

    return g_snd.effect->wetlevel;
}

void MSS_InitFailed(void)
{
    if (Dvar_GetInt("r_vc_compile") != 2)
        Com_Printf("Miles sound system initialization failed\n");
}

bool MSS_Init(void)
{
    int hertz;
    int bytes;
    int channels;

    switch (((dvar_t *)snd_khz)->current.integer) {
    case 11:
        hertz = 11025;
        break;
    case 44:
        hertz = 44100;
        break;
    default:
        Com_Printf("invalid value %i for snd_khz, using 22 khz instead\n", ((dvar_t *)snd_khz)->current.integer);
    case 22:
        hertz = 22050;
        break;
    }

    switch (((dvar_t *)snd_bits)->current.integer) {
    case 8:
        bytes = 1;
        break;
    default:
        Com_Printf("invalid value %i for snd_bits (should be 8 or 16), using 16 instead\n", ((dvar_t *)snd_bits)->current.integer);
    case 16:
        bytes = 2;
        break;
    }

    channels = (((dvar_t *)snd_stereo)->current.enabled != 0) + 1;
    Com_Printf("Attempting %i kHz %i bit %s sound\n", hertz / 1000, bytes * 8, channels == 2 ? "stereo" : "mono");

    AIL_set_preference(1, 53);
    milesGlob.driver_2D = (HDIGDRIVER)AIL_open_digital_driver(hertz, bytes, channels, 0);
    if (!milesGlob.driver_2D) {
        Com_Printf("couldn't initialize 2D provider: %s\n", AIL_last_error());
        return 0;
    }

    g_snd.Initialized2d = 1;
    g_snd.max_2D_channels = 8;
    g_snd.max_stream_channels = 13;
    g_snd.playback_rate = hertz + hertz / 2;
    if (g_snd.playback_rate >= 44100)
        g_snd.playback_rate = 0x7FFFFFFF;
    g_snd.playback_channels = channels;
    g_snd.playback_bits = bytes * 8;
    g_snd.timescale = 1.0f;
    return 1;
}

// Original name unknown.
static bool MSS_Open3DProvider(const char *wantedName, bool verbose)
{
    void *providerIter;
    void *handle;
    const char *name;
    void *wantedHandle;
    void *defaultHandle;

    if (verbose)
        Com_Printf("available 3D providers:\n");
    wantedHandle = NULL;
    defaultHandle = NULL;
    providerIter = NULL;
    while (AIL_enumerate_3D_providers(&providerIter, &handle, &name)) {
        if (verbose)
            Com_Printf("  %s\n", name);
        if (!stricmp(name, wantedName))
            wantedHandle = handle;
        if (!stricmp(name, "Miles Fast 2D Positional Audio"))
            defaultHandle = handle;
    }

    milesGlob.provider_3D = NULL;
    if (wantedHandle) {
        if (!AIL_open_3D_provider(wantedHandle)) {
            Com_Printf("using 3D provider '%s'\n", wantedName);
            milesGlob.provider_3D = (HPROVIDER)wantedHandle;
        } else {
            Com_Printf("couldn't open 3D provider '%s': %s\n", wantedName, AIL_last_error());
        }
    }

    if (!milesGlob.provider_3D) {
        if (!defaultHandle || defaultHandle == wantedHandle)
            return 0;
        if (wantedName[0] && stricmp(wantedName, "Miles Fast 2D Positional Audio"))
            Com_Printf("trying to use 'Miles Fast 2D Positional Audio' instead of '%s'\n", wantedName);
        if (!AIL_open_3D_provider(defaultHandle)) {
            Com_Printf("using 3D provider '%s'\n", "Miles Fast 2D Positional Audio");
            milesGlob.provider_3D = (HPROVIDER)defaultHandle;
            Dvar_SetString((dvar_t *)mss_3d_provider, "Miles Fast 2D Positional Audio");
        } else {
            Com_Printf("couldn't open 3D provider '%s': %s\n", "Miles Fast 2D Positional Audio", AIL_last_error());
        }
        if (!milesGlob.provider_3D)
            return 0;
    }

    g_snd.Initialized3d = 1;
    AIL_3D_provider_attribute((void *)milesGlob.provider_3D, "Maximum supported samples", &g_snd.max_3D_channels);
    if (g_snd.max_3D_channels > 32)
        g_snd.max_3D_channels = 32;
    Com_Printf("%i max 3D channels\n", g_snd.max_3D_channels);
    AIL_set_3D_distance_factor((void *)milesGlob.provider_3D, 0.0254f);
    AIL_set_3D_rolloff_factor((void *)milesGlob.provider_3D, 0.0f);
    return 1;
}

void MSS_InitChannels(void)
{
    int i;

    for (i = 0; i < g_snd.max_2D_channels; i++) {
        milesGlob.handle_2D[i] = (HSAMPLE)AIL_allocate_sample_handle((void *)milesGlob.driver_2D);
        if (!milesGlob.handle_2D[i])
            Com_Error(ERR_DROP, "\x15MILES 2D sound sample allocation failed on channel %i\n", i + 1);
    }
    for (i = 0; i < g_snd.max_3D_channels; i++) {
        milesGlob.handle_3D[i] = AIL_allocate_3D_sample_handle((void *)milesGlob.provider_3D);
        if (!milesGlob.handle_3D[i])
            Com_Error(ERR_DROP, "\x15MILES 3D sound sample allocation failed on channel %i\n", i + 1);
    }
    g_snd.ambient_track = 2;
}

bool MSS_Startup(void)
{
    bool started;

    started = AIL_startup() != 0;
    _controlfp(_PC_24, _MCW_PC);
    _controlfp(_MCW_EM, _MCW_EM);
    return started;
}

void MSS_ShutdownCleanup(void)
{
    memset(&milesGlob, 0, sizeof(milesGlob));
}

Bool SND_InitDriver(void)
{
    mss_q3fs = Dvar_RegisterBool("mss_q3fs", 1, 0x1020);
    if (mss_q3fs->current.enabled)
        AIL_set_file_callbacks((void *)MSS_FileOpenCallback, (void *)MSS_FileCloseCallback, (void *)MSS_FileSeekCallback, (void *)MSS_FileReadCallback);
    AIL_set_redist_directory("miles");
    if (!MSS_Startup()) {
        MSS_InitFailed();
        return 0;
    }

    mss_3d_provider = Dvar_RegisterString("mss_3d_provider", "Miles Fast 2D Positional Audio", 0x1021);
    if (!MSS_Init()) {
        AIL_shutdown();
        MSS_ShutdownCleanup();
        MSS_InitFailed();
        return 0;
    }
    if (!MSS_Open3DProvider(mss_3d_provider->current.string, 1)) {
        AIL_shutdown();
        MSS_ShutdownCleanup();
        MSS_InitFailed();
        return 0;
    }
    MSS_InitChannels();
    return 1;
}

void SND_ShutdownDriver(void)
{
    AIL_close_3D_provider( (void *)(milesGlob.provider_3D));
    AIL_shutdown();
    memset(&milesGlob, 0, sizeof(MssLocal));
}

int SND_GetDriverCPUPercentage(void)
{
    return AIL_digital_CPU_percent( (void *)(milesGlob.driver_2D));
}

static void MSS_SpatializeStreamImpl(int i, float *volume, float *pan)
{
    const snd_alias_t *pAlias0, *pAlias1;
    int listenerIdx;
    snd_listener *listener;
    float *right;
    float delta[3];
    float dist, dot;
    float lerp, oneMinusLerp;
    float fDistMin, fDistMax;

    pAlias0 = g_snd.chaninfo[32 + i].pAlias0;
    pAlias1 = g_snd.chaninfo[32 + i].pAlias1;

    listenerIdx = SND_GetListenerIndexNearestToOrigin(g_snd.chaninfo[32 + i].org);
    listener = &g_snd.listeners[listenerIdx];

    delta[0] = g_snd.chaninfo[32 + i].org[0] - listener->orient.origin[0];
    delta[1] = g_snd.chaninfo[32 + i].org[1] - listener->orient.origin[1];
    delta[2] = g_snd.chaninfo[32 + i].org[2] - listener->orient.origin[2];

    dist = Vec3Normalize(delta);

    right = listener->orient.axis[1];
    dot = right[2] * delta[2] + right[1] * delta[1] + right[0] * delta[0];

    lerp = g_snd.chaninfo[32 + i].lerp;
    oneMinusLerp = 1.0f - lerp;

    fDistMax = oneMinusLerp * pAlias0->fDistMax + lerp * pAlias1->fDistMax;
    fDistMin = oneMinusLerp * pAlias0->fDistMin + lerp * pAlias1->fDistMin;

    *volume *= SND_Attenuate(pAlias0->volumeFalloffCurve, dist, fDistMin, fDistMax);
    *pan = (1.0f - dot) * 0.5f;
}

void SND_Set3DPosition(int index, const vec_t *org)
{
    vec3_t delta;
    vec3_t pos;

    // 1.0 always spatializes against listener 0
    delta[0] = org[0] - g_snd.listeners[0].orient.origin[0];
    delta[1] = org[1] - g_snd.listeners[0].orient.origin[1];
    delta[2] = org[2] - g_snd.listeners[0].orient.origin[2];

    pos[0] = DotProduct(delta, g_snd.listeners[0].orient.axis[0]);
    pos[1] = DotProduct(delta, g_snd.listeners[0].orient.axis[1]);
    pos[2] = DotProduct(delta, g_snd.listeners[0].orient.axis[2]);

    AIL_set_3D_position(milesGlob.handle_3D[index], -pos[1], pos[2], pos[0]);
}

void SND_SetHWND(HWND hwnd)
{
    if (g_snd.Initialized2d)
        AIL_set_DirectSound_HWND( (void *)(milesGlob.driver_2D), hwnd);
}

void SND_StopStreamChannel(int index)
{
    AIL_close_stream(milesGlob.handle_stream[index - 32]);
    milesGlob.handle_stream[index - 32] = NULL;
    g_snd.chaninfo[index].paused = 0;
    g_snd.chaninfo[index].startDelay = 0;
}

void SND_PauseStreamChannel(int index)
{
    snd_local_t *sndGlob;
    snd_channel_info_t *chaninfo;
    AIL_pause_stream(milesGlob.handle_stream[index - 0x20], 1);
    sndGlob = &g_snd;
    chaninfo = &g_snd.chaninfo[index];
    chaninfo->paused = 1;
}

void SND_UnpauseStreamChannel(int index, int timeshift)
{
    if (g_snd.chaninfo[index].startDelay == 0) {
        AIL_pause_stream(milesGlob.handle_stream[index - 0x20], 0);
    }
    g_snd.chaninfo[index].endtime += timeshift;
    g_snd.chaninfo[index].paused = 0;
}

Bool SND_IsStreamChannelFree(int index)
{
    if (!milesGlob.handle_stream[index - 32])
        return 1;
    if (g_snd.chaninfo[index].paused)
        return 0;
    if (g_snd.chaninfo[index].startDelay)
        return 0;
    if (AIL_stream_status(milesGlob.handle_stream[index - 32]) == 2) {
        AIL_close_stream(milesGlob.handle_stream[index - 32]);
        milesGlob.handle_stream[index - 32] = NULL;
        return 1;
    }
    return 0;
}

// Retail 0x4b30f0; historical name unknown.
int SND_GetSampleType_4b30f0(int format, int channels, int bits)
{
    if (channels == 1) {
        if (format == 0x11)
            return 5;
        return bits > 8;
    }
    if (format == 0x11)
        return 7;
    return (bits > 8) + 2;
}

int SND_StartAlias2DSample(const snd_alias_t *pAlias0, const snd_alias_t *pAlias1, float lerp, int entnum, float volume, float pitch, int timeshift, float fraction, int startDelay, Bool master, int *pChannel, snd_alias_system_t system)
{
    int index, channel, aliasFlags, sampleType;
    void *handle;
    byte *sound;
    float scaledVol, volMul;
    long int total_msec;
    int start_msec;
    int channels, bits, format;
    float reverbLevel;

    index = SND_FindFree2DChannel(entnum, (pAlias0->flags & 0x780) >> 7);
    if (pChannel != NULL) {
        *pChannel = index;
    }
    if (index < 0) {
        return 0;
    }

    handle = milesGlob.handle_2D[index - 45];
    sound = (byte *)Com_GetSoundFileMem(pAlias0);
    AIL_init_sample(handle);

    channels = (int)((MssSound *)sound)->info.channels;
    bits = (int)((MssSound *)sound)->info.bits;
    format = (int)((MssSound *)sound)->info.format;

    sampleType = SND_GetSampleType_4b30f0(format, channels, bits);

    AIL_set_sample_type(handle, sampleType, 0);
    AIL_set_sample_address(handle, (void *)((MssSound *)sound)->info.data_ptr, (int)((MssSound *)sound)->info.data_len);
    AIL_set_sample_adpcm_block_size(handle, (int)((MssSound *)sound)->info.block_size);

    AIL_set_sample_playback_rate(handle, I_fround((float)((MssSound *)sound)->info.rate * g_snd.timescale * pitch));

    aliasFlags = pAlias0->flags;
    channel = (aliasFlags & 0x780) >> 7;
    scaledVol = g_snd.volume * ((float *)g_snd.channelvol)[channel * 3] * volume;

    if (g_snd.slaveLerp != 0.0f && !master && (aliasFlags & 4)) {
        scaledVol *= SND_GetLerpedSlavePercentage(pAlias0->fSlavePercentage);
    }

    volMul = (((MssSound *)sound)->info.channels == 1) ? 0.5f : 1.0f;
    AIL_set_sample_volume_levels(handle, volMul * scaledVol, volMul * scaledVol);

    AIL_set_sample_loop_count(handle, ~pAlias0->flags & 1);

    if ((((dvar_t *)snd_enableReverb))->current.enabled != 0 && !((byte)(pAlias0->flags) & 0x10)) {
        reverbLevel = g_snd.effect->wetlevel;
    } else {
        reverbLevel = 0;
    }
    AIL_set_sample_reverb_levels(handle, 1.0f, reverbLevel);

    AIL_sample_ms_position(handle, &total_msec, NULL);
    if (timeshift >= total_msec) {
        return 0;
    }

    if (fraction != 0.0f) {
        start_msec = I_fround((float)total_msec * fraction);
    } else {
        start_msec = timeshift;
    }

    if (start_msec != 0)
        startDelay = 0;

    AIL_set_sample_ms_position(handle, start_msec);

    if (!startDelay) {
        if (!g_snd.paused || !g_snd.pauseSettings[(pAlias0->flags & 0x780) >> 7])
            AIL_resume_sample(handle);
    }

    total_msec += startDelay;
    if (pAlias0->flags & 1)
        total_msec = 0;

    SND_SetChannelInfo(index, entnum, pAlias0, pAlias1, lerp, NULL, volume, pitch, (int)((MssSound *)sound)->info.channels, (int)((MssSound *)sound)->info.rate, total_msec, start_msec, startDelay, master, system);
    return (int)total_msec;
}

int SND_StartAlias3DSample(const snd_alias_t *pAlias0, const snd_alias_t *pAlias1, float lerp, int entnum, const vec_t *org, float volume, float pitch, int timeshift, float fraction, int startDelay, Bool master, int *pChannel, snd_alias_system_t system)
{
    int index, channel, aliasFlags;
    void *handle;
    byte *sound;
    float scaledVol;
    float fDistMin, fDistMax, oneMinusLerp;
    float dist, attenuation;
    int playbackRate, total_msec, start_msec;

    index = SND_FindFree3DChannel(entnum, (pAlias0->flags & 0x780) >> 7);
    if (pChannel)
        *pChannel = index;
    if (index < 0)
        return 0;

    handle = milesGlob.handle_3D[index];
    sound = (byte *)Com_GetSoundFileMem(pAlias0);

    oneMinusLerp = 1.0f - lerp;
    fDistMin = oneMinusLerp * pAlias0->fDistMin + lerp * pAlias1->fDistMin;
    fDistMax = oneMinusLerp * pAlias0->fDistMax + lerp * pAlias1->fDistMax;

    AIL_set_3D_sample_info(handle, sound);

    {
        int listenerIdx = SND_GetListenerIndexNearestToOrigin(org);
        dist = Vec3Distance(org, g_snd.listeners[listenerIdx].orient.origin);
    }
    attenuation = SND_Attenuate(pAlias0->volumeFalloffCurve, dist, fDistMin, fDistMax);

    aliasFlags = pAlias0->flags;
    channel = (aliasFlags & 0x780) >> 7;
    scaledVol = volume * attenuation * ((float *)g_snd.channelvol)[channel * 3];
    scaledVol = scaledVol * g_snd.volume;

    if (g_snd.slaveLerp != 0.0f && !master && (aliasFlags & 4)) {
        scaledVol *= SND_GetLerpedSlavePercentage(pAlias0->fSlavePercentage);
    }

    AIL_set_3D_sample_volume(handle, pow(scaledVol, 0.6f));
    AIL_set_3D_sample_distances(handle, pAlias0->fDistMax, pAlias0->fDistMin);

    playbackRate = AIL_3D_sample_playback_rate(handle);
    playbackRate = I_fround((float)playbackRate * pitch * g_snd.timescale);
    AIL_set_3D_sample_playback_rate(handle, playbackRate);
    SND_Set3DPosition(index, org);

    AIL_set_3D_sample_loop_count(handle, ~pAlias0->flags & 1);
    AIL_set_3D_sample_effects_level(handle, g_snd.effect->wetlevel);

    total_msec = I_fround((float)(((MssSound *)sound)->info.samples * 1000) * g_snd.timescale / playbackRate);

    if (timeshift >= total_msec) {
        return 0;
    }

    if (fraction != 0.0f) {
        start_msec = I_fround((float)total_msec * fraction);
    } else {
        start_msec = timeshift;
    }

    if (start_msec == 0) {

    } else {
        startDelay = 0;
    }

    AIL_set_3D_sample_offset(handle, I_fround((float)((MssSound *)sound)->info.data_len * fraction));

    if (!startDelay) {
        if (!g_snd.paused || !g_snd.pauseSettings[(pAlias0->flags & 0x780) >> 7])
            AIL_resume_3D_sample(handle);
    }

    total_msec += startDelay;
    if (pAlias0->flags & 1)
        total_msec = 0;

    SND_SetChannelInfo(index, entnum, pAlias0, pAlias1, lerp, org, volume, pitch, (int)((MssSound *)sound)->info.channels, (int)((MssSound *)sound)->info.rate, total_msec, start_msec, startDelay, master, system);
    return total_msec;
}

int SND_StartAliasStreamOnChannel(const snd_alias_t *pAlias0, const snd_alias_t *pAlias1, float lerp, int entnum, const vec_t *org, float volume, float pitch, int timeshift, float fraction, int startDelay, Bool master, int index, snd_alias_system_t system)
{
    HSTREAM handle;
    char realname[256];
    int filetype;
    int srcChannelCount;
    int baserate;
    long int total_msec;
    int start_msec;
    float realVolume;
    float pan;
    float reverbLevel;
    float *to;

    if (!pAlias0->soundFile->isStreamFound) {
        Com_DPrintf("Tried to play streamed sound '%s' from alias '%s', but it was not found at load time.\n", Com_GetSoundFileName(pAlias0), pAlias0->pszAliasName);
        return 0;
    }

    if (milesGlob.handle_stream[index - 32]) {
        AIL_close_stream(milesGlob.handle_stream[index - 32]);
        milesGlob.handle_stream[index - 32] = NULL;
    }

    sprintf(realname, "sound/%s", Com_GetSoundFileName(pAlias0));
    if (mss_q3fs->current.enabled)
        handle = (HSTREAM)AIL_open_stream((void *)milesGlob.driver_2D, realname, 0);
    else {
        const char *filename = FS_ShortOSFilePath(realname);
        handle = (HSTREAM)AIL_open_stream((void *)milesGlob.driver_2D, filename, 0);
    }
    if (!handle) {
        Com_Printf("Couldn't play stream '%s' from alias '%s' - %s\n", realname, pAlias0->pszAliasName, AIL_last_error());
        return 0;
    }

    milesGlob.handle_stream[index - 32] = handle;
    AIL_stream_info(handle, NULL, &filetype, NULL, NULL);
    srcChannelCount = (filetype & 2) ? 2 : 1;
    baserate = AIL_stream_playback_rate(handle);
    AIL_set_stream_playback_rate(handle, I_fround((float)baserate * g_snd.timescale * pitch));

    realVolume = g_snd.volume * g_snd.channelvol->channelvol[(pAlias0->flags & 0x780) >> 7].volume * volume;
    if (g_snd.slaveLerp != 0.0f && !master && (pAlias0->flags & 4))
        realVolume = SND_GetLerpedSlavePercentage(pAlias0->fSlavePercentage) * realVolume;

    pan = 0.5f;
    if (srcChannelCount != 1)
        pan = 1.0f;
    AIL_set_stream_volume_levels(handle, pan * realVolume, pan * realVolume);
    AIL_set_stream_loop_count(handle, ~pAlias0->flags & 1);

    if (!snd_enableReverb->current.enabled || (pAlias0->flags & 0x10))
        reverbLevel = 0.0f;
    else
        reverbLevel = g_snd.effect->wetlevel;
    AIL_set_stream_reverb_levels(handle, 1.0f, reverbLevel);

    AIL_stream_ms_position(handle, &total_msec, NULL);
    if (timeshift >= total_msec)
        return 0;
    if (!total_msec) {
        Com_Printf("^1ERROR: Sound file '%s' is zero length, invalid\n", realname);
        return 0;
    }

    if (fraction != 0.0f)
        start_msec = I_fround((float)total_msec * fraction);
    else
        start_msec = timeshift;
    if (start_msec)
        startDelay = 0;
    AIL_set_stream_ms_position(handle, start_msec);
    if (!startDelay && (!g_snd.paused || !g_snd.pauseSettings[(pAlias0->flags & 0x780) >> 7]))
        AIL_pause_stream(handle, 0);

    total_msec += startDelay;
    if (pAlias0->flags & 1)
        total_msec = 0;

    g_snd.chaninfo[index].org[0] = org[0];
    g_snd.chaninfo[index].org[1] = org[1];
    g_snd.chaninfo[index].org[2] = org[2];
    SND_SetChannelInfo(index, entnum, pAlias0, pAlias1, lerp, org, volume, pitch, srcChannelCount, baserate, (int)total_msec, start_msec, startDelay, master, system);

    if (SND_IsAliasChannel3D((g_snd.chaninfo[index].pAlias0->flags & 0x780) >> 7)) {
        MSS_SpatializeStreamImpl(index - 32, &volume, &pan);
        realVolume = g_snd.volume * g_snd.channelvol->channelvol[(pAlias0->flags & 0x780) >> 7].volume * volume;
        if (g_snd.slaveLerp != 0.0f && !master && (pAlias0->flags & 4))
            realVolume = SND_GetLerpedSlavePercentage(pAlias0->fSlavePercentage) * realVolume;
        AIL_set_stream_volume_levels(handle, (1.0f - pan) * realVolume, pan * realVolume);
    }

    return total_msec;
}

void SND_SetRoomtype(int roomtype)
{
    AIL_set_digital_master_room_type( (void *)(milesGlob.driver_2D), roomtype);
    AIL_set_digital_master_reverb_levels( (void *)(milesGlob.driver_2D), 1.0f, MSS_GetWetLevel(NULL));
    AIL_set_3D_room_type( (void *)(milesGlob.provider_3D), roomtype);
}

float SND_Get2DChannelVolume(int index)
{
    float left, right;
    snd_local_t *sndGlob;
    snd_channel_info_t *chaninfo;
    AIL_sample_volume_levels(milesGlob.handle_2D[index - 45], &left, &right);
    sndGlob = &g_snd;
    chaninfo = &sndGlob->chaninfo[index];
    if (chaninfo->srcChannelCount == 2) {
        return left;
    }
    return left + right;
}

// retail 0x4b3bc0; CoD4 name, pan read back from Miles
void SND_Set2DChannelVolume(int index, float volume)
{
    float pan;

    AIL_sample_volume_pan(milesGlob.handle_2D[index - 45], NULL, &pan);
    if (g_snd.chaninfo[index].srcChannelCount == 2)
        AIL_set_sample_volume_levels(milesGlob.handle_2D[index - 45], volume, volume);
    else
        AIL_set_sample_volume_levels(milesGlob.handle_2D[index - 45], (1.0f - pan) * volume, pan * volume);
}

// Original name unknown.
void SND_Set2DChannelVolumePan(int index, float volume, float pan)
{
    if (g_snd.chaninfo[index].srcChannelCount == 2) {
        AIL_set_sample_volume_levels(milesGlob.handle_2D[index - 45], volume, volume);
    } else {
        AIL_set_sample_volume_levels(milesGlob.handle_2D[index - 45], (1.0f - pan) * volume, volume * pan);
    }
}

float SND_Get3DChannelVolume(int index)
{
    return pow(AIL_3D_sample_volume(milesGlob.handle_3D[index]), 1.0f / 0.6f);
}

// retail 0x4b3cb0; CoD4 name
void SND_Set3DChannelVolume(int index, float volume)
{
    AIL_set_3D_sample_volume(milesGlob.handle_3D[index], pow(volume, 0.6f));
}

float SND_GetStreamChannelVolume(int index)
{
    float left, right;

    AIL_stream_volume_levels(milesGlob.handle_stream[index - 0x20], &left, &right);
    if (g_snd.chaninfo[index].srcChannelCount == 2) {
        if (!SND_IsAliasChannel3D((g_snd.chaninfo[index].pAlias0->flags & 0x780) >> 7)) {
            return left;
        }
    }
    return left + right;
}

// retail 0x4b3d60; CoD4 name, pan read back from Miles
void SND_SetStreamChannelVolume(int index, float volume)
{
    float pan;

    AIL_stream_volume_pan(milesGlob.handle_stream[index - 32], NULL, &pan);
    if (g_snd.chaninfo[index].srcChannelCount == 2 && !SND_IsAliasChannel3D((g_snd.chaninfo[index].pAlias0->flags & 0x780) >> 7))
        AIL_set_stream_volume_levels(milesGlob.handle_stream[index - 32], volume, volume);
    else
        AIL_set_stream_volume_levels(milesGlob.handle_stream[index - 32], (1.0f - pan) * volume, pan * volume);
}

// Original name unknown.
void SND_SetStreamChannelVolumePan(int index, float volume, float pan)
{
    if (g_snd.chaninfo[index].srcChannelCount == 2 && !SND_IsAliasChannel3D((g_snd.chaninfo[index].pAlias0->flags & 0x780) >> 7)) {
        AIL_set_stream_volume_levels(milesGlob.handle_stream[index - 32], volume, volume);
    } else {
        AIL_set_stream_volume_levels(milesGlob.handle_stream[index - 32], (1.0f - pan) * volume, volume * pan);
    }
}

int SND_Get2DChannelPlaybackRate(int index)
{
    return AIL_sample_playback_rate(milesGlob.handle_2D[index - 45]);
}

void SND_Set2DChannelPlaybackRate(int index, int rate)
{
    AIL_set_sample_playback_rate(milesGlob.handle_2D[index - 45], rate);
}

int SND_Get3DChannelPlaybackRate(int index)
{
    return AIL_3D_sample_playback_rate(milesGlob.handle_3D[index]);
}

void SND_Set3DChannelPlaybackRate(int index, int rate)
{
    AIL_set_3D_sample_playback_rate(milesGlob.handle_3D[index], rate);
}

int SND_GetStreamChannelPlaybackRate(int index)
{
    return AIL_stream_playback_rate(milesGlob.handle_stream[index - 0x20]);
}

void SND_SetStreamChannelPlaybackRate(int index, int rate)
{
    AIL_set_stream_playback_rate(milesGlob.handle_stream[index - 0x20], rate);
}

void SND_Update3DChannelReverb(int index)
{
    const snd_alias_t *pAlias = g_snd.chaninfo[index].pAlias0;
    float reverbLevel;

    if (pAlias == NULL) {
        reverbLevel = g_snd.effect->wetlevel;
    } else if (((dvar_t *)snd_enableReverb)->current.enabled != 0 && !(pAlias->flags & 0x10)) {
        reverbLevel = g_snd.effect->wetlevel;
    } else {
        reverbLevel = 0;
    }
    AIL_set_3D_sample_effects_level(milesGlob.handle_3D[index], reverbLevel);
}

int SND_Get3DChannelLength(int index)
{
    return AIL_3D_sample_length(milesGlob.handle_3D[index]);
}

void SND_Get2DChannelSaveInfo(int index, snd_save_2D_sample_t *info)
{
    void *handle;
    long int length, offset;

    handle = milesGlob.handle_2D[index - 45];
    AIL_sample_ms_position(handle, &length, &offset);
    info->fraction = (float)(unsigned long int)offset / (float)(unsigned long int)length;
    info->pitch = g_snd.chaninfo[index].pitch;
    AIL_sample_volume_pan(handle, &info->volume, &info->pan);
    if (g_snd.volume != 0.0f) {
        info->volume /= g_snd.volume;
    } else {
        info->volume = g_snd.chaninfo[index].basevolume;
    }
}

void SND_Set2DChannelFromSaveInfo(int index, snd_save_2D_sample_t *info)
{
    SND_Set2DChannelVolumePan(index, info->volume * g_snd.volume, info->pan);
}

void SND_Get3DChannelSaveInfo(int index, snd_save_3D_sample_t *info)
{
    void *handle;
    long unsigned int sampleOffset;
    long unsigned int sampleLength;
    float vol;

    handle = milesGlob.handle_3D[index];
    sampleOffset = AIL_3D_sample_offset(handle);
    sampleLength = AIL_3D_sample_length(handle);
    info->fraction = (float)sampleOffset / (float)sampleLength;
    info->pitch = g_snd.chaninfo[index].pitch;
    vol = AIL_3D_sample_volume(handle);
    info->volume = vol;
    if (g_snd.volume != 0.0f) {
        info->volume = vol / g_snd.volume;
    } else {
        info->volume = g_snd.chaninfo[index].basevolume;
    }
    AIL_3D_position(handle, &info->org[0], &info->org[1], &info->org[2]);
}

void SND_GetStreamChannelSaveInfo(int index, snd_save_stream_t *info)
{
    void *handle;
    long int offset, length;

    handle = milesGlob.handle_stream[index - 0x20];
    AIL_stream_ms_position(handle, &length, &offset);
    info->fraction = (float)(unsigned long int)offset / (float)(unsigned long int)length;
    info->rate = I_fround((float)AIL_stream_playback_rate(handle) / g_snd.timescale);
    info->basevolume = g_snd.chaninfo[index].basevolume;
    AIL_stream_volume_pan(handle, &info->volume, &info->pan);
    if (g_snd.volume != 0.0f) {
        info->volume /= g_snd.volume;
    } else {
        info->volume = g_snd.chaninfo[index].basevolume;
    }
    info->org[0] = g_snd.chaninfo[index].org[0];
    info->org[1] = g_snd.chaninfo[index].org[1];
    info->org[2] = g_snd.chaninfo[index].org[2];
}

void SND_SetStreamChannelFromSaveInfo(int index, snd_save_stream_t *info)
{
    SND_SetStreamChannelVolumePan(index, info->volume * g_snd.volume, info->pan);
}

// retail 0x4b4330; name unknown
static void SND_InitRawSamples(int rate, int width, int s_channels)
{
    int format, bufSize;
    float vol;

    milesGlob.raw.handle = (HSAMPLE)(AIL_allocate_sample_handle( (void *)(milesGlob.driver_2D)));
    if (milesGlob.raw.handle == NULL) {
        Com_Error(ERR_DROP, (const char *)"\x15MILES 2D sound sample allocation failed on raw channel\n");
    }
    milesGlob.raw.rate = rate;
    milesGlob.raw.width = width;
    milesGlob.raw.channels = s_channels;
    AIL_init_sample(milesGlob.raw.handle);

    if (s_channels == 1) {
        format = width != 1;
    } else {
        format = (width != 1) + 2;
    }
    AIL_set_sample_type(milesGlob.raw.handle, format, 0);
    AIL_set_sample_playback_rate(milesGlob.raw.handle, rate);
    vol = g_snd.volume * 0.5f;
    AIL_set_sample_volume_levels(milesGlob.raw.handle, vol, vol);
    bufSize = AIL_minimum_sample_buffer_size( (void *)(milesGlob.driver_2D), rate, format);
    milesGlob.raw.bufSize = 0x2000;
    if (bufSize > 0x2000) {
        milesGlob.raw.bufSize = bufSize;
    }
    milesGlob.raw.buf = (byte *)(Z_MallocInternal(milesGlob.raw.bufSize * 32));
    milesGlob.raw.sampleTime = 0.0;
    memset(milesGlob.raw.bufReady, 0, 32);
    milesGlob.raw.readBuf = 0;
    milesGlob.raw.writeBuf = 0;
    milesGlob.raw.bufUsed = 0;
    milesGlob.raw.bufRate = 1000.0f / (float)(milesGlob.raw.channels * milesGlob.raw.width * milesGlob.raw.rate);
}

void SND_EndRawSamples(void)
{
    if (milesGlob.raw.handle != NULL) {
        AIL_end_sample(milesGlob.raw.handle);
        AIL_release_sample_handle(milesGlob.raw.handle);
        milesGlob.raw.handle = NULL;
        Z_FreeInternal(milesGlob.raw.buf);
    }
}

int SND_RawSamplesTime(void)
{
    if (!milesGlob.raw.handle) {
        return 0;
    }
    return (int)(AIL_sample_position(milesGlob.raw.handle) * milesGlob.raw.bufRate + milesGlob.raw.sampleTime);
}

void SND_DriverPostUpdate(void)
{
    void *rawHandle;
    float vol;
    int bufNum;

    rawHandle = milesGlob.raw.handle;
    if (rawHandle == NULL) {
        return;
    }

    vol = 0.5f * g_snd.volume;
    AIL_set_sample_volume_levels(rawHandle, vol, vol);

    if (milesGlob.raw.bufReady[milesGlob.raw.readBuf] == 0) {
        return;
    }

    bufNum = AIL_sample_buffer_ready(milesGlob.raw.handle);
    if (bufNum == -1) {
        return;
    }

    milesGlob.raw.sampleTime += (double)milesGlob.raw.bufSize * milesGlob.raw.bufRate;

    AIL_load_sample_buffer(milesGlob.raw.handle, bufNum,
                           (void *)(milesGlob.raw.buf + milesGlob.raw.readBuf * milesGlob.raw.bufSize),
                           milesGlob.raw.bufSize);

    milesGlob.raw.bufReady[milesGlob.raw.readBuf] = 0;
    milesGlob.raw.readBuf = (milesGlob.raw.readBuf + 1) % 32;
}

void SND_RawSamples(int samples, int rate, int width, int s_channels, const byte *data)
{
    int bytes, copy;

    if (!g_snd.Initialized2d) {
        return;
    }

    if (milesGlob.raw.handle == NULL) {
        SND_InitRawSamples(rate, width, s_channels);
        if (milesGlob.raw.handle == NULL) {
            return;
        }
    }

    bytes = samples * width * s_channels;
    while (bytes) {
        while (milesGlob.raw.bufReady[milesGlob.raw.writeBuf]) {
            SND_Update();
        }
        copy = milesGlob.raw.bufSize - milesGlob.raw.bufUsed;
        if (copy > bytes) {
            copy = bytes;
        }
        Com_Memcpy((void *)(milesGlob.raw.buf + milesGlob.raw.writeBuf * milesGlob.raw.bufSize + milesGlob.raw.bufUsed), (void *)data, copy);
        milesGlob.raw.bufUsed += copy;
        data += copy;
        bytes -= copy;
        if (milesGlob.raw.bufUsed == milesGlob.raw.bufSize) {
            milesGlob.raw.bufUsed = 0;
            milesGlob.raw.bufReady[milesGlob.raw.writeBuf] = 1;
            milesGlob.raw.writeBuf = (milesGlob.raw.writeBuf + 1) % 32;
        }
    }
}

/* Original name unknown (KisakCOD: SND_LoadFromBuffer). sound is overwritten
   on every path, but callers still pass it in 1.0. */
MssSound *SND_LoadFromBuffer(void *buffer, const char *realname, MssSound *sound)
{
    AILMIXINFO mixinfo;
    int datasize, totalSize;
    unsigned int rate, samples;
    int targetBits, targetChannels;
    int procFormat;

    if (!AIL_WAV_info(buffer, &mixinfo.Info)) {
        Com_Printf((const char *)"^1ERROR: Sound file '%s' is in an invalid or corrupted format\n", realname);
        sound = NULL;
    } else if ((totalSize = (int)mixinfo.Info.data_len + 0x24) == 0) {
        Com_Printf((const char *)"^1ERROR: Sound file '%s' is zero length, invalid\n", realname);
        sound = NULL;
    } else if (mixinfo.Info.rate <= (unsigned long int)g_snd.playback_rate &&
               ((long int)mixinfo.Info.bits <= g_snd.playback_bits || mixinfo.Info.format == 0x11) &&
               (long int)mixinfo.Info.channels <= g_snd.playback_channels) {

        sound = (MssSound *)Hunk_AllocInternal(totalSize);
        Com_Memcpy(sound->data, (void *)mixinfo.Info.data_ptr, mixinfo.Info.data_len);
        sound->info = mixinfo.Info;
        sound->info.data_ptr = sound->data;
        sound->info.initial_ptr = sound->data;
    } else {

        rate = mixinfo.Info.rate;
        samples = mixinfo.Info.samples;
        while (rate > (unsigned int)g_snd.playback_rate) {
            rate >>= 1;
            samples >>= 1;
        }

        targetBits = (int)mixinfo.Info.bits - g_snd.playback_bits >= 0 ? g_snd.playback_bits : (int)mixinfo.Info.bits;
        targetChannels = (int)mixinfo.Info.channels - g_snd.playback_channels >= 0 ? g_snd.playback_channels : (int)mixinfo.Info.channels;

        if (targetChannels == 1) {
            if (mixinfo.Info.format == 0x11)
                procFormat = 5;
            else
                procFormat = targetBits > 8 ? 1 : 0;
        } else {
            if (mixinfo.Info.format == 0x11)
                procFormat = 7;
            else
                procFormat = targetBits > 8 ? 3 : 2;
        }

        datasize = AIL_size_processed_digital_audio(rate, procFormat, 1, &mixinfo);
        sound = (MssSound *)Hunk_AllocInternal(datasize + 0x24);

        sound->info.format = mixinfo.Info.format;
        sound->info.data_ptr = sound->data;
        sound->info.data_len = datasize;
        sound->info.rate = rate;
        sound->info.bits = targetBits;
        sound->info.channels = targetChannels;
        sound->info.samples = samples;
        sound->info.block_size = mixinfo.Info.block_size;
        sound->info.initial_ptr = sound->data;

        AIL_process_digital_audio(sound->data, datasize, rate, procFormat, 1, &mixinfo);
    }

    return sound;
}

// retail 0x4b4890, unreferenced; name unknown
MssSound *SND_LoadFromSound_4b4890(MssSound **sound, const char *realname)
{
    return SND_LoadFromBuffer((*sound)->data, realname, *sound);
}

void SND_LoadSoundFile(SoundFile *soundFile)
{
    char realname[256];
    void *buffer;

    if (!g_snd.Initialized2d) {
        soundFile->fileMem = NULL;
        return;
    }

    sprintf(realname, (const char *)"sound/%s", soundFile->soundName);
    if (FS_ReadFile(realname, &buffer) < 0) {
        Com_Printf((const char *)"^1ERROR: Sound file '%s' not found\n", realname);
        soundFile->fileMem = NULL;
        return;
    }

    soundFile->fileMem = SND_LoadFromBuffer(buffer, realname, NULL);
    FS_FreeFile(buffer);
}

int SND_GetSoundFileSize(const void *pSoundFile)
{
    const MssSound *sound = (const MssSound *)pSoundFile;
    return (int)sound->info.data_len + 0x24;
}

static inline int I_max(const int x, const int y)
{
    return I_sel(x - y, x, y);
}
void SND_Update2DChannel(int i, int frametime)
{
    const snd_alias_t *pAlias0;
    float volume;
    int startDelay, newDelay;

    if (g_snd.chaninfo[i].paused != 0) {
        return;
    }

    pAlias0 = g_snd.chaninfo[i].pAlias0;
    volume = g_snd.chaninfo[i].basevolume;

    if (g_snd.slaveLerp != 0.0f && !g_snd.chaninfo[i].master && (pAlias0->flags & 4)) {
        volume *= SND_GetLerpedSlavePercentage(pAlias0->fSlavePercentage);
    }

    volume *= ((float *)g_snd.channelvol)[((pAlias0->flags & 0x780) >> 7) * 3];
    SND_Set2DChannelVolumePan(i, volume * g_snd.volume, 0.5f);

    startDelay = g_snd.chaninfo[i].startDelay;
    if (startDelay != 0) {
        newDelay = I_max(0, startDelay - frametime);
        g_snd.chaninfo[i].startDelay = newDelay;
        if (newDelay == 0) {
            AIL_resume_sample(milesGlob.handle_2D[i - 45]);
        }
    }
}

static inline float I_sqrt(const float value)
{
    return sqrt(value);
}
void SND_Update3DChannel(int i, int frametime)
{
    const snd_alias_t *pAlias0, *pAlias1;
    float lerp, oneMinusLerp;
    float volume;
    float fDistMin, fDistMax;
    float org[3];
    int startDelay, newDelay;

    if (g_snd.chaninfo[i].paused != 0) {
        return;
    }

    pAlias0 = g_snd.chaninfo[i].pAlias0;
    pAlias1 = g_snd.chaninfo[i].pAlias1;
    lerp = g_snd.chaninfo[i].lerp;
    volume = g_snd.chaninfo[i].basevolume;

    SND_GetCurrent3DPosition(g_snd.chaninfo[i].entnum, g_snd.chaninfo[i].offset, org);
    SND_Set3DPosition(i, org);

    oneMinusLerp = 1.0f - lerp;
    fDistMin = oneMinusLerp * pAlias0->fDistMin + lerp * pAlias1->fDistMin;
    fDistMax = oneMinusLerp * pAlias0->fDistMax + lerp * pAlias1->fDistMax;

    volume *= SND_Attenuate(pAlias0->volumeFalloffCurve, I_sqrt(Vec3DistanceSq(org, g_snd.listeners[0].orient.origin)), fDistMin, fDistMax);

    if (g_snd.slaveLerp != 0.0f && !g_snd.chaninfo[i].master && (pAlias0->flags & 4)) {
        volume *= SND_GetLerpedSlavePercentage(pAlias0->fSlavePercentage);
    }

    volume *= ((float *)g_snd.channelvol)[((pAlias0->flags & 0x780) >> 7) * 3];
    AIL_set_3D_sample_volume(milesGlob.handle_3D[i], pow(g_snd.volume * volume, 0.6f));

    startDelay = g_snd.chaninfo[i].startDelay;
    if (startDelay != 0) {
        newDelay = I_max(0, startDelay - frametime);
        g_snd.chaninfo[i].startDelay = newDelay;
        if (newDelay == 0) {
            AIL_resume_3D_sample(milesGlob.handle_3D[i]);
        }
    }
}

void SND_UpdateStreamChannel(int i, int frametime)
{
    const snd_alias_t *pAlias0;
    float volume, pan;
    int startDelay, newDelay;

    if (g_snd.chaninfo[i].paused != 0) {
        return;
    }

    if (i < 35 && !SND_UpdateBackgroundVolume(i - 32, frametime)) {
        return;
    }

    pAlias0 = g_snd.chaninfo[i].pAlias0;
    volume = g_snd.chaninfo[i].basevolume;
    pan = 0.5f;

    if (SND_IsAliasChannel3D((pAlias0->flags & 0x780) >> 7)) {
        SND_GetCurrent3DPosition(g_snd.chaninfo[i].entnum, g_snd.chaninfo[i].offset, g_snd.chaninfo[i].org);
        MSS_SpatializeStreamImpl(i - 32, &volume, &pan);
    }

    if (g_snd.slaveLerp != 0.0f && !g_snd.chaninfo[i].master && (pAlias0->flags & 4)) {
        volume *= SND_GetLerpedSlavePercentage(pAlias0->fSlavePercentage);
    }

    volume *= ((float *)g_snd.channelvol)[((pAlias0->flags & 0x780) >> 7) * 3];
    SND_SetStreamChannelVolumePan(i, volume * g_snd.volume, pan);

    startDelay = g_snd.chaninfo[i].startDelay;
    if (startDelay != 0) {
        newDelay = I_max(0, startDelay - frametime);
        g_snd.chaninfo[i].startDelay = newDelay;
        if (newDelay == 0) {
            AIL_pause_stream(milesGlob.handle_stream[i - 32], 0);
        }
    }
}

void SND_DriverPreUpdate(int frametime)
{
}
static void MSS_SpatializeStream(float *volume, float *pan)
{

}
void SND_Stop2DChannel(int index)
{
    AIL_end_sample(milesGlob.handle_2D[index - 45]);
    g_snd.chaninfo[index].paused = 0;
    g_snd.chaninfo[index].startDelay = 0;
}
void SND_Pause2DChannel(int index)
{
    snd_local_t *sndGlob;
    snd_channel_info_t *chaninfo;
    AIL_stop_sample(milesGlob.handle_2D[index - 45]);
    sndGlob = &g_snd;
    chaninfo = &sndGlob->chaninfo[index];
    chaninfo->paused = 1;
}
void SND_Unpause2DChannel(int index, int timeshift)
{
    if (g_snd.chaninfo[index].startDelay == 0) {
        AIL_resume_sample(milesGlob.handle_2D[index - 45]);
    }
    g_snd.chaninfo[index].endtime += timeshift;
    g_snd.chaninfo[index].paused = 0;
}
Bool SND_Is2DChannelFree(int index)
{
    if (!g_snd.chaninfo[index].paused && !g_snd.chaninfo[index].startDelay)
        return AIL_sample_status(milesGlob.handle_2D[index - 45]) == 2 ? 1 : 0;
    return 0;
}
void SND_Stop3DChannel(int index)
{
    AIL_end_3D_sample(milesGlob.handle_3D[index]);
    g_snd.chaninfo[index].paused = 0;
    g_snd.chaninfo[index].startDelay = 0;
}
void SND_Pause3DChannel(int index)
{
    snd_local_t *sndGlob;
    snd_channel_info_t *chaninfo;
    AIL_stop_3D_sample(milesGlob.handle_3D[index]);
    sndGlob = &g_snd;
    chaninfo = &sndGlob->chaninfo[index];
    chaninfo->paused = 1;
}
void SND_Unpause3DChannel(int index, int timeshift)
{
    if (g_snd.chaninfo[index].startDelay == 0) {
        AIL_resume_3D_sample(milesGlob.handle_3D[index]);
    }
    g_snd.chaninfo[index].endtime += timeshift;
    g_snd.chaninfo[index].paused = 0;
}
Bool SND_Is3DChannelFree(int index)
{
    if (!g_snd.chaninfo[index].paused && !g_snd.chaninfo[index].startDelay)
        return AIL_3D_sample_status(milesGlob.handle_3D[index]) == 2 ? 1 : 0;
    return 0;
}
void SND_UpdateStreamChannelReverb(int index)
{
    const snd_alias_t *pAlias = g_snd.chaninfo[index].pAlias0;
    float reverbLevel;

    if (pAlias == NULL) {
        reverbLevel = g_snd.effect->wetlevel;
    } else if (((dvar_t *)snd_enableReverb)->current.enabled != 0 && !(pAlias->flags & 0x10)) {
        reverbLevel = g_snd.effect->wetlevel;
    } else {
        reverbLevel = 0;
    }
    AIL_set_stream_reverb_levels(milesGlob.handle_stream[index - 0x20], 1.0f, reverbLevel);
}
int SND_Get2DChannelLength(int index)
{
    long int length;
    long int *lenp = &length;
    AIL_sample_ms_position(milesGlob.handle_2D[index - 45], lenp, NULL);
    return (int)length;
}
int SND_GetStreamChannelLength(int index)
{
    long int length;
    long int *lenp = &length;
    AIL_stream_ms_position(milesGlob.handle_stream[index - 0x20], lenp, NULL);
    return (int)length;
}
void SND_Update2DChannelReverb(int index)
{
    const snd_alias_t *pAlias = g_snd.chaninfo[index].pAlias0;
    float reverbLevel;

    if (pAlias == NULL) {
        reverbLevel = g_snd.effect->wetlevel;
    } else if (((dvar_t *)snd_enableReverb)->current.enabled != 0 && !(pAlias->flags & 0x10)) {
        reverbLevel = g_snd.effect->wetlevel;
    } else {
        reverbLevel = 0;
    }
    AIL_set_sample_reverb_levels(milesGlob.handle_2D[index - 45], 1.0f, reverbLevel);
}
