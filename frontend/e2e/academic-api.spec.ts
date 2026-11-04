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
