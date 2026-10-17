#include "../qcommon/qcommon.h"

static bool shouldQuitOnError = false;

static HWND g_hwndGame[4];
static unsigned int g_hiddenCount;

void FixWindowsDesktop();
void RefreshQuitOnErrorCondition();
bool QuitOnError();

static BOOL CALLBACK HideWindowCallback( HWND hwnd, LPARAM lParam )
{
	char caption[1024];
	LONG style;
	LONG styleEx;

	if ( GetWindowTextA( hwnd, caption, sizeof( caption ) ) && strcmp( caption, "Call of Duty 2 Multiplayer" ) )
		return TRUE;

	style = GetWindowLongA( hwnd, GWL_STYLE );
	styleEx = GetWindowLongA( hwnd, GWL_EXSTYLE );

	if ( style & WS_VISIBLE )
	{
		g_hwndGame[g_hiddenCount++] = hwnd;
		SetWindowLongA( hwnd, GWL_STYLE, style & ~WS_VISIBLE );
		SetWindowLongA( hwnd, GWL_EXSTYLE, styleEx & ~WS_EX_TOPMOST );
	}

	return TRUE;
}

/*
============
FixWindowsDesktop

Restores the desktop mode and a linear gamma ramp after a fatal error.
============
*/
void FixWindowsDesktop()
{
	HWND hwndDesktop;
	HDC hdc;
	unsigned short ramp[3 * 256];
	unsigned short i;

	ChangeDisplaySettingsA( NULL, 0 );
	EnumThreadWindows( GetCurrentThreadId(), HideWindowCallback, 0 );

	hwndDesktop = GetDesktopWindow();
	hdc = GetDC( hwndDesktop );

	for ( i = 0; i < 256; i++ )
	{
		ramp[i] = 257 * i;
		ramp[i + 256] = 257 * i;
		ramp[i + 512] = 257 * i;
	}

	SetDeviceGammaRamp( hdc, ramp );
	ReleaseDC( hwndDesktop, hdc );
}

/*
============
RefreshQuitOnErrorCondition
============
*/
void RefreshQuitOnErrorCondition()
{
	if ( Dvar_IsSystemActive() )
		shouldQuitOnError = Dvar_GetBool("QuitOnError") || Dvar_GetInt("r_vc_compile") == 2;
}

/*
============
QuitOnError
============
*/
bool QuitOnError()
{
	RefreshQuitOnErrorCondition();
	return shouldQuitOnError;
}

