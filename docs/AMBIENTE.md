# Ambiente — React + C++ + Firebase

O fluxo principal usa os três serviços diretamente no Windows x64. Siga [Desenvolvimento local](DESENVOLVIMENTO_LOCAL.md) para preparar, iniciar e validar.
Bun instala dependências e executa os scripts do frontend, conforme [BUN.md](BUN.md).

## Formas de execução

| Modo | Frontend | C++ | Firebase | Uso |
|---|---|---|---|---|
| Nativo | Bun/Vite | CMake + GCC | CLI e emuladores locais | Desenvolvimento principal |
| Docker | Container | Container | Container | CI e alternativa local |
| Híbrido | Bun/Vite no computador | Container | Container | Alternativa local |

Os modos usam as mesmas portas. Encerre os serviços ativos antes de trocar de modo.

## Endereços e dados

| Serviço | Nativo | Docker de desenvolvimento |
|---|---|---|
| Frontend | `http://127.0.0.1:5173` | `http://localhost:5173` |
| C++ | `http://127.0.0.1:8081/api/health` | `http://localhost:8081/api/health` |
| Authentication | `127.0.0.1:9099` | `localhost:9099` |
| Firestore | `127.0.0.1:8080` | `localhost:8080` |
| Painel Firebase | `http://127.0.0.1:4000` | Desabilitado |
| Configuração dos emuladores | `firebase.local.json` | `firebase.json` |
| Persistência | Exportação em `.firebase-data/` ao encerrar normalmente | Sem exportação/importação configurada |

Ambos usam o projeto fictício `demo-bcc323`. Os testes criam contas e documentos fictícios.
A execução nativa pode manter esses dados na próxima sessão; os emuladores não são armazenamento definitivo.

## Comunicação entre serviços

```text
Navegador → Vite :5173 → /api/* → Crow :8081
         → SDK Firebase → Authentication :9099 e Firestore :8080
```

O navegador solicita `/api` na origem do frontend. O Vite encaminha para `API_PROXY_TARGET`, variável do servidor: `http://127.0.0.1:8081` no modo nativo ou `http://backend:8081` no Compose.
O CI usa nomes internos de serviços, conforme `compose.test.yml`.

O backend aceita `PORT` entre 1 e 65535; o padrão é 8081. O script local fixa 8081.
Alterar essa porta exige ajustar o inicializador, o proxy e, no Docker, o mapeamento e o healthcheck.

## Alternativa com Docker

Requer Docker Desktop com containers Linux ou Docker Engine, e Compose V2.
Na raiz:

```powershell
docker compose -f compose.dev.yml up --build -d
docker compose -f compose.dev.yml ps
```

Abra `http://localhost:5173` e confira `http://localhost:5173/api/health`.
O Docker fornece as ferramentas dos serviços; não exige instalar compilador, CMake, Bun ou Java no computador. A primeira compilação precisa de internet.

```powershell
docker compose -f compose.dev.yml logs backend web
docker compose -f compose.dev.yml down
```

O Vite recarrega alterações em `frontend/src`, montado no container.
Após mudar dependências, configuração ou C++, execute novamente `up --build -d`.
O backend não possui recompilação automática.

## Alternativa híbrida

Na raiz:

```powershell
docker compose -f compose.dev.yml up --build -d backend firebase
```

Em outro terminal, use `./scripts/dev-local.ps1 frontend` se já executou o setup nativo.
Para execução manual com Bun e Node instalados, entre em `frontend/`, prepare `.env.local` a partir de `.env.example` somente se ele não existir, confira as variáveis e execute `bun install --frozen-lockfile` e `bun run dev`.
Mantenha `VITE_USE_EMULATORS=true`, projeto `demo-bcc323` e host do emulador acessível pelo computador (`localhost`).

## Validação

| Verificação | Comando na raiz | Pré-requisitos |
|---|---|---|
| Rápida | `./scripts/check-local.ps1` | Setup nativo concluído |
| Integração nativa | `./scripts/check-local.ps1 -Integration` | Três serviços nativos ativos |
| Bateria Docker | `./scripts/test-all.ps1` | Docker ativo |
| Bateria Docker em Linux/WSL | `bash scripts/test-all.sh` | Docker e Bash |

A bateria Docker usa `compose.test.yml`, isolado do Compose de desenvolvimento, e remove seus containers e volumes ao terminar.
Os resultados de 15/10/2026 estão em [Desenvolvimento local](DESENVOLVIMENTO_LOCAL.md#validação-desta-preparação).
Os checks e a proteção de PR são descritos em [Implantação](IMPLANTACAO.md).

## Limites atuais

A única rota C++ é `GET /api/health`, que retorna `{"status":"ok","service":"bcc323-api"}`.
Ela verifica o processo HTTP sem consultar Firebase.
Cadastro e perfis continuam acessando Firebase pelo SDK cliente.
Autenticação de rotas privadas, acesso do C++ ao banco e funcionalidades acadêmicas da API ainda dependem de implementação.

O proxy Vite atua no desenvolvimento e na prévia dos testes. Ele não acompanha o build estático.
Os workflows atuais publicam o frontend no Firebase Hosting; a hospedagem da API e o encaminhamento remoto de `/api` estão pendentes.
