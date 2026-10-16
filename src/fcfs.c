#include <stdio.h>
#define MAX 10

typedef struct {
    int id;
    int tempoChegada;
    int burst;
    int tempoInicio;
    int tempoTermino;
    int tempoEspera;
    int tempoRetorno;
} Processo;

typedef struct {
    Processo processos[MAX];
    int inicio;
    int fim;
    int quantidade;
} Fila;

void inicializarFila(Fila *f) {
    f->inicio = 0;
    f->fim = 0;
    f->quantidade = 0;
}

int enfileirar(Fila *f, Processo processo) {
    if (f->quantidade == MAX) {
        printf("Fila cheia!\n");
        return 0;
    }

    f->processos[f->fim] = processo;
    f->fim = (f->fim + 1) % MAX;
    f->quantidade++;

    return 1;
}

int desenfileirar(Fila *f, Processo *processo) {
    if (f->quantidade == 0) {
        return 0;
    }

    *processo = f->processos[f->inicio];
    f->inicio = (f->inicio + 1) % MAX;
    f->quantidade--;

    return 1;
}

void ordenarPorChegada(Processo processos[], int n) {
    int i, j;
    Processo auxiliar;

    for (i = 1; i < n; i++) {
        auxiliar = processos[i];
        j = i - 1;

        while (j >= 0 &&
               processos[j].tempoChegada > auxiliar.tempoChegada) {
            processos[j + 1] = processos[j];
            j--;
        }

        processos[j + 1] = auxiliar;
    }
}

int main() {
    Fila fila;
    Processo processos[MAX];
    Processo processo;

    int n, i;
    int tempoAtual = 0;
    int somaEspera = 0;
    int somaRetorno = 0;

    inicializarFila(&fila);

    printf("Quantidade de processos (1 a %d): ", MAX);

    if (scanf("%d", &n) != 1 || n < 1 || n > MAX) {
        printf("Quantidade invalida!\n");
        return 1;
    }

    for (i = 0; i < n; i++) {
        processos[i].id = i + 1;

        printf("\nProcesso P%d\n", processos[i].id);

        printf("Tempo de chegada: ");
        if (scanf("%d", &processos[i].tempoChegada) != 1 ||
            processos[i].tempoChegada < 0) {
            printf("Tempo de chegada invalido!\n");
            return 1;
        }

        printf("Burst (tempo de execucao): ");
        if (scanf("%d", &processos[i].burst) != 1 ||
            processos[i].burst <= 0) {
            printf("Burst invalido!\n");
            return 1;
        }
    }

    ordenarPorChegada(processos, n);

    for (i = 0; i < n; i++) {
        if (!enfileirar(&fila, processos[i])) {
            return 1;
        }
    }

    printf("\n========== ESCALONAMENTO FCFS ==========\n\n");

    printf("%-10s %-10s %-10s %-10s %-10s %-10s\n",
           "Processo", "Chegada", "Burst",
           "Inicio", "Termino", "Espera");

    while (desenfileirar(&fila, &processo)) {
        if (tempoAtual < processo.tempoChegada) {
            tempoAtual = processo.tempoChegada;
        }

        processo.tempoInicio = tempoAtual;
        processo.tempoEspera = processo.tempoInicio - processo.tempoChegada;

        tempoAtual += processo.burst;

        processo.tempoTermino = tempoAtual;
        processo.tempoRetorno = processo.tempoTermino - processo.tempoChegada;

        somaEspera += processo.tempoEspera;
        somaRetorno += processo.tempoRetorno;

        printf("P%-9d %-10d %-10d %-10d %-10d %-10d\n",
               processo.id,
               processo.tempoChegada,
               processo.burst,
               processo.tempoInicio,
               processo.tempoTermino,
               processo.tempoEspera);
    }

    printf("\nOrdem de execucao: ");

    for (i = 0; i < n; i++) {
        printf("P%d", processos[i].id);

        if (i < n - 1) {
            printf(" -> ");
        }
    }

    printf("\n");
    printf("Tempo total de execucao: %d\n", tempoAtual);
    printf("Tempo medio de espera: %.2f\n",
           (float)somaEspera / n);
    printf("Tempo medio de retorno: %.2f\n",
           (float)somaRetorno / n);

    return 0;
}