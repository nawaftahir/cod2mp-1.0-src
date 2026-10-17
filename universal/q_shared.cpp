#include "../qcommon/qcommon.h"

const vec4_t colorBlack    = { 0.0f, 0.0f, 0.0f, 1.0f };
const vec4_t colorRed      = { 1.0f, 0.0f, 0.0f, 1.0f };
const vec4_t colorGreen    = { 0.0f, 1.0f, 0.0f, 1.0f };
static const vec4_t colorLtGreen  = { 0.0f, 0.7f, 0.0f, 1.0f };
const vec4_t colorBlue     = { 0.0f, 0.0f, 1.0f, 1.0f };
static const vec4_t colorLtBlue   = { 0.0f, 0.0f, 0.75f, 1.0f };
const vec4_t colorYellow   = { 1.0f, 1.0f, 0.0f, 1.0f };
static const vec4_t colorLtYellow = { 0.75f, 0.75f, 0.0f, 1.0f };
static const vec4_t colorMdYellow = { 0.5f, 0.5f, 0.0f, 1.0f };
static const vec4_t colorMagenta  = { 1.0f, 0.0f, 1.0f, 1.0f };
static const vec4_t colorCyan     = { 0.0f, 1.0f, 1.0f, 1.0f };
static const vec4_t colorLtCyan   = { 0.0f, 0.75f, 0.75f, 1.0f };
static const vec4_t colorMdCyan   = { 0.0f, 0.5f, 0.5f, 1.0f };
static const vec4_t colorDkCyan   = { 0.0f, 0.25f, 0.25f, 1.0f };
const vec4_t colorWhite    = { 1.0f, 1.0f, 1.0f, 1.0f };
const vec4_t colorLtGrey   = { 0.75f, 0.75f, 0.75f, 1.0f };
static const vec4_t colorMdGrey   = { 0.5f, 0.5f, 0.5f, 1.0f };
static const vec4_t colorDkGrey   = { 0.25f, 0.25f, 0.25f, 1.0f };
static const vec4_t colorOrange   = { 1.0f, 0.7f, 0.0f, 1.0f };
static const vec4_t colorLtOrange = { 0.75f, 0.525f, 0.0f, 1.0f };

short ShortSwap( short l );
short ShortNoSwap( short l );
int LongSwap( int l );
int LongNoSwap( int l );
int64_t Long64Swap( int64_t l );
int64_t Long64NoSwap( int64_t l );
float FloatSwap( float f );
float FloatNoSwap( float f );
int FloatSwapAsInt( float f );
int FloatNoSwapAsInt( float f );
void Swap_Init();
void I_strncpyz( char *dest, const char *src, int destsize );
int Com_sprintf( char *dest, size_t size, const char *format, ... );
char *va( const char *format, ... );
const char *Info_ValueForKey( const char *s, const char *key );

// byte order handlers, bound once by Swap_Init
static short ( *_BigShort )( short l );
static short ( *_LittleShort )( short l );
static int ( *_BigLong )( int l );
static int ( *_LittleLong )( int l );
static int64_t ( *_LittleLong64 )( int64_t l );
static float ( *_LittleFloat )( float l );
static int ( *_LittleFloatAsInt )( float l );

static va_info_t va_info[NUMTHREADS];
static jmp_buf g_com_error[NUMTHREADS];
TraceThreadInfo g_traceThreadInfo[NUMTHREADS];

// two buffers so compares work without stomping on each other
static char infoValue[2][BIG_INFO_VALUE];

struct orientation_t
{
	vec3_t origin;
	vec3_t axis[3];
};










unsigned char ColorIndex( unsigned char c )
{
	unsigned char index;

	index = c - '0';

	return index < 10 ? index : 7;
}

const char *Com_GetFilenameSubString( const char *pathname )
{
	const char *last;

	last = pathname;

	while ( *pathname )
	{
		if ( *pathname == '/' )
		{
			last = pathname + 1;
		}

		pathname++;
	}

	return last;
}

