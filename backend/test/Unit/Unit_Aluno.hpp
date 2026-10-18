#ifndef UNIT_ALUNO_HPP
#define UNIT_ALUNO_HPP

/*!
 * @brief Classe com testes unitários estáticos para a classe Aluno.
 */
class Unit_Aluno {
public:
    /*! @brief Testa o construtor por default de AlunoImpl. 
     * * @return true se o teste passou. 
     */
    static bool defaultConstructor(void);

    /*! @brief Testa o construtor parametrizado de AlunoImpl. 
     * * @return true se o teste passou. 
     */
    static bool constructor(void);

    /*! @brief Testa o destrutor de AlunoImpl. 
     * * @return true se o teste passou. 
     */
    static bool destructor(void);

    /*! @brief Testa o método getCurso de AlunoImpl. 
     * * @return true se o teste passou. 
     */
    static bool getCurso(void);

    /*! @brief Testa o método setCurso de AlunoImpl. 
     * * @return true se o teste passou. 
     */
    static bool setCurso(void);

    /*! @brief Testa o método getPeriodoAtual de AlunoImpl. 
     * * @return true se o teste passou. 
     */
    static bool getPeriodoAtual(void);

    /*! @brief Testa o método setPeriodoAtual de AlunoImpl. 
     * * @return true se o teste passou. 
     */
    static bool setPeriodoAtual(void);

    /*! @brief Testa o método auxiliar contem de AlunoImpl. 
     * * @return true se o teste passou. 
     */
    static bool contem(void);

    /*! @brief Testa o método auxiliar preRequisitosCumpridos de AlunoImpl. 
     * * @return true se o teste passou. 
     */
    static bool preRequisitosCumpridos(void);

    /*! @brief Testa o método marcarConcluida de AlunoImpl. 
     * * @return true se o teste passou. 
     */
    static bool marcarConcluida(void);

    /*! @brief Testa o método desmarcarConcluida de AlunoImpl. 
     * * @return true se o teste passou. 
     */
    static bool desmarcarConcluida(void);
    
    /*! @brief Testa o método marcarEmAndamento de AlunoImpl. 
     * * @return true se o teste passou. 
     */
    static bool marcarEmAndamento(void);

    /*! @brief Testa o método desmarcarEmAndamento de AlunoImpl. 
     * * @return true se o teste passou. 
     */
    static bool desmarcarEmAndamento(void);

    /*! @brief Testa o método getConcluidas de AlunoImpl. 
     * * @return true se o teste passou. 
     */
    static bool getConcluidas(void);

    /*! @brief Testa o método getEmAndamento de AlunoImpl. 
     * * @return true se o teste passou. 
     */
    static bool getEmAndamento(void);
    /*! @brief Testa o método getDisponiveis de AlunoImpl. 
     * * @return true se o teste passou. 
     */
    static bool getDisponiveis(void);

    /*! @brief Testa o método getEstado de AlunoImpl. 
     * * @return true se o teste passou. 
     */
    static bool getEstado(void);

    /*! @brief Testa o método getProgresso de AlunoImpl. 
     * * @return true se o teste passou. 
     */
    static bool getProgresso(void);

    /*! @brief Testa o construtor por cópia de AlunoImpl. 
     * * @return true se o teste passou. 
     */
    static bool copyConstructor(void);

    /*! @brief Testa o operador de atribuição (=) de AlunoImpl. 
     * * @return true se o teste passou. 
     */
    static bool assignmentOperator(void);

    /*! @brief Executa todos os testes unitários da classe Aluno (teste de regressão). 
     * * @return true se todos passaram. 
     */
    static bool regressiveTest(void);
};

#endif