import type { Curriculum, Evaluation } from "../../../services/api";
import { Panel } from "../../../components/Panel/Panel";

type Props = {
  catalog: Curriculum;
  completed: string[];
  evaluation: Evaluation;
  go: (route: string) => void;
};

export function HistoryScreen({ catalog, completed, evaluation, go }: Props) {
  return (
    <>
      <Panel>
        <h2>{evaluation.completedHours} horas concluídas</h2>
        <p>
          {evaluation.completedCount} disciplinas · Atualizado a partir da
          matriz curricular
        </p>
      </Panel>
      {catalog.subjects
        .filter((item) => completed.includes(item.id))
        .map((item) => (
          <Panel key={item.id}>
            <h3 className="green">
              ✓ {item.id} · {item.name}
            </h3>
            <p>
              {item.period}º período · {item.hours} horas · Concluída
            </p>
          </Panel>
        ))}
      {!completed.length && (
        <Panel>
          <h2>Seu histórico começa na matriz</h2>
          <p>Você ainda não registrou disciplinas concluídas.</p>
        </Panel>
      )}
      <Panel tone="highlight">
        <h2>Quer corrigir uma conclusão?</h2>
        <p>
          Edite na matriz. Ela é a única entrada para registrar ou remover
          conclusões.
        </p>
      </Panel>
      <div className="actions">
        <button onClick={() => go("matrix")}>
          Editar conclusões na matriz
        </button>
        <button className="secondary" onClick={() => go("overview")}>
          Ver visão geral
        </button>
      </div>
    </>
  );
}
