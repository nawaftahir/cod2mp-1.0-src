#include "../qcommon/qcommon.h"

// multiple character punctuation tokens
static const char *punctuation[] =
{
	"+=", "-=",  "*=",  "/=", "&=", "|=", "++", "--",
	"&&", "||",  "<=",  ">=", "==", "!=",
	NULL
};

static ParseThreadInfo g_parse = { { { "", 1, false, true, false, false, true, false, "", "", 1, NULL } } };

char *Com_Parse( const char **data_p );

ParseThreadInfo *Com_GetParseThreadInfo()
{
	return &g_parse;
}

void Com_InitParseInfo( parseInfo_t *pi )
{
	pi->lines = 1;
	pi->ungetToken = false;
	pi->spaceDelimited = true;
	pi->keepStringQuotes = false;
	pi->csv = false;
	pi->negativeNumbers = false;
	pi->errorPrefix = "";
	pi->warningPrefix = "";
	pi->backup_lines = 0;
	pi->backup_text = NULL;
}

void Com_BeginParseSession( const char *filename )
{
	int i;
	ParseThreadInfo *parse;
	parseInfo_t *pi;

	parse = Com_GetParseThreadInfo();

	if ( parse->parseInfoNum == MAX_PARSE_INFO - 1 )
	{
		Com_Printf( "Already parsing:\n" );

		for ( i = 0; i < parse->parseInfoNum; i++ )
		{
			Com_Printf( "%i. %s\n", i, parse->parseInfo[i].parseFile );
		}

		Com_Error( ERR_FATAL, "\x15" "Com_BeginParseSession: session overflow trying to parse %s\n", filename );
	}

	parse->parseInfoNum++;
	pi = &parse->parseInfo[parse->parseInfoNum];

	Com_InitParseInfo( pi );
	I_strncpyz( pi->parseFile, filename, sizeof( pi->parseFile ) );
}

__forceinline void Com_EndParseSessionInl( void )
{
	ParseThreadInfo *parse;

	parse = Com_GetParseThreadInfo();

	if ( parse->parseInfoNum == 0 )
	{
		Com_Error( ERR_FATAL, "\x15" "Com_EndParseSession: session underflow" );
	}

	parse->parseInfoNum--;
}

// retail calls it from 7 sites only, inlines elsewhere
__declspec(noinline) void Com_EndParseSession( void )
{
	Com_EndParseSessionInl();
}

void Com_ResetParseSessions()
{
	ParseThreadInfo *parse;

	parse = Com_GetParseThreadInfo();
	parse->parseInfoNum = 0;
}

void Com_SetSpaceDelimited( qboolean spaceDelimited )
{
	ParseThreadInfo *parse;
	parseInfo_t *pi;

	parse = Com_GetParseThreadInfo();
	pi = &parse->parseInfo[parse->parseInfoNum];
	pi->spaceDelimited = spaceDelimited != qfalse;
}

void Com_SetKeepStringQuotes( qboolean keepStringQuotes )
{
	ParseThreadInfo *parse;
	parseInfo_t *pi;

	parse = Com_GetParseThreadInfo();
	pi = &parse->parseInfo[parse->parseInfoNum];
	pi->keepStringQuotes = keepStringQuotes != qfalse;
}

void Com_SetCSV( qboolean csv )
{
	ParseThreadInfo *parse;
	parseInfo_t *pi;

	parse = Com_GetParseThreadInfo();
	pi = &parse->parseInfo[parse->parseInfoNum];
	pi->csv = csv != qfalse;
}

void Com_SetParseNegativeNumbers( qboolean negativeNumbers )
{
	ParseThreadInfo *parse;
	parseInfo_t *pi;

	parse = Com_GetParseThreadInfo();
	pi = &parse->parseInfo[parse->parseInfoNum];
	pi->negativeNumbers = negativeNumbers != qfalse;
}

int Com_GetCurrentParseLine( void )
{
	ParseThreadInfo *parse;
	parseInfo_t *pi;

	parse = Com_GetParseThreadInfo();
	pi = &parse->parseInfo[parse->parseInfoNum];
	return pi->lines;
}

void Com_SetScriptErrorPrefix( const char *prefix )
{
	ParseThreadInfo *parse;
	parseInfo_t *pi;

	parse = Com_GetParseThreadInfo();
	pi = &parse->parseInfo[parse->parseInfoNum];
	pi->errorPrefix = prefix;
}

void Com_SetScriptWarningPrefix( const char *prefix )
{
	ParseThreadInfo *parse;
	parseInfo_t *pi;

	parse = Com_GetParseThreadInfo();
	pi = &parse->parseInfo[parse->parseInfoNum];
	pi->warningPrefix = prefix;
}

