from pathlib import Path
from html import escape
from reportlab.pdfgen import canvas
from reportlab.platypus import SimpleDocTemplate, Paragraph, Spacer, Table, TableStyle, PageBreak, Preformatted, KeepTogether
from reportlab.lib import colors
from reportlab.lib.styles import ParagraphStyle
from reportlab.pdfbase import pdfmetrics
from reportlab.pdfbase.ttfonts import TTFont
from reportlab.lib.enums import TA_LEFT
import pypdfium2 as pdfium
from pypdf import PdfReader

ROOT = Path('D:/ufop/trabalhosEng/eng2')
OUT = ROOT / 'output/pdf/guia_endpoints_api_eng2.pdf'
OUT.parent.mkdir(parents=True, exist_ok=True)
QA = ROOT / 'tmp/pdf_endpoint_qa'
QA.mkdir(parents=True, exist_ok=True)
for name, file in [('Body', 'arial.ttf'), ('Bold', 'arialbd.ttf'), ('Mono', 'consola.ttf')]:
    pdfmetrics.registerFont(TTFont(name, 'C:/Windows/Fonts/' + file))
pdfmetrics.registerFontFamily('Body', normal='Body', bold='Bold', italic='Body', boldItalic='Bold')
WINE = colors.HexColor('#AB003E')
INK = colors.HexColor('#202024')
GRAY = colors.HexColor('#656575')
LINE = colors.HexColor('#E2E4E8')
PINK = colors.HexColor('#FBF0F5')
styles = {
    'title': ParagraphStyle('title', fontName='Bold', fontSize=25, leading=29, textColor=INK, spaceAfter=12),
    'h1': ParagraphStyle('h1', fontName='Bold', fontSize=20, leading=25, textColor=INK, spaceAfter=13),
    'h2': ParagraphStyle('h2', fontName='Bold', fontSize=12, leading=16, textColor=WINE, spaceBefore=12, spaceAfter=6),
    'body': ParagraphStyle('body', fontName='Body', fontSize=9.6, leading=14, textColor=INK, spaceAfter=8),
    'small': ParagraphStyle('small', fontName='Body', fontSize=8.1, leading=11.7, textColor=GRAY, spaceAfter=7),
    'cell': ParagraphStyle('cell', fontName='Body', fontSize=8.3, leading=11.4, textColor=INK),
    'cellhead': ParagraphStyle('cellhead', fontName='Bold', fontSize=8.3, leading=11.4, textColor=colors.white),
    'label': ParagraphStyle('label', fontName='Bold', fontSize=8.3, leading=12, textColor=WINE, spaceAfter=6),
    'code': ParagraphStyle('code', fontName='Mono', fontSize=7.6, leading=10.2, textColor=INK, leftIndent=0, rightIndent=0, spaceAfter=0),
}
story=[]
def p(text, style='body'):
    story.append(Paragraph(text, styles[style]))
def h(text): p(text, 'h2')
def code(text):
    text=text.strip('\n')
    for line in text.splitlines():
        if pdfmetrics.stringWidth(line, 'Mono', 7.6) > 475:
            raise ValueError('Linha de código longa: '+line)
    t=Table([[Preformatted(text,styles['code'])]], colWidths=[503])
    t.setStyle(TableStyle([('BACKGROUND',(0,0),(-1,-1),colors.HexColor('#F3F5F8')),('BOX',(0,0),(-1,-1),.5,LINE),('LEFTPADDING',(0,0),(-1,-1),12),('RIGHTPADDING',(0,0),(-1,-1),12),('TOPPADDING',(0,0),(-1,-1),10),('BOTTOMPADDING',(0,0),(-1,-1),10)]))
    story.extend([t,Spacer(1,9)])
def table(headers, rows, widths):
    data=[[Paragraph(escape(x),styles['cellhead']) for x in headers]]
    data += [[Paragraph(escape(str(x)).replace('\n','<br/>'),styles['cell']) for x in row] for row in rows]
    t=Table(data,colWidths=widths,repeatRows=1,hAlign='LEFT')
    t.setStyle(TableStyle([('BACKGROUND',(0,0),(-1,0),WINE),('VALIGN',(0,0),(-1,-1),'TOP'),('LEFTPADDING',(0,0),(-1,-1),9),('RIGHTPADDING',(0,0),(-1,-1),9),('TOPPADDING',(0,0),(-1,-1),7),('BOTTOMPADDING',(0,0),(-1,-1),7),('ROWBACKGROUNDS',(0,1),(-1,-1),[colors.white,colors.HexColor('#F6F6F8')]),('LINEBELOW',(0,1),(-1,-1),.4,LINE)]))
    story.extend([t,Spacer(1,9)])
