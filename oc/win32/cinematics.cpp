#include <math.h>
#include "common_types.h"
#include "imports.h"

struct cinematics_t cin;
cin_cache cinTable[16];

short unsigned int vq4[65536];
long int ROQ_UG_tab[256];
long int ROQ_VG_tab[256];
short unsigned int vq8[262144];
short unsigned int vq2[16384];
long int ROQ_VR_tab[256];
long int ROQ_YY_tab[256];
long int ROQ_UB_tab[256];
extern refexport_t re;
extern snd_local_t g_snd;
extern clientConnection_t clientConnections[1];
#define clc clientConnections[0]
extern clientStatic_t cls;

/* ((void *)&clc) holds the address of the `clc` POINTER, not of the connection itself, so
   reaching the connection state needs two indirections -- the same form the rest of the
   client code uses (`&::clc`). Treating ((void *)&clc) as if it pointed
   straight at the connection would read and, worse, WRITE the clc pointer itself. */
#define CLC_CONN (&::clc)

extern dvar_t *nextmap;
extern const dvar_t *cl_inGameVideo;

extern void Com_DPrintf(const char *fmt, ...);
extern void SND_StopSounds(snd_stopsounds_arg_t);
extern void SND_EndRawSamples(void);
extern int SND_RawSamplesTime(void);
extern void FS_FCloseFile(fileHandle_t);
extern int FS_FOpenFileRead(const char *, fileHandle_t *, int);
extern char *va( const char *format, ... );
extern void Cbuf_AddText(const char *);
extern void Dvar_SetString(dvar_t *, const char *);
extern qboolean UI_SetActiveMenu(int);
extern int CL_ScaledMilliseconds(void);
extern const char *Cmd_Argv( int arg );
extern void CL_StopLogo(void);
extern void SND_FadeAllSounds(float, int);
extern void CL_LookupColor(unsigned char, float *);
extern float floorf(float);
extern float ceilf(float);

extern long int ROQ_YY_tab[256];
extern long int ROQ_UB_tab[256];
extern long int ROQ_UG_tab[256];
extern long int ROQ_VG_tab[256];
extern long int ROQ_VR_tab[256];
extern short unsigned int vq2[16384];
extern short unsigned int vq4[65536];
extern short unsigned int vq8[262144];
extern struct cinematics_t cin;
extern cin_cache cinTable[16];
__attribute__((used)) int CL_handle = -1;
__attribute__((used, section(".data.currentHandle"), aligned(4))) int currentHandle = -1;
__attribute__((used, section(".data.currentHandle"), aligned(4))) static int currentHandle_pad[6];
;

static void recurseQuad(long int startX, long int startY, long int quadSize, long int xOff, long int yOff);
static void RoQPrepMcomp(long int xoff, long int yoff);
static void RoQShutdown(void);
e_status ROQ_StopCinematicFromHandle(int handle);
void ROQ_SetExtentsFromHandle(int handle, int x, int y, int w, int h);
void ROQ_UploadCinematicFromHandle(int handle);
static void blitVQQuad32fs(byte **status, unsigned char *data);
int ROQ_PlayCinematic(const char *arg, int x, int y, int w, int h, int systemBits);
void ROQ_DrawCinematicFromHandle(int handle);
void ROQ_DrawCinematic(void);
static void RoQReset(void);
void initRoQ(void);
void ROQ_CloseAllVideos(void);
e_status ROQ_RunCinematicFromHandle(int handle);
void ROQ_RunCinematic(void);
void ROQ_PlayCinematic_f(void);

static void RoQ_init(void);
static void RoQInterrupt(void);

e_status ROQ_StopCinematicFromHandle(int handle);
static void RoQ_init(void);
e_status ROQ_RunCinematicFromHandle(int handle);

extern int Com_sprintf(char *dest, size_t size, const char *format, ...);
extern int I_stricmp(const char *s1, const char *s2);
extern void Com_Error(errorParm_t level, const char *fmt, ...);
extern void Com_Memset(void *dest, const int val, int count);
extern int FS_Read(void *buffer, int len, fileHandle_t f);
extern void SND_RawSamples(int samples, int rate, int width, int channels, const byte *data);
extern void Con_Close(void);

#define ROQ_QUAD_INFO 0x1001
#define ROQ_CODEBOOK 0x1002
#define ROQ_QUAD_VQ 0x1011
#define ROQ_QUAD_JPEG 0x1012
#define ROQ_QUAD_HANG 0x1013
#define ROQ_PACKET 0x1030
#define ZA_SOUND_MONO 0x1020
#define ZA_SOUND_STEREO 0x1021
extern "C" void Com_Memcpy( void *dest, const void *src, int count );

#define VQ2TO4(a, b, c, d) { \
    *c++ = a[0]; \
    *d++ = a[0]; \
    *d++ = a[0]; \
    *c++ = a[1]; \
    *d++ = a[1]; \
    *d++ = a[1]; \
    *c++ = b[0]; \
    *d++ = b[0]; \
    *d++ = b[0]; \
    *c++ = b[1]; \
    *d++ = b[1]; \
    *d++ = b[1]; \
    *d++ = a[0]; \
    *d++ = a[0]; \
    *d++ = a[1]; \
    *d++ = a[1]; \
    *d++ = b[0]; \
    *d++ = b[0]; \
    *d++ = b[1]; \
    *d++ = b[1]; \
    a += 2; b += 2; }
#define VQ2TO2(a, b, c, d) { \
    *c++ = *a; \
    *d++ = *a; \
    *d++ = *a; \
    *c++ = *b; \
    *d++ = *b; \
    *d++ = *b; \
    *d++ = *a; \
    *d++ = *a; \
    *d++ = *b; \
    *d++ = *b; \
    a++; b++; }

void ROQ_RunCinematic(void)
{
    if (CL_handle >= 0 && CL_handle < 16) {
        ROQ_RunCinematicFromHandle(CL_handle);
    }
}

void ROQ_CloseAllVideos(void)
{
    int i;

    for (i = 0; i < 16; i++) {
        if (cinTable[i].fileName[0] != 0) {
            ROQ_StopCinematicFromHandle(i);
        }
    }
}

void ROQ_StopCinematic(void)
{
    if (CL_handle >= 0 && CL_handle < 16) {
        ROQ_StopCinematicFromHandle(CL_handle);
        SND_StopSounds(SND_STOP_ALL);
        CL_handle = -1;
    }
}

static int CIN_findHandle(void)
{
    int i;
    for (i = 0; i < 16; i++) {
        if (cinTable[i].fileName[0] == '\0') {
            return i;
        }
    }
    Com_Error(ERR_DROP, "CIN_findHandle: none free");
    return 0;
}

static void RllSetupTable(void)
{
    int z;

    for (z = 0; z < 128; z++) {
        cin.sqrTable[z] = (short)(z * z);
        cin.sqrTable[z + 128] = (short)(-cin.sqrTable[z]);
    }
}

