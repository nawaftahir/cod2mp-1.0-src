#ifndef CLEAN_PC_UNIVERSAL_Q_SHARED_H
#define CLEAN_PC_UNIVERSAL_Q_SHARED_H

#include "../../cod2_fwd.h"
typedef struct EffectVisInfo EffectVisInfo;
typedef struct ping_t ping_t;
typedef struct qtime_s qtime_t;

struct EffectVisInfo {
    vec3_t origin;
    float distSq;
    float vis;
};

struct ping_t {
    netadr_t adr;
    int start;
    int time;
    char info[1024];
};

COD2_ASSERT_FIELD(ping_t, adr, 0x0);
COD2_ASSERT_FIELD(ping_t, start, 0x14);
COD2_ASSERT_FIELD(ping_t, time, 0x18);
COD2_ASSERT_FIELD(ping_t, info, 0x1c);
COD2_ASSERT_SIZE(ping_t, 0x41c);
#endif
