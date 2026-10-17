#ifndef CLEAN_PC_SCRIPT_SCR_LOCAL_H
#define CLEAN_PC_SCRIPT_SCR_LOCAL_H

#include "../../cod2_fwd.h"

typedef struct stype_t stype_t;
typedef int yy_state_type;

typedef struct BreakStatementInfo BreakStatementInfo;
typedef struct CaseStatementInfo CaseStatementInfo;
typedef struct ContinueStatementInfo ContinueStatementInfo;
typedef struct OpcodeLookup OpcodeLookup;
typedef struct PrecacheEntry PrecacheEntry;
typedef struct SaveSourceBufferInfo SaveSourceBufferInfo;
typedef struct SourceBufferInfo SourceBufferInfo;
typedef struct SourceLookup SourceLookup;
typedef struct UI_ScrollPane UI_ScrollPane;
typedef struct UI_VerticalDivider UI_VerticalDivider;
typedef struct VariableStackBuffer VariableStackBuffer;
typedef struct scrAnimGlob_t scrAnimGlob_t;
typedef struct scrCompilePub_t scrCompilePub_t;
typedef struct scrMemTreeGlob_t scrMemTreeGlob_t;
typedef struct scrMemTreePub_t scrMemTreePub_t;
typedef struct scrParserGlob_t scrParserGlob_t;
typedef struct scrParserPub_t scrParserPub_t;
typedef struct scrVarPub_t scrVarPub_t;
typedef struct scrVmGlob_t scrVmGlob_t;
typedef struct scrVmPub_t scrVmPub_t;
typedef struct scrAnimPub_t scrAnimPub_t;
typedef struct scr_classStruct_t scr_classStruct_t;
typedef struct scr_block_s scr_block_s;
typedef struct scrCompileGlob_t scrCompileGlob_t;
typedef struct yy_buffer_state yy_buffer_state;

struct stype_t {
    sval_t val;
    unsigned int pos;
};

struct BreakStatementInfo {
    const char *codePos;
    const char *nextCodePos;
    struct BreakStatementInfo *next;   /* compile-time temp list; was int -> truncated the pointer on x64 */
};

struct CaseStatementInfo {
    unsigned int name;
    const char *codePos;
    unsigned int sourcePos;
    struct CaseStatementInfo *next;   /* compile-time temp list; was int -> truncated the pointer on x64 */
};

struct ContinueStatementInfo {
    const char *codePos;
    const char *nextCodePos;
    struct ContinueStatementInfo *next;   /* compile-time temp list; was int -> truncated the pointer on x64 */
};

struct OpcodeLookup {
    const char *codePos;
    unsigned int sourcePosIndex;
    unsigned int sourcePosCount;
    int profileTime;
    int profileUsage;
};

struct PrecacheEntry {
    scr_string_t filename;
    Bool include;
    unsigned int sourcePos;
    struct PrecacheEntry *next;   /* links per-file precache arrays; was int -> truncated the pointer on x64 */
};

struct SaveSourceBufferInfo {
    char *sourceBuf;
    int len;
};

struct SourceBufferInfo {
    const char *codePos;
    char *buf;
    const char *sourceBuf;
    int len;
    int sortedIndex;
    Bool archive;
};

struct SourceLookup {
    unsigned int sourcePos;
    int type;
};

// UI_ScrollPane / UI_VerticalDivider are declared in ui/ui_component.cpp.

struct VariableStackBuffer {
    const char *pos;
    short unsigned int size;
    short unsigned int bufLen;
    short unsigned int localId;
    byte time;
    char buf[1];
};

struct scrAnimGlob_t {
    const char *start;
    const char *pos;
    short unsigned int using_xanim_lookup[2][128];
    int bAnimCheck;
};

struct scrCompilePub_t {
    int value_count;
    int far_function_count;
    unsigned int loadedscripts;
    unsigned int scripts;
    unsigned int builtinFunc;
    unsigned int builtinMeth;
    short unsigned int *canonicalStrings;
    const char *in_ptr;
    const char *parseBuf;
    Bool script_loading;
    Bool allowedBreakpoint;
    int developer_statement;
    byte *opcodePos;
    int programLen;
    int func_table_size;
    intptr_t func_table[1024];   /* holds BuiltinFunction/BuiltinMethod pointers; was int -> truncated on x64 */
};

struct scrMemTreeGlob_t {
    MemoryNode nodes[65536];
    unsigned char leftBits[256];
    unsigned char numBits[256];
    unsigned char logBits[256];
    short unsigned int head[17];
    int totalAlloc;
    int totalAllocBuckets;
};

