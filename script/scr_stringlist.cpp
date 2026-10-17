#include "../qcommon/qcommon.h"
#include "script_public.h"

static scrStringGlob_t scrStringGlob;

RefString *GetRefString( const char *str );
void SL_RemoveRefToString( unsigned int stringValue );
void SL_RemoveRefToStringOfLen( unsigned int stringValue, unsigned int len );
void SL_Clear();
int I_strlen( const char *s );

RefString *GetRefString( const char *str );
RefString *GetRefString( const char *str );
const char *SL_ConvertToString( unsigned int stringValue );
int SL_GetRefStringLen( RefString *refString );
int SL_GetStringLen( unsigned int stringValue );
int SL_ConvertFromRefString( RefString *refString );
unsigned int SL_ConvertFromString( const char *str );
unsigned int GetHashCode( const char *str, unsigned int len );
void SL_Init();
void SL_Restart();
void SL_Shutdown();
unsigned int SL_FindStringOfLen( const char *str, unsigned int len );
unsigned int SL_FindString( const char *str );
unsigned int SL_FindLowercaseString( const char *str );
void SL_AddUserInternal( RefString *refStr, unsigned int user );
void SL_AddUser( unsigned int stringValue, unsigned int user );
unsigned int SL_GetStringOfLen( const char *str, unsigned int user, unsigned int len, int type );
unsigned int SL_GetString_( const char *str, unsigned int user, int type );
unsigned int SL_GetString( const char *str, unsigned int user );
unsigned int SL_GetLowercaseStringOfLen( const char *str, unsigned int user, unsigned int len, int type );
unsigned int SL_GetLowercaseString_( const char *str, unsigned int user, int type );
unsigned int SL_GetLowercaseString( const char *str, unsigned int user );
unsigned int SL_ConvertToLowercase( unsigned int stringValue, unsigned int user, int type );
void SL_TransferRefToUser( unsigned int stringValue, unsigned int user );
void SL_AddRefToString( unsigned int stringValue );
void SL_FreeString( unsigned int stringValue, RefString *refStr, unsigned int len );
void SL_RemoveRefToString( unsigned int stringValue );
void SL_RemoveAllRefToString( unsigned int stringValue );
void SL_RemoveRefToStringOfLen( unsigned int stringValue, unsigned int len );
void Scr_SetString( unsigned short *to, unsigned int from );
void Scr_SetStringFromCharString( unsigned short *to, const char *from );
unsigned int Scr_AllocString( const char *s, int user );
unsigned int SL_GetStringForFloat( float f );
unsigned int SL_GetStringForInt( int i );
unsigned int SL_GetStringForVector( const vec3_t v );
void SL_ShutdownSystem( unsigned int user );
void Scr_ShutdownGameStrings();
void SL_TransferSystem( unsigned int from, unsigned int to );
void SL_Clear();
void SL_CreateCanonicalFilename( char *newFilename, const char *filename, int count );
unsigned int Scr_CreateCanonicalFilename( const char *filename );
int I_strlen( const char *s );

/*
==============
SL_Restart
==============
*/
void SL_Restart()
{
	if ( scrStringGlob.inited )
	{
		SL_Clear();
	}
	else
	{
		SL_Init();
	}
}

/*
==============
GetRefString
==============
*/
RefString *GetRefString( unsigned int stringValue )
{
	return (RefString *)( scrMemTreePub.mt_buffer + stringValue * sizeof( MemoryNode ) );
}

/*
==============
GetRefString
==============
*/
RefString *GetRefString( const char *str )
{
	return (RefString *)( str - REFSTRING_STRING_OFFSET );
}

/*
==============
SL_ConvertToString
==============
*/
const char *SL_ConvertToString( unsigned int stringValue )
{
	return stringValue ? GetRefString( stringValue )->str : NULL;
}

/*
==============
SL_GetRefStringLen
==============
*/
int SL_GetRefStringLen( RefString *refString )
{
	int len;
	int result;

	len = ( refString->byteLen - 1 ) & 0xff;

	while ( 1 )
	{
		if ( !refString->str[len] )
		{
			result = len;
			break;
		}

		len += 256;
	}

	return result;
}

