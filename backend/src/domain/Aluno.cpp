#include "domain/AlunoImpl.hpp"
#include "domain/CursoImpl.hpp"
#include "domain/DisciplinaImpl.hpp"
#include <algorithm>

//CONSTRUTORES
AlunoImpl::AlunoImpl() : UsuarioImpl() {
    this->curso = nullptr;
    this->periodoAtual = 1;
}

AlunoImpl::AlunoImpl(int id, std::string nome, std::string email, std::string senha,
                     Curso* curso, int periodoAtual)
    : UsuarioImpl(id, nome, email, senha), curso(curso), periodoAtual(periodoAtual) {}

//DESTRUTOR
AlunoImpl::~AlunoImpl() {
    //limpa os vetores, sem destruir as disciplinas
    concluidas.clear();
    emAndamento.clear();
}

//Busca se uma disviplina pertence ao vetor de disciĺinas
bool AlunoImpl::contem(const std::vector<Disciplina*>& v, Disciplina* d) {
    return std::find(v.begin(), v.end(), d) != v.end();
}

//Verifica se uma disciplina possui pre requisitos
bool AlunoImpl::preRequisitosCumpridos(Disciplina* d) const {
    for (auto pre : d->getPreRequisitos()) {
        if (!contem(concluidas, pre)) {
            return false;
        }
    }
    return true;
}

//Set e get de curso
void AlunoImpl::setCurso(Curso* curso) { 
    this->curso = curso; 
}
Curso* AlunoImpl::getCurso() const { 
    return this->curso; 
}

//set e get de periodo atual
void AlunoImpl::setPeriodoAtual(int periodo) { 
    this->periodoAtual = periodo; 
}
int AlunoImpl::getPeriodoAtual() const { 
    return this->periodoAtual; 
}

//Marcar disciplina como concluída
void AlunoImpl::marcarConcluida(Disciplina* d) {
    if (d == nullptr) {
        return;
    }
    desmarcarEmAndamento(d); //nao pode estar concluida e em andamento
    if (!contem(concluidas, d)) {
        concluidas.push_back(d);
    }
}

//desmaca uma disciplina 
void AlunoImpl::desmarcarConcluida(Disciplina* d) {
    auto it = std::find(concluidas.begin(), concluidas.end(), d);
    if (it != concluidas.end()) {
        concluidas.erase(it);
    }
}

//Marca uma disciplina como em andamento
void AlunoImpl::marcarEmAndamento(Disciplina* d) {
    if (d == nullptr || contem(concluidas, d)) {
        return;
    }
    if (!contem(emAndamento, d)) {
        emAndamento.push_back(d);
    }
}

//desmarca uma disciplina em andamento
void AlunoImpl::desmarcarEmAndamento(Disciplina* d) {
    auto it = std::find(emAndamento.begin(), emAndamento.end(), d);
    if (it != emAndamento.end()) {
        emAndamento.erase(it);
    }
}

//retorna um vetor de disciplinas concluídas
std::vector<Disciplina*> AlunoImpl::getConcluidas() const { 
    return this->concluidas; 
}
//retorna as disciplinas em andamento
std::vector<Disciplina*> AlunoImpl::getEmAndamento() const { 
    return this->emAndamento; 
}

//retorna as disciplinas disposníveis baseado nos estados da disciplina
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

//atribui estados as disciplinas
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

//retorna o progresso do aluno
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

//Constrtutor de copia
AlunoImpl::AlunoImpl(const AlunoImpl& a) : Usuario(), UsuarioImpl(a), Aluno() {
    this->curso = a.curso;
    this->periodoAtual = a.periodoAtual;
    this->concluidas = a.concluidas; 
    this->emAndamento = a.emAndamento;
}

//Operrador igual
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
