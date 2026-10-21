import "./Planner.css";
import type { User } from "firebase/auth";
import { usePlanner } from "../../hooks/usePlanner";
import { links } from "./constants";
import { OverviewScreen } from "./OverviewScreen/OverviewScreen";
import { MatrixScreen } from "./MatrixScreen/MatrixScreen";
import { HistoryScreen } from "./HistoryScreen/HistoryScreen";
import { PlanScreen } from "./PlanScreen/PlanScreen";
import { SavedPlanScreen } from "./SavedPlanScreen/SavedPlanScreen";
import { SubjectScreen } from "./SubjectScreen/SubjectScreen";
import { ProfileForm } from "../../components/ProfileForm/ProfileForm";
import { Panel } from "../../components/Panel/Panel";

export function Planner({
  user,
  onSignOut,
}: {
  user: User;
  onSignOut: () => Promise<void>;
}) {
  const {
    route,
    go,
    profile,
    catalog,
    completed,
    evaluation,
    plan,
    savedPlan,
    error,
    notice,
    busy,
    search,
    setSearch,
    confirmRemoval,
    setConfirmRemoval,
    chooser,
    setChooser,
    dirty,
    perform,
    subject,
    state,
    selected,
    missingOfferings,
    conflicts,
    invalidPlan,
    planHours,
    title,
    toggleCompleted,
    savePlan,
    addOffering,
    setPlan,
    saveProfile,
    retryLoad,
  } = usePlanner(user);
  const navRoute = route.startsWith("subject/")
    ? "matrix"
    : route === "saved"
      ? "plan"
      : route;

  function renderContent() {
    if (profile === undefined) return <p role="status">Carregando perfil…</p>;
    if (profile === null || route === "profile") {
      return (
        <ProfileForm
          key={profile === null ? "new" : "existing"}
          user={user}
          profile={profile}
          busy={busy}
          save={saveProfile}
        />
      );
    }
    if (!catalog || !completed || !evaluation) {
      return <p role="status">Carregando dados acadêmicos…</p>;
    }
    return (
      <>
        <p className="session" role="status">
          Sessão iniciada: {user.email}
        </p>
        {profile.course !== catalog.name ? (
          <Panel tone="warning">
            <h2>Matriz ainda não cadastrada</h2>
            <p>
              O recorte disponível corresponde a {catalog.name}. Atualize seu
              curso no perfil ou aguarde o cadastro da sua matriz.
            </p>
            <button onClick={() => go("profile")}>Abrir perfil</button>
          </Panel>
        ) : (
          <>
            {route === "overview" && (
              <OverviewScreen
                evaluation={evaluation}
                savedPlan={savedPlan}
                go={go}
              />
            )}
            {route === "matrix" && (
              <MatrixScreen
                catalog={catalog}
                evaluation={evaluation}
                search={search}
                setSearch={setSearch}
                go={go}
              />
            )}
            {subject && state && (
              <SubjectScreen
                subject={subject}
                state={state}
                catalog={catalog}
                completed={completed}
                confirmRemoval={confirmRemoval}
                setConfirmRemoval={setConfirmRemoval}
                busy={busy}
                toggleCompleted={toggleCompleted}
                go={go}
                setChooser={setChooser}
              />
            )}
            {route.startsWith("subject/") && !subject && (
              <Panel>
                <h2>Disciplina não encontrada</h2>
                <button onClick={() => go("matrix")}>Voltar à matriz</button>
              </Panel>
            )}
            {route === "history" && (
              <HistoryScreen
                catalog={catalog}
                completed={completed}
                evaluation={evaluation}
                go={go}
              />
            )}
            {route === "plan" && (
              <PlanScreen
                catalog={catalog}
                evaluation={evaluation}
                plan={plan}
                setPlan={setPlan}
                dirty={dirty}
                busy={busy}
                conflicts={conflicts}
                invalidPlan={invalidPlan}
                missingOfferings={missingOfferings}
                selected={selected}
                planHours={planHours}
                savePlan={savePlan}
                addOffering={addOffering}
                chooser={chooser}
                setChooser={setChooser}
              />
            )}
            {route === "saved" && (
              <SavedPlanScreen
                catalog={catalog}
                selected={selected}
                planHours={planHours}
                go={go}
              />
            )}
            {!links.some(([id]) => id === route) &&
              !subject &&
              !route.startsWith("subject/") &&
              route !== "saved" && (
                <button onClick={() => go("overview")}>
                  Voltar à visão geral
                </button>
              )}
          </>
        )}
      </>
    );
  }

  return (
    <div className="app-shell">
      <aside className="sidebar">
        <a className="brand" href="#overview">
          MyPlanner
        </a>
        <p>Planejamento acadêmico</p>
        <nav aria-label="Navegação principal">
          {links.map(([id, label]) => (
            <a
              key={id}
              href={`#${id}`}
              aria-current={navRoute === id ? "page" : undefined}
            >
              {label}
            </a>
          ))}
        </nav>
        <button
          className="text-button logout"
          disabled={busy}
          onClick={() => {
            if (
              dirty &&
              !window.confirm(
                "Há alterações no plano que não foram salvas. Sair mesmo assim?",
              )
            )
              return;

            void perform(onSignOut);
          }}
        >
          Sair da conta
        </button>
      </aside>
      <main className="workspace">
        <header>
          <h1>{profile === null ? "Falta só um passo" : title}</h1>
          <p>
            {profile === null
              ? "Sua conta já existe. Complete os dados para continuar."
              : route === "profile"
                ? "Mantenha curso e período atualizados."
                : subject
                  ? `${subject.id} · ${subject.period}º período · ${subject.hours} horas`
                  : `${profile?.course || ""}${profile ? ` · ${profile.period}º período` : ""}`}
          </p>
        </header>
        {error && (
          <div role="alert" className="notice danger">
            <p>{error}</p>
            <button className="secondary" disabled={busy} onClick={retryLoad}>
              Tentar novamente
            </button>
          </div>
        )}
        {notice && (
          <p role="status" className="notice success">
            {notice}
          </p>
        )}
        {renderContent()}
        <footer>
          Recorte ilustrativo · Validar matriz, carga horária, ofertas e
          pré-requisitos com a coordenação. Não realiza matrícula.
        </footer>
      </main>
    </div>
  );
}
