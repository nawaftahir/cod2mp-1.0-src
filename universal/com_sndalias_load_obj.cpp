#include "../qcommon/qcommon.h"
#include "com_sndalias.h"

struct VolumeModGroup
{
	char name[64];
	float value;
};

struct saLoadObjGlob_t
{
	snd_alias_build_t *tempAliases;
	int tempAliasCount;
	VolumeModGroup volumeModGroups[MAX_VOLUMEMODGROUPS];
	bool volumeModGroupsInitialized;
	bool refreshVolumeModGroupsCommandInitialized;
};

static const char *snd_aliasFieldNames[SA_NUMFIELDS] =
{
	NULL,
	"name",
	"sequence",
	"file",
	"subtitle",
	"vol_min",
	"vol_max",
	"vol_mod",
	"pitch_min",
	"pitch_max",
	"dist_min",
	"dist_max",
	"channel",
	"type",
	"loop",
	"probability",
	"loadspec",
	"masterslave",
	"secondaryaliasname",
	"volumefalloffcurve",
	"startdelay",
	"speakermap",
	"reverb",
	"lfe percentage"
};

static const char *snd_channelNames[SND_CHANNEL_COUNT] =
{
	"auto",
	"auto2d",
	"menu",
	"weapon",
	"voice",
	"item",
	"body",
	"local",
	"music",
	"announcer",
	"shellshock"
};

saLoadObjGlob_t saLoadObjGlob;

void SND_LoadSoundFile( SoundFile *soundFile );
extern const dvar_t *snd_touchStreamFilesOnLoad;
extern dvar_t *fs_copyfiles;

static char com_subtitleReference[MAX_STRING_CHARS];

float Com_GetVolumeModValue( const char *name, const char *sourceFile )
{
	int i;

	i = 0;
	while ( i < MAX_VOLUMEMODGROUPS )
	{
		if ( !strcasecmp(name, saLoadObjGlob.volumeModGroups[i].name) )
		{
			return saLoadObjGlob.volumeModGroups[i].value;
		}
		i++;
	}
	Com_Error(ERR_DROP, "\x15Sound alias file %s: Volume Mod Group '%s' not found.", sourceFile, name);
	return 0.0f;
}

void Com_InitBuildSoundAlias( snd_alias_build_t *alias, const char *sourceFile, const char *loadspec )
{
	strcpy(alias->sourceFile, sourceFile);
	alias->aliasName[0] = 0;
	alias->secondaryAliasName[0] = 0;
	alias->sequence = 0;
	alias->soundFile[0] = 0;
	alias->subtitleText = NULL;
	alias->volMin = 1.0f;
	alias->volMax = 1.0f;
	alias->volMod = 1.0f;
	alias->pitchMin = 1.0f;
	alias->pitchMax = 1.0f;
	alias->distMin = 120.0f;
	alias->distMax = 0.0f;
	alias->channel = 0;
	alias->type = SAT_LOADED;
	alias->looping = false;
	alias->probability = 1.0f;
	alias->lfePercentage = 0.0f;
	alias->error = false;
	alias->keep = strcmp(loadspec, "menu") != 0;
	alias->master = false;
	alias->slave = false;
	alias->fullDryLevel = false;
	alias->noWetLevel = false;
	alias->slavePercentage = 1.0f;
	alias->startDelay = 0;
	alias->volumeFalloffCurve = Com_GetDefaultSoundAliasVolumeFalloffCurve();
	alias->next = NULL;
}

qboolean Com_IsValidAliasName( const char *name )
{
	if ( *name < ' ' || !isalnum(*name) && *name != '_' )
	{
		return qfalse;
	}
	name++;
	while ( *name )
	{
		if ( *name < ' ' || !isalnum(*name) && *name != '_' )
		{
			return qfalse;
		}
		name++;
	}
	return qtrue;
}

void Com_LoadSoundAliasChannel( const char *token, const char *sourceFile, snd_alias_build_t *alias )
{
	int i;
	int len;
	char channelList[16384];

	i = 0;
	while ( i < SND_CHANNEL_COUNT )
	{
		if ( !I_stricmp(token, snd_channelNames[i]) )
		{
			alias->channel = i;
			return;
		}
		i++;
	}
	len = 0;
	i = 0;
	while ( i < SND_CHANNEL_COUNT )
	{
		len += sprintf(&channelList[len], "%s", snd_channelNames[i]);
		if ( i < SND_CHANNEL_COUNT - 2 )
		{
			len += sprintf(&channelList[len], ", ");
		}
		else if ( i == SND_CHANNEL_COUNT - 2 )
		{
			len += sprintf(&channelList[len], " or ");
		}
		i++;
	}
	Com_Printf("^1ERROR: Sound alias file %s: Unknown sound channel '%s'; should be %s\n", sourceFile, token, channelList);
	alias->error = true;
}

void Com_LoadSoundAliasType( const char *token, const char *sourceFile, snd_alias_build_t *alias )
{
	if ( !I_stricmp(token, "streamed") )
	{
		alias->type = SAT_STREAMED;
		return;
	}

	if ( !I_stricmp(token, "primed") )
	{
		alias->type = SAT_STREAMED;
		return;
	}

	if ( !I_stricmp(token, "loaded") )
	{
		alias->type = SAT_LOADED;
		return;
	}

	Com_Printf("^1ERROR: Sound alias file %s: Unknown sound type '%s'; should be primed, streamed or loaded\n", sourceFile, token);
	alias->error = true;
}

void Com_LoadSoundAliasLooping( const char *token, const char *sourceFile, snd_alias_build_t *alias )
{
	if ( !I_stricmp(token, "looping") )
	{
		alias->looping = true;
	}
	else if ( !I_stricmp(token, "nonlooping") )
	{
		alias->looping = false;
	}
	else
	{
		Com_Printf("^1ERROR: Sound alias file %s: Unknown sound looping type '%s'; should be looping or nonlooping\n", sourceFile, token);
		alias->error = true;
	}
}

// A leading '!' inverts the loadspec; "menu" aliases always load.
bool Com_SoundAliasLoadspecKeep( const char *loadspec, const char *loadspecCurGame, const char *token, const char *sourceFile )
{
	char *found;
	char *text;
	char spec[16384];
	unsigned int loadspecLen;
	bool keep;

	loadspecLen = strlen(loadspec);
	spec[sizeof(spec) - 1] = 0;
	strncpy(spec, token, sizeof(spec));
	if ( spec[sizeof(spec) - 1] )
	{
		Com_Printf("^1ERROR: Sound alias file %s: loadspec is > %i characters\n", sourceFile, sizeof(spec) - 1);
		return false;
	}
	strlwr(spec);
	text = spec;
	keep = *text != '!' || !strcmp(loadspec, "menu");
	while ( 1 )
	{
		found = strstr(text, loadspec);
		if ( !found )
		{
			if ( strcmp(loadspec, "menu") && !strcmp(text, loadspecCurGame) )
			{
				return keep;
			}
			return !keep;
		}
		if ( (found == spec || *(found - 1) <= '!') && found[loadspecLen] <= ' ' )
		{
			return keep;
		}
		text = found + 1;
	}
}

