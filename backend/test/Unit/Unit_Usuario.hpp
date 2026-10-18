#ifndef UNIT_USUARIO_HPP
#define UNIT_USUARIO_HPP

/*!
 * @brief Classe com testes unitários para a classe Usuario.
 */
class Unit_Usuario {
public:
    /*! @brief Testa o construtor por default de UsuarioImpl. 
     * * @return true se o teste passou. 
     */
    static bool defaultConstructor(void);
    
    /*! @brief Testa o construtor parametrizado de UsuarioImpl. 
     * * @return true se o teste passou. 
     */
    static bool constructor(void);

    /*! @brief Testa o destrutor de UsuarioImpl. 
     * * @return true se o teste passou. 
     */
    static bool destructor(void);

    /*! @brief Testa o método getId de UsuarioImpl. 
     * * @return true se o teste passou. 
     */
    static bool getId(void);

    /*! @brief Testa o método setId de UsuarioImpl. 
     * * @return true se o teste passou. 
     */
    static bool setId(void);

    /*! @brief Testa o método getNome de UsuarioImpl. 
     * * @return true se o teste passou. 
     */
    static bool getNome(void);

    /*! @brief Testa o método setNome de UsuarioImpl. 
     * * @return true se o teste passou. 
     */
    static bool setNome(void);

    /*! @brief Testa o método getEmail de UsuarioImpl. 
     * * @return true se o teste passou. 
     */
    static bool getEmail(void);

    /*! @brief Testa o método setEmail de UsuarioImpl. 
     * * @return true se o teste passou. 
     */
    static bool setEmail(void);

    /*! @brief Testa o método setSenha de UsuarioImpl. 
     * * @return true se o teste passou. 
     */
    static bool setSenha(void);

    /*! @brief Testa o método autenticar de UsuarioImpl. 
     * * @return true se o teste passou. 
     */
    static bool autenticar(void);

    /*! @brief Testa o construtor por cópia de UsuarioImpl. 
     * * @return true se o teste passou. 
     */
    static bool copyConstructor(void);

    /*! @brief Testa o operador de atribuição (=) de UsuarioImpl. 
     * * @return true se o teste passou. 
     */
    static bool assignmentOperator(void);

    /*! @brief Executa todos os testes unitários da classe Usuario. 
     * * @return true se todos passaram. 
     */
    static bool regressiveTest(void);
};

#endif