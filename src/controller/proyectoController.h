#pragma once

#include <iomanip>
#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include "model/proyecto.h"

using namespace web::http;
using namespace web::http::experimental::listener;

class ProyectoController {
public:
	ProyectoController(); // Constructor

	// Methods to handle different types of HTTP requests related to the project
	void crearProyecto(http_request request);
	void realizarInversion(http_request request);
	void consultarProyecto(http_request request);
	void listarInversionesRealizadas(http_request request);
	void eliminarProyecto(http_request request);
	void eliminarInversiones(http_request request);

private:
	Inversionista inversionista; // An instance of the Inversionista class
	Proyecto proyecto; // An instance of the Proyecto class
};
