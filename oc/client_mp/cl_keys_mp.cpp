#include "common_types.h"
#include "imports.h"
#include <ctype.h>
#include <string.h>

PlayerKeyState playerKeys[1];
field_t *chatField = &playerKeys[0].chatField;
qboolean *chat_team = &playerKeys[0].chat_team;
qboolean *key_overstrikeMode = &playerKeys[0].key_overstrikeMode;
qboolean *anykeydown = &playerKeys[0].anykeydown;
qkey_t *keys = playerKeys[0].keys;

int historyLine = 0;
int nextHistoryLine;
field_t g_consoleField;
field_t historyEditLines[32];
extern const dvar_t *cl_bypassMouseInput;
extern dvar_t *com_sv_running;
extern dvar_t *sv_disableClientConsole;
extern clientActive_t clients[1];
extern clientConnection_t clientConnections[1];
#define clc clientConnections[0]
extern int dvar_modifiedFlags;
extern int g_console_field_width;
extern float g_console_char_height;

extern keyname_t keynames[96];
extern keyname_t keynames_localized[96];

extern clientStatic_t cls;
extern refexport_t re;

extern void ReplaceStringInternal(const char **str, const char *in);
extern void Cmd_AddCommand(const char *name, void (*func)(void));
extern int Cmd_Argc(void);
extern const char *Cmd_Argv( int arg );
extern void I_strncat(char *dest, int maxlen, const char *src);
extern int I_stricmp(const char *s0, const char *s1);
extern bool I_isdigit( int c );
extern int I_strnicmp(const char *s0, const char *s1, int n);
extern void I_strncpyz(char *dest, const char *src, int destsize);
extern void Com_Printf(const char *fmt, ...);
extern void FS_Printf(fileHandle_t f, const char *fmt, ...);
extern int SEH_GetCurrentLanguage(void);
extern void CL_SwitchToLocalClient(int localClientNum);
extern "C" void Z_FreeInternal( void *ptr );
extern void UI_KeyEvent(int key, int down);
extern void Field_Clear(field_t *edit);
extern char *Sys_GetClipboardData(void);
extern void Cbuf_AddText(const char *text);
extern int Com_sprintf(char *dest, size_t size, const char *format, ...);
extern void CL_AddReliableCommand(const char *cmd);
extern void Con_ToggleConsole_f(void);
extern void Con_ToggleConsoleOutput(void);
extern Bool Con_IsActive(void);
extern void Con_PageUp(void);
extern void Con_PageDown(void);
extern void Con_Top(void);
extern void Con_Bottom(void);
extern cmd_function_t *Cmd_FindCommand(const char *cmdName);
extern char **FS_ListFiles( const char *path, const char *extension, FsListBehavior behavior, int *numfiles, int allocType );
extern void FS_FreeFileList(char **list, int allocType);
extern void Com_StripExtension(const char *in, char *out);
extern dvar_s *Dvar_FindVar(const char *dvarName);
extern void Con_AutoCompleteFromList(const char **strings, int stringCount, const char *prefix, char *completed, int sizeofCompleted);
extern const char *Con_TokenizeInput(void);
extern Bool Con_AnySpaceAfterCommand(void);
extern qboolean UI_SetActiveMenu(int menu);
extern qboolean UI_checkKeyExec(int key);
extern int CG_KeyInterceptEvent(int key, qboolean down);
extern void CL_StopLogoOrCinematic(void);
extern void CL_Disconnect(void);
extern const dvar_t *nextdemo;
extern LegacyHacks *legacyHacks;
int stricmp(const char *str1, const char *str2);
extern void Dvar_SetString(dvar_t *dvar, const char *value);
extern int I_strncmp(const char *s0, const char *s1, int n);
extern void Cmd_ForEach(void (*callback)(const char *));
extern void Dvar_ForEach(void (*callback)(const char *));
extern void SCR_UpdateScreen(void);
extern Bool CL_AllLocalClientsDisconnected(void);

extern FontHandle UI_GetFontHandle(int fontEnum, float scale);
extern void CL_DrawTextWithCursor(const char *text, int maxChars, FontHandle font, float x, float y, int horzAlign, int vertAlign, float xScale, float yScale, const vec_t *color, int style, int cursorPos, char cursor);
extern float GetRealWidthFromVirtualWidth(float w);
extern float GetRealHeightFromVirtualHeight(float h);
extern float GetVirtualWidthFromRealWidth(float w);
extern float GetVirtualHeightFromRealHeight(float h);
extern int SEH_PrintStrlen(const char *str);

