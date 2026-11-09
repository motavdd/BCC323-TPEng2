#include "application/AcademicService.hpp"
#include "domain/DisciplinaImpl.hpp"
#include <cmath>
#include <iostream>
#include <functional>
#include <algorithm>
#include <stdexcept>

using namespace bcc323;

namespace {
    void check(bool value, const char* message) { 
        if (!value) throw std::runtime_error(message); 
    }

    struct MemoryRepository : AcademicRepository {

        std::map<std::string, AcademicProfile> profiles;

        std::map<std::string, std::vector<std::string>> history;

        CourseRecord catalog{"course", "Computação", false, {
            {"A", "Introdução", 60, 1, {}, true},
            {"B", "Estruturas", 90, 2, {"A"}, true},
            {"C", "A validar", 30, 2, {}, false},
        }, {}};

        std::optional<AcademicProfile> profile(const RequestContext& c) override {
            if (!profiles.count(c.uid)) {
                return std::nullopt; 
            }
            return profiles.at(c.uid);
        }

        void saveProfile(const RequestContext& c, const AcademicProfile& p, bool) override { 
            profiles[c.uid] = p; 
        }

        std::vector<CourseRecord> courses(const RequestContext&) override { 
            return {catalog}; 
        }

        CourseRecord course(const RequestContext&, const std::string& id) override {
            if (id != catalog.id) throw AppError(404, "course_not_found", "Curso ausente"); 
            return catalog;
        }

        std::vector<std::string> completed(const RequestContext& c) override { 
            return history[c.uid]; 
        }

        void setCompleted(const RequestContext& c, const std::string& id, bool done) override {
            auto& ids = history[c.uid]; 
            ids.erase(std::remove(ids.begin(), ids.end(), id), ids.end()); 
            if (done) ids.push_back(id);
        }
    };

    void expectError(int status, const std::function<void()>& action) {
        try { 
            action(); 
        } catch (const AppError& e) { 
            check(e.status == status, "status incorreto"); 
            return; 
        }
        throw std::runtime_error("Era esperado erro de aplicação");
    }

    void testarConflitoHorariosETurmas(AcademicService& service) {
        // 1. Dias distintos -> Sem conflito
        HorarioAula seg8_10{DiaSemana::SEGUNDA, 8, 0, 10, 0, "Sala 101"};
        HorarioAula ter8_10{DiaSemana::TERCA, 8, 0, 10, 0, "Sala 101"};
        check(!AcademicService::verificarConflitoHorarios(seg8_10, ter8_10),
              "Aulas em dias distintos nao devem conflitar");

        // 2. Mesmo dia, contíguas (uma termina às 10:00 e a outra começa às 10:00) -> Sem conflito
        HorarioAula seg10_12{DiaSemana::SEGUNDA, 10, 0, 12, 0, "Sala 102"};
        check(!AcademicService::verificarConflitoHorarios(seg8_10, seg10_12),
              "Aulas contiguas nao devem conflitar");

        // 3. Mesmo dia, sobreposição parcial (08:00-10:00 vs 09:00-11:00) -> Conflito detectado
        HorarioAula seg9_11{DiaSemana::SEGUNDA, 9, 0, 11, 0, "Sala 103"};
        check(AcademicService::verificarConflitoHorarios(seg8_10, seg9_11),
              "Sobreposicao parcial de horario deve gerar conflito");

        // 4. Validação de invariante temporal (hora fim <= hora início) -> Dispara std::invalid_argument
        HorarioAula horarioInvalido{DiaSemana::SEXTA, 12, 0, 10, 0, "Lab 1"};
        bool disparouExcecao = false;
        try {
            horarioInvalido.validar();
        } catch (const std::invalid_argument&) {
            disparouExcecao = true;
        }
        check(disparouExcecao, "Horario com termino anterior ao inicio deve disparar excecao");

        // 5. Conflito entre turmas completas
        Turma turmaA{"T01", "BCC323", 40, {seg8_10}};
        Turma turmaB{"T02", "BCC264", 30, {seg9_11}}; // Choque com turmaA na segunda
        Turma turmaC{"T01", "MTM112", 40, {ter8_10}}; // Terça, compatível com turmaA
        check(AcademicService::verificarConflitoTurmas(turmaA, turmaB),
              "Turmas com choque de horario devem ser detectadas");
        check(!AcademicService::verificarConflitoTurmas(turmaA, turmaC),
              "Turmas sem choque de horario devem ser compativeis");

        // 6. Validação de matrícula em grade com múltiplas turmas existentes
        std::vector<Turma> gradeAluno = {turmaA, turmaC};
        check(!service.validarMatriculaTurmas(gradeAluno, turmaB),
              "Matricula com choque de horario na grade deve ser recusada");

        Turma turmaCompativel{"T03", "BCC322", 35, {seg10_12}};
        check(service.validarMatriculaTurmas(gradeAluno, turmaCompativel),
              "Matricula em turma de horario compativel deve ser aceita");
    }

