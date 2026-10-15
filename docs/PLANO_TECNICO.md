# Arquitetura e plano técnico — BCC323-TPEng2

## 1. Estrutura do sistema

O sistema de planejamento acadêmico utiliza React, TypeScript e Vite no frontend. O navegador acessa Firebase Authentication e Cloud Firestore diretamente pelo SDK cliente. As regras do Firestore controlam o acesso aos documentos. O fluxo implementado não utiliza API própria.

O repositório é [BCC323-TPEng2](https://github.com/motavdd/BCC323-TPEng2). As branches principais são `Develop`, `Staging` e `Master`.

## 2. Arquitetura

```text
Navegador (React + TypeScript + Vite)
    ├── Firebase Authentication: cadastro, login e sessão
    └── Cloud Firestore: perfis e regras de acesso

Desenvolvimento e testes (Docker Compose)
    ├── frontend: lint, tipos, testes unitários e build
    ├── firebase: emuladores de Authentication e Firestore
    ├── web: aplicação servida na rede interna
    └── playwright: teste de cadastro e saída no Chromium

GitHub Actions
    ├── PR → quality, integration e e2e
    ├── Staging → CI → Environment staging → Firebase Hosting
    └── Master → CI → Environment production → Firebase Hosting
```

Os fluxos de publicação dependem dos GitHub Environments, das permissões IAM e da configuração de Workload Identity Federation. A aprovação de produção depende da proteção configurada no Environment `production`.

## 3. Organização do repositório

| Caminho | Conteúdo |
|---|---|
| `frontend/src/` | Interface, inicialização do Firebase e validação de horários |
| `frontend/tests/integration/` | Testes das regras Firestore |
| `frontend/e2e/` | Testes de navegador |
| `frontend/bun.lock` | Versões resolvidas das dependências |
| `firebase/` | Regras e configuração de índices |
| `docker/firebase/` | Imagem dos emuladores |
| `compose.dev.yml` | Ambiente de desenvolvimento |
| `compose.test.yml` | Ambiente isolado de testes |
| `.github/workflows/` | CI, publicação de Hosting e publicação manual de regras |
| `scripts/` | Instalação e execução da bateria de testes |
| `docs/` | Documentação técnica |
| `implementacao.md` | Funcionalidades, execução e validação |

## 4. Testes

| Verificação | Escopo |
|---|---|
| ESLint | Análise estática |
| TypeScript | Verificação de tipos |
| Vitest e Testing Library | Conflitos de horário e componente de boas-vindas |
| Integração | Autorização de acesso a perfis e catálogo |
| Playwright | Cadastro e saída com emuladores |
| Build | Geração da aplicação com Vite |

A bateria completa utiliza Docker e o projeto fictício `demo-bcc323`. O navegador dos testes acessa `web:4173` e os emuladores em `firebase:8080` e `firebase:9099`. No desenvolvimento pelo navegador do computador, os serviços são acessados por `localhost`.

As verificações locais foram aprovadas em 14/10/2026. O CI do PR #24 também foi concluído com sucesso. Os resultados dos testes não equivalem à validação da publicação nos ambientes remotos.

## 5. Integração e publicação

- `ci.yml`: PRs para `Develop`, `Staging` e `Master`, pushes em `Develop`, execução manual e chamada por outros workflows.
- `deploy.yml`: pushes em `Staging` e `Master`; executa o CI antes do deploy de Hosting.
- `deploy-firestore.yml`: execução manual com correspondência obrigatória entre branch e ambiente; publica apenas regras.
- O deploy utiliza OIDC/WIF e variáveis dos GitHub Environments.
- A promoção segue `Develop → Staging → Master`, com revisão de PR e preservação do README da branch de destino.

## 6. Etapas de desenvolvimento

| Fase | Escopo | Issues | Entregável |
|---|---|---|---|
| 0 | Docker, CI e configuração Firebase | #4, #11, #12, #13, #15 | Infraestrutura de desenvolvimento e testes |
| 1 | Autenticação e cadastro | #1, #16–#21 | Sessão autenticada com perfil |
| 2 | Histórico, cursos e pré-requisitos | #2, #5 | Matriz e validação de disciplinas |
| 3 | Painel de progresso | #3 | Cálculo de progresso acadêmico |
| 4 | Simulador e agenda | #6, #7 | Grade com validação de horários |
| 5 | Fórum e comentários | #8–#10 | Colaboração com controle de acesso |
| 6 | Homologação e entrega | Todas | Versão validada em `Master` |

## 7. Escopo implementado

A interface oferece cadastro, login e saída. Os perfis são armazenados no Firestore. A verificação de conflitos de horário possui testes, mas ainda não está integrada a uma tela de montagem de grade.

Histórico, matriz curricular, pré-requisitos, painel de progresso, grade persistida, fórum e administração do catálogo ainda não possuem interface. A descrição detalhada está em [Implementação](../implementacao.md).
