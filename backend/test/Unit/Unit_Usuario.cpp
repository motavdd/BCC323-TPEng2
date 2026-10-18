#include "Unit_Usuario.hpp"
#include "../../include/UsuarioImpl.hpp"
#include <cassert>
#include <functional>
#include <string>

bool Unit_Usuario::defaultConstructor(void) {
    UsuarioImpl u1;
    assert(u1.id == 0);
    assert(u1.nome == "");
    assert(u1.email == "");
    assert(u1.senhaHash == 0);
    return true;
}

bool Unit_Usuario::constructor(void) {
    UsuarioImpl u2(1, "User", "user@ufop.edu.br", "1234");
    assert(u2.id == 1);
    assert(u2.nome == "User");
    assert(u2.email == "user@ufop.edu.br");
    //a senha nao e guardada em texto puro, apenas o hash
    assert(u2.senhaHash == std::hash<std::string>()("1234"));
    return true;
}

bool Unit_Usuario::destructor(void) {
    UsuarioImpl* u = new UsuarioImpl();
    delete u;
    return true;
}

bool Unit_Usuario::getId(void) {
    UsuarioImpl u;
    u.id = 42;
    assert(u.getId() == 42);
    return true;
}

bool Unit_Usuario::setId(void) {
    UsuarioImpl u;
    u.setId(7);
    assert(u.id == 7);
    return true;
}

bool Unit_Usuario::getNome(void) {
    UsuarioImpl u;
    u.nome = "TestName";
    assert(u.getNome() == "TestName");
    return true;
}

bool Unit_Usuario::setNome(void) {
    UsuarioImpl u;
    u.setNome("NewName");
    assert(u.nome == "NewName");
    return true;
}

bool Unit_Usuario::getEmail(void) {
    UsuarioImpl u;
    u.email = "a@b.com";
    assert(u.getEmail() == "a@b.com");
    return true;
}

bool Unit_Usuario::setEmail(void) {
    UsuarioImpl u;
    u.setEmail("novo@b.com");
    assert(u.email == "novo@b.com");
    return true;
}

bool Unit_Usuario::setSenha(void) {
    UsuarioImpl u(1, "Ana", "a@b.com", "antiga");
    std::size_t hashAntigo = u.senhaHash;

    u.setSenha("nova");
    assert(u.senhaHash == std::hash<std::string>()("nova"));
    assert(u.senhaHash != hashAntigo);
    return true;
}

bool Unit_Usuario::autenticar(void) {
    UsuarioImpl u(1, "User", "a@b.com", "1234");

    //senha correta
    assert(u.autenticar("1234") == true);
    //senha errada, vazia e com diferenca de maiusculas
    assert(u.autenticar("0000") == false);
    assert(u.autenticar("") == false);
    assert(u.autenticar("1234 ") == false);

    //apos trocar a senha, a antiga deixa de valer
    u.setSenha("abcd");
    assert(u.autenticar("1234") == false);
    assert(u.autenticar("abcd") == true);

    //usuario default nao autentica senha nao vazia
    UsuarioImpl vazio;
    assert(vazio.autenticar("1234") == false);
    return true;
}

bool Unit_Usuario::copyConstructor(void) {
    UsuarioImpl original(5, "Original", "o@b.com", "segredo");

    UsuarioImpl copia(original);
    assert(copia.id == 5);
    assert(copia.nome == "Original");
    assert(copia.email == "o@b.com");
    assert(copia.senhaHash == original.senhaHash);
    assert(copia.autenticar("segredo"));

    //a copia e independente do original
    copia.setNome("Copia");
    assert(original.nome == "Original");
    return true;
}

bool Unit_Usuario::assignmentOperator(void) {
    UsuarioImpl original(5, "Original", "o@b.com", "segredo");
    UsuarioImpl destino(9, "Destino", "d@b.com", "outra");

    destino = original;
    assert(destino.id == 5);
    assert(destino.nome == "Original");
    assert(destino.email == "o@b.com");
    assert(destino.senhaHash == original.senhaHash);
    assert(destino.autenticar("segredo"));
    assert(!destino.autenticar("outra"));

    //auto-atribuicao nao altera o objeto
    UsuarioImpl& ref = destino;
    destino = ref;
    assert(destino.id == 5);
    assert(destino.nome == "Original");
    return true;
}

bool Unit_Usuario::regressiveTest(void) {
    assert(defaultConstructor());
    assert(constructor());
    assert(destructor());
    assert(getId());
    assert(setId());
    assert(getNome());
    assert(setNome());
    assert(getEmail());
    assert(setEmail());
    assert(setSenha());
    assert(autenticar());
    assert(copyConstructor());
    assert(assignmentOperator());
    return true;
}