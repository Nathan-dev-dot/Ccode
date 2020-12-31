/*
    Nathan Letourneau & Sarah Schlegel
    01.11.2020
    Project library with GTK functions
*/

#include "all.h"

extern char nameDB[30] ;


/*
Function : hello
-------------------------
Prints hello
-------------------------
GtkWidget *widget : widget
gpointer data : pointer to data of type (void *)
-------------------------
*/
void hello (GtkWidget *widget, gpointer data) {
    printf("Hello %p\n", data) ;
}

/*
Function : printMessage
-------------------------
Prints a message in a new window
-------------------------
GtkWidget *widget : widget
uint8_t errNo : error number given by the previous functions
char *message : message to be eventually printed
-------------------------
*/
void printMessage (GtkWidget *widget, uint8_t errNo, char *message) {
    uint8_t found = 0;
    GtkBuilder *builder;
    GtkWidget *window;
    GtkWidget *label ;

    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "main.glade", NULL);

    window = GTK_WIDGET(gtk_builder_get_object(builder, "windowPopup"));
    gtk_builder_connect_signals(builder, NULL);
    gtk_window_set_default_size (GTK_WINDOW (window), 200, 100);
    backgroundColor(window, "#999999" );

    label = GTK_WIDGET(gtk_builder_get_object(builder, "label"));
    if (strlen(message) == 0) {
        found = getErrorsFromConf(errNo, label) ;

        if (found == 0)
            gtk_label_set_text(GTK_LABEL(label), (const gchar *)"Error code not found") ;
    } else {
        gtk_label_set_text(GTK_LABEL(label), (const gchar *)message) ;
    }

    g_object_unref(builder);
    gtk_widget_show_all(window);
    gtk_main();
}

/*
Function : createWindow
-------------------------
Creates a gtk window
-------------------------
char *windowTitle : title given to the window
uint16_t width : width of the window
uint16_t height : height of the window
-------------------------
Return values
    The GtkWidget * pointing on the window
*/
GtkWidget * createWindow (char *windowTitle, uint16_t width, uint16_t height) {
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), (const gchar *)windowTitle) ;
    gtk_window_set_default_size(GTK_WINDOW (window), width, height);
    backgroundColor(window, "#999999" );
    return window ;
}

/*
Function : createGrid
-------------------------
Creates a gtk grid attached a the window
-------------------------
GtkWidget *window : window
-------------------------
Return values
    The GtkWidget * pointing on the grid
*/
GtkWidget * createGrid (GtkWidget *window) {
    GtkWidget *grid = gtk_grid_new() ;
    gtk_container_add(GTK_CONTAINER(window), grid) ;
    g_object_set(grid, "margin", 12, NULL);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10) ;
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10) ;
    return grid ;
}

/*
Function : createInput
-------------------------
Creates a gtk input
-------------------------
char *placeholder : the value of the input's placeholder
-------------------------
Return values
    The GtkWidget * pointing on the input
*/
GtkWidget * createInput (char *placeholder) {
    GtkWidget *input = gtk_entry_new() ;
    gtk_entry_set_placeholder_text(GTK_ENTRY(input), placeholder) ;
    return input ;
}

/*
Function : addLabel
-------------------------
Adds a label to the grid at position (lin, col)
-------------------------
GtkWidget *grid : the grid pointer
uint8_t lin : lin position of the label
uint8_t col : col position of the label
char *str : label content
-------------------------
*/
void addLabel (GtkWidget *grid, uint8_t lin, uint8_t col, char *str) {
    GtkWidget *label ;
    label = gtk_label_new((const gchar *)str) ;
    gtk_grid_attach(GTK_GRID(grid), label, col, lin, 1, 1) ;
}

/*
Function : backgroundColor
-------------------------
Sets the background of a window
-------------------------
GtkWidget *widget : widget
char *color : the color that you want in background
-------------------------
*/
void backgroundColor (GtkWidget *widget, char *color) {
  GtkCssProvider * cssProvider = gtk_css_provider_new();

  char css[64] = "* { background-image:none; background-color:";
  strcat(strcat(css, color), ";}");

  gtk_css_provider_load_from_data(cssProvider, css,-1,NULL);
  GtkStyleContext * context = gtk_widget_get_style_context(widget);
  gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(cssProvider),GTK_STYLE_PROVIDER_PRIORITY_USER);
}



/*
Function : destroy
-------------------------
Quits the program
-------------------------
GtkWidget *widget : widget
gpointer data : void pointer
-------------------------
*/
void destroy (GtkWidget *widget, gpointer data) {
    gtk_main_quit();
}

/*
Function : closeWindow
-------------------------
Closes a window
-------------------------
GtkWidget *widget : widget sent by callback functions
GtkWidget *window : window to be destroyed
-------------------------
*/
void closeWindow (GtkWidget *widget, GtkWidget *window) {
    gtk_window_close(GTK_WINDOW(window)) ;
}



/*
Function : initProg
-------------------------
Initialises the program
-------------------------
char *placeholder : the value of the input's placeholder
-------------------------
*/
void initProg (int argc, char **argv) {
    gtk_init(&argc, &argv);
    askCredentials() ;
}

/*
Function : askCredentials
-------------------------
Displays a window asking the mysql username and password
Sends the data to verifyCredentials
-------------------------
*/
void askCredentials (void) {
    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *button ;
    GtkDualInputs dbCredentials;

    window = createWindow("Input your credentials", 200, 200) ;
    grid = createGrid(window) ;

    dbCredentials.window = window ;

    addLabel(grid, 0, 0, "Enter your MYSQL username") ;
    dbCredentials.name = createInput("Username") ;
    gtk_grid_attach(GTK_GRID(grid), dbCredentials.name, 0, 2, 1, 1) ;

    addLabel(grid, 3, 0, "Enter your MYSQL password") ;
    dbCredentials.nb = createInput("Password") ;
    gtk_grid_attach(GTK_GRID(grid), dbCredentials.nb, 0, 4, 1, 1) ;

    button = gtk_button_new_with_label((const gchar *)"Send") ;
    g_signal_connect (button, "clicked", G_CALLBACK(verifyCredentials), &dbCredentials);
    gtk_grid_attach(GTK_GRID(grid), button, 0, 6, 1, 1) ;

    gtk_widget_show_all(window) ;
    gtk_main() ;
}

/*
Function : verifyCredentials
-------------------------
Retrieves the data in the credentials entries
Verifies if the username isn't null
If the data is valid, sends it to setCredentials
-------------------------
GtkWidget *widget : widget sent by callback function
GtkDualInputs *dbCredentials : structure of two inputs (username and password)
-------------------------
*/
void verifyCredentials (GtkWidget *widget, GtkDualInputs *dbCredentials) {
    char *username ;
    char *password ;
    int kill ;

    retrieveData(widget, dbCredentials->name, &username) ;
    retrieveData(widget, dbCredentials->nb, &password) ;
    if (strlen(username) == 0)
        return ;

    if ((kill = setCredentials(username, password)) != 0) {
        printMessage(NULL, 0, "Invalid MYSQL credentials") ;
        return ;
    }

    closeWindow(NULL, dbCredentials->window) ;
    mainMenu() ;
}



