#include "proyectoController.h"

ProyectoController::ProyectoController(DBConnection* dbConn) : dbConn(dbConn) {}

void ProyectoController::crearProyecto(http_request request) {
    web::json::value json = request.extract_json().get();
    std::string nombreProyecto = utility::conversions::to_utf8string(json[U("nombreProyecto")].as_string());
    double cantidadARecaudar = json[U("cantidadARecaudar")].as_double();
    Proyecto proyecto(nombreProyecto, cantidadARecaudar, dbConn);

    if (proyecto.existe(nombreProyecto)) {
        web::json::value respuesta;
        respuesta[L"message"] = web::json::value::string(U("El proyecto ya existe."));
        request.reply(status_codes::BadRequest, respuesta);
        return;
    }
    try {
        proyecto.create();
        web::json::value respuesta;
        respuesta[L"message"] = web::json::value::string(U("Proyecto creado con éxito."));
        request.reply(status_codes::OK, respuesta);
    }
    catch (const std::runtime_error& e) {
        web::json::value respuesta;
        respuesta[L"message"] = web::json::value::string(utility::conversions::to_string_t(e.what()));
        request.reply(status_codes::BadRequest, respuesta);
    }
}


void ProyectoController::realizarInversion(http_request request) {
    web::json::value json = request.extract_json().get();
    std::string nombreInversionista = utility::conversions::to_utf8string(json[U("nombreInversionista")].as_string());
    std::string nombreProyecto = utility::conversions::to_utf8string(json[U("nombreProyecto")].as_string());
    std::string cantidad = utility::conversions::to_utf8string(json[U("cantidad")].as_string());

    Proyecto proyecto(nombreProyecto, dbConn);
    try {
        proyecto.realizarInversion(nombreInversionista, nombreProyecto, cantidad);

        web::json::value respuesta;
        respuesta[L"message"] = web::json::value::string(U("Inversión realizada con éxito."));
        request.reply(status_codes::OK, respuesta);
    }
    catch (const std::exception& e) {
        web::json::value respuesta;
        respuesta[L"message"] = web::json::value::string(utility::conversions::to_string_t(e.what()));
        request.reply(status_codes::BadRequest, respuesta);
    }
}

void ProyectoController::consultarProyecto(http_request request) {
    web::json::value json = request.extract_json().get();
    std::string nombreProyecto = utility::conversions::to_utf8string(json[U("nombreProyecto")].as_string());

    Proyecto proyecto(nombreProyecto, dbConn);
    try {
        proyecto.read(nombreProyecto);
        web::json::value respuesta;
        respuesta[U("nombre")] = web::json::value::string(utility::conversions::to_string_t(proyecto.getNombre()));
        respuesta[U("cantidadARecaudar")] = web::json::value::number(proyecto.getCantidadARecaudar());
        respuesta[U("cantidadRecaudada")] = web::json::value::number(proyecto.getCantidadRecaudada());
        respuesta[U("estado")] = web::json::value::boolean(proyecto.getEstado());

        request.reply(status_codes::OK, respuesta);
    }
    catch (const std::runtime_error& e) {
        web::json::value respuesta;
        respuesta[L"message"] = web::json::value::string(utility::conversions::to_string_t(e.what()));
        request.reply(status_codes::BadRequest, respuesta);
    }
}

void ProyectoController::listarInversionesRealizadas(http_request request) {
    web::json::value json = request.extract_json().get();
    std::string nombreProyecto = utility::conversions::to_utf8string(json[U("nombreProyecto")].as_string());

    Proyecto proyecto(nombreProyecto, dbConn);
    try {
        proyecto.listarInversionesRealizadas();
        web::json::value respuesta;
        respuesta[L"message"] = web::json::value::string(U("Consulta de inversiones realizada con éxito."));
        request.reply(status_codes::OK, respuesta);
    }
    catch (const std::runtime_error& e) {
        web::json::value respuesta;
        respuesta[L"message"] = web::json::value::string(utility::conversions::to_string_t(e.what()));
        request.reply(status_codes::BadRequest, respuesta);
    }
}

void ProyectoController::eliminarProyecto(http_request request) {
    web::json::value json = request.extract_json().get();
    std::string nombreProyecto = utility::conversions::to_utf8string(json[U("nombreProyecto")].as_string());

    Proyecto proyecto(nombreProyecto, dbConn);
    if (!proyecto.existe(nombreProyecto)) {
        web::json::value respuesta;
        respuesta[L"message"] = web::json::value::string(U("El proyecto no existe."));
        request.reply(status_codes::BadRequest, respuesta);
        return;
    }

    try {
        proyecto.del(nombreProyecto);
        web::json::value respuesta;
        respuesta[L"message"] = web::json::value::string(U("Proyecto eliminado con éxito."));
        request.reply(status_codes::OK, respuesta);
    }
    catch (const std::runtime_error& e) {
        web::json::value respuesta;
        respuesta[L"message"] = web::json::value::string(utility::conversions::to_string_t(e.what()));
        request.reply(status_codes::BadRequest, respuesta);
    }
}


void ProyectoController::eliminarInversion(http_request request) {
    web::json::value json = request.extract_json().get();
    std::string nombreInversionista = utility::conversions::to_utf8string(json[U("nombreInversionista")].as_string());
    std::string nombreProyecto = utility::conversions::to_utf8string(json[U("nombreProyecto")].as_string());

    Proyecto proyecto(nombreProyecto, dbConn);
    try {
        proyecto.eliminarInversion(nombreInversionista, nombreProyecto);
        web::json::value respuesta;
        respuesta[L"message"] = web::json::value::string(U("Inversión eliminada con éxito."));
        request.reply(status_codes::OK, respuesta);
    }
    catch (const std::runtime_error& e) {
        web::json::value respuesta;
        respuesta[L"message"] = web::json::value::string(utility::conversions::to_string_t(e.what()));
        request.reply(status_codes::BadRequest, respuesta);
    }
}

void ProyectoController::cambiarNombreProyecto(http_request request) {
    web::json::value json = request.extract_json().get();
    std::string nombreActual = utility::conversions::to_utf8string(json[U("nombreActual")].as_string());
    std::string nuevoNombre = utility::conversions::to_utf8string(json[U("nuevoNombre")].as_string());

    Proyecto proyecto(nombreActual, dbConn);
    try {
        proyecto.read(nombreActual);
        proyecto.setNombre(nuevoNombre);
        proyecto.update(nombreActual);

        web::json::value respuesta;
        respuesta[L"message"] = web::json::value::string(U("Nombre del proyecto cambiado con éxito."));
        request.reply(status_codes::OK, respuesta);
    }
    catch (const std::runtime_error& e) {
        web::json::value respuesta;
        respuesta[L"message"] = web::json::value::string(utility::conversions::to_string_t(e.what()));
        request.reply(status_codes::BadRequest, respuesta);
    }
}