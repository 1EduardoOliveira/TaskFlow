
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <limits.h>

#include "mongoose.h"

// =====================================================
// 1. ESTRUTURA DA TAREFA
// =====================================================

/*
 * Representa uma tarefa do TaskFlow.
 *
 * Cada tarefa possui:
 * - id: identificador único.
 * - titulo: descrição da tarefa.
 * - concluida: 0 para pendente e 1 para concluída.
 * - proxima: endereço da próxima tarefa da lista.
 */

typedef struct Tarefa {
    int id;
    char titulo[200];
    int concluida;

    struct Tarefa *proxima;

} Tarefa;


// =====================================================
// 2. VARIÁVEIS GLOBAIS
// =====================================================

// Ponteiro para o primeiro elemento da lista.
// NULL significa que não existem tarefas cadastradas.

Tarefa *lista_tarefas = NULL;

// Identificador que será atribuído à próxima tarefa.

int proximo_id = 1;


// =====================================================
// 3. CADASTRAR UMA TAREFA
// =====================================================

/*
 * Cria uma nova tarefa e adiciona no início da lista.
 *
 * Retorno:
 * 1 = tarefa cadastrada.
 * 0 = erro na alocação de memória.
 */

int cadastrar_tarefa(char *titulo) {

    // Reserva memória para uma nova tarefa.
    Tarefa *nova = malloc(sizeof(Tarefa));

    // Verifica se a alocação funcionou.
    if (nova == NULL) {
        printf("Erro ao alocar memoria!\n");
        return 0;
    }

    // Preenche os dados da nova tarefa.
    nova->id = proximo_id++;
    nova->concluida = 0;

    // Copia o título para dentro da estrutura.
    snprintf(
        nova->titulo,
        sizeof(nova->titulo),
        "%s",
        titulo
    );

    // A nova tarefa aponta para o antigo primeiro elemento.
    nova->proxima = lista_tarefas;

    // Atualiza o início da lista.
    lista_tarefas = nova;

    printf("Tarefa cadastrada com sucesso!\n");

    return 1;
}


// =====================================================
// 4. LISTAR TODAS AS TAREFAS
// =====================================================

void listar_tarefas(void) {

    // Começa pelo primeiro elemento.
    Tarefa *atual = lista_tarefas;

    // Verifica se a lista está vazia.
    if (atual == NULL) {
        printf("Nenhuma tarefa cadastrada!\n");
        return;
    }

    printf("\n===== LISTA DE TAREFAS =====\n");

    // Percorre a lista até encontrar NULL.
    while (atual != NULL) {

        printf("ID: %d\n", atual->id);
        printf("Titulo: %s\n", atual->titulo);

        printf(
            "Concluida: %s\n",
            atual->concluida ? "Sim" : "Nao"
        );

        printf("----------------------------\n");

        // Avança para o próximo elemento.
        atual = atual->proxima;
    }
}


// =====================================================
// 5. CONVERTER A LISTA COMPLETA EM JSON
// =====================================================

/*
 * Função utilizada pelo Mongoose para transformar
 * nossa lista encadeada em um array JSON.
 */

size_t gerar_json_tarefas(
    void (*saida)(char, void *),
    void *parametro,
    va_list *argumentos
) {

    // Recupera a lista recebida pelo Mongoose.
    Tarefa *atual = va_arg(*argumentos, Tarefa *);

    size_t tamanho = 0;

    // Controla a primeira tarefa para não colocar
    // uma vírgula antes do primeiro objeto.
    int primeira = 1;

    // Inicia o array JSON.
    tamanho += mg_xprintf(saida, parametro, "[");

    // Percorre todas as tarefas.
    while (atual != NULL) {

        // Adiciona vírgula entre os objetos.
        if (!primeira) {
            tamanho += mg_xprintf(saida, parametro, ",");
        }

        // Gera o objeto JSON da tarefa atual.
        tamanho += mg_xprintf(
            saida,
            parametro,
            "{\"id\":%d,\"titulo\":%m,\"concluida\":%s}",
            atual->id,
            mg_print_esc, 0, atual->titulo,
            atual->concluida ? "true" : "false"
        );

        primeira = 0;

        // Avança para a próxima tarefa.
        atual = atual->proxima;
    }

    // Finaliza o array JSON.
    tamanho += mg_xprintf(saida, parametro, "]");

    return tamanho;
}


