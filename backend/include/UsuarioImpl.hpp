#ifndef USUARIOIMPL_H
#define USUARIOIMPL_H

#include "Usuario.hpp"

class UsuarioImpl : public virtual Usuario{
protected:
    int id;
    std::string nome;
    std::string email;
    std::size_t senhaHash;
public:
    UsuarioImpl();

    UsuarioImpl(int id, std::string name, std::string email, std::string password);

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
    UsuarioImpl(const UsuarioImpl& u);
    UsuarioImpl& operator=(const UsuarioImpl& u);
};


#endif