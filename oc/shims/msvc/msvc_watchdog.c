/* MSVC-build freeze watchdog.
 *
 * The reconstructed engine can hang (e.g. map load) with no crash/exit, so a
 * debugger-attach is needed to see where. cdb isn't always installed, so this
 * acts as an in-process debugger: a background thread periodically suspends the
 * main thread and walks its stack (dbghelp, already linked) into
 * cod2_msvc_freeze.txt. When the engine is frozen, successive dumps show the
 * same stack -- that's the hang site. Isolated TU (includes <windows.h> but no
 * engine headers). Compiled only in the MSVC build.
 */
#ifdef _MSC_VER
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <dbghelp.h>
#include <stdio.h>
#pragma comment(lib, "dbghelp.lib")

/* CONTEXT register names and the StackWalk image type differ by arch. */
#if defined(_M_X64)
#define WD_PC       Rip
#define WD_FRAME    Rbp
#define WD_STACK    Rsp
#define WD_MACHINE  IMAGE_FILE_MACHINE_AMD64
#else
#define WD_PC       Eip
#define WD_FRAME    Ebp
#define WD_STACK    Esp
#define WD_MACHINE  IMAGE_FILE_MACHINE_I386
#endif

static DWORD g_main_tid;

static DWORD WINAPI cod2_watchdog(LPVOID unused)
{
    HANDLE proc = GetCurrentProcess();
    HANDLE main = OpenThread(THREAD_GET_CONTEXT | THREAD_SUSPEND_RESUME | THREAD_QUERY_INFORMATION,
                             FALSE, g_main_tid);
    (void)unused;
    if (!main) return 0;
    SymSetOptions(SYMOPT_LOAD_LINES | SYMOPT_DEFERRED_LOADS | SYMOPT_UNDNAME);
    SymInitialize(proc, NULL, TRUE);

    for (;;) {
        CONTEXT ctx;
        STACKFRAME64 sf;
        FILE *f;
        int i;
        Sleep(5000);

        SuspendThread(main);
        memset(&ctx, 0, sizeof(ctx));
        ctx.ContextFlags = CONTEXT_FULL;
        if (!GetThreadContext(main, &ctx)) { ResumeThread(main); continue; }

        memset(&sf, 0, sizeof(sf));
        sf.AddrPC.Offset    = ctx.WD_PC;    sf.AddrPC.Mode    = AddrModeFlat;
        sf.AddrFrame.Offset = ctx.WD_FRAME; sf.AddrFrame.Mode = AddrModeFlat;
        sf.AddrStack.Offset = ctx.WD_STACK; sf.AddrStack.Mode = AddrModeFlat;

        f = fopen("cod2_msvc_freeze.txt", "w");
        if (f) {
            fprintf(f, "main-thread stack (tid %lu) pc=%p\n",
                    g_main_tid, (void *)(DWORD_PTR)ctx.WD_PC);
            for (i = 0; i < 48; i++) {
                char b[sizeof(SYMBOL_INFO) + 260];
                SYMBOL_INFO *si = (SYMBOL_INFO *)b;
                DWORD64 disp = 0;
                IMAGEHLP_LINE64 line; DWORD ld = 0;
                if (!StackWalk64(WD_MACHINE, proc, main, &sf, &ctx, NULL,
                                 SymFunctionTableAccess64, SymGetModuleBase64, NULL))
                    break;
                if (!sf.AddrPC.Offset) break;
                si->SizeOfStruct = sizeof(SYMBOL_INFO); si->MaxNameLen = 255;
                line.SizeOfStruct = sizeof(line);
                if (SymFromAddr(proc, sf.AddrPC.Offset, &disp, si)) {
                    if (SymGetLineFromAddr64(proc, sf.AddrPC.Offset, &ld, &line))
                        fprintf(f, "  #%-2d %s+0x%llx  (%s:%lu)\n", i, si->Name, disp,
                                line.FileName, line.LineNumber);
                    else
                        fprintf(f, "  #%-2d %s+0x%llx\n", i, si->Name, disp);
                } else {
                    fprintf(f, "  #%-2d 0x%08llx\n", i, sf.AddrPC.Offset);
                }
            }
            fclose(f);
        }
        ResumeThread(main);
    }
}

COD2_CONSTRUCTOR(cod2_start_watchdog)
{
    if (GetEnvironmentVariableA("COD2_WATCHDOG", NULL, 0)) {
        g_main_tid = GetCurrentThreadId();
        CloseHandle(CreateThread(NULL, 0, cod2_watchdog, NULL, 0, NULL));
    }
}
#endif
