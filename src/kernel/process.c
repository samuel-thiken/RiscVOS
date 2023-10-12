#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "process.h"
#include "context.h"
#include "timer.h"
#include "liste.h"
#include "tests/tests.h"
#include "message_file.h"
#include "userspace_apps.h"
#include "./traps/trap.h"
#include "riscv.h"

#include "assert.h"
#include "mem.h"

static void starter(int (*function)(void *), void *arg);
static int add_sleep(process_t *proc1, process_t *proc2);

int proc1();
int proc2();
int proc3();

// Table des processus
process_t *process_table[NBPROC];

// Processus élu
process_t *running_process = NULL;

// liste des processus activables
list_t *activable_process_list;

// liste des processus endormis
static list_t *sleeping_process_list;

// liste des processus mis en attente
static list_t *waiting_process_list;

// liste des processus zombis
static list_t *zombi_process_list;

// liste des processus finis
static list_t *finished_process_list;

int idle()
{
    ENABLE_SUPERVISOR_INTERRUPTS();
    printf("Début de idle\n");

    // const struct uapps *hello_app = find_app("hello");
    // start(hello_app->start, 512, 1, "hello", NULL);

    start_in_kernel(kernel_tests, 4000, 1, "kernel_tests", (int)NULL);
    // start_in_kernel(proc1, 512, 1, "proc1", (int)NULL);
    // start_in_kernel(proc2, 512, 1, "proc2", (int)NULL);
    // start_in_kernel(proc3, 512, 1, "proc3", (int)NULL);

    while (1)
    {
        __asm__ __volatile__("wfi");
    }
    return 0;
}

int proc1()
{
    printf("Début de proc1\n");
    for (int32_t i = 0; i < 5; i++)
    {
        printf("processus %s pid = %d\n", process_name(), getpid());
        wait_clock(2);
    }
    return 0;
}

int proc2()
{
    for (int32_t i = 0; i < 3; i++)
    {
        printf("processus %s pid = %d\n", process_name(), getpid());
        wait_clock(3);
    }
    return 0;
}

int proc3()
{
    for (int32_t i = 0; i < 2; i++)
    {
        printf("processus %s pid = %d\n", process_name(), getpid());
        wait_clock(5);
    }
    return 0;
}

void set_up_process()
{
    // Initialisation des listes de processus
    activable_process_list = init_liste();
    sleeping_process_list = init_liste();
    waiting_process_list = init_liste();
    zombi_process_list = init_liste();
    finished_process_list = init_liste();

    // Gestion du timer (interruptions)
    freq_call_func = init_liste();
    schedule(ordonnance, 1);

    // Initialisation du processus idle
    start_in_kernel(idle, 512, 1, "idle", (uint64_t)NULL);

    // Libération des listes
    free_liste(activable_process_list);
    free_liste(sleeping_process_list);
    free_liste(waiting_process_list);
    free_liste(zombi_process_list);
    free_liste(finished_process_list);
    free_liste(freq_call_func);
}

void ps()
{
    char *state = NULL;
    for (int i = 0; i < NBPROC; i++)
    {
        if (process_table[i] != NULL)
        {
            switch (process_table[i]->state)
            {
            case RUNNING:
                state = "running";
                break;
            case ACTIVABLE:
                state = "activable";
                break;
            case SLEEPING:
                state = "sleeping";
                break;
            case WAITING:
                state = "waiting";
                break;
            case ZOMBI:
                state = "zombi";
                break;
            case FINISHED:
                state = "finished";
                break;
            case KILLED:
                state = "killed";
                break;
            case BLOCKED_ON_MSG:
                state = "blocked on message";
                break;

            default:
                state = "";
                break;
            }
            printf("pid: %d - name: %s - state: %s\n", process_table[i]->pid, process_table[i]->name, state);
        }
    }
}

static int set_pid()
{
    for (int i = 0; i < NBPROC; i++)
    {
        if (process_table[i] == NULL)
            return i;
    }
    return -1;
}

