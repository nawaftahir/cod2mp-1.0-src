#ifndef CLEAN_PC_CLIENT_MP_CL_TYPES_H
#define CLEAN_PC_CLIENT_MP_CL_TYPES_H

#include "../../cod2_fwd.h"
#include "../../cod2_defs.h"

typedef struct ConDrawInputGlob ConDrawInputGlob;
typedef struct ScreenPlacementGlob ScreenPlacementGlob;
typedef struct kbutton_t kbutton_t;
typedef struct keyname_t keyname_t;
typedef struct clientConnection_t clientConnection_t;
typedef struct PlayerKeyState PlayerKeyState;

struct ConDrawInputGlob {
    int matchCount;
    const char *inputText;
    int inputTextLen;
    Bool hasExactMatch;
    float x;
    float y;
    float leftX;
    float fontHeight;
};

struct ScreenPlacementGlob {
    vec2_t scaleVirtualToReal;
    vec2_t scaleVirtualToFull;
    vec2_t scaleRealToVirtual;
    float virtualScreenOffsetX;
    vec2_t virtualViewableMin;
    vec2_t virtualViewableMax;
    vec2_t realViewportSize;
    vec2_t realViewableMin;
    vec2_t realViewableMax;
    float subScreenLeft;
};

struct kbutton_t {
    int down[2];
    unsigned int downtime;
    unsigned int msec;
    Bool active;
    Bool wasPressed;
};

struct keyname_t {
    char *name;
    int keynum;
};

struct clientConnection_t {
    connstate_t state;
    int qport;
    int clientNum;
    int lastPacketSentTime;
    int lastPacketTime;
    netadr_t serverAddress;
    int connectTime;
    int connectPacketCount;
    char serverMessage[256];
    int challenge;
    int checksumFeed;
    int reliableSequence;
    int reliableAcknowledge;
    char reliableCommands[128][1024];
    int serverMessageSequence;
    int serverCommandSequence;
    int lastExecutedServerCommand;
    char serverCommands[128][1024];
    fileHandle_t download;
    char downloadTempName[256];
    char downloadName[256];
    int downloadNumber;
    int downloadBlock;
    int downloadCount;
    int downloadSize;
    char downloadList[1024];
    qboolean downloadRestart;
    char demoName[64];
    qboolean demorecording;
    qboolean demoplaying;
    qboolean isTimeDemo;
    qboolean demowaiting;
    qboolean firstDemoFrameSkipped;
    fileHandle_t demofile;
    fileHandle_t timeDemoLog;
    int timeDemoFrames;
    int timeDemoStart;
    int timeDemoPrev;
    int timeDemoBaseTime;
    struct netchan_t netchan;
    netProfileInfo_t *pOOBProf;
};

COD2_ASSERT_FIELD(clientConnection_t, serverAddress, 0x14);
COD2_ASSERT_FIELD(clientConnection_t, connectTime, 0x28);
COD2_ASSERT_FIELD(clientConnection_t, challenge, 0x130);
COD2_ASSERT_FIELD(clientConnection_t, reliableAcknowledge, 0x13c);
COD2_ASSERT_FIELD(clientConnection_t, reliableCommands, 0x140);
COD2_ASSERT_FIELD(clientConnection_t, serverMessageSequence, 0x20140);
COD2_ASSERT_FIELD(clientConnection_t, serverCommands, 0x2014c);
COD2_ASSERT_FIELD(clientConnection_t, netchan, 0x407d0);
#if COD2_IS_PATCH_13
COD2_ASSERT_FIELD(clientConnection_t, pOOBProf, 0x80810);
COD2_ASSERT_SIZE(clientConnection_t, 0x80814);
#else
COD2_ASSERT_FIELD(clientConnection_t, pOOBProf, 0x48810);
COD2_ASSERT_SIZE(clientConnection_t, 0x48814);
#endif

struct PlayerKeyState {
    field_t chatField;
    qboolean chat_team;
    qboolean key_overstrikeMode;
    qboolean anykeydown;
    qkey_t keys[256];
};
#endif
