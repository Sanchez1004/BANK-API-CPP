// InversionistaController.cpp
#include "InversionistaController.h"
#include "model/inversionista.h"

void InversionistaController::handle_post(http_request request) {
    utility::string_t path = request.request_uri().path();
    std::string path_normal = utility::conversions::to_utf8string(path);

    if (path_normal == "/crearUsuario") {
        crearUsuario(request);
    }
    else if (path_normal == "/modificarUsuario") {
        modificarUsuario(request);
    }
    else {
        request.reply(status_codes::NotFound);
    }
}

void InversionistaController::crearUsuario(http_request request) {
    web::json::value json = request.extract_json().get();
    std::string nombre = utility::conversions::to_utf8string(json[U("nombre")].as_string());
    std::string tipo = utility::conversions::to_utf8string(json[U("tipo")].as_string());

    Inversionista inversionista(nombre, tipo, dbConn);
    inversionista.create();
    
    web::json::value respuesta;
    respuesta[L"message"] = json::value::string(U("Usuario creado exitosamente"));
    request.reply(status_codes::OK, respuesta);
}

void InversionistaController::modificarUsuario(http_request request) {
    auto json = request.extract_json().get();
    std::string nombre = utility::conversions::to_utf8string(json[U("nombre")].as_string());
    std::string tipo = utility::conversions::to_utf8string(json[U("tipo")].as_string());

    Inversionista inversionista("", "", dbConn);
    inversionista.read(nombre);
    inversionista.setTipo(tipo);
    inversionista.update(nombre);

    json::value respuesta;
    respuesta[L"message"] = json::value::string(U("Usuario modificado exitosamente"));
    request.reply(status_codes::OK, respuesta);
}
