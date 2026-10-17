#ifndef CLEAN_PC_GAME_MP_G_TYPES_H
#define CLEAN_PC_GAME_MP_G_TYPES_H

#include "../../cod2_fwd.h"
#include "../../cod2_defs.h"
#include "../universal/com_vector.h"

typedef void (*fn_reached)(gentity_t *ent);
typedef void (*fn_blocked)(gentity_t *ent, gentity_t *other);
typedef void (*fn_controller)(gentity_t *ent, int *boneIndices);
typedef void (*fn_pain)(gentity_t *self, gentity_t *attacker, int damage,
                        const vec_t *point, int mod, hitLocation_t hitLoc);
typedef void (*fn_die)(gentity_t *self, gentity_t *inflictor, gentity_t *attacker,
                       int damage, int mod, int contents, const vec_t *point,
                       hitLocation_t hitLoc, int modelIndex);

typedef struct BuiltinFunctionDef BuiltinFunctionDef;
typedef struct BuiltinMethodDef BuiltinMethodDef;
typedef struct client_fields_s client_fields_s;
typedef struct entityHandler_t entityHandler_t;
typedef struct game_hudelem_field_t game_hudelem_field_t;
typedef struct game_hudelem_s game_hudelem_s;
typedef struct ipFilter_s ipFilter_s;
typedef struct ent_field_t ent_field_t;
typedef struct pushed_t pushed_t;
typedef struct scr_data_t scr_data_t;
typedef struct spawn_t spawn_t;
typedef struct tagInfo_s tagInfo_s;
typedef struct level_locals_t level_locals_t;
typedef struct turretInfo_s turretInfo_s;
typedef struct useList_t useList_t;
typedef struct gclient_s gclient_s;

enum hudelem_update_t {
    HUDELEM_UPDATE_ARCHIVAL = 1,
    HUDELEM_UPDATE_CURRENT = 2,
    HUDELEM_UPDATE_ARCHIVAL_AND_CURRENT = 3
};

struct BuiltinFunctionDef {
    const char *actionString;
    BuiltinFunction actionFunc;
    int type;
};

struct BuiltinMethodDef {
    const char *actionString;
    BuiltinMethod actionFunc;
    int type;
};

struct client_fields_s {
    const char *name;
    int ofs;
    fieldtype_t type;
    ScriptCallbackClient setter;
    ScriptCallbackClient getter;
};

struct entityHandler_t {
    fn_think think;
    fn_reached reached;
    fn_blocked blocked;
    fn_touch touch;
    fn_use use;
    fn_pain pain;
    fn_die die;
    fn_controller controller;
    int methodOfDeath;
    int splashMethodOfDeath;
};

struct game_hudelem_field_t {
    const char *name;
    int ofs;
    fieldtype_t type;
    int mask;
    int shift;
    ScriptCallbackHudElem setter;
    ScriptCallbackHudElem getter;
};

struct game_hudelem_s {
    hudelem_t elem;
    int clientNum;
    int team;
    qboolean archived;
};

struct ipFilter_s {
    unsigned int mask;
    unsigned int compare;
};

struct ent_field_t {
    const char *name;
    int ofs;
    fieldtype_t type;
    ScriptCallbackEnt callback;
};

struct pushed_t {
    gentity_t *ent;
    vec3_t origin;
    vec3_t angles;
    float deltayaw;
};

struct scr_data_t {
    scr_func_t levelscript;
    scr_func_t gametypescript;
    struct { scr_func_t main; scr_func_t startupgametype; scr_func_t playerconnect; scr_func_t playerdisconnect; scr_func_t playerdamage; scr_func_t playerkilled; scr_func_t votecalled; scr_func_t playervote; int iNumGameTypes; gameTypeScript_t list[32]; } gametype;
    scr_func_t delete_;
    scr_func_t initstructs;
    scr_func_t createstruct;
    corpseInfo_t playerCorpseInfo[8];
};

struct spawn_t {
    const char *name;
    void (*spawn)(gentity_t *ent);
};

