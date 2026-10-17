#include "../qcommon/qcommon.h"
#include <d3d9.h>
#include "win_local.h"

const char *Win_LocalizeRef( const char *ref );
void Sys_NormalExit( void );

double Sys_BenchmarkGHz( void );
int Sys_SystemMemoryMB( void );
void Sys_DetectVideoCard( char *description );
bool Sys_SupportsSSE( void );

// rounds up for anything past an exact integer
int Sys_RoundUp( float x )
{
	int result;
	const double bias = 0.5 - 1.0 / ( 1 << 30 );

	__asm fld x
	__asm fadd bias
	__asm fistp result

	return result;
}

/*
==============
Sys_BenchmarkGHz
==============
*/
double Sys_BenchmarkGHz( void )
{
	unsigned __int64 tscStart;
	unsigned __int64 tscEnd;
	LARGE_INTEGER freq;
	LARGE_INTEGER qpcEnd;
	LARGE_INTEGER qpcStart;
	double ghz;

	Sleep( 0 );

	tscStart = 0;
	tscEnd = 0;
	qpcStart.QuadPart = 0;
	qpcEnd.QuadPart = 0;

	QueryPerformanceFrequency( &freq );
	QueryPerformanceCounter( &qpcStart );
	__asm rdtsc
	__asm mov dword ptr tscStart, eax
	__asm mov dword ptr tscStart + 4, edx
	QueryPerformanceCounter( &qpcStart );
	Sleep( 250 );
	__asm rdtsc
	__asm mov dword ptr tscEnd, eax
	__asm mov dword ptr tscEnd + 4, edx
	QueryPerformanceCounter( &qpcEnd );

	ghz = (double)(__int64)( ( tscEnd - tscStart ) * freq.QuadPart ) * 1e-9 / (double)( qpcEnd.QuadPart - qpcStart.QuadPart );
	return ghz;
}

/*
==============
Sys_SystemMemoryMB
==============
*/
int Sys_SystemMemoryMB( void )
{
	HMODULE hm;
	BOOL (__stdcall *MemStatEx)( MEMORYSTATUSEX * );
	MEMORYSTATUSEX statusEx;
	MEMORYSTATUS status;
	int sysMB;

	hm = GetModuleHandleA( "kernel32.dll" );
	if ( hm )
	{
		MemStatEx = (BOOL (__stdcall *)( MEMORYSTATUSEX * ))GetProcAddress( hm, "GlobalMemoryStatusEx" );
		if ( MemStatEx )
		{
			statusEx.dwLength = sizeof( statusEx );
			MemStatEx( &statusEx );

			if ( statusEx.ullAvailVirtual < 0x8000000 )
			{
				if ( MessageBoxA( NULL, Win_LocalizeRef( "WIN_LOW_MEMORY_BODY" ), Win_LocalizeRef( "WIN_LOW_MEMORY_TITLE" ), MB_YESNO | MB_ICONWARNING ) != IDYES )
				{
					Sys_NormalExit();
					exit( 0 );
				}
			}

			sysMB = Sys_RoundUp( (float)statusEx.ullTotalPhys * ( 1.0f / ( 1024 * 1024 ) ) );
			if ( (float)sysMB * ( 1024.0f * 1024.0f ) < (float)statusEx.ullTotalPhys || sysMB > 1024 )
				sysMB = 1024;
			return sysMB;
		}
	}

	status.dwLength = sizeof( status );
	GlobalMemoryStatus( &status );

	if ( status.dwAvailVirtual < 0x8000000 )
	{
		if ( MessageBoxA( NULL, Win_LocalizeRef( "WIN_LOW_MEMORY_BODY" ), Win_LocalizeRef( "WIN_LOW_MEMORY_TITLE" ), MB_YESNO | MB_ICONWARNING ) != IDYES )
		{
			Sys_NormalExit();
			exit( 0 );
		}
	}

	sysMB = Sys_RoundUp( (float)status.dwTotalPhys * ( 1.0f / ( 1024 * 1024 ) ) );
	if ( (float)sysMB * ( 1024.0f * 1024.0f ) < (float)status.dwTotalPhys || sysMB > 1024 )
		sysMB = 1024;
	return sysMB;
}

/*
==============
Sys_DetectVideoCard
==============
*/
void Sys_DetectVideoCard( char *description )
{
	D3DADAPTER_IDENTIFIER9 id;
	IDirect3D9 *d3d9;

	strcpy( description, "Unknown video card" );

	d3d9 = Direct3DCreate9( D3D_SDK_VERSION );
	if ( !d3d9 )
		return;

	if ( d3d9->GetAdapterIdentifier( 0, 0, &id ) >= 0 )
		strcpy( description, id.Description );

	d3d9->Release();
}

/*
==============
Sys_SupportsSSE
==============
*/
bool Sys_SupportsSSE( void )
{
	bool result;

	result = false;

	try
	{
		__asm
		{
			mov eax, 1
			cpuid
			xor eax, eax
			test edx, 0x2000000
			je noSSE
			xorps xmm0, xmm0
			inc eax
			mov result, al
noSSE:
		}
	}
	catch ( ... )
	{
	}

	return result;
}

