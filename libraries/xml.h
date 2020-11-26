#ifndef XML_H
#define XML_H

int parseDoc (void) ;
int parseNodes (xmlNodePtr node, FILE *sqlFile) ;
void enterPath (char *fileName) ;
int initConf (char *conf[10][2][20]) ;

#endif
