# Implantação, CI/CD e proteção de branches

## 0. Pré-requisitos

- GitHub: permissão de administrador para criar Rulesets e Environments.
- Dois projetos Firebase **distintos**: um de homologação e um de produção, com Firebase Authentication, Firestore e Hosting habilitados.
- Em cada projeto: registrar um aplicativo Web, obter `projectId`, `apiKey`, `authDomain`, `appId`.
- Firebase Authentication: habilitar o provedor **E-mail/senha**; conferir domínios autorizados.
- Google Cloud: permissão para criar contas de serviço, Workload Identity Pool/Provider e conceder IAM.
- Docker Compose V2 localmente; para executar fora do Docker, Bun 1.3.14 e Node 22.12+.

## Ambientes do projeto

| Ambiente | Projeto Firebase | Branch |
|---|---|---|
| Homologação | `spoa-eng2-staging` | `Staging` |
| Produção | `spoa-eng2-prod` | `Master` |

Os aplicativos Web, Authentication e Firestore estão configurados nos dois projetos. O cadastro e a gravação de perfis foram validados. As seções seguintes descrevem a configuração necessária à publicação automática.

## 1. Preparar os arquivos no repositório

1. Vincule a alteração à Issue correspondente e trabalhe em uma branch a partir de `Develop`.
2. Em `frontend/`, execute `bun install --frozen-lockfile`. Os procedimentos de dependências estão em [Bun](BUN.md).
3. Versione `frontend/bun.lock` e os arquivos de configuração, sem incluir arquivos `.env.*.local`.
4. Execute os testes Docker e abra PR para `Develop`.
5. Configure os GitHub Environments e a autenticação WIF antes de promover para `Staging` e `Master`, pois os pushes nessas branches disparam o deploy. O workflow reutilizável precisa existir na revisão que chama a publicação.
6. Preserve o README específico da branch de destino na revisão de cada PR de promoção. Merges também incorporam alterações no `README.md` da origem.

**Checks obrigatórios:** regras que ainda exigem `success` precisam ser atualizadas para os checks `quality`, `integration` e `e2e`, após a execução do CI e a confirmação dos nomes exibidos no GitHub.

## 2. Testes no Docker

```bash
# Qualidade sem emulador
docker compose -f compose.test.yml build frontend
docker compose -f compose.test.yml run --rm --no-deps frontend bun run lint
docker compose -f compose.test.yml run --rm --no-deps frontend bun run typecheck
docker compose -f compose.test.yml run --rm --no-deps frontend bun run test:unit
docker compose -f compose.test.yml run --rm --no-deps frontend bun run build

# Testes de integração com Firebase Emulator
docker compose -f compose.test.yml run --rm frontend bun run test:integration

# Teste de navegador com emuladores
docker compose -f compose.test.yml run --rm playwright

# Parar/limpar
docker compose -f compose.test.yml down -v --remove-orphans
```

No desenvolvimento local:

```bash
docker compose -f compose.dev.yml up --build
# http://localhost:5173
```

O container Firebase expõe portas somente no `compose.dev.yml`, limitadas a `127.0.0.1`. No CI não publica portas no host. Nunca exponha emuladores publicamente.

## 3. Configurar Workload Identity Federation (WIF)

Execute a configuração para cada projeto Firebase. Os comandos utilizam Bash e o Google Cloud CLI autenticado com permissões administrativas. Defina `PROJECT_ID` como `spoa-eng2-staging` para homologação e `spoa-eng2-prod` para produção.

