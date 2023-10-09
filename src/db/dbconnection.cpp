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

sql::ResultSet* DBConnection::ejecutarQueryR(const std::string& consulta) {
    try {
        sql::Statement* stmt = con->createStatement();
        sql::ResultSet* res = stmt->executeQuery(consulta);
        delete stmt;
        return res;
    }
    catch (sql::SQLException& e) {
        std::cerr << "Error al ejecutar la actualización: " << e.what() << e.getErrorCode() << std::endl;
    }
}

void DBConnection::ejecutarQuery(const std::string& consulta) {
    try {
        sql::Statement* stmt = con->createStatement();
        stmt->executeUpdate(consulta);
        delete stmt;
    }
    catch (sql::SQLException& e) {
        std::cerr << "Error al ejecutar la actualización: " << e.what() << e.getErrorCode() << std::endl;
    }
}