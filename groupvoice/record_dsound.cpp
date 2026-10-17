#include "../qcommon/qcommon.h"
#include "directsound.h"

int g_sound_recordBufferSize = 0xC800;
int g_sound_recordFrequency = 0x2000;
int g_sound_recordVolume = 0xFF;
int g_sound_channels = 1;

int (*current_audioCallback)( audioSample_t * );
dsound_sample_t *s_recordingSamplePtr;
dsound_sample_t s_recordingSamples[65];
bool g_recording_initialized = false;
bool g_currently_recording = false;
LPDIRECTSOUNDCAPTURE g_pDSCaptureInstance = NULL;
dsound_sample_t *g_current_sample = NULL;

void DSOUNDRecord_UpdateSample( dsound_sample_t *pRecSample )
{
	HRESULT hr;
	int lLockSize;
	int iChannels;
	void *pLock1;
	void *pLock2;
	DWORD dwLockLen1;
	DWORD dwLockLen2;
	DWORD dwTotalLen;
	DWORD dwCapturePos;
	DWORD dwReadPos;
	audioSample_t audioSample;

	if ( !g_recording_initialized )
		return;
	if ( FAILED(pRecSample->DSCB->GetCurrentPosition(&dwCapturePos, &dwReadPos)) )
		Com_Printf("Error: Failed to get cursor positions \n");
	if ( dwReadPos < pRecSample->dwCaptureOffset )
		lLockSize = pRecSample->dwBufferSize - pRecSample->dwCaptureOffset + dwReadPos;
	else
		lLockSize = dwReadPos - pRecSample->dwCaptureOffset;
	lLockSize -= lLockSize % 4;
	if ( !lLockSize )
		return;
	hr = pRecSample->DSCB->Lock(pRecSample->dwCaptureOffset, lLockSize, &pLock1, &dwLockLen1, &pLock2, &dwLockLen2, 0);
	if ( FAILED(hr) )
	{
		switch ( hr )
		{
		case DSERR_INVALIDPARAM:
			Com_Printf("DSERR_INVALIDPARAM\n");
			Com_Printf(" Offset : %i, length: %i\n", pRecSample->dwCaptureOffset, lLockSize);
			break;
		case DSERR_INVALIDCALL:
			Com_Printf("DSERR_INVALIDCALL\n");
			break;
		}
		Com_Printf("Error trying to lock sample!\n");
		return;
	}
	audioSample.stereo = g_sound_channels > 1;
	iChannels = audioSample.stereo ? 2 : 1;
	dwTotalLen = dwLockLen1 + dwLockLen2;
	audioSample.bytesPerSample = 2;
	audioSample.frequency = g_sound_recordFrequency;
	audioSample.sampleOffset = 0;
	audioSample.channels = g_sound_channels;
	if ( pLock1 )
	{
		audioSample.lengthInSamples = dwLockLen1 / (2 * iChannels);
		audioSample.lengthInBytes = dwLockLen1;
		audioSample.buffer = (unsigned char *)pLock1;
		current_audioCallback(&audioSample);
	}
	if ( pLock2 )
	{
		audioSample.lengthInSamples = dwLockLen2 / (2 * iChannels);
		audioSample.lengthInBytes = dwLockLen2;
		audioSample.buffer = (unsigned char *)pLock2;
		current_audioCallback(&audioSample);
	}
	pRecSample->dwCaptureOffset = (pRecSample->dwCaptureOffset + dwTotalLen) % pRecSample->dwBufferSize;
	hr = pRecSample->DSCB->Unlock(pLock1, dwLockLen1, pLock2, dwLockLen2);
	if ( FAILED(hr) )
	{
		switch ( hr )
		{
		case DSERR_INVALIDPARAM:
			Com_Printf("DSERR_INVALIDPARAM\n");
			Com_Printf(" Offset : %i\n", pRecSample->dwCaptureOffset);
			break;
		case DSERR_INVALIDCALL:
			Com_Printf("DSERR_INVALIDCALL\n");
			break;
		}
		Com_Printf("Error trying to unlock sample!\n");
	}
}

void DSOUNDRecord_Frame( void )
{
	if ( g_recording_initialized && g_currently_recording && g_current_sample )
		DSOUNDRecord_UpdateSample(g_current_sample);
}

dsound_sample_t *DSOUNDRecord_NewSample( void )
{
	dsound_sample_t *pRecSample;

	if ( !g_recording_initialized )
		return NULL;
	pRecSample = s_recordingSamplePtr++;
	memset(pRecSample, 0, sizeof(dsound_sample_t));
	pRecSample->frequency = g_sound_recordFrequency;
	pRecSample->volume = g_sound_recordVolume;
	pRecSample->pan = 128;
	pRecSample->channels = g_sound_channels;
	pRecSample->stopPosition = -1;
	pRecSample->playMode = 2;
	return pRecSample;
}

