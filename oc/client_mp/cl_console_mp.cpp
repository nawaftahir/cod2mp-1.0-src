#include <ctype.h>
#include "common_types.h"
#include "imports.h"
#include "bytematch.h"

int g_console_field_width = 620;

const dvar_t *con_restricted;
const dvar_t *con_boldgamemessagetime;
const dvar_t *con_gamemessagetime;
const dvar_t *con_minicontime;
const dvar_t *con_miniconlines;
extern field_t *chatField;
extern qboolean *chat_team;
extern const vec4_t colorWhite;
extern field_t historyEditLines[];
extern qkey_t *keys;
extern clientActive_t clients[1];
extern field_t g_consoleField;
/* dvar globals */
extern const dvar_t *cg_hudSayPosition;
extern const dvar_t *cl_noprint;

float g_console_char_height = 16.0f;

extern float ceilf(float x);
extern float floorf(float x);


extern void Field_Clear(field_t *field);
extern bool Dvar_GetBool(const char *dvarName);
extern int g_console_field_width;
extern float g_console_char_height;
extern const dvar_t *con_gamemessagetime;
extern const dvar_t *con_boldgamemessagetime;
extern const dvar_t *con_minicontime;
extern const dvar_t *con_miniconlines;
extern const dvar_t *con_restricted;

extern char *CopyStringInternal(const char *in);
extern "C" void Z_FreeInternal( void *ptr );
extern unsigned char ColorIndex(unsigned char c);
extern int Cmd_Argc(void);
extern const char *Cmd_Argv( int arg );
extern void Com_Printf(const char *fmt, ...);
extern fileHandle_t FS_FOpenFileWrite(const char *filename);
extern int FS_Write(const void *buffer, int len, int h);
extern void FS_FCloseFile(fileHandle_t h);
extern void I_strncat(char *dest, int size, const char *src);
extern int I_strnicmp(const char *s1, const char *s2, int n);
extern void I_strncpyz(char *dest, const char *src, int destsize);
extern void Cmd_TokenizeString(const char *text);
extern void Dvar_ForEach(void (*callback)(const char *));
extern void Cmd_ForEach(void (*callback)(const char *));
extern void Cmd_AddCommand(const char *name, void (*function)(void));
extern dvar_t *Dvar_RegisterVec4(const char *dvarName, float x, float y, float z, float w, float min, float max, unsigned short flags);
extern dvar_t *Dvar_RegisterFloat(const char *dvarName, float value, float min, float max, unsigned short flags);
extern dvar_t *Dvar_RegisterInt(const char *dvarName, int value, int min, int max, unsigned short flags);
dvar_t *Dvar_RegisterBool(const char *dvarName, bool value, unsigned short flags);
extern dvar_t* Dvar_FindVar(const char* dvarName);
extern bool Dvar_HasLatchedValue(const dvar_t *var);
extern const char *Dvar_DisplayableValue(const dvar_t *dvar);
extern const char *Dvar_DisplayableLatchedValue(const dvar_t *dvar);
extern const char *Dvar_DisplayableResetValue(const dvar_t *dvar);
extern const char *Dvar_GetVariantString(const char *dvarName);
extern const char *Dvar_DomainToString_GetLines(unsigned char type, DvarLimits domain, char *outBuffer, int outBufferLen, int *outLineCount);
extern void CalcScreenPlacement(float *xAdj, float *yAdj, float *xScale, float *yScale, int horzAlign, int vertAlign);
extern void CalcScreenX(float *x, int align);
extern void CalcScreenY(float *y, int align);
extern FontHandle UI_GetFontHandle(int fontEnum, float scale);
extern const char *SEH_SafeTranslateString(const char *str);
extern char *va( const char *format, ... );
extern void CL_DrawText(const char *text, int maxChars, struct Font_s *font, float x, float y, int horzAlign, int vertAlign, float xScale, float yScale, const float *color, int style);
extern void Field_Draw(field_t *edit, int x, int y, int horzAlign, int vertAlign);
extern void CL_LookupColor(unsigned char c, float *color);
extern void SCR_DrawSmallStringExt(int x, int y, const char *str, const float *color);
extern void SCR_DrawConsoleString(int x, int y, const short int *text, int len, const float *color);
extern int SEH_PrintStrlen(const char *str);
extern "C" const char *getBuildNumber(void);
extern Bool Sys_IsMainThread(void);
extern int Sys_Milliseconds(void);
extern void Com_StripExtension(const char *in, char *out);
#ifndef _WIN32
extern void qsort(void *base, int nmemb, int size, int (*compar)(const void *, const void *));
#endif
extern const char **Cmd_GetAutoCompleteFileList(const char *cmd, int *fileCount, int maxCount);
extern void FS_FreeFileList(char **list, int count);
extern int strnicmp(const char *s1, const char *s2, size_t n);
extern void *__DefaultRuneLocale;
extern refexport_t re;
extern clientStatic_t cls;

static struct ConDrawInputGlob conDrawInputGlob;
static struct Console con;
static const dvar_t *con_inputBoxColor;
static const dvar_t *con_inputHintBoxColor;
static const dvar_t *con_outputBarColor;
static const dvar_t *con_outputSliderColor;
static const dvar_t *con_outputWindowColor;
extern int I_stricmp(const char *s1, const char *s2);

extern const vec4_t con_versionColor;
extern const vec4_t con_inputCommandMatchColor;
extern const vec4_t con_inputDvarMatchColor;
extern const vec4_t con_inputDvarValueColor;
extern const vec4_t con_inputDvarInactiveValueColor;
extern const vec4_t con_inputDvarInfoColor;
static int registeredIconMaterialCount;
static const char *hudMsgIconMaterials[256];

void CL_RegisterHudMsgIconMaterial(const char *name);
const char *CL_GetHudMsgIconMaterialName(int index);
void Con_ToggleConsole_f(void);
static void Con_ChatModePublic_f(void);
static void Con_ChatModeTeam_f(void);
void Con_Bottom(void);
static void Con_Dump_f(void);
void Con_Shutdown(void);
static void Con_UpdateMessageWindowLine(MessageWindow *msgwnd, qboolean linefeed, int duration);
static void Con_UpdateMessageWindow(print_msg_type_t type, qboolean linefeed, int duration);
static void Con_Linefeed(print_msg_type_t type, int duration);
void Con_AutoCompleteFromList(const char **strings, int stringCount, const char *prefix, char *completed, int sizeofCompleted);
static void ConDraw_Box(float x, float y, float w, float h, const vec_t *color);
static int ConDrawInput_CompareStrings(const void *e0, const void *e1);
static void Con_DrawStringOnHUD(int x, int y, int charHeight, int horzAlign, int vertAlign, const short int *string, int length, float alpha, float msgwndScale, qboolean centerText);
void Con_DrawSay(int y);
void Con_ToggleConsoleOutput(void);
void Con_DrawOuputWindow(void);
void Con_PageUp(void);
void Con_PageDown(void);
void Con_Top(void);
Bool Con_IsActive(void);
static void Con_Clear_f(void);
static void CL_AddConsoleInfoColor(int iFirstInfo, const vec_t *vColor);
static void Con_DrawMessageWindow(MessageWindow *msgwnd, int x, int y, int charHeight, int horzAlign, int vertAlign, float alpha, float msgwndScale, msgwnd_mode_t mode);
void Con_DrawSubtitles(int xPos, int yPos, int charHeight, float alpha, msgwnd_mode_t mode);
void Con_DrawMiniConsole(int xPos, int yPos, float alpha);
void Con_DrawBoldMessages(int xPos, int yPos, float alpha, msgwnd_mode_t mode);
void Con_DrawNotify(int xPos, int yPos, float alpha, msgwnd_mode_t mode);
static void ConDrawInput_IncrMatchCounter(const char *str);
void Con_ClearNotify(void);
void Con_ClearSubtitles(void);
static void ConDrawInput_DvarMatch(const char *str);
const char *Con_TokenizeInput(void);
Bool Con_AnySpaceAfterCommand(void);
void Con_CheckResize(void);
static void Con_OneTimeInit(void);
static int CL_ConsolePrint_AddLine(print_msg_type_t type, const char *txt, int duration, int linewidth, int color);
void CL_ConsolePrint(print_msg_type_t type, const char *txt, int duration, int linewidth);
void CL_ConsoleFixPosition(void);
void Con_Close(void);
static void ConDrawInput_AutoCompleteArg(const char **stringList, int stringCount);
static void ConDrawInput_CmdMatch(const char *str);
static void ConDrawInput_DetailedDvarMatch(const char *str);
static void ConDrawInput_DetailedCmdMatch(const char *str);
static void Con_DrawInput(void);
void Con_DrawConsole(void);
void Con_Init(void);
void CL_DeathMessagePrint(const char *attackerName, const vec_t *attackerColor, const char *victimName, const vec_t *victimColor, const char *iconShader, float iconWidth, float iconHeight, const vec_t *iconColor, bool horzFlipIcon);