int start_process(int (*pt_func)(void *), memory_tables_t mem_table, uint64_t *stack, uint64_t satp_val, uint64_t sstatus_val, unsigned long ssize, int prio, const char *name, void *arg, void (*starter_func)(int (*)(void *), void *))
{
    process_t *proc = mem_alloc(sizeof(process_t));
    assert(proc != NULL);

    // pid
    int pid = set_pid();
    if (pid == -1)
    {
        printf("Error : pid < 0");
        exit(1);
    }
    proc->pid = pid;

    // mem table
    proc->mem_tables = mem_table;

    // nom
    strcpy(proc->name, name);

    // état
    proc->state = ACTIVABLE;

    // prorité
    proc->prio = prio;

    // pile
    proc->stack = stack;
    proc->ssize = ssize;

    // contexte
    context_t ctx;
    if (starter_func) {
        // Kernel mode
        ctx = (context_t) {
            .sp = ((unsigned long) proc->stack) + ssize,
            .ra = (uint64_t)process_starter,
            .s0 = (int)(unsigned long) pt_func,
            .s1 = (unsigned long) arg,
            .satp = satp_val,
            .sstatus = sstatus_val,
            .sscratch = (uint64_t) get_frame(),
            .sepc = (uint64_t)starter_func
        };
    } else {
        // User mode
        ctx = (context_t) {
            .sp = ((unsigned long) proc->stack) + ssize,
            .ra = (uint64_t)process_starter,
            .s0 = (unsigned long) arg,
            .satp = satp_val,
            .sstatus = sstatus_val,
            .sscratch = (uint64_t) get_frame(),
            .sepc = (uint64_t)pt_func
        };
    }
    proc->context = ctx;

    // réveil
    proc->reveil = 0;

    // processus_fils
    proc->fils = init_liste();
    if (running_process != NULL)
    {
        add_queue(running_process->fils, proc);
        sort_process_list(running_process->fils, (void *)same_pid, proc, (void *)add_with_priority);
    }

    // processus père
    if (running_process != NULL)
        proc->pere = running_process;
    else
        proc->pere = NULL;

    // waitpid
    proc->waitpid = -2;

    // Ajout dans la table des processus
    process_table[pid] = proc;

    // Ajout du processus dans la liste des processus activables
    add_queue(activable_process_list, proc);
    sort_process_list(activable_process_list, (void *)same_pid, proc, (void *)add_with_priority);

    // On lance le processus s'il est prioritaire ou s'il n'y en a pas en cours
    if (running_process == NULL || prio > running_process->prio)
        ordonnance();

    return proc->pid;
}

int start_in_kernel(int (*pt_func)(void *), unsigned long ssize, int prio, const char *name, void *arg)
{
    uint64_t *stack = get_frame();//mem_alloc(sizeof(unsigned long) * ssize);
    stack[ssize - 1] = (unsigned long)exit_process;

    uint64_t sstatus_val = SSTATUS_SPP | SSTATUS_SIE; // Mode superviseur + enable interrupts

    return start_process(pt_func, KERNEL_TABLE_ADDR, stack, get_satp(KERNEL_TABLE_ADDR), sstatus_val, ssize, prio, name, arg, starter);
}

