#include "application/AcademicService.hpp"
#include "domain/AlunoImpl.hpp"
#include "domain/CursoImpl.hpp"
#include "domain/DisciplinaImpl.hpp"
#include "domain/Turma.hpp"
#include <algorithm>
#include <memory>
#include <set>

namespace bcc323 {
    namespace {
        AcademicSummary evaluateHistory(const CourseRecord& catalog, const CourseRecord& base,
                                        const AcademicProfile& profile, std::map<std::string, std::string> names) {
            std::vector<std::string> ids;
            for (auto& [id, name] : names) {
                ids.push_back(id);
                if (!name.empty()) continue;
                const auto original = std::find_if(base.subjects.begin(), base.subjects.end(),
                    [&](const auto& item) { return item.id == id; });
                name = original == base.subjects.end() ? "Disciplina sem nome registrado" : original->name;
            }
            auto result = AcademicService::evaluate(catalog, profile, ids);
            result.completedNames = std::move(names);
            return result;
        }
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

    CourseRecord AcademicService::baseCurriculum(const RequestContext& context) {
        const auto value = repository_.profile(context);
        if (!value) throw AppError(409, "profile_required", "Complete seu perfil acadêmico para continuar.");
        if (!value->courseId.empty()) return repository_.course(context, value->courseId);
        // Compatibilidade com perfis anteriores, que persistiam somente o nome.
        for (const auto& c : repository_.courses(context)) if (c.name == value->course) return c;
        throw AppError(404, "course_not_found", "A matriz do seu curso ainda não foi cadastrada.");
    }

    CourseRecord AcademicService::curriculum(const RequestContext& context) {
        return resolveCourse(context, baseCurriculum(context), {});
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
            if (state.state != "completed" && !item.requirementsKnown){
                state.state = "unknown";
            } 
            for (auto* pre : d->getPreRequisitos()){
                if (student.getEstado(pre) != EstadoDisciplina::Concluida){
                    state.missing.push_back(pre->getCodigo());
                } 
            }

            if (item.requirementsKnown && std::find(available.begin(), available.end(), d) != available.end()){
                ++result.availableCount;
            } 
        }
        return result;
    }

    AcademicSummary AcademicService::summary(const RequestContext& context) {
        const auto base = baseCurriculum(context);
        const auto catalog = resolveCourse(context, base, {});
        const auto p = repository_.profile(context);
        if (!p){
            throw AppError(409, "profile_required", "Complete seu perfil acadêmico.");
        } 
        return evaluateHistory(catalog, base, *p, repository_.completedNames(context));
    }

    std::pair<CourseRecord, AcademicSummary> AcademicService::planning(const RequestContext& context) {
        const auto base = baseCurriculum(context);
        auto catalog = resolveCourse(context, base, "next");
        const auto profile = repository_.profile(context);
        if (!profile) throw AppError(409, "profile_required", "Complete seu perfil acadêmico para continuar.");
        auto summary = evaluateHistory(catalog, base, *profile, repository_.completedNames(context));
        return {std::move(catalog), std::move(summary)};
    }

    AcademicSummary AcademicService::setCompleted(const RequestContext& context, const std::string& subjectId, bool done) {
        
        const auto base = baseCurriculum(context);
        const auto catalog = resolveCourse(context, base, {});
        const auto subject = std::find_if(catalog.subjects.begin(), catalog.subjects.end(),
            [&](const auto& item) { return item.id == subjectId; });
        if (subject == catalog.subjects.end()){
            throw AppError(404, "subject_not_found", "Disciplina fora da matriz do seu curso.");

        }
        // Valide o grafo antes de persistir; concluir não exige pré-requisito cumprido:
        // o histórico real pode conter equivalências. As demais conclusões são mantidas.
        const auto p = repository_.profile(context);
        if (!p){
            throw AppError(409, "profile_required", "Complete seu perfil acadêmico.");
        } 
        auto history = repository_.completedNames(context);
        if (done) {
            history.try_emplace(subjectId, subject->name);
        } else {
            history.erase(subjectId);
        }
        auto result = evaluateHistory(catalog, base, *p, std::move(history));
        repository_.setCompleted(context, subjectId, done, subject->name);
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
