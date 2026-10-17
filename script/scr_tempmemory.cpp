#include "../qcommon/qcommon.h"
#include "script_public.h"

static int currentPos;

void TempMemoryReset();
char *TempMalloc( int len );
char *TempMallocAlign( int len );
char *TempMallocAlignStrict( int len );
void TempMemorySetPos( char *pos );

/*
==============
TempMalloc
==============
*/
char *TempMalloc( int len )
{
	char *buf;
	int newPos;

	newPos = currentPos + len;
	buf = (char *)Hunk_ReallocateTempMemoryInternal( newPos ) + currentPos;
	currentPos = newPos;
	return buf;
}

/*
==============
TempMallocAlign
==============
*/
char *TempMallocAlign( int len )
{
	return TempMalloc( len );
}

/*
==============
TempMallocAlignStrict
==============
*/
char *TempMallocAlignStrict( int len )
{
	return TempMalloc( len );
}

/*
==============
TempMemoryReset
==============
*/
void TempMemoryReset()
{
	currentPos = 0;
}

/*
==============
TempMemorySetPos
==============
*/
void TempMemorySetPos( char *pos )
{
	char *end;

	end = TempMalloc( 0 );
	currentPos -= end - pos;
	Hunk_ReallocateTempMemoryInternal( currentPos );
}

