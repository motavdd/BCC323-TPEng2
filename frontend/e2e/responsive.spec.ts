import { expect, test, type Page } from '@playwright/test';
import { authBase, createAccount, signIn, verifyRegisteredEmail } from './auth-emulator';

const widths = [320, 390, 768, 1440, 1920];

async function checkLayout(page: Page, screen: string) {
  for (const width of widths) {
    await page.setViewportSize({ width, height: width <= 390 ? 680 : 900 });
    const sizes = await page.evaluate(() => {
      const pageWidth = document.documentElement.clientWidth;
      const overflow = [...document.querySelectorAll<HTMLElement>(
        'main input, main select, main button, .sidebar nav button',
      )].flatMap((element) => {
        if (element.closest('.matrix-scroll, .week-scroll')) return [];
        const rect = element.getBoundingClientRect();
        const panel = element.closest('.panel');
        const parent = panel?.getBoundingClientRect();
        const padding = panel ? parseFloat(getComputedStyle(panel).paddingRight) : 0;
        const rightEdge = parent ? parent.right - padding : pageWidth;
        return rect.right > rightEdge + 1 || rect.left < -1
          ? [element.getAttribute('name') || element.textContent?.trim() || element.tagName]
          : [];
      });
      const sidebarLeft = document.querySelector('.app-shell > .sidebar')?.getBoundingClientRect().left;
      return { documentWidth: document.documentElement.scrollWidth, overflow, sidebarLeft };
    });
    expect.soft(sizes.documentWidth, `${screen}, documento em ${width}px`).toBeLessThanOrEqual(width);
    expect.soft(sizes.overflow, `${screen}, controles em ${width}px`).toEqual([]);
    if (sizes.sidebarLeft !== undefined) {
      expect.soft(sizes.sidebarLeft, `${screen}, posição do menu em ${width}px`).toBe(0);
    }
  }
}

