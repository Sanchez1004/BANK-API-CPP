#pragma once
#ifndef DBCONNECTION_H
#define DBCONNECTION_H

#include <string>
#include <jdbc/mysql_connection.h>
#include <jdbc/mysql_driver.h>
#include <jdbc/cppconn/statement.h>

class DBConnection {
private:
    sql::Driver* driver;
    sql::Connection* con;

public:
    DBConnection(const std::string& db, const std::string& server, const std::string& user, const std::string& password);
    ~DBConnection();

    sql::Connection* getConnection();

    void ejecutarActualizacion(const std::string& consulta);
};
#endif // DBCONNECTION_H