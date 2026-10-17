// stringed_ingame.cpp -- StringEd package parser.
#include <string>
#include <map>
#include "stringed_alloc.h"
#include <cstring>
#include <cctype>
#include <strings.h>
#include <stdlib.h>
using namespace std;

typedef const char *LPCSTR;
typedef int SE_BOOL;
#define SE_TRUE 1
#define SE_FALSE 0

#define iSE_MAX_FILENAME_LENGTH	64
#define iSE_VERSION				1
#define sSE_KEYWORD_LANG		"LANG_"

extern "C" void *Z_MallocInternal( int size );
extern "C" void Z_FreeInternal( void *ptr );
#define Z_Free Z_FreeInternal
extern char *va( const char *format, ... );
extern void I_strncpyz( char *dest, const char *src, int destsize );
extern char *I_strupr( char *s1 );
extern unsigned char *SE_LoadFileData( const char *psFileName );
extern void SE_FreeFileDataAfterLoad( unsigned char *psLoadedFile );
extern int SE_BuildFileList( const char *psStartDir, se_string &strResults );

typedef map< se_string, se_string, less<se_string>, se_alloc< pair<const se_string, se_string> > > mapStringEntries_t;

class CStringEdPackage
{
public:
	SE_BOOL				m_bEndMarkerFound_ParseOnly;
	se_string				m_strCurrentEntryRef_ParseOnly;
	se_string				m_strCurrentEntryEnglish_ParseOnly;
	se_string				m_strCurrentFileRef_ParseOnly;
	mapStringEntries_t	m_StringEntries;

	void *operator new( size_t size )
	{
		return Z_MallocInternal( sizeof(CStringEdPackage) );
	}
	void operator delete( void *ptr )
	{
		Z_Free( ptr );
	}

	void	Clear( void );
	void	SetupNewFileParse( LPCSTR psFileName );
	SE_BOOL	ReadLine( LPCSTR &psParsePos, char *psDest );
	LPCSTR	ParseLine( LPCSTR psLine, bool forceEnglish );
	LPCSTR	ExtractLanguageFromPath( LPCSTR psFileName );
	SE_BOOL	EndMarkerFoundDuringParse( void )
	{
		return m_bEndMarkerFound_ParseOnly;
	}
	bool	IsStringFormatCorrect( const char *psString );
	LPCSTR	GetCurrentReference_ParseOnly( void );

private:
	void	AddEntry( LPCSTR psLocalReference );
	void	SetString( LPCSTR psLocalReference, LPCSTR psNewString, SE_BOOL bEnglishDebug );
	SE_BOOL	CheckLineForKeyword( LPCSTR psKeyword, LPCSTR &psLine );
	se_string	InsideQuotes( LPCSTR psLine );
	se_string	ConvertCRLiterals_Read( se_string psString );
	void	REMKill( char *psBuffer );
	char	*Filename_PathOnly( LPCSTR psFilename );
	char	*Filename_WithoutPath( LPCSTR psFilename );
	char	*Filename_WithoutExt( LPCSTR psFilename );
};

static CStringEdPackage *TheStringPackage;

void CStringEdPackage::Clear( void )
{
	m_StringEntries.clear();

	m_bEndMarkerFound_ParseOnly = SE_FALSE;
	m_strCurrentEntryRef_ParseOnly = "";
	m_strCurrentEntryEnglish_ParseOnly = "";
}

// loses anything after the path (if any), (eg) "dir/name.bmp" becomes "dir"
char *CStringEdPackage::Filename_PathOnly( LPCSTR psFilename )
{
	static char sString[ iSE_MAX_FILENAME_LENGTH ];

	I_strncpyz( sString, psFilename, sizeof(sString) );

	char *p1 = strrchr(sString,'\\');
	char *p2 = strrchr(sString,'/');
	char *p = (p1>p2)?p1:p2;
	if (p)
		*p = 0;

	return sString;
}

// returns (eg) "dir/name" for "dir/name.bmp"
char *CStringEdPackage::Filename_WithoutExt( LPCSTR psFilename )
{
	static char sString[ iSE_MAX_FILENAME_LENGTH ];

	strcpy(sString,psFilename);

	char *p = strrchr(sString,'.');
	char *p2 = strrchr(sString,'\\');
	char *p3 = strrchr(sString,'/');

	// make sure the suffix found wasn't just a directory suffix
	if (p &&
		(p2==0 || (p2 && p>p2)) &&
		(p3==0 || (p3 && p>p3))
		)
		*p=0;

	return sString;
}

