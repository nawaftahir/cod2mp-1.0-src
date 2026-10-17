#include "common_types.h"
#include "imports.h"

struct HWND__;

extern int DSound_UpdateSample(dsound_sample_t *sample, char *data, unsigned int data_len);
extern dsound_sample_t *DSound_NewSample(void);
extern int Record_DestroySample(dsound_sample_t *sample);
extern int DSound_PlaySample(dsound_sample_t *sample);
extern bool DSound_StopSample(dsound_sample_t *sample);
extern bool DSound_FinishSample(dsound_sample_t *sample);
extern void DSound_Frame(void);
extern bool DSound_SetSampleFrequency(dsound_sample_t *sample, int frequency);
extern void DSound_SampleFrame(dsound_sample_t *sample);
extern int DSound_Init(bool callDsoundInit, HWND__ *handle);
extern void DSound_Shutdown(void);

int Sound_UpdateSample(dsound_sample_t *sample, char *data, unsigned int data_len)
{
    return DSound_UpdateSample(sample, data, data_len);
}

dsound_sample_t *Sound_NewSample(void)
{
    return DSound_NewSample();
}

int Sound_DestroySample(dsound_sample_t *sample)
{
    return Record_DestroySample(sample);
}

// Names of the next five wrappers not recovered (not in the Mac build).
int Sound_PlaySample(dsound_sample_t *sample)
{
    return DSound_PlaySample(sample);
}

bool Sound_StopSample(dsound_sample_t *sample)
{
    return DSound_StopSample(sample);
}

bool Sound_FinishSample(dsound_sample_t *sample)
{
    return DSound_FinishSample(sample);
}

void Sound_Frame(void)
{
    DSound_Frame();
}

bool Sound_SetSampleFrequency(dsound_sample_t *sample, int frequency)
{
    return DSound_SetSampleFrequency(sample, frequency);
}

void Sound_SampleFrame(dsound_sample_t *sample)
{
    DSound_SampleFrame(sample);
}

int Sound_Init(const void *handle)
{
    return DSound_Init(true, (HWND__ *)handle);
}

void Sound_Shutdown(void)
{
    DSound_Shutdown();
}
