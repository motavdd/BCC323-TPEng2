#include "Unit_AcademicSystem.hpp"
#include "Unit_ClasseMock.hpp"
#include "../../include/AcademicSystemImpl.hpp"
#include "../../include/UsuarioImpl.hpp"
#include "../../include/Aluno.hpp"
#include "../../include/Coordenador.hpp"
#include <cassert>
#include <string>

namespace {
    /*! @brief Usuario que conta quantas vezes foi destruido (para testar a posse dos objetos). */
    class UsuarioContado : public UsuarioImpl {
    public:
        static int destruidos;
        UsuarioContado(int id, std::string nome, std::string email, std::string senha)
            : UsuarioImpl(id, nome, email, senha) {}
        virtual ~UsuarioContado() { ++destruidos; }
    };
    int UsuarioContado::destruidos = 0;

    /*! @brief Curso que conta quantas vezes foi destruido. */
    class CursoContado : public CursoMock {
    public:
        static int destruidos;
        CursoContado(std::string nome) : CursoMock(nome) {}
        virtual ~CursoContado() { ++destruidos; }
    };
    int CursoContado::destruidos = 0;

    /*! @brief Disciplina que conta quantas vezes foi destruida. */
    class DisciplinaContada : public DisciplinaMock {
    public:
        static int destruidos;
        DisciplinaContada(std::string codigo) : DisciplinaMock(codigo, "Nome", 60, 1) {}
        virtual ~DisciplinaContada() { ++destruidos; }
    };
    int DisciplinaContada::destruidos = 0;
}

bool Unit_AcademicSystem::defaultConstructor(void) {
    AcademicSystemImpl s1;
    assert(s1.nome == "");
    assert(s1.proximoId == 1);
    assert(s1.usuarios.empty());
    assert(s1.cursos.empty());
    assert(s1.disciplinas.empty());
    return true;
}

bool Unit_AcademicSystem::constructor(void) {
    AcademicSystemImpl s2("Planejamento Academico");
    assert(s2.nome == "Planejamento Academico");
    assert(s2.proximoId == 1);
    assert(s2.usuarios.empty());
    return true;
}

bool Unit_AcademicSystem::destructor(void) {
    UsuarioContado::destruidos = 0;
    CursoContado::destruidos = 0;
    DisciplinaContada::destruidos = 0;

    AcademicSystemImpl* s = new AcademicSystemImpl("Sistema");
    s->add(static_cast<Usuario*>(new UsuarioContado(1, "A", "a@x.com", "1")));
    s->add(static_cast<Usuario*>(new UsuarioContado(2, "B", "b@x.com", "1")));
    s->add(static_cast<Curso*>(new CursoContado("BCC")));
    s->add(static_cast<Curso*>(new CursoContado("BSI")));
    s->add(static_cast<Disciplina*>(new DisciplinaContada("D1")));
    s->add(static_cast<Disciplina*>(new DisciplinaContada("D2")));

    assert(UsuarioContado::destruidos == 0);
    delete s;

    //o sistema e dono dos objetos, entao deve destrui-los
    assert(UsuarioContado::destruidos == 2);
    assert(CursoContado::destruidos == 2);
    assert(DisciplinaContada::destruidos == 2);
    return true;
}

bool Unit_AcademicSystem::getNome(void) {
    AcademicSystemImpl s;
    s.nome = "TestName";
    assert(s.getNome() == "TestName");
    return true;
}

bool Unit_AcademicSystem::setNome(void) {
    AcademicSystemImpl s;
    s.setNome("NewName");
    assert(s.nome == "NewName");
    return true;
}

bool Unit_AcademicSystem::addUsuario(void) {
    AcademicSystemImpl s;
    Usuario* u = new UsuarioImpl(1, "User", "user@x.com", "1234");

    s.add(u);
    assert(s.usuarios.size() == 1);
    assert(s.usuarios[0] == u);

    //duplicata e ignorada
    s.add(u);
    assert(s.usuarios.size() == 1);

    //ponteiro nulo e ignorado
    s.add(static_cast<Usuario*>(nullptr));
    assert(s.usuarios.size() == 1);
    return true;
}

bool Unit_AcademicSystem::addCurso(void) {
    AcademicSystemImpl s;
    Curso* c = new CursoMock("BCC");

    s.add(c);
    assert(s.cursos.size() == 1);
    assert(s.cursos[0] == c);

    s.add(c);
    assert(s.cursos.size() == 1);

    s.add(static_cast<Curso*>(nullptr));
    assert(s.cursos.size() == 1);
    return true;
}

bool Unit_AcademicSystem::addDisciplina(void) {
    AcademicSystemImpl s;
    Disciplina* d = new DisciplinaMock("D1", "Nome", 60, 1);

    s.add(d);
    assert(s.disciplinas.size() == 1);
    assert(s.disciplinas[0] == d);

    s.add(d);
    assert(s.disciplinas.size() == 1);

    s.add(static_cast<Disciplina*>(nullptr));
    assert(s.disciplinas.size() == 1);
    return true;
}

