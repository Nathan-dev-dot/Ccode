/*
    Nathan Letourneau & Sarah Schlegel
    01.11.2020
    Project library with GTK functions
*/

// gcc `pkg-config --cflags gtk+-3.0` main.c -o main `pkg-config --libs gtk+-3.0`


#include "all.h"

extern char nameDB[30] ;

void hello (GtkWidget *widget, gpointer data) {
    printf("Hello %p\n", data) ;
}



/*
Function : printError                                   ///comm à modifier
-------------------------
Prints the errors reading from the config file

int errorNo : error number given by the previous functions
*/
void printError (GtkWidget *widget, int errNo, char *message) {
    FILE *confFile ;
    char str[150] ;
    int errCode ;
    int found = 0;
    GtkBuilder *builder;
    GtkWidget *window;
    GtkWidget *label ;

    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "main.glade", NULL);

    window = GTK_WIDGET(gtk_builder_get_object(builder, "window_popup"));
    gtk_builder_connect_signals(builder, NULL);
    gtk_window_set_default_size (GTK_WINDOW (window), 200, 100);

    label = GTK_WIDGET(gtk_builder_get_object(builder, "label"));
    if (strlen(message) == 0) {
        confFile = fopen("../config", "r") ;
        if (confFile == NULL){
            gtk_label_set_text(GTK_LABEL(label), (const gchar *)"Error in opening the configuration file") ;
        }

        while (fgets(str, 150, confFile) != NULL) {
            if (strcmp(str, "[error_codes]\n") == 0) {
                while (fgets(str, 150, confFile) != NULL && strcmp(str, "\n") != 0) {
                    sscanf(str, "%d", &errCode) ;
                    if (errCode == errNo) {
                        found = 1 ;
                        gtk_label_set_text(GTK_LABEL(label), (const gchar *)strchr(str, ':') + 1) ;
                        break ;
                    }
                }
            }
        }
        fclose(confFile) ;

        if (found == 0)
            gtk_label_set_text(GTK_LABEL(label), (const gchar *)"Error code not found") ;
    } else {
        gtk_label_set_text(GTK_LABEL(label), (const gchar *)message) ;
    }

    background_color(window, "#999999" );
    mainMenu(builder, window) ;

    g_object_unref(builder);

    gtk_widget_show(window);
    gtk_main();
}

/*
*/
void destroy (GtkWidget *widget, gpointer data) {
    gtk_main_quit();
}

/*
*/
void closeWindow (GtkWidget *window) {
    gtk_window_close(GTK_WINDOW(window)) ;
}

/*
*/
int initProg (int argc, char **argv) {
    GtkBuilder *builder;
    GtkWidget  *window;

    gtk_init(&argc, &argv);

    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "main.glade", NULL);

    window = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));
    gtk_builder_connect_signals(builder, NULL);
    g_signal_connect(window, "delete_event", G_CALLBACK(destroy), NULL) ;

    background_color(window, "#999999" );
    mainMenu(builder, window) ;

    g_object_unref(builder);

    gtk_widget_show(window);
    gtk_main();

    return 0;
}

/*
*/
void mainMenu (GtkBuilder *builder, GtkWidget *window) {
    GtkWidget *option1;
    GtkWidget *option2;
    GtkWidget *option3;

    option1 = GTK_WIDGET(gtk_builder_get_object(builder, "dbFromXml"));
    g_signal_connect(option1, "clicked", G_CALLBACK(dbFromXMLWindow), NULL);

    option2 = GTK_WIDGET(gtk_builder_get_object(builder, "createXml"));
    g_signal_connect(option2, "clicked", G_CALLBACK(xmlFromEntries), NULL);

    option3 = GTK_WIDGET(gtk_builder_get_object(builder, "manageDatabase"));
    g_signal_connect(option3, "clicked", G_CALLBACK(dbManagerWindow), NULL);
}

/*
*/
void dbFromXMLWindow (GtkWidget *widget) {
    GtkBuilder *builder;
    GtkWidget *window;
    GtkWidget *okButton ;
    GtkWidget *input ;

    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "main.glade", NULL);

    window = GTK_WIDGET(gtk_builder_get_object(builder, "window_xml"));
    gtk_builder_connect_signals(builder, NULL);

    input = GTK_WIDGET(gtk_builder_get_object(builder, "XMLPath")) ;
    okButton = GTK_WIDGET(gtk_builder_get_object(builder, "validate")) ;
    g_signal_connect(okButton, "clicked", G_CALLBACK(dbFromXML), input);

    background_color(window, "#999999" );

    g_object_unref(builder);

    gtk_widget_show(window);
    gtk_main();
}

