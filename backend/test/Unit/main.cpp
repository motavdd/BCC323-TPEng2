#include "Unit_Usuario.hpp"
#include "Unit_Aluno.hpp"
#include "Unit_Coordenador.hpp"
#include "Unit_AcademicSystem.hpp"

#include <iostream>

int main(){
    std::cout << "INICIANDO TESTES UNITARIOS" << std::endl;

    //TESTES REGRESSIVOS DE USUARIO
    Unit_Usuario :: regressiveTest();
    std::cout << "Testes unitarios de Usuario passaram com sucesso!" << std::endl;

    //TESTES REGRESSIVOS DE ALUNO
    Unit_Aluno :: regressiveTest();
    std::cout << "Testes unitarios de Aluno passaram com sucesso!" << std::endl;

    //TESTES REGRESSIVOS DE COORDENADOR
    Unit_Coordenador :: regressiveTest();
    std::cout << "Testes unitarios de Coordenador passaram com sucesso!" << std::endl;

    //TESTES REGRESSIVOS DE AcademicSystem
    Unit_AcademicSystem :: regressiveTest();
    std::cout << "Testes unitarios de AcademicSystem passaram com sucesso!" << std::endl;

    std::cout << "TODOS OS TESTES PASSARAM COM SUCESSO!" << std::endl;

    return 0;
}