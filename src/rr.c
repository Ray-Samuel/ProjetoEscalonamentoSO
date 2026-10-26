#include <locale.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>

#define QUANTUM 3
#define MAX_PROCESSOS 5
#define MAX_EXECUCOES 32

typedef enum
{
    PRONTO,      /* Esta aguardando sua vez na fila. */
    EM_EXECUCAO, /* Esta usando a CPU no turno atual. */
    TERMINADO    /* Ja consumiu todo o tempo de CPU necessario. */
} Status;

/* Cada processo e um no da lista de processos e guarda seu estado de execucao. */
typedef struct Processo
{
    int pid;      /* Identificador exibido como P1, P2 etc. */
    int chegada;  /* Instante em que o processo pode entrar na fila. */
    int burst;    /* Tempo total de CPU que o processo precisa. */
    int restante; /* Tempo de CPU que ainda falta executar. */
    int admitido; /* Evita adicionar o processo varias vezes a fila. */
    Status status;
    struct Processo *proximoGlobal; /* Proximo no da lista de todos os processos. */
    struct Processo *proximoPronto; /* Proximo no da fila de prontos. */
} Processo;

/* Fila FIFO: inicio sai primeiro e fim recebe os novos processos. */
typedef struct
{
    Processo *inicio;
    Processo *fim;
    int quantidade;
} FilaProntos;

static const char *nomeStatus(Status status)
{
    /* Converte o estado interno em um texto legivel na interface. */
    switch (status)
    {
    case PRONTO:
        return "Pronto";
    case EM_EXECUCAO:
        return "Em Execução";
    case TERMINADO:
        return "Terminado";
    }

    return "Desconhecido";
}

static Processo *criarProcesso(int pid, int chegada, int burst)
{
    /* Reserva memoria para um no e inicializa o processo como pronto. */
    Processo *processo = malloc(sizeof(*processo));
    if (processo == NULL)
    {
        return NULL;
    }

    processo->pid = pid;
    processo->chegada = chegada;
    processo->burst = burst;
    processo->restante = burst;
    processo->admitido = 0;
    processo->status = PRONTO;
    processo->proximoGlobal = NULL;
    processo->proximoPronto = NULL;
    return processo;
}

static void adicionarProcesso(Processo **lista, Processo *processo)
{
    /* Percorre a lista ate encontrar o ponteiro onde o novo no sera ligado. */
    while (*lista != NULL)
    {
        lista = &(*lista)->proximoGlobal;
    }

    *lista = processo;
}

static int criarEAdicionarProcesso(Processo **lista, int pid, int chegada,
                                   int burst)
{
    /* Junta a criacao e a insercao para simplificar o cadastro dos exemplos. */
    Processo *processo = criarProcesso(pid, chegada, burst);
    if (processo == NULL)
    {
        return EXIT_FAILURE;
    }

    adicionarProcesso(lista, processo);
    return EXIT_SUCCESS;
}

static void liberarProcessos(Processo *lista)
{
    /* Libera cada no alocado antes de encerrar o programa. */
    while (lista != NULL)
    {
        Processo *proximoGlobal = lista->proximoGlobal;
        free(lista);
        lista = proximoGlobal;
    }
}

/* A fila de prontos encadeia diretamente os nos dos processos. */
static void enfileirar(FilaProntos *filaProntos, Processo *processo)
{
    processo->proximoPronto = NULL;
    if (filaProntos->fim == NULL)
    {
        /* Quando a fila esta vazia, o processo e simultaneamente inicio e fim. */
        filaProntos->inicio = processo;
    }
    else
    {
        filaProntos->fim->proximoPronto = processo;
    }

    filaProntos->fim = processo;
    filaProntos->quantidade++;
}

static Processo *desenfileirar(FilaProntos *filaProntos)
{
    Processo *processo = filaProntos->inicio;
    if (processo == NULL)
    {
        return NULL;
    }

    /* Retira o primeiro da fila e atualiza o fim se ela ficar vazia. */
    filaProntos->inicio = processo->proximoPronto;
    if (filaProntos->inicio == NULL)
    {
        filaProntos->fim = NULL;
    }

    processo->proximoPronto = NULL;
    filaProntos->quantidade--;
    return processo;
}

/* Coloca na fila, na ordem da lista, os processos que ja chegaram. */
static void admitirProcessos(Processo *processos, FilaProntos *filaProntos,
                             int tempoAtual)
{
    for (Processo *processo = processos; processo != NULL;
         processo = processo->proximoGlobal)
    {
        if (!processo->admitido && processo->chegada <= tempoAtual)
        {
            enfileirar(filaProntos, processo);
            /* Marcado para que o mesmo processo nao seja admitido novamente. */
            processo->admitido = 1;
        }
    }
}