/*
==============
SL_GetStringLen
==============
*/
int SL_GetStringLen( unsigned int stringValue )
{
	RefString *refStr;

	refStr = GetRefString( stringValue );
	return SL_GetRefStringLen( refStr );
}

/*
==============
SL_ConvertFromRefString
==============
*/
int SL_ConvertFromRefString( RefString *refString )
{
	return ( (char *)refString - scrMemTreePub.mt_buffer ) / (int)sizeof( MemoryNode );
}

/*
==============
SL_ConvertFromString
==============
*/
unsigned int SL_ConvertFromString( const char *str )
{
	return SL_ConvertFromRefString( GetRefString( str ) );
}

/*
==============
GetHashCode
==============
*/
unsigned int GetHashCode( const char *str, unsigned int len )
{
	unsigned int hash;
	const char *p;
	int c;

	if ( len < 256 )
	{
		hash = 0;
		p = str;

		while ( len )
		{
			c = *p;
			hash *= 31;
			hash += c;
			p++;
			len--;
		}

		return hash % ( HASH_TABLE_SIZE - 1 ) + 1;
	}
	else
	{
		return ( len >> 2 ) % ( HASH_TABLE_SIZE - 1 ) + 1;
	}
}

/*
==============
SL_Init
==============
*/
void SL_Init()
{
	unsigned int hash;
	HashEntry *entry;
	unsigned int prev;

	MT_Init();

	scrStringGlob.hashTable[0].status_next = 0;
	prev = 0;

	for ( hash = 1; hash < HASH_TABLE_SIZE; hash++ )
	{
		entry = &scrStringGlob.hashTable[hash];
		entry->status_next = 0;

		scrStringGlob.hashTable[prev].status_next |= hash;

		entry->prev = prev;
		prev = hash;
	}

	scrStringGlob.hashTable[0].prev = prev;
	scrStringGlob.inited = true;
}

/*
==============
SL_Shutdown
==============
*/
void SL_Shutdown()
{
	if ( !scrStringGlob.inited )
	{
		return;
	}

	scrStringGlob.inited = false;
}

/*
==============
SL_FindStringOfLen
==============
*/
unsigned int SL_FindStringOfLen( const char *str, unsigned int len )
{
	HashEntry *entry;
	unsigned int prev;
	HashEntry *newEntry;
	RefString *refStr;
	unsigned int hash;
	unsigned int newIndex;
	int byteLen;
	unsigned int stringValue;

	hash = GetHashCode(str, len);
	entry = &scrStringGlob.hashTable[hash];

	if ( (entry->status_next & HASH_STAT_MASK) == HASH_STAT_HEAD )
	{
		byteLen = (unsigned char)len;
		refStr = GetRefString(entry->prev);

		if ( refStr->byteLen == byteLen && !memcmp(refStr->str, str, len) )
		{
			stringValue = entry->str;
			return stringValue;
		}

		prev = hash;
		newIndex = entry->status_next & HASH_NEXT_MASK;
		newEntry = &scrStringGlob.hashTable[newIndex];

		while ( newEntry != entry )
		{
			refStr = GetRefString(newEntry->prev);

			if ( refStr->byteLen == byteLen && !memcmp(refStr->str, str, len) )
			{
				scrStringGlob.hashTable[prev].status_next = scrStringGlob.hashTable[prev].status_next & HASH_STAT_MASK | newEntry->status_next & HASH_NEXT_MASK;
				newEntry->status_next = newEntry->status_next & HASH_STAT_MASK | entry->status_next & HASH_NEXT_MASK;
				entry->status_next = entry->status_next & HASH_STAT_MASK | newIndex;

				stringValue = newEntry->str;
				newEntry->prev = entry->prev;
				entry->str = stringValue;

				return stringValue;
			}

			prev = newIndex;
			newIndex = newEntry->status_next & HASH_NEXT_MASK;
			newEntry = &scrStringGlob.hashTable[newIndex];
		}
	}

	return 0;
}

/*
==============
SL_FindString
==============
*/
unsigned int SL_FindString( const char *str )
{
	return SL_FindStringOfLen( str, I_strlen( str ) + 1 );
}

