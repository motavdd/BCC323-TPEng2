#include "application/AcademicService.hpp"
#include "domain/AlunoImpl.hpp"
#include "domain/CursoImpl.hpp"
#include "domain/DisciplinaImpl.hpp"
#include "domain/Turma.hpp"
#include <algorithm>
#include <memory>
#include <regex>
#include <set>

namespace bcc323 {
    namespace {
        bool validId(const std::string& id) {
            static const std::regex pattern("[A-Za-z0-9_-]{1,100}");
            return std::regex_match(id, pattern);
        }

        bool validLabel(const std::string& value, std::size_t limit) {
            return value.size() <= limit && value.find_first_not_of(" \t\r\n") != std::string::npos
                && std::none_of(value.begin(), value.end(), [](unsigned char c) { return c < 32 || c == 127; });
        }

        void requireRevision(const CourseRecord& catalog, const std::string& revision) {
            if (revision.empty() || revision != catalog.revision) {
                throw AppError(409, "write_conflict", "A matriz foi alterada. Recarregue antes de salvar.");
            }
        }

        int clockMinutes(const std::string& time) {
            static const std::regex pattern("([01][0-9]|2[0-3]):[0-5][0-9]");
            if (!std::regex_match(time, pattern)) {
                throw AppError(400, "invalid_offering", "Informe horários no formato HH:MM.");
            }
            return std::stoi(time.substr(0, 2)) * 60 + std::stoi(time.substr(3));
        }
    }

    std::optional<std::string> AcademicService::coordinatorCourse(const RequestContext& context) {
        return context.emailVerified ? repository_.coordinatorCourse(context) : std::nullopt;
    }

    CourseRecord AcademicService::coordinatorCatalog(const RequestContext& context) {
        const auto id = coordinatorCourse(context);
        if (!id) {
            throw AppError(403, "coordinator_required", "Acesso restrito à coordenação do curso.");
        }
        return repository_.course(context, *id);
    }

    CourseRecord AcademicService::changeSubject(const RequestContext& context, const std::string& revision,
                                                const std::string& id, const std::optional<SubjectRecord>& value) {
        auto catalog = coordinatorCatalog(context);
        requireRevision(catalog, revision);
        if (!validId(id)) {
            throw AppError(400, "invalid_identifier", "Código da disciplina inválido.");
        }
        const auto subject = std::find_if(catalog.subjects.begin(), catalog.subjects.end(),
            [&](const auto& item) { return item.id == id; });
        if (value) {
            if (subject != catalog.subjects.end() && subject->archived) {
                throw AppError(409, "archived_subject", "O código pertence a uma disciplina arquivada.");
            }
            if (!validLabel(value->name, 160) || value->hours < 1 || value->hours > 10000
                || value->period < 1 || value->period > 20) {
                throw AppError(400, "invalid_subject", "Informe nome, carga horária e período válidos.");
            }
            std::set<std::string> prerequisites;
            for (const auto& prerequisite : value->prerequisites) {
                const auto active = std::any_of(catalog.subjects.begin(), catalog.subjects.end(),
                    [&](const auto& item) { return item.id == prerequisite && !item.archived; });
                if (!active || prerequisite == id || !prerequisites.insert(prerequisite).second) {
                    throw AppError(400, "invalid_prerequisites", "Use pré-requisitos ativos, distintos e da mesma matriz.");
                }
            }
            if (subject == catalog.subjects.end()) {
                if (catalog.subjects.size() >= 500) {
                    throw AppError(409, "catalog_limit", "A matriz atingiu o limite de disciplinas.");
                }
                catalog.subjects.push_back(*value);
            } else {
                *subject = *value;
            }
            try {
                evaluate(catalog, {}, {});
            } catch (const AppError&) {
                throw AppError(400, "invalid_prerequisites", "Revise os pré-requisitos. A matriz não pode ter ciclos.");
            }
        } else {
            if (subject == catalog.subjects.end() || subject->archived) {
                throw AppError(404, "subject_not_found", "Disciplina ativa não encontrada.");
            }
            const auto linked = std::any_of(catalog.subjects.begin(), catalog.subjects.end(), [&](const auto& item) {
                return !item.archived && std::find(item.prerequisites.begin(), item.prerequisites.end(), id) != item.prerequisites.end();
            });
            if (linked) {
                throw AppError(409, "subject_in_use", "Remova os vínculos de pré-requisito antes de arquivar.");
            }
            // Preserve o código e a carga horária para os históricos existentes.
            subject->archived = true;
            catalog.offerings.erase(std::remove_if(catalog.offerings.begin(), catalog.offerings.end(),
                [&](const auto& item) { return item.subjectId == id; }), catalog.offerings.end());
        }
        return repository_.saveCatalog(catalog);
    }

