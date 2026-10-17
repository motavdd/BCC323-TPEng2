# Implementação — Planejamento Acadêmico

**Disciplina:** BCC323 — Trabalho Prático de Engenharia de Software II  
**Revisão:** 15/10/2026

O documento descreve a implementação presente no repositório. Os resultados de
validação têm datas próprias e não confirmam publicação remota. Os guias operacionais
estão no [índice de documentação](docs/README.md).

## 1. Objetivo e escopo

Este documento descreve a implementação atual do protótipo de planejamento acadêmico, sua organização, as funcionalidades disponíveis e os procedimentos de execução e validação.

A versão atual estabelece a base técnica do sistema: interface web, autenticação, armazenamento de perfis, regras de acesso ao banco e infraestrutura para testes. As funcionalidades completas de planejamento acadêmico ainda dependem de desenvolvimento.

## 2. Tecnologias utilizadas

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

## 3. Arquitetura e organização

A aplicação é uma SPA executada no navegador. O frontend se comunica diretamente com o Firebase por meio do SDK cliente. As permissões de acesso aos documentos são verificadas pelas regras do Firestore. O diretório `backend/` contém a base C++/Crow com a rota `/api/health`, ainda sem participação no fluxo de autenticação e perfis.

No ambiente nativo, o navegador acessa Vite em `http://127.0.0.1:5173`, Authentication
na porta `9099` e Firestore na porta `8080`. O painel local fica na porta `4000`.
O proxy `/api` do Vite encaminha para o C++ na porta `8081`.

| Arquivo ou diretório | Responsabilidade |
|---|---|
| `frontend/src/App.tsx` | Formulário, cadastro, login, estado da sessão e saída |
| `frontend/src/components/Welcome.tsx` | Exibição do e-mail da sessão autenticada |
| `frontend/src/lib/firebase.ts` | Inicialização do Firebase e conexão aos emuladores |
| `frontend/src/utils/schedule.ts` | Verificação de conflitos entre horários |
| `backend/src/main.cpp` | Serviço C++ e rota pública de saúde |
| `backend/CMakeLists.txt` | Compilação e bibliotecas do backend |
| `backend/test/smoke.py` | Testes HTTP reais e validação da porta do processo |
| `tools/` | Firebase CLI para o desenvolvimento nativo |
| `frontend/tests/integration/` | Testes das regras do Firestore |
| `frontend/e2e/` | Testes de navegador |
| `firebase/firestore.rules` | Controle de acesso e validação dos documentos |
| `firebase.json` | Configuração de emuladores, regras e Hosting |
| `firebase.local.json` | Emuladores nativos e painel em loopback |
| `compose.dev.yml` | Alternativa de desenvolvimento em Docker |
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

### 4.4 Serviço C++

O backend oferece `GET /api/health`, com resposta JSON
`{"status":"ok","service":"bcc323-api"}` e `Cache-Control: no-store`.
Essa rota verifica o processo HTTP, sem consultar Firebase. A variável `PORT`
aceita inteiros de 1 a 65535 e usa 8081 por padrão; valores inválidos impedem a inicialização.

O Vite encaminha `/api` para o backend durante desenvolvimento e preview.
O build estático não inclui esse proxy. O serviço ainda não recebe tokens Firebase,
acessa Firestore ou executa regras de negócio acadêmicas.

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

O fluxo principal passou a ser nativo no Windows: preparar com
`./scripts/setup-local.ps1` e iniciar cada serviço em seu próprio terminal com
os comandos abaixo. O painel dos emuladores
fica em `http://127.0.0.1:4000`. Os dados fictícios são exportados ao encerrar normalmente.
Use `./scripts/check-local.ps1` para verificações rápidas ou acrescente `-Integration`
com os serviços ativos para testar regras e navegador. Instruções completas:
[Desenvolvimento local](docs/DESENVOLVIMENTO_LOCAL.md).

```powershell
# Cada comando em um terminal separado, na raiz.
./scripts/dev-local.ps1 firebase
./scripts/dev-local.ps1 backend
./scripts/dev-local.ps1 frontend
```

O script do frontend chama `bun run dev` em `frontend/` e define as variáveis do
projeto fictício `demo-bcc323`. Os arquivos `.env.local` existentes são preservados.
Os scripts carregam Node, Ninja e Java portáteis em seus processos; Git, Bun,
CMake e GCC são pré-requisitos. Os comandos Bun diretos estão em [Bun](docs/BUN.md).

