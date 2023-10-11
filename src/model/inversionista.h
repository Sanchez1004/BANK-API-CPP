#pragma once
#ifndef INVERSIONISTA_H
#define INVERSIONISTA_H

#include <string>
#include "db/dbconnection.h"

class Inversionista {
private:
	DBConnection* dbConn; // Pointer to the database connection
	std::string nombre; // Name of the investor
	std::string tipo; // Type of investor
	double ingresoMensual; // Monthly income of the investor
	double inversionMax; // Maximum investment the investor can make

public:
	Inversionista(); // Constructor

	// ------------ Getters ------------
	std::string getNombre() const;
	std::string getTipo() const;
	double getIngresoMensual() const;
	double getInversionMax() const;
	void inversionConfig(std::string tipo);


	// ------------ Setters ------------
	void setNombre(const std::string& nombre);
	void setTipo(const std::string& tipo);
	void setIngresoMensual(double ingresoMensual);
	void setInversionMax(double inversionMaxima);


	// ------------ Database operations ------------
	void create();
	void read(std::string qNombre);
	void update(std::string qNombre);
	void del(std::string qNombre);
	int getId(const std::string& nombre);
	bool existe(const std::string& nombreInversionista);


	// ------------ Investment Operations ------------
	double getInversionTotal();
	std::vector<std::pair<std::string, double>> consultarEstadoInversiones(const std::string& nombreInversionista);
	sql::ResultSet* verUsuarios();
};


#endif //INVERSIONISTA_H