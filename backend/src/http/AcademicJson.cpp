#include "http/AcademicJson.hpp"
#include <cmath>

namespace bcc323 {
    using Json = crow::json::wvalue;
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
            item["requirementsKnown"] = s.requirementsKnown; subjects.push_back(std::move(item));
        }
        for (const auto& o : c.offerings) {
            Json item; item["id"] = o.id; item["subjectId"] = o.subjectId; item["group"] = o.group;
            item["professor"] = o.professor;
            std::vector<Json> times;
            for (const auto& t : o.times) {
                Json time;
                time["day"] = t.day; time["start"] = t.start; time["end"] = t.end; time["room"] = t.room;
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
        out["completedNames"] = Json::object{};
        for (const auto& [id, name] : s.completedNames) out["completedNames"][id] = name;
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

    namespace {
        [[noreturn]] void invalidChange() {
            throw AppError(400, "invalid_change", "Dados do agendamento inválidos.");
        }

        std::string readString(const crow::json::rvalue& input, const char* field) {
            if (!input.has(field) || input[field].t() != crow::json::type::String) invalidChange();
            return input[field].s();
        }

        int readInteger(const crow::json::rvalue& input, const char* field, int minimum, int maximum) {
            if (!input.has(field) || input[field].t() != crow::json::type::Number) invalidChange();
            const auto value = input[field].d();
            if (!std::isfinite(value) || value < minimum || value > maximum
                || value != static_cast<int>(value)) invalidChange();
            return static_cast<int>(value);
        }
    }

    ScheduledChange changeFromJson(const crow::json::rvalue& input) {
        if (!input || input.t() != crow::json::type::Object) invalidChange();
        ScheduledChange change;
        change.term = readString(input, "term");
        if (input.has("effectiveDate")) change.effectiveDate = readString(input, "effectiveDate");
        const auto type = readString(input, "type");
        const auto id = readString(input, "id");
        if (type == "subject") {
            SubjectRecord subject{id, readString(input, "name"), readInteger(input, "hours", 0, 10000),
                readInteger(input, "period", 1, 20), {}, true};
            if (!input.has("prerequisites") || input["prerequisites"].t() != crow::json::type::List
                || input["prerequisites"].size() > 500) invalidChange();
            for (const auto& item : input["prerequisites"]) {
                if (item.t() != crow::json::type::String) invalidChange();
                subject.prerequisites.emplace_back(item.s());
            }
            change.subject = std::move(subject);
        } else if (type == "offering") {
            OfferingRecord offering;
            offering.id = id;
            if (input.has("professor")) {
                offering.professor = readString(input, "professor");
                if (offering.professor.empty()) invalidChange();
            }
            if (input.has("times")) {
                const auto& times = input["times"];
                if (times.t() != crow::json::type::List || times.size() == 0 || times.size() > 24) invalidChange();
                for (const auto& slot : times) {
                    if (slot.t() != crow::json::type::Object) invalidChange();
                    offering.times.push_back({readString(slot, "day"), readString(slot, "start"),
                        readString(slot, "end"), slot.has("room") ? readString(slot, "room") : ""});
                }
            }
            if (offering.professor.empty() && offering.times.empty()) invalidChange();
            change.offering = std::move(offering);
        } else {
            invalidChange();
        }
        return change;
    }
}
