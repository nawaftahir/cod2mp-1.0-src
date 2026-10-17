#ifndef CLEAN_PC_BOTLIB_BOT_TYPES_H
#define CLEAN_PC_BOTLIB_BOT_TYPES_H

#include "../../cod2_fwd.h"
#include "../../cod2_defs.h"

typedef struct define_s define_s;
typedef struct directive_t directive_t;
typedef struct indent_s indent_s;
typedef struct punctuation_s punctuation_s;
typedef struct script_s script_s;

struct define_s {
    char *name;
    int flags;
    int builtin;
    int numparms;
    token_t *parms;
    token_t *tokens;
    intptr_t next;       /* pointer-as-int -> truncated on x64 (intptr_t==int on x86) */
    intptr_t hashnext;
};

struct directive_t {
    char *name;
    int (*func)(struct source_s *);   /* real handler sig (PC_Directive_*); byte-neutral fn-ptr */
};

struct indent_s {
    int type;
    int skip;
    script_t *script;
    intptr_t next;       /* pointer-as-int -> truncated on x64 (intptr_t==int on x86) */
};

struct punctuation_s {
    char *p;
    int n;
    intptr_t next;   /* linked-list ptr (was int -> truncated on x64; intptr_t==int on x86) */
};

struct script_s {
    char filename[64];
    char *buffer;
    char *script_p;
    char *end_p;
    char *lastscript_p;
    char *whitespace_p;
    char *endwhitespace_p;
    int length;
    int line;
    int lastline;
    int tokenavailable;
    int flags;
    punctuation_t *punctuations;
    punctuation_t * *punctuationtable;
    char _pad_token_base[12];
    token_t token;
    struct script_s *next;
};
#endif