void CL_RegisterHudMsgIconMaterial(const char *name);
const char *CL_GetHudMsgIconMaterialName(int index);
void Con_ToggleConsole_f(void);
static void Con_ChatModePublic_f(void);
static void Con_ChatModeTeam_f(void);
void Con_Bottom(void);
static void Con_Dump_f(void);
void Con_Shutdown(void);
static void Con_UpdateMessageWindowLine(MessageWindow *msgwnd, qboolean linefeed, int duration);
static void Con_UpdateMessageWindow(print_msg_type_t type, qboolean linefeed, int duration);
static void Con_Linefeed(print_msg_type_t type, int duration);
static int CL_ConsolePrint_AddLine(print_msg_type_t type, const char *txt, int duration, int linewidth, int color);
static void Con_UpdateMessageWindowLine_impl(MessageWindow *msgwnd, qboolean linefeed, int duration);
void Con_AutoCompleteFromList(const char **strings, int stringCount, const char *prefix, char *completed, int sizeofCompleted);
static void ConDraw_Box(float x, float y, float w, float h, const vec_t *color);
static int ConDrawInput_CompareStrings(const void *e0, const void *e1);
static void Con_DrawStringOnHUD(int x, int y, int charHeight, int horzAlign, int vertAlign, const short int *string, int length, float alpha, float msgwndScale, qboolean centerText);
void Con_DrawSay(int y);
void Con_ToggleConsoleOutput(void);
void Con_DrawOuputWindow(void);
void Con_PageUp(void);
void Con_PageDown(void);
void Con_Top(void);
Bool Con_IsActive(void);
static void Con_Clear_f(void);
static void CL_AddConsoleInfoColor(int iFirstInfo, const vec_t *vColor);
static void Con_DrawMessageWindow(MessageWindow *msgwnd, int x, int y, int charHeight, int horzAlign, int vertAlign, float alpha, float msgwndScale, msgwnd_mode_t mode);
void Con_DrawSubtitles(int xPos, int yPos, int charHeight, float alpha, msgwnd_mode_t mode);
void Con_DrawMiniConsole(int xPos, int yPos, float alpha);
void Con_DrawBoldMessages(int xPos, int yPos, float alpha, msgwnd_mode_t mode);
void Con_DrawNotify(int xPos, int yPos, float alpha, msgwnd_mode_t mode);
static void ConDrawInput_IncrMatchCounter(const char *str);
void Con_ClearNotify(void);
void Con_ClearSubtitles(void);
static void ConDrawInput_DvarMatch(const char *str);
const char *Con_TokenizeInput(void);
Bool Con_AnySpaceAfterCommand(void);
void Con_CheckResize(void);
static void Con_OneTimeInit(void);
void Con_Close(void);
static void ConDrawInput_AutoCompleteArg(const char **strings, int stringCount);
static void ConDrawInput_CmdMatch(const char *str);
void CL_ConsolePrint(print_msg_type_t type, const char *txt, int duration, int linewidth);
void CL_ConsoleFixPosition(void);
static void ConDrawInput_DetailedDvarMatch(const char *str);
static void ConDrawInput_DetailedCmdMatch(const char *str);
static void Con_DrawInput(void);
void Con_DrawConsole(void);
void Con_Init(void);
void CL_DeathMessagePrint(const char *attackerName, const vec_t *attackerColor, const char *victimName, const vec_t *victimColor, const char *iconShader, float iconWidth, float iconHeight, const vec_t *iconColor, bool horzFlipIcon);







// retail 0x4036a0, unreferenced; name unknown
void CL_InitHudMsgIconMaterials_4036a0(void)
{
    registeredIconMaterialCount = 0;
    CL_RegisterHudMsgIconMaterial("killiconcrush");
    CL_RegisterHudMsgIconMaterial("killicondied");
    CL_RegisterHudMsgIconMaterial("killiconfalling");
    CL_RegisterHudMsgIconMaterial("killiconheadshot");
    CL_RegisterHudMsgIconMaterial("killiconmelee");
    CL_RegisterHudMsgIconMaterial("killiconsuicide");
}

void Con_Shutdown(void)
{
    while (registeredIconMaterialCount != 0) {
        registeredIconMaterialCount--;
        Z_FreeInternal((void *)hudMsgIconMaterials[registeredIconMaterialCount]);
    }
}

void CL_RegisterHudMsgIconMaterial(const char *name)
{
    int index;

    for (index = 0; index < registeredIconMaterialCount; index++) {
        if (strcmp(hudMsgIconMaterials[index], name) == 0)
            return;
    }

    hudMsgIconMaterials[registeredIconMaterialCount] = CopyStringInternal(name);
    registeredIconMaterialCount++;
}

const char *CL_GetHudMsgIconMaterialName(int index)
{
    return hudMsgIconMaterials[(unsigned char)index];
}

static unsigned char CL_FindHudMsgIconMaterialIndex(const char *iconShader)
{
    int index;

    for (index = 0; index < registeredIconMaterialCount; index++) {
        if (strcmp(hudMsgIconMaterials[index], iconShader) == 0)
            return index;
    }

    return 0;
}

void Con_ToggleConsole_f(void)
{
    char *field;

    if (con_restricted->current.enabled) {
        if (*(int *)((char *)((void *)keys) + 0x780))
            goto toggle;
        if (!((clients)->keyCatchers & 1))
            return;
    }
toggle:
    field = (char *)&g_consoleField;
    Field_Clear((field_t *)(field));
    ((field_t *)field)->widthInPixels = g_console_field_width;
    ((field_t *)field)->charHeight = g_console_char_height;
    ((field_t *)field)->fixedSize = 1;
    con.outputVisible = 0;
    clients[0].keyCatchers ^= 1;
}

// retail 0x4038a0/0x4038b0/0x4038d0, unreferenced; names unknown
void Con_Open_4038a0(void)
{
    if (!(clients[0].keyCatchers & 1))
        Con_ToggleConsole_f();
}

void Con_ShowOutput_4038b0(void)
{
    if (clients[0].keyCatchers & 1)
        con.outputVisible = 1;
}

void Con_Hide_4038d0(void)
{
    if (clients[0].keyCatchers & 1)
        Con_ToggleConsole_f();
}

static void Con_ChatModePublic_f(void)
{
    *chat_team = 0;
    Field_Clear(chatField);
    chatField->widthInPixels = 0x24c;
    chatField->charHeight = 10.0f;
    chatField->fixedSize = 0;
    clients->keyCatchers ^= 0x10;
}

static void Con_ChatModeTeam_f(void)
{
    *chat_team = 1;
    Field_Clear(chatField);
    chatField->widthInPixels = 0x21f;
    chatField->charHeight = 10.0f;
    chatField->fixedSize = 0;
    clients->keyCatchers ^= 0x10;
}

static void Con_Clear_f(void)
{
    int i;
    for (i = 0; i < 65536; i++) {
        con.textBuffer[i] = (ColorIndex(0x37) << 8) | 0x20;
    }
    con.display = con.currentLine;
}

