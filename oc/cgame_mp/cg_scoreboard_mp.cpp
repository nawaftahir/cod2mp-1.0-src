#include "common_types.h"
#include "imports.h"
#include "bytematch.h"
#include "headers/PC/cgame_mp/cg_local.h"
extern const dvar_t *cg_scoreboardScrollStep;
extern const vec4_t colorBlack;
extern const vec4_t colorWhite;

extern const dvar_t *cg_scoreboardBannerHeight;

extern const dvar_t *cg_paused;

extern MaterialHandle CL_RegisterMaterialNoMip(const char *name, int imageTrack);
extern const char* Dvar_GetString(const char *dvarName);
extern void Dvar_GetUnpackedColorByName(const char *dvarName, vec_t *color);
extern void UI_DrawHandlePic(float x, float y, float w, float h, int horzAlign, int vertAlign, const vec_t *color, MaterialHandle hMaterial);
extern void UI_DrawText(const char *text, int maxChars, FontHandle font, float x, float y, int horzAlign, int vertAlign, float scale, const vec_t *color, int style);
extern int UI_TextWidth(const char *text, int maxChars, struct Font_s *font, float scale);
extern int UI_TextHeight(struct Font_s *font, float scale);
extern FontHandle UI_GetFontHandle(int fontEnum, float scale);
extern const char *UI_GetGameTypeDisplayName(const char *gameType);
extern const char *UI_GetMapDisplayName(const char *mapName);
extern const char *SEH_LocalizeTextMessage(const char *msg, const char *context, msgLocErrType_t errType);
extern const char *CL_GetServerIPAddress(void);
extern Bool CL_IsPlayerMuted(int clientIndex);
extern Bool CL_IsPlayerTalking(int clientIndex);
extern void CG_TranslateHudElemMessage(const char *message, const char *messageType, char *hudElemString);
extern int I_stricmp(const char *s1, const char *s2);
extern int I_strnicmp(const char *s1, const char *s2, int n);
extern void I_strncpyz(char *dest, const char *src, int destsize);
extern char *FS_GetMapBaseName(const char *mapname);
extern const char *UI_SafeTranslateString(const char *key);
extern void CL_AddReliableCommand(const char *cmd);
extern float *CG_FadeColor(int startMsec, int totalMsec, int fadeMsec);
extern char *va( const char *format, ... );
extern qboolean Language_IsAsian(void);
extern unsigned int SEH_ReadCharFromString(const char **ppsText, qboolean *pbIsTrailingPunctuation);


static const listColumnInfo_t columnInfo[6] = {
    { LCT_TALKING_ICON, 0.05f, "", 0 },
    { LCT_STATUS_ICON, 0.05f, "", 0 },
    { LCT_NAME, 0.47f, "", 0 },
    { LCT_SCORE, 0.13f, "CGAME_SB_SCORE", 2 },
    { LCT_DEATHS, 0.13f, "CGAME_SB_DEATHS", 2 },
    { LCT_PING, 0.17f, "CGAME_SB_PING", 2 },
};

// unreferenced scoreboard layout getters, kept by /OPT:NOREF
float CG_ScoreboardGetter_4d2bb0() { return 0.24f; }
float CG_ScoreboardGetter_4d2bc0() { return 12.0f; }
float CG_ScoreboardGetter_4d2bd0() { return 400.0f; }
float CG_ScoreboardGetter_4d2be0() { return 430.0f; }
float CG_ScoreboardGetter_4d2bf0() { return 120.0f; }
float CG_ScoreboardGetter_4d2c00() { return 22.0f; }

