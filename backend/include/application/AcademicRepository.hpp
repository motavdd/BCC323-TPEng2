#pragma once
#include <map>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>
#include <utility>

namespace bcc323 {

    struct AppError : std::runtime_error {
        int status;
        std::string code;
        AppError(int status, std::string code, std::string message) : std::runtime_error(std::move(message)), status(status), code(std::move(code)) {}
    };

    // UID é a identidade persistente. Nunca se recebe o UID do corpo da requisição.
    struct RequestContext { std::string uid; std::string token; };

    struct AcademicProfile { std::string displayName; std::string course; std::string courseId; int period = 1; };

    struct SubjectRecord {
        std::string id, name;
        int hours = 0, period = 1;
        std::vector<std::string> prerequisites;
        bool requirementsKnown = false;
    };

    struct ClassTimeRecord { std::string day, start, end; std::string room{}; };

    struct OfferingRecord {
        std::string id, subjectId, group;
        std::vector<ClassTimeRecord> times;
        std::string professor{};
    };

    struct CourseRecord {
        std::string id, name;
        bool illustrative = false;
        std::vector<SubjectRecord> subjects;
        std::vector<OfferingRecord> offerings;
    };

    struct AcademicTerm { std::string id, start, end; };
    struct ScheduledChange {
        std::string term, effectiveDate;
        std::optional<SubjectRecord> subject;
        std::optional<OfferingRecord> offering;
    };
    struct AcademicSchedule {
        std::string revision;
        std::vector<ScheduledChange> changes;
    };
    struct SubjectEvaluation { std::string state; std::vector<std::string> missing; };
    
    struct AcademicSummary {
        std::string courseId;
        bool illustrative = false;
        int completedHours = 0, completedCount = 0, availableCount = 0, totalHours = 0;
        double progress = 0;
        std::vector<std::string> completed, unmappedCompleted;
        std::map<std::string, std::string> completedNames;
        std::map<std::string, SubjectEvaluation> subjects;
    };

    class AcademicRepository {
    public:
        virtual ~AcademicRepository() = default;
        virtual std::optional<AcademicProfile> profile(const RequestContext&) = 0;
        virtual void saveProfile(const RequestContext&, const AcademicProfile&, bool create) = 0;
        virtual std::vector<CourseRecord> courses(const RequestContext&) = 0;
        virtual CourseRecord course(const RequestContext&, const std::string& courseId) = 0;
        virtual std::vector<std::string> completed(const RequestContext&) = 0;
        virtual std::map<std::string, std::string> completedNames(const RequestContext&) = 0;
        virtual void setCompleted(const RequestContext&, const std::string&, bool, const std::string&) = 0;
        virtual bool coordinates(const RequestContext&, const std::string&) = 0;
        virtual std::vector<AcademicTerm> academicTerms(const RequestContext&) = 0;
        virtual AcademicSchedule schedule(const RequestContext&, const std::string&) = 0;
        virtual void saveSchedule(const RequestContext&, const std::string&, const AcademicSchedule&) = 0;
    };
}
