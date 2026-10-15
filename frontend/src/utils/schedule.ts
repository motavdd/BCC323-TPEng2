export type ClassTime = { day: string; start: string; end: string };

function toMinutes(hhmm: string): number {
  if (!/^([01]\d|2[0-3]):[0-5]\d$/.test(hhmm)) {
    throw new Error(`Horário inválido: ${hhmm}`);
  }
  const [hours, minutes] = hhmm.split(':').map(Number);
  return hours * 60 + minutes;
}

export function hasScheduleConflict(a: ClassTime, b: ClassTime): boolean {
  const aStart = toMinutes(a.start);
  const aEnd = toMinutes(a.end);
  const bStart = toMinutes(b.start);
  const bEnd = toMinutes(b.end);
  if (aStart >= aEnd || bStart >= bEnd) {
    throw new Error('O início deve ser anterior ao término.');
  }
  return a.day === b.day && aStart < bEnd && bStart < aEnd;
}
