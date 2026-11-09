import { fireEvent, render, screen, waitFor } from '@testing-library/react';
import { beforeEach, describe, expect, it, vi } from 'vitest';
import { signInWithEmailAndPassword } from 'firebase/auth';
import { Access } from './Access';

vi.mock('firebase/auth', () => ({ signInWithEmailAndPassword: vi.fn() }));
vi.mock('../../services/firebase', () => ({ auth: { name: 'auth-de-teste' } }));

const signIn = vi.mocked(signInWithEmailAndPassword);

/** Preenche o formulário de login e clica em "Entrar". */
function entrar(email = 'aluno@ufop.edu.br', senha = 'senha123') {
  fireEvent.change(screen.getByLabelText('E-mail'), { target: { value: email } });
  fireEvent.change(screen.getByLabelText('Senha'), { target: { value: senha } });
  fireEvent.click(screen.getByRole('button', { name: 'Entrar' }));
}

describe('Access (tela de login)', () => {
  beforeEach(() => signIn.mockReset());

  it('mostra os campos de e-mail e senha ligados aos seus rótulos', () => {
    render(<Access onCreateAccount={vi.fn()} />);
    expect(screen.getByRole('heading', { name: 'Seja Bem Vindo' })).toBeInTheDocument();
    expect(screen.getByLabelText('E-mail')).toHaveAttribute('type', 'email');
    expect(screen.getByLabelText('Senha')).toHaveAttribute('type', 'password');
  });

  it('autentica no Firebase com o e-mail e a senha digitados', async () => {
    signIn.mockResolvedValue({} as never);
    render(<Access onCreateAccount={vi.fn()} />);
    entrar();
    await waitFor(() =>
      expect(signIn).toHaveBeenCalledWith({ name: 'auth-de-teste' }, 'aluno@ufop.edu.br', 'senha123'),
    );
    expect(screen.queryByRole('alert')).not.toBeInTheDocument();
  });

  it('desabilita o botão enquanto o login está em andamento', async () => {
    let concluir: () => void = () => {};
    signIn.mockReturnValue(new Promise((resolve) => { concluir = () => resolve({} as never); }));
    render(<Access onCreateAccount={vi.fn()} />);
    entrar();
    const botao = await screen.findByRole('button', { name: 'Aguarde…' });
    expect(botao).toBeDisabled();
    concluir();
    expect(await screen.findByRole('button', { name: 'Entrar' })).toBeEnabled();
  });

  it('mostra uma mensagem amigável quando a senha está errada', async () => {
    signIn.mockRejectedValueOnce({ code: 'auth/invalid-credential' });
    render(<Access onCreateAccount={vi.fn()} />);
    entrar('aluno@ufop.edu.br', 'senhaerrada');
    expect(await screen.findByRole('alert')).toHaveTextContent('E-mail ou senha incorretos.');
    expect(signIn).toHaveBeenCalledOnce();
    expect(screen.getByRole('button', { name: 'Entrar' })).toBeEnabled();
  });

  it('limpa o erro anterior ao tentar de novo', async () => {
    signIn.mockRejectedValueOnce({ code: 'auth/invalid-credential' }).mockResolvedValueOnce({} as never);
    render(<Access onCreateAccount={vi.fn()} />);
    entrar();
    expect(await screen.findByRole('alert')).toBeInTheDocument();
    entrar();
    await waitFor(() => expect(screen.queryByRole('alert')).not.toBeInTheDocument());
  });

  it('alterna entre mostrar e ocultar a senha', () => {
    render(<Access onCreateAccount={vi.fn()} />);
    fireEvent.click(screen.getByRole('button', { name: 'Mostrar senha' }));
    expect(screen.getByLabelText('Senha')).toHaveAttribute('type', 'text');
    fireEvent.click(screen.getByRole('button', { name: 'Ocultar senha' }));
    expect(screen.getByLabelText('Senha')).toHaveAttribute('type', 'password');
  });

  it('leva para o autocadastro em "Primeiro acesso? Criar conta"', () => {
    const onCreateAccount = vi.fn();
    render(<Access onCreateAccount={onCreateAccount} />);
    fireEvent.click(screen.getByRole('button', { name: 'Primeiro acesso? Criar conta' }));
    expect(onCreateAccount).toHaveBeenCalledOnce();
    expect(signIn).not.toHaveBeenCalled();
  });
});
