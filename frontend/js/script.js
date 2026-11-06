
console.log("TaskFlow iniciado!");

// 1. ELEMENTOS DO HTML

let formulario = document.getElementById("form-task");
let campoTarefa = document.getElementById("task-title");
let listaTarefas = document.getElementById("tasks-container");

let totalTarefas = document.getElementById("total-tasks");
let tarefasPendentes = document.getElementById("pending-tasks");
let tarefasConcluidas = document.getElementById("completed-tasks");

let opcoesMenu = document.querySelectorAll(".menu a");


// 2. FUNÇÃO DOS CONTADORES

function atualizarContadores() {

    let cards = listaTarefas.querySelectorAll(".task-card");

    let total = cards.length;
    let concluidas = listaTarefas.querySelectorAll(".concluida").length;
    let pendentes = total - concluidas;

    totalTarefas.textContent = total;
    tarefasPendentes.textContent = pendentes;
    tarefasConcluidas.textContent = concluidas;

}

function atualizarMensagemVazia() {

    let mensagem = listaTarefas.querySelector(".empty-message");

    let possuiTarefas = listaTarefas.querySelector(".task-card");

    if (possuiTarefas) {

        if (mensagem) {
            mensagem.remove();
        }

    } else {

        if (!mensagem) {

            let paragrafo = document.createElement("p");

            paragrafo.classList.add("empty-message");
            paragrafo.textContent = "Nenhuma tarefa cadastrada.";

            listaTarefas.appendChild(paragrafo);

        }

    }

}

function filtrarTarefas(filtro) {

    let cards = listaTarefas.querySelectorAll(".task-card");

    cards.forEach(function(card) {

        let concluida = card.querySelector(".concluida");

        if (filtro === "all") {

            card.style.display = "flex";

        } else if (filtro === "pending") {

            card.style.display = concluida ? "none" : "flex";

        } else if (filtro === "completed") {

            card.style.display = concluida ? "flex" : "none";

        }

    });

}

opcoesMenu.forEach(function(opcao) {

    opcao.addEventListener("click", function(event) {

        event.preventDefault();

        let filtro = opcao.dataset.filter;
        filtroAtual = filtro;

        filtrarTarefas(filtro);

        opcoesMenu.forEach(function(item) {
            item.classList.remove("active");
        });

        opcao.classList.add("active");

    });

});

// 3. EVENTO DO FORMULÁRIO

formulario.addEventListener("submit", function(event) {

    event.preventDefault();

    let titulo = campoTarefa.value.trim();

    if (titulo === "") {
        return;
    }

    // CRIANDO O CARD

    let card = document.createElement("div");
    card.classList.add("task-card");

    // CRIANDO O TÍTULO

    let tituloElemento = document.createElement("span");
    tituloElemento.textContent = titulo;

    card.appendChild(tituloElemento);

    // ÁREA DOS BOTÕES

    let acoes = document.createElement("div");
    acoes.classList.add("task-actions");

    // BOTÃO CONCLUIR

    let botaoConcluir = document.createElement("button");
    botaoConcluir.textContent = "Concluir";

    acoes.appendChild(botaoConcluir);

    botaoConcluir.addEventListener("click", function() {

    tituloElemento.classList.toggle("concluida");

    atualizarContadores();
    atualizarMensagemVazia();
    filtrarTarefas(filtroAtual);

});

    // BOTÃO EXCLUIR

    let botaoExcluir = document.createElement("button");
    botaoExcluir.textContent = "Excluir";

    acoes.appendChild(botaoExcluir);

    botaoExcluir.addEventListener("click", function() {

    card.remove();

    atualizarContadores();
    atualizarMensagemVazia();
    filtrarTarefas(filtroAtual);

});

    // ADICIONANDO OS BOTÕES AO CARD

    card.appendChild(acoes);

    // ADICIONANDO NA LISTA

    listaTarefas.appendChild(card);

    // ATUALIZANDO CONTADORES

    atualizarContadores();
    atualizarMensagemVazia();

    // LIMPANDO O CAMPO

    campoTarefa.value = "";

});
