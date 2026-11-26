import { fireEvent, render, screen, within } from "@testing-library/react";
import { expect, it, vi } from "vitest";
import type { Evaluation, Subject } from "../../services/api";
import { SubjectCategories } from "./SubjectCategories";

const subjects: Subject[] = [
  { id: "BCC201", name: "Introdução à Programação", period: 1, hours: 90, prerequisites: [], requirementsKnown: true },
  { id: "BCC101", name: "Matemática Discreta", period: 1, hours: 60, prerequisites: [], requirementsKnown: true },
  { id: "BCC202", name: "Estrutura de Dados I", period: 2, hours: 90, prerequisites: ["BCC201"], requirementsKnown: true },
  { id: "BCC203", name: "Estrutura de Dados II", period: 3, hours: 60, prerequisites: ["BCC202"], requirementsKnown: true },
];

const states: Evaluation["subjects"] = {
  BCC201: { state: "completed", missing: [] },
  BCC101: { state: "available", missing: [] },
  BCC202: { state: "available", missing: [] },
  BCC203: { state: "blocked", missing: ["BCC202"] },
};

function chip(name: RegExp) {
  return screen.getByRole("button", { name });
}

it("mostra a quantidade de disciplinas em cada categoria", () => {
  render(<SubjectCategories subjects={subjects} states={states} onOpen={() => {}} />);
  expect(chip(/^Concluídas/)).toHaveTextContent("1");
  expect(chip(/^Em andamento/)).toHaveTextContent("0");
  expect(chip(/^Disponíveis/)).toHaveTextContent("2");
  expect(chip(/^Ainda não liberadas/)).toHaveTextContent("1");
});

it("começa pelas disponíveis e filtra ao trocar de categoria", () => {
  render(<SubjectCategories subjects={subjects} states={states} onOpen={() => {}} />);
  expect(chip(/^Disponíveis/)).toHaveAttribute("aria-pressed", "true");
  let list = screen.getByRole("list", { name: "Disciplinas: Disponíveis" });
  expect(within(list).getAllByRole("listitem")).toHaveLength(2);

  fireEvent.click(chip(/^Concluídas/));
  expect(chip(/^Concluídas/)).toHaveAttribute("aria-pressed", "true");
  list = screen.getByRole("list", { name: "Disciplinas: Concluídas" });
  expect(within(list).getByText("Introdução à Programação")).toBeInTheDocument();
  expect(within(list).queryByText("Matemática Discreta")).not.toBeInTheDocument();
});

it("mostra uma mensagem quando a categoria está vazia", () => {
  render(<SubjectCategories subjects={subjects} states={states} onOpen={() => {}} />);
  fireEvent.click(chip(/^Em andamento/));
  expect(screen.getByRole("status")).toHaveTextContent("Nenhuma disciplina em andamento");
  expect(screen.queryByRole("list")).not.toBeInTheDocument();
});

it("abre a disciplina ao clicar nela", () => {
  const onOpen = vi.fn();
  render(<SubjectCategories subjects={subjects} states={states} onOpen={onOpen} />);
  fireEvent.click(screen.getByRole("button", { name: /Matemática Discreta/ }));
  expect(onOpen).toHaveBeenCalledWith("BCC101");
});
