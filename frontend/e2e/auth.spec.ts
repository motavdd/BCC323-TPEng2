import { expect, test } from '@playwright/test';

test('aluno consegue se cadastrar e sair no ambiente emulado', async ({ page }) => {
  await page.goto('/');
  await page.getByLabel('E-mail').fill(`aluno-${Date.now()}@example.test`);
  await page.getByLabel('Senha').fill('senha12345');
  await page.getByLabel('Curso').fill('Ciência da Computação');
  await page.getByLabel('Período').fill('4');
  await page.getByRole('button', { name: 'Cadastrar' }).click();
  await expect(page.getByRole('status')).toContainText('Sessão iniciada');
  await page.getByRole('button', { name: 'Sair' }).click();
  await expect(page.getByRole('button', { name: 'Cadastrar' })).toBeVisible();
});
