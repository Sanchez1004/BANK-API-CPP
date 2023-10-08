#include "requestHandler.h"

RequestHandler::RequestHandler(DBConnection* dbConn) : dbConn(dbConn){}

void RequestHandler::handle_post(http_request request) {
    InversionistaController inversionistaController(dbConn);
	ProyectoController proyectoController(dbConn);

    utility::string_t path = request.request_uri().path();
    std::string pathString = utility::conversions::to_utf8string(path);

    if (pathString == "/crearUsuario") {
        inversionistaController.crearUsuario(request);
    }
    else if (pathString == "/crearProyecto") {
        proyectoController.crearProyecto(request);
    }
    else if (pathString == "/realizarInversion") {
        proyectoController.realizarInversion(request);
    }
    else {
        request.reply(status_codes::NotFound);
    }   
}

void RequestHandler::handle_get(http_request request) {
    InversionistaController inversionistaController(dbConn);
    ProyectoController proyectoController(dbConn);

    utility::string_t path = request.request_uri().path();
    std::string pathString = utility::conversions::to_utf8string(path);

    if (pathString == "/verUsuarios") {
        inversionistaController.verUsuarios(request);
    }
    else if (pathString == "/consultarProyecto") {
        proyectoController.consultarProyecto(request);
    }
    else if (pathString == "/listarInversiones") {
        proyectoController.listarInversionesRealizadas(request);
    }
    else {
        request.reply(status_codes::NotFound);
    }
}

void RequestHandler::handle_delete(http_request request) {
    InversionistaController inversionistaController(dbConn);
    ProyectoController proyectoController(dbConn);

    utility::string_t path = request.request_uri().path();
    std::string pathString = utility::conversions::to_utf8string(path);

    if (pathString == "/eliminarUsuarios") {
        inversionistaController.eliminarUsuario(request);
    }
    else if (pathString == "/eliminarProyecto") {
        proyectoController.eliminarProyecto(request);
    }
    else if (pathString == "/eliminarInversion") {
        proyectoController.eliminarInversion(request);
    }
    else {
        request.reply(status_codes::NotFound);
    }
}

void RequestHandler::handle_put(http_request request) {
    InversionistaController inversionistaController(dbConn);
    ProyectoController proyectoController(dbConn);

    utility::string_t path = request.request_uri().path();
    std::string pathString = utility::conversions::to_utf8string(path);

    if (pathString == "/modificarUsuario") {
        inversionistaController.modificarUsuario(request);
    }
    else if (pathString == "/cambiarNombreProyecto") {
        proyectoController.cambiarNombreProyecto(request);
    }
    else {
        request.reply(status_codes::NotFound);
    }
}