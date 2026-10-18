#ifndef UNIT_MOCKS_H
#define UNIT_MOCKS_H

#include "../../include/Curso.hpp"
#include "../../include/Disciplina.hpp"
#include <algorithm>
#include <string>
#include <vector>

/*!
 * @brief Para isolar as dependências de Disciplina nos testes.
 */
class DisciplinaMock : public Disciplina {
private:
    std::string codigo;
    std::string nome;
    int cargaHoraria;
    int periodoSugerido;
    std::vector<Disciplina*> preRequisitos;
public:
    DisciplinaMock() : codigo(""), nome(""), cargaHoraria(0), periodoSugerido(0) {}
    DisciplinaMock(std::string codigo, std::string nome, int cargaHoraria, int periodoSugerido)
        : codigo(codigo), nome(nome), cargaHoraria(cargaHoraria), periodoSugerido(periodoSugerido) {}
    virtual ~DisciplinaMock() {}

    virtual void setCodigo(std::string c) override { this->codigo = c; }
    virtual std::string getCodigo() const override { return this->codigo; }
    virtual void setNome(std::string n) override { this->nome = n; }
    virtual std::string getNome() const override { return this->nome; }
    virtual void setCargaHoraria(int c) override { this->cargaHoraria = c; }
    virtual int getCargaHoraria() const override { return this->cargaHoraria; }
    virtual void setPeriodoSugerido(int p) override { this->periodoSugerido = p; }
    virtual int getPeriodoSugerido() const override { return this->periodoSugerido; }
    virtual void addPreRequisito(Disciplina* d) override { this->preRequisitos.push_back(d); }
    virtual void removePreRequisito(Disciplina* d) override {
        this->preRequisitos.erase(std::remove(this->preRequisitos.begin(), this->preRequisitos.end(), d),
                                  this->preRequisitos.end());
    }
    virtual const std::vector<Disciplina*>& getPreRequisitos() const override { return this->preRequisitos; }
};

/*!
 * @brief Para isolar as dependências de Curso nos testes.
 */
class CursoMock : public Curso {
private:
    std::string nome;
    std::vector<Disciplina*> disciplinas;
public:
    CursoMock() : nome("") {}
    CursoMock(std::string nome) : nome(nome) {}
    virtual ~CursoMock() {}

    virtual void setNome(std::string n) override { this->nome = n; }
    virtual std::string getNome() const override { return this->nome; }
    virtual void add(Disciplina* d) override { this->disciplinas.push_back(d); }
    virtual void remove(Disciplina* d) override {
        this->disciplinas.erase(std::remove(this->disciplinas.begin(), this->disciplinas.end(), d),
                                this->disciplinas.end());
    }
    virtual const std::vector<Disciplina*>& getDisciplinas() const override { return this->disciplinas; }
    virtual int getCargaTotal() const override {
        int total = 0;
        for (auto d : this->disciplinas) {
            total += d->getCargaHoraria();
        }
        return total;
    }
};

#endif