void Com_LoadSoundAliasMasterSlave( const char *token, const char *sourceFile, snd_alias_build_t *alias )
{
	if ( !strcasecmp(token, "master") )
	{
		alias->master = true;
		alias->slave = false;
	}
	else
	{
		alias->master = false;
		alias->slave = true;
		alias->slavePercentage = atof(token);
		if ( alias->slavePercentage < 0.0f || alias->slavePercentage > 1.0f )
		{
			Com_Printf("^1ERROR: Sound alias file %s: SlavePercentage'%f' is not within the range of '%f'-'%f'.\n", sourceFile, alias->slavePercentage, 0.0f, 1.0f);
			alias->error = true;
		}
	}
}

void Com_LoadSoundAliasReverb( const char *token, snd_alias_build_t *alias )
{
	if ( strstr(token, "fulldrylevel") )
	{
		alias->fullDryLevel = true;
	}
	if ( strstr(token, "nowetlevel") )
	{
		alias->noWetLevel = true;
	}
}

void Com_LoadSoundAliasField( const char *loadspec, const char *loadspecCurGame, const char *sourceFile, const char *token, int field, char *fieldSet, snd_alias_build_t *alias )
{
	unsigned int i;
	unsigned int len;
	float lfePercentage;

	if ( !field )
	{
		return;
	}
	if ( fieldSet[field] )
	{
		Com_Printf("^1ERROR: Sound alias file %s: Duplicate entries for the '%s' column\n", sourceFile, snd_aliasFieldNames[field]);
		alias->error = true;
		return;
	}
	fieldSet[field] = 1;
	switch ( field )
	{
	case SA_NAME:
		if ( strlen(token) >= 63 )
		{
			Com_Printf("^1ERROR: Sound alias file %s: Alias name '%s' is longer than %i characters\n", sourceFile, token, 63);
			alias->error = true;
		}
		else if ( !Com_IsValidAliasName(token) )
		{
			Com_Printf("^1ERROR: Sound alias file %s: Alias name '%s' is invalid\n", sourceFile, token);
			alias->error = true;
		}
		else
		{
			strcpy(alias->aliasName, token);
		}
		break;
	case SA_SECONDARYALIASNAME:
		if ( strlen(token) >= 63 )
		{
			Com_Printf("^1ERROR: Sound alias file %s: Secondary Alias name '%s' is longer than %i characters\n", sourceFile, token, 63);
			alias->error = true;
		}
		else if ( !Com_IsValidAliasName(token) )
		{
			Com_Printf("^1ERROR: Sound alias file %s: Secondary Alias name '%s' is invalid\n", sourceFile, token);
			alias->error = true;
		}
		else
		{
			strcpy(alias->secondaryAliasName, token);
		}
		break;
	case SA_SEQUENCE:
		alias->sequence = atoi(token);
		break;
	case SA_FILE:
		if ( strlen(token) >= 63 )
		{
			Com_Printf("^1ERROR: Sound alias file %s: Sound file '%s' is longer than %i characters\n", sourceFile, token, 63);
			alias->error = true;
		}
		else
		{
			strcpy(alias->soundFile, token);
		}
		break;
	case SA_SUBTITLE:
		for ( i = 0; token[i]; i++ )
		{
			if ( token[i] < 0 )
			{
				Com_Printf("^1ERROR: Sound alias file %s: Subtitle '%s' has invalid character '%c' ascii %i\n", sourceFile, token, token[i], (unsigned char)token[i]);
				alias->error = true;
				return;
			}
		}
		len = i;
		alias->subtitleText = (char *)Hunk_AllocateTempMemoryInternal(len + 1);
		memcpy(alias->subtitleText, token, len);
		alias->subtitleText[len] = 0;
		break;
	case SA_VOL_MIN:
		alias->volMin = atof(token);
		if ( alias->volMin < 0.0f || alias->volMin > 1.0f )
		{
			Com_Printf("^1ERROR: Sound alias file %s: MinVolume '%f' is not within the range of '%f'-'%f'.\n", sourceFile, alias->volMin, 0.0, 1.0);
			alias->error = true;
		}
		else if ( !fieldSet[SA_VOL_MAX] )
		{
			alias->volMax = alias->volMin;
		}
		break;
	case SA_VOL_MAX:
		alias->volMax = atof(token);
		if ( alias->volMax < 0.0f || alias->volMax > 1.0f )
		{
			Com_Printf("^1ERROR: Sound alias file %s: MaxVolume '%f' is not within the range of '%f'-'%f'.\n", sourceFile, alias->volMax, 0.0, 1.0);
			alias->error = true;
		}
		break;
	case SA_VOL_MOD:
		alias->volMod = Com_GetVolumeModValue(token, sourceFile);
		break;
	case SA_PITCH_MIN:
		alias->pitchMin = atof(token);
		if ( !fieldSet[SA_PITCH_MAX] )
		{
			alias->pitchMax = alias->pitchMin;
		}
		break;
	case SA_PITCH_MAX:
		alias->pitchMax = atof(token);
		break;
	case SA_DIST_MIN:
		alias->distMin = atof(token);
		break;
	case SA_DIST_MAX:
		alias->distMax = atof(token);
		break;
	case SA_CHANNEL:
		Com_LoadSoundAliasChannel(token, sourceFile, alias);
		break;
	case SA_TYPE:
		Com_LoadSoundAliasType(token, sourceFile, alias);
		break;
	case SA_LOOP:
		Com_LoadSoundAliasLooping(token, sourceFile, alias);
		break;
	case SA_PROBABILITY:
		alias->probability = atof(token);
		break;
	case SA_LOADSPEC:
		alias->keep = Com_SoundAliasLoadspecKeep(loadspec, loadspecCurGame, token, sourceFile);
		break;
	case SA_MASTERSLAVE:
		Com_LoadSoundAliasMasterSlave(token, sourceFile, alias);
		break;
	case SA_VOLUMEFALLOFFCURVE:
		alias->volumeFalloffCurve = Com_RegisterSoundAliasVolumeFalloffCurve(token, sourceFile);
		break;
	case SA_STARTDELAY:
		alias->startDelay = atoi(token);
		break;
	case SA_REVERB:
		Com_LoadSoundAliasReverb(token, alias);
		break;
	case SA_LFEPERCENTAGE:
		alias->lfePercentage = I_fclamp(lfePercentage = atof(token), 0.0f, 1.0f);
		break;
	}
}

