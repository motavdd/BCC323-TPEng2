#include "infrastructure/auth/FirebaseTokenVerifier.hpp"

#include <jwt-cpp/jwt.h>

#include <chrono>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <cstdint>
#include <utility>

using namespace bcc323;

std::string readFile(const std::string& path) {
    std::ifstream file(path);

    if (!file) {
        throw std::runtime_error("Não foi possível abrir: " + path);
    }

    return {
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    };
}

// Substitui o fornecedor HTTP durante os testes.
class TestCertificates : public FirebaseCertificateProvider {
public:
    explicit TestCertificates(std::string publicKey) : publicKey_(std::move(publicKey)) {}

    std::string certificateFor(const std::string& keyId) override {
        if (keyId != "test-key") {
            throw InvalidToken{};
        }

        return publicKey_;
    }

private:
    std::string publicKey_;
};

template <typename Operation>
void expectInvalid(const std::string& name, Operation operation){
    try {
        operation();
    } catch (const InvalidToken&) {
        std::cout << "PASS: " << name << '\n';
        return;
    }

    throw std::runtime_error(
        "Token inválido foi aceito: " + name
    );
}

int main() {
    try {
        const std::string fixtures = AUTH_FIXTURES_DIR;

        const auto privateKey =
            readFile(fixtures + "/private.pem");

        const auto publicKey =
            readFile(fixtures + "/public.pem");

        TestCertificates certificates(publicKey);

        FirebaseTokenVerifier verifier(
            "test-project",
            certificates
        );

        const auto now = std::chrono::system_clock::now();

        const auto makeToken = [&](
            const std::string& audience,
            std::chrono::system_clock::time_point expiresAt,
            const std::string& keyId
        ) {
            return jwt::create()
                .set_key_id(keyId)
                .set_issuer(
                    "https://securetoken.google.com/test-project"
                )
                .set_audience(audience)
                .set_subject("aluno-teste")
                .set_issued_at(now - std::chrono::seconds(10))
                .set_expires_at(expiresAt)
                .set_payload_claim(
                    "auth_time",
                    jwt::claim(now - std::chrono::seconds(10))
                )
                .sign(jwt::algorithm::rs256(publicKey, privateKey, "", ""));
        };

        const auto valid = makeToken(
            "test-project",
            now + std::chrono::minutes(5),
            "test-key"
        );

        const auto user = verifier.verifyToken(valid);

        if (user.uid != "aluno-teste") {
            throw std::runtime_error("UID retornado incorretamente");
        }

        std::cout << "PASS: token válido\n";

        expectInvalid("token vazio", [&] {
            verifier.verifyToken("");
        });

        expectInvalid("token malformado", [&] {
            verifier.verifyToken("isso-nao-e-um-jwt");
        });

        expectInvalid("token expirado", [&] {
            verifier.verifyToken(makeToken(
                "test-project",
                now - std::chrono::seconds(1),
                "test-key"
            ));
        });

        expectInvalid("outro projeto", [&] {
            verifier.verifyToken(makeToken(
                "outro-projeto",
                now + std::chrono::minutes(5),
                "test-key"
            ));
        });

        expectInvalid("chave desconhecida", [&] {
            verifier.verifyToken(makeToken(
                "test-project",
                now + std::chrono::minutes(5),
                "unknown-key"
            ));
        });

        expectInvalid("assinatura adulterada", [&] {
            auto altered = valid;

            const auto signatureStart =
                altered.rfind('.') + 1;

            altered[signatureStart] =
                altered[signatureStart] == 'A' ? 'B' : 'A';

            verifier.verifyToken(altered);
        });

        return 0;
    } catch (const std::exception& error) {
        std::cerr << "FAIL: " << error.what() << '\n';
        return 1;
    }
}
