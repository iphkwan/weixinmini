#include "memcached.h"
#include "headers.h"

memcached_st *mem_init(const char *host, int port)
{
  memcached_st *mem;
  memcached_return_t rc;

  mem = memcached_create(NULL);
  if (mem == NULL) {
    return NULL;
  }

  rc = memcached_server_add(mem, host, port);
  if (rc != MEMCACHED_SUCCESS) {
    memcached_free(mem);
    return NULL;
  }

  return mem;
}

void mem_done(memcached_st *mem)
{
  if (mem) {
    memcached_free(mem);
  }
}

int mem_login(memcached_st *mem, const char *uid, int fd)
{
  char value[32];
  memcached_return_t rc;
  if (mem == NULL || uid == NULL) {
    return -1;
  }
  snprintf(value, sizeof(value), "%d", fd);
  rc = memcached_set(mem, uid, strlen(uid), value, strlen(value), 0, 0);
  return rc == MEMCACHED_SUCCESS ? 0 : -1;
}

int mem_is_online(memcached_st *mem, const char *uid)
{
  char *value;
  memcached_return_t rc;
  if (mem == NULL || uid == NULL) {
    return -1;
  }
  value = memcached_get(mem, uid, strlen(uid), NULL, NULL, &rc);
  return rc == MEMCACHED_SUCCESS ? atoi(value) : -1;
}

int mem_logout(memcached_st *mem, const char *uid)
{
  memcached_return_t rc;
  if (mem == NULL || uid == NULL) {
    return -1;
  }
  rc = memcached_delete(mem, uid, strlen(uid), 0);
  return rc == MEMCACHED_SUCCESS ? 0 : -1;
}


#ifdef DEBUG_MEM
int main(int argc, char *argv[])
{
  memcached_st *mem = mem_init("localhost", MEMCACHED_DEFAULT_PORT);
  if (mem == NULL) {
    return -1;
  }
  if (argc != 3) {
    return -1;
  }
  if (strcmp(argv[1], "login") == 0) {
    if (mem_login(mem, argv[2], 10) < 0) {
      fprintf(stderr, "%s login failed\n", argv[2]);
      return -1;
    }
  } else if (strcmp(argv[1], "logout") == 0) {
    if (mem_logout(mem, argv[2]) < 0) {
      fprintf(stderr, "%s logout failed\n", argv[2]);
      return -1;
    }
  } else if (strcmp(argv[1], "online") == 0) {
    printf("%s %d\n", argv[2], mem_is_online(mem, argv[2]));
  }

  return 0;
}
#endif
