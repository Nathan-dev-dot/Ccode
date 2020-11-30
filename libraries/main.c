/*
    Nathan Letourneau & Sarah Schlegel
    01.11.2020
    Database manager
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

void printError (int errorNo) ;

char nameDB[30] ;

int main(int argc, char **argv){
    int kill ;
    kill = parseDoc();
    if (kill != 0) {
        printError (kill) ;
        return 1 ;
    }
}

void printError (int errorNo) {
    FILE *confFile = fopen("../config", "r") ;
    char str[150] ;
    char errMessage[150] ;
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
