/* MSVC-only <dirent.h> shim: POSIX directory iteration, backed by the Win32
 * FindFirstFile API in dirent_msvc.c. This HEADER stays free of <windows.h> so
 * engine TUs that include it don't drag the Windows SDK types into the engine's
 * own Win32 type universe; the implementation TU is isolated. */
#ifndef COD2_MSVC_DIRENT_H
#define COD2_MSVC_DIRENT_H
#ifdef _WIN32

#define DT_UNKNOWN 0
#define DT_DIR     4
#define DT_REG     8

struct dirent {
    char d_name[260];   /* MAX_PATH */
    int  d_type;
};

typedef struct cod2_DIR DIR;   /* opaque; completed in dirent_msvc.c */

#ifdef __cplusplus
extern "C" {
#endif
DIR           *opendir(const char *path);
struct dirent *readdir(DIR *dir);
int            closedir(DIR *dir);
void           rewinddir(DIR *dir);
#ifdef __cplusplus
}
#endif

#endif /* _WIN32 */
#endif
