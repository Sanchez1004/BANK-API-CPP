#pragma once

#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include "controller/inversionistaController.h"
#include "controller/proyectoController.h"

class RequestHandler {
public:
	RequestHandler(DBConnection* dbConn);
	void handle_post(http_request request);
	void handle_get(http_request request);
	void handle_delete(http_request request);
	void handle_put(http_request request);

private:
	DBConnection* dbConn;
};