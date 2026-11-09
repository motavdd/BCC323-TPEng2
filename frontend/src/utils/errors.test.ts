import { describe, expect, it } from 'vitest';
import { message } from './errors';

describe('message', () => {
  it.each([
    ['auth/invalid-credential', 'E-mail ou senha incorretos.'],
    ['auth/wrong-password', 'E-mail ou senha incorretos.'],
    ['auth/user-not-found', 'E-mail ou senha incorretos.'],
    ['auth/invalid-email', 'Informe um e-mail válido (ex.: aluno@ufop.edu.br).'],
    ['auth/email-already-in-use', 'Este e-mail já possui uma conta. Entre para continuar.'],
    ['auth/weak-password', 'Use uma senha com pelo menos 6 caracteres.'],
    ['auth/too-many-requests', 'Muitas tentativas. Aguarde um pouco e tente novamente.'],
    ['auth/network-request-failed', 'Confira sua conexão e tente novamente.'],
  ])('traduz o código %s do Firebase', (code, esperado) => {
    expect(message({ code })).toBe(esperado);
  });

  it('usa o texto do erro quando o código não é conhecido', () => {
    expect(message(new Error('Falha na API'))).toBe('Falha na API');
  });

  it('usa uma mensagem genérica para erros sem texto', () => {
    expect(message(undefined)).toBe('Não foi possível concluir. Tente novamente.');
    expect(message({ code: 'auth/desconhecido' })).toBe('Não foi possível concluir. Tente novamente.');
  });
});
