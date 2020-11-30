/*
    Nathan Letourneau & Sarah Schlegel
    01.11.2020
    Library of string modifiers
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
    str[strlen(str) - 1] = '\0' ;
}
