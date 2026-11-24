import { render, screen, within } from "@testing-library/react";
import { expect, it } from "vitest";
import { Week } from "./Week";

const catalog = {
  id: "computacao",
  name: "Computação",
  illustrative: true,
  subjects: [{
    id: "BCC202", name: "Estrutura de Dados I", period: 2, hours: 90,
    prerequisites: [], requirementsKnown: true,
  }],
  offerings: [],
};

it("ordena os horários e coloca cada aula na coluna correspondente, incluindo sábado", () => {
  render(<Week catalog={catalog} offerings={[{
    id: "BCC202-A", subjectId: "BCC202", group: "A",
    times: [
      { day: "Sáb", start: "14:40", end: "16:20" },
      { day: "Segunda-feira", start: "09:15", end: "10:45" },
      { day: "Sabado", start: "14:40", end: "15:30" },
    ],
  }]} />);

  expect(screen.getByRole("columnheader", { name: "Sábado" })).toBeVisible();
  expect(screen.getAllByRole("rowheader").map((header) => header.textContent))
    .toEqual(["09:15", "14:40"]);
  const monday = screen.getByRole("rowheader", { name: "09:15" }).closest("tr")!;
  const saturday = screen.getByRole("rowheader", { name: "14:40" }).closest("tr")!;
  expect(within(within(monday).getAllByRole("cell")[0]).getByText("09:15–10:45")).toBeVisible();
  const saturdayCell = within(within(saturday).getAllByRole("cell")[5]);
  expect(saturdayCell.getByText("14:40–16:20")).toBeVisible();
  expect(saturdayCell.getByText("14:40–15:30")).toBeVisible();
});

it("informa quando nenhuma turma foi selecionada", () => {
  render(<Week catalog={catalog} offerings={[]} />);
  expect(screen.getByText("Nenhuma aula na semana.")).toBeVisible();
});
