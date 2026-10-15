$ErrorActionPreference = 'Stop'
Push-Location (Join-Path $PSScriptRoot '..')
function Invoke-Compose {
    & docker compose -f compose.test.yml @args
    if ($LASTEXITCODE -ne 0) { throw "Docker Compose terminou com código $LASTEXITCODE" }
}
try {
    Invoke-Compose build
    foreach ($taskName in @('lint', 'typecheck', 'test:unit', 'build')) {
        Invoke-Compose run --rm --no-deps frontend bun run $taskName
    }
    Invoke-Compose run --rm frontend bun run test:integration
    Invoke-Compose run --rm playwright
    Write-Host 'Todos os testes terminaram com sucesso.'
}
finally {
    try { Invoke-Compose down -v --remove-orphans }
    finally { Pop-Location }
}
