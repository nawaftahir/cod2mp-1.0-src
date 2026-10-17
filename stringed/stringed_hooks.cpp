#include "../qcommon/qcommon.h"
#include "../stringed/stringed_public.h"

int FS_LanguageHasAssets( int iLanguage );
const char *SE_GetString( const char *psPackageAndStringReference );
const char *SE_LoadLanguage( bool forceEnglish );
void SE_Init();
void SE_ShutDown();


static languageInfo_t g_languages[MAX_LANGUAGES] =
{
	{ "english", 0 },
	{ "french", 0 },
	{ "german", 0 },
	{ "italian", 0 },
	{ "spanish", 0 },
	{ "british", 0 },
	{ "russian", 0 },
	{ "polish", 0 },
	{ "korean", 0 },
	{ "taiwanese", 0 },
	{ "japanese", 0 },
	{ "chinese", 0 },
	{ "thai", 0 },
	{ "leet", 0 }
};

static int g_currentAsian;

// Original name and purpose unknown; unreferenced.
const int stringedHooksUnused = 2;

void SEH_UpdateLanguageInfo();
int SEH_StringEd_SetLanguageStrings( int iLanguage );
int Language_IsAsian();
unsigned char SEH_IsKoreanChar( unsigned int c ) throw();
unsigned char SEH_IsJapaneseChar( unsigned int c ) throw();
unsigned char SEH_IsChineseChar( unsigned int c ) throw();
int SEH_IsKoreanLeadTrail( unsigned char lead, unsigned char trail );

int SEH_StringEd_SetLanguageStrings( int iLanguage );
int SEH_IsChineseLeadTrail( unsigned char lead, unsigned char trail );
int Language_IsAsian();
const char *SEH_GetLanguageName( const int iLanguage );
int SEH_IsKoreanLeadTrail( unsigned char lead, unsigned char trail );
unsigned char SEH_IsKoreanChar( unsigned int c ) throw();







void Language_UpdateCurrentAsian()
{
	switch ( loc_language->current.integer )
	{
	case LANGUAGE_KOREAN:
	case LANGUAGE_TAIWANESE:
	case LANGUAGE_JAPANESE:
	case LANGUAGE_CHINESE:
	case LANGUAGE_THAI:
		g_currentAsian = 1;
		break;
	default:
		g_currentAsian = 0;
		break;
	}
}

int SEH_GetCurrentLanguage()
{
	return loc_language->current.integer;
}

void SEH_InitLanguage()
{
	loc_language = Dvar_RegisterInt("loc_language", 0, 0, MAX_LANGUAGES - 1, DVAR_ARCHIVE | DVAR_LATCH | DVAR_CHANGEABLE_RESET);
	loc_forceEnglish = Dvar_RegisterBool("loc_forceEnglish", false, DVAR_ARCHIVE | DVAR_LATCH | DVAR_CHANGEABLE_RESET);
	loc_translate = Dvar_RegisterBool("loc_translate", true, DVAR_LATCH | DVAR_CHANGEABLE_RESET);
	loc_warnings = Dvar_RegisterBool("loc_warnings", false, DVAR_CHANGEABLE_RESET);
	loc_warningsAsErrors = Dvar_RegisterBool("loc_warningsAsErrors", false, DVAR_CHANGEABLE_RESET);
	Language_UpdateCurrentAsian();
}

