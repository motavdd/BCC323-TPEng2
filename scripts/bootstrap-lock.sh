#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
# Atualiza apenas o lockfile usando a mesma versão de Bun do CI.
BUN_VERSION=$(tr -d '\r\n' < .bun-version)
MSYS_NO_PATHCONV=1 docker run --rm -v "$(pwd)/frontend:/app" -w /app "oven/bun:${BUN_VERSION}" bun install --lockfile-only
printf '\nLockfile gerado: frontend/bun.lock\n'
printf 'Revise e versione o arquivo antes de executar bun install --frozen-lockfile.\n'
