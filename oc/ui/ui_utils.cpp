#include <ctype.h>
#include "common_types.h"
#include "imports.h"
#include <string.h>
#include <stdlib.h>

extern const char *Dvar_GetVariantString(const char *dvarName);
extern void I_strncat(char *dest, int destSize, const char *src);
extern int I_stricmp(const char *s1, const char *s2);
extern qboolean String_Parse(const char **p, char *out, int outSize);
extern char *Com_ParseOnLine( const char *( *data_p ) );
extern void *Hunk_AllocAlignInternal(int size, int alignment);
extern void *Hunk_AllocLowAlignInternal(int size, int alignment);
extern void Com_Printf(const char *fmt, ...);

/* The menu string pool must persist for the whole UI lifetime. On x64 it was
 * being allocated from the HIGH hunk, which the menu-load path churns (temp
 * allocations / mark rollbacks), so freshly-added stringDef nodes had their
 * 8-byte str field clobbered and later lookups crashed. Put the pool in the
 * LOW permanent hunk on x64 (x86 keeps its original high-hunk behavior). */
#if defined(COD2_X64)
#define STRPOOL_ALLOC(sz, al) Hunk_AllocLowAlignInternal((sz), (al))
#else
#define STRPOOL_ALLOC(sz, al) Hunk_AllocAlignInternal((sz), (al))
#endif

static stringDef_t *g_strHandle[2048];

void Window_SetStaticFlags(Window *w, const int flags);
qboolean Item_EnableShowViaDvar(const itemDef_t *item, int flag);
qboolean Item_IsEditFieldDef(itemDef_t *item);
listBoxDef_t *Item_GetListBoxDef(itemDef_t *item);
multiDef_t *Item_GetMultiDef(itemDef_t *item);
void *UI_Alloc(int size, int alignment);
void String_Init(void);
qboolean Int_Parse(const char **p, int *i);
qboolean Float_Parse(const char **p, float *f);
void Window_SetOffsetTime(Window *w, int offsetTime);
void Window_SetRect(Window *w, const UiRectangle *rect);
void Window_SetRectClient(Window *w, const UiRectangle *rectClient);
void Window_SetRectEffects0(Window *w, const UiRectangle *rectEffects0);
void Window_SetRectEffects1(Window *w, const UiRectangle *rectEffects1);
void Window_SetDynamicFlags(Window *w, const int flags);
void Menu_SetCursorItem(menuDef_t *menu, int cursorItem);
bool Window_IsVisible(Window *w);
void Item_SetTextRect(itemDef_t *item, const rectDef_t *textRect);
void Item_SetCursorPos(itemDef_t *item, int cursorPos);
int Item_GetCursorPosOffset(const itemDef_t *item, const char *text, int delta);
void ListBox_SetCursorPos(listBoxDef_t *listBox, int cursorPos);
void ListBox_SetStartPos(listBoxDef_t *listBox, int startPos);
void ListBox_SetEndPos(listBoxDef_t *listBox, int endPos);
Bool ListBox_HasValidCursorPos(const listBoxDef_t *listBox);
void Window_AddDynamicFlags(Window *w, const int newFlags);
void Window_RemoveDynamicFlags(Window *w, const int newFlags);
editFieldDef_t *Item_GetEditFieldDef(itemDef_t *item);
const char *String_Alloc(const char *p);
void Item_SetScreenCoords(itemDef_t *item, float x, float y, int horzAlign, int vertAlign);
void Menu_UpdatePosition(menuDef_t *menu);
qboolean Rect_Parse(const char **p, rectDef_t *r);

// Q_IsColorString plus the CoD2 digit check.
#define I_IsColorString(p) ((p) && *(p) == '^' && *((p) + 1) && *((p) + 1) != '^' && *((p) + 1) >= '0' && *((p) + 1) <= '9')

void Window_SetRectClient(Window *w, const UiRectangle *rectClient)
{
    w->rectClient[0] = *rectClient;
}

void Window_SetRectEffects0(Window *w, const UiRectangle *rectEffects0)
{
    w->rectEffects0[0] = *rectEffects0;
}