extern PlayerKeyState playerKeys[1];
extern field_t *chatField;
extern qboolean *chat_team;
extern qboolean *key_overstrikeMode;
extern qboolean *anykeydown;
extern qkey_t *keys;
extern field_t historyEditLines[32];
extern field_t g_consoleField;
extern int nextHistoryLine;
extern int historyLine;
static keyname_t keynames[96] = {
    { (char *)"TAB", 0x9 },
    { (char *)"ENTER", 0xd },
    { (char *)"ESCAPE", 0x1b },
    { (char *)"SPACE", 0x20 },
    { (char *)"BACKSPACE", 0x7f },
    { (char *)"UPARROW", 0x9a },
    { (char *)"DOWNARROW", 0x9b },
    { (char *)"LEFTARROW", 0x9c },
    { (char *)"RIGHTARROW", 0x9d },
    { (char *)"ALT", 0x9e },
    { (char *)"CTRL", 0x9f },
    { (char *)"SHIFT", 0xa0 },
    { (char *)"CAPSLOCK", 0x97 },
    { (char *)"F1", 0xa7 },
    { (char *)"F2", 0xa8 },
    { (char *)"F3", 0xa9 },
    { (char *)"F4", 0xaa },
    { (char *)"F5", 0xab },
    { (char *)"F6", 0xac },
    { (char *)"F7", 0xad },
    { (char *)"F8", 0xae },
    { (char *)"F9", 0xaf },
    { (char *)"F10", 0xb0 },
    { (char *)"F11", 0xb1 },
    { (char *)"F12", 0xb2 },
    { (char *)"INS", 0xa1 },
    { (char *)"DEL", 0xa2 },
    { (char *)"PGDN", 0xa3 },
    { (char *)"PGUP", 0xa4 },
    { (char *)"HOME", 0xa5 },
    { (char *)"END", 0xa6 },
    { (char *)"MOUSE1", 0xc8 },
    { (char *)"MOUSE2", 0xc9 },
    { (char *)"MOUSE3", 0xca },
    { (char *)"MOUSE4", 0xcb },
    { (char *)"MOUSE5", 0xcc },
    { (char *)"MWHEELUP", 0xce },
    { (char *)"MWHEELDOWN", 0xcd },
    { (char *)"AUX1", 0xcf },
    { (char *)"AUX2", 0xd0 },
    { (char *)"AUX3", 0xd1 },
    { (char *)"AUX4", 0xd2 },
    { (char *)"AUX5", 0xd3 },
    { (char *)"AUX6", 0xd4 },
    { (char *)"AUX7", 0xd5 },
    { (char *)"AUX8", 0xd6 },
    { (char *)"AUX9", 0xd7 },
    { (char *)"AUX10", 0xd8 },
    { (char *)"AUX11", 0xd9 },
    { (char *)"AUX12", 0xda },
    { (char *)"AUX13", 0xdb },
    { (char *)"AUX14", 0xdc },
    { (char *)"AUX15", 0xdd },
    { (char *)"AUX16", 0xde },
    { (char *)"KP_HOME", 0xb6 },
    { (char *)"KP_UPARROW", 0xb7 },
    { (char *)"KP_PGUP", 0xb8 },
    { (char *)"KP_LEFTARROW", 0xb9 },
    { (char *)"KP_5", 0xba },
    { (char *)"KP_RIGHTARROW", 0xbb },
    { (char *)"KP_END", 0xbc },
    { (char *)"KP_DOWNARROW", 0xbd },
    { (char *)"KP_PGDN", 0xbe },
    { (char *)"KP_ENTER", 0xbf },
    { (char *)"KP_INS", 0xc0 },
    { (char *)"KP_DEL", 0xc1 },
    { (char *)"KP_SLASH", 0xc2 },
    { (char *)"KP_MINUS", 0xc3 },
    { (char *)"KP_PLUS", 0xc4 },
    { (char *)"KP_NUMLOCK", 0xc5 },
    { (char *)"KP_STAR", 0xc6 },
    { (char *)"KP_EQUALS", 0xc7 },
    { (char *)"PAUSE", 0x99 },
    { (char *)"SEMICOLON", 0x3b },
    { (char *)"COMMAND", 0x96 },
    { (char *)"181", 0x80 },
    { (char *)"191", 0x81 },
    { (char *)"223", 0x82 },
    { (char *)"224", 0x83 },
    { (char *)"225", 0x84 },
    { (char *)"228", 0x85 },
    { (char *)"229", 0x86 },
    { (char *)"230", 0x87 },
    { (char *)"231", 0x88 },
    { (char *)"232", 0x89 },
    { (char *)"233", 0x8a },
    { (char *)"236", 0x8b },
    { (char *)"241", 0x8c },
    { (char *)"242", 0x8d },
    { (char *)"243", 0x8e },
    { (char *)"246", 0x8f },
    { (char *)"248", 0x90 },
    { (char *)"249", 0x91 },
    { (char *)"250", 0x92 },
    { (char *)"252", 0x93 },
    { 0, 0x0 }
};
static keyname_t keynames_localized[96] = {
    { (char *)"KEY_TAB", 0x9 },
    { (char *)"KEY_ENTER", 0xd },
    { (char *)"KEY_ESCAPE", 0x1b },
    { (char *)"KEY_SPACE", 0x20 },
    { (char *)"KEY_BACKSPACE", 0x7f },
    { (char *)"KEY_UPARROW", 0x9a },
    { (char *)"KEY_DOWNARROW", 0x9b },
    { (char *)"KEY_LEFTARROW", 0x9c },
    { (char *)"KEY_RIGHTARROW", 0x9d },
    { (char *)"KEY_ALT", 0x9e },
    { (char *)"KEY_CTRL", 0x9f },
    { (char *)"KEY_SHIFT", 0xa0 },
    { (char *)"KEY_CAPSLOCK", 0x97 },
    { (char *)"KEY_F1", 0xa7 },
    { (char *)"KEY_F2", 0xa8 },
    { (char *)"KEY_F3", 0xa9 },
    { (char *)"KEY_F4", 0xaa },
    { (char *)"KEY_F5", 0xab },
    { (char *)"KEY_F6", 0xac },
    { (char *)"KEY_F7", 0xad },
    { (char *)"KEY_F8", 0xae },
    { (char *)"KEY_F9", 0xaf },
    { (char *)"KEY_F10", 0xb0 },
    { (char *)"KEY_F11", 0xb1 },
    { (char *)"KEY_F12", 0xb2 },
    { (char *)"KEY_INS", 0xa1 },
    { (char *)"KEY_DEL", 0xa2 },
    { (char *)"KEY_PGDN", 0xa3 },
    { (char *)"KEY_PGUP", 0xa4 },
    { (char *)"KEY_HOME", 0xa5 },
    { (char *)"KEY_END", 0xa6 },
    { (char *)"KEY_MOUSE1", 0xc8 },
    { (char *)"KEY_MOUSE2", 0xc9 },
    { (char *)"KEY_MOUSE3", 0xca },
    { (char *)"KEY_MOUSE4", 0xcb },
    { (char *)"KEY_MOUSE5", 0xcc },
    { (char *)"KEY_MWHEELUP", 0xce },
    { (char *)"KEY_MWHEELDOWN", 0xcd },
    { (char *)"KEY_AUX1", 0xcf },
    { (char *)"KEY_AUX2", 0xd0 },
    { (char *)"KEY_AUX3", 0xd1 },
    { (char *)"KEY_AUX4", 0xd2 },
    { (char *)"KEY_AUX5", 0xd3 },
    { (char *)"KEY_AUX6", 0xd4 },
    { (char *)"KEY_AUX7", 0xd5 },
    { (char *)"KEY_AUX8", 0xd6 },
    { (char *)"KEY_AUX9", 0xd7 },
    { (char *)"KEY_AUX10", 0xd8 },
    { (char *)"KEY_AUX11", 0xd9 },
    { (char *)"KEY_AUX12", 0xda },
    { (char *)"KEY_AUX13", 0xdb },
    { (char *)"KEY_AUX14", 0xdc },
    { (char *)"KEY_AUX15", 0xdd },
    { (char *)"KEY_AUX16", 0xde },
    { (char *)"KEY_KP_HOME", 0xb6 },
    { (char *)"KEY_KP_UPARROW", 0xb7 },
    { (char *)"KEY_KP_PGUP", 0xb8 },
    { (char *)"KEY_KP_LEFTARROW", 0xb9 },
    { (char *)"KEY_KP_5", 0xba },
    { (char *)"KEY_KP_RIGHTARROW", 0xbb },
    { (char *)"KEY_KP_END", 0xbc },
    { (char *)"KEY_KP_DOWNARROW", 0xbd },
    { (char *)"KEY_KP_PGDN", 0xbe },
    { (char *)"KEY_KP_ENTER", 0xbf },
    { (char *)"KEY_KP_INS", 0xc0 },
    { (char *)"KEY_KP_DEL", 0xc1 },
    { (char *)"KEY_KP_SLASH", 0xc2 },
    { (char *)"KEY_KP_MINUS", 0xc3 },
    { (char *)"KEY_KP_PLUS", 0xc4 },
    { (char *)"KEY_KP_NUMLOCK", 0xc5 },
    { (char *)"KEY_KP_STAR", 0xc6 },
    { (char *)"KEY_KP_EQUALS", 0xc7 },
    { (char *)"KEY_PAUSE", 0x99 },
    { (char *)"KEY_SEMICOLON", 0x3b },
    { (char *)"KEY_COMMAND", 0x96 },
    { (char *)"\xb5", 0x80 },
    { (char *)"\xbf", 0x81 },
    { (char *)"\xdf", 0x82 },
    { (char *)"\xe0", 0x83 },
    { (char *)"\xe1", 0x84 },
    { (char *)"\xe4", 0x85 },
    { (char *)"\xe5", 0x86 },
    { (char *)"\xe6", 0x87 },
    { (char *)"\xe7", 0x88 },
    { (char *)"\xe8", 0x89 },
    { (char *)"\xe9", 0x8a },
    { (char *)"\xec", 0x8b },
    { (char *)"\xf1", 0x8c },
    { (char *)"\xf2", 0x8d },
    { (char *)"\xf3", 0x8e },
    { (char *)"\xf6", 0x8f },
    { (char *)"\xf8", 0x90 },
    { (char *)"\xf9", 0x91 },
    { (char *)"\xfa", 0x92 },
    { (char *)"\xfc", 0x93 },
    { 0, 0x0 }
};

