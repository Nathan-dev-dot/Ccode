#ifndef STR_H
#define STR_H

void trimWhiteSpace (char *str) ;
void addSpace (char *str) ;
void removeLastChar (char *str) ;

uint8_t getErrorsFromConf (uint8_t errNo, GtkWidget *label) ;

//Manage database
uint8_t addColumnCommand(GtkWidget *widget, char *command, GtkColumn colInputs, char *pk) ;
void addNotMandatoryCommand(GtkWidget *widget, char *command, GtkColumn colInputs, char *type) ;
uint8_t addPrimaryKeyCommand (GtkWidget *widget, char *pk, GtkWidget *primKeyInput, char *name) ;
void addForeignKeyCommand (GtkWidget *widget, char *command, GtkWidget *input) ;
void retrieveAI (GtkWidget *widget, GtkColumn col, char *aiName) ;

#endif
