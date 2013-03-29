CC = gcc
CXX = g++

CPPFLAGS = 
CFLAGS = -g
LIBS = -lpthread -lmysqlclient -lmemcached

WEIXIND_SRCS = weixind.c socket.c memcached.c task.c command.c mysql.c
WEIXIN_SRCS = weixin.c socket.c
MEM_SRCS = memcached.c

all: weixind weixin test_mem

weixind: $(WEIXIND_SRCS)
	$(CC) -o $@ $^ $(CPPFLAGS) $(CFLAGS) $(LIBS)

weixin: $(WEIXIN_SRCS)
	$(CC) -o $@ $^ $(CPPFLAGS) $(CFLAGS) $(LIBS)

test_mem: $(MEM_SRCS)
	$(CC) -o $@ $^ -DDEBUG_MEM $(CPPFLAGS) $(CFLAGS) $(LIBS)

