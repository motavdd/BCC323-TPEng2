import { defineConfig } from 'vitest/config';
import react from '@vitejs/plugin-react';
import tailwindcss from '@tailwindcss/vite';
import { loadEnv } from 'vite';

export default defineConfig(({ mode }) => {
  const env = loadEnv(mode, process.cwd(), 'API_');
  const proxy = {
    '/api': {
      target: process.env.API_PROXY_TARGET || env.API_PROXY_TARGET || 'http://127.0.0.1:8081',
      changeOrigin: true,
    },
  };
  return {
    plugins: [react(), tailwindcss()],
    server: { proxy },
    // Nome do serviço acessado pelo Chromium na rede interna do Compose.
    preview: { allowedHosts: ['web'], proxy },
    test: {
      environment: 'jsdom',
      setupFiles: ['./src/test/setup.ts'],
      include: ['src/**/*.test.ts', 'src/**/*.test.tsx'],
      clearMocks: true,
    },
  };
});
