/*
    Nathan Letourneau & Sarah Schlegel
    01.11.2020
    Database managing program
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>
#include <mysql/mysql.h>
#include <gtk/gtk.h>

#include "gtk.h"
#include "db.h"
#include "sql.h"
#include "xml.h"
#include "str.h"
#include "errCodes.h"

char nameDB[30] ;

int main(int argc, char **argv) {
    initProg(argc, argv) ;
    return 0 ;
}
