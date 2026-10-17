#include "common_types.h"
#include "imports.h"
#include "bytematch.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>

extern dvar_t *com_developer;
extern clientStatic_t cls;
extern sharedUiInfo_t sharedUiInfo;
extern qkey_t *keys;

extern UI_Component_data_t __ZN12UI_Component1gE;
#define UI_Component_g __ZN12UI_Component1gE
extern refexport_t re;
extern void *imp_com_developer;

void Con_ToggleConsole_f(void);

// Key numbers (KisakCOD keycodes).
#define UIC_K_UPARROW       154
#define UIC_K_DOWNARROW     155
#define UIC_K_LEFTARROW     156
#define UIC_K_RIGHTARROW    157
#define UIC_K_ALT           158
#define UIC_K_CTRL          159
#define UIC_K_SHIFT         160
#define UIC_K_PGDN          163
#define UIC_K_PGUP          164
#define UIC_K_HOME          165
#define UIC_K_END           166
#define UIC_K_MOUSE1        200
#define UIC_K_MWHEELDOWN    205
#define UIC_K_MWHEELUP      206

struct UI_Component
{
    virtual void Init();
    virtual void Shutdown();
    virtual ~UI_Component();
    virtual void Draw(float x, float y, float width, float height, float compX, float compY) = 0;
    virtual bool KeyEvent(float *point, int key) = 0;
    virtual UI_Component *GetCompAtLocation(float *point);
    virtual void AddText(const char *text);

    float size[2];
    UI_Component *selectionParent;

    static MaterialHandle RegisterMaterialNoMip(const char *name, int imageTrack);
    static void DrawPic(float x, float y, float width, float height, float *color, MaterialHandle material);
    static void DrawPicRotate(float x, float y, float width, float height, float *color, MaterialHandle material);
    static void DrawText(float x, float y, float width, int fontEnum, const float *color, const char *text);
    static void DrawCursor();
    static void MouseEvent(int x, int y);
    static void GetCursorPos(int *x, int *y);
};

struct UI_LinesComponent : UI_Component
{
    virtual void Init();
    virtual bool SetSelectedLineFocus(int newSelectedLine, bool user);
    virtual bool KeyEvent(float *point, int key);
    virtual void AddText(const char *text);

    void IncSelectedLineFocus(bool wrap);
    void DecSelectedLineFocus(bool wrap);
    void UpdateHeight();
    void ClearFocus();

    int selectedLine;
    bool focusOnSelectedLine;
    bool focusOnSelectedLineUser;
    int numLines;
    float pos[2];
};

struct UI_ScrollPane : UI_Component
{
    virtual void Init();
    virtual void Draw(float x, float y, float width, float height, float compX, float compY);
    virtual bool KeyEvent(float *point, int key);
    virtual UI_Component *GetCompAtLocation(float *point);
    virtual void AddText(const char *text);

    bool GetInnerSize(float *innerSize);
    int GetInnerLinesCount();
    void SetPos();
    int GetFirstDisplayedLine();
    int GetLastDisplayedLine();
    void DisplaySelectedLine();
    void CheckMouseScroll(int index, float *thumbPos, float *thumbSize, float thumbMaxSize);

    UI_LinesComponent *comp;
    bool forceHorScoll;
    float mouseHeldScale[2];
    float mouseHeldPos[2];
    float mouseHeldCompPos[2];
    bool mouseWasDown[2];
};

struct UI_VerticalDivider : UI_Component
{
    virtual void Init();
    virtual void Draw(float x, float y, float width, float height, float compX, float compY);
    virtual bool KeyEvent(float *point, int key);
    virtual UI_Component *GetCompAtLocation(float *point);

    void DrawTop(float x, float y, float width, float topHeight);

    UI_ScrollPane *topComp;
    UI_ScrollPane *bottomComp;
    float posY;
};

void UI_Component_Init(void);
int UI_Component_IsCursorHidden(void);

// universal/com_math.h inlines
static inline void Vector2Copy(const vec2_t a, vec2_t b)
{
    b[0] = a[0];
    b[1] = a[1];
}

void UI_Component::Shutdown()
{
}

