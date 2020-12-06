#ifndef GTK_H
#define GTK_H

int initProg (int argc, char **argv) ;
void launcher (GtkApplication *app, gpointer user_data) ;
void print_hello (GtkWidget *widget, gpointer data) ;

#endif
