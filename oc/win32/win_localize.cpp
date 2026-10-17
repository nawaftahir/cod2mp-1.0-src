#include "common_types.h"
#include "imports.h"
#include <stdio.h>

extern "C" FILE *FS_FileOpen( const char *filename, const char *mode );
extern "C" size_t FS_FileRead( void *ptr, size_t size, size_t nmemb, FILE *stream );
extern "C" int FS_FileClose( FILE *stream );
extern "C" int FS_FileSeek( FILE *file, int offset, int whence );
extern qboolean SEH_GetLanguageIndexForName(const char *name, int *langindex);
extern void Com_BeginParseSession(const char *name);
extern void Com_EndParseSession(void);
extern void Com_EndParseSessionInl(void);
extern char *Com_Parse( const char *( *data_p ) );
extern int strcmp(const char *s1, const char *s2);
extern char *va( const char *format, ... );

static char *localization;
static char *localized_strings;
static char language_buffer[4096];

language_t Win_InitLocalization(void)
{
    FILE *fp;
    int size;
    int i;
    int lang;

    localization = 0;
    localized_strings = 0;

    fp = FS_FileOpen("localization.txt", "r");
    if (!fp)
        return (language_t)(0);

    FS_FileSeek(fp, 0, 2);
    size = (int)ftell(fp);
    FS_FileSeek(fp, 0, 0);

    localization = language_buffer;
    size = (int)FS_FileRead(language_buffer, 1, (size_t)size, fp);
    FS_FileClose(fp);

    if (!size) {
        localization = 0;
        return (language_t)(0);
    }

    localization[size] = '\0';

    lang = (language_t)(0);
    for (i = 0; localization[i]; i++) {
        if (localization[i] == '\n') {
            localization[i] = '\0';
            localized_strings = &localization[i + 1];
            SEH_GetLanguageIndexForName(localization, &lang);
            break;
        }
    }

    return (language_t)lang;
}

void Win_ShutdownLocalization(void)
{
    localization = 0;
    localized_strings = 0;
}

// retail 0x461f90, unreferenced; name unknown
const char *Win_CopyString_461f90(const char *s)
{
    return va("%s", s);
}

const char *Win_LocalizeRef(const char *ref)
{
    const char *strings;
    const char *key;
    const char *value;
    int match;

    Com_BeginParseSession("localization");
    strings = localized_strings;

    for (;;) {
        key = Com_Parse(&strings);
        if (*key == '\0')
            break;

        match = strcmp(key, ref) == 0;

        value = Com_Parse(&strings);
        if (*value == '\0')
            break;

        if (match) {
            Com_EndParseSessionInl();
            return va("%s", value);
        }
    }

    Com_EndParseSessionInl();
    return va("%s", ref);
}