    CourseRecord AcademicService::changeOffering(const RequestContext& context, const std::string& revision,
                                                 const std::string& id, const std::optional<OfferingRecord>& value) {
        auto catalog = coordinatorCatalog(context);
        requireRevision(catalog, revision);
        if (!validId(id)) {
            throw AppError(400, "invalid_identifier", "Código da turma inválido.");
        }
        const auto offering = std::find_if(catalog.offerings.begin(), catalog.offerings.end(),
            [&](const auto& item) { return item.id == id; });
        if (value) {
            const auto active = std::any_of(catalog.subjects.begin(), catalog.subjects.end(),
                [&](const auto& subject) { return subject.id == value->subjectId && !subject.archived; });
            if (!active || !validLabel(value->group, 40) || value->capacity < 0 || value->capacity > 10000
                || value->times.empty() || value->times.size() > 20) {
                throw AppError(400, "invalid_offering", "Informe disciplina ativa, turma, vagas e horários válidos.");
            }
            if (offering != catalog.offerings.end() && offering->subjectId != value->subjectId) {
                throw AppError(409, "offering_subject_changed", "O código da turma já pertence a outra disciplina.");
            }
            const auto duplicate = std::any_of(catalog.offerings.begin(), catalog.offerings.end(), [&](const auto& item) {
                return item.id != id && item.subjectId == value->subjectId && item.group == value->group;
            });
            if (duplicate) {
                throw AppError(409, "duplicate_group", "Já existe uma turma com esse nome na disciplina.");
            }
            const std::vector<std::string> days{"Seg", "Ter", "Qua", "Qui", "Sex", "Sáb"};
            std::vector<HorarioAula> times;
            for (const auto& slot : value->times) {
                const auto day = std::find(days.begin(), days.end(), slot.day);
                const auto start = clockMinutes(slot.start);
                const auto end = clockMinutes(slot.end);
                if (day == days.end() || start >= end || !validLabel(slot.room, 100)) {
                    throw AppError(400, "invalid_offering", "Informe dia, sala e horário de término posterior ao início.");
                }
                HorarioAula time{static_cast<DiaSemana>(day - days.begin()), start / 60, start % 60, end / 60, end % 60, slot.room};
                if (std::any_of(times.begin(), times.end(), [&](const auto& other) { return verificarConflitoHorarios(other, time); })) {
                    throw AppError(400, "overlapping_offering_times", "Os horários da turma se sobrepõem.");
                }
                times.push_back(std::move(time));
            }
            if (offering == catalog.offerings.end()) {
                if (catalog.offerings.size() >= 500) {
                    throw AppError(409, "catalog_limit", "O catálogo atingiu o limite de turmas.");
                }
                catalog.offerings.push_back(*value);
            } else {
                *offering = *value;
            }
        } else {
            if (offering == catalog.offerings.end()) {
                throw AppError(404, "offering_not_found", "Turma não encontrada.");
            }
            catalog.offerings.erase(offering);
        }
        return repository_.saveCatalog(catalog);
    }

    std::optional<AcademicProfile> AcademicService::profile(const RequestContext& context) { 
        return repository_.profile(context); 
    }
    std::vector<CourseRecord> AcademicService::courses(const RequestContext& context) { 
        return repository_.courses(context); 
    }

