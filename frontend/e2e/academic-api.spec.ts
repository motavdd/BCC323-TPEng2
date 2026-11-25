import { expect, test, type APIRequestContext } from '@playwright/test';

async function account(request: APIRequestContext) {
  const base = process.env.E2E_AUTH_EMULATOR_URL || 'http://127.0.0.1:9099';
  const response = await request.post(`${base}/identitytoolkit.googleapis.com/v1/accounts:signUp?key=demo-key`, {
    data: { email: `api-${Date.now()}-${Math.random().toString(16).slice(2)}@example.test`, password: 'senha12345', returnSecureToken: true },
  });
  expect(response.ok()).toBeTruthy();
  const { idToken } = await response.json();
  return { Authorization: `Bearer ${idToken}` };
}

test('API usa histórico persistido, separa usuários e aplica as regras do domínio', async ({ request }) => {
  const first = await account(request), second = await account(request);
  const wrongMethod = await request.post('/api/profile', { headers: first });
  expect(wrongMethod.status()).toBe(405);
  expect(wrongMethod.headers().allow).toContain('PUT');
  expect((await request.get('/api/academic/summary')).status()).toBe(401);
  expect((await request.get('/api/profile', { headers: first })).status()).toBe(200);
  expect(await (await request.get('/api/profile', { headers: first })).json()).toEqual({ profile: null });
  expect((await request.get('/api/academic/summary', { headers: first })).status()).toBe(409);
  for (const headers of [first, second]) {
    const response = await request.put('/api/profile', { headers, data: { course: 'Ciência da Computação', period: 1 } });
    expect(response.status()).toBe(200);
    expect((await response.json()).profile.courseId).toBe('computacao-exemplo');
  }
  const spoof = await request.put('/api/profile', { headers: first, data: { course: 'Ciência da Computação', period: 1, uid: 'someone-else' } });
  expect(spoof.status()).toBe(400);
  const invalid = await request.put('/api/profile', { headers: first, data: { course: 'Ciência da Computação', period: 1.5 } });
  expect(invalid.status()).toBe(400);
  const catalog = await (await request.get('/api/curriculum', { headers: first })).json();
  expect(catalog.subjects).toHaveLength(24);
  const initial = await (await request.get('/api/academic/summary', { headers: first })).json();
  expect(initial.subjects.BCC202.state).toBe('blocked');
  const conclusion = await request.put('/api/history/BCC201', { headers: first });
  expect(conclusion.status()).toBe(200);
  const result = await conclusion.json();
  expect(result.completedHours).toBe(90);
  expect(result.subjects.BCC202.state).toBe('available');
  expect(result.subjects.BCC323.state).toBe('unknown');
  expect(result.progress).toBeCloseTo(90 / result.totalHours * 100);
  expect((await (await request.put('/api/history/BCC201', { headers: first })).json()).completedCount).toBe(1);
  expect((await (await request.get('/api/academic/summary', { headers: second })).json()).completedCount).toBe(0);
  expect((await request.put('/api/history/not-in-course', { headers: first })).status()).toBe(404);
  await request.put('/api/history/BCC202', { headers: first });
  const removed = await (await request.delete('/api/history/BCC201', { headers: first })).json();
  expect(removed.completed).toEqual(['BCC202']);
  expect(removed.completedHours).toBe(90);
  await request.delete('/api/history/BCC202', { headers: first });
  const final = await (await request.get('/api/academic/summary', { headers: first })).json();
  expect(final.completedHours).toBe(0);
  expect(final.subjects.BCC202.state).toBe('blocked');
  await Promise.all([
    request.put('/api/history/BCC201', { headers: first }).then(r => expect(r.ok()).toBeTruthy()),
    request.put('/api/history/BCC202', { headers: first }).then(r => expect(r.ok()).toBeTruthy()),
  ]);
  const concurrent = await (await request.get('/api/history', { headers: first })).json();
  expect(concurrent.completed.sort()).toEqual(['BCC201', 'BCC202']);
});

