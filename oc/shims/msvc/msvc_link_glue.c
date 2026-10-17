/* MSVC link glue for the cod2_win32 executable.
 *
 * Provides symbols the GNU build supplied via linker --defsym, and the Mach-O
 * image-base symbol the reconstructed data blob references. Compiled only in the
 * MSVC build (added to the source set in win32-msvc.cmake).
 */
#ifdef _MSC_VER

/* The data blob references `&__mh_execute_header + offset` as interior image
 * pointers. The GNU build defined it as the absolute image base (0x1000); here
 * we give it real backing storage so those address-constants resolve. Runtime
 * correctness of the exact offsets is a later concern (Stage 6 boot). */
__declspec(align(4096)) char __mh_execute_header[0x10000];

/* /alternatename directives for every blob/stub __attribute__((alias)) pointer
 * (no-op'd by the GCC-compat shim, so otherwise unresolved). */
#include "msvc_alias_pragmas.h"

/* Engine-global seam aliases (the GNU build's --defsym engine seams). */
#include "msvc_seam_aliases.h"

/* --wrap,R_Error: the engine references __real_R_Error (the un-wrapped original).
 * Without the wrap we just bind it straight to R_Error. */
COD2_ALT("__real_R_Error", "R_Error")

/* --- runtime diagnostics --------------------------------------------------
 * The CRT "Visual C++ Runtime Library" dialog (pure-virtual R6025, invalid
 * params, abort()/assert/terminate R6010) fires OUTSIDE the engine's SEH crash
 * handler -> no cod2_crash_*.txt. Capture a stack (exe-relative, symbolized via
 * the linker .map) to cod2_msvc_runtime.txt instead. */
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <crtdbg.h>
extern char __ImageBase;   /* linker-provided image base */
unsigned short __stdcall RtlCaptureStackBackTrace(unsigned long, unsigned long, void **, unsigned long *);

static void cod2_diag_stack(const char *what)
{
    void *fr[28];
    unsigned short n = RtlCaptureStackBackTrace(0, 28, fr, 0), i;
    FILE *f = fopen("cod2_msvc_runtime.txt", "a");
    if (f) {
        fprintf(f, "=== %s ===\n", what);
        for (i = 0; i < n; i++)
            fprintf(f, "  exe+0x%X\n", (unsigned)((char *)fr[i] - (char *)&__ImageBase));
        fclose(f);
    }
}
static void cod2_purecall(void) { cod2_diag_stack("PURECALL (R6025)"); }
static void cod2_invparam(const wchar_t *e, const wchar_t *fn, const wchar_t *f,
                          unsigned line, uintptr_t r)
{ (void)e;(void)fn;(void)f;(void)line;(void)r; cod2_diag_stack("INVALID_PARAMETER"); }
static void cod2_sigabrt(int s) { (void)s; cod2_diag_stack("SIGABRT (abort/assert/terminate)"); _exit(3); }

/* Debug-CRT _CrtDbgReport (asserts, _CRT_ERROR) defaults to a dialog window;
 * hook it to log the message text + stack and suppress the dialog. */
static int cod2_report_hook(int type, char *msg, int *ret)
{
    FILE *f = fopen("cod2_msvc_runtime.txt", "a");
    if (f) { fprintf(f, "CRT_REPORT[%d]: %s\n", type, msg ? msg : "(null)"); fclose(f); }
    cod2_diag_stack("CRT_REPORT");
    if (ret) *ret = 0;   /* don't break to debugger */
    return 1;            /* handled -> no dialog */
}

COD2_CONSTRUCTOR(cod2_install_diag)   /* _set_*_handler come from <stdlib.h> */
{
    FILE *f = fopen("cod2_msvc_runtime.txt", "w");
    if (f) { fputs("diag installed\n", f); fclose(f); }
    _set_purecall_handler(cod2_purecall);
    _set_invalid_parameter_handler(cod2_invparam);
    signal(SIGABRT, cod2_sigabrt);
    /* suppress abort()'s modal "Runtime Library" message box so it proceeds
     * straight to raise(SIGABRT) -> our handler captures the stack. */
    _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
    /* route debug-CRT reports to our hook (+ file) instead of a dialog. */
    _CrtSetReportHook(cod2_report_hook);
    _CrtSetReportMode(_CRT_WARN,   _CRTDBG_MODE_FILE);
    _CrtSetReportMode(_CRT_ERROR,  _CRTDBG_MODE_FILE);
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
}

/* C++ Itanium-ABI exception entry points referenced by the reconstructed
 * (decompiled-as-C) renderer. Log the throw site before aborting. */
void *__cxa_allocate_exception(unsigned long thrown_size) { (void)thrown_size; return 0; }
void  __cxa_throw(void *thrown, void *type, void (*destructor)(void *))
{ (void)thrown; (void)type; (void)destructor; cod2_diag_stack("__cxa_throw (C++ exception)"); abort(); }

#endif
