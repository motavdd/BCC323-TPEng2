# Implementação — Planejamento Acadêmico

**Disciplina:** BCC323 — Trabalho Prático de Engenharia de Software II  
**Data:** 14/10/2026

## 1. Objetivo e escopo

Este documento descreve a implementação atual do protótipo de planejamento acadêmico, sua organização, as funcionalidades disponíveis e os procedimentos de execução e validação.

A versão atual estabelece a base técnica do sistema: interface web, autenticação, armazenamento de perfis, regras de acesso ao banco e infraestrutura para testes. As funcionalidades completas de planejamento acadêmico ainda dependem de desenvolvimento.

## 2. Tecnologias utilizadas

| Tecnologia | Aplicação no projeto |
|---|---|
| React 19 | Construção da interface e controle do estado dos componentes |
| TypeScript | Tipagem do código e verificação estática |
| Vite 7 | Servidor de desenvolvimento e geração da aplicação para publicação |
| Bun 1.3.14 | Instalação de dependências e execução dos scripts |
| Node.js 22.12 ou superior | Runtime das ferramentas do projeto |
| Firebase Authentication | Cadastro e autenticação por e-mail e senha |
| Cloud Firestore | Armazenamento dos perfis dos usuários |
| Firebase Emulator Suite | Execução local de Authentication e Firestore |
| Docker Compose | Organização dos ambientes de desenvolvimento e testes |
| ESLint, Vitest e Testing Library | Análise estática e testes unitários e de componentes |
| Playwright | Testes de fluxo no navegador |
| GitHub Actions | Workflows de integração contínua e implantação |

As versões resolvidas das dependências do frontend estão registradas em `frontend/bun.lock`.

## 3. Arquitetura e organização

A aplicação é uma SPA executada no navegador. O frontend se comunica diretamente com o Firebase por meio do SDK cliente. As permissões de acesso aos documentos são verificadas pelas regras do Firestore. O fluxo atual não utiliza uma API própria no diretório `backend/`.

No ambiente local, o navegador acessa o Vite em `http://localhost:5173`, o emulador de autenticação na porta `9099` e o emulador do Firestore na porta `8080`.

| Arquivo ou diretório | Responsabilidade |
|---|---|
| `frontend/src/App.tsx` | Formulário, cadastro, login, estado da sessão e saída |
| `frontend/src/components/Welcome.tsx` | Exibição do e-mail da sessão autenticada |
| `frontend/src/lib/firebase.ts` | Inicialização do Firebase e conexão aos emuladores |
| `frontend/src/utils/schedule.ts` | Verificação de conflitos entre horários |
| `frontend/tests/integration/` | Testes das regras do Firestore |
| `frontend/e2e/` | Testes de navegador |
| `firebase/firestore.rules` | Controle de acesso e validação dos documentos |
| `firebase.json` | Configuração de emuladores, regras e Hosting |
| `compose.dev.yml` | Serviços usados no desenvolvimento local |
| `compose.test.yml` | Serviços usados nos testes automatizados |
| `.github/workflows/` | Workflows de CI e implantação |
| `scripts/` | Scripts auxiliares para instalação e validação |
| `docs/` | Documentação técnica complementar |

## 4. Funcionalidades implementadas

### 4.1 Cadastro de usuário

O formulário solicita e-mail, senha, curso e período. Os campos são obrigatórios; a interface exige senha com pelo menos seis caracteres e período entre 1 e 20.

Ao cadastrar, a aplicação cria a conta no Firebase Authentication e, em seguida, grava o perfil em `users/{uid}` no Firestore, usando o identificador da conta como identificador do documento. A senha é tratada pelo Authentication e não é gravada nesse perfil.

| Campo do perfil | Conteúdo |
|---|---|
| `displayName` | Texto inicialmente vazio |
| `course` | Curso informado no formulário |
| `period` | Período informado pelo usuário |
| `createdAt` | Data e hora atribuídas pelo servidor |

### 4.2 Login e controle de sessão

O login utiliza e-mail e senha. A aplicação acompanha alterações de autenticação com `onAuthStateChanged` e apresenta uma mensagem de carregamento enquanto o estado inicial é determinado.

Após a autenticação, a interface mostra o e-mail do usuário e o botão **Sair**, que encerra a sessão com `signOut`. Durante o envio do formulário, os botões de cadastro e login ficam desabilitados. Falhas de cadastro ou login são apresentadas na tela.

### 4.3 Verificação de conflitos de horário

A função `hasScheduleConflict` compara dois intervalos, cada um composto por dia, início e término. Há conflito quando os intervalos se sobrepõem no mesmo dia. Horários consecutivos, em que um termina exatamente quando o outro começa, são permitidos.

A função valida o formato `HH:mm` e rejeita intervalos cujo início seja igual ou posterior ao término. Essa lógica está implementada e possui testes unitários, mas ainda não está integrada a uma tela de montagem de grade.

