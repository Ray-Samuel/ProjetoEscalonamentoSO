#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_PROC 10
#define MIN_PROC 5

typedef struct proc_t {
    int pid;
    int burst;          // burst original 
    int arrival;        // tempo de chegada 
    int remaining;      // tempo restante
    struct proc_t *next;
} proc_t;

typedef struct {
    int size;
    int next_pid;
    proc_t *first;
} list_t;

void init_list(list_t **list) {
    list_t *aux = malloc(sizeof(*aux));
    if(aux == NULL) {
        printf("Allocation Error: init_list\n");
        sleep(2);
        return;
    }

    aux->first = NULL;
    aux->size = 0;
    aux->next_pid = 1;
    *list = aux;

    return;
}

void add_proc(list_t *list, proc_t proc) {

    proc_t *new_proc = malloc(sizeof(*new_proc));
    if(new_proc == NULL) {
        printf("Allocation Error: add_proc\n");
        sleep(2);
        return;
    }
    *new_proc = proc;
    new_proc->next = NULL;

    if(list->size == 0) {
        list->first = new_proc;
        list->size++;
        list->next_pid++;
        return;
    }

    proc_t *aux = list->first;
    while(aux->next != NULL) {
        aux = aux->next;
    }

    aux->next = new_proc;
    list->size++;
    list->next_pid++;
    return;
}

void remove_proc(list_t *list, proc_t *target) {
    if(list->first == target) {
        list->first = target->next;
    } else {
        proc_t *prev = list->first;
        while(prev != NULL && prev->next != target) {
            prev = prev->next;
        }
        if(prev == NULL) return;
        prev->next = target->next;
    }
    free(target);
    list->size--;
}

void run_srtf(list_t *list) {
    proc_t *running = NULL;

    for(int i = 0; list->size > 0; i++) {
        int same_proc = (running != NULL);

        for(proc_t *aux = list->first; aux != NULL; aux = aux->next) {

            if(aux->arrival <= i) {
                if(running == NULL || aux->remaining < running->remaining) {
                    running = aux;
                    same_proc = 0;

                } 
                else if(!same_proc
                    && aux->remaining == running->remaining
                    && aux->arrival < running->arrival) {
                        running = aux;
                    }
            }
        }

        if(running == NULL) {
            printf("%-10s %-10s %-10s %-10s %-10d\n", 
                "ocioso", "-", "-", "-", i);
            sleep(2);
            continue;
        }

        running->remaining--;
        printf("P%-9d %-10d %-10d %-10d %-10d\n",
        running->pid,
        running->arrival,
        running->burst,
        running->remaining,
        i);
        
        if(running->remaining == 0) {
            remove_proc(list, running);
            running = NULL;
        }

        sleep(2);
    }

    return ;
}

void menu(list_t *list) {
    int option;
    proc_t new_proc;

    do {
        system("clear");
        printf("\n========== Cadastro de processos ==========\n");
        printf("Quantidade atual de processos: %d\n", list->size);
        printf("Máximo de processos suportado: %d\n", MAX_PROC);
        printf("1: Adicionar novo processo\n");
        if(list->size >= MIN_PROC) {
            printf("2: Iniciar escalonamento\n");
        }
        printf("0: Sair\n> ");

        scanf("%d", &option);

        switch(option) {
            case 1:
            {
                if(list->size == MAX_PROC) {
                    printf("Lista cheia.\n");
                    sleep(2);
                    break;
                }

                system("clear");

                new_proc.pid = list->next_pid;
                printf("PID do processo: %d\n", new_proc.pid);

                do {

                    printf("Tempo de execução: ");
                    scanf("%d", &new_proc.burst);
                    
                    if(new_proc.burst <= 0) {
                        printf("\nTempo de execução inválido.\n");
                        sleep(2);
                    }

                } while(new_proc.burst <= 0);
                new_proc.remaining = new_proc.burst;

                do {

                    printf("Tempo de chegada: ");
                    scanf("%d", &new_proc.arrival);

                    if(new_proc.arrival < 0) {
                        printf("\nTempo de chegada inválido.\n");
                        sleep(2);
                    }

                } while(new_proc.arrival < 0);

                add_proc(list, new_proc);
                break;
            }    

            case 2: 
            {
                if(list->size >= MIN_PROC) {
                    system("clear");

                    printf("\n========== ESCALONAMENTO SRTF ==========\n\n");
                    printf("%-10s %-10s %-10s %-10s %-10s\n",
                            "Processo", "Chegada", "Burst",
                            "Restante", "Tempo");

                    run_srtf(list);
                    return;
                }
                printf("Cadastre pelo menos %d processos.\n", MIN_PROC);

                sleep(2);
                break;
            }
            case 0:
            {
                break;
            }

            default:
            {
                printf("Opção inválida\n");
                sleep(2);
            }
        }
    } while(option != 0);
}

int main() {
    list_t *list;

    init_list(&list);
    if(list == NULL) return 1;

    menu(list);

    while(list->size > 0) {
        remove_proc(list, list->first);
    }

    free(list);
    return 0;
}