#ifndef _WEIXIND_H_
#define _WEIXIND_H_

#define WEIXIND_LISTEN_PORT     12345
#define WEIXIND_LISTEN_BACKLOG  1024
#define WEIXIND_EVENT_PER_LOOP  16
#define WEIXIND_EVENT_TIMEOUT   1000

typedef struct weixind_s {
  int epoll_fd;
  int listen_fd;
  int log_fd;
} weixind_t;

#endif /* _WEIXIND_H_ */
