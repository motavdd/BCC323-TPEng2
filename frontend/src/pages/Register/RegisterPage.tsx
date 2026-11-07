import "../Access/Access.css";
import "./Register.css";
import { useState, type FormEvent } from "react";
import { useRegistration } from "../../hooks/useRegistration";
import type {
  RegistrationData,
  RegistrationField,
} from "../../types/registration";
import {
  COURSE_MAX_LENGTH,
  NAME_MAX_LENGTH,
  PASSWORD_MIN_LENGTH,
  PERIOD_MAX,
  PERIOD_MIN,
} from "../../utils/registrationValidation";

type Props = {
  /** Cria a conta e o perfil; rejeita a Promise em caso de falha. */
  onRegister: (data: RegistrationData) => Promise<void>;
  /** Volta para a tela de login. */
  onBackToLogin: () => void;
};

const INITIAL_DATA: RegistrationData = {
  name: "",
  email: "",
  password: "",
  passwordConfirmation: "",
  course: "Ciência da Computação",
  period: 1,
};

/** Mensagem de erro exibida logo abaixo do campo inválido. */
function FieldError({ id, text }: { id: string; text?: string }) {
  if (!text) return null;
  return (
    <p id={id} className="field-error">
      {text}
    </p>
  );
}

/** Formulário de autocadastro do aluno (issue #16). */
export function RegisterPage({ onRegister, onBackToLogin }: Props) {
  const [data, setData] = useState<RegistrationData>(INITIAL_DATA);
  const [visible, setVisible] = useState(false);
  const { state, fieldErrors, submit, clearFieldError } =
    useRegistration(onRegister);
  const busy = state.status === "submitting";

  function update<K extends RegistrationField>(
    field: K,
    value: RegistrationData[K],
  ) {
    setData((current) => ({ ...current, [field]: value }));
    clearFieldError(field);
  }

  /** Atributos de acessibilidade que ligam o campo à sua mensagem de erro. */
  function errorProps(field: RegistrationField) {
    const invalid = Boolean(fieldErrors[field]);
    return {
      "aria-invalid": invalid,
      "aria-describedby": invalid ? `${field}-error` : undefined,
    };
  }

  function handleSubmit(event: FormEvent) {
    event.preventDefault();
    void submit(data);
  }

  return (
    <main className="access register">
      <aside className="access-brand">
        <strong>MyPlanner</strong>
      </aside>
      <section className="access-body">
        <header>
          <h1>Crie sua conta</h1>
          <p>Informe seus dados e o ponto em que você está no curso.</p>
        </header>
        <form
          className="panel access-form register-form"
          onSubmit={handleSubmit}
          noValidate
        >
          <fieldset disabled={busy}>
            <legend>Dados de acesso</legend>

            <label htmlFor="name">
              Nome completo
              <input
                id="name"
                autoComplete="name"
                placeholder="Como você quer ser chamado"
                value={data.name}
                onChange={(e) => update("name", e.target.value)}
                maxLength={NAME_MAX_LENGTH}
                required
                {...errorProps("name")}
              />
            </label>
            <FieldError id="name-error" text={fieldErrors.name} />

            <label htmlFor="email">
              E-mail
              <input
                id="email"
                type="email"
                autoComplete="email"
                placeholder="seuemail@exemplo.com"
                value={data.email}
                onChange={(e) => update("email", e.target.value)}
                maxLength={254}
                required
                {...errorProps("email")}
              />
            </label>
            <FieldError id="email-error" text={fieldErrors.email} />

            <label htmlFor="password">Senha</label>
            <div className="password-field">
              <input
                id="password"
                type={visible ? "text" : "password"}
                autoComplete="new-password"
                placeholder={`Pelo menos ${PASSWORD_MIN_LENGTH} caracteres`}
                value={data.password}
                onChange={(e) => update("password", e.target.value)}
                minLength={PASSWORD_MIN_LENGTH}
                required
                {...errorProps("password")}
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
            <FieldError id="password-error" text={fieldErrors.password} />

            <label htmlFor="passwordConfirmation">
              Confirmar senha
              <input
                id="passwordConfirmation"
                type={visible ? "text" : "password"}
                autoComplete="new-password"
                placeholder="Repita a senha"
                value={data.passwordConfirmation}
                onChange={(e) => update("passwordConfirmation", e.target.value)}
                required
                {...errorProps("passwordConfirmation")}
              />
            </label>
            <FieldError
              id="passwordConfirmation-error"
              text={fieldErrors.passwordConfirmation}
            />
          </fieldset>

          <fieldset disabled={busy}>
            <legend>Dados acadêmicos</legend>

            <label htmlFor="course">
              Curso
              <input
                id="course"
                value={data.course}
                onChange={(e) => update("course", e.target.value)}
                maxLength={COURSE_MAX_LENGTH}
                required
                {...errorProps("course")}
              />
            </label>
            <FieldError id="course-error" text={fieldErrors.course} />

            <label htmlFor="period">
              Período atual
              <input
                id="period"
                type="number"
                inputMode="numeric"
                min={PERIOD_MIN}
                max={PERIOD_MAX}
                step={1}
                value={Number.isNaN(data.period) ? "" : data.period}
                onChange={(e) => update("period", e.target.valueAsNumber)}
                required
                {...errorProps("period")}
              />
            </label>
            <FieldError id="period-error" text={fieldErrors.period} />
            {!fieldErrors.period && (
              <p className="muted field-hint">
                Informe um período de {PERIOD_MIN} a {PERIOD_MAX}.
              </p>
            )}
          </fieldset>

          {state.status === "error" && (
            <p role="alert" className="notice danger">
              {state.message}
            </p>
          )}

          <div className="actions">
            <button type="submit" disabled={busy}>
              {busy ? "Aguarde…" : "Criar conta"}
            </button>
            <button
              type="button"
              className="secondary"
              disabled={busy}
              onClick={onBackToLogin}
            >
              Já tenho conta
            </button>
          </div>
        </form>
        <footer>Planejamento acadêmico · Não realiza matrícula</footer>
      </section>
    </main>
  );
}
