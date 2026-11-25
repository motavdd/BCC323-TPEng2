#include "application/AcademicService.hpp"
#include "../support/memory_academic_repository.hpp"
#include <cmath>
#include <iostream>
#include <functional>
#include <stdexcept>

using namespace bcc323;

namespace {
    void check(bool value, const char* message) { 
        if (!value) throw std::runtime_error(message); 
    }

    void expectError(int status, const std::function<void()>& action) {
        try { 
            action(); 
        } catch (const AppError& e) { 
            check(e.status == status, "status incorreto"); 
            return; 
        }
        throw std::runtime_error("Era esperado erro de aplicação");
    }

    void testarHistoricoAcademico() {
        test::MemoryRepository repository;
        AcademicService service(repository);

        RequestContext aluno{"historico", ""};

        service.saveProfile(aluno, {"Aluno", "Computação", "", 1});

        // 1. Registro com pre-requisito pendente
        auto result = service.setCompleted(aluno, "B", true);

        check(result.completedCount == 1,
            "Historico deve conter uma disciplina concluida");

        check(result.subjects.at("B").state == "completed",
            "Registro deve permitir conclusao por equivalencia");

        check(result.subjects.at("A").state == "available",
            "Pre-requisito nao deve ser concluido automaticamente");

        check(result.completedHours == 90,
            "Carga horaria deve considerar apenas a disciplina concluida");

        // 2. Consultar novamente o historico
        result = service.summary(aluno);

        check(result.completedCount == 1 &&
            result.subjects.at("B").state == "completed",
            "Conclusao deve ser preservada em nova consulta");

        // 3. Remocao de disciplina ausente do historico
        result = service.setCompleted(aluno, "A", false);

        check(result.completedCount == 1 &&
            result.subjects.at("B").state == "completed",
            "Remover disciplina ausente deve preservar o historico");

        // 4. Registro de disciplina inexistente
        const auto historicoAnterior = repository.completed(aluno);

        expectError(404, [&] {
            service.setCompleted(aluno, "inexistente", true);
        });

        check(repository.completed(aluno) == historicoAnterior,
            "Registro invalido nao deve modificar o historico");

        // 5. Remocao da disciplina concluida
        result = service.setCompleted(aluno, "B", false);

        check(result.completedCount == 0,
            "Historico deve ficar vazio apos a remocao");

        check(result.completedHours == 0,
            "Carga horaria concluida deve voltar a zero");

        check(result.subjects.at("B").state == "blocked",
            "Disciplina deve voltar a ficar bloqueada");
    }

    void checkScheduledChanges() {
        test::MemoryRepository repository;
        repository.terms = {
            {"2026-1", "2026-03-01", "2026-07-15"},
            {"2026-2", "2026-08-10", "2026-12-15"},
        };
        repository.catalog.offerings = {{"A-01", "A", "01", {{"Seg", "08:00", "10:00", "101"}}, "Docente atual"}};
        const RequestContext coordinator{"coordinator", ""};
        const RequestContext student{"student", ""};
        AcademicService april(repository, "2026-04-01");
        april.saveProfile(student, {"Aluno", "Computação", "", 1});
        april.setCompleted(student, "A", true);
        repository.history["legacy"] = {"A"};
        const RequestContext legacy{"legacy", ""};
        april.saveProfile(legacy, {"Aluno", "Computação", "", 1});

        const ScheduledChange replacement{"2026-1", "2026-04-15", std::nullopt,
            OfferingRecord{"A-01", "", "", {{"Qua", "09:30", "11:00", "202"}}, "Docente substituto"}};
        expectError(403, [&] { april.scheduleChange(student, "course", replacement); });
        expectError(403, [&] { april.scheduleChange(coordinator, "other-course", replacement); });
        auto past = replacement;
        past.effectiveDate = "2026-03-31";
        expectError(400, [&] { april.scheduleChange(coordinator, "course", past); });
        auto invalidDay = replacement;
        invalidDay.effectiveDate = "2026-04-31";
        expectError(400, [&] { april.scheduleChange(coordinator, "course", invalidDay); });
        april.scheduleChange(coordinator, "course", replacement);

        auto invalidMatrix = repository.catalog.subjects.front();
        invalidMatrix.prerequisites = {"B"};
        expectError(400, [&] { april.scheduleChange(coordinator, "course", {"2026-2", "", invalidMatrix, {}}); });
        auto renamed = repository.catalog.subjects.front();
        renamed.name = "Fundamentos";
        expectError(400, [&] { april.scheduleChange(coordinator, "course", {"2026-1", "", renamed, {}}); });
        check(april.scheduleChange(coordinator, "course", {"2026-2", "", renamed, {}}).effectiveDate == "2026-08-10",
              "mudança da matriz vigora no início do semestre");

        check(april.curriculum(student).offerings.front().professor == "Docente atual", "troca não antecipada");
        AcademicService afterLeave(repository, "2026-04-15");
        const auto offering = afterLeave.curriculum(student).offerings.front();
        check(offering.professor == "Docente substituto" && offering.times.front().room == "202"
              && offering.times.front().start == "09:30", "troca vigora na data agendada");
        auto immediate = replacement;
        immediate.effectiveDate.clear();
        immediate.offering->professor = "Docente imediato";
        immediate.offering->times.clear();
        check(afterLeave.scheduleChange(coordinator, "course", immediate).effectiveDate == "2026-04-15",
              "troca sem data vigora hoje");
        check(afterLeave.curriculum(student).offerings.front().times.front().room == "202",
              "troca apenas de docente preserva a sala agendada");
        check(afterLeave.curriculum(student).subjects.front().name == "Introdução", "matriz vigente preservada");
        check(afterLeave.planning(student).first.subjects.front().name == "Fundamentos",
              "planejamento usa a matriz do próximo semestre");

        AcademicService august(repository, "2026-08-10");
        check(august.curriculum(student).subjects.front().name == "Fundamentos", "novo semestre usa a matriz programada");
        check(august.summary(student).completedNames.at("A") == "Introdução", "nome do histórico preservado");
        check(august.setCompleted(student, "A", true).completedNames.at("A") == "Introdução",
              "repetir registro não troca o nome histórico");
        check(august.summary(legacy).completedNames.at("A") == "Introdução", "histórico legado usa a matriz original");
        check(august.summary(student).subjects.at("B").state == "available",
              "ID estável preserva validação de pré-requisito");
    }
}

