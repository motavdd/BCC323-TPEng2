# Compartilhado pelos scripts locais. As variáveis afetam apenas este processo.
$ErrorActionPreference = 'Stop'
$taskRoot = Split-Path $PSScriptRoot -Parent
$taskTools = Join-Path $taskRoot '.tools'
$taskBuild = Join-Path $taskRoot 'backend/build/native'
$taskNode = Get-ChildItem "$taskTools/node" -Directory -ErrorAction SilentlyContinue |
    Where-Object { Test-Path (Join-Path $_.FullName 'node.exe') } | Select-Object -First 1
if ($taskNode) { $env:PATH = "$($taskNode.FullName);$env:PATH" }
$taskCompiler = Get-Command g++ -ErrorAction SilentlyContinue
if (!$taskCompiler -and (Test-Path 'C:/msys64/ucrt64/bin/g++.exe')) {
    $env:PATH = "C:\msys64\ucrt64\bin;$env:PATH"
    $taskCompiler = Get-Command g++
}
if ($taskCompiler) {
    $env:PATH = "$(Split-Path $taskCompiler.Source);$env:PATH"
}
if (Test-Path "$taskTools/ninja/ninja.exe") {
    $env:PATH = "$taskTools\ninja;$env:PATH"
}
$taskJava = Get-ChildItem "$taskTools/java" -Directory -ErrorAction SilentlyContinue |
    Where-Object { Test-Path (Join-Path $_.FullName 'bin/java.exe') } | Select-Object -First 1
if ($taskJava) {
    $env:JAVA_HOME = $taskJava.FullName
    $env:PATH = "$env:JAVA_HOME\bin;$env:PATH"
}
if (Test-Path "$taskTools/ca-windows.pem") {
    $env:NODE_EXTRA_CA_CERTS = "$taskTools/ca-windows.pem"
}
$env:FIREBASE_EMULATORS_PATH = "$taskTools/firebase-emulators"
$env:PLAYWRIGHT_BROWSERS_PATH = "$taskTools/playwright"

function Invoke-Checked {
    param([string]$Executable, [string[]]$Arguments)
    & $Executable @Arguments
    if ($LASTEXITCODE -ne 0) { throw "$Executable terminou com código $LASTEXITCODE" }
}

function Build-LocalBackend {
    if (!$taskCompiler) { throw 'Instale GCC/MinGW (MSYS2 UCRT64) e adicione g++ ao PATH.' }
    Invoke-Checked cmake @('-S', "$taskRoot/backend", '-B', $taskBuild, '-G', 'Ninja',
        "-DCMAKE_CXX_COMPILER=$($taskCompiler.Source)", '-DCMAKE_BUILD_TYPE=Debug', '-DBUILD_TESTING=OFF')
    Invoke-Checked cmake @('--build', $taskBuild, '--parallel', '2')
}

function Set-LocalFrontendEnvironment {
    $env:VITE_USE_EMULATORS = 'true'
    $env:VITE_FIREBASE_EMULATOR_HOST = '127.0.0.1'
    $env:VITE_FIREBASE_PROJECT_ID = 'demo-bcc323'
    $env:VITE_FIREBASE_API_KEY = 'demo-key'
    $env:VITE_FIREBASE_AUTH_DOMAIN = 'demo-bcc323.firebaseapp.com'
    $env:VITE_FIREBASE_APP_ID = '1:123456789:web:demo'
    $env:API_PROXY_TARGET = 'http://127.0.0.1:8081'
}
