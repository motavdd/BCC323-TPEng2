# 🎓 Sistema de Planejamento e Organização Acadêmica

> Plataforma para estruturar, visualizar e acompanhar o progresso acadêmico ao longo dos semestres.

[🔗 Repositório no GitHub](https://github.com/motavdd/BCC323-TPEng2) · [📋 Quadro Kanban](https://github.com/users/motavdd/projects/7)

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

> 📍 Este README está na branch **`staging`**, usada para homologação. **Não se desenvolve aqui.**

Todo o trabalho da equipe é organizado por **Issues** e pelo **Quadro Kanban**.

### Regras da `staging`

- A branch é **protegida**: não há push direto nem criação de branches a partir dela.
- Ela só recebe código vindo da **`develop`**, por Pull Request.
- Serve para validar o que foi desenvolvido antes de seguir para a `master`.

### Fluxo

1. Uma versão estável da `develop` é promovida para a `staging` via Pull Request.
2. A equipe testa e valida as funcionalidades na `staging`.
3. Se for encontrado um problema, **abra uma Issue** descrevendo-o e mova o card no Kanban. A correção é feita em uma branch criada **a partir da `develop`**, nunca da `staging`, e volta pelo mesmo caminho.
4. Com tudo validado, a `staging` é promovida para a `master` via Pull Request.

### Como desenvolver

Para contribuir com código, use a branch **`develop`**: consulte o README dela para o passo a passo (criação de branch, Pull Request e convenções).

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