// returns actual filename only, no path
char *CStringEdPackage::Filename_WithoutPath( LPCSTR psFilename )
{
	static char sString[ iSE_MAX_FILENAME_LENGTH ];

	LPCSTR psCopyPos = psFilename;

	while (*psFilename)
	{
		if (*psFilename == '/' || *psFilename == '\\')
			psCopyPos = psFilename+1;
		psFilename++;
	}

	strcpy(sString,psCopyPos);

	return sString;
}

LPCSTR CStringEdPackage::ExtractLanguageFromPath( LPCSTR psFileName )
{
	return Filename_WithoutPath( Filename_PathOnly( psFileName ) );
}

void CStringEdPackage::SetupNewFileParse( LPCSTR psFileName )
{
	char sString[ iSE_MAX_FILENAME_LENGTH ];

	strcpy(sString, Filename_WithoutPath( Filename_WithoutExt( psFileName ) ));
	_strupr(sString);

	m_strCurrentFileRef_ParseOnly = sString;	// eg "OBJECTIVES"
}

SE_BOOL CStringEdPackage::CheckLineForKeyword( LPCSTR psKeyword, LPCSTR &psLine )
{
	if (!strncasecmp(psKeyword, psLine, strlen(psKeyword)))
	{
		psLine += strlen(psKeyword);

		// skip whitespace to arrive at next item
		while ( *psLine == '\t' || *psLine == ' ' )
		{
			psLine++;
		}
		return SE_TRUE;
	}

	return SE_FALSE;
}

bool CStringEdPackage::IsStringFormatCorrect( const char *psString )
{
	const char *p;
	char args[9];
	int argIndex;

	memset(args, 0, sizeof(args));
	for (p = strstr(psString, "&&"); p; p = strstr(p, "&&"))
	{
		p += 2;
		if (!isdigit(*p))
			return false;
		argIndex = *p - '0';
		argIndex--;
		if (args[argIndex])
			return false;
		args[argIndex] = 1;
		p++;
	}
	return true;
}

// change "\n" to '\n' (2-byte char-string to 1-byte ctrl-code)
se_string CStringEdPackage::ConvertCRLiterals_Read( se_string psString )
{
	se_string str;
	int iLoc;

	str = psString;
	while ( (iLoc = str.find("\\n")) != -1 )
	{
		str[iLoc] = '\n';
		str.erase( iLoc+1, 1 );
	}

	return str;
}

// kill off any "//" onwards part in the line, but NOT if it's inside a quoted string
void CStringEdPackage::REMKill( char *psBuffer )
{
	char *psScanPos = psBuffer;
	char *p;
	int iDoubleQuotesSoFar = 0;

	while ( (p=strstr(psScanPos,"//")) != NULL )
	{
		int iDoubleQuoteCount = iDoubleQuotesSoFar;

		for (int i=0; i<p-psScanPos; i++)
		{
			if (psScanPos[i] == '"')
			{
				iDoubleQuoteCount++;
			}
		}
		if (!(iDoubleQuoteCount&1))
		{
			*p='\0';

			if (psScanPos[0])
			{
				int iWhiteSpaceScanPos = strlen(psScanPos)-1;
				while (iWhiteSpaceScanPos>=0 && isspace(psScanPos[iWhiteSpaceScanPos]))
				{
					psScanPos[iWhiteSpaceScanPos--] = '\0';
				}
			}

			return;
		}
		else
		{
			psScanPos = p+1;
			iDoubleQuotesSoFar = iDoubleQuoteCount;
		}
	}
}

// returns true while new lines available to be read
SE_BOOL CStringEdPackage::ReadLine( LPCSTR &psParsePos, char *psDest )
{
	if (psParsePos[0])
	{
		LPCSTR psLineEnd = strchr(psParsePos, '\n');
		if (psLineEnd)
		{
			int iCharsToCopy = (psLineEnd - psParsePos);
			strncpy(psDest, psParsePos, iCharsToCopy);
			psDest[iCharsToCopy] = '\0';
			psParsePos += iCharsToCopy;
			while (*psParsePos && strchr("\r\n",*psParsePos))
			{
				psParsePos++;
			}
		}
		else
		{
			strcpy(psDest, psParsePos);
			psParsePos = psParsePos + strlen(psParsePos);
		}

		if (psDest[0])
		{
			int iWhiteSpaceScanPos = strlen(psDest)-1;
			while (iWhiteSpaceScanPos>=0 && isspace(psDest[iWhiteSpaceScanPos]))
			{
				psDest[iWhiteSpaceScanPos--] = '\0';
			}

			REMKill( psDest );
		}
		return SE_TRUE;
	}

	return SE_FALSE;
}