int CalcBorderLines(sbpicinfo_t *backdropBorderLines)
{

    backdropBorderLines[0].x = 123.0f;
    backdropBorderLines[0].y = 25.0f;
    backdropBorderLines[0].w = 394.0f;
    backdropBorderLines[0].h = 2.0f;

    backdropBorderLines[1].x = 123.0f;
    backdropBorderLines[1].y = 447.0f;
    backdropBorderLines[1].w = 394.0f;
    backdropBorderLines[1].h = 2.0f;

    backdropBorderLines[2].x = 123.0f;
    backdropBorderLines[2].y = 27.0f;
    backdropBorderLines[2].w = 2.0f;
    backdropBorderLines[2].h = 420.0f;

    backdropBorderLines[3].x = 515.0f;
    backdropBorderLines[3].y = 27.0f;
    backdropBorderLines[3].w = 2.0f;
    backdropBorderLines[3].h = 420.0f;

    backdropBorderLines[4].x = 125.0f;
    backdropBorderLines[4].y = 51.0f;
    backdropBorderLines[4].w = 390.0f;
    backdropBorderLines[4].h = 1.0f;

    backdropBorderLines[5].x = 125.0f;
    backdropBorderLines[5].y = 432.0f;
    backdropBorderLines[5].w = 390.0f;
    backdropBorderLines[5].h = 1.0f;

    return 6;
}

// retail 0x4d2ca0, name unknown
void CG_DrawScoreboard_Backdrop(float alpha)
{
    float color[4];

    color[0] = 0.0f;
    color[1] = 0.0f;
    color[2] = 0.0f;
    color[3] = alpha * 0.8f;
    UI_DrawHandlePic(120.0f, 22.0f, 400.0f, 430.0f, 0, 0, color, CL_RegisterMaterialNoMip("white", 7));
}

// retail 0x4d2d10, name unknown
void CG_DrawScoreboard_BackdropBorder(float alpha)
{
    float color[4];
    MaterialHandle material;
    sbpicinfo_t lines[6];
    int numLines;
    int i;

    color[0] = 1.0f;
    color[1] = 1.0f;
    color[2] = 1.0f;
    color[3] = alpha * 0.1f;
    material = CL_RegisterMaterialNoMip("white", 7);
    numLines = CalcBorderLines(lines);
    for (i = 0; i < numLines; i++)
        UI_DrawHandlePic(lines[i].x, lines[i].y, lines[i].w, lines[i].h, 0, 0, color, material);
}

// retail 0x4d2da0, name unknown
void CG_DrawScoreboard_GameType(float alpha, const char *gameType)
{
    float color[4];
    FontHandle font;
    float y;

    color[0] = 1.0f;
    color[1] = 1.0f;
    color[2] = 1.0f;
    color[3] = alpha;
    font = UI_GetFontHandle(0, 0.41f);
    y = 51.0f - (float)(24 - UI_TextHeight(font, 0.41f)) * 0.5f;
    UI_DrawText(gameType, 0x7fffffff, font, 129.0f, y, 0, 0, 0.41f, color, 3);
}

// retail 0x4d2eb0, name unknown
void CG_DrawScoreboard_MapName(float alpha, const char *gameType)
{
    float color[4];
    char mapName[64];
    const char *map;
    FontHandle font;
    float scale;
    float x;
    float y;
    int len;

    color[0] = 1.0f;
    color[1] = 1.0f;
    color[2] = 1.0f;
    color[3] = alpha;
    I_strncpyz(mapName, UI_GetMapDisplayName(FS_GetMapBaseName(cgs->mapname)), 64);
    if (!mapName[0])
    {
        strcpy(mapName, cgs->mapname);
        len = strlen(mapName);
        if (!I_strnicmp(&mapName[len - 3], "bsp", 3))
            mapName[len - 7] = 0;
    }

    map = mapName;
    if (!I_strnicmp(map, "maps", 4) && (mapName[4] == '/' || mapName[4] == '\\'))
        map = &mapName[5];
    if (!I_strnicmp(map, "mp", 2) && (map[2] == '/' || map[2] == '\\'))
        map += 3;

    scale = 0.41f;
    do
    {
        font = UI_GetFontHandle(0, scale);
        if ((float)(UI_TextWidth(gameType, 0, font, scale) + UI_TextWidth(map, 0, font, scale) + 4) <= 386.0f)
            break;
        scale -= 0.025f;
    } while (scale > 0.075f);

    x = 511.0f - (float)(UI_TextWidth(map, 0, font, scale) + 4);
    y = 51.0f - (float)(24 - UI_TextHeight(font, scale)) * 0.5f;
    UI_DrawText(map, 0x7fffffff, font, x, y, 0, 0, scale, color, 3);
}