void SEH_UpdateLanguageInfo()
{
	int i;
	int numLanguages;

	Dvar_RegisterInt(loc_language->name, 0, 0, MAX_LANGUAGES - 1, DVAR_ARCHIVE | DVAR_LATCH | DVAR_CHANGEABLE_RESET);
	Dvar_RegisterBool(loc_forceEnglish->name, false, DVAR_ARCHIVE | DVAR_LATCH | DVAR_CHANGEABLE_RESET);
	Language_UpdateCurrentAsian();
	numLanguages = 0;
	for ( i = 0; i < MAX_LANGUAGES; i++ )
	{
		if ( FS_LanguageHasAssets(i) )
		{
			g_languages[i].bPresent = 1;
			numLanguages++;
		}
		else
		{
			g_languages[i].bPresent = 0;
		}
	}
	if ( numLanguages < 1 )
		Com_Printf("^1ERROR: No languages available because no localized assets were found\n");
	if ( SEH_StringEd_SetLanguageStrings(loc_language->current.integer) )
		return;
	for ( i = 0; i < MAX_LANGUAGES; i++ )
	{
		Dvar_SetInt(loc_language, i);
		Language_UpdateCurrentAsian();
		if ( SEH_StringEd_SetLanguageStrings(i) )
			return;
	}
	Dvar_SetInt(loc_language, 0);
	Language_UpdateCurrentAsian();
}

int SEH_VerifyLanguageSelection( int iLanguage )
{
	int i;

	if ( g_languages[iLanguage].bPresent )
		return iLanguage;
	for ( i = 0; i < MAX_LANGUAGES; i++ )
	{
		if ( g_languages[(iLanguage + i) % MAX_LANGUAGES].bPresent )
			return (iLanguage + i) % MAX_LANGUAGES;
	}
	return 0;
}

int SEH_StringEd_SetLanguageStrings( int iLanguage )
{
	const char *psErrorMessage;

	if ( !g_languages[iLanguage].bPresent )
		return 0;
	psErrorMessage = SE_LoadLanguage(loc_forceEnglish->current.boolean);
	if ( !psErrorMessage )
		return 1;
	if ( !fs_ignoreLocalized->current.boolean && loc_warnings->current.boolean )
	{
		if ( loc_warningsAsErrors->current.boolean )
			Com_Error(ERR_LOCALIZATION, "Could not load localization strings for %s: %s", SEH_GetLanguageName(iLanguage), psErrorMessage);
		else
			Com_Printf("^3WARNING: Could not load localization strings for %s: %s\n", SEH_GetLanguageName(iLanguage), psErrorMessage);
	}
	return 0;
}

const char *SEH_StringEd_GetString( const char *pszReference )
{
	if ( !loc_translate || !loc_translate->current.boolean )
		return pszReference;
	if ( !pszReference[0] || ( pszReference[0] && !pszReference[1] ) )
		return pszReference;
	return SE_GetString(pszReference);
}

const char *SEH_SafeTranslateString( const char *pszReference )
{
	static char szErrorString[1024];
	const char *pszTranslated;

	pszTranslated = SEH_StringEd_GetString(pszReference);
	if ( !pszTranslated )
	{
		if ( loc_warnings->current.boolean )
		{
			if ( loc_warningsAsErrors->current.boolean )
				Com_Error(ERR_LOCALIZATION, "Could not translate exe string \"%s\"", pszReference);
			else
				Com_Printf("^3WARNING: Could not translate exe string \"%s\"\n", pszReference);
			strcpy(szErrorString, "^1UNLOCALIZED(^7");
			I_strncat(szErrorString, sizeof(szErrorString), pszReference);
			I_strncat(szErrorString, sizeof(szErrorString), "^1)^7");
		}
		else
		{
			I_strncpyz(szErrorString, pszReference, sizeof(szErrorString));
		}
		pszTranslated = szErrorString;
	}
	return pszTranslated;
}

int SEH_GetLocalizedTokenReference( char *token, const char *reference, const char *messageType, msgLocErrType_t errType )
{
	const char *translation;

	translation = SEH_StringEd_GetString(reference);
	if ( !translation )
	{
		if ( loc_warnings && loc_warnings->current.boolean )
		{
			if ( loc_warningsAsErrors && loc_warningsAsErrors->current.boolean && errType != LOCMSG_NOERR )
				Com_Error(ERR_LOCALIZATION, "Could not translate part of %s: \"%s\"", messageType, reference);
			else
				Com_Printf("^3WARNING: Could not translate part of %s: \"%s\"\n", messageType, reference);
			translation = va("^1UNLOCALIZED(^7%s^1)^7", reference);
		}
		else
		{
			translation = va("%s", reference);
		}
		if ( errType == LOCMSG_NOERR )
			return 0;
	}
	strcpy(token, translation);
	return 1;
}

