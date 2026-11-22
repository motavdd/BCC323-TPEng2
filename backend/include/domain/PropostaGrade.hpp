#ifndef PROPOSTAGRADE_HPP
#define PROPOSTAGRADE_HPP

#include "Turma.hpp"
#include <string>
#include <vector>

class Aluno;

/*!
 * @brief Resultado da tentativa de adicionar uma turma à proposta de grade.
 */
enum class ResultadoAdicao {
    Adicionada,             /*A turma foi adicionada na proposta. */
    TurmaInvalida,          /* Turma::validar() recusou os dados (código vazio, horário inválido...) */
    SemCurso,               /* A proposta não tem aluno ou o aluno não tem curso. */
    DisciplinaInexistente,  /* O código da disciplina não está na matriz do curso do aluno. */
    DisciplinaJaNaGrade,    /* Já existe uma turma dessa disciplina na proposta. */
    DisciplinaIndisponivel, /* Concluída, em andamento ou com pré-requisitos pendentes. */
    ConflitoDeHorario       /* Os horários colidem com os de outra turma da proposta. */
};

/*!
 * @brief Esta classe representa a interface da proposta de grade horária de um aluno.
 * Guarda as turmas que o aluno pretende cursar em um período letivo e só aceita
 * disciplinas disponíveis para ele e sem conflito de horário.
 */
class PropostaGrade {
public:
    /*! @brief Destrutor padrão para a interface PropostaGrade. */
    virtual ~PropostaGrade() {}

    /*! @brief Configura o aluno dono da proposta (não assume sua propriedade).
     * @param aluno o Aluno.
     */
    virtual void setAluno(Aluno*) = 0;
    /*! @brief Retorna o aluno dono da proposta.
     * @return Aluno* - o Aluno.
     */
    virtual Aluno* getAluno() const = 0;

    /*! @brief Configura o período letivo da proposta (ex.: "2026/2").
     * @param periodoLetivo o período letivo.
     */
    virtual void setPeriodoLetivo(std::string) = 0;

    /*! @brief Retorna o período letivo da proposta.
     * @return string - o período letivo.
     */
    virtual std::string getPeriodoLetivo() const = 0;

    /*!
     * @brief Tenta adicionar uma turma à proposta.
     * As verificações seguem a ordem do enum ResultadoAdicao; a turma só entra se todas passarem.
     * @param turma a turma desejada.
     * @return ResultadoAdicao - Adicionada ou o motivo da recusa.
     */
    virtual ResultadoAdicao adicionarTurma(const bcc323::Turma&) = 0;

    /*! @brief Remove da proposta a turma de uma disciplina.
     * @param disciplinaCodigo o código da disciplina.
     * @return bool - true se havia turma dessa disciplina na proposta.
     */
    virtual bool removerTurma(const std::string& disciplinaCodigo) = 0;

    /*! @brief Remove todas as turmas da proposta. */
    virtual void limpar() = 0;

    /*! @brief Retorna as turmas da proposta, na ordem em que foram adicionadas.
     * @return vector - vetor de Turmas (cópias).
     */
    virtual std::vector<bcc323::Turma> getTurmas() const = 0;

    /*! @brief Verifica se uma turma colide em horário com alguma turma da proposta.
     * @param turma a turma a verificar.
     * @return bool - true se há conflito.
     */
    virtual bool temConflito(const bcc323::Turma&) const = 0;

    /*! @brief Retorna as turmas da proposta que colidem em horário com a turma dada.
     * @param turma a turma a verificar.
     * @return vector - vetor de Turmas em conflito.
     */
    virtual std::vector<bcc323::Turma> getConflitantes(const bcc323::Turma&) const = 0;

    /*! @brief Retorna a soma da carga horária das disciplinas das turmas da proposta.
     * @return int - a carga horária total (0 se não houver aluno, curso ou turmas).
     */
    virtual int getCargaHorariaTotal() const = 0;
};

#endif