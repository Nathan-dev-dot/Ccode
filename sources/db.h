#ifndef DB_H
#define DB_H

int setCredentials (char *username, char *pwd) ;
int initDB (char *name) ;
int connectDB (char *command) ;
int reachMysql (MysqlCoAndRes *db, char *command) ;
void dropDB (void) ;

#endif
