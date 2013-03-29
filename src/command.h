#ifndef _COMMAND_H_
#define _COMMAND_H_

#include "headers.h"
#include "weixind.h"
#include "task.h"

typedef struct command_s {
  char *name;
  void (*handler)(weixind_t *weixind, task_t *task);
} command_t;

extern command_t Commands[];

#endif /* _COMMAND_H_ */
