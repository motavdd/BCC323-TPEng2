# Decisões técnicas

| Área | Configuração do projeto |
|---|---|
| Frontend | React 19, TypeScript e Vite 7 |
| Dependências | Bun 1.3.14 e lockfile `frontend/bun.lock` |
| Runtime das ferramentas | Node.js 22.12 ou superior |
| Comunicação com o banco | SDK cliente Firebase, sem API própria no fluxo atual |
| Autenticação | Firebase Authentication com e-mail e senha |
| Banco | Cloud Firestore com autorização por Security Rules |
| Hospedagem | Workflows para Firebase Hosting estático |
| Testes | Docker Compose, Vitest, Testing Library e Playwright |
| Integração contínua | GitHub Actions: `quality`, `integration` e `e2e` |
| Autenticação de implantação | Workflows com OIDC e Workload Identity Federation |
| Branches | `Develop`, `Staging` e `Master` |

## Ambientes

| Ambiente | Projeto Firebase | Uso |
|---|---|---|
| Local e testes | `demo-bcc323` | Authentication e Firestore emulados |
| Homologação | `spoa-eng2-staging` | Validação com serviços Firebase reais |
| Produção | `spoa-eng2-prod` | Serviços Firebase do ambiente de produção |

Os projetos de homologação e produção possuem aplicativos Web separados. As configurações locais ficam em arquivos `.env.*.local`, excluídos do versionamento. Os workflows recebem as configurações pelas variáveis dos GitHub Environments.

## Autorização

O usuário autenticado acessa apenas o próprio perfil e o próprio histórico de disciplinas concluídas. O catálogo permite leitura para usuários autenticados e bloqueia escrita pelo cliente. Os demais caminhos permanecem bloqueados.

O sistema não implementa permissões administrativas para coordenadores. A definição de perfis privilegiados, das regras do fórum e da modelagem completa dos módulos acadêmicos permanece fora do escopo implementado.

## Dependências

`package.json` e `bun.lock` são versionados juntos. Docker, CI e implantação utilizam `bun install --frozen-lockfile`. A versão do Bun é definida em `.bun-version`, no manifesto do frontend e nas imagens Docker.
