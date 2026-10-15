# Segurança

## Firebase no navegador

A configuração web (`apiKey`, `projectId`, `appId`, `authDomain`) é entregue ao navegador e não substitui regras de autorização. O acesso aos dados deve ser protegido pelas **Firestore Security Rules** e por identidade autenticada. Restrinja API keys conforme orientação do Google Cloud e habilite somente os serviços necessários.

## Regras de acesso

- `users/{uid}`: o usuário autenticado lê/cria/atualiza somente o próprio perfil, com validação de campos; não pode excluir.
- `users/{uid}/completedCourses/{courseId}`: somente o proprietário lê e modifica; o schema ainda é mínimo.
- `courses/{id}`: usuários autenticados podem consultar; ninguém pode escrever via SDK cliente.
- Qualquer outro caminho: acesso negado por padrão.

As regras cobrem o escopo implementado de perfis e os caminhos de catálogo e disciplinas concluídas. As operações de coordenação, publicação de grades, comentários e privacidade não possuem autorização implementada. A interface atual oferece autenticação e não inclui administração.

## Separação de ambientes

- Testes usam projeto `demo-bcc323` e emuladores locais, sem credenciais reais.
- Homologação utiliza `spoa-eng2-staging`; produção utiliza `spoa-eng2-prod`.
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

O manifesto e o lockfile fixam `@grpc/grpc-js` em **1.14.6** por meio de um override. Essa configuração controla a resolução da dependência utilizada pelas ferramentas de Firestore no ambiente Node.

Os avisos oficiais [GHSA-m9gg-hp2v-232j](https://github.com/grpc/grpc-node/security/advisories/GHSA-m9gg-hp2v-232j) e [GHSA-f596-whhp-79r4](https://github.com/grpc/grpc-node/security/advisories/GHSA-f596-whhp-79r4) indicam correções na linha 1.14 a partir de 1.14.5 e 1.14.6, respectivamente. A versão fixada cobre ambos. Os cenários descritos envolvem servidores gRPC; o aplicativo usa SDK cliente no navegador. Isso reduz a exposição direta a esses cenários, mas não justifica manter uma dependência vulnerável nas ferramentas Node.

Alterações no override exigem validação do cliente Firestore e dos testes de regras com emulador. A bateria executada em 14/10/2026 incluiu integração e E2E com essa configuração e foi concluída com sucesso.

O procedimento de atualização inclui revisão de `package.json` e `bun.lock`, análise com `bun audit` e execução dos testes de integração e E2E. As referências acima tratam de dois avisos específicos e não representam uma auditoria completa das dependências.

## Requisitos de segurança dos módulos acadêmicos

1. **Coordenador:** nunca confiar em `role=coordinator` vindo do frontend. Usar claims atribuídas por serviço privilegiado ou API backend com checagem de identidade e autorização.
2. **Privacidade do fórum:** definir quem lê publicações, quem comenta, quem edita/apaga e como bloquear acesso cruzado entre cursos.
3. **Histórico acadêmico:** evitar dados pessoais desnecessários e definir retenção/consentimento segundo LGPD.
4. **Integridade:** validação de pré-requisitos e conflito de horários não pode depender apenas de UI se outros clientes puderem gravar diretamente no Firestore.
5. **Abuso:** a proteção contra cadastro automatizado, o monitoramento de custos e os limites de uso dependem de configuração específica; o código atual não implementa esses controles.
6. **Dependências:** o repositório contém configuração do Dependabot; as atualizações passam por PR e CI.
7. **Backup:** configurar estratégia de recuperação de dados antes de liberar acesso a usuários reais.