static inline void Vector2Add(const vec2_t a, const vec2_t b, vec2_t c)
{
    c[0] = a[0] + b[0];
    c[1] = a[1] + b[1];
}

// retail 0x45f980, unreferenced; name unknown
void UI_Component_InitIfStarted_45f980(void)
{
    if (cls.uiStarted)
        UI_Component_Init();
}

void UI_Component_Init(void)
{
    memset(&UI_Component_g, 0, sizeof(UI_Component_data_t));

    UI_Component_g.screenWidth = (float)cls.vidConfig.width;
    UI_Component_g.screenHeight = (float)cls.vidConfig.height;

    UI_Component_g.charWidth = 8.0f;
    UI_Component_g.charHeight = 16.0f;
    UI_Component_g.scrollBarSize = 16.0f;

    if (com_developer->current.integer)
    {
        UI_Component_g.cursor = re.RegisterMaterial("ui/assets/3_cursor3", 0x30, 1);
        UI_Component_g.filledCircle = re.RegisterMaterial("ui/assets/sliderbutt_1", 0x30, 1);
    }
}

MaterialHandle UI_Component::RegisterMaterialNoMip(const char *name, int imageTrack)
{
    return re.RegisterMaterial(name, 0x30, imageTrack);
}

void UI_Component::DrawPic(float x, float y, float width, float height, float *color, MaterialHandle material)
{
    re.DrawStretchPic(floor(x), floor(y), width, height, 0.0f, 0.0f, 1.0f, 1.0f, color, material);
}

void UI_Component::DrawPicRotate(float x, float y, float width, float height, float *color, MaterialHandle material)
{
    float verts[4][2];

    verts[1][0] = floor(x);
    verts[1][1] = floor(y);
    verts[2][0] = verts[1][0] + width;
    verts[2][1] = verts[1][1];
    verts[3][0] = verts[2][0];
    verts[3][1] = verts[1][1] + height;
    verts[0][0] = verts[1][0];
    verts[0][1] = verts[3][1];
    re.DrawQuadPic(verts, color, material);
}

void UI_Component::DrawText(float x, float y, float width, int fontEnum, const float *color, const char *text)
{
    int maxChars;

    maxChars = (int)(width / UI_Component_g.charWidth);
    x = floor(x);
    y = floor(UI_Component_g.charHeight + y);
    re.DrawText(text, maxChars, cls.consoleFont, x, y, 1.0f, 1.0f, color, 0);
}

void UI_Component::DrawCursor()
{
    if (!UI_Component_g.hideCursor)
        DrawPic(UI_Component_g.cursorPos[0] - 16.0f, UI_Component_g.cursorPos[1] - 16.0f, 32.0f, 32.0f, NULL, UI_Component_g.cursor);
}

void UI_Component::MouseEvent(int x, int y)
{
    UI_Component_g.hideCursor = 0;
    if (x < 0 || x >= UI_Component_g.screenWidth)
        UI_Component_g.hideCursor = 1;
    if (y < 0 || y >= UI_Component_g.screenHeight)
        UI_Component_g.hideCursor = 1;
    if (!UI_Component_g.hideCursor)
    {
        UI_Component_g.cursorPos[0] = (float)x;
        UI_Component_g.cursorPos[1] = (float)y;
    }
}

// Original name unknown.
int UI_Component_IsCursorHidden(void)
{
    return UI_Component_g.hideCursor;
}

void UI_Component::GetCursorPos(int *x, int *y)
{
    *x = (int)UI_Component_g.cursorPos[0];
    *y = (int)UI_Component_g.cursorPos[1];
    UI_Component_g.hideCursor = 0;
}

void UI_Component::Init()
{
    this->size[0] = 0.0f;
    this->size[1] = 0.0f;
    this->selectionParent = NULL;
}

UI_Component *UI_Component::GetCompAtLocation(float *point)
{
    if (point[0] >= 0.0f && point[0] < this->size[0] && point[1] >= 0.0f && point[1] < this->size[1])
        return this;
    return NULL;
}

