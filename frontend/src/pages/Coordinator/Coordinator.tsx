import "./Coordinator.css";
import { useEffect, useState, type FormEvent, type ReactNode } from "react";
import { Panel } from "../../components/Panel/Panel";
import { api, type Curriculum, type Offering, type Subject } from "../../services/api";
import { message } from "../../utils/errors";

type Catalog = Curriculum & { revision: string };
type Editor = { kind: "subject" | "offering"; id?: string };
const days = ["Seg", "Ter", "Qua", "Qui", "Sex", "Sáb"];
const emptyTime = { day: "Seg", start: "", end: "", room: "" };

export function Coordinator({ onSignOut }: { onSignOut: () => Promise<void> }) {
  const [catalog, setCatalog] = useState<Catalog>();
  const [tab, setTab] = useState<Editor["kind"]>("subject");
  const [editor, setEditor] = useState<Editor>();
  const [attempt, setAttempt] = useState(0);
  const [busy, setBusy] = useState(false);
  const [error, setError] = useState("");
  const [notice, setNotice] = useState("");

  useEffect(() => {
    let active = true;
    api<Catalog>("/coordinator/curriculum").then((result) => {
      if (active) setCatalog(result);
    }).catch((cause) => {
      if (active) setError(message(cause));
    });
    return () => { active = false; };
  }, [attempt]);

  async function perform(action: () => Promise<void>) {
    setBusy(true);
    setNotice("");
    setError("");
    try {
      await action();
    } catch (cause) {
      setError(message(cause));
    } finally {
      setBusy(false);
    }
  }

  function change(kind: Editor["kind"], id: string, body?: unknown) {
    if (!catalog) return;
    const collection = kind === "subject" ? "subjects" : "offerings";
    void perform(async () => {
      const result = await api<Catalog>(`/coordinator/${collection}/${encodeURIComponent(id)}`,
        body, body === undefined ? "DELETE" : "PUT", catalog.revision);
      setCatalog(result);
      setEditor(undefined);
      setNotice(body !== undefined ? "Alterações salvas."
        : kind === "subject" ? "Disciplina arquivada." : "Turma removida.");
    });
  }

  function selectTab(next: Editor["kind"]) {
    setTab(next);
    openEditor();
  }

  function openEditor(next?: Editor) {
    setEditor(next);
    setError("");
    setNotice("");
  }

  function reload() {
    setEditor(undefined);
    setCatalog(undefined);
    setError("");
    setNotice("");
    setAttempt((value) => value + 1);
  }

  const subjects = catalog?.subjects.filter((item) => !item.archived) ?? [];
  const editSubject = catalog?.subjects.find((item) => item.id === editor?.id);
  const editOffering = catalog?.offerings.find((item) => item.id === editor?.id);
  const feedback = error && (
    <div className="notice danger">
      <p role="alert">{error}</p>
      <button type="button" className="secondary" disabled={busy} onClick={reload}>Recarregar matriz</button>
    </div>
  );

  return (
    <div className="app-shell coordinator">
      <aside className="sidebar">
        <strong className="brand">MyPlanner</strong>
        <p>Coordenação</p>
        <nav aria-label="Navegação da coordenação">
          <button disabled={busy} aria-current={tab === "subject" ? "page" : undefined}
            onClick={() => selectTab("subject")}>Disciplinas</button>
          <button disabled={busy} aria-current={tab === "offering" ? "page" : undefined}
            onClick={() => selectTab("offering")}>Turmas e horários</button>
        </nav>
        <button className="text-button logout" disabled={busy} onClick={() => void perform(onSignOut)}>Sair da conta</button>
      </aside>
      <main className="workspace">
        <header>
          <h1>{tab === "subject" ? "Matriz curricular" : "Turmas e horários"}</h1>
          {catalog && <p>{catalog.name}{catalog.illustrative ? " · Catálogo ilustrativo" : ""}</p>}
        </header>
        {notice && <p role="status" className="notice success">{notice}</p>}
        {!editor && feedback}
        {!catalog && !error && <p role="status">Carregando matriz…</p>}
        {catalog && tab === "subject" && (
          <>
            <div className="actions">
              <button disabled={busy} onClick={() => openEditor({ kind: "subject" })}>Nova disciplina</button>
            </div>
            {editor?.kind === "subject" && <SubjectForm key={editor.id ?? "new"} subject={editSubject}
              catalog={catalog} busy={busy} onCancel={() => openEditor()} feedback={feedback}
              onSave={(id, data) => change("subject", id, data)} />}
            {catalog.subjects.map((item) => <Panel key={item.id}>
              <h2>{item.id} · {item.name}</h2>
              <p>{item.hours} h · {item.period}º período{item.archived && " · Arquivada"}</p>
              <p>Pré-requisitos: {item.prerequisites.join(", ") || (item.requirementsKnown ? "Nenhum" : "A validar")}</p>
              {!item.archived && <div className="actions">
                <button className="secondary" disabled={busy} aria-label={`Editar disciplina ${item.id}`}
                  onClick={() => openEditor({ kind: "subject", id: item.id })}>Editar</button>
                <button className="text-button" disabled={busy} aria-label={`Arquivar disciplina ${item.id}`}
                  onClick={() => {
                    if (window.confirm(`Arquivar ${item.id}? As turmas serão retiradas e o histórico será preservado.`)) {
                      change("subject", item.id);
                    }
                  }}>Arquivar</button>
              </div>}
            </Panel>)}
          </>
        )}
        {catalog && tab === "offering" && (
          <>
            <div className="actions">
              <button disabled={busy || !subjects.length} onClick={() => openEditor({ kind: "offering" })}>Nova turma</button>
            </div>
            {editor?.kind === "offering" && <OfferingForm key={editor.id ?? "new"} offering={editOffering}
              catalog={catalog} subjects={subjects} busy={busy} onCancel={() => openEditor()} feedback={feedback}
              onSave={(id, data) => change("offering", id, data)} />}
            {!catalog.offerings.length && <p>Nenhuma turma cadastrada.</p>}
            {catalog.offerings.map((item) => <Panel key={item.id}>
              <h2>{item.id} · {item.subjectId} · Turma {item.group}</h2>
              <p>{item.capacity} vagas</p>
              <ul>
                {item.times.map((slot) => <li key={`${slot.day}-${slot.start}`}>
                  {slot.day} {slot.start}–{slot.end}{slot.room && ` · ${slot.room}`}
                </li>)}
              </ul>
              <div className="actions">
                <button className="secondary" disabled={busy} aria-label={`Editar turma ${item.id}`}
                  onClick={() => openEditor({ kind: "offering", id: item.id })}>Editar</button>
                <button className="text-button" disabled={busy} aria-label={`Remover turma ${item.id}`}
                  onClick={() => {
                    if (window.confirm(`Remover a turma ${item.id}?`)) change("offering", item.id);
                  }}>Remover</button>
              </div>
            </Panel>)}
          </>
        )}
      </main>
    </div>
  );
}

