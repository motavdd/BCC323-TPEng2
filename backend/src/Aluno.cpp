#include "../include/AlunoImpl.hpp"
#include "../include/CursoImpl.hpp"
#include "../include/DisciplinaImpl.hpp"
#include <algorithm>

AlunoImpl::AlunoImpl() : UsuarioImpl() {
    this->curso = nullptr;
    this->periodoAtual = 1;
}

AlunoImpl::AlunoImpl(int id, std::string nome, std::string email, std::string senha,
                     Curso* curso, int periodoAtual)
    : UsuarioImpl(id, nome, email, senha), curso(curso), periodoAtual(periodoAtual) {}

AlunoImpl::~AlunoImpl() {
    //limpa os vetores, sem destruir as disciplinas
    concluidas.clear();
    emAndamento.clear();
}

bool AlunoImpl::contem(const std::vector<Disciplina*>& v, Disciplina* d) {
    return std::find(v.begin(), v.end(), d) != v.end();
}

bool AlunoImpl::preRequisitosCumpridos(Disciplina* d) const {
    for (auto pre : d->getPreRequisitos()) {
        if (!contem(concluidas, pre)) {
            return false;
        }
    }
    return true;
}

void AlunoImpl::setCurso(Curso* curso) { 
    this->curso = curso; 
}

Curso* AlunoImpl::getCurso() const { 
    return this->curso; 
}

void AlunoImpl::setPeriodoAtual(int periodo) { 
    this->periodoAtual = periodo; 
}

int AlunoImpl::getPeriodoAtual() const { 
    return this->periodoAtual; 
}

void AlunoImpl::marcarConcluida(Disciplina* d) {
    if (d == nullptr) {
        return;
    }
    desmarcarEmAndamento(d); //nao pode estar concluida e em andamento
    if (!contem(concluidas, d)) {
        concluidas.push_back(d);
    }
}

void AlunoImpl::desmarcarConcluida(Disciplina* d) {
    auto it = std::find(concluidas.begin(), concluidas.end(), d);
    if (it != concluidas.end()) {
        concluidas.erase(it);
    }
}

void AlunoImpl::marcarEmAndamento(Disciplina* d) {
    if (d == nullptr || contem(concluidas, d)) {
        return;
    }
    if (!contem(emAndamento, d)) {
        emAndamento.push_back(d);
    }
}

void AlunoImpl::desmarcarEmAndamento(Disciplina* d) {
    auto it = std::find(emAndamento.begin(), emAndamento.end(), d);
    if (it != emAndamento.end()) {
        emAndamento.erase(it);
    }
}

std::vector<Disciplina*> AlunoImpl::getConcluidas() const { 
    return this->concluidas; 
}

std::vector<Disciplina*> AlunoImpl::getEmAndamento() const { 
    return this->emAndamento; 
}

std::vector<Disciplina*> AlunoImpl::getDisponiveis() const {
    std::vector<Disciplina*> resultado;
    if (curso == nullptr) {
        return resultado;
    }
    for (auto d : curso->getDisciplinas()) {
        if (getEstado(d) == EstadoDisciplina::Disponivel) {
            resultado.push_back(d);
        }
    }
    return resultado;
}

EstadoDisciplina AlunoImpl::getEstado(Disciplina* d) const {
    if (contem(concluidas, d)) {
        return EstadoDisciplina::Concluida;
    }
    if (contem(emAndamento, d)) {
        return EstadoDisciplina::EmAndamento;
    }
    if (preRequisitosCumpridos(d)) {
        return EstadoDisciplina::Disponivel;
    }
    return EstadoDisciplina::Bloqueada;
}

double AlunoImpl::getProgresso() const {
    if (curso == nullptr || curso->getCargaTotal() == 0) {
        return 0.0;
    }
    int feita = 0;
    for (auto d : concluidas) {
        //so conta disciplinas que pertencem a matriz do curso
        if (contem(curso->getDisciplinas(), d)) {
            feita += d->getCargaHoraria();
        }
    }
    return 100.0 * feita / curso->getCargaTotal();
}

AlunoImpl::AlunoImpl(const AlunoImpl& a) : Usuario(), UsuarioImpl(a), Aluno() {
    this->curso = a.curso;
    this->periodoAtual = a.periodoAtual;
    this->concluidas = a.concluidas; //copia os ponteiros
    this->emAndamento = a.emAndamento;
}

AlunoImpl& AlunoImpl::operator=(const AlunoImpl& a) {
    if (&a != this) {
        UsuarioImpl::operator=(a);
        this->curso = a.curso;
        this->periodoAtual = a.periodoAtual;
        this->concluidas = a.concluidas;
        this->emAndamento = a.emAndamento;
    }
    return *this;
}
