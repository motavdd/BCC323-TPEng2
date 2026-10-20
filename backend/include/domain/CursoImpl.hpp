#ifndef CURSOIMPL_HPP
#define CURSOIMPL_HPP

#include "domain/Curso.hpp"

/**
 * @brief Implementação concreta de um curso.
 *
 * As disciplinas são referências não proprietárias. Devem permanecer vivas
 * enquanto estiverem vinculadas ao curso; o curso não as destrói.
 */
class CursoImpl : public Curso {
protected:
    std::string nome;
    std::vector<Disciplina*> disciplinas;

public:
    CursoImpl() = default;
    explicit CursoImpl(std::string nome);
    ~CursoImpl() override = default;

    void setNome(std::string nome) override;
    std::string getNome() const override;

    /** @brief Ignora referências nulas e disciplinas já vinculadas. */
    void add(Disciplina* disciplina) override;
    void remove(Disciplina* disciplina) override;
    std::vector<Disciplina*> getDisciplinas() const override;
    int getCargaTotal() const override;

    // A cópia compartilha referências às mesmas disciplinas.
    CursoImpl(const CursoImpl&) = default;
    CursoImpl& operator=(const CursoImpl&) = default;
};

#endif
