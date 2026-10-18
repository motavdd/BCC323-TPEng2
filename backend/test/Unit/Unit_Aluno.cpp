#include "Unit_Aluno.hpp"
#include "Unit_ClasseMock.hpp"
#include "../../include/AlunoImpl.hpp"
#include <cassert>
#include <cmath>
#include <string>
#include <vector>

namespace {
    /*!
     * @brief Matriz curricular usada nos testes (carga total de 180 h):
     * Calculo I (60 h), Calculo II (60 h, pre-requisito Calculo I) e Programacao I (60 h).
     */
    struct Matriz {
        DisciplinaMock calc1;
        DisciplinaMock calc2;
        DisciplinaMock prog1;
        CursoMock curso;

        Matriz()
            : calc1("MAT101", "Calculo I", 60, 1),
              calc2("MAT102", "Calculo II", 60, 2),
              prog1("BCC101", "Programacao I", 60, 1),
              curso("Ciencia da Computacao") {
            calc2.addPreRequisito(&calc1);
            curso.add(&calc1);
            curso.add(&calc2);
            curso.add(&prog1);
        }
    };
}

bool Unit_Aluno::defaultConstructor(void) {
    AlunoImpl a1;
    assert(a1.curso == nullptr);
    assert(a1.periodoAtual == 1);
    assert(a1.concluidas.empty());
    assert(a1.emAndamento.empty());
    //atributos herdados de UsuarioImpl
    assert(a1.id == 0);
    assert(a1.nome == "");
    assert(a1.email == "");
    return true;
}

bool Unit_Aluno::constructor(void) {
    Matriz m;
    AlunoImpl a2(7, "User", "user@ufop.edu.br", "1234", &m.curso, 3);

    assert(a2.id == 7);
    assert(a2.nome == "User");
    assert(a2.email == "ana@ufop.edu.br");
    assert(a2.curso == &m.curso);
    assert(a2.periodoAtual == 3);
    assert(a2.concluidas.empty());
    assert(a2.emAndamento.empty());
    assert(a2.autenticar("1234"));
    return true;
}

bool Unit_Aluno::destructor(void) {
    AlunoImpl* a = new AlunoImpl();
    delete a;

    //destruicao via ponteiro da interface (destrutor virtual)
    Usuario* u = new AlunoImpl();
    delete u;
    return true;
}

bool Unit_Aluno::getCurso(void) {
    Matriz m;
    AlunoImpl a;
    a.curso = &m.curso;
    assert(a.getCurso() == &m.curso);
    return true;
}

bool Unit_Aluno::setCurso(void) {
    Matriz m;
    AlunoImpl a;
    a.setCurso(&m.curso);
    assert(a.curso == &m.curso);

    a.setCurso(nullptr);
    assert(a.curso == nullptr);
    return true;
}

bool Unit_Aluno::getPeriodoAtual(void) {
    AlunoImpl a;
    a.periodoAtual = 5;
    assert(a.getPeriodoAtual() == 5);
    return true;
}

bool Unit_Aluno::setPeriodoAtual(void) {
    AlunoImpl a;
    a.setPeriodoAtual(4);
    assert(a.periodoAtual == 4);
    return true;
}

bool Unit_Aluno::contem(void) {
    Matriz m;
    std::vector<Disciplina*> v;

    assert(!AlunoImpl::contem(v, &m.calc1)); //vetor vazio

    v.push_back(&m.calc1);
    assert(AlunoImpl::contem(v, &m.calc1));
    assert(!AlunoImpl::contem(v, &m.calc2)); //outra disciplina
    return true;
}

bool Unit_Aluno::preRequisitosCumpridos(void) {
    Matriz m;
    AlunoImpl a(1, "User", "user@ufop.edu.br", "1234", &m.curso, 1);

    //sem pre-requisitos: sempre cumpridos
    assert(a.preRequisitosCumpridos(&m.calc1));
    //pre-requisito pendente
    assert(!a.preRequisitosCumpridos(&m.calc2));
    //em andamento ainda nao conta como cumprido
    a.emAndamento.push_back(&m.calc1);
    assert(!a.preRequisitosCumpridos(&m.calc2));
    //pre-requisito concluido
    a.concluidas.push_back(&m.calc1);
    assert(a.preRequisitosCumpridos(&m.calc2));
    return true;
}

