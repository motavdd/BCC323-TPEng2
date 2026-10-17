# Implementação — Planejamento Acadêmico

**Disciplina:** BCC323 — Trabalho Prático de Engenharia de Software II  
**Revisão:** 17/10/2026

O documento descreve a implementação presente no repositório. Os resultados de
validação têm datas próprias e não confirmam publicação remota. Os guias operacionais
estão no [índice de documentação](docs/README.md).

## 1. Objetivo e escopo

Este documento descreve a implementação atual do protótipo de planejamento acadêmico, as funcionalidades disponíveis, as limitações e os resultados de validação.

A versão atual estabelece a base técnica do sistema: interface web, autenticação, armazenamento de perfis, regras de acesso ao banco e infraestrutura para testes. As funcionalidades completas de planejamento acadêmico ainda dependem de desenvolvimento.

A arquitetura, a lista de tecnologias e a organização dos arquivos ficam no
[plano técnico](docs/PLANO_TECNICO.md). Para executar o projeto, consulte
[Desenvolvimento local](docs/DESENVOLVIMENTO_LOCAL.md); para contribuir, use o
[guia da equipe](docs/CONTRIBUICAO.md).

## 2. Funcionalidades implementadas

### 2.1 Cadastro de usuário

O formulário solicita e-mail, senha, curso e período. Os campos são obrigatórios; a interface exige senha com pelo menos seis caracteres e período entre 1 e 20.

Ao cadastrar, a aplicação cria a conta no Firebase Authentication e, em seguida, grava o perfil em `users/{uid}` no Firestore, usando o identificador da conta como identificador do documento. A senha é tratada pelo Authentication e não é gravada nesse perfil.

| Campo do perfil | Conteúdo |
|---|---|
| `displayName` | Texto inicialmente vazio |
| `course` | Curso informado no formulário |
| `period` | Período informado pelo usuário |
| `createdAt` | Data e hora atribuídas pelo servidor |

### 2.2 Login e controle de sessão

O login utiliza e-mail e senha. A aplicação acompanha alterações de autenticação com `onAuthStateChanged` e apresenta uma mensagem de carregamento enquanto o estado inicial é determinado.

Após a autenticação, a interface mostra o e-mail do usuário e o botão **Sair**, que encerra a sessão com `signOut`. Durante o envio do formulário, os botões de cadastro e login ficam desabilitados. Falhas de cadastro ou login são apresentadas na tela.

### 2.3 Verificação de conflitos de horário

A função `hasScheduleConflict` compara dois intervalos, cada um composto por dia, início e término. Há conflito quando os intervalos se sobrepõem no mesmo dia. Horários consecutivos, em que um termina exatamente quando o outro começa, são permitidos.

A função valida o formato `HH:mm` e rejeita intervalos cujo início seja igual ou posterior ao término. Essa lógica está implementada e possui testes unitários, mas ainda não está integrada a uma tela de montagem de grade.

### 2.4 Serviço C++

O backend oferece `GET /api/health`, com resposta JSON
`{"status":"ok","service":"bcc323-api"}` e `Cache-Control: no-store`.
Essa rota verifica o processo HTTP, sem consultar Firebase. A variável `PORT`
aceita inteiros de 1 a 65535 e usa 8081 por padrão; valores inválidos impedem a inicialização.

`HOST` aceita IPv4 ou IPv6, com padrão `127.0.0.1`; a imagem Docker define
`0.0.0.0`. O script nativo fixa `127.0.0.1:8081`. Configuração e rotas possuem
módulos separados. Erros 404, 405 e 500 retornam JSON no formato
`{"error":{"code":"...","message":"..."}}`; exceções nas rotas não expõem
detalhes internos na resposta. Veja o [guia do backend](backend/README.md).

O Vite encaminha `/api` para o backend durante desenvolvimento e preview.
O build estático não inclui esse proxy. O serviço ainda não recebe tokens Firebase,
acessa Firestore ou executa regras de negócio acadêmicas.

## 3. Regras de acesso ao banco

As regras em `firebase/firestore.rules` estabelecem as seguintes permissões:

