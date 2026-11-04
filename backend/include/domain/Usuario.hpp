#ifndef USUARIO_H
#define USUARIO_H

#include <string>

/*!
 * @brief Essa classe representa a interface da classe Ususario
 */
class Usuario {
public:
    /*! @brief Destrtutor. */
    virtual ~Usuario() {}

    //getters e settes
    /*! @brief Configurar o id. 
     * * @param id o id do usuário. 
     */
    virtual void setId(int) = 0;
    /*! @brief Retorna o id. 
     * * @return int - o id do Usuário. 
     */
    virtual int getId() const = 0;

    /*! @brief Configura o nome. 
     * * @param nome o nome do Usuário. 
     */
    virtual void setNome(std::string) = 0;
    /*! @brief Retorna o nome. 
     * * @return string - nome do usuário. 
     */
    virtual std::string getNome() const = 0;

    /*! @brief Configura o email. 
     * * @param email o email do usuário. 
     */
    virtual void setEmail(std::string) = 0;
    /*! @brief Retorna o email. 
     * * @return string - o email do usuário. */
    virtual std::string getEmail() const = 0;

    /*!
     * @brief Configura a senha.
     * @param senha a senha.
     */
    virtual void setSenha(std::string) = 0;
    /*!
     * @brief verificar se a senha é a correta e autenticar.
     * @param senha a senha a ser verificada.
     * @return bool - retorna true se a senha for igual.
     */
    virtual bool autenticar(std::string) const = 0;
};

#endif