const char *SEH_LocalizeTextMessage( const char *pszInputBuffer, const char *pszMessageType, msgLocErrType_t errType )
{
	static int iCurrString;
	static char szStrings[2][1024];
	int i;
	int outputLen;
	size_t iTokenLen;
	char *pszString;
	char szTokenBuf[1024];
	char szInsertBuf[1024];
	const char *pszScanStart;
	const char *pszIn;
	int bLocOn;
	int bInsertEnabled;
	int iInsertLevel;
	int bLocSkipped;
	int insertIndex;
	int digit;

	iCurrString = ( iCurrString + 1 ) % 2;
	memset(szStrings[iCurrString], 0, sizeof(szStrings[0]));
	pszString = szStrings[iCurrString];
	outputLen = 0;
	bLocOn = 1;
	bInsertEnabled = 1;
	iInsertLevel = 0;
	insertIndex = 1;
	bLocSkipped = 0;
	pszScanStart = pszInputBuffer;
	pszIn = pszInputBuffer;
	while ( *pszScanStart )
	{
		if ( !*pszIn || *pszIn == 0x14 || *pszIn == 0x15 || *pszIn == 0x16 )
		{
			if ( pszIn > pszScanStart )
			{
				iTokenLen = pszIn - pszScanStart;
				I_strncpyz(szTokenBuf, pszScanStart, iTokenLen + 1);
				if ( bLocOn )
				{
					if ( !SEH_GetLocalizedTokenReference(szTokenBuf, szTokenBuf, pszMessageType, errType) )
						return NULL;
					iTokenLen = strlen(szTokenBuf);
				}
				if ( outputLen + iTokenLen >= sizeof(szStrings[0]) )
				{
					if ( loc_warnings && loc_warnings->current.boolean && loc_warningsAsErrors && loc_warningsAsErrors->current.boolean && errType != LOCMSG_NOERR )
						Com_Error(ERR_DROP, "%s too long when translated: \"%s\"", pszMessageType, pszInputBuffer);
					Com_Printf("%s too long when translated: \"%s\"\n", pszMessageType, pszInputBuffer);
				}
				for ( i = 0; i < (int)( iTokenLen - 2 ); i++ )
				{
					if ( !strncmp(&szTokenBuf[i], "&&", 2) && isdigit(szTokenBuf[i + 2]) )
					{
						if ( !bInsertEnabled )
						{
							szTokenBuf[i] = 0x16;
							bLocSkipped = 1;
						}
						else
						{
							iInsertLevel++;
						}
					}
				}
				if ( iInsertLevel > 0 && outputLen > 0 )
				{
					for ( i = 0; i < outputLen - 2; i++ )
					{
						if ( !strncmp(&pszString[i], "&&", 2) && isdigit(pszString[i + 2]) )
						{
							digit = pszString[i + 2] - '0';
							if ( !digit )
								Com_Printf("%s cannot have &&0 as conversion format: \"%s\"\n", pszMessageType, pszInputBuffer);
							if ( digit == insertIndex )
							{
								strcpy(szInsertBuf, &pszString[i + 3]);
								pszString[i] = 0;
								insertIndex++;
								break;
							}
						}
					}
					strcpy(&pszString[i], szTokenBuf);
					strcpy(&pszString[i + iTokenLen], szInsertBuf);
					outputLen -= 3;
					iInsertLevel--;
				}
				else
				{
					strcpy(&pszString[outputLen], szTokenBuf);
				}
				outputLen += iTokenLen;
			}
			bInsertEnabled = 1;
			if ( *pszIn == 0x14 )
			{
				bLocOn = 1;
				pszIn++;
			}
			else if ( *pszIn == 0x15 )
			{
				bLocOn = 0;
				pszIn++;
			}
			if ( *pszIn == 0x16 )
			{
				bInsertEnabled = 0;
				pszIn++;
			}
			pszScanStart = pszIn;
		}
		else
		{
			pszIn++;
		}
	}
	if ( bLocSkipped )
	{
		for ( i = 0; i < outputLen; i++ )
		{
			if ( pszString[i] == 0x16 )
				pszString[i] = '%';
		}
	}
	return pszString;
}

