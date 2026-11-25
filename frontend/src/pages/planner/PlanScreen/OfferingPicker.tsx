import { useState } from "react";
import { Panel } from "../../../components/Panel/Panel";
import type { Curriculum, Evaluation } from "../../../services/api";
import { labels } from "../constants";

type Props = {
  catalog: Curriculum;
  evaluation: Evaluation;
  plan: string[];
  busy: boolean;
  addOffering: (id: string) => void;
};

export function OfferingPicker({ catalog, evaluation, plan, busy, addOffering }: Props) {
  const [search, setSearch] = useState("");
  const query = search.trim().toLocaleLowerCase("pt-BR");
  const subjects = catalog.subjects.filter(({ id, name }) =>
    `${id} ${name}`.toLocaleLowerCase("pt-BR").includes(query),
  );

  return (
    <Panel>
      <h2>Escolher disciplina e turma</h2>
      <label className="search">
        Buscar disciplina para a grade
        <input type="search" value={search} onChange={(event) => setSearch(event.target.value)} />
      </label>
      {subjects.length === 0 && <p>Nenhuma disciplina encontrada.</p>}
      {subjects.map((subject) => {
        const status = evaluation.subjects[subject.id];
        const offerings = catalog.offerings.filter((item) => item.subjectId === subject.id);
        return (
          <div className="plan-item" key={subject.id}>
            <h3>{subject.id} · {subject.name}</h3>
            <p>{labels[status?.state ?? "unknown"]}</p>
            {!!status?.missing.length && (
              <p>Pré-requisitos pendentes: {status.missing.join(", ")}</p>
            )}
            {offerings.length === 0 && <p>Sem turmas no catálogo.</p>}
            {offerings.map((item) => {
              const selected = plan.includes(item.id);
              return (
                <div className="offering" key={item.id}>
                  <p>{item.times.map((t) => `${t.day} ${t.start}–${t.end}`).join(" · ")}</p>
                  <button
                    disabled={busy || selected || status?.state !== "available"}
                    onClick={() => addOffering(item.id)}
                  >
                    {selected ? `Turma ${item.group} selecionada` : `Usar turma ${item.group}`}
                  </button>
                </div>
              );
            })}
          </div>
        );
      })}
    </Panel>
  );
}