bool Com_ValidateSoundAlias( snd_alias_build_t *alias )
{
	float swap;
	float value;

	if ( alias->pitchMin > alias->pitchMax )
	{
		swap = alias->pitchMax;
		alias->pitchMax = alias->pitchMin;
		alias->pitchMin = swap;
	}
	if ( alias->pitchMin <= 0.0f )
	{
		Com_Printf("^1ERROR: sound alias '%s' has pitch_min %g <= 0\n", alias->aliasName, alias->pitchMin);
		return false;
	}
	if ( alias->volMin > alias->volMax )
	{
		swap = alias->volMax;
		alias->volMax = alias->volMin;
		alias->volMin = swap;
	}
	if ( alias->volMin < 0.0f )
	{
		Com_Printf("^1ERROR: sound alias '%s' has vol_min %g < 0\n", alias->aliasName, alias->volMin);
		return false;
	}
	if ( alias->distMax == 0.0f )
	{
		alias->distMax = alias->distMin * 5.0f;
	}
	if ( alias->distMax < alias->distMin )
	{
		Com_Printf("^1ERROR: sound alias '%s' has dist_min %g <= dist_max %g\n", alias->aliasName, alias->distMin, alias->distMax);
		return false;
	}
	if ( alias->distMin <= 0.0f )
	{
		Com_Printf("^1ERROR: sound alias '%s' has dist_min %g <= 0\n", alias->aliasName, alias->distMin);
		return false;
	}
	if ( alias->volMod != 1.0f )
	{
		value = alias->volMin * alias->volMod;
		if ( value < 0.0f )
		{
			value = 0.0f;
		}
		else if ( value > 1.0f )
		{
			value = 1.0f;
		}
		alias->volMin = value;
		value = alias->volMax * alias->volMod;
		if ( value < 0.0f )
		{
			value = 0.0f;
		}
		else if ( value > 1.0f )
		{
			value = 1.0f;
		}
		alias->volMax = value;
	}
	return true;
}

void Com_AddBuildSoundAlias( snd_alias_build_t *alias )
{
	snd_alias_build_t *newAlias;

	newAlias = (snd_alias_build_t *)Com_AllocateTempSoundMemory(sizeof(snd_alias_build_t), "Com_AddBuildSoundAlias");
	*newAlias = *alias;
	newAlias->next = saLoadObjGlob.tempAliases;
	saLoadObjGlob.tempAliases = newAlias;
	saLoadObjGlob.tempAliasCount++;
}

void Com_InitSoundAlias()
{
	saLoadObjGlob.tempAliases = NULL;
	saLoadObjGlob.tempAliasCount = 0;
}

void Com_LoadVolumeModGroups( VolumeModGroup *groups )
{
	char path[64];
	int fileSize;
	int f;
	char buffer[8192];
	const char *data;
	char *token;
	const char *header;
	unsigned int headerLen;
	int i;

	header = "VOLUMEMODGROUPS";
	headerLen = strlen(header);
	strcpy(path, "soundaliases/volumemodgroups.def");
	fileSize = FS_FOpenFileRead(path, &f, 1);
	if ( fileSize < 0 )
	{
		Com_Error(ERR_DROP, "ERROR: Could not find '%s'\n", path);
		return;
	}
	if ( !fileSize )
	{
		FS_FCloseFile(f);
		Com_Error(ERR_DROP, "ERROR: '%s' is empty\n", path);
		return;
	}
	FS_Read(buffer, headerLen, f);
	buffer[headerLen] = 0;
	if ( strncmp(buffer, header, headerLen) )
	{
		FS_FCloseFile(f);
		Com_Error(ERR_DROP, "ERROR: \"%s\" does not appear to be a volumemodgroups file\n", path);
		return;
	}
	if ( (int)(fileSize - headerLen) >= (int)sizeof(buffer) )
	{
		FS_FCloseFile(f);
		Com_Error(ERR_DROP, "ERROR: \"%s\" Is too long of a volumemodgroups file to parse\n", path);
		return;
	}
	memset(buffer, 0, sizeof(buffer));
	FS_Read(buffer, fileSize - headerLen, f);
	buffer[fileSize - headerLen] = 0;
	FS_FCloseFile(f);
	Com_BeginParseSession(path);
	data = buffer;
	for ( i = 0; ; i++ )
	{
		token = Com_ParseInl(&data);
		if ( !*token || *token == '}' )
		{
			break;
		}
		if ( i >= MAX_VOLUMEMODGROUPS )
		{
			Com_EndParseSession();
			Com_Error(ERR_DROP, "ERROR: volumemodgroups parse failure on file \"%s\": groups parsed (%d) is greater than or equal to maxGroups(%d)\n", path, i, MAX_VOLUMEMODGROUPS);
			return;
		}
		strcpy(groups[i].name, token);
		token = Com_ParseInl(&data);
		if ( !*token || *token == '}' )
		{
			Com_EndParseSession();
			Com_Error(ERR_DROP, "ERROR: volumemodgroups parse failure on file \"%s\": groupname '%s' missing a matching value\n", path, groups[i].name);
			return;
		}
		groups[i].value = atof(token);
	}
	Com_EndParseSession();
}

void Com_RefreshVolumeModGroups_f()
{
	saLoadObjGlob.volumeModGroupsInitialized = false;
}

void Com_LoadSoundAliasFile( const char *loadspec, const char *loadspecCurGame, const char *sourceFile )
{
	char *buffer;
	const char *data;
	char *token;
	int i;
	int columnCount;
	int columnFields[256];
	snd_alias_build_t alias;
	char path[64];
	char fieldSet[SA_NUMFIELDS];
	int hasName;
	int hasFile;

	Com_sprintf(path, sizeof(path), "soundaliases/%s", sourceFile);
	if ( FS_ReadFile(path, (void **)&buffer) < 0 )
	{
		return;
	}
	if ( !saLoadObjGlob.volumeModGroupsInitialized )
	{
		if ( !saLoadObjGlob.refreshVolumeModGroupsCommandInitialized )
		{
			Cmd_AddCommand("snd_refreshVolumeModGroups", Com_RefreshVolumeModGroups_f);
			saLoadObjGlob.refreshVolumeModGroupsCommandInitialized = true;
		}
		Com_LoadVolumeModGroups(saLoadObjGlob.volumeModGroups);
		saLoadObjGlob.volumeModGroupsInitialized = true;
	}
	Com_BeginParseSession(path);
	Com_SetCSV(qtrue);
	data = buffer;
	columnCount = 0;
	while ( 1 )
	{
		token = Com_ParseInl(&data);
		if ( !data )
		{
			break;
		}
		if ( !*token || *token == '#' )
		{
			Com_SkipRestOfLineInl(&data);
			continue;
		}
		if ( !columnCount )
		{
			hasName = 0;
			hasFile = 0;
			while ( 1 )
			{
				columnFields[columnCount] = SA_INVALID;
				for ( i = SA_NAME; i < SA_NUMFIELDS; i++ )
				{
					if ( !I_stricmp(snd_aliasFieldNames[i], token) )
					{
						columnFields[columnCount] = i;
						if ( i == SA_NAME )
						{
							hasName = 1;
						}
						else if ( i == SA_FILE )
						{
							hasFile = 1;
						}
						break;
					}
				}
				if ( ++columnCount == 256 )
				{
					break;
				}
				if ( data && *data != '\n' )
				{
					token = Com_ParseOnLineInl(&data);
					continue;
				}
				break;
			}
			if ( !hasName || !hasFile )
			{
				Com_Printf("^1ERROR: Sound alias file %s: missing 'name' and/or 'file' columns\n", sourceFile);
				Com_EndParseSessionInl();
				return;
			}
		}
		else
		{
			memset(fieldSet, 0, sizeof(fieldSet));
			Com_InitBuildSoundAlias(&alias, sourceFile, loadspec);
			i = 0;
			while ( 1 )
			{
				if ( *token )
				{
					Com_LoadSoundAliasField(loadspec, loadspecCurGame, sourceFile, token, columnFields[i], fieldSet, &alias);
				}
				if ( ++i == columnCount )
				{
					break;
				}
				token = Com_ParseOnLineInl(&data);
			}
			if ( !fieldSet[SA_NAME] || !fieldSet[SA_FILE] )
			{
				Com_Printf("^1ERROR: Sound alias file %s: alias entry missing name and/or file\n", sourceFile);
				Com_EndParseSessionInl();
				return;
			}
			if ( alias.keep && !alias.error && Com_ValidateSoundAlias(&alias) )
			{
				Com_AddBuildSoundAlias(&alias);
			}
		}
		Com_SkipRestOfLineInl(&data);
	}
	Com_EndParseSessionInl();
}

