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
  if (*token == '\"') {
    token += 1;
    for (next = token; *next && *next != '\"'; ++next);
  } else {
    for (next = token; *next && !isspace(*next); ++next);
  }
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
    write(task->fd, STRING("response error invalid arguments"));
    return;
  }
  snprintf(sql, sizeof(sql), "INSERT INTO weixin_user(pwd, name, timestamp)"
           " VALUES ('%s', '%s', Now());", pwd, name);
  ret = mysql_query(weixind->db, sql);
  if (ret == 0) {
    snprintf(response, sizeof(response), "response ok uid = %ld",
             mysql_insert_id(weixind->db));
    write(task->fd, STRING(response));
  } else {
    write(task->fd, STRING("response error database"));
  }
}

void command_login_handler   (weixind_t *weixind, task_t *task)
{
  char *uid, *pwd;
  char sql[BUF_SIZE];
  char message[BUF_SIZE];
  char timestamp[50];
  int ret;
  uid = command_get_token(&task->args);
  pwd = command_get_token(&task->args);
  if (uid == NULL || pwd == NULL) {
    write(task->fd, STRING("response error invalid arguments"));
    return;
  }

  if (mem_is_online(weixind->mem, uid) > 0) {
    write(task->fd, STRING("response error already logined"));
    return;
  }

  snprintf(sql, sizeof(sql), "SELECT timestamp FROM weixin_user"
           " WHERE uid = %ld and pwd = '%s';", atol(uid), pwd);
  ret = mysql_query(weixind->db, sql);
  if (ret == 0) {
    MYSQL_RES *res = mysql_store_result(weixind->db);
    MYSQL_ROW row = mysql_fetch_row(res);
    if (!row) {
      mysql_free_result(res);
      write(task->fd, STRING("response error uid or pwd wrong"));
      return;
    }
    strncpy(timestamp, row[0], sizeof(timestamp));
    mysql_free_result(res);
  }

  snprintf(sql, sizeof(sql), "UPDATE weixin_user SET timestamp = Now()"
           " WHERE uid = %ld and pwd = '%s';", atol(uid), pwd);
  ret = mysql_query(weixind->db, sql);
  if (ret == 0) {
    if (mysql_affected_rows(weixind->db) == 1) {
      if (mem_login(weixind->mem, uid, task->fd) == 0) {
        write(task->fd, STRING("response ok"));
        snprintf(sql, sizeof(sql), "SELECT fid, content FROM weixin_usermsg"
                 " WHERE uid = %ld and timestamp > '%s';",
                 atol(uid), timestamp);
        ret = mysql_query(weixind->db, sql);
        if (ret == 0) {
          MYSQL_RES *res = mysql_store_result(weixind->db);
          MYSQL_ROW row;
          while (row = mysql_fetch_row(res)) {
            snprintf(message, sizeof(message), "message from [fid:%s] %s",
                     row[0], row[1]);
            write(task->fd, STRING(message));
          }
          mysql_free_result(res);
        }
        /* ignore error */
      } else {
        write(task->fd, STRING("response error memcached"));
      }
    } else {
      write(task->fd, STRING("response error uid or pwd wrong"));
    }
  } else {
    write(task->fd, STRING("response error database"));
  }
}

void command_logout_handler  (weixind_t *weixind, task_t *task)
{
  /* FIXME: user abort without logout */
  char sql[BUF_SIZE];
  int ret;
  if (mem_is_online >= 0) {
    mem_logout(weixind->mem, task->uid);
    snprintf(sql, sizeof(sql), "UPDATE weixin_user SET timestamp = Now()"
             " WHERE uid = %ld;", atol(task->uid));
    ret = mysql_query(weixind->db, sql);
    if (ret == 0) {
      write(task->fd, STRING("response ok"));
    } else {
      write(task->fd, STRING("response error database"));
    }
  }
}
                         
void command_fadd_handler(weixind_t *weixind, task_t *task)
{
  char *fid;
  char sql[BUF_SIZE];
  int ret;
  fid = command_get_token(&task->args);
  if (fid == NULL) {
    write(task->fd, STRING("response error invalid arguments"));
    return;
  }

  snprintf(sql, sizeof(sql), "SELECT * FROM weixin_friend"
           " WHERE uid = %ld and fid = %ld or fid = %ld and uid = %ld;",
           atol(task->uid), atol(fid), atol(task->uid), atol(fid));
  ret = mysql_query(weixind->db, sql);
  if (ret == 0) {
    MYSQL_RES *res;
    res = mysql_store_result(weixind->db);
    if (mysql_num_rows(res) > 0) {
      write(task->fd, STRING("response ok"));
      mysql_free_result(res);
      return;
    }
    mysql_free_result(res);
  } else {
    write(task->fd, STRING("response error database"));
    return;
  }

  snprintf(sql, sizeof(sql), "INSERT INTO weixin_friend(uid, fid)"
           " VALUES (%ld, %ld);", atol(task->uid), atol(fid));
  ret = mysql_query(weixind->db, sql);
  if (ret == 0) {
    write(task->fd, STRING("response ok"));
  } else {
    write(task->fd, STRING("response error database"));
  }
}

