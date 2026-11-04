#!/usr/bin/env bash
source "$(dirname -- "${BASH_SOURCE[0]}")/local-env.sh"
require_linux
if (( $# > 1 )) || [[ "${1:-}" != '' && "${1:-}" != --integration ]]; then
  printf 'Uso: bash scripts/check-local.sh [--integration]\n' >&2
  exit 1
fi
require_command bun
build_local_backend
set_local_frontend_environment
cd "$task_root/frontend"
for task_name in lint typecheck test:unit build; do
  bun run "$task_name"
done
if [[ "${1:-}" == --integration ]]; then
  require_command node
  printf 'Integração: mantenha firebase, backend e frontend locais ativos.\n'
  node -e 'fetch("http://127.0.0.1:5173/api/health").then(async r=>{const h=await r.json();if(!r.ok||h.service!=="bcc323-api"||h.status!=="ok")throw Error("API local indisponível");}).catch(e=>{console.error(e);process.exit(1);})'
  export FIRESTORE_EMULATOR_HOST=127.0.0.1:8080
  export FIREBASE_PROJECT_ID=demo-bcc323
  export FIRESTORE_RULES_PATH="$task_root/firebase/firestore.rules"
  export E2E_BASE_URL=http://127.0.0.1:5173
  bun run test:integration
  node "$task_root/frontend/node_modules/@playwright/test/cli.js" install chromium --only-shell
  bun run test:e2e
fi
printf 'Verificações locais aprovadas. A bateria completa também executa no CI.\n'
