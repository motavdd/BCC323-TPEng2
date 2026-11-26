import type { Evaluation, Subject } from '../services/api';

/** Estado de uma disciplina para o aluno, como a API devolve. */
export type SubjectState = Evaluation['subjects'][string]['state'];

/** Categorias da listagem de progresso (issue #74). */
export type ProgressCategory = 'completed' | 'in_progress' | 'available' | 'locked';

export const PROGRESS_CATEGORIES: ProgressCategory[] = ['completed', 'in_progress', 'available', 'locked'];

export const CATEGORY_LABELS: Record<ProgressCategory, string> = {
  completed: 'Concluídas',
  in_progress: 'Em andamento',
  available: 'Disponíveis',
  locked: 'Ainda não liberadas',
};

/**
 * Converte o estado da API na categoria exibida.
 * "blocked" (pré-requisito pendente) e "unknown" (requisitos a validar)
 * ficam juntos em "Ainda não liberadas"; estado ausente também.
 */
export function categoryOf(state: SubjectState | undefined): ProgressCategory {
  switch (state) {
    case 'completed': return 'completed';
    case 'in_progress': return 'in_progress';
    case 'available': return 'available';
    default: return 'locked';
  }
}

/**
 * Separa as disciplinas da matriz por categoria (filtro de status).
 * Cada lista sai ordenada por período e, dentro do período, pelo nome.
 */
export function groupSubjectsByCategory(
  subjects: Subject[],
  states: Evaluation['subjects'],
): Record<ProgressCategory, Subject[]> {
  const groups: Record<ProgressCategory, Subject[]> = {
    completed: [], in_progress: [], available: [], locked: [],
  };
  for (const subject of subjects) {
    groups[categoryOf(states[subject.id]?.state)].push(subject);
  }
  for (const category of PROGRESS_CATEGORIES) {
    groups[category].sort((a, b) => a.period - b.period || a.name.localeCompare(b.name, 'pt-BR'));
  }
  return groups;
}

/**
 * Percentual de progresso pela carga horária concluída, entre 0 e 100.
 * Retorna 0 quando a carga total é zero ou os valores são inválidos.
 */
export function progressPercent(completedHours: number, totalHours: number): number {
  if (!Number.isFinite(completedHours) || !Number.isFinite(totalHours) || totalHours <= 0) return 0;
  return clampPercent((100 * completedHours) / totalHours);
}

/** Limita um percentual ao intervalo de 0 a 100 (NaN vira 0). */
export function clampPercent(value: number): number {
  if (!Number.isFinite(value)) return 0;
  return Math.min(100, Math.max(0, value));
}

/** Formata o percentual para exibição: inteiro, exceto valores entre 0 e 1 (ex.: "0,5%"). */
export function formatPercent(value: number): string {
  const percent = clampPercent(value);
  if (percent > 0 && percent < 1) return `${percent.toFixed(1).replace('.', ',')}%`;
  return `${Math.floor(percent)}%`;
}