void Com_ScriptErrorDrop( const char *msg, ... )
{
	va_list va;
	char string[MAXPRINTMSG];
	ParseThreadInfo *parse;
	parseInfo_t *pi;

	parse = Com_GetParseThreadInfo();
	pi = &parse->parseInfo[parse->parseInfoNum];

	va_start( va, msg );
	vsprintf( string, msg, va );

	if ( parse->parseInfoNum )
	{
		Com_Error( ERR_DROP, "\x15%sFile %s, line %i: %s", pi->errorPrefix, pi->parseFile, pi->lines, string );
	}
	else
	{
		Com_Error( ERR_DROP, "\x15%s", string );
	}
}

void Com_ScriptWarning( const char *msg, ... )
{
	va_list va;
	char string[MAXPRINTMSG];
	ParseThreadInfo *parse;
	parseInfo_t *pi;

	parse = Com_GetParseThreadInfo();
	pi = &parse->parseInfo[parse->parseInfoNum];

	va_start( va, msg );
	vsprintf( string, msg, va );

	if ( parse->parseInfoNum )
	{
		Com_Printf( "%sFile %s, line %i: %s", pi->warningPrefix, pi->parseFile, pi->lines, string );
	}
	else
	{
		Com_Printf( "%s", string );
	}
}

/*
Calling this will make the next Com_Parse return
the current token instead of advancing the pointer
*/
void Com_UngetToken()
{
	ParseThreadInfo *parse;
	parseInfo_t *pi;

	parse = Com_GetParseThreadInfo();
	pi = &parse->parseInfo[parse->parseInfoNum];

	if ( pi->ungetToken )
	{
		Com_ScriptErrorDrop( "UngetToken called twice" );
	}

	pi->ungetToken = true;
	parse->tokenPos = parse->prevTokenPos;
}

void Com_ParseSetMark( const char **text, com_parse_mark_t *mark )
{
	ParseThreadInfo *parse;
	parseInfo_t *pi;

	parse = Com_GetParseThreadInfo();
	pi = &parse->parseInfo[parse->parseInfoNum];

	mark->lines = pi->lines;
	mark->text = *text;
	mark->ungetToken = pi->ungetToken;
	mark->backup_lines = pi->backup_lines;
	mark->backup_text = pi->backup_text;
}

void Com_ParseReturnToMark( const char **text, com_parse_mark_t *mark )
{
	ParseThreadInfo *parse;
	parseInfo_t *pi;

	parse = Com_GetParseThreadInfo();
	pi = &parse->parseInfo[parse->parseInfoNum];

	pi->lines = mark->lines;
	*text = mark->text;
	pi->ungetToken = mark->ungetToken != qfalse;
	pi->backup_lines = mark->backup_lines;
	pi->backup_text = mark->backup_text;
}

static const char *SkipWhitespace( const char *data, qboolean *hasNewLines )
{
	int c;
	ParseThreadInfo *parse;
	parseInfo_t *pi;

	parse = Com_GetParseThreadInfo();
	pi = &parse->parseInfo[parse->parseInfoNum];

	while ( ( c = *data ) <= ' ' )
	{
		if ( !c )
		{
			return NULL;
		}

		if ( c == '\n' )
		{
			pi->lines++;
			*hasNewLines = qtrue;
		}

		data++;
	}

	return data;
}

int Com_Compress( char *data_p )
{
	char *datai, *datao;
	char c;
	int size;
	qboolean ws = qfalse;

	size = 0;
	datai = datao = data_p;

	if ( datai )
	{
		while ( ( c = *datai ) != 0 )
		{
			if ( c == 13 || c == 10 )
			{
				*datao = c;
				datao++;
				size++;
				ws = qfalse;
				datai++;
			}
			// skip double slash comments
			else if ( c == '/' && datai[1] == '/' )
			{
				while ( *datai && *datai != '\n' )
				{
					datai++;
				}

				ws = qfalse;
			}
			// skip /* */ comments
			else if ( c == '/' && datai[1] == '*' )
			{
				while ( *datai && ( *datai != '*' || datai[1] != '/' ) )
				{
					if ( *datai == 10 )
					{
						*datao = 10;
						datao++;
						++size;
					}

					datai++;
				}

				if ( *datai )
				{
					datai += 2;
				}

				ws = qfalse;
			}
			else
			{
				if ( ws )
				{
					*datao = ' ';
					datao++;
				}

				*datao = c;
				datao++;
				size++;
				datai++;
				ws = qfalse;
			}
		}
	}

	*datao = 0;
	return size;
}