void UI_ScrollPane::Init()
{
    UI_Component::Init();
    this->comp = NULL;
    this->forceHorScoll = 0;
    this->mouseHeldScale[0] = 0.0f;
    this->mouseHeldScale[1] = 0.0f;
    this->mouseWasDown[0] = 0;
    this->mouseWasDown[1] = 0;
}

static bool InRect(float *point, float *pos, float *size)
{
    int i;

    for (i = 0; i < 2; i++)
    {
        if (point[i] < pos[i])
            return 0;
        if (point[i] >= pos[i] + size[i])
            return 0;
    }
    return 1;
}

void UI_ScrollPane::CheckMouseScroll(int index, float *thumbPos, float *thumbSize, float thumbMaxSize)
{
    if (!keys[UIC_K_MOUSE1].down)
    {
        this->mouseWasDown[index] = 0;
        this->mouseHeldScale[0] = 0.0f;
        this->mouseHeldScale[1] = 0.0f;
        return;
    }
    if (this->mouseHeldScale[index] != 0.0f)
    {
        this->comp->pos[index] = (UI_Component_g.cursorPos[index] - this->mouseHeldPos[index]) * this->mouseHeldScale[index]
                               + this->mouseHeldCompPos[index];
        SetPos();
        return;
    }
    if (!UI_Component_g.hideCursor && InRect(UI_Component_g.cursorPos, thumbPos, thumbSize))
    {
        if (!this->mouseWasDown[index])
        {
            this->mouseHeldScale[index] = this->comp->size[index] / thumbMaxSize;
            Vector2Copy(UI_Component_g.cursorPos, this->mouseHeldPos);
            Vector2Copy(this->comp->pos, this->mouseHeldCompPos);
        }
    }
    else
    {
        this->mouseWasDown[index] = 1;
    }
}

