# 		XML / database manager

## 			User guide



With the database manager, you can insert an XML file describing your database, and the program will create the associated database in MySQL and generate a .sql creation file. The configuration file contains the correspondences that can create the database (for example <table tname="example">\*<table> ⬄ `create table example`).

Another option of the program allows the user to generate an XML database file from the entries the user sets in the command line.

Finally, the user can manage his databases through the GUI (alter tables, add columns or tables, etc.)



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

To launch the program, execute it from the command line using the  `./main` command. Enter your MySQL log inputs.

If you have modified the MySQL listening port, or if your have a different hostname, open the config file and modify the corresponding elements in `[mysql_setup]`.





### 	Functionnalities

For general use, we have set the boundaries of our text entries to **30 characters**, that constraint being mostly used for the names of the databases and their table and column names.



#### 			a) Database from XML document

The access path can be given from the folder in which is executed the `dbmanager`or as an absolute path.

If you have any douts about the format used for your XML document, please check the `format.xml` or the `example.xml`documents, but here's a list of the props and a way to se them :

- the mandatory props are the name of the table / column, the type of the column, and the size in the case of a CHAR or a VARCHAR. The other ones can be left empty ;
- the `reference="target"`column prop is meant to show to the program that this specific column will be referenced by another column. If that value is absent, the reference command won't be written ;
- there must be at least one referenced tabble in the file (at least one `reference="target"`)
- the `constraints`prop contains values like `auto_increment`, `not null`, etc ;
- the `check`prop contains values like `colName > 5`for an integer for example.



#### b) XML from entries

The database name you enter will also be the name of the associated xml file. The previous files with the same name will be overwritten.

The general constraints for the XML from entries functionnality are the folllowing:

- no MySQL keywords as names of tables or columns ;
- if the type is CHAR or VARCHAR, you must set a size ;
- at least one primary key by table ;
- the foreign key has to be written like following: `table(column)` ;
- the `Is referenced` combo box will set the `reference="target"`prop in the XML.

Please note there are no constraints on the number of foreign keys per table in this functionality.



#### 			c) Manage a database

First select the database you want to manage (the internal mysql databases are excluded from the list). Then either select the table you want to show or alter, or click on the button "Add a table".

If you have selected a table, you can show its content, alter its structure by modifying the columns or adding/dropping columns, insert data in the table, or drop it.

When altering a table that already contains data, please consider checking the integrity constraints for the primary keys, auto_increment etc, the program may return an "Invalid format" or a "Data syntax" error if these constraints are violated.



### 	General troubleshooting

###### Invalid format (check the mandatory attributes, the foreign and primary keys, and the names of the tables, columns and database) or Data syntax error

Please check the usual database constraints, such as:

- no MySQL keywords as names of tables or columns ;
- if the type is CHAR or VARCHAR, you must set a size ;
- at least one primary key by table ;
- the foreign key has to reference `table(column)`.





### 	Credits

This program was developped by Nathan LETOURNEAU and Sarah SCHLEGEL.

