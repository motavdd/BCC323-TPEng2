#ifndef ALUNO_H
#define ALUNO_H

#include "Usuario.hpp"
#include <vector>

class Curso;
class Disciplina;


enum class EstadoDisciplina {
    Concluida,
    EmAndamento,
    Disponivel,
    Bloqueada //caso não tenha os ppre requisitos
};

class Aluno : public virtual Usuario {
public:
    virtual ~Aluno() {}

    //getters setters
    virtual void setCurso(Curso*) = 0;
    virtual Curso* getCurso() const = 0;

    virtual void setPeriodoAtual(int) = 0;
    virtual int getPeriodoAtual() const = 0;

    virtual void marcarConcluida(Disciplina*) = 0;
    virtual void desmarcarConcluida(Disciplina*) = 0;

    virtual void marcarEmAndamento(Disciplina*) = 0;
    virtual void desmarcarEmAndamento(Disciplina*) = 0;

    virtual std::vector<Disciplina*> getConcluidas() const = 0;

    virtual std::vector<Disciplina*> getEmAndamento() const = 0;

    virtual std::vector<Disciplina*> getDisponiveis() const = 0;
    
    virtual EstadoDisciplina getEstado(Disciplina*) const = 0;
    virtual double getProgresso() const = 0;
};

#endif
