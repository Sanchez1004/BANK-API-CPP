#include "requestHandler.h"

RequestHandler::RequestHandler() : inversionistaController(), proyectoController() {}

void RequestHandler::handle_post(http_request request) {
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
    else if (pathString == "/consultarEstadoInversiones") {
        inversionistaController.consultarEstadoInversiones(request);
    }
    else {
        request.reply(status_codes::NotFound);
    }
}
    
void RequestHandler::handle_delete(http_request request) {
    utility::string_t path = request.request_uri().path();
    std::string pathString = utility::conversions::to_utf8string(path);

    if (pathString == "/eliminarUsuarios") {
        inversionistaController.eliminarUsuario(request);
    }
    else if (pathString == "/eliminarProyecto") {
        proyectoController.eliminarProyecto(request);
    }
    else if (pathString == "/eliminarInversiones") {
        proyectoController.eliminarInversiones(request);
    }
    else {
        request.reply(status_codes::NotFound);
    }
}

void RequestHandler::handle_put(http_request request) { 
    utility::string_t path = request.request_uri().path();
    std::string pathString = utility::conversions::to_utf8string(path);

    if (pathString == "/modificarUsuario") {
        inversionistaController.modificarUsuario(request);
    }
    else {
        request.reply(status_codes::NotFound);
    }
}