type FormProps = { catalog: Catalog; busy: boolean; onCancel: () => void; feedback: ReactNode };

function SubjectForm({ subject, catalog, busy, onCancel, onSave, feedback }: FormProps & {
  subject?: Subject;
  onSave: (id: string, data: Omit<Subject, "id" | "archived">) => void;
}) {
  const [prerequisites, setPrerequisites] = useState(subject?.prerequisites ?? []);
  const [search, setSearch] = useState("");
  const query = search.trim().toLocaleLowerCase();
  const initialSelection = new Set(subject?.prerequisites);
  const options = catalog.subjects
    .filter((item) => !item.archived && item.id !== subject?.id
      && `${item.id} ${item.name}`.toLocaleLowerCase().includes(query))
    .sort((a, b) => Number(initialSelection.has(b.id)) - Number(initialSelection.has(a.id)));

  function submit(event: FormEvent<HTMLFormElement>) {
    event.preventDefault();
    const fields = new FormData(event.currentTarget);
    const id = subject?.id ?? String(fields.get("code")).trim();
    if (!subject && catalog.subjects.some((item) => item.id === id)) {
      const code = event.currentTarget.elements.namedItem("code") as HTMLInputElement;
      code.setCustomValidity("Este código já está em uso.");
      code.reportValidity();
      return;
    }
    onSave(id, {
      name: String(fields.get("name")).trim(),
      hours: Number(fields.get("hours")),
      period: Number(fields.get("period")),
      prerequisites,
      requirementsKnown: fields.has("requirementsKnown"),
    });
  }

  return <form className="panel coordinator-form" onSubmit={submit}>
    <h2>{subject ? `Editar ${subject.id}` : "Cadastrar disciplina"}</h2>
    <fieldset disabled={busy}>
      <label>Código da disciplina
        <input name="code" defaultValue={subject?.id} readOnly={!!subject} autoFocus={!subject} required maxLength={100}
          pattern="[A-Za-z0-9_\x2D]+" onInput={(event) => event.currentTarget.setCustomValidity("")} />
      </label>
      <label>Nome da disciplina <input name="name" defaultValue={subject?.name} autoFocus={!!subject} required maxLength={160} /></label>
      <div className="coordinator-fields">
        <label>Carga horária <input name="hours" type="number" defaultValue={subject?.hours ?? 60} required min={1} max={10000} /></label>
        <label>Período sugerido <input name="period" type="number" defaultValue={subject?.period ?? 1} required min={1} max={20} /></label>
      </div>
      <fieldset className="prerequisites">
        <legend>Pré-requisitos</legend>
        <label>Buscar pré-requisito
          <input type="search" value={search} placeholder="Código ou nome da disciplina"
            onChange={(event) => setSearch(event.target.value)} />
        </label>
        <p className="muted" role="status">{prerequisites.length} selecionado(s)</p>
        {options.length ? (
          <div className="prerequisite-options">
            {options.map((item) => (
              <label className="check-field" key={item.id}>
                <input type="checkbox" checked={prerequisites.includes(item.id)}
                  onChange={(event) => {
                    const checked = event.target.checked;
                    setPrerequisites((current) => checked
                      ? [...current, item.id]
                      : current.filter((id) => id !== item.id));
                  }} />
                <span>{item.id} · {item.name}</span>
              </label>
            ))}
          </div>
        ) : <p className="muted">Nenhuma disciplina encontrada.</p>}
        <label className="check-field">
          <input name="requirementsKnown" type="checkbox" defaultChecked={subject?.requirementsKnown ?? true} />
          <span>Pré-requisitos conferidos</span>
        </label>
        <p className="muted">Marque após confirmar a lista, inclusive quando a disciplina não tiver pré-requisitos.</p>
      </fieldset>
      {feedback}
      <div className="actions">
        <button type="submit">Salvar disciplina</button>
        <button type="button" className="secondary" onClick={onCancel}>Cancelar</button>
      </div>
    </fieldset>
  </form>;
}

