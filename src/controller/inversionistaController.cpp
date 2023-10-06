#include "InversionistaController.h"
#include "model/inversionista.h"

InversionistaController::InversionistaController(DBConnection* dbConn) : dbConn(dbConn){}

void InversionistaController::crearUsuario(http_request request) {
    web::json::value json = request.extract_json().get();
    std::string nombre = utility::conversions::to_utf8string(json[U("nombre")].as_string());
    std::string tipo = utility::conversions::to_utf8string(json[U("tipo")].as_string());

    Inversionista inversionista(nombre, tipo, dbConn);
    inversionista.create();
    
    web::json::value respuesta;
    respuesta[L"message"] = web::json::value::string(U("Usuario creado exitosamente"));
    request.reply(status_codes::OK, respuesta);
}

void InversionistaController::modificarUsuario(http_request request) {
    auto json = request.extract_json().get();
    std::string nombre = utility::conversions::to_utf8string(json[U("nombre")].as_string());
    std::string tipo = utility::conversions::to_utf8string(json[U("tipo")].as_string());

    Inversionista inversionista("", "", dbConn);
    inversionista.read(nombre);
    inversionista.setTipo(tipo);
    inversionista.inversionConfig(tipo);
    inversionista.update(nombre);

    web::json::value respuesta;
    respuesta[L"message"] = web::json::value::string(U("Usuario modificado exitosamente"));
    request.reply(status_codes::OK, respuesta);
}
