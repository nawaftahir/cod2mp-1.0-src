#include "../qcommon/qcommon.h"
#include "com_sndalias.h"

static g_sa_t g_sa;

snd_alias_list_t *Com_FindSoundAlias( const char *name );
snd_alias_t *Com_PickSoundAliasFromList( snd_alias_list_t *aliasList );
void *Com_AllocSoundMemory( int size, const char *name, int type );

enum snd_stopsounds_arg_t
{
	SND_STOP_ALL = 0
};
void SND_StopSounds( snd_stopsounds_arg_t which );
int SND_GetSoundFileSize( const void *pSoundFile );
extern const dvar_t *snd_errorOnMissing;

float GraphGetValueFromFraction( int knotCount, const float (*knots)[2], float fraction );

snd_alias_t *Com_PickSoundAlias( const char *aliasName )
{
	snd_alias_list_t *aliasList;

	aliasList = Com_FindSoundAlias(aliasName);
	return Com_PickSoundAliasFromList(aliasList);
}

float Com_GetVolumeFalloffCurveValue( SndCurve *volumeFalloffCurve, float fraction )
{
	return GraphGetValueFromFraction(volumeFalloffCurve->knotCount, volumeFalloffCurve->knots, fraction);
}

// retail 0x42c3b0, unreferenced; name unknown
int Com_GetVolumeFalloffCurveKnots_42c3b0( SndCurve *volumeFalloffCurve, vec2_t **knots )
{
	*knots = volumeFalloffCurve->knots;
	return volumeFalloffCurve->knotCount;
}

static void Com_LoadedSoundList( snd_alias_system_t system )
{
	int totalSize;
	snd_alias_t *head;
	int i;
	int j;
	int size;

	if ( !g_sa.initialized[system] )
	{
		return;
	}
	totalSize = 0;
	head = g_sa.aliasInfo[system].head;
	for ( i = 0; i < g_sa.aliasInfo[system].count; i++ )
	{
		if ( ((head[i].flags & 0x60) >> 5) != SAT_LOADED )
		{
			continue;
		}
		for ( j = 0; j < i; j++ )
		{
			if ( ((head[j].flags & 0x60) >> 5) == SAT_LOADED && head[j].soundFile->soundName == head[i].soundFile->soundName )
			{
				goto next;
			}
		}
		if ( head[i].soundFile->fileMem )
		{
			size = SND_GetSoundFileSize(head[i].soundFile->fileMem);
			totalSize += size;
			Com_Printf("%-64s %7.1f KB\n", head[i].soundFile->soundName, (float)size * 0.0009765625f);
		}
		else
		{
			Com_Printf("%-64s FAILED TO LOAD\n", head[i].soundFile->soundName);
		}
next:
		;
	}
	Com_Printf("\ntotal usage %7.3f MB\n", (float)totalSize * 0.00000095367431640625f);
}

static void Com_StreamedSoundList( snd_alias_system_t system )
{
	snd_alias_t *head;
	int i;
	int j;

	if ( !g_sa.initialized[system] )
	{
		return;
	}
	head = g_sa.aliasInfo[system].head;
	for ( i = 0; i < g_sa.aliasInfo[system].count; i++ )
	{
		if ( ((head[i].flags & 0x60) >> 5) != SAT_STREAMED )
		{
			continue;
		}
		for ( j = 0; j < i; j++ )
		{
			if ( ((head[j].flags & 0x60) >> 5) == SAT_STREAMED && head[j].soundFile->soundName == head[i].soundFile->soundName )
			{
				goto next;
			}
		}
		if ( head[i].soundFile->isStreamFound )
		{
			Com_Printf("%-64s\n", head[i].soundFile->soundName);
		}
		else
		{
			Com_Printf("%-64s FILE NOT FOUND\n", head[i].soundFile->soundName);
		}
next:
		;
	}
}

static void Com_SoundList_f()
{
	Com_Printf("\n________________________________________\ncurrently streamed menu sounds:\n");
	Com_StreamedSoundList(SASYS_UI);
	Com_Printf("\n________________________________________\ncurrently streamed in-game sounds:\n");
	Com_StreamedSoundList(SASYS_CGAME);
	Com_Printf("________________________________________\ncurrently loaded menu sounds:\n");
	Com_LoadedSoundList(SASYS_UI);
	Com_Printf("\n________________________________________\ncurrently loaded in-game sounds:\n");
	Com_LoadedSoundList(SASYS_CGAME);
	Com_Printf("\n");
}