/*
==============
SL_FindLowercaseString
==============
*/
unsigned int SL_FindLowercaseString( const char *str )
{
	char buf[8192];
	int i, len;

	len = I_strlen(str) + 1;

	if ( len > (int)sizeof(buf) )
	{
		return 0;
	}

	for ( i = 0; i < len; i++ )
	{
		buf[i] = tolower(str[i]);
	}

	return SL_FindStringOfLen(buf, len);
}

/*
==============
SL_AddUserInternal
==============
*/
void SL_AddUserInternal( RefString *refStr, unsigned int user )
{
	if ( user & refStr->user )
	{
		return;
	}

	refStr->user |= user;
	refStr->refCount++;
}

/*
==============
SL_AddUser
==============
*/
void SL_AddUser( unsigned int stringValue, unsigned int user )
{
	SL_AddUserInternal( GetRefString( stringValue ), user );
}

/*
==============
SL_GetStringOfLen
==============
*/
unsigned int SL_GetStringOfLen( const char *str, unsigned int user, unsigned int len, int type )
{
	HashEntry *entry;
	unsigned int prev;
	unsigned int next;
	unsigned int nextFree;
	HashEntry *newEntry;
	RefString *refStr;
	unsigned int hash;
	unsigned int newIndex;
	unsigned int lenByte;
	unsigned int stringValue;

	hash = GetHashCode(str, len);
	entry = &scrStringGlob.hashTable[hash];
	lenByte = (unsigned char)len;

	if ( ( entry->status_next & HASH_STAT_MASK ) == HASH_STAT_HEAD )
	{
		refStr = GetRefString(entry->str);

		if ( refStr->byteLen == lenByte && !memcmp(refStr->str, str, len) )
		{
			SL_AddUserInternal(refStr, user);

			stringValue = entry->str;
			return stringValue;
		}

		prev = hash;
		newIndex = entry->status_next & HASH_NEXT_MASK;
		newEntry = &scrStringGlob.hashTable[newIndex];

		while ( 1 )
		{
			if ( newEntry == entry )
			{
				break;
			}

			refStr = GetRefString(newEntry->prev);

			if ( refStr->byteLen == lenByte && !memcmp(refStr->str, str, len) )
			{
				scrStringGlob.hashTable[prev].status_next = scrStringGlob.hashTable[prev].status_next & HASH_STAT_MASK | newEntry->status_next & HASH_NEXT_MASK;

				newEntry->status_next = newEntry->status_next & HASH_STAT_MASK | entry->status_next & HASH_NEXT_MASK;
				entry->status_next = entry->status_next & HASH_STAT_MASK | newIndex;

				stringValue = newEntry->str;
				newEntry->prev = entry->prev;
				entry->str = stringValue;

				SL_AddUserInternal(refStr, user);

				return stringValue;
			}

			prev = newIndex;
			newIndex = newEntry->status_next & HASH_NEXT_MASK;
			newEntry = &scrStringGlob.hashTable[newIndex];
		}

		newIndex = scrStringGlob.hashTable[0].status_next;

		if ( !newIndex )
		{
			Scr_DumpScriptThreads();
			Scr_DumpScriptVariablesDefault();
			Com_Error(ERR_DROP, "\x15" "exceeded maximum number of script strings\n");
		}

		stringValue = MT_AllocIndex( len + REFSTRING_STRING_OFFSET, type );
		newEntry = &scrStringGlob.hashTable[newIndex];

		nextFree = newEntry->status_next & HASH_NEXT_MASK;
		scrStringGlob.hashTable[0].status_next = nextFree;
		scrStringGlob.hashTable[nextFree].prev = 0;

		newEntry->status_next = entry->status_next & HASH_NEXT_MASK | HASH_STAT_MOVABLE;
		entry->status_next = entry->status_next & HASH_STAT_MASK | newIndex & HASH_NEXT_MASK;
		newEntry->prev = entry->prev;
	}
	else
	{
		if ( !( entry->status_next & HASH_STAT_MASK ) )
		{
			stringValue = MT_AllocIndex( len + REFSTRING_STRING_OFFSET, type );

			prev = entry->prev;
			next = entry->status_next & HASH_NEXT_MASK;

			scrStringGlob.hashTable[prev].status_next = scrStringGlob.hashTable[prev].status_next & HASH_STAT_MASK | next;
			scrStringGlob.hashTable[next].prev = prev;
		}
		else
		{
			next = (unsigned short)entry->status_next & HASH_NEXT_MASK;
			prev = next;

			while ( 1 )
			{
				if ( ( scrStringGlob.hashTable[prev].status_next & HASH_NEXT_MASK ) == hash )
				{
					break;
				}

				prev = scrStringGlob.hashTable[prev].status_next & HASH_NEXT_MASK;
			}

			newIndex = scrStringGlob.hashTable[0].status_next;

			if ( !newIndex )
			{
				Scr_DumpScriptThreads();
				Scr_DumpScriptVariablesDefault();
				Com_Error(ERR_DROP, "\x15" "exceeded maximum number of script strings\n");
			}

			stringValue = MT_AllocIndex( len + REFSTRING_STRING_OFFSET, type );
			newEntry = &scrStringGlob.hashTable[newIndex];

			nextFree = newEntry->status_next & HASH_NEXT_MASK;
			scrStringGlob.hashTable[0].status_next = nextFree;
			scrStringGlob.hashTable[nextFree].prev = 0;
			scrStringGlob.hashTable[prev].status_next = scrStringGlob.hashTable[prev].status_next & HASH_STAT_MASK | newIndex;

			newEntry->status_next = next | HASH_STAT_MOVABLE;
			newEntry->prev = entry->prev;
		}

		entry->status_next = hash | HASH_STAT_HEAD;
	}

	entry->str = stringValue;

	refStr = GetRefString(stringValue);
	memcpy(refStr->str, str, len);

	refStr->user = user;
	refStr->refCount = 1;
	refStr->byteLen = lenByte;

	return stringValue;
}

