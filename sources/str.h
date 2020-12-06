#ifndef STR_H
#define STR_H

typedef struct ForeignKey {
    char tableName[30] ;
    char colName[30] ;
    char type[30] ;
} ForeignKey ;

typedef struct Conf {
    int mand ;
    char prop[20] ;
} Conf ;

void printError (int errorNo) ;
void trimWhiteSpace (char *str) ;
void addSpace (char *str) ;
void removeLastChar (char *str) ;

#endif
