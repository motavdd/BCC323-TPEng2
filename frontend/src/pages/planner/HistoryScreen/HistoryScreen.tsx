import { useState } from "react";
import type { Curriculum, Evaluation } from "../../../services/api";
import { Panel } from "../../../components/Panel/Panel";
import "./HistoryScreen.css";

type Props = {
  catalog: Curriculum;
  evaluation: Evaluation;
  busy: boolean;
  save: (id: string, done: boolean) => Promise<void>;
};

export function HistoryScreen({ catalog, evaluation, busy, save }: Props) {
  const [search, setSearch] = useState("");
  const query = search.trim().toLocaleLowerCase("pt-BR");
  const subjects = catalog.subjects.filter(
    ({ id, name }) =>
      id.toLocaleLowerCase("pt-BR").includes(query) ||
      name.toLocaleLowerCase("pt-BR").includes(query),
  );
  const periods = [...new Set(subjects.map(({ period }) => period))].sort(
    (a, b) => a - b,
  );
  const completed = new Set(evaluation.completed);

  return (
    <>
      <Panel>
        <h2>{evaluation.completedHours} horas concluídas</h2>
        <p>{evaluation.completedCount} disciplinas concluídas</p>
        <p>Marque as disciplinas que você já concluiu. Cada alteração é salva automaticamente.</p>
      </Panel>
      <label className="search">
        Buscar disciplina
        <input
          type="search"
          placeholder="Nome ou código da disciplina"
          value={search}
          onChange={(event) => setSearch(event.target.value)}
        />
      </label>
      {busy && <p role="status">Salvando alteração…</p>}
      {periods.map((period) => (
        <Panel key={period}>
          <h2>{period}º período</h2>
          <ul className="history-subjects">
            {subjects.filter((item) => item.period === period).map((item) => (
              <li key={item.id}>
                <label>
                  <input
                    type="checkbox"
                    checked={completed.has(item.id)}
                    disabled={busy}
                    onChange={(event) => void save(item.id, event.target.checked)}
                  />
                  {" "}{item.id} · {item.name} · {item.hours} h
                </label>
              </li>
            ))}
          </ul>
        </Panel>
      ))}
      {periods.length === 0 && <p>Nenhuma disciplina encontrada.</p>}
      {evaluation.unmappedCompleted.length > 0 && (
        <Panel tone="warning">
          <h2>Conclusões fora da matriz atual</h2>
          <p>{evaluation.unmappedCompleted.join(", ")}</p>
        </Panel>
      )}
    </>
  );
}