void Com_AssembleFilepath( const char *folder, const char *name, const char *extension, char *path, int maxLen )
{
	int folderLen;
	int nameLen;
	int extensionLen;

	folderLen = I_strlen( folder );
	nameLen = I_strlen( name );
	extensionLen = I_strlen( extension );

	if ( folderLen + nameLen + extensionLen >= maxLen )
	{
		Com_Error( ERR_DROP, "filepath '%s%s%s' is longer than %i characters", folder, name, extension, maxLen - 1 );
	}

	memcpy( path, folder, folderLen );
	path += folderLen;
	memcpy( path, name, nameLen );
	path += nameLen;
	memcpy( path, extension, extensionLen + 1 );
}

const char *Com_GetExtensionSubString( const char *filename )
{
	const char *substr;

	substr = NULL;

	while ( *filename )
	{
		if ( *filename == '.' )
		{
			substr = filename;
		}
		else if ( *filename == '/' || *filename == '\\' )
		{
			substr = NULL;
		}

		filename++;
	}

	if ( !substr )
	{
		substr = filename;
	}

	return substr;
}

void Com_StripExtension( const char *in, char *out )
{
	const char *extension;

	extension = Com_GetExtensionSubString( in );

	while ( in != extension )
	{
		*out++ = *in++;
	}

	*out = 0;
}

void Com_StripFilename( const char *in, char *out )
{
	const char *filename;
	int len;

	filename = Com_GetFilenameSubString( in );
	len = filename - in;
	memcpy( out, in, len );
	out[len] = 0;
}

void Com_DefaultExtension( char *path, int maxSize, const char *extension )
{
	char oldPath[MAX_QPATH];
	char *src;

	// if path doesn't have a .EXT, append extension
	// (extension should include the .)
	src = path + strlen( path ) - 1;

	while ( *src != '/' && src != path )
	{
		if ( *src == '.' )
		{
			return; // it has an extension
		}

		src--;
	}

	I_strncpyz( oldPath, path, sizeof( oldPath ) );
	Com_sprintf( path, maxSize, "%s%s", oldPath, extension );
}

short BigShort( short l )
{
	return _BigShort( l );
}

int BigLong( int l )
{
	return _BigLong( l );
}

int64_t LittleLong64( int64_t l )
{
	return _LittleLong64( l );
}

// the header's inline LittleShort/LittleLong serve the engine; these go through the table
short LittleShortRuntime( short l )
{
	return _LittleShort( l );
}

int LittleLongRuntime( int l )
{
	return _LittleLong( l );
}

int LittleFloatAsInt( float l )
{
	return _LittleFloatAsInt( l );
}

short ShortSwap( short l )
{
	byte b1, b2;

	b1 = l & 255;
	b2 = ( l >> 8 ) & 255;

	return ( b1 << 8 ) + b2;
}

short ShortNoSwap( short l )
{
	return l;
}

int LongSwap( int l )
{
	byte b1, b2, b3, b4;

	b1 = l & 255;
	b2 = ( l >> 8 ) & 255;
	b3 = ( l >> 16 ) & 255;
	b4 = ( l >> 24 ) & 255;

	return ( (int)b1 << 24 ) + ( (int)b2 << 16 ) + ( (int)b3 << 8 ) + b4;
}

int64_t Long64Swap( int64_t l )
{
	byte b1, b2, b3, b4, b5, b6, b7, b8;

	b1 = l;
	b2 = (uint64_t)l >> 8;
	b3 = (uint64_t)l >> 16;
	b4 = (uint64_t)l >> 24;
	b5 = (uint64_t)l >> 32;
	b6 = (uint64_t)l >> 40;
	b7 = (uint64_t)l >> 48;
	b8 = (uint64_t)l >> 56;

	return ( (uint64_t)b1 << 56 ) + ( (uint64_t)b2 << 48 ) + ( (uint64_t)b3 << 40 ) + ( (uint64_t)b4 << 32 )
	     + ( (uint64_t)b5 << 24 ) + ( (uint64_t)b6 << 16 ) + ( (uint64_t)b7 << 8 ) + b8;
}

int64_t Long64NoSwap( int64_t l )
{
	return l;
}

int FloatSwapAsInt( float f )
{
	union
	{
		float f;
		int i;
		byte b[4];
	} dat1, dat2;

	dat1.f = f;
	dat2.b[0] = dat1.b[3];
	dat2.b[1] = dat1.b[2];
	dat2.b[2] = dat1.b[1];
	dat2.b[3] = dat1.b[0];

	return dat2.i;
}