/*
==============
SL_GetString_
==============
*/
unsigned int SL_GetString_( const char *str, unsigned int user, int type )
{
	return SL_GetStringOfLen( str, user, I_strlen( str ) + 1, type );
}

/*
==============
SL_GetString
==============
*/
unsigned int SL_GetString( const char *str, unsigned int user )
{
	return SL_GetString_( str, user, 6 );
}

/*
==============
SL_GetLowercaseStringOfLen
==============
*/
unsigned int SL_GetLowercaseStringOfLen( const char *str, unsigned int user, unsigned int len, int type )
{
	char buf[8192];
	unsigned int i;

	if ( len > sizeof(buf) )
	{
		Com_Error(ERR_DROP, "max string length exceeded: \"%s\"", str);
		return 0;
	}

	for ( i = 0; i < len; i++ )
	{
		buf[i] = tolower(str[i]);
	}

	return SL_GetStringOfLen(buf, user, len, type);
}

/*
==============
SL_GetLowercaseString_
==============
*/
unsigned int SL_GetLowercaseString_( const char *str, unsigned int user, int type )
{
	return SL_GetLowercaseStringOfLen( str, user, I_strlen( str ) + 1, type );
}

/*
==============
SL_GetLowercaseString
==============
*/
unsigned int SL_GetLowercaseString( const char *str, unsigned int user )
{
	return SL_GetLowercaseString_( str, user, 6 );
}

/*
==============
SL_ConvertToLowercase
==============
*/
unsigned int SL_ConvertToLowercase( unsigned int stringValue, unsigned int user, int type )
{
	char buf[8192];
	unsigned int i;
	const char *str;
	unsigned int ns;
	unsigned int len;

	len = SL_GetStringLen(stringValue) + 1;

	if ( len > sizeof(buf) )
	{
		return stringValue;
	}

	str = SL_ConvertToString(stringValue);

	for ( i = 0; i < len; i++ )
	{
		buf[i] = tolower(str[i]);
	}

	ns = SL_GetStringOfLen(buf, user, len, type);
	SL_RemoveRefToString(stringValue);

	return ns;
}

/*
==============
SL_TransferRefToUser
==============
*/
void SL_TransferRefToUser( unsigned int stringValue, unsigned int user )
{
	RefString *refStr = GetRefString( stringValue );

	if ( user & refStr->user )
	{
		refStr->refCount--;
		return;
	}

	refStr->user |= user;
}

/*
==============
SL_AddRefToString
==============
*/
void SL_AddRefToString( unsigned int stringValue )
{
	RefString *refStr = GetRefString(stringValue);

	refStr->refCount++;
}

