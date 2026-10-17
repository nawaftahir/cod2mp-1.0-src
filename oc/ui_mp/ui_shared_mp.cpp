#include <ctype.h>
#include "common_types.h"
#include "imports.h"
#include "bytematch.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>
itemDef_t *g_editItem = NULL;
qboolean g_waitingForKey = 0;
extern sharedUiInfo_t sharedUiInfo;

extern commandDef_t commandList[29];
extern bind_t g_bindings[56];

qboolean g_editingField = 0x0;

COD2_ASSERT_FIELD(struct displayContextDef_s, realTime, 0x4);
COD2_ASSERT_FIELD(struct displayContextDef_s, blurRadiusOut, 0x28);
COD2_ASSERT_FIELD(struct displayContextDef_s, Menus, 0x2c);
COD2_ASSERT_FIELD(struct displayContextDef_s, menuCount, 0x22c);
COD2_ASSERT_FIELD(struct displayContextDef_s, menuStack, 0x230);
COD2_ASSERT_FIELD(struct itemDef_s, type, 0x270);
COD2_ASSERT_FIELD(struct itemDef_s, text, 0x294);
COD2_ASSERT_FIELD(struct itemDef_s, parent, 0x29c);
COD2_ASSERT_FIELD(struct itemDef_s, dvar, 0x2c0);
COD2_ASSERT_FIELD(struct itemDef_s, window.foreColor, 0x1cc);
COD2_ASSERT_FIELD(struct itemDef_s, window.backColor, 0x1dc);
COD2_ASSERT_FIELD(struct itemDef_s, window.borderColor, 0x1ec);
COD2_ASSERT_FIELD(struct menuDef_t, items, 0x27c);
COD2_ASSERT_FIELD(struct menuDef_t, onClose, 0x248);
COD2_ASSERT_FIELD(struct menuDef_t, onKey, 0x250);
COD2_ASSERT_FIELD(struct menuDef_t, window.dynamicFlags, 0xe8);
COD2_ASSERT_FIELD(struct editFieldDef_s, maxPaintChars, 0x18);
COD2_ASSERT_FIELD(struct editFieldDef_s, paintOffset, 0x1c);
COD2_ASSERT_FIELD(struct editFieldDef_s, maxChars,         0x10);
COD2_ASSERT_FIELD(struct editFieldDef_s, maxCharsGotoNext, 0x14);
COD2_ASSERT_FIELD(struct itemDef_s, action, 0x2b0);

bool Window_IsVisible(Window *w);
static bool Item_IsTextField(itemDef_t *item);
static qboolean Menu_OverActiveItem(menuDef_t *menu, float x, float y);
static qboolean Menus_MenuIsInStack(displayContextDef_t *dc, menuDef_t *menu);

extern char *va( const char *format, ... );
extern void Cbuf_ExecuteText(int exec_when, const char *text);
extern void Cbuf_AddText(const char *text);
extern int I_stricmp(const char *s0, const char *s1);
extern int I_strncmp(const char *s0, const char *s1, int n);
extern void I_strncat(char *dest, int destsize, const char *src);
extern void Com_Error(errorParm_t code, const char *fmt, ...);
extern void Com_Printf(const char *fmt, ...);
extern char *Com_ParseOnLine( const char *( *data_p ) );
extern const char *SEH_StringEd_GetString(const char *key);
extern bool Dvar_GetBool(const char *dvarName);
extern int Dvar_GetInt(const char *name);
extern float Dvar_GetFloat(const char *name);
extern const char* Dvar_GetString(const char *dvarName);
extern void Dvar_SetFromStringByName(const char *dvarName, const char *string);
extern dvar_t* Dvar_FindVar(const char* dvarName);
void Com_LocalizedFloatToString(float f, char *buffer, unsigned int maxlen, unsigned int numDecimalPlaces);
extern int UI_PlayLocalSoundAlias(snd_alias_list_t *aliasList);
extern int UI_PlayLocalSoundAliasByName(const char *name);
extern void UI_RunMenuScript(const char **args);
extern qboolean UI_ClientIsInGame(void);
extern void UI_Pause(qboolean pause);
extern void UI_DrawHandlePic(float x, float y, float w, float h, int horzAlign, int vertAlign, const vec_t *color, MaterialHandle hMaterial);
extern void UI_DrawRect(float x, float y, float w, float h, int horzAlign, int vertAlign, float size, const vec_t *color);
extern void UI_FillRect(float x, float y, float width, float height, int horzAlign, int vertAlign, const vec_t *color);
extern void UI_DrawSides(float x, float y, float w, float h, int horzAlign, int vertAlign, float size, const vec_t *color);
extern void UI_DrawTopBottom(float x, float y, float w, float h, int horzAlign, int vertAlign, float size, const vec_t *color);
extern void UI_DrawLoadBar(float x, float y, float w, float h, int horzAlign, int vertAlign, const vec_t *color, MaterialHandle material);
extern int UI_TextWidth(const char *text, int maxChars, FontHandle font, float scale);
extern int UI_TextHeight(FontHandle font, float scale);
extern const char *UI_SafeTranslateString(const char *key);
extern int UI_OwnerDrawWidth(int ownerDraw, FontHandle font, float scale);
extern qboolean UI_OwnerDrawVisible(int flags);
extern void UI_OwnerDraw(float x, float y, float w, float h, int horzAlign, int vertAlign, float text_x, float text_y, int ownerDraw, int ownerDrawFlags, int align, float special, FontHandle font, float scale, vec_t *color, MaterialHandle shader, int textStyle);
extern qboolean UI_OwnerDrawHandleKey(int ownerDraw, int flags, float *special, int key);
extern int UI_FeederCount(float feederID);
extern MaterialHandle UI_FeederItemImage(const float feederID, int index);
extern const char *UI_FeederItemText(float feederID, int index, int column, MaterialHandle *handle);
extern void UI_FeederSelection(float feederID, int index);
extern void UI_OverrideCursorPos(itemDef_t *item);
extern qboolean UI_OwnerDrawVisible(int flags);
extern void CalcScreenPlacement(float *x, float *y, float *w, float *h, int horzAlign, int vertAlign);
extern void CalcScreenX(float *x, int horzAlign);
extern void CalcScreenY(float *y, int vertAlign);
extern qboolean Float_Parse(const char **p, float *f);
extern qboolean Int_Parse(const char **p, int *i);
extern qboolean Rect_Parse(const char **p, rectDef_t *r);
extern MaterialHandle CL_RegisterMaterialNoMip(const char *name, int imageTrack);
extern const char *CL_GetConfigString(int index);
extern void CIN_DrawCinematic(int handle);
extern int CIN_PlayCinematic(const char *name, int x, int y, int w, int h, int flags);
extern e_status CIN_RunCinematic(int handle);
extern void CIN_SetExtents(int handle, int x, int y, int w, int h);
extern e_status CIN_StopCinematic(int handle);
extern void Key_GetBindingBuf(int keynum, char *buf, int buflen);
extern void Key_KeynumToStringBuf(int keynum, char *buf, int buflen);
extern void Key_SetBinding(int keynum, const char *binding);
extern void Key_SetOverstrikeMode(qboolean state);
extern int Key_GetCatcher(void);
extern void Key_SetCatcher(int catcher);
extern qboolean Item_EnableShowViaDvar(const itemDef_t *item, int flag);
extern int Item_GetCursorPosOffset(const itemDef_t *item, const char *buff, int direction);
extern struct listBoxDef_s *Item_GetListBoxDef(itemDef_t *item);
extern struct multiDef_s *Item_GetMultiDef(itemDef_t *item);
extern void Item_SetCursorPos(itemDef_t *item, int cursorPos);
extern void Item_SetScreenCoords(itemDef_t *item, float x, float y, int horzAlign, int vertAlign);

static inline void FastSinCos(const float value, float *pSin, float *pCos)
{
    __asm
    {
        push eax
        push ecx
        fld value
        fsincos
        mov ecx, pCos
        mov eax, pSin
        fstp dword ptr [ecx]
        fstp dword ptr [eax]
        pop ecx
        pop eax
    }
}

extern void Item_SetTextRect(itemDef_t *item, const rectDef_t *textRect);
extern void Item_SetupKeywordHash(void);
extern Bool ListBox_HasValidCursorPos(const listBoxDef_t *listBox);
extern void ListBox_SetCursorPos(listBoxDef_t *listBox, int cursorPos);
extern void ListBox_SetEndPos(listBoxDef_t *listBox, int endPos);
extern void ListBox_SetStartPos(listBoxDef_t *listBox, int startPos);
extern void Menu_SetCursorItem(menuDef_t *menu, int cursorItem);
extern void Menu_SetupKeywordHash(void);
extern void Menu_UpdatePosition(menuDef_t *menu);
extern void Window_AddDynamicFlags(Window *w, const int newFlags);
extern void Window_RemoveDynamicFlags(Window *w, const int newFlags);
extern void Window_SetDynamicFlags(Window *w, const int flags);
extern void Window_SetOffsetTime(Window *w, int offsetTime);
extern void Window_SetRect(Window *w, const UiRectangle *rect);
extern void Window_SetRectClient(Window *w, const UiRectangle *rectClient);
extern void Window_SetRectEffects0(Window *item, const rectDef_t *rect);
extern void Window_SetRectEffects1(Window *item, const rectDef_t *rect);
extern bool I_isdigit( int c );
extern bool I_isforfilename( int c );
extern int Sys_Milliseconds(void);
extern char Com_GetDecimalDelimiter(void);
extern unsigned char updateScreenCalled[];
extern qboolean g_waitingForKey;
extern qboolean g_editingField;
extern itemDef_t *g_editItem;
extern int scriptCommandCount;
void CL_GetAutoUpdate(void);
static void (*captureFunc)();
void LerpColor(vec_t *a, vec_t *b, vec_t *c, float t);
qboolean String_Parse(const char **p, char *out, int len);
int Menu_ItemsMatchingGroup(menuDef_t *menu, const char *name);
itemDef_t *Menu_GetMatchingItemByNumber(menuDef_t *menu, int index, const char *name);
void Script_SetColor(displayContextDef_t *dc, itemDef_t *item, const char **args);
void Script_SetBackground(displayContextDef_t *dc, itemDef_t *item, const char **args);
itemDef_t *Menu_FindItemByName(menuDef_t *menu, const char *p);
void Menu_ShowItemByName(menuDef_t *menu, const char *p, qboolean bShow);
void Menu_FadeItemByName(menuDef_t *menu, const char *p, qboolean fadeOut);
menuDef_t *Menus_FindByName(displayContextDef_t *dc, const char *p);
void Script_Show(displayContextDef_t *dc, itemDef_t *item, const char **args);
void Script_Hide(displayContextDef_t *dc, itemDef_t *item, const char **args);
void Script_FadeIn(displayContextDef_t *dc, itemDef_t *item, const char **args);
void Script_FadeOut(displayContextDef_t *dc, itemDef_t *item, const char **args);
void Script_SetDvar(displayContextDef_t *dc, itemDef_t *item, const char **args);
void Script_ExecNow(displayContextDef_t *dc, itemDef_t *item, const char **args);
static void Script_ConditionalExecHandler(int execWhen, Bool (*shouldExec)(const char *, const char *), const char **args);
static void Script_ConditionalExecHandler_impl(int execWhen, itemDef_t *item, const char **args, Bool (*shouldExec)(const char *, const char *));
static Bool Script_ExecIfStringsEqual(const char *dvarValue, const char *testValue);
static Bool Script_ExecIfIntsEqual(const char *dvarValue, const char *testValue);
static Bool Script_ExecIfFloatsEqual(const char *dvarValue, const char *testValue);
void Script_ExecOnDvarStringValue(displayContextDef_t *dc, itemDef_t *item, const char **args);
void Script_ExecOnDvarIntValue(displayContextDef_t *dc, itemDef_t *item, const char **args);
void Script_ExecOnDvarFloatValue(displayContextDef_t *dc, itemDef_t *item, const char **args);
void Script_ExecNowOnDvarStringValue(displayContextDef_t *dc, itemDef_t *item, const char **args);
void Script_ExecNowOnDvarIntValue(displayContextDef_t *dc, itemDef_t *item, const char **args);
void Script_ExecNowOnDvarFloatValue(displayContextDef_t *dc, itemDef_t *item, const char **args);
void Script_Play(displayContextDef_t *dc, itemDef_t *item, const char **args);
void Script_ScriptMenuResponse(displayContextDef_t *dc, itemDef_t *item, const char **args);
qboolean Display_MouseMove(displayContextDef_t *dc, void *p, int x, int y);
void Item_RunScript(displayContextDef_t *dc, itemDef_t *item, const char *s);
void Controls_GetConfig(void);
void Controls_SetConfig(qboolean restart);
void Controls_SetDefaults(void);
void BindingFromName(const char *dvar, char *nameBind);
qboolean GetCommandHasBinding(const char *command);
int GetKeyBindings(const char *command, char (*bindings)[128]);
int GetKeyBindingLocalizedString(const char *command, char *keys);
qboolean Display_KeyBindPending(void);
int Menu_Count(displayContextDef_t *dc);
void UI_AddMenuList(displayContextDef_t *dc, MenuList *menuList);
qboolean Item_SetFocus(displayContextDef_t *dc, itemDef_t *item, float x, float y);
void Item_MouseLeave(displayContextDef_t *dc, itemDef_t *item);
float Item_Slider_ThumbPosition(itemDef_t *item);
bool Window_IsVisible(Window *w);
qboolean Menu_CheckOnKey(displayContextDef_t *dc, menuDef_t *menu, int key);
qboolean Menus_AnyFullScreenVisible(displayContextDef_t *dc);
menuDef_t *Menu_GetFocused(displayContextDef_t *dc);
int Item_ListBox_MaxScroll(itemDef_t *item);
int Item_ListBox_ThumbPosition(itemDef_t *item);
int Item_ListBox_ThumbDrawPosition(displayContextDef_t *dc, itemDef_t *item);
static void Scroll_Slider_SetThumbPos(displayContextDef_t *dc, itemDef_t *item);
static void Scroll_Slider_ThumbFunc(displayContextDef_t *dc, void *p);
void Menus_Close(displayContextDef_t *dc, menuDef_t *menu);
void Menus_CloseAll(displayContextDef_t *dc);
void Item_SetTextExtents(itemDef_t *item, int *width, int *height, const char *text);
void Item_Text_AutoWrapped_Paint(itemDef_t *item, const char *textPtr, vec_t *color);
void Item_Text_Wrapped_Paint(itemDef_t *item, const char *textPtr, vec_t *color);
void Item_ListBox_Paint(displayContextDef_t *dc, itemDef_t *item);
editFieldDef_t *Item_GetEditFieldDef(itemDef_t *item);
const char *Dvar_GetVariantString(const char *dvarName);
void I_strncpyz(char *dest, const char *src, int destsize);
int Key_GetOverstrikeMode(void);
FontHandle UI_GetFontHandle(int fontEnum, float scale);
void UI_DrawText(const char *text, int maxChars, FontHandle font, float x, float y, int horzAlign, int vertAlign, float scale, const vec_t *color, int style);
void UI_DrawTextWithCursor(const char *text, int maxChars, FontHandle font, float x, float y, int horzAlign, int vertAlign, float scale, const vec_t *color, int style, int cursorPos, char cursor);
void Script_SetItemColor(displayContextDef_t *dc, itemDef_t *item, const char **args);
void Window_Paint(displayContextDef_t *dc, float (*w)[4][32], float fadeAmount, float fadeInAmount, float fadeClamp, float fadeCycle);
void Menu_Setup(displayContextDef_t *dc);
void Menus_CloseByName(displayContextDef_t *dc, const char *p);
void Menu_SetFeederSelection(displayContextDef_t *dc, menuDef_t *menu, int feeder, int index, const char *name);
void Script_Exec(displayContextDef_t *dc, itemDef_t *item, const char **args);
void Menu_TransitionItemByName(menuDef_t *menu, const char *p, rectDef_t rectFrom, rectDef_t rectTo, int time, float amt);
void Script_Transition(displayContextDef_t *dc, itemDef_t *item, const char **args);
void Menu_OrbitItemByName(menuDef_t *menu, const char *p, float x, float y, float cx, float cy, int time);
void Script_Orbit(displayContextDef_t *dc, itemDef_t *item, const char **args);
void Script_CloseForGameType(displayContextDef_t *dc, itemDef_t *item, const char **args);
void Script_Close(displayContextDef_t *dc, itemDef_t *item, const char **args);
void Script_InGameClose(displayContextDef_t *dc, itemDef_t *item, const char **args);
void Item_TextColor(displayContextDef_t *dc, itemDef_t *item, vec4_t *newColor);
void Item_Text_Paint(displayContextDef_t *dc, itemDef_t *item);
void Item_TextField_Paint(displayContextDef_t *dc, itemDef_t *item);
void Item_YesNo_Paint(displayContextDef_t *dc, itemDef_t *item);
void Item_Slider_Paint(displayContextDef_t *dc, itemDef_t *item);
void Item_Bind_Paint(displayContextDef_t *dc, itemDef_t *item);
void Item_OwnerDraw_Paint(displayContextDef_t *dc, itemDef_t *item);
static void Item_List_Paint(displayContextDef_t *dc, itemDef_t *item, const char *text);
qboolean Item_Slider_HandleKey(displayContextDef_t *dc, itemDef_t *item, int key, qboolean down);
qboolean Item_YesNo_HandleKey(displayContextDef_t *dc, itemDef_t *item, int key);
itemDef_t *Menu_ClearFocus(displayContextDef_t *dc, menuDef_t *menu);
void Script_SetFocusByDvar(displayContextDef_t *dc, itemDef_t *item, const char **args);
void Script_SetFocus(displayContextDef_t *dc, itemDef_t *item, const char **args);
static bool Item_IsTextField(itemDef_t *item);
static bool Item_ShouldHandleKey(displayContextDef_t *dc, itemDef_t *item, int key);
static int Item_Enum_GetIndex(itemDef_t *item);
int Item_ListBox_OverLB(itemDef_t *item, float x, float y);
static qboolean Rect_ContainsPoint(const rectDef_t *rect, float x, float y);
void Item_ListBox_MouseEnter(itemDef_t *item, float x, float y);
void Item_MouseEnter(displayContextDef_t *dc, itemDef_t *item, float x, float y);
static __declspec(noinline) qboolean Item_ListBox_HandleKey(displayContextDef_t *dc, itemDef_t *item, int key, qboolean down, qboolean force);
static void Scroll_ListBox_ThumbFunc(displayContextDef_t *dc, void *p);
static void Scroll_ListBox_AutoFunc(displayContextDef_t *dc, void *p);
qboolean Item_Bind_HandleKey(displayContextDef_t *dc, itemDef_t *item, int key, qboolean down);
qboolean Item_HandleKey(displayContextDef_t *dc, itemDef_t *item, int key, qboolean down);
qboolean Menu_HandleMouseMove(displayContextDef_t *dc, menuDef_t *menu, float x, float y);
void Menus_Open(displayContextDef_t *dc, menuDef_t *menu);
qboolean Menus_OpenByName(displayContextDef_t *dc, const char *p);
void Script_Open(displayContextDef_t *dc, itemDef_t *item, const char **args);
void Script_OpenForGameType(displayContextDef_t *dc, itemDef_t *item, const char **args);
void Script_InGameOpen(displayContextDef_t *dc, itemDef_t *item, const char **args);
itemDef_t *Menu_SetNextCursorItem(displayContextDef_t *dc, menuDef_t *menu);
itemDef_t *Menu_SetPrevCursorItem(displayContextDef_t *dc, menuDef_t *menu);
qboolean Item_TextField_HandleKey(displayContextDef_t *dc, itemDef_t *item, int key);
void Menu_HandleKey(displayContextDef_t *dc, menuDef_t *menu, int key, qboolean down);
void Item_Paint(displayContextDef_t *dc, itemDef_t *item);
void Menu_Paint(displayContextDef_t *dc, menuDef_t *menu, qboolean forcePaint);
void Menu_PaintAll(displayContextDef_t *dc);
void Menus_HandleOOBClick(displayContextDef_t *dc, menuDef_t *menu, int key, qboolean down);
extern void Dvar_SetFromStringByName(const char *name, const char *val);
extern void Cbuf_ExecuteText(int execWhen, const char *text);
extern void Key_SetBinding(int key, const char *command);
static bool Item_IsTextField(itemDef_t *item);
extern const char *Dvar_GetVariantString(const char *dvarName);
extern int inHandleKey;

// unreferenced field getters, kept by /OPT:NOREF
int Window_Getter_533860(Window *w) { return w->offsetTime[0]; }
UiRectangle *Window_Getter_533880(Window *w) { return &w->rectEffects0[0]; }
UiRectangle *Window_Getter_533890(Window *w) { return &w->rectEffects1[0]; }
int Window_Getter_5338a0(Window *w) { return w->staticFlags; }
int Menu_Getter_5338b0(menuDef_t *menu) { return menu->cursorItem[0]; }
UiRectangle *Item_Getter_5338c0(itemDef_t *item) { return &item->textRect[0]; }
static bool Window_IsHorizontal(Window *w)
{
    return ((unsigned int)w->staticFlags >> 21) & 1;
}

static rectDef_t rect;
static qboolean inHandleKey;
static scrollInfo_t scrollInfo;
static void *captureData;
static itemDef_t *itemCapture;
static itemDef_t *g_bindItem;
static qboolean debugMode;
static int lastListBoxClickTime;

static float I_fsel(const float x, const float y, const float z)
{
    return x >= 0.0f ? y : z;
}

static float I_fclamp(const float val, const float min, const float max)
{
    return I_fsel(min - val, min, I_fsel(val - max, max, val));
}

static void Item_TextField_BeginEdit(itemDef_t *item);

static bool Window_HasFocus(Window *w)
{
    if (!Window_IsVisible(w))
        return false;
    return ((unsigned int)w->dynamicFlags[0] >> 1) & 1;
}

