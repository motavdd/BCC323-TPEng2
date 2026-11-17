import { useState } from "react";
import { sendEmailVerification, type User } from "firebase/auth";
import { isInstitutionalEmail } from "../../utils/registrationValidation";
import { message } from "../../utils/errors";

export function VerifyEmail({ user, onVerified, onSignOut }: {
  user: User;
  onVerified: () => void;
  onSignOut: () => Promise<void>;
}) {
  const [busy, setBusy] = useState(false);
  const [error, setError] = useState("");
  const [notice, setNotice] = useState("");
  const institutional = isInstitutionalEmail(user.email ?? "");

  async function perform(action: () => Promise<void>) {
    setBusy(true);
    setError("");
    setNotice("");
    try {
      await action();
    } catch (cause) {
      setError(message(cause));
    } finally {
      setBusy(false);
    }
  }

  return (
    <main className="workspace verification">
      <section className="panel">
        <h1>{institutional ? "Confirme seu e-mail institucional" : "Use seu e-mail da UFOP"}</h1>
        {institutional ? (
          <>
            <p>Abra o link de confirmação de {user.email} e volte a esta tela. Se precisar, solicite outro link.</p>
            <div className="actions">
              <button disabled={busy} onClick={() => void perform(async () => {
                await user.reload();
                if (!user.emailVerified) throw new Error("A confirmação ainda está pendente.");
                await user.getIdToken(true);
                onVerified();
              })}>
                Já confirmei
              </button>
              <button className="secondary" disabled={busy} onClick={() => void perform(async () => {
                await sendEmailVerification(user);
                setNotice("Link de confirmação enviado.");
              })}>
                Enviar link de confirmação
              </button>
            </div>
          </>
        ) : (
          <p>Entre com uma conta de @aluno.ufop.edu.br ou @ufop.edu.br.</p>
        )}
        {notice && <p role="status">{notice}</p>}
        {error && <p role="alert" className="notice danger">{error}</p>}
        <button className="text-button" disabled={busy} onClick={() => void perform(onSignOut)}>Sair da conta</button>
      </section>
    </main>
  );
}
