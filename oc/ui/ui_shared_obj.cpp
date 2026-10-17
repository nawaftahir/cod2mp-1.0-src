#include "common_types.h"
#include "imports.h"
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

extern keywordHash_t itemParseKeywords[66];
extern keywordHash_t menuParseKeywords[33];

extern char *va( const char *format, ... );

extern keywordHash_t *itemParseKeywordHash[512];
static char string_00f3b9c0[4096];
static struct
{
    loadAssets_t loadAssets;
    MenuList menuList;
    itemDef_t *items[256];
    menuDef_t *menus[128];
} g_load;
static char menuBuf[32768];
static keywordHash_t *menuParseKeywordHash[512];
static char menuBuf1[4096];
void UI_MapLoadInfo(const char *filename);
void PC_SourceError(int handle, char *format, ...);
qboolean PC_Script_Parse(int handle, const char **out);
void Window_SetStaticFlags(Window *w, const int flags);
void Window_SetDynamicFlags(Window *w, const int flags);
int PC_ReadTokenHandle(int handle, pc_token_t *pc_token);
int PC_ReadLineHandle(int handle, pc_token_t *pc_token);
const char *String_Alloc(const char *str);
void I_strncpyz(char *dest, const char *src, int destsize);
char *I_strlwr(char *str);
char *I_strlwrInl(char *str);
void I_strncat(char *dest, int destsize, const char *src);
MaterialHandle CL_RegisterMaterialNoMip(const char *name, int imageTrack);
snd_alias_list_t *Com_FindSoundAlias( const char *name );
void *UI_Alloc(int size, int align);
int PC_LoadSourceHandle(const char *filename);
int PC_FreeSourceHandle(int handle);
FontHandle CL_RegisterFont(const char *fontName, int imageTrack);
void CL_DuplicateFont(FontHandle fontCopy, const char *name);
void Com_DuplicateSoundAlias(const snd_alias_list_t *aliasCopy, const char *name);
int PC_SourceFileAndLine(int handle, char *filename, int *line);
listBoxDef_t *Item_GetListBoxDef(itemDef_t *item);
multiDef_t *Item_GetMultiDef(itemDef_t *item);
void Item_SetCursorPos(itemDef_t *item, int cursorPos);
void ListBox_SetCursorPos(listBoxDef_t *listBox, int cursorPos);
void ListBox_SetStartPos(listBoxDef_t *listBox, int startPos);
void ListBox_SetEndPos(listBoxDef_t *listBox, int endPos);
void Window_SetRectClient(Window *w, const UiRectangle *rectClient);
extern int FS_ReadFile(const char *filename, void **buffer);
extern void FS_FreeFile(void *buffer);
extern void Com_SetCSV(int csv);
extern char *Com_ParseOnLine( const char *( *data_p ) );
extern int Com_sprintf(char *dest, size_t size, const char *format, ...);
extern MaterialHandle CL_Material_Duplicate(MaterialHandle mtlCopy, const char *name);
void Window_SetRect(Window *w, const UiRectangle *rect);
void Menu_UpdatePosition(menuDef_t *menu);
void Item_InitControls(const char (*item)[4]);
qboolean MenuParse_onOpen(const char (*item)[4], int handle);
qboolean MenuParse_onClose(const char (*item)[4], int handle);
qboolean MenuParse_onESC(const char (*item)[4], int handle);
qboolean MenuParse_soundLoop(const char (*item)[4], int handle);
void __attribute_regparm__(2) Item_ValidateTypeData(itemDef_t *item, int handle);
qboolean Item_IsEditFieldDef(itemDef_t *item);
editFieldDef_t *Item_GetEditFieldDef(itemDef_t *item);
qboolean ItemParse_textsavegame(const char (*item)[4], int handle);
qboolean ItemParse_notselectable(const char (*item)[4], int handle);
qboolean ItemParse_noScrollBars(const char (*item)[4], int handle);
qboolean ItemParse_horizontalscroll(const char (*item)[4], int handle);
qboolean ItemParse_doubleClick(const char (*item)[4], int handle);
qboolean ItemParse_onFocus(const char (*item)[4], int handle);
qboolean ItemParse_leaveFocus(const char (*item)[4], int handle);
qboolean ItemParse_mouseEnter(const char (*item)[4], int handle);
qboolean ItemParse_mouseExit(const char (*item)[4], int handle);
qboolean ItemParse_mouseEnterText(const char (*item)[4], int handle);
qboolean ItemParse_mouseExitText(const char (*item)[4], int handle);
qboolean ItemParse_action(itemDef_t *item, int handle);
qboolean ItemParse_accept(itemDef_t *item, int handle);
qboolean Item_Parse_maxCharsGotoNext(const char (*item)[4], int handle);
qboolean ItemParse_dvarStrList(const char (*item)[4], int handle);
qboolean ItemParse_enableDvar(const char (*item)[4], int handle);
qboolean ItemParse_disableDvar(const char (*item)[4], int handle);
qboolean ItemParse_showDvar(const char (*item)[4], int handle);
qboolean ItemParse_hideDvar(const char (*item)[4], int handle);
qboolean ItemParse_focusDvar(const char (*item)[4], int handle);
void Menu_PostParse(menuDef_t *menu);
qboolean MenuParse_popup(const char (*item)[4], int handle);
qboolean MenuParse_outOfBounds(const char (*item)[4], int handle);
qboolean MenuParse_legacySplitScreenScale(const char (*item)[4], int handle);
qboolean ItemParse_decoration(const char (*item)[4], int handle);
qboolean ItemParse_wrapped(const char (*item)[4], int handle);
qboolean ItemParse_autowrapped(const char (*item)[4], int handle);
qboolean MenuParse_execKey(const char (*item)[4], int handle);
qboolean ItemParse_execKey(const char (*item)[4], int handle);
qboolean ItemParse_textfile(const char (*item)[4], int handle);
int FS_FOpenFileByMode(const char *qpath, fileHandle_t *f, fsMode_t mode);
int FS_Read(void *buffer, int len, fileHandle_t h);
void FS_FCloseFile(fileHandle_t h);
int Com_Compress(char *data_p);
void Com_BeginParseSession(const char *filename);
void Com_EndParseSession(void);
void Com_EndParseSessionInl(void);
char *Com_Parse( const char *( *data_p ) );
int I_stricmp(const char *s1, const char *s2);
bool I_isupper( int c );
bool I_islower( int c );
void Com_Printf(const char *fmt, ...);
void Com_Error(errorParm_t code, const char *fmt, ...);
void Menu_SetCursorItem(menuDef_t *menu, int cursorItem);
static bool Menu_New(int handle, int imageTrack);
qboolean Item_Parse(int handle, const char (*item)[4]);
qboolean MenuParse_itemDef(const char (*item)[4], int handle);
void Item_SetupKeywordHash(void);
void Menu_SetupKeywordHash(void);
qboolean MenuParse_name(menuDef_t *menu, int handle);
qboolean MenuParse_background(const char (*item)[4], int handle);
qboolean MenuParse_cinematic(menuDef_t *menu, int handle);
qboolean ItemParse_name(itemDef_t *item, int handle);
qboolean ItemParse_focusSound(const char (*item)[4], int handle);
qboolean ItemParse_text(itemDef_t *item, int handle);
qboolean ItemParse_group(itemDef_t *item, int handle);
qboolean ItemParse_background(const char (*item)[4], int handle);
qboolean ItemParse_cinematic(itemDef_t *item, int handle);
qboolean ItemParse_dvarTest(itemDef_t *item, int handle);
qboolean ItemParse_dvar(const char (*item)[4], int handle);
qboolean ItemParse_dvarEnumList(const char (*item)[4], int handle);
qboolean ItemParse_outlinecolor(const char (*item)[4], int handle);
qboolean MenuParse_outlinecolor(const char (*item)[4], int handle);
qboolean PC_Rect_Parse(int handle, rectDef_t *r);
qboolean ItemParse_rect(const char (*item)[4], int handle);
qboolean MenuParse_rect(const char (*item)[4], int handle);
qboolean MenuParse_borderSize(const char (*item)[4], int handle);
qboolean MenuParse_backcolor(const char (*item)[4], int handle);
qboolean MenuParse_forecolor(const char (*item)[4], int handle);
qboolean MenuParse_bordercolor(const char (*item)[4], int handle);
qboolean MenuParse_focuscolor(const char (*item)[4], int handle);
qboolean MenuParse_disablecolor(const char (*item)[4], int handle);
qboolean MenuParse_fadeClamp(const char (*item)[4], int handle);
qboolean MenuParse_fadeAmount(const char (*item)[4], int handle);
qboolean MenuParse_fadeInAmount(const char (*item)[4], int handle);
qboolean MenuParse_blurWorld(const char (*item)[4], int handle);
qboolean ItemParse_elementwidth(const char (*item)[4], int handle);
qboolean ItemParse_elementheight(const char (*item)[4], int handle);
qboolean ItemParse_feeder(const char (*item)[4], int handle);
qboolean ItemParse_bordersize(const char (*item)[4], int handle);
qboolean ItemParse_textalignx(const char (*item)[4], int handle);
qboolean ItemParse_textaligny(const char (*item)[4], int handle);
qboolean ItemParse_textscale(const char (*item)[4], int handle);
qboolean ItemParse_backcolor(const char (*item)[4], int handle);
qboolean ItemParse_forecolor(const char (*item)[4], int handle);
qboolean ItemParse_bordercolor(const char (*item)[4], int handle);
qboolean ItemParse_special(const char (*item)[4], int handle);
qboolean ItemParse_dvarFloat(const char (*item)[4], int handle);
qboolean ItemParse_dvarFloatList(const char (*item)[4], int handle);
qboolean MenuParse_fullscreen(const char (*item)[4], int handle);
qboolean MenuParse_style(const char (*item)[4], int handle);
qboolean MenuParse_visible(const char (*item)[4], int handle);
qboolean MenuParse_border(const char (*item)[4], int handle);
qboolean MenuParse_ownerdrawFlag(const char (*item)[4], int handle);
qboolean MenuParse_ownerdraw(const char (*item)[4], int handle);
qboolean MenuParse_fadeCycle(const char (*item)[4], int handle);
qboolean MenuParse_execKeyInt(const char (*item)[4], int handle);
qboolean ItemParse_origin(const char (*item)[4], int handle);
qboolean ItemParse_style(const char (*item)[4], int handle);
qboolean ItemParse_type(const char (*item)[4], int handle);
qboolean ItemParse_elementtype(const char (*item)[4], int handle);
qboolean ItemParse_columns(const char (*item)[4], int handle);
qboolean ItemParse_border(const char (*item)[4], int handle);
qboolean ItemParse_visible(const char (*item)[4], int handle);
qboolean ItemParse_ownerdraw(const char (*item)[4], int handle);
qboolean ItemParse_align(const char (*item)[4], int handle);
qboolean ItemParse_textalign(const char (*item)[4], int handle);
qboolean ItemParse_textstyle(const char (*item)[4], int handle);
qboolean ItemParse_textfont(const char (*item)[4], int handle);
qboolean ItemParse_maxChars(const char (*item)[4], int handle);
qboolean ItemParse_maxPaintChars(const char (*item)[4], int handle);
qboolean ItemParse_ownerdrawFlag(const char (*item)[4], int handle);
qboolean ItemParse_execKeyInt(const char (*item)[4], int handle);
static Bool __attribute_regparm__(2) UI_ParseMenuInternal(const char *menuFile, int imageTrack);
MenuList *UI_LoadMenus(const char *menuFile, int imageTrack);
MenuList *UI_LoadMenu(const char *menuFile, int imageTrack);


