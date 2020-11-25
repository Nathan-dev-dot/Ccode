dbmanager:
	gcc `pkg-config --cflags libxml-2.0`  -o main main.c `pkg-config --libs libxml-2.0` -l mysqlclient
