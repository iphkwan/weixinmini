#include "weixin.h"
#include "weixind.h"

weixin_t *weixin_init(void)
{
  weixin_t *weixin;
  weixin = calloc(1, sizeof(*weixin));
  if (weixin == NULL) {
    return NULL;
  }
  strcpy(weixin->uid, "-1");
  weixin->mutex = malloc(sizeof(*weixin->mutex));
  if (weixin->mutex == NULL) {
    free(weixin);
    return NULL;
  }
  if (pthread_mutex_init(weixin->mutex, NULL) != 0) {
    free(weixin->mutex);
    free(weixin);
    return NULL;
  }
  return weixin;
}

void weixin_done(weixin_t *weixin)
{
  pthread_mutex_destroy(weixin->mutex);
  free(weixin);
}

char *weixin_getline(char *buffer, size_t len)
{
  if (fgets(buffer, len, stdin) == NULL) {
    return NULL;
  }
  buffer[strlen(buffer) - 1] = '\0';
  return buffer;
}

void *weixin_client_handler(void *arg)
{
  /* handle user input */
  weixin_t *weixin = arg;
  char request[BUF_SIZE + sizeof(weixin->uid)];
  char *buffer;
  snprintf(request, sizeof(request), "%s ", weixin->uid);
  buffer = request + strlen(weixin->uid) + 1;
  while (weixin_getline(buffer, BUF_SIZE)) {
    if (strcmp(buffer, "login") == 0) {
      char passwd[32];
      printf("uid: ");
      if (weixin_getline(weixin->uid, sizeof(weixin->uid)) == NULL) {
        fprintf(stderr, "invalid uid\n");
        continue;
      }
      printf("pwd: ");
      if (weixin_getline(passwd, sizeof(passwd)) == NULL) {
        fprintf(stderr, "invalid password\n");
        continue;
      }
      snprintf(request, sizeof(request), "-1 login %s %s", weixin->uid, passwd);
      write(weixin->fd, request, strlen(request));
      snprintf(request, sizeof(request), "%s ", weixin->uid);
      buffer = request + strlen(weixin->uid) + 1;
    } else if (strcmp(buffer, "logout") == 0) {
      write(weixin->fd, request, strlen(request));
      strcpy(weixin->uid, "-1");
      snprintf(request, sizeof(request), "%s ", weixin->uid);
      buffer = request + strlen(weixin->uid) + 1;
    } else {
      write(weixin->fd, request, strlen(request));
    }
  }
  return (void *)0;
}

void *weixin_server_handler(void *arg)
{
  /* handle server message */
  weixin_t *weixin = arg;
  char buffer[BUF_SIZE];
  size_t n;
  while ((n = read(weixin->fd, buffer, sizeof(buffer))) > 0) {
    buffer[n] = '\0';
    printf("%s\n", buffer);
  }
  if (n < 0) {
    fprintf(stderr, "read failed: %s\n", strerror(errno));
    return (void *)-1;
  } 
  return (void *)0;
}

int weixin_run(const char *host, int port)
{
  weixin_t *weixin;
  char buffer[BUF_SIZE];
  
  weixin = weixin_init();
  if (weixin == NULL) {
    fprintf(stderr, "weixin_init failed\n");
    return -1;
  }
  
  weixin->fd = tcp_connect(host, port);
  if (weixin->fd < 0) {
    fprintf(stderr, "tcp_connect failed\n");
    return -1;
  }

  if (pthread_create(&weixin->tid, NULL, weixin_server_handler, weixin) != 0) {
    fprintf(stderr, "pthread_create failed\n");
    return -1;
  }
  weixin_client_handler(weixin);
  
  weixin_done(weixin);
  return 0;
}

void show_help(void)
{
  printf("weixin - mini weixin\n"
         "Usage: weixin host [options...]\n"
         "Options:\n"
         "-h        show this help\n");
}

int main(int argc, char *argv[])
{
  char *host;
  int port, opt;

  if (argc < 2) {
    show_help();
    return -1;
  }

  while ((opt = getopt(argc, argv, "h")) != -1) {
    switch (opt) {
    case 'h':
      show_help();
      return 0;
    }
  }

  host = argv[1];
  port = WEIXIND_LISTEN_PORT;
  
  return weixin_run(host, port);
}
