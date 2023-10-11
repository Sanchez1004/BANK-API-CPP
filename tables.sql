create table inversionistas(
	id int not null auto_increment primary key,
	nombre varchar(30) not null,
	tipo varchar(6) not null,
	ingreso_mensual double
);

create table proyectos(
	id int primary key not null auto_increment,
	nombre varchar(50) not null,
	cantidadARecaudar double not null,	
	cantidadRecaudada double,
	estado boolean not null
);

CREATE TABLE inversiones (
	id INT AUTO_INCREMENT,
	nombreInversionistaID INT,
	nombreProyectoID INT,
	cantidad DOUBLE,
	fecha TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
	PRIMARY KEY(id),
	FOREIGN KEY(nombreInversionistaID) REFERENCES inversionistas(id),
	FOREIGN KEY(nombreProyectoID) REFERENCES proyectos(id)
);