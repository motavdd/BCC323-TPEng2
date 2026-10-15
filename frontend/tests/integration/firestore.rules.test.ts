import { afterAll, afterEach, beforeAll, describe, it } from 'vitest';
import { readFileSync } from 'node:fs';
import { resolve } from 'node:path';
import { assertFails, assertSucceeds, initializeTestEnvironment, type RulesTestEnvironment } from '@firebase/rules-unit-testing';
import { doc, getDoc, serverTimestamp, setDoc } from 'firebase/firestore';

let env: RulesTestEnvironment;
const rulesPath = process.env.FIRESTORE_RULES_PATH ?? resolve(process.cwd(), '../firebase/firestore.rules');

beforeAll(async () => {
  env = await initializeTestEnvironment({
    projectId: process.env.FIREBASE_PROJECT_ID ?? 'demo-bcc323',
    firestore: {
      host: (process.env.FIRESTORE_EMULATOR_HOST ?? 'localhost:8080').split(':')[0],
      port: Number((process.env.FIRESTORE_EMULATOR_HOST ?? 'localhost:8080').split(':')[1]),
      rules: readFileSync(rulesPath, 'utf8'),
    },
  });
});
afterEach(async () => { await env.clearFirestore(); });
afterAll(async () => { await env?.cleanup(); });

describe('Regras Firestore', () => {
  const profile = { displayName: '', course: 'Computação', period: 1, createdAt: serverTimestamp() };
  it('permite que o aluno crie o próprio perfil', async () => {
    const db = env.authenticatedContext('aluno-1').firestore();
    await assertSucceeds(setDoc(doc(db, 'users/aluno-1'), profile));
  });
  it('impede escrita em perfil de outra pessoa', async () => {
    const db = env.authenticatedContext('aluno-2').firestore();
    await assertFails(setDoc(doc(db, 'users/aluno-1'), profile));
  });
  it('impede acesso anônimo a perfis', async () => {
    const db = env.unauthenticatedContext().firestore();
    await assertFails(getDoc(doc(db, 'users/aluno-1')));
  });
  it('impede que aluno altere catálogo de disciplinas', async () => {
    const db = env.authenticatedContext('aluno-1').firestore();
    await assertFails(setDoc(doc(db, 'courses/BCC323'), { name: 'TPEng2' }));
  });
});
