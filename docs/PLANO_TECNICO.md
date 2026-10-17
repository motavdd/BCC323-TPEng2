# Arquitetura e plano técnico — BCC323-TPEng2

Este documento concentra a arquitetura, as tecnologias e a sequência de desenvolvimento.
As funcionalidades entregues e os resultados de validação estão em
[Implementação atual](../implementacao.md). O roteiro de trabalho está no
[guia da equipe](CONTRIBUICAO.md).

## 1. Tecnologias e responsabilidades


| Tecnologia | Aplicação no projeto |
|---|---|
| React 19 | Construção da interface e controle do estado dos componentes |
| TypeScript | Tipagem do código e verificação estática |
| Vite 7 | Servidor de desenvolvimento e geração da aplicação para publicação |
| Tailwind CSS 4 | Base de estilos integrada ao Vite; as telas atuais ainda usam estilos inline |
| Bun 1.3.14 | Instalação de dependências e execução dos scripts |
| Node.js | Runtime das ferramentas; 22.23.3 portátil no fluxo nativo e Node 22 no CI |
| C++17 e Crow 1.3.0 | Serviço HTTP inicial para futuras funcionalidades acadêmicas |
| CMake, Ninja e GCC | Compilação do backend nativo no Windows |
| Java 21 | Execução do emulador Firestore |
| Firebase Authentication | Cadastro e autenticação por e-mail e senha |
| Cloud Firestore | Armazenamento dos perfis dos usuários |
| Firebase Emulator Suite | Execução local de Authentication e Firestore |
| Docker Compose | Bateria integrada no CI e alternativa de execução local |
| ESLint, Vitest e Testing Library | Análise estática e testes unitários e de componentes |
| Playwright | Testes de fluxo no navegador |
| GitHub Actions | Workflows de integração contínua e implantação |

As dependências do frontend estão em `frontend/bun.lock`; o Firebase CLI 15.0.0
local é definido por `tools/package.json` e `tools/bun.lock`. Os manifestos e os
lockfiles correspondentes são versionados juntos. Bun instala ambos; não há
manifesto de pacotes na raiz.

O frontend acessa Authentication e Firestore pelo SDK cliente. A autorização dos
documentos é feita pelas Security Rules; o C++ ainda não participa desse fluxo.
A API possui somente a rota pública de saúde. Rotas privadas exigirão validação
de tokens no servidor; não há permissões administrativas para coordenadores.

O backend separa inicialização (`main.cpp`), configuração (`config.*`) e rotas
(`routes.*`). O contrato HTTP e a configuração estão no
[guia do backend](../backend/README.md). Os detalhes de instalação e atualização
de dependências ficam em [Bun](BUN.md).

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

O desenvolvimento principal é nativo no Windows; Docker padroniza a bateria
integrada do CI e é uma alternativa local. Endereços, persistência e diferenças
entre os modos estão em [Ambiente](AMBIENTE.md).

O proxy Vite não acompanha o build estático. Os workflows publicam o frontend no
Firebase Hosting; a hospedagem do C++ e o encaminhamento remoto de `/api` estão
pendentes. A implantação usa OIDC/WIF e GitHub Environments, com configurações
separadas para homologação e produção. Projetos, variáveis, revisão de produção
e proteção de branches estão descritos em [Implantação](IMPLANTACAO.md).

## 3. Organização do repositório


| Caminho | Conteúdo |
|---|---|
| `frontend/src/` | Interface, inicialização do Firebase e validação de horários |
| `backend/` | API C++/Crow, CMake, Dockerfile e testes HTTP |
| `backend/include/` | Cabeçalhos próprios da API |
| `backend/src/config.cpp` | Configuração de endereço e porta do processo |
| `backend/src/routes.cpp` | Registro das rotas, saúde e erros HTTP em JSON |
| `backend/build/native/bin/` | Executável nativo gerado, ignorado pelo Git |
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
| `implementacao.md` | Funcionalidades, limitações e histórico de validação |

## 4. Etapas de desenvolvimento


| Fase | Escopo | Issues | Entregável |
|---|---|---|---|
| 0 | Ambiente nativo, Docker no CI e Firebase local | #4, #11, #12, #13, #15 | Infraestrutura de desenvolvimento e testes |
| 1 | Autenticação e cadastro | #1, #16–#21 | Sessão autenticada com perfil |
| 2 | Histórico, cursos e pré-requisitos | #2, #5 | Matriz e validação de disciplinas |
| 3 | Painel de progresso | #3 | Cálculo de progresso acadêmico |
| 4 | Simulador e agenda | #6, #7 | Grade com validação de horários |
| 5 | Fórum e comentários | #8–#10 | Colaboração com controle de acesso |
| 6 | Homologação e entrega | Todas | Versão validada em `Master` |

As fases representam o planejamento, não a conclusão das Issues. Consulte a
[implementação atual](../implementacao.md) e o
[quadro da equipe](https://github.com/users/motavdd/projects/7) antes de iniciar uma tarefa.

## 5. Guias de execução e validação

- [Desenvolvimento local](DESENVOLVIMENTO_LOCAL.md): setup, serviços e verificações nativas.
- [Contribuição](CONTRIBUICAO.md#4-validar-a-alteração): testes por tipo de mudança.
- [Backend](../backend/README.md#testes): testes do processo HTTP.
- [Implantação](IMPLANTACAO.md): CI, bateria Docker e publicação.
- [Implementação atual](../implementacao.md): cobertura e histórico das validações.
