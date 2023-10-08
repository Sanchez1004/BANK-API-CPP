#pragma once
#ifndef INVERSIONISTA_H
#define INVERSIONISTA_H

#include <string>
#include "db/dbconnection.h"

class Inversionista {
private:
	DBConnection* dbConn;
	std::string nombre;
	std::string tipo;
	double ingresoMensual;
	double inversionMax;

public:
	Inversionista(const std::string& nombre, const std::string& tipo, DBConnection* dbConn);
	Inversionista(std::string& nombreInversionista, DBConnection* dbConn);
	Inversionista(DBConnection* dbConn);
	Inversionista();

	// Getters
	std::string getNombre() const;
	std::string getTipo() const;
	double getIngresoMensual() const;
	double getInversionMax() const;
	void inversionConfig(std::string tipo);

	// Setters
	void setNombre(const std::string& nombre);
	void setTipo(const std::string& tipo);
	void setIngresoMensual(double ingresoMensual);
	void setInversionMax(double inversionMaxima);

	// Database operations
	void create();
    void read(std::string qNombre);
    void update(std::string qNombre);
    void del(std::string qNombre);
	int getId(const std::string& nombre);

	// Investment Operations
	double getInversionTotal();
	void consultarEstadoInversiones(const std::string& nombreInversionista);
};

#endif //INVERSIONISTA_H
