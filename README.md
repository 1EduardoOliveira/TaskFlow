# TaskFlow

**Gerenciador de tarefas desenvolvido com C, HTML, CSS e JavaScript.**

O TaskFlow é um projeto desenvolvido com o objetivo de praticar programação, desenvolvimento web e integração entre frontend e backend.

## Funcionalidades

- Cadastro de tarefas.
- Listagem de tarefas.
- Atualização de tarefas.
- Exclusão de tarefas.
- Identificação das tarefas por ID.
- Indicadores de tarefas totais, pendentes e concluídas.
- Interface com tema dark e layout responsivo.

## Tecnologias utilizadas

**Backend**
- Linguagem C
- Biblioteca Mongoose
- Servidor HTTP
- Estrutura de dados: lista simplesmente encadeada

**Frontend**
- HTML5
- CSS3
- JavaScript

## API

| Método | Rota | Operação |
|---|---|---|
| GET | `/tarefas` | Listar tarefas |
| POST | `/tarefas` | Cadastrar tarefa |
| GET | `/tarefas/:id` | Consultar tarefa |
| PUT | `/tarefas/:id` | Atualizar tarefa |
| DELETE | `/tarefas/:id` | Excluir tarefa |

## Estrutura do projeto

```text
web-gerenciador/
├── backend/
│   ├── src/
│   ├── include/
│   └── vendor/
├── frontend/
│   ├── css/
│   ├── js/
│   └── index.html
├── data/
├── .gitignore
└── README.md
```

## Objetivo

Projeto pessoal desenvolvido para aprendizado e evolução prática em programação, estruturas de dados e desenvolvimento web.

## Autor

Eduardo Gabriel