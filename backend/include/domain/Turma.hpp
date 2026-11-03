#pragma once
#include <string>
#include <vector>
#include <stdexcept>

enum class DiaSemana {
    SEGUNDA,
    TERCA,
    QUARTA,
    QUINTA,
    SEXTA,
    SABADO
};

struct HorarioAula {
    DiaSemana dia;
    int horaInicio;
    int minutoInicio;
    int horaFim;
    int minutoFim;
    std::string sala;

    int inicioEmMinutos() const {
        return horaInicio * 60 + minutoInicio;
    }

    int fimEmMinutos() const {
        return horaFim * 60 + minutoFim;
    }

    void validar() const {
        if (horaInicio < 0 || horaInicio > 23 || horaFim < 0 || horaFim > 23 ||
            minutoInicio < 0 || minutoInicio > 59 || minutoFim < 0 || minutoFim > 59) {
            throw std::invalid_argument("Horario com valores fora dos limites de 24h.");
        }
        if (fimEmMinutos() <= inicioEmMinutos()) {
            throw std::invalid_argument("O horario de termino deve ser estritamente posterior ao de inicio.");
        }
    }
};

struct Turma {
    std::string codigo;
    std::string disciplinaCodigo;
    int vagasTotais{0};
    std::vector<HorarioAula> horarios;

    void validar() const {
        if (codigo.empty()) {
            throw std::invalid_argument("Codigo da turma obrigatorio.");
        }
        if (disciplinaCodigo.empty()) {
            throw std::invalid_argument("Codigo da disciplina obrigatorio.");
        }
        if (vagasTotais < 0) {
            throw std::invalid_argument("Numero de vagas nao pode ser negativo.");
        }
        for (const auto& h : horarios) {
            h.validar();
        }
    }
};
