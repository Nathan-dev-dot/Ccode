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

xmlNodePtr start : first table node counting from

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
*/
void createXMLFile (GtkWidget *widget, XMLdbParams *dbParams) {
    int kill ;
    kill = createDoc(widget, dbParams) ;
}

/*
*/
int createDoc (GtkWidget *widget, XMLdbParams *dbParams) {
    Conf *colConf ;
    xmlDocPtr doc;
    xmlNodePtr root ;
    char path[50] = "" ;
    int kill ;
    int nbTables = 0 ;
    int i ;

    if ((kill = setXMLDatabase(widget, dbParams->name, (char *)path)) != 0)
        return kill ;
    if ((kill = retrieveInteger(widget, dbParams->nb, &nbTables)) != 0)
        return kill ;

    kill = duplicateTemplate(path) ;
    if (kill != 0)
        return kill ;

    doc = xmlParseFile(path) ;
    if (doc == NULL) {
         fprintf(stderr, "Invalid XML document\n") ;
         return EXIT_FAILURE ;
    }

    root = xmlDocGetRootElement(doc);
    kill = setRoot(root) ;
    if (kill != 0)
        return kill ;

    if ((colConf = initConf()) == NULL){
        xmlFreeDoc(doc) ;
        return ERR_CONF ;
    }

    closeWindow(dbParams->window) ;

    for (i = 0 ; i < nbTables ; ++i) {
        tableData(widget) ;
        printf("No\n") ;
    //     kill = addTableNode(root, colConf) ;
    }
    xmlFreeDoc(doc) ;
    return 0 ;
    kill = writeXMLFile(path, doc) ;
    if (kill == -1) {
        xmlFreeDoc(doc) ;
        return ERR_CREA ;
    }
    xmlFreeDoc(doc) ;
    return 0 ;
}

/*
*/
int setXMLDatabase (GtkWidget *widget, GtkWidget *input, char *path) {
    char *tmp ;
    retrieveData(widget, input, &tmp) ;

    if (strlen(tmp) == 0)
        return ERR_ENTRY ;

    strcpy(nameDB, tmp) ;
    strcat(strcat(strcpy(path, "../outputs/"), nameDB), ".xml") ;

    return 0 ;
}

void setTableData (GtkWidget *widget, XMLdbParams *tableParams) {
    int nbCol = 0 ;
    char *tName ;

    retrieveInteger(widget, tableParams->nb, &nbCol) ;
    retrieveData(widget, tableParams->name, &tName) ;
    closeWindow(tableParams->window) ;
    getTableColumns(nbCol, tName) ;
}

/*
*/
int retrieveInteger (GtkWidget *widget, GtkWidget *input, int *integer) {
    char *tmp = "" ;
    retrieveData(widget, input, &tmp) ;

    *integer = atoi(tmp) ;
    if (*integer <= 0) {
        return ERR_ENTRY ;
    }

    return 0 ;
}

/*
*/
int duplicateTemplate (char *fileName) {
    FILE *origin ;
    FILE *duplicate ;
    char str[50] = "" ;

    origin = fopen("../ressources/template.xml", "r") ;
    if (origin == NULL) {
        printf("Erreur d'ouverture du fichier d'origine\n") ;
        return ERR_FILE ;
    }
    duplicate = fopen(fileName, "w+") ;
    if (origin == NULL)
        return ERR_FILE ;

    while (fgets(str, 50, origin) != NULL) {
        fprintf(duplicate, "%s", str) ;
    }

    fclose(origin) ;
    fclose(duplicate) ;
    return 0 ;
}

/*
*/
int setRoot (xmlNodePtr root) {
    xmlAttrPtr attr = xmlSetProp(root, (const xmlChar *)"dbname", (const xmlChar *)nameDB) ;
    return attr != NULL ? 0 : ERR_CREA ;
}

/*
*/
int addTableNode (xmlNodePtr root, Conf *conf) {
    char tname[50] = "" ;
    xmlNodePtr newNode ;
    xmlAttrPtr attr ;
    int kill ;
    int nbCol = 0 ;
    int i ;

    while (strlen(tname) == 0) {
        printf("Enter table name : ") ;
        fgets(tname, 50, stdin) ;
        removeChariot(tname) ;
        trimWhiteSpace(tname) ;
    }

    newNode = xmlNewNode(NULL, (const xmlChar *)"table") ;
    if (newNode == NULL)
        return ERR_CREA ;

    attr = xmlSetProp(newNode, (const xmlChar *)"tname", (const xmlChar *)tname) ;
    if (attr == NULL)
        return ERR_CREA ;

    while (nbCol <= 0) {
        printf("Enter number of columns : ") ;
        scanf("%d", &nbCol) ;
        fflush(stdin) ;
    }
    for (i = 0 ; i < nbCol ; ++i) {
        kill = addColumnNode(newNode, conf) ;
    }

    xmlAddChild(root, newNode) ;
    return 0 ;
}

