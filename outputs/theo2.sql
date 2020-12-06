CREATE DATABASE theo2;
CREATE TABLE place(id integer auto_increment, name varchar(32), address varchar(32), map char(32) default null, primary key(id));
CREATE TABLE drink(id integer auto_increment, name varchar(32), category varchar(32), primary key(id));
CREATE TABLE _place_propose_drink(place integer references place(id), drink integer references drink(id), price integer, primary key(place, drink));
CREATE TABLE room(id integer auto_increment, name varchar(32), max_capacity integer, price_half_day integer, place integer references place(id), primary key(id));
CREATE TABLE booking(id integer auto_increment, nb_persons integer, price integer, date_booking date, time_slot timestamp, room integer references room(id), primary key(id));
CREATE TABLE _booking_include_drink(booking integer references booking(id), drink integer references drink(id), primary key(booking, drink));
CREATE TABLE equipment(id integer auto_increment, name varchar(32), category varchar(32), primary key(id));
CREATE TABLE _room_owns_equipment(room integer references room(id), equipment integer references equipment(id), price integer, primary key(room, equipment));