    AcademicProfile AcademicService::saveProfile(const RequestContext& context, AcademicProfile value) {
        if (value.course.empty() || value.course.size() > 100 || value.displayName.size() > 100 || value.period < 1 || value.period > 20){
            throw AppError(400, "invalid_profile", "Informe curso e período entre 1 e 20.");

        }

        const auto all = repository_.courses(context);
        const auto found = std::find_if(all.begin(), all.end(), [&](const auto& c) {
            return value.courseId.empty() ? c.name == value.course : c.id == value.courseId;
        });
        
        if (found == all.end()) {
            throw AppError(404, "course_not_found", "Curso ainda não cadastrado.");
        }

        value.courseId = found->id;
        value.course = found->name;

        const auto previous = repository_.profile(context);

        if (previous) {
            if (previous->course != value.course && !repository_.completed(context).empty()){
                throw AppError(409, "history_requires_migration", "Seu histórico precisa ser revisado antes de mudar de curso.");

            }
            // A tela atual edita apenas curso e período; preserve o nome existente.
            if (value.displayName.empty()) value.displayName = previous->displayName;
        }
        repository_.saveProfile(context, value, !previous.has_value());
        return value;
    }

    CourseRecord AcademicService::curriculum(const RequestContext& context) {
        const auto value = repository_.profile(context);
        if (!value) throw AppError(409, "profile_required", "Complete seu perfil acadêmico para continuar.");
        if (!value->courseId.empty()) return repository_.course(context, value->courseId);
        // Compatibilidade com perfis anteriores, que persistiam somente o nome.
        for (const auto& c : repository_.courses(context)) if (c.name == value->course) return c;
        throw AppError(404, "course_not_found", "A matriz do seu curso ainda não foi cadastrada.");
    }

    AcademicSummary AcademicService::evaluate(const CourseRecord& catalog, const AcademicProfile& profile, const std::vector<std::string>& completed) {
        
        if (catalog.subjects.empty() || catalog.subjects.size() > 500){
            throw AppError(503, "invalid_curriculum", "A matriz cadastrada precisa de revisão.");

        }

        // Propriedade local por requisição: as referências do domínio nunca escapam.
        // Destruição inversa: aluno -> curso -> disciplinas. Sem AcademicSystem global.
        std::vector<std::unique_ptr<DisciplinaImpl>> owned;
        std::map<std::string, DisciplinaImpl*> subjects;

        CursoImpl course(catalog.name);
        AlunoImpl student;

        student.setNome(profile.displayName);
        student.setCurso(&course);
        student.setPeriodoAtual(profile.period);

        for (const auto& item : catalog.subjects) {
            if (item.id.empty() || item.hours < 0 || item.hours > 10000 || subjects.count(item.id)){
                throw AppError(503, "invalid_curriculum", "A matriz cadastrada precisa de revisão.");

            }

            owned.push_back(std::make_unique<DisciplinaImpl>(item.id, item.name, item.hours));
            subjects[item.id] = owned.back().get();
            course.add(owned.back().get());
        }

        try {

            for (const auto& item : catalog.subjects) for (const auto& pre : item.prerequisites) {
                if (!subjects.count(pre)){
                  throw std::invalid_argument("Requisito ausente");  
                } 
                subjects.at(item.id)->addPreRequisito(subjects.at(pre));
            }

        } catch (const std::invalid_argument&) {
            throw AppError(503, "invalid_curriculum", "Há pré-requisitos inválidos na matriz cadastrada.");
        }

        AcademicSummary result;
        result.courseId = catalog.id; result.illustrative = catalog.illustrative;

        for (const auto& id : std::set<std::string>(completed.begin(), completed.end())) {
            if (subjects.count(id)){
                student.marcarConcluida(subjects.at(id));
            } else{
                result.unmappedCompleted.push_back(id);
            } 
        }

        result.totalHours = course.getCargaTotal();
        result.progress = student.getProgresso();
        result.completedCount = static_cast<int>(student.getConcluidas().size());

        for (const auto* d : student.getConcluidas()) {
            result.completed.push_back(d->getCodigo()); result.completedHours += d->getCargaHoraria();
        }

        const auto available = student.getDisponiveis();

        for (const auto& item : catalog.subjects) {
            const auto* d = subjects.at(item.id);
            auto& state = result.subjects[item.id];
            switch (student.getEstado(subjects.at(item.id))) {
                case EstadoDisciplina::Concluida: state.state = "completed"; break;
                case EstadoDisciplina::EmAndamento: state.state = "in_progress"; break;
                case EstadoDisciplina::Disponivel: state.state = "available"; break;
                case EstadoDisciplina::Bloqueada: state.state = "blocked"; break;
            }

            // Requisito desconhecido não equivale à ausência de pré-requisito.
            if (state.state != "completed" && item.archived) {
                state.state = "archived";
            } else if (state.state != "completed" && !item.requirementsKnown){
                state.state = "unknown";
            } 
            for (auto* pre : d->getPreRequisitos()){
                if (student.getEstado(pre) != EstadoDisciplina::Concluida){
                    state.missing.push_back(pre->getCodigo());
                } 
            }

            if (!item.archived && item.requirementsKnown && std::find(available.begin(), available.end(), d) != available.end()){
                ++result.availableCount;
            } 
        }
        return result;
    }

