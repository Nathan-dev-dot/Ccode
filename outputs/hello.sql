CREATE DATABASE hello;
CREATE TABLE kikou(inde INTEGER auto_increment, hiko VARCHAR(32), primary key(inde));
CREATE TABLE kikou2(yolo VARCHAR(32) references kikou(hiko), dt DATETIME, primary key(yolo));
