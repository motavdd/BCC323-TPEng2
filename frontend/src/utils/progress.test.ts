import { describe, expect, it } from 'vitest';
import type { Evaluation, Subject } from '../services/api';
import {
  categoryOf,
  clampPercent,
  formatPercent,
  groupSubjectsByCategory,
  progressPercent,
} from './progress';

function subject(id: string, name: string, period: number, hours = 60): Subject {
  return { id, name, period, hours, prerequisites: [], requirementsKnown: true };
}

const subjects: Subject[] = [
  subject('BCC202', 'Estrutura de Dados I', 2, 90),
  subject('BCC201', 'Introdução à Programação', 1, 90),
  subject('MTM122', 'Cálculo I', 1, 90),
  subject('BCC203', 'Estrutura de Dados II', 3),
  subject('BCC221', 'Programação Orientada a Objetos', 3),
  subject('BCC101', 'Matemática Discreta', 1),
];

const states: Evaluation['subjects'] = {
  BCC201: { state: 'completed', missing: [] },
  MTM122: { state: 'completed', missing: [] },
  BCC202: { state: 'in_progress', missing: [] },
  BCC101: { state: 'available', missing: [] },
  BCC203: { state: 'blocked', missing: ['BCC202'] },
  BCC221: { state: 'unknown', missing: [] },
};

describe('Cálculo do progresso', () => {
  it('calcula o percentual pela carga horária concluída', () => {
    expect(progressPercent(180, 720)).toBe(25);
    expect(progressPercent(60, 180)).toBeCloseTo(100 / 3);
  });

  it('retorna 0 sem disciplinas concluídas', () => {
    expect(progressPercent(0, 720)).toBe(0);
  });

  it('retorna 100 com o curso completo', () => {
    expect(progressPercent(720, 720)).toBe(100);
  });

  it('não divide por zero quando a matriz não tem carga horária', () => {
    expect(progressPercent(0, 0)).toBe(0);
    expect(progressPercent(60, 0)).toBe(0);
  });

  it('mantém o resultado entre 0 e 100 com valores inválidos', () => {
    expect(progressPercent(900, 720)).toBe(100);
    expect(progressPercent(-60, 720)).toBe(0);
    expect(progressPercent(Number.NaN, 720)).toBe(0);
    expect(clampPercent(Number.POSITIVE_INFINITY)).toBe(0);
  });

  it('formata o percentual sem arredondar para cima', () => {
    expect(formatPercent(0)).toBe('0%');
    expect(formatPercent(33.33)).toBe('33%');
    expect(formatPercent(99.9)).toBe('99%');
    expect(formatPercent(100)).toBe('100%');
    expect(formatPercent(0.5)).toBe('0,5%');
  });
});

describe('Filtragem de status', () => {
  it('converte cada estado da API na sua categoria', () => {
    expect(categoryOf('completed')).toBe('completed');
    expect(categoryOf('in_progress')).toBe('in_progress');
    expect(categoryOf('available')).toBe('available');
    expect(categoryOf('blocked')).toBe('locked');
    expect(categoryOf('unknown')).toBe('locked');
    expect(categoryOf(undefined)).toBe('locked');
  });

  it('separa as disciplinas por categoria sem perder nenhuma', () => {
    const groups = groupSubjectsByCategory(subjects, states);
    expect(groups.completed.map((s) => s.id)).toEqual(['MTM122', 'BCC201']);
    expect(groups.in_progress.map((s) => s.id)).toEqual(['BCC202']);
    expect(groups.available.map((s) => s.id)).toEqual(['BCC101']);
    expect(groups.locked.map((s) => s.id)).toEqual(['BCC203', 'BCC221']);
    const total = Object.values(groups).reduce((sum, list) => sum + list.length, 0);
    expect(total).toBe(subjects.length);
  });

  it('ordena cada categoria por período e depois pelo nome', () => {
    const groups = groupSubjectsByCategory(subjects, {
      ...states, BCC202: { state: 'completed', missing: [] },
    });
    expect(groups.completed.map((s) => s.id)).toEqual(['MTM122', 'BCC201', 'BCC202']);
  });

  it('trata disciplina sem estado como ainda não liberada', () => {
    const groups = groupSubjectsByCategory([subject('ELT002', 'Eletiva', 7)], {});
    expect(groups.locked.map((s) => s.id)).toEqual(['ELT002']);
  });

  it('não altera a ordem da lista original', () => {
    const original = subjects.map((s) => s.id);
    groupSubjectsByCategory(subjects, states);
    expect(subjects.map((s) => s.id)).toEqual(original);
  });
});
