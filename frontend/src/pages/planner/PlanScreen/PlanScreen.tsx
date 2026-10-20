import "./PlanScreen.css";
import type { Curriculum, Evaluation, Offering } from "../../../services/api";
import type { Dispatch, SetStateAction } from "react";
import { Panel } from "../../../components/Panel/Panel";
import { Week } from "../../../components/Week/Week";

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
            Há disciplinas concluídas, bloqueadas ou sem oferta cadastrada.
            Remova-as ou atualize seu histórico.
          </p>
          {missingOfferings.map((id) => (
            <button
              key={id}
              className="secondary"
              onClick={() => setPlan(plan.filter((old) => old !== id))}
            >
              Remover oferta indisponível {id}
            </button>
          ))}
        </Panel>
      )}
      {conflicts.length > 0 && (
        <Panel tone="danger">
          <h2>Resolva o conflito de horário</h2>
          <p>Troque uma turma ou remova uma disciplina para continuar.</p>
          {conflicts.map(([a, b]) => (
            <p key={a.id + b.id}>
              {a.id} e {b.id} têm horários sobrepostos.
            </p>
          ))}
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
            {conflicts.length ? "! Há conflitos" : "✓ Sem conflitos"}
          </p>
        </Panel>
        <Panel>
          <h2>Sua semana</h2>
          <Week offerings={selected} catalog={catalog} />
          <p className="muted">Horários consecutivos são permitidos.</p>
        </Panel>
      </div>
      {chooser && (
        <Panel>
          <h2>Escolher turma</h2>
          <p>
            Turmas ilustrativas do protótipo. Apenas ofertas com requisitos
            conhecidos e cumpridos são exibidas.
          </p>
          {catalog.offerings
            .filter(
              (item) =>
                evaluation.subjects[item.subjectId].state === "available" &&
                !plan.includes(item.id),
            )
            .map((item) => (
              <div className="offering" key={item.id}>
                <div>
                  <b>
                    {
                      catalog.subjects.find((s) => s.id === item.subjectId)
                        ?.name
                    }{" "}
                    · Turma {item.group}
                  </b>
                  <p>
                    {item.times
                      .map((t) => `${t.day} ${t.start}–${t.end}`)
                      .join(" · ")}
                  </p>
                </div>
                <button onClick={() => addOffering(item.id)}>
                  Usar turma {item.group}
                </button>
              </div>
            ))}
          {!catalog.offerings.some(
            (item) =>
              evaluation.subjects[item.subjectId].state === "available" &&
              !plan.includes(item.id),
          ) && (
            <p>
              Nenhuma outra oferta disponível. Consulte os requisitos na matriz.
            </p>
          )}
        </Panel>
      )}
    </>
  );
}
