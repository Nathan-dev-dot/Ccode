#ifndef GTK_H
#define GTK_H

void hello (GtkWidget *widget) ;
int initProg (int argc, char **argv) ;
void destroy(GtkWidget *widget, gpointer data) ;
void closeWindow (GtkWidget *window) ;

void mainMenu (GtkBuilder *builder, GtkWidget *window) ;
void retrieveData(GtkWidget *widget, GtkWidget *input, char **str);

void dbFromXMLWindow (GtkWidget *widget) ;

void xmlFromEntries (GtkWidget *widget);
void tableData (GtkWidget *widget) ;

GtkWidget * createInput (char *placeholder) ;
void background_color( GtkWidget **widget, char *color ) ;

#endif
