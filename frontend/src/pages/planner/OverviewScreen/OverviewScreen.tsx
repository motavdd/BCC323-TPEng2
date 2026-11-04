import "./OverviewScreen.css";
import type { Evaluation } from "../../../services/api";
import { Panel } from "../../../components/Panel/Panel";

type Props = {
  evaluation: Evaluation;
  savedPlan: string[];
  go: (route: string) => void;
};

export function OverviewScreen({ evaluation, savedPlan, go }: Props) {
  return (
    <>
      <Panel tone="highlight">
        <h2>Uma matriz para registrar sua trajetória</h2>
        <p>
          Marque as concluídas, consulte os requisitos e descubra o próximo
          passo.
        </p>
        <button onClick={() => go("matrix")}>Abrir matriz interativa</button>
      </Panel>
      <div className="stats">
        <Panel>
          <p>Carga horária concluída</p>
          <strong>{evaluation.completedHours} h</strong>
        </Panel>
        <Panel>
          <p>Disciplinas concluídas</p>
          <strong>{evaluation.completedCount}</strong>
        </Panel>
        <Panel>
          <p>Dados do exemplo</p>
          <h3>Recorte ilustrativo</h3>
        </Panel>
      </div>
      <Panel>
        <h2>Histórico para consulta; matriz para edição</h2>
        <p>
          Você registra as conclusões uma vez e acompanha o resumo no histórico.
        </p>
        <div className="actions">
          <button className="secondary" onClick={() => go("history")}>
            Consultar histórico
          </button>
          <button onClick={() => go("plan")}>Planejar semestre</button>
        </div>
      </Panel>
      {savedPlan.length > 0 && (
        <button className="secondary" onClick={() => go("plan")}>
          Retomar plano salvo
        </button>
      )}
    </>
  );
}
