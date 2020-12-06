/*
    Nathan Letourneau & Sarah Schlegel
    01.11.2020
    Database managing program
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>
#include <mysql/mysql.h>
#include <gtk/gtk.h>

#include "gtk.h"
#include "db.h"
#include "sql.h"
#include "xml.h"
#include "str.h"
#include "errCodes.h"

void printMenu (void) ;
int scanChoice (void) ;
void switchActions (int choice) ;

char nameDB[30] ;

int main(int argc, char **argv) {
    initProg(argc, argv) ;
    return 0 ;
}

/*
Function : printMenu
-------------------------
Prints the menu of the program
*/
void printMenu (void) {
    printf("\n***************************************\n\n") ;
    printf("0 – Quit\n") ;
    printf("1 - Create a database from an XML file and create the related SQL file\n") ;
    printf("2 – Manage database\n") ;
    printf("3 – Export a database in XML format\n") ;
}

/*
Function : scanChoice
-------------------------
Scans the user's choice

returns : the number of the selected choice
*/
int scanChoice (void) {
    int c ;
    do {
        printf("Select : ") ;
        scanf("%d", &c) ;
        fflush(stdin) ;
    } while (c < 0 || c > 3) ;
    return c ;
}

/*
Function : switchActions
-------------------------
Calls the selected function

int choice : the choice number that the user has input
*/
void switchActions (int choice) {
    switch (choice) {
        case 1 :
            dbFromXML() ;
            break ;
        case 3 :
            xmlFromDB() ;
            break ;
        default :
            break ;
    }
}