int AliasNameCompare( snd_alias_build_t *alias0, snd_alias_build_t *alias1 )
{
	return I_stricmp(alias0->aliasName, alias1->aliasName);
}

snd_alias_build_t *Com_SortTempSoundAliases_r( snd_alias_build_t *aliasList, int *aliasCount, int (*compare)( snd_alias_build_t *, snd_alias_build_t * ), bool removeDuplicates )
{
	snd_alias_build_t *front;
	snd_alias_build_t *back;
	snd_alias_build_t **tail;
	int frontCount;
	int backCount;
	int result;

	if ( *aliasCount == 1 )
	{
		aliasList->next = NULL;
		return aliasList;
	}
	frontCount = *aliasCount / 2;
	backCount = *aliasCount - frontCount;
	result = 0;
	back = aliasList;
	while ( result < frontCount )
	{
		result++;
		back = back->next;
	}
	front = Com_SortTempSoundAliases_r(aliasList, &frontCount, compare, removeDuplicates);
	back = Com_SortTempSoundAliases_r(back, &backCount, compare, removeDuplicates);
	*aliasCount = 0;
	aliasList = NULL;
	tail = &aliasList;
	while ( frontCount && backCount )
	{
		result = compare(front, back);
		if ( removeDuplicates && !result )
		{
			result = front->sequence - back->sequence;
			if ( !result )
			{
				result = I_stricmp(front->sourceFile, back->sourceFile);
				if ( !result )
				{
					Com_Printf("^1ERROR: sound alias file %s: duplicate alias '%s'\n", front->sourceFile, front->aliasName);
					front = front->next;
					frontCount--;
					back = back->next;
					backCount--;
					continue;
				}
				if ( result < 0 )
				{
					front = front->next;
					frontCount--;
				}
				else
				{
					back = back->next;
					backCount--;
				}
				continue;
			}
		}
		if ( result <= 0 )
		{
			*tail = front;
			front = front->next;
			frontCount--;
		}
		else
		{
			*tail = back;
			back = back->next;
			backCount--;
		}
		++*aliasCount;
		tail = &(*tail)->next;
	}
	if ( frontCount )
	{
		*tail = front;
		*aliasCount += frontCount;
	}
	else
	{
		*tail = back;
		*aliasCount += backCount;
	}
	return aliasList;
}

int FileNameTypeCompare( snd_alias_build_t *alias0, snd_alias_build_t *alias1 )
{
	int nameCompare;
	int typeCompare;

	nameCompare = I_stricmp(alias0->soundFile, alias1->soundFile);
	if ( !nameCompare )
	{
		typeCompare = alias0->type - alias1->type;
		if ( !typeCompare )
		{
			return AliasNameCompare(alias0, alias1);
		}
		return typeCompare;
	}
	return nameCompare;
}

void Com_WarnSoundFileTypeMismatch( snd_alias_build_t *alias, snd_alias_build_t *other )
{
	const char *typeName;
	const char *otherTypeName;
	const char *streamed = "streamed";
	const char *primed = "primed";
	const char *loaded = "loaded";

	switch ( alias->type )
	{
	case SAT_STREAMED:
		typeName = streamed;
		break;
	case SAT_PRIMED:
		typeName = primed;
		break;
	case SAT_LOADED:
	default:
		typeName = loaded;
		break;
	}
	switch ( other->type )
	{
	case SAT_STREAMED:
		otherTypeName = streamed;
		break;
	case SAT_PRIMED:
		otherTypeName = primed;
		break;
	case SAT_LOADED:
	default:
		otherTypeName = loaded;
		break;
	}
	Com_Printf("WARNING: sound file '%s' used as %s in alias '%s' and %s in alias '%s'\n", alias->soundFile, typeName, alias->aliasName, otherTypeName, other->aliasName);
}

void Com_InitSoundFile( SoundFile *soundFile, const char *soundName, int type )
{
	soundFile->soundName = soundName;
	soundFile->fileMem = NULL;
	soundFile->type = type;
}

void Com_AddSoundAlias( snd_alias_build_t *alias, snd_alias_t *permAlias, const char *aliasName, SoundFile *soundFile, const char *subtitle )
{
	permAlias->aliasName = aliasName;
	if ( alias->secondaryAliasName[0] )
	{
		permAlias->secondaryAliasName = (const char *)Com_AllocSoundMemory(strlen(alias->secondaryAliasName) + 1, "Com_AddSoundAlias", 13);
		strcpy((char *)permAlias->secondaryAliasName, alias->secondaryAliasName);
	}
	else
	{
		permAlias->secondaryAliasName = NULL;
	}
	permAlias->soundFile = soundFile;
	permAlias->subtitle = subtitle;
	permAlias->sequence = 0;
	permAlias->volMin = alias->volMin;
	permAlias->volMax = alias->volMax;
	permAlias->pitchMin = alias->pitchMin;
	permAlias->pitchMax = alias->pitchMax;
	permAlias->distMin = alias->distMin;
	permAlias->distMax = alias->distMax;
	permAlias->flags = permAlias->flags & ~0x780 | alias->channel << 7;
	permAlias->flags = permAlias->flags & ~0x60 | alias->type << 5;
	permAlias->volumeFalloffCurve = alias->volumeFalloffCurve;
	if ( alias->looping )
	{
		permAlias->flags |= 1;
	}
	else
	{
		permAlias->flags &= ~1;
	}
	if ( alias->master )
	{
		permAlias->flags |= 2;
	}
	else
	{
		permAlias->flags &= ~2;
	}
	if ( alias->slave )
	{
		permAlias->flags |= 4;
	}
	else
	{
		permAlias->flags &= ~4;
	}
	if ( alias->fullDryLevel )
	{
		permAlias->flags |= 8;
	}
	else
	{
		permAlias->flags &= ~8;
	}
	if ( alias->noWetLevel )
	{
		permAlias->flags |= 0x10;
	}
	else
	{
		permAlias->flags &= ~0x10;
	}
	permAlias->slavePercentage = alias->slavePercentage;
	permAlias->probability = alias->probability;
	permAlias->lfePercentage = alias->lfePercentage;
	permAlias->startDelay = alias->startDelay;
}

