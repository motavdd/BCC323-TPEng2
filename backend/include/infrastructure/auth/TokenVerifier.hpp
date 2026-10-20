#ifndef TOKEN_VERIFIER_HPP
#define TOKEN_VERIFIER_HPP

#include <string>
#include <stdexcept>

namespace bcc323 {
    struct AuthenticatedUser {
        std::string uid;
        std::string role;
    };

    class InvalidToken : public std::runtime_error {
    public:
        InvalidToken() : std::runtime_error("Token de autenticação inválido  ") {}
    };

    class TokenVerifier {
    public:
        virtual ~TokenVerifier() = default;
        virtual AuthenticatedUser verifyToken(const std::string& token) = 0;
    };
} // namespace bcc323

#endif
