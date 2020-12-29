#ifndef STR_H
#define STR_H

void trimWhiteSpace (char *str) ;
void addSpace (char *str) ;
void removeLastChar (char *str) ;

int addColumnCommand(GtkWidget *widget, char *command, GtkColumn colInputs, char *pk) ;
void addNotMandatoryCommand(GtkWidget *widget, char *command, GtkColumn colInputs, char *type) ;
int addPrimaryKeyCommand (GtkWidget *widget, char *pk, GtkWidget *primKeyInput, char *name) ;
int addForeignKeyCommand (GtkWidget *widget, char *command, GtkWidget *input) ;
int retrieveAI (GtkWidget *widget, GtkColumn col, char *aiName) ;

#endif
