CREATE DATABASE test;
CREATE TABLE table1(t1c1 integer auto_increment, t1c2 varchar(32), message varchar(32), primary key(t1c1,t1c2));
CREATE TABLE table2(t2c1 varchar(32), t2c2 integer, t2c3 integer not null references table1(t1c1), primary key(t2c1,t2c2));