static void Con_Dump_f(void)
{
    int l, x, i;
    short *line;
    int f;
    char buffer[0x400];

    if (Cmd_Argc() != 2) {
        Com_Printf("usage: condump <filename>\n");
        return;
    }

    Com_Printf("Dumped console text to %s.\n", Cmd_Argv(1));

    f = FS_FOpenFileWrite(Cmd_Argv(1));
    if (!f) {
        Com_Printf("ERROR: couldn't open.\n");
        return;
    }

    // skip empty lines
    for (l = con.currentLine - con.totallines + 1; l <= con.currentLine; l++) {
        line = con.textBuffer + (l % con.totallines) * con.linewidth;
        for (x = 0; x < con.linewidth; x++)
            if ((line[x] & 0xff) != ' ')
                break;
        if (x != con.linewidth)
            break;
    }

    // write the remaining lines
    buffer[con.linewidth] = 0;
    for (; l <= con.currentLine; l++) {
        line = con.textBuffer + (l % con.totallines) * con.linewidth;
        for (i = 0; i < con.linewidth; i++)
            buffer[i] = line[i] & 0xff;
        for (x = con.linewidth - 1; x >= 0; x--) {
            if (buffer[x] == ' ')
                buffer[x] = 0;
            else
                break;
        }
        I_strncat(buffer, 0x400, "\n");
        FS_Write(buffer, strlen(buffer), f);
    }

    FS_FCloseFile(f);
}

static inline void MsgWnd_Clear(MessageWindow *wnd)
{

    int byteSize = wnd->count * 164;
    memset(wnd->lines, 0, byteSize);
    wnd->current_line = 0;
}

void Con_ClearNotify(void)
{
    MsgWnd_Clear(&con.messageBuffer->gamemsg);
    MsgWnd_Clear(&con.messageBuffer->boldgamemsg);
}

// retail 0x403be0, unreferenced; CoD4 Con_ClearMiniConsole (SUPPORTED)
void Con_ClearMiniConsole(void)
{
    MsgWnd_Clear(&con.messageBuffer->minicon);
}

void Con_ClearSubtitles(void)
{
    MsgWnd_Clear(&con.messageBuffer->subtitle);
}

void Con_CheckResize(void)
{
    int i;
    int j;
    int width;
    int oldwidth;
    int oldtotallines;
    int numlines;
    int numchars;
    short *tbuf;

    LargeLocal largeLocal(0x20000);
    tbuf = (short *)largeLocal.GetBuf();

    con.screenMin[0] = 4.0f;
    con.screenMin[1] = 4.0f;
    con.screenMax[0] = -4.0f;
    con.screenMax[1] = -4.0f;

    CalcScreenX(&con.screenMin[0], 1);
    CalcScreenY(&con.screenMin[1], 1);
    CalcScreenX(&con.screenMax[0], 3);
    CalcScreenY(&con.screenMax[1], 3);

    con.screenMin[0] = floor(con.screenMin[0]);
    con.screenMin[1] = floor(con.screenMin[1]);
    con.screenMax[0] = floor(con.screenMax[0]);
    con.screenMax[1] = floor(con.screenMax[1]);

    width = (int)(con.screenMax[0] - con.screenMin[0]);
    if (width < 640)
        width = 640;
    width = width / 8 - 2;

    if (width == con.linewidth)
        return;

    if (width < 1) {
        width = 78;
        con.linewidth = width;
        con.totallines = 0x10000 / con.linewidth;
        for (i = 0; i < 0x10000; i++)
            con.textBuffer[i] = (short)((ColorIndex(0x37) << 8) | 0x20);
    } else {
        oldwidth = con.linewidth;
        con.linewidth = width;
        oldtotallines = con.totallines;
        con.totallines = 0x10000 / con.linewidth;
        numlines = oldtotallines;

        if (con.totallines < numlines)
            numlines = con.totallines;

        numchars = oldwidth;

        if (con.linewidth < numchars)
            numchars = con.linewidth;

        memcpy(tbuf, con.textBuffer, 0x20000);
        for (i = 0; i < 0x10000; i++)
            con.textBuffer[i] = (short)((ColorIndex(0x37) << 8) | 0x20);

        for (i = 0; i < numlines; i++) {
            for (j = 0; j < numchars; j++) {
                con.textBuffer[(con.totallines - 1 - i) * con.linewidth + j] =
                    tbuf[((con.currentLine - i + oldtotallines) % oldtotallines) * oldwidth + j];
            }
        }

        MsgWnd_Clear(&con.messageBuffer->gamemsg);
        MsgWnd_Clear(&con.messageBuffer->boldgamemsg);
        MsgWnd_Clear(&con.messageBuffer->minicon);
        MsgWnd_Clear(&con.messageBuffer->subtitle);
    }

    con.currentLine = con.totallines - 1;
    con.display = con.currentLine;
}

// Name from CoD4; original CoD2 name unknown.
static void Con_InitMessageWindow(MessageWindow *msgwnd, MessageLine *lines, int lineCount, int padding, int scrollTime, int fadeIn, int fadeOut)
{
    msgwnd->lines = lines;
    msgwnd->current_line = 0;
    msgwnd->count = lineCount;
    msgwnd->padding = padding;
    msgwnd->scrolltime = scrollTime;
    msgwnd->fadein = fadeIn;
    msgwnd->fadeout = fadeOut;
}

void Con_Init(void)
{
    field_t *consoleField;
    field_t *history;
    int i;

    con_restricted = Dvar_RegisterBool("con_restricted", 0, 0x1001);

    consoleField = (&g_consoleField);
    Field_Clear((field_t *)(consoleField));
    consoleField->widthInPixels = g_console_field_width;
    consoleField->charHeight = g_console_char_height;
    consoleField->fixedSize = 1;

    history = (field_t *)((void *)&historyEditLines);
    for (i = 0; i < 32; i++) {
        Field_Clear((field_t *)(&history[i]));
        history[i].widthInPixels = g_console_field_width;
        history[i].charHeight = g_console_char_height;
        history[i].fixedSize = 1;
    }

    Cmd_AddCommand("toggleconsole", Con_ToggleConsole_f);
    Cmd_AddCommand("chatmodepublic", Con_ChatModePublic_f);
    Cmd_AddCommand("chatmodeteam", Con_ChatModeTeam_f);
    Cmd_AddCommand("clear", Con_Clear_f);
    Cmd_AddCommand("condump", Con_Dump_f);

    registeredIconMaterialCount = 0;
    CL_RegisterHudMsgIconMaterial("killiconcrush");
    CL_RegisterHudMsgIconMaterial("killicondied");
    CL_RegisterHudMsgIconMaterial("killiconfalling");
    CL_RegisterHudMsgIconMaterial("killiconheadshot");
    CL_RegisterHudMsgIconMaterial("killiconmelee");
    CL_RegisterHudMsgIconMaterial("killiconsuicide");
}

static void Con_CopyCurrentConsoleLineText(short *text)
{
    int len;
    int textOffset;
    int i;

    len = con.linewidth;
    if (len >= 78)
        len = 78;

    textOffset = (con.currentLine % con.totallines) * con.linewidth;
    for (i = 0; i < len; i++)
        text[i] = con.textBuffer[textOffset + i];

    for (i = len; i < 78; i++)
        text[i] = (short)((ColorIndex(0x37) << 8) | 0x20);
}

static void Con_UpdateMessageWindowLine(MessageWindow *msgwnd, qboolean linefeed, int duration)
{
    MessageLine *line;
    int i;

    line = &msgwnd->lines[msgwnd->current_line];
    line->startTime = clients->serverTime;
    line->endTime = clients->serverTime + duration;
    Con_CopyCurrentConsoleLineText(line->textBuffer);

    if (!linefeed || msgwnd->count <= 0)
        return;

    msgwnd->current_line = (msgwnd->current_line + 1) % msgwnd->count;

    for (i = 0; i < msgwnd->padding; i++) {
        line = &msgwnd->lines[(msgwnd->current_line + i) % msgwnd->count];
        if (line->endTime - msgwnd->fadeout > clients->serverTime) {
            line->startTime += clients->serverTime - (line->endTime - msgwnd->fadeout);
            line->endTime = msgwnd->fadeout + clients->serverTime;
        }
    }
}

static void Con_UpdateMessageWindow(print_msg_type_t type, qboolean linefeed, int duration)
{
    if (con.currentLine < 0)
        return;

    switch (type) {
    case PMSG_CONSOLE:
        Con_UpdateMessageWindowLine(&con.messageBuffer->minicon, linefeed, duration);
        break;
    case PMSG_GAME:
        Con_UpdateMessageWindowLine(&con.messageBuffer->gamemsg, linefeed, duration);
        break;
    case PMSG_BOLDGAME:
        Con_UpdateMessageWindowLine(&con.messageBuffer->boldgamemsg, linefeed, duration);
        break;
    case PMSG_SUBTITLE:
        Con_UpdateMessageWindowLine(&con.messageBuffer->subtitle, linefeed, duration);
        break;
    }
}

