import { fireEvent, render, screen } from "@testing-library/react";
import { expect, it, vi } from "vitest";
import type { Curriculum, Evaluation } from "../../../services/api";
import { OfferingPicker } from "./OfferingPicker";

it("exibe o pré-requisito informado pela API e permite escolher a turma quando liberada", () => {
  const catalog: Curriculum = {
    id: "computacao",
    name: "Computação",
    illustrative: true,
    subjects: [{
      id: "BCC202", name: "Estrutura de Dados I", period: 2, hours: 90,
      prerequisites: ["BCC201"], requirementsKnown: true,
    }],
    offerings: [{
      id: "BCC202-A", subjectId: "BCC202", group: "A",
      times: [{ day: "Seg", start: "08:00", end: "10:00" }],
    }],
  };
  const evaluation: Evaluation = {
    courseId: catalog.id, illustrative: true, completed: [], unmappedCompleted: [],
    completedHours: 0, totalHours: 90, completedCount: 0, availableCount: 0,
    progress: 0, subjects: { BCC202: { state: "blocked", missing: ["BCC201"] } },
  };
  const addOffering = vi.fn();
  const props = { catalog, plan: [], busy: false, addOffering };
  const { rerender } = render(<OfferingPicker {...props} evaluation={evaluation} />);
  const search = screen.getByRole("searchbox");

  fireEvent.change(search, { target: { value: "BCC999" } });
  expect(screen.getByText("Nenhuma disciplina encontrada.")).toBeVisible();
  fireEvent.change(search, { target: { value: "BCC202" } });
  expect(screen.getByText("Pré-requisitos pendentes: BCC201")).toBeVisible();
  expect(screen.getByRole("button", { name: "Usar turma A" })).toBeDisabled();

  rerender(<OfferingPicker {...props} evaluation={{
    ...evaluation, subjects: { BCC202: { state: "available", missing: [] } },
  }} />);
  fireEvent.click(screen.getByRole("button", { name: "Usar turma A" }));
  expect(addOffering).toHaveBeenCalledExactlyOnceWith("BCC202-A");
});
