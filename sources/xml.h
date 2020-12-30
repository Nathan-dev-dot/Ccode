#ifndef XML_H
#define XML_H

#include "all.h"

void dbFromXML (GtkWidget *widget, GtkWidget *input) ;
uint8_t parseDoc (char *path) ;
uint8_t countForeignKeys (xmlNodePtr start) ;
uint8_t writeSQLTables (xmlNodePtr node) ;
Conf * initConf () ;

void createXMLFile (GtkWidget *widget, GtkDualInputs *dbParams) ;
uint8_t createDoc (GtkWidget *widget, GtkDualInputs *dbParams) ;
uint8_t setXMLDatabase (GtkWidget *widget, GtkWidget *input, char *path) ;
uint8_t duplicateTemplate (char *fileName) ;
uint8_t setRoot (xmlNodePtr root) ;

void setTableData (GtkWidget *widget, XMLdbData *dbData) ;

uint8_t addTableNode (GtkWidget *widget, XMLdbData *dbData) ;
xmlNodePtr addColumnNode (GtkWidget *widget, GtkColumn colInputs) ;
uint8_t addMandatory (GtkWidget *widget, xmlNodePtr col, GtkColumn colInputs) ;
uint8_t addNotMandatory (GtkWidget *widget, xmlNodePtr col, GtkColumn colInputs) ;
int8_t addPrimaryKey (GtkWidget *widget, xmlNodePtr col, GtkWidget *primKeyInput) ;
uint8_t addForeignKey (GtkWidget *widget, xmlNodePtr col, GtkColumn colInputs) ;

uint8_t retrieveInteger (GtkWidget *widget, GtkWidget *input, int16_t *integer) ;

uint8_t writeXMLFile (char *fileName, xmlDocPtr doc) ;

#endif
