#pragma once
#include "application/AcademicRepository.hpp"

namespace bcc323 {
    class AcademicService {
        AcademicRepository& repository_;
    public:
        explicit AcademicService(AcademicRepository& repository) : repository_(repository) {}
        std::optional<AcademicProfile> profile(const RequestContext& context);
        AcademicProfile saveProfile(const RequestContext& context, AcademicProfile profile);
        std::vector<CourseRecord> courses(const RequestContext& context);
        CourseRecord curriculum(const RequestContext& context);
        AcademicSummary summary(const RequestContext& context);
        AcademicSummary setCompleted(const RequestContext& context, const std::string& subjectId, bool completed);
        // Ponto puro, usado também nos testes: mesmas classes do domínio, sem Firebase.
        static AcademicSummary evaluate(const CourseRecord&, const AcademicProfile&, const std::vector<std::string>&);
    };
}
