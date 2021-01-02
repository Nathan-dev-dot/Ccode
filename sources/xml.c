/*
    Nathan Letourneau & Sarah Schlegel
    01.11.2020
    Project library with XML functions
*/

#include "all.h"

extern char nameDB[30] ;

/*----------------------------------------------------------------------------*/
/*-------------------Create a database from an XML document-------------------*/
/*----------------------------------------------------------------------------*/

/*
Function : dbFromXML
--------------------------------------------------------------------------------
Launches the program that will create a database from an XML file
Calls
    gtk.c/(retrieveData) to get the XML file path
    (parseDoc) to parse the XML document

--------------------------------------------------------------------------------
GtkWidget *widget : widget sent by the callback function
GtkWidget *input : input from which is retrieved the data
--------------------------------------------------------------------------------

*/
void dbFromXML (GtkWidget *widget, GtkWidget *input) {
    uint8_t kill ;
    char *path = "" ;
    retrieveData(widget, input, &path) ;
    kill = parseDoc(path);
    if (kill != 0)
        printMessage(widget, kill, "") ;
    else
        printMessage(widget, 0, "Database created\nSQL creation file available in Outputs folder") ;
}

/*
Function : parseDoc
-------------------
Opens the required files for the program (configuration, database and xml)
Initialises the database
Calls (writeSQLTables) to write the tables one by one

--------------------------------------------------------------------------------
Return values
    0 if ok
    1 if something went wrong
*/
uint8_t parseDoc (char *path) {
    uint8_t kill ;
    xmlDocPtr doc;
    xmlNodePtr root;

    doc = xmlParseFile(path) ;

    if (doc == NULL) {
        return ERR_PATH ;
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
Function : countForeignKeys
--------------------------------------------------------------------------------
Counts the number of foreign keys in the XML file

--------------------------------------------------------------------------------
xmlNodePtr start : first table node counting from
--------------------------------------------------------------------------------
Return values
    the number of foreign keys
*/
uint8_t countForeignKeys (xmlNodePtr start) {
    uint8_t count = 0 ;
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
Function : writeSQLTables
--------------------------------------------------------------------------------
Parses the XML file through the table nodes
Sets the variables used for parsing the tables
Writes the complete command for creating a mysql table
Calls
    (countForeignKeys) & sql.c(getForeignKeys) to get the foreign keys of the table
    sql.c/(catPrimaryKeys) to concatenate the primary keys at the end of the command
    sql.c/(writeSQLColumn) to write the commands to each column
    db.c/(connectDB) to send the command to mysql
    sql.c/(writeSQLFile) to write the associated SQL file

--------------------------------------------------------------------------------
xmlNodePtr node : pointer to the first table node
--------------------------------------------------------------------------------
Return values
    0 if ok
    1 if something went wrong
*/
uint8_t writeSQLTables (xmlNodePtr node) {
    uint8_t kill ;
    xmlNodePtr n ;
    Conf *colConf = NULL ;
    ForeignKey *foreignKeys = NULL ;
    uint8_t size ;
    char command[500] ;

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

            if ((kill = connectDB(command)) != 0)
                return kill ;

            if ((kill = writeSQLFile(command, 1)) < 0)
                return kill ;
        }
    }
    if (foreignKeys != NULL)
        freePointer(NULL, foreignKeys) ;
    if (colConf != NULL)
        freePointer(NULL, colConf) ;
    return 0 ;
}

/*
Function : initConf
-------------------
Reads the config file of the columns and stores the data in an array of structs
Storing format : 1/0 | propName

--------------------------------------------------------------------------------
Return values
    the pointer of the array containing the configuration
*/
Conf * initConf (void) {
    Conf *colConf = NULL ;
    FILE *confFile = fopen("../config", "r") ;
    char mand ;
    char str[20] ;
    uint8_t lines = 0 ;
    uint8_t i ;

    if (confFile == NULL)
        return NULL ;

    fseek(confFile, 47, SEEK_SET) ;
    while (fgets(str, 50, confFile) != NULL && strcmp(str, "\n"))
        lines++ ;

    colConf = (Conf *)malloc(sizeof(Conf) * (lines + 1)) ;
    if (colConf == NULL) {
        printMessage(NULL, ERR_MEM, "") ;
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



/*----------------------------------------------------------------------------*/
/*-------------------Create an XML document from entries----------------------*/
/*----------------------------------------------------------------------------*/

/*
Function : createXMLDoc
-------------------
Creates the XML document and sets it's basic data (root, colFunc, etc.)
Calls gtk.c/(writeTables) to write the tables one by one

--------------------------------------------------------------------------------
GtkWidget *widget : widget sent by the callback function
GtkDualInputs *dbParams : structure of inputs defining the name and number of tables in the database
--------------------------------------------------------------------------------
Return values
    0 if all good
    ERR_PATH or ERR_CONF in cases of errors
*/
uint8_t createXMLDoc (GtkWidget *widget, GtkDualInputs *dbParams) {
    char path[50] = "" ;
    uint8_t kill ;
    int16_t nbTables = 0 ;
    XMLdbData xmlData ;
    GtkDualInputs di ;

    if ((kill = setXMLDatabase(widget, dbParams->name, (char *)path)) != 0)
        return kill ;
    if ((kill = retrieveInteger(widget, dbParams->nb, &nbTables)) != 0)
        return kill ;
    if (nbTables <= 0)
        return ERR_ENTRY ;

    kill = duplicateTemplate(path) ;
    if (kill != 0)
        return kill ;

    xmlData.doc = xmlParseFile(path) ;
    if (xmlData.doc == NULL)
        return ERR_PATH ;

    xmlData.root = xmlDocGetRootElement(xmlData.doc);
    kill = setRoot(xmlData.root) ;
    if (kill != 0)
        return kill ;

    if ((xmlData.conf = initConf()) == NULL){
        xmlFreeDoc(xmlData.doc) ;
        return ERR_CONF ;
    }

    xmlData.colFunc = &addTableNode ;
    xmlData.pos.total = nbTables ;
    xmlData.pos.current = 0 ;
    closeWindow(NULL, dbParams->window) ;
    di.window = NULL ;
    xmlData.dualInputs = &di ;
    writeTables(widget, &xmlData) ;
    return 0 ;
}

/*
Function : setXMLDatabase
-------------------
Calls gtk.c/(retrieveData) to get the database name
Sets it in the global nameDB variable

--------------------------------------------------------------------------------
GtkWidget *widget : widget sent by the callback function
GtkWidget *input : GtkEntry used to get the name of the database
char *path : path of the XML file
--------------------------------------------------------------------------------
Return values
    0 if all good
    ERR_ENTRY if couldn't get the input
*/
uint8_t setXMLDatabase (GtkWidget *widget, GtkWidget *input, char *path) {
    char *tmp ;
    retrieveData(widget, input, &tmp) ;

    if (strlen(tmp) == 0)
        return ERR_ENTRY ;

    strcpy(nameDB, tmp) ;
    strcat(strcat(strcpy(path, "../outputs/"), nameDB), ".xml") ;

    return 0 ;
}

/*
Function : duplicateTemplate
--------------------------------------------------------------------------------
Called from (createXMLDoc)
Duplicates an XML template

--------------------------------------------------------------------------------
char *fileName : the path of the duplicate file
--------------------------------------------------------------------------------
Return values
    0 if all good
    ERR_FILE if the files can't be opened
*/
uint8_t duplicateTemplate (char *fileName) {
    FILE *origin ;
    FILE *duplicate ;
    char str[50] = "" ;

    origin = fopen("../ressources/template.xml", "r") ;
    if (origin == NULL) {
        printMessage(NULL, 0, "Erreur d'ouverture du fichier d'origine") ;
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
Function : setRoot
--------------------------------------------------------------------------------
Called from (createXMLDoc)
Sets the root prop in the XML duplicated file

--------------------------------------------------------------------------------
xmlNodePtr root : pointer to the XML root
--------------------------------------------------------------------------------
Return values
    0 if ok
    ERR_CREA if the attribute couldn't be set
*/
uint8_t setRoot (xmlNodePtr root) {
    xmlAttrPtr attr = xmlSetProp(root, (const xmlChar *)"dbname", (const xmlChar *)nameDB) ;
    return attr != NULL ? 0 : ERR_CREA ;
}



/*
Function : setTableData
--------------------------------------------------------------------------------
Gets the table name and the number of columns of the table from GTK inputs
Sends the data to gtk.c/(getTableColumns)

--------------------------------------------------------------------------------
GtkWidget *widget : widget sent by GTK
XMLdbData *dbData : structure containing the setup of the database
*/
void setTableData (GtkWidget *widget, XMLdbData *dbData) {
    char *tmp ;
    if (retrieveInteger(widget, dbData->dualInputs->nb, &(dbData->size)) != 0)
        return ;
    if (dbData->size <= 0)
        return ;
    retrieveData(widget, dbData->dualInputs->name, &tmp) ;
    if (strlen(tmp) == 0)
        return ;
    strcpy(dbData->name, tmp) ;
    closeWindow(NULL, dbData->dualInputs->window) ;
    getTableColumns(widget, dbData) ;
}



/*
Function : addTableNode
--------------------------------------------------------------------------------
Called from (createXMLDoc)
Adds a full table node to the XML document
Calls
    (addColumnNode) to add the different child nodes to the table node
    (writeXMLFile) to write the data in the XML file

--------------------------------------------------------------------------------
GtkWidget *widget : widget needed in addColumnNode
XMLdbData *tableData : struct of database and table data
--------------------------------------------------------------------------------
Return values
    0 if ok
    ERR_CREA if the attribute couldn't be set
*/
uint8_t addTableNode (GtkWidget *widget, XMLdbData *tableData) {
    xmlNodePtr tNode ;
    xmlNodePtr child ;
    xmlAttrPtr attr ;
    uint8_t i = 0 ;
    uint8_t nbCol = tableData->size ;
    char path[50] = "" ;

    tNode = xmlNewNode(NULL, (const xmlChar *)"table") ;
    if (tNode == NULL)
        return ERR_CREA ;

    attr = xmlSetProp(tNode, (const xmlChar *)"tname", (const xmlChar *)tableData->name) ;
    if (attr == NULL)
        return ERR_CREA ;

    for (i = 0 ; i < nbCol ; ++i) {
        child = addColumnNode(widget, tableData->columns[i]) ;
        if (child == NULL)
            return ERR_CREA ;
        xmlAddChild(tNode, child) ;
    }

    freePointer(widget, tableData->columns) ;
    xmlAddChild(tableData->root, tNode) ;

    strcat(strcat(strcat(path, "../outputs/"), (const char *)xmlGetProp(tableData->root, (const xmlChar *)"dbname")), ".xml") ;
    writeXMLFile(path, tableData->doc) ;
    closeWindow(NULL, tableData->dualInputs->window) ;
    writeTables(widget, tableData) ;
    printf("All good\n") ;
    return 0 ;
}

/*
Function : addColumnNode
--------------------------------------------------------------------------------
Called from (addTableNode)
Adds a full column node to the XML document
Calls
    (addMandatory) to add the mandatory props
    (addNotMandatory) to add the not mandatory props
    gtk.c/(retrieveData) to retrieve the column name
    (addPrimaryKey) to add the different primary keys
    (addForeignKey) to add the different foreign keys

--------------------------------------------------------------------------------
GtkWidget *widget : widget needed in retrieveData functions
GtkColumn colInputs : struct of Gtk inputs used to retrieve the column data
--------------------------------------------------------------------------------
Return values
    the pointer to the created column node
*/
xmlNodePtr addColumnNode (GtkWidget *widget, GtkColumn colInputs) {
    printf("Alive\n") ;
    char *tmp = "" ;
    xmlNodePtr cNode = NULL ;
    int8_t kill ;
    uint8_t primKeys = 0 ;

    cNode = xmlNewNode(NULL, (const xmlChar *)"column") ;
    if (cNode == NULL)
        return NULL ;

    retrieveData(widget, colInputs.name, &tmp) ;
    xmlNodeSetContent(cNode, (const xmlChar *)tmp) ;

    if ((kill = addMandatory(widget, cNode, colInputs)) != 0)
        return NULL ;

    addNotMandatory(widget, cNode, colInputs) ;

    kill = addPrimaryKey(widget, cNode, colInputs.primKey) ;
    if (kill == -1)
        return NULL ;
    else primKeys += kill ;

    kill = addForeignKey(widget, cNode, colInputs) ;
    if (kill == ERR_CREA)
        return NULL ;
    printf("Debug 5\n") ;
    return cNode ;
}

/*
Function : addMandatory
--------------------------------------------------------------------------------
Called from (addColumnNode)
Adds the mandatory props to the node (type and eventuall size)

--------------------------------------------------------------------------------
GtkWidget *widget : widget needed to retrieve values from GtkEntries
xmlNodePtr col : pointer to the column node being set
GtkColumn colInputs : struct of Gtk inputs used to retrieve the column data
--------------------------------------------------------------------------------
Return values
    0 if ok
    ERR_CREA if the mandatory props couldn't be set
*/
uint8_t addMandatory (GtkWidget *widget, xmlNodePtr col, GtkColumn colInputs) {
    char *prop = "" ;
    int16_t size ;
    xmlAttrPtr attr ;

    retrieveComboBoxContent(widget, colInputs.type, &prop) ;
    if (prop == NULL)
        return ERR_XML ;
    if (strlen(prop) != 0) {
        attr = xmlSetProp(col, (const xmlChar *)"type", (const xmlChar *)prop) ;
        if (attr == NULL)
            return ERR_CREA ;
    }

    if (strcmp(prop, "VARCHAR") == 0 || strcmp(prop, "CHAR") == 0 || strcmp(prop, "varchar") == 0 || strcmp(prop, "char") == 0) {
        retrieveInteger(widget, colInputs.size, &size) ;
        if (size <= 0)
            return ERR_ENTRY ;
        sprintf(prop, "%d", size) ;
        attr = xmlSetProp(col, (const xmlChar *)"size", (const xmlChar *)prop) ;
        if (attr == NULL)
            return ERR_CREA ;
    }
    return 0 ;
}

/*
Function : addNotMandatory
--------------------------------------------------------------------------------
Called from (addColumnNode)
Adds the not mandatory props to the node (constraints, check, default values)

--------------------------------------------------------------------------------
GtkWidget *widget : widget needed to retrieve values from GtkEntries
xmlNodePtr col : pointer to the column node being set
GtkColumn colInputs : struct of Gtk inputs used to retrieve the column data
--------------------------------------------------------------------------------
Return values
    0 if ok
    ERR_CREA if the mandatory props couldn't be set
*/
uint8_t addNotMandatory (GtkWidget *widget, xmlNodePtr col, GtkColumn colInputs) {
    char *prop = "" ;
    xmlAttrPtr attr ;

    retrieveData(widget, colInputs.constraints, &prop) ;
    if (strlen(prop) != 0) {
        attr = xmlSetProp(col, (const xmlChar *)"constraints", (const xmlChar *)prop) ;
        if (attr == NULL)
            return ERR_CREA ;
    }

    retrieveData(widget, colInputs.check, &prop) ;
    if (strlen(prop) != 0) {
        attr = xmlSetProp(col, (const xmlChar *)"check", (const xmlChar *)prop) ;
        if (attr == NULL)
            return ERR_CREA ;
    }

    retrieveData(widget, colInputs.def, &prop) ;
    if (strlen(prop) != 0) {
        attr = xmlSetProp(col, (const xmlChar *)"default", (const xmlChar *)prop) ;
        if (attr == NULL)
        return ERR_CREA ;
    }

    return 0 ;
}

/*
Function : addPrimaryKey
--------------------------------------------------------------------------------
Called from (addColumnNode)
Adds the primary key prop to the node

--------------------------------------------------------------------------------
GtkWidget *widget : widget needed to retrieve values from GtkEntries
xmlNodePtr col : pointer to the column node being set
GtkWidget *primKeyInput : pointer to the GtkEntry of the primary key
--------------------------------------------------------------------------------
Return values
    1 if the attribute was set
    -1 if the attribute couldn't be set
    0 otherwise
*/
int8_t addPrimaryKey (GtkWidget *widget, xmlNodePtr col, GtkWidget *primKeyInput) {
    char *yn = "" ;
    xmlAttrPtr attr ;
    retrieveComboBoxContent(widget, primKeyInput, &yn) ;

    if (yn != NULL && strcmp(yn, "YES") == 0) {
        attr = xmlSetProp(col, (const xmlChar *)"attribute", (const xmlChar *)"primary key") ;
        if (attr == NULL)
            return -1 ;
        return 1 ;
    }

    return 0 ;
}

/*
Function : addForeignKey
--------------------------------------------------------------------------------
Called from (addColumnNode)
Adds the foreign key prop to the node

--------------------------------------------------------------------------------
GtkWidget *widget : widget needed to retrieve values from GtkEntries
xmlNodePtr col : pointer to the column node being set
GtkColumn colInputs : struct of Gtk inputs used to retrieve the col data
--------------------------------------------------------------------------------
Return values
    0 if ok
    ERR_CREA if the prop couldn't be set
*/
uint8_t addForeignKey (GtkWidget *widget, xmlNodePtr col, GtkColumn colInputs) {
    xmlAttrPtr attr ;
    char *prop ;

    retrieveData(widget, colInputs.ref, &prop) ;

    if (strlen(prop) != 0) {
        attr = xmlSetProp(col, (const xmlChar *)"reference", (const xmlChar *)prop) ;
        if (attr == NULL)
            return ERR_CREA ;
    }

    if (strlen(prop) == 0) {
        retrieveComboBoxContent(widget, colInputs.refd, &prop) ;
        if (prop != NULL && strcmp(prop, "YES") == 0) {
            attr = xmlSetProp(col, (const xmlChar *)"reference", (const xmlChar *)"target") ;
            if (attr == NULL)
                return ERR_CREA ;
        }
    }
    return 0 ;
}



/*
Function : retrieveInteger
--------------------------------------------------------------------------------
Retrieves an integer from a GtkEntry
Calls gtk.c/(retrieveData) to retrieve the entry as a string before converting it

--------------------------------------------------------------------------------
GtkWidget *widget : widget sent by callback function
GtkWidget *input : pointer to the Gtk input
int16_t *integer : integer to store the result
--------------------------------------------------------------------------------
Return values
    0 if all good
    ERR_ENTRY if the value is wrong
*/
uint8_t retrieveInteger (GtkWidget *widget, GtkWidget *input, int16_t *integer) {
    char *tmp = "" ;
    retrieveData(widget, input, &tmp) ;

    if (strlen(tmp) == 0) {
        *integer = 0 ;
        return 0 ;
    }
    trimWhiteSpace(tmp) ;

    *integer = atoi(tmp) ;
    if (*integer <= 0) {
        return ERR_ENTRY ;
    }

    return 0 ;
}



/*
Function : writeXMLFile
--------------------------------------------------------------------------------
Opens the file and dumps the XML tree into it

--------------------------------------------------------------------------------
char *fileName : path to the XML file
xmlDocPtr doc : pointer to the XML tree in the memory
--------------------------------------------------------------------------------
Return values
    0 if ok
    ERR_CREA if the prop couldn't be set
*/
uint8_t writeXMLFile (char *fileName, xmlDocPtr doc) {
    FILE *file = fopen(fileName, "w+") ;
    uint8_t success ;
    if (file == NULL)
        return ERR_FILE ;
    success = xmlDocDump(file, doc) ;
    return success ;
}