qboolean PC_Int_Parse(int handle, int *i);
static bool Menu_New(int handle, int imageTrack);
static Bool __attribute_regparm__(2) UI_ParseMenuInternal(const char *menuFile, int imageTrack);


























// Name from CoD4; original CoD2 name unknown.
static void UI_ParseMenuMaterial(const char *key, const char *value)
{
    MaterialHandle material;
    char name[64];

    material = (MaterialHandle)CL_RegisterMaterialNoMip(value, 3);
    Com_sprintf(name, 64, (const char *)"$%s", key);
    I_strlwrInl(name);
    CL_Material_Duplicate(material, name);
}

void UI_MapLoadInfo(const char *filename)
{
    void *loadfile;
    const char *parse;
    const char *token;
    char key[256];
    unsigned int tokenLen;
    const char *value;

    if (!filename[0])
        return;

    if (FS_ReadFile(filename, &loadfile) < 0) {
        Com_Printf((const char *)"^3WARNING: Could not find '%s'.\n", filename);
        return;
    }

    parse = (const char *)loadfile;
    Com_BeginParseSession(filename);
    Com_SetCSV(1);

    for (;;) {
        token = Com_Parse(&parse);
        if (!token[0])
            break;

        tokenLen = strlen(token) + 1;
        if (tokenLen >= 256) {
            Com_EndParseSessionInl();
            Com_Error( (errorParm_t)(1), (const char *)"key '%s' is %i > %i characters long", key, tokenLen - 1, 255);
        }

        memcpy(key, token, tokenLen);

        value = Com_ParseOnLine(&parse);
        if (!value[0]) {
            Com_EndParseSessionInl();
            Com_Error( (errorParm_t)(1), (const char *)"key '%s' missing value in '%s'\n", key, filename);
            break;
        }

        UI_ParseMenuMaterial(key, value);
    }

    Com_EndParseSessionInl();
    FS_FreeFile(loadfile);
}

void PC_SourceError(int handle, char *format, ...)
{
    int line;
    va_list argptr;
    char filename[128];

    va_start(argptr, format);
    vsprintf(string_00f3b9c0, format, argptr);
    va_end(argptr);

    filename[0] = '\0';
    line = 0;
    PC_SourceFileAndLine(handle, filename, &line);
    Com_Printf("^1Menu load error: %s, line %d: %s\n", filename, line, string_00f3b9c0);
}

qboolean PC_Float_Parse(int handle, float *f)
{
    pc_token_t token;
    int negative;

    if (!PC_ReadTokenHandle(handle, &token))
        return 0;
    negative = 0;
    if (token.string[0] == '-') {
        if (!PC_ReadTokenHandle(handle, &token))
            return 0;
        negative = 1;
    }
    if (token.type != 3) {
        PC_SourceError(handle, "expected float but found %s\n", token.string);
        return 0;
    }
    if (negative)
        *f = -token.floatvalue;
    else
        *f = token.floatvalue;
    return 1;
}

qboolean PC_Color_Parse(int handle, float (*c)[4])
{
    int i;
    float f;

    for (i = 0; i < 4; i++) {
        if (!PC_Float_Parse(handle, &f))
            return 0;
        (*c)[i] = f;
    }
    return 1;
}

// name unconfirmed; PC_Int_Parse on the current line only
qboolean PC_Int_ParseLine(int handle, int *i)
{
    pc_token_t token;
    int negative;

    if (!PC_ReadLineHandle(handle, &token))
        return 0;
    negative = 0;
    if (token.string[0] == '-') {
        if (!PC_ReadLineHandle(handle, &token))
            return 0;
        negative = 1;
    }
    if (token.type != 3) {
        PC_SourceError(handle, "expected integer but found %s\n", token.string);
        return 0;
    }
    *i = token.intvalue;
    if (negative)
        *i = -*i;
    return 1;
}
qboolean PC_Rect_Parse(int handle, rectDef_t *r)
{
    if (PC_Float_Parse(handle, &r->x) && PC_Float_Parse(handle, &r->y) && PC_Float_Parse(handle, &r->w) && PC_Float_Parse(handle, &r->h))
    {
        if (!PC_Int_ParseLine(handle, &r->horzAlign))
            r->horzAlign = 0;
        if (!PC_Int_ParseLine(handle, &r->vertAlign))
            r->vertAlign = 0;
        return 1;
    }
    return 0;
}

qboolean PC_String_Parse(int handle, const char **out)
{
    pc_token_t token;

    if (!PC_ReadTokenHandle(handle, &token))
        return 0;
    *out = String_Alloc(token.string);
    return 1;
}

qboolean PC_Char_Parse(int handle, char *out)
{
    pc_token_t token;

    if (!PC_ReadTokenHandle(handle, &token))
        return 0;
    *out = token.string[0];
    return 1;
}

qboolean PC_Script_Parse(int handle, const char **out)
{
    char script[1024];
    pc_token_t token;

    memset(script, 0, sizeof(script));

    if (!PC_ReadTokenHandle(handle, &token))
        return 0;
    if (I_stricmp(token.string, "{"))
        return 0;

    for (;;) {
        if (!PC_ReadTokenHandle(handle, &token))
            return 0;
        if (!I_stricmp(token.string, "}")) {
            *out = String_Alloc(script);
            return 1;
        }

        if (token.string[0] == '\0' || token.string[1] != '\0')
            I_strncat(script, sizeof(script), va("\"%s\"", token.string));
        else
            I_strncat(script, sizeof(script), token.string);
        I_strncat(script, sizeof(script), " ");
    }
}

void Window_Init(Window *w)
{
    memset(w, 0, sizeof(Window));
    w->borderSize = 1.0f;
    w->foreColor[0] = w->foreColor[1] = w->foreColor[2] = w->foreColor[3] = 1.0f;
    w->cinematic = -1;
}

void Menu_Init(menuDef_t *menu, int imageTrack)
{
    loadAssets_t *load = &g_load.loadAssets;

    memset(menu, 0, sizeof(menuDef_t));
    menu->cursorItem[0] = -1;
    menu->fadeAmount = load->fadeAmount;
    menu->fadeInAmount = load->fadeInAmount;
    menu->fadeClamp = load->fadeClamp;
    menu->fadeCycle = load->fadeCycle;
    menu->imageTrack = imageTrack;
    menu->items = g_load.items;
    Window_Init(&menu->window);
}

void Item_Init(itemDef_t *item, int imageTrack)
{
    memset(item, 0, sizeof(itemDef_t));
    item->imageTrack = imageTrack;
    item->textscale = 0.55f;
    Window_Init(&item->window);
}

int KeywordHash_Key(const char *keyword)
{
    int hash, i;
    int c;

    hash = 0;
    for (i = 0; keyword[i]; i++) {
        c = keyword[i];
        if (c >= 'A' && c <= 'Z')
            hash += (c + ('a' - 'A')) * (119 + i);
        else
            hash += c * (119 + i);
    }
    hash = (hash ^ (hash >> 10) ^ (hash >> 20)) & 0x1ff;
    return hash;
}

void KeywordHash_Add(keywordHash_t *table[], keywordHash_t *key)
{
    int hash;

    hash = KeywordHash_Key(key->keyword);
    key->next = (intptr_t)table[hash];
    table[hash] = key;
}

keywordHash_t *KeywordHash_Find(keywordHash_t *table[], const char *keyword)
{
    keywordHash_t *key;
    int hash;

    hash = KeywordHash_Key(keyword);
    for (key = table[hash]; key; key = (keywordHash_t *)key->next) {
        if (!I_stricmp(key->keyword, keyword))
            return key;
    }
    return NULL;
}

