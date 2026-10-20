import { api, type Curriculum, type Evaluation, type Profile } from './api';

// Contrato compartilhado pelas telas do MVP. Não recebe UID: a API usa o token.
export const academicApi = {
  profile: () => api<{ profile: Profile | null }>('/profile'),
  saveProfile: (profile: Pick<Profile, 'course' | 'period'> & Partial<Pick<Profile, 'courseId' | 'displayName'>>) =>
    api<{ profile: Profile }>('/profile', profile, 'PUT'),
  courses: () => api<{ courses: { id: string; name: string; illustrative: boolean }[] }>('/courses'),
  curriculum: () => api<Curriculum>('/curriculum'),
  summary: () => api<Evaluation>('/academic/summary'),
  history: () => api<{ completed: string[]; unmappedCompleted: string[] }>('/history'),
  setCompleted: (id: string, done: boolean) => api<Evaluation>(`/history/${encodeURIComponent(id)}`, undefined, done ? 'PUT' : 'DELETE'),
};
