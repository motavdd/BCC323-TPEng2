#ifndef DISCIPLINA_HPP
#define DISCIPLINA_HPP

#include <string>
#include <vector>

/** @brief Contrato de uma disciplina e seus pre-requisitos. */
class Disciplina {
public:
    virtual ~Disciplina() = default;

    virtual void setCodigo(std::string codigo) = 0;
    virtual std::string getCodigo() const = 0;
    virtual void setNome(std::string nome) = 0;
    virtual std::string getNome() const = 0;
    virtual void setCargaHoraria(int cargaHoraria) = 0;
    virtual int getCargaHoraria() const = 0;

    /** @brief Vincula um pre-requisito sem assumir sua propriedade. */
    virtual void addPreRequisito(Disciplina* disciplina) = 0;
    virtual void removePreRequisito(Disciplina* disciplina) = 0;
    virtual std::vector<Disciplina*> getPreRequisitos() const = 0;
};

#endif