// retail 0x4d3160, name unknown
void CG_DrawScoreboard_ServerInfo(float alpha)
{
    float color[4];
    const char *serverIP;
    FontHandle font;
    float scale;
    float x;
    float y;

    color[0] = 1.0f;
    color[1] = 1.0f;
    color[2] = 1.0f;
    color[3] = alpha;
    serverIP = CL_GetServerIPAddress();
    if (!I_stricmp(serverIP, "0.0.0.0:0"))
        serverIP = UI_SafeTranslateString("CGAME_LISTENSERVER");

    scale = 0.2f;
    do
    {
        font = UI_GetFontHandle(0, scale);
        if ((float)(UI_TextWidth(cgs->szHostName, 0, font, scale) + UI_TextWidth(serverIP, 0, font, scale) + 4) <= 386.0f)
            break;
        scale -= 0.01f;
    } while (scale > 0.075f);

    y = 447.0f - (float)(14 - UI_TextHeight(font, scale)) * 0.5f;
    UI_DrawText(cgs->szHostName, 0x7fffffff, font, 129.0f, y, 0, 0, scale, color, 3);
    x = 511.0f - (float)(UI_TextWidth(serverIP, 0, font, scale) + 4);
    UI_DrawText(serverIP, 0x7fffffff, font, x, y, 0, 0, scale, color, 3);
}

void CG_DrawBackdropServerInfo(float alpha)
{
    const char *gameType;

    CG_DrawScoreboard_Backdrop(alpha);
    CG_DrawScoreboard_BackdropBorder(alpha);
    gameType = SEH_LocalizeTextMessage(UI_GetGameTypeDisplayName(cgs->gametype), "scoreboard gametype display", LOCMSG_SAFE);
    CG_DrawScoreboard_GameType(alpha, gameType);
    CG_DrawScoreboard_MapName(alpha, gameType);
    CG_DrawScoreboard_ServerInfo(alpha);
}

// retail 0x4d33f0, name unknown
float CG_DrawScoreboard_Objective(const float *color, float y)
{
    FontHandle font;
    char hudElemString[256];
    const char *textStart;
    const char *textEnd;
    const char *whiteSpace;
    MaterialHandle material;
    float lineColor[4];

    if (cg->objectiveText[0])
    {
        y += 4.0f;
        font = UI_GetFontHandle(0, 0.24f);
        CG_TranslateHudElemMessage(cg->objectiveText, "scoreboard objective info", hudElemString);

        textStart = hudElemString;
        textEnd = hudElemString;
        whiteSpace = NULL;
        while (textStart && textEnd)
        {
            if (*textStart == ' ')
            {
                textStart++;
            }
            else if (*textStart == '\n' || (*textStart == '\\' && textStart[1] == 'n'))
            {
                if (*textStart == '\n')
                    textStart++;
                else
                    textStart += 2;
                y += 12.0f;
                whiteSpace = NULL;
                textEnd = textStart;
                continue;
            }
            else
            {
                textEnd++;
                if (!*textEnd)
                {
                    UI_DrawText(textStart, textEnd - textStart, font, 129.0f, y + 10.08f, 0, 0, 0.24f, color, 3);
                    y += 12.0f;
                    break;
                }
                if (*textEnd == '\n' || (*textEnd == '\\' && textEnd[1] == 'n'))
                {
                    UI_DrawText(textStart, textEnd - textStart, font, 129.0f, y + 10.08f, 0, 0, 0.24f, color, 3);
                    if (*textEnd == '\n')
                        textStart = textEnd + 1;
                    else
                        textStart = textEnd + 2;
                    whiteSpace = NULL;
                    y += 12.0f;
                    textEnd = textStart;
                    continue;
                }
                if ((float)UI_TextWidth(textStart, textEnd - textStart, font, 0.24f) > 374.0f)
                {
                    UI_DrawText(textStart, whiteSpace - textStart, font, 129.0f, y + 10.08f, 0, 0, 0.24f, color, 3);
                    if (whiteSpace)
                        textStart = whiteSpace + 1;
                    else
                        textStart = textEnd - 1;
                    whiteSpace = NULL;
                    y += 12.0f;
                    textEnd = textStart;
                    continue;
                }
                if (*textEnd == ' ')
                    whiteSpace = textEnd;
                continue;
            }
            whiteSpace = NULL;
            textEnd = textStart;
        }

        y += 2.0f;
        material = CL_RegisterMaterialNoMip("white", 7);
        lineColor[0] = color[0];
        lineColor[1] = color[1];
        lineColor[2] = color[2];
        lineColor[3] = color[3] * 0.1f;
        UI_DrawHandlePic(125.0f, y, 390.0f, 1.0f, 0, 0, lineColor, material);
        return y + 1.0f;
    }
    return y;
}

