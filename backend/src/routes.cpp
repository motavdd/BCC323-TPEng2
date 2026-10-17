#include "routes.hpp"

namespace {
crow::response error_response(int status, const char* code, const char* message)
{
    crow::json::wvalue body;
    body["error"]["code"] = code;
    body["error"]["message"] = message;
    crow::response response(status, body);
    response.set_header("Cache-Control", "no-store");
    return response;
}
} // namespace

namespace bcc323 {
void register_routes(crow::SimpleApp& app)
{
    CROW_ROUTE(app, "/api/health")([] {
        crow::json::wvalue body;
        body["status"] = "ok";
        body["service"] = "bcc323-api";
        crow::response response(body);
        response.set_header("Cache-Control", "no-store");
        return response;
    });

    CROW_CATCHALL_ROUTE(app)([](const crow::request&, crow::response& response) {
        if (response.code == 405) {
            response = error_response(405, "method_not_allowed", "Method not allowed");
            response.set_header("Allow", "GET, HEAD, OPTIONS");
        } else {
            response = error_response(404, "not_found", "Route not found");
        }
        response.end();
    });

    app.exception_handler([](crow::response& response) {
        CROW_LOG_ERROR << "Unhandled exception while processing request";
        response = error_response(500, "internal_error", "Internal server error");
    });
}
} // namespace bcc323
