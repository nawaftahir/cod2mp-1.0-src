
#ifndef COD2_WIN32_SYSCON_API_H
#define COD2_WIN32_SYSCON_API_H

#if defined(COD2_VC71_COMPAT_H)

typedef unsigned int WPARAM;
typedef long LPARAM;
typedef unsigned long COLORREF;

struct HMENU__;
typedef struct HMENU__ *HMENU;
struct HICON__;
typedef struct HICON__ *HICON;
struct HBRUSH__;
typedef struct HBRUSH__ *HBRUSH;
struct HFONT__;
typedef struct HFONT__ *HFONT;
struct HBITMAP__;
typedef struct HBITMAP__ *HBITMAP;
struct HGDIOBJ__;
typedef struct HGDIOBJ__ *HGDIOBJ;

typedef LRESULT(__stdcall *WNDPROC)(HWND, UINT, WPARAM, LPARAM);

typedef struct tagWNDCLASSA {
    UINT style;
    WNDPROC lpfnWndProc;
    int cbClsExtra;
    int cbWndExtra;
    HINSTANCE hInstance;
    HICON hIcon;
    HCURSOR hCursor;
    HBRUSH hbrBackground;
    LPCSTR lpszMenuName;
    LPCSTR lpszClassName;
} WNDCLASSA, *LPWNDCLASSA;
#    define WNDCLASS WNDCLASSA

BM_EXTERN_C
{
    __declspec(dllimport) short __stdcall RegisterClassA(const WNDCLASSA *);
    __declspec(dllimport) HWND __stdcall CreateWindowExA(DWORD, LPCSTR, LPCSTR, DWORD, int, int, int, int,
                                                         HWND, HMENU, HINSTANCE, void *);
    __declspec(dllimport) LRESULT __stdcall DefWindowProcA(HWND, UINT, WPARAM, LPARAM);
    __declspec(dllimport) LRESULT __stdcall SendMessageA(HWND, UINT, WPARAM, LPARAM);
    __declspec(dllimport) LRESULT __stdcall CallWindowProcA(WNDPROC, HWND, UINT, WPARAM, LPARAM);
    __declspec(dllimport) long __stdcall SetWindowLongA(HWND, int, long);
    __declspec(dllimport) BOOL __stdcall ShowWindow(HWND, int);
    __declspec(dllimport) BOOL __stdcall UpdateWindow(HWND);
    __declspec(dllimport) BOOL __stdcall DestroyWindow(HWND);
    __declspec(dllimport) BOOL __stdcall CloseWindow(HWND);
    __declspec(dllimport) HWND __stdcall SetFocus(HWND);
    __declspec(dllimport) BOOL __stdcall SetForegroundWindow(HWND);
    __declspec(dllimport) void __stdcall PostQuitMessage(int);
    __declspec(dllimport) int __stdcall GetWindowTextA(HWND, LPSTR, int);
    __declspec(dllimport) BOOL __stdcall SetWindowTextA(HWND, LPCSTR);
    __declspec(dllimport) int __stdcall GetWindowTextLengthA(HWND);
    __declspec(dllimport) HICON __stdcall LoadIconA(HINSTANCE, LPCSTR);
    __declspec(dllimport) HCURSOR __stdcall LoadCursorA(HINSTANCE, LPCSTR);
    __declspec(dllimport) HBITMAP __stdcall LoadBitmapA(HINSTANCE, LPCSTR);
    __declspec(dllimport) HDC __stdcall GetDC(HWND);
    __declspec(dllimport) int __stdcall ReleaseDC(HWND, HDC);
    __declspec(dllimport) BOOL __stdcall AdjustWindowRect(RECT *, DWORD, BOOL);
    __declspec(dllimport) int __stdcall GetSystemMetrics(int);
    __declspec(dllimport) HGDIOBJ __stdcall GetStockObject(int);
    __declspec(dllimport) COLORREF __stdcall SetBkColor(HDC, COLORREF);
    __declspec(dllimport) COLORREF __stdcall SetTextColor(HDC, COLORREF);
    __declspec(dllimport) int __stdcall GetDeviceCaps(HDC, int);
    __declspec(dllimport) HFONT __stdcall CreateFontA(int, int, int, int, int, DWORD, DWORD, DWORD,
                                                      DWORD, DWORD, DWORD, DWORD, DWORD, LPCSTR);
    __declspec(dllimport) HINSTANCE __stdcall GetModuleHandleA(LPCSTR);
    __declspec(dllimport) int __stdcall MulDiv(int, int, int);
    __declspec(dllimport) HWND __stdcall GetDesktopWindow(void);
    __declspec(dllimport) void *__stdcall LoadImageA(HINSTANCE, LPCSTR, UINT, int, int, UINT);
}