// remove any outside quotes from this supplied line, plus any leading or trailing whitespace
se_string CStringEdPackage::InsideQuotes( LPCSTR psLine )
{
	se_string str;

	str = "";

	while (*psLine == ' ' || *psLine == '\t')
	{
		psLine++;
	}

	if (*psLine == '"')
	{
		psLine++;
	}

	str = psLine;

	if (psLine[0])
	{
		while (	str.c_str()[ strlen(str.c_str()) -1 ] == ' ' ||
				str.c_str()[ strlen(str.c_str()) -1 ] == '\t'
				)
		{
			str.erase( strlen(str.c_str()) -1, 1);
		}

		if (str.c_str()[ strlen(str.c_str()) -1 ] == '"')
		{
			str.erase( strlen(str.c_str()) -1, 1);
		}
	}

	return str;
}

LPCSTR CStringEdPackage::ParseLine( LPCSTR psLine, bool forceEnglish )
{
	LPCSTR psErrorMessage = NULL;

	if (psLine)
	{
		if (CheckLineForKeyword( "VERSION", psLine ))
		{
			// VERSION 	"1"
			se_string strVersionNumber = InsideQuotes( psLine );
			int iVersionNumber = atoi( strVersionNumber.c_str() );

			if (iVersionNumber != iSE_VERSION)
			{
				psErrorMessage = va("Unexpected version number %d, expecting %d!\n", iVersionNumber, iSE_VERSION);
			}
		}
		else
		if (	CheckLineForKeyword("CONFIG", psLine)
			||	CheckLineForKeyword("FILENOTES", psLine)
			||	CheckLineForKeyword("NOTES", psLine)
			||	CheckLineForKeyword("FLAGS", psLine))
		{
			// not used ingame, but need to absorb the token
		}
		else
		if (CheckLineForKeyword("REFERENCE", psLine))
		{
			// REFERENCE	GUARD_GOOD_TO_SEE_YOU
			se_string strReference = InsideQuotes( psLine );
			AddEntry( strReference.c_str() );
		}
		else
		if (CheckLineForKeyword("ENDMARKER", psLine))
		{
			m_bEndMarkerFound_ParseOnly = SE_TRUE;	// the only major error checking done (for file truncation)
		}
		else
		if (!strncasecmp(sSE_KEYWORD_LANG, psLine, strlen(sSE_KEYWORD_LANG)))
		{
			// LANG_ENGLISH 	"GUARD:  Good to see you, sir."
			LPCSTR psReference = GetCurrentReference_ParseOnly();
			if ( psReference[0] )
			{
				psLine += strlen(sSE_KEYWORD_LANG);

				// what language is this?
				LPCSTR psWordEnd = psLine;
				while (*psWordEnd && *psWordEnd != ' ' && *psWordEnd != '\t')
				{
					psWordEnd++;
				}
				char sThisLanguage[1024]={0};
				int iCharsToCopy = psWordEnd - psLine;
				if ((unsigned int)iCharsToCopy > sizeof(sThisLanguage)-1)
				{
					iCharsToCopy = sizeof(sThisLanguage)-1;
				}
				strncpy(sThisLanguage, psLine, iCharsToCopy);

				psLine += strlen(sThisLanguage);
				se_string strSentence = ConvertCRLiterals_Read( InsideQuotes( psLine ) );

				if ( !IsStringFormatCorrect(strSentence.c_str()) )
				{
					psErrorMessage = va("Illegal string format \"%s\"\n", strSentence.c_str());
				}

				// see whether this is the english master or not
				SE_BOOL bSentenceIsEnglish = !strcasecmp(sThisLanguage,"english");

				if (!psErrorMessage && (bSentenceIsEnglish || !forceEnglish))
				{
					SetString( psReference, strSentence.c_str(), bSentenceIsEnglish );
				}
			}
			else
			{
				psErrorMessage = "Error parsing file: Unexpected \"" sSE_KEYWORD_LANG "\"\n";
			}
		}
		else
		{
			psErrorMessage = va("Unknown keyword at linestart: \"%s\"\n", psLine);
		}
	}

	return psErrorMessage;
}

// returns reference of string being parsed, else "" for none.
LPCSTR CStringEdPackage::GetCurrentReference_ParseOnly( void )
{
	return m_strCurrentEntryRef_ParseOnly.c_str();
}

