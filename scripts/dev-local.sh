#!/usr/bin/env bash
source "$(dirname -- "${BASH_SOURCE[0]}")/local-env.sh"
require_linux
cd "$task_root"
if (( $# > 1 )); then
  printf 'Uso: bash scripts/dev-local.sh [all|firebase|backend|frontend]\n' >&2
  exit 1
fi
task_service="${1:-all}"
case "$task_service" in
  all|firebase)
    require_command node
    task_firebase="$task_root/tools/node_modules/firebase-tools/lib/bin/firebase.js"
    if [[ ! -f "$task_firebase" ]]; then
      printf 'Execute bash scripts/setup-local.sh primeiro.\n' >&2
      exit 1
    fi
    task_command=emulators:start
    if [[ "$task_service" == all ]]; then
      node scripts/run-local.mjs --check-ports
      if [[ ! -f "$task_root/frontend/node_modules/vite/bin/vite.js" ]]; then
        printf 'Execute bash scripts/setup-local.sh primeiro.\n' >&2
        exit 1
      fi
      build_local_backend
      set_local_frontend_environment
      export BCC323_LOCAL_API="$task_build/bin/bcc323-api"
      task_command=emulators:exec
    fi
    task_data="$task_root/.firebase-data"
    task_arguments=("$task_command" --config "$task_root/firebase.local.json"
      --project demo-bcc323 --only auth,firestore "--export-on-exit=$task_data")
    if [[ -f "$task_data/firebase-export-metadata.json" ]]; then
      task_arguments+=("--import=$task_data")
    fi
    if [[ "$task_service" == all ]]; then
      task_arguments+=(--ui "node scripts/run-local.mjs")
    fi
    exec node "$task_firebase" "${task_arguments[@]}"
    ;;
  backend)
    build_local_backend
    export HOST=127.0.0.1 PORT=8081
    export FIREBASE_PROJECT_ID=demo-bcc323 FIREBASE_AUTH_EMULATOR_HOST=127.0.0.1:9099
    export FIRESTORE_EMULATOR_HOST=127.0.0.1:8080
    exec "$task_build/bin/bcc323-api"
    ;;
  frontend)
    require_command bun
    set_local_frontend_environment
    cd "$task_root/frontend"
    exec bun run dev --host 127.0.0.1 --port 5173 --strictPort
    ;;
  *) printf 'Serviço inválido: %s. Use all, firebase, backend ou frontend.\n' "$task_service" >&2; exit 1 ;;
esac