/*
*/
void xmlFromEntries (GtkWidget *widget) {
    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *button ;
    GtkWidget *nbLabel ;
    GtkWidget *dbLabel ;
    GtkDualInputs dbParams;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), (const gchar *)"Create an XML file") ;
    gtk_window_set_default_size (GTK_WINDOW (window), 200, 200);
    dbParams.window = window ;

    grid = gtk_grid_new() ;
    gtk_container_add(GTK_CONTAINER(window), grid) ;
    g_object_set(grid, "margin", 12, NULL);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10) ;


    dbLabel = gtk_label_new((const gchar *)"Enter the database name") ;
    gtk_grid_attach(GTK_GRID(grid), dbLabel, 0, 0, 1, 1) ;
    dbParams.name = createInput("Database name") ;
    gtk_grid_attach(GTK_GRID(grid), dbParams.name, 0, 2, 1, 1) ;

    nbLabel = gtk_label_new((const gchar *)"Enter the number of tables") ;
    gtk_grid_attach(GTK_GRID(grid), nbLabel, 0, 3, 1, 1) ;
    dbParams.nb = createInput("Number") ;
    gtk_grid_attach(GTK_GRID(grid), dbParams.nb, 0, 4, 1, 1) ;

    button = gtk_button_new_with_label((const gchar *)"Send") ;
    g_signal_connect (button, "clicked", G_CALLBACK (createXMLFile), &dbParams);
    gtk_grid_attach(GTK_GRID(grid), button, 0, 6, 1, 1) ;

    background_color(window, "#999999") ;
    gtk_widget_show_all(window) ;
    gtk_main() ;
}

/*
*/
void writeTables (GtkWidget *widget, XMLdbData *xmlData) {
    xmlData->pos.current++ ;
    if (xmlData->pos.current <= xmlData->pos.total) {
        tableData(widget, xmlData) ;
    } else {
        printError(widget, 0, "XML file created in Outputs folder") ;
        xmlFreeDoc(xmlData->doc) ;
    }
    return ;
}

/*
*/
void tableData (GtkWidget *widget, XMLdbData *dbData) {
    GtkWidget *window ;
    GtkWidget *grid;
    GtkWidget *button ;
    GtkWidget *nbLabel ;
    GtkWidget *tLabel ;
    GtkDualInputs dbParams;

    if (dbData->dualInputs->window != NULL)
        closeWindow(dbData->dualInputs->window) ;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), (const gchar *)"Table data") ;
    gtk_window_set_default_size (GTK_WINDOW (window), 200, 200);

    dbParams.window = window ;

    grid = gtk_grid_new() ;
    gtk_container_add(GTK_CONTAINER(window), grid) ;
    g_object_set(grid, "margin", 12, NULL);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10) ;

    tLabel = gtk_label_new((const gchar *)"Enter the table name") ;
    gtk_grid_attach(GTK_GRID(grid), tLabel, 0, 0, 1, 1) ;
    dbParams.name = createInput("Database name") ;
    gtk_grid_attach(GTK_GRID(grid), dbParams.name, 0, 2, 1, 1) ;

    nbLabel = gtk_label_new((const gchar *)"Enter the number of columns") ;
    gtk_grid_attach(GTK_GRID(grid), nbLabel, 0, 3, 1, 1) ;
    dbParams.nb = createInput("Number") ;
    gtk_grid_attach(GTK_GRID(grid), dbParams.nb, 0, 4, 1, 1) ;

    dbData->dualInputs = &dbParams ;

    button = gtk_button_new_with_label((const gchar *)"Send") ;
    g_signal_connect (button, "clicked", G_CALLBACK (setTableData), dbData);
    gtk_grid_attach(GTK_GRID(grid), button, 0, 6, 1, 1) ;

    background_color(window, "#999999") ;
    gtk_widget_show_all(window) ;
    gtk_main() ;
}

