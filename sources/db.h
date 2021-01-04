#ifndef DB_H
#define DB_H

uint8_t setCredentials (char *username, char *pwd) ;
uint8_t initDB (char *name) ;
uint8_t connectDB (char *command) ;
uint8_t reachMysql (MysqlCoAndRes *db, char *command) ;
void dropDB (void) ;

#endif
