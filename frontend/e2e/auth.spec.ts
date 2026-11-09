import { expect, test } from '@playwright/test';

test('aluno consegue se cadastrar e sair no ambiente emulado', async ({ page }) => {
  await page.goto('/');
  await page.getByRole('button', { name: 'Primeiro acesso? Criar conta' }).click();
  await page.getByLabel('Nome completo').fill('Aluno Teste');
  await page.getByLabel('E-mail').fill(`aluno-${Date.now()}@example.test`);
  await page.getByLabel('Senha', { exact: true }).fill('senha12345');
  await page.getByLabel('Confirmar senha').fill('senha12345');
  await page.getByLabel('Curso').fill('Ciência da Computação');
  await page.getByLabel('Período').fill('4');
  await page.getByRole('button', { name: 'Criar conta', exact: true }).click();
  await expect(page.getByRole('heading', { name: 'Seu progresso começa na matriz' })).toBeVisible();
  await page.getByRole('button', { name: 'Sair' }).click();
  await expect(page.getByRole('button', { name: 'Entrar', exact: true })).toBeVisible();
});

test('autocadastro mostra os erros dos campos e não cria conta inválida', async ({ page }) => {
  await page.goto('/');
  await page.getByRole('button', { name: 'Primeiro acesso? Criar conta' }).click();
  await page.getByLabel('E-mail').fill('email-invalido');
  await page.getByLabel('Senha', { exact: true }).fill('senha12345');
  await page.getByLabel('Confirmar senha').fill('outra-senha');
  await page.getByLabel('Período atual').fill('25');
  await page.getByRole('button', { name: 'Criar conta', exact: true }).click();
  await expect(page.getByText('Informe seu nome.')).toBeVisible();
  await expect(page.getByText('Informe um e-mail válido (ex.: aluno@ufop.edu.br).')).toBeVisible();
  await expect(page.getByText('As senhas não conferem.')).toBeVisible();
  await expect(page.getByText('Informe um período de 1 a 20.')).toBeVisible();
  await page.getByRole('button', { name: 'Já tenho conta' }).click();
  await expect(page.getByRole('button', { name: 'Entrar', exact: true })).toBeVisible();
});

test('conta sem perfil consegue completar os dados sem recriar a conta', async ({ page, request }) => {
  const email = `incomplete-${Date.now()}@example.test`;
  const emulator = process.env.E2E_AUTH_EMULATOR_URL || 'http://127.0.0.1:9099';
  const signup = await request.post(`${emulator}/identitytoolkit.googleapis.com/v1/accounts:signUp?key=demo-key`, {
    data: { email, password: 'senha12345', returnSecureToken: true },
  });
  expect(signup.ok()).toBeTruthy();
  await page.goto('/');
  await page.getByLabel('E-mail', { exact: true }).fill(email);
  await page.getByLabel('Senha', { exact: true }).fill('senha12345');
  await page.getByRole('button', { name: 'Entrar', exact: true }).click();
  await expect(page.getByRole('heading', { name: 'Falta só um passo' })).toBeVisible();
  await page.getByLabel('Período atual').fill('3');
  await page.getByRole('button', { name: 'Salvar perfil e continuar' }).click();
  await expect(page.getByRole('heading', { name: 'Seu progresso começa na matriz' })).toBeVisible();
  await page.reload();
  await expect(page.getByRole('heading', { name: 'Seu progresso começa na matriz' })).toBeVisible();
});
