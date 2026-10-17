# Verificações rápidas; -Integration usa os três serviços nativos em execução.
param([switch]$Integration)
. "$PSScriptRoot/local-env.ps1"
Build-LocalBackend
Set-LocalFrontendEnvironment
Push-Location "$taskRoot/frontend"
try {
    foreach ($taskName in @('lint', 'typecheck', 'test:unit', 'build')) {
        Invoke-Checked bun @('run', $taskName)
    }
    if ($Integration) {
        Write-Host 'Integração nativa: mantenha firebase, backend e frontend locais em execução.'
        $taskHealth = Invoke-RestMethod 'http://127.0.0.1:5173/api/health'
        if ($taskHealth.service -ne 'bcc323-api' -or $taskHealth.status -ne 'ok') {
            throw 'O frontend não está conectado à API C++ local.'
        }
        $env:FIRESTORE_EMULATOR_HOST = '127.0.0.1:8080'
        $env:FIREBASE_PROJECT_ID = 'demo-bcc323'
        $env:FIRESTORE_RULES_PATH = "$taskRoot/firebase/firestore.rules"
        $env:E2E_BASE_URL = 'http://127.0.0.1:5173'
        Invoke-Checked bun @('run', 'test:integration')
        Invoke-Checked node @("$taskRoot/frontend/node_modules/@playwright/test/cli.js",
            'install', 'chromium', '--only-shell')
        Invoke-Checked bun @('run', 'test:e2e')
    }
}
finally { Pop-Location }
if ($Integration) { Write-Host 'Verificações e integração nativas aprovadas.' }
else { Write-Host 'Verificações locais aprovadas. Use -Integration com os serviços ativos ou aguarde o CI do PR.' }