void Menu_Setup(displayContextDef_t *dc)
{
    dc->menuCount = 0;
    dc->openMenuCount = 0;
    Item_SetupKeywordHash();
    Menu_SetupKeywordHash();
    Controls_GetConfig();
}

void LerpColor(vec_t *a, vec_t *b, vec_t *c, float t)
{
    int i;

    for (i = 0; i < 4; i++) {
        c[i] = (b[i] - a[i]) * t + a[i];
        if (c[i] < 0.0f)
            c[i] = 0.0f;
        else if (c[i] > 1.0)
            c[i] = 1.0f;
    }
}

static qboolean Color_Parse(const char **p, float (*c)[4])
{
    float f;
    int i;

    for (i = 0; i < 4; i++) {
        if (!Float_Parse(p, &f))
            return 0;
        (*c)[i] = f;
    }
    return 1;
}

qboolean String_Parse(const char **p, char *out, int len)
{
    const char *token;
    const char *translated;

    token = Com_ParseOnLine(p);
    if (*p) {
        if (token[0] == '@') {
            translated = SEH_StringEd_GetString(token + 1);
            if (translated) {
                I_strncpyz(out, translated, len);
                return 1;
            }
            if (Dvar_GetBool("loc_warnings")) {
                if (Dvar_GetBool("loc_warningsAsErrors"))
                    Com_Error(ERR_LOCALIZATION, "Could not translate menu string reference %s", token);
                else
                    Com_Printf("^3WARNING: Could not translate menu string reference %s\n", token);
            }
        }
        I_strncpyz(out, token, len);
        return 1;
    }
    return 0;
}

// retail 0x533b60
void Fade(int *flags, float *f, float clamp, int *nextTime, int offsetTime, qboolean bFlags, float fadeAmount, float fadeInAmount, displayContextDef_t *dc)
{
    if (*flags & 0x30) {
        if (dc->realTime > *nextTime) {
            *nextTime = dc->realTime + offsetTime;
            if (*flags & 0x10) {
                *f -= fadeAmount;
                if (bFlags && *f <= 0.0)
                    *flags &= ~0x14;
            } else {
                *f += fadeInAmount;
                if (*f >= clamp) {
                    *f = clamp;
                    if (bFlags)
                        *flags &= ~0x20;
                }
            }
        }
    }
}

// retail 0x5340d0
void Item_UpdatePosition(itemDef_t *item)
{
    menuDef_t *menu;
    float x, y;

    if (item == NULL || item->parent == NULL)
        return;

    menu = (menuDef_t *)item->parent;
    x = menu->window.rect[0].x;
    y = menu->window.rect[0].y;

    if (menu->window.border != 0) {
        x += menu->window.borderSize;
        y += menu->window.borderSize;
    }

    Item_SetScreenCoords(item, x, y, menu->window.rect[0].horzAlign, menu->window.rect[0].vertAlign);
}
// retail 0x534130
itemDef_t *Menu_ClearFocus(displayContextDef_t *dc, menuDef_t *menu)
{
    itemDef_t *ret;
    int i;

    ret = NULL;
    if (!menu)
        return NULL;
    for (i = 0; i < menu->itemCount; i++) {
        if (Window_HasFocus(&menu->items[i]->window))
            ret = menu->items[i];
        Window_RemoveDynamicFlags(&menu->items[i]->window, 2);
        if (menu->items[i]->leaveFocus)
            Item_RunScript(dc, menu->items[i], menu->items[i]->leaveFocus);
    }
    return ret;
}

// retail 0x5341d0
qboolean IsVisible(int flags)
{
    return (flags & 4) && !(flags & 0x10);
}

static qboolean Rect_ContainsPoint(const rectDef_t *rect, float x, float y)
{
    rectDef_t r;

    r.x = rect->x;
    r.y = rect->y;
    r.w = rect->w;
    r.h = rect->h;
    CalcScreenX(&x, 4);
    CalcScreenY(&y, 4);
    CalcScreenPlacement(&r.x, &r.y, &r.w, &r.h, rect->horzAlign, rect->vertAlign);
    return x >= r.x && x <= r.x + r.w && y >= r.y && y <= r.y + r.h;
}

int Menu_ItemsMatchingGroup(menuDef_t *menu, const char *name)
{
    const char *p;
    int wildcard;
    int count;
    int i;

    count = 0;
    wildcard = -1;
    p = strstr(name, "*");
    if (p)
        wildcard = p - name;

    for (i = 0; i < menu->itemCount; i++) {
        if (wildcard != -1) {
            if ((menu->items[i]->window.name && !I_strncmp(menu->items[i]->window.name, name, wildcard))
                || (menu->items[i]->window.group && !I_strncmp(menu->items[i]->window.group, name, wildcard)))
                count++;
        } else {
            if ((menu->items[i]->window.name && !I_stricmp(menu->items[i]->window.name, name))
                || (menu->items[i]->window.group && !I_stricmp(menu->items[i]->window.group, name)))
                count++;
        }
    }
    return count;
}

itemDef_t *Menu_GetMatchingItemByNumber(menuDef_t *menu, int index, const char *name)
{
    const char *p;
    int wildcard;
    int count;
    int i;

    count = 0;
    wildcard = -1;
    p = strstr(name, "*");
    if (p)
        wildcard = p - name;

    for (i = 0; i < menu->itemCount; i++) {
        if (wildcard != -1) {
            if ((menu->items[i]->window.name && !I_strncmp(menu->items[i]->window.name, name, wildcard))
                || (menu->items[i]->window.group && !I_strncmp(menu->items[i]->window.group, name, wildcard))) {
                if (count == index)
                    return menu->items[i];
                count++;
            }
        } else {
            if ((menu->items[i]->window.name && !I_stricmp(menu->items[i]->window.name, name))
                || (menu->items[i]->window.group && !I_stricmp(menu->items[i]->window.group, name))) {
                if (count == index)
                    return menu->items[i];
                count++;
            }
        }
    }
    return NULL;
}

void Script_SetColor(displayContextDef_t *dc, itemDef_t *item, const char **args)
{
    char name[0x400];
    float *out = NULL;
    int i;
    (void)dc;

    if (!String_Parse(args, name, 0x400))
        return;

    if (I_stricmp(name, "backcolor") == 0) {
        out = item->window.backColor;
        Window_AddDynamicFlags( (Window *)((itemDef_t *)((void *)item)), 0x8000);
    } else if (I_stricmp(name, "forecolor") == 0) {
        out = item->window.foreColor;
        Window_AddDynamicFlags( (Window *)((itemDef_t *)((void *)item)), 0x10000);
    } else if (I_stricmp(name, "bordercolor") == 0) {
        out = item->window.borderColor;
    }

    if (!out)
        return;

    for (i = 0; i < 4; i++) {
        float f;
        if (!Float_Parse(args, &f))
            return;
        out[i] = f;
    }
}

void Script_SetBackground(displayContextDef_t *dc, itemDef_t *item, const char **args)
{
    (void)dc;
    char name[0x400];
    if (String_Parse(args, name, 0x400)) {
        byte *it = (byte *)item;
        (*(MaterialHandle *)&((itemDef_t *)it)->window.background) = CL_RegisterMaterialNoMip(name, item->imageTrack);
    }
}

itemDef_t *Menu_FindItemByName(menuDef_t *menu, const char *p)
{
    int i;

    if (!menu || !p)
        return NULL;

    for (i = 0; i < menu->itemCount; i++) {
        if (menu->items[i]->window.name && !I_stricmp(p, menu->items[i]->window.name))
            return menu->items[i];
    }
    return NULL;
}

void Script_SetItemColor(displayContextDef_t *dc, itemDef_t *item, const char **args)
{
    char itemname[1024];
    char name[1024];
    float color[4];
    itemDef_t *item2;
    float *out;
    int count, i, j;
    (void)dc;

    if (String_Parse(args, itemname, 1024)) {
        if (String_Parse(args, name, 1024)) {
            count = Menu_ItemsMatchingGroup(item->parent, itemname);
            if (Color_Parse(args, &color)) {
                for (j = 0; j < count; j++) {
                    item2 = (itemDef_t *)Menu_GetMatchingItemByNumber(item->parent, j, itemname);
                    if (item2) {
                        out = NULL;
                        if (!I_stricmp(name, "backcolor")) {
                            out = (float *)((byte *)item2 + 0x1dc);
                        } else if (!I_stricmp(name, "forecolor")) {
                            out = (float *)((byte *)item2 + 0x1cc);
                            Window_AddDynamicFlags((Window *)item2, 0x10000);
                        } else if (!I_stricmp(name, "bordercolor")) {
                            out = (float *)((byte *)item2 + 0x1ec);
                        }
                        if (out) {
                            for (i = 0; i < 4; i++)
                                out[i] = color[i];
                        }
                    }
                }
            }
        }
    }
}

void Menu_ShowItemByName(menuDef_t *menu, const char *p, qboolean bShow)
{
    int count = Menu_ItemsMatchingGroup(menu, p);
    int i;
    for (i = 0; i < count; i++) {
        byte *item = (byte *)Menu_GetMatchingItemByNumber(menu, i, p);
        if (!item)
            continue;
        if (bShow) {
            Window_AddDynamicFlags( (Window *)((itemDef_t *)((void *)item)), 4);
        } else {
            Window_RemoveDynamicFlags( (Window *)((itemDef_t *)((void *)item)), 4);
            int cinHandle = ((itemDef_t *)item)->window.cinematic;
            if (cinHandle >= 0) {
                CIN_StopCinematic(cinHandle);
                ((itemDef_t *)item)->window.cinematic = -1;
            }
        }
    }
}

void Menu_FadeItemByName(menuDef_t *menu, const char *p, qboolean fadeOut)
{
    int count = Menu_ItemsMatchingGroup(menu, p);
    int i;
    for (i = 0; i < count; i++) {
        byte *item = (byte *)Menu_GetMatchingItemByNumber(menu, i, p);
        if (!item)
            continue;
        if (fadeOut) {
            Window_AddDynamicFlags( (Window *)((itemDef_t *)((void *)item)), 0x14);
            Window_RemoveDynamicFlags( (Window *)((itemDef_t *)((void *)item)), 0x20);
        } else {
            Window_AddDynamicFlags( (Window *)((itemDef_t *)((void *)item)), 0x24);
            Window_RemoveDynamicFlags( (Window *)((itemDef_t *)((void *)item)), 0x10);
        }
    }
}

int Menus_RemoveFromStack(displayContextDef_t *dc, menuDef_t *menu)
{
    int i;

    for (i = dc->openMenuCount - 1; ; i--) {
        if (i < 0)
            return 0;
        if (dc->menuStack[i] == menu)
            break;
    }

    dc->openMenuCount--;
    for (; i < dc->openMenuCount; i++)
        dc->menuStack[i] = dc->menuStack[i + 1];
    return 1;
}

void Menus_AddToStack(displayContextDef_t *dc, menuDef_t *menu)
{
    Menus_RemoveFromStack(dc, menu);
    if (dc->openMenuCount == 16)
        Com_Error(ERR_DROP, "\x15Too many menus opened");
    dc->menuStack[dc->openMenuCount++] = menu;
}

menuDef_t *Menus_FindByName(displayContextDef_t *dc, const char *p)
{
    int i;
    for (i = 0; i < dc->menuCount; i++) {
        if (I_stricmp(dc->Menus[i]->window.name, p) == 0)
            return dc->Menus[i];
    }
    return NULL;
}

static void Menu_RunCloseScript(displayContextDef_t *dc, menuDef_t *menu)
{
    itemDef_t item;

    if (Window_IsVisible(&menu->window) && menu->onClose) {
        item.parent = menu;
        Item_RunScript(dc, &item, menu->onClose);
    }
}

void Menus_Close(displayContextDef_t *dc, menuDef_t *menu)
{
    bool hadFocus;
    int i;

    if (Menus_MenuIsInStack(dc, menu)) {
        hadFocus = Window_HasFocus(&menu->window);
        Menu_RunCloseScript(dc, menu);

        Menus_RemoveFromStack(dc, menu);

        if (hadFocus && dc->openMenuCount) {
            for (i = dc->openMenuCount - 1; i >= 0; i--) {
                if (Window_IsVisible(&dc->menuStack[i]->window)) {
                    Window_AddDynamicFlags(&dc->menuStack[i]->window, 2);
                    break;
                }
            }
        }
    }

    Window_RemoveDynamicFlags(&menu->window, 6);
}

void Menus_CloseByName(displayContextDef_t *dc, const char *p)
{
    menuDef_t *menu = Menus_FindByName(dc, p);
    if (menu)
        Menus_Close(dc, menu);
}

void Menus_CloseAll(displayContextDef_t *dc)
{
    int i;

    for (i = 0; i < dc->menuCount; i++)
        Menus_Close(dc, dc->Menus[i]);
}

void Script_Open(displayContextDef_t *dc, itemDef_t *item, const char **args)
{
    char name[0x400];
    (void)item;
    if (!String_Parse(args, name, 0x400))
        return;
    Menus_OpenByName(dc, name);
}

void Script_OpenForGameType(displayContextDef_t *dc, itemDef_t *item, const char **args)
{
    char name[0x400];
    if (!String_Parse(args, name, 0x400))
        return;
    const char *p = va(name, Dvar_GetString(item->dvar));
    Menus_OpenByName(dc, p);
}

void Script_CloseForGameType(displayContextDef_t *dc, itemDef_t *item, const char **args)
{
    char name[0x400];
    (void)item;
    if (!String_Parse(args, name, 0x400))
        return;
    const char *p = va(name, Dvar_GetString(item->dvar));
    Menus_CloseByName(dc, p);
}

void Script_Close(displayContextDef_t *dc, itemDef_t *item, const char **args)
{
    char name[0x400];
    (void)item;
    if (!String_Parse(args, name, 0x400))
        return;
    Menus_CloseByName(dc, name);
}

void Script_InGameOpen(displayContextDef_t *dc, itemDef_t *item, const char **args)
{
    char name[0x400];
    (void)item;
    if (!String_Parse(args, name, 0x400))
        return;
    if (!UI_ClientIsInGame())
        return;
    Menus_OpenByName(dc, name);
}

void Script_InGameClose(displayContextDef_t *dc, itemDef_t *item, const char **args)
{
    char name[0x400];
    (void)item;
    if (!String_Parse(args, name, 0x400))
        return;
    if (!UI_ClientIsInGame())
        return;
    Menus_CloseByName(dc, name);
}

void Menu_TransitionItemByName(menuDef_t *menu, const char *p, rectDef_t rectFrom, rectDef_t rectTo, int time, float amt)
{
    int count = Menu_ItemsMatchingGroup(menu, p);
    int i;

    for (i = 0; i < count; i++) {
        byte *item = (byte *)Menu_GetMatchingItemByNumber(menu, i, p);
        if (!item)
            continue;

        Window_AddDynamicFlags( (Window *)((itemDef_t *)((void *)item)), 0x84);
        Window_SetOffsetTime( (Window *)((itemDef_t *)item), time);
        Window_SetRectClient( (Window *)(item), &rectFrom);
        Window_SetRectEffects0((Window *)item, &rectTo);

        rectDef_t newRect = (*(rectDef_t *)&((itemDef_t *)item)->window.rectEffects1[0].x);
        newRect.x = (float)abs((int)(rectTo.x - rectFrom.x)) / amt;
        newRect.y = (float)abs((int)(rectTo.y - rectFrom.y)) / amt;
        newRect.w = (float)abs((int)(rectTo.w - rectFrom.w)) / amt;
        newRect.h = (float)abs((int)(rectTo.h - rectFrom.h)) / amt;

        Window_SetRectEffects1((Window *)item, &newRect);

        Item_UpdatePosition((itemDef_t *)item);
    }
}

void Script_Transition(displayContextDef_t *dc, itemDef_t *item, const char **args)
{
    char name[0x400];
    rectDef_t rectFrom, rectTo;
    int time;
    float amt;
    (void)dc;

    if (!String_Parse(args, name, 0x400))
        return;
    if (!Rect_Parse(args, &rectFrom))
        return;
    if (!Rect_Parse(args, &rectTo))
        return;
    if (!Int_Parse(args, &time))
        return;
    if (!Float_Parse(args, &amt))
        return;

    Menu_TransitionItemByName(item->parent, name, rectFrom, rectTo, time, amt);
}

void Menu_OrbitItemByName(menuDef_t *menu, const char *p, float x, float y, float cx, float cy, int time)
{
    int count = Menu_ItemsMatchingGroup(menu, p);
    int i;

    for (i = 0; i < count; i++) {
        byte *item = (byte *)Menu_GetMatchingItemByNumber(menu, i, p);
        if (!item)
            continue;

        Window_AddDynamicFlags( (Window *)((itemDef_t *)((void *)item)), 0x2004);
        Window_SetOffsetTime( (Window *)((itemDef_t *)item), time);

        rectDef_t newRect;
        newRect = (*(rectDef_t *)&((itemDef_t *)item)->window.rectEffects0[0].x);
        newRect.x = cx;
        newRect.y = cy;

        Window_SetRectEffects0((Window *)item, &newRect);

        newRect = (*(rectDef_t *)&((itemDef_t *)item)->window.rectClient[0].x);
        newRect.x = x;
        newRect.y = y;
        Window_SetRectClient( (Window *)(item), &newRect);

        Item_UpdatePosition((itemDef_t *)item);
    }
}

void Script_Orbit(displayContextDef_t *dc, itemDef_t *item, const char **args)
{
    char name[0x400];
    float cx, cy, x, y;
    int time;
    (void)dc;

    if (!String_Parse(args, name, 0x400))
        return;
    if (!Float_Parse(args, &x))
        return;
    if (!Float_Parse(args, &y))
        return;
    if (!Float_Parse(args, &cx))
        return;
    if (!Float_Parse(args, &cy))
        return;
    if (!Int_Parse(args, &time))
        return;

    Menu_OrbitItemByName(item->parent, name, x, y, cx, cy, time);
}

void Script_SetFocus(displayContextDef_t *dc, itemDef_t *item, const char **args)
{
    char name[0x400];
    byte *focusItem;

    if (!String_Parse(args, name, 0x400))
        return;

    focusItem = (byte *)Menu_FindItemByName(item->parent, name);

    if (!focusItem) {
        Com_Printf("setFocus: could not find widget named '%s'\n", name);
        return;
    }

    if (!Item_SetFocus(dc, (itemDef_t *)focusItem, (((rectDef_t*)(focusItem))->x), (((rectDef_t*)(focusItem))->y))) {
        Com_Printf("setFocus: error focusing widget '%s' (widget was found but could not accept focus)\n", name);
        return;
    }

    if (Item_IsTextField((itemDef_t *)focusItem))
        Item_TextField_BeginEdit((itemDef_t *)focusItem);
}

void Script_SetFocusByDvar(displayContextDef_t *dc, itemDef_t *item, const char **args)
{
    char dvarName[0x400];
    menuDef_t *parent;
    itemDef_t *focusItem;
    int i;

    if (!String_Parse(args, dvarName, 0x400))
        return;

    parent = (menuDef_t *)item->parent;
    for (i = 0; i < parent->itemCount; i++) {
        focusItem = parent->items[i];

        if (!(focusItem->dvarFlags & 0x10))
            continue;

        if (!focusItem->dvarTest)
            Com_Error(ERR_DROP, "\x15Script_SetFocusByDvar: Item's dvarTest field is empty.");

        if (stricmp(focusItem->dvarTest, dvarName))
            continue;

        if (!Item_EnableShowViaDvar(focusItem, 0x10))
            continue;

        if (Item_SetFocus(dc, focusItem, focusItem->window.rect[0].x, focusItem->window.rect[0].y))
            return;
    }
}

void Script_SetDvar(displayContextDef_t *dc, itemDef_t *item, const char **args)
{
    (void)dc;
    (void)item;
    char dvarName[0x400], val[0x400];
    if (!String_Parse(args, dvarName, 0x400))
        return;
    if (!String_Parse(args, val, 0x400))
        return;
    Dvar_SetFromStringByName(dvarName, val);
}

// retail 0x535660, unreferenced; name unknown
void Script_ExecHandler_535660(int execWhen, const char **args)
{
    char val[0x400];
    if (String_Parse(args, val, 0x400)) {
        Cbuf_ExecuteText(execWhen, va("%s\n", val));
    }
}

void Script_Exec(displayContextDef_t *dc, itemDef_t *item, const char **args)
{
    (void)dc;
    (void)item;
    char val[0x400];
    if (String_Parse(args, val, 0x400)) {
        Cbuf_ExecuteText(2, va("%s\n", val));
    }
}

void Script_ExecNow(displayContextDef_t *dc, itemDef_t *item, const char **args)
{
    (void)dc;
    (void)item;
    char val[0x400];
    if (String_Parse(args, val, 0x400)) {
        Cbuf_ExecuteText(0, va("%s\n", val));
    }
}

static void Script_ConditionalExecHandler_impl(int execWhen, itemDef_t *item, const char **args, Bool (*shouldExec)(const char *, const char *))
{
    char dvarName[1024];
    (void)item;
    char testValue[1024];
    char command[1024];

    if (!String_Parse(args, dvarName, 1024))
        return;
    if (!String_Parse(args, testValue, 1024))
        return;
    if (!String_Parse(args, command, 1024))
        return;

    {
        const char *dvarValue = Dvar_GetVariantString(dvarName);
        if (!shouldExec(dvarValue, testValue))
            return;
    }

    Cbuf_ExecuteText(execWhen, va("%s\n", command));
}

static Bool Script_ExecIfStringsEqual(const char *dvarValue, const char *testValue)
{
    return I_stricmp(dvarValue, testValue) == 0 ? 1 : 0;
}

static Bool Script_ExecIfIntsEqual(const char *dvarValue, const char *testValue)
{
    return atoi(dvarValue) == atoi(testValue) ? 1 : 0;
}

static Bool Script_ExecIfFloatsEqual(const char *dvarValue, const char *testValue)
{
    return (float)fabs(atof(dvarValue) - atof(testValue)) < 9.999999747378752e-06f ? 1 : 0;
}