function OfferingForm({ offering, catalog, subjects, busy, onCancel, onSave, feedback }: FormProps & {
  offering?: Offering;
  subjects: Subject[];
  onSave: (id: string, data: Omit<Offering, "id">) => void;
}) {
  const [times, setTimes] = useState(offering?.times ?? [emptyTime]);
  function updateTime(index: number, field: keyof typeof emptyTime, value: string) {
    setTimes((current) => current.map((time, position) => position === index ? { ...time, [field]: value } : time));
  }
  function submit(event: FormEvent<HTMLFormElement>) {
    event.preventDefault();
    const fields = new FormData(event.currentTarget);
    const id = offering?.id ?? String(fields.get("code")).trim();
    if (!offering && catalog.offerings.some((item) => item.id === id)) {
      const code = event.currentTarget.elements.namedItem("code") as HTMLInputElement;
      code.setCustomValidity("Este código já está em uso.");
      code.reportValidity();
      return;
    }
    onSave(id, {
      subjectId: offering?.subjectId ?? String(fields.get("subject")),
      group: String(fields.get("group")).trim(),
      capacity: Number(fields.get("capacity")),
      times,
    });
  }

  return <form className="panel coordinator-form" onSubmit={submit}>
    <h2>{offering ? `Editar ${offering.id}` : "Cadastrar turma"}</h2>
    <fieldset disabled={busy}>
      <label>Disciplina
        <select name="subject" defaultValue={offering?.subjectId ?? ""} disabled={!!offering} autoFocus={!offering} required>
          <option value="" disabled>Selecione</option>
          {subjects.map((item) => <option key={item.id} value={item.id}>{item.id} · {item.name}</option>)}
        </select>
      </label>
      <div className="coordinator-fields">
        <label>Código da turma
          <input name="code" defaultValue={offering?.id} readOnly={!!offering} required maxLength={100}
            pattern="[A-Za-z0-9_\x2D]+" onInput={(event) => event.currentTarget.setCustomValidity("")} />
        </label>
        <label>Nome da turma <input name="group" defaultValue={offering?.group} autoFocus={!!offering} required maxLength={40} /></label>
        <label>Vagas <input name="capacity" type="number" defaultValue={offering?.capacity ?? 0} min={0} max={10000} required /></label>
      </div>
      {times.map((time, index) => <fieldset className="coordinator-slot" key={index}>
        <legend>Horário {index + 1}</legend>
        <div className="coordinator-fields">
          <label>Dia <select value={time.day} onChange={(event) => updateTime(index, "day", event.target.value)}>
            {days.map((day) => <option key={day}>{day}</option>)}
          </select></label>
          <label>Início <input type="time" value={time.start} required
            onChange={(event) => updateTime(index, "start", event.target.value)} /></label>
          <label>Término <input type="time" value={time.end} required
            onChange={(event) => updateTime(index, "end", event.target.value)} /></label>
          <label>Sala <input value={time.room} required maxLength={100}
            onChange={(event) => updateTime(index, "room", event.target.value)} /></label>
        </div>
        {times.length > 1 && <button className="text-button" type="button"
          onClick={() => setTimes((current) => current.filter((_, position) => position !== index))}>Remover horário</button>}
      </fieldset>)}
      {feedback}
      <div className="actions">
        <button type="button" className="secondary" disabled={times.length >= 20}
          onClick={() => setTimes((current) => [...current, emptyTime])}>Adicionar horário</button>
        <button type="submit">Salvar turma</button>
        <button type="button" className="secondary" onClick={onCancel}>Cancelar</button>
      </div>
    </fieldset>
  </form>;
}
