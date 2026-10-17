#ifndef CLEAN_PC_BGAME_BG_TYPES_H
#define CLEAN_PC_BGAME_BG_TYPES_H

#include "../../cod2_fwd.h"
#include "../../cod2_defs.h"

typedef struct MantleAnimTransition MantleAnimTransition;
typedef struct animConditionTable_t animConditionTable_t;
typedef struct animStringItem_t animStringItem_t;
typedef struct pmoveHandler_t pmoveHandler_t;
typedef struct loadAnim_t loadAnim_t;
typedef struct viewLerpWaypoint_s viewLerpWaypoint_s;

enum aistateEnum_t {
    AISTATE_NULL = -1,
    AISTATE_RELAXED = 0,
    AISTATE_QUERY = 1,
    AISTATE_ALERT = 2,
    AISTATE_COMBAT = 3,
    MAX_AISTATES = 4
};

enum animBodyPart_t {
    ANIM_BP_UNUSED = 0,
    ANIM_BP_LEGS = 1,
    ANIM_BP_TORSO = 2,
    ANIM_BP_BOTH = 3,
    NUM_ANIM_BODYPARTS = 4
};

enum proneCheckType_t {
    PCT_CLIENT = 0,
    PCT_ACTOR = 1
};

enum scriptAnimEventTypes_t {
    ANIM_ET_PAIN = 0,
    ANIM_ET_DEATH = 1,
    ANIM_ET_FIREWEAPON = 2,
    ANIM_ET_JUMP = 3,
    ANIM_ET_JUMPBK = 4,
    ANIM_ET_LAND = 5,
    ANIM_ET_DROPWEAPON = 6,
    ANIM_ET_RAISEWEAPON = 7,
    ANIM_ET_CLIMB_MOUNT = 8,
    ANIM_ET_CLIMB_DISMOUNT = 9,
    ANIM_ET_RELOAD = 10,
    ANIM_ET_CROUCH_TO_PRONE = 11,
    ANIM_ET_PRONE_TO_CROUCH = 12,
    ANIM_ET_STAND_TO_CROUCH = 13,
    ANIM_ET_CROUCH_TO_STAND = 14,
    ANIM_ET_STAND_TO_PRONE = 15,
    ANIM_ET_PRONE_TO_STAND = 16,
    ANIM_ET_MELEEATTACK = 17,
    ANIM_ET_SHELLSHOCK = 18,
    NUM_ANIM_EVENTTYPES = 19
};

enum scriptAnimMoveTypes_t {
    ANIM_MT_UNUSED = 0,
    ANIM_MT_IDLE = 1,
    ANIM_MT_IDLECR = 2,
    ANIM_MT_IDLEPRONE = 3,
    ANIM_MT_WALK = 4,
    ANIM_MT_WALKBK = 5,
    ANIM_MT_WALKCR = 6,
    ANIM_MT_WALKCRBK = 7,
    ANIM_MT_WALKPRONE = 8,
    ANIM_MT_WALKPRONEBK = 9,
    ANIM_MT_RUN = 10,
    ANIM_MT_RUNBK = 11,
    ANIM_MT_RUNCR = 12,
    ANIM_MT_RUNCRBK = 13,
    ANIM_MT_TURNRIGHT = 14,
    ANIM_MT_TURNLEFT = 15,
    ANIM_MT_TURNRIGHTCR = 16,
    ANIM_MT_TURNLEFTCR = 17,
    ANIM_MT_CLIMBUP = 18,
    ANIM_MT_CLIMBDOWN = 19,
    ANIM_MT_MANTLE_ROOT = 20,
    ANIM_MT_MANTLE_UP_57 = 21,
    ANIM_MT_MANTLE_UP_51 = 22,
    ANIM_MT_MANTLE_UP_45 = 23,
    ANIM_MT_MANTLE_UP_39 = 24,
    ANIM_MT_MANTLE_UP_33 = 25,
    ANIM_MT_MANTLE_UP_27 = 26,
    ANIM_MT_MANTLE_UP_21 = 27,
    ANIM_MT_MANTLE_OVER_HIGH = 28,
    ANIM_MT_MANTLE_OVER_MID = 29,
    ANIM_MT_MANTLE_OVER_LOW = 30,
    ANIM_MT_FLINCH_FORWARD = 31,
    ANIM_MT_FLINCH_BACKWARD = 32,
    ANIM_MT_FLINCH_LEFT = 33,
    ANIM_MT_FLINCH_RIGHT = 34,
    ANIM_MT_STUMBLE_FORWARD = 35,
    ANIM_MT_STUMBLE_BACKWARD = 36,
    ANIM_MT_STUMBLE_WALK_FORWARD = 37,
    ANIM_MT_STUMBLE_WALK_BACKWARD = 38,
    ANIM_MT_STUMBLE_CROUCH_FORWARD = 39,
    ANIM_MT_STUMBLE_CROUCH_BACKWARD = 40,
    NUM_ANIM_MOVETYPES = 41
};

struct MantleAnimTransition {
    int upAnimIndex;
    int overAnimIndex;
    float height;
};

struct animConditionTable_t {
    animScriptConditionTypes_t type;
    animStringItem_t *values;
};

struct animStringItem_t {
    const char *string;
    int hash;
};

struct pmoveHandler_t {
    pmove_trace trace;
    pmove_pointcontents pointcontents;
    pmove_PlayerEvent playerEvent;
};

struct loadAnim_t {
    scr_anim_t anim;
    int iNameHash;
    char szAnimName[64];
};

struct viewLerpWaypoint_s {
    int iFrac;
    float fViewHeight;
    int iOffset;
};
#endif