void UI_ScrollPane::Draw(float x, float y, float width, float height, float compX, float compY)
{
    int selectedLine;
    float focusRegionStart2;
    float focusRegionStart;
    float focusRegionEnd;
    float thumbColor[4];
    float thumbMaxSize;
    float thumbSize[2];
    float thumbPos[2];
    float thumbStart;
    float thumbEnd;
    float thumbFiller;
    float innerSize[2];
    float scrollbarX;
    float scrollbarY;
    float scrollBarSize;
    bool horScroll;

    this->size[0] = width;
    this->size[1] = height;
    if (!this->comp || this->comp->size[0] == 0.0f || this->comp->size[1] == 0.0f)
    {
        DrawPic(x, y, width, height, NULL, cls.consoleMaterial);
        return;
    }

    horScroll = GetInnerSize(innerSize);
    if (this->comp->focusOnSelectedLine)
    {
        this->comp->focusOnSelectedLine = 0;
        selectedLine = this->comp->selectedLine < 0 ? 0 : this->comp->selectedLine;
        focusRegionStart2 = (float)selectedLine * UI_Component_g.charHeight;
        if (this->comp->focusOnSelectedLineUser)
        {
            focusRegionStart = focusRegionStart2;
            focusRegionEnd = UI_Component_g.charHeight + focusRegionStart2;
        }
        else
        {
            focusRegionStart = focusRegionStart2 - UI_Component_g.charHeight * 4.0f;
            focusRegionEnd = UI_Component_g.charHeight * 5.0f + focusRegionStart2;
        }
        if (focusRegionStart < this->comp->pos[1])
        {
            if (focusRegionEnd >= this->comp->pos[1])
            {
                this->comp->pos[1] = focusRegionStart;
                if (focusRegionStart2 - innerSize[1] / 2.0f > this->comp->pos[1])
                    this->comp->pos[1] = focusRegionStart2 - innerSize[1] / 2.0f;
            }
            else
            {
                this->comp->pos[1] = focusRegionStart2 - innerSize[1] / 2.0f;
            }
        }
        else if (focusRegionEnd > this->comp->pos[1] + innerSize[1])
        {
            if (focusRegionStart <= this->comp->pos[1] + innerSize[1])
            {
                this->comp->pos[1] = focusRegionEnd - innerSize[1];
                if (focusRegionStart2 - innerSize[1] / 2.0f < this->comp->pos[1])
                    this->comp->pos[1] = focusRegionStart2 - innerSize[1] / 2.0f;
            }
            else
            {
                this->comp->pos[1] = focusRegionStart2 - innerSize[1] / 2.0f;
            }
        }
    }
    SetPos();

    thumbColor[0] = 0.5f;
    thumbColor[1] = 0.5f;
    thumbColor[2] = 0.5f;
    thumbColor[3] = 1.0f;
    this->comp->Draw(x, y, innerSize[0], innerSize[1], this->comp->pos[0], this->comp->pos[1]);

    scrollbarX = x + innerSize[0];
    DrawPic(scrollbarX, y, UI_Component_g.scrollBarSize, height, NULL, cls.consoleMaterial);
    DrawPic(scrollbarX, y, UI_Component_g.scrollBarSize, UI_Component_g.scrollBarSize, NULL, sharedUiInfo.assets.scrollBarArrowUp);
    scrollbarY = y + UI_Component_g.scrollBarSize;
    scrollBarSize = innerSize[1] - (UI_Component_g.scrollBarSize + UI_Component_g.scrollBarSize);
    if (scrollBarSize > UI_Component_g.scrollBarSize)
        thumbFiller = UI_Component_g.scrollBarSize;
    else
        thumbFiller = scrollBarSize;
    DrawPic(scrollbarX, scrollbarY, UI_Component_g.scrollBarSize, scrollBarSize, NULL, sharedUiInfo.assets.scrollBar);
    DrawPic(scrollbarX, scrollbarY + scrollBarSize, UI_Component_g.scrollBarSize, UI_Component_g.scrollBarSize, NULL, sharedUiInfo.assets.scrollBarArrowDown);
    thumbMaxSize = scrollBarSize - thumbFiller;
    thumbStart = this->comp->pos[1] / this->comp->size[1] * thumbMaxSize;
    thumbEnd = (this->comp->pos[1] + innerSize[1]) / this->comp->size[1] * thumbMaxSize + thumbFiller;
    if (thumbEnd > scrollBarSize)
        thumbEnd = scrollBarSize;
    thumbPos[0] = scrollbarX;
    thumbPos[1] = y + UI_Component_g.scrollBarSize + thumbStart;
    thumbSize[0] = UI_Component_g.scrollBarSize;
    thumbSize[1] = thumbEnd - thumbStart;
    DrawPic(scrollbarX, thumbPos[1], thumbSize[0], thumbSize[1], thumbColor, sharedUiInfo.assets.whiteMaterial);
    CheckMouseScroll(1, thumbPos, thumbSize, thumbMaxSize);

    if (horScroll)
    {
        scrollbarY = y + innerSize[1];
        DrawPic(x, scrollbarY, innerSize[0], UI_Component_g.scrollBarSize, NULL, cls.consoleMaterial);
        DrawPicRotate(x, scrollbarY, UI_Component_g.scrollBarSize, UI_Component_g.scrollBarSize, NULL, sharedUiInfo.assets.scrollBarArrowUp);
        scrollbarX = x + UI_Component_g.scrollBarSize;
        scrollBarSize = innerSize[0] - (UI_Component_g.scrollBarSize + UI_Component_g.scrollBarSize);
        if (scrollBarSize > UI_Component_g.scrollBarSize)
            thumbFiller = UI_Component_g.scrollBarSize;
        else
            thumbFiller = scrollBarSize;
        DrawPicRotate(scrollbarX, scrollbarY, scrollBarSize, UI_Component_g.scrollBarSize, NULL, sharedUiInfo.assets.scrollBar);
        DrawPicRotate(scrollbarX + scrollBarSize, scrollbarY, UI_Component_g.scrollBarSize, UI_Component_g.scrollBarSize, NULL, sharedUiInfo.assets.scrollBarArrowDown);
        thumbMaxSize = scrollBarSize - thumbFiller;
        thumbStart = this->comp->pos[0] / this->comp->size[0] * thumbMaxSize;
        thumbEnd = (this->comp->pos[0] + innerSize[0]) / this->comp->size[0] * thumbMaxSize + thumbFiller;
        if (thumbEnd > scrollBarSize)
            thumbEnd = scrollBarSize;
        thumbPos[0] = x + UI_Component_g.scrollBarSize + thumbStart;
        thumbPos[1] = scrollbarY;
        thumbSize[0] = thumbEnd - thumbStart;
        thumbSize[1] = UI_Component_g.scrollBarSize;
        DrawPicRotate(thumbPos[0], thumbPos[1], thumbSize[0], thumbSize[1], thumbColor, sharedUiInfo.assets.whiteMaterial);
        CheckMouseScroll(0, thumbPos, thumbSize, thumbMaxSize);
    }
}

