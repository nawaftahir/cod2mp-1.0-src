#include "../qcommon/qcommon.h"
#include "g_shared.h"

static const char *accuracyDirName[] =
{
	"aivsai",
	"aivsplayer",
	NULL
};

static bool G_ParseAIWeaponAccurayGraphFile( const char *buffer, const char *fileName, float (*knots)[2], int *knotCount );
static bool G_ParseWeaponAccurayGraphInternal( WeaponDef *weaponDef, const char *dirName, const char *graphName, float (*knots)[2], int *knotCount );
bool G_ParseWeaponAccurayGraphs( WeaponDef *weaponDef );

/*
====================
G_ParseAIWeaponAccurayGraphFile
====================
*/
static bool G_ParseAIWeaponAccurayGraphFile( const char *buffer, const char *fileName, float (*knots)[2], int *knotCount )
{
	const char *token;
	int knotCountIndex;

	Com_BeginParseSession(fileName);
	token = Com_Parse(&buffer);

	*knotCount = atoi(token);
	knotCountIndex = 0;

	while ( 1 )
	{
		token = Com_Parse(&buffer);

		if ( !token[0] || *token == '}' )
		{
			break;
		}

		knots[knotCountIndex][0] = (float)atof(token);
		token = Com_Parse(&buffer);

		if ( !token[0] || *token == '}' )
		{
			break;
		}

		knots[knotCountIndex][1] = (float)atof(token);
		knotCountIndex++;

		if ( knotCountIndex >= 16 )
		{
			Com_Printf("^3WARNING: \"%s\" has too many graph knots\n", fileName);
			Com_EndParseSessionInl();
			return false;
		}
	}

	Com_EndParseSessionInl();

	if ( knotCountIndex != *knotCount )
	{
		Com_Printf("^3WARNING: \"%s\" Error in parsing an ai weapon accuracy file\n", fileName);
		return false;
	}

	return true;
}

/*
====================
G_ParseWeaponAccurayGraphInternal
====================
*/
static bool G_ParseWeaponAccurayGraphInternal( WeaponDef *weaponDef, const char *dirName, const char *graphName, float (*knots)[2], int *knotCount )
{
	int l;
	fileHandle_t f;
	char string[MAX_QPATH];
	char buffer[8192];
	const char *last = "WEAPONACCUFILE";
	int len = strlen(last);
	int count;

	if ( weaponDef->weaponType != WEAPTYPE_BULLET && weaponDef->weaponType != WEAPTYPE_PROJECTILE )
	{
		return true;
	}

	if ( !graphName[0] )
	{
		return true;
	}

	sprintf(string, "accuracy/%s/%s", dirName, graphName);
	l = FS_FOpenFileByMode(string, &f, FS_READ);

	if ( l < 0 )
	{
		Com_Printf("^3WARNING: Could not load ai weapon accuracy file '%s'\n", string);
		return false;
	}

	FS_Read(buffer, len, f);
	buffer[len] = 0;

	if ( strncmp( buffer, last, len ) )
	{
		Com_Printf("^3WARNING: \"%s\" does not appear to be an ai weapon accuracy file\n", string);
		FS_FCloseFile(f);
		return false;
	}

	if ( l - len >= (int)sizeof(buffer) )
	{
		Com_Printf("^3WARNING: \"%s\" Is too long of an ai weapon accuracy file to parse\n", string);
		FS_FCloseFile(f);
		return false;
	}

	memset(buffer, 0, sizeof(buffer));

	FS_Read(buffer, l - len, f);
	buffer[l - len] = 0;
	FS_FCloseFile(f);

	count = 0;

	if ( !G_ParseAIWeaponAccurayGraphFile( buffer, string, knots, &count ) )
	{
		return false;
	}

	*knotCount = count;

	return true;
}

/*
====================
G_ParseWeaponAccurayGraphs
====================
*/
bool G_ParseWeaponAccurayGraphs( WeaponDef *weaponDef )
{
	float accuracyGraphKnots[16][2];
	int accuracyGraphKnotCount;
	int allocSize;
	int weaponType;

	for ( weaponType = 0; weaponType < 2; weaponType++ )
	{
		memset(accuracyGraphKnots, 0, sizeof(accuracyGraphKnots));
		accuracyGraphKnotCount = 0;

		if ( !G_ParseWeaponAccurayGraphInternal( weaponDef, accuracyDirName[weaponType], weaponDef->accuracyGraphName[weaponType], accuracyGraphKnots, &accuracyGraphKnotCount ) )
		{
			return false;
		}

		if ( accuracyGraphKnotCount > 0 )
		{
			allocSize = 8 * accuracyGraphKnotCount;

			weaponDef->accuracyGraphKnots[weaponType] = (float (*)[2])Hunk_AllocLowAlignInternal( allocSize, 4 );
			memcpy(weaponDef->accuracyGraphKnots[weaponType], accuracyGraphKnots, allocSize);
			weaponDef->accuracyGraphKnotCount[weaponType] = accuracyGraphKnotCount;
		}
	}

	return true;
}