test('telas públicas, aluno e coordenação se ajustam à largura disponível', async ({ page, request }) => {
  const suffix = Date.now();
  const email = `layout-${suffix}@aluno.ufop.edu.br`;
  const password = 'layout-teste-123';
  const courseId = `layout-${suffix}`;
  const courseName = `Curso Layout ${suffix}`;
  const owner = { Authorization: 'Bearer owner' };
  const db = `http://${process.env.FIRESTORE_EMULATOR_HOST || '127.0.0.1:8080'}/v1/projects/demo-bcc323/databases/(default)/documents`;
  const user = await createAccount(request, email, password, false);
  const course = `${db}/courses/${courseId}`;
  const grant = `${db}/coordinators/${user.localId}`;
  const subject = {
    id: { stringValue: 'LAYOUT' }, name: { stringValue: 'Disciplina de teste' },
    hours: { integerValue: '60' }, period: { integerValue: '1' },
    requirementsKnown: { booleanValue: true },
  };

  async function logIn() {
    await page.getByLabel('E-mail', { exact: true }).fill(email);
    await page.getByLabel('Senha', { exact: true }).fill(password);
    await page.getByRole('button', { name: 'Entrar', exact: true }).click();
  }

  try {
    expect((await request.patch(course, { headers: owner, data: { fields: {
      name: { stringValue: courseName }, subjects: { arrayValue: { values: [
        { mapValue: { fields: subject } },
        { mapValue: { fields: { ...subject, id: { stringValue: 'PREREQ' }, name: { stringValue: 'P'.repeat(140) } } } },
      ] } },
    } } })).ok()).toBeTruthy();
    await page.goto('/');
    await expect(page.getByRole('heading', { name: 'Seja Bem Vindo' })).toBeVisible();
    await checkLayout(page, 'login');
    await page.getByRole('button', { name: 'Primeiro acesso? Criar conta' }).click();
    await checkLayout(page, 'cadastro');
    await page.getByRole('button', { name: 'Já tenho conta' }).click();
    await logIn();
    await expect(page.getByRole('heading', { name: 'Confirme seu e-mail institucional' })).toBeVisible();
    await checkLayout(page, 'confirmação');
    await page.getByRole('button', { name: 'Enviar link de confirmação' }).click();
    await expect(page.getByRole('status')).toHaveText('Link de confirmação enviado.');
    await verifyRegisteredEmail(page, email);
    await expect(page.getByRole('heading', { name: 'Falta só um passo' })).toBeVisible();
    await checkLayout(page, 'perfil incompleto');
    await page.getByLabel('Curso', { exact: true }).fill(courseName);
    await page.getByRole('button', { name: 'Salvar perfil e continuar' }).click();
    await expect(page.getByRole('heading', { name: 'Seu progresso começa na matriz' })).toBeVisible();
    await checkLayout(page, 'visão geral');
    await page.getByRole('link', { name: 'Matriz curricular' }).click();
    await expect(page.getByRole('button', { name: /LAYOUT Disciplina de teste/ })).toBeVisible();
    await checkLayout(page, 'matriz');
    await page.getByRole('button', { name: /LAYOUT Disciplina de teste/ }).click();
    await checkLayout(page, 'detalhe');
    await page.getByRole('link', { name: 'Histórico' }).click();
    await checkLayout(page, 'histórico');
    await page.getByRole('link', { name: 'Planejar semestre' }).click();
    await checkLayout(page, 'planejamento');
    await page.goto('/#saved');
    await expect(page.getByRole('heading', { name: '✓ Plano salvo com sucesso' })).toBeVisible();
    await checkLayout(page, 'plano salvo');
    await page.getByRole('link', { name: 'Perfil' }).click();
    await checkLayout(page, 'perfil');

    expect((await request.patch(grant, { headers: owner, data: { fields: {
      courseId: { stringValue: courseId },
    } } })).ok()).toBeTruthy();
    await page.getByRole('button', { name: 'Sair da conta' }).click();
    await logIn();
    await expect(page.getByRole('heading', { name: 'Matriz curricular' })).toBeVisible();
    await checkLayout(page, 'disciplinas');
    await page.getByRole('button', { name: 'Editar disciplina LAYOUT' }).click();
    await checkLayout(page, 'editor de disciplina');

    await page.setViewportSize({ width: 320, height: 680 });
    const search = page.getByRole('searchbox', { name: 'Buscar pré-requisito' });
    const prerequisite = page.getByRole('checkbox', { name: /^PREREQ ·/ });
    await search.fill('PREREQ');
    await prerequisite.check();
    await search.fill('sem resultado');
    await expect(prerequisite).toHaveCount(0);
    await expect(page.getByText('1 selecionado(s)')).toBeVisible();
    await page.getByRole('button', { name: 'Salvar disciplina' }).click();
    await expect(page.getByText('Pré-requisitos: PREREQ', { exact: true })).toBeVisible();

    await page.reload();
    await page.getByRole('button', { name: 'Editar disciplina LAYOUT' }).click();
    await expect(prerequisite).toBeChecked();
    await page.getByRole('button', { name: 'Cancelar', exact: true }).click();
    await page.getByRole('button', { name: 'Editar disciplina PREREQ' }).click();
    await page.getByRole('checkbox', { name: 'LAYOUT · Disciplina de teste' }).check();
    await page.getByRole('button', { name: 'Salvar disciplina' }).click();
    await expect(page.getByRole('alert')).toContainText('a matriz não pode ter ciclos', { ignoreCase: true });
    await expect(page.getByRole('checkbox', { name: 'LAYOUT · Disciplina de teste' })).toBeChecked();
    await page.getByRole('button', { name: 'Cancelar', exact: true }).click();
    await page.getByRole('button', { name: 'Editar disciplina LAYOUT' }).click();
    await prerequisite.uncheck();
    await page.getByRole('button', { name: 'Salvar disciplina' }).click();
    await expect(page.getByText('Pré-requisitos: PREREQ', { exact: true })).toHaveCount(0);
    await page.reload();
    await page.getByRole('button', { name: 'Editar disciplina LAYOUT' }).click();
    await expect(prerequisite).not.toBeChecked();

    await page.getByRole('button', { name: 'Turmas e horários' }).click();
    await page.getByRole('button', { name: 'Nova turma' }).click();
    await checkLayout(page, 'editor de turma');
  } finally {
    await request.delete(grant, { headers: owner });
    await request.delete(course, { headers: owner });
    await request.delete(`${db}/users/${user.localId}`, { headers: owner });
    const account = await signIn(request, email, password);
    await request.post(`${authBase}/identitytoolkit.googleapis.com/v1/accounts:delete?key=demo-key`,
      { data: { idToken: account.idToken } });
  }
});
