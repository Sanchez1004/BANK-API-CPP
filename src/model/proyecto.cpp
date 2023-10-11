#include "Proyecto.h"
#include "inversionista.h"

Proyecto::Proyecto() : nombre(""), cantidadARecaudar(0.0), cantidadRecaudada(0.0), estado(true), inversionista() {
    dbConn = new DBConnection("bankdb", "localhost", "root", "1234");
} // Constructor, 

// ------------------------------------------------ Getters -----------------------------------------------

std::string Proyecto::getNombre() const { return nombre; }
double Proyecto::getCantidadARecaudar() const { return cantidadARecaudar; }
double Proyecto::getCantidadRecaudada() const { return cantidadRecaudada; }
bool Proyecto::getEstado() const { return estado; }

// ------------------------------------------------ Setters -----------------------------------------------

void Proyecto::setNombre(const std::string& nombre) { this->nombre = nombre; }
void Proyecto::setCantidadARecaudar(double cantidadARecaudar) { this->cantidadARecaudar = cantidadARecaudar; }
void Proyecto::setCantidadRecaudada(double cantidadRecaudada) { this->cantidadRecaudada = cantidadRecaudada; }
void Proyecto::setEstado(bool estado) { this->estado = estado; }

// ------------------------------------------ DATABASE OPERATIONS ------------------------------------------  

// Method used to insert a new project into the ‘proyectos’ table
void Proyecto::create() {
    // Prepare SQL query to insert a new project into the 'proyectos' table
    std::string consulta = "INSERT INTO proyectos(nombre, cantidadARecaudar, cantidadRecaudada, estado) VALUES ('"
        + getNombre() + "', "
        + std::to_string(getCantidadARecaudar()) + ", "
        + std::to_string(getCantidadRecaudada()) + ", "
        + std::to_string(getEstado()) + ")";
    dbConn->executeQuery(consulta);
}

// Function to read project details from the database
void Proyecto::read(std::string nombreProyecto) {
    try {
        // Prepare SQL query to select all fields from 'proyectos' table where the project name matches the input
        std::string consulta = "SELECT * FROM proyectos WHERE nombre = '" + nombreProyecto + "'";
        sql::ResultSet* res = dbConn->executeQuery(consulta);

        if (res->next()) { // If a matching record is found
            // Set the project's name, amount to be raised, and amount raised from the result set
            setNombre(res->getString("nombre"));
            setCantidadARecaudar(res->getDouble("cantidadARecaudar"));
            setCantidadRecaudada(res->getDouble("cantidadRecaudada"));

            // If the amount raised is greater than or equal to the amount to be raised
            if (getCantidadRecaudada() >= getCantidadARecaudar()) {
                // Update the project's status in the database
                sql::ResultSet* res = dbConn->executeQuery("UPDATE proyectos SET estado = 0 WHERE nombre = '"
                    + getNombre() + "'");
            }
            else {
                // Set the project's status from the result set
                setEstado(res->getBoolean("estado"));
            }
        }
        else { // If no matching record is found
            throw std::runtime_error("Proyecto no encontrado");
        }
    }
    catch (const std::runtime_error& e) {
        std::cerr << "Ocurrió un error: " << e.what() << '\n';
        throw;
    }
}

// Method to update a project's details in the database
void Proyecto::update(std::string nombreAntiguo) {
    // Prepare SQL query to update the 'nombre', 'cantidadARecaudar', 'cantidadRecaudada', and 'estado' fields in the 'proyectos' table
    // where the project's name matches the old name
    std::string consulta = "UPDATE proyectos SET nombre = " + getNombre()
        + ", cantidadARecaudar = " + std::to_string(getCantidadARecaudar())
        + ", cantidadRecaudada = " + std::to_string(getCantidadRecaudada())
        + ", estado = " + std::to_string(getEstado())
        + " WHERE nombre = '" + nombreAntiguo + "'";
    dbConn->executeQuery(consulta);
}

// Method to delete a project from the database
void Proyecto::del(std::string nombreProyecto) {
    // Prepare SQL query to select all fields from 'proyectos' table where the project name matches the input
    std::string consulta = "SELECT * FROM proyectos WHERE nombre = '"
        + nombreProyecto + "'";
    sql::ResultSet* res = dbConn->executeQuery(consulta);

    if (!res->next()) {
        throw std::runtime_error("El proyecto no existe.");
    }

    // Prepare SQL query to delete a project from the 'proyectos' table where the name matches the input
    consulta = "DELETE FROM proyectos WHERE nombre = '"
        + nombreProyecto + "'";
    dbConn->executeQuery(consulta);
}

// Method to get a project's ID from the database
int Proyecto::getId(const std::string& nombreProyecto) {
    // Prepare SQL query to select the ID from the 'proyectos' table where the name matches the input
    std::string consulta = "SELECT id FROM proyectos WHERE nombre = '"
        + nombreProyecto + "'";
    sql::ResultSet* res = dbConn->executeQuery(consulta);

    // If a result is found, return the ID; otherwise, throw an error
    if (res->next()) {
        return res->getInt("id");
    }
    else {
        throw std::runtime_error("Proyecto no encontrado");
    }
}


