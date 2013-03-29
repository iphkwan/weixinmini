#ifndef _MYSQL_H_
#define _MYSQL_H_

#include <mysql/mysql.h>
#include "headers.h"

MYSQL *db_init(const char *host, int port,
               const char *user, const char *passwd,
               const char *db);
void db_done(MYSQL *db);

#endif /* _MYSQL_H_ */
