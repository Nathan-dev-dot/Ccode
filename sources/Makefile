#Compilation flags 
CFLAGS = -Wall `pkg-config --cflags libxml-2.0 gtk+-3.0` -o

#Libraries
LIBS = `pkg-config --libs libxml-2.0 gtk+-3.0` -l mysqlclient

#C files
CFILES = main.c xml.c db.c sql.c str.c gtk.c

#Executable name
EXE = main

dbmanager:
	gcc $(CFLAGS) $(EXE) $(CFILES) $(LIBS)
