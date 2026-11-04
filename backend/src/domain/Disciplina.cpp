#include "domain/DisciplinaImpl.hpp"

#include <algorithm>
#include <stdexcept>
#include <unordered_set>
#include <utility>

DisciplinaImpl::DisciplinaImpl(
    std::string codigo, std::string nome, int cargaHoraria, int periodoSugerido
) : codigo(std::move(codigo)), nome(std::move(nome)) {
    setCargaHoraria(cargaHoraria);
    setPeriodoSugerido(periodoSugerido);
}

void DisciplinaImpl::setCodigo(std::string codigo) {
    this->codigo = std::move(codigo);
}

std::string DisciplinaImpl::getCodigo() const {
    return codigo;
}

void DisciplinaImpl::setNome(std::string nome) {
    this->nome = std::move(nome);
}

std::string DisciplinaImpl::getNome() const {
    return nome;
}

void DisciplinaImpl::setCargaHoraria(int cargaHoraria) {
    if (cargaHoraria < 0) {
        throw std::invalid_argument("A carga horaria nao pode ser negativa");
    }
    this->cargaHoraria = cargaHoraria;
}

int DisciplinaImpl::getCargaHoraria() const {
    return cargaHoraria;
}

void DisciplinaImpl::addPreRequisito(Disciplina* disciplina) {
    if (disciplina == nullptr
        || std::find(preRequisitos.begin(), preRequisitos.end(), disciplina)
            != preRequisitos.end()) {
        return;
    }

    // Percorre as dependencias antes de alterar o objeto.
    std::vector<Disciplina*> pendentes{disciplina};
    std::unordered_set<Disciplina*> visitadas;

    while (!pendentes.empty()) {
        auto* atual = pendentes.back();
        pendentes.pop_back();

        if (atual == this) {
            throw std::invalid_argument("Pre-requisito formaria um ciclo");
        }
        if (atual == nullptr || !visitadas.insert(atual).second) {
            continue;
        }

        const auto anteriores = atual->getPreRequisitos();
        pendentes.insert(pendentes.end(), anteriores.begin(), anteriores.end());
    }

    preRequisitos.push_back(disciplina);
}

void DisciplinaImpl::removePreRequisito(Disciplina* disciplina) {
    preRequisitos.erase(
        std::remove(preRequisitos.begin(), preRequisitos.end(), disciplina),
        preRequisitos.end()
    );
}

std::vector<Disciplina*> DisciplinaImpl::getPreRequisitos() const {
    return preRequisitos;
}

void DisciplinaImpl::setPeriodoSugerido(int periodoSugerido) {
    if (periodoSugerido <= 0) {
        throw std::invalid_argument("O periodo sugerido deve ser maior que zero");
    }
    this->periodoSugerido = periodoSugerido;
}

int DisciplinaImpl::getPeriodoSugerido() const {
    return periodoSugerido;
}

void DisciplinaImpl::editarDados(std::string novoNome, int novaCargaHoraria, int novoPeriodoSugerido) {
    setNome(std::move(novoNome));
    setCargaHoraria(novaCargaHoraria);
    setPeriodoSugerido(novoPeriodoSugerido);
}