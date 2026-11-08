import type {
  RegistrationData,
  RegistrationErrors,
} from '../types/registration';

// Limites alinhados com o backend (AcademicService::saveProfile) e o Firebase.
export const NAME_MAX_LENGTH = 100;
export const COURSE_MAX_LENGTH = 100;
export const PASSWORD_MIN_LENGTH = 6;
export const PERIOD_MIN = 1;
export const PERIOD_MAX = 20;

const EMAIL_PATTERN = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;

/**
 * Valida os campos do autocadastro antes de enviar.
 * Ajuda a experiência do aluno, mas não substitui as regras da API e do Firebase.
 */
export function validateRegistration(data: RegistrationData): RegistrationErrors {
  const errors: RegistrationErrors = {};

  const name = data.name.trim();
  if (!name) errors.name = 'Informe seu nome.';
  else if (name.length < 2) errors.name = 'O nome deve ter pelo menos 2 caracteres.';
  else if (name.length > NAME_MAX_LENGTH) errors.name = `Use no máximo ${NAME_MAX_LENGTH} caracteres.`;

  const email = data.email.trim();
  if (!email) errors.email = 'Informe seu e-mail.';
  else if (!EMAIL_PATTERN.test(email)) errors.email = 'Informe um e-mail válido (ex.: aluno@ufop.edu.br).';

  if (!data.password) errors.password = 'Crie uma senha.';
  else if (data.password.length < PASSWORD_MIN_LENGTH) {
    errors.password = `A senha deve ter pelo menos ${PASSWORD_MIN_LENGTH} caracteres.`;
  }

  if (!data.passwordConfirmation) errors.passwordConfirmation = 'Repita a senha.';
  else if (data.passwordConfirmation !== data.password) errors.passwordConfirmation = 'As senhas não conferem.';

  const course = data.course.trim();
  if (!course) errors.course = 'Informe seu curso.';
  else if (course.length > COURSE_MAX_LENGTH) errors.course = `Use no máximo ${COURSE_MAX_LENGTH} caracteres.`;

  if (!Number.isInteger(data.period) || data.period < PERIOD_MIN || data.period > PERIOD_MAX) {
    errors.period = `Informe um período de ${PERIOD_MIN} a ${PERIOD_MAX}.`;
  }

  return errors;
}

/** Indica se a validação encontrou algum campo inválido. */
export function hasErrors(errors: RegistrationErrors): boolean {
  return Object.keys(errors).length > 0;
}