/*
*/
void getTableColumns (GtkWidget *widget, XMLdbData *dbData) {
    GtkWidget *window ;
    GtkWidget *grid ;
    GtkWidget *button ;
    char tmp[30] = "Table " ;

    strcat(tmp, dbData->name) ;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), (const gchar *)tmp) ;
    gtk_window_set_default_size (GTK_WINDOW (window), 1500, 200 + 50 * dbData->size);

    grid = gtk_grid_new() ;
    gtk_container_add(GTK_CONTAINER(window), grid) ;
    g_object_set(grid, "margin", 12, NULL);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10) ;
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10) ;

    dbData->columns = createXmlColInputs(dbData->size, grid) ;
    dbData->dualInputs->window = window ;

    button = gtk_button_new_with_label((const gchar *)"Send") ;
    gtk_grid_attach(GTK_GRID(grid), button, 8, dbData->size + 1, 1, 1) ;
    g_signal_connect(button, "clicked", G_CALLBACK(dbData->colFunc), dbData) ;

    background_color(window, "#999999") ;
    gtk_widget_show_all(window) ;
    gtk_main() ;
}

/*
*/
void dbManagerWindow (GtkWidget *widget) {
    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *button ;
    GtkWidget *combo ;
    GtkWidget *label ;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), (const gchar *)"Database") ;
    gtk_window_set_default_size (GTK_WINDOW (window), 200, 200);

    grid = gtk_grid_new() ;
    gtk_container_add(GTK_CONTAINER(window), grid) ;
    g_object_set(grid, "margin", 12, NULL);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10) ;

    label = gtk_label_new((const gchar *)"Select your database") ;
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1) ;

    combo = createComboBoxDBQuery();
    gtk_grid_attach(GTK_GRID(grid), combo, 0, 1, 1, 1) ;

    button = gtk_button_new_with_label((const gchar *)"Select") ;
    g_signal_connect (button, "clicked", G_CALLBACK (setDBName), combo);
    gtk_grid_attach(GTK_GRID(grid), button, 0, 2, 1, 1) ;

    background_color(window, "#999999") ;
    gtk_widget_show_all(window) ;
    gtk_main() ;

    return ;
}

void setDBName (GtkWidget *widget, GtkWidget *comboBox) {
    char *tmp = "" ;
    retrieveComboBoxContent(widget, comboBox, &tmp) ;
    strcpy(nameDB, tmp) ;
    showTables(widget) ;
}

void showTables (GtkWidget *widget) {
    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *selectButton ;
    GtkWidget *addButton ;
    GtkWidget *combo ;
    GtkWidget *label ;
    char str[50] = "Using db " ;
    XMLdbData table ;
    GtkDualInputs di ;

    table.colFunc = &addTable ;

    strcat(str, nameDB) ;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), (const gchar *)str) ;
    gtk_window_set_default_size (GTK_WINDOW (window), 200, 200);

    di.window = window ;
    table.dualInputs = &di ;

    grid = gtk_grid_new() ;
    gtk_container_add(GTK_CONTAINER(window), grid) ;
    g_object_set(grid, "margin", 20, NULL);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10) ;

    label = gtk_label_new((const gchar *)"Select your table") ;
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1) ;

    combo = createComboBoxTables();
    gtk_grid_attach(GTK_GRID(grid), combo, 0, 1, 1, 1) ;

    selectButton = gtk_button_new_with_label((const gchar *)"Select") ;
    g_signal_connect (selectButton, "clicked", G_CALLBACK (setTableName), combo);
    gtk_grid_attach(GTK_GRID(grid), selectButton, 0, 2, 1, 1) ;

    addButton = gtk_button_new_with_label((const gchar *)"Add a table") ;
    g_signal_connect (addButton, "clicked", G_CALLBACK (tableData), &table);
    gtk_grid_attach(GTK_GRID(grid), addButton, 0, 3, 1, 1) ;

    background_color(window, "#999999") ;
    gtk_widget_show_all(window) ;
    gtk_main() ;
}

