#include "Proyecto.h"
#include "inversionista.h"

Proyecto::Proyecto(const std::string& nombre, double cantidadARecaudar, DBConnection* dbConn)
    : nombre(nombre), cantidadARecaudar(cantidadARecaudar), dbConn(dbConn), cantidadRecaudada(0.0), estado(true) {}

Proyecto::Proyecto(std::string& nombreProyecto, DBConnection* dbConn) {
    this->dbConn = dbConn;
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
void Proyecto::setCantidadARecaudar(double cantidadARecaudar) { read(getNombre());  this->cantidadARecaudar = cantidadARecaudar; }
void Proyecto::setCantidadRecaudada(double cantidadRecaudada) { this->cantidadRecaudada = cantidadRecaudada; }
void Proyecto::setEstado(bool estado) { this->estado = estado; }

// Database operations
void Proyecto::create() {
    sql::PreparedStatement* pstmt = dbConn->getConnection()->prepareStatement("INSERT INTO proyectos(nombre, cantidadARecaudar, cantidadRecaudada, estado) VALUES (?, ?, ?, ?)");
    pstmt->setString(1, getNombre());
    pstmt->setDouble(2, getCantidadARecaudar());
    pstmt->setDouble(3, getCantidadRecaudada());
    pstmt->setBoolean(4, getEstado());
    dbConn->ejecutarQueryR(pstmt);
}

void Proyecto::read(std::string nombreProyecto) {
    try {
        sql::PreparedStatement* pstmt = dbConn->prepareStatement("SELECT * FROM proyectos WHERE nombre = ?");
        pstmt->setString(1, nombreProyecto);
        sql::ResultSet* res = dbConn->ejecutarQueryR(pstmt);

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
    sql::PreparedStatement* pstmt = dbConn->getConnection()->prepareStatement("UPDATE proyectos SET cantidadARecaudar = ?, cantidadRecaudada = ?, estado = ? WHERE nombre = ?");
    pstmt->setDouble(1, getCantidadARecaudar());
    pstmt->setDouble(2, getCantidadRecaudada());
    pstmt->setBoolean(3, getEstado());
    pstmt->setString(4, nombreProyecto);
    dbConn->ejecutarQueryR(pstmt);
}

void Proyecto::del(std::string nombreProyecto) {
    sql::PreparedStatement* pstmt = dbConn->getConnection()->prepareStatement("SELECT * FROM proyectos WHERE nombre = ?");
    pstmt->setString(1, nombreProyecto);
    sql::ResultSet* res = pstmt->executeQuery();

    if (!res->next()) {
        throw std::runtime_error("El proyecto no existe.");
    }

    pstmt = dbConn->getConnection()->prepareStatement("DELETE FROM proyectos WHERE nombre = ?");
    pstmt->setString(1, nombreProyecto);
    dbConn->ejecutarQueryR(pstmt);
}

int Proyecto::getId(const std::string& nombreProyecto) {
    sql::PreparedStatement* pstmt = dbConn->getConnection()->prepareStatement("SELECT id FROM proyectos WHERE nombre = ?");
    pstmt->setString(1, nombreProyecto);
    sql::ResultSet* res = pstmt->executeQuery();

    if (res->next()) {
        return res->getInt("id");
    }
    else {
        throw std::runtime_error("Proyecto no encontrado");
    }
}

bool Proyecto::existe(const std::string& nombreProyecto) {
    try {
        sql::PreparedStatement* pstmt = dbConn->getConnection()->prepareStatement("SELECT COUNT(*) AS count FROM proyectos WHERE nombre = ?");
        pstmt->setString(1, nombreProyecto);
        sql::ResultSet* res = pstmt->executeQuery();

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



// Investment operations
void Proyecto::realizarInversion(std::string nombreInversionista, std::string nombreProyecto, std::string cantidad) {
    try {
        Inversionista inversionista(nombreInversionista, dbConn);
        int idInversionista = inversionista.getId(nombreInversionista);
        int idProyecto = getId(nombreProyecto);

        std::istringstream iss(cantidad);
        double dcantidad;
        iss >> dcantidad; // Convertir string a double

        // Verificar si la inversión supera la cantidad máxima que puede hacer un inversionista
        if (dcantidad > inversionista.getInversionMax()) {
            std::cerr << "La inversión supera la cantidad máxima que puede hacer el inversionista.\n";
            return;
        }

        // Verificar si la inversión supera la cantidad a recaudar del proyecto
        if (dcantidad > getCantidadARecaudar()) {
            std::cerr << "La inversión supera la cantidad a recaudar del proyecto.\n";
            return;
        }

        sql::PreparedStatement* pstmt = dbConn->prepareStatement("INSERT INTO inversiones (nombreInversionistaID, nombreProyectoID, cantidad) VALUES (?, ?, ?)");
        pstmt->setInt(1, idInversionista);
        pstmt->setInt(2, idProyecto);
        pstmt->setDouble(3, dcantidad);

        dbConn->ejecutarQueryR(pstmt);

        setCantidadRecaudada(getCantidadRecaudada() + dcantidad);
    }
    catch (const std::exception& e) {
        std::cerr << "Ocurrió un error: " << e.what() << '\n';
        throw;
    }
}


void Proyecto::eliminarInversion(const std::string& nombreInversionista, const std::string& nombreProyecto) {
    Inversionista inversionista(dbConn);
    int idInversionista = inversionista.getId(nombreInversionista);
    int idProyecto = getId(nombreProyecto);

    sql::PreparedStatement* pstmt = dbConn->getConnection()->prepareStatement("SELECT cantidad FROM inversiones WHERE nombreInversionistaID = ? AND nombreProyectoID = ?");
    pstmt->setInt(1, idInversionista);
    pstmt->setInt(2, idProyecto);
    sql::ResultSet* res = pstmt->executeQuery();

    double cantidad = 0.0;
    if (res->next()) {
        cantidad = res->getDouble("cantidad");
    }

    pstmt = dbConn->getConnection()->prepareStatement("DELETE FROM inversiones WHERE nombreInversionistaID = ? AND nombreProyectoID = ?");
    pstmt->setInt(1, idInversionista);
    pstmt->setInt(2, idProyecto);
    dbConn->ejecutarQueryR(pstmt);

    setCantidadRecaudada(getCantidadRecaudada() - cantidad);
}



std::vector<std::tuple<int, std::string, double, std::string>> Proyecto::listarInversionesRealizadas(const std::string& nombreProyecto) {
    int idProyecto = getId(nombreProyecto);

    sql::PreparedStatement* pstmt = dbConn->getConnection()->prepareStatement("SELECT inversiones.id, inversionistas.nombre, inversiones.cantidad, inversiones.fecha FROM inversiones INNER JOIN inversionistas ON inversiones.nombreInversionistaID = inversionistas.id WHERE nombreProyectoID = ?");
    pstmt->setInt(1, idProyecto);
    sql::ResultSet* res = pstmt->executeQuery();

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
