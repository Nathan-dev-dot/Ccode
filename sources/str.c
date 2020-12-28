/*
    Nathan Letourneau & Sarah Schlegel
    01.11.2020
    Project library of string modifiers
*/

#include "all.h"


/*
Function : printError
-------------------------
Prints the errors reading from the config file

int errorNo : error number given by the previous functions
*/
void printError (int errorNo) {
    FILE *confFile = fopen("../config", "r") ;
    char str[150] ;
    int errCode ;

    if (confFile == NULL){
        printf("Error in opening the configuration file") ;
        return ;
    }

    while (fgets(str, 150, confFile) != NULL) {
        if (strcmp(str, "[error_codes]\n") == 0) {
            while (fgets(str, 150, confFile) != NULL && strcmp(str, "\n") != 0) {
                sscanf(str, "%d", &errCode) ;
                if (errCode == errorNo) {
                    printf("************ERROR************\n") ;
                    printf("%s", strchr(str, ':') + 1) ;
                    printf("*****************************\n") ;
                    return ;
                }
            }
        }
    }

    printf("Error code not found\n") ;
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
