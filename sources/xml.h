#ifndef XML_H
#define XML_H

#include "all.h"

void dbFromXML (GtkWidget *widget, GtkWidget *input) ;
int parseDoc (char *path) ;
int countForeignKeys (xmlNodePtr start) ;
int writeSQLTables (xmlNodePtr node) ;
void enterPath (char *fileName) ;
Conf * initConf () ;

#endif
