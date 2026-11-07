import { describe, expect, it } from 'vitest';
import type { RegistrationData } from '../types/registration';
import { hasErrors, validateRegistration } from './registrationValidation';

const valid: RegistrationData = {
  name: 'Samara Paloma',
  email: 'aluno@ufop.edu.br',
  password: 'senha123',
  passwordConfirmation: 'senha123',
  course: 'Ciência da Computação',
  period: 8,
};

describe('Validação do autocadastro', () => {
  it('aceita um cadastro completo', () => {
    expect(hasErrors(validateRegistration(valid))).toBe(false);
  });

  it('exige todos os campos', () => {
    const errors = validateRegistration({
      name: ' ', email: '', password: '', passwordConfirmation: '', course: '', period: 0,
    });
    expect(Object.keys(errors).sort()).toEqual(
      ['course', 'email', 'name', 'password', 'passwordConfirmation', 'period'],
    );
  });

  it('rejeita e-mail mal formatado', () => {
    expect(validateRegistration({ ...valid, email: 'aluno@ufop' }).email).toBeDefined();
  });

  it('exige senha com pelo menos 6 caracteres', () => {
    const errors = validateRegistration({ ...valid, password: '123', passwordConfirmation: '123' });
    expect(errors.password).toBeDefined();
  });

  it('exige que a confirmação seja igual à senha', () => {
    const errors = validateRegistration({ ...valid, passwordConfirmation: 'outra123' });
    expect(errors.passwordConfirmation).toBe('As senhas não conferem.');
  });

  it('aceita apenas períodos inteiros de 1 a 20', () => {
    expect(validateRegistration({ ...valid, period: 21 }).period).toBeDefined();
    expect(validateRegistration({ ...valid, period: 2.5 }).period).toBeDefined();
    expect(validateRegistration({ ...valid, period: 1 }).period).toBeUndefined();
    expect(validateRegistration({ ...valid, period: 20 }).period).toBeUndefined();
  });

  it('limita o nome a 100 caracteres', () => {
    expect(validateRegistration({ ...valid, name: 'a'.repeat(101) }).name).toBeDefined();
  });
});
