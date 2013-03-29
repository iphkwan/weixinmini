#include "headers.h"
#include "weixind.h"
#include "task.h"

void weixind_log(int log_fd, const char *format, ...)
{
  char buffer[BUF_SIZE];
  int n;
  time_t t;
  va_list ap;

  if (log_fd < 0) {
    log_fd = STDERR_FILENO;
  }

  time(&t);
  n = snprintf(buffer, sizeof(buffer) - 1, "%s", ctime(&t));
  buffer[n-1] = '\t';

  va_start(ap, format);
  n += vsnprintf(buffer + n, sizeof(buffer) - n - 1, format, ap);
  va_end(ap);

  strncat(buffer + n, "\n", sizeof(buffer) - n);

  write(log_fd, buffer, strlen(buffer));
}

void weixind_accept_handler(int accept_fd, weixind_t *weixind)
{
  struct sockaddr cliaddr;
  socklen_t clilen;
  int clifd;
  struct epoll_event ee;
  while (1) {
    clilen = sizeof(cliaddr);
    clifd = accept(accept_fd, &cliaddr, &clilen);
    if (clifd < 0) {
      return;
    }

#ifndef NDEBUG
    weixind_log(weixind->log_fd, "accept %d", clifd);
#endif

    if (fcntl(clifd, F_SETFL, fcntl(clifd, F_GETFL, 0) | O_NONBLOCK) < 0) {
      weixind_log(weixind->log_fd, "fcntl[O_NONBLOCK, clifd:%d] failed: %s",
                  clifd, strerror(errno));
      close(clifd);
      continue;
    }

    ee.events = EPOLLIN;
    ee.data.fd = clifd;
    if (epoll_ctl(weixind->epoll_fd, EPOLL_CTL_ADD, clifd, &ee) < 0) {
      weixind_log(weixind->log_fd, "epoll_ctl[EPOLL_CTL_ADD, clifd:%d] failed: %s",
                  clifd, strerror(errno));
      close(clifd);
    }
  }
}

void weixind_client_handler(int client_fd, weixind_t *weixind)
{
  task_t *task;
  int n;

  task = calloc(1, sizeof(*task));
  if (task == NULL) {
    weixind_log(weixind->log_fd, "calloc task failed: %s", strerror(errno));
    return;
  }
  task->fd = client_fd;
  n = read(task->fd, task->buffer, sizeof(task->buffer) - 1);
  if (n <= 0) {
    struct epoll_event ee;
#ifndef NDEBUG
    weixind_log(weixind->log_fd, "close %d", task->fd);
#endif
    epoll_ctl(weixind->epoll_fd, EPOLL_CTL_DEL, task->fd, &ee);
    close(task->fd);
    free(task);
    return;
  }
  task->buffer[n] = '\0';
  task_queue_push(weixind->task_queue, task);
}

weixind_t *weixind_init(void)
{
  weixind_t *weixind;
  struct epoll_event ee;

  weixind = calloc(1, sizeof(*weixind));
  if (weixind == NULL) {
    return NULL;
  }

#ifdef NDEBUG
  weixind->log_fd = open(WEIXIND_LOG_FILENAME, O_CREAT | O_APPEND, 0644);
#else
  weixind->log_fd = -1;
#endif

  weixind->listen_fd = tcp_listen(WEIXIND_LISTEN_PORT, WEIXIND_LISTEN_BACKLOG);
  if (weixind->listen_fd < 0) {
    weixind_log(weixind->log_fd, "tcp_listen failed");
    free(weixind);
    return NULL;
  }

  weixind->epoll_fd = epoll_create(255);
  if (weixind->epoll_fd < 0) {
    weixind_log(weixind->log_fd, "epoll_create failed: %s", strerror(errno));
    free(weixind);
    return NULL;
  }

  ee.events = EPOLLIN;
  ee.data.fd = weixind->listen_fd;
  if (epoll_ctl(weixind->epoll_fd, EPOLL_CTL_ADD, weixind->listen_fd, &ee) < 0) {
    weixind_log(weixind->log_fd, "epoll_ctl failed[EPOLL_CTL_ADD, listen_fd:%d] failed: %s",
                weixind->listen_fd, strerror(errno));
    free(weixind);
    return NULL;
  }

  weixind->task_queue = task_queue_init();
  if (weixind->task_queue == NULL) {
    weixind_log(weixind->log_fd, "task_queue_init failed");
    free(weixind);
    return NULL;
  }

  weixind->db = db_init(WEIXIND_DB_HOST, WEIXIND_DB_PORT,
                        WEIXIND_DB_USER, WEIXIND_DB_PASSWD,
                        WEIXIND_DB_DB);
  if (weixind->db == NULL) {
    weixind_log(weixind->log_fd, "db_init failed");
    weixind_done(weixind);
    return NULL;
  }
  
  weixind->mem = mem_init(WEIXIND_MEM_HOST, WEIXIND_MEM_PORT);
  if (weixind->mem == NULL) {
    weixind_log(weixind->log_fd, "mem_init failed");
    weixind_done(weixind);
    return NULL;
  }

  return weixind;
}

void weixind_done(weixind_t *weixind)
{
  if (weixind) {
    struct epoll_event ee;
    epoll_ctl(weixind->epoll_fd, EPOLL_CTL_DEL, weixind->listen_fd, &ee);
    close(weixind->listen_fd);
    close(weixind->epoll_fd);
    close(weixind->log_fd);
    db_done(weixind->db);
    mem_done(weixind->mem);
    free(weixind);
  }
}

int weixind_server(void)
{
  weixind_t *weixind;
  struct epoll_event events[WEIXIND_EVENT_PER_LOOP];
  int i, n;

  weixind = weixind_init();
  if (weixind == NULL) {
    weixind_log(-1, "weixind_init failed");
    return -1;
  }

  if (pthread_create(&weixind->tid, NULL, task_queue_handler, weixind) != 0) {
    weixind_log(-1, "pthread_create[task_queue_handler] failed");
    return -1;
  }
  
  while (1) {
    n = epoll_wait(weixind->epoll_fd, events,
                   WEIXIND_EVENT_PER_LOOP, WEIXIND_EVENT_TIMEOUT);
    for (i = 0; i < n; ++i) {
      if (events[i].events == EPOLLIN) {
        if (events[i].data.fd == weixind->listen_fd) {
          weixind_accept_handler(events[i].data.fd, weixind);
        } else {
          weixind_client_handler(events[i].data.fd, weixind);
        }
      }
    }
  }

  weixind_done(weixind);
  return 0;
}

void show_help(void)
{
  printf("weixind - server of weixin\n"
         "Usage: weixind [options...]\n"
         "Options:\n"
         "-h        show this help\n");
}

int main(int argc, char *argv[])
{
  int opt;
  while ((opt = getopt(argc, argv, "h")) != -1) {
    switch (opt) {
    case 'h':
      show_help();
      return 0;
    }
  }
  return weixind_server();
}
