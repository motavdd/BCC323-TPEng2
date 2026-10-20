import { getApp, getApps, initializeApp } from 'firebase/app';
import { connectAuthEmulator, getAuth } from 'firebase/auth';
import { connectFirestoreEmulator, getFirestore } from 'firebase/firestore';

const useEmulators = import.meta.env.VITE_USE_EMULATORS === 'true';
const projectId = import.meta.env.VITE_FIREBASE_PROJECT_ID;
const apiKey = import.meta.env.VITE_FIREBASE_API_KEY;
const appId = import.meta.env.VITE_FIREBASE_APP_ID;

if (!useEmulators && (!projectId || !apiKey || !appId)) {
  throw new Error('Configuração Firebase ausente no build de produção.');
}

const app = getApps().length ? getApp() : initializeApp({
  projectId: projectId || 'demo-bcc323',
  apiKey: apiKey || 'demo-key',
  authDomain: import.meta.env.VITE_FIREBASE_AUTH_DOMAIN || 'demo-bcc323.firebaseapp.com',
  appId: appId || '1:123456789:web:demo',
});

export const auth = getAuth(app);
export const db = getFirestore(app);

if (useEmulators) {
  // Browser no Docker Playwright: "firebase". Browser do host: "localhost".
  const host = import.meta.env.VITE_FIREBASE_EMULATOR_HOST || 'localhost';
  connectAuthEmulator(auth, `http://${host}:9099`, { disableWarnings: true });
  connectFirestoreEmulator(db, host, 8080);
}
