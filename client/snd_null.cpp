#include "../qcommon/qcommon.h"

// Sound entry points for the dedicated server. Only SND_Init and
// SND_ShutdownChannels are referenced; the remaining stubs are unreferenced
// and their original names are not recoverable from this binary.

const dvar_t *snd_errorOnMissing;

void SND_NullStub1()
{
}

void SND_NullStub2()
{
}

void SND_NullStub3()
{
}

int SND_NullQuery1()
{
	return 0;
}

int SND_NullQuery2()
{
	return 0;
}

int SND_NullQuery3()
{
	return 0;
}

int SND_NullQuery4()
{
	return 0;
}

int SND_NullQuery5()
{
	return 0;
}

void SND_NullStub4()
{
}

void SND_NullStub5()
{
}

void SND_NullStub6()
{
}

void SND_NullStub7()
{
}

void SND_NullStub8()
{
}

void SND_NullStub9()
{
}

void SND_NullStub10()
{
}

int SND_NullQuery6()
{
	return 0;
}

void SND_NullStub11()
{
}

int SND_NullQuery7()
{
	return 0;
}

void SND_NullStub12()
{
}

void SND_NullStub13()
{
}

void SND_NullStub14()
{
}

void SND_NullStub15()
{
}

void SND_NullStub16()
{
}

void SND_NullStub17()
{
}

void SND_Init()
{
	snd_errorOnMissing = Dvar_RegisterBool("snd_errorOnMissing", 0, DVAR_ARCHIVE | DVAR_CHANGEABLE_RESET);
}

void SND_NullStub18()
{
}

void SND_ShutdownChannels()
{
}

void SND_NullStub19()
{
}

void SND_NullStub20()
{
}

int SND_NullQuery8()
{
	return 0;
}

void SND_NullStub21()
{
}

void SND_NullStub22()
{
}

int SND_NullQuery9()
{
	return 0;
}
