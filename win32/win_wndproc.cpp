#include "../qcommon/qcommon.h"
#include "win_local.h"

#define K_MWHEELDOWN 205
#define K_MWHEELUP 206

// Windows 2000+ messages, hidden by the SDK at the default _WIN32_WINNT
#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL 0x020A
#endif
#ifndef WM_XBUTTONDOWN
#define WM_XBUTTONDOWN 0x020B
#define WM_XBUTTONUP 0x020C
#define MK_XBUTTON1 0x0020
#define MK_XBUTTON2 0x0040
#endif

// Leading members only; the full layouts live in oc/headers/common_types.h.
enum { CA_LOADING = 6 };
struct clientConnection_t
{
	int state;
};
struct clientStatic_t
{
	qboolean quit;
};
extern clientConnection_t clientConnections[1];
#define clc clientConnections[0]
extern clientStatic_t cls;

void Key_ClearStates( void );
void Com_TouchMemory( void );
void SND_SetHWND( HWND hwnd );

static dvar_t *vid_xpos;
static dvar_t *vid_ypos;
dvar_t *r_fullscreen;
static dvar_t *r_autopriority;

static UINT MSH_MOUSEWHEEL;

// Windows virtual key -> engine key, indexed by [vk][extended].
static byte virtualKeyConvert[0x92][2] =
{
	{ 0, 0 },         // 00
	{ 200, 200 },     // VK_LBUTTON
	{ 201, 201 },     // VK_RBUTTON
	{ 0, 0 },         // VK_CANCEL
	{ 202, 202 },     // VK_MBUTTON
	{ 203, 203 },     // VK_XBUTTON1
	{ 204, 204 },     // VK_XBUTTON2
	{ 0, 0 },         // 07
	{ 127, 127 },     // VK_BACK
	{ 9, 9 },         // VK_TAB
	{ 0, 0 },         // 0A
	{ 0, 0 },         // 0B
	{ 186, 0 },       // VK_CLEAR
	{ 13, 191 },      // VK_RETURN
	{ 0, 0 },         // 0E
	{ 0, 0 },         // 0F
	{ 160, 160 },     // VK_SHIFT
	{ 159, 159 },     // VK_CONTROL
	{ 158, 158 },     // VK_MENU
	{ 153, 153 },     // VK_PAUSE
	{ 151, 151 },     // VK_CAPITAL
	{ 0, 0 },         // 15
	{ 0, 0 },         // 16
	{ 0, 0 },         // 17
	{ 0, 0 },         // 18
	{ 0, 0 },         // 19
	{ 0, 0 },         // 1A
	{ 27, 27 },       // VK_ESCAPE
	{ 0, 0 },         // 1C
	{ 0, 0 },         // 1D
	{ 0, 0 },         // 1E
	{ 0, 0 },         // 1F
	{ 32, 32 },       // VK_SPACE
	{ 184, 164 },     // VK_PRIOR
	{ 190, 163 },     // VK_NEXT
	{ 188, 166 },     // VK_END
	{ 182, 165 },     // VK_HOME
	{ 185, 156 },     // VK_LEFT
	{ 183, 154 },     // VK_UP
	{ 187, 157 },     // VK_RIGHT
	{ 189, 155 },     // VK_DOWN
	{ 0, 0 },         // VK_SELECT
	{ 0, 0 },         // VK_PRINT
	{ 0, 0 },         // VK_EXECUTE
	{ 0, 0 },         // VK_SNAPSHOT
	{ 192, 161 },     // VK_INSERT
	{ 193, 162 },     // VK_DELETE
	{ 0, 0 },         // VK_HELP
	{ '0', '0' },
	{ '1', '1' },
	{ '2', '2' },
	{ '3', '3' },
	{ '4', '4' },
	{ '5', '5' },
	{ '6', '6' },
	{ '7', '7' },
	{ '8', '8' },
	{ '9', '9' },
	{ 0, 0 },         // 3A
	{ 0, 0 },         // 3B
	{ 0, 0 },         // 3C
	{ 0, 0 },         // 3D
	{ 0, 0 },         // 3E
	{ 0, 0 },         // 3F
	{ 0, 0 },         // 40
	{ 'a', 'A' },
	{ 'b', 'B' },
	{ 'c', 'C' },
	{ 'd', 'D' },
	{ 'e', 'E' },
	{ 'f', 'F' },
	{ 'g', 'G' },
	{ 'h', 'H' },
	{ 'i', 'I' },
	{ 'j', 'J' },
	{ 'k', 'K' },
	{ 'l', 'L' },
	{ 'm', 'M' },
	{ 'n', 'N' },
	{ 'o', 'O' },
	{ 'p', 'P' },
	{ 'q', 'Q' },
	{ 'r', 'R' },
	{ 's', 'S' },
	{ 't', 'T' },
	{ 'u', 'U' },
	{ 'v', 'V' },
	{ 'w', 'W' },
	{ 'x', 'X' },
	{ 'y', 'Y' },
	{ 'z', 'Z' },
	{ 0, 0 },         // VK_LWIN
	{ 0, 0 },         // VK_RWIN
	{ 0, 0 },         // VK_APPS
	{ 0, 0 },         // 5E
	{ 0, 0 },         // VK_SLEEP
	{ 192, 192 },     // VK_NUMPAD0
	{ 188, 188 },     // VK_NUMPAD1
	{ 189, 189 },     // VK_NUMPAD2
	{ 190, 190 },     // VK_NUMPAD3
	{ 185, 185 },     // VK_NUMPAD4
	{ 186, 186 },     // VK_NUMPAD5
	{ 187, 187 },     // VK_NUMPAD6
	{ 182, 182 },     // VK_NUMPAD7
	{ 183, 183 },     // VK_NUMPAD8
	{ 184, 184 },     // VK_NUMPAD9
	{ 198, 198 },     // VK_MULTIPLY
	{ 196, 196 },     // VK_ADD
	{ 0, 0 },         // VK_SEPARATOR
	{ 195, 195 },     // VK_SUBTRACT
	{ 193, 193 },     // VK_DECIMAL
	{ 194, 194 },     // VK_DIVIDE
	{ 167, 167 },     // VK_F1
	{ 168, 168 },     // VK_F2
	{ 169, 169 },     // VK_F3
	{ 170, 170 },     // VK_F4
	{ 171, 171 },     // VK_F5
	{ 172, 172 },     // VK_F6
	{ 173, 173 },     // VK_F7
	{ 174, 174 },     // VK_F8
	{ 175, 175 },     // VK_F9
	{ 176, 176 },     // VK_F10
	{ 177, 177 },     // VK_F11
	{ 178, 178 },     // VK_F12
	{ 0, 0 },         // VK_F13
	{ 0, 0 },         // VK_F14
	{ 0, 0 },         // VK_F15
	{ 0, 0 },         // VK_F16
	{ 0, 0 },         // VK_F17
	{ 0, 0 },         // VK_F18
	{ 0, 0 },         // VK_F19
	{ 0, 0 },         // VK_F20
	{ 0, 0 },         // VK_F21
	{ 0, 0 },         // VK_F22
	{ 0, 0 },         // VK_F23
	{ 0, 0 },         // VK_F24
	{ 0, 0 },         // 88
	{ 0, 0 },         // 89
	{ 0, 0 },         // 8A
	{ 0, 0 },         // 8B
	{ 0, 0 },         // 8C
	{ 0, 0 },         // 8D
	{ 0, 0 },         // 8E
	{ 0, 0 },         // 8F
	{ 197, 197 },     // VK_NUMLOCK
	{ 0, 0 },         // VK_SCROLL
};

