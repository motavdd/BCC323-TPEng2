import "./ProfileForm.css";
import { useState, type FormEvent } from "react";
import type { User } from "firebase/auth";
import type { Profile } from "../../services/api";
export function ProfileForm({
  user,
  profile,
  busy,
  save,
}: {
  user: User;
  profile: Profile | null;
  busy: boolean;
  save: (course: string, period: number) => Promise<void>;
}) {
  const [course, setCourse] = useState(
    profile?.course || "Ciência da Computação",
  );
  const [period, setPeriod] = useState(profile?.period || 1);
  function submit(event: FormEvent) {
    event.preventDefault();
    if (course.trim()) void save(course.trim(), period);
  }
  return (
    <form className="panel profile-form" onSubmit={submit}>
      <h2>Dados da conta</h2>
      <p>E-mail: {user.email}</p>
      {!profile && (
        <p className="notice warning">
          Complete seu perfil. Se o salvamento falhar, tente novamente sem criar
          outra conta.
        </p>
      )}
      <label htmlFor="course">
        Curso
        <input
          id="course"
          value={course}
          onChange={(e) => setCourse(e.target.value)}
          required
          maxLength={100}
        />
      </label>
      <label htmlFor="period">
        Período atual
        <input
          id="period"
          type="number"
          min={1}
          max={20}
          step={1}
          value={period}
          onChange={(e) => setPeriod(Number(e.target.value))}
          required
        />
      </label>
      <p className="amber">
        Mudar de curso pode alterar a matriz e a elegibilidade.
      </p>
      <button disabled={busy} type="submit">
        {busy
          ? "Salvando…"
          : profile
            ? "Salvar alterações"
            : "Salvar perfil e continuar"}
      </button>
    </form>
  );
}
