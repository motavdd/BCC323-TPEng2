#include "Unit_Coordenador.hpp"
#include "Unit_ClasseMock.hpp"
#include "../../include/CoordenadorImpl.hpp"
#include <cassert>
#include <string>

bool Unit_Coordenador::defaultConstructor(void) {
    CoordenadorImpl c1;
    assert(c1.curso == nullptr);
    //atributos herdados de UsuarioImpl
    assert(c1.id == 0);
    assert(c1.nome == "");
    assert(c1.email == "");
    return true;
}

bool Unit_Coordenador::constructor(void) {
    CursoMock curso("Ciencia da Computacao");
    CoordenadorImpl c2(3, "Coord", "coord@ufop.edu.br", "1234", &curso);

    assert(c2.id == 3);
    assert(c2.nome == "Coord");
    assert(c2.email == "coord@ufop.edu.br");
    assert(c2.curso == &curso);
    assert(c2.autenticar("1234"));
    return true;
}

bool Unit_Coordenador::destructor(void) {
    CoordenadorImpl* c = new CoordenadorImpl();
    delete c;

    //destruicao via ponteiro da interface (destrutor virtual)
    Usuario* u = new CoordenadorImpl();
    delete u;
    return true;
}

bool Unit_Coordenador::getCurso(void) {
    CursoMock curso("BCC");
    CoordenadorImpl c;
    c.curso = &curso;
    assert(c.getCurso() == &curso);
    return true;
}

bool Unit_Coordenador::setCurso(void) {
    CursoMock curso("BCC");
    CoordenadorImpl c;
    c.setCurso(&curso);
    assert(c.curso == &curso);

    c.setCurso(nullptr);
    assert(c.curso == nullptr);
    return true;
}

bool Unit_Coordenador::copyConstructor(void) {
    CursoMock curso("BCC");
    CoordenadorImpl original(3, "Coord", "coord@ufop.edu.br", "1234", &curso);

    CoordenadorImpl copia(original);
    assert(copia.id == 3);
    assert(copia.nome == "Coord");
    assert(copia.email == "coord@ufop.edu.br");
    assert(copia.curso == &curso);
    assert(copia.autenticar("1234"));
    return true;
}

bool Unit_Coordenador::assignmentOperator(void) {
    CursoMock curso1("BCC");
    CursoMock curso2("BSI");
    CoordenadorImpl original(3, "Coord", "coord@ufop.edu.br", "1234", &curso1);
    CoordenadorImpl destino(8, "Outro", "outro@ufop.edu.br", "9999", &curso2);

    destino = original;
    assert(destino.id == 3);
    assert(destino.nome == "Coord");
    assert(destino.email == "coord@ufop.edu.br");
    assert(destino.curso == &curso1);
    assert(destino.autenticar("1234"));
    assert(!destino.autenticar("9999"));

    //auto-atribuicao nao altera o objeto
    CoordenadorImpl& ref = destino;
    destino = ref;
    assert(destino.curso == &curso1);
    assert(destino.nome == "Coord");
    return true;
}

bool Unit_Coordenador::inheritance(void) {
    CursoMock curso("BCC");
    Usuario* u = new CoordenadorImpl(3, "Coord", "coord@ufop.edu.br", "1234", &curso);

    //com heranca virtual e necessario dynamic_cast
    Coordenador* c = dynamic_cast<Coordenador*>(u);
    assert(c != nullptr);
    assert(c->getCurso() == &curso);
    assert(c->getNome() == "Coord");
    //um coordenador nao e um aluno
    assert(dynamic_cast<Aluno*>(u) == nullptr);

    delete u;
    return true;
}

bool Unit_Coordenador::regressiveTest(void) {
    assert(defaultConstructor());
    assert(constructor());
    assert(destructor());
    assert(getCurso());
    assert(setCurso());
    assert(copyConstructor());
    assert(assignmentOperator());
    return true;
}