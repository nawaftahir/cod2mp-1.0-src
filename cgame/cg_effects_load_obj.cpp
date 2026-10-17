#include "../qcommon/qcommon.h"
#include "../universal/surfaceflags.h"

#define IMPACT_TYPECOUNT 11

struct FxEffectDef;
FxEffectDef *FX_RegisterEffect( const char *name );
int Com_SurfaceTypeFromName( const char *name );

struct FxImpactTable
{
	const char *name;
	FxEffectDef **table;
};

static const char *g_TypeName[IMPACT_TYPECOUNT] =
{
	"bullet_small_normal",
	"bullet_small_reflect",
	"bullet_large_normal",
	"bullet_large_reflect",
	"shotgun_normal",
	"shotgun_reflect",
	"bullet_ap_normal",
	"bullet_ap_reflect",
	"grenade_bounce",
	"grenade_explode",
	"rocket_explode"
};

static int CG_RegisterImpactEffectsForType( char **szEffectFile, const char *pszTypeName, FxEffectDef **fx )
{
	int iBadCount;
	int i;

	iBadCount = 0;
	for ( i = 0; i < SURF_TYPECOUNT; i++ )
	{
		if ( !szEffectFile[i] )
		{
			Com_Printf("no entry for effect type '%s' on surface type '%s'\n", pszTypeName, Com_SurfaceTypeToName(i));
			iBadCount++;
			fx[i] = NULL;
		}
		else if ( !szEffectFile[i][0] )
		{
			fx[i] = NULL;
		}
		else
		{
			fx[i] = FX_RegisterEffect(szEffectFile[i]);
		}
	}
	return iBadCount;
}

static const char *CG_ParseImpactEffects( const char *filename, const char *buf, int iTypeCount, const char **pszTypeName, char **szEffectFile )
{
	const char *token;
	int iEffectType;
	int iSurfaceType;
	int len;
	char *effectName;

	while ( 1 )
	{
		token = Com_Parse(&buf);
		if ( !buf )
			return NULL;
		if ( !token[0] || token[0] == '#' )
		{
			Com_SkipRestOfLine(&buf);
			continue;
		}
		for ( iEffectType = 0; iEffectType < iTypeCount; iEffectType++ )
		{
			if ( !stricmp(pszTypeName[iEffectType], token) )
				break;
		}
		if ( iEffectType == iTypeCount )
			return va("unknown effect type '%s' in first column of file '%s'", token, filename);
		token = Com_ParseOnLine(&buf);
		if ( !token[0] )
			return va("missing surface type in second column of file '%s'", filename);
		iSurfaceType = Com_SurfaceTypeFromName(token);
		if ( iSurfaceType < 0 )
			return va("unknown surface type '%s' in second column of file '%s'", token, filename);
		token = Com_ParseOnLineInl(&buf);
		len = strlen(token);
		if ( len >= 64 )
			return va("effect filename '%s' in third column of file '%s' is longer than %i characters", token, filename, 63);
		effectName = (char *)Hunk_AllocateTempMemoryInternal(len + 1);
		szEffectFile[iEffectType * SURF_TYPECOUNT + iSurfaceType] = effectName;
		{
			const char *src = token;
			char *dst = effectName;
			char c;
			do
			{
				c = *src++;
				*dst++ = c;
			} while ( c );
		}
		Com_SkipRestOfLine(&buf);
	}
}

static int compare_impact_files( const char **pe0, const char **pe1 )
{
	const char *s0;
	const char *s1;

	s0 = *pe0;
	s1 = *pe1;
	return stricmp(s0, s1);
}

static void CG_RegisterImpactEffectsForDir( const char *dir, char **szEffectFile, char *listbuf )
{
	const char *pszFiles[4096];
	int iListCount;
	int i;
	char *psz;
	const char *filename;
	int len;
	fileHandle_t f;
	char *buf;
	const char *pszErrorMsg;

	iListCount = FS_GetFileList(dir, "csv", FS_LIST_PURE_ONLY, listbuf, 0x10000);
	if ( !iListCount )
		return;
	if ( (unsigned)iListCount > 4096 )
		iListCount = 4096;
	psz = listbuf;
	for ( i = 0; i < iListCount; i++ )
	{
		pszFiles[i] = psz;
		psz += strlen(psz) + 1;
	}
	qsort(pszFiles, iListCount, sizeof(pszFiles[0]), (int (*)( const void *, const void * ))compare_impact_files);
	for ( i = 0; i < iListCount; i++ )
	{
		filename = va("%s/%s", dir, pszFiles[i]);
		len = FS_FOpenFileByMode(filename, &f, FS_READ);
		if ( len < 0 )
			continue;
		buf = (char *)Hunk_AllocateTempMemoryHighInternal(len + 1);
		FS_Read(buf, len, f);
		FS_FCloseFile(f);
		buf[len] = 0;
		Com_BeginParseSession(filename);
		Com_SetCSV(qtrue);
		pszErrorMsg = CG_ParseImpactEffects(filename, buf, IMPACT_TYPECOUNT, g_TypeName, szEffectFile);
		Com_EndParseSessionInl();
		Hunk_ClearTempMemoryHighInternal();
		if ( pszErrorMsg )
		{
			Com_Printf("^1ERROR: %s", pszErrorMsg);
			return;
		}
	}
}

FxImpactTable *CG_RegisterImpactEffects( const char *mapname )
{
	char *listbuf;
	char *szEffectFile[IMPACT_TYPECOUNT * SURF_TYPECOUNT];
	char mapdir[64];
	FxImpactTable *fx;
	int iBadCount;
	unsigned int i;

	listbuf = (char *)Hunk_AllocateTempMemoryInternal(0x10000);
	memset(szEffectFile, 0, sizeof(szEffectFile));
	CG_RegisterImpactEffectsForDir("fx", szEffectFile, listbuf);
	if ( mapname )
	{
		Com_sprintf(mapdir, sizeof(mapdir), "fx/maps/%s", mapname);
		CG_RegisterImpactEffectsForDir(mapdir, szEffectFile, listbuf);
	}
	fx = (FxImpactTable *)Hunk_AllocAlignInternal(sizeof(FxImpactTable), 4);
	fx->table = (FxEffectDef **)Hunk_AllocAlignInternal(sizeof(szEffectFile), 4);
	iBadCount = 0;
	for ( i = 0; i < IMPACT_TYPECOUNT; i++ )
		iBadCount += CG_RegisterImpactEffectsForType(&szEffectFile[i * SURF_TYPECOUNT], g_TypeName[i], &fx->table[i * SURF_TYPECOUNT]);
	if ( iBadCount )
	{
		Com_Printf("^1ERROR: %i missing entries in effect CSV files (see console for details)", iBadCount);
		return NULL;
	}
	fx->name = "";
	Hunk_ClearTempMemoryInternal();
	return fx;
}

