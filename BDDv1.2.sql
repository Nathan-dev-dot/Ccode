/*
OMNES-MOUHAMADOU Théo
7 juillet 2020
BBD v1.2 de ÔME
*/




CREATE TABLE compte(
	id INTEGER AUTO_INCREMENT PRIMARY KEY,
  login VARCHAR(32),
  password VARCHAR(255)
);


CREATE TABLE profile(
  id INTEGER AUTO_INCREMENT PRIMARY KEY,
  nom VARCHAR(32),
  fk_compte INTEGER NOT NULL,
  FOREIGN KEY(fk_compte) REFERENCES compte(id)
);

CREATE TABLE piece(
  id INTEGER AUTO_INCREMENT PRIMARY KEY,
  nom VARCHAR(32),
	photo VARCHAR(64)
);

CREATE TABLE _profile_piece(
  fk_profile INTEGER,
	fk_piece INTEGER,
	PRIMARY KEY(fk_profile,fk_piece),
  FOREIGN KEY(fk_profile) REFERENCES profile(id),
	FOREIGN KEY(fk_piece) REFERENCES piece(id)
);

CREATE TABLE emplacement(
  id INTEGER AUTO_INCREMENT PRIMARY KEY,
  nom VARCHAR(32),
  fk_piece INTEGER NOT NULL,
  fk_emplacement_parent INTEGER,
  FOREIGN KEY(fk_piece) REFERENCES piece(id),
  FOREIGN KEY(fk_emplacement_parent) REFERENCES emplacement(id)
);

CREATE TABLE objet(
  id INTEGER AUTO_INCREMENT PRIMARY KEY,
  nom VARCHAR(32),
	quantite SMALLINT,
	description VARCHAR(128),
	date_ajout DATE,
	photo VARCHAR(128),
	fk_emplacement INTEGER NOT NULL,
	FOREIGN KEY(fk_emplacement) REFERENCES emplacement(id)
);

CREATE TABLE _recherche(
  fk_profile INTEGER,
	fk_objet INTEGER,
	PRIMARY KEY(fk_profile, fk_objet),
	FOREIGN KEY(fk_profile) REFERENCES profile(id),
	FOREIGN KEY(fk_objet) REFERENCES objet(id)
);

CREATE TABLE couleur(
  id INTEGER AUTO_INCREMENT PRIMARY KEY,
  nom VARCHAR(32),
  hexa CHAR(7)
);

CREATE TABLE etiquette(
  id INTEGER AUTO_INCREMENT PRIMARY KEY,
  nom VARCHAR(32),
	fk_compte INTEGER NOT NULL,
  fk_couleur INTEGER NOT NULL,
  FOREIGN KEY(fk_couleur) REFERENCES couleur(id),
	FOREIGN KEY(fk_compte) REFERENCES compte(id)
);

CREATE TABLE _objet_etiquette(
  fk_objet INTEGER,
  fk_etiquette INTEGER,
	PRIMARY KEY(fk_objet,fk_etiquette),
  FOREIGN KEY(fk_objet) REFERENCES objet(id),
  FOREIGN KEY(fk_etiquette) REFERENCES etiquette(id)
);
















/**/