// French keyboard number row (CP1252), retail 0x58b700 after the key name tables
static char *frenchNumberKeysMap[10] = {
    "\xe0",
    "&",
    "\xe9",
    "\"",
    "'",
    "(",
    "-",
    "\xe8",
    "_",
    "\xe7",
};

#define COMMAND_HISTORY 32

static char tinystr[5];
static const char *completionString;
static Bool hasExactMatch;
static char shortestMatch[1024];
static int matchCount;

enum {
    K_ENTER = 0x0d,
    K_TAB = 0x09,
    K_ESCAPE = 0x1b,
    K_UPARROW = 0x9a,
    K_DOWNARROW = 0x9b,
    K_RIGHTARROW = 0x9d,
    K_LEFTARROW = 0x9c,
    K_CTRL = 0x9f,
    K_SHIFT = 0xa0,
    K_INS = 0xa1,
    K_DEL = 0xa2,
    K_PGDN = 0xa3,
    K_PGUP = 0xa4,
    K_HOME = 0xa5,
    K_END = 0xa6,
    K_KP_UPARROW = 0xb7,
    K_KP_DOWNARROW = 0xbd,
    K_KP_ENTER = 0xbf,
    K_KP_INS = 0xc0,
    K_MWHEELDOWN = 0xcd,
    K_MWHEELUP = 0xce
};

qboolean Key_GetOverstrikeMode(void);
void Field_AdjustScroll(field_t *edit);
void Key_SetOverstrikeMode(qboolean state);
static void FindMatches(const char *s);
static void PrintMatches(const char *s);
static void keyConcatArgs(void);
int Key_StringToKeynum(const char *str);
char *Key_KeynumToString(int keynum, qboolean translate);
void Key_SetBinding(int keynum, const char *binding);
char *Key_GetBinding(int keynum);
void CL_InitKeyCommands(void);
void Key_Shutdown(void);
void Field_Draw(field_t *edit, int x, int y, int horzAlign, int vertAlign);
void Key_Unbind_f(void);
void Key_Unbindall_f(void);
void Key_Bind_f(void);
void Field_CharEvent(field_t *edit, int ch);
void CL_CharEvent(int key);
void Field_KeyDownEvent(field_t *edit, int key);
void Message_Key(int key);
void Console_Key(int key);
void Key_WriteBindings(fileHandle_t f);
void Key_Bindlist_f(void);
void CL_KeyEvent(int key, const qboolean down, const unsigned int time);
void Key_ClearStates(void);

#if COD2_FEATURE_GAMEPAD
extern void CL_Gamepad_NoteKbmActive(void);
#endif

qboolean Key_GetOverstrikeMode(void);
void Key_SetOverstrikeMode(qboolean state);
void Key_Unbind_f(void);
void Key_Unbindall_f(void);
void Key_Bind_f(void);
void Field_CharEvent(field_t *edit, int ch);
void Key_Bindlist_f(void);