void Script_ExecOnDvarStringValue(displayContextDef_t *dc, itemDef_t *item, const char **args)
{
    (void)dc;
    (void)item;
    Script_ConditionalExecHandler_impl(2, item, args, Script_ExecIfStringsEqual);
}

void Script_ExecOnDvarIntValue(displayContextDef_t *dc, itemDef_t *item, const char **args)
{
    (void)dc;
    (void)item;
    Script_ConditionalExecHandler_impl(2, item, args, Script_ExecIfIntsEqual);
}

void Script_ExecOnDvarFloatValue(displayContextDef_t *dc, itemDef_t *item, const char **args)
{
    (void)dc;
    (void)item;
    Script_ConditionalExecHandler_impl(2, item, args, Script_ExecIfFloatsEqual);
}

void Script_ExecNowOnDvarStringValue(displayContextDef_t *dc, itemDef_t *item, const char **args)
{
    (void)dc;
    (void)item;
    Script_ConditionalExecHandler_impl(0, item, args, Script_ExecIfStringsEqual);
}

void Script_ExecNowOnDvarIntValue(displayContextDef_t *dc, itemDef_t *item, const char **args)
{
    (void)dc;
    (void)item;
    Script_ConditionalExecHandler_impl(0, item, args, Script_ExecIfIntsEqual);
}

void Script_ExecNowOnDvarFloatValue(displayContextDef_t *dc, itemDef_t *item, const char **args)
{
    (void)dc;
    (void)item;
    Script_ConditionalExecHandler_impl(0, item, args, Script_ExecIfFloatsEqual);
}

void Script_Play(displayContextDef_t *dc, itemDef_t *item, const char **args)
{
    char val[1024];
    (void)dc;
    (void)item;
    if (String_Parse(args, val, 1024)) {
        UI_PlayLocalSoundAliasByName(val);
    }
}

void Script_ScriptMenuResponse(displayContextDef_t *dc, itemDef_t *item, const char **args)
{
    char val[0x400];
    byte *it = (byte *)item;
    int iIndex;
    (void)dc;

    if (!((LegacyHacks *)::legacyHacks)->ui_scriptMenuAllowResponse)
        return;

    if (!String_Parse(args, val, 0x400))
        return;

    for (iIndex = 0; iIndex < 0x20; iIndex++) {
        const char *cs = CL_GetConfigString(0x4de + iIndex);
        if (cs[0] == '\0')
            continue;
        if (!I_stricmp(item->parent->window.name, cs))
            break;
    }
    if (iIndex == 0x20)
        iIndex = -1;

    Cbuf_ExecuteText(2, va("cmd mr %i %i %s\n", Dvar_GetInt("sv_serverId"), iIndex, val));
}

void Item_RunScript(displayContextDef_t *dc, itemDef_t *item, const char *s)
{
    char script[0x400];
    char command[0x400];
    const char *p;
    int i;
    int bRan;

    memset(script, 0, 0x400);
    if (!item || !s || s[0] == '\0')
        return;

    I_strncat(script, 0x400, s);
    p = script;

    while (String_Parse(&p, command, 0x400)) {
        if (command[0] == ';' && command[1] == '\0')
            continue;

        bRan = 0;
        for (i = 0; i < scriptCommandCount; i++) {
            if (I_stricmp(command, commandList[i].name) == 0) {
                ((void (__cdecl *)(displayContextDef_t *, itemDef_t *, const char **))commandList[i].handler)(dc, item, &p);
                bRan = 1;
                break;
            }
        }
        if (!bRan)
            UI_RunMenuScript(&p);
    }
}

// retail 0x535e10
int Item_ListBox_MaxScroll(itemDef_t *item)
{
    listBoxDef_s *listPtr;
    int count;
    int max;

    listPtr = Item_GetListBoxDef(item);
    count = UI_FeederCount(item->special);
    if (!listPtr)
        return 0;
    if (Window_IsHorizontal(&item->window))
        max = count - (int)(item->window.rect[0].w / listPtr->elementWidth) + 1;
    else
        max = count - (int)(item->window.rect[0].h / listPtr->elementHeight) + 1;
    if (max < 0)
        return 0;
    return max;
}

// retail 0x535e80
int Item_ListBox_ThumbPosition(itemDef_t *item)
{
    listBoxDef_s *listPtr;
    float size;
    float pos;
    int max;

    listPtr = Item_GetListBoxDef(item);
    if (!listPtr)
        return 0;
    max = Item_ListBox_MaxScroll(item);
    if (Window_IsHorizontal(&item->window)) {
        size = item->window.rect[0].w - 32.0f - 2.0f;
        if (max > 0)
            pos = (size - 16.0f) / (float)max;
        else
            pos = 0.0f;
        pos *= listPtr->startPos[0];
        return (int)(item->window.rect[0].x + 1.0f + 16.0f + pos);
    }
    size = item->window.rect[0].h - 32.0f - 2.0f;
    if (max > 0)
        pos = (size - 16.0f) / (float)max;
    else
        pos = 0.0f;
    pos *= listPtr->startPos[0];
    return (int)(item->window.rect[0].y + 1.0f + 16.0f + pos);
}

int Item_ListBox_ThumbDrawPosition(displayContextDef_t *dc, itemDef_t *item)
{
    int min, max;

    if (itemCapture == item) {
        if (Window_IsHorizontal((Window *)item)) {
            min = (int)(((rectDef_t *)item)->x + 16.0f + 1.0f);
            max = (int)(((rectDef_t *)item)->x + ((rectDef_t *)item)->w - 32.0f - 1.0f);
            if (dc->cursorx >= min + 8 && dc->cursorx <= max + 8)
                return dc->cursorx - 8;
            return Item_ListBox_ThumbPosition(item);
        } else {
            min = (int)(((rectDef_t *)item)->y + 16.0f + 1.0f);
            max = (int)(((rectDef_t *)item)->y + ((rectDef_t *)item)->h - 32.0f - 1.0f);
            if (dc->cursory >= min + 8 && dc->cursory <= max + 8)
                return dc->cursory - 8;
            return Item_ListBox_ThumbPosition(item);
        }
    }
    return Item_ListBox_ThumbPosition(item);
}

float Item_Slider_ThumbPosition(itemDef_t *item)
{
    byte *it = (byte *)item;
    editFieldDef_t *editDef = Item_GetEditFieldDef(item);
    float baseX, value;

    if (!editDef)
        return 0.0f;

    if (((itemDef_t *)it)->text)
        baseX = (((itemDef_s*)(it))->textRect[0].x) + (((itemDef_s*)(it))->textRect[0].w) + 8.0f;
    else
        baseX = (((rectDef_t*)(it))->x);

    value = Dvar_GetFloat(item->dvar);

    if (value < editDef->minVal)
        value = editDef->minVal;
    else if (value > editDef->maxVal)
        value = editDef->maxVal;

    value = (value - editDef->minVal) / (editDef->maxVal - editDef->minVal) * 84.0f;
    return value + baseX + 6.0f;
}

// retail 0x5360b0
static int Item_Slider_OverSlider(itemDef_t *item, float x, float y)
{
    rectDef_t r;

    r.x = Item_Slider_ThumbPosition(item) - 5.0f;
    r.y = item->window.rect[0].y - 2.0f;
    r.w = 10.0f;
    r.h = 20.0f;
    r.horzAlign = item->window.rect[0].horzAlign;
    r.vertAlign = item->window.rect[0].vertAlign;
    if (Rect_ContainsPoint(&r, x, y))
        return 0x400;
    return 0;
}

int Item_ListBox_OverLB(itemDef_t *item, float x, float y)
{
    rectDef_t r;
    struct listBoxDef_s *listPtr;
    int thumbstart;
    int count;

    count = UI_FeederCount(item->special);
    listPtr = Item_GetListBoxDef(item);
    if (!listPtr)
        return 0;
    r.horzAlign = item->window.rect[0].horzAlign;
    r.vertAlign = item->window.rect[0].vertAlign;
    if (Window_IsHorizontal((Window *)item)) {
        // left arrow
        r.y = item->window.rect[0].h + item->window.rect[0].y - 16.0f;
        r.x = item->window.rect[0].x;
        r.h = r.w = 16.0f;
        if (Rect_ContainsPoint(&r, x, y))
            return 0x100;
        // right arrow
        r.x = item->window.rect[0].x + item->window.rect[0].w - 16.0f;
        if (Rect_ContainsPoint(&r, x, y))
            return 0x200;
        // thumb
        thumbstart = Item_ListBox_ThumbPosition(item);
        r.x = thumbstart;
        if (Rect_ContainsPoint(&r, x, y))
            return 0x400;
        r.x = item->window.rect[0].x + 16.0f;
        r.w = thumbstart - r.x;
        if (Rect_ContainsPoint(&r, x, y))
            return 0x800;
        r.x = thumbstart + 16.0f;
        r.w = item->window.rect[0].x + item->window.rect[0].w - 16.0f;
        if (Rect_ContainsPoint(&r, x, y))
            return 0x1000;
    } else {
        r.x = item->window.rect[0].x + item->window.rect[0].w - 16.0f;
        r.y = item->window.rect[0].y;
        r.h = r.w = 16.0f;
        if (Rect_ContainsPoint(&r, x, y))
            return 0x100;
        r.y = item->window.rect[0].y + item->window.rect[0].h - 16.0f;
        if (Rect_ContainsPoint(&r, x, y))
            return 0x200;
        thumbstart = Item_ListBox_ThumbPosition(item);
        r.y = thumbstart;
        if (Rect_ContainsPoint(&r, x, y))
            return 0x400;
        r.y = item->window.rect[0].y + 16.0f;
        r.h = thumbstart - r.y;
        if (Rect_ContainsPoint(&r, x, y))
            return 0x800;
        r.y = thumbstart + 16.0f;
        r.h = item->window.rect[0].y + item->window.rect[0].h - 16.0f;
        if (Rect_ContainsPoint(&r, x, y))
            return 0x1000;
    }
    return 0;
}

void Item_MouseEnter(displayContextDef_t *dc, itemDef_t *item, float x, float y)
{
    rectDef_t r;
    const rectDef_t *textRect;
    int flags;

    if (!item)
        return;
    textRect = &item->textRect[0];
    r = *textRect;
    r.y -= r.h;
    r.horzAlign = textRect->horzAlign;
    r.vertAlign = textRect->vertAlign;
    if ((item->dvarFlags & 3) && !Item_EnableShowViaDvar(item, 1))
        return;
    if ((item->dvarFlags & 0xc) && !Item_EnableShowViaDvar(item, 4))
        return;
    flags = item->window.dynamicFlags[0];
    if (Rect_ContainsPoint(&r, x, y)) {
        if (!(flags & 0x40)) {
            Item_RunScript(dc, item, item->mouseEnterText);
            Window_AddDynamicFlags(&item->window, 0x40);
        }
        if (!(flags & 1)) {
            Item_RunScript(dc, item, item->mouseEnter);
            Window_AddDynamicFlags(&item->window, 1);
        }
    } else {
        if (flags & 0x40) {
            Item_RunScript(dc, item, item->mouseExitText);
            Window_RemoveDynamicFlags(&item->window, 0x40);
        }
        if (!(flags & 1)) {
            Item_RunScript(dc, item, item->mouseEnter);
            Window_AddDynamicFlags(&item->window, 1);
        }
        if (item->type == 6)
            Item_ListBox_MouseEnter(item, x, y);
    }
}

void Item_MouseLeave(displayContextDef_t *dc, itemDef_t *item)
{
    byte *it = (byte *)item;
    if (!item)
        return;
    if ((*(byte *)&((itemDef_t *)it)->window.dynamicFlags[0]) & 0x40) {

        Item_RunScript(dc, item, ((itemDef_t *)it)->mouseExitText);
        Window_RemoveDynamicFlags( (Window *)((itemDef_t *)((void *)item)), 0x40);
    }

    Item_RunScript(dc, item, ((itemDef_t *)it)->mouseExit);
    Window_RemoveDynamicFlags( (Window *)((itemDef_t *)((void *)item)), 0x300);
}

// retail 0x5366a0, unreferenced; Q3 Menu_HitTest (SUPPORTED)
itemDef_t *Menu_HitTest(menuDef_t *menu, float x, float y)
{
    int i;

    for (i = 0; i < menu->itemCount; i++) {
        if (Rect_ContainsPoint(&menu->items[i]->window.rect[0], x, y))
            return menu->items[i];
    }
    return NULL;
}

static void Item_SetMouseOver(itemDef_t *item, qboolean focus)
{
    if (item) {
        if (focus)
            Window_AddDynamicFlags(&item->window, 1);
        else
            Window_RemoveDynamicFlags(&item->window, 1);
    }
}

// retail 0x536730
static qboolean Item_OwnerDraw_HandleKey(itemDef_t *item, int key)
{
    return UI_OwnerDrawHandleKey(item->window.ownerDraw, item->window.ownerDrawFlags, &item->special, key);
}

static int Item_ContainsMouse(displayContextDef_t *dc, itemDef_t *item)
{
    return Rect_ContainsPoint(&item->window.rect[0], (float)dc->cursorx, (float)dc->cursory);
}

// retail 0x536d20
static bool Item_ShouldHandleKey(displayContextDef_t *dc, itemDef_t *item, int key)
{
    if (!Window_HasFocus(&item->window))
        return false;
    if ((key == 200 || key == 201 || key == 202) && !Item_ContainsMouse(dc, item))
        return false;
    return true;
}

// retail 0x536d80
qboolean Item_YesNo_HandleKey(displayContextDef_t *dc, itemDef_t *item, int key)
{
    char dvarString[25];

    if (!item->dvar)
        return 0;
    if (!Item_ShouldHandleKey(dc, item, key))
        return 0;
    if (key == 13 || key == 156 || key == 157 || key == 164 || key == 163
        || key == 200 || key == 201 || key == 202) {
        strncpy(dvarString, Dvar_GetVariantString(item->dvar), 24);
        dvarString[24] = 0;
        Dvar_SetFromStringByName(item->dvar, va("%i", atoi(dvarString) == 0));
        return 1;
    }
    return 0;
}

// retail 0x536e60
int Item_Multi_CountSettings(itemDef_t *item)
{
    multiDef_t *multiPtr;

    multiPtr = Item_GetMultiDef(item);
    if (!multiPtr)
        return 0;
    return multiPtr->count;
}

// retail 0x536e80
int Item_DvarEnum_CountSettings(itemDef_t *item)
{
    struct dvar_s *enumDvar;

    enumDvar = Dvar_FindVar(*(const char **)&item->typeData.listBox);
    if (enumDvar->type != 6)
        return 0;
    return enumDvar->domain.enumeration.stringCount;
}

// retail 0x536ea0
static int Item_Multi_FindDvarByValue(itemDef_t *item)
{
    multiDef_t *multiPtr;
    const char *string;
    float value;
    int i;

    multiPtr = Item_GetMultiDef(item);
    if (!multiPtr)
        return 0;
    if (multiPtr->strDef) {
        string = Dvar_GetVariantString(item->dvar);
        for (i = 0; i < multiPtr->count; i++) {
            if (!I_stricmp(string, multiPtr->dvarStr[i]))
                return i;
        }
    } else {
        value = (float)atof(Dvar_GetVariantString(item->dvar));
        for (i = 0; i < multiPtr->count; i++) {
            if (multiPtr->dvarValue[i] == value)
                return i;
        }
    }
    return 0;
}

// retail 0x536fa0
static const char *Item_Multi_GetText(itemDef_t *item)
{
    multiDef_t *multiDef;
    const char *value;
    float fval;
    int j;

    multiDef = *(multiDef_t **)&item->typeData.listBox;
    if (!multiDef)
        return "<dvarStrList or dvarFloatList not set>";

    if (multiDef->strDef) {
        value = Dvar_GetVariantString(item->dvar);
        for (j = 0; j < multiDef->count; j++) {
            if (I_stricmp(value, multiDef->dvarStr[j]) == 0)
                return multiDef->dvarList[j];
        }
    } else {
        fval = (float)atof(Dvar_GetVariantString(item->dvar));
        for (j = 0; j < multiDef->count; j++) {
            if (multiDef->dvarValue[j] == fval)
                return multiDef->dvarList[j];
        }
    }
    return "";
}

// retail 0x5370a0
static int Item_Enum_GetIndex(itemDef_t *item)
{
    struct dvar_s *dvar;
    const char *value;
    int idx;
    int j;

    dvar = Dvar_FindVar(*(const char **)&item->typeData.listBox);
    if (dvar->type != 6)
        return 0;

    value = Dvar_GetVariantString(item->dvar);
    idx = atoi(value);
    if (idx >= 0 && idx < dvar->domain.enumeration.stringCount)
        return idx;

    for (j = 0; j < dvar->domain.enumeration.stringCount; j++) {
        if (I_stricmp(value, dvar->domain.enumeration.strings[j]) == 0)
            return j;
    }
    return 0;
}

// retail 0x537140, unreferenced; Kisak Item_DvarEnum_Setting (SUPPORTED)
const char *Item_DvarEnum_Setting(itemDef_t *item)
{
    struct dvar_s *enumDvar;

    if (!*(const char **)&item->typeData.listBox)
        return "<dvarEnumList not set>";
    enumDvar = Dvar_FindVar(*(const char **)&item->typeData.listBox);
    if (enumDvar->type != 6)
        return "<not an enum dvar>";
    if (!enumDvar->domain.enumeration.stringCount)
        return "";
    return enumDvar->domain.enumeration.strings[Item_Enum_GetIndex(item)];
}

// retail 0x537190
static int Item_List_NextEntryForKey(int key, int current, int count)
{
    if (!count)
        return 0;
    if (key == 200 || key == 13 || key == 202 || key == 163 || key == 157)
        return (current + 1) % count;
    if (key == 201 || key == 164 || key == 156)
        return (current + count - 1) % count;
    return current;
}

// retail 0x5371f0
static qboolean Item_Multi_HandleKey(displayContextDef_t *dc, itemDef_t *item, int key)
{
    multiDef_t *multiPtr;
    int current;
    int count;
    int next;

    multiPtr = Item_GetMultiDef(item);
    if (!multiPtr)
        return 0;
    if (!Item_ShouldHandleKey(dc, item, key))
        return 0;
    current = Item_Multi_FindDvarByValue(item);
    count = Item_Multi_CountSettings(item);
    next = Item_List_NextEntryForKey(key, current, count);
    if (next == current)
        return 0;
    if (multiPtr->strDef)
        Dvar_SetFromStringByName(item->dvar, multiPtr->dvarStr[next]);
    else
        Dvar_SetFromStringByName(item->dvar, va("%g", multiPtr->dvarValue[next]));
    return 1;
}

// retail 0x5372a0
static qboolean Item_DvarEnum_HandleKey(displayContextDef_t *dc, itemDef_t *item, int key)
{
    int current;
    int count;
    int next;

    if (!Item_ShouldHandleKey(dc, item, key))
        return 0;
    current = Item_Enum_GetIndex(item);
    count = Item_DvarEnum_CountSettings(item);
    next = Item_List_NextEntryForKey(key, current, count);
    if (next == current)
        return 0;
    Dvar_SetFromStringByName(item->dvar, va("%i", next));
    return 1;
}

// retail 0x537320
static bool Item_IsTextField(itemDef_t *item)
{
    switch (item->type) {
    case 4:
    case 9:
    case 0x10:
    case 0x11:
    case 0x12:
        return true;
    default:
        return false;
    }
}

// retail 0x537360
static void Item_TextField_BeginEdit(itemDef_t *item)
{
    editFieldDef_t *editDef;

    editDef = Item_GetEditFieldDef(item);
    if (editDef)
        editDef->paintOffset = 0;
    item->cursorPos[0] = 0;
    g_editingField = 1;
    g_editItem = item;
    Key_SetOverstrikeMode(1);
}

// retail 0x5373a0
static void Item_TextField_EnsureCursorVisible(itemDef_t *item, const char *text)
{
    editFieldDef_t *editPtr;
    int cursorPos;
    int paintOffsetMin;

    editPtr = Item_GetEditFieldDef(item);
    cursorPos = item->cursorPos[0];
    if (editPtr->paintOffset > cursorPos) {
        editPtr->paintOffset = cursorPos;
    } else if (editPtr->maxPaintChars) {
        paintOffsetMin = Item_GetCursorPosOffset(item, text, -editPtr->maxPaintChars);
        if (editPtr->paintOffset < paintOffsetMin)
            editPtr->paintOffset = paintOffsetMin;
    }
}

