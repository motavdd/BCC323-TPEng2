# BCC323-TPEng2 — Guia de Desenvolvimento (`Develop`)

Protótipo de planejamento acadêmico com React, TypeScript, Vite e Firebase. O gerenciador de pacotes é **Bun 1.3.14**; o lockfile do frontend é `frontend/bun.lock`.

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

## Executar com Docker

Requer Docker Desktop ou Docker Engine com Compose V2:

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

Copie o arquivo de exemplo apenas se `.env.local` ainda não existir. No PowerShell, use `Copy-Item .env.example .env.local`.

Os emuladores usam dados fictícios. O ambiente de desenvolvimento não configura persistência desses dados entre recriações dos containers.

## Validar

Na raiz, execute `bash scripts/test-all.sh` ou, no PowerShell, `./scripts/test-all.ps1`. A bateria inclui lint, TypeScript, testes unitários, build, regras Firestore e cadastro/logout no Chromium com emuladores.

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
4. Execute a bateria de testes com os emuladores, utilizando dados fictícios.
5. Faça o commit e envie a branch de trabalho ao GitHub.
6. Abra um PR para `Develop`, vincule a Issue e solicite a revisão de um colega.
7. Aguarde os checks e as aprovações exigidas antes do merge. Atualize o Kanban após a integração.

As branches de trabalho utilizam o formato `<tipo>/<numero-da-issue>-<resumo>`, com prefixos `feature/`, `fix/`, `docs/`, `test/`, `refactor/` ou `chore/`.

## Critérios de aprovação do PR

- [ ] A alteração atende aos critérios da Issue vinculada.
- [ ] Os checks `quality`, `integration` e `e2e` passaram.
- [ ] Os testes verificam os comportamentos afetados.
- [ ] A documentação corresponde à implementação.
- [ ] O diff não contém senhas, tokens, chaves de conta de serviço ou dados pessoais de usuários.
- [ ] O PR possui a aprovação exigida pelo Ruleset da branch.

## Estrutura do repositório

| Caminho | Responsabilidade |
|---|---|
| `frontend/src/` | Interface React, integração Firebase e lógica de horários |
| `frontend/tests/integration/` | Testes das regras Firestore |
| `frontend/e2e/` | Testes de navegador com Playwright |
| `firebase/` | Regras de acesso e configuração de índices |
| `docker/firebase/` | Imagem dos emuladores |
| `compose.dev.yml` | Ambiente de desenvolvimento |
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

As configurações locais `.env.*.local` não são versionadas. No GitHub Actions, as configurações Web do Firebase são fornecidas pelas variáveis de cada Environment.

## Documentação

- [Desenvolvimento, dependências e Bun](docs/BUN.md)
- [Implantação e GitHub Actions](docs/IMPLANTACAO.md)
- [Decisões técnicas](docs/DECISOES.md)
- [Implementação](implementacao.md)
