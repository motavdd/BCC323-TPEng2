#include "http/academic_routes.hpp"
#include "http/AcademicJson.hpp"
#include "application/AcademicService.hpp"
#include "infrastructure/auth/RequestAuthenticator.hpp"
#include "infrastructure/auth/TokenVerifier.hpp"
#include "infrastructure/auth/FirebaseCertificateProvider.hpp"
#include "infrastructure/firestore/FirestoreRepository.hpp"
#ifdef DELETE
#undef DELETE
#endif

namespace {

    using Json = crow::json::wvalue;
    crow::response error(int status, const std::string& code, const std::string& message) {
        Json body; body["error"]["code"] = code; body["error"]["message"] = message;
        crow::response response(status, body); response.set_header("Cache-Control", "no-store"); return response;
    }

    template<class Action> crow::response handle(const crow::request& request, Action action) {
        try {
            const auto context = bcc323::authenticate(request);
            if (request.body.size() > 16384){
                return error(413, "payload_too_large", "Dados demais.");
            } 
            bcc323::FirestoreRepository repository;
            bcc323::AcademicService service(repository);
            crow::response response(action(context, service));
            response.set_header("Cache-Control", "no-store"); return response;
        } catch (const bcc323::AppError& e) { 
            return error(e.status, e.code, e.what());
        } catch (const bcc323::InvalidToken&) { 
            return error(401, "unauthenticated", "Entre novamente para continuar.");
        } catch (const bcc323::CertificateUnavailable&) { 
            return error(503, "auth_unavailable", "Autenticação temporariamente indisponível.");
        } catch (const std::exception&) {
            CROW_LOG_ERROR << "Falha interna ao processar operação acadêmica";
            return error(500, "internal_error", "Não foi possível concluir a operação.");
        }
    }
}
namespace bcc323 {
    void register_academic_routes(crow::SimpleApp& app) {
        CROW_ROUTE(app, "/api/me")([](const crow::request& request) {
            return handle(request, [](const RequestContext& context, AcademicService&) {
                Json body; body["uid"] = context.uid; return body;
            });
        });
        CROW_ROUTE(app, "/api/courses")([](const crow::request& request) {
            return handle(request, [](const RequestContext& context, AcademicService& service) {
                Json body; std::vector<Json> courses;
                for (const auto& c : service.courses(context)) {
                    Json item; item["id"] = c.id; item["name"] = c.name; item["illustrative"] = c.illustrative;
                    courses.push_back(std::move(item));
                }
                body["courses"] = std::move(courses); return body;
            });
        });
        CROW_ROUTE(app, "/api/profile").methods(crow::HTTPMethod::GET, crow::HTTPMethod::PUT)
        ([](const crow::request& request) {
            return handle(request, [&](const RequestContext& context, AcademicService& service) {
                Json body;
                if (request.method == crow::HTTPMethod::PUT){
                    body["profile"] = toJson(service.saveProfile(context, profileFromJson(crow::json::load(request.body))));
                } else {
                    const auto profile = service.profile(context);
                    if (profile){
                        body["profile"] = toJson(*profile);
                    } else {
                        body["profile"] = nullptr;
                    }
                }
                return body;
            });
        });

        CROW_ROUTE(app, "/api/curriculum")([](const crow::request& request) {
            return handle(request, [](const RequestContext& context, AcademicService& service) { return toJson(service.curriculum(context)); });
        });

        CROW_ROUTE(app, "/api/academic/summary")([](const crow::request& request) {
            return handle(request, [](const RequestContext& context, AcademicService& service) { return toJson(service.summary(context)); });
        });

        CROW_ROUTE(app, "/api/history")([](const crow::request& request) {
            return handle(request, [](const RequestContext& context, AcademicService& service) {
                const auto summary = service.summary(context);
                Json body; body["completed"] = summary.completed; body["unmappedCompleted"] = summary.unmappedCompleted; return body;
            });
        });
        
        CROW_ROUTE(app, "/api/history/<string>").methods(crow::HTTPMethod::PUT, crow::HTTPMethod::DELETE)
        ([](const crow::request& request, const std::string& id) {
            return handle(request, [&](const RequestContext& context, AcademicService& service) {
                return toJson(service.setCompleted(context, id, request.method == crow::HTTPMethod::PUT));
            });
        });
    }
}
