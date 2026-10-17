#include "../qcommon/qcommon.h"

struct MemoryFile
{
	byte *buffer;
	int bufferSize;
	int bytesUsed;
	bool errorOnOverflow;
	bool memoryOverflow;
	void (*archiveProc)( MemoryFile *memFile, int bytes, void *data );
};

void MemFile_WriteData( MemoryFile *memFile, int bytes, const void *data );
void MemFile_ReadData( MemoryFile *memFile, int bytes, void *data );

void MemFile_WriteData( MemoryFile *memFile, int bytes, const void *data );
void MemFile_ReadData( MemoryFile *memFile, int bytes, void *data );

static void MemFile_WriteDataForArchive( MemoryFile *memFile, int bytes, void *data )
{
	MemFile_WriteData( memFile, bytes, data );
}

void MemFile_CommonInit( MemoryFile *memFile, int size, void *buffer, bool errorOnOverflow )
{
	memFile->buffer = (byte *)buffer;
	memFile->bufferSize = size;
	memFile->bytesUsed = 0;
	memFile->errorOnOverflow = errorOnOverflow;
	memFile->memoryOverflow = false;
}

void MemFile_InitForReading( MemoryFile *memFile, int size, void *buffer )
{
	MemFile_CommonInit( memFile, size, buffer, true );
	memFile->archiveProc = MemFile_ReadData;
}

void MemFile_InitForWriting( MemoryFile *memFile, int size, void *buffer, bool errorOnOverflow )
{
	MemFile_CommonInit( memFile, size, buffer, errorOnOverflow );
	memFile->archiveProc = MemFile_WriteDataForArchive;
}

// unreferenced; clears only a pointer's worth (sizeof applied to the pointer)
void MemFile_Shutdown( MemoryFile *memFile )
{
	memset( memFile, 0, sizeof( memFile ) );
}

bool MemFile_IsReading( MemoryFile *memFile )
{
	return memFile->archiveProc == MemFile_ReadData;
}

bool MemFile_IsWriting( MemoryFile *memFile )
{
	return memFile->archiveProc == MemFile_WriteDataForArchive;
}

void MemFile_WriteData( MemoryFile *memFile, int bytes, const void *data )
{
	if ( !bytes )
	{
		return;
	}

	if ( memFile->memoryOverflow )
	{
		return;
	}

	if ( bytes + memFile->bytesUsed > memFile->bufferSize )
	{
		if ( memFile->errorOnOverflow )
		{
			Com_Error( ERR_DROP, "Couldn't write %i bytes to %i-byte buffer (only %i bytes free)\n",
			           bytes, memFile->bufferSize, memFile->bufferSize - memFile->bytesUsed );
		}

		memFile->memoryOverflow = true;
		return;
	}

	memcpy( memFile->buffer + memFile->bytesUsed, data, bytes );
	memFile->bytesUsed += bytes;
}

void MemFile_WriteCString( MemoryFile *memFile, const char *string )
{
	MemFile_WriteData( memFile, strlen( string ) + 1, string );
}

void MemFile_SkipData( MemoryFile *memFile, int bytes )
{
	if ( !bytes )
	{
		return;
	}

	if ( memFile->memoryOverflow )
	{
		return;
	}

	if ( bytes + memFile->bytesUsed > memFile->bufferSize )
	{
		if ( memFile->errorOnOverflow )
		{
			Com_Error( ERR_DROP, "Couldn't skip %i bytes from %i-byte buffer (only %i bytes left)\n",
			           bytes, memFile->bufferSize, memFile->bufferSize - memFile->bytesUsed );
		}

		memFile->memoryOverflow = true;
		return;
	}

	memFile->bytesUsed += bytes;
}

void MemFile_ReadData( MemoryFile *memFile, int bytes, void *data )
{
	if ( !bytes )
	{
		return;
	}

	if ( memFile->memoryOverflow )
	{
		return;
	}

	if ( bytes + memFile->bytesUsed > memFile->bufferSize )
	{
		if ( memFile->errorOnOverflow )
		{
			Com_Error( ERR_DROP, "Couldn't read %i bytes from %i-byte buffer (only %i bytes left)\n",
			           bytes, memFile->bufferSize, memFile->bufferSize - memFile->bytesUsed );
		}

		memFile->memoryOverflow = true;
		return;
	}

	memcpy( data, memFile->buffer + memFile->bytesUsed, bytes );
	memFile->bytesUsed += bytes;
}

const char *MemFile_ReadCString( MemoryFile *memFile )
{
	int start;
	const char *string;

	if ( memFile->memoryOverflow )
	{
		return "";
	}

	start = memFile->bytesUsed;

	while ( memFile->buffer[memFile->bytesUsed] )
	{
		memFile->bytesUsed++;

		if ( memFile->bytesUsed == memFile->bufferSize )
		{
			if ( memFile->errorOnOverflow )
			{
				Com_Error( ERR_DROP, "End of memory file while reading string (%i bytes read)\n",
				           memFile->bytesUsed - start );
			}

			memFile->memoryOverflow = true;
			return "";
		}
	}

	memFile->bytesUsed++;
	string = (const char *)&memFile->buffer[start];
	return string;
}

