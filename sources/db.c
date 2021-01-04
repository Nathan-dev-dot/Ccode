/*
    Nathan Letourneau & Sarah Schlegel
    01.11.2020
    Program library with mysql functions
*/

#include "all.h"

extern char nameDB[30] ;
char userDB[30] = "" ;
char pwdDB[50] = "" ;
char host[50] = "" ;
uint16_t port ;


/*
Function : setCredentials
--------------------------------------------------------------------------------
Sets the global connexion variables
Calls getDBConf to get the hostname and the port
--------------------------------------------------------------------------------
char *username : username to be set
char *pwd : password to be set
--------------------------------------------------------------------------------
Return values
    0 if ok
    ERR_DB if the connection to the mysql failed
*/
int setCredentials (char *username, char *pwd) {
    MYSQL mysql ;

    strcpy(userDB, username) ;
    strcpy(pwdDB, pwd) ;

    mysql_init(&mysql) ;
    mysql_options(&mysql, MYSQL_READ_DEFAULT_GROUP, "option") ;

    if (getDBConf(host, &port) != 0)
        return ERR_CONF ;

    if(mysql_real_connect(&mysql, host, userDB, pwdDB, NULL, port, NULL, 0) == NULL){
        printMessage(NULL, 0, "Unable to connect to mysql\n");
        mysql_close(&mysql);
        return ERR_DB ;
    }

    mysql_close(&mysql) ;

    return 0 ;
}

/*
Function : initDB
--------------------------------------------------------------------------------
Initialises the connection to the mysql database
Sets the value of the global variable nameDB

--------------------------------------------------------------------------------
char *name : name of the database
--------------------------------------------------------------------------------
Return values
    0 if ok
    ERR_DB if the connection to mysql failed
*/
int initDB (char *name) {
    MYSQL mysql ;
    char drop[100] = "DROP DATABASE IF EXISTS " ;
    char create[100] = "CREATE DATABASE " ;
    int success ;

    mysql_init(&mysql) ;
    mysql_options(&mysql, MYSQL_READ_DEFAULT_GROUP, "option") ;
    strcat(create, name) ;
    strcat(drop, name) ;
    if(mysql_real_connect(&mysql, host, userDB, pwdDB, NULL, port, NULL, 0) == NULL){
        printMessage(NULL, 0, "Unable to connect to mysql\n");
        mysql_close(&mysql);
        return ERR_DB ;
    }
    strcpy(nameDB, name) ;
    mysql_query(&mysql, drop) ;
    success = mysql_query(&mysql, create) == 0 ? 0 : 1 ;
    if (!success)
        writeSQLFile(create, 0) ;
    mysql_close(&mysql);
    return success ;
}

/*
Function : connectDB
--------------------------------------------------------------------------------
Connects to the databse created by createDB
Sends the commands to the database

--------------------------------------------------------------------------------
char *command : command sent to be executed
--------------------------------------------------------------------------------
Return values
    0 if ok
    ERR_DB if something went wrong
*/
int connectDB (char *command) {
    MYSQL mysql ;
    int success ;

    printf("Connect db / %s\n", command) ;

    mysql_init(&mysql) ;
    mysql_options(&mysql, MYSQL_READ_DEFAULT_GROUP, "option") ;

    if(mysql_real_connect(&mysql, host, userDB, pwdDB, nameDB, port, NULL, 0) == NULL) {
        printMessage(NULL, 0, "Connection to mysql database failed");
        mysql_close(&mysql);
        return ERR_DB ;
    }
    success = mysql_query(&mysql, command) ;
    printf("Success %d\n", success) ;
    if (success != 0)
        return 3 ;

    mysql_close(&mysql);
    return 0 ;
}

/*
Function : reachMysql
--------------------------------------------------------------------------------
Connects to the Mysql
Sends a query to Mysql expecting results

--------------------------------------------------------------------------------
char *command : command sent to be executed
MysqlCoAndRes : struct containing the mysql data and the results
--------------------------------------------------------------------------------
Return values
    0 if ok
    ERR_DB if something went wrong
*/
 int reachMysql (MysqlCoAndRes *db, char *command) {
    int success ;

    mysql_init(db->mysql) ;
    mysql_options(db->mysql, MYSQL_READ_DEFAULT_GROUP, "option") ;

    if(mysql_real_connect(db->mysql, host, userDB, pwdDB, nameDB, port, NULL, 0) == NULL) {
        printMessage(NULL, 0, "Connexion to mysql database failed");
        mysql_close(db->mysql);
        return ERR_DB ;
    }

    success = mysql_query(db->mysql, command) == 0 ? 0 : 3;
    db->results = mysql_use_result(db->mysql) ;
    if (strncmp(command, "USE ", 4) != 0)
        db->nbFields = mysql_num_fields(db->results);
    else
        db->nbFields = 1 ;

    return success ;
}

/*
Function : dropDB
--------------------------------------------------------------------------------
Drops the database if it was created

--------------------------------------------------------------------------------

*/
void dropDB (void) {
    MYSQL mysql ;
    char command[30] = "DROP DATABASE " ;

    strcat(command, nameDB) ;

    mysql_init(&mysql) ;
    mysql_options(&mysql, MYSQL_READ_DEFAULT_GROUP, "option") ;

    if(mysql_real_connect(&mysql, host, userDB, pwdDB, nameDB, port, NULL, 0) == NULL){
        printMessage(NULL, 0, "Connection to mysql database failed") ;
        mysql_close(&mysql);
        return ;
    }
    mysql_query(&mysql,command);
    mysql_close(&mysql);
}
