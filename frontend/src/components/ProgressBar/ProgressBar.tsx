import "./ProgressBar.css";
import { clampPercent, formatPercent } from "../../utils/progress";

type Props = {
  /** Percentual concluído (0 a 100), como a API devolve em `evaluation.progress`. */
  percent: number;
  completedHours: number;
  totalHours: number;
  label?: string;
};

/** Barra de progresso do curso pela carga horária concluída (issue #74). */
export function ProgressBar({
  percent,
  completedHours,
  totalHours,
  label = "Progresso no curso",
}: Props) {
  const value = clampPercent(percent);
  const text = formatPercent(value);
  return (
    <div className="progress">
      <div className="progress-header">
        <span id="progress-label">{label}</span>
        <strong>{text}</strong>
      </div>
      <div
        className="progress-track"
        role="progressbar"
        aria-labelledby="progress-label"
        aria-valuemin={0}
        aria-valuemax={100}
        aria-valuenow={Math.round(value * 10) / 10}
        aria-valuetext={`${text} concluído, ${completedHours} de ${totalHours} horas`}
      >
        <div className="progress-fill" style={{ width: `${value}%` }} />
      </div>
      <p className="progress-hours">
        {completedHours} h de {totalHours} h concluídas
      </p>
    </div>
  );
}