struct scrMemTreePub_t {
    char *mt_buffer;
};

struct scrParserGlob_t {
    OpcodeLookup *opcodeLookup;
    unsigned int opcodeLookupMaxLen;
    unsigned int opcodeLookupLen;
    SourceLookup *sourcePosLookup;
    unsigned int sourcePosLookupMaxLen;
    unsigned int sourcePosLookupLen;
    unsigned int sourceBufferLookupMaxLen;
    const byte *currentCodePos;
    unsigned int currentSourcePosCount;
    SaveSourceBufferInfo *saveSourceBufferLookup;
    int saveSourceBufferLookupLen;
    int delayedSourceIndex;
    int threadStartSourceIndex;
};

struct scrParserPub_t {
    SourceBufferInfo *sourceBufferLookup;
    unsigned int sourceBufferLookupLen;
    const char *scriptfilename;
    const char *sourceBuf;
};

struct scrVarPub_t {
    const char *fieldBuffer;
    int mark;
    short unsigned int canonicalStrCount;
    Bool developer;
    Bool developer_script;
    Bool evaluate;
    const char *error_message;
    int error_index;
    int time;
    unsigned int timeArrayId;
    unsigned int pauseArrayId;
    unsigned int levelId;
    unsigned int gameId;
    unsigned int animId;
    unsigned int freeEntList;
    unsigned int tempVariable;
    Bool bInited;
    short unsigned int savecount;
    unsigned int checksum;
    unsigned int entId;
    unsigned int entFieldName;
    const char *programBuffer;
    const char *endScriptBuffer;
    short unsigned int saveIdMap[65534];
    short unsigned int saveIdMapRev[65534];
};

struct scrVmGlob_t {
    VariableValue eval_stack[2];
    const char *dialog_error_message;
    int loading;
    unsigned int starttime;
    unsigned int localVarsStack[2048];
};

struct scrVmPub_t {
    unsigned int *localVars;
    VariableValue *maxstack;
    int function_count;
    function_frame_t *function_frame;
    VariableValue *top;
    Bool debugCode;
    Bool abort_on_error;
    Bool terminal_error;
    unsigned int inparamcount;
    unsigned int outparamcount;
    function_frame_t function_frame_start[32];
    VariableValue stack[2048];
};

struct scrAnimPub_t {
    unsigned int animtrees;
    unsigned int animtree_node;
    unsigned int animTreeNames;
    scr_animtree_t xanim_lookup[2][128];
    int xanim_num[2];
    int animTreeIndex;
    Bool animtree_loading;
};

struct scr_classStruct_t {
    short unsigned int id;
    short unsigned int entArrayId;
    char charId;
    const char *name;
};

struct scr_block_s {
    int abortLevel;
    int localVarsCreateCount;
    int localVarsPublicCount;
    int localVarsCount;
    byte localVarsInitBits[8];
    scr_localVar_t localVars[64];
};

struct scrCompileGlob_t {
    byte *codePos;
    byte *prevOpcodePos;
    unsigned int fileId;
    unsigned int threadId;
    int cumulOffset;
    int maxOffset;
    int maxCallOffset;
    Bool bConstRefCount;
    Bool in_developer_thread;
    unsigned int developer_thread_sourcePos;
    Bool firstThread[2];
    Bool bCanIgnoreCase;
    CaseStatementInfo *currentCaseStatement;
    Bool bCanBreak;
    Bool bCanIgnoreBreak;
    BreakStatementInfo *currentBreakStatement;
    Bool bCanContinue;
    Bool bCanIgnoreContinue;
    ContinueStatementInfo *currentContinueStatement;
    scr_block_t * *breakChildBlocks;
    int *breakChildCount;
    scr_block_t *breakBlock;
    scr_block_t * *continueChildBlocks;
    int *continueChildCount;
    Bool forceNotCreate;
    PrecacheEntry *precachescriptList;
    PrecacheEntry *precachescriptListHead;
    VariableCompileValue value_start[32];
};

struct yy_buffer_state {
    FILE *yy_input_file;
    char *yy_ch_buf;
    char *yy_buf_pos;
    yy_size_t yy_buf_size;
    int yy_n_chars;
    int yy_is_our_buffer;
    int yy_is_interactive;
    int yy_at_bol;
    int yy_fill_buffer;
    int yy_buffer_status;
};
#endif
