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

#include "db.h"
#include "sql.h"
#include "xml.h"
#include "str.h"

extern FILE *confFile ;
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
int initDB (FILE *sqlFile, char *name) {
    MYSQL mysql ;
    char command[30] = "CREATE DATABASE " ;
    mysql_init(&mysql) ;
    mysql_options(&mysql, MYSQL_READ_DEFAULT_GROUP, "option") ;
    strcat(command, name) ;
    writeSqlFile(sqlFile, command);
    if(mysql_real_connect(&mysql, "localhost", "root", "root", NULL, 3306, NULL, 0) == NULL){
        printf("Connection to mysql failed\n");
        mysql_close(&mysql);
        return 1 ;
    }
    strcpy(nameDB, name) ;
    mysql_query(&mysql, command);
    mysql_close(&mysql);
    return 0 ;
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

    mysql_init(&mysql) ;
    mysql_options(&mysql, MYSQL_READ_DEFAULT_GROUP, "option") ;


    if(mysql_real_connect(&mysql, "localhost", "root", "root", nameDB, 3306, NULL, 0) == NULL){
        printf("\nConnection to mysql database failed\n");
        mysql_close(&mysql);
        return 1 ;
    }
    mysql_query(&mysql,command);
    mysql_close(&mysql);
    return 0 ;
}