void Window_SetRectEffects1(Window *w, const UiRectangle *rectEffects1)
{
    w->rectEffects1[0] = *rectEffects1;
}

void Window_SetDynamicFlags(Window *w, const int flags)
{
    w->dynamicFlags[0] = flags;
}

void Menu_SetCursorItem(menuDef_t *menu, int cursorItem)
{
    menu->cursorItem[0] = cursorItem;
}

void Window_RemoveDynamicFlags(Window *w, const int newFlags)
{
    int flags = w->dynamicFlags[0];
    int mask = newFlags;

    if (mask & 4)
        mask |= 2;

    flags &= ~mask;
    w->dynamicFlags[0] = flags;
}

// Retail inlines this into Menu_HasItemLeft/Right.
static __forceinline int Item_CanFocusInl(const itemDef_t *item)
{
    if (!item || (item->window.staticFlags & 0x100000) || !Window_IsVisible((Window *)&item->window))
        return 0;
    if ((item->dvarFlags & 3) && !Item_EnableShowViaDvar(item, 1))
        return 0;
    if ((item->dvarFlags & 0xc) && !Item_EnableShowViaDvar(item, 4))
        return 0;
    return 1;
}


extern uiInfo_t *uiInfo;

// retail 0x540670, unreferenced; name unknown: cursor inside rect
qboolean UI_CursorInRect_540670(int x, int y, int w, int h)
{
    if (uiInfo->uiDC.cursorx < x || uiInfo->uiDC.cursory < y
        || uiInfo->uiDC.cursorx > x + w || uiInfo->uiDC.cursory > y + h)
        return 0;
    return 1;
}

// retail 0x5407e0, unreferenced, name unknown: item to the left of the cursor item
bool Menu_HasItemLeft_5407e0(menuDef_t *menu)
{
    const itemDef_t *cur;
    const itemDef_t *item;
    float y;
    int i;
    int cursorItem;

    cursorItem = menu->cursorItem[0];
    if (cursorItem < 0 || cursorItem >= menu->itemCount)
        return 0;
    cur = menu->items[cursorItem];
    y = cur->window.rect[0].h * 0.5f + cur->window.rect[0].y;
    for (i = cursorItem - 1; i >= 0; i--) {
        if (!Item_CanFocusInl(menu->items[i]))
            continue;
        item = menu->items[i];
        if (y > item->window.rect[0].y
            && item->window.rect[0].h + item->window.rect[0].y > y
            && item->window.rect[0].x < cur->window.rect[0].x)
            return 1;
        return 0;
    }
    return 0;
}

// retail 0x5408c0, unreferenced, name unknown: item to the right of the cursor item
bool Menu_HasItemRight_5408c0(menuDef_t *menu)
{
    const itemDef_t *cur;
    const itemDef_t *item;
    float y;
    int i;
    int cursorItem;

    cursorItem = menu->cursorItem[0];
    if (cursorItem < 0 || cursorItem >= menu->itemCount)
        return 0;
    cur = menu->items[cursorItem];
    y = cur->window.rect[0].h * 0.5f + cur->window.rect[0].y;
    for (i = cursorItem + 1; i < menu->itemCount; i++) {
        if (!Item_CanFocusInl(menu->items[i]))
            continue;
        item = menu->items[i];
        if (y > item->window.rect[0].y
            && item->window.rect[0].h + item->window.rect[0].y > y
            && item->window.rect[0].x > cur->window.rect[0].x)
            return 1;
        return 0;
    }
    return 0;
}

// retail 0x5409b0, unreferenced, name unknown (Item_SetFocus checks)
int Item_CanFocus_5409b0(const itemDef_t *item)
{
    return Item_CanFocusInl(item);
}

qboolean Item_EnableShowViaDvar(const itemDef_t *item, int flag)
{
    char script[1024];
    char val[1024];
    const char *testValue;
    const char *p;

    memset(script, 0, 1024);

    if (item && item->enableDvar && *item->enableDvar && item->dvarTest && *item->dvarTest) {
        testValue = Dvar_GetVariantString(item->dvarTest);

        I_strncat(script, 1024, item->enableDvar);
        p = script;

        while (String_Parse(&p, val, 1024)) {
            if (val[0] == ';' && val[1] == '\0')
                continue;

            if (item->dvarFlags & flag) {
                if (I_stricmp(testValue, val) == 0)
                    return 1;
            } else {
                if (I_stricmp(testValue, val) == 0)
                    return 0;
            }
        }

        return (item->dvarFlags & flag) == 0;
    }

    return 1;
}

