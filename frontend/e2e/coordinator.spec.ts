import { expect, test } from '@playwright/test';
import { createAccount } from './auth-emulator';
import type { Curriculum } from '../src/services/api';

test('coordenação altera seu catálogo e os dados persistem no Firestore', async ({ page, request, browser, baseURL }) => {
  const suffix = Date.now();
  const courseId = `course-${suffix}`;
  const courseName = `Curso de teste ${suffix}`;
  const email = `coordenador-${suffix}@ufop.edu.br`;
  const password = 'senha-teste-123';
  const coordinator = await createAccount(request, email, password);
  const studentEmail = `aluno-${suffix}@aluno.ufop.edu.br`;
  const student = await createAccount(request, studentEmail, password);
  const pending = await createAccount(request, `pendente-${suffix}@aluno.ufop.edu.br`, password, false);
  const outside = await createAccount(request, `externo-${suffix}@example.test`, password, false);
  const headers = { Authorization: `Bearer ${coordinator.idToken}` };
  const studentHeaders = { Authorization: `Bearer ${student.idToken}` };
  const owner = { Authorization: 'Bearer owner' };
  const firestore = process.env.FIRESTORE_EMULATOR_HOST || '127.0.0.1:8080';
  const db = `http://${firestore}/v1/projects/demo-bcc323/databases/(default)/documents`;
  const courseUrl = `${db}/courses/${courseId}`;
  const grantUrl = `${db}/coordinators/${coordinator.localId}`;
  const auth = (idToken: string) => ({ Authorization: `Bearer ${idToken}` });
  const studentContext = await browser.newContext({ baseURL });

  expect((await request.get('/api/coordinator/curriculum')).status()).toBe(401);
  expect((await request.get('/api/coordinator/curriculum', { headers: studentHeaders })).status()).toBe(403);
  expect((await request.get('/api/me', { headers: auth(pending.idToken) })).status()).toBe(403);
  expect((await request.get('/api/profile', { headers: auth(outside.idToken) })).status()).toBe(403);
  const [head, payload] = coordinator.idToken.split('.');
  const claims = JSON.parse(Buffer.from(payload, 'base64url').toString());
  const forged = `${head}.${Buffer.from(JSON.stringify({ ...claims, aud: 'another-project' })).toString('base64url')}.`;
  expect((await request.get('/api/me', { headers: auth(forged) })).status()).toBe(401);

  expect((await request.patch(courseUrl, { headers: owner, data: { fields: {
    name: { stringValue: courseName },
    subjects: { arrayValue: { values: [{ mapValue: { fields: {
      id: { stringValue: 'BASE' }, name: { stringValue: 'Disciplina base' },
      hours: { integerValue: '60' }, period: { integerValue: '1' },
      requirementsKnown: { booleanValue: true },
    } } }] } },
  } } })).ok()).toBeTruthy();
  try {
    expect((await request.patch(grantUrl, { headers: owner, data: {
      fields: { courseId: { stringValue: courseId } },
    } })).ok()).toBeTruthy();
    expect((await request.patch(courseUrl, { headers, data: { fields: { name: { stringValue: 'Direto' } } } })).status()).toBe(403);
    expect((await request.put('/api/profile', { headers: studentHeaders,
      data: { course: courseName, period: 2, displayName: 'Aluno de teste' },
    })).ok()).toBeTruthy();
    const storedProfile = await (await request.get(`${db}/users/${student.localId}`, { headers: owner })).json();
    expect(storedProfile.fields).toMatchObject({
      courseId: { stringValue: courseId }, displayName: { stringValue: 'Aluno de teste' }, period: { integerValue: '2' },
    });
    expect(storedProfile.fields).not.toHaveProperty('password');

    await page.goto('/');
    await page.getByLabel('E-mail', { exact: true }).fill(email);
    await page.getByLabel('Senha', { exact: true }).fill(password);
    await page.getByRole('button', { name: 'Entrar', exact: true }).click();
    await expect(page.getByText(courseName)).toBeVisible();
    await page.getByRole('button', { name: 'Nova disciplina' }).click();
    await page.getByLabel('Código da disciplina').fill('NOVA');
    await page.getByLabel('Nome da disciplina').fill('Disciplina nova');
    await page.getByRole('checkbox', { name: 'BASE · Disciplina base' }).check();
    await page.getByRole('button', { name: 'Salvar disciplina' }).click();
    await expect(page.getByRole('heading', { name: 'NOVA · Disciplina nova' })).toBeVisible();
    await page.getByRole('button', { name: 'Editar disciplina NOVA' }).click();
    await page.getByLabel('Carga horária').fill('90');
    await page.getByRole('button', { name: 'Salvar disciplina' }).click();
    await expect(page.getByRole('status')).toHaveText('Alterações salvas.');

    const catalog = await (await request.get('/api/coordinator/curriculum', { headers })).json() as Curriculum & { revision: string };
    const put = (id: string, data: unknown, revision = catalog.revision) => request.put(`/api/coordinator/subjects/${id}`, {
      headers: { ...headers, 'If-Match': revision }, data,
    });
    const subject = { name: 'Disciplina base', hours: 60, period: 1, requirementsKnown: true, prerequisites: ['NOVA'] };
    expect((await put('BASE', subject)).status()).toBe(400);
    expect((await put('BASE', { ...subject, courseId: 'computacao-exemplo' })).status()).toBe(400);
    expect((await put('BASE', subject, 'stale')).status()).toBe(409);
    expect((await request.delete('/api/coordinator/subjects/BASE', {
      headers: { ...headers, 'If-Match': catalog.revision },
    })).status()).toBe(409);
    expect((await request.delete('/api/coordinator/subjects/NOVA', { headers: studentHeaders })).status()).toBe(403);

    expect((await request.put('/api/history/BASE', { headers: studentHeaders })).ok()).toBeTruthy();
    const studentPage = await studentContext.newPage();
    await studentPage.goto('/');
    await studentPage.getByLabel('E-mail', { exact: true }).fill(studentEmail);
    await studentPage.getByLabel('Senha', { exact: true }).fill(password);
    await studentPage.getByRole('button', { name: 'Entrar', exact: true }).click();
    await studentPage.getByRole('link', { name: 'Planejar semestre', exact: true }).click();
    await studentPage.getByRole('button', { name: 'Adicionar disciplina' }).click();
    await expect(studentPage.getByText('Nenhuma outra oferta disponível.', { exact: false })).toBeVisible();

    await page.getByRole('button', { name: 'Turmas e horários' }).click();
    await page.getByRole('button', { name: 'Nova turma' }).click();
    await page.getByRole('combobox', { name: 'Disciplina', exact: true }).selectOption('NOVA');
    await page.getByLabel('Código da turma').fill('NOVA-A');
    await page.getByLabel('Nome da turma').fill('A');
    await page.getByLabel('Vagas').fill('30');
    await page.getByLabel('Início').fill('08:30');
    await page.getByLabel('Término').fill('10:00');
    await page.getByLabel('Sala').fill('Sala 203');
    await page.getByRole('button', { name: 'Salvar turma' }).click();
    await expect(page.getByText('Seg 08:30–10:00 · Sala 203')).toBeVisible();
    await expect(studentPage.getByRole('button', { name: 'Usar turma A' })).toBeVisible();
    await page.getByRole('button', { name: 'Editar turma NOVA-A' }).click();
    await page.getByLabel('Vagas').fill('35');
    await page.getByRole('button', { name: 'Salvar turma' }).click();
    await expect(page.getByText('35 vagas')).toBeVisible();
    await page.reload();
    await page.getByRole('button', { name: 'Turmas e horários' }).click();
    await expect(page.getByText('35 vagas')).toBeVisible();

    const stored = await (await request.get(courseUrl, { headers: owner })).json();
    expect(stored.fields.subjects.arrayValue.values[1].mapValue.fields.hours.integerValue).toBe('90');
    const offer = stored.fields.offerings.arrayValue.values[0].mapValue.fields;
    expect(offer.capacity.integerValue).toBe('35');
    expect(offer.times.arrayValue.values[0].mapValue.fields.room.stringValue).toBe('Sala 203');
    const studentCatalog = await (await request.get('/api/curriculum', { headers: studentHeaders })).json() as Curriculum;
    expect(studentCatalog.offerings[0]).toMatchObject({ id: 'NOVA-A', capacity: 35 });
    expect((await request.get('/api/profile', { headers: studentHeaders })).ok()).toBeTruthy();
    expect((await request.put('/api/history/NOVA', { headers: studentHeaders })).ok()).toBeTruthy();

    await page.once('dialog', (dialog) => dialog.accept());
    await page.getByRole('button', { name: 'Remover turma NOVA-A' }).click();
    await expect(page.getByText('Nenhuma turma cadastrada.')).toBeVisible();
    await expect(studentPage.getByRole('button', { name: 'Usar turma A' })).toHaveCount(0);
    await page.getByRole('button', { name: 'Disciplinas', exact: true }).click();
    await page.once('dialog', (dialog) => dialog.accept());
    await page.getByRole('button', { name: 'Arquivar disciplina NOVA' }).click();
    await expect(page.getByText('Disciplina arquivada.', { exact: true })).toBeVisible();
    expect(await (await request.get('/api/history', { headers: studentHeaders })).json()).toMatchObject({ completed: ['BASE', 'NOVA'] });
    expect((await request.put('/api/history/NOVA', { headers: studentHeaders })).status()).toBe(404);

    const current = await (await request.get('/api/coordinator/curriculum', { headers })).json();
    const concurrent = await Promise.all([
      put('BASE', { ...subject, name: 'Edição A', prerequisites: [] }, current.revision),
      put('BASE', { ...subject, name: 'Edição B', prerequisites: [] }, current.revision),
    ]);
    expect(concurrent.map((result) => result.status()).sort()).toEqual([200, 409]);

    expect((await request.delete(grantUrl, { headers: owner })).ok()).toBeTruthy();
    expect((await request.get('/api/coordinator/curriculum', { headers })).status()).toBe(403);
    const original = await (await request.get(`${db}/courses/computacao-exemplo`, { headers: owner })).json();
    expect(original.fields.subjects.arrayValue.values).toHaveLength(24);
  } finally {
    await studentContext.close();
    await request.delete(grantUrl, { headers: owner });
    await request.delete(courseUrl, { headers: owner });
  }
});
