/*
    Nathan Letourneau & Sarah Schlegel
    01.11.2020
    Project library with GTK functions
*/

// gcc `pkg-config --cflags gtk+-3.0` main.c -o main `pkg-config --libs gtk+-3.0`


#include "all.h"

void hello (GtkWidget *widget, XMLdbData *data) {
    printf("Hello %p\n", data) ;
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

    background_color(&window, "#999999" );
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

    background_color(&window, "#999999" );

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
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10) ;


    dbLabel = gtk_label_new((const gchar *)"Enter the database name") ;
    gtk_grid_attach(GTK_GRID(grid), dbLabel, 0, 0, 1, 1) ;
    dbParams.name = createInput("Database name") ;
    gtk_grid_attach(GTK_GRID(grid), dbParams.name, 0, 2, 1, 1) ;

    nbLabel = gtk_label_new((const gchar *)"Enter the number of tables") ;
    gtk_grid_attach(GTK_GRID(grid), nbLabel, 0, 3, 1, 1) ;
    dbParams.nb = createInput("Number") ;
    gtk_grid_attach(GTK_GRID(grid), dbParams.nb, 0, 4, 1, 1) ;

    button = gtk_button_new_with_label((const gchar *)"button") ;
    g_signal_connect (button, "clicked", G_CALLBACK (createXMLFile), &dbParams);
    gtk_grid_attach(GTK_GRID(grid), button, 0, 6, 1, 1) ;

    background_color(&window, "#999999") ;
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

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), (const gchar *)"Number of columns") ;
    gtk_window_set_default_size (GTK_WINDOW (window), 200, 200);

    dbParams.window = window ;

    grid = gtk_grid_new() ;
    gtk_container_add(GTK_CONTAINER(window), grid) ;
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

    button = gtk_button_new_with_label((const gchar *)"button") ;
    g_signal_connect (button, "clicked", G_CALLBACK (setTableData), dbData);
    gtk_grid_attach(GTK_GRID(grid), button, 0, 6, 1, 1) ;

    background_color(&window, "#999999") ;
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
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10) ;

    dbData->columns = createColInputs(dbData->size, grid) ;
    dbData->dualInputs->window = window ;

    button = gtk_button_new_with_label((const gchar *)"Send") ;
    gtk_grid_attach(GTK_GRID(grid), button, 1, dbData->size + 1, 1, 1) ;
    g_signal_connect(button, "clicked", G_CALLBACK(addTableNode), dbData) ;

    background_color(&window, "#999999") ;
    gtk_widget_show_all(window) ;
    gtk_main() ;
}

void dbManagerWindow (GtkWidget *widget) {
    printf("coucou\n") ;
    return ;
}


GtkColumn * createColInputs (int nbCol, GtkWidget *grid) {
    GtkColumn *columns ;
    int i ;

    columns = malloc(sizeof(GtkColumn) * nbCol) ;
    if (columns == NULL)
        return NULL ;

    for (i = 0 ; i < nbCol ; ++i) {
        columns[i].name = createInput("Name") ;
        gtk_grid_attach(GTK_GRID(grid), columns[i].name, 0, i, 1, 1) ;
        columns[i].type = createComboBoxType() ;
        gtk_grid_attach(GTK_GRID(grid), columns[i].type, 1, i, 1, 1) ;
        columns[i].size = createInput("Size") ;
        gtk_grid_attach(GTK_GRID(grid), columns[i].size, 2, i, 1, 1) ;
        columns[i].constraints = createInput("Constraints (auto_increment, etc.)") ;
        gtk_grid_attach(GTK_GRID(grid), columns[i].constraints, 3, i, 1, 1) ;
        columns[i].check = createInput("Check") ;
        gtk_grid_attach(GTK_GRID(grid), columns[i].check, 4, i, 1, 1) ;
        columns[i].def = createInput("Default") ;
        gtk_grid_attach(GTK_GRID(grid), columns[i].def, 5, i, 1, 1) ;
        columns[i].primKey = createComboBoxYN();
        gtk_grid_attach(GTK_GRID(grid), columns[i].primKey, 6, i, 1, 1) ;
        columns[i].ref = createInput("References table(col)") ;
        gtk_grid_attach(GTK_GRID(grid), columns[i].ref, 7, i, 1, 1) ;
        columns[i].refd = createComboBoxYN() ;
        gtk_grid_attach(GTK_GRID(grid), columns[i].refd, 8, i, 1, 1) ;
    }

    return columns ;
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
}

/*
*/
void retrieveData(GtkWidget *widget, GtkWidget *input, char **str)
{
    if (input == NULL)
        return ;
    if (gtk_entry_get_text (GTK_ENTRY (input)) != NULL)
        *str = (char *)gtk_entry_get_text (GTK_ENTRY (input));
    else
        strcpy(*str, "") ;
}

/*
*/
void background_color (GtkWidget **widget, char *color) {
  GtkCssProvider * cssProvider = gtk_css_provider_new();    //store the css

  char css[64] = "* { background-image:none; background-color:";
  strcat(strcat(css, color), ";}");

  gtk_css_provider_load_from_data(cssProvider, css,-1,NULL);
  GtkStyleContext * context = gtk_widget_get_style_context(*widget);   //manage CSS provider
  gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(cssProvider),GTK_STYLE_PROVIDER_PRIORITY_USER);
}
