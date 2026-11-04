#ifndef USUARIOIMPL_H
#define USUARIOIMPL_H

#include "domain/Usuario.hpp"

/*!
 * @brief Essa classe representa a implementação da classe concreta Usuário.
 */
class UsuarioImpl : public virtual Usuario{
protected:
    /*! O id do usuário */
    int id;
    /*! o nome do Usuário */
    std::string nome;
    /*! Email do Usuário */
    std::string email;
    /*! código da senha */
    std::size_t senhaHash;
public:
    /*! @brief Construtor por default. */
    UsuarioImpl();

    /*!
     * @brief Construtor parametrizado.
     * @param id Id.
     * @param nome nome.
     * @param email email.
     * @param senha senha.
     */
    UsuarioImpl(int id, std::string name, std::string email, std::string password);

    /*! @brief Destrutor. */
    virtual ~UsuarioImpl();

    //Getters e Setters
    void setId(int id) override;
    int getId() const override;
    void setNome(std::string nome) override;
    std::string getNome() const override;
    void setEmail(std::string email) override;
    std::string getEmail() const override;
    void setSenha(std::string senha) override;
    bool autenticar(std::string senha) const override;

    //construtor de copia e operador de iguall
    /*! @brief Construtor por copia. 
     * * @param u o usuário copiado. */
    UsuarioImpl(const UsuarioImpl& u);
    
    /*! @brief Operador de igualdade. 
     * * @param u o usuario a ser copiado. 
     * * @return UsuarioImpl& 
     */
    UsuarioImpl& operator=(const UsuarioImpl& u);
};


#endif
