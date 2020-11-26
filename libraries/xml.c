/*
    Nathan Letourneau & Sarah Schlegel
    01.11.2020
    Library with XML functions
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
Function : initConf
-------------------
Reads the config file of the columns and stores the data in an array
(Storing format : n/m | propName)

char *conf[10][2][20] : configuration array

returns :
0 if ok
1 in case of an error
*/
int initConf (char *conf[10][2][20]) {
    int i = 0 ;
    char str[50] ;
    fseek(confFile, 47, SEEK_SET) ;
    while (fgets(str, 50, confFile) != NULL && strcmp(str, "\n")) {
        strcpy((char *)conf[i][0], "") ;
        strncat((char *)conf[i][0], str, 1) ;
        strcpy((char *)conf[i][1], "") ;
        strncat((char *)conf[i][1], strchr(str, ':') + 1, strlen(strchr(str, ':') + 1) - 1) ;
        i++ ;
    }
    if (i < 10) {
        strcpy((char *)conf[i][0], "STOP") ;
        strcpy((char *)conf[i][1], "STOP") ;
    }
    return 0 ;
}

/*
Function : parseDoc
-------------------
Opens the required files for the program (configuration, database and xml)
Initialises the database then calls parseNodes

returns :
0 if ok
1 if something went wrong
*/
int parseDoc(void) {
    int kill ;
    FILE *sqlFile ;
    char xmlFile[100];
    xmlDocPtr doc;
    xmlNodePtr root;

    confFile = fopen("../config", "r") ;
    if (confFile == NULL) {
        fprintf(stderr, "Config file not found\n") ;
        return EXIT_FAILURE ;
    }

    sqlFile = fopen("../db.sql", "w") ;
    if (sqlFile == NULL) {
        fprintf(stderr, "Error in creating the sql file\n") ;
        return EXIT_FAILURE ;
    }

    enterPath(xmlFile) ;
    doc = xmlParseFile(xmlFile) ;

    if (doc == NULL) {
         fprintf(stderr, "Invalid XML document\n") ;
         return EXIT_FAILURE ;
    }

    root = xmlDocGetRootElement(doc);

    kill = initDB(sqlFile,(char *)xmlGetProp(root, (const xmlChar *)"dbname")) ;
    if (kill != 0) {
        return 1 ;
    }
    kill = parseNodes(root->children, sqlFile) ;
    if (kill != 0) {
        return 1 ;
    }

    fclose(confFile);
    fclose(sqlFile);
    xmlFreeDoc(doc);
    return 0 ;
}


/*
Function : parseNodes
-------------------
Parses the XML file through the table nodes
Sets the variables used for parsing the tables
Writes a full table creation command

xmlNodePtr node : first table node
FILE *sqlFile : sql file to be written

returns :
0 if ok
1 if something went wrong
*/
int parseNodes (xmlNodePtr node, FILE *sqlFile) {
    int kill ;
    int i ;
    int j ;
    xmlNodePtr n ;
    char *colConf[10][2][20] = { "" } ;
    char *primKeys[30][20] ;
    char *foreignKeys[15][3][20] ;
    char command[500] ;
    MYSQL mysql ;
    kill = initConf(colConf) ;
    if (kill != 0) {
        return 1 ;
    }
    for (i = 0 ; i < 15 ; ++i)
        for (j = 0 ; j < 3 ; ++j)
            strcpy((char *)foreignKeys[i][j], "STOP") ;
    for (n = node ; n != NULL ; n = n->next) {
        kill = getForeignKeys(foreignKeys, n) ;
        if (kill != 0)
            return 1 ;
        strcpy(command, "") ;
        if (n->type == XML_ELEMENT_NODE && strcmp((const char *)n->name, "table") == 0){

            for (j = 0 ; j < 10 ; ++j) {
                strcpy((char *)primKeys[j], "STOP") ;
            }
            if (xmlGetProp(n, (const xmlChar *)"tname") == NULL)
                return 1 ;
            strcat(strcat(strcat(command, "CREATE TABLE "), (const char *)xmlGetProp(n, (const xmlChar *)"tname")), "(") ;
            kill = writeSQLColumn(n->children, colConf, command, primKeys, foreignKeys) ;
            if (kill != 0) {
                return 1 ;
            }
            catPrimaryKeys(primKeys, command) ;
            strcat(command, ")") ;
            kill = connectDB(command) ;

            if (kill != 0)
                return 1 ;

            kill = writeSqlFile(sqlFile, command);
            if (kill < 0)
                return 1;
        }
    }
    return 0 ;
}

/*
Function : enterPath
-------------------
Allows the user to enter the file he wants to use

char *fileName : string storing the file path
*/
void enterPath (char *fileName) {
     printf("Enter XML file path : ") ;
     fgets(fileName, 100, stdin) ;
     if (fileName[strlen(fileName) - 1] == '\n')
          fileName[strlen(fileName) - 1] = '\0' ;
}