## 5. Regras de acesso ao banco

As regras em `firebase/firestore.rules` estabelecem as seguintes permissões:

- O usuário autenticado pode consultar, criar e atualizar o próprio perfil, respeitando a validação dos campos.
- A listagem geral de perfis e a exclusão de perfis são bloqueadas.
- O curso deve conter de 1 a 100 caracteres, o nome pode ter até 100 caracteres e o período deve ser inteiro entre 1 e 20.
- A criação exige um timestamp do servidor em `createdAt`; atualizações não podem alterar esse campo.
- A subcoleção `users/{uid}/completedCourses` permite operações apenas ao proprietário, com documentos contendo um timestamp `completedAt`.
- Usuários autenticados podem consultar o catálogo `courses`, mas não podem alterá-lo pelo cliente.
- Os demais caminhos têm leitura e escrita negadas por padrão.

As regras para disciplinas concluídas e catálogo preparam parte da estrutura de dados; elas não representam telas já implementadas.

## 6. Execução local

O ambiente utiliza Docker Desktop com Compose V2, Bun 1.3.14 e Node.js 22.12 ou superior.

Na raiz do projeto, iniciar os emuladores:

```powershell
docker compose -f compose.dev.yml up --build firebase
```

Em outro terminal, preparar e iniciar o frontend:

```powershell
cd frontend
# Executar a cópia apenas se o arquivo .env.local ainda não existir.
Copy-Item .env.example .env.local
bun install --frozen-lockfile
bun run dev
```

A aplicação fica disponível em `http://localhost:5173`. A configuração de exemplo utiliza `VITE_USE_EMULATORS=true` e o projeto de demonstração `demo-bcc323`.

Também é possível iniciar os dois serviços pelo Docker, executando na raiz:

```powershell
docker compose -f compose.dev.yml up --build
```

O Compose de desenvolvimento não configura exportação e importação dos dados dos emuladores. Portanto, a conservação desses dados entre recriações do ambiente não está garantida.

## 7. Testes e validação

O projeto contém as seguintes verificações automatizadas:

| Verificação | Escopo presente no código |
|---|---|
| ESLint | Análise estática do frontend |
| TypeScript | Verificação de tipos |
| Testes unitários | Sobreposição, horários consecutivos, dias diferentes e intervalo inválido |
| Teste de componente | Renderização do componente de boas-vindas |
| Testes de integração | Criação do próprio perfil, bloqueio de escrita em perfil alheio, acesso anônimo e escrita no catálogo |
| Teste de navegador | Cadastro e saída com emuladores |
| Build | Verificação de tipos e geração da aplicação pelo Vite |

Para executar as verificações locais do frontend, dentro de `frontend/`:

```powershell
bun run lint
bun run typecheck
bun run test:unit
bun run build
```

Para executar a bateria completa em Docker, na raiz:

```powershell
./scripts/test-all.ps1
```

O script completo cria os serviços de teste e, ao finalizar, remove os containers e volumes desse ambiente.

**Validação em 14/10/2026:** instalação, execução local, cadastro e armazenamento de perfis foram validados. A bateria automatizada foi executada com `./scripts/test-all.ps1` em Docker e todas as verificações passaram: ESLint, TypeScript, cinco testes unitários/de componente, build, quatro testes de integração das regras Firestore e um teste de cadastro e saída no Chromium. O build emitiu um aviso de bundle acima de 500 kB, sem impedir a conclusão. O CI do PR #24 no GitHub também foi concluído com sucesso.

## 8. Integração contínua e implantação

O repositório inclui workflows para verificações de qualidade, integração e testes de navegador, além de publicação no Firebase Hosting e publicação manual de regras do Firestore.

Os projetos Firebase `spoa-eng2-staging` e `spoa-eng2-prod` possuem aplicativos Web, Authentication e Firestore configurados, com cadastro e gravação de perfis validados. A publicação automática depende das permissões entre GitHub e Google Cloud e dos GitHub Environments. A validação da conexão aos projetos Firebase não equivale à publicação no Hosting.

Os procedimentos de implantação estão descritos em [docs/IMPLANTACAO.md](docs/IMPLANTACAO.md).

## 9. Limitações da implementação

O protótipo ainda não oferece interface para histórico de disciplinas, consulta de matriz curricular, pré-requisitos, painel de progresso, montagem e persistência da grade, fórum ou administração do catálogo.

Também não há telas de edição de perfil ou recuperação de senha. O cadastro da conta e a gravação do perfil são operações separadas: se a gravação falhar após a criação da conta, o código atual não desfaz automaticamente o cadastro nem implementa recuperação desse perfil.

Os módulos acadêmicos dependem da modelagem dos dados, das interfaces e das regras de negócio correspondentes. Os procedimentos de publicação e validação por ambiente estão no guia de implantação.
