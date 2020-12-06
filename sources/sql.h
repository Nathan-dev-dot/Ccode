#ifndef SQL_H
#define SQL_H

#include "str.h"

int writeSQLColumn (xmlNodePtr node, Conf *colConf, char *command, ForeignKey *foreignKeys) ;
int catMandatory (xmlNodePtr n, Conf *colConf, int i, char *command) ;
void catNotMandatory (xmlNodePtr n, Conf *colConf, int i, char *command) ;
int catPrimaryKeys (xmlNodePtr parent, char *command) ;
ForeignKey * getForeignKeys (xmlNodePtr parent, int size) ;
int catForeignKeys(ForeignKey *foreignKeys, xmlNodePtr n, char *command) ;
int writeSQLFile(char *command, int first) ;
void dropSQLFile (void) ;

#endif
