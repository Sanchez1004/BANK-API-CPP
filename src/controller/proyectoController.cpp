#include "proyectoController.h"

ProyectoController::ProyectoController() : inversionista(), proyecto(){}

void ProyectoController::crearProyecto(http_request request) {
    web::json::value json = request.extract_json().get();
    std::string nombreProyecto = utility::conversions::to_utf8string(json[U("nombreProyecto")].as_string());
    double cantidadARecaudar = json[U("cantidadARecaudar")].as_double();
    proyecto.setNombre(nombreProyecto);
    proyecto.setCantidadARecaudar(cantidadARecaudar);

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

    try {
        proyecto.read(nombreProyecto);
        inversionista.read(nombreInversionista);
        
        if (proyecto.getEstado() == false) {
            web::json::value respuesta;
            respuesta[L"message"] = web::json::value::string(U("Estado del proyecto: CERRADO.\nNo puede realizar más inversiones."));
            request.reply(status_codes::BadRequest, respuesta);
            return;
        }

        std::istringstream iss(cantidad);
        double dcantidad;
        iss >> dcantidad;

        // Verificar si la inversión supera la cantidad máxima que puede hacer un inversionista
        if (dcantidad > inversionista.getInversionMax()) {
            web::json::value respuesta;
            respuesta[L"message"] = web::json::value::string(U("La inversión supera la cantidad máxima que puede hacer el inversionista."));
            request.reply(status_codes::BadRequest, respuesta);
            return;
        }

        // Verificar si la inversión supera la cantidad a recaudar del proyecto
        if ((proyecto.getCantidadRecaudada() + dcantidad) > proyecto.getCantidadARecaudar()) {
            web::json::value respuesta;
            respuesta[L"message"] = web::json::value::string(U("La inversión supera la cantidad a recaudar del proyecto."));
            request.reply(status_codes::BadRequest, respuesta);
            return;
        }

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

    proyecto.read(nombreProyecto);
    try {
        sql::ResultSet* res = proyecto.consultarProyecto(nombreProyecto);

        if (res->next()) {
            web::json::value respuesta;
            respuesta[L"nombre"] = web::json::value::string(utility::conversions::to_string_t(res->getString("nombre")));
            respuesta[L"cantidadARecaudar"] = web::json::value::number(static_cast<int>(res->getDouble("cantidadARecaudar")));
            respuesta[L"cantidadRecaudada"] = web::json::value::number(static_cast<int>(res->getDouble("cantidadRecaudada")));
            if (res->getBoolean("estado") == 1) {
                respuesta[L"estado"] = web::json::value::string(utility::conversions::to_string_t("Abierto"));
            }
            else {
                respuesta[L"estado"] = web::json::value::string(utility::conversions::to_string_t("cerrado"));
            }

            request.reply(status_codes::OK, respuesta);
        }
        else {
            // Enviar una respuesta HTTP indicando que el proyecto no se encontró.
            web::json::value respuesta;
            respuesta[L"message"] = web::json::value::string(U("El proyecto no se encontró."));
            request.reply(status_codes::BadRequest, respuesta);
        }
    }
    catch (const std::exception& e) {
        web::json::value respuesta;
        respuesta[L"message"] = web::json::value::string(utility::conversions::to_string_t(e.what()));
        request.reply(status_codes::BadRequest, respuesta);
    }
}


void ProyectoController::listarInversionesRealizadas(http_request request) {
    auto json = request.extract_json().get();
    std::string nombreProyecto = utility::conversions::to_utf8string(json[U("nombreProyecto")].as_string());

    proyecto.read(nombreProyecto);

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

    proyecto.read(nombreProyecto);
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

void ProyectoController::eliminarInversiones(http_request request) {
    web::json::value json = request.extract_json().get();
    std::string nombreInversionista = utility::conversions::to_utf8string(json[U("nombreInversionista")].as_string());
    std::string nombreProyecto = utility::conversions::to_utf8string(json[U("nombreProyecto")].as_string());

    try {
        proyecto.read(nombreProyecto);
        if (!proyecto.existe(nombreProyecto)) {
            web::json::value respuesta;
            respuesta[L"message"] = web::json::value::string(U("Error: El proyecto no existe."));
            request.reply(status_codes::BadRequest, respuesta);
            return;
        }
        if (!proyecto.tieneInversiones(nombreInversionista, nombreProyecto)) {
            web::json::value respuesta;
            respuesta[L"message"] = web::json::value::string(U("Error: No hay inversiones disponibles para eliminar."));
            request.reply(status_codes::BadRequest, respuesta);
            return;
        }
        try {
            proyecto.eliminarInversiones(nombreInversionista, nombreProyecto);
            web::json::value respuesta;
            respuesta[L"message"] = web::json::value::string(U("Inversión eliminada con éxito."));
            request.reply(status_codes::OK, respuesta);
        }
        catch (const std::runtime_error& e) {
            web::json::value respuesta;
            respuesta[L"message"] = web::json::value::string(U("Error: No hay inversiones disponibles para eliminar."));
            request.reply(status_codes::BadRequest, respuesta);
        }
    }
    catch (const std::runtime_error& e) {
        web::json::value respuesta;
        respuesta[L"message"] = web::json::value::string(utility::conversions::to_string_t(e.what()));
        request.reply(status_codes::BadRequest, respuesta);
    }
}
