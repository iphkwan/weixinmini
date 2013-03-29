#ifndef _WEIXIND_H_
#define _WEIXIND_H_

#include "headers.h"
#include "mysql.h"
#include "memcached.h"
#include "task.h"

#define WEIXIND_LOG_FILENAME    "weixind.log"
#define WEIXIND_LISTEN_PORT     12345
#define WEIXIND_LISTEN_BACKLOG  1024
#define WEIXIND_EVENT_PER_LOOP  16
#define WEIXIND_EVENT_TIMEOUT   1000
#define WEIXIND_DB_HOST         "localhost"
#define WEIXIND_DB_PORT         3306
#define WEIXIND_DB_USER         "weixin"
#define WEIXIND_DB_PASSWD       "weixin"
#define WEIXIND_DB_DB           "weixin"
#define WEIXIND_MEM_HOST        "localhost"
#define WEIXIND_MEM_PORT        MEMCACHED_DEFAULT_PORT

typedef struct weixind_s {
  int epoll_fd;
  int listen_fd;
  int log_fd;

  pthread_t tid;
  task_queue_t *task_queue;
  MYSQL *db;
  memcached_st *mem;
} weixind_t;

weixind_t *weixind_init(void);
void weixind_done(weixind_t *weixind);

#endif /* _WEIXIND_H_ */
