import { fireEvent, render, screen, waitFor } from '@testing-library/react';
import { describe, expect, it, vi } from 'vitest';
import { RegisterPage } from './RegisterPage';

/** Preenche o autocadastro com dados válidos (o curso já vem preenchido). */
function preencher() {
  const campo = (rotulo: string, valor: string) =>
    fireEvent.change(screen.getByLabelText(rotulo), { target: { value: valor } });
  campo('Nome completo', '  Luisa Aluna  ');
  campo('E-mail', 'luisa@ufop.edu.br');
  campo('Senha', 'senha123');
  campo('Confirmar senha', 'senha123');
  campo('Período atual', '4');
}

const criarConta = () => fireEvent.click(screen.getByRole('button', { name: 'Criar conta' }));

describe('RegisterPage (autocadastro)', () => {
  it('mostra os erros de cada campo e não envia um cadastro inválido', () => {
    const onRegister = vi.fn();
    render(<RegisterPage onRegister={onRegister} onBackToLogin={vi.fn()} />);
    fireEvent.change(screen.getByLabelText('E-mail'), { target: { value: 'email-invalido' } });
    fireEvent.change(screen.getByLabelText('Senha'), { target: { value: '123' } });
    fireEvent.change(screen.getByLabelText('Confirmar senha'), { target: { value: '321' } });
    fireEvent.change(screen.getByLabelText('Curso'), { target: { value: '' } });
    fireEvent.change(screen.getByLabelText('Período atual'), { target: { value: '25' } });
    criarConta();

    expect(screen.getByText('Informe seu nome.')).toBeInTheDocument();
    expect(screen.getByText('Informe um e-mail válido (ex.: aluno@ufop.edu.br).')).toBeInTheDocument();
    expect(screen.getByText('A senha deve ter pelo menos 6 caracteres.')).toBeInTheDocument();
    expect(screen.getByText('As senhas não conferem.')).toBeInTheDocument();
    expect(screen.getByText('Informe seu curso.')).toBeInTheDocument();
    expect(screen.getByText('Informe um período de 1 a 20.')).toBeInTheDocument();
    expect(screen.getByLabelText('Nome completo')).toHaveAttribute('aria-invalid', 'true');
    expect(screen.getByLabelText('Nome completo')).toHaveAttribute('aria-describedby', 'name-error');
    expect(onRegister).not.toHaveBeenCalled();
  });

  it('apaga o erro de um campo assim que o aluno volta a digitar nele', () => {
    render(<RegisterPage onRegister={vi.fn()} onBackToLogin={vi.fn()} />);
    criarConta();
    expect(screen.getByText('Informe seu nome.')).toBeInTheDocument();
    fireEvent.change(screen.getByLabelText('Nome completo'), { target: { value: 'L' } });
    expect(screen.queryByText('Informe seu nome.')).not.toBeInTheDocument();
    expect(screen.getByText('Informe seu e-mail.')).toBeInTheDocument();
  });

  it('envia os dados do aluno com curso e período quando tudo está válido', async () => {
    const onRegister = vi.fn().mockResolvedValue(undefined);
    render(<RegisterPage onRegister={onRegister} onBackToLogin={vi.fn()} />);
    preencher();
    criarConta();
    await waitFor(() => expect(onRegister).toHaveBeenCalledOnce());
    expect(onRegister).toHaveBeenCalledWith({
      name: '  Luisa Aluna  ',
      email: 'luisa@ufop.edu.br',
      password: 'senha123',
      passwordConfirmation: 'senha123',
      course: 'Ciência da Computação',
      period: 4,
    });
  });

  it('bloqueia o formulário enquanto a conta está sendo criada', async () => {
    let concluir: () => void = () => {};
    const onRegister = vi.fn(() => new Promise<void>((resolve) => { concluir = resolve; }));
    render(<RegisterPage onRegister={onRegister} onBackToLogin={vi.fn()} />);
    preencher();
    criarConta();
    expect(await screen.findByRole('button', { name: 'Aguarde…' })).toBeDisabled();
    expect(screen.getByLabelText('Nome completo')).toBeDisabled();
    expect(screen.getByRole('button', { name: 'Já tenho conta' })).toBeDisabled();
    concluir();
    expect(await screen.findByRole('button', { name: 'Criar conta' })).toBeEnabled();
  });

  it('avisa quando o e-mail já tem conta e permite tentar de novo', async () => {
    const onRegister = vi.fn().mockRejectedValue({ code: 'auth/email-already-in-use' });
    render(<RegisterPage onRegister={onRegister} onBackToLogin={vi.fn()} />);
    preencher();
    criarConta();
    expect(await screen.findByRole('alert')).toHaveTextContent(
      'Este e-mail já possui uma conta. Entre para continuar.',
    );
    expect(screen.getByRole('button', { name: 'Criar conta' })).toBeEnabled();
  });

  it('volta para o login em "Já tenho conta"', () => {
    const onBackToLogin = vi.fn();
    const onRegister = vi.fn();
    render(<RegisterPage onRegister={onRegister} onBackToLogin={onBackToLogin} />);
    fireEvent.click(screen.getByRole('button', { name: 'Já tenho conta' }));
    expect(onBackToLogin).toHaveBeenCalledOnce();
    expect(onRegister).not.toHaveBeenCalled();
  });
});
