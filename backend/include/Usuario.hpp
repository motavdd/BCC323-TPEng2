#ifndef USUARIO_H
#define USUARIO_H

#include <string>

class Usuario {
public:
    virtual ~Usuario() {}

    //getters e settes
    virtual void setId(int) = 0;
    virtual int getId() const = 0;
    virtual void setNome(std::string) = 0;
    virtual std::string getNome() const = 0;
    virtual void setEmail(std::string) = 0;
    virtual std::string getEmail() const = 0;
    virtual void setSenha(std::string) = 0;
    virtual bool autenticar(std::string) const = 0;
};

#endif