void Item_SetTextRect(itemDef_t *item, const rectDef_t *textRect)
{
    item->textRect[0] = *textRect;
}

void Item_SetCursorPos(itemDef_t *item, int cursorPos)
{
    item->cursorPos[0] = cursorPos;
}

int Item_GetCursorPosOffset(const itemDef_t *item, const char *text, int delta)
{
    int pos;

    pos = item->cursorPos[0];
    if (delta <= 0) {
        while (pos) {
            if (!delta)
                break;
            if (pos >= 2 && I_IsColorString(&text[pos - 2])) {
                pos -= 2;
            } else {
                pos--;
                delta++;
            }
        }
    } else {
        while (1) {
            while (I_IsColorString(&text[pos]))
                pos += 2;
            if (!text[pos] || !delta)
                break;
            pos++;
            delta--;
        }
    }

    return pos;
}

void ListBox_SetCursorPos(listBoxDef_t *listBox, int cursorPos)
{
    listBox->cursorPos[0] = cursorPos;
}

void ListBox_SetStartPos(listBoxDef_t *listBox, int startPos)
{
    listBox->startPos[0] = startPos;
}

void ListBox_SetEndPos(listBoxDef_t *listBox, int endPos)
{
    listBox->endPos[0] = endPos;
}

// retail 0x540c30
Bool ListBox_HasValidCursorPos(const listBoxDef_t *listBox)
{
    if (listBox->cursorPos[0] >= listBox->endPos[0])
        return false;
    return listBox->cursorPos[0] >= listBox->startPos[0];
}

void Window_AddDynamicFlags(Window *w, const int newFlags)
{
    w->dynamicFlags[0] |= newFlags;
}

void Item_SetScreenCoords(itemDef_t *item, float x, float y, int horzAlign, int vertAlign)
{
    rectDef_t newRect;
    const rectDef_t *textRect;

    if (item == NULL)
        return;

    if (item->window.border != 0) {
        x += item->window.borderSize;
        y += item->window.borderSize;
    }

    newRect = item->window.rect[0];
    newRect.x = item->window.rectClient[0].x + x;
    newRect.y = item->window.rectClient[0].y + y;
    newRect.w = item->window.rectClient[0].w;
    newRect.h = item->window.rectClient[0].h;

    if (item->window.rectClient[0].horzAlign != 0 || item->window.rectClient[0].vertAlign != 0) {
        newRect.horzAlign = item->window.rectClient[0].horzAlign;
        newRect.vertAlign = item->window.rectClient[0].vertAlign;
    } else {
        newRect.horzAlign = horzAlign;
        newRect.vertAlign = vertAlign;
    }
    item->window.rect[0] = newRect;

    textRect = &item->textRect[0];
    newRect = *textRect;
    newRect.w = 0;
    newRect.h = 0;
    Item_SetTextRect(item, &newRect);
}

void Menu_UpdatePosition(menuDef_t *menu)
{
    float x, y;
    int i;

    if (menu == NULL)
        return;

    x = menu->window.rect[0].x;
    y = menu->window.rect[0].y;

    if (menu->window.border != 0) {
        x += menu->window.borderSize;
        y += menu->window.borderSize;
    }

    for (i = 0; i < menu->itemCount; i++) {
        Item_SetScreenCoords(menu->items[i], x, y,
                             menu->window.rect[0].horzAlign, menu->window.rect[0].vertAlign);
    }
}

qboolean Item_IsEditFieldDef(itemDef_t *item)
{
    switch (item->dataType) {
    case 0:
    case 4:
    case 9:
    case 10:
    case 11:
    case 14:
    case 16:
    case 17:
    case 18:
        return 1;
    default:
        return 0;
    }
}

