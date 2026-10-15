# Plano técnico final — BCC323-TPEng2

**Data da análise original:** 14/10/2026. **Origem registrada no material recebido:** consulta somente leitura às branches `Develop`, `Staging` e `Master` de https://github.com/motavdd/BCC323-TPEng2. As observações remotas abaixo são históricas e não foram reconfirmadas nesta migração. O estado desta cópia e as validações atuais estão em `RELATORIO_MIGRACAO_BUN.md`; o workflow C/C++ foi removido localmente.

## 1. Situação confirmada do repositório

- Branches existentes: `Develop`, `Staging`, `Master` (letras maiúsculas).
- Cada branch possui README com finalidade diferente. `Develop` já descreve GitFlow, mas comandos anteriores usam nomes minúsculos e o guia de execução contém placeholders. **Atenção:** merges de `Develop` para `Staging`/`Master` podem sobrescrever o README específico da branch de destino; conferir e restaurar o arquivo em cada PR de promoção.
- Diretórios `src/`, `test/`, `bin/` e `docs/` existem, mas estão vazios (apenas `.gitkeep`).
- Workflow `.github/workflows/blank.yml` existente é **C/C++ CI**, executa `make` e usa nomes de branch **minúsculos** (`master`, `staging`, `develop`). Não atende ao stack React/Firebase nem aos nomes reais das branches.
- Ruleset ativo `master-protection`: 1 aprovação, sem exclusão/force-push, status obrigatório com contexto `success`, bypass explícito de usuário.
- Ruleset ativo `staging-protection`: **0 aprovações**, sem exclusão/force-push, status obrigatório `success`, bypass explícito de usuário.
- **Não foi encontrado ruleset para `Develop`** na listagem consultada.
- A leitura do endpoint clássico de branch protection retornou 403 por permissão da integração; a auditoria acima se limita às rulesets acessíveis.
- Os READMEs atuais descrevem as funcionalidades e 21 Issues de cadastro, histórico, progresso, grade e fórum.

**Consequência:** os arquivos deste pacote são um *scaffold proposto*, não uma alteração já aplicada ou um build comprovadamente verde.

## 2. Arquitetura

```text
Navegador (React + TS + Vite)
    ├── Firebase Authentication: login e identidade
    ├── Cloud Firestore: perfis, histórico, catálogos e planos
    └── Firebase Hosting: SPA estática

Desenvolvimento / CI (Docker Compose)
    ├── frontend: lint + tsc + Vitest + build
    ├── firebase: Auth Emulator + Firestore Emulator
    ├── web: build Vite servido na rede interna
    └── playwright: Chromium para testes E2E

GitHub Actions
    PR → CI / Docker → revisão → merge
    Staging → CI reutilizável → aprovação/ambiente staging → Hosting homologação
    Master → CI reutilizável → aprovação/ambiente production → Hosting produção
```

**Backend:** não definido. Inicialmente, operações do aluno podem usar SDK cliente Firebase + regras Firestore. A administração do catálogo pelo coordenador, privilégios, importações e integrações exigirão uma decisão sobre backend confiável (Cloud Functions/Cloud Run ou API própria), ou claims customizadas emitidas por um ambiente privilegiado. Não conceder papel de coordenador pelo frontend.

## 3. Estrutura proposta

```text
BCC323-TPEng2/
├── .github/
│   ├── workflows/{ci.yml,deploy.yml,deploy-firestore.yml}
│   ├── dependabot.yml
│   └── pull_request_template.md
├── docker/firebase/Dockerfile
├── firebase/{firestore.rules,firestore.indexes.json}
├── frontend/
│   ├── src/{components,lib,utils,test}
│   ├── tests/integration/
│   ├── e2e/
│   ├── Dockerfile.test
│   ├── Dockerfile.e2e
│   ├── package.json
│   ├── bun.lock          # Versionar; instalação congelada no Docker/CI
│   ├── vite.config.ts
│   ├── vitest.integration.config.ts
│   └── playwright.config.ts
├── compose.dev.yml
├── compose.test.yml
├── firebase.json
├── docs/{PLANO_TECNICO,IMPLANTACAO,SEGURANCA,DECISOES}.md
└── README.md             # Versão proposta para Develop
```

**Nota de migração:** manter os diretórios vazios do repositório durante a introdução do frontend; não mover código inexistente. Em fase posterior, `frontend/src` poderá ser dividido em `features/auth`, `features/academic`, `features/schedule`, `features/forum`, `components`, `services`, `routes` e `types`.

