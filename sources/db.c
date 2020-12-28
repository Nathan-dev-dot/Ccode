/*
    Nathan Letourneau & Sarah Schlegel
    01.11.2020
    Program library with mysql functions
*/

#include "all.h"

extern char nameDB[30] ;

/*
Function : initDB
-------------------
Initialises the connection to the mysql database

char *name : name of the database

returns :
0 if ok
ERR_DB if something went wrong
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
    if(mysql_real_connect(&mysql, "localhost", "root", "root", NULL, 3306, NULL, 0) == NULL){
        printf("Unable to connect to the database\n");
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
-------------------
Connects to the databse created by createDB()
Sends the commands to the database

char *command : command sent to be executed
MYSQL_RES *results : structure of results (used only if output required)

returns :
0 if ok
ERR_DB if something went wrong
*/
int connectDB (char *command) {
    MYSQL mysql ;
    int success ;

    mysql_init(&mysql) ;
    mysql_options(&mysql, MYSQL_READ_DEFAULT_GROUP, "option") ;

    if(mysql_real_connect(&mysql, "localhost", "root", "root", nameDB, 3306, NULL, 0) == NULL) {
        printf("\nConnection to mysql database failed\n");
        mysql_close(&mysql);
        return ERR_DB ;
    }
    success = mysql_query(&mysql, command) ;
    if (success != 0)
        return 3 ;

    mysql_close(&mysql);
    return 0 ;
}

/*
Function : reachMysql
-------------------
Connects to the Mysql
Sends a commands to Mysql

char *command : command sent to be executed
MYSQL_RES *results : structure of results (used only if output required)

returns :
0 if ok
ERR_DB if something went wrong
*/
 int reachMysql (MysqlCoAndRes *db, char *command) {
    int success ;

    mysql_init(db->mysql) ;
    mysql_options(db->mysql, MYSQL_READ_DEFAULT_GROUP, "option") ;

    if(mysql_real_connect(db->mysql, "localhost", "root", "root", nameDB, 3306, NULL, 0) == NULL) {
        printf("\nConnection to mysql database failed\n");
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
-------------------
Drops the database if it was created and an error occured
*/
void dropDB (void) {
    MYSQL mysql ;
    char command[30] = "DROP DATABASE " ;

    strcat(command, nameDB) ;

    mysql_init(&mysql) ;
    mysql_options(&mysql, MYSQL_READ_DEFAULT_GROUP, "option") ;

    if(mysql_real_connect(&mysql, "localhost", "root", "root", nameDB, 3306, NULL, 0) == NULL){
        printf("\nConnection to mysql database failed\n");
        mysql_close(&mysql);
        return ;
    }
    mysql_query(&mysql,command);
    mysql_close(&mysql);
}


void xmlFromDB (void) {
    printf("Hello\n") ;
}