static void Con_Linefeed(print_msg_type_t type, int duration)
{
    int i;

    Con_UpdateMessageWindow(type, 1, duration);

    con.lineOffset = 0;
    if (con.display == con.currentLine)
        con.display++;
    con.currentLine++;

    for (i = 0; i < con.linewidth; i++)
        con.textBuffer[(con.currentLine % con.totallines) * con.linewidth + i] = (short)((ColorIndex(0x37) << 8) | 0x20);
}

static void Con_InitMessageBuffers(void)
{
    MessageBuffer *messageBuffer;

    messageBuffer = con.messageBufferArray;

    Con_InitMessageWindow(&messageBuffer->gamemsg, messageBuffer->gamemsg_lines, 8, 3, 250, 250, 500);
    Con_InitMessageWindow(&messageBuffer->boldgamemsg, messageBuffer->boldgamemsg_lines, 8, 3, 250, 250, 500);
    Con_InitMessageWindow(&messageBuffer->subtitle, messageBuffer->subtitle_lines, 8, 3, 250, 250, 500);
    Con_InitMessageWindow(&messageBuffer->minicon, messageBuffer->minicon_lines, con_miniconlines->current.integer, 0, 0, 0, 0);

    con.messageBuffer = messageBuffer;
}

static void Con_OneTimeInit(void)
{
    con_inputBoxColor = Dvar_RegisterVec4("con_inputBoxColor", 0.25f, 0.25f, 0.2f, 1.0f, 0.0f, 1.0f, 0x1001);
    con_inputHintBoxColor = Dvar_RegisterVec4("con_inputHintBoxColor", 0.4f, 0.4f, 0.35f, 1.0f, 0.0f, 1.0f, 0x1001);
    con_outputBarColor = Dvar_RegisterVec4("con_outputBarColor", 1.0f, 1.0f, 0.95f, 0.6f, 0.0f, 1.0f, 0x1001);
    con_outputSliderColor = Dvar_RegisterVec4("con_outputSliderColor", 0.15f, 0.15f, 0.1f, 0.6f, 0.0f, 1.0f, 0x1001);
    con_outputWindowColor = Dvar_RegisterVec4("con_outputWindowColor", 0.35f, 0.35f, 0.3f, 0.75f, 0.0f, 1.0f, 0x1001);

    con_gamemessagetime = Dvar_RegisterFloat("con_gamemessagetime", 5.0f, 0.0f, 3.402823466e38f, 0x1000);
    con_boldgamemessagetime = Dvar_RegisterFloat("con_boldgamemessagetime", 8.0f, 0.0f, 3.402823466e38f, 0x1000);
    con_minicontime = Dvar_RegisterFloat("con_minicontime", 4.0f, 0.0f, 3.402823466e38f, 0x1001);
    con_miniconlines = Dvar_RegisterInt("con_miniconlines", 5, 0, 100, 0x1001);

    Con_InitMessageBuffers();


    con.color[0] = 1.0f;
    con.color[1] = 1.0f;
    con.color[2] = 1.0f;
    con.color[3] = 1.0f;
    con.linewidth = -1;
    Con_CheckResize();
    con.initialized = 1;
}

static int CL_ConsolePrint_AddLine(print_msg_type_t type, const char *txt, int duration, int linewidth, int color)
{
    int c;
    int l;
    int lineBroken;
    int targetLineWidth;
    int maxLinewidth;
    float printLen;
    const char *p;

    if (type == PMSG_CONSOLE || type == PMSG_LOGFILE)
        maxLinewidth = con.linewidth;
    else
        maxLinewidth = con.linewidth - 78 < 0 ? con.linewidth : 78;

    if (linewidth <= 0 || linewidth > maxLinewidth)
        linewidth = maxLinewidth;

    targetLineWidth = linewidth;
    if (type == PMSG_GAME || type == PMSG_BOLDGAME) {
        l = SEH_PrintStrlen(txt);
        if (l > linewidth) {
            printLen = (float)l;
            targetLineWidth = (int)(printLen / (int)ceil(printLen / (float)linewidth));
        }
    }

    if (type != con.prevType && con.lineOffset > 0)
        Con_Linefeed(con.prevType, duration);

    p = txt;
    lineBroken = 0;
    while ((c = *(unsigned char *)p) != 0) {
        if (*p == '^' && p[1] && p[1] != '^' && (unsigned char)p[1] >= '0' && (unsigned char)p[1] <= '9') {
            color = ColorIndex(p[1]);
            p += 2;
            continue;
        }

        for (l = 0; l < linewidth; l++) {
            if ((unsigned char)p[l] <= ' ')
                break;
        }

        if (l != linewidth && con.lineOffset + l > linewidth) {
            Con_Linefeed(type, duration);
            lineBroken = 1;
        }

        p++;

        switch (c) {
        case '\n':
            Con_Linefeed(type, duration);
            break;
        case '\r':
            con.lineOffset = 0;
            break;
        default:
            if (!con.lineOffset && c == ' ' && lineBroken)
                break;
            con.textBuffer[(con.currentLine % con.totallines) * con.linewidth + con.lineOffset] = (short)((color << 8) | c);
            con.lineOffset++;
            if (con.lineOffset >= linewidth || (con.lineOffset >= targetLineWidth && c == ' ')) {
                Con_Linefeed(type, duration);
                lineBroken = 1;
            }
            break;
        }
    }

    if (con.lineOffset > 0) {
        if (type)
            Con_Linefeed(type, duration);
        else
            Con_UpdateMessageWindow(type, 0, duration);
    }

    con.prevType = type;
    return color;
}

void CL_ConsolePrint(print_msg_type_t type, const char *txt, int duration, int linewidth)
{
    const dvar_t *noPrint;
    int color;

    noPrint = cl_noprint;
    if (!noPrint)
        return;
    if (noPrint->current.enabled)
        return;
    if (type == PMSG_LOGFILE)
        return;

    if (!con.initialized)
        Con_OneTimeInit();

    if (!duration) {
        switch (type) {
        case PMSG_CONSOLE:
            duration = I_fround(con_minicontime->current.value * 1000.0f);
            break;
        case PMSG_GAME:
            duration = I_fround(con_gamemessagetime->current.value * 1000.0f);
            break;
        case PMSG_BOLDGAME:
            duration = I_fround(con_boldgamemessagetime->current.value * 1000.0f);
            break;
        case PMSG_SUBTITLE:
            duration = 5000;
        default:
            goto skipClamp;
        }
    }

    if (duration < 0)
        duration = 0;

skipClamp:

    color = ColorIndex(0x37);

    if (type == PMSG_GAME || type == PMSG_BOLDGAME) {
        const char *newline;
        char lineText[0x1000];

        while ((newline = strchr(txt, '\n')) != NULL) {
            unsigned int copyCount = (unsigned int)(newline - txt) + 1;

            if (copyCount > 0xfff)
                copyCount = 0xfff;

            memcpy(lineText, txt, copyCount);
            lineText[copyCount] = '\0';
            color = CL_ConsolePrint_AddLine(type, lineText, duration, linewidth, color);
            txt = newline + 1;
        }
    }

    CL_ConsolePrint_AddLine(type, txt, duration, linewidth, color);
}

void CL_ConsoleFixPosition(void)
{
    CL_ConsolePrint(PMSG_CONSOLE, "\n", 0, 0);
    con.display = con.currentLine - 1;
}

static void CL_AddConsoleChar(int ch, unsigned char colorIdx)
{
    con.textBuffer[(con.currentLine % con.totallines) * con.linewidth + con.lineOffset] = (colorIdx << 8) | ch;
    con.lineOffset++;
}

