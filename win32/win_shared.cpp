#include "../qcommon/qcommon.h"

/*
================
Sys_Milliseconds
================
*/
int sys_timeBase;
int Sys_Milliseconds( void )
{
	int sys_curtime;
	static qboolean initialized = qfalse;

	if ( !initialized )
	{
		sys_timeBase = timeGetTime();
		initialized = qtrue;
	}
	sys_curtime = timeGetTime() - sys_timeBase;

	return sys_curtime;
}

int Sys_MillisecondsRaw( void )
{
	return timeGetTime();
}

/*
================
Sys_SnapVector
================
*/
void Sys_SnapVector( float *v )
{
	int i;
	float f;

	f = *v;
	__asm fld f;
	__asm fistp i;
	*v = i;
	v++;
	f = *v;
	__asm fld f;
	__asm fistp i;
	*v = i;
	v++;
	f = *v;
	__asm fld f;
	__asm fistp i;
	*v = i;
}

// cpuid probing relies on exact register use
#pragma optimize( "", off )

/*
** --------------------------------------------------------------------------------
**
** PROCESSOR STUFF
**
** --------------------------------------------------------------------------------
*/
static void CPUID( int func, unsigned regs[4] )
{
	unsigned regEAX, regEBX, regECX, regEDX;

	__asm pusha
	__asm mov eax, func
	__asm __emit 00fh
	__asm __emit 0a2h
	__asm mov regEAX, eax
	__asm mov regEBX, ebx
	__asm mov regECX, ecx
	__asm mov regEDX, edx
	__asm popa

	regs[0] = regEAX;
	regs[1] = regEBX;
	regs[2] = regECX;
	regs[3] = regEDX;
}

static int IsPentium( void )
{
	__asm
	{
		pushfd // save eflags
		pop eax
		test eax, 0x00200000 // check ID bit
		jz set21 // bit 21 is not set, so jump to set_21
		and eax, 0xffdfffff // clear bit 21
		push eax // save new value in register
		popfd // store new value in flags
		pushfd
		pop eax
		test eax, 0x00200000 // check ID bit
		jz good
		jmp err // cpuid not supported
set21:
		or eax, 0x00200000 // set ID bit
		push eax // store new value
		popfd // store new value in EFLAGS
		pushfd
		pop eax
		test eax, 0x00200000 // if bit 21 is on
		jnz good
		jmp err
	}

err:
	return qfalse;
good:
	return qtrue;
}

static int Is3DNOW( void )
{
	unsigned regs[4];
	char pstring[16];

	CPUID( 0, (unsigned int *)pstring );

	// check AMD-specific functions
	CPUID( 0x80000000, regs );
	if ( regs[0] < 0x80000000 )
		return qfalse;

	// bit 31 of EDX denotes 3DNOW! support
	CPUID( 0x80000001, regs );
	if ( regs[3] & ( 1 << 31 ) )
		return qtrue;

	return qfalse;
}

static int IsKNI( void )
{
	unsigned regs[4];

	// get CPU feature bits
	CPUID( 1, regs );

	// bit 25 of EDX denotes KNI existence
	if ( regs[3] & ( 1 << 25 ) )
		return qtrue;

	return qfalse;
}

static int IsMMX( void )
{
	unsigned regs[4];

	// get CPU feature bits
	CPUID( 1, regs );

	// bit 23 of EDX denotes MMX existence
	if ( regs[3] & ( 1 << 23 ) )
		return qtrue;
	return qfalse;
}

int Sys_GetProcessorId( void )
{
	// verify we're at least a Pentium or 486 w/ CPUID support
	if ( !IsPentium() )
		return CPUID_INTEL_UNSUPPORTED;

	// check for MMX
	if ( !IsMMX() )
	{
		// Pentium or PPro
		return CPUID_INTEL_PENTIUM;
	}

	// see if we're an AMD 3DNOW! processor
	if ( Is3DNOW() )
		return CPUID_AMD_3DNOW;

	// see if we're an Intel Katmai
	if ( IsKNI() )
		return CPUID_INTEL_KATMAI;

	// by default we're functionally a vanilla Pentium/MMX or P2/MMX
	return CPUID_INTEL_MMX;
}

#pragma optimize( "", on )

extern "C" __declspec(dllimport) int __stdcall GetUserNameA(char *lpBuffer, unsigned long *pcbBuffer);

// retail 0x465210
const char *Sys_GetCurrentUser( void )
{
	static char s_userName[1024];
	unsigned long size = sizeof( s_userName );

	if ( !GetUserNameA( s_userName, &size ) )
		strcpy( s_userName, "player" );

	if ( !s_userName[0] )
		strcpy( s_userName, "player" );

	return s_userName;
}
