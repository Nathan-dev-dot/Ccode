#ifndef GTK_H
#define GTK_H

void hello (GtkWidget *widget, gpointer data) ;
void printError (GtkWidget *widget, int errNo, char *message) ;
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
int addTable (GtkWidget *widget, XMLdbData *table) ;
void setTableName (GtkWidget *widget, GtkWidget *comboBox) ;
void actionOnTable (GtkWidget *widget, char *tName) ;
void showTableContent (GtkWidget *widget, char *tName) ;

void alterTableWindow (GtkWidget *widget, char *tName) ;
int retrieveColData(char *tName, GtkWidget *grid) ;
void getColStructure (GtkWidget *grid, XMLdbData *table, GtkWidget **dropButtons) ;
void dropColumn (GtkWidget *widget, TableCol *drop) ;

int retrieveColNames (char *tName, GtkWidget *grid) ;
void retrieveColNb (GtkWidget *widget, XMLdbData *table) ;
int addColumns (GtkWidget *widget, XMLdbData *table) ;
void alterTable (GtkWidget *widget, XMLdbData *tableData) ;
int checkIfModified (GtkWidget *widget, XMLdbData *tableData, int i, char *command, MYSQL_ROW row) ;

void retrievePrimKeys (GtkWidget *widget, char *pk, XMLdbData *tableData, char *newAi) ;
int managePrimKeys (GtkWidget *widget, MYSQL_RES *results, XMLdbData *table) ;
void unsetAI (char *ai, char *tName) ;
void dropAddPrimKeys (char *primKeys, char *tName) ;

void inputDataSpeedRush (GtkWidget *widget) ;
void getCommand (GtkWidget *widget, GtkWidget *input) ;
void inputData (GtkWidget *widget, char *tName) ;
void inputDataWindow (GtkWidget *widget, Inserts *table) ;
void createInsertInputs(Inserts *table, GtkWidget *grid) ;
void insertData (GtkWidget *widget, Inserts *table) ;
void freeDoubleWidgets (GtkWidget ***widgets, int nbRows) ;

int countLin (MYSQL_RES *res) ;
GtkColumn * createXmlColInputs (int nbCol, GtkWidget *grid) ;
void createXmlColLabels (GtkWidget *grid) ;
GtkColumn * createSqlColInputs (int nbCol, GtkWidget *grid) ;
void createSqlColLabels (GtkWidget *grid) ;

GtkWidget * createComboBoxType (void) ;
GtkWidget * createComboBoxYN (void) ;
GtkWidget * createComboBoxDBQuery (void) ;
GtkWidget * createComboBoxTables (void) ;
GtkWidget * createInput (char *placeholder) ;
void addLabel (GtkWidget *grid, int lin, int col, char *str) ;
void background_color (GtkWidget *widget, char *color) ;

#endif
