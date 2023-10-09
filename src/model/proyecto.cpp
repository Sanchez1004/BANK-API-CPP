#include "Proyecto.h"
#include "inversionista.h"

Proyecto::Proyecto(const std::string& nombre, double cantidadARecaudar, DBConnection* dbConn)
    : nombre(nombre), cantidadARecaudar(cantidadARecaudar), dbConn(dbConn), cantidadRecaudada(0.0), estado(true) {}

Proyecto::Proyecto(std::string& nombreProyecto, DBConnection* dbConn) : dbConn(dbConn) {
    read(nombreProyecto);
}
Proyecto::Proyecto(DBConnection* dbConn) : dbConn(dbConn) {};


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

void Proyecto::read(std::string nombreProyecto) {
    try {
        std::string query = "SELECT * FROM proyectos WHERE nombre = '" + nombreProyecto + "'";
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
        std::cerr << "Ocurrió un error: " << e.what() << '\n';
        throw;
    }
}

void Proyecto::update(std::string nombreProyecto) {
    std::string query = "UPDATE proyectos SET cantidadARecaudar = " + std::to_string(getCantidadARecaudar()) +
        ", cantidadRecaudada = " + std::to_string(getCantidadRecaudada()) +
        ", estado = " + (getEstado() ? "1" : "0") +
        " WHERE nombre = '" + nombreProyecto + "'";
    dbConn->ejecutarQuery(query);
}

void Proyecto::del(std::string nombreProyecto) {
    std::string query = "SELECT * FROM proyectos WHERE nombre = '" + nombreProyecto + "'";
    sql::ResultSet* res = dbConn->ejecutarQueryR(query);

    if (!res->next()) {
        throw std::runtime_error("El proyecto no existe.");
    }

    query = "DELETE FROM proyectos WHERE nombre = '" + nombreProyecto + "'";
    dbConn->ejecutarQuery(query);
}



int Proyecto::getId(const std::string& nombreProyecto) {
    sql::Statement* stmt = dbConn->getConnection()->createStatement();
    sql::ResultSet* res = stmt->executeQuery("SELECT id FROM proyectos WHERE nombre = '" + nombreProyecto + "'");

    if (res->next()) {
        return res->getInt("id");
    }
    else {
        throw std::runtime_error("Proyecto no encontrado");
    }
}

bool Proyecto::existe(const std::string& nombreProyecto) {
    try {
        std::string query = "SELECT COUNT(*) AS count FROM proyectos WHERE nombre = '" + nombreProyecto + "'";
        sql::ResultSet* res = dbConn->ejecutarQueryR(query);

        bool exists = false;
        if (res->next()) {
            int count = res->getInt("count");
            exists = count > 0;
        }

        delete res;

        return exists;
    }
    catch (const sql::SQLException& e) {
        std::cerr << "Ocurrió un error: " << e.what() << '\n';
        throw;
    }
}


// Investment operations
void Proyecto::realizarInversion(std::string nombreInversionista, std::string nombreProyecto, std::string cantidad) {
    try {
        Inversionista inversionista(nombreInversionista, dbConn);
        int idInversionista = inversionista.getId(nombreInversionista);
        int idProyecto = getId(nombreProyecto);

        std::istringstream iss(cantidad);
        double dcantidad;
        iss >> dcantidad; // Convertir string a double

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

        dbConn->ejecutarQueryR(query);
        setCantidadRecaudada(getCantidadRecaudada() + dcantidad);
    }
    catch (const std::exception& e) {
        std::cerr << "Ocurrió un error: " << e.what() << '\n';
        throw;
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