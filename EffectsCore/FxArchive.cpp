#include <string.h>

struct MemoryFile
{
	unsigned char *buffer;
	int bufferSize;
	int bytesUsed;
	bool errorOnOverflow;
	bool memoryOverflow;
	void (*archiveProc)( MemoryFile *memFile, int bytes, void *data );
};

void MemFile_WriteData( MemoryFile *memFile, int bytes, const void *data );
void MemFile_ReadData( MemoryFile *memFile, int bytes, void *data );

struct FxEffectDef
{
	const char *name;
};
struct FxCurve;
struct XModel;
struct Material;
struct FxCurveIterator
{
	const FxCurve *master;
	int currentKeyIndex;
};
struct FxChannelInstance
{
	FxCurveIterator curveIterator;
	float scale;
};
// Only the member called here.
struct trace_t;
#include "FxHelper.h"
extern FxHelper *theFxHelper;
FxEffectDef *FX_RegisterEffect( const char *name );
Material *FX_RegisterMaterial( const char *name );
XModel *FX_ModelRegister( const char *name );
const char *XModelGetName( const XModel *model );
int Com_sprintf( char *dest, size_t size, const char *fmt, ... );
// Run-length coded stream: a control byte gives a literal count and a
// following run of zero bytes.
class FxArchive
{
public:
	FxArchive();
	void BeginReading( MemoryFile *memFile );
	void BeginWriting( MemoryFile *memFile );
	void ArchiveData( void *p, int byteCount );
	void ArchiveEffect( const FxEffectDef **fx );
	void ArchiveMaterial( Material **material );
	void ArchiveModel( XModel **model );
	void ArchiveChannelInstance( FxChannelInstance *channelInstance );
private:
	FxEffectDef *ReadEffect();
	Material *ReadMaterial();
	XModel *ReadModel();
	void ReadChannelInstance( FxChannelInstance *channelInstance );
	void ReadData( void *p, int byteCount );
	void WriteEffect( const FxEffectDef *fx );
	void WriteMaterial( Material *material );
	void WriteModel( XModel *model );
	void WriteChannelInstance( const FxChannelInstance *channelInstance );
	void WriteData( const void *p, int byteCount );
	unsigned char ReadByte()
	{
		unsigned char value;
		ReadData( &value, 1 );
		return value;
	}
	float ReadFloat()
	{
		float value;
		ReadData( &value, 4 );
		return value;
	}
	void WriteByte( unsigned char value )
	{
		WriteData( &value, 1 );
	}
	void WriteFloat( float value )
	{
		WriteData( &value, 4 );
	}
	MemoryFile *memFile;
	bool isReading;
	bool isWriting;
	int byteCount;
	int literalCount;
	int zeroCount;
	int controlPos;
};

inline void MemFile_WriteByte( MemoryFile *memFile, unsigned char value )
{
	MemFile_WriteData( memFile, 1, &value );
}

inline unsigned char MemFile_ReadByte( MemoryFile *memFile )
{
	unsigned char value;

	MemFile_ReadData( memFile, 1, &value );
	return value;
}

FxArchive::FxArchive()
{
	memFile = 0;
	isReading = false;
	isWriting = false;
	byteCount = 0;
	controlPos = 0;
	literalCount = 0;
	zeroCount = 0;
}

void FxArchive::BeginReading( MemoryFile *file )
{
	memFile = file;
	isReading = true;
	isWriting = false;
	byteCount = 0;
	controlPos = memFile->bytesUsed;
	literalCount = 0;
	zeroCount = 0;
}

void FxArchive::BeginWriting( MemoryFile *file )
{
	memFile = file;
	isReading = false;
	isWriting = true;
	byteCount = 0;
	controlPos = memFile->bytesUsed;
	literalCount = 0;
	zeroCount = 0;
}

FxEffectDef *FxArchive::ReadEffect()
{
	unsigned char len;
	char effectName[64];
	char filename[64];

	len = ReadByte();
	if ( !len || len >= 64 )
		return 0;
	ReadData( effectName, len );
	effectName[len] = 0;
	Com_sprintf( filename, 64, "fx/%s", effectName );
	return FX_RegisterEffect( filename );
}

Material *FxArchive::ReadMaterial()
{
	unsigned char len;
	char materialName[64];

	len = ReadByte();
	if ( !len || len >= 64 )
		return 0;
	ReadData( materialName, len );
	materialName[len] = 0;
	return FX_RegisterMaterial( materialName );
}

void FxArchive::ArchiveMaterial( Material **material )
{
	if ( isReading )
		*material = ReadMaterial();
	else
		WriteMaterial( *material );
}

void FxArchive::ArchiveModel( XModel **model )
{
	if ( isReading )
		*model = ReadModel();
	else
		WriteModel( *model );
}

XModel *FxArchive::ReadModel()
{
	unsigned char len;
	char modelName[64];

	len = ReadByte();
	if ( !len || len >= 64 )
		return 0;
	ReadData( modelName, len );
	modelName[len] = 0;
	return FX_ModelRegister( modelName );
}

void FxArchive::ReadChannelInstance( FxChannelInstance *channelInstance )
{
	channelInstance->curveIterator.currentKeyIndex = 0;
	channelInstance->scale = ReadFloat();
	channelInstance->curveIterator.master = 0;
}

