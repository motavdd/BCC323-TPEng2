# Arquitetura e plano técnico — BCC323-TPEng2

## 1. Estrutura do sistema

O sistema de planejamento acadêmico utiliza React, TypeScript, Vite e Tailwind CSS no frontend. O navegador acessa Firebase Authentication e Cloud Firestore diretamente pelo SDK cliente. As regras do Firestore controlam o acesso aos documentos. A preparação inclui um serviço C++17 com Crow e CMake, acessível pelo proxy `/api` do Vite. A API oferece somente verificação de funcionamento; o fluxo de cadastro e perfis ainda não a utiliza.

Os comandos e limites desta preparação estão em [AMBIENTE.md](AMBIENTE.md).
O fluxo principal da equipe é [nativo no Windows](DESENVOLVIMENTO_LOCAL.md),
com Firebase emulado localmente; o Docker padroniza a bateria integrada no CI.

O repositório é [BCC323-TPEng2](https://github.com/motavdd/BCC323-TPEng2). As branches principais são `Develop`, `Staging` e `Master`.

## 2. Arquitetura

```text
Navegador (React + TypeScript + Vite)
    ├── Firebase Authentication: cadastro, login e sessão
    ├── Cloud Firestore: perfis e regras de acesso
    └── /api via proxy Vite → C++/Crow: verificação de funcionamento

Desenvolvimento principal (Windows nativo)
    ├── Bun: dependências e scripts de React/Vite
    ├── CMake + Ninja + GCC: compilação do C++
    └── Firebase CLI + Node + Java: emuladores e painel local

CI e alternativa local (Docker Compose)
    ├── backend-test: compilação C++ e testes HTTP via CTest
    ├── backend: serviço C++ para os testes integrados
    ├── frontend: lint, tipos, testes unitários e build
    ├── firebase: emuladores de Authentication e Firestore
    ├── web: aplicação servida na rede interna
    └── playwright: cadastro/saída e comunicação frontend → C++

GitHub Actions
    ├── PR → backend, quality, integration e e2e
    ├── Staging → CI → Environment staging → Firebase Hosting
    └── Master → CI → Environment production → Firebase Hosting
```

Os fluxos de publicação dependem dos GitHub Environments, das permissões IAM e da configuração de Workload Identity Federation. A aprovação de produção depende da proteção configurada no Environment `production`.

## 3. Organização do repositório

| Caminho | Conteúdo |
|---|---|
| `frontend/src/` | Interface, inicialização do Firebase e validação de horários |
| `backend/` | API C++/Crow, CMake, Dockerfile e testes HTTP |
| `frontend/tests/integration/` | Testes das regras Firestore |
| `frontend/e2e/` | Testes de navegador |
| `frontend/bun.lock` | Versões resolvidas das dependências |
| `tools/bun.lock` | Versões resolvidas das ferramentas Firebase locais |
| `firebase/` | Regras e configuração de índices |
| `docker/firebase/` | Imagem dos emuladores |
| `compose.dev.yml` | Alternativa de desenvolvimento em Docker |
| `compose.test.yml` | Ambiente isolado de testes |
| `.github/workflows/` | CI, publicação de Hosting e publicação manual de regras |
| `scripts/` | Instalação e execução da bateria de testes |
| `tools/` | Firebase CLI local e lockfile das ferramentas |
| `firebase.local.json` | Emuladores nativos e painel, vinculados a loopback |
| `docs/` | Documentação técnica |
| `implementacao.md` | Funcionalidades, execução e validação |

## 4. Testes

| Verificação | Escopo |
|---|---|
| ESLint | Análise estática |
| CMake e CTest | Compilação C++ e contrato HTTP do serviço |
| TypeScript | Verificação de tipos |
| Vitest e Testing Library | Conflitos de horário e componente de boas-vindas |
| Integração | Autorização de acesso a perfis e catálogo |
| Playwright | Cadastro e saída com emuladores; encaminhamento frontend → C++ |
| Build | Geração da aplicação com Vite |

A verificação rápida nativa usa `check-local.ps1`; com `-Integration` inclui regras
Firestore e navegador, com os três serviços ativos. A bateria do CI utiliza Docker
e o projeto fictício `demo-bcc323`. No Docker, o navegador dos testes acessa `web:4173`
e os emuladores em `firebase:8080` e `firebase:9099`. O ambiente nativo usa loopback,
conforme [Ambiente](AMBIENTE.md).

As verificações da base anterior e o CI do PR #24 foram registrados como aprovados
em 14/10/2026, antes de C++ e Tailwind. A validação local de 15/10/2026 está em
[Desenvolvimento local](DESENVOLVIMENTO_LOCAL.md#validação-desta-preparação).
O workflow atualizado ainda precisa executar no GitHub após o envio do PR.
Os resultados locais não comprovam publicação nos ambientes remotos.

## 5. Integração e publicação

- `ci.yml`: PRs para `Develop`, `Staging` e `Master`, pushes em `Develop`, execução manual e chamada por outros workflows.
- `deploy.yml`: pushes em `Staging` e `Master`; executa o CI antes do deploy de Hosting.
- `deploy-firestore.yml`: execução manual com correspondência obrigatória entre branch e ambiente; publica apenas regras.
- O deploy utiliza OIDC/WIF e variáveis dos GitHub Environments.
- A promoção segue `Develop → Staging → Master`, com revisão de PR e preservação do README da branch de destino.

## 6. Etapas de desenvolvimento

| Fase | Escopo | Issues | Entregável |
|---|---|---|---|
| 0 | Ambiente nativo, Docker no CI e Firebase local | #4, #11, #12, #13, #15 | Infraestrutura de desenvolvimento e testes |
| 1 | Autenticação e cadastro | #1, #16–#21 | Sessão autenticada com perfil |
| 2 | Histórico, cursos e pré-requisitos | #2, #5 | Matriz e validação de disciplinas |
| 3 | Painel de progresso | #3 | Cálculo de progresso acadêmico |
| 4 | Simulador e agenda | #6, #7 | Grade com validação de horários |
| 5 | Fórum e comentários | #8–#10 | Colaboração com controle de acesso |
| 6 | Homologação e entrega | Todas | Versão validada em `Master` |

## 7. Escopo implementado

A interface oferece cadastro, login e saída. Os perfis são armazenados no Firestore. A verificação de conflitos de horário possui testes, mas ainda não está integrada a uma tela de montagem de grade.

Histórico, matriz curricular, pré-requisitos, painel de progresso, grade persistida, fórum e administração do catálogo ainda não possuem interface. A descrição detalhada está em [Implementação](../implementacao.md).
