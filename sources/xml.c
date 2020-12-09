/*
    Nathan Letourneau & Sarah Schlegel
    01.11.2020
    Project library with XML functions
*/

#include "all.h"

extern char nameDB[30] ;

/*
Function : dbFromXML
-------------------
Launches the program that will create a database from an XML file
*/
void dbFromXML (GtkWidget *widget, GtkWidget *input) {
    int kill ;
    char *path = "" ;
    retrieveData(widget, input, &path) ;
    printf("Path : %s\n", path) ;
    kill = parseDoc(path);
    if (kill != 0)
        printError (kill) ;
    else
        printf("Database created\n") ;
}

/*
Function : initConf
-------------------
Reads the config file of the columns and stores the data in an array of structs
(Storing format : 1/0 | propName)

returns :
The pointer of the array or NULL if the file wasn't loaded correctly
*/
Conf * initConf () {
    Conf *colConf = NULL ;
    FILE *confFile = fopen("../config", "r") ;
    char mand ;
    char str[20] ;
    int lines = 0 ;
    int i ;

    if (confFile == NULL)
        return NULL ;

    fseek(confFile, 47, SEEK_SET) ;
    while (fgets(str, 50, confFile) != NULL && strcmp(str, "\n"))
        lines++ ;

    colConf = (Conf *)malloc(sizeof(Conf) * (lines + 1)) ;
    if (colConf == NULL) {
        fprintf(stderr, "Not enough memory available\n") ;
        return NULL ;
    }

    fseek(confFile, 47, SEEK_SET) ;
    for (i = 0 ; i < lines ; ++i) {
        fscanf(confFile, "%c :%s\n", &mand, colConf[i].prop) ;
        colConf[i].mand = mand == 'm' ;
    }
    colConf[i].mand = 0 ;
    strcpy(colConf[i].prop, "STOP") ;

    fclose(confFile) ;
    return colConf ;
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
int parseDoc (char *path) {
    int kill ;
    char xmlFile[100];
    xmlDocPtr doc;
    xmlNodePtr root;

    doc = xmlParseFile(path) ;

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
    Conf *colConf = NULL ;
    ForeignKey *foreignKeys = NULL ;
    int size ;
    char command[500] ;
    MYSQL_RES res ;

    if ((colConf = initConf()) == NULL)
         return ERR_CONF ;

    if ((size = countForeignKeys(node)) == 0)
        return ERR_XML ;
    if ((foreignKeys = getForeignKeys(node, size)) == NULL)
        return ERR_MEM ;

    for (n = node ; n != NULL ; n = n->next) {
        strcpy(command, "") ;
        if (n->type == XML_ELEMENT_NODE && strcmp((const char *)n->name, "table") == 0){
            if (xmlGetProp(n, (const xmlChar *)"tname") == NULL)
                return ERR_XML ;
            strcat(strcat(strcat(command, "CREATE TABLE "), (const char *)xmlGetProp(n, (const xmlChar *)"tname")), "(") ;

            if ((kill = writeSQLColumn(n->children, colConf, command, foreignKeys)) != 0)
                return kill ;

            if ((kill = catPrimaryKeys(n, command)) != 0)
                return kill ;
            strcat(command, ")") ;

            if ((kill = connectDB(command, &res)) != 0)
                return kill ;

            if ((kill = writeSQLFile(command, 1)) < 0)
                return kill ;
        }
    }
    if (foreignKeys != NULL)
        free(foreignKeys) ;
    if (colConf != NULL)
        free(colConf) ;
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
