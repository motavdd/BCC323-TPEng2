#ifndef FIREBASE_TOKEN_VERIFIER_HPP
#define FIREBASE_TOKEN_VERIFIER_HPP

#include "infrastructure/auth/TokenVerifier.hpp"
#include "infrastructure/auth/FirebaseCertificateProvider.hpp"

namespace bcc323 {

    class FirebaseTokenVerifier : public TokenVerifier {
    public:
        FirebaseTokenVerifier(
            const std::string& projectId,
            FirebaseCertificateProvider& certificates
        );

        AuthenticatedUser verifyToken(
            const std::string& token
        ) override;

    private:
        std::string projectId_;
        FirebaseCertificateProvider& certificates_;
    };

} // namespace bcc323

#endif