struct tagInfo_s {
    gentity_t *parent;
    gentity_t *next;
    scr_string_t name;
    int index;
    vec3_t axis[4];
    vec3_t parentInvAxis[4];
};

struct level_locals_t {
    gclient_s *clients;
    gentity_s *gentities;
    int gentitySize;
    int num_entities;
    gentity_s *firstFreeEnt;
    gentity_s *lastFreeEnt;
    fileHandle_t logFile;
    qboolean initializing;
    qboolean clientIsSpawning;
    objective_t objectives[16];
    int maxclients;
    int framenum;
    int time;
    int previousTime;
    int frametime;
    int startTime;
    int teamScores[4];
    int lastTeammateHealthTime;
    qboolean bUpdateScoresForIntermission;
    qboolean manualNameChange;
    int numConnectedClients;
    int sortedClients[64];
    char voteString[1024];
    char voteDisplayString[1024];
    int voteTime;
    int voteExecuteTime;
    int voteYes;
    int voteNo;
    int numVotingClients;
    char teamVoteString[2][1024];
    int teamVoteTime[2];
    int teamVoteYes[2];
    int teamVoteNo[2];
    struct SpawnVar spawnVar;
    qboolean savepersist;
    gentity_t * droppedWeaponCue[32];
    float fFogOpaqueDist;
    float fFogOpaqueDistSqrd;
    int remapCount;
    int currentPlayerClone;
    trigger_info_t pendingTriggerList[256];
    trigger_info_t currentTriggerList[256];
    int pendingTriggerListSize;
    int currentTriggerListSize;
    int finished;
    qboolean bPlayerIgnoreRadiusDamage;
    qboolean bPlayerIgnoreRadiusDamageLatched;
    qboolean registerWeapons;
    qboolean bRegisterItems;
    int currentEntityThink;
    fileHandle_t openScriptIOFileHandles[1];
    char * openScriptIOFileBuffers[1];
    com_parse_mark_t currentScriptIOLineMark[1];
};

struct turretInfo_s {
    qboolean inuse;
    int flags;
    int fireTime;
    float arcmin[2];
    float arcmax[2];
    float dropPitch;
    int stance;
    int prevStance;
    int fireSndDelay;
    vec3_t userOrigin;
    float playerSpread;
    float pitchCap;
    SoundAlias fireSnd;
    SoundAlias fireSndPlayer;
    SoundAlias stopSnd;
    SoundAlias stopSndPlayer;
};

struct useList_t {
    gentity_t *ent;
    float score;
};

struct gclient_s {
    playerState_t ps;
    struct clientSession_t sess;
    int spectatorClient;
    qboolean noclip;
    qboolean ufo;
    qboolean bFrozen;
    int lastCmdTime;
    int buttons;
    int oldbuttons;
    int latched_buttons;
    int buttonsSinceLastFrame;
    vec3_t oldOrigin;
    float fGunPitch;
    float fGunYaw;
    int damage_blood;
    vec3_t damage_from;
    qboolean damage_fromWorld;
    int accurateCount;
    int accuracy_shots;
    int accuracy_hits;
    int inactivityTime;
    qboolean inactivityWarning;
    int lastVoiceTime;
    int switchTeamTime;
    float currentAimSpreadScale;
    gentity_t *persistantPowerup;
    int portalID;
    int dropWeaponTime;
    int sniperRifleFiredTime;
    float sniperRifleMuzzleYaw;
    int PCSpecialPickedUpCount;
    gentity_t *pLookatEnt;
    int useHoldEntity;
    int useHoldTime;
    int iLastCompassFriendlyInfoEnt;
    int compassPingTime;
    int damageTime;
    float v_dmg_roll;
    float v_dmg_pitch;
    vec3_t swayViewAngles;
    vec3_t swayOffset;
    vec3_t swayAngles;
    vec3_t vLastMoveAng;
    float fLastIdleFactor;
    vec3_t vGunOffset;
    vec3_t vGunSpeed;
    int weapIdleTime;
    int lastServerTime;
    int lastSpawnTime;
};
#endif
