#include <stdio.h>
#include <stdlib.h>

#define MAX_PROCS 5

typedef struct proc_t {
    int data;
    int priority;
    struct proc_t* next;
    int blacklist;
} proc_t;

typedef struct {
    proc_t* first;
    proc_t* last;
    int queue_len;
} proc_queue_t;

void enqueue_proc(proc_queue_t *proc_queue, proc_t proc)
{
    proc_t* new_proc = malloc(sizeof(proc_t));

    if(new_proc == NULL)
    {
        printf("Allocation Error: new_proc\n");
        return;
    }

    new_proc->data = proc.data;
    new_proc->priority = proc.priority;
    new_proc->blacklist = proc.blacklist;
    new_proc->next = NULL;

    if (proc_queue->first == NULL) {
        proc_queue->first = new_proc;
        proc_queue->last = new_proc;
    } else {
        proc_queue->last->next = new_proc;
        proc_queue->last = new_proc;
    }
    
    proc_queue->queue_len++;
}

void dequeue_proc(proc_queue_t *proc_queue)
{
    if(proc_queue == NULL || proc_queue->first == NULL)
    {
        printf("The queue is empty!\n");
        return;
    }

    proc_t* tmp = proc_queue->first->next;
    free(proc_queue->first);
    proc_queue->first = tmp;

    if(proc_queue->first == NULL)
    {
        proc_queue->last = NULL;
    }
    
    proc_queue->queue_len--;
}

proc_t get_proc_hg_priority(proc_t **proc_list, int size_list)
{
    int target = -1;

    for(int i = 0; i < size_list; i++)
    {
        if((*proc_list)[i].blacklist == 0)
        {
            if(target == -1 || (*proc_list)[i].priority < (*proc_list)[target].priority)
            {
                target = i;
            }
        }
    }

    if(target != -1) {
        (*proc_list)[target].blacklist = 1;
        return (*proc_list)[target];
    }
    
    proc_t empty_proc = {0, 0, NULL, 1};
    return empty_proc;
}

void run_scheduler(proc_queue_t *proc_queue, proc_t **proc_list, int size_list)
{
    for(int i = 0; i < size_list; i++)
    {
        proc_t high_prior_proc = get_proc_hg_priority(proc_list, size_list);
        enqueue_proc(proc_queue, high_prior_proc);
    }
}

int main()
{
    proc_t proc_list[MAX_PROCS];

    proc_queue_t proc_queue;
    proc_queue.first = NULL;
    proc_queue.last = NULL;
    proc_queue.queue_len = 0;

    proc_t proc1;
    proc1.data = 0x2B;
    proc1.priority = 2;
    proc1.next = NULL;
    proc1.blacklist = 0;

    proc_t proc2;
    proc2.data = 0x1C;
    proc2.priority = 0;
    proc2.next = NULL;
    proc2.blacklist = 0;

    proc_t proc3;
    proc3.data = 0x32;
    proc3.priority = 1;
    proc3.next = NULL;
    proc3.blacklist = 0;

    proc_list[0] = proc1;
    proc_list[1] = proc2;
    proc_list[2] = proc3;

    proc_t *p_list = proc_list;

    run_scheduler(&proc_queue, &p_list, 3);

    proc_t *current = proc_queue.first;
    while(current != NULL) {
        printf("Processo Data: 0x%X | Prioridade: %d\n", current->data, current->priority);
        current = current->next;
    }

    while(proc_queue.first != NULL) {
        dequeue_proc(&proc_queue);
    }

    return 0;
}
