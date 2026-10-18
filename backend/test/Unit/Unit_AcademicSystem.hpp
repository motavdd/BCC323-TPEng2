#ifndef UNIT_ACADEMICSYSTEM_HPP
#define UNIT_ACADEMICSYSTEM_HPP

/*!
 * @brief Classe com testes unitários para a classe AcademicSystem.
 */
class Unit_AcademicSystem {
public:
    /*! @brief Testa o construtor por default de AcademicSystemImpl. 
     * * @return true se o teste passou. 
     */
    static bool defaultConstructor(void);

    /*! @brief Testa o construtor parametrizado de AcademicSystemImpl. 
     * * @return true se o teste passou. 
     */
    static bool constructor(void);

    /*! @brief Testa o destrutor de AcademicSystemImpl. 
     * * @return true se o teste passou. 
     */
    static bool destructor(void);

    /*! @brief Testa o método getNome de AcademicSystemImpl. 
     * * @return true se o teste passou. 
     */
    static bool getNome(void);

    /*! @brief Testa o método setNome de AcademicSystemImpl. 
     * * @return true se o teste passou. 
     */
    static bool setNome(void);

    /*! @brief Testa a adição de Usuarios em AcademicSystemImpl. 
     * * @return true se o teste passou. 
     */
    static bool addUsuario(void);

    /*! @brief Testa a adição de Cursos em AcademicSystemImpl. 
     * * @return true se o teste passou. 
     */
    static bool addCurso(void);

    /*! @brief Testa a adição de Disciplinas em AcademicSystemImpl. 
     * * @return true se o teste passou. 
     */
    static bool addDisciplina(void);

    /*! @brief Testa a remoção de Usuarios de AcademicSystemImpl. 
     * * @return true se o teste passou. 
     */
    static bool removeUsuario(void);

    /*! @brief Testa a remoção de Cursos de AcademicSystemImpl. 
     * * @return true se o teste passou. 
     */
    static bool removeCurso(void);

    /*! @brief Testa a remoção de Disciplinas de AcademicSystemImpl. 
     * * @return true se o teste passou. 
     */
    static bool removeDisciplina(void);

    /*! @brief Testa o cadastro bem sucedido de Alunos. 
     * * @return true se o teste passou. 
     */
    static bool cadastrarAluno(void);

    /*! @brief Testa a recusa de cadastro de Alunos. 
     * * @return true se o teste passou. 
     */
    static bool cadastrarAlunoInvalido(void);

    /*! @brief Testa o cadastro bem sucedido de Coordenadores. 
     * * @return true se o teste passou. 
     */
    static bool cadastrarCoordenador(void);

    /*! @brief Testa a recusa no cadastro de Coordenadores. 
     * * @return true se o teste passou. 
     */
    static bool cadastrarCoordenadorInvalido(void);

    /*! @brief Testa o método autenticar de AcademicSystemImpl. 
     * * @return true se o teste passou. 
     */
    static bool autenticar(void);

    /*! @brief Testa o método buscarCurso de AcademicSystemImpl. 
     * * @return true se o teste passou. 
     */
    static bool buscarCurso(void);

    /*! @brief Testa o método buscarDisciplina de AcademicSystemImpl. 
     * * @return true se o teste passou. 
     */
    static bool buscarDisciplina(void);

    /*! @brief Testa o método buscarUsuario de AcademicSystemImpl. 
     * * @return true se o teste passou. 
     */
    static bool buscarUsuario(void);
    /*! @brief Executa todos os testes unitários da classe AcademicSystem (teste de regressão). 
     * * @return true se todos passaram. 
     */
    static bool regressiveTest(void);
};

#endif