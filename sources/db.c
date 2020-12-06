/*
    Nathan Letourneau & Sarah Schlegel
    01.11.2020
    Library with mysql functions
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>
#include <mysql/mysql.h>

#include "str.h"
#include "db.h"
#include "sql.h"
#include "xml.h"
#include "errCodes.h"

extern char nameDB[30] ;

/*
Function : initDB
-------------------
Initialises the connection to the mysql database

char *name : name of the database

returns :
0 if ok
1 if something went wrong
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

returns :
0 if ok
1 if something went wrong
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
    success = mysql_query(&mysql, command) == 0 ? 0 : 3;
    mysql_close(&mysql);
    return 0 ;
}

/*
Function : dropD
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