void Swap_InitLittleEndian()
{
	_BigShort = ShortSwap;
	_LittleShort = ShortNoSwap;
	_BigLong = LongSwap;
	_LittleLong = LongNoSwap;
	_LittleLong64 = Long64NoSwap;
	_LittleFloat = FloatNoSwap;
	_LittleFloatAsInt = FloatNoSwapAsInt;
}

void Swap_InitBigEndian()
{
	_BigShort = ShortNoSwap;
	_LittleShort = ShortSwap;
	_BigLong = LongNoSwap;
	_LittleLong = LongSwap;
	_LittleLong64 = Long64Swap;
	_LittleFloat = FloatSwap;
	_LittleFloatAsInt = FloatSwapAsInt;
}

void Swap_Init()
{
	byte swaptest[2] = { 1, 0 };

	if ( *(short *)swaptest == 1 )
	{
		Swap_InitLittleEndian();
	}
	else
	{
		Swap_InitBigEndian();
	}
}

bool I_isprint( int c )
{
	return c >= ' ' && c <= '~';
}

bool I_islower( int c )
{
	return c >= 'a' && c <= 'z';
}

bool I_isalpha( int c )
{
	return ( c >= 'a' && c <= 'z' ) || ( c >= 'A' && c <= 'Z' );
}

bool I_isdigit( int c )
{
	return c >= '0' && c <= '9';
}

bool I_isalnum( int c )
{
	return I_isalpha( c ) || I_isdigit( c );
}

bool I_isforfilename( int c )
{
	return I_isalnum( c ) || c == '_' || c == '-';
}

void I_strncpyz( char *dest, const char *src, int destsize )
{
	strncpy( dest, src, destsize - 1 );
	dest[destsize - 1] = 0;
}

int I_strnicmp( const char *s0, const char *s1, int n )
{
	int c0;
	int c1;

	do
	{
		c0 = *s0++;
		c1 = *s1++;

		if ( !n-- )
		{
			return 0; // strings are equal until end point
		}

		if ( c0 != c1 )
		{
			if ( I_islower( c0 ) )
			{
				c0 -= 'a' - 'A';
			}

			if ( I_islower( c1 ) )
			{
				c1 -= 'a' - 'A';
			}

			if ( c0 != c1 )
			{
				return c0 < c1 ? -1 : 1;
			}
		}
	}
	while ( c0 );

	return 0; // strings are equal
}

int I_strncmp( const char *s0, const char *s1, int n )
{
	int c0;
	int c1;

	do
	{
		c0 = *s0++;
		c1 = *s1++;

		if ( !n-- )
		{
			return 0; // strings are equal until end point
		}

		if ( c0 != c1 )
		{
			return c0 < c1 ? -1 : 1;
		}
	}
	while ( c0 );

	return 0; // strings are equal
}

int I_stricmp( const char *s0, const char *s1 )
{
	return I_strnicmp( s0, s1, 0x7FFFFFFF );
}

int I_strcmp( const char *s0, const char *s1 )
{
	return I_strncmp( s0, s1, 0x7FFFFFFF );
}

// '*' matches any run of characters, '?' any single character
int I_stricmpwild( const char *wild, const char *s )
{
	char charWild;
	char charRef;
	int delta;

	do
	{
		charWild = *wild;
		wild++;

		if ( charWild == '*' )
		{
			if ( !*wild )
			{
				return 0;
			}

			if ( *s && !I_stricmpwild( wild - 1, s + 1 ) )
			{
				return 0;
			}
		}
		else
		{
			charRef = *s;
			s++;

			if ( charWild != charRef && charWild != '?' )
			{
				delta = tolower( charWild ) - tolower( charRef );

				if ( delta )
				{
					return delta < 0 ? -1 : 1;
				}
			}
		}
	}
	while ( charWild );

	return 0;
}

char *I_strupr( char *s )
{
	char *iter;

	for ( iter = s; *iter; iter++ )
	{
		if ( I_islower( *iter ) )
		{
			*iter -= 'a' - 'A';
		}
	}

	return s;
}

void I_strncat( char *dest, int size, const char *src )
{
	int l1;

	l1 = I_strlen( dest );

	if ( l1 >= size )
	{
		Com_Error( ERR_FATAL, "\x15" "I_strncat: already overflowed" );
	}

	I_strncpyz( dest + l1, src, size - l1 );
}