static float I_fsel(const float x, const float y, const float z)
{
    return x >= 0.0f ? y : z;
}
static float I_fclamp(const float val, const float min, const float max)
{
    return I_fsel(min - val, min, I_fsel(val - max, max, val));
}
static void CL_AddConsoleInfoColor(int iFirstInfo, const vec_t *vColor)
{
    con.textBuffer[(con.currentLine % con.totallines) * con.linewidth + con.lineOffset] = (iFirstInfo << 8) | (int)I_fclamp((float)(int)(vColor[0] * 255.0f), 0.0f, 255.0f);
    con.lineOffset++;
    con.textBuffer[(con.currentLine % con.totallines) * con.linewidth + con.lineOffset] = ((iFirstInfo + 1) << 8) | (int)I_fclamp((float)(int)(vColor[1] * 255.0f), 0.0f, 255.0f);
    con.lineOffset++;
    con.textBuffer[(con.currentLine % con.totallines) * con.linewidth + con.lineOffset] = ((iFirstInfo + 2) << 8) | (int)I_fclamp((float)(int)(vColor[2] * 255.0f), 0.0f, 255.0f);
    con.lineOffset++;
}

static void CL_AddDeathMessageText(const char *txt, int color)
{
    int row;
    int curColor;
    int c;

    if (color < 0)
        curColor = 7;
    else
        curColor = color;

    row = con.currentLine % con.totallines;
    while ((c = (unsigned char)*txt) != 0 && con.lineOffset < con.linewidth) {
        if (*txt == '^' && txt[1] && txt[1] != '^' && txt[1] >= '0' && txt[1] <= '9') {
            if (color < 0)
                curColor = ColorIndex(txt[1]);
            txt += 2;
            continue;
        }
        txt++;
        if (c == '\n' || c == '\r')
            continue;
        con.textBuffer[row * con.linewidth + con.lineOffset] = (curColor << 8) | c;
        con.lineOffset++;
    }
}

void CL_DeathMessagePrint(const char *attackerName, const vec_t *attackerColor, const char *victimName, const vec_t *victimColor, const char *iconShader, float iconWidth, float iconHeight, const vec_t *iconColor, bool horzFlipIcon)
{
    int duration;

    if (cl_noprint && cl_noprint->current.enabled)
        return;

    if (!con.initialized)
        Con_OneTimeInit();

    duration = I_fround(con_gamemessagetime->current.value * 1000.0f);
    if (duration < 0)
        duration = 0;

    if (con.lineOffset > 0)
        Con_Linefeed(con.prevType, duration);

    if (*attackerName) {
        CL_AddConsoleInfoColor(10, attackerColor);
        CL_AddDeathMessageText(attackerName, 7);
        CL_AddDeathMessageText(" ", 7);
    }

    CL_AddConsoleInfoColor(13, iconColor);
    if (horzFlipIcon)
        CL_AddConsoleChar((int)(iconWidth * 32.0f), 0x13);
    else
        CL_AddConsoleChar((int)(iconWidth * 32.0f), 0x10);
    CL_AddConsoleChar((int)(iconHeight * 32.0f), 0x11);
    CL_AddConsoleChar(CL_FindHudMsgIconMaterialIndex(iconShader), 0x12);
    CL_AddDeathMessageText(" ", 7);

    CL_AddConsoleInfoColor(10, victimColor);
    if ((int)strlen(victimName) + con.lineOffset + 3 >= con.linewidth) {
        Con_Linefeed(PMSG_GAME, duration);
        CL_AddConsoleInfoColor(10, victimColor);
    }

    CL_AddDeathMessageText(victimName, 7);
    Con_Linefeed(PMSG_GAME, duration);
    con.prevType = PMSG_GAME;
}

void Con_AutoCompleteFromList(const char **strings, int stringCount, const char *prefix, char *completed, int sizeofCompleted)
{
    int prefixLen = strlen(prefix);
    int stringIndex;
    int i;

    completed[0] = '\0';

    for (stringIndex = 0; stringIndex < stringCount; stringIndex++) {
        const char *string = strings[stringIndex];

        if (strnicmp(prefix, string, prefixLen) != 0)
            continue;

        if (completed[0] == '\0') {

            I_strncpyz(completed, string, sizeofCompleted);
        } else {

            for (i = prefixLen; string[i] == completed[i] && completed[i]; i++)
                ;
            completed[i] = '\0';
        }
    }
}

const char *Con_TokenizeInput(void)
{
    const char *cmd;

    Cmd_TokenizeString(g_consoleField.buffer);
    cmd = Cmd_Argv(0);
    if (*cmd == '\\' || *cmd == '/')
        ++cmd;
    while (isspace(*cmd))
        ++cmd;
    return cmd;
}

Bool Con_AnySpaceAfterCommand(void)
{
    int charIndex;

    for (charIndex = 0; isspace(g_consoleField.buffer[charIndex]); ++charIndex)
        ;
    while (g_consoleField.buffer[charIndex]) {
        if (isspace(g_consoleField.buffer[charIndex]))
            return 1;
        ++charIndex;
    }
    return 0;
}

// retail 0x404fc0, unreferenced; name unknown
int ConDrawInput_TextHeight_404fc0(FontHandle font)
{
    return re.TextHeight(font);
}

static int ConDrawInput_TextWidth(const char *text)
{
    return re.TextWidth(text, 0, cls.consoleFont);
}

static void ConDrawInput_Text(const char *str, const vec_t *color)
{
    re.DrawText(str, 0x7fffffff, cls.consoleFont, conDrawInputGlob.x, conDrawInputGlob.y + conDrawInputGlob.fontHeight, 1.0f, 1.0f, color, 0);
}

static void ConDrawInput_DrawText(const char *text, int maxChars, const vec_t *color)
{
    FontHandle font = cls.consoleFont;

    re.DrawText(text, maxChars, font, conDrawInputGlob.x, conDrawInputGlob.y + conDrawInputGlob.fontHeight, 1.0f, 1.0f, color, 0);
}

static void ConDrawInput_NextLine(void)
{
    conDrawInputGlob.y += conDrawInputGlob.fontHeight;
    conDrawInputGlob.x = conDrawInputGlob.leftX;
}

// retail 0x405090, unreferenced; name unknown
void ConDrawInput_Over_405090(int width)
{
    conDrawInputGlob.x += (float)width;
}

static void ConDrawInput_TextAndOver(const char *str, const vec_t *color)
{
    ConDrawInput_Text(str, color);
    conDrawInputGlob.x += (float)re.TextWidth(str, 0, cls.consoleFont);
}

static void ConDraw_Box(float x, float y, float w, float h, const vec_t *color)
{
    clientStatic_t *cls_ = &cls;
    vec4_t darkColor;

    re.DrawStretchPic(x, y, w, h, 0.0f, 0.0f, 0.0f, 0.0f, color, cls_->whiteMaterial);

    darkColor[0] = color[0] * 0.5f;
    darkColor[1] = color[1] * 0.5f;
    darkColor[2] = color[2] * 0.5f;
    darkColor[3] = color[3];

    re.DrawStretchPic(x, y, 2.0f, h, 0.0f, 0.0f, 0.0f, 0.0f, darkColor, cls_->whiteMaterial);
    re.DrawStretchPic(x + w - 2.0f, y, 2.0f, h, 0.0f, 0.0f, 0.0f, 0.0f, darkColor, cls_->whiteMaterial);
    re.DrawStretchPic(x, y, w, 2.0f, 0.0f, 0.0f, 0.0f, 0.0f, darkColor, cls_->whiteMaterial);
    re.DrawStretchPic(x, y + h - 2.0f, w, 2.0f, 0.0f, 0.0f, 0.0f, 0.0f, darkColor, cls_->whiteMaterial);
}

static void ConDrawInput_Box(int lines, const vec_t *color)
{
    float x;
    float y;
    float w;
    float h;

    x = conDrawInputGlob.x - 6.0f;
    y = conDrawInputGlob.y - 6.0f;
    w = con.screenMax[0] - con.screenMin[0] - (x - con.screenMin[0]);
    h = (float)lines * conDrawInputGlob.fontHeight + 12.0f;
    ConDraw_Box(x, y, w, h, color);
}

// retail 0x4052a0, unreferenced; name unknown
int ConDrawInput_IsMatch_4052a0(const char *str)
{
    return I_strnicmp(str, conDrawInputGlob.inputText, conDrawInputGlob.inputTextLen) == 0;
}

static void ConDrawInput_IncrMatchCounter(const char *str)
{
    if (I_strnicmp(str, conDrawInputGlob.inputText, conDrawInputGlob.inputTextLen) != 0)
        return;

    conDrawInputGlob.matchCount++;

    if (str[conDrawInputGlob.inputTextLen] == '\0')
        conDrawInputGlob.hasExactMatch = 1;
}

