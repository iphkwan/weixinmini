#include "task.h"
#include "weixind.h"
#include "command.h"

task_queue_t *task_queue_init(void)
{
  task_queue_t *task_queue = calloc(1, sizeof(*task_queue));
  if (task_queue == NULL) {
    return NULL;
  }
  task_queue->head = task_queue->tail = NULL;
  task_queue->mutex = malloc(sizeof(*task_queue->mutex));
  if (task_queue->mutex == NULL) {
    free(task_queue);
    return NULL;
  }
  if (pthread_mutex_init(task_queue->mutex, NULL) != 0) {
    free(task_queue->mutex);
    free(task_queue);
    return NULL;
  }
  return task_queue;
}

void task_queue_done(task_queue_t *task_queue)
{
  pthread_mutex_destroy(task_queue->mutex);
  free(task_queue->mutex);
  free(task_queue);
}

int task_queue_push(task_queue_t *task_queue, task_t *task)
{
  pthread_mutex_lock(task_queue->mutex);
  if (task_queue->head == NULL) {
    task_queue->head = task;
  } else {
    task_queue->tail->next = task;
  }
  task_queue->tail = task;
  task->next = NULL;
  pthread_mutex_unlock(task_queue->mutex);
}

task_t *task_queue_pop(task_queue_t *task_queue)
{
  task_t *task;
  pthread_mutex_lock(task_queue->mutex);
  task = task_queue->head;
  if (task_queue->head) {
    task_queue->head = task_queue->head->next;
  }
  if (task_queue->tail == task) {
    task_queue->tail = NULL;
  }
  pthread_mutex_unlock(task_queue->mutex);
  return task;
}

void *task_queue_handler(void *arg)
{
  weixind_t *weixind = arg;
  task_t *task;
  char *name;
  int i;
  
  while (1) {
    task = task_queue_pop(weixind->task_queue);
    if (task) {
      /* assume task->buffer is null-terminated  */
#ifndef NDEBUG
      weixind_log(weixind->log_fd, "receive %s", task->buffer);
#endif
      task->args = task->buffer;
      task->uid = command_get_token(&task->args);
      name = command_get_token(&task->args);
      if (task->uid == NULL || name == NULL) {
        write(task->fd, STRING("response error invalid command"));
        continue;
      }
      for (i = 0; Commands[i].name; ++i) {
        if (i >= COMMAND_MUST_LOGIN &&
            mem_is_online(weixind->mem, task->uid) <= 0) {
          write(task->fd, STRING("response error you must login first"));
          free(task);
          break;
        }
        if (strcmp(Commands[i].name, name) == 0) {
          Commands[i].handler(weixind, task);
          free(task);
          break;
        }
      }
      if (Commands[i].name == NULL) {
        write(task->fd, STRING("response error command not found"));
      }
    } else {
      usleep(100000);           /* 100ms */
    }
  }
}