// retail 0x4acf70, unreferenced; Q3 RllDecodeMonoToMono (SUPPORTED)
long RllDecodeMonoToMono(unsigned char *from, short *to, unsigned int size, char signedOutput, unsigned short flag)
{
    unsigned int z;
    short prev;

    if (signedOutput)
        prev = flag - 0x8000;
    else
        prev = flag;

    for (z = 0; z < size; z++) {
        prev = to[z] = (short)(prev + cin.sqrTable[from[z]]);
    }
    return size;
}

static long RllDecodeMonoToStereo(unsigned char *from, short *to, unsigned int size, char signedOutput, unsigned short flag)
{
    unsigned int z;
    short prev;

    if (signedOutput)
        prev = flag - 0x8000;
    else
        prev = flag;

    for (z = 0; z < size; z++) {
        prev = (short)(prev + cin.sqrTable[from[z]]);
        to[z * 2 + 0] = to[z * 2 + 1] = (short)(prev);
    }

    return size;
}

long RllDecodeStereoToStereo(unsigned char *from, short *to, unsigned int size, char signedOutput, unsigned short flag)
{
    unsigned int z;
    unsigned char *zz = from;
    short prevL, prevR;

    if (signedOutput) {
        prevL = (flag & 0xff00) - 0x8000;
        prevR = ((flag & 0x00ff) << 8) - 0x8000;
    } else {
        prevL = flag & 0xff00;
        prevR = (flag & 0x00ff) << 8;
    }

    for (z = 0; z < size; z += 2) {
        prevL = (short)(prevL + cin.sqrTable[*zz++]);
        prevR = (short)(prevR + cin.sqrTable[*zz++]);
        to[z + 0] = (short)(prevL);
        to[z + 1] = (short)(prevR);
    }

    return (size >> 1);
}

long RllDecodeStereoToMono(unsigned char *from, short *to, unsigned int size, char signedOutput, unsigned short flag)
{
    unsigned int z;
    short prevL, prevR;

    if (signedOutput) {
        prevL = (flag & 0xff00) - 0x8000;
        prevR = ((flag & 0x00ff) << 8) - 0x8000;
    } else {
        prevL = flag & 0xff00;
        prevR = (flag & 0x00ff) << 8;
    }

    for (z = 0; z < size; z += 1) {
        prevL = prevL + cin.sqrTable[from[z * 2]];
        prevR = prevR + cin.sqrTable[from[z * 2 + 1]];
        to[z] = (short)((prevL + prevR) / 2);
    }

    return size;
}

static void move8_32(byte *src, byte *dst, int spl)
{
    __int64 *dsrc;
    __int64 *ddst;
    int dspl;

    dsrc = (__int64 *)src;
    ddst = (__int64 *)dst;
    dspl = spl >> 3;

    ddst[0] = dsrc[0];
    ddst[1] = dsrc[1];
    ddst[2] = dsrc[2];
    ddst[3] = dsrc[3];
    dsrc += dspl;
    ddst += dspl;
    ddst[0] = dsrc[0];
    ddst[1] = dsrc[1];
    ddst[2] = dsrc[2];
    ddst[3] = dsrc[3];
    dsrc += dspl;
    ddst += dspl;
    ddst[0] = dsrc[0];
    ddst[1] = dsrc[1];
    ddst[2] = dsrc[2];
    ddst[3] = dsrc[3];
    dsrc += dspl;
    ddst += dspl;
    ddst[0] = dsrc[0];
    ddst[1] = dsrc[1];
    ddst[2] = dsrc[2];
    ddst[3] = dsrc[3];
    dsrc += dspl;
    ddst += dspl;
    ddst[0] = dsrc[0];
    ddst[1] = dsrc[1];
    ddst[2] = dsrc[2];
    ddst[3] = dsrc[3];
    dsrc += dspl;
    ddst += dspl;
    ddst[0] = dsrc[0];
    ddst[1] = dsrc[1];
    ddst[2] = dsrc[2];
    ddst[3] = dsrc[3];
    dsrc += dspl;
    ddst += dspl;
    ddst[0] = dsrc[0];
    ddst[1] = dsrc[1];
    ddst[2] = dsrc[2];
    ddst[3] = dsrc[3];
    dsrc += dspl;
    ddst += dspl;
    ddst[0] = dsrc[0];
    ddst[1] = dsrc[1];
    ddst[2] = dsrc[2];
    ddst[3] = dsrc[3];
}

static void move4_32(byte *src, byte *dst, int spl)
{
    __int64 *dsrc;
    __int64 *ddst;
    int dspl;

    dsrc = (__int64 *)src;
    ddst = (__int64 *)dst;
    dspl = spl >> 3;

    ddst[0] = dsrc[0];
    ddst[1] = dsrc[1];
    dsrc += dspl;
    ddst += dspl;
    ddst[0] = dsrc[0];
    ddst[1] = dsrc[1];
    dsrc += dspl;
    ddst += dspl;
    ddst[0] = dsrc[0];
    ddst[1] = dsrc[1];
    dsrc += dspl;
    ddst += dspl;
    ddst[0] = dsrc[0];
    ddst[1] = dsrc[1];
}

static void blit8_32(byte *src, byte *dst, int spl)
{
    __int64 *dsrc;
    __int64 *ddst;
    int dspl;

    dsrc = (__int64 *)src;
    ddst = (__int64 *)dst;
    dspl = spl >> 3;

    ddst[0] = dsrc[0];
    ddst[1] = dsrc[1];
    ddst[2] = dsrc[2];
    ddst[3] = dsrc[3];
    dsrc += 4;
    ddst += dspl;
    ddst[0] = dsrc[0];
    ddst[1] = dsrc[1];
    ddst[2] = dsrc[2];
    ddst[3] = dsrc[3];
    dsrc += 4;
    ddst += dspl;
    ddst[0] = dsrc[0];
    ddst[1] = dsrc[1];
    ddst[2] = dsrc[2];
    ddst[3] = dsrc[3];
    dsrc += 4;
    ddst += dspl;
    ddst[0] = dsrc[0];
    ddst[1] = dsrc[1];
    ddst[2] = dsrc[2];
    ddst[3] = dsrc[3];
    dsrc += 4;
    ddst += dspl;
    ddst[0] = dsrc[0];
    ddst[1] = dsrc[1];
    ddst[2] = dsrc[2];
    ddst[3] = dsrc[3];
    dsrc += 4;
    ddst += dspl;
    ddst[0] = dsrc[0];
    ddst[1] = dsrc[1];
    ddst[2] = dsrc[2];
    ddst[3] = dsrc[3];
    dsrc += 4;
    ddst += dspl;
    ddst[0] = dsrc[0];
    ddst[1] = dsrc[1];
    ddst[2] = dsrc[2];
    ddst[3] = dsrc[3];
    dsrc += 4;
    ddst += dspl;
    ddst[0] = dsrc[0];
    ddst[1] = dsrc[1];
    ddst[2] = dsrc[2];
    ddst[3] = dsrc[3];
}