int addTable (GtkWidget *widget, XMLdbData *table) {
    int nbCol = table->size ;
    char command[500] = "CREATE TABLE " ;
    char pk[100] = "PRIMARY KEY(" ;
    int i ;
    int kill ;
    size_t pkNb = 0 ;

    strcat(strcat(command, table->name),"(") ;

    for (i = 0 ; i < nbCol ; ++i) {
        kill = addColumnCommand(widget, command, table->columns[i], pk) ;
        if (kill != 0 && kill != 1)
            return kill ;
        pkNb += kill ;
    }

    if (pkNb == 0) {
        printError(widget, ERR_XML, "") ;
        return ERR_XML ;
    }

    removeLastChar(pk) ;
    strcat(strcat(command, pk), "))") ;
    kill = connectDB(command) ;
    if (kill != 0) {
        printError(widget, kill, "") ;
    } else {
        closeWindow(table->dualInputs->window) ;
        showTables(widget) ;
        printError(widget, 0, "Table added !") ;
    }
    return 0 ;
}

void setTableName (GtkWidget *widget, GtkWidget *comboBox) {
    char *table = "" ;
    retrieveComboBoxContent(widget, comboBox, &table) ;
    actionOnTable(widget, table) ;
}

void actionOnTable (GtkWidget *widget, char *tName) {
    GtkBuilder *builder;
    GtkWidget *window;
    GtkWidget *showCol ;
    GtkWidget *alterT ;
    GtkWidget *input ;

    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "main.glade", NULL);

    window = GTK_WIDGET(gtk_builder_get_object(builder, "window_table"));
    gtk_builder_connect_signals(builder, NULL);

    showCol = GTK_WIDGET(gtk_builder_get_object(builder, "showCol"));
    g_signal_connect(showCol, "clicked", G_CALLBACK(showTableContent), tName);

    alterT = GTK_WIDGET(gtk_builder_get_object(builder, "alterT"));
    g_signal_connect(alterT, "clicked", G_CALLBACK(alterTableWindow), tName);

    input = GTK_WIDGET(gtk_builder_get_object(builder, "inputData"));
    g_signal_connect(input, "clicked", G_CALLBACK(inputData), tName);

    background_color(window, "#999999" );
    mainMenu(builder, window) ;

    g_object_unref(builder);

    gtk_widget_show(window);
    gtk_main();
}

void inputData (GtkWidget *widget, char *tName) {
    GtkBuilder *builder;
    GtkWidget *okButton ;
    GtkWidget *input ;
    GtkDualInputs tInput ;
    Inserts tData ;

    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "main.glade", NULL);

    tInput.window = GTK_WIDGET(gtk_builder_get_object(builder, "window_input"));
    gtk_builder_connect_signals(builder, NULL);

    strcpy(tData.name, tName) ;

    input = GTK_WIDGET(gtk_builder_get_object(builder, "nbRows")) ;
    tInput.nb = input ;
    tData.dualInputs = &tInput ;
    okButton = GTK_WIDGET(gtk_builder_get_object(builder, "validate_input")) ;
    g_signal_connect(okButton, "clicked", G_CALLBACK(inputDataWindow), &tData);

    background_color(tInput.window, "#999999" );
    g_object_unref(builder);
    gtk_widget_show(tInput.window);
    gtk_main();
}

void inputDataWindow (GtkWidget *widget, Inserts *table) {
    GtkWidget *window ;
    GtkWidget *grid ;
    GtkWidget *button ;
    GtkWidget *label ;
    int nbRows ;
    char str[50] = "Input into table " ;

    strcat(str, table->name) ;
    retrieveInteger(widget, table->dualInputs->nb, &nbRows) ;
    table->nbRows = nbRows ;
    closeWindow(table->dualInputs->window) ;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), (const gchar *)str) ;
    gtk_window_set_default_size(GTK_WINDOW (window), 600, 200);
    table->dualInputs->window = window ;

    grid = gtk_grid_new() ;
    gtk_container_add(GTK_CONTAINER(window), grid) ;
    g_object_set(grid, "margin", 12, NULL);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10) ;
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10) ;

    table->nbCols = retrieveColNames(table->name, grid) ;
    createInsertInputs(table, grid) ;

    button = gtk_button_new_with_label((const gchar *)"Insert") ;
    gtk_grid_attach(GTK_GRID(grid), button, 0, nbRows+2, 1, 1) ;
    g_signal_connect(button, "clicked", G_CALLBACK(insertData), table) ;

    label = gtk_label_new("Don't forget the quotes around char/varchar/text/date(time) values !") ;
    gtk_grid_attach(GTK_GRID(grid), label, 0, nbRows+3, 2, 1) ;

    background_color(window, "#999999") ;
    gtk_widget_show_all(window) ;
    gtk_main() ;
}

