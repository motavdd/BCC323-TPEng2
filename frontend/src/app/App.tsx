import "./App.css";
import { useEffect, useState } from "react";
import {
  onAuthStateChanged,
  signOut,
  type User,
} from "firebase/auth";
import { auth } from "../services/firebase";
import { Access } from "../pages/Access/Access";
import { Planner } from "../pages/planner/Planner";

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