qboolean Item_TextField_HandleKey(displayContextDef_t *dc, itemDef_t *item, int key)
{
    char buff[1024];
    int len;
    itemDef_t *newItem;
    editFieldDef_t *editPtr;

    editPtr = Item_GetEditFieldDef(item);
    if (!editPtr || !item->dvar)
        return 0;

    memset(buff, 0, sizeof(buff));
    I_strncpyz(buff, Dvar_GetVariantString(item->dvar), sizeof(buff));
    len = strlen(buff);
    if (editPtr->maxChars && len > editPtr->maxChars) {
        len = editPtr->maxChars;
        buff[len] = 0;
    }

    if (key & 0x400) {
        key &= ~0x400;

        if (key == 8) {
            if (item->cursorPos[0] > 0)
                memmove(&buff[item->cursorPos[0] - 1], &buff[item->cursorPos[0]], len + 1 - item->cursorPos[0]);
            Dvar_SetFromStringByName(item->dvar, buff);
            I_strncpyz(buff, Dvar_GetVariantString(item->dvar), sizeof(buff));
            item->cursorPos[0] = Item_GetCursorPosOffset(item, buff, -1);
            Item_TextField_EnsureCursorVisible(item, buff);
            return 1;
        }

        if (item->type == 16 && !I_isforfilename(key))
            return 1;
        if (key < 32 || !item->dvar)
            return 1;
        if (item->type == 9 && !I_isdigit(key))
            return 0;
        if (item->type == 17) {
            if (!I_isdigit(key) && key != Com_GetDecimalDelimiter())
                return 0;
            if (key == Com_GetDecimalDelimiter())
                key = '.';
        }
        if (item->type == 18)
            key = toupper(key);

        if (!Key_GetOverstrikeMode()) {
            if (len == 255 || editPtr->maxChars && len >= editPtr->maxChars)
                return 1;
            memmove(&buff[item->cursorPos[0] + 1], &buff[item->cursorPos[0]], len + 1 - item->cursorPos[0]);
        } else if (editPtr->maxChars && item->cursorPos[0] >= editPtr->maxChars) {
            if (editPtr->maxCharsGotoNext) {
                newItem = Menu_SetNextCursorItem(dc, item->parent);
                if (newItem && Item_IsTextField(newItem))
                    g_editItem = newItem;
            }
            return 1;
        }

        buff[item->cursorPos[0]] = key;
        Dvar_SetFromStringByName(item->dvar, buff);
        I_strncpyz(buff, Dvar_GetVariantString(item->dvar), sizeof(buff));
        item->cursorPos[0] = Item_GetCursorPosOffset(item, buff, 1);
        Item_TextField_EnsureCursorVisible(item, buff);
        if (editPtr->maxChars && item->cursorPos[0] >= editPtr->maxChars && editPtr->maxCharsGotoNext) {
            newItem = Menu_SetNextCursorItem(dc, item->parent);
            newItem->cursorPos[0] = 0;
            if (newItem && Item_IsTextField(newItem))
                g_editItem = newItem;
        }
    } else if (key == 162 || key == 193) {
        if (item->cursorPos[0] < len) {
            memmove(&buff[item->cursorPos[0]], &buff[item->cursorPos[0] + 1], len - item->cursorPos[0]);
            Dvar_SetFromStringByName(item->dvar, buff);
        }
        return 1;
    } else if (key == 157 || key == 187) {
        item->cursorPos[0] = Item_GetCursorPosOffset(item, buff, 1);
        Item_TextField_EnsureCursorVisible(item, buff);
        return 1;
    } else if (key == 156 || key == 185) {
        item->cursorPos[0] = Item_GetCursorPosOffset(item, buff, -1);
        Item_TextField_EnsureCursorVisible(item, buff);
        return 1;
    } else if (key == 165 || key == 182) {
        item->cursorPos[0] = 0;
        editPtr->paintOffset = 0;
        return 1;
    } else if (key == 166 || key == 188) {
        item->cursorPos[0] = len;
        Item_TextField_EnsureCursorVisible(item, buff);
        return 1;
    } else if (key == 161 || key == 192) {
        Key_SetOverstrikeMode(Key_GetOverstrikeMode() == 0);
        return 1;
    }

    if (key == 9 || key == 155 || key == 189) {
        newItem = Menu_SetNextCursorItem(dc, item->parent);
        if (newItem && Item_IsTextField(newItem))
            g_editItem = newItem;
    }
    if (key == 154 || key == 183) {
        newItem = Menu_SetPrevCursorItem(dc, item->parent);
        if (newItem && Item_IsTextField(newItem))
            g_editItem = newItem;
    }
    if ((key == 13 || key == 191) && item->onAccept)
        Item_RunScript(dc, item, item->onAccept);
    if (key == 13 || key == 191 || key == 27)
        return 0;
    return 1;
}

// q3 ui_shared.c shape
static void Scroll_ListBox_AutoFunc(displayContextDef_t *dc, void *p)
{
    scrollInfo_s *si = (scrollInfo_s *)p;

    if (dc->realTime > si->nextScrollTime) {
        // hold to keep scrolling
        Item_ListBox_HandleKey(dc, si->item, si->scrollKey, 1, 0);
        si->nextScrollTime = dc->realTime + si->adjustValue;
    }

    if (dc->realTime > si->nextAdjustTime) {
        si->nextAdjustTime = dc->realTime + 150;
        if (si->adjustValue > 20)
            si->adjustValue -= 40;
    }
}

static void Scroll_ListBox_ThumbFunc(displayContextDef_t *dc, void *p)
{
    scrollInfo_s *si = (scrollInfo_s *)p;
    rectDef_t r;
    int pos, max;
    listBoxDef_t *listPtr;

    listPtr = Item_GetListBoxDef(si->item);
    if (!listPtr)
        return;

    if (Window_IsHorizontal(&si->item->window)) {
        if (dc->cursorx == si->xStart)
            return;
        r.x = si->item->window.rect[0].x + 16.0f + 1.0f;
        r.w = si->item->window.rect[0].w - 32.0f - 2.0f;
        max = Item_ListBox_MaxScroll(si->item);
        pos = (int)((dc->cursorx - r.x - 8.0f) * max / (r.w - 16.0f));
        if (pos < 0)
            pos = 0;
        else if (pos > max)
            pos = max;
        listPtr->startPos[0] = pos;
        si->xStart = (float)dc->cursorx;
    } else if (dc->cursory != si->yStart) {
        r.y = si->item->window.rect[0].y + 16.0f + 1.0f;
        r.h = si->item->window.rect[0].h - 32.0f - 2.0f;
        max = Item_ListBox_MaxScroll(si->item);
        pos = (int)((dc->cursory - r.y - 8.0f) * max / (r.h - 16.0f));
        if (pos < 0)
            pos = 0;
        else if (pos > max)
            pos = max;
        listPtr->startPos[0] = pos;
        si->yStart = (float)dc->cursory;
    }

    if (dc->realTime > si->nextScrollTime) {
        // hold to keep scrolling
        Item_ListBox_HandleKey(dc, si->item, si->scrollKey, 1, 0);
        si->nextScrollTime = dc->realTime + si->adjustValue;
    }

    if (dc->realTime > si->nextAdjustTime) {
        si->nextAdjustTime = dc->realTime + 150;
        if (si->adjustValue > 20)
            si->adjustValue -= 40;
    }
}

// retail 0x537b90
static void Scroll_Slider_SetThumbPos(displayContextDef_t *dc, itemDef_t *item)
{
    editFieldDef_t *editDef;
    float x;
    float y;
    float w;
    float h;
    float cursorx;
    float value;

    editDef = Item_GetEditFieldDef(item);
    if (!editDef)
        return;
    if (item->text)
        x = item->textRect[0].x + item->textRect[0].w + 8.0f;
    else
        x = item->window.rect[0].x;
    cursorx = (float)dc->cursorx;
    CalcScreenX(&cursorx, 4);
    x += 6.0f;
    w = 84.0f;
    y = 0.0f;
    h = 0.0f;
    CalcScreenPlacement(&x, &y, &w, &h, item->window.rect[0].horzAlign, item->window.rect[0].vertAlign);
    value = I_fclamp(cursorx - x, 0.0f, w);
    Dvar_SetFromStringByName(item->dvar, va("%g", (editDef->maxVal - editDef->minVal) * (value / w) + editDef->minVal));
}

static void Scroll_Slider_ThumbFunc(displayContextDef_t *dc, void *p)
{
    byte *scroll = (byte *)p;
    Scroll_Slider_SetThumbPos(dc, *(itemDef_t **)(scroll + 0x18));
}

// retail 0x537ce0
static void Item_StartCapture(displayContextDef_t *dc, itemDef_t *item, int key)
{
    int flags;

    switch (item->type) {
    case 6:
        flags = Item_ListBox_OverLB(item, (float)dc->cursorx, (float)dc->cursory);
        if (flags & 0x300) {
            scrollInfo.nextScrollTime = dc->realTime + 500;
            scrollInfo.nextAdjustTime = dc->realTime + 150;
            scrollInfo.adjustValue = 500;
            scrollInfo.scrollKey = key;
            scrollInfo.scrollDir = (flags & 0x100) ? 1 : 0;
            scrollInfo.item = item;
            captureData = &scrollInfo;
            captureFunc = (void (__cdecl *)(void))(Scroll_ListBox_AutoFunc);
            itemCapture = item;
        } else if (flags & 0x400) {
            scrollInfo.scrollKey = key;
            scrollInfo.item = item;
            scrollInfo.xStart = (float)dc->cursorx;
            scrollInfo.yStart = (float)dc->cursory;
            captureData = &scrollInfo;
            captureFunc = (void (__cdecl *)(void))(Scroll_ListBox_ThumbFunc);
            itemCapture = item;
        }
        break;
    case 10:
        flags = Item_Slider_OverSlider(item, (float)dc->cursorx, (float)dc->cursory);
        if (flags & 0x400) {
            scrollInfo.scrollKey = key;
            scrollInfo.item = item;
            scrollInfo.xStart = (float)dc->cursorx;
            scrollInfo.yStart = (float)dc->cursory;
            captureData = &scrollInfo;
            captureFunc = (void (__cdecl *)(void))(Scroll_Slider_ThumbFunc);
            itemCapture = item;
        }
        break;
    }
}

qboolean Item_Slider_HandleKey(displayContextDef_t *dc, itemDef_t *item, int key, qboolean down)
{
    editFieldDef_t *editDef;
    float step;
    float value;

    if (!item->dvar)
        return 0;
    if (!Item_ShouldHandleKey(dc, item, key))
        return 0;
    if (key == 200 || key == 201 || key == 202) {
        Scroll_Slider_SetThumbPos(dc, item);
        return 1;
    }
    editDef = Item_GetEditFieldDef(item);
    if (!editDef)
        return 0;
    step = (editDef->maxVal - editDef->minVal) * 0.05f;
    value = (float)atof(Dvar_GetVariantString(item->dvar));
    if (key == 156 || key == 164) {
        value -= step;
        value = value > editDef->minVal ? value : editDef->minVal;
        Dvar_SetFromStringByName(item->dvar, va("%g", value));
        return 1;
    }
    if (key == 157 || key == 163) {
        value += step;
        value = value < editDef->maxVal ? value : editDef->maxVal;
        Dvar_SetFromStringByName(item->dvar, va("%g", value));
        return 1;
    }
    return 0;
}

qboolean Item_HandleKey(displayContextDef_t *dc, itemDef_t *item, int key, qboolean down)
{
    if (itemCapture) {
        itemCapture = NULL;
        captureFunc = NULL;
        captureData = NULL;
    } else {
        if (!down)
            return 0;
        if (key == 200 || key == 201 || key == 202)
            Item_StartCapture(dc, item, key);
    }

    if (!down)
        return 0;

    switch (item->type) {
    case 1:
        return 0;
    case 2:
        return 0;
    case 3:
        return 0;
    case 4:
    case 9:
        return 0;
    case 5:
        return 0;
    case 6:
        return Item_ListBox_HandleKey(dc, item, key, down, 0);
    case 11:
        return Item_YesNo_HandleKey(dc, item, key);
    case 12:
        return Item_Multi_HandleKey(dc, item, key);
    case 13:
        return Item_DvarEnum_HandleKey(dc, item, key);
    case 8:
        return Item_OwnerDraw_HandleKey(item, key);
    case 14:
        return Item_Bind_HandleKey(dc, item, key, down);
    case 10:
        return Item_Slider_HandleKey(dc, item, key, down);
    case 7:
        return 0;
    case 15:
        return 0;
    case 16:
    case 17:
    case 18:
        return 0;
    default:
        return 0;
    }
}

// retail 0x538050
static void Item_Action(displayContextDef_t *dc, itemDef_t *item)
{
    if (item)
        Item_RunScript(dc, item, item->action);
}

itemDef_t *Menu_SetPrevCursorItem(displayContextDef_t *dc, menuDef_t *menu)
{
    qboolean wrapped;
    int oldCursor;

    wrapped = 0;
    oldCursor = menu->cursorItem[0];
    if (menu->cursorItem[0] < 0) {
        menu->cursorItem[0] = menu->itemCount - 1;
        wrapped = 1;
    }
    while (menu->cursorItem[0] > -1) {
        menu->cursorItem[0]--;
        if (menu->cursorItem[0] < 0 && !wrapped) {
            wrapped = 1;
            menu->cursorItem[0] = menu->itemCount - 1;
        }
        if (menu->cursorItem[0] < 0)
            break;
        if (Item_SetFocus(dc, menu->items[menu->cursorItem[0]], (float)dc->cursorx, (float)dc->cursory)) {
            Menu_HandleMouseMove(dc, menu, menu->items[menu->cursorItem[0]]->window.rect[0].x + 1.0f,
                                 menu->items[menu->cursorItem[0]]->window.rect[0].y + 1.0f);
            return menu->items[menu->cursorItem[0]];
        }
    }
    menu->cursorItem[0] = oldCursor;
    return NULL;
}

static void Window_CloseCinematic(Window *window)
{
    if (window->style == 5 && window->cinematic >= 0) {
        CIN_StopCinematic(window->cinematic);
        window->cinematic = -1;
    }
}

static void Menu_CloseCinematics(menuDef_t *menu)
{
    int i;

    if (menu) {
        Window_CloseCinematic(&menu->window);
        for (i = 0; i < menu->itemCount; i++) {
            Window_CloseCinematic(&menu->items[i]->window);
            if (menu->items[i]->type == 8)
                CIN_StopCinematic(-menu->items[i]->window.ownerDraw);
        }
    }
}

static void Display_CloseCinematics(displayContextDef_t *dc)
{
    int i;

    for (i = dc->openMenuCount - 1; i >= 0; i--)
        Menu_CloseCinematics(dc->menuStack[i]);
}

void Menus_Open(displayContextDef_t *dc, menuDef_t *menu)
{
    itemDef_t item;
    int i;

    for (i = dc->openMenuCount - 1; i >= 0; i--)
        Window_RemoveDynamicFlags(&dc->menuStack[i]->window, 2);
    Menus_AddToStack(dc, menu);
    Window_AddDynamicFlags(&menu->window, 6);
    if (menu->onOpen) {
        item.parent = menu;
        Item_RunScript(dc, &item, menu->onOpen);
    }
    if (menu->soundName)
        UI_PlayLocalSoundAliasByName(menu->soundName);
    Display_CloseCinematics(dc);
    Display_MouseMove(dc, NULL, dc->cursorx, dc->cursory);
}

qboolean Menus_OpenByName(displayContextDef_t *dc, const char *p)
{
    menuDef_t *menu = Menus_FindByName(dc, p);
    if (menu) {
        Menus_Open(dc, menu);
        return 1;
    }
    return 0;
}

static int Display_VisibleMenuCount(displayContextDef_t *dc)
{
    int i, count;

    count = 0;
    for (i = 0; i < dc->menuCount; i++) {
        if (dc->Menus[i]->window.dynamicFlags[0] & 0x4004)
            count++;
    }
    return count;
}

void Menus_HandleOOBClick(displayContextDef_t *dc, menuDef_t *menu, int key, qboolean down)
{
    int i, j;

    if (!menu)
        return;
    if (down && (menu->window.staticFlags & 0x2000000)) {
        Menu_RunCloseScript(dc, menu);
        Window_RemoveDynamicFlags(&menu->window, 6);
    }
    for (i = dc->openMenuCount - 1; i >= 0; i--) {
        if (Menu_OverActiveItem(dc->menuStack[i], (float)dc->cursorx, (float)dc->cursory)) {
            for (j = dc->openMenuCount - 1; j >= 0; j--)
                Window_RemoveDynamicFlags(&dc->menuStack[j]->window, 2);
            Window_AddDynamicFlags(&dc->menuStack[i]->window, 6);
            Display_MouseMove(dc, NULL, dc->cursorx, dc->cursory);
            Menu_HandleMouseMove(dc, dc->menuStack[i], (float)dc->cursorx, (float)dc->cursory);
            Menu_HandleKey(dc, dc->menuStack[i], key, down);
            break;
        }
    }
    if (!Display_VisibleMenuCount(dc))
        UI_Pause(0);
    Display_CloseCinematics(dc);
}

static rectDef_t *Item_CorrectedTextRect(itemDef_t *item)
{
    static rectDef_t rect;
    const rectDef_t *textRect;

    memset(&rect, 0, sizeof(rect));
    if (item) {
        textRect = &item->textRect[0];
        rect = *textRect;
        if (rect.w != 0.0f)
            rect.y -= rect.h;
    }
    return &rect;
}

qboolean Menu_CheckOnKey(displayContextDef_t *dc, menuDef_t *menu, int key)
{
    itemDef_t it;
    ItemKeyHandler *handler;
    itemDef_t *item;
    int i;

    for (handler = menu->onKey; handler; handler = (ItemKeyHandler *)handler->next) {
        if (handler->key == key) {
            it.parent = menu;
            Item_RunScript(dc, &it, handler->action);
            return 1;
        }
    }

    for (i = 0; i < menu->itemCount; i++) {
        item = menu->items[i];
        if (!Window_IsVisible(&item->window))
            continue;
        if ((item->dvarFlags & 0xc) && !Item_EnableShowViaDvar(item, 4))
            continue;
        for (handler = item->onKey; handler; handler = (ItemKeyHandler *)handler->next) {
            if (handler->key == key) {
                it.parent = menu;
                Item_RunScript(dc, &it, handler->action);
                return 1;
            }
        }
    }

    return 0;
}

void Menu_HandleKey(displayContextDef_t *dc, menuDef_t *menu, int key, qboolean down)
{
    itemDef_t *item = NULL;
    int i;

    if (g_waitingForKey && down) {
        Item_Bind_HandleKey(dc, g_bindItem, key, down);
        return;
    }

    if (g_editingField && down) {
        if (!Item_TextField_HandleKey(dc, g_editItem, key)) {
            g_editingField = 0;
            g_editItem = NULL;
            return;
        } else if (key == 200 || key == 201 || key == 202) {
            g_editingField = 0;
            g_editItem = NULL;
            Display_MouseMove(dc, NULL, dc->cursorx, dc->cursory);
        } else {
            return;
        }
    }

    if (menu == NULL)
        return;

    // see if the mouse is within the window bounds and if so is this a mouse click
    if (down && !(menu->window.staticFlags & 0x1000000) && !menu->fullScreen
        && !Rect_ContainsPoint(&menu->window.rect[0], (float)dc->cursorx, (float)dc->cursory)) {
        if (!inHandleKey && (key == 200 || key == 201 || key == 202)) {
            inHandleKey = 1;
            Menus_HandleOOBClick(dc, menu, key, down);
            inHandleKey = 0;
            return;
        }
    }

    // get the item with focus
    for (i = 0; i < menu->itemCount; i++) {
        if (Window_HasFocus(&menu->items[i]->window))
            item = menu->items[i];
    }

    // mouse wheel only scrolls listboxes
    if (key == 205 || key == 206) {
        if (!item || item->type != 6)
            return;
    }

    if (item != NULL) {
        if (Item_HandleKey(dc, item, key, down)) {
            Item_RunScript(dc, item, item->action);
            return;
        }
    }

    if (!down)
        return;

    if (key > 0 && key <= 255 && Menu_CheckOnKey(dc, menu, key))
        return;

    // default handling
    switch (key) {
    case 177:
        if (Dvar_GetInt("developer"))
            debugMode ^= 1;
        break;

    case 178:
        if (Dvar_GetInt("developer"))
            Cbuf_AddText("screenshot\n");
        break;

    case 154:
    case 156:
    case 183:
    case 206:
        Menu_SetPrevCursorItem(dc, menu);
        break;

    case 27:
        if (!g_waitingForKey && menu->onESC) {
            itemDef_t it;
            it.parent = menu;
            Item_RunScript(dc, &it, menu->onESC);
        }
        break;

    case 9:
    case 155:
    case 157:
    case 189:
    case 205:
        Menu_SetNextCursorItem(dc, menu);
        break;

    case 200:
    case 201:
        if (item) {
            if (item->type == 0) {
                if (Rect_ContainsPoint(Item_CorrectedTextRect(item), (float)dc->cursorx, (float)dc->cursory))
                    Item_Action(dc, item);
            } else if (Rect_ContainsPoint(&item->window.rect[0], (float)dc->cursorx, (float)dc->cursory)) {
                if (Item_IsTextField(item))
                    Item_TextField_BeginEdit(item);
                else
                    Item_Action(dc, item);
            }
        }
        break;

    case 13:
    case 191:
    case 202:
        if (item) {
            if (Item_IsTextField(item)) {
                item->cursorPos[0] = 0;
                g_editingField = 1;
                g_editItem = item;
                Key_SetOverstrikeMode(1);
            } else {
                Item_Action(dc, item);
            }
        }
        break;
    }
}

// retail 0x538d10
void ToWindowCoords(float *x, float *y, Window *window)
{
    if (window->border) {
        *x += window->borderSize;
        *y += window->borderSize;
    }
    *x += window->rect[0].x;
    *y += window->rect[0].y;
}

// retail 0x538d60
// retail 0x538d40, unreferenced; name unknown
FontHandle Item_GetFont_538d40(itemDef_t *item)
{
    return UI_GetFontHandle(item->fontEnum, item->textscale);
}

void Item_SetTextExtents(itemDef_t *item, int *width, int *height, const char *text)
{
    const char *textPtr;
    FontHandle font;
    int originalWidth;
    rectDef_t *textRect;
    rectDef_t newRect;
    const char *dvarString;

    if (text) {
        textPtr = text;
    } else {
        textPtr = item->text;
        if (!textPtr)
            return;
        if (*textPtr == '@')
            textPtr = UI_SafeTranslateString(textPtr + 1);
    }

    textRect = &item->textRect[0];
    *width = (int)textRect->w;
    *height = (int)textRect->h;

    if (*width && ((item->type != 8 && !item->dvar) || item->textalignment != 1) && item->textalignment != 3)
        return;

    font = UI_GetFontHandle(item->fontEnum, item->textscale);
    originalWidth = UI_TextWidth(textPtr, 0, font, item->textscale);
    if (item->type == 8 && (item->textalignment == 1 || item->textalignment == 2)) {
        originalWidth += UI_OwnerDrawWidth(item->window.ownerDraw, font, item->textscale);
    } else if (Item_IsTextField(item) && item->textalignment == 1 && item->dvar) {
        dvarString = Dvar_GetVariantString(item->dvar);
        originalWidth += UI_TextWidth(dvarString, 0, font, item->textscale);
    } else if (item->textalignment == 3) {
        originalWidth += UI_TextWidth(text, 0, font, item->textscale);
    }

    *width = UI_TextWidth(textPtr, 0, font, item->textscale);
    *height = UI_TextHeight(font, item->textscale);

    newRect = *textRect;
    newRect.w = (float)*width;
    newRect.h = (float)*height;
    newRect.x = item->textalignx;
    newRect.y = item->textaligny;
    newRect.horzAlign = item->window.rect[0].horzAlign;
    newRect.vertAlign = item->window.rect[0].vertAlign;
    if (item->textalignment == 2)
        newRect.x = item->textalignx - (float)originalWidth;
    else if (item->textalignment == 1 || item->textalignment == 3)
        newRect.x = item->textalignx - (float)(originalWidth / 2);
    ToWindowCoords(&newRect.x, &newRect.y, &item->window);
    *textRect = newRect;
}

