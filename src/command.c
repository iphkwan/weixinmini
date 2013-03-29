#include "command.h"

void command_register_handler(weixind_t *weixind, task_t *task);
void command_login_handler   (weixind_t *weixind, task_t *task);
void command_logout_handler  (weixind_t *weixind, task_t *task);
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
  { "logout",          command_logout_handler     },
  
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
  char *name, *pwd;
  char sql[BUF_SIZE];
  char response[BUF_SIZE];
  int ret;
  name = command_get_token(&task->args);
  pwd = command_get_token(&task->args);
  if (pwd == NULL || name == NULL) {
    write(task->fd, STRING("error invalid arguments"));
    return;
  }
  snprintf(sql, sizeof(sql), "INSERT INTO weixin_user(pwd, name, timestamp)"
           " VALUES ('%s', '%s', Now());", pwd, name);
  ret = mysql_query(weixind->db, sql);
  if (ret == 0) {
    my_ulonglong uid = mysql_insert_id(weixind->db);
    MYSQL_RES *res;
    res = mysql_use_result(weixind->db);
    mysql_free_result(res);
    snprintf(response, sizeof(response), "response ok %ld", uid);
    write(task->fd, response, strlen(response));
  } else {
    write(task->fd, STRING("response error"));
  }
}

void command_login_handler   (weixind_t *weixind, task_t *task)
{
  char *uid, *pwd;
  char sql[BUF_SIZE];
  int ret;
  uid = command_get_token(&task->args);
  pwd = command_get_token(&task->args);
  if (uid == NULL || pwd == NULL) {
    write(task->fd, STRING("error invalid arguments"));
    return;
  }
  snprintf(sql, sizeof(sql), "SELECT * FROM weixin_user"
           " WHERE uid = %ld and pwd = '%s';", atol(uid), pwd);
  ret = mysql_query(weixind->db, sql);
  if (ret == 0) {
    MYSQL_RES *res;
    res = mysql_store_result(weixind->db);
    if (mysql_num_rows(res) == 1) {
      if (mem_login(weixind->mem, uid, task->fd) == 0) {
        write(task->fd, STRING("response ok"));
      } else {
        write(task->fd, STRING("response error memcached"));
      }
    } else {
      write(task->fd, STRING("response error uid or pwd wrong"));
    }
    mysql_free_result(res);
  } else {
    write(task->fd, STRING("response error database"));
  }
}

void command_logout_handler  (weixind_t *weixind, task_t *task)
{
  mem_logout(weixind->mem, task->uid);
  write(task->fd, STRING("response ok"));
}
                         
void command_fadd_handler(weixind_t *weixind, task_t *task)
{
  char *fid;
  char sql[BUF_SIZE];
  int ret;
  fid = command_get_token(&task->args);
  if (fid == NULL) {
    write(task->fd, STRING("error invalid arguments"));
    return;
  }
  snprintf(sql, sizeof(sql), "SELECT * FROM weixin_friend"
           " WHERE uid = %ld and fid = %ld or fid = %ld and uid = %ld;",
           atol(task->uid), atol(fid), atol(task->uid), atol(fid));
  ret = mysql_query(weixind->db, sql);
  if (ret == 0) {
    MYSQL_RES *res;
    res = mysql_use_result(weixind->db);
    mysql_free_result(res);
    write(task->fd, STRING("response ok"));
    return;
  }
  snprintf(sql, sizeof(sql), "INSERT INTO weixin_friend(uid, fid)"
           " VALUES (%ld, %ld);", atol(task->uid), atol(fid));
  ret = mysql_query(weixind->db, sql);
  if (ret == 0) {
    MYSQL_RES *res;
    res = mysql_use_result(weixind->db);
    mysql_free_result(res);
    write(task->fd, STRING("response ok"));
  } else {
    write(task->fd, STRING("response error"));
  }
}

void command_fdel_handler    (weixind_t *weixind, task_t *task)
{
  char *fid;
  char sql[BUF_SIZE];
  int ret;
  fid = command_get_token(&task->args);
  if (fid == NULL) {
    write(task->fd, STRING("error invalid arguments"));
    return;
  }
  snprintf(sql, sizeof(sql), "DELETE FROM weixin_friend"
           " WHERE uid = %ld and fid = %ld or fid = %ld and uid = %ld;",
           atol(task->uid), atol(fid), atol(task->uid), atol(fid));
  ret = mysql_query(weixind->db, sql);
  if (ret == 0) {
    MYSQL_RES *res;
    res = mysql_use_result(weixind->db);
    mysql_free_result(res);
    write(task->fd, STRING("response ok"));
  } else {
    write(task->fd, STRING("response error"));
  }
}

void command_fsearch_handler (weixind_t *weixind, task_t *task)
{
  char *name;
  char sql[BUF_SIZE];
  int ret;
  name = command_get_token(&task->args);
  if (name == NULL) {
    write(task->fd, STRING("error invalid arguments"));
    return;
  }
  snprintf(sql, sizeof(sql), "SELECT FROM weixin_user"
           " WHERE name LINKS '%%%s%%';", name);
  ret = mysql_query(weixind->db, sql);
  if (ret == 0) {
    MYSQL_RES *res;
    res = mysql_use_result(weixind->db);
    mysql_free_result(res);
    write(task->fd, STRING("response ok"));
  } else {
    write(task->fd, STRING("response error"));
  }
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
