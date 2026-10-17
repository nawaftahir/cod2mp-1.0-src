#include "../qcommon/qcommon.h"
#include "directsound.h"

int g_sound_playBufferSize = 0xC800;
int MIN_COMFORTABLE_BUFFER_AMOUNT = 0x140;
int MAX_COMFORTABLE_BUFFER_AMOUNT = 0x2710;
int COMFORTABLE_BUFFER_AMOUNT = 0x1770;

bool dsoundplay_initialized = false;
LPDIRECTSOUND8 lpds = NULL;

static int DSound_GetBytesLeft( dsound_sample_t *sample )
{
	return sample->bytesBuffered;
}

HRESULT CreateBasicBuffer( IDirectSoundBuffer **ppDsb, unsigned int sampleRate, int channels, unsigned int bufferSize )
{
	WAVEFORMATEX wfx;
	DSBUFFERDESC dsbdesc;
	HRESULT hr;

	memset(&wfx, 0, sizeof(WAVEFORMATEX));
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = channels;
	wfx.nSamplesPerSec = sampleRate;
	wfx.wBitsPerSample = 16;
	wfx.nBlockAlign = wfx.wBitsPerSample * wfx.nChannels / 8;
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
	memset(&dsbdesc, 0, sizeof(DSBUFFERDESC));
	dsbdesc.dwSize = sizeof(DSBUFFERDESC);
	dsbdesc.dwFlags = DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN | DSBCAPS_CTRLFREQUENCY | DSBCAPS_LOCSOFTWARE;
	dsbdesc.dwBufferBytes = bufferSize;
	dsbdesc.lpwfxFormat = &wfx;
	hr = lpds->CreateSoundBuffer(&dsbdesc, ppDsb, NULL);
	if ( FAILED(hr) )
		Com_Printf("Failed to create sound buffer!\n");
	return hr;
}

int DSound_UpdateSample( dsound_sample_t *sample, char *data, unsigned int data_len )
{
	HRESULT hr;
	int bytesToUpload;
	unsigned int dataOffset;
	void *pLock1;
	void *pLock2;
	DWORD dwLockLen1;
	DWORD dwLockLen2;

	if ( !dsoundplay_initialized )
		return -1;
	if ( !data_len )
		return 0;
	bytesToUpload = (int)I_fmin((float)(sample->dwBufferSize - DSound_GetBytesLeft(sample)), (float)data_len);
	if ( !bytesToUpload )
		return 0;
	hr = sample->DSB->Lock(sample->currentOffset, bytesToUpload, &pLock1, &dwLockLen1, &pLock2, &dwLockLen2, 0);
	if ( FAILED(hr) )
	{
		switch ( hr )
		{
		case DSERR_BUFFERLOST:
			Com_Printf("DSERR_BUFFERLOST\n");
			break;
		case DSERR_PRIOLEVELNEEDED:
			Com_Printf("DSERR_PRIOLEVELNEEDED\n");
			break;
		case DSERR_INVALIDPARAM:
			Com_Printf("DSERR_INVALIDPARAM\n");
			Com_Printf(" Offset : %i, length: %i\n", sample->currentOffset, bytesToUpload);
			break;
		case DSERR_INVALIDCALL:
			Com_Printf("DSERR_INVALIDCALL\n");
			break;
		}
		Com_Printf("Error trying to lock sample!\n");
		return -1;
	}
	dataOffset = 0;
	if ( pLock1 )
	{
		memcpy(pLock1, data, dwLockLen1);
		dataOffset = dwLockLen1;
	}
	if ( pLock2 )
	{
		memcpy(pLock2, &data[dataOffset], dwLockLen2);
		dataOffset += dwLockLen2;
	}
	hr = sample->DSB->Unlock(pLock1, dwLockLen1, pLock2, dwLockLen2);
	if ( FAILED(hr) )
	{
		switch ( hr )
		{
		case DSERR_INVALIDPARAM:
			Com_Printf("DSERR_INVALIDPARAM\n");
			break;
		case DSERR_INVALIDCALL:
			Com_Printf("DSERR_INVALIDCALL\n");
			break;
		}
		Com_Printf("Error trying to unlock sample!\n");
		return -1;
	}
	if ( sample->currentBufferLength < sample->dwBufferSize )
	{
		sample->currentBufferLength += dataOffset;
		if ( sample->currentBufferLength > sample->dwBufferSize )
			sample->currentBufferLength = sample->dwBufferSize;
	}
	sample->bytesBuffered += dataOffset;
	if ( sample->playing )
		sample->lastOffset = sample->currentOffset;
	sample->currentOffset = (dataOffset + sample->currentOffset) % sample->dwBufferSize;
	return dwLockLen2 + dwLockLen1;
}