def note(title,text):
    t=Table([[Paragraph('<b>'+title+'</b><br/>'+text,styles['body'])]],colWidths=[503])
    t.setStyle(TableStyle([('BACKGROUND',(0,0),(-1,-1),PINK),('LINEBEFORE',(0,0),(0,-1),3,WINE),('LEFTPADDING',(0,0),(-1,-1),12),('TOPPADDING',(0,0),(-1,-1),9),('BOTTOMPADDING',(0,0),(-1,-1),3)]))
    story.extend([t,Spacer(1,10)])
def page(number,title):
    if story: story.append(PageBreak())
    p(f'GUIA PRÁTICO  /  {number:02d}', 'label')
    p(title,'h1')

page(1,'Como criar endpoints<br/>e integrá-los à API')
p('Planejamento Acadêmico - TP Eng2', 'h2')
p('Guia para a equipe de C++ e React. Baseado nos arquivos locais do projeto em 20/10/2026. C++17, Crow, Firebase Authentication e Firestore.')
note('Objetivo', 'Acrescentar uma operação à API sem colocar HTTP nas entidades nem duplicar as regras acadêmicas nas telas. O exemplo será consultar uma disciplina pelo código.')
h('O que é um endpoint?')
p('É a combinação de um <b>método HTTP</b>, um <b>caminho</b> e um <b>contrato de entrada e saída</b>. Uma função C++ só fica acessível ao front quando uma rota registrada no Crow chama essa função e transforma o resultado em uma resposta HTTP.')
table(['Método','Uso típico','Exemplo'],[
('GET','Consultar, sem alterar dados','GET /api/curriculum'),
('POST','Criar um recurso ou executar uma ação','Exemplo futuro: criar disciplina'),
('PUT','Criar/substituir o recurso identificado; pode ser idempotente','PUT /api/history/BCC201'),
('DELETE','Excluir o recurso identificado','DELETE /api/history/BCC201')],[61,204,238])
h('O que já existe no MVP')
table(['Caminho','Operações disponíveis'],[
('/api/me','GET: identidade validada pelo token'),
('/api/courses','GET: cursos cadastrados'),
('/api/profile','GET e PUT: consultar e salvar o próprio perfil'),
('/api/curriculum','GET: matriz do curso do aluno'),
('/api/history','GET: conclusões persistidas'),
('/api/history/{codigo}','PUT e DELETE: registrar/remover conclusão'),
('/api/academic/summary','GET: progresso, horas e elegibilidade')],[193,310])
p('A rota pública GET /api/health verifica o processo. As operações acadêmicas exigem Authorization: Bearer &lt;ID token&gt;.', 'small')

page(2,'1. Escolha a camada certa')
p('As pastas de backend/include/ e backend/src/ espelham as mesmas responsabilidades. O cabeçalho declara o contrato; o .cpp implementa o comportamento.')
table(['Camada','Responsabilidade','Exemplos'],[
('domain/','Entidades e regras acadêmicas; sem Crow ou Firebase.','AlunoImpl, CursoImpl, DisciplinaImpl'),
('application/','Orquestrar a operação e usar o repositório.','AcademicService; AcademicRepository'),
('http/','Receber a requisição, validar JSON e retornar JSON.','academic_routes.cpp; AcademicJson.cpp'),
('infrastructure/auth/','Validar o token e obter a identidade.','RequestAuthenticator.cpp'),
('infrastructure/firestore/','Executar leitura e escrita no Firestore.','FirestoreRepository.cpp'),
('config/','Configurar host e porta do processo.','config.cpp')],[112,236,155])
h('Como a chamada percorre o projeto')
p('<b>React envia o token</b> → a rota autentica a requisição → o serviço executa o caso de uso → o repositório acessa o Firestore → o serviço utiliza o domínio → a rota serializa a resposta.')
p('O UID vem de RequestContext, criado a partir do token validado. Não aceite uid, userId ou um papel de coordenador enviados pelo navegador como prova de identidade ou autorização.')
h('Antes de escrever código, defina o contrato')
table(['Decisão','Exemplo didático a implementar'],[
('Operação','Consultar uma disciplina da matriz do aluno'),
('Método e caminho','GET /api/subjects/{codigo}'),
('Entrada','Código no caminho, por exemplo BCC201'),
('Autorização','Aluno autenticado; somente sua matriz'),
('Saída','id, name, hours, period, prerequisites, requirementsKnown'),
('Erros principais','401: sem autenticação; 409: perfil incompleto; 404: disciplina ausente')],[134,369])
note('Exemplo novo, não endpoint já entregue', 'As páginas seguintes mostram alterações a fazer. O guia não adiciona /api/subjects/{codigo} ao repositório automaticamente. O contrato atual está em backend/openapi.json.')

