# 		XML / database manager

## 			User guide



The database manager is a program that will allow you insert a database in MYSQL from an XML file and to manage it through the GUI. It can also let you create an XML file from the entries input in the graphical interface. 



### 	Table of contents

1) General setup

2) Functionnalities
	a) Database from XML document
	b) XML from entries
	c) Manage a database

3) General troubleshooting

4) Credits



### 	General setup

The database manager requires an installed instance of mysql on your computer.

To launch the program, execute it from the command line using the  `./dbmanager` command. Enter your MySQL log inputs.

If you have modified the MySQL listening port, or if your have a different hostname, open the config file and modify the corresponding elements in `[mysql_setup]`.



### 	Functionnalities

For general use, we have set the boundaries of our text entries to **30 characters**, that constraint being mostly used for the names of the databases and their table and column names.

#### 			a) Database from XML document

[blabla]

If you have any douts about the format used for your XML document, please check the `format.xml`document.

	#### 			b) XML from entries

[blabla]

#### 			c) Manage a database

[blabla]



### 	General troubleshooting

###### Invalid format (check the mandatory attributes, the foreign and primary keys, and the names of the tables, columns and database)

Please check the usual database constraints, such as

- no MySQL keywords as names of tables or columns
- if the type is CHAR or VARCHAR, you must set a size
- at least one primary key by table
- the foreign key has to reference `table(column)`

If you're altering a database with data inside, please consider checking the integrity constraints for the primary keys, auto_increment etc.



### 	Credits

This program was developped by Nathan LETOURNEAU and Sarah SCHLEGEL.

