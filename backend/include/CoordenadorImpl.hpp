#ifndef COORDENADORIMPL_HPP
#define COORDENADORIMPL_HPP

#include "Coordenador.hpp"
#include "UsuarioImpl.hpp"
#include <string>

/*!
 * @brief Essa classe representa a implementação da classe concreta do Coordenador.
 */
class CoordenadorImpl : public UsuarioImpl, public Coordenador {
protected:
    /*! Curso */
    Curso* curso;
public:
    /*! @brief Construtor por default */
    CoordenadorImpl();
    /*!
     * @brief Construtor Parametrizado.
     * @param id Id.
     * @param nome Nome do Coordenador.
     * @param email Email do Coordenador.
     * @param senha Senha.
     * @param curso Curso.
     */
    CoordenadorImpl(int id, std::string nome, std::string email, std::string senha, Curso* curso);

    /*! @brief Destrutor. */
    virtual ~CoordenadorImpl();

    void setCurso(Curso* curso) override;
    Curso* getCurso() const override;

    /*! @brief Construtor por copia. 
     * * @param c o coordenador a ser copiado 
     */
    CoordenadorImpl(const CoordenadorImpl& c);

    /*! @brief Operador de igualdade. 
     * * @param c o coordenador a ser copiado. 
     * * @return CoordenadorImpl& 
     */
    CoordenadorImpl& operator=(const CoordenadorImpl& c);
};

#endif
