# Requisitos - Sistema de Planejamento e Organização Acadêmica

**Disciplina:** BCC323 - Engenharia de Software 2

**Instituição:** Universidade Federal de Ouro Preto (UFOP) - DECOM

**Autores:** César Augusto, Ciro Junio, Jhonata, Juliana Borges, Luisa Notaro, Matheus Mota, Samara Paloma

## 1. Visão do Produto
O sistema de Planejamento e Organização Acadêmica resolve a dificuldade dos estudantes universitários em montar e planejar suas grades horários semestrais sem conflitos de horários e respeitando os pré-requisitos das disciplinas. Voltado para alunos e coordenadores de curso, o sistema permite que os estudantes simulem horários, visualizem sua rotina semanal e compartilhem propostas de grade em fórum para receberem feedback de colegas sobre a carga de trabalho do semestre. Enquanto a função dos coordenadores dos cursos é na gestão de disciplinas na grade curricular, editando horários, turmas, capacidade...

## 2. Papéis de Usuário
| Papel | Descrição | 
| ----- | ----- |
| Aluno | Estudante da instituição que utiliza o sistema para gerenciar seu histórico acadêmico, simular e montar sua grade horária, visualizar sua rotina e interagir no fórum de propostas de grade.| 
| Coordenador| Administrador do curso responsável pelo cadastramento, atualização da matriz curricular, turmas, vagas e ofertas de horários das disciplinas oferecidas a cada período letivo.

## 3. Histórias de Usuário

### H01 - Autenticação e Cadastro do Usuário (Essencial)

**História:** Como **Aluno**, eu quero me cadastrar e autenticar no sistema informando meu curso e período atual, para que eu possa ter um perfil personalizado com as disciplinas do meu curso.

**Critérios de aceitação:**
- [ ] O sistema deve permitir o cadastro informando nome, e-mail institucional único, senha, curso e período atual.
- [ ] O sistema deve validar as credenciais no login e bloquear o acesso a e-mails não cadastrados ou senhas incorretas.

### H02 - Registro de Disciplinas Concluídas (Essencial)

**História:** Como **Aluno**, eu quero selecionar e marcar as disciplinas que já concluí em semestres anteriores, para que o sistema saiba o meu histórico e valide os pré-requisitos das matérias futuras.

**Critérios de aceitação:**
- [ ] O sistema deve exibir a lista de disciplinas da matriz curricular do curso para seleção.
- [ ] O sistema deve salvar o histórico de matérias concluídas e proibir a seleção de disciplinas cujos pré-requisitos não foram atendidos.

### H03 - Visualização do Progresso Acadêmico (Importante)

**História:** Como **Aluno**, eu quero visualizar uma barra de progresso com a porcentagem de integralização do meu curso e a separação das disciplinas (concluídas, em andamento e disponíveis), para que eu possa acompanhar meu avanço acadêmico de forma clara.

**Critérios de aceitação:**
- [ ] A barra de progresso deve calcular e exibir automaticamente a porcentagem de horas/créditos concluídos em relação ao total exigido pelo curso.
- [ ] As disciplinas devem ser exibidas organizadas visualmente em três categorias distintas: concluídas, em andamento e disponíveis para matrícula.

### H04 - Atualização de Quadro de Horários e Matriz Curricular (Essencial)

**História:** Como **Coordenador do Curso**, eu quero cadastrar e atualizar as disciplinas oferecidas no período letivo (com horários, turmas e vagas), para que os alunos tenham acesso às informações atualizadas para montagem da grade.

**Critérios de aceitação:**
- [ ] O coordenador deve conseguir criar, editar e remover disciplinas, informando código, nome, horários, turmas, limite de vagas e pré-requisitos.
- [ ] As alterações efetuadas pelo coordenador devem refletir imediatamente nas opções de escolha para montagem de grade dos alunos.

### H05 - Simulação e Montagem da Grade Horária (Essencial)

**História:** Como **Aluno**, eu quero selecionar as disciplinas disponíveis que pretendo cursar no novo período, para que eu monte a minha proposta de grade sem conflito de horários.

