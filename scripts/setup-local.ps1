# Preparação nativa para Windows x64. Não instala ferramentas globalmente.
. "$PSScriptRoot/local-env.ps1"
foreach ($taskCommand in @('git', 'bun', 'cmake')) {
    if (!(Get-Command $taskCommand -ErrorAction SilentlyContinue)) {
        throw "Instale $taskCommand e adicione-o ao PATH. Consulte docs/AMBIENTE.md."
    }
}
if (!$taskCompiler) { throw 'Instale GCC/MinGW pelo MSYS2 UCRT64; consulte docs/AMBIENTE.md.' }
New-Item -ItemType Directory -Force $taskTools | Out-Null

function Install-PortableArchive {
    param([string]$Url, [string]$Sha256, [string]$Name, [string]$ExpectedFile)
    if (Test-Path $ExpectedFile) { return }
    $taskArchive = Join-Path $taskTools "$Name.zip"
    Invoke-WebRequest -Uri $Url -OutFile $taskArchive
    if ((Get-FileHash -LiteralPath $taskArchive -Algorithm SHA256).Hash -ne $Sha256) {
        throw "Checksum inválido para $Name. O arquivo não será extraído."
    }
    Expand-Archive -LiteralPath $taskArchive -DestinationPath "$taskTools/$Name" -Force
    Remove-Item -LiteralPath $taskArchive
}

Install-PortableArchive `
    'https://nodejs.org/dist/v22.23.3/node-v22.23.3-win-x64.zip' `
    '2b0ff57b049cda1bbcea2240eec20467018713c1efe1f7360c2681859b90ed71' `
    'node' "$taskTools/node/node-v22.23.3-win-x64/node.exe"
Install-PortableArchive `
    'https://github.com/ninja-build/ninja/releases/download/v1.13.1/ninja-win.zip' `
    '26a40fa8595694dec2fad4911e62d29e10525d2133c9a4230b66397774ae25bf' `
    'ninja' "$taskTools/ninja/ninja.exe"
Install-PortableArchive `
    'https://github.com/adoptium/temurin21-binaries/releases/download/jdk-21.0.12.1%2B1/OpenJDK21U-jre_x64_windows_hotspot_21.0.12.1_1.zip' `
    'd35f31e712f0fcf6ac5a093edc90204fbff22f720ba3950bd09d331d5e621636' `
    'java' "$taskTools/java/jdk-21.0.12.1+1-jre/bin/java.exe"

# Bun e Node usam as autoridades já confiáveis no Windows, sem desativar TLS.
$taskPem = Get-ChildItem Cert:\LocalMachine\Root,Cert:\CurrentUser\Root,Cert:\LocalMachine\CA,Cert:\CurrentUser\CA |
    ForEach-Object { "-----BEGIN CERTIFICATE-----`n" +
        [Convert]::ToBase64String($_.RawData, [Base64FormattingOptions]::InsertLineBreaks) +
        "`n-----END CERTIFICATE-----" }
[IO.File]::WriteAllText("$taskTools/ca-windows.pem", ($taskPem -join "`n"))
. "$PSScriptRoot/local-env.ps1"
foreach ($taskDirectory in @('tools', 'frontend')) {
    Push-Location "$taskRoot/$taskDirectory"
    try { Invoke-Checked bun @('install', '--frozen-lockfile', '--cafile', "$taskTools/ca-windows.pem") }
    finally { Pop-Location }
}
Build-LocalBackend
Invoke-Checked node @("$taskRoot/tools/node_modules/firebase-tools/lib/bin/firebase.js",
    'setup:emulators:firestore', '--config', "$taskRoot/firebase.local.json", '--project', 'demo-bcc323')
Write-Host 'Ambiente preparado. Inicie firebase, backend e frontend com scripts/dev-local.ps1.'