// length without ^N color codes
int I_DrawStrlen( const char *str )
{
	const char *s;
	int count;

	s = str;
	count = 0;

	while ( *s )
	{
		if ( s && *s == '^' && s[1] && s[1] != '^' && s[1] >= '0' && s[1] <= '9' )
		{
			s += 2;
		}
		else
		{
			count++;
			s++;
		}
	}

	return count;
}

char *I_CleanStr( char *string )
{
	char *d;
	char *s;
	char c;

	s = string;
	d = string;

	while ( ( c = *s ) != 0 )
	{
		if ( s && *s == '^' && s[1] && s[1] != '^' && s[1] >= '0' && s[1] <= '9' )
		{
			s++;
		}
		else if ( c >= 0x20 && c <= 0x7E )
		{
			*d++ = c;
		}

		s++;
	}

	*d = 0;

	return string;
}

// maps the typographic right quote to an apostrophe
char I_CleanChar( char character )
{
	const int rightQuote = 146;

	if ( (unsigned char)character == rightQuote )
	{
		return '\'';
	}

	return character;
}

int Com_sprintf( char *dest, size_t size, const char *format, ... )
{
	int result;
	va_list argptr;

	va_start( argptr, format );
	result = vsnprintf( dest, size, format, argptr );
	va_end( argptr );

	dest[size - 1] = 0;

	return result;
}

// false when the string lives on the current stack or in this thread's va() buffer
int CanKeepStringPointer( const char *string )
{
	int stackLocal;
	char *vaBuffer;

	if ( string >= (char *)&stackLocal && string < (char *)&stackLocal + 0x2000 )
	{
		return false;
	}

	vaBuffer = (char *)Sys_GetValue( THREAD_VALUE_VA );

	if ( string >= vaBuffer && string < vaBuffer + 0xC00 )
	{
		return false;
	}

	return true;
}

char *va( const char *format, ... )
{
	va_list argptr;
	char *buf;
	int len;
	va_info_t *info;

	va_start( argptr, format );
	info = (va_info_t *)Sys_GetValue( THREAD_VALUE_VA );
	buf = info->va_string[info->index];
	info->index = ( info->index + 1 ) % MAX_VASTRINGS;
	len = vsnprintf( buf, sizeof( info->va_string[0] ), format, argptr );
	va_end( argptr );

	buf[MAX_STRING_CHARS - 1] = 0;

	if ( len < 0 || len >= MAX_STRING_CHARS )
	{
		Com_Error( ERR_DROP, "\x15" "Attempted to overrun string in call to va()" );
	}

	return buf;
}

void Com_InitThreadData( int threadContext )
{
	Sys_SetValue( THREAD_VALUE_VA, &va_info[threadContext] );
	Sys_SetValue( THREAD_VALUE_COM_ERROR, &g_com_error[threadContext] );
	Sys_SetValue( THREAD_VALUE_TRACE, &g_traceThreadInfo[threadContext] );
}

// narrows a little-endian UTF-16 string, keeping the high byte of each unit
void Com_WideToNarrow( const char *in, char *out, int len )
{
	int i;

	for ( i = 0; i < len; i++ )
	{
		out[i] = in[2 * i + 1];
	}

	out[len - 1] = 0;
}

// searches a string of the form \key\value\key\value
const char *Info_ValueForKey( const char *s, const char *key )
{
	char pkey[BIG_INFO_KEY];
	static int valueindex = 0;
	char *dst;
	char *o;

	if ( !s || !key )
	{
		return "";
	}

	if ( strlen( s ) >= BIG_INFO_STRING )
	{
		Com_Error( ERR_DROP, "\x15" "Info_ValueForKey: oversize infostring" );
	}

	valueindex ^= 1;

	if ( *s == '\\' )
	{
		s++;
	}

nextPair:
	o = pkey;

	while ( *s != '\\' )
	{
		if ( !*s )
		{
			return "";
		}

		*o++ = *s++;
	}

	*o = 0;
	s++;

	dst = infoValue[valueindex];
	o = dst;

	while ( *s != '\\' && *s )
	{
		*o++ = *s++;
	}

	*o = 0;

	if ( !I_stricmp( key, pkey ) )
	{
		return dst;
	}

	if ( !*s )
	{
		goto notFound;
	}

	s++;
	goto nextPair;

notFound:
	return "";
}

