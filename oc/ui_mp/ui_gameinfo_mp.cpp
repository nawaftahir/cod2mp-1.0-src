#include "common_types.h"
#include "imports.h"
#include <string.h>

extern const char *String_Alloc(const char *str);
extern char *Com_Parse( const char *( *data_p ) );
extern char *Com_ParseOnLine( const char *( *data_p ) );
extern void I_strncpyz(char *dest, const char *src, int destsize);
extern void Info_SetValueForKey(char *s, const char *key, const char *value);
extern void Com_Printf(const char *msg, ...);
extern void Com_EndParseSession(void);
extern void Com_EndParseSessionInl(void);
extern char *va( const char *format, ... );
extern void *UI_Alloc(int size, int align);
extern int FS_GetFileList(const char *path, const char *extension, FsListBehavior behavior, char *listBuf, int bufSize);
extern int FS_FOpenFileByMode(const char *filename, int *f, fsMode_t mode);
extern int FS_Read(void *buffer, int len, int f);
extern void FS_FCloseFile(fileHandle_t f);
extern const char *Info_ValueForKey( const char *s, const char *key );
extern MaterialHandle CL_RegisterMaterialNoMip(const char *name, int imageTrack);
extern void Com_BeginParseSession(const char *name);
extern int I_stricmp(const char *s1, const char *s2);

extern sharedUiInfo_t sharedUiInfo;

static int ui_numArenas;
static char *ui_arenaInfos[64];

int UI_ParseInfos(const char *buf, int max, char **infos);
void UI_LoadArenas(void);

int UI_ParseInfos(const char *buf, int max, char **infos)
{
    int count = 0;
    char info[0x400];
    char key[0x400];
    const char *token;
    char *vaStr;
    int infoLen;
    int vaLen;
    int allocSize;

    while (1) {
        token = Com_Parse(&buf);
        if (*token == '\0')
            break;

        if (strcmp(token, "{") != 0) {
            Com_Printf("Missing { in info file\n");
            return count;
        }

        if (count == max) {
            Com_Printf("Max infos exceeded\n");
            return count;
        }

        info[0] = '\0';

        while (1) {
            token = Com_Parse(&buf);
            if (*token == '\0') {
                Com_Printf("Unexpected end of info file\n");
                break;
            }
            if (strcmp(token, "}") == 0)
                break;

            I_strncpyz(key, token, 0x400);

            token = Com_ParseOnLine(&buf);
            if (*token == '\0')
                token = "<NULL>";

            Info_SetValueForKey(info, key, token);
        }

        infoLen = strlen(info);
        vaStr = (char *)va("%d", 0x40);
        vaLen = strlen(vaStr) + 1;
        allocSize = vaLen + infoLen + 5;

        infos[count] = (char *)UI_Alloc(allocSize, 1);
        if (infos[count] == NULL)
            continue;

        strcpy(infos[count], info);
        count++;
    }

    return count;
}

static void UI_LoadArenasFromFile(const char *filename)
{
    int len;
    fileHandle_t f;
    char buf[0x2000];

    len = FS_FOpenFileByMode(filename, &f, FS_READ);
    if (!f) {
        Com_Printf("^1file not found: %s\n", filename);
        return;
    }
    if (len >= 0x2000) {
        Com_Printf("^1file too large: %s is %i, max allowed is %i", filename, len, 0x2000);
        FS_FCloseFile(f);
        return;
    }

    FS_Read(buf, len, f);
    buf[len] = 0;
    FS_FCloseFile(f);

    ui_numArenas += UI_ParseInfos(buf, 64 - ui_numArenas, &ui_arenaInfos[ui_numArenas]);
}

void UI_LoadArenas(void)
{
    int numdirs;
    char filename[128];
    char dirlist[1024];
    char *dirptr;
    int i, n, j;
    int dirlen;
    const char *type;
    const char *token;
    const char *pszText;

    ui_numArenas = 0;
    sharedUiInfo.mapCount = 0;

    numdirs = FS_GetFileList("mp", "arena", FS_LIST_PURE_ONLY, dirlist, 1024);
    dirptr = dirlist;
    for (i = 0; i < numdirs; i++, dirptr += dirlen + 1) {
        dirlen = strlen(dirptr);
        sprintf(filename, "%s/%s", "mp", dirptr);
        UI_LoadArenasFromFile(filename);
    }

    for (n = 0; n < ui_numArenas; n++) {
        sharedUiInfo.mapList[sharedUiInfo.mapCount].mapLoadName = String_Alloc(Info_ValueForKey(ui_arenaInfos[n], "map"));
        sharedUiInfo.mapList[sharedUiInfo.mapCount].mapName = String_Alloc(Info_ValueForKey(ui_arenaInfos[n], "longname"));
        sharedUiInfo.mapList[sharedUiInfo.mapCount].cinematic = -1;
        sharedUiInfo.mapList[sharedUiInfo.mapCount].levelShot = 0;
        sharedUiInfo.mapList[sharedUiInfo.mapCount].imageName =
            String_Alloc(va("loadscreen_%s", sharedUiInfo.mapList[sharedUiInfo.mapCount].mapLoadName));
        sharedUiInfo.mapList[sharedUiInfo.mapCount].levelShot =
            CL_RegisterMaterialNoMip(sharedUiInfo.mapList[sharedUiInfo.mapCount].imageName, 3);

        type = Info_ValueForKey(ui_arenaInfos[n], "gametype");
        if (type && *type) {
            sharedUiInfo.mapList[sharedUiInfo.mapCount].typeBits = 0;
            Com_BeginParseSession(va(".arena files : %s", sharedUiInfo.mapList[sharedUiInfo.mapCount].mapLoadName));
            pszText = type;
            while (1) {
                token = Com_Parse(&pszText);
                if (!token || !*token)
                    break;
                for (j = 0; j < sharedUiInfo.numGameTypes; j++) {
                    if (!I_stricmp(token, sharedUiInfo.gameTypes[j].gameType))
                        sharedUiInfo.mapList[sharedUiInfo.mapCount].typeBits |= 1 << j;
                }
            }
            Com_EndParseSessionInl();
        } else {
            sharedUiInfo.mapList[sharedUiInfo.mapCount].typeBits = -1;
        }

        sharedUiInfo.mapCount++;
        if (sharedUiInfo.mapCount >= 128)
            break;
    }
}