int DSOUNDRecord_DestroySample( dsound_sample_t *pRecSample )
{
	if ( !g_recording_initialized )
		return 0;
	if ( pRecSample->DSB )
	{
		pRecSample->DSB->Release();
		pRecSample->DSB = NULL;
	}
	return 1;
}

HRESULT DSOUNDRecord_Start( dsound_sample_t *pRecSample )
{
	HRESULT hr;
	DSCBUFFERDESC dscbd;
	WAVEFORMATEX wfx;

	if ( !g_recording_initialized )
		return -1;
	if ( !pRecSample )
		return -1;
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = g_sound_channels;
	wfx.wBitsPerSample = 16;
	wfx.nSamplesPerSec = g_sound_recordFrequency;
	wfx.nAvgBytesPerSec = 2 * g_sound_channels * g_sound_recordFrequency;
	wfx.nBlockAlign = 2 * g_sound_channels;
	wfx.cbSize = 0;
	dscbd.dwSize = sizeof(DSCBUFFERDESC);
	dscbd.dwFlags = 0;
	dscbd.dwBufferBytes = g_sound_recordBufferSize;
	dscbd.dwReserved = 0;
	dscbd.lpwfxFormat = &wfx;
	dscbd.dwFXCount = 0;
	dscbd.lpDSCFXDesc = NULL;
	if ( !pRecSample->DSCB )
	{
		hr = g_pDSCaptureInstance->CreateCaptureBuffer(&dscbd, &pRecSample->DSCB, NULL);
		if ( FAILED(hr) )
		{
			switch ( hr )
			{
			case DSERR_BADFORMAT:
				Com_Printf("BADFORMAT\n");
				break;
			case DSERR_GENERIC:
				Com_Printf("GENERIC\n");
				break;
			case DSERR_OUTOFMEMORY:
				Com_Printf("OUTOFMEMORY \n");
				break;
			case DSERR_INVALIDPARAM:
				Com_Printf("INVALIDPARAM\n");
				break;
			case DSERR_NODRIVER:
				Com_Printf("NODRIVER \n");
				break;
			case DSERR_UNINITIALIZED:
				Com_Printf("UNINITIALIZED \n");
				break;
			}
			Com_Printf("error: Failed to allocate Capture Buffer \n");
			return -1;
		}
	}
	pRecSample->dwBufferSize = dscbd.dwBufferBytes;
	pRecSample->dwCaptureOffset = 0;
	hr = pRecSample->DSCB->Start(DSCBSTART_LOOPING);
	if ( SUCCEEDED(hr) )
	{
		g_current_sample = pRecSample;
		g_currently_recording = true;
		return hr;
	}
	Com_Printf("error: Unable to Read to Buffer\n");
	return -1;
}

HRESULT DSOUNDRecord_Stop( dsound_sample_t *pRecSample )
{
	HRESULT hr;

	if ( !g_recording_initialized )
		return -1;
	if ( !pRecSample->DSCB )
		return -1;
	if ( SUCCEEDED(pRecSample->DSCB->Stop()) )
	{
		g_current_sample = NULL;
		g_currently_recording = false;
	}
	else
	{
		Com_Printf("Error stopping recording buffer\n");
	}
	hr = pRecSample->DSCB->Release();
	if ( SUCCEEDED(hr) )
	{
		pRecSample->DSCB = NULL;
		return hr;
	}
	Com_Printf("Error releasing recording buffer\n");
	return hr;
}

int DSOUNDRecord_Init( bool bCallDsoundInit )
{
	HRESULT hr;

	s_recordingSamplePtr = s_recordingSamples;
	g_recording_initialized = false;
	if ( bCallDsoundInit )
	{
		hr = DirectSoundCaptureCreate(NULL, &g_pDSCaptureInstance, NULL);
		if ( FAILED(hr) )
		{
			Com_Printf("Error initializing direct sound instance!  %s\n", hr);
			return 0;
		}
	}
	g_recording_initialized = true;
	return 1;
}

void DSOUNDRecord_Shutdown( void )
{
	HRESULT hr;

	hr = 0;
	if ( g_pDSCaptureInstance )
		hr = g_pDSCaptureInstance->Release();
	g_pDSCaptureInstance = NULL;
	if ( FAILED(hr) )
		Com_Printf("Error releasing direct sound instance!  %s\n", hr);
}

// Name not recovered; sets the capture sample rate.
bool DSOUNDRecord_SetFrequency( int frequency )
{
	if ( frequency <= 0 )
		return false;
	g_sound_recordFrequency = frequency;
	return true;
}
