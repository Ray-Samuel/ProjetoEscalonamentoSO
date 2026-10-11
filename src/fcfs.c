#include <stdio.h>
#define MAX 10
// Struct de uma fila circular
typedef struct {
    int processos[MAX];    // vetor que armazena até 10 processos
    int inicio;
    int fim;
    int quantidade;       // número de processos que estão na fila.
} Fila;



void enfileirar(Fila *f, int processo) {
    if (f->quantidade == MAX) {
        printf("Fila cheia! processo não entrou na fila de execussão\n");
        return;
    }

    f->processos[f->fim] = processo;       // Inserindo na ultima posição representado por "f->fim"
    f->fim = (f->fim + 1) % MAX;           // Pode inserir caso posiçao estiver livre, mas nao interfere na ordem de execussão
    f->quantidade++;                       // Apos armazenar em uma posição x, pulamos para a posição x+1  para armazenar outro valor
    printf("Processo enfilerado, posição: %d\n", f->quantidade);
}

int desenfileirar(Fila *f) {
    if (f->quantidade == 0) {
        return -1;                             // Acabou!
    }

    int processo = f->processos[f->inicio];    // Guarda o processo que vai executar
    f->inicio = (f->inicio + 1) % MAX;         // Passagem para o proximo da fila
    f->quantidade--;                           

    return processo;                           // Retornando o número do processo que foi executado
}

int main() {
    Fila fila = {{0}, 0, 0, 0};               // Inicializa o vetor com zeros e os três controles (inicio, fim e quantidade) com zero.
    int n, i, processo;                       // Auxiliares

    printf("Quantidade de processos: ");
    scanf("%d", &n);

    for (i = 1; i <= n; i++) {
        enfileirar(&fila, i); 
    }

    while (fila.quantidade > 0) {
        processo = desenfileirar(&fila);
        printf("Executando processo da posição: %d\n", processo);
    }

    return 0;
}


// cd src
// gcc fcfs.c -o fcfs 
// ./fcfs