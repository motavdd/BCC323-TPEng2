# Guia de contribuição da equipe

Este é o roteiro para sair de uma cópia do repositório e entregar uma funcionalidade
em PR. Os comandos abaixo usam PowerShell, a partir da raiz, salvo indicação contrária.

## 1. Preparar a primeira execução

Clone o repositório e use a branch de integração:

```powershell
git clone https://github.com/motavdd/BCC323-TPEng2.git
cd BCC323-TPEng2
git switch Develop
git pull --ff-only origin Develop
```

Instale os pré-requisitos e execute o setup descritos em
[Desenvolvimento local](DESENVOLVIMENTO_LOCAL.md#preparar-no-windows-x64).
O setup fornece Node, Ninja, Java, dependências e emuladores. Para Linux/macOS,
use a [alternativa Docker](AMBIENTE.md#alternativa-com-docker); os scripts nativos
foram preparados para Windows x64.

```powershell
./scripts/setup-local.ps1
```

Abra três terminais na raiz e execute um serviço em cada um:

| Terminal | Comando |
|---|---|
| 1 | `./scripts/dev-local.ps1 firebase` |
| 2 | `./scripts/dev-local.ps1 backend` |
| 3 | `./scripts/dev-local.ps1 frontend` |

Abra a [aplicação local](http://127.0.0.1:5173), crie uma conta fictícia e saia.
Confira a [API pelo proxy](http://127.0.0.1:5173/api/health): a resposta deve conter
`status: "ok"` e `service: "bcc323-api"`. O [painel Firebase](http://127.0.0.1:4000)
permite consultar os usuários e documentos criados.
Esses endereços só respondem com os respectivos serviços ativos.

Não é necessário entrar em uma conta Google para usar o projeto emulado
`demo-bcc323`. O frontend iniciado pelo script já recebe a configuração local.

## 2. Escolher e iniciar uma tarefa

Escolha uma Issue no [quadro da equipe](https://github.com/users/motavdd/projects/7),
confira responsável e critérios de aceite e registre quem está trabalhando nela.
Combine alterações de contratos e modelos compartilhados com os colegas envolvidos
na própria Issue ou no PR antes de implementar os dois lados.

Com seu trabalho anterior salvo em commit e a árvore de trabalho limpa:

```powershell
git switch Develop
git pull --ff-only origin Develop
git switch -c feature/16-formulario-cadastro
```

Substitua o número e o resumo pelos da sua Issue. Os prefixos usados pela equipe
são `feature/`, `fix/`, `docs/`, `test/`, `refactor/` e `chore/`.
Abra o PR para `Develop`; as promoções para `Staging` e `Master` seguem um fluxo separado.

## 3. Entender a base antes de alterar

| Área | Ponto de entrada | Responsabilidade atual |
|---|---|---|
| Interface | `frontend/src/App.tsx`, `components/`, `styles.css` | Cadastro, login e saída; Tailwind disponível |
| Firebase no navegador | `frontend/src/lib/firebase.ts` | Authentication e Firestore pelo SDK cliente |
| Regras de negócio existentes | `frontend/src/utils/schedule.ts` | Comparação de horários, ainda sem tela de grade |
| API C++ | `backend/src/main.cpp`, `config.*`, `routes.*` | Inicialização, configuração, saúde e erros HTTP |
| Permissões do banco | `firebase/firestore.rules` | Validação e autorização dos documentos |
| Ferramentas e testes | `scripts/`, `compose.test.yml`, `.github/workflows/ci.yml` | Execução local e CI |

Cadastro, login e perfis já existem como protótipo. Histórico, matriz curricular,
pré-requisitos, progresso, grade, fórum e administração ainda exigem implementação.
Consulte [Implementação atual](../implementacao.md) e [Plano técnico](PLANO_TECNICO.md)
para distinguir o que está implementado do que está planejado.

### Frontend

Coloque componentes reutilizáveis em `frontend/src/components/` e lógica sem UI
em módulos próprios, como `utils/`. O projeto ainda não tem um roteador de telas;
caso sua tarefa precise dele, combine essa mudança e versione a dependência e seu
lockfile juntos. Reutilize a inicialização de `lib/firebase.ts`.

Para chamar a API local, use caminhos relativos, como `fetch('/api/health')`.
O Vite encaminha a chamada para o C++; não coloque `localhost:8081` nas telas.
O build publicado ainda não tem encaminhamento remoto da API.

### Backend

Siga o [guia do backend](../backend/README.md). `main.cpp` inicia o processo;
`config.*` concentra configuração; `routes.*` registra os endpoints.
Crie módulos por funcionalidade conforme necessário e inclua os novos arquivos
`.cpp` no alvo de `backend/CMakeLists.txt`. Mantenha o contrato de `/api/health`.

Para cada endpoint novo, defina método, caminho, entrada, resposta, erros e quem
pode acessá-lo. Inclua testes de comportamento. Se usar o tratamento atual de 405,
ajuste o cabeçalho `Allow` para corresponder aos métodos da rota.
Após editar C++, encerre o backend com Ctrl+C e inicie-o novamente para recompilar.

O C++ ainda não verifica tokens Firebase nem acessa Firestore. Rotas privadas
dependem dessa implementação; o login no navegador não autentica automaticamente
uma requisição recebida pelo C++.

### Firestore

As regras atuais são a referência para as permissões implementadas:

| Caminho | Acesso atual |
|---|---|
| `users/{uid}` | Dono lê, cria e altera campos permitidos; listagem e exclusão bloqueadas |
| `users/{uid}/completedCourses/{courseId}` | Dono lê e grava `completedAt` do tipo timestamp e exclui o registro |
| `courses/{courseId}` | Leitura autenticada; escrita pelo cliente bloqueada |
| Demais caminhos | Leitura e escrita bloqueadas |

Perfil usa `displayName`, `course`, `period` e `createdAt`. A criação precisa de
`serverTimestamp()` para `createdAt`, curso não vazio com até 100 caracteres,
nome com até 100 caracteres e período inteiro de 1 a 20.
Ao alterar o modelo, atualize quem lê/grava os dados, as regras e os testes de
permissão juntos. Para dados de catálogo locais, use documentos fictícios pelo
painel do emulador; o código cliente continua sujeito às regras.

## 4. Validar a alteração

Execute a verificação rápida e confira os critérios da Issue na aplicação:

```powershell
./scripts/check-local.ps1
```

| Alteração | Verificação adicional |
|---|---|
| Lógica ou componentes React | Testes em `frontend/src/**/*.test.ts` ou `.test.tsx` |
| Regras ou acesso ao Firestore | Casos permitidos e negados em `frontend/tests/integration/` |
| Fluxo completo no navegador | Testes em `frontend/e2e/` |
| Contrato ou configuração do C++ | Testes HTTP do [backend](../backend/README.md#testes) |
| Documentação | Conferir comandos, caminhos e links afetados |

Com os três serviços ativos, a integração nativa executa regras e navegador:

```powershell
./scripts/check-local.ps1 -Integration
```

Os testes de regras apagam documentos do Firestore emulado. Use dados fictícios.
A verificação rápida compila C++, mas não executa seus testes HTTP; rode-os
separadamente com Python 3 ou pela bateria Docker. O CI executa a bateria completa.

## 5. Enviar e revisar

Revise o diff antes do commit. Inclua somente arquivos da tarefa: dependências
instaladas, builds, `.tools/`, dados dos emuladores, arquivos `.env.local` e caches
do editor não fazem parte da entrega.

```powershell
git status --short
git diff
# Selecione os arquivos da tarefa e faça o commit.
git push -u origin HEAD
```

Abra um PR para `Develop`, vincule a Issue com `Closes #numero`, descreva o
comportamento e os testes executados e solicite revisão. Use o template do PR.
Antes do merge, os checks `backend`, `quality`, `integration` e `e2e` devem passar
e a revisão deve ser aprovada. Depois, atualize o quadro da equipe.

## Problemas comuns

| Sintoma | O que conferir |
|---|---|
| Comando não encontrado | Pré-requisitos e PATH; reinicie o terminal após instalar ferramentas |
| PowerShell bloqueia `.ps1` | Consulte `Get-ExecutionPolicy -List`; se a política do computador permitir, use `Set-ExecutionPolicy -Scope Process RemoteSigned` no terminal atual |
| Porta ocupada | Encerre a instância anterior ou o Compose de desenvolvimento antes de iniciar outro modo |
| Configuração Firebase ausente | Inicie o frontend pelo script; para comandos manuais, siga [Bun](BUN.md#comandos-do-frontend) |
| Cadastro falha com `permission-denied` | Confira os emuladores, o projeto `demo-bcc323`, os campos e as regras; conta e perfil são operações separadas |
| Conta existe, mas o perfil não foi salvo | Confira Auth e Firestore no painel; a recuperação automática desse caso ainda não existe |
| `/api/health` falha | Teste primeiro a porta 8081 e depois o proxy em 5173; confira se o backend está ativo |
| Mudança em C++ não aparece | Reinicie pelo script para recompilar |
| Instalação congelada falha | Alinhe manifesto e lockfile; veja [Dependências](BUN.md#dependências-e-lockfiles) |
| Chromium ausente | Execute a integração pelo script, que instala o navegador no caminho local configurado |

Se persistir, registre na Issue o comando, a mensagem de erro e o modo usado
(nativo ou Docker), sem incluir credenciais.
