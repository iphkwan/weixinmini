#include "weixin.h"
#include "weixind.h"
#include "socket.h"

void show_help(void)
{
  printf("weixind - server of weixin\n"
         "Usage: weixind [options...]\n"
         "Options:\n"
         "-h        show this help\n");
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
    ee.events = EPOLLIN;
    ee.data.fd = clifd;
    epoll_ctl(weixind->epoll_fd, EPOLL_CTL_ADD, clifd, &ee);
  }
}

void weixind_client_handler(int client_fd, weixind_t *weixind)
{
  
}

weixind_t *weixind_init(void)
{
  weixind_t *weixind;
  struct epoll_event ee;
  
  weixind = calloc(1, sizeof(*weixind));
  if (weixind == NULL) {
    return NULL;
  }

  weixind->listen_fd = tcp_listen(WEIXIND_LISTEN_PORT, WEIXIND_LISTEN_BACKLOG);
  if (weixind->listen_fd < 0) {
    free(weixind);
    return NULL;
  }

  weixind->epoll_fd = epoll_create(255);
  if (weixind->epoll_fd < 0) {
    free(weixind);
    return NULL;
  }
  
  ee.events = EPOLLIN;
  ee.data.fd = weixind->listen_fd;
  if (epoll_ctl(weixind->epoll_fd, EPOLL_CTL_ADD, weixind->listen_fd, &ee) < 0) {
    free(weixind);
    return NULL;
  }
  
  return weixind;
}

int weixind_server(void) 
{
  weixind_t *weixind;
  struct epoll_event events[WEIXIND_EVENT_PER_LOOP];
  int i, n;

  weixind = weixind_init();
  if (weixind == NULL) {
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