void Com_MakeSoundAliasesPermanent( snd_alias_list_t *aliasInfo, SoundFileInfo *soundFileInfo )
{
	char *aliasName;
	char *soundName;
	char *subtitle;
	snd_alias_build_t *alias;
	snd_alias_build_t *prevAlias;
	char *strings;
	char *stringPos;
	unsigned int len;
	unsigned int stringSize;
	unsigned int sharedSize;
	SoundFile *soundFile;
	bool sameFile;
	bool sameType;
	bool sameName;
	snd_alias_t *permAlias;
	snd_alias_list_t *aliasList;
	int soundFileCount;

	soundFileInfo->count = 0;
	aliasInfo->count = 0;
	if ( !saLoadObjGlob.tempAliasCount )
	{
		return;
	}
	saLoadObjGlob.tempAliases = Com_SortTempSoundAliases_r(saLoadObjGlob.tempAliases, &saLoadObjGlob.tempAliasCount, AliasNameCompare, true);
	if ( !saLoadObjGlob.tempAliases )
	{
		return;
	}
	saLoadObjGlob.tempAliases = Com_SortTempSoundAliases_r(saLoadObjGlob.tempAliases, &saLoadObjGlob.tempAliasCount, FileNameTypeCompare, false);
	if ( !saLoadObjGlob.tempAliases )
	{
		return;
	}
	stringSize = 0;
	sharedSize = 0;
	prevAlias = NULL;
	aliasName = NULL;
	soundFileCount = 0;
	for ( alias = saLoadObjGlob.tempAliases; alias; alias = alias->next )
	{
		len = strlen(alias->soundFile) + 1;
		sameType = prevAlias && alias->type == prevAlias->type;
		sameName = aliasName && !I_stricmp(aliasName, alias->soundFile);
		sameFile = sameName && sameType;
		if ( !aliasName || !sameFile )
		{
			if ( sameName && !sameType )
			{
				Com_WarnSoundFileTypeMismatch(alias, prevAlias);
			}
			prevAlias = alias;
			aliasName = alias->soundFile;
			alias->sameSoundFile = NULL;
			stringSize += len;
			soundFileCount++;
		}
		else
		{
			sharedSize += len;
			alias->sameSoundFile = prevAlias;
		}
	}
	saLoadObjGlob.tempAliases = Com_SortTempSoundAliases_r(saLoadObjGlob.tempAliases, &saLoadObjGlob.tempAliasCount, AliasNameCompare, true);
	if ( !saLoadObjGlob.tempAliases )
	{
		return;
	}
	aliasName = NULL;
	for ( alias = saLoadObjGlob.tempAliases; alias; alias = alias->next )
	{
		len = strlen(alias->aliasName) + 1;
		if ( !aliasName || I_stricmp(aliasName, alias->aliasName) )
		{
			stringSize += len;
			aliasName = alias->aliasName;
		}
		else
		{
			sharedSize += len;
		}
		if ( alias->subtitleText )
		{
			stringSize += strlen(alias->subtitleText) + 1;
		}
	}
	aliasInfo->head = (snd_alias_t *)Com_AllocSoundMemory(sizeof(snd_alias_t) * saLoadObjGlob.tempAliasCount, "Com_MakeSoundAliasesPermanent:aliases", 13);
	soundFileInfo->files = (SoundFile *)Com_AllocSoundMemory(sizeof(SoundFile) * soundFileCount, "Com_MakeSoundAliasesPermanent:soundFiles", 13);
	stringPos = (char *)Com_AllocSoundMemory(stringSize, "Com_MakeSoundAliasesPermanent:strings", 13);
	strings = stringPos;
	aliasName = NULL;
	soundFile = NULL;
	aliasList = NULL;
	for ( alias = saLoadObjGlob.tempAliases; alias; alias = alias->next )
	{
		if ( !aliasName || I_stricmp(aliasName, alias->aliasName) )
		{
			aliasName = stringPos;
			strcpy(aliasName, alias->aliasName);
			stringPos += strlen(aliasName) + 1;
		}
		if ( alias->subtitleText )
		{
			subtitle = stringPos;
			strcpy(subtitle, alias->subtitleText);
			stringPos += strlen(subtitle) + 1;
		}
		else
		{
			subtitle = NULL;
		}
		permAlias = &aliasInfo->head[aliasInfo->count];
		if ( !aliasList || I_stricmp(aliasList->head->aliasName, aliasName) )
		{
			aliasList = (snd_alias_list_t *)Com_AllocSoundMemory(sizeof(snd_alias_list_t), "Com_MakeSoundAliasesPermanent:aliasList", 13);
			if ( !Com_AddAliasList(aliasName, aliasList) )
			{
				aliasList = NULL;
				Com_Printf("^1ERROR: alias '%s' already added - ignoring\n", aliasName);
				continue;
			}
			aliasList->aliasName = aliasName;
			aliasList->head = permAlias;
		}
		if ( alias->sameSoundFile )
		{
			soundFile = alias->sameSoundFile->permSoundFile;
		}
		else
		{
			soundName = stringPos;
			strcpy(soundName, alias->soundFile);
			stringPos += strlen(soundName) + 1;
			soundFile = &soundFileInfo->files[soundFileInfo->count];
			Com_InitSoundFile(soundFile, soundName, alias->type);
			soundFileInfo->count++;
		}
		alias->permSoundFile = soundFile;
		Com_AddSoundAlias(alias, permAlias, aliasName, soundFile, subtitle);
		aliasInfo->count++;
		aliasList->count++;
	}
}

bool Com_ParseVolumeFalloffCurve( const char *buffer, const char *filename, SndCurve *curve )
{
	const char *token;
	int knotIndex;

	Com_BeginParseSession(filename);
	token = Com_ParseInl(&buffer);
	curve->knotCount = atoi(token);
	if ( curve->knotCount < 2 )
	{
		Com_EndParseSessionInl();
		Com_Printf("^1ERROR: sndcurve parse failure on file \"%s\": knot count (%d) is less than 2\n", filename, curve->knotCount);
		return false;
	}
	if ( curve->knotCount > MAX_SNDCURVE_KNOTS )
	{
		Com_EndParseSessionInl();
		Com_Printf("^1ERROR: sndcurve parse failure on file \"%s\": knot count (%d) is greater than maxKnots (%d)\n", filename, curve->knotCount, MAX_SNDCURVE_KNOTS);
		return false;
	}
	for ( knotIndex = 0; ; knotIndex++ )
	{
		token = Com_ParseInl(&buffer);
		if ( !*token || *token == '}' )
		{
			break;
		}
		if ( knotIndex >= MAX_SNDCURVE_KNOTS )
		{
			Com_EndParseSession();
			Com_Printf("^1ERROR: sndcurve parse failure on file \"%s\": knots parsed (%d) is greater than or equal to maxKnots (%d)\n", filename, knotIndex, MAX_SNDCURVE_KNOTS);
			return false;
		}
		curve->knots[knotIndex][0] = atof(token);
		if ( curve->knots[knotIndex][0] < 0 || curve->knots[knotIndex][0] > 1 )
		{
			Com_EndParseSession();
			Com_Printf("^1ERROR: sndcurve parse failure on file \"%s\": knot x-coord '%f' is not in the range 0-1.\n", filename, curve->knots[knotIndex][0]);
			return false;
		}
		token = Com_ParseInl(&buffer);
		if ( !*token || *token == '}' )
		{
			break;
		}
		curve->knots[knotIndex][1] = atof(token);
		if ( curve->knots[knotIndex][1] < 0 || curve->knots[knotIndex][1] > 1 )
		{
			Com_EndParseSession();
			Com_Printf("^1ERROR: sndcurve parse failure on file \"%s\": knot x-coord '%f' is not in the range 0-1.\n", filename, curve->knots[knotIndex][1]);
			return false;
		}
	}
	Com_EndParseSessionInl();
	if ( knotIndex != curve->knotCount )
	{
		Com_Printf("^1ERROR: sndcurve parse failure on file \"%s\": knot count (%d) does not match knots parsed (%d).\n", filename, curve->knotCount, knotIndex);
		return false;
	}
	knotIndex--;
	if ( curve->knots[0][0] != 0 || curve->knots[0][1] != 1 || curve->knots[knotIndex][0] != 1 || curve->knots[knotIndex][1] != 0 )
	{
		curve->knots[0][0] = 0;
		curve->knots[0][1] = 1.0f;
		curve->knots[knotIndex][0] = 1.0f;
		curve->knots[knotIndex][1] = 0;
		Com_Printf("^3WARNING^7: sndcurve parse on file \"%s\": the first point must be '0.0000 1.0000' and the last point must be '1.0000 0.0000'.\nadjusting sndcurve endpoints.\n", filename);
	}
	return true;
}