const char *Com_GetLastTokenPos()
{
	ParseThreadInfo *parse;

	parse = Com_GetParseThreadInfo();
	return parse->tokenPos;
}

static char *Com_ParseCSV( const char **data_p, qboolean allowLineBreaks )
{
	unsigned int len;
	const char *data;
	ParseThreadInfo *parse;
	parseInfo_t *pi;

	parse = Com_GetParseThreadInfo();
	pi = &parse->parseInfo[parse->parseInfoNum];

	data = *data_p;
	len = 0;
	pi->token[0] = 0;

	if ( !allowLineBreaks )
	{
		if ( *data == '\r' || *data == '\n' )
		{
			return pi->token;
		}
	}
	else
	{
		while ( *data == '\r' || *data == '\n' )
		{
			data++;
		}
	}

	parse->prevTokenPos = parse->tokenPos;
	parse->tokenPos = data;

	while ( *data && *data != ',' && *data != '\n' )
	{
		if ( *data == '\r' )
		{
			data++;
			continue;
		}

		if ( *data != '\"' )
		{
			if ( len < MAX_TOKEN_CHARS - 1 )
			{
				pi->token[len] = *data;
				len++;
			}

			data++;
		}
		else
		{
			data++;

			while ( 1 )
			{
				if ( *data == '\"' )
				{
					if ( data[1] == '\"' )
					{
						if ( len < MAX_TOKEN_CHARS - 1 )
						{
							pi->token[len] = '\"';
							len++;
						}

						data += 2;
					}
					else
					{
						data++;
						break;
					}
				}
				else
				{
					if ( len < MAX_TOKEN_CHARS - 1 )
					{
						pi->token[len] = *data;
						len++;
					}

					data++;
				}
			}
		}
	}

	if ( *data )
	{
		if ( *data != '\n' )
		{
			data++;
		}

		*data_p = data;
	}
	else
	{
		*data_p = NULL;
	}

	pi->token[len] = 0;
	return pi->token;
}

