import { expect, type APIRequestContext, type Page } from '@playwright/test';

export const authBase = process.env.E2E_AUTH_EMULATOR_URL || 'http://127.0.0.1:9099';
const accounts = `${authBase}/identitytoolkit.googleapis.com/v1/accounts`;
const key = 'key=demo-key';

export async function verifyEmail(request: APIRequestContext, email: string) {
  const response = await request.get(`${authBase}/emulator/v1/projects/demo-bcc323/oobCodes`);
  expect(response.ok()).toBeTruthy();
  const { oobCodes } = await response.json() as { oobCodes: { email: string; oobCode: string; requestType: string }[] };
  const code = oobCodes.reverse().find((item) => item.email === email && item.requestType === 'VERIFY_EMAIL');
  expect(code).toBeDefined();
  const verified = await request.post(`${accounts}:update?${key}`, { data: { oobCode: code!.oobCode } });
  expect(verified.ok()).toBeTruthy();
}

export async function verifyRegisteredEmail(page: Page, email: string) {
  await expect(page.getByRole('heading', { name: 'Confirme seu e-mail institucional' })).toBeVisible();
  await verifyEmail(page.request, email);
  await page.getByRole('button', { name: 'Já confirmei' }).click();
}

export async function signIn(request: APIRequestContext, email: string, password: string) {
  const response = await request.post(`${accounts}:signInWithPassword?${key}`, {
    data: { email, password, returnSecureToken: true },
  });
  expect(response.ok()).toBeTruthy();
  return await response.json() as { localId: string; idToken: string };
}

export async function createAccount(request: APIRequestContext, email: string, password: string, verified = true) {
  const response = await request.post(`${accounts}:signUp?${key}`, {
    data: { email, password, returnSecureToken: true },
  });
  expect(response.ok()).toBeTruthy();
  const account = await response.json() as { localId: string; idToken: string };
  if (!verified) return account;
  const sent = await request.post(`${accounts}:sendOobCode?${key}`, {
    data: { requestType: 'VERIFY_EMAIL', idToken: account.idToken },
  });
  expect(sent.ok()).toBeTruthy();
  await verifyEmail(request, email);
  return signIn(request, email, password);
}