**Critérios de aceitação:**
- [ ] O sistema deve impedir a adição de disciplinas cujos pré-requisitos não tenham sido concluídos previamente pelo aluno.
- [ ] O sistema deve identificar e alertar graficamente qualquer choque de horário entre disciplinas selecionadas, bloqueando a confirmação da grade até a resolução do conflito.

### H06 - Visualização da Rotina Semanal (Importante)

**História:** Como **Aluno**, eu quero visualizar uma agenda semanal com os horários e dias das aulas escolhidas, para que eu possa organizar minha rotina pessoal e acadêmica.

**Critérios de aceitação:**
- [ ]  O sistema deve renderizar uma grade horária semanal (de segunda a sábado) mapeando os blocos de aulas das disciplinas selecionadas.
- [ ] O aluno deve conseguir exportar ou visualizar os detalhes de cada bloco de aula (nome da matéria, sala, turma e horário).

### H07 - Publicação da Proposta de Grade no Fórum (Importante)

**História:** Como **Aluno**, eu quero publicar a minha proposta de grade horária em um fórum privado, para pedir a opinião de outros estudantes do mesmo curso sobre o nível de dificuldade/carga de trabalho do semestre.

**Critérios de aceitação:**
- [ ]  O aluno deve conseguir enviar sua proposta de grade diretamente para a seção de fórum do curso com um título e uma descrição opcional.
- [ ] A publicação deve exibir a representação gráfica das matérias e horários escolhidos pelo autor.

### H08 - Comentários e Avaliações nas Publicações (Importante)

**História:** Como **Aluno**, eu quero comentar nas propostas de grade de outros colegas, para compartilhar minha experiência sobre o peso de determinado conjunto de disciplinas.

**Critérios de aceitação:**
- [ ]  O autor do comentário deve poder editar ou excluir seus próprios comentários.

### H09 - Controle de Privacidade da Publicação (Desejável)

**História:** Como **Aluno**, eu quero definir quem pode visualizar e comentar na minha publicação, para garantir a minha privacidade.

**Critérios de aceitação:**
- [ ]  O aluno deve poder escolher o nível de visibilidade da sua publicação (Público para o curso, Apenas Amigos/Seguidores ou Privado).

## 4. Priorização de Requisitos
| Prioridade | Histórias |
|------------|-----------|
| Essencial | H01, H02, H04, H05 |
| Importante | H03, H06, H07, H08 |
| Desejável | H09 |
| Fora do escopo | - |

## 5. Requisitos Não Funcionais
| ID | Requisito |
|----|-----------|
| RNF01 | **Desempenho**: O tempo de resposta para validação de pré-requisitos e detecção de conflito de horários durante a montagem da grade deve ser inferior a 1 segundo.|
| RNF02 | **Segurança**: Todas as senhas dos usuários devem ser armazenadas utilizando algoritmos de hash seguros.|
| RNF03 | **Usabilidade**: A interface de visualização do progresso e da grade semanal deve ser responsiva e adaptada para navegação em dispositivos móveis e desktops.|
| RNF04| **Confiabilidade e Consistência**: O sistema deve garantir a integridade referencial dos dados, evitando a deleção de disciplinas ativas vinculadas a históricos de alunos.|


## Tabela Resumo

| ID | Título | Ator | Prioridade | 
| ----- | ----- | ----- | ----- | 
| **RF01** | Autenticação e Cadastro do Usuário | Aluno | Alta | 
| **RF02** | Registro de Disciplinas Concluídas | Aluno | Alta | 
| **RF03** | Visualização do Progresso Acadêmico | Aluno | Média | 
| **RF04** | Atualização de Quadro de Horários e Matriz Curricular | Coordenador | Alta | 
| **RF05** | Simulação e Montagem da Grade Horária | Aluno | Alta | 
| **RF06** | Visualização da Rotina Semanal | Aluno | Média | 
| **RF07** | Publicação da Proposta de Grade no Fórum | Aluno | Média | 
| **RF08** | Comentários e Avaliações nas Publicações | Aluno | Média | 
| **RF09** | Controle de Privacidade da Publicação | Aluno | Baixa | 