/*
Function : mainMenu
-------------------------
Creates the main menu of the program
-------------------------
*/
void mainMenu (void) {
    GtkBuilder *builder;
    GtkWidget *window;
    GtkWidget *option1;
    GtkWidget *option2;
    GtkWidget *option3;

    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "main.glade", NULL);

    window = GTK_WIDGET(gtk_builder_get_object(builder, "windowMain"));
    gtk_builder_connect_signals(builder, NULL);
    g_signal_connect(window, "delete_event", G_CALLBACK(destroy), NULL) ;
    backgroundColor(window, "#999999" );

    option1 = GTK_WIDGET(gtk_builder_get_object(builder, "dbFromXml"));
    g_signal_connect(option1, "clicked", G_CALLBACK(dbFromXMLWindow), NULL);

    option2 = GTK_WIDGET(gtk_builder_get_object(builder, "createXml"));
    g_signal_connect(option2, "clicked", G_CALLBACK(xmlFromEntries), NULL);

    option3 = GTK_WIDGET(gtk_builder_get_object(builder, "manageDatabase"));
    g_signal_connect(option3, "clicked", G_CALLBACK(dbManagerWindow), NULL);

    g_object_unref(builder);
    gtk_widget_show_all(window);
    gtk_main();
}



/*-------------------Create a database from an XML document-------------------*/

/*
Function : dbFromXMLWindow
-------------------------
Creates a window asking to input the XML file path
-------------------------
GtkWidget *widget : widget sent by callback function
-------------------------
*/
void dbFromXMLWindow (GtkWidget *widget) {
    GtkBuilder *builder;
    GtkWidget *window;
    GtkWidget *okButton ;
    GtkWidget *input ;

    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "main.glade", NULL);

    window = GTK_WIDGET(gtk_builder_get_object(builder, "windowXml"));
    gtk_builder_connect_signals(builder, NULL);
    backgroundColor(window, "#999999");

    input = GTK_WIDGET(gtk_builder_get_object(builder, "XMLPath")) ;
    okButton = GTK_WIDGET(gtk_builder_get_object(builder, "validate")) ;
    g_signal_connect(okButton, "clicked", G_CALLBACK(dbFromXML), input);

    g_object_unref(builder);
    gtk_widget_show_all(window);
    gtk_main();
}



/*-------------------Create an XML document from entries----------------------*/

/*
Function : xmlFromEntries
-------------------------
Asks for the xml database and file name and the number of tables it will contain
Sends the data to createXMLDoc
-------------------------
GtkWidget *widget : widget sent by the callback function
-------------------------
*/
void xmlFromEntries (GtkWidget *widget) {
    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *button ;
    GtkDualInputs dbParams;

    window = createWindow("Create an XML file", 200, 200) ;
    grid = createGrid(window) ;

    dbParams.window = window ;

    addLabel(grid, 0, 0, "Enter the database name") ;
    dbParams.name = createInput("Database name") ;
    gtk_grid_attach(GTK_GRID(grid), dbParams.name, 0, 2, 1, 1) ;

    addLabel(grid, 3, 0, "Enter the number of tables") ;
    dbParams.nb = createInput("Number") ;
    gtk_grid_attach(GTK_GRID(grid), dbParams.nb, 0, 4, 1, 1) ;

    button = gtk_button_new_with_label((const gchar *)"Send") ;
    g_signal_connect (button, "clicked", G_CALLBACK (createXMLDoc), &dbParams);
    gtk_grid_attach(GTK_GRID(grid), button, 0, 6, 1, 1) ;

    gtk_widget_show_all(window) ;
    gtk_main() ;
}

/*
Function : writeTables
-------------------------
Calls for tableData as long as all the tables are being written
At the end, frees the tables allocated in xmlData
-------------------------
GtkWidget *widget : widget sent by the callback function
XMLdbData *xmlData : structure with the XML data
-------------------------
*/
void writeTables (GtkWidget *widget, XMLdbData *xmlData) {
    xmlData->pos.current++ ;
    if (xmlData->pos.current <= xmlData->pos.total) {
        tableDataWindow(widget, xmlData) ;
    } else {
        printMessage(widget, 0, "XML file created in Outputs folder") ;
        freeTableData(widget, xmlData) ;
    }
    return ;
}

/*
Function : tableDataWindow
-------------------------
Creates a window asking for the table data (name and number of columns)
Sends the data to setTableData
-------------------------
GtkWidget *widget : widget sent by the callback function
XMLdbData *xmlData : structure with the XML data
-------------------------
*/
void tableDataWindow (GtkWidget *widget, XMLdbData *dbData) {
    GtkWidget *window ;
    GtkWidget *grid;
    GtkWidget *button ;
    GtkDualInputs dbParams;

    if (dbData->dualInputs->window != NULL)
        closeWindow(NULL, dbData->dualInputs->window) ;

    window = createWindow("Table data", 200, 200) ;
    grid = createGrid(window) ;

    dbParams.window = window ;

    addLabel(grid, 0, 0, "Enter the table name") ;
    dbParams.name = createInput("Database name") ;
    gtk_grid_attach(GTK_GRID(grid), dbParams.name, 0, 2, 1, 1) ;

    addLabel(grid, 3, 0, "Enter the number of columns") ;
    dbParams.nb = createInput("Number") ;
    gtk_grid_attach(GTK_GRID(grid), dbParams.nb, 0, 4, 1, 1) ;

    dbData->dualInputs = &dbParams ;

    button = gtk_button_new_with_label((const gchar *)"Send") ;
    g_signal_connect (button, "clicked", G_CALLBACK (setTableData), dbData);
    gtk_grid_attach(GTK_GRID(grid), button, 0, 6, 1, 1) ;

    gtk_widget_show_all(window) ;
    gtk_main() ;
}

/*
Function : getTableColumns
-------------------------
Creates the columns for the table
Calls createXmlColInputs to create all the inputs for each column
Sends the data to the function set in the dbData parameter
-------------------------
GtkWidget *widget : widget sent by the callback function
XMLdbData *xmlData : structure with the XML data
-------------------------
*/
void getTableColumns (GtkWidget *widget, XMLdbData *dbData) {
    GtkWidget *window ;
    GtkWidget *grid ;
    GtkWidget *button ;
    char tmp[30] = "Table " ;

    strcat(tmp, dbData->name) ;

    window = createWindow(tmp, 1500,  200 + 50 * dbData->size) ;
    grid = createGrid(window) ;

    dbData->columns = createXmlColInputs(dbData->size, grid) ;
    dbData->dualInputs->window = window ;

    button = gtk_button_new_with_label((const gchar *)"Send") ;
    gtk_grid_attach(GTK_GRID(grid), button, 8, dbData->size + 1, 1, 1) ;
    g_signal_connect(button, "clicked", G_CALLBACK(dbData->colFunc), dbData) ;

    gtk_widget_show_all(window) ;
    gtk_main() ;
}