int start(int (*pt_func)(void *), unsigned long ssize, int prio, const char *name, void *arg)
{
    const struct uapps *app = find_app(name);

    // 2) Créer la table de 1G
    memory_tables_t table_1G = get_frame();

    // 2.1) Créer une table de 2M
    memory_tables_t table_2M = get_frame();

    // 2.2) Créer une table de 4K
    memory_tables_t table_4K = get_frame();

    // 3) Remplissage des tables
    // Code du process
    int size = app->end - app->start;
    int page_index = 0;
    int frame_size = 4096;
    while (size > 0)
    {
        void *code_frame = get_frame();
        memcpy(code_frame, pt_func + (page_index * frame_size), size < frame_size ? size : frame_size);

        size -= frame_size;

        table_4K[page_index] = (table_entry_t){
            .fields_merged = {
                .valid = 1,
                .execute = 1,
                .write = 1,
                .read = 1,
                .user = 1,
                .global = 0,
                .accessed = 0,
                .dirty = 0,
                .rsw = 0,
                .ppn = (uint64_t)(void *)code_frame >> 12}};

        page_index++;
    }
    // 2M[0] --> 4K
    table_2M[0] = (table_entry_t){
        .fields_merged = {
            .valid = 1,
            .execute = 0,
            .write = 0,
            .read = 0,
            .user = 1,
            .global = 0,
            .accessed = 0,
            .dirty = 0,
            .rsw = 0,
            .ppn = (uint64_t)(void *)table_4K >> 12}};
    // 1G[0] --> kernel (rwx = 000)
    table_1G[0] = (table_entry_t){
        .fields_merged = {
            .valid = 1,
            .execute = 1,
            .write = 1,
            .read = 1,
            .user = 0,
            .global = 0,
            .accessed = 0,
            .dirty = 0,
            .rsw = 0,
            .ppn = 0}};
    // 1G[1] --> 2M
    table_1G[1] = (table_entry_t){
        .fields_merged = {
            .valid = 1,
            .execute = 0,
            .write = 0,
            .read = 0,
            .user = 1,
            .global = 0,
            .accessed = 0,
            .dirty = 0,
            .rsw = 0,
            .ppn = (uint64_t)(void *)table_2M >> 12}};

    // 4) Création de la stack
    // 4.1) Créer une table de 2M
    memory_tables_t table_2M_stack = get_frame();

    // 4.2) Créer une table de 4K
    memory_tables_t table_4K_stack = get_frame();
    // 4.3) Allouer la stack
    int stack_size = ssize;
    int stack_frame_index = 0;
    while (stack_size > 0) {
        table_entry_t *stack = get_frame();

        stack_size -= frame_size;

        table_4K_stack[stack_frame_index] = (table_entry_t){
            .fields_merged = {
                .valid = 1,
                .execute = 1,
                .write = 1,
                .read = 1,
                .user = 1,
                .global = 0,
                .accessed = 0,
                .dirty = 0,
                .rsw = 0,
                .ppn = (uint64_t)(void *)stack >> 12}};

        stack_frame_index++;
    }

    // // 4.3) Allouer la stack
    table_2M_stack[0] = (table_entry_t){
        .fields_merged = {
            .valid = 1,
            .execute = 0,
            .write = 0,
            .read = 0,
            .user = 1,
            .global = 0,
            .accessed = 0,
            .dirty = 0,
            .rsw = 0,
            .ppn = (uint64_t)(void *)table_4K_stack >> 12}};

    table_1G[9] = (table_entry_t){
        .fields_merged = {
            .valid = 1,
            .execute = 0,
            .write = 0,
            .read = 0,
            .user = 1,
            .global = 0,
            .accessed = 0,
            .dirty = 0,
            .rsw = 0,
            .ppn = (uint64_t)(void *)table_2M_stack >> 12}};

    uint64_t *stack_addr = (uint64_t *)(9UL << 30);

    uint64_t sstatus_val = 0 | SSTATUS_SUM; // Mode user + Pages user accessible en superviseur

    void *pt_func_vir = (void *)(1 << 30);

    return start_process(pt_func_vir, table_1G, stack_addr, get_satp(table_1G), sstatus_val, ssize, prio, name, arg, NULL);
}

static void starter(int (*function)(void *), void *arg)
{
    int retval = (*function)(arg);
    // TODO PAS SUR : remplacer par un ecall pour repasser en mode superviseur
    exit_process(retval);
}

int getpid(void)
{
    return running_process->pid;
}

char *process_name(void)
{
    return running_process->name;
}

process_t *get_process_elu()
{
    return running_process;
}

void set_process_elu(process_t *proc)
{
    running_process = proc;
}

static int add_sleep(process_t *proc1, process_t *proc2)
{
    return (int)(proc1->reveil < proc2->reveil);
}

int add_with_priority(process_t *proc1, process_t *proc2)
{
    return (int)(proc1->prio > proc2->prio);
}

int same_pid(process_t *proc1, process_t *proc2)
{
    return (int)(((process_t *)proc1)->pid == ((process_t *)proc2)->pid);
}

int getprio(int pid)
{
    int prio = process_table[pid]->prio;
    if (prio >= 1 && prio <= MAXPRIO)
        return process_table[pid]->prio;
    else
        return -1;
}

int chprio(int pid, int newprio)
{
    uint32_t old_prio;

    if (process_table[pid] == NULL || pid == 0)
        return -1;
    if (newprio > MAXPRIO || newprio <= 0)
        return -1;
    if (process_table[pid]->state == ZOMBI)
        return -1;

    old_prio = process_table[pid]->prio;
    process_table[pid]->prio = newprio;

    // On trie la liste des fils du processus père en fonction de la nouvelle priorité
    sort_process_list(process_table[pid]->pere->fils, (void *)same_pid, process_table[pid], (void *)add_with_priority);

    if (process_table[pid]->state == ACTIVABLE)
        sort_process_list(activable_process_list, (void *)same_pid, process_table[pid], (void *)add_with_priority);
    if (process_table[pid]->state == BLOCKED_ON_MSG)
    {
        sort_process_list(process_table[pid]->msg_file, (void *)same_pid, process_table[pid], (void *)add_with_priority);
    }
    if (running_process->prio <= newprio)
        ordonnance();
    return old_prio;
}

