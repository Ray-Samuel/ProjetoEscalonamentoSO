#include <stdio.h>

#define MAX_PROCESSOS 100

typedef struct{
    int pid;
    int chegada;
    int burst;
    int inicio;
    int termino;
    int espera;
    int retorno;
    int executado;
} Processo;

int main(){
    Processo processos[MAX_PROCESSOS];
    int n;
    int tempo_atual = 0;
    int concluidos = 0;
   

    printf("ESCALONAMENTO SJF\n");

    printf("Digite a quantidade de processos (1 a %d): ", MAX_PROCESSOS);
    scanf("%d", &n);

    if (n < 1 || n > MAX_PROCESSOS){
        printf("Quantidade de processos invalida\n");
        return 1;
    }

    // aq eu leio as infos dos processos
    for (int i = 0; i < n; i++){
        processos[i].pid = i + 1;
        processos[i].executado = 0;

        printf("\nProcesso P%d\n", processos[i].pid);

        printf("Tempo de chegada: ");
        scanf("%d", &processos[i].chegada);

        if (processos[i].chegada < 0){
            printf("Tempo de chegada invalido\n");
            return 1;
        }

        printf("Tempo de execucao (burst): ");
        scanf("%d", &processos[i].burst);

        if (processos[i].burst <= 0){
            printf("Tempo de execucao invalido.\n");
            return 1;
        }
    }


    printf("\nORDEM DE EXECUCAO\n");

    while (concluidos < n){
        int escolhido = -1;

        for (int i = 0; i < n; i++){
            if (processos[i].executado == 0 && processos[i].chegada <= tempo_atual){

                if (escolhido == -1 || processos[i].burst < processos[escolhido].burst){
                    escolhido = i;
                }
            }
        }

        if (escolhido == -1){
            tempo_atual++;
            continue;
        }

        processos[escolhido].inicio = tempo_atual;

        processos[escolhido].espera = processos[escolhido].inicio - processos[escolhido].chegada;

        processos[escolhido].termino = processos[escolhido].inicio + processos[escolhido].burst;

        processos[escolhido].retorno = processos[escolhido].termino - processos[escolhido].chegada;

        printf("P%d: Inicia em %d, termina em %d\n", processos[escolhido].pid, processos[escolhido].inicio, processos[escolhido].termino);

        tempo_atual = processos[escolhido].termino;
        processos[escolhido].executado = 1;
        concluidos++;
    }

    printf("\nRESULTADOS\n");

    printf("%-8s %-10s %-8s %-8s %-9s %-9s %-9s\n", "PID", "Chegada", "Burst", "Inicio", "Termino", "Espera", "Retorno");

    for (int i = 0; i < n; i++){
        printf("P%-7d %-10d %-8d %-8d %-9d %-9d %-9d\n", processos[i].pid, processos[i].chegada, processos[i].burst, processos[i].inicio, processos[i].termino, processos[i].espera, processos[i].retorno);
    }

    printf("\nTempo total de execucao: %d\n", tempo_atual);

    return 0;
}