static void blit4_32(byte *src, byte *dst, int spl)
{
    __int64 *dsrc;
    __int64 *ddst;
    int dspl;

    dsrc = (__int64 *)src;
    ddst = (__int64 *)dst;
    dspl = spl >> 3;

    ddst[0] = dsrc[0];
    ddst[1] = dsrc[1];
    dsrc += 2;
    ddst += dspl;
    ddst[0] = dsrc[0];
    ddst[1] = dsrc[1];
    dsrc += 2;
    ddst += dspl;
    ddst[0] = dsrc[0];
    ddst[1] = dsrc[1];
    dsrc += 2;
    ddst += dspl;
    ddst[0] = dsrc[0];
    ddst[1] = dsrc[1];
}

static void blit2_32(byte *src, byte *dst, int spl)
{
    __int64 *dsrc;
    __int64 *ddst;
    int dspl;

    dsrc = (__int64 *)src;
    ddst = (__int64 *)dst;
    dspl = spl >> 3;

    ddst[0] = dsrc[0];
    ddst[dspl] = dsrc[1];
}

static void blitVQQuad32fs(byte **status, unsigned char *data)
{
    unsigned short newd;
    unsigned short celdata;
    unsigned short code;
    unsigned int index;
    unsigned int i;
    int spl;

    newd = 0;
    celdata = 0;
    index = 0;
    spl = cinTable[currentHandle].samplesPerLine;

    do {
        if (!newd) {
            newd = 7;
            celdata = data[0] + data[1] * 256;
            data += 2;
        } else {
            newd--;
        }

        code = (unsigned short)(celdata & 0xc000);
        celdata <<= 2;

        switch (code) {
        case 0x8000:
            blit8_32((byte *)&vq8[(*data) * 128], status[index], spl);
            data++;
            index += 5;
            break;

        case 0xc000:
            index++;
            for (i = 0; i < 4; i++) {
                if (!newd) {
                    newd = 7;
                    celdata = data[0] + data[1] * 256;
                    data += 2;
                } else {
                    newd--;
                }

                code = (unsigned short)(celdata & 0xc000);
                celdata <<= 2;

                switch (code) {
                case 0x8000:
                    blit4_32((byte *)&vq4[(*data) * 32], status[index], spl);
                    data++;
                    break;

                case 0xc000:
                    blit2_32((byte *)&vq2[(*data) * 8], status[index], spl);
                    data++;
                    blit2_32((byte *)&vq2[(*data) * 8], status[index] + 8, spl);
                    data++;
                    blit2_32((byte *)&vq2[(*data) * 8], status[index] + spl * 2, spl);
                    data++;
                    blit2_32((byte *)&vq2[(*data) * 8], status[index] + spl * 2 + 8, spl);
                    data++;
                    break;

                case 0x4000:
                    move4_32(status[index] + cin.mcomp[*data], status[index], spl);
                    data++;
                    break;
                }
                index++;
            }
            break;

        case 0x4000:
            move8_32(status[index] + cin.mcomp[*data], status[index], spl);
            data++;
            index += 5;
            break;

        case 0x0000:
            index += 5;
            break;
        }
    } while (status[index] != NULL);
}

static void ROQ_GenYUVTables(void)
{
    float t_ub, t_vr, t_ug, t_vg;
    long i;

    t_ub = (1.77200f / 2.0f) * (float)(1 << 6) + 0.5f;
    t_vr = (1.40200f / 2.0f) * (float)(1 << 6) + 0.5f;
    t_ug = (0.34414f / 2.0f) * (float)(1 << 6) + 0.5f;
    t_vg = (0.71414f / 2.0f) * (float)(1 << 6) + 0.5f;
    for (i = 0; i < 256; i++) {
        float x = (float)(2 * i - 255);

        ROQ_UB_tab[i] = (long)((t_ub * x) + (1 << 5));
        ROQ_VR_tab[i] = (long)((t_vr * x) + (1 << 5));
        ROQ_UG_tab[i] = (long)((-t_ug * x));
        ROQ_VG_tab[i] = (long)((-t_vg * x) + (1 << 5));
        ROQ_YY_tab[i] = (long)((i << 6) | (i >> 2));
    }
}

static unsigned short yuv_to_rgb(long y, long u, long v)
{
    long r, g, b, YY = (long)(ROQ_YY_tab[(y)]);

    r = (YY + ROQ_VR_tab[v]) >> 9;
    g = (YY + ROQ_UG_tab[u] + ROQ_VG_tab[v]) >> 8;
    b = (YY + ROQ_UB_tab[u]) >> 9;

    if (r < 0) r = 0;
    if (g < 0) g = 0;
    if (b < 0) b = 0;
    if (r > 31) r = 31;
    if (g > 63) g = 63;
    if (b > 31) b = 31;

    return (unsigned short)((r << 11) + (g << 5) + (b));
}

static unsigned int yuv_to_rgb24(long y, long u, long v)
{
    long r, g, b, YY = (long)(ROQ_YY_tab[(y)]);

    r = (YY + ROQ_VR_tab[v]) >> 6;
    g = (YY + ROQ_UG_tab[u] + ROQ_VG_tab[v]) >> 6;
    b = (YY + ROQ_UB_tab[u]) >> 6;

    if (r < 0) r = 0;
    if (g < 0) g = 0;
    if (b < 0) b = 0;
    if (r > 255) r = 255;
    if (g > 255) g = 255;
    if (b > 255) b = 255;

    return ((r) | (g << 8) | (b << 16) | (255 << 24));
}