/*
*/
int addColumnNode (xmlNodePtr table, Conf *conf) {
    char cname[50] = "" ;
    xmlNodePtr newNode ;
    int kill ;
    int i = 0 ;
    int primKeys = 0 ;

    while (strlen(cname) == 0) {
        printf("Enter column name : ") ;
        fgets(cname, 50, stdin) ;
        removeChariot(cname) ;
        trimWhiteSpace(cname) ;
    }

    newNode = xmlNewNode(NULL, (const xmlChar *)"column") ;
    if (newNode == NULL)
        return ERR_CREA ;

    xmlNodeSetContent(newNode, (const xmlChar *)cname) ;
    while (strcmp(conf[i].prop, "STOP") != 0) {
        if (conf[i].mand == 1) {
            if ((kill = addMandatory(newNode, conf, i)) != 0)
                return kill ;
            i++ ;
        }
        if (conf[i].mand == 0) {
            addNotMandatory(newNode, conf, i) ;
        }
        i++ ;
    }
    kill = addPrimaryKey(newNode) ;
    if (kill == 0)
        return ERR_CREA ;
    else primKeys += kill ;

    kill = addForeignKey(newNode) ;
    if (kill == ERR_CREA)
        return kill ;

    xmlAddChild(table, newNode) ;
    return 0 ;
}

/*
*/
int addMandatory (xmlNodePtr col, Conf *conf, int i) {
    char prop[30] = "" ;
    xmlAttrPtr attr ;
    int size = -1 ;

    while (strlen(prop) == 0) {
        printf("Enter %s : ", conf[i].prop) ;
        fflush(stdin) ;
        fgets(prop, 30, stdin) ;
        removeChariot(prop) ;
        trimWhiteSpace(prop) ;
    }

    attr = xmlSetProp(col, (const xmlChar *)conf[i].prop, (const xmlChar *)prop) ;
    if (attr == NULL)
        return ERR_CREA ;

    if (strcmp(prop, "varchar") == 0 || strcmp(prop, "char") == 0) {
        strcpy(prop, "") ;
        while (size <= 0) {
            printf("Enter %s : ", conf[i+1].prop) ;
            scanf("%d", &size) ;
            fflush(stdin) ;
        }
        sprintf(prop, "%d", size) ;
        attr = xmlSetProp(col, (const xmlChar *)conf[i+1].prop, (const xmlChar *)prop) ;
        if (attr == NULL)
            return ERR_CREA ;
    }
    return 0 ;
}

/*
*/
int addNotMandatory (xmlNodePtr col, Conf *conf, int i) {
    char prop[30] ;
    xmlAttrPtr attr = NULL ;

    printf("Enter %s : ", conf[i].prop) ;
    fflush(stdin) ;
    fgets(prop, 30, stdin) ;
    removeChariot(prop) ;

    if (strlen(prop) != 0)
        attr = xmlSetProp(col, (const xmlChar *)conf[i].prop, (const xmlChar *)prop) ;
    if (attr == NULL)
        return ERR_CREA ;

    return 0 ;
}

/*
*/
int addPrimaryKey (xmlNodePtr col) {
    int yn = -1 ;
    xmlAttrPtr attr ;
    while (yn != 0 && yn != 1) {
        printf("Primary key ? (0 : no / 1 : yes) ") ;
        scanf("%d", &yn) ;
        fflush(stdin) ;
    }
    if (yn == 1) {
        attr = xmlSetProp(col, (const xmlChar *)"attribute", (const xmlChar *)"primary key") ;
        if (attr == NULL)
            return 0 ;
    }
    return 1 ;
}

/*
*/
int addForeignKey (xmlNodePtr col) {
    int yn = -1 ;
    char ref[50] = "" ;
    xmlAttrPtr attr ;

    while (yn != 0 && yn != 1) {
        printf("Referenced table ? (0 : no / 1 : yes) ") ;
        scanf("%d", &yn) ;
        fflush(stdin) ;
    }
    if (yn == 1) {
        attr = xmlSetProp(col, (const xmlChar *)"reference", (const xmlChar *)"target") ;
        if (attr == NULL)
            return ERR_CREA ;
    }

    yn = -1 ;
    while (yn != 0 && yn != 1) {
        printf("References another table ? (0 : no / 1 : yes) ") ;
        scanf("%d", &yn) ;
        fflush(stdin) ;
    }
    if (yn == 1) {
        printf("Referenced table name : ") ;
        fgets(ref, 50, stdin) ;
        removeChariot(ref) ;
        attr = xmlSetProp(col, (const xmlChar *)"reference", (const xmlChar *)ref) ;
        if (attr == NULL)
            return ERR_CREA ;
    }
    return 0 ;
}

/*
*/
int writeXMLFile (char *fileName, xmlDocPtr doc) {
    FILE *file = fopen(fileName, "w+") ;
    int success ;
    if (file == NULL)
        return ERR_FILE ;
    success = xmlDocDump(file, doc) ;
    return success ;
}
