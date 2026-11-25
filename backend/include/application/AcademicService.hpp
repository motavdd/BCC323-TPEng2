#pragma once
#include "application/AcademicRepository.hpp"
#include "domain/Turma.hpp"

namespace bcc323 {
    class AcademicService {
        AcademicRepository& repository_;
        CourseRecord baseCurriculum(const RequestContext&);
        std::string today_;
        CourseRecord resolveCourse(const RequestContext&, CourseRecord, const std::string&);
    public:
        explicit AcademicService(AcademicRepository& repository, std::string today = {})
            : repository_(repository), today_(std::move(today)) {}
        std::optional<AcademicProfile> profile(const RequestContext& context);
        AcademicProfile saveProfile(const RequestContext& context, AcademicProfile profile);
        std::vector<CourseRecord> courses(const RequestContext& context);
        CourseRecord curriculum(const RequestContext& context);
        CourseRecord courseAt(const RequestContext&, const std::string&, const std::string& term = {});
        std::pair<CourseRecord, AcademicSummary> planning(const RequestContext&);
        ScheduledChange scheduleChange(const RequestContext&, const std::string&, ScheduledChange);
        AcademicSummary summary(const RequestContext& context);
        AcademicSummary setCompleted(const RequestContext& context, const std::string& subjectId, bool completed);
        // Ponto puro, usado também nos testes: mesmas classes do domínio, sem Firebase.
        static AcademicSummary evaluate(const CourseRecord&, const AcademicProfile&, const std::vector<std::string>&);

        // Turmas e conflitos de horários
        static bool verificarConflitoHorarios(const HorarioAula& h1, const HorarioAula& h2);
        static bool verificarConflitoTurmas(const Turma& t1, const Turma& t2);
        bool validarMatriculaTurmas(const std::vector<Turma>& turmasExistentes, const Turma& novaTurma) const;
    };
}