void CG_GetScoreboardInfo(const listColumnInfo_t **colInfo, int *numFields)
{
    *colInfo = columnInfo;
    *numFields = 6;
}

float CG_DrawScoreboard_ListColumnHeaders(const float *color, float y, float listWidth)
{
    const listColumnInfo_t *info;
    int fieldCount;
    FontHandle font;
    const char *translation;
    float xAdj;
    float x;
    int i;

    font = UI_GetFontHandle(0, 0.3f);
    x = 129.0f;
    CG_GetScoreboardInfo(&info, &fieldCount);
    for (i = 0; i < fieldCount; i++)
    {
        if (*info[i].pszName)
        {
            translation = UI_SafeTranslateString(info[i].pszName);
            if (info[i].iAlignment == 2)
                xAdj = listWidth * info[i].fWidth - UI_TextWidth(translation, 0, font, 0.3f);
            else
                xAdj = 0.0f;
            UI_DrawText(translation, 0x7fffffff, font, x + xAdj, y + 11.75999927520752f, 0, 0, 0.3f, color, 3);
        }
        x += listWidth * info[i].fWidth;
    }
    return y + 14.0f;
}

// retail 0x4d3870, name unknown
float CG_ScoreboardTotalHeight(int *totalLines)
{
    float height;
    int i;

    height = 10.0f;
    *totalLines = 0;
    if (cg->teamPlayers[1] || cg->teamPlayers[2])
    {
        height = (float)(cg_scoreboardBannerHeight->current.integer * 2 + 8) + 10.0f;
        *totalLines = 2;
    }
    if (cg->teamPlayers[0])
    {
        height += cg_scoreboardBannerHeight->current.integer + 4;
        ++*totalLines;
    }
    if (cg->teamPlayers[3])
    {
        height += cg_scoreboardBannerHeight->current.integer + 4;
        ++*totalLines;
    }
    for (i = 0; i < cg->numScores; i++)
    {
        height += 12.0f;
        ++*totalLines;
    }
    return height;
}

int CG_CheckDrawScoreboardLine(int *drawLine, float y, float lineHeight)
{
    if (cg->scoresOffBottom)
        return 0;
    if (*drawLine < cg->scoresTop)
    {
        ++*drawLine;
        return 0;
    }
    if (y + lineHeight > 432.0f)
    {
        cg->scoresOffBottom = 1;
        return 0;
    }
    ++*drawLine;
    return 1;
}

void CG_DrawScoreboard_GetTeamColor(int team, vec_t *color)
{
    float savedAlpha = color[3];

    switch (team)
    {
    case 1:
        Dvar_GetUnpackedColorByName("g_TeamColor_Axis", color);
        break;
    case 2:
        Dvar_GetUnpackedColorByName("g_TeamColor_Allies", color);
        break;
    default:
        color[0] = 1.0f;
        color[1] = 1.0f;
        color[2] = 1.0f;
        break;
    }

    color[3] = savedAlpha;
}