page(3,'2. Implemente serviço e saída JSON')
h('A. Declare o método no serviço')
p('Em backend/include/application/AcademicService.hpp, dentro de public: de AcademicService, acrescente:', 'small')
code('SubjectRecord subject(const RequestContext& context,\n                      const std::string& id);')
h('B. Implemente o caso de uso')
p('Em backend/src/application/AcademicService.cpp, dentro de namespace bcc323. O arquivo já inclui &lt;algorithm&gt;. curriculum(context) carrega o curso do perfil autenticado.', 'small')
code('''SubjectRecord AcademicService::subject(
    const RequestContext& context, const std::string& id) {
    const auto catalog = curriculum(context);
    const auto found = std::find_if(
        catalog.subjects.begin(), catalog.subjects.end(),
        [&](const SubjectRecord& s) { return s.id == id; });
    if (found == catalog.subjects.end()) {
        throw AppError(404, "subject_not_found",
                       "Disciplina fora da matriz do seu curso.");
    }
    return *found;
}''')
h('C. Acrescente a conversão para JSON')
p('Declare em backend/include/http/AcademicJson.hpp e implemente em backend/src/http/AcademicJson.cpp, dentro de namespace bcc323:', 'small')
code('''// Declaracao no .hpp:
crow::json::wvalue toJson(const SubjectRecord& subject);

// Implementacao no .cpp:
crow::json::wvalue toJson(const SubjectRecord& s) {
    crow::json::wvalue out;
    out["id"] = s.id;
    out["name"] = s.name;
    out["hours"] = s.hours;
    out["period"] = s.period;
    out["prerequisites"] = s.prerequisites;
    out["requirementsKnown"] = s.requirementsKnown;
    return out;
}''')
p('Essa consulta usa o repositório já existente: não precisa criar uma consulta Firestore dentro da rota. O estado individual da disciplina continua disponível em GET /api/academic/summary.', 'small')

page(4,'3. Registre a rota no Crow')
p('Em backend/src/http/academic_routes.cpp, coloque o trecho abaixo <b>dentro de register_academic_routes(crow::SimpleApp&amp; app)</b>, junto às outras rotas:')
code('''CROW_ROUTE(app, "/api/subjects/<string>")
([](const crow::request& request, const std::string& id) {
    return handle(request,
        [&](const RequestContext& context,
            AcademicService& service) {
            return toJson(service.subject(context, id));
        });
});''')
p('O marcador &lt;string&gt; corresponde ao parâmetro id da função. Neste projeto, uma rota declarada sem .methods(...) usa GET. Para escrita, declare o método explicitamente.')
h('Por que usar handle(request, ...)?')
p('O helper já autentica, limita o corpo da requisição, cria o repositório e o serviço, trata AppError e falhas de autenticação e aplica Cache-Control: no-store. Ele está no namespace anônimo deste .cpp: <b>não pode ser chamado diretamente em outro arquivo</b>.')
h('Como essa rota entra no executável?')
code('''// src/main.cpp ja executa:
bcc323::register_routes(app);

// src/http/routes.cpp ja chama:
register_academic_routes(app);''')
p('Ao adicionar a rota na função já registrada, não altere main.cpp nem crie outro servidor. A próxima compilação incorpora a operação.')
h('Se precisar criar outro módulo de rotas')
p('Crie um .hpp em include/http/ e um .cpp em src/http/, declare a função register_... e chame-a a partir de register_routes. Antes, extraia o wrapper de autenticação/erros para uma função HTTP compartilhada; não suponha que handle está exportado.')
h('Atenção ao CMake')
p('Os .cpp em src/http/, src/domain/ e src/infrastructure/ são coletados pelos globs atuais. AcademicService.cpp está listado explicitamente. Se criar outro .cpp de aplicação, adicione-o ao alvo, por exemplo:', 'small')
code('''target_sources(academic-application PRIVATE
    src/application/SubjectService.cpp
)''')
p('Use esse trecho somente se o arquivo existir. Se apenas editar AcademicService.cpp, não é necessário alterar o CMake.', 'small')

