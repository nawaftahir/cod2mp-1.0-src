#ifndef PB_PUBLIC_H
#define PB_PUBLIC_H

#include "cod2_feature_config.h"

#if COD2_FEATURE_PUNKBUSTER

void PbClientInitialize(void *param);
void PbServerInitialize(void);
void PbClientProcessEvents(void);
void PbServerProcessEvents(void);
void EnablePbCl(void);
void DisablePbCl(void);
void EnablePbSv(void);
void DisablePbSv(void);

const char *PB_BuildAuthToken(void);

int  PB_HandleClientOobPacket(const void *from, void *msg);
int  PB_HandleServerOobPacket(const void *from, void *msg);
void PB_DropClient(int clientNum, const char *reason);

#else

static inline void PbClientInitialize(void *param) { (void)param; }
static inline void PbServerInitialize(void) {}
static inline void PbClientProcessEvents(void) {}
static inline void PbServerProcessEvents(void) {}
static inline void EnablePbCl(void) {}
static inline void DisablePbCl(void) {}
static inline void EnablePbSv(void) {}
static inline void DisablePbSv(void) {}
static inline const char *PB_BuildAuthToken(void) { return ""; }
static inline int  PB_HandleClientOobPacket(const void *from, void *msg) { (void)from; (void)msg; return 0; }
static inline int  PB_HandleServerOobPacket(const void *from, void *msg) { (void)from; (void)msg; return 0; }
static inline void PB_DropClient(int clientNum, const char *reason) { (void)clientNum; (void)reason; }

#endif

static inline const char *Com_GetPunkBusterAuthToken(void)
{
#if COD2_FEATURE_PUNKBUSTER
    return PB_BuildAuthToken();
#elif COD2_IS_PATCH_13
    return "";
#else
    return (const char *)0;
#endif
}

#endif
