#ifndef CLEAN_PC_UNIVERSAL_COM_TYPES_H
#define CLEAN_PC_UNIVERSAL_COM_TYPES_H

#include "../../cod2_fwd.h"
#include "../../cod2_defs.h"

typedef struct directory_t directory_t;
typedef struct hunkUsed_t hunkUsed_t;
typedef struct infoParm_t infoParm_t;
typedef struct ParseThreadInfo ParseThreadInfo;
typedef struct saLoadObjGlob_type saLoadObjGlob_type;
typedef struct g_sa_type g_sa_type;
typedef struct va_info_t va_info_t;

enum FsListBehavior {
    FS_LIST_PURE_ONLY = 0,
    FS_LIST_ALL = 1
};

enum snd_alias_members_t {
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

struct directory_t {
    char path[256];
    char gamedir[256];
};

struct hunkUsed_t {
    int permanent;
    int temp;
};

struct infoParm_t {
    char *name;
    int clearSolid;
    int surfaceFlags;
    int contents;
    int toolFlags;
};

struct ParseThreadInfo {
    parseInfo_t parseInfo[16];
    int parseInfoNum;
    const char *prevTokenPos;
    const char *tokenPos;
    char line[1024];
};

struct saLoadObjGlob_type {
    snd_alias_build_t *tempAliases;
    int tempAliasCount;
    VolumeModGroup volumeModGroups[32];
    Bool volumeModGroupsInitialized;
    Bool refreshVolumeModGroupsCommandInitialized;
};

struct g_sa_type {
    byte initialized[3];
    int randSeed;
    snd_alias_list_t * pHash[1024];
    snd_alias_list_t aliasInfo[3];
    SoundFileInfo soundFileInfo[3];
    char szLoadSpec[64];
    Bool curvesInitialized;
    SndCurve volumeFalloffCurves[16];
    char volumeFalloffCurveNames[16][64];
};

struct va_info_t {
    char va_string[2][1024];
    int index;
};
#endif
