import "./PlanScreen.css";
import type { Curriculum, Evaluation, Offering } from "../../../services/api";
import type { Dispatch, SetStateAction } from "react";
import { Panel } from "../../../components/Panel/Panel";
import { Week } from "../../../components/Week/Week";
import { OfferingPicker } from "./OfferingPicker";
import { labels } from "../constants";

type Props = {
  catalog: Curriculum;
  evaluation: Evaluation;
  plan: string[];
  setPlan: Dispatch<SetStateAction<string[]>>;
  dirty: boolean;
  busy: boolean;
  conflicts: [Offering, Offering][];
  invalidPlan: Offering[];
  missingOfferings: string[];
  selected: Offering[];
  planHours: number;
  savePlan: () => Promise<void>;
  addOffering: (id: string) => void;
  chooser: boolean;
  setChooser: (value: boolean) => void;
};

export function PlanScreen({
  catalog,
  evaluation,
  plan,
  setPlan,
  dirty,
  busy,
  conflicts,
  invalidPlan,
  missingOfferings,
  selected,
  planHours,
  savePlan,
  addOffering,
  chooser,
  setChooser,
}: Props) {
  return (
    <>
      <Panel tone="warning">
        <h2>Montagem de grade · Rascunho</h2>
        <p>
          A disponibilidade das disciplinas vem da API acadêmica. A comparação
          dos horários acontece nesta tela. A API ainda não confirma vagas nem
          valida propostas de grade. O plano é salvo como rascunho pessoal.
        </p>
        {catalog.illustrative && <p>O catálogo deste curso é ilustrativo.</p>}
      </Panel>
      <div className="actions plan-toolbar">
        <p>
          {dirty
            ? "Rascunho com alterações"
            : "Próximo semestre · Plano atualizado"}
        </p>
        <button
          disabled={
            busy ||
            !evaluation ||
            !!conflicts.length ||
            !!invalidPlan.length ||
            !!missingOfferings.length
          }
          onClick={() => void savePlan()}
        >
          {busy ? "Salvando…" : "Salvar plano"}
        </button>
      </div>
      {(invalidPlan.length > 0 || missingOfferings.length > 0) && (
        <Panel tone="warning">
          <h2>Seu plano precisa de revisão</h2>
          <p>
            Remova as turmas indisponíveis ou atualize seu histórico.
          </p>
          {invalidPlan.map((item) => {
            const status = evaluation.subjects[item.subjectId];
            return (
              <p key={item.id}>
                {item.subjectId} · {labels[status?.state ?? "unknown"]}
                {!!status?.missing.length && ` · Faltam: ${status.missing.join(", ")}`}
              </p>
            );
          })}
          {missingOfferings.map((id) => (
            <button
              key={id}
              className="secondary"
              disabled={busy}
              onClick={() => setPlan(plan.filter((old) => old !== id))}
            >
              Remover oferta indisponível {id}
            </button>
          ))}
        </Panel>
      )}
      {conflicts.length > 0 && (
        <Panel tone="danger">
          <div role="alert">
            <h2>Conflito de horário no rascunho</h2>
            <p>Troque uma turma ou remova uma disciplina para continuar.</p>
            {conflicts.map(([a, b]) => (
              <p key={JSON.stringify([a.id, b.id])}>
                {a.subjectId} (turma {a.group}) e {b.subjectId} (turma {b.group}) têm horários sobrepostos.
              </p>
            ))}
          </div>
        </Panel>
      )}
      <div className="plan-layout">
        <Panel>
          <h2>No seu plano</h2>
          {selected.map((item) => (
            <div className="plan-item" key={item.id}>
              <h3>
                {catalog.subjects.find((s) => s.id === item.subjectId)?.name}
              </h3>
              <p>
                Turma {item.group} ·{" "}
                {catalog.subjects.find((s) => s.id === item.subjectId)?.hours} h
              </p>
              <label>
                Trocar turma
                <select
                  value={item.id}
                  disabled={busy || evaluation.subjects[item.subjectId]?.state !== "available"}
                  onChange={(e) => addOffering(e.target.value)}
                >
                  {catalog.offerings
                    .filter((o) => o.subjectId === item.subjectId)
                    .map((o) => (
                      <option key={o.id} value={o.id}>
                        Turma {o.group} ·{" "}
                        {o.times
                          .map((t) => `${t.day} ${t.start}–${t.end}`)
                          .join(" / ")}
                      </option>
                    ))}
                </select>
              </label>
              <button
                className="text-button"
                disabled={busy}
                onClick={() => setPlan(plan.filter((id) => id !== item.id))}
              >
                Remover do plano
              </button>
            </div>
          ))}
          {!selected.length && (
            <p>Adicione uma disciplina disponível para começar.</p>
          )}
          <button
            className="secondary"
            onClick={() => setChooser(!chooser)}
            aria-expanded={chooser}
          >
            Adicionar disciplina
          </button>
          <h3>Carga total: {planHours} h</h3>
          <p className={conflicts.length ? "red" : "green"}>
            {conflicts.length ? "! Há conflitos" : "Sem sobreposição nos horários exibidos"}
          </p>
        </Panel>
        <Panel>
          <h2>Sua semana</h2>
          <Week offerings={selected} catalog={catalog} />
          <p className="muted">Horários consecutivos são permitidos.</p>
        </Panel>
      </div>
      {chooser && (
        <OfferingPicker
          catalog={catalog}
          evaluation={evaluation}
          plan={plan}
          busy={busy}
          addOffering={addOffering}
        />
      )}
    </>
  );
}
