# Segurança e limites de responsabilidade

## Firebase no navegador

A configuração web (`apiKey`, `projectId`, `appId`, `authDomain`) é entregue ao navegador e não substitui regras de autorização. O acesso aos dados deve ser protegido pelas **Firestore Security Rules** e por identidade autenticada. Restrinja API keys conforme orientação do Google Cloud e habilite somente os serviços necessários.

## Regras deste scaffold

- `users/{uid}`: o usuário autenticado lê/cria/atualiza somente o próprio perfil, com validação de campos; não pode excluir.
- `users/{uid}/completedCourses/{courseId}`: somente o proprietário lê e modifica; o schema ainda é mínimo.
- `courses/{id}`: usuários autenticados podem consultar; ninguém pode escrever via SDK cliente.
- Qualquer outro caminho: acesso negado por padrão.

**Não usar essas regras como produto final** antes de definir os requisitos de coordenadores, histórico, publicação de grades, comentários e privacidade. O protótipo de UI demonstra autenticação, não autorização administrativa.

## Separação de ambientes

- Testes usam projeto `demo-bcc323` e emuladores locais, sem credenciais reais.
- Staging e produção devem ter projetos Firebase diferentes, incluindo Auth, Firestore e Hosting.
- Não importar dados reais de estudantes para emuladores/ambiente de homologação.
- Não adicionar secrets ou chaves de conta de serviço ao Git.
- Não expor portas dos emuladores para a internet.

## Deploy e IAM

- GitHub Actions de CI: somente `contents: read`.
- Jobs de deploy: `contents: read` + `id-token: write`.
- Autenticação cloud: Workload Identity Federation, limitada ao repositório e às branches autorizadas.
- Contas de serviço por ambiente, papéis mínimos e Environment `production` com aprovação.
- Publicação de regras Firestore manual, separada de Hosting.

## Override de gRPC

O manifesto e o lockfile recebidos já fixavam `@grpc/grpc-js` em **1.14.6**. Esse override foi mantido: o Firestore 4.18.0 declara `~1.9.0`, limitando a resolução à série 1.9.x, anterior às correções citadas abaixo. A conversa anterior registrava 1.9.16 instalada. Mudar de gerenciador não corrige isso automaticamente.

Os avisos oficiais [GHSA-m9gg-hp2v-232j](https://github.com/grpc/grpc-node/security/advisories/GHSA-m9gg-hp2v-232j) e [GHSA-f596-whhp-79r4](https://github.com/grpc/grpc-node/security/advisories/GHSA-f596-whhp-79r4) indicam correções na linha 1.14 a partir de 1.14.5 e 1.14.6, respectivamente. A versão fixada cobre ambos. Os cenários descritos envolvem servidores gRPC; o aplicativo usa SDK cliente no navegador. Isso reduz a exposição direta a esses cenários, mas não justifica manter uma dependência vulnerável nas ferramentas Node.

O override ultrapassa a faixa `~1.9.0` exigida pelo SDK. Mesmo dentro da mesma versão principal, isso exige validar o cliente Firestore e os testes de regras com emulador; lint/build sozinhos não comprovam compatibilidade. Confira os resultados desta execução no relatório. Não foi feito downgrade de Firebase nem troca do Vitest por `bun test`.

Reavalie o override ao atualizar Firebase: remova-o temporariamente numa cópia, regenere o lockfile com Bun e confira se todas as resoluções de gRPC já incluem as correções necessárias. Só então remova-o definitivamente e repita integração e E2E. Consulte `bun audit` para outros avisos; os dois avisos acima não são uma auditoria completa.

## Riscos ainda a tratar

1. **Coordenador:** nunca confiar em `role=coordinator` vindo do frontend. Usar claims atribuídas por serviço privilegiado ou API backend com checagem de identidade e autorização.
2. **Privacidade do fórum:** definir quem lê publicações, quem comenta, quem edita/apaga e como bloquear acesso cruzado entre cursos.
3. **Histórico acadêmico:** evitar dados pessoais desnecessários e definir retenção/consentimento segundo LGPD.
4. **Integridade:** validação de pré-requisitos e conflito de horários não pode depender apenas de UI se outros clientes puderem gravar diretamente no Firestore.
5. **Abuso:** considerar App Check, limites de uso, monitoramento de custos e proteção contra cadastro automatizado.
6. **Cadeia de suprimentos:** atualizar dependências com Dependabot, revisar PRs de atualização e fixar Actions por SHA em fase de endurecimento.
7. **Backup:** configurar estratégia de recuperação de dados antes de liberar acesso a usuários reais.
