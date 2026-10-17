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
};

#endif
