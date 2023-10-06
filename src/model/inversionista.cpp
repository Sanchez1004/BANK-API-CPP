#include "inversionista.h"
#include "proyecto.h"

Inversionista::Inversionista(const std::string& nombre, const std::string& tipo, DBConnection* dbConn)
    : nombre(nombre), tipo(tipo), dbConn(dbConn) {
    inversionConfig(tipo);
}

Inversionista::Inversionista(){}

// Getters
std::string Inversionista::getNombre() const { return nombre; }
std::string Inversionista::getTipo() const { return tipo; }
double Inversionista::getIngresoMensual() const { return ingresoMensual; }
double Inversionista::getInversionMax() const { return inversionMax; }

// Setters
void Inversionista::setNombre(const std::string& nombre) { this->nombre = nombre; }
void Inversionista::setTipo(const std::string& tipo) { this->tipo = tipo; }
void Inversionista::setIngresoMensual(double ingresoMensual) { this->ingresoMensual = ingresoMensual; }
void Inversionista::setInversionMax(double inversionMax) { this->inversionMax = inversionMax; }
void Inversionista::inversionConfig(std::string tipo) {
    if (tipo == "basic") {
        setIngresoMensual(4000000);
        inversionMax = ((getIngresoMensual() * 20) / 100);
    }
    else if (tipo == "full") {
        setIngresoMensual(8500000);
        inversionMax = ((getIngresoMensual() * 20) / 100);
    }
}

// Database Operations
void Inversionista::create() {
    std::string query = "INSERT INTO inversionistas(nombre, tipo, ingreso_mensual) VALUES ('" +
        getNombre() + "', '" +
        getTipo() + "', " +
        std::to_string(getIngresoMensual()) + ")";
    dbConn->ejecutarActualizacion(query);
}

void Inversionista::read(std::string qNombre) {
    sql::Statement* stmt = dbConn->getConnection()->createStatement();
    sql::ResultSet* res = stmt->executeQuery("SELECT * FROM inversionistas WHERE nombre = '" + qNombre + "'");

    if (res->next()) {
        setNombre(res->getString("nombre"));
        setTipo(res->getString("tipo"));
        setIngresoMensual(res->getDouble("ingreso_mensual"));
    }
    else {
        throw std::runtime_error("Inversionista not found");
    }
}

void Inversionista::update(std::string qNombre) {
    std::string query = "UPDATE inversionistas SET tipo = '" + getTipo() +
        "', ingreso_mensual = " + std::to_string(getIngresoMensual()) +
        " WHERE nombre = '" + qNombre + "'";
    dbConn->ejecutarActualizacion(query);
}

void Inversionista::del(std::string qNombre) {
    std::string query = "DELETE FROM inversionistas WHERE nombre = '" + qNombre + "'";
    dbConn->ejecutarActualizacion(query);
}

int Inversionista::getId(const std::string& qNombre) {
    sql::Statement* stmt = dbConn->getConnection()->createStatement();
    sql::ResultSet* res = stmt->executeQuery("SELECT id FROM inversionistas WHERE nombre = '" + qNombre + "'");

    if (res->next()) {
        return res->getInt("id");
    }
    else {
        throw std::runtime_error("Inversionista not found");
    }
}

// Investment Operations
std::string Inversionista::realizarInversion(std::string nombreProyecto, double cantidad) {
    Proyecto proyecto(nombreProyecto, 3000000, dbConn);
    proyecto.read(nombreProyecto);

    if (cantidad > getInversionMax()) {
        return "La cantidad excede el límite de inversión permitido.";
    }

    try {
        proyecto.agregarInversion(this, cantidad);
    }
    catch (const std::runtime_error& e) {
        return e.what();
    }

    return "Inversión realizada con éxito.";
}

void Inversionista::consultarEstadoInversiones(const std::string& nombreInversionista) {
    int idInversionista = getId(nombreInversionista);

    std::string query = "SELECT * FROM inversiones WHERE nombreInversionistaID = " + std::to_string(idInversionista);
    sql::Statement* stmt = dbConn->getConnection()->createStatement();
    sql::ResultSet* res = stmt->executeQuery(query);

    while (res->next()) {
        std::cout << "Inversión en el proyecto " << res->getString("nombreProyectoID")
            << " de cantidad " << res->getDouble("cantidad") << std::endl;
    }

    double inversionTotal = getInversionTotal();
    std::cout << "La inversión total realizada por el inversionista " << nombreInversionista
        << " es de " << inversionTotal << std::endl;
}

double Inversionista::getInversionTotal() {
    std::string query = "SELECT SUM(cantidad) as total FROM inversiones WHERE nombreInversionistaID = " + std::to_string(getId(getNombre()));

    sql::Statement* stmt = dbConn->getConnection()->createStatement();
    sql::ResultSet* res = stmt->executeQuery(query);

    if (res->next()) {
        return res->getDouble("total");
    }
    else {
        return 0.0;
    }
}
