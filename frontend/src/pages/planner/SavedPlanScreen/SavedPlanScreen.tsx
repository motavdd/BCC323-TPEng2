import type { Curriculum, Offering } from "../../../services/api";
import { Panel } from "../../../components/Panel/Panel";

type Props = {
  catalog: Curriculum;
  selected: Offering[];
  planHours: number;
  go: (route: string) => void;
};

export function SavedPlanScreen({ catalog, selected, planHours, go }: Props) {
  return (
    <>
      <Panel tone="success">
        <h2>✓ Plano salvo com sucesso</h2>
        <p>Você pode voltar e ajustar suas escolhas quando precisar.</p>
        <p>
          Próximo semestre · {selected.length} disciplinas · {planHours} horas
        </p>
      </Panel>
      <Panel>
        {selected.map((item) => (
          <h3 key={item.id}>
            {catalog.subjects.find((s) => s.id === item.subjectId)?.name} ·
            Turma {item.group}
          </h3>
        ))}
        {!selected.length && <p>Plano vazio salvo.</p>}
        <p>Confira a oferta oficial antes de se matricular.</p>
      </Panel>
      <Panel tone="warning">
        <b>Este plano não realiza nem garante sua matrícula.</b>
      </Panel>
      <div className="actions">
        <button onClick={() => go("plan")}>Voltar ao planejamento</button>
        <button className="secondary" onClick={() => go("overview")}>
          Ver visão geral
        </button>
      </div>
    </>
  );
}
