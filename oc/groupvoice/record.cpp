
#include "common_types.h"
#include "imports.h"
#include <string.h>
#include <stdlib.h>


static int samples_in_partial_audio_buffer = 0x0;

extern char enc_buffer[4096];
extern int enc_buffer_pos;
extern short int partial_audio_buffer[640];
extern int samples_in_partial_audio_buffer;

extern long DSOUNDRecord_Start(recordingSample_t *sample);
extern long DSOUNDRecord_Stop(recordingSample_t *sample);
extern recordingSample_t *DSOUNDRecord_NewSample(void);
extern int DSOUNDRecord_DestroySample(recordingSample_t *sample);
extern void DSOUNDRecord_Shutdown(void);
extern void DSOUNDRecord_Frame(void);
extern int DSOUNDRecord_Init(bool bCallDsoundInit);
extern bool DSOUNDRecord_SetFrequency(int frequency);
extern void Record_SetRecordingCallback(int (*new_audioCallback)(audioSample_t *));
extern Bool Encode_Shutdown(void);
extern int Encode_GetFrameSize(void);
extern int Encode_Sample(short *buffer_in, char *buffer_out, int maxLength);
extern bool Voice_SendVoiceData(void);
extern Bool IsTalking(void);
extern void CL_VoiceTransmit(void);
extern int Client_SendVoiceData(int size, char *data);
extern void Com_Printf(const char *msg, ...);

extern float voice_current_voicelevel;
extern float voice_current_scaler;

int Record_Start(recordingSample_t *sample);
int Record_Stop(recordingSample_t *sample);
recordingSample_t * Record_NewSample(void);
int Record_DestroySample(recordingSample_t *sample);
void Record_Shutdown(void);
void Record_Frame(void);
int Record_QueueAudioDataForEncoding(audioSample_t *sample);
int Record_AudioCallback(audioSample_t *sample);
int Record_Init(int callInit, const void *handle);

int Record_Start(recordingSample_t *sample)
{
    return DSOUNDRecord_Start(sample);
}

int Record_Stop(recordingSample_t *sample)
{
    return DSOUNDRecord_Stop(sample);
}

recordingSample_t *Record_NewSample(void)
{
    return DSOUNDRecord_NewSample();
}

int Record_DestroySample(recordingSample_t *sample)
{
    return DSOUNDRecord_DestroySample(sample);
}

void Record_Shutdown(void)
{
    Encode_Shutdown();
    DSOUNDRecord_Shutdown();
}

void Record_Frame(void)
{
    DSOUNDRecord_Frame();
}

static float I_fsel(const float x, const float y, const float z)
{
    return x >= 0.0f ? y : z;
}

static float I_fmin(const float x, const float y)
{
    return I_fsel(y - x, x, y);
}

int Record_QueueAudioDataForEncoding(audioSample_t *sample)
{
    short audio_buffer[4096];
    int bytesEncoded;
    int encoded;
    int frameSize;
    int partialSamples;
    int i;

    voice_current_voicelevel = 0.0f;
    if (sample->bytesPerSample == 2 && sample->lengthInSamples > 0) {
        for (i = 0; i < sample->lengthInSamples; i++) {
            ((short *)sample->buffer)[i] = (short)(((short *)sample->buffer)[i] * voice_current_scaler);
            voice_current_voicelevel += abs(((short *)sample->buffer)[i]);
        }
        voice_current_voicelevel /= sample->lengthInSamples;
    }

    if (!Voice_SendVoiceData())
        return 0;

    if (!IsTalking()) {
        CL_VoiceTransmit();
        return 0;
    }

    frameSize = Encode_GetFrameSize();
    sample->sampleOffset = 0;
    bytesEncoded = 0;
    if (frameSize <= 0) {
        Com_Printf("Invalid encode frame size of %i\n", frameSize);
        return 0;
    }

    if (sample->lengthInBytes <= 0) {
        Com_Printf("Invalid sample length of %i samples\n", sample->lengthInSamples);
        return 0;
    }

    while (frameSize <= sample->lengthInSamples + samples_in_partial_audio_buffer - sample->sampleOffset) {
        partialSamples = 0;
        if (samples_in_partial_audio_buffer) {
            partialSamples = (int)I_fmin((float)(frameSize - 4), (float)samples_in_partial_audio_buffer);
            memcpy(audio_buffer, partial_audio_buffer, sample->bytesPerSample * partialSamples);
            samples_in_partial_audio_buffer = 0;
        }
        memcpy(&audio_buffer[partialSamples], sample->buffer + sample->sampleOffset * sample->bytesPerSample,
               (frameSize - partialSamples) * sample->bytesPerSample);
        sample->sampleOffset += frameSize - partialSamples;
        encoded = Encode_Sample(audio_buffer, enc_buffer + enc_buffer_pos, 4096 - enc_buffer_pos);
        bytesEncoded += encoded;
        Client_SendVoiceData(encoded, enc_buffer + enc_buffer_pos);
    }

    if (sample->sampleOffset < sample->lengthInSamples) {
        memcpy(&partial_audio_buffer[samples_in_partial_audio_buffer],
               sample->buffer + sample->sampleOffset * sample->bytesPerSample,
               (sample->lengthInSamples - sample->sampleOffset) * sample->bytesPerSample);
        samples_in_partial_audio_buffer += sample->lengthInSamples - sample->sampleOffset;
    }

    return bytesEncoded;
}

int Record_AudioCallback(audioSample_t *sample)
{
    return Record_QueueAudioDataForEncoding(sample);
}

int Record_Init(int callInit, const void *handle)
{
    Record_SetRecordingCallback(Record_AudioCallback);
    return DSOUNDRecord_Init(true);
}

extern int g_current_bandwidth_setting;
extern Bool Encode_Shutdown(void);
extern bool Encode_Init(int bandwidthEnum);

// retail 0x4b73f0, unreferenced twin of Encode_SetBandwidth_4b67f0; name unknown
bool Encode_SetBandwidth_4b73f0(int bandwidthEnum)
{
    if (bandwidthEnum == g_current_bandwidth_setting)
        return 1;
    Encode_Shutdown();
    return Encode_Init(bandwidthEnum);
}

// retail 0x4b7410; name not recovered (Record_* -> DSOUNDRecord_* wrapper)
bool Record_SetFrequency(int frequency)
{
    return DSOUNDRecord_SetFrequency(frequency);
}

