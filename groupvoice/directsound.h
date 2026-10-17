#ifndef __DIRECTSOUND_H__
#define __DIRECTSOUND_H__

#include <dsound.h>

struct dsound_sample_t
{
	IDirectSoundCaptureBuffer *DSCB;
	IDirectSoundBuffer *DSB;
	unsigned int dwBufferSize;
	unsigned int dwCaptureOffset;
	unsigned int currentOffset;
	unsigned int lastOffset;
	unsigned int currentBufferLength;
	int stopPosition;
	unsigned int lastPlayPos;
	int bytesBuffered;
	int mode;
	int frequency;
	int volume;
	int pan;
	int channels;
	bool playing;
	int channel;
	unsigned char playMode;
};

struct audioSample_t
{
	unsigned char *buffer;
	int lengthInBytes;
	int lengthInSamples;
	int bytesPerSample;
	int frequency;
	bool stereo;
	int channels;
	int sampleOffset;
};

extern int g_sound_recordFrequency;
extern int g_sound_channels;

// play_dsound
int DSound_UpdateSample( dsound_sample_t *sample, char *data, unsigned int data_len );
void DSound_SampleFrame( dsound_sample_t *sample );
dsound_sample_t *DSound_NewSample( void );
bool DSound_StopSample( dsound_sample_t *sample );
int DSound_Init( bool callDsoundInit, HWND handle );
void DSound_Shutdown( void );
void DSound_Frame( void );
int DSound_PlaySample( dsound_sample_t *sample );
bool DSound_FinishSample( dsound_sample_t *sample );
bool DSound_SetSampleFrequency( dsound_sample_t *sample, int frequency );

// record_dsound
void DSOUNDRecord_UpdateSample( dsound_sample_t *pRecSample );
void DSOUNDRecord_Frame( void );
dsound_sample_t *DSOUNDRecord_NewSample( void );
int DSOUNDRecord_DestroySample( dsound_sample_t *pRecSample );
HRESULT DSOUNDRecord_Start( dsound_sample_t *pRecSample );
HRESULT DSOUNDRecord_Stop( dsound_sample_t *pRecSample );
int DSOUNDRecord_Init( bool bCallDsoundInit );
void DSOUNDRecord_Shutdown( void );
void Record_SetRecordingCallback( int (*new_audioCallback)( audioSample_t * ) );

#endif
