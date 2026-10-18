#ifndef ACADEMICSYSTEMIMPL_HPP
#define ACADEMICSYSTEMIMPL_HPP

#include "AcademicSystem.hpp" // interface
#include <string>
#include <vector>

/*!
 * @brief Esta classe representa a implementação concreta de um AcademicSystem.
 */
class AcademicSystemImpl : public AcademicSystem {
protected:
    /*! Nome da plataforma. */
    std::string nome;
    /*! Próximo ID a ser atribuído a um novo usuário. */
    int proximoId;
    /*! Vetor de usuários pertencentes ao sistema. */
    std::vector<Usuario*> usuarios;
    /*! Vetor de cursos pertencentes ao sistema. */
    std::vector<Curso*> cursos;
    /*! Vetor de disciplinas pertencentes ao sistema. */
    std::vector<Disciplina*> disciplinas;

public:
    /*! @brief Construtor por default. */
    AcademicSystemImpl();

    /*! @brief Construtor parametrizado. 
     * * @param nome o nome da plataforma. 
     */
    AcademicSystemImpl(std::string nome);

    /*! @brief Destrutor: deleta todos os usuários, cursos e disciplinas armazenados. */
    virtual ~AcademicSystemImpl();


    void setNome(std::string nome) override;
    std::string getNome() const override;
    void add(Usuario* u) override;
    void add(Curso* c) override;
    void add(Disciplina* d) override;
    void remove(Usuario* u) override;
    void remove(Curso* c) override;
    void remove(Disciplina* d) override;
    Aluno* cadastrarAluno(std::string nome, std::string email, std::string senha,
                          Curso* curso, int periodo) override;
    Coordenador* cadastrarCoordenador(std::string nome, std::string email, std::string senha,
                                      Curso* curso) override;
    Usuario* autenticar(std::string email, std::string senha) override;
    Curso* buscarCurso(std::string nome) override;
    Disciplina* buscarDisciplina(std::string codigo) override;
    Usuario* buscarUsuario(std::string email) override;

    friend class Unit_AcademicSystem;
};

#endif