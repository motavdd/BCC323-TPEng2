#ifndef COORDENADOR_HPP
#define COORDENADOR_HPP

#include "domain/Usuario.hpp"

class Curso;

/*!
 * @brief Esta classe representa a interface do classe Coordenador.
 */
class Coordenador : public virtual Usuario {
public:
    /*! @brief Destrutor */
    virtual ~Coordenador() {}

    /*! @brief Configurar o Curso. 
     * * @param curso o Curso 
     */
    virtual void setCurso(Curso*) = 0;

    /*! @brief Retorna o Curso
     * * @return Curso* - o Curso. 
     */
    virtual Curso* getCurso() const = 0;
};

#endif
