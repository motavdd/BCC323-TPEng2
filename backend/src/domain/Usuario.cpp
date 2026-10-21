#include "domain/UsuarioImpl.hpp"
#include <functional>

//CONSTRUTORES
UsuarioImpl::UsuarioImpl() {
    this->id = 0;
    this->nome = "";
    this->email = "";
    this->senhaHash = 0;
}

UsuarioImpl::UsuarioImpl(int id, std::string nome, std::string email, std::string senha)
    : id(id), nome(nome), email(email) {
    this->senhaHash = std::hash<std::string>()(senha);
}

//DESTRTUTORES
UsuarioImpl::~UsuarioImpl() {}

//SETTERS E GETTERS
void UsuarioImpl::setId(int id) { 
    this->id = id; 
}
int UsuarioImpl::getId() const { 
    return this->id; 
}


void UsuarioImpl::setNome(std::string nome) { 
    this->nome = nome; 
}
std::string UsuarioImpl::getNome() const { 
    return this->nome; 
}

void UsuarioImpl::setEmail(std::string email) { 
    this->email = email; 
}
std::string UsuarioImpl::getEmail() const { 
    return this->email; 
}

//Pega a senha e tranforma em código
void UsuarioImpl::setSenha(std::string senha) {
    this->senhaHash = std::hash<std::string>()(senha);
}

//verifica se a senha é igual e retorna true se sim.
bool UsuarioImpl::autenticar(std::string senha) const {
    return this->senhaHash == std::hash<std::string>()(senha);
}

//CONSTRUTOR POR COPIA
UsuarioImpl::UsuarioImpl(const UsuarioImpl& u) : Usuario() {
    this->id = u.id;
    this->nome = u.nome;
    this->email = u.email;
    this->senhaHash = u.senhaHash;
}

//OPERADOR DE IGUALDADE
UsuarioImpl& UsuarioImpl::operator=(const UsuarioImpl& u) {
    if (&u == this) {
        return *this;
    }
    this->id = u.id;
    this->nome = u.nome;
    this->email = u.email;
    this->senhaHash = u.senhaHash;
    return *this;
}
