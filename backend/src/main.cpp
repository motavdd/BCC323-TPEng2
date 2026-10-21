#include "config/config.hpp"
#include "http/routes.hpp"

#include <cstdlib>
#include <exception>
#include <iostream>

int main()
{
    try {
        const auto config = bcc323::load_config();
        crow::SimpleApp app;
        bcc323::register_routes(app);
        app.bindaddr(config.host).port(config.port).concurrency(2).run();
    } catch (const std::exception& error) {
        std::cerr << "Backend startup failed: " << error.what() << '\n';
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
