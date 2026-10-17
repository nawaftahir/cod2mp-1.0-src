
#if defined(_WIN32) || defined(COD2_VC71_COMPAT_H)

#    include <stdio.h>
#    include <string.h>
#    include <float.h>

#    if defined(COD2_VC71_COMPAT_H)
#        include "common_types.h"
#    else
typedef int qboolean;
typedef unsigned char Boolean;
#        define qfalse 0
#        define qtrue 1
#        define ERR_FATAL 0
#    endif
#    include "headers/PC/win32/win32_syscon_api.h"
typedef int fileHandle_t;
#    include "../../universal/dvar.h"

typedef enum
{
    SE_NONE = 0,
    SE_KEY,
    SE_CHAR,
    SE_MOUSE,
    SE_CONSOLE,
    SE_PACKET
} sysEventType_t;

#    define SYSCON_CLASS "CoD2 WinConsole"
#    define SYSCON_TITLE "CoD2 Console"
#    define SYSCON_FONT "Courier New"
#    define SYSCON_LOGO "codlogo.bmp"

#    define ERRORBOX_ID 10
#    define ERRORTEXT_ID 11
#    define QUIT_ID 12
#    define CLEAR_ID 13
#    define COPY_ID 14
#    define EDIT_ID 100
#    define INPUT_ID 101

extern void Sys_Error(const char *error, ...);
extern void Com_Printf(const char *fmt, ...);
extern void Cbuf_AddText(const char *text);
extern void Sys_Quit(void);
extern void Sys_QueEvent(int time, sysEventType_t type, int value, int value2, int ptrLength, void *ptr);
extern dvar_t *com_dedicated;
extern dvar_t *com_viewlog;
extern char *CopyStringInternal(const char *in);
extern char *va(const char *format, ...);

void Sys_CreateConsole(HINSTANCE hInstance);
void Sys_DestroyConsole(void);
void Sys_ShowConsole(int visLevel, qboolean quitOnClose);
char *Sys_ConsoleInput(void);
void Conbuf_AppendText(const char *pMsg);
void Sys_SetErrorText(const char *buf);
const char *Win_LocalizeRef(const char *ref);

extern Boolean gConsoleRunning;

typedef struct {
    HWND hWnd;
    HWND hwndBuffer;
    HWND hwndLogo;
    HBRUSH hbrEditBackground;
    HBRUSH hbrErrorBackground;
    HFONT hfBufferFont;
    HWND hwndInputLine;
    char errorString[512];
    char consoleText[512];
    char returnedText[512];
    int windowVisLevel;
    qboolean quitOnClose;
    int windowWidth, windowHeight;
    WNDPROC SysInputLineWndProc;
} WinConData;

static WinConData s_wcd;

#    define CONSOLE_BUFFER_SIZE 16384

static LONG WINAPI ConWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    char *cmdString;
    int bufferHeight;
    int bufferWidth;
    int width;
    int height;

    switch (uMsg) {
    case WM_SIZE:
        width = LOWORD(lParam);
        height = HIWORD(lParam);
        bufferWidth = width - 15;
        bufferHeight = height - 100;
        SetWindowPos(s_wcd.hwndBuffer, 0, 5, 70, bufferWidth, bufferHeight, 0);
        SetWindowPos(s_wcd.hwndInputLine, 0, 5, 70 + bufferHeight + 8, bufferWidth, 20, 0);
        s_wcd.windowWidth = width;
        s_wcd.windowHeight = height;
        break;

    case WM_ACTIVATE:
        if (LOWORD(wParam) != WA_INACTIVE)
            SetFocus(s_wcd.hwndInputLine);

        if (com_dedicated && !com_dedicated->current.integer && com_viewlog) {
            // if the viewlog is open, check to see if it's being minimized
            if (com_viewlog->current.integer == 1) {
                if (HIWORD(wParam))
                    Dvar_SetInt(com_viewlog, 2);
            } else if (com_viewlog->current.integer == 2) {
                if (!HIWORD(wParam))
                    Dvar_SetInt(com_viewlog, 1);
            }
        }
        break;

    case WM_CLOSE:
        if (com_dedicated && com_dedicated->current.integer) {
            cmdString = CopyStringInternal("quit");
            Sys_QueEvent(0, SE_CONSOLE, 0, 0, strlen(cmdString) + 1, cmdString);
        } else if (s_wcd.quitOnClose) {
            PostQuitMessage(0);
        } else {
            Sys_ShowConsole(0, qfalse);
            Dvar_SetInt(com_viewlog, 0);
        }
        return 0;

    case WM_CREATE:
        s_wcd.hbrEditBackground = CreateSolidBrush(RGB(0x56, 0x75, 0x64));
        s_wcd.hbrErrorBackground = CreateSolidBrush(RGB(0x80, 0x80, 0x80));
        break;

    case WM_ERASEBKGND:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

static LONG WINAPI InputLineWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    char inputBuffer[1024];

    switch (uMsg) {
    case WM_KILLFOCUS:
        if ((HWND)wParam == s_wcd.hWnd) {
            SetFocus(hWnd);
            return 0;
        }
        break;

    case WM_CHAR:
        if (wParam == 13) {
            GetWindowText(s_wcd.hwndInputLine, inputBuffer, sizeof(inputBuffer));
            strncat(s_wcd.consoleText, inputBuffer, sizeof(s_wcd.consoleText) - strlen(s_wcd.consoleText) - 5);
            strcat(s_wcd.consoleText, "\n");
            SetWindowText(s_wcd.hwndInputLine, "");
            Conbuf_AppendText(va("]%s\n", inputBuffer));

            return 0;
        }
        break;
    }

    return CallWindowProc(s_wcd.SysInputLineWndProc, hWnd, uMsg, wParam, lParam);
}