// Latin-1 characters from MapVirtualKey -> engine K_ASCII_* keys.
static byte extendedVirtualKeyConvert[21][2] =
{
	{ 0xB5, 128 },
	{ 0xBF, 129 },
	{ 0xDF, 130 },
	{ 0xE0, 131 },
	{ 0xE1, 132 },
	{ 0xE4, 133 },
	{ 0xE5, 134 },
	{ 0xE6, 135 },
	{ 0xE7, 136 },
	{ 0xE8, 137 },
	{ 0xE9, 138 },
	{ 0xEC, 139 },
	{ 0xF1, 140 },
	{ 0xF2, 141 },
	{ 0xF3, 142 },
	{ 0xF6, 143 },
	{ 0xF8, 144 },
	{ 0xF9, 145 },
	{ 0xFA, 146 },
	{ 0xFC, 147 },
	{ 0, 0 },
};

static void VID_AppActivate( BOOL fActive, BOOL minimize );
static byte MapKey( int key, unsigned int wParam );
LONG WINAPI MainWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

/*
==================
VID_AppActivate
==================
*/
static void VID_AppActivate( BOOL fActive, BOOL minimize )
{
	g_wv.isMinimized = minimize;

	Key_ClearStates();

	g_wv.activeApp = fActive && !g_wv.isMinimized;
	if ( g_wv.activeApp )
		Com_TouchMemory();

	IN_Activate( g_wv.activeApp );
}

