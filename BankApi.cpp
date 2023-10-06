#include <iostream>

#include "db/dbconnection.h"

int main()
{
	DBConnection db("bankdb", "localhost", "root", "1234");
	

	std::cout << "Hello world!";
}