void Sys_CreateConsole(HINSTANCE hInstance)
{
    WNDCLASS wc;
    RECT rect;
    HDC hDC;
    HBITMAP hbmLogo;
    int nHeight;
    int swidth, sheight;
    int DEDSTYLE = WS_POPUPWINDOW | WS_CAPTION | WS_MINIMIZEBOX;

    memset(&wc, 0, sizeof(wc));
    wc.style = 0;
    wc.lpfnWndProc = (WNDPROC)ConWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(1));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpszMenuName = 0;
    wc.lpszClassName = SYSCON_CLASS;

    if (!RegisterClass(&wc))
        return;

    rect.left = 0;
    rect.right = 620;
    rect.top = 0;
    rect.bottom = 450;
    AdjustWindowRect(&rect, DEDSTYLE, FALSE);

    hDC = GetDC(GetDesktopWindow());
    swidth = GetDeviceCaps(hDC, HORZRES);
    sheight = GetDeviceCaps(hDC, VERTRES);
    ReleaseDC(GetDesktopWindow(), hDC);

    s_wcd.windowWidth = rect.right - rect.left + 1;
    s_wcd.windowHeight = rect.bottom - rect.top + 1;

    s_wcd.hWnd = CreateWindowEx(0, SYSCON_CLASS, SYSCON_TITLE, DEDSTYLE,
                                (swidth - 600) / 2, (sheight - 450) / 2,
                                rect.right - rect.left + 1, rect.bottom - rect.top + 1,
                                NULL, NULL, hInstance, NULL);

    _controlfp(_PC_24, _MCW_PC);
    _controlfp(_MCW_EM, _MCW_EM);

    if (s_wcd.hWnd == NULL)
        return;

    hDC = GetDC(s_wcd.hWnd);
    nHeight = -MulDiv(8, GetDeviceCaps(hDC, LOGPIXELSY), 72);
    s_wcd.hfBufferFont = CreateFont(nHeight, 0, 0, 0, FW_LIGHT, 0, 0, 0, DEFAULT_CHARSET,
                                    OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                                    FF_MODERN | FIXED_PITCH, SYSCON_FONT);
    ReleaseDC(s_wcd.hWnd, hDC);

    hbmLogo = (HBITMAP)LoadImageA(NULL, SYSCON_LOGO, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    if (hbmLogo) {
        s_wcd.hwndLogo = CreateWindowEx(0, "Static", NULL, WS_CHILD | WS_VISIBLE | SS_BITMAP,
                                        5, 5, 0, 0, s_wcd.hWnd, (HMENU)1, hInstance, NULL);
        SendMessage(s_wcd.hwndLogo, STM_SETIMAGE, 0, (LPARAM)hbmLogo);
    }

    s_wcd.hwndInputLine = CreateWindowEx(0, "edit", NULL,
                                         WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                                         6, 400, 608, 20, s_wcd.hWnd, (HMENU)INPUT_ID,
                                         hInstance, NULL);

    s_wcd.hwndBuffer = CreateWindowEx(0, "edit", NULL,
                                      WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_BORDER |
                                          ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
                                      6, 70, 606, 324, s_wcd.hWnd, (HMENU)EDIT_ID,
                                      hInstance, NULL);
    SendMessage(s_wcd.hwndBuffer, WM_SETFONT, (WPARAM)s_wcd.hfBufferFont, 0);

    s_wcd.SysInputLineWndProc =
        (WNDPROC)SetWindowLongPtr(s_wcd.hwndInputLine, GWLP_WNDPROC, (LONG_PTR)InputLineWndProc);
    SendMessage(s_wcd.hwndInputLine, WM_SETFONT, (WPARAM)s_wcd.hfBufferFont, 0);
    SetFocus(s_wcd.hwndInputLine);

    s_wcd.windowVisLevel = 0;
}

