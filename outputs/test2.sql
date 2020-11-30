CREATE DATABASE test2;
CREATE TABLE table1(tcv integer auto_increment , t1c2 varchar(32) default null , primary key(tcv));
CREATE TABLE table2(t2c1 varchar(32) , t2c2 varchar(32) default "Hello" , t2c3 integer not null references table1(tcv), primary key(t2c1,t2c2));
