#ifndef SQL_H
#define SQL_H

#include "str.h"

uint8_t writeSQLColumn (xmlNodePtr node, Conf *colConf, char *command, ForeignKey *foreignKeys) ;
uint8_t catMandatory (xmlNodePtr n, Conf *colConf, uint8_t i, char *command) ;
void catNotMandatory (xmlNodePtr n, Conf *colConf, uint8_t i, char *command) ;
uint8_t catPrimaryKeys (xmlNodePtr parent, char *command) ;
ForeignKey * getForeignKeys (xmlNodePtr parent, uint8_t size) ;
uint8_t catForeignKeys(ForeignKey *foreignKeys, xmlNodePtr n, char *command) ;
uint8_t writeSQLFile(char *command, uint8_t first) ;
void dropSQLFile (void) ;

#endif
