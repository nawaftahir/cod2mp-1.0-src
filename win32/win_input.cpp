#include "../qcommon/qcommon.h"
#include "win_local.h"

typedef struct
{
	int oldButtonState;
	bool mouseActive;
	bool mouseInitialized;
} WinMouseVars_t;

static WinMouseVars_t s_wmv;

static int window_center_x, window_center_y;

static dvar_t *in_mouse;

static qboolean in_appactive;

int Key_GetCatcher( void );
void CL_MouseEvent( int dx, int dy );

/*
============================================================

WIN32 MOUSE CONTROL

============================================================
*/

void IN_ActivateWin32Mouse( void );
static void IN_DeactivateWin32Mouse( void );
void IN_Win32Mouse( int *mx, int *my );
void IN_ActivateMouse( qboolean force );
void IN_DeactivateMouse( void );
void IN_Activate( qboolean active );
void IN_MouseEvent( int mstate );
void IN_MouseMove( void );
static void IN_StartupMouse( void );
void IN_Startup( void );
void IN_Shutdown( void );
void IN_Init( void );
void IN_Frame( void );

/*
============================================================
  MOUSE CONTROL
============================================================
*/

/*
=========================================================================
=========================================================================
*/

/*
===========
IN_DeactivateMouse

Called when the window loses focus
===========
*/

/*
===========
IN_Shutdown
===========
*/


/*
================
IN_ActivateWin32Mouse
================
*/
void IN_ActivateWin32Mouse( void )
{
	RECT window_rect;

	SetCapture( g_wv.hWnd );
	GetWindowRect( g_wv.hWnd, &window_rect );

	window_center_x = ( window_rect.right + window_rect.left ) / 2;
	window_center_y = ( window_rect.top + window_rect.bottom ) / 2;

	SetCursorPos( window_center_x, window_center_y );

	while ( ShowCursor( FALSE ) >= 0 )
		;
}

/*
================
IN_DeactivateWin32Mouse
================
*/
static void IN_DeactivateWin32Mouse( void )
{
	ReleaseCapture();

	while ( ShowCursor( TRUE ) < 0 )
		;
}

void IN_SetForegroundWindow( void )
{
	SetForegroundWindow( g_wv.hWnd );
	SetFocus( g_wv.hWnd );
}

qboolean IN_IsForegroundWindow( void )
{
	return GetForegroundWindow() == g_wv.hWnd;
}

/*
================
IN_Win32Mouse
================
*/
void IN_Win32Mouse( int *mx, int *my )
{
	POINT current_pos;

	// find mouse movement
	GetCursorPos( &current_pos );

	// force the mouse to the center, so there's room to move
	SetCursorPos( window_center_x, window_center_y );

	*mx = current_pos.x - window_center_x;
	*my = current_pos.y - window_center_y;
}

/*
===========
IN_ActivateMouse

Called when the window gains focus or changes in some way
===========
*/
void IN_ActivateMouse( qboolean force )
{
	if ( !s_wmv.mouseInitialized )
		return;

	if ( !in_mouse->current.boolean )
	{
		s_wmv.mouseActive = false;
		return;
	}

	if ( !force )
	{
		if ( s_wmv.mouseActive )
			return;
	}
	else
	{
		// don't grab the mouse over the console on the desktop
		if ( ( Key_GetCatcher() & KEYCATCH_CONSOLE ) && !Dvar_GetBool( "r_fullscreen" ) )
			return;
	}

	if ( GetForegroundWindow() != g_wv.hWnd )
	{
		s_wmv.mouseActive = false;
		return;
	}

	s_wmv.mouseActive = true;
	IN_ActivateWin32Mouse();
}

/*
===========
IN_StartupMouse
===========
*/
static void IN_StartupMouse( void )
{
	s_wmv.mouseInitialized = false;

	if ( !in_mouse->current.boolean )
	{
		Com_Printf( "Mouse control not active.\n" );
		return;
	}

	s_wmv.mouseInitialized = true;
}

/*
===========
IN_MouseEvent
===========
*/
void IN_MouseEvent( int mstate )
{
	int i;

	if ( !s_wmv.mouseInitialized )
		return;

	// perform button actions
	for ( i = 0; i < 5; i++ )
	{
		if ( ( mstate & ( 1 << i ) ) && !( s_wmv.oldButtonState & ( 1 << i ) ) )
		{
			Sys_QueEvent( g_wv.sysMsgTime, SE_KEY, K_MOUSE1 + i, qtrue, 0, NULL );
		}

		if ( !( mstate & ( 1 << i ) ) && ( s_wmv.oldButtonState & ( 1 << i ) ) )
		{
			Sys_QueEvent( g_wv.sysMsgTime, SE_KEY, K_MOUSE1 + i, qfalse, 0, NULL );
		}
	}

	s_wmv.oldButtonState = mstate;
}

/*
===========
IN_MouseMove
===========
*/
void IN_MouseMove( void )
{
	int mx, my;

	if ( GetForegroundWindow() != g_wv.hWnd )
		return;

	IN_Win32Mouse( &mx, &my );

	if ( !mx && !my )
		return;

	CL_MouseEvent( mx, my );
}

/*
===========
IN_Startup
===========
*/
void IN_Startup( void )
{
	IN_StartupMouse();

	in_mouse->modified = false;
}

/*
===========
IN_Init
===========
*/
void IN_Init( void )
{
	in_mouse = Dvar_RegisterBool( "in_mouse", true, DVAR_ARCHIVE | DVAR_LATCH | DVAR_CHANGEABLE_RESET );

	IN_Startup();
}

/*
===========
IN_Activate

Called when the main window gains or loses focus.
===========
*/
void IN_Activate( qboolean active )
{
	in_appactive = active;

	if ( !active )
		IN_DeactivateMouse();
}

/*
==================
IN_Frame

Called every frame, even if not generating commands
==================
*/
void IN_Frame( void )
{
	if ( Dvar_GetBool( "ClickToContinue" ) )
		PostMessage( g_wv.hWnd, WM_LBUTTONDOWN, MK_LBUTTON, 0 );

	if ( !s_wmv.mouseInitialized )
		return;

	if ( !in_appactive )
	{
		IN_DeactivateMouse();
		return;
	}

	// temporarily deactivate if not in the game and running on the desktop
	if ( ( Key_GetCatcher() & KEYCATCH_CONSOLE ) && !Dvar_GetBool( "r_fullscreen" ) )
	{
		IN_DeactivateMouse();
		return;
	}

	IN_ActivateMouse( qfalse );

	// post events to the system que
	IN_MouseMove();
}

// retail 0x461ea0, unreferenced; name from Q3 (SUPPORTED)
void IN_ClearStates( void )
{
	s_wmv.oldButtonState = 0;
}

void IN_DeactivateMouse( void )
{
	if ( !s_wmv.mouseInitialized )
		return;

	if ( !s_wmv.mouseActive )
		return;

	s_wmv.mouseActive = false;

	IN_DeactivateWin32Mouse();
}
void IN_Shutdown( void )
{
	IN_DeactivateMouse();
}
