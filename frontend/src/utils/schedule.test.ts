import { describe, expect, it } from 'vitest';
import { hasScheduleConflict } from './schedule';

describe('Conflito de horários', () => {
  it('detecta sobreposição no mesmo dia', () => {
    expect(hasScheduleConflict(
      { day: 'segunda', start: '08:00', end: '10:00' },
      { day: 'segunda', start: '09:00', end: '11:00' },
    )).toBe(true);
  });
  it('permite horários consecutivos', () => {
    expect(hasScheduleConflict(
      { day: 'segunda', start: '08:00', end: '10:00' },
      { day: 'segunda', start: '10:00', end: '12:00' },
    )).toBe(false);
  });
  it('não identifica conflito em dias diferentes', () => {
    expect(hasScheduleConflict(
      { day: 'segunda', start: '08:00', end: '10:00' },
      { day: 'terça', start: '09:00', end: '11:00' },
    )).toBe(false);
  });
  it('rejeita intervalo inválido', () => {
    expect(() => hasScheduleConflict(
      { day: 'segunda', start: '10:00', end: '08:00' },
      { day: 'segunda', start: '09:00', end: '11:00' },
    )).toThrow();
  });
});
