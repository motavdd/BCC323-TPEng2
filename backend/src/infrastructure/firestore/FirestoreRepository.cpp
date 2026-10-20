#include "infrastructure/firestore/FirestoreRepository.hpp"
#include <curl/curl.h>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <regex>

namespace {
    using Json = crow::json::wvalue;
    std::string env(const char* name) { 
        const char* value = std::getenv(name); return value ? value : ""; 
    }
    std::string segment(const std::string& value) {
        if (!std::regex_match(value, std::regex("[A-Za-z0-9_-]{1,128}"))){
            throw bcc323::AppError(400, "invalid_identifier", "Identificador inválido.");

        }
        return value;
    }

    Json encode(const crow::json::rvalue& value) {
        Json result;
        switch (value.t()) {
            case crow::json::type::String: {
                result["stringValue"] = std::string(value.s()); 
                break;
            } 
            case crow::json::type::True: case crow::json::type::False: {
                result["booleanValue"] = value.b(); 
                break;
            }
            case crow::json::type::Number: {
                if (std::floor(value.d()) == value.d()){
                    result["integerValue"] = std::to_string(value.i());
                } else {
                    result["doubleValue"] = value.d();
                }
                break;
            }
            case crow::json::type::List: {
                std::vector<Json> items;
                for (const auto& item : value){
                     items.push_back(encode(item));
                }
                result["arrayValue"]["values"] = std::move(items); break;
            }
            case crow::json::type::Object: {
                result["mapValue"]["fields"] = Json::object{};
                for (const auto& item : value) result["mapValue"]["fields"][std::string(item.key())] = encode(item);
                break;
                
            }
            default: result["nullValue"] = nullptr;
        }
        return result;
    }

    Json decode(const crow::json::rvalue& value) {
        if (value.has("stringValue")) {
            return std::string(value["stringValue"].s());
        }
        if (value.has("integerValue")){
            return static_cast<std::int64_t>(std::stoll(std::string(value["integerValue"].s())));
        } 
        if (value.has("doubleValue")) {
            return value["doubleValue"].d();
        }
        if (value.has("booleanValue")) {
            return value["booleanValue"].b();
        }
        if (value.has("timestampValue")) {
            return std::string(value["timestampValue"].s());
        }
        if (value.has("arrayValue")) {
            std::vector<Json> items;
            if (value["arrayValue"].has("values")) {
                for (const auto& item : value["arrayValue"]["values"]) {
                    items.push_back(decode(item));
                }
            }
            return items;
        }
        Json result(Json::object{});
        if (value.has("mapValue") && value["mapValue"].has("fields")) {
            for (const auto& item : value["mapValue"]["fields"]) {
                result[std::string(item.key())] = decode(item);
            }
        }
        return result;
    }

    Json fieldsOf(const crow::json::rvalue& document) {
        Json result(Json::object{});
        if (document.has("fields")) {
            for (const auto& field : document["fields"]) {
                result[std::string(field.key())] = decode(field);
            }
        }
        return result;
    }

    bcc323::CourseRecord parseCourse(const Json& document) {
        const auto raw = crow::json::load(document.dump());
        const auto fields = crow::json::load(fieldsOf(raw).dump());
        bcc323::CourseRecord course;

        const std::string name = raw["name"].s();
        course.id = name.substr(name.find_last_of('/') + 1);
        course.name = fields["name"].s();

        if (course.name.empty() || course.name.size() > 100 || !fields.has("subjects") || fields["subjects"].t() != crow::json::type::List || fields["subjects"].size() == 0 || fields["subjects"].size() > 500) {
            throw std::invalid_argument("Matriz inválida");

        }
        course.illustrative = fields.has("illustrative") && fields["illustrative"].b();
        for (const auto& s : fields["subjects"]) {
            bcc323::SubjectRecord subject;
            subject.id = segment(s["id"].s()); subject.name = s["name"].s();

            if (s["hours"].i() < 0 || s["hours"].i() > 10000 || s["period"].i() < 1 || s["period"].i() > 20){
                throw std::invalid_argument("Carga horária ou período inválido");

            }

            subject.hours = static_cast<int>(s["hours"].i()); subject.period = static_cast<int>(s["period"].i());
            subject.requirementsKnown = s.has("requirementsKnown") && s["requirementsKnown"].b();

            if (s.has("prerequisites")){
                for (const auto& p : s["prerequisites"]) {
                    subject.prerequisites.push_back(segment(p.s()));
                }
            }
            course.subjects.push_back(std::move(subject));
        }
        if (fields.has("offerings")) {
            for (const auto& o : fields["offerings"]) {
                bcc323::OfferingRecord offering;
                offering.id = o["id"].s(); offering.subjectId = o["subjectId"].s(); offering.group = o["group"].s();
                for (const auto& t : o["times"]) {
                    offering.times.push_back({t["day"].s(), t["start"].s(), t["end"].s()});
                }
                course.offerings.push_back(std::move(offering));
            }
        }
        return course;
    }