int main() {
    try {
        test::MemoryRepository repository;
        AcademicService service(repository);
        RequestContext first{"first", ""}, second{"second", ""};
        
        check(!service.profile(first), "perfil ausente");
        expectError(409, [&] { service.summary(first); });
        expectError(400, [&] { service.saveProfile(first, {"", "Computação", "", 0}); });
        expectError(400, [&] { service.saveProfile(first, {"Aluno", "Computação", "", 21}); });
        expectError(404, [&] { service.saveProfile(first, {"Aluno", "Curso inexistente", "", 1}); });
        check(!service.profile(first), "cadastro inválido não persiste perfil");
        
        service.saveProfile(first, {"Aluno", "Computação", "", 1});
        service.saveProfile(second, {"Outro", "Computação", "", 2});
        
        check(service.profile(first)->courseId == "course", "curso persistente");
        const auto updated = service.saveProfile(first, {"", "Computação", "course", 20});
        check(updated.period == 20 && updated.displayName == "Aluno",
              "edição aceita o último período e preserva o nome");
        check(service.summary(first).subjects.at("B").state == "blocked", "requisito inicial");
        
        auto result = service.setCompleted(first, "A", true);
        check(result.completedHours == 60 && std::abs(result.progress - 100.0 / 3) < 0.001, "progresso do domínio");
        check(result.subjects.at("B").state == "available", "requisito liberado");
        check(result.subjects.at("C").state == "unknown", "requisito desconhecido");
        check(service.summary(second).completedCount == 0, "isolamento por UID");
        check(service.setCompleted(first, "A", true).completedCount == 1, "PUT idempotente");
        
        repository.failHistoryWrite = true;
        expectError(503, [&] { service.setCompleted(first, "B", true); });
        check(service.summary(first).completed == std::vector<std::string>{"A"},
              "gravação rejeitada preserva o histórico anterior");
        repository.failHistoryWrite = false;
        check(service.setCompleted(first, "B", true).completedCount == 2,
              "nova tentativa conclui a disciplina");
        check(service.setCompleted(first, "B", true).completedCount == 2,
              "repetição após recuperação não duplica a conclusão");
        result = service.setCompleted(first, "A", false);
        check(result.completedCount == 1 && result.subjects.at("B").state == "completed", "preservar demais conclusões");
        
        result = service.setCompleted(first, "B", false);
        check(result.completedHours == 0 && result.subjects.at("B").state == "blocked", "remoção atualiza elegibilidade");
        
        expectError(404, [&] { service.setCompleted(first, "missing", true); });
        repository.catalog.subjects[0].prerequisites = {"B"};
        expectError(503, [&] { service.summary(first); });
        checkScheduledChanges();
        testarHistoricoAcademico();
        check(repository.history[first.uid].empty(), "falha não grava histórico");

        std::cout << "PASS: perfil, progresso, elegibilidade, isolamento e persistência\n";
        return 0;
    } catch (const std::exception& e) { 
        std::cerr << e.what() << '\n'; 
        return 1; 
    }
}
