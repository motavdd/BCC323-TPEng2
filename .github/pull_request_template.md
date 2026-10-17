## Objetivo

Descreva o problema e a solução.

## Issue

Closes #NUMERO

## Mudanças

- Descreva as alterações relevantes.

## Como testar

Informe os comandos executados, o resultado e os passos para conferir o comportamento.
Use somente as verificações aplicáveis à alteração:

- Verificação rápida no Windows: `./scripts/check-local.ps1`.
- Integração nativa com os três serviços ativos: `./scripts/check-local.ps1 -Integration`.
- Alternativa completa em Docker: `./scripts/test-all.ps1` ou `bash scripts/test-all.sh`.
- Para documentação, confira links e instruções; descreva essa validação.

Os testes de regras limpam o Firestore emulado do projeto de demonstração.
Consulte [o guia local](../docs/DESENVOLVIMENTO_LOCAL.md) para iniciar os serviços.

O CI executa em Docker; não é necessário executar Docker localmente para abrir o PR.

## Checklist

- [ ] Origem e destino seguem o fluxo da tarefa ou a promoção `Develop → Staging → Master`
- [ ] Alteração atende à Issue vinculada
- [ ] Validações aplicáveis descritas acima
- [ ] Testes de comportamento adicionados/atualizados quando necessário
- [ ] Nenhum segredo incluído
- [ ] Documentação atualizada
- [ ] README da branch de destino preservado, quando for promoção
- [ ] Checks `backend`, `quality`, `integration` e `e2e` aprovados antes do merge
- [ ] Solicitei revisão