test('coordenador agenda mudanças por semestre sem alterar o nome no histórico', async ({ request }) => {
  const host = process.env.FIRESTORE_EMULATOR_HOST || '127.0.0.1:8080';
  if (!/^[a-zA-Z0-9.-]+:\d{1,5}$/.test(host)) throw new Error('Use o emulador do Firestore.');
  const firestore = `http://${host}/v1/projects/demo-bcc323/databases/(default)/documents`;
  const course = `course-schedule-${Date.now()}`;
  const coordinator = await account(request);
  const student = await account(request);
  const { uid } = await (await request.get('/api/me', { headers: coordinator })).json();
  const admin = { Authorization: 'Bearer owner' };
  const calendar = '/academicCalendar/terms';
  const assignment = `/courses/${course}/coordinators/${uid}`;
  const schedule = `/courses/${course}/schedules/catalog`;
  const coursePath = `/courses/${course}`;
  const today = new Intl.DateTimeFormat('sv-SE', { timeZone: 'America/Sao_Paulo' }).format(new Date());
  const year = Number(today.slice(0, 4));
  const current = { id: 'current', start: `${year}-01-01`, end: `${year}-12-31` };
  const next = { id: 'following', start: `${year + 1}-01-01`, end: `${year + 1}-12-31` };
  const oldCalendar = await request.get(firestore + calendar, { headers: admin });
  expect([200, 404]).toContain(oldCalendar.status());
  const previousFields = oldCalendar.ok() ? (await oldCalendar.json()).fields : undefined;
  try {
    const fixture = await request.get(firestore + '/courses/computacao-exemplo', { headers: admin });
    expect(fixture.ok()).toBeTruthy();
    const fields = (await fixture.json()).fields;
    fields.name = { stringValue: 'Computação — teste de vigência' };
    expect((await request.patch(firestore + coursePath, { headers: admin, data: { fields } })).ok()).toBeTruthy();
    const terms = [current, next].map(term => ({
      mapValue: { fields: Object.fromEntries(Object.entries(term).map(([key, value]) => [key, { stringValue: value }])) },
    }));
    expect((await request.patch(firestore + calendar, {
      headers: admin, data: { fields: { terms: { arrayValue: { values: terms } } } },
    })).ok()).toBeTruthy();
    expect((await request.patch(firestore + assignment, {
      headers: admin, data: { fields: { active: { booleanValue: true } } },
    })).ok()).toBeTruthy();
    expect((await request.put('/api/profile', {
      headers: student, data: { course: 'Computação — teste de vigência', courseId: course, period: 1 },
    })).ok()).toBeTruthy();
    expect((await request.put('/api/history/BCC201', { headers: student })).ok()).toBeTruthy();

    const url = `/api/courses/${course}/schedule`;
    const change = {
      type: 'offering', id: 'BCC202-A', term: current.id, professor: 'Docente substituto',
      times: [{ day: 'Qua', start: '09:30', end: '11:00', room: '202' }],
    };
    expect((await request.post(url, { headers: student, data: change })).status()).toBe(403);
    expect((await request.post('/api/courses/outro-curso/schedule', {
      headers: coordinator, data: change,
    })).status()).toBe(403);
    const applied = await request.post(url, { headers: coordinator, data: change });
    expect(applied.ok()).toBeTruthy();
    expect(await applied.json()).toEqual({ term: current.id, effectiveDate: today });

    const rename = { type: 'subject', id: 'BCC201', name: 'Fundamentos de Programação', hours: 90, period: 1, prerequisites: [] };
    const rejected = await request.post(url, { headers: coordinator, data: { ...rename, term: current.id } });
    expect(rejected.status()).toBe(400);
    const scheduled = await request.post(url, { headers: coordinator, data: { ...rename, term: next.id } });
    expect(scheduled.ok()).toBeTruthy();
    expect(await scheduled.json()).toEqual({ term: next.id, effectiveDate: next.start });

    const active = await (await request.get('/api/curriculum', { headers: student })).json();
    expect(active.subjects.find(({ id }: { id: string }) => id === 'BCC201').name).toBe('Introdução à Programação');
    expect(active.offerings.find(({ id }: { id: string }) => id === change.id)).toMatchObject({
      professor: change.professor, times: change.times,
    });
    const plan = await (await request.get('/api/academic/planning', { headers: student })).json();
    expect(plan.curriculum.subjects.find(({ id }: { id: string }) => id === 'BCC201').name).toBe(rename.name);
    expect(plan.curriculum.offerings.find(({ id }: { id: string }) => id === change.id).professor).toBe('');
    expect(plan.summary.subjects.BCC202.state).toBe('available');
    const history = await (await request.get('/api/history', { headers: student })).json();
    expect(history.completedNames).toEqual({ BCC201: 'Introdução à Programação' });
  } finally {
    for (const path of [schedule, assignment, coursePath]) await request.delete(firestore + path, { headers: admin });
    if (previousFields) {
      await request.patch(firestore + calendar, { headers: admin, data: { fields: previousFields } });
    } else {
      await request.delete(firestore + calendar, { headers: admin });
    }
  }
});
