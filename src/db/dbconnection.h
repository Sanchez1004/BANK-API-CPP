#pragma once
#ifndef DBCONNECTION_H
#define DBCONNECTION_H

#include <string>
#include <jdbc/mysql_connection.h>
#include <jdbc/mysql_driver.h>
#include <jdbc/cppconn/statement.h>
#include <jdbc/cppconn/prepared_statement.h>
#include "cpprest/http_listener.h"
#include "cpprest/json.h"

using namespace web::http;
using namespace web::http::experimental::listener;

class DBConnection {
private:
    sql::Driver* driver; // Pointer to the SQL driver
    sql::Connection* con; // Pointer to the SQL connection

public:
    // Constructor: Initializes a new instance of the DBConnection class
    DBConnection(const std::string& db, const std::string& server, const std::string& user, const std::string& password);

    // Destructor: Cleans up any resources being used.
    ~DBConnection();

    // Method: Returns the SQL connection
    sql::Connection* getConnection();
    
    // Method: Executes a query and returns the result set
    sql::ResultSet* executeQuery(const std::string& consulta);
};
#endif // DBCONNECTION_H