#include "mysql.h"

MYSQL *db_init(const char *host, int port,
               const char *user, const char *passwd,
               const char *db)
{
  MYSQL *mysql;
  mysql = mysql_init(NULL);
  if (mysql == NULL) {
    return NULL;
  }
  if (mysql_real_connect(mysql, host, user, passwd, db, port, NULL, 0) == NULL) {
    mysql_close(mysql);
    return NULL;
  }
  return mysql;
}

void db_done(MYSQL *mysql)
{
  if (mysql == NULL) {
    return;
  }
  mysql_close(mysql);
}
