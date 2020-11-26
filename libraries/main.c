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

FILE *confFile ;
char nameDB[30] ;

int main(int argc, char **argv){
    int kill ;
    kill = parseDoc();
    if (kill != 0) {
        return 1 ;
    }
}