/*
Parse a token out of a string
Will never return NULL, just empty strings.
An empty string will only be returned at end of file.

If "allowLineBreaks" is qtrue then an empty
string will be returned if the next token is
a newline.
*/
char *Com_ParseExt( const char **data_p, qboolean allowLineBreaks )
{
	char c = 0;
	int len;
	qboolean hasNewLines = qfalse;
	const char *data;
	const char **punc;
	ParseThreadInfo *parse;
	parseInfo_t *pi;
	int l, j;

	parse = Com_GetParseThreadInfo();
	pi = &parse->parseInfo[parse->parseInfoNum];

	data = *data_p;
	len = 0;
	pi->token[0] = 0;

	// make sure incoming data is valid
	if ( !data )
	{
		*data_p = NULL;
		return pi->token;
	}

	pi->backup_lines = pi->lines;
	pi->backup_text = *data_p;

	if ( pi->csv )
	{
		return Com_ParseCSV( data_p, allowLineBreaks );
	}

	// skip any leading whitespace
restart:
	data = SkipWhitespace( data, &hasNewLines );

	if ( !data )
	{
		*data_p = NULL;
		return pi->token;
	}

	if ( hasNewLines && !allowLineBreaks )
	{
		return pi->token;
	}

	c = *data;

	// skip double slash comments
	if ( c == '/' && data[1] == '/' )
	{
		while ( *data && *data != '\n' )
		{
			data++;
		}

		goto restart;
	}

	// skip /* */ comments
	if ( c == '/' && data[1] == '*' )
	{
		while ( *data && ( *data != '*' || data[1] != '/' ) )
		{
			if ( *data == '\n' )
			{
				pi->lines++;
			}

			data++;
		}

		if ( *data )
		{
			data += 2;
		}

		goto restart;
	}

	// a real token to parse
	parse->prevTokenPos = parse->tokenPos;
	parse->tokenPos = data;

	// handle quoted strings
	if ( c == '\"' )
	{
		if ( pi->keepStringQuotes )
		{
			pi->token[len] = '\"';
			len++;
		}

		data++;

		while ( 1 )
		{
			c = *data++;

			// allow quoted strings to use \" to indicate the " character
			if ( c == '\\' && ( *data == '\"' || *data == '\\' ) )
			{
				c = *data++;
			}
			else if ( c == '\"' || !c )
			{
				if ( pi->keepStringQuotes )
				{
					pi->token[len] = '\"';
					len++;
				}

				pi->token[len] = 0;
				*data_p = data;
				return pi->token;
			}
			else if ( *data == '\n' )
			{
				pi->lines++;
			}

			if ( len < MAX_TOKEN_CHARS - 1 )
			{
				pi->token[len] = c;
				len++;
			}
		}
	}

	if ( pi->spaceDelimited )
	{
		do
		{
			if ( len < MAX_TOKEN_CHARS - 1 )
			{
				pi->token[len] = c;
				len++;
			}

			c = *++data;
		}
		while ( c > ' ' );

		if ( len == MAX_TOKEN_CHARS )
		{
			len = 0;
		}

		pi->token[len] = 0;
		*data_p = data;
		return pi->token;
	}

	// check for a number
	if ( ( c >= '0' && c <= '9' )
	        || ( pi->negativeNumbers && c == '-' && data[1] >= '0' && data[1] <= '9' )
	        || ( c == '.' && data[1] >= '0' && data[1] <= '9' ) )
	{
		do
		{
			if ( len < MAX_TOKEN_CHARS - 1 )
			{
				pi->token[len] = c;
				len++;
			}

			data++;
			c = *data;
		}
		while ( ( c >= '0' && c <= '9' ) || c == '.' );

		// parse the exponent
		if ( c == 'e' || c == 'E' )
		{
			if ( len < MAX_TOKEN_CHARS - 1 )
			{
				pi->token[len] = c;
				len++;
			}

			data++;
			c = *data;

			if ( c == '-' || c == '+' )
			{
				if ( len < MAX_TOKEN_CHARS - 1 )
				{
					pi->token[len] = c;
					len++;
				}

				data++;
				c = *data;
			}

			do
			{
				if ( len < MAX_TOKEN_CHARS - 1 )
				{
					pi->token[len] = c;
					len++;
				}

				data++;
				c = *data;
			}
			while ( c >= '0' && c <= '9' );
		}

		if ( len == MAX_TOKEN_CHARS )
		{
			len = 0;
		}

		pi->token[len] = 0;
		*data_p = data;
		return pi->token;
	}

	// check for a regular word
	// we still allow forward and back slashes in name tokens for pathnames
	if ( ( c >= 'a' && c <= 'z' ) || ( c >= 'A' && c <= 'Z' ) || c == '_' || c == '/' || c == '\\' )
	{
		do
		{
			if ( len < MAX_TOKEN_CHARS - 1 )
			{
				pi->token[len] = c;
				len++;
			}

			data++;
			c = *data;
		}
		while ( ( c >= 'a' && c <= 'z' ) || ( c >= 'A' && c <= 'Z' ) || c == '_' || ( c >= '0' && c <= '9' ) );

		if ( len == MAX_TOKEN_CHARS )
		{
			len = 0;
		}

		pi->token[len] = 0;
		*data_p = data;
		return pi->token;
	}

	// check for multi-character punctuation token
	for ( punc = punctuation; *punc; punc++ )
	{
		l = I_strlen( *punc );

		for ( j = 0; j < l; j++ )
		{
			if ( data[j] != ( *punc )[j] )
			{
				break;
			}
		}

		if ( j == l )
		{
			// a valid multi-character punctuation
			memcpy( pi->token, *punc, l );
			pi->token[l] = 0;
			data += l;
			*data_p = data;
			return pi->token;
		}
	}

	// single character punctuation
	pi->token[0] = *data;
	pi->token[1] = 0;
	data++;
	*data_p = data;
	return pi->token;
}

char *Com_Parse( const char **data_p )
{
	ParseThreadInfo *parse;
	parseInfo_t *pi;

	parse = Com_GetParseThreadInfo();
	pi = &parse->parseInfo[parse->parseInfoNum];

	if ( pi->ungetToken )
	{
		pi->ungetToken = false;
		*data_p = pi->backup_text;
		pi->lines = pi->backup_lines;
	}

	return Com_ParseExt( data_p, qtrue );
}

__forceinline char *Com_ParseOnLineInl( const char **data_p )
{
	ParseThreadInfo *parse;
	parseInfo_t *pi;

	parse = Com_GetParseThreadInfo();
	pi = &parse->parseInfo[parse->parseInfoNum];

	if ( pi->ungetToken )
	{
		pi->ungetToken = false;

		if ( !pi->spaceDelimited )
		{
			return pi->token;
		}

		*data_p = pi->backup_text;
		pi->lines = pi->backup_lines;
	}

	return Com_ParseExt( data_p, qfalse );
}

char *Com_ParseOnLine( const char **data_p )
{
	return Com_ParseOnLineInl( data_p );
}