qboolean Item_Parse(int handle, const char (*item)[4])
{
    pc_token_t token;
    keywordHash_t *key;

    if (!PC_ReadTokenHandle(handle, &token))
        return 0;
    if (token.string[0] != '{')
        return 0;
    while (1) {
        if (!PC_ReadTokenHandle(handle, &token)) {
            PC_SourceError(handle, "end of file inside menu item\n");
            return 0;
        }
        if (token.string[0] == '}')
            return 1;
        key = KeywordHash_Find(itemParseKeywordHash, token.string);
        if (!key) {
            PC_SourceError(handle, "unknown menu item keyword %s", token.string);
            continue;
        }
        if (!((qboolean (*)(const char (*)[4], int))key->func)(item, handle)) {
            PC_SourceError(handle, "couldn't parse menu item keyword %s", token.string);
            return 0;
        }
    }
}

void Item_InitControls(const char (*item)[4])
{
    itemDef_t *itemDef = (itemDef_t *)item;
    listBoxDef_t *listPtr;

    if (!itemDef || itemDef->type != 6)
        return;

    listPtr = Item_GetListBoxDef(itemDef);
    Item_SetCursorPos(itemDef, 0);
    if (!listPtr)
        return;

    ListBox_SetCursorPos(listPtr, 0);
    ListBox_SetStartPos(listPtr, 0);
    ListBox_SetEndPos(listPtr, 0);
}

qboolean MenuParse_fullscreen(const char (*item)[4], int handle)
{
    if (!PC_Int_Parse(handle, (int *)&((menuDef_t *)item)->fullScreen))
        return 0;
    return 1;
}

qboolean MenuParse_rect(const char (*item)[4], int handle)
{
    rectDef_t rect;

    if (!PC_Rect_Parse(handle, &rect))
        return 0;

    Window_SetRect((Window *)((menuDef_t *)item), &rect);
    return 1;
}

qboolean MenuParse_style(const char (*item)[4], int handle)
{
    if (!PC_Int_Parse(handle, (int *)&((itemDef_t *)item)->window.style))
        return 0;
    return 1;
}

qboolean MenuParse_onOpen(const char (*item)[4], int handle)
{
    return PC_Script_Parse(handle, (const char **)&((menuDef_t *)item)->onOpen) != 0;
}

qboolean MenuParse_onClose(const char (*item)[4], int handle)
{
    return PC_Script_Parse(handle, (const char **)&((menuDef_t *)item)->onClose) != 0;
}

qboolean MenuParse_onESC(const char (*item)[4], int handle)
{
    return PC_Script_Parse(handle, (const char **)&((menuDef_t *)item)->onESC) != 0;
}

qboolean MenuParse_backcolor(const char (*item)[4], int handle)
{
    int i;
    float f;
    menuDef_t *menu = (menuDef_t *)item;

    for (i = 0; i < 4; i++) {
        if (!PC_Float_Parse(handle, &f))
            return 0;
        menu->window.backColor[i] = f;
    }
    return 1;
}

qboolean MenuParse_bordercolor(const char (*item)[4], int handle)
{
    int i;
    float f;
    menuDef_t *menu = (menuDef_t *)item;

    for (i = 0; i < 4; i++) {
        if (!PC_Float_Parse(handle, &f))
            return 0;
        menu->window.borderColor[i] = f;
    }
    return 1;
}

qboolean MenuParse_focuscolor(const char (*item)[4], int handle)
{
    int i;
    float f;
    menuDef_t *menu = (menuDef_t *)item;

    for (i = 0; i < 4; i++) {
        if (!PC_Float_Parse(handle, &f))
            return 0;
        menu->focusColor[i] = f;
    }
    return 1;
}

qboolean MenuParse_disablecolor(const char (*item)[4], int handle)
{
    int i;
    float f;
    menuDef_t *menu = (menuDef_t *)item;

    for (i = 0; i < 4; i++) {
        if (!PC_Float_Parse(handle, &f))
            return 0;
        menu->disableColor[i] = f;
    }
    return 1;
}

qboolean MenuParse_background(const char (*item)[4], int handle)
{
    const char *temp;
    char name[64];

    if (!PC_String_Parse(handle, &temp))
        return 0;
    I_strncpyz(name, temp, 64);
    I_strlwrInl(name);
    (*(void **)&((itemDef_t *)item)->window.background) = CL_RegisterMaterialNoMip(name, ((menuDef_t *)item)->imageTrack);
    return 1;
}

qboolean MenuParse_ownerdraw(const char (*item)[4], int handle)
{
    if (!PC_Int_Parse(handle, (int *)&((itemDef_t *)item)->window.ownerDraw))
        return 0;
    return 1;
}

qboolean MenuParse_popup(const char (*item)[4], int handle)
{
    Window_SetStaticFlags((Window *)((void *)item), ((itemDef_t *)item)->window.staticFlags | 0x1000000);
    return 1;
}

qboolean MenuParse_outOfBounds(const char (*item)[4], int handle)
{
    Window_SetStaticFlags((Window *)((void *)item), ((itemDef_t *)item)->window.staticFlags | 0x2000000);
    return 1;
}

qboolean MenuParse_soundLoop(const char (*item)[4], int handle)
{
    pc_token_t token;

    if (!PC_ReadTokenHandle(handle, &token))
        return 0;
    if (token.string[0])
        ((menuDef_t *)item)->soundName = String_Alloc(token.string);
    return 1;
}

qboolean MenuParse_fadeClamp(const char (*item)[4], int handle)
{
    if (!PC_Float_Parse(handle, &((menuDef_t *)item)->fadeClamp))
        return 0;
    return 1;
}

qboolean MenuParse_fadeAmount(const char (*item)[4], int handle)
{
    if (!PC_Float_Parse(handle, &((menuDef_t *)item)->fadeAmount))
        return 0;
    return 1;
}

qboolean MenuParse_fadeInAmount(const char (*item)[4], int handle)
{
    if (!PC_Float_Parse(handle, &((menuDef_t *)item)->fadeInAmount))
        return 0;
    return 1;
}

qboolean MenuParse_fadeCycle(const char (*item)[4], int handle)
{
    if (!PC_Int_Parse(handle, (int *)&((menuDef_t *)item)->fadeCycle))
        return 0;
    return 1;
}

qboolean MenuParse_itemDef(const char (*item)[4], int handle)
{
    menuDef_t *menu = (menuDef_t *)item;

    if (menu->itemCount < 256) {
        menu->items[menu->itemCount] = (itemDef_t *)UI_Alloc(sizeof(itemDef_t), 4);
        Item_Init(menu->items[menu->itemCount], menu->imageTrack);
        if (!Item_Parse(handle, (const char (*)[4])menu->items[menu->itemCount]))
            return 0;
        Item_InitControls((const char (*)[4])menu->items[menu->itemCount]);
        menu->items[menu->itemCount++]->parent = menu;
    }
    return 1;
}

qboolean MenuParse_execKey(const char (*item)[4], int handle)
{
    menuDef_t *menu = (menuDef_t *)item;
    char keyname;
    const char *action;
    ItemKeyHandler *handler;

    if (!PC_Char_Parse(handle, &keyname))
        return 0;
    if (!PC_Script_Parse(handle, &action))
        return 0;

    handler = (ItemKeyHandler *)UI_Alloc(sizeof(ItemKeyHandler), 4);
    handler->key = (unsigned char)keyname;
    handler->action = action;
    handler->next = (intptr_t)menu->onKey;
    menu->onKey = handler;
    return 1;
}

qboolean MenuParse_execKeyInt(const char (*item)[4], int handle)
{
    menuDef_t *menu = (menuDef_t *)item;
    const char *action;
    ItemKeyHandler *handler;
    int keyname;

    if (!PC_Int_Parse(handle, &keyname))
        return 0;
    if (!PC_Script_Parse(handle, &action))
        return 0;

    handler = (ItemKeyHandler *)UI_Alloc(sizeof(ItemKeyHandler), 4);
    handler->key = keyname;
    handler->action = action;
    handler->next = (intptr_t)menu->onKey;
    menu->onKey = handler;
    return 1;
}

qboolean MenuParse_blurWorld(const char (*item)[4], int handle)
{
    float *blurRadius = &((menuDef_t *)item)->blurRadius;

    if (!PC_Float_Parse(handle, blurRadius))
        return 0;

    if (*blurRadius < 0.0f) {
        PC_SourceError(handle, "blur must be >= 0; %g is invalid", *blurRadius);
        return 0;
    }

    return 1;
}

qboolean MenuParse_legacySplitScreenScale(const char (*item)[4], int handle)
{
    Window_SetStaticFlags((Window *)((void *)item), ((itemDef_t *)item)->window.staticFlags | 0x4000000);
    return 1;
}

// name from KisakCOD; 1.0 has no buffer-size check
char *UI_FileText(const char *fileName)
{
    int len;
    fileHandle_t f;

    len = FS_FOpenFileByMode(fileName, &f, FS_READ);
    if (!f)
        return NULL;
    FS_Read(menuBuf1, len, f);
    menuBuf1[len] = 0;
    FS_FCloseFile(f);
    return menuBuf1;
}

