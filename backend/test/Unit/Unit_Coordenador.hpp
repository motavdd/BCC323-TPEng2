#ifndef UNIT_COORDENADOR_HPP
#define UNIT_COORDENADOR_HPP

/*!
 * @brief Classe com testes unitários para a classe Coordenador.
 */
class Unit_Coordenador {
public:
    /*! @brief Testa o construtor por default de CoordenadorImpl. 
     * * @return true se o teste passou. 
     */
    static bool defaultConstructor(void);

    /*! @brief Testa o construtor parametrizado de CoordenadorImpl. 
     * * @return true se o teste passou. 
     */
    static bool constructor(void);

    /*! @brief Testa o destrutor de CoordenadorImpl. 
     * * @return true se o teste passou. 
     */
    static bool destructor(void);

    /*! @brief Testa o método getCurso de CoordenadorImpl. 
     * * @return true se o teste passou. 
     */
    static bool getCurso(void);

    /*! @brief Testa o método setCurso de CoordenadorImpl. 
     * * @return true se o teste passou. 
     */
    static bool setCurso(void);

    /*! @brief Testa o construtor por cópia de CoordenadorImpl. 
     * * @return true se o teste passou. 
     */
    static bool copyConstructor(void);

    /*! @brief Testa o operador de atribuição (=) de CoordenadorImpl. 
     * * @return true se o teste passou. 
     */
    static bool assignmentOperator(void);
    
    /*! @brief Executa todos os testes unitários da classe Coordenador (teste de regressão). 
     * * @return true se todos passaram. 
     */
    static bool regressiveTest(void);
};

#endif