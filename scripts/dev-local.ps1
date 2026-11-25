param([ValidateSet('all', 'firebase', 'backend', 'frontend')][string]$Service = 'all')
. "$PSScriptRoot/local-env.ps1"
Push-Location $taskRoot
try {
    switch ($Service) {
        { $_ -in 'all', 'firebase' } {
            $taskFirebase = "$taskRoot/tools/node_modules/firebase-tools/lib/bin/firebase.js"
            if (!(Test-Path $taskFirebase)) { throw 'Execute ./scripts/setup-local.ps1 primeiro.' }
            $taskData = "$taskRoot/.firebase-data"
            $taskCommand = 'emulators:start'
            if ($Service -eq 'all') {
                Invoke-Checked node @('scripts/run-local.mjs', '--check-ports')
                if (!(Test-Path "$taskRoot/frontend/node_modules/vite/bin/vite.js")) {
                    throw 'Execute ./scripts/setup-local.ps1 primeiro.'
                }
                Build-LocalBackend
                Set-LocalFrontendEnvironment
                $env:BCC323_LOCAL_API = "$taskBuild/bin/bcc323-api.exe"
                $taskCommand = 'emulators:exec'
            }
            $taskArguments = @($taskFirebase, $taskCommand, '--config', "$taskRoot/firebase.local.json",
                '--project', 'demo-bcc323', '--only', 'auth,firestore', "--export-on-exit=$taskData")
            if (Test-Path "$taskData/firebase-export-metadata.json") { $taskArguments += "--import=$taskData" }
            if ($Service -eq 'all') { $taskArguments += @('--ui', 'node scripts/run-local.mjs') }
            Invoke-Checked node $taskArguments
        }
        backend {
            Build-LocalBackend
            $env:FIREBASE_PROJECT_ID = 'demo-bcc323'
            $env:FIREBASE_AUTH_EMULATOR_HOST = '127.0.0.1:9099'
            $env:FIRESTORE_EMULATOR_HOST = '127.0.0.1:8080'
            $env:PORT = '8081'
            $env:HOST = '127.0.0.1'
            Invoke-Checked "$taskBuild/bin/bcc323-api.exe" @()
        }
        frontend {
            Set-LocalFrontendEnvironment
            Set-Location "$taskRoot/frontend"
            Invoke-Checked bun @('run', 'dev', '--host', '127.0.0.1', '--port', '5173', '--strictPort')
        }
    }
}
finally { Pop-Location }