unsigned long nbr_secondes(void)
{
    return current_clock();
}

void wake_up_process(list_t *list)
{
    // On parcours la liste et on regarde si on ne peut pas réveiller des processus
    process_t *next;
    while (list->head != NULL && ((process_t *)(list->head->val))->state == ACTIVABLE)
    {
        next = (process_t *)extract_head(list);
        add_queue(activable_process_list, next);
        sort_process_list(activable_process_list, (void *)same_pid, next, (void *)add_with_priority);
    }
}

static void end_process(process_t *proc)
{
    if (proc->state == FINISHED || proc->state == KILLED)
    {
        add_queue(finished_process_list, proc);
        process_table[proc->pid] = NULL;
        if (proc->pere != NULL)
            extract_pid(proc->pere->fils, (void *)same_pid, proc);

        // tous les fils de ce processus n'ont plus de père
        cellule_t *cell = proc->fils->head;
        while (cell != NULL)
        {
            ((process_t *)(cell->val))->pere = NULL;
            if (((process_t *)(cell->val))->state == ZOMBI)
            {
                ((process_t *)(cell->val))->state = FINISHED;
                end_process((process_t *)(cell->val));
            }
            cell = cell->next;
        }

        proc->pere = NULL;
    }
}

void ordonnance()
{
    unsigned long time = nbr_secondes();

    // On parcours la liste des endormis et on regarde si on ne peut pas en réveiller
    cellule_t *next = sleeping_process_list->head;
    while (next != NULL && ((process_t *)(next->val))->reveil <= time)
    {
        ((process_t *)(next->val))->state = ACTIVABLE;
        next = next->next;
    }

    wake_up_process(sleeping_process_list);
    wake_up_process(waiting_process_list);

    // On détruit les processus terminés
    // process_t *buffer;
    // unsigned long stack_size;
    // while (finished_process_list->head != NULL)
    // {
    //     buffer = extract_head(finished_process_list);
    //     stack_size = buffer->ssize * sizeof(unsigned long);
    //     mem_free(buffer->stack, stack_size);
    //     mem_free(buffer, sizeof(process_t));
    // }

    // On enlève le processus actif de l'état ELU
    process_t *old_proc = running_process;
    if (old_proc != NULL)
    {
        if (old_proc->state == SLEEPING)
        {
            add_queue(sleeping_process_list, old_proc);
            sort_process_list(sleeping_process_list, (void *)same_pid, old_proc, (void *)add_sleep);
        }
        else if (old_proc->state == WAITING)
        {
            add_queue(waiting_process_list, old_proc);
            sort_process_list(waiting_process_list, (void *)same_pid, old_proc, (void *)add_with_priority);
        }
        else if (old_proc->state == FINISHED || old_proc->state == KILLED)
        {
            end_process(old_proc);
        }
        else if (old_proc->state == ZOMBI)
            add_queue(zombi_process_list, old_proc);
        else if (old_proc->state == BLOCKED_ON_MSG)
        {
        }
        else
        {
            old_proc->state = ACTIVABLE;
            add_queue(activable_process_list, old_proc);
            sort_process_list(activable_process_list, (void *)same_pid, old_proc, (void *)add_with_priority);
        }
    }
    // On met le nouvel état en tête de la liste des activables à ELU et on le place dans le running_process
    if (activable_process_list->head != NULL)
        running_process = (process_t *)extract_head(activable_process_list);
    else
        running_process = NULL;
    if (running_process != NULL)
        running_process->state = RUNNING;

    // sauvegarde et restauration des processus
    if (old_proc == running_process)
        return;
    if (old_proc != NULL && running_process != NULL)
    {
        context_switch(&(old_proc->context), &(running_process->context));
    }
    else if (running_process != NULL)
    {
        context_switch(NULL, &(running_process->context));
    }
}

