#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
trap 'docker compose -f compose.test.yml down -v --remove-orphans' EXIT
docker compose -f compose.test.yml build
for task in lint typecheck test:unit build; do
  docker compose -f compose.test.yml run --rm --no-deps frontend bun run "$task"
done
docker compose -f compose.test.yml run --rm frontend bun run test:integration
docker compose -f compose.test.yml run --rm playwright
printf '\nTodos os testes terminaram com sucesso.\n'
