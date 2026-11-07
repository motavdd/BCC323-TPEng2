import "./App.css";
import { useEffect, useRef, useState } from "react";
import { onAuthStateChanged, signOut, type User } from "firebase/auth";
import { auth } from "../services/firebase";
import { registerStudent } from "../services/authService";
import type { RegistrationData } from "../types/registration";
import { Access } from "../pages/Access/Access";
import { RegisterPage } from "../pages/Register/RegisterPage";
import { Planner } from "../pages/planner/Planner";

/** Tela exibida para quem ainda não entrou. */
type PublicScreen = "login" | "register";

export default function App() {
  const [user, setUser] = useState<User | null | undefined>(undefined);
  const [screen, setScreen] = useState<PublicScreen>("login");
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
  if (user)
    return (
      <Planner
        key={user.uid}
        user={user}
        onSignOut={() => {
          setScreen("login");
          return signOut(auth);
        }}
      />
    );
  return screen === "register" ? (
    <RegisterPage
      onRegister={registerAccount}
      onBackToLogin={() => setScreen("login")}
    />
  ) : (
    <Access onCreateAccount={() => setScreen("register")} />
  );
}