void __attribute_regparm__(2) Item_ValidateTypeData(itemDef_t *item, int handle)
{
    editFieldDef_t *editPtr;

    if (item->typeData.data) {
        if (item->dataType != item->type) {
            PC_SourceError(handle,
                           "Attempting to change type from %d to %d.\nMove the type definition higher up in the itemDef.\n",
                           item->dataType, item->type);
        }
        return;
    }
    item->dataType = item->type;
    if (item->type == 6) {
        item->typeData.listBox = (listBoxDef_t *)UI_Alloc(sizeof(listBoxDef_t), 4);
    } else if (item->type == 4 || item->type == 9 || item->type == 16 || item->type == 18 || item->type == 11
               || item->type == 14 || item->type == 10 || item->type == 0 || item->type == 17) {
        item->typeData.editField = (editFieldDef_t *)UI_Alloc(sizeof(editFieldDef_t), 4);
        if (item->type == 4 || item->type == 16 || item->type == 9 || item->type == 18 || item->type == 17) {
            editPtr = Item_GetEditFieldDef(item);
            if (!editPtr->maxPaintChars)
                editPtr->maxPaintChars = 256;
        }
    } else if (item->type == 12) {
        item->typeData.multi = (multiDef_t *)UI_Alloc(sizeof(multiDef_t), 4);
    }
}

qboolean ItemParse_focusSound(const char (*item)[4], int handle)
{
    itemDef_t *itemDef = (itemDef_t *)item;
    const char *temp;

    if (!PC_String_Parse(handle, &temp))
        return 0;
    itemDef->focusSound = Com_FindSoundAlias(temp);
    return 1;
}

qboolean ItemParse_text(itemDef_t *item, int handle)
{
    pc_token_t token;

    if (!PC_ReadTokenHandle(handle, &token))
        return 0;
    item->text = String_Alloc(token.string);
    return 1;
}

qboolean ItemParse_textfile(const char (*item)[4], int handle)
{
    pc_token_t token;

    if (!PC_ReadTokenHandle(handle, &token))
        return 0;
    ((itemDef_t *)item)->text = String_Alloc(UI_FileText(token.string));
    return 1;
}

qboolean ItemParse_textsavegame(const char (*item)[4], int handle)
{
    ((itemDef_t *)item)->text = (const char *)"savegameinfo";
    ((itemDef_t *)item)->textSavegameInfo = 1;
    return 1;
}

qboolean ItemParse_group(itemDef_t *item, int handle)
{
    pc_token_t token;

    if (!PC_ReadTokenHandle(handle, &token))
        return 0;
    item->window.group = String_Alloc(token.string);
    return 1;
}

qboolean ItemParse_rect(const char (*item)[4], int handle)
{
    rectDef_t rectClient;

    if (!PC_Rect_Parse(handle, &rectClient))
        return 0;

    Window_SetRectClient((Window *)item, &rectClient);
    return 1;
}

qboolean ItemParse_style(const char (*item)[4], int handle)
{
    return PC_Int_Parse(handle, (int *)&((itemDef_t *)item)->window.style);
}

qboolean ItemParse_decoration(const char (*item)[4], int handle)
{
    Window_SetStaticFlags((Window *)((void *)item), ((itemDef_t *)item)->window.staticFlags | 0x100000);
    return 1;
}

qboolean ItemParse_notselectable(const char (*item)[4], int handle)
{
    itemDef_t *itemDef = (itemDef_t *)item;
    listBoxDef_t *listPtr;

    Item_ValidateTypeData(itemDef, handle);
    listPtr = Item_GetListBoxDef(itemDef);
    if (!listPtr)
        return 0;
    if (itemDef->type == 6)
        listPtr->notselectable = 1;
    return 1;
}

qboolean ItemParse_noScrollBars(const char (*item)[4], int handle)
{
    itemDef_t *itemDef = (itemDef_t *)item;
    listBoxDef_t *listPtr;

    Item_ValidateTypeData(itemDef, handle);
    listPtr = Item_GetListBoxDef(itemDef);
    if (!listPtr)
        return 0;
    if (itemDef->type == 6)
        listPtr->noScrollBars = 1;
    return 1;
}

qboolean ItemParse_wrapped(const char (*item)[4], int handle)
{
    Window_SetStaticFlags((Window *)((void *)item), ((itemDef_t *)item)->window.staticFlags | 0x400000);
    return 1;
}

qboolean ItemParse_autowrapped(const char (*item)[4], int handle)
{
    Window_SetStaticFlags((Window *)((void *)item), ((itemDef_t *)item)->window.staticFlags | 0x800000);
    return 1;
}

qboolean ItemParse_horizontalscroll(const char (*item)[4], int handle)
{
    Window_SetStaticFlags((Window *)((void *)item), ((itemDef_t *)item)->window.staticFlags | 0x200000);
    return 1;
}

qboolean ItemParse_type(const char (*item)[4], int handle)
{
    itemDef_t *itemDef = (itemDef_t *)item;

    if (!PC_Int_Parse(handle, &itemDef->type))
        return 0;

    Item_ValidateTypeData(itemDef, handle);
    return 1;
}

qboolean ItemParse_elementwidth(const char (*item)[4], int handle)
{
    itemDef_t *itemDef = (itemDef_t *)item;
    listBoxDef_t *listPtr;

    Item_ValidateTypeData(itemDef, handle);
    listPtr = Item_GetListBoxDef(itemDef);
    if (!listPtr)
        return 0;

    if (!PC_Float_Parse(handle, &listPtr->elementWidth))
        return 0;
    return 1;
}

qboolean ItemParse_elementheight(const char (*item)[4], int handle)
{
    itemDef_t *itemDef = (itemDef_t *)item;
    listBoxDef_t *listPtr;

    Item_ValidateTypeData(itemDef, handle);
    listPtr = Item_GetListBoxDef(itemDef);
    if (!listPtr)
        return 0;

    if (!PC_Float_Parse(handle, &listPtr->elementHeight))
        return 0;
    return 1;
}

qboolean ItemParse_elementtype(const char (*item)[4], int handle)
{
    itemDef_t *itemDef = (itemDef_t *)item;
    listBoxDef_t *listPtr;

    Item_ValidateTypeData(itemDef, handle);
    if (!itemDef->typeData.data)
        return 0;

    listPtr = Item_GetListBoxDef(itemDef);
    if (!listPtr)
        return 0;

    if (!PC_Int_Parse(handle, &listPtr->elementStyle))
        return 0;
    return 1;
}

qboolean ItemParse_columns(const char (*item)[4], int handle)
{
    itemDef_t *itemDef = (itemDef_t *)item;
    listBoxDef_t *listPtr;
    int num;
    int i;

    Item_ValidateTypeData(itemDef, handle);
    if (!itemDef->typeData.data)
        return 0;
    listPtr = Item_GetListBoxDef(itemDef);
    if (!listPtr)
        return 0;
    if (!PC_Int_Parse(handle, &num))
        return 0;
    if (num > 16)
        num = 16;
    listPtr->numColumns = num;
    for (i = 0; i < num; i++) {
        int pos, width, maxChars;

        if (PC_Int_Parse(handle, &pos) && PC_Int_Parse(handle, &width) && PC_Int_Parse(handle, &maxChars)) {
            listPtr->columnInfo[i].pos = pos;
            listPtr->columnInfo[i].width = width;
            listPtr->columnInfo[i].maxChars = maxChars;
        } else {
            return 0;
        }
    }
    return 1;
}

qboolean ItemParse_ownerdraw(const char (*item)[4], int handle)
{
    itemDef_t *itemDef = (itemDef_t *)item;

    if (!PC_Int_Parse(handle, &itemDef->window.ownerDraw))
        return 0;

    itemDef->type = 8;
    return 1;
}

qboolean ItemParse_align(const char (*item)[4], int handle)
{
    if (!PC_Int_Parse(handle, (int *)&((itemDef_t *)item)->alignment))
        return 0;
    return 1;
}

qboolean ItemParse_textalign(const char (*item)[4], int handle)
{
    if (!PC_Int_Parse(handle, (int *)&((itemDef_t *)item)->textalignment))
        return 0;
    return 1;
}

qboolean ItemParse_textalignx(const char (*item)[4], int handle)
{
    if (!PC_Float_Parse(handle, (float *)&((itemDef_t *)item)->textalignx))
        return 0;
    return 1;
}

qboolean ItemParse_textaligny(const char (*item)[4], int handle)
{
    if (!PC_Float_Parse(handle, (float *)&((itemDef_t *)item)->textaligny))
        return 0;
    return 1;
}

qboolean ItemParse_textscale(const char (*item)[4], int handle)
{
    if (!PC_Float_Parse(handle, (float *)&((itemDef_t *)item)->textscale))
        return 0;
    return 1;
}

qboolean ItemParse_textstyle(const char (*item)[4], int handle)
{
    if (!PC_Int_Parse(handle, (int *)&((itemDef_t *)item)->textStyle))
        return 0;
    return 1;
}

qboolean ItemParse_textfont(const char (*item)[4], int handle)
{
    if (!PC_Int_Parse(handle, (int *)&((itemDef_t *)item)->fontEnum))
        return 0;
    return 1;
}

qboolean ItemParse_backcolor(const char (*item)[4], int handle)
{
    int i;
    float f;

    for (i = 0; i < 4; i++) {
        if (!PC_Float_Parse(handle, &f))
            return 0;
        ((itemDef_t *)item)->window.backColor[i] = f;
    }
    return 1;
}

qboolean ItemParse_bordercolor(const char (*item)[4], int handle)
{
    int i;
    float f;

    for (i = 0; i < 4; i++) {
        if (!PC_Float_Parse(handle, &f))
            return 0;
        ((itemDef_t *)item)->window.borderColor[i] = f;
    }
    return 1;
}

