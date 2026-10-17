#if defined(_WIN32) && defined(COD2_GFX_DLL_RETAIL_V60)

typedef void *HANDLE;
typedef unsigned long DWORD;
typedef int BOOL;
typedef const char *LPCSTR;
typedef void *LPVOID;
typedef void *LPSECURITY_ATTRIBUTES;
typedef DWORD(__stdcall *LPTHREAD_START_ROUTINE)(LPVOID);

extern HANDLE __stdcall CreateEventA(LPSECURITY_ATTRIBUTES, BOOL, BOOL, LPCSTR);
extern HANDLE __stdcall CreateThread(LPSECURITY_ATTRIBUTES, unsigned, LPTHREAD_START_ROUTINE, LPVOID, DWORD, DWORD *);
extern DWORD __stdcall ResumeThread(HANDLE);
extern BOOL __stdcall SetEvent(HANDLE);
extern BOOL __stdcall ResetEvent(HANDLE);
extern DWORD __stdcall WaitForSingleObject(HANDLE, DWORD);
extern DWORD __stdcall WaitForMultipleObjects(DWORD, const HANDLE *, BOOL, DWORD);
extern long __stdcall InterlockedExchange(long volatile *, long);
extern DWORD __stdcall GetCurrentThreadId(void);
extern DWORD __stdcall TlsAlloc(void);
extern LPVOID __stdcall TlsGetValue(DWORD);
extern BOOL __stdcall TlsSetValue(DWORD, LPVOID);
extern void Com_InitThreadData(int threadContext);

#    define WAIT_TIMEOUT 0x102u
#    define INFINITE 0xFFFFFFFFu

typedef struct {
    unsigned hwnd, msg;
    unsigned wParam;
    long lParam;
    unsigned time;
    long pt[2];
} MSG_;
extern void *__stdcall FindWindowA(const char *, const char *);
extern int __stdcall ShowWindow(void *, int);
extern int __stdcall PeekMessageA(MSG_ *, void *, unsigned, unsigned, unsigned);
extern int __stdcall TranslateMessage(const MSG_ *);
extern long __stdcall DispatchMessageA(const MSG_ *);

void *gfxv60_MaterialDup(void *srcMaterial, const char *name)
{
    (void)name;
    return srcMaterial;
}
static void *gfxv60_hwnd(void)
{
    return FindWindowA("CoD2", (void *)0);
}
void gfxv60_ShowWin(void)
{
    void *h = gfxv60_hwnd();
    if (h)
        ShowWindow(h, 5 );
}
void gfxv60_HideWin(void)
{
    void *h = gfxv60_hwnd();
    if (h)
        ShowWindow(h, 0 );
}
void gfxv60_PumpMsgs(void)
{
    MSG_ m;
    int n = 0;
    while (PeekMessageA(&m, (void *)0, 0, 0, 1 ) && ++n < 64) {
        TranslateMessage(&m);
        DispatchMessageA(&m);
    }
}

static HANDLE g_ev[4];
static HANDLE g_thread[8];
static DWORD g_threadId[8];
static DWORD g_workerId;
static DWORD g_tls;
static long g_interlock;
static long g_misc;
static LPTHREAD_START_ROUTINE g_proc[8];

extern void *__stdcall GetModuleHandleA(const char *);
extern void Com_Printf(const char *, ...);
static void *g_gfx_rip;
static unsigned long g_gfx_code;
static int gfx_seh_filt(void *ep_v)
{
    void **ep = (void **)ep_v;       /* EXCEPTION_POINTERS: [0]=ExceptionRecord [1]=ContextRecord */
    void **rec = (void **)ep[0];     /* EXCEPTION_RECORD: code@0, flags@4, nested@8, address@16 */
    g_gfx_code = *(unsigned long *)rec;
    g_gfx_rip = rec[2];
    return 1; /* EXCEPTION_EXECUTE_HANDLER */
}

static DWORD __stdcall gfxv60_trampoline(LPVOID arg)
{
    int slot = (int)(long)arg;

    Com_InitThreadData(1);
    __try {
        if (g_proc[slot])
            g_proc[slot]((LPVOID)0);
    } __except (gfx_seh_filt(_exception_info())) {
        Com_Printf("[GFXCRASH] render-thread fault code=0x%lx rip=%p rva=0x%llx\n",
                   g_gfx_code, g_gfx_rip,
                   (unsigned long long)((char *)g_gfx_rip - (char *)GetModuleHandleA(0)));
    }
    return 0;
}