void Field_Draw(field_t *edit, int x, int y, int horzAlign, int vertAlign)
{
    vec4_t vColor;
    char str[0x400];
    int cursorPos;
    FontHandle font;
    int fontStyle;
    float xScale;
    float yScale;
    float xAdj;
    float yAdj;
    char cursorChar;
    float scale;

    vColor[0] = 1.0f;
    vColor[1] = 1.0f;
    vColor[2] = 1.0f;
    vColor[3] = 1.0f;

    I_strncpyz(str, edit->buffer + edit->scroll, 0x100 - edit->scroll);

    cursorPos = edit->cursor - edit->scroll;

    if (edit->fixedSize) {
        font = cls.consoleFont;
        fontStyle = 0;
        xScale = 1.0f;
        yScale = 1.0f;
        cursorChar = *key_overstrikeMode ? '_' : '|';
    } else {
        scale = edit->charHeight / 48.0f;
        font = UI_GetFontHandle(0, scale);
        scale = re.NormalizedTextScale(font, scale);
        if (vertAlign == 5) {
            xScale = GetRealWidthFromVirtualWidth(scale);
            yScale = GetRealHeightFromVirtualHeight(scale);
        } else {
            xScale = scale;
            yScale = scale;
        }
        fontStyle = 3;
        cursorChar = *key_overstrikeMode ? '_' : '|';
    }

    xAdj = (float)x;
    yAdj = (float)y + (float)re.TextHeight(font) * yScale;

    if (!edit->drawWidth)
        edit->drawWidth = 0x100;

    CL_DrawTextWithCursor(str, edit->drawWidth, font,
                          xAdj, yAdj, horzAlign, vertAlign,
                          xScale, yScale,
                          vColor, fontStyle, cursorPos, cursorChar);
}

void Field_AdjustScroll(field_t *edit)
{
	float fontScale;
	float lineWidth;
	FontHandle font;
	float actualScale;
	float len;
	int strLen;

	fontScale = edit->charHeight; fontScale = fontScale * 0.020833334f;
	lineWidth = (float)edit->widthInPixels;
	if ( edit->fixedSize )
	{
		fontScale = GetVirtualWidthFromRealWidth(fontScale);
		lineWidth = GetVirtualHeightFromRealHeight(lineWidth);
		font = cls.consoleFont;
	}
	else
	{
		font = UI_GetFontHandle(0, fontScale);
	}
	actualScale = re.NormalizedTextScale(font, fontScale);
	if ( (float)re.TextWidth(edit->buffer, 0, font) * actualScale < lineWidth )
	{
		edit->scroll = 0;
		edit->drawWidth = SEH_PrintStrlen(edit->buffer);
		return;
	}
	len = 0.0f;
	while ( len < lineWidth && edit->scroll > 0 )
	{
		len = (float)re.TextWidth(&edit->buffer[edit->scroll - 1], 0, font) * actualScale;
		if ( len >= lineWidth )
			break;
		edit->scroll--;
	}
	do
	{
		len = (float)re.TextWidth(&edit->buffer[edit->scroll], 0, font) * actualScale
			- (float)re.TextWidth(&edit->buffer[edit->cursor], 0, font) * actualScale;
		if ( len < 0.0f || len >= lineWidth )
		{
			if ( len < 0.0f )
			{
				if ( !edit->scroll )
					len = 0.0f;
				else
					edit->scroll--;
			}
			else
			{
				edit->scroll++;
			}
		}
	}
	while ( len < 0.0f || len >= lineWidth );
	strLen = strlen(&edit->buffer[edit->scroll]);
	edit->drawWidth = edit->cursor - edit->scroll;
	len = 0.0f;
	while ( len < lineWidth && edit->drawWidth < strLen )
	{
		len = (float)re.TextWidth(&edit->buffer[edit->scroll], edit->drawWidth + 1, font) * actualScale;
		if ( len < lineWidth )
			edit->drawWidth++;
	}
}

void Field_PasteClipboard(field_t *edit)
{
    char *cbd;
    int len;
    int i;

    cbd = Sys_GetClipboardData();
    if (!cbd)
        return;

    len = (int)strlen(cbd);
    for (i = 0; i < len; ++i) {
        Field_CharEvent(edit, cbd[i]);
    }

    Z_FreeInternal(cbd);
}

void Field_KeyDownEvent(field_t *edit, int key)
{
	int len;

	len = strlen(edit->buffer);
	if ( (key == K_INS || key == K_KP_INS) && keys[K_SHIFT].down )
	{
		Field_PasteClipboard(edit);
	}
	else if ( key == K_DEL )
	{
		if ( edit->cursor < len )
			memmove(&edit->buffer[edit->cursor], &edit->buffer[edit->cursor + 1], len - edit->cursor);
	}
	else if ( key == K_RIGHTARROW )
	{
		if ( edit->cursor < len )
			edit->cursor++;
		if ( keys[K_CTRL].down )
		{
			while ( edit->cursor < len && isalnum(edit->buffer[edit->cursor]) )
				edit->cursor++;
			while ( edit->cursor < len && !isalnum(edit->buffer[edit->cursor]) )
				edit->cursor++;
		}
	}
	else if ( key == K_LEFTARROW )
	{
		if ( edit->cursor > 0 )
			edit->cursor--;
		if ( keys[K_CTRL].down )
		{
			while ( edit->cursor > 0 && isalnum(edit->buffer[edit->cursor - 1]) )
				edit->cursor--;
		}
		if ( edit->cursor < edit->scroll )
			edit->scroll = edit->cursor;
	}
	else if ( key == K_HOME || tolower(key) == 'a' && keys[K_CTRL].down )
	{
		edit->cursor = 0;
	}
	else if ( key == K_END || tolower(key) == 'e' && keys[K_CTRL].down )
	{
		edit->cursor = len;
	}
	else if ( key == K_INS )
	{
		Key_SetOverstrikeMode(!Key_GetOverstrikeMode());
	}
	if ( cls.uiStarted )
		Field_AdjustScroll(edit);
}

void Field_CharEvent(field_t *edit, int ch)
{
    int len = (int)strlen(edit->buffer);

    if (ch == 0x16) {
        Field_PasteClipboard(edit);
        Field_AdjustScroll(edit);
        return;
    }

    if (ch == 3) {
        Field_Clear(edit);
        Field_AdjustScroll(edit);
        return;
    }

    if (ch == 8) {
        if (edit->cursor > 0) {
            memmove(edit->buffer + edit->cursor - 1,
                    edit->buffer + edit->cursor,
                    (size_t)(len - edit->cursor + 1));
            edit->cursor--;
        }
        Field_AdjustScroll(edit);
        return;
    }

    if (ch == 1) {
        edit->cursor = 0;
        edit->scroll = 0;
        return;
    }

    if (ch == 5) {
        edit->cursor = len;
        Field_AdjustScroll(edit);
        return;
    }

    if (ch < 32)
        return;

    if (*key_overstrikeMode) {
        if (edit->cursor == 255)
            return;

        edit->buffer[edit->cursor] = (char)ch;
        edit->cursor++;
    } else {
        if (len == 255)
            return;

        memmove(edit->buffer + edit->cursor + 1,
                edit->buffer + edit->cursor,
                (size_t)(len - edit->cursor + 1));

        edit->buffer[edit->cursor] = (char)ch;
        edit->cursor++;
    }

    if (edit->cursor == len + 1)
        edit->buffer[edit->cursor] = 0;

    Field_AdjustScroll(edit);
}

