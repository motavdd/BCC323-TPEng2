# Frontend React

A aplicação usa React com TypeScript e Vite. A entrada é `src/main.tsx`, que
monta `App` em `StrictMode`.

## Pastas

```text
src/
  app/                 App.tsx e estilo do carregamento da sessão
  pages/
    Access/            Página de login e cadastro, com Access.css
    planner/           Composição do planejador, navegação e telas
      OverviewScreen/  Resumo e seu CSS
      MatrixScreen/    Matriz curricular e seu CSS
      HistoryScreen/   Histórico
      SubjectScreen/   Detalhes e conclusão de disciplina
      PlanScreen/      Planejamento e seu CSS
      SavedPlanScreen/ Confirmação do plano salvo
  components/
    Panel/             Painel reutilizável e seu CSS
    ProfileForm/       Formulário de perfil e seu CSS
    Week/              Grade semanal e seu CSS
  hooks/               Estado do planejador e navegação pelo hash
  services/            API HTTP, Firebase e persistência de planos
  styles/              Base global e estilos compartilhados
  utils/               Funções de horários e mensagens de erro
  test/                Configuração dos testes
```

## Onde colocar cada alteração

- Interface de uma tela: na pasta correspondente em `pages/`.
- Componente reutilizável: em `components/Nome/Nome.tsx`.
- Estilo específico: em `Nome.css`, ao lado do componente, importado no TSX.
- Estado e efeitos compartilhados: em `hooks/`.
- Chamadas acadêmicas: em `services/academic.ts`.
- Persistência do plano: em `services/planStorage.ts`.
- Lógica sem React: em `utils/`.

`styles/global.css` contém Tailwind, variáveis, tipografia e padrões dos elementos
HTML. `styles/shared.css` contém avisos, cores de estado, botões de texto e grupos
de ações usados por várias telas. Ambos entram antes dos estilos dos componentes
em `main.tsx`. As regras responsivas específicas ficam no CSS do componente.
Telas que usam apenas componentes e estilos compartilhados não precisam de um
arquivo CSS vazio.

Os componentes continuam sendo funções React com propriedades tipadas. As telas
recebem dados e ações; não fazem chamadas diretas à API ou ao Firestore.
Identificadores de código seguem os contratos existentes em inglês; os textos da
interface ficam em português.

## Verificação

Execute na pasta `frontend`, após instalar as dependências:

```powershell
bun run typecheck
bun run lint
bun run test:unit
bun run build
```

Para iniciar os serviços e emuladores, veja o [guia de contribuição](../docs/CONTRIBUICAO.md).
