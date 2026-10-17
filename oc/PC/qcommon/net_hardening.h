#ifndef COD2_NET_HARDENING_H
#define COD2_NET_HARDENING_H

#include "cod2_feature_config.h"

#if COD2_FEATURE_NET_HARDENING

#    ifndef COD2_NET_HARDENING_NEED_NETADR
struct netadr_t;
#    endif

int NetHardening_ValidDownloadName(const char *name, int require_iwd_ext);

int NetHardening_ValidUrl(const char *url);

int NetHardening_ServerMaySetCvar(const char *key);

int NetHardening_SVC_RateLimit(int nowMsec);
int NetHardening_SVC_RateLimitAddress(struct netadr_t from, int nowMsec);

#endif

#endif
