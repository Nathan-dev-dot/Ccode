# 		XML / database manager

## 			User guide



With the database manager, you can insert an XML file describing your database, and the program will create the associated database in MySQL and generate a .sql creation file. The configuration file contains the correspondences that can create the database (for example <table tname="example">*<table> ⬄ `create table example`).

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

- The mandatory props are the name of the table / column, the type of the column, and the size in the case of a CHAR or a VARCHAR. The other ones can be left empty.
- The `reference="target"`column prop is meant to show to the program that this specific column will be referenced by another column. If that value is absent, the reference command won't be executed.
- The `constraints`prop contains values like `auto_increment`, `not null`, etc.
- The `check`prop contains values like `colName > 5`for an integer for example.

#### b) XML from entries

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