int Com_SoundAliasHash( const char *name )
{
	int hash;

	hash = 0;
	while ( *name )
	{
		hash = hash * 31337 + tolower(*name);
		name++;
	}
	return hash & 0x3ff;
}

snd_alias_list_t *Com_FindSoundAlias( const char *name )
{
	int hash;
	snd_alias_list_t *alias;

	if ( !name )
	{
		return NULL;
	}

	hash = Com_SoundAliasHash(name);

	for ( alias = g_sa.hash[hash]; alias; alias = alias->next )
	{
		if ( !I_stricmp(name, alias->aliasName) )
		{
			return alias;
		}
	}

	return NULL;
}

int SND_GetAliasOffset( const snd_alias_t *alias )
{
	snd_alias_list_t *aliasList;
	snd_alias_t *aliasOffset;
	int i;

	aliasList = Com_FindSoundAlias(alias->aliasName);
	aliasOffset = aliasList->head;
	for ( i = 0; i < aliasList->count; i++ )
	{
		if ( aliasOffset == alias )
		{
			return i;
		}
		aliasOffset++;
	}
	return 0;
}

snd_alias_t *SND_GetAliasWithOffset( const char *name, int offset )
{
	snd_alias_list_t *aliasList;
	snd_alias_t *alias;
	int i;

	aliasList = Com_FindSoundAlias(name);
	alias = aliasList->head;
	for ( i = 0; i < aliasList->count; i++ )
	{
		if ( i == offset )
		{
			return alias;
		}
		alias++;
	}
	if ( aliasList->count )
	{
		return aliasList->head;
	}
	Com_Error(ERR_DROP, "SND_GetAliasWithOffset: could not find sound alias '%s' with offset %d", name, offset);
	return NULL;
}

bool Com_AddAliasList( const char *name, snd_alias_list_t *aliasList )
{
	int hash;
	snd_alias_list_t *alias;

	hash = Com_SoundAliasHash(name);
	for ( alias = g_sa.hash[hash]; alias; alias = alias->next )
	{
		if ( !I_stricmp(name, alias->aliasName) )
		{
			return false;
		}
	}
	aliasList->next = g_sa.hash[hash];
	g_sa.hash[hash] = aliasList;
	return true;
}

void Com_DuplicateSoundAlias( const snd_alias_list_t *aliasCopy, const char *name )
{
	int hash;
	snd_alias_list_t *aliasList;
	const char *aliasName;
	char *nameCopy;

	hash = Com_SoundAliasHash(name);
	for ( aliasList = g_sa.hash[hash]; aliasList; aliasList = aliasList->next )
	{
		if ( I_stricmp(name, aliasList->aliasName) )
		{
			continue;
		}
		aliasName = aliasList->aliasName;
		*aliasList = *aliasCopy;
		aliasList->aliasName = aliasName;
		return;
	}
	aliasList = (snd_alias_list_t *)Com_AllocSoundMemory(sizeof(snd_alias_list_t), "Com_DuplicateSoundAlias", 13);
	*aliasList = *aliasCopy;
	nameCopy = (char *)Com_AllocSoundMemory(strlen(name) + 1, "Com_DuplicateSoundAlias", 13);
	strcpy(nameCopy, name);
	aliasList->aliasName = nameCopy;
	aliasList->next = g_sa.hash[hash];
	g_sa.hash[hash] = aliasList;
}

const char *Com_GetSoundFileName( const snd_alias_t *alias )
{
	return alias->soundFile->soundName;
}

void *Com_GetSoundFileMem( const snd_alias_t *alias )
{
	return alias->soundFile->fileMem;
}

void Com_UnloadSoundAliasSounds( snd_alias_system_t system )
{
	snd_alias_t *head;
	int i;
	int j;

	SND_StopSounds(SND_STOP_ALL);
	head = g_sa.aliasInfo[system].head;
	for ( i = 0; i < g_sa.aliasInfo[system].count; i++ )
	{
		if ( ((head[i].flags >> 5) & 3) != SAT_LOADED && ((head[i].flags >> 5) & 3) != SAT_PRIMED )
		{
			continue;
		}
		for ( j = 0; j < i; j++ )
		{
			if ( head[j].soundFile->soundName == head[i].soundFile->soundName && ((head[j].flags >> 5) & 3) == ((head[i].flags >> 5) & 3) )
			{
				break;
			}
		}
		head[i].soundFile->fileMem = NULL;
	}
}

