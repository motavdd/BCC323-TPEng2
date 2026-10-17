# Decisões técnicas

| Área | Configuração do projeto |
|---|---|
| Frontend | React 19, TypeScript e Vite 7 |
| Estilos | Tailwind CSS 4, integrado pelo plugin oficial do Vite |
| Serviço de backend | C++17, Crow 1.3.0 e CMake; rota inicial `/api/health` |
| Dependências | Bun 1.3.14; lockfiles separados em `frontend/bun.lock` e `tools/bun.lock` |
| Runtime das ferramentas | Node 22.23.3 portátil no fluxo nativo; Node 22 nas imagens e no deploy |
| Comunicação com o banco | SDK cliente Firebase; o serviço C++ ainda não acessa o banco |
| Autenticação | Firebase Authentication com e-mail e senha |
| Banco | Cloud Firestore com autorização por Security Rules |
| Hospedagem | Workflows para Firebase Hosting estático |
| Testes | Vitest, Testing Library e Playwright; CTest para o backend; Docker no CI |
| Desenvolvimento local | React/Bun/Vite, C++/CMake e Firebase Emulator Suite nativos; scripts PowerShell no Windows |
| Integração contínua | GitHub Actions: `backend`, `quality`, `integration` e `e2e` |
| Autenticação de implantação | Workflows com OIDC e Workload Identity Federation |
| Branches | `Develop`, `Staging` e `Master` |

## Ambientes

O fluxo principal de desenvolvimento e a validação antes do PR estão em
[DESENVOLVIMENTO_LOCAL.md](DESENVOLVIMENTO_LOCAL.md). Docker é usado pela bateria
integrada no CI e permanece opcional no computador do desenvolvedor.

A preparação React + C++ + Firebase e os comandos para a equipe estão em
[AMBIENTE.md](AMBIENTE.md). A API possui somente uma rota pública de saúde.
Sua autenticação e publicação remota ainda não foram implementadas.

| Ambiente | Projeto Firebase | Uso |
|---|---|---|
| Local e testes | `demo-bcc323` | Authentication e Firestore emulados |
| Homologação | `spoa-eng2-staging` | Validação com serviços Firebase reais |
| Produção | `spoa-eng2-prod` | Serviços Firebase do ambiente de produção |

Os projetos de homologação e produção usam aplicativos Web separados. Os workflows
recebem configurações pelas variáveis dos GitHub Environments. Os scripts nativos
definem as variáveis do projeto fictício no processo, sem sobrescrever arquivos
`.env.*.local`. Esses arquivos permanecem disponíveis para execução manual e não
são versionados.

## Autorização

O usuário autenticado acessa apenas o próprio perfil e o próprio histórico de disciplinas concluídas. O catálogo permite leitura para usuários autenticados e bloqueia escrita pelo cliente. Os demais caminhos permanecem bloqueados.

O sistema não implementa permissões administrativas para coordenadores. A definição de perfis privilegiados, das regras do fórum e da modelagem completa dos módulos acadêmicos permanece fora do escopo implementado.

## Dependências

Cada `package.json` é versionado junto ao `bun.lock` de seu diretório. Setup nativo,
Docker, CI e implantação utilizam `bun install --frozen-lockfile` nas instalações
correspondentes. A versão do Bun é definida em `.bun-version`, nos dois manifestos
e nas imagens que incluem Bun. Não há manifesto nem workspace Bun na raiz.

`tools/` fornece o Firebase CLI para o desenvolvimento nativo. As imagens e os
workflows de publicação instalam sua própria cópia da CLI na mesma versão fixada.