void UI_ScrollPane::SetPos()
{
    int i;
    float maxPos;
    float innerSize[2];

    for (i = 0; i < 2; i++)
    {
        if (this->comp->pos[i] < 0.0f)
        {
            this->comp->pos[i] = 0.0f;
        }
        else
        {
            GetInnerSize(innerSize);
            maxPos = this->comp->size[i] - innerSize[i];
            if (maxPos < 0.0f)
                maxPos = 0.0f;
            if (maxPos < this->comp->pos[i])
                this->comp->pos[i] = maxPos;
        }
    }
    this->comp->ClearFocus();
}

int UI_ScrollPane::GetFirstDisplayedLine()
{
    float startLineFrac;
    int startLine;

    startLineFrac = this->comp->pos[1] / UI_Component_g.charHeight;
    startLine = (int)startLineFrac;
    if ((float)startLine - startLineFrac < 0.0f)
        return startLine + 1;
    return startLine;
}

int UI_ScrollPane::GetLastDisplayedLine()
{
    int lastLine;
    float innerSize[2];

    GetInnerSize(innerSize);
    lastLine = (int)((this->comp->pos[1] + innerSize[1]) / UI_Component_g.charHeight);
    if (innerSize[1] < (float)(lastLine + 1) * UI_Component_g.charHeight)
        lastLine--;
    if (lastLine >= this->comp->numLines)
        return this->comp->numLines - 1;
    return lastLine;
}

void UI_ScrollPane::DisplaySelectedLine()
{
    int firstLine;
    int lastLine;

    firstLine = GetFirstDisplayedLine();
    if (this->comp->selectedLine < firstLine)
    {
        this->comp->selectedLine = firstLine;
    }
    else
    {
        lastLine = GetLastDisplayedLine();
        if (this->comp->selectedLine > lastLine)
            this->comp->selectedLine = lastLine;
    }
}

bool UI_ScrollPane::KeyEvent(float *point, int key)
{
    int linesCount;
    float vec2_origin[2];
    float innerSize[2];
    float innerPoint[2];

    if (!this->comp)
        return 0;

    if (!keys[UIC_K_ALT].down && !keys[UIC_K_CTRL].down && !keys[UIC_K_SHIFT].down)
    {
        switch (key)
        {
        case UIC_K_PGDN:
            linesCount = GetInnerLinesCount();
            this->comp->pos[1] += (float)linesCount * UI_Component_g.charHeight;
            this->comp->selectedLine += linesCount;
            SetPos();
            DisplaySelectedLine();
            return 1;
        case UIC_K_PGUP:
            linesCount = GetInnerLinesCount();
            this->comp->pos[1] -= (float)linesCount * UI_Component_g.charHeight;
            this->comp->selectedLine -= linesCount;
            SetPos();
            DisplaySelectedLine();
            return 1;
        case UIC_K_MWHEELUP:
            this->comp->pos[1] -= UI_Component_g.charHeight * 3.0f;
            SetPos();
            return 1;
        case UIC_K_MWHEELDOWN:
            this->comp->pos[1] += UI_Component_g.charHeight * 3.0f;
            SetPos();
            return 1;
        }
    }
    else if (!keys[UIC_K_ALT].down && keys[UIC_K_CTRL].down && !keys[UIC_K_SHIFT].down)
    {
        switch (key)
        {
        case UIC_K_UPARROW:
            this->comp->pos[1] -= UI_Component_g.charHeight;
            SetPos();
            return 1;
        case UIC_K_DOWNARROW:
            this->comp->pos[1] += UI_Component_g.charHeight;
            SetPos();
            return 1;
        case UIC_K_LEFTARROW:
            this->comp->pos[0] -= UI_Component_g.charWidth;
            SetPos();
            return 1;
        case UIC_K_RIGHTARROW:
            this->comp->pos[0] += UI_Component_g.charWidth;
            SetPos();
            return 1;
        case UIC_K_PGDN:
            this->comp->selectedLine = 0x7FFFFFFF;
            DisplaySelectedLine();
            return 1;
        case UIC_K_PGUP:
            this->comp->selectedLine = 0;
            DisplaySelectedLine();
            return 1;
        case UIC_K_HOME:
            this->comp->pos[1] = 0.0f;
            this->comp->selectedLine = 0;
            SetPos();
            DisplaySelectedLine();
            return 1;
        case UIC_K_END:
            this->comp->pos[1] = 3.4028235e38f;
            this->comp->selectedLine = 0x7FFFFFFF;
            SetPos();
            DisplaySelectedLine();
            return 1;
        }
    }

    if (key == UIC_K_MOUSE1)
    {
        vec2_origin[0] = 0.0f;
        vec2_origin[1] = 0.0f;
        GetInnerSize(innerSize);
        if (!InRect(point, vec2_origin, innerSize))
            return 0;
    }
    Vector2Add(point, this->comp->pos, innerPoint);
    return this->comp->KeyEvent(innerPoint, key);
}

