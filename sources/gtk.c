/*
    Nathan Letourneau & Sarah Schlegel
    01.11.2020
    Project library with GTK functions
*/

// gcc `pkg-config --cflags gtk+-3.0` main.c -o main `pkg-config --libs gtk+-3.0`


#include "all.h"

void hello (GtkWidget *widget) {
    printf("Hello\n") ;
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
    //GtkWidget *option3;

    option1 = GTK_WIDGET(gtk_builder_get_object(builder, "dbFromXml"));
    g_signal_connect(option1, "clicked", G_CALLBACK(dbFromXMLWindow), NULL);

    option2 = GTK_WIDGET(gtk_builder_get_object(builder, "createXml"));
    g_signal_connect(option2, "clicked", G_CALLBACK(xmlFromEntries), window);

    // option3 = GTK_WIDGET(gtk_builder_get_object(builder, "manageDatabase"));
    // g_signal_connect(option3, "clicked", G_CALLBACK(dbFromXMLWindow), window);
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
    XMLdbParams dbParams;

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
void tableData (GtkWidget *widget) {
    GtkWidget *newWindow ;
    GtkWidget *grid;
    GtkWidget *button ;
    GtkWidget *nbLabel ;
    GtkWidget *tLabel ;
    XMLdbParams dbParams;

    newWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(newWindow), (const gchar *)"Number of columns") ;
    gtk_window_set_default_size (GTK_WINDOW (newWindow), 200, 200);

    grid = gtk_grid_new() ;
    gtk_container_add(GTK_CONTAINER(newWindow), grid) ;
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10) ;

    tLabel = gtk_label_new((const gchar *)"Enter the table name") ;
    gtk_grid_attach(GTK_GRID(grid), tLabel, 0, 0, 1, 1) ;
    dbParams.name = createInput("Database name") ;
    gtk_grid_attach(GTK_GRID(grid), dbParams.name, 0, 2, 1, 1) ;

    nbLabel = gtk_label_new((const gchar *)"Enter the number of columns") ;
    gtk_grid_attach(GTK_GRID(grid), nbLabel, 0, 3, 1, 1) ;
    dbParams.nb = createInput("Number") ;
    gtk_grid_attach(GTK_GRID(grid), dbParams.nb, 0, 4, 1, 1) ;

    button = gtk_button_new_with_label((const gchar *)"button") ;
    g_signal_connect (button, "clicked", G_CALLBACK (hello), NULL);
    gtk_grid_attach(GTK_GRID(grid), button, 0, 6, 1, 1) ;

    background_color(&newWindow, "#999999") ;

    gtk_widget_show_all(newWindow) ;
    gtk_main() ;
}

/*
*/
GtkWidget * createInput (char *placeholder) {
    GtkWidget *input = gtk_entry_new() ;
    gtk_entry_set_placeholder_text(GTK_ENTRY(input), placeholder) ;
    return input ;
}

/*
*/
void retrieveData(GtkWidget *widget, GtkWidget *input, char **str)
{
    if (input == NULL)
        return ;
    *str = (char *)gtk_entry_get_text (GTK_ENTRY (input));
    printf("%s\n", *str) ;
}

/*
*/
void background_color (GtkWidget **widget, char *color) {
  GtkCssProvider * cssProvider = gtk_css_provider_new();    //store the css

  char css[64] = "* { background-image:none; background-color:";
  strcat( strcat( css , color ), ";}" );

  gtk_css_provider_load_from_data(cssProvider, css,-1,NULL);
  GtkStyleContext * context = gtk_widget_get_style_context(*widget);   //manage CSS provider
  gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(cssProvider),GTK_STYLE_PROVIDER_PRIORITY_USER);
}