/*-------------------Create an XML document from entries----------------------*/

/*
Function : dbManagerWindow
-------------------------
Creates a window asking for the database you want to modify
Sends the data to setDBName
-------------------------
GtkWidget *widget : widget sent by the callback function
-------------------------
*/
void dbManagerWindow (GtkWidget *widget) {
    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *button ;
    GtkWidget *combo ;
    GtkWidget *label ;

    window = createWindow("Database", 200, 200) ;
    grid = createGrid(window) ;

    label = gtk_label_new((const gchar *)"Select your database") ;
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1) ;

    combo = createComboBoxDBQuery();
    gtk_grid_attach(GTK_GRID(grid), combo, 0, 1, 1, 1) ;

    button = gtk_button_new_with_label((const gchar *)"Select") ;
    g_signal_connect (button, "clicked", G_CALLBACK (setDBName), combo);
    gtk_grid_attach(GTK_GRID(grid), button, 0, 2, 1, 1) ;

    gtk_widget_show_all(window) ;
    gtk_main() ;

    return ;
}

/*
Function : setDBName
-------------------------
Sets the database name in the global variable
-------------------------
GtkWidget *widget : widget sent by the callback function
GtkWidget *comboBox : GTK combo box retrieving the database name
-------------------------
*/
void setDBName (GtkWidget *widget, GtkWidget *comboBox) {
    char *tmp = "" ;
    retrieveComboBoxContent(widget, comboBox, &tmp) ;
    if (tmp == NULL)
        return ;
    strcpy(nameDB, tmp) ;
    showTables(widget) ;
}

/*
Function : showTables
-------------------------
Displays the tables in the selected database
Sends the data to setTableName or calls tableDataWindow to add a table
-------------------------
GtkWidget *widget : widget sent by the callback function
-------------------------
*/
void showTables (GtkWidget *widget) {
    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *selectButton ;
    GtkWidget *addButton ;
    GtkWidget *combo ;
    char str[50] = "Using db " ;
    XMLdbData table ;
    GtkDualInputs di ;

    table.colFunc = &addTable ;

    strcat(str, nameDB) ;

    window = createWindow(str, 200, 200) ;
    grid = createGrid(window) ;

    di.window = window ;
    table.dualInputs = &di ;

    addLabel(grid, 0, 0, "Select your table") ;

    combo = createComboBoxTables();
    gtk_grid_attach(GTK_GRID(grid), combo, 0, 1, 1, 1) ;

    selectButton = gtk_button_new_with_label((const gchar *)"Select") ;
    g_signal_connect (selectButton, "clicked", G_CALLBACK (setTableName), combo);
    gtk_grid_attach(GTK_GRID(grid), selectButton, 0, 2, 1, 1) ;

    addButton = gtk_button_new_with_label((const gchar *)"Add a table") ;
    g_signal_connect (addButton, "clicked", G_CALLBACK (tableDataWindow), &table);
    gtk_grid_attach(GTK_GRID(grid), addButton, 0, 3, 1, 1) ;

    gtk_widget_show_all(window) ;
    gtk_main() ;
}

/*
Function : addTable
-------------------------
Writes the SQL command to add a table
Calls for addColumnCommand to retrieve the values of the inputs
Sends the command to connectDB
-------------------------
GtkWidget *widget : widget sent by the callback function
XMLdbData *table : structure containing the table data
-------------------------
*/
uint8_t addTable (GtkWidget *widget, XMLdbData *table) {
    size_t nbCol = table->size ;
    char command[500] = "CREATE TABLE " ;
    char pk[100] = "PRIMARY KEY(" ;
    size_t i ;
    size_t kill ;
    size_t pkNb = 0 ;

    strcat(strcat(command, table->name),"(") ;

    for (i = 0 ; i < nbCol ; ++i) {
        kill = addColumnCommand(widget, command, table->columns[i], pk) ;
        if (kill != 0 && kill != 1)
            return kill ;
        pkNb += kill ;
    }

    if (pkNb == 0) {
        printMessage(widget, ERR_XML, "") ;
        return ERR_XML ;
    }

    removeLastChar(pk) ;
    strcat(strcat(command, pk), "))") ;
    kill = connectDB(command) ;
    if (kill != 0) {
        printMessage(widget, kill, "") ;
    } else {
        closeWindow(NULL, table->dualInputs->window) ;
        showTables(widget) ;
        printMessage(widget, 0, "Table added !") ;
    }
    return 0 ;
}

/*
Function : setTableName
-------------------------
Sets the talbe name in a variable
Sends the data to actionOnTableWindow
-------------------------
GtkWidget *widget : widget sent by the callback function
GtkWidget *comboBox : GTK combo box retrieving the table name
-------------------------
*/
void setTableName (GtkWidget *widget, GtkWidget *comboBox) {
    char *table = "" ;
    retrieveComboBoxContent(widget, comboBox, &table) ;
    if (table == NULL)
        return ;
    actionOnTableWindow(widget, table) ;
}

/*
Function : actionOnTableWindow
-------------------------
Displays the different actions that can be performed on a table
Can send the table name to
    showTableContent    to show the table's content
    alterTableWindow    to alter the table
    inputData           to input data in the table
    dropVerify          to drop the table
-------------------------
GtkWidget *widget : widget sent by the callback function
char *tName : selected table name
-------------------------
*/
void actionOnTableWindow (GtkWidget *widget, char *tName) {
    GtkBuilder *builder;
    GtkWidget *window;
    GtkWidget *showCol ;
    GtkWidget *alterT ;
    GtkWidget *input ;
    GtkWidget *dropT ;

    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "main.glade", NULL);

    window = GTK_WIDGET(gtk_builder_get_object(builder, "windowTable"));
    gtk_builder_connect_signals(builder, NULL);
    backgroundColor(window, "#999999" );

    showCol = GTK_WIDGET(gtk_builder_get_object(builder, "showCol"));
    g_signal_connect(showCol, "clicked", G_CALLBACK(showTableContent), tName);

    alterT = GTK_WIDGET(gtk_builder_get_object(builder, "alterT"));
    g_signal_connect(alterT, "clicked", G_CALLBACK(alterTableWindow), tName);

    input = GTK_WIDGET(gtk_builder_get_object(builder, "inputData"));
    g_signal_connect(input, "clicked", G_CALLBACK(inputData), tName);

    dropT = GTK_WIDGET(gtk_builder_get_object(builder, "dropTable"));
    g_signal_connect(dropT, "clicked", G_CALLBACK(dropVerify), tName);

    g_object_unref(builder);
    gtk_widget_show_all(window);
    gtk_main();
}



