#include "requestHandler.h"

RequestHandler::RequestHandler(DBConnection* dbConn) : dbConn(dbConn){}

void RequestHandler::handle_post(http_request request) {
    InversionistaController inversionistaController(dbConn);
	ProyectoController proyectoController(dbConn);

    utility::string_t path = request.request_uri().path();
    std::string path_normal = utility::conversions::to_utf8string(path);

    if (path_normal == "/crearUsuario") {
        inversionistaController.crearUsuario(request);
    }
    else if (path_normal == "/modificarUsuario") {
        inversionistaController.modificarUsuario(request);
    }
    else {
        request.reply(status_codes::NotFound);
    }
}