import "./App.css";
import { useEffect, useRef, useState } from "react";
import {
  createUserWithEmailAndPassword,
  onAuthStateChanged,
  signOut,
  type User,
} from "firebase/auth";
import { auth } from "../services/firebase";
import { academicApi } from "../services/academic";
import { Access } from "../pages/Access/Access";
import { Planner } from "../pages/planner/Planner";

export default function App() {
  const [user, setUser] = useState<User | null | undefined>(undefined);
  const registering = useRef(false);
  useEffect(
    () =>
      onAuthStateChanged(auth, (value) => {
        if (!registering.current) setUser(value);
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
    <Access
      registerAccount={async (email, password, course, period) => {
        registering.current = true;
        try {
          await createUserWithEmailAndPassword(auth, email, password);
          await academicApi.saveProfile({ course: course.trim(), period });
        } finally {
          registering.current = false;
          setUser(auth.currentUser);
        }
      }}
    />
  );
}
