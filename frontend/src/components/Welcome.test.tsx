import { render, screen } from '@testing-library/react';
import { describe, expect, it } from 'vitest';
import { Welcome } from './Welcome';

describe('Welcome', () => {
  it('mostra o email da pessoa autenticada', () => {
    render(<Welcome email="aluno@universidade.br" />);
    expect(screen.getByRole('status').textContent).toContain('aluno@universidade.br');
  });
});