void *gfxv60_InitWorker(void *proc)
{
    if (!g_tls)
        g_tls = TlsAlloc();

    if (g_thread[1])
        return (void *)1;
    g_ev[0] = CreateEventA(0, 0, 0, 0);
    g_ev[1] = CreateEventA(0, 1, 1, 0);
    g_ev[2] = CreateEventA(0, 1, 0, 0);
    g_ev[3] = CreateEventA(0, 0, 0, 0);
    g_proc[1] = (LPTHREAD_START_ROUTINE)proc;
    g_thread[1] = CreateThread(0, 0, gfxv60_trampoline, (LPVOID)1, 4 , &g_threadId[1]);
    g_workerId = g_threadId[1];
    return (void *)(intptr_t)(g_thread[1] != 0);
}

void gfxv60_ResumeWorker(int i)
{
    (void)i;
    if (g_thread[1])
        ResumeThread(g_thread[1]);
}

DWORD gfxv60_CurThreadId(void)
{
    return GetCurrentThreadId();
}

int gfxv60_IsWorker(void)
{
    return (GetCurrentThreadId() < g_workerId) ? 0 : 1;
}

extern void *calloc(unsigned, unsigned);
void *gfxv60_GetThreadData(void)
{
    void *p;
    if (!g_tls)
        g_tls = TlsAlloc();
    p = TlsGetValue(g_tls);
    if (!p) {
        p = calloc(1, 0x40000);
        TlsSetValue(g_tls, p);
    }
    return p;
}

void gfxv60_m114(void)
{
    if (WaitForSingleObject(g_ev[2], 0) != WAIT_TIMEOUT) {
        ResetEvent(g_ev[2]);
        SetEvent(g_ev[3]);
        WaitForSingleObject(g_ev[0], INFINITE);
    }
}

void gfxv60_m115(void)
{
    SetEvent(g_ev[2]);
}

void gfxv60_m116(long command)
{
    ResetEvent(g_ev[1]);
    g_misc = command;
    SetEvent(g_ev[3]);
}

void gfxv60_m117(void)
{
    WaitForSingleObject(g_ev[1], INFINITE);
}

void gfxv60_m118(void)
{
    SetEvent(g_ev[0]);
    WaitForSingleObject(g_ev[2], INFINITE);
}

void gfxv60_m119(void)
{
    InterlockedExchange(&g_misc, 0);
}

int gfxv60_m120(void)
{
    return WaitForSingleObject(g_ev[2], 0);
}

void gfxv60_m121(void)
{
    WaitForMultipleObjects(1, &g_ev[3], 0, INFINITE);
}

void gfxv60_m122(void)
{
    SetEvent(g_ev[1]);
}

extern void *malloc(unsigned int);
extern void Com_Error(errorParm_t code, const char *fmt, ...);
static unsigned char *g_v60HiArena;
static unsigned g_v60HiOff;
static unsigned g_v60HiPeak;
#    define GFXV60_HI_ARENA_SIZE (64u * 1024u * 1024u)
static void gfxv60_HiEnsure(void)
{
    if (!g_v60HiArena) {
        g_v60HiArena = (unsigned char *)malloc(GFXV60_HI_ARENA_SIZE);
        if (!g_v60HiArena)
            Com_Error(1, "gfxv60: high-temp arena alloc (%u) failed", GFXV60_HI_ARENA_SIZE);
    }
}

unsigned gfxv60_HiAlloc(unsigned size)
{
    unsigned off;
    gfxv60_HiEnsure();
    off = g_v60HiOff;
    g_v60HiOff += (size + 3u) & ~3u;
    if (g_v60HiOff > g_v60HiPeak)
        g_v60HiPeak = g_v60HiOff;
    if (g_v60HiOff > GFXV60_HI_ARENA_SIZE)
        Com_Error(1, "gfxv60: high-temp arena overflow (%u > %u)", g_v60HiOff, GFXV60_HI_ARENA_SIZE);
    return off;
}

void gfxv60_HiSetOffset(unsigned off)
{
    g_v60HiOff = off;
}

void *gfxv60_HiOffToPtr(unsigned off)
{
    gfxv60_HiEnsure();
    return g_v60HiArena + off;
}

#endif