// used to iterate through all the key/value pairs in an info string
void Info_NextPair( const char **head, char *key, char *value )
{
	char *o;
	const char *s;

	s = *head;

	if ( *s == '\\' )
	{
		s++;
	}

	key[0] = 0;
	value[0] = 0;

	o = key;

	while ( *s != '\\' )
	{
		if ( !*s )
		{
			*o = 0;
			*head = s;
			return;
		}

		*o++ = *s++;
	}

	*o = 0;
	s++;

	o = value;

	while ( *s != '\\' && *s )
	{
		*o++ = *s++;
	}

	*o = 0;

	*head = s;
}

void Info_RemoveKey( char *s, const char *key )
{
	char *start;
	char pkey[MAX_INFO_KEY];
	char value[MAX_INFO_VALUE];
	char *o;

	if ( strlen( s ) >= MAX_INFO_STRING )
	{
		Com_Error( ERR_DROP, "\x15" "Info_RemoveKey: oversize infostring" );
	}

	if ( strchr( key, '\\' ) )
	{
		return;
	}

nextPair:
	start = s;

	if ( *s == '\\' )
	{
		s++;
	}

	o = pkey;

	while ( *s != '\\' )
	{
		if ( !*s )
		{
			return;
		}

		*o++ = *s++;
	}

	*o = 0;
	s++;

	o = value;

	while ( *s != '\\' && *s )
	{
		if ( !*s )
		{
			return;
		}

		*o++ = *s++;
	}

	*o = 0;

	if ( !strcmp( key, pkey ) )
	{
		strcpy( start, s ); // remove this part
		return;
	}

	if ( !*s )
	{
		return;
	}

	goto nextPair;
}

void Info_RemoveKey_Big( char *s, const char *key )
{
	char *start;
	char pkey[BIG_INFO_KEY];
	char value[BIG_INFO_VALUE];
	char *o;

	if ( strlen( s ) >= BIG_INFO_STRING )
	{
		Com_Error( ERR_DROP, "\x15" "Info_RemoveKey_Big: oversize infostring" );
	}

	if ( strchr( key, '\\' ) )
	{
		return;
	}

nextPair:
	start = s;

	if ( *s == '\\' )
	{
		s++;
	}

	o = pkey;

	while ( *s != '\\' )
	{
		if ( !*s )
		{
			return;
		}

		*o++ = *s++;
	}

	*o = 0;
	s++;

	o = value;

	while ( *s != '\\' && *s )
	{
		if ( !*s )
		{
			return;
		}

		*o++ = *s++;
	}

	*o = 0;

	if ( !strcmp( key, pkey ) )
	{
		strcpy( start, s ); // remove this part
		return;
	}

	if ( !*s )
	{
		return;
	}

	goto nextPair;
}

// some characters are illegal in info strings because they can mess up the server's parsing
qboolean Info_Validate( const char *s )
{
	if ( strchr( s, '\"' ) )
	{
		return qfalse;
	}

	if ( strchr( s, ';' ) )
	{
		return qfalse;
	}

	return qtrue;
}

// changes or adds a key/value pair
void Info_SetValueForKey( char *s, const char *key, const char *value )
{
	char newi[MAX_INFO_STRING];
	char cleanValue[MAX_INFO_STRING];
	int i;
	int j;
	int len;
	char c;

	if ( strlen( s ) >= MAX_INFO_STRING )
	{
		Com_Printf( "\x15" "Info_SetValueForKey: oversize infostring" );
		return;
	}

	j = 0;

	for ( i = 0; i < MAX_INFO_STRING - 1; i++ )
	{
		c = value[i];

		if ( !c )
		{
			break;
		}

		if ( c != '\\' && c != ';' && c != '\"' )
		{
			cleanValue[j] = c;
			j++;
		}
	}

	cleanValue[j] = 0;

	if ( strchr( key, '\\' ) )
	{
		Com_Printf( "\x15" "Can't use keys with a \\\nkey: '%s'\nvalue: '%s'", key, value );
		return;
	}

	if ( strchr( key, ';' ) )
	{
		Com_Printf( "\x15" "Can't use keys with a semicolon\nkey: '%s'\nvalue: '%s'", key, value );
		return;
	}

	if ( strchr( key, '\"' ) )
	{
		Com_Printf( "\x15" "Can't use keys with a \"\nkey: '%s'\nvalue: '%s'", key, value );
		return;
	}

	Info_RemoveKey( s, key );

	if ( !cleanValue[0] )
	{
		return;
	}

	len = Com_sprintf( newi, sizeof( newi ), "\\%s\\%s", key, cleanValue );

	if ( len <= 0 )
	{
		Com_Printf( "\x15" "Info buffer length exceeded, not including key/value pair in response\n" );
		return;
	}

	if ( strlen( newi ) + strlen( s ) > MAX_INFO_STRING )
	{
		Com_Printf( "\x15" "Info string length exceeded\nkey: '%s'\nvalue: '%s'\nInfo string:\n%s\n", key, value, s );
		return;
	}

	strcat( s, newi );
}