/*
Function : showTableContent
-------------------------
Displays the content of the selected table
-------------------------
GtkWidget *widget : widget sent by the callback function
char *tName : selected table name
-------------------------
*/
void showTableContent (GtkWidget *widget, char *tName) {
    GtkWidget *window;
    GtkWidget *grid;
    char str[50] = "Showing table " ;
    size_t nbLin ;

    strcat(str, tName) ;

    window = createWindow(str, 600, 200) ;
    grid = createGrid(window) ;


    retrieveColNames(tName, grid) ;
    nbLin = retrieveColData(tName, grid) ;
    if (nbLin == 0) {
        printMessage(widget, 0, "Sorry, there's no data in this table") ;
        return ;
    }

    gtk_window_set_default_size(GTK_WINDOW (window), 600, 200 + 15 * nbLin);

    gtk_widget_show_all(window) ;
    gtk_main() ;
}

/*
Function : retrieveColNames
-------------------------
Retrieves the names of the columns from mysql and adds them to the grid in labels
-------------------------
char *tName : selected table name
GtkWidget *grid : grid to which are attached the labels
-------------------------
Return values
    the number of columns in the table
    0 if there's no data in the table
*/
uint8_t retrieveColNames (char *tName, GtkWidget *grid) {
    MYSQL mysql ;
    MysqlCoAndRes db ;
    MYSQL_ROW row ;
    char command[40] = "DESCRIBE " ;
    uint8_t col = 0 ;

    db.mysql = &mysql ;
    strcat(command, tName) ;
    reachMysql(&db, command) ;

    if (db.results == NULL) {
        return 0 ;
    }

    while ((row = mysql_fetch_row(db.results)) != NULL) {
        addLabel(grid, 0, col, *row) ;
        col++;
    }

    mysql_free_result(db.results);
    mysql_close(&mysql) ;

    return col ;
}

/*
Function : retrieveColData
-------------------------
Retrieves the content of the columns from mysql and adds them to the grid in labels
-------------------------
char *tName : selected table name
GtkWidget *grid : grid to which are attached the labels
-------------------------
Return values
    the number of rows in the table
    0 if there's no data in the table
*/
uint8_t retrieveColData (char *tName, GtkWidget *grid) {
    MYSQL mysql ;
    MysqlCoAndRes db ;
    MYSQL_ROW row ;
    char command[40] = "SELECT * FROM " ;
    uint8_t col = 0 ;
    uint8_t lin = 1 ;
    uint8_t kill ;

    strcat(command, tName) ;
    db.mysql = &mysql ;
    kill = reachMysql(&db, command) ;
    if (kill != 0)
        printMessage(NULL, 1, "") ;

    if (db.results == NULL) {
        return 0 ;
    }

    while ((row = mysql_fetch_row(db.results)) != NULL) {
        for (col = 0 ; col < db.nbFields ; col++) {
            addLabel(grid, lin, col, row[col]) ;
        }
        lin++ ;
    }

    reachMysql(&db, command) ;
    lin = countLin(db.results) ;

    mysql_free_result(db.results);
    mysql_close(&mysql) ;

    return lin;
}



/*
Function : alterTableWindow
-------------------------
Creates the window to alter the table
Calls for getColStructure to get the columns of the table and their values
Sends the data to alterTable
Can also call retrieveColNb to later add a certain number of columns to the table
-------------------------
GtkWidget *widget : widget sent by the callback function
char *tName : selected table name
-------------------------
*/
void alterTableWindow (GtkWidget *widget, char *tName) {
    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *alterButton ;
    GtkWidget *addColButton ;
    GtkWidget *nbColInput ;
    XMLdbData table ;
    GtkDualInputs di ;
    GtkWidget **dropButtons = NULL ;
    TableCol *colNames ;

    window = createWindow("Alter table", 1000, 600) ;
    grid = createGrid(window) ;

    strcpy(table.name, tName) ;
    di.window = window ;
    table.dualInputs = &di ;
    colNames = getColStructure(grid, &table, dropButtons) ;
    addLabel(grid, 0, 5, "Drop column") ;

    alterButton = gtk_button_new_with_label((const gchar *)"Alter") ;
    gtk_grid_attach(GTK_GRID(grid), alterButton, 5, table.size + 1, 1, 1) ;
    g_signal_connect(alterButton, "clicked", G_CALLBACK(alterTable), &table) ;

    table.colFunc = &addColumns ;

    nbColInput = gtk_entry_new() ;
    gtk_grid_attach(GTK_GRID(grid), nbColInput, 0, table.size + 2, 1, 1) ;
    di.nb = nbColInput ;

    addColButton = gtk_button_new_with_label((const gchar *)"Add columns") ;
    gtk_grid_attach(GTK_GRID(grid), addColButton, 1, table.size + 2, 1, 1) ;
    g_signal_connect(addColButton, "clicked", G_CALLBACK(retrieveColNb), &table) ;

    g_signal_connect(window, "destroy", G_CALLBACK(freePointer), dropButtons) ;
    g_signal_connect(window, "destroy", G_CALLBACK(freePointer), colNames) ;
    g_signal_connect(window, "destroy", G_CALLBACK(freePointer), table.columns) ;

    gtk_widget_show_all(window) ;
    gtk_main() ;
}

/*
Function : getColStructure
-------------------------
Gets the structure of each column from mysql (name of the column, type, etc.)
Calls for createSqlColInputs to create the inputs for each column of the table
Calls for setColEntries to set the input contents
Creates a TableCol array to store the column names
Creates the buttons to drop each column independently of the others
-------------------------
GtkWidget *widget : widget sent by the callback function
XMLdbData *table : data of the table
GtkWidget **dropButtons : buttons to drop each column
-------------------------
Return values
    The pointer to the TableCol array created
*/
TableCol * getColStructure (GtkWidget *grid, XMLdbData *table, GtkWidget **dropButtons) {
    MYSQL mysql ;
    MysqlCoAndRes db ;
    MYSQL_ROW row ;
    char command[30] = "DESCRIBE " ;
    uint8_t lin = 0 ;
    TableCol *colNames ;

    strcat(command, table->name) ;
    db.mysql = &mysql ;
    reachMysql(&db, command) ;
    if (db.results == NULL) {
        return NULL ;
    }

    table->size = countLin(db.results) ;
    table->columns = createSqlColInputs(table->size, grid) ;
    dropButtons = malloc(table->size * sizeof(GtkWidget *)) ;
    if (dropButtons == NULL)
        return NULL ;
    colNames = malloc(table->size * sizeof(TableCol)) ;
    if (colNames == NULL)
        return NULL ;

    reachMysql(&db, command) ;
    while ((row = mysql_fetch_row(db.results)) != NULL) {
        setColEntries(row, colNames[lin], table->columns[lin], table->name) ;
        colNames[lin].window = table->dualInputs->window ;

        dropButtons[lin] = gtk_button_new_with_label((const gchar *)"Drop") ;
        gtk_grid_attach(GTK_GRID(grid), dropButtons[lin], 5, lin + 1, 1, 1) ;
        g_signal_connect(dropButtons[lin], "clicked", G_CALLBACK(dropColumn), &colNames[lin]) ;
        lin++ ;
    }

    mysql_free_result(db.results);
    mysql_close(&mysql) ;
    return colNames;
}