// retail 0x539040
void Item_TextColor(displayContextDef_t *dc, itemDef_t *item, vec4_t *newColor)
{
    vec4_t lowLight;
    menuDef_t *parent = item->parent;
    int flags = item->window.dynamicFlags[0];

    Fade(&flags, &item->window.foreColor[3], parent->fadeClamp, &item->window.nextTime, parent->fadeCycle, 1, parent->fadeAmount, parent->fadeInAmount, dc);
    Window_SetDynamicFlags(&item->window, flags);

    if (Window_HasFocus(&item->window)) {
        lowLight[0] = parent->focusColor[0] * 0.8f;
        lowLight[1] = parent->focusColor[1] * 0.8f;
        lowLight[2] = parent->focusColor[2] * 0.8f;
        lowLight[3] = parent->focusColor[3] * 0.8f;
        LerpColor(parent->focusColor, lowLight, *newColor, (sin((float)(dc->realTime / 75)) + 1.0f) * 0.5f);
    } else if (item->textStyle == 1 && !((dc->realTime / 256) & 1)) {
        lowLight[0] = item->window.foreColor[0] * 0.8f;
        lowLight[1] = item->window.foreColor[1] * 0.8f;
        lowLight[2] = item->window.foreColor[2] * 0.8f;
        lowLight[3] = item->window.foreColor[3] * 0.8f;
        LerpColor(item->window.foreColor, lowLight, *newColor, (sin((float)(dc->realTime / 75)) + 1.0f) * 0.5f);
    } else {
        memcpy(newColor, item->window.foreColor, sizeof(vec4_t));
    }

    if (item->enableDvar && *item->enableDvar && item->dvarTest && *item->dvarTest) {
        if ((item->dvarFlags & 3) && !Item_EnableShowViaDvar(item, 1))
            memcpy(newColor, parent->disableColor, sizeof(vec4_t));
    }
}

void Item_Text_Wrapped_Paint(itemDef_t *item, const char *textPtr, vec_t *color)
{
    char buff[1024];
    FontHandle font;
    int height;
    float x, y;
    const char *start;
    const char *p;

    font = UI_GetFontHandle(item->fontEnum, item->textscale);
    height = UI_TextHeight(font, item->textscale);
    y = item->textRect[0].y;
    start = textPtr;
    p = strchr(textPtr, '\r');
    while (p && *p)
    {
        strncpy(buff, start, p - start + 1);
        buff[p - start] = '\0';
        if (item->textalignment == 1)
            x = item->textRect[0].x + (item->textRect[0].w - UI_TextWidth(buff, 0, font, item->textscale)) * 0.5f;
        else if (item->textalignment == 2)
            x = item->textRect[0].w + item->textRect[0].x - UI_TextWidth(buff, 0, font, item->textscale);
        else
            x = item->textRect[0].x;
        UI_DrawText(buff, 0x7fffffff, font, x, y, item->window.rect[0].horzAlign, item->window.rect[0].vertAlign, item->textscale, color, item->textStyle);
        y += height + 5;
        start = p + 1;
        p = strchr(p + 1, '\r');
    }
    if (item->textalignment == 1)
        x = item->textRect[0].x + (item->textRect[0].w - UI_TextWidth(start, 0, font, item->textscale)) * 0.5f;
    else if (item->textalignment == 2)
        x = item->textRect[0].w + item->textRect[0].x - UI_TextWidth(start, 0, font, item->textscale);
    else
        x = item->textRect[0].x;
    UI_DrawText(start, 0x7fffffff, font, x, y, item->window.rect[0].horzAlign, item->window.rect[0].vertAlign, item->textscale, color, item->textStyle);
}

void Item_Text_Paint(displayContextDef_t *dc, itemDef_t *item)
{
    byte *it = (byte *)item;
    const char *textPtr = ((itemDef_t *)it)->text;
    char text[0x400];
    float color[4];
    int width, height;
#if COD2_TRACE
    const char *traceParent = UI_TraceScriptItemParentName(item);
    int traceText = 0;

    if (traceParent) {
        static int textTraceCount;
        if (textTraceCount < 320) {
            traceText = 1;
            ++textTraceCount;
        }
    }
#endif

    if (!textPtr) {
        if (!(*(void **)&((itemDef_t *)it)->dvar)) {
#if COD2_TRACE
            if (traceText)
                if (getenv("MTRACE"))
                    Com_Printf("[menu-trace] Item_Text_Paint no text parent=%s item=%s type=%d dvar=%s\n",
                               traceParent,
                               item->window.name ? item->window.name : "<null>",
                               item->type,
                               item->dvar ? item->dvar : "");
#endif
            return;
        }
        I_strncpyz(text, Dvar_GetVariantString(item->dvar), 0x400);
        textPtr = text;
    }

    if (textPtr[0] == '@')
        textPtr = UI_SafeTranslateString(textPtr + 1);

    if (textPtr[0] == '\0') {
#if COD2_TRACE
        if (traceText)
            if (getenv("MTRACE"))
                Com_Printf("[menu-trace] Item_Text_Paint empty parent=%s item=%s type=%d dvar=%s\n",
                           traceParent,
                           item->window.name ? item->window.name : "<null>",
                           item->type,
                           item->dvar ? item->dvar : "");
#endif
        return;
    }

    Item_TextColor(dc, item, (vec4_t *)color);
    Item_SetTextExtents(item, &width, &height, textPtr);

    int staticFlags = ((itemDef_t *)it)->window.staticFlags;
    if (staticFlags & 0x400000) {
#if COD2_TRACE
        if (traceText)
            if (getenv("MTRACE"))
                Com_Printf("[menu-trace] Item_Text_Paint wrapped parent=%s item=%s text='%.80s' tr=(%.1f %.1f %.1f %.1f) color=(%.2f %.2f %.2f %.2f) static=0x%x\n",
                           traceParent,
                           item->window.name ? item->window.name : "<null>",
                           textPtr,
                           (((itemDef_s*)(it))->textRect[0].x), (((itemDef_s*)(it))->textRect[0].y), (((itemDef_s*)(it))->textRect[0].w), (((itemDef_s*)(it))->textRect[0].h),
                           color[0], color[1], color[2], color[3],
                           staticFlags);
#endif
        Item_Text_Wrapped_Paint(item, textPtr, color);
        return;
    }
    if (staticFlags & 0x800000) {
#if COD2_TRACE
        if (traceText)
            if (getenv("MTRACE"))
                Com_Printf("[menu-trace] Item_Text_Paint autowrap parent=%s item=%s text='%.80s' tr=(%.1f %.1f %.1f %.1f) color=(%.2f %.2f %.2f %.2f) static=0x%x\n",
                           traceParent,
                           item->window.name ? item->window.name : "<null>",
                           textPtr,
                           (((itemDef_s*)(it))->textRect[0].x), (((itemDef_s*)(it))->textRect[0].y), (((itemDef_s*)(it))->textRect[0].w), (((itemDef_s*)(it))->textRect[0].h),
                           color[0], color[1], color[2], color[3],
                           staticFlags);
#endif
        Item_Text_AutoWrapped_Paint(item, textPtr, color);
        return;
    }

    FontHandle font = UI_GetFontHandle(((itemDef_t *)it)->fontEnum, ((itemDef_t *)it)->textscale);
#if COD2_TRACE
    if (traceText)
        if (getenv("MTRACE"))
            Com_Printf("[menu-trace] Item_Text_Paint draw parent=%s item=%s text='%.80s' tr=(%.1f %.1f %.1f %.1f) wh=(%d %d) rectAlign=(%d %d) scale=%.3f style=%d font=%p color=(%.2f %.2f %.2f %.2f) static=0x%x\n",
                       traceParent,
                       item->window.name ? item->window.name : "<null>",
                       textPtr,
                       (((itemDef_s*)(it))->textRect[0].x), (((itemDef_s*)(it))->textRect[0].y), (((itemDef_s*)(it))->textRect[0].w), (((itemDef_s*)(it))->textRect[0].h),
                       width, height,
                       (((rectDef_t*)(it))->horzAlign), (((rectDef_t*)(it))->vertAlign),
                       ((itemDef_t *)it)->textscale, (((itemDef_s*)(it))->textStyle), (void *)font,
                       color[0], color[1], color[2], color[3],
                       staticFlags);
#endif
    UI_DrawText(textPtr, 0x7fffffff, font,
                (((itemDef_s*)(it))->textRect[0].x), (((itemDef_s*)(it))->textRect[0].y),
                (((rectDef_t*)(it))->horzAlign), (((rectDef_t*)(it))->vertAlign),
                ((itemDef_t *)it)->textscale, color, (((itemDef_s*)(it))->textStyle));
}

void Item_TextField_Paint(displayContextDef_t *dc, itemDef_t *item)
{
    char buff[1024];
    vec4_t newColor;
    vec4_t lowLight;
    int offset;
    int maxChars;
    float x;
    const char *text;
    int width;
    int height;
    menuDef_t *parent;
    editFieldDef_t *editPtr;
    dvar_t *dvar;

    editPtr = Item_GetEditFieldDef(item);
    if (!editPtr)
        return;

    Item_Text_Paint(dc, item);

    buff[0] = 0;
    if (item->dvar) {
        if (item->type == 17) {
            dvar = Dvar_FindVar(item->dvar);
            if (dvar && dvar->type == 1)
                Com_LocalizedFloatToString(dvar->current.value, buff, sizeof(buff), 2);
        } else {
            I_strncpyz(buff, Dvar_GetVariantString(item->dvar), sizeof(buff));
        }
    }

    Item_SetTextExtents(item, &width, &height, buff);

    parent = item->parent;
    if (Window_HasFocus(&item->window)) {
        lowLight[0] = parent->focusColor[0] * 0.8f;
        lowLight[1] = parent->focusColor[1] * 0.8f;
        lowLight[2] = parent->focusColor[2] * 0.8f;
        lowLight[3] = parent->focusColor[3] * 0.8f;
        LerpColor(parent->focusColor, lowLight, newColor, (sin((float)(dc->realTime / 75)) + 1.0f) * 0.5f);
    } else {
        memcpy(newColor, item->window.foreColor, sizeof(vec4_t));
    }

    offset = (item->text && *item->text) ? 8 : 0;
    x = item->textRect[0].x + (item->textRect[0].w + offset);
    text = buff + editPtr->paintOffset;
    maxChars = editPtr->maxPaintChars ? editPtr->maxPaintChars : 0x7fffffff;

    if (Window_HasFocus(&item->window) && g_editingField) {
        UI_DrawTextWithCursor(text, maxChars, UI_GetFontHandle(item->fontEnum, item->textscale), x, item->textRect[0].y,
                              item->window.rect[0].horzAlign, item->window.rect[0].vertAlign,
                              item->textscale, newColor, item->textStyle,
                              item->cursorPos[0] - editPtr->paintOffset, Key_GetOverstrikeMode() ? '_' : '|');
    } else {
        UI_DrawText(text, maxChars, UI_GetFontHandle(item->fontEnum, item->textscale), x, item->textRect[0].y,
                    item->textRect[0].horzAlign, item->textRect[0].vertAlign,
                    item->textscale, newColor, item->textStyle);
    }
}

void Item_YesNo_Paint(displayContextDef_t *dc, itemDef_t *item)
{
    vec4_t newColor;
    vec4_t lowLight;
    float value;
    menuDef_t *parent;
    const char *yesno;

    parent = item->parent;
    value = item->dvar ? (float)atof(Dvar_GetVariantString(item->dvar)) : 0.0f;

    if (Window_HasFocus(&item->window)) {
        lowLight[0] = parent->focusColor[0] * 0.8f;
        lowLight[1] = parent->focusColor[1] * 0.8f;
        lowLight[2] = parent->focusColor[2] * 0.8f;
        lowLight[3] = parent->focusColor[3] * 0.8f;
        LerpColor(parent->focusColor, lowLight, newColor, (sin((float)(dc->realTime / 75)) + 1.0f) * 0.5f);
    } else {
        memcpy(newColor, item->window.foreColor, sizeof(vec4_t));
    }

    yesno = UI_SafeTranslateString(value != 0.0f ? "EXE_YES" : "EXE_NO");

    if (item->text) {
        Item_Text_Paint(dc, item);
        UI_DrawText(yesno, 0x7fffffff, UI_GetFontHandle(item->fontEnum, item->textscale),
                    item->textRect[0].x + item->textRect[0].w + 8.0f, item->textRect[0].y,
                    item->window.rect[0].horzAlign, item->window.rect[0].vertAlign, item->textscale, newColor, item->textStyle);
    } else {
        UI_DrawText(yesno, 0x7fffffff, UI_GetFontHandle(item->fontEnum, item->textscale),
                    item->textRect[0].x, item->textRect[0].y,
                    item->window.rect[0].horzAlign, item->window.rect[0].vertAlign, item->textscale, newColor, item->textStyle);
    }
}

static void Item_List_Paint(displayContextDef_t *dc, itemDef_t *item, const char *text)
{
    vec4_t newColor;
    vec4_t lowLight;
    float *focusColor;

    if (Window_HasFocus(&item->window)) {
        focusColor = item->parent->focusColor;
        lowLight[0] = item->parent->focusColor[0] * 0.8f;
        lowLight[1] = focusColor[1] * 0.8f;
        lowLight[2] = focusColor[2] * 0.8f;
        lowLight[3] = focusColor[3] * 0.8f;
        LerpColor(focusColor, lowLight, newColor, (sin((float)(dc->realTime / 75)) + 1.0f) * 0.5f);
    } else {
        memcpy(newColor, item->window.foreColor, sizeof(vec4_t));
    }

    if (*text == '@')
        text = UI_SafeTranslateString(text + 1);

    if (item->text) {
        Item_Text_Paint(dc, item);
        UI_DrawText(text, 0x7fffffff, UI_GetFontHandle(item->fontEnum, item->textscale),
                    item->textRect[0].x + item->textRect[0].w + 8.0f, item->textRect[0].y,
                    item->window.rect[0].horzAlign, item->window.rect[0].vertAlign, item->textscale, newColor, item->textStyle);
    } else {
        UI_DrawText(text, 0x7fffffff, UI_GetFontHandle(item->fontEnum, item->textscale),
                    item->textRect[0].x, item->textRect[0].y,
                    item->window.rect[0].horzAlign, item->window.rect[0].vertAlign, item->textscale, newColor, item->textStyle);
    }
}

// retail 0x53a040
static void Item_Multi_Paint(displayContextDef_t *dc, itemDef_t *item)
{
    Item_List_Paint(dc, item, Item_Multi_GetText(item));
}

// retail 0x53a060
static void Item_Enum_Paint(displayContextDef_t *dc, itemDef_t *item)
{
    const char *text;
    struct dvar_s *dvar;

    if (!*(const char **)&item->typeData.listBox) {
        text = "<dvarEnumList not set>";
    } else {
        dvar = Dvar_FindVar(*(const char **)&item->typeData.listBox);
        if (dvar->type != 6)
            text = "<not an enum dvar>";
        else if (!dvar->domain.enumeration.stringCount)
            text = "";
        else
            text = dvar->domain.enumeration.strings[Item_Enum_GetIndex(item)];
    }
    Item_List_Paint(dc, item, text);
}

static void Controls_GetKeyAssignment(const char *command, int *twokeys)
{
    int count;
    int j;
    char b[256];

    twokeys[0] = twokeys[1] = -1;
    count = 0;

    for (j = 0; j < 256; j++) {
        Key_GetBindingBuf(j, b, 256);
        if (*b == 0)
            continue;
        if (!I_stricmp(b, command)) {
            twokeys[count] = j;
            count++;
            if (count == 2)
                break;
        }
    }
}

void Controls_GetConfig(void)
{
    int i;
    int twokeys[2];

    for (i = 0; i < 56; i++) {
        Controls_GetKeyAssignment(g_bindings[i].command, twokeys);
        g_bindings[i].bind1 = twokeys[0];
        g_bindings[i].bind2 = twokeys[1];
    }
}

void Controls_SetConfig(qboolean restart)
{
    int i;

    for (i = 0; i < 56; i++) {
        if (g_bindings[i].bind1 != -1) {
            Key_SetBinding(g_bindings[i].bind1, g_bindings[i].command);
            if (g_bindings[i].bind2 != -1)
                Key_SetBinding(g_bindings[i].bind2, g_bindings[i].command);
        }
    }
    Cbuf_ExecuteText(2, "in_restart\n");
}

void Controls_SetDefaults(void)
{
    int i;   /* was byte-stride 0x14 (x86 sizeof(bind_t)) */
    for (i = 0; i < 56; i++) {
        g_bindings[i].bind1 = g_bindings[i].defaultbind1;
        g_bindings[i].bind2 = g_bindings[i].defaultbind2;
    }
}

static int BindingIDFromName(const char *name)
{
    int i;

    for (i = 0; i < 56; i++) {
        if (!I_stricmp(name, g_bindings[i].command))
            return i;
    }
    return -1;
}

void BindingFromName(const char *dvar, char *nameBind)
{
    int i, b1, b2;
    char nameBind2[0x80];

    for (i = 0; i < 0x38; i++) {
        if (I_stricmp(dvar, g_bindings[i].command) == 0) {
            b1 = g_bindings[i].bind1;
            if (b1 == -1)
                break;

            Key_KeynumToStringBuf(b1, nameBind, 0x20);
            I_strncpyz(nameBind, UI_SafeTranslateString(nameBind), 0x80);

            b2 = g_bindings[i].bind2;
            if (b2 != -1) {
                Key_KeynumToStringBuf(b2, nameBind2, 0x20);
                I_strncpyz(nameBind2, UI_SafeTranslateString(nameBind2), 0x80);
                I_strncat(nameBind, 0x80, va(" %s ", UI_SafeTranslateString("KEY_OR")));
                I_strncat(nameBind, 0x80, nameBind2);
            }
            return;
        }
    }

    I_strncpyz(nameBind, UI_SafeTranslateString("KEY_UNBOUND"), 0x80);
}

qboolean GetCommandHasBinding(const char *command)
{

    int i;
    for (i = 0; i < 56; i++) {
        if (I_stricmp(command, g_bindings[i].command) == 0) {
            if (g_bindings[i].bind1 == -1)
                break;
            return 1;
        }
    }
    return 0;
}

int GetKeyBindings(const char *command, char (*bindings)[128])
{
    int i;
    int count;

    bindings[0][0] = 0;
    bindings[1][0] = 0;
    for (i = 0; i < 56; i++) {
        if (!I_stricmp(command, g_bindings[i].command)) {
            if (g_bindings[i].bind1 == -1)
                break;
            Key_KeynumToStringBuf(g_bindings[i].bind1, bindings[0], 128);
            count = 1;
            if (g_bindings[i].bind2 != -1) {
                Key_KeynumToStringBuf(g_bindings[i].bind2, bindings[1], 128);
                count = 2;
            }
            return count;
        }
    }
    strcpy(bindings[0], "KEY_UNBOUND");
    return 0;
}

int GetKeyBindingLocalizedString(const char *command, char *keys)
{
    char bindings[2][128];
    const char *translated;
    int bindCount = GetKeyBindings(command, bindings);

    if (bindCount) {
        translated = SEH_StringEd_GetString(bindings[0]);
        if (translated)
            I_strncpyz(keys, translated, 0x100);
        else
            I_strncpyz(keys, bindings[0], 0x100);

        if (bindCount > 1) {
            I_strncat(keys, 0x100, va(" %s ", UI_SafeTranslateString("KEY_OR")));
            translated = SEH_StringEd_GetString(bindings[1]);
            if (translated)
                I_strncat(keys, 0x100, translated);
            else
                I_strncat(keys, 0x100, bindings[1]);
        }
    } else {
        I_strncpyz(keys, UI_SafeTranslateString("KEY_UNBOUND"), 0x100);
    }

    return bindCount;
}

void Item_Slider_Paint(displayContextDef_t *dc, itemDef_t *item)
{
    vec4_t newColor;
    vec4_t lowLight;
    float x;
    float y;
    float value;
    menuDef_t *parent;

    parent = item->parent;
    value = item->dvar ? (float)atof(Dvar_GetVariantString(item->dvar)) : 0.0f;

    if (Window_HasFocus(&item->window)) {
        lowLight[0] = parent->focusColor[0] * 0.8f;
        lowLight[1] = parent->focusColor[1] * 0.8f;
        lowLight[2] = parent->focusColor[2] * 0.8f;
        lowLight[3] = parent->focusColor[3] * 0.8f;
        LerpColor(parent->focusColor, lowLight, newColor, (sin((float)(dc->realTime / 75)) + 1.0f) * 0.5f);
    } else {
        memcpy(newColor, item->window.foreColor, sizeof(vec4_t));
    }

    y = item->window.rect[0].y;
    if (item->text) {
        Item_Text_Paint(dc, item);
        x = item->textRect[0].x + item->textRect[0].w + 8.0f;
    } else {
        x = item->window.rect[0].x;
    }
    UI_DrawHandlePic(x, y, 96.0f, 16.0f, item->window.rect[0].horzAlign, item->window.rect[0].vertAlign,
                     newColor, sharedUiInfo.assets.sliderBar);

    x = Item_Slider_ThumbPosition(item);
    UI_DrawHandlePic(x - 5.0f, y - 2.0f, 10.0f, 20.0f, item->window.rect[0].horzAlign, item->window.rect[0].vertAlign,
                     newColor, sharedUiInfo.assets.sliderThumb);
}

