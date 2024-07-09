#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#define CLEAR_SCREEN "cls"
#else
#define CLEAR_SCREEN "clear"
#endif

typedef enum {
    ALTA,
    BAIXA
} Prioridade;

typedef enum {
    SUCESSO,
    INSUCESSO
} EstadoExecucao;

typedef struct {
    int id;
    char descricao[100];
    Prioridade prioridade;
    time_t data_registo;
    time_t data_conclusao;
    EstadoExecucao estado_execucao;
} Tarefa;

typedef struct {
    Tarefa *tarefas;
    int tamanho;
    int capacidade;
} ListaTarefas;

typedef struct {
    Tarefa *itens;
    int frente;
    int fundo;
    int capacidade;
} Fila;

typedef struct {
    Tarefa *itens;
    int topo;
    int capacidade;
} Pilha;

void limpar_tela() {
    system(CLEAR_SCREEN);
}

// Função para criar uma nova tarefa
Tarefa criar_tarefa(int id, const char *descricao, Prioridade prioridade) {
    Tarefa nova_tarefa;
    nova_tarefa.id = id;
    strcpy(nova_tarefa.descricao, descricao);
    nova_tarefa.prioridade = prioridade;
    nova_tarefa.data_registo = time(NULL);
    nova_tarefa.estado_execucao = INSUCESSO;
    return nova_tarefa;
}

// Função para processar uma tarefa
void processar_tarefa(Tarefa *tarefa) {
    // Simulando execução da tarefa
    tarefa->estado_execucao = (rand() % 2 == 0) ? SUCESSO : INSUCESSO; // Randomiza sucesso ou insucesso
    tarefa->data_conclusao = time(NULL);
}

// Função para adicionar uma tarefa à lista
void adicionar_tarefa(ListaTarefas *lista, Tarefa tarefa) {
    if (lista->tamanho >= lista->capacidade) {
        lista->capacidade = lista->capacidade ? lista->capacidade * 2 : 10; 
        lista->tarefas = realloc(lista->tarefas, lista->capacidade * sizeof(Tarefa));
    }
    lista->tarefas[lista->tamanho++] = tarefa;
}

// Função para procurar uma tarefa por ID
Tarefa *procurar_tarefa_por_id(ListaTarefas *lista, int id) {
    for (int i = 0; i < lista->tamanho; ++i) {
        if (lista->tarefas[i].id == id) {
            return &lista->tarefas[i];
        }
    }
    return NULL;
}

// Função para imprimir uma tarefa
void imprimir_tarefa(const Tarefa *tarefa) {
    printf("ID: %d\n", tarefa->id);
    printf("Descrição: %s\n", tarefa->descricao);
    printf("Prioridade: %s\n", (tarefa->prioridade == ALTA) ? "Alta" : "Baixa");
    printf("Data/Hora de Registo: %s", ctime(&tarefa->data_registo));
    if (tarefa->estado_execucao != INSUCESSO) {
        printf("Data/Hora de Conclusão: %s", ctime(&tarefa->data_conclusao));
        printf("Estado: %s\n", (tarefa->estado_execucao == SUCESSO) ? "Sucesso" : "Insucesso");
    } else {
        printf("Estado: Pendente\n");
    }
}

// Funções para operações de Pilha (LIFO)
void inicializar_pilha(Pilha *pilha, int capacidade) {
    pilha->itens = (Tarefa *)malloc(capacidade * sizeof(Tarefa));
    pilha->topo = -1; // Inicia a pilha como vazia
    pilha->capacidade = capacidade;
}

void empilhar(Pilha *pilha, Tarefa tarefa) {
    if (pilha->topo >= pilha->capacidade - 1) {
        pilha->capacidade *= 2;
        pilha->itens = realloc(pilha->itens, pilha->capacidade * sizeof(Tarefa));
    }
    pilha->itens[++pilha->topo] = tarefa;
}

Tarefa *desempilhar(Pilha *pilha) {
    if (pilha->topo == -1) {
        fprintf(stderr, "ERRO: Pilha vazia, não é possível desempilhar.\n");
        return NULL;
    }
    return &pilha->itens[pilha->topo--];
}

// Funções para operações de Fila (FIFO)
void inicializar_fila(Fila *fila, int capacidade) {
    fila->itens = (Tarefa *)malloc(capacidade * sizeof(Tarefa));
    fila->frente = 0;
    fila->fundo = -1; // Inicia a fila como vazia
    fila->capacidade = capacidade;
}