static void ConDrawInput_DvarMatch(const char *str)
{
    clientStatic_t *cls_;
    const char *value;

    if (I_strnicmp(str, conDrawInputGlob.inputText, conDrawInputGlob.inputTextLen) != 0)
        return;

    cls_ = &cls;

    re.DrawText(str, 0x18, cls_->consoleFont, conDrawInputGlob.x, conDrawInputGlob.y + conDrawInputGlob.fontHeight, 1.0f, 1.0f, con_inputDvarMatchColor, 0);
    conDrawInputGlob.x += 200.0f;

    value = Dvar_GetVariantString(str);
    re.DrawText(value, 0x28, cls_->consoleFont, conDrawInputGlob.x, conDrawInputGlob.y + conDrawInputGlob.fontHeight, 1.0f, 1.0f, con_inputDvarValueColor, 0);

    conDrawInputGlob.y += conDrawInputGlob.fontHeight;
    conDrawInputGlob.x = conDrawInputGlob.leftX;
}

static int ConDrawInput_CompareStrings(const void *e0, const void *e1)
{
    return I_stricmp(*(const char **)e0, *(const char **)e1);
}

static int ConDrawInput_TextFieldFirstArgChar(void)
{
    int charIndex;

    for (charIndex = 0; isspace(g_consoleField.buffer[charIndex]); ++charIndex)
        ;
    while (!isspace(g_consoleField.buffer[charIndex]))
        ++charIndex;
    while (isspace(g_consoleField.buffer[charIndex]))
        ++charIndex;
    return charIndex;
}

static void ConDrawInput_AutoCompleteArg(const char **stringList, int stringCount)
{
    const char *prefix;
    int prefixLen;
    int matchCount;
    int matchBufferUsed;
    int matchLenMax;
    int matchLen;
    int stringIndex;
    int matchIndex;
    const char *matches[16];
    char matchBuffer[1024];
    float x;
    float y;
    float w;
    float h;

    prefix = Cmd_Argv(1);
    prefixLen = strlen(prefix);
    matchCount = 0;
    matchBufferUsed = 0;
    matchLenMax = 0;
    for (stringIndex = 0; stringIndex < stringCount; ++stringIndex) {
        if (!I_strnicmp(prefix, stringList[stringIndex], prefixLen)) {
            if (matchCount == 16)
                return;
            if (matchBufferUsed + strlen(stringList[stringIndex]) >= 0x400)
                return;
            matches[matchCount] = &matchBuffer[matchBufferUsed];
            Com_StripExtension(stringList[stringIndex], (char *)matches[matchCount]);
            matchBufferUsed += strlen(matches[matchCount]) + 1;
            matchLen = ConDrawInput_TextWidth(matches[matchCount]);
            if (matchLenMax < matchLen)
                matchLenMax = matchLen;
            ++matchCount;
        }
    }
    if (matchCount) {
        qsort(matches, matchCount, sizeof(matches[0]), ConDrawInput_CompareStrings);
        x = (float)re.TextWidth(g_consoleField.buffer, ConDrawInput_TextFieldFirstArgChar(), cls.consoleFont) + conDrawInputGlob.leftX - 6.0f;
        y = con.screenMin[1] + conDrawInputGlob.fontHeight + 6.0f;
        w = (float)matchLenMax + 12.0f;
        h = (float)matchCount * conDrawInputGlob.fontHeight + 12.0f;
        ConDraw_Box(x, y, w, h, con_inputHintBoxColor->current.vector);
        conDrawInputGlob.x = x + 6.0f;
        conDrawInputGlob.y = y + 6.0f;
        conDrawInputGlob.leftX = conDrawInputGlob.x;
        for (matchIndex = 0; matchIndex < matchCount; ++matchIndex) {
            ConDrawInput_Text(matches[matchIndex], con_inputDvarInfoColor);
            conDrawInputGlob.y += conDrawInputGlob.fontHeight;
            conDrawInputGlob.x = conDrawInputGlob.leftX;
        }
    }
}

static void ConDrawInput_DetailedDvarMatch(const char *str)
{
    const dvar_t *dvar;
    bool hasLatchedValue;
    int lineCount;
    char dvarInfo[1024];

    if (I_strnicmp(str, conDrawInputGlob.inputText, conDrawInputGlob.inputTextLen))
        return;
    if (conDrawInputGlob.hasExactMatch && str[conDrawInputGlob.inputTextLen])
        return;

    dvar = Dvar_FindVar(str);
    hasLatchedValue = Dvar_HasLatchedValue(dvar);
    lineCount = hasLatchedValue ? 3 : 2;
    ConDrawInput_Box(lineCount, con_inputHintBoxColor->current.vector);
    ConDrawInput_DrawText(str, 24, con_inputDvarMatchColor);
    conDrawInputGlob.x += 200.0f;
    ConDrawInput_DrawText(Dvar_DisplayableValue(dvar), 40, con_inputDvarValueColor);
    conDrawInputGlob.y = conDrawInputGlob.y + conDrawInputGlob.fontHeight;
    conDrawInputGlob.x = conDrawInputGlob.leftX;
    if (hasLatchedValue) {
        ConDrawInput_Text("  latched value", con_inputDvarInactiveValueColor);
        conDrawInputGlob.x += 200.0f;
        ConDrawInput_DrawText(Dvar_DisplayableLatchedValue(dvar), 40, con_inputDvarInactiveValueColor);
        conDrawInputGlob.y += conDrawInputGlob.fontHeight;
        conDrawInputGlob.x = conDrawInputGlob.leftX;
    }
    ConDrawInput_Text("  default", con_inputDvarInactiveValueColor);
    conDrawInputGlob.x += 200.0f;
    ConDrawInput_DrawText(Dvar_DisplayableResetValue(dvar), 40, con_inputDvarInactiveValueColor);
    ConDrawInput_NextLine();
    ConDrawInput_NextLine();
    Dvar_DomainToString_GetLines(dvar->type, dvar->domain, dvarInfo, sizeof(dvarInfo), &lineCount);
    ConDrawInput_Box(lineCount + 1, con_inputHintBoxColor->current.vector);
    ConDrawInput_Text(dvarInfo, con_inputDvarInfoColor);
    conDrawInputGlob.y += conDrawInputGlob.fontHeight;
    conDrawInputGlob.x = conDrawInputGlob.leftX;
    if (dvar->type == DVAR_TYPE_ENUM && Cmd_Argc() == 2)
        ConDrawInput_AutoCompleteArg(dvar->domain.enumeration.strings, dvar->domain.enumeration.stringCount);
}

static void ConDrawInput_DetailedCmdMatch(const char *str)
{
    int fileCount;
    const char **files;

    if (I_strnicmp(str, conDrawInputGlob.inputText, conDrawInputGlob.inputTextLen))
        return;
    if (conDrawInputGlob.hasExactMatch && str[conDrawInputGlob.inputTextLen])
        return;

    ConDrawInput_Box(1, con_inputHintBoxColor->current.vector);
    ConDrawInput_Text(str, con_inputCommandMatchColor);
    conDrawInputGlob.y += conDrawInputGlob.fontHeight;
    conDrawInputGlob.x = conDrawInputGlob.leftX;
    if (Cmd_Argc() == 2) {
        files = Cmd_GetAutoCompleteFileList(str, &fileCount, 10);
        if (fileCount) {
            ConDrawInput_AutoCompleteArg(files, fileCount);
            FS_FreeFileList((char **)files, 10);
        }
    }
}

static void ConDrawInput_CmdMatch(const char *str)
{
    if (I_strnicmp(str, conDrawInputGlob.inputText, conDrawInputGlob.inputTextLen) != 0)
        return;

    ConDrawInput_DrawText(str, 0x7fffffff, con_inputCommandMatchColor);
    ConDrawInput_NextLine();
}

