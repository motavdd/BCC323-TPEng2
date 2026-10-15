# Desenvolvimento com Bun

Use **Bun 1.3.14** e **Node 22.12+**. Bun gerencia os pacotes e chama os scripts; Vite, Vitest, Playwright e a CLI Firebase continuam usando Node conforme seus executáveis. Não é necessário forçar o runtime com `--bun`.

Instalação do Bun: https://bun.sh/docs/installation. Reinicie o terminal após instalar para disponibilizar `bun` no PATH.

Na pasta `frontend`:

```sh
bun install --frozen-lockfile
bun run dev
```

O Vite serve a aplicação em `http://localhost:5173`. Para usar os emuladores, copie `.env.example` para `.env.local` e inicie o serviço Firebase na raiz:

```sh
docker compose -f compose.dev.yml up --build firebase
```

Também é possível executar frontend e emuladores juntos com `docker compose -f compose.dev.yml up --build`.

## Verificação

Na pasta `frontend`:

```sh
bun run lint
bun run typecheck
bun run test:unit
bun run build
bun run preview
```

Use `bun run build`, pois `bun build` chama o bundler próprio do Bun. Os testes existentes continuam no Vitest e no Playwright.

Na raiz, execute a bateria completa com emuladores e Chromium:

```sh
bash scripts/test-all.sh
```

## Dependências e lockfile

`frontend/bun.lock` é o único lockfile do frontend. A instalação congelada falha quando ele diverge de `package.json`, em vez de atualizar versões silenciosamente. Docker, CI e deploy usam `bun install --frozen-lockfile`.

Para alterar dependências em `frontend`:

```sh
bun add nome-do-pacote
bun add --dev nome-da-ferramenta
bun remove nome-do-pacote
```

Revise e versione `package.json` e `bun.lock` juntos. Para resolver apenas o lockfile, use `bun install --lockfile-only` ou, na raiz, `bash scripts/bootstrap-lock.sh` (Docker). Não é necessário regenerá-lo antes de cada teste.

Para atualizar o próprio Bun, mantenha `.bun-version`, `packageManager`, `engines.bun` e as três imagens Docker alinhados. O script de bootstrap e os workflows de deploy leem `.bun-version`. Dependabot usa o ecossistema `bun`.

No PowerShell, a bateria completa pode ser executada com `./scripts/test-all.ps1` na raiz. Ambos os scripts encerram os containers de teste e removem seus volumes ao terminar.

Para verificar dependências instaladas e vulnerabilidades, execute `bun pm ls --all` e `bun audit` em `frontend/`. A configuração do override gRPC está descrita em [Segurança](SEGURANCA.md).

O preview permite explicitamente o host interno `web` para os testes via Compose. Os comandos de saúde dos serviços usam Bun. Node permanece nas imagens e no setup dos deploys para respeitar os executáveis das ferramentas; não é usado para instalar pacotes.

## Firebase e deploy

As imagens utilizam Node e Java 21 para a CLI e os emuladores Firebase. A CLI Firebase 15.0.0 é instalada com Bun. As variáveis `VITE_*` são definidas antes do build de produção. A configuração dos ambientes e da autenticação WIF está descrita em [Implantação](IMPLANTACAO.md).

Referências: [instalação congelada](https://bun.sh/docs/pm/cli/install), [scripts](https://bun.sh/docs/runtime), [Dependabot](https://docs.github.com/en/code-security/reference/supply-chain-security/supported-ecosystems-and-repositories).
