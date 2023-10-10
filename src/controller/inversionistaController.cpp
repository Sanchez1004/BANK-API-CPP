#include "InversionistaController.h"
#include "model/inversionista.h"

InversionistaController::InversionistaController() : inversionista(){}

void InversionistaController::crearUsuario(http_request request) {
    web::json::value json = request.extract_json().get();
    std::string nombre = utility::conversions::to_utf8string(json[U("nombre")].as_string());
    std::string tipo = utility::conversions::to_utf8string(json[U("tipo")].as_string());

    if (std::any_of(nombre.begin(), nombre.end(), ::isdigit)) {
        web::json::value respuesta;
        respuesta[L"message"] = web::json::value::string(U("El nombre del inversionista no puede contener números."));
        request.reply(status_codes::BadRequest, respuesta);
        return;
    }

    if (tipo != "basic" && tipo != "full") {
        web::json::value respuesta;
        respuesta[L"message"] = web::json::value::string(U("El tipo de inversionista debe ser 'basic' o 'full'."));
        request.reply(status_codes::BadRequest, respuesta);
        return;
    }

    inversionista.setNombre(nombre);
    inversionista.setTipo(tipo);
    inversionista.create();

    web::json::value respuesta;
    respuesta[L"message"] = web::json::value::string(U("Usuario creado exitosamente"));
    request.reply(status_codes::OK, respuesta);
}


void InversionistaController::modificarUsuario(http_request request) {
    auto json = request.extract_json().get();
    std::string nombreInversionista = utility::conversions::to_utf8string(json[U("nombre")].as_string());
    std::string tipo = utility::conversions::to_utf8string(json[U("tipo")].as_string());

    if (std::any_of(nombreInversionista.begin(), nombreInversionista.end(), ::isdigit)) {
        web::json::value respuesta;
        respuesta[L"message"] = web::json::value::string(U("El nombre del inversionista no puede contener números."));
        request.reply(status_codes::BadRequest, respuesta);
        return;
    }

    if (tipo != "basic" && tipo != "full") {
        web::json::value respuesta;
        respuesta[L"message"] = web::json::value::string(U("El tipo de inversionista debe ser 'basic' o 'full'."));
        request.reply(status_codes::BadRequest, respuesta);
        return;
    }

    try {
        inversionista.read(nombreInversionista);
        inversionista.setTipo(tipo);
        inversionista.inversionConfig(tipo);
        inversionista.update(nombreInversionista);

        web::json::value respuesta;
        respuesta[L"message"] = web::json::value::string(U("Usuario modificado exitosamente"));
        request.reply(status_codes::OK, respuesta);
    }
    catch (const std::exception& e) {
        web::json::value respuesta;
        respuesta[L"message"] = web::json::value::string(utility::conversions::to_string_t(e.what()));
        request.reply(status_codes::BadRequest, respuesta);
    }
}

void InversionistaController::verUsuarios(http_request request) {
    sql::ResultSet* res = inversionista.verUsuarios();

    web::json::value respuesta;
    int i = 0;

    if (!res->next()) {
        respuesta[L"message"] = web::json::value::string(U("No se encontraron usuarios."));
        request.reply(status_codes::NotFound, respuesta);
    }
    else {
        do {
            web::json::value usuario;
            usuario[U("id")] = web::json::value::number(res->getInt("id"));
            usuario[U("nombre")] = web::json::value::string(utility::conversions::to_string_t(res->getString("nombre")));
            usuario[U("tipo")] = web::json::value::string(utility::conversions::to_string_t(res->getString("tipo")));
            usuario[U("ingreso_mensual")] = web::json::value::number(static_cast<double>(res->getDouble("ingreso_mensual")));

            respuesta[i++] = usuario;
        } while (res->next());

        request.reply(status_codes::OK, respuesta);
    }
}

void InversionistaController::eliminarUsuario(http_request request) {
    web::json::value json = request.extract_json().get();
    std::string nombreInversionista = utility::conversions::to_utf8string(json[U("nombreInversionista")].as_string());

    if (!inversionista.existe(nombreInversionista)) {
        web::json::value respuesta;
        respuesta[L"message"] = web::json::value::string(U("Error: El usuario no existe."));
        request.reply(status_codes::BadRequest, respuesta);
        return;
    }

    inversionista.read(nombreInversionista);
    try {
        inversionista.del(nombreInversionista);
        web::json::value respuesta;
        respuesta[L"message"] = web::json::value::string(U("Usuario eliminado con éxito."));
        request.reply(status_codes::OK, respuesta);
    }
    catch (const std::runtime_error& e) {
        web::json::value respuesta;
        respuesta[L"message"] = web::json::value::string(utility::conversions::to_string_t(e.what()));
        request.reply(status_codes::BadRequest, respuesta);
    }
}

void InversionistaController::consultarEstadoInversiones(http_request request) {
    auto json = request.extract_json().get();
    std::string nombreInversionista = utility::conversions::to_utf8string(json[U("nombreInversionista")].as_string());

    inversionista.read(nombreInversionista);
    try {
        std::vector<std::pair<std::string, double>> inversiones = inversionista.consultarEstadoInversiones(nombreInversionista);

        std::vector<web::json::value> inversionesJson;
        for (const auto& inversion : inversiones) {
            web::json::value inversionJson;
            inversionJson[U("nombreProyecto")] = web::json::value::string(utility::conversions::to_string_t(inversion.first));
            inversionJson[U("cantidad")] = web::json::value::number(inversion.second);
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