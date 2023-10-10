#include "Proyecto.h"
#include "inversionista.h"

Proyecto::Proyecto() : nombre(""), cantidadARecaudar(0.0), cantidadRecaudada(0.0), estado(true), inversionista() {}

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

// ------------------------------------------ DATABASE OPERATIONS ------------------------------------------  
void Proyecto::create() {
    // Preparar y ejecutar la consulta SQL
    std::string consulta = "INSERT INTO proyectos(nombre, cantidadARecaudar, cantidadRecaudada, estado) VALUES ('"
        + getNombre() + "', "
        + std::to_string(getCantidadARecaudar()) + ", "
        + std::to_string(getCantidadRecaudada()) + ", "
        + std::to_string(getEstado()) + ")";
    dbConn->executeQuery(consulta);
}

void Proyecto::read(std::string nombreProyecto) {
    try {
        // Preparar y ejecutar la consulta SQL
        std::string consulta = "SELECT * FROM proyectos WHERE nombre = '"
            + nombreProyecto + "'";
        sql::ResultSet* res = dbConn->executeQuery(consulta);

        if (res->next()) {
            setNombre(res->getString("nombre"));
            setCantidadARecaudar(res->getDouble("cantidadARecaudar"));
            setCantidadRecaudada(res->getDouble("cantidadRecaudada"));
            
            if (getCantidadRecaudada() >= getCantidadARecaudar()) {
                sql::ResultSet* res = dbConn->executeQuery("UPDATE proyectos SET estado = 0 WHERE nombre = '"
                    + getNombre() + "'");
            } 
            else {
                setEstado(res->getBoolean("estado"));
            }
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

void Proyecto::update(std::string nombreAntiguo) {
    // Preparar y ejecutar la consulta SQL
    std::string consulta = "UPDATE proyectos SET nombre = " + getNombre() 
        + ", cantidadARecaudar = " + std::to_string(getCantidadARecaudar()) 
        + ", cantidadRecaudada = " + std::to_string(getCantidadRecaudada()) 
        + ", estado = " + std::to_string(getEstado()) 
        + " WHERE nombre = '" + nombreAntiguo + "'";
    dbConn->executeQuery(consulta);
}

void Proyecto::del(std::string nombreProyecto) {
    // Preparar y ejecutar la consulta SQL
    std::string consulta = "SELECT * FROM proyectos WHERE nombre = '"
        + nombreProyecto + "'";
    sql::ResultSet* res = dbConn->executeQuery(consulta);

    if (!res->next()) {
        throw std::runtime_error("El proyecto no existe.");
    }

    // Preparar y ejecutar la consulta SQL
    consulta = "DELETE FROM proyectos WHERE nombre = '"
        + nombreProyecto + "'";
    dbConn->executeQuery(consulta);
}

int Proyecto::getId(const std::string& nombreProyecto) {
    // Preparar y ejecutar la consulta SQL
    std::string consulta = "SELECT id FROM proyectos WHERE nombre = '"
        + nombreProyecto + "'";
    sql::ResultSet* res = dbConn->executeQuery(consulta);

    if (res->next()) {
        return res->getInt("id");
    }
    else {
        throw std::runtime_error("Proyecto no encontrado");
    }
}

bool Proyecto::existe(const std::string& nombreProyecto) {
    try {
        // Preparar y ejecutar la consulta SQL
        std::string consulta = "SELECT COUNT(*) AS count FROM proyectos WHERE nombre = '"
            + nombreProyecto + "'";
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

bool Proyecto::tieneInversiones(const std::string& nombreInversionista, const std::string& nombreProyecto) {
    try {
        int idInversionista = inversionista.getId(nombreInversionista);
        int idProyecto = getId(nombreProyecto);

        // Preparar y ejecutar la consulta SQL
        std::string consulta = "SELECT COUNT(*) AS count FROM inversiones WHERE nombreInversionistaID = "
            + std::to_string(idInversionista) + " AND nombreProyectoID = "
            + std::to_string(idProyecto);
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

void Proyecto::realizarInversion(std::string nombreInversionista, std::string nombreProyecto, std::string cantidad) {
    try {
        inversionista.setNombre(nombreInversionista);
        int idInversionista = inversionista.getId(nombreInversionista);
        int idProyecto = getId(nombreProyecto);

        std::istringstream iss(cantidad);
        double dcantidad;
        iss >> dcantidad; // Convertir string a double

        // Preparar y ejecutar la consulta SQL
        std::string consulta = "INSERT INTO inversiones (nombreInversionistaID, nombreProyectoID, cantidad) VALUES ("
            + std::to_string(idInversionista) + ", "
            + std::to_string(idProyecto) + ", "
            + std::to_string(dcantidad) + ")";

        sql::ResultSet* res = dbConn->executeQuery(consulta);

        // Actualizar la cantidad recaudada del proyecto
        setCantidadRecaudada(getCantidadRecaudada() + dcantidad);
        dbConn->executeQuery("UPDATE proyectos SET cantidadRecaudada = "
            + std::to_string(getCantidadRecaudada()) + " WHERE id = " 
            + std::to_string(getId(nombreProyecto)));
    }
    catch (const std::exception& e) {
        std::cerr << "Ocurrió un error: " << e.what() << '\n';
        throw;
    }
}

void Proyecto::eliminarInversiones(const std::string& nombreInversionista, const std::string& nombreProyecto) {
    int idInversionista = inversionista.getId(nombreInversionista);
    int idProyecto = getId(nombreProyecto);

    // Preparar y ejecutar la consulta SQL
    std::string consulta = "SELECT cantidad FROM inversiones WHERE nombreInversionistaID = "
        + std::to_string(idInversionista) + " AND nombreProyectoID = "
        + std::to_string(idProyecto);
    sql::ResultSet* res = dbConn->executeQuery(consulta);

    double cantidad = 0.0;
    if (res->next()) {
        cantidad = res->getDouble("cantidad");
    }
    else {
        throw std::runtime_error("No hay inversiones disponibles para eliminar.");
    }

    // Preparar y ejecutar la consulta SQL
    consulta = "DELETE FROM inversiones WHERE nombreInversionistaID = "
        + std::to_string(idInversionista) + " AND nombreProyectoID = "
        + std::to_string(idProyecto);

    dbConn->executeQuery(consulta);

    setCantidadRecaudada(getCantidadRecaudada() - cantidad);

    // Eliminar las inversiones del usuario de la tabla 'inversiones'
    consulta = "DELETE FROM inversionistas WHERE id = "
        + std::to_string(idInversionista);

    dbConn->executeQuery(consulta);
}



sql::ResultSet* Proyecto::consultarProyecto(std::string nombreProyecto) {
    // Preparar y ejecutar la consulta SQL
    std::string consulta = "SELECT nombre, cantidadARecaudar, cantidadRecaudada, estado FROM proyectos WHERE nombre = '"
        + nombreProyecto + "'";
    sql::ResultSet* res = dbConn->executeQuery(consulta);

    return res;
}

std::vector<std::tuple<int, std::string, double, std::string>> Proyecto::listarInversionesRealizadas(const std::string& nombreProyecto) {
    int idProyecto = getId(nombreProyecto);

    // Preparar y ejecutar la consulta SQL
    std::string consulta = "SELECT inversiones.id, inversionistas.nombre, inversiones.cantidad, inversiones.fecha FROM inversiones INNER JOIN inversionistas ON inversiones.nombreInversionistaID = inversionistas.id WHERE nombreProyectoID = "
        + std::to_string(idProyecto);
    sql::ResultSet* res = dbConn->executeQuery(consulta);

    std::vector<std::tuple<int, std::string, double, std::string>> inversiones;
    while (res->next()) {
        int id = res->getInt("id");
        std::string nombre = res->getString("nombre");
        double cantidad = res->getDouble("cantidad");
        std::string fecha = res->getString("fecha");

        inversiones.push_back(std::make_tuple(id, nombre, cantidad, fecha));
    }

    if (inversiones.empty()) {
        throw std::runtime_error("No se encontraron inversiones para este proyecto.");
    }

    return inversiones;
}

