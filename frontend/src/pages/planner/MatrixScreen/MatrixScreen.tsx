import "./MatrixScreen.css";
import type { Curriculum, Evaluation } from "../../../services/api";
import { Panel } from "../../../components/Panel/Panel";
import { labels } from "../constants";

type Props = {
  catalog: Curriculum;
  evaluation: Evaluation;
  search: string;
  setSearch: (value: string) => void;
  go: (route: string) => void;
};

export function MatrixScreen({
  catalog,
  evaluation,
  search,
  setSearch,
  go,
}: Props) {
  return (
    <>
      <Panel>
        <div className="summary">
          <div>
            <b>Curso: {catalog.name}</b>
            <p>
              Versão da matriz: a validar · Recorte de {catalog.subjects.length}{" "}
              disciplinas
            </p>
          </div>
          <div>
            <b>{evaluation.completedHours} h concluídas · Sua trajetória</b>
            <p>Marque apenas as disciplinas já concluídas.</p>
          </div>
        </div>
      </Panel>
      <label className="search">
        Buscar disciplina
        <input
          type="search"
          placeholder="Nome ou código da disciplina"
          value={search}
          onChange={(e) => setSearch(e.target.value)}
        />
      </label>
      <p className="legend">
        ✓ Concluída · ○ Disponível · ! Pré-requisito pendente · ? Requisitos a
        validar
      </p>
      <div
        className="matrix-scroll"
        tabIndex={0}
        aria-label="Matriz por período"
      >
        <div className="matrix">
          {Array.from({ length: 8 }, (_, index) => (
            <section key={index}>
              <h3>{index + 1}º período</h3>
              {catalog.subjects
                .filter(
                  (item) =>
                    item.period === index + 1 &&
                    `${item.name} ${item.id}`
                      .toLocaleLowerCase()
                      .includes(search.toLocaleLowerCase()),
                )
                .map((item) => (
                  <button
                    className={`subject-card ${evaluation.subjects[item.id].state}`}
                    key={item.id}
                    onClick={() => go(`subject/${item.id}`)}
                  >
                    <span>{item.id}</span>
                    <b>{item.name}</b>
                    <small>{item.hours} h</small>
                    <span>{labels[evaluation.subjects[item.id].state]}</span>
                  </button>
                ))}
            </section>
          ))}
        </div>
      </div>
      {search &&
        !catalog.subjects.some((item) =>
          `${item.name} ${item.id}`
            .toLowerCase()
            .includes(search.toLowerCase()),
        ) && <p>Nenhuma disciplina encontrada.</p>}
      <Panel tone="highlight">
        <h2>Atualize seu progresso em um só lugar</h2>
        <p>
          Use “Marcar como concluída” no detalhe da disciplina. O histórico
          acompanha a matriz.
        </p>
        <div className="actions">
          <button onClick={() => go("subject/BCC201")}>
            Ver Introdução à Programação
          </button>
          <button className="secondary" onClick={() => go("history")}>
            Ver histórico
          </button>
        </div>
      </Panel>
    </>
  );
}
