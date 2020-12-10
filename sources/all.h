#ifndef ALL_H
#define ALL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>
#include <mysql/mysql.h>
#include <gtk/gtk.h>

typedef struct ForeignKey {
    char tableName[30] ;
    char colName[30] ;
    char type[30] ;
    struct ForeignKey *next ;
} ForeignKey ;

typedef struct Conf {
    int mand ;
    char prop[20] ;
} Conf ;

#include "gtk.h"
#include "db.h"
#include "sql.h"
#include "xml.h"
#include "str.h"
#include "errCodes.h"

#define removeChariot(str) if (str[strlen(str) - 1] == '\n') { str[strlen(str) - 1] = '\0' ; }

#endif
