/*
    Nathan Letourneau & Sarah Schlegel
    01.11.2020
    Project library with SQL writing functions
*/

#include "all.h"

extern char nameDB[30] ;

/*
Function : writeSQLColumn
--------------------------------------------------------------------------------
Called from xml.c/(writeSQLTables)
Concatenates the column creation command for each column node in the table
Calls (catForeignKeys) to put the foreign key at the end of the column command

--------------------------------------------------------------------------------
xmlNodePtr node : first column node in the table
Conf *colConf : array with the column configuration (mandatory and not mandatory)
char *command : SQL command
ForeignKey *foreignKeys : array with the foreign keys of the database
--------------------------------------------------------------------------------
Return values
    0 if ok
    an error code otherwise
*/
uint8_t writeSQLColumn (xmlNodePtr node, Conf *colConf, char *command, ForeignKey *foreignKeys) {
    uint8_t kill ;
    uint8_t i ;
    xmlNodePtr n ;
    char name[30] ;

    for (n = node ; n != NULL ; n = n->next) {
        i = 0 ;
        if (n->type == XML_ELEMENT_NODE) {
            if (xmlNodeGetContent(n) == NULL)
                return ERR_XML ;
            strcpy(name, (const char *)xmlNodeGetContent(n)) ;
            trimWhiteSpace(name) ;
            addSpace(strcat(command, name)) ;
            while (strcmp(colConf[i].prop, "STOP") != 0) {
                if (colConf[i].mand == 1) {
                    if ((kill = catMandatory(n, colConf, i, command)) != 0)
                        return kill ;
                    i++ ;
                }
                if (colConf[i].mand == 0) {
                    catNotMandatory(n, colConf, i, command) ;
                }
                i++ ;
            }
            kill = catForeignKeys(foreignKeys, n, command) ;
            trimWhiteSpace(command) ;
            addSpace(strcat(command, ",")) ;
        }
    }
    return 0 ;
}

/*
Function : catMandatory
--------------------------------------------------------------------------------
Concatenates the mandatory props for the column

--------------------------------------------------------------------------------
xmlNodePtr n : column node
Conf *colConf : array with the column configuration (mandatory and not mandatory)
uint8_t i : given line in colConf
char *command : SQL command
--------------------------------------------------------------------------------
Return values
    0 if ok
    1 if something went wrong
*/
uint8_t catMandatory (xmlNodePtr n, Conf *colConf, uint8_t i, char *command) {
    char prop[30] ;

    if (xmlGetProp(n, (const xmlChar *)colConf[i].prop) == NULL)
        return ERR_XML ;
    strcpy(prop, (const char *)xmlGetProp(n, (const xmlChar *)colConf[i].prop)) ;
    strcat(command, prop) ;

    if (!strcmp((const char *)prop, "varchar") || !strcmp((const char *)prop, "char")) {
        if (xmlGetProp(n, (const xmlChar *)colConf[i+1].prop) == NULL)
            return ERR_XML ;
        strcat(strcat(strcat(command, "("), (const char *)xmlGetProp(n, (const xmlChar *)colConf[i+1].prop)), ")") ;
    }
    addSpace(command) ;

    return 0 ;
}

/*
Function : catNotMandatory
--------------------------------------------------------------------------------
Concatenates the not mandatory props for the column

--------------------------------------------------------------------------------
xmlNodePtr node : column node
Conf *colConf : array with the column configuration (mandatory and not mandatory)
uint8_t i : given line in colConf
char *command : SQL command
--------------------------------------------------------------------------------

*/
void catNotMandatory (xmlNodePtr n, Conf *colConf, uint8_t i, char *command) {
    char prop[30] ;


    if (xmlGetProp(n, (const xmlChar *)colConf[i].prop) != NULL) {
        strcpy(prop, (const char *)xmlGetProp(n, (const xmlChar *)colConf[i].prop)) ;

        if (strcmp(colConf[i].prop, "default") == 0) {

            strcat(command, "default ") ;
            if ((!strcmp((const char *)xmlGetProp(n, (const xmlChar *)"type"), "varchar") ||
                    !strcmp((const char *)xmlGetProp(n, (const xmlChar *)"type"), "VARCHAR") ||
                    !strcmp((const char *)xmlGetProp(n, (const xmlChar *)"type"), "char") ||
                    !strcmp((const char *)xmlGetProp(n, (const xmlChar *)"type"), "CHAR")
                    ) && strcmp(prop, "null") && strcmp(prop, "NULL")) {
                addSpace(strcat(strcat(strcat(command, "\""), prop), "\"")) ;
            } else {
                addSpace(strcat(command, prop)) ;
            }
        } else if (strcmp(colConf[i].prop, "check") == 0 || strcmp(colConf[i].prop, "CHECK") == 0) {
            addSpace(strcat(strcat(strcat(command, "check ("), prop), ")")) ;
        } else {
            addSpace(strcat(command, prop)) ;
        }
    }
}

