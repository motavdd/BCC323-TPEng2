import { readFile } from 'node:fs/promises';
import { pathToFileURL } from 'node:url';

// Somente dados demonstrativos em emulador. Não usa credenciais de produção.
const project = process.env.FIREBASE_PROJECT_ID || 'demo-bcc323';
const host = process.env.FIRESTORE_EMULATOR_HOST || '127.0.0.1:8080';
if (!project.startsWith('demo-') || !/^[a-zA-Z0-9.-]+:\d{1,5}$/.test(host)) {
  throw new Error('O seed exige projeto demo-* e FIRESTORE_EMULATOR_HOST=host:porta.');
}
const fixture = process.env.COURSE_FIXTURE_PATH
  ? pathToFileURL(process.env.COURSE_FIXTURE_PATH)
  : new URL('../backend/test/fixtures/curriculum.json', import.meta.url);
const course = JSON.parse(await readFile(fixture, 'utf8'));
if (!course.illustrative || !/^[a-zA-Z0-9_-]+$/.test(course.id)) throw new Error('Use apenas um catálogo ilustrativo válido.');
function encode(value) {
  if (value === null) return { nullValue: null };
  if (typeof value === 'string') return { stringValue: value };
  if (typeof value === 'boolean') return { booleanValue: value };
  if (typeof value === 'number') return Number.isInteger(value) ? { integerValue: String(value) } : { doubleValue: value };
  if (Array.isArray(value)) return { arrayValue: { values: value.map(encode) } };
  return { mapValue: { fields: Object.fromEntries(Object.entries(value).map(([key, item]) => [key, encode(item)])) } };
}
const url = `http://${host}/v1/projects/${project}/databases/(default)/documents/courses/${course.id}`;
const headers = { Authorization: 'Bearer owner', 'Content-Type': 'application/json' };
const existing = await fetch(url, { headers, signal: AbortSignal.timeout(10000) });
if (existing.ok) {
  console.log(`Catálogo ${course.id} já cadastrado; dados preservados.`);
} else {
  if (existing.status !== 404) throw new Error(`Não foi possível consultar o emulador: ${existing.status}`);
  const response = await fetch(`${url}?currentDocument.exists=false`, {
    method: 'PATCH', headers, body: JSON.stringify({ fields: encode(course).mapValue.fields }), signal: AbortSignal.timeout(10000),
  });
  if (!response.ok) throw new Error(`Não foi possível cadastrar o catálogo: ${response.status}`);
  console.log(`Catálogo ilustrativo ${course.id} cadastrado no emulador.`);
}
