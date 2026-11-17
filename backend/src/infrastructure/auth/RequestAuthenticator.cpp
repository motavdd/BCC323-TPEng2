#include "infrastructure/auth/RequestAuthenticator.hpp"
#include "infrastructure/auth/FirebaseTokenVerifier.hpp"
#include "infrastructure/auth/HttpFirebaseCertificateProvider.hpp"
#include <jwt-cpp/jwt.h>
#include <chrono>
#include <cstdlib>
#include <regex>

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
        const bool useEmulator = emulator && *emulator && std::string(project).rfind("demo-", 0) == 0;
        if (!useEmulator) {
            HttpFirebaseCertificateProvider certificates;
            FirebaseTokenVerifier verifier(project, certificates);
            context.uid = verifier.verifyToken(context.token).uid;
        }
        try {
            const auto decoded = jwt::decode(context.token);
            if (useEmulator) {
                if (!decoded.has_subject() || !decoded.has_expires_at() || !decoded.has_issued_at() || decoded.get_subject().empty() || decoded.get_subject().size() > 128){
                    throw InvalidToken{};
                }
                jwt::verify().allow_algorithm(jwt::algorithm::none{})
                    .with_issuer(std::string("https://securetoken.google.com/") + project)
                    .with_audience(project).verify(decoded);
                const auto now = std::chrono::system_clock::now();
                if (decoded.get_expires_at() <= now || decoded.get_issued_at() > now) {
                    throw InvalidToken{};
                }
                context.uid = decoded.get_subject();
            }
            static const std::regex institutionalEmail(R"([^\s@]+@(aluno\.)?ufop\.edu\.br)", std::regex::icase);
            if (!decoded.has_payload_claim("email")
                || !std::regex_match(decoded.get_payload_claim("email").as_string(), institutionalEmail)) {
                throw AppError(403, "institutional_email_required", "Use um e-mail institucional da UFOP.");
            }
            context.emailVerified = decoded.has_payload_claim("email_verified")
                && decoded.get_payload_claim("email_verified").as_boolean();
            return context;
        } catch (const AppError&) {
            throw;
        } catch (const std::exception&) {
            throw InvalidToken{};
        }
    }
}
