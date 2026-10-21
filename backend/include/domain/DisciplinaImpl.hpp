#ifndef DISCIPLINAIMPL_HPP
#define DISCIPLINAIMPL_HPP

#include "domain/Disciplina.hpp"

/**
 * @brief Implementacao concreta de uma disciplina.
 * Os pre-requisitos sao referencias nao proprietarias e nao sao destruidos
 * por esta classe. Devem permanecer vivos enquanto estiverem vinculados.
 */
class DisciplinaImpl : public Disciplina {
protected:
    std::string codigo;
    std::string nome;
    int cargaHoraria = 0;
    std::vector<Disciplina*> preRequisitos;

public:
    DisciplinaImpl() = default;
    DisciplinaImpl(std::string codigo, std::string nome, int cargaHoraria);
    ~DisciplinaImpl() override = default;

    void setCodigo(std::string codigo) override;
    std::string getCodigo() const override;
    void setNome(std::string nome) override;
    std::string getNome() const override;
    /** @throws std::invalid_argument Se a carga for negativa. */
    void setCargaHoraria(int cargaHoraria) override;
    int getCargaHoraria() const override;

    /**
     * @brief Ignora nulos e duplicados por ponteiro.
     * @throws std::invalid_argument Se o vinculo formar um ciclo.
     */
    void addPreRequisito(Disciplina* disciplina) override;
    void removePreRequisito(Disciplina* disciplina) override;
    std::vector<Disciplina*> getPreRequisitos() const override;
};

#endif
