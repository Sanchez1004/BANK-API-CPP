// This class is used to establish a connection with a database and execute queries
#include "dbconnection.h"

// The constructor takes four parameters : the database name, server, username, and password.It uses these to establish a connection to the database.
// used to establish a connection with a database
DBConnection::DBConnection(const std::string& db, const std::string& server, const std::string& user, const std::string& password) {
    try {
        driver = get_driver_instance(); // This line gets an instance of the SQL driver.

        con = driver->connect(server, user, password); // This line uses the driver to establish a connection to the server using the provided username and password.

        con->setSchema(db); // This line sets the database schema to the provided database name.
    }
    catch (sql::SQLException& e) {
        std::cerr << "Error al conectar a la base de datos: " << e.what() << e.getErrorCode() << std::endl;
    }
}

// The destructor deletes the connection object if it exists.
DBConnection::~DBConnection() {
    if (con != nullptr) {
        delete con;
        con = nullptr;
    }
}

// --------------------------------  Getters -------------------------------
sql::Connection* DBConnection::getConnection() { return con; } // The getConnection function returns the current connection object.


// This method takes a SQL query as a parameter, prepares and executes it, and returns the result set.
//  If an SQL exception occurs during this process, it prints an error message and rethrows the exception.
sql::ResultSet* DBConnection::executeQuery(const std::string& consulta) {

    sql::PreparedStatement* pstmt = nullptr; // This line declares a pointer to a PreparedStatement object and initializes it to nullptr
    sql::ResultSet* res = nullptr; // This line declares a pointer to a ResultSet object and initializes it to nullptr

    try {
        pstmt = con->prepareStatement(consulta); // This line prepares the SQL query.
        res = pstmt->executeQuery(); // This line executes the SQL query and stores the result set in res.
    }

    // If an SQL exception occurs during this process, it is caught and an error message is printed to the console. 
    // The PreparedStatement object is deleted if it exists, and the exception is rethrown.
    catch (sql::SQLException& e) {
        std::cerr << "Error al preparar o ejecutar la consulta: " << e.what() << " " << e.getErrorCode() << std::endl;
        if (pstmt != nullptr) {
            delete pstmt;
        }
        throw;
    }

    // After the try block, if the PreparedStatement object exists, it is deleted.
    if (pstmt != nullptr) {
        delete pstmt;
    }

    return res;
}