bool Unit_Aluno::marcarConcluida(void) {
    Matriz m;
    AlunoImpl a(1, "User", "user@ufop.edu.br", "1234", &m.curso, 1);

    a.marcarConcluida(&m.calc1);
    assert(a.concluidas.size() == 1);
    assert(a.concluidas[0] == &m.calc1);

    //duplicata e ignorada
    a.marcarConcluida(&m.calc1);
    assert(a.concluidas.size() == 1);

    //ponteiro nulo e ignorado
    a.marcarConcluida(nullptr);
    assert(a.concluidas.size() == 1);

    //ao concluir, sai de "em andamento"
    a.marcarEmAndamento(&m.prog1);
    assert(a.emAndamento.size() == 1);
    a.marcarConcluida(&m.prog1);
    assert(a.emAndamento.empty());
    assert(a.concluidas.size() == 2);
    return true;
}

bool Unit_Aluno::desmarcarConcluida(void) {
    Matriz m;
    AlunoImpl a(1, "User", "user@ufop.edu.br", "1234", &m.curso, 1);
    a.concluidas.push_back(&m.calc1);
    a.concluidas.push_back(&m.prog1);

    a.desmarcarConcluida(&m.calc1);
    assert(a.concluidas.size() == 1);
    assert(a.concluidas[0] == &m.prog1);

    //disciplina que nao esta no vetor nao altera nada
    a.desmarcarConcluida(&m.calc2);
    assert(a.concluidas.size() == 1);
    return true;
}

bool Unit_Aluno::marcarEmAndamento(void) {
    Matriz m;
    AlunoImpl a(1, "User", "user@ufop.edu.br", "1234", &m.curso, 1);

    a.marcarEmAndamento(&m.prog1);
    assert(a.emAndamento.size() == 1);
    assert(a.emAndamento[0] == &m.prog1);

    //duplicata e ignorada
    a.marcarEmAndamento(&m.prog1);
    assert(a.emAndamento.size() == 1);

    //ponteiro nulo e ignorado
    a.marcarEmAndamento(nullptr);
    assert(a.emAndamento.size() == 1);

    //disciplina ja concluida nao pode ficar em andamento
    a.marcarConcluida(&m.calc1);
    a.marcarEmAndamento(&m.calc1);
    assert(a.emAndamento.size() == 1);
    return true;
}

bool Unit_Aluno::desmarcarEmAndamento(void) {
    Matriz m;
    AlunoImpl a(1, "User", "user@ufop.edu.br", "1234", &m.curso, 1);
    a.emAndamento.push_back(&m.calc1);
    a.emAndamento.push_back(&m.prog1);

    a.desmarcarEmAndamento(&m.calc1);
    assert(a.emAndamento.size() == 1);
    assert(a.emAndamento[0] == &m.prog1);

    //disciplina que nao esta no vetor nao altera nada
    a.desmarcarEmAndamento(&m.calc2);
    assert(a.emAndamento.size() == 1);
    return true;
}

bool Unit_Aluno::getConcluidas(void) {
    Matriz m;
    AlunoImpl a(1, "User", "user@ufop.edu.br", "1234", &m.curso, 1);
    assert(a.getConcluidas().empty());

    a.concluidas.push_back(&m.calc1);
    a.concluidas.push_back(&m.prog1);
    std::vector<Disciplina*> esperado = {&m.calc1, &m.prog1};
    assert(a.getConcluidas() == esperado);

    //o retorno e uma copia: alterar o resultado nao altera o aluno
    std::vector<Disciplina*> copia = a.getConcluidas();
    copia.clear();
    assert(a.concluidas.size() == 2);
    return true;
}

bool Unit_Aluno::getEmAndamento(void) {
    Matriz m;
    AlunoImpl a(1, "User", "user@ufop.edu.br", "1234", &m.curso, 1);
    assert(a.getEmAndamento().empty());

    a.emAndamento.push_back(&m.prog1);
    std::vector<Disciplina*> esperado = {&m.prog1};
    assert(a.getEmAndamento() == esperado);
    return true;
}

bool Unit_Aluno::getDisponiveis(void) {
    Matriz m;

    //sem curso: nenhuma disciplina disponivel
    AlunoImpl semCurso;
    assert(semCurso.getDisponiveis().empty());

    AlunoImpl a(1, "Ana", "ana@ufop.edu.br", "1234", &m.curso, 1);

    //inicio: Calculo II esta bloqueada
    std::vector<Disciplina*> inicio = {&m.calc1, &m.prog1};
    assert(a.getDisponiveis() == inicio);

    //concluir Calculo I libera Calculo II
    a.concluidas.push_back(&m.calc1);
    std::vector<Disciplina*> depois = {&m.calc2, &m.prog1};
    assert(a.getDisponiveis() == depois);

    //disciplina em andamento nao e disponivel
    a.emAndamento.push_back(&m.prog1);
    std::vector<Disciplina*> semProg = {&m.calc2};
    assert(a.getDisponiveis() == semProg);

    //tudo concluido ou em andamento: nada disponivel
    a.emAndamento.push_back(&m.calc2);
    assert(a.getDisponiveis().empty());
    return true;
}

