#ifndef _TASK_QUEUE_H_
#define _TASK_QUEUE_H_

#include "headers.h"

typedef struct task_s {
  int fd;
  char buffer[BUF_SIZE];
  struct task_s *next;
} task_t;

typedef struct task_queue_s {
  task_t *head;
  task_t *tail;
  pthread_mutex_t *mutex;
} task_queue_t;

task_queue_t *task_queue_init(void);
void task_queue_done(task_queue_t *task_queue);
int task_queue_push(task_queue_t *task_queue, task_t *task);
task_t *task_queue_pop(task_queue_t *task_queue);
void *task_queue_handler(void *arg);

#endif /* _TASK_QUEUE_H_ */
