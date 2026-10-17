#ifndef WIN32_SHIM_DLFCN_H
#define WIN32_SHIM_DLFCN_H
#define RTLD_NOW   0
#define RTLD_LAZY  0
#define RTLD_GLOBAL 0
void *cod2_dlopen(const char *path, int flag);
void *cod2_dlsym(void *handle, const char *sym);
int   cod2_dlclose(void *handle);
const char *cod2_dlerror(void);
#define dlopen(p,f)  cod2_dlopen((p),(f))
#define dlsym(h,s)   cod2_dlsym((h),(s))
#define dlclose(h)   cod2_dlclose((h))
#define dlerror()    cod2_dlerror()
#endif
