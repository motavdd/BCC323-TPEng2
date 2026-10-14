# BCC323-TPEng2 — Guia de Desenvolvimento (`Develop`)

**Sistema de Planejamento e Organização Acadêmica** — frontend React + TypeScript + Vite; Firebase Authentication, Cloud Firestore e Hosting; Docker para testes. Backend próprio ainda não definido.

- [Repositório](https://github.com/motavdd/BCC323-TPEng2)
- [Kanban](https://github.com/users/motavdd/projects/7)
- [Issues](https://github.com/motavdd/BCC323-TPEng2/issues)
- [Plano técnico](docs/PLANO_TECNICO.md) · [Implantação](docs/IMPLANTACAO.md) · [Segurança](docs/SEGURANCA.md)

## 1. Branches e responsabilidades

| Branch | Uso | Entrada permitida |
|---|---|---|
| `Develop` | Integração diária | PR de `feature/*`, `fix/*`, `docs/*`, `test/*` |
| `Staging` | Homologação | PR de `Develop` |
| `Master` | Entrega estável | PR de `Staging` |

**Atenção:** os nomes reais usam maiúsculas: `Develop`, `Staging` e `Master`. Git diferencia maiúsculas/minúsculas. Ao promover código, o PR deve preservar o README próprio da branch de destino: merges normais podem levar este guia de desenvolvedores para `Staging`/`Master`.

## 2. Começando

Pré-requisitos: Git, Docker Engine/Desktop com Compose V2. Node.js 22 é necessário **apenas para gerar o lockfile** ou executar npm fora do Docker.

```bash
git clone https://github.com/motavdd/BCC323-TPEng2.git
cd BCC323-TPEng2
git switch Develop
git pull origin Develop
```


```bash
cd frontend
npm install
cd ..
docker compose -f compose.test.yml build
```

### Desenvolvimento local

```bash
docker compose -f compose.dev.yml up --build
# Aplicação: http://localhost:5173
# Auth emulator: localhost:9099 | Firestore emulator: localhost:8080
```

Os emuladores usam o projeto **fictício** `demo-bcc323`; os testes não exigem credenciais Firebase reais. Os dados emulados são efêmeros.

### Testes padronizados

```bash
# Lint, TypeScript, unitários e build
for task in lint typecheck test:unit build; do
  docker compose -f compose.test.yml run --rm --no-deps frontend npm run "$task"
done

# Integração: Firestore + regras de segurança
docker compose -f compose.test.yml run --rm frontend npm run test:integration

# E2E: navegador Playwright + React + emuladores
docker compose -f compose.test.yml run --rm playwright

# Limpeza
docker compose -f compose.test.yml down -v --remove-orphans
```

Se estiver no Windows/PowerShell, execute os quatro comandos da primeira etapa individualmente em vez do loop `for` do Bash.

## 3. Fluxo de contribuição

1. Escolha uma [Issue](https://github.com/motavdd/BCC323-TPEng2/issues), verifique critérios de aceitação, responsável e status no Kanban.
2. Atualize `Develop` e crie uma branch por tarefa:

   ```bash
   git switch Develop
   git pull origin Develop
   git switch -c feature/16-formulario-cadastro
   ```

3. Implemente alterações pequenas, documente decisões e escreva testes.
4. Execute os testes no Docker; **não** utilize um projeto Firebase de produção.
5. Faça commits claros (`feat: adiciona cadastro (#16)`) e envie a branch.
6. Abra PR para `Develop`, relacione a Issue (`Closes #16`), solicite ao menos uma revisão e aguarde os checks.
7. Após o merge, atualize o Kanban. A promoção `Develop → Staging → Master` é feita por PR e validação.

Prefixos: `feature/`, `fix/`, `docs/`, `test/`, `refactor/`, `chore/`. Formato: `<tipo>/<numero-da-issue>-<resumo>`.

## 4. Critérios para aprovar um PR

- [ ] Resolve a Issue e não inclui mudanças fora de escopo.
- [ ] CI passou (`quality`, `integration`, `e2e`).
- [ ] Testes cobrem os casos relevantes e regras de acesso.
- [ ] Sem credenciais, tokens ou dados pessoais em commits e logs.
- [ ] Pelo menos uma revisão de colega (após configuração das rulesets).
- [ ] Documentação atualizada, quando aplicável.

## 5. Estrutura do pacote

- `frontend/`: React, TypeScript, Vite, testes Vitest e Playwright.
- `firebase/`: regras e índices do Firestore.
- `docker/firebase/`: imagem dos emuladores.
- `compose.test.yml`: ambiente isolado e reprodutível de CI.
- `compose.dev.yml`: desenvolvimento local com portas somente no loopback.
- `.github/workflows/ci.yml`: validação automatizada de PRs.
- `.github/workflows/deploy.yml`: homologação e produção após nova verificação.
- `.github/workflows/deploy-firestore.yml`: publicação **manual** de regras revisadas.
- `docs/`: arquitetura, implantação, segurança e decisões pendentes.

## 6. Ambientes e deploy

| Contexto | Infraestrutura | Dados |
|---|---|---|
| Local/CI | Docker + Firebase Emulator Suite | Fictícios, descartáveis |
| `Staging` | Firebase Hosting no projeto de homologação | Dados de teste |
| `Master` | Firebase Hosting no projeto de produção | Dados reais autorizados |

O frontend estático é publicado no Firebase Hosting; **Docker não roda permanentemente em produção**. Deploys dependem de GitHub Environments, Workload Identity Federation e aprovação para produção. Consulte [Implantação](docs/IMPLANTACAO.md).

