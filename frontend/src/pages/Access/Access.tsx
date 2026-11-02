import "./Access.css";
import { useState, type FormEvent } from "react";
import { signInWithEmailAndPassword } from "firebase/auth";
import { auth } from "../../services/firebase";
import { message } from "../../utils/errors";

type FieldErrors = {
  email?: string;
  password?: string;
  course?: string;
  period?: string;
};

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
  const [fieldErrors, setFieldErrors] = useState<FieldErrors>({});
  const [busy, setBusy] = useState(false);

  // Função de validação local dos campos de entrada (Issue #18)
  function validateFields(): boolean {
    const errors: FieldErrors = {};

    // 1. Validação do E-mail
    if (!email.trim()) {
      errors.email = "O e-mail é obrigatório.";
    } else if (!/^[^\s@]+@[^\s@]+\.[^\s@]+$/.test(email.trim())) {
      errors.email = "Insira um e-mail válido (ex: usuario@dominio.com).";
    }

    // 2. Validação da Senha
    if (!password) {
      errors.password = "A senha é obrigatória.";
    } else if (password.length < 6) {
      errors.password = "A senha deve ter pelo menos 6 caracteres.";
    }

    // 3. Validações adicionais para o fluxo de Cadastro
    if (register) {
      if (!course.trim()) {
        errors.course = "Informe o nome do curso.";
      }
      if (!period || period < 1 || period > 20) {
        errors.period = "O período deve ser um número entre 1 e 20.";
      }
    }

    setFieldErrors(errors);
    return Object.keys(errors).length === 0;
  }

  async function submit(event: FormEvent) {
    event.preventDefault();
    setError("");
    setFieldErrors({});

    // Interrompe o envio caso a validação de campos falhe
    if (!validateFields()) {
      return;
    }

    setBusy(true);
    try {
      if (register) {
        await registerAccount(email, password, course, period);
      } else {
        await signInWithEmailAndPassword(auth, email, password);
      }
    } catch (e) {
      setError(message(e));
    } finally {
      setBusy(false);
    }
  }

  const resetFormState = () => {
    setError("");
    setFieldErrors({});
  };

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
          noValidate
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
              maxLength={254}
              className={fieldErrors.email ? "input-error" : ""}
            />
          </label>
          {fieldErrors.email && (
            <p role="alert" className="notice danger font-xs">
              {fieldErrors.email}
            </p>
          )}

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
              className={fieldErrors.password ? "input-error" : ""}
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
          {fieldErrors.password && (
            <p role="alert" className="notice danger font-xs">
              {fieldErrors.password}
            </p>
          )}

          {register && (
            <>
              <label htmlFor="course">
                Curso
                <input
                  id="course"
                  value={course}
                  onChange={(e) => setCourse(e.target.value)}
                  maxLength={100}
                  className={fieldErrors.course ? "input-error" : ""}
                />
              </label>
              {fieldErrors.course && (
                <p role="alert" className="notice danger font-xs">
                  {fieldErrors.course}
                </p>
              )}

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
                  className={fieldErrors.period ? "input-error" : ""}
                />
              </label>
              {fieldErrors.period ? (
                <p role="alert" className="notice danger font-xs">
                  {fieldErrors.period}
                </p>
              ) : (
                <p className="muted">Informe um período de 1 a 20.</p>
              )}
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
                  resetFormState();
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
              resetFormState();
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