qboolean ItemParse_background(const char (*item)[4], int handle)
{
    const char *temp;
    char name[64];

    if (!PC_String_Parse(handle, &temp))
        return 0;
    I_strncpyz(name, temp, 64);
    I_strlwrInl(name);
    (*(void **)&((itemDef_t *)item)->window.background) = CL_RegisterMaterialNoMip(name, ((itemDef_t *)item)->imageTrack);
    return 1;
}

qboolean ItemParse_doubleClick(const char (*item)[4], int handle)
{
    itemDef_t *itemDef = (itemDef_t *)item;
    listBoxDef_t *listPtr;

    Item_ValidateTypeData(itemDef, handle);
    if (!itemDef->typeData.data)
        return 0;

    listPtr = Item_GetListBoxDef(itemDef);
    if (!listPtr)
        return 0;

    return PC_Script_Parse(handle, &listPtr->doubleClick) != 0;
}

qboolean ItemParse_onFocus(const char (*item)[4], int handle)
{
    return PC_Script_Parse(handle, (const char **)&((itemDef_t *)item)->onFocus) != 0;
}

qboolean ItemParse_leaveFocus(const char (*item)[4], int handle)
{
    return PC_Script_Parse(handle, (const char **)&((itemDef_t *)item)->leaveFocus) != 0;
}

qboolean ItemParse_mouseEnter(const char (*item)[4], int handle)
{
    return PC_Script_Parse(handle, (const char **)&((itemDef_t *)item)->mouseEnter) != 0;
}

qboolean ItemParse_mouseExit(const char (*item)[4], int handle)
{
    return PC_Script_Parse(handle, (const char **)&((itemDef_t *)item)->mouseExit) != 0;
}

qboolean ItemParse_mouseEnterText(const char (*item)[4], int handle)
{
    return PC_Script_Parse(handle, (const char **)&((itemDef_t *)item)->mouseEnterText) != 0;
}

qboolean ItemParse_mouseExitText(const char (*item)[4], int handle)
{
    return PC_Script_Parse(handle, (const char **)&((itemDef_t *)item)->mouseExitText) != 0;
}

qboolean ItemParse_action(itemDef_t *item, int handle)
{
    return PC_Script_Parse(handle, &item->action) != 0;
}

qboolean ItemParse_accept(itemDef_t *item, int handle)
{
    return PC_Script_Parse(handle, &item->onAccept) != 0;
}

qboolean ItemParse_dvarTest(itemDef_t *item, int handle)
{
    if (!PC_String_Parse(handle, &item->dvarTest))
        return 0;
    return 1;
}

qboolean ItemParse_dvar(const char (*item)[4], int handle)
{
    itemDef_t *itemDef = (itemDef_t *)item;
    editFieldDef_t *editPtr;

    Item_ValidateTypeData(itemDef, handle);
    if (!PC_String_Parse(handle, &itemDef->dvar))
        return 0;
    if (itemDef->typeData.data && Item_IsEditFieldDef(itemDef)) {
        editPtr = Item_GetEditFieldDef(itemDef);
        editPtr->minVal = -1.0f;
        editPtr->maxVal = -1.0f;
        editPtr->defVal = -1.0f;
    }
    return 1;
}

qboolean ItemParse_maxChars(const char (*item)[4], int handle)
{
    itemDef_t *itemDef = (itemDef_t *)item;
    editFieldDef_t *editDef;
    int maxChars;

    Item_ValidateTypeData(itemDef, handle);
    if (!itemDef->typeData.data)
        return 0;
    if (!PC_Int_Parse(handle, &maxChars))
        return 0;

    editDef = Item_GetEditFieldDef(itemDef);
    if (!editDef)
        return 0;

    editDef->maxChars = maxChars;
    return 1;
}

qboolean Item_Parse_maxCharsGotoNext(const char (*item)[4], int handle)
{
    itemDef_t *itemDef = (itemDef_t *)item;
    editFieldDef_t *editDef;

    Item_ValidateTypeData(itemDef, handle);
    if (!itemDef->typeData.data)
        return 0;

    editDef = Item_GetEditFieldDef(itemDef);
    if (!editDef)
        return 0;

    editDef->maxCharsGotoNext = 1;
    return 1;
}

qboolean ItemParse_maxPaintChars(const char (*item)[4], int handle)
{
    itemDef_t *itemDef = (itemDef_t *)item;
    editFieldDef_t *editDef;
    int maxPaintChars;

    Item_ValidateTypeData(itemDef, handle);
    if (!itemDef->typeData.data)
        return 0;
    if (!PC_Int_Parse(handle, &maxPaintChars))
        return 0;

    editDef = Item_GetEditFieldDef(itemDef);
    if (!editDef)
        return 0;

    editDef->maxPaintChars = maxPaintChars;
    return 1;
}

qboolean ItemParse_dvarFloat(const char (*item)[4], int handle)
{
    itemDef_t *itemDef = (itemDef_t *)item;
    editFieldDef_t *editPtr;

    Item_ValidateTypeData(itemDef, handle);
    if (!itemDef->typeData.data)
        return 0;
    editPtr = Item_GetEditFieldDef(itemDef);
    if (editPtr && PC_String_Parse(handle, &itemDef->dvar)
        && PC_Float_Parse(handle, &editPtr->defVal)
        && PC_Float_Parse(handle, &editPtr->minVal)
        && PC_Float_Parse(handle, &editPtr->maxVal))
        return 1;
    return 0;
}

qboolean ItemParse_dvarStrList(const char (*item)[4], int handle)
{
    itemDef_t *itemDef = (itemDef_t *)item;
    multiDef_t *multiPtr;
    pc_token_t token;
    int pass;

    Item_ValidateTypeData(itemDef, handle);
    if (!itemDef->typeData.data)
        return 0;
    if (itemDef->type != 12)
        return 0;
    multiPtr = Item_GetMultiDef(itemDef);
    multiPtr->count = 0;
    multiPtr->strDef = 1;

    if (!PC_ReadTokenHandle(handle, &token))
        return 0;
    if (token.string[0] != '{')
        return 0;

    pass = 0;
    while (1) {
        if (!PC_ReadTokenHandle(handle, &token)) {
            PC_SourceError(handle, "end of file inside menu item\n");
            return 0;
        }
        if (token.string[0] == '}')
            return 1;
        if (token.string[0] == ',' || token.string[0] == ';')
            continue;
        if (pass == 0) {
            multiPtr->dvarList[multiPtr->count] = String_Alloc(token.string);
            pass = 1;
        } else {
            multiPtr->dvarStr[multiPtr->count] = String_Alloc(token.string);
            pass = 0;
            multiPtr->count++;
            if (multiPtr->count >= 32)
                return 0;
        }
    }
}

qboolean ItemParse_dvarFloatList(const char (*item)[4], int handle)
{
    itemDef_t *itemDef = (itemDef_t *)item;
    multiDef_t *multiPtr;
    pc_token_t token;

    Item_ValidateTypeData(itemDef, handle);
    if (!itemDef->typeData.data)
        return 0;
    if (itemDef->type != 12)
        return 0;
    multiPtr = Item_GetMultiDef(itemDef);
    multiPtr->count = 0;
    multiPtr->strDef = 0;

    if (!PC_ReadTokenHandle(handle, &token))
        return 0;
    if (token.string[0] != '{')
        return 0;

    while (1) {
        if (!PC_ReadTokenHandle(handle, &token)) {
            PC_SourceError(handle, "end of file inside menu item\n");
            return 0;
        }
        if (token.string[0] == '}')
            return 1;
        if (token.string[0] == ',' || token.string[0] == ';')
            continue;
        multiPtr->dvarList[multiPtr->count] = String_Alloc(token.string);
        if (!PC_Float_Parse(handle, &multiPtr->dvarValue[multiPtr->count]))
            return 0;
        multiPtr->count++;
        if (multiPtr->count >= 32)
            return 0;
    }
}

qboolean ItemParse_dvarEnumList(const char (*item)[4], int handle)
{
    itemDef_t *itemDef = (itemDef_t *)item;

    Item_ValidateTypeData(itemDef, handle);
    if (itemDef->type != 13)
        return 0;
    if (itemDef->typeData.data) {
        PC_SourceError(handle, "enumDvarList already given");
        return 0;
    }
    return PC_String_Parse(handle, (const char **)&itemDef->typeData.data);
}

qboolean ItemParse_enableDvar(const char (*item)[4], int handle)
{
    if (PC_Script_Parse(handle, (const char **)&((itemDef_t *)item)->enableDvar)) {
        ((itemDef_t *)item)->dvarFlags |= 1;
        return 1;
    }
    return 0;
}

qboolean ItemParse_disableDvar(const char (*item)[4], int handle)
{
    if (PC_Script_Parse(handle, (const char **)&((itemDef_t *)item)->enableDvar)) {
        ((itemDef_t *)item)->dvarFlags |= 2;
        return 1;
    }
    return 0;
}

qboolean ItemParse_showDvar(const char (*item)[4], int handle)
{
    if (PC_Script_Parse(handle, (const char **)&((itemDef_t *)item)->enableDvar)) {
        ((itemDef_t *)item)->dvarFlags |= 4;
        return 1;
    }
    return 0;
}

qboolean ItemParse_hideDvar(const char (*item)[4], int handle)
{
    if (PC_Script_Parse(handle, (const char **)&((itemDef_t *)item)->enableDvar)) {
        ((itemDef_t *)item)->dvarFlags |= 8;
        return 1;
    }
    return 0;
}