void FindMatches(const char *s)
{
	int i;

	if ( I_strnicmp(s, completionString, strlen(completionString)) )
		return;

	matchCount++;
	if ( matchCount == 1 )
	{
		I_strncpyz(shortestMatch, s, sizeof(shortestMatch));
		hasExactMatch = 1;
		return;
	}

	for ( i = 0; s[i]; i++ )
	{
		if ( tolower(shortestMatch[i]) != tolower(s[i]) )
			break;
	}

	if ( s[i] && (!hasExactMatch || shortestMatch[i]) )
		hasExactMatch = 0;
	else
		hasExactMatch = 1;

	shortestMatch[i] = 0;
}

static void PrintMatches(const char *s)
{
    if (I_strnicmp(s, shortestMatch, (int)strlen(shortestMatch)) == 0) {
        Com_Printf("    %s\n", s);
    }
}

static void keyConcatArgs(void)
{
    int i;
    const char *arg;

    for (i = 1; i < Cmd_Argc(); i++) {
        I_strncat(g_consoleField.buffer, sizeof(g_consoleField.buffer), " ");
        arg = Cmd_Argv(i);
        while (*arg) {
            if (*arg == ' ') {
                I_strncat(g_consoleField.buffer, sizeof(g_consoleField.buffer), "\"");
                break;
            }
            arg++;
        }
        I_strncat(g_consoleField.buffer, sizeof(g_consoleField.buffer), Cmd_Argv(i));
        if (*arg == ' ') {
            I_strncat(g_consoleField.buffer, sizeof(g_consoleField.buffer), "\"");
        }
    }
}

static void ConcatRemaining(const char *src, const char *start)
{
	const char *str;

	str = strstr(src, start);
	if ( !str )
	{
		keyConcatArgs();
		return;
	}

	str += strlen(start);
	I_strncat(g_consoleField.buffer, sizeof(g_consoleField.buffer), str);
}

static void ReplaceConsoleInputArgument(int replaceCount, const char *replacement)
{
	int cmdLineLen;
	int replaceStart;

	if ( !*replacement )
		return;

	cmdLineLen = strlen(g_consoleField.buffer);
	while ( cmdLineLen && isspace(g_consoleField.buffer[cmdLineLen - 1]) )
		cmdLineLen--;

	replaceStart = cmdLineLen - replaceCount;
	I_strncpyz(&g_consoleField.buffer[replaceStart], replacement, sizeof(g_consoleField.buffer) - replaceStart);
}

static void CompleteCmdArgument(void)
{
	const char *cmdName;
	const char *prefix;
	cmd_function_t *cmd;
	char **files;
	int fileCount;
	char arg[256];

	cmdName = Con_TokenizeInput();
	prefix = Cmd_Argv(1);
	if ( !*prefix )
		return;

	fileCount = 0;
	cmd = Cmd_FindCommand(cmdName);
	if ( !cmd->autoCompleteDir || !cmd->autoCompleteExt )
		return;

	files = FS_ListFiles(cmd->autoCompleteDir, cmd->autoCompleteExt, FS_LIST_PURE_ONLY, &fileCount, 10);
	if ( !fileCount )
		return;

	Con_AutoCompleteFromList((const char **)files, fileCount, prefix, arg, sizeof(arg));
	FS_FreeFileList(files, 10);
	Com_StripExtension(arg, arg);
	ReplaceConsoleInputArgument(strlen(prefix), arg);
}

static void CompleteDvarArgument(void)
{
	const dvar_s *dvar;
	const char *prefix;
	char value[256];

	dvar = Dvar_FindVar(Con_TokenizeInput());
	if ( dvar->type != DVAR_TYPE_ENUM )
		return;

	prefix = Cmd_Argv(1);
	if ( !*prefix )
		return;

	Con_AutoCompleteFromList(dvar->domain.enumeration.strings, dvar->domain.enumeration.stringCount, prefix, value, sizeof(value));
	ReplaceConsoleInputArgument(strlen(prefix), value);
}

static void CompleteCommand(void)
{
	field_t temp;
	int matchLenAfterCmds;
	int matchLenAfterDvars;

	completionString = Con_TokenizeInput();
	shortestMatch[0] = 0;
	matchCount = 0;
	if ( !*completionString )
		return;

	Cmd_ForEach(FindMatches);
	matchLenAfterCmds = strlen(shortestMatch);
	Dvar_ForEach(FindMatches);
	matchLenAfterDvars = strlen(shortestMatch);
	if ( !matchCount )
		return;

	temp = g_consoleField;

	if ( matchCount == 1 || hasExactMatch && Con_AnySpaceAfterCommand() )
	{
		Com_sprintf(g_consoleField.buffer, sizeof(g_consoleField.buffer), "\\%s", shortestMatch);
		if ( Cmd_Argc() == 1 )
		{
			I_strncat(g_consoleField.buffer, sizeof(g_consoleField.buffer), " ");
		}
		else
		{
			ConcatRemaining(temp.buffer, completionString);
			if ( Cmd_Argc() == 2 )
			{
				if ( matchLenAfterCmds == matchLenAfterDvars )
					CompleteCmdArgument();
				else
					CompleteDvarArgument();
			}
		}
		g_consoleField.cursor = strlen(g_consoleField.buffer);
		Field_AdjustScroll(&g_consoleField);
		return;
	}

	Com_sprintf(g_consoleField.buffer, sizeof(g_consoleField.buffer), "\\%s", shortestMatch);
	g_consoleField.cursor = strlen(g_consoleField.buffer);
	ConcatRemaining(temp.buffer, completionString);
	Field_AdjustScroll(&g_consoleField);

	Com_Printf("]%s\n", g_consoleField.buffer);
	Cmd_ForEach(PrintMatches);
	Dvar_ForEach(PrintMatches);
}

