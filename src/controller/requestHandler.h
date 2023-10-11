#pragma once

#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include "controller/inversionistaController.h"
#include "controller/proyectoController.h"

using namespace web;
using namespace http;
using namespace utility;
using namespace http::experimental::listener;

class RequestHandler {
public:
	RequestHandler(); // Constructor

	// Methods to handle different types of HTTP requests
	void handle_post(http_request request);
	void handle_get(http_request request);
	void handle_delete(http_request request);
	void handle_put(http_request request);

private:
	InversionistaController inversionistaController; // An instance of the InversionistaController class
	ProyectoController proyectoController; // An instance of the ProyectoController class
};
