#include "common_types.h"
#include "imports.h"
#include "bytematch.h"
#include <stdio.h>
int voice_freq = 0;
int sampleCount = 0;

float voice_current_voicelevel;

int mic_old_reclevel;
const dvar_t *winvoice_mic_reclevel;
char old_rec_source[256] = {0};
int mic_current_reclevel;
const dvar_t *winvoice_mic_mute;
const dvar_t *winvoice_mic_scaler;
float levelSamples[6];
const dvar_t *winvoice_save_voice;

static int g_voice_initialized = 0x0;

extern dvar_t *Dvar_RegisterBool(const char *dvarName, bool value, unsigned short flags);
extern dvar_t *Dvar_RegisterFloat(const char *dvarName, float value, float min, float max, unsigned short flags);
extern int Record_Init(int callInit, const void *handle);
extern int Sound_Init(const void *handle);
extern bool Encode_Init(int freq);
extern unsigned char Decode_Init(int freq);
extern void Record_Shutdown(void);
extern Bool Encode_Shutdown(void);
extern void Decode_Shutdown(void);
extern void Sound_Shutdown(void);
extern dsound_sample_t *Sound_NewSample(void);
extern void Sound_SampleFrame(dsound_sample_t *sample);
extern int Sound_DestroySample(dsound_sample_t *sample);
extern int Sound_UpdateSample(dsound_sample_t *sample, char *data, unsigned int size);
extern void Sound_Frame(void);
extern void Record_Frame(void);
extern int Record_Start(recordingSample_t *sample);
extern int Record_Stop(recordingSample_t *sample);
extern recordingSample_t *Record_NewSample(void);
extern int Record_DestroySample(recordingSample_t *sample);
extern int mixerGetRecordSource(char *source);
extern int mixerSetRecordSource(const char *source);
extern int mixerGetRecordLevel(const char *source);
extern int mixerSetRecordLevel(const char *source, unsigned short level);
extern int mixerSetMicrophoneMute(bool mute);
extern int Sys_Milliseconds(void);
extern int Decode_Sample(char *inData, int inSize, short *outData, int maxOutSize);

extern int voice_freq;
extern "C" __declspec(dllimport) HWND__ *__stdcall GetDesktopWindow(void);
extern int g_frame_size;

extern const dvar_t *winvoice_mic_mute;
extern const dvar_t *winvoice_mic_reclevel;
extern const dvar_t *winvoice_save_voice;
extern const dvar_t *winvoice_mic_scaler;
extern int mic_old_reclevel;
extern int mic_current_reclevel;
extern char old_rec_source[256];
extern float voice_current_voicelevel;
extern float voice_current_scaler;
extern int g_voice_initialized;
extern float levelSamples[6];
extern int sampleCount;
static int count;
static Bool recording;
static Bool playing;
static recordingSample_t *currentRecordingSample;

static short unsigned int *(*s_clientSamples[128])[4];
static int s_clientTalkTime[64];

Bool Voice_Init(void);
float Voice_GetVoiceLevel(void);
void Voice_Playback(void);
void Voice_IncomingVoiceData(unsigned char talker, unsigned char *data, int packetDataSize);
Bool Voice_IsClientTalking(int clientNum);
void Voice_Shutdown(void);
int Voice_GetLocalVoiceData(ClientVoicePacket_t *voiceData);
Bool Voice_StartRecording(void);
Bool Voice_StopRecording(void);

Bool Voice_StartRecording(void);
Bool Voice_StopRecording(void);

Bool Voice_IsClientTalking(int clientNum)
{
    return (Sys_Milliseconds() - s_clientTalkTime[clientNum]) < 300 ? 1 : 0;
}

Bool Voice_Init(void)
{
    int i;
    unsigned short level;
    HWND__ *hwnd;

    winvoice_mic_mute = Dvar_RegisterBool("winvoice_mic_mute", 1, 0x1001);
    winvoice_mic_reclevel = Dvar_RegisterFloat("winvoice_mic_reclevel", 65535.0f, 0.0f, 65535.0f, 0x1001);
    winvoice_save_voice = Dvar_RegisterBool("winvoice_save_voice", 0, 0x1001);
    winvoice_mic_scaler = Dvar_RegisterFloat("winvoice_mic_scaler", 1.0f, 0.25f, 2.0f, 0x1001);

    mixerGetRecordSource(old_rec_source);
    mixerSetRecordSource("Mic");
    mic_old_reclevel = mixerGetRecordLevel("Mic");

    level = (int)winvoice_mic_reclevel->current.value;
    mic_current_reclevel = (unsigned short)level;
    mixerSetRecordLevel("Mic", level);

    mixerSetMicrophoneMute(*(bool *)&winvoice_mic_mute->current.enabled);

    voice_freq = 0;
    hwnd = GetDesktopWindow();
    Record_Init(0, 0);
    Sound_Init(hwnd);
    Encode_Init(voice_freq);
    Decode_Init(voice_freq);

    g_voice_initialized = 1;

    memset(s_clientTalkTime, 0, sizeof(s_clientTalkTime));

    for (i = 0; i < 64; i++) {
        s_clientSamples[i] = (unsigned short *(*)[4])(Sound_NewSample());
    }

    return 0;
}

