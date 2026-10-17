# Documentação do projeto

Comece pelo [desenvolvimento local](DESENVOLVIMENTO_LOCAL.md) para preparar e executar React, C++ e Firebase no Windows. Bun instala dependências e executa os scripts do frontend. Docker é opcional no computador e utilizado pelo CI.

| Documento | Quando consultar |
|---|---|
| [Desenvolvimento local](DESENVOLVIMENTO_LOCAL.md) | Preparar o computador, iniciar serviços e validar antes de enviar |
| [Bun](BUN.md) | Usar `bun run`, gerenciar dependências e atualizar lockfiles |
| [Ambiente](AMBIENTE.md) | Comparar execução nativa, Docker e modo híbrido; conferir portas e limites |
| [Decisões técnicas](DECISOES.md) | Consultar tecnologias, versões e responsabilidades |
| [Plano técnico](PLANO_TECNICO.md) | Entender arquitetura, escopo atual e etapas do projeto |
| [Implantação](IMPLANTACAO.md) | Configurar CI, proteção de branches e publicação |
| [Implementação atual](../implementacao.md) | Consultar funcionalidades e limitações do protótipo |

## Comandos principais

Na raiz do repositório, em PowerShell no Windows x64:

| Objetivo | Comando |
|---|---|
| Preparar o ambiente | `./scripts/setup-local.ps1` |
| Iniciar Firebase local | `./scripts/dev-local.ps1 firebase` |
| Compilar e iniciar C++ | `./scripts/dev-local.ps1 backend` |
| Iniciar React com Bun e configuração local | `./scripts/dev-local.ps1 frontend` |
| Verificar código e builds sem serviços ativos | `./scripts/check-local.ps1` |
| Incluir regras e navegador, com três serviços ativos | `./scripts/check-local.ps1 -Integration` |
| Executar a bateria completa em Docker | `./scripts/test-all.ps1` |

Cada serviço roda em seu próprio terminal. Os comandos `bun run dev`, `bun run lint` e os demais scripts do frontend são executados dentro de `frontend/`.
A raiz ainda não possui `package.json` nem um comando único `bun run dev`.

O fluxo de integração é **branch da tarefa → PR para Develop → checks e revisão aprovados → merge**. A promoção segue **Develop → Staging → Master**.
