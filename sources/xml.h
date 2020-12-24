#ifndef XML_H
#define XML_H

#include "all.h"

void dbFromXML (GtkWidget *widget, GtkWidget *input) ;
int parseDoc (char *path) ;
int countForeignKeys (xmlNodePtr start) ;
int writeSQLTables (xmlNodePtr node) ;
Conf * initConf () ;

void createXMLFile (GtkWidget *widget, GtkDualInputs *dbParams) ;
int createDoc (GtkWidget *widget, GtkDualInputs *dbParams) ;
int setXMLDatabase (GtkWidget *widget, GtkWidget *input, char *path) ;
int duplicateTemplate (char *fileName) ;
int setRoot (xmlNodePtr root) ;

void setTableData (GtkWidget *widget, XMLdbData *dbData) ;

int addTableNode (GtkWidget *widget, XMLdbData *dbData) ;
xmlNodePtr addColumnNode (GtkWidget *widget, GtkColumn colInputs) ;
int addMandatory (GtkWidget *widget, xmlNodePtr col, GtkColumn colInputs) ;
int addNotMandatory (GtkWidget *widget, xmlNodePtr col, GtkColumn colInputs) ;
int addPrimaryKey (GtkWidget *widget, xmlNodePtr col, GtkWidget *primKeyInput) ;
int addForeignKey (GtkWidget *widget, xmlNodePtr col, GtkColumn colInputs) ;

int retrieveInteger (GtkWidget *widget, GtkWidget *input, int *integer) ;

int writeXMLFile (char *fileName, xmlDocPtr doc) ;

#endif