bool Console_IsRconCmd(const char *commandString)
{
    const char *rcon = "rcon";

    return I_strncmp(commandString, rcon, strlen(rcon)) == 0;
}

void Console_Key(int key)
{
    // ctrl-L clears screen
    if (key == 'l' && keys[K_CTRL].down) {
        Cbuf_AddText("clear\n");
        return;
    }

    // enter finishes the line
    if (key == K_ENTER || key == K_KP_ENTER) {
        // if not in the game explicitly prepend a slash if needed
        if (::clc.state != CA_ACTIVE && g_consoleField.buffer[0] != '\\' && g_consoleField.buffer[0] != '/') {
            char temp[1024];

            I_strncpyz(temp, g_consoleField.buffer, sizeof(temp));
            Com_sprintf(g_consoleField.buffer, sizeof(g_consoleField.buffer), "\\%s", temp);
            g_consoleField.cursor++;
        }

        Com_Printf("]%s\n", g_consoleField.buffer);

        // leading slash is an explicit command
        if (g_consoleField.buffer[0] == '\\' || g_consoleField.buffer[0] == '/') {
            Cbuf_AddText(g_consoleField.buffer + 1);
            Cbuf_AddText("\n");
        } else {
            // other text will be chat messages
            if (!g_consoleField.buffer[0]) {
                return;
            }
            if (!Console_IsRconCmd(g_consoleField.buffer)) {
                Cbuf_AddText("cmd say ");
                Cbuf_AddText(g_consoleField.buffer);
                Cbuf_AddText("\n");
            }
        }

        // copy line to history buffer
        if (g_consoleField.buffer[0]) {
            historyEditLines[nextHistoryLine % COMMAND_HISTORY] = g_consoleField;
            nextHistoryLine++;
            historyLine = nextHistoryLine;
        }

        Field_Clear(&g_consoleField);
        g_consoleField.widthInPixels = g_console_field_width;
        g_consoleField.charHeight = g_console_char_height;
        g_consoleField.fixedSize = 1;

        if (CL_AllLocalClientsDisconnected()) {
            SCR_UpdateScreen(); // the command may take some time
        }
        return;
    }

    // command completion
    if (key == K_TAB) {
        CompleteCommand();
        return;
    }

    // command history (ctrl-p ctrl-n for unix style)
    if ((key == K_MWHEELUP && keys[K_SHIFT].down) || key == K_UPARROW || key == K_KP_UPARROW
        || (tolower(key) == 'p' && keys[K_CTRL].down)) {
        if (nextHistoryLine - historyLine < COMMAND_HISTORY && historyLine > 0) {
            historyLine--;
        }
        g_consoleField = historyEditLines[historyLine % COMMAND_HISTORY];
        Field_AdjustScroll(&g_consoleField);
        return;
    }

    if ((key == K_MWHEELDOWN && keys[K_SHIFT].down) || key == K_DOWNARROW || key == K_KP_DOWNARROW
        || (tolower(key) == 'n' && keys[K_CTRL].down)) {
        if (historyLine == nextHistoryLine) {
            return;
        }
        historyLine++;
        g_consoleField = historyEditLines[historyLine % COMMAND_HISTORY];
        Field_AdjustScroll(&g_consoleField);
        return;
    }

    // console scrolling
    if (key == K_PGUP) {
        Con_PageUp();
        return;
    }

    if (key == K_PGDN) {
        Con_PageDown();
        return;
    }

    if (key == K_MWHEELUP) {
        Con_PageUp();
        // hold <ctrl> to accelerate scrolling
        if (keys[K_CTRL].down) {
            Con_PageUp();
            Con_PageUp();
        }
        return;
    }

    if (key == K_MWHEELDOWN) {
        Con_PageDown();
        if (keys[K_CTRL].down) {
            Con_PageDown();
            Con_PageDown();
        }
        return;
    }

    // ctrl-home = top of console
    if (key == K_HOME && keys[K_CTRL].down) {
        Con_Top();
        return;
    }

    // ctrl-end = bottom of console
    if (key == K_END && keys[K_CTRL].down) {
        Con_Bottom();
        return;
    }

    // pass to the normal editline routine
    Field_KeyDownEvent(&g_consoleField, key);
}

void Message_Key(int key)
{
    if (key == K_ESCAPE) {
        (clients)->keyCatchers &= ~0x10;
        Field_Clear(chatField);
        return;
    }

    if (key == K_ENTER || key == K_KP_ENTER) {
        if (chatField->buffer[0]) {
            if ((&::clc)->state == 8) {
                char buffer[1024];

                if (*chat_team) {
                    Com_sprintf(buffer, sizeof(buffer),
                                "say_team \"\x15%s\"\n", chatField->buffer);
                } else {
                    Com_sprintf(buffer, sizeof(buffer),
                                "say \"\x15%s\"\n", chatField->buffer);
                }
                CL_AddReliableCommand(buffer);
            }
        }
        (clients)->keyCatchers &= ~0x10;
        Field_Clear(chatField);
        return;
    }

    Field_KeyDownEvent(chatField, key);
}

qboolean Key_GetOverstrikeMode(void)
{
    return *key_overstrikeMode;
}

void Key_SetOverstrikeMode(qboolean state)
{
    *key_overstrikeMode = state;
}

// retail 0x40a490, unreferenced; Q3 Key_IsDown (SUPPORTED)
qboolean Key_IsDown(int keynum)
{
    if (keynum == -1)
        return 0;

    return keys[keynum].down;
}

