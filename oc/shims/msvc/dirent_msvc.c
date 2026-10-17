/* Win32 FindFirstFile-backed implementation of the <dirent.h> shim.
 *
 * Isolated TU: includes <windows.h> but NOT the engine headers (cod2_defs.h
 * etc.), so the Windows SDK types stay out of the engine's own Win32 universe.
 * Compiled only in the MSVC build (added to the source set in win32-msvc.cmake).
 */
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>
#include <string.h>

/* must match the declarations in shims-msvc/dirent.h */
#define DT_UNKNOWN 0
#define DT_DIR     4
#define DT_REG     8
struct dirent { char d_name[260]; int d_type; };

struct cod2_DIR {
    HANDLE          handle;
    WIN32_FIND_DATAA find;
    int             pending;     /* a FindFirstFile result is buffered */
    char            pattern[260];
    struct dirent   ent;
};
typedef struct cod2_DIR DIR;

static void fill_pattern(struct cod2_DIR *d, const char *path)
{
    size_t n;
    strncpy(d->pattern, path, sizeof(d->pattern) - 3);
    d->pattern[sizeof(d->pattern) - 3] = '\0';
    n = strlen(d->pattern);
    if (n && d->pattern[n - 1] != '\\' && d->pattern[n - 1] != '/') d->pattern[n++] = '\\';
    d->pattern[n++] = '*';
    d->pattern[n]   = '\0';
}

DIR *opendir(const char *path)
{
    struct cod2_DIR *d;
    if (!path) return NULL;
    d = (struct cod2_DIR *)calloc(1, sizeof(*d));
    if (!d) return NULL;
    fill_pattern(d, path);
    d->handle = FindFirstFileA(d->pattern, &d->find);
    if (d->handle == INVALID_HANDLE_VALUE) { free(d); return NULL; }
    d->pending = 1;
    return d;
}

struct dirent *readdir(DIR *dir)
{
    if (!dir) return NULL;
    if (!dir->pending) {
        if (!FindNextFileA(dir->handle, &dir->find)) return NULL;
    }
    dir->pending = 0;
    strncpy(dir->ent.d_name, dir->find.cFileName, sizeof(dir->ent.d_name) - 1);
    dir->ent.d_name[sizeof(dir->ent.d_name) - 1] = '\0';
    dir->ent.d_type = (dir->find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? DT_DIR : DT_REG;
    return &dir->ent;
}

int closedir(DIR *dir)
{
    if (!dir) return -1;
    if (dir->handle != INVALID_HANDLE_VALUE) FindClose(dir->handle);
    free(dir);
    return 0;
}

void rewinddir(DIR *dir)
{
    if (!dir) return;
    if (dir->handle != INVALID_HANDLE_VALUE) FindClose(dir->handle);
    dir->handle = FindFirstFileA(dir->pattern, &dir->find);
    dir->pending = (dir->handle != INVALID_HANDLE_VALUE);
}
#endif /* _WIN32 */