void alinhar(Fila *fila, Tarefa tarefa) {
    if (fila->fundo >= fila->capacidade - 1) {
        fila->capacidade *= 2;
        fila->itens = realloc(fila->itens, fila->capacidade * sizeof(Tarefa));
    }
    fila->itens[++fila->fundo] = tarefa;
}

Tarefa *desalinhar(Fila *fila) {
    if (fila->frente > fila->fundo) {
        fprintf(stderr, "ERRO: Fila vazia, não é possível desalinhar.\n");
        return NULL;
    }
    return &fila->itens[fila->frente++];
}

// Funções para manipulação de listas de tarefas realizadas
void guardar_tarefas_em_arquivo(ListaTarefas *realizadas_sucesso, ListaTarefas *realizadas_insucesso, const char *nome_arquivo) {
    FILE *arquivo = fopen(nome_arquivo, "wb");
    if (!arquivo) {
        perror("Erro ao abrir arquivo para escrita");
        return;
    }
    
    fwrite(&realizadas_sucesso->tamanho, sizeof(int), 1, arquivo);
    fwrite(realizadas_sucesso->tarefas, sizeof(Tarefa), realizadas_sucesso->tamanho, arquivo);
    
    fwrite(&realizadas_insucesso->tamanho, sizeof(int), 1, arquivo);
    fwrite(realizadas_insucesso->tarefas, sizeof(Tarefa), realizadas_insucesso->tamanho, arquivo);
    
    fclose(arquivo);
}

void carregar_tarefas_de_arquivo(ListaTarefas *realizadas_sucesso, ListaTarefas *realizadas_insucesso, const char *nome_arquivo) {
    FILE *arquivo = fopen(nome_arquivo, "rb");
    if (!arquivo) {
        perror("Erro ao abrir arquivo para leitura");
        return;
    }
    
    fread(&realizadas_sucesso->tamanho, sizeof(int), 1, arquivo);
    realizadas_sucesso->capacidade = realizadas_sucesso->tamanho;
    realizadas_sucesso->tarefas = malloc(realizadas_sucesso->tamanho * sizeof(Tarefa));
    fread(realizadas_sucesso->tarefas, sizeof(Tarefa), realizadas_sucesso->tamanho, arquivo);
    
    fread(&realizadas_insucesso->tamanho, sizeof(int), 1, arquivo);
    realizadas_insucesso->capacidade = realizadas_insucesso->tamanho;
    realizadas_insucesso->tarefas = malloc(realizadas_insucesso->tamanho * sizeof(Tarefa));
    fread(realizadas_insucesso->tarefas, sizeof(Tarefa), realizadas_insucesso->tamanho, arquivo);
    
    fclose(arquivo);
}

void gerar_relatorio_sucesso(ListaTarefas *realizadas_sucesso, const char *nome_arquivo) {
    FILE *arquivo = fopen(nome_arquivo, "wt");
    if (!arquivo) {
        perror("Erro ao abrir arquivo para escrita");
        return;
    }

    fprintf(arquivo, "RELATÓRIO DE TAREFAS REALIZADAS COM SUCESSO\n\n");
    for (int i = 0; i < realizadas_sucesso->tamanho; ++i) {
        Tarefa *tarefa = &realizadas_sucesso->tarefas[i];
        fprintf(arquivo, "ID: %d\n", tarefa->id);
        fprintf(arquivo, "Descrição: %s\n", tarefa->descricao);
        fprintf(arquivo, "Data/Hora de Conclusão: %s", ctime(&tarefa->data_conclusao));
    }

    fclose(arquivo);
}

void mostrar_menu() {
    limpar_tela(); // Limpa a tela antes de mostrar o menu
    printf("\n===== GESTÃO DE TAREFAS =====\n");
    printf("1. Adicionar Tarefa (Alta Prioridade - FIFO)\n");
    printf("2. Adicionar Tarefa (Baixa Prioridade - LIFO)\n");
    printf("3. Processar Tarefa\n");
    printf("4. Listar Tarefas Pendente\n");
    printf("5. Listar Tarefas Realizadas\n");
    printf("6. Procurar Tarefa por ID\n");
    printf("7. Gerar Relatório de Tarefas Realizadas com Sucesso\n");
    printf("8. Guardar Tarefas em Arquivo\n");
    printf("9. Carregar Tarefas num Arquivo\n");
    printf("0. Sair\n");
    printf("Escolha uma opção: ");
}

