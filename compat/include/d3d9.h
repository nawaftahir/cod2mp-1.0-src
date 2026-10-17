// Minimal DirectX 9 declarations; the DX9 SDK headers are not part of the toolchain image.
#pragma once

#include <windows.h>

#define D3D_SDK_VERSION 32

#pragma pack( push, 4 )
typedef struct _D3DADAPTER_IDENTIFIER9
{
	char Driver[512];
	char Description[512];
	char DeviceName[32];
	LARGE_INTEGER DriverVersion;
	DWORD VendorId;
	DWORD DeviceId;
	DWORD SubSysId;
	DWORD Revision;
	GUID DeviceIdentifier;
	DWORD WHQLLevel;
} D3DADAPTER_IDENTIFIER9;
#pragma pack( pop )

struct IDirect3D9
{
	virtual HRESULT __stdcall QueryInterface( REFIID riid, void **ppvObj ) = 0;
	virtual ULONG __stdcall AddRef( void ) = 0;
	virtual ULONG __stdcall Release( void ) = 0;
	virtual HRESULT __stdcall RegisterSoftwareDevice( void *pInitializeFunction ) = 0;
	virtual UINT __stdcall GetAdapterCount( void ) = 0;
	virtual HRESULT __stdcall GetAdapterIdentifier( UINT Adapter, DWORD Flags, D3DADAPTER_IDENTIFIER9 *pIdentifier ) = 0;
};

extern "C" IDirect3D9 *__stdcall Direct3DCreate9( UINT SDKVersion );
