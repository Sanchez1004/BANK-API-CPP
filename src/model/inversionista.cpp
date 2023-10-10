#include "inversionista.h"
#include "proyecto.h"

Inversionista::Inversionista() : nombre(""), tipo(""), ingresoMensual(0.0), inversionMax(0.0){}

// Getters
std::string Inversionista::getNombre() const { return nombre; }
std::string Inversionista::getTipo() const { return tipo; }
double Inversionista::getIngresoMensual() const { return ingresoMensual; }
double Inversionista::getInversionMax() const { return inversionMax; }

// Setters
void Inversionista::setNombre(const std::string& nombre) { this->nombre = nombre;   }
void Inversionista::setTipo(const std::string& tipo) { this->tipo = tipo; inversionConfig(tipo); }
void Inversionista::setIngresoMensual(double ingresoMensual) { this->ingresoMensual = ingresoMensual; }
void Inversionista::setInversionMax(double inversionMax) { this->inversionMax = inversionMax; }
void Inversionista::inversionConfig(std::string tipo) {
    if (tipo == "basic") {
        setIngresoMensual(4000000);
        setInversionMax (((getIngresoMensual() * 20) / 100));
    }
    else if (tipo == "full") {
        setIngresoMensual(8500000);
        setInversionMax(((getIngresoMensual() * 25) / 100));
    }
}

// Database Operations
void Inversionista::create() {
    // Preparar y ejecutar la consulta SQL
    std::string consulta = "INSERT INTO inversionistas(nombre, tipo, ingreso_mensual) VALUES ('"
        + getNombre() + "', '"
        + getTipo() + "', "
        + std::to_string(getIngresoMensual()) + ")";
    dbConn->executeQuery(consulta);
}

void Inversionista::read(std::string qNombre) {
    // Preparar y ejecutar la consulta SQL
    std::string consulta = "SELECT * FROM inversionistas WHERE nombre = '"
        + qNombre + "'";
    sql::ResultSet* res = dbConn->executeQuery(consulta);

    if (res->next()) {
        setNombre(res->getString("nombre"));
        setTipo(res->getString("tipo"));
        setIngresoMensual(res->getDouble("ingreso_mensual"));

    }
    else {
        throw std::runtime_error("Inversionista no encontrado");
    }
}

void Inversionista::update(std::string qNombre) {
    // Preparar y ejecutar la consulta SQL
    std::string consulta = "UPDATE inversionistas SET tipo = '"
        + getTipo() + "', ingreso_mensual = "
        + std::to_string(getIngresoMensual()) + " WHERE nombre = '"
        + qNombre + "'";
    dbConn->executeQuery(consulta);
}

void Inversionista::del(std::string qNombre) {
    // Preparar y ejecutar la consulta SQL
    std::string consulta = "DELETE FROM inversionistas WHERE nombre = '"
        + qNombre + "'";
    dbConn->executeQuery(consulta);
}

int Inversionista::getId(const std::string& qNombre) {
    // Preparar y ejecutar la consulta SQL
    std::string consulta = "SELECT id FROM inversionistas WHERE nombre = '"
        + qNombre + "'";
    sql::ResultSet* res = dbConn->executeQuery(consulta);

    if (res->next()) {
        return res->getInt("id");
    }
    else {
        throw std::runtime_error("Inversionista no encontrado");
    }
}

bool Inversionista::existe(const std::string& nombreInversionista) {
    try {
        // Preparar y ejecutar la consulta SQL
        std::string consulta = "SELECT COUNT(*) AS count FROM inversionistas WHERE nombre = '"
            + nombreInversionista + "'";
        sql::ResultSet* res = dbConn->executeQuery(consulta);

        bool exists = false;
        if (res->next()) {
            int count = res->getInt("count");
            exists = count > 0;
        }

        return exists;
    }
    catch (const sql::SQLException& e) {
        std::cerr << "Ocurrió un error: " << e.what() << '\n';
        throw;
    }
}   

// -------------------------------- Investment operations --------------------------------
std::vector<std::pair<std::string, double>> Inversionista::consultarEstadoInversiones(const std::string& nombreInversionista) {
    int idInversionista = getId(nombreInversionista);

    // Preparar y ejecutar la consulta SQL
    std::string consulta = "SELECT proyectos.nombre, inversiones.cantidad FROM inversiones INNER JOIN inversionistas ON inversiones.nombreInversionistaID = inversionistas.id INNER JOIN proyectos ON inversiones.nombreProyectoID = proyectos.id WHERE nombreInversionistaID = "
        + std::to_string(idInversionista);
    sql::ResultSet* res = dbConn->executeQuery(consulta);

    std::vector<std::pair<std::string, double>> inversiones;
    while (res->next()) {
        inversiones.push_back(std::make_pair(res->getString("nombre"), res->getDouble("cantidad")));
    }

    return inversiones;
}

double Inversionista::getInversionTotal() {
    // Preparar y ejecutar la consulta SQL
    std::string consulta = "SELECT SUM(cantidad) as total FROM inversiones WHERE nombreInversionistaID = "
        + std::to_string(getId(getNombre()));
    sql::ResultSet* res = dbConn->executeQuery(consulta);

    if (res->next()) {
        return res->getDouble("total");
    }
    else {
        return 0.0;
    }
}

sql::ResultSet* Inversionista::verUsuarios() {
    // Preparar y ejecutar la consulta SQL
    std::string consulta = "SELECT * FROM inversionistas";
    sql::ResultSet* res = dbConn->executeQuery(consulta);

    return res;
}
