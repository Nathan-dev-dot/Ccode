#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>
#include <mysql/mysql.h>

int initDB (FILE *sqlFile, char *name) ;
int connectDB(char *command) ;
int parseDoc(void);
void enterPath (char *fileName) ;
void trimWhiteSpace (char *str) ;
void replaceWhiteSpace (char *str) ;
int parseNodes (xmlNodePtr node, FILE *sqlFile);
int writeSQLColumn (xmlNodePtr node, char *colConf[10][2][20], char *command, char *primKeys[][20], char *foreignKeys[15][3][20]) ;
int catMandatory (xmlNodePtr n, char *colConf[10][2][20], int i, char *command) ;
int catNotMandatory (xmlNodePtr n, char *colConf[10][2][20], int i, char *command, char *primKeys[][20]) ;
void addPrimaryKey (char *table[][20], char *colName) ;
void catPrimaryKeys (char *table[][20], char *command) ;
int getForeignKeys (char *table[15][3][20], xmlNodePtr n) ;
int catForeignKeys(char *table[15][3][20], xmlNodePtr n, char *command) ;
int initConf (char *conf[10][2][20]) ;
void addSpace (char *str) ;
void removeLastChar (char *str) ;
int writeSqlFile(FILE *sqlFile, char *command);

FILE *confFile ;
char nameDB[30] ;


int main(int argc, char **argv){
    int kill ;
    kill = parseDoc();
    if (kill != 0) {
        return 1 ;
    }
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
int parseDoc(void){
    int kill ;
    FILE *sqlFile ;
    char xmlFile[100];
    xmlDocPtr doc;
    xmlNodePtr root;

    confFile = fopen("config", "r") ;
    if (confFile == NULL) {
        fprintf(stderr, "Config file not found\n") ;
        return EXIT_FAILURE ;
    }

    sqlFile = fopen("db.sql", "w") ;
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
    if(mysql_real_connect(&mysql, "localhost", "root", "", NULL, 3306, NULL, 0) == NULL){
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


    if(mysql_real_connect(&mysql, "localhost", "root", "", nameDB, 3306, NULL, 0) == NULL){
        printf("\nConnection to mysql database failed\n");
        mysql_close(&mysql);
        return 1 ;
    }
    mysql_query(&mysql,command);
    mysql_close(&mysql);
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
Function : trimWhiteSpace
-------------------------
Trims the white spaces at the beginning and end of a string

char *str : string to be trimmed
*/
void trimWhiteSpace (char *str) {
    int start ;
    int end ;
    char *tmp = malloc(sizeof(char) * (strlen(str) + 1)) ;
    strcpy(tmp, str) ;

    start = 0 ;
    while (tmp[start] == ' ')
        start++ ;

    end = strlen(tmp) - 1 ;
    while (tmp[end] == ' ')
        end-- ;

    strncpy(str, tmp + start, end - start + 1) ;
    str[end - start + 1] = '\0' ;
    free(tmp) ;
}

/*
Function : writeSqlFile
-------------------
Writes a sql command in the sql file

FILE *sqlFile : sqlFile stream
char *command : command to be written
*/
int writeSqlFile(FILE *sqlFile, char *command){
    strcat(command,";\n");
    return fprintf(sqlFile, "%s",command);
}

/*
Function : addSpace
-------------------
Concatenates a space at the end of a string

char *str : string to be modified
*/
void addSpace (char *str) {
    strcat(str, " ") ;
}

/*
Function : removeLastChar
-------------------
Removes the last character of a string

char *str : string to be modified
*/
void removeLastChar (char *str) {
    str[strlen(str) - 1] = '\0' ;
}
