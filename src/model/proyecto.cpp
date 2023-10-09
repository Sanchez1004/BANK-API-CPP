#include "Proyecto.h"
#include "inversionista.h"

Proyecto::Proyecto(const std::string& nombre, double cantidadARecaudar, DBConnection* dbConn)
    : nombre(nombre), cantidadARecaudar(cantidadARecaudar), dbConn(dbConn), cantidadRecaudada(0.0), estado(true) {}

Proyecto::Proyecto(std::string& nombreProyecto, DBConnection* dbConn) : dbConn(dbConn) {
    read(nombreProyecto);
}

// Getters
std::string Proyecto::getNombre() const { return nombre; }
double Proyecto::getCantidadARecaudar() const { return cantidadARecaudar; }
double Proyecto::getCantidadRecaudada() const { return cantidadRecaudada; }
bool Proyecto::getEstado() const { return estado; }

// Setters
void Proyecto::setNombre(const std::string& nombre) { this->nombre = nombre; }
void Proyecto::setCantidadARecaudar(double cantidadARecaudar) { this->cantidadARecaudar = cantidadARecaudar; }
void Proyecto::setCantidadRecaudada(double cantidadRecaudada) { this->cantidadRecaudada = cantidadRecaudada; }
void Proyecto::setEstado(bool estado) { this->estado = estado; }

// Database operations
void Proyecto::create() {
    std::string query = "INSERT INTO proyectos(nombre, cantidadARecaudar, cantidadRecaudada, estado) VALUES ('" +
        getNombre() + "', " +
        std::to_string(getCantidadARecaudar()) + ", " +
        std::to_string(getCantidadRecaudada()) + ", " +
        (getEstado() ? "1" : "0") + ")";
    dbConn->ejecutarQuery(query);
}

void Proyecto::read(std::string qNombre) {
    try {
        std::string query = "SELECT * FROM proyectos WHERE nombre = '" + qNombre + "'";
        sql::ResultSet* res = dbConn->ejecutarQueryR(query);

        if (res->next()) {
            setNombre(res->getString("nombre"));
            setCantidadARecaudar(res->getDouble("cantidadARecaudar"));
            setCantidadRecaudada(res->getDouble("cantidadRecaudada"));
            setEstado(res->getBoolean("estado"));
        }
        else {
            throw std::runtime_error("Proyecto no encontrado");
        }
    }
    catch (const std::runtime_error& e) {
        std::cerr << "Runtime error: " << e.what() << std::endl;
    }
}

void Proyecto::read(std::string qNombre, http_request request, web::json::value json) {
    try {
        std::string query = "SELECT * FROM proyectos WHERE nombre = '" + qNombre + "'";
        sql::ResultSet* res = dbConn->ejecutarQueryR(query, request,  json);

        if (res->next()) {
            setNombre(res->getString("nombre"));
            setCantidadARecaudar(res->getDouble("cantidadARecaudar"));
            setCantidadRecaudada(res->getDouble("cantidadRecaudada"));
            setEstado(res->getBoolean("estado"));
        }
        else {
            web::json::value respuesta;
            respuesta[L"message"] = web::json::value::string(utility::conversions::to_string_t("Proyecto no encontrado"));
            request.reply(status_codes::BadRequest, respuesta);
        }
    }
    catch (const std::runtime_error& e) {
        web::json::value respuesta;
        respuesta[L"message"] = web::json::value::string(utility::conversions::to_string_t(e.what()));
        request.reply(status_codes::BadRequest, respuesta);
    }
}

void Proyecto::update(std::string qNombre) {
    std::string query = "UPDATE proyectos SET cantidadARecaudar = " + std::to_string(getCantidadARecaudar()) +
        ", cantidadRecaudada = " + std::to_string(getCantidadRecaudada()) +
        ", estado = " + (getEstado() ? "1" : "0") +
        " WHERE nombre = '" + qNombre + "'";
    dbConn->ejecutarQuery(query);
}

void Proyecto::del(std::string qNombre) {
    std::string query = "DELETE FROM proyectos WHERE nombre = '" + qNombre + "'";
    dbConn->ejecutarQuery(query);
}

int Proyecto::getId(const std::string& qNombre) {
    sql::Statement* stmt = dbConn->getConnection()->createStatement();
    sql::ResultSet* res = stmt->executeQuery("SELECT id FROM proyectos WHERE nombre = '" + qNombre + "'");

    if (res->next()) {
        return res->getInt("id");
    }
    else {
        throw std::runtime_error("Proyecto no encontrado");
    }
}

// Investment operations
void Proyecto::realizarInversion(std::string nombreInversionista, std::string nombreProyecto, std::string cantidad, http_request request, web::json::value json) {
    try {
        Inversionista inversionista(nombreInversionista, dbConn);
        int idInversionista = inversionista.getId(nombreInversionista);
        int idProyecto = getId(nombreProyecto);
        double dcantidad = std::stod(cantidad);

        if (getCantidadRecaudada() + dcantidad > getCantidadARecaudar()) {
            throw std::runtime_error("La inversión excede la cantidad a recaudar del proyecto.");
        }

        if (dcantidad > inversionista.getInversionMax()) {
            throw std::runtime_error("La inversión excede el límite de inversión permitido para el inversionista.");
        }

        std::string query = "INSERT INTO inversiones (nombreInversionistaID, nombreProyectoID, cantidad) VALUES ("
            + std::to_string(idInversionista) + ", "
            + std::to_string(idProyecto) + ", "
            + std::to_string(dcantidad) + ")";

        dbConn->ejecutarQueryR(query, request, json);
        setCantidadRecaudada(getCantidadRecaudada() + dcantidad);

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

void Proyecto::eliminarInversion(const std::string& nombreInversionista, const std::string& nombreProyecto) {
    Inversionista inversionista(dbConn);
    sql::Statement* stmt = dbConn->getConnection()->createStatement();
    sql::ResultSet* res = stmt->executeQuery("SELECT cantidad FROM inversiones WHERE nombreInversionistaID = " + std::to_string(inversionista.getId(nombreInversionista)) + " AND nombreProyectoID = " + std::to_string(getId(nombreProyecto)));

    double cantidad = 0.0;
    if (res->next()) {
        cantidad = res->getDouble("cantidad");
    }

    std::string query = "DELETE FROM inversiones WHERE nombreInversionistaID = " + std::to_string(inversionista.getId(nombreInversionista)) + " AND nombreProyectoID = " + std::to_string(getId(nombreProyecto));
    dbConn->ejecutarQuery(query);

    setCantidadRecaudada(getCantidadRecaudada() - cantidad);
}


void Proyecto::listarInversionesRealizadas() {
    sql::Statement* stmt = dbConn->getConnection()->createStatement();
    sql::ResultSet* res = stmt->executeQuery("SELECT inversiones.id, inversionistas.nombre, inversiones.cantidad, inversiones.fecha FROM inversiones INNER JOIN inversionistas ON inversiones.nombreInversionistaID = inversionistas.id WHERE nombreProyectoID = " + std::to_string(getId(getNombre())));

    if (!res->next()) {
        throw std::runtime_error("No se encontraron inversiones para este proyecto.");
    }
    else {
        do {
            std::cout << "ID de la inversión: " << res->getInt("id") << std::endl;
            std::cout << "Nombre del inversionista: " << res->getString("nombre") << std::endl;
            std::cout << "Cantidad invertida: " << res->getDouble("cantidad") << std::endl;
            std::cout << "Fecha de la inversión: " << res->getString("fecha") << std::endl;
        } while (res->next());
    }
}