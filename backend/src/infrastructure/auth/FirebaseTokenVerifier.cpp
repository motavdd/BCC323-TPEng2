#include "infrastructure/auth/FirebaseTokenVerifier.hpp"

#include <jwt-cpp/jwt.h>

#include <chrono>
#include <stdexcept>

namespace bcc323 {
    FirebaseTokenVerifier::FirebaseTokenVerifier( const std::string& projectId, FirebaseCertificateProvider& certificates) : projectId_(projectId), certificates_(certificates) {
        if (projectId.empty()) {
            throw std::invalid_argument("O indentificador do projeto não pode ser vazio.");
        }
    }

    AuthenticatedUser FirebaseTokenVerifier::verifyToken(const std::string& token) {
        if (token.empty()) {
            throw InvalidToken{};
        }

        // A leitura inicial ainda não comprova autenticidade.
        const auto decoded = [&]() {
            try {
                return jwt::decode(token);
            } catch (const std::exception&) {
                throw InvalidToken{};
            }
        }();

        std::string keyId;

        try {
            if (!decoded.has_algorithm() || decoded.get_algorithm() != "RS256" || !decoded.has_key_id()) {
                throw InvalidToken{};
            }

            keyId = decoded.get_key_id();

            if (keyId.empty()) {
                throw InvalidToken{};
            }
        } catch (const InvalidToken&) {
            throw;
        } catch (const std::exception&) {
            throw InvalidToken{};
        }

        // Falhas de rede continuam sendo CertificateUnavailable.
        const std::string certificate =
            certificates_.certificateFor(keyId);

        // Configuração criptográfica fica fora do tratamento
        // de token inválido: certificado inutilizável é falha interna.
        const auto algorithm =
            jwt::algorithm::rs256(certificate, "", "", "");

        auto verifier = jwt::verify()
            .allow_algorithm(algorithm)
            .with_issuer(
                "https://securetoken.google.com/" + projectId_
            )
            .with_audience(projectId_);

        std::string uid;

        try {
            if (!decoded.has_expires_at() || !decoded.has_issued_at() || !decoded.has_subject() || !decoded.has_payload_claim("auth_time")) {
                throw InvalidToken{};
            }

            verifier.verify(decoded);

            const auto now = std::chrono::system_clock::now();

            if (decoded.get_expires_at() <= now || decoded.get_issued_at() > now) {
                throw InvalidToken{};
            }

            const auto authTime = decoded.get_payload_claim("auth_time").as_integer();

            const auto nowSeconds = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();

            if (authTime > nowSeconds) {
                throw InvalidToken{};
            }

            uid = decoded.get_subject();

            if (uid.empty() || uid.size() > 128) {
                throw InvalidToken{};
            }
        } catch (const InvalidToken&) {
            throw;
        } catch (const std::exception&) {
            throw InvalidToken{};
        }

        AuthenticatedUser user{};
        user.uid = uid;
        return user;
    }
}
