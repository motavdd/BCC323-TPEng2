#ifndef PROPOSTAGRADEIMPL_HPP
#define PROPOSTAGRADEIMPL_HPP

#include "PropostaGrade.hpp"
#include "Disciplina.hpp"
#include <string>
#include <vector>

/*!
 * @brief Esta classe representa a implementação concreta de uma PropostaGrade.
 * O aluno é uma referência não proprietária; as turmas são guardadas por valor.
 */
class PropostaGradeImpl : public PropostaGrade {
protected:
    /*! Aluno dono da proposta*/
    Aluno* aluno;
    /*! Período letivo da proposta. */
    std::string periodoLetivo;
    /*! Turmas escolhidas. */
    std::vector<bcc323::Turma> turmas;

    /*! @brief Retorna true se alguma aula de a colide com alguma aula de b.
     * Aulas encostadas (fim de uma == início da outra) não colidem.
     */
    static bool conflitam(const bcc323::Turma& a, const bcc323::Turma& b);
    /*! @brief Busca a disciplina pelo código na matriz do curso do aluno (nullptr se não houver). */
    Disciplina* buscarDisciplina(const std::string& codigo) const;
    /*! @brief Retorna a posição da turma da disciplina no vetor, ou -1. */
    int indiceDaDisciplina(const std::string& disciplinaCodigo) const;

public:
    /*! @brief Construtor por default. */
    PropostaGradeImpl();
    /*!
     * @brief Construtor parametrizado.
     * @param aluno o aluno dono da proposta.
     * @param periodoLetivo o período letivo (ex.: "2026/2").
     */
    PropostaGradeImpl(Aluno* aluno, std::string periodoLetivo);
    /*! @brief Destrutor (não destrói o aluno). */
    virtual ~PropostaGradeImpl();

    void setAluno(Aluno* aluno) override;
    Aluno* getAluno() const override;
    void setPeriodoLetivo(std::string periodoLetivo) override;
    std::string getPeriodoLetivo() const override;
    ResultadoAdicao adicionarTurma(const bcc323::Turma& turma) override;
    bool removerTurma(const std::string& disciplinaCodigo) override;
    void limpar() override;
    std::vector<bcc323::Turma> getTurmas() const override;
    bool temConflito(const bcc323::Turma& turma) const override;
    std::vector<bcc323::Turma> getConflitantes(const bcc323::Turma& turma) const override;
    int getCargaHorariaTotal() const override;

    /*! @brief Construtor por copia. 
     ** @param p a proposta a ser copiada. 
     */
    PropostaGradeImpl(const PropostaGradeImpl& p);

    /*! @brief Operador de igualdade. 
     * * @param p a proposta a ser copiada. 
     * * @return PropostaGradeImpl& 
     */
    PropostaGradeImpl& operator=(const PropostaGradeImpl& p);

    // da permissao a classe de teste para que faça testes unitarios
    friend class Unit_PropostaGrade;
};



#endif