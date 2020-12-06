/*
    Nathan Letourneau & Sarah Schlegel
    01.11.2020
    Project library with GTK functions
*/

// gcc `pkg-config --cflags gtk+-3.0` main.c -o main `pkg-config --libs gtk+-3.0`


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>
#include <mysql/mysql.h>
#include <gtk/gtk.h>

#include "gtk.h"
#include "db.h"
#include "sql.h"
#include "xml.h"
#include "str.h"
#include "errCodes.h"

void print_hello (GtkWidget *widget, gpointer data) {
  g_print ("Hello World\n");
}

void launcher (GtkApplication *app, gpointer user_data) {
  GtkWidget *window;
  GtkWidget *grid;
  GtkWidget *button;


  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "Database manager");
  gtk_container_set_border_width (GTK_CONTAINER (window), 10);


  grid = gtk_grid_new ();


  gtk_container_add (GTK_CONTAINER (window), grid);

  button = gtk_button_new_with_label ("Create database from XML");
  g_signal_connect (button, "clicked", G_CALLBACK (dbFromXML), NULL);


  gtk_grid_attach (GTK_GRID (grid), button, 0, 0, 1, 1);

  button = gtk_button_new_with_label ("Button 2");
  g_signal_connect (button, "clicked", G_CALLBACK (print_hello), NULL);


  gtk_grid_attach (GTK_GRID (grid), button, 1, 0, 1, 1);

  button = gtk_button_new_with_label ("Quit");
  g_signal_connect_swapped (button, "clicked", G_CALLBACK (gtk_widget_destroy), window);


  gtk_grid_attach (GTK_GRID (grid), button, 0, 1, 2, 1);


  gtk_widget_show_all (window);

}

int initProg (int argc, char **argv) {
  GtkApplication *app;
  int status;

  app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
  g_signal_connect (app, "activate", G_CALLBACK (launcher), NULL);
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
}
