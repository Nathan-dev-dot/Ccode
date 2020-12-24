#ifndef DB_H
#define DB_H

int initDB (char *name) ;
int connectDB (char *command, MYSQL_RES *results) ;
int reachMysql (MysqlCoAndRes *db, char *command) ;
void dropDB (void) ;
void xmlFromDB (void) ;

#endif