void Item_Bind_Paint(displayContextDef_t *dc, itemDef_t *item)
{
    vec4_t newColor;
    vec4_t lowLight;
    char nameBind[128];
    float value;
    int maxChars;
    menuDef_t *parent;
    editFieldDef_t *editPtr;

    parent = item->parent;
    editPtr = Item_GetEditFieldDef(item);
    maxChars = 0x7fffffff;
    if (editPtr && editPtr->maxPaintChars)
        maxChars = editPtr->maxPaintChars;

    value = item->dvar ? (float)atof(Dvar_GetVariantString(item->dvar)) : 0.0f;

    if (Window_HasFocus(&item->window)) {
        if (g_bindItem == item) {
            lowLight[0] = 0.8f;
            lowLight[1] = 0.0f;
            lowLight[2] = 0.0f;
            lowLight[3] = 0.8f;
        } else {
            lowLight[0] = parent->focusColor[0] * 0.8f;
            lowLight[1] = parent->focusColor[1] * 0.8f;
            lowLight[2] = parent->focusColor[2] * 0.8f;
            lowLight[3] = parent->focusColor[3] * 0.8f;
        }
        LerpColor(parent->focusColor, lowLight, newColor, (sin((float)(dc->realTime / 75)) + 1.0f) * 0.5f);
    } else {
        memcpy(newColor, item->window.foreColor, sizeof(vec4_t));
    }

    if (item->text) {
        Item_Text_Paint(dc, item);
        BindingFromName(item->dvar, nameBind);
        UI_DrawText(nameBind, maxChars, UI_GetFontHandle(item->fontEnum, item->textscale),
                    item->textRect[0].x + item->textRect[0].w + 8.0f, item->textRect[0].y,
                    item->window.rect[0].horzAlign, item->window.rect[0].vertAlign,
                    item->textscale, newColor, item->textStyle);
    } else {
        UI_DrawText("FIXME", maxChars, UI_GetFontHandle(item->fontEnum, item->textscale),
                    item->textRect[0].x, item->textRect[0].y,
                    item->window.rect[0].horzAlign, item->window.rect[0].vertAlign,
                    item->textscale, newColor, item->textStyle);
    }
}

qboolean Display_KeyBindPending(void)
{
    return g_waitingForKey;
}

qboolean Item_Bind_HandleKey(displayContextDef_t *dc, itemDef_t *item, int key, qboolean down)
{
    int id;
    int i;

    if (!g_waitingForKey) {
        if (down && (key == 13 || key == 200 && Item_ContainsMouse(dc, item))) {
            g_waitingForKey = 1;
            g_bindItem = item;
            return 1;
        }
        return 0;
    }

    if (!g_bindItem)
        return 0;

    if (key & 0x400)
        return 1;

    switch (key) {
    case 27:
        g_waitingForKey = 0;
        return 1;
    case '`':
        return 1;
    case 127:
        id = BindingIDFromName(item->dvar);
        if (id != -1)
            key = -1;
        break;
    }

    if (key != -1) {
        for (i = 0; i < 56; i++) {
            if (g_bindings[i].bind2 == key)
                g_bindings[i].bind2 = -1;
            if (g_bindings[i].bind1 == key) {
                g_bindings[i].bind1 = g_bindings[i].bind2;
                g_bindings[i].bind2 = -1;
            }
        }
    }

    id = BindingIDFromName(item->dvar);
    if (id != -1) {
        if (key == -1) {
            if (g_bindings[id].bind1 != -1) {
                Key_SetBinding(g_bindings[id].bind1, "");
                g_bindings[id].bind1 = -1;
            }
            if (g_bindings[id].bind2 != -1) {
                Key_SetBinding(g_bindings[id].bind2, "");
                g_bindings[id].bind2 = -1;
            }
        } else if (g_bindings[id].bind1 == -1) {
            g_bindings[id].bind1 = key;
        } else if (g_bindings[id].bind1 != key && g_bindings[id].bind2 == -1) {
            g_bindings[id].bind2 = key;
        } else {
            Key_SetBinding(g_bindings[id].bind1, "");
            Key_SetBinding(g_bindings[id].bind2, "");
            g_bindings[id].bind1 = key;
            g_bindings[id].bind2 = -1;
        }
    }

    Controls_SetConfig(1);
    g_waitingForKey = 0;
    return 1;
}

void Item_ListBox_Paint(displayContextDef_t *dc, itemDef_t *item)
{
    byte *it = (byte *)item;
    byte *listPtr;
    int count;
    float size, thumb;
    int startPos;
    int i, j;
    int numColumns;
    MaterialHandle optionalImage;
    const char *text;
    float x, y;
    byte *uiInfo;

    listPtr = (byte *)Item_GetListBoxDef(item);
    if (!listPtr)
        return;

    count = UI_FeederCount(((itemDef_t *)it)->special);

    if (Window_IsHorizontal((Window *)it)) {
        x = (((rectDef_t*)(it))->x) + 1.0f;
        y = (((rectDef_t*)(it))->y) + (((rectDef_t*)(it))->h) - 16.0f - 1.0f;

        uiInfo = (byte *)((void *)&sharedUiInfo);

        UI_DrawHandlePic(
            x, y, 16.0f, 16.0f,
            ((rectDef_t *)it)->horzAlign, ((rectDef_t *)it)->vertAlign, 0, ((sharedUiInfo_t*)uiInfo)->assets.scrollBarArrowLeft);

        x += 16.0f - 1.0f;

        size = (((rectDef_t*)(it))->w) - 32.0f;
        UI_DrawHandlePic(
            x, y, size + 1.0f, 16.0f,
            ((rectDef_t *)it)->horzAlign, ((rectDef_t *)it)->vertAlign, 0, ((sharedUiInfo_t*)uiInfo)->assets.scrollBar);

        x += size - 1.0f;
        UI_DrawHandlePic(
            x, y, 16.0f, 16.0f,
            ((rectDef_t *)it)->horzAlign, ((rectDef_t *)it)->vertAlign, 0, ((sharedUiInfo_t *)uiInfo)->assets.scrollBarArrowRight);

        thumb = (float)Item_ListBox_ThumbDrawPosition(dc, item);
        if (thumb > x - 16.0f - 1.0f)
            thumb = x - 16.0f - 1.0f;
        UI_DrawHandlePic(
            thumb, y, 16.0f, 16.0f,
            ((rectDef_t *)it)->horzAlign, ((rectDef_t *)it)->vertAlign, 0, ((sharedUiInfo_t*)uiInfo)->assets.scrollBarThumb);

        ListBox_SetEndPos( (listBoxDef_t *)((itemDef_t *)((void *)listPtr)), *(int *)listPtr);

        size = (((rectDef_t*)(it))->w) - 2.0f;

        if (((listBoxDef_t *)listPtr)->elementStyle != 1)
            return;

        x = (((rectDef_t*)(it))->x) + 1.0f;
        y = (((rectDef_t*)(it))->y) + 1.0f;
        for (i = *(int *)listPtr; i < count; i++) {

            MaterialHandle img = UI_FeederItemImage(((itemDef_t *)it)->special, i);
            if (img) {
                UI_DrawHandlePic(
                    x + 1.0f, y + 1.0f,
                    (((listBoxDef_s*)(listPtr))->elementWidth) - 2.0f,
                    (((listBoxDef_s*)(listPtr))->elementHeight) - 2.0f,
                    ((rectDef_t *)it)->horzAlign, ((rectDef_t *)it)->vertAlign, 0, img);
            }

            if (i == ((itemDef_t *)it)->cursorPos[0]) {
                UI_DrawRect(x, y,
                            (((listBoxDef_s*)(listPtr))->elementWidth) - 1.0f,
                            (((listBoxDef_s*)(listPtr))->elementHeight) - 1.0f,
                            ((rectDef_t *)it)->horzAlign, ((rectDef_t *)it)->vertAlign,
                            ((itemDef_t *)it)->window.borderSize,
                            ((itemDef_t *)it)->window.borderColor);
            }

            size -= (((listBoxDef_s*)(listPtr))->elementWidth);
            if (size < (((listBoxDef_s*)(listPtr))->elementWidth)) {
                (((listBoxDef_s*)(listPtr))->drawPadding) = (int)size;
                break;
            }

            x += (((listBoxDef_s*)(listPtr))->elementWidth);
            ListBox_SetEndPos( (listBoxDef_t *)((itemDef_t *)((void *)listPtr)), (((listBoxDef_s*)(listPtr))->endPos[0]) + 1);
        }
        return;
    }

    UI_OverrideCursorPos(item);

    numColumns = ((listBoxDef_t *)listPtr)->noScrollBars;

    if (numColumns == 0) {
        x = (((rectDef_t*)(it))->w) + (((rectDef_t*)(it))->x) - 16.0f - 1.0f;
        y = (((rectDef_t*)(it))->y) + 1.0f;

        uiInfo = (byte *)((void *)&sharedUiInfo);

        UI_DrawHandlePic(
            x, y, 16.0f, 16.0f,
            ((rectDef_t *)it)->horzAlign, ((rectDef_t *)it)->vertAlign, 0, *(MaterialHandle *)uiInfo);

        y += 16.0f - 1.0f;

        ListBox_SetEndPos( (listBoxDef_t *)((itemDef_t *)((void *)listPtr)), *(int *)listPtr);

        size = (((rectDef_t*)(it))->h) - 32.0f;
        UI_DrawHandlePic(
            x, y, 16.0f, size + 1.0f,
            ((rectDef_t *)it)->horzAlign, ((rectDef_t *)it)->vertAlign, 0, ((sharedUiInfo_t*)uiInfo)->assets.scrollBar);

        y += size - 1.0f;
        UI_DrawHandlePic(
            x, y, 16.0f, 16.0f,
            ((rectDef_t *)it)->horzAlign, ((rectDef_t *)it)->vertAlign, 0, ((sharedUiInfo_t*)uiInfo)->assets.scrollBarArrowDown);

        thumb = (float)Item_ListBox_ThumbDrawPosition(dc, item);
        if (thumb > y - 16.0f - 1.0f)
            thumb = y - 16.0f - 1.0f;
        UI_DrawHandlePic(
            x, thumb, 16.0f, 16.0f,
            ((rectDef_t *)it)->horzAlign, ((rectDef_t *)it)->vertAlign, 0, ((sharedUiInfo_t*)uiInfo)->assets.scrollBarThumb);
    }

    size = (((rectDef_t*)(it))->h) - 2.0f;
    x = (((rectDef_t*)(it))->x) + 1.0f;
    y = (((rectDef_t*)(it))->y) + 1.0f;

    if (((listBoxDef_t *)listPtr)->elementStyle == 1) {

        for (i = *(int *)listPtr; i < count; i++) {

            MaterialHandle img2 = UI_FeederItemImage(((itemDef_t *)it)->special, i);
            if (img2) {
                UI_DrawHandlePic(
                    x + 1.0f, y + 1.0f,
                    (((listBoxDef_s*)(listPtr))->elementWidth) - 2.0f,
                    (((listBoxDef_s*)(listPtr))->elementHeight) - 2.0f,
                    ((rectDef_t *)it)->horzAlign, ((rectDef_t *)it)->vertAlign, 0, img2);
            }

            if (i == ((itemDef_t *)it)->cursorPos[0]) {
                UI_DrawRect(x, y,
                            (((listBoxDef_s*)(listPtr))->elementWidth) - 1.0f,
                            (((listBoxDef_s*)(listPtr))->elementHeight) - 1.0f,
                            ((rectDef_t *)it)->horzAlign, ((rectDef_t *)it)->vertAlign,
                            ((itemDef_t *)it)->window.borderSize,
                            ((itemDef_t *)it)->window.borderColor);
            }

            ListBox_SetEndPos( (listBoxDef_t *)((itemDef_t *)((void *)listPtr)), (((listBoxDef_s*)(listPtr))->endPos[0]) + 1);
            size -= (((listBoxDef_s*)(listPtr))->elementWidth);
            if (size < (((listBoxDef_s*)(listPtr))->elementHeight)) {
                (((listBoxDef_s*)(listPtr))->drawPadding) = (int)((((listBoxDef_s*)(listPtr))->elementHeight) - size);
                return;
            }
            y += (((listBoxDef_s*)(listPtr))->elementHeight);
        }
        return;
    }

    for (i = *(int *)listPtr; i < count; i++) {
        if ((((listBoxDef_s*)(listPtr))->numColumns) > 0) {

            for (j = 0; j < (((listBoxDef_s*)(listPtr))->numColumns); j++) {

                text = UI_FeederItemText(((itemDef_t *)it)->special, i, j, &optionalImage);

                if (optionalImage) {

                    float imgSize = (float)((((listBoxDef_s*)(listPtr))->columnInfo[j].width));
                    UI_DrawHandlePic(
                        (float)((((listBoxDef_s*)(listPtr))->columnInfo[j].pos)) + x + 2.0f,
                        y + 3.0f,
                        imgSize, imgSize,
                        ((rectDef_t *)it)->horzAlign, ((rectDef_t *)it)->vertAlign, 0, optionalImage);
                } else if (text) {

                    UI_DrawText(text, (((listBoxDef_s*)(listPtr))->columnInfo[j].maxChars),
                                UI_GetFontHandle(((itemDef_t *)it)->fontEnum, ((itemDef_t *)it)->textscale),
                                (float)((((listBoxDef_s*)(listPtr))->columnInfo[j].pos)) + (((itemDef_s*)(it))->textalignx) + x + 4.0f,
                                (((itemDef_s*)(it))->textaligny) + (((listBoxDef_s*)(listPtr))->elementHeight) + y,
                                ((rectDef_t *)it)->horzAlign, ((rectDef_t *)it)->vertAlign,
                                ((itemDef_t *)it)->textscale,
                                ((itemDef_t *)it)->window.foreColor,
                                (((itemDef_s*)(it))->textStyle));
                }
            }
        } else {

            text = UI_FeederItemText(((itemDef_t *)it)->special, i, 0, &optionalImage);

            if (optionalImage) {

                UI_DrawHandlePic(
                    x + 2.0f,
                    y + 2.0f,
                    (((listBoxDef_s*)(listPtr))->elementWidth) - 4.0f,
                    (((listBoxDef_s*)(listPtr))->elementHeight) - 4.0f,
                    ((rectDef_t *)it)->horzAlign, ((rectDef_t *)it)->vertAlign, 0, optionalImage);
            } else if (text) {

                UI_DrawText(text, 0x7fffffff, UI_GetFontHandle(((itemDef_t *)it)->fontEnum, ((itemDef_t *)it)->textscale),
                            x + 4.0f, y + (((listBoxDef_s*)(listPtr))->elementHeight),
                            ((rectDef_t *)it)->horzAlign, ((rectDef_t *)it)->vertAlign,
                            ((itemDef_t *)it)->textscale,
                            ((itemDef_t *)it)->window.foreColor,
                            (((itemDef_s*)(it))->textStyle));
            }
        }

        if (i == ((itemDef_t *)it)->cursorPos[0]) {
            UI_FillRect(x + 2.0f, y + 2.0f,
                        (((rectDef_t*)(it))->w) - 16.0f - 4.0f,
                        (((listBoxDef_s*)(listPtr))->elementHeight),
                        ((rectDef_t *)it)->horzAlign, ((rectDef_t *)it)->vertAlign,
                        (float *)(((char *)it + offsetof(itemDef_t, window.outlineColor[0]))));
        }

        size -= (((listBoxDef_s*)(listPtr))->elementHeight);
        if (size < (((listBoxDef_s*)(listPtr))->elementHeight)) {
            (((listBoxDef_s*)(listPtr))->drawPadding) = (int)((((listBoxDef_s*)(listPtr))->elementHeight) - size);
            return;
        }

        ListBox_SetEndPos( (listBoxDef_t *)((itemDef_t *)((void *)listPtr)), (((listBoxDef_s*)(listPtr))->endPos[0]) + 1);
        y += (((listBoxDef_s*)(listPtr))->elementHeight);
    }
}

// retail 0x53bf60, unreferenced; Q3 Menu_GetFocusedItem (SUPPORTED)
itemDef_t *Menu_GetFocusedItem(menuDef_t *menu)
{
    int i;

    if (menu) {
        for (i = 0; i < menu->itemCount; i++) {
            if (Window_HasFocus(&menu->items[i]->window))
                return menu->items[i];
        }
    }
    return NULL;
}

menuDef_t *Menu_GetFocused(displayContextDef_t *dc)
{
    int i;
    for (i = dc->openMenuCount - 1; i >= 0; i--) {
        if (Window_HasFocus(&dc->menuStack[i]->window) && Window_IsVisible(&dc->menuStack[i]->window))
            return dc->menuStack[i];
    }
    return NULL;
}

// retail 0x53c030, q3 shape; no callers in 1.0
void Menu_ScrollFeeder(displayContextDef_t *dc, menuDef_t *menu, int feeder, qboolean down)
{
    if (menu) {
        int i;
        for (i = 0; i < menu->itemCount; i++) {
            if (menu->items[i]->special == feeder) {
                Item_ListBox_HandleKey(dc, menu->items[i], down ? 155 : 154, 1, 1);
                return;
            }
        }
    }
}

void Menu_SetFeederSelection(displayContextDef_t *dc, menuDef_t *menu, int feeder, int index, const char *name)
{
    listBoxDef_t *listPtr;
    int i;

    if (!menu) {
        if (!name)
            menu = Menu_GetFocused(dc);
        else
            menu = Menus_FindByName(dc, name);
        if (!menu)
            return;
    }

    for (i = 0; i < menu->itemCount; i++) {
        if (menu->items[i]->special != (float)feeder)
            continue;

        listPtr = Item_GetListBoxDef(menu->items[i]);
        Item_SetCursorPos(menu->items[i], index);
        UI_FeederSelection(menu->items[i]->special, menu->items[i]->cursorPos[0]);

        if (!listPtr)
            continue;

        if (index == 0) {
            ListBox_SetCursorPos(listPtr, 0);
            ListBox_SetStartPos(listPtr, 0);
        } else if (index < listPtr->startPos[0]) {
            ListBox_SetStartPos(listPtr, index);
        }
    }
}

qboolean Menus_AnyFullScreenVisible(displayContextDef_t *dc)
{
    int i;
    for (i = dc->openMenuCount - 1; i >= 0; i--) {
        if (Window_IsVisible(&dc->menuStack[i]->window) && dc->menuStack[i]->fullScreen)
            return 1;
    }
    return 0;
}

qboolean Menu_HandleMouseMove(displayContextDef_t *dc, menuDef_t *menu, float x, float y)
{
    int i;
    int pass;
    qboolean focusSet;
    itemDef_t *overItem;
    itemDef_t *focusItem;

    focusSet = 0;
    focusItem = NULL;
    if (!menu)
        return 0;
    if (!(menu->window.dynamicFlags[0] & 0x4004))
        return 0;
    if (itemCapture)
        return 0;
    if (g_waitingForKey || g_editingField)
        return 0;

    for (pass = 0; pass < 2; pass++) {
        for (i = menu->itemCount - 1; i >= 0; i--) {
            if (!(menu->items[i]->window.dynamicFlags[0] & 0x4004))
                continue;
            if ((menu->items[i]->dvarFlags & 3) && !Item_EnableShowViaDvar(menu->items[i], 1))
                continue;
            if ((menu->items[i]->dvarFlags & 0xc) && !Item_EnableShowViaDvar(menu->items[i], 4))
                continue;
            overItem = menu->items[i];
            if (Window_HasFocus(&overItem->window) && !focusItem)
                focusItem = overItem;
            if (Rect_ContainsPoint(&overItem->window.rect[0], x, y)) {
                if (pass == 1) {
                    if (overItem->type == 0 && overItem->text) {
                        if (!Rect_ContainsPoint(Item_CorrectedTextRect(overItem), x, y))
                            continue;
                    }
                    if (IsVisible(overItem->window.dynamicFlags[0])) {
                        Item_MouseEnter(dc, overItem, x, y);
                        if (!focusSet) {
                            focusSet = Item_SetFocus(dc, overItem, x, y);
                            if (focusSet)
                                focusItem = overItem;
                        }
                    }
                }
            } else if (menu->items[i]->window.dynamicFlags[0] & 1) {
                Item_MouseLeave(dc, overItem);
                Item_SetMouseOver(menu->items[i], 0);
            }
        }
    }
    if (!focusSet) {
        if (focusItem && !Rect_ContainsPoint(&focusItem->window.rect[0], x, y))
            Menu_ClearFocus(dc, menu);
        return 0;
    }
    return 1;
}

void Menu_Paint(displayContextDef_t *dc, menuDef_t *menu, qboolean forcePaint)
{
    int i;

    if (!menu)
        return;
    if (!Window_IsVisible(&menu->window) && !forcePaint)
        return;
    if (menu->window.ownerDrawFlags && !UI_OwnerDrawVisible(menu->window.ownerDrawFlags))
        return;

    if (menu->soundName)
        UI_PlayLocalSoundAliasByName(menu->soundName);

    if (menu->blurRadius != 0.0f)
        dc->blurRadiusOut = sqrt(menu->blurRadius * menu->blurRadius + dc->blurRadiusOut * dc->blurRadiusOut);

    if (forcePaint) {
        Window_AddDynamicFlags(&menu->window, 0x4000);
        Menus_AddToStack(dc, menu);
    }

    if (menu->fullScreen && menu->window.background)
        UI_DrawHandlePic(0.0f, 0.0f, 640.0f, 480.0f, menu->window.rect[0].horzAlign, menu->window.rect[0].vertAlign, NULL, menu->window.background);

    Window_Paint(dc, (float (*)[4][32])menu, menu->fadeAmount, menu->fadeInAmount, menu->fadeClamp, (float)menu->fadeCycle);

    for (i = 0; i < menu->itemCount; i++)
        Item_Paint(dc, menu->items[i]);

    if (debugMode) {
        vec4_t color;
        color[0] = color[2] = color[3] = 1.0f;
        color[1] = 0.0f;
        UI_DrawRect(menu->window.rect[0].x, menu->window.rect[0].y, menu->window.rect[0].w, menu->window.rect[0].h,
                    menu->window.rect[0].horzAlign, menu->window.rect[0].vertAlign, 1.0f, color);
    }
}

