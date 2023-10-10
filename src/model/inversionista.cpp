#include "inversionista.h"
#include "proyecto.h"

Inversionista::Inversionista(const std::string& nombre, const std::string& tipo, DBConnection* dbConn)
    : nombre(nombre), tipo(tipo), dbConn(dbConn) {
    inversionConfig(tipo);
}

Inversionista::Inversionista(std::string& nombreInversionista, DBConnection* dbConn) : dbConn(dbConn) {
    read(nombreInversionista);
    inversionConfig(getTipo());
}

Inversionista::Inversionista(DBConnection* dbConn) : dbConn(dbConn){}
Inversionista::Inversionista(std::string nombreinversionista) {
    read(nombreinversionista);
    inversionConfig(getTipo());
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
    sql::PreparedStatement* pstmt = dbConn->getConnection()->prepareStatement("INSERT INTO inversionistas(nombre, tipo, ingreso_mensual) VALUES (?, ?, ?)");
    pstmt->setString(1, getNombre());
    pstmt->setString(2, getTipo());
    pstmt->setDouble(3, getIngresoMensual());
    dbConn->ejecutarQueryR(pstmt);
}

void Inversionista::read(std::string qNombre) {
    sql::PreparedStatement* pstmt = dbConn->getConnection()->prepareStatement("SELECT * FROM inversionistas WHERE nombre = ?");
    pstmt->setString(1, qNombre);
    sql::ResultSet* res = pstmt->executeQuery();

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
    sql::PreparedStatement* pstmt = dbConn->getConnection()->prepareStatement("UPDATE inversionistas SET tipo = ?, ingreso_mensual = ? WHERE nombre = ?");
    pstmt->setString(1, getTipo());
    pstmt->setDouble(2, getIngresoMensual());
    pstmt->setString(3, qNombre);
    dbConn->ejecutarQueryR(pstmt);
}

void Inversionista::del(std::string qNombre) {
    sql::PreparedStatement* pstmt = dbConn->prepareStatement("DELETE FROM inversionistas WHERE nombre = ?");
    pstmt->setString(1, qNombre);
    dbConn->ejecutarQueryR(pstmt);
}

int Inversionista::getId(const std::string& qNombre) {
    sql::PreparedStatement* pstmt = dbConn->prepareStatement("SELECT id FROM inversionistas WHERE nombre = ?");
    pstmt->setString(1, qNombre);
    sql::ResultSet* res = dbConn->ejecutarQueryR(pstmt);

    if (res->next()) {
        return res->getInt("id");
    }
    else {
        throw std::runtime_error("Inversionista no encontrado");
    }
}

// Investment Operations
std::vector<std::pair<std::string, double>> Inversionista::consultarEstadoInversiones(const std::string& nombreInversionista) {
    int idInversionista = getId(nombreInversionista);

    sql::PreparedStatement* pstmt = dbConn->getConnection()->prepareStatement("SELECT proyectos.nombre, inversiones.cantidad FROM inversiones INNER JOIN inversionistas ON inversiones.nombreInversionistaID = inversionistas.id INNER JOIN proyectos ON inversiones.nombreProyectoID = proyectos.id WHERE nombreInversionistaID = ?");
    pstmt->setInt(1, idInversionista);
    sql::ResultSet* res = pstmt->executeQuery();

    std::vector<std::pair<std::string, double>> inversiones;
    while (res->next()) {
        inversiones.push_back(std::make_pair(res->getString("nombre"), res->getDouble("cantidad")));
    }

    return inversiones;
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

sql::ResultSet* Inversionista::verUsuarios() {
    std::string query = "SELECT * FROM inversionistas";
    sql::ResultSet* res = dbConn->ejecutarQueryR(query);
    return res;
}