import { useState } from 'react';
import type {
  RegistrationData,
  RegistrationErrors,
  RegistrationField,
} from '../types/registration';
import { hasErrors, validateRegistration } from '../utils/registrationValidation';
import { message } from '../utils/errors';

type SubmitState =
  | { status: 'idle' }
  | { status: 'submitting' }
  | { status: 'error'; message: string };

/**
 * Controla o envio do formulário de autocadastro: valida os campos,
 * marca o envio em andamento e transforma falhas em mensagens para o aluno.
 */
export function useRegistration(register: (data: RegistrationData) => Promise<void>) {
  const [state, setState] = useState<SubmitState>({ status: 'idle' });
  const [fieldErrors, setFieldErrors] = useState<RegistrationErrors>({});

  async function submit(data: RegistrationData) {
    const errors = validateRegistration(data);
    setFieldErrors(errors);
    if (hasErrors(errors)) {
      setState({ status: 'idle' });
      return;
    }
    setState({ status: 'submitting' });
    try {
      await register(data);
      setState({ status: 'idle' });
    } catch (error) {
      setState({ status: 'error', message: message(error) });
    }
  }

  /** Remove o erro de um campo assim que o aluno volta a editá-lo. */
  function clearFieldError(field: RegistrationField) {
    setFieldErrors((current) => {
      if (!(field in current)) return current;
      const next = { ...current };
      delete next[field];
      return next;
    });
  }

  return { state, fieldErrors, submit, clearFieldError };
}