void insertData (GtkWidget *widget, Inserts *table) {
    char command[500] ;
    char *data ;
    int i ;
    int j ;
    int kill ;

    for (i = 0 ; i < table->nbRows ; ++i) {
        strcat(strcat(strcpy(command, "INSERT INTO "), table->name), " VALUES (") ;
        for (j = 0 ; j < table->nbCols ; ++j) {
            retrieveData(widget, table->inputs[i][j], &data) ;
            printf("%s\n", data) ;
            if (strlen(data) != 0)
                strcat(strcat(command, data), ",") ;
            else
                strcat(strcat(command, "NULL"), ",") ;
        }
        removeLastChar(command) ;
        strcat(command, ")") ;
        kill = connectDB(command) ;
        if (kill != 0) {
            printError(NULL, 0, "Check your values") ;
            return ;
        }
    }
    freeDoubleWidgets(table->inputs, table->nbRows) ;
    closeWindow(table->dualInputs->window) ;
    printError(NULL, 0, "Data inserted !") ;
}

void freeDoubleWidgets (GtkWidget ***widgets, int nbRows) {
    int i ;
    for (i = 0 ; i < nbRows ; ++i) {
        if (widgets[i] != NULL)
            free(widgets[i]) ;
    }
    if (widgets != NULL)
        free(widgets) ;
}

void createInsertInputs(Inserts *table, GtkWidget *grid) {
    int i ;
    int j ;

    table->inputs = malloc(sizeof(GtkWidget **) * table->nbRows) ;
    if (table->inputs == NULL) {
        printError(NULL, ERR_MEM, "") ;
    }
    for (i = 0 ; i < table->nbRows ; ++i) {
        table->inputs[i] = malloc(sizeof(GtkWidget *) * table->nbCols) ;
        if (table->inputs[i] == NULL) {
            printError(NULL, ERR_MEM, "") ;
        }

        for (j = 0 ; j < table->nbCols ; ++j) {
            table->inputs[i][j] = gtk_entry_new() ;
            gtk_grid_attach(GTK_GRID(grid), table->inputs[i][j], j, i+1, 1, 1) ;
        }
    }
}

void inputDataSpeedRush (GtkWidget *widget) {
    GtkBuilder *builder;
    GtkWidget *window;
    GtkWidget *label ;
    GtkWidget *button ;
    GtkWidget *input ;

    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "main.glade", NULL);

    window = GTK_WIDGET(gtk_builder_get_object(builder, "window_sql"));
    gtk_builder_connect_signals(builder, NULL);

    label = GTK_WIDGET(gtk_builder_get_object(builder, "label_cmd")) ;
    input = GTK_WIDGET(gtk_builder_get_object(builder, "sql_input")) ;
    button = GTK_WIDGET(gtk_builder_get_object(builder, "sql_send")) ;
    g_signal_connect(button, "clicked", G_CALLBACK(getCommand), input);

    background_color(window, "#999999" );
    g_object_unref(builder);
    gtk_widget_show(window);
    gtk_main();
}

void getCommand (GtkWidget *widget, GtkWidget *input) {
    char *command ;
    int kill ;
    retrieveData(widget, input, &command) ;
    printf("%s\n", command) ;
    kill = connectDB(command) ;
    printf("All good ? %d\n", kill) ;
}

void showTableContent (GtkWidget *widget, char *tName) {
    GtkWidget *window;
    GtkWidget *grid;
    char str[50] = "Showing table " ;
    int nbLin ;

    strcat(str, tName) ;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), (const gchar *)str) ;

    grid = gtk_grid_new() ;
    gtk_container_add(GTK_CONTAINER(window), grid) ;
    g_object_set(grid, "margin", 12, NULL);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10) ;
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10) ;

    nbLin = retrieveColData(tName, grid) ;
    if (nbLin == 0) {
        closeWindow(window) ;
        printError(widget, 0, "Sorry, there's no data in this table") ;
    }
    gtk_window_set_default_size(GTK_WINDOW (window), 600, 200 + 15 * nbLin);

    background_color(window, "#999999") ;
    gtk_widget_show_all(window) ;
    gtk_main() ;
}