int Key_StringToKeynum(const char *str)
{
    keyname_t *name;

    if (str == NULL || *str == '\0') {
        return -1;
    }

    if (str[1] == '\0') {
        return (signed char)str[0];
    }

    if (str[0] == '0' && str[1] == 'x' && strlen(str) == 4) {
        int n1, n2;

        n1 = str[2];
        if (I_isdigit(n1)) {
            n1 -= '0';
        } else if (n1 >= 'a' && n1 <= 'f') {
            n1 = n1 - 'a' + 10;
        } else {
            n1 = 0;
        }

        n2 = str[3];
        if (I_isdigit(n2)) {
            n2 -= '0';
        } else if (n2 >= 'a' && n2 <= 'f') {
            n2 = n2 - 'a' + 10;
        } else {
            n2 = 0;
        }

        return n1 * 16 + n2;
    }

    for (name = keynames; name->name; ++name) {
        if (I_stricmp(str, name->name) == 0) {
            return name->keynum;
        }
    }

    return -1;
}

char *Key_KeynumToString(int keynum, qboolean translate)
{
    keyname_t *name;
    int upperNibble;
    int lowerNibble;

    if (keynum == -1) {
        return "<KEY NOT FOUND>";
    }

    if (keynum < 0 || keynum > 0xff) {
        return "<OUT OF RANGE>";
    }

    if (translate && SEH_GetCurrentLanguage() == 1 && keynum >= '0' && keynum <= '9') {
        return frenchNumberKeysMap[keynum - '0'];
    }

    if (keynum > 0x20 && keynum < 0x7f && keynum != '"') {
        tinystr[0] = toupper(keynum);
        tinystr[1] = '\0';
        if (keynum != ';' || translate) {
            return tinystr;
        }
    }

    name = translate ? keynames_localized : keynames;
    while (name->name) {
        if (keynum == name->keynum) {
            return name->name;
        }
        ++name;
    }

    upperNibble = keynum >> 4;
    lowerNibble = keynum & 0xf;
    tinystr[0] = '0';
    tinystr[1] = 'x';
    tinystr[2] = upperNibble > 9 ? (char)(upperNibble + 'a' - 10) : (char)(upperNibble + '0');
    tinystr[3] = lowerNibble > 9 ? (char)(lowerNibble + 'a' - 10) : (char)(lowerNibble + '0');
    tinystr[4] = '\0';
    return tinystr;
}

void Key_SetBinding(int keynum, const char *binding)
{
    if (keynum == -1)
        return;
    ReplaceStringInternal((const char **)&keys[keynum].binding, binding);
    dvar_modifiedFlags |= 1;
}

char *Key_GetBinding(int keynum)
{
    if (keynum == -1)
        return (char *)"";
    return keys[keynum].binding;
}

// retail 0x40a6e0, unreferenced; Q3 Key_GetKey (SUPPORTED)
int Key_GetKey(const char *binding)
{
    int i;

    if (binding) {
        for (i = 0; i < 256; i++) {
            if (keys[i].binding && I_stricmp(binding, keys[i].binding) == 0)
                return i;
        }
    }
    return -1;
}

void CL_InitKeyCommands(void)
{
    Cmd_AddCommand("bind", Key_Bind_f);
    Cmd_AddCommand("unbind", Key_Unbind_f);
    Cmd_AddCommand("unbindall", Key_Unbindall_f);
    Cmd_AddCommand("bindlist", Key_Bindlist_f);
}

void Key_Unbind_f(void)
{
    int keynum;

    if (Cmd_Argc() != 2) {
        Com_Printf("unbind <key> : remove commands from a key\n");
        return;
    }

    keynum = Key_StringToKeynum(Cmd_Argv(1));
    if (keynum == -1) {
        Com_Printf("\"%s\" isn't a valid key\n", Cmd_Argv(1));
        return;
    }

    Key_SetBinding(keynum, "");
}

void Key_Unbindall_f(void)
{
    int keynum;

    for (keynum = 0; keynum < 256; ++keynum) {
        if (keys[keynum].binding != NULL && keynum != -1) {
            ReplaceStringInternal((const char **)&keys[keynum].binding, "");
            dvar_modifiedFlags |= 1;
        }
    }
}

void Key_Bind_f(void)
{
    bool hasSpace;
    int keynum;
    int argIndex;
    const char *binding;
    int argc;
    char cmd[1024];

    argc = Cmd_Argc();
    if (Cmd_Argc() < 2) {
        Com_Printf("bind <key> [command] : attach a command to a key\n");
        return;
    }

    keynum = Key_StringToKeynum(Cmd_Argv(1));
    if (keynum == -1) {
        Com_Printf("\"%s\" isn't a valid key\n", Cmd_Argv(1));
        return;
    }

    keynum = tolower(keynum);
    if (argc == 2) {
        binding = keys[keynum].binding;
        if (binding) {
            Com_Printf("\"%s\" = \"%s\"\n", Cmd_Argv(1), binding);
        } else {
            Com_Printf("\"%s\" is not bound\n", Cmd_Argv(1));
        }
        return;
    }

    cmd[0] = '\0';
    for (argIndex = 2; argIndex < argc; ++argIndex) {
        hasSpace = argc > 3 && strchr(Cmd_Argv(argIndex), ' ');
        if (hasSpace) {
            I_strncat(cmd, sizeof(cmd), "\"");
        }

        I_strncat(cmd, sizeof(cmd), Cmd_Argv(argIndex));

        if (hasSpace) {
            I_strncat(cmd, sizeof(cmd), "\"");
        }

        if (argIndex != argc - 1) {
            I_strncat(cmd, sizeof(cmd), " ");
        }
    }

    Key_SetBinding(keynum, cmd);
}

void Key_WriteBindings(fileHandle_t f)
{
    int keynum;

    FS_Printf(f, "unbindall\n");

    for (keynum = 0; keynum < 256; ++keynum) {
        const char *binding;
        const char *scan;

        binding = keys[keynum].binding;
        if (binding == NULL || *binding == '\0') {
            continue;
        }

        FS_Printf(f, "bind %s \"", Key_KeynumToString(keynum, 0));
        for (scan = keys[keynum].binding; *scan; ++scan) {
            if (*scan == '"') {
                FS_Printf(f, "\\\"");
            } else {
                FS_Printf(f, "%c", *scan);
            }
        }
        FS_Printf(f, "\"\n");
    }
}

void Key_Bindlist_f(void)
{
    int keynum;

    for (keynum = 0; keynum < 256; keynum++) {
        if (keys[keynum].binding && keys[keynum].binding[0]) {
            Com_Printf("%s \"%s\"\n", Key_KeynumToString(keynum, 0), keys[keynum].binding);
        }
    }
}

