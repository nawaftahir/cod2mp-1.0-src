#ifndef CLEAN_PC_WIN32_WIN_TYPES_H
#define CLEAN_PC_WIN32_WIN_TYPES_H

#include "../../cod2_fwd.h"
#include "../../cod2_defs.h"

typedef struct MssLocal MssLocal;
typedef struct MssSound MssSound;
typedef struct cin_cache cin_cache;
typedef struct cinematics_t cinematics_t;

struct MssLocal {
    HDIGDRIVER driver_2D;
    HPROVIDER provider_3D;
    HSAMPLE handle_2D[8];
    void * handle_3D[32];
    HSTREAM handle_stream[13];
    struct { HSAMPLE handle; int rate; int width; int channels; byte *buf; byte bufReady[32]; int bufSize; int bufUsed; int readBuf; int writeBuf; double sampleTime; double bufRate; } raw;
};

struct MssSound {
    AILSOUNDINFO info;
    char data[1];
};

struct cin_cache {
    char fileName[256];
    int CIN_WIDTH;
    int CIN_HEIGHT;
    int xpos;
    int ypos;
    int width;
    int height;
    qboolean looping;
    qboolean holdAtEnd;
    qboolean dirty;
    qboolean alterGameState;
    qboolean silent;
    qboolean material;
    qboolean letterBox;
    qboolean sound;
    fileHandle_t iFile;
    e_status status;
    unsigned int startTime;
    unsigned int lastFrameTime;
    unsigned int lastTime;
    long int tfps;
    long int RoQPlayed;
    long int ROQSize;
    unsigned int RoQFrameSize;
    long int onQuad;
    long int numQuads;
    long int samplesPerLine;
    unsigned int roq_id;
    long int screenDelta;
    void (*VQ0)(byte **status, unsigned char *data);
    void (*VQ1)(byte **status, unsigned char *data);
    void (*VQNormal)(byte **status, unsigned char *data);
    void (*VQBuffer)(byte **status, unsigned char *data);
    long int samplesPerPixel;
    byte *gray;
    unsigned int xsize;
    unsigned int ysize;
    unsigned int maxsize;
    unsigned int minsize;
    qboolean half;
    qboolean smootheddouble;
    qboolean inMemory;
    long int normalBuffer0;
    long int roq_flags;
    long int roqF0;
    long int roqF1;
    long int t[2];
    long int roqFPS;
    int playonwalls;
    byte *buf;
    connstate_t previousGameState;
};

struct cinematics_t {
    byte linbuf[2097152];
    byte file[65536];
    short int sqrTable[256];
    unsigned int mcomp[256];
    byte * qStatus[2][32768];
    long int oldXOff;
    long int oldYOff;
    long int oldysize;
    long int oldxsize;
    int currentHandle;
};
#endif
