#include "domain/CursoImpl.hpp"
#include "domain/Disciplina.hpp"

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <utility>

CursoImpl::CursoImpl(std::string nome) : nome(std::move(nome)) {}

void CursoImpl::setNome(std::string nome) {
    this->nome = std::move(nome);
}

std::string CursoImpl::getNome() const {
    return nome;
}

void CursoImpl::add(Disciplina* disciplina) {
    if (disciplina == nullptr) {
        return;
    }

    if (std::find(disciplinas.begin(), disciplinas.end(), disciplina)
        == disciplinas.end()) {
        disciplinas.push_back(disciplina);
    }
}

void CursoImpl::remove(Disciplina* disciplina) {
    disciplinas.erase(
        std::remove(disciplinas.begin(), disciplinas.end(), disciplina),
        disciplinas.end()
    );
}

std::vector<Disciplina*> CursoImpl::getDisciplinas() const {
    return disciplinas;
}

int CursoImpl::getCargaTotal() const {
    int total = 0;

    for (const auto* disciplina : disciplinas) {
        if (disciplina == nullptr) {
            continue;
        }

        const int carga = disciplina->getCargaHoraria();

        if (carga < 0) {
            throw std::invalid_argument(
                "A carga horaria de uma disciplina nao pode ser negativa"
            );
        }

        if (carga > std::numeric_limits<int>::max() - total) {
            throw std::overflow_error(
                "A carga horaria total excede o limite de int"
            );
        }

        total += carga;
    }

    return total;
}
