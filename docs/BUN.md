# Bun: comandos e dependências

O projeto usa **Bun 1.3.14** para instalar dependências e executar scripts.
`bun run` continua sendo a forma de iniciar e validar o frontend.
Os scripts PowerShell preparam o ambiente e chamam essas mesmas ferramentas.

## Responsabilidade de cada ferramenta

| Ferramenta | Uso |
|---|---|
| Bun | Instalar dependências e executar scripts de `package.json` |
| Node.js | Runtime dos executáveis JavaScript, incluindo Firebase CLI e Playwright |
| CMake, Ninja e GCC | Compilar o serviço C++ |
| Java 21 | Executar o emulador Firestore |

O setup nativo fornece Node.js 22.23.3 portátil em `.tools/`. Os scripts locais carregam esse runtime no PATH de seus processos. Em execução manual fora deles, use Node 22.12 ou superior.
As imagens e os workflows também incluem Node; os pacotes são instalados com Bun, sem npm. Não é necessário adicionar `--bun` aos comandos.
No Linux, os scripts `.sh` usam Node, Java, Ninja e compilador instalados no PATH.
Consulte os [pré-requisitos Linux](DESENVOLVIMENTO_LOCAL.md#preparar-no-linux).

## Comandos do frontend

Após [preparar o ambiente](DESENVOLVIMENTO_LOCAL.md), execute em `frontend/`:

| Comando | O que faz |
|---|---|
| `bun run dev` | Inicia Vite; os fluxos integrados dependem de C++ e Firebase ativos |
| `bun run lint` | Executa ESLint |
| `bun run typecheck` | Verifica tipos TypeScript |
| `bun run test:unit` | Executa testes unitários e de componentes no Vitest |
| `bun run build` | Verifica tipos e gera `frontend/dist/` com Vite |
| `bun run preview` | Serve uma prévia do build existente |
| `bun run test:integration` | Testa regras com Firestore emulado ativo |
| `bun run test:e2e` | Testa os fluxos com os serviços ativos; exige Chromium do Playwright |

Use `bun run build`: `bun build` chama o bundler próprio do Bun.
Os testes usam Vitest e Playwright; `bun test` não é o atalho utilizado neste projeto.

Para executar `bun run dev` diretamente com a configuração do script local:

```powershell
cd frontend
. ../scripts/local-env.ps1
Set-LocalFrontendEnvironment
bun run dev --host 127.0.0.1 --port 5173 --strictPort
```

Mantenha Firebase e C++ em outros terminais. Esse trecho carrega o Node portátil e define as variáveis de demonstração no terminal atual. O atalho equivalente, a partir da raiz, é `./scripts/dev-local.ps1 frontend`.
Não há `bun run dev` na raiz nem inicialização automática dos três serviços por ele.

Para validar a integração, prefira `./scripts/check-local.ps1 -Integration` na raiz, com os serviços ativos. O script configura os endereços e instala Chromium headless em `.tools/playwright`.

## Dependências e lockfiles

| Diretório | Manifesto | Lockfile | Conteúdo |
|---|---|---|---|
| `frontend/` | `package.json` | `bun.lock` | React, SDK Firebase, Vite e testes |
| `tools/` | `package.json` | `bun.lock` | Firebase CLI local |

Cada diretório tem sua própria instalação; não são workspaces de um manifesto na raiz. O setup instala ambos com `bun install --frozen-lockfile`.
A instalação congelada falha se manifesto e lockfile divergirem.

No diretório da dependência que será alterada:

```powershell
bun add nome-do-pacote
bun add --dev nome-da-ferramenta
bun remove nome-do-pacote
```

Revise e versione o `package.json` e o `bun.lock` correspondentes juntos.
Para gerar apenas o lockfile, execute `bun install --lockfile-only` nesse diretório.
O auxiliar `scripts/bootstrap-lock.sh` usa Docker e atualiza somente o lockfile do frontend. Não é necessário regenerá-lo antes de cada teste.

O frontend mantém o override `@grpc/grpc-js: 1.14.6` em seu manifesto. Ao alterar essa resolução, revise o lockfile e execute os testes afetados.
`bun pm ls --all` lista dependências e `bun audit` consulta vulnerabilidades; eles não integram a bateria atual.

## Atualizações de ferramentas

Ao atualizar Bun, alinhe `.bun-version`, `packageManager` nos manifestos de `frontend/` e `tools/`, `engines.bun` do frontend e as imagens Docker que copiam Bun.
Os workflows de publicação e o bootstrap leem `.bun-version`.
O Dependabot atual cobre `frontend/` e GitHub Actions; não cobre `tools/`.

Firebase CLI está fixado em `tools/package.json`, na imagem dos emuladores e nos workflows de publicação. Atualize esses locais juntos ao trocar sua versão.
Node, Ninja e Java portáteis têm URLs e checksums fixados em `scripts/setup-local.ps1`.

Referências: [instalação do Bun](https://bun.sh/docs/installation) e [instalação congelada](https://bun.sh/docs/pm/cli/install).
