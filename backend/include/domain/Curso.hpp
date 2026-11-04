#ifndef CURSO_HPP
#define CURSO_HPP

#include <string>
#include <vector>

class Disciplina;

/** @brief Contrato de um curso e de sua matriz curricular. */
class Curso {
public:
    virtual ~Curso() = default;

    virtual void setNome(std::string nome) = 0;
    virtual std::string getNome() const = 0;

    /** @brief Vincula uma disciplina sem assumir sua propriedade. */
    virtual void add(Disciplina* disciplina) = 0;
    /** @brief Desvincula uma disciplina sem destruí-la. */
    virtual void remove(Disciplina* disciplina) = 0;
    virtual std::vector<Disciplina*> getDisciplinas() const = 0;

    /** @brief Retorna a soma das cargas horárias da matriz. */
    virtual int getCargaTotal() const = 0;
};

#endif
