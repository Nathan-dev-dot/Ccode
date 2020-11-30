#ifndef XML_H
#define XML_H

void dbFromXML (void) ;
int parseDoc (void) ;
int writeSQLTables (xmlNodePtr node) ;
void enterPath (char *fileName) ;
int initConf (char *conf[10][2][20]) ;

#endif
