#ifndef WWW_DOWNLOAD_H
#define WWW_DOWNLOAD_H

#include "cod2_feature_config.h"

typedef enum {
    DL_STATUS_IDLE,
    DL_STATUS_IN_PROGRESS,
    DL_STATUS_DONE,
    DL_STATUS_FAILED,
    DL_STATUS_CANCELLED
} dlStatus_t;

#if COD2_FEATURE_WWW_DOWNLOAD

void       DL_InitDownload(void);

int        DL_BeginDownload(const char *localName, const char *remoteUrl, int checksum);
void       DL_DownloadLoop(void);
void       DL_CancelDownload(void);
int        DL_InProgress(void);
int        DL_DLIsMotd(void);
dlStatus_t DL_GetStatus(void);

#else

static inline void       DL_InitDownload(void) {}
static inline int        DL_BeginDownload(const char *l, const char *u, int c) { (void)l; (void)u; (void)c; return 0; }
static inline void       DL_DownloadLoop(void) {}
static inline void       DL_CancelDownload(void) {}
static inline int        DL_InProgress(void) { return 0; }
static inline int        DL_DLIsMotd(void) { return 0; }
static inline dlStatus_t DL_GetStatus(void) { return DL_STATUS_IDLE; }

#endif

#endif
