#include "Proyecto.h"
#include "inversionista.h"

Proyecto::Proyecto(const std::string& nombre, double cantidadARecaudar, DBConnection* dbConn)
    : nombre(nombre), cantidadARecaudar(cantidadARecaudar), dbConn(dbConn), cantidadRecaudada(0.0), estado(true) {}

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
    dbConn->ejecutarActualizacion(query);
}

void Proyecto::read(std::string qNombre) {
    sql::Statement* stmt = dbConn->getConnection()->createStatement();
    sql::ResultSet* res = stmt->executeQuery("SELECT * FROM proyectos WHERE nombre = '" + qNombre + "'");

    if (res->next()) {
        setNombre(res->getString("nombre"));
        setCantidadARecaudar(res->getDouble("cantidadARecaudar"));
        setCantidadRecaudada(res->getDouble("cantidadRecaudada"));
        setEstado(res->getBoolean("estado"));
    }
    else {
        throw std::runtime_error("Proyecto not found");
    }
}

void Proyecto::update(std::string qNombre) {
    std::string query = "UPDATE proyectos SET cantidadARecaudar = " + std::to_string(getCantidadARecaudar()) +
        ", cantidadRecaudada = " + std::to_string(getCantidadRecaudada()) +
        ", estado = " + (getEstado() ? "1" : "0") +
        " WHERE nombre = '" + qNombre + "'";
    dbConn->ejecutarActualizacion(query);
}

void Proyecto::del(std::string qNombre) {
    std::string query = "DELETE FROM proyectos WHERE nombre = '" + qNombre + "'";
    dbConn->ejecutarActualizacion(query);
}

int Proyecto::getId(const std::string& qNombre) {
    sql::Statement* stmt = dbConn->getConnection()->createStatement();
    sql::ResultSet* res = stmt->executeQuery("SELECT id FROM proyectos WHERE nombre = '" + qNombre + "'");

    if (res->next()) {
        return res->getInt("id");
    }
    else {
        throw std::runtime_error("Proyecto not found");
    }
}


// Investment operations
void Proyecto::agregarInversion(Inversionista* inversionista, double cantidad) {
    if (!getEstado()) {
        throw std::runtime_error("El proyecto está cerrado para inversiones.");
    }

    if (getCantidadRecaudada() + cantidad > getCantidadARecaudar()) {
        throw std::runtime_error("La inversión excede la cantidad a recaudar del proyecto.");
    }

    if (cantidad > inversionista->getInversionMax()) {
        throw std::runtime_error("La inversión excede el límite de inversión permitido para el inversionista.");
    }

    std::string query = "INSERT INTO inversiones(nombreInversionistaID, nombreProyectoID, cantidad) VALUES (" +
        std::to_string(inversionista->getId(inversionista->getNombre())) + ", " +
        std::to_string(getId(getNombre())) + ", " +
        std::to_string(cantidad) + ")";

    dbConn->ejecutarActualizacion(query);
    setCantidadRecaudada(getCantidadRecaudada() + cantidad);
}


void Proyecto::eliminarInversion(const std::string& nombreInversionista, const std::string& nombreProyecto) {
    Inversionista inversionista;
    sql::Statement* stmt = dbConn->getConnection()->createStatement();
    sql::ResultSet* res = stmt->executeQuery("SELECT cantidad FROM inversiones WHERE nombreInversionistaID = " + std::to_string(inversionista.getId(nombreInversionista)) + " AND nombreProyectoID = " + std::to_string(getId(nombreProyecto)));

    double cantidad = 0.0;
    if (res->next()) {
        cantidad = res->getDouble("cantidad");
    }

    std::string query = "DELETE FROM inversiones WHERE nombreInversionistaID = " + std::to_string(inversionista.getId(nombreInversionista)) + " AND nombreProyectoID = " + std::to_string(getId(nombreProyecto));
    dbConn->ejecutarActualizacion(query);

    setCantidadRecaudada(getCantidadRecaudada() - cantidad);
    if (getCantidadRecaudada() < getCantidadARecaudar()) {
        setEstado(true);
    }
}