static void decodeCodeBook(byte *input, unsigned short roq_flags)
{
    long i, j, two, four;
    unsigned short *aptr, *bptr, *cptr, *dptr;
    long y0, y1, y2, y3, cr, cb;
    byte *bbptr, *baptr, *bcptr, *bdptr;
    unsigned int *iaptr, *ibptr, *icptr, *idptr;

    if (!roq_flags) {
        two = four = 256;
    } else {
        two = roq_flags >> 8;
        if (!two) two = 256;
        four = roq_flags & 0xff;
    }

    four *= 2;

    bptr = (unsigned short *)vq2;

    if (!cinTable[currentHandle].half) {
        if (!cinTable[currentHandle].smootheddouble) {
            // normal height
            if (cinTable[currentHandle].samplesPerPixel == 2) {
                for (i = 0; i < two; i++) {
                    y0 = (long)*input++;
                    y1 = (long)*input++;
                    y2 = (long)*input++;
                    y3 = (long)*input++;
                    cr = (long)*input++;
                    cb = (long)*input++;
                    *bptr++ = yuv_to_rgb(y0, cr, cb);
                    *bptr++ = yuv_to_rgb(y1, cr, cb);
                    *bptr++ = yuv_to_rgb(y2, cr, cb);
                    *bptr++ = yuv_to_rgb(y3, cr, cb);
                }

                cptr = (unsigned short *)vq4;
                dptr = (unsigned short *)vq8;

                for (i = 0; i < four; i++) {
                    aptr = (unsigned short *)vq2 + (*input++) * 4;
                    bptr = (unsigned short *)vq2 + (*input++) * 4;
                    for (j = 0; j < 2; j++)
                        VQ2TO4(aptr, bptr, cptr, dptr);
                }
            } else if (cinTable[currentHandle].samplesPerPixel == 4) {
                ibptr = (unsigned int *)bptr;
                for (i = 0; i < two; i++) {
                    y0 = (long)*input++;
                    y1 = (long)*input++;
                    y2 = (long)*input++;
                    y3 = (long)*input++;
                    cr = (long)*input++;
                    cb = (long)*input++;
                    *ibptr++ = yuv_to_rgb24(y0, cr, cb);
                    *ibptr++ = yuv_to_rgb24(y1, cr, cb);
                    *ibptr++ = yuv_to_rgb24(y2, cr, cb);
                    *ibptr++ = yuv_to_rgb24(y3, cr, cb);
                }

                icptr = (unsigned int *)vq4;
                idptr = (unsigned int *)vq8;

                for (i = 0; i < four; i++) {
                    iaptr = (unsigned int *)vq2 + (*input++) * 4;
                    ibptr = (unsigned int *)vq2 + (*input++) * 4;
                    for (j = 0; j < 2; j++)
                        VQ2TO4(iaptr, ibptr, icptr, idptr);
                }
            } else if (cinTable[currentHandle].samplesPerPixel == 1) {
                bbptr = (byte *)bptr;
                for (i = 0; i < two; i++) {
                    *bbptr++ = cinTable[currentHandle].gray[*input++];
                    *bbptr++ = cinTable[currentHandle].gray[*input++];
                    *bbptr++ = cinTable[currentHandle].gray[*input++];
                    *bbptr++ = cinTable[currentHandle].gray[*input]; input += 3;
                }

                bcptr = (byte *)vq4;
                bdptr = (byte *)vq8;

                for (i = 0; i < four; i++) {
                    baptr = (byte *)vq2 + (*input++) * 4;
                    bbptr = (byte *)vq2 + (*input++) * 4;
                    for (j = 0; j < 2; j++)
                        VQ2TO4(baptr, bbptr, bcptr, bdptr);
                }
            }
        } else {
            // double height, smoothed
            if (cinTable[currentHandle].samplesPerPixel == 2) {
                for (i = 0; i < two; i++) {
                    y0 = (long)*input++;
                    y1 = (long)*input++;
                    y2 = (long)*input++;
                    y3 = (long)*input++;
                    cr = (long)*input++;
                    cb = (long)*input++;
                    *bptr++ = yuv_to_rgb(y0, cr, cb);
                    *bptr++ = yuv_to_rgb(y1, cr, cb);
                    *bptr++ = yuv_to_rgb(((y0 * 3) + y2) / 4, cr, cb);
                    *bptr++ = yuv_to_rgb(((y1 * 3) + y3) / 4, cr, cb);
                    *bptr++ = yuv_to_rgb((y0 + (y2 * 3)) / 4, cr, cb);
                    *bptr++ = yuv_to_rgb((y1 + (y3 * 3)) / 4, cr, cb);
                    *bptr++ = yuv_to_rgb(y2, cr, cb);
                    *bptr++ = yuv_to_rgb(y3, cr, cb);
                }

                cptr = (unsigned short *)vq4;
                dptr = (unsigned short *)vq8;

                for (i = 0; i < four; i++) {
                    aptr = (unsigned short *)vq2 + (*input++) * 8;
                    bptr = (unsigned short *)vq2 + (*input++) * 8;
                    for (j = 0; j < 2; j++) {
                        VQ2TO4(aptr, bptr, cptr, dptr);
                        VQ2TO4(aptr, bptr, cptr, dptr);
                    }
                }
            } else if (cinTable[currentHandle].samplesPerPixel == 4) {
                ibptr = (unsigned int *)bptr;
                for (i = 0; i < two; i++) {
                    y0 = (long)*input++;
                    y1 = (long)*input++;
                    y2 = (long)*input++;
                    y3 = (long)*input++;
                    cr = (long)*input++;
                    cb = (long)*input++;
                    *ibptr++ = yuv_to_rgb24(y0, cr, cb);
                    *ibptr++ = yuv_to_rgb24(y1, cr, cb);
                    *ibptr++ = yuv_to_rgb24(((y0 * 3) + y2) / 4, cr, cb);
                    *ibptr++ = yuv_to_rgb24(((y1 * 3) + y3) / 4, cr, cb);
                    *ibptr++ = yuv_to_rgb24((y0 + (y2 * 3)) / 4, cr, cb);
                    *ibptr++ = yuv_to_rgb24((y1 + (y3 * 3)) / 4, cr, cb);
                    *ibptr++ = yuv_to_rgb24(y2, cr, cb);
                    *ibptr++ = yuv_to_rgb24(y3, cr, cb);
                }

                icptr = (unsigned int *)vq4;
                idptr = (unsigned int *)vq8;

                for (i = 0; i < four; i++) {
                    iaptr = (unsigned int *)vq2 + (*input++) * 8;
                    ibptr = (unsigned int *)vq2 + (*input++) * 8;
                    for (j = 0; j < 2; j++) {
                        VQ2TO4(iaptr, ibptr, icptr, idptr);
                        VQ2TO4(iaptr, ibptr, icptr, idptr);
                    }
                }
            } else if (cinTable[currentHandle].samplesPerPixel == 1) {
                bbptr = (byte *)bptr;
                for (i = 0; i < two; i++) {
                    y0 = (long)*input++;
                    y1 = (long)*input++;
                    y2 = (long)*input++;
                    y3 = (long)*input; input += 3;
                    *bbptr++ = cinTable[currentHandle].gray[y0];
                    *bbptr++ = cinTable[currentHandle].gray[y1];
                    *bbptr++ = cinTable[currentHandle].gray[((y0 * 3) + y2) / 4];
                    *bbptr++ = cinTable[currentHandle].gray[((y1 * 3) + y3) / 4];
                    *bbptr++ = cinTable[currentHandle].gray[(y0 + (y2 * 3)) / 4];
                    *bbptr++ = cinTable[currentHandle].gray[(y1 + (y3 * 3)) / 4];
                    *bbptr++ = cinTable[currentHandle].gray[y2];
                    *bbptr++ = cinTable[currentHandle].gray[y3];
                }

                bcptr = (byte *)vq4;
                bdptr = (byte *)vq8;

                for (i = 0; i < four; i++) {
                    baptr = (byte *)vq2 + (*input++) * 8;
                    bbptr = (byte *)vq2 + (*input++) * 8;
                    for (j = 0; j < 2; j++) {
                        VQ2TO4(baptr, bbptr, bcptr, bdptr);
                        VQ2TO4(baptr, bbptr, bcptr, bdptr);
                    }
                }
            }
        }
    } else {
        // 1/4 screen
        if (cinTable[currentHandle].samplesPerPixel == 2) {
            for (i = 0; i < two; i++) {
                y0 = (long)*input; input += 2;
                y2 = (long)*input; input += 2;
                cr = (long)*input++;
                cb = (long)*input++;
                *bptr++ = yuv_to_rgb(y0, cr, cb);
                *bptr++ = yuv_to_rgb(y2, cr, cb);
            }

            cptr = (unsigned short *)vq4;
            dptr = (unsigned short *)vq8;

            for (i = 0; i < four; i++) {
                aptr = (unsigned short *)vq2 + (*input++) * 2;
                bptr = (unsigned short *)vq2 + (*input++) * 2;
                for (j = 0; j < 2; j++) {
                    VQ2TO2(aptr, bptr, cptr, dptr);
                }
            }
        } else if (cinTable[currentHandle].samplesPerPixel == 1) {
            bbptr = (byte *)bptr;

            for (i = 0; i < two; i++) {
                *bbptr++ = cinTable[currentHandle].gray[*input]; input += 2;
                *bbptr++ = cinTable[currentHandle].gray[*input]; input += 4;
            }

            bcptr = (byte *)vq4;
            bdptr = (byte *)vq8;

            for (i = 0; i < four; i++) {
                baptr = (byte *)vq2 + (*input++) * 2;
                bbptr = (byte *)vq2 + (*input++) * 2;
                for (j = 0; j < 2; j++) {
                    VQ2TO2(baptr, bbptr, bcptr, bdptr);
                }
            }
        } else if (cinTable[currentHandle].samplesPerPixel == 4) {
            ibptr = (unsigned int *)bptr;
            for (i = 0; i < two; i++) {
                y0 = (long)*input; input += 2;
                y2 = (long)*input; input += 2;
                cr = (long)*input++;
                cb = (long)*input++;
                *ibptr++ = yuv_to_rgb24(y0, cr, cb);
                *ibptr++ = yuv_to_rgb24(y2, cr, cb);
            }

            icptr = (unsigned int *)vq4;
            idptr = (unsigned int *)vq8;

            for (i = 0; i < four; i++) {
                iaptr = (unsigned int *)vq2 + (*input++) * 2;
                ibptr = (unsigned int *)vq2 + (*input++) * 2;
                for (j = 0; j < 2; j++) {
                    VQ2TO2(iaptr, ibptr, icptr, idptr);
                }
            }
        }
    }
}

