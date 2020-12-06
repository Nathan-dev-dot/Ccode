#ifndef XML_H
#define XML_H

#include "str.h"

void dbFromXML (void) ;
int parseDoc (void) ;
int countForeignKeys (xmlNodePtr start) ;
int writeSQLTables (xmlNodePtr node) ;
void enterPath (char *fileName) ;
Conf * initConf () ;

#endif
