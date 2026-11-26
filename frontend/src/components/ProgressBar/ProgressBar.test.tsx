import { render, screen } from "@testing-library/react";
import { expect, it } from "vitest";
import { ProgressBar } from "./ProgressBar";

it("mostra o percentual e as horas concluídas", () => {
  render(<ProgressBar percent={25} completedHours={180} totalHours={720} />);
  const bar = screen.getByRole("progressbar", { name: "Progresso no curso" });
  expect(bar).toHaveAttribute("aria-valuenow", "25");
  expect(bar).toHaveAttribute("aria-valuetext", "25% concluído, 180 de 720 horas");
  expect(screen.getByText("25%")).toBeInTheDocument();
  expect(screen.getByText("180 h de 720 h concluídas")).toBeInTheDocument();
});

it("preenche a barra na proporção do progresso", () => {
  const { container } = render(
    <ProgressBar percent={40} completedHours={288} totalHours={720} />,
  );
  expect(container.querySelector(".progress-fill")).toHaveStyle({ width: "40%" });
});

it("limita valores fora do intervalo de 0 a 100", () => {
  const { container, rerender } = render(
    <ProgressBar percent={130} completedHours={900} totalHours={720} />,
  );
  expect(screen.getByRole("progressbar")).toHaveAttribute("aria-valuenow", "100");
  expect(container.querySelector(".progress-fill")).toHaveStyle({ width: "100%" });

  rerender(<ProgressBar percent={-5} completedHours={0} totalHours={720} />);
  expect(screen.getByRole("progressbar")).toHaveAttribute("aria-valuenow", "0");
  expect(screen.getByText("0%")).toBeInTheDocument();
});
