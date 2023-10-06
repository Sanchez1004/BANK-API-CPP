#pragma once

#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include "controller/inversionistaController.h"
#include "controller/proyectoController.h"

class RequestHandler {
public:
	RequestHandler(DBConnection* dbConn);
	void handle_post(http_request request);

private:
	DBConnection* dbConn;
};