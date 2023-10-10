#include "requestHandler.h"

// Constructor for the RequestHandler class
RequestHandler::RequestHandler() : inversionistaController(), proyectoController() {}

// Function to handle POST requests
void RequestHandler::handle_post(http_request request) {
    // Get the path of the request
    utility::string_t path = request.request_uri().path();
    std::string pathString = utility::conversions::to_utf8string(path);

    // Depending on the path, call the appropriate function from the controller classes
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
        // If the path does not match any of the above, return a 404 Not Found status code
        request.reply(status_codes::NotFound);
    }
}

// Function to handle GET requests
void RequestHandler::handle_get(http_request request) {
    // Get the path of the request
    utility::string_t path = request.request_uri().path();
    std::string pathString = utility::conversions::to_utf8string(path);

    // Depending on the path, call the appropriate function from the controller classes
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
        // If the path does not match any of the above, return a 404 Not Found status code
        request.reply(status_codes::NotFound);
    }
}

// Function to handle DELETE requests
void RequestHandler::handle_delete(http_request request) {
    // Get the path of the request
    utility::string_t path = request.request_uri().path();
    std::string pathString = utility::conversions::to_utf8string(path);

    // Depending on the path, call the appropriate function from the controller classes
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
        // If the path does not match any of the above, return a 404 Not Found status code
        request.reply(status_codes::NotFound);
    }
}

// Function to handle PUT requests
void RequestHandler::handle_put(http_request request) {
    // Get the path of the request
    utility::string_t path = request.request_uri().path();
    std::string pathString = utility::conversions::to_utf8string(path);

    // Depending on the path, call the appropriate function from the controller classes
    if (pathString == "/modificarUsuario") {
        inversionistaController.modificarUsuario(request);
    }
    else {
        // If the path does not match any of the above, return a 404 Not Found status code
        request.reply(status_codes::NotFound);
    }
}