static void recurseQuad(long startX, long startY, long quadSize, long xOff, long yOff)
{
    byte *scroff;
    long bigx, bigy, lowx, lowy, useY;
    long offset;

    offset = cinTable[currentHandle].screenDelta;

    lowx = lowy = 0;
    bigx = cinTable[currentHandle].xsize;
    bigy = cinTable[currentHandle].ysize;

    if (bigx > cinTable[currentHandle].CIN_WIDTH) bigx = cinTable[currentHandle].CIN_WIDTH;
    if (bigy > cinTable[currentHandle].CIN_HEIGHT) bigy = cinTable[currentHandle].CIN_HEIGHT;

    if ((startX >= lowx) && (startX + quadSize) <= (bigx) && (startY + quadSize) <= (bigy) && (startY >= lowy) && quadSize <= 8) {
        useY = startY;
        scroff = cin.linbuf + (useY + ((cinTable[currentHandle].CIN_HEIGHT - bigy) >> 1) + yOff) * (cinTable[currentHandle].samplesPerLine) + (((startX + xOff)) * cinTable[currentHandle].samplesPerPixel);

        cin.qStatus[0][cinTable[currentHandle].onQuad] = scroff;
        cin.qStatus[1][cinTable[currentHandle].onQuad++] = scroff + offset;
    }

    if (quadSize != 4) {
        quadSize >>= 1;
        recurseQuad(startX, startY, quadSize, xOff, yOff);
        recurseQuad(startX + quadSize, startY, quadSize, xOff, yOff);
        recurseQuad(startX, startY + quadSize, quadSize, xOff, yOff);
        recurseQuad(startX + quadSize, startY + quadSize, quadSize, xOff, yOff);
    }
}

static void setupQuad(long xOff, long yOff)
{
    long numQuadCels, i, x, y;
    byte *temp;

    if (xOff == cin.oldXOff && yOff == cin.oldYOff && cinTable[currentHandle].ysize == cin.oldysize && cinTable[currentHandle].xsize == cin.oldxsize) {
        return;
    }

    cin.oldXOff = xOff;
    cin.oldYOff = yOff;
    cin.oldysize = cinTable[currentHandle].ysize;
    cin.oldxsize = cinTable[currentHandle].xsize;

    numQuadCels = (cinTable[currentHandle].xsize * cinTable[currentHandle].ysize) / (16);
    numQuadCels += numQuadCels / 4;
    numQuadCels += 64; // for overflow

    cinTable[currentHandle].onQuad = 0;

    for (y = 0; y < (long)cinTable[currentHandle].ysize; y += 16)
        for (x = 0; x < (long)cinTable[currentHandle].xsize; x += 16)
            recurseQuad(x, y, 16, xOff, yOff);

    temp = NULL;

    for (i = (numQuadCels - 64); i < numQuadCels; i++) {
        cin.qStatus[0][i] = temp; // eoq
        cin.qStatus[1][i] = temp; // eoq
    }
}

static void readQuadInfo(byte *qData)
{
    if (currentHandle < 0) return;

    cinTable[currentHandle].xsize = qData[0] + qData[1] * 256;
    cinTable[currentHandle].ysize = qData[2] + qData[3] * 256;
    cinTable[currentHandle].maxsize = qData[4] + qData[5] * 256;
    cinTable[currentHandle].minsize = qData[6] + qData[7] * 256;

    cinTable[currentHandle].CIN_HEIGHT = cinTable[currentHandle].ysize;
    cinTable[currentHandle].CIN_WIDTH = cinTable[currentHandle].xsize;

    cinTable[currentHandle].samplesPerLine = cinTable[currentHandle].CIN_WIDTH * cinTable[currentHandle].samplesPerPixel;
    cinTable[currentHandle].screenDelta = cinTable[currentHandle].CIN_HEIGHT * cinTable[currentHandle].samplesPerLine;

    cinTable[currentHandle].half = 0;
    cinTable[currentHandle].smootheddouble = 0;

    cinTable[currentHandle].VQ0 = cinTable[currentHandle].VQNormal;
    cinTable[currentHandle].VQ1 = cinTable[currentHandle].VQBuffer;

    cinTable[currentHandle].t[0] = (0 - (unsigned int)cin.linbuf) + (unsigned int)cin.linbuf + cinTable[currentHandle].screenDelta;
    cinTable[currentHandle].t[1] = (0 - ((unsigned int)cin.linbuf + cinTable[currentHandle].screenDelta)) + (unsigned int)cin.linbuf;
}

static void RoQPrepMcomp(long int xoff, long int yoff)
{
    long int i;
    long int j;
    long int x;
    long int y;
    long int temp;
    long int temp2;

    i = cinTable[currentHandle].samplesPerLine;
    j = cinTable[currentHandle].samplesPerPixel;
    if (cinTable[currentHandle].xsize == cinTable[currentHandle].ysize * 4 && !cinTable[currentHandle].half) {
        j += j;
        i += i;
    }

    for (y = 0; y < 16; y++) {
        temp2 = (y + yoff - 8) * i;
        for (x = 0; x < 16; x++) {
            temp = (x + xoff - 8) * j;
            cin.mcomp[x * 16 + y] = cinTable[currentHandle].normalBuffer0 - (temp2 + temp);
        }
    }
}

