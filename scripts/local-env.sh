#!/usr/bin/env bash
# Shared by the native Linux entry points; does not change global installations.
set -euo pipefail
task_root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
task_tools="$task_root/.tools"
# Keep Linux CMake caches separate from Windows in shared/WSL checkouts.
task_build="$task_root/backend/build/linux"
export FIREBASE_EMULATORS_PATH="$task_tools/firebase-emulators"
export PLAYWRIGHT_BROWSERS_PATH="$task_tools/playwright"

require_command() {
  if ! command -v "$1" >/dev/null 2>&1; then
    printf 'Instale %s e adicione ao PATH. Consulte docs/DESENVOLVIMENTO_LOCAL.md.\n' "$1" >&2
    exit 1
  fi
}

require_linux() {
  if [[ "$(uname -s)" != Linux ]]; then
    printf 'Estes scripts nativos exigem Linux. No Windows, use scripts/*.ps1.\n' >&2
    exit 1
  fi
}

build_local_backend() {
  require_command cmake
  require_command ninja
  require_command g++
  cmake -S "$task_root/backend" -B "$task_build" -G Ninja \
    "-DCMAKE_CXX_COMPILER=$(command -v g++)" -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=OFF
  cmake --build "$task_build" --parallel 2
}

set_local_frontend_environment() {
  export VITE_USE_EMULATORS=true
  export VITE_FIREBASE_EMULATOR_HOST=127.0.0.1
  export VITE_FIREBASE_PROJECT_ID=demo-bcc323
  export VITE_FIREBASE_API_KEY=demo-key
  export VITE_FIREBASE_AUTH_DOMAIN=demo-bcc323.firebaseapp.com
  export VITE_FIREBASE_APP_ID=1:123456789:web:demo
  export API_PROXY_TARGET=http://127.0.0.1:8081
}
