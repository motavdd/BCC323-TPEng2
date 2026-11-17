import "./App.css";
import { useEffect, useRef, useState } from "react";
import { onAuthStateChanged, signOut, type User } from "firebase/auth";
import { auth } from "../services/firebase";
import { registerStudent } from "../services/authService";
import type { RegistrationData } from "../types/registration";
import { Access } from "../pages/Access/Access";
import { RegisterPage } from "../pages/Register/RegisterPage";
import { Planner } from "../pages/planner/Planner";
import { Coordinator } from "../pages/Coordinator/Coordinator";
import { VerifyEmail } from "../pages/VerifyEmail/VerifyEmail";
import { academicApi } from "../services/academic";
import { isInstitutionalEmail } from "../utils/registrationValidation";
import { message } from "../utils/errors";

/** Tela exibida para quem ainda não entrou. */
type PublicScreen = "login" | "register";

function AcademicWorkspace({ user, onSignOut }: { user: User; onSignOut: () => Promise<void> }) {
  const [role, setRole] = useState<"coordinator" | "student">();
  const [error, setError] = useState("");
  const [attempt, setAttempt] = useState(0);
  useEffect(() => {
    let active = true;
    academicApi.session().then(({ coordinatorCourseId }) => {
      if (active) setRole(coordinatorCourseId ? "coordinator" : "student");
    }).catch((cause) => {
      if (active) setError(message(cause));
    });
    return () => { active = false; };
  }, [attempt]);

  if (error) return (
    <main className="workspace">
      <p role="alert" className="notice danger">{error}</p>
      <div className="actions">
        <button onClick={() => { setError(""); setAttempt((value) => value + 1); }}>Tentar novamente</button>
        <button className="secondary" onClick={() => void onSignOut().catch((cause) => setError(message(cause)))}>Sair da conta</button>
      </div>
    </main>
  );
  if (!role) return <main className="loading" role="status">Carregando permissões…</main>;
  return role === "coordinator" ? <Coordinator onSignOut={onSignOut} /> : <Planner user={user} onSignOut={onSignOut} />;
}

export default function App() {
  const [user, setUser] = useState<User | null | undefined>(undefined);
  const [screen, setScreen] = useState<PublicScreen>("login");
  const [, refreshVerification] = useState(0);
  // Durante o autocadastro, só mostra o Planner depois de salvar o perfil.
  const registering = useRef(false);

  useEffect(
    () =>
      onAuthStateChanged(auth, (value) => {
        if (!registering.current) setUser(value);
      }),
    [],
  );

  async function registerAccount(data: RegistrationData) {
    registering.current = true;
    try {
      await registerStudent(data);
    } finally {
      registering.current = false;
      setUser(auth.currentUser);
    }
  }

  if (user === undefined)
    return (
      <main className="loading" role="status">
        Carregando sua sessão…
      </main>
    );

  if (user) {
    const logout = () => {
      setScreen("login");
      return signOut(auth);
    };
    if (!user.emailVerified || !isInstitutionalEmail(user.email ?? "")) {
      return <VerifyEmail key={user.uid} user={user} onSignOut={logout}
        onVerified={() => refreshVerification((version) => version + 1)} />;
    }
    return <AcademicWorkspace key={user.uid} user={user} onSignOut={logout} />;
  }

  return screen === "register" ? (
    <RegisterPage
      onRegister={registerAccount}
      onBackToLogin={() => setScreen("login")}
    />
  ) : (
    <Access onCreateAccount={() => setScreen("register")} />
  );
}