void alterTableWindow (GtkWidget *widget, char *tName) {
    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *alterButton ;
    GtkWidget *addColButton ;
    GtkWidget *nbColInput ;
    XMLdbData table ;
    GtkDualInputs di ;
    GtkWidget **dropButtons = NULL ;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), (const gchar *)"Alter table") ;
    gtk_window_set_default_size (GTK_WINDOW (window), 1000, 600);

    grid = gtk_grid_new() ;
    gtk_container_add(GTK_CONTAINER(window), grid) ;
    g_object_set(grid, "margin", 12, NULL);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10) ;
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10) ;

    strcpy(table.name, tName) ;
    di.window = window ;
    table.dualInputs = &di ;
    getColStructure(grid, &table, dropButtons) ;
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

    background_color(window, "#999999") ;
    gtk_widget_show_all(window) ;
    gtk_main() ;
}

void retrieveColNb (GtkWidget *widget, XMLdbData *table) {
    int nbCol ;
    retrieveInteger(widget, table->dualInputs->nb, &nbCol) ;
    if (nbCol <= 0)
        return ;
    table->size = nbCol ;
    closeWindow(table->dualInputs->window) ;
    getTableColumns(widget, table) ;
}

int addColumns (GtkWidget *widget, XMLdbData *table) {
    int nbCol = table->size ;
    int i ;
    char aiName[30] = "" ;
    char pk[100] = "" ;
    char command[250] ;
    int kill ;

    for (i = 0 ; i < nbCol ; ++i) {
        strcpy(command, "ALTER TABLE ") ;
        addSpace(strcat(command, table->name)) ;
        strcat(command, "ADD ") ;
        addColumnCommand(widget, command, table->columns[i], pk) ;
        retrieveAI(widget, table->columns[i], aiName) ;
        removeLastChar(command) ;
        kill = connectDB(command) ;
        if (kill != 0) {
            printError(widget, kill, "") ;
            return kill ;
        }
    }

    if (strlen(pk) != 0)
        retrievePrimKeys(widget, pk, table, aiName) ;

    free(table->columns) ;
    closeWindow(table->dualInputs->window) ;
    printError(widget, 0, "Columns added !") ;
    return 0 ;
}

/*
À découper en SOUS-fonctions !
*/
void getColStructure (GtkWidget *grid, XMLdbData *table, GtkWidget **dropButtons) {
    MYSQL mysql ;
    MysqlCoAndRes db ;
    MYSQL_ROW row ;
    char command[30] = "DESCRIBE " ;
    int lin = 0 ;
    int col = 0 ;
    TableCol *colNames ;
    strcat(command, table->name) ;
    db.mysql = &mysql ;
    reachMysql(&db, command) ;

    if (db.results == NULL) {
        return ;
    }

    table->size = countLin(db.results) ;
    table->columns = createSqlColInputs(table->size, grid) ;
    dropButtons = malloc(table->size * sizeof(GtkWidget *)) ;
    colNames = malloc(table->size * sizeof(TableCol)) ;
    if (dropButtons == NULL)
        return ;

    reachMysql(&db, command) ;
    while ((row = mysql_fetch_row(db.results)) != NULL) {
        col = 0 ;
        gtk_entry_set_text(GTK_ENTRY(table->columns[lin].name), row[0]) ;
        strcpy(colNames[lin].cName, row[0]) ;
        strcpy(colNames[lin].tName, table->name) ;
        colNames[lin].window = table->dualInputs->window ;
        gtk_entry_set_text(GTK_ENTRY(table->columns[lin].size), row[1]) ;
        if (row[4] != NULL)
            gtk_entry_set_text(GTK_ENTRY(table->columns[lin].def), row[4]) ;
        if (row[5] != NULL)
            gtk_entry_set_text(GTK_ENTRY(table->columns[lin].constraints), row[5]) ;
        if (row[3] != NULL && strcmp(row[3], "PRI") == 0)
            gtk_combo_box_set_active_id(GTK_COMBO_BOX(table->columns[lin].primKey), "yes") ;

        dropButtons[lin] = gtk_button_new_with_label((const gchar *)"Drop") ;
        gtk_grid_attach(GTK_GRID(grid), dropButtons[lin], 5, lin + 1, 1, 1) ;
        g_signal_connect(dropButtons[lin], "clicked", G_CALLBACK(dropColumn), &colNames[lin]) ;
        lin++ ;
    }

    mysql_free_result(db.results);
    mysql_close(&mysql) ;
    return;
}