int Menu_Count(displayContextDef_t *dc)
{
    return dc->menuCount;
}

qboolean Display_MouseMove(displayContextDef_t *dc, void *p, int x, int y)
{
    int i;
    menuDef_t *menu;
    UiRectangle r;

    menu = (menuDef_t *)p;
    if (!menu) {
        menu = Menu_GetFocused(dc);
        if (menu && (menu->window.staticFlags & 0x1000000)) {
            Menu_HandleMouseMove(dc, menu, (float)x, (float)y);
            return 1;
        }
        for (i = dc->openMenuCount - 1; i >= 0; i--) {
            if (Menu_HandleMouseMove(dc, dc->menuStack[i], (float)x, (float)y))
                break;
        }
    } else {
        r = menu->window.rect[0];
        r.x += (float)x;
        r.y += (float)y;
        Window_SetRect(&menu->window, &r);
        Menu_UpdatePosition(menu);
    }
    return 1;
}

static qboolean Menu_OverActiveItem(menuDef_t *menu, float x, float y)
{
    int i;
    itemDef_t *overItem;

    if (menu && (menu->window.dynamicFlags[0] & 0x4004)) {
        if (Rect_ContainsPoint(&menu->window.rect[0], x, y)) {
            for (i = 0; i < menu->itemCount; i++) {
                if (!(menu->items[i]->window.dynamicFlags[0] & 0x4004))
                    continue;
                if (menu->items[i]->window.staticFlags & 0x100000)
                    continue;
                if (Rect_ContainsPoint(&menu->items[i]->window.rect[0], x, y)) {
                    overItem = menu->items[i];
                    if (overItem->type == 0 && overItem->text) {
                        if (Rect_ContainsPoint(Item_CorrectedTextRect(overItem), x, y))
                            return 1;
                        continue;
                    }
                    return 1;
                }
            }
        }
    }
    return 0;
}

// retail 0x53cdd0, unreferenced; Kisak UI_AddMenu (SUPPORTED)
void UI_AddMenu(displayContextDef_t *dc, menuDef_t *menu)
{
    if (dc->menuCount >= 128)
        Com_Error(ERR_DROP, "UI_AddMenu: Maximum number of menus %d exceeded.", 128);
    dc->Menus[dc->menuCount] = menu;
    dc->menuCount++;
}

void UI_AddMenuList(displayContextDef_t *dc, MenuList *menuList)
{
    int i;

    /* was raw x86 offsets: menus ptr @+4 read as 4-byte (truncated), Menus[] stride *4 */
    if (!menuList)
        return;

    for (i = 0; i < menuList->menuCount; i++) {
        menuDef_t *menu = menuList->menus[i];
        if (dc->menuCount >= 128)
            Com_Error(ERR_DROP, "UI_AddMenu: Maximum number of menus %d exceeded.", 128);
        dc->Menus[dc->menuCount] = menu;
        dc->menuCount++;
    }
}


#if COD2_TRACE
void diag_item_paint_enter(void *item)
{
    (void)item;
}
void diag_item_paint_skip(void *item, int reason)
{
    (void)item;
    (void)reason;
}
void diag_item_paint_draw(void *item)
{
    (void)item;
}
#endif



void Script_Show(displayContextDef_t *dc, itemDef_t *item, const char **args)
{
    char name[1024];
    (void)dc;
    if (String_Parse(args, name, 1024)) {
        Menu_ShowItemByName(item->parent, name, 1);
    }
}

void Script_Hide(displayContextDef_t *dc, itemDef_t *item, const char **args)
{
    char name[1024];
    (void)dc;
    if (String_Parse(args, name, 1024)) {
        Menu_ShowItemByName(item->parent, name, 0);
    }
}

void Script_FadeIn(displayContextDef_t *dc, itemDef_t *item, const char **args)
{
    char name[1024];
    (void)dc;
    if (String_Parse(args, name, 1024)) {
        Menu_FadeItemByName(item->parent, name, 0);
    }
}

void Script_FadeOut(displayContextDef_t *dc, itemDef_t *item, const char **args)
{
    char name[1024];
    (void)dc;
    if (String_Parse(args, name, 1024)) {
        Menu_FadeItemByName(item->parent, name, 1);
    }
}

static void Script_ConditionalExecHandler(int execWhen, Bool (*shouldExec)(const char *, const char *), const char **args)
{
    Script_ConditionalExecHandler_impl(execWhen, NULL, args, shouldExec);
}

void Script_GetAutoUpdate(displayContextDef_t *dc, itemDef_t *item, const char **args)
{
    CL_GetAutoUpdate();
}

UiRectangle *Window_Getter_533870(Window *w) { return &w->rectClient[0]; }
// retail 0x52b230..0x52b270, unreferenced; names unknown
int Window_Getter_52b230(Window *w) { return w->dynamicFlags[0]; }
int Item_Getter_52b240(itemDef_t *item) { return item->cursorPos[0]; }
int ListBox_Getter_52b250(listBoxDef_s *listPtr) { return listPtr->cursorPos[0]; }
int ListBox_Getter_52b260(listBoxDef_s *listPtr) { return listPtr->startPos[0]; }
int ListBox_Getter_52b270(listBoxDef_s *listPtr) { return listPtr->endPos[0]; }

bool Window_IsVisible(Window *w)
{
    return ((unsigned int)w->dynamicFlags[0] >> 2) & 1;
}

// retail 0x5348c0, unreferenced twin of Menus_MenuIsInStack; name unknown
qboolean Menus_MenuIsInStack_5348c0(displayContextDef_t *dc, menuDef_t *menu)
{
    int i;

    for (i = dc->openMenuCount - 1; i >= 0; i--) {
        if (dc->menuStack[i] == menu)
            return 1;
    }
    return 0;
}

static qboolean Menus_MenuIsInStack(displayContextDef_t *dc, menuDef_t *menu)
{
    int i;

    for (i = dc->openMenuCount - 1; i >= 0; i--) {
        if (dc->menuStack[i] == menu)
            return 1;
    }
    return 0;
}


