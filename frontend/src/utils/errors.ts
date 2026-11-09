export function message(error: unknown): string {
  const code = error && typeof error === 'object' && 'code' in error ? error.code : '';
  const messages: Record<string, string> = {
    'auth/invalid-credential': 'E-mail ou senha incorretos.',
    // Sem a proteção contra enumeração de e-mails (ex.: emulador), o Firebase
    // usa códigos separados para senha errada e conta inexistente.
    'auth/wrong-password': 'E-mail ou senha incorretos.',
    'auth/user-not-found': 'E-mail ou senha incorretos.',
    'auth/invalid-email': 'Informe um e-mail válido (ex.: aluno@ufop.edu.br).',
    'auth/email-already-in-use': 'Este e-mail já possui uma conta. Entre para continuar.',
    'auth/weak-password': 'Use uma senha com pelo menos 6 caracteres.',
    'auth/too-many-requests': 'Muitas tentativas. Aguarde um pouco e tente novamente.',
    'auth/network-request-failed': 'Confira sua conexão e tente novamente.',
    'permission-denied': 'Você não tem permissão para salvar estes dados.',
    'unavailable': 'Serviço indisponível. Tente novamente; suas escolhas foram mantidas.',
  };
  return messages[String(code)] || (error instanceof Error ? error.message : 'Não foi possível concluir. Tente novamente.');
}
