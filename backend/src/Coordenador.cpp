#include "../include/CoordenadorImpl.hpp"

//CONSTRTURORES
CoordenadorImpl::CoordenadorImpl() : UsuarioImpl() {
    this->curso = nullptr;
}

CoordenadorImpl::CoordenadorImpl(int id, std::string nome, std::string email, std::string senha, Curso* curso)
    : UsuarioImpl(id, nome, email, senha), curso(curso) {}

//DESTRUTOR
CoordenadorImpl::~CoordenadorImpl() {}

//SET E GET
void CoordenadorImpl::setCurso(Curso* curso) { 
    this->curso = curso; 
}
Curso* CoordenadorImpl::getCurso() const { 
    return this->curso; 
}

//CONSTRUTOR POR COPIA
CoordenadorImpl::CoordenadorImpl(const CoordenadorImpl& c) : Usuario(), UsuarioImpl(c), Coordenador() {
    this->curso = c.curso;
}

//OPERADOR DE IGUALDADE
CoordenadorImpl& CoordenadorImpl::operator=(const CoordenadorImpl& c) {
    if (&c != this) {
        UsuarioImpl::operator=(c);
        this->curso = c.curso;
    }
    return *this;
}
