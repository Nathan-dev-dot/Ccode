/*
    Nathan Letourneau & Sarah Schlegel
    01.11.2020
    Project library of string modifiers
*/

#include "all.h"


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
    if (strlen(str) != 0)
        str[strlen(str) - 1] = '\0' ;
}

int addColumnCommand(GtkWidget *widget, char *command, GtkColumn colInputs, char *pk) {
    char *cName ;
    char *type ;
    char sizeChar[5] ;
    int size ;

    retrieveData(widget, colInputs.name, &cName) ;
    addSpace(strcat(command, cName)) ;

    retrieveComboBoxContent(widget, colInputs.type, &type) ;
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

void addNotMandatoryCommand(GtkWidget *widget, char *command, GtkColumn colInputs, char *type) {
    char *prop = "" ;

    retrieveData(widget, colInputs.constraints, &prop) ;
    if (strlen(prop) != 0) {
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

int addPrimaryKeyCommand (GtkWidget *widget, char *pk, GtkWidget *primKeyInput, char *name) {
    char *yn = "" ;
    retrieveComboBoxContent(widget, primKeyInput, &yn) ;

    if (strcmp(yn, "YES") == 0) {
        strcat(strcat(pk, name), ",") ;
        return 1 ;
    }
    return 0 ;
}

int addForeignKeyCommand (GtkWidget *widget, char *command, GtkWidget *input) {
    char *prop ;

    retrieveData(widget, input, &prop) ;
    if (strlen(prop) != 0) {
        strcat(strcat(command, "REFERENCES "), prop) ;
    }

    return 0 ;
}
