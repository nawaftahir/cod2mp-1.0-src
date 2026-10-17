#ifndef CLEAN_PC_CGAME_MP_CG_TYPES_H
#define CLEAN_PC_CGAME_MP_CG_TYPES_H

#include "../../cod2_fwd.h"
#include "../../cod2_defs.h"

typedef struct cg_hudelem_t cg_hudelem_t;
typedef struct consoleCommand_t consoleCommand_t;
typedef struct lagometer_t lagometer_t;
typedef struct listColumnInfo_t listColumnInfo_t;
typedef struct sbpicinfo_t sbpicinfo_t;
typedef struct cgs_t cgs_t;

typedef struct weaponInfo_s weaponInfo_s;

struct cg_hudelem_t {
    float x;
    float y;
    float z;
    float width;
    float height;
    char hudElemLabel[256];
    float labelWidth;
    char hudElemText[256];
    float textWidth;
    FontHandle font;
    float fontScale;
    float fontHeight;
    vec4_t color;
};

struct consoleCommand_t {
    char *cmd;
    void (*function)();
};

struct lagometer_t {
    int frameSamples[128];
    int frameCount;
    int snapshotFlags[128];
    int snapshotSamples[128];
    int snapshotCount;
};

struct listColumnInfo_t {
    listColumnTypes_t type;
    float fWidth;
    const char *pszName;
    int iAlignment;
};

struct sbpicinfo_t {
    float x;
    float y;
    float w;
    float h;
};

struct cgs_t {
    struct gameState_t gameState;
    int viewX;
    int viewY;
    int viewWidth;
    int viewHeight;
    float viewAspect;
    int serverCommandSequence;
    int processedSnapshotNum;
    qboolean localServer;
    char gametype[32];
    char szHostName[256];
    int maxclients;
    char mapname[64];
    char redTeam[64];
    char blueTeam[64];
    int voteTime;
    int voteYes;
    int voteNo;
    char voteString[256];
    int teamVoteTime[2];
    int teamVoteYes[2];
    int teamVoteNo[2];
    qboolean teamVoteModified[2];
    char teamVoteString[2][256];
    int levelStartTime;
    int teamScores[2];
    struct XModel * gameModels[256];
    struct FxEffectDef * fxs[64];
    struct FxEffectDef *smokeGrenadeFx;
    shellshock_parms_t shellshockParms[16];
    struct shellshock_parms_t holdBreathParams;
    int numInlineModels;
    struct GfxBrushModel * inlineDrawModel[1023];
    vec3_t inlineModelMidpoints[1023];
    char teamChatMsgs[8][271];
    int teamChatMsgTimes[8];
    int teamChatPos;
    int teamLastChatPos;
    struct cgMedia_t media;
    float compassWidth;
    float compassHeight;
    float compassY;
    clientInfo_t corpseinfo[8];
};

struct weaponInfo_s {
    struct DObj_s *viewModelDObj;
    float viewModelAnimRates[23];
    char handModel[64];
    int iPrevAnim;
    struct XAnimTree_s *tree;
    qboolean registered;
    const gitem_t *item;
    const char *pszTranslatedDisplayName;
    const char *pszTranslatedModename;
    const char *pszTranslatedAIOverlayDescription;
    struct XModel *worldSurfModel;
    struct FxEffectDef *viewFlashEffect;
    struct FxEffectDef *worldFlashEffect;
    vec3_t weaponMidpoint;
    snd_alias_list_t *missileSound;
    snd_alias_list_t *pullbackSound;
    snd_alias_list_t *flashSound;
    snd_alias_list_t *flashSoundPlayer;
    snd_alias_list_t *lastShotSound;
    snd_alias_list_t *lastShotSoundPlayer;
    snd_alias_list_t *meleeSwipeSound;
    snd_alias_list_t *rechamberSound;
    snd_alias_list_t *rechamberSoundPlayer;
    snd_alias_list_t *reloadSound;
    snd_alias_list_t *reloadSoundPlayer;
    snd_alias_list_t *reloadEmptySound;
    snd_alias_list_t *reloadEmptySoundPlayer;
    snd_alias_list_t *reloadStartSound;
    snd_alias_list_t *reloadStartSoundPlayer;
    snd_alias_list_t *reloadEndSound;
    snd_alias_list_t *reloadEndSoundPlayer;
    snd_alias_list_t *raiseSound;
    snd_alias_list_t *altSwitchSound;
    snd_alias_list_t *putawaySound;
    snd_alias_list_t * noteTrackSound[4];
    MaterialHandle ammoIcon;
    MaterialHandle hHudIcon;
    MaterialHandle hModeIcon;
    struct XModel *missileSurfModel;
    void (*missileTrailFunc)();
    float missileDlight;
    vec3_t missileDlightColor;
    int missileRenderfx;
    struct FxEffectDef *shellEjectEffect;
    struct FxEffectDef *lastShotEjectEffect;
    struct FxEffectDef *projExplosionEffect;
    snd_alias_list_t *projExplosionSound;
    struct FxEffectDef *projTrailEffect;
    float trailRadius;
    float wiTrailTime;
    MaterialHandle hReticleCenter;
    MaterialHandle hReticleSide;
    MaterialHandle hADSOverlay;
    vec3_t vPositionOrg[2];
    vec3_t vPositionAng[2];
};
#endif