// =====================================================
// 6. BUSCAR UMA TAREFA PELO ID
// =====================================================

/*
 * Percorre a lista procurando uma tarefa específica.
 *
 * Retorna:
 * - Endereço da tarefa, se encontrar.
 * - NULL, caso não encontre.
 */

Tarefa *buscar_tarefa(int id) {

    Tarefa *atual = lista_tarefas;

    while (atual != NULL) {

        // Compara o ID atual com o ID procurado.
        if (atual->id == id) {
            return atual;
        }

        atual = atual->proxima;
    }

    return NULL;
}


// =====================================================
// 7. ALTERAR UMA TAREFA
// =====================================================

/*
 * Localiza uma tarefa pelo ID e substitui seu título.
 *
 * Retorna:
 * 1 = alteração realizada.
 * 0 = tarefa não encontrada.
 */

int alterar_tarefa(int id, char *novo_titulo) {

    // Procura a tarefa.
    Tarefa *tarefa = buscar_tarefa(id);

    if (tarefa == NULL) {
        return 0;
    }

    // Substitui o título antigo.
    snprintf(
        tarefa->titulo,
        sizeof(tarefa->titulo),
        "%s",
        novo_titulo
    );

    return 1;
}


// =====================================================
// 8. REMOVER UMA TAREFA
// =====================================================

/*
 * Remove um elemento da lista encadeada.
 *
 * É necessário ajustar os ponteiros antes de liberar
 * a memória do elemento removido.
 *
 * Retorna:
 * 1 = remoção realizada.
 * 0 = tarefa não encontrada.
 */

int remover_tarefa(int id) {

    Tarefa *atual = lista_tarefas;
    Tarefa *anterior = NULL;

    // Procura a tarefa pelo ID.
    while (atual != NULL && atual->id != id) {

        anterior = atual;
        atual = atual->proxima;
    }

    // Não encontrou a tarefa.
    if (atual == NULL) {
        return 0;
    }

    // Caso especial: a tarefa está no início da lista.
    if (anterior == NULL) {

        lista_tarefas = atual->proxima;

    } else {

        // O elemento anterior passa a apontar
        // para o elemento seguinte ao removido.
        anterior->proxima = atual->proxima;
    }

    // Libera a memória da tarefa removida.
    free(atual);

    return 1;
}


// =====================================================
// 9. EXTRAIR O ID DA URL
// =====================================================

/*
 * Exemplo:
 *
 * URL: /tarefas/15
 * ID extraído: 15
 *
 * Retorna 1 se conseguir extrair um ID válido.
 * Retorna 0 caso contrário.
 */

int extrair_id_url(
    struct mg_http_message *mensagem,
    int *id
) {

    struct mg_str capturas[1];

    unsigned int valor;

    // Verifica se a URL segue o padrão /tarefas/ID.
    if (!mg_match(
        mensagem->uri,
        mg_str("/tarefas/*"),
        capturas
    )) {
        return 0;
    }

    // Converte o texto capturado em número.
    if (!mg_str_to_num(
        capturas[0],
        10,
        &valor,
        sizeof(valor)
    )) {
        return 0;
    }

    // IDs precisam ser positivos e caber em um int.
    if (valor == 0 || valor > INT_MAX) {
        return 0;
    }

    *id = (int) valor;

    return 1;
}


// =====================================================
// 10. CONVERTER UMA TAREFA INDIVIDUAL EM JSON
// =====================================================

/*
 * Diferente da função anterior, esta gera somente
 * um objeto JSON, e não um array de tarefas.
 */

size_t gerar_json_tarefa(
    void (*saida)(char, void *),
    void *parametro,
    va_list *argumentos
) {

    // Recupera o endereço da tarefa.
    Tarefa *tarefa = va_arg(*argumentos, Tarefa *);

    // Monta o objeto JSON.
    return mg_xprintf(
        saida,
        parametro,
        "{\"id\":%d,\"titulo\":%m,\"concluida\":%s}",
        tarefa->id,
        mg_print_esc, 0, tarefa->titulo,
        tarefa->concluida ? "true" : "false"
    );
}


