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

void destroy (GtkWidget *widget, gpointer data) {
    gtk_main_quit();
}

int initProg (int argc, char **argv) {
    GtkBuilder *builder;
    GtkWidget  *window;
    GtkWidget *option1 ;

    gtk_init(&argc, &argv);

    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "main.glade", NULL);

    window = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));
    gtk_builder_connect_signals(builder, NULL);
    g_signal_connect(window, "destroy", G_CALLBACK(destroy), NULL) ;

    background_color(&window, "#999999" );
    mainMenu(builder, window) ;

    g_object_unref(builder);

    gtk_widget_show(window);
    gtk_main();

    return 0;
}

void mainMenu (GtkBuilder *builder, GtkWidget *window) {
    GtkWidget *option1;
    GtkWidget *option2;
    GtkWidget *option3;

    option1 = GTK_WIDGET(gtk_builder_get_object(builder, "dbFromXml"));
    g_signal_connect(option1, "clicked", G_CALLBACK(dbFromXMLWindow), NULL);

    // option2 = GTK_WIDGET(gtk_builder_get_object(builder, "createXml"));
    // g_signal_connect(option2, "clicked", G_CALLBACK(dbFromXMLWindow), window);
    //
    // option3 = GTK_WIDGET(gtk_builder_get_object(builder, "manageDatabase"));
    // g_signal_connect(option3, "clicked", G_CALLBACK(dbFromXMLWindow), window);
}

void dbFromXMLWindow (GtkWidget *widget) {
    GtkBuilder *builder;
    GtkWidget *window;
    GtkWidget *ok ;
    GtkWidget *input ;

    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "main.glade", NULL);

    window = GTK_WIDGET(gtk_builder_get_object(builder, "window_xml"));
    gtk_builder_connect_signals(builder, NULL);

    input = GTK_WIDGET(gtk_builder_get_object(builder, "XMLPath")) ;
    ok = GTK_WIDGET(gtk_builder_get_object(builder, "validate")) ;
    g_signal_connect(ok, "clicked", G_CALLBACK(dbFromXML), input);

    background_color(&window, "#999999" );

    g_object_unref(builder);

    gtk_widget_show(window);
    gtk_main();
}

void retrieveData(GtkWidget *widget, GtkWidget *input, char **str)
{
    *str = (char *)gtk_entry_get_text (GTK_ENTRY (input));
}

void background_color (GtkWidget **widget, char *color) {
  GtkCssProvider * cssProvider = gtk_css_provider_new();    //store the css

  char css[64] = "* { background-image:none; background-color:";
  strcat( strcat( css , color ), ";}" );

  gtk_css_provider_load_from_data(cssProvider, css,-1,NULL);
  GtkStyleContext * context = gtk_widget_get_style_context(*widget);   //manage CSS provider
  gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(cssProvider),GTK_STYLE_PROVIDER_PRIORITY_USER);
}