bool Unit_AcademicSystem::removeUsuario(void) {
    UsuarioContado::destruidos = 0;
    AcademicSystemImpl s;
    Usuario* u = new UsuarioContado(1, "User", "user@x.com", "1234");
    s.add(u);

    s.remove(u);
    assert(s.usuarios.empty());
    //remove apenas desvincula: o objeto continua vivo
    assert(UsuarioContado::destruidos == 0);

    //remover quem nao esta no sistema ou nulo nao altera nada
    UsuarioImpl outro;
    s.remove(&outro);
    s.remove(static_cast<Usuario*>(nullptr));
    assert(s.usuarios.empty());

    delete u; //quem removeu volta a ser o dono
    assert(UsuarioContado::destruidos == 1);
    return true;
}

bool Unit_AcademicSystem::removeCurso(void) {
    CursoContado::destruidos = 0;
    AcademicSystemImpl s;
    Curso* c = new CursoContado("BCC");
    s.add(c);

    s.remove(c);
    assert(s.cursos.empty());
    assert(CursoContado::destruidos == 0);

    CursoMock outro("Outro");
    s.remove(&outro);
    s.remove(static_cast<Curso*>(nullptr));
    assert(s.cursos.empty());

    delete c;
    assert(CursoContado::destruidos == 1);
    return true;
}

bool Unit_AcademicSystem::removeDisciplina(void) {
    DisciplinaContada::destruidos = 0;
    AcademicSystemImpl s;
    Disciplina* d = new DisciplinaContada("D1");
    s.add(d);

    s.remove(d);
    assert(s.disciplinas.empty());
    assert(DisciplinaContada::destruidos == 0);

    DisciplinaMock outra;
    s.remove(&outra);
    s.remove(static_cast<Disciplina*>(nullptr));
    assert(s.disciplinas.empty());

    delete d;
    assert(DisciplinaContada::destruidos == 1);
    return true;
}

bool Unit_AcademicSystem::cadastrarAluno(void) {
    AcademicSystemImpl s("Sistema");
    CursoMock* curso = new CursoMock("BCC");
    s.add(static_cast<Curso*>(curso));

    Aluno* a = s.cadastrarAluno("User", "user@x.com", "1234", curso, 2);
    assert(a != nullptr);
    assert(s.usuarios.size() == 1);
    assert(s.proximoId == 2);
    assert(a->getId() == 1);
    assert(a->getNome() == "User");
    assert(a->getEmail() == "user@x.com");
    assert(a->getCurso() == curso);
    assert(a->getPeriodoAtual() == 2);
    assert(a->autenticar("1234"));
    assert(!a->autenticar("0000"));

    //o segundo aluno recebe o proximo id
    Aluno* b = s.cadastrarAluno("User2", "user2@x.com", "5678", curso, 1);
    assert(b != nullptr);
    assert(b->getId() == 2);
    assert(s.usuarios.size() == 2);
    assert(s.proximoId == 3);
    return true;
}

bool Unit_AcademicSystem::cadastrarAlunoInvalido(void) {
    AcademicSystemImpl s("Sistema");
    CursoMock* curso = new CursoMock("BCC");
    s.add(static_cast<Curso*>(curso));

    //curso nulo
    assert(s.cadastrarAluno("A", "a@x.com", "1", nullptr, 1) == nullptr);

    //curso que nao pertence ao sistema
    CursoMock fora("Fora");
    assert(s.cadastrarAluno("A", "a@x.com", "1", &fora, 1) == nullptr);

    //recusas nao consomem id nem criam usuario
    assert(s.usuarios.empty());
    assert(s.proximoId == 1);

    //email duplicado (aluno)
    assert(s.cadastrarAluno("User", "user@x.com", "1234", curso, 1) != nullptr);
    assert(s.cadastrarAluno("Outra", "user@x.com", "9999", curso, 1) == nullptr);
    assert(s.usuarios.size() == 1);
    assert(s.proximoId == 2);

    return true;
}

bool Unit_AcademicSystem::cadastrarCoordenador(void) {
    AcademicSystemImpl s("Sistema");
    CursoMock* curso = new CursoMock("BCC");
    s.add(static_cast<Curso*>(curso));

    Coordenador* c = s.cadastrarCoordenador("Coord", "coord@x.com", "1234", curso);
    assert(c != nullptr);
    assert(s.usuarios.size() == 1);
    assert(s.proximoId == 2);
    assert(c->getId() == 1);
    assert(c->getNome() == "Coord");
    assert(c->getEmail() == "coord@x.com");
    assert(c->getCurso() == curso);
    assert(c->autenticar("1234"));

    //ids sao compartilhados entre alunos e coordenadores
    Aluno* a = s.cadastrarAluno("User", "user@x.com", "1234", curso, 1);
    assert(a != nullptr);
    assert(a->getId() == 2);
    return true;
}

