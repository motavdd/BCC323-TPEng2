# Desenvolvimento local sem Docker

O fluxo segue a separação observada no TerraPlanner: desenvolvimento direto no
computador, verificações rápidas antes do envio e validação integrada no CI.
Neste projeto, acrescentamos o serviço C++ e os emuladores Firebase locais.

## Preparar no Windows x64

Instalar Git, Bun 1.3.14, CMake 3.24 ou superior e
GCC/MinGW do MSYS2 UCRT64. `git`, `bun`, `cmake` e `g++` devem estar no PATH.
Node, Ninja e Java são fornecidos pelo setup; não precisam de instalação global.
Os scripts também reconhecem o compilador em `C:/msys64/ucrt64/bin/g++.exe`.
Referências: [MSYS2](https://www.msys2.org/) e [CMake](https://cmake.org/download/).

Na raiz do repositório:

```powershell
./scripts/setup-local.ps1
```

O script instala as dependências com os lockfiles, baixa Node.js 22.23.3,
Ninja 1.13.1 e Java Temurin JRE 21.0.12.1 em `.tools/`, verifica o SHA-256 dos arquivos, compila o C++
e baixa o emulador Firestore. As ferramentas portáteis não alteram as instalações
globais. Os scripts usam o Node portátil em seus processos. O Firebase CLI 15.0.0 fica em `tools/node_modules`, com versões resolvidas
em `tools/bun.lock`; o frontend mantém seu próprio `frontend/bun.lock`.

Execute novamente o setup após mudanças nos manifestos ou lockfiles. A primeira
execução exige internet para baixar ferramentas, pacotes e bibliotecas.

Crow e Asio são obtidos pelo CMake na primeira configuração. O build nativo fica
em `backend/build/native`. Python não é necessário para desenvolver; os testes
HTTP de CTest continuam executados pelo Docker no CI.

Para redes com inspeção HTTPS, o script gera um arquivo de autoridades públicas
já confiáveis no Windows em `.tools/ca-windows.pem`. Bun e Node usam esse arquivo
somente nos processos iniciados pelos scripts. A verificação TLS permanece ativa.

## Iniciar os três serviços

Se o ambiente Docker anterior estiver em execução, pare-o primeiro para liberar
as mesmas portas. Isso é necessário somente na troca de ambiente:

```powershell
docker compose -f compose.dev.yml down
```

Abra três terminais na raiz do projeto:

```powershell
# Terminal 1: Firebase Authentication, Firestore e painel local
./scripts/dev-local.ps1 firebase
```

```powershell
# Terminal 2: compilar e executar C++
./scripts/dev-local.ps1 backend
```

```powershell
# Terminal 3: React/Vite
./scripts/dev-local.ps1 frontend
```

O terceiro comando define as variáveis locais e executa `bun run dev` dentro de
`frontend/`. Bun continua sendo utilizado normalmente. Para executar seus comandos
diretamente, consulte [Bun](BUN.md#comandos-do-frontend). A raiz não possui
um `package.json` ou um comando único para iniciar todos os serviços.

| Endereço | Uso |
|---|---|
| `http://127.0.0.1:5173` | Aplicação React |
| `http://127.0.0.1:5173/api/health` | Comunicação React/Vite → C++ |
| `http://127.0.0.1:8081/api/health` | Serviço C++ diretamente |
| `http://127.0.0.1:4000` | Painel dos emuladores Firebase |
| `127.0.0.1:9099` | Authentication local |
| `127.0.0.1:8080` | Firestore local |

O frontend recarrega ao editar os arquivos. Após alterar C++, encerre o terminal
do backend com Ctrl+C e execute seu comando novamente para recompilar.
Encerre os serviços com Ctrl+C. No terminal do Firebase, aguarde a exportação
terminar; se aparecer uma confirmação para sobrescrever a exportação anterior,
confirme para salvar a sessão. Evite fechar a janela ou forçar o encerramento.

O Firebase usa o projeto fictício `demo-bcc323`, sem login no Google nem acesso a
um projeto remoto. O script do frontend define explicitamente as variáveis locais,
com prioridade sobre arquivos `.env.local` existentes, sem sobrescrevê-los.
A configuração nativa está em `firebase.local.json`, limitada a loopback.
O Docker continua usando `firebase.json` e os nomes internos da sua rede.

Os dados são exportados para `.firebase-data/` ao encerrar normalmente e importados
na próxima execução. `.tools/`, `.firebase-data/`, `node_modules` e os builds
não entram no Git. Use apenas dados fictícios nos emuladores.

## Validar antes de enviar

```powershell
./scripts/check-local.ps1
```

Esse comando compila C++, executa lint, tipos e testes unitários do frontend e
gera o build. Não precisa de Docker nem dos emuladores em execução.
Além disso, teste a funcionalidade manualmente na aplicação com os três serviços
locais ativos. Compilar o C++ não substitui seus testes de comportamento.

Para executar também os testes Firestore e os fluxos de navegador sem Docker,
mantenha os três serviços ativos e execute:

```powershell
./scripts/check-local.ps1 -Integration
```

A primeira execução baixa o Chromium headless para `.tools/playwright`.
Os testes criam contas e perfis fictícios no projeto emulado. Os testes de regras
limpam o Firestore desse projeto após cada caso; os dados locais existentes podem
ser removidos ao executar a integração. A exportação não impede essa limpeza.

Para verificar a conexão:

```powershell
Invoke-RestMethod http://127.0.0.1:5173/api/health
```

## Validar no PR para Develop

1. Trabalhar em uma branch criada da `Develop`.
2. Executar a verificação local e conferir o comportamento implementado.
3. Enviar a branch e abrir um PR para `Develop`, como rascunho se necessário.
4. Aguardar os jobs `backend`, `quality`, `integration` e `e2e` do GitHub Actions.
5. Solicitar revisão e integrar depois de aprovar os testes e a revisão.

O CI utiliza Docker para compilar e testar o serviço C++, testar as regras Firestore
e executar os fluxos completos de navegador. Os workflows estão preparados para
PRs; o mantenedor deve configurar os checks como obrigatórios na proteção da
`Develop`. A existência do workflow não configura essa proteção automaticamente.

Quem tiver Docker pode executar a mesma bateria antes de enviar:

```powershell
./scripts/test-all.ps1
```

O Compose de desenvolvimento permanece disponível como alternativa. Docker não
é pré-requisito para os comandos `setup-local`, `dev-local` e `check-local`.

## Validação desta preparação

Em 15/10/2026, a preparação e a compilação foram executadas no Windows x64.
`check-local.ps1 -Integration` passou: lint, tipos, cinco testes unitários/de
componente, build, quatro testes das regras Firestore e dois testes Playwright.
Frontend → C++ e o painel Firebase responderam localmente. A exportação dos dados
dos emuladores foi confirmada. O build mantém o aviso existente de bundle JavaScript
acima de 500 kB, sem falhar.

O teste HTTP do backend em Docker também passou após a adaptação para Windows.
A execução do workflow atualizado no GitHub e a configuração dos checks obrigatórios
dependem do envio do PR e das permissões do mantenedor.

Referência: [Firebase Local Emulator Suite](https://firebase.google.com/docs/emulator-suite/install_and_configure).
