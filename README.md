# BCC323-TPEng2 — Desenvolvimento (`Develop`)

Protótipo de planejamento acadêmico com React, TypeScript, Tailwind CSS,
C++/Crow e Firebase. A base oferece cadastro, login, perfis e uma API de saúde;
as funcionalidades acadêmicas estão em desenvolvimento.

Comece pelo [guia de contribuição da equipe](docs/CONTRIBUICAO.md), com o roteiro
da primeira execução até o PR. Consulte o [índice de documentação](docs/README.md)
para encontrar os guias de cada área.

- [Quadro da equipe](https://github.com/users/motavdd/projects/7)
- [Issues](https://github.com/motavdd/BCC323-TPEng2/issues)

## Início rápido

No Windows x64, instale Git, Bun 1.3.14, CMake e GCC/MinGW UCRT64 conforme
[Desenvolvimento local](docs/DESENVOLVIMENTO_LOCAL.md). O setup fornece Node,
Ninja e Java portáteis. Na raiz da cópia do repositório:

```powershell
git switch Develop
git pull --ff-only origin Develop
./scripts/setup-local.ps1
```

Inicie um serviço por terminal, sempre na raiz:

```powershell
# Terminal 1
./scripts/dev-local.ps1 firebase
# Terminal 2
./scripts/dev-local.ps1 backend
# Terminal 3
./scripts/dev-local.ps1 frontend
```

Abra a [aplicação local](http://127.0.0.1:5173) e confira a
[API pelo proxy](http://127.0.0.1:5173/api/health).
Para Docker ou modo híbrido, siga [Ambiente](docs/AMBIENTE.md).
A raiz não tem `package.json`; comandos Bun diretos são executados em `frontend/`.

## Linux

No Linux, use `bash scripts/setup-local.sh`, `bash scripts/dev-local.sh SERVICO`
e `bash scripts/check-local.sh`. Pré-requisitos e equivalências:
[Linux e Windows](docs/DESENVOLVIMENTO_LOCAL.md#comandos-por-sistema).

## Antes de enviar

Execute `./scripts/check-local.ps1` e valide o comportamento da tarefa.
A integração nativa, com os três serviços ativos, usa
`./scripts/check-local.ps1 -Integration` e limpa dados do Firestore emulado.
Alterações no C++ também exigem os [testes HTTP do backend](backend/README.md#testes).

Trabalhe em uma branch da tarefa e abra PR para `Develop`. Aguarde os checks
`backend`, `quality`, `integration` e `e2e` e a revisão antes do merge.
O fluxo completo e os testes por área estão no [guia da equipe](docs/CONTRIBUICAO.md).

A promoção segue `Develop → Staging → Master`, respeitando maiúsculas e
minúsculas. Nos PRs de promoção, preserve o README específico da branch de destino.
As proteções e a publicação são descritas em [Implantação](docs/IMPLANTACAO.md).
Os workflows atuais publicam somente o frontend; a hospedagem da API C++ está pendente.
