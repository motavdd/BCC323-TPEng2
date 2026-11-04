#ifndef HTTP_FIREBASE_CERTIFICATE_PROVIDER_HPP
#define HTTP_FIREBASE_CERTIFICATE_PROVIDER_HPP

#include "infrastructure/auth/FirebaseCertificateProvider.hpp"

namespace bcc323 {

    class HttpFirebaseCertificateProvider
        : public FirebaseCertificateProvider {
    public:
        std::string certificateFor(
            const std::string& keyId
        ) override;
    };

} // namespace bcc323

#endif
