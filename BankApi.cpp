// API that uses the libraries RESTCPPSDK, CONNECTOR C++ 8.0 & MYSQL SERVER 8.0
#include <iostream>
#include <cpprest/http_listener.h>
#include "controller/requestHandler.h"

int main() {
    RequestHandler requestHandler;

    http_listener listener(U("http://localhost:8080")); // Create an HTTP listener that listens on http://localhost:8080

    // Set up the server to handle different types of HTTP requests (POST, GET, DELETE, PUT) using corresponding methods from the RequestHandler instance
    listener.support(http::methods::POST, [&requestHandler](http_request request) {
        requestHandler.handle_post(request);
        });
    listener.support(http::methods::GET, [&requestHandler](http_request request) {
        requestHandler.handle_get(request);
        });
    listener.support(http::methods::DEL, [&requestHandler](http_request request) {
        requestHandler.handle_delete(request);
        });
    listener.support(http::methods::PUT, [&requestHandler](http_request request) {
        requestHandler.handle_put(request);
        });

    try {
        listener
            .open() // Open the listener
            .then([&listener]() { std::cout << "Starting to listen on " << utility::conversions::to_utf8string(listener.uri().to_string()) << std::endl;}) // Print a message to the console when the listener starts listening
            .wait(); // Wait for all asynchronous operations to complete

        while (true); // Keep the server running indefinitely
    }
    catch (std::exception const& e) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
