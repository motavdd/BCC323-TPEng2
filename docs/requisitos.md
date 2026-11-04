# Histórias de Utilizador - Sistema de Planejamento e Organização Acadêmica

**Disciplina:** BCC323 - Engenharia de Software 2

**Instituição:** Universidade Federal de Ouro Preto (UFOP) - DECOM

**Autores:** César Augusto, Ciro Junio, Jhonata, Juliana Borges, Luisa Notaro, Matheus Mota, Samara Paloma

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

## Detalhamento das Histórias de Utilizador

### RF01 - Autenticação e Cadastro do Usuário

* **História:** Como **Aluno**, eu quero me cadastrar e autenticar no sistema informando meu curso e período atual, para que eu possa ter um perfil personalizado com as disciplinas do meu curso.

* **Prioridade:** **Alta** *(Essencial para o funcionamento básico do sistema)*.

### RF02 - Registro de Disciplinas Concluídas

* **História:** Como **Aluno**, eu quero selecionar e marcar as disciplinas que já concluí em semestres anteriores, para que o sistema saiba o meu histórico e valide os pré-requisitos das matérias futuras.

* **Prioridade:** **Alta** *(Essencial para calcular o progresso e filtrar opções)*.

### RF03 - Visualização do Progresso Acadêmico

* **História:** Como **Aluno**, eu quero visualizar uma barra de progresso com a porcentagem de integralização do meu curso e a separação das disciplinas (concluídas, em andamento e disponíveis), para que eu possa acompanhar meu avanço acadêmico de forma clara.

* **Prioridade:** **Média** *(Importante para a experiência visual do usuário, porém depende do cadastro inicial)*.

### RF04 - Atualização de Quadro de Horários e Matriz Curricular

* **História:** Como **Coordenador do Curso**, eu quero cadastrar e atualizar as disciplinas oferecidas no período letivo (com horários, turmas e vagas), para que os alunos tenham acesso às informações atualizadas para montagem da grade.

* **Prioridade:** **Alta** *(Essencial, pois alimenta os dados utilizados pelos alunos)*.

### RF05 - Simulação e Montagem da Grade Horária

* **História:** Como **Aluno**, eu quero selecionar as disciplinas disponíveis que pretendo cursar no novo período, para que eu monte a minha proposta de grade sem conflito de horários.

* **Prioridade:** **Alta** *(Funcionalidade principal do sistema)*.

### RF06 - Visualização da Rotina Semanal

* **História:** Como **Aluno**, eu quero visualizar uma agenda semanal com os horários e dias das aulas escolhidas, para que eu possa organizar minha rotina pessoal e acadêmica.

* **Prioridade:** **Média** *(Importante para uso prático após a escolha das matérias)*.

### RF07 - Publicação da Proposta de Grade no Fórum

* **História:** Como **Aluno**, eu quero publicar a minha proposta de grade horária em um fórum privado, para pedir a opinião de outros estudantes do mesmo curso sobre o nível de dificuldade/carga de trabalho do semestre.

* **Prioridade:** **Média** *(Funcionalidade diferencial e importante do ecossistema, mas não impede a montagem individual da grade na entrega inicial)*.

### RF08 - Comentários e Avaliações nas Publicações

* **História:** Como **Aluno**, eu quero comentar nas propostas de grade de outros colegas, para compartilhar minha experiência sobre o peso de determinado conjunto de disciplinas.

* **Prioridade:** **Média** *(Completa a dinâmica do fórum de discussão)*.

### RF09 - Controle de Privacidade da Publicação

* **História:** Como **Aluno**, eu quero definir quem pode visualizar e comentar na minha publicação, para garantir a minha privacidade.

* **Prioridade:** **Baixa** *(Pode ser implementada em versões futuras após o funcionamento básico do fórum)*.