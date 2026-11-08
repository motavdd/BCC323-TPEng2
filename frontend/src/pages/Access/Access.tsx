import "./Access.css";
import { useState, type FormEvent } from "react";
import { signInWithEmailAndPassword } from "firebase/auth";
import { auth } from "../../services/firebase";
import { message } from "../../utils/errors";

export function Access({
  onCreateAccount,
}: {
  onCreateAccount?: () => void;
}) {
  const [email, setEmail] = useState("");
  const [password, setPassword] = useState("");
  const [visible, setVisible] = useState(false);
  const [error, setError] = useState("");
  const [busy, setBusy] = useState(false);

  async function submit(event: FormEvent) {
    event.preventDefault();
    setError("");

    if (!email.trim() || !password) {
      setError("Por favor, preencha o e-mail e a senha.");
      return;
    }

    setBusy(true);
    try {
      await signInWithEmailAndPassword(auth, email, password);
    } catch (e) {
      setError(message(e));
    } finally {
      setBusy(false);
    }
  }

  return (
    <main className="access">
      <aside className="access-brand">
        <strong>MyPlanner</strong>
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
      </aside>

      <section className="access-body">
        <header>
          <h1>Seja Bem Vindo</h1>
          <p>Entre para continuar seu planejamento.</p>
        </header>

        <form className="access-form" onSubmit={(event) => void submit(event)}>
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
              autoComplete="current-password"
              placeholder="Digite sua senha"
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

          {error && (
            <p role="alert" className="notice danger">
              {error}
            </p>
          )}

          <div className="actions">
            <button type="submit" disabled={busy}>
              {busy ? "Aguarde…" : "Entrar"}
            </button>
          </div>
        </form>

        {onCreateAccount && (
          <button
            type="button"
            className="text-button switch-access"
            onClick={onCreateAccount}
          >
            Primeiro acesso? Criar conta
          </button>
        )}
      </section>
    </main>
  );
}