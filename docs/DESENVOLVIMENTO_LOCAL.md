# Desenvolvimento local sem Docker

Os passos abaixo iniciam a aplicação no seu computador. O React mostra as telas,
a API C++ processa os dados acadêmicos e os emuladores Firebase cuidam do login
e dos dados de teste.

## Início rápido

Na raiz do projeto, rode os dois comandos do seu sistema. O primeiro prepara o
ambiente e pode ser repetido quando as dependências mudarem. No segundo, aguarde
a mensagem **Aplicação pronta**.

| Sistema | Preparar | Iniciar |
|---|---|---|
| Windows | `./scripts/setup-local.ps1` | `./scripts/dev-local.ps1` |
| Linux | `bash scripts/setup-local.sh` | `bash scripts/dev-local.sh` |

Abra a [aplicação local](http://127.0.0.1:5173/) e crie uma conta fictícia.
Para escolher uma tarefa ou abrir um PR, siga o [guia da equipe](CONTRIBUICAO.md).

## Onde ficam os requisitos

- Frontend e Firebase CLI usam `frontend/package.json`, `tools/package.json`
  e os respectivos `bun.lock`. A versão do Bun está em `.bun-version`.
- As bibliotecas C++ são configuradas em `backend/CMakeLists.txt`.
  OpenSSL e libcurl vêm do ambiente de compilação.
- Git, Bun e as ferramentas nativas são pré-requisitos do sistema,
  detalhados abaixo para cada plataforma.

O teste HTTP do backend usa somente a biblioteca padrão do Python. Instale
Python 3 se quiser rodá-lo localmente. O projeto não precisa de `requirements.txt`
para instalar pacotes Python.

## Preparar no Windows x64

Instalar Git, Bun 1.3.14, CMake 3.24 ou superior e
GCC/MinGW do MSYS2 UCRT64. `git`, `bun`, `cmake` e `g++` devem estar no PATH.
OpenSSL e libcurl de desenvolvimento também devem estar disponíveis para o CMake
na mesma distribuição UCRT64 do compilador.
Node, Ninja e Java são fornecidos pelo setup; não precisam de instalação global.
Os scripts também reconhecem o compilador em `C:/msys64/ucrt64/bin/g++.exe`.
Referências: [MSYS2](https://www.msys2.org/) e [CMake](https://cmake.org/download/).

Na raiz do repositório:

```powershell
./scripts/setup-local.ps1
```

O setup usa as versões registradas nos lockfiles e compila a API C++. Também
baixa o emulador Firestore e o painel Firebase.

No Windows, Node.js 22.23.3, Ninja 1.13.1 e Java Temurin JRE 21.0.12.1 ficam
em `.tools/`. O setup confere o SHA-256 dos downloads e usa essas ferramentas
só no projeto. O Firebase CLI 15.0.0 fica em `tools/node_modules`.
Os lockfiles ficam em `tools/bun.lock` e `frontend/bun.lock`.

Execute novamente o setup após mudanças nos manifestos ou lockfiles. A primeira
execução exige internet para baixar ferramentas, pacotes e bibliotecas.

Crow e Asio são obtidos pelo CMake na primeira configuração. O build nativo fica
em `backend/build/native`. Python não é necessário para iniciar o projeto.
Os testes HTTP rodam no Docker do CI; para executá-los diretamente no computador,
instale Python 3 e siga o [guia do backend](../backend/README.md#testes).

Para redes com inspeção HTTPS, o script gera um arquivo de autoridades públicas
já confiáveis no Windows em `.tools/ca-windows.pem`. Bun e Node usam esse arquivo
somente nos processos iniciados pelos scripts. A verificação TLS permanece ativa.

## Preparar no Linux

Use Bash com Git, Bun na versão de `.bun-version`, Node 22.12+, Java 21+,
CMake 3.24+, Ninja e G++ instalados no PATH. Python 3 é opcional para os testes
HTTP do backend. O setup Linux não usa `sudo` nem instala ferramentas globais.

No Ubuntu/Debian, as ferramentas de compilação podem ser instaladas com
`sudo apt-get install git build-essential cmake ninja-build python3`.
Instale também `libssl-dev` e `libcurl4-openssl-dev` para autenticação e Firestore REST.
Instale também Bun, Node e Java nas versões indicadas acima; confira com
`bun --version`, `node --version` e `java -version`.

Na raiz:

```bash
bash scripts/setup-local.sh
```

O script verifica os pré-requisitos, instala os pacotes pelos lockfiles,
compila o backend em `backend/build/linux/` e baixa o emulador Firestore e o painel.
O cache de compilação Windows permanece em `backend/build/native/`.
Use instalações de dependências próprias de cada sistema: ao trocar o sistema
de uma cópia compartilhada, execute o setup novamente. Prefira cópias separadas
para Windows e Linux/WSL para não compartilhar pacotes binários de `node_modules`.

## Comandos por sistema

Execute a partir da raiz:

| Objetivo | Windows (PowerShell) | Linux (Bash) |
|---|---|---|
| Preparar | `./scripts/setup-local.ps1` | `bash scripts/setup-local.sh` |
| Aplicação completa | `./scripts/dev-local.ps1` | `bash scripts/dev-local.sh` |
| Firebase | `./scripts/dev-local.ps1 firebase` | `bash scripts/dev-local.sh firebase` |
| Backend | `./scripts/dev-local.ps1 backend` | `bash scripts/dev-local.sh backend` |
| Frontend | `./scripts/dev-local.ps1 frontend` | `bash scripts/dev-local.sh frontend` |
| Verificação rápida | `./scripts/check-local.ps1` | `bash scripts/check-local.sh` |
| Integração com serviços ativos | `./scripts/check-local.ps1 -Integration` | `bash scripts/check-local.sh --integration` |
| Bateria Docker | `./scripts/test-all.ps1` | `bash scripts/test-all.sh` |

Os endereços e dados emulados são os mesmos nos dois sistemas. No Linux,
Playwright também precisa das bibliotecas de sistema do Chromium. Se reportar
dependências ausentes, com o setup concluído, execute em `frontend/`:

```bash
node node_modules/@playwright/test/cli.js install-deps chromium
```

Esse instalador pode solicitar privilégios administrativos para instalar pacotes
do sistema. Depois, execute novamente a integração pelo script.

## Iniciar os três serviços

Os exemplos abaixo usam Windows. No Linux, use os equivalentes da tabela acima.

Se o ambiente Docker anterior estiver em execução, pare-o primeiro para liberar
as mesmas portas. Isso é necessário somente na troca de ambiente:

```powershell
docker compose -f compose.dev.yml down
```

Depois do setup, execute na raiz do projeto:

```powershell
./scripts/dev-local.ps1
```

O comando também aceita `all` explicitamente. Ele verifica se as portas estão
livres, compila a API, inicia o Firebase emulado, prepara o catálogo fictício e
sobe o frontend. Aguarde a mensagem **Aplicação pronta** e abra o endereço exibido.
Para encerrar, pressione Ctrl+C no mesmo terminal e aguarde a exportação do Firebase.

Para investigar um serviço separadamente, abra três terminais:

```powershell
# Terminal 1: Firebase Authentication, Firestore e painel local
./scripts/dev-local.ps1 firebase
```

Com o Firebase ativo, execute na raiz para cadastrar o catálogo fictício, se ele
ainda estiver ausente. No Windows, a primeira linha carrega o Node portátil.

```powershell
. ./scripts/local-env.ps1
node scripts/seed-demo.mjs
```

No Linux, basta executar `node scripts/seed-demo.mjs`.

```powershell
# Terminal 2: compilar e executar C++
./scripts/dev-local.ps1 backend
```

```powershell
# Terminal 3: React/Vite
./scripts/dev-local.ps1 frontend
```

O comando individual do frontend define as variáveis locais e executa
`bun run dev` dentro de `frontend/`. Consulte [Comandos do Bun](BUN.md#comandos-do-frontend).

| Endereço | Uso |
|---|---|
| `http://127.0.0.1:5173` | Aplicação React |
| `http://127.0.0.1:5173/api/health` | Comunicação React/Vite → C++ |
| `http://127.0.0.1:8081/api/health` | Serviço C++ diretamente |
| `http://127.0.0.1:4000` | Painel dos emuladores Firebase |
| `127.0.0.1:9099` | Authentication local |
| `127.0.0.1:8080` | Firestore local |

O frontend recarrega ao editar os arquivos. Após alterar C++, reinicie o comando
de inicialização para recompilar a API. No modo individual, reinicie o backend.
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

## PR e CI

Siga o [guia de contribuição](CONTRIBUICAO.md#5-enviar-e-revisar) para abrir o PR,
acompanhar os checks e solicitar revisão.

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

As datas, os resultados e os limites de cada rodada de testes estão no
[histórico de validação](../implementacao.md#4-cobertura-e-histórico-de-validação).
Esses registros não substituem a execução dos testes nas novas alterações.

Referência: [Firebase Local Emulator Suite](https://firebase.google.com/docs/emulator-suite/install_and_configure).
