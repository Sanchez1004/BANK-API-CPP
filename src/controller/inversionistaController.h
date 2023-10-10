#pragma once

#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include "model/inversionista.h"

using namespace web::http;
using namespace web::http::experimental::listener;

class InversionistaController {
public:
    InversionistaController();
    void crearUsuario(http_request request);
    void modificarUsuario(http_request request);
    void verUsuarios(http_request request);
    void eliminarUsuario(http_request request);
    void consultarEstadoInversiones(http_request request);
private:
    Inversionista inversionista;
}; 