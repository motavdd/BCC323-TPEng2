import { expect, test } from '@playwright/test';

test('frontend encaminha /api para o serviço C++', async ({ request }) => {
  const response = await request.get('/api/health');
  expect(response.status()).toBe(200);
  expect(await response.json()).toEqual({ status: 'ok', service: 'bcc323-api' });
});
