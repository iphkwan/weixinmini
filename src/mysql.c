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

#ifdef DEBUG_MYSQL
#include "weixind.h"

int main(int argc, char *argv[])
{
  MYSQL *mysql = db_init(WEIXIND_DB_HOST, WEIXIND_DB_PORT,
                         WEIXIND_DB_USER, WEIXIND_DB_PASSWD,
                         WEIXIND_DB_DB);
  if (mysql == NULL) {
    fprintf(stderr, "db_init failed\n");
    return -1;
  }
  db_done(mysql);
  return 0;
}
#endif
