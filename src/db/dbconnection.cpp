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

DBConnection::~DBConnection() {
    if (con != nullptr) {
        delete con;
        con = nullptr;
    }
}

// Getters
sql::Connection* DBConnection::getConnection() { return con; }

sql::ResultSet* DBConnection::executeQuery(const std::string& consulta) {
    sql::PreparedStatement* pstmt = nullptr;
    sql::ResultSet* res = nullptr;

    try {
        pstmt = con->prepareStatement(consulta);
        res = pstmt->executeQuery();
    }
    catch (sql::SQLException& e) {
        std::cerr << "Error al preparar o ejecutar la consulta: " << e.what() << " " << e.getErrorCode() << std::endl;
        if (pstmt != nullptr) {
            delete pstmt;
        }
        throw;
    }

    if (pstmt != nullptr) {
        delete pstmt;
    }

    return res;
}