page(5,'4. Operações que alteram dados')
p('Uma rota de escrita precisa validar entrada, aplicar regras, verificar autorização e confirmar persistência. Retornar sucesso sem salvar não conclui a operação.')
h('Modelo já existente: salvar o próprio perfil')
code('''// Dentro do handler de /api/profile:
const auto input = crow::json::load(request.body);
const auto profile = profileFromJson(input);
const auto saved = service.saveProfile(context, profile);
crow::json::wvalue body;
body["profile"] = toJson(saved);
return body;''')
p('Esse trecho reutiliza o parser atual. Ele valida os campos aceitos e o período inteiro de 1 a 20. O serviço resolve o curso cadastrado. O repositório grava usando o UID validado e usa timestamp do servidor ao criar o perfil.')
table(['Etapa','O que conferir'],[
('Entrada HTTP','JSON válido; tipos; campos obrigatórios; limites; campos extras; IDs.'),
('Serviço','Operação permitida; existência do recurso; regras das classes do domínio.'),
('Repositório','Caminho correto no Firestore; gravação confirmada; falhas propagadas.'),
('Resposta','Status coerente; JSON estável; mensagem que o front consegue tratar.')],[105,398])
h('Como evoluir a frente 2 da divisão de tarefas')
table(['Tarefa','Contratos possíveis - ainda a implementar'],[
('Cadastrar/editar disciplinas','POST /api/courses/{curso}/subjects; PUT /api/courses/{curso}/subjects/{codigo}'),
('Vincular pré-requisitos','PUT /api/courses/{curso}/subjects/{codigo}/prerequisites'),
('Turmas e horários','POST /api/courses/{curso}/offerings; PUT /api/courses/{curso}/offerings/{turma}')],[145,358])
note('Pré-condição: autorização administrativa', 'O MVP atual não define autorização de coordenador para editar o catálogo. Antes de expor essas escritas, definam uma fonte de papel confiável, a verificação no servidor e as permissões de persistência. Login sozinho não concede esse poder. Não basta liberar a rota ou confiar em role enviado pelo front.')
p('Ao incluir uma operação no repositório, atualize a interface AcademicRepository, a implementação FirestoreRepository e os repositórios falsos dos testes. Para requisitos, reutilize DisciplinaImpl::addPreRequisito, inclusive a prevenção de ciclos.', 'small')

page(6,'5. Consuma e teste a operação')
h('Acrescente o método no cliente do front')
p('Em frontend/src/lib/academic.ts, importe o tipo Subject de ./api e acrescente ao objeto academicApi:', 'small')
code('''subject: (id: string) =>
  api<Subject>(`/subjects/${encodeURIComponent(id)}`),''')
p('api() já acrescenta /api, obtém o token Firebase, envia Authorization e trata respostas de erro. A tela chama o cliente e mantém seus estados de carregamento, erro e resultado. Não coloque localhost:8081 nos componentes.')
code('''try {
  const subject = await academicApi.subject("BCC201");
  // Atualize o estado da tela com o resultado.
} catch (error) {
  // Mostre a mensagem e ofereca nova tentativa.
}''')
h('Prepare o ambiente local')
code('''# Terminais separados, a partir da raiz do projeto:
./scripts/dev-local.ps1 firebase
node scripts/seed-demo.mjs
./scripts/dev-local.ps1 backend
./scripts/dev-local.ps1 frontend''')
p('Execute o seed após os emuladores estarem ativos. Ele cadastra somente um recorte ilustrativo em projeto demo. Cadastre/entre com um aluno e complete o perfil. No Linux, use bash scripts/dev-local.sh para iniciar os serviços.', 'small')
h('Teste manual depois de implementar o exemplo')
code('''# Substitua pelo ID token de uma sessao de teste.
$token = '<ID_TOKEN>'
curl.exe 'http://127.0.0.1:8081/api/subjects/BCC201' `
  -H "Authorization: Bearer $token"''')
p('Também teste sem Authorization, com código inexistente e sem perfil completo. O esperado é 401, 404 e 409, respectivamente. Não coloque tokens reais no Git, na documentação nem em capturas compartilhadas.')
h('Adicionar ao código não significa publicar')
p('Os scripts rodam a API localmente. O proxy do Vite existe em desenvolvimento/preview, mas não acompanha o build estático. Para usar a nova rota no site publicado, será necessário publicar o C++ e configurar o encaminhamento remoto de /api, que ainda está pendente no projeto.')

