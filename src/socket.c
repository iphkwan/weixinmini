#include "weixin.h"

int tcp_listen(int port, int backlog)
{
  struct sockaddr_in sa;
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
  
  return fd;
}