```bash
export PROJECT_ID='spoa-eng2-staging'
export REPO='motavdd/BCC323-TPEng2'
export PROJECT_NUMBER="$(gcloud projects describe "$PROJECT_ID" --format='value(projectNumber)')"
export SA_NAME='github-hosting-deployer'
export SA_EMAIL="${SA_NAME}@${PROJECT_ID}.iam.gserviceaccount.com"

gcloud iam service-accounts create "$SA_NAME" --project "$PROJECT_ID" \
  --display-name 'GitHub Actions Firebase Hosting'

gcloud projects add-iam-policy-binding "$PROJECT_ID" \
  --member "serviceAccount:${SA_EMAIL}" \
  --role roles/firebasehosting.admin

gcloud projects add-iam-policy-binding "$PROJECT_ID" \
  --member "serviceAccount:${SA_EMAIL}" \
  --role roles/serviceusage.apiKeysViewer

gcloud iam workload-identity-pools create github --project "$PROJECT_ID" \
  --location global --display-name 'GitHub Actions'

gcloud iam workload-identity-pools providers create-oidc bcc323 \
  --project "$PROJECT_ID" --location global \
  --workload-identity-pool github \
  --issuer-uri 'https://token.actions.githubusercontent.com' \
  --attribute-mapping 'google.subject=assertion.sub,attribute.repository=assertion.repository,attribute.ref=assertion.ref' \
  --attribute-condition "assertion.repository=='${REPO}' && (assertion.ref=='refs/heads/Staging' || assertion.ref=='refs/heads/Master')"

gcloud iam service-accounts add-iam-policy-binding "$SA_EMAIL" \
  --project "$PROJECT_ID" \
  --role roles/iam.workloadIdentityUser \
  --member "principalSet://iam.googleapis.com/projects/${PROJECT_NUMBER}/locations/global/workloadIdentityPools/github/attribute.repository/${REPO}"

echo "projects/${PROJECT_NUMBER}/locations/global/workloadIdentityPools/github/providers/bcc323"
echo "$SA_EMAIL"
```

Verifique a existência dos pools e providers antes da criação. Cada projeto utiliza sua própria conta de serviço, com os papéis necessários ao deploy. A condição apresentada aceita apenas as branches `Staging` e `Master` do repositório `motavdd/BCC323-TPEng2`.

O workflow manual de regras Firestore utiliza a conta definida em `GCP_DEPLOY_SERVICE_ACCOUNT` e exige também permissão de publicação de regras, como `roles/firebaserules.admin`. Os papéis de Hosting apresentados acima não concedem essa permissão.

## 4. Criar GitHub Environments

No repositório: **Settings → Environments**. Criar `staging` e `production`.

Em **cada Environment**, configurar estas **Variables** (não precisam ser secrets: a configuração web Firebase é pública; regras/IAM protegem os dados):

| Variable | Exemplo/descrição |
|---|---|
| `FIREBASE_PROJECT_ID` | ID real do projeto Firebase do ambiente |
| `VITE_FIREBASE_API_KEY` | API key do app Web daquele projeto |
| `VITE_FIREBASE_AUTH_DOMAIN` | Domínio Auth daquele projeto |
| `VITE_FIREBASE_APP_ID` | App ID Web daquele projeto |
| `GCP_WIF_PROVIDER` | `projects/NUM/locations/global/workloadIdentityPools/github/providers/bcc323` |
| `GCP_DEPLOY_SERVICE_ACCOUNT` | Email da conta de serviço do ambiente |

`production`: habilitar **Required reviewers**, restringir implantação à branch `Master` e, se disponível no plano, impedir autoaprovação. `staging`: restringir implantação à branch `Staging`; revisão opcional. Configurar proteção dos Environments **antes** de habilitar o workflow de deploy.

**Não** configurar `VITE_USE_EMULATORS=true` em ambiente publicado. O workflow define explicitamente `false`. As variáveis `VITE_*` são incorporadas ao JavaScript gerado e **não** devem conter senhas ou tokens secretos.

## 5. Proteção de branches (GitHub Rulesets)

Em **Settings → Rules → Rulesets**, após executar o primeiro CI e confirmar os nomes exatos dos checks:

