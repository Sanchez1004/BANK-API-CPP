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

        if (proyecto.getCantidadRecaudada() > proyecto.getCantidadARecaudar()) {
            web::json::value respuesta;
            respuesta[L"message"] = web::json::value::string(U("La inversión supera la cantidad a recaudar del proyecto."));
            request.reply(status_codes::BadRequest, respuesta);
            return;
        }

        Inversionista inversionista(nombreInversionista, dbConn);
        if (std::stod(cantidad) > inversionista.getInversionMax()) {
            web::json::value respuesta;
            respuesta[L"message"] = web::json::value::string(U("La inversión supera la cantidad máxima que puede hacer el inversionista."));
            request.reply(status_codes::BadRequest, respuesta);
            return;
        }

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
    auto json = request.extract_json().get();
    std::string nombreProyecto = utility::conversions::to_utf8string(json[U("nombreProyecto")].as_string());

    Proyecto proyecto(dbConn);
    try {
        std::vector<std::tuple<int, std::string, double, std::string>> inversiones = proyecto.listarInversionesRealizadas(nombreProyecto);

        std::vector<web::json::value> inversionesJson;
        for (const auto& inversion : inversiones) {
            web::json::value inversionJson;
            inversionJson[U("id")] = web::json::value::number(std::get<0>(inversion));
            inversionJson[U("nombre")] = web::json::value::string(utility::conversions::to_string_t(std::get<1>(inversion)));
            inversionJson[U("cantidad")] = web::json::value::number(std::get<2>(inversion));
            inversionJson[U("fecha")] = web::json::value::string(utility::conversions::to_string_t(std::get<3>(inversion)));
            inversionesJson.push_back(inversionJson);
        }

        web::json::value respuesta;
        respuesta[U("inversiones")] = web::json::value::array(inversionesJson);

        request.reply(status_codes::OK, respuesta);
    }
    catch (const std::exception& e) {
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