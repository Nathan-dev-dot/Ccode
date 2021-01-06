#Compilation flags
CFLAGS = -Wall `pkg-config --cflags libxml-2.0 gtk+-3.0` -o

#Libraries
LIBS = `pkg-config --libs libxml-2.0 gtk+-3.0` -l mysqlclient

#C files
CFILES = sources/main.c sources/xml.c sources/db.c sources/sql.c sources/str.c sources/gtk.c

#Executable name
EXE = dbmanager

program:
	gcc $(CFLAGS) $(EXE) $(CFILES) $(LIBS)
