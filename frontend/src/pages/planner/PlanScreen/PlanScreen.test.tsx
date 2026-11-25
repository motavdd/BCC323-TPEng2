import { render, screen, within } from "@testing-library/react";
import { expect, it, vi } from "vitest";
import type { Offering } from "../../../services/api";
import { PlanScreen } from "./PlanScreen";

it("identifica as turmas em conflito e bloqueia o salvamento do rascunho", () => {
  const first: Offering = {
    id: "BCC201-A", subjectId: "BCC201", group: "A",
    times: [{ day: "Seg", start: "08:00", end: "10:00" }],
  };
  const second: Offering = {
    id: "BCC202-B", subjectId: "BCC202", group: "B",
    times: [{ day: "Seg", start: "09:00", end: "11:00" }],
  };
  const selected = [first, second];
  render(<PlanScreen
    catalog={{ id: "computacao", name: "Computação", illustrative: true, subjects: [], offerings: selected }}
    evaluation={{
      courseId: "computacao", illustrative: true, completed: [], unmappedCompleted: [], completedNames: {},
      completedHours: 0, totalHours: 150, completedCount: 0, availableCount: 2,
      progress: 0, subjects: {
        BCC201: { state: "available", missing: [] },
        BCC202: { state: "available", missing: [] },
      },
    }}
    plan={selected.map(({ id }) => id)}
    selected={selected}
    setPlan={vi.fn()}
    dirty
    busy={false}
    conflicts={[[first, second]]}
    invalidPlan={[]}
    missingOfferings={[]}
    planHours={150}
    savePlan={vi.fn()}
    addOffering={vi.fn()}
    chooser={false}
    setChooser={vi.fn()}
  />);

  expect(within(screen.getByRole("alert")).getByText(/BCC201 \(turma A\) e BCC202 \(turma B\)/)).toBeVisible();
  expect(screen.getByRole("button", { name: "Salvar plano" })).toBeDisabled();
});