bool UI_ScrollPane::GetInnerSize(float *innerSize)
{
    bool horScroll;

    innerSize[0] = this->size[0] - UI_Component_g.scrollBarSize;
    horScroll = this->forceHorScoll || this->comp->pos[0] != 0.0f || innerSize[0] < this->comp->size[0];
    if (innerSize[0] > this->comp->size[0])
        this->comp->size[0] = innerSize[0];
    innerSize[1] = this->size[1];
    if (horScroll)
        innerSize[1] -= UI_Component_g.scrollBarSize;
    return horScroll;
}

int UI_ScrollPane::GetInnerLinesCount()
{
    float innerSize[2];

    GetInnerSize(innerSize);
    return (int)(innerSize[1] / UI_Component_g.charHeight);
}

UI_Component *UI_ScrollPane::GetCompAtLocation(float *point)
{
    float innerSize[2];

    if (this->comp)
    {
        GetInnerSize(innerSize);
        if (point[0] < innerSize[0] && point[1] < innerSize[1])
        {
            Vector2Add(point, this->comp->pos, point);
            return this->comp->GetCompAtLocation(point);
        }
    }
    return UI_Component::GetCompAtLocation(point);
}

void UI_ScrollPane::AddText(const char *text)
{
    if (this->comp)
        this->comp->AddText(text);
}

void UI_LinesComponent::Init()
{
    UI_Component::Init();
    this->selectedLine = -1;
    this->numLines = 0;
    this->focusOnSelectedLine = 0;
    this->pos[0] = 0.0f;
    this->pos[1] = 0.0f;
}

bool UI_LinesComponent::SetSelectedLineFocus(int newSelectedLine, bool user)
{
    if (newSelectedLine >= (user ? 0 : -1) && newSelectedLine < this->numLines)
    {
        this->selectedLine = newSelectedLine;
        this->focusOnSelectedLine = 1;
        this->focusOnSelectedLineUser = user;
        return 1;
    }
    this->selectedLine = -1;
    return 0;
}

void UI_LinesComponent::IncSelectedLineFocus(bool wrap)
{
    if (this->selectedLine < 0 || wrap && this->selectedLine >= this->numLines - 1)
        SetSelectedLineFocus(0, 0);
    else
        SetSelectedLineFocus(this->selectedLine + 1, 0);
}

void UI_LinesComponent::DecSelectedLineFocus(bool wrap)
{
    if (this->selectedLine < 0 || wrap && this->selectedLine <= 0)
        SetSelectedLineFocus(this->numLines - 1, 0);
    else
        SetSelectedLineFocus(this->selectedLine - 1, 0);
}

void UI_LinesComponent::UpdateHeight()
{
    this->size[1] = (float)this->numLines * UI_Component_g.charHeight;
}

