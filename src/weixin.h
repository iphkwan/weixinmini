#ifndef _WEIXIN_H_
#define _WEIXIN_H_

#include "headers.h"

typedef struct weixin_s {
  char uid[32];
  int fd;
  pthread_t tid;
  pthread_mutex_t *mutex;
} weixin_t;

weixin_t *weixin_init(void);
void weixin_done(weixin_t *weixin);

#endif /* _WEIXIN_H_ */