static void Con_DrawInput(void)
{
    const char *promptString;
    int matchCount;

    if (!(clients[0].keyCatchers & 1))
        return;
    if (!Sys_IsMainThread())
        return;

    promptString = va("%s: %s> ", "CoD2 MP", COD2_VERSION_SHORT);
    conDrawInputGlob.fontHeight = (float)re.TextHeight(cls.consoleFont);
    conDrawInputGlob.x = con.screenMin[0] + 6.0f;
    conDrawInputGlob.y = con.screenMin[1] + 6.0f;
    conDrawInputGlob.leftX = conDrawInputGlob.x;
    ConDrawInput_Box(1, con_inputBoxColor->current.vector);
    ConDrawInput_TextAndOver(promptString, con_versionColor);
    conDrawInputGlob.leftX = conDrawInputGlob.x;
    g_consoleField.widthInPixels = (int)(con.screenMax[0] - 6.0f - conDrawInputGlob.x);
    Field_Draw(&g_consoleField, (int)conDrawInputGlob.x, (int)conDrawInputGlob.y, 5, 5);
    conDrawInputGlob.y += conDrawInputGlob.fontHeight;
    conDrawInputGlob.x = conDrawInputGlob.leftX;

    conDrawInputGlob.inputText = Con_TokenizeInput();
    conDrawInputGlob.inputTextLen = strlen(conDrawInputGlob.inputText);
    if (!conDrawInputGlob.inputTextLen)
        return;

    conDrawInputGlob.hasExactMatch = 0;
    conDrawInputGlob.matchCount = 0;
    Dvar_ForEach(ConDrawInput_IncrMatchCounter);
    Cmd_ForEach(ConDrawInput_IncrMatchCounter);

    matchCount = conDrawInputGlob.matchCount;
    if (!matchCount)
        return;

    conDrawInputGlob.y += conDrawInputGlob.fontHeight;
    conDrawInputGlob.x = conDrawInputGlob.leftX;
    if (matchCount > 24) {
        promptString = va("%i matches (too many to show)", matchCount);
        ConDrawInput_Box(1, con_inputHintBoxColor->current.vector);
        ConDrawInput_Text(promptString, con_inputDvarMatchColor);
        return;
    }

    if (matchCount == 1 || conDrawInputGlob.hasExactMatch && Con_AnySpaceAfterCommand()) {
        Dvar_ForEach(ConDrawInput_DetailedDvarMatch);
        Cmd_ForEach(ConDrawInput_DetailedCmdMatch);
    } else {
        ConDrawInput_Box(matchCount, con_inputHintBoxColor->current.vector);
        Dvar_ForEach(ConDrawInput_DvarMatch);
        Cmd_ForEach(ConDrawInput_CmdMatch);
    }
}

static void Con_DrawStringOnHUD(int x, int y, int charHeight, int horzAlign, int vertAlign, const short int *string, int length, float alpha, float msgwndScale, qboolean centerText)
{
    vec4_t color;
    FontHandle font;
    float fontScale;
    float xScale;
    float yScale;
    float xAdj;
    float yAdj;

    color[0] = 1.0f;
    color[1] = 1.0f;
    color[2] = 1.0f;
    color[3] = alpha;

    fontScale = (float)charHeight / 48.0f;
    if (centerText) {
        font = UI_GetFontHandle(4, fontScale);
        xScale = re.NormalizedTextScale(font, fontScale);
        x += (int)((float)re.ConsoleTextWidth(string, length, font) * xScale * -0.5f);
    } else {
        font = UI_GetFontHandle(0, fontScale);
        xScale = re.NormalizedTextScale(font, fontScale);
    }

    xScale *= msgwndScale;
    yScale = xScale;
    xAdj = (float)x;
    yAdj = (float)(y + (int)((float)re.TextHeight(font) * yScale));

    CalcScreenPlacement(&xAdj, &yAdj, &xScale, &yScale, horzAlign, vertAlign);
    re.DrawConsoleText(string, length, font, xAdj, yAdj, xScale, yScale, color, 3);
}

static void Con_DrawMessageWindowOldToNew(MessageWindow *msgwnd, int x, int y, int charHeight, int horzAlign, int vertAlign, float alpha, float msgwndScale, qboolean centered)
{
    int lineIndex;
    MessageLine *line;
    int elapsed;
    int remaining;
    int lineHeight;
    float drawAlpha;

    lineHeight = charHeight;
    if (centered)
        lineHeight = charHeight + 4;

    for (lineIndex = msgwnd->current_line; lineIndex < msgwnd->current_line + msgwnd->count; lineIndex++) {
        line = &msgwnd->lines[lineIndex % msgwnd->count];
        if (!line->startTime)
            continue;

        if (line->startTime > clients[0].serverTime) {
            line->startTime = 0;
            continue;
        }

        elapsed = clients[0].serverTime - line->startTime;
        if (elapsed < msgwnd->scrolltime)
            y += I_fround((1.0f - (float)elapsed / msgwnd->scrolltime) * lineHeight);
    }

    for (lineIndex = msgwnd->current_line + msgwnd->count - 1; lineIndex >= msgwnd->current_line; lineIndex--) {
        line = &msgwnd->lines[lineIndex % msgwnd->count];
        if (!line->startTime)
            continue;

        if (clients[0].serverTime - line->endTime >= 0) {
            line->startTime = 0;
            continue;
        }

        elapsed = clients[0].serverTime - line->startTime;
        if (elapsed < msgwnd->fadein) {
            drawAlpha = (float)elapsed * alpha / msgwnd->fadein;
        } else {
            ;
            if ((line->endTime - clients[0].serverTime) < msgwnd->fadeout)
                drawAlpha = (float)(line->endTime - clients[0].serverTime) * alpha / msgwnd->fadeout;
            else
                drawAlpha = alpha;
        }

        y -= lineHeight;
        Con_DrawStringOnHUD(x, y, lineHeight, horzAlign, vertAlign, line->textBuffer, 78, drawAlpha, msgwndScale, centered);
    }
}

static void Con_DrawMessageWindow(MessageWindow *msgwnd, int x, int y, int charHeight, int horzAlign, int vertAlign, float alpha, float msgwndScale, msgwnd_mode_t mode)
{
    qboolean up;
    int lineIndex;
    int nextIndex;
    MessageLine *line;
    int scroll;
    float drawAlpha;
    int sameTimeCount;
    int groupY;
    int v;

    groupY = 0;
    if (clients[0].snap.ps.pm_type != 5 && (clients[0].keyCatchers & 8) && !clients[0].displayHUDWithKeycatchUI)
        return;

    switch (mode) {
    case MWM_TOPDOWN:
        up = 0;
        break;
    case MWM_TOPDOWN_REVERSED:
        up = 1;
        break;
    case MWM_BOTTOMUP:
    case MWM_BOTTOMUP_CENTERED:
        Con_DrawMessageWindowOldToNew(msgwnd, x, y, charHeight, horzAlign, vertAlign, alpha, msgwndScale, mode == MWM_BOTTOMUP_CENTERED);
        return;
    default:
        return;
    }

    sameTimeCount = 0;
    v = y;
    if (up)
        v -= charHeight;

    for (lineIndex = msgwnd->current_line; lineIndex < msgwnd->current_line + msgwnd->count; lineIndex++) {
        line = &msgwnd->lines[lineIndex % msgwnd->count];
        if (!line->startTime)
            continue;

        if (line->startTime > clients[0].serverTime) {
            line->startTime = 0;
            continue;
        }

        if (clients[0].serverTime > line->endTime) {
            scroll = msgwnd->scrolltime - (clients[0].serverTime - line->endTime);
            if (scroll > 0) {
                if (up)
                    v -= I_fround((float)scroll / msgwnd->scrolltime * charHeight);
                else
                    v += I_fround((float)scroll / msgwnd->scrolltime * charHeight);
            }
            continue;
        }

        drawAlpha = alpha;
        if (clients[0].serverTime - line->startTime < msgwnd->fadein)
            drawAlpha = (float)(clients[0].serverTime - line->startTime) / msgwnd->fadein * alpha;
        else if (line->endTime - clients[0].serverTime < msgwnd->fadeout)
            drawAlpha = (float)(line->endTime - clients[0].serverTime) / msgwnd->fadeout * alpha;

        if (up) {
            if (!sameTimeCount) {
                for (nextIndex = lineIndex + 1; nextIndex < msgwnd->current_line + msgwnd->count; nextIndex++) {
                    if (msgwnd->lines[nextIndex % msgwnd->count].startTime != line->startTime)
                        break;
                    sameTimeCount++;
                }
                if (sameTimeCount) {
                    v -= sameTimeCount * charHeight;
                    groupY = v;
                    sameTimeCount++;
                }
            } else {
                v += 2 * charHeight;
            }
        }

        Con_DrawStringOnHUD(x, v, charHeight, horzAlign, vertAlign, line->textBuffer, 78, drawAlpha, msgwndScale, 0);

        if (up) {
            if (sameTimeCount) {
                sameTimeCount--;
                if (!sameTimeCount)
                    v = groupY;
            }
            v -= charHeight;
        } else {
            v += charHeight;
        }
    }
}

