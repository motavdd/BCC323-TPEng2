import { auth } from './firebase';
import type { ClassTime } from '../utils/schedule';
export type Subject = { id: string; name: string; hours: number; period: number; prerequisites: string[]; requirementsKnown: boolean };
export type Offering = { id: string; subjectId: string; group: string; times: ClassTime[] };
export type Curriculum = { id: string; name: string; illustrative: boolean; subjects: Subject[]; offerings: Offering[] };
export type Profile = { displayName: string; course: string; courseId?: string; period: number };
export type Evaluation = { courseId: string; illustrative: boolean; completed: string[]; unmappedCompleted: string[]; completedHours: number; totalHours: number; completedCount: number; availableCount: number; progress: number; subjects: Record<string, { state: 'completed' | 'available' | 'blocked' | 'unknown' | 'in_progress'; missing: string[] }> };
export async function api<T>(path: string, body?: unknown, method?: 'GET' | 'POST' | 'PUT' | 'DELETE'): Promise<T> {
  const user = auth.currentUser;
  if (!user) throw new Error('Entre novamente para continuar.');
  const token = await user.getIdToken();
  const response = await fetch(`/api${path}`, {
    method: method || (body === undefined ? 'GET' : 'POST'),
    signal: AbortSignal.timeout(15000),
    headers: { Authorization: `Bearer ${token}`, ...(body === undefined ? {} : { 'Content-Type': 'application/json' }) },
    ...(body === undefined ? {} : { body: JSON.stringify(body) }),
  });
  if (!response.headers.get('content-type')?.includes('application/json')) throw new Error('A API não está disponível neste endereço.');
  const data = await response.json();
  if (!response.ok) throw new Error(data.error?.message || 'Não foi possível acessar a API.');
  return data as T;
}
