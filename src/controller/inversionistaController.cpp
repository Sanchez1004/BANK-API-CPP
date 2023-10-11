#include "InversionistaController.h"
#include "model/inversionista.h"

InversionistaController::InversionistaController() : inversionista(){}

void InversionistaController::crearUsuario(http_request request) {
    try {
        // Extracts the JSON from the request
        web::json::value json = request.extract_json().get();

        // Gets the investor's name and type from the JSON
        std::string nombre = utility::conversions::to_utf8string(json[U("nombre")].as_string());
        std::string tipo = utility::conversions::to_utf8string(json[U("tipo")].as_string());

        // Checks if the investor's name contains any digits
        if (std::any_of(nombre.begin(), nombre.end(), ::isdigit)) {
            // If it does, sends a Bad Request response with an error message
            web::json::value respuesta;
            respuesta[L"message"] = web::json::value::string(U("El nombre del inversionista no puede contener números."));
            request.reply(status_codes::BadRequest, respuesta);
            return;
        }

        // Checks if the investor's type is either 'basic' or 'full'
        if (tipo != "basic" && tipo != "full") {
            // If not, sends a Bad Request response with an error message
            web::json::value respuesta;
            respuesta[L"message"] = web::json::value::string(U("El tipo de inversionista debe ser 'basic' o 'full'."));
            request.reply(status_codes::BadRequest, respuesta);
            return;
        }

        // Sets the investor's name and type
        inversionista.setNombre(nombre);
        inversionista.setTipo(tipo);

        // Creates the investor
        inversionista.create();

        // Sends an OK response with a success message
        web::json::value respuesta;
        respuesta[L"message"] = web::json::value::string(U("Usuario creado exitosamente"));
        request.reply(status_codes::OK, respuesta);
    }
    catch (const std::exception& e) {
        // Sends an Internal Server Error response with an error message
        web::json::value respuesta;
        respuesta[L"message"] = web::json::value::string(U("Error al crear el usuario: ") + utility::conversions::to_string_t(e.what()));
        request.reply(status_codes::BadRequest, respuesta);
    }
}



void InversionistaController::modificarUsuario(http_request request) {
    // Extracts the JSON from the request
    auto json = request.extract_json().get();

    // Gets the investor's name and type from the JSON
    std::string nombreInversionista = utility::conversions::to_utf8string(json[U("nombre")].as_string());
    std::string tipo = utility::conversions::to_utf8string(json[U("tipo")].as_string());

    // Checks if the investor's name contains any digits
    if (std::any_of(nombreInversionista.begin(), nombreInversionista.end(), ::isdigit)) {
        // If it does, sends a Bad Request response with an error message
        web::json::value respuesta;
        respuesta[L"message"] = web::json::value::string(U("El nombre del inversionista no puede contener números."));
        request.reply(status_codes::BadRequest, respuesta);
        return;
    }

    // Checks if the investor's type is either 'basic' or 'full'
    if (tipo != "basic" && tipo != "full") {
        // If not, sends a Bad Request response with an error message
        web::json::value respuesta;
        respuesta[L"message"] = web::json::value::string(U("El tipo de inversionista debe ser 'basic' o 'full'."));
        request.reply(status_codes::BadRequest, respuesta);
        return;
    }

    try {
        // Reads the investor's data
        inversionista.read(nombreInversionista);

        // Sets the investor's type
        inversionista.setTipo(tipo);

        // Configures the investment based on the type
        inversionista.inversionConfig(tipo);

        // Updates the investor's data
        inversionista.update(nombreInversionista);

        // Sends an OK response with a success message
        web::json::value respuesta;
        respuesta[L"message"] = web::json::value::string(U("Usuario modificado exitosamente"));
        request.reply(status_codes::OK, respuesta);
    }
    catch (const std::exception& e) {
        // If an error occurs, sends a Bad Request response with the error message
        web::json::value respuesta;
        respuesta[L"message"] = web::json::value::string(utility::conversions::to_string_t(e.what()));
        request.reply(status_codes::BadRequest, respuesta);
    }
}

void InversionistaController::verUsuarios(http_request request) {
    // Calls the verUsuarios method of the Inversionista class to get all users
    sql::ResultSet* res = inversionista.verUsuarios();

    // Creates a JSON object to store the response
    web::json::value respuesta;
    int i = 0;

    // Checks if there are any users
    if (!res->next()) {
        // If not, sends a Not Found response with an error message
        respuesta[L"message"] = web::json::value::string(U("No se encontraron usuarios."));
        request.reply(status_codes::NotFound, respuesta);
    }
    else {
        // If there are users, iterates over them
        do {
            // Creates a JSON object to store the user data
            web::json::value usuario;
            usuario[U("id")] = web::json::value::number(res->getInt("id"));
            usuario[U("nombre")] = web::json::value::string(utility::conversions::to_string_t(res->getString("nombre")));
            usuario[U("tipo")] = web::json::value::string(utility::conversions::to_string_t(res->getString("tipo")));
            usuario[U("ingreso_mensual")] = web::json::value::number(static_cast<double>(res->getDouble("ingreso_mensual")));

            // Adds the user data to the response
            respuesta[i++] = usuario;
        } while (res->next());

        // Sends an OK response with the user data
        request.reply(status_codes::OK, respuesta);
    }
}

void InversionistaController::eliminarUsuario(http_request request) {
    // Extracts the JSON from the request
    web::json::value json = request.extract_json().get();

    // Gets the investor's name from the JSON
    std::string nombreInversionista = utility::conversions::to_utf8string(json[U("nombreInversionista")].as_string());

    // Checks if the investor exists
    if (!inversionista.existe(nombreInversionista)) {
        // If not, sends a Bad Request response with an error message
        web::json::value respuesta;
        respuesta[L"message"] = web::json::value::string(U("Error: El usuario no existe."));
        request.reply(status_codes::BadRequest, respuesta);
        return;
    }

    // Reads the investor's data
    inversionista.read(nombreInversionista);
    try {
        // Tries to delete the investor
        inversionista.del(nombreInversionista);

        // If successful, sends an OK response with a success message
        web::json::value respuesta;
        respuesta[L"message"] = web::json::value::string(U("Usuario eliminado con éxito."));
        request.reply(status_codes::OK, respuesta);
    }
    catch (const std::runtime_error& e) {
        // If an error occurs, sends a Bad Request response with the error message
        web::json::value respuesta;
        respuesta[L"message"] = web::json::value::string(utility::conversions::to_string_t(e.what()));
        request.reply(status_codes::BadRequest, respuesta);
    }
}

void InversionistaController::consultarEstadoInversiones(http_request request) {
    // Extracts the JSON from the request
    web::json::value json = request.extract_json().get();

    // Gets the investor's name from the JSON
    std::string nombreInversionista = utility::conversions::to_utf8string(json[U("nombreInversionista")].as_string());

    // Reads the investor's data
    inversionista.read(nombreInversionista);
    try {
        // Tries to get the state of the investments
        std::vector<std::pair<std::string, double>> inversiones = inversionista.consultarEstadoInversiones(nombreInversionista);

        // Converts the investments to JSON
        std::vector<web::json::value> inversionesJson;
        for (const auto& inversion : inversiones) {
            web::json::value inversionJson;
            inversionJson[U("nombreProyecto")] = web::json::value::string(utility::conversions::to_string_t(inversion.first));
            inversionJson[U("cantidad")] = web::json::value::number(inversion.second);
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
