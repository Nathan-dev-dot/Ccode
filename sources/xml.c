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
#include "errCodes.h"

extern char nameDB[30] ;

/*
Function : dbFromXML
-------------------
Launches the program that will create a database from an XML file
*/
void dbFromXML (void) {
    int kill ;
    kill = parseDoc();
    if (kill != 0)
        printError (kill) ;
    else
        printf("Database created\n") ;
}

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
    FILE *confFile = fopen("../config", "r") ;

    if (confFile == NULL) {
        fprintf(stderr, "Config file not found\n") ;
        return ERR_CONF ;
    }

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

    fclose(confFile) ;
    return 0 ;
}

/*
Function : parseDoc
-------------------
Opens the required files for the program (configuration, database and xml)
Initialises the database then calls writeSQLTables

returns :
0 if ok
1 if something went wrong
*/
int parseDoc(void) {
    int kill ;
    char xmlFile[100];
    xmlDocPtr doc;
    xmlNodePtr root;

    enterPath(xmlFile) ;
    doc = xmlParseFile(xmlFile) ;

    if (doc == NULL) {
         fprintf(stderr, "Invalid XML document\n") ;
         return EXIT_FAILURE ;
    }

    root = xmlDocGetRootElement(doc);

    if ((kill = initDB((char *)xmlGetProp(root, (const xmlChar *)"dbname"))) != 0)
        return kill ;

    if ((kill = writeSQLTables(root->children)) != 0){
        dropDB() ;
        dropSQLFile() ;
        return kill ;
    }

    xmlFreeDoc(doc);
    return 0 ;
}


/*
Function : writeSQLTables
-------------------
Parses the XML file through the table nodes
Sets the variables used for parsing the tables
Writes a full table creation command

xmlNodePtr node : first table node

returns :
0 if ok
1 if something went wrong
*/
int writeSQLTables (xmlNodePtr node) {
    int kill ;
    int i ;
    int j ;
    xmlNodePtr n ;
    char *colConf[10][2][20] = { "" } ;
    char *primKeys[30][20] ;
    ForeignKey *foreignKeys = NULL ;
    int size ;
    char command[500] ;

    if ((kill = initConf(colConf)) != 0)
        return kill ;

    if ((size = countForeignKeys(node)) == 0)
        return ERR_XML ;
    if ((foreignKeys = getForeignKeys(node, size)) == NULL)
        return ERR_MEM ;

    for (n = node ; n != NULL ; n = n->next) {
        strcpy(command, "") ;
        if (n->type == XML_ELEMENT_NODE && strcmp((const char *)n->name, "table") == 0){
            for (j = 0 ; j < 10 ; ++j)
                strcpy((char *)primKeys[j], "STOP") ;
            if (xmlGetProp(n, (const xmlChar *)"tname") == NULL)
                return ERR_XML ;
            strcat(strcat(strcat(command, "CREATE TABLE "), (const char *)xmlGetProp(n, (const xmlChar *)"tname")), "(") ;

            if ((kill = writeSQLColumn(n->children, colConf, command, primKeys, foreignKeys)) != 0)
                return kill ;

            if ((kill = catPrimaryKeys(primKeys, command)) != 0)
                return kill ;
            strcat(command, ")") ;

            if ((kill = connectDB(command)) != 0)
                return kill ;

            if ((kill = writeSQLFile(command, 1)) < 0)
                return kill ;
        }
    }
    if (foreignKeys != NULL)
        free(foreignKeys) ;
    return 0 ;
}


/*
Function : countForeignKeys
-------------------
Counts the number of foreign keys in the XML file

xmlNodePtr start : first parent node counting from

returns : the number of foreign keys
*/
int countForeignKeys (xmlNodePtr start) {
    int count = 0 ;
    xmlNodePtr nColumn ;
    xmlNodePtr nTable ;

    for (nTable = start ; nTable != NULL ; nTable = nTable->next) {
        for (nColumn = nTable->children ; nColumn != NULL ; nColumn = nColumn->next) {
            if (nColumn->type == XML_ELEMENT_NODE && strcmp((const char *)nColumn->name, "column") == 0) {
                if (xmlGetProp(nColumn, (const xmlChar *)"reference") != NULL && strcmp((const char *)xmlGetProp(nColumn, (const xmlChar *)"reference"), "target") == 0) {
                    count++ ;
                }
            }
        }
    }
    return count ;
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
