#ifndef ALUNOIMPL_H
#define ALUNOIMPL_H

#include "Aluno.hpp"
#include "UsuarioImpl.hpp"
#include <string>
#include <vector>


class AlunoImpl : public UsuarioImpl, public Aluno {
protected:
    Curso* curso; //objetoi do curso
    int periodoAtual;
    std::vector<Disciplina*> concluidas;
    std::vector<Disciplina*> emAndamento;

    bool preRequisitosCumpridos(Disciplina* d) const;
    static bool contem(const std::vector<Disciplina*>& v, Disciplina* d);
public:
    AlunoImpl();
   
    AlunoImpl(int id, std::string nome, std::string email, std::string senha,
              Curso* curso, int periodoAtual);
    virtual ~AlunoImpl();

    void setCurso(Curso* curso) override;
    Curso* getCurso() const override;
    void setPeriodoAtual(int periodo) override;
    int getPeriodoAtual() const override;
    void marcarConcluida(Disciplina* d) override;
    void desmarcarConcluida(Disciplina* d) override;
    void marcarEmAndamento(Disciplina* d) override;
    void desmarcarEmAndamento(Disciplina* d) override;
    std::vector<Disciplina*> getConcluidas() const override;
    std::vector<Disciplina*> getEmAndamento() const override;
    std::vector<Disciplina*> getDisponiveis() const override;
    EstadoDisciplina getEstado(Disciplina* d) const override;
    double getProgresso() const override;

    AlunoImpl(const AlunoImpl& a);
    AlunoImpl& operator=(const AlunoImpl& a);
};

#endif
