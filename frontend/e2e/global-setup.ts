import { execFileSync } from 'node:child_process';
import { resolve } from 'node:path';

export default function setup() {
  execFileSync(process.execPath, [process.env.SEED_SCRIPT_PATH || resolve('../scripts/seed-demo.mjs')], {
    env: { ...process.env, FIREBASE_PROJECT_ID: 'demo-bcc323', FIRESTORE_EMULATOR_HOST: process.env.FIRESTORE_EMULATOR_HOST || '127.0.0.1:8080' },
    stdio: 'inherit',
  });
}
