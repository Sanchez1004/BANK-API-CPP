#pragma once

#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include "model/inversionista.h"

using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;

class InversionistaController {
public:
    InversionistaController();
    void handle_post(http_request request);
    void crearUsuario(http_request request);
    void modificarUsuario(http_request request);

private:
    DBConnection* dbConn = new DBConnection("bankapi", "localhost", "root", "1234");
};