#pragma once

// Sound alias system: csv-defined aliases, loaded per system (ui/cgame/game).

enum snd_alias_system_t
{
	SASYS_UI = 0,
	SASYS_CGAME = 1,
	SASYS_GAME = 2,
	SASYS_COUNT = 3
};

enum snd_alias_type_t
{
	SAT_UNKNOWN = 0,
	SAT_LOADED = 1,
	SAT_STREAMED = 2,
	SAT_PRIMED = 3,
	SAT_COUNT = 4
};

enum snd_alias_members_t
{
	SA_INVALID = 0,
	SA_NAME = 1,
	SA_SEQUENCE = 2,
	SA_FILE = 3,
	SA_SUBTITLE = 4,
	SA_VOL_MIN = 5,
	SA_VOL_MAX = 6,
	SA_VOL_MOD = 7,
	SA_PITCH_MIN = 8,
	SA_PITCH_MAX = 9,
	SA_DIST_MIN = 10,
	SA_DIST_MAX = 11,
	SA_CHANNEL = 12,
	SA_TYPE = 13,
	SA_LOOP = 14,
	SA_PROBABILITY = 15,
	SA_LOADSPEC = 16,
	SA_MASTERSLAVE = 17,
	SA_SECONDARYALIASNAME = 18,
	SA_VOLUMEFALLOFFCURVE = 19,
	SA_STARTDELAY = 20,
	SA_SPEAKERMAP = 21,
	SA_REVERB = 22,
	SA_LFEPERCENTAGE = 23,
	SA_NUMFIELDS = 24
};

#define SND_CHANNEL_COUNT 11
#define MAX_VOLUMEFALLOFFCURVES 16
#define MAX_SNDCURVE_KNOTS 8
#define MAX_VOLUMEMODGROUPS 32

struct SndCurve
{
	const char *filename;
	int knotCount;
	float knots[MAX_SNDCURVE_KNOTS][2];
};

struct SoundFile
{
	const char *soundName;
	void *fileMem;
	byte isStreamFound;
	int type;
};

struct SoundFileInfo
{
	int count;
	SoundFile *files;
};

// flags: bit 0 looping, 1 master, 2 slave, 3 full dry level, 4 no wet level,
// bits 5-6 snd_alias_type_t, bits 7-10 channel
struct snd_alias_t
{
	const char *aliasName;
	const char *subtitle;
	const char *secondaryAliasName;
	SoundFile *soundFile;
	int sequence;
	float volMin;
	float volMax;
	float pitchMin;
	float pitchMax;
	float distMin;
	float distMax;
	int flags;
	float slavePercentage;
	float probability;
	float lfePercentage;
	int startDelay;
	SndCurve *volumeFalloffCurve;
};

struct snd_alias_build_t
{
	char sourceFile[64];
	char aliasName[64];
	char secondaryAliasName[64];
	char *subtitleText;
	int sequence;
	char soundFile[64];
	SoundFile *permSoundFile;
	float volMin;
	float volMax;
	float volMod;
	float pitchMin;
	float pitchMax;
	float distMin;
	float distMax;
	int channel;
	int type;
	SndCurve *volumeFalloffCurve;
	float slavePercentage;
	float probability;
	float lfePercentage;
	int startDelay;
	bool looping;
	bool master;
	bool slave;
	bool fullDryLevel;
	bool noWetLevel;
	bool error;
	byte keep;
	snd_alias_build_t *sameSoundFile;
	snd_alias_build_t *next;
};

struct g_sa_t
{
	byte initialized[SASYS_COUNT];
	int randSeed;
	snd_alias_list_t *hash[1024];
	snd_alias_list_t aliasInfo[SASYS_COUNT];
	SoundFileInfo soundFileInfo[SASYS_COUNT];
	char loadSpec[64];
	bool curvesInitialized;
	SndCurve volumeFalloffCurves[MAX_VOLUMEFALLOFFCURVES];
	char volumeFalloffCurveNames[MAX_VOLUMEFALLOFFCURVES][64];
};

extern g_sa_t g_sa;

snd_alias_list_t *Com_FindSoundAlias( const char *name );
bool Com_AddAliasList( const char *name, snd_alias_list_t *aliasList );
void Com_LoadSoundAliases( const char *loadspec, const char *loadspecCurGame, snd_alias_system_t system );
void Com_UnloadSoundAliases( snd_alias_system_t system );
snd_alias_t *Com_PickSoundAliasFromList( snd_alias_list_t *aliasList );
snd_alias_t *Com_PickSoundAlias( const char *aliasName );
SndCurve *Com_RegisterSoundAliasVolumeFalloffCurve( const char *filename, const char *sourceFile );
SndCurve *Com_GetDefaultSoundAliasVolumeFalloffCurve();
void *Com_AllocateTempSoundMemory( int size, const char *name );
void *Com_AllocSoundMemory( int size, const char *name, int type );

void Com_InitSoundAlias();
void Com_LoadSoundAliasFile( const char *loadspec, const char *loadspecCurGame, const char *sourceFile );
void Com_MakeSoundAliasesPermanent( snd_alias_list_t *aliasInfo, SoundFileInfo *soundFileInfo );
bool Com_LoadVolumeFalloffCurve( const char *name, SndCurve *curve );
void Com_InitDefaultSoundAliasVolumeFalloffCurve( SndCurve *curve );
int Com_LoadSoundAliasSounds( SoundFileInfo *info );
