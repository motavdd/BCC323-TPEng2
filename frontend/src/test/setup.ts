import '@testing-library/jest-dom/vitest';
import { cleanup } from '@testing-library/react';
import { afterEach } from 'vitest';

// Sem `globals: true` no Vitest, a Testing Library não desmonta sozinha os
// componentes entre um teste e outro; sem isso as telas se acumulam no DOM.
afterEach(() => cleanup());
