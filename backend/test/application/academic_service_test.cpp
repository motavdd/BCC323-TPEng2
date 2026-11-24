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
        check(repository.history[first.uid].empty(), "falha não grava histórico");

        std::cout << "PASS: perfil, progresso, elegibilidade, isolamento e persistência\n";
        return 0;
    } catch (const std::exception& e) { 
        std::cerr << e.what() << '\n'; 
        return 1; 
    }
}