    void testarVinculoPreRequisitos() {
        DisciplinaImpl discA("BCC101", "Programacao I", 60, 1);
        DisciplinaImpl discB("BCC102", "Programacao II", 60, 2);
        DisciplinaImpl discC("BCC103", "Estruturas de Dados", 60, 3);

        // 1. Adição válida de pré-requisito (A é pré-requisito de B)
        discB.addPreRequisito(&discA);
        check(discB.getPreRequisitos().size() == 1, "Deve conter 1 pre-requisito");
        check(discB.getPreRequisitos()[0]->getCodigo() == "BCC101", "O pre-requisito deve ser a disciplina A");

        // 2. Remoção válida de pré-requisito
        discB.removePreRequisito(&discA);
        check(discB.getPreRequisitos().empty(), "A lista de pre-requisitos deve ficar vazia apos a remocao");

        // Monta a cadeia válida: BCC101 -> BCC102 -> BCC103
        discB.addPreRequisito(&discA);
        discC.addPreRequisito(&discB);

        // 3. Impeditivo de Auto-dependência (BCC101 -> BCC101)
        bool autoDepInvalida = false;
        try {
            discA.addPreRequisito(&discA);
        } catch (const std::invalid_argument&) {
            autoDepInvalida = true;
        }
        check(autoDepInvalida, "Adicionar a propria disciplina como pre-requisito deve disparar excecao");

        // 4. Impeditivo de Ciclo Direto (Tentar criar dependência circular BCC102 -> BCC101)
        bool cicloDiretoInvalido = false;
        try {
            discA.addPreRequisito(&discB);
        } catch (const std::invalid_argument&) {
            cicloDiretoInvalido = true;
        }
        check(cicloDiretoInvalido, "Ciclo direto entre duas disciplinas deve ser bloqueado");

        // 5. Impeditivo de Ciclo Transitivo/Indireto (Tentar criar dependência circular BCC103 -> BCC101)
        bool cicloIndiretoInvalido = false;
        try {
            discA.addPreRequisito(&discC);
        } catch (const std::invalid_argument&) {
            cicloIndiretoInvalido = true;
        }
        check(cicloIndiretoInvalido, "Ciclo transitivo/indireto em cadeia deve ser bloqueado");
    }
}

int main() {
    try {
        MemoryRepository repository; 
        AcademicService service(repository);
        RequestContext first{"first", ""}, second{"second", ""};
        
        check(!service.profile(first), "perfil ausente");
        expectError(409, [&] { service.summary(first); });
        expectError(400, [&] { service.saveProfile(first, {"", "Computação", "", 0}); });
        
        service.saveProfile(first, {"Aluno", "Computação", "", 1});
        service.saveProfile(second, {"Outro", "Computação", "", 2});
        
        check(service.profile(first)->courseId == "course", "curso persistente");
        check(service.summary(first).subjects.at("B").state == "blocked", "requisito inicial");
        
        auto result = service.setCompleted(first, "A", true);
        check(result.completedHours == 60 && std::abs(result.progress - 100.0 / 3) < 0.001, "progresso do domínio");
        check(result.subjects.at("B").state == "available", "requisito liberado");
        check(result.subjects.at("C").state == "unknown", "requisito desconhecido");
        check(service.summary(second).completedCount == 0, "isolamento por UID");
        check(service.setCompleted(first, "A", true).completedCount == 1, "PUT idempotente");
        
        service.setCompleted(first, "B", true);
        result = service.setCompleted(first, "A", false);
        check(result.completedCount == 1 && result.subjects.at("B").state == "completed", "preservar demais conclusões");
        
        result = service.setCompleted(first, "B", false);
        check(result.completedHours == 0 && result.subjects.at("B").state == "blocked", "remoção atualiza elegibilidade");
        
        expectError(404, [&] { service.setCompleted(first, "missing", true); });
        repository.catalog.subjects[0].prerequisites = {"B"};
        expectError(503, [&] { service.summary(first); });
        check(repository.history[first.uid].empty(), "falha não grava histórico");

        // Execução da suíte de turmas e conflitos
        testarConflitoHorariosETurmas(service);

        // Execução da suíte de vinculo e deteção de ciclos de pré-requisitos (Issue 2.2)
        testarVinculoPreRequisitos();

        std::cout << "PASS: perfil, domínio, progresso, elegibilidade, isolamento, persistência, turmas e pré-requisitos\n";
        return 0;
    } catch (const std::exception& e) { 
        std::cerr << e.what() << '\n'; 
        return 1; 
    }
}