float CG_DrawScoreboard_ListBanner(const float *color, float y, float listWidth, float h, int team, int *drawLine)
{
    float bannerColor[4];
    const char *playersString;
    const char *shaderName;
    const char *teamName;
    const char *displayString;
    const listColumnInfo_t *info;
    int fieldCount;
    FontHandle font;
    const char *text;
    float xAdj;
    float x;
    int i;

    if (!CG_CheckDrawScoreboardLine(drawLine, y, (float)cg_scoreboardBannerHeight->current.integer))
        return y;

    bannerColor[0] = 1.0f;
    bannerColor[1] = 1.0f;
    bannerColor[2] = 1.0f;
    bannerColor[3] = color[3];
    if (cg->teamPlayers[team] == 1)
        playersString = SEH_LocalizeTextMessage(va("CGAME_SB_PLAYER\x15%i", cg->teamPlayers[team]), "scoreboard banner text", LOCMSG_SAFE);
    else
        playersString = SEH_LocalizeTextMessage(va("CGAME_SB_PLAYERS\x15%i", cg->teamPlayers[team]), "scoreboard banner text", LOCMSG_SAFE);

    switch (team)
    {
    case 0:
        shaderName = Dvar_GetString("g_ScoresBanner_None");
        displayString = playersString;
        break;
    case 1:
        shaderName = Dvar_GetString("g_ScoresBanner_Axis");
        teamName = Dvar_GetString("g_TeamName_Axis");
        displayString = va("%s (%s)", SEH_LocalizeTextMessage(teamName, "scoreboard team name", LOCMSG_SAFE), playersString);
        break;
    case 2:
        shaderName = Dvar_GetString("g_ScoresBanner_Allies");
        teamName = Dvar_GetString("g_TeamName_Allies");
        displayString = va("%s (%s)", SEH_LocalizeTextMessage(teamName, "scoreboard team name", LOCMSG_SAFE), playersString);
        break;
    default:
        shaderName = Dvar_GetString("g_ScoresBanner_Spectators");
        displayString = va("%s (%s)", SEH_LocalizeTextMessage("CGAME_SPECTATORS", "scoreboard team name", LOCMSG_SAFE), playersString);
        break;
    }

    UI_DrawHandlePic(129.0f, y, listWidth, h, 0, 0, bannerColor, CL_RegisterMaterialNoMip(shaderName, 7));
    font = UI_GetFontHandle(0, 0.32f);
    CG_DrawScoreboard_GetTeamColor(team, bannerColor);
    UI_DrawText(displayString, 0x7fffffff, font, 133.0f, y + cg_scoreboardBannerHeight->current.integer * 0.84f, 0, 0, 0.32f, bannerColor, 3);

    if (team == 1 || team == 2)
    {
        x = 129.0f;
        CG_GetScoreboardInfo(&info, &fieldCount);
        for (i = 0; i < fieldCount; i++)
        {
            if (info[i].type == LCT_SCORE || info[i].type == LCT_PING)
            {
                text = va("%i", info[i].type == LCT_SCORE ? cg->teamScores[team] : cg->teamPings[team]);
                if (info[i].iAlignment == 2)
                    xAdj = listWidth * info[i].fWidth - UI_TextWidth(text, 0, font, 0.32f);
                else
                    xAdj = 0.0f;
                UI_DrawText(text, 0x7fffffff, font, x + xAdj, y + cg_scoreboardBannerHeight->current.integer * 0.84f, 0, 0, 0.32f, bannerColor, 3);
            }
            x += listWidth * info[i].fWidth;
        }
    }
    return y + h;
}