void DSound_AdjustSamplePlayback( dsound_sample_t *sample, int bytesLeft )
{
	static float maxScalingDownMultiplier = 0.0099999998f;
	static float maxScalingUpMultiplier = 0.0099999998f;
	if ( bytesLeft < MIN_COMFORTABLE_BUFFER_AMOUNT )
	{
		sample->playMode = 1;
	}
	else if ( bytesLeft > MAX_COMFORTABLE_BUFFER_AMOUNT )
	{
		sample->playMode = 0;
	}
	else if ( !sample->playMode && bytesLeft <= COMFORTABLE_BUFFER_AMOUNT
		|| sample->playMode == 1 && bytesLeft >= COMFORTABLE_BUFFER_AMOUNT )
	{
		sample->playMode = 2;
	}
	switch ( sample->playMode )
	{
	case 0:
		{
		float freq = (float)sample->frequency;
		sample->DSB->SetFrequency((DWORD)(freq * (maxScalingUpMultiplier + 1.0f)));
		}
		break;
	case 1:
		sample->DSB->SetFrequency((DWORD)(sample->frequency - sample->frequency * maxScalingDownMultiplier));
		break;
	default:
		sample->DSB->SetFrequency(sample->frequency);
		break;
	}
}

unsigned char DSound_BufferUnderrunOccurred( dsound_sample_t *sample )
{
	return sample->bytesBuffered <= 0;
}

void DSound_HandleBufferUnderrun( dsound_sample_t *sample )
{
	sample->DSB->Stop();
	sample->currentBufferLength = 0;
	sample->currentOffset = 0;
	sample->lastOffset = 0;
	sample->lastPlayPos = 0;
	if ( sample->stopPosition )
		sample->stopPosition -= sample->currentOffset;
	else
		sample->stopPosition = 0;
	sample->DSB->SetCurrentPosition(sample->currentOffset);
	sample->playing = false;
	sample->bytesBuffered = 0;
	sample->lastOffset = sample->currentOffset;
}

void DSound_SampleFrame( dsound_sample_t *sample )
{
	HRESULT hr;
	int bytesLeft;
	int bytesPlayed;
	DWORD dwPlayPos;
	DWORD dwWritePos;

	bytesLeft = DSound_GetBytesLeft(sample);
	if ( !sample->playing )
	{
		if ( bytesLeft >= COMFORTABLE_BUFFER_AMOUNT )
		{
			hr = sample->DSB->Play(0, 0, DSBPLAY_LOOPING);
			if ( FAILED(hr) )
			{
				Com_Printf("Error: Failed to play DirectSound play buffer (%i)!\n", hr);
				return;
			}
			if ( FAILED(sample->DSB->SetCurrentPosition(sample->lastPlayPos)) )
				Com_Printf("Error: Failed to set current position to %i when playing sound buffer!\n", sample->lastOffset);
			sample->playing = true;
			sample->playMode = 2;
		}
		if ( !sample->playing )
			return;
	}
	if ( FAILED(sample->DSB->GetCurrentPosition(&dwPlayPos, &dwWritePos)) )
		Com_Printf("Error: Failed to get cursor positions \n");
	dwWritePos = sample->currentOffset;
	if ( sample->stopPosition >= 0 )
	{
		if ( sample->lastPlayPos <= dwPlayPos )
		{
			if ( (int)sample->lastPlayPos < sample->stopPosition && (int)dwPlayPos >= sample->stopPosition )
			{
				DSound_StopSample(sample);
				return;
			}
		}
		else if ( sample->stopPosition >= (int)sample->lastPlayPos || sample->stopPosition <= (int)dwPlayPos )
		{
			DSound_StopSample(sample);
			return;
		}
	}
	else if ( DSound_BufferUnderrunOccurred(sample) )
	{
		DSound_HandleBufferUnderrun(sample);
		return;
	}
	DSound_AdjustSamplePlayback(sample, bytesLeft);
	bytesPlayed = dwPlayPos - sample->lastPlayPos;
	if ( bytesPlayed < 0 )
		bytesPlayed = dwPlayPos + sample->currentBufferLength - sample->lastPlayPos;
	sample->bytesBuffered -= bytesPlayed;
	if ( DSound_BufferUnderrunOccurred(sample) )
	{
		DSound_HandleBufferUnderrun(sample);
		return;
	}
	sample->lastPlayPos = dwPlayPos;
}