// =====================================================
// 11. TRATAR AS REQUISIÇÕES HTTP
// =====================================================

static void tratar_requisicao(
    struct mg_connection *conexao,
    int evento,
    void *dados
) {

    // Só processamos eventos de mensagens HTTP.
    if (evento != MG_EV_HTTP_MSG) {
        return;
    }

    // Converte os dados recebidos em uma mensagem HTTP.
    struct mg_http_message *mensagem = dados;

    // Exibe o método recebido no terminal.
    printf(
        "Metodo recebido: %.*s\n",
        (int) mensagem->method.len,
        mensagem->method.buf
    );


    // =================================================
    // ROTA 1: /
    // =================================================

    if (mg_match(mensagem->uri, mg_str("/"), NULL)) {

        // Responde que a API está funcionando.
        mg_http_reply(
            conexao,
            200,
            "Content-Type: text/plain\r\n",
            "TaskFlow API funcionando!\n"
        );

    }


    // =================================================
    // ROTA 2: /tarefas
    // =================================================

    else if (
        mg_match(mensagem->uri, mg_str("/tarefas"), NULL)
    ) {

        // ---------------------------------------------
        // GET - LISTAR TODAS AS TAREFAS
        // ---------------------------------------------

        if (
            mg_match(mensagem->method, mg_str("GET"), NULL)
        ) {

            mg_http_reply(
                conexao,
                200,
                "Content-Type: application/json\r\n",
                "%M\n",
                gerar_json_tarefas,
                lista_tarefas
            );

        }


        // ---------------------------------------------
        // POST - CADASTRAR UMA TAREFA
        // ---------------------------------------------

        else if (
            mg_match(mensagem->method, mg_str("POST"), NULL)
        ) {

            // Extrai o título do JSON recebido.
            char *titulo = mg_json_get_str(
                mensagem->body,
                "$.titulo"
            );

            // Verifica se o título é válido.
            if (titulo == NULL || titulo[0] == '\0') {

                mg_free(titulo);

                mg_http_reply(
                    conexao,
                    400,
                    "",
                    "Titulo invalido!\n"
                );

                return;
            }

            // Cadastra a tarefa na lista dinâmica.
            int sucesso = cadastrar_tarefa(titulo);

            // Libera a memória da string extraída.
            mg_free(titulo);

            // Verifica se o cadastro funcionou.
            if (!sucesso) {

                mg_http_reply(
                    conexao,
                    500,
                    "",
                    "Erro ao cadastrar tarefa!\n"
                );

                return;
            }

            // Responde que a tarefa foi cadastrada.
            mg_http_reply(
                conexao,
                201,
                "Content-Type: application/json\r\n",
                "{\"mensagem\":\"Tarefa cadastrada!\"}\n"
            );

        }


        // ---------------------------------------------
        // MÉTODO NÃO PERMITIDO EM /tarefas
        // ---------------------------------------------

        else {

            mg_http_reply(
                conexao,
                405,
                "",
                "Metodo nao permitido!\n"
            );

        }

    }


    // =================================================
    // ROTA 3: /tarefas/:id
    // =================================================

    else if (
        mg_match(mensagem->uri, mg_str("/tarefas/*"), NULL)
    ) {

        int id;

        // Extrai o ID da URL.
        if (!extrair_id_url(mensagem, &id)) {

            mg_http_reply(
                conexao,
                400,
                "",
                "ID invalido!\n"
            );

            return;
        }


        // ---------------------------------------------
        // GET - CONSULTAR UMA TAREFA
        // ---------------------------------------------

        if (
            mg_match(mensagem->method, mg_str("GET"), NULL)
        ) {

            // Procura a tarefa pelo ID.
            Tarefa *tarefa = buscar_tarefa(id);

            // Verifica se a tarefa existe.
            if (tarefa == NULL) {

                mg_http_reply(
                    conexao,
                    404,
                    "",
                    "Tarefa nao encontrada!\n"
                );

                return;
            }

            // Retorna a tarefa encontrada em JSON.
            mg_http_reply(
                conexao,
                200,
                "Content-Type: application/json\r\n",
                "%M\n",
                gerar_json_tarefa,
                tarefa
            );

        }


        // ---------------------------------------------
        // PUT - ALTERAR UMA TAREFA
        // ---------------------------------------------

        else if (
            mg_match(mensagem->method, mg_str("PUT"), NULL)
        ) {

            // Extrai o novo título do JSON.
            char *titulo = mg_json_get_str(
                mensagem->body,
                "$.titulo"
            );

            // Verifica se o título foi enviado.
            if (titulo == NULL || titulo[0] == '\0') {

                mg_free(titulo);

                mg_http_reply(
                    conexao,
                    400,
                    "",
                    "Titulo invalido!\n"
                );

                return;
            }

            // Tenta alterar a tarefa.
            int sucesso = alterar_tarefa(id, titulo);

            // Libera a memória do título recebido.
            mg_free(titulo);

            // Verifica se a tarefa foi encontrada.
            if (!sucesso) {

                mg_http_reply(
                    conexao,
                    404,
                    "",
                    "Tarefa nao encontrada!\n"
                );

                return;
            }

            // Busca novamente a tarefa já atualizada.
            Tarefa *tarefa = buscar_tarefa(id);

            // Retorna os dados atualizados.
            mg_http_reply(
                conexao,
                200,
                "Content-Type: application/json\r\n",
                "%M\n",
                gerar_json_tarefa,
                tarefa
            );

        }


        // ---------------------------------------------
        // DELETE - REMOVER UMA TAREFA
        // ---------------------------------------------

        else if (
            mg_match(mensagem->method, mg_str("DELETE"), NULL)
        ) {

            // Tenta remover a tarefa pelo ID.
            int sucesso = remover_tarefa(id);

            // Verifica se ela foi encontrada.
            if (!sucesso) {

                mg_http_reply(
                    conexao,
                    404,
                    "",
                    "Tarefa nao encontrada!\n"
                );

                return;
            }

            // Confirma a remoção.
            mg_http_reply(
                conexao,
                200,
                "Content-Type: application/json\r\n",
                "{\"mensagem\":\"Tarefa removida com sucesso!\"}\n"
            );

        }


        // ---------------------------------------------
        // MÉTODO NÃO PERMITIDO EM /tarefas/:id
        // ---------------------------------------------

        else {

            mg_http_reply(
                conexao,
                405,
                "",
                "Metodo nao permitido!\n"
            );

        }

    }


    // =================================================
    // ROTA 4: NÃO ENCONTRADA
    // =================================================

    else {

        mg_http_reply(
            conexao,
            404,
            "Content-Type: text/plain\r\n",
            "Rota nao encontrada!\n"
        );

    }

}


// =====================================================
// 12. FUNÇÃO PRINCIPAL
// =====================================================

int main(void) {

    // Gerenciador responsável pelas conexões.
    struct mg_mgr gerenciador;

    // Inicializa o gerenciador do Mongoose.
    mg_mgr_init(&gerenciador);

    printf("TaskFlow iniciado!\n");

    // Inicia o servidor HTTP na porta 8080.
    struct mg_connection *servidor = mg_http_listen(
        &gerenciador,
        "http://0.0.0.0:8080",
        tratar_requisicao,
        NULL
    );

    // Verifica se o servidor foi iniciado.
    if (servidor == NULL) {

        printf("Erro ao iniciar o servidor!\n");

        mg_mgr_free(&gerenciador);

        return 1;
    }

    printf("Servidor rodando em http://localhost:8080\n");

    // Mantém o servidor funcionando.
    for (;;) {

        // Verifica eventos e novas requisições.
        mg_mgr_poll(&gerenciador, 1000);

    }

    // Libera os recursos do Mongoose.
    // Este ponto não é alcançado no loop infinito normal.
    mg_mgr_free(&gerenciador);

    return 0;
}
