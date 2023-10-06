#include <iostream>

#include <string>
#include "db/dbconnection.h"
#include "model/inversionista.h"
#include "model/proyecto.h"

int main()
{
	DBConnection* db = new DBConnection("bankdb", "localhost", "root", "1234");

	std::cout << "Hello world!";
}