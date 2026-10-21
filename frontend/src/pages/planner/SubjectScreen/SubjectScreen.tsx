import type { Curriculum, Evaluation, Subject } from "../../../services/api";
import { Panel } from "../../../components/Panel/Panel";
import { labels } from "../constants";

type Props = {
  subject: Subject;
  state: Evaluation["subjects"][string];
  catalog: Curriculum;
  completed: string[];
  confirmRemoval: boolean;
  setConfirmRemoval: (value: boolean) => void;
  busy: boolean;
  toggleCompleted: () => Promise<void>;
  go: (route: string) => void;
  setChooser: (value: boolean) => void;
};

export function SubjectScreen({
  subject,
  state,
  catalog,
  completed,
  confirmRemoval,
  setConfirmRemoval,
  busy,
  toggleCompleted,
  go,
  setChooser,
}: Props) {
  return (
    <>
      {confirmRemoval ? (
        <Panel>
          <h2>Remover conclusão de {subject.id}?</h2>
          <p>
            {subject.name} · {subject.hours} horas
          </p>
          <div className="notice warning">
            <h3>Revise as disciplinas e os planos afetados</h3>
            <p>
              {catalog.subjects
                .filter((item) => item.prerequisites.includes(subject.id))
                .map((item) => item.name)
                .join(", ") ||
                "Nenhum requisito dependente cadastrado neste recorte."}
            </p>
            <p>Os planos afetados precisarão de revisão.</p>
          </div>
          <p>
            Conclusões de outras disciplinas não serão apagadas automaticamente.
          </p>
          <div className="actions">
            <button disabled={busy} onClick={() => setConfirmRemoval(false)}>
              Manter conclusão
            </button>
            <button
              className="secondary"
              disabled={busy}
              onClick={() => void toggleCompleted()}
            >
              Remover e atualizar matriz
            </button>
          </div>
        </Panel>
      ) : (
        <>
          <Panel tone="highlight">
            <h2>{labels[state.state]}</h2>
            {subject.prerequisites.length === 0 ? (
              <p>
                {subject.requirementsKnown
                  ? "Nenhum pré-requisito indicado neste cenário ilustrativo."
                  : "Os requisitos desta disciplina ainda precisam de validação."}
              </p>
            ) : (
              <ul>
                {subject.prerequisites.map((id) => (
                  <li key={id}>
                    <button
                      className="text-button"
                      onClick={() => go(`subject/${id}`)}
                    >
                      {id} ·{" "}
                      {catalog.subjects.find((item) => item.id === id)?.name}
                    </button>{" "}
                    · {completed.includes(id) ? "Concluída" : "Pendente"}
                  </li>
                ))}
              </ul>
            )}
            <p className="muted">
              Confira as regras da sua versão curricular antes de usar a
              informação.
            </p>
          </Panel>
          <Panel>
            <h2>
              {state.state === "completed"
                ? "Conclusão registrada"
                : "Você já concluiu esta disciplina?"}
            </h2>
            <p>
              Ao registrar a conclusão, o histórico e o total de horas serão
              atualizados.
            </p>
            <div className="actions">
              <button disabled={busy} onClick={() => void toggleCompleted()}>
                {state.state === "completed"
                  ? "Remover conclusão"
                  : "Marcar como concluída"}
              </button>
              <button
                className="secondary"
                disabled={state.state !== "available"}
                onClick={() => {
                  setChooser(true);
                  go("plan");
                }}
              >
                Ir para o planejamento
              </button>
            </div>
          </Panel>
        </>
      )}
      <button className="secondary" onClick={() => go("matrix")}>
        Voltar à matriz
      </button>
    </>
  );
}