void command_fdel_handler    (weixind_t *weixind, task_t *task)
{
  char *fid;
  char sql[BUF_SIZE];
  int ret;
  fid = command_get_token(&task->args);
  if (fid == NULL) {
    write(task->fd, STRING("response error invalid arguments"));
    return;
  }
  snprintf(sql, sizeof(sql), "DELETE FROM weixin_friend"
           " WHERE uid = %ld and fid = %ld or fid = %ld and uid = %ld;",
           atol(task->uid), atol(fid), atol(task->uid), atol(fid));
  ret = mysql_query(weixind->db, sql);
  if (ret == 0) {
    if (mysql_affected_rows(weixind->db) > 0) {
      write(task->fd, STRING("response ok"));
    } else {
      write(task->fd, STRING("response error friend not found"));
    }
  } else {
    write(task->fd, STRING("response error database"));
  }
}

void command_fsearch_handler (weixind_t *weixind, task_t *task)
{
  char *name;
  char sql[BUF_SIZE];
  char response[BUF_SIZE];
  int ret;
  name = command_get_token(&task->args);
  if (name == NULL) {
    write(task->fd, STRING("response error invalid arguments"));
    return;
  }
  snprintf(sql, sizeof(sql), "SELECT uid, name FROM weixin_user"
           " WHERE name LIKE '%%%s%%';", name);
  ret = mysql_query(weixind->db, sql);
  if (ret == 0) {
    MYSQL_RES *res = mysql_store_result(weixind->db);
    unsigned int num_fields = mysql_num_fields(res);
    MYSQL_ROW row;
    int i, size;;
    for (size = 0; size < sizeof(response) - 1 && (row = mysql_fetch_row(res)); ) {
      for (i = 0; size < sizeof(response) - 1 && i < num_fields; ++i) {
        size += snprintf(response + size, sizeof(response) - size,
                         "%s ", row[i]);
      }
      response[size++] = '\n';
    }
    write(task->fd, response, size);
    mysql_free_result(res);
  } else {
    write(task->fd, STRING("response error database"));
  }
}

void command_fsend_handler(weixind_t *weixind, task_t *task)
{
  char *fid, *content;
  char sql[BUF_SIZE];
  int ret;
  int fd;
  fid = command_get_token(&task->args);
  content = command_get_token(&task->args);
  if (fid == NULL || content == NULL) {
    write(task->fd, STRING("response error invalid arguments"));
    return;
  }

  snprintf(sql, sizeof(sql), "SELECT * FROM weixin_friend"
           " WHERE uid = %ld and fid = %ld or fid = %ld and uid = %ld;",
           atol(task->uid), atol(fid), atol(task->uid), atol(fid));
  ret = mysql_query(weixind->db, sql);
  if (ret == 0) {
    MYSQL_RES *res = mysql_store_result(weixind->db);
    if (mysql_num_rows(res) <= 0) {
      mysql_free_result(res);
      write(task->fd, STRING("response error you are not friends"));
      return;
    }
    mysql_free_result(res);
  } else {
    write(task->fd, STRING("response error database"));
    return;
  }
  
  snprintf(sql, sizeof(sql), "INSERT INTO weixin_usermsg (uid, fid, content, timestamp)"
           " VALUES (%ld, %ld, '%s', Now());",
           atol(fid), atol(task->uid), content);
  ret = mysql_query(weixind->db, sql);
  if (ret == 0) {
    write(task->fd, STRING("response ok"));
  } else {
    write(task->fd, STRING("response error database"));
  }
  
  fd = mem_is_online(weixind->mem, fid);
  if (fd > 0) {
    char message[BUF_SIZE];
    snprintf(message, sizeof(message), "message [fid:%ld] %s",
             atol(task->uid), content);
    write(fd, STRING(message));
  }
}

void command_gadd_handler    (weixind_t *weixind, task_t *task)
{
  char *name;
  char sql[BUF_SIZE];
  char response[BUF_SIZE];
  int ret;
  name = command_get_token(&task->args);
  if (name == NULL) {
    write(task->fd, STRING("response error invalid arguments"));
    return;
  }

  snprintf(sql, sizeof(sql), "INSERT INTO weixin_group (uid, name)"
           " VALUES (%ld, '%s');", atol(task->uid), name);
  ret = mysql_query(weixind->db, sql);
  if (ret == 0) {
    int gid = mysql_insert_id(weixind->db);
    snprintf(sql, sizeof(sql), "INSERT INTO weixin_user_group (uid, gid)"
             " VALUES (%ld, %ld);", atol(task->uid), gid);
    ret = mysql_query(weixind->db, sql);
    /* FIXME: check ret */
    snprintf(response, sizeof(response), "response ok gid = %ld", gid);
    write(task->fd, STRING(response));
  } else {
    write(task->fd, STRING("response error database"));
  }
}