void Info_SetValueForKey_Big( char *s, const char *key, const char *value )
{
	char newi[BIG_INFO_STRING];
	char cleanValue[BIG_INFO_STRING];
	int i;
	int j;
	int len;
	char c;

	if ( strlen( s ) >= BIG_INFO_STRING )
	{
		Com_Printf( "\x15" "Info_SetValueForKey: oversize infostring" );
		return;
	}

	j = 0;

	for ( i = 0; i < BIG_INFO_STRING - 1; i++ )
	{
		c = value[i];

		if ( !c )
		{
			break;
		}

		if ( c != '\\' && c != ';' && c != '\"' )
		{
			cleanValue[j] = c;
			j++;
		}
	}

	cleanValue[j] = 0;

	if ( strchr( key, '\\' ) )
	{
		Com_Printf( "\x15" "Can't use keys with a \\\nkey: '%s'\nvalue: '%s'", key, value );
		return;
	}

	if ( strchr( key, ';' ) )
	{
		Com_Printf( "\x15" "Can't use keys with a semicolon\nkey: '%s'\nvalue: '%s'", key, value );
		return;
	}

	if ( strchr( key, '\"' ) )
	{
		Com_Printf( "\x15" "Can't use keys with a \"\nkey: '%s'\nvalue: '%s'", key, value );
		return;
	}

	Info_RemoveKey_Big( s, key );

	if ( !cleanValue[0] )
	{
		return;
	}

	len = Com_sprintf( newi, sizeof( newi ), "\\%s\\%s", key, cleanValue );

	if ( len <= 0 )
	{
		Com_Printf( "\x15" "Info buffer length exceeded, not including key/value pair in response\n" );
		return;
	}

	if ( strlen( newi ) + strlen( s ) > MAX_INFO_STRING )
	{
		Com_Printf( "\x15" "Info string length exceeded\nkey: '%s'\nvalue: '%s'\nInfo string:\n%s\n", key, value, s );
		return;
	}

	strcat( s, newi );
}

qboolean ParseConfigStringToStruct( unsigned char *pStruct, const cspField_t *pFieldList, int iNumFields, const char *pszBuffer, int iMaxFieldTypes,
                                    int ( *parseSpecialFieldType )( unsigned char *, const char *, const int ), void ( *parseStrCpy )( unsigned char *, const char * ) )
{
	int iField;
	const char *pszKeyValue;
	const cspField_t *pField;
	for ( iField = 0, pField = pFieldList; iField < iNumFields; iField++, pField++ )
	{
		pszKeyValue = Info_ValueForKey( pszBuffer, pField->szName );
		if ( !*pszKeyValue )
		{
			continue;
		}
		if ( pField->iFieldType < CSPFT_NUM_BASE_FIELD_TYPES )
		{
			switch ( pField->iFieldType )
			{
			case CSPFT_STRING:
				parseStrCpy( &pStruct[pField->iOffset], pszKeyValue );
				break;
			case CSPFT_STRING_MAX_STRING_CHARS:
				I_strncpyz( (char *)&pStruct[pField->iOffset], pszKeyValue, MAX_STRING_CHARS );
				break;
			case CSPFT_STRING_MAX_QPATH:
				I_strncpyz( (char *)&pStruct[pField->iOffset], pszKeyValue, MAX_QPATH );
				break;
			case CSPFT_STRING_MAX_OSPATH:
				I_strncpyz( (char *)&pStruct[pField->iOffset], pszKeyValue, MAX_OSPATH );
				break;
			case CSPFT_INT:
				*(int *)&pStruct[pField->iOffset] = atoi( pszKeyValue );
				break;
			case CSPFT_QBOOLEAN:
				*(qboolean *)&pStruct[pField->iOffset] = atoi( pszKeyValue ) != 0;
				break;
			case CSPFT_FLOAT:
				*(float *)&pStruct[pField->iOffset] = atof( pszKeyValue );
				break;
			case CSPFT_MILLISECONDS:
				*(int *)&pStruct[pField->iOffset] = (float)atof( pszKeyValue ) * 1000.0f;
				break;
			default:
				// a check compiled out of this build; its empty test still shapes the code
				if ( pszKeyValue )
				{
				}
				break;
			}
		}
		else if ( iMaxFieldTypes > 0 && pField->iFieldType < iMaxFieldTypes )
		{
			if ( !parseSpecialFieldType( pStruct, pszKeyValue, pField->iFieldType ) )
			{
				return qfalse;
			}
		}
		else
		{
			Com_Error( ERR_DROP, "\x15" "Bad field type %i\n", pField->iFieldType );
		}
	}
	if ( iField != iNumFields )
	{
		return qfalse;
	}
	return qtrue;
}

