# BCC323-TPEng2 — Guia de Desenvolvimento (`Develop`)

Planejamento acadêmico com React, TypeScript, Vite, Tailwind CSS, C++/Crow e Firebase.
**Bun 1.3.14** instala as dependências e executa os scripts. Os lockfiles ficam em
`frontend/bun.lock` e `tools/bun.lock`, cada um junto ao seu manifesto.

A documentação está organizada no [índice de guias](docs/README.md). O backend
oferece inicialmente `/api/health`; cadastro e perfis continuam usando Firebase diretamente.

- [Repositório](https://github.com/motavdd/BCC323-TPEng2)
- [Quadro Kanban](https://github.com/users/motavdd/projects/7)
- [Issues](https://github.com/motavdd/BCC323-TPEng2/issues)

## Branches e responsabilidades

| Branch | Finalidade | Origem dos PRs |
|---|---|---|
| `Develop` | Integração do desenvolvimento | Branches de trabalho |
| `Staging` | Homologação | `Develop` |
| `Master` | Versão estável | `Staging` |

Os nomes das branches diferenciam maiúsculas e minúsculas. O fluxo de promoção é `Develop → Staging → Master`. A origem de cada PR de promoção é conferida na revisão; os Rulesets não garantem essa sequência automaticamente.

Este README é o guia de desenvolvimento. Nas promoções, o PR deve preservar o README específico da branch de destino.

## Preparar o repositório

```sh
git clone https://github.com/motavdd/BCC323-TPEng2.git
cd BCC323-TPEng2
git switch Develop
git pull --ff-only origin Develop
```

## Desenvolvimento local sem Docker

O fluxo principal usa React/Vite, C++ e Firebase Emulator Suite diretamente no
Windows x64. Instale Git, Bun, CMake e GCC/MinGW do MSYS2 UCRT64.
O setup fornece Node, Ninja e Java portáteis. Versões e detalhes:
[Desenvolvimento local](docs/DESENVOLVIMENTO_LOCAL.md).

Preparar na raiz; repita após mudanças nas dependências:

```powershell
./scripts/setup-local.ps1
```

Iniciar cada serviço em um terminal separado:

```powershell
# Terminal 1
./scripts/dev-local.ps1 firebase
# Terminal 2
./scripts/dev-local.ps1 backend
# Terminal 3
./scripts/dev-local.ps1 frontend
```

O comando do frontend configura o Firebase local e chama **`bun run dev`** dentro
de `frontend/`. A raiz não possui `package.json` nem um `bun run dev` único.
Para executar comandos Bun diretamente, consulte [Bun](docs/BUN.md).

Abra a [aplicação](http://127.0.0.1:5173) e o [painel Firebase local](http://127.0.0.1:4000).
Os emuladores salvam dados fictícios em `.firebase-data/` ao encerrar normalmente.
Se o Compose anterior estiver ativo, pare-o antes para liberar as portas.

Antes de enviar sua branch, execute `./scripts/check-local.ps1` e teste a
funcionalidade na aplicação. A bateria completa em Docker roda no PR para `Develop`.
Para testar também regras Firebase e navegador sem Docker, mantenha os três
serviços ativos e execute `./scripts/check-local.ps1 -Integration`.
Os testes de regras limpam o Firestore emulado do projeto de demonstração.

## Alternativa com Docker

Requer Docker Desktop ou Docker Engine com Compose V2:

```sh
docker compose -f compose.dev.yml up --build
```

Abra a [aplicação em Docker](http://localhost:5173). Auth e Firestore usam o projeto de demonstração `demo-bcc323`.

Confira a [comunicação com C++](http://localhost:5173/api/health).
O Docker fornece compilador, CMake e demais ferramentas do backend.

## Alternativa híbrida: frontend local e serviços em Docker

Instale Bun 1.3.14 e Node 22.12+ (runtime das ferramentas). Na raiz, inicie o backend e os emuladores:

```sh
docker compose -f compose.dev.yml up --build backend firebase
```

Em outro terminal:

```powershell
cd frontend
if (!(Test-Path .env.local)) { Copy-Item .env.example .env.local }
bun install --frozen-lockfile
bun run dev
```

Confira `VITE_USE_EMULATORS=true` e o projeto `demo-bcc323` em `.env.local`.
Para quem já concluiu o setup nativo, `./scripts/dev-local.ps1 frontend` na raiz
é a alternativa que configura essas variáveis e utiliza o Node portátil.

Os emuladores usam dados fictícios. O ambiente de desenvolvimento não configura persistência desses dados entre recriações dos containers.

## Validar

Na raiz:

| Verificação | Comando | Requisitos |
|---|---|---|
| Rápida | `./scripts/check-local.ps1` | Setup nativo |
| Integração nativa | `./scripts/check-local.ps1 -Integration` | Três serviços locais ativos |
| Bateria Docker | `./scripts/test-all.ps1` | Docker ativo |
| Bateria Docker em Linux/WSL | `bash scripts/test-all.sh` | Docker e Bash |

A bateria Docker inclui testes HTTP do C++, lint, tipos, testes unitários, build,
regras Firestore e fluxos de cadastro/saída e acesso à API. O CI executa essa bateria
em jobs separados no PR; Docker é opcional para desenvolver localmente.

Para qualidade local, em `frontend/`: `bun run lint`, `bun run typecheck`, `bun run test:unit` e `bun run build`. Use `bun run build` para executar o build do projeto.

## Fluxo de contribuição

1. Selecione uma Issue e confira o responsável, os critérios de aceite e o status no Kanban.
2. Atualize `Develop` e crie uma branch de trabalho:

   ```sh
   git switch Develop
   git pull --ff-only origin Develop
   git switch -c feature/16-formulario-cadastro
   ```

3. Implemente a alteração e atualize a documentação e os testes correspondentes.
4. Execute `./scripts/check-local.ps1` e valide a funcionalidade com os emuladores locais, utilizando dados fictícios.
5. Faça o commit e envie a branch de trabalho ao GitHub.
6. Abra um PR para `Develop`, vincule a Issue e solicite a revisão de um colega.
7. Aguarde a bateria completa em Docker no CI e as aprovações exigidas antes do merge. Atualize o Kanban após a integração.

As branches de trabalho utilizam o formato `<tipo>/<numero-da-issue>-<resumo>`, com prefixos `feature/`, `fix/`, `docs/`, `test/`, `refactor/` ou `chore/`.

## Critérios de aprovação do PR

- [ ] A alteração atende aos critérios da Issue vinculada.
- [ ] Os checks `backend`, `quality`, `integration` e `e2e` passaram.
- [ ] Os testes verificam os comportamentos afetados.
- [ ] A documentação corresponde à implementação.
- [ ] O diff não contém senhas, tokens, chaves de conta de serviço ou dados pessoais de usuários.
- [ ] O PR possui a aprovação exigida pelo Ruleset da branch.

Os critérios acima descrevem a política do projeto. Configure os checks e as
aprovações como obrigatórios no GitHub; os workflows não criam essa proteção.

## Estrutura do repositório

| Caminho | Responsabilidade |
|---|---|
| `frontend/src/` | Interface React, integração Firebase e lógica de horários |
| `backend/` | Serviço C++/Crow, CMake, Dockerfile e testes HTTP |
| `tools/` | Firebase CLI local e lockfile das ferramentas |
| `frontend/tests/integration/` | Testes das regras Firestore |
| `frontend/e2e/` | Testes de navegador com Playwright |
| `firebase/` | Regras de acesso e configuração de índices |
| `firebase.local.json` | Emuladores nativos e painel em loopback |
| `docker/firebase/` | Imagem dos emuladores |
| `compose.dev.yml` | Alternativa de desenvolvimento em Docker |
| `compose.test.yml` | Ambiente isolado de testes |
| `.github/workflows/` | Integração contínua e publicação |
| `scripts/` | Scripts de instalação e testes |
| `docs/` | Documentação técnica |

## Ambientes e publicação

| Ambiente | Projeto Firebase | Finalidade |
|---|---|---|
| Local e CI | `demo-bcc323` | Testes com emuladores e dados fictícios |
| Homologação | `spoa-eng2-staging` | Validação com Firebase real |
| Produção | `spoa-eng2-prod` | Ambiente de produção |

A publicação no Firebase Hosting é definida pelos workflows de `Staging` e `Master` e depende dos GitHub Environments e da autenticação com o Google Cloud. O deploy de Hosting executa o CI antes da publicação. A aprovação de produção é configurada no Environment `production`. As regras Firestore possuem workflow manual separado.

Os workflows atuais não publicam a API C++. O proxy `/api` funciona no Vite local e
na prévia de testes; a hospedagem da API e seu encaminhamento remoto estão pendentes.

Os scripts nativos definem a configuração de demonstração no processo, sem
sobrescrever `.env.*.local`. Esses arquivos não são versionados. No GitHub Actions,
as configurações Web do Firebase são fornecidas pelas variáveis de cada Environment.

## Documentação

- [Índice de documentação](docs/README.md)
- [Desenvolvimento local sem Docker](docs/DESENVOLVIMENTO_LOCAL.md)
- [Ambiente React + C++ + Firebase](docs/AMBIENTE.md)
- [Desenvolvimento, dependências e Bun](docs/BUN.md)
- [Implantação e GitHub Actions](docs/IMPLANTACAO.md)
- [Decisões técnicas](docs/DECISOES.md)
- [Arquitetura e plano técnico](docs/PLANO_TECNICO.md)
- [Implementação atual](implementacao.md)
