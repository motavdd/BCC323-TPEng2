import { getApp, getApps, initializeApp } from 'firebase/app';
import { connectAuthEmulator, getAuth } from 'firebase/auth';

const useEmulators = import.meta.env.VITE_USE_EMULATORS === 'true';
const projectId = import.meta.env.VITE_FIREBASE_PROJECT_ID;
const apiKey = import.meta.env.VITE_FIREBASE_API_KEY;
const appId = import.meta.env.VITE_FIREBASE_APP_ID;

if (!useEmulators && (!projectId || !apiKey || !appId)) {
  throw new Error('Configuração Firebase ausente no build de produção.');
}

export const app = getApps().length ? getApp() : initializeApp({
  projectId: projectId || 'demo-bcc323',
  apiKey: apiKey || 'demo-key',
  authDomain: import.meta.env.VITE_FIREBASE_AUTH_DOMAIN || 'demo-bcc323.firebaseapp.com',
  appId: appId || '1:123456789:web:demo',
});

export const auth = getAuth(app);

// No Docker, o navegador usa o nome do serviço. No host, usa localhost.
export const emulatorHost = useEmulators
  ? import.meta.env.VITE_FIREBASE_EMULATOR_HOST || 'localhost'
  : undefined;

if (emulatorHost) {
  connectAuthEmulator(auth, `http://${emulatorHost}:9099`, { disableWarnings: true });
}
