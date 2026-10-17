# Backend C++

API C++17 com Crow. O cadastro e os perfis continuam no SDK Firebase do frontend.

## Desenvolvimento

Na raiz, execute `./scripts/setup-local.ps1` e depois
`./scripts/dev-local.ps1 backend`. A API responde em
`http://127.0.0.1:8081/api/health`. O frontend usa o proxy `/api` do Vite.

| Variável | Padrão do executável | Docker |
|---|---|---|
| `HOST` | `127.0.0.1` | `0.0.0.0` |
| `PORT` | `8081` | `8081` |

`HOST` aceita endereços IPv4 ou IPv6, sem protocolo nem porta; `PORT` aceita
inteiros de 1 a 65535. O inicializador nativo fixa `127.0.0.1:8081`.
Configurações inválidas encerram o processo com código de erro.

## Organização

- `src/main.cpp`: inicialização do processo.
- `src/config.*`: leitura e validação de configuração.
- `src/routes.*`: registro de rotas e respostas de erro.
- `test/smoke.py`: contrato HTTP e validação de configuração no processo real.

`GET /api/health` retorna `{"status":"ok","service":"bcc323-api"}` e não
consulta serviços externos. `HEAD` e `OPTIONS` são tratados pelo Crow.
Rotas inexistentes retornam 404; métodos não permitidos retornam 405.
Erros usam `{"error":{"code":"not_found","message":"Route not found"}}`,
com código e mensagem correspondentes ao status. Exceções nas rotas retornam 500
sem expor detalhes internos. As respostas de saúde e erro usam `Cache-Control: no-store`.

Ao adicionar rotas, mantenha os módulos separados por funcionalidade e atualize
o registro, os testes e os métodos do cabeçalho `Allow` para as novas rotas.
Endpoints privados precisam verificar o token Firebase no servidor antes de
acessar dados; essa integração ainda não está implementada.

## Testes

A bateria existente do CI compila e executa CTest em Docker. Na raiz:

```sh
docker compose -f compose.test.yml build backend-test
docker compose -f compose.test.yml run --rm --no-deps backend-test
```

Para testar o binário nativo já compilado, com Python 3 instalado:

```powershell
python backend/test/smoke.py backend/build/native/bcc323-api.exe
```

O teste escolhe uma porta livre e encerra seu próprio processo. Não precisa
iniciar o Firebase. `check-local.ps1` compila o C++, mas os testes HTTP são
executados separadamente ou no CI.

## Publicação

O Dockerfile gera uma imagem de execução com usuário sem privilégios e suporte
a `HOST`/`PORT`. A publicação da API e o encaminhamento de `/api` no Firebase
Hosting ainda precisam ser configurados antes de usar endpoints C++ remotamente.
