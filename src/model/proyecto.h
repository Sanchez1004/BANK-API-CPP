#pragma once
#ifndef PROYECTO_H
#define PROYECTO_H

#include <string>
#include "db/dbconnection.h"
#include "inversionista.h"
#include "cpprest/http_listener.h"
#include "cpprest/json.h"
#include <sstream>

using namespace web::http;
using namespace web::http::experimental::listener;

class Proyecto {
private:
	DBConnection* dbConn; // Pointer to the database connection
	Inversionista inversionista; // An instance of the Inversionista class
	std::string nombre; // Name of the project
	double cantidadARecaudar; // Amount to be raised for the project
	double cantidadRecaudada; // Amount raised for the project
	bool estado; // Status of the project (True for open, False for closed)

public:
	Proyecto(); // Constructor

	// ------------ Getters ------------
	std::string getNombre() const;
	double getCantidadARecaudar() const;
	double getCantidadRecaudada() const;
	bool getEstado() const;


	//  ------------ Setters ------------
	void setNombre(const std::string& nombre);
	void setCantidadARecaudar(double cantidadARecaudar);
	void setCantidadRecaudada(double cantidadRecaudada);
	void setEstado(bool estado);


	// ------------ Database Operations ------------
	void create();
	void read(std::string nombreProyecto);
	void update(std::string nombreAntiguo);
	void del(std::string nombreProyecto);
	int getId(const std::string& nombreProyecto);
	bool existe(const std::string& nombreProyecto);
	bool tieneInversiones(const std::string& nombreInversionista, const std::string& nombreProyecto);


	// ------------ Investment Operations ------------
	void realizarInversion(std::string nombreInversionista, std::string nombreProyecto, std::string cantidad);
	void eliminarInversiones(const std::string& nombreInversionista, const std::string& nombreProyecto);
	sql::ResultSet* consultarProyecto(std::string nombreProyecto);
	std::vector<std::tuple<int, std::string, double, std::string>> listarInversionesRealizadas(const std::string& nombreProyecto);
};


#endif //PROYECTO_H