# Decisões pendentes e hipóteses explícitas

| Tema | Escolha provisória | Pendente para a equipe |
|---|---|---|
| Framework | React 19 + TypeScript + Vite 7 | Dependências fixadas em `frontend/bun.lock` |
| Gerenciador de pacotes | Bun 1.3.14 | Usar a mesma versão localmente, no Docker e no CI |
| Backend | Sem API própria no MVP | Definir como coordenadores terão permissões privilegiadas |
| Autenticação | Firebase Authentication (email/senha no protótipo) | Provedores, recuperação de senha, política de contas |
| Banco | Firestore | Coleções finais, índices, política de retenção e LGPD |
| Hosting | Firebase Hosting estático | Criar projetos `staging` e `production`, registrar apps Web |
| Testes | Docker + Vitest + RTL + Playwright | Metas de cobertura, casos reais por módulo |
| CI/CD | GitHub Actions + WIF | Habilitar WIF, IAM, GitHub Environments, checks reais |
| Regras Firestore | Perfis do próprio aluno; catálogo só leitura; demais acessos negados | Políticas de coordenação, fórum e privacidade |
| Branches | `Develop`, `Staging`, `Master` | Revisar rulesets no remoto; C/C++ CI legado removido desta cópia |
| Custos | Começar com menor plano que atenda às necessidades | Verificar limites atuais, WIF e requisitos de cobrança no GCP |
| Domínio | Domínios padrão Firebase | Registrar domínios autorizados no Firebase Auth |

**Não é necessário resolver o backend para implementar CI, testes de regras e frontend.** Entretanto, não lançar funcionalidade administrativa com privilégios gerenciados apenas pelo cliente.

**O lockfile do projeto é `frontend/bun.lock`.** Versione-o junto com `package.json`. Docker e Actions usam `bun install --frozen-lockfile`; para alterar dependências, use `bun add`, `bun remove` ou `bun install` e revise o diff. Node 22 continua como runtime das ferramentas; Bun gerencia a instalação e executa os scripts.