    bcc323::CourseRecord courseOf(const Json& document) {
        try { return parseCourse(document); }
        catch (const std::exception&) {
            throw bcc323::AppError(503, "invalid_curriculum", "A matriz cadastrada precisa de revisão.");
        }
    }

    std::size_t receive(char* data, std::size_t size, std::size_t count, void* target) noexcept {
        constexpr std::size_t limit = 8 * 1024 * 1024;
        auto& body = *static_cast<std::string*>(target);
        if (size != 0 && count > limit / size) {
            return 0;
        }
        const auto bytes = size * count;
        if (bytes > limit - body.size()) {
            return 0;
        }
        try { 
            body.append(data, bytes); return bytes; 
        } catch (...) { 
            return 0; 
        }
    }
}

namespace bcc323 {
    FirestoreRepository::FirestoreRepository() {
        static const auto initialization = curl_global_init(CURL_GLOBAL_DEFAULT);
        if (initialization != CURLE_OK) {
            throw AppError(503, "storage_unavailable", "Banco indisponível.");
        }
        const auto project = env("FIREBASE_PROJECT_ID");
        if (project.empty()) {
            throw AppError(503, "api_not_configured", "Configure FIREBASE_PROJECT_ID no backend.");
        }
        const auto host = env("FIRESTORE_EMULATOR_HOST");
        if (!host.empty() && (project.rfind("demo-", 0) != 0 || !std::regex_match(host, std::regex("[A-Za-z0-9.\\-]+:[0-9]{1,5}")))){
            throw AppError(503, "invalid_emulator_config", "O emulador exige um projeto demo e host:porta.");

        }
        base_ = host.empty() ? "https://firestore.googleapis.com/v1/" : "http://" + host + "/v1/";
        documentName_ = "projects/" + segment(project) + "/databases/(default)/documents";
        documents_ = base_ + documentName_;
    }

    Json FirestoreRepository::request(const RequestContext& context, const std::string& method, const std::string& url, const Json* input, bool allowMissing) {

        static const auto init = curl_global_init(CURL_GLOBAL_DEFAULT);
        if (init != CURLE_OK) {
            throw AppError(503, "storage_unavailable", "Banco temporariamente indisponível.");
        }
        std::unique_ptr<CURL, decltype(&curl_easy_cleanup)> handle(curl_easy_init(), &curl_easy_cleanup);
        if (!handle) {
            throw AppError(503, "storage_unavailable", "Banco temporariamente indisponível.");
        }
        curl_slist* rawHeaders = nullptr;
        rawHeaders = curl_slist_append(rawHeaders, ("Authorization: Bearer " + context.token).c_str());
        rawHeaders = curl_slist_append(rawHeaders, "Content-Type: application/json");
        std::unique_ptr<curl_slist, decltype(&curl_slist_free_all)> headers(rawHeaders, &curl_slist_free_all);
        const auto configure = [&](CURLoption option, auto value) {
            if (curl_easy_setopt(handle.get(), option, value) != CURLE_OK){
                throw AppError(503, "storage_unavailable", "Banco temporariamente indisponível.");

            }
        };
        std::string response;
        const auto body = input ? input->dump() : "";
        configure(CURLOPT_URL, url.c_str()); 
        configure(CURLOPT_CUSTOMREQUEST, method.c_str());
        configure(CURLOPT_HTTPHEADER, headers.get()); 
        configure(CURLOPT_WRITEFUNCTION, &receive);
        configure(CURLOPT_WRITEDATA, &response); 
        configure(CURLOPT_CONNECTTIMEOUT, 3L);
        configure(CURLOPT_TIMEOUT, 10L); 
        configure(CURLOPT_NOSIGNAL, 1L);
        configure(CURLOPT_SSL_VERIFYPEER, 1L); 
        configure(CURLOPT_SSL_VERIFYHOST, 2L);
        if (input) { 
            configure(CURLOPT_POSTFIELDS, body.c_str()); 
            configure(CURLOPT_POSTFIELDSIZE, static_cast<long>(body.size())); 
        }

        const auto result = curl_easy_perform(handle.get());
        long status = 0;
        if (result != CURLE_OK || curl_easy_getinfo(handle.get(), CURLINFO_RESPONSE_CODE, &status) != CURLE_OK){
            throw AppError(503, "storage_unavailable", "Não foi possível acessar o banco. Tente novamente.");

        }
        if (status == 404 && allowMissing) {
            return nullptr;
        }
        if (status == 401) {
            throw AppError(401, "unauthenticated", "Entre novamente para continuar.");
        }
        if (status == 403) {
            throw AppError(403, "permission_denied", "Acesso aos dados não autorizado.");
        }
        if (status == 404) {
            throw AppError(404, "course_not_found", "Curso ainda não cadastrado.");
        }
        if (status == 409 || status == 412) {
            throw AppError(409, "write_conflict", "Os dados foram alterados. Recarregue e tente novamente.");
        }
        if (status < 200 || status >= 300) {
            throw AppError(503, "storage_unavailable", "O banco não conseguiu concluir a operação.");
        }
        if (response.empty()) {
            return nullptr;
        }
        const auto parsed = crow::json::load(response);
        if (!parsed) {
            throw AppError(503, "invalid_storage_response", "Resposta inválida do banco.");
        }
        return Json(parsed);
    }