float CG_DrawClientScore(const float *color, float y, const score_t *score, float listWidth, int highlight, int *drawLine, int numDrawn)
{
    clientInfo_t *ci;
    const listColumnInfo_t *info;
    int fieldCount;
    MaterialHandle material;
    float backColor[4];
    float textColor[4];
    FontHandle listFont;
    const char *string;
    float scale;
    int style;
    float xAdj;
    float x;
    int i;

    if (!CG_CheckDrawScoreboardLine(drawLine, y, 12.0f))
        return y;

    ci = &cg->bgs.clientinfo[score->client];
    if (!ci->infoValid)
        return y;

    if (cg->firstLineVisible < 0)
        cg->firstLineVisible = numDrawn;
    cg->lastLineVisible = numDrawn;

    if (score->client == cg->clientNum)
    {
        material = CL_RegisterMaterialNoMip("white", 7);
        backColor[0] = color[0];
        backColor[1] = color[1];
        backColor[2] = color[2];
        backColor[3] = color[3] * 0.2f;
        UI_DrawHandlePic(129.0f, y, listWidth, 12.0f, 0, 0, backColor, material);
    }
    else if (highlight)
    {
        material = CL_RegisterMaterialNoMip("black", 7);
        backColor[0] = color[0];
        backColor[1] = color[1];
        backColor[2] = color[2];
        backColor[3] = color[3] * 0.15f;
        UI_DrawHandlePic(129.0f, y, listWidth, 12.0f, 0, 0, backColor, material);
    }

    listFont = UI_GetFontHandle(0, 0.22f);
    x = 129.0f;
    CG_GetScoreboardInfo(&info, &fieldCount);
    for (i = 0; i < fieldCount; x += listWidth * info[i].fWidth, i++)
    {
        if (info[i].type == LCT_STATUS_ICON)
        {
            if (score->hStatusIcon)
            {
                if (info[i].iAlignment == 2)
                    xAdj = listWidth * info[i].fWidth - 13.0f;
                else
                    xAdj = 0.0f;
                backColor[0] = 1.0f;
                backColor[1] = 1.0f;
                backColor[2] = 1.0f;
                backColor[3] = color[3];
                UI_DrawHandlePic(x + xAdj, y, 13.0f, 13.0f, 0, 0, backColor, score->hStatusIcon);
            }
            continue;
        }
        if (info[i].type == LCT_TALKING_ICON)
        {
            if (CL_IsPlayerMuted(score->client))
                material = CL_RegisterMaterialNoMip("voice_off", 7);
            else if (CL_IsPlayerTalking(score->client))
                material = CL_RegisterMaterialNoMip("voice_on", 7);
            else
                material = 0;
            if (material)
            {
                backColor[0] = 1.0f;
                backColor[1] = 1.0f;
                backColor[2] = 1.0f;
                backColor[3] = color[3];
                UI_DrawHandlePic(x, y, 13.0f, 13.0f, 0, 0, backColor, material);
            }
            continue;
        }
        switch (info[i].type)
        {
        case LCT_NAME:
            string = ci->name;
            break;
        case LCT_SCORE:
            if (score->team == 3)
                continue;
            string = va("%i", score->score);
            break;
        case LCT_DEATHS:
            if (score->team == 3)
                continue;
            string = va("%i", score->deaths);
            break;
        case LCT_PING:
            string = va("%i", score->ping);
            break;
        default:
            continue;
        }
        if (!*string)
            continue;
        scale = 0.22f;
        style = 3;
        while (UI_TextWidth(string, 0x7fffffff, listFont, scale) > listWidth * info[i].fWidth)
            scale -= 0.02f;
        if (scale < 0.16f)
            style = 0;
        if (info[i].iAlignment == 2)
            xAdj = listWidth * info[i].fWidth - UI_TextWidth(string, 0x7fffffff, listFont, scale);
        else
            xAdj = 0.0f;
        textColor[0] = 1.0f;
        textColor[1] = 1.0f;
        textColor[2] = 1.0f;
        textColor[3] = color[3];
        UI_DrawText(string, 0x7fffffff, listFont, x + xAdj, y + 10.08f, 0, 0, scale, textColor, style);
    }
    return y + 12.0f;
}

float CG_DrawTeamOfClientScore(const float *color, float y, int team, float listWidth, int *drawLine, int *numDrawn)
{
    float teamColor[4];
    const score_t *score;
    int highlight;
    int i;

    CG_DrawScoreboard_GetTeamColor(team, teamColor);
    teamColor[3] = color[3];
    highlight = 0;
    score = cg->scores;
    for (i = 0; i < cg->numScores; i++, score++)
    {
        if (cg->bgs.clientinfo[score->client].infoValid && score->team == team)
        {
            y = CG_DrawClientScore(teamColor, y, score, listWidth, highlight, drawLine, *numDrawn);
            highlight ^= 1;
            ++*numDrawn;
        }
    }
    return y;
}