qboolean ItemParse_focusDvar(const char (*item)[4], int handle)
{
    if (PC_Script_Parse(handle, (const char **)&((itemDef_t *)item)->enableDvar)) {
        ((itemDef_t *)item)->dvarFlags |= 0x10;
        return 1;
    }
    return 0;
}

qboolean ItemParse_execKey(const char (*item)[4], int handle)
{
    itemDef_t *itemDef = (itemDef_t *)item;
    char keyname;
    const char *action;
    ItemKeyHandler *handler;

    if (!PC_Char_Parse(handle, &keyname))
        return 0;
    if (!PC_Script_Parse(handle, &action))
        return 0;

    handler = (ItemKeyHandler *)UI_Alloc(sizeof(ItemKeyHandler), 4);
    handler->key = (unsigned char)keyname;
    handler->action = action;
    handler->next = (intptr_t)itemDef->onKey;
    itemDef->onKey = handler;
    return 1;
}

qboolean ItemParse_execKeyInt(const char (*item)[4], int handle)
{
    itemDef_t *itemDef = (itemDef_t *)item;
    const char *action;
    ItemKeyHandler *handler;
    int keyname;

    if (!PC_Int_Parse(handle, &keyname))
        return 0;
    if (!PC_Script_Parse(handle, &action))
        return 0;

    handler = (ItemKeyHandler *)UI_Alloc(sizeof(ItemKeyHandler), 4);
    handler->key = keyname;
    handler->action = action;
    handler->next = (intptr_t)itemDef->onKey;
    itemDef->onKey = handler;
    return 1;
}

static qboolean UI_ParseAssetGlobalDef(int handle, int imageTrack)
{
    loadAssets_t *load = &g_load.loadAssets;
    pc_token_t token;
    const char *tempStr;
    snd_alias_list_t *alias;

    if (!PC_ReadTokenHandle(handle, &token))
        return 0;
    if (I_stricmp(token.string, "{"))
        return 0;

    while (1) {
        if (!PC_ReadTokenHandle(handle, &token))
            return 0;
        if (!I_stricmp(token.string, "}"))
            return 1;

        if (!I_stricmp(token.string, "font")) {
            int pointSize;
            if (!PC_String_Parse(handle, &tempStr) || !PC_Int_Parse(handle, &pointSize))
                return 0;
            CL_DuplicateFont(CL_RegisterFont(tempStr, imageTrack), "$font");
            continue;
        }

        if (!I_stricmp(token.string, "smallFont")) {
            int pointSize;
            if (!PC_String_Parse(handle, &tempStr) || !PC_Int_Parse(handle, &pointSize))
                return 0;
            CL_DuplicateFont(CL_RegisterFont(tempStr, imageTrack), "$smallfont");
            continue;
        }

        if (!I_stricmp(token.string, "bigFont")) {
            int pointSize;
            if (!PC_String_Parse(handle, &tempStr) || !PC_Int_Parse(handle, &pointSize))
                return 0;
            CL_DuplicateFont(CL_RegisterFont(tempStr, imageTrack), "$bigfont");
            continue;
        }

        if (!I_stricmp(token.string, "extraBigFont")) {
            int pointSize;
            if (!PC_String_Parse(handle, &tempStr) || !PC_Int_Parse(handle, &pointSize))
                return 0;
            CL_DuplicateFont(CL_RegisterFont(tempStr, imageTrack), "$extrabigfont");
            continue;
        }

        if (!I_stricmp(token.string, "boldFont")) {
            int pointSize;
            if (!PC_String_Parse(handle, &tempStr) || !PC_Int_Parse(handle, &pointSize))
                return 0;
            CL_DuplicateFont(CL_RegisterFont(tempStr, imageTrack), "$boldfont");
            continue;
        }

        if (!I_stricmp(token.string, "consoleFont")) {
            int pointSize;
            if (!PC_String_Parse(handle, &tempStr) || !PC_Int_Parse(handle, &pointSize))
                return 0;
            CL_DuplicateFont(CL_RegisterFont(tempStr, imageTrack), "$consolefont");
            continue;
        }

        if (!I_stricmp(token.string, "gradientbar")) {
            if (!PC_String_Parse(handle, &tempStr))
                return 0;
            CL_Material_Duplicate(CL_RegisterMaterialNoMip(tempStr, imageTrack), "$gradientbar");
            continue;
        }

        if (!I_stricmp(token.string, "menuEnterSound")) {
            if (!PC_String_Parse(handle, &tempStr))
                return 0;
            load->menuEnterSound = Com_FindSoundAlias(tempStr);
            continue;
        }

        if (!I_stricmp(token.string, "menuExitSound")) {
            if (!PC_String_Parse(handle, &tempStr))
                return 0;
            load->menuExitSound = Com_FindSoundAlias(tempStr);
            continue;
        }

        if (!I_stricmp(token.string, "itemFocusSound")) {
            if (!PC_String_Parse(handle, &tempStr))
                return 0;
            alias = Com_FindSoundAlias(tempStr);
            if (alias)
                Com_DuplicateSoundAlias(alias, "$itemfocussound");
            continue;
        }

        if (!I_stricmp(token.string, "menuBuzzSound")) {
            if (!PC_String_Parse(handle, &tempStr))
                return 0;
            load->menuBuzzSound = Com_FindSoundAlias(tempStr);
            continue;
        }

        if (!I_stricmp(token.string, "cursor")) {
            if (!PC_String_Parse(handle, &tempStr))
                return 0;
            CL_Material_Duplicate(CL_RegisterMaterialNoMip(tempStr, imageTrack), "$cursor");
            continue;
        }

        if (!I_stricmp(token.string, "fadeClamp")) {
            if (!PC_Float_Parse(handle, &load->fadeClamp))
                return 0;
            continue;
        }

        if (!I_stricmp(token.string, "fadeCycle")) {
            if (!PC_Int_Parse(handle, &load->fadeCycle))
                return 0;
            continue;
        }

        if (!I_stricmp(token.string, "fadeAmount")) {
            if (!PC_Float_Parse(handle, &load->fadeAmount))
                return 0;
            continue;
        }

        if (!I_stricmp(token.string, "fadeInAmount")) {
            if (!PC_Float_Parse(handle, &load->fadeInAmount))
                return 0;
            continue;
        }

        if (!I_stricmp(token.string, "shadowX")) {
            if (!PC_Float_Parse(handle, &load->shadowX))
                return 0;
            continue;
        }

        if (!I_stricmp(token.string, "shadowY")) {
            if (!PC_Float_Parse(handle, &load->shadowY))
                return 0;
            continue;
        }

        if (!I_stricmp(token.string, "shadowColor")) {
            if (!PC_Color_Parse(handle, (float (*)[4])load->shadowColor))
                return 0;
            load->shadowFadeClamp = load->shadowColor[3];
            continue;
        }
    }
}

qboolean Menu_Parse(int handle, menuDef_t *menu)
{
    pc_token_t token;
    keywordHash_t *key;

    if (!PC_ReadTokenHandle(handle, &token))
        return 0;
    if (token.string[0] != '{')
        return 0;
    while (1) {
        memset(&token, 0, sizeof(pc_token_t));
        if (!PC_ReadTokenHandle(handle, &token)) {
            PC_SourceError(handle, "end of file inside menu\n");
            return 0;
        }
        if (token.string[0] == '}')
            return 1;
        key = KeywordHash_Find(menuParseKeywordHash, token.string);
        if (!key) {
            PC_SourceError(handle, "unknown menu keyword %s", token.string);
            continue;
        }
        if (!((qboolean (*)(menuDef_t *, int))key->func)(menu, handle)) {
            PC_SourceError(handle, "couldn't parse menu keyword %s", token.string);
            return 0;
        }
    }
    return 0;
}

void Menu_PostParse(menuDef_t *menu)
{
    int size = ((menuDef_t *)menu)->itemCount * (int)sizeof(itemDef_t *);   /* was *4 (x86 ptr) */
    void *items = UI_Alloc(size, 4);
    (*(void **)&((menuDef_t *)menu)->items) = items;
    memcpy(items, g_load.items, size);
    if (((menuDef_t *)menu)->fullScreen) {
        UiRectangle rect;
        rect = menu->window.rect[0];
        rect.x = 0.0f;
        rect.y = 0.0f;
        rect.w = 640.0f;
        rect.h = 480.0f;
        Window_SetRect((Window *)menu, &rect);
    }
    Menu_UpdatePosition(menu);
}

static bool Menu_New(int handle, int imageTrack)
{
    menuDef_t *menu;
    MenuList *menuList;

    menu = (menuDef_t *)UI_Alloc(sizeof(menuDef_t), 4);
    Menu_Init(menu, imageTrack);
    if (!Menu_Parse(handle, menu))
        return false;
    if (!menu->window.name) {
        PC_SourceError(handle, "menu has no name");
        return false;
    }
    Menu_PostParse(menu);
    menuList = &g_load.menuList;
    if (menuList->menuCount >= 128)
        Com_Error(ERR_DROP, "\x15Menu_New: \x14" "EXE_ERR_OUT_OF_MEMORY");
    menuList->menus[menuList->menuCount++] = menu;
    return true;
}

static Bool __attribute_regparm__(2) UI_ParseMenuInternal(const char *menuFile, int imageTrack)
{
    int handle;
    pc_token_t token;

    Com_Printf("\tLoading '%s'...\n", menuFile);

    handle = PC_LoadSourceHandle(menuFile);
    if (!handle)
        return 0;

    while (PC_ReadTokenHandle(handle, &token) && token.string[0] != '}') {
        if (!I_stricmp(token.string, "assetGlobalDef")) {
            if (!UI_ParseAssetGlobalDef(handle, imageTrack))
                break;
        } else if (!I_stricmp(token.string, "menudef")) {
            if (!Menu_New(handle, imageTrack))
                break;
        }
    }

    PC_FreeSourceHandle(handle);
    return 1;
}

