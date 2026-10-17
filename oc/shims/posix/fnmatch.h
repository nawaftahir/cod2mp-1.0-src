#ifndef WIN32_SHIM_FNMATCH_H
#define WIN32_SHIM_FNMATCH_H
#include <string.h>
#define FNM_NOMATCH 1
#define FNM_PATHNAME 0
#define FNM_NOESCAPE 0
#define FNM_PERIOD 0
#define FNM_CASEFOLD 0
static __inline int fnmatch(const char *pat, const char *str, int flags) {
    (void)flags;
    const char *star = strchr(pat, '*');
    if (!star) return strcmp(pat, str) ? FNM_NOMATCH : 0;
    size_t pre = (size_t)(star - pat);
    if (strncmp(pat, str, pre) != 0) return FNM_NOMATCH;
    const char *suf = star + 1; size_t sl = strlen(suf), tl = strlen(str);
    if (sl == 0) return 0;
    if (tl < pre + sl) return FNM_NOMATCH;
    return strcmp(str + tl - sl, suf) ? FNM_NOMATCH : 0;
}
#endif
