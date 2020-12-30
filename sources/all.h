#ifndef ALL_H
#define ALL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <libxml/parser.h>
#include <mysql/mysql.h>
#include <gtk/gtk.h>

typedef struct ForeignKey {
    char tableName[30] ;
    char colName[30] ;
    char type[30] ;
} ForeignKey ;

typedef struct Conf {
    uint8_t mand ;
    char prop[20] ;
} Conf ;

typedef struct GtkDualInputs {
    GtkWidget *name;
    GtkWidget *nb;
    GtkWidget *window ;
} GtkDualInputs ;

typedef struct GtkColumn {
    GtkWidget *name ;
    GtkWidget *type ;
    GtkWidget *size ;
    GtkWidget *constraints ;
    GtkWidget *check ;
    GtkWidget *def ;
    GtkWidget *primKey ;
    GtkWidget *ref ; //references another table
    GtkWidget *refd ; //is referenced by another table
} GtkColumn ;

typedef struct TablePosition {
    uint8_t current ;
    uint8_t total ;
} TablePosition ;

typedef struct XMLdbData {
    xmlDocPtr doc ;
    xmlNodePtr root ;
    TablePosition pos ;
    char name[30];
    int16_t size ;
    GtkColumn *columns;
    Conf *conf ;
    GtkDualInputs *dualInputs;
    uint8_t (*colFunc) (GtkWidget *, struct XMLdbData *) ;
} XMLdbData ;

typedef struct Inserts {
    char name[30] ;
    int16_t nbRows ;
    int8_t nbCols ;
    GtkWidget ***inputs ;
    GtkDualInputs *dualInputs ;
} Inserts ;

typedef struct MysqlCoAndRes {
    MYSQL *mysql ;
    MYSQL_RES *results ;
    uint16_t nbFields ;
} MysqlCoAndRes ;

typedef struct TableCol {
    char tName[30] ;
    char cName[30] ;
    GtkWidget *window ;
} TableCol ;

#include "gtk.h"
#include "db.h"
#include "sql.h"
#include "xml.h"
#include "str.h"
#include "errCodes.h"

#define removeChariot(str) if (str[strlen(str) - 1] == '\n') { str[strlen(str) - 1] = '\0' ; }

#endif
