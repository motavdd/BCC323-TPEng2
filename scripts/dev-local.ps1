param([Parameter(Mandatory)][ValidateSet('firebase', 'backend', 'frontend')][string]$Service)
. "$PSScriptRoot/local-env.ps1"
Push-Location $taskRoot
try {
    switch ($Service) {
        firebase {
            $taskFirebase = "$taskRoot/tools/node_modules/firebase-tools/lib/bin/firebase.js"
            if (!(Test-Path $taskFirebase)) { throw 'Execute ./scripts/setup-local.ps1 primeiro.' }
            $taskData = "$taskRoot/.firebase-data"
            $taskArguments = @($taskFirebase, 'emulators:start', '--config', "$taskRoot/firebase.local.json",
                '--project', 'demo-bcc323', '--only', 'auth,firestore', "--export-on-exit=$taskData")
            if (Test-Path "$taskData/firebase-export-metadata.json") { $taskArguments += "--import=$taskData" }
            Invoke-Checked node $taskArguments
        }
        backend {
            Build-LocalBackend
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
