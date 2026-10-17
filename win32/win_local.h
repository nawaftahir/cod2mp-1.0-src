#pragma once

#define K_MOUSE1 200

typedef struct
{
	HINSTANCE reflib_library;
	qboolean reflib_active;
	HWND hWnd;
	HINSTANCE hInstance;
	qboolean activeApp;
	qboolean isMinimized;
	unsigned int sysMsgTime;
} WinVars_t;

extern WinVars_t g_wv;

void Sys_QueEvent( int time, sysEventType_t type, int value, int value2, int ptrLength, void *ptr );

void IN_Init( void );
void IN_Activate( qboolean active );
void IN_ActivateMouse( qboolean force );
void IN_DeactivateMouse( void );
void IN_MouseEvent( int mstate );
void IN_Shutdown( void );