void CG_DrawScrollbar(const float *color, float top, int totalLines, int drawLine)
{
    float barColor[4];
    MaterialHandle material;
    float height;
    float y;
    int visibleLines;

    barColor[0] = color[0];
    barColor[1] = color[1];
    barColor[2] = color[2];
    material = CL_RegisterMaterialNoMip("black", 7);
    barColor[3] = color[3] * 0.5f;
    height = 432.0f - top - 1.0f;
    UI_DrawHandlePic(505.0f, top, 8.0f, height, 0, 0, barColor, material);

    y = top + 1.0f;
    height -= 2.0f;
    if (cg->scoresTop && totalLines)
        y += (float)cg->scoresTop / totalLines * height;
    visibleLines = drawLine - cg->scoresTop;
    if (visibleLines > 1 && totalLines > 1)
        height *= (float)visibleLines / totalLines;
    material = CL_RegisterMaterialNoMip("white", 7);
    barColor[3] = color[3] * 0.25f;
    UI_DrawHandlePic(506.0f, y, 6.0f, height, 0, 0, barColor, material);

    barColor[3] = color[3];
    if (cg->scoresTop > 0)
    {
        UI_DrawHandlePic(521.0f, top, 16.0f, 16.0f, 0, 0, barColor, CL_RegisterMaterialNoMip("hudscoreboardscroll_uparrow", 7));
        UI_DrawHandlePic(521.0f, top + 18.0f, 16.0f, 16.0f, 0, 0, barColor, CL_RegisterMaterialNoMip("hudscoreboardscroll_upkey", 7));
    }
    if (drawLine < totalLines)
    {
        UI_DrawHandlePic(521.0f, 415.0f, 16.0f, 16.0f, 0, 0, barColor, CL_RegisterMaterialNoMip("hudscoreboardscroll_downarrow", 7));
        UI_DrawHandlePic(521.0f, 397.0f, 16.0f, 16.0f, 0, 0, barColor, CL_RegisterMaterialNoMip("hudscoreboardscroll_downkey", 7));
    }
}

void CG_DrawScoreboard_ScoresList(float alpha)
{
    static int lastLeadTeam = 2;
    float color[4];
    float y;
    float headerY;
    float listWidth;
    float bannerHeight;
    int drawScrollbar;
    int totalLines;
    int drawLine;
    int numDrawn;
    int team;
    int otherTeam;
    int axisScore;
    int alliesScore;

    drawScrollbar = 0;
    cg->scoresOffBottom = 0;
    cg->firstLineVisible = -1;
    color[0] = 1.0f;
    color[1] = 1.0f;
    color[2] = 1.0f;
    color[3] = alpha;
    y = CG_DrawScoreboard_Objective(color, 52.0f);
    if (CG_ScoreboardTotalHeight(&totalLines) > 432.0f - y)
    {
        listWidth = 374.0f;
        drawScrollbar = 1;
    }
    else
    {
        listWidth = 382.0f;
    }
    bannerHeight = cg_scoreboardBannerHeight->current.integer / 0.75f;
    y = CG_DrawScoreboard_ListColumnHeaders(color, y + 4.0f, listWidth);
    headerY = y;

    drawLine = 0;
    numDrawn = 0;
    if (cg->teamPlayers[1] || cg->teamPlayers[2])
    {
        axisScore = cg->teamScores[1];
        alliesScore = cg->teamScores[2];
        if (alliesScore < axisScore)
            team = 1;
        else if (alliesScore > axisScore)
            team = 2;
        else
            team = lastLeadTeam;
        lastLeadTeam = team;

        y = CG_DrawScoreboard_ListBanner(color, y, listWidth, bannerHeight, team, &drawLine);
        y = CG_DrawTeamOfClientScore(color, y, team, listWidth, &drawLine, &numDrawn);
        otherTeam = team == 1 ? 2 : 1;
        y = CG_DrawScoreboard_ListBanner(color, y + 4.0f, listWidth, bannerHeight, otherTeam, &drawLine);
        y = CG_DrawTeamOfClientScore(color, y, otherTeam, listWidth, &drawLine, &numDrawn) + 4.0f;
    }
    if (cg->teamPlayers[0])
    {
        y = CG_DrawScoreboard_ListBanner(color, y, listWidth, bannerHeight, 0, &drawLine);
        y = CG_DrawTeamOfClientScore(color, y, 0, listWidth, &drawLine, &numDrawn) + 4.0f;
    }
    if (cg->teamPlayers[3])
    {
        y = CG_DrawScoreboard_ListBanner(color, y, listWidth, bannerHeight, 3, &drawLine);
        CG_DrawTeamOfClientScore(color, y, 3, listWidth, &drawLine, &numDrawn);
    }

    if (drawScrollbar)
        CG_DrawScrollbar(color, headerY, totalLines, drawLine);
    if (cg->scoresTop > totalLines - 1)
        cg->scoresTop = totalLines - 1;
}

