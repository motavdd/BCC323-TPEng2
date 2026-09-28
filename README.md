# 🎓 Sistema de Planejamento e Organização Acadêmica

> Plataforma para estruturar, visualizar e acompanhar o progresso acadêmico ao longo dos semestres.

[🔗 Repositório no GitHub](#) · [📋 Quadro Kanban](#)

---

## Sobre o Projeto

O **Sistema de Planejamento e Organização Acadêmica** é uma solução voltada ao ambiente universitário que ajuda estudantes a planejarem sua trajetória no curso.
A plataforma centraliza as informações sobre disciplinas **concluídas**, **em andamento** e **pendentes**, facilita a montagem da grade horária e promove a troca de experiências entre os próprios alunos.

## Problema

Muitos estudantes têm dificuldade em planejar sua rotina acadêmica por não saberem ao certo o esforço exigido por determinado conjunto de disciplinas. O sistema reduz esse problema combinando **controle visual do progresso** com **colaboração entre alunos**.

## Atores

| Ator | Responsabilidades |
| --- | --- |
| **Aluno** | Realiza o autocadastro (curso, período atual e histórico de disciplinas cursadas); monta a grade, acompanha o progresso e participa do fórum de discussão. |
| **Coordenador do Curso** (Administrador) | Mantém atualizadas as disciplinas do curso a cada período letivo, incluindo horários e pré-requisitos. |

## Funcionalidades

| Funcionalidade | Descrição |
| --- | --- |
| **Cadastro e Perfis** | O aluno informa seus dados acadêmicos e marca as disciplinas já concluídas. |
| **Painel de Progresso** | Barra de progresso do curso e categorização visual das matérias em três estados: *Concluídas*, *Em Andamento* e *Disponíveis*. |
| **Simulador de Grade** | Seleção das disciplinas que o aluno pretende cursar no período. |
| **Agenda Semanal Personalizada** | Geração automática de uma grade de horários (segunda a sexta) com base nas turmas escolhidas. |
| **Publicação da Grade** | O aluno publica sua proposta de grade horária para o semestre. |
| **Feedback** | Outros estudantes do mesmo curso comentam a publicação, opinando se a combinação de matérias é pesada ou equilibrada. |

## Estrutura do Repositório

| Diretório | Conteúdo |
| --- | --- |
| `src/` | Código-fonte com as implementações das funcionalidades e cabeçalhos |
| `bin/` | Binários e executáveis |
| `test/` | Testes funcionais e de regressão |
| `doc/` | Documentação técnica |

## Como Executar

> _Preencher: pré-requisitos, comandos de compilação e execução._

```bash
# exemplo
git clone [<url-do-repositorio>(https://github.com/motavdd/BCC323-TPEng2.git)
cd <nome-do-repositorio>
# comandos de build e execução
```

## Guia de Contribuição

>  Este README está na branch **`develop`**, onde acontece o desenvolvimento do dia a dia.

Todo o trabalho da equipe é organizado por **Issues** e pelo **Quadro Kanban**. Antes de escrever código, leia esta seção.

### Branches

| Branch | Finalidade | Proteção |
| --- | --- | --- |
| `master` | Versão estável, pronta para entrega | Protegida |
| `staging` | Homologação e validação antes de ir para a `master` | Protegida |
| `develop` | Integração do desenvolvimento; **única origem de novas branches** | Base de trabalho |

> ⚠️ Não é possível fazer push direto em `master` nem em `staging`. Novas branches devem ser criadas **sempre a partir da `develop`**.

### Fluxo de Trabalho

1. **Escolha ou crie uma Issue** e mova o card correspondente no Kanban para *Doing*, atribuindo-o a você.
2. **Atualize sua `develop` local** e crie a branch da tarefa a partir dela:
   ```bash
   git checkout develop
   git pull origin develop
   git checkout -b feat/<numero-da-issue>-<descricao-curta>
   ```
3. **Desenvolva** em commits pequenos e com mensagens claras.
4. **Abra um Pull Request para a `develop`**, referenciando a issue (ex.: `Closes #12`) e solicitando revisão de pelo menos um colega.
5. **Após o merge**, mova o card para *Concluído* e apague a branch da tarefa.
6. A promoção de código segue o caminho `develop` → `staging` → `master`, feita via Pull Request e somente após validação.

### Convenções Sugeridas

- **Nome de branch:** `feat/`, `fix/`, `doc/` ou `test/` + número da issue + descrição curta (ex.: `feature/12-painel-progresso`).
- **Mensagens de commit:** curtas e no imperativo (ex.: `Adiciona cálculo da barra de progresso`).
- **Pull Requests:** um assunto por PR, com descrição do que mudou e como testar.
- **Testes:** inclua ou atualize os testes em `test/` quando alterar comportamento.

## 👨‍💻 Equipe

| Nome | Matrícula |
| --- | --- |
| César Augusto Tiago Totô | 24.1.4038 |
| Ciro Junio | _a preencher_ |
| Jhonata | _a preencher_ |
| Juliana Borges | _a preencher_ |
| Luisa Notaro | _a preencher_ |
| Matheus Mota | 22.2.4101 |
| Samara Paloma | 22.2.4091 |

## 🔗 Links Úteis

- **GitHub:** https://github.com/motavdd/BCC323-TPEng2
- **Quadro Kanban:** https://github.com/users/motavdd/projects/7