bool Com_LoadVolumeFalloffCurve( const char *name, SndCurve *curve )
{
	char path[64];
	int fileSize;
	int f;
	char buffer[8192];
	const char *header;
	int headerLen;

	header = "SNDCURVE";
	headerLen = strlen(header);
	Com_sprintf(path, sizeof(path), "soundaliases/%s.vfcurve", name);
	fileSize = FS_FOpenFileRead(path, &f, 1);
	if ( fileSize < 0 )
	{
		Com_Printf("^1ERROR: Could not load sndcurve file '%s'\n", path);
		return false;
	}
	if ( !fileSize )
	{
		FS_FCloseFile(f);
		Com_Printf("^1ERROR: sndcurve file '%s' is empty\n", path);
		return false;
	}
	FS_Read(buffer, headerLen, f);
	buffer[headerLen] = 0;
	if ( strncmp(buffer, header, headerLen) )
	{
		FS_FCloseFile(f);
		Com_Printf("^1ERROR: \"%s\" does not appear to be a sndcurve file\n", path);
		return false;
	}
	if ( fileSize - headerLen >= (int)sizeof(buffer) )
	{
		FS_FCloseFile(f);
		Com_Printf("^1ERROR: \"%s\" Is too long of a sndcurve file to parse\n", path);
		return false;
	}
	memset(buffer, 0, sizeof(buffer));
	FS_Read(buffer, fileSize - headerLen, f);
	buffer[fileSize - headerLen] = 0;
	FS_FCloseFile(f);
	if ( !Com_ParseVolumeFalloffCurve(buffer, path, curve) )
	{
		return false;
	}
	curve->filename = name;
	return true;
}

int Com_LoadSoundAliasSounds( SoundFileInfo *info )
{
	int missing;
	int i;
	SoundFile *soundFile;

	missing = 0;
	for ( i = 0; i < info->count; i++ )
	{
		soundFile = &info->files[i];
		if ( soundFile->type == SAT_LOADED )
		{
			SND_LoadSoundFile(soundFile);
			if ( !soundFile->fileMem )
			{
				missing++;
			}
		}
		else
		{
			if ( snd_touchStreamFilesOnLoad->current.boolean || fs_copyfiles->current.boolean )
			{
				soundFile->isStreamFound = FS_TouchFile(va("sound/%s", soundFile->soundName));
			}
			else
			{
				soundFile->isStreamFound = 1;
			}
			if ( !soundFile->isStreamFound )
			{
				Com_Printf("^1ERROR: Streamed sound file '%s' not found\n", soundFile->soundName);
			}
		}
	}
	return missing;
}

void Com_InitDefaultSoundAliasVolumeFalloffCurve( SndCurve *curve )
{
	curve->filename = "";
	curve->knots[0][0] = 0;
	curve->knots[0][1] = 1.0f;
	curve->knots[1][0] = 1.0f;
	curve->knots[1][1] = 0;
	curve->knotCount = 2;
}

qboolean Com_SubtitleReferenceExists( const char *reference )
{
	char *buffer;
	const char *data;
	const char *token;
	qboolean found;

	found = qfalse;
	if ( I_strncmp(reference, "SUBTITLE_", 9) )
	{
		return qfalse;
	}
	if ( FS_ReadFile("soundaliases/subtitle.st", (void **)&buffer) < 0 )
	{
		Com_Printf("WARNING: Could not read local copy of StringEd file %s\n", "soundaliases/subtitle.st");
		return qfalse;
	}
	Com_BeginParseSession("soundaliases/subtitle.st");
	for ( data = buffer; ; Com_SkipRestOfLineInl(&data) )
	{
		token = Com_ParseInl(&data);
		if ( !data )
		{
			break;
		}
		if ( !strcmp(token, "REFERENCE") )
		{
			token = Com_ParseOnLineInl(&data);
			if ( !I_stricmp(reference + 9, token) )
			{
				found = qtrue;
				break;
			}
		}
	}
	Com_EndParseSessionInl();
	FS_FreeFile(buffer);
	return found;
}

const char *Com_GetSubtitleStringEdReference( const char *text )
{
	void *buffer;
	const char *data;
	char *token;

	if ( FS_ReadFile("soundaliases/subtitle.st", &buffer) < 0 )
	{
		Com_Printf("WARNING: Could not read local copy of StringEd file %s\n", "soundaliases/subtitle.st");
		return NULL;
	}
	Com_BeginParseSession("soundaliases/subtitle.st");
	data = (const char *)buffer;
	while ( 1 )
	{
		token = Com_Parse(&data);
		if ( !data )
		{
			break;
		}
		if ( !strcmp(token, "REFERENCE") )
		{
			token = Com_ParseOnLine(&data);
			strcpy(com_subtitleReference, token);
			Com_SkipRestOfLine(&data);
			do
			{
				token = Com_Parse(&data);
				if ( !data )
				{
					Com_Error(ERR_DROP, "\x15StringEd file %s has bad syntax", "soundaliases/subtitle.st");
				}
			}
			while ( strcmp(token, "LANG_ENGLISH") );
			token = Com_ParseOnLine(&data);
			if ( !I_stricmp(text, token) )
			{
				Com_EndParseSessionInl();
				FS_FreeFile(buffer);
				return com_subtitleReference;
			}
		}
		Com_SkipRestOfLine(&data);
	}
	Com_EndParseSessionInl();
	FS_FreeFile(buffer);
	return NULL;
}

void Com_WriteStringEdReferenceToFile( const char *reference, const char *subtitle, fileHandle_t f )
{
	const char *text;

	text = "REFERENCE           ";
	FS_Write(text, strlen(text), f);
	FS_Write(reference, strlen(reference), f);
	text = "\r\nLANG_ENGLISH        \"";
	FS_Write(text, strlen(text), f);
	FS_Write(subtitle, strlen(subtitle), f);
	text = "\"\r\n\r\n";
	FS_Write(text, strlen(text), f);
}

