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

void command_register_handler(weixind_t *weixind, task_t *task)
{
  write(task->fd, STRING("register"));
}

void command_login_handler   (weixind_t *weixind, task_t *task)
{
  
}

                         
void command_fadd_handler    (weixind_t *weixind, task_t *task)
{
  
}

void command_fdel_handler    (weixind_t *weixind, task_t *task)
{
  
}

void command_fsearch_handler (weixind_t *weixind, task_t *task)
{
  
}

void command_fsend_handler   (weixind_t *weixind, task_t *task)
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