/*
Function : setColEntries
-------------------------
Sets the values of the inputs in a GtkColumn by getting the associated row data
-------------------------
MYSQL_ROW row : mysql row from which the data is retrieved
GtkColumn col : structure of inputs in which the data is set
char *tName : table name
-------------------------
*/
void setColEntries (MYSQL_ROW row, TableCol colNames, GtkColumn col, char *tName) {
    gtk_entry_set_text(GTK_ENTRY(col.name), row[0]) ;
    strcpy(colNames.cName, row[0]) ;
    strcpy(colNames.tName, tName) ;
    gtk_entry_set_text(GTK_ENTRY(col.size), row[1]) ;
    if (row[4] != NULL)
        gtk_entry_set_text(GTK_ENTRY(col.def), row[4]) ;
    if (row[5] != NULL)
        gtk_entry_set_text(GTK_ENTRY(col.constraints), row[5]) ;
    if (row[3] != NULL && strcmp(row[3], "PRI") == 0)
        gtk_combo_box_set_active_id(GTK_COMBO_BOX(col.primKey), "yes") ;
}

/*
Function : retrieveColNb
-------------------------
Retrieves the number of columns in a table from an input
-------------------------
GtkWidget *widget : widget sent by the callback function
XMLdbData *table : table data
-------------------------
*/
void retrieveColNb (GtkWidget *widget, XMLdbData *table) {
    int16_t nbCol ;
    retrieveInteger(widget, table->dualInputs->nb, &nbCol) ;
    if (nbCol <= 0)
        return ;
    table->size = nbCol ;
    closeWindow(NULL, table->dualInputs->window) ;
    getTableColumns(widget, table) ;
}

/*
Function : addColumn
-------------------------
Creates a mysql command to alter add a column to a table
-------------------------
GtkWidget *widget : widget sent by the callback function
XMLdbData *table : table data
-------------------------
Return values
    0 if all went well
    the error code sent by the database
*/
uint8_t addColumns (GtkWidget *widget, XMLdbData *table) {
    uint8_t nbCol = table->size ;
    uint8_t i ;
    char aiName[30] = "" ;
    char pk[100] = "" ;
    char command[250] ;
    uint8_t kill ;

    for (i = 0 ; i < nbCol ; ++i) {
        strcpy(command, "ALTER TABLE ") ;
        addSpace(strcat(command, table->name)) ;
        strcat(command, "ADD ") ;
        addColumnCommand(widget, command, table->columns[i], pk) ;
        retrieveAI(widget, table->columns[i], aiName) ;
        removeLastChar(command) ;
        kill = connectDB(command) ;
        if (kill != 0) {
            printMessage(widget, kill, "") ;
            return kill ;
        }
    }

    if (strlen(pk) != 0)
        retrievePrimKeys(widget, pk, table->name, aiName) ;

    freePointer(widget, table->columns) ;
    closeWindow(NULL, table->dualInputs->window) ;
    printMessage(widget, 0, "Columns added !") ;
    return 0 ;
}


void dropVerify (GtkWidget *widget, char *tName) {
    GtkWidget *window ;
    GtkWidget *grid ;
    GtkWidget *yes ;
    GtkWidget *no ;
    GtkWidget *label ;
    char str[50] = "Dropping table " ;

    strcat(str, tName) ;

    window = createWindow(str, 200, 200) ;
    grid = createGrid(window) ;

    label = gtk_label_new("Are you sure ?") ;
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 2, 1) ;

    yes = gtk_button_new_with_label((const gchar *)"yes") ;
    gtk_grid_attach(GTK_GRID(grid), yes, 0, 1, 1, 1) ;
    g_signal_connect(yes, "clicked", G_CALLBACK(closeWindow), window) ;
    g_signal_connect(yes, "clicked", G_CALLBACK(dropTable), tName) ;

    no = gtk_button_new_with_label((const gchar *)"no") ;
    gtk_grid_attach(GTK_GRID(grid), no, 1, 1, 1, 1) ;
    g_signal_connect(no, "clicked", G_CALLBACK(closeWindow), window) ;

    gtk_widget_show_all(window);
    gtk_main();
}

void dropTable (GtkWidget *widget, char *tName) {
    char command[50] = "DROP TABLE ";
    size_t kill ;
    strcat(command, tName) ;
    kill = connectDB(command) ;
    if (kill != 0)
        printMessage(NULL, 0, "An error occurred") ;
    else
        printMessage(NULL, 0, "Table dropped !") ;
}

void inputData (GtkWidget *widget, char *tName) {
    GtkBuilder *builder;
    GtkWidget *okButton ;
    GtkWidget *input ;
    GtkDualInputs tInput ;
    Inserts tData ;

    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "main.glade", NULL);

    tInput.window = GTK_WIDGET(gtk_builder_get_object(builder, "windowInput"));
    gtk_builder_connect_signals(builder, NULL);
    backgroundColor(tInput.window, "#999999" );

    strcpy(tData.name, tName) ;

    input = GTK_WIDGET(gtk_builder_get_object(builder, "nbRows")) ;
    tInput.nb = input ;
    tData.dualInputs = &tInput ;
    okButton = GTK_WIDGET(gtk_builder_get_object(builder, "validateInput")) ;
    g_signal_connect(okButton, "clicked", G_CALLBACK(inputDataWindow), &tData);

    g_object_unref(builder);
    gtk_widget_show_all(tInput.window);
    gtk_main();
}

void inputDataWindow (GtkWidget *widget, Inserts *table) {
    GtkWidget *window ;
    GtkWidget *grid ;
    GtkWidget *button ;
    GtkWidget *label ;
    int16_t nbRows ;
    char str[50] = "Input into table " ;

    strcat(str, table->name) ;
    retrieveInteger(widget, table->dualInputs->nb, &nbRows) ;
    if (nbRows <= 0)
        return ;
    table->nbRows = nbRows ;
    closeWindow(NULL, table->dualInputs->window) ;

    window = createWindow(str, 600, 200) ;
    grid = createGrid(window) ;
    table->dualInputs->window = window ;

    table->nbCols = retrieveColNames(table->name, grid) ;
    createInsertInputs(table, grid) ;

    button = gtk_button_new_with_label((const gchar *)"Insert") ;
    gtk_grid_attach(GTK_GRID(grid), button, 0, nbRows+2, 1, 1) ;
    g_signal_connect(button, "clicked", G_CALLBACK(insertData), table) ;

    label = gtk_label_new("Don't forget the quotes around char/varchar/text/date(time) values !") ;
    gtk_grid_attach(GTK_GRID(grid), label, 0, nbRows+3, 2, 1) ;

    g_signal_connect(window, "destroy", G_CALLBACK(freeInserts), table) ;

    gtk_widget_show_all(window) ;
    gtk_main() ;
}

