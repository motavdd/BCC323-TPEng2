#ifndef ALUNOIMPL_HPP
#define ALUNOIMPL_HPP

#include "domain/Aluno.hpp"
#include "domain/UsuarioImpl.hpp"
#include <string>
#include <vector>

/*!
 * @brief Esta classe representa a implementação da Classe Concreta de Aluno.
 */
class AlunoImpl : public UsuarioImpl, public Aluno {
protected:
    /*! Objeto Curso do Aluno. */
    Curso* curso;
    /*! Período atual. */
    int periodoAtual;
    /*! Vetor de disciplinas concluídas. */
    std::vector<Disciplina*> concluidas;
    /*! Vetor de disciplinas em andamento */
    std::vector<Disciplina*> emAndamento;

    /*! @brief Retorna true se todos os pré-requisitos da disciplina estiverem concluídos. 
     * * @param d a Disciplina. 
     */
    bool preRequisitosCumpridos(Disciplina* d) const;

    /*! @brief Retorna true se a Disciplina estiver no vetor */
    static bool contem(const std::vector<Disciplina*>& v, Disciplina* d);
public:
    /*! @brief Construtor por default. */
    AlunoImpl();
    
    /*!
     * @brief Construtor parametrizado.
     * @param id id do Aluno.
     * @param nome nome do Aluno.
     * @param email email do Aluno.
     * @param senha senha do Aluno.
     * @param curso curso do Aluno.
     * @param periodoAtual atual período do Aluno.
     */
    AlunoImpl(int id, std::string nome, std::string email, std::string senha,
              Curso* curso, int periodoAtual);

    /*! @brief Destrutor de Aluno */
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

    /*! @brief Construtor por cópia. 
     * * @param a é Aluno */
    AlunoImpl(const AlunoImpl& a);

    /*! @brief Operador de igualdade. 
     * * @param a é o Aluno. 
     * * @return AlunoImpl& 
     */
    AlunoImpl& operator=(const AlunoImpl& a);
};

#endif