MenuList *UI_LoadMenu(const char *menuFile, int imageTrack)
{
    memset(&g_load, 0, sizeof(g_load));
    g_load.menuList.menus = g_load.menus;

    if (!UI_ParseMenuInternal(menuFile, imageTrack)) {
        Com_Printf("^3WARNING: menu file not found: %s\n", menuFile);
        if (!UI_ParseMenuInternal("ui/default.menu", imageTrack))
            Com_Error(ERR_DROP, "default menu file not found");
    }

    return &g_load.menuList;
}

// name from Q3 ui_main.c
static qboolean Load_Menu(const char **p, int imageTrack)
{
    const char *token;

    token = Com_Parse(p);
    if (token[0] != '{')
        return 0;

    while (1) {
        token = Com_Parse(p);
        if (!I_stricmp(token, "}"))
            return 1;
        if (!token || !token[0])
            return 0;
        UI_ParseMenuInternal(token, imageTrack);
    }
}

// retail inlines the I_stricmp body at both compares in UI_LoadMenus
static __forceinline int I_stricmpInl(const char *s0, const char *s1)
{
    int c0;
    int c1;
    int n = 0x7FFFFFFF;

    do {
        c0 = *s0++;
        c1 = *s1++;
        if (!n--)
            return 0;
        if (c0 != c1) {
            if (I_islower(c0))
                c0 -= 'a' - 'A';
            if (I_islower(c1))
                c1 -= 'a' - 'A';
            if (c0 != c1)
                return c0 < c1 ? -1 : 1;
        }
    } while (c0);

    return 0;
}

MenuList *UI_LoadMenus(const char *menuFile, int imageTrack)
{
    const char *p;
    const char *token;
    fileHandle_t f;
    int len;

    memset(&g_load, 0, sizeof(g_load));
    g_load.menuList.menus = g_load.menus;

    len = FS_FOpenFileByMode(menuFile, &f, FS_READ);
    if (!f) {
        Com_Printf("^3WARNING: menu file not found: %s\n", menuFile);
        len = FS_FOpenFileByMode("ui/default.menu", &f, FS_READ);
        if (!f)
            Com_Error(ERR_DROP, "\x15" "default.menu file not found. This is a default menu that you should have.\n");
    }

    if (len >= 0x8000) {
        FS_FCloseFile(f);
        Com_Error(ERR_DROP, "\x15^1menu file too large: %s is %i, max allowed is %i", menuFile, len, 0x8000);
    }

    FS_Read(menuBuf, len, f);
    menuBuf[len] = 0;
    FS_FCloseFile(f);
    Com_Compress(menuBuf);

    p = menuBuf;
    Com_BeginParseSession(menuFile);

    while (1) {
        token = Com_Parse(&p);
        if (!token || !token[0] || token[0] == '}')
            break;
        if (!I_stricmpInl(token, "}"))
            break;
        if (!I_stricmpInl(token, "loadmenu")) {
            if (Load_Menu(&p, imageTrack))
                continue;
            break;
        }
    }

    Com_EndParseSessionInl();
    return &g_load.menuList;
}

