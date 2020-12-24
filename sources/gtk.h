#ifndef GTK_H
#define GTK_H

void hello (GtkWidget *widget, XMLdbData *data) ;
int initProg (int argc, char **argv) ;
void destroy(GtkWidget *widget, gpointer data) ;
void closeWindow (GtkWidget *window) ;

void mainMenu (GtkBuilder *builder, GtkWidget *window) ;
void retrieveData(GtkWidget *widget, GtkWidget *input, char **str);
void retrieveComboBoxContent (GtkWidget *widget, GtkWidget *box, char **str) ;

void dbFromXMLWindow (GtkWidget *widget) ;

void xmlFromEntries (GtkWidget *widget);
void writeTables (GtkWidget *widget, XMLdbData *xmlData) ;
void tableData (GtkWidget *widget, XMLdbData *dbData) ;
void getTableColumns (GtkWidget *widget, XMLdbData *dbData) ;

void dbManagerWindow (GtkWidget *widget) ;

GtkColumn * createColInputs (int nbCol, GtkWidget *grid) ;
GtkWidget * createComboBoxType (void) ;
GtkWidget * createComboBoxYN (void) ;
GtkWidget * createInput (char *placeholder) ;
void background_color( GtkWidget **widget, char *color ) ;

#endif
