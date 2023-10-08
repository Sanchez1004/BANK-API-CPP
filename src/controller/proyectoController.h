#pragma once

#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include "model/proyecto.h"

using namespace web::http;
using namespace web::http::experimental::listener;

class ProyectoController {
public:
	ProyectoController(DBConnection* dbConn);
	void crearProyecto(http_request request);
	void realizarInversion(http_request request);
	void consultarProyecto(http_request request);
	void listarInversionesRealizadas(http_request request);
	void eliminarProyecto(http_request request);
	void eliminarInversion(http_request request);
	void cambiarNombreProyecto(http_request request);

private:
	DBConnection* dbConn;
};