bool Unit_Aluno::getEstado(void) {
    Matriz m;
    AlunoImpl a(1, "User", "user@ufop.edu.br", "1234", &m.curso, 1);

    assert(a.getEstado(&m.calc1) == EstadoDisciplina::Disponivel);
    assert(a.getEstado(&m.calc2) == EstadoDisciplina::Bloqueada);

    a.concluidas.push_back(&m.calc1);
    assert(a.getEstado(&m.calc1) == EstadoDisciplina::Concluida);
    assert(a.getEstado(&m.calc2) == EstadoDisciplina::Disponivel);

    a.emAndamento.push_back(&m.prog1);
    assert(a.getEstado(&m.prog1) == EstadoDisciplina::EmAndamento);
    return true;
}

bool Unit_Aluno::getProgresso(void) {
    Matriz m;

    //sem curso
    AlunoImpl semCurso;
    assert(semCurso.getProgresso() == 0.0);

    //curso com carga total zero
    CursoMock vazio("Vazio");
    AlunoImpl aVazio(1, "User", "user@ufop.edu.br", "1234", &vazio, 1);
    assert(aVazio.getProgresso() == 0.0);

    AlunoImpl a(2, "User2", "user2@ufop.edu.br", "1234", &m.curso, 1);
    //nada concluido
    assert(std::fabs(a.getProgresso() - 0.0) < 1e-4);

    //60 h de 180 h
    a.concluidas.push_back(&m.calc1);
    assert(std::fabs(a.getProgresso() - 100.0 / 3.0) < 1e-4);

    //curso completo
    a.concluidas.push_back(&m.calc2);
    a.concluidas.push_back(&m.prog1);
    assert(std::fabs(a.getProgresso() - 100.0) < 1e-4);
    return true;
}

bool Unit_Aluno::copyConstructor(void) {
    Matriz m;
    AlunoImpl original(4, "Original", "o@ufop.edu.br", "segredo", &m.curso, 2);
    original.concluidas.push_back(&m.calc1);
    original.emAndamento.push_back(&m.prog1);

    AlunoImpl copia(original);
    assert(copia.id == 4);
    assert(copia.nome == "Original");
    assert(copia.email == "o@ufop.edu.br");
    assert(copia.autenticar("segredo"));
    assert(copia.curso == &m.curso);
    assert(copia.periodoAtual == 2);
    assert(copia.concluidas.size() == 1);
    assert(copia.concluidas[0] == &m.calc1);
    assert(copia.emAndamento.size() == 1);
    assert(copia.emAndamento[0] == &m.prog1);

    //os vetores da copia sao independentes do original
    copia.marcarConcluida(&m.calc2);
    assert(copia.concluidas.size() == 2);
    assert(original.concluidas.size() == 1);
    return true;
}

bool Unit_Aluno::assignmentOperator(void) {
    Matriz m;
    AlunoImpl original(4, "Original", "o@ufop.edu.br", "segredo", &m.curso, 2);
    original.concluidas.push_back(&m.calc1);
    original.emAndamento.push_back(&m.prog1);

    AlunoImpl destino(9, "Destino", "d@ufop.edu.br", "outra", nullptr, 7);
    destino.concluidas.push_back(&m.calc2);

    destino = original;
    assert(destino.id == 4);
    assert(destino.nome == "Original");
    assert(destino.email == "o@ufop.edu.br");
    assert(destino.autenticar("segredo"));
    assert(!destino.autenticar("outra"));
    assert(destino.curso == &m.curso);
    assert(destino.periodoAtual == 2);
    //os vetores sao substituidos e nao acumulados
    assert(destino.concluidas.size() == 1);
    assert(destino.concluidas[0] == &m.calc1);
    assert(destino.emAndamento.size() == 1);
    assert(destino.emAndamento[0] == &m.prog1);

    //auto-atribuicao nao altera o objeto
    AlunoImpl& ref = destino;
    destino = ref;
    assert(destino.nome == "Original");
    assert(destino.concluidas.size() == 1);
    return true;
}


bool Unit_Aluno::regressiveTest(void) {
    assert(defaultConstructor());
    assert(constructor());
    assert(destructor());
    assert(getCurso());
    assert(setCurso());
    assert(getPeriodoAtual());
    assert(setPeriodoAtual());
    assert(contem());
    assert(preRequisitosCumpridos());
    assert(marcarConcluida());
    assert(desmarcarConcluida());
    assert(marcarEmAndamento());
    assert(desmarcarEmAndamento());
    assert(getConcluidas());
    assert(getEmAndamento());
    assert(getDisponiveis());
    assert(getEstado());
    assert(getProgresso());
    assert(copyConstructor());
    assert(assignmentOperator());
    return true;
}