void Sys_DestroyConsole(void)
{
    if (s_wcd.hWnd) {
        ShowWindow(s_wcd.hWnd, SW_HIDE);
        CloseWindow(s_wcd.hWnd);
        DestroyWindow(s_wcd.hWnd);
        s_wcd.hWnd = 0;
    }
}

void Sys_ShowConsole(int visLevel, qboolean quitOnClose)
{
    s_wcd.quitOnClose = quitOnClose;

    if (visLevel == s_wcd.windowVisLevel)
        return;

    s_wcd.windowVisLevel = visLevel;

    if (!s_wcd.hWnd)
        return;

    switch (visLevel) {
    case 0:
        ShowWindow(s_wcd.hWnd, SW_HIDE);
        break;
    case 1:
        ShowWindow(s_wcd.hWnd, SW_SHOWNORMAL);
        SendMessage(s_wcd.hwndBuffer, EM_LINESCROLL, 0, 0xffff);
        break;
    case 2:
        ShowWindow(s_wcd.hWnd, SW_MINIMIZE);
        break;
    default:

        Sys_Error("Invalid visLevel %d sent to Sys_ShowConsole\n", visLevel);
        break;
    }
}

char *Sys_ConsoleInput(void)
{
    if (s_wcd.consoleText[0] == '\0')
        return NULL;

    strcpy(s_wcd.returnedText, s_wcd.consoleText);
    s_wcd.consoleText[0] = '\0';
    return s_wcd.returnedText;
}

// Q_IsColorString plus the CoD2 digit check.
#define I_IsColorString(p) ((p) && *(p) == '^' && *((p) + 1) && *((p) + 1) != '^' && *((p) + 1) >= '0' && *((p) + 1) <= '9')

void Conbuf_AppendText(const char *pMsg)
{
    char buffer[CONSOLE_BUFFER_SIZE * 2];
    char *b = buffer;
    const char *msg;
    int bufLen;
    int i = 0;
    static unsigned long s_totalChars;

    if (strlen(pMsg) > CONSOLE_BUFFER_SIZE - 1)
        msg = pMsg + strlen(pMsg) - CONSOLE_BUFFER_SIZE + 1;
    else
        msg = pMsg;

    while (msg[i] && ((b - buffer) < sizeof(buffer) - 1)) {
        if (msg[i] == '\n' && msg[i + 1] == '\r') {
            b[0] = '\r';
            b[1] = '\n';
            b += 2;
            i++;
        } else if (msg[i] == '\r') {
            b[0] = '\r';
            b[1] = '\n';
            b += 2;
        } else if (msg[i] == '\n') {
            b[0] = '\r';
            b[1] = '\n';
            b += 2;
        } else if (I_IsColorString(&msg[i])) {
            i++;
        } else {
            *b = msg[i];
            b++;
        }
        i++;
    }
    *b = 0;
    bufLen = b - buffer;

    s_totalChars += bufLen;

    if (s_totalChars > CONSOLE_BUFFER_SIZE) {
        SendMessageA(s_wcd.hwndBuffer, EM_SETSEL, 0, -1);
        s_totalChars = bufLen;
    } else {
        SendMessageA(s_wcd.hwndBuffer, EM_SETSEL, 0xFFFF, 0xFFFF);
    }

    SendMessageA(s_wcd.hwndBuffer, EM_LINESCROLL, 0, 0xffff);
    SendMessageA(s_wcd.hwndBuffer, EM_SCROLLCARET, 0, 0);
    SendMessageA(s_wcd.hwndBuffer, EM_REPLACESEL, 0, (LPARAM)buffer);
}

void Sys_SetErrorText(const char *buf)
{
    strncpy(s_wcd.errorString, buf, sizeof(s_wcd.errorString) - 1);
    s_wcd.errorString[sizeof(s_wcd.errorString) - 1] = '\0';

    DestroyWindow(s_wcd.hwndInputLine);
    s_wcd.hwndInputLine = NULL;
    MessageBoxA(NULL, buf, Win_LocalizeRef("WIN_ERROR"), MB_ICONERROR);
}

#endif