// retail 0x4aec00 (also inlined); Q3 initRoQ (SUPPORTED)
void initRoQ(void)
{
    if (currentHandle < 0)
        return;
    cinTable[currentHandle].VQNormal = blitVQQuad32fs;
    cinTable[currentHandle].VQBuffer = blitVQQuad32fs;
    cinTable[currentHandle].samplesPerPixel = 4;
    ROQ_GenYUVTables();
    RllSetupTable();
}

static void RoQReset(void)
{
    if (currentHandle < 0) return;

    FS_FCloseFile(cinTable[currentHandle].iFile);
    cinTable[currentHandle].iFile = 0;
    FS_FOpenFileRead(cinTable[currentHandle].fileName, &cinTable[currentHandle].iFile, 1);
    FS_Read(cin.file, 16, cinTable[currentHandle].iFile);
    RoQ_init();
    cinTable[currentHandle].status = FMV_LOOPED;
}

static void RoQInterrupt(void)
{
    byte *framedata;
    short sbuf[32768];
    int ssize;

    if (currentHandle < 0) return;

    FS_Read(cin.file, cinTable[currentHandle].RoQFrameSize + 8, cinTable[currentHandle].iFile);
    if (cinTable[currentHandle].RoQPlayed >= cinTable[currentHandle].ROQSize) {
        if (cinTable[currentHandle].holdAtEnd == 0) {
            if (cinTable[currentHandle].looping) {
                RoQReset();
            } else {
                cinTable[currentHandle].status = FMV_EOF;
            }
        } else {
            cinTable[currentHandle].status = FMV_IDLE;
        }
        return;
    }

    framedata = cin.file;
    // new frame is ready
redump:
    switch (cinTable[currentHandle].roq_id) {
    case ROQ_QUAD_VQ:
        re.SyncRenderThread();
        if ((cinTable[currentHandle].numQuads & 1)) {
            cinTable[currentHandle].normalBuffer0 = cinTable[currentHandle].t[1];
            RoQPrepMcomp(cinTable[currentHandle].roqF0, cinTable[currentHandle].roqF1);
            cinTable[currentHandle].VQ1(cin.qStatus[1], framedata);
            cinTable[currentHandle].buf = cin.linbuf + cinTable[currentHandle].screenDelta;
        } else {
            cinTable[currentHandle].normalBuffer0 = cinTable[currentHandle].t[0];
            RoQPrepMcomp(cinTable[currentHandle].roqF0, cinTable[currentHandle].roqF1);
            cinTable[currentHandle].VQ0(cin.qStatus[0], framedata);
            cinTable[currentHandle].buf = cin.linbuf;
        }
        if (cinTable[currentHandle].numQuads == 0) { // first frame
            Com_Memcpy(cin.linbuf + cinTable[currentHandle].screenDelta, cin.linbuf, cinTable[currentHandle].samplesPerLine * cinTable[currentHandle].ysize);
        }
        cinTable[currentHandle].numQuads++;
        cinTable[currentHandle].dirty = 1;
        break;
    case ROQ_CODEBOOK:
        decodeCodeBook(framedata, (unsigned short)cinTable[currentHandle].roq_flags);
        break;
    case ZA_SOUND_MONO:
        if (!cinTable[currentHandle].silent) {
            ssize = RllDecodeMonoToStereo(framedata, sbuf, cinTable[currentHandle].RoQFrameSize, 0, (unsigned short)cinTable[currentHandle].roq_flags);
            SND_RawSamples(ssize, 22050, 2, 1, (byte *)sbuf);
            cinTable[currentHandle].sound = 1;
        }
        break;
    case ZA_SOUND_STEREO:
        if (!cinTable[currentHandle].silent) {
            ssize = RllDecodeStereoToStereo(framedata, sbuf, cinTable[currentHandle].RoQFrameSize, 0, (unsigned short)cinTable[currentHandle].roq_flags);
            SND_RawSamples(ssize, 22050, 2, 2, (byte *)sbuf);
            cinTable[currentHandle].sound = 1;
        }
        break;
    case ROQ_QUAD_INFO:
        if (cinTable[currentHandle].numQuads == -1) {
            readQuadInfo(framedata);
            setupQuad(0, 0);
            cinTable[currentHandle].startTime = cinTable[currentHandle].lastTime = CL_ScaledMilliseconds();
        }
        if (cinTable[currentHandle].numQuads != 1) cinTable[currentHandle].numQuads = 0;
        break;
    case ROQ_PACKET:
        cinTable[currentHandle].inMemory = cinTable[currentHandle].roq_flags;
        cinTable[currentHandle].RoQFrameSize = 0; // for header
        break;
    case ROQ_QUAD_HANG:
        cinTable[currentHandle].RoQFrameSize = 0;
        break;
    case ROQ_QUAD_JPEG:
        break;
    default:
        cinTable[currentHandle].status = FMV_EOF;
        break;
    }
    // read in next frame data
    if (cinTable[currentHandle].RoQPlayed >= cinTable[currentHandle].ROQSize) {
        if (cinTable[currentHandle].holdAtEnd == 0) {
            if (cinTable[currentHandle].looping) {
                RoQReset();
            } else {
                cinTable[currentHandle].status = FMV_EOF;
            }
        } else {
            cinTable[currentHandle].status = FMV_IDLE;
        }
        return;
    }

    framedata += cinTable[currentHandle].RoQFrameSize;
    cinTable[currentHandle].roq_id = framedata[0] + framedata[1] * 256;
    cinTable[currentHandle].RoQFrameSize = framedata[2] + framedata[3] * 256 + framedata[4] * 65536;
    cinTable[currentHandle].roq_flags = framedata[6] + framedata[7] * 256;
    cinTable[currentHandle].roqF0 = (char)framedata[7];
    cinTable[currentHandle].roqF1 = (char)framedata[6];

    if (cinTable[currentHandle].RoQFrameSize > 65536 || cinTable[currentHandle].roq_id == 0x1084) {
        Com_DPrintf("roq_size>65536||roq_id==0x1084 (roq_size=%i,roq_id=%i)\n", cinTable[currentHandle].RoQFrameSize, cinTable[currentHandle].roq_id);
        cinTable[currentHandle].status = FMV_EOF;
        if (cinTable[currentHandle].looping) {
            RoQReset();
        }
        return;
    }
    if (cinTable[currentHandle].inMemory && (cinTable[currentHandle].status != FMV_EOF)) { cinTable[currentHandle].inMemory--; framedata += 8; goto redump; }
    // one more frame hits the dust
    cinTable[currentHandle].RoQPlayed += cinTable[currentHandle].RoQFrameSize + 8;
}

static void RoQ_init(void)
{
    cinTable[currentHandle].lastTime = cinTable[currentHandle].startTime = CL_ScaledMilliseconds();
    cinTable[currentHandle].lastFrameTime = cinTable[currentHandle].startTime;

    cinTable[currentHandle].RoQPlayed = 24;

    // get frame rate
    cinTable[currentHandle].roqFPS = cin.file[6] + cin.file[7] * 256;

    if (!cinTable[currentHandle].roqFPS) cinTable[currentHandle].roqFPS = 30;

    cinTable[currentHandle].numQuads = -1;

    cinTable[currentHandle].roq_id = cin.file[8] + cin.file[9] * 256;
    cinTable[currentHandle].RoQFrameSize = cin.file[10] + cin.file[11] * 256 + cin.file[12] * 65536;
    cinTable[currentHandle].roq_flags = cin.file[14] + cin.file[15] * 256;
}

