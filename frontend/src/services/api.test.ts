import { afterEach, beforeEach, describe, expect, it, vi } from 'vitest';
import { academicApi } from './academic';
import { api } from './api';

const { auth, getIdToken } = vi.hoisted(() => {
  const getIdToken = vi.fn(async () => 'token-de-teste');
  const auth: { currentUser: { getIdToken: typeof getIdToken } | null } = {
    currentUser: null,
  };
  return { auth, getIdToken };
});

vi.mock('./firebase', () => ({ auth }));

const fetchApi = vi.fn<typeof fetch>();

describe('cliente da API acadêmica', () => {
  beforeEach(() => {
    auth.currentUser = { getIdToken };
    fetchApi.mockReset();
    vi.stubGlobal('fetch', fetchApi);
  });

  afterEach(() => vi.unstubAllGlobals());

  it('envia o token Firebase e salva o perfil em /api/profile', async () => {
    const input = { displayName: 'Aluna', course: 'Computação', period: 3 };
    const profile = { ...input, courseId: 'computacao' };
    fetchApi.mockResolvedValue(Response.json({ profile }));

    await expect(academicApi.saveProfile(input)).resolves.toEqual({ profile });
    expect(fetchApi).toHaveBeenCalledWith('/api/profile', expect.objectContaining({
      method: 'PUT',
      headers: { Authorization: 'Bearer token-de-teste', 'Content-Type': 'application/json' },
      body: JSON.stringify(input),
    }));
  });

  it('interrompe a requisição quando não há usuário autenticado', async () => {
    auth.currentUser = null;

    await expect(api('/profile')).rejects.toThrow('Entre novamente para continuar.');
    expect(fetchApi).not.toHaveBeenCalled();
  });

  it('detecta quando o Hosting devolve a página HTML em /api', async () => {
    fetchApi.mockResolvedValue(new Response('<!doctype html>', {
      headers: { 'Content-Type': 'text/html' },
    }));

    await expect(api('/profile')).rejects.toThrow('A API não está disponível neste endereço.');
  });

  it('repassa a mensagem de erro em JSON da API', async () => {
    fetchApi.mockResolvedValue(Response.json(
      { error: { message: 'Curso ainda não cadastrado.' } },
      { status: 404 },
    ));

    await expect(api('/profile')).rejects.toThrow('Curso ainda não cadastrado.');
  });
});
