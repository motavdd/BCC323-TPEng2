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

const institutional = { email: 'aluno@aluno.ufop.edu.br', email_verified: true };

describe('Regras Firestore', () => {
  const profile = { displayName: '', course: 'Computação', period: 1, createdAt: serverTimestamp() };
  it('permite que o aluno crie o próprio perfil', async () => {
    const db = env.authenticatedContext('aluno-1', institutional).firestore();
    await assertSucceeds(setDoc(doc(db, 'users/aluno-1'), profile));
  });
  it('impede escrita em perfil de outra pessoa', async () => {
    const db = env.authenticatedContext('aluno-2', institutional).firestore();
    await assertFails(setDoc(doc(db, 'users/aluno-1'), profile));
  });
  it('impede acesso anônimo a perfis', async () => {
    const db = env.unauthenticatedContext().firestore();
    await assertFails(getDoc(doc(db, 'users/aluno-1')));
  });
  it('aceita courseId válido e impede tipo inválido no perfil do MVP', async () => {
    const db = env.authenticatedContext('aluno-1', institutional).firestore();
    await assertSucceeds(setDoc(doc(db, 'users/aluno-1'), { ...profile, courseId: 'computacao-exemplo' }));
    await assertFails(setDoc(doc(db, 'users/aluno-1'), { ...profile, courseId: 123 }));
  });
  it('impede que aluno altere catálogo de disciplinas', async () => {
    const db = env.authenticatedContext('aluno-1', institutional).firestore();
    await assertFails(setDoc(doc(db, 'courses/BCC323'), { name: 'TPEng2' }));
  });
  it('salva e recupera o plano apenas para o dono', async () => {
    const db = env.authenticatedContext('aluno-1', institutional).firestore();
    const ref = doc(db, 'users/aluno-1/plans/next');
    await assertSucceeds(setDoc(ref, { offeringIds: ['BCC202-A'], updatedAt: serverTimestamp() }));
    await assertSucceeds(getDoc(ref));
    const other = env.authenticatedContext('aluno-2', institutional).firestore();
    await assertFails(getDoc(doc(other, 'users/aluno-1/plans/next')));
    await assertFails(setDoc(doc(other, 'users/aluno-1/plans/next'), { offeringIds: [], updatedAt: serverTimestamp() }));
  });
  it('rejeita turmas duplicadas, desconhecidas e campos extras no plano', async () => {
    const db = env.authenticatedContext('aluno-1', institutional).firestore();
    const ref = doc(db, 'users/aluno-1/plans/next');
    for (const offeringIds of [['BCC202-A', 'BCC202-A'], ['BCC202-A', 'BCC202-B'], ['invalid']]) {
      await assertFails(setDoc(ref, { offeringIds, updatedAt: serverTimestamp() }));
    }
    await assertFails(setDoc(ref, { offeringIds: [], updatedAt: serverTimestamp(), admin: true }));
  });

  it('impede concessão de privilégios e acesso direto de escrita pelo coordenador', async () => {
    await env.withSecurityRulesDisabled(async (context) => {
      await setDoc(doc(context.firestore(), 'coordinators/coord'), { courseId: 'course-a' });
    });
    const coordinator = env.authenticatedContext('coord', { ...institutional, email: 'coord@ufop.edu.br' }).firestore();
    const student = env.authenticatedContext('aluno-1', institutional).firestore();
    await assertSucceeds(getDoc(doc(coordinator, 'coordinators/coord')));
    await assertFails(getDoc(doc(student, 'coordinators/coord')));
    await assertFails(setDoc(doc(student, 'coordinators/aluno-1'), { courseId: 'course-a' }));
    await assertFails(setDoc(doc(student, 'users/aluno-1'), { ...profile, role: 'coordinator' }));
    await assertFails(setDoc(doc(coordinator, 'coordinators/coord'), { courseId: 'course-b' }));
    await assertFails(setDoc(doc(coordinator, 'courses/course-a'), { name: 'Curso A' }));
    await assertFails(getDoc(doc(coordinator, 'users/aluno-1')));
  });

  it('isola dados acadêmicos de e-mails externos e não confirmados', async () => {
    const outside = env.authenticatedContext('outside', { ...institutional, email: 'aluno@example.test' }).firestore();
    await assertFails(setDoc(doc(outside, 'users/outside'), profile));
    await assertFails(getDoc(doc(outside, 'courses/course-a')));
    const pending = env.authenticatedContext('pending', { ...institutional, email_verified: false }).firestore();
    await assertSucceeds(setDoc(doc(pending, 'users/pending'), profile));
    await assertFails(setDoc(doc(pending, 'users/pending/completedCourses/BCC201'), { completedAt: serverTimestamp() }));
    await assertFails(getDoc(doc(pending, 'coordinators/pending')));
  });
});
