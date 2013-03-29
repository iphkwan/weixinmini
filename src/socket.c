#include "headers.h"

int tcp_listen(int port, int backlog)
{
  struct sockaddr_in sa;
  const int on = 1;
  int fd;

  if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    return -1;
  }
  
  bzero(&sa, sizeof(sa));
  sa.sin_family = AF_INET;
  sa.sin_port = htons(port);
  sa.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(fd, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
    return -1;
  }

  if (listen(fd, backlog) < 0) {
    return -1;
  }

  /* ignore error */
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));

  if (fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK) < 0) {
    return -1;
  }

  return fd;
}

int tcp_connect(const char *host, int port)
{
  struct sockaddr_in sa;
  const int on = 1;
  int fd;

  if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    return -1;
  }
  
  bzero(&sa, sizeof(sa));
  sa.sin_family = AF_INET;
  sa.sin_port = htons(port);
  if (inet_pton(AF_INET, host, &sa.sin_addr.s_addr) <= 0) {
    return -1;
  }

  if (connect(fd, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
    return -1;
  }

  return fd;
}
