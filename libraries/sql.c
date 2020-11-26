/*
    Nathan Letourneau & Sarah Schlegel
    01.11.2020
    Library with SQL writing functions
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
Function : writeSQLColumn
-------------------
Concatenates the column creation command for each column node in the table
Calls for catForeignKeys to put the foreign key at the end of the column command

xmlNodePtr node : first column node in the table
char *colConf[10][2][20] : array with the column configuration (mandatory and not mandatory)
char *command : SQL command
char *primKeys[][20] : array with the primary keys of the table
char *foreignKeys[15][3][20] : array with the foreign keys of the database

returns :
0 if ok
1 if something went wrong
*/
int writeSQLColumn (xmlNodePtr node, char *colConf[10][2][20], char *command, char *primKeys[][20], char *foreignKeys[15][3][20]) {
    int kill ;
    int i ;
    xmlNodePtr n ;
    char str[30] ;
    char name[30] ;

    for (n = node ; n != NULL ; n = n->next) {
        i = 0 ;
        if (n->type == XML_ELEMENT_NODE) {
            if (xmlNodeGetContent(n) == NULL)
                return 1 ;
            strcpy(name, (const char *)xmlNodeGetContent(n)) ;
            trimWhiteSpace(name) ;
            addSpace(strcat(command, name)) ;
            while (strcmp((const char *)colConf[i][0], "STOP") != 0) {
                if (strcmp((const char *)colConf[i][0], "m") == 0) {
                    kill = catMandatory(n, colConf, i, command) ;
                    i++ ;
                    if (kill != 0)
                        return 1 ;
                }
                if (strcmp((const char *)colConf[i][0], "n") == 0) {
                    kill = catNotMandatory(n, colConf, i, command, primKeys) ;
                    if (kill != 0)
                        return 1 ;
                }
                i++ ;
            }
            catForeignKeys(foreignKeys, n, command) ;
            addSpace(strcat(command, ",")) ;

        }
    }
    return 0 ;
}

/*
Function : catMandatory
-------------------
Concatenates the mandatory props for the column

xmlNodePtr node : column node
char *colConf[10][2][20] : array with the column configuration (mandatory and not mandatory)
int i : given line in colConf
char *command : SQL command

returns :
0 if ok
1 if something went wrong
*/
int catMandatory (xmlNodePtr n, char *colConf[10][2][20], int i, char *command) {
    char prop[30] ;
    if (xmlGetProp(n, (const xmlChar *)colConf[i][1]) == NULL){
        return 1 ;}
    strcpy(prop, (const char *)xmlGetProp(n, (const xmlChar *)colConf[i][1])) ;
    strcat(command, prop) ;
    if (!strcmp((const char *)prop, "varchar") || !strcmp((const char *)prop, "char")) {
        if (colConf[i+1][1] == NULL){
            return 1 ;}
        strcat(strcat(strcat(command, "("), (const char *)xmlGetProp(n, (const xmlChar *)colConf[i+1][1])), ")") ;
    }
    addSpace(command) ;
    return 0 ;
}

/*
Function : catNotMandatory
-------------------
Concatenates the not mandatory props for the column
Calls to addPrimaryKey to add the primary keys in the primKeys array

xmlNodePtr node : column node
char *colConf[10][2][20] : array with the column configuration (mandatory and not mandatory)
int i : given line in colConf
char *command : SQL command
char *primKeys[][20] : array of the primary key of the table

returns :
0 if ok
1 if something went wrong
*/
int catNotMandatory (xmlNodePtr n, char *colConf[10][2][20], int i, char *command, char *primKeys[][20]) {
    char prop[30] ;
    if (xmlGetProp(n, (const xmlChar *)colConf[i][1]) != NULL) {
        strcpy(prop, (const char *)xmlGetProp(n, (const xmlChar *)colConf[i][1])) ;
        if (strcmp((const char *)colConf[i][1], "default") == 0) {
            strcat(command, "default ") ;
            if ((!strcmp((const char *)xmlGetProp(n, (const xmlChar *)"type"), "varchar") ||
                    !strcmp((const char *)xmlGetProp(n, (const xmlChar *)"type"), "char")) && strcmp(prop, "null")) {
                addSpace(strcat(strcat(strcat(command, "\""), prop), "\"")) ;
            } else {
                addSpace(strcat(command, prop)) ;
            }
        } else if (strcmp((const char *)colConf[i][1], "attribute") == 0 && strcmp((const char *)xmlGetProp(n, (const xmlChar *)"attribute"), "primary key") == 0) {
            addPrimaryKey(primKeys, (char *)xmlNodeGetContent(n)) ;
        } else {
            addSpace(strcat(command, prop)) ;
        }
    }
    return 0 ;
}

/*
Function : addPrimaryKey
------------------------
Adds a primary key (column name) to the array of primary keys
The primary keys will be written at the end of the table command in case there are multiple ones

char *table[][20] : array of primary keys to be modified
char *colName : column name
*/
void addPrimaryKey (char *table[][20], char *colName) {
    int i = 0 ;
    while (strcmp((const char *)table[i], "STOP") != 0) {
        i++ ;
    }
    strcpy((char *)table[i], colName) ;
}

