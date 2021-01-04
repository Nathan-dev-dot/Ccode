/*
    Nathan Letourneau & Sarah Schlegel
    01.11.2020
    Project library of string modifiers
*/

#include "all.h"


/*
Function : trimWhiteSpace
--------------------------------------------------------------------------------
Trims the white spaces at the beginning and end of a string

--------------------------------------------------------------------------------
char *str : string to be trimmed
--------------------------------------------------------------------------------

*/
void trimWhiteSpace (char *str) {
    uint8_t start ;
    uint8_t end ;
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
Function : addSpace
--------------------------------------------------------------------------------
Concatenates a space at the end of a string

--------------------------------------------------------------------------------
char *str : string to be modified
--------------------------------------------------------------------------------

*/
void addSpace (char *str) {
    strcat(str, " ") ;
}

/*
Function : removeLastChar
--------------------------------------------------------------------------------
Removes the last character of a string

--------------------------------------------------------------------------------
char *str : string to be modified
--------------------------------------------------------------------------------

*/
void removeLastChar (char *str) {
    if (strlen(str) != 0)
        str[strlen(str) - 1] = '\0' ;
}



/*
Function : getErrorsFromConf
--------------------------------------------------------------------------------
Called from gtk.c/(printMessage)
Reads the conf file to find the messages associated to error codes

--------------------------------------------------------------------------------
uint8_t errNo : error code
GtkWidget *label : GtkLabel in which the message is displayed
--------------------------------------------------------------------------------
Return values
    1 if ok
    0 if the error code wasn't found
*/
uint8_t getErrorsFromConf (uint8_t errNo, GtkWidget *label) {
    FILE *confFile = fopen("../config", "r") ;
    char str[150] ;
    size_t errCode ;

    if (confFile == NULL){
        gtk_label_set_text(GTK_LABEL(label), (const gchar *)"Error in opening the configuration file") ;
    }

    while (fgets(str, 150, confFile) != NULL) {
        if (strcmp(str, "[error_codes]\n") == 0) {
            while (fgets(str, 150, confFile) != NULL && strcmp(str, "\n") != 0) {
                sscanf(str, "%zd", &errCode) ;
                if (errCode == errNo) {
                    gtk_label_set_text(GTK_LABEL(label), (const gchar *)strchr(str, ':') + 1) ;
                    return 1 ;
                }
            }
        }
    }
    fclose(confFile) ;
    return 0 ;
}



/*----------------------------------------------------------------------------*/
/*-------------------Manage a database----------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
Function : addColumnCommand
--------------------------------------------------------------------------------
Called from gtk.c/(addTable)
Writes the command to add a column to a table

--------------------------------------------------------------------------------
GtkWidget *widget : widget used by (retrieveData)
char *command : SQL command
GtkColumn colInput : struct of inputs containing the new column data
--------------------------------------------------------------------------------
Return values
    1 if the column is set as a primary key (requested from (addPrimaryKeyCommand))
    0 otherwise
*/
uint8_t addColumnCommand(GtkWidget *widget, char *command, GtkColumn colInputs, char *pk) {
    char *cName ;
    char *type ;
    char sizeChar[5] ;
    int16_t size ;

    retrieveData(widget, colInputs.name, &cName) ;
    addSpace(strcat(command, cName)) ;

    retrieveComboBoxContent(widget, colInputs.type, &type) ;
    if (type == NULL)
        return ERR_XML ;
    addSpace(strcat(command, type)) ;

    if (strcmp(type, "CHAR") == 0 || strcmp(type, "VARCHAR") == 0) {
        retrieveInteger(widget, colInputs.size, &size) ;
        if (size <= 0) {
            return ERR_XML ;
        }
        sprintf(sizeChar, "%d", size) ;
        removeLastChar(command) ;
        strcat(strcat(strcat(command, "("), sizeChar), ") ") ;
    }

    addNotMandatoryCommand(widget, command, colInputs, type) ;
    addForeignKeyCommand(widget, command, colInputs.ref) ;

    if (command[strlen(command) - 1] == ' ')
        removeLastChar(command) ;
    strcat(command, ",") ;

    return addPrimaryKeyCommand(widget, pk, colInputs.primKey, cName) ;
}

/*
Function : addNotMandatoryCommand
--------------------------------------------------------------------------------
Called from (addColumnCommand)
Adds the not mandatory parameters to the command

--------------------------------------------------------------------------------
GtkWidget *widget : widget used by (retrieveData)
char *command : SQL command
GtkColumn colInput : struct of inputs containing the new column data
char *type : type of the column
--------------------------------------------------------------------------------
*/
void addNotMandatoryCommand(GtkWidget *widget, char *command, GtkColumn colInputs, char *type) {
    char *prop = "" ;

    retrieveData(widget, colInputs.constraints, &prop) ;
    if (strlen(prop) != 0 && strcmp(prop, "auto_increment") != 0 && strcmp(prop, "AUTO_INCREMENT") != 0) {
        addSpace(strcat(command, prop)) ;
    }

    retrieveData(widget, colInputs.check, &prop) ;
    if (strlen(prop) != 0) {
        addSpace(strcat(strcat(strcat(command, "CHECK ("), prop), ")")) ;
    }

    retrieveData(widget, colInputs.def, &prop) ;
    if (strlen(prop) != 0) {
        if ((strcmp(type, "CHAR") == 0 || strcmp(type, "VARCHAR") == 0) && strcmp(prop, "NULL") != 0 && strcmp(prop, "null") != 0) {
            addSpace(strcat(strcat(strcat(command, "DEFAULT \""), prop), "\"")) ;
        } else {
            addSpace(strcat(strcat(command, "DEFAULT "), prop)) ;
        }
    }
}

/*
Function : addPrimaryKeyCommand
--------------------------------------------------------------------------------
called form (addColumnCommand) to get the Primary Key
--------------------------------------------------------------------------------
GtkWidget *widget : widget sent by callback function
char *pk:array that contain the primary key colum name
GtkWidget *primKeyInput: the combobox containing the primary key setting
char *name:
--------------------------------------------------------------------------------
Return values
    1 if there is one or mulriple primary primKeys
    0 if no primary key to concatenate

*/
uint8_t addPrimaryKeyCommand (GtkWidget *widget, char *pk, GtkWidget *primKeyInput, char *name) {
    char *yn = "" ;
    retrieveComboBoxContent(widget, primKeyInput, &yn) ;

    if (yn != NULL && strcmp(yn, "YES") == 0) {
        strcat(strcat(pk, name), ",") ;
        return 1 ;
    }
    return 0 ;
}

/*
Function : addForeignKeyCommand
--------------------------------------------------------------------------------
Called form (addColumnCommand)
Concats the foreign key constaint at the end of the command

--------------------------------------------------------------------------------
GtkWidget *widget : widget sent by callback function
char *command: the SQL command
GtkWidget *input: input of referenced table
--------------------------------------------------------------------------------
Return values

*/
void addForeignKeyCommand (GtkWidget *widget, char *command, GtkWidget *input) {
    char *prop ;

    retrieveData(widget, input, &prop) ;
    if (strlen(prop) != 0) {
        strcat(strcat(command, "REFERENCES "), prop) ;
    }
}

/*
Function : retrieveAI
--------------------------------------------------------------------------------
Called from gtk.c/(addColumns)
Retrives the current auto_increment constraint

--------------------------------------------------------------------------------
GtkWidget *widget : widget sent by callback function
GtkColumn col : struct containing all the inputs for add column
char *aiName : name of auto_increment column
--------------------------------------------------------------------------------

*/
void retrieveAI (GtkWidget *widget, GtkColumn col, char *aiName) {
    char *tmp ;
    retrieveData(widget, col.constraints, &tmp) ;
    if (strcmp(tmp, "auto_increment") == 0 || strcmp(tmp, "AUTO_INCREMENT") == 0) {
        retrieveData(widget, col.name, &tmp) ;
        strcpy(aiName, tmp) ;
    }

}