float GetLeanFraction( const float fFrac )
{
	return ( 2.0f - I_fabs( fFrac ) ) * fFrac;
}

float UnGetLeanFraction( const float fFrac )
{
	return 1.0f - I_sqrt( 1.0f - fFrac );
}

void AddLeanToPosition( float *position, float fViewYaw, float fLeanFrac, float fViewRoll, float fLeanDist )
{
	float fLean;
	vec3_t vRight;
	vec3_t vAng;

	if ( fLeanFrac != 0.0f )
	{
		fLean = GetLeanFraction( fLeanFrac );
		VectorSet( vAng, 0, fViewYaw, fLean * fViewRoll );
		AngleVectors( vAng, NULL, vRight, NULL );
		fLean = fLean * fLeanDist;
		VectorMA( position, fLean, vRight, position );
	}
}

void OrientationPosToWorld( const orientation_t *orient, const float *pos, float *out )
{
	out[0] = orient->origin[0] + pos[0] * orient->axis[0][0] + pos[1] * orient->axis[1][0] + pos[2] * orient->axis[2][0];
	out[1] = orient->origin[1] + pos[0] * orient->axis[0][1] + pos[1] * orient->axis[1][1] + pos[2] * orient->axis[2][1];
	out[2] = orient->origin[2] + pos[0] * orient->axis[0][2] + pos[1] * orient->axis[1][2] + pos[2] * orient->axis[2][2];
}

void OrientationDirToWorld( const orientation_t *orient, const float *dir, float *out )
{
	out[0] = dir[0] * orient->axis[0][0] + dir[1] * orient->axis[1][0] + dir[2] * orient->axis[2][0];
	out[1] = dir[0] * orient->axis[0][1] + dir[1] * orient->axis[1][1] + dir[2] * orient->axis[2][1];
	out[2] = dir[0] * orient->axis[0][2] + dir[1] * orient->axis[1][2] + dir[2] * orient->axis[2][2];
}

void OrientationPlaneToWorld( const orientation_t *orient, const float *plane, float *out )
{
	OrientationDirToWorld( orient, plane, out );
	out[3] = plane[3] + DotProduct( orient->origin, out );
}

void OrientationPosToLocal( const orientation_t *orient, const float *pos, float *out )
{
	vec3_t delta;

	delta[0] = pos[0] - orient->origin[0];
	delta[1] = pos[1] - orient->origin[1];
	delta[2] = pos[2] - orient->origin[2];

	out[0] = delta[0] * orient->axis[0][0] + delta[1] * orient->axis[0][1] + delta[2] * orient->axis[0][2];
	out[1] = delta[0] * orient->axis[1][0] + delta[1] * orient->axis[1][1] + delta[2] * orient->axis[1][2];
	out[2] = delta[0] * orient->axis[2][0] + delta[1] * orient->axis[2][1] + delta[2] * orient->axis[2][2];
}

