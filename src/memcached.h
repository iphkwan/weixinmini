#ifndef _MEMCACHED_H_
#define _MEMCACHED_H_

#include <libmemcached/memcached.h>

memcached_st *mem_init(const char *host, int port);
void mem_done(memcached_st *mem);
int mem_login(memcached_st *mem, const char *uid, int fd);
int mem_is_online(memcached_st *mem, const char *uid);
int mem_logout(memcached_st *mem, const char *uid);

#endif /* _MEMCACHED_H_ */