void Item_Text_AutoWrapped_Paint(itemDef_t *item, const char *textPtr, vec_t *color)
{
    const char *p;
    const char *newLinePtr;
    char buff[1024];
    int height, len, textWidth, newLine, newLineWidth;
    float y;
    FontHandle font;
    int iTargetLineWidth;
    rectDef_t newRect;
    rectDef_t *textRect;

    newLinePtr = NULL;
    font = UI_GetFontHandle(item->fontEnum, item->textscale);
    height = UI_TextHeight(font, item->textscale);
    textWidth = UI_TextWidth(textPtr, 0, font, item->textscale);
    if (item->window.rect[0].w != 0.0f && (float)textWidth > item->window.rect[0].w)
        iTargetLineWidth = (int)((float)textWidth / (float)(int)ceil((double)(textWidth / item->window.rect[0].w)));
    else
        iTargetLineWidth = (int)item->window.rect[0].w;
    y = item->textaligny;
    len = 0;
    buff[0] = '\0';
    newLine = 0;
    newLineWidth = 0;
    p = textPtr;
    while (p)
    {
        if (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\0')
        {
            newLine = len;
            newLinePtr = p + 1;
            newLineWidth = textWidth;
        }
        textWidth = UI_TextWidth(buff, 0, font, item->textscale);
        if ((newLine && textWidth > item->window.rect[0].w) || *p == '\n' || *p == '\0' || (*p == ' ' && textWidth > iTargetLineWidth))
        {
            if (len)
            {
                newRect = item->textRect[0];
                if (item->textalignment == 1)
                    newRect.x = item->textalignx - (float)(newLineWidth / 2);
                else if (item->textalignment == 2)
                    newRect.x = item->textalignx - (float)newLineWidth;
                else
                    newRect.x = item->textalignx;
                newRect.y = y;
                ToWindowCoords(&newRect.x, &newRect.y, &item->window);
                textRect = &item->textRect[0];
                *textRect = newRect;
                buff[newLine] = '\0';
                UI_DrawText(buff, 0x7fffffff, font, textRect->x, textRect->y, item->window.rect[0].horzAlign, item->window.rect[0].vertAlign, item->textscale, color, item->textStyle);
            }
            if (*p == '\0')
                break;
            y += height + 5;
            p = newLinePtr;
            len = 0;
            newLine = 0;
            newLineWidth = 0;
            continue;
        }
        buff[len++] = *p++;
        if (buff[len - 1] == '\r')
            buff[len - 1] = ' ';
        buff[len] = '\0';
    }
}

// retail 0x533bd0
void Window_Paint(displayContextDef_t *dc, float (*w_param)[4][32], float fadeAmount, float fadeInAmount, float fadeClamp, float fadeCycle)
{
    windowDef_t *w = (windowDef_t *)w_param;
    rectDef_t fillRect;
    float color[4];
    int flags;

    fillRect = *(rectDef_t *)&w->rect[0];

    if (debugMode) {
        color[0] = color[1] = color[2] = color[3] = 1.0f;
        UI_DrawRect(w->rect[0].x, w->rect[0].y, w->rect[0].w, w->rect[0].h, w->rect[0].horzAlign, w->rect[0].vertAlign, 1.0f, color);
    }

    if (!w->style && !w->border)
        return;

    if (w->border) {
        fillRect.x += w->borderSize;
        fillRect.y += w->borderSize;
        fillRect.w -= w->borderSize + 1.0f;
        fillRect.h -= w->borderSize + 1.0f;
    }

    switch (w->style) {
    case 1:
        if (w->background) {
            flags = w->dynamicFlags[0];
            Fade(&flags, &w->backColor[3], fadeClamp, &w->nextTime, (int)fadeCycle, 1, fadeAmount, fadeInAmount, dc);
            Window_SetDynamicFlags((Window *)w, flags);
            UI_DrawHandlePic(fillRect.x, fillRect.y, fillRect.w, fillRect.h, w->rect[0].horzAlign, w->rect[0].vertAlign, w->backColor, w->background);
        } else {
            UI_FillRect(fillRect.x, fillRect.y, fillRect.w, fillRect.h, w->rect[0].horzAlign, w->rect[0].vertAlign, w->backColor);
        }
        break;
    case 3:
        UI_DrawHandlePic(fillRect.x, fillRect.y, fillRect.w, fillRect.h, w->rect[0].horzAlign, w->rect[0].vertAlign,
                         (w->dynamicFlags[0] & 0x10000) ? w->foreColor : NULL, w->background);
        break;
    case 7:
        UI_DrawLoadBar(fillRect.x, fillRect.y, fillRect.w, fillRect.h, w->rect[0].horzAlign, w->rect[0].vertAlign,
                       (w->dynamicFlags[0] & 0x10000) ? w->foreColor : NULL, w->background);
        break;
    case 5:
        if (w->cinematic == -1) {
            w->cinematic = CIN_PlayCinematic(w->cinematicName, (int)fillRect.x, (int)fillRect.y, (int)fillRect.w, (int)fillRect.h, 10);
            if (w->cinematic == -1)
                w->cinematic = -2;
        }
        if (w->cinematic >= 0) {
            CIN_RunCinematic(w->cinematic);
            CIN_SetExtents(w->cinematic, (int)fillRect.x, (int)fillRect.y, (int)fillRect.w, (int)fillRect.h);
            CIN_DrawCinematic(w->cinematic);
        }
        break;
    case 6:
        if (w->background)
            UI_DrawHandlePic(fillRect.x, fillRect.y, fillRect.w, fillRect.h, w->rect[0].horzAlign, w->rect[0].vertAlign,
                             (w->dynamicFlags[0] & 0x10000) ? w->foreColor : NULL, w->background);
        break;
    }

    if (w->border == 1) {
        if (w->style == 4) {
            if (w->borderColor[0] > 0.0f) {
                color[0] = 1.0f;
                color[1] = color[2] = 0.5f;
            } else {
                color[2] = 1.0f;
                color[0] = color[1] = 0.5f;
            }
            color[3] = 1.0f;
            UI_DrawRect(w->rect[0].x, w->rect[0].y, w->rect[0].w, w->rect[0].h, w->rect[0].horzAlign, w->rect[0].vertAlign, w->borderSize, color);
        } else {
            UI_DrawRect(w->rect[0].x, w->rect[0].y, w->rect[0].w, w->rect[0].h, w->rect[0].horzAlign, w->rect[0].vertAlign, w->borderSize, w->borderColor);
        }
    } else if (w->border == 2) {
        UI_DrawTopBottom(w->rect[0].x, w->rect[0].y, w->rect[0].w, w->rect[0].h, w->rect[0].horzAlign, w->rect[0].vertAlign, w->borderSize, w->borderColor);
    } else if (w->border == 3) {
        UI_DrawSides(w->rect[0].x, w->rect[0].y, w->rect[0].w, w->rect[0].h, w->rect[0].horzAlign, w->rect[0].vertAlign, w->borderSize, w->borderColor);
    }
}

itemDef_t *Menu_SetNextCursorItem(displayContextDef_t *dc, menuDef_t *menu)
{
    qboolean wrapped;
    int oldCursor;
    itemDef_t *item;

    wrapped = 0;
    oldCursor = menu->cursorItem[0];
    if (menu->cursorItem[0] == -1) {
        menu->cursorItem[0] = 0;
        wrapped = 1;
    }
    while (menu->cursorItem[0] < menu->itemCount) {
        menu->cursorItem[0]++;
        if (menu->cursorItem[0] >= menu->itemCount) {
            if (wrapped)
                return menu->items[oldCursor];
            wrapped = 1;
            menu->cursorItem[0] = 0;
        }
        item = menu->items[menu->cursorItem[0]];
        if (Item_SetFocus(dc, item, (float)dc->cursorx, (float)dc->cursory)) {
            item = menu->items[menu->cursorItem[0]];
            Menu_HandleMouseMove(dc, menu, item->window.rect[0].x + 1.0f, item->window.rect[0].y + 1.0f);
            return menu->items[menu->cursorItem[0]];
        }
    }
    menu->cursorItem[0] = oldCursor;
    return NULL;
}

#if COD2_TRACE
static const char *UI_TraceScriptItemParentName(itemDef_t *item)
{
    byte *it;
    byte *parent;
    const char *name;

    if (!item)
        return NULL;

    it = (byte *)item;
    parent = (byte *)item->parent;
    if (!parent)
        return NULL;

    name = ((menuDef_t *)parent)->window.name;
    if (!name)
        return NULL;

    if (!I_stricmp(name, "serverinfo_dm") ||
        !strncmp(name, "team_", 5) ||
        !I_stricmp(name, "team"))
        return name;

    return NULL;
}
#endif

void Item_OwnerDraw_Paint(displayContextDef_t *dc, itemDef_t *item)
{
    vec4_t color;
    vec4_t lowLight;
    menuDef_t *parent;
    int flags;

    if (!item)
        return;

    parent = item->parent;
    flags = item->window.dynamicFlags[0];
    Fade(&flags, &item->window.foreColor[3], parent->fadeClamp, &item->window.nextTime, parent->fadeCycle, 1, parent->fadeAmount, parent->fadeInAmount, dc);
    Window_SetDynamicFlags(&item->window, flags);

    memcpy(color, item->window.foreColor, sizeof(vec4_t));

    if (Window_HasFocus(&item->window)) {
        lowLight[0] = parent->focusColor[0] * 0.8f;
        lowLight[1] = parent->focusColor[1] * 0.8f;
        lowLight[2] = parent->focusColor[2] * 0.8f;
        lowLight[3] = parent->focusColor[3] * 0.8f;
        LerpColor(parent->focusColor, lowLight, color, (sin((float)(dc->realTime / 75)) + 1.0f) * 0.5f);
    } else if (item->textStyle == 1 && !((dc->realTime / 256) & 1)) {
        lowLight[0] = item->window.foreColor[0] * 0.8f;
        lowLight[1] = item->window.foreColor[1] * 0.8f;
        lowLight[2] = item->window.foreColor[2] * 0.8f;
        lowLight[3] = item->window.foreColor[3] * 0.8f;
        LerpColor(item->window.foreColor, lowLight, color, (sin((float)(dc->realTime / 75)) + 1.0f) * 0.5f);
    }

    if ((item->dvarFlags & 3) && !Item_EnableShowViaDvar(item, 1))
        memcpy(color, parent->disableColor, sizeof(vec4_t));

    if (item->text) {
        Item_Text_Paint(dc, item);
        if (item->text[0]) {
            UI_OwnerDraw(item->textRect[0].x + item->textRect[0].w + 8.0f, item->window.rect[0].y,
                         item->window.rect[0].w, item->window.rect[0].h,
                         item->window.rect[0].horzAlign, item->window.rect[0].vertAlign,
                         0.0f, item->textaligny, item->window.ownerDraw, item->window.ownerDrawFlags,
                         item->alignment, item->special, UI_GetFontHandle(item->fontEnum, item->textscale),
                         item->textscale, color, item->window.background, item->textStyle);
        } else {
            UI_OwnerDraw(item->textRect[0].x + item->textRect[0].w, item->window.rect[0].y,
                         item->window.rect[0].w, item->window.rect[0].h,
                         item->window.rect[0].horzAlign, item->window.rect[0].vertAlign,
                         0.0f, item->textaligny, item->window.ownerDraw, item->window.ownerDrawFlags,
                         item->alignment, item->special, UI_GetFontHandle(item->fontEnum, item->textscale),
                         item->textscale, color, item->window.background, item->textStyle);
        }
    } else {
        UI_OwnerDraw(item->window.rect[0].x, item->window.rect[0].y,
                     item->window.rect[0].w, item->window.rect[0].h,
                     item->window.rect[0].horzAlign, item->window.rect[0].vertAlign,
                     item->textalignx, item->textaligny, item->window.ownerDraw, item->window.ownerDrawFlags,
                     item->alignment, item->special, UI_GetFontHandle(item->fontEnum, item->textscale),
                     item->textscale, color, item->window.background, item->textStyle);
    }
}

qboolean Item_SetFocus(displayContextDef_t *dc, itemDef_t *item, float x, float y)
{
    rectDef_t r;
    const rectDef_t *textRect;
    itemDef_t *oldFocus;
    menuDef_t *parent;
    menuDef_t *focusedMenu;
    snd_alias_list_t *sfx;
    int i;
    int playSound;

    playSound = 0;
    sfx = sharedUiInfo.assets.itemFocusSound;
    if (!item || (item->window.staticFlags & 0x100000) || !Window_IsVisible(&item->window))
        return 0;
    if (Window_HasFocus(&item->window))
        return 1;
    parent = item->parent;
    if (parent && !Window_HasFocus(&parent->window)) {
        focusedMenu = Menu_GetFocused(dc);
        if (focusedMenu && Rect_ContainsPoint(&focusedMenu->window.rect[0], x, y)
            && Rect_ContainsPoint(&parent->window.rect[0], x, y))
            return 0;
    }
    if ((item->dvarFlags & 3) && !Item_EnableShowViaDvar(item, 1))
        return 0;
    if ((item->dvarFlags & 0xc) && !Item_EnableShowViaDvar(item, 4))
        return 0;
    oldFocus = Menu_ClearFocus(dc, item->parent);
    if (item->type == 0) {
        textRect = &item->textRect[0];
        r = *textRect;
        r.y -= r.h;
        r.horzAlign = textRect->horzAlign;
        r.vertAlign = textRect->vertAlign;
        if (Rect_ContainsPoint(&r, x, y)) {
            Window_AddDynamicFlags(&item->window, 2);
            if (item->focusSound)
                sfx = item->focusSound;
            playSound = 1;
        } else if (oldFocus) {
            Window_AddDynamicFlags(&oldFocus->window, 2);
            if (oldFocus->onFocus)
                Item_RunScript(dc, oldFocus, oldFocus->onFocus);
        }
    } else {
        Window_AddDynamicFlags(&item->window, 2);
        if (item->onFocus)
            Item_RunScript(dc, item, item->onFocus);
        if (item->focusSound)
            sfx = item->focusSound;
        playSound = 1;
    }
    if (playSound && sfx)
        UI_PlayLocalSoundAlias(sfx);
    for (i = 0; i < parent->itemCount; i++) {
        if (parent->items[i] == item) {
            parent->cursorItem[0] = i;
            return 1;
        }
    }
    return 1;
}

// retail 0x536760
static __declspec(noinline) qboolean Item_ListBox_HandleKey(displayContextDef_t *dc, itemDef_t *item, int key, qboolean down, qboolean force)
{
    listBoxDef_s *listPtr;
    int count;
    int max;
    int viewmax;
    int inRect;
    int flags;

    listPtr = Item_GetListBoxDef(item);
    if (!listPtr)
        return 0;
    count = UI_FeederCount(item->special);
    inRect = Rect_ContainsPoint(&item->window.rect[0], (float)dc->cursorx, (float)dc->cursory);
    flags = item->window.dynamicFlags[0];
    if (!force && (!inRect || !(flags & 2)))
        return 0;

    UI_OverrideCursorPos(item);
    max = Item_ListBox_MaxScroll(item);
    if (Window_IsHorizontal(&item->window)) {
        viewmax = (int)(item->window.rect[0].w / listPtr->elementWidth);
        if (key == 156 || key == 185) {
            if (!listPtr->notselectable) {
                listPtr->cursorPos[0]--;
                if (listPtr->cursorPos[0] < 0)
                    listPtr->cursorPos[0] = 0;
                if (listPtr->cursorPos[0] < listPtr->startPos[0])
                    listPtr->startPos[0] = listPtr->cursorPos[0];
                if (listPtr->cursorPos[0] >= listPtr->startPos[0] + viewmax)
                    listPtr->startPos[0] = listPtr->cursorPos[0] - viewmax + 1;
                item->cursorPos[0] = listPtr->cursorPos[0];
                UI_FeederSelection(item->special, item->cursorPos[0]);
            } else {
                listPtr->startPos[0]--;
                if (listPtr->startPos[0] < 0)
                    listPtr->startPos[0] = 0;
            }
            return 1;
        }
        if (key == 157 || key == 187) {
            if (!listPtr->notselectable) {
                listPtr->cursorPos[0]++;
                if (listPtr->cursorPos[0] < listPtr->startPos[0])
                    listPtr->startPos[0] = listPtr->cursorPos[0];
                if (listPtr->cursorPos[0] >= count)
                    listPtr->cursorPos[0] = count - 1;
                if (listPtr->cursorPos[0] >= listPtr->startPos[0] + viewmax)
                    listPtr->startPos[0] = listPtr->cursorPos[0] - viewmax + 1;
                item->cursorPos[0] = listPtr->cursorPos[0];
                UI_FeederSelection(item->special, item->cursorPos[0]);
            } else {
                listPtr->startPos[0]++;
                if (listPtr->startPos[0] >= count)
                    listPtr->startPos[0] = count - 1;
            }
            return 1;
        }
    } else {
        viewmax = (int)(item->window.rect[0].h / listPtr->elementHeight);
        if (key == 154 || key == 183 || key == 206) {
            if (!listPtr->notselectable) {
                item->cursorPos[0]--;
                if (item->cursorPos[0] < 0)
                    item->cursorPos[0] = 0;
                if (item->cursorPos[0] < listPtr->startPos[0])
                    listPtr->startPos[0] = item->cursorPos[0];
                if (item->cursorPos[0] >= listPtr->startPos[0] + viewmax)
                    listPtr->startPos[0] = item->cursorPos[0] - viewmax + 1;
                UI_FeederSelection(item->special, item->cursorPos[0]);
            } else {
                listPtr->startPos[0]--;
                if (listPtr->startPos[0] < 0)
                    listPtr->startPos[0] = 0;
            }
            return 1;
        }
        if (key == 155 || key == 189 || key == 205) {
            if (!listPtr->notselectable) {
                item->cursorPos[0]++;
                if (item->cursorPos[0] < listPtr->startPos[0])
                    listPtr->startPos[0] = item->cursorPos[0];
                if (item->cursorPos[0] >= count)
                    item->cursorPos[0] = count - 1;
                if (item->cursorPos[0] < 0)
                    item->cursorPos[0] = 0;
                if (item->cursorPos[0] >= listPtr->startPos[0] + viewmax)
                    listPtr->startPos[0] = item->cursorPos[0] - viewmax + 1;
                UI_FeederSelection(item->special, item->cursorPos[0]);
            } else {
                listPtr->startPos[0]++;
                if (listPtr->startPos[0] > max)
                    listPtr->startPos[0] = max;
            }
            return 1;
        }
    }
    // mouse hit
    if (key == 200 || key == 201) {
        if (flags & 0x100) {
            listPtr->startPos[0]--;
            if (listPtr->startPos[0] < 0)
                listPtr->startPos[0] = 0;
        } else if (flags & 0x200) {
            listPtr->startPos[0]++;
            if (listPtr->startPos[0] > max)
                listPtr->startPos[0] = max;
        } else if (flags & 0x800) {
            listPtr->startPos[0] -= viewmax;
            if (listPtr->startPos[0] < 0)
                listPtr->startPos[0] = 0;
        } else if (flags & 0x1000) {
            listPtr->startPos[0] += viewmax;
            if (listPtr->startPos[0] > max)
                listPtr->startPos[0] = max;
        } else if (!(flags & 0x400)) {
            if (item->special == 18.0f) {
                UI_OverrideCursorPos(item);
                UI_FeederSelection(item->special, item->cursorPos[0]);
            }
            if (dc->realTime < lastListBoxClickTime && listPtr->doubleClick
                && item->cursorPos[0] == listPtr->cursorPos[0] && ListBox_HasValidCursorPos(listPtr))
                Item_RunScript(dc, item, listPtr->doubleClick);
            lastListBoxClickTime = dc->realTime + 300;
            if (item->cursorPos[0] != listPtr->cursorPos[0]) {
                if (item->cursorPos[0] < 0)
                    item->cursorPos[0] = listPtr->cursorPos[0];
                if (listPtr->cursorPos[0] < UI_FeederCount(item->special))
                    item->cursorPos[0] = listPtr->cursorPos[0];
                UI_FeederSelection(item->special, item->cursorPos[0]);
            }
        }
        return 1;
    }
    if (key == 165 || key == 182) {
        if (!listPtr->notselectable) {
            item->cursorPos[0] = 0;
            listPtr->cursorPos[0] = 0;
            listPtr->startPos[0] = 0;
            UI_FeederSelection(item->special, item->cursorPos[0]);
        } else {
            listPtr->startPos[0] = 0;
        }
        return 1;
    }
    if (key == 166 || key == 188) {
        if (!listPtr->notselectable) {
            item->cursorPos[0] = count - 1;
            listPtr->cursorPos[0] = count - 1;
            if (item->cursorPos[0] >= listPtr->startPos[0] + viewmax)
                listPtr->startPos[0] = item->cursorPos[0] - viewmax + 1;
            if (item->cursorPos[0] < 0) {
                item->cursorPos[0] = 0;
                listPtr->cursorPos[0] = 0;
            }
            UI_FeederSelection(item->special, item->cursorPos[0]);
        } else {
            listPtr->startPos[0] = max;
        }
        return 1;
    }
    if (key == 164 || key == 184) {
        if (!listPtr->notselectable) {
            listPtr->cursorPos[0] -= viewmax;
            if (listPtr->cursorPos[0] < 0)
                listPtr->cursorPos[0] = 0;
            if (listPtr->cursorPos[0] < listPtr->startPos[0])
                listPtr->startPos[0] = listPtr->cursorPos[0];
            if (listPtr->cursorPos[0] >= listPtr->startPos[0] + viewmax)
                listPtr->startPos[0] = listPtr->cursorPos[0] - viewmax + 1;
            item->cursorPos[0] = listPtr->cursorPos[0];
            UI_FeederSelection(item->special, item->cursorPos[0]);
        } else {
            listPtr->startPos[0] -= viewmax;
            if (listPtr->startPos[0] < 0)
                listPtr->startPos[0] = 0;
        }
        return 1;
    }
    if (key == 163 || key == 190) {
        if (!listPtr->notselectable) {
            listPtr->cursorPos[0] += viewmax;
            if (listPtr->cursorPos[0] < listPtr->startPos[0])
                listPtr->startPos[0] = listPtr->cursorPos[0];
            if (listPtr->cursorPos[0] >= count)
                listPtr->cursorPos[0] = count - 1;
            if (listPtr->cursorPos[0] < 0)
                listPtr->cursorPos[0] = 0;
            if (listPtr->cursorPos[0] >= listPtr->startPos[0] + viewmax)
                listPtr->startPos[0] = listPtr->cursorPos[0] - viewmax + 1;
            item->cursorPos[0] = listPtr->cursorPos[0];
            UI_FeederSelection(item->special, item->cursorPos[0]);
        } else {
            listPtr->startPos[0] += viewmax;
            if (listPtr->startPos[0] > max)
                listPtr->startPos[0] = max;
        }
        return 1;
    }
    return 0;
}

void Item_ListBox_MouseEnter(itemDef_t *item, float x, float y)
{
    rectDef_t r;
    listBoxDef_s *listPtr;

    listPtr = Item_GetListBoxDef(item);
    if (!listPtr)
        return;
    Window_RemoveDynamicFlags(&item->window, 0x1f00);
    item->window.dynamicFlags[0] |= Item_ListBox_OverLB(item, x, y);
    r.horzAlign = item->window.rect[0].horzAlign;
    r.vertAlign = item->window.rect[0].vertAlign;
    if (Window_IsHorizontal(&item->window)) {
        if (!(item->window.dynamicFlags[0] & 0x1f00)) {
            if (listPtr->elementStyle == 1) {
                r.x = item->window.rect[0].x;
                r.y = item->window.rect[0].y;
                r.h = item->window.rect[0].h - 16.0f;
                r.w = item->window.rect[0].w - listPtr->drawPadding;
                if (Rect_ContainsPoint(&r, x, y)) {
                    listPtr->cursorPos[0] = (int)((x - r.x) / listPtr->elementWidth) + listPtr->startPos[0];
                    if (listPtr->cursorPos[0] >= listPtr->endPos[0])
                        listPtr->cursorPos[0] = listPtr->endPos[0];
                }
            }
        }
    } else if (!(item->window.dynamicFlags[0] & 0x1f00)) {
        r.x = item->window.rect[0].x;
        r.y = item->window.rect[0].y;
        r.w = item->window.rect[0].w - 16.0f;
        r.h = item->window.rect[0].h - listPtr->drawPadding;
        if (Rect_ContainsPoint(&r, x, y)) {
            listPtr->cursorPos[0] = (int)((y - 2.0f - r.y) / listPtr->elementHeight) + listPtr->startPos[0];
            if (listPtr->cursorPos[0] > listPtr->endPos[0])
                listPtr->cursorPos[0] = listPtr->endPos[0];
        }
    }
}

void Item_Paint(displayContextDef_t *dc, itemDef_t *item)
{
    byte *it = (byte *)item;
    menuDef_t *parent;

    parent = item->parent;

    if (item->window.dynamicFlags[0] & 0x2000) {
        if (dc->realTime > item->window.nextTime) {
            rectDef_t newRect;
            float rx;
            float ry;
            float w;
            float h;
            float s;
            float c;
            rectDef_t *r;

            item->window.nextTime = dc->realTime + item->window.offsetTime[0];
            r = &item->window.rectClient[0];
            w = r->w * 0.5f;
            h = r->h; h = h * 0.5f;
            rx = r->x + w; rx = rx - item->window.rectEffects0[0].x;
            ry = r->y + h - item->window.rectEffects0[0].y;
            float ang = 0.05235988f;
            FastSinCos(ang, &s, &c);
            newRect = *r;
            newRect.x = rx * c - ry * s + item->window.rectEffects0[0].x; newRect.x = newRect.x - w;
            newRect.y = rx * s + ry * c + item->window.rectEffects0[0].y - h;
            Window_SetRectClient(&item->window, &newRect);
            Item_UpdatePosition((itemDef_t *)item);

        }
    }

    if (item->window.dynamicFlags[0] & 0x80) {
        rectDef_t newRect;
        int dcTime = dc->realTime;
        if (dcTime > ((itemDef_t *)it)->window.nextTime) {

            ((itemDef_t *)it)->window.nextTime = dcTime + ((itemDef_t *)it)->window.offsetTime[0];

            float *target = (float *)(((char *)it + offsetof(itemDef_t, window.rectEffects0[0].x)));
            float *step = (float *)(((char *)it + offsetof(itemDef_t, window.rectEffects1[0].x)));
            int done = 0;

            newRect = (*(rectDef_t *)&((itemDef_t *)it)->window.rectClient[0].x);

            if (newRect.x == target[0]) {
                done++;
            } else if (newRect.x < target[0]) {
                newRect.x = newRect.x + step[0];
                if (newRect.x > target[0]) {
                    newRect.x = target[0];
                    done++;
                }
            } else {
                newRect.x -= step[0];
                if (newRect.x < target[0]) {
                    newRect.x = target[0];
                    done++;
                }
            }

            if (newRect.y == target[1]) {
                done++;
            } else if (newRect.y < target[1]) {
                newRect.y += step[1];
                if (newRect.y > target[1]) {
                    newRect.y = target[1];
                    done++;
                }
            } else {
                newRect.y -= step[1];
                if (newRect.y < target[1]) {
                    newRect.y = target[1];
                    done++;
                }
            }

            if (newRect.w == target[2]) {
                done++;
            } else if (newRect.w < target[2]) {
                newRect.w += step[2];
                if (newRect.w > target[2]) {
                    newRect.w = target[2];
                    done++;
                }
            } else {
                newRect.w -= step[2];
                if (newRect.w < target[2]) {
                    newRect.w = target[2];
                    done++;
                }
            }

            if (newRect.h == target[3]) {
                done++;
            } else if (newRect.h < target[3]) {
                newRect.h += step[3];
                if (newRect.h > target[3]) {
                    newRect.h = target[3];
                    done++;
                }
            } else {
                newRect.h -= step[3];
                if (newRect.h < target[3]) {
                    newRect.h = target[3];
                    done++;
                }
            }

            Window_SetRectClient( (Window *)(item), &newRect);

            Item_UpdatePosition((itemDef_t *)item);

            if (done == 4) {
                Window_RemoveDynamicFlags((Window *)item, 0x80);
            }
        }
    }

    if (item->window.ownerDrawFlags) {
        if (!UI_OwnerDrawVisible(item->window.ownerDrawFlags))
            Window_RemoveDynamicFlags(&item->window, 6);
        else
            Window_AddDynamicFlags(&item->window, 4);
    }

    if ((item->dvarFlags & 0xc) && !Item_EnableShowViaDvar(item, 4)) {
        if (Window_HasFocus(&item->window)) {
            Menu_HandleKey(dc, parent, 0x9b, 1);
            Menu_HandleKey(dc, parent, 0x9b, 0);
        }
        return;
    }

    if (!Window_IsVisible(&item->window))
        return;

    if (item->window.style == 6)
        item->window.background = CL_RegisterMaterialNoMip(Dvar_GetString(item->dvar), item->imageTrack);

    Window_Paint(dc, (float (*)[4][32])item, parent->fadeAmount, parent->fadeInAmount, parent->fadeClamp, (float)parent->fadeCycle);

    if (debugMode) {
        rectDef_t *r;
        vec4_t color;

        r = Item_CorrectedTextRect(item);
        color[1] = color[3] = 1.0f;
        color[0] = color[2] = 0.0f;
        UI_DrawRect(r->x, r->y, r->w, r->h, r->horzAlign, r->vertAlign, 1.0f, color);
    }

    if (item->window.style == 6)
        return;

    switch (item->type) {
    case 8:
        Item_OwnerDraw_Paint(dc, item);
        break;
    case 0:
    case 1:
        Item_Text_Paint(dc, item);
        break;
    case 4:
    case 9:
    case 16:
    case 17:
    case 18:
        Item_TextField_Paint(dc, item);
        break;
    case 6:
        Item_ListBox_Paint(dc, item);
        break;
    case 11:
        Item_YesNo_Paint(dc, item);
        break;
    case 12:
        Item_Multi_Paint(dc, item);
        break;
    case 13:
        Item_Enum_Paint(dc, item);
        break;
    case 14:
        Item_Bind_Paint(dc, item);
        break;
    case 10:
        Item_Slider_Paint(dc, item);
        break;
    default:
        break;
    }
}

void Menu_PaintAll(displayContextDef_t *dc)
{
    vec4_t v;
    int i;
    int start;
    menuDef_t *menu;

    dc->blurRadiusOut = 0.0f;
    if (captureFunc)
        ((void (__cdecl *)(displayContextDef_t *, void *))captureFunc)(dc, captureData);

    for (i = 0; i < dc->menuCount; i++) {
        menu = dc->Menus[i];
        if (!Menus_MenuIsInStack(dc, menu))
            Menu_Paint(dc, menu, 0);
    }

    start = 0;
    for (i = dc->openMenuCount - 1; i >= 0; i--) {
        if (dc->menuStack[i]->fullScreen) {
            start = i;
            break;
        }
    }

    for (i = start; i < dc->openMenuCount; i++)
        Menu_Paint(dc, dc->menuStack[i], 0);

    if (debugMode) {
        v[0] = 1.0f;
        v[1] = 1.0f;
        v[2] = 1.0f;
        v[3] = 1.0f;
        UI_DrawText(va("fps: %f", dc->FPS), 0x7fffffff, UI_GetFontHandle(0, 0.5f), 5.0f, 25.0f, 0, 0, 0.5f, v, 0);
    }
}

static commandDef_t commandList[29] = {
    { "fadein", (void (*)())&Script_FadeIn },
    { "fadeout", (void (*)())&Script_FadeOut },
    { "show", (void (*)())&Script_Show },
    { "hide", (void (*)())&Script_Hide },
    { "setcolor", (void (*)())&Script_SetColor },
    { "open", (void (*)())&Script_Open },
    { "openforgametype", (void (*)())&Script_OpenForGameType },
    { "closeforgametype", (void (*)())&Script_CloseForGameType },
    { "close", (void (*)())&Script_Close },
    { "ingameopen", (void (*)())&Script_InGameOpen },
    { "ingameclose", (void (*)())&Script_InGameClose },
    { "setbackground", (void (*)())&Script_SetBackground },
    { "setitemcolor", (void (*)())&Script_SetItemColor },
    { "setfocus", (void (*)())&Script_SetFocus },
    { "setfocusbydvar", (void (*)())&Script_SetFocusByDvar },
    { "transition", (void (*)())&Script_Transition },
    { "setdvar", (void (*)())&Script_SetDvar },
    { "exec", (void (*)())&Script_Exec },
    { "execnow", (void (*)())&Script_ExecNow },
    { "execOnDvarStringValue", (void (*)())&Script_ExecOnDvarStringValue },
    { "execOnDvarIntValue", (void (*)())&Script_ExecOnDvarIntValue },
    { "execOnDvarFloatValue", (void (*)())&Script_ExecOnDvarFloatValue },
    { "execNowOnDvarStringValue", (void (*)())&Script_ExecNowOnDvarStringValue },
    { "execNowOnDvarIntValue", (void (*)())&Script_ExecNowOnDvarIntValue },
    { "execNowOnDvarFloatValue", (void (*)())&Script_ExecNowOnDvarFloatValue },
    { "play", (void (*)())&Script_Play },
    { "orbit", (void (*)())&Script_Orbit },
    { "getautoupdate", (void (*)())&Script_GetAutoUpdate },
    { "scriptmenuresponse", (void (*)())&Script_ScriptMenuResponse }
};
static int scriptCommandCount = sizeof(commandList) / sizeof(commandDef_t);
static bind_t g_bindings[56] = {
    { (char *)"+scores", 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff },
    { (char *)"+speed", 0xc9, 0xffffffff, 0xffffffff, 0xffffffff },
    { (char *)"+forward", 0x77, 0xffffffff, 0xffffffff, 0xffffffff },
    { (char *)"+back", 0x73, 0xffffffff, 0xffffffff, 0xffffffff },
    { (char *)"+moveleft", 0x2c, 0xffffffff, 0xffffffff, 0xffffffff },
    { (char *)"+moveright", 0x2e, 0xffffffff, 0xffffffff, 0xffffffff },
    { (char *)"+moveup", 0x20, 0xffffffff, 0xffffffff, 0xffffffff },
    { (char *)"+movedown", 0x63, 0xffffffff, 0xffffffff, 0xffffffff },
    { (char *)"+left", 0x9c, 0xffffffff, 0xffffffff, 0xffffffff },
    { (char *)"+right", 0x9d, 0xffffffff, 0xffffffff, 0xffffffff },
    { (char *)"+strafe", 0x9e, 0xffffffff, 0xffffffff, 0xffffffff },
    { (char *)"+lookup", 0xa3, 0xffffffff, 0xffffffff, 0xffffffff },
    { (char *)"+lookdown", 0xa2, 0xffffffff, 0xffffffff, 0xffffffff },
    { (char *)"+mlook", 0x2f, 0xffffffff, 0xffffffff, 0xffffffff },
    { (char *)"centerview", 0xa6, 0xffffffff, 0xffffffff, 0xffffffff },
    { (char *)"+attack", 0x9f, 0xffffffff, 0xffffffff, 0xffffffff },
    { (char *)"weapprev", 0xcd, 0xffffffff, 0xffffffff, 0xffffffff },
    { (char *)"weapnext", 0xce, 0xffffffff, 0xffffffff, 0xffffffff },
    { (char *)"+frag", 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff },
    { (char *)"+smoke", 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff },
    { (char *)"scoresUp", 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff },
    { (char *)"scoresDown", 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff },
    { (char *)"chatmodepublic", 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff },
    { (char *)"chatmodeteam", 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff },
    { (char *)"+activate", 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff },
    { (char *)"+reload", 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff },
    { (char *)"help", 0xa7, 0xffffffff, 0xffffffff, 0xffffffff },
    { (char *)"+leanleft", 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff },
    { (char *)"+leanright", 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff },
    { (char *)"+usereload", 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff },
    { (char *)"vote yes", 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff },
    { (char *)"vote no", 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff },
    { (char *)"mp_QuickMessage", 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff },
    { (char *)"weaponslot primary", 0x31, 0xffffffff, 0xffffffff, 0xffffffff },
    { (char *)"weaponslot primaryb", 0x32, 0xffffffff, 0xffffffff, 0xffffffff },
    { (char *)"+binoculars", 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff },
    { (char *)"+breath_binoculars", 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff },
    { (char *)"+melee", 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff },
    { (char *)"+holdbreath", 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff },
    { (char *)"+melee_breath", 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff },
    { (char *)"+prone", 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff },
    { (char *)"+stance", 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff },
    { (char *)"lowerstance", 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff },
    { (char *)"raisestance", 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff },
    { (char *)"togglecrouch", 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff },
    { (char *)"toggleprone", 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff },
    { (char *)"goprone", 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff },
    { (char *)"gocrouch", 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff },
    { (char *)"+gostand", 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff },
    { (char *)"toggleads", 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff },
    { (char *)"leaveads", 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff },
    { (char *)"+talk", 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff },
    { (char *)"togglemenu", 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff },
    { (char *)"screenshot", 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff },
    { (char *)"screenshotJPEG", 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff },
    { (char *)"openScriptMenu ingame changeweapon", 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff }
};

const unsigned char border[96] = {
    0x10, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x08, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00,
    0x0a, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
    0x0c, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
    0x0e, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