void Con_DrawSay(int y)
{
    clientActive_t *cl = clients;
    const dvar_t *hudSayPosition;
    const char *string;
    FontHandle font;
    float fontScale;
    float normalizedScale;
    int x;
    int fontHeight;

    if (!(cl->keyCatchers & 0x10))
        return;

    fontScale = 10.0f / 48.0f;
    font = (FontHandle)UI_GetFontHandle(0, fontScale);

    if (*chat_team)
        string = va("%s: ", SEH_SafeTranslateString("EXE_SAYTEAM"));
    else
        string = va("%s: ", SEH_SafeTranslateString("EXE_SAY"));
    normalizedScale = re.NormalizedTextScale(font, fontScale);

    hudSayPosition = cg_hudSayPosition;
    x = (int)hudSayPosition->current.vector[0];

    fontHeight = re.TextHeight(font);
    CL_DrawText(string, 0x7fffffff, font, (float)x,
                (float)y + (float)fontHeight * normalizedScale,
                1, 1, normalizedScale, normalizedScale,
                (const float *)((void *)&colorWhite), 3);

    x += (int)((float)re.TextWidth(string, 0, font) * normalizedScale);
    Field_Draw((field_t *)chatField, x, y, 1, 1);
}

void Con_DrawNotify(int xPos, int yPos, float alpha, msgwnd_mode_t mode)
{
    Con_DrawMessageWindow(&con.messageBuffer->gamemsg, xPos, yPos, 12, 1, 3, alpha, 1.0f, mode);
}

void Con_DrawBoldMessages(int xPos, int yPos, float alpha, msgwnd_mode_t mode)
{
    Con_DrawMessageWindow(&con.messageBuffer->boldgamemsg, xPos, yPos, 12, 7, 7, alpha, 1.0f, mode);
}

void Con_DrawMiniConsole(int xPos, int yPos, float alpha)
{
    con.messageBuffer->minicon.count = con_miniconlines->current.integer;
    Con_DrawMessageWindow(&con.messageBuffer->minicon, xPos, yPos, 12, 1, 1, alpha, 1.0f, MWM_TOPDOWN);
}

void Con_DrawSubtitles(int xPos, int yPos, int charHeight, float alpha, msgwnd_mode_t mode)
{
    Con_DrawMessageWindow(&con.messageBuffer->subtitle, xPos, yPos, charHeight, 1, 1, alpha, 1.0f, mode);
}

void Con_ToggleConsoleOutput(void)
{
    con.outputVisible = (con.outputVisible == 0) ? 1 : 0;
}

static void Con_DrawOutputScrollBar(float x, float y, float width, float height)
{
    float frac;

    x = x + width - 10.0f;
    ConDraw_Box(x, y, 10.0f, height, con_outputBarColor->current.vector);
    frac = 1.0f - (float)(con.currentLine - con.display) / (con.totallines - 1);
    y = (y + height - 10.0f - y) * frac + y;
    ConDraw_Box(x, y, 10.0f, 10.0f, con_outputSliderColor->current.vector);
}

static void Con_DrawOutputText(float x, float y, float height)
{
    vec4_t color;
    int rowCount;
    int rowIndex;
    int lineIndex;

    CL_LookupColor('7', color);
    rowCount = (int)(height / 16.0f);
    lineIndex = con.display;
    if (!con.lineOffset)
        lineIndex--;
    y += height;
    for (rowIndex = 0; rowIndex < rowCount; rowIndex++, lineIndex--) {
        if (lineIndex < 0)
            break;
        if (con.currentLine - lineIndex >= con.totallines)
            continue;
        y -= 16.0f;
        {
            const short *line = &con.textBuffer[(lineIndex % con.totallines) * con.linewidth];
            SCR_DrawConsoleString((int)x, (int)y, line, con.linewidth, color);
        }
    }
}

const char *Con_GetVersionString(void)
{
    return va("Build %s %s", getBuildNumber(), "win-x86");
}

static void Con_DrawOutputVersion(float x, float y, float width, float height)
{
    const char *version;

    version = Con_GetVersionString();
    SCR_DrawSmallStringExt((int)x, (int)(height - 16.0f + y), version, con_versionColor);
}

void Con_DrawOuputWindow(void)
{
    float x;
    float y;
    float width;
    float height;

    x = con.screenMin[0];
    width = con.screenMax[0] - con.screenMin[0];
    y = con.screenMin[1] + 32.0f;
    height = con.screenMax[1] - con.screenMin[1] - 32.0f;
    ConDraw_Box(con.screenMin[0], y, width, height, con_outputWindowColor->current.vector);

    x += 6.0f;
    y += 6.0f;
    width -= 12.0f;
    height -= 12.0f;
    Con_DrawOutputVersion(x, y, width, height);
    Con_DrawOutputScrollBar(x, y, width, height);
    Con_DrawOutputText(x, y, height - 28.0f);
}

void Con_DrawSolidConsole(void)
{
    if (!(clients[0].keyCatchers & 1))
        con.outputVisible = 0;
    if (con.outputVisible)
        Con_DrawOuputWindow();
    Con_DrawInput();
}

void Con_DrawConsole(void)
{
    Con_CheckResize();
    if (clients[0].keyCatchers & 1)
        Con_DrawSolidConsole();
}

void Con_PageUp(void)
{
    con.display -= 2;
    if (con.currentLine - con.display >= con.totallines)
        con.display = con.currentLine - con.totallines + 1;
}

void Con_PageDown(void)
{
    con.display += 2;
    if (con.display > con.currentLine)
        con.display = con.currentLine;
}

void Con_Top(void)
{
    con.display = con.totallines;
    if (con.currentLine - con.totallines >= con.totallines)
        con.display = con.currentLine - con.totallines + 1;
}

void Con_Bottom(void)
{
    con.display = con.currentLine;
}

void Con_Close(void)
{
    int *legacyHacks = (int *)::legacyHacks;
    int active = legacyHacks[1];
    if (!active)
        return;

    Field_Clear((&g_consoleField));
    Con_ClearNotify();
    MsgWnd_Clear(&con.messageBuffer->minicon);
    Con_ClearSubtitles();
    clients[0].keyCatchers &= ~1;
}

Bool Con_IsActive(void)
{
    return clients[0].keyCatchers & 1;
}

static inline void Con_WriteChar(int colorIdx, int ch)
{
    int row = con.currentLine % con.totallines;
    int offset = row * con.linewidth + con.lineOffset;
    con.textBuffer[offset] = (short)((colorIdx << 8) | ch);
    con.lineOffset++;
}
static inline void Con_ClearAllMessageWindows(void)
{
    MsgWnd_Clear(&con.messageBuffer->gamemsg);
    MsgWnd_Clear(&con.messageBuffer->boldgamemsg);
    MsgWnd_Clear(&con.messageBuffer->minicon);
    MsgWnd_Clear(&con.messageBuffer->subtitle);
}
static void Con_ClearMessageWindow(MessageWindow *wnd)
{
    if (!wnd)
        return;
    MsgWnd_Clear(wnd);
}

static const vec4_t con_versionColor = { 1.0f, 1.0f, 0.0f, 1.0f };

static const vec4_t con_inputDvarMatchColor = { 1.0f, 1.0f, 0.8f, 1.0f };

static const vec4_t con_inputCommandMatchColor = { 0.8f, 0.8f, 1.0f, 1.0f };

static const vec4_t con_inputDvarValueColor = { 1.0f, 1.0f, 1.0f, 1.0f };

static const vec4_t con_inputDvarInfoColor = { 0.8f, 0.8f, 1.0f, 1.0f };

static const vec4_t con_inputDvarInactiveValueColor = { 0.8f, 0.8f, 0.8f, 1.0f };
