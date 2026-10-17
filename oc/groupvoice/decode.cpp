
#include "common_types.h"
#include "imports.h"
void *g_decoder = NULL;

int g_decode_frame_size;


static int g_current_decode_bandwidth_setting = 0x0;

#ifdef __cplusplus
extern "C" {
#endif
extern void *speex_decoder_init(const SpeexMode *mode);
extern int speex_decoder_ctl(void *state, int request, void *ptr);
extern int speex_encoder_ctl(void *state, int request, void *ptr);
extern void speex_decoder_destroy(void *state);
extern void speex_bits_init(SpeexBits *bits);
extern void speex_bits_destroy(SpeexBits *bits);
extern void speex_bits_read_from(SpeexBits *bits, char *buffer, int len);
extern int speex_decode(void *state, SpeexBits *bits, float *out);
#ifdef __cplusplus
}
#endif
extern void Com_Printf(const char *fmt, ...);

#ifdef __cplusplus
extern "C" {
#endif
extern const SpeexMode speex_nb_mode;
extern const SpeexMode speex_wb_mode;
extern const SpeexMode speex_uwb_mode;
#ifdef __cplusplus
}
#endif
extern int g_encoder_samplerate;

extern void *g_decoder;
extern int g_current_decode_bandwidth_setting;
extern int g_decode_frame_size;
static SpeexBits decodeBits;

Bool Decode_Init(int bandwidthEnum)
{
    const void *mode;
    int tmp = 1;

    switch (bandwidthEnum) {
    case 0:
        mode = &speex_nb_mode;
        break;
    case 1:
        mode = &speex_wb_mode;
        break;
    case 2:
        mode = &speex_uwb_mode;
        break;
    default:
        Com_Printf("Unknown bandwidth mode %i\n", bandwidthEnum);
        return 0;
    }

    g_decoder = speex_decoder_init( (const SpeexMode *)(mode));
    speex_decoder_ctl(g_decoder, 0, &tmp);
    speex_decoder_ctl(g_decoder, 0x18, &g_encoder_samplerate);
    speex_encoder_ctl(g_decoder, 3, &g_decode_frame_size);
    g_current_decode_bandwidth_setting = bandwidthEnum;
    speex_bits_init(&decodeBits);
    return 1;
}

void Decode_Shutdown(void)
{
    if (g_decoder) {
        speex_bits_destroy(&decodeBits);
        speex_decoder_destroy(g_decoder);
    }
    g_decoder = 0;
}

int Decode_Sample(char *buffer, int maxLength, short *out, int frame_size)
{
    float output[4096];
    int i, count;

    speex_bits_read_from(&decodeBits, buffer, maxLength);

    if (speex_decode(g_decoder, &decodeBits, output) != 0)
        count = 0;
    else
        count = frame_size * 2;

    for (i = 0; i < count; i++) {
        out[i] = (short)(int)output[i];
    }

    return count;
}


// retail 0x4b6510, unreferenced; name unknown
Bool Decode_SetBandwidth_4b6510(int bandwidthEnum)
{
    if (bandwidthEnum == g_current_decode_bandwidth_setting)
        return 1;
    Decode_Shutdown();
    return Decode_Init(bandwidthEnum);
}

// retail 0x4b65d0, unreferenced; name unknown
int Decode_GetFrameSize_4b65d0(void *state)
{
    int frame_size;

    speex_decoder_ctl(state, 3, &frame_size);
    return frame_size;
}