static void mostrarInterface(const Processo *processos,
                             const FilaProntos *filaProntos,
                             int tempoAtual, int pidExecutando,
                             const int historico[], int quantidadeExecucoes,
                             int concluido)
{
    /* Redesenha a tela para mostrar o estado mais recente da simulacao. */
    erase();
    mvprintw(0, 0, "========== ESCALONAMENTO ROUND ROBIN ==========");
    mvprintw(1, 0, "Quantum: %d segundos | Tempo simulado: %d s",
             QUANTUM, tempoAtual);
    mvprintw(3, 0, "%-10s %-10s %-10s %-12s %-14s",
             "Processo", "Chegada", "Burst", "Restante", "Status");

    /* Mostra todos os processos percorrendo a lista encadeada. */
    int linha = 4;
    for (const Processo *processo = processos; processo != NULL;
         processo = processo->proximoGlobal)
    {
        mvprintw(linha++, 0, "P%-9d %-10d %-10d %-12d %-14s",
                 processo->pid,
                 processo->chegada,
                 processo->burst,
                 processo->restante,
                 nomeStatus(processo->status));
    }

    /* Informa qual processo esta usando a CPU ou se ela esta ociosa. */
    mvprintw(10, 0, "Executando: %s",
             pidExecutando == 0 ? "Nenhum (CPU ociosa)" : "");
    if (pidExecutando != 0)
    {
        mvprintw(10, 12, "P%d", pidExecutando);
    }

    /* Exibe a fila na mesma ordem em que os processos serao executados. */
    mvprintw(11, 0, "Fila de prontos: ");
    if (filaProntos->inicio == NULL)
    {
        printw("(vazia)");
    }
    else
    {
        int primeiro = 1;
        for (const Processo *processo = filaProntos->inicio; processo != NULL;
             processo = processo->proximoPronto)
        {
            printw("%sP%d", primeiro ? "" : " -> ", processo->pid);
            primeiro = 0;
        }
    }

    /* O historico registra cada turno; um processo pode aparecer varias vezes. */
    mvprintw(13, 0, "Ordem de execucao:");
    for (int i = 0; i < quantidadeExecucoes; i++)
    {
        int linha = 14 + i / 8;
        int coluna = (i % 8) * 10;
        mvprintw(linha, coluna, "P%d%s", historico[i],
                 i + 1 == quantidadeExecucoes ? "" : " ->");
    }

    if (concluido)
    {
        mvprintw(18, 0, "Simulacao concluida. Pressione Enter para sair.");
    }

    refresh();
}

int main(void)
{
    /* Os processos de exemplo ficam ligados em uma lista encadeada. */
    Processo *processos = NULL;
    /* A fila e independente da lista: guarda a ordem dos processos prontos. */
    FilaProntos filaProntos = {NULL, NULL, 0};
    /* O historico guarda os PIDs na ordem em que receberam a CPU. */
    int historico[MAX_EXECUCOES];
    int quantidadeExecucoes = 0;
    int processosTerminados = 0;
    int tempoAtual = 0;

    /* Cada cadastro informa PID, tempo de chegada e burst de CPU. */
    if (criarEAdicionarProcesso(&processos, 1, 0, 8) != EXIT_SUCCESS ||
        criarEAdicionarProcesso(&processos, 2, 0, 6) != EXIT_SUCCESS ||
        criarEAdicionarProcesso(&processos, 3, 1, 10) != EXIT_SUCCESS ||
        criarEAdicionarProcesso(&processos, 4, 2, 5) != EXIT_SUCCESS ||
        criarEAdicionarProcesso(&processos, 5, 4, 7) != EXIT_SUCCESS)
    {
        liberarProcessos(processos);
        fprintf(stderr, "Erro: nao foi possivel alocar um processo.\n");
        return EXIT_FAILURE;
    }

    setlocale(LC_ALL, "");
    /* Inicializa o terminal ncurses para atualizar a interface sem rolar a tela. */
    initscr();
    cbreak();
    noecho();
    curs_set(0);

    while (processosTerminados < MAX_PROCESSOS)
    {
        /* A cada instante, adiciona a fila os processos que ja chegaram. */
        admitirProcessos(processos, &filaProntos, tempoAtual);

        Processo *processo = desenfileirar(&filaProntos);
        if (processo == NULL)
        {
            /* Sem processos prontos, a CPU fica ociosa e o relogio avanca. */
            mostrarInterface(processos, &filaProntos, tempoAtual, 0, historico,
                             quantidadeExecucoes, 0);
            napms(QUANTUM * 1000);
            tempoAtual++;
            continue;
        }

        /* O processo executa no maximo um quantum, ou so o que ainda falta. */
        int tempoExecutado = processo->restante < QUANTUM ? processo->restante : QUANTUM;

        processo->status = EM_EXECUCAO;

        if (quantidadeExecucoes == MAX_EXECUCOES)
        {
            endwin();
            fprintf(stderr, "Erro: historico de execucao cheio.\n");
            liberarProcessos(processos);
            return EXIT_FAILURE;
        }

        /* Registra o turno e mostra quem esta executando antes da pausa. */
        historico[quantidadeExecucoes++] = processo->pid;
        mostrarInterface(processos, &filaProntos, tempoAtual, processo->pid,
                         historico, quantidadeExecucoes, 0);
        /* A pausa em tempo real representa a execucao simulada do turno. */
        napms(tempoExecutado * 1000);

        /* Atualiza o relogio e impede que o tempo restante fique negativo. */
        processo->restante -= tempoExecutado;
        tempoAtual += tempoExecutado;

        /* Processos que chegaram durante o turno entram antes do revezamento. */
        admitirProcessos(processos, &filaProntos, tempoAtual);

        if (processo->restante == 0)
        {
            /* Processo concluido sai da fila definitivamente. */
            processo->status = TERMINADO;
            processosTerminados++;
        }
        else
        {
            /* Se ainda falta CPU, volta ao fim da fila para esperar outro turno. */
            processo->status = PRONTO;
            enfileirar(&filaProntos, processo);
        }
    }

    /* Mantem a tela final visivel ate o usuario pressionar Enter. */
    mostrarInterface(processos, &filaProntos, tempoAtual, 0, historico,
                     quantidadeExecucoes, 1);
    while (getch() != '\n')
    {
    }
    endwin();
    liberarProcessos(processos);
    return EXIT_SUCCESS;
}