/**
 * @brief Passe le processus dans l'état endormi jusqu'à ce que le nombre d'interruptions
 * horloge passé en paramètre soit atteint ou dépassé.
 *
 * @param clock
 */
void wait_clock(unsigned long clock)
{
    running_process->reveil = nbr_secondes() + clock;
    running_process->state = SLEEPING;

    ordonnance();
}

void exit_process(int retval)
{
    // on stocke retval dans la structure du processus et on réveille le père s'il est en attente
    if (running_process->pere == NULL)
        running_process->state = FINISHED;
    else if (running_process->pere->state == WAITING)
    {
        running_process->state = FINISHED;
        running_process->pere->state = ACTIVABLE;
    }
    else
        running_process->state = ZOMBI;

    running_process->retval = retval;
    running_process->pere->waitpid = running_process->pid;

    ordonnance();
    while (1)
    {
    }
}

int kill(int pid)
{
    process_t *proc = NULL;
    if (process_table[pid]->state == RUNNING && pid != 0)
        proc = running_process;
    else if (process_table[pid]->state == ACTIVABLE && pid != 0)
        proc = (process_t *)extract_pid(activable_process_list, (void *)same_pid, process_table[pid]);
    else if (process_table[pid]->state == SLEEPING)
        proc = (process_t *)extract_pid(sleeping_process_list, (void *)same_pid, process_table[pid]);
    else if (process_table[pid]->state == BLOCKED_ON_MSG)
        proc = (process_t *)extract_pid(process_table[pid]->msg_file, (void *)same_pid, process_table[pid]);

    if (proc != NULL)
    {
        // Si le processus est le processus élu, on appelle ordonnance()
        process_state state = proc->state;

        // on stocke retval dans la structure du processus père et on réveille le père s'il est en attente
        if (proc->pere == NULL)
        {
            proc->state = KILLED;
            if (state != RUNNING)
                end_process(proc);
        }
        else if (proc->pere->state == WAITING && (proc->pere->waitpid == pid || proc->pere->waitpid == -1))
        {
            proc->state = KILLED;
            proc->pere->state = ACTIVABLE;
            proc->pere->waitpid = proc->pid;
            end_process(proc);
        }
        else
        {
            proc->state = ZOMBI;
            add_queue(zombi_process_list, proc);
            proc->pere->waitpid = proc->pid;
        }

        proc->retval = 0;

        if (state == RUNNING)
            ordonnance();

        return 0;
    }

    return -1;
}

int waitpid(int pid, int *retvalp)
{
    if (pid >= 0)
    {
        cellule_t *cell = running_process->fils->head;
        while (cell != NULL && ((process_t *)(cell->val))->pid != pid)
        {
            cell = cell->next;
        }

        // Cas où le processus ayant le pid pid n'est pas un fils d'un processus élu
        if (cell == NULL)
            return -1;

        process_t *proc_fils = (process_t *)cell->val;

        if (proc_fils->state == ZOMBI)
        {
            if (retvalp != NULL)
                *retvalp = proc_fils->retval;
            proc_fils->state = FINISHED;
            end_process(proc_fils);
        }
        else if (proc_fils->state == FINISHED)
            return -1;
        else
        {
            running_process->state = WAITING;
            running_process->waitpid = pid;
            ordonnance();
            if (retvalp != NULL)
                *retvalp = proc_fils->retval;
        }

        return pid;
    }
    else
    {
        cellule_t *cell = running_process->fils->head;
        if (cell == NULL)
            return -1;
        while (cell != NULL)
        {
            if (((process_t *)(cell->val))->state == ZOMBI)
                break;
            cell = cell->next;
        }

        if (cell != NULL)
        {
            process_t *proc_fils = (process_t *)cell->val;
            if (retvalp != NULL)
                *retvalp = proc_fils->retval;
            running_process->waitpid = proc_fils->pid;
            proc_fils->state = FINISHED;
            end_process(proc_fils);
        }
        else
        {
            running_process->state = WAITING;
            running_process->waitpid = -1;
            ordonnance();
            pid = running_process->waitpid;
            if (retvalp != NULL)
            {
                cell = running_process->fils->head;
                while (cell != NULL && ((process_t *)(cell->val))->pid != pid)
                    cell = cell->next;
                if (cell != NULL)
                    *retvalp = ((process_t *)(cell->val))->retval;
            }
        }

        return running_process->waitpid;
    }
}
