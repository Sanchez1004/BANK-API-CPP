#include "inversionista.h"
#include "proyecto.h"

Inversionista::Inversionista() : nombre(""), tipo(""), ingresoMensual(0.0), inversionMax(0.0){}

// ------------------------------------------------ Getters -----------------------------------------------

std::string Inversionista::getNombre() const { return nombre; }
std::string Inversionista::getTipo() const { return tipo; }
double Inversionista::getIngresoMensual() const { return ingresoMensual; }
double Inversionista::getInversionMax() const { return inversionMax; }

// ------------------------------------------------ Setters -----------------------------------------------

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

// ------------------------------------------ DATABASE OPERATIONS ------------------------------------------  

// Method to create a new investor in the database
void Inversionista::create() {
    // Prepare SQL query to insert a new investor into the 'inversionistas' table
    std::string consulta = "INSERT INTO inversionistas(nombre, tipo, ingreso_mensual) VALUES ('"
        + getNombre() + "', '"
        + getTipo() + "', "
        + std::to_string(getIngresoMensual()) + ")";
    dbConn->executeQuery(consulta);
}

// Method to read project details from the database
void Inversionista::read(std::string qNombre) {
    // Prepare SQL query to select all fields from 'proyectos' table where the project name matches the input
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

// Method to update a project's details in the database
void Inversionista::update(std::string qNombre) {
    // Prepare SQL query to update the 'nombre', 'cantidadARecaudar', 'cantidadRecaudada', and 'estado' fields in the 'proyectos' table
    // where the project's name matches the old name
    std::string consulta = "UPDATE inversionistas SET tipo = '"
        + getTipo() + "', ingreso_mensual = "
        + std::to_string(getIngresoMensual()) + " WHERE nombre = '"
        + qNombre + "'";
    dbConn->executeQuery(consulta);
}

// Method to delete an investor from the database
void Inversionista::del(std::string qNombre) {
    // Prepare SQL query to delete an investor from the 'inversionistas' table where the name matches the input
    std::string consulta = "DELETE FROM inversionistas WHERE nombre = '"
        + qNombre + "'";
    dbConn->executeQuery(consulta);
}

// Method to get an investor's ID from the database
int Inversionista::getId(const std::string& qNombre) {
    // Prepare SQL query to select the ID from the 'inversionistas' table where the name matches the input
    std::string consulta = "SELECT id FROM inversionistas WHERE nombre = '"
        + qNombre + "'";
    sql::ResultSet* res = dbConn->executeQuery(consulta);

    // If a result is found, return the ID; otherwise, throw an error
    if (res->next()) {
        return res->getInt("id");
    }
    else {
        throw std::runtime_error("Inversionista no encontrado");
    }
}

// Method to check if an investor exists in the database
bool Inversionista::existe(const std::string& nombreInversionista) {
    try {
        // Prepare SQL query to count the number of investors in the 'inversionistas' table where the name matches the input
        std::string consulta = "SELECT COUNT(*) AS count FROM inversionistas WHERE nombre = '"
            + nombreInversionista + "'";
        sql::ResultSet* res = dbConn->executeQuery(consulta);

        bool exists = false;
        // If a result is found and the count is greater than 0, set exists to true
        if (res->next()) {
            int count = res->getInt("count");
            exists = count > 0;
        }

        // Return whether or not the investor exists
        return exists;
    }
    catch (const sql::SQLException& e) {
        std::cerr << "Ocurrió un error: " << e.what() << '\n';
        throw;
    }
}   



// -------------------------------- Investment operations --------------------------------

// Function to get the state of investments for a given investor
std::vector<std::pair<std::string, double>> Inversionista::consultarEstadoInversiones(const std::string& nombreInversionista) {
    int idInversionista = getId(nombreInversionista);

    // Prepare SQL query to select the project name and investment amount from the 'inversiones' table for the given investor
    std::string consulta = "SELECT proyectos.nombre, inversiones.cantidad FROM inversiones INNER JOIN inversionistas ON inversiones.nombreInversionistaID = inversionistas.id INNER JOIN proyectos ON inversiones.nombreProyectoID = proyectos.id WHERE nombreInversionistaID = "
        + std::to_string(idInversionista);
    sql::ResultSet* res = dbConn->executeQuery(consulta);

    // Create a vector to store the investments
    std::vector<std::pair<std::string, double>> inversiones;

    while (res->next()) { // For each result, add a pair of project name and investment amount to the vector
        inversiones.push_back(std::make_pair(res->getString("nombre"), res->getDouble("cantidad")));
    }

    // Return the vector of investments
    return inversiones;
}

// Function to get the total investment amount for an investor
double Inversionista::getInversionTotal() {
    // Prepare SQL query to get the sum of investment amounts from the 'inversiones' table for the given investor
    std::string consulta = "SELECT SUM(cantidad) as total FROM inversiones WHERE nombreInversionistaID = "
        + std::to_string(getId(getNombre()));
    sql::ResultSet* res = dbConn->executeQuery(consulta);

    // If a result is found, return the total; otherwise, return 0.0
    if (res->next()) {
        return res->getDouble("total");
    }
    else {
        return 0.0;
    }
}

// Function to get all users from the database
sql::ResultSet* Inversionista::verUsuarios() {
    // Prepare SQL query to select all fields from the 'inversionistas' table
    std::string consulta = "SELECT * FROM inversionistas";
    sql::ResultSet* res = dbConn->executeQuery(consulta);

    return res;
}
