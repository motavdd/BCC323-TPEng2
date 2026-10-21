#ifndef FIREBASE_CERTIFICATE_PROVIDER_HPP
#define FIREBASE_CERTIFICATE_PROVIDER_HPP

#include <stdexcept>
#include <string>

namespace bcc323 {

    class CertificateUnavailable : public std::runtime_error {
    public:
        CertificateUnavailable()
            : std::runtime_error(
                "Não foi possível obter os certificados de autenticação"
            ) {}
    };

    class FirebaseCertificateProvider {
    public:
        virtual ~FirebaseCertificateProvider() = default;

        virtual std::string certificateFor(
            const std::string& keyId
        ) = 0;
    };

}

#endif
