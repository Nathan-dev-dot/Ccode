#ifndef GTK_H
#define GTK_H

void hello (GtkWidget *widget, gpointer data) ;
void printMessage (GtkWidget *widget, uint8_t errNo, char *message) ;
GtkWidget * createWindow (char *windowTitle, uint16_t width, uint16_t height) ;
GtkWidget * createGrid (GtkWidget *window) ;

void initProg (int argc, char **argv) ;
void destroy(GtkWidget *widget, gpointer data) ;
void closeWindow (GtkWidget *widget, GtkWidget *window) ;

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
uint8_t addTable (GtkWidget *widget, XMLdbData *table) ;
void setTableName (GtkWidget *widget, GtkWidget *comboBox) ;
void actionOnTable (GtkWidget *widget, char *tName) ;
void showTableContent (GtkWidget *widget, char *tName) ;

void alterTableWindow (GtkWidget *widget, char *tName) ;
uint8_t retrieveColData(char *tName, GtkWidget *grid) ;
TableCol * getColStructure (GtkWidget *grid, XMLdbData *table, GtkWidget **dropButtons) ;
void setColEntries (MYSQL_ROW row, TableCol colNames, GtkColumn col, char *tName) ;

void dropColumn (GtkWidget *widget, TableCol *drop) ;

void dropVerify (GtkWidget *widget, char *tName) ;
void dropTable (GtkWidget *widget, char *tName) ;

uint8_t retrieveColNames (char *tName, GtkWidget *grid) ;
void retrieveColNb (GtkWidget *widget, XMLdbData *table) ;
uint8_t addColumns (GtkWidget *widget, XMLdbData *table) ;
void alterTable (GtkWidget *widget, XMLdbData *tableData) ;
uint8_t checkIfModified (GtkWidget *widget, XMLdbData *tableData, uint8_t i, char *command, MYSQL_ROW row) ;

void retrievePrimKeys (GtkWidget *widget, char *pk, XMLdbData *tableData, char *newAi) ;
uint8_t managePrimKeys (GtkWidget *widget, MYSQL_RES *results, XMLdbData *table) ;
void unsetAI (char *ai, char *tName) ;
void dropAddPrimKeys (char *primKeys, char *tName) ;

void inputDataSpeedRush (GtkWidget *widget) ;
void getCommand (GtkWidget *widget, GtkWidget *input) ;
void inputData (GtkWidget *widget, char *tName) ;
void inputDataWindow (GtkWidget *widget, Inserts *table) ;
void createInsertInputs(Inserts *table, GtkWidget *grid) ;
void insertData (GtkWidget *widget, Inserts *table) ;

void freeInserts (GtkWidget *widget, Inserts *inserts) ;
void freePointer (GtkWidget *widget, void *ptr) ;
void freeTableData (GtkWidget *widget, XMLdbData *xmlData) ;

uint8_t countLin (MYSQL_RES *res) ;
GtkColumn * createXmlColInputs (uint8_t nbCol, GtkWidget *grid) ;
void createXmlColLabels (GtkWidget *grid) ;
GtkColumn * createSqlColInputs (uint8_t nbCol, GtkWidget *grid) ;
void createSqlColLabels (GtkWidget *grid) ;

GtkWidget * createComboBoxType (void) ;
GtkWidget * createComboBoxYN (void) ;
GtkWidget * createComboBoxDBQuery (void) ;
GtkWidget * createComboBoxTables (void) ;
GtkWidget * createInput (char *placeholder) ;
void addLabel (GtkWidget *grid, uint8_t lin, uint8_t col, char *str) ;
void backgroundColor (GtkWidget *widget, char *color) ;

#endif