void Com_InitVolumeFalloffCurves()
{
	char **fileList;
	int fileCount;
	int i;
	char *name;

	memset(g_sa.volumeFalloffCurves, 0, sizeof(g_sa.volumeFalloffCurves));
	Com_InitDefaultSoundAliasVolumeFalloffCurve(&g_sa.volumeFalloffCurves[0]);
	fileList = FS_ListFiles("soundaliases", "vfcurve", FS_LIST_PURE_ONLY, &fileCount, 10);
	if ( fileCount > MAX_VOLUMEFALLOFFCURVES - 1 )
	{
		Com_Error(ERR_DROP, "\x15Snd_Alias Curve initialization: '.vfcurve' file count (%d) exceeds maximum (%d)", fileCount, MAX_VOLUMEFALLOFFCURVES - 1);
	}
	for ( i = 0; i < fileCount; i++ )
	{
		name = g_sa.volumeFalloffCurveNames[i + 1];
		I_strncpyz(name, fileList[i], strlen(fileList[i]) - 7);
		if ( !Com_LoadVolumeFalloffCurve(name, &g_sa.volumeFalloffCurves[i + 1]) )
		{
			Com_Error(ERR_FATAL, "\x15" "Failed to load sndcurve file '%s'", fileList[i]);
		}
	}
	FS_FreeFileList(fileList, 10);
	g_sa.curvesInitialized = 1;
}

void Com_LoadSoundAliases( const char *loadspec, const char *loadspecCurGame, snd_alias_system_t system )
{
	char **fileList;
	int fileCount;
	int i;
	char mapName[64];
	int tempMemory;
	int missing;

	if ( !g_sa.curvesInitialized )
	{
		Com_InitVolumeFalloffCurves();
	}
	if ( !strnicmp(loadspec, "maps/mp/", 8) )
	{
		Com_StripExtension(loadspec + 8, mapName);
	}
	else if ( !strnicmp(loadspec, "maps/", 5) )
	{
		Com_StripExtension(loadspec + 5, mapName);
	}
	else
	{
		strcpy(mapName, loadspec);
	}
	strlwr(mapName);
	if ( system != SASYS_CGAME || !com_sv_running->current.boolean )
	{
		fileList = FS_ListFiles("soundaliases", "csv", FS_LIST_PURE_ONLY, &fileCount, 10);
		if ( !fileCount )
		{
			Com_Printf("WARNING: can't find any sound alias files (soundaliases/*.csv)\n");
			return;
		}
		tempMemory = Hunk_HideTempMemory();
		Com_InitSoundAlias();
		for ( i = 0; i < fileCount; i++ )
		{
			Com_LoadSoundAliasFile(mapName, loadspecCurGame, fileList[i]);
		}
		Com_MakeSoundAliasesPermanent(&g_sa.aliasInfo[system], &g_sa.soundFileInfo[system]);
		Hunk_ClearTempMemoryInternal();
		Hunk_ShowTempMemory(tempMemory);
		FS_FreeFileList(fileList, 10);
	}
	else
	{
		g_sa.aliasInfo[SASYS_CGAME] = g_sa.aliasInfo[SASYS_GAME];
		g_sa.soundFileInfo[SASYS_CGAME] = g_sa.soundFileInfo[SASYS_GAME];
	}
	if ( (system == SASYS_CGAME || system == SASYS_UI) && !g_sa.initialized[SASYS_CGAME] && !g_sa.initialized[SASYS_UI] )
	{
		Cmd_AddCommand("snd_list", Com_SoundList_f);
	}
	g_sa.initialized[system] = 1;
	if ( system == SASYS_UI || system == SASYS_CGAME )
	{
		missing = Com_LoadSoundAliasSounds(&g_sa.soundFileInfo[system]);
		if ( missing && snd_errorOnMissing->current.boolean )
		{
			Com_Error((errorParm_t)(system != SASYS_UI), va("\x15%i sound file(s) are missing or in a bad format\n", missing));
		}
	}
}