page(7,'6. Testes e entrega para a equipe')
h('Teste o comportamento, não apenas o HTTP 200')
p('Em frontend/e2e/academic-api.spec.ts, dentro do teste que já prepara o aluno, o perfil e headers first, acrescente após implementar o novo endpoint:', 'small')
code('''const response = await request.get('/api/subjects/BCC201', {
  headers: first,
});
expect(response.status()).toBe(200);
expect(await response.json()).toMatchObject({ id: 'BCC201' });

const missing = await request.get('/api/subjects/INVALID', {
  headers: first,
});
expect(missing.status()).toBe(404);
expect((await request.get('/api/subjects/BCC201')).status())
  .toBe(401);''')
p('Para regras novas, acrescente testes em backend/test/application/. Para gravações, verifique persistência após nova consulta, isolamento entre usuários, falha de validação e comportamento em repetição da operação.', 'small')
h('Compilar e verificar no Windows')
code('''. ./scripts/local-env.ps1
cmake -S backend -B backend/build/native -DBUILD_TESTING=ON
cmake --build backend/build/native --parallel 2
ctest --test-dir backend/build/native --output-on-failure
# Com os servicos ativos, dentro de frontend/:
bun run typecheck
bun run lint
bun run test:e2e''')
p('Encerre o backend antes de recompilar no Windows: o executável aberto pode impedir a linkedição. Depois, reinicie-o. Testes HTTP em Python só são habilitados no CTest quando o interpretador está disponível.', 'small')
h('Checklist do PR')
table(['Verificação','Pronto quando...'],[
('Contrato','Método, caminho, entrada, saída, erros e autorização estão definidos em backend/openapi.json.'),
('Implementação','A rota foi registrada; serviço usa o domínio; persistência fica no repositório.'),
('Integração','Cliente academic.ts atualizado; testes de sucesso e falha passam.'),
('Equipe','Responsável da frente 2 ou 3 consegue usar o contrato sem alterar a arquitetura base.')],[100,403])
p('<b>Erros comuns:</b> 404 por esquecer o registro; 405 por método incorreto; 401 por token ausente; referência indefinida por esquecer um .cpp no CMake; dados não persistidos por só alterar objetos em memória.', 'small')
p('Referências do próprio projeto: backend/README.md; backend/CONTRATO_MVP.md; backend/openapi.json; frontend/src/lib/academic.ts. Os exemplos deste guia não foram aplicados automaticamente ao código.', 'small')

class NumberedCanvas(canvas.Canvas):
    def __init__(self,*args,**kwargs):
        canvas.Canvas.__init__(self,*args,**kwargs); self.states=[]
    def showPage(self):
        self.states.append(dict(self.__dict__)); self._startPage()
    def save(self):
        count=len(self.states)
        for state in self.states:
            self.__dict__.update(state)
            self.setStrokeColor(LINE); self.setLineWidth(.6); self.line(46,42,549,42)
            self.setFont('Body',8); self.setFillColor(GRAY)
            self.drawString(46,28,'TP ENG2  |  GUIA DE ENDPOINTS  |  20/10/2026')
            self.drawRightString(549,28,f'{self._pageNumber} / {count}')
            canvas.Canvas.showPage(self)
        canvas.Canvas.save(self)

doc=SimpleDocTemplate(str(OUT),pagesize=(595.2756,841.8898),leftMargin=46,rightMargin=46,topMargin=43,bottomMargin=57,title='Como criar endpoints e integrá-los à API - TP Eng2',author='Guia técnico da equipe TP Eng2')
doc.build(story,canvasmaker=NumberedCanvas)
reader=PdfReader(str(OUT))
print('PDF:',OUT,'paginas:',len(reader.pages))
if len(reader.pages)!=7:
    print([(i+1,(page.extract_text() or '')[:75]) for i,page in enumerate(reader.pages)])
    raise RuntimeError('Paginação inesperada; revisar o layout antes de entregar.')
pdf=pdfium.PdfDocument(str(OUT))
for index in range(len(pdf)):
    image=pdf[index].render(scale=1.35).to_pil()
    image.save(QA/f'page-{index+1}.png')
print('Renderizadas as 7 paginas para revisao visual.')
