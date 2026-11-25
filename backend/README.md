# Backend C++ — MVP acadêmico

O C++ autentica as requisições, carrega dados do Firestore e utiliza as classes
`AlunoImpl`, `CursoImpl` e `DisciplinaImpl` para histórico, progresso e elegibilidade.
O React usa Firebase Authentication para cadastro e login, e a API para os dados
do MVP. Operações do aluno usam o token Firebase e respeitam as Security Rules.
O servidor usa sua identidade de serviço para ler agendamentos privados e
gravar alterações autorizadas e validadas.

Consulte o [contrato OpenAPI](openapi.json). O cliente tipado das telas fica em
[academic.ts](../frontend/src/services/academic.ts).

## Estrutura

As pastas `include/` e `src/` espelham as mesmas camadas:

| Pasta | Responsabilidade |
|---|---|
| `domain/` | Interfaces e implementações de aluno, usuário, curso, disciplina e sistema acadêmico |
| `application/` | Serviços do MVP, DTOs e interface do repositório |
| `http/` | Rotas Crow, validação de entrada e serialização JSON |
| `infrastructure/auth/` | Verificação de tokens e certificados Firebase |
| `infrastructure/firestore/` | Repositório persistente pela API REST |
| `config/` | Configuração de host e porta |
| `src/main.cpp` | Inicialização |
| `test/application/` | Regras e integração com o domínio, sem rede |
| `test/auth/` | Verificação criptográfica de tokens |
| `test/fixtures/` | Dados ilustrativos usados exclusivamente para preparar testes |
| `test/smoke.py` | Contrato HTTP do processo e configuração, sem Firebase |

As bibliotecas CMake separam `academic-domain`, `academic-application` e
`academic-infrastructure`. O domínio e a aplicação compilam sem Crow ou Firebase.
Não coloque consultas ao banco nas entidades nem cálculos acadêmicos nos handlers.

## Executar localmente

Depois do setup, execute `./scripts/dev-local.ps1` no Windows ou
`bash scripts/dev-local.sh` no Linux. O comando inicia o ambiente completo e
prepara o catálogo ilustrativo no emulador. O seed preserva o catálogo existente
e só aceita projetos fictícios `demo-*`.

Para iniciar só a API ou depurar um serviço, siga
[Desenvolvimento local](../docs/DESENVOLVIMENTO_LOCAL.md#iniciar-os-três-serviços).

| Variável | Execução direta | Scripts locais / Compose |
|---|---|---|
| `HOST` | `127.0.0.1` | `127.0.0.1` / `0.0.0.0` |
| `PORT` | `8081` | `8081` |
| `FIREBASE_PROJECT_ID` | Obrigatório nas rotas privadas | `demo-bcc323` |
| `FIREBASE_AUTH_EMULATOR_HOST` | Ausente: validação RS256 | `127.0.0.1:9099` / `firebase:9099` |
| `FIRESTORE_EMULATOR_HOST` | Ausente: Firestore HTTPS | `127.0.0.1:8080` / `firebase:8080` |

Tokens sem assinatura só são aceitos quando o projeto começa com `demo-` e
`FIREBASE_AUTH_EMULATOR_HOST` está explicitamente definido. Firestore emulado
também exige projeto demo. Na implantação remota, remova ambas as variáveis de
emulador e configure o projeto real. `GET /api/health` verifica apenas o processo.

## API

Envie `Authorization: Bearer <Firebase ID token>`. Nunca envie senha ou UID no corpo.

| Operação | Método e caminho |
|---|---|
| Identidade | `GET /api/me` |
| Cursos | `GET /api/courses` |
| Perfil | `GET /api/profile`, `PUT /api/profile` |
| Matriz do aluno | `GET /api/curriculum` |
| Matriz e ofertas por semestre | `GET /api/courses/{courseId}/curriculum?term={termId}` |
| Planejamento do próximo semestre | `GET /api/academic/planning` |
| Agendar alteração | `POST /api/courses/{courseId}/schedule` |
| Histórico | `GET /api/history` |
| Progresso e elegibilidade | `GET /api/academic/summary` |
| Concluir disciplina | `PUT /api/history/{codigo}` |
| Remover conclusão | `DELETE /api/history/{codigo}` |

O servidor lê o histórico persistido para calcular o progresso. O antigo
`POST /api/academic/evaluate` e o cabeçalho `example_curriculum.hpp` foram removidos.
O catálogo fica em `courses/{courseId}` no Firestore. Perfis antigos que só guardam
o nome do curso continuam legíveis; novos perfis persistem também `courseId`.
Mudar de curso com histórico existente exige uma migração ainda fora do MVP.

### Vigência e coordenação

Cadastre o calendário no documento `academicCalendar/terms` com um campo
`terms` contendo objetos com `id`, `start` e `end` em datas `AAAA-MM-DD`.
Os intervalos são inclusivos e separados. A data efetiva segue o dia civil de
Ouro Preto (UTC−03:00). O planejamento usa a matriz e as ofertas do primeiro
dia do próximo semestre cadastrado. Sem um próximo semestre cadastrado, usa os
dados disponíveis para a data atual.

O vínculo de coordenação é um documento `courses/{courseId}/coordinators/{uid}`
com `active: true`, provisionado pela administração do Firebase. A API rejeita
gravações de usuários sem esse vínculo, mesmo que sejam coordenadores de outro
curso. Em Cloud Run, configure uma conta de serviço com permissão de leitura,
criação e atualização de documentos de agendamento via Firestore REST.
No emulador, o backend usa a identidade administrativa do emulador para a agenda.
As regras do Firestore precisam ser atualizadas junto com a API.

Para trocar só o professor de uma turma existente no semestre vigente:

```json
{"type":"offering","id":"BCC202-A","term":"2026-2","professor":"Professora substituta"}
```

Opcionalmente envie `effectiveDate` e/ou `times` com os blocos completos
(`day`, `start`, `end` e `room`). Dias aceitos: Seg, Ter, Qua, Qui, Sex e Sáb.
Uma mudança de turma vale só para o semestre indicado, a partir da data
agendada. Sem data, vale hoje no semestre vigente ou no início de um semestre
futuro. Uma mudança na mesma turma e data substitui o agendamento para essa data.

Para editar uma disciplina existente, envie `type: "subject"`, `id`, `term`,
`name`, `hours`, `period` e `prerequisites`. A vigência é o primeiro dia de um
semestre futuro e permanece nos semestres seguintes até nova alteração.
`completedNames` guarda os nomes exibidos no histórico por ID estável.
Registros legados sem nome consultam a matriz original, que os agendamentos
preservam.

Erros seguem `{"error":{"code":"...","message":"..."}}`; respostas usam
`Cache-Control: no-store`. O contrato detalha os estados e o tratamento esperado.

## Testes

```sh
cmake -S backend -B backend/build/native -DBUILD_TESTING=ON
cmake --build backend/build/native --parallel 2
ctest --test-dir backend/build/native --output-on-failure
```

Use um diretório de build configurado para o compilador da sua máquina. No Windows,
carregue `. ./scripts/local-env.ps1` antes de compilar para configurar GCC e DLLs.
Se Python 3 não estiver disponível, CMake habilita os testes C++ e informa que
o smoke HTTP ficará disponível no CI. Quando instalado, pode executar também:

```sh
python3 backend/test/smoke.py backend/build/linux/bin/bcc323-api
```

A bateria Docker completa continua em `scripts/test-all.ps1` / `scripts/test-all.sh`.
`frontend/e2e/academic-api.spec.ts` testa a API com Firebase emulado, incluindo
persistência e isolamento entre alunos. O setup do Playwright prepara o catálogo
de teste; ele não depende de telas para validar esses contratos.

## Fora desta etapa

Planejamento com turmas, conflitos, fórum e coordenação não fazem parte do contrato
do MVP. O fluxo anterior de planos no front foi preservado e ainda usa o SDK cliente.
A matriz oficial, a hospedagem do C++ e o encaminhamento remoto de `/api` precisam
ser preparados antes da entrega publicada. Nenhum deploy é feito por esta alteração.
