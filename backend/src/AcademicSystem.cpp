#include "../include/AcademicSystemImpl.hpp"
#include "../include/AlunoImpl.hpp"
#include "../include/CoordenadorImpl.hpp"
#include "../include/CursoImpl.hpp"
#include "../include/DisciplinaImpl.hpp"
#include <algorithm>

namespace {
    // Adiciona p em v se não for nulo nem duplicado
    template <typename T>
    void adicionar(std::vector<T*>& v, T* p) {
        if (p != nullptr && std::find(v.begin(), v.end(), p) == v.end()) {
            v.push_back(p);
        }
    }

    // Retira p de v sem deletar
    template <typename T>
    void retirar(std::vector<T*>& v, T* p) {
        auto it = std::find(v.begin(), v.end(), p);
        if (it != v.end()) {
            v.erase(it);
        }
    }
}

//CONSTRUTORES
AcademicSystemImpl::AcademicSystemImpl() {
    this->nome = "";
    this->proximoId = 1;
}

AcademicSystemImpl::AcademicSystemImpl(std::string nome) : nome(nome), proximoId(1) {}

AcademicSystemImpl::~AcademicSystemImpl() {
    //deletar todos os objetos
    for (auto u : usuarios){
        delete u;
    }
    for (auto c : cursos) {
        delete c;
    }
    for (auto d : disciplinas){ 
        delete d;
    }
    usuarios.clear();
    cursos.clear();
    disciplinas.clear();
}

void AcademicSystemImpl::setNome(std::string nome) { 
    this->nome = nome; 
}
std::string AcademicSystemImpl::getNome() const { 
    return this->nome; 
}

void AcademicSystemImpl::add(Usuario* u) { 
    adicionar(usuarios, u); 
}
void AcademicSystemImpl::add(Curso* c) { 
    adicionar(cursos, c); 
}
void AcademicSystemImpl::add(Disciplina* d) { 
    adicionar(disciplinas, d);
}

void AcademicSystemImpl::remove(Usuario* u) { 
    retirar(usuarios, u); 
}
void AcademicSystemImpl::remove(Curso* c) { 
    retirar(cursos, c); 
}
void AcademicSystemImpl::remove(Disciplina* d) { 
    retirar(disciplinas, d); 
}

Aluno* AcademicSystemImpl::cadastrarAluno(std::string nome, std::string email, std::string senha,
                                      Curso* curso, int periodo) {
    // O curso precisa pertencer ao sistema e o e-mail não pode estar em uso
    if (std::find(cursos.begin(), cursos.end(), curso) == cursos.end()) {
        return nullptr;
    }
    if (buscarUsuario(email) != nullptr) {
        return nullptr;
    }
    AlunoImpl* aluno = new AlunoImpl(proximoId++, nome, email, senha, curso, periodo);
    usuarios.push_back(aluno);
    return aluno;
}

Coordenador* AcademicSystemImpl::cadastrarCoordenador(std::string nome, std::string email, std::string senha,
                                                  Curso* curso) {
    if (std::find(cursos.begin(), cursos.end(), curso) == cursos.end()) {
        return nullptr;
    }
    if (buscarUsuario(email) != nullptr) {
        return nullptr;
    }
    CoordenadorImpl* coord = new CoordenadorImpl(proximoId++, nome, email, senha, curso);
    usuarios.push_back(coord);
    return coord;
}

Usuario* AcademicSystemImpl::autenticar(std::string email, std::string senha) {
    Usuario* u = buscarUsuario(email);
    if (u != nullptr && u->autenticar(senha)) {
        return u;
    }
    return nullptr;
}

Curso* AcademicSystemImpl::buscarCurso(std::string nome) {
    for (auto c : cursos) {
        if (c->getNome() == nome) return c;
    }
    return nullptr;
}

Disciplina* AcademicSystemImpl::buscarDisciplina(std::string codigo) {
    for (auto d : disciplinas) {
        if (d->getCodigo() == codigo) return d;
    }
    return nullptr;
}

Usuario* AcademicSystemImpl::buscarUsuario(std::string email) {
    for (auto u : usuarios) {
        if (u->getEmail() == email) return u;
    }
    return nullptr;
}