int main() {
    srand(time(NULL)); // Inicializa a semente do randomizador com a hora atual

    ListaTarefas tarefas_pendentes = {NULL, 0, 0};
    ListaTarefas tarefas_realizadas_sucesso = {NULL, 0, 0};
    ListaTarefas tarefas_realizadas_insucesso = {NULL, 0, 0};

    Fila fila_alta_prioridade;
    Pilha pilha_baixa_prioridade;

    inicializar_fila(&fila_alta_prioridade, 10);
    inicializar_pilha(&pilha_baixa_prioridade, 10);

    int escolha, id;
    char descricao[100];
    Tarefa tarefa;

    while (1) {
        mostrar_menu();
        scanf("%d", &escolha);

        switch (escolha) {
            case 1:
                printf("Descrição da Tarefa: ");
                scanf(" %[^\n]s", descricao); // Lê a descrição da tarefa
                tarefa = criar_tarefa(tarefas_pendentes.tamanho + 1, descricao, ALTA);
                adicionar_tarefa(&tarefas_pendentes, tarefa);
                alinhar(&fila_alta_prioridade, tarefa);
                break;

            case 2:
                printf("Descrição da Tarefa: ");
                scanf(" %[^\n]s", descricao); // Lê a descrição da tarefa
                tarefa = criar_tarefa(tarefas_pendentes.tamanho + 1, descricao, BAIXA);
                adicionar_tarefa(&tarefas_pendentes, tarefa);
                empilhar(&pilha_baixa_prioridade, tarefa);
                break;

            case 3:
                if (fila_alta_prioridade.frente <= fila_alta_prioridade.fundo) {
                    tarefa = *desalinhar(&fila_alta_prioridade);
                } else if (pilha_baixa_prioridade.topo >= 0) {
                    tarefa = *desempilhar(&pilha_baixa_prioridade);
                } else {
                    printf("Não há tarefas pendentes para processar.\n");
                    break;
                }
                processar_tarefa(&tarefa);
                if (tarefa.estado_execucao == SUCESSO) {
                    adicionar_tarefa(&tarefas_realizadas_sucesso, tarefa);
                } else {
                    adicionar_tarefa(&tarefas_realizadas_insucesso, tarefa);
                }
                printf("Tarefa processada:\n");
                imprimir_tarefa(&tarefa);
                break;

            case 4:
                printf("\nTAREFAS PENDENTES:\n");
                for (int i = 0; i < tarefas_pendentes.tamanho; ++i) {
                    imprimir_tarefa(&tarefas_pendentes.tarefas[i]);
                    printf("\n");
                }
                break;

            case 5:
                printf("\nTAREFAS REALIZADAS COM SUCESSO:\n");
                for (int i = 0; i < tarefas_realizadas_sucesso.tamanho; ++i) {
                    imprimir_tarefa(&tarefas_realizadas_sucesso.tarefas[i]);
                    printf("\n");
                }
                printf("\nTAREFAS REALIZADAS COM INSUCESSO:\n");
                for (int i = 0; i < tarefas_realizadas_insucesso.tamanho; ++i) {
                    imprimir_tarefa(&tarefas_realizadas_insucesso.tarefas[i]);
                    printf("\n");
                }
                break;

            case 6:
                printf("ID da Tarefa: ");
                scanf("%d", &id);
                Tarefa *tarefa_procurada = procurar_tarefa_por_id(&tarefas_pendentes, id);
                if (tarefa_procurada) {
                    imprimir_tarefa(tarefa_procurada);
                } else {
                    printf("Tarefa não encontrada.\n");
                }
                break;

            case 7:
                gerar_relatorio_sucesso(&tarefas_realizadas_sucesso, "relatorio_sucesso.txt");
                printf("Relatório gerado com sucesso.\n");
                break;

            case 8:
                guardar_tarefas_em_arquivo(&tarefas_realizadas_sucesso, &tarefas_realizadas_insucesso, "tarefas.bin");
                printf("Tarefas guardadas com sucesso.\n");
                break;

            case 9:
                carregar_tarefas_de_arquivo(&tarefas_realizadas_sucesso, &tarefas_realizadas_insucesso, "tarefas.bin");
                printf("Tarefas carregadas com sucesso.\n");
                break;

            case 0:
                printf("Saindo...\n");
                exit(0);
                break;

            default:
                printf("Opção inválida. Tente novamente.\n");
                break;
        }

        printf("\nPressione Enter para continuar...");
        getchar(); // Aguarda o usuário pressionar Enter
        getchar(); // Aguarda o usuário pressionar Enter
    }

    return 0;
}