#!/usr/bin/env bash
source "$(dirname -- "${BASH_SOURCE[0]}")/local-env.sh"
require_linux
for task_command in git bun node java cmake ninja g++; do
  require_command "$task_command"
done
node -e 'const [major,minor]=process.versions.node.split(".").map(Number); if(major<22 || (major===22 && minor<12)) { console.error("Node 22.12+ obrigatório"); process.exit(1); }'
if [[ "$(bun --version)" != "$(tr -d '\r\n' < "$task_root/.bun-version")" ]]; then
  printf 'Use a versão Bun indicada em .bun-version.\n' >&2
  exit 1
fi
task_java_version="$(java -version 2>&1)"
if [[ ! "$task_java_version" =~ version\ \"([0-9]+) ]] || (( BASH_REMATCH[1] < 21 )); then
  printf 'Java 21 ou superior obrigatório para o Firestore emulado.\n' >&2
  exit 1
fi
for task_directory in tools frontend; do
  (cd "$task_root/$task_directory" && bun install --frozen-lockfile)
done
build_local_backend
node "$task_root/tools/node_modules/firebase-tools/lib/bin/firebase.js" \
  setup:emulators:firestore --config "$task_root/firebase.local.json" --project demo-bcc323
printf 'Ambiente preparado. Use bash scripts/dev-local.sh firebase|backend|frontend.\n'
