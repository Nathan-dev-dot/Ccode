#include <stdio.h>
#include <stdlib.h>

int malloc2DimTable (char ***table, int size) {
    if ((*table = (char **)malloc(size * sizeof(char [30]))) == NULL)
        return 1 ;
    // for (int i = 0 ; i < size ; ++i) {
    //     printf("breakpoint\n") ;
    //     *table[i] = (char *)malloc(30 * sizeof(char)) ;
    // }
    printf("Hey\n") ;
    return 0 ;
}

void free2DimTable (char ***table, int size) {
    printf("Hello\n") ;
    free(*table) ;
    *table = NULL ;
}

int main() {
    char **tableGood ;
    char **tableBad ;

    malloc2DimTable(&tableGood, 2) ;
    printf("Ok1") ;
    for (int j = 0 ; j < 2 ; ++j) {
        printf("%p\n", tableGood) ;
    }
    free2DimTable(&tableGood, 2) ;

    printf("Ok") ;

    malloc2DimTable(&tableBad, 5) ;
    for (int j = 0 ; j < 5 ; ++j) {
        printf("%p\n", tableBad) ;
    }
    free2DimTable(&tableBad, 5) ;
    return 0 ;
}