// add new string entry (during parse)
void CStringEdPackage::AddEntry( LPCSTR psLocalReference )
{
	mapStringEntries_t::iterator itEntry = m_StringEntries.find( va("%s_%s",m_strCurrentFileRef_ParseOnly.c_str(), psLocalReference) );
	if (itEntry == m_StringEntries.end())
	{
		se_string SE_Entry;
		m_StringEntries[ va("%s_%s", m_strCurrentFileRef_ParseOnly.c_str(), psLocalReference) ] = SE_Entry;
	}
	m_strCurrentEntryRef_ParseOnly = psLocalReference;
}

void CStringEdPackage::SetString( LPCSTR psLocalReference, LPCSTR psNewString, SE_BOOL bEnglishDebug )
{
	mapStringEntries_t::iterator itEntry = m_StringEntries.find( va("%s_%s",m_strCurrentFileRef_ParseOnly.c_str(), psLocalReference) );
	se_string &Entry = itEntry->second;

	if ( bEnglishDebug )
	{
		Entry = psNewString;
		m_strCurrentEntryEnglish_ParseOnly = psNewString;	// for possible "#same" resolving in foreign later
	}
	else
	{
		if (!strcasecmp(psNewString, "#same"))
		{
			Entry = m_strCurrentEntryEnglish_ParseOnly;	// foreign "#same" is now english
		}
		else
		{
			Entry = psNewString;
		}
	}
}

static LPCSTR SE_GetFoundFile( se_string &strResult )
{
	static char sTemp[64];

	if (strlen(strResult.c_str()) == 0)
		return NULL;

	strncpy(sTemp,strResult.c_str(),sizeof(sTemp)-1);
	sTemp[sizeof(sTemp)-1]='\0';

	char *psSemiColon = strchr(sTemp,';');
	if (psSemiColon)
	{
		*psSemiColon = '\0';

		strResult.erase(0,(psSemiColon-sTemp)+1);
	}
	else
	{
		// no semicolon found, probably last entry
		strResult.erase();
	}

	return sTemp;
}

// return is either NULL for good else error message to display
LPCSTR SE_Load( LPCSTR psFileName, bool forceEnglish )
{
	char sLineBuffer[16384];
	LPCSTR psErrorMessage;
	unsigned char *psLoadedData;
	LPCSTR psParsePos;
	int len;

	psErrorMessage = NULL;
	psLoadedData = SE_LoadFileData( psFileName );
	if ( !psLoadedData )
		return va("Unable to load \"%s\"!", psFileName);

	psParsePos = (LPCSTR)psLoadedData;
	TheStringPackage->SetupNewFileParse( psFileName );

	while ( !psErrorMessage && TheStringPackage->ReadLine( psParsePos, sLineBuffer ) )
	{
		len = strlen(sLineBuffer);
		if (len)
		{
			psErrorMessage = TheStringPackage->ParseLine( sLineBuffer, forceEnglish );
		}
	}

	SE_FreeFileDataAfterLoad( psLoadedData );

	if (!psErrorMessage && !TheStringPackage->EndMarkerFoundDuringParse())
	{
		psErrorMessage = va("Truncated file, failed to find \"%s\" at file end!", "ENDMARKER");
	}

	return psErrorMessage;
}

LPCSTR SE_GetString( LPCSTR psPackageAndStringReference )
{
	mapStringEntries_t::iterator itEntry = TheStringPackage->m_StringEntries.find( psPackageAndStringReference );
	if (itEntry != TheStringPackage->m_StringEntries.end())
	{
		se_string &Entry = itEntry->second;
		return Entry.c_str();
	}
	return NULL;
}

void SE_NewLanguage( void )
{
	TheStringPackage->Clear();
}

void SE_Init( void )
{
	TheStringPackage = new CStringEdPackage();
	TheStringPackage->Clear();
}

void SE_ShutDown( void )
{
	if (!TheStringPackage)
		return;

	TheStringPackage->Clear();
	delete TheStringPackage;
	TheStringPackage = NULL;
}

// returns error message else NULL for ok
LPCSTR SE_LoadLanguage( bool forceEnglish )
{
	LPCSTR psErrorMessage = NULL;
	se_string strResults;

	SE_NewLanguage();

	SE_BuildFileList( "localizedstrings", strResults );

	LPCSTR psFileName;
	while ( (psFileName = SE_GetFoundFile( strResults )) != NULL && !psErrorMessage )
	{
		psErrorMessage = SE_Load( psFileName, forceEnglish );
	}

	return psErrorMessage;
}

