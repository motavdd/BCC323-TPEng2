/**
 * Dados informados pelo aluno no formulário de autocadastro (issue #16).
 * `name`, `course` e `period` inicializam o perfil acadêmico na API;
 * `email` e `password` criam a conta no Firebase Authentication.
 */
export type RegistrationData = {
  name: string;
  email: string;
  password: string;
  passwordConfirmation: string;
  course: string;
  period: number;
};

/** Nome de cada campo do formulário. */
export type RegistrationField = keyof RegistrationData;

/** Mensagem de erro de cada campo inválido; campos válidos ficam ausentes. */
export type RegistrationErrors = Partial<Record<RegistrationField, string>>;
