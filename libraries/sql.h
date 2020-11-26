#ifndef SQL_H
#define SQL_H

int writeSQLColumn (xmlNodePtr node, char *colConf[10][2][20], char *command, char *primKeys[][20], char *foreignKeys[15][3][20]) ;
int catMandatory (xmlNodePtr n, char *colConf[10][2][20], int i, char *command) ;
int catNotMandatory (xmlNodePtr n, char *colConf[10][2][20], int i, char *command, char *primKeys[][20]) ;
void addPrimaryKey (char *table[][20], char *colName) ;
void catPrimaryKeys (char *table[][20], char *command) ;
int getForeignKeys (char *table[15][3][20], xmlNodePtr parent) ;
int catForeignKeys(char *table[15][3][20], xmlNodePtr n, char *command) ;
int writeSqlFile(char *command, int first) ;

#endif