void Com_UnloadSoundAliases( snd_alias_system_t system )
{
	if ( !g_sa.initialized[system] )
	{
		return;
	}
	if ( system != SASYS_GAME )
	{
		Com_UnloadSoundAliasSounds(system);
	}
	if ( g_sa.aliasInfo[system].head )
	{
		g_sa.aliasInfo[system].head = NULL;
		g_sa.aliasInfo[system].count = 0;
		memset(g_sa.hash, 0, sizeof(g_sa.hash));
	}
	g_sa.initialized[system] = 0;
	if ( (system == SASYS_CGAME || system == SASYS_UI) && !g_sa.initialized[SASYS_CGAME] && !g_sa.initialized[SASYS_UI] )
	{
		Cmd_RemoveCommand("snd_list");
	}
}

const char *Com_GetSoundAliasName( const char *name )
{
	snd_alias_list_t *aliasList;

	aliasList = Com_FindSoundAlias(name);
	if ( !aliasList )
	{
		return NULL;
	}
	return aliasList->aliasName;
}

int Com_GetSoundAliasRandomSeed()
{
	return g_sa.randSeed;
}

// retail 0x42cd10, unreferenced; name unknown
void Com_SetSoundAliasRandomSeed_42cd10(int seed)
{
	g_sa.randSeed = seed;
}

void Com_SetSoundAliasRandomSeed( int seed )
{
	g_sa.randSeed = seed;
}

static inline int Com_SoundAliasRandom()
{
	g_sa.randSeed = g_sa.randSeed * 214013 + 2531011;
	return (g_sa.randSeed >> 16) & 0x7fff;
}

snd_alias_t *Com_PickSoundAliasFromList( snd_alias_list_t *aliasList )
{
	snd_alias_t *head;
	snd_alias_t *alias;
	snd_alias_t *select;
	float totalProb;
	int maxSequence;
	int i;

	if ( !aliasList )
	{
		return NULL;
	}
	head = aliasList->head;
	select = head;
	totalProb = head->probability;
	maxSequence = head->sequence;
	alias = head;
	i = 0;
	while ( ++i != aliasList->count )
	{
		alias++;
		totalProb += alias->probability;
		if ( Com_SoundAliasRandom() * totalProb < alias->probability * 32768.0f )
		{
			select = alias;
		}
		if ( maxSequence < alias->sequence )
		{
			maxSequence = alias->sequence;
		}
	}
	if ( aliasList->count > 2 && maxSequence == select->sequence )
	{
		totalProb = 0;
		alias = head;
		for ( i = 0; i < aliasList->count; alias++, i++ )
		{
			if ( maxSequence == alias->sequence )
			{
				continue;
			}
			totalProb += alias->probability;
			if ( Com_SoundAliasRandom() * totalProb < alias->probability * 32768.0f )
			{
				select = alias;
			}
		}
	}
	select->sequence = maxSequence + 1;
	return select;
}

// retail 0x42ce60, unreferenced twin of Com_PickSoundAlias; name unknown
snd_alias_t *Com_PickSoundAlias_42ce60( const char *aliasName )
{
	snd_alias_list_t *aliasList;

	aliasList = Com_FindSoundAlias(aliasName);
	return Com_PickSoundAliasFromList(aliasList);
}

SndCurve *Com_RegisterSoundAliasVolumeFalloffCurve( const char *filename, const char *sourceFile )
{
	int curveIndex;

	for ( curveIndex = 0; curveIndex < MAX_VOLUMEFALLOFFCURVES; curveIndex++ )
	{
		if ( g_sa.volumeFalloffCurves[curveIndex].filename && !stricmp(filename, g_sa.volumeFalloffCurves[curveIndex].filename) )
		{
			return &g_sa.volumeFalloffCurves[curveIndex];
		}
	}
	Com_Error(ERR_DROP, "\x15" "Sound alias file %s: Volume Falloff Curve '%s' not found.", sourceFile, filename);
	return NULL;
}

SndCurve *Com_GetDefaultSoundAliasVolumeFalloffCurve()
{
	return g_sa.volumeFalloffCurves;
}

void *Com_AllocateTempSoundMemory( int size, const char *name )
{
	return Hunk_AllocateTempMemoryInternal(size);
}

void *Com_AllocSoundMemory( int size, const char *name, int type )
{
	return Hunk_AllocInternal(size);
}