void insertData (GtkWidget *widget, Inserts *table) {
    char command[500] ;
    char *data ;
    size_t i ;
    size_t j ;
    size_t kill ;

    for (i = 0 ; i < table->nbRows ; ++i) {
        strcat(strcat(strcpy(command, "INSERT INTO "), table->name), " VALUES (") ;
        for (j = 0 ; j < table->nbCols ; ++j) {
            retrieveData(widget, table->inputs[i][j], &data) ;
            if (strlen(data) != 0)
                strcat(strcat(command, data), ",") ;
            else
                strcat(strcat(command, "NULL"), ",") ;
        }
        removeLastChar(command) ;
        strcat(command, ")") ;
        kill = connectDB(command) ;
        if (kill != 0) {
            printMessage(NULL, 0, "Check your values") ;
            return ;
        }
    }
    freeInserts(NULL, table) ;
    closeWindow(NULL, table->dualInputs->window) ;
    printMessage(NULL, 0, "Data inserted !") ;
}

void freePointer (GtkWidget *widget, void *ptr) {
    if (ptr != NULL) {
        free(ptr) ;
        ptr = NULL ;
    }
}

void freeInserts (GtkWidget *widget, Inserts *inserts) {
    uint8_t i ;
    for (i = 0 ; i < inserts->nbRows ; ++i) {
        freePointer(NULL, inserts->inputs[i]) ;
    }
    freePointer(NULL, inserts->inputs) ;
}


void freeTableData (GtkWidget *widget, XMLdbData *xmlData) {
    freePointer(widget, xmlData->conf) ;
    if (xmlData->doc != NULL)
        xmlFreeDoc(xmlData->doc) ;
}

void createInsertInputs(Inserts *table, GtkWidget *grid) {
    uint8_t i ;
    uint8_t j ;

    table->inputs = malloc(sizeof(GtkWidget **) * table->nbRows) ;
    if (table->inputs == NULL) {
        printMessage(NULL, ERR_MEM, "") ;
    }
    for (i = 0 ; i < table->nbRows ; ++i) {
        table->inputs[i] = malloc(sizeof(GtkWidget *) * table->nbCols) ;
        if (table->inputs[i] == NULL) {
            printMessage(NULL, ERR_MEM, "") ;
        }

        for (j = 0 ; j < table->nbCols ; ++j) {
            table->inputs[i][j] = gtk_entry_new() ;
            gtk_grid_attach(GTK_GRID(grid), table->inputs[i][j], j, i+1, 1, 1) ;
        }
    }
}

void inputCommand (GtkWidget *widget) {
    GtkBuilder *builder;
    GtkWidget *window;
    GtkWidget *button ;
    GtkWidget *input ;

    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "main.glade", NULL);

    window = GTK_WIDGET(gtk_builder_get_object(builder, "windowSql"));
    gtk_builder_connect_signals(builder, NULL);
    backgroundColor(window, "#999999" );

    input = GTK_WIDGET(gtk_builder_get_object(builder, "sqlInput")) ;
    button = GTK_WIDGET(gtk_builder_get_object(builder, "sqlSend")) ;
    g_signal_connect(button, "clicked", G_CALLBACK(getCommand), input);

    g_object_unref(builder);
    gtk_widget_show_all(window);
    gtk_main();
}

void getCommand (GtkWidget *widget, GtkWidget *input) {
    char *command ;
    size_t kill ;
    retrieveData(widget, input, &command) ;
    kill = connectDB(command) ;
}

void dropColumn (GtkWidget *widget, TableCol *drop) {
    char command[100] = "ALTER TABLE ";
    uint8_t deleted ;
    strcat(strcat(strcat(command, drop->tName), " DROP "), drop->cName) ;

    if ((deleted = connectDB(command)) != 0) {
        printMessage(widget, 0, "Can't drop this column, bound to a primary key") ;
    }
    else {
        printMessage(widget, 0, "Column dropped") ;
        closeWindow(NULL, drop->window) ;
    }

}

void alterTable (GtkWidget *widget, XMLdbData *tableData) {
    char command[100] = "DESCRIBE " ;
    MYSQL mysql ;
    MysqlCoAndRes db ;
    MYSQL_ROW row ;

    uint8_t kill ;
    uint8_t i = 0 ;
    uint8_t modified = 0 ;
    uint8_t mod ;

    db.mysql = &mysql ;
    strcat(command, tableData->name) ;
    reachMysql(&db, command) ;

    modified += managePrimKeys(widget, db.results, tableData) ;

    reachMysql(&db, command) ;
    while ((row = mysql_fetch_row(db.results)) != NULL) {
        mod = checkIfModified(widget, tableData, i, command, row) ;
        if (mod == 1) {
            kill = connectDB(command) ;
            if (kill != 0)
                printMessage(widget, 0, "Data syntax error") ;
            modified += 1 ;
        }
        i++ ;
    }

    if (modified != 0) {
        freePointer(widget, tableData->columns) ;
        closeWindow(NULL, tableData->dualInputs->window) ;
        printMessage(widget, 0, "Table altered") ;
    }

    mysql_free_result(db.results);
    mysql_close(&mysql) ;
}