void command_gdel_handler    (weixind_t *weixind, task_t *task)
{
  char *gid;
  char sql[BUF_SIZE];
  int ret;
  gid = command_get_token(&task->args);
  if (gid == NULL) {
    write(task->fd, STRING("response error invalid arguments"));
    return;
  }

  snprintf(sql, sizeof(sql), "SELECT uid FROM weixin_group"
           " WHERE gid = %ld;", atol(gid));
  ret = mysql_query(weixind->db, sql);
  if (ret == 0) {
    MYSQL_RES *res = mysql_store_result(weixind->db);
    MYSQL_ROW row = mysql_fetch_row(res);
    if (!row) {
      write(task->fd, STRING("response error group not found"));
      mysql_free_result(res);
      return;
    } else if (strcmp(row[0], task->uid) != 0) {
      write(task->fd, STRING("response error you are not the group onwer"));
      mysql_free_result(res);
      return;
    }
  } else {
    write(task->fd, STRING("response error database"));
    return;
  }

  snprintf(sql, sizeof(sql), "DELETE FROM weixin_user_group"
           " WHERE gid = %ld;", atol(gid));
  ret = mysql_query(weixind->db, sql);
  if (ret != 0) {
    write(task->fd, STRING("response error database"));
    return;
  }

  snprintf(sql, sizeof(sql), "DELETE FROM weixin_groupmsg"
           " WHERE gid = %ld;", atol(gid));
  ret = mysql_query(weixind->db, sql);
  if (ret != 0) {
    write(task->fd, STRING("response error database"));
    return;
  }

  snprintf(sql, sizeof(sql), "DELETE FROM weixin_group"
           " WHERE uid = %ld and gid = %ld;", atol(task->uid), atol(gid));
  ret = mysql_query(weixind->db, sql);
  if (ret == 0) {
    if (mysql_affected_rows(weixind->db) == 1) {
      write(task->fd, STRING("response ok"));
    } else {
      write(task->fd, STRING("response error group not found or you are not the group onwer"));      
    }
  } else {
    write(task->fd, STRING("response error database"));
  }
}

void command_ginv_handler    (weixind_t *weixind, task_t *task)
{
  char *gid, *uid;
  char sql[BUF_SIZE];
  char response[BUF_SIZE];
  int ret;
  gid = command_get_token(&task->args);
  uid = command_get_token(&task->args);
  if (gid == NULL || uid == NULL) {
    write(task->fd, STRING("response error invalid arguments"));
    return;
  }

  snprintf(sql, sizeof(sql), "SELECT * FROM weixin_friend"
           " WHERE uid = %ld and fid = %ld or fid = %ld and uid = %ld;",
           atol(task->uid), atoi(uid), atol(task->uid), atol(uid));
  ret = mysql_query(weixind->db, sql);
  if (ret == 0) {
    MYSQL_RES *res = mysql_store_result(weixind->db);
    if (mysql_num_rows(res) <= 0) {
      mysql_free_result(res);
      write(task->fd, STRING("response error user if not your friend"));
      return;
    }
    mysql_free_result(res);
  } else {
    write(task->fd, STRING("response error database"));
    return;
  }
  
  snprintf(sql, sizeof(sql), "INSERT INTO weixin_user_group (uid, gid)"
           " VALUES (%ld, %ld);", atol(uid), atol(gid));
  ret = mysql_query(weixind->db, sql);
  if (ret == 0) {
    write(task->fd, STRING("response ok"));
  } else {
    write(task->fd, STRING("response error group or user not found"));
  }
}

void command_gsend_handler   (weixind_t *weixind, task_t *task)
{
  char *gid, *content;
  char sql[BUF_SIZE];
  int ret;
  int fd;
  gid = command_get_token(&task->args);
  content = command_get_token(&task->args);
  if (gid == NULL || content == NULL) {
    write(task->fd, STRING("response error invalid arguments"));
    return;
  }

  snprintf(sql, sizeof(sql), "INSERT INTO weixin_groupmsg (uid, gid, content, timestamp)"
           " VALUES (%ld, %ld, '%s', Now());", atol(task->uid), atol(gid), content);
  ret = mysql_query(weixind->db, sql);
  if (ret != 0) {
    write(task->fd, STRING("response error database"));
    return;
  }

  snprintf(sql, sizeof(sql), "SELECT uid FROM weixin_user_group"
           " WHERE gid = %ld;", atol(gid));
  ret = mysql_query(weixind->db, sql);
  if (ret == 0) {
    MYSQL_RES *res = mysql_store_result(weixind->db);
    MYSQL_ROW row;
    int fd;
    while (row = mysql_fetch_row(res)) {
      fd = mem_is_online(weixind->mem, row[0]);
      if (fd > 0) {
        char message[BUF_SIZE];
        snprintf(message, sizeof(message), "message [gid:%ld, uid:%ld] %s",
                 atol(gid), atol(task->uid), content);
        write(fd, STRING(message));
      }
    }
  }
}