## 4. Estratégia de testes

| Categoria | Ferramenta | Gatilho | Resultado |
|---|---|---|---|
| Lint | ESLint | PR e push Develop | Falha por problemas de estilo/qualidade |
| Tipagem | TypeScript `tsc` | PR e push Develop | Falha por incompatibilidade de tipos |
| Unitário/UI | Vitest + RTL | PR e push Develop | Regras de horários e componentes |
| Integração | Firebase Emulator + Rules Unit Testing | PR e promoção | Isolamento entre usuários e acesso ao catálogo |
| E2E | Playwright/Chromium | PR e promoção | Cadastro/logout no Auth Emulator |
| Build | Vite | PR e promoção | SPA gerada sem erros |

Todos os testes rodam em Docker. Os testes de navegador acessam `web:4173` e os emuladores `firebase:8080/9099` **de dentro da rede Docker**. No desenvolvimento local, o navegador no host acessa `localhost:5173`, e o frontend aponta para emuladores em `localhost`. Isso evita o erro clássico de resolver o hostname `firebase` no navegador do computador.

**Escopo dos exemplos:** o protótipo inclui testes reais de horários, renderização, regras Firestore e fluxo de cadastro. Os casos completos de pré-requisitos, matrícula, grade, fórum e privacidade devem ser implementados quando os módulos existirem.

## 5. CI/CD e regras de merge

- `ci.yml`: executa em PR para qualquer branch principal, push em `Develop` e pode ser chamado como workflow reutilizável.
- `deploy.yml`: executa em push para `Staging` ou `Master`, chama `ci.yml` novamente e só publica Hosting se **todos** os jobs passarem.
- `deploy-firestore.yml`: execução manual, com aprovação de ambiente e checkout da branch correspondente. Publica apenas **regras**, não índices.
- `production` deve exigir revisores no GitHub Environment. Não permitir autodeploy de PRs nem deploys em forks.
- Não usar `FIREBASE_TOKEN` legado; usar Workload Identity Federation com contas de serviço separadas por ambiente.
- Não usar `paths` no CI requerido: checks obrigatórios não devem ficar em estado pendente por filtro de arquivos.

## 6. Planejamento de execução vinculado às Issues

| Fase | Escopo | Issues relevantes | Entregável |
|---|---|---|---|
| 0 | Organização, Docker, CI, Firebase, README Develop | #4, #11, #12, #13, #15 | CI verde e PR de infraestrutura |
| 1 | Autenticação e cadastro | #1, #16–#21 | Fluxo aluno autenticado com perfil |
| 2 | Histórico, cursos e pré-requisitos | #2, #5 | Matriz e validação de disciplinas |
| 3 | Painel de progresso | #3 | Progresso acadêmico calculado |
| 4 | Simulador e agenda | #6, #7 | Grade sem choques de horários |
| 5 | Fórum, comentários e privacidade | #8–#10 | Colaboração com regras de acesso |
| 6 | Homologação, regressão, documentação e release | Todas | Versão aprovada em Master |

A distribuição por pessoa e datas de sprints deve ser alinhada com o cronograma acadêmico; não foi inferida a data final de entrega.

## 7. Checklist de aceite da infraestrutura

- [ ] `frontend/bun.lock` está versionado e `bun install --frozen-lockfile` passa.
- [ ] `docker compose -f compose.test.yml build` conclui sem erro.
- [ ] Lint, typecheck, unitários e build passam.
- [ ] Integração com regras Firestore passa sem projeto real.
- [ ] E2E no Chromium passa com Auth Emulator.
- [ ] GitHub Actions executa em PR para `Develop`.
- [ ] Rulesets exigem os nomes **reais** dos checks verdes.
- [ ] `Staging` e `Master` só recebem PRs revisados.
- [ ] Hosting de staging e produção usam **projetos Firebase distintos**.
- [ ] Workload Identity Federation testada em cada ambiente.
- [ ] Deploy de regras revisadas foi executado antes do uso real do Firestore.
- [ ] `Master` exige aprovação de Environment `production`.
- [ ] Documentação de rollback e logs está acessível.

## 8. Limitações desta entrega

O plano original foi validado apenas estaticamente. A migração para Bun é descrita em `docs/BUN.md`. A execução completa de integração e E2E depende de acesso ao Docker e aos emuladores; conclua o checklist acima antes de publicar.