void Voice_Shutdown(void)
{
    int i;

    for (i = 0; i < 64; i++) {
        Sound_DestroySample((dsound_sample_t *)s_clientSamples[i]);
    }

    Voice_StopRecording();

    Record_Shutdown();
    Encode_Shutdown();
    Decode_Shutdown();
    Sound_Shutdown();

    mixerSetMicrophoneMute(1);
    mixerSetRecordLevel("Mic", (unsigned short)mic_old_reclevel);
    mixerSetRecordSource(old_rec_source);

    g_voice_initialized = 0;
}

float Voice_GetVoiceLevel(void)
{
    float result;
    int idx;
    int i;

    if (!g_voice_initialized)
        return 0.0f;

    idx = sampleCount % 6;
    result = ( voice_current_voicelevel / 32767.0f ) / 6.0f;
    levelSamples[idx] = result;
    sampleCount++;

    result = 0.0f;
    for (i = 0; i < 6; i++) {
        result += levelSamples[i];
    }

    return result;
}

void Voice_Playback(void)
{
    int i;

    if (!g_voice_initialized)
        return;

    if (mic_current_reclevel != (unsigned short)(int)winvoice_mic_reclevel->current.value) {
        mic_current_reclevel = (unsigned short)(int)winvoice_mic_reclevel->current.value;
        mixerSetRecordLevel("Mic", (unsigned short)(int)winvoice_mic_reclevel->current.value);
    }

    for (i = 0; i < 64; i++) {
        Sound_SampleFrame((dsound_sample_t *)s_clientSamples[i]);
    }

    Sound_Frame();
}

int Voice_GetLocalVoiceData(ClientVoicePacket_t *voiceData)
{

    if (!g_voice_initialized)
        return 0;

    if (!recording)
        Voice_StartRecording();

    if (voice_current_scaler != winvoice_mic_scaler->current.value) {
        voice_current_scaler = winvoice_mic_scaler->current.value;
        if (voice_current_scaler < 0.5)
            voice_current_scaler = 0.5f;
        else if (voice_current_scaler > 1.5)
            voice_current_scaler = 1.5f;
    }

    Record_Frame();
    return 0;
}

Bool Voice_StartRecording(void)
{
    if (recording)
        return 1;
    currentRecordingSample = Record_NewSample();
    Record_Start(currentRecordingSample);
    recording = 1;
    count++;
    return 1;
}

Bool Voice_StopRecording(void)
{
    if (!recording)
        return 0;
    Record_Stop(currentRecordingSample);
    Record_DestroySample(currentRecordingSample);
    recording = 0;
    return 1;
}

void Voice_IncomingVoiceData(unsigned char talker, unsigned char *data, int packetDataSize)
{
    short decodedData[4096];
    int processedBytes;
    int bytesToProcess;
    int decodedLen;
    FILE *fptr;

    if (!playing)
        playing = 1;

    decodedLen = 0;
    processedBytes = 0;
    s_clientTalkTime[talker] = Sys_Milliseconds();

    while (processedBytes < packetDataSize) {
        float remaining = (float)(packetDataSize - processedBytes);
        float frameSize = (float)g_frame_size;
        bytesToProcess = (int)(remaining - frameSize >= 0.0f ? frameSize : remaining);
        decodedLen = Decode_Sample((char *)(data + processedBytes), bytesToProcess, decodedData, g_frame_size);
        if (decodedLen > 0)
            Sound_UpdateSample((dsound_sample_t *)s_clientSamples[talker], (char *)decodedData, decodedLen);
        processedBytes += bytesToProcess;
    }

    if (winvoice_save_voice->current.enabled) {
        fptr = fopen("voice.wav", "ab");
        if (fptr != NULL) {
            fwrite(decodedData, decodedLen, 1, fptr);
            fclose(fptr);
        }
    }
}

