#ifndef ALUNO_HPP
#define ALUNO_HPP

#include "Usuario.hpp"
#include <vector>

class Curso;
class Disciplina;

/*!
 * @brief Estado de uma disciplina sob a perspectiva de um estudante.
 */
enum class EstadoDisciplina {
    Concluida,
    EmAndamento,
    Disponivel,
    Bloqueada //caso não tenha os ppre requisitos
};

/*!
 * @brief Esta classe representa a interface de um aluno.
 */
class Aluno : public virtual Usuario {
public:
    /*! @brief Destrutor padrão para a interface Aluno. */
    virtual ~Aluno() {}

    //getters setters

    /*! @brief Configura o Curso do Aluno. 
     * * @param curso o Curso. 
     */
    virtual void setCurso(Curso*) = 0;
    /*! @brief Retorna o Curso do Aluno. 
     * * @return Curso* - o Curso. 
     */
    virtual Curso* getCurso() const = 0;

    /*! @brief Configura o período do Aluno. 
     * * @param periodo o período atual. 
     */
    virtual void setPeriodoAtual(int) = 0;
    /*! @brief Retorna o período do Aluno. 
     * * @return int - o período atual. 
     */
    virtual int getPeriodoAtual() const = 0;

    /*! @brief Marca a disciplina como concluída. 
     * * @param d a Disciplina. 
     */
    virtual void marcarConcluida(Disciplina*) = 0;
    /*! @brief Desmarca a disciplina concluída . 
     * * @param d a Disciplina. 
     */
    virtual void desmarcarConcluida(Disciplina*) = 0;

    /*! @brief Marca a disciplina como em andamento. 
     * * @param d a Disciplina. 
     */
    virtual void marcarEmAndamento(Disciplina*) = 0;
    /*! @brief Desmarca a disciplina em andamento. 
     * * @param d a Disciplina. 
     */
    virtual void desmarcarEmAndamento(Disciplina*) = 0;

    /*! @brief Retorna o vetor de disciplinas concluídas. 
     * * @return vector - vetor de Disciplinas. 
     */
    virtual std::vector<Disciplina*> getConcluidas() const = 0;

    /*! @brief Retorna o vetor de disciplinas em andamento. 
     * * @return vector - vetor de Disciplinas. 
     */
    virtual std::vector<Disciplina*> getEmAndamento() const = 0;

    /*!
     * @brief Retorna as disciplinas disponíveis: do curso, não concluídas, não em andamento e com todos os pré-requisitos atendidos.
     * @return vector - vetor de Disciplinas.
     */
    virtual std::vector<Disciplina*> getDisponiveis() const = 0;
    
    /*! @brief Retorna o status de uma disciplina do Aluno. 
     * * @param d a Disciplina. 
     * * @return EstadoDisciplina 
     */
    virtual EstadoDisciplina getEstado(Disciplina*) const = 0;

    /*! @brief Retorna o progresso do Aluno. 
     * * @return double - poercentagem do progresso. 
     */
    virtual double getProgresso() const = 0;

     /*! @brief Verifica se a disciplina consta no histórico (concluída).
     * @param d a Disciplina.
     * @return bool - true se a disciplina está concluída (false para nullptr).
     */
    virtual bool isConcluida(Disciplina*) const = 0;

    /*! @brief Verifica se todos os pré-requisitos da disciplina estão concluídos.
     * @param d a Disciplina.
     * @return bool - true se todos foram cumpridos (false para nullptr).
     */
    virtual bool preRequisitosAtendidos(Disciplina*) const = 0;

    /*! @brief Retorna os pré-requisitos da disciplina que o aluno ainda não concluiu.
     * @param d a Disciplina.
     * @return vector - vetor de Disciplinas pendentes (vazio se nenhum ou se d for nullptr).
     */
    virtual std::vector<Disciplina*> getPreRequisitosPendentes(Disciplina*) const = 0;

    /*!
     * @brief Registra uma disciplina no histórico validando as regras do curso.
     * A disciplina só é registrada se pertencer à matriz do curso do aluno e se todos
     * os seus pré-requisitos já estiverem concluídos. Se já estiver concluída, nada muda.
     * Ao ser registrada, deixa de constar como "em andamento".
     * @param d a Disciplina.
     * @return bool - true se a disciplina consta no histórico após a chamada; false se foi rejeitada.
     */
    virtual bool registrarConcluida(Disciplina*) = 0;

     /*!
     * @brief Remove uma disciplina do histórico sem quebrar a consistência dele.
     * É recusada se a disciplina não estiver concluída ou se outra disciplina
     * concluída a tem como pré-requisito.
     * @param d a Disciplina.
     * @return bool - true se foi removida.
     */
    virtual bool removerConcluida(Disciplina*) = 0;

    /*!
     * @brief Registra várias disciplinas de uma vez.
     * A ordem da lista não importa: pré-requisitos presentes na mesma lista são
     * considerados. Cada disciplina passa pelas mesmas regras de registrarConcluida.
     * @param disciplinas as disciplinas a registrar.
     * @return vector - as disciplinas rejeitadas, na ordem em que foram enviadas.
     */
    virtual std::vector<Disciplina*> registrarHistorico(const std::vector<Disciplina*>& disciplinas) = 0;
};

#endif
