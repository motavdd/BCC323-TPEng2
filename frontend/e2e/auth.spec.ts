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

/** Cria uma conta pelo autocadastro e devolve o e-mail usado. */
async function cadastrarAluno(page: import('@playwright/test').Page, senha: string) {
  const email = `fluxo-${Date.now()}@example.test`;
  await page.goto('/');
  await page.getByRole('button', { name: 'Primeiro acesso? Criar conta' }).click();
  await page.getByLabel('Nome completo').fill('Aluna Fluxo Completo');
  await page.getByLabel('E-mail').fill(email);
  await page.getByLabel('Senha', { exact: true }).fill(senha);
  await page.getByLabel('Confirmar senha').fill(senha);
  await page.getByLabel('Curso').fill('Ciência da Computação');
  await page.getByLabel('Período atual').fill('2');
  await page.getByRole('button', { name: 'Criar conta', exact: true }).click();
  await expect(page.getByRole('heading', { name: 'Seu progresso começa na matriz' })).toBeVisible();
  return email;
}

test('fluxo completo (#21): cadastro, saída, login de novo e sessão mantida ao recarregar', async ({ page }) => {
  const senha = 'senha-fluxo-123';
  const email = await cadastrarAluno(page, senha);

  await page.getByRole('button', { name: 'Sair' }).click();
  await expect(page.getByRole('heading', { name: 'Seja Bem Vindo' })).toBeVisible();

  await page.getByLabel('E-mail', { exact: true }).fill(email);
  await page.getByLabel('Senha', { exact: true }).fill(senha);
  await page.getByRole('button', { name: 'Entrar', exact: true }).click();
  // O perfil salvo no cadastro é reaproveitado: não pede curso e período de novo.
  await expect(page.getByRole('heading', { name: 'Seu progresso começa na matriz' })).toBeVisible();
  await expect(page.getByRole('heading', { name: 'Falta só um passo' })).toHaveCount(0);

  await page.reload();
  await expect(page.getByRole('heading', { name: 'Seu progresso começa na matriz' })).toBeVisible();
});

test('login com senha errada mostra o erro e não entra no sistema', async ({ page }) => {
  const email = await cadastrarAluno(page, 'senha-certa-123');
  await page.getByRole('button', { name: 'Sair' }).click();

  await page.getByLabel('E-mail', { exact: true }).fill(email);
  await page.getByLabel('Senha', { exact: true }).fill('senha-errada-123');
  await page.getByRole('button', { name: 'Entrar', exact: true }).click();
  await expect(page.getByRole('alert')).toHaveText('E-mail ou senha incorretos.');
  await expect(page.getByRole('button', { name: 'Entrar', exact: true })).toBeEnabled();
  await expect(page.getByRole('heading', { name: 'Seu progresso começa na matriz' })).toHaveCount(0);
});

test('autocadastro com e-mail já usado avisa o aluno em vez de criar outra conta', async ({ page }) => {
  const email = await cadastrarAluno(page, 'senha-dupla-123');
  await page.getByRole('button', { name: 'Sair' }).click();

  await page.getByRole('button', { name: 'Primeiro acesso? Criar conta' }).click();
  await page.getByLabel('Nome completo').fill('Outra Pessoa');
  await page.getByLabel('E-mail').fill(email);
  await page.getByLabel('Senha', { exact: true }).fill('outra-senha-123');
  await page.getByLabel('Confirmar senha').fill('outra-senha-123');
  await page.getByLabel('Período atual').fill('1');
  await page.getByRole('button', { name: 'Criar conta', exact: true }).click();
  await expect(page.getByRole('alert')).toHaveText('Este e-mail já possui uma conta. Entre para continuar.');
  await expect(page.getByRole('heading', { name: 'Crie sua conta' })).toBeVisible();
});

test('login com e-mail sem conta mostra a mesma mensagem genérica', async ({ page }) => {
  await page.goto('/');
  await page.getByLabel('E-mail', { exact: true }).fill(`ninguem-${Date.now()}@example.test`);
  await page.getByLabel('Senha', { exact: true }).fill('qualquer-senha');
  await page.getByRole('button', { name: 'Entrar', exact: true }).click();
  await expect(page.getByRole('alert')).toHaveText('E-mail ou senha incorretos.');
});