uint8_t checkIfModified (GtkWidget *widget, XMLdbData *tableData, uint8_t i, char *command, MYSQL_ROW row) {
    uint8_t modified = 0 ;
    char *newName ;
    char *type ;
    char *extra ;
    char *def ;

    retrieveData(widget, tableData->columns[i].name, &newName) ;
    retrieveData(widget, tableData->columns[i].size, &type) ;
    retrieveData(widget, tableData->columns[i].def, &def) ;
    retrieveData(widget, tableData->columns[i].constraints, &extra) ;

    strcat(strcpy(command, "ALTER TABLE "), tableData->name) ;
    strcat(strcat(strcat(strcat(strcat(strcat(command, " CHANGE "), row[0]), " "), newName), " "), type) ;

    if (strcmp(row[0], newName) != 0 || strcmp(row[1], type) != 0)
        modified = 1 ;
    if (strcmp(row[5], extra) != 0) {
        strcat(strcat(command, " "), extra) ;
        modified = 1 ;
    }

    if ((row[4] == NULL && strlen(def) != 0) || (row[4] != NULL && strcmp(row[4], def) != 0)) {
        if ((strncmp(type, "char", 4) == 0 || strncmp(type, "varchar", 7) == 0) && strcmp(def, "null") != 0) {
            strcat(strcat(strcat(command, " DEFAULT \""), def), "\"") ;
        } else {
            strcat(strcat(command, " DEFAULT "), def) ;
        }
        modified = 1 ;
    }
    return modified ;
}
/*
Function : retrievePrimKeys
-------------------------
call from addColumns();

-------------------------
GtkWidget *widget: widget sent by the callback function
char *pk: contain the column name of the prim key
char *tName: contain the of the table
char *newAi: contain the column name that have auto_increment constraint
-------------------------
return uint8_t
*/
void retrievePrimKeys (GtkWidget *widget, char *pk, char *tName, char *newAi) {
    char command[100] = "DESCRIBE " ;
    char ai[30] = "" ;
    MYSQL mysql ;
    MysqlCoAndRes db ;
    MYSQL_ROW row ;

    db.mysql = &mysql ;
    strcat(command, tName) ;
    reachMysql(&db, command) ;

    while ((row = mysql_fetch_row(db.results)) != NULL) {
        if (strcmp(row[3], "PRI") == 0) {
            strcat(strcat(pk, row[0]), ",") ;
        }
        if (strcmp(row[5], "auto_increment") == 0) {
            strcpy(ai, row[0]) ;
        }
    }

    if (strlen(ai) != 0 && strlen(newAi) != 0) {
        printMessage(widget, 0, "The auto_increment constraint can't be set (multiple primary keys)") ;
    }

    if (pk[strlen(pk) - 1] == ',') {
        removeLastChar(pk) ;
    }

    dropAddPrimKeys(pk, tName) ;
    mysql_free_result(db.results);
    mysql_close(&mysql) ;
}
/*
Function : managePrimKeys
-------------------------
call from alterTable()
Permit to compare the comboBox value of prim Key with the database
Call unsetAI and dropAddPrimKeys if there is a diffrence
-------------------------
GtkWidget *widget : widget sent by the callback function
MYSQL_RES *results : contains the result of the query send by alterTable
XMLdbData *table : struct that contains the table data
-------------------------
return uint8_t
*/
uint8_t managePrimKeys (GtkWidget *widget, MYSQL_RES *results, XMLdbData *table) {
    MYSQL_ROW row ;
    char *pk ;
    uint8_t i = 0 ;
    uint8_t diff = 0 ;
    char primKeys[50] = "" ;
    char ai[30] = "" ;

    while ((row = mysql_fetch_row(results)) != NULL) {
        pk = NULL ;
        retrieveComboBoxContent(widget, table->columns[i].primKey, &pk) ;
        if (pk != NULL) {
            if (strcmp(pk, "YES") == 0) {  //if is being set as primary key
                strcat(strcat(primKeys, row[0]), ",") ;
                if (strcmp(row[5], "auto_increment") == 0) //if has an AI constraint
                    strcpy(ai, row[0]) ;

                if (strcmp(row[3], "PRI") != 0) //if wasn't already set as primary key
                    diff = 1 ;
            } else { //if is not being set as primary key
                if (strcmp(row[5], "auto_increment") == 0) //if has an AI constraint
                    strcpy(ai, row[0]) ;

                if (strcmp(row[3], "PRI") == 0) //if was set as primary key
                    diff = 1 ;
            }
        }
        i++ ;
    }
    removeLastChar(primKeys) ;
    if (strlen(primKeys) == 0)
        return 0 ;

    if (diff == 1) {
        unsetAI(ai, table->name) ;
        dropAddPrimKeys(primKeys, table->name) ;
        if (strlen(ai) != 0 && strchr(primKeys, ',') != NULL)
            printMessage(widget, 0, "The auto_increment constraint can't be set (multiple primary keys)") ;
    }
    return 1 ;
}
/*
Function : unsetAI
-------------------------
call from managePrimKeys()
Allows us to drop the auto_increment
-------------------------
char *ai: column name
char *tName: the name of the table
-------------------------
*/
void unsetAI (char *ai, char *tName) {
    char unsetAI[100] = "ALTER TABLE " ;
    strcat(unsetAI, tName) ;
    if (strlen(ai) != 0) {
        strcat(strcat(strcat(strcat(strcat(unsetAI, " CHANGE "), ai), " "), ai), " INTEGER") ;
        connectDB(unsetAI) ;
    }
}
/*
Function : dropAddPrimKeys
-------------------------
Allows us to drop the primary keys and add it
-------------------------
char *primKeys: take the name of the primary key
char *tName: the name of the table
-------------------------
send data to connectDB

*/
void dropAddPrimKeys (char *primKeys, char *tName) {
    char str[50] = "ALTER TABLE " ;
    char add[100] = "" ;
    char drop[100] = "" ;

    strcat(str, tName) ;
    strcat(strcpy(drop, str), " DROP PRIMARY KEY") ;
    strcat(strcat(strcat(strcpy(add, str), " ADD PRIMARY KEY("), primKeys), ")") ;

    connectDB(drop) ;
    connectDB(add) ;
}
/*
Function : countLin
-------------------------
call from getColStructure(), retrieveColData
count the lines of the results of mysql query
-------------------------
MYSQL_RES *res: the result of mysql query
-------------------------

*/
uint8_t countLin (MYSQL_RES *res) {
    uint8_t count = 0 ;
    MYSQL_ROW row ;
    while ((row = mysql_fetch_row(res)) != NULL)
        count++;
    return count ;
}

