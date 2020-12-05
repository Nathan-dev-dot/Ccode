#ifndef SQL_H
#define SQL_H

typedef struct ForeignKey {
    char tableName[30] ;
    char colName[30] ;
    char type[30] ;
} ForeignKey ;

int writeSQLColumn (xmlNodePtr node, char *colConf[10][2][20], char *command, ForeignKey *foreignKeys) ;
int catMandatory (xmlNodePtr n, char *colConf[10][2][20], int i, char *command) ;
void catNotMandatory (xmlNodePtr n, char *colConf[10][2][20], int i, char *command) ;
int catPrimaryKeys (xmlNodePtr parent, char *command) ;
ForeignKey * getForeignKeys (xmlNodePtr parent, int size) ;
int catForeignKeys(ForeignKey *foreignKeys, xmlNodePtr n, char *command) ;
int writeSQLFile(char *command, int first) ;
void dropSQLFile (void) ;

#endif
