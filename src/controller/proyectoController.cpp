#include "proyectoController.h"

ProyectoController::ProyectoController() : inversionista(), proyecto(){}

void ProyectoController::crearProyecto(http_request request) {
    // Extracts the JSON from the request
    web::json::value json = request.extract_json().get();

    // Gets the project name and the amount to be raised from the JSON
    std::string nombreProyecto = utility::conversions::to_utf8string(json[U("nombreProyecto")].as_string());
    double cantidadARecaudar = json[U("cantidadARecaudar")].as_double();

    // Sets the project's name and the amount to be raised
    proyecto.setNombre(nombreProyecto);
    proyecto.setCantidadARecaudar(cantidadARecaudar);

    // Checks if the project already exists
    if (proyecto.existe(nombreProyecto)) {
        // If it does, sends a Bad Request response with an error message
        web::json::value respuesta;
        respuesta[L"message"] = web::json::value::string(U("El proyecto ya existe."));
        request.reply(status_codes::BadRequest, respuesta);
        return;
    }
    try {
        // Tries to create the project
        proyecto.create();

        // If successful, sends an OK response with a success message
        web::json::value respuesta;
        respuesta[L"message"] = web::json::value::string(U("Proyecto creado con éxito."));
        request.reply(status_codes::OK, respuesta);
    }
    catch (const std::runtime_error& e) {
        // If an error occurs during creation, sends a Bad Request response with the error message
        web::json::value respuesta;
        respuesta[L"message"] = web::json::value::string(utility::conversions::to_string_t(e.what()));
        request.reply(status_codes::BadRequest, respuesta);
    }
}

void ProyectoController::realizarInversion(http_request request) {
    // Extracts the JSON from the request
    web::json::value json = request.extract_json().get();

    // Gets the investor's name, project name, and investment amount from the JSON
    std::string nombreInversionista = utility::conversions::to_utf8string(json[U("nombreInversionista")].as_string());
    std::string nombreProyecto = utility::conversions::to_utf8string(json[U("nombreProyecto")].as_string());
    std::string cantidad = utility::conversions::to_utf8string(json[U("cantidad")].as_string());

    try {
        // Reads the project's and investor's data
        proyecto.read(nombreProyecto);
        inversionista.read(nombreInversionista);
        
        // Checks if the project is open
        if (proyecto.getEstado() == false) {
            // If not, sends a Bad Request response with an error message
            web::json::value respuesta;
            respuesta[L"message"] = web::json::value::string(U("Estado del proyecto: CERRADO.\nNo puede realizar más inversiones."));
            request.reply(status_codes::BadRequest, respuesta);
            return;
        }

        // Converts the investment amount to a double
        std::istringstream iss(cantidad);
        double dcantidad;
        iss >> dcantidad;

        // Checks if the investment exceeds the maximum amount the investor can make
        if (dcantidad > inversionista.getInversionMax()) {
            // If it does, sends a Bad Request response with an error message
            web::json::value respuesta;
            respuesta[L"message"] = web::json::value::string(U("La inversión supera la cantidad máxima que puede hacer el inversionista."));
            request.reply(status_codes::BadRequest, respuesta);
            return;
        }

        // Checks if the investment exceeds the amount to be raised for the project
        if ((proyecto.getCantidadRecaudada() + dcantidad) > proyecto.getCantidadARecaudar()) {
            // If it does, sends a Bad Request response with an error message
            web::json::value respuesta;
            respuesta[L"message"] = web::json::value::string(U("La inversión supera la cantidad a recaudar del proyecto."));
            request.reply(status_codes::BadRequest, respuesta);
            return;
        }

        // Makes the investment
        proyecto.realizarInversion(nombreInversionista, nombreProyecto, cantidad);

        // Sends an OK response with a success message
        web::json::value respuesta;
        respuesta[L"message"] = web::json::value::string(U("Inversión realizada con éxito."));
        request.reply(status_codes::OK, respuesta);
    }
    catch (const std::exception& e) {
        // If an error occurs, sends a Bad Request response with the error message
        web::json::value respuesta;
        respuesta[L"message"] = web::json::value::string(utility::conversions::to_string_t(e.what()));
        request.reply(status_codes::BadRequest, respuesta);
    }
}

void ProyectoController::consultarProyecto(http_request request) {
    // Extracts the JSON from the request
    web::json::value json = request.extract_json().get();

    // Gets the project name from the JSON
    std::string nombreProyecto = utility::conversions::to_utf8string(json[U("nombreProyecto")].as_string());

    // Reads the project's data
    proyecto.read(nombreProyecto);
    try {
        // Tries to get the project's data
        sql::ResultSet* res = proyecto.consultarProyecto(nombreProyecto);

        // Checks if the project exists
        if (res->next()) {
            // If it does, sends an OK response with the project data
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
            // If not, sends a Bad Request response with an error message
            web::json::value respuesta;
            respuesta[L"message"] = web::json::value::string(U("El proyecto no se encontró."));
            request.reply(status_codes::BadRequest, respuesta);
        }
    }
    catch (const std::exception& e) {
        // If an error occurs, sends a Bad Request response with the error message
        web::json::value respuesta;
        respuesta[L"message"] = web::json::value::string(utility::conversions::to_string_t(e.what()));
        request.reply(status_codes::BadRequest, respuesta);
    }
}