void OrientationDirToLocal( const orientation_t *orient, const float *dir, float *out )
{
	out[0] = dir[0] * orient->axis[0][0] + dir[1] * orient->axis[0][1] + dir[2] * orient->axis[0][2];
	out[1] = dir[0] * orient->axis[1][0] + dir[1] * orient->axis[1][1] + dir[2] * orient->axis[1][2];
	out[2] = dir[0] * orient->axis[2][0] + dir[1] * orient->axis[2][1] + dir[2] * orient->axis[2][2];
}

void OrientationConcatenate( const orientation_t *child, const orientation_t *parent, orientation_t *out )
{
	OrientationDirToWorld( parent, child->axis[0], out->axis[0] );
	OrientationDirToWorld( parent, child->axis[1], out->axis[1] );
	OrientationDirToWorld( parent, child->axis[2], out->axis[2] );
	OrientationPosToWorld( parent, child->origin, out->origin );
}

void OrientationPlaneToLocal( const orientation_t *orient, const float *plane, float *out )
{
	OrientationDirToLocal( orient, plane, out );
	out[3] = plane[3] - DotProduct( orient->origin, plane );
}

void ScaledOrientationPosToWorld( const orientation_t *orient, float scale, const float *pos, float *out )
{
	out[0] = orient->origin[0] + ( pos[0] * orient->axis[0][0] + pos[1] * orient->axis[1][0] + pos[2] * orient->axis[2][0] ) * scale;
	out[1] = orient->origin[1] + ( pos[0] * orient->axis[0][1] + pos[1] * orient->axis[1][1] + pos[2] * orient->axis[2][1] ) * scale;
	out[2] = orient->origin[2] + ( pos[0] * orient->axis[0][2] + pos[1] * orient->axis[1][2] + pos[2] * orient->axis[2][2] ) * scale;
}

void ScaledOrientationPlaneToWorld( const orientation_t *orient, float scale, const float *plane, float *out )
{
	OrientationDirToWorld( orient, plane, out );
	out[3] = plane[3] * scale + DotProduct( orient->origin, out );
}

void ScaledOrientationPosToLocal( const orientation_t *orient, float scale, const float *pos, float *out )
{
	vec3_t delta;
	float invScale;

	delta[0] = pos[0] - orient->origin[0];
	delta[1] = pos[1] - orient->origin[1];
	delta[2] = pos[2] - orient->origin[2];

	invScale = 1.0f / scale;

	out[0] = invScale * ( delta[0] * orient->axis[0][0] + delta[1] * orient->axis[0][1] + delta[2] * orient->axis[0][2] );
	out[1] = invScale * ( delta[0] * orient->axis[1][0] + delta[1] * orient->axis[1][1] + delta[2] * orient->axis[1][2] );
	out[2] = invScale * ( delta[0] * orient->axis[2][0] + delta[1] * orient->axis[2][1] + delta[2] * orient->axis[2][2] );
}

void ScaledOrientationPlaneToLocal( const orientation_t *orient, float scale, const float *plane, float *out )
{
	OrientationDirToLocal( orient, plane, out );
	out[3] = ( plane[3] - DotProduct( orient->origin, plane ) ) / scale;
}

float LittleFloat( float l )
{
	return _LittleFloat( l );
}
int LongNoSwap( int l )
{
	return l;
}
float FloatSwap( float f )
{
	union
	{
		float f;
		byte b[4];
	} dat1, dat2;

	dat1.f = f;
	dat2.b[0] = dat1.b[3];
	dat2.b[1] = dat1.b[2];
	dat2.b[2] = dat1.b[1];
	dat2.b[3] = dat1.b[0];

	return dat2.f;
}
float FloatNoSwap( float f )
{
	return FloatIdentity( f );
}
int FloatNoSwapAsInt( float f )
{
	return FloatAsInt( f );
}
bool I_isupper( int c )
{
	return c >= 'A' && c <= 'Z';
}
__forceinline char *I_strlwrInl( char *s )
{
	char *iter;

	for ( iter = s; *iter; iter++ )
	{
		if ( I_isupper( *iter ) )
		{
			*iter += 'a' - 'A';
		}
	}

	return s;
}
__declspec(noinline) char *I_strlwr( char *s )
{
	return I_strlwrInl( s );
}

// Unreferenced storage; original declarations unknown (sized from the layout).
static int unusedStorage[2];
