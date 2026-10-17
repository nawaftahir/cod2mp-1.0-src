#ifndef CLEAN_PC_UI_MP_UI_TYPES_H
#define CLEAN_PC_UI_MP_UI_TYPES_H

#include "../../cod2_fwd.h"
#include "../../cod2_defs.h"

typedef struct ConversionArguments ConversionArguments;
typedef struct ItemKeyHandler ItemKeyHandler;
typedef struct bind_t bind_t;
typedef struct commandDef_t commandDef_t;
typedef struct keywordHash_s keywordHash_s;
typedef struct loadAssets_t loadAssets_t;
typedef struct scrollInfo_s scrollInfo_s;
typedef struct serverFilter_s serverFilter_s;
typedef struct serverStatusDvar_t serverStatusDvar_t;
typedef struct sharedUiInfo_t sharedUiInfo_t;
typedef struct stringDef_s stringDef_s;
typedef struct uiInfo_t uiInfo_t;
typedef struct windowDef_t windowDef_t;

struct ConversionArguments {
    int argCount;
    const char * args[9];
};

struct ItemKeyHandler {
    int key;
    const char *action;
    intptr_t next;
};

struct bind_t {
    char *command;
    int defaultbind1;
    int defaultbind2;
    int bind1;
    int bind2;
};

struct commandDef_t {
    const char *name;
    void (*handler)();
};

struct keywordHash_s {
    char *keyword;
    qboolean (*func)();
    intptr_t next;       /* pointer-as-int -> truncated on x64 (intptr_t==int on x86) */
};

struct loadAssets_t {
    snd_alias_list_t *menuEnterSound;
    snd_alias_list_t *menuExitSound;
    snd_alias_list_t *menuBuzzSound;
    float fadeClamp;
    int fadeCycle;
    float fadeAmount;
    float fadeInAmount;
    float shadowX;
    float shadowY;
    vec4_t shadowColor;
    float shadowFadeClamp;
};

struct scrollInfo_s {
    int nextScrollTime;
    int nextAdjustTime;
    int adjustValue;
    int scrollKey;
    float xStart;
    float yStart;
    itemDef_t *item;
    qboolean scrollDir;
};

struct serverFilter_s {
    const char *description;
    const char *basedir;
};

struct serverStatusDvar_t {
    const char *name;
    const char *altName;
    sscType_t type;
};

struct sharedUiInfo_t {
    struct CachedAssets_t assets;
    int playerCount;
    char playerNames[64][32];
    char teamNames[64][32];
    int playerClientNums[64];
    int numGameTypes;
    gameTypeInfo gameTypes[32];
    int numJoinGameTypes;
    gameTypeInfo joinGameTypes[32];
    int mapCount;
    mapInfo mapList[128];
    MaterialHandle serverHardwareIconList[8];
    modInfo_t modList[64];
    int modCount;
    int modIndex;
    const char * movieList[256];
    int movieCount;
    int movieIndex;
    int previewMovie;
    serverStatus_t serverStatus;
    char serverStatusAddress[64];
    serverStatusInfo_t serverStatusInfo;
    int nextServerStatusRefresh;
    struct pendingServerStatus_t pendingServerStatus;
};

struct stringDef_s {
    struct stringDef_s *next;
    const char *str;
};

struct uiInfo_t {
    displayContextDef_t uiDC;
    int myTeamCount;
    int playerRefresh;
    int playerIndex;
    int playerProfileCount;
    const char * playerProfileName[64];
    struct PlayerProfileStatus playerProfileStatus;
    int timeIndex;
    int previousTimes[4];
    uiMenuCommand_t currentMenuType;
    char findPlayerName[960];
    char foundPlayerServerAddresses[16][64];
    char foundPlayerServerNames[17][64];
    int numFoundPlayerServers;
    int nextFindPlayerRefresh;
};

struct windowDef_t {
    UiRectangle rect[4];
    UiRectangle rectClient[4];
    const char *name;
    const char *group;
    const char *cinematicName;
    int cinematic;
    int style;
    int border;
    int ownerDraw;
    int ownerDrawFlags;
    float borderSize;
    int staticFlags;
    int dynamicFlags[4];
    UiRectangle rectEffects0[4];
    UiRectangle rectEffects1[4];
    int offsetTime[4];
    int nextTime;
    vec4_t foreColor;
    vec4_t backColor;
    vec4_t borderColor;
    vec4_t outlineColor;
    MaterialHandle background;
};
#endif