// Rewrites subtitle.st with the reference set to this subtitle, adding it if new.
void Com_WriteSubtitleToStringEd( const char *reference, const char *subtitle )
{
	char *len;
	char *buffer;
	const char *data;
	char tempOSPath[256];
	char stringEdOSPath[256];
	const char *name;
	const char *token;
	const char *copyStart;
	fileHandle_t f;
	qboolean written;
	const char *tempFile;

	written = qfalse;
	tempFile = "soundaliases/temp.st";
	name = reference + 9;
	f = FS_FOpenFileWrite(tempFile);
	if ( !f )
	{
		Com_Printf("WARNING: Could not open output file %s for writing\n", tempFile);
		return;
	}
	if ( FS_ReadFile("soundaliases/subtitle.st", (void **)&buffer) < 0 )
	{
		Com_Printf("WARNING: Could not read local copy of StringEd file %s\n", "soundaliases/subtitle.st");
		FS_FCloseFile(f);
		return;
	}
	Com_BeginParseSession("soundaliases/subtitle.st");
	data = buffer;
	copyStart = data;
	while ( 1 )
	{
		token = Com_Parse(&data);
		if ( !data )
		{
			break;
		}
		if ( !strcmp(token, "ENDMARKER") )
		{
			if ( copyStart < data )
			{
				len = (char *)(data - copyStart - 11);
				FS_Write(copyStart, (int)len, f);
			}
			break;
		}
		if ( !strcmp(token, "REFERENCE") )
		{
			token = Com_ParseOnLine(&data);
			if ( !strcmp(token, name) )
			{
				if ( copyStart < data )
				{
					len = (char *)(data - copyStart);
					FS_Write(copyStart, (int)len, f);
				}
				Com_WriteStringEdReferenceToFile(name, subtitle, f);
				written = qtrue;
				do
				{
					copyStart = data;
					token = Com_Parse(&data);
					if ( !data )
					{
						copyStart = NULL;
						goto skipLine;
					}
				}
				while ( strcmp(token, "REFERENCE") && strcmp(token, "ENDMARKER") );
				Com_UngetToken();
			}
		}
skipLine:
		Com_SkipRestOfLine(&data);
	}
	if ( !written )
	{
		Com_WriteStringEdReferenceToFile(name, subtitle, f);
	}
	Com_EndParseSessionInl();
	FS_FreeFile(buffer);
	token = "\r\nENDMARKER\r\n\r\n\r\n";
	FS_Write(token, strlen(token), f);
	FS_FCloseFile(f);
	FS_BuildOSPath(fs_basepath->current.string, fs_gamedir, tempFile, tempOSPath);
	FS_BuildOSPath(fs_basepath->current.string, fs_gamedir, "soundaliases/subtitle.st", stringEdOSPath);
	FS_CopyFile(tempOSPath, stringEdOSPath);
	FS_Remove(tempOSPath);
}

