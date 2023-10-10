#include "dbconnection.h"

DBConnection::DBConnection(const std::string& db, const std::string& server, const std::string& user, const std::string& password) {
    try {
        driver = get_driver_instance();
        con = driver->connect(server, user, password);
        con->setSchema(db);
    }
    catch (sql::SQLException& e) {
        std::cerr << "Error al conectar a la base de datos: " << e.what() << e.getErrorCode() << std::endl;
    }
}

DBConnection::~DBConnection() { delete con; }

// Getters
sql::Connection* DBConnection::getConnection() { return con; }

sql::ResultSet* DBConnection::ejecutarQueryR(sql::PreparedStatement* pstmt) {
    try {
        sql::ResultSet* res = pstmt->executeQuery();
        return res;
    }
    catch (sql::SQLException& e) {
        std::cerr << "Error al ejecutar la actualización: " << e.what() << e.getErrorCode() << std::endl;
        throw; // Lanza la excepción para que pueda ser capturada en el método que llamó a ejecutarQueryR
    }
}

sql::PreparedStatement* DBConnection::prepareStatement(const std::string& consulta) {
    try {
        sql::PreparedStatement* pstmt = con->prepareStatement(consulta);
        return pstmt;
    }
    catch (sql::SQLException& e) {
        std::cerr << "Error al preparar la consulta: " << e.what() << e.getErrorCode() << std::endl;
        throw;
    }
}