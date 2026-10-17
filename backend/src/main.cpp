#include <crow.h>

#include <charconv>
#include <cstdlib>
#include <iostream>
#include <string_view>

int main()
{
    unsigned int port = 8081;
    if (const char* configured_port = std::getenv("PORT")) {
        const std::string_view value(configured_port);
        const auto result = std::from_chars(value.data(), value.data() + value.size(), port);
        if (result.ec != std::errc{} || result.ptr != value.data() + value.size()
            || port == 0 || port > 65535) {
            std::cerr << "PORT must be an integer between 1 and 65535\n";
            return EXIT_FAILURE;
        }
    }

    crow::SimpleApp app;
    CROW_ROUTE(app, "/api/health")([] {
        crow::json::wvalue body;
        body["status"] = "ok";
        body["service"] = "bcc323-api";
        crow::response response(body);
        response.set_header("Cache-Control", "no-store");
        return response;
    });

    app.bindaddr("0.0.0.0").port(static_cast<std::uint16_t>(port)).concurrency(2).run();
}
