#include "infrastructure/auth/HttpFirebaseCertificateProvider.hpp"
#include "infrastructure/auth/TokenVerifier.hpp"

#include <crow/json.h>
#include <curl/curl.h>

#include <memory>
#include <string>

namespace {

    constexpr const char* certificatesUrl =
        "https://www.googleapis.com/robot/v1/metadata/x509/"
        "securetoken@system.gserviceaccount.com";

    constexpr std::size_t maxResponseSize = 1024 * 1024;

    std::size_t receiveBody(char* data, std::size_t size, std::size_t count, void* destination) noexcept {
        // Evita overflow na multiplicação.
        if (size != 0 && count > maxResponseSize / size) {
            return 0;
        }

        const std::size_t bytes = size * count;
        auto& body = *static_cast<std::string*>(destination);

        if (bytes > maxResponseSize - body.size()) {
            return 0;
        }

        try {
            body.append(data, bytes);
            return bytes;
        } catch (...) {
            // Exceções C++ não devem atravessar a callback da libcurl.
            return 0;
        }
    }

} // namespace

namespace bcc323 {

    std::string HttpFirebaseCertificateProvider::certificateFor(const std::string& keyId) {
        if (keyId.empty()) {
            throw InvalidToken{};
        }

        // Inicialização única, antes de criar os handles.
        static const CURLcode initialization = curl_global_init(CURL_GLOBAL_DEFAULT);

        if (initialization != CURLE_OK) {
            throw CertificateUnavailable{};
        }

        using CurlHandle = std::unique_ptr<CURL, decltype(&curl_easy_cleanup)>;

        CurlHandle request(curl_easy_init(), &curl_easy_cleanup);

        if (!request) {
            throw CertificateUnavailable{};
        }

        std::string body;

        // Confere se a configuração de cada opção foi aceita.
        const auto configure = [&](CURLoption option, auto value) {
            if (curl_easy_setopt(request.get(), option, value) != CURLE_OK) {
                throw CertificateUnavailable{};
            }
        };

        configure(CURLOPT_URL, certificatesUrl);
        configure(CURLOPT_WRITEFUNCTION, &receiveBody);
        configure(CURLOPT_WRITEDATA, &body);
        configure(CURLOPT_CONNECTTIMEOUT, 5L);
        configure(CURLOPT_TIMEOUT, 10L);
        configure(CURLOPT_NOSIGNAL, 1L);

        // Mantém a validação do servidor HTTPS habilitada.
        configure(CURLOPT_SSL_VERIFYPEER, 1L);
        configure(CURLOPT_SSL_VERIFYHOST, 2L);

        const CURLcode result = curl_easy_perform(request.get());

        long status = 0;
        const CURLcode statusResult = curl_easy_getinfo(
            request.get(),
            CURLINFO_RESPONSE_CODE,
            &status
        );

        if (result != CURLE_OK || statusResult != CURLE_OK || status != 200) {
            throw CertificateUnavailable{};
        }

        auto certificates = crow::json::load(body);

        if (!certificates || certificates.t() != crow::json::type::Object) {
            throw CertificateUnavailable{};
        }

        if (!certificates.has(keyId)) {
            throw InvalidToken{};
        }

        const auto& certificate = certificates[keyId];

        if (certificate.t() != crow::json::type::String) {
            throw CertificateUnavailable{};
        }

        std::string pem = certificate.s();

        if (pem.empty()) {
            throw CertificateUnavailable{};
        }

        return pem;
    }

} // namespace bcc323