qboolean PC_Int_Parse(int handle, int *i)
{
    pc_token_t token;
    int negative;

    if (!PC_ReadTokenHandle(handle, &token))
        return 0;
    negative = 0;
    if (token.string[0] == '-') {
        if (!PC_ReadTokenHandle(handle, &token))
            return 0;
        negative = 1;
    }
    if (token.type != 3) {
        PC_SourceError(handle, "expected integer but found %s\n", token.string);
        return 0;
    }
    *i = token.intvalue;
    if (negative)
        *i = -*i;
    return 1;
}
qboolean ItemParse_name(itemDef_t *item, int handle)
{
    pc_token_t token;

    if (!PC_ReadTokenHandle(handle, &token))
        return 0;
    item->window.name = String_Alloc(token.string);
    return 1;
}
qboolean MenuParse_borderSize(const char (*item)[4], int handle)
{
    if (!PC_Float_Parse(handle, (float *)&((itemDef_t *)item)->window.borderSize))
        return 0;
    return 1;
}
qboolean MenuParse_visible(const char (*item)[4], int handle)
{
    int visible;

    if (!PC_Int_Parse(handle, &visible))
        return 0;

    if (visible)
        ((itemDef_t *)item)->window.dynamicFlags[0] |= 4;

    return 1;
}
qboolean MenuParse_border(const char (*item)[4], int handle)
{
    if (!PC_Int_Parse(handle, (int *)&((itemDef_t *)item)->window.border))
        return 0;
    return 1;
}
qboolean MenuParse_ownerdrawFlag(const char (*item)[4], int handle)
{
    int flags;

    if (!PC_Int_Parse(handle, &flags))
        return 0;

    ((itemDef_t *)item)->window.ownerDrawFlags |= flags;
    return 1;
}
qboolean MenuParse_forecolor(const char (*item)[4], int handle)
{
    int i;
    float f;

    for (i = 0; i < 4; i++) {
        if (!PC_Float_Parse(handle, &f))
            return 0;
        ((itemDef_t *)item)->window.foreColor[i] = f;
        ((itemDef_t *)item)->window.dynamicFlags[0] |= 0x10000;
    }
    return 1;
}
qboolean MenuParse_cinematic(menuDef_t *menu, int handle)
{
    if (!PC_String_Parse(handle, &menu->window.cinematicName))
        return 0;
    return 1;
}
qboolean ItemParse_cinematic(itemDef_t *item, int handle)
{
    if (!PC_String_Parse(handle, &item->window.cinematicName))
        return 0;
    return 1;
}
qboolean ItemParse_ownerdrawFlag(const char (*item)[4], int handle)
{
    int flags;

    if (!PC_Int_Parse(handle, &flags))
        return 0;

    ((itemDef_t *)item)->window.ownerDrawFlags |= flags;
    return 1;
}
void Menu_SetupKeywordHash(void)
{
    int i;

    memset(menuParseKeywordHash, 0, sizeof(menuParseKeywordHash));
    for (i = 0; menuParseKeywords[i].keyword; i++)
        KeywordHash_Add(menuParseKeywordHash, &menuParseKeywords[i]);
}
qboolean ItemParse_origin(const char (*item)[4], int handle)
{
    UiRectangle newRect;
    int x, y;

    if (!PC_Int_Parse(handle, &x))
        return 0;
    if (!PC_Int_Parse(handle, &y))
        return 0;
    newRect = ((Window *)item)->rectClient[0];
    newRect.x = x + newRect.x;
    newRect.y = y + newRect.y;
    Window_SetRectClient((Window *)item, &newRect);
    return 1;
}
qboolean ItemParse_feeder(const char (*item)[4], int handle)
{
    if (!PC_Float_Parse(handle, (float *)&((itemDef_t *)item)->special))
        return 0;
    return 1;
}
qboolean ItemParse_bordersize(const char (*item)[4], int handle)
{
    if (!PC_Float_Parse(handle, (float *)&((itemDef_t *)item)->window.borderSize))
        return 0;
    return 1;
}
qboolean ItemParse_border(const char (*item)[4], int handle)
{
    if (!PC_Int_Parse(handle, (int *)&((itemDef_t *)item)->window.border))
        return 0;
    return 1;
}
qboolean ItemParse_visible(const char (*item)[4], int handle)
{
    int visible;

    if (!PC_Int_Parse(handle, &visible))
        return 0;
    if (visible)
        ((itemDef_t *)item)->window.dynamicFlags[0] |= 4;
    return 1;
}
qboolean ItemParse_forecolor(const char (*item)[4], int handle)
{
    int i;
    float f;

    for (i = 0; i < 4; i++) {
        if (!PC_Float_Parse(handle, &f))
            return 0;
        ((itemDef_t *)item)->window.foreColor[i] = f;
        ((itemDef_t *)item)->window.dynamicFlags[0] |= 0x10000;
    }
    return 1;
}
qboolean ItemParse_special(const char (*item)[4], int handle)
{
    if (!PC_Float_Parse(handle, (float *)&((itemDef_t *)item)->special))
        return 0;
    return 1;
}
qboolean ItemParse_outlinecolor(const char (*item)[4], int handle)
{
    if (!PC_Color_Parse(handle, &((itemDef_t *)item)->window.outlineColor))
        return 0;
    return 1;
}
qboolean MenuParse_outlinecolor(const char (*item)[4], int handle)
{
    if (!PC_Color_Parse(handle, &((itemDef_t *)item)->window.outlineColor))
        return 0;
    return 1;
}
void Item_SetupKeywordHash(void)
{
    int i;

    memset(itemParseKeywordHash, 0, sizeof(itemParseKeywordHash));
    for (i = 0; itemParseKeywords[i].keyword; i++)
        KeywordHash_Add(itemParseKeywordHash, &itemParseKeywords[i]);
}
qboolean MenuParse_name(menuDef_t *menu, int handle)
{
    pc_token_t token;

    if (!PC_ReadTokenHandle(handle, &token))
        return 0;
    menu->window.name = String_Alloc(token.string);
    return 1;
}
static keywordHash_t menuParseKeywords[] = {
    { (char *)"name", (qboolean(*)())&MenuParse_name, 0x0 },
    { (char *)"fullscreen", (qboolean(*)())&MenuParse_fullscreen, 0x0 },
    { (char *)"rect", (qboolean(*)())&MenuParse_rect, 0x0 },
    { (char *)"style", (qboolean(*)())&MenuParse_style, 0x0 },
    { (char *)"visible", (qboolean(*)())&MenuParse_visible, 0x0 },
    { (char *)"onOpen", (qboolean(*)())&MenuParse_onOpen, 0x0 },
    { (char *)"onClose", (qboolean(*)())&MenuParse_onClose, 0x0 },
    { (char *)"onESC", (qboolean(*)())&MenuParse_onESC, 0x0 },
    { (char *)"border", (qboolean(*)())&MenuParse_border, 0x0 },
    { (char *)"borderSize", (qboolean(*)())&MenuParse_borderSize, 0x0 },
    { (char *)"backcolor", (qboolean(*)())&MenuParse_backcolor, 0x0 },
    { (char *)"forecolor", (qboolean(*)())&MenuParse_forecolor, 0x0 },
    { (char *)"bordercolor", (qboolean(*)())&MenuParse_bordercolor, 0x0 },
    { (char *)"focuscolor", (qboolean(*)())&MenuParse_focuscolor, 0x0 },
    { (char *)"disablecolor", (qboolean(*)())&MenuParse_disablecolor, 0x0 },
    { (char *)"outlinecolor", (qboolean(*)())&MenuParse_outlinecolor, 0x0 },
    { (char *)"background", (qboolean(*)())&MenuParse_background, 0x0 },
    { (char *)"ownerdraw", (qboolean(*)())&MenuParse_ownerdraw, 0x0 },
    { (char *)"ownerdrawFlag", (qboolean(*)())&MenuParse_ownerdrawFlag, 0x0 },
    { (char *)"outOfBoundsClick", (qboolean(*)())&MenuParse_outOfBounds, 0x0 },
    { (char *)"soundLoop", (qboolean(*)())&MenuParse_soundLoop, 0x0 },
    { (char *)"itemDef", (qboolean(*)())&MenuParse_itemDef, 0x0 },
    { (char *)"cinematic", (qboolean(*)())&MenuParse_cinematic, 0x0 },
    { (char *)"popup", (qboolean(*)())&MenuParse_popup, 0x0 },
    { (char *)"fadeClamp", (qboolean(*)())&MenuParse_fadeClamp, 0x0 },
    { (char *)"fadeCycle", (qboolean(*)())&MenuParse_fadeCycle, 0x0 },
    { (char *)"fadeAmount", (qboolean(*)())&MenuParse_fadeAmount, 0x0 },
    { (char *)"fadeInAmount", (qboolean(*)())&MenuParse_fadeInAmount, 0x0 },
    { (char *)"execKey", (qboolean(*)())&MenuParse_execKey, 0x0 },
    { (char *)"execKeyInt", (qboolean(*)())&MenuParse_execKeyInt, 0x0 },
    { (char *)"blurWorld", (qboolean(*)())&MenuParse_blurWorld, 0x0 },
    { (char *)"legacySplitScreenScale", (qboolean(*)())&MenuParse_legacySplitScreenScale, 0x0 },
    { 0, 0, 0x0 }
};
static keywordHash_t itemParseKeywords[] = {
    { (char *)"name", (qboolean(*)())&ItemParse_name, 0x0 },
    { (char *)"text", (qboolean(*)())&ItemParse_text, 0x0 },
    { (char *)"textfile", (qboolean(*)())&ItemParse_textfile, 0x0 },
    { (char *)"textsavegame", (qboolean(*)())&ItemParse_textsavegame, 0x0 },
    { (char *)"group", (qboolean(*)())&ItemParse_group, 0x0 },
    { (char *)"rect", (qboolean(*)())&ItemParse_rect, 0x0 },
    { (char *)"origin", (qboolean(*)())&ItemParse_origin, 0x0 },
    { (char *)"style", (qboolean(*)())&ItemParse_style, 0x0 },
    { (char *)"decoration", (qboolean(*)())&ItemParse_decoration, 0x0 },
    { (char *)"notselectable", (qboolean(*)())&ItemParse_notselectable, 0x0 },
    { (char *)"noscrollbars", (qboolean(*)())&ItemParse_noScrollBars, 0x0 },
    { (char *)"wrapped", (qboolean(*)())&ItemParse_wrapped, 0x0 },
    { (char *)"autowrapped", (qboolean(*)())&ItemParse_autowrapped, 0x0 },
    { (char *)"horizontalscroll", (qboolean(*)())&ItemParse_horizontalscroll, 0x0 },
    { (char *)"type", (qboolean(*)())&ItemParse_type, 0x0 },
    { (char *)"elementwidth", (qboolean(*)())&ItemParse_elementwidth, 0x0 },
    { (char *)"elementheight", (qboolean(*)())&ItemParse_elementheight, 0x0 },
    { (char *)"feeder", (qboolean(*)())&ItemParse_feeder, 0x0 },
    { (char *)"elementtype", (qboolean(*)())&ItemParse_elementtype, 0x0 },
    { (char *)"columns", (qboolean(*)())&ItemParse_columns, 0x0 },
    { (char *)"border", (qboolean(*)())&ItemParse_border, 0x0 },
    { (char *)"bordersize", (qboolean(*)())&ItemParse_bordersize, 0x0 },
    { (char *)"visible", (qboolean(*)())&ItemParse_visible, 0x0 },
    { (char *)"ownerdraw", (qboolean(*)())&ItemParse_ownerdraw, 0x0 },
    { (char *)"align", (qboolean(*)())&ItemParse_align, 0x0 },
    { (char *)"textalign", (qboolean(*)())&ItemParse_textalign, 0x0 },
    { (char *)"textalignx", (qboolean(*)())&ItemParse_textalignx, 0x0 },
    { (char *)"textaligny", (qboolean(*)())&ItemParse_textaligny, 0x0 },
    { (char *)"textscale", (qboolean(*)())&ItemParse_textscale, 0x0 },
    { (char *)"textstyle", (qboolean(*)())&ItemParse_textstyle, 0x0 },
    { (char *)"textfont", (qboolean(*)())&ItemParse_textfont, 0x0 },
    { (char *)"backcolor", (qboolean(*)())&ItemParse_backcolor, 0x0 },
    { (char *)"forecolor", (qboolean(*)())&ItemParse_forecolor, 0x0 },
    { (char *)"bordercolor", (qboolean(*)())&ItemParse_bordercolor, 0x0 },
    { (char *)"outlinecolor", (qboolean(*)())&ItemParse_outlinecolor, 0x0 },
    { (char *)"background", (qboolean(*)())&ItemParse_background, 0x0 },
    { (char *)"onFocus", (qboolean(*)())&ItemParse_onFocus, 0x0 },
    { (char *)"leaveFocus", (qboolean(*)())&ItemParse_leaveFocus, 0x0 },
    { (char *)"mouseEnter", (qboolean(*)())&ItemParse_mouseEnter, 0x0 },
    { (char *)"mouseExit", (qboolean(*)())&ItemParse_mouseExit, 0x0 },
    { (char *)"mouseEnterText", (qboolean(*)())&ItemParse_mouseEnterText, 0x0 },
    { (char *)"mouseExitText", (qboolean(*)())&ItemParse_mouseExitText, 0x0 },
    { (char *)"action", (qboolean(*)())&ItemParse_action, 0x0 },
    { (char *)"accept", (qboolean(*)())&ItemParse_accept, 0x0 },
    { (char *)"special", (qboolean(*)())&ItemParse_special, 0x0 },
    { (char *)"dvar", (qboolean(*)())&ItemParse_dvar, 0x0 },
    { (char *)"maxChars", (qboolean(*)())&ItemParse_maxChars, 0x0 },
    { (char *)"maxCharsGotoNext", (qboolean(*)())&Item_Parse_maxCharsGotoNext, 0x0 },
    { (char *)"maxPaintChars", (qboolean(*)())&ItemParse_maxPaintChars, 0x0 },
    { (char *)"focusSound", (qboolean(*)())&ItemParse_focusSound, 0x0 },
    { (char *)"dvarFloat", (qboolean(*)())&ItemParse_dvarFloat, 0x0 },
    { (char *)"dvarStrList", (qboolean(*)())&ItemParse_dvarStrList, 0x0 },
    { (char *)"dvarFloatList", (qboolean(*)())&ItemParse_dvarFloatList, 0x0 },
    { (char *)"dvarEnumList", (qboolean(*)())&ItemParse_dvarEnumList, 0x0 },
    { (char *)"ownerdrawFlag", (qboolean(*)())&ItemParse_ownerdrawFlag, 0x0 },
    { (char *)"enableDvar", (qboolean(*)())&ItemParse_enableDvar, 0x0 },
    { (char *)"dvarTest", (qboolean(*)())&ItemParse_dvarTest, 0x0 },
    { (char *)"disableDvar", (qboolean(*)())&ItemParse_disableDvar, 0x0 },
    { (char *)"showDvar", (qboolean(*)())&ItemParse_showDvar, 0x0 },
    { (char *)"hideDvar", (qboolean(*)())&ItemParse_hideDvar, 0x0 },
    { (char *)"focusDvar", (qboolean(*)())&ItemParse_focusDvar, 0x0 },
    { (char *)"cinematic", (qboolean(*)())&ItemParse_cinematic, 0x0 },
    { (char *)"doubleclick", (qboolean(*)())&ItemParse_doubleClick, 0x0 },
    { (char *)"execKey", (qboolean(*)())&ItemParse_execKey, 0x0 },
    { (char *)"execKeyInt", (qboolean(*)())&ItemParse_execKeyInt, 0x0 },
    { 0, 0, 0x0 }
};

