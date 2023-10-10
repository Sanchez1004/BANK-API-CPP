#include <iostream>
#include <cpprest/http_listener.h>
#include "controller/requestHandler.h"

int main() {
    RequestHandler requestHandler;

    http_listener listener(U("http://localhost:8080"));

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
            .open()
            .then([&listener]() { std::cout << "Starting to listen on " << utility::conversions::to_utf8string(listener.uri().to_string()) << std::endl;})
            .wait();

        while (true);
    }
    catch (std::exception const& e) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