unsigned char SEH_IsKoreanChar( unsigned int c ) throw()
{
	return SEH_IsKoreanLeadTrail(c >> 8, c);
}

unsigned int SEH_KoreanCharToIndex( unsigned int c )
{
	if ( SEH_IsKoreanChar(c) )
	{
		c -= 0xb0a0;
		c = ( c >> 8 ) * 96 + ( c & 0xff );
		return c;
	}
	return 0;
}

unsigned char SEH_IsTraditionalChineseChar( unsigned int c )
{
	unsigned char lead;
	unsigned char trail;

	lead = c >> 8;
	if ( ( lead >= 0xa1 && lead <= 0xc6 ) || ( lead >= 0xc9 && lead <= 0xf9 ) )
	{
		trail = c;
		if ( ( trail >= 0x40 && trail <= 0x7e ) || ( trail >= 0xa1 && trail <= 0xfe ) )
			return 1;
	}
	return 0;
}

unsigned char SEH_IsTraditionalChinesePunctuation( unsigned int c )
{
	if ( c >= 0xa140 && c < 0xa154 )
		return 1;
	return 0;
}

unsigned int SEH_TraditionalChineseCharToIndex( unsigned int c )
{
	if ( SEH_IsTraditionalChineseChar(c) )
	{
		c -= 0xa140;
		if ( ( c & 0xff ) >= 0x60 )
			c -= 0x20;
		c = ( c >> 8 ) * 160 + ( c & 0xff );
		return c;
	}
	return 0;
}

unsigned char SEH_IsJapaneseLeadTrail( unsigned char lead, unsigned char trail )
{
	if ( ( lead >= 0x81 && lead <= 0x9f ) || ( lead >= 0xe0 && lead <= 0xef ) )
	{
		if ( ( trail >= 0x40 && trail <= 0x7e ) || ( trail >= 0x80 && trail <= 0xfc ) )
			return 1;
	}
	return 0;
}
unsigned char SEH_IsJapaneseChar( unsigned int c ) throw()
{
	return SEH_IsJapaneseLeadTrail(c >> 8, c);
}

unsigned char SEH_IsJapanesePunctuation( unsigned int c )
{
	if ( c >= 0x8140 && c < 0x8152 )
		return 1;
	return 0;
}

unsigned int SEH_JapaneseCharToIndex( unsigned int c )
{
	if ( SEH_IsJapaneseChar(c) )
	{
		c -= 0x8140;
		if ( ( c & 0xff ) >= 0x40 )
			c--;
		if ( ( c & 0xff00 ) >= 0x5f00 )
			c -= 0x4000;
		c = ( c >> 8 ) * 0xbc + ( c & 0xff );
		return c;
	}
	return 0;
}

unsigned char SEH_IsChineseChar( unsigned int c ) throw()
{
	return SEH_IsChineseLeadTrail(c >> 8, c);
}

bool SEH_IsChinesePunctuation( unsigned int c )
{
	if ( c > 0xa1a0 && c < 0xa1ae )
		return 1;
	return 0;
}

unsigned int SEH_ChineseCharToIndex( unsigned int c )
{
	if ( SEH_IsChineseChar(c) )
	{
		c -= 0xa1a0;
		c = ( c >> 8 ) * 95 + ( c & 0xff );
		return c;
	}
	return 0;
}