/*
Function : catPrimaryKeys
--------------------------------------------------------------------------------
Concatenates the primary key command at the end of a string

--------------------------------------------------------------------------------
xmlNodePtr parent : node of the table
char *command : SQL command
*/
uint8_t catPrimaryKeys (xmlNodePtr parent, char *command) {
    xmlNodePtr n ;
    uint8_t i = 0;

    strcat(command, "primary key(") ;
    for (n = parent->children ; n != NULL ; n = n->next) {
        if (n->type == XML_ELEMENT_NODE) {
            if (xmlNodeGetContent(n) != NULL && xmlGetProp(n, (const xmlChar *)"attribute") != NULL
                    && strcmp((const char *)xmlGetProp(n, (const xmlChar *)"attribute"), "primary key") == 0) {
                strcat(strcat(command, (const char *)xmlNodeGetContent(n)), ",") ;
                i++ ;
            }
        }
    }

    if (i == 0)
        return ERR_XML ;

    removeLastChar(command) ;
    strcat(command, ")") ;
    return 0 ;
}

/*
Function : getForeignKeys
--------------------------------------------------------------------------------
Called from xml.c/(writeSQLTables)
Allocates an array of foreign keys
(Storing format : table | column | type&size)

--------------------------------------------------------------------------------
ForeignKey *foreignKeys : foreign keys array
xmlNodePtr parent : table node
--------------------------------------------------------------------------------
Return values
    the pointer to the array of foreign keys
*/
ForeignKey * getForeignKeys (xmlNodePtr parent, uint8_t size) {
    uint8_t i = 0 ;
    ForeignKey *foreignKeys ;
    xmlNodePtr nColumn ;
    xmlNodePtr nTable ;

    foreignKeys = (ForeignKey *)malloc(sizeof(ForeignKey) * (size + 1)) ;
    if (foreignKeys == NULL)
        return NULL ;

    for (nTable = parent ; nTable != NULL ; nTable = nTable->next) {
        for (nColumn = nTable->children ; nColumn != NULL ; nColumn = nColumn->next) {
            if (nColumn->type == XML_ELEMENT_NODE && strcmp((const char *)nColumn->name, "column") == 0) {
                if (xmlGetProp(nColumn, (const xmlChar *)"reference") != NULL && strcmp((const char *)xmlGetProp(nColumn, (const xmlChar *)"reference"), "target") == 0) {
                    if (xmlGetProp(nTable, (const xmlChar *)"tname") == NULL || xmlNodeGetContent(nColumn) == NULL || xmlGetProp(nColumn, (const xmlChar *)"type") == NULL)
                        return foreignKeys ;

                    strcpy(foreignKeys[i].tableName, (const char *)xmlGetProp(nTable, (const xmlChar *)"tname")) ;
                    strcpy(foreignKeys[i].colName, (const char *)xmlNodeGetContent(nColumn)) ;
                    strcpy(foreignKeys[i].type, (const char *)xmlGetProp(nColumn, (const xmlChar *)"type")) ;
                    if (strcmp(foreignKeys[i].type, "varchar") == 0 || strcmp(foreignKeys[i].type, "char") == 0) {
                        if (xmlGetProp(nColumn, (const xmlChar *)"size") == NULL)
                            return foreignKeys ;
                        strcat(foreignKeys[i].type, (const char *)xmlGetProp(nColumn, (const xmlChar *)"size")) ;
                    }
                    i++ ;
                }
            }
        }
    }
    strcpy(foreignKeys[i].type, strcpy(foreignKeys[i].colName, strcpy(foreignKeys[i].tableName, "STOP"))) ;
    return foreignKeys ;
}

