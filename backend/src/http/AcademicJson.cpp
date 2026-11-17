#include "http/AcademicJson.hpp"
#include <cmath>
#include <initializer_list>

namespace bcc323 {
    using Json = crow::json::wvalue;
    namespace {
        using Input = crow::json::rvalue;
        using Type = crow::json::type;

        void validateFields(const Input& object, std::initializer_list<const char*> allowed) {
            if (!object || object.t() != Type::Object || object.size() != allowed.size()) {
                throw AppError(400, "invalid_catalog_data", "Confira os campos enviados.");
            }
            for (const auto* field : allowed) {
                if (!object.has(field)) {
                    throw AppError(400, "invalid_catalog_data", "Confira os campos enviados.");
                }
            }
        }

        std::string readText(const Input& value) {
            if (value.t() != Type::String) {
                throw AppError(400, "invalid_catalog_data", "Era esperado um campo de texto.");
            }
            const std::string text = value.s();
            const auto first = text.find_first_not_of(" \t\r\n");
            return first == std::string::npos ? "" : text.substr(first, text.find_last_not_of(" \t\r\n") - first + 1);
        }

        int readInteger(const Input& value, int min, int max) {
            if (value.t() != Type::Number) {
                throw AppError(400, "invalid_catalog_data", "Informe um número inteiro.");
            }
            const auto number = value.d();
            if (!std::isfinite(number) || number < min || number > max || std::floor(number) != number) {
                throw AppError(400, "invalid_catalog_data", "Número inteiro fora do intervalo permitido.");
            }
            return static_cast<int>(number);
        }
    }
    Json toJson(const AcademicProfile& p) {
        Json out; out["displayName"] = p.displayName; out["course"] = p.course;
        out["courseId"] = p.courseId; out["period"] = p.period; return out;
    }
    Json toJson(const CourseRecord& c) {
        Json out; out["id"] = c.id; out["name"] = c.name; out["illustrative"] = c.illustrative;
        std::vector<Json> subjects, offerings;
        for (const auto& s : c.subjects) {
            Json item; item["id"] = s.id; item["name"] = s.name; item["hours"] = s.hours;
            item["period"] = s.period; item["prerequisites"] = s.prerequisites;
            item["requirementsKnown"] = s.requirementsKnown; item["archived"] = s.archived;
            subjects.push_back(std::move(item));
        }
        for (const auto& o : c.offerings) {
            Json item; item["id"] = o.id; item["subjectId"] = o.subjectId; item["group"] = o.group;
            item["capacity"] = o.capacity;
            std::vector<Json> times;
            for (const auto& t : o.times) {
                Json time; time["day"] = t.day; time["start"] = t.start; time["end"] = t.end; time["room"] = t.room;
                times.push_back(std::move(time));
            }
            item["times"] = std::move(times); offerings.push_back(std::move(item));
        }
        out["subjects"] = std::move(subjects); out["offerings"] = std::move(offerings); return out;
    }
    Json toJson(const AcademicSummary& s) {
        Json out; out["courseId"] = s.courseId; out["illustrative"] = s.illustrative;
        out["completedHours"] = s.completedHours; out["totalHours"] = s.totalHours;
        out["completedCount"] = s.completedCount; out["availableCount"] = s.availableCount;
        out["progress"] = s.progress; out["completed"] = s.completed; out["unmappedCompleted"] = s.unmappedCompleted;
        out["subjects"] = Json::object{};
        for (const auto& [id, state] : s.subjects) { 
            out["subjects"][id]["state"] = state.state; out["subjects"][id]["missing"] = state.missing; 
        }
        return out;
    }
    AcademicProfile profileFromJson(const crow::json::rvalue& input) {
        if (!input || input.t() != crow::json::type::Object || !input.has("course") || !input.has("period") || input["course"].t() != crow::json::type::String || input["period"].t() != crow::json::type::Number|| input["period"].d() < 1 || input["period"].d() > 20 || input["period"].d() != input["period"].i()){
            throw AppError(400, "invalid_profile", "Informe curso e período inteiro entre 1 e 20.");

        }
        for (const auto& field : input) {
            const std::string key = field.key();
            if (key != "course" && key != "period" && key != "courseId" && key != "displayName"){
                throw AppError(400, "unexpected_field", "O perfil contém campos não permitidos.");

            }
        }
        AcademicProfile p; p.course = input["course"].s(); p.period = static_cast<int>(input["period"].i());
        if (input.has("courseId")) {
            if (input["courseId"].t() != crow::json::type::String) {
                throw AppError(400, "invalid_profile", "Curso inválido.");
            }
            p.courseId = input["courseId"].s();
        }
        if (input.has("displayName")) {
            if (input["displayName"].t() != crow::json::type::String) {
                throw AppError(400, "invalid_profile", "Nome inválido.");
            }
            p.displayName = input["displayName"].s();
        }
        const auto first = p.course.find_first_not_of(" \t\r\n");
        p.course = first == std::string::npos ? "" : p.course.substr(first, p.course.find_last_not_of(" \t\r\n") - first + 1);
        return p;
    }

    SubjectRecord subjectFromJson(const std::string& id, const crow::json::rvalue& input) {
        validateFields(input, {"name", "hours", "period", "prerequisites", "requirementsKnown"});
        const auto& known = input["requirementsKnown"];
        const auto& prerequisites = input["prerequisites"];
        if ((known.t() != Type::True && known.t() != Type::False)
            || prerequisites.t() != Type::List || prerequisites.size() > 100) {
            throw AppError(400, "invalid_prerequisites", "Informe uma lista de pré-requisitos válida.");
        }
        SubjectRecord result{id, readText(input["name"]), readInteger(input["hours"], 1, 10000),
            readInteger(input["period"], 1, 20), {}, known.b()};
        for (const auto& prerequisite : prerequisites) {
            result.prerequisites.push_back(readText(prerequisite));
        }
        return result;
    }

    OfferingRecord offeringFromJson(const std::string& id, const crow::json::rvalue& input) {
        validateFields(input, {"subjectId", "group", "capacity", "times"});
        const auto& times = input["times"];
        if (times.t() != Type::List || times.size() > 20) {
            throw AppError(400, "invalid_offering", "Informe uma lista de horários válida.");
        }
        OfferingRecord result{id, readText(input["subjectId"]), readText(input["group"]), {},
            readInteger(input["capacity"], 0, 10000)};
        for (const auto& slot : times) {
            validateFields(slot, {"day", "start", "end", "room"});
            result.times.push_back({readText(slot["day"]), readText(slot["start"]), readText(slot["end"]), readText(slot["room"])});
        }
        return result;
    }
}
