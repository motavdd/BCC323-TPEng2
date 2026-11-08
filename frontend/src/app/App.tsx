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

  useEffect(
    () =>
      onAuthStateChanged(auth, (value) => {
        setUser(value);
      }),
    [],
  );

  if (user === undefined)
    return (
      <main className="loading" role="status">
        Carregando sua sessão…
      </main>
    );

  return user ? (
    <Planner key={user.uid} user={user} onSignOut={() => signOut(auth)} />
  ) : (
    <Access />
  );
}