void ProyectoController::listarInversionesRealizadas(http_request request) {
    // Extracts the JSON from the request
    auto json = request.extract_json().get();

    // Gets the project name from the JSON
    std::string nombreProyecto = utility::conversions::to_utf8string(json[U("nombreProyecto")].as_string());

    // Reads the project's data
    proyecto.read(nombreProyecto);

    try {
        // Tries to get the list of investments made
        std::vector<std::tuple<int, std::string, double, std::string>> inversiones = proyecto.listarInversionesRealizadas(nombreProyecto);

        // Converts the investments to JSON
        std::vector<web::json::value> inversionesJson;
        for (const auto& inversion : inversiones) {
            // Converts the investments to JSON
            web::json::value inversionJson;
            inversionJson[U("id")] = web::json::value::number(std::get<0>(inversion));
            inversionJson[U("nombre")] = web::json::value::string(utility::conversions::to_string_t(std::get<1>(inversion)));
            inversionJson[U("cantidad")] = web::json::value::number(std::get<2>(inversion));
            inversionJson[U("fecha")] = web::json::value::string(utility::conversions::to_string_t(std::get<3>(inversion)));
            inversionesJson.push_back(inversionJson);
        }

        // Sends an OK response with the investments
        web::json::value respuesta;
        respuesta[U("inversiones")] = web::json::value::array(inversionesJson);

        request.reply(status_codes::OK, respuesta);
    }
    catch (const std::exception& e) {
        // If an error occurs, sends a Bad Request response with the error message
        web::json::value respuesta;
        respuesta[L"message"] = web::json::value::string(utility::conversions::to_string_t(e.what()));
        request.reply(status_codes::BadRequest, respuesta);
    }
}


void ProyectoController::eliminarProyecto(http_request request) {
    // Extracts the JSON from the request
    web::json::value json = request.extract_json().get();

    // Gets the project name from the JSON
    std::string nombreProyecto = utility::conversions::to_utf8string(json[U("nombreProyecto")].as_string());

    // Reads the project's data
    proyecto.read(nombreProyecto);

    // Checks if the project exists
    if (!proyecto.existe(nombreProyecto)) {
        // If not, sends a Bad Request response with an error message
        web::json::value respuesta;
        respuesta[L"message"] = web::json::value::string(U("El proyecto no existe."));
        request.reply(status_codes::BadRequest, respuesta);
        return;
    }

    try {
        // Tries to delete the project
        proyecto.del(nombreProyecto);

        // If successful, sends an OK response with a success message
        web::json::value respuesta;
        respuesta[L"message"] = web::json::value::string(U("Proyecto eliminado con éxito."));
        request.reply(status_codes::OK, respuesta);
    }
    catch (const std::runtime_error& e) {
        // If an error occurs during deletion, sends a Bad Request response with the error messag
        web::json::value respuesta;
        respuesta[L"message"] = web::json::value::string(utility::conversions::to_string_t(e.what()));
        request.reply(status_codes::BadRequest, respuesta);
    }
}

void ProyectoController::eliminarInversiones(http_request request) {
    // Extracts the JSON from the request
    web::json::value json = request.extract_json().get();

    // Gets the investor's name and project name from the JSON
    std::string nombreInversionista = utility::conversions::to_utf8string(json[U("nombreInversionista")].as_string());
    std::string nombreProyecto = utility::conversions::to_utf8string(json[U("nombreProyecto")].as_string());

    try {
        // Reads the project's data
        proyecto.read(nombreProyecto);

        // Checks if the project exists
        if (!proyecto.existe(nombreProyecto)) {
            // If not, sends a Bad Request response with an error message
            web::json::value respuesta;
            respuesta[L"message"] = web::json::value::string(U("Error: El proyecto no existe."));
            request.reply(status_codes::BadRequest, respuesta);
            return;
        }

        // Checks if the project has any investments
        if (!proyecto.tieneInversiones(nombreInversionista, nombreProyecto)) {
            // If not, sends a Bad Request response with an error message
            web::json::value respuesta;
            respuesta[L"message"] = web::json::value::string(U("Error: No hay inversiones disponibles para eliminar."));
            request.reply(status_codes::BadRequest, respuesta);
            return;
        }
        try {
            // Tries to delete the investments
            proyecto.eliminarInversiones(nombreInversionista, nombreProyecto);

            // If successful, sends an OK response with a success message
            web::json::value respuesta;
            respuesta[L"message"] = web::json::value::string(U("Inversión eliminada con éxito."));
            request.reply(status_codes::OK, respuesta);
        }
        catch (const std::runtime_error& e) {
            // If an error occurs during deletion, sends a Bad Request response with an error message
            web::json::value respuesta;
            respuesta[L"message"] = web::json::value::string(U("Error: No hay inversiones disponibles para eliminar."));
            request.reply(status_codes::BadRequest, respuesta);
        }
    }
    catch (const std::runtime_error& e) {
        // If an error occurs during this process, sends a Bad Request response with the error message
        web::json::value respuesta;
        respuesta[L"message"] = web::json::value::string(utility::conversions::to_string_t(e.what()));
        request.reply(status_codes::BadRequest, respuesta);
    }
}