unsigned int SEH_ReadCharFromString( const char **text, int *isTrailingPunctuation )
{
	const unsigned char *str;
	unsigned int letter;

	str = (const unsigned char *)*text;
	if ( Language_IsAsian() )
	{
		switch ( SEH_GetCurrentLanguage() )
		{
		case LANGUAGE_KOREAN:
			if ( (unsigned char)SEH_IsKoreanLeadTrail(str[0], str[1]) )
			{
				letter = ( str[0] << 8 ) + str[1];
				*text += 2;
				if ( isTrailingPunctuation )
					*isTrailingPunctuation = 0;
				return letter;
			}
			break;
		case LANGUAGE_TAIWANESE:
			if ( SEH_IsTraditionalChineseChar(( str[0] << 8 ) + str[1]) )
			{
				letter = ( str[0] << 8 ) + str[1];
				*text += 2;
				if ( isTrailingPunctuation )
					*isTrailingPunctuation = SEH_IsTraditionalChinesePunctuation(letter);
				return letter;
			}
			break;
		case LANGUAGE_JAPANESE:
			if ( SEH_IsJapaneseLeadTrail(str[0], str[1]) )
			{
				letter = ( str[0] << 8 ) + str[1];
				*text += 2;
				if ( isTrailingPunctuation )
					*isTrailingPunctuation = SEH_IsJapanesePunctuation(letter);
				return letter;
			}
			break;
		case LANGUAGE_CHINESE:
			if ( SEH_IsChineseChar(( str[0] << 8 ) + str[1]) )
			{
				letter = ( str[0] << 8 ) + str[1];
				*text += 2;
				if ( isTrailingPunctuation )
					*isTrailingPunctuation = SEH_IsChinesePunctuation(letter) ? 1 : 0;
				return letter;
			}
			break;
		}
	}
	letter = str[0];
	*text += 1;
	if ( isTrailingPunctuation )
		*isTrailingPunctuation = letter == '!' || letter == '?' || letter == ',' || letter == '.' || letter == ';' || letter == ':';
	return letter;
}

int Language_IsAsian()
{
	return g_currentAsian;
}

// Original name unknown. False for the languages written without word spaces.
int SEH_LanguageUsesSpaces()
{
	switch ( SEH_GetCurrentLanguage() )
	{
	case LANGUAGE_TAIWANESE:
	case LANGUAGE_JAPANESE:
	case LANGUAGE_CHINESE:
		return 0;
	default:
		return 1;
	}
}

// Printable length, skipping color codes and line breaks.
int SEH_PrintStrlen( const char *string )
{
	int len;
	int letter;
	const char *str;

	if ( !string )
		return 0;
	len = 0;
	str = string;
	while ( *str )
	{
		letter = SEH_ReadCharFromString(&str, NULL);
		if ( letter == '^' && str && *str != '^' && *str >= '0' && *str <= '9' )
			str++;
		else if ( letter != '\n' && letter != '\r' )
			len++;
	}
	return len;
}

const char *SEH_GetLanguageName( const int iLanguage )
{
	if ( iLanguage < 0 || iLanguage >= MAX_LANGUAGES )
		return g_languages[0].pszName;
	return g_languages[iLanguage].pszName;
}

qboolean SEH_GetLanguageIndexForName( const char *language, int *langindex )
{
	int i;

	for ( i = 0; i < MAX_LANGUAGES; i++ )
	{
		if ( !I_stricmp(language, g_languages[i].pszName) )
		{
			*langindex = i;
			return qtrue;
		}
	}
	*langindex = 0;
	return qfalse;
}

void SEH_Init_StringEd()
{
	SE_Init();
}
void SEH_Shutdown_StringEd()
{
	SE_ShutDown();
}
int SEH_IsChineseLeadTrail( unsigned char lead, unsigned char trail )
{
	if ( lead >= 0xa1 && lead <= 0xf7 && trail > 0xa0 && trail < 0xff )
		return 1;
	return 0;
}
int SEH_IsKoreanLeadTrail( unsigned char lead, unsigned char trail )
{
	if ( lead >= 0xb0 && lead <= 0xc8 && trail > 0xa0 && trail < 0xff )
		return 1;
	return 0;
}
