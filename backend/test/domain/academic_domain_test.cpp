#include "application/AcademicService.hpp"
#include "domain/DisciplinaImpl.hpp"
#include "../support/memory_academic_repository.hpp"
#include <iostream>
#include <stdexcept>

using namespace bcc323;

namespace {
    void check(bool value, const char* message) {
        if (!value) throw std::runtime_error(message);
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
        test::MemoryRepository repository;
        AcademicService service(repository);
        testarConflitoHorariosETurmas(service);
        testarVinculoPreRequisitos();
        std::cout << "PASS: horários, turmas e pré-requisitos\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
}