/*
==============
SL_FreeString
==============
*/
void SL_FreeString( unsigned int stringValue, RefString *refStr, unsigned int len )
{
	HashEntry *entry, *newEntry;
	unsigned int index, newIndex, newNext, prev;
	const char *str;

	str = refStr->str;
	index = GetHashCode(str, len);
	entry = &scrStringGlob.hashTable[index];

	MT_FreeIndex(stringValue, len + REFSTRING_STRING_OFFSET);

	newIndex = entry->status_next & HASH_NEXT_MASK;
	newEntry = &scrStringGlob.hashTable[newIndex];

	if ( entry->str == stringValue )
	{
		if ( newEntry != entry )
		{
			entry->status_next = newEntry->status_next & HASH_NEXT_MASK | HASH_STAT_HEAD;
			entry->prev = newEntry->prev;

			scrStringGlob.nextFreeEntry = entry;
		}
		else
		{
			newEntry = entry;
			newIndex = index;
		}
	}
	else
	{
		prev = index;

		while ( 1 )
		{
			if ( newEntry->str == stringValue )
			{
				scrStringGlob.hashTable[prev].status_next = scrStringGlob.hashTable[prev].status_next & HASH_STAT_MASK | newEntry->status_next & HASH_NEXT_MASK;
				break;
			}

			prev = newIndex;

			newIndex = newEntry->status_next & HASH_NEXT_MASK;
			newEntry = &scrStringGlob.hashTable[newIndex];
		}
	}

	newNext = scrStringGlob.hashTable[0].status_next;

	newEntry->status_next = newNext;
	newEntry->prev = 0;

	scrStringGlob.hashTable[newNext].prev = newIndex;
	scrStringGlob.hashTable[0].status_next = newIndex;
}

/*
==============
SL_RemoveRefToString
==============
*/
void SL_RemoveRefToString( unsigned int stringValue )
{
	SL_RemoveRefToStringOfLen( stringValue, SL_GetRefStringLen( GetRefString( stringValue ) ) + 1 );
}

/*
==============
SL_RemoveAllRefToString
==============
*/
void SL_RemoveAllRefToString( unsigned int stringValue )
{
	RefString *refStr = GetRefString( stringValue );

	if ( refStr->user & 4 )
	{
		refStr->refCount = 1;
		refStr->user = 4;
		return;
	}

	refStr->refCount = 0;
	refStr->user = 0;

	SL_FreeString( stringValue, refStr, SL_GetRefStringLen( refStr ) + 1 );
}

/*
==============
SL_RemoveRefToStringOfLen
==============
*/
void SL_RemoveRefToStringOfLen( unsigned int stringValue, unsigned int len )
{
	RefString *refStr = GetRefString(stringValue);

	refStr->refCount--;

	if ( refStr->refCount )
	{
		return;
	}

	SL_FreeString(stringValue, refStr, len);
}

/*
==============
Scr_SetString
==============
*/
void Scr_SetString( unsigned short *to, unsigned int from )
{
	if ( from )
	{
		SL_AddRefToString(from);
	}

	if ( *to )
	{
		SL_RemoveRefToString(*to);
	}

	*to = from;
}

/*
==============
Scr_SetStringFromCharString
==============
*/
void Scr_SetStringFromCharString( unsigned short *to, const char *from )
{
	if ( *to )
	{
		SL_RemoveRefToString(*to);
	}

	*to = SL_GetString(from, 0);
}

/*
==============
Scr_AllocString
==============
*/
unsigned int Scr_AllocString( const char *s, int user )
{
	return SL_GetString( s, 1 );
}

/*
==============
SL_GetStringForFloat
==============
*/
unsigned int SL_GetStringForFloat( float f )
{
	char tempString[128];

	sprintf( tempString, "%g", f );
	return SL_GetString_( tempString, 0, 14 );
}

/*
==============
SL_GetStringForInt
==============
*/
unsigned int SL_GetStringForInt( int i )
{
	char tempString[128];

	sprintf( tempString, "%i", i );
	return SL_GetString_( tempString, 0, 14 );
}

/*
==============
SL_GetStringForVector
==============
*/
unsigned int SL_GetStringForVector( const vec3_t v )
{
	char tempString[128];

	sprintf( tempString, "(%g, %g, %g)", v[0], v[1], v[2] );
	return SL_GetString_( tempString, 0, 14 );
}