listBoxDef_t *Item_GetListBoxDef(itemDef_t *item)
{
    if (item->dataType == 6)
        return item->typeData.listBox;
    Com_Printf("^1Menu Error: Expecting type: ITEM_TYPE_LISTBOX\n");
    return NULL;
}

multiDef_t *Item_GetMultiDef(itemDef_t *item)
{
    return item->typeData.multi;
}

editFieldDef_t *Item_GetEditFieldDef(itemDef_t *item)
{

    switch (item->dataType) {
    case 0:
    case 4:
    case 9:
    case 10:
    case 11:
    case 14:
    case 16:
    case 17:
    case 18:
        return item->typeData.editField;
    }

    Com_Printf("^1Menu Error: Expecting type: ITEM_TYPE_EDITFIELD, ITEM_TYPE_NUMERICFIELD, ITEM_TYPE_DECIMALFIELD, ITEM_TYPE_VALIDFILEFIELD, ITEM_TYPE_UPREDITFIELD, ITEM_TYPE_YESNO, ITEM_TYPE_BIND, ITEM_TYPE_SLIDER, or ITEM_TYPE_TEXT\n");
    return NULL;
}

static long hashForString(const char *str)
{
    int i;
    int letter;
    long hash = 0;

    for (i = 0; str[i] != '\0'; ++i) {
        letter = tolower(str[i]);
        hash += (long)letter * (i + 119);
    }

    return hash & 2047;
}

void Window_SetStaticFlags(Window *w, const int flags)
{
    w->staticFlags = flags;
}

void *UI_Alloc(int size, int alignment)
{
    return Hunk_AllocAlignInternal(size, alignment);
}

void String_Init(void)
{
    memset(g_strHandle, 0, sizeof(g_strHandle));
}

const char *String_Alloc(const char *p)
{
    static const char *staticNULL = "";
    long hash;
    char *str;
    stringDef_t *s;
    stringDef_t *last;
    stringDef_t *newDef;

    if (p == NULL)
        return NULL;

    if (*p == '\0')
        return staticNULL;

    hash = hashForString(p);

    s = g_strHandle[hash];
    while (s != NULL) {
        if (strcmp(p, s->str) == 0) {   /* was *(char**)(s+4): x86 stringDef_t.str offset */
            return s->str;
        }
        s = s->next;
    }

    str = (char *)STRPOOL_ALLOC(strlen(p) + 1, 1);
    strcpy(str, p);

    s = g_strHandle[hash];
    last = s;
    while (s != NULL && s->next) {
        last = s;
        s = s->next;
    }

    newDef = (stringDef_t *)STRPOOL_ALLOC((int)sizeof(stringDef_t), (int)sizeof(void *));
    newDef->next = 0;
    newDef->str = str;

    if (last != NULL) {
        last->next = newDef;
    } else {
        g_strHandle[hash] = newDef;
    }

    return str;
}

qboolean Int_Parse(const char **p, int *i)
{
    const char *token;

    token = Com_ParseOnLine(p);
    if (!token || *token == '\0')
        return 0;

    *i = atoi(token);
    return 1;
}

qboolean Float_Parse(const char **p, float *f)
{
    const char *token;

    token = Com_ParseOnLine(p);
    if (!token || *token == '\0')
        return 0;

    *f = (float)atof(token);
    return 1;
}

void Window_SetOffsetTime(Window *w, int offsetTime)
{
    w->offsetTime[0] = offsetTime;
}

void Window_SetRect(Window *w, const UiRectangle *rect)
{
    w->rect[0] = *rect;
}

qboolean Rect_Parse(const char **p, rectDef_t *r)
{
    const char *token;

    token = Com_ParseOnLine(p);
    if (!token || *token == '\0')
        return 0;
    r->x = (float)atof(token);

    token = Com_ParseOnLine(p);
    if (!token || *token == '\0')
        return 0;
    r->y = (float)atof(token);

    token = Com_ParseOnLine(p);
    if (!token || *token == '\0')
        return 0;
    r->w = (float)atof(token);

    token = Com_ParseOnLine(p);
    if (!token || *token == '\0')
        return 0;
    r->h = (float)atof(token);

    return 1;
}

