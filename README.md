# BCC323-TPEng2

Protótipo de planejamento acadêmico com React, TypeScript, Vite e Firebase. O gerenciador de pacotes é **Bun 1.3.14**; o lockfile do frontend é `frontend/bun.lock`.

## Executar com Docker

Requer Docker com Compose V2:

```sh
docker compose -f compose.dev.yml up --build
```

Abra http://localhost:5173. Auth e Firestore usam o projeto de demonstração `demo-bcc323`, sem credenciais de produção.

## Executar o frontend localmente

Instale Bun 1.3.14 e Node 22.12+ (runtime das ferramentas). Na raiz, inicie os emuladores:

```sh
docker compose -f compose.dev.yml up --build firebase
```

Em outro terminal:

```sh
cd frontend
cp .env.example .env.local
bun install --frozen-lockfile
bun run dev
```

No PowerShell, use `Copy-Item .env.example .env.local` para copiar o arquivo.

## Validar

Na raiz, execute `bash scripts/test-all.sh` ou, no PowerShell, `./scripts/test-all.ps1`. A bateria inclui lint, TypeScript, testes unitários, build, regras Firestore e cadastro/logout no Chromium com emuladores.

Para qualidade local, em `frontend/`: `bun run lint`, `bun run typecheck`, `bun run test:unit` e `bun run build`. Use `bun run build` para executar o build do projeto.

## Documentação

- [Desenvolvimento, dependências e Bun](docs/BUN.md)
- [Implantação e GitHub Actions](docs/IMPLANTACAO.md)
- [Segurança e avaliação do override gRPC](docs/SEGURANCA.md)
- [Plano técnico e contexto anterior](docs/PLANO_TECNICO.md)
- [Relatório desta migração](docs/RELATORIO_MIGRACAO_BUN.md)

Os workflows locais preservam CI e deploy de Hosting/regras. Publicar esta cópia ou configurar GitHub/Firebase requer uma ação posterior do mantenedor. Este pacote foi preparado sem commits, push ou alterações remotas.