/*
==============
SL_ShutdownSystem
==============
*/
void SL_ShutdownSystem( unsigned int user )
{
	unsigned int hash;
	HashEntry *entry;
	RefString *refStr;

	for ( hash = 1; hash < HASH_TABLE_SIZE; hash++ )
	{
		do
		{
			entry = &scrStringGlob.hashTable[hash];

			if ( !( entry->status_next & HASH_STAT_MASK ) )
			{
				break;
			}

			refStr = GetRefString(entry->str);

			if ( !( user & refStr->user ) )
			{
				break;
			}

			refStr->user &= ~user;
			scrStringGlob.nextFreeEntry = NULL;
			SL_RemoveRefToString(entry->str);
		}
		while ( scrStringGlob.nextFreeEntry );
	}
}

/*
==============
Scr_ShutdownGameStrings
==============
*/
void Scr_ShutdownGameStrings()
{
	SL_ShutdownSystem(1);
}

/*
==============
SL_TransferSystem
==============
*/
void SL_TransferSystem( unsigned int from, unsigned int to )
{
	unsigned int hash;
	HashEntry *entry;
	RefString *refStr;

	for ( hash = 1; hash < HASH_TABLE_SIZE; hash++ )
	{
		entry = &scrStringGlob.hashTable[hash];

		if ( !( entry->status_next & HASH_STAT_MASK ) )
		{
			continue;
		}

		refStr = GetRefString(entry->str);

		if ( !( refStr->user & from ) )
		{
			continue;
		}

		refStr->user &= ~from;
		refStr->user |= to;
	}
}

/*
==============
SL_Clear
==============
*/
void SL_Clear()
{
	unsigned int hash;
	HashEntry *entry;
	byte *allocBits;
	RefString *refStr;
	const char *str;
	int len;

	for ( hash = 1; hash < HASH_TABLE_SIZE; hash++ )
	{
		do
		{
			entry = &scrStringGlob.hashTable[hash];

			if ( !( entry->status_next & HASH_STAT_MASK ) )
			{
				break;
			}

			scrStringGlob.nextFreeEntry = NULL;
			SL_RemoveAllRefToString(entry->str);
		}
		while ( scrStringGlob.nextFreeEntry );
	}

	allocBits = MT_InitForceAlloc();

	for ( hash = 1; hash < HASH_TABLE_SIZE; hash++ )
	{
		entry = &scrStringGlob.hashTable[hash];

		if ( !( entry->status_next & HASH_STAT_MASK ) )
		{
			continue;
		}

		refStr = GetRefString(entry->str);

		if ( !( refStr->user & 4 ) )
		{
			continue;
		}

		str = SL_ConvertToString(entry->str);
		len = I_strlen(str) + 1;
		MT_ForceAllocIndex(allocBits, entry->str, len + REFSTRING_STRING_OFFSET);
	}

	MT_FinishForceAlloc(allocBits);
}

/*
==============
SL_CreateCanonicalFilename
==============
*/
void SL_CreateCanonicalFilename( char *newFilename, const char *filename, int count )
{
	unsigned int c;

	do
	{
		do
		{
			do
			{
				c = *filename;
				*filename++;
			}
			while ( c == '\\' );
		}
		while ( c == '/' );
		while ( c >= ' ' )
		{
			*newFilename = tolower(c);
			*newFilename++;

			--count;

			if ( !count )
			{
				Com_Error(ERR_DROP, "\x15" "Filename '%s' exceeds maximum length of %d", filename, count);
			}

			if ( c == '/' )
			{
				break;
			}

			c = *filename;
			*filename++;

			if ( c == '\\' )
			{
				c = '/';
			}
		}
	}
	while ( c );

	*newFilename = 0;
}

/*
==============
Scr_CreateCanonicalFilename
==============
*/
unsigned int Scr_CreateCanonicalFilename( const char *filename )
{
	char newFilename[MAX_STRING_CHARS];

	SL_CreateCanonicalFilename(newFilename, filename, sizeof(newFilename));
	return SL_GetString_(newFilename, 0, 7);
}

/*
==============
I_strlen
==============
*/
int I_strlen( const char *s )
{
	return strlen(s);
}


// Unreferenced storage; original declarations unknown (sized from the layout).
static int unusedStorage;
