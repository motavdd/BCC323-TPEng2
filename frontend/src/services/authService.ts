import { createUserWithEmailAndPassword } from 'firebase/auth';
import { auth } from './firebase';
import { academicApi } from './academic';
import type { RegistrationData } from '../types/registration';

/**
 * Autocadastro do aluno (issue #16):
 * 1. cria a conta no Firebase Authentication (e-mail e senha);
 * 2. inicializa o perfil acadêmico na API C++ (nome, curso e período).
 *
 * Se o passo 2 falhar, a conta já existe: o aluno entra e completa o perfil
 * na tela "Falta só um passo", sem criar outra conta.
 */
export async function registerStudent(data: RegistrationData): Promise<void> {
  await createUserWithEmailAndPassword(auth, data.email.trim(), data.password);
  await academicApi.saveProfile({
    displayName: data.name.trim(),
    course: data.course.trim(),
    period: data.period,
  });
}
