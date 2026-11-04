#include "infrastructure/auth/RequestAuthenticator.hpp"
#include "infrastructure/auth/FirebaseTokenVerifier.hpp"
#include "infrastructure/auth/HttpFirebaseCertificateProvider.hpp"
#include <jwt-cpp/jwt.h>
#include <cstdlib>

namespace bcc323 {
    RequestContext authenticate(const crow::request& request) {

        const auto header = request.get_header_value("Authorization");

        if (header.rfind("Bearer ", 0) != 0 || header.size() > 16384){
            throw InvalidToken{};
        } 
        RequestContext context; context.token = header.substr(7);
        const char* project = std::getenv("FIREBASE_PROJECT_ID");
        if (!project || !*project){
            throw AppError(503, "api_not_configured", "Configure FIREBASE_PROJECT_ID no backend.");
        } 
        const char* emulator = std::getenv("FIREBASE_AUTH_EMULATOR_HOST");
        if (emulator && *emulator && std::string(project).rfind("demo-", 0) == 0) {
            try {
                const auto decoded = jwt::decode(context.token);
                if (!decoded.has_subject() || !decoded.has_expires_at() || !decoded.has_issued_at() || decoded.get_subject().empty() || decoded.get_subject().size() > 128){
                    throw InvalidToken{}; jwt::verify().allow_algorithm(jwt::algorithm::none{}).with_issuer(std::string("https://securetoken.google.com/") + project).with_audience(project).verify(decoded);
                } 

                context.uid = decoded.get_subject();
                return context;
            } catch (const std::exception&) { throw InvalidToken{}; }
        }
        HttpFirebaseCertificateProvider certificates;
        FirebaseTokenVerifier verifier(project, certificates);
        context.uid = verifier.verifyToken(context.token).uid;
        return context;
    }
}
