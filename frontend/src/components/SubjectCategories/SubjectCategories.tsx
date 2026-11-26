import "./SubjectCategories.css";
import { useState } from "react";
import type { Evaluation, Subject } from "../../services/api";
import {
  CATEGORY_LABELS,
  PROGRESS_CATEGORIES,
  groupSubjectsByCategory,
  type ProgressCategory,
} from "../../utils/progress";

type Props = {
  subjects: Subject[];
  states: Evaluation["subjects"];
  /** Abre a tela de detalhes da disciplina. */
  onOpen: (subjectId: string) => void;
  initial?: ProgressCategory;
};

const EMPTY_MESSAGES: Record<ProgressCategory, string> = {
  completed: "Você ainda não registrou disciplinas concluídas. Marque-as na matriz curricular.",
  in_progress: "Nenhuma disciplina em andamento no momento.",
  available: "Nenhuma disciplina disponível agora. Confira os pré-requisitos na matriz.",
  locked: "Todas as disciplinas já estão liberadas.",
};

/** Listagem das disciplinas separadas por categoria, com filtro por status (issue #74). */
export function SubjectCategories({ subjects, states, onOpen, initial = "available" }: Props) {
  const [selected, setSelected] = useState<ProgressCategory>(initial);
  const groups = groupSubjectsByCategory(subjects, states);
  const list = groups[selected];

  return (
    <div className="categories">
      <div className="category-filter" role="group" aria-label="Filtrar disciplinas por status">
        {PROGRESS_CATEGORIES.map((category) => (
          <button
            key={category}
            type="button"
            className={`category-chip ${category}`}
            aria-pressed={selected === category}
            onClick={() => setSelected(category)}
          >
            {CATEGORY_LABELS[category]}
            <span className="category-count">{groups[category].length}</span>
          </button>
        ))}
      </div>

      {list.length === 0 ? (
        <p className="category-empty" role="status">
          {EMPTY_MESSAGES[selected]}
        </p>
      ) : (
        <ul className="category-list" aria-label={`Disciplinas: ${CATEGORY_LABELS[selected]}`}>
          {list.map((subject) => (
            <li key={subject.id}>
              <button
                type="button"
                className="category-item"
                onClick={() => onOpen(subject.id)}
              >
                <span className="category-code">{subject.id}</span>
                <span className="category-name">{subject.name}</span>
                <span className="category-meta">
                  {subject.period}º período · {subject.hours} h
                </span>
              </button>
            </li>
          ))}
        </ul>
      )}
    </div>
  );
}
