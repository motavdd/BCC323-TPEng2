import { beforeEach, describe, expect, it, vi } from 'vitest';
import { createUserWithEmailAndPassword } from 'firebase/auth';
import { academicApi } from './academic';
import { registerStudent } from './authService';
import type { RegistrationData } from '../types/registration';

vi.mock('firebase/auth', () => ({ createUserWithEmailAndPassword: vi.fn() }));
vi.mock('./firebase', () => ({ auth: { name: 'auth-de-teste' } }));
vi.mock('./academic', () => ({ academicApi: { saveProfile: vi.fn() } }));

const createUser = vi.mocked(createUserWithEmailAndPassword);
const saveProfile = vi.mocked(academicApi.saveProfile);

const dados: RegistrationData = {
  name: '  Luisa Aluna ',
  email: ' luisa@ufop.edu.br ',
  password: ' senha com espaços ',
  passwordConfirmation: ' senha com espaços ',
  course: ' Ciência da Computação ',
  period: 3,
};

describe('registerStudent', () => {
  beforeEach(() => {
    createUser.mockReset();
    saveProfile.mockReset();
  });

  it('cria a conta e depois salva o perfil acadêmico sem espaços sobrando', async () => {
    createUser.mockResolvedValue({} as never);
    saveProfile.mockResolvedValue({} as never);
    await registerStudent(dados);

    // A senha é enviada exatamente como digitada; só e-mail, nome e curso são aparados.
    expect(createUser).toHaveBeenCalledWith({ name: 'auth-de-teste' }, 'luisa@ufop.edu.br', ' senha com espaços ');
    expect(saveProfile).toHaveBeenCalledWith({
      displayName: 'Luisa Aluna',
      course: 'Ciência da Computação',
      period: 3,
    });
    expect(createUser.mock.invocationCallOrder[0]).toBeLessThan(saveProfile.mock.invocationCallOrder[0]);
  });

  it('não tenta salvar o perfil se a conta não foi criada', async () => {
    createUser.mockRejectedValue({ code: 'auth/email-already-in-use' });
    await expect(registerStudent(dados)).rejects.toEqual({ code: 'auth/email-already-in-use' });
    expect(saveProfile).not.toHaveBeenCalled();
  });

  it('repassa a falha da API quando o perfil não pôde ser salvo', async () => {
    createUser.mockResolvedValue({} as never);
    saveProfile.mockRejectedValue(new Error('API indisponível'));
    await expect(registerStudent(dados)).rejects.toThrow('API indisponível');
  });
});