/*
Function : catForeignKeys
--------------------------------------------------------------------------------
Called from (writeSQLColumn)
Concatenates the foreign key constraint at the end of the column creation command

--------------------------------------------------------------------------------
ForeignKey *foreignKeys: pointer to the array of foreign keys
xmlNodePtr n : column node
char *command : SQL command
--------------------------------------------------------------------------------
Return values
    0 if ok
    1 in case of an error
*/
uint8_t catForeignKeys (ForeignKey *foreignKeys, xmlNodePtr n, char *command) {
    uint8_t i = 0 ;
    ForeignKey tmp ;
    char strTmp[30] = "" ;

    while (strcmp(foreignKeys[i].tableName, "STOP") != 0){
        strcpy(tmp.type, strcpy(tmp.colName, strcpy(tmp.tableName, ""))) ;
        strcpy(strTmp, "") ;

        if (xmlGetProp(n, (const xmlChar *)"reference") != NULL && strcmp((const char *)xmlGetProp(n, (const xmlChar *)"reference"), "target") != 0) {
            strcpy(strTmp, (const char *)xmlGetProp(n, (const xmlChar *)"reference")) ;
            strncat(tmp.tableName, strTmp, strchr(strTmp, '(') - strTmp) ;
            strncat(tmp.colName, strchr(strTmp, '(') + 1, strchr(strTmp, ')') - strchr(strTmp, '(') - 1) ;
            if (xmlGetProp(n, (const xmlChar *)"type") == NULL) {
                return ERR_XML ;
            }
            strcpy(tmp.type, (const char *)xmlGetProp(n, (const xmlChar *)"type")) ;
            if (strcmp(tmp.type, "varchar") == 0 || strcmp(tmp.type, "char") == 0) {
                if (xmlGetProp(n, (const xmlChar *)"size") == NULL) {
                    return ERR_XML;
                }
                strcat(tmp.type, (const char *)xmlGetProp(n, (const xmlChar *)"size")) ;
            }

            if (strcmp(tmp.tableName, foreignKeys[i].tableName) == 0 && strcmp(tmp.colName, foreignKeys[i].colName) == 0 && strcmp(tmp.type, foreignKeys[i].type) == 0) {
                strcat(strcat(command, "references "), strTmp) ;
                return 0 ;
            }
        }
        i++ ;
    }
    return 0 ;
}



/*
Function : writeSQLFile
--------------------------------------------------------------------------------
Called from xml.c/(writeSQLTables)
Writes a sql command in the sql file

--------------------------------------------------------------------------------
char *command : command to be written
uint8_t first : first time calling the function (if 0, creates the file and overwrites previous content)
--------------------------------------------------------------------------------
Return values
    0 if ok
    ERR_SQL if an error occured in the process
*/
uint8_t writeSQLFile (char *command, uint8_t first) {
    FILE *sqlFile ;
    char filePath[30] = "outputs/" ;
    int8_t kill ;

    strcat(strcat(filePath, nameDB), ".sql") ;

    if (first == 0) {
        sqlFile = fopen(filePath, "w+") ;
        first = 1 ;
    } else {
        sqlFile = fopen(filePath, "a+") ;
    }
    if (sqlFile == NULL) {
        printMessage(NULL, 0, "Error in creating the sql file") ;
        return ERR_SQL ;
    }

    fseek(sqlFile, 0, SEEK_END) ;

    strcat(command, ";\n");
    kill = fprintf(sqlFile, "%s", command);
    fclose(sqlFile) ;
    return kill < 0 ? ERR_SQL : 0 ;
}

/*
Function : dropSQLFile
--------------------------------------------------------------------------------
Called from xml.c/(parseDoc)
Drops an SQL file

--------------------------------------------------------------------------------

*/
void dropSQLFile (void) {
    char filePath[30] = "outputs/" ;

    strcat(strcat(filePath, nameDB), ".sql") ;
    remove(filePath) ;
}