#    define RegisterClass RegisterClassA

#    define CreateWindowEx CreateWindowExA
#    define CreateWindow(cls, name, style, x, y, w, h, parent, menu, inst, param) \
        CreateWindowExA(0L, cls, name, style, x, y, w, h, parent, menu, inst, param)
#    define DefWindowProc DefWindowProcA
#    define SendMessage SendMessageA
#    define CallWindowProc CallWindowProcA
#    define GetWindowText GetWindowTextA
#    define SetWindowText SetWindowTextA
#    define GetWindowTextLength GetWindowTextLengthA
#    define LoadIcon LoadIconA
#    define LoadCursor LoadCursorA
#    define LoadBitmap LoadBitmapA
#    define CreateFont CreateFontA
#    define GetModuleHandle GetModuleHandleA
#    define SetWindowLongPtr SetWindowLongA
#    define LONG_PTR long
#    define GWLP_WNDPROC (-4)
#    define MAKEINTRESOURCE(i) ((LPCSTR)((DWORD)((unsigned short)(i))))
#    define WINAPI __stdcall
#    ifndef qfalse
#        define qfalse 0
#        define qtrue 1
#    endif
#    ifndef ERR_FATAL
#        define ERR_FATAL 0
#    endif
#    define FALSE 0
#    define TRUE 1
#    define RGB(r, g, b) ((COLORREF)(((unsigned char)(r) | ((unsigned short)((unsigned char)(g)) << 8)) | (((DWORD)(unsigned char)(b)) << 16)))
#    define LOWORD(l) ((unsigned short)((DWORD)(l) & 0xffff))

#    define WS_CHILD 0x40000000L
#    define WS_VISIBLE 0x10000000L
#    define WS_VSCROLL 0x00200000L
#    define WS_BORDER 0x00800000L
#    define WS_CAPTION 0x00C00000L
#    define WS_MINIMIZEBOX 0x00020000L
#    define WS_TABSTOP 0x00010000L
#    define WS_POPUP 0x80000000L
#    define WS_SYSMENU 0x00080000L
#    define WS_POPUPWINDOW (WS_POPUP | WS_BORDER | WS_SYSMENU)

#    define ES_LEFT 0x0000L
#    define ES_MULTILINE 0x0004L
#    define ES_AUTOVSCROLL 0x0040L
#    define ES_AUTOHSCROLL 0x0080L
#    define ES_READONLY 0x0800L
#    define BS_PUSHBUTTON 0x00000000L
#    define SS_SUNKEN 0x00001000L
#    define SS_BITMAP 0x0000000EL

#    define WM_CREATE 0x0001
#    define WM_DESTROY 0x0002
#    define WM_CLOSE 0x0010
#    define WM_ACTIVATE 0x0006
#    define WM_SETTEXT 0x000C
#    define WM_SETFONT 0x0030
#    define WM_COMMAND 0x0111
#    define WM_CHAR 0x0102
#    define WM_KILLFOCUS 0x0008
#    define WM_COPY 0x0301
#    define WM_CTLCOLORSTATIC 0x0138
#    define EM_SETSEL 0x00B1
#    define EM_REPLACESEL 0x00C2
#    define EM_LINESCROLL 0x00B6
#    define EM_SCROLLCARET 0x00B7
#    define EM_SETREADONLY 0x00CF
#    define STM_SETIMAGE 0x0172

#    define SW_HIDE 0
#    define SW_SHOWNORMAL 1
#    define SW_MINIMIZE 6
#    define SW_SHOWDEFAULT 10
#    define WA_INACTIVE 0

#    define FW_LIGHT 300
#    define DEFAULT_CHARSET 1
#    define OUT_DEFAULT_PRECIS 0
#    define CLIP_DEFAULT_PRECIS 0
#    define DEFAULT_QUALITY 0
#    define FIXED_PITCH 1
#    define FF_MODERN 48
#    define BLACK_BRUSH 4
#    define LOGPIXELSY 90
#    define HORZRES 8
#    define VERTRES 10
#    define IMAGE_BITMAP 0
#    define LR_LOADFROMFILE 0x0010
#    define SM_CXSCREEN 0
#    define SM_CYSCREEN 1
#    define COLOR_WINDOW 5
#    define IDC_ARROW MAKEINTRESOURCE(32512)

#else

#    include <windows.h>
#endif

#endif
