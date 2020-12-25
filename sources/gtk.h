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

void setDBName (GtkWidget *widget, GtkWidget *comboBox);
void showTables (GtkWidget *widget) ;
void setTableName (GtkWidget *widget, GtkWidget *comboBox) ;
void actionOnTable (GtkWidget *widget, char *tName) ;
void describeTable (GtkWidget *widget, char *tName) ;
int retrieveColData(char *tName, GtkWidget *grid) ;
void addLabel (GtkWidget *grid, int lin, int col, char *str) ;

GtkColumn * createColInputs (int nbCol, GtkWidget *grid) ;
void createColLabels (GtkWidget *grid) ;
GtkWidget * createComboBoxType (void) ;
GtkWidget * createComboBoxYN (void) ;
GtkWidget * createComboBoxDBQuery (void) ;
GtkWidget * createComboBoxTables (void) ;
GtkWidget * createInput (char *placeholder) ;
void background_color (GtkWidget *widget, char *color) ;

#endif