dsound_sample_t *DSound_NewSample( void )
{
	dsound_sample_t *sample;

	if ( !dsoundplay_initialized )
		return NULL;
	sample = DSOUNDRecord_NewSample();
	sample->dwBufferSize = g_sound_playBufferSize;
	if ( FAILED(CreateBasicBuffer(&sample->DSB, sample->frequency, sample->channels, g_sound_playBufferSize)) )
	{
		Com_Printf("Error: Failed to create DirectSound play buffer\n");
		sample->DSB->Release();
		sample->DSB = NULL;
		return NULL;
	}
	return sample;
}

// Name not recovered; starts looping playback with no stop position.
int DSound_PlaySample( dsound_sample_t *sample )
{
	HRESULT hr;

	if ( !dsoundplay_initialized )
		return -1;
	hr = sample->DSB->Play(0, 0, DSBPLAY_LOOPING);
	if ( FAILED(hr) )
	{
		Com_Printf("Error: Failed to play DirectSound play buffer (%i)!\n", hr);
		return -1;
	}
	sample->channel = 1;
	sample->stopPosition = -1;
	sample->playing = true;
	return 1;
}

bool DSound_StopSample( dsound_sample_t *sample )
{
	if ( !dsoundplay_initialized )
		return false;
	if ( !sample->playing )
		return false;
	if ( sample->DSB )
		sample->DSB->Stop();
	sample->stopPosition = -1;
	sample->channel = -1;
	sample->playing = false;
	if ( FAILED(sample->DSB->SetCurrentPosition(0)) )
		Com_Printf("Error: Failed to set current position to %i when playing sound buffer!\n", sample->lastOffset);
	return true;
}

// Name not recovered; stops at the current write offset after a trailing block of silence.
bool DSound_FinishSample( dsound_sample_t *sample )
{
	char silence[0x800];

	sample->stopPosition = sample->currentOffset;
	memset(silence, 0, sizeof(silence));
	DSound_UpdateSample(sample, silence, sizeof(silence));
	return true;
}

int DSound_Init( bool callDsoundInit, HWND handle )
{
	if ( dsoundplay_initialized )
		return 1;
	if ( FAILED(DirectSoundCreate8(NULL, &lpds, NULL)) )
	{
		Com_Printf("ERROR: [play] failed to initialize DirectSound\n");
		return 0;
	}
	if ( FAILED(lpds->SetCooperativeLevel(handle, DSSCL_PRIORITY)) )
	{
		Com_Printf("Error: [play] failure when calling SetCooperativeLevel()\n");
		return 0;
	}
	dsoundplay_initialized = true;
	return 1;
}

void DSound_Shutdown( void )
{
	if ( dsoundplay_initialized )
	{
		lpds->Release();
		dsoundplay_initialized = false;
	}
}

void DSound_Frame( void )
{
}

// Name not recovered; frequency change for a live sample.
bool DSound_SetSampleFrequency( dsound_sample_t *sample, int frequency )
{
	sample->frequency = frequency;
	sample->DSB->SetFrequency(frequency);
	return true;
}

extern int (*current_audioCallback)( audioSample_t * );

void Record_SetRecordingCallback( int (*new_audioCallback)( audioSample_t * ) )
{
	current_audioCallback = new_audioCallback;
}