| Branch | PR obrigatório | Aprovações | Checks obrigatórios | Outras regras |
|---|---|---:|---|---|
| `Develop` | Sim | 1 | `quality`, `integration`, `e2e` | Bloquear exclusão/force push |
| `Staging` | Sim | 1 | `quality`, `integration`, `e2e` | Bloquear exclusão/force push |
| `Master` | Sim | 1–2 | `quality`, `integration`, `e2e` | Bloquear exclusão/force push |

Os checks são selecionados pelos nomes exibidos no GitHub após um workflow bem-sucedido. O CI executa em PRs para as três branches; o CD executa outra rodada de CI no push para `Staging` ou `Master`. A lista de bypass define quem pode ignorar as proteções; o modo **For pull requests only** exige PR, mas ainda permite ignorar exigências ao fazer merge.

**Importante:** Rulesets não impõem automaticamente que o PR para `Master` tenha origem em `Staging`, nem que o PR para `Staging` tenha origem em `Develop`. Isso deve ser verificado na revisão ou por política adicional (workflow/branch rules específicos) antes de considerar o fluxo estritamente garantido.

## 6. Implantação automática

- PR para `Develop`, `Staging` ou `Master` → `CI / Docker`: quality, integration, e2e.
- Push após merge em `Staging` → `CD / Firebase Hosting`: chama CI novamente, usa Environment `staging`, publica no projeto Firebase de homologação.
- Push após merge em `Master` → CI novamente, aguarda aprovação de Environment `production`, publica no projeto Firebase de produção.
- `firebase deploy --only hosting` **não publica regras Firestore**.

O workflow usa autenticação OIDC/WIF (`google-github-actions/auth@v3`) e a CLI Firebase; **não usa chave JSON nem `FIREBASE_TOKEN`**. Se houver falha de IAM, verifique as permissões, APIs habilitadas, configurações do pool/provider e a propagação das políticas.

## 7. Regras Firestore: implantação separada e consciente

**Antes de publicar a aplicação pela primeira vez**, revise `firebase/firestore.rules` e rode os testes de integração. No GitHub Actions, execute **Manual / Firestore Rules** selecionando a branch **`Staging` e target `staging`**, e depois a branch **`Master` e target `production`** (após aprovação e validação). O workflow não executa se branch e target não corresponderem. O workflow busca a versão de regras na branch `Staging` ou `Master` correspondente.

Publicar regras via CLI substitui as regras atuais do projeto Firebase. Preserve a configuração existente e confirme a compatibilidade com os dados armazenados. O workflow não publica índices: `firebase/firestore.indexes.json` contém a configuração inicial, sem índices compostos cadastrados.

## 8. Rollback e resposta a incidentes

- **Hosting:** usar o histórico de releases no Console Firebase Hosting para voltar à versão anterior; registrar incidente/Issue.
- **Firestore rules:** não há rollback equivalente automático; restaurar o arquivo da revisão aprovada e executar novo deploy manual de regras.
- **Banco de dados:** manter estratégia própria de backup/restore antes de migrações; rollback de Hosting **não reverte dados**.
- **Falha de CI:** não contornar o check obrigatório. Corrigir a branch e reexecutar os testes.
- **Comprometimento:** revogar permissões da conta de serviço/WIF, revisar logs de auditoria e rodar novamente os testes de segurança.

## 9. Fontes oficiais para manutenção

- GitHub Rulesets: https://docs.github.com/en/repositories/configuring-branches-and-merges-in-your-repository/managing-rulesets/available-rules-for-rulesets
- Firebase CLI: https://firebase.google.com/docs/cli
- Firebase Hosting: https://firebase.google.com/docs/hosting
- Firebase Security Rules Testing: https://firebase.google.com/docs/rules/unit-tests
- Workload Identity Federation: https://docs.cloud.google.com/iam/docs/workload-identity-federation-with-deployment-pipelines
- Google GitHub Auth Action: https://github.com/google-github-actions/auth
