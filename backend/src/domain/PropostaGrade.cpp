#include "../../include/domain/PropostaGradeImpl.hpp"
#include "../../include/domain/Aluno.hpp"
#include "../../include/domain/Curso.hpp"
#include "../../include/domain/Disciplina.hpp"
#include <stdexcept>

//CONSTRUTORES
PropostaGradeImpl::PropostaGradeImpl() {
    this->aluno = nullptr;
    this->periodoLetivo = "";
}

PropostaGradeImpl::PropostaGradeImpl(Aluno* aluno, std::string periodoLetivo)
    : aluno(aluno), periodoLetivo(periodoLetivo) {}

//DESTRUTOR
PropostaGradeImpl::~PropostaGradeImpl() {
    //limpa o vetor, sem destruir o aluno
    turmas.clear();
}

bool PropostaGradeImpl::conflitam(const bcc323::Turma& a, const bcc323::Turma& b) {
    for (const auto& ha : a.horarios) {
        for (const auto& hb : b.horarios) {
            if (ha.dia == hb.dia
                && ha.inicioEmMinutos() < hb.fimEmMinutos()
                && hb.inicioEmMinutos() < ha.fimEmMinutos()) {
                return true;
            }
        }
    }
    return false;
}

Disciplina* PropostaGradeImpl::buscarDisciplina(const std::string& codigo) const {
    if (aluno == nullptr || aluno->getCurso() == nullptr) {
        return nullptr;
    }
    return aluno->getCurso()->buscarDisciplina(codigo);
}

int PropostaGradeImpl::indiceDaDisciplina(const std::string& disciplinaCodigo) const {
    for (size_t i = 0; i < turmas.size(); i++) {
        if (turmas[i].disciplinaCodigo == disciplinaCodigo) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

//settesrs e getters
void PropostaGradeImpl::setAluno(Aluno* aluno) {
    this->aluno = aluno;
}
Aluno* PropostaGradeImpl::getAluno() const {
    return this->aluno;
}

void PropostaGradeImpl::setPeriodoLetivo(std::string periodoLetivo) {
    this->periodoLetivo = periodoLetivo;
}
std::string PropostaGradeImpl::getPeriodoLetivo() const {
    return this->periodoLetivo;
}

//ADICIONAR TURMA: valida na ordem do enum ResultadoAdicao
ResultadoAdicao PropostaGradeImpl::adicionarTurma(const bcc323::Turma& turma) {
    try {
        turma.validar();
    } catch (const std::invalid_argument&) {
        return ResultadoAdicao::TurmaInvalida;
    }
    if (aluno == nullptr || aluno->getCurso() == nullptr) {
        return ResultadoAdicao::SemCurso;
    }
    Disciplina* d = buscarDisciplina(turma.disciplinaCodigo);
    if (d == nullptr) {
        return ResultadoAdicao::DisciplinaInexistente;
    }
    if (indiceDaDisciplina(turma.disciplinaCodigo) >= 0) {
        return ResultadoAdicao::DisciplinaJaNaGrade;
    }
    //so disciplinas disponiveis: nao concluida, nao em andamento e com pre requisitos cumpridos
    if (aluno->getEstado(d) != EstadoDisciplina::Disponivel) {
        return ResultadoAdicao::DisciplinaIndisponivel;
    }
    if (temConflito(turma)) {
        return ResultadoAdicao::ConflitoDeHorario;
    }
    turmas.push_back(turma);
    return ResultadoAdicao::Adicionada;
}

bool PropostaGradeImpl::removerTurma(const std::string& disciplinaCodigo) {
    int i = indiceDaDisciplina(disciplinaCodigo);
    if (i < 0) {
        return false;
    }
    turmas.erase(turmas.begin() + i);
    return true;
}

void PropostaGradeImpl::limpar() {
    turmas.clear();
}

std::vector<bcc323::Turma> PropostaGradeImpl::getTurmas() const {
    return this->turmas;
}

bool PropostaGradeImpl::temConflito(const bcc323::Turma& turma) const {
    return !getConflitantes(turma).empty();
}

std::vector<bcc323::Turma> PropostaGradeImpl::getConflitantes(const bcc323::Turma& turma) const {
    std::vector<bcc323::Turma> resultado;
    for (const auto& t : turmas) {
        if (conflitam(t, turma)) {
            resultado.push_back(t);
        }
    }
    return resultado;
}

int PropostaGradeImpl::getCargaHorariaTotal() const {
    int total = 0;
    for (const auto& t : turmas) {
        Disciplina* d = buscarDisciplina(t.disciplinaCodigo);
        if (d != nullptr) {
            total += d->getCargaHoraria();
        }
    }
    return total;
}

//CONSTRUTOR POR COPIA
PropostaGradeImpl::PropostaGradeImpl(const PropostaGradeImpl& p) : PropostaGrade() {
    this->aluno = p.aluno;
    this->periodoLetivo = p.periodoLetivo;
    this->turmas = p.turmas;
}

//OPERADOR DE IGUALDADE
PropostaGradeImpl& PropostaGradeImpl::operator=(const PropostaGradeImpl& p) {
    if (&p != this) {
        this->aluno = p.aluno;
        this->periodoLetivo = p.periodoLetivo;
        this->turmas = p.turmas;
    }
    return *this;
}