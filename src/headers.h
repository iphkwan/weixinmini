#ifndef _HEADERS_H_
#define _HEADERS_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <limits.h>
#include <pwd.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <netinet/in.h>
#include <pthread.h>

#ifndef LINE_SIZE
#define LINE_SIZE 1024
#endif

#ifndef BUF_SIZE
#define BUF_SIZE 4096
#endif

#endif /* _HEADERS_H_ */
