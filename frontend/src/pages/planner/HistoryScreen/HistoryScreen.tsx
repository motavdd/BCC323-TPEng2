import type { Evaluation } from "../../../services/api";
import { Panel } from "../../../components/Panel/Panel";

type Props = {
  evaluation: Evaluation;
  go: (route: string) => void;
};

export function HistoryScreen({ evaluation, go }: Props) {
  const entries = Object.entries(evaluation.completedNames);
  return (
    <>
      <Panel>
        <h2>Disciplinas concluídas</h2>
      </Panel>
      {entries.map(([id, name]) => (
          <Panel key={id}>
            <h3 className="green">{name}</h3>
          </Panel>
        ))}
      {!entries.length && (
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