- O usuário autenticado pode consultar, criar e atualizar o próprio perfil, respeitando a validação dos campos.
- A listagem geral de perfis e a exclusão de perfis são bloqueadas.
- O curso deve conter de 1 a 100 caracteres, o nome pode ter até 100 caracteres e o período deve ser inteiro entre 1 e 20.
- A criação exige um timestamp do servidor em `createdAt`; atualizações não podem alterar esse campo.
- A subcoleção `users/{uid}/completedCourses` permite operações apenas ao proprietário, com documentos contendo um timestamp `completedAt`.
- Usuários autenticados podem consultar o catálogo `courses`, mas não podem alterá-lo pelo cliente.
- Os demais caminhos têm leitura e escrita negadas por padrão.

As regras para disciplinas concluídas e catálogo preparam parte da estrutura de dados; elas não representam telas já implementadas.

## 4. Cobertura e histórico de validação


O projeto contém as seguintes verificações automatizadas:

| Verificação | Escopo presente no código |
|---|---|
| ESLint | Análise estática do frontend |
| TypeScript | Verificação de tipos |
| Testes unitários | Sobreposição, horários consecutivos, dias diferentes e intervalo inválido |
| Teste de componente | Renderização do componente de boas-vindas |
| Testes C++ com CTest | Saúde, erros 404/405, HEAD e rejeição de `HOST`/`PORT` inválidos |
| Testes de integração | Criação do próprio perfil, bloqueio de escrita em perfil alheio, acesso anônimo e escrita no catálogo |
| Testes de navegador | Cadastro/saída e encaminhamento `/api/health` para o C++ |
| Build | Verificação de tipos e geração da aplicação pelo Vite |

Os comandos e pré-requisitos de validação estão nos guias de
[desenvolvimento local](docs/DESENVOLVIMENTO_LOCAL.md#validar-antes-de-enviar),
[backend](backend/README.md#testes) e [CI/Docker](docs/IMPLANTACAO.md#2-testes-no-docker).

**Validação em 14/10/2026:** instalação, execução local, cadastro e armazenamento de perfis foram validados. A bateria automatizada foi executada com `./scripts/test-all.ps1` em Docker e todas as verificações passaram: ESLint, TypeScript, cinco testes unitários/de componente, build, quatro testes de integração das regras Firestore e um teste de cadastro e saída no Chromium. O build emitiu um aviso de bundle acima de 500 kB, sem impedir a conclusão. O CI do PR #24 no GitHub também foi concluído com sucesso.

**Validação em 15/10/2026:** após adicionar C++ e Tailwind, a bateria Docker passou
com CTest, cinco testes unitários/de componente, quatro testes Firestore e dois
testes Playwright, além de lint, tipos e build. A preparação nativa no Windows e
`check-local.ps1 -Integration` também passaram; a exportação Firebase foi verificada.
O teste CTest em Docker foi repetido com sucesso após adaptar o CMake para Windows.
O aviso de bundle acima de 500 kB permanece.

**Validação em 17/10/2026:** os checks `backend`, `quality`, `integration` e `e2e`
do PR #37 estavam aprovados para `59c73fb`, que também passou na verificação
rápida nativa. Após os ajustes locais de organização, configuração e erros do
backend, a compilação e o CTest passaram com três casos de teste, incluindo
verificações de métodos HTTP e valores inválidos por subcasos. Docker e integração
Firebase/navegador não foram repetidos nessa rodada. As alterações posteriores
ao commit citado precisam executar novamente no CI após o envio.

**Ambientes remotos:** o registro de 14/10/2026 informa Authentication e Firestore configurados nos projetos
`spoa-eng2-staging` e `spoa-eng2-prod`, com cadastro e gravação de perfis validados.
Esse registro não confirma a configuração remota atual nem a publicação no Hosting.

## 5. Limitações da implementação


O protótipo ainda não oferece interface para histórico de disciplinas, consulta de matriz curricular, pré-requisitos, painel de progresso, montagem e persistência da grade, fórum ou administração do catálogo.

Também não há telas de edição de perfil ou recuperação de senha. O cadastro da conta e a gravação do perfil são operações separadas: se a gravação falhar após a criação da conta, o código atual não desfaz automaticamente o cadastro nem implementa recuperação desse perfil.

Os módulos acadêmicos dependem da modelagem dos dados, das interfaces e das regras de negócio correspondentes. Os procedimentos de publicação e validação por ambiente estão no guia de implantação.

O C++ ainda possui somente a rota pública de saúde. Autenticação de rotas privadas,
acesso ao banco, hospedagem da API e encaminhamento remoto de `/api` estão pendentes.

A configuração dos ambientes, proteções e workflows é mantida no
[guia de implantação](docs/IMPLANTACAO.md).
