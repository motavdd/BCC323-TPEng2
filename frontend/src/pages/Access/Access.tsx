import "./Access.css";
import { useState, type FormEvent } from "react";
import { signInWithEmailAndPassword } from "firebase/auth";
import { auth } from "../../services/firebase";
import { message } from "../../utils/errors";

export function Access({
  registerAccount,
}: {
  registerAccount: (
    email: string,
    password: string,
    course: string,
    period: number,
  ) => Promise<void>;
}) {
  const [register, setRegister] = useState(false);
  const [email, setEmail] = useState("");
  const [password, setPassword] = useState("");
  const [visible, setVisible] = useState(false);
  const [course, setCourse] = useState("Ciência da Computação");
  const [period, setPeriod] = useState(1);
  const [error, setError] = useState("");
  const [busy, setBusy] = useState(false);
  async function submit(event: FormEvent) {
    event.preventDefault();
    setError("");
    setBusy(true);
    try {
      if (register) {
        await registerAccount(email, password, course, period);
      } else await signInWithEmailAndPassword(auth, email, password);
    } catch (e) {
      setError(message(e));
    } finally {
      setBusy(false);
    }
  }
  return (
    <main className={register ? "access register" : "access"}>
      <aside className="access-brand">
        <strong>MyPlanner</strong>
        {!register && (
          <div>
            <h1>
              Seu próximo semestre,
              <br />
              com mais clareza.
            </h1>
            <p>Acompanhe seu progresso.</p>
            <p>Conheça suas possibilidades.</p>
            <p>Organize uma grade sem conflitos.</p>
          </div>
        )}
      </aside>
      <section className="access-body">
        <header>
          <h1>{register ? "Crie sua conta" : "Seja Bem Vindo"}</h1>
          <p>
            {register
              ? "Comece pelo seu curso e período atual."
              : "Entre para continuar seu planejamento."}
          </p>
        </header>
        <form
          className={register ? "panel access-form" : "access-form"}
          onSubmit={(event) => void submit(event)}
        >
          <label htmlFor="email">
            E-mail
            <input
              id="email"
              type="email"
              autoComplete="email"
              placeholder="seuemail@exemplo.com"
              value={email}
              onChange={(e) => setEmail(e.target.value)}
              required
              maxLength={254}
            />
          </label>
          <label htmlFor="password">Senha</label>
          <div className="password-field">
            <input
              id="password"
              type={visible ? "text" : "password"}
              autoComplete={register ? "new-password" : "current-password"}
              placeholder={
                register ? "Pelo menos 6 caracteres" : "Digite sua senha"
              }
              value={password}
              onChange={(e) => setPassword(e.target.value)}
              minLength={6}
              required
            />
            <button
              type="button"
              className="text-button"
              aria-pressed={visible}
              aria-label={visible ? "Ocultar senha" : "Mostrar senha"}
              onClick={() => setVisible(!visible)}
            >
              {visible ? "Ocultar" : "Mostrar"}
            </button>
          </div>
          {register && (
            <>
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
              <p className="muted">Informe um período de 1 a 20.</p>
            </>
          )}
          {error && (
            <p role="alert" className="notice danger">
              {error}
            </p>
          )}
          <div className="actions">
            <button type="submit" disabled={busy}>
              {busy ? "Aguarde…" : register ? "Criar conta" : "Entrar"}
            </button>
            {register && (
              <button
                type="button"
                className="secondary"
                onClick={() => {
                  setRegister(false);
                  setError("");
                }}
              >
                Já tenho conta
              </button>
            )}
          </div>
        </form>
        {!register && (
          <button
            className="text-button switch-access"
            onClick={() => {
              setRegister(true);
              setError("");
            }}
          >
            Primeiro acesso? Criar conta
          </button>
        )}
        {register && (
          <footer>Planejamento acadêmico · Não realiza matrícula</footer>
        )}
      </section>
    </main>
  );
}
