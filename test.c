#include <stdio.h>
#include <string.h>

int readConfFile (FILE *confFile) {
    char static data[6][2][255] = { "" } ;
    char str[255] = "" ;
    int i = 0 ;
    if (strcmp(data[i][i], "")) {
        //while ()
        i++ ;
    } else {
        while (fgets(str, 30, confFile) != NULL && strcmp(str, "\n")) {
            strncpy(data[i][0], str, 1) ;
            data[i][0][1] = "/n" ;
            strcpy(data[i][1], strchr(str, ':')) ;
            i++ ;
        }
    }
    return 0 ;
}

int main () {
    FILE *confFile = fopen("config", "r") ;
    char str[30] ;
    if (confFile == NULL) {
        printf("Le fichier n'a pas été trouvé") ;
        return 1 ;
    }
    int i = readConfFile(confFile) ;
    fclose(confFile) ;
    return 0 ;
}