bool UI_LinesComponent::KeyEvent(float *point, int key)
{
    if (keys[UIC_K_ALT].down || keys[UIC_K_CTRL].down || keys[UIC_K_SHIFT].down)
    {
        if (keys[UIC_K_ALT].down || !keys[UIC_K_CTRL].down || keys[UIC_K_SHIFT].down)
            return 0;
        switch (key)
        {
        case 'g':
            UI_Component_g.consoleReason = 0;
            Con_ToggleConsole_f();
            return 1;
        case UIC_K_MOUSE1:
            if (this->selectedLine == (int)(point[1] / UI_Component_g.charHeight))
                this->selectedLine = -1;
            return 0;
        }
        return 0;
    }

    switch (key)
    {
    case 45:
        this->selectedLine = -1;
        return 1;
    case UIC_K_MOUSE1:
        SetSelectedLineFocus((int)(point[1] / UI_Component_g.charHeight), 1);
        return 0;
    case UIC_K_UPARROW:
        DecSelectedLineFocus(0);
        return 1;
    case UIC_K_DOWNARROW:
        IncSelectedLineFocus(0);
        return 1;
    }
    return 0;
}

void UI_LinesComponent::AddText(const char *text)
{
    int lineIndex;

    if (!UI_Component_g.consoleReason)
    {
        lineIndex = atoi(text);
        SetSelectedLineFocus(lineIndex - 1, 0);
    }
}

void UI_VerticalDivider::Init()
{
    UI_Component::Init();
    this->topComp = NULL;
    this->bottomComp = NULL;
}

void UI_VerticalDivider::DrawTop(float x, float y, float width, float topHeight)
{
    this->topComp->Draw(x, y, width, topHeight, 0.0f, 0.0f);
    if (this->topComp->mouseHeldScale[0] != 0.0f)
        this->posY = UI_Component_g.cursorPos[1] + UI_Component_g.charHeight;
}

bool UI_VerticalDivider::KeyEvent(float *point, int key)
{
    if (!keys[UIC_K_ALT].down || keys[UIC_K_CTRL].down || keys[UIC_K_SHIFT].down)
        return 0;
    switch (key)
    {
    case UIC_K_UPARROW:
        this->posY -= UI_Component_g.charHeight;
        return 1;
    case UIC_K_DOWNARROW:
        this->posY += UI_Component_g.charHeight;
        return 1;
    }
    return 0;
}

UI_Component *UI_VerticalDivider::GetCompAtLocation(float *point)
{
    if (!this->topComp)
    {
        if (!this->bottomComp)
            return this;
    }
    else
    {
        if (!this->bottomComp)
            return this->topComp->GetCompAtLocation(point);
        if (point[1] < this->posY)
            return this->topComp->GetCompAtLocation(point);
        point[1] -= this->posY;
    }
    return this->bottomComp->GetCompAtLocation(point);
}

void UI_VerticalDivider::Draw(float x, float y, float width, float height, float compX, float compY)
{
    this->size[0] = width;
    this->size[1] = height;
    this->posY = (float)(int)(this->posY / UI_Component_g.charHeight) * UI_Component_g.charHeight;
    if (this->posY < UI_Component_g.charHeight)
        this->posY = UI_Component_g.charHeight;
    else if (this->posY > height)
        this->posY = height;

    if (!this->topComp)
    {
        if (this->bottomComp)
            this->bottomComp->Draw(x, y, width, height, 0.0f, 0.0f);
        return;
    }
    if (!this->bottomComp)
        goto drawFull;
    if (this->posY <= compY)
    {
        this->bottomComp->Draw(x, y, width, height, 0.0f, 0.0f);
        return;
    }
    compY = this->posY - compY;
    if (compY >= height)
    {
drawFull:
        DrawTop(x, y, width, height);
    }
    else
    {
        this->bottomComp->Draw(x, y + this->posY, width, height - compY, 0.0f, 0.0f);
        DrawTop(x, y, width, compY);
    }
}

void UI_Component::AddText(const char *text)
{
}

void UI_LinesComponent::ClearFocus()
{
    this->focusOnSelectedLine = 0;
}

UI_Component::~UI_Component()
{
}