    std::vector<Json> FirestoreRepository::list(const RequestContext& context, const std::string& path) {
        std::vector<Json> documents;
        std::string page;
        do {
            std::unique_ptr<CURL, decltype(&curl_easy_cleanup)> handle(curl_easy_init(), &curl_easy_cleanup);
            // pageToken vem do Firestore, mas ainda deve ser codificado como query string.
            std::string query = "?pageSize=100";

            if (!page.empty()) {
                if (!handle) {
                    throw AppError(503, "storage_unavailable", "Banco indisponível.");
                }
                std::unique_ptr<char, decltype(&curl_free)> escaped(curl_easy_escape(handle.get(), page.c_str(), 0), &curl_free);
                if (!escaped) {
                    throw AppError(503, "storage_unavailable", "Banco indisponível.");
                }
                query += "&pageToken=" + std::string(escaped.get());
            }

            const auto raw = request(context, "GET", documents_ + "/" + path + query);
            const auto data = crow::json::load(raw.dump());

            if (data.has("documents")) {
                for (const auto& d : data["documents"]) {
                    documents.emplace_back(d);
                }
            }

            page = data.has("nextPageToken") ? std::string(data["nextPageToken"].s()) : "";

            if (documents.size() > 1000) {
                throw AppError(503, "collection_too_large", "Dados acadêmicos excedem o limite do MVP.");
            }
        } while (!page.empty());
        return documents;
    }

    void FirestoreRepository::write(const RequestContext& context, const std::string& path, const Json& fields, const std::string& timestampField, bool createOnly) {
        const auto plain = crow::json::load(fields.dump());
        Json commit, write;
        write["update"]["name"] = documentName_ + "/" + path;
        write["update"]["fields"] = Json::object{};
        std::vector<Json> mask;

        for (const auto& f : plain) {
            write["update"]["fields"][std::string(f.key())] = encode(f);
            mask.emplace_back(std::string(f.key()));
        }

        write["updateMask"]["fieldPaths"] = std::move(mask);
        if (createOnly) {
            write["currentDocument"]["exists"] = false;
        }
        if (!timestampField.empty()) {
            Json transform; transform["fieldPath"] = timestampField; transform["setToServerValue"] = "REQUEST_TIME";
            std::vector<Json> transforms; transforms.push_back(std::move(transform));
            write["updateTransforms"] = std::move(transforms);
        }
        std::vector<Json> writes; writes.push_back(std::move(write)); 
        commit["writes"] = std::move(writes);
        request(context, "POST", documents_ + ":commit", &commit);
    }

    std::optional<AcademicProfile> FirestoreRepository::profile(const RequestContext& context) {
        const auto raw = request(context, "GET", documents_ + "/users/" + segment(context.uid), nullptr, true);
        if (raw.dump() == "null") {
            return std::nullopt;
        }

        const auto fields = crow::json::load(fieldsOf(crow::json::load(raw.dump())).dump());
        AcademicProfile p;
        p.displayName = fields["displayName"].s(); p.course = fields["course"].s(); p.period = static_cast<int>(fields["period"].i());

        if (fields.has("courseId")) {
            p.courseId = fields["courseId"].s();
        }
        return p;
    }
    void FirestoreRepository::saveProfile(const RequestContext& context, const AcademicProfile& p, bool create) {
        Json fields; fields["displayName"] = p.displayName; fields["course"] = p.course; fields["courseId"] = p.courseId; fields["period"] = p.period;
        write(context, "users/" + segment(context.uid), fields, create ? "createdAt" : "", create);
    }
    std::vector<CourseRecord> FirestoreRepository::courses(const RequestContext& context) {
        std::vector<CourseRecord> result;
        for (const auto& document : list(context, "courses")) {
            result.push_back(courseOf(document));
        }
        return result;
    }
    CourseRecord FirestoreRepository::course(const RequestContext& context, const std::string& id) {
        return courseOf(request(context, "GET", documents_ + "/courses/" + segment(id)));
    }
    std::vector<std::string> FirestoreRepository::completed(const RequestContext& context) {
        std::vector<std::string> result;
        for (const auto& d : list(context, "users/" + segment(context.uid) + "/completedCourses")) {
            const auto parsed = crow::json::load(d.dump()); const std::string name = parsed["name"].s();
            result.push_back(name.substr(name.find_last_of('/') + 1));
        }
        return result;
    }
    void FirestoreRepository::setCompleted(const RequestContext& context, const std::string& id, bool done) {
        const auto path = "users/" + segment(context.uid) + "/completedCourses/" + segment(id);
        if (done) { 
            Json fields(Json::object{}); 
            write(context, path, fields, "completedAt"); 
        } else {
            request(context, "DELETE", documents_ + "/" + path, nullptr, true);
        }
    }
}