void ROQ_SetLoopingFromHandle(int handle, qboolean loop)
{
    if (handle < 0 || handle >= 16 || cinTable[handle].status == FMV_EOF) return;
    cinTable[handle].looping = loop;
}

static void RoQShutdown(void)
{
    if (!cinTable[currentHandle].buf) {
        return;
    }

    if (cinTable[currentHandle].status == FMV_IDLE) {
        return;
    }
    Com_DPrintf("finished cinematic\n");
    cinTable[currentHandle].status = FMV_IDLE;

    if (cinTable[currentHandle].sound) {
        SND_EndRawSamples();
    }

    if (cinTable[currentHandle].iFile) {
        FS_FCloseFile(cinTable[currentHandle].iFile);
        cinTable[currentHandle].iFile = 0;
    }

    if (cinTable[currentHandle].alterGameState) {
        CLC_CONN->state = cinTable[currentHandle].previousGameState;
        CL_handle = -1;
        if (CLC_CONN->state == 0 && nextmap->current.string[0]) {
            Cbuf_AddText(va("%s\n", nextmap->current.string));
            Dvar_SetString(nextmap, "");
            UI_SetActiveMenu(1);
        } else {
            UI_SetActiveMenu(0);
        }
    }

    if (re.SyncRenderThread) {
        re.SyncRenderThread();
    }

    memset(&cinTable[currentHandle], 0, sizeof(cinTable[currentHandle]));
    currentHandle = -1;
}

e_status ROQ_StopCinematicFromHandle(int handle)
{
    if (handle < 0 || handle >= 16 || cinTable[handle].status == FMV_EOF) {
        return FMV_EOF;
    }
    currentHandle = handle;
    Com_DPrintf("trFMV::stop(), closing %s\n", cinTable[currentHandle].fileName);
    if (!cinTable[currentHandle].buf) {
        return FMV_EOF;
    }
    if (cinTable[currentHandle].alterGameState) {
        if (CLC_CONN->state != 1) {
            return cinTable[currentHandle].status;
        }
    }
    cinTable[currentHandle].status = FMV_EOF;
    RoQShutdown();
    return FMV_EOF;
}

void ROQ_SetExtentsFromHandle(int handle, int x, int y, int w, int h)
{
    if (handle >= 0 && handle < 16 && cinTable[handle].status != FMV_EOF) {
        cinTable[handle].xpos = x;
        cinTable[handle].ypos = y;
        cinTable[handle].width = w;
        cinTable[handle].height = h;
        cinTable[handle].dirty = 1;
    }
}

e_status ROQ_RunCinematicFromHandle(int handle)
{
    int start;
    unsigned int thisTime;
    int frame;

    if (handle < 0 || handle >= 16 || cinTable[handle].status == FMV_EOF) return FMV_EOF;

    if (cin.currentHandle != handle) {
        currentHandle = handle;
        cin.currentHandle = currentHandle;
        cinTable[currentHandle].status = FMV_EOF;
        RoQReset();
    }

    if (cinTable[handle].playonwalls < -1) {
        return cinTable[handle].status;
    }

    currentHandle = handle;

    if (cinTable[currentHandle].alterGameState) {
        if (CLC_CONN->state != CA_CINEMATIC) {
            return cinTable[currentHandle].status;
        }
    }

    if (cinTable[currentHandle].status == FMV_IDLE) {
        return cinTable[currentHandle].status;
    }

    thisTime = CL_ScaledMilliseconds();
    if (cinTable[currentHandle].material && fabs((float)(thisTime - cinTable[currentHandle].lastTime)) > 100.0f) {
        cinTable[currentHandle].startTime += thisTime - cinTable[currentHandle].lastTime;
    }

    if (cinTable[currentHandle].sound) {
        frame = SND_RawSamplesTime() * cinTable[currentHandle].roqFPS / 1000 + 1;
    } else {
        frame = (thisTime - cinTable[currentHandle].startTime) * cinTable[currentHandle].roqFPS / 1000;
    }

    if (cinTable[currentHandle].tfps < frame) {
        cinTable[currentHandle].tfps = frame;
        cinTable[currentHandle].lastFrameTime = thisTime;
    } else if ((thisTime - cinTable[currentHandle].lastFrameTime) * cinTable[currentHandle].roqFPS > 4000) {
        cinTable[currentHandle].status = FMV_EOF;
    }

    start = cinTable[currentHandle].startTime;
    while ((cinTable[currentHandle].tfps != cinTable[currentHandle].numQuads)
        && (cinTable[currentHandle].status == FMV_PLAY)) {
        RoQInterrupt();
        if (start != cinTable[currentHandle].startTime) {
            cinTable[currentHandle].tfps = (CL_ScaledMilliseconds() - cinTable[currentHandle].startTime) * cinTable[currentHandle].roqFPS / 1000;
            start = cinTable[currentHandle].startTime;
        }
    }

    cinTable[currentHandle].lastTime = thisTime;

    if (cinTable[currentHandle].status == FMV_LOOPED) {
        cinTable[currentHandle].status = FMV_PLAY;
    }

    if (cinTable[currentHandle].status == FMV_EOF) {
        if (cinTable[currentHandle].looping) {
            RoQReset();
        } else {
            RoQShutdown();
        }
    }

    return cinTable[currentHandle].status;
}

int ROQ_PlayCinematic(const char *arg, int x, int y, int w, int h, int systemBits)
{
    unsigned short RoQID;
    char name[256];
    int i;

    Com_sprintf(name, sizeof(name), "video/%s.roq", arg);

    if (!(systemBits & 1)) {
        for (i = 0; i < 16; i++) {
            if (!I_stricmp(cinTable[i].fileName, name)) {
                return i;
            }
        }
    }

    Com_DPrintf("SCR_PlayCinematic( %s )\n", arg);

    Com_Memset(&cin, 0, sizeof(cinematics_t));
    currentHandle = CIN_findHandle();

    cin.currentHandle = currentHandle;

    strcpy(cinTable[currentHandle].fileName, name);

    cinTable[currentHandle].ROQSize = 0;
    cinTable[currentHandle].ROQSize = FS_FOpenFileRead(cinTable[currentHandle].fileName, &cinTable[currentHandle].iFile, 1);

    if (cinTable[currentHandle].ROQSize <= 0) {
        Com_DPrintf("play(%s), ROQSize<=0\n", arg);
        cinTable[currentHandle].fileName[0] = 0;
        return -1;
    }

    ROQ_SetExtentsFromHandle(currentHandle, x, y, w, h);
    ROQ_SetLoopingFromHandle(currentHandle, (systemBits & 2) != 0);

    cinTable[currentHandle].holdAtEnd = (systemBits & 4) != 0;
    cinTable[currentHandle].alterGameState = systemBits & 1;
    cinTable[currentHandle].playonwalls = 1;
    cinTable[currentHandle].silent = (systemBits & 8) != 0;
    cinTable[currentHandle].material = (systemBits & 0x10) != 0;
    cinTable[currentHandle].letterBox = (systemBits & 0x20) != 0;
    cinTable[currentHandle].sound = 0;

    if (cinTable[currentHandle].alterGameState) {
        // close the menu
        if (cls.uiStarted) {
            UI_SetActiveMenu(0);
        }
    } else {
        cinTable[currentHandle].playonwalls = cl_inGameVideo->current.enabled != 0;
    }

    initRoQ();

    FS_Read(cin.file, 16, cinTable[currentHandle].iFile);

    RoQID = (unsigned short)(cin.file[0]) + (unsigned short)(cin.file[1]) * 256;
    if (RoQID == 0x1084) {
        RoQ_init();

        cinTable[currentHandle].status = FMV_PLAY;
        Com_DPrintf("trFMV::play(), playing %s\n", arg);

        if (cinTable[currentHandle].alterGameState) {
            cinTable[currentHandle].previousGameState = CLC_CONN->state;
            CLC_CONN->state = CA_CINEMATIC;
        }

        Con_Close();

        return currentHandle;
    }
    Com_DPrintf("trFMV::play(), invalid RoQ ID\n");

    RoQShutdown();
    return -1;
}