bool Unit_AcademicSystem::cadastrarCoordenadorInvalido(void) {
    AcademicSystemImpl s("Sistema");
    CursoMock* curso = new CursoMock("BCC");
    s.add(static_cast<Curso*>(curso));

    //curso nulo
    assert(s.cadastrarCoordenador("C", "c@x.com", "1", nullptr) == nullptr);

    //curso que nao pertence ao sistema
    CursoMock fora("Fora");
    assert(s.cadastrarCoordenador("C", "c@x.com", "1", &fora) == nullptr);
    assert(s.usuarios.empty());
    assert(s.proximoId == 1);

    //email duplicado
    assert(s.cadastrarCoordenador("Coord", "coord@x.com", "1234", curso) != nullptr);
    assert(s.cadastrarCoordenador("Outro", "coord@x.com", "9999", curso) == nullptr);
    assert(s.usuarios.size() == 1);
    assert(s.proximoId == 2);
    return true;
}

bool Unit_AcademicSystem::autenticar(void) {
    AcademicSystemImpl s("Sistema");
    CursoMock* curso = new CursoMock("BCC");
    s.add(static_cast<Curso*>(curso));

    //sistema sem usuarios
    assert(s.autenticar("user@x.com", "1234") == nullptr);

    Aluno* a = s.cadastrarAluno("User", "user@x.com", "1234", curso, 1);
    Coordenador* c = s.cadastrarCoordenador("Coord", "coord@x.com", "abcd", curso);
    Usuario* esperadoAluno = a;
    Usuario* esperadoCoord = c;

    //credenciais corretas devolvem o proprio usuario
    assert(s.autenticar("user@x.com", "1234") == esperadoAluno);
    assert(s.autenticar("coord@x.com", "abcd") == esperadoCoord);

    //senha errada, email inexistente e senha de outro usuario
    assert(s.autenticar("user@x.com", "0000") == nullptr);
    assert(s.autenticar("user@x.com", "") == nullptr);
    assert(s.autenticar("naoexiste@x.com", "1234") == nullptr);
    assert(s.autenticar("user@x.com", "abcd") == nullptr);
    return true;
}

bool Unit_AcademicSystem::buscarCurso(void) {
    AcademicSystemImpl s;
    Curso* bcc = new CursoMock("BCC");
    Curso* bsi = new CursoMock("BSI");
    s.add(bcc);
    s.add(bsi);

    assert(s.buscarCurso("BCC") == bcc);
    assert(s.buscarCurso("BSI") == bsi);
    assert(s.buscarCurso("Inexistente") == nullptr);
    assert(s.buscarCurso("") == nullptr);
    return true;
}

bool Unit_AcademicSystem::buscarDisciplina(void) {
    AcademicSystemImpl s;
    Disciplina* d1 = new DisciplinaMock("MAT101", "Calculo I", 60, 1);
    Disciplina* d2 = new DisciplinaMock("BCC101", "Programacao I", 60, 1);
    s.add(d1);
    s.add(d2);

    assert(s.buscarDisciplina("MAT101") == d1);
    assert(s.buscarDisciplina("BCC101") == d2);
    assert(s.buscarDisciplina("XXX000") == nullptr);
    //a busca e pelo codigo, nao pelo nome
    assert(s.buscarDisciplina("Calculo I") == nullptr);
    return true;
}

bool Unit_AcademicSystem::buscarUsuario(void) {
    AcademicSystemImpl s;
    Usuario* u1 = new UsuarioImpl(1, "Ana", "ana@x.com", "1");
    Usuario* u2 = new UsuarioImpl(2, "Bia", "bia@x.com", "2");
    s.add(u1);
    s.add(u2);

    assert(s.buscarUsuario("ana@x.com") == u1);
    assert(s.buscarUsuario("bia@x.com") == u2);
    assert(s.buscarUsuario("naoexiste@x.com") == nullptr);
    //a busca e pelo email, nao pelo nome
    assert(s.buscarUsuario("Ana") == nullptr);
    return true;
}

bool Unit_AcademicSystem::regressiveTest(void) {
    assert(defaultConstructor());
    assert(constructor());
    assert(destructor());
    assert(getNome());
    assert(setNome());
    assert(addUsuario());
    assert(addCurso());
    assert(addDisciplina());
    assert(removeUsuario());
    assert(removeCurso());
    assert(removeDisciplina());
    assert(cadastrarAluno());
    assert(cadastrarAlunoInvalido());
    assert(cadastrarCoordenador());
    assert(cadastrarCoordenadorInvalido());
    assert(autenticar());
    assert(buscarCurso());
    assert(buscarDisciplina());
    assert(buscarUsuario());
    return true;
}