/*
=======
MapKey

Map from windows to engine keynums
=======
*/
static byte MapKey( int key, unsigned int wParam )
{
	unsigned int result;
	int i;

	// the key left of '1' always opens the console
	if ( ( ( key >> 16 ) & 0xff ) == 0x29 )
		return '~';

	result = 0;
	if ( wParam > 0 && wParam <= 0x91 )
		result = virtualKeyConvert[wParam][( key >> 24 ) & 1];

	if ( !result )
	{
		result = MapVirtualKeyA( wParam, 2 ) & 0xff;
		if ( result > 0x7f )
		{
			for ( i = 0; extendedVirtualKeyConvert[i][0]; i++ )
			{
				if ( result == extendedVirtualKeyConvert[i][0] )
				{
					result = extendedVirtualKeyConvert[i][1];
					break;
				}
			}
		}
	}

	return result;
}

/*
====================
MainWndProc

main window procedure
====================
*/
LONG WINAPI MainWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	byte code;
	int buttons;
	RECT r;
	int style;
	HMONITOR monitor;
	MONITORINFO monitorInfo;

	SetThreadExecutionState( ES_DISPLAY_REQUIRED );

	if ( uMsg == MSH_MOUSEWHEEL )
	{
		if ( (int)wParam > 0 )
		{
			Sys_QueEvent( g_wv.sysMsgTime, SE_KEY, K_MWHEELUP, qtrue, 0, NULL );
			Sys_QueEvent( g_wv.sysMsgTime, SE_KEY, K_MWHEELUP, qfalse, 0, NULL );
		}
		else
		{
			Sys_QueEvent( g_wv.sysMsgTime, SE_KEY, K_MWHEELDOWN, qtrue, 0, NULL );
			Sys_QueEvent( g_wv.sysMsgTime, SE_KEY, K_MWHEELDOWN, qfalse, 0, NULL );
		}
		return DefWindowProcA( hWnd, uMsg, wParam, lParam );
	}

	switch ( uMsg )
	{
	case WM_CREATE:
		g_wv.hWnd = hWnd;
		SND_SetHWND( hWnd );

		vid_xpos = Dvar_RegisterInt( "vid_xpos", 3, -4096, 4096, DVAR_ARCHIVE | DVAR_CHANGEABLE_RESET );
		vid_ypos = Dvar_RegisterInt( "vid_ypos", 22, -4096, 4096, DVAR_ARCHIVE | DVAR_CHANGEABLE_RESET );
		r_fullscreen = Dvar_RegisterBool( "r_fullscreen", true, DVAR_ARCHIVE | DVAR_LATCH | DVAR_CHANGEABLE_RESET );
		r_autopriority = Dvar_RegisterBool( "r_autopriority", false, DVAR_ARCHIVE | DVAR_CHANGEABLE_RESET );

		MSH_MOUSEWHEEL = RegisterWindowMessageA( "MSWHEEL_ROLLMSG" );
		break;

	case WM_DESTROY:
		g_wv.hWnd = NULL;
		break;

	case WM_CLOSE:
		cls.quit = qtrue;
		Cbuf_AddText( "quit\n" );
		break;

	case WM_ACTIVATE:
		VID_AppActivate( LOWORD( wParam ) != WA_INACTIVE, HIWORD( wParam ) );
		if ( r_fullscreen->current.boolean )
			IN_ActivateMouse( qtrue );
		break;

	case WM_MOVE:
		if ( !r_fullscreen->current.boolean )
		{
			r.left = 0;
			r.top = 0;
			r.right = 1;
			r.bottom = 1;

			style = GetWindowLongA( hWnd, GWL_STYLE );
			AdjustWindowRect( &r, style, FALSE );

			Dvar_SetInt( vid_xpos, (short)LOWORD( lParam ) + r.left );
			Dvar_SetInt( vid_ypos, (short)HIWORD( lParam ) + r.top );
			vid_xpos->modified = false;
			vid_ypos->modified = false;

			if ( g_wv.activeApp )
				IN_Activate( qtrue );
		}
		IN_ActivateMouse( qfalse );
		break;

	case WM_KILLFOCUS:
		if ( r_autopriority->current.boolean )
		{
			SetPriorityClass( GetCurrentProcess(), IDLE_PRIORITY_CLASS );
			return 0;
		}
		break;

	case WM_SETFOCUS:
		if ( r_autopriority->current.boolean )
		{
			SetPriorityClass( GetCurrentProcess(), NORMAL_PRIORITY_CLASS );
			return 0;
		}
		break;

	case WM_DISPLAYCHANGE:
		monitor = MonitorFromWindow( hWnd, MONITOR_DEFAULTTONEAREST );
		monitorInfo.cbSize = sizeof( monitorInfo );
		GetMonitorInfoA( monitor, &monitorInfo );
		GetWindowRect( hWnd, &r );

		// keep a fullscreen-sized window on the monitor after a mode change
		if ( r.right - r.left == monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left
			&& r.bottom - r.top == monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top )
		{
			MoveWindow( hWnd, monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.top, r.right - r.left, r.bottom - r.top, TRUE );
		}
		break;

	case WM_SYSKEYDOWN:
		if ( wParam == VK_RETURN )
		{
			if ( clc.state == CA_LOADING )
				return 0;

			if ( r_fullscreen && Dvar_GetInt( "developer" ) )
			{
				Dvar_SetBool( r_fullscreen, !r_fullscreen->current.boolean );
				Cbuf_AddText( "vid_restart\n" );
			}
			return 0;
		}
		// fall through
	case WM_KEYDOWN:
		code = MapKey( lParam, wParam );
		if ( code )
			Sys_QueEvent( g_wv.sysMsgTime, SE_KEY, code, qtrue, 0, NULL );
		break;

	case WM_SYSKEYUP:
	case WM_KEYUP:
		code = MapKey( lParam, wParam );
		if ( code )
			Sys_QueEvent( g_wv.sysMsgTime, SE_KEY, code, qfalse, 0, NULL );
		break;

	case WM_CHAR:
		Sys_QueEvent( g_wv.sysMsgTime, SE_CHAR, wParam, 0, 0, NULL );
		break;

	case WM_SYSCOMMAND:
		if ( wParam == SC_SCREENSAVE )
			return 0;
		break;

	case WM_MOUSEWHEEL:
		if ( (short)HIWORD( wParam ) > 0 )
		{
			Sys_QueEvent( g_wv.sysMsgTime, SE_KEY, K_MWHEELUP, qtrue, 0, NULL );
			Sys_QueEvent( g_wv.sysMsgTime, SE_KEY, K_MWHEELUP, qfalse, 0, NULL );
		}
		else
		{
			Sys_QueEvent( g_wv.sysMsgTime, SE_KEY, K_MWHEELDOWN, qtrue, 0, NULL );
			Sys_QueEvent( g_wv.sysMsgTime, SE_KEY, K_MWHEELDOWN, qfalse, 0, NULL );
		}
		break;

	case WM_POWERBROADCAST:
		// refuse suspend requests
		if ( wParam == PBT_APMQUERYSUSPEND || wParam == PBT_APMQUERYSTANDBY )
			return BROADCAST_QUERY_DENY;
		break;

	// Win32 may pack several mouse events into one update, so check all states
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
		buttons = 0;
		if ( wParam & MK_LBUTTON )
			buttons = 1;
		if ( wParam & MK_RBUTTON )
			buttons |= 2;
		if ( wParam & MK_MBUTTON )
			buttons |= 4;
		if ( wParam & MK_XBUTTON1 )
			buttons |= 8;
		if ( wParam & MK_XBUTTON2 )
			buttons |= 16;
		IN_MouseEvent( buttons );
		break;
	}

	return DefWindowProcA( hWnd, uMsg, wParam, lParam );
}

