import { spawn } from 'node:child_process';
import { existsSync } from 'node:fs';
import { createServer } from 'node:net';
import { join } from 'node:path';
import { fileURLToPath } from 'node:url';
import { setTimeout as delay } from 'node:timers/promises';

const root = fileURLToPath(new URL('../', import.meta.url));
const frontend = join(root, 'frontend');
const host = '127.0.0.1';
const web = 'http://127.0.0.1:5173';

async function checkPorts() {
  for (const [service, port] of [
    ['frontend', 5173], ['API', 8081], ['Firestore', 8080], ['Authentication', 9099],
    ['painel Firebase', 4000], ['hub Firebase', 4400],
  ]) {
    const server = createServer();
    await new Promise((resolve, reject) => {
      server.once('error', (error) => reject(new Error(
        error.code === 'EADDRINUSE'
          ? `A porta ${port} (${service}) já está em uso. Encerre o serviço anterior antes de iniciar todos.`
          : `Não foi possível abrir a porta ${port} (${service}): ${error.message}`,
      )));
      server.listen({ host, port, exclusive: true }, () => server.close(resolve));
    });
  }
}

async function run() {
  const { BCC323_LOCAL_API, FIREBASE_AUTH_EMULATOR_HOST, FIRESTORE_EMULATOR_HOST } = process.env;
  if (!BCC323_LOCAL_API || !existsSync(BCC323_LOCAL_API) ||
      FIREBASE_AUTH_EMULATOR_HOST !== '127.0.0.1:9099' ||
      FIRESTORE_EMULATOR_HOST !== '127.0.0.1:8080') {
    throw new Error('Inicie pelo comando dev-local all com os emuladores locais.');
  }
  const children = new Set();
  let stopping = false;
  function stop() {
    stopping = true;
    for (const child of children) child.kill();
  }
  const start = (name, executable, args, options) => new Promise((resolve) => {
    const child = spawn(executable, args, { stdio: 'inherit', ...options });
    children.add(child);
    child.once('error', (error) => resolve({ name, error }));
    child.once('close', (code) => {
      children.delete(child);
      resolve({ name, code });
    });
  });
  process.once('SIGINT', stop);
  process.once('SIGTERM', stop);

  const env = { ...process.env, FIREBASE_PROJECT_ID: 'demo-bcc323' };
  try {
    const seed = await start('Catálogo', process.execPath, ['scripts/seed-demo.mjs'], { cwd: root, env });
    if (stopping) return;
    if (seed.error || seed.code !== 0) throw new Error('Falha ao preparar o catálogo.', { cause: seed.error });

    const api = start('API', BCC323_LOCAL_API, [], { cwd: root, env: { ...env, HOST: host, PORT: '8081' } });
    const vite = start('Frontend', process.execPath, [
      'node_modules/vite/bin/vite.js', '--host', host, '--port', '5173', '--strictPort',
    ], { cwd: frontend, env });
    const stopped = Promise.race([api, vite]);
    const ready = async () => {
      for (let attempt = 0; attempt < 40 && !stopping; attempt += 1) {
        try {
          const response = await fetch(`${web}/api/health`, { signal: AbortSignal.timeout(1000) });
          if (response.ok) {
            const health = await response.json();
            if (health.status === 'ok' && health.service === 'bcc323-api') return true;
          }
        } catch {
          // Vite ou a API ainda pode estar iniciando.
        }
        await delay(250);
      }
      return false;
    };
    const result = await Promise.race([ready(), stopped]);
    if (result === true) {
      console.log(`Aplicação pronta em ${web}. Pressione Ctrl+C para encerrar.`);
      const finished = await stopped;
      if (!stopping) throw new Error(`${finished.name} encerrou durante a execução.`, { cause: finished.error });
    } else if (!stopping) {
      if (result === false) throw new Error('O frontend não conseguiu acessar a API local.');
      throw new Error(`${result.name} encerrou durante a inicialização.`, { cause: result.error });
    }
  } finally {
    stop();
  }
}

try {
  if (process.argv[2] === '--check-ports') await checkPorts();
  else await run();
} catch (error) {
  console.error(error.message);
  if (error.cause) console.error(error.cause.message);
  process.exitCode = 1;
}
