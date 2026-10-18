#ifndef ACADEMICSYSTEM_HPP
#define ACADEMICSYSTEM_HPP

#include <string>

class Usuario;
class Aluno;
class Coordenador;
class Curso;
class Disciplina;

/*!
 * @brief Esta classe representa a interface da plataforma: a classe geral que
 * contém todos os usuários, cursos e disciplinas.
 */
class AcademicSystem {
public:
    /*! @brief Destrutor. */
    virtual ~AcademicSystem() {}

    /*! @brief Define o nome. 
     * * @param nome o nome da plataforma. 
     */
    virtual void setNome(std::string) = 0;
    /*! @brief Retorna o nome. 
     * *@return string - o nome da plataforma. 
     */
    virtual std::string getNome() const = 0;

    /*! @brief Adiciona um usuário. 
     * * @param u o usuário. 
     */
    virtual void add(Usuario*) = 0;
    /*! @brief Adiciona um curso. 
     * * @param c o curso. 
     */
    virtual void add(Curso*) = 0;
    /*! @brief Adiciona uma disciplina. 
     * * @param d a disciplina. 
     */
    virtual void add(Disciplina*) = 0;

    /*! @brief Desvincula um usuário. 
     * * @param u o usuário. 
     */
    virtual void remove(Usuario*) = 0;

    /*! @brief Desvincula um curso. 
     * * @param c o curso. 
     */
    virtual void remove(Curso*) = 0;

    /*! @brief Desvincula uma disciplina. 
     * * @param d a disciplina. 
     */
    virtual void remove(Disciplina*) = 0;

    /*!
     * @brief Cadastra um novo aluno.
     * @param nome nome do aluno.
     * @param email e-mail do aluno.
     * @param senha senha.
     * @param curso um curso já cadastrado na plataforma.
     * @param periodo período/semestre atual.
     * @return Aluno* - o novo aluno.
     */
    virtual Aluno* cadastrarAluno(std::string nome, std::string email, std::string senha,
                                  Curso* curso, int periodo) = 0;

    /*!
     * @brief Cadastra um novo coordenador.
     * @param nome nome do coordenador.
     * @param email e-mail do coordenador.
     * @param senha senha.
     * @param curso um curso já cadastrado na plataforma.
     * @return Coordenador* - o novo coordenador.
     */
    virtual Coordenador* cadastrarCoordenador(std::string nome, std::string email, std::string senha,
                                              Curso* curso) = 0;

    /*!
     * @brief RF01: Autentica um usuário.
     * @param email e-mail do usuário.
     * @param senha senha.
     * @return Usuario* - o usuário autenticado.
     */
    virtual Usuario* autenticar(std::string email, std::string senha) = 0;

    /*! @brief Busca um curso pelo nome. 
     * * @param nome o nome do curso. 
     * * @return Curso* - o Curso. 
     */
    virtual Curso* buscarCurso(std::string nome) = 0;

    /*! @brief Busca uma disciplina pelo código. 
     * * @param codigo o código da disciplina. 
     * * @return Disciplina* - a Disciplina. 
     */
    virtual Disciplina* buscarDisciplina(std::string codigo) = 0;

    /*! @brief Busca um usuário pelo e-mail. 
     * * @param email o e-mail do usuário. 
     * * @return Usuario* - o Usuario. 
     */
    virtual Usuario* buscarUsuario(std::string email) = 0;
};

#endif