void Com_ProcessSoundAliasFileLocalization( const char *sourceFile, const char *loadspecCurGame, const char *stringEdFileName )
{
	int i;
	int columnCount;
	int len;
	int localizedCount;
	char *buffer;
	const char *data;
	char osPath[256];
	char aliasTokens[SA_NUMFIELDS][1024];
	char newReference[1024];
	char tempOSPath[256];
	char aliasOSPath[256];
	const char *token;
	const char *lineStart;
	const char *lineEnd;
	char fieldSet[SA_NUMFIELDS];
	int localize;
	fileHandle_t f;
	FILE *file;
	int columnFields[256];
	snd_alias_build_t alias;
	char path[256];
	int hasName;
	int hasFile;

	Com_sprintf(path, sizeof(path), "soundaliases/%s", sourceFile);
	FS_BuildOSPath(fs_basepath->current.string, fs_gamedir, path, osPath);
	Com_Printf("Processing sound alias file %s..\n", osPath);
	file = FS_FileOpen(osPath, "r+");
	if ( !file )
	{
		Com_Printf("WARNING: Can not write to sound alias file %s\n", osPath);
		return;
	}
	FS_FileClose(file);
	if ( FS_ReadFile(path, (void **)&buffer) < 0 )
	{
		Com_Printf("WARNING: Could not read sound alias file %s\n", path);
		return;
	}
	f = FS_FOpenFileWrite("soundaliases/temp.csv");
	if ( !f )
	{
		Com_Printf("WARNING: Could not open output file %s for writing\n", "soundaliases/temp.csv");
		return;
	}
	Com_BeginParseSession(path);
	Com_SetCSV(qtrue);
	data = buffer;
	columnCount = 0;
	localizedCount = 0;
	while ( data )
	{
		if ( *data == '\r' )
		{
			while ( *data == '\r' )
			{
				data++;
			}
		}
		if ( *data == '\n' )
		{
			data++;
			FS_Write("\r\n", 2, f);
		}
		lineStart = data;
		token = Com_Parse(&data);
		if ( !data )
		{
			break;
		}
		if ( !I_stricmp(token, "#Chateau") )
		{
			i = 0;
		}
		if ( !*token || *token == '#' )
		{
			Com_SkipRestOfLine(&data);
			if ( *lineStart == '\n' )
			{
				FS_Write("\r", 1, f);
			}
			lineEnd = data;
			FS_Write(lineStart, lineEnd - lineStart, f);
			continue;
		}
		if ( !columnCount )
		{
			hasName = 0;
			hasFile = 0;
			while ( 1 )
			{
				columnFields[columnCount] = SA_INVALID;
				for ( i = SA_NAME; i < SA_NUMFIELDS; i++ )
				{
					if ( !I_stricmp(snd_aliasFieldNames[i], token) )
					{
						columnFields[columnCount] = i;
						if ( i == SA_NAME )
						{
							hasName = 1;
						}
						else if ( i == SA_FILE )
						{
							hasFile = 1;
						}
						break;
					}
				}
				if ( ++columnCount == 256 )
				{
					break;
				}
				if ( !data || *data == '\n' )
				{
					break;
				}
				token = Com_ParseOnLine(&data);
			}
			if ( !hasName || !hasFile )
			{
				Com_Error(ERR_DROP, "\x15Sound alias file %s: missing 'name' and/or 'file' columns\n", sourceFile);
			}
			Com_SkipRestOfLine(&data);
			if ( *lineStart == '\n' )
			{
				FS_Write("\r", 1, f);
			}
			lineEnd = data;
			FS_Write(lineStart, lineEnd - lineStart, f);
		}
		else
		{
			memset(fieldSet, 0, sizeof(fieldSet));
			Com_InitBuildSoundAlias(&alias, sourceFile, "menu");
			i = 0;
			while ( 1 )
			{
				strcpy(aliasTokens[columnFields[i]], token);
				if ( *token )
				{
					Com_LoadSoundAliasField("menu", loadspecCurGame, sourceFile, token, columnFields[i], fieldSet, &alias);
				}
				if ( ++i == columnCount )
				{
					break;
				}
				token = Com_ParseOnLine(&data);
			}
			if ( !fieldSet[SA_NAME] || !fieldSet[SA_FILE] )
			{
				Com_Error(ERR_DROP, "\x15Sound alias file %s: alias entry missing name and/or file\n", sourceFile);
			}
			localize = 0;
			if ( fieldSet[SA_SUBTITLE] )
			{
				len = strlen(aliasTokens[SA_SUBTITLE]);
				for ( i = 0; i < len; i++ )
				{
					if ( !(aliasTokens[SA_SUBTITLE][i] >= 'A' && aliasTokens[SA_SUBTITLE][i] <= 'Z'
						|| aliasTokens[SA_SUBTITLE][i] >= '0' && aliasTokens[SA_SUBTITLE][i] <= '9'
						|| aliasTokens[SA_SUBTITLE][i] == '_') )
					{
						break;
					}
				}
				if ( i < len || I_strncmp(aliasTokens[SA_SUBTITLE], "SUBTITLE_", 9) || !Com_SubtitleReferenceExists(aliasTokens[SA_SUBTITLE]) )
				{
					localize = 1;
				}
			}
			if ( localize )
			{
				for ( i = 0; i < columnCount; i++ )
				{
					if ( columnFields[i] && fieldSet[columnFields[i]] )
					{
						if ( columnFields[i] == SA_SUBTITLE )
						{
							token = Com_GetSubtitleStringEdReference(aliasTokens[SA_SUBTITLE]);
							if ( token )
							{
								Com_sprintf(newReference, sizeof(newReference), "%s%s", "SUBTITLE_", token);
								token = I_strupr(newReference);
							}
							else
							{
								if ( fieldSet[SA_SEQUENCE] )
								{
									Com_sprintf(newReference, sizeof(newReference), "%s%s_%s", "SUBTITLE_", aliasTokens[SA_NAME], aliasTokens[SA_SEQUENCE]);
								}
								else
								{
									Com_sprintf(newReference, sizeof(newReference), "%s%s", "SUBTITLE_", aliasTokens[SA_NAME]);
								}
								token = I_strupr(newReference);
								Com_WriteSubtitleToStringEd(newReference, aliasTokens[SA_SUBTITLE]);
								localizedCount++;
							}
							len = strlen(token);
							FS_Write(token, len, f);
						}
						else
						{
							if ( i == columnCount - 1 )
							{
								if ( strchr(aliasTokens[columnFields[i]], ',') || strchr(aliasTokens[columnFields[i]], ' ') || strchr(aliasTokens[columnFields[i]], '\n') || strchr(aliasTokens[columnFields[i]], '\r') )
								{
									token = va("\"%s\"", aliasTokens[columnFields[i]]);
								}
								else
								{
									token = va("%s", aliasTokens[columnFields[i]]);
								}
							}
							else if ( strchr(aliasTokens[columnFields[i]], ',') || strchr(aliasTokens[columnFields[i]], ' ') || strchr(aliasTokens[columnFields[i]], '\n') || strchr(aliasTokens[columnFields[i]], '\r') )
							{
								token = va("\"%s\",", aliasTokens[columnFields[i]]);
							}
							else
							{
								token = va("%s,", aliasTokens[columnFields[i]]);
							}
							len = strlen(token);
							FS_Write(token, len, f);
						}
					}
					else if ( i != columnCount - 1 )
					{
						FS_Write(",", 1, f);
					}
				}
				FS_Write("\r\n", 2, f);
			}
			else
			{
				Com_SkipRestOfLine(&data);
				lineEnd = data;
				FS_Write(lineStart, lineEnd - lineStart, f);
				continue;
			}
			Com_SkipRestOfLine(&data);
		}
	}
	Com_EndParseSessionInl();
	FS_FCloseFile(f);
	FS_BuildOSPath(fs_basepath->current.string, fs_gamedir, "soundaliases/temp.csv", tempOSPath);
	FS_BuildOSPath(fs_basepath->current.string, fs_gamedir, path, aliasOSPath);
	if ( localizedCount )
	{
		FS_CopyFile(tempOSPath, aliasOSPath);
	}
	FS_Remove(tempOSPath);
	Com_Printf("Localized %i sound alias subtitles\n", localizedCount);
}

void Com_CopyFinalStringEdFile( const char *stringEdFileName, const char *stringEdExternalFileName )
{
	FILE *file;
	int length;
	void *buffer;

	file = FS_FileOpen(stringEdFileName, "rb");
	if ( !file )
	{
		return;
	}
	FS_FileSeek(file, 0, SEEK_END);
	length = ftell(file);
	FS_FileSeek(file, 0, SEEK_SET);
	buffer = malloc(length);
	if ( FS_FileRead(buffer, 1, length, file) != length )
	{
		Com_Error(ERR_FATAL, "\x15Short read in COM_WriteFinalStringEdFile()\n");
	}
	FS_FileClose(file);
	file = FS_FileOpen(stringEdExternalFileName, "wb");
	if ( !file )
	{
		free(buffer);
		return;
	}
	if ( FS_FileWrite(buffer, 1, length, file) != length )
	{
		Com_Error(ERR_FATAL, "\x15Short write in COM_WriteFinalStringEdFile()\n");
	}
	FS_FileClose(file);
	free(buffer);
}

void Com_WriteLocalizedSoundAliasFiles()
{
	int i;
	int fileCount;
	char **fileList;
	char externalPath[256];
	char stringEdPath[256];
	FILE *file;
	int tempMemory;

	FS_BuildOSPath(fs_homepath->current.string, "../source_data/string_resources/subtitle.st", "", externalPath);
	externalPath[strlen(externalPath) - 1] = 0;
	file = FS_FileOpen(externalPath, "r+");
	if ( !file )
	{
		Com_Printf("WARNING: Can not write to StringEd file %s\n", externalPath);
		return;
	}
	FS_FileClose(file);
	FS_BuildOSPath(fs_basepath->current.string, fs_gamedir, "soundaliases/subtitle.st", stringEdPath);
	FS_CopyFile(externalPath, stringEdPath);
	if ( !FS_FileExists("soundaliases/subtitle.st") )
	{
		Com_Printf("WARNING: Could not make local copy of StringEd file %s\n", "soundaliases/subtitle.st");
		return;
	}
	Com_Printf("Localizing sound alias subtitle text...\n");
	Com_Printf("Writing to StringEd file %s\n", externalPath);
	fileList = FS_ListFiles("soundaliases", "csv", FS_LIST_PURE_ONLY, &fileCount, 10);
	if ( !fileCount )
	{
		Com_Printf("WARNING: can't find any sound alias files (soundaliases/*.csv)\n");
		return;
	}
	tempMemory = Hunk_HideTempMemory();
	for ( i = 0; i < fileCount; i++ )
	{
		Com_ProcessSoundAliasFileLocalization(fileList[i], "all_mp", stringEdPath);
		Hunk_ClearTempMemoryInternal();
	}
	Hunk_ShowTempMemory(tempMemory);
	FS_FreeFileList(fileList, 10);
	Com_CopyFinalStringEdFile(stringEdPath, externalPath);
	FS_Remove(stringEdPath);
	Com_Printf("done\n");
}