    AcademicSummary AcademicService::summary(const RequestContext& context) {
        const auto catalog = curriculum(context);
        const auto p = repository_.profile(context);
        if (!p){
            throw AppError(409, "profile_required", "Complete seu perfil acadêmico.");
        } 
        return evaluate(catalog, *p, repository_.completed(context));
    }

    AcademicSummary AcademicService::setCompleted(const RequestContext& context, const std::string& subjectId, bool done) {
        
        const auto catalog = curriculum(context);
        if (std::none_of(catalog.subjects.begin(), catalog.subjects.end(), [&](const auto& s) {
            return s.id == subjectId && (!done || !s.archived);
        })){
            throw AppError(404, "subject_not_found", "Disciplina fora da matriz do seu curso.");

        }
        // Valide o grafo antes de persistir; concluir não exige pré-requisito cumprido:
        // o histórico real pode conter equivalências. As demais conclusões são mantidas.
        const auto p = repository_.profile(context);
        if (!p){
            throw AppError(409, "profile_required", "Complete seu perfil acadêmico.");
        } 
        auto history = repository_.completed(context);
        history.erase(std::remove(history.begin(), history.end(), subjectId), history.end());
        if (done){
            history.push_back(subjectId);
        }
        auto result = evaluate(catalog, *p, history);
        repository_.setCompleted(context, subjectId, done);
        return result;
    }
    bool AcademicService::verificarConflitoHorarios(const HorarioAula& h1, const HorarioAula& h2) {
        h1.validar();
        h2.validar();

        if (h1.dia != h2.dia) {
            return false;
        }

        // Teorema da sobreposicao de intervalos abertos:
        // A_ini < B_fim E B_ini < A_fim
        return (h1.inicioEmMinutos() < h2.fimEmMinutos()) &&
               (h2.inicioEmMinutos() < h1.fimEmMinutos());
    }

    bool AcademicService::verificarConflitoTurmas(const Turma& t1, const Turma& t2) {
        t1.validar();
        t2.validar();

        for (const auto& slot1 : t1.horarios) {
            for (const auto& slot2 : t2.horarios) {
                if (verificarConflitoHorarios(slot1, slot2)) {
                    return true;
                }
            }
        }
        return false;
    }

    bool AcademicService::validarMatriculaTurmas(
        const std::vector<Turma>& turmasExistentes,
        const Turma& novaTurma
    ) const {
        novaTurma.validar();
        for (const auto& turmaCadastrada : turmasExistentes) {
            if (verificarConflitoTurmas(turmaCadastrada, novaTurma)) {
                return false;
            }
        }
        return true;
    }

}