int retrieveColNames (char *tName, GtkWidget *grid) {
    MYSQL mysql ;
    MysqlCoAndRes db ;
    MYSQL_ROW row ;
    char command[40] = "DESCRIBE " ;
    int col = 0 ;
    int lin = 0 ;

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
    lin++ ;

    mysql_free_result(db.results);
    mysql_close(&mysql) ;

    return col ;
}


int retrieveColData (char *tName, GtkWidget *grid) {
    MYSQL mysql ;
    MysqlCoAndRes db ;
    MYSQL_ROW row ;
    char command[40] = "SELECT * FROM " ;
    int col = 0 ;
    int lin = 1 ;
    int kill ;

    retrieveColNames(tName, grid) ;

    strcat(command, tName) ;

    db.mysql = &mysql ;
    kill = reachMysql(&db, command) ;
    if (kill != 0)
        printf("Erreur Mysql\n") ;

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

void dropColumn (GtkWidget *widget, TableCol *drop) {
    char command[100] = "ALTER TABLE ";
    int deleted ;
    strcat(strcat(strcat(command, drop->tName), " DROP "), drop->cName) ;

    if ((deleted = connectDB(command)) != 0) {
        printError(widget, 0, "Can't drop this column, bound to a primary key") ;
    }
    else {
        printError(widget, 0, "Column dropped") ;
        closeWindow(drop->window) ;
    }

}

void alterTable (GtkWidget *widget, XMLdbData *tableData) {
    char command[100] = "DESCRIBE " ;
    MYSQL mysql ;
    MysqlCoAndRes db ;
    MYSQL_ROW row ;

    int kill ;
    int i = 0 ;
    int modified = 0 ;
    int mod ;

    db.mysql = &mysql ;
    strcat(command, tableData->name) ;
    reachMysql(&db, command) ;

    modified += managePrimKeys(widget, db.results, tableData) ;

    reachMysql(&db, command) ;
    while ((row = mysql_fetch_row(db.results)) != NULL) {
        mod = checkIfModified(widget, tableData, i, command, row) ;
        if (mod == 1) {
            kill = connectDB(command) ;
            mod += 1 ;
        }
        i++ ;
    }

    if (modified != 0) {
        free(tableData->columns) ;
        closeWindow(tableData->dualInputs->window) ;
        printError(widget, 0, "Table altered") ;
    }

    mysql_free_result(db.results);
    mysql_close(&mysql) ;
}

int checkIfModified (GtkWidget *widget, XMLdbData *tableData, int i, char *command, MYSQL_ROW row) {
    int modified = 0 ;
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

void retrievePrimKeys (GtkWidget *widget, char *pk, XMLdbData *tableData, char *newAi) {
    char command[100] = "DESCRIBE " ;
    char ai[30] = "" ;
    MYSQL mysql ;
    MysqlCoAndRes db ;
    MYSQL_ROW row ;

    db.mysql = &mysql ;
    strcat(command, tableData->name) ;
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
        printError(widget, 0, "The auto_increment constraint can't be set (multiple primary keys)") ;
    }

    if (pk[strlen(pk) - 1] == ',') {
        removeLastChar(pk) ;
    }

    dropAddPrimKeys(pk, tableData->name) ;
    mysql_free_result(db.results);
    mysql_close(&mysql) ;
}

int managePrimKeys (GtkWidget *widget, MYSQL_RES *results, XMLdbData *table) {
    MYSQL_ROW row ;
    char *pk ;
    int i = 0 ;
    int diff = 0 ;
    char primKeys[50] = "" ;
    char ai[30] = "" ;

    while ((row = mysql_fetch_row(results)) != NULL) {
        pk = NULL ;
        retrieveComboBoxContent(widget, table->columns[i].primKey, &pk) ;
        if (pk != NULL) {
            if (strcmp(pk, "YES") == 0) {  //if is being set as primary key
                strcat(strcat(primKeys, row[0]), ",") ;
                if (strcmp(row[5], "auto_increment") == 0) { //if has an AI constraint
                    strcpy(ai, row[0]) ;
                }

                if (strcmp(row[3], "PRI") != 0) //if wasn't set as primary key
                    diff = 1 ;
            } else { //if is not being set as primary key
                if (strcmp(row[5], "auto_increment") == 0) { //if has an AI constraint
                    strcpy(ai, row[0]) ;
                }
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
            printError(widget, 0, "The auto_increment constraint can't be set (multiple primary keys)") ;
    }
    return 1 ;
}

void unsetAI (char *ai, char *tName) {
    char unsetAI[100] = "ALTER TABLE " ;
    strcat(unsetAI, tName) ;
    if (strlen(ai) != 0) {
        strcat(strcat(strcat(strcat(strcat(unsetAI, " CHANGE "), ai), " "), ai), " INTEGER") ;
        connectDB(unsetAI) ;
    }
}

void dropAddPrimKeys (char *primKeys, char *tName) {
    char str[50] = "ALTER TABLE " ;
    char add[100] = "" ;
    char drop[100] = "" ;

    strcat(str, tName) ;
    strcat(strcpy(drop, str), " DROP PRIMARY KEY") ;
    strcat(strcat(strcat(strcpy(add, str), " ADD PRIMARY KEY("), primKeys), ")") ;
    printf("%s\n", add) ;

    connectDB(drop) ;
    connectDB(add) ;
}

int countLin (MYSQL_RES *res) {
    int count = 0 ;
    MYSQL_ROW row ;
    while ((row = mysql_fetch_row(res)) != NULL)
        count++;
    return count ;
}

/*
*/
GtkColumn * createXmlColInputs (int nbCol, GtkWidget *grid) {
    GtkColumn *columns ;
    int i ;

    columns = malloc(sizeof(GtkColumn) * nbCol) ;
    if (columns == NULL)
        return NULL ;

    createXmlColLabels(grid) ;

    for (i = 0 ; i < nbCol ; ++i) {
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

GtkColumn * createSqlColInputs (int nbCol, GtkWidget *grid) {
    GtkColumn *columns ;
    int i ;

    columns = malloc(sizeof(GtkColumn) * nbCol) ;
    if (columns == NULL)
        return NULL ;

    createSqlColLabels(grid) ;

    for (i = 0 ; i < nbCol ; ++i) {
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

void createSqlColLabels (GtkWidget *grid) {
    addLabel(grid, 0, 0, "Column name") ;
    addLabel(grid, 0, 1, "Type") ;
    addLabel(grid, 0, 2, "Extra") ;
    addLabel(grid, 0, 3, "Default") ;
    addLabel(grid, 0, 4, "Primary key") ;
}

/*
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

GtkWidget * createComboBoxYN (void) {
    GtkWidget *box ;

    box = gtk_combo_box_text_new() ;
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(box), "yes", "YES") ;
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(box), "no", "NO") ;

    return box ;
}

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

void addLabel (GtkWidget *grid, int lin, int col, char *str) {
    GtkWidget *label ;
    label = gtk_label_new((const gchar *)str) ;
    gtk_grid_attach(GTK_GRID(grid), label, col, lin, 1, 1) ;
}

/*
*/
GtkWidget * createInput (char *placeholder) {
    GtkWidget *input = gtk_entry_new() ;
    gtk_entry_set_placeholder_text(GTK_ENTRY(input), placeholder) ;
    return input ;
}

void retrieveComboBoxContent (GtkWidget *widget, GtkWidget *box, char **str) {
    if (box == NULL)
        return ;
    if (gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(box)) != NULL)
        *str = (char *)gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(box)) ;
    else
        str = NULL ;
}

/*
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
*/
void background_color (GtkWidget *widget, char *color) {
  GtkCssProvider * cssProvider = gtk_css_provider_new();    //store the css

  char css[64] = "* { background-image:none; background-color:";
  strcat(strcat(css, color), ";}");

  gtk_css_provider_load_from_data(cssProvider, css,-1,NULL);
  GtkStyleContext * context = gtk_widget_get_style_context(widget);   //manage CSS provider
  gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(cssProvider),GTK_STYLE_PROVIDER_PRIORITY_USER);
}
