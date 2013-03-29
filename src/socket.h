#ifndef _SOCKET_H_
#define _SOCKET_H_

int tcp_listen(int port, int backlog);
int tcp_connect(const char *host, int port);

#endif /* _SOCKET_H_ */