// retail 0x40acf0
static __forceinline void CL_KeyEventBody(int key, const qboolean down, const unsigned int time)
{
    char *kb;
    char cmd[1024];
    qboolean bypassMenu;

    bypassMenu = 0;

    // update auto-repeat status and BUTTON_ANY status
    keys[key].down = down;
    if (down) {
        keys[key].repeats++;
        if (keys[key].repeats == 1)
            anykeydown++;
    } else {
        keys[key].repeats = 0;
        anykeydown--;
    }

    if (key != '`' && key != '~' && !(clients->keyCatchers & 1) && keys[key].binding
        && !stricmp(keys[key].binding, "devgui")) {
        if (down && keys[key].repeats == 1) {
            Cbuf_AddText(keys[key].binding);
            return;
        }
    }

    // only a few keys repeat
    if (down && keys[key].repeats > 1) {
        if (!(clients->keyCatchers & (1 | 0x10))) {
            if (!(clients->keyCatchers & 8))
                return;
            switch (key) {
            case 0x9a:
            case 0x9b:
            case 0xa3:
            case 0xa4:
                break;
            default:
                return;
            }
        }
        if (key == '`' || key == '~' || key == 0x1b)
            return;
    }

    // console key is hardcoded, so the user can never unbind it
    if (key == '`' || key == '~') {
        if (!down)
            return;
        if (!(clients->keyCatchers & 1) && !com_sv_running->current.enabled
            && sv_disableClientConsole->current.enabled)
            return;
        if (keys[0xa0].down) {
            if (!Con_IsActive())
                Con_ToggleConsole_f();
            Con_ToggleConsoleOutput();
        } else {
            Con_ToggleConsole_f();
        }
        return;
    }

    // any key during a demo or cinematic acts like escape
    if (down && (key < 0x80 || key == 0xc8) && (clc.demoplaying || clc.state == CA_CINEMATIC || clc.state == CA_LOGO)
        && !clients->keyCatchers) {
        Dvar_SetString((dvar_t *)nextdemo, "");
        key = 0x1b;
    }

    // escape is always handled special
    if (key == 0x1b && down) {
        if (clients->keyCatchers & 0x10) {
            Message_Key(key);
            return;
        }
        if (!(clients->keyCatchers & 8)) {
            switch (clc.state) {
            case CA_ACTIVE:
                if (!clc.demoplaying && !legacyHacks->cl_serverloadwaiting) {
                    UI_SetActiveMenu(2);
                    return;
                }
                break;
            case CA_CINEMATIC:
            case CA_LOGO:
                CL_StopLogoOrCinematic();
                return;
            case CA_CONNECTING:
            case CA_CHALLENGING:
            case CA_CONNECTED:
                CL_Disconnect();
                if (com_sv_running->current.enabled)
                    legacyHacks->sv_killserver = 1;
                return;
            default:
                if (!cls.uiStarted)
                    return;
                break;
            }
            UI_SetActiveMenu(1);
            return;
        }
        UI_KeyEvent(key, down);
        return;
    }

    if (clients->cgameInitialized && CG_KeyInterceptEvent(key, down))
        return;

    // key up events only perform actions if the game key binding is
    // a button command (leading + sign)
    if (!down) {
        kb = keys[key].binding;
        if (kb && kb[0] == '+') {
            Com_sprintf(cmd, sizeof(cmd), "-%s %i %i\n", kb + 1, key, time);
            Cbuf_AddText(cmd);
        }
        if ((clients->keyCatchers & 8) && cls.uiStarted)
            UI_KeyEvent(key, down);
        return;
    }

    if (cl_bypassMouseInput && cl_bypassMouseInput->current.enabled) {
        if (key == 0xc8 || key == 0xc9 || key == 0xca || !UI_checkKeyExec(key))
            bypassMenu = 1;
    }

    // distribute the key down event to the apropriate handler
    if (clients->keyCatchers & 1) {
        Console_Key(key);
        return;
    }
    if ((clients->keyCatchers & 8) && !bypassMenu) {
        UI_KeyEvent(key, down);
        return;
    }
    if (clients->keyCatchers & 0x10) {
        Message_Key(key);
        return;
    }
    if (clc.state == CA_DISCONNECTED) {
        Console_Key(key);
        return;
    }

    // send the bound action
    kb = keys[key].binding;
    if (!kb) {
        if (key >= 0xc8)
            Com_Printf("%s is unbound, use controls menu to set.\n", Key_KeynumToString(key, 0));
    } else if (kb[0] == '+') {
        // button commands add keynum and time as parms
        Com_sprintf(cmd, sizeof(cmd), "%s %i %i\n", kb, key, time);
        Cbuf_AddText(cmd);
    } else {
        Cbuf_AddText(kb);
        Cbuf_AddText("\n");
    }
}

void CL_KeyEvent(int key, const qboolean down, const unsigned int time)
{
    CL_KeyEventBody(key, down, time);
}

void CL_CharEvent(int key)
{
    unsigned char keyCatchers;

    if (key == '`' || key == '~')
        return;

    keyCatchers = (clients)->keyCatchers;

    if (keyCatchers & 1) {
        Field_CharEvent(&g_consoleField, key);
        return;
    }
    if (keyCatchers & 8) {
        UI_KeyEvent(key | 0x400, 1);
        return;
    }
    if (keyCatchers & 0x10) {
        Field_CharEvent((field_t *)chatField, key);
        return;
    }

    if (!*((int *)&::clc)) {
        Field_CharEvent(&g_consoleField, key);
        return;
    }
}

void Key_ClearStates(void)
{
    int i;
    *anykeydown = 0;

    for (i = 0; i < 256; i++) {
        if (keys[i].down) {
            CL_KeyEventBody(i, 0, 0);
        }
        keys[i].down = 0;
        keys[i].repeats = 0;
    }
}

void Key_Shutdown(void)
{
    int keyIndex;

    CL_SwitchToLocalClient(0);

    for (keyIndex = 0; keyIndex < 256; ++keyIndex) {
        char *binding = keys[keyIndex].binding;
        if (binding != NULL) {
            Z_FreeInternal(binding);
            keys[keyIndex].binding = NULL;
        }
    }
}