// Original name unknown.
static void ROQ_GetDrawRect(float aspect, int *x, int *y, int *w, int *h)
{
    float barSize;
    int barFloor;
    int barCeil;

    if (aspect == cls.vidConfig.aspectRatioWindow) {
        *x = 0;
        *y = 0;
        *w = cls.vidConfig.width;
        *h = cls.vidConfig.height;
    } else if (aspect > cls.vidConfig.aspectRatioWindow) {
        barSize = ((float)cls.vidConfig.height - (cls.vidConfig.aspectRatioWindow / aspect) * (float)cls.vidConfig.height) * 0.5f;
        barFloor = FastFloor(barSize);
        barCeil = FastCeil(barSize);
        *x = 0;
        *y = barFloor;
        *w = cls.vidConfig.width;
        *h = cls.vidConfig.height - 2 * barCeil;
    } else {
        barSize = ((float)cls.vidConfig.width - (aspect / cls.vidConfig.aspectRatioWindow) * (float)cls.vidConfig.width) * 0.5f;
        barFloor = FastFloor(barSize);
        barCeil = FastCeil(barSize);
        *x = barFloor;
        *y = 0;
        *w = cls.vidConfig.width - 2 * barCeil;
        *h = cls.vidConfig.height;
    }
}

// Original name unknown.
// retail 0x4afbc0, unreferenced; name unknown
void ROQ_DrawRect_4afbc0(const float *pos, const float *size, const float *color)
{
    refexport_t *re;

    re = (refexport_t *)(void *)((void *)&::re);
    ((void (*)(float, float, float, float, float, float, float, float, const vec_t *, int))re->DrawStretchPic)(
        pos[0], pos[1], size[0], size[1],
        0.0f, 0.0f, 0.0f, 0.0f, color, (int)cls.whiteMaterial);
}

static void ROQ_DrawLetterBox(int x, int y, int w, int h, const float *color)
{
    float barSize;
    int barTop;
    int barBottom;
    float pos[2];
    float size[2];

    barSize = ((float)cls.vidConfig.height * (1.0f / 480.0f)) * 105.0f;
    barTop = FastFloor(barSize);
    barBottom = FastCeil(barSize);

    pos[0] = (float)x;
    pos[1] = (float)y;
    size[0] = (float)w;
    size[1] = (float)barTop;
    ROQ_DrawRect_4afbc0(pos, size, color);

    pos[0] = (float)x;
    pos[1] = (float)(y + h - barBottom);
    size[0] = (float)w;
    size[1] = (float)barBottom;
    ROQ_DrawRect_4afbc0(pos, size, color);
}

void ROQ_DrawCinematicFromHandle(int handle)
{
    byte *buf;
    float clearColor[4];
    int x, y, w, h;
    refexport_t *re;

    if (handle < 0 || handle >= 16 || cinTable[handle].status == FMV_EOF)
        return;
    buf = cinTable[handle].buf;
    if (!buf)
        return;

    CL_LookupColor(0x30, clearColor);
    ROQ_GetDrawRect((float)cinTable[handle].width / cinTable[handle].height, &x, &y, &w, &h);

    re = (refexport_t *)(void *)((void *)&::re);
    ((void (*)(int, const vec_t *, float, int))re->ClearScreen)(1, clearColor, 0.0f, 0);

    ((void (*)(int, int, int, int, int, int, byte *, int, int))re->DrawStretchRaw)(
        x, y, w, h, cinTable[handle].CIN_WIDTH, cinTable[handle].CIN_HEIGHT,
        buf, handle, cinTable[handle].dirty);
    cinTable[handle].dirty = 0;

    if (cinTable[handle].letterBox)
        ROQ_DrawLetterBox(x, y, w, h, clearColor);
}

void ROQ_DrawCinematic(void)
{
    if (CL_handle >= 0 && CL_handle < 16) {
        ROQ_DrawCinematicFromHandle(CL_handle);
    }
}

void ROQ_PlayCinematic_f(void)
{
    const char *arg;
    const char *s;
    int bits;


    bits = 1;
    Com_DPrintf("CL_PlayCinematic_f\n");

    if (CLC_CONN->state == CA_CINEMATIC) {
        if (CL_handle >= 0 && CL_handle < 16) {
            ROQ_StopCinematicFromHandle(CL_handle);
            SND_StopSounds((snd_stopsounds_arg_t)0);
            CL_handle = -1;
        }
    } else if (CLC_CONN->state == 2) {
        CL_StopLogo();
    }

    arg = Cmd_Argv(1);
    s = Cmd_Argv(2);
    if (s) {
        if (s[0] == '1')
            bits = 5;
        if (s[0] == '2')
            bits |= 2;
        if (s[0] == '3')
            bits |= 0x20;
    }

    if (bits & 0x20) {
        CL_handle = ROQ_PlayCinematic(arg, 0, 105, 640, 270, bits);
    } else {
        CL_handle = ROQ_PlayCinematic(arg, 0, 0, 640, 480, bits);
    }
    if (CL_handle >= 0) {
        SND_StopSounds((snd_stopsounds_arg_t)0);
        SND_FadeAllSounds(1.0f, 0);
        do {
            ROQ_RunCinematic();
        } while (cinTable[currentHandle].buf == NULL && cinTable[currentHandle].status == FMV_PLAY);
    }
}

void ROQ_UploadCinematicFromHandle(int handle)
{
    if (handle >= 0 && handle < 16 && cinTable[handle].buf) {
        if (cinTable[handle].playonwalls <= 0 && cinTable[handle].dirty) {
            if (cinTable[handle].playonwalls == 0) {
                cinTable[handle].playonwalls = -1;
            } else if (cinTable[handle].playonwalls == -1) {
                cinTable[handle].playonwalls = -2;
            } else {
                cinTable[handle].dirty = 0;
            }
        }
        if (!cl_inGameVideo->current.enabled) {
            if (cinTable[handle].playonwalls == 1) {
                cinTable[handle].playonwalls = 0;
            }
        }
    }
}

