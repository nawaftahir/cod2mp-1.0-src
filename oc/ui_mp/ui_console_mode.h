#ifndef UI_CONSOLE_MODE_H
#define UI_CONSOLE_MODE_H

#include "cod2_feature_config.h"

#if COD2_FEATURE_GAMEPAD

#    define UI_CONSOLE_SAFEAREA_FRAC 0.05f
#    define UI_CONSOLE_VIRTUAL_W 640.0f
#    define UI_CONSOLE_VIRTUAL_H 480.0f

int UI_ConsoleMode_Active(void);

void UI_ConsoleMode_RegisterDvar(void);

void UI_ConsoleMode_Init(void *info);

const char *UI_ConsoleMode_MainMenuName(void);

int UI_ConsoleMode_SuppressCursor(void);

void UI_ConsoleMode_DrawGlyphs(void *info);

void UI_ConsoleMode_SafeAreaRect(float *x, float *y, float *w, float *h);

#endif
#endif
