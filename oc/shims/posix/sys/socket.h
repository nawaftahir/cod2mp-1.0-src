#ifndef WIN32_SHIM_SYS_SOCKET_H
#define WIN32_SHIM_SYS_SOCKET_H
#ifdef _WIN32
#if defined(_MSC_VER)
#define COD2_WINAPI __stdcall      /* MSVC no-ops __attribute__; need real stdcall to match ws2_32 (_bind@12) */
#else
#define COD2_WINAPI __attribute__((__stdcall__))
#endif
#ifndef _COD2_SOCKET_T
#define _COD2_SOCKET_T
typedef unsigned int SOCKET;
#endif

struct in_addr  { unsigned long s_addr; };
struct sockaddr { unsigned short sa_family; char sa_data[14]; };
struct sockaddr_in { short sin_family; unsigned short sin_port; struct in_addr sin_addr; char sin_zero[8]; };
struct hostent  { char *h_name; char **h_aliases; short h_addrtype; short h_length; char **h_addr_list; };
typedef unsigned int cod2_socklen_t; typedef cod2_socklen_t socklen_t;
typedef struct cod2_fd_set { unsigned int fd_count; SOCKET fd_array[64]; } fd_set;
#ifndef _TIMEVAL_DEFINED
#define _TIMEVAL_DEFINED
struct timeval { long tv_sec; long tv_usec; };
#endif

#define AF_INET 2
#define AF_UNSPEC 0
#define SOCK_STREAM 1
#define SOCK_DGRAM 2
#define IPPROTO_UDP 17
#define IPPROTO_TCP 6
#define SOL_SOCKET 0xffff
#define SO_BROADCAST 0x20
#define SO_REUSEADDR 4
#define SO_RCVBUF 0x1002
#define SO_SNDBUF 0x1001
#define FIONBIO 0x8004667eUL
#define INADDR_ANY 0UL
#define INADDR_NONE 0xffffffffUL
#define INADDR_BROADCAST 0xffffffffUL
#define INVALID_SOCKET ((SOCKET)(~0))
#define SOCKET_ERROR (-1)
#define MSG_PEEK 2
#define FD_SETSIZE 64
#define FD_ZERO(set) ((set)->fd_count=0)
#define FD_SET(fd,set) do{ if((set)->fd_count<FD_SETSIZE)(set)->fd_array[(set)->fd_count++]=(fd);}while(0)
#define FD_ISSET(fd,set) cod2_fd_isset((fd),(set))
static __inline int cod2_fd_isset(SOCKET fd, fd_set*s){unsigned i;for(i=0;i<s->fd_count;i++)if(s->fd_array[i]==fd)return 1;return 0;}

SOCKET COD2_WINAPI socket(int af, int type, int protocol);
int    COD2_WINAPI bind(SOCKET s, const struct sockaddr *a, int len);
int    COD2_WINAPI connect(SOCKET s, const struct sockaddr *a, int len);
int    COD2_WINAPI recv(SOCKET s, char *buf, int len, int flags);
int    COD2_WINAPI send(SOCKET s, const char *buf, int len, int flags);
int    COD2_WINAPI recvfrom(SOCKET s, char *buf, int len, int flags, struct sockaddr *from, int *fromlen);
int    COD2_WINAPI sendto(SOCKET s, const char *buf, int len, int flags, const struct sockaddr *to, int tolen);
int    COD2_WINAPI setsockopt(SOCKET s, int level, int opt, const char *val, int len);
int    COD2_WINAPI getsockname(SOCKET s, struct sockaddr *name, int *namelen);
int    COD2_WINAPI select(int nfds, fd_set *r, fd_set *w, fd_set *e, const struct timeval *t);
int    COD2_WINAPI closesocket(SOCKET s);
int    COD2_WINAPI ioctlsocket(SOCKET s, long cmd, unsigned long *argp);
unsigned short COD2_WINAPI htons(unsigned short v);
unsigned short COD2_WINAPI ntohs(unsigned short v);
unsigned long  COD2_WINAPI htonl(unsigned long v);
unsigned long  COD2_WINAPI ntohl(unsigned long v);
unsigned long  COD2_WINAPI inet_addr(const char *cp);
char *         COD2_WINAPI inet_ntoa(struct in_addr in);
struct hostent* COD2_WINAPI gethostbyname(const char *name);
int    COD2_WINAPI gethostname(char *name, int len);
int    COD2_WINAPI WSAStartup(unsigned short ver, void *data);
int    COD2_WINAPI WSACleanup(void);
int    COD2_WINAPI WSAGetLastError(void);
#endif
#endif