qboolean CG_DrawScoreboard(void)
{
    float *fadeColor;
    float alpha;

    if (cg_paused->current.integer)
        return 0;

    if (cg->showScores)
    {
        alpha = 1.0f;
    }
    else
    {
        fadeColor = CG_FadeColor(cg->scoreFadeTime, 100, 100);
        if (!fadeColor)
        {
            cg->killerName[0] = 0;
            return 0;
        }
        alpha = fadeColor[0];
    }

    if (cg->scoresRequestTime + 2000 < cg->time)
    {
        cg->scoresRequestTime = cg->time;
        CL_AddReliableCommand("score");
    }

    CG_DrawBackdropServerInfo(alpha);
    CG_DrawScoreboard_ScoresList(alpha);
    return 1;
}

qboolean CG_ScoreboardDisplayed(void)
{
    return cg->showScores;
}

void CG_ScrollScoreboardUp(void)
{
    if (cg->scoresTop > 0) {
        cg->scoresTop -= *(int *)(((int)cg_scoreboardScrollStep) + 8);
        if (cg->scoresTop < 0)
            cg->scoresTop = 0;
    }
}

void CG_ScrollScoreboardDown(void)
{
    if (cg->scoresOffBottom != 0) {
        cg->scoresTop += *(int *)(((int)cg_scoreboardScrollStep) + 8);
        int maxScroll = cg->numScores - 1;
        if (cg->scoresTop > maxScroll)
            cg->scoresTop = maxScroll;
    }
}

void CG_RegisterScoreboardGraphics(void)
{
    CL_RegisterMaterialNoMip((const char *)"white", 7);
    CL_RegisterMaterialNoMip((const char *)"white", 7);
    CL_RegisterMaterialNoMip((const char *)"black", 7);
    CL_RegisterMaterialNoMip((const char *)"white", 7);
    CL_RegisterMaterialNoMip((const char *)"white", 7);
    CL_RegisterMaterialNoMip((const char *)"black", 7);
    CL_RegisterMaterialNoMip((const char *)"hudscoreboardscroll_uparrow", 7);
    CL_RegisterMaterialNoMip((const char *)"hudscoreboardscroll_upkey", 7);
    CL_RegisterMaterialNoMip((const char *)"hudscoreboardscroll_downarrow", 7);
    CL_RegisterMaterialNoMip((const char *)"hudscoreboardscroll_downkey", 7);
    CL_RegisterMaterialNoMip(Dvar_GetString((const char *)"g_ScoresBanner_Spectators"), 7);
    CL_RegisterMaterialNoMip(Dvar_GetString((const char *)"g_ScoresBanner_Axis"), 7);
    CL_RegisterMaterialNoMip(Dvar_GetString((const char *)"g_ScoresBanner_Allies"), 7);
    CL_RegisterMaterialNoMip(Dvar_GetString((const char *)"g_ScoresBanner_None"), 7);
    CL_RegisterMaterialNoMip((const char *)"voice_on", 7);
    CL_RegisterMaterialNoMip((const char *)"voice_off", 7);
}