void FxArchive::ReadData( void *p, int count )
{
	unsigned char *out;
	unsigned char control;

	byteCount += count;
	out = (unsigned char *)p;
	for ( ;; )
	{
		while ( literalCount )
		{
			literalCount--;
			count--;
			*out = MemFile_ReadByte( memFile );
			out++;
			if ( !count )
				return;
		}
		while ( zeroCount && count )
		{
			zeroCount--;
			count--;
			*out = 0;
			out++;
			if ( !count )
				return;
		}
		control = MemFile_ReadByte( memFile );
		switch ( control & 0xC0 )
		{
		case 0x00:
			literalCount = 1;
			zeroCount = ( control & 0x3F ) + 1;
			break;
		case 0x40:
			literalCount = 2;
			zeroCount = ( control & 0x3F ) + 1;
			break;
		case 0x80:
			literalCount = 4;
			zeroCount = ( control & 0x3F ) + 1;
			break;
		default:
			literalCount = ( control & 0x3F ) + 1;
			zeroCount = 0;
			break;
		}
	}
}

void FxArchive::WriteEffect( const FxEffectDef *fx )
{
	int len;
	const char *effectName;

	if ( fx )
		effectName = fx->name;
	else
		effectName = "";
	len = strlen( effectName );
	WriteByte( len );
	if ( len )
		WriteData( effectName, len );
}

void FxArchive::WriteMaterial( Material *material )
{
	int len;
	const char *materialName;

	if ( material )
		materialName = theFxHelper->GetMaterialName( material );
	else
		materialName = "";
	len = strlen( materialName );
	WriteByte( len );
	if ( len )
		WriteData( materialName, len );
}

void FxArchive::WriteModel( XModel *model )
{
	int len;
	const char *modelName;

	modelName = model ? XModelGetName( model ) : "";
	len = strlen( modelName );
	WriteByte( len );
	if ( len )
		WriteData( modelName, len );
}

void FxArchive::WriteChannelInstance( const FxChannelInstance *channelInstance )
{
	WriteFloat( channelInstance->scale );
}

void FxArchive::WriteData( const void *p, int count )
{
	const unsigned char *in;
	int i;

	byteCount += count;
	in = (const unsigned char *)p;
	if ( controlPos == memFile->bytesUsed )
	{
		MemFile_WriteByte( memFile, 0xC0 );
		MemFile_WriteByte( memFile, *in );
		in++;
		count--;
	}
	i = 0;
	while ( i < count )
	{
		switch ( memFile->buffer[controlPos] & 0xC0 )
		{
		case 0x40:
		case 0x00:
		case 0x80:
			while ( 1 )
			{
				if ( i >= count )
					break;
				if ( in[i] || ( memFile->buffer[controlPos] & 0x3F ) == 0x3F )
				{
					controlPos = memFile->bytesUsed;
					MemFile_WriteByte( memFile, 0xC0 );
					MemFile_WriteByte( memFile, in[i] );
					i++;
					break;
				}
				memFile->buffer[controlPos]++;
				i++;
			}
			break;
		default:
			while ( 1 )
			{
				if ( i >= count )
					break;
				if ( ( memFile->buffer[controlPos] & 0x3F ) == 0 )
				{
					if ( !in[i] )
					{
						memFile->buffer[controlPos] = 0x00;
						i++;
						break;
					}
				}
				else if ( ( memFile->buffer[controlPos] & 0x3F ) == 1 )
				{
					if ( !in[i] )
					{
						memFile->buffer[controlPos] = 0x40;
						i++;
						break;
					}
				}
				else if ( ( memFile->buffer[controlPos] & 0x3F ) == 3 )
				{
					if ( !in[i] )
					{
						memFile->buffer[controlPos] = 0x80;
						i++;
						break;
					}
				}
				else if ( ( memFile->buffer[controlPos] & 0x3F ) == 0x3F )
				{
					controlPos = memFile->bytesUsed;
					MemFile_WriteByte( memFile, 0xC0 );
					MemFile_WriteByte( memFile, in[i] );
					i++;
					continue;
				}
				else if ( ( memFile->buffer[controlPos] & 0x3F ) > 5 && !in[i]
					&& !memFile->buffer[memFile->bytesUsed - 2] && !memFile->buffer[memFile->bytesUsed - 1] )
				{
					// Split a long literal run so its last two zero bytes become a zero run.
					memFile->buffer[controlPos] -= 3;
					memFile->buffer[memFile->bytesUsed - 2] = memFile->buffer[memFile->bytesUsed - 3];
					memFile->buffer[memFile->bytesUsed - 3] = 0x02;
					controlPos = memFile->bytesUsed - 3;
					memFile->bytesUsed--;
					i++;
					break;
				}
				memFile->buffer[controlPos]++;
				MemFile_WriteByte( memFile, in[i] );
				i++;
			}
			break;
		}
	}
}

void FxArchive::ArchiveData( void *p, int count )
{
	if ( isReading )
		ReadData( p, count );
	else
		WriteData( p, count );
}

void FxArchive::ArchiveEffect( const FxEffectDef **fx )
{
	if ( isReading )
		*fx = ReadEffect();
	else
		WriteEffect( *fx );
}

void FxArchive::ArchiveChannelInstance( FxChannelInstance *channelInstance )
{
	if ( isReading )
		ReadChannelInstance( channelInstance );
	else
		WriteChannelInstance( channelInstance );
}