/*
Function : catPrimaryKeys
-------------------
Concatenates the primary keys at the end of the table creation command

char *table[][20] : array storing the primary keys of the table
char *command : SQL command
*/
void catPrimaryKeys (char *table[][20], char *command) {
    int i = 0 ;
    strcat(command, "primary key(") ;
    while (strcmp((const char *)table[i], "STOP") != 0) {
        strcat(strcat(command, (const char *)table[i]), ",") ;
        i++ ;
    }
    removeLastChar(command) ;
    strcat(command, ")") ;
}

/*
Function : getForeignKeys
-------------------
Stores all the foreign keys in an array to be used later
(Storing format : table |column | type&size)

char *table[15][3][20] : foreign keys array
xmlNodePtr parent : table node

returns :
0 if ok
1 in case of an error
*/
int getForeignKeys (char *table[15][3][20], xmlNodePtr parent) {
    int count = 0 ;
    xmlNodePtr n ;
    while (strcmp((const char *)table[count][0], "STOP") != 0)
        count++ ;
    for (n = parent->children ; n != NULL ; n = n->next) {
        if (xmlGetProp(n, (const xmlChar *)"reference") != NULL) {
            if (strcmp((const char *)xmlGetProp(n, (const xmlChar *)"reference"), "target") == 0) {
                strcpy((char *)table[count][0], (const char *)xmlGetProp(parent, (const xmlChar *)"tname")) ;
                if (xmlNodeGetContent(n) == NULL)
                    return 1 ;
                strcpy((char *)table[count][1], (const char *)xmlNodeGetContent(n)) ;
                if (xmlGetProp(n, (const xmlChar *)"type") == NULL)
                    return 1 ;
                strcpy((char *)table[count][2], (const char *)xmlGetProp(n, (const xmlChar *)"type")) ;
                if (strcmp((const char *)table[count][2], "varchar") == 0 || strcmp((const char *)table[count][2], "char") == 0) {
                    if (xmlGetProp(n, (const xmlChar *)"size") == NULL)
                        return 1 ;
                    strcat((char *)table[count][2], (const char *)xmlGetProp(n, (const xmlChar *)"size")) ;
                }
            }
        }
    }
    return 0 ;
}

/*
Function : catForeignKeys
-------------------
Concatenates the foreign key at the end of the column creation command

char *table[15][3][20] : foreign keys array
xmlNodePtr n : column node
char *command : SQL command

returns :
0 if ok
1 in case of an error
*/
int catForeignKeys(char *table[15][3][20], xmlNodePtr n, char *command) {
    int count = 0 ;
    char tmp[30] = "" ;
    char tab[30] = "" ;
    char col[30] = "" ;
    char type[30] = "" ;
    while (strcmp((const char *)table[count][0], "STOP") != 0){
        strcpy(tab, "") ;
        strcpy(col, "") ;
        strcpy(type, "") ;
        strcpy(tmp, "") ;
        if (xmlGetProp(n, (const xmlChar *)"reference") != NULL && strcmp((const char *)xmlGetProp(n, (const xmlChar *)"reference"), "target") != 0) {
            strcpy(tmp, (const char *)xmlGetProp(n, (const xmlChar *)"reference")) ;
            strncat(tab, tmp, strchr(tmp, '(') - tmp) ;
            strncat(col, strchr(tmp, '(') + 1, strchr(tmp, ')') - strchr(tmp, '(') - 1) ;
            if (xmlGetProp(n, (const xmlChar *)"type") == NULL)
                return 1 ;
            strcpy(type, (const char *)xmlGetProp(n, (const xmlChar *)"type")) ;
            if (strcmp(type, "varchar") == 0 || strcmp(type, "char") == 0) {
                if (xmlGetProp(n, (const xmlChar *)"size") == NULL)
                    return 1 ;
                strcat(type, (const char *)xmlGetProp(n, (const xmlChar *)"size")) ;
            }

            if (strcmp(tab, (const char *)table[count][0]) == 0 && strcmp(col, (const char *)table[count][1]) == 0 && strcmp(type, (const char *)table[count][2]) == 0) {
                strcat(strcat(command, "references "), tmp) ;
                return 0 ;
            }
        }
        count++ ;
    }

    return 1 ;
}

/*
Function : writeSqlFile
-------------------
Writes a sql command in the sql file

char *command : command to be written
int first : first time calling the function
*/
int writeSqlFile(char *command, int first) {
    FILE *sqlFile ;
    char filePath[30] = "../" ;
    int print ;

    strcat(strcat(filePath, nameDB), ".sql") ;

    if (first == 0) {
        sqlFile = fopen(filePath, "w+") ;
        first = 1 ;
    } else {
        sqlFile = fopen(filePath, "a+") ;
    }
    if (sqlFile == NULL) {
        fprintf(stderr, "Error in creating the sql file\n") ;
        return EXIT_FAILURE ;
    }

    fseek(sqlFile, 0, SEEK_END) ;

    strcat(command,";\n");
    print = fprintf(sqlFile, "%s",command);
    fclose(sqlFile) ;
    return print ;
}
