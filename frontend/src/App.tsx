import { useEffect, useState, type FormEvent } from 'react';
import { createUserWithEmailAndPassword, onAuthStateChanged, signInWithEmailAndPassword, signOut, type User } from 'firebase/auth';
import { doc, serverTimestamp, setDoc } from 'firebase/firestore';
import { Welcome } from './components/Welcome';
import { auth, db } from './lib/firebase';

export default function App() {
  const [user, setUser] = useState<User | null | undefined>(undefined);
  const [email, setEmail] = useState('');
  const [password, setPassword] = useState('');
  const [course, setCourse] = useState('Ciência da Computação');
  const [period, setPeriod] = useState(1);
  const [error, setError] = useState('');
  const [busy, setBusy] = useState(false);

  useEffect(() => onAuthStateChanged(auth, setUser), []);

  async function submit(event: FormEvent<HTMLFormElement>, register: boolean) {
    event.preventDefault();
    setError('');
    setBusy(true);
    try {
      if (register) {
        const result = await createUserWithEmailAndPassword(auth, email, password);
        await setDoc(doc(db, 'users', result.user.uid), {
          displayName: '', course, period, createdAt: serverTimestamp(),
        });
      } else {
        await signInWithEmailAndPassword(auth, email, password);
      }
    } catch (e) {
      setError(e instanceof Error ? e.message : 'Falha na autenticação');
    } finally {
      setBusy(false);
    }
  }

  return (
    <main style={{ maxWidth: 520, margin: '3rem auto', fontFamily: 'sans-serif' }}>
      <h1>Planejamento Acadêmico</h1>
      <p>Protótipo técnico para validar CI, Firebase e Docker.</p>
      {user === undefined ? <p>Carregando...</p> : user ? (
        <section>
          <Welcome email={user.email ?? ''} />
          <button onClick={() => void signOut(auth)}>Sair</button>
        </section>
      ) : (
        <form onSubmit={(event) => void submit(event, (event.nativeEvent as SubmitEvent).submitter?.id === 'register')}>
          <label htmlFor="email">E-mail</label><br />
          <input id="email" type="email" value={email} onChange={e => setEmail(e.target.value)} required /><br />
          <label htmlFor="password">Senha</label><br />
          <input id="password" type="password" value={password} onChange={e => setPassword(e.target.value)} minLength={6} required /><br />
          <label htmlFor="course">Curso</label><br />
          <input id="course" value={course} onChange={e => setCourse(e.target.value)} required /><br />
          <label htmlFor="period">Período</label><br />
          <input id="period" type="number" min={1} max={20} value={period} onChange={e => setPeriod(Number(e.target.value))} required /><br /><br />
          <button id="register" type="submit" disabled={busy}>Cadastrar</button>{' '}
          <button id="login" type="submit" disabled={busy}>Entrar</button>
          {error && <p role="alert">{error}</p>}
        </form>
      )}
    </main>
  );
}
