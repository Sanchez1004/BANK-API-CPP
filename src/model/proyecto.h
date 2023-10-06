#pragma once
#ifndef PROYECTO_H
#define PROYECTO_H

#include <string>
#include "db/dbconnection.h"
#include "inversionista.h"

class Proyecto {
private:
	DBConnection* dbConn;
	std::string nombre;
	double cantidadARecaudar;
	double cantidadRecaudada;
	bool estado; // True para abierto, False para cerrado

public:
	Proyecto(const std::string& nombre, double cantidadARecaudar, DBConnection* dbConn);

	// Getters
	std::string getNombre() const;
	double getCantidadARecaudar() const;
	double getCantidadRecaudada() const;
	bool getEstado() const;

	// Setters
	void setNombre(const std::string& nombre);
	void setCantidadARecaudar(double cantidadARecaudar);
	void setCantidadRecaudada(double cantidadRecaudada);
	void setEstado(bool estado);

	// Database Operations
	void create();
	void read(std::string qNombre);
	void update(std::string qNombre);
	void del(std::string qNombre);
	int getId(const std::string& nombre);


	// Investment Operations
	void agregarInversion(Inversionista* inversionista, double cantidad);
	void eliminarInversion(const std::string& nombreInversionista, const std::string& nombreProyecto);
};

#endif //PROYECTO_H