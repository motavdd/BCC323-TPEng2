# Backend C++ — MVP acadêmico

O C++ autentica as requisições, carrega dados do Firestore e utiliza as classes
`AlunoImpl`, `CursoImpl` e `DisciplinaImpl` para histórico, progresso e elegibilidade.
O React usa Firebase Authentication para cadastro e login, e a API para os dados
do MVP. Perfis e históricos usam o token Firebase do aluno e respeitam as
Security Rules. As alterações da coordenação passam pela API, que valida o
vínculo com o curso e grava somente no catálogo com uma identidade de serviço.

Consulte [OpenAPI](openapi.json). O cliente tipado das telas do aluno fica em
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

Depois de `./scripts/setup-local.ps1`, abra terminais separados na raiz:

1. `./scripts/dev-local.ps1 firebase`
2. `node scripts/seed-demo.mjs` (após os emuladores iniciarem)
3. `./scripts/dev-local.ps1 backend`
4. `./scripts/dev-local.ps1 frontend`

No Linux, use `bash scripts/setup-local.sh` e
`bash scripts/dev-local.sh firebase|backend|frontend`.
O seed preserva catálogo existente e só aceita projeto `demo-*` no emulador.
Não é executado automaticamente no início do servidor nem usado em produção.

| Variável | Execução direta | Scripts locais / Compose |
|---|---|---|
| `HOST` | `127.0.0.1` | `127.0.0.1` / `0.0.0.0` |
| `PORT` | `8081` | `8081` |
| `FIREBASE_PROJECT_ID` | Obrigatório nas rotas privadas | `demo-bcc323` |
| `FIREBASE_AUTH_EMULATOR_HOST` | Ausente: validação RS256 | `127.0.0.1:9099` / `firebase:9099` |
| `FIRESTORE_EMULATOR_HOST` | Ausente: Firestore HTTPS | `127.0.0.1:8080` / `firebase:8080` |
| `FIRESTORE_CATALOG_USE_METADATA` | `true` habilita escrita do catálogo com a identidade do serviço no Google Cloud | Dispensável no emulador |

Tokens sem assinatura só são aceitos quando o projeto começa com `demo-` e
`FIREBASE_AUTH_EMULATOR_HOST` está explicitamente definido. Firestore emulado
também exige projeto demo. Na implantação remota, remova ambas as variáveis de
emulador e configure o projeto real. `GET /api/health` verifica apenas o processo.

## API

Envie `Authorization: Bearer <Firebase ID token>`. Nunca envie senha ou UID no corpo.

| Operação | Método e caminho |
|---|---|
| Identidade | `GET /api/me` |
| Permissão da sessão | `GET /api/session` |
| Cursos | `GET /api/courses` |
| Perfil | `GET /api/profile`, `PUT /api/profile` |
| Matriz do aluno | `GET /api/curriculum` |
| Histórico | `GET /api/history` |
| Progresso e elegibilidade | `GET /api/academic/summary` |
| Concluir disciplina | `PUT /api/history/{codigo}` |
| Remover conclusão | `DELETE /api/history/{codigo}` |
| Matriz da coordenação | `GET /api/coordinator/curriculum` |
| Salvar ou arquivar disciplina | `PUT` ou `DELETE /api/coordinator/subjects/{codigo}` |
| Salvar ou remover turma | `PUT` ou `DELETE /api/coordinator/offerings/{codigo}` |

O servidor lê o histórico persistido para calcular o progresso. O antigo
`POST /api/academic/evaluate` e o cabeçalho `example_curriculum.hpp` foram removidos.
O catálogo fica em `courses/{courseId}` no Firestore. Perfis antigos que só guardam
o nome do curso continuam legíveis; novos perfis persistem também `courseId`.
Mudar de curso com histórico existente exige uma migração ainda fora do MVP.

Erros seguem `{"error":{"code":"...","message":"..."}}`; respostas usam
`Cache-Control: no-store`. O contrato detalha os estados e o tratamento esperado.

## Habilitar um coordenador

1. Crie a conta no Firebase Authentication com e-mail institucional da UFOP e
   confirme o e-mail. O cadastro aceita `@aluno.ufop.edu.br` e `@ufop.edu.br`.
2. Um administrador do ambiente cria o documento `coordinators/{uid}` no
   Firestore com o campo string `courseId`. Use o UID do Authentication e o
   ID de um curso existente em `courses`. O seed de demonstração cria
   `computacao-exemplo`.
3. Entre com essa conta para abrir a tela da coordenação. Para revogar a
   permissão, remova o documento de concessão. Cada chamada à API verifica o
   vínculo vigente.

No ambiente local, a concessão pode ser criada na aba Firestore da interface
do emulador em `http://127.0.0.1:4000`. O próprio usuário só pode ler sua
concessão. As regras negam alterações de catálogo diretamente pelo SDK, inclusive
para coordenadores. Antes da confirmação de e-mail, uma conta institucional
pode acessar o próprio perfil e ler o catálogo público do aplicativo para
concluir o cadastro. O histórico, o plano e a coordenação exigem confirmação.

Envie o valor de `revision` retornado pela coordenação no cabeçalho `If-Match`
ao salvar ou remover. O Firestore usa essa revisão como precondição atômica.
Em caso de 409, recarregue a matriz antes de refazer a alteração. Disciplinas
arquivadas preservam código e carga horária para o histórico. A operação
retira as turmas e rejeita o arquivamento de pré-requisitos de disciplinas ativas.

Em homologação ou produção no Google Cloud, atribua uma conta de serviço
dedicada ao backend com a permissão IAM de atualizar documentos Firestore
existentes (`datastore.entities.update`) no projeto ou banco da aplicação e
habilite `FIRESTORE_CATALOG_USE_METADATA=true`. A API obtém um token temporário
da identidade do serviço. A concessão de coordenador continua sendo
administrada separadamente e a API mantém a escrita restrita ao catálogo
do curso autorizado. Use HTTPS para o acesso pelo navegador e publique as
Security Rules do mesmo commit.

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
de teste. `frontend/e2e/coordinator.spec.ts` grava pelas telas, lê o documento
diretamente no emulador, verifica o reflexo para o aluno e exercita as
restrições de autorização e de integridade.

## Fora desta etapa

O salvamento geral de planos com as novas ofertas e o fórum ficam para as
próximas sprints. O fluxo anterior de planos ainda usa o SDK cliente.
A matriz oficial, a hospedagem do C++ e o encaminhamento remoto de `/api` precisam
ser preparados antes da entrega publicada. Nenhum deploy é feito por esta alteração.
