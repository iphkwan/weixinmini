#include "command.h"

void command_register_handler(weixind_t *weixind, task_t *task);
void command_login_handler   (weixind_t *weixind, task_t *task);
void command_fadd_handler    (weixind_t *weixind, task_t *task);
void command_fdel_handler    (weixind_t *weixind, task_t *task);
void command_fsearch_handler (weixind_t *weixind, task_t *task);
void command_fsend_handler   (weixind_t *weixind, task_t *task);
void command_gadd_handler    (weixind_t *weixind, task_t *task);
void command_gdel_handler    (weixind_t *weixind, task_t *task);
void command_ginv_handler    (weixind_t *weixind, task_t *task);
void command_gsend_handler   (weixind_t *weixind, task_t *task);

command_t Commands[] = {
  { "register",        command_register_handler   },
  { "login",           command_login_handler      },

  { "fadd",            command_fadd_handler       },
  { "fdel",            command_fdel_handler       },
  { "fsearch",         command_fsearch_handler    },
  { "fsend",           command_fsend_handler      },

  { "gadd",            command_gadd_handler       },
  { "gdel",            command_gdel_handler       },
  { "ginv",            command_ginv_handler       },
  { "gsend",           command_gsend_handler      },

  { NULL,              NULL }
};

char *command_get_token(char **command)
{
  char *token, *next;
  for (token = *command; *token && isspace(*token); ++token);
  for (next = token; *next && !isspace(*next); ++next);
  *command = *next ? next + 1 : next;
  *next = '\0';
  if (*token) {
    return token;
  } else {
    return NULL;
  }
}

void command_register_handler(weixind_t *weixind, task_t *task)
{
  char *nickname, *passwd;
  nickname = command_get_token(&task->args);
  passwd = command_get_token(&task->args);
  if (nickname == NULL || passwd == NULL) {
    write(task->fd, STRING("error invalid arguments"));
  }
}

void command_login_handler   (weixind_t *weixind, task_t *task)
{
}

                         
void command_fadd_handler(weixind_t *weixind, task_t *task)
{
}

void command_fdel_handler    (weixind_t *weixind, task_t *task)
{
}

void command_fsearch_handler (weixind_t *weixind, task_t *task)
{
}

void command_fsend_handler(weixind_t *weixind, task_t *task)
{
}

                         
void command_gadd_handler    (weixind_t *weixind, task_t *task)
{
}

void command_gdel_handler    (weixind_t *weixind, task_t *task)
{
  
}

void command_ginv_handler    (weixind_t *weixind, task_t *task)
{
  
}

void command_gsend_handler   (weixind_t *weixind, task_t *task)
{
}