/*
Function : createXmlColInputs
-------------------------
create inputs that we need into the window while we create new table
-------------------------
uint8_t nbLin: number of labels that we need
GtkWidget *grid: take the grid of the window to attach the inputs
-------------------------
return columns

*/
GtkColumn * createXmlColInputs (uint8_t nbLin, GtkWidget *grid) {
    GtkColumn *columns ;
    uint8_t i ;

    columns = malloc(sizeof(GtkColumn) * nbLin) ;
    if (columns == NULL)
        return NULL ;

    createXmlColLabels(grid) ;

    for (i = 0 ; i < nbLin ; ++i) {
        columns[i].name = createInput("Name") ;
        gtk_grid_attach(GTK_GRID(grid), columns[i].name, 0, i+1, 1, 1) ;
        columns[i].type = createComboBoxType() ;
        gtk_grid_attach(GTK_GRID(grid), columns[i].type, 1, i+1, 1, 1) ;
        columns[i].size = createInput("Size") ;
        gtk_grid_attach(GTK_GRID(grid), columns[i].size, 2, i+1, 1, 1) ;
        columns[i].constraints = createInput("auto_increment, not null, etc.") ;
        gtk_grid_attach(GTK_GRID(grid), columns[i].constraints, 3, i+1, 1, 1) ;
        columns[i].check = createInput("Check") ;
        gtk_grid_attach(GTK_GRID(grid), columns[i].check, 4, i+1, 1, 1) ;
        columns[i].def = createInput("null") ;
        gtk_grid_attach(GTK_GRID(grid), columns[i].def, 5, i+1, 1, 1) ;
        columns[i].primKey = createComboBoxYN();
        gtk_grid_attach(GTK_GRID(grid), columns[i].primKey, 6, i+1, 1, 1) ;
        columns[i].ref = createInput("table(col)") ;
        gtk_grid_attach(GTK_GRID(grid), columns[i].ref, 7, i+1, 1, 1) ;
        columns[i].refd = createComboBoxYN() ;
        gtk_grid_attach(GTK_GRID(grid), columns[i].refd, 8, i+1, 1, 1) ;
    }

    return columns ;
}
/*
Function : createXmlColLabels
-------------------------
call from createXmlColInputs
create label that we need into the window while we create new table
-------------------------
GtkWidget *grid: take the grid of the window to attach the label
-------------------------
call addLabel()
*/
void createXmlColLabels (GtkWidget *grid) {
    addLabel(grid, 0, 0, "Column name") ;
    addLabel(grid, 0, 1, "Type") ;
    addLabel(grid, 0, 2, "Size") ;
    addLabel(grid, 0, 3, "Constraints") ;
    addLabel(grid, 0, 4, "Check") ;
    addLabel(grid, 0, 5, "Default") ;
    addLabel(grid, 0, 6, "Primary key") ;
    addLabel(grid, 0, 7, "References") ;
    addLabel(grid, 0, 8, "Is referenced") ;
}
/*
Function : createSqlColInputs
-------------------------
call form getColStructure()
create the number of inputs that we need into the alter table window
-------------------------
uint8_t nbCol: number of inputs that we need
GtkWidget *grid: take the grid of the window to attach the inputs
-------------------------
return columns
call createSqlColLabels(grid);
*/
GtkColumn * createSqlColInputs (uint8_t nbLin, GtkWidget *grid) {
    GtkColumn *columns ;
    uint8_t i ;

    columns = malloc(sizeof(GtkColumn) * nbLin) ;
    if (columns == NULL)
        return NULL ;

    createSqlColLabels(grid) ;

    for (i = 0 ; i < nbLin ; ++i) {
        columns[i].name = createInput("Name") ;
        gtk_grid_attach(GTK_GRID(grid), columns[i].name, 0, i+1, 1, 1) ;
        columns[i].size = createInput("Size") ;
        gtk_grid_attach(GTK_GRID(grid), columns[i].size, 1, i+1, 1, 1) ;
        columns[i].constraints = createInput("auto_increment, not null, etc.") ;
        gtk_grid_attach(GTK_GRID(grid), columns[i].constraints, 2, i+1, 1, 1) ;
        columns[i].def = createInput("null") ;
        gtk_grid_attach(GTK_GRID(grid), columns[i].def, 3, i+1, 1, 1) ;
        columns[i].primKey = createComboBoxYN();
        gtk_grid_attach(GTK_GRID(grid), columns[i].primKey, 4, i+1, 1, 1) ;
        columns[i].ref = createInput("table(col)") ;
    }

    return columns ;
}
/*
Function : createSqlColLabels
-------------------------
create labels that we need into the alter table window
-------------------------
GtkWidget *grid: take the grid of the window to attach the label
-------------------------

*/
void createSqlColLabels (GtkWidget *grid) {
    addLabel(grid, 0, 0, "Column name") ;
    addLabel(grid, 0, 1, "Type") ;
    addLabel(grid, 0, 2, "Extra") ;
    addLabel(grid, 0, 3, "Default") ;
    addLabel(grid, 0, 4, "Primary key") ;
}

/*
Function : createComboBoxType
-------------------------
call from createXmlColInputs
create the comboBox and put all type that we want inside it
-------------------------
return box: the comboBox
*/
GtkWidget * createComboBoxType (void) {
    GtkWidget *box ;

    box = gtk_combo_box_text_new() ;
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(box), "integer", "INTEGER") ;
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(box), "float", "FLOAT") ;
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(box), "double", "DOUBLE") ;
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(box), "char", "CHAR") ;
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(box), "varchar", "VARCHAR") ;
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(box), "text", "TEXT") ;
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(box), "timestamp", "TIMESTAMP") ;
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(box), "date", "DATE") ;
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(box), "datetime", "DATETIME") ;

    return box ;
}
/*
Function : createComboBoxYN
-------------------------
create the comboBox and put YES and NO inside it
-------------------------
return box: the comboBox
*/
GtkWidget * createComboBoxYN (void) {
    GtkWidget *box ;

    box = gtk_combo_box_text_new() ;
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(box), "yes", "YES") ;
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(box), "no", "NO") ;

    return box ;
}
/*
Function : createComboBoxDBQuery
-------------------------
call from dbManagerWindow
create the comboBox and put the names of databases in this comboBox
-------------------------
return box: the comboBox
call reachMysql(&db, command)
*/
GtkWidget * createComboBoxDBQuery () {
    GtkWidget *box ;
    MYSQL mysql ;
    MysqlCoAndRes db ;
    MYSQL_ROW row ;
    char *command = "SHOW DATABASES" ;

    strcpy(nameDB, "") ;
    box = gtk_combo_box_text_new() ;
    db.mysql = &mysql ;
    reachMysql(&db, command) ;

    if (db.results == NULL) {
        return box ;
    }

    while ((row = mysql_fetch_row(db.results)) != NULL)
        gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(box), *row, *row) ;

    mysql_free_result(db.results);
    mysql_close(&mysql) ;

    return box ;
}

/*
Function : createComboBoxTables
-------------------------
call from showTables
create the comboBox and put the names of tables in this comboBox
-------------------------
void
-------------------------
return the comboBox
*/
GtkWidget * createComboBoxTables (void) {
    GtkWidget *box ;
    MYSQL mysql ;
    MysqlCoAndRes db ;
    MYSQL_ROW row ;
    char command[40] = "USE " ;

    strcat(command, nameDB) ;
    box = gtk_combo_box_text_new() ;
    db.mysql = &mysql ;
    reachMysql(&db, command) ;
    reachMysql(&db, "SHOW TABLES") ;

    if (db.results == NULL) {
        return box ;
    }

    while ((row = mysql_fetch_row(db.results)) != NULL)
        gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(box), *row, *row) ;

    mysql_free_result(db.results);
    mysql_close(&mysql) ;

    return box ;
}



/*
Function : retrieveData
-------------------------
retrive value from an input
-------------------------
GtkWidget *widget : widget
GtkWidget *input: the GtkWidget that we want to retrieve the value
char **str: store our result
-------------------------
*/
void retrieveData(GtkWidget *widget, GtkWidget *input, char **str) {
    if (input == NULL)
        return ;
    if (gtk_entry_get_text (GTK_ENTRY (input)) != NULL)
       *str = (char *)gtk_entry_get_text (GTK_ENTRY (input));
    else
        strcpy(*str, "") ;
}

/*
Function : retrieveComboBoxContent
-------------------------
retrive value for a comboBox
-------------------------
GtkWidget *widget : widget
GtkWidget *box: the GtkWidget that we want to retrieve the value
char **str: store our result
-------------------------
*/
void retrieveComboBoxContent (GtkWidget *widget, GtkWidget *box, char **str) {
    if (box == NULL)
        return ;
    if (gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(box)) != NULL)
        *str = (char *)gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(box)) ;
    else
        *str = NULL ;
}