qboolean Com_MatchToken( const char **buf_p, const char *match, qboolean warning )
{
	const char *token;

	token = Com_Parse( buf_p );

	if ( !strcmp( token, match ) )
	{
		return qtrue;
	}

	if ( warning )
	{
		Com_ScriptWarning( "MatchToken: %s != %s\n", token, match );
	}
	else
	{
		Com_ScriptErrorDrop( "MatchToken: %s != %s\n", token, match );
	}

	return qfalse;
}

/*
The next token should be an open brace.
Skips until a matching close brace is found.
Internal brace depths are properly skipped.
*/
qboolean Com_SkipBracedSection( const char **program, int maxNesting )
{
	const char *token;
	int depth;
	qboolean nestingExceeded;

	nestingExceeded = qfalse;
	depth = 0;

	do
	{
		token = Com_Parse( program );

		if ( token[1] == 0 )
		{
			if ( token[0] == '{' )
			{
				if ( depth == maxNesting )
				{
					nestingExceeded = qtrue;
				}
				else
				{
					depth++;
				}
			}
			else if ( token[0] == '}' )
			{
				depth--;
			}
		}
	}
	while ( depth && *program );

	return nestingExceeded;
}

__forceinline void Com_SkipRestOfLineInl( const char **data )
{
	const char *p;
	int c;
	ParseThreadInfo *parse;
	parseInfo_t *pi;

	parse = Com_GetParseThreadInfo();
	pi = &parse->parseInfo[parse->parseInfoNum];
	p = *data;

	if ( !p )
	{
		return;
	}

	while ( ( c = *p ) != 0 )
	{
		p++;

		if ( c == '\n' )
		{
			pi->lines++;
			break;
		}
	}

	*data = p;
}

void Com_SkipRestOfLine( const char **data )
{
	Com_SkipRestOfLineInl( data );
}

int Com_GetArgCountOnLine( const char **data_p )
{
	const char *token;
	int count;
	com_parse_mark_t mark;

	Com_ParseSetMark( data_p, &mark );

	for ( count = 0; ; count++ )
	{
		token = Com_ParseOnLine( data_p );

		if ( !token[0] )
		{
			break;
		}
	}

	Com_ParseReturnToMark( data_p, &mark );
	return count;
}

const char *Com_ParseRestOfLine( const char **data_p )
{
	const char *token;
	ParseThreadInfo *parse;
	char *line;

	parse = Com_GetParseThreadInfo();
	line = parse->line;
	line[0] = 0;

	while ( 1 )
	{
		token = Com_ParseOnLine( data_p );

		if ( !token[0] )
		{
			break;
		}

		if ( line[0] )
		{
			I_strncat( line, MAX_TOKEN_CHARS, " " );
		}

		I_strncat( line, MAX_TOKEN_CHARS, token );
	}

	return line;
}

float Com_ParseFloat( const char **buf_p )
{
	const char *token;

	token = Com_Parse( buf_p );
	return atof( token );
}

float Com_ParseFloatOnLine( const char **buf_p )
{
	const char *token;

	token = Com_ParseOnLine( buf_p );
	return atof( token );
}

int Com_ParseInt( const char **buf_p )
{
	const char *token;

	token = Com_Parse( buf_p );
	return atoi( token );
}

int Com_ParseIntOnLine( const char **buf_p )
{
	const char *token;

	token = Com_ParseOnLine( buf_p );
	return atoi( token );
}

void Com_Parse1DMatrix( const char **buf_p, int x, float *m )
{
	const char *token;
	int i;

	Com_MatchToken( buf_p, "(", qfalse );

	for ( i = 0; i < x; i++ )
	{
		token = Com_Parse( buf_p );
		m[i] = atof( token );
	}

	Com_MatchToken( buf_p, ")", qfalse );
}

void Com_Parse2DMatrix( const char **buf_p, int y, int x, float *m )
{
	int i;

	Com_MatchToken( buf_p, "(", qfalse );

	for ( i = 0; i < y; i++ )
	{
		Com_Parse1DMatrix( buf_p, x, m + i * x );
	}

	Com_MatchToken( buf_p, ")", qfalse );
}

void Com_Parse3DMatrix( const char **buf_p, int z, int y, int x, float *m )
{
	int i;

	Com_MatchToken( buf_p, "(", qfalse );

	for ( i = 0; i < z; i++ )
	{
		Com_Parse2DMatrix( buf_p, y, x, m + i * x * y );
	}

	Com_MatchToken( buf_p, ")", qfalse );
}