O CI continua usando Docker, também disponível como alternativa local:

```powershell
docker compose -f compose.dev.yml up --build
```

Esse comando inicia frontend, C++ e Firebase em containers. O painel Firebase está
desabilitado nessa configuração, e o Compose não exporta/importa os dados emulados.
Encerre os serviços do outro modo antes de trocar, pois usam as mesmas portas.
Os detalhes do modo híbrido estão em [Ambiente](docs/AMBIENTE.md).

## 7. Testes e validação

O projeto contém as seguintes verificações automatizadas:

| Verificação | Escopo presente no código |
|---|---|
| ESLint | Análise estática do frontend |
| TypeScript | Verificação de tipos |
| Testes unitários | Sobreposição, horários consecutivos, dias diferentes e intervalo inválido |
| Teste de componente | Renderização do componente de boas-vindas |
| Testes C++ com CTest | Resposta HTTP e rejeição de valores inválidos de `PORT` |
| Testes de integração | Criação do próprio perfil, bloqueio de escrita em perfil alheio, acesso anônimo e escrita no catálogo |
| Testes de navegador | Cadastro/saída e encaminhamento `/api/health` para o C++ |
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

A verificação nativa rápida usa `./scripts/check-local.ps1`. Com os três serviços
ativos, `./scripts/check-local.ps1 -Integration` inclui regras Firestore e navegador.
Os testes de regras limpam o Firestore do projeto emulado após cada caso; a
exportação de dados locais não impede essa limpeza.

**Validação em 14/10/2026:** instalação, execução local, cadastro e armazenamento de perfis foram validados. A bateria automatizada foi executada com `./scripts/test-all.ps1` em Docker e todas as verificações passaram: ESLint, TypeScript, cinco testes unitários/de componente, build, quatro testes de integração das regras Firestore e um teste de cadastro e saída no Chromium. O build emitiu um aviso de bundle acima de 500 kB, sem impedir a conclusão. O CI do PR #24 no GitHub também foi concluído com sucesso.

**Validação em 15/10/2026:** após adicionar C++ e Tailwind, a bateria Docker passou
com CTest, cinco testes unitários/de componente, quatro testes Firestore e dois
testes Playwright, além de lint, tipos e build. A preparação nativa no Windows e
`check-local.ps1 -Integration` também passaram; a exportação Firebase foi verificada.
O teste CTest em Docker foi repetido com sucesso após adaptar o CMake para Windows.
O aviso de bundle acima de 500 kB permanece. Esses resultados são locais; o workflow
atualizado ainda precisa executar no GitHub após o envio do PR.

## 8. Integração contínua e implantação

O CI possui os jobs `backend`, `quality`, `integration` e `e2e`, executados em PRs
para `Develop`, `Staging` e `Master`, além de push em `Develop` e execução manual.
A proteção de branches precisa exigir os checks e a revisão; os workflows não
criam Rulesets automaticamente. A promoção segue `Develop → Staging → Master`.

Os workflows de `Staging` e `Master` executam o CI antes de publicar o frontend
no Firebase Hosting, usando OIDC/WIF e GitHub Environments. A publicação manual
de regras Firestore é separada e não publica índices. A API C++ não é publicada
pelos workflows atuais.

O registro de 14/10/2026 informa Authentication e Firestore configurados nos projetos
`spoa-eng2-staging` e `spoa-eng2-prod`, com cadastro e gravação de perfis validados.
Esse registro não confirma a configuração remota atual nem a publicação no Hosting.

Os procedimentos de implantação estão descritos em [docs/IMPLANTACAO.md](docs/IMPLANTACAO.md).

## 9. Limitações da implementação

O protótipo ainda não oferece interface para histórico de disciplinas, consulta de matriz curricular, pré-requisitos, painel de progresso, montagem e persistência da grade, fórum ou administração do catálogo.

Também não há telas de edição de perfil ou recuperação de senha. O cadastro da conta e a gravação do perfil são operações separadas: se a gravação falhar após a criação da conta, o código atual não desfaz automaticamente o cadastro nem implementa recuperação desse perfil.

Os módulos acadêmicos dependem da modelagem dos dados, das interfaces e das regras de negócio correspondentes. Os procedimentos de publicação e validação por ambiente estão no guia de implantação.

O C++ ainda possui somente a rota pública de saúde. Autenticação de rotas privadas,
acesso ao banco, hospedagem da API e encaminhamento remoto de `/api` estão pendentes.