// Method to check if a project exists in the database
bool Proyecto::existe(const std::string& nombreProyecto) {
    try {
        // Prepare SQL query to count the number of records in the 'proyectos' table where the project name matches the input
        std::string consulta = "SELECT COUNT(*) AS count FROM proyectos WHERE nombre = '"
            + nombreProyecto + "'";
        sql::ResultSet* res = dbConn->executeQuery(consulta);

        bool exists = false;
        if (res->next()) {
            // If a result is found and the count is greater than 0, set 'exists' to true
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

// Method to check if an investor has investments in a project
bool Proyecto::tieneInversiones(const std::string& nombreInversionista, const std::string& nombreProyecto) {
    try {
        // Get the IDs of the investor and the project
        int idInversionista = inversionista.getId(nombreInversionista);
        int idProyecto = getId(nombreProyecto);

        // Prepare SQL query to count the number of records in the 'inversiones' table where the investor ID and project ID match the input
        std::string consulta = "SELECT COUNT(*) AS count FROM inversiones WHERE nombreInversionistaID = "
            + std::to_string(idInversionista) + " AND nombreProyectoID = "
            + std::to_string(idProyecto);
        sql::ResultSet* res = dbConn->executeQuery(consulta);

        bool exists = false;
        if (res->next()) {
            // If a result is found and the count is greater than 0, set 'exists' to true
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



// Method used to make inversions into the table INVERSIONS
void Proyecto::realizarInversion(std::string nombreInversionista, std::string nombreProyecto, std::string cantidad) {
    try {
        inversionista.setNombre(nombreInversionista);
        int idInversionista = inversionista.getId(nombreInversionista);
        int idProyecto = getId(nombreProyecto);

        std::istringstream iss(cantidad);
        double dcantidad;
        iss >> dcantidad; // Convert string to double

        // SQL query is prepared to insert the investor ID, project ID, and investment amount into the 'inversiones' table
        std::string consulta = "INSERT INTO inversiones (nombreInversionistaID, nombreProyectoID, cantidad) VALUES ("
            + std::to_string(idInversionista) + ", "
            + std::to_string(idProyecto) + ", "
            + std::to_string(dcantidad) + ")";

        dbConn->executeQuery(consulta);

        // Another SQL query is prepared and executed to update the proyectos table with the new total amount raised for the project
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


    // Method used to delete investments made by an investor from the ‘inversiones’ table
    void Proyecto::eliminarInversiones(const std::string& nombreInversionista, const std::string& nombreProyecto) {
        int idInversionista = inversionista.getId(nombreInversionista);
        int idProyecto = getId(nombreProyecto);

        // Prepare a SQL query is prepared to select the investment amount from the ‘inversiones’ table for the given investor and project
        std::string consulta = "SELECT cantidad FROM inversiones WHERE nombreInversionistaID = "
            + std::to_string(idInversionista) + " AND nombreProyectoID = "
            + std::to_string(idProyecto);
        sql::ResultSet* res = dbConn->executeQuery(consulta);

        // The investment amount is retrieved from the result set.If no investments are found, an exception is thrown
        double cantidad = 0.0;
        if (res->next()) {
            cantidad = res->getDouble("cantidad");
        }
        else {
            throw std::runtime_error("No hay inversiones disponibles para eliminar.");
        }
        dbConn->executeQuery(consulta);
        //The total amount raised for the project is updated by subtracting the investment amount
        setCantidadRecaudada(getCantidadRecaudada() - cantidad);

        // Delete the inverisions made in the table 'INVERSIONS'
        consulta = "DELETE FROM inversiones WHERE nombreInversionistaID = "
            + std::to_string(idInversionista);

        dbConn->executeQuery(consulta);
    }

// Method used to retrieve information about a project from the ‘proyectos’ table
sql::ResultSet* Proyecto::consultarProyecto(std::string nombreProyecto) {
    // prepares an SQL query to select the name, amount to be raised, amount raised, and status of the project with the given name
    std::string consulta = "SELECT nombre, cantidadARecaudar, cantidadRecaudada, estado FROM proyectos WHERE nombre = '"
        + nombreProyecto + "'";
    sql::ResultSet* res = dbConn->executeQuery(consulta);

    return res;
}

// Method used to list all the investments made in a specific project
std::vector<std::tuple<int, std::string, double, std::string>> Proyecto::listarInversionesRealizadas(const std::string& nombreProyecto) {
    int idProyecto = getId(nombreProyecto);

    // A SQL query is prepared to select the ID, name, amount, and date of investments from the ‘inversiones’ table for the given project
    std::string consulta = "SELECT inversiones.id, inversionistas.nombre, inversiones.cantidad, inversiones.fecha FROM inversiones INNER JOIN inversionistas ON inversiones.nombreInversionistaID = inversionistas.id WHERE nombreProyectoID = "
        + std::to_string(idProyecto);
    sql::ResultSet* res = dbConn->executeQuery(consulta);
    
    //  Then creates a vector of tuples to store the investment details
    std::vector<std::tuple<int, std::string, double, std::string>> inversiones;
    while (res->next()) { // It iterates over each result and adds a tuple of investment ID, investor name, investment amount, and investment date to the vector
        int id = res->getInt("id"); 
        std::string nombre = res->getString("nombre");
        double cantidad = res->getDouble("cantidad");
        std::string fecha = res->getString("fecha");

        inversiones.push_back(std::make_tuple(id, nombre, cantidad, fecha));
    }
    
    // If no investments are found, an exception is thrown
    if (inversiones.empty()) {
        throw std::runtime_error("No se encontraron inversiones para este proyecto